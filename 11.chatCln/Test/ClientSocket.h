#pragma once

// CClientSocket 命令目标
class CClientSocket : public CSocket
{
public:
	CClientSocket();
	virtual ~CClientSocket();
	virtual void OnReceive(int nErrorCode);
};
