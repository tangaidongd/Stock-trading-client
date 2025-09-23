#if !defined(_DLGUSERCYCLESET_H_)
#define _DLGUSERCYCLESET_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgusercycleset.h : header file
//
#include "DialogEx.h"
#include "IoViewBase.h"
#include "IoViewShare.h"
#include "IoViewKLine.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgUserCycleSet dialog

class CDlgUserCycleSet : public CDialog
{
public:
	void SetIoViewParent(CIoViewBase* pIoView)									{ ASSERT(NULL != pIoView); m_pParentIoView = pIoView; }
	void SetInitialParam(E_NodeTimeInterval eNodeInterval, UINT uiUserSet);		
private:
	CIoViewBase*		m_pParentIoView;
private:
    std::map<int, CNCButton> m_mapCheckBtn;
    std::map<int, CString> m_mapCheckItem;
    int		m_iXButtonHovering;			// 标识鼠标进入按钮区域
    CEdit	m_ePeriod;
    CFont	m_fontEditSmall;
	Image  *m_pImgBk;
	Image  *m_pImgCheckPeriod;
	Image  *m_pImgCheckPeriodOK;
public:
    enum ePeriodType
    {
        PT_1_MINUTE,
        PT_3_MINUTE,
        PT_5_MINUTE,
        PT_10_MINUTE,
        PT_15_MINUTE,
        PT_30_MINUTE,
        PT_60_MINUTE,
        PT_120_MINUTE,
        PT_180_MINUTE,
        PT_240_MINUTE,
        PT_DAY_LINE,
        PT_WEEK_LINE,
        PT_MONTH_LINE,
        PT_YEAR_LINE,
    };

private:
    void InitMapCheckItem();
    void InitCheckButton();
    int TCheckButtonHitTest(CPoint point);
    void AddCheckButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL, CString strHotImg=L"");
    void DrawCheckButton(CDC& dc);
    void SetPeriodInfo(UINT nID);

// Construction
public:
	CDlgUserCycleSet(CWnd* pParent = NULL);   // standard constructor
	~CDlgUserCycleSet();
// Dialog Data
	//{{AFX_DATA(CDlgUserCycleSet)
	enum { IDD = IDD_DIALOG_USERCYCLE_SET };
	//CString	m_StrPre;
	CString	m_StrAft;
    CString m_period;
	UINT	m_uiUserSet;
    UINT    m_uiOldUserSet;
    E_NodeTimeInterval	m_eNodeInterval;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgUserCycleSet)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
private:
    void InitKlineCheckPeriod();
    void SetChecPeriodBtnStatus(UINT nID);
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgUserCycleSet)
	afx_msg void OnClose();
    afx_msg void OnPaint();
	afx_msg void OnButtonConfirm();
	virtual BOOL OnInitDialog();
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void	OnEnKillFocusEditPeriod();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DLGUSERCYCLESET_H_)
