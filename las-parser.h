
// las-parser.h : main header file for the las-parser application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CLasParserApp:
// See las-parser.cpp for the implementation of this class
//

class CLasParserApp : public CWinApp
{
public:
	CLasParserApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CLasParserApp theApp;
