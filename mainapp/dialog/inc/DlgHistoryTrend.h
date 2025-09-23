#ifndef _DLG_HISTORY_TREND_H_
#define _DLG_HISTORY_TREND_H_


#include "IoViewMainTimeSale.h"
#include "GmtTime.h"
#include "DialogEx.h"

//////////////////////////////////////////////////////////////////////////
class CIoViewTimeSale;
class CIoViewTrend;
class CIoViewKLine;
class CMerch;
class CKLine;
/////////////////////////////////////////////////////////////////////////////
// CDlgHistoryTrend dialog used 

class CDlgHistoryTrend : public CDialogEx
{
	DECLARE_DYNCREATE(CDlgHistoryTrend)

	typedef CDialogEx	BaseDailog;
public:
	~CDlgHistoryTrend();

private:
	CDlgHistoryTrend(CWnd *pParent = NULL);
	
public:

	// 显示历史分时对话框 - kline视图不能为空，KLineSrc为当前的cross日线周期, 该函数模态对话框，返回IDOK/IDCANCEL
	static	int		ShowHistoryTrend(CIoViewKLine *pIoViewKLine, const CKLine &KLineSrc);
	
private:
	void	RecalcLayout();

	void	ChangeAttendTimeStart(const CGmtTime &TimeAttend);
	void	ChangeAttendTimeEnd();

	bool32	DoLButtonDown(CPoint pt);
	bool32	DoLButtonUp(CPoint pt);
	bool32	DoMouseMove(CPoint pt);
	
private:
	CIoViewTrend		*m_pIoViewTrend;	// 分时窗口
	CIoViewTimeSale		*m_pIoViewTimeSale;	// 分笔窗口

	bool32				m_bAttendTimeChanged;

	CPoint				m_ptLastDrawSplit;
	CRect				m_RectAxis;
	
	CMerch				*m_pAttendMerch;	// 关注商品
	CGmtTime			m_TimeHistory;		// 历史时间/日 - 可变动

	CSize				m_sizeTimeSale;		// 分笔宽度 - 仅宽度有用
	
	// Dialog Data
	//{{AFX_DATA(CDlgHistoryTrend)
	enum { IDD = IDD_DIALOG_MAIN_TIMESALE };		// 注意实际是用的一个空对话框
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgHistoryTrend)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CDlgHistoryTrend)
	afx_msg	void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnPaint();
	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);
	afx_msg void		OnDestroy();
	afx_msg BOOL		OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



#endif //_DLG_HISTORY_TREND_H_