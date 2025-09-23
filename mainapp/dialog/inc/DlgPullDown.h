#if !defined(AFX_DLGPULLDOWN_H__EE72E666_9350_4C7C_BED8_5E5C5836172F__INCLUDED_)
#define AFX_DLGPULLDOWN_H__EE72E666_9350_4C7C_BED8_5E5C5836172F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPullDown.h : header file
//

#include "NCButton.h"

#include <map>
using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CDlgPullDown dialog

class CDlgPullDown : public CDialog
{
// Construction
public:
	CDlgPullDown(CWnd* pParent = NULL);   // standard constructor

public:
	void SetShowButtons(const buttonContainerType& mapBtnDst, CRect &parentRct);
	void SetParentTitle(CString strTitle);
	CString GetParentTitle();

	void IncrementHeightAndWidth(int nIncrement, int nWidth);
	int GetHeight();
	int GetWidth();

	CRect GetRect();

	DWORD GetIndexSelectStockFlag() const;
	DWORD GetHotSelectStockFlag() const;
	E_DpnxState	GetCurrentCowBearState() const;
private:	
	// 获取当前鼠标所在的按钮
	int TButtonHitTest(CPoint point);

	inline CNCButton&	GetCNCButton(buttonContainerType& container, int iId)
	{
		buttonContainerType::iterator it = container.begin();
		FOR_EACH(container, it)
		{
			if ((*it).GetControlId() == iId)
			{
				return *it;
			}
		}
		//理论容器中应该存在
		//ASSERT(0);
		static CNCButton btn;
		return btn;
	}
private:
	buttonContainerType m_mapMoreBtn;	// 工具栏更多按钮下拉列表
	int			m_iXButtonHovering;		    // 标识鼠标进入按钮区域
	CRect		m_parentBtnRect;
	CWnd		*m_pParent;
	CWnd		*m_pPrevChild;				// 前一个显示的子菜单窗口指针
	CString		m_strParentTitle;// 父菜单的显示标题
	int			m_nWidth, m_nHeight;
	bool		m_bHoverMenu;
	CNCButton   *m_pHoverBtn;

// Dialog Data
	//{{AFX_DATA(CDlgPullDown)
	enum { IDD = IDD_DIALOG_PULLDOWN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPullDown)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPullDown)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPULLDOWN_H__EE72E666_9350_4C7C_BED8_5E5C5836172F__INCLUDED_)
