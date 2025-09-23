#pragma once


// CDlgSloganExit �Ի���
#include "NCButton.h"
#include "GdiPlusTS.h"

class CDlgSloganExit : public CDialog
{
	DECLARE_DYNAMIC(CDlgSloganExit)

	typedef enum	//	��껬������
	{
		e_mouse_lbd,
		e_mouse_lbu,
		e_mouse_leave,
		e_mouse_hover,
	}e_mouse_state;

public:
	CDlgSloganExit(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSloganExit();

// �Ի�������
	enum { IDD = IDD_DIALOG_SLOGAN_EXIT };
	
private:
	map<int, CNCButton> m_mapButton;
	Image*				m_pImgLogoLeft;		//	logoͼƬ
	Image*				m_pImgSlogan;		//	��������ͼƬ
	Image*				m_pImgClose;		//	x�ر�ͼƬ
	int					m_iXhoverId;	
	void	AddButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption);

	CString				m_StrAppName;		// Ӧ�ó�������
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int			GetHitTestControlID(CPoint pt);
	void		HitTest(int id, e_mouse_state state);
	BOOL				m_bReLogin;			//	�Ƿ��ظ���¼
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL OnInitDialog();	
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
