#include "pch.h"
#include "CServerSocket.h"
#define BUFFER_SIZE 1024

CServerSocket* CServerSocket::m_instance = NULL;
CServerSocket::CHelper CServerSocket::m_helper;

inline CServerSocket::CServerSocket() {
	m_sock = INVALID_SOCKET;
	m_client = INVALID_SOCKET;
	if (InitSockEnv() == FALSE) {
		MessageBox(NULL, _T("�޷���ʼ���׽��ֻ���, ������������!"), _T("��ʼ������!"), MB_OK | MB_ICONERROR);
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
	serv_addr.sin_addr.s_addr = INADDR_ANY; // ��������IP
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
	if (m_client == INVALID_SOCKET) return -1;
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

		//TODO: ��������
		m_packet = CPacket((BYTE*)buffer, len);
		if (len > 0) {
			memmove(buffer, buffer + len, BUFFER_SIZE - len);
			index -= len;
			return m_packet.sCmd; // ���ܳɹ�, ��������
		}
	}
	return -1;
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

CPacket::CPacket(WORD nCmd, const BYTE* pData, size_t nSize)
{
	sHead = 0xFEFF;
	nLength = nSize + 4;
	sCmd = nCmd;
	if (nSize > 0) {
		strData.resize(nSize);
		memcpy((void*)strData.c_str(), pData, nSize);
	}
	else {
		strData.clear(); // ���������
	}
	sSum = 0;
	for (size_t i = 0; i < strData.size(); i++) {
		sSum += BYTE(strData[i]) & 0xFF;
	}
}

CPacket::CPacket(const CPacket& pack)
{
	sHead = pack.sHead;
	nLength = pack.nLength;
	sCmd = pack.sCmd;
	strData = pack.strData;
	sSum = pack.sCmd;
}

CPacket& CPacket::operator=(const CPacket& pack)
{
	if (this != &pack) {
		sHead = pack.sHead;
		nLength = pack.nLength;
		sCmd = pack.sCmd;
		strData = pack.strData;
		sSum = pack.sCmd;
	}

	return *this;
}

CPacket::CPacket(const BYTE* pData, size_t& nSize) // ���ڰ����ݽ���
{
	size_t i = 0;
	for (; i < nSize; i++) {
		if (*(WORD*)(pData + i) == 0xFEFE) {  // ʶ���ͷ
			sHead = *(WORD*)(pData + i); // ȡ��ͷ
			i += 2;
			break;
		}
	}
	if (i + 4 + 2 + 2 > nSize) // �����ݿ��ܲ�ȫ
	{
		nSize = 0;
		return; // ����ʧ��
	}
	nLength = *(DWORD*)(pData + i); i += 4; // ������
	if (nLength + i > nSize) 
	{
		nSize = 0;
		return; // ����ʧ��
	}

	sCmd = *(WORD*)(pData + i); i += 2; // ����������

	if (nLength > 4) { // ���ݳ��ȴ���4
		strData.resize(nLength - 2 - 2);
		memcpy((void *)strData.c_str(), pData + i, nLength - 2 - 2);
		i = nLength - 4;
	}

	sSum = *(WORD*)(pData + i); i += 2;// ����У����
	WORD sum = 0;
	for (size_t j = 0; j < strData.size(); j++) {
		sum += BYTE(strData[j]) & 0xFF;
	}
	if (sum == sSum) {     // У��
		nSize = i;         // ͷ + ����+ ����
		return;
	}
	nSize = 0;
}

int CPacket::Size() // ��ȡ�����ݳ���
{
	return nLength + 6;
}

const char* CPacket::Data() // ���ڽ��������������뻺����
{
	strOut.resize(Size());
	BYTE* pData = (BYTE*)strOut.c_str();
	*(WORD*)pData = sHead; pData += 2;
	*(DWORD*)pData = nLength; pData += 4;
	*(WORD*)pData = sCmd; pData += 2;
	memcpy(pData, strData.c_str(), strData.size()); pData += strData.size();
	*(WORD*)pData = sSum;
	return strOut.c_str(); 
}
