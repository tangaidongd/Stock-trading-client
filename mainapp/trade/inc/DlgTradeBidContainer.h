#ifndef _DlgTradeBidContainer_H_
#define _DlgTradeBidContainer_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTradeBidContainer.h : header file
//
#include "GuiTabWnd.h"
#include "DlgTradeQuery.h"
#include "TradeLoginInterface.h"
#include "synch.h"
#include "ButtonST.h"
#include "BtnST.h"
#include <string>

#include "DlgTradeLogin.h"
#include "DlgTradeClose.h"
#include "DlgTradeOpen.h"
#include "DlgQuickOrder.h"
#include "BeautifulStatic.h"
#include "BeautifulTree.h"
#include "DialogChangePassword.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgTradeBidContainer dialog
#include "DialogEx.h"
#include "MenuEx.h"

#include <vector>
using std::vector;

enum E_TaderTabGroup
{
	Group_open = 0,		
	Group_close,
	Group_query,
	Group_none
};

class CDlgTradeBidContainer : public CDialog
{
private:

	// Construction
public:
	CDlgTradeBidContainer(CWnd* pParent = NULL, E_TradeLoginType eTpye = ETT_TRADE_FIRM);   // standard constructor
	~CDlgTradeBidContainer();

public:
	int				Create(CWnd *pParent) { return CDialog::Create(IDD, pParent); }

	CDlgTradeQuery	&GetQueryDlg() { return m_dlgQuery; }
	CDlgTradeOpen	&GetTradeOpenDlg() { return m_DlgOpen; }
	CDlgTradeClose	&GetTradeCloseDlg() { return m_DlgClose; }
	CDlgQuickOrder  &GetTradeQuickOrderDlg() { return m_DlgQuickOrder; }
	CDlgChangePwd   &GetChangePwdDlg(){return m_DlgChangePwd; }

	void SetGridHeadColor();

	void RecalcLayout();
private:
	void InitTree();
	CRect RecalcThreeStatic(const CRect& rect);	// 重新计算显示（占用保证金、浮动盈亏和当前权益3个静态文本框的位置）
private:
	// Dialog Data
	//{{AFX_DATA(CDlgTradeBidContainer)
	enum { IDD = IDD_DIALOG_TRADEBIDCONTAINER };
	//}}AFX_DATA
public:
	CDlgTradeQuery			m_dlgQuery;
	CDlgTradeClose			m_DlgClose;
	CDlgTradeOpen			m_DlgOpen;
	CDlgQuickOrder			m_DlgQuickOrder;
	CDlgChangePwd           m_DlgChangePwd;
	
 	T_TradeLoginInfo	*m_pTradeLoginInfo;

	// 实盘交易
	CBeautifulStatic	m_staticDeal;
	CBeautifulStatic	m_staticOccupy;
	CBeautifulStatic	m_staticOccupyValue;
	CBeautifulStatic	m_staticCurrentInsterest;
	CBeautifulStatic	m_staticCurrentInsterestValue;
	CBeautifulStatic	m_staticFloatGainLost;
	CBeautifulStatic	m_staticFloatGainLostValue;

	// 标题栏菜单
	CButtonSTMain	*m_pBtnRefresh;			//刷新
	CButtonSTMain	*m_pBtnCloseWnd;		//关闭

	CButtonSTMain	*m_pBtnMaxMin;			//缩放大小
	bool			m_bWndMaxMinStatus;		//缩放状态

	CButtonSTMain	*m_pBtnShift;			// 切换模拟和实盘交易

	CGuiTabWnd		m_GuiTab;				//TAB标签
	E_TaderTabGroup m_TabGroup;				//标签类型

	CFont			m_fontButton;			//菜单按钮字体
	
	//CButtonSTMain	*m_pBtnNetWorkStatus;	// 网络状态（连接）
	CMenuEx			m_mNetWorkStatus;		//状态弹出菜单

	//左侧导航树
	CBeautifulTree	*m_pTreeMenu;	
	CImageList		*m_pImagelist;
	bool			m_bShowMenu;			// 显示左菜单
	
	CArray<T_PlusInfo, T_PlusInfo>  m_aPlusInfo;
	int m_iSelectedServer;

	E_TradeLoginType m_eTradeLoginType;

public:
	void CreateButton();
	void ShowHideControl(int nCmdShow);

	void ShowHideModule(int nCmdShow);
	bool ToXml();
	bool FromXml();
	int	 GetServerID();
	void InitNetworkServer();
	void SetDefMaxMin();
	void ShowHideQuickOrder();
	void HideQuickOrder();
	bool IsVisibleQuickOrder(); // 快手下单界面是否可见
	void TradeOpen(CString str,bool bBuy);
	void ShowAccountInfo(T_CommUserInfoList *pList);	// 设置可用保证金、当前权益、浮动盈亏的值

private:	
	void	OnBtnOpen();
	void	OnBtnClose(E_TradeType eType);
	void	OnBtnQuery(E_QueryType eType);
	void	OnBtnChangePwd();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTradeBidContainer)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	
	// Implementation

public:	
	// Generated message map functions
	//{{AFX_MSG(CDlgTradeBidContainer)
	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);
	afx_msg void		OnSize(UINT nType, int cx, int cy);
	afx_msg	void		OnPaint();
	afx_msg void		OnBtnRefresh();
	afx_msg void		OnBtnMaxMin();
	afx_msg void		OnBtnShift();
	afx_msg HBRUSH		OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void		OnTvnSelchangedWebTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnClickTreeMenu(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnNetworkMenu(UINT nID);
	afx_msg void		OnBtnCloseWnd();
	afx_msg void		OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult); 

	LRESULT OnMsgEntrustTypeClose( WPARAM w, LPARAM l );
	LRESULT OnMsgCloseTrade( WPARAM w, LPARAM l );
	LRESULT OnMsgNetworkStatusChange( WPARAM w, LPARAM l );
	LRESULT OnHoldDetailClick( WPARAM w, LPARAM l );
        
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!_DlgTradeBidContainer_H_