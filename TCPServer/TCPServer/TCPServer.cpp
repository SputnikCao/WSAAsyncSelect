// TCPServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "InitSock.h"
#include "stdio.h"

#define WM_SOCKET WM_USER + 1
CInitSock initSock;
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int _tmain(int argc, _TCHAR* argv[])
{
	// 事件句柄和套接字句柄表
	WSAEVENT eventArray[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET socketArray[WSA_MAXIMUM_WAIT_EVENTS];
	int nEventTotal = 0;
	// 创建套接字
	SOCKET sListen = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sListen == INVALID_SOCKET)
	{  
		printf("Failed socket() \n");
		return 0;
	}
	// 填充sockaddr_in 结构
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4567);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	// 绑定这个套接字到一个本地地址
	if(::bind(sListen, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("Failed bind() \n");
		return 0;
	}
	// 进入监听模式
	if(::listen(sListen, 5) == SOCKET_ERROR)
	{
		printf("Failed listen() \n");
		return 0;
	}
	// 创建事件对象
	WSAEVENT event = ::WSACreateEvent();
	::WSAEventSelect(sListen, event, FD_ACCEPT|FD_CLOSE);
	eventArray[nEventTotal] = event;
	socketArray[nEventTotal] = sListen;
	nEventTotal++;
	// 处理网络事件
	while(TRUE)
	{
		// 在所有事件对象上等待
		int nIndex = ::WSAWaitForMultipleEvents(nEventTotal, eventArray, FALSE, WSA_INFINITE, FALSE);
		nIndex = nIndex - WSA_WAIT_EVENT_0;
		for (int i = nIndex; i < nEventTotal; i++)
		{
			nIndex = ::WSAWaitForMultipleEvents(1, &eventArray[i], TRUE, 1000, FALSE);
			if (nIndex == WSA_WAIT_FAILED || nIndex == WSA_WAIT_TIMEOUT)
				continue;
			else
			{
				// 获取得到的通知消息，自动重置受信事件
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
						// 添加到列表中
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
							printf("接收到数据：%s\n", szText);
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