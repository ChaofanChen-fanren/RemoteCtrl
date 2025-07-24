#pragma once
#include "pch.h"
#include "framework.h"

class CPacket
{
public:
	CPacket() : sHead(0), nLength(0), sCmd(0), sSum(0) {};
	CPacket(WORD nCmd, const BYTE* pData, size_t nSize);
	CPacket(const CPacket& pack);
	CPacket& operator= (const CPacket& pack);
	CPacket(const BYTE* pData, size_t& nSize);
	~CPacket() {};

	int Size();
	const char* Data();

public:
	WORD sHead;              // 包头, 固定为FE FF
	DWORD nLength;           // 包长
	WORD sCmd;               // 命令
	std::string strData;     // 包数据
	WORD sSum;               // 和校验

	std::string strOut;      // 整个包的数据
};


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
	CPacket m_packet;

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

