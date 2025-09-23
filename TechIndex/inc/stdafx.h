// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE 类
#include <afxodlgs.h>       // MFC OLE 对话框类
#include <afxdisp.h>        // MFC 自动化类
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC 数据库类
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO 数据库类
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4244)
#pragma warning(disable:4996)
#pragma warning(disable:4018)
#include "IndexMsgDef.h"

#define	MEMSET_ARRAY(arr, val) memset((void*)arr, 0, sizeof(arr))
#define ZERO_ARRAY(arr) MEMSET_ARRAY(arr, 0)
#define ARRAY_SIZE(arr) sizeof(arr)/sizeof(arr[0])
#define LESS_ARRARY_INDEX(arr, index) ((index) >= 0 && (index) < ARRAY_SIZE(arr))
#define LESS_INDEX(index, total) ((index)>=0 && (index)<(total))
#define FLOAT_IS_ZEROR(fVal) ((fVal)<0.000001&&(fVal)>-0.000001)