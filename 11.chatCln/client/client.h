#pragma once

#include "ClientSocket.h"

class CclientApp : public CWinApp
{
public:
	CString m_strName;
	CClientSocket m_clinetsock;

	CclientApp();

	// 重写
public:
	virtual BOOL InitInstance();

	// 实现

	DECLARE_MESSAGE_MAP()
};

extern CclientApp theApp;