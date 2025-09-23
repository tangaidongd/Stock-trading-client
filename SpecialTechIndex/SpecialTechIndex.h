// SpecialTechIndex.h : main header file for the SpecialTechIndex DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSpecialTechIndexApp
// See SpecialTechIndex.cpp for the implementation of this class
//

class CSpecialTechIndexApp : public CWinApp
{
public:
	CSpecialTechIndexApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
