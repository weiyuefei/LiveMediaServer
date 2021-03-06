
#ifndef __HTTPCLIENTSESSION_H__
#define __HTTPCLIENTSESSION_H__

#include <sys/time.h>

#include "BaseServer/OSHeaders.h"
#include "BaseServer/Task.h"
#include "BaseServer/ClientSocket.h"
#include "BaseServer/TimeoutTask.h"

#include "ts2flv/ts2flv.h"

#include "deque.h"
#include "channel.h"
#include "HTTPClient.h"
#include "M3U8Parser.h"

#define MAX_TRY_COUNT	3

typedef struct segment_download_t
{
	//SEGMENT_T* segp;
	// for download
	int32_t		get_index;
	int32_t 	download_count;
	//u_int64_t	content_length;
	u_int64_t	range_start;
	//u_int64_t	receive_length;
} SEGMENT_DOWNLOAD_T;


class HTTPClientSession : public Task
{
	public:
			HTTPClientSession(CHANNEL_T* channelp, char* type);
	virtual	~HTTPClientSession();
	virtual     SInt64      Run();
		int		Start();
		int		Stop();
		char*	GetM3U8Path() { return fM3U8Path; }
		char*	GetSourceHost() { return fHost; }	
		MEMORY_T*	GetMemory() { return fMemory; }	
		int		UpdateSources(DEQUE_NODE* source_list);
		struct timeval*	GetBeginTime() { return &fBeginTime; } 

	protected:
		int 	MakeUrlM3U8();
		int		TryStop();
		int		TrySwitchSources();
		Bool16	IsDownloaded(SEGMENT_T* segp);
		Bool16	ConnectionTimeout();
		Bool16	DownloadTimeout();
		int		SetSources(DEQUE_NODE* source_list);
		int 	Log(char* url, char* datap, UInt32 len);	
		int 	Write(StrPtrLen& file_name, char* datap, UInt32 len);
		int 	RewriteM3U8(M3U8Parser* parserp);	
		int 	MemoM3U8(M3U8Parser* parserp, time_t begin_time, time_t end_time);
		int 	MemoSegment(SEGMENT_T* segp, UInt64 range_start, UInt64 content_length, char* datap, UInt32 len, time_t begin_time, time_t end_time);			
		int 	MoveSegment();
		time_t	CalcBreakTime();
		int 	GetSegmentInit();

		//
        // States. Find out what the object is currently doing
        enum
        {
            kSendingGetM3U8     = 0,
            kSendingGetSegment  = 1,
            kDone               = 2
        };
        //
        // Why did this session die?
        enum
        {
            kDiedNormally       = 0,    // Session went fine
            kTeardownFailed     = 1,    // Teardown failed, but session stats are all valid
            kRequestFailed      = 2,    // Session couldn't be setup because the server returned an error
            kBadSDP             = 3,    // Server sent back some bad SDP
            kSessionTimedout    = 4,    // Server not responding
            kConnectionFailed   = 5,    // Couldn't connect at all.
            kDiedWhilePlaying   = 6     // Connection was forceably closed while playing the movie
        };

	
		int		SwitchSource(OS_Error theErr);
		int		SetSource(SOURCE_T* sourcep);
		int		RateSourceM3U8(time_t newest_time, u_int64_t download_byte, struct timeval download_begin_time, struct timeval download_end_time);
		int		RateSourceSegment(u_int64_t download_byte, struct timeval download_begin_time, struct timeval download_end_time);
		int		SwitchLog(struct timeval until);
		
		char				fHost[MAX_HOST_LEN];
		TCPClientSocket* 	fSocket;
		HTTPClient*			fClient;
		char				fLiveType[MAX_LIVE_TYPE];
		char 				fM3U8Path[MAX_URL_LEN];
		char 				fUrlM3U8[MAX_URL_LEN];
		char 				fUrlSegment[MAX_URL_LEN];
		CHANNEL_T*			fChannel;				
		MEMORY_T*			fMemory;
		MEMORY_T*			fMemory2;
		int					fSourceNum;
		DEQUE_NODE*			fSourceList;
		DEQUE_NODE*			fSourceNow;
		
		UInt32          	fState;     // the state machine
		UInt32          	fDeathReason;

		TimeoutTask     	fTimeoutTask; // Kills this connection in the event the server isn't responding

		M3U8Parser		   	fM3U8Parser;  
		struct timeval		fM3U8BeginTime;
		struct timeval		fM3U8EndTime;
		struct timeval		fSegmentBeginTime;
		struct timeval		fSegmentEndTime;
		int					fGetIndex;
		SEGMENT_DOWNLOAD_T	fGetSegment;

		char				fLogFile[PATH_MAX];
		FILE*				fLog;
		struct timezone		fTimeZone;
		struct timeval		fBeginTime;
        struct timeval		fEndTime;
        struct timeval		fLogTime;

		// from cmd
		Bool16				fWillStop;
		Bool16				fWillUpdateSources;
		DEQUE_NODE*			fWillSourceList;

		//sequence, or chunk id
		u_int64_t			fLastChunkId;
		Bool16				fWithChunkId;

		TS2FLV				fTs2flv;
		
};

#endif
