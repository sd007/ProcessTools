
#include <iostream>
#include "ProcessTool.h"
using namespace std;

#ifdef _WIN32
#define SLEEP(s) Sleep(s) 
#else
#define SLEEP(s) usleep(s*1000)
#endif

//create side
int main()
{
	ProcessTool *instance= new ProcessTool();
	int ret = 1;
	MsgHead buffer;
	memset(&buffer, 0, sizeof(MsgHead));
	char text[128] = {0};
	memcpy(text, "hello, process 123!", 128);
	memcpy(buffer.content, text, sizeof(text));
	buffer.contentsize = strlen(text)+1;
	void *mem = instance->createChannel("123", &ret);
	if (mem && ret) //OK
	{
		ret = instance->sendBuf(&buffer);
//		cout<<"send ret == "<<ret<<endl;
		if (ret ==0)
		{
			cout<<"send OK!"<<endl;
		}
	}
	int count = 1500;
	while(count --)
		SLEEP(1000);
	return 0;
}
