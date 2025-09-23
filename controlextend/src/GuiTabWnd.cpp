#include "stdafx.h"

#include "memDc.h"
#include "GuiBasetab.h"
#include "resource.h"
#include "sharefun.h"
#include "GuiTabWnd.h"
#include "FontFactory.h"
#include "NewMenu.h"
#include "TabSplitWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGuiTabWnd

#define  SPACE_BORDER	3
#define  SPACE_TAB		0
#define  DEFAULT_FLAT	18
#define  HIDE_WIDTH		0
#define  SUB_HEIGHT		2

const COLORREF KColorTextNormal = RGB(182,183,185);
const COLORREF KColorTextSelect = RGB(228,229,230)/*CFaceScheme::Instance()->GetActiveColor()*/;
const COLORREF KColorTabFrame   = RGB(26, 28, 30);


const COLORREF KColorBkGround	= RGB(89, 75, 66);

const int KTimerTip		= 1;	// tip定时器id

///////////////////////////////////////////////
///////////////////////////////////////////////
CGuiTabWnd::CGuiTabWnd()
{
	m_rectCliente.SetRectEmpty();
	m_rectTabs.SetRectEmpty();
	m_ActualVar=0;
	m_Numtabs=0;
	m_iSelectTab=0;
	m_iHotTab=0;
	m_iIndexBegin=0;
	m_iIndexEnd=m_Numtabs;
	m_sizeImag=CSize(1, 1);
	m_pArray.RemoveAll();
	m_iMinValRec=0;  //minimo valor requerido para mostrar toda las carpetas
	
	m_FontHorz.CreateFont(-13,0,0,0,400,0,0,0,0,1,2,1,34, gFontFactory.GetExistFontName(L"宋体"));	//...
	m_FontHorzBold.CreateFont(-13,2,0,0,FW_BOLD,0,0,0,0,1,2,1,34,L"MS Sans Serif");
	
	m_style	 = FLAT;		// 默认平面
	m_alnTab = ALN_RIGHT;
	m_StyleDisplay = GUISTYLE_XP;
	m_eLayoutStyle = ELSNormal; // 总是占据全部的可排区域，均分显示，当显示不下的时候， 将当前条尽量放大， 其他条均分
	m_eGraphicStyle = EGSStyle1; //

	//
	m_bBkGroundBitmap		= false;
	m_ClrBkGround			= RGB(0,0,0);
	m_uiBitmapIDNormal		= 0;
	m_uiBitmapIDSelected	= 0;
	m_uiBitmapIDBK			= 0;

	m_iTabSpace = SPACE_TAB;

	m_ibmpWidth = 0;
	m_ibmpHeight = 0;

	m_iDefTabLen = 0;

	m_pngIDNormal = NULL;
	m_pngIDSelected = NULL;

	SetTabBkColor();			// 赋值默认的颜色
	SetTabTextColor();
	SetTabFrameColor();

	m_pUserCB = NULL;
}

CGuiTabWnd::~CGuiTabWnd()
{
	//
	m_FontHorz.DeleteObject();
	m_FontHorzBold.DeleteObject();
	
	//
	for (int32 i = 0; i < m_pArray.GetSize(); i++)
	{
		CGuiTab *ct = (CGuiTab*)m_pArray[i];
		DEL(ct);
	}
	m_pArray.RemoveAll();
	
}

void CGuiTabWnd::SetUserCB(CGuiTabWndCB* pUserCB)
{
	m_pUserCB = pUserCB;
}

void CGuiTabWnd::FillWithBitmap(CDC* pDC, UINT uiBitmapID, const CRect& rect)
{
	if ( NULL == pDC)
	{
		return;
	}
	
	CDC bmpdc;
	bmpdc.CreateCompatibleDC(NULL);
	
	CBitmap bmp;	
	bmp.LoadBitmap(uiBitmapID);
		
	BITMAP BitMap;
	bmp.GetObject(sizeof(BitMap),&BitMap);

	CBitmap * pOldBmp = bmpdc.SelectObject(&bmp);
	pDC->StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), &bmpdc, 0, 0, BitMap.bmWidth, BitMap.bmHeight, SRCCOPY);
	bmpdc.SelectObject(pOldBmp);
	bmpdc.DeleteDC();
	bmp.DeleteObject();
}

void CGuiTabWnd::SetBkGround(bool32 bBitmap,COLORREF clrBkGround,UINT uiBitmapIDNormal,UINT uiBitmapIDSelected,int nType,UINT uiBitmapIDBK)
{
	m_bBkGroundBitmap	 = bBitmap;
	
	if(bBitmap)
	{		
		m_uiBitmapIDNormal	 = uiBitmapIDNormal;
		m_uiBitmapIDSelected = uiBitmapIDSelected;
		m_uiBitmapIDBK = uiBitmapIDBK;

		CBitmap cbmp;
		BITMAP bmp;
		cbmp.LoadBitmap(uiBitmapIDNormal);
		cbmp.GetBitmap(&bmp);

		m_ibmpWidth = bmp.bmWidth+2;
		m_ibmpHeight = bmp.bmHeight/nType;
	}
	else
	{
		m_ClrBkGround = clrBkGround;
	}
}

void CGuiTabWnd::SetPngBkGround(Gdiplus::Image *pngIDNormal, Gdiplus::Image *pngIDSelected, uint32 uiType,UINT uiBitmapIDBK)
{
	if ((NULL == pngIDNormal) || (NULL == pngIDSelected))
	{
		return;
	}
	m_pngIDNormal = pngIDNormal;
	m_pngIDSelected = pngIDSelected;
	m_uiBitmapIDBK = uiBitmapIDBK;

 	m_ibmpWidth = m_pngIDNormal->GetWidth() - HIDE_WIDTH;
 	m_ibmpHeight = m_pngIDNormal->GetHeight()/uiType + SUB_HEIGHT;

	m_bBkGroundBitmap = TRUE;
}

BEGIN_MESSAGE_MAP(CGuiTabWnd, CWnd)
	//{{AFX_MSG_MAP(CGuiTabWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_RBUTTONDOWN()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_INITMENUPOPUP()
	ON_WM_MEASUREITEM()
	ON_WM_MENUCHAR()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGuiTabWnd message handlers
//****************************************************************************
void CGuiTabWnd::OnSysColorChange( )
{
	CWnd::OnSysColorChange( );
	
}

//****************************************************************************
BOOL CGuiTabWnd::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
}


void CGuiTabWnd::SetALingTabs(AlingTab alnTab)
{
	m_alnTab=alnTab;
	
	RecalLayout();
		
// 	CRect rtClient;
// 	GetClientRect(&rtClient);
// 	for (int32 i=0;i<m_pArray.GetSize();i++)
// 	{
// 		CGuiTab* ct=(CGuiTab*) m_pArray[i];	
// 		CRect  rt = ct->rect;
//	}
}

void CGuiTabWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	dc.SetBkColor(RGB(0,0,0));
	CMemDC memDC(&dc, m_rectCliente);

	COLORREF ClrBackgroud = RGB(13, 14, 18);//m_ClrBkGround;
	GetClientRect(m_rectCliente);

	if (m_Numtabs <=0) 
	{
		CMemDC memDCNull(&dc, m_rectCliente);
		memDCNull.FillSolidRect(m_rectCliente,ClrBackgroud);
		return; 
	}

	GetClientRect(m_rectCliente);	

	if ( !m_bBkGroundBitmap || 0 == m_uiBitmapIDBK)  // m_uiBitmapIDBK为0没有背景图片
	{
		// 画背景色
		memDC.FillSolidRect(m_rectCliente, ClrBackgroud);
	}
	else
	{
		// 背景图片
		CBitmap bmpBackgroud;
		bmpBackgroud.LoadBitmap(m_uiBitmapIDBK);      
		BITMAP bmp;
		bmpBackgroud.GetBitmap(&bmp);
		
		CDC memDC2;               
		memDC2.CreateCompatibleDC(&memDC);           
		memDC2.SelectObject(&bmpBackgroud);
		memDC.StretchBlt(m_rectCliente.left, m_rectCliente.top, m_rectCliente.Width(), m_rectCliente.Height(), 
			&memDC2, 1, 0, bmp.bmWidth-2, bmp.bmHeight, SRCCOPY);
		memDC2.DeleteDC();
		bmpBackgroud.DeleteObject();
	}

	// 	
	Drawtabs(&memDC);	
}

BOOL CGuiTabWnd::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	// TODO: Add your specialized code here and/or call the base class
	pParent= pParentWnd;
	return CWnd::Create(NULL, _T(""), dwStyle |WS_CLIPSIBLINGS|WS_CLIPCHILDREN, rect, pParentWnd, nID);
}

int CGuiTabWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_TipWnd.Create(this);
	
	return 0;
}

void CGuiTabWnd::RecalLayout()
{
	// 计算出所有tab显示的区域
	GetClientRect(&m_rectCliente);
	m_rectTabs = m_rectCliente;


	//好像没什么用，暂时注释掉  BY HX 2015/6/25
//	CClientDC dc(this);
	
// 	CFont* pFontOld = dc.SelectObject(&m_FontHorz);
// 	
// 	if (ALN_LEFT == m_alnTab || ALN_RIGHT == m_alnTab)
// 	{
// 		int iFontWidth;
// 		if (m_bBkGroundBitmap)
// 		{
// 			iFontWidth = m_ibmpWidth;
// 		}
// 		else
// 		{
// 			iFontWidth =  GuiDrawLayer::GetTextWidth(&dc, "123", FALSE);
// 		}
// 		int iIconWidth = m_sizeImag.cx;
// 		
// 		int iTabWidth = iFontWidth > iIconWidth ? iFontWidth : iIconWidth;
// 		iTabWidth += m_iTabSpace;
// 		
// 		if (ALN_LEFT == m_alnTab)
// 			m_rectTabs.left = m_rectTabs.right - iTabWidth;
// 		else 
// 			m_rectTabs.right = m_rectTabs.left + iTabWidth;
// 	}
// 	else
// 	{
// 		int iFontHeight;
// 		if (m_bBkGroundBitmap)
// 		{
// 			iFontHeight = m_ibmpHeight;
// 		}
// 		else
// 		{
// 			iFontHeight = GuiDrawLayer::GetTextHeight(&dc, "123", TRUE);
// 		}
// 		int iIconHeight = m_sizeImag.cy;
// 		
// 		int iTabHeight = iFontHeight > iIconHeight ? iFontHeight : iIconHeight;
// 		iTabHeight += m_iTabSpace;
// 		
// 		if (ALN_TOP == m_alnTab)
// 			m_rectTabs.top = m_rectTabs.bottom - iTabHeight;
// 		else
// 			m_rectTabs.bottom = m_rectTabs.top + iTabHeight;
// 	}
// 	
// 	//
// 	dc.SelectObject(pFontOld);
// 	
// 	m_TipWnd.Hide();
	// 计算各个tab显示位置
	AjustTabs(m_rectTabs);
	Invalidate();
	UpdateWindow();
}

void CGuiTabWnd::AjustTabs(CRect &RectTabs)
{
	if (m_Numtabs <= 0)
		return;	
	
	m_iMinValRec=SPACE_BORDER;
	
	CClientDC dc(this);
	
	CFont* pFontOld = dc.SelectObject(&m_FontHorz);
	
	if (ELSNormal == m_eLayoutStyle) // 总是占据全部的可排区域，均分显示，当显示不下的时候， 将当前条尽量放大， 其他条均分
	{
		if (m_iDefTabLen == 0)
		{
			// 查看当前选中的条是不是不够最小尺寸了， 如果是这样， 尽量保证当前显示条的显示空间
			TEXTMETRIC textMertric;
			dc.GetTextMetrics(&textMertric);
			
			int iFontWidth = textMertric.tmHeight;
			iFontWidth += 2;
			
			int iRectWidth = RectTabs.Width();
			if (ALN_LEFT == m_alnTab || ALN_RIGHT == m_alnTab)
				iRectWidth = RectTabs.Height();

			iRectWidth -= m_iTabSpace* m_Numtabs;
			
			float fWidthAve = ((float)iRectWidth) / (m_Numtabs-m_iIndexBegin);
			float fWidthCur = fWidthAve;
			float fWidthOther = fWidthAve;
			
			if ((float)iFontWidth > fWidthAve)
			{
				if (iFontWidth >= iRectWidth)
				{
					// 区域实在太小了
					fWidthCur = (float)iRectWidth;
				}
				else
				{
					fWidthCur = (float)iFontWidth;
				}
				
				if (m_Numtabs - 1 > 0)
				{
					fWidthOther = (iRectWidth - fWidthCur) / (float)(m_Numtabs - 1 - m_iIndexBegin);
				}
				else
				{
					fWidthOther = 0.;
				}
			}
			
			// 
			int iPos = 0;
			for (int i = m_iIndexBegin; i < m_Numtabs; i++)
			{
				CGuiTab* ct = (CGuiTab*) m_pArray[i];
				CRect rectTab;
				rectTab = RectTabs;

				if (iPos > 0)
				{
					iPos += m_iTabSpace;
				}
				
				if (ALN_LEFT == m_alnTab || ALN_RIGHT == m_alnTab)
				{
					rectTab.top = iPos;
					
					if (i == m_iSelectTab)
						rectTab.bottom = (int)(iPos + fWidthCur);
					else
						rectTab.bottom = (int)(iPos + fWidthOther);
					
					if (rectTab.top < RectTabs.top)
						rectTab.top = RectTabs.top;
					if (rectTab.bottom > RectTabs.bottom)
						rectTab.bottom = RectTabs.bottom;
					
					if (i == m_Numtabs - 1)	rectTab.bottom = RectTabs.bottom;
					
					ct->rect= rectTab;
					// 
					iPos = rectTab.bottom;
				}
				else
				{
					rectTab.left = iPos;
					
					if (i == m_iSelectTab)
						rectTab.right = (int)(iPos + fWidthCur);
					else
						rectTab.right = (int)(iPos + fWidthOther);
					
					if (rectTab.left < RectTabs.left)
						rectTab.left = RectTabs.left;
					if (rectTab.right > RectTabs.right)
						rectTab.right = RectTabs.right;
					
					if (i == m_Numtabs - 1)	rectTab.right = RectTabs.right;
					
					ct->rect= rectTab;
					
					// 
					iPos = rectTab.right;
				}
			}

		} 
		else
		{
			int iPos = 0;
			for (int i = m_iIndexBegin; i < m_Numtabs; i++)
			{
				CGuiTab* ct = (CGuiTab*) m_pArray[i];
				CRect rectTab;
				rectTab = RectTabs;

				if (iPos > 0)
				{
					iPos += m_iTabSpace;
				}

				if (ALN_LEFT == m_alnTab || ALN_RIGHT == m_alnTab)
				{
					rectTab.top = iPos;
					
					rectTab.bottom = (int)(iPos + m_iDefTabLen);
					
					if (rectTab.top < RectTabs.top)
						rectTab.top = RectTabs.top;
					if (rectTab.bottom > RectTabs.bottom)
						rectTab.bottom = RectTabs.bottom;
					
					ct->rect= rectTab;
					// 
					iPos = rectTab.bottom;
				}
				else
				{
					rectTab.left = iPos;
					rectTab.right = (int)(iPos + m_iDefTabLen);
					
					if (rectTab.left < RectTabs.left)
						rectTab.left = RectTabs.left;
					if (rectTab.right > RectTabs.right)
						rectTab.right = RectTabs.right;
					
					ct->rect= rectTab;
					iPos = rectTab.right;
				}
			}
		} // if (m_iDefTabWidth == 0)
	}
	else if(ELSGraphic == m_eLayoutStyle)  
	{
		int iPos = 0;
		for (int i = m_iIndexBegin; i < m_Numtabs; i++)
		{
			CGuiTab* ct = (CGuiTab*) m_pArray[i];
			CRect rectTab;
			rectTab = RectTabs;
			if (ALN_LEFT == m_alnTab || ALN_RIGHT == m_alnTab)
			{
				rectTab.top = iPos;
				
				rectTab.bottom = (int)(iPos + m_ibmpHeight);
				
				if (rectTab.top < RectTabs.top)
					rectTab.top = RectTabs.top;
				if (rectTab.bottom > RectTabs.bottom)
					rectTab.bottom = RectTabs.bottom;
				
				ct->rect= rectTab;
				// 
				iPos = rectTab.bottom;
			}
			else
			{
				rectTab.left = iPos;
				rectTab.right = (int)(iPos + m_ibmpWidth);
				
				if (rectTab.left < RectTabs.left)
					rectTab.left = RectTabs.left;
				if (rectTab.right > RectTabs.right)
					rectTab.right = RectTabs.right;
				
				ct->rect= rectTab;
				iPos = rectTab.right;
			}
		}
	}
	else if (ELSCustomSize == m_eLayoutStyle) // 根据文本长度确定tab标签项的排版
	{
		// 查看当前选中的条是不是不够最小尺寸了， 如果是这样， 尽量保证当前显示条的显示空间
		TEXTMETRIC textMertric;
		dc.GetTextMetrics(&textMertric);
		
		int iFontWidth = textMertric.tmHeight;
		iFontWidth += 2;
		
		int iRectWidth = RectTabs.Width();
		if (ALN_LEFT == m_alnTab || ALN_RIGHT == m_alnTab)
			iRectWidth = RectTabs.Height();
		
		float fWidthAve = ((float)iRectWidth) / (m_Numtabs-m_iIndexBegin);
		float fWidthCur = 70;//fWidthAve;
		float fWidthOther = 70;//fWidthAve;
		
		if ((float)iFontWidth > fWidthAve)
		{
			if (iFontWidth >= iRectWidth)
			{
				// 区域实在太小了
				fWidthCur = (float)iRectWidth;
			}
			else
			{
				fWidthCur = (float)iFontWidth;
			}
			
			if (m_Numtabs - 1 > 0)
			{
				fWidthOther = (iRectWidth - fWidthCur) / (float)(m_Numtabs - 1 - m_iIndexBegin);
			}
			else
			{
				fWidthOther = 0.;
			}
		}
		
		// 
		int iPos = 0;
		for (int i = m_iIndexBegin; i < m_Numtabs; i++)
		{
			CGuiTab* ct = (CGuiTab*) m_pArray[i];
			CRect rectTab;
			rectTab = RectTabs;
			
			if (ALN_LEFT == m_alnTab || ALN_RIGHT == m_alnTab)
			{
				rectTab.top = iPos;
				
				if (i == m_iSelectTab)
					rectTab.bottom = (int)(iPos + fWidthCur);
				else
					rectTab.bottom = (int)(iPos + fWidthOther);
				
				if (rectTab.top < RectTabs.top)
					rectTab.top = RectTabs.top;
				if (rectTab.bottom > RectTabs.bottom)
					rectTab.bottom = RectTabs.bottom;
				
			//	if (i == m_Numtabs - 1)	rectTab.bottom = RectTabs.bottom;
				
				ct->rect= rectTab;
				// 
				iPos = rectTab.bottom;
			}
			else
			{
				rectTab.left = iPos;
				
				if (i == m_iSelectTab)
					rectTab.right = (int)(iPos + fWidthCur);
				else
					rectTab.right = (int)(iPos + fWidthOther);
				
				if (rectTab.left < RectTabs.left)
					rectTab.left = RectTabs.left;
				if (rectTab.right > RectTabs.right)
					rectTab.right = RectTabs.right;
				
			//	if (i == m_Numtabs - 1)	rectTab.right = RectTabs.right;
				
				ct->rect= rectTab;
				
				// 
				iPos = rectTab.right;
			}
		}
	}
	
	dc.SelectObject(pFontOld);
}

void CGuiTabWnd::DrawImageTab(CDC* dc, int32 iIndexTab)
{
	if ( iIndexTab < 0 || iIndexTab >= m_pArray.GetSize() )
	{
		//ASSERT(0);
		return;
	}
	
	if ( NULL == dc )
	{
		//ASSERT(0);
		return;
	}
	
	CGuiTab* ct = (CGuiTab*)m_pArray[iIndexTab];
	bool32 bSelectedTab = (iIndexTab == m_iSelectTab);
	CRect  rectTab = ct->rect;

	if (EGSStyle1 == m_eGraphicStyle)
	{
		// 背景图片
		CBitmap bmpBackgroud;
		bmpBackgroud.LoadBitmap(m_uiBitmapIDNormal);
		
		CDC memDC;               
		memDC.CreateCompatibleDC(dc);           
		memDC.SelectObject(&bmpBackgroud); 
		if (!bSelectedTab)
		{
			bool32 bHotTab = (iIndexTab == m_iHotTab);
			int iY = 0;
			if (bHotTab)
			{
				iY = m_ibmpHeight;
			}

			dc->BitBlt(rectTab.left, rectTab.top, m_ibmpWidth, m_ibmpHeight, &memDC, 0, iY,SRCCOPY);
		}
		else
		{
			dc->BitBlt(rectTab.left, rectTab.top, m_ibmpWidth, m_ibmpHeight, &memDC, 0, m_ibmpHeight*2,SRCCOPY);
		}
		memDC.DeleteDC();
		bmpBackgroud.DeleteObject();
	}
	else if(EGSStyle2 == m_eGraphicStyle)
	{
		Graphics graphics(dc->m_hDC);
		RectF destRect;
		destRect.X = (REAL)rectTab.left;
		destRect.Y = (REAL)rectTab.top + SUB_HEIGHT;
		destRect.Width	= (REAL)rectTab.Width();
		destRect.Height = (REAL)rectTab.Height() - SUB_HEIGHT;
		if (!bSelectedTab)
		{
			bool32 bHotTab = (iIndexTab == m_iHotTab);
			int iY = 0;
			if (bHotTab)
			{
				iY = m_ibmpHeight-SUB_HEIGHT;
			}

			if (iIndexTab == (m_iIndexEnd-1)/*(m_Numtabs-1)*/)
			{
				destRect.Width += HIDE_WIDTH;
				graphics.DrawImage(m_pngIDNormal, destRect, 0, (REAL)iY, (REAL)(m_ibmpWidth+HIDE_WIDTH), (REAL)(m_ibmpHeight-SUB_HEIGHT), UnitPixel);
			}
			else
			{
				graphics.DrawImage(m_pngIDNormal, destRect, 0, (REAL)iY, (REAL)m_ibmpWidth, (REAL)(m_ibmpHeight-SUB_HEIGHT), UnitPixel);
			}
		}
		else
		{
			destRect.Width += HIDE_WIDTH;
			graphics.DrawImage(m_pngIDNormal, destRect, 0,  (REAL)(m_ibmpHeight-SUB_HEIGHT)*2, (REAL)(m_ibmpWidth+HIDE_WIDTH), (REAL)(m_ibmpHeight-SUB_HEIGHT), UnitPixel);
		}
	}


	// 显示文字
	CString strText = ct->lpMsg;
	int iTextWidth =  GuiDrawLayer::GetTextWidth(dc, strText, TRUE);
	if (iTextWidth > (m_ibmpWidth-10))
	{
		// 长文字显示不下， 用短文字代替
		strText = ct->lpShortMsg;
	}

	UINT uiTextAlign = 0;
	if (EGSStyle1 == m_eGraphicStyle)
	{
		uiTextAlign = GuiDrawLayer::ETACenter | GuiDrawLayer::ETAVCenter;
	}
	else if (EGSStyle2 == m_eGraphicStyle)
	{
		uiTextAlign = GuiDrawLayer::ETALeft | GuiDrawLayer::ETAVCenter;
	}
	CRect rectText = ct->rect;
	rectText.DeflateRect(5, 2);
	int nMode = dc->SetBkMode(TRANSPARENT);
	
	COLORREF clrtext;
	COLORREF clrTextNormal, clrTextSelect;
	GetTabTextColor(clrTextNormal, clrTextSelect);
	
	if (!bSelectedTab)
	{
		// 非选中的标签页
		// clrtext = ct->m_clrTextOther;
		// clrtext = KColorTextNormal;
		
		if ( m_bBkGroundBitmap )
		{
			clrtext = RGB(201,208,214);
		}
		else
		{
			clrtext = clrTextNormal;
		}
	}
	else
	{
		// 选中的标签页
		// clrtext = ct->m_clrTextCur;
		// clrtext = KColorTextSelect;
		
		if ( m_bBkGroundBitmap )
		{
			if (EGSStyle2 == m_eGraphicStyle)
			{
				clrtext = RGB(201,208,214);
			}
			else
			{
				clrtext = RGB(255, 255, 255);
			}
		}
		else
		{				
			clrtext = clrTextSelect;
		}			
	}
	
	dc->SetTextColor(clrtext);
	
	if (ALN_LEFT == m_alnTab || ALN_RIGHT == m_alnTab)
	{
		GuiDrawLayer::DrawText(dc, strText, rectText, uiTextAlign, FALSE);
	}
	else
	{
		GuiDrawLayer::DrawText(dc, strText, rectText, uiTextAlign, TRUE);
	}
	
	//
	dc->SetBkMode(nMode);
}

void CGuiTabWnd::DrawTab(CDC* dc, int32 iIndexTab)
{
	if ( iIndexTab < 0 || iIndexTab >= m_pArray.GetSize() )
	{
		//ASSERT(0);
		return;
	}

	if ( NULL == dc )
	{
		//ASSERT(0);
		return;
	}

	CGuiTab* ct = (CGuiTab*)m_pArray[iIndexTab];
	bool32 bSelectedTab = (iIndexTab == m_iSelectTab);

	//
	COLORREF clrBackNormal = RGB(44, 44, 52);
	COLORREF clrBackSelect = RGB(44, 44, 52);
	COLORREF clrRound	   = RGB(13, 14, 18);
	GetTabBkColor(clrBackNormal, clrBackSelect);
	//GetTabFrameColor(clrRound);
	
	CRect  rectTab = ct->rect;
	// rectTab.DeflateRect(1, 1);
	
	CPen pen(PS_SOLID, 1, clrRound);
	CPen* pOldPen = dc->SelectObject(&pen);
	
	const int32 KiTabDif = rectTab.Height() / 4;
	
	// 画边框, 填充底色	
	{	
		CRect RectClient;
		GetClientRect(&RectClient);

		int32 iRight  = rectTab.right;		

		if ( iIndexTab == (m_Numtabs - 1) )
		{
			iRight -= 1;
		}

		CBrush br;
		COLORREF clrMyBkTop, clrMyBkBottom;
		if ( bSelectedTab )
		{
			br.CreateSolidBrush(clrBackSelect);
			clrMyBkTop = clrBackSelect;
			clrMyBkBottom = m_ClrBkGround;
		}
		else
		{
			br.CreateSolidBrush(clrBackNormal);
			clrMyBkBottom = clrBackNormal;
			clrMyBkTop = m_ClrBkGround;
		}
		
		CBrush* pOldBrush = dc->SelectObject(&br);

		if ( ALN_TOP == m_alnTab )
		{
			dc->BeginPath();
			{
				// 填充
				dc->MoveTo(iRight, rectTab.bottom);
				dc->LineTo(iRight, rectTab.top + KiTabDif);
				dc->LineTo(iRight - KiTabDif, rectTab.top);
				dc->LineTo(rectTab.left + KiTabDif, rectTab.top);
				dc->LineTo(rectTab.left, rectTab.top + KiTabDif);
				dc->LineTo(rectTab.left, rectTab.bottom);	
				dc->LineTo(iRight, rectTab.bottom);
			}
			
			dc->EndPath();
			if ( S3D == m_style )		// 暂时不独立开
			{
				dc->SelectClipPath(RGN_COPY);
				// 渐变色 左上 预定color 右下 背景color
				GRADIENT_RECT grt;
				TRIVERTEX triVertexs[2];
				triVertexs[0].x = rectTab.left;
				triVertexs[0].y = rectTab.top;
				triVertexs[0].Red = (GetRValue(clrMyBkTop) << 8);
				triVertexs[0].Green = (GetGValue(clrMyBkTop)<<8);
				triVertexs[0].Blue = (GetBValue(clrMyBkTop)<<8);
				triVertexs[0].Alpha = 0;
				
				triVertexs[1].x = rectTab.right;
				triVertexs[1].y = rectTab.bottom;
				triVertexs[1].Red = (GetRValue(clrMyBkBottom)<<8);
				triVertexs[1].Green = (GetGValue(clrMyBkBottom)<<8);
				triVertexs[1].Blue = (GetBValue(clrMyBkBottom)<<8);
				triVertexs[1].Alpha = 0;
				
				grt.UpperLeft = 0;
				grt.LowerRight = 1;
				
				BOOL b = ::GradientFill(dc->GetSafeHdc(), triVertexs, 2, &grt, 1, GRADIENT_FILL_RECT_V);
				dc->SelectClipRgn(NULL);
			}
			else
			{
				// 普通
				dc->FillPath();
			}
			
			{
				// 边框
				dc->MoveTo(iRight, rectTab.bottom);
				dc->LineTo(iRight, rectTab.top + KiTabDif);
				dc->LineTo(iRight - KiTabDif, rectTab.top);
				dc->LineTo(rectTab.left + KiTabDif, rectTab.top);
				dc->LineTo(rectTab.left, rectTab.top + KiTabDif);
				if ( bSelectedTab && S3D == m_style )
				{
					dc->LineTo(rectTab.left, rectTab.bottom); // 不画底边
					//dc->LineTo(rectTab.left, rectTab.bottom);
				}
				else
				{
					dc->LineTo(rectTab.left, rectTab.bottom-1);	// 画出底边界
					dc->LineTo(iRight, rectTab.bottom-1);
				}
			}
		}
		else
		{	
			// 交换颜色
			{
				COLORREF clrTemp = clrMyBkTop;
				clrMyBkTop = clrMyBkBottom;
				clrMyBkBottom = clrTemp;
			}
			// 其余对齐
			dc->BeginPath();
			{
				// 填充
				dc->MoveTo(rectTab.TopLeft());
				dc->LineTo(iRight, rectTab.top);
				dc->LineTo(iRight, rectTab.bottom);
				dc->LineTo(rectTab.left, rectTab.bottom);
				//dc->LineTo(iRight, rectTab.bottom - KiTabDif);
				//dc->LineTo(iRight - KiTabDif, rectTab.bottom);
				//dc->LineTo(rectTab.left + KiTabDif, rectTab.bottom);
				//dc->LineTo(rectTab.left, rectTab.bottom - KiTabDif);
				dc->LineTo(rectTab.TopLeft());			
			}
			
			dc->EndPath();
			if ( S3D == m_style )		// 暂时不独立开
			{
				dc->SelectClipPath(RGN_COPY);
				// 渐变色 左上 预定color 右下 背景color
				GRADIENT_RECT grt;
				TRIVERTEX triVertexs[2];
				triVertexs[0].x = rectTab.left;
				triVertexs[0].y = rectTab.top;
				triVertexs[0].Red = (GetRValue(clrMyBkTop) << 8);
				triVertexs[0].Green = (GetGValue(clrMyBkTop)<<8);
				triVertexs[0].Blue = (GetBValue(clrMyBkTop)<<8);
				triVertexs[0].Alpha = 0;
				
				triVertexs[1].x = rectTab.right;
				triVertexs[1].y = rectTab.bottom;
				triVertexs[1].Red = (GetRValue(clrMyBkBottom)<<8);
				triVertexs[1].Green = (GetGValue(clrMyBkBottom)<<8);
				triVertexs[1].Blue = (GetBValue(clrMyBkBottom)<<8);
				triVertexs[1].Alpha = 0;
				
				grt.UpperLeft = 0;
				grt.LowerRight = 1;
				
				BOOL b = ::GradientFill(dc->GetSafeHdc(), triVertexs, 2, &grt, 1, GRADIENT_FILL_RECT_V);
				dc->SelectClipRgn(NULL);
			}
			else
			{
				// 普通
				CRect rect(rectTab.left + 1 , rectTab.top + 1, rectTab.right, rectTab.bottom - 1);
				if(iIndexTab == m_pArray.GetSize() - 1)
				{
					rect.right = rectTab.right - 1;
				}
				dc->FillRect(&rect, &br);
			}
			
			{
				// 边框
				//dc->MoveTo(rectTab.TopLeft());
				//if ( bSelectedTab && S3D == m_style )
				//{
				//	dc->MoveTo(iRight, rectTab.top);	// 跳过顶边绘制
				//}
				//else
				//{
				//	dc->LineTo(iRight, rectTab.top);	//  绘制顶边
				//}
				//dc->LineTo(iRight, rectTab.bottom - KiTabDif);
				//dc->LineTo(iRight - KiTabDif, rectTab.bottom);
				//dc->LineTo(rectTab.left + KiTabDif, rectTab.bottom);
				//dc->LineTo(rectTab.left, rectTab.bottom - KiTabDif);
				//dc->LineTo(rectTab.TopLeft());
			}
		}
			
			dc->SelectObject(pOldBrush);
			br.DeleteObject();	
	}
	
	dc->SelectObject(pOldPen);
	pen.DeleteObject();
	
	// 显示图片、文字		
	BOOL bShowImage = FALSE;
	CString strText = ct->lpMsg;
	UINT uiTextAlign = 0;
	
	CRect rectDraw = ct->rect;
	rectDraw.DeflateRect(1, 1);
	
	CRect rectImage = ct->rect;
	rectImage.DeflateRect(1, 1);
	
	CRect rectText = ct->rect;
	rectText.DeflateRect(1, 1);
	
	if (ALN_LEFT == m_alnTab || ALN_RIGHT == m_alnTab)
	{
		int iTextHeight = GuiDrawLayer::GetTextHeight(dc, strText, FALSE);
		
		if ( NULL != Image.m_hImageList)
		{
			if (ct->uIcon < (UINT)Image.GetImageCount())
			{
				// 该条指定有图片需要显示
				rectImage.bottom = rectImage.top + m_sizeImag.cy + 2;
				if (m_sizeImag.cy + 2 + iTextHeight <= rectDraw.Height())
				{
					// 可以显示
					bShowImage = TRUE;
					rectImage.bottom = rectImage.top + m_sizeImag.cy + 2;
					rectText.top = rectImage.bottom;
					
					uiTextAlign = GuiDrawLayer::ETATop | GuiDrawLayer::ETACenter;
				}				
			}
		}
		
		if (!bShowImage)
		{
			if (iTextHeight > rectText.Height())
			{
				// 长文字显示不下， 用短文字代替
				strText = ct->lpShortMsg;
			}
			
			uiTextAlign = GuiDrawLayer::ETAVCenter | GuiDrawLayer::ETACenter;
		}
	}
	else
	{
		int iTextWidth = GuiDrawLayer::GetTextWidth(dc, strText, TRUE);
		
		if ( NULL != Image.m_hImageList)
		{				
			if ( ct->uIcon < (UINT)Image.GetImageCount())
			{
				// 该条指定有图片需要显示
				rectImage.right = rectImage.left + m_sizeImag.cx + 2;
				if (m_sizeImag.cx + 2 + iTextWidth <= rectDraw.Width())
				{
					// 可以显示
					bShowImage = TRUE;
					rectImage.right = rectImage.left + m_sizeImag.cx + 2;
					rectText.left = rectImage.right;
					
					uiTextAlign = GuiDrawLayer::ETALeft | GuiDrawLayer::ETAVCenter;
				}				
			}
		}
		
		if (!bShowImage)
		{
			if (iTextWidth > rectText.Width())
			{
				// 长文字显示不下， 用短文字代替
				strText = ct->lpShortMsg;
				GetSuitableDisplayShortName(dc, rectText, ct->lpMsg, strText);
			}		
			
			uiTextAlign = GuiDrawLayer::ETACenter | GuiDrawLayer::ETAVCenter;
		}
	}
	
	// 显示图片
	if (bShowImage)
	{
 		int iX = rectImage.left + (rectImage.Width() - m_sizeImag.cx) / 2;
 		int iY = rectImage.top + (rectImage.Height() - m_sizeImag.cy) / 2;
		
		Image.Draw(dc, ct->uIcon, CPoint(iX, iY), ILD_TRANSPARENT);
	}

	// 显示文字
	int nMode = dc->SetBkMode(TRANSPARENT);
	
	COLORREF clrtext;
	COLORREF clrTextNormal, clrTextSelect;
	GetTabTextColor(clrTextNormal, clrTextSelect);
	
	if (!bSelectedTab)
	{
		// 非选中的标签页
		// clrtext = ct->m_clrTextOther;
		// clrtext = KColorTextNormal;
		
		if ( m_bBkGroundBitmap )
		{
			clrtext = RGB(0, 0, 0);
		}
		else
		{
			clrtext = clrTextNormal;
		}
	}
	else
	{
		// 选中的标签页
		// clrtext = ct->m_clrTextCur;
		// clrtext = KColorTextSelect;
		
		if ( m_bBkGroundBitmap )
		{
			clrtext = RGB(0, 0, 120);
		}
		else
		{				
			clrtext = clrTextSelect;
		}			
	}
	
	dc->SetTextColor(clrtext);
	
	if (ALN_LEFT == m_alnTab || ALN_RIGHT == m_alnTab)
	{
		GuiDrawLayer::DrawText(dc, strText, rectText, uiTextAlign, FALSE);
	}
	else
	{
		GuiDrawLayer::DrawText(dc, strText, rectText, uiTextAlign, TRUE);
	}
	
	//
	dc->SetBkMode(nMode);
}

void CGuiTabWnd::Drawtabs(CDC* dc)
{	


	CPen light(PS_SOLID,1,GetSysColor(COLOR_BTNHIGHLIGHT));
	CPen Dark(PS_SOLID,1,GuiDrawLayer::GetRGBColorShadow(m_StyleDisplay));
	CPen Black(PS_SOLID,1,GetSysColor(BLACK_PEN));	
	CPen pPress(PS_SOLID,1,GuiDrawLayer::GetRGBCaptionXP());
	CRect rectText;
	CPen* oldPen= dc->SelectObject(&light);
	CString szStrPoints=_T("...");
	CRect rcClient;
	GetClientRect(rcClient);

	int iIndexEnd = m_Numtabs;
	if(EGSStyle2 == m_eGraphicStyle)
	{
		iIndexEnd = m_iIndexEnd;
	}
	// 挨个tab
	CFont* pFontOld = dc->SelectObject(&m_FontHorz);
	for (int iCont = m_iIndexBegin; iCont < iIndexEnd; iCont++)
	{
		if ( iCont == m_iSelectTab )
		{
			continue;
		}
		
		// 非选中项目的绘制
		if (m_bBkGroundBitmap)
		{
			DrawImageTab(dc, iCont);
		}
		else
		{
			DrawTab(dc, iCont);
		}

	}
	
	// 选中项目的绘制
	{ 
		if (m_bBkGroundBitmap)
		{
			DrawImageTab(dc, m_iSelectTab);
		}
		else
		{
			DrawTab(dc, m_iSelectTab);	
		}
	}

	dc->SelectObject(oldPen);
	dc->SelectObject(pFontOld);
}

void CGuiTabWnd::SetImageList(UINT nBitmapID, int cx, int nGrow, COLORREF crMask)
{
	CBitmap cbmp;
	BITMAP bmp;
	Image.Create(nBitmapID,cx,nGrow,crMask);
	cbmp.LoadBitmap(nBitmapID);
	cbmp.GetBitmap(&bmp);
	Image.Add(&cbmp,crMask);
	m_sizeImag=CSize(cx,bmp.bmHeight);	
}

void CGuiTabWnd::Addtab(CString lpMsg, CString lpShortMsg, CString lpTipMsg, CString lpValue)
{
	m_pArray.SetAtGrow(m_Numtabs, new CGuiTab(lpMsg, lpShortMsg, lpTipMsg, lpValue));
	if (m_alnTab!=ALN_TOP)
	{
		m_iSelectTab=0;
	}
	else
		m_iSelectTab=m_Numtabs;
	
	m_Numtabs++;
	
	//assign color
	CGuiTab* ctn=(CGuiTab*)m_pArray[m_Numtabs-1];
	GuiDrawLayer::GetNextColor(m_Numtabs,ctn->m_clrLTab,ctn->m_clrHtab);
	
	RecalLayout();
}
int CGuiTabWnd::GetCount()
{
	return m_Numtabs;
}
void CGuiTabWnd::DeleteAll()
{
	//
	for (int32 i = 0; i < m_pArray.GetSize(); i++)
	{
		CGuiTab *ct = (CGuiTab*)m_pArray[i];
		DEL(ct);
	}
	m_pArray.RemoveAll();

	//
	m_Numtabs = 0;

	//
	RecalLayout();
}

void CGuiTabWnd::DeleteTab(int m_numtab)
{
	if (m_numtab < m_Numtabs)
	{
		CGuiTab* ctn=(CGuiTab*) m_pArray[m_numtab]; 	
		if (ctn)
			delete ctn;
		
		m_Numtabs--;
		m_pArray.RemoveAt(m_numtab,1);
		if (m_Numtabs >0)
		{
			if (m_iSelectTab == m_numtab)
			{
				m_iSelectTab=m_iSelectTab-1;
				if (m_iSelectTab< 0) m_iSelectTab=0;
			}
			else
			{
				if (m_iSelectTab > m_numtab)
				{
					m_iSelectTab--;
				}
			}
			RecalLayout();
		}
		else
		{
			RedrawWindow();
		}

		// 给父窗口发送消息， 通知当前项删除了
		NMHDR   nmhdr;  
		nmhdr.code=WM_DELETETAB;  
		nmhdr.hwndFrom=m_hWnd;  
		nmhdr.idFrom=GetDlgCtrlID();  
		::SendMessage(pParent->GetSafeHwnd(),WM_NOTIFY,(WPARAM)GetDlgCtrlID(),(LPARAM)(&nmhdr));   
	}
}

void CGuiTabWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	if (m_Numtabs > 0)
		RecalLayout();
}

void CGuiTabWnd::SetCurtab(int m_numtab, bool bSilence/* = false*/)
{	
	if (m_numtab >= 0 && m_numtab < m_Numtabs && m_numtab != m_iSelectTab )
	{
		m_iSelectTab=m_numtab;
		m_iHotTab = m_iSelectTab;
		RecalLayout();
		
// 		if ( !bSilence )
// 		{
// 			// 给父窗口发送消息， 通知当前选择项发生了改变
// 			NMHDR   nmhdr;  
// 			nmhdr.code=TCN_SELCHANGE;  
// 			nmhdr.hwndFrom=m_hWnd;  
// 			nmhdr.idFrom=GetDlgCtrlID();  
// 			::SendMessage(pParent->GetSafeHwnd(),WM_NOTIFY,(WPARAM)GetDlgCtrlID(),(LPARAM)(&nmhdr));   
// 		}
	}

	m_iHotTab = m_iSelectTab;
	if ( !bSilence && m_numtab >= 0 && m_numtab < m_Numtabs )
	{
		// 给父窗口发送消息， 通知当前选择项发生了改变
		NMHDR   nmhdr;  
		//lint --e{648} suppress " Overflow in computing constant for operation: 'unsigned sub.' 有符号转无符号，不清楚如何改动(关联到消息外部调用)，暂时忽略"
		nmhdr.code=TCN_SELCHANGE;  
		nmhdr.hwndFrom=m_hWnd;  
		nmhdr.idFrom=GetDlgCtrlID();  
		::SendMessage(pParent->GetSafeHwnd(),WM_NOTIFY,(WPARAM)GetDlgCtrlID(),(LPARAM)(&nmhdr));   
	}
}

int CGuiTabWnd::GetCurtab()
{
	return m_iSelectTab;
}


void CGuiTabWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CWnd::OnLButtonDown(nFlags, point);
	
	int32 iTabCur = -1;
	
	for (int iCont=m_iIndexBegin; iCont< m_Numtabs;iCont++)
	{
		CGuiTab* ct=(CGuiTab*) m_pArray[iCont];
		if (ct->rect.PtInRect(point) != 0)
		{
			iTabCur = iCont;
			if (m_bBkGroundBitmap)
			{
				CRect rcTmp = ct->rect;
				if (EGSStyle1 == m_eGraphicStyle)
				{
					rcTmp.left = rcTmp.right - (m_ibmpWidth / 6);
				}
				else if (EGSStyle2 == m_eGraphicStyle)
				{
					rcTmp.left = rcTmp.right - 30 + HIDE_WIDTH;
					rcTmp.right -= (15 - HIDE_WIDTH);
				}

				if (rcTmp.PtInRect(point))
				{
					DeleteTab(iCont);
				}
			}
			break;
		}
	}

	//
	bool32 bDealed = false;
	if ( NULL != m_pUserCB )
	{
		bDealed = m_pUserCB->OnLButtonDown2(point, iTabCur);
	}

	// 
	m_TipWnd.Hide();
	if ( !bDealed )
	{
		SetCurtab(iTabCur);
		m_InTab=TRUE;		
	}	
} 

static CString GetShort(UINT id)
{
	CString str;
	if (str.LoadString(id))
	{
		int nIndex = str.ReverseFind(_T('\n'));
		if(nIndex!=-1)
		{
			str=str.Mid(nIndex+1);
		}
	}	
	return str;
}
BOOL CGuiTabWnd::IsValid()
{
	if ( m_iSelectTab < 0 || m_iSelectTab >= m_Numtabs )
	{
		return FALSE;
	}
	return TRUE;
}
void CGuiTabWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	m_InTab=FALSE;
	
	
	// 先点一下左键吧
	int32 iTabCur = -1;	
	{
		for (int iCont=m_iIndexBegin; iCont< m_Numtabs;iCont++)
		{
			CGuiTab* ct=(CGuiTab*) m_pArray[iCont];
			
			if (ct->rect.PtInRect(point) != 0)
			{
				iTabCur = iCont;
				break;
			}
		}
		
		bool32 bBlindLbutton = false;
		if ( NULL != m_pUserCB )
		{
			bBlindLbutton = m_pUserCB->BlindLButtonBeforeRButtonDown(iTabCur);
		}
		
		// 
		if ( !bBlindLbutton )
		{
			CWnd::OnLButtonDown(nFlags, point);
			m_TipWnd.Hide();
			SetCurtab(iTabCur);
			m_InTab=TRUE;
		}
	}

	//
	if ( m_pUserCB )
	{
		m_pUserCB->OnRButtonDown2(point, iTabCur);
	}

	//m_pArray[m_iSelectTab]可能非法
	if ( !IsValid() )
	{
		return;
	}
	if (((CGuiTab*)m_pArray[m_iSelectTab])->nMenu > 0 && m_InTab==TRUE)
	{
		CNewMenu menu;
		menu.LoadMenu(((CGuiTab*)m_pArray[m_iSelectTab])->nMenu);
		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
		UINT uResID = (UINT)CTabSplitWnd::m_pMainFram->SendMessage(UM_GetIDRMainFram);
		ASSERT(uResID);
		pPopMenu->LoadToolBar(uResID);
		pPopMenu->SetMenuTitle(m_strTipTitle, MFT_ROUND|MFT_LINE|MFT_CENTER|MFT_SIDE_TITLE);		
		ClientToScreen(&point);
		
		pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,
			point.x, point.y, AfxGetMainWnd()/*, &rc*/);
		return;
	}
}
//Thanks to Ernesto Garcia
void CGuiTabWnd::SetStyle(Style estyle)
{
	m_style=estyle;
}

void CGuiTabWnd::OnDestroy()
{
// Libera la memoria
//     for( int i=0; i<m_Numtabs; i++ )
//     {
// 		CGuiTab *pTab = (CGuiTab*)m_pArray.GetAt(i);
// 		if( pTab )
// 			delete pTab;
//    }
	
//
	m_TipWnd.DestroyWindow();
	
//
	CWnd::OnDestroy();
	
	// TODO: Add your message handler code here
}

// daniel_h

void CGuiTabWnd::SetTabsTitle(int iTabNr, CString sNewTitle)
{
	if ( iTabNr < 0 || iTabNr >= m_pArray.GetSize() )
	{
		ASSERT( 0 );
		return;
	}

	CGuiTab* ct=(CGuiTab*) m_pArray[iTabNr];
	if (ct!=NULL)
	{
		ct->lpShortMsg = sNewTitle.Left(4);
		ct->lpMsg=sNewTitle;
		ct->lpTipMsg = sNewTitle;
		m_pArray[iTabNr]=ct;
		RecalLayout();		
	}
}
CString CGuiTabWnd::GetTabsTitle(int iTabNr)
{
	CGuiTab* ct=(CGuiTab*) m_pArray[iTabNr];
	if (ct!=NULL)
	{
		return ct->lpMsg;
	}	
	return AfxGetApp()->m_pszAppName;
}

CString CGuiTabWnd::GetTabsValue(int iTabNr)
{
	CGuiTab* ct=(CGuiTab*) m_pArray[iTabNr];
	if (ct!=NULL)
	{
		return ct->lpValue;
	}
	
	return L"";
}

void CGuiTabWnd::SetTabsValue(int iTabNr,CString cstrValue)
{
	if ( iTabNr < 0 || iTabNr >= m_pArray.GetSize() )
	{
		ASSERT( 0 );
		return;
	}

	CGuiTab* ct=(CGuiTab*) m_pArray[iTabNr];
	if (ct!=NULL)
	{
		ct->lpValue = cstrValue;
		m_pArray[iTabNr]=ct;
	}
}

void  CGuiTabWnd::SetTabMenu(int iNumTab, UINT uiMenuId)
{
	if (iNumTab >= 0 && iNumTab < m_Numtabs)
	{
		((CGuiTab*)m_pArray[iNumTab])->AddMenu(uiMenuId);
	}
}

void CGuiTabWnd::SetTabIcon(int iNumTab, UINT uiIconIndex)
{
	if (iNumTab >= 0 && iNumTab < m_Numtabs)
	{
		((CGuiTab*)m_pArray[iNumTab])->SetIcon(uiIconIndex);
	}
}

int CGuiTabWnd::GetFitFontW()
{
	CClientDC dc(this);
	CFont* pFontOld = dc.SelectObject(&m_FontHorz);

	int iFontWidth = GuiDrawLayer::GetTextWidth(&dc, _T("金"), FALSE)+2;	

	dc.SelectObject(pFontOld);
	return iFontWidth;
}

int CGuiTabWnd::GetFitHorW()
{
	CClientDC dc(this);
	CFont* pFontOld = dc.SelectObject(&m_FontHorz);
	
	int iFontWidth, iFontHeight;
	if (m_bBkGroundBitmap)
	{
		iFontWidth = m_ibmpWidth;
		iFontHeight = m_ibmpHeight;
	}
	else
	{
		iFontWidth	= GuiDrawLayer::GetTextWidth(&dc, _T("123"), FALSE);
		iFontHeight = GuiDrawLayer::GetTextHeight(&dc,_T("123"), TRUE);
	}


	int iReturnValue = -1;

	if (ALN_LEFT == m_alnTab || ALN_RIGHT == m_alnTab)
	{
		iReturnValue = iFontWidth > m_rectTabs.Width() ? iFontWidth : m_rectTabs.Width();		
	}
	else
	{
		iReturnValue = iFontHeight > m_rectTabs.Height() ? iFontHeight : m_rectTabs.Height();		
	}

	//
	dc.SelectObject(pFontOld);
	return iReturnValue;
}

void CGuiTabWnd::SetTipTitle(CString strTipTitle)
{
	m_strTipTitle = strTipTitle;
}

BOOL CGuiTabWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class
	CWnd::OnNotify(wParam,lParam,pResult);
	CWnd* pParentWnd= GetParent();
	
	if (pParentWnd->GetSafeHwnd())
		pParentWnd->SendMessage(WM_NOTIFY, wParam, lParam);
	
	return TRUE;
}

void CGuiTabWnd::GetSuitableDisplayShortName(IN CDC *pDC, IN const CRect &RectMax, IN const CString &StrLongName, OUT CString &StrShortName)
{
	StrShortName = L"";

	if ( NULL == pDC )
	{
		//ASSERT(0);
		return;
	}
	
	//
	int	iMaxDisplayLength =	0;
	
	CRect rectText = RectMax;
	StrShortName = StrLongName.Left(1);
	
	if ( ALN_LEFT == m_alnTab || ALN_RIGHT == m_alnTab )
	{
		//垂直显示
		iMaxDisplayLength = rectText.Height();
		for ( int i = 1; i <= StrLongName.GetLength(); i++ )
		{
			StrShortName = StrLongName.Left(i);
			int	iCurrentHeight = GuiDrawLayer::GetTextHeight(pDC, StrShortName, FALSE);
			if (iCurrentHeight > iMaxDisplayLength)
			{
				break;
			}
		}
	} 
	else
	{
		//水平显示
		iMaxDisplayLength	=	rectText.Width();
		int	iLongLength	=	StrLongName.GetLength();
		for (int i = 1; i <= iLongLength; i++)
		{
			StrShortName = StrLongName.Left(i);
			int	iCurrentHeight	=	GuiDrawLayer::GetTextWidth(pDC, StrShortName, TRUE);
			if (iCurrentHeight > iMaxDisplayLength)
			{
				break;
			}
		}
	}
	
	int	iLength = StrShortName.GetLength();
	if ( iLength > 1 )
	{
		//如果有进行过长度调整，则还原长度为最大能显示的长度
		StrShortName.Delete(iLength - 1);
	}	

}

void CGuiTabWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_PointLast != point)
	{
		m_PointLast = point;
		m_TipWnd.Hide();
		SetTimer(KTimerTip, 400, NULL);
	}

	if(ELSGraphic == m_eLayoutStyle)
	{
		for (int i = m_iIndexBegin; i < m_Numtabs; i++)
		{
			CGuiTab* ct=(CGuiTab*) m_pArray[i];
			
			if (NULL == ct)
				continue;
			
			if (PtInRect(&ct->rect, point))
			{
				m_iHotTab = i;
				Invalidate();
				break;
			}
		}

		// 响应 WM_MOUSELEAVE消息
		TRACKMOUSEEVENT csTME;
		csTME.cbSize	= sizeof (csTME);
		csTME.dwFlags	= TME_LEAVE;
		csTME.hwndTrack = m_hWnd ;
		::_TrackMouseEvent (&csTME);
	}

	CWnd::OnMouseMove(nFlags, point);
}

LRESULT CGuiTabWnd::OnMouseLeave(WPARAM wParam, LPARAM lParam)       
{     
	m_iHotTab = m_iSelectTab;
	Invalidate();
	return 0;       
} 

void CGuiTabWnd::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (KTimerTip == nIDEvent)
	{
		KillTimer(KTimerTip);
		
		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);

		CRect rectClient;
		GetClientRect(&rectClient);
		
		if (!PtInRect(&rectClient, point))
		{
			return;
		}

		// 判断需要显示文字
		BOOL bShowTip = FALSE;
		CString strTipMsg;
		
		for (int i = m_iIndexBegin; i < m_Numtabs; i++)
		{
			CGuiTab* ct=(CGuiTab*) m_pArray[i];
			
			if (NULL == ct)
				continue;
			
			if (PtInRect(&ct->rect, point))
			{
				if (ct->lpTipMsg.GetLength() > 0)
				{
					bShowTip = TRUE;
					strTipMsg = ct->lpTipMsg;
				}
				
				break;
			}
		}
		
		if (bShowTip)
		{
			strTipMsg += "\n";
			m_TipWnd.Show(point, strTipMsg, m_strTipTitle);
		}
		else
		{
			m_TipWnd.Hide();
		}
	}
	
	CWnd::OnTimer(nIDEvent);
}



void CGuiTabWnd::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	
	// TODO: Add your message handler code here
	CNewMenu::OnInitMenuPopup(m_hWnd,pPopupMenu, nIndex, bSysMenu);
}

void CGuiTabWnd::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	if(!CNewMenu::OnMeasureItem(GetCurrentMessage()))
    {
		CWnd::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
	}
}

LRESULT CGuiTabWnd::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu) 
{
	// TODO: Add your message handler code here and/or call default
	LRESULT lresult;
    if( DYNAMIC_DOWNCAST(CNewMenu,pMenu) )
      lresult=CNewMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
    else
		lresult=CWnd::OnMenuChar(nChar, nFlags, pMenu);

	return lresult;
}

BOOL CGuiTabWnd::GetTabRect( OUT CRect &RectTab, int32 iIndexTab )
{
	if ( iIndexTab >=0 && iIndexTab < m_Numtabs )
	{
		CGuiTab* ct=(CGuiTab*) m_pArray[iIndexTab];
		RectTab = ct->rect;
		return TRUE;
	}
	return FALSE;
}

void CGuiTabWnd::SetTabLength(int iTabLen)
{
	m_iDefTabLen = iTabLen;
}

CSize CGuiTabWnd::GetTabWishSize( int32 iIndexTab )
{
	static const int32 iDefaultHV = 45;	// 默认45
	CSize sizeRet(0,0);

	if ( iIndexTab < 0 || iIndexTab >= m_pArray.GetSize() )
	{
		//ASSERT(0);
		return sizeRet;
	}

	CClientDC dc(this);
	CFont *pOldFont = dc.SelectObject(&m_FontHorz);
	
	CGuiTab* ct = (CGuiTab*)m_pArray[iIndexTab];
	
	// 仅计算长文字		
	CString strText = ct->lpMsg;

	int32 iTextHV = iDefaultHV;
	
	
	if (ALN_LEFT == m_alnTab || ALN_RIGHT == m_alnTab)
	{
		iTextHV = GuiDrawLayer::GetTextHeight(&dc, strText, FALSE);
		
		if ( NULL != Image.m_hImageList)
		{
			if ( ct->uIcon < (UINT)Image.GetImageCount())
			{
				// 该条指定有图片需要显示
				ASSERT( 0 );	// 不支持图片
			}
		}
		iTextHV += 8*2; // 两边留点空隙
		sizeRet.cx = GetFitHorW();
		sizeRet.cy = iTextHV > iDefaultHV ? iTextHV : iDefaultHV;
	}
	else
	{
		iTextHV = GuiDrawLayer::GetTextWidth(&dc, strText, TRUE);
		
		if ( NULL != Image.m_hImageList)
		{
			if ( ct->uIcon < (UINT)Image.GetImageCount())
			{
				// 该条指定有图片需要显示
				ASSERT( 0 ); // 不支持图片
			}
		}

		iTextHV += 8*2; // 两边留点空隙
		sizeRet.cy = GetFitHorW();
		sizeRet.cx = iTextHV > iDefaultHV ? iTextHV : iDefaultHV;
	}
	
	dc.SelectObject(pOldFont);

	return sizeRet;
}

CSize CGuiTabWnd::GetWindowWishSize()
{
	CSize sizeTab(0,0);
	for ( int i=0;  i < m_pArray.GetSize() ; i++ )
	{
		CSize size1 = GetTabWishSize(i);
		if (ALN_LEFT == m_alnTab || ALN_RIGHT == m_alnTab)
		{
			sizeTab.cy = MAX(sizeTab.cy, size1.cy);
		}
		else
		{
			sizeTab.cx = MAX(sizeTab.cx, size1.cx);
		}
	}

	if (ALN_LEFT == m_alnTab || ALN_RIGHT == m_alnTab)
	{
		sizeTab.cx = GetFitHorW();
		sizeTab.cy *= m_pArray.GetSize();
	}
	else
	{
		sizeTab.cy = GetFitHorW();
		sizeTab.cx *= m_pArray.GetSize();
	}

	return sizeTab;
}

CPoint CGuiTabWnd::GetLeftTopPoint(int32 iTab)
{
	CPoint pt(-1, -1);
	
	if ( 0 == m_Numtabs || iTab < 0 || iTab > m_Numtabs )
	{
		return pt;
	}

	//
	CRect rect;
	GetClientRect(&rect);

	//
	int32 iLeft = rect.left + iTab * rect.Width() / (m_Numtabs-m_iIndexBegin);

	pt.x = iLeft;
	pt.y = rect.top;

	ClientToScreen(&pt);

	return pt;
}

bool CGuiTabWnd::SetFontHeight( int iHeight )
{
	ASSERT( iHeight != 0 );
	if ( iHeight != 0 )
	{
		LOGFONT lf;
		ZeroMemory(&lf, sizeof(lf));
		if ( m_FontHorz.GetLogFont(&lf) )
		{
			lf.lfHeight = iHeight;
			return SetFontByLF(lf);
		}
	}

	return false;
}

bool CGuiTabWnd::SetFontByLF( const LOGFONT &lf )
{
	CFont font;
	BOOL b = font.CreateFontIndirect(&lf);
	if ( b )
	{
		m_FontHorz.DeleteObject();
		m_FontHorz.Attach(font.Detach());

		LOGFONT lfBold = lf;
		lfBold.lfWeight = FW_BOLD;
		if ( font.CreateFontIndirect(&lfBold) )
		{
			m_FontHorzBold.DeleteObject();
			m_FontHorzBold.Attach(font.Detach());
			return true;
		}
	}
	return false;
}

bool CGuiTabWnd::SetFontByName( const CString &StrFaceName, BYTE lfCharSet )
{
	LOGFONT lf;
	ZeroMemory(&lf, sizeof(lf));
	if ( m_FontHorz.GetLogFont(&lf) )
	{
		_tcsncpy(lf.lfFaceName, StrFaceName, sizeof(lf.lfFaceName)/sizeof(lf.lfFaceName[0]));
		lf.lfCharSet = lfCharSet;	
		return SetFontByLF(lf);
	}
	return false;
}

int CGuiTabWnd::SetTabSpace( int iSpace )
{
	int iOld = m_iTabSpace;
	m_iTabSpace = iSpace;
	return iOld;
}

void CGuiTabWnd::SetTabBkColor( COLORREF clrNormal /*= CLR_DEFAULT*/, COLORREF clrSelect /*= CLR_DEFAULT*/ )
{
	const COLORREF clrBackNormal = RGB(58,62,70);
	const COLORREF clrBackSelect = RGB(42,46,52);
	//const COLORREF clrRound	   = RGB(76, 85, 118);

	if ( CLR_DEFAULT == clrNormal )
	{
		m_clrTabBkNormal = clrBackNormal;
	}
	else
	{
		m_clrTabBkNormal = clrNormal;
	}

	if ( CLR_DEFAULT == clrSelect )
	{
		m_clrTabBkSelect = clrBackSelect;
	}
	else
	{
		m_clrTabBkSelect = clrSelect;
	}
}

void CGuiTabWnd::GetTabBkColor( COLORREF &clrNormal, COLORREF &clrSelect )
{
	clrNormal = m_clrTabBkNormal;
	clrSelect = m_clrTabBkSelect;
}

void CGuiTabWnd::SetTabTextColor( COLORREF clrNormal /*= CLR_DEFAULT*/, COLORREF clrSelect /*= CLR_DEFAULT*/ )
{	
	if ( CLR_DEFAULT == clrNormal )
	{
		m_clrTabTextNormal = KColorTextNormal;
	}
	else
	{
		m_clrTabTextNormal = clrNormal;
	}
	
	if ( CLR_DEFAULT == clrSelect )
	{
		m_clrTabTextSelect = KColorTextSelect;
	}
	else
	{
		m_clrTabTextSelect = clrSelect;
	}	
}

void CGuiTabWnd::GetTabTextColor( COLORREF &clrNormal, COLORREF &clrSelect )
{
	clrNormal = m_clrTabTextNormal;
	clrSelect = m_clrTabTextSelect;
}

void CGuiTabWnd::SetTabFrameColor(COLORREF clrFrame)
{
	if ( CLR_DEFAULT == clrFrame )
	{
		m_clrTabFrame = KColorTabFrame;
	}
	else
	{
		m_clrTabFrame = clrFrame;
	}
}

void CGuiTabWnd::GetTabFrameColor(COLORREF& clrFrame)
{
	clrFrame = m_clrTabFrame;
}

void CGuiTabWnd::SetShowBeginIndex(int32 index, int32 iEnd)	
{
	if (0 > index)
	{
		m_iIndexBegin = 0;
	}

	m_iIndexBegin = index;
	m_iIndexEnd = iEnd;

	for (int i=m_iIndexBegin; i<m_iIndexEnd; i++)
	{
		CGuiTab* ct = (CGuiTab*)m_pArray[i];

		int iWidth = ct->rect.Width();
		ct->rect.left -= m_iIndexBegin*iWidth;
		ct->rect.right = ct->rect.left + iWidth; 
	}
}