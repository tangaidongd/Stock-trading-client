// OleRichEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "OleRichEditCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COleRichEditCtrl

COleRichEditCtrl::COleRichEditCtrl()
{
	m_bCallbackSet = FALSE;
	m_pIRichEditOleCallback = NULL;
	
}
//lint --e{1540} suppress " Pointer member 'COleRichEditCtrl::m_pIRichEditOleCallback'neither freed nor zeroed by destructor -- Effective C++ #6"
//m_pIRichEditOleCallback 属于com指针部分，需要通过调用release释放
COleRichEditCtrl::~COleRichEditCtrl()
{
	// IExRichEditOleCallback class is a reference-counted class  
	// which deletes itself and for which delete should not be called
	
	// delete m_pIRichEditOleCallback;
}


BEGIN_MESSAGE_MAP(COleRichEditCtrl, CRichEditCtrl)
	//{{AFX_MSG_MAP(COleRichEditCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void COleRichEditCtrl::SetOneFormat(TCHAR* pFontName,COLORREF clrText)
{
	CFont font;
	font.CreateFont(   18, // nHeight
					   0, // nWidth
					   0, // nEscapement
					   0, // nOrientation
					   FW_NORMAL, // nWeight
					   FALSE, // bItalic
					   FALSE, // bUnderline
					   0, // cStrikeOut
					   ANSI_CHARSET, // nCharSet
					   OUT_DEFAULT_PRECIS, // nOutPrecision
					   CLIP_DEFAULT_PRECIS, // nClipPrecision
					   DEFAULT_QUALITY, // nQuality
					   DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
					   pFontName); // lpszFacename
	SetFont(&font, true);

	SetBackgroundColor(FALSE, 0x4d4137);
	
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);

	cf.dwMask |= CFM_COLOR;
	cf.crTextColor = clrText;

	SetDefaultCharFormat(cf);
};


int COleRichEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
 	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
 		return -1;
 	
	// m_pIRichEditOleCallback should have been created in PreSubclassWindow

 	ASSERT( m_pIRichEditOleCallback != NULL );	

	// set the IExRichEditOleCallback pointer if it wasn't set 
	// successfully in PreSubclassWindow

	if ( !m_bCallbackSet )
	{
		SetOLECallback( m_pIRichEditOleCallback );
	}
 	
 	return 0;
}

void COleRichEditCtrl::PreSubclassWindow() 
{
	// base class first
	CRichEditCtrl::PreSubclassWindow();	

	m_pIRichEditOleCallback = NULL;
	m_pIRichEditOleCallback = new IExRichEditOleCallback;
	ASSERT( m_pIRichEditOleCallback != NULL );

	m_bCallbackSet = SetOLECallback( m_pIRichEditOleCallback );
}

bool COleRichEditCtrl::ReadRtfFile(CString strFileName)
{
	CFile file;
	CFileException e;
	if( !file.Open( strFileName, CFile::modeRead, &e ) )
	{
#ifdef _DEBUG
		afxDump << "File could not be opened " << e.m_cause << "\n";
#endif
		return false;
	}
	
	EDITSTREAM es;
	es.pfnCallback = readFunction;
	es.dwError = 0;
	es.dwCookie = (DWORD) &file;
	
	return StreamIn( SF_RTF, es ) > 0 ;

}

long COleRichEditCtrl::StreamInFromResource(int iRes, LPCTSTR sType)
{
	HINSTANCE hInst = AfxGetInstanceHandle();
	HRSRC hRsrc = ::FindResource(hInst,
		MAKEINTRESOURCE(iRes), sType);
	
	if (NULL != hRsrc)
	{
		DWORD len = SizeofResource(hInst, hRsrc); 
		BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc); 
		ASSERT(lpRsrc); 

		CMemFile mfile;
		mfile.Attach(lpRsrc, len); 

		EDITSTREAM es;
		es.pfnCallback = readFunction;
		es.dwError = 0;
		es.dwCookie = (DWORD) &mfile;
		return StreamIn( SF_RTF, es );
	}
	
	return 0;
	
}


/* static */
DWORD CALLBACK COleRichEditCtrl::readFunction(DWORD dwCookie,
		 LPBYTE lpBuf,			// the buffer to fill
		 LONG nCount,			// number of bytes to read
		 LONG* nRead)			// number of bytes actually read
{
	CFile* fp = (CFile *)dwCookie;
	*nRead = fp->Read(lpBuf,nCount);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

COleRichEditCtrl::IExRichEditOleCallback::IExRichEditOleCallback()
{
	m_pStorage = NULL;
	m_iNumStorages = 0;
	m_dwRef = 0;

	// set up OLE storage

	HRESULT hResult = ::StgCreateDocfile(NULL,
		STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE /*| STGM_DELETEONRELEASE */|STGM_CREATE ,
		0, &m_pStorage );

	if ( m_pStorage == NULL ||
		hResult != S_OK )
	{
		AfxThrowOleException( hResult );
	}
}

COleRichEditCtrl::IExRichEditOleCallback::~IExRichEditOleCallback()
{
	if (NULL != m_pStorage)
	{
		m_pStorage->Release();
		m_pStorage = NULL;
	}

}

HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetNewStorage(LPSTORAGE* lplpstg)
{
	if (NULL == m_pStorage)
	{
		return S_FALSE;
	}

	m_iNumStorages++;
	WCHAR tName[50];
	swprintf(tName, L"REOLEStorage%d", m_iNumStorages);

	HRESULT hResult = m_pStorage->CreateStorage(tName, 
		STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE ,
		0, 0, lplpstg );

	if (hResult != S_OK )
	{
		::AfxThrowOleException( hResult );
	}
	return hResult;
}

HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::QueryInterface(REFIID iid, void ** ppvObject)
{

	HRESULT hr = S_OK;
	*ppvObject = NULL;
	
	if ( iid == IID_IUnknown ||
		iid == IID_IRichEditOleCallback )
	{
		*ppvObject = this;
		AddRef();
		hr = NOERROR;
	}
	else
	{
		hr = E_NOINTERFACE;
	}

	return hr;
}



ULONG STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::AddRef()
{
	return ++m_dwRef;
}



ULONG STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::Release()
{
	if ( --m_dwRef == 0 )
	{
		delete this;
		return 0;
	}

	return m_dwRef;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
	LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::ShowContainerUI(BOOL fShow)
{
	return S_OK;
}



HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::DeleteObject(LPOLEOBJECT lpoleobj)
{
	return S_OK;
}



HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
	DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::ContextSensitiveHelp(BOOL fEnterMode)
{
	return S_OK;
}



HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj)
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
	HMENU FAR *lphmenu)
{
	return S_OK;
}






