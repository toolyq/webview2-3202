
// MFC_webview2Dlg.h : header file
//
#include <Memory>
#include "WebView2.h"
#include <wrl.h>
#include <wininet.h>
#include <functional>
#pragma once

#include <Wtsapi32.h>
#pragma comment(lib, "WtsApi32.lib") 

// CMFC_webview2Dlg dialog
class CMFC_webview2Dlg : public CDialogEx
{
// Construction
public:
	CMFC_webview2Dlg(CWnd* pParent = NULL);	// standard constructor

	CComPtr<ICoreWebView2> m_webView;
	//CString url = _T("file:///C:/inetpub/wwwroot/DirectXml/PDF/D_DU_ETF_4Q08.pdf");
	CString url = _T("http://local.stg/a.html");
	HRESULT OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* environment);
	HRESULT OnCreateCoreWebView2ControllerCompleted(HRESULT result, ICoreWebView2Controller* controller);
	HRESULT OnGetCookies(HRESULT rs, ICoreWebView2CookieList *cookieList);
	HRESULT OnDevToolsProtocolMethod(HRESULT errorCode, LPCWSTR returnObjectAsJson);
	std::function<void()> m_onWebViewFirstInitialized;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_WEBVIEW2_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//CString ExecScript(ICoreWebView2* webview, CString script, DWORD timeout=10000);
	// CMFC_webview2Dlg message handlers
	//std::unique_ptr<CWebBrowser> m_pWebBrowser;
	CComPtr<ICoreWebView2Environment> m_webViewEnvironment;
	CComPtr<ICoreWebView2Controller> m_controller;
	CComPtr<ICoreWebView2_2> m_webView2_2;

	CComPtr<ICoreWebView2CookieManager> cookieManager;
	CComPtr<ICoreWebView2Cookie> myCookie;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


	CString GetUserAppData()
	{
		TCHAR outPath[MAX_PATH] = { 0 };
		LPITEMIDLIST ppidl = NULL;
		if (S_OK == SHGetSpecialFolderLocation(NULL, CSIDL_LOCAL_APPDATA, &ppidl))
		{
			SHGetPathFromIDList(ppidl, outPath);
			CoTaskMemFree(ppidl);
		}
		CString appFolder = outPath;

		
		DWORD size = 256;
		LPWSTR  szBuffer = (LPWSTR)malloc(256);
		CString strLoginOnUser;
		if (GetSessionUserName(strLoginOnUser) && GetUserName(szBuffer, &size))
		{
			CString runningUserFolder = szBuffer;
			if (runningUserFolder != strLoginOnUser)
			{
				runningUserFolder = _T("\\") + runningUserFolder + _T("\\");
				strLoginOnUser = _T("\\") + strLoginOnUser + _T("\\");
				appFolder.Replace(runningUserFolder, strLoginOnUser);
			}
		}
		//return appFolder;
		return _T("${LOCALAPPDATA}\\aaaaaa");
	}

	bool GetSessionUserName(CString& strUserName)
	{
		DWORD dwSessionId = WTSGetActiveConsoleSessionId();

		LPTSTR pBuffer = NULL;
		DWORD dwBufferLen;
		BOOL bRes = WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, dwSessionId, WTSUserName, &pBuffer, &dwBufferLen);
		if (bRes == FALSE)
		{
			return false;
		}

		CString schar = CString(pBuffer);
		USES_CONVERSION;
		strUserName = CString(T2A(schar));

		WTSFreeMemory(pBuffer);

		return true;
	}
};
