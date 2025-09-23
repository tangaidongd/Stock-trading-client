#if !defined(AFX_DLGNETWORK_H__)
#define AFX_DLGNETWORK_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgnetwork.h : header file
//
#include "XScrollBar.h"

#include "DialogEx.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgNetWork dialog
#define DLGNETWORKTIMER		2349653

#define PINGMIN				100
#define PINGMAX				1000	
	
struct T_ServiceObject;
class CDlgNetWork : public CDialogEx
{
// Construction
public:
	CDlgNetWork(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgNetWork)
	enum { IDD = IDD_DIALOG_NETWORK };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNetWork)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();	
	//}}AFX_VIRTUAL

	// Implementation
protected:
	CGridCtrlSys    m_GridCtrl;
	CImageList		m_ImageList;
	CXScrollBar		m_XSBVert;
	CXScrollBar		m_XSBHorz;
	
	bool32			m_bShowText;
	bool32			m_bShowAll;
	CStringArray	m_aStrTipsText;
	CStringArray	m_aStrTipsTitle;
	CArray<CAbsCommunicationShow*,CAbsCommunicationShow*> m_aCommunicationList;
public:
	bool32			CreateGrid();	
	void			ChangeShowStyle();
	void			AdjustToBestLayout(CRect rectClient);
	void			SetColumnColorAccordPingValue(int32 iCol,uint32 uiPing,bool32 bConnect);
	void			FillTable(CAbsCommunicationShow * pAbsCommunicationShow,int32 iMarketid,E_DataServiceType eDataServiceType,int32 iRow,int32 iCol);
protected:		
	// Generated message map functions
	//{{AFX_MSG(CDlgNetWork)
	// NOTE: the ClassWizard will add member functions here
	afx_msg void OnClose();	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
typedef struct T_ServiceObject
{
	CString					m_StrDataServiceName;
	E_DataServiceType		m_eDataServiceType;
	
	T_ServiceObject(const CString &StrDataServiceName,const E_DataServiceType& eDataServiceType)		
	{
		m_StrDataServiceName	= StrDataServiceName;
		m_eDataServiceType		= eDataServiceType;
	};
}T_ServiceObject;
typedef struct T_SpeedShowObject
{
	int32			m_iPingLow;
	int32			m_iPingHigh;	
	COLORREF		m_Color;
	
	T_SpeedShowObject(const int32 & iPingLow,const int32 & iPingHigh,const COLORREF & Color)
	{
		m_iPingLow	= iPingLow;
		m_iPingHigh	= iPingHigh;
		m_Color		= Color;		
	};
}T_SpeedShowObject;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNETWORK_H__)
