#if !defined(AFX_KEYBOARDDLG_H__28D9E918_0248_4979_BF05_C4723BBF6CD5__INCLUDED_)
#define AFX_KEYBOARDDLG_H__28D9E918_0248_4979_BF05_C4723BBF6CD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// keyboarddlg.h : header file
//
#include "listboxex.h"
#include "DialogEx.h"
#include "hotkey.h"
#include <set>

using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CKeyBoardDlg dialog

typedef struct T_MerchSortByPY
{
public:
	T_MerchSortByPY()
	{
		m_iPos	= 0;
		m_bUserCode	= false;
		m_iUserData = 0;;
	}

	//
	bool32 T_MerchSortByPY::operator < (const T_MerchSortByPY& stOther) const
	{
		if ( !m_bUserCode )
		{
			if ( m_iPos < stOther.m_iPos )
			{
				return true;			
			}
			else if ( m_iPos == stOther.m_iPos )
			{			
				int32 iCmp = m_HotKey.m_StrParam2.CompareNoCase(stOther.m_HotKey.m_StrParam2);

				if ( 0 == iCmp )
				{
					return m_iUserData < stOther.m_iUserData;
				}
				else
				{
					return (iCmp < 0);
				}
			}
		}
		else
		{
			if ( m_iPos < stOther.m_iPos )
			{
				return true;			
			}
			else if ( m_iPos == stOther.m_iPos )
			{
				int32 iCmp = m_HotKey.m_StrParam1.CompareNoCase(stOther.m_HotKey.m_StrParam1);

				if ( 0 == iCmp )
				{
					return m_iUserData < stOther.m_iUserData;
				}
				else
				{
					return (iCmp < 0);
				}				
			}
		}

		return false;
	}

	int32		m_iPos;
	bool32		m_bUserCode;		// 用代码排序的时候, "<" 的规则, 用代码, 不要用拼音
	CHotKey		m_HotKey;
	int32		m_iUserData;		// 有完全一样的情况下, 防止被 set 替换. 人为做一个索引

}T_MerchSortByPY;

class CKeyBoardDlg : public CDialogEx
{
// Construction
public:
	CKeyBoardDlg(CArray<CHotKey, CHotKey&>& arrHotKeys, CWnd* pParent = NULL,E_HotKeyType eHotKeyType = EHKTCount);
public:
	void	SetAutoCloseFlag(bool32 bAutoClose)				{ m_bAutoClose = bAutoClose;}
	bool32	SendInfomation(CHotKey & HotKey);
	void	SetStatusBarSign(bool bSign);
	void	SetActiveByClickSign(bool bSign)			   { m_bIsActiveByClickSign = bSign;}					// 点击激活方式
	void    SetEditText(CString szDipData);

	bool	CustomClose(CString strEditData);
public:
	MSG				m_Msg;

private:
	bool32			m_bAutoClose;	
	bool32          m_bStatusBarSign;						// 是否通过按键输入框调用
	bool32			m_bIsActiveByClickSign;					// 是否通过点击激活，避免直接退出
	CArray<CHotKey, CHotKey&>	m_HotKeyListSave;
	CArray<CHotKey, CHotKey&>	m_HotKeyListFromType;		// 使用eHotKeyType构造时使用的快捷键源列表

#ifdef _DEBUG
    virtual void AssertValid() const;    // Override
#endif


/////////////////////////////////////////////////////////////////////////////
//
// Dialog Data
	//{{AFX_DATA(CKeyBoardDlg)
	enum { IDD = IDD_KEYBOARD };
	CEdit			m_edit;
	CListBoxEx  	m_CtrlList;
	CString			m_StrText;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyBoardDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CKeyBoardDlg)
	afx_msg void OnClose();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnChangeText();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnUserSelect();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYBOARDDLG_H__28D9E918_0248_4979_BF05_C4723BBF6CD5__INCLUDED_)
