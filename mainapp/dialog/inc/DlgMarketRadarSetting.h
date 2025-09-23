#ifndef _DLGMARKETRADARSETTING_H_
#define _DLGMARKETRADARSETTING_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMarketRadarList.h : header file
//

#include "DialogEx.h"

#include "MarketRadar.h"
#include "DlgTimeSaleStatistic.h"

#include <map>
using std::map;
#include <set>
using std::set;

/////////////////////////////////////////////////////////////////////////////
// CDlgMarketRadarOption dialog

// 子对话框 - 雷达选项

class CDlgMarketRadarOption : public CDialogEx
{
public:
	CDlgMarketRadarOption(CWnd* pParent = NULL);   // standard constructor
	~CDlgMarketRadarOption();

	void	SetRadarSubParam(const T_MarketRadarSubParameter &SubParam);;
	const T_MarketRadarSubParameter &GetRadarSubParam() const { return m_SubParam; };
	
private:

	// Construction
protected:
	typedef map<UINT, UINT>  IdMap;

	void		PromptErrorInput(const CString &StrPrompt, CWnd *pWndFocus = NULL);


	BOOL		UpdateSubParam(BOOL bUpdate);		// TRUE-将控件中的值更新到param, FALSE-param中的值更新到控件

	void		UpdateCtrlStatus();		// 根据当前控件的选择，使能或禁止相关控件

	T_MarketRadarSubParameter	m_SubParam;

	bool32		m_bZoomData;

	static	IdMap	s_mapConditionIds;	// 所有条件的id与条件值的对应, key为控件id
	static	IdMap	s_mapConditionIdBindCtrls;	// 所有与条件控件绑定的其它控件, key为其它控件的id, value为对应的条件checkid

	static	IdMap	s_mapVoiceIds;		// 声音控件的id
	static	IdMap	s_mapVoiceIdBindCtrls;

public:
	// Dialog Data
	//{{AFX_DATA(CDlgMarketRadarOption)
	enum { IDD = IDD_DIALOG_MARKETRADAROPTION };
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMarketRadarOption)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgMarketRadarOption)
	afx_msg	void	OnBtnConditionClick(UINT nId);
	afx_msg void	OnRadioVoiceClick(UINT nId);
	afx_msg void	OnBtnFile();
	afx_msg void	OnBtnTestVoice();
	afx_msg void	OnResetToDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


// 子对话框 - 其它选项

class CDlgMarketRadarEtc : public CDialogEx
{
public:
	CDlgMarketRadarEtc(CWnd* pParent = NULL);   // standard constructor
	~CDlgMarketRadarEtc();
	
	void	SetRadarParam(const T_MarketRadarParameter &Param){ m_Param = Param; };
	const T_MarketRadarParameter &GetRadarParam() const { return m_Param; };
	
private:
	
	// Construction
protected:
	typedef map<UINT, UINT>  IdMap;
	typedef set<CMerch *> MerchSet;
	
	void		PromptErrorInput(const CString &StrPrompt, CWnd *pWndFocus = NULL);
	
	void		UpdateCtrlStatus();		// 根据当前控件的选择，使能或禁止相关控件

	void		FillList();

	CImageList	m_ImageList;
	
	T_MarketRadarParameter	m_Param;	// 商品范围列表

	static	MerchArray	s_aMerchRange;		// 自设商品列表, 模态对话框，一个就够了
	static	MerchSet	s_setMerchRange;

	static	IdMap	s_mapMerchRangeIds;	// 沪深a，自选，自设类型 控件id - 类型
	static	IdMap	s_mapMerchRangeBindCtrls;	// 相关的控件 相关控件 - radio控件
	
public:
	// Dialog Data
	//{{AFX_DATA(CDlgMarketRadarEtc)
	enum { IDD = IDD_DIALOG_MARKETRADARETC };
	CListCtrl	m_List;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMarketRadarEtc)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgMarketRadarEtc)
	afx_msg void	OnRadioClick(UINT nId);
	afx_msg void	OnBtnAdd();
	afx_msg void	OnBtnRemove();
	afx_msg void	OnBtnClear();
	afx_msg void	OnGetDispInfo(NMHDR *pHdr, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


// 设置对话框 - 包含两个子页面

class CDlgMarketRadarSetting : public CDialogEx
{
public:
	CDlgMarketRadarSetting(CWnd* pParent = NULL);   // standard constructor
	~CDlgMarketRadarSetting();
	
	void	SetRadarParam(const T_MarketRadarParameter &Param);;
	const T_MarketRadarParameter &GetRadarParam() const { return m_Param; };
	
private:
	
	// Construction
protected:
	
	void		PromptErrorInput(const CString &StrPrompt, CWnd *pWndFocus = NULL);
	
	void		UpdateCtrlStatus();		// 根据当前控件的选择，使能或禁止相关控件

	void		RecalcLayout();

	void		OnTabChanged();
	
	T_MarketRadarParameter	m_Param;	// 商品范围列表

	CDlgMarketRadarOption  m_dlgOption;
	CDlgMarketRadarEtc	   m_dlgEtc;
	
	// Dialog Data
	//{{AFX_DATA(CDlgMarketRadarSetting)
	enum { IDD = IDD_DIALOG_MARKETRADARSETTING };
	CTabWndCtrl	m_Tab;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMarketRadarSetting)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgMarketRadarSetting)
	afx_msg void	OnNMTabChange(NMHDR *pHdr, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif //_DLGMARKETRADARSETTING_H_