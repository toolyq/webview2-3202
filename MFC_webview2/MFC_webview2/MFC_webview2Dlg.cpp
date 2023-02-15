
// MFC_webview2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFC_webview2.h"
#include "MFC_webview2Dlg.h"
#include "afxdialogex.h"
#include <afxinet.h>
#include "CTtt.h"
//#include <stdlib.h>  // to use getenv()
//#define_CRT_SECURE_NO_WARNINGS // to use getenv()

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Microsoft::WRL; //Microsoft::WRL::Callback

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CMFC_webview2Dlg dialog
CMFC_webview2Dlg::CMFC_webview2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFC_WEBVIEW2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFC_webview2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFC_webview2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()

BOOL CMFC_webview2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	CRgn m_rgn;
	RECT rc;
	SetWindowRgn(m_rgn, TRUE);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	//as do model
	DWORD dwStyle = GetStyle();
	DWORD dwNewStyle = dwStyle&~WS_MAXIMIZEBOX&~WS_MINIMIZEBOX;
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwNewStyle);
	////////////////

	// TODO: Add extra initialization here
	////////////////////////////////////////////////////////
	CoInitialize(nullptr); 
	CString subFolder = nullptr;
	CString appData = GetUserAppData();
	ICoreWebView2EnvironmentOptions* options = nullptr;
	//Microsoft::WRL::Callback
	HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(subFolder, appData, options,
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(this, 
		&CMFC_webview2Dlg::OnCreateEnvironmentCompleted).Get());
	if (!SUCCEEDED(hr))
	{
		CString text;
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			text = L"Cannot found the WebView2 component.";
		}
		else
		{
			text = L"Cannot create the webview environment.";
		}
		AfxMessageBox(text);
	}
	//char* userFolder = getenv("WEBVIEW2_USER_DATA_FOLDER");
	CString cstr;
	DWORD nRet = ::GetEnvironmentVariable(_T("WEBVIEW2_USER_DATA_FOLDER"), cstr.GetBuffer(1024), 1024);
	cstr.ReleaseBuffer(nRet);
	return TRUE;  // return TRUE  unless you set the focus to a control
}



HRESULT CMFC_webview2Dlg::OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* environment)
{
	m_webViewEnvironment = environment;
	if (!m_webViewEnvironment)
	{
		CString msg;
		msg.Format(_T("Failed to create environment. hResult=0x%08x"), result);
		AfxMessageBox(msg);
		return FAILED(result) ? result : E_FAIL;
	}

	HWND hWnd = this->GetSafeHwnd();
	m_webViewEnvironment->CreateCoreWebView2Controller(hWnd, 
		Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
			[=](HRESULT result, ICoreWebView2Controller* controller)->HRESULT{
		OnCreateCoreWebView2ControllerCompleted(result, controller);
		return S_OK;
	}
			).Get());

	return S_OK;
}

HRESULT CMFC_webview2Dlg::OnCreateCoreWebView2ControllerCompleted(HRESULT result, ICoreWebView2Controller* controller)
{
	if (controller != nullptr) {
		m_controller = controller;
		m_controller->get_CoreWebView2(&m_webView);
	}
	else
	{
		return S_FALSE;
	}
	RECT bounds;
	GetClientRect(&bounds);
	m_controller->put_Bounds(bounds);

	EventRegistrationToken newWindowRequestedtoken;
	m_webView->add_NewWindowRequested(Microsoft::WRL::Callback<ICoreWebView2NewWindowRequestedEventHandler>(//wrap windown.open()
		[=](ICoreWebView2* webview, ICoreWebView2NewWindowRequestedEventArgs * args) -> HRESULT {
		LPWSTR url;
		UINT32 left, top, width, height;
		args->get_Uri(&url);

		CMFC_webview2Dlg* dlg = new CMFC_webview2Dlg(this);
		dlg->url = url;
		CComPtr<ICoreWebView2Deferral> deferral;
		args->GetDeferral(&deferral);

		dlg->m_onWebViewFirstInitialized = [args, dlg, deferral]() {
			args->put_NewWindow(dlg->m_webView);
			//dlg->m_webView->AddWebResourceRequestedFilter(
			//	L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);
			//dlg->m_webView->add_WebResourceRequested(
			//	Microsoft::WRL::Callback<ICoreWebView2WebResourceRequestedEventHandler>(
			//		[](
			//			ICoreWebView2* sender,
			//			ICoreWebView2WebResourceRequestedEventArgs* args) {
			//	COREWEBVIEW2_WEB_RESOURCE_CONTEXT resourceContext;
			//	args->get_ResourceContext(&resourceContext);
			//	ICoreWebView2WebResourceRequest* request;
			//	args->get_Request(&request);

			//	LPWSTR uri;
			//	request->get_Uri(&uri);

			//	LPWSTR method;
			//	request->get_Method(&method);

			//	IStream*content;
			//	request->get_Content(&content);
			//	return S_OK;
			//})
			//	.Get(), nullptr);
			args->put_Handled(TRUE);
			deferral->Complete();
		};
		if (dlg->Create(IDD_MFC_WEBVIEW2_DIALOG, this))
		{
			dlg->ShowWindow(SW_SHOWNORMAL);
		}
		return S_OK;
	}).Get(), &newWindowRequestedtoken);

	//m_webView->Navigate(url);//**************************************************

	if (m_onWebViewFirstInitialized)
	{
		m_onWebViewFirstInitialized();
		m_onWebViewFirstInitialized = nullptr;
	}
	m_webView->Navigate(url);// reporduce the issue

	return S_OK;
}

HRESULT CMFC_webview2Dlg::OnDevToolsProtocolMethod(HRESULT errorCode, LPCWSTR returnObjectAsJson)
{
	//get all cookie
	//{"cookies":[{"domain":"www.jiben.tech","expires":-1,"httpOnly":false,"name":"name","path":"/","priority":"Medium","sameParty":false,"sameSite":"Lax","secure":false,"session":true,"size":7,"sourcePort":80,"sourceScheme":"NonSecure","value":"val"}]}
	
	return S_OK;
}


void CMFC_webview2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFC_webview2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFC_webview2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
