/****************************************************************************
 *																			*		 
 *								GuiToolKit  								*	
 * (MFC extension)								*			 
 * Created by Francisco Campos G. www.beyondata.com fcampos@beyondata.com	*
 *--------------------------------------------------------------------------*		   
 *																			*
 * This program is free software; so you are free to use it any of your		*
 * applications(Freeware, Shareware, Commercial), but leave this header		*
 * intact.																	*
 *																			*
 * These files are provided "as is" without warranty of any kind.			*
 *																			*
 *			       GuiToolKit is forever FREE CODE !!!!!					*
 *																			*
 *--------------------------------------------------------------------------*
 * Created by: Francisco Campos G.											*
 * Bug Fixes and improvements :(Add your name)								*
 * -Francisco Campos														*				
 *																			*	
 ****************************************************************************/

#if !defined(AFX_GUIDRAWLAYER_H__0BFB5AA0_94B4_47FC_8F93_84DB87A364F8__INCLUDED_)
#define AFX_GUIDRAWLAYER_H__0BFB5AA0_94B4_47FC_8F93_84DB87A364F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




#define 	GUISTYLE_XP   0x08001 	
#define		GUISTYLE_2003 0x08002
#define		GUISTYLE_2003MENUBTN 0x08021
#define		WIN_NT		  0x08003 	
#define		WIN_2000	0x08004
#define		WIN_XP		0x08005
#define		WIN_95		0x08006
#define		WIN_98		0x08007
#define		WIN_32		0x08008
#define		WIN_ME		0x08009
#define		WIN_95OSR2	0x0800A
#define		WIN_98_SE	0x0800B


#include "dllexport.h"


class CONTROL_EXPORT GuiDrawLayer  
{
public:
	enum E_TextAlign
	{
		ETALeft = 0x01,
		ETARight = 0x02,
		ETATop = 0x04,
		ETABottom = 0x08,
		ETACenter = 0x10,
		ETAVCenter = 0x20
	};

public:
	GuiDrawLayer();
	virtual ~GuiDrawLayer();
public:
	static DWORD m_SO;
	static BOOL m_Theme;
	static DWORD m_Style;
	void		DrawArrow(CDC* pDC, CRect m_rc, BOOL m_bDown = TRUE);
	HICON		GetIcon(int nIcon);
	static HICON		LoadIconLib(UINT uIcon);
	static COLORREF	GetRGBColorXP();
	static COLORREF	GetRGBFondoXP();
	static COLORREF	GetRGBCaptionXP();
	static COLORREF	GetRGBPressBXP();
	static COLORREF	GetRGBColorFace(DWORD dwStyle=GUISTYLE_XP);
	static COLORREF	GetRGBColorShadow(DWORD IsTheme2003=GUISTYLE_XP);
	static COLORREF	GetRGBColorBTNHigh();
	static COLORREF	GetRGBColorTabs();
	static COLORREF	GetRGBColorWhite();
	static COLORREF GetRGBTitleMenu();
	static COLORREF GetRGBMenu();
	static COLORREF GetRGBColorGrayText();
	static void DrawCheck(CDC* pDC, CRect m_rcTemp);
	static BOOL		IsGrayed();
	static void DrawShade(CRect Rect, CPoint cp, CDC* pDC);
	static COLORREF DarkenColor(long lScale, COLORREF lColor);
	static COLORREF GetRGBSkinMenu();

	// bHorz 横排， 正常方式
	static int GetTextWidth(CDC *pDC, const CString& strText, BOOL bHorz = TRUE);
	static int GetTextHeight(CDC *pDC, const CString& strText, BOOL bHorz = TRUE);
	static void DrawText(CDC *pDC, const CString& strText, CRect rectText, UINT uiTextAlign, BOOL bHorz = TRUE);

	//*icon routines
	static HICON LoadIcon(UINT hIcon)
	{
		HINSTANCE hirc = AfxFindResourceHandle(MAKEINTRESOURCE(hIcon), RT_BITMAP);
	
		if (hIcon != -1) 
		{
			return (HICON)::LoadImage(hirc, MAKEINTRESOURCE(hIcon), 
			IMAGE_ICON, 16, 16, LR_LOADMAP3DCOLORS);
			
		}
		return NULL;
	}

	static CSize GetSizeIcon(HICON hIcon)
	{
		CSize m_SizeImage;
		ICONINFO bm;
		ZeroMemory(&bm, sizeof(ICONINFO));
		::GetIconInfo(hIcon,&bm);
		m_SizeImage =CSize((BYTE)(bm.xHotspot*2),(BYTE)(bm.yHotspot*2));
		::DeleteObject(bm.hbmMask);
		::DeleteObject(bm.hbmColor);
		return m_SizeImage;
	}


	static void DrawIcon(CDC* pDC, CPoint m_point,CSize m_sizeImg,HICON hIcon,BOOL Enabled=TRUE)
	{
		pDC->DrawState(m_point, m_sizeImg, hIcon,
					(Enabled== FALSE ? DSS_DISABLED:DSS_NORMAL), (CBrush*)NULL);
	}
	//**

	static HINSTANCE hTheme;
	
	static int verSO()
	{
		   
			if (m_SO!=MAXUINT)
				return m_SO;
		
		   OSVERSIONINFOEX osvi;
		   BOOL bOsVersionInfoEx;

		   // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
		   //
		   // If that fails, try using the OSVERSIONINFO structure.
				
		   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

		   if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
		   {
			  // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.

			  osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
			  if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
				 return FALSE;
		   }

		   switch (osvi.dwPlatformId)
		   {
			  case VER_PLATFORM_WIN32_NT:

			  // Test for the product.

				 if ( osvi.dwMajorVersion <= 4 )
						return m_SO=WIN_NT;

				 if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
						return m_SO=WIN_2000;;

				 if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
					    return m_SO=WIN_XP;

			  // Test for product type.

				 break;

			  case VER_PLATFORM_WIN32_WINDOWS:

				 if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
				 {
					 return WIN_95;
					 if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
						printf("OSR2 " );
				 } 

				 if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
				 {
					 return WIN_98;
					 if ( osvi.szCSDVersion[1] == 'A' )
						printf("SE " );
				 } 

				 if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
				 {
					 return m_SO=WIN_ME;
				 } 
				 break;

			  case VER_PLATFORM_WIN32s:

				 return m_SO=WIN_32;
				 break;
		   }
		   return -1; 

	}

	static BOOL IsThemeXP()
	{

		// zhangbo 0313 # //
// 		if (m_SO < WIN_XP) return FALSE;
// 		CVisualStylesXP  m_vsXP;
// 		if (m_Theme=m_vsXP.IsAppThemed())
 			return TRUE;
//		return FALSE;
			

	}
	static void GetNextColor(int nIndex, COLORREF& m_lowColor, COLORREF& m_HighColor)
	{

		int nResult= (nIndex >10 ) ? nIndex % 10 : nIndex;
		switch(nResult)
		{
		case 1:
			m_lowColor=RGB(255,255,255);
			m_HighColor=RGB (147,174,230);
			break;
		case 2:
			m_lowColor=RGB(255,255,255);
			m_HighColor=RGB( 255,223,133);
			break;
		case 3:
			m_lowColor=RGB(255,255,255);
			m_HighColor=RGB (210,221,189);
			break;
		case 4:
			m_lowColor=RGB(255,255,255);
			m_HighColor=RGB (244,189,189);
			break;
		case 5:
			m_lowColor=RGB(255,255,255);
			m_HighColor=RGB (210,198,235);
			break;
		case 6:
			m_lowColor=RGB(255,255,255);
			m_HighColor=RGB (186,211,204);
			break;
		case 7:
			m_lowColor=RGB(255,255,255);
			m_HighColor=RGB (247,202,165);
			break;
		case 8:
			m_lowColor=RGB(255,255,255);
			m_HighColor=RGB (219,177,197);
			break;
		case 9:
			m_lowColor=RGB(255,255,255);
			m_HighColor=RGB (180,199,237);
			break;
		case 10:
			m_lowColor=RGB(255,255,255);
			m_HighColor=RGB (244,189,189);
			break;
		default:
			m_lowColor=RGB(255,255,255);
			m_HighColor=RGB (247,202,165);
		}
	}

	// 获取平滑效果的颜色
	// iStep 从 1开始编号， iCount至少为3
	static void GetSmoothColor(COLORREF ClrStart, COLORREF ClrEnd, int iStep, int iCount, COLORREF &ClrSmooth)
	{
		if (iCount < 3 || iStep < 1 || iStep > iCount)
		{
			ClrSmooth = ClrStart;
			return;
		}
		
		if (1 == iStep)	
		{
			ClrSmooth = ClrStart;
			return;
		}
		else if (iCount == iStep)
		{
			ClrSmooth = ClrEnd;
			return;
		}
		else
		{
			int iRStart	= (ClrStart & 0xff);
			int iGStart	= ((ClrStart >> 8) & 0xff);
			int iBStart	= ((ClrStart >> 16) & 0xff);
		
			int iREnd	= (ClrEnd & 0xff);
			int iGEnd	= ((ClrEnd >> 8) & 0xff);
			int iBEnd	= ((ClrEnd >> 16) & 0xff);

			int iRStep	= (iREnd - iRStart) / (iCount - 1);
			int iGStep	= (iGEnd - iGStart) / (iCount - 1);
			int iBStep	= (iBEnd - iBStart) / (iCount - 1);

			int iRSmooth = iRStart;
			int iGSmooth = iGStart;
			int iBSmooth = iBStart;
			for (int i = 1; i < iStep; i++)
			{
				iRSmooth += iRStep;
				iGSmooth += iGStep;
				iBSmooth += iBStep;
			}

			ClrSmooth = RGB(iRSmooth, iGSmooth, iBSmooth);
		}		
	}

protected:
	CImageList	m_img;
	CPoint		mHeight;
	int			m_iWidthDrowDown;
	
};


class CONTROL_EXPORT CGradient  
{
public:
	CGradient(CSize=CSize(800,600));
	virtual ~CGradient();
	//members
	virtual void PrepareVertical(CDC* pDC,DWORD m_StyleDisplay=GUISTYLE_XP,COLORREF m_clr=GuiDrawLayer::GetRGBColorTabs(), int nRate=30);
	virtual void PrepareVertical(CDC *pDC,UINT RTop,UINT GTop,UINT BTOP,UINT RBot,UINT GBot,UINT BBot);
	virtual void PrepareHorizontal(CDC* pDC,DWORD m_StyleDisplay=GUISTYLE_XP,COLORREF m_clr=GuiDrawLayer::GetRGBColorTabs());
	virtual void PrepareReverseHor(CDC* pDC,DWORD m_StyleDisplay=GUISTYLE_XP,COLORREF m_clr=GuiDrawLayer::GetRGBColorTabs());
	void PrepareTabs(CDC* pDC, COLORREF m_clrL,COLORREF m_clrH );
	void PrepareCaption(CDC* pDC,DWORD m_StyleDisplay=GUISTYLE_XP);
	void PrepareCaptionVert(CDC* pDC,DWORD m_StyleDisplay=GUISTYLE_XP);
	void PrepareReverseVert(CDC* pDC,DWORD m_StyleDisplay=GUISTYLE_XP, COLORREF m_clr=GuiDrawLayer::GetRGBColorTabs());
	void PrepareReverseVertTab(CDC* pDC,DWORD m_StyleDisplay=GUISTYLE_XP, COLORREF m_clr=GuiDrawLayer::GetRGBColorTabs());
	void PrepareReverseColorTab(CDC* pDC,DWORD m_StyleDisplay=GUISTYLE_XP, COLORREF m_clrL=GuiDrawLayer::GetRGBColorTabs(), COLORREF m_clrH=GuiDrawLayer::GetRGBColorTabs());
	void PrepareHorizontal(CDC *pDC,UINT RRight,UINT GRight,UINT BRight,UINT RLeft,UINT GLeft,UINT BLeft);
	CSize GetDimensions();
	void  SetDimensions(CSize Size);
	void Draw(CDC *pDC, int xDest,int yDest,int xSrc, int ySrc, int Width, int Height,DWORD Rop);

protected:
	CDC *m_dcMem;
	CSize m_Size;


};

#endif // !defined(AFX_GUIDRAWLAYER_H__0BFB5AA0_94B4_47FC_8F93_84DB87A364F8__INCLUDED_)
