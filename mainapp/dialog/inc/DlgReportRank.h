#ifndef _DLGREPORTRANK_H_
#define _DLGREPORTRANK_H_


#include "DialogEx.h"


#define  MAX_REPORTRANK_COUNT (9)			// 9个小视图


//////////////////////////////////////////////////////////////////////////

class CMerch;
class CIoViewReportRank;
/////////////////////////////////////////////////////////////////////////////
// CDlgReportRank dialog used 

class CDlgReportRank : public CDialogEx
{
	DECLARE_DYNCREATE(CDlgReportRank)
private:
	CDlgReportRank(CWnd *pParent = NULL);
	
public:
	static int32		ShowPopupReportRank(int32 iBlockId);		// 打开某分类板块，如果没有该板块，且没有打开过任何板块以沪深a替代, 
	static int32		ShowPopupReportRank(const CString &StrBlockName);	// 指定名字，返回id
	
private:
	void	RecalcLayout();

	bool32	OpenBlock(int32 iBlockId);		// 打开指定板块
	
private:
	static	CDlgReportRank	*m_spThis;		// 

	int32		m_iBlockId;		// 板块id

	typedef CArray<CIoViewReportRank *, CIoViewReportRank *>  SubIoViewArray;
	SubIoViewArray	m_aSubIoViews;

	CRect			m_RectTitle;
	
	// Dialog Data
	//{{AFX_DATA(CDlgReportRank)
	enum { IDD = IDD_DIALOG_MAIN_TIMESALE };			// 一个空的可缩放的对话框
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgReportRank)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	virtual void OnCancel();		// 直接关闭对话框
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CDlgReportRank)
	afx_msg	void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnPaint();
	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);
	afx_msg void		OnDestroy();
	afx_msg void		OnContextMenu(CWnd* pWnd, CPoint pos);	// 菜单
	afx_msg void		OnGetMinMaxInfo(MINMAXINFO* lpMMI);		// 限制最小大小
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



#endif	// _DLGREPORTRANK_H_