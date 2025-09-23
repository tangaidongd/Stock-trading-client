#ifndef _DLGMARKETRADARLIST_H_
#define _DLGMARKETRADARLIST_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMarketRadarList.h : header file
//

#include "DialogEx.h"

#include "MarketRadar.h"

#include "MerchManager.h"
#include "GmtTime.h"
#include <afxmt.h>
#include "BlockConfig.h"

#include <map>
using std::map;

/////////////////////////////////////////////////////////////////////////////
// CDlgMarketRadarList dialog

// 标记文字对话框
#ifdef _MYBASE_DLG_
#undef _MYBASE_DLG_
#endif
#define _MYBASE_DLG_ CDialogEx

class CDlgMarketRadarList : public _MYBASE_DLG_, public CMarketRadarListener, public CBlockConfigListener
{
public:
	~CDlgMarketRadarList();

	static	void	ShowMarketRadarList();		// 显示雷达列表

	virtual	 void OnAddMarketRadarResult(const MarketRadarResultMap &mapResult, E_AddCause eCause);	// 线程调用结果新的结果集, 异步
	virtual	 void OnRemoveMarketRadarResult(CMerch *pMerch, E_RemoveCause eCause);		// 结果被新的运算排除, 异步
	virtual  void OnClearMarketRadarResult();			// 结果集被清除

	virtual void  OnBlockConfigChange(int32 iBlockId, E_BlockNotifyType eNotifyType);
	virtual void  OnConfigInitialize(E_InitializeNotifyType eInitializeType){};
	
private:
	CDlgMarketRadarList(CWnd* pParent = NULL);   // standard constructor

	// Construction
protected:
	
	void		PromptErrorInput(const CString &StrPrompt, CWnd *pWndFocus = NULL);

	void		FillList();
	CMerch		*GetSelectMerch();
	void		SelectMerch(CMerch *pMerch);

	void		UpdateOpenCloseString();

	void		OnEnterKey();

	void		RecalcLayout();


	typedef map<CMerch *, CGmtTime> MerchTimeMap;
	MerchTimeMap	m_mapMerchDels;	// 需要删除的商品列表, 相隔若干时间后移除该计算结果

	CString			m_StrTitleOrg;	// 原来的标题

	MarketRadarResultArray	m_aResults;	// 给显示用的结果集合
	MarketRadarResultArray  m_aResAsync;	// 
	CCriticalSection	m_LockResult;

	CRect			m_rcList;
	CRect			m_rcOpenClose;
	CRect			m_rcSave;
	CRect			m_rcAnalyze;
	CRect			m_rcSetting;
	CRect			m_rcOrg;
	
	// Dialog Data
	//{{AFX_DATA(CDlgMarketRadarList)
	enum { IDD = IDD_DIALOG_MARKETRADAR };
	CListCtrl		  m_List;	// 列表显示
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMarketRadarList)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgMarketRadarList)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBtnOpenClose();
	afx_msg void OnBtnSave();
	afx_msg void OnBtnAnalyse();
	afx_msg void OnBtnSetting();
	afx_msg void OnMsgAddMerch();		// 有新的结果出现
	afx_msg LRESULT OnMsgRemoveMerch(WPARAM w, LPARAM l);	// 有结果不符合条件了
	afx_msg void	OnGetDispInfo(NMHDR *pHdr, LRESULT *pResult);
	afx_msg void	OnListCustomDraw(NMHDR *pHdr, LRESULT *pResult);
	afx_msg void	OnListDBLClick(NMHDR *pHdr, LRESULT *pResult);
	afx_msg void	OnListKeyDown(NMHDR *pHdr, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_DLGMARKETRADARLIST_H_