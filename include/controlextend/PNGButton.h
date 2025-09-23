#pragma once

#include <map>
#include "dllexport.h"
using std::map;
// CPNGButton
// ���ڵ�����GidPlus�� GdiplusStartup�����Բ�����DllMain��ʹ�ô���

class CONTROL_EXPORT CPNGButton : public CBitmapButton
{
	DECLARE_DYNAMIC(CPNGButton)
	
	enum{
		EGray  = RGB(215,215,215),
		EBlack = RGB(0,0,0),
		EWhite = RGB(255,255,255),
		ELightBlue  = RGB(205,205,255)
	};

	enum E_ShowMode{   // Ŀǰ3����ʾ��background -> ͼƬ -> ���֣�Ŀǰ����������ʾ ���� �ж���
		EShowText   = 1,    // ��ʾ�������� - �����������ˣ�������ʾ�����
		EShowImage  = 2,    // ��ʾ�趨��״̬ͼƬ - λ��Backgroud�ϲ�
		EShowOuterFrame = 4,    // ������ı߿� - ���趨������������ͬ��
		EShowFocusFrame = 8, // �۽��� - ���趨��������ͬ��
	};

public:
	CPNGButton();
	virtual ~CPNGButton();

public:
	// 4��״̬�� Normal, Press/Select, Focus, Disable
	void        SetTextColor(COLORREF clrNormal = EBlack,  COLORREF clrPress = EBlack, COLORREF clrFocus = EBlack, COLORREF clrDisable = EBlack); // ���� 4 ��״̬������ʾ����ɫ
	void        SetBackgroundColor(COLORREF clrNormal = EWhite, COLORREF clrPress = EWhite, COLORREF clrFocus = EWhite, COLORREF clrDisable = EWhite); // ���� 4 ��״̬������ʾ����ɫ
	void        SetOuterFrameColor(COLORREF clrNormal = EGray, COLORREF clrPress = EGray, COLORREF clrFocus = EGray, COLORREF clrDisable = EGray); // ���� 4 ��״̬������ʾ����ɫ
	void        SetFocusFrameColor(COLORREF clrNormal = ELightBlue, COLORREF clrPress = ELightBlue, COLORREF clrFocus = ELightBlue, COLORREF clrDisable = ELightBlue); // ���� 4 ��״̬������ʾ����ɫ

	int			SetOuterFrameWidth(int iNewWidth = 1);  // �趨��߿��������
	int			SetFocusFrameWidth(int iNewWidth = 2);  // �趨�۽����������

	// �������Ӧ���ڴ��ڴ�������ܵ���
	void		SetFontSize(int iFontSize); // �ı�Font Height

	//void		SetDrawItemInterceptor();   // ��DrawItemʱ�ᷢ��ָ����Ϣ��ָ���Ĵ��ڣ������Ϣ����TRUE����DrawItem�����������

	// ���õ�����ƶ�����ť�Ϸ�ʱ�������״ - NULLʱ�����������״
	void		SetCursorOnButton(HCURSOR hCursor);
	void		SetDrawRectOnButton(bool bNeedDrawRect = true);		// �Ƿ���Ҫ������Ƶ���ť�Ϸ�ʱ������һ�����α��
	void		SetDrawRectOnButtonColorAndWidth(COLORREF clr = RGB(255,216,176), int iNewWidth = 2);  // ���ε���ɫ����

	// �����Ƿ���ʾDefault��ť��־
	void		SetDrawDefaultRect(bool bNeedDraw = true);

	bool		SetImages(LPCTSTR lpszResSection, UINT nImageNormal, UINT nImagePress = 0,  UINT nImageFocus = 0, UINT nImageDisable = 0); // Ĭ���� normal һ�£�ʧ�ܵĻ��������ֻ���ʾ����
	bool		SetBitmaps(UINT nBmpNormal,  UINT nBmpPress = 0, UINT nBmpFocus = 0, UINT nBmpDisable = 0);

	UINT32			SetShowMode(UINT32 esmShow = EShowText|EShowOuterFrame|EShowFocusFrame|EShowImage );  // �趨��ʾ��ģʽ��һ�����߶��E_ShowMode�е�ֵ�����ֻḲ��ͼƬ��ͼƬ���Ǳ���ɫ


private:
	bool		InitialFont();
	void        FillMySolidRect(HDC hDC, LPRECT pRect, COLORREF clr);
	void		FrameMyRect(HDC hDC, LPRECT pRect, COLORREF clr, int iWidth, int iHeight);
	bool		SetStatusColor(int ODSStatus, COLORREF &clrText, COLORREF &clrBackground, COLORREF &clrOuterFrame, COLORREF &clrFocusFrame);

private:
	struct T_StatusData{
		COLORREF  m_clrText;           // ������ɫ
		COLORREF  m_clrBackground;		// ������ɫ
		COLORREF  m_clrOuterFrame;      // �����߿���ɫ
		COLORREF  m_clrFocusFrame;      // �۽�����ɫ
	};
	map<int, T_StatusData>		m_statusData;			

	int         m_iOuterFrameWidth;
	int			m_iFocusFrameWidth;

	bool		m_bNeedDrawRectOnButton;
	COLORREF	m_clrDrawRectOnButton;
	int			m_iWidthDrawRectOnButton;

	bool		m_bNeedDrawDefaultRect;

	CFont		m_font;

	HCURSOR		m_hCursor;
	bool		m_bOnButton;				// ����Ƿ���button��

	UINT32		m_nShowMode;				// ��ʾģʽ: OnlyShowText, OnlyShowImage, Both�����ʲô��û��������StrTextΪ�գ�����BG=White,TC=Black��ʾWindowText

protected:
	DECLARE_MESSAGE_MAP()

	virtual void PreSubclassWindow();

public:
	virtual BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);  // OwnerDraw
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM w, LPARAM l);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};


