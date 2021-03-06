
#ifndef _CHANNEL_H__
#define _CHANNEL_H__

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "common.h"
#include "deque.h"

#define HASH_LEN 			40
#define MAX_LIVE_ID     	44
#define MAX_CHANNEL_NAME	64
#define MAX_LIVE_TYPE     	4

#define MAX_M3U8_NUM		2
#define MAX_URL_LEN			256

#define MAX_M3U8_CONTENT_LEN (MAX_SEGMENT_NUM*1024)

#define LIVE_TS				"ts"
#define LIVE_FLV			"flv"
#define LIVE_MP4			"mp4"


typedef struct data_t
{
	void*		datap;	
	u_int64_t 	size;	// total size
	int64_t 	len;	// used length	
} DATA_T;

typedef struct clip_t
{
	//#EXTINF:10,
	u_int32_t 	inf;
	//#EXT-X-BYTERANGE:1095852
	u_int64_t	byte_range;
	// 565631
	u_int64_t	sequence;
	char 		relative_url[MAX_URL_LEN];	
	char 		m3u8_relative_url[MAX_URL_LEN];
	char 		file_name[MAX_URL_LEN];
	DATA_T		data;
	time_t		begin_time;
	// if end_time == 0, download undone.
	time_t		end_time;	
} CLIP_T;

//typedef DATA_T M3U8_T;
typedef struct m3u8_t
{	
	DATA_T		data;
	time_t		begin_time;
	time_t		end_time;
} M3U8_T;

typedef struct memory_t
{
	int 		target_duration;
	int 		m3u8_index;
	int 		m3u8_num;
	M3U8_T		m3u8s[MAX_M3U8_NUM];	
	int			clip_index;
	int			clip_num;
	//CLIP_T		clips[MAX_CLIP_NUM];
	CLIP_T*		clips;
} MEMORY_T;

typedef struct download_rate_t
{
	time_t			m3u8_newest_time;
	struct timeval 	m3u8_begin_time;
	struct timeval 	m3u8_end_time;
	u_int16_t		m3u8_bytes;
	struct timeval 	segment_begin_time;
	struct timeval 	segment_end_time;
	u_int16_t		segment_bytes;
} DOWNLOAD_RATE_T;

typedef struct source_t
{
	u_int32_t	ip;
	u_int16_t	port;
	DOWNLOAD_RATE_T download_rate;
} SOURCE_T;

typedef struct statistics_t
{
	// session_num ++
	// session_num --
	// when?
	u_int64_t	session_num;	
	u_int64_t	download_bytes;
} STATISTICS_T;

class HTTPClientSession;
//{"channel_id":"21","LOWER(t.liveid)":"0b49884b3b85f7ccddbe4e96e4ae2eae7a6dec56","bitrate":"800","channel_name":"\u4e1c\u65b9\u536b\u89c6"},
typedef struct channel_t
{
	int 	channel_id;	
	char 	liveid[MAX_LIVE_ID];
	int 	bitrate;
	char	channel_name[MAX_CHANNEL_NAME];
	int	  	codec_ts; 	// 1: on, 0: off
	int		codec_flv;	// 1: on, 0: off
	int		codec_mp4;	// 1: on, 0: off
	DEQUE_NODE* source_list;
	// HTTPClientSession
	HTTPClientSession* 	sessionp_ts;
	HTTPClientSession* 	sessionp_flv;
	HTTPClientSession* 	sessionp_mp4;
	// memory
	MEMORY_T* 	memoryp_ts;
	MEMORY_T* 	memoryp_flv;
	MEMORY_T* 	memoryp_mp4;
	// statistics
	STATISTICS_T	statistics_ts;
	STATISTICS_T	statistics_flv;
	STATISTICS_T	statistics_mp4;
} CHANNEL_T;

void source_release(void* datap);
void channel_release(void* datap);
DEQUE_NODE* channel_find_source(CHANNEL_T* channelp, u_int32_t ip, u_int16_t port);
int channel_add_source(CHANNEL_T* channelp, u_int32_t ip, u_int16_t port);
int start_channel(CHANNEL_T* channelp);
int stop_channel(CHANNEL_T* channelp);



class ChannelList
{
	public:
		ChannelList();
		~ChannelList();

		int			ReadConfig(char* config_file);
		int			WriteConfig(char* config_file);

		CHANNEL_T* 	FindChannelById(int channel_id);
		CHANNEL_T* 	FindChannelByHash(char* liveid);
		int			AddChannel(CHANNEL_T* channelp);
		int			DeleteChannel(char* liveid);
		DEQUE_NODE* GetChannels() { return m_channel_list; };
		int			GetNum() { return deque_num(m_channel_list); };
		
	protected:
		DEQUE_NODE* ParseChannels(xmlDocPtr doc, xmlNodePtr cur);
		int			ParseChannel(xmlDocPtr doc, xmlNodePtr cur, CHANNEL_T* channelp);
		DEQUE_NODE* ParseSources(xmlDocPtr doc, xmlNodePtr cur);
		int			ParseSource(xmlDocPtr doc, xmlNodePtr cur, SOURCE_T* sourcep);

		DEQUE_NODE*	m_channel_list;		
};

extern ChannelList g_channels;

#endif
