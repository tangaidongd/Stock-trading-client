#pragma once
#include "WndCef.h"

// CDlgTradeDownload �Ի���

class CDlgTradeDownload : public CDialog
{
	DECLARE_DYNAMIC(CDlgTradeDownload)

public:
	CDlgTradeDownload(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgTradeDownload();

private:
	void			AddButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	int				TButtonHitTest(CPoint point);	// ����Ƿ��ڰ�ť��

	void			ParseJson2TradeCfgParam(CString StrJsonValue, T_TradeCfgParam &stTrade);
private:

	void            DrawClientButton();

protected:	
	CFont			m_fontStaticText;
	CFont			m_fontCheckText;

private:

	CWndCef		m_wndCef;
	std::map<int, CNCButton> m_mapBtn;
	int			m_iXButtonHovering;			// ��ʶ�����밴ť����
	CRect		m_rcCaption;				// ������������
	CRect       m_rcRegister;				// ע��

	Image       *m_pImgMyTrade;

	HCURSOR		m_hArrowCursor;				//
	HCURSOR		m_hHandCursor;				//
public:
	
// �Ի�������
	enum { IDD = IDD_DIALOG_TRADE_DOWNLOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
