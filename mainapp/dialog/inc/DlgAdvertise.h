#if !defined(AFX_DLGADVERTISE_H__43B85636_28F1_42FE_A0F4_B7C7EA7E3099__INCLUDED_)
#define AFX_DLGADVERTISE_H__43B85636_28F1_42FE_A0F4_B7C7EA7E3099__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////
//
#include "DialogEx.h"
#include "ImageEx.h"

class CDlgAdvertise : public CDialogEx
{
// Construction
public:
	CDlgAdvertise(CWnd* pParent = NULL);   // standard constructor
	~CDlgAdvertise();
// Dialog Data
	//{{AFX_DATA(CDlgIm)
	enum { IDD = IDD_DIALOG_ADVERTISE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
private:
	void SaveSysConfig();
private:	
	//2014-03-06 by cym
	ImageEx           *m_pKLineImage;
	//
	CString            m_strAdvText;
	//
	CWnd               *m_pWnd;
	CString            m_StrIconPath;
	UINT			   m_imageHeight;
	UINT               m_imageWidth;
	//
	bool               m_bUserReaded;
	bool               m_bHaveAdve;
	bool               m_bGifFileEixst;
	//
	bool               m_IsAllAreadly;
	//
	//k线广告对话框标题 2014-03-08 by cym
	T_NewsInfo  m_tKLineNewsInfo; 
	//
	bool               m_bShowContent;
	//
	CString m_StrWeb1;
	CString m_StrWeb2;

public:
	void  SetAdvText(CString strText, int iIndex);
	bool  RedrawAdvertise();
	void  SetAdvertiseOwner( CWnd *pWnd );
	//
	void SetImageFont(LOGFONT *pLogFont);
	//
	void GetDlgAdvertisePos(CRect &rect);
	//
	bool GetReadSTatus();
	void SetReadStatus(bool bBeRead);
	//
	void SetCheckShowTimer();
	//
	void AddBatchComment(IN listNewsInfo& listTitles);
	void InsertAtComment(T_NewsInfo &stFirst);
	void OnVDataNewsContentUpdate(const T_NewsInfo& stNewsInfo);
protected:
	void ShowKLineAdvertise();

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgIm)
public:
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
		virtual BOOL OnInitDialog();
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgIm)
	//afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint pt );
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIm_H__43B85636_28F1_42FE_A0F4_B7C7EA7E3088__INCLUDED_)
