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

// ������ֶԻ���
#ifdef _MYBASE_DLG_
#undef _MYBASE_DLG_
#endif
#define _MYBASE_DLG_ CDialogEx

class CDlgMarketRadarList : public _MYBASE_DLG_, public CMarketRadarListener, public CBlockConfigListener
{
public:
	~CDlgMarketRadarList();

	static	void	ShowMarketRadarList();		// ��ʾ�״��б�

	virtual	 void OnAddMarketRadarResult(const MarketRadarResultMap &mapResult, E_AddCause eCause);	// �̵߳��ý���µĽ����, �첽
	virtual	 void OnRemoveMarketRadarResult(CMerch *pMerch, E_RemoveCause eCause);		// ������µ������ų�, �첽
	virtual  void OnClearMarketRadarResult();			// ����������

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
	MerchTimeMap	m_mapMerchDels;	// ��Ҫɾ������Ʒ�б�, �������ʱ����Ƴ��ü�����

	CString			m_StrTitleOrg;	// ԭ���ı���

	MarketRadarResultArray	m_aResults;	// ����ʾ�õĽ������
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
	CListCtrl		  m_List;	// �б���ʾ
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
	afx_msg void OnMsgAddMerch();		// ���µĽ������
	afx_msg LRESULT OnMsgRemoveMerch(WPARAM w, LPARAM l);	// �н��������������
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