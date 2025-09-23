#if !defined(AFX_DLGSYSTEMFACE_H__B7F60925_BE17_4D16_BA22_6B32BA2C68FE__INCLUDED_)
#define AFX_DLGSYSTEMFACE_H__B7F60925_BE17_4D16_BA22_6B32BA2C68FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgsystemface.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDlgSystemFace dialog

#include "staticsetcolor.h"
#include "facescheme.h"
#include "MPIChildFrame.h"
#include "DialogEx.h"
 
class CDlgSystemFace: public CDialog
{
// Construction
public:
	CDlgSystemFace(CWnd* pParent = NULL);   // standard constructor
public:
	void			SetCurrentColor(COLORREF clr);
	void			SaveSysFace();
	void			SaveSysFont(E_SysFont eFontType);
	void			RefreshPicture();
	void			SetActiveIoView(CIoViewBase* pIoViewActive) { ASSERT(NULL!=pIoViewActive); m_pIoViewActive = pIoViewActive; }
	CMPIChildFrame* GetCurrentChildFrame();
public:
// Dialog Data
	//{{AFX_DATA(CDlgSystemFace)
	enum { IDD = IDD_DIALOG_SYSFACE };
	CButton	m_CtrlStaticPreView;
	CButton	m_CtrlStaticColor;
	CStaticSetColor	m_CtrlStaticColorSkyBlue;
	CStaticSetColor	m_CtrlStaticColorPink;
	CStaticSetColor	m_CtrlStaticChartAxisLine;
	CStaticSetColor	m_CtrlStaticColorNavyBlue;
	CStaticSetColor	m_CtrlStaticColorDodgerBlue;
	CStaticSetColor	m_CtrlStaticColorGreenYellow;
	CStaticSetColor	m_CtrlStaticKLineFall;
	CStaticSetColor	m_CtrlStaticColorPurple;
	CStaticSetColor	m_CtrlStaticText;
	CStaticSetColor	m_CtrlStaticKLineKeep;
	CStaticSetColor	m_CtrlStaticGridLine;
	CStaticSetColor	m_CtrlStaticAmount;
	CStaticSetColor	m_CtrlStaticVolume;
	CStaticSetColor	m_CtrlStaticColorMagenta;
	CStaticSetColor	m_CtrlStaticColorCadetblue;
	CStaticSetColor	m_CtrlStaticBackground;
	CStaticSetColor	m_CtrlStaticColorBlue;
	CStaticSetColor	m_CtrlStaticGridSelected;
	CStaticSetColor	m_CtrlStaticColorYellow;
	CStaticSetColor	m_CtrlStaticKLineRise;
	CListBox		m_CtrlList;
	CComboBox		m_CtrlCombo;
	int				m_iRadio;
	//}}AFX_DATA
protected:
	CRect      m_rectPreView;
	CIoViewBase*	m_pIoViewActive;

public:
	//////////////////////////////////////////////////////////////////////////
	// 颜色
	COLORREF		 m_aSysColor[ESCCount];		
	int32			 m_iStyleIndex;
	CArray<T_SysColorSave,T_SysColorSave> m_aSysColorSave;		 

	COLORREF		 m_aSysColorBk[ESCCount];	    // 备份,用于用户选择"自定义	"项目的时候,还原数据
	int32			 m_iStyleIndexBk;
	CArray<T_SysColorSave,T_SysColorSave> m_aSysColorSaveBk;		 

	//////////////////////////////////////////////////////////////////////////
	//字体

	CFontNode	     m_aSysFont[ESFCount];
	CArray<T_SysFontSave,T_SysFontSave>	 m_aSysFontSave;
public:
	const T_SysColorSave & GetArraySysColorSave(int32 iIndex);
	const int32			   GetArraySysColorSaveSize();
	//CString				   TranslateToESysColorString(UINT uiSysColor);
public:   
	COLORREF   m_CurrentColor;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSystemFace)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSystemFace)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonColor();
	afx_msg void OnButtonFontReset();
	afx_msg void OnPaint();
	afx_msg void OnSelchangeList1();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnButtonSet();
	afx_msg void OnButtonBigfont();
	afx_msg void OnButtonNormalfont();
	afx_msg void OnButtonSmallfont();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	LRESULT OnSetColor(WPARAM wParam,LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSYSTEMFACE_H__B7F60925_BE17_4D16_BA22_6B32BA2C68FE__INCLUDED_)
