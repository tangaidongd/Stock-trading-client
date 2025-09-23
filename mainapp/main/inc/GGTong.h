// GGTong.h : main header file for the GGTONG application
//

#if !defined(AFX_GGTONG_H__9787184F_CE3E_4F74_B959_BC27651B0E80__INCLUDED_)
#define AFX_GGTONG_H__9787184F_CE3E_4F74_B959_BC27651B0E80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

       // main symbols
#include "MPIDocTemplate.h"
#include "ConfigInfo.h"
#include "client_app.h"
#include <locale.h>
//char* old_locale = _strdup( setlocale(LC_ALL,NULL) );    

class CGGTongApp;
class CGGTongDoc;
class CBindHotkey
{
public:
	CBindHotkey();
	~CBindHotkey();
	void Release();
	void AddRef();
private:
	static CGGTongApp* m_pApp;
friend class CGGTongApp;
};

extern HWND g_hwndTrace;

/////////////////////////////////////////////////////////////////////////////
// CGGTongApp:
// See GGTong.cpp for the implementation of this class
//
typedef struct T_WspFileInfo
{
	CString		m_StrFilePath;			// 文件路径
	CString		m_StrFileName;			// 文件名			 (真正的文件名)
	CString		m_StrFileVersion;		// 文件版本
	CString		m_StrFileXmlName;		// Xml 中描述的文件名(用于显示)
	bool32		m_bReadOnly;			// 只读属性，默认非只读(false)

	T_WspFileInfo();
	
	bool32		IsSpecialWsp() const ;			// 是否为特殊工作区
	CString		GetNormalWspFileVersion() const;	// 获取文件版本 - 特殊工作区的文件版本不能复制, so 使用该函数做下处理
}T_WspFileInfo;

class CGGTongApp : public CWinApp
{
public:
	CefRefPtr<ClientApp> m_cefApp;
public:
enum E_UserRightType
{
	EURTStock  = 0,			// 有股票权限的用户
	EURTFuture,				// 有期货权限的用户
	EURTAll,				// 两者都有的用户

	EURTCount,
};
public:
	CGGTongApp();
	~CGGTongApp();
	// GDI++
	bool32			LoadGdiPlus();
	void			RealeseGdiPlus();
	


	// 工作区相关
	void			ScanAvaliableWspFile(const CString& StrUserName, OUT bool32& bNewUser);	
	void			GetWspFileNameArray(CStringArray& aFileNames);
	void			GetWspFileXmlNameArray(CStringArray& aFileXmlNames);
	void			GetWspFilePathArray(CStringArray& aFilePaths);

    void            ReStart();

	CString			GetWspFilePathByXmlName(const CString& StrXmlName);
	CString			GetWspFilePathByFileName(const CString& StrXmlName);

	const T_WspFileInfo*	GetWspFileInfo(const char* KStrPath);
	const T_WspFileInfo*	GetWspFileInfo(const CString& KStrPath);

	static void		CopyPublicWCVFilesToPrivate(CString StrUserName = L"");
	
	// 取用户权限类型
	E_UserRightType GetUserRightType();

	//
	static CString	GetFilesVersion(LPTSTR StrFileName);							// 得到文件版本
	static CString	GetFilesLastModifyTime(LPTSTR StrFileName);						// 得到最新修改时间

public:
	CMPIDocTemplate *m_pDocTemplate;
	CGGTongDoc		*m_pDocument;
	bool32			 m_bOpenLast;	
	int32			 m_iBindHotkey;
	bool32			 m_bBindHotkey;
	int32			 m_iGroupID;	
	ULONG_PTR		 m_pGdiToken;

	//
	CString			 m_StrCurWsp;								// 当前的工作区(全路径)
	CArray<T_WspFileInfo, T_WspFileInfo&>	m_aWspFileInfo;		// 所有检测到的工作区文件

	static bool32	 m_bOffLine;								// 是否脱机登录

	CConfigInfo		*m_pConfigInfo;								// 系统配置
	bool32			m_bCustom;									// 是否游客

	CString			m_strNewsInfo1;								// 新闻资讯名称
	CString			m_strNewsInfo2;
	CString			m_strNewsInfo3;

	bool32			m_bTradeExit;								// 模拟交易已有行情账号登录
    bool32			m_bUserLoginExit;							// 点击主界面登录按钮
	bool32			m_HQRegisterExit;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGGTongApp)
	public:
	virtual BOOL InitInstance();
	virtual int	 ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle(LONG lCount);
	void		 SetCurrentMoudlePath();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CGGTongApp)
	afx_msg void OnAppAbout();
	afx_msg void OnAppResponsibility();

	// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void OnGlobalInit();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GGTONG_H__9787184F_CE3E_4F74_B959_BC27651B0E80__INCLUDED_)
