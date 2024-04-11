#pragma once

class CclientDlg : public CDialogEx
{
	// 构造
public:
	CclientDlg(CWnd *pParent = NULL); // 标准构造函数

	// 对话框数据
	enum
	{
		IDD = IDD_CLIENT_DIALOG
	};

protected:
	virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV 支持

	// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();

	CIPAddressCtrl m_ip;
	int m_nServPort;
	afx_msg void OnBnClickedButtonReg();
	CEdit m_port;
	CEdit m_nick;
	CString m_strName;
};
