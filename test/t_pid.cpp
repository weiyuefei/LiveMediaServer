#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/time.h>

pthread_t tid[10];

void * thread_handler(void *arg)
{
    printf("tid: %ld, pthreas_self: %lu, pid: %lu\n", 
    	syscall(SYS_gettid), 
    	(unsigned long)pthread_self(), 
        (unsigned long)getpid());
    while(1)
    {
        sleep(1);
    }
    return NULL;
}

int main(void)
{
	struct timeval now1 = {};
	struct timezone tz1 = {0, 0};
	gettimeofday(&now1, &tz1);
	printf("now1: %ld, tz: %d, %d\n", now1.tv_sec, tz1.tz_minuteswest, tz1.tz_dsttime);
	struct timeval now2 = {};
	struct timezone tz2 = {0, 0};
	gettimeofday(&now2, NULL);
	printf("now2: %ld\n", now2.tv_sec);
	
	
    int i, ret;
    pid_t pid;
    printf("main tid:%u,pid:%u\n", (unsigned)pthread_self(),
        (unsigned)getpid());
    for(i = 0; i < 10; i++){
        if((ret = pthread_create(&tid[i], NULL, thread_handler, 
            (void *)i)) != 0){
            fprintf(stderr, "pthread_create:%s\n",
                strerror(ret));
            exit(1);
        }
    }
    sleep(3);
    pid = fork();
    if(pid == 0){
        printf("son tid:%u,pid:%u\n", (unsigned)pthread_self(),
            (unsigned)getpid());
        while(1);
            sleep(1);
    }
    while(1)
        sleep(2);
    exit(0);
}

