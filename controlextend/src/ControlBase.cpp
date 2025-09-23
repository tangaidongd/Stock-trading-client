#include "stdafx.h"
#include "memdc.h"

#include "ControlBase.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CControlBase, CStatic)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CControlBase, CStatic)
	//{{AFX_MSG_MAP(CControlBase)
	ON_WM_PAINT()
	ON_WM_CHAR()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CControlBase::CControlBase()
{
	m_pParent              = NULL;
	m_rectClient           = CRect(-1,-1,-1,-1);
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CControlBase::~CControlBase()
{
	m_pParent = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
// CreateFromStatic
//
// Purpose:     Create the CControlBase control from STATIC placeholder
//
// Parameters:  dwStyle    - the scroll bar抯 style. Typically this will be
//                           SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE.
//              pParentWnd - the scroll bar抯 parent window, usually a CDialog
//                           object. It must not be NULL.
//              nIdStatic  - the resource id of the placeholder STATIC
//              nId        - the resource id of the CControlBase control
//
// Returns:     BOOL       - TRUE = success
//
// Notes:       Hides the STATIC placeholder.  Also loads hand cursor, and
//              sets the thumb bitmap size.

BOOL CControlBase::CreateFromStatic(DWORD dwStyle,
								   CWnd* pParentWnd,
								   UINT nIdStatic,
								   UINT nId)
{
	// TRACE(_T("in CControlBase::CreateFromStatic\n"));

	m_pParent = pParentWnd;
	ASSERT(m_pParent);
	if (NULL == m_pParent)
	{
		return FALSE;
	}
	ASSERT(::IsWindow(pParentWnd->GetDlgItem(nIdStatic)->m_hWnd));

	if (!::IsWindow(pParentWnd->GetDlgItem(nIdStatic)->m_hWnd))
	{
		return FALSE;
	}
	CRect rect;
	pParentWnd->GetDlgItem(nIdStatic)->GetWindowRect(&rect);
	pParentWnd->ScreenToClient(&rect);

	// hide placeholder STATIC
	pParentWnd->GetDlgItem(nIdStatic)->ShowWindow(SW_HIDE);

	BOOL bResult = 	CStatic::Create(_T(""), dwStyle, rect, pParentWnd, nId);

	//if (bResult)
	//{
	//	GetClientRect(&m_rectClient);
	//	// TRACE(_T("m_rectClient:  %d, %d, %d, %d\n"),
	//	//	m_rectClient.left, m_rectClient.top, 
	//	//	m_rectClient.right, m_rectClient.bottom);
	//}
	//else
	//{
	//	// TRACE(_T("ERROR - failed to create CControlBase\n"));
	//	ASSERT(FALSE);
	//}

	return bResult;
}


BOOL CControlBase::Create(DWORD dwStyle,
						 CWnd* pParentWnd,
						 LPRECT lpRect,
						 UINT nId)
{
	// TRACE(_T("in CControlBase::Create\n"));

	ASSERT(pParentWnd);
	m_pParent = pParentWnd;

	return  CStatic::Create(_T(""), dwStyle, *lpRect, pParentWnd, nId);
}

void CControlBase::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CStatic::OnPaint() for painting messages
	CRect rect;
	GetClientRect(rect);

	CClientDC dc1(this);
	CMemDC memDC(&dc1, &rect);

	memDC.FillSolidRect(&rect, 0x00ff00);
}

BOOL CControlBase::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	if (WM_SYSKEYDOWN == pMsg->message)
	{
		UINT nChar		= (UINT)pMsg->wParam;
		UINT nRepCnt	= pMsg->lParam & 0x0ffff;
		UINT nFlags		= (pMsg->lParam >> 16) & 0x0ffff;

		if (TestKeyDown(nChar, nRepCnt, nFlags))
			return TRUE;	
	}

	// 先让自己的视图处理，如果本机处理了， return TRUE, 就不再往父亲传递了
	if (WM_KEYDOWN == pMsg->message)
	{
		UINT nChar		= (UINT)pMsg->wParam;
		UINT nRepCnt	= pMsg->lParam & 0x0ffff;
		UINT nFlags		= (pMsg->lParam >> 16) & 0x0ffff;

		if (TestKeyDown(nChar, nRepCnt, nFlags))
			return TRUE;
	}
	else if (WM_KEYUP == pMsg->message)
	{
		UINT nChar		= (UINT)pMsg->wParam;
		UINT nRepCnt	= pMsg->lParam & 0x0ffff;
		UINT nFlags		= (pMsg->lParam >> 16) & 0x0ffff;

		if (TestKeyUp(nChar, nRepCnt, nFlags))
			return TRUE;
	}
	else if (WM_CHAR == pMsg->message)
	{
		UINT nChar		= (UINT)pMsg->wParam;
		UINT nRepCnt	= pMsg->lParam & 0x0ffff;
		UINT nFlags		= (pMsg->lParam >> 16) & 0x0ffff;

		if (TestChar(nChar, nRepCnt, nFlags))
			return TRUE;
	}

	return CStatic::PreTranslateMessage(pMsg);
}

BOOL CControlBase::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return FALSE;
}

BOOL CControlBase::TestChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return FALSE;
}

BOOL CControlBase::TestKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return FALSE;
}


