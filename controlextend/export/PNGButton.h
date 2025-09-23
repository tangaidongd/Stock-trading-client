#pragma once

#include <map>
#include "dllexport.h"
using std::map;
// CPNGButton
// 由于调用了GidPlus的 GdiplusStartup，所以不能在DllMain中使用此类

class CONTROL_EXPORT CPNGButton : public CBitmapButton
{
	DECLARE_DYNAMIC(CPNGButton)
	
	enum{
		EGray  = RGB(215,215,215),
		EBlack = RGB(0,0,0),
		EWhite = RGB(255,255,255),
		ELightBlue  = RGB(205,205,255)
	};

	enum E_ShowMode{   // 目前3层显示：background -> 图片 -> 文字，目前所有文字显示 单行 中对齐
		EShowText   = 1,    // 显示窗体文字 - 这个如果设置了，将会显示在最顶层
		EShowImage  = 2,    // 显示设定的状态图片 - 位于Backgroud上层
		EShowOuterFrame = 4,    // 最外面的边框 - 如设定，将会与文字同层
		EShowFocusFrame = 8, // 聚焦框 - 如设定，与文字同层
	};

public:
	CPNGButton();
	virtual ~CPNGButton();

public:
	// 4种状态： Normal, Press/Select, Focus, Disable
	void        SetTextColor(COLORREF clrNormal = EBlack,  COLORREF clrPress = EBlack, COLORREF clrFocus = EBlack, COLORREF clrDisable = EBlack); // 设置 4 种状态文字显示的颜色
	void        SetBackgroundColor(COLORREF clrNormal = EWhite, COLORREF clrPress = EWhite, COLORREF clrFocus = EWhite, COLORREF clrDisable = EWhite); // 设置 4 种状态背景显示的颜色
	void        SetOuterFrameColor(COLORREF clrNormal = EGray, COLORREF clrPress = EGray, COLORREF clrFocus = EGray, COLORREF clrDisable = EGray); // 设置 4 种状态背景显示的颜色
	void        SetFocusFrameColor(COLORREF clrNormal = ELightBlue, COLORREF clrPress = ELightBlue, COLORREF clrFocus = ELightBlue, COLORREF clrDisable = ELightBlue); // 设置 4 种状态背景显示的颜色

	int			SetOuterFrameWidth(int iNewWidth = 1);  // 设定外边框线条宽度
	int			SetFocusFrameWidth(int iNewWidth = 2);  // 设定聚焦框线条宽度

	// 这个函数应当在窗口创建后才能调用
	void		SetFontSize(int iFontSize); // 改变Font Height

	//void		SetDrawItemInterceptor();   // 当DrawItem时会发送指定消息给指定的窗口，如果消息返回TRUE，则DrawItem不会继续绘制

	// 设置当鼠标移动到按钮上方时，鼠标形状 - NULL时不设置鼠标形状
	void		SetCursorOnButton(HCURSOR hCursor);
	void		SetDrawRectOnButton(bool bNeedDrawRect = true);		// 是否需要在鼠标移到按钮上方时，绘制一个矩形标记
	void		SetDrawRectOnButtonColorAndWidth(COLORREF clr = RGB(255,216,176), int iNewWidth = 2);  // 矩形的颜色与宽度

	// 设置是否显示Default按钮标志
	void		SetDrawDefaultRect(bool bNeedDraw = true);

	bool		SetImages(LPCTSTR lpszResSection, UINT nImageNormal, UINT nImagePress = 0,  UINT nImageFocus = 0, UINT nImageDisable = 0); // 默认与 normal 一致，失败的话，有文字会显示文字
	bool		SetBitmaps(UINT nBmpNormal,  UINT nBmpPress = 0, UINT nBmpFocus = 0, UINT nBmpDisable = 0);

	UINT32			SetShowMode(UINT32 esmShow = EShowText|EShowOuterFrame|EShowFocusFrame|EShowImage );  // 设定显示的模式，一个或者多个E_ShowMode中的值，文字会覆盖图片，图片覆盖背景色


private:
	bool		InitialFont();
	void        FillMySolidRect(HDC hDC, LPRECT pRect, COLORREF clr);
	void		FrameMyRect(HDC hDC, LPRECT pRect, COLORREF clr, int iWidth, int iHeight);
	bool		SetStatusColor(int ODSStatus, COLORREF &clrText, COLORREF &clrBackground, COLORREF &clrOuterFrame, COLORREF &clrFocusFrame);

private:
	struct T_StatusData{
		COLORREF  m_clrText;           // 文字颜色
		COLORREF  m_clrBackground;		// 背景颜色
		COLORREF  m_clrOuterFrame;      // 最外层边框颜色
		COLORREF  m_clrFocusFrame;      // 聚焦框颜色
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
	bool		m_bOnButton;				// 鼠标是否在button上

	UINT32		m_nShowMode;				// 显示模式: OnlyShowText, OnlyShowImage, Both，如果什么都没有设置且StrText为空，则以BG=White,TC=Black显示WindowText

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


