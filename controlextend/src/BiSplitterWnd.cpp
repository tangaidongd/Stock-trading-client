// BiSplitterWnd.cpp : implementation file
//

#include "stdafx.h"
#include "afxpriv.h"
#include "BiSplitterWnd.h"
#include <float.h>
#include <shlwapi.h>

#include "memdc.h"
#include "facescheme.h"
#include "TabSplitWnd.h"
#include "faceschemetype.h"
//TODO: check styles during pane and splitter wnd construction
//TODO: add function for view delete
#define VALIDAREASIZE		50
/***************************************************************************************************************/
/** follow definitions were copied from file \Microsoft Visual Studio .NET 2003\Vc7\atlmfc\src\mfc\afximpl.h  **/
/**/ 
/**/ #ifndef _UNICODE
/**/ #define _UNICODE_SUFFIX
/**/ #else
/**/ #define _UNICODE_SUFFIX _T("u")
/**/ #endif
/**/ 
/**/ #ifndef _DEBUG
/**/ #define _DEBUG_SUFFIX
/**/ #else
/**/ #define _DEBUG_SUFFIX _T("d")
/**/ #endif
/**/ 
/**/ #ifdef _AFXDLL
/**/ #define _STATIC_SUFFIX
/**/ #else
/**/ #define _STATIC_SUFFIX _T("s")
/**/ #endif

/**/ #if (_MSC_VER == 1200) /* VS 6 */
/**/ #define AFX_WNDCLASS(s) _T("Afx") _T(s) _T("42") _STATIC_SUFFIX _UNICODE_SUFFIX _DEBUG_SUFFIX
/**/ #elif (_MSC_VER == 1310) /* VS .NET 2003 */
/**/ #define AFX_WNDCLASS(s) _T("Afx") _T(s) _T("70") _STATIC_SUFFIX _UNICODE_SUFFIX _DEBUG_SUFFIX
/**/ #elif (_MSC_VER == 1400)/*vc8.0 ����vs2005*/
/**/ #define AFX_WNDCLASS(s) _T("Afx") _T(s) _T("80") _STATIC_SUFFIX _UNICODE_SUFFIX _DEBUG_SUFFIX
/**/ #elif (_MSC_VER == 1500)/*vc9.0 ����vs2008*/
/**/ #define AFX_WNDCLASS(s) _T("Afx") _T(s) _T("90") _STATIC_SUFFIX _UNICODE_SUFFIX _DEBUG_SUFFIX
/**/ #elif (_MSC_VER == 1600)/*vc10.0 ����vs2010*/
/**/ #define AFX_WNDCLASS(s) _T("Afx") _T(s) _T("100") _STATIC_SUFFIX _UNICODE_SUFFIX _DEBUG_SUFFIX
/**/ #else
/**/ #error Unsupported version of MFC
/**/ #endif
/**/ 
/**/ #define AFX_WNDMDIFRAME_REG                             0x00004
/**/ #define AFX_WNDMDIFRAME     AFX_WNDCLASS("MDIFrame")
/**/ #define AfxDeferRegisterClass(fClass) AfxEndDeferRegisterClass(fClass)
/**/ BOOL AFXAPI AfxEndDeferRegisterClass(LONG fToRegister);
/**/ #define _AfxGetDlgCtrlID(hWnd)          ((UINT)(WORD)::GetDlgCtrlID(hWnd))
/**/ 
/***************************************************************************************************************/

#define IMAGE_GRIP_L 0
#define IMAGE_GRIP_R 1
#define IMAGE_GRIP_T 2
#define IMAGE_GRIP_B 3
#define IMAGE_CLOSE 4


enum HitTestValue
{
	noHit                   = 0,
	splitterBar             = 1,
	closeBtn0				= 2,
	closeBtn1				= 3,
	gripBtn0				= 4,
	gripBtn1				= 5
};

static const int32	KiSensitiveSize					= 1;				// ���гߴ�
static const double KdF9Radion						= 0.99999999;		// F7 �ı���

// �������ص�xml�����������Ϣ
static const char * KStrElementValue				= "DualSplitWnd";	// �Ȳ�������.����ǰ�ļ���	
static const char * KStrElementAttrIsH				= "isH";				
static const char * KStrElementAttrScale			= "Scale";   
static const char * KStrElementAttrRightRect		= "RightRect"; 
static const char * KStrElementAttrTopRect			= "TopRect";
static const char * KStrElementAttrCanDrag			= "CanDrag"; 

#define INVALID_ID                 -1
#define ID_ZOOM_RIGHT              6600
#define ID_ZOOM_TOP				   6601

// CBiSplitterWnd

IMPLEMENT_DYNCREATE(CBiSplitterWnd, CWnd)
BEGIN_MESSAGE_MAP(CBiSplitterWnd, CWnd)
	ON_WM_NCCREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_SETCURSOR()
	ON_WM_PAINT()
	ON_WM_CAPTURECHANGED()
	ON_WM_CANCELMODE()
	ON_WM_GETMINMAXINFO()
	ON_MESSAGE_VOID(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_MESSAGE_VOID(WM_WININICHANGE, OnDisplayChange)
	ON_MESSAGE_VOID(WM_SETTINGCHANGE, OnDisplayChange)
//	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//*** Construction ***
CBiSplitterWnd::CBiSplitterWnd():
		m_trackingState(NOTRACKING),
        m_nGap(3),
        m_nSplitterPos(0),
        m_nSplitterDenominator(0),
		m_nSplitterNumerator(0),
		m_lastActivePane(-1),
		m_isTrackMouseEvent(FALSE),
		m_clrFlatBorder(RGB(0,0,0)),
        m_clrFlatBtnActiveFace(RGB(182,189,210)),
		m_clrFlatBtnActiveBorder(RGB(10,36,106)),
		m_pGlyphs(NULL),
		m_pSmGlyphs(NULL),
		m_glyphsCount(5),
		m_autoDelete(FALSE)
{
	//
	m_images = new TCHAR[m_glyphsCount];
	m_images[0] = _T('\x33');
	m_images[1] = _T('\x34');
	m_images[2] = _T('\x36');
	m_images[3] = _T('\x35');
	m_images[4] = _T('\x72');


	m_pGlyphs = new GLYPHINFO[m_glyphsCount];
	m_pSmGlyphs = new GLYPHINFO[m_glyphsCount];
	//lint --e{415} suppress "across the bound"
	for(int i = 0; i < m_glyphsCount; i++)
	{
		m_pGlyphs[i].pBitmapInfo = (BITMAPINFO*)(new BYTE[sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD)]);
		m_pGlyphs[i].pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		m_pGlyphs[i].pBitmapInfo->bmiHeader.biPlanes = 1;
		m_pGlyphs[i].pBitmapInfo->bmiHeader.biBitCount = 1;
		m_pGlyphs[i].pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	
		m_pGlyphs[i].pBitmapInfo->bmiColors[0].rgbRed = 255;	
		m_pGlyphs[i].pBitmapInfo->bmiColors[0].rgbGreen = 255;
		m_pGlyphs[i].pBitmapInfo->bmiColors[0].rgbBlue = 255;
		m_pGlyphs[i].pBitmapInfo->bmiColors[0].rgbReserved = 0;

		m_pGlyphs[i].pBitmapInfo->bmiColors[1].rgbRed = 0;	
		m_pGlyphs[i].pBitmapInfo->bmiColors[1].rgbGreen = 0;
		m_pGlyphs[i].pBitmapInfo->bmiColors[1].rgbBlue = 0;
		m_pGlyphs[i].pBitmapInfo->bmiColors[1].rgbReserved = 0;

		m_pGlyphs[i].pBits = 0;


		m_pSmGlyphs[i].pBitmapInfo = (BITMAPINFO*)(new BYTE[sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD)]);
		m_pSmGlyphs[i].pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		m_pSmGlyphs[i].pBitmapInfo->bmiHeader.biPlanes = 1;
		m_pSmGlyphs[i].pBitmapInfo->bmiHeader.biBitCount = 1;
		m_pSmGlyphs[i].pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	
		m_pSmGlyphs[i].pBitmapInfo->bmiColors[0].rgbRed = 255;	
		m_pSmGlyphs[i].pBitmapInfo->bmiColors[0].rgbGreen = 255;
		m_pSmGlyphs[i].pBitmapInfo->bmiColors[0].rgbBlue = 255;
		m_pSmGlyphs[i].pBitmapInfo->bmiColors[0].rgbReserved = 0;

		m_pSmGlyphs[i].pBitmapInfo->bmiColors[1].rgbRed = 0;	
		m_pSmGlyphs[i].pBitmapInfo->bmiColors[1].rgbGreen = 0;
		m_pSmGlyphs[i].pBitmapInfo->bmiColors[1].rgbBlue = 0;
		m_pSmGlyphs[i].pBitmapInfo->bmiColors[1].rgbReserved = 0;

		m_pSmGlyphs[i].pBits = 0;
	}

	m_dRatio		= 0.0;
	m_dRatioXml		= 0.0;
	m_dF9Ratio		= 0.0;
	m_bRightBtn		= false;
	m_bTopBtn		= false;
	m_bCanDrag		= true;
	
	m_RectDelText	= CRect(0,0,0,0);
	m_pActiveView	= NULL;

	UpdateSysMetrics();
	UpdateSysColors();
//	UpdateSysImages();

	m_pImgZoomLeft = NULL;
	m_pImgZoomRight = NULL;
	m_iRightBtnHovering = INVALID_ID;
	m_bRightZoom = false;
	m_bShowRightBk = true;
	m_iRightBtnWidth = 4;
	m_iTopBtnHeight = 4;

	m_pImgZoomTop = NULL;
	m_pImgZoomBottom = NULL;
	m_iTopBtnHovering = INVALID_ID;
	m_bTopZoom = false;
	m_bShowTopBk = true;
}

BOOL CBiSplitterWnd::Create(CWnd *pWnd, UINT bswStyles, UINT nID /* = AFX_IDW_PANE_FIRST */)
{
	ASSERT(pWnd != NULL);
	if(NULL == pWnd)
	{
		return FALSE;
	}
	ASSERT(bswStyles & SPLITTER_STYLES);
	ASSERT(bswStyles & SIZING_MODE_STYLES);

	DWORD dwCreateStyle = WS_CHILD;// | WS_VISIBLE;
	VERIFY(AfxDeferRegisterClass(AFX_WNDMDIFRAME_REG));
	m_bswStyles = bswStyles;

	// create with the same wnd-class as MDI-Frame (no erase bkgnd)
	const TCHAR _afxWndMDIFrame[] = AFX_WNDMDIFRAME;
	if (!CreateEx(0, _afxWndMDIFrame, NULL, dwCreateStyle, 0, 0, 0, 0,
	  pWnd->m_hWnd, (HMENU)(UINT_PTR)nID, NULL))
		return FALSE;       // create invisible

	UpdateSysMetrics();
	UpdateSysColors();
	UpdateSysImages();

	//BOOL rc = tip.CreateEx(this,TTS_ALWAYSTIP | WS_POPUP);
	//tip.SetWindowPos(&CWnd::wndTop,0, 0, 0, 0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	//tip.AddTool(this,/*LPSTR_TEXTCALLBACK*/ "YYYY" , CRect(0,0,100,100), nID);
	//CToolInfo toolInfo;
	//tip.GetToolInfo(toolInfo, this, nID);
	//toolInfo.uFlags |= TTF_SUBCLASS ;
	//tip.SetToolInfo(&toolInfo);
	//tip.Activate(TRUE);

	//CString str;
	//tip.GetText(str, this, nID);

	if (m_bRightBtn || m_bTopBtn)
	{
		SetSplitterGap(0);
	}
	else
	{
		SetSplitterGap(8);
	}
	
	return TRUE;
}

BOOL CBiSplitterWnd::CreatePane(UINT nPane, UINT bspStyles, LPCTSTR lpszPaneCaptionText/* = NULL */, CWnd *pWnd/* = NULL*/)
{
    ASSERT(nPane<=1);
	if ( !(nPane < sizeof(m_panes)/sizeof(m_panes[0]))) 
	{
		return FALSE;
	}
	ASSERT((bspStyles & BORDER_STYLES) == NOTHING || (bspStyles & BORDER_STYLES) == THIN_BORDER || (bspStyles & BORDER_STYLES) == THICK_BORDER);
	ASSERT((bspStyles & CAPTION_STYLES) == NOTHING || (bspStyles & CAPTION_STYLES) == SMCAPTION || (bspStyles & CAPTION_STYLES) == CAPTION);
	ASSERT_VALID(this);

	if ((RIGHT_BTN==bspStyles) && (NULL==m_pImgZoomLeft))
	{
		//��ʼ��ͼƬ
		TCHAR szAbsPathFile[MAX_PATH+1]={NULL};
		GetModuleFileName(NULL, szAbsPathFile, MAX_PATH);
		PathRemoveFileSpec(szAbsPathFile);
		CString strPath = szAbsPathFile;
		strPath += L"\\";

		m_pImgZoomLeft = Image::FromFile(strPath + L"image//zoomRight.png");
		m_pImgZoomRight = Image::FromFile(strPath + L"image//zoomLeft.png");

		if (m_pImgZoomLeft != NULL)
		{
			m_iRightBtnWidth = m_pImgZoomLeft->GetWidth();
		}

		//��ʼ����ť
		CRect rtZoom(0,0,0,0);
		m_RightBtnZoom.CreateButton(L"", rtZoom, this, m_pImgZoomLeft, 3, ID_ZOOM_RIGHT);
	}

	if ((TOP_BTN==bspStyles) && (NULL==m_pImgZoomTop))
	{
		//��ʼ��ͼƬ
		TCHAR szAbsPathFile[MAX_PATH+1]={NULL};
		GetModuleFileName(NULL, szAbsPathFile, MAX_PATH);
		PathRemoveFileSpec(szAbsPathFile);
		CString strPath = szAbsPathFile;
		strPath += L"\\";

		m_pImgZoomTop = Image::FromFile(strPath + L"image//zoomTop.png");
		m_pImgZoomBottom = Image::FromFile(strPath + L"image//zoomBottom.png");

		if (m_pImgZoomTop != NULL)
		{
			m_iTopBtnHeight = m_pImgZoomTop->GetHeight()/3;
		}

		//��ʼ����ť
		CRect rtZoom(0,0,0,0);
		m_TopBtnZoom.CreateButton(L"", rtZoom, this, m_pImgZoomTop, 3, ID_ZOOM_TOP);
	}

	CPane &curPane = m_panes[nPane];
	ASSERT( curPane.m_pView == NULL);
	curPane.m_bspStyles = bspStyles;
	curPane.m_sCaptionText = lpszPaneCaptionText;
	if(pWnd != NULL) 
	{
		pWnd->SetWindowPos(&CWnd::wndTop, curPane.m_rctViewBox.left, curPane.m_rctViewBox.top, curPane.m_rctViewBox.Width(), curPane.m_rctViewBox.Height(), 0/*SWP_SHOWWINDOW*/);
		
		if(pWnd->IsKindOf(RUNTIME_CLASS(CView)))
		{
			CFrameWnd* pFrameWnd = GetParentFrame();
			ASSERT_VALID(pFrameWnd);
			pFrameWnd->SetActiveView((CView*)pWnd);
		}
		curPane.m_pView = pWnd;
	}

	if( curPane.IsVisible() && !(GetStyle() & WS_VISIBLE))//BiSplitter window not yet visible
        ShowWindow(SW_SHOW);
	
	SetActivePane(nPane);
	return TRUE;
}

CWnd *CBiSplitterWnd::CreateView(CRuntimeClass* pBiSplitterWndClass, UINT nID, UINT bswStyles)
{
	ASSERT_VALID(this);

	ASSERT(pBiSplitterWndClass != NULL);
	if (NULL == pBiSplitterWndClass)
	{
		return NULL;
	}
	ASSERT(pBiSplitterWndClass->IsDerivedFrom(RUNTIME_CLASS(CBiSplitterWnd)));
	if(!pBiSplitterWndClass->IsDerivedFrom(RUNTIME_CLASS(CBiSplitterWnd)))
	{
		return NULL;
	}
	ASSERT(AfxIsValidAddress(pBiSplitterWndClass, sizeof(CRuntimeClass), FALSE));
	if(!AfxIsValidAddress(pBiSplitterWndClass, sizeof(CRuntimeClass), FALSE))
	{
		return NULL;
	}
	ASSERT(bswStyles & SPLITTER_STYLES);
	ASSERT(bswStyles & SIZING_MODE_STYLES);

    CBiSplitterWnd* pBiSplitterWnd;
	pBiSplitterWnd = (CBiSplitterWnd*)pBiSplitterWndClass->CreateObject();
	if (NULL != pBiSplitterWnd)
	{
		pBiSplitterWnd->m_autoDelete = TRUE;
		ASSERT_KINDOF(CBiSplitterWnd, pBiSplitterWnd);
		ASSERT(pBiSplitterWnd->m_hWnd == NULL);       // not yet created
		if (!pBiSplitterWnd->Create(this, bswStyles, nID))
		{
			// TRACE(traceAppMsg, 0, _T("Warning: couldn't create subsplitter.\n"));
			// pWnd will be cleaned up by PostNcDestroy
			return NULL;
		}
		if (NULL != pBiSplitterWnd->m_hWnd)
		{
			ASSERT((int)_AfxGetDlgCtrlID(pBiSplitterWnd->m_hWnd) == nID);
		}
	

	}
	else
	{
		AfxThrowMemoryException();
	}	
	return pBiSplitterWnd;
}

CWnd *CBiSplitterWnd::CreateView(CRuntimeClass* pWndClass, UINT nID)
{
	ASSERT_VALID(this);
	ASSERT(pWndClass != NULL);
	if (NULL == pWndClass)
	{
		return NULL;
	}
	ASSERT(pWndClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	if (!pWndClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)))
	{
		return FALSE;
	}
	ASSERT(AfxIsValidAddress(pWndClass, sizeof(CRuntimeClass), FALSE));
	if (!AfxIsValidAddress(pWndClass, sizeof(CRuntimeClass), FALSE))
	{
		return FALSE;
	}
    CWnd* pWnd;
	pWnd = (CWnd*)pWndClass->CreateObject();
	if (pWnd == NULL)
	{
		AfxThrowMemoryException();
	}
	else
	{
		ASSERT_KINDOF(CWnd, pWnd);
		ASSERT(pWnd->m_hWnd == NULL);       // not yet created
		if (NULL == pWnd->m_hWnd)
		{
			DWORD dwStyle = WS_CHILD | WS_VISIBLE;
			// Create with the zero size
			if (!pWnd->Create(NULL, NULL, dwStyle, CRect(0,0,0,0), this, nID, NULL))
			{
				// TRACE(traceAppMsg, 0, _T("Warning: couldn't create pane window for splitter.\n"));
				// pWnd will be cleaned up by PostNcDestroy
				return NULL;
			}			
			ASSERT((int)_AfxGetDlgCtrlID(pWnd->m_hWnd) == nID);
		}
		
	}



	return pWnd;
}

CWnd *CBiSplitterWnd::CreateView(CRuntimeClass* pViewClass, UINT nID, CCreateContext* pContext)
{
	ASSERT_VALID(this);
	ASSERT(pViewClass != NULL);
	if(NULL == pViewClass)
	{
		return NULL;
	}
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CView)));
	if (!pViewClass->IsDerivedFrom(RUNTIME_CLASS(CView)))
	{
		return NULL;
	}
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));
	if(!AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE))
	{
		return NULL;
	}
    CView* pView;
	pView = (CView*)pViewClass->CreateObject();
	if (pView == NULL)
	{
		AfxThrowMemoryException();
	}
	else
	{
		ASSERT_KINDOF(CView, pView);
		ASSERT(pView->m_hWnd == NULL);       // not yet created
		if (NULL == pView->m_hWnd)
		{
			DWORD dwStyle = WS_CHILD | WS_VISIBLE;

			// Create with the zero size
			if (!pView->Create(NULL, NULL, dwStyle, CRect(0,0,0,0), this, nID, pContext))
			{
				// TRACE(traceAppMsg, 0, _T("Warning: couldn't create view for splitter.\n"));
				// pWnd will be cleaned up by PostNcDestroy
				return NULL;
			}
			ASSERT((int)_AfxGetDlgCtrlID(pView->m_hWnd) == nID);
		}		
	}
	return pView;
}


//*** Attributes ***
UINT CBiSplitterWnd::GetSplitterGap()
{
	return m_nGap;
}

void CBiSplitterWnd::SetSplitterGap(UINT nGap)
{
	m_nGap=nGap;
	RecalcLayout();
}

int CBiSplitterWnd::GetSplitterPos()
{
	return m_nSplitterPos;
}

void CBiSplitterWnd::SetSplitterPos(int nSplitterPos)
{
	m_nSplitterNumerator = m_nSplitterPos = nSplitterPos;
	CRect rct;
	GetClientRect(&rct);
	if(m_bswStyles & VSPLITTER)
		m_nSplitterDenominator =  rct.Width();
	else
		m_nSplitterDenominator =  rct.Height();
	RecalcLayout();
}

CWnd *CBiSplitterWnd::GetPaneView(UINT nPane)
{
	ASSERT(nPane <= 1);
	if (!(nPane < sizeof(m_panes)/sizeof(m_panes[0]))) 
	{
		return NULL;
	}
	return m_panes[nPane].m_pView;
}


UINT CBiSplitterWnd::GetPaneViewID(UINT nPane)
{
	ASSERT(nPane <= 1);
	if (!(nPane < sizeof(m_panes)/sizeof(m_panes[0]))) 
	{
		return 0;
	}
	ASSERT(m_panes[nPane].m_pView != NULL);
	if (NULL != m_panes[nPane].m_pView)
	{
		return m_panes[nPane].m_pView->GetDlgCtrlID();
	}
	return 0;
	
}

CString CBiSplitterWnd::GetPaneCaptionText(UINT nPane)
{
    ASSERT(nPane <= 1);
	if (!(nPane < sizeof(m_panes)/sizeof(m_panes[0]))) 
	{
		return m_panes[0].m_sCaptionText;
	}
	return m_panes[nPane].m_sCaptionText;
}

void CBiSplitterWnd::SetPaneCaptionText(UINT nPane, const CString &m_sCaptionText)
{
    ASSERT(nPane <= 1);
	if (!(nPane < sizeof(m_panes)/sizeof(m_panes[0]))) 
	{
		return ;
	}
	m_panes[nPane].m_sCaptionText=m_sCaptionText;
	RecalcLayout();
}

//*** Operations ***
UINT CBiSplitterWnd::AcquireStyles()
{
	return m_bswStyles;
}

void CBiSplitterWnd::ChangeStyles(UINT excludeStyles, UINT includeStyles)
{
	m_bswStyles &= ~excludeStyles;
	m_bswStyles |= includeStyles;
	RecalcLayout();
}

UINT CBiSplitterWnd::AcquirePaneStyles(UINT nPane)
{
    ASSERT(nPane <= 1);
	if ( !(nPane < sizeof(m_panes)/sizeof(m_panes[0]))) 
	{
		return m_panes[0].m_bspStyles;
	}
	return m_panes[nPane].m_bspStyles;
}

void CBiSplitterWnd::ChangePaneStyles(UINT nPane, UINT excludeStyles, UINT includeStyles)
{
    ASSERT(nPane <= 1);
	if ( !(nPane < sizeof(m_panes)/sizeof(m_panes[0]))) 
	{
		return ;
	}
	m_panes[nPane].m_bspStyles &= ~excludeStyles;
	m_panes[nPane].m_bspStyles |= includeStyles;
	RecalcLayout();
}

void CBiSplitterWnd::AssignViewToPane(UINT viewID, UINT nPane)
{
	//TODO: add posibility to assign NULL view
    ASSERT(nPane <= 1);
	if ( !(nPane < sizeof(m_panes)/sizeof(m_panes[0]))) 
	{
		return ;
	}

	if (m_panes[nPane].m_pView)
		m_panes[nPane].m_pView->ShowWindow(SW_HIDE);
	CWnd *pWnd = GetDlgItem(viewID);
	ASSERT(pWnd);
	if (NULL != pWnd)
	{
		m_panes[nPane].m_pView = pWnd;
		pWnd->SetWindowPos(NULL, m_panes[nPane].m_rctViewBox.left, m_panes[nPane].m_rctViewBox.top,
			m_panes[nPane].m_rctViewBox.right, m_panes[nPane].m_rctViewBox.bottom, SWP_SHOWWINDOW);
	}
	
	
}

void CBiSplitterWnd::RecalcLayout()
{
	
	/*	Pane can be in 4 states:
		1. visible and not gripped - normal state
		2. hidden and not gripped - pane and splitter gap are not displayed
		3. hidden and gripped - the same as 2 state 
		4. visible and gripped - pane is not displayed, splitter gap is docked to splitter window border 		

		Both panes can be in 16 states.

	*/
	if ( 0.0 != m_dRatioXml )
	{
		AdjustLayoutByRadioXml();
		return;
	}

	CRect rctClient;
	GetClientRect(&rctClient);

	BYTE state = (((((m_panes[0].IsVisible() << 1) | m_panes[0].IsGripped()) << 1) | m_panes[1].IsVisible()) << 1) | m_panes[1].IsGripped();
	
	//zero bit of the state show splitter orientation 0 - vertical 1 - horizontal
	ASSERT(m_bswStyles & SPLITTER_STYLES);
	state <<= 1;
	if(m_bswStyles & HSPLITTER)
		state = state | 1; 
		
	switch (state)
	{
	case 0x00:	//00000 
	case 0x01:	//00001 
		{//both pane are hidden and not gripped
			//draw empty splitter window - fill it with background color
			RclHideBothPanes();
            break;
		}
	case 0x02:	//00010
	case 0x03:	//00011
		{//left pane is hidden and not gripped, right pane is hidden and gripped
			//draw empty splitter window - fill it with background color
			RclHideBothPanes();
			break;
		}
	case 0x04:	//00100
	case 0x05:	//00101
		{//left pane is hidden and not gripped, right pane is visible and not gripped
			//expand the right pane into the whole splitter window
			RclExpandSecondPane(rctClient, FALSE);
			break;
		}
	case 0x06:	//00110
	case 0x07:	//00111
		{//left pane is hidden and not gripped, right pane is visible and gripped
			//expand the right pane into the whole splitter window
			RclExpandSecondPane(rctClient, FALSE);
			break;
		}
	case 0x08:	//01000
	case 0x09:	//01001
		{//left pane is hidden and gripped, right pane is hidden and not gripped
			//draw empty splitter window - fill it with background color
			RclHideBothPanes();
			break;
		}
	case 0x0A:	//01010
	case 0x0B:	//01011
		{//left pane is hidden and gripped, right pane is hidden and gripped
			//This is wrong situation - both pane can not be gripped simultaneously
			ASSERT(FALSE);
			break;
		}
	case 0x0C:	//01100
	case 0x0D:	//01101
		{//left pane is hidden and gripped, right pane is visible and not gripped
			//expand the right pane into the whole splitter window
			RclExpandSecondPane(rctClient, FALSE);
			break;
		}
	case 0x0E:	//01110
	case 0x0F:	//01111
		{//left pane is hidden and gripped, right pane is visible and gripped
			//expand the right pane into the whole splitter window
			RclExpandSecondPane(rctClient, FALSE);
			break;
		}
	case 0x10:	//10000
	case 0x11:	//10001
		{//left pane is visible and not gripped, right pane is hidden and not gripped
			//expand the left pane into the whole splitter window
			RclExpandFirstPane(rctClient, FALSE);
			break;
		}
	case 0x12:	//10010
	case 0x13:	//10011
		{//left pane is visible and not gripped, right pane is hidden and gripped
			//expand the left pane into the whole splitter window
			RclExpandFirstPane(rctClient, FALSE);
			break;
		}
	case 0x14:	//10100
		{//left pane is visible and not gripped, right pane is visible and not gripped
			//draw both panes and vertical splitter gap
			RclPanesV(rctClient);
			break;
		}
	case 0x15:	//10101
		{//left pane is visible and not gripped, right pane is visible and not gripped
			//draw both panes and horizontal splitter
			RclPanesH(rctClient);
			break;
		}
	case 0x16:	//10110
		{//left pane is visible and not gripped, right pane is visible and gripped
			//draw expanded left pane and vertical splitter gap docked to right window border
			rctClient.right -= m_rctGap.Width();
			m_rctGap.MoveGapToX(rctClient.right);
			RclExpandFirstPane(rctClient, TRUE);
			break;
		}
	case 0x17:	//10111
		{//left pane is visible and not gripped, right pane is visible and gripped
			//draw expanded left pane and horizontal splitter gap docked to bottom window border
			rctClient.bottom -= m_rctGap.Height();
			m_rctGap.MoveGapToY(rctClient.bottom);
			RclExpandFirstPane(rctClient, TRUE);
			break;
		}
	case 0x18:	//11000
	case 0x19:	//11001
		{//left pane is visible and gripped, right pane is hidden and not gripped
			//draw expanded left pane 
			RclExpandFirstPane(rctClient, FALSE);
			break;
		}
	case 0x1A:	//11010
	case 0x1B:	//11011
		{//left pane is visible and gripped, right pane is hidden and gripped
			//draw expanded left pane 
			RclExpandFirstPane(rctClient, FALSE);
			break;
		}
	case 0x1C:	//11100
		{//left pane is visible and gripped, right pane is visible and not gripped
			//draw expanded right pane and vertical splitter gap docked to left window border
			m_rctGap.MoveGapToX(0);
			rctClient.left += m_rctGap.Width();
			RclExpandSecondPane(rctClient, TRUE);
			break;
		}
	case 0x1D:	//11101
		{//left pane is visible and gripped, right pane is visible and not gripped
			//draw expanded right pane and horizontal splitter gap docked to top window border
			m_rctGap.MoveGapToY(0);
			rctClient.top += m_rctGap.Height();
			RclExpandSecondPane(rctClient, TRUE);
			break;
		}
	case 0x1E:	//11110
	case 0x1F:	//11111
		{//left pane is visible and gripped, right pane is visible and gripped
			//This is wrong situation - both pane can not be gripped simultaneously
			ASSERT(FALSE);
			break;
		}
	}

	// ����ͼ����Ҫ���·����С
	CWnd *pWnd1 = GetPaneView(0);
	CWnd *pWnd2 = GetPaneView(1);
	if ( NULL != pWnd1 )
	{
		CBiSplitterWnd *pSplit = DYNAMIC_DOWNCAST(CBiSplitterWnd, pWnd1);
		if ( NULL != pSplit )
		{
			pSplit->RecalcLayout();
		}
	}
	if ( NULL != pWnd2 )
	{
		CBiSplitterWnd *pSplit = DYNAMIC_DOWNCAST(CBiSplitterWnd, pWnd2);
		if ( NULL != pSplit )
		{
			pSplit->RecalcLayout();
		}
	}

	// UpdateWindow();
}

void CBiSplitterWnd::GripPane(UINT nPane)
{
    ASSERT(nPane <=1);
	if ( !(nPane < sizeof(m_panes)/sizeof(m_panes[0]))) 
	{
		return ;
	}
	if(m_panes[nPane].m_isGripped)	// expand pane
	{
		m_panes[nPane].ExpandPane();
		int nOtherPane = nPane ^ 1;  // 0 if nPane=1 and 1 if nPane=0
        if(m_panes[nOtherPane].IsGripped())
		{
			m_panes[nOtherPane].ExpandPane();
		}
		ASSERT(m_bswStyles & SPLITTER_STYLES);

		if(nPane == 0 && m_bswStyles & VSPLITTER)
		{
			SetSplitterPos(m_panes[0].m_rctPaneBox.right);
		}
		else if(nPane == 1 && m_bswStyles & VSPLITTER)
		{
			SetSplitterPos(m_panes[1].m_rctPaneBox.left - m_nGap);
		}
		else if(nPane == 0 && m_bswStyles & HSPLITTER)
		{
			SetSplitterPos(m_panes[0].m_rctPaneBox.bottom);
		}
		else if(nPane == 1 && m_bswStyles & HSPLITTER)
		{
			SetSplitterPos(m_panes[1].m_rctPaneBox.top - m_nGap);
		}
	}
	else	//grip pane
	{
		m_panes[nPane].GripPane();
		int nOtherPane = nPane ^ 1;  // 0 if nPane=1 and 1 if nPane=0
        if(m_panes[nOtherPane].IsGripped())
		{
			m_panes[nOtherPane].ExpandPane();
		}
		RecalcLayout();
	}
}

void CBiSplitterWnd::HidePane(CWnd *pView)
{
	for(int i = 0; i < 2; i++)
	{
		if(m_panes[i].m_pView && m_panes[i].m_pView == pView)
			HidePane(i);
	}
}

void CBiSplitterWnd::HidePane(UINT nPane)
{
	ASSERT(nPane <= 1);
	if ( !(nPane < sizeof(m_panes)/sizeof(m_panes[0]))) 
	{
		return ;
	}
	int nOtherPane = OtherPane(nPane);

	if(m_panes[nOtherPane].IsVisible())//hide only this pane
	{
		if(m_panes[nPane].m_pView)
			m_panes[nPane].m_pView->ShowWindow(SW_HIDE);
		m_panes[nPane].m_isVisible = FALSE;
		m_rctGap.HideGap();
		RecalcLayout();
	}
	else 
	{
		CWnd *pParent = GetParent();
		ASSERT_VALID(pParent);
        if (pParent->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)))
		{//hide parent window pane
			((CBiSplitterWnd*)pParent)->HidePane(this);	
		}
		else
		{//hide splitter window
			//TODO: hide only panes and redraw empty splitter window
			CRect rct;
			GetWindowRect(&rct);
			if(m_panes[nPane].m_pView)
				m_panes[nPane].m_pView->ShowWindow(SW_HIDE);
			m_panes[nPane].m_isVisible = FALSE;
			ShowWindow(SW_HIDE);
			pParent->ScreenToClient(&rct);
			pParent->InvalidateRect(&rct, TRUE);
		}
	}
}

void CBiSplitterWnd::ShowPane(CWnd *pView)
{
	for(int i = 0; i < 2; i++)
	{
		if(m_panes[i].m_pView && m_panes[i].m_pView == pView)
			ShowPane(i);
	}
}


void CBiSplitterWnd::ShowPane(UINT nPane)
{
	ASSERT(nPane <=1);
	if ( !(nPane < sizeof(m_panes)/sizeof(m_panes[0]))) 
	{
		return ;
	}
	int nOtherPane = OtherPane(nPane);

	if(!m_panes[nPane].m_isVisible && m_panes[nOtherPane].m_isVisible && (GetStyle() & WS_VISIBLE))
	{
        if(m_panes[nPane].m_pView != NULL)
        {
            m_panes[nPane].m_pView->ShowWindow(SW_SHOW);
		}
        m_panes[nPane].m_isVisible = TRUE;
		m_rctGap.ShowGap();
		RecalcLayout();
	}
	else if(!m_panes[nPane].m_isVisible && m_panes[nOtherPane].m_isVisible && !(GetStyle() & WS_VISIBLE))
	{
		if(m_panes[nPane].m_pView != NULL)
        {
            m_panes[nPane].m_pView->ShowWindow(SW_SHOW);
		}
        m_panes[nPane].m_isVisible = TRUE;
		/*
			This case is posibble only if splitter wnd was hiden for two stages.
			In a first stage nPane view was hiden and in a second stage all splitter wnd was hidden.
			We should show only one pane and hide other pane.
		*/
		if(m_panes[nOtherPane].m_pView != NULL)
        {
            m_panes[nOtherPane].m_pView->ShowWindow(SW_HIDE);
		}
        m_panes[nOtherPane].m_isVisible = FALSE;
		CWnd *pParent = GetParent();
		ASSERT_VALID(pParent);
        if (pParent->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)))
		{//show parent window pane
			((CBiSplitterWnd*)pParent)->ShowPane(this);	
		}
		else
            ShowWindow(SW_SHOW);
		RecalcLayout();
	}
	else if(m_panes[nPane].m_isVisible && !m_panes[nOtherPane].m_isVisible && (GetStyle() & WS_VISIBLE))
	{
		return;
	}
	else if(m_panes[nPane].m_isVisible && !m_panes[nOtherPane].m_isVisible && !(GetStyle() & WS_VISIBLE))
	{
		CWnd *pParent = GetParent();
		ASSERT_VALID(pParent);
        if (pParent->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)))
		{//show parent window pane
			((CBiSplitterWnd*)pParent)->ShowPane(this);	
		}
		else
            ShowWindow(SW_SHOW);
	}
	else if(m_panes[nPane].m_isVisible && m_panes[nOtherPane].m_isVisible && (GetStyle() & WS_VISIBLE))
	{
		return;
	}
	else if(m_panes[nPane].m_isVisible && m_panes[nOtherPane].m_isVisible && !(GetStyle() & WS_VISIBLE))
	{
		CWnd *pParent = GetParent();
		ASSERT_VALID(pParent);
        if (pParent->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)))
		{//show parent window pane
			((CBiSplitterWnd*)pParent)->ShowPane(this);	
		}
		else
            ShowWindow(SW_SHOW);
	}
	else if(!m_panes[nPane].m_isVisible && !m_panes[nOtherPane].m_isVisible && !(GetStyle() & WS_VISIBLE))
	{
		if(m_panes[nPane].m_pView != NULL)
        {
            m_panes[nPane].m_pView->ShowWindow(SW_SHOW);
		}
        m_panes[nPane].m_isVisible = TRUE;

		CWnd *pParent = GetParent();
		ASSERT_VALID(pParent);
        if (pParent->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)))
		{//show parent window pane
			((CBiSplitterWnd*)pParent)->ShowPane(this);	
		}
		else
            ShowWindow(SW_SHOW);
		RecalcLayout();
		
	}
	else if(!m_panes[nPane].m_isVisible && !m_panes[nOtherPane].m_isVisible && (GetStyle() & WS_VISIBLE))
	{
        if(m_panes[nPane].m_pView != NULL)
        {
            m_panes[nPane].m_pView->ShowWindow(SW_SHOW);
		}
        m_panes[nPane].m_isVisible = TRUE;
		RecalcLayout();
	}
}

void CBiSplitterWnd::SetActivePane(UINT nPane)
{
	ASSERT( nPane <=1);
	if ( !(nPane < sizeof(m_panes)/sizeof(m_panes[0]))) 
	{
		return ;
	}
	if(m_panes[nPane].m_pView != NULL)
        m_panes[nPane].m_pView->SetActiveWindow();
	m_lastActivePane = nPane;
}

int CBiSplitterWnd::GetActivePane()
{
	return m_lastActivePane;
}

BOOL CBiSplitterWnd::IsPaneVisible(UINT nPane)
{
	ASSERT(nPane <=1);
	if ( !(nPane < sizeof(m_panes)/sizeof(m_panes[0]))) 
	{
		return FALSE;
	}
	return m_panes[nPane].IsVisible() && (GetStyle() & WS_VISIBLE);
}

CWnd *CBiSplitterWnd::GetView(UINT viewID)
{
	ASSERT(GetDlgItem(viewID));
	return GetDlgItem(viewID);
}

//*** Implementation ***

CBiSplitterWnd::~CBiSplitterWnd()
{
	//if(m_pBiSplitterWnd)
	//	delete m_pBiSplitterWnd;
	
	DEL(m_pImgZoomTop);
	DEL(m_pImgZoomBottom);
	DEL(m_pImgZoomLeft);
	DEL(m_pImgZoomRight);

	if(m_pGlyphs)
	{
		for(int i = 0; i < m_glyphsCount; i++)
		{
			if(m_pGlyphs[i].pBits)
				delete [](m_pGlyphs[i].pBits);

			if(m_pGlyphs[i].pBitmapInfo)
				delete []((BYTE*)(m_pGlyphs[i].pBitmapInfo));
		}
		delete[] m_pGlyphs;
	}

	if(m_pSmGlyphs)
	{
		for(int i = 0; i < m_glyphsCount; i++)
		{
			if(m_pSmGlyphs[i].pBits)
				delete [](m_pSmGlyphs[i].pBits);

			if(m_pSmGlyphs[i].pBitmapInfo)
				delete []((BYTE*)(m_pSmGlyphs[i].pBitmapInfo));
		}
		delete[] m_pSmGlyphs;
	}

	if(m_images)
		delete []m_images;


}

void CBiSplitterWnd::SetPaneEmpty(CPane *pPane)
{
	pPane->m_rctCaption.SetRectEmpty();
	pPane->m_rctPaneBox.SetRectEmpty();
	pPane->m_closeBtn.SetRectEmpty();
	pPane->m_rctViewBox.SetRectEmpty();
	pPane->m_rctRightRect.SetRectEmpty();
	pPane->m_rctTopRect.SetRectEmpty();
}

void CBiSplitterWnd::RecalcGap(int left, int top, int right, int bottom)
{
	m_rctGap.left = left;
	m_rctGap.right = right;
	m_rctGap.top = top;
	m_rctGap.bottom = bottom;

	CBtn *pGripBtn = NULL;
	for( int i = 0; i < 2; i++)
	{
		pGripBtn = m_rctGap.m_gripBtn + i;
        if(m_panes[i].m_bspStyles & (GRIPBTN | SMGRIPBTN))
		{
			if(m_bswStyles & VSPLITTER)
			{
				pGripBtn->right = m_rctGap.right;
				pGripBtn->left = m_rctGap.left;
				pGripBtn->top = m_rctGap.Height() / 2 - 2 * m_nBtnCX + 3 * i * m_nBtnCY;
				pGripBtn->bottom = pGripBtn->top + m_nBtnCY;
			}
			else if(m_bswStyles & HSPLITTER)
			{
				pGripBtn->left = m_rctGap.Width() / 2 - 2 * m_nBtnCX + 3 * i * m_nBtnCX;
				pGripBtn->right = pGripBtn->left + m_nBtnCX;
				pGripBtn->top = m_rctGap.top;
				pGripBtn->bottom = m_rctGap.bottom;
			}
#ifdef _DEBUG
			else
				ASSERT(FALSE);
#endif
		}
		else
			pGripBtn->SetRectEmpty();
	}
}

void CBiSplitterWnd::RecalcPane(UINT nPane, int left, int top, int right, int bottom)
{
	ASSERT( nPane <=1);

	CPane *pPane = &(m_panes[nPane]);
	pPane->m_rctPaneBox.SetRect(left, top, right, bottom);
	switch(pPane->m_bspStyles & CAPTION_STYLES)
	{
	case NOTHING:
        pPane->m_rctCaption.SetRectEmpty();
		pPane->m_closeBtn.SetRectEmpty();
		pPane->m_rctRightRect.SetRectEmpty();
		pPane->m_rctTopRect.SetRectEmpty();
		break;
	case SMCAPTION:
		{
			if(m_bswStyles & VIEW3D)
			{
                pPane->m_rctCaption.top = pPane->m_rctPaneBox.top + BorderWidth(pPane->m_bspStyles);
				pPane->m_rctCaption.bottom = pPane->m_rctCaption.top + m_nSmCaptionHeight + 2 * CaptionBorderWidth(pPane->m_bspStyles);
				pPane->m_rctCaption.left = pPane->m_rctPaneBox.left + BorderWidth(pPane->m_bspStyles);
				pPane->m_rctCaption.right = pPane->m_rctPaneBox.right - BorderWidth(pPane->m_bspStyles);
			}
			else//flat view
			{
                pPane->m_rctCaption.top = pPane->m_rctPaneBox.top + BorderWidth(pPane->m_bspStyles);;
				pPane->m_rctCaption.bottom = pPane->m_rctCaption.top + m_nSmCaptionHeight + CaptionBorderWidth(pPane->m_bspStyles);
				pPane->m_rctCaption.left = pPane->m_rctPaneBox.left + BorderWidth(pPane->m_bspStyles);
				pPane->m_rctCaption.right = pPane->m_rctPaneBox.right - BorderWidth(pPane->m_bspStyles);
			}
		
			if(pPane->m_bspStyles & CLOSEBTN)
			{
				int btnHeight = m_nSmCaptionHeight;
				int btnWidth = btnHeight;
				if(pPane->m_rctCaption.Width() - 2 * CaptionBorderWidth(pPane->m_bspStyles) >= btnWidth )
				{
					pPane->m_closeBtn.right = pPane->m_rctCaption.right - CaptionBorderWidth(pPane->m_bspStyles);
					pPane->m_closeBtn.left = pPane->m_closeBtn.right - btnWidth;
					pPane->m_closeBtn.top = pPane->m_rctCaption.top + ((m_bswStyles & VIEW3D) ? CaptionBorderWidth(pPane->m_bspStyles) : 0);
					pPane->m_closeBtn.bottom = pPane->m_closeBtn.top + btnHeight;
				}
				else
					pPane->m_closeBtn.SetRectEmpty();
			}
		}
		break;
	case CAPTION:
		{
			if(m_bswStyles & VIEW3D)
			{

				pPane->m_rctCaption.top = pPane->m_rctPaneBox.top + BorderWidth(pPane->m_bspStyles);
				pPane->m_rctCaption.bottom = pPane->m_rctCaption.top + m_nCaptionHeight + 2 * CaptionBorderWidth(pPane->m_bspStyles);
				pPane->m_rctCaption.left = pPane->m_rctPaneBox.left + BorderWidth(pPane->m_bspStyles);
				pPane->m_rctCaption.right = pPane->m_rctPaneBox.right - BorderWidth(pPane->m_bspStyles);
			}
			else
			{
                pPane->m_rctCaption.top = pPane->m_rctPaneBox.top + BorderWidth(pPane->m_bspStyles);
				pPane->m_rctCaption.bottom = pPane->m_rctCaption.top + m_nCaptionHeight + CaptionBorderWidth(pPane->m_bspStyles);
				pPane->m_rctCaption.left = pPane->m_rctPaneBox.left + BorderWidth(pPane->m_bspStyles);
				pPane->m_rctCaption.right = pPane->m_rctPaneBox.right - BorderWidth(pPane->m_bspStyles);
			}

			if(pPane->m_bspStyles & CLOSEBTN)
			{
				int btnHeight = m_nCaptionHeight;
				int btnWidth = btnHeight;
				if(pPane->m_rctCaption.Width() - 2 * CaptionBorderWidth(pPane->m_bspStyles) >= btnWidth)
				{
					pPane->m_closeBtn.right = pPane->m_rctCaption.right - CaptionBorderWidth(pPane->m_bspStyles);
					pPane->m_closeBtn.left = pPane->m_closeBtn.right - btnWidth;
					pPane->m_closeBtn.top = pPane->m_rctCaption.top + ((m_bswStyles & VIEW3D) ? CaptionBorderWidth(pPane->m_bspStyles) : 0);//CaptionBorderWidth(pPane->m_bspStyles);
					pPane->m_closeBtn.bottom = pPane->m_closeBtn.top + btnHeight;
				}
				else
					pPane->m_closeBtn.SetRectEmpty();
			}
		}
		break;
	default:
		ASSERT(FALSE);
	}

	if ((RIGHT_BTN==pPane->m_bspStyles) && m_bShowRightBk)
	{
		pPane->m_rctRightRect.top = pPane->m_rctPaneBox.top;
		pPane->m_rctRightRect.bottom = pPane->m_rctPaneBox.bottom;
		pPane->m_rctRightRect.left = pPane->m_rctPaneBox.right - m_iRightBtnWidth;
		pPane->m_rctRightRect.right = pPane->m_rctPaneBox.right - BorderWidth(pPane->m_bspStyles);
	}

	if ((TOP_BTN==pPane->m_bspStyles) && m_bShowTopBk)
	{
		pPane->m_rctTopRect.top = pPane->m_rctPaneBox.top + BorderWidth(pPane->m_bspStyles);
		pPane->m_rctTopRect.bottom = pPane->m_rctPaneBox.top + m_iTopBtnHeight;
		pPane->m_rctTopRect.left = pPane->m_rctPaneBox.left;
		pPane->m_rctTopRect.right = pPane->m_rctPaneBox.right;
	}

	pPane->m_rctViewBox.left = pPane->m_rctPaneBox.left + BorderWidth(pPane->m_bspStyles);
	pPane->m_rctViewBox.right	= pPane->m_rctPaneBox.right - BorderWidth(pPane->m_bspStyles)-pPane->m_rctRightRect.Width();
	pPane->m_rctViewBox.top = pPane->m_rctPaneBox.top + BorderWidth(pPane->m_bspStyles) + pPane->m_rctCaption.Height() + pPane->m_rctTopRect.Height();
	pPane->m_rctViewBox.bottom = pPane->m_rctPaneBox.bottom - BorderWidth(pPane->m_bspStyles);
}

void CBiSplitterWnd::DrawPane(CDC *pDC, CPane *pPane)
{
	ASSERT_VALID(pDC);
	if(!pPane->IsVisible() || pPane->m_isGripped) 
	{
		if(pPane->m_pView != NULL)
			pPane->m_pView->ShowWindow(SW_HIDE);
		return;
	}

	if( pPane->m_pView != NULL)
	{
		// TRACE(L" ���� \n");

		if ( pPane->m_bPrepareToDel )
		{
			pPane->m_pView->SendMessage(WM_SETREDRAW, (WPARAM)(FALSE), 0L);
		}
		else
		{
			pPane->m_pView->SendMessage(WM_SETREDRAW, (WPARAM)(TRUE), 0L);

			pPane->m_pView->SetWindowPos(NULL, pPane->m_rctViewBox.left, pPane->m_rctViewBox.top,
				pPane->m_rctViewBox.Width(), pPane->m_rctViewBox.Height(), SWP_SHOWWINDOW);
			
			pPane->m_pView->UpdateWindow();
		}
	}		

	switch(pPane->m_bspStyles & BORDER_STYLES)
	{
	case THIN_BORDER: //one pixel width pane border
        if(m_bswStyles & VIEW3D)
			pDC->Draw3dRect(pPane->m_rctPaneBox, m_clrBtnShadow, m_clrBtnHilite);
		else
		{
			CBrush brush(m_clrFlatBorder);
			pDC->FrameRect(pPane->m_rctPaneBox, &brush);							
		}
		break;
	case THICK_BORDER: // two pixel width border
	{
		CRect box = pPane->m_rctPaneBox;
        if(m_bswStyles & VIEW3D)
		{
            pDC->Draw3dRect(box,m_clrBtnShadow, m_clrBtnHilite);
            box.DeflateRect(1, 1);
            pDC->Draw3dRect(box, m_clrWindowFrame, m_clrBtnFace);
		}
		else
		{
			CBrush brush(m_clrFlatBorder);
            pDC->FrameRect(box, &brush);
            box.DeflateRect(1, 1);
            pDC->FrameRect(box, &brush);
		}
	}
		break;
#ifdef _DEBUG
	case NOTHING:
		break;
	default:
		ASSERT(FALSE);
#endif
	}
	
	if(((pPane->m_bspStyles & CAPTION) || (pPane->m_bspStyles & SMCAPTION))	)
	{
        DrawCaption(pDC, pPane);
        if(pPane->m_bspStyles & CLOSEBTN)
            DrawCloseBtn(pDC, pPane);
	}

	if(pPane->m_bspStyles & RIGHT_BTN)
	{
		DrawRightRect(pDC, pPane);
	}

	if(pPane->m_bspStyles & TOP_BTN)
	{
		DrawTopRect(pDC, pPane);
	}
}

void CBiSplitterWnd::DrawRightRect(CDC *pDC, CPane *pPane)
{
	CRect rect;
	CRect rect1 = pPane->m_rctPaneBox;
	rect.IntersectRect(pPane->m_rctRightRect, rect1);

	COLORREF ClrGap = COLORREF(CTabSplitWnd::m_pMainFram->SendMessage(UM_GetNowUseColor,ESCSpliter));
	if ( 0 == ClrGap )
	{
		ClrGap = COLORREF(CTabSplitWnd::m_pMainFram->SendMessage(UM_GetSysColor,ESCSpliter));
	}

	COLORREF clrMid(RGB(26,25,30));
	pDC->FillSolidRect(rect, clrMid);

	CRect rtBtn = rect;
	int32 iBtnHeight = 0;
	if (NULL != m_pImgZoomLeft)
	{
		iBtnHeight = m_pImgZoomLeft->GetHeight()/3;
	}
	rtBtn.top = (rect.Height()-iBtnHeight)/2;
	rtBtn.bottom = rtBtn.top + iBtnHeight;
	m_RightBtnZoom.SetRect(&rtBtn);

	if(!m_bRightZoom)
	{
		m_RightBtnZoom.SetImage(m_pImgZoomLeft);
	}
	else
	{
		m_RightBtnZoom.SetImage(m_pImgZoomRight);
	}

	Gdiplus::Graphics graphics(pDC->GetSafeHdc());
	m_RightBtnZoom.DrawButton(&graphics);
}

void CBiSplitterWnd::DrawTopRect(CDC *pDC, CPane *pPane)
{
	CRect rect;
	CRect rect1 = pPane->m_rctPaneBox;
	rect.IntersectRect(pPane->m_rctTopRect, rect1);

	COLORREF ClrGap = COLORREF(CTabSplitWnd::m_pMainFram->SendMessage(UM_GetNowUseColor,ESCSpliter));
	if ( 0 == ClrGap )
	{
		ClrGap = COLORREF(CTabSplitWnd::m_pMainFram->SendMessage(UM_GetSysColor,ESCSpliter));
	}
	pDC->FillSolidRect(rect, ClrGap);

	CRect rtBtn = rect;
	int32 iBtnWidth = 0;
	if (NULL != m_pImgZoomTop)
	{
		iBtnWidth = m_pImgZoomTop->GetWidth();
	}
	rtBtn.left = (rect.Width()-iBtnWidth)/2;
	rtBtn.right = rtBtn.left + iBtnWidth;
	m_TopBtnZoom.SetRect(&rtBtn);

	if(!m_bTopZoom)
	{
		m_TopBtnZoom.SetImage(m_pImgZoomTop);
	}
	else
	{
		m_TopBtnZoom.SetImage(m_pImgZoomBottom);
	}

	Gdiplus::Graphics graphics(pDC->GetSafeHdc());
	m_TopBtnZoom.DrawButton(&graphics);
}

void CBiSplitterWnd::DrawCaption(CDC *pDC, CPane *pPane)
{
	ASSERT(pPane->m_bspStyles & CAPTION_STYLES);

	CRect rect;
	CRect rect1 = pPane->m_rctPaneBox;
	rect1.DeflateRect(BorderWidth(pPane->m_bspStyles), BorderWidth(pPane->m_bspStyles));
	rect.IntersectRect(pPane->m_rctCaption, rect1);

	switch(pPane->m_bspStyles & CAPTIONBORDER_STYLES)
	{
	case NOTHING:
		{
            pDC->FillSolidRect(rect, m_clrBtnFace);
			rect.right -= pPane->m_closeBtn.Width();
			rect.left += BorderWidth(pPane->m_bspStyles);
            CFont *pOldFont	= pDC->SelectObject((pPane->m_bspStyles & CAPTION ? &m_fntCaption : &m_fntSmCaption));
            SetBkMode(pDC->m_hDC, TRANSPARENT);
            pDC->DrawText(pPane->m_sCaptionText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
            pDC->SelectObject(pOldFont);
		}
		break;
	case THIN_CAPTIONBORDER: //one pixel width pane border
		{
			if(pPane->m_rctCaption.Width() >= 2 * BorderWidth(pPane->m_bspStyles))
			{
		        if(m_bswStyles & VIEW3D)
				{
					pDC->Draw3dRect(rect, m_clrBtnHilite, m_clrBtnShadow);
					rect.DeflateRect(1, 1);
				}
				else
				{
					CPen pen(PS_SOLID, 0, m_clrFlatBorder);
					CPen *pOldPen = pDC->SelectObject(&pen);
					pDC->MoveTo(rect.left, rect.bottom - 1);
					pDC->LineTo(rect.right, rect.bottom - 1);
					pDC->SelectObject(pOldPen);
					rect.bottom -= 1;
				}
	            
			}
            pDC->FillSolidRect(rect, m_clrBtnFace);
			rect.right -= pPane->m_closeBtn.Width();
			rect.left += BorderWidth(pPane->m_bspStyles);
            CFont *pOldFont	= pDC->SelectObject((pPane->m_bspStyles & CAPTION ? &m_fntCaption : &m_fntSmCaption));
            SetBkMode(pDC->m_hDC, TRANSPARENT);
            pDC->DrawText(pPane->m_sCaptionText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
            pDC->SelectObject(pOldFont);
		}
		break;
	case THICK_CAPTIONBORDER: // two pixel width border
	{
        //draw thick border for caption
		if(((pPane->m_bspStyles & CAPTION) || (pPane->m_bspStyles & SMCAPTION)))
		{
			if(pPane->m_rctCaption.Width() >= 2 * BorderWidth(pPane->m_bspStyles))
			{
		        if(m_bswStyles & VIEW3D)
				{
					pDC->Draw3dRect(rect, m_clrBtnHilite, m_clrWindowFrame);
				    rect.DeflateRect(1, 1);
		            pDC->Draw3dRect(rect, m_clrBtnFace, m_clrBtnShadow);
					rect.DeflateRect(1, 1);
				}
				else
				{
					CPen pen(PS_SOLID, 0, m_clrFlatBorder);
					CPen *pOldPen = pDC->SelectObject(&pen);
					pDC->MoveTo(rect.left, rect.bottom - 1);
					pDC->LineTo(rect.right, rect.bottom - 1);
					pDC->MoveTo(rect.left, rect.bottom - 2);
					pDC->LineTo(rect.right, rect.bottom - 2);
					pDC->SelectObject(pOldPen);
					rect.bottom -= 2;
				}

			}
			//fill caption
			pDC->FillSolidRect(rect, m_clrBtnFace);
			rect.right -= pPane->m_closeBtn.Width();
			rect.left += BorderWidth(pPane->m_bspStyles);
            CFont *pOldFont	= pDC->SelectObject((pPane->m_bspStyles & CAPTION ? &m_fntCaption : &m_fntSmCaption));
            SetBkMode(pDC->m_hDC, TRANSPARENT);
            pDC->DrawText(pPane->m_sCaptionText, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
            pDC->SelectObject(pOldFont);
		}
	}
		break;
	default:
		ASSERT(FALSE);
	}
}

void CBiSplitterWnd::DrawSplitterGap(CDC *pDC, const CRect &rect)
{
	ASSERT_VALID(pDC);
	if(!m_rctGap.IsVisible()) 
		return;

// 	COLORREF Clrs[ESCCount] = {0};
// 	CFaceScheme::Instance()->GetNowUseColors(Clrs);
// 	COLORREF ClrGap = Clrs[ESCSpliter];

	COLORREF ClrGap = COLORREF(CTabSplitWnd::m_pMainFram->SendMessage(UM_GetNowUseColor,ESCSpliter));
	if ( 0 == ClrGap )
	{
		ClrGap = COLORREF(CTabSplitWnd::m_pMainFram->SendMessage(UM_GetSysColor,ESCSpliter));/*CFaceScheme::Instance()->GetSysColor(ESCSpliter);*/
	}
	
	COLORREF clrBk(RGB(30,30,30));
	COLORREF clrMid(RGB(26,25,30));

	pDC->FillSolidRect(rect, clrBk);
	CRect rct(rect.left +1 , rect.top + 1, rect.right -1, rect.bottom -1);
	pDC->FillSolidRect(rct, clrMid);

	if(m_panes[0].m_bspStyles & (GRIPBTN | SMGRIPBTN))
		DrawGripBtn(pDC, 0);
	if(m_panes[1].m_bspStyles & (GRIPBTN | SMGRIPBTN))
		DrawGripBtn(pDC, 1);	
}

void CBiSplitterWnd::DrawGripBtn(CDC *pDC, int nGrip)
{
	if (nGrip < 0 || !(nGrip < sizeof(m_panes)/sizeof(m_panes[0]))) 
	{
		return;
	}
	if ( (m_panes[nGrip].m_bspStyles & (GRIPBTN | SMGRIPBTN)) == 0)
		return;
	int index = IMAGE_GRIP_L;
	if(nGrip == 0 && (m_bswStyles & VSPLITTER) && !m_panes[nGrip].m_isGripped)
		index = IMAGE_GRIP_L;
	else if(nGrip == 0 && (m_bswStyles & VSPLITTER) && m_panes[nGrip].m_isGripped)
		index = IMAGE_GRIP_R;
	else if(nGrip == 1 && (m_bswStyles & VSPLITTER) && !m_panes[nGrip].m_isGripped)
		index = IMAGE_GRIP_R;
	else if(nGrip == 1 && (m_bswStyles & VSPLITTER) && m_panes[nGrip].m_isGripped)
		index = IMAGE_GRIP_L;
	else if(nGrip == 0 && (m_bswStyles & HSPLITTER) && !m_panes[nGrip].m_isGripped)
		index = IMAGE_GRIP_T;
	else if(nGrip == 0 && (m_bswStyles & HSPLITTER) && m_panes[nGrip].m_isGripped)
		index = IMAGE_GRIP_B;
	else if(nGrip == 1 && (m_bswStyles & HSPLITTER)&& !m_panes[nGrip].m_isGripped)
		index = IMAGE_GRIP_B;
	else if(nGrip == 1 && (m_bswStyles & HSPLITTER)&& m_panes[nGrip].m_isGripped)
		index = IMAGE_GRIP_T;
#ifdef _DEBUG
	else
		ASSERT(FALSE);
#endif

	switch(m_panes[nGrip].m_bspStyles & GRIP_STYLES)
	{
	case GRIPBTN:
        DrawBtn(pDC, m_rctGap.m_gripBtn + nGrip, index, m_panes[nGrip].m_bspStyles, FALSE);
		break;
	case SMGRIPBTN:
        DrawBtn(pDC, m_rctGap.m_gripBtn + nGrip, index, m_panes[nGrip].m_bspStyles, TRUE);
		break;
#ifdef _DEBUG
	default:
		ASSERT(FALSE);
#endif
	}
}

void CBiSplitterWnd::DrawCloseBtn(CDC *pDC, CPane *pPane)
{
	if(pPane->m_closeBtn.IsRectEmpty()
			|| (pPane->m_bspStyles & CAPTION_STYLES) == NOTHING
			|| (pPane->m_bspStyles & CLOSEBTN) == NOTHING)
		return;
	
	switch(pPane->m_bspStyles & CAPTION_STYLES)
	{
	case CAPTION:
        DrawBtn(pDC, &(pPane->m_closeBtn), IMAGE_CLOSE, pPane->m_bspStyles, FALSE);
		break;
	case SMCAPTION:
        DrawBtn(pDC, &(pPane->m_closeBtn), IMAGE_CLOSE, pPane->m_bspStyles, TRUE);
		break;
#ifdef _DEBUG
	default:
		ASSERT(FALSE);
#endif
	}
}

void CBiSplitterWnd::DrawBtn(CDC *pDC, CBtn *pBtn, int nImageIndex, UINT styles, BOOLEAN smImageSize)
{
	if(pBtn->IsRectEmpty())
		return;

	CRect rctBtn = *pBtn;
	COLORREF bkColor;
	
	switch(pBtn->m_curState)
	{
	case CBtn::DOWN: // down
		switch(styles & BUTTON_STYLES)
		{
		case THICK_BUTTON:
            pDC->Draw3dRect(rctBtn, m_clrWindowFrame, m_clrBtnHilite);
	        rctBtn.DeflateRect(1,1);
		    pDC->Draw3dRect(rctBtn, m_clrBtnShadow, m_clrBtnFace);
			rctBtn.DeflateRect(1,1);
			pDC->FillSolidRect(rctBtn, m_clrBtnFace);
			bkColor = m_clrBtnFace;
			break;
		case THIN_BUTTON:
			pDC->Draw3dRect(rctBtn, m_clrBtnShadow, m_clrBtnHilite);
			rctBtn.DeflateRect(1,1);
			pDC->FillSolidRect(rctBtn, m_clrBtnFace);
			bkColor = m_clrBtnFace;
			break;
		case FLAT_BUTTON:
            pDC->Draw3dRect(rctBtn, m_clrFlatBorder, m_clrFlatBorder);
	        rctBtn.DeflateRect(1,1);
			pDC->FillSolidRect(rctBtn, m_clrBtnFace);
			bkColor = m_clrBtnFace;
			break;
		default:
			ASSERT(FALSE);
		}
		break;
	case CBtn::NEUTRAL:
			pDC->FillSolidRect(rctBtn, m_clrBtnFace);
			bkColor = m_clrBtnFace;
			break;
	case CBtn::UP:
		switch(styles & BUTTON_STYLES)
		{
		case THICK_BUTTON:
            pDC->Draw3dRect(rctBtn, m_clrBtnHilite, m_clrWindowFrame);
            rctBtn.DeflateRect(1,1);
            pDC->Draw3dRect(rctBtn, m_clrBtnFace, m_clrBtnShadow);
            rctBtn.DeflateRect(1,1);
			pDC->FillSolidRect(rctBtn, m_clrBtnFace);
			bkColor = m_clrBtnFace;
			break;
		case THIN_BUTTON:
            pDC->Draw3dRect(rctBtn, m_clrBtnHilite, m_clrBtnShadow);
            rctBtn.DeflateRect(1,1);
			pDC->FillSolidRect(rctBtn, m_clrBtnFace);
			bkColor = m_clrBtnFace;
			break;
		case FLAT_BUTTON:
            pDC->Draw3dRect(rctBtn, m_clrFlatBtnActiveBorder, m_clrFlatBtnActiveBorder);
	        rctBtn.DeflateRect(1,1);
			pDC->FillSolidRect(rctBtn, m_clrFlatBtnActiveFace);
			bkColor = m_clrFlatBtnActiveFace;
			break;
		default:
			ASSERT(FALSE);
		}
		break;
	default:
		ASSERT(FALSE);
	}
	
	GLYPHINFO *pGlyph = NULL;
	if(smImageSize)
	{
		pGlyph = m_pSmGlyphs + nImageIndex;
	}
	else 
	{
		pGlyph = m_pGlyphs + nImageIndex;
	}
	//lint --e{415} suppress "across the array bound"
	pGlyph->pBitmapInfo->bmiColors[0].rgbRed = GetRValue(m_clrFlatBtnActiveFace);	
	pGlyph->pBitmapInfo->bmiColors[0].rgbGreen = GetGValue(m_clrFlatBtnActiveFace);
	pGlyph->pBitmapInfo->bmiColors[0].rgbBlue = GetBValue(m_clrFlatBtnActiveFace);
	pGlyph->pBitmapInfo->bmiColors[0].rgbReserved = 0;


	pGlyph->pBitmapInfo->bmiColors[1].rgbRed = 0;	
	pGlyph->pBitmapInfo->bmiColors[1].rgbGreen = 0;
	pGlyph->pBitmapInfo->bmiColors[1].rgbBlue = 0;
	pGlyph->pBitmapInfo->bmiColors[1].rgbReserved = 0;

	int cx = pGlyph->pBitmapInfo->bmiHeader.biWidth;
	int cy = pGlyph->pBitmapInfo->bmiHeader.biHeight;

	StretchDIBits(pDC->GetSafeHdc(), pBtn->left + (pBtn->Width() - cx) / 2,
		pBtn->top + (pBtn->Height() - cy) / 2,
		cx,
		cy,
		0,
		0,
		cx,
		cy,
		pGlyph->pBits,
		pGlyph->pBitmapInfo,
		DIB_RGB_COLORS,
		SRCCOPY);

}

void CBiSplitterWnd::UpdateSysColors()
{
	m_clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
	m_clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);
	m_clrBtnHilite = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	m_clrBtnText = ::GetSysColor(COLOR_BTNTEXT);
	m_clrWindowFrame = ::GetSysColor(COLOR_WINDOWFRAME);
}

void CBiSplitterWnd::UpdateSysMetrics()
{
    LOGFONT lf;
	// System metrics
	NONCLIENTMETRICS metrics = {0};

	OSVERSIONINFO osvi = {0};  
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);  
    GetVersionEx(&osvi);  
  
    if(osvi.dwMajorVersion <6 )  
        metrics.cbSize = sizeof(NONCLIENTMETRICS) - sizeof(metrics.iPaddedBorderWidth);  
    else  
        metrics.cbSize = sizeof(NONCLIENTMETRICS);  

	//metrics.cbSize=sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &metrics, 0);
	
	m_nSmCaptionHeight=metrics.iSmCaptionHeight;
	if(m_fntSmCaption.m_hObject != NULL)
		m_fntSmCaption.DeleteObject();
    memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
    lf.lfHeight = m_nSmCaptionHeight - 2; 
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfWeight = FW_NORMAL;
	lf.lfQuality = ANTIALIASED_QUALITY;
	lstrcpy(lf.lfFaceName,_T("Tahoma"));        // request a face name "Marlett"

	VERIFY(m_fntSmCaption.CreateFontIndirect(&lf));  // create the font

	m_nCaptionHeight=metrics.iCaptionHeight;
	if(m_fntCaption.m_hObject != NULL)
		m_fntCaption.DeleteObject();
    lf.lfHeight = m_nSmCaptionHeight - 2; 
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfWeight = FW_NORMAL;
	lf.lfQuality = ANTIALIASED_QUALITY;
	lstrcpy(lf.lfFaceName,_T("Tahoma"));        // request a face name "Marlett"

	VERIFY(m_fntCaption.CreateFontIndirect(&lf));  // create the font	
	m_hSplitterCursorV = ::LoadCursor(NULL, IDC_SIZEWE);
	m_hSplitterCursorH = ::LoadCursor(NULL, IDC_SIZENS);
	m_hArrowCursor = ::LoadCursor(NULL, IDC_ARROW);

	// Dimensions, in pixels, of small caption buttons
	m_nBtnSmCX = GetSystemMetrics(SM_CXSMSIZE); 
	m_nBtnSmCY = GetSystemMetrics(SM_CYSMSIZE);
	m_nBtnCX = GetSystemMetrics(SM_CXSIZE); 
	m_nBtnCY = GetSystemMetrics(SM_CYSIZE);

	//font for close button image drawing
	if(m_fntSymbol.m_hObject != NULL)
		m_fntSymbol.DeleteObject();
		
    memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
    lf.lfHeight = m_nCaptionHeight - 4; 
	lf.lfCharSet = SYMBOL_CHARSET;
	lf.lfWeight = FW_THIN;
	lf.lfQuality = DEFAULT_QUALITY;//ANTIALIASED_QUALITY;
	lstrcpy(lf.lfFaceName,_T("Marlett"));        // request a face name "Marlett"
	VERIFY(m_fntSymbol.CreateFontIndirect(&lf));  // create the font

	//font for small close button image drawing
	if(m_fntSmSymbol.m_hObject != NULL)
		m_fntSmSymbol.DeleteObject();

    memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
    lf.lfHeight = m_nSmCaptionHeight - 4; 
	lf.lfCharSet = SYMBOL_CHARSET;
	lf.lfWeight = FW_THIN;
	lf.lfQuality = DEFAULT_QUALITY; //ANTIALIASED_QUALITY;
	lstrcpy(lf.lfFaceName, _T("Marlett"));        // request a face name "Marlett"
	VERIFY(m_fntSmSymbol.CreateFontIndirect(&lf));  // create the font
}

void CBiSplitterWnd::UpdateSysImages()
{
	ASSERT(m_fntSmSymbol.m_hObject);
	ASSERT(m_fntSymbol.m_hObject);
	
	CDC *pDC = GetDC();
	ASSERT(pDC);
	if (NULL == pDC)
	{
		return;
	}
	GLYPHMETRICS gm;
	MAT2 mt = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};//identity matrix
	int sizeImage;

	for(int i = 0; i < m_glyphsCount; i++)
	{
		pDC->SelectObject(&m_fntSymbol);
		sizeImage = pDC->GetGlyphOutline(m_images[i], GGO_BITMAP, &gm, 0, NULL, &mt);
		ASSERT (sizeImage != -1);
		if(m_pGlyphs[i].pBits)
			delete []m_pGlyphs[i].pBits;
		m_pGlyphs[i].pBits = new BYTE[sizeImage];
		pDC->GetGlyphOutline(m_images[i], GGO_BITMAP, &gm, sizeImage, m_pGlyphs[i].pBits, &mt);
		m_pGlyphs[i].pBitmapInfo->bmiHeader.biWidth = gm.gmBlackBoxX;
		m_pGlyphs[i].pBitmapInfo->bmiHeader.biHeight = gm.gmBlackBoxY;

		pDC->SelectObject(&m_fntSmSymbol);
		sizeImage = pDC->GetGlyphOutline(m_images[i], GGO_BITMAP, &gm, 0, NULL, &mt);
		ASSERT (sizeImage != -1);
		if(m_pSmGlyphs[i].pBits)
			delete []m_pSmGlyphs[i].pBits;
		m_pSmGlyphs[i].pBits = new BYTE[sizeImage];
		pDC->GetGlyphOutline(m_images[i], GGO_BITMAP, &gm, sizeImage, m_pSmGlyphs[i].pBits, &mt);
		m_pSmGlyphs[i].pBitmapInfo->bmiHeader.biWidth = gm.gmBlackBoxX;
		m_pSmGlyphs[i].pBitmapInfo->bmiHeader.biHeight = gm.gmBlackBoxY;
	}

	ReleaseDC(pDC);
}

void CBiSplitterWnd::DrawTracker(const CRect& rect)
{	
	ASSERT_VALID(this);
	ASSERT(!rect.IsRectEmpty());
	
	// pat-blt without clip children on
	CDC* pDC = GetDC();
	// invert the brush pattern (looks just like frame window sizing)
	CBrush* pBrush = CDC::GetHalftoneBrush();
	HBRUSH hOldBrush = NULL;
	if (pBrush != NULL)
		hOldBrush = (HBRUSH)SelectObject(pDC->m_hDC, pBrush->m_hObject);
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT);
	if (hOldBrush != NULL)
		SelectObject(pDC->m_hDC, hOldBrush);
	ReleaseDC(pDC);
}

void CBiSplitterWnd::DrawDelPane(CDC* pDC, const CRect& rect)
{
	if ( NULL == pDC )
	{
		return;
	}

	int iOldMode = pDC->SetROP2(R2_XORPEN);

	CPen pen(PS_SOLID, 1, RGB(128,128,128));
	CPen* pPenOld = pDC->SelectObject(&pen);
	
	//
	for ( int32 i = rect.top + 1; i <= rect.bottom - 1; i += 2 )
	{
		pDC->MoveTo(rect.left + 1, i);
		pDC->LineTo(rect.right- 1, i);
	}
		
	pDC->SetROP2(iOldMode);
	pDC->SelectObject(pPenOld);
	pen.DeleteObject();
}

void CBiSplitterWnd::DrawTrackerDelPane(const CRect& rectTrack)
{
	ASSERT_VALID(this);
	ASSERT(!rectTrack.IsRectEmpty());

	CDC* pDC = GetDC();

	if ( BeVSplit() )
	{
		int32 iLeftPos  = m_panes[0].m_rctViewBox.left  + VALIDAREASIZE;
		int32 iRightPos = m_panes[1].m_rctViewBox.right - rectTrack.Width() - VALIDAREASIZE;
		
		if ( rectTrack.left <= iLeftPos )
		{			
			// �����������
			if ( !m_panes[0].m_bPrepareToDel )
			{
				m_panes[0].SetTrackDelFlag(true);
				DrawDelPane(pDC, m_panes[0].m_rctViewBox);
			}					
		}
		else if ( rectTrack.left >= iRightPos )
		{
			// �ұ���������
			if ( !m_panes[1].m_bPrepareToDel )
			{										
				m_panes[1].SetTrackDelFlag(true);
				DrawDelPane(pDC, m_panes[1].m_rctViewBox);
			}
		}
		else if ( rectTrack.left > iLeftPos && rectTrack.left < iRightPos )
		{
			// �м���������
			if ( m_panes[0].m_bPrepareToDel )
			{						
				m_panes[0].SetTrackDelFlag(false);																

				// �ָ�
				DrawDelPane(pDC, m_panes[0].m_rctViewBox);
			}			
			
			if ( m_panes[1].m_bPrepareToDel )
			{
				m_panes[1].SetTrackDelFlag(false);												

				// �ָ�
				DrawDelPane(pDC, m_panes[1].m_rctViewBox);
			}
		}		
	}
	else
	{
		int32 iTopPos    = m_panes[0].m_rctViewBox.top  + VALIDAREASIZE;
		int32 iBottomPos = m_panes[1].m_rctViewBox.bottom - rectTrack.Height() - VALIDAREASIZE;
		
		// TRACE(L"Top = %d Bottom = %d rect.top = %d \n", iTopPos, iBottomPos, rectTrack.top);

		if ( rectTrack.top <= iTopPos )
		{
			// �ϱ���������
			if ( !m_panes[0].m_bPrepareToDel )
			{										
				m_panes[0].SetTrackDelFlag(true);
				DrawDelPane(pDC, m_panes[0].m_rctViewBox);
			}					
		}  
		else if ( rectTrack.top >= iBottomPos )
		{
			// �±���������
			if ( !m_panes[1].m_bPrepareToDel )
			{										
				m_panes[1].SetTrackDelFlag(true);
				DrawDelPane(pDC, m_panes[1].m_rctViewBox);
			}
		}
		else if ( rectTrack.top > iTopPos && rectTrack.top < iBottomPos )
		{
			// �м���������
			if ( m_panes[0].m_bPrepareToDel )
			{						
				m_panes[0].SetTrackDelFlag(false);																
				
				// �ָ�
				DrawDelPane(pDC, m_panes[0].m_rctViewBox);
			}			
			
			if ( m_panes[1].m_bPrepareToDel )
			{
				m_panes[1].SetTrackDelFlag(false);												
				
				// �ָ�
				DrawDelPane(pDC, m_panes[1].m_rctViewBox);
			}
		}
	}

	ReleaseDC(pDC);
}

void CBiSplitterWnd::StartTracking(int ht)
{
	ASSERT_VALID(this);
	if (ht != splitterBar)
		return;
	SetCapture();
	SetFocus();
	// make sure no updates are pending
	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW);
	// set tracking state and appropriate cursor
	m_trackingState = SPLITTER;
	m_rctTracker = m_rctGap;
	DrawTracker(m_rctTracker);
	//m_pActiveView = (CView*)CTabSplitWnd::m_pMainFram->SendMessage(UM_FindGGTongView);
}

void CBiSplitterWnd::TrackSplitter(const CPoint &point)
{
    switch(m_bswStyles & SPLITTER_STYLES)
	{
	case VSPLITTER:
		{
			// move tracker to current cursor position        
            CRect rctInside;
			GetClientRect(&rctInside);
			rctInside.left = m_panes[0].m_rctViewBox.left;
			rctInside.right = m_panes[1].m_rctViewBox.right;
			DrawTracker(m_rctTracker);

			if(rctInside.PtInRect(point))
				m_rctTracker.OffsetRect(point.x - m_rctTracker.left, 0);
			else if(point.x < rctInside.left)
				m_rctTracker.OffsetRect(rctInside.left - m_rctTracker.left, 0);
			else if(point.x > rctInside.right)
				m_rctTracker.OffsetRect(rctInside.right - m_rctTracker.right, 0);
			DrawTracker(m_rctTracker);
			break;
		}
	case HSPLITTER:
		{
            // move tracker to current cursor position        
			CRect rctInside;
			GetClientRect(&rctInside);
			rctInside.top = 0;//m_panes[0].m_rctViewBox.top;
			rctInside.bottom = m_panes[1].m_rctViewBox.bottom - m_nGap;
			DrawTracker(m_rctTracker);

			if(rctInside.PtInRect(point))
				m_rctTracker.OffsetRect(0, point.y - m_rctTracker.top);
			else if(point.y < rctInside.top)
				m_rctTracker.OffsetRect(0, rctInside.top - m_rctTracker.top);
			else if(point.y > rctInside.bottom)
				m_rctTracker.OffsetRect(0, rctInside.bottom - m_rctTracker.top);
			DrawTracker(m_rctTracker);
			break;
		}
	default:
		ASSERT(FALSE);
	}

	DrawTrackerDelPane(m_rctTracker);
}

bool32 CBiSplitterWnd::StopTracking()
{
    ASSERT_VALID(this);
    
	if(m_trackingState != SPLITTER)
		return false;
	ReleaseCapture();

	bool32 bRet = false;

	//
	if ( !IsLockedSplit() )		// �����������ɾ��
	{
		if ( m_panes[0].m_bPrepareToDel && m_panes[1].m_bPrepareToDel )
		{
			ASSERT(0);
			return false;
		}
		//
		if ( m_panes[0].m_bPrepareToDel  )
		{
			DelPane(0);
			return true;
		}
		else if ( m_panes[1].m_bPrepareToDel )
		{
			DelPane(1);
			return true;
		}
	}
	else
	{
		if ( m_panes[0].m_bPrepareToDel && m_panes[1].m_bPrepareToDel )
		{
			//ASSERT(0);
			return false;
		}
		//
		if ( m_panes[0].m_bPrepareToDel  )
		{
			m_panes[0].SetTrackDelFlag(false);
			::InvalidateRect(m_panes[0].m_pView->GetSafeHwnd(), NULL, TRUE);
			bRet = true;
		}
		else if ( m_panes[1].m_bPrepareToDel )
		{
			m_panes[1].SetTrackDelFlag(false);
			::InvalidateRect(m_panes[1].m_pView->GetSafeHwnd(), NULL, TRUE);
			bRet = true;
		}
	}

	if ( NULL != m_pActiveView )
	{
		CFrameWnd* pParentFrame = GetParentFrame();
		pParentFrame->SetActiveView(m_pActiveView);		
	}

	return bRet;
}

BOOL CBiSplitterWnd::PreTranslateMessage(MSG* pMsg)
{
	if ( WM_MOUSEMOVE == pMsg->message )
	{
		CPoint pt;
		::GetCursorPos(&pt);
		ScreenToClient(&pt);

		if ( splitterBar == HitTest(pt) )
		{
			if (m_bCanDrag)
			{
				SendMessage(WM_SETCURSOR, splitterBar, WM_MOUSEMOVE);
			}
			return TRUE;
		}
	}
	else if ( WM_LBUTTONDOWN == pMsg->message )
	{
		CPoint pt;
		::GetCursorPos(&pt);
		ScreenToClient(&pt);
		
		if ( splitterBar == HitTest(pt) )
		{
			if (m_bCanDrag)
			{
				// �϶�			
				SendMessage(WM_LBUTTONDOWN, (WPARAM)MK_LBUTTON , MAKELPARAM(pt.x, pt.y));
			}
			return TRUE;
		}
	}

	return FALSE;
}

int CBiSplitterWnd::HitTest(CPoint pt) const
{
	ASSERT_VALID(this);


	if(m_panes[0].IsVisible() && !m_panes[0].IsGripped() && m_panes[0].m_closeBtn.PtInRect(pt))
		return closeBtn0;
	
	if(m_panes[1].IsVisible() && !m_panes[1].IsGripped() && m_panes[1].m_closeBtn.PtInRect(pt))
		return closeBtn1;

	if(m_rctGap.IsVisible())
	{
 		CRect rectSensitive = m_rctGap;
 		if ( BeVSplit() )
 		{
 			// �����з�
			rectSensitive.left    -= KiSensitiveSize;
 			rectSensitive.right   += KiSensitiveSize;
 		}
 		else
 		{
 			// ����з�
			rectSensitive.top    -= KiSensitiveSize;
 			rectSensitive.bottom += KiSensitiveSize; 			
 		}
		
		if(rectSensitive.PtInRect(pt))
			return splitterBar;

// 		if(m_rctGap.m_gripBtn[0].PtInRect(pt))
// 			return gripBtn0;
// 
// 		if(m_rctGap.m_gripBtn[1].PtInRect(pt))
// 			return gripBtn1;
// 
// 		if(m_rctGap.PtInRect(pt))
// 			return splitterBar;
	}
	
// 	CWnd* pParent = GetParent();
// 	
// 	if ( NULL != pParent && pParent->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)) )
// 	{
// 		CBiSplitterWnd* pParent = (CBiSplitterWnd*)pParent;
// 		pParent->HitTest(pt);			
// 	}

	return noHit;
}


/******************************* private members *********************************/
void CBiSplitterWnd::RclHideBothPanes(void)
{
//	m_panes[0].HidePane();
//	m_panes[1].HidePane();
	m_rctGap.HideGap();
	Invalidate(TRUE);
}

void CBiSplitterWnd::RclExpandSecondPane(const CRect &rctClient, BOOL isGapVisible)
{
	m_rctGap.ShowGap(isGapVisible);
//	m_panes[0].HidePane();
	RecalcPane(1, rctClient.left, rctClient.top, rctClient.right, rctClient.bottom);
	Invalidate(FALSE);
}

void CBiSplitterWnd::RclExpandFirstPane(const CRect &rctClient, BOOL isGapVisible)
{
	m_rctGap.ShowGap(isGapVisible);
//	m_panes[1].HidePane();
	RecalcPane(0, rctClient.left, rctClient.top, rctClient.right, rctClient.bottom);
	Invalidate(FALSE);
}

void CBiSplitterWnd::RclPanesV(const CRect &rctClient)
{
	if(rctClient.IsRectEmpty())
		return;
	//int minClientWidth = 2 * (BorderWidth(m_panes[0].m_bspStyles) + BorderWidth(m_panes[1].m_bspStyles)) + (int)m_nGap; 
	//if(minClientWidth >= rctClient.Width())
	//{
	//	RclHideBothPanes();
	//	return;
	//}

	if( m_nSplitterDenominator == 0)
	{
		m_nSplitterNumerator = m_nSplitterPos;
		m_nSplitterDenominator = rctClient.Width();
	}

	//calculate the Panes new width
	switch(m_bswStyles & SIZING_MODE_STYLES)
	{
	case FIXED0:
		//nothing to do 
		break;
	case FIXED1:
		m_nSplitterPos = rctClient.Width() - (m_nSplitterDenominator - m_nSplitterNumerator);
		m_nSplitterDenominator = rctClient.Width();
		m_nSplitterNumerator = m_nSplitterPos;
		break;
	case PROPORTIONAL:
		m_nSplitterPos = MulDiv(m_nSplitterNumerator, rctClient.Width(), m_nSplitterDenominator);//(m_nSplitterPos * rctClient.Width()) / m_nSplitterPos2;
		break;
	default:
		ASSERT(FALSE);
	}

	int minSplitterPos = 2 * BorderWidth(m_panes[0].m_bspStyles);
	int maxSplitterPos = rctClient.Width() - 2 * BorderWidth(m_panes[1].m_bspStyles) - m_nGap;

	// ��԰������ָ�Ľ������Сֵ����
	// ������������ͻ�Ļ����ڶ���pane�Ḳ�ǵ�һ��pane��
	MINMAXINFO mmi1, mmi2;
	if ( GetPaneMinMaxInfo(0, mmi1) )
	{
		// ��ֱ�ָ���������
		const int iBorderWidth = BorderWidth(m_panes[0].m_bspStyles);
		if ( m_nSplitterPos - rctClient.left - iBorderWidth > mmi1.ptMaxTrackSize.x )
		{
			// ��ֵ�޶�
			m_nSplitterPos = rctClient.left + iBorderWidth + mmi1.ptMaxTrackSize.x;
		}
		if ( m_nSplitterPos - rctClient.left - iBorderWidth < mmi1.ptMinTrackSize.x )
		{
			// Сֵ�޶�
			m_nSplitterPos = rctClient.left + iBorderWidth + mmi1.ptMinTrackSize.x;
		}
	}
	if ( GetPaneMinMaxInfo(1, mmi2) )
	{
		// ��ֱ�ָ���������
		const int iBorderWidth = BorderWidth(m_panes[1].m_bspStyles);
		if ( rctClient.right - m_nSplitterPos - iBorderWidth - (int)m_nGap > mmi2.ptMaxTrackSize.x )
		{
			m_nSplitterPos = rctClient.right - iBorderWidth - mmi2.ptMaxTrackSize.x - m_nGap;
		}
		if ( rctClient.right - m_nSplitterPos - iBorderWidth - (int)m_nGap < mmi2.ptMinTrackSize.x )
		{
			m_nSplitterPos = rctClient.right - iBorderWidth - mmi2.ptMinTrackSize.x - m_nGap;
		}
	}

	if(m_nSplitterPos < minSplitterPos)
		m_nSplitterPos = minSplitterPos;
	if(m_nSplitterPos > maxSplitterPos)
		m_nSplitterPos = maxSplitterPos;

	RecalcPane(0, rctClient.left, rctClient.top, m_nSplitterPos, rctClient.bottom);
	RecalcPane(1, m_nSplitterPos + m_nGap, rctClient.top, rctClient.right, rctClient.bottom);
	RecalcGap(m_nSplitterPos, 0, m_nSplitterPos + m_nGap, rctClient.Height());
	m_rctGap.ShowGap();
//	m_panes[0].ShowPane();
//	m_panes[1].ShowPane();
	Invalidate(FALSE);
}


void CBiSplitterWnd::RclPanesH(const CRect &rctClient)
{
	//TODO: calculate min client height
	if(rctClient.IsRectEmpty())
		return;
	if( m_nSplitterDenominator == 0)
	{
		m_nSplitterDenominator = rctClient.Height();
	}

	//calculate the Panes new height
	switch(m_bswStyles & SIZING_MODE_STYLES)
	{	
	case FIXED0:
		//nothing to do 
		break;
	case FIXED1:
		m_nSplitterPos = rctClient.Height() - (m_nSplitterDenominator - m_nSplitterNumerator);
		m_nSplitterDenominator = rctClient.Height();
		m_nSplitterNumerator = m_nSplitterPos;
		break;
	case PROPORTIONAL:
		m_nSplitterPos = MulDiv(m_nSplitterNumerator, rctClient.Height(), m_nSplitterDenominator);
		break;
	default:
		ASSERT(FALSE);
	}
	
	int minSplitterPos = 2 * BorderWidth(m_panes[0].m_bspStyles);
	int maxSplitterPos = rctClient.Height() - 2 * BorderWidth(m_panes[1].m_bspStyles) - m_nGap;

	// ��԰������ָ�Ľ������Сֵ����
	// ������������ͻ�Ļ����ڶ���pane�Ḳ�ǵ�һ��pane��
	MINMAXINFO mmi1, mmi2;
	if ( GetPaneMinMaxInfo(0, mmi1) )
	{
		// ˮƽ�ָ���������
		const int iBorderWidth = BorderWidth(m_panes[0].m_bspStyles);
		if ( m_nSplitterPos - rctClient.top - iBorderWidth > mmi1.ptMaxTrackSize.y )
		{
			m_nSplitterPos = rctClient.top + iBorderWidth + mmi1.ptMaxTrackSize.y;
		}
		if ( m_nSplitterPos - rctClient.top - iBorderWidth < mmi1.ptMinTrackSize.y )
		{
			m_nSplitterPos = rctClient.top + iBorderWidth + mmi1.ptMinTrackSize.y;
		}
	}
	if ( GetPaneMinMaxInfo(1, mmi2) )
	{
		// ˮƽ�ָ���������
		const int iBorderWidth = BorderWidth(m_panes[1].m_bspStyles);
		int iA = rctClient.bottom - m_nSplitterPos - iBorderWidth - m_nGap;
		int iB = mmi2.ptMaxTrackSize.y;
		int iC = mmi2.ptMinTrackSize.y;
		if ( iA > iB )
		{
			m_nSplitterPos = rctClient.bottom - iBorderWidth - mmi2.ptMaxTrackSize.y - m_nGap;
		}
		if ( iA < iC )
		{
			m_nSplitterPos = rctClient.bottom - iBorderWidth - mmi2.ptMinTrackSize.y - m_nGap;
		}
	}

	if(m_nSplitterPos < minSplitterPos)
		m_nSplitterPos = minSplitterPos;
	if(m_nSplitterPos > maxSplitterPos)
		m_nSplitterPos = maxSplitterPos;

	RecalcPane(0, rctClient.left, rctClient.top, rctClient.right, m_nSplitterPos);
	RecalcPane(1, rctClient.left, m_nSplitterPos + m_nGap, rctClient.right, rctClient.bottom);
	RecalcGap(0, m_nSplitterPos, rctClient.right, m_nSplitterPos + m_nGap);
	m_rctGap.ShowGap();
//	m_panes[0].ShowPane();
//	m_panes[1].ShowPane();
	Invalidate(FALSE);
}


/************************* CBiSplitterWnd message handlers ****************************/


BOOL CBiSplitterWnd::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (!CWnd::OnNcCreate(lpCreateStruct))
		return FALSE;

	// remove WS_EX_CLIENTEDGE style from parent window
	//  (the splitter itself will provide the 3d look)
	CWnd* pParent = GetParent();
	ASSERT_VALID(pParent);
	pParent->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_DRAWFRAME);

	return TRUE;
}

void CBiSplitterWnd::OnSize(UINT nType, int cx, int cy)
{
	if (nType != SIZE_MINIMIZED && cx > 0 && cy > 0)
	{
		RecalcLayout();	
	}
	
	CWnd::OnSize(nType, cx, cy);
}

void CBiSplitterWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	int32 iButton = 0;
	iButton = TRightBtnHitTest(point);
	if (INVALID_ID != iButton)
	{
		if (iButton == m_RightBtnZoom.GetControlId())
		{
			m_RightBtnZoom.LButtonDown();
		}
	}

	iButton = TTopBtnHitTest(point);
	if (INVALID_ID != iButton)
	{
		if (iButton == m_TopBtnZoom.GetControlId())
		{
			m_TopBtnZoom.LButtonDown();
		}
	}

//	CWnd::OnLButtonDown(nFlags, point);
	SetFocus();

	if(m_trackingState != NOTRACKING)
		return;
	CClientDC dc(this);
	switch(HitTest(point))
	{
	case splitterBar:
        StartTracking(splitterBar);
		break;
	case closeBtn0:
		SetCapture();
		SetFocus();
		// make sure no updates are pending
		RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW);
		// set tracking state and appropriate cursor
		m_trackingState = CLOSEBTN0;
		m_panes[0].m_closeBtn.m_curState = CBtn::DOWN;
		DrawCloseBtn(&dc, m_panes);
		break;
	case closeBtn1:
		SetCapture();
		SetFocus();
		// make sure no updates are pending
		RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW);
		// set tracking state and appropriate cursor
		m_trackingState = CLOSEBTN1;
		m_panes[1].m_closeBtn.m_curState = CBtn::DOWN;
		DrawCloseBtn(&dc, m_panes + 1);
		break;
	case gripBtn0:
		SetCapture();
		SetFocus();
		// make sure no updates are pending
		RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW);
		// set tracking state and appropriate cursor
		m_trackingState = GRIPBTN0;
		m_rctGap.m_gripBtn[0].m_curState = CBtn::DOWN;
		DrawGripBtn(&dc, 0);
		break;
	case gripBtn1:
		SetCapture();
		SetFocus();
		// make sure no updates are pending
		RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW);
		// set tracking state and appropriate cursor
		m_trackingState = GRIPBTN1;
		m_rctGap.m_gripBtn[1].m_curState = CBtn::DOWN;
		DrawGripBtn(&dc, 1);
		break;
	case noHit:
		m_trackingState = NOTRACKING;
		break;
	default:
		ASSERT(FALSE);
	}
}

void CBiSplitterWnd::OnLButtonUp(UINT nFlags, CPoint point)
{	
	int32 iButton = 0;
	iButton = TRightBtnHitTest(point);
	if (INVALID_ID != iButton)
	{
		if (iButton == m_RightBtnZoom.GetControlId())
		{
			m_RightBtnZoom.LButtonUp();
		}
	}	

	iButton = TTopBtnHitTest(point);
	if (INVALID_ID != iButton)
	{
		if (iButton == m_TopBtnZoom.GetControlId())
		{
			m_TopBtnZoom.LButtonUp();
		}
	}

	bool32 bDelByTracking = false;

	switch(m_trackingState)
	{
	case SPLITTER:
		{
			bDelByTracking = StopTracking();
			if ( !bDelByTracking )
			{
				SetSplitterPos(m_bswStyles & VSPLITTER ? m_rctTracker.left : m_rctTracker.top);
			}
		}
		break;
	case CLOSEBTN0:
		m_panes[0].m_closeBtn.m_curState = CBtn::NEUTRAL;
		m_trackingState = NOTRACKING;
		ReleaseCapture();
		if(m_panes[0].m_closeBtn.PtInRect(point))
            HidePane(BSW_FIRST_PANE);
		break;
	case CLOSEBTN1:
		m_panes[1].m_closeBtn.m_curState = CBtn::NEUTRAL;
		m_trackingState = NOTRACKING;
		ReleaseCapture();
		if(m_panes[1].m_closeBtn.PtInRect(point))
            HidePane(1);
		break;
	case GRIPBTN0:
		m_rctGap.m_gripBtn[0].m_curState = CBtn::NEUTRAL;
		m_trackingState = NOTRACKING;
		ReleaseCapture();
		if(m_rctGap.m_gripBtn[0].PtInRect(point))
            GripPane(0);
		break;
	case GRIPBTN1:
		m_rctGap.m_gripBtn[1].m_curState = CBtn::NEUTRAL;
		m_trackingState = NOTRACKING;
		ReleaseCapture();
		if(m_rctGap.m_gripBtn[1].PtInRect(point))
            GripPane(1);
		break;
	default:
		ASSERT(m_trackingState == NOTRACKING);
	}

	if ( !bDelByTracking )
	{
		if(m_lastActivePane != -1)
			SetActivePane(m_lastActivePane);
	}
}

void CBiSplitterWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!m_isTrackMouseEvent)
	{
		TRACKMOUSEEVENT trackMouseEvent;
		trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
		trackMouseEvent.dwFlags = TME_LEAVE;
		trackMouseEvent.hwndTrack = m_hWnd;
		_TrackMouseEvent(&trackMouseEvent);
		m_isTrackMouseEvent = TRUE;
	}

	int32 iButton = TRightBtnHitTest(point);
	if (INVALID_ID != iButton)
	{	
		if (iButton == m_RightBtnZoom.GetControlId())
		{
			m_RightBtnZoom.MouseHover();
			m_iRightBtnHovering = iButton;
		}
	}
	else
	{
		m_RightBtnZoom.MouseLeave();
	}

	iButton = TTopBtnHitTest(point);
	if (INVALID_ID != iButton)
	{	
		if (iButton == m_TopBtnZoom.GetControlId())
		{
			m_TopBtnZoom.MouseHover();
			m_iTopBtnHovering = iButton;
		}
	}
	else
	{
		m_TopBtnZoom.MouseLeave();
	}

	switch(m_trackingState)
	{
	case SPLITTER:
			TrackSplitter(point);
		break;
	case CLOSEBTN0:
		{
			int hitTest = HitTest(point);
			if(hitTest == closeBtn0 && m_panes[0].m_closeBtn.m_curState != CBtn::DOWN)
			{
	            CClientDC dc(this);
		        m_panes[0].m_closeBtn.m_curState = CBtn::DOWN;
			    DrawCloseBtn(&dc, m_panes);
			}
			else if(hitTest != closeBtn0 && m_panes[0].m_closeBtn.m_curState != CBtn::NEUTRAL)
			{
				CClientDC dc(this);
				m_panes[0].m_closeBtn.m_curState = CBtn::NEUTRAL;
				DrawCloseBtn(&dc, m_panes);
			}
		}
		break;
	case CLOSEBTN1:
		{
			int hitTest = HitTest(point);
			if(hitTest == closeBtn1 && m_panes[1].m_closeBtn.m_curState != CBtn::DOWN)
			{
	            CClientDC dc(this);
		        m_panes[1].m_closeBtn.m_curState = CBtn::DOWN;
			    DrawCloseBtn(&dc, m_panes + 1);
			}
			else if(hitTest != closeBtn1 && m_panes[1].m_closeBtn.m_curState != CBtn::NEUTRAL)
			{
				CClientDC dc(this);
				m_panes[1].m_closeBtn.m_curState = CBtn::NEUTRAL;
				DrawCloseBtn(&dc, m_panes + 1);
			}
		}
		break;
	case GRIPBTN0:
		{
			int hitTest = HitTest(point);
			if(hitTest == gripBtn0 && m_rctGap.m_gripBtn[0].m_curState != CBtn::DOWN)
			{
	            CClientDC dc(this);
		        m_rctGap.m_gripBtn[0].m_curState = CBtn::DOWN;
			    DrawGripBtn(&dc, 0);
			}
			else if(hitTest != gripBtn0 && m_rctGap.m_gripBtn[0].m_curState != CBtn::NEUTRAL)
			{
				CClientDC dc(this);
				m_rctGap.m_gripBtn[0].m_curState = CBtn::NEUTRAL;
				DrawGripBtn(&dc, 0);
			}
		}
		break;
	case GRIPBTN1:
		{
			int hitTest = HitTest(point);
			if(hitTest == gripBtn1 && m_rctGap.m_gripBtn[1].m_curState != CBtn::DOWN)
			{
	            CClientDC dc(this);
		        m_rctGap.m_gripBtn[1].m_curState = CBtn::DOWN;
			    DrawGripBtn(&dc, 1);
			}
			else if(hitTest != gripBtn1 && m_rctGap.m_gripBtn[1].m_curState != CBtn::NEUTRAL)
			{
				CClientDC dc(this);
				m_rctGap.m_gripBtn[1].m_curState = CBtn::NEUTRAL;
				DrawGripBtn(&dc, 1);
			}
		}
		break;
	case NOTRACKING:
		{
			int hitTest = HitTest(point);
			if(hitTest == closeBtn0 && m_panes[0].m_closeBtn.m_curState != CBtn::UP)
			{
	            CClientDC dc(this);
		        m_panes[0].m_closeBtn.m_curState = CBtn::UP;
			    DrawCloseBtn(&dc, m_panes);
			}
			else if(hitTest != closeBtn0 && m_panes[0].m_closeBtn.m_curState != CBtn::NEUTRAL)
			{
				CClientDC dc(this);
				m_panes[0].m_closeBtn.m_curState = CBtn::NEUTRAL;
				DrawCloseBtn(&dc, m_panes);
			}
			else if(hitTest == closeBtn1 && m_panes[1].m_closeBtn.m_curState != CBtn::UP)
			{
	            CClientDC dc(this);
		        m_panes[1].m_closeBtn.m_curState = CBtn::UP;
			    DrawCloseBtn(&dc, m_panes + 1);
			}
			else if(hitTest != closeBtn1 && m_panes[1].m_closeBtn.m_curState != CBtn::NEUTRAL)
			{
				CClientDC dc(this);
				m_panes[1].m_closeBtn.m_curState = CBtn::NEUTRAL;
				DrawCloseBtn(&dc, m_panes + 1);
			}

			else if(hitTest == gripBtn0 && m_rctGap.m_gripBtn[0].m_curState != CBtn::UP)
			{
	            CClientDC dc(this);
		        m_rctGap.m_gripBtn[0].m_curState = CBtn::UP;
			    DrawGripBtn(&dc, 0);
			}
			else if(hitTest != gripBtn0 && m_rctGap.m_gripBtn[0].m_curState != CBtn::NEUTRAL)
			{
				CClientDC dc(this);
				m_rctGap.m_gripBtn[0].m_curState = CBtn::NEUTRAL;
				DrawGripBtn(&dc, 0);
			}

			else if(hitTest == gripBtn1 && m_rctGap.m_gripBtn[1].m_curState != CBtn::UP)
			{
	            CClientDC dc(this);
		        m_rctGap.m_gripBtn[1].m_curState = CBtn::UP;
			    DrawGripBtn(&dc, 1);
			}
			else if(hitTest != gripBtn1 && m_rctGap.m_gripBtn[1].m_curState != CBtn::NEUTRAL)
			{
				CClientDC dc(this);
				m_rctGap.m_gripBtn[1].m_curState = CBtn::NEUTRAL;
				DrawGripBtn(&dc, 1);
			}

		}
		break;
	default:
		ASSERT(FALSE);
	}
	CWnd::OnMouseMove(nFlags, point);
}

BOOL CBiSplitterWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    CPoint pt;
    ::GetCursorPos(&pt);
	ScreenToClient(&pt);
	int hitTest = HitTest(pt);

	if((hitTest == splitterBar || m_trackingState == SPLITTER) && m_bCanDrag)
	{
		SetCursor((m_bswStyles & VSPLITTER ? m_hSplitterCursorV : m_hSplitterCursorH));
		return TRUE;
	}
	SetCursor(m_hArrowCursor);
	return TRUE;

//	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CBiSplitterWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
//draw all items
	DrawSplitterGap(&dc, m_rctGap);
	for( CPane *pPane = m_panes; pPane < m_panes + 2; pPane++)
	{	
		DrawPane(&dc, pPane);
	}

	//DrawSplitterGap(&dc, m_rctGap);

	//for( CPane *pPane = m_panes; pPane < m_panes + 2; pPane++)
	//{	
	//	if(pPane->IsVisible())
	//	{
	//		DrawPane(&dc, pPane);
 //           pPane->m_pView->UpdateWindow();
	//	}
	//}
	////draw splitter gap
}

LRESULT CBiSplitterWnd::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	if (INVALID_ID != m_iRightBtnHovering)
	{
		if (m_iRightBtnHovering == m_RightBtnZoom.GetControlId())
		{
			m_RightBtnZoom.MouseLeave();
		}

		m_iRightBtnHovering = INVALID_ID;
	}

	if (INVALID_ID != m_iTopBtnHovering)
	{
		if (m_iTopBtnHovering == m_TopBtnZoom.GetControlId())
		{
			m_TopBtnZoom.MouseLeave();
		}

		m_iTopBtnHovering = INVALID_ID;
	}

    for(int i = 0; i < 2; ++i)
	{
        if(m_panes[i].m_closeBtn.m_curState != CBtn::NEUTRAL)
		{
            CClientDC dc(this);
            m_panes[i].m_closeBtn.m_curState = CBtn::NEUTRAL;
            DrawCloseBtn(&dc, m_panes + i);
		}
        if(m_rctGap.m_gripBtn[i].m_curState != CBtn::NEUTRAL)
		{
            CClientDC dc(this);
            m_rctGap.m_gripBtn[i].m_curState = CBtn::NEUTRAL;
            DrawGripBtn(&dc, i);
		}
	}
	m_isTrackMouseEvent = FALSE;
	return TRUE;	
}

void CBiSplitterWnd::OnCaptureChanged(CWnd *pWnd)
{
	switch(m_trackingState)
	{
	case SPLITTER:
		DrawTracker(m_rctTracker);
		break;
	case CLOSEBTN0:
		{
			m_panes[0].m_closeBtn.m_curState = CBtn::NEUTRAL;
            CClientDC dc(this);
			DrawCloseBtn(&dc, m_panes);
		}
		break;
	case CLOSEBTN1:
		{
			m_panes[1].m_closeBtn.m_curState = CBtn::NEUTRAL;
			CClientDC dc(this);
			DrawCloseBtn(&dc, m_panes + 1);
		}
		break;
	case GRIPBTN0:
		{
			m_rctGap.m_gripBtn[0].m_curState = CBtn::NEUTRAL;
            CClientDC dc(this);
			DrawGripBtn(&dc, 0);
		}
		break;
	case GRIPBTN1:
		{
			m_rctGap.m_gripBtn[1].m_curState = CBtn::NEUTRAL;
            CClientDC dc(this);
			DrawGripBtn(&dc, 1);
		}
		break;
	default:
		ASSERT(m_trackingState == NOTRACKING);
	}

	m_trackingState = NOTRACKING;
	m_isTrackMouseEvent = FALSE;
	CWnd::OnCaptureChanged(pWnd);
}

void CBiSplitterWnd::OnCancelMode()
{
	CWnd::OnCancelMode();

	if (GetCapture() == this)
		ReleaseCapture();
}

void CBiSplitterWnd::OnDisplayChange()
{
	UpdateSysMetrics();
	UpdateSysColors();
	UpdateSysImages();
	RecalcLayout();
}

void CBiSplitterWnd::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_autoDelete)
        delete this;
	CWnd::PostNcDestroy();
}

//////////////////////////////////////////////////////////////////////////
// ...fangz1119
const char * CBiSplitterWnd::GetXmlElementValue()
{
	return KStrElementValue;
}

const char * CBiSplitterWnd::GetXmlElmAttrBiSplitIsH()
{
	return KStrElementAttrIsH; 
}

const char * CBiSplitterWnd::GetXmlElmAttrBiSplitScale()
{
	return KStrElementAttrScale; 
}

const char * CBiSplitterWnd::GetXmlElmAttrBiSplitRightRect()
{
	return KStrElementAttrRightRect; 
}

const char * CBiSplitterWnd::GetXmlElmAttrBiSplitTopRect()
{
	return KStrElementAttrTopRect; 
}

const char * CBiSplitterWnd::GetXmlElmAttrBiSplitCanDrag()
{
	return KStrElementAttrCanDrag; 
}

bool32 CBiSplitterWnd::FromXml(TiXmlElement *pTiXmlElement)
{
	return true;
}

CString CBiSplitterWnd::ToXml(CView *pActiveView)
{	
	CWnd * pWnd			= GetPaneView(0);
	CWnd * pWndanother  = GetPaneView(1);

	if (NULL == pWnd || NULL == pWndanother)
	{
		return L"";
	}
	
	CString StrThis;
	
	// �õ�����,����������;
	CString StrElmtAttrIsH, StrElmtAttrScale;
	CString StrElmtAttrRightRect, StrElmtAttrTopRect, StrElmtAttrCanDrag;

	if (m_bRightBtn)
	{
		StrElmtAttrRightRect = L"1";
	}
	else
	{
		StrElmtAttrRightRect = L"0";
	}

	if (m_bTopBtn)
	{
		StrElmtAttrTopRect = L"1";
	}
	else
	{
		StrElmtAttrTopRect = L"0";
	}

	if (m_bCanDrag)
	{
		StrElmtAttrCanDrag = L"1";
	}
	else
	{
		StrElmtAttrCanDrag = L"0";
	}
	
	if (m_bswStyles & VSPLITTER)
	{
		StrElmtAttrIsH = L"0";
	}
	else
	{
		StrElmtAttrIsH = L"1";
	}
	
	// ���¼������ - F7ʱ��������ClientArea̫�󣬵��±������
	RecalcRatio();
	double dRatio = 0.0;

	if ( 0.0 == m_dRatio  )
	{
		if ( 0.0 != m_dRatioXml )
		{
			dRatio = m_dRatioXml;	
		}		
		else
		{
			// ��Ӧ�ó���
			dRatio = 0.5;
		}
	}
	else
	{
		dRatio = m_dRatio;
	}

	if ( 0.0 != m_dF9Ratio )
	{
		dRatio = m_dF9Ratio;
	}

	CString StrFormat;
	StrFormat.Format(_T("%%0.%dlf"), DBL_DIG);
	StrElmtAttrScale.Format(StrFormat, (dRatio * 100));
	
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\">  \n", 
		CString(GetXmlElementValue()).GetBuffer(),
		CString(GetXmlElmAttrBiSplitIsH()).GetBuffer(),
		StrElmtAttrIsH.GetBuffer(),
		CString(GetXmlElmAttrBiSplitScale()).GetBuffer(),
		StrElmtAttrScale.GetBuffer(),
		CString(GetXmlElmAttrBiSplitRightRect()).GetBuffer(),
		StrElmtAttrRightRect.GetBuffer(),
		CString(GetXmlElmAttrBiSplitTopRect()).GetBuffer(),
		StrElmtAttrTopRect.GetBuffer(),		
		CString(GetXmlElmAttrBiSplitCanDrag()).GetBuffer(),
		StrElmtAttrCanDrag.GetBuffer());
	
	//if (pWnd->IsKindOf(RUNTIME_CLASS(CGGTongView)))
	if (pWnd->SendMessage(UM_ISKINDOFCGGTongView))
	{
		//StrThis += ((CGGTongView*)pWnd)->ToXml(pActiveView);
		TCHAR * sz = (TCHAR *)pWnd->SendMessage(UM_TOXML,(WPARAM)pActiveView);
		if(sz != NULL)
		{
			StrThis += sz;

			delete []sz;
		}		
	}
	else if (pWnd->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)))
	{
		StrThis +=  ((CBiSplitterWnd*)pWnd)->ToXml(pActiveView);
	}
	
	//if (pWndanother->IsKindOf(RUNTIME_CLASS(CGGTongView)))
	if(pWndanother->SendMessage(UM_ISKINDOFCGGTongView))
	{
		//StrThis += ((CGGTongView*)pWndanother)->ToXml(pActiveView);	
		TCHAR * sz = (TCHAR *)pWndanother->SendMessage(UM_TOXML,(WPARAM)pActiveView);
		if(sz != NULL)
		{
			StrThis += sz;

			delete []sz;
		}		
	}
	
	else if (pWndanother->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)))
	{
		StrThis += ((CBiSplitterWnd*)pWndanother)->ToXml(pActiveView);
	}
	
	// 
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";
	
	//
	return StrThis;
}

bool32 CBiSplitterWnd::BeVSplit() const
{
	switch (m_bswStyles & SPLITTER_STYLES)
	{		
	case VSPLITTER: 
		{
			return true;
		} 
		break;
	case HSPLITTER:
		{
			return false;
		}
		break;
	}

	return false;
}

bool32 CBiSplitterWnd::BeTracking() const
{
	if ( SPLITTER == m_trackingState )
	{
		return true;
	}
	
	return false;
}

void CBiSplitterWnd::SetRadio(double dradio, bool32 bF9 /*= false*/)
{
	if ( !bF9 )
	{
		if ( dradio <= 0.0 || dradio >= 1 )
		{
			dradio = 0.5;
		}
		
		m_dRatio = dradio;
	}
	else
	{
		if ( 0.0 == m_dF9Ratio )
		{
			if (m_bRightBtn)
			{
				m_bRightZoom = FALSE;
				m_bShowRightBk = FALSE;
				m_RightBtnZoom.SetVisiable(FALSE);
			}

			if (m_bTopBtn)
			{
				m_bTopZoom = FALSE;
				m_bShowTopBk = FALSE;
				m_TopBtnZoom.SetVisiable(FALSE);
			}

			m_dF9Ratio = m_dRatio;
			m_dRatio   = dradio;

			if ( dradio == KdF9Radion )
			{
				TestMaxPane(true, 0);				
			}
			else
			{
				TestMaxPane(true, 1);				
			}
			
			SetF9LockSplit(true);	// F7�Ŵ���Զ��������ڷָ�
		}
		else
		{
			if (m_bRightBtn)
			{
				m_RightBtnZoom.SetVisiable(TRUE);
				m_bShowRightBk = TRUE;
			}

			if (m_bTopBtn)
			{
				m_TopBtnZoom.SetVisiable(TRUE);
				m_bShowTopBk = TRUE;
			}

			m_dRatio   = m_dF9Ratio;
			m_dF9Ratio = 0.0;

			if ( dradio == KdF9Radion )
			{
				TestMaxPane(false, 0);				
			}
			else
			{
				TestMaxPane(false, 1);				
			}

			SetF9LockSplit(false);		// ȡ��F7�ķָ�
		}
	}
	
	if ( bF9 )
	{
		return;
	}

	CRect rect;
	GetClientRect(&rect);
	int cx = rect.Width();
	int cy = rect.Height();
	
	if ( cx <= 0 || cy <= 0 )
	{
		return;
	}
	
	if(m_bswStyles & VSPLITTER)
	{
		int iSplitterPos = (int)((rect.right - rect.left) * m_dRatio);
		SetSplitterPos(iSplitterPos);
	}
	else if(m_bswStyles & HSPLITTER)
	{
		int iSplitterPos = (int)((rect.bottom - rect.top) * m_dRatio);
		SetSplitterPos(iSplitterPos);
	}
	
	RecalcLayout();
}

void CBiSplitterWnd::SetSplitterParam(bool32 bRight, bool32 bTop, bool32 bCanDrag)
{
	m_bRightBtn = bRight;
	m_bTopBtn = bTop;
	m_bCanDrag = bCanDrag;
}

void CBiSplitterWnd::SetRadioXml(double dradio)
{
	m_dRatioXml = dradio;
}

double CBiSplitterWnd::GetF9Ratio(CWnd * pWnd)
{
	double dRatio = 0.0;

	if ( NULL == pWnd )
	{
		return dRatio;
	}

	CWnd * pWnd1 = GetPaneView(BSW_FIRST_PANE);
	CWnd * pWnd2 = GetPaneView(BSW_SECOND_PANE);
	
	if ( pWnd == pWnd1 )
	{
		return KdF9Radion;
	}
	else if ( pWnd == pWnd2 )
	{
		return (double(1.0) - KdF9Radion);
	}
	else
	{
		return dRatio;
	}
}

void CBiSplitterWnd::AdjustLayoutByRadioXml()
{
	CRect rctClient;
	GetClientRect(&rctClient);
	
	if ( rctClient.Width() <= 0 || rctClient.Height() <= 0 )
	{
		return;
	}

	m_dRatio	= m_dRatioXml;
	m_dRatioXml = 0.0; // �˴�ע���Ǵ�recalclayout���ã������set�ᵼ�µ���recalclayout����ֹ�ظ�����
	
	if(m_bswStyles & VSPLITTER)
	{
		SetSplitterPos((int)((rctClient.right - rctClient.left) * m_dRatio));
	}
	else if(m_bswStyles & HSPLITTER)
	{
		SetSplitterPos((int)((rctClient.bottom - rctClient.top) * m_dRatio));
	}
}

void CBiSplitterWnd::RecalcRatio()
{
	// F7�£������������Frame�Ĵ�С�� - ����m_dRadio�������
	CRect RectClient;
	GetClientRect(&RectClient);
	
	int32 cx = RectClient.Width();
	int32 cy = RectClient.Height();

	// ���÷ָ�ϵ��������
	cx = m_nSplitterDenominator;
	cy = m_nSplitterDenominator;
	
	
	
	if ( m_bswStyles & VSPLITTER )
	{
		if ( cx > 0 )
		{
			//m_dRatio = (double)iPos / (double)cx;
			m_dRatio = m_nSplitterNumerator / (double)cx;
		}		
	}
	else
	{
		if ( cy > 0 )
		{
			//m_dRatio = (double)iPos / (double)cy;
			m_dRatio = m_nSplitterNumerator / (double)cy;
		}
	}
}

void CBiSplitterWnd::DelAllChildGGTongView(CBiSplitterWnd * pBiSplitWnd)
{
	if (NULL == pBiSplitWnd)
		return;

	CWnd * pWnd1 = pBiSplitWnd->GetPaneView(BSW_FIRST_PANE);
	CWnd * pWnd2 = pBiSplitWnd->GetPaneView(BSW_SECOND_PANE);
	
	if ( NULL == pWnd1 || NULL == pWnd2 )
	{
		return;
	}
	
	//
	//CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	CFrameWnd *pParentFrame = GetParentFrame();

	if (pWnd1->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)))
	{
		DelAllChildGGTongView((CBiSplitterWnd*)pWnd1);
	}
	else if (pWnd1->SendMessage(UM_ISKINDOFCGGTongView)/*IsKindOf(RUNTIME_CLASS(CGGTongView))*/)
	{
		//pParentFrame->DelSplit((CView *)pWnd1);
		pParentFrame->SendMessage(UM_DelSplit,(WPARAM)pWnd1);
	}
	
	if (pWnd2->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)))
	{
		DelAllChildGGTongView((CBiSplitterWnd*)pWnd2);
	}
	else if (pWnd2->SendMessage(UM_ISKINDOFCGGTongView)/*IsKindOf(RUNTIME_CLASS(CGGTongView))*/)
	{
		//pParentFrame->DelSplit((CView *)pWnd2);
		pParentFrame->SendMessage(UM_DelSplit,(WPARAM)pWnd2);
	}
}

void CBiSplitterWnd::DelPane(UINT uiD)
{
	if ( uiD != BSW_FIRST_PANE && uiD != BSW_SECOND_PANE )
	{
		return;
	}

	CWnd * pWndToDel = GetPaneView(uiD);
	CWnd * pWndLeave = GetPaneView(1-uiD);
	
	if ( NULL == pWndToDel || NULL == pWndLeave )
	{
		return;
	}
	
	if (pWndToDel->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)))
	{
		// ����ɾ��pWndToDel �����GGTongView;
		DelAllChildGGTongView((CBiSplitterWnd *)pWndToDel);
	}
	else if ( pWndToDel->SendMessage(UM_ISKINDOFCGGTongView)/*IsKindOf(RUNTIME_CLASS(CGGTongView))*/)
	{
		CFrameWnd * pParentFrame = GetParentFrame();//DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
		//pParentFrame->DelSplit((CView *)pWndToDel);
		pParentFrame->SendMessage(UM_DelSplit,(WPARAM)pWndToDel);
	}
}

int32 CBiSplitterWnd::GetViewPaneID(CWnd* pWnd)
{
	if ( NULL == pWnd )
	{	
		return -1;
	}

	if ( GetPaneView(BSW_FIRST_PANE) == pWnd )
	{
		return BSW_FIRST_PANE;
	}
	else if ( GetPaneView(BSW_SECOND_PANE) == pWnd )
	{
		return BSW_SECOND_PANE;
	}

	return -1;
}

void CPane::SetTrackDelFlag(bool32 bDel)
{
	m_bPrepareToDel = bDel;

	// ...fangz1124 ��ҵ�����,��Ҫ��
 	if ( m_pView->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)) )
 	{
 		CBiSplitterWnd* pSplit = (CBiSplitterWnd*)m_pView;
 		pSplit->m_panes[0].SetTrackDelFlag(bDel);
 		pSplit->m_panes[1].SetTrackDelFlag(bDel);
 	}
 	else if ( m_pView->SendMessage(UM_ISKINDOFCGGTongView)/*IsKindOf(RUNTIME_CLASS(CGGTongView))*/ )
 	{
 		//CGGTongView* pGGTongView = (CGGTongView*)m_pView;
		m_pView->SendMessage(UM_SetBiSplitTrackDelFlag,(WPARAM)bDel);
 		//pGGTongView->SetBiSplitTrackDelFlag(bDel);
 	}
}

void CBiSplitterWnd::TestMaxPane(bool32 bMax, UINT uID)
{
	if ( uID != BSW_FIRST_PANE && uID != BSW_SECOND_PANE )
	{
		return;
	}
	if ( bMax )
	{
		HidePane(1-uID);
	}
	else
	{
		ShowPane(1-uID);
	}
	return;
	
	CWnd* pPaneNow = this;
	CWnd* pParent  = GetParent();
	
	while (pParent)
	{			
		if ( pParent->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)) )
		{
			// ���׻����зִ���,���������ڵ��Ǹ� pane ���		
			CBiSplitterWnd* pParentSplit = (CBiSplitterWnd*)pParent;
			
			int32 iD = pParentSplit->GetViewPaneID(pPaneNow);
			
			if ( iD != BSW_FIRST_PANE && iD != BSW_SECOND_PANE )
			{
				//ASSERT(0);
			}
			
			if ( bMax )
			{
				pParentSplit->HidePane(1-iD);
			}
			else
			{
				pParentSplit->ShowPane(1-iD);
			}
			break;
		}
		else
		{
			if ( bMax )
			{
				HidePane(1-uID);
			}
			else
			{
				ShowPane(1-uID);
			}

			break;
		}
		
		//
		pPaneNow = pParent; 
		pParent  = pParent->GetParent();
	}	
}

BOOL CBiSplitterWnd::IsLockedSplit() const
{
	CFrameWnd *pWnd = GetParentFrame();
	if ( NULL != pWnd && pWnd->SendMessage(UM_ISKINDOFCMPIChildFrame)/*IsKindOf(RUNTIME_CLASS(CMPIChildFrame))*/ )
	{
		//CMPIChildFrame *pChildFrame = (CMPIChildFrame *)pWnd;
		//return pChildFrame->IsLockedSplit();

		return ::SendMessage(pWnd->GetSafeHwnd(), UM_IsLockedSplit, 0, 0);
	}
	return FALSE;	// Ĭ�ϲ�����
}

void CBiSplitterWnd::OnGetMinMaxInfo( MINMAXINFO* lpMMI )
{
	CWnd::OnGetMinMaxInfo(lpMMI);
	if ( NULL != lpMMI )
	{
		lpMMI->ptMinTrackSize = CPoint(0, 0);	// ����Ҫ��Сֵ
		CWnd *pPane0 = GetPaneView(0);
		CWnd *pPane1 = GetPaneView(1);
		MINMAXINFO mmi1, mmi2, mmiOld;
		mmi1 = *lpMMI;
		mmi2 = *lpMMI;
		mmiOld = *lpMMI;
		if ( NULL != pPane0 )
		{
			pPane0->SendMessage(WM_GETMINMAXINFO, NULL, (LPARAM)&mmi1);
			if ( m_bswStyles & VSPLITTER )
			{
				// ���ָ���������
				lpMMI->ptMinTrackSize.x += mmi1.ptMinTrackSize.x;
				lpMMI->ptMinTrackSize.y = max(mmi1.ptMinTrackSize.y, lpMMI->ptMinTrackSize.y);

				// ���е������ȡ����֮��
				// �߶�ȡ���ߵ�Сֵ
				lpMMI->ptMaxTrackSize.x += mmi1.ptMaxTrackSize.x;
				lpMMI->ptMaxTrackSize.y = min(mmi1.ptMaxTrackSize.y, lpMMI->ptMaxTrackSize.y);
			}
			else
			{
				// ��ָ���������
				lpMMI->ptMinTrackSize.x = max(mmi1.ptMinTrackSize.x, lpMMI->ptMinTrackSize.x);
				lpMMI->ptMinTrackSize.y += mmi1.ptMinTrackSize.y;

				// ���е������ȡСֵ
				// �߶�ȡ���ߵĺ�
				lpMMI->ptMaxTrackSize.x = min(mmi1.ptMaxTrackSize.x, lpMMI->ptMaxTrackSize.x);
				lpMMI->ptMaxTrackSize.y += mmi1.ptMaxTrackSize.y;
			}
		}
		else
		{
			ASSERT( 0 );
		}
		if ( NULL != pPane1 )
		{
			pPane1->SendMessage(WM_GETMINMAXINFO, NULL, (LPARAM)&mmi2);
			if ( m_bswStyles & VSPLITTER )
			{
				// ���ָ���������
				lpMMI->ptMinTrackSize.x += mmi2.ptMinTrackSize.x;
				lpMMI->ptMinTrackSize.y = max(mmi2.ptMinTrackSize.y, lpMMI->ptMinTrackSize.y);

				lpMMI->ptMaxTrackSize.x += mmi2.ptMaxTrackSize.x;
				lpMMI->ptMaxTrackSize.y = min(mmi2.ptMaxTrackSize.y, lpMMI->ptMaxTrackSize.y);
			}
			else
			{
				// ��ָ���������
				lpMMI->ptMinTrackSize.x = max(mmi2.ptMinTrackSize.x, lpMMI->ptMinTrackSize.x);
				lpMMI->ptMinTrackSize.y += mmi2.ptMinTrackSize.y;

				lpMMI->ptMaxTrackSize.x = min(mmi2.ptMaxTrackSize.x, lpMMI->ptMaxTrackSize.x);
				lpMMI->ptMaxTrackSize.y += mmi2.ptMaxTrackSize.y;
			}
		}
		else
		{
			ASSERT( 0 );
		}

		// ���Ҳֻ�������洫����������
		lpMMI->ptMaxTrackSize.x = min(mmiOld.ptMaxTrackSize.x, lpMMI->ptMaxTrackSize.x);
		lpMMI->ptMaxTrackSize.y = min(mmiOld.ptMaxTrackSize.y, lpMMI->ptMaxTrackSize.y);
	}
	else
	{
		ASSERT( 0 );
	}
}

void CBiSplitterWnd::InitMinMaxInfo( OUT MINMAXINFO &mmi )
{
	ZeroMemory(&mmi, sizeof(mmi));
	mmi.ptMaxPosition.x = 0;
	mmi.ptMaxPosition.y = 0;
	mmi.ptMaxSize.x = GetSystemMetrics(SM_CXMAXIMIZED);
	mmi.ptMaxSize.y = GetSystemMetrics(SM_CYMAXIMIZED);

	mmi.ptMaxTrackSize.x = GetSystemMetrics(SM_CXMAXTRACK);
	mmi.ptMaxTrackSize.y = GetSystemMetrics(SM_CYMAXTRACK);
	mmi.ptMinTrackSize.x = GetSystemMetrics(SM_CXMINTRACK);
	mmi.ptMinTrackSize.y = GetSystemMetrics(SM_CYMINTRACK);
}

bool32 CBiSplitterWnd::GetPaneMinMaxInfo( int iPane, OUT MINMAXINFO &mmi )
{
	if ( iPane < 0 || iPane > 1 )
	{
		ASSERT( 0 );
		return false;
	}
	CWnd *pPane = GetPaneView(iPane);
	if ( NULL != pPane )
	{
		InitMinMaxInfo(mmi);
		pPane->SendMessage(WM_GETMINMAXINFO, NULL, (LPARAM)&mmi);
		return true;
	}
	return false;
}

void CBiSplitterWnd::SetF9LockSplit( bool32 bLock )
{
	CFrameWnd *pFrame = GetParentFrame();
	if ( NULL != pFrame )
	{
		//CMPIChildFrame *pParent = DYNAMIC_DOWNCAST(CMPIChildFrame, pFrame);
		//if ( NULL != pParent )
		//{
			pFrame->SendMessage(UM_SetF7AutoLock,bLock);//SetF7AutoLock(bLock);
		//}
	}
}

bool32 CBiSplitterWnd::IsF9LockSplit() const
{
	CFrameWnd *pFrame = GetParentFrame();
	if ( NULL != pFrame )
	{
		//CMPIChildFrame *pParent = DYNAMIC_DOWNCAST(CMPIChildFrame, pFrame);
		//if ( NULL != pParent )
		{
			return pFrame->SendMessage(UM_IsF7AutoLock);//IsF7AutoLock();
		}
	}
	return false;	// Ĭ����F9
}

CWnd				* CBiSplitterWnd::GetF9MaxPane()
{
	// ��ʵ�ϣ����f7�����и������f9���ڲ���һ����view���п�����view�ļ��� pane

	if ( m_dF9Ratio != 0.0 ) // ֱ�����õ�0.0���ܱȽϣ�
	{
		CWnd *pPane1 = GetPaneView(BSW_FIRST_PANE);
		CWnd *pPane2 = GetPaneView(BSW_SECOND_PANE);
		if ( m_dRatio == GetF9Ratio(pPane1) )
		{
			return pPane1;
		}
		else if ( m_dRatio == GetF9Ratio(pPane2) )
		{
			return pPane2;
		}
	}
	return NULL;
}

int32 CBiSplitterWnd::TRightBtnHitTest(CPoint point)
{
	if (m_RightBtnZoom.PtInButton(point) && m_RightBtnZoom.GetCreate())
	{
		return m_RightBtnZoom.GetControlId();
	}

	return INVALID_ID;
}

int32 CBiSplitterWnd::TTopBtnHitTest(CPoint point)
{
	if (m_TopBtnZoom.PtInButton(point) && m_TopBtnZoom.GetCreate())
	{
		return m_TopBtnZoom.GetControlId();
	}

	return INVALID_ID;
}

BOOL CBiSplitterWnd::OnCommand( WPARAM wParam, LPARAM lParam )
{
	int32 iID = (int32)wParam;
	if (ID_ZOOM_RIGHT == iID)
	{
		m_bRightZoom = !m_bRightZoom;
		if (m_bRightZoom)
		{
			TestMaxPane(true, 0);
		}
		else
		{
			TestMaxPane(false, 0);
		}
		if(!m_bRightZoom)
		{
			m_RightBtnZoom.SetImage(m_pImgZoomLeft);
		}
		else
		{
			m_RightBtnZoom.SetImage(m_pImgZoomRight);
		}
	}
	else if (ID_ZOOM_TOP == iID)
	{
		m_bTopZoom = !m_bTopZoom;
		if (m_bTopZoom)
		{
			TestMaxPane(true, 1);
		}
		else
		{
			TestMaxPane(false, 1);
		}
		if(!m_bTopZoom)
		{
			m_TopBtnZoom.SetImage(m_pImgZoomTop);
		}
		else
		{
			m_TopBtnZoom.SetImage(m_pImgZoomBottom);
		}
	}

	return CWnd::OnCommand(wParam, lParam);
}