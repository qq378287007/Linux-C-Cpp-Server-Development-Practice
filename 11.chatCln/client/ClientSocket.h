#pragma once
#include "DlgChat.h"
class CClientSocket : public CSocket
{
public:
	CClientSocket();
	virtual ~CClientSocket();
	void SetWnd(CDlgChat *pDlg);
	virtual void OnReceive(int nErrorCode);
	CDlgChat *m_pDlg;
};
