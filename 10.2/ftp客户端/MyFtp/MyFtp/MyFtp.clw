; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CFtpDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "myftp.h"
LastPage=0

ClassCount=8
Class1=CConnectDlg
Class2=CFtpDlg
Class3=CMainFrame
Class4=CMyFtpApp
Class5=CAboutDlg
Class6=CMyFtpDoc
Class7=CMyFtpView

ResourceCount=4
Resource1=IDR_MAINFRAME
Resource2=IDD_DIALOG2
Resource3=IDD_DIALOG1
Class8=CNewNameDlg
Resource4=IDD_DIALOG3

[CLS:CConnectDlg]
Type=0
BaseClass=CDialog
HeaderFile=ConnectDlg.h
ImplementationFile=ConnectDlg.cpp
LastObject=CConnectDlg

[CLS:CFtpDlg]
Type=0
BaseClass=CDialog
HeaderFile=FtpDlg.h
ImplementationFile=FtpDlg.cpp
LastObject=IDM_CONNECT
Filter=D
VirtualFilter=dWC

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
VirtualFilter=fWC

[CLS:CMyFtpApp]
Type=0
BaseClass=CWinApp
HeaderFile=MyFtp.h
ImplementationFile=MyFtp.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=MyFtp.cpp
ImplementationFile=MyFtp.cpp
LastObject=CAboutDlg

[CLS:CMyFtpDoc]
Type=0
BaseClass=CDocument
HeaderFile=MyFtpDoc.h
ImplementationFile=MyFtpDoc.cpp

[CLS:CMyFtpView]
Type=0
BaseClass=CView
HeaderFile=MyFtpView.h
ImplementationFile=MyFtpView.cpp

[DLG:IDD_DIALOG1]
Type=1
Class=CConnectDlg
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_EDIT1,edit,1350631552
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT2,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_EDIT3,edit,1350631584

[DLG:IDD_DIALOG2]
Type=1
Class=CFtpDlg
ControlCount=11
Control1=IDC_LIST_FILE,SysListView32,1350631425
Control2=IDC_QUARY,button,1342246656
Control3=IDC_UPLOAD,button,1342242816
Control4=IDC_EXIT,button,1342242816
Control5=IDC_DOWNLOAD,button,1342242816
Control6=IDC_DELETE,button,1342242816
Control7=IDC_RENAME,button,1342242816
Control8=IDC_STATIC,button,1342308359
Control9=IDC_STATIC,static,1342308352
Control10=IDC_LASTDIRECTORY,button,1342242816
Control11=IDC_NEXTDIRECTORY,button,1342242816

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=IDM_CONNECT
Command2=IDM_EXIT
CommandCount=2

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_BUTTON32778
Command2=ID_APP_ABOUT
CommandCount=2

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[DLG:IDD_DIALOG3]
Type=1
Class=CNewNameDlg
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDC_EDIT1,edit,1350631552
Control3=IDCANCEL,button,1342242816
Control4=IDC_STATIC,static,1342308352

[CLS:CNewNameDlg]
Type=0
HeaderFile=NewNameDlg.h
ImplementationFile=NewNameDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CNewNameDlg

