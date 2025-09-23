// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__E75FF41A_3E91_434F_86C6_463DA7B5A9FB__INCLUDED_)
#define AFX_STDAFX_H__E75FF41A_3E91_434F_86C6_463DA7B5A9FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#define _WIN32_IE 0x0700

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning(disable:4786)
#pragma warning(disable:4146)

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxtempl.h>
#include <comdef.h>//初始化一下com口

#include "resource.h"

#include "GGTong.h"
#include "GGTongDoc.h"
#include "MainFrm.h"
#include "faceschemetype.h"
#include "typedef.h"
#include "ConstVal.h"
#include "EngineCenterBase.h"
#include "MainMsgDef.h"
#include "CenterMsgDef.h"
#include "ControlMsgDef.h"
#include "IndexMsgDef.h"
#include "ChartMsgDef.h"

#include <GdiPlus.h>
using namespace Gdiplus;

#pragma warning(error:4101)
#pragma warning(error:4700)
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E75FF41A_3E91_434F_86C6_463DA7B5A9FB__INCLUDED_)
