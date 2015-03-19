// TCPServer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "InitSock.h"
#include "stdio.h"

#define WM_SOCKET WM_USER + 1
CInitSock initSock;
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int _tmain(int argc, _TCHAR* argv[])
{
	// �¼�������׽��־����
	WSAEVENT eventArray[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET socketArray[WSA_MAXIMUM_WAIT_EVENTS];
	int nEventTotal = 0;
	// �����׽���
	SOCKET sListen = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sListen == INVALID_SOCKET)
	{  
		printf("Failed socket() \n");
		return 0;
	}
	// ���sockaddr_in �ṹ
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4567);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	// ������׽��ֵ�һ�����ص�ַ
	if(::bind(sListen, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("Failed bind() \n");
		return 0;
	}
	// �������ģʽ
	if(::listen(sListen, 5) == SOCKET_ERROR)
	{
		printf("Failed listen() \n");
		return 0;
	}
	// �����¼�����
	WSAEVENT event = ::WSACreateEvent();
	::WSAEventSelect(sListen, event, FD_ACCEPT|FD_CLOSE);
	eventArray[nEventTotal] = event;
	socketArray[nEventTotal] = sListen;
	nEventTotal++;
	// ���������¼�
	while(TRUE)
	{
		// �������¼������ϵȴ�
		int nIndex = ::WSAWaitForMultipleEvents(nEventTotal, eventArray, FALSE, WSA_INFINITE, FALSE);
		nIndex = nIndex - WSA_WAIT_EVENT_0;
		for (int i = nIndex; i < nEventTotal; i++)
		{
			nIndex = ::WSAWaitForMultipleEvents(1, &eventArray[i], TRUE, 1000, FALSE);
			if (nIndex == WSA_WAIT_FAILED || nIndex == WSA_WAIT_TIMEOUT)
				continue;
			else
			{
				// ��ȡ�õ���֪ͨ��Ϣ���Զ����������¼�
				WSANETWORKEVENTS event;
				::WSAEnumNetworkEvents(socketArray[i], eventArray[i], &event);
				if (event.lNetworkEvents & FD_ACCEPT )
				{
					if (event.iErrorCode[FD_ACCEPT_BIT] == 0)
					{
						if (nEventTotal > WSA_MAXIMUM_WAIT_EVENTS)
						{
							printf("Too many connections!\n");
							continue;
						}
						SOCKET sNew = ::accept(socketArray[i], NULL, NULL);
						WSAEVENT event = ::WSACreateEvent();
						::WSAEventSelect(sNew, event, FD_READ|FD_CLOSE|FD_WRITE);
						// ��ӵ��б���
						eventArray[nEventTotal] = event;
						socketArray[nEventTotal] = sNew;
						nEventTotal++;
					}
				}
				else if (event.lNetworkEvents & FD_READ)
				{
					if (event.iErrorCode[FD_READ_BIT] == 0)
					{
						char szText[256];
						int nRecv = ::recv(socketArray[i], szText, strlen(szText), 0);
						if (nRecv > 0)
						{
							szText[nRecv] = '\0';
							printf("���յ����ݣ�%s\n", szText);
						}
					}
				}
				else if (event.lNetworkEvents & FD_CLOSE)
				{
					if (event.iErrorCode[FD_CLOSE_BIT] == 0)
					{
						::closesocket(socketArray[i]);
						for (int j = i; j < nEventTotal - 1; j++)
						{
							socketArray[j] = socketArray[i+1];
							eventArray[j] = eventArray[i+1];
						}
						nEventTotal++;
					}
				}
				else if (event.lNetworkEvents & FD_WRITE)
				{
				}
			}
		}
	}
	return 0;
}