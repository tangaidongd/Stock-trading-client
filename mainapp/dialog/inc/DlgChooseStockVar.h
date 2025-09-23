#if !defined(AFX_DLGCHOOSESTOCKVAR_H_)
#define AFX_DLGCHOOSESTOCKVAR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgChooseStockVar.h : header file
//

#include "DialogEx.h"
#include "BlockConfig.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgChooseStockVar dialog

// 选择品种对话框

class CDlgChooseStockVar : public CDialogEx
{
public:
	typedef CBlockConfig::MerchArray MerchArray;
	
	// 弹出对话框 选定要从哪些板块中删除该商品
	// pInputMerch添加一个额外的与指定商品相关的板块列表
	static bool32 ChooseStockVar(OUT MerchArray &aMerchSel, bool32 bMultiMerchSel = true, CMerch *pInputMerch = NULL, CWnd *pWndParent=NULL); 
	
private:
	
	// Construction
protected:
	CDlgChooseStockVar(CWnd* pParent = NULL);   // standard constructor
	
	void	GetSelMerchs();			// 获取选择的商品

	void	SetInputMerch(CMerch *pMerch);
	
	void		FillBlockList();		// 填充左侧板块列表
	void		FillMerchList();		// 填充右侧商品列表

	void	UpdateMerchSrcChange();	// 板块选择状态变更
	void	ClearMerchSrc();		// 清除商品列表的数据
	
	bool32	m_bMultiMerchSel;		// 是否允许选择多个商品
	MerchArray	m_aMerchSel;		// 选择的商品列表结果

	CMerch	*m_pInputMerch;			// 输入的商品，会新建一个所属板块tab
	CBlockConfig::IdArray		m_aInputBelongToBlocks; // 输入商品相关的block的id

	// 左侧有可能是物理市场列表，或者板块列表
	bool32		m_bLeftIsMarket;	// true为物理市场，false为板块
	MerchArray  m_aMerchSrc;		// 右侧商品列表数据源

	CImageList	m_ImageList;
	
	// Dialog Data
	//{{AFX_DATA(CDlgChooseStockVar)
	enum { IDD = IDD_DIALOG_CHOOSESTOCK };
	CListCtrl	  m_ListBlock;
	CListCtrl	  m_ListMerch;
	CTabCtrl	  m_Tab;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChooseStockVar)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgChooseStockVar)
	afx_msg void OnTabChanged(NMHDR *pHdr, LRESULT *pResult);			// tab切换
	afx_msg void OnLeftListSelChanged(NMHDR *pHdr, LRESULT *pResult);	// 左侧选择变更
	afx_msg void OnGetDispInfo(NMHDR *pHdr, LRESULT *pResult);
	afx_msg void OnBtnSelAll();
	afx_msg LRESULT OnMsgHotKey(WPARAM w, LPARAM l);
	afx_msg void OnRightListLDbClick(NMHDR *pHdr, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHOOSESTOCKVAR_H_)