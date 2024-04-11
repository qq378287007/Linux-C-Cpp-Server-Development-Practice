#pragma once

#include "ServerSocket.h"

class CTestApp : public CWinApp
{
public:
	CServerSocket m_ServerSock;
	CTestApp();

	// 重写
public:
	virtual BOOL InitInstance();

	// 实现

	DECLARE_MESSAGE_MAP()
};

extern CTestApp theApp;