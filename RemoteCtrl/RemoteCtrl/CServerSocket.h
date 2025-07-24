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
	WORD sHead;              // ��ͷ, �̶�ΪFE FF
	DWORD nLength;           // ����
	WORD sCmd;               // ����
	std::string strData;     // ������
	WORD sSum;               // ��У��

	std::string strOut;      // ������������
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
	static CHelper m_helper; // �����ͷ�m_instance�Ķ���
	CPacket m_packet;

	class CHelper
	{
	public:
		CHelper() {
			CServerSocket::getInstance(); // ���ڳ�ʼ��CServerSocket����
		}
		~CHelper() {
			CServerSocket::releaseInstance();
		}
	};
};

