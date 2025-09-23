#pragma once
#include "WndCef.h"

// CDlgTradeDownload 对话框

class CDlgTradeDownload : public CDialog
{
	DECLARE_DYNAMIC(CDlgTradeDownload)

public:
	CDlgTradeDownload(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgTradeDownload();

private:
	void			AddButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	int				TButtonHitTest(CPoint point);	// 鼠标是否在按钮上

	void			ParseJson2TradeCfgParam(CString StrJsonValue, T_TradeCfgParam &stTrade);
private:

	void            DrawClientButton();

protected:	
	CFont			m_fontStaticText;
	CFont			m_fontCheckText;

private:

	CWndCef		m_wndCef;
	std::map<int, CNCButton> m_mapBtn;
	int			m_iXButtonHovering;			// 标识鼠标进入按钮区域
	CRect		m_rcCaption;				// 标题栏的区域
	CRect       m_rcRegister;				// 注册

	Image       *m_pImgMyTrade;

	HCURSOR		m_hArrowCursor;				//
	HCURSOR		m_hHandCursor;				//
public:
	
// 对话框数据
	enum { IDD = IDD_DIALOG_TRADE_DOWNLOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT  OnMsgDownloadTrade(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT  OnMsgShowWarnningWnd(WPARAM wParam, LPARAM lParam);

};
