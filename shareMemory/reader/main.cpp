#include "ProcessTool.h"
#include <iostream>
using namespace std;


int main()
{
	ProcessTool *instance= new ProcessTool();
	int ret = 1;
	MsgHead buffer;
	memset(&buffer, 0, sizeof(MsgHead));
	void *mem = instance->openChannel("123", &ret);
	if (mem && ret) //OK
	{
		ret = instance->recvBuf(&buffer);
		if (ret ==0)
		{
			cout<<"recv buffer: "<< buffer.contentsize <<" - "<<buffer.content<<endl;
		}
	}
	return 0;
}