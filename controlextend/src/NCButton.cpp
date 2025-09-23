// NCButton.cpp: implementation of the NCButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NCButton.h"
#include "FontFactory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCButton::CNCButton()
{
	m_bCheck    = FALSE;
	m_bHovering = FALSE;
	m_bPressed  = FALSE;
	m_bDisabled = FALSE;
	m_pParent   = NULL;
	m_pImage	= NULL;
	m_pImgBackup= NULL;
	m_pImgHot	= NULL;
	m_pImgStatus= NULL; 
	m_bCreate	= FALSE;
	m_index		= 0;
	m_point.x   = 0;
	m_point.y   = 0;
	m_subpoint.x = 0;
	m_subpoint.y = 0;
    m_rcRect = CRect(0, 0, 0, 0);
	m_rcAreaOffset = CRect(0, 0, 0, 0);

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

	m_bClientDC		= FALSE;
    m_bNeedDBuffering = TRUE;
    m_bDirVertical = false;
    m_bChangeTextColor = true;		
	m_nID = -1;
	m_nImageCount = 0;
	m_StrDescription = L"";
	m_pSubMenu = NULL;
	m_bChangeStatus = false;
	m_dwHotSelectStockFlag   = 0;
	m_dwIndexSelectStockFlag = 0;
	m_eDaPanStatus = EDSCount;
	m_bShowCaption = TRUE;

	m_bIsCustomFrameLine = false;
	m_bDrawTop = true;
	m_bDrawLeft = true;
	m_bDrawRight = true;
	m_bDrawBottom = true;

	m_iGroupId = -1;
}

CNCButton::~CNCButton()
{
	m_pImage = NULL;
	m_pImgHot = NULL;
	m_pParent = NULL;
	m_pImgStatus = NULL;
}

// 绘制按钮
void CNCButton::DrawButton(Graphics *pGraphics)
{	
	if (!m_bVisiable)
	{
		return;
	}
	
	if (NULL == pGraphics)
	{
		return;
	}
	//
	ASSERT(pGraphics);
	m_bCreate = TRUE;
	
	// 更新下选股状态
	UpdateSelStockStatus();

	RectF grect;
	grect.X = (REAL)m_rcRect.left;
	grect.Y = (REAL)m_rcRect.top;
	grect.Width = (REAL)m_rcRect.Width();
	grect.Height = (REAL)m_rcRect.Height();

	Image *pImage = m_pImage;
	
	// 
	if (m_bChangeStatus && m_pImgStatus && 
 		Ok == m_pImgStatus->GetLastStatus() && !m_bDisabled)
	{
		pImage = m_pImgStatus;
	}

	if (NULL == pImage)
	{
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
		pGraphics->FillRectangle(&brushBkg, m_rcRect.left, m_rcRect.top, m_rcRect.Width(), m_rcRect.Height());
		if (m_bIsCustomFrameLine)
		{
			CRect rcDraw(m_rcRect);
			//rcDraw.top += 1;
			//rcDraw.left += 1;
			rcDraw.right -= 1;
			rcDraw.bottom -= 1;
			PointF L_PTTopLeft(rcDraw.left, rcDraw.top);  
			PointF L_PTTopRight(rcDraw.right, rcDraw.top);  
			PointF L_PTBottomLeft(rcDraw.left, rcDraw.bottom);  
			PointF L_PTBottomRight(rcDraw.right, rcDraw.bottom); 

			if (m_bDrawTop)
			{
				pGraphics->DrawLine(&pen,L_PTTopLeft, L_PTTopRight);
			}
			if (m_bDrawLeft)
			{
				pGraphics->DrawLine(&pen,L_PTTopLeft, L_PTBottomLeft);
			}
			if (m_bDrawRight)
			{
				pGraphics->DrawLine(&pen,L_PTTopRight, L_PTBottomRight);
			}
			if (m_bDrawBottom)
			{
				pGraphics->DrawLine(&pen,L_PTBottomLeft, L_PTBottomRight);
			}
		}
		else
		{
			pGraphics->DrawRectangle(&pen, m_rcRect.left, m_rcRect.top, m_rcRect.Width()-1, m_rcRect.Height()-1);
		}

		//显示文字
		ShowText(pGraphics,grect);
	}
	else
	{
		/*  获取按钮图片信息 */
		UINT uiCount = m_nImageCount;	// 按钮的状态个数
		int	iButtonIndex = 0;			// 标识按钮的状态
		if (m_bDisabled && (4 <= uiCount)) 
		{
			iButtonIndex = 3;
		}
		else if ((m_bPressed || m_bCheck) && (3 <= uiCount))
		{
			iButtonIndex = 2;
		}
		else if (m_bHovering && (2 <= uiCount))
		{
			iButtonIndex = 1;
		}
		else
		{
			iButtonIndex = 0;
		}

		// 在指定位置绘制按钮
		int iWidth = pImage->GetWidth();
		int iHeight = 0;
		if(uiCount != 0)
		{
			iHeight = pImage->GetHeight() / uiCount;
		}
		pGraphics->DrawImage(pImage, grect, 0, (REAL)iHeight*iButtonIndex, (REAL)iWidth, (REAL)iHeight, UnitPixel);

		ShowText(pGraphics,grect);

		if (NULL != m_pImgHot)
		{
			iWidth = m_pImgHot->GetWidth();
			iHeight = m_pImgHot->GetHeight();

			grect.X = (REAL)(m_rcRect.right - iWidth);
			grect.Y = (REAL)m_rcRect.top;
			grect.Width = (REAL)iWidth;
			grect.Height = (REAL)iHeight;

			pGraphics->DrawImage(m_pImgHot, grect, 0, 0, (REAL)iWidth, (REAL)iHeight, UnitPixel);
		}
	}
}

void CNCButton::DrawSysButton(Graphics *pGraphics, int index)
{
	if (!m_bVisiable)
	{
		return;
	}
	
	if (NULL == pGraphics)
	{
		return;
	}
	//
	ASSERT(pGraphics);
	m_bCreate = TRUE;
	StringFormat strFormat;
	strFormat.SetAlignment(StringAlignmentCenter);
	strFormat.SetLineAlignment(StringAlignmentCenter);
	
	/*  获取按钮图片信息 */
	UINT uiCount = m_nImageCount;	// 按钮的状态个数
	int	iButtonIndex = 0;			// 标识按钮的状态
	if (m_bDisabled && (4 <= uiCount)) 
	{
		iButtonIndex = 3;
	}
	else if (m_bPressed && (3 <= uiCount))
	{
		iButtonIndex = 2;
	}
	else if (m_bHovering && (2 <= uiCount))
	{
		iButtonIndex = 1;
	}
	else
	{
		iButtonIndex = 0;
	}

	RectF grect;
	grect.X = (REAL)m_rcRect.left;
	grect.Y = (REAL)m_rcRect.top;
	grect.Width = (REAL)m_rcRect.Width();
	grect.Height = (REAL)m_rcRect.Height();
	if (NULL != m_pImage)
	{
		// 在指定位置绘制按钮
		int iWidth = m_pImage->GetWidth()/4;
		int iHeight = 0;
		if(uiCount != 0)
		{
			iHeight = m_pImage->GetHeight() / uiCount;
		}		
		pGraphics->DrawImage(m_pImage, grect, (REAL)(iWidth*(index-1)), (REAL)iHeight*iButtonIndex, (REAL)iWidth, (REAL)iHeight, UnitPixel);
	}

	ShowText(pGraphics,grect);

// 	if (!m_strCaption.IsEmpty())
// 	{
// 		//绘制文字
// 		Gdiplus::FontFamily fontFamily(m_Font.m_StrName);
// 		Gdiplus::Font font(&fontFamily, m_Font.m_Size, m_Font.m_iStyle, UnitPoint);
// 		
// 		CString strTitle(m_strCaption);
// 		SolidBrush brush((ARGB)Color::White);
// 
// 		if (m_bPressed || m_bCheck)
// 		{
// 			brush.SetColor(Color::Color(m_clrPressText.GetB(),m_clrPressText.GetG(),m_clrPressText.GetR()));
// 		}
// 		else if (m_bHovering)
// 		{
// 			brush.SetColor(Color::Color(m_clrHotText.GetB(),m_clrHotText.GetG(),m_clrHotText.GetR()));
// 		}
// 		else
// 		{
// 			brush.SetColor(Color::Color(m_clrNormalText.GetB(),m_clrNormalText.GetG(),m_clrNormalText.GetR()));
// 		}
// 
// 		if (m_bDisabled)
// 		{
// 			brush.SetColor((ARGB)Color::Gray);
// 		}
// 		
// 		grect.X += m_point.x;
// 		grect.Y += (2+m_point.y);
// 		pGraphics->DrawString(strTitle, strTitle.GetLength(), &font, grect, &strFormat, &brush);
// 	}
}

/************************************************************************/
/* 函数名称：CreateButton                                               */
/* 功能描述：接收绘制按钮所需要的参数									*/
/* 输入参数：lpszCaption,按钮上显示的文本; lpRect,按钮矩形区域;			*/
/*	         pParent,按钮的父窗口;  pImage,按钮上的位图;				*/
/*			 nCount,按钮的状态数;   nID,按钮ID号;						*/
/************************************************************************/
void CNCButton::CreateButton(LPCTSTR lpszCaption, LPRECT lpRect, CWnd *pParent, Image *pImage, UINT nCount, UINT nID, Image *pImgHot, Image *pImgStatus, CString StrParam)
{
	// ASSERT(pParent);
	// ASSERT(pImage);

	if (lpszCaption)
	{
		m_strCaption = lpszCaption;
	}
	m_pParent	  = pParent;
	m_rcRect	  = lpRect;
	m_pImage      = pImage;
	m_pImgHot	  = pImgHot;
	m_pImgStatus  = pImgStatus;
	m_nImageCount = nCount;
	m_nID		  = nID;
	m_bCreate	  = true;
	SpliterSelStockFlags(StrParam);
}

void CNCButton::Refresh()
{
    if (!m_bVisiable)
    {
        return;
    }
	if (NULL == m_pParent)
	{
		return;
	}

    CDC memDC;
    CBitmap bmp;
    if (m_bClientDC)
    {
        CClientDC dc(m_pParent);
        CRect rcBtn, rcTmp;
        rcBtn = m_rcRect;
        if(m_bNeedDBuffering)
        {
            memDC.CreateCompatibleDC(&dc);
            rcTmp = rcBtn;

            rcBtn.left = 0;
            rcBtn.top = 0;
            rcBtn.right = rcTmp.Width();
            rcBtn.bottom = rcTmp.Height();
            m_rcRect = rcBtn;
            bmp.CreateCompatibleBitmap(&dc, rcBtn.Width(), rcBtn.Height());
            memDC.SelectObject(&bmp);
            memDC.FillSolidRect(rcBtn, m_clrBtnBkg);
            memDC.SetBkMode(TRANSPARENT);
            Graphics pGraphics(memDC.GetSafeHdc());

            if (0 < m_index)  // 系统按钮 1 min, 2 max, 3 restore, 4 close
            {
                if (2 == m_index)
                {
                    if (m_pParent->IsZoomed())
                    {
                        m_index = 3;
                    }
                    else
                    {
                        m_index = 2;
                    }
                }
                DrawSysButton(&pGraphics, m_index);
            }
            else
            {
                DrawButton(&pGraphics);
            }

            dc.BitBlt(rcTmp.left, rcTmp.top, rcTmp.Width(), rcTmp.Height(), &memDC, rcBtn.left, rcBtn.top, SRCCOPY);
            dc.SelectClipRgn(NULL);
            m_rcRect = rcTmp;
			//
			memDC.DeleteDC();
			bmp.DeleteObject();
        }
        else
        {
            Graphics pGraphics(dc.GetSafeHdc());
            if (0 < m_index)  // 系统按钮 1 min, 2 max, 3 restore, 4 close
            {
                if (2 == m_index)
                {
                    if (m_pParent->IsZoomed())
                    {
                        m_index = 3;
                    }
                    else
                    {
                        m_index = 2;
                    }
                }
                DrawSysButton(&pGraphics, m_index);
            }
            else
            {
                DrawButton(&pGraphics);
            }
        }
    }
    else
    {
        CWindowDC dc(m_pParent);
        CRect rcBtn, rcTmp;
        rcBtn = m_rcRect;
        if(m_bNeedDBuffering)
        {
            memDC.CreateCompatibleDC(&dc);
            rcTmp = rcBtn;

            rcBtn.left = 0;
            rcBtn.top = 0;
            rcBtn.right = rcTmp.Width();
            rcBtn.bottom = rcTmp.Height();
            m_rcRect = rcBtn;
            bmp.CreateCompatibleBitmap(&dc, rcBtn.Width(), rcBtn.Height());
            memDC.SelectObject(&bmp);
            memDC.FillSolidRect(rcBtn, m_clrBtnBkg);
            memDC.SetBkMode(TRANSPARENT);
            Graphics pGraphics(memDC.GetSafeHdc());
            if (0 < m_index)  // 系统按钮 1 min, 2 max, 3 restore, 4 close
            {
                if (2 == m_index)
                {
                    if (m_pParent->IsZoomed())
                    {
                        m_index = 3;
                    }
                    else
                    {
                        m_index = 2;
                    }
                }
                DrawSysButton(&pGraphics, m_index);
            }
            else
            {
                DrawButton(&pGraphics);
            }

            dc.BitBlt(rcTmp.left, rcTmp.top, rcTmp.Width(), rcTmp.Height(), &memDC, rcBtn.left, rcBtn.top, SRCCOPY);
            dc.SelectClipRgn(NULL);
            m_rcRect = rcTmp;
			//
			memDC.DeleteDC();
			bmp.DeleteObject();
        }
        else
        {
            Graphics pGraphics(dc.GetSafeHdc());
            if (0 < m_index)  // 系统按钮 1 min, 2 max, 3 restore, 4 close
            {
                if (2 == m_index)
                {
                    if (m_pParent->IsZoomed())
                    {
                        m_index = 3;
                    }
                    else
                    {
                        m_index = 2;
                    }
                }
                DrawSysButton(&pGraphics, m_index);
            }
            else
            {
                DrawButton(&pGraphics);
            }
        }
    }
    memDC.DeleteDC();
    bmp.DeleteObject();
}

void CNCButton::CheckCaptionLen(CString &StrCaption,RectF& rcShow,RectF& rcStr)
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

void CNCButton::ShowText(Graphics *pGraphics,RectF& rcShow)
{
	if (!m_bShowCaption)
	{
		return;
	}

	if (m_strCaption.IsEmpty() && m_strSubCaption.IsEmpty())
	{
		return;
	}

	CString strTitle(m_strCaption);
	CString strSubTitle(m_strSubCaption);

	Gdiplus::StringFormat strFormat;	
	strFormat.SetAlignment(m_Font.m_bHorCenter != 0 ? StringAlignmentCenter : StringAlignmentNear);
	strFormat.SetLineAlignment(m_Font.m_bVerCenter != 0 ?StringAlignmentCenter : StringAlignmentNear);
	if (m_bDirVertical)
	{
		strFormat.SetFormatFlags(StringFormatFlagsDirectionVertical);
	}

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
	
	RectF rcSubShow;
	rcShow.GetBounds(&rcSubShow);
	if (!m_strSubCaption.IsEmpty())
	{
		//绘制字体
		Gdiplus::FontFamily fontFamily(m_Font.m_StrName);
		Gdiplus::Font font(&fontFamily, m_Font.m_Size - 2, m_Font.m_iStyle, UnitPoint);  //副标题比正标题字体小

		rcSubShow.Y = rcShow.Y + rcShow.Height/2;
		rcSubShow.Height = rcShow.Height/2;
		rcShow.Height = rcSubShow.Height;
		
		rcSubShow.X += m_subpoint.x;
		rcSubShow.Y += m_subpoint.y;
		
		RectF rcBound;
		PointF point;
		point.X = rcSubShow.X;
		point.Y = rcSubShow.Y;
		pGraphics->MeasureString(strSubTitle, strSubTitle.GetLength(), &font, point, &strFormat, &rcBound);
		CheckCaptionLen(strSubTitle,rcSubShow,rcBound); // 字符串占的宽度, 超出显示区域用...表示
		pGraphics->DrawString(strSubTitle, strSubTitle.GetLength(), &font, rcSubShow, &strFormat, &brush);
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

void CNCButton::SpliterSelStockFlags(CString StrParams)
{
	if ( !StrParams.IsEmpty() )
	{
		int iLeftPos  = StrParams.Find('(');
		int iRightPos = StrParams.Find(')');
		int iDotPos   = StrParams.Find(':');

		if (-1 == iLeftPos || -1 == iRightPos 
			|| -1 == iDotPos)
		{
			return ;
		}

		int iCount = iRightPos - iDotPos - 1;
		CString StrSelType = StrParams.Left(iLeftPos);
		CString StrSelFlags = StrParams.Mid(iDotPos + 1, iCount);

		int  iSlectId = 0;

		string sSelFlags = _Unicode2MultiChar(StrSelFlags);
		const char* strFilePath = sSelFlags.c_str();
		sscanf(strFilePath, "%d", &iSlectId);

		if (!StrSelType.CompareNoCase(L"indexselect") || 
			!StrSelType.CompareNoCase(L"dynamicselect"))
		{
			m_stSelStockParams.eType = ESSTIndexSelStock;
		}
		else if (!StrSelType.CompareNoCase(L"hotselect"))
		{
			m_stSelStockParams.eType = ESSTHotSelStock;
		}

		m_stSelStockParams.uSelectFlags = iSlectId;
	}
}

void CNCButton::UpdateSelStockStatus()
{
	switch(m_stSelStockParams.eType)
	{
	case ESSTIndexSelStock:
		{
			m_bChangeStatus = m_stSelStockParams.uSelectFlags & m_dwIndexSelectStockFlag;
			
			if (m_stSelStockParams.uSelectFlags == 0x18)		// 大盘红色上升策略
			{
				EnableButton(EDS0 == m_eDaPanStatus);
			}
			else if (m_stSelStockParams.uSelectFlags == 0x28)	// 大盘绿色调整策略	
			{
				EnableButton(EDS3 == m_eDaPanStatus);
			}
			else if (m_stSelStockParams.uSelectFlags == 0x48)	// 大盘蓝色下跌策略
			{
				EnableButton( EDS2 == m_eDaPanStatus);
			}
			else if (m_stSelStockParams.uSelectFlags == 0x88)	// 大盘黄色震荡策略
			{
				EnableButton(EDS1 == m_eDaPanStatus);
			}
			else if (m_stSelStockParams.uSelectFlags == 0x108)	// 大盘暴跌行情策略
			{
				EnableButton(EDS4 == m_eDaPanStatus);
			}
		}
		break;
	case ESSTHotSelStock:
		{
			m_bChangeStatus = m_stSelStockParams.uSelectFlags & m_dwHotSelectStockFlag;
		}
		break;
	default:
		break;
	}
}

void CNCButton::SetSelectStockFlagAndDaPanStatus(DWORD dwIndexSelStockFlag, DWORD dwHotSelStockFlag, UINT eDaPanStatus)
{
	m_dwIndexSelectStockFlag = dwIndexSelStockFlag;
	m_dwHotSelectStockFlag   = dwHotSelStockFlag;
	m_eDaPanStatus           = (E_DaPanState)eDaPanStatus;
}