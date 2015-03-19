// TCPClient.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "InitSock.h"
#include "stdio.h"
#include "time.h"

CInitSock initSock;

int _tmain(int argc, _TCHAR* argv[])
{
	// �����׽���
	SOCKET s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s == INVALID_SOCKET)
	{  
		printf("Failed socket() \n");
		return 0;
	}
	// Ҳ�������������bind ������һ�����ص�ַ
	// ����ϵͳ�����Զ�����
	//... 
	// ��дԶ�̵�ַ��Ϣ
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(4567);
	// ע�⣬����Ҫ��д����������TCPServer �������ڻ�����IP��ַ
	// �����ļ����û��������ֱ��ʹ��127.0.0.1����
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
	// ��������
	char* pTime = (char*)asctime (timeinfo);
	int nSend = ::send(s, (char*)asctime (timeinfo), strlen(pTime), 0);

	if(nSend > 0)
	{
		printf("�������ݣ�%s", pTime);
	}
	// �ر��׽���
	::closesocket(s);
	getchar();
	return 0;
}

