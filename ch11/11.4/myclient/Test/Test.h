
// Test.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

#include "ServerSocket.h"
// CTestApp: 
// �йش����ʵ�֣������ Test.cpp
//

class CTestApp : public CWinApp
{
public:
	CServerSocket m_ServerSock;
	CTestApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestApp theApp;