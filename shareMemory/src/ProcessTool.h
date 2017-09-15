#ifndef _PROCESS_TOOL_
#define _PROCESS_TOOL_

/************************************************* 
Copyright:sd 
Author: sdguet
Date:2017-09-14 
Platfrom: win & linux
Description: process communication tools,use for  multiprocess exchanging messages.
             1. shared memory
**************************************************/  

#include <iostream>
#include <string.h>
using namespace std;
#ifdef _WIN32
#include <Windows.h>
#include <assert.h>
#endif

#ifdef __linux
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  //getpagesize(  )  
#include <sys/ipc.h>  
#include <sys/shm.h>  
#include <fcntl.h> 
#include <sys/types.h>
#include <sys/stat.h> 
#include <sys/sem.h>
#endif

#define MAX_MSG_LENGTH 1024*102

 #define MY_SHM_ID 67483  

typedef struct{
	long contentsize;
	char content[MAX_MSG_LENGTH];
}MsgHead;

#ifdef __linux
union semun  
{  
	int val;  
	struct semid_ds *buf;  
	unsigned short *arry;  
};  
#endif

class ProcessTool{
public:
	explicit ProcessTool();
	~ProcessTool();
	//one side chName 
	//shoud be number string:like "12345"
	void* createChannel(char* chName, int *result, long size = MAX_MSG_LENGTH);
	//other side
	//shoud be number string:like "12345"
	void* openChannel(char* chName, int *result);

	void closeChannel();
	//get memory pointer
	void* getShareMemory();
	//block recv 
	void setBlock(bool bblock = true);
	//send msg content
	int sendBuf(MsgHead *buffer);
	//recv msg content
	int recvBuf(MsgHead *buffer);
private:
	int set_semvalue();  
	void del_semvalue();  
	int semaphore_p();  
	int semaphore_v();  
private:
	void* m_pSharedMemory;
	void* m_bufferMutex;
	bool m_bBlock;
	void* m_fileMapping;
	int m_semId;  
};

#endif //_PROCESS_TOOL_
