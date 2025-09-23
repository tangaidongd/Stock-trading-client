#ifndef _DLGREPORTRANK_H_
#define _DLGREPORTRANK_H_


#include "DialogEx.h"


#define  MAX_REPORTRANK_COUNT (9)			// 9��С��ͼ


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
	static int32		ShowPopupReportRank(int32 iBlockId);		// ��ĳ�����飬���û�иð�飬��û�д򿪹��κΰ���Ի���a���, 
	static int32		ShowPopupReportRank(const CString &StrBlockName);	// ָ�����֣�����id
	
private:
	void	RecalcLayout();

	bool32	OpenBlock(int32 iBlockId);		// ��ָ�����
	
private:
	static	CDlgReportRank	*m_spThis;		// 

	int32		m_iBlockId;		// ���id

	typedef CArray<CIoViewReportRank *, CIoViewReportRank *>  SubIoViewArray;
	SubIoViewArray	m_aSubIoViews;

	CRect			m_RectTitle;
	
	// Dialog Data
	//{{AFX_DATA(CDlgReportRank)
	enum { IDD = IDD_DIALOG_MAIN_TIMESALE };			// һ���յĿ����ŵĶԻ���
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgReportRank)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	virtual void OnCancel();		// ֱ�ӹرնԻ���
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CDlgReportRank)
	afx_msg	void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnPaint();
	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);
	afx_msg void		OnDestroy();
	afx_msg void		OnContextMenu(CWnd* pWnd, CPoint pos);	// �˵�
	afx_msg void		OnGetMinMaxInfo(MINMAXINFO* lpMMI);		// ������С��С
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



#endif	// _DLGREPORTRANK_H_