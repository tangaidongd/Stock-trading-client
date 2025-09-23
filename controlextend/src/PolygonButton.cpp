// NCButton.cpp: implementation of the NCButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PolygonButton.h"
#include "FontFactory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPolygonButton::CPolygonButton()
{
	m_bCheck    = FALSE;
	m_bHovering = FALSE;
	m_bPressed  = FALSE;
	m_bDisabled = FALSE;
	m_pParent   = NULL;
	m_point.x   = 0;
	m_point.y   = 0;
	m_subpoint.x = 0;
	m_subpoint.y = 0;
    m_rcRect = CRect(0, 0, 0, 0);

	m_clrHotText	 = RGB(255,255,255);
	m_clrNormalText  = RGB(255,255,255);
	m_clrPressText	 = RGB(255,255,255);

	m_clrBkgNormalText = RGB(0,0,0);
	m_clrBkgHotText	   = RGB(0,0,0);
	m_clrBkgPressText  = RGB(238,69,2);

	m_clrFrameNormalText = RGB(100,100,100);
	m_clrFrameHotText	 = RGB(255,255,255);
	m_clrFramePressText  = m_clrBkgPressText;

	m_clrBtnBkg		 = RGB(250,247,234);

	m_Font.m_StrName = gFontFactory.GetExistFontName(L"微软雅黑");//...
	m_Font.m_Size	 = 10;
	m_Font.m_iStyle	 = FontStyleRegular;	
	m_Font.m_bHorCenter	 = true;	
	m_Font.m_bVerCenter	 = true;	
	m_bSetFocus		 = TRUE;
	m_bVisiable		= TRUE;

    m_bChangeTextColor = true;		
	m_nID = -1;
	m_eStyle = ECStyle1;
	m_iOpenCfm = 0;
}

CPolygonButton::~CPolygonButton()
{
	m_pParent = NULL;
}

// 绘制按钮
void CPolygonButton::DrawButton(Graphics *pGraphics)
{	
	if (!m_bVisiable)
	{
		return;
	}

	if (NULL == pGraphics)
	{
		return;
	}

	pGraphics->SetSmoothingMode(SmoothingModeAntiAlias);

	PointF point1, point2, point3, point4;
	if (ECStyle1 == m_eStyle)
	{
		point1.X = m_rcRect.left;
		point1.Y = m_rcRect.top;
		point2.X = m_rcRect.right-m_iShift;
		point2.Y = m_rcRect.top;
		point3.X = m_rcRect.right;
		point3.Y = m_rcRect.bottom;
		point4.X = m_rcRect.left+m_iShift;
		point4.Y = m_rcRect.bottom;
	}
	else if (ECStyle2 == m_eStyle)
	{
		point1.X = m_rcRect.left;
		point1.Y = m_rcRect.top;
		point2.X = m_rcRect.right;
		point2.Y = m_rcRect.top;
		point3.X = m_rcRect.right;
		point3.Y = m_rcRect.bottom;
		point4.X = m_rcRect.left+m_iShift;
		point4.Y = m_rcRect.bottom;
	}
	else if (ECStyle3 == m_eStyle)
	{
		point1.X = m_rcRect.left;
		point1.Y = m_rcRect.top;
		point2.X = m_rcRect.right;
		point2.Y = m_rcRect.top;
		point3.X = m_rcRect.right-m_iShift;
		point3.Y = m_rcRect.bottom;
		point4.X = m_rcRect.left;
		point4.Y = m_rcRect.bottom;
	}

	PointF points[4] = {point1, point2, point3, point4}; 

	//绘制背景
	Pen pen(Color(255, 255, 255), 1);
	SolidBrush brushBkg((ARGB)Color::Red);	
	if (m_bPressed || m_bCheck)
	{
		brushBkg.SetColor(Color::Color(m_clrBkgPressText.GetB(),m_clrBkgPressText.GetG(),m_clrBkgPressText.GetR()));
		pen.SetColor(Color::Color(m_clrFramePressText.GetB(),m_clrFramePressText.GetG(),m_clrFramePressText.GetR()));
	}
	else if (m_bHovering)
	{
		brushBkg.SetColor(Color::Color(m_clrBkgHotText.GetB(),m_clrBkgHotText.GetG(),m_clrBkgHotText.GetR()));
		pen.SetColor(Color::Color(m_clrFrameHotText.GetB(),m_clrFrameHotText.GetG(),m_clrFrameHotText.GetR()));
	}
	else
	{
		brushBkg.SetColor(Color::Color(m_clrBkgNormalText.GetB(),m_clrBkgNormalText.GetG(),m_clrBkgNormalText.GetR()));
		pen.SetColor(Color::Color(m_clrFrameNormalText.GetB(),m_clrFrameNormalText.GetG(),m_clrFrameNormalText.GetR()));
	}
	pGraphics->FillPolygon(&brushBkg, points, 4); 
	pGraphics->DrawPolygon(&pen, points, 4);

	RectF grect;
	grect.X = (REAL)m_rcRect.left;
	grect.Y = (REAL)m_rcRect.top;
	grect.Width = (REAL)m_rcRect.Width();
	grect.Height = (REAL)m_rcRect.Height();
	ShowText(pGraphics,grect);
}

/************************************************************************/
/* 函数名称：CreateButton                                               */
/* 功能描述：接收绘制按钮所需要的参数									*/
/* 输入参数：lpszCaption,按钮上显示的文本; lpRect,按钮区域;				*/
/* pParent,按钮的父窗口;nID,按钮ID号;iShift平行四边形偏移量, style样式	*/
/************************************************************************/
void CPolygonButton::CreateButton(LPCTSTR lpszCaption, LPRECT lpRect, CWnd *pParent, UINT nID, int32 iShift,CBtn_Style style)
{
	if (lpszCaption)
	{
		m_strCaption = lpszCaption;
	}

	m_rcRect	= lpRect;
	m_pParent	= pParent;
	m_nID		= nID;
	m_iShift	= iShift;
	m_eStyle	= style;
}

void CPolygonButton::Refresh()
{
	if (!m_bVisiable)
	{
		return;
	}

	if (NULL == m_pParent)
	{
		return;
	}

	CWindowDC dc(m_pParent);
	Graphics pGraphics(dc.GetSafeHdc());
	DrawButton(&pGraphics);
	return;
}

void CPolygonButton::CheckCaptionLen(CString &StrCaption,RectF& rcShow,RectF& rcStr)
{
	int32 iLen = WideCharToMultiByte(CP_ACP, 0, StrCaption, -1, NULL, 0, NULL, NULL);
	iLen -= 1;
	if (0 < iLen && L"" != StrCaption)
	{
		int32 iWidth = (int32)(rcStr.Width / iLen);
		int32 iRow = (int32)(rcShow.Height / rcStr.Height);
		int32 iAllWidth = (int32)(rcShow.Width * iRow);
		if (rcStr.Width > iAllWidth)
		{
			int32 iShow = (iAllWidth/iWidth - 2)/2;
			if (0 < iShow)
			{
				StrCaption = StrCaption.Left(iShow);
				StrCaption += L"...";
			}
		}
	}
}

void CPolygonButton::ShowText(Graphics *pGraphics,RectF& rcShow)
{
	if (m_strCaption.IsEmpty())
	{
		return;
	}

	CString strTitle(m_strCaption);

	Gdiplus::StringFormat strFormat;	
	strFormat.SetAlignment(m_Font.m_bHorCenter != 0 ? StringAlignmentCenter : StringAlignmentNear);
	strFormat.SetLineAlignment(m_Font.m_bVerCenter != 0 ?StringAlignmentCenter : StringAlignmentNear);

	//颜色
	SolidBrush brush((ARGB)Color::White);
	if (m_bPressed || m_bCheck)
	{
		brush.SetColor(Color::Color(m_clrPressText.GetB(),m_clrPressText.GetG(),m_clrPressText.GetR()));
	}
	else if (m_bHovering)
	{
		brush.SetColor(Color::Color(m_clrHotText.GetB(),m_clrHotText.GetG(),m_clrHotText.GetR()));
	}
	else
	{
		brush.SetColor(Color::Color(m_clrNormalText.GetB(),m_clrNormalText.GetG(),m_clrNormalText.GetR()));
	}
	if (m_bDisabled && m_bChangeTextColor)
	{
		brush.SetColor((ARGB)Color::Gray);
	}
	
	if (!m_strCaption.IsEmpty())
	{
		//绘制字体
		Gdiplus::FontFamily fontFamily(m_Font.m_StrName);
		Gdiplus::Font font(&fontFamily, m_Font.m_Size, m_Font.m_iStyle, UnitPoint);


		rcShow.X += m_point.x;
		rcShow.Y += m_point.y;
		RectF rcBound;
		PointF point;
		point.X = rcShow.X;
		point.Y = rcShow.Y;
		pGraphics->MeasureString(strTitle, strTitle.GetLength(), &font, point, &strFormat, &rcBound);
		CheckCaptionLen(strTitle,rcShow,rcBound); // 字符串占的宽度, 超出显示区域用...表示
		pGraphics->DrawString(strTitle, strTitle.GetLength(), &font, rcShow, &strFormat, &brush);
	}
}

// 检测指定点是否处于按钮内
BOOL CPolygonButton::PtInButton(CPoint &point)
{
	PointF point1, point2, point3, point4;
	if (ECStyle1 == m_eStyle)
	{
		point1.X = m_rcRect.left;
		point1.Y = m_rcRect.top;
		point2.X = m_rcRect.right-m_iShift;
		point2.Y = m_rcRect.top;
		point3.X = m_rcRect.right;
		point3.Y = m_rcRect.bottom;
		point4.X = m_rcRect.left+m_iShift;
		point4.Y = m_rcRect.bottom;
	}
	else if (ECStyle2 == m_eStyle)
	{
		point1.X = m_rcRect.left;
		point1.Y = m_rcRect.top;
		point2.X = m_rcRect.right;
		point2.Y = m_rcRect.top;
		point3.X = m_rcRect.right;
		point3.Y = m_rcRect.bottom;
		point4.X = m_rcRect.left+m_iShift;
		point4.Y = m_rcRect.bottom;
	}
	else if (ECStyle3 == m_eStyle)
	{
		point1.X = m_rcRect.left;
		point1.Y = m_rcRect.top;
		point2.X = m_rcRect.right;
		point2.Y = m_rcRect.top;
		point3.X = m_rcRect.right-m_iShift;
		point3.Y = m_rcRect.bottom;
		point4.X = m_rcRect.left;
		point4.Y = m_rcRect.bottom;
	}

	PointF points[4] = {point1, point2, point3, point4}; 

	GraphicsPath myGraphicsPath;              
	Region myRegion;                       

	myGraphicsPath.Reset();  
	myGraphicsPath.AddPolygon(points, 4); 
	myRegion.MakeEmpty();  
	myRegion.Union(&myGraphicsPath);  

	Point pt;
	pt.X = point.x;
	pt.Y = point.y;
	return myRegion.IsVisible(pt);       
}