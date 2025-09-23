#if !defined(AFX_DLGTRENDINDEXSTATISTICS_H__)
#define AFX_DLGTRENDINDEXSTATISTICS_H__


#include "TrendIndex.h"
#include "GridCtrlSys.h"
#include "XScrollBar.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgtrendindexstatistics.h : header file
// 
#include "DialogEx.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgTrendIndexstatistics dialog
class CIoViewKLine;

class CDlgTrendIndexstatistics : public CDialogEx
{
public:
	void	SetParentIoView(CIoViewKLine* pParent);
	void	SetIndexValue(const CArray<T_TrendIndexNode,T_TrendIndexNode>& aBigCyleTrendIndexValue,const CTimeSpan TimeSpanBigCyle,const CArray<T_TrendIndexNode,T_TrendIndexNode>& aTrendIndexValue,int32 iBegin,int32 iEnd);

	void	SetPos();
	bool32	CreateTable();
	bool32	SetTableHead(int32 iIndex);
	void	UpdateTable();
	void	Initial();
private:
	CArray<T_TrendIndexNode,T_TrendIndexNode> m_aTrendIndexNode;		// 给的整条线的值
	CArray<T_TrendIndexNode,T_TrendIndexNode> m_aBigCyleTrendIndexNode;	// 给的整条线的值
	
	CIoViewKLine*							  m_pParentIoView;			// 父亲视图

	CGridCtrlSys							  m_GridCtrl;				// 数据显示表格
	CXScrollBar								  m_XSBVert;
	CXScrollBar							      m_XSBHorz;

	int32								      m_iHeadIndex;
	int32									  m_iBegin;					// 当前用于计算的
	int32									  m_iEnd;				
	int32									  m_iBeginInitial;			// 初始设置的当前画面的
	int32									  m_iEndInitial;
	bool32									  m_bStrart;	
	bool32									  m_bNewStatistica;			// 老王提出的新的统计方法
	CTimeSpan								  m_TimeSpanBigCyle;		
// Construction
public:
	CDlgTrendIndexstatistics(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTrendIndexstatistics)
	enum { IDD = IDD_DIALOG_TRENDINDEX };
	CEdit	m_Edit;
	CTime	m_TimeBgYMD;
	CTime	m_TimeEdHMS;
	CTime	m_TimeBgHMS;
	CTime	m_TimeEdYMD;
	int     m_iRadio;
	float	m_fStability;
	float	m_fStrong;
	float	m_fWeek;
	UINT	m_iMA13;
	UINT	m_iMA5;
	UINT	m_iMA8;
	UINT	m_uiCdt5KLineNums;
	float	m_fIntensityOC;
	float	m_fMA13QZ;
	float	m_fMA5QZ;
	float	m_fMA8QZ;
	BOOL	m_bCdt5;
	BOOL	m_bCdt2;
	BOOL	m_bCdt3;
	BOOL	m_bCdt4;
	float	m_fCdt5Times;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTrendIndexstatistics)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual	BOOL OnInitDialog();
	virtual void PostNcDestroy(); 
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTrendIndexstatistics)
	afx_msg void OnButtonStart();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRadio0();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnButtonConfirm();
	afx_msg void OnClose();	
	afx_msg void OnRadio3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTRENDINDEXSTATISTICS_H__)
