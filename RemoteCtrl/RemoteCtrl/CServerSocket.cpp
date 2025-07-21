#include "pch.h"
#include "CServerSocket.h"
#define BUFFER_SIZE 1024

CServerSocket* CServerSocket::m_instance = NULL;
CServerSocket::CHelper CServerSocket::m_helper;

inline CServerSocket::CServerSocket() {
	m_sock = INVALID_SOCKET;
	m_client = INVALID_SOCKET;
	if (InitSockEnv() == FALSE) {
		MessageBox(NULL, _T("无法初始化套接字环境, 请检查网络设置!"), _T("初始化错误!"), MB_OK | MB_ICONERROR);
		exit(0);
	}
}
inline CServerSocket::~CServerSocket() {
	closesocket(m_sock);
	WSACleanup();
}

BOOL CServerSocket::InitSockEnv()
{
	WSADATA data;
	if (WSAStartup(MAKEWORD(1, 1), &data) != 0) {
		return FALSE;
	}
	return TRUE;
}

bool CServerSocket::InitSock()
{
	m_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (m_sock == INVALID_SOCKET) return false;
	sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY; // 监听所有IP
	serv_addr.sin_port = htons(1234);
	// bind
	if(bind(m_sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) return false;
	// listen
	if(listen(m_sock, 1) == -1) return false;
    return true;
}

void CServerSocket::releaseInstance() {
	if (m_instance != NULL) {
		CServerSocket* tmp = m_instance;
		m_instance = NULL;
		delete tmp;
	}
}

bool CServerSocket::AcceptClient()
{
	sockaddr_in client_addr;
	int cli_sz = sizeof(client_addr);

	m_client = accept(m_sock, (sockaddr*)&client_addr, &cli_sz);
	if (m_sock == -1) return false;
	return true;
}

int CServerSocket::DealCommand()
{
	if (m_client == NULL) return -1;
	char* buffer = new char[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	size_t index = 0;
	while (true) {
		size_t len = recv(m_client, buffer + index, BUFFER_SIZE - index, 0);
		if (len <= 0) {
			return -1;
		}
		index += len;
		len = index;
		// 解析命名
	}
	
	return 0;
}

bool CServerSocket::Send(const char* pData, int nSize)
{
	return send(m_client, pData, nSize, 0);
}



CServerSocket* CServerSocket::getInstance()
{
	if (m_instance == NULL) {
		m_instance = new CServerSocket();
	}
	return m_instance;
}
