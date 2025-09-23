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
	CString		m_StrFilePath;			// �ļ�·��
	CString		m_StrFileName;			// �ļ���			 (�������ļ���)
	CString		m_StrFileVersion;		// �ļ��汾
	CString		m_StrFileXmlName;		// Xml ���������ļ���(������ʾ)
	bool32		m_bReadOnly;			// ֻ�����ԣ�Ĭ�Ϸ�ֻ��(false)

	T_WspFileInfo();
	
	bool32		IsSpecialWsp() const ;			// �Ƿ�Ϊ���⹤����
	CString		GetNormalWspFileVersion() const;	// ��ȡ�ļ��汾 - ���⹤�������ļ��汾���ܸ���, so ʹ�øú������´���
}T_WspFileInfo;

class CGGTongApp : public CWinApp
{
public:
	CefRefPtr<ClientApp> m_cefApp;
public:
enum E_UserRightType
{
	EURTStock  = 0,			// �й�ƱȨ�޵��û�
	EURTFuture,				// ���ڻ�Ȩ�޵��û�
	EURTAll,				// ���߶��е��û�

	EURTCount,
};
public:
	CGGTongApp();
	~CGGTongApp();
	// GDI++
	bool32			LoadGdiPlus();
	void			RealeseGdiPlus();
	


	// ���������
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
	
	// ȡ�û�Ȩ������
	E_UserRightType GetUserRightType();

	//
	static CString	GetFilesVersion(LPTSTR StrFileName);							// �õ��ļ��汾
	static CString	GetFilesLastModifyTime(LPTSTR StrFileName);						// �õ������޸�ʱ��

public:
	CMPIDocTemplate *m_pDocTemplate;
	CGGTongDoc		*m_pDocument;
	bool32			 m_bOpenLast;	
	int32			 m_iBindHotkey;
	bool32			 m_bBindHotkey;
	int32			 m_iGroupID;	
	ULONG_PTR		 m_pGdiToken;

	//
	CString			 m_StrCurWsp;								// ��ǰ�Ĺ�����(ȫ·��)
	CArray<T_WspFileInfo, T_WspFileInfo&>	m_aWspFileInfo;		// ���м�⵽�Ĺ������ļ�

	static bool32	 m_bOffLine;								// �Ƿ��ѻ���¼

	CConfigInfo		*m_pConfigInfo;								// ϵͳ����
	bool32			m_bCustom;									// �Ƿ��ο�

	CString			m_strNewsInfo1;								// ������Ѷ����
	CString			m_strNewsInfo2;
	CString			m_strNewsInfo3;

	bool32			m_bTradeExit;								// ģ�⽻�����������˺ŵ�¼
    bool32			m_bUserLoginExit;							// ����������¼��ť
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
