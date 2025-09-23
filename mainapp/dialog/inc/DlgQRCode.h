#ifndef _DLGQRCODE_H_
#define _DLGQRCODE_H_


#include "DialogEx.h"


//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDlgQRCode dialog used 

class CDlgQRCode : public CDialogEx
{
	DECLARE_DYNCREATE(CDlgQRCode)
private:
	CDlgQRCode(CWnd *pParent = NULL);
	
public:
	static void		ShowPopupQRCode();	
	
private:
	void	RecalcLayout();
	
private:
	static	CDlgQRCode	*m_spThis;		// 


	static Image	*m_pImage;
	// Dialog Data
	//{{AFX_DATA(CDlgQRCode)
	enum { IDD = IDD_DIALOG_QR_CODE };			// 一个空的可缩放的对话框
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgQRCode)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	virtual void OnCancel();		// 直接关闭对话框
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CDlgQRCode)
	afx_msg	void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnPaint();
	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);
	afx_msg void		OnDestroy();
	afx_msg void		OnContextMenu(CWnd* pWnd, CPoint pos);	// 菜单
	afx_msg void		OnGetMinMaxInfo(MINMAXINFO* lpMMI);		// 限制最小大小
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



#endif	// _DLGQRCODE_H_