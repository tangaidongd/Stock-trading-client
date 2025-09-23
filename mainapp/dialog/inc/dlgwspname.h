#if !defined(AFX_DLGWSPNAME_H__E6E02034_ACFE_4D64_8374_4317C3A800DC__INCLUDED_)
#define AFX_DLGWSPNAME_H__E6E02034_ACFE_4D64_8374_4317C3A800DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgwspname.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWspName dialog
#include "dlgworkspaceset.h"
#include "listboxex.h"
#include "DialogEx.h"

class CDlgWspName : public CDialogEx
{
// Construction
public:
	CDlgWspName(CWnd* pParent = NULL);   // standard constructor

	enum E_DoneAction		// ���Ĳ��� - ��
	{
		DoneNone = 0,				// ���κβ���
		DoneAdd = 1,				// ������µ���
		DoneDelete = 2,				// ɾ������
	};

public:
	bool32			FindExistFile(CString StrFileName);	
	bool32			IsSpecialWspName(CString StrFileName);
	void			SetNewFileType(CDlgWorkSpaceSet::E_FileType eFileType);
	void			GetCurrentView(CGGTongView * pView);	
	//void			ReWriteFile();
	//void	        SaveFile(const CString& StrFilePath);
	void			ReMoveFile();

	
	CString			m_StrSelectedWspPath;	// �п���ѡ������Ѵ��ڵĹ�������������Ѵ��ڵģ����¼�Ѵ��ڵĹ�����·��, �����
	DWORD			m_dwDoneAction;		// �öԻ�����������ʲô����
	

protected:
	CGGTongView *   m_pView;	
	CStringArray    m_aFileExist;
	CArray<T_WspFileInfo, T_WspFileInfo &>    m_aWspSpecial;		// ���⹤����·��
	CDlgWorkSpaceSet::E_FileType m_eFileType;
public:
// Dialog Data
	//{{AFX_DATA(CDlgWspName)
	enum { IDD = IDD_DIALOG_WSPNAME };
	CEdit	m_edit;
	CListBoxEx	m_ListExist;
	CString	m_StrName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWspName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWspName)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListExist();	
	afx_msg void OnButtonDel();
	virtual void OnOK();	
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWSPNAME_H__E6E02034_ACFE_4D64_8374_4317C3A800DC__INCLUDED_)
