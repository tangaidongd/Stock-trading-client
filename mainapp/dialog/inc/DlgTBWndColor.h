#ifndef _DLGTBWNDCOLOR_H_
#define _DLGTBWNDCOLOR_H_

#pragma warning(push)
#include <map>


//#include "TBWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgTBWndColor dialog used 
class CTBWndColorValue
{
public:
	CTBWndColorValue(void  *pLParam = NULL, LPCTSTR lpszDesc=NULL, COLORREF clr=RGB(0,0,0))
	{
		m_pParam = pLParam;
		m_StrDesc = lpszDesc;
		m_clr	  = clr;
	}

	bool operator==(const CTBWndColorValue &v){
		return v.m_clr == m_clr && m_StrDesc == m_StrDesc;
	}
	void			*m_pParam;
	CString			m_StrDesc;
	COLORREF		m_clr;
};

typedef std::map<int, CTBWndColorValue >		TBWndColorItemMap;

class CDlgTBWndColor : public CDialog
{
public:
	CDlgTBWndColor(CWnd *pParent = NULL);
	
	int	 SetColorItem(const TBWndColorItemMap &mapItemsIn, TBWndColorItemMap *pMapItemsOut=NULL, LPCTSTR lpszTitle=NULL);		// 返回DoModal的返回，并且根据返回结果，设置mapItemsOut
	

	bool	 GetCurrentColor(COLORREF &clr);
	int		 SetCurrentColor(COLORREF  clr);		// 返回-1表示失败，否则返回上次的color

	TBWndColorItemMap	m_mapColorItems;
	CBrush				m_brhColor;
	CString				m_StrTitle;
	
	// Dialog Data
	//{{AFX_DATA(CDlgTBWndColor)
	enum { IDD = IDD_DIALOG_TBWND_COLOR };
	CComboBox		m_CtrlComboItem;
	// CString m_StrLMTime;
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTBWndColor)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CDlgTBWndColor)
	afx_msg	HBRUSH	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg	void	OnColorItemChange();
	afx_msg	void	OnColorItemSet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#pragma warning(pop)

#endif //_DLGTBWNDCOLOR_H_