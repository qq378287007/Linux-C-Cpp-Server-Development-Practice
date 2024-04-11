#include "client.h"
#include "ClientSocket.h"

void GetReply(char str[], char reply[])
{
	const char *split = ",";
	char *p;
	p = strtok(str, split);
	int i = 0;
	while (p != NULL)
	{
		printf("%s\n", p);
		if (i == 1)
			sprintf(reply, p);
		i++;
		p = strtok(NULL, split);
	}
}

CClientSocket::CClientSocket()
{
	m_pDlg = NULL;
}

CClientSocket::~CClientSocket()
{
}

void CClientSocket::SetWnd(CDlgChat *pDlg)
{
	m_pDlg = pDlg;
}

void CClientSocket::OnReceive(int nErrorCode)
{
	// TODO:  在此添加专用代码和/或调用基类
	CString str;
	char buffer[2048], rep[128] = "";
	if (m_pDlg) // m_pDlg指向聊天对话框
	{
		int len = Receive(buffer, 2048);
		if (len != -1)
		{
			buffer[len] = '\0';
			buffer[len + 1] = '\0';
			str.Format(_T("%s"), buffer);
			m_pDlg->m_lst.AddString(str);
		}
	}
	else
	{
		// 注册回复
		int len = Receive(buffer, 2048);
		if (len != -1)
		{
			buffer[len] = '\0';
			buffer[len + 1] = '\0';
			str.Format(_T("%s"), buffer);
			if (buffer[0] == 'r')
			{
				GetReply(buffer, rep);
				if (strcmp("ok", rep) == 0)
					AfxMessageBox("注册成功");
				else if (strcmp("exist", rep) == 0)
					AfxMessageBox("注册失败，用户名已经存在!");
			}
			else if (buffer[0] == 'l')
			{
				GetReply(buffer, rep);
				if (strcmp("noexist", rep) == 0)
					AfxMessageBox("登录失败，用户名不存在，请先注册.");
				else if (strcmp("ok", rep) == 0)
				{
					AfxMessageBox("登录成功");
					CDlgChat dlg;
					theApp.m_clinetsock.SetWnd(&dlg);
					dlg.DoModal();
				}
			}
		}
	}
	CSocket::OnReceive(nErrorCode);
}
