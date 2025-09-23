#if !defined(AFX_OLERICHEDITCTRL_H__3DFF15EE_7336_4297_9620_7F00B611DAA1__INCLUDED_)
#define AFX_OLERICHEDITCTRL_H__3DFF15EE_7336_4297_9620_7F00B611DAA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OleRichEditCtrl.h : header file
//


#include <richole.h>
#include "dllexport.h"

/////////////////////////////////////////////////////////////////////////////
// COleRichEditCtrl window

class CONTROL_EXPORT COleRichEditCtrl : public CRichEditCtrl
{
// Construction
public:
	COleRichEditCtrl();
	virtual ~COleRichEditCtrl();

	long StreamInFromResource(int iRes, LPCTSTR sType);

	bool ReadRtfFile(CString strFileName);

	void SetOneFormat(TCHAR* pFontName,COLORREF clrText);	// �������塢��ɫ

protected:
	static DWORD CALLBACK readFunction(DWORD dwCookie,
		 LPBYTE lpBuf,			// the buffer to fill
		 LONG nCount,			// number of bytes to read
		 LONG* nRead);			// number of bytes actually read

	interface IExRichEditOleCallback;	// forward declaration (see below in this header file)

	IExRichEditOleCallback* m_pIRichEditOleCallback;
	BOOL m_bCallbackSet;
	
	
	interface IExRichEditOleCallback : public IRichEditOleCallback
	{
	public:
		IExRichEditOleCallback();
		virtual ~IExRichEditOleCallback();
		int m_iNumStorages;
		IStorage* m_pStorage;
		DWORD m_dwRef;

		virtual HRESULT STDMETHODCALLTYPE GetNewStorage(LPSTORAGE* lplpstg);
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
		virtual ULONG STDMETHODCALLTYPE AddRef();
		virtual ULONG STDMETHODCALLTYPE Release();
		virtual HRESULT STDMETHODCALLTYPE GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
			LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo);
 		virtual HRESULT STDMETHODCALLTYPE ShowContainerUI(BOOL fShow);
 		virtual HRESULT STDMETHODCALLTYPE QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp);
 		virtual HRESULT STDMETHODCALLTYPE DeleteObject(LPOLEOBJECT lpoleobj);
 		virtual HRESULT STDMETHODCALLTYPE QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
			DWORD reco, BOOL fReally, HGLOBAL hMetaPict);
 		virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);
 		virtual HRESULT STDMETHODCALLTYPE GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj);
 		virtual HRESULT STDMETHODCALLTYPE GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect);
 		virtual HRESULT STDMETHODCALLTYPE GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
			HMENU FAR *lphmenu);
	};
 

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COleRichEditCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:


	// Generated message map functions
protected:
	//{{AFX_MSG(COleRichEditCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OLERICHEDITCTRL_H__3DFF15EE_7336_4297_9620_7F00B611DAA1__INCLUDED_)
