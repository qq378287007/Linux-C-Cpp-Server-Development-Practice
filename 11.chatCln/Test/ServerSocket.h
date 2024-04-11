#pragma once

class CServerSocket : public CSocket
{
public:
	CServerSocket();
	virtual ~CServerSocket();
	virtual void OnAccept(int nErrorCode);
	void SendAll(char *bufferdata, int len);
	void DelAll();
	CPtrList m_socketlist;
};
