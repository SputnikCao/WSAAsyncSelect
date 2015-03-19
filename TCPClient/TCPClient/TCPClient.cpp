// TCPClient.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "InitSock.h"
#include "stdio.h"
#include "time.h"

CInitSock initSock;

int _tmain(int argc, _TCHAR* argv[])
{
	// 创建套接字
	SOCKET s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s == INVALID_SOCKET)
	{  
		printf("Failed socket() \n");
		return 0;
	}
	// 也可以在这里调用bind 函数绑定一个本地地址
	// 否则系统将会自动安排
	//... 
	// 填写远程地址信息
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(4567);
	// 注意，这里要填写服务器程序（TCPServer 程序）所在机器的IP地址
	// 如果你的计算机没有联网，直接使用127.0.0.1即可
	servAddr.sin_addr.S_un.S_addr = inet_addr("192.168.0.215");
	if(::connect(s, (sockaddr*)&servAddr, sizeof(servAddr)) == -1)
	{  
		printf("Failed connect() \n");
		return 0;
	}

	time_t rawtime;
	struct tm * timeinfo;

	time( &rawtime );
	timeinfo = localtime( &rawtime );
	// 接收数据
	char* pTime = (char*)asctime (timeinfo);
	int nSend = ::send(s, (char*)asctime (timeinfo), strlen(pTime), 0);

	if(nSend > 0)
	{
		printf("发送数据：%s", pTime);
	}
	// 关闭套接字
	::closesocket(s);
	getchar();
	return 0;
}

