
#include <iostream>
#include "ProcessTool.h"
using namespace std;

//create side
int main()
{
	ProcessTool *instance= new ProcessTool();
	int ret = 1;
	MsgHead buffer;
	memset(&buffer, 0, sizeof(MsgHead));
	char text[128] = {0};
	memcpy(text, "hello, process!", 128);
	memcpy(buffer.content, text, sizeof(text));
	buffer.contentsize = strlen(text)+1;
	void *mem = instance->createChannel("123", &ret);
	if (mem && ret) //OK
	{
		ret = instance->sendBuf(&buffer);
		if (ret ==0)
		{
			cout<<"send OK!";
		}
	}
	int count = 1500;
	while(count --)
		Sleep(1000);
	return 0;
}
