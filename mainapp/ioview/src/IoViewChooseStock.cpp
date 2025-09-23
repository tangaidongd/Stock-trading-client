#include "StdAfx.h"
#include "IoViewChooseStock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
const int32 	KIRequestDataTimerId = 1008;						// 定时获取选股状态
const int32	    KIRequestDataTimerPeriod = 1000 * 60;

IMPLEMENT_DYNAMIC(CChooseStockState, CWnd)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CChooseStockState, CWnd)
//{{AFX_MSG_MAP(CChooseStockState)
ON_WM_TIMER()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////

CChooseStockState::CChooseStockState()
{
	m_uChooseStockStatus = ECSSNONE;	
}

CChooseStockState & CChooseStockState::Instance()
{
	static CChooseStockState chooseStock;
	if ( NULL == chooseStock.m_hWnd )
	{
		chooseStock.CreateEx(0, _T("Static"), _T("ChooseStock"), WS_POPUP, CRect(0,0,0,0), NULL, 0, NULL );
		
		if (chooseStock.m_hWnd != NULL)
		{
			chooseStock.SetTimer(KIRequestDataTimerId, KIRequestDataTimerPeriod, NULL);
		}
	}
	return chooseStock;
}

void CChooseStockState::OnTimer( UINT nIDEvent )
{
	if ( KIRequestDataTimerId == nIDEvent )
	{
		RequestViewData();
	}
	CWnd::OnTimer(nIDEvent);
}

void CChooseStockState::RequestViewData()
{
	CGGTongDoc	*pDoc = AfxGetDocument();
	if ( pDoc != NULL && pDoc->m_pAbsCenterManager != NULL )
	{
		// 是否需要无论什么版本都请求该信息
		CMmiReqChooseStockStatus req;
		pDoc->m_pAbsCenterManager->RequestViewData(&req);
	}
}

CChooseStockState::~CChooseStockState()
{
	m_pChooseStateNotifyPtrList.RemoveAll();

	CWnd::DestroyWindow();
}

void CChooseStockState::SetCurrentChooseStockState(u32 uStatus)
{ 
	m_uChooseStockStatus = uStatus; 

	for (int32 i = 0; i < m_pChooseStateNotifyPtrList.GetSize(); i++)
	{
		CChooseStockStateNotify *pNotify = m_pChooseStateNotifyPtrList[i];
		if (NULL != pNotify)
		{
			pNotify->OnChooseStockStateResp();
		}
	}
}

void CChooseStockState::AddChooseStateNotify(CChooseStockStateNotify *pNotify)
{
	if (NULL == pNotify)
		return;

	bool32 bFind = FALSE;
	for (int32 i = 0; i < m_pChooseStateNotifyPtrList.GetSize(); i++)
	{
		if (m_pChooseStateNotifyPtrList[i] == pNotify)
		{
			bFind = TRUE;
			break;
		}
	}

	if (!bFind)
	{
		m_pChooseStateNotifyPtrList.Add(pNotify);
	}
}

void CChooseStockState::RemoveChooseStateNotify(CChooseStockStateNotify *pNotify)
{
	if (NULL == pNotify)
		return;

	// 找到所有的, 剔除（可能有重复的情况）
	while (1)
	{
		int32 iFindPos = -1;
		for (int32 i = 0; i < m_pChooseStateNotifyPtrList.GetSize(); i++)
		{
			if (m_pChooseStateNotifyPtrList[i] == pNotify)
			{
				iFindPos = i;
				break;
			}
		}

		if (iFindPos >= 0)
		{
			m_pChooseStateNotifyPtrList.RemoveAt(iFindPos, 1);
		}
		else
		{
			break;
		}
	}
}


