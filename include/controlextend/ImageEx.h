// GDIPlusHelper.h: interface for the CGDIPlusHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GDIPLUSHELPER_H__BD5F6266_5686_43E2_B146_5EA1217A56FE__INCLUDED_)
#define AFX_GDIPLUSHELPER_H__BD5F6266_5686_43E2_B146_5EA1217A56FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "dllexport.h"
#include "GdiPlusTS.h"
class CONTROL_EXPORT ImageEx : public Image
{
public:
	ImageEx(IN LPCTSTR  sResourceType, IN LPCTSTR  sResource);
	ImageEx(const WCHAR* filename, BOOL useEmbeddedColorManagement = FALSE);

	
	~ImageEx();
public:

	void	Draw(CDC* pDC);
	CSize	GetSize();

	bool	IsAnimatedGIF() { return m_nFrameCount > 1; }
	void	SetPause(bool bPause);
	bool	IsPaused() { return m_bPause; }
	bool	InitAnimation(HWND hWnd, CPoint pt);
	bool    InitAnimation(CDC *pDC, CPoint pt);
	void    StopPlaying();
	//lint -sem(ImageEx::Destroy,cleanup)
	void	Destroy();
	void    SetContentOnGif(WCHAR *pStrConent, CRect &offRect, Color &clr);
	void    SetContentOnGif(CString strText, CRect &offRect, Color &clr);
	void	SetFontEx(LOGFONT *pLogFont);
protected:

	bool				TestForAnimatedGIF();
	//lint -sem(ImageEx::Initialize,initializer)
	void				Initialize();
	bool				DrawFrameGIF();

	IStream*			m_pStream;
	
	bool LoadFromBuffer(BYTE* pBuff, int nSize);
	bool GetResource(LPCTSTR lpName, LPCTSTR lpType, void* pResource, int& nBufSize);
	bool Load(CString sResourceType, CString sResource);

	void ThreadAnimation();

	static UINT WINAPI _ThreadAnimationProc(LPVOID pParam);

	HANDLE			m_hThread;
	HANDLE			m_hPause;
	HANDLE			m_hExitEvent;
	HINSTANCE		m_hInst;
	//HWND			m_hGraphicsWnd;
	HWND			m_hWnd;
	CDC             *m_pDC;
	UINT			m_nFrameCount;
	UINT			m_nFramePosition;
	bool			m_bIsInitialized;
	bool			m_bPause;
	PropertyItem*	m_pPropertyItem;
	CPoint			m_pt;

private:
	WCHAR *m_pStrConent;
	CString m_strText;
	CRect  m_offsetRect;
	Color  m_clrText;
	FontFamily *m_pfontFamily;
	REAL   m_fontSize;
	StringFormat m_sf;
};



#endif // !defined(AFX_GDIPLUSHELPER_H__BD5F6266_5686_43E2_B146_5EA1217A56FE__INCLUDED_)
