// GDIPlusHelper.cpp: implementation of the CGDIPlusHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageEx.h"
#include <process.h>
#include "ShareFun.h"
/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::ImageEx
// 
// DESCRIPTION:	Constructor for constructing images from a resource 
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
ImageEx::ImageEx(LPCTSTR sResourceType, LPCTSTR sResource)
{
	Initialize();

	if (Load(sResourceType, sResource))
	{

	   nativeImage = NULL;
	   
	   lastResult = DllExports::GdipLoadImageFromStreamICM(m_pStream, &nativeImage);
	  
	   if (TestForAnimatedGIF())
	   {
		     m_bIsInitialized = true;
	   }	 
    }

	
}



////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::ImageEx
// 
// DESCRIPTION:	Constructor for constructing images from a file
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
ImageEx::ImageEx(const WCHAR* filename, BOOL useEmbeddedColorManagement) : Image(filename, useEmbeddedColorManagement)
{
	Initialize();
	if (TestForAnimatedGIF())
	{
		m_bIsInitialized = true;
	}	
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::~ImageEx
// 
// DESCRIPTION:	Free up fresources
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
ImageEx::~ImageEx()
{
	Destroy();

	
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	InitAnimation
// 
// DESCRIPTION:	Prepare animated GIF
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::InitAnimation(HWND hWnd, CPoint pt)
{

	m_hWnd = hWnd;
	m_pt = pt;

	if (!m_bIsInitialized)
	{
		TRACE(_T("GIF not initialized\n"));
		return false;
	};

	if (IsAnimatedGIF())
	{
		if (m_hThread == NULL)
		{
		
			unsigned int nTID = 0;

			m_hThread = (HANDLE) _beginthreadex( NULL, 0, _ThreadAnimationProc, this, CREATE_SUSPENDED,&nTID);
			
			if (!m_hThread)
			{
				TRACE(_T("Couldn't start a GIF animation thread\n"));
				return true;
			} 
			else 
				ResumeThread(m_hThread);
		}
	} 

	return false;	

}

bool ImageEx::InitAnimation(CDC *pDC, CPoint pt)
{
	m_pt = pt;
	m_pDC = pDC;
	if (!m_bIsInitialized)
	{
		TRACE(_T("GIF not initialized\n"));
		return false;
	};
	
	if (IsAnimatedGIF())
	{
		if (m_hThread == NULL)
		{
			
			unsigned int nTID = 0;
			
			m_hThread = (HANDLE) _beginthreadex( NULL, 0, _ThreadAnimationProc, this, CREATE_SUSPENDED,&nTID);
			
			if (!m_hThread)
			{
				TRACE(_T("Couldn't start a GIF animation thread\n"));
				return true;
			} 
			else 
				ResumeThread(m_hThread);
		}
	} 
	
	return false;	
}

void ImageEx::StopPlaying()
{
	SuspendThread(m_hThread);
}
////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	LoadFromBuffer
// 
// DESCRIPTION:	Helper function to copy phyical memory from buffer a IStream
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::LoadFromBuffer(BYTE* pBuff, int nSize)
{
	bool bResult = false;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);
	if (hGlobal)
	{
		void* pData = GlobalLock(hGlobal);
		if (pData)
			memcpy(pData, pBuff, nSize);
		
		GlobalUnlock(hGlobal);

		if (CreateStreamOnHGlobal(hGlobal, TRUE, &m_pStream) == S_OK)
			bResult = true;

	}


	return bResult;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	GetResource
// 
// DESCRIPTION:	Helper function to lock down resource
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::GetResource(LPCTSTR lpName, LPCTSTR lpType, void* pResource, int& nBufSize)
{ 
	HRSRC		hResInfo;
	HANDLE		hRes;
	LPSTR		lpRes	= NULL; 	
	bool		bResult	= FALSE;

	// Find the resource

	hResInfo = FindResource(m_hInst , lpName, lpType);
	if (hResInfo == NULL) 
	{		
		return false;
	}

	// Load the resource
	hRes = LoadResource(m_hInst , hResInfo);

	if (hRes == NULL) 
		return false;

	// Lock the resource
	lpRes = (char*)LockResource(hRes);

	if (lpRes != NULL)
	{ 
		if (pResource == NULL)
		{
			nBufSize = SizeofResource(m_hInst , hResInfo);
			bResult = true;
		}
		else
		{
			if (nBufSize >= (int)SizeofResource(m_hInst , hResInfo))
			{
				memcpy(pResource, lpRes, nBufSize);
				bResult = true;
			}
		} 

		UnlockResource(hRes);  
	}

	// Free the resource
	FreeResource(hRes);

	return bResult;
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	Load
// 
// DESCRIPTION:	Helper function to load resource from memory
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::Load(CString sResourceType, CString sResource)
{
	bool bResult = false;


	BYTE*	pBuff = NULL;
	int		nSize = 0;
	if (GetResource(sResource.GetBuffer(0), sResourceType.GetBuffer(0), pBuff, nSize))
	{
		if (nSize > 0)
		{
			pBuff = new BYTE[nSize];

			if (GetResource(sResource, sResourceType.GetBuffer(0), pBuff, nSize))
			{
				if (LoadFromBuffer(pBuff, nSize))
				{

					bResult = true;
				}
			}

			delete [] pBuff;
		}
	}


	m_bIsInitialized = bResult;

	return bResult;
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	GetSize
// 
// DESCRIPTION:	Returns Width and Height object
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
CSize ImageEx::GetSize()
{
	return CSize(GetWidth(), GetHeight());
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	TestForAnimatedGIF
// 
// DESCRIPTION:	Check GIF/Image for avialability of animation
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::TestForAnimatedGIF()
{
   if (NULL != m_pPropertyItem)
   {
	   free(m_pPropertyItem);
	   m_pPropertyItem = NULL;
   }
   
   UINT count = GetFrameDimensionsCount();
   GUID* pDimensionIDs = new GUID[count];

   // Get the list of frame dimensions from the Image object.
   GetFrameDimensionsList(pDimensionIDs, count);

   // Get the number of frames in the first dimension.
   m_nFrameCount = GetFrameCount(&pDimensionIDs[0]);

	// Assume that the image has a property item of type PropertyItemEquipMake.
	// Get the size of that property item.
   int nSize = GetPropertyItemSize(PropertyTagFrameDelay);

   // Allocate a buffer to receive the property item.
   m_pPropertyItem = (PropertyItem*) malloc(nSize);
   if(NULL != m_pPropertyItem)
   {
		GetPropertyItem(PropertyTagFrameDelay, nSize, m_pPropertyItem);
   }
   
   delete  []pDimensionIDs;
   return m_nFrameCount > 1;
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::Initialize
// 
// DESCRIPTION:	Common function called from Constructors
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
void ImageEx::Initialize()
{
	m_pStream = NULL;
	m_nFramePosition = 0;
	m_nFrameCount = 0;
	m_pStream = NULL;
	lastResult = InvalidParameter;
	m_hThread = NULL;
	m_bIsInitialized = false;
	
	
#ifdef INDIGO_CTRL_PROJECT
	m_hInst = _Module.GetResourceInstance();
#else
	m_hInst = AfxGetResourceHandle();
#endif

	m_bPause = false;
	m_hExitEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hPause = CreateEvent(NULL,TRUE,TRUE,NULL);
	m_strText = L"";
	m_offsetRect.SetRectEmpty();
	m_clrText = RGB(0,0,0);
	//lint --e{423} suppress "Creation of memory leak in assignment to 'ImageEx::m_pPropertyItem'"
	////lint 已修过代码，仍然报错内存泄露，直接对其错误429忽略
	m_pPropertyItem = NULL;
	m_hWnd = NULL;
	m_pDC = NULL;
	m_pt.SetPoint(0,0);
	m_pStrConent = NULL;
	m_offsetRect = CRect(0, 0, 0, 0);
	m_pfontFamily = new FontFamily(L"宋体");
	m_fontSize  = 12;
	m_sf.SetAlignment(StringAlignmentCenter);
	//	Image基类成员变量初始化
	lastResult = Ok;
	loadStatus = Ok;
	nativeImage = NULL;

	
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::_ThreadAnimationProc
// 
// DESCRIPTION:	Thread to draw animated gifs
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
UINT WINAPI ImageEx::_ThreadAnimationProc(LPVOID pParam)
{
	ASSERT(pParam);
	if(NULL != pParam)
	{
		ImageEx *pImage = reinterpret_cast<ImageEx *> (pParam);
		pImage->ThreadAnimation();

	}
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::ThreadAnimation
// 
// DESCRIPTION:	Helper function
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
void ImageEx::ThreadAnimation()
{
	m_nFramePosition = 0;

	bool bExit = false;
	while (bExit == false)
	{
		bExit = DrawFrameGIF();
	}
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::DrawFrameGIF
// 
// DESCRIPTION:	
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::DrawFrameGIF()
{	
	::WaitForSingleObject(m_hPause, INFINITE);

	GUID   pageGuid = FrameDimensionTime;

	long hmWidth = GetWidth();
	long hmHeight = GetHeight();
	HDC hDC = NULL;

	if ( m_hWnd )
		 hDC = GetDC(m_hWnd); 

	if ( m_pDC )
		hDC = m_pDC->m_hDC;

	if (hDC)
	{
		if (NULL != m_pfontFamily)
		{
			Graphics graphics(hDC);
			graphics.DrawImage(this, m_pt.x, m_pt.y, hmWidth, hmHeight);
			Gdiplus::Font font(&(*m_pfontFamily), m_fontSize, FontStyleRegular, UnitPixel);
			if ( m_pStrConent )
			{
				SolidBrush solidBrush(m_clrText);
				//graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
				graphics.DrawString(m_pStrConent, wcslen(m_pStrConent), &font, RectF((Gdiplus::REAL)m_offsetRect.left, (Gdiplus::REAL)m_offsetRect.top, (Gdiplus::REAL)m_offsetRect.Width(), (Gdiplus::REAL)m_offsetRect.Height() ),&m_sf, &solidBrush);
			}
			ReleaseDC(NULL, hDC);
		}
		
	}

	SelectActiveFrame(&pageGuid, m_nFramePosition++);		
	
	if (m_nFramePosition == m_nFrameCount)
		m_nFramePosition = 0;

	//	gif图片不存在多帧图片
	if (NULL != m_pPropertyItem)
	{
		long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;
		return WaitForSingleObject(m_hExitEvent, lPause) == WAIT_OBJECT_0;
	}

	return true;
	
}

void ImageEx::SetContentOnGif(CString strText, CRect &offRect, Color &clr)
{
	if ( !strText.IsEmpty() )
		m_strText = strText;
		
	if ( 0 != offRect.top || 0 != offRect.left )
		m_offsetRect = offRect;
	else
		m_offsetRect = CRect(0, 0, 0, 0);
	
	m_clrText = clr;
}

//void ImageEx::SetContentOnGif(WCHAR *pStrConent, CRect &offRect, const Font &font, Color &clr)
void ImageEx::SetContentOnGif(WCHAR *pStrConent, CRect &offRect, Color &clr)
{
	if ( pStrConent )
	{
		DEL_ARRAY(m_pStrConent);
		

		m_pStrConent = new WCHAR[wcslen(pStrConent) + 1];

		wcsncpy(m_pStrConent, pStrConent, wcslen(pStrConent) + 1);
	}
	else
	{
		DEL_ARRAY(m_pStrConent);
	}	

	if ( 0 != offRect.top || 0 != offRect.left )
		m_offsetRect = offRect;
	else
		m_offsetRect = CRect(0, 0, 0, 0);
	
//	font.GetFamily(&m_fontFamily);
//	m_fontSize = font.GetSize();

	//Font font(&fontFamily, 15, FontStyleRegular, UnitPixel);
	//Font *m_pFontText = new Font(font);
	//m_pFontText = new Font(&fontFamily, font.GetSize(), FontStyleRegular, UnitPixel);

	//Font fff(&fontFamily, font.GetSize(), FontStyleRegular, UnitPixel);

	m_clrText = clr;
}

void ImageEx::SetFontEx(LOGFONT *pLogFont)
{
	if ( pLogFont ) 
	{
		DEL(m_pfontFamily);
		m_pfontFamily = new FontFamily(pLogFont->lfFaceName);
	
		if ( pLogFont->lfHeight > 0 )
			m_fontSize = (Gdiplus::REAL)pLogFont->lfHeight;
	}

}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::SetPause
// 
// DESCRIPTION:	Toggle Pause state of GIF
// 
// RETURNS:		
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
void ImageEx::SetPause(bool bPause)
{
	if (!IsAnimatedGIF())
		return;

	if (bPause && !m_bPause)
	{
		::ResetEvent(m_hPause);
	}
	else
	{

		if (m_bPause && !bPause)
		{
			::SetEvent(m_hPause);
		}
	}

	m_bPause = bPause;
}


void ImageEx::Destroy()
{
	
	if (m_hThread)
	{
		// If pause un pause
		SetPause(false);

		SetEvent(m_hExitEvent);
		WaitForSingleObject(m_hThread, INFINITE);
		DEL_HANDLE(m_hThread);
	}

	
	DEL_HANDLE(m_hExitEvent);
	DEL_HANDLE(m_hPause);

	
	
	m_hThread = NULL;
	m_hExitEvent = NULL;
	m_hPause = NULL;

	if (m_pStream)
	{
		m_pStream->Release();
		m_pStream = NULL;
	}
		
	DEL_ARRAY(m_pStrConent);
	m_pStrConent = NULL;
	DEL(m_pfontFamily);
	if (NULL != m_pPropertyItem)
	{
		free(m_pPropertyItem);
		m_pPropertyItem = NULL;	
	}
	m_pfontFamily = NULL;
	m_hWnd = NULL;
	m_pDC = NULL;
	m_hInst = NULL;
	
}