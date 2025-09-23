#include "StdAfx.h"
#include "ExportMainThreadWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define UM_TOMAINTHREADWNDPROCESS	(WM_USER+0x100)

IMPLEMENT_DYNAMIC(CExportMainThreadWnd, CWnd);

BEGIN_MESSAGE_MAP(CExportMainThreadWnd, CWnd)
ON_MESSAGE(UM_TOMAINTHREADWNDPROCESS, OnMsgToMainProcess)
END_MESSAGE_MAP();

CExportMainThreadWnd::CExportMainThreadWnd()
{
	
}

CExportMainThreadWnd::~CExportMainThreadWnd()
{
	
}

CExportMainThreadWnd & CExportMainThreadWnd::Instance()
{
	static CExportMainThreadWnd sObj;
	return sObj;
}

bool CExportMainThreadWnd::InitInstance()
{
	if ( NULL==GetSafeHwnd() )
	{
		BOOL bRet = Create(NULL, _T("CExportMainThreadWnd"), WS_CHILD, CRect(0,0,0,0), CWnd::FromHandle(((HWND)-3)), 0x1234);
		ASSERT( NULL!=GetSafeHwnd() );
		if ( !bRet )
		{
			return false;
		}
	}

	return true;
}

void CExportMainThreadWnd::UnInit()
{
	if ( NULL!=GetSafeHwnd() )
	{
		DestroyWindow();
	}
}

bool CExportMainThreadWnd::SendToMainProcess( iExportMainThreadWndProcessor *pProcessor, LPARAM l )
{
	if ( NULL==pProcessor || NULL==GetSafeHwnd() )
	{
		ASSERT( 0 );
		return false;
	}

	return SendMessage(UM_TOMAINTHREADWNDPROCESS, (WPARAM)pProcessor, l)!=0;
}

LRESULT CExportMainThreadWnd::OnMsgToMainProcess( WPARAM w, LPARAM l )
{
	iExportMainThreadWndProcessor *pProcess = (iExportMainThreadWndProcessor *)w;
	if ( NULL==pProcess )
	{
		ASSERT( 0 );
		return 1;
	}

	return pProcess->MainThreadWndProcess(l);
}
