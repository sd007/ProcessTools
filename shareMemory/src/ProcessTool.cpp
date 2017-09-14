#include "ProcessTool.h"

ProcessTool::ProcessTool():m_pSharedMemory(NULL)
	,m_bBlock(false)
	,m_fileMapping(NULL)
{

}

ProcessTool::~ProcessTool()
{
	closeChannel();
}


void* ProcessTool::createChannel( char* chName, int *result, long size /*= 1024*1024*/)
{
		WCHAR wszName[256];  
		memset(wszName,0,sizeof(wszName));  
		MultiByteToWideChar(CP_ACP,0,chName,strlen(chName)+1,wszName,  sizeof(wszName)/sizeof(wszName[0])); 

		//create share memory
		m_fileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0 , size, wszName);
		if (m_fileMapping == NULL)
		{
			*result = -1;
			cout<<"createChannel::CreateFileMapping error!"<<endl;
			return NULL;
		}
		m_pSharedMemory = MapViewOfFile(m_fileMapping, FILE_MAP_ALL_ACCESS, 0 , 0, 0);
		if (NULL == m_pSharedMemory)
		{
			*result = -2;
			cout<<"createChannel::MapViewOfFile error!"<<endl;
			return NULL;
		}
		m_bufferMutex = CreateMutex(NULL, false, (LPCWSTR)"SM_Mutex");
		if (NULL == m_bufferMutex)
		{
			*result = -3;
			cout<<"CreateMutex error!"<<endl;
			return NULL;
		}
		return m_pSharedMemory;
}

void* ProcessTool::openChannel( char* chName, int *result )
{
	WCHAR wszName[256];  
	memset(wszName,0,sizeof(wszName));  
	MultiByteToWideChar(CP_ACP,0,chName,strlen(chName)+1,wszName,  sizeof(wszName)/sizeof(wszName[0])); 
	m_fileMapping  = OpenFileMapping(FILE_MAP_ALL_ACCESS, false, wszName);
	if (NULL == m_fileMapping)
	{
		*result = -1;
		cout<<"OpenChannel::OpenFileMapping error!"<<endl;
		return NULL;
	}
	m_pSharedMemory = MapViewOfFile(m_fileMapping, FILE_MAP_ALL_ACCESS, 0 , 0, 0);
	if (NULL == m_pSharedMemory)
	{
		*result = -2;
		cout<<"OpenChannel::MapViewOfFile error!"<<endl;
		return NULL;
	}
	m_bufferMutex = OpenMutex(MUTEX_ALL_ACCESS, false, (LPCWSTR)"SM_Mutex");
	if (NULL == m_bufferMutex)
	{
		*result = -3;
		cout<<"OpenMutex error!"<<endl;
		return NULL;
	}
	return m_pSharedMemory;
}

void* ProcessTool::getShareMemory()
{
	return m_pSharedMemory;
}

int ProcessTool::sendBuf( MsgHead *buffer )
{
	if (buffer && m_pSharedMemory)
	{
		if (m_bBlock)
		{
			WaitForSingleObject(m_bufferMutex, INFINITE); 
		}else{
			if (WaitForSingleObject(m_bufferMutex, 5000) != WAIT_OBJECT_0)
			{
				return 1;
			}
		}
		memcpy(m_pSharedMemory, (char*)buffer, MAX_MSG_LENGTH);
		ReleaseMutex(m_bufferMutex);  
		return 0;
	}
	return -1;
}

int ProcessTool::recvBuf( MsgHead *buffer )
{
	if (buffer && m_pSharedMemory)
	{
		if (m_bBlock)
		{
			WaitForSingleObject(m_bufferMutex, INFINITE); 
		}else{
			if (WaitForSingleObject(m_bufferMutex, 5000) != WAIT_OBJECT_0)
			{
				return 1;
			}
		} 
		memcpy((char*)buffer, m_pSharedMemory, MAX_MSG_LENGTH);
		ReleaseMutex(m_bufferMutex);  
		return 0;
	}
	return -1;
}

void ProcessTool::setBlock( bool bblock /*= true*/ )
{
	m_bBlock = bblock;
}

void ProcessTool::closeChannel()
{
	if (m_fileMapping)
	{
		CloseHandle(m_fileMapping);
	}
	if (m_pSharedMemory)
	{
		UnmapViewOfFile(m_pSharedMemory);
	}
	if (m_bufferMutex)
	{
		CloseHandle(m_bufferMutex); 
	} 
}
