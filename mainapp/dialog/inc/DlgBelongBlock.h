#if !defined(AFX_DLGBELONGBLOCK_H__E6B8AC6C_F863_4F50_BFB5_3009AFF61254__INCLUDED_)
#define AFX_DLGBELONGBLOCK_H__E6B8AC6C_F863_4F50_BFB5_3009AFF61254__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBelongBlock.h : header file
//linhc 20100918添加多媒体定时器
#include "XTimer.h"
#include "BlockConfig.h"
#include "DialogEx.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgBelongBlock dialog
//linhc	20100904 添加用于显示所属性板块对话框
class CDlgBelongBlock : public CDialogEx,public CXTimerAgent
{
// Construction
public:
	CDlgBelongBlock(IN CMerch* pMerchXml, CWnd* pParent = NULL);   // standard constructor
    ~CDlgBelongBlock();
// Dialog Data
	//{{AFX_DATA(CDlgBelongBlock)
	enum { IDD = IDD_DIALOG_BELONGBLO };
	CListCtrl	m_ListRight;
	CListCtrl	m_ListLeft;
	BOOL m_bIsChangeBySel;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBelongBlock)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgBelongBlock)
	virtual void OnCancel();
	afx_msg void OnClose();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListBlLeft(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckButSel();
	//}}AFX_MSG
	afx_msg LRESULT OnProcessIoViewMerchChange(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
		// from CXTimerAgent
public:
	//linhc 20100918添加定时器
	virtual	void	OnXTimer(int32 iTimerId);

public:
    void   NotifyChangeMerch(IN CMerch& oMerch);//linhc 20100909
	void   DoDisplayEmpty();
private:
	bool   IsSameMerch(const CMerch& oMerchXml);
	void   DisplayDlgTitle();
	bool32 ExistBlockPre(const CString& strBlockId);
	void   DealDbClick(const int32& iBlockId);
	void   InsertSingleBlock(const CBlockLikeMarket& oBlock);
	void   InitializeListData();
	void   InitializeRightList();
	void   InitializeLeftBlockList();
	void   Initialize();
	CImageList m_ImageList;
	CMerchInfo m_oMerchInfo;//用于保存前一个显示的商品，相同则不再重复显示; 
	CMerch*    m_pMerchXml;
	CArray<CString,CString> m_ArriBlock;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBELONGBLOCK_H__E6B8AC6C_F863_4F50_BFB5_3009AFF61254__INCLUDED_)
