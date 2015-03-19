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
	WCHAR szClassName[] = _T("MainClass");
	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc = WindowProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = NULL;
	wndclass.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szClassName;
	wndclass.hIconSm = NULL;
	::RegisterClassEx(&wndclass);

	HWND hWnd = ::CreateWindowEx(0, szClassName, _T(""), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, NULL, NULL);
	if (hWnd == NULL)
	{
		::MessageBox(NULL, _T("创建窗口出错！"), _T("error"), MB_OK);
		return -1;
	}
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
	// 将套接字设为窗口通知类型消息
	::WSAAsyncSelect(sListen, hWnd, WM_SOCKET, FD_ACCEPT|FD_CLOSE);
	// 进入监听模式
	if(::listen(sListen, 5) == SOCKET_ERROR)
	{
		printf("Failed listen() \n");
		return 0;
	}

	MSG msg;
	while(::GetMessage(&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_SOCKET:
		{
			SOCKET s = wParam;
			if (WSAGETSELECTERROR(lParam))
			{
				::closesocket(s);
				return 0;
			}
			switch(WSAGETSELECTEVENT(lParam))
			{
			case FD_ACCEPT:
				{
					SOCKET client = ::accept(s, NULL, NULL);
					::WSAAsyncSelect(client, hWnd, WM_SOCKET, FD_READ|FD_WRITE|FD_CLOSE);
				}
				break;
			case FD_WRITE:
				{
				}
				break;
			case FD_READ:
				{
					char szText[1024] = {0};
					if (recv(s, szText, 1024, 0) == -1)
						closesocket(s);
					else
						printf(("接收数据：%s"), szText);
				}
				break;
			case FD_CLOSE:
				{
					::closesocket(s);
				}
				break;
			}
		}
		return 0;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//int _tmain(int argc, _TCHAR* argv[])
//{
//	// 创建套接字
//	SOCKET sListen = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//	if(sListen == INVALID_SOCKET)
//	{  
//		printf("Failed socket() \n");
//		return 0;
//	}
//	// 填充sockaddr_in 结构
//	sockaddr_in sin;
//	sin.sin_family = AF_INET;
//	sin.sin_port = htons(4567);
//	sin.sin_addr.S_un.S_addr = INADDR_ANY;
//	// 绑定这个套接字到一个本地地址
//	if(::bind(sListen, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
//	{
//		printf("Failed bind() \n");
//		return 0;
//	}
//	// 进入监听模式
//	if(::listen(sListen, 2) == SOCKET_ERROR)
//	{
//		printf("Failed listen() \n");
//		return 0;
//	}
//	// 循环接受客户的连接请求
//	sockaddr_in remoteAddr;
//	int nAddrLen = sizeof(remoteAddr);
//	SOCKET sClient;
//	char szText[] = " TCP Server Demo! \r\n";
//	while(TRUE)
//	{  // 接受新连接
//		sClient = ::accept(sListen, (SOCKADDR*)&remoteAddr, &nAddrLen);
//		if(sClient == INVALID_SOCKET)
//		{
//			printf("Failed accept()");
//			continue;
//		}
//		printf(" 接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));
//		// 向客户端发送数据
//		::send(sClient, szText, strlen(szText), 0);
//		// 关闭同客户端的连接
//		::closesocket(sClient);
//	}
//	// 关闭监听套接字
//	::closesocket(sListen);
//	return 0;
//}
//



//fd_set fdSocket;
//FD_ZERO(&fdSocket);
//FD_SET(sListen, &fdSocket);
//while(TRUE)
//{
//	fd_set fdRead = fdSocket;
//	int nRet = ::select(0, &fdRead, NULL, NULL, NULL);
//	if (nRet > 0)
//	{
//		for (int i = 0; i < (int)fdSocket.fd_count; i++)
//		{
//			if(FD_ISSET(fdSocket.fd_array[i], &fdRead))
//			{
//				if(fdSocket.fd_array[i] == sListen)		// （1）监听套节字接收到新连接
//				{
//					if(fdSocket.fd_count < FD_SETSIZE)
//					{
//						sockaddr_in addrRemote;
//						int nAddrLen = sizeof(addrRemote);
//						SOCKET sNew = ::accept(sListen, (SOCKADDR*)&addrRemote, &nAddrLen);
//						FD_SET(sNew, &fdSocket);
//						printf("接收到连接（%s）\n", ::inet_ntoa(addrRemote.sin_addr));
//					}
//					else
//					{
//						printf(" Too much connections! \n");
//						continue;
//					}
//				}
//				else
//				{
//					char szText[256];
//					int nRecv = ::recv(fdSocket.fd_array[i], szText, strlen(szText), 0);
//					if(nRecv > 0)						// （2）可读
//					{
//						szText[nRecv] = '\0';
//						printf("接收到数据：%s \n", szText);
//					}
//					else								// （3）连接关闭、重启或者中断
//					{
//						::closesocket(fdSocket.fd_array[i]);
//						FD_CLR(fdSocket.fd_array[i], &fdSocket);
//					}
//				}
//			}
//		}
//	}
//	else
//	{
//		printf("failed select\n");
//		break;
//	}
//}
//return 0;