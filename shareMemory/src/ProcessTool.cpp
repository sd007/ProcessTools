#include "ProcessTool.h"

ProcessTool::ProcessTool():m_pSharedMemory(NULL)
	,m_bBlock(false)
	,m_fileMapping(NULL)
	,m_semId(0)
{

}

ProcessTool::~ProcessTool()
{
	closeChannel();
}


void* ProcessTool::createChannel( char* chName, int *result, long size /*= 1024*1024*/)
{
#ifdef _WIN32
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
#else
		//create shared memory 
		shmid=shmget(key_t)std::atoi( chName.c_str()), sizeof(MsgHead), IPC_CREAT|0666  );  
		if( shmid == -1 )  
		{  
			*result = -1;
			cout<<"createChannel::shmget error!"<<endl;
			return NULL; 
		}
		//attach mem to current process
		m_pSharedMemory = shmat( shmid, ( const void* )0, 0);  
		if (NULL == m_pSharedMemory)
		{
			*result = -2;
			cout<<"createChannel::shmat error!"<<endl;
			return NULL;
		}
		//创建信号量  
		m_semId = semget((key_t)std::atoi( chName.c_str()), 1, 0666 | IPC_CREAT); 
		if(!set_semvalue())  
		{  
			*result = -3;
			cout<<"Failed to initialize semaphore"<<endl;  
			return NULL;  
		}  
#endif
		*result = 1;
		return m_pSharedMemory;
}

void* ProcessTool::openChannel( char* chName, int *result )
{
#ifdef _WIN32
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
#else
	//create shared memory 
	shmid=shmget(key_t)std::atoi( chName.c_str()), sizeof(MsgHead), IPC_CREAT|0666  );  
	if( shmid == -1 )  
	{  
		*result = -1;
		cout<<"createChannel::shmget error!"<<endl;
		return NULL; 
	}
	//attach mem to current process
	m_pSharedMemory = shmat( shmid, ( const void* )0, 0);  
	if (NULL == m_pSharedMemory)
	{
		*result = -2;
		cout<<"createChannel::shmat error!"<<endl;
		return NULL;
	}
	//创建信号量  
	m_semId = semget((key_t)std::atoi( chName.c_str()), 1, 0666 | IPC_CREAT); 
	if(!set_semvalue())  
	{  
		*result = -3;
		cout<<"Failed to initialize semaphore"<<endl;  
		return NULL;  
	}  
#endif
	*result = 1;
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
#ifdef _WIN32
		if (m_bBlock)
		{
			WaitForSingleObject(m_bufferMutex, INFINITE); 
		}else{
			if (WaitForSingleObject(m_bufferMutex, 5000) != WAIT_OBJECT_0)
			{
				return 1;
			}
		}
#else
		if(!semaphore_p()) 
		{
			cout<<"sendBuf:: semaphore_p error!"<<endl;
			return 1; 
		}
#endif
		memcpy(m_pSharedMemory, (char*)buffer, MAX_MSG_LENGTH);
#ifdef _WIN32
		ReleaseMutex(m_bufferMutex);  
#else
		if(!semaphore_v())  
		{
			cout<<"sendBuf:: semaphore_v error!"<<endl;
			return 1; 
		}
#endif
		return 0;
	}
	return -1;
}

int ProcessTool::recvBuf( MsgHead *buffer )
{
	if (buffer && m_pSharedMemory)
	{
#ifdef _WIN32
		if (m_bBlock)
		{
			WaitForSingleObject(m_bufferMutex, INFINITE); 
		}else{
			if (WaitForSingleObject(m_bufferMutex, 5000) != WAIT_OBJECT_0)
			{
				return 1;
			}
		} 
#else
		if(!semaphore_p()) 
		{
			cout<<"recvBuf:: semaphore_p error!"<<endl;
			return 1; 
		}
#endif
		memcpy((char*)buffer, m_pSharedMemory, MAX_MSG_LENGTH);
#ifdef _WIN32
		ReleaseMutex(m_bufferMutex);  
#else
		if(!semaphore_v())  
		{
			cout<<"recvBuf:: semaphore_v error!"<<endl;
			return 1; 
		}
#endif 
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
#ifdef _WIN32
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
#else
	del_semvalue();
	if(m_pSharedMemory)
	{
		//delete mem
		shmdt( m_pSharedMemory );
	}      
#endif
}

int ProcessTool::set_semvalue()
{
#ifdef __linux
	//用于初始化信号量，在使用信号量前必须这样做  
	union semun sem_union;  
	sem_union.val = 1;  
	if(semctl(m_semId, 0, SETVAL, sem_union) == -1)  
		return 0;  
#endif
	return 1; 

}

void ProcessTool::del_semvalue()
{
#ifdef __linux
	union semun sem_union;  
	if(semctl(m_semId, 0, IPC_RMID, sem_union) == -1)  
		cout<<"Failed to delete semaphore"<<endl;  
#endif
}

int ProcessTool::semaphore_p()
{
#ifdef __linux
	//对信号量做减1操作，即等待P（sv）  
	struct sembuf sem_b;  
	sem_b.sem_num = 0;  
	sem_b.sem_op = -1;//P()  
	sem_b.sem_flg = SEM_UNDO;  
	if(semop(m_semId, &sem_b, 1) == -1)  
	{  
		cout<<"semaphore_p failed"<<endl;   
		return 0;  
	}  
#endif
	return 1;  
}

int ProcessTool::semaphore_v()
{
#ifdef __linux
	//这是一个释放操作，它使信号量变为可用，即发送信号V（sv）  
	struct sembuf sem_b;  
	sem_b.sem_num = 0;  
	sem_b.sem_op = 1;//V()  
	sem_b.sem_flg = SEM_UNDO;  
	if(semop(m_semId, &sem_b, 1) == -1)  
	{  
		cout<<"semaphore_v failed"<<endl;  
		return 0;  
	}  
#endif
	return 1;
}
