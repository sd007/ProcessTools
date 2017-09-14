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
using namespace std;
#ifdef _WIN32
#include <Windows.h>
#include <assert.h>
#endif

#define MAX_MSG_LENGTH 1024*102

typedef struct{
	long contentsize;
	char content[MAX_MSG_LENGTH];
}MsgHead;

class ProcessTool{
public:
	explicit ProcessTool();
	~ProcessTool();
	//one side
	void* createChannel(char* chName, int *result, long size = MAX_MSG_LENGTH);
	//other side
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
	void* m_pSharedMemory;
	void* m_bufferMutex;
	bool m_bBlock;
	void* m_fileMapping;
};

#endif //_PROCESS_TOOL_
