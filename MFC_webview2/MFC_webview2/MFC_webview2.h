
// MFC_webview2.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMFC_webview2App:
// See MFC_webview2.cpp for the implementation of this class
//

class CMFC_webview2App : public CWinApp
{
public:
	CMFC_webview2App();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMFC_webview2App theApp;