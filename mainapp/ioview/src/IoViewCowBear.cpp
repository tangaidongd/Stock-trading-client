#include "StdAfx.h"
#include "IoViewCowBear.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
const int32 KIRequestDataTimerId = 1001;						// 定时获取牛熊状态
const int32	KIRequestDataTimerPeriod = 60*1000*2;

IMPLEMENT_DYNAMIC(CCowBearState, CWnd)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CCowBearState, CWnd)
//{{AFX_MSG_MAP(CCowBearState)
ON_WM_TIMER()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////

CCowBearState::CCowBearState()
{
	m_eCowBearState = EDSCount;	
}

CCowBearState & CCowBearState::Instance()
{
	static CCowBearState cowBear;
	if ( cowBear.m_hWnd == NULL )
	{
		// #define HWND_MESSAGE     ((HWND)-3)
		//HWND hwnd = ::CreateWindowEx(0, _T("Static"), _T("牛熊分布"), WS_CHILD, 0, 0,0, 0, ((HWND)-3), 0, AfxGetInstanceHandle(), NULL);
		cowBear.CreateEx(0, _T("Static"), _T("CowBear"), WS_POPUP, CRect(0,0,0,0), NULL, 0, NULL );
		//cowBear.Attach(hwnd);
		if (cowBear.m_hWnd != NULL)
		{
			cowBear.SetTimer(KIRequestDataTimerId, KIRequestDataTimerPeriod, NULL);
		}
	}
	return cowBear;
}

void CCowBearState::OnTimer( UINT nIDEvent )
{
	if(!IsWindowVisible())
	{
		return;
	}

	if ( KIRequestDataTimerId == nIDEvent )
	{
		RequestViewData();
	}
	CWnd::OnTimer(nIDEvent);
}

void CCowBearState::RequestViewData()
{
	CGGTongDoc *pDoc = AfxGetDocument();
	if ( pDoc != NULL && pDoc->m_pAbsCenterManager != NULL )
	{
		// 是否需要无论什么版本都请求该信息
		CMmiReqDapanState	req;
		pDoc->m_pAbsCenterManager->RequestViewData(&req);
	}
}

CCowBearState::~CCowBearState()
{
	CWnd::DestroyWindow();
}


