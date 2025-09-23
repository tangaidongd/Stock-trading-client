#if !defined(_DLGCUSTOMSTOCK_H_)
#define _DLGCUSTOMSTOCK_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgcustomstock.h : header file
//

#include "DialogEx.h"
#include "ReportScheme.h"
#include "SelectStockCenter.h"
#include "BlockManager.h"
#include "TextProgressCtrl.h"
#include <set>
#include <vector>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CDlgCustomStock dialog
class CDlgCustomStock : public CDialogEx
{
private:		
	// 填充字段列表控件
    void			FillListValue();
	
	// 所有商品列表框
	void			FillListBox();

	// 重设条件列表框
	void			ReSetListBoxCdt();

	// 清空进度条
	void			ReSetProgress();
	
	// 选股开始
	bool32			BeginChooseStock();																	
	
	// 停止选股(主动停止)
	bool32			StopChooseStock();
	
	// 选股完成(被动的选完后接受的通知)
	void			OnFinishChooseStock();
	
	// 验证参数合法性
	bool32			ValidChooseParams(OUT CString& StrErr);

private:
	// 是否在工作的标志.
	bool32					m_bWorking;
	
	// 指标公式中心
	CFormulaLib*			m_pFormulaLib;
	
	// 选股中心
	CSelectStockCenter*		m_pSelectStockCenter;

	// 股票最终选入的板块
	T_Block					m_BlockFinal;
	
	// 参加选股的商品范围
	set<CMerch*, MerchCmp>	m_aMerchToChoose;			
	
	// 最终确定的参加选股的各种条件
	CArray<T_CustomCdtCell, T_CustomCdtCell&>	m_aCustomConditions;

	// 图标
	CImageList				m_ImageList;

// Construction
public:
	CDlgCustomStock(CWnd* pParent = NULL);   // standard constructor
	~CDlgCustomStock();
// Dialog Data
	//{{AFX_DATA(CDlgCustomStock)
	enum { IDD = IDD_DIALOG_CUSTOM_STOCK };
	CTextProgressCtrl	m_Progress;
	CListCtrl	m_ListValues;
	CListBox	m_ListMerchs;
	CListBox	m_ListCondition;
	UINT	m_iMerchAll;
	UINT	m_iMerchChoose;
	int		m_iRadioAnd;
	int		m_iRadioValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCustomStock)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCustomStock)
	afx_msg void OnRadioValue();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonModify();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonChangeMerch();
	afx_msg void OnButtonDelAllMerch();
	afx_msg void OnButtonAddTo();
	afx_msg void OnButtonDo();	
	afx_msg void OnDblclkListCondition();
	afx_msg void OnDblclkListValue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg	LRESULT OnMsgProgress(WPARAM wParam , LPARAM lParam);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DLGCUSTOMSTOCK_H_)
