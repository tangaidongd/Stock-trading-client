#if !defined(AFX_DLGRECENTMERCH_H__)
#define AFX_DLGRECENTMERCH_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRecentMerch.h : header file
//

#include "DialogEx.h"
#include "XScrollBar.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgRecentMerch dialog

class CDlgRecentMerch : public CDialogEx
{
public:
	bool32			CreateGrid();
	void			UpdateGrid();
	void			SetPosition();
	void			UpdateOneRow(int32 iIndex, IN CMerch *pMerch);
	int32			GetGridData(IN CAbsCenterManager * pAbsCenterManager, IN CMerch *pMerch, OUT CString &StrName, OUT float &fPriceNow, OUT float &fPreClose, OUT float &fRate, OUT int32 &iSaveDec);
protected:
	CGridCtrlSys    m_GridCtrl;
	CImageList		m_ImageList;
	CXScrollBar		m_XSBVert;
	CXScrollBar		m_XSBHorz;
	
	// Construction
public:
	CDlgRecentMerch(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgRecentMerch)
	enum { IDD = IDD_DIALOG_RECENTMERCH };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRecentMerch)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL
	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgRecentMerch)
	// NOTE: the ClassWizard will add member functions here
	afx_msg void OnClose();	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg LRESULT OnMsgRealtimeRecentMerchUpdate(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRECENTMERCH_H__)
