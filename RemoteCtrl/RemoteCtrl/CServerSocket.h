#pragma once
#include "pch.h"
#include "framework.h"



class CServerSocket
{
public:
	static CServerSocket* getInstance();
	bool InitSock();
	bool AcceptClient();
	int DealCommand();
	bool Send(const char* pData, int nSize);
private:
	CServerSocket();
	CServerSocket(const CServerSocket&) = default;
	CServerSocket& operator=(const CServerSocket&) = default;
	~CServerSocket();
	BOOL InitSockEnv();
	static void releaseInstance();
private:
	class CHelper;
	SOCKET m_sock;
	SOCKET m_client;
	static CServerSocket *m_instance;
	static CHelper m_helper; // 帮助释放m_instance的对象

	class CHelper
	{
	public:
		CHelper() {
			CServerSocket::getInstance(); // 用于初始化CServerSocket对象
		}
		~CHelper() {
			CServerSocket::releaseInstance();
		}
	};
};

