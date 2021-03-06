// PreDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PreDemo.h"
#include "PreDemoDlg.h"
#include "bm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define Start 10012
#define Pause 10013
#define Close 10014
#define Screenshot 10015
#define WM_MYNOTIFY WM_USER + 100

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

    // 对话框数据
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    // 实现
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPreDemoDlg 对话框




CPreDemoDlg::CPreDemoDlg(CWnd* pParent /*=NULL*/)
: CMyDialog(CPreDemoDlg::IDD, pParent)
{
    m_bThread			=	FALSE;
    m_pHostThread		=	NULL;
    memset(&params, 0, sizeof(PARAMS));
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPreDemoDlg::DoDataExchange(CDataExchange* pDX)
{
    CMyDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB, m_TabPage);
}

BEGIN_MESSAGE_MAP(CPreDemoDlg, CMyDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_WM_HOTKEY()
    ON_BN_CLICKED(IDOK, &CPreDemoDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDC_BTN_START, &CPreDemoDlg::OnBnClickedBtnStart)
    ON_BN_CLICKED(IDC_BTN_PAUSE, &CPreDemoDlg::OnBnClickedBtnPause)
    ON_MESSAGE(WM_MYNOTIFY, &CPreDemoDlg::OnMyNotify)
    ON_COMMAND(IDM_ABOUTBOX, &CPreDemoDlg::OnNotifyAboutbox)
    ON_COMMAND(IDM_SHOW, &CPreDemoDlg::OnNotifyShow)
    ON_COMMAND(IDM_CLOSE, &CPreDemoDlg::OnNotifyClose)
    ON_BN_CLICKED(IDCANCEL, &CPreDemoDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CPreDemoDlg 消息处理程序

BOOL CPreDemoDlg::OnInitDialog()
{
    CMyDialog::OnInitDialog();

    // 将“关于...”菜单项添加到系统菜单中。

    // IDM_ABOUTBOX 必须在系统命令范围内。
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码


    //初始化选项页面
    m_Page1.Create(IDD_DIALOG1, &m_TabPage);
    m_Page2.Create(IDD_DIALOG2, &m_TabPage);
    m_Page3.Create(IDD_DIALOG3, &m_TabPage);
    m_Page4.Create(IDD_DIALOG4, &m_TabPage);

    m_TabPage.AddPage(&m_Page1, _T("基础设定"));
    m_TabPage.AddPage(&m_Page2, _T("店铺设定"));
    m_TabPage.AddPage(&m_Page3, _T("传送设定"));
    m_TabPage.AddPage(&m_Page4, _T("高级设定"));

    RECT rect;
    m_TabPage.GetClientRect(&rect);

    rect.left	+=	2;
    rect.top	+=	22;
    rect.right	-=	4;
    rect.bottom	-=	2;

    m_Page1.MoveWindow(&rect);
    m_Page2.MoveWindow(&rect);
    m_Page3.MoveWindow(&rect);
    m_Page4.MoveWindow(&rect);

    //初始化托盘变量
    memset(&m_NotifyIcon, 0, sizeof(NOTIFYICONDATA));

    m_NotifyIcon.cbSize	=	sizeof(NOTIFYICONDATA);
    m_NotifyIcon.hWnd	=	m_hWnd;
    m_NotifyIcon.uID	=	IDR_MAINFRAME;
    m_NotifyIcon.hIcon	=	m_hIcon;
    _tcscpy_s(m_NotifyIcon.szTip, _countof(m_NotifyIcon.szTip), AfxGetAppName());
    m_NotifyIcon.uFlags	=	NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_NotifyIcon.uCallbackMessage	=	WM_MYNOTIFY;

    ::Shell_NotifyIcon(NIM_ADD, &m_NotifyIcon);

    //注册全局热键
    DWORD HotKeyStart      = theApp.GetProfileInt(_T("BaseSetting"), _T("HotKeyStart"), 0);
    DWORD HotKeyPause      = theApp.GetProfileInt(_T("BaseSetting"), _T("HotKeyPause"), 0);
    DWORD HotKeyClose      = theApp.GetProfileInt(_T("BaseSetting"), _T("HotKeyClose"), 0);
    DWORD HotKeyScreenshot = theApp.GetProfileInt(_T("BaseSetting"), _T("HotKeyScreenshot"), 0);

    ::RegisterHotKey(m_hWnd, Start, HIBYTE(HotKeyStart), LOBYTE(HotKeyStart));
    ::RegisterHotKey(m_hWnd, Pause, HIBYTE(HotKeyPause), LOBYTE(HotKeyPause));
    ::RegisterHotKey(m_hWnd, Close, HIBYTE(HotKeyClose), LOBYTE(HotKeyClose));
    ::RegisterHotKey(m_hWnd, Screenshot, HIBYTE(HotKeyScreenshot), LOBYTE(HotKeyScreenshot));

    GetDlgItem(IDOK)->EnableWindow(FALSE);

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPreDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else if (theApp.GetProfileInt(_T("BaseSetting"), _T("bNotify"), 0) && nID == SC_MINIMIZE)
    {
        ::Shell_NotifyIcon(NIM_ADD, &m_NotifyIcon);
        ShowWindow(SW_HIDE);
    }
    else
    {
        CMyDialog::OnSysCommand(nID, lParam);
    }
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPreDemoDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CMyDialog::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPreDemoDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CPreDemoDlg::OnDestroy()
{
    ::Shell_NotifyIcon(NIM_DELETE, &m_NotifyIcon);

    if (m_pHostThread)
        delete m_pHostThread;
    m_Page1.DestroyWindow();
    m_Page2.DestroyWindow();
    m_Page3.DestroyWindow();
    m_Page4.DestroyWindow();

    ::UnregisterHotKey(m_hWnd, Start);
    ::UnregisterHotKey(m_hWnd, Pause);
    ::UnregisterHotKey(m_hWnd, Close);
    ::UnregisterHotKey(m_hWnd, Screenshot);

    CMyDialog::OnDestroy();
}

//保存程序配置
void CPreDemoDlg::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    FlashWindow(TRUE);

    //卸载旧的热键
    ::UnregisterHotKey(m_hWnd, Start);
    ::UnregisterHotKey(m_hWnd, Pause);
    ::UnregisterHotKey(m_hWnd, Close);
    ::UnregisterHotKey(m_hWnd, Screenshot);

    m_Page1.SaveState();
    m_Page2.SaveState();
    m_Page3.SaveState();
    m_Page4.SaveState();

    //注册新的热键
    DWORD HotKeyStart      = theApp.GetProfileInt(_T("BaseSetting"), _T("HotKeyStart"), 0);
    DWORD HotKeyPause      = theApp.GetProfileInt(_T("BaseSetting"), _T("HotKeyPause"), 0);
    DWORD HotKeyClose      = theApp.GetProfileInt(_T("BaseSetting"), _T("HotKeyClose"), 0);
    DWORD HotKeyScreenshot = theApp.GetProfileInt(_T("BaseSetting"), _T("HotKeyScreenshot"), 0);

    ::RegisterHotKey(m_hWnd, Start, HIBYTE(HotKeyStart), LOBYTE(HotKeyStart));
    ::RegisterHotKey(m_hWnd, Pause, HIBYTE(HotKeyPause), LOBYTE(HotKeyPause));
    ::RegisterHotKey(m_hWnd, Close, HIBYTE(HotKeyClose), LOBYTE(HotKeyClose));
    ::RegisterHotKey(m_hWnd, Screenshot, HIBYTE(HotKeyScreenshot), LOBYTE(HotKeyScreenshot));

    GetDlgItem(IDOK)->EnableWindow(FALSE);
}

// 托盘消息响应
LRESULT CPreDemoDlg::OnMyNotify(WPARAM wParam, LPARAM lParam)
{
    if (wParam != IDR_MAINFRAME)
        return 1;

    switch (lParam)
    {
    case WM_LBUTTONDOWN:
        {
            ShowWindow(SW_SHOW);
            break;
        }
    case WM_RBUTTONDOWN:
        {
            CMenu menu;
            menu.LoadMenu(IDR_MENU);
            CMenu *pPopUp = menu.GetSubMenu(0);
            POINT pt;
            ::GetCursorPos(&pt);
            pPopUp->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
            break;
        }
    default:
        break;
    }
    return 0;
}

//启动工作线程
void CPreDemoDlg::OnBnClickedBtnStart()
{
    // TODO: Add your control notification handler code here
    DWORD dwExitCode;

    if (m_pHostThread)
    {
        ::GetExitCodeThread(m_pHostThread->m_hThread, &dwExitCode);
        if (dwExitCode == STILL_ACTIVE)
            return;
        else
        {
            delete m_pHostThread;
            m_pHostThread = NULL;
        }
    }

    if (!m_pHostThread)
    {
        HWND hWnd = ::FindWindow(_T("WSGAME"), NULL);
        if (!hWnd)
        {
            AfxMessageBox(_T("找不到游戏窗口！"));
            DestroyWindow();
            return;
        }
        params.hwndScreen = hWnd;
        ShowWindow(SW_HIDE);
        m_pHostThread = AfxBeginThread(ThreadHostProc, &params, 0, 0, CREATE_SUSPENDED);
        m_pHostThread->m_bAutoDelete = FALSE;
        m_pHostThread->ResumeThread();
        m_bThread     = TRUE;
    }
}

//暂停线程
void CPreDemoDlg::OnBnClickedBtnPause()
{
    // TODO: Add your control notification handler code here
    DWORD dwExitCode;

    if (m_pHostThread && 
        ::GetExitCodeThread(m_pHostThread->m_hThread, &dwExitCode) &&
        dwExitCode == STILL_ACTIVE)
    {
        if (m_bThread)
        {
            m_pHostThread->SuspendThread();
            m_bThread = FALSE;
        }
        else
        {
            m_pHostThread->ResumeThread();
            m_bThread = TRUE;
        }
    }
}

// 游戏窗口截图
void CPreDemoDlg::OnScreenshot()
{
    HWND hWnd = ::FindWindow(_T("WSGAME"), NULL);
    if (!hWnd)
        return;

    Cbm(hWnd).SaveBitmapToFile();
    TRACE(_T("Screenshot end.\n"));
}

//热键消息函数
void CPreDemoDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
    switch (nHotKeyId)
    {
    case Start:
        OnBnClickedBtnStart();
        break;

    case Pause:
        OnBnClickedBtnPause();
        break;

    case Close:
        OnBnClickedCancel();
        break;

    case Screenshot:
        OnScreenshot();
        break;

    default:
        break;
    }

    CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}

//////////////////////////////////////////////
// 托盘菜单响应  ///////////////////////////////

void CPreDemoDlg::OnNotifyAboutbox()
{
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
}

void CPreDemoDlg::OnNotifyShow()
{
    ShowWindow(SW_SHOW);
}

void CPreDemoDlg::OnNotifyClose()
{
    DestroyWindow();
}

//////////////////////////////////////////////

// 退出按钮
void CPreDemoDlg::OnBnClickedCancel()
{
    DestroyWindow();
}


BOOL CPreDemoDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_HOTKEY)
    {
        CWnd* pWnd = CWnd::GetFocus();
        if (pWnd)
        {
            int ctrlId = pWnd->GetDlgCtrlID();
            if (ctrlId >= IDC_HOTKEY1 && ctrlId <= IDC_HOTKEY4)
                return TRUE;
        }
    }

    return CMyDialog::PreTranslateMessage(pMsg);
}
