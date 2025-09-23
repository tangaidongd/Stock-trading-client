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

// �ӶԻ��� - �״�ѡ��

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


	BOOL		UpdateSubParam(BOOL bUpdate);		// TRUE-���ؼ��е�ֵ���µ�param, FALSE-param�е�ֵ���µ��ؼ�

	void		UpdateCtrlStatus();		// ���ݵ�ǰ�ؼ���ѡ��ʹ�ܻ��ֹ��ؿؼ�

	T_MarketRadarSubParameter	m_SubParam;

	bool32		m_bZoomData;

	static	IdMap	s_mapConditionIds;	// ����������id������ֵ�Ķ�Ӧ, keyΪ�ؼ�id
	static	IdMap	s_mapConditionIdBindCtrls;	// �����������ؼ��󶨵������ؼ�, keyΪ�����ؼ���id, valueΪ��Ӧ������checkid

	static	IdMap	s_mapVoiceIds;		// �����ؼ���id
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


// �ӶԻ��� - ����ѡ��

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
	
	void		UpdateCtrlStatus();		// ���ݵ�ǰ�ؼ���ѡ��ʹ�ܻ��ֹ��ؿؼ�

	void		FillList();

	CImageList	m_ImageList;
	
	T_MarketRadarParameter	m_Param;	// ��Ʒ��Χ�б�

	static	MerchArray	s_aMerchRange;		// ������Ʒ�б�, ģ̬�Ի���һ���͹���
	static	MerchSet	s_setMerchRange;

	static	IdMap	s_mapMerchRangeIds;	// ����a����ѡ���������� �ؼ�id - ����
	static	IdMap	s_mapMerchRangeBindCtrls;	// ��صĿؼ� ��ؿؼ� - radio�ؼ�
	
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


// ���öԻ��� - ����������ҳ��

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
	
	void		UpdateCtrlStatus();		// ���ݵ�ǰ�ؼ���ѡ��ʹ�ܻ��ֹ��ؿؼ�

	void		RecalcLayout();

	void		OnTabChanged();
	
	T_MarketRadarParameter	m_Param;	// ��Ʒ��Χ�б�

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