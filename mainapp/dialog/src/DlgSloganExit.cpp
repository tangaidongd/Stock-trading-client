// DlgSloganExit.cpp : 实现文件
//

#include "stdafx.h"
#include "GGTong.h"
#include "DlgSloganExit.h"
#include "PathFactory.h"
#include "FontFactory.h"
const int32 KiCatpionHeight			=		32;
const int32 KiButtomHeight			=		60;
const RECT KrtBorder				=		{1,1,1,1};
const COLORREF KclrBorderColor		=		RGB(44,52,62);
const COLORREF KclrCationColor		=		RGB(44,52,62);
const COLORREF KclrButtomColor		=		RGB(255,255,255);
const COLORREF KclrCaptionTextColor =		RGB(255,255,255);

namespace
{
	#define IDC_RELOGIN_ID		 WM_USER+1
	#define IDC_CANCEL_ID		 WM_USER+2
	#define IDC_EXIT_ID			 WM_USER+3
	#define IDC_CLOSE_ID		 WM_USER+4
}
// CDlgSloganExit 对话框

IMPLEMENT_DYNAMIC(CDlgSloganExit, CDialog)

CDlgSloganExit::CDlgSloganExit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSloganExit::IDD, pParent)
{

	m_pImgLogoLeft = Image::FromFile(CPathFactory::GetImagePath()  +L"loginLogo.png");
	m_pImgSlogan = Image::FromFile(CPathFactory::GetImagePath() + L"Slogan.png");
	m_pImgClose = Image::FromFile(CPathFactory::GetImagePath() + L"exit.png");
	m_iXhoverId =-1;
	m_bReLogin =	FALSE;
}

CDlgSloganExit::~CDlgSloganExit()
{
	DEL(m_pImgLogoLeft);
	DEL(m_pImgSlogan);
	DEL(m_pImgClose);
}

void CDlgSloganExit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSloganExit, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()	
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CDlgSloganExit 消息处理程序
BOOL CDlgSloganExit::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if(pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgSloganExit::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rect;
	GetClientRect(&rect);

	CDC* pDC =&dc;
	CBitmap bitmap;	
	CDC MemeDc;

	MemeDc.CreateCompatibleDC(pDC);	
	bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());

	CBitmap *pOldBitmap = MemeDc.SelectObject(&bitmap);
	Graphics graphics(MemeDc.m_hDC);
	Color color;
	//绘制标题栏
	{
		CRect rtCation(rect);
		rtCation.DeflateRect(&KrtBorder);
		rtCation.bottom = rtCation.top + KiCatpionHeight;
		
		color.SetFromCOLORREF(KclrCationColor);
		SolidBrush brCaption(color);
		graphics.FillRectangle(&brCaption, rtCation.left, rtCation.top, rtCation.Width(), rtCation.Height());

		int iMageW =0;
		//draw icon
		if(NULL != m_pImgLogoLeft)
		{
			graphics.DrawImage(m_pImgLogoLeft, rtCation.left + 5, rtCation.top + (rtCation.Height() - m_pImgLogoLeft->GetHeight()) /2 - 2,18, 18);
			iMageW = m_pImgLogoLeft->GetWidth();
		}
		
		//draw text		


		rtCation.left += iMageW + 10;
		rtCation.top -= 2;
		MemeDc.SetBkMode(TRANSPARENT);
		CFont* pFont = pDC->GetCurrentFont();
		LOGFONT lf;
		pFont->GetLogFont(&lf);
		CFontFactory ff;
		_tcscpy(lf.lfFaceName, ff.GetExistFontName(L"宋体"));
		lf.lfWeight = FW_BOLD;
		lf.lfHeight = 16;
		
		CFont font;
		font.CreateFontIndirect(&lf);
		HGDIOBJ hOld = MemeDc.SelectObject(&font);
		MemeDc.SetTextColor(KclrCaptionTextColor);
		MemeDc.DrawText(m_StrAppName, -1, &rtCation, DT_VCENTER|DT_SINGLELINE );
		MemeDc.SelectObject(hOld);

	}

	//	宣传区
	{
		if(NULL != m_pImgSlogan)
		{
			CRect rtSlogan(rect);
			rtSlogan.DeflateRect(KrtBorder.left, KrtBorder.top, KrtBorder.right, KrtBorder.bottom);
			rtSlogan.DeflateRect(0, KiCatpionHeight, 0, KiButtomHeight);
			graphics.DrawImage(m_pImgSlogan, rtSlogan.left, rtSlogan.top, m_pImgSlogan->GetWidth(), m_pImgSlogan->GetHeight());
		}
	}
	
	//底部区域
	{
		CRect rtBottom(rect);
		rtBottom.DeflateRect(KrtBorder.left, KrtBorder.top, KrtBorder.right, KrtBorder.bottom);
		rtBottom.top = rtBottom.bottom - KiButtomHeight;
		color.SetFromCOLORREF(KclrButtomColor);
		SolidBrush btmBr(color);
		graphics.FillRectangle(&btmBr, rtBottom.left, rtBottom.top, rtBottom.Width(), rtBottom.Height() );
	}
	
	

	//边框
	{
		color.SetFromCOLORREF(KclrBorderColor);
		Pen penBorder(color, 1.0f);
		graphics.DrawRectangle(&penBorder, 0, 0, rect.Width() -KrtBorder.left , rect.Height() -KrtBorder.bottom );

	}
	

	//	绘制按钮
	CRect rcPaint;
	pDC->GetClipBox(&rcPaint);
	map<int, CNCButton>::iterator iter;
	CRect rcControl;

	// 遍历所有按钮
	for (iter=m_mapButton.begin(); iter!=m_mapButton.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		btnControl.GetRect(rcControl);

		// 判断当前按钮是否需要重绘
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}

		btnControl.DrawButton(&graphics);
	}


	pDC->BitBlt( rect.left, rect.top, rect.Width(), rect.Height(), &MemeDc, 0, 0, SRCCOPY);

	MemeDc.SelectObject(pOldBitmap);
	MemeDc.DeleteDC();
	bitmap.DeleteObject();
}

BOOL CDlgSloganExit::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

void CDlgSloganExit::OnLButtonDown(UINT nFlags, CPoint point)
{
	int id = GetHitTestControlID(point);
	if (id != -1)
	{
		HitTest(id, e_mouse_lbd);
		return;
	}
	CRect rect;
	GetClientRect(rect);
	rect.bottom = KiCatpionHeight;
	if(rect.PtInRect(point))
	{
		PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y)); 
	}
}

void CDlgSloganExit::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	HitTest(GetHitTestControlID(point), e_mouse_lbu);
}

void CDlgSloganExit::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	HitTest(GetHitTestControlID(point), e_mouse_hover);
}

void CDlgSloganExit::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	HitTest(m_iXhoverId, e_mouse_leave);
}

void CDlgSloganExit::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int id = GetHitTestControlID(point);
	if (id != -1)
	{
		if (m_iXhoverId != id)
		{
			HitTest(m_iXhoverId, e_mouse_leave);
			HitTest(id, e_mouse_hover);
		}		
		m_iXhoverId = id;
	}	
	else
	{
		HitTest(m_iXhoverId, e_mouse_leave);
		m_iXhoverId = -1;
	}
}

BOOL CDlgSloganExit::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (wParam == IDC_CLOSE_ID)
	{
		OnCancel();
		return true;
	}
	if (wParam == IDC_CANCEL_ID)
	{
		OnCancel();
		return true;
	}
	if (wParam == IDC_RELOGIN_ID)
	{
		m_bReLogin = TRUE;
		OnOK();

		return true;
	}
	if (wParam == IDC_EXIT_ID)
	{
		OnOK();
		return true;
	}
	return CDialog::OnCommand(wParam, lParam);
}

BOOL CDlgSloganExit::OnInitDialog()
{
	CDialog::OnInitDialog();

	CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();	
	m_StrAppName = pApp->m_pszAppName;

	// TODO:  在此添加额外的初始化
	if (NULL != m_pImgSlogan)
	{
		int iWndW = KrtBorder.left + KrtBorder.right + m_pImgSlogan->GetWidth();
		int iWndH = KrtBorder.top + KrtBorder.bottom + m_pImgSlogan->GetHeight() + KiButtomHeight + KiCatpionHeight;
		SetWindowPos(NULL, 0, 0, iWndW, iWndH, SWP_NOZORDER|SWP_NOMOVE);
	}

	CRect rt;
	GetClientRect(&rt);

	//close
	{		
		if (NULL != m_pImgClose)
		{	
			CRect rtClose(rt);
			rtClose.DeflateRect(0,4,4,0);
			rtClose.DeflateRect(&KrtBorder);
			rtClose.left = rtClose.right  - m_pImgClose->GetWidth();
			rtClose.top -= 3;
			rtClose.bottom = rtClose.top + m_pImgClose->GetHeight()/3;
			AddButton(&rtClose, m_pImgClose, 3, IDC_CLOSE_ID, L"");
		}		
	}

	//exit
	{
		int iControlHeight = 32;
		int iControlWidth  = 92;	
		int iControlSpace  = 20;
		CRect rtBottom(rt);
		rtBottom.DeflateRect(&KrtBorder);		
		rtBottom.top = rt.bottom - KiButtomHeight;

		int iXPos = rtBottom.left+ (rtBottom.Width() -2 * iControlSpace - 3 * iControlWidth)/2;
		int iYPos =  rtBottom.top + (rtBottom.Height() - iControlHeight )/2;

		CRect rtLogin( iXPos, iYPos, iXPos + iControlWidth, iYPos + iControlHeight);
		AddButton(&rtLogin, NULL, 0, IDC_RELOGIN_ID, L"重新登录");
		CRect rtExit(rtLogin);
		rtExit.OffsetRect(iControlSpace + iControlWidth, 0);
		AddButton(&rtExit, NULL, 0, IDC_EXIT_ID, L"退出");
		CRect rtCancel(rtExit);
		rtCancel.OffsetRect(iControlSpace + iControlWidth, 0);
		AddButton(&rtCancel, NULL, 0, IDC_CANCEL_ID, L"取消");
	}
	//relogin

	//cancel
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

int	CDlgSloganExit::GetHitTestControlID(CPoint pt)
{
	for (map<int, CNCButton>::iterator it = m_mapButton.begin(); it != m_mapButton.end(); ++it)
	{
		if (it->second.PtInButton(pt))
		{
			return it->second.GetControlId();
		}
	}

	return -1;
}

void CDlgSloganExit::HitTest(int id, e_mouse_state state)
{
	if (id == -1)
	{
		return;
	}
	for (map<int, CNCButton>::iterator it = m_mapButton.begin(); it != m_mapButton.end(); ++it)
	{	

		if (it->second.GetControlId() == id)
		{
			switch(state)
			{
			case e_mouse_hover:
				it->second.MouseHover();
				break;
			case e_mouse_leave:
				it->second.MouseLeave();
				break;
			case e_mouse_lbu:
				it->second.LButtonUp();
				break;
			case e_mouse_lbd:
				it->second.LButtonDown();
				break;
			}

			if (id == IDC_CLOSE_ID)
			{
				CRect rt;
				it->second.GetRect(rt);
				InvalidateRect(&rt, FALSE);
			}
		}
	}
}

void CDlgSloganExit::AddButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption)
{
	CNCButton btnControl;
	btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
	switch(nID)
	{
	case IDC_CLOSE_ID:
		{
			btnControl.SetTextBkgColor(RGB(245,245,245),RGB(216,216,216),RGB(216,216,216));
			btnControl.SetTextFrameColor(RGB(215,215,215),RGB(215,215,215),RGB(215,215,215));
		}
		break;
	case IDC_CANCEL_ID:
		{
			btnControl.SetTextBkgColor(RGB(245,245,245),RGB(255,112,0),RGB(255,112,0));
			btnControl.SetTextFrameColor(RGB(215,215,215),RGB(255,112,0),RGB(255,112,0));
			btnControl.SetTextColor(RGB(120,120,120), RGB(255,255,255), RGB(255,255,255));
		}
		break;
	case IDC_EXIT_ID:
		{
			btnControl.SetTextBkgColor(RGB(245,245,245),RGB(255,112,0),RGB(255,112,0));
			btnControl.SetTextFrameColor(RGB(215,215,215),RGB(255,112,0),RGB(255,112,0));
			btnControl.SetTextColor(RGB(120,120,120), RGB(255,255,255), RGB(255,255,255));
		}
		break;
	case IDC_RELOGIN_ID:
		{
			btnControl.SetTextBkgColor(RGB(245,245,245),RGB(255,112,0),RGB(255,112,0));
			btnControl.SetTextFrameColor(RGB(215,215,215),RGB(255,112,0),RGB(255,112,0));
			btnControl.SetTextColor(RGB(120,120,120), RGB(255,255,255), RGB(255,255,255));
		}
		break;
	}
	
	m_mapButton[nID] = btnControl;
}

