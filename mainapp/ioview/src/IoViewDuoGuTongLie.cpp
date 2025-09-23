#include "stdafx.h"
#include "memdc.h"
#include <math.h>
#include "IoViewManager.h"
#include "MerchManager.h"
#include "facescheme.h"
#include "IoViewKLine.h"
#include "IoViewTrend.h"
#include "IoViewDuoGuTongLie.h"
#include "ColorStep.h"
#include "BJSpecialStringImage.h"
#include "PluginFuncRight.h"


//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////
// id
const UINT KIdIoViewKlineStart	= 0x300;
const UINT KIdIoViewTrendStart	= 0x400;

//const int32 KIFixedColumnCount	= 3;

const int32 KTimerIdInit		= 1115;		// ����ʼ����timer
const int32 KTimerIdDoShowAppend = 1116;	// show������ɺ������⴦���timer

const int32 KITopSkip            = 2;                  // ��ť���뵽�������ϱ߾࣬��ť�밴ť�ļ��Ҳȡ��ֵ
const int32 KITopBtnHeight       = 20;                 // ������ť�ĸ߶�
const int32 KITopBtnWidth        = 30;                 // ������ť�Ŀ��
const char* KStrViewIoViewRowCount    = "SubViewRowCount";   // ���ͬ����ʾ����ͼ�ж�����
const char* KStrViewIoViewColCount    = "SubViewColCount";   // ���ͬ����ʾ����ͼ�ж�����
const char* KStrViewSubViewType       = "SubViewType";   // ���ͬ����ʾ����ͼ�ж�����

const int32	KiBorderWidth = 1;							//	�߿���

namespace
{
	#define  INVALID_ID             -1
	#define  TOP_STOCK_NUMBER       4000
	#define  TOP_STOCK_THREE        4001
	#define  TOP_STOCK_SIX          4002
	#define  TOP_STOCK_NINE         4003
	#define  TOP_STOCK_TWELVE       4004
	#define  TOP_STOCK_FIFTEEN      4005

    #define  TOP_CYCLE_TREND        5000
	#define  TOP_CYCLE_DAY          5001
	#define  TOP_CYCLE_WEEK         5002
	#define  TOP_CYCLE_MONTH        5003
	#define  TOP_CYCLE_QUARTER      5004
	#define  TOP_CYCLE_YEAR         5005

	#define  TOP_CYCLE_INVALID      5060	// ��Ч

}

/////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewDuoGuTongLie, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewDuoGuTongLie, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewDuoGuTongLie)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)	
	ON_MESSAGE(UM_HOTKEY, OnHotKeyMsg)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewDuoGuTongLie::CIoViewDuoGuTongLie()
:CIoViewBase()
{
	m_bActive			= false;
	m_iActiveIoView		= 0;
	m_pMerchBase		= NULL;
	m_iSubViewCount     = 0;
	m_iSubViewRowCount  = 0;
	m_iSubViewColCount  = 0;

	m_bTracking			= false;
	m_iTopBtnHovering = INVALID_ID;
	m_uCurTimeInterval = TOP_CYCLE_DAY;
	m_pFirstMerch = NULL;
	//m_RectTopBar.SetRectEmpty();
	m_rtSel.SetRectEmpty();
	m_nShowPicId = ID_PIC_TREND;
	m_DGTLType = EDGLT_KLINE;

	m_aIoViewTrends.RemoveAll();
	m_aIoViewKlines.RemoveAll();
	m_clrSelBorder = RGB(0, 251, 246);
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewDuoGuTongLie::~CIoViewDuoGuTongLie()
{
	RegisterHotKeyTarget(false);

	DeleteSubIoViews();
}

//��ǰ�����л���ͼ�����ģ���ʱֱ�ӷ���
void CIoViewDuoGuTongLie::ResetView( int32 iBtnID )
{
	return;
	switch(iBtnID)
	{
	case TOP_STOCK_THREE:
		m_iSubViewCount = 3;
		m_iSubViewRowCount = 1;
		break;

	case TOP_STOCK_SIX:
		m_iSubViewCount = 6;
		m_iSubViewRowCount = 2;
		break;

	case TOP_STOCK_NINE:
		m_iSubViewCount = 9;
		m_iSubViewRowCount = 3;
		break;

	case TOP_STOCK_TWELVE:
		m_iSubViewCount = 12;
		m_iSubViewRowCount = 4;
		break;

	case TOP_STOCK_FIFTEEN:
		m_iSubViewCount = 15;
		m_iSubViewRowCount = 5;
		break;

	default:
		break;
	}

	DeleteSubIoViews();
	CreateSubViews();
	if(m_pFirstMerch)
	{
		SetBaseMerch(m_pFirstMerch);
	}
	else
	{
		CMerch *pMerch = GetMerchXml();
		if ( NULL != pMerch )
		{
			SetBaseMerch(pMerch);
		}
	}

	if(ID_PIC_KLINE == m_nShowPicId)      // K�ߵĴ��� 
	{
		ChangeKLineCycle(m_uCurTimeInterval);
		RefreshIndex();
	}
}

void CIoViewDuoGuTongLie::RefreshView( CMerch* pMerch )
{
	if(m_iSubViewCount <= 0 )
	{
		return;
	}

	if (pMerch)
	{
		m_pFirstMerch = pMerch;
	}
	else if (m_pMerchXml)
	{
		m_pFirstMerch = m_pMerchXml;
	}
	else
	{
		return;
	}
	
	SetBaseMerch(m_pFirstMerch);

	if(EDGLT_KLINE == m_DGTLType)      // K�ߵĴ��� 
	{
		//ChangeKLineCycle(m_uCurTimeInterval);               //�л������Ǹ�����ͼҪ�������ݣ�����ͳһ�л�
		RefreshIndex();
	}
}

// �л����ڣ���ʱע�͵�
void	CIoViewDuoGuTongLie::KLineCycleChange(UINT nID)
{
	return;
	int iCommand = -1;
	switch (nID)
	{
	case IDM_CHART_KDAY:	
		iCommand = TOP_CYCLE_DAY;
		break;
	case IDM_CHART_KWEEK:	
		iCommand = TOP_CYCLE_WEEK;
		break;
	case IDM_CHART_KMONTH:	
		iCommand = TOP_CYCLE_MONTH;
		break;
	case IDM_CHART_KQUARTER:
		iCommand = TOP_CYCLE_QUARTER;
		break;
	case IDM_CHART_KYEAR:
		iCommand = TOP_CYCLE_YEAR;
		break;
	}
	if (iCommand != -1)
	{
		SendMessage(WM_COMMAND, iCommand);
	}
	else
	{
		if(0 != m_aIoViewKlines.GetSize())
		{
			for (int i =0; i < m_aIoViewKlines.GetSize(); ++i)
			{
				if (NULL != m_aIoViewKlines[i])
				{
					m_aIoViewKlines[i]->KLineCycleChange(nID);
				}			
			}	
		}

		int32 iCycle = FindKLineCycle(nID);
		if (INVALID_ID == iCycle)
		{
			return;
		}

		m_uCurTimeInterval = TOP_CYCLE_INVALID;

		ShowViews(ID_PIC_KLINE, true);
		// �л�����
		if ( m_aIoViewKlines.GetSize() > 0 && m_aIoViewKlines[0]->m_MerchParamArray.GetSize() > 0 &&
			(m_aIoViewKlines[0]->m_MerchParamArray[0]->m_eTimeIntervalFull != iCycle))
		{
			for ( int32 i=0; i < m_aIoViewKlines.GetSize() ; ++i )
			{
				T_MerchNodeUserData *pData = m_aIoViewKlines[i]->m_MerchParamArray.GetSize() > 0 ? m_aIoViewKlines[i]->m_MerchParamArray[0] : NULL;
				if ( NULL == pData )
				{
					continue;
				}
				pData->m_eTimeIntervalFull = E_NodeTimeInterval(iCycle);

				m_aIoViewKlines[i]->SetTimeInterval(*pData, E_NodeTimeInterval(iCycle));
				m_aIoViewKlines[i]->SetCurveTitle(pData);
				m_aIoViewKlines[i]->ReDrawAysnc();
				m_aIoViewKlines[i]->RequestViewData();
			}
		}
		OnUpdateCmdUI(GetParentFrame(), FALSE);

	}
	
}


// �������ڣ���ʱע�͵�
int32 CIoViewDuoGuTongLie::FindKLineCycle( int32 eTimeInterval )
{
	return  INVALID_ID;
	int iCycle = INVALID_ID;
	if (IDM_CHART_KMINUTE == eTimeInterval)
	{
		iCycle = ENTIMinute;
	}
	else if (IDM_CHART_KMINUTE5 == eTimeInterval)
	{
		iCycle = ENTIMinute5;
	}
	else if (IDM_CHART_KMINUTE15 == eTimeInterval)
	{
		iCycle = ENTIMinute15;
	}
	else if (IDM_CHART_KMINUTE30 == eTimeInterval)
	{
		iCycle = ENTIMinute30;
	}
	else if (IDM_CHART_KMINUTE60 == eTimeInterval)
	{
		iCycle = ENTIMinute60;
	}
	else if (IDM_CHART_KDAY == eTimeInterval)
	{
		iCycle = IDM_CHART_KDAY;
	}
	else if (IDM_CHART_KWEEK == eTimeInterval)
	{
		iCycle = ENTIWeek;
	}
	else if (IDM_CHART_KMONTH == eTimeInterval)
	{
		iCycle = ENTIMonth;
	}
	else if (IDM_CHART_KMINUTEX == eTimeInterval)
	{
		iCycle = ENTIMinuteUser;
	}
	else if (IDM_CHART_KQUARTER == eTimeInterval)
	{
		iCycle = ENTIQuarter;
	}
	else if (IDM_CHART_KYEAR == eTimeInterval)
	{
		iCycle = ENTIYear;
	}
	return iCycle;
}

// ֮ǰ������ͳһ�л����ڵģ�������ע��
void	CIoViewDuoGuTongLie::ChangeKLineCycle( int32 iBtnID )
{
	return;
	E_NodeTimeInterval eNodeInterval = ENTIMinute;
	switch(iBtnID)
	{
	case TOP_CYCLE_DAY:
		eNodeInterval = ENTIDay;
		m_uCurTimeInterval = TOP_CYCLE_DAY;
		break;

	case TOP_CYCLE_WEEK:
		eNodeInterval = ENTIWeek;
		m_uCurTimeInterval = TOP_CYCLE_WEEK;
		break;

	case TOP_CYCLE_MONTH:
		eNodeInterval = ENTIMonth;
		m_uCurTimeInterval = TOP_CYCLE_MONTH;
		break;

	case TOP_CYCLE_QUARTER:
		eNodeInterval = ENTIQuarter;
		m_uCurTimeInterval = TOP_CYCLE_QUARTER;
		break;

	case TOP_CYCLE_YEAR:
		eNodeInterval = ENTIYear;
		m_uCurTimeInterval = TOP_CYCLE_YEAR;
		break;
	default:
		break;
	}

	if(m_pFirstMerch)
	{
		SetBaseMerch(m_pFirstMerch);
	}
	else
	{
		CMerch *pMerch = GetMerchXml();
		if ( NULL != pMerch )
		{
			SetBaseMerch(pMerch);
		}
	}

	ShowViews(ID_PIC_KLINE, true);

	// �л�����
	if ( m_aIoViewKlines.GetSize() > 0 && m_aIoViewKlines[0]->m_MerchParamArray.GetSize() > 0 &&
		 (m_aIoViewKlines[0]->m_MerchParamArray[0]->m_eTimeIntervalFull != eNodeInterval))
	{
		for ( int32 i=0; i < m_aIoViewKlines.GetSize() ; ++i )
		{
			T_MerchNodeUserData *pData = m_aIoViewKlines[i]->m_MerchParamArray.GetSize() > 0 ? m_aIoViewKlines[i]->m_MerchParamArray[0] : NULL;
			if ( NULL == pData )
			{
				continue;
			}
			pData->m_eTimeIntervalFull = eNodeInterval;

			m_aIoViewKlines[i]->SetTimeInterval(*pData, eNodeInterval);
			m_aIoViewKlines[i]->SetCurveTitle(pData);
			m_aIoViewKlines[i]->ReDrawAysnc();
			m_aIoViewKlines[i]->RequestViewData();
		}
	}

	OnUpdateCmdUI(GetParentFrame(), FALSE);
}

BOOL CIoViewDuoGuTongLie::OnCommand( WPARAM wParam, LPARAM lParam )
{
	int32 iID = (int32)wParam;

	// ��ť��Ӧ�¼�����ʱע�͵�


	//std::map<int, CNCButton>::iterator itHome = m_mapTopBtn.find(iID);
	//if (m_mapTopBtn.end() != itHome)
	//{	
	//	if(TOP_STOCK_THREE <= iID && TOP_STOCK_FIFTEEN >= iID)
	//	{
	//		ResetView(iID);
	//	}
	//	else if(TOP_CYCLE_DAY <= iID && TOP_CYCLE_YEAR >= iID)
	//	{
	//		m_nShowPicId = ID_PIC_KLINE;
	//		ChangeKLineCycle(iID);
	//		RefreshIndex();
	//	}
	//	else
	//	{
	//		m_nShowPicId = ID_PIC_TREND;

	//		if(m_pFirstMerch)
	//		{
	//			SetBaseMerch(m_pFirstMerch);
	//		}
	//		else
	//		{
	//			CMerch *pMerch = GetMerchXml();
	//			if ( NULL != pMerch )
	//			{
	//				SetBaseMerch(pMerch);
	//			}
	//		}

	//		ShowViews(ID_PIC_TREND);
	//	}
	//}

	return CWnd::OnCommand(wParam, lParam);
}

// ��ʼ����ť������Ҫ����ʱע�͵�
void CIoViewDuoGuTongLie::InitialTopBtns()
{
	return;
	// ���������еİ�ť
	CRect rcBtn(KITopSkip, KITopSkip, KITopSkip + 70, KITopSkip + KITopBtnHeight);    // ��Ʊ������ť����ݶ�Ϊ70
	AddTopButton(&rcBtn, NULL, 3, TOP_STOCK_NUMBER, L"��Ʊ����:");

	rcBtn.left += (KITopSkip + 80);
	rcBtn.right = rcBtn.left + KITopBtnWidth;
	AddTopButton(&rcBtn, NULL, 3, TOP_STOCK_THREE, L"3");

	rcBtn.left += (KITopSkip + KITopBtnWidth);
	rcBtn.right = rcBtn.left + KITopBtnWidth;
	AddTopButton(&rcBtn, NULL, 3, TOP_STOCK_SIX, L"6");

	rcBtn.left += (KITopSkip + KITopBtnWidth);
	rcBtn.right = rcBtn.left + KITopBtnWidth;
	AddTopButton(&rcBtn, NULL, 3, TOP_STOCK_NINE, L"9");

	rcBtn.left += (KITopSkip + KITopBtnWidth);
	rcBtn.right = rcBtn.left + KITopBtnWidth;
	AddTopButton(&rcBtn, NULL, 3, TOP_STOCK_TWELVE, L"12");

	rcBtn.left += (KITopSkip + KITopBtnWidth);
	rcBtn.right = rcBtn.left + KITopBtnWidth;
	AddTopButton(&rcBtn, NULL, 3, TOP_STOCK_FIFTEEN, L"15");

	rcBtn.left += (KITopSkip + 150);
	rcBtn.right = rcBtn.left + KITopBtnWidth;
	AddTopButton(&rcBtn, NULL, 3, TOP_CYCLE_TREND, L"��");

	rcBtn.left += (KITopSkip + KITopBtnWidth);
	rcBtn.right = rcBtn.left + KITopBtnWidth;
	AddTopButton(&rcBtn, NULL, 3, TOP_CYCLE_DAY, L"��");

	rcBtn.left += (KITopSkip + KITopBtnWidth);
	rcBtn.right = rcBtn.left + KITopBtnWidth;
	AddTopButton(&rcBtn, NULL, 3, TOP_CYCLE_WEEK, L"��");

	rcBtn.left += (KITopSkip + KITopBtnWidth);
	rcBtn.right = rcBtn.left + KITopBtnWidth;
	AddTopButton(&rcBtn, NULL, 3, TOP_CYCLE_MONTH, L"��");

	rcBtn.left += (KITopSkip + KITopBtnWidth);
	rcBtn.right = rcBtn.left + KITopBtnWidth;
	AddTopButton(&rcBtn, NULL, 3, TOP_CYCLE_QUARTER, L"��");

	rcBtn.left += (KITopSkip + KITopBtnWidth);
	rcBtn.right = rcBtn.left + KITopBtnWidth;
	AddTopButton(&rcBtn, NULL, 3, TOP_CYCLE_YEAR, L"��");
}

void CIoViewDuoGuTongLie::AddTopButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption)
{
	// ASSERT(pImage);
	CNCButton btnControl;
	btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
	if (TOP_STOCK_NUMBER == nID)
	{
		btnControl.SetTextBkgColor(RGB(42,43,45), RGB(75,81,85), RGB(75,81,85));
		btnControl.SetTextFrameColor(RGB(33,36,41), RGB(33,36,41), RGB(33,36,41));
		btnControl.SetTextColor(RGB(255,255,255), RGB(255,255,255), RGB(255,255,255));
	}
	else
	{
		btnControl.SetTextBkgColor(RGB(42,43,45), RGB(75,81,85), RGB(75,81,85));
		btnControl.SetTextFrameColor(RGB(33,36,41), RGB(75,81,85), RGB(75,81,85));
		btnControl.SetTextColor(RGB(201,208,214), RGB(255,255,255), RGB(255,255,255));
	}

	m_mapTopBtn[nID] = btnControl;
}

int CIoViewDuoGuTongLie::TTopButtonHitTest(CPoint point)
{
	map<int, CNCButton>::iterator iter;

	for (iter=m_mapTopBtn.begin(); iter!=m_mapTopBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		// ��point�Ƿ����ѻ��Ƶİ�ť������
		if (btnControl.PtInButton(point))
		{
			return btnControl.GetControlId();
		}
	}

	return INVALID_ID;
}

void CIoViewDuoGuTongLie::DrawTopButton()
{
	CClientDC dc(this);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	CRect rcWnd;
	GetClientRect(&rcWnd);
	rcWnd.bottom = rcWnd.top + 25;

	bmp.CreateCompatibleBitmap(&dc, rcWnd.Width(), rcWnd.Height());
	memDC.SelectObject(&bmp);
	memDC.FillSolidRect(0, 0, rcWnd.Width(), rcWnd.Height(), RGB(42,43,45));
	memDC.SetBkMode(TRANSPARENT);
	Gdiplus::Graphics graphics(memDC.GetSafeHdc());

	OnUpdateCmdUI(GetParentFrame(), FALSE);

	CRect rcPaint;
	dc.GetClipBox(&rcPaint);
	map<int, CNCButton>::iterator iter;
	CRect rcControl;

	for (iter=m_mapTopBtn.begin(); iter!=m_mapTopBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		btnControl.GetRect(rcControl);

		if(TOP_STOCK_NUMBER == btnControl.GetControlId())
		{
			btnControl.EnableButton(FALSE);
			btnControl.SetChangeTextColor(false);
		}

		// �жϵ�ǰ��ť�Ƿ���Ҫ�ػ�
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}

		btnControl.DrawButton(&graphics);
	}

	//
	dc.BitBlt(0, 0, rcWnd.Width(), rcWnd.Height(), &memDC, 0, 0, SRCCOPY);
	dc.SelectClipRgn(NULL);
	memDC.DeleteDC();
	bmp.DeleteObject();
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint

void CIoViewDuoGuTongLie::OnPaint()
{
	CPaintDC dc(this); // device context for painting	
	
	if ( GetParentGGTongViewDragFlag() || m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}
	
	//
	UnLockRedraw();

	if(m_iSubViewCount <= 0)
	{
		return;
	}
	CRect rcClient(0,0,0,0);
	GetClientRect(rcClient);
	CMemDC memdc(&dc, rcClient);
	// ���Ʒָ���
	COLORREF clrAxis = GetIoViewColor(ESCChartAxisLine);
	

	memdc.FillSolidRect(&rcClient,GetIoViewColor(ESCBackground) );		//	ͼ�����ķ�϶���ɫ
	

	DrawViewBorder(&memdc, m_rtSel, m_clrSelBorder);					//	���Ƶ�ǰ��ѡ�е�ͼ��߿�ɫ
}

void CIoViewDuoGuTongLie::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();

	// ���Ӵ��ڵ�ioview��ɫ�ı� TODO
	ChangeColorFont();
	Invalidate();
}

void CIoViewDuoGuTongLie::OnIoViewFontChanged()
{	
	CIoViewBase::OnIoViewFontChanged();

	// ���Ӵ��ڵ�ioview����ı� TODO
	ChangeColorFont();
	Invalidate();
}

void CIoViewDuoGuTongLie::LockRedraw()
{
	if ( !m_bLockRedraw )
	{
		if ( IsWindowVisible() )
		{
			SendMessage(WM_SETREDRAW, (WPARAM)FALSE, 0);
			CWnd *pChild = GetWindow(GW_CHILD);
			while ( NULL != pChild )
			{
				if ( pChild->IsWindowVisible() )
				{
					if ( pChild->IsKindOf(RUNTIME_CLASS(CIoViewBase)) )
					{
						(DYNAMIC_DOWNCAST(CIoViewBase, pChild))->LockRedraw();
					}
					pChild->SendMessage(WM_SETREDRAW, (WPARAM)FALSE, 0);
				}
				pChild = pChild->GetWindow(GW_HWNDNEXT);
			}
		}
		m_bLockRedraw = true;
	}
}

void CIoViewDuoGuTongLie::UnLockRedraw()
{
	if ( m_bLockRedraw )
	{
		if ( IsWindowVisible() )
		{
			SendMessage(WM_SETREDRAW, (WPARAM)TRUE, 0);
			Invalidate();
			CWnd *pChild = GetWindow(GW_CHILD);
			while ( NULL != pChild )
			{
				if ( pChild->IsWindowVisible() )
				{
					pChild->SendMessage(WM_SETREDRAW, (WPARAM)TRUE, 0);		// ����redraw���û�ʹhide��Ϊshow��so��Ҫע��
					if ( pChild->IsKindOf(RUNTIME_CLASS(CIoViewBase)) )
					{
						(DYNAMIC_DOWNCAST(CIoViewBase, pChild))->UnLockRedraw();
					}
				}
				
				pChild = pChild->GetWindow(GW_HWNDNEXT);
			}
		}
		m_bLockRedraw = false;
	}
}

void CIoViewDuoGuTongLie::CreateSubViews()
{
	if(m_DGTLType < EDGLT_KLINE || m_DGTLType > EDGLT_KLINE_TREND || m_iSubViewColCount <= 0)
	{
		return; 
	}

    switch(m_DGTLType)
	{
	case EDGLT_KLINE:                  // ֻ��K�ߵĶ��ͬ��
		{
			for (int32 i=0; i < m_iSubViewRowCount ; ++i )
			{
				for ( int32 j=0; j < m_iSubViewColCount ; ++j )
				{
					// ����ioview�̳е���static�ؼ������Բ���notify��־���ܻ�ȡ���㣬�����������ӿؼ�
					CIoViewKLine *pKline = new CIoViewKLine();
					pKline->SetCenterManager(m_pAbsCenterManager);
					pKline->m_bShowTopToolBar = FALSE;
					pKline->m_bShowIndexToolBar = FALSE;
					//pKline->m_iChartType = 6;
					pKline->m_bMoreView = true;
					pKline->Create(WS_CHILD, this, CRect(0,0,0,0), KIdIoViewKlineStart + m_iSubViewColCount*i + j);
					m_aIoViewKlines.Add(pKline);
				}
			}
		}
		break;

	case EDGLT_TREND:                  // ֻ�з�ʱ�Ķ��ͬ��
		{
			for (int32 k =0; k < m_iSubViewRowCount ; ++k )
			{
				for ( int32 h = 0; h < m_iSubViewColCount ; ++h )
				{
					// ����ioview�̳е���static�ؼ������Բ���notify��־���ܻ�ȡ���㣬�����������ӿؼ�
					CIoViewTrend *pTrend = new CIoViewTrend();
					pTrend->SetCenterManager(m_pAbsCenterManager);
					pTrend->m_bShowTopToolBar = FALSE;
					pTrend->m_bShowIndexToolBar = FALSE;
					//pTrend->m_iChartType = 7;
					pTrend->m_bMoreView = true;
					pTrend->Create(WS_CHILD, this, CRect(0,0,0,0), KIdIoViewTrendStart + m_iSubViewColCount*k + h);
					m_aIoViewTrends.Add(pTrend);
				}
			}
		}
		break;

	case EDGLT_KLINE_TREND:             // K�߷�ʱ�Ķ��ͬ�У�һ��ֻ�����У�������K�ߣ������Ƿ�ʱ
		{
			for ( int32 iCol=0; iCol < m_iSubViewColCount ; ++iCol )
			{
				// ����ioview�̳е���static�ؼ������Բ���notify��־���ܻ�ȡ���㣬�����������ӿؼ�
				CIoViewKLine *pKline = new CIoViewKLine();
				pKline->SetCenterManager(m_pAbsCenterManager);
				pKline->m_bShowTopToolBar = FALSE;
				pKline->m_bShowIndexToolBar = FALSE;
				//pKline->m_iChartType = 6;
				pKline->m_bMoreView = true;
				pKline->Create(WS_CHILD, this, CRect(0,0,0,0), KIdIoViewKlineStart + iCol);
				m_aIoViewKlines.Add(pKline);

				CIoViewTrend *pTrend = new CIoViewTrend();
				pTrend->SetCenterManager(m_pAbsCenterManager);
				pTrend->m_bShowTopToolBar = FALSE;
				pTrend->m_bShowIndexToolBar = FALSE;
				//pTrend->m_iChartType = 7;
				pTrend->m_bMoreView = true;
				pTrend->Create(WS_CHILD, this, CRect(0,0,0,0), KIdIoViewTrendStart + iCol);
				m_aIoViewTrends.Add(pTrend);
			}
		}
		break;

	default:
		break;
	}

	ChangeColorFont();

	ShowViews(m_DGTLType, true);

	SetActiveIoView(m_iActiveIoView);
	RecalcLayout();
	Invalidate();
}

int CIoViewDuoGuTongLie::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	int iRet = CIoViewBase::OnCreate(lpCreateStruct);
	if ( -1 == iRet )
	{
		return -1;
	}

	//
	InitialIoViewFace(this);
	//InitialTopBtns();

	return iRet;
}

void CIoViewDuoGuTongLie::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);
	
	RecalcLayout();
	RedrawWindow();	// ��������, �����Ӵ���ˢ��ռ��̫��ʱ�䵼���Լ���ʾ��ȫ
}

// ֪ͨ��ͼ�ı��ע����Ʒ
void CIoViewDuoGuTongLie::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch || NULL == pMerch)
		return;
	
	// �޸ĵ�ǰ�鿴����Ʒ
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
	
	// ��ǰ��Ʒ��Ϊ�յ�ʱ��
	if (NULL != pMerch)
	{
		// ���ù�ע����Ʒ��Ϣ
		RefreshView(pMerch);
	}
	else
	{
		// zhangbo 20090824 #�����䣬 ������Ʒ������ʱ����յ�ǰ��ʾ����
		//...
	}
	
	if ( NULL == m_pMerchBase )
	{
		// �������һ����Ʒ�л���
		SetTimer(KTimerIdInit, 50, NULL);
	}
}

//
void CIoViewDuoGuTongLie::OnVDataForceUpdate()
{
	RequestViewData();	// ������
}

void CIoViewDuoGuTongLie::OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch)
{
	
}

bool32 CIoViewDuoGuTongLie::FromXml(TiXmlElement * pElement)
{
	if (NULL == pElement)
		return false;

	// �ж��ǲ���IoView�Ľڵ�
	const char *pcValue = pElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;

	// �ж��ǲ��������Լ����ҵ����ͼ�Ľڵ�
	const char *pcAttrValue = pElement->Attribute(GetXmlElementAttrIoViewType());
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(CString(pcAttrValue)) != 0)	// ���������Լ���ҵ��ڵ�
		return false;
	
	// ��ȡ��ҵ����ͼ���е�����
	SetFontsFromXml(pElement);
	SetColorsFromXml(pElement);

	//
	int32 iMarketId			= -1;
	CString StrMerchCode	= L"";

	pcAttrValue = pElement->Attribute(GetXmlElementAttrMarketId());
	if (NULL != pcAttrValue)
	{
		iMarketId = atoi(pcAttrValue);
	}

	pcAttrValue = pElement->Attribute(GetXmlElementAttrMerchCode());
	if (NULL != pcAttrValue)
	{
		StrMerchCode = pcAttrValue;
	}

	pcAttrValue = pElement->Attribute(KStrViewSubViewType);
	if (NULL != pcAttrValue)
	{
		int iType = atoi(pcAttrValue);
		if(iType < EDGLT_KLINE)
		{
			iType = EDGLT_KLINE;
		}
		else if(iType > EDGLT_KLINE_TREND)
		{
			iType = EDGLT_KLINE_TREND;
		}

		m_DGTLType = (E_DuoGuTongLieType)iType;
	}

	pcAttrValue = pElement->Attribute(KStrViewIoViewRowCount);
	if (NULL != pcAttrValue)
	{
		int32 iRowCount = atoi(pcAttrValue);
		if(iRowCount < 1)
		{
			iRowCount = 1;
		}
		else if(iRowCount > 4)
		{
			iRowCount = 4;
		}
		m_iSubViewRowCount = iRowCount;
	}

	pcAttrValue = pElement->Attribute(KStrViewIoViewColCount);
	if (NULL != pcAttrValue)
	{
		int32 iColCount = atoi(pcAttrValue);
		if(iColCount < 1)
		{
			iColCount = 1;
		}
		else if(iColCount > 4)
		{
			iColCount = 4;
		}
		m_iSubViewColCount = iColCount;
	}

	if(m_DGTLType == EDGLT_KLINE_TREND)
	{
		m_iSubViewRowCount = 2;
	}

    m_iSubViewCount = m_iSubViewRowCount * m_iSubViewColCount;

	DeleteSubIoViews();
	CreateSubViews();
	// 
	CMerch *pMerchFound = NULL;
	if (!m_pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerchFound))
	{
		pMerchFound = NULL;
	}

	if(pMerchFound)
	{
		m_pFirstMerch = pMerchFound;
		SetBaseMerch(pMerchFound);

		// ��Ʒ�����ı�, ������Ʒ
		OnVDataMerchChanged(iMarketId, StrMerchCode, pMerchFound);	
	}	

	// ��������ͼ����ɫ����������
	ChangeColorFont();

	return true;
}

CString CIoViewDuoGuTongLie::ToXml()
{
	CString StrThis;

	CString StrMarketId;
	StrMarketId.Format(L"%d", m_MerchXml.m_iMarketId);

	int32 iViewType = (int)m_DGTLType; 
	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%d\" %s=\"%d\" %s=\"%d\" ",
		CString(GetXmlElementValue()), 
		CString(GetXmlElementAttrIoViewType()), 
		CIoViewManager::GetIoViewString(this),
		CString(GetXmlElementAttrMerchCode()),
		m_MerchXml.m_StrMerchCode,
		CString(GetXmlElementAttrMarketId()), 
		StrMarketId,
		KStrViewSubViewType,
        iViewType,
		KStrViewIoViewRowCount,
        m_iSubViewRowCount,
		KStrViewIoViewColCount,
		m_iSubViewColCount
		);
	//
	CString StrFace;
	StrFace  = SaveColorsToXml();
	StrFace += SaveFontsToXml();
	
	StrThis += StrFace;
	StrThis += L">\n";
	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

CString CIoViewDuoGuTongLie::GetDefaultXML()
{
	CString StrThis;

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" >\n", 
		CString(GetXmlElementValue()), 
		CString(GetXmlElementAttrIoViewType()), 
		CIoViewManager::GetIoViewString(this),
		CString(GetXmlElementAttrMerchCode()),
		L"",
		CString(GetXmlElementAttrMarketId()), 
		L"-1");

	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

void CIoViewDuoGuTongLie::OnIoViewActive()
{
	m_bActive = IsActiveInFrame();

	CIoViewBase *pActive = GetActiveIoView();
	if ( NULL != pActive )
	{
		pActive->SetActiveFlag(m_bActive);	// �����������־
		CIoViewChart *pChart = DYNAMIC_DOWNCAST(CIoViewChart, pActive);
		if ( NULL != pChart )
		{
			pChart->ReDrawAysnc();
		}
	}
	
	CIoViewBase::OnIoViewActive();

	if ( m_bActive )
	{
		RegisterHotKeyTarget(true) ;
	}

	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	Invalidate();

	SetFocus();	// �Լ���ȡ���������Ϣ
}

void CIoViewDuoGuTongLie::OnIoViewDeactive()
{
	// ��������ͼ��ȡ����, ��Ӧdeactive
	m_bActive = false;

	CIoViewBase *pActive = GetActiveIoView();
	if ( NULL != pActive )
	{
		pActive->SetActiveFlag(false);
	}
	
	RegisterHotKeyTarget(false);

	Invalidate();
}

void  CIoViewDuoGuTongLie::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;

	if (NULL != m_pMerchXml)
	{	
		StrTitle += L" ";
		StrTitle += m_pMerchXml->m_MerchInfo.m_StrMerchCnName;
	}
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

BOOL CIoViewDuoGuTongLie::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

void CIoViewDuoGuTongLie::RecalcLayout()
{
	CRect rcClient(0,0,0,0);
	GetClientRect(rcClient);
    if(m_iSubViewCount <= 0 || rcClient.Height() <= 0)
	{
		return;
	}

	// �����K�߷�ʱ����Ķ��ͬ�У�����������зֲ�
	if(EDGLT_KLINE_TREND == m_DGTLType && m_iSubViewRowCount != 2)
	{
		return;
	}

	//m_RectTopBar = rcClient;
	//m_RectTopBar.bottom = m_RectTopBar.top + KITopBtnHeight + 2 * KITopSkip;  // �����������ĸ߶ȣ���ť�߶� + �ϡ��¼�϶
  //  m_RectTopBar.bottom = m_RectTopBar.top;

	if (rcClient.Height()/2 != 0)
	{
		rcClient.bottom -= 1;	//	��֤��ż���߶�
	}

	if(rcClient.Width()/2 != 0)
	{
		rcClient.right -= 1;	//	��֤��ż�����
	}
	
	//	���㵥��ͼ��ƽ�ָ߶�,���.Ԥ���߾��ȣ�ͼ��֮��Ŀ��*2(�м��ͼ���ռһ����Ԫ���)
	//	��Ԫͼ��� = �ܸ� - 2 * ���ұ߾�� - �������� - 1��* 2 * ��Ԫ�߾�� / ����
	int32 iHeightPer = (rcClient.Height()  - 2*KiBorderWidth - ((m_iSubViewRowCount - 1) * 2 *KiBorderWidth))/m_iSubViewRowCount;
	int32 iWidthPer = (rcClient.Width() - 2*KiBorderWidth - (m_iSubViewColCount - 1) * 2 * KiBorderWidth)/m_iSubViewColCount;
	CRect rc(rcClient);	
	rc.bottom = rc.top;
	rc.right = rc.left;

	for ( int32 i=0; i < m_iSubViewRowCount ; ++i )
	{
		rc.top = rc.bottom + KiBorderWidth;	
		rc.bottom = rc.top + iHeightPer;	
		
		rc.right = rcClient.left;
		for ( int32 j=0; j < m_iSubViewColCount ; ++j )
		{
			rc.left = rc.right + KiBorderWidth;	//	��ʼλ��Ӧ��ռ��һ���߾��
			rc.right = rc.left + iWidthPer;					
			if(EDGLT_KLINE == m_DGTLType)
			{
				m_aIoViewKlines[i*m_iSubViewColCount+j]->MoveWindow(rc);
			}
			else if(EDGLT_TREND == m_DGTLType)
			{
				m_aIoViewTrends[i*m_iSubViewColCount+j]->MoveWindow(rc);
			}
			else if(EDGLT_KLINE_TREND == m_DGTLType)
			{
				if(0 == i)
				{
					m_aIoViewKlines[j]->MoveWindow(rc);
				}
				else if(1 == i)
				{
					m_aIoViewTrends[j]->MoveWindow(rc);
				}
			}
			rc.right += KiBorderWidth;	//	ĩβ��Ҫռ��һ���߾��
			
		}	
		rc.bottom += KiBorderWidth;	//	ĩβ��Ҫռ��һ���߾��
	}
	
}

CMerch * CIoViewDuoGuTongLie::GetMerchXml()
{
	CIoViewBase *pSubIoView = GetActiveIoView();
	if ( NULL != pSubIoView )
	{
		CMerch *pMerch = pSubIoView->GetMerchXml();
		if ( NULL != pMerch )
		{
			return pMerch;
		}
	}
	return CIoViewBase::GetMerchXml();
}

BOOL CIoViewDuoGuTongLie::PreTranslateMessage( MSG* pMsg )
{
	if ( WM_MOUSEWHEEL == pMsg->message )
	{
		short sDelt = (short)(HIWORD(pMsg->wParam));
		OnPageUpDown(sDelt/WHEEL_DELTA);
		return TRUE;
	}
	if (WM_MOUSEMOVE == pMsg->message)
	{
		//	��ѯ���������ͼ
		CWnd* pWnd = FromPoint(CPoint(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)));
		if (NULL != pWnd)
		{
			CRect rt;
			pWnd->GetWindowRect(&rt);
			ScreenToClient(&rt);
			rt.InflateRect(KiBorderWidth,KiBorderWidth,KiBorderWidth,KiBorderWidth);	//	�߿�λ������һ����Ԫ��

			if (rt != m_rtSel)
			{
				CClientDC dc(this);
				DrawViewBorder(&dc, m_rtSel, GetIoViewColor(ESCBackground));			//	�����ɵı߿�
				DrawViewBorder(&dc, rt, m_clrSelBorder);								//	�����µı߿�
				m_rtSel = rt;															//	��¼�µı߿�
			}	
		}

		// ע������뿪���¼���־
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd; 
		tme.dwFlags = TME_LEAVE ;
		tme.dwHoverTime = 100;

		m_bTracking = _TrackMouseEvent(&tme);		
		
	}
	if (WM_MOUSELEAVE == pMsg->message)
	{

		if (pMsg->hwnd == m_hWnd && !m_rtSel.IsRectEmpty())
		{
			CClientDC dc(this);
			DrawViewBorder(&dc, m_rtSel, GetIoViewColor(ESCBackground));			//	�����ɵı߿�
			m_rtSel.SetRectEmpty();
		}
	}
	else if ( WM_KEYDOWN == pMsg->message )
	{
		if ( VK_F5 == pMsg->wParam )
		{
			//ShowNextViews();
			return TRUE;
		}
		else if ( VK_RETURN == pMsg->wParam )
		{
			OnShowSubIoView();
			return TRUE;
		}
	}
	return CIoViewBase::PreTranslateMessage(pMsg);
}

void CIoViewDuoGuTongLie::ChangeColorFont()
{
	if(m_DGTLType < EDGLT_KLINE || m_DGTLType > EDGLT_KLINE_TREND)
	{
		return; 
	}

	switch(m_DGTLType)
	{
	case EDGLT_KLINE:                  // ֻ��K�ߵĶ��ͬ��
		{
			// ��������ͼ����ɫ����������
			for ( int32 i=0; i < m_aIoViewKlines.GetSize() ; ++i )
			{
				CIoViewKLine *pKline = m_aIoViewKlines[i];
				memcpyex(pKline->m_aIoViewColor,		m_aIoViewColor, sizeof(m_aIoViewColor));
				memcpyex(pKline->m_aIoViewFont, m_aIoViewFont, sizeof(m_aIoViewFont));
				pKline->ReDrawAysnc();
			}
		}
		break;

	case EDGLT_TREND:                  // ֻ�з�ʱ�Ķ��ͬ��
		{
			// ��������ͼ����ɫ����������
			for ( int32 i=0; i < m_aIoViewTrends.GetSize() ; ++i )
			{
				CIoViewTrend *pTrend = m_aIoViewTrends[i];
				memcpyex(pTrend->m_aIoViewColor, m_aIoViewColor, sizeof(m_aIoViewColor));
				memcpyex(pTrend->m_aIoViewFont, m_aIoViewFont, sizeof(m_aIoViewFont));	
				pTrend->ReDrawAysnc();
			}
		}
		break;

	case EDGLT_KLINE_TREND:             // K�߷�ʱ�Ķ��ͬ�У�һ��ֻ�����У�������K�ߣ������Ƿ�ʱ
		{
			// ��������ͼ����ɫ����������
			for ( int32 i=0; i < m_aIoViewKlines.GetSize() ; ++i )
			{
				CIoViewKLine *pKline = m_aIoViewKlines[i];
				CIoViewTrend *pTrend = m_aIoViewTrends[i];

				memcpyex(pKline->m_aIoViewColor,		m_aIoViewColor, sizeof(m_aIoViewColor));
				memcpyex(pTrend->m_aIoViewColor, m_aIoViewColor, sizeof(m_aIoViewColor));
				memcpyex(pKline->m_aIoViewFont, m_aIoViewFont, sizeof(m_aIoViewFont));
				memcpyex(pTrend->m_aIoViewFont, m_aIoViewFont, sizeof(m_aIoViewFont));	

				pKline->ReDrawAysnc();
				pTrend->ReDrawAysnc();
			}
		}
		break;

	default:
		break;
	}
}

CMerch * CIoViewDuoGuTongLie::GetNextMerch( CMerch* pMerchNow, bool32 bPre )
{
	return CIoViewBase::GetNextMerch(pMerchNow, bPre);
}

void CIoViewDuoGuTongLie::OnEscBackFrameMerch( CMerch *pMerch )
{
	CArray<CIoViewBase *, CIoViewBase *> aIoViews;
	GetShowIoViews(aIoViews);
	for ( int32 i=0; i < aIoViews.GetSize() ; ++i )
	{
		if ( pMerch == aIoViews[i]->GetMerchXml() )
		{
			SetActiveIoView(i);
			break;
		}
	}
}

void CIoViewDuoGuTongLie::GetShowIoViews( OUT CArray<CIoViewBase *, CIoViewBase *> &aIoViews )
{
	if ( EDGLT_KLINE == m_DGTLType )
	{
		for ( int32 i=0; i < m_aIoViewKlines.GetSize() ; ++i )
		{
			aIoViews.Add( m_aIoViewKlines[i] );
		}
	}
	else if(EDGLT_TREND == m_DGTLType)
	{
		for ( int32 i=0; i < m_aIoViewTrends.GetSize() ; ++i )
		{
			aIoViews.Add( m_aIoViewTrends[i] );
		}
	}
	else if(EDGLT_KLINE_TREND == m_DGTLType)
	{
		for ( int32 i=0; i < m_aIoViewKlines.GetSize() ; ++i )
		{
			aIoViews.Add( m_aIoViewKlines[i] );
		}

		for ( int32 i=0; i < m_aIoViewTrends.GetSize() ; ++i )
		{
			aIoViews.Add( m_aIoViewTrends[i] );
		}
	}
}

void CIoViewDuoGuTongLie::SetActiveIoView( CIoViewBase *pIoViewActive )
{
	CArray<CIoViewBase *, CIoViewBase *> aIoViews;
	GetShowIoViews(aIoViews);
	for ( int32 i=0; i < aIoViews.GetSize() ; ++i )
	{
		if ( pIoViewActive == aIoViews[i] )
		{
			SetActiveIoView(i);
			break;
		}
	}
}

void CIoViewDuoGuTongLie::SetActiveIoView( int32 iIndex )
{
	CArray<CIoViewBase *, CIoViewBase *> aIoViews;
	GetShowIoViews(aIoViews);
	if ( iIndex >=0 && iIndex < aIoViews.GetSize() )
	{
		for(int i = 0; i< aIoViews.GetSize(); i++)
		{
			aIoViews[i]->SetActiveFlag(false);

			CIoViewChart *pChart = DYNAMIC_DOWNCAST(CIoViewChart, aIoViews[i]);
			if ( NULL != pChart )
			{
				pChart->ReDrawAysnc();
			}
			else
			{
				aIoViews[i]->Invalidate();
			}
		}

		aIoViews[iIndex]->SetActiveFlag(IsActiveInFrame());
		m_iActiveIoView = iIndex;
		
		CIoViewChart *pChart = DYNAMIC_DOWNCAST(CIoViewChart, aIoViews[m_iActiveIoView]);
		if ( NULL != pChart )
		{
			pChart->ReDrawAysnc();
		}
		else
		{
			aIoViews[m_iActiveIoView]->Invalidate();
		}
	}
}

CIoViewBase* CIoViewDuoGuTongLie::GetActiveIoView()
{
	CArray<CIoViewBase *, CIoViewBase *> aIoViews;
	GetShowIoViews(aIoViews);
	if ( m_iActiveIoView >=0 && m_iActiveIoView < aIoViews.GetSize() )
	{
		return aIoViews[m_iActiveIoView];
	}
	return NULL;
}

// F5�л���ʱ��K�߶��ͬ�У����θù��ܣ���ʱע�͵�
void CIoViewDuoGuTongLie::ShowNextViews()
{
	//ASSERT( ID_PIC_KLINE == m_nShowPicId || ID_PIC_TREND == m_nShowPicId );

	return;
	if ( ID_PIC_TREND == m_nShowPicId )
	{
		ShowViews(ID_PIC_KLINE, true);
	}
	else
	{
		ShowViews(ID_PIC_TREND, true);
	}

	//OnUpdateCmdUI(GetParentFrame(), FALSE);
}

// û�еط����ã���ʱע�͵�
void CIoViewDuoGuTongLie::ShowViews( CIoViewBase *pIoViewSrc )
{
	return;

	//if ( NULL == pIoViewSrc )
	//{
	//	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	//	if ( NULL != pMainFrame )
	//	{
	//		pIoViewSrc = pMainFrame->FindActiveIoView();
	//	}
	//}

	//if ( NULL == pIoViewSrc )
	//{
	//	ShowViews(ID_PIC_TREND);
	//}
	//else if ( pIoViewSrc->IsKindOf(RUNTIME_CLASS(CIoViewDuoGuTongLie)) )
	//{
	//	CIoViewDuoGuTongLie *pDuoGu = (CIoViewDuoGuTongLie *)pIoViewSrc;
	//	if ( pDuoGu->m_nShowPicId == ID_PIC_TREND )
	//	{
	//		ShowViews(ID_PIC_KLINE);
	//	}
	//	else
	//	{
	//		ShowViews(ID_PIC_TREND);
	//	}
	//}
	//else if ( pIoViewSrc->IsKindOf(RUNTIME_CLASS(CIoViewKLine)) )
	//{
	//	ShowViews(ID_PIC_KLINE);
	//}
	//else
	//{
	//	ShowViews(ID_PIC_TREND);
	//}

	//if ( NULL != pIoViewSrc )
	//{
	//	CMerch *pMerch = pIoViewSrc->GetMerchXml();
	//	if ( NULL != pMerch )
	//	{
	//		m_pFirstMerch = pMerch;
	//		SetBaseMerch(pMerch);

	//		// �����K�ߣ���ҪǨ����ͼָ��&��һ����ͼ��ָ��
	//		CIoViewKLine *pKline = DYNAMIC_DOWNCAST(CIoViewKLine, pIoViewSrc);
	//		if ( NULL != pKline )
	//		{
	//			// �����бȽϸ��ӵ��л���������k��
	//			int32 i=0;
	//			ASSERT( ID_PIC_KLINE == m_nShowPicId );
	//			for ( i=0; i < m_aIoViewKlines.GetSize() ; ++i )
	//			{
	//				m_aIoViewKlines[i]->ShowWindow(SW_HIDE);
	//			}

	//			T_MerchNodeUserData *pMainDataSrc = pKline->m_MerchParamArray.GetSize() > 0 ? pKline->m_MerchParamArray[0] : NULL;
	//			if ( NULL != pMainDataSrc 
	//				&& m_aIoViewKlines.GetSize() > 0 
	//				&& m_aIoViewKlines[0]->m_MerchParamArray.GetSize() > 0 )
	//			{
	//				// �л�����
	//				if ( m_aIoViewKlines[0]->m_MerchParamArray[0]->m_eTimeIntervalFull != pMainDataSrc->m_eTimeIntervalFull )
	//				{
	//					E_NodeTimeInterval eNodeInterval = pMainDataSrc->m_eTimeIntervalFull;
	//					for ( int32 i=0; i < m_aIoViewKlines.GetSize() ; ++i )
	//					{
	//						T_MerchNodeUserData *pData = m_aIoViewKlines[i]->m_MerchParamArray.GetSize() > 0 ? m_aIoViewKlines[i]->m_MerchParamArray[0] : NULL;
	//						if ( NULL == pData )
	//						{
	//							continue;
	//						}
	//						pData->m_eTimeIntervalFull = eNodeInterval;
	//						if ( ENTIDayUser == eNodeInterval )
	//						{
	//							pData->m_iTimeUserMultipleDays = pMainDataSrc->m_iTimeUserMultipleDays;
	//						}
	//						else if ( ENTIMinuteUser == eNodeInterval )
	//						{
	//							pData->m_iTimeUserMultipleMinutes = pMainDataSrc->m_iTimeUserMultipleMinutes;
	//						}
	//						
	//						m_aIoViewKlines[i]->SetTimeInterval(*pData, eNodeInterval);
	//						m_aIoViewKlines[i]->SetCurveTitle(pData);
	//						m_aIoViewKlines[i]->RequestViewData();
	//					}
	//				}

	//				// �л�ָ��
	//				if ( pKline->m_bShowSaneIndexNow )
	//				{
	//					for ( int32 i=0; i < m_aIoViewKlines.GetSize() ; ++i )
	//					{
	//						m_aIoViewKlines[i]->m_bShowSaneIndexNow = pKline->m_bShowSaneIndexNow;
	//					}
	//				}
	//				if ( pKline->m_bShowTrendIndexNow )
	//				{
	//					for ( int32 i=0; i < m_aIoViewKlines.GetSize() ; ++i )
	//					{
	//						m_aIoViewKlines[i]->m_bShowTrendIndexNow = pKline->m_bShowTrendIndexNow;
	//					}
	//				}
	//				
	//				if ( NULL != pKline->m_pExpertTrade )
	//				{
	//					UINT nId = pKline->m_pExpertTrade->GetTradeType() + IDM_IOVIEWKLINE_EXPERT_BEGIN;
	//					for ( int32 i=0; i < m_aIoViewKlines.GetSize() ; ++i )
	//					{
	//						m_aIoViewKlines[i]->OnMenuExpertTrade(nId);
	//					}
	//				}
	//				
	//				if ( NULL != pKline->m_pFormularClrKLine )
	//				{
	//					for ( int32 i=0; i < m_aIoViewKlines.GetSize() ; ++i )
	//					{
	//						m_aIoViewKlines[i]->DoAddClrKLine(pKline->m_pFormularClrKLine);	// �������ָ��
	//					}
	//				}
	//				
	//				// ��ʵ���ڵ�ָ��
	//				{
	//					bool32 bHasSub = pKline->m_SubRegions.GetSize() > 0;
	//					bool32 bAddSubIndex = true;
	//					for ( int32 i = pMainDataSrc->aIndexs.GetSize()-1 ; i >= 0 ; i-- )
	//					{
	//						T_IndexParam *pParam = pMainDataSrc->aIndexs[i];
	//						if ( NULL == pParam )
	//						{
	//							continue;
	//						}
	//						if ( pParam->pRegion == pKline->m_pRegionMain )
	//						{
	//							// ��ͼ�ϵ�ָ��
	//							for ( int32 j=0; j < m_aIoViewKlines.GetSize() ; ++j )
	//							{
	//								m_aIoViewKlines[j]->AddShowIndex(pParam->strIndexName);
	//							}
	//						}
	//						else if ( bHasSub && pParam->pRegion == pKline->m_SubRegions[0].m_pSubRegionMain )
	//						{
	//							// ��һ����ͼ
	//							for ( int32 j=0; j < m_aIoViewKlines.GetSize() ; ++j )
	//							{
	//								m_aIoViewKlines[j]->AddShowIndex(pParam->strIndexName);
	//							}
	//							bAddSubIndex = false;
	//						}
	//					}
	//				} // ��ʵָ��
	//			}//if ( NULL != pMainDataSrc )

	//			for ( i=0; i < m_aIoViewKlines.GetSize() ; ++i )
	//			{
	//				m_aIoViewKlines[i]->ShowWindow(SW_SHOW);	// ��ʾ��
	//			}
	//		}//if ( NULL != pKline )
	//	}
	//}
}

void CIoViewDuoGuTongLie::ShowViews( int iPicId, bool32 bChangeMerchByOldShow/* = false*/ )
{
	if(iPicId < EDGLT_KLINE || iPicId > EDGLT_KLINE_TREND)
	{
		return; 
	}

	int32 iSize = m_iSubViewCount;
	if(iSize <= 0) // K�ߺͷ�ʱͼ�ĸ���һ��
	{
		return;
	}

	MerchArray aMerchsInShow;
	aMerchsInShow.SetSize(iSize);
	CIoViewBase* pIoViewMerchSrc = m_pIoViewMerchChangeSrc;
	if ( NULL == pIoViewMerchSrc )
	{
		pIoViewMerchSrc = this;
	}

	CMerch *pMerchNext = NULL;
	if(m_pFirstMerch)
	{
		pMerchNext = m_pFirstMerch;
	}
	else
	{
		pMerchNext = m_pMerchXml;
	}

	for (int i=0; i < iSize ; ++i )
	{
		aMerchsInShow[i] = pMerchNext;
		pMerchNext = pIoViewMerchSrc->GetNextMerch(pMerchNext, false);	// ��һ����Ʒ������
	}

	if ( EDGLT_KLINE == iPicId )
	{
		m_DGTLType = EDGLT_KLINE;
		int32 i=0;

		for ( i=0; i < m_aIoViewTrends.GetSize() ; ++i )
		{
			m_aIoViewTrends[i]->ShowWindow(SW_HIDE);
		}

		for ( i=0; i < aMerchsInShow.GetSize() && bChangeMerchByOldShow ; ++i )
		{
			if ( NULL != aMerchsInShow[i] )
			{
				CMerch *pMerch = aMerchsInShow[i];
				m_aIoViewKlines[i]->OnVDataMerchChanged(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, pMerch);
				m_aIoViewKlines[i]->OnVDataForceUpdate();
			}
		}
		for ( i=0; i < m_aIoViewKlines.GetSize() ; ++i )
		{
			m_aIoViewKlines[i]->ShowWindow(SW_SHOW);
		}
	}
	else if(EDGLT_TREND == iPicId)
	{
		m_DGTLType = EDGLT_TREND;
		int32 i=0;


		for ( i=0; i < m_aIoViewKlines.GetSize() ; ++i )
		{
			m_aIoViewKlines[i]->ShowWindow(SW_HIDE);
		}

		for ( i=0; i < aMerchsInShow.GetSize() && bChangeMerchByOldShow ; ++i )
		{
			if ( NULL != aMerchsInShow[i] )
			{
				CMerch *pMerch = aMerchsInShow[i];
				m_aIoViewTrends[i]->OnVDataMerchChanged(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, pMerch);
				m_aIoViewTrends[i]->OnVDataForceUpdate();
			}
		}
		for ( i=0; i < m_aIoViewTrends.GetSize() ; ++i )
		{
			m_aIoViewTrends[i]->ShowWindow(SW_SHOW);
		}
	}
	else if(EDGLT_KLINE_TREND == iPicId)
	{
		m_DGTLType = EDGLT_KLINE_TREND;
		int32 i=0;

		for ( i=0; i < m_aIoViewKlines.GetSize() && bChangeMerchByOldShow ; ++i )
		{
			if ( NULL != aMerchsInShow[i] )
			{
				CMerch *pMerch = aMerchsInShow[i];
				m_aIoViewKlines[i]->OnVDataMerchChanged(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, pMerch);
				m_aIoViewKlines[i]->OnVDataForceUpdate();

				m_aIoViewTrends[i]->OnVDataMerchChanged(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, pMerch);
				m_aIoViewTrends[i]->OnVDataForceUpdate();
			}
		}

		for (int32  k =0; k < m_aIoViewTrends.GetSize() ; ++k )
		{
			m_aIoViewKlines[k]->ShowWindow(SW_SHOW);
			m_aIoViewTrends[k]->ShowWindow(SW_SHOW);
		}
	}

	SetActiveIoView(m_iActiveIoView);
	SetTimer(KTimerIdDoShowAppend, 10, NULL);
}

void CIoViewDuoGuTongLie::OnContextMenu( CWnd* pWnd, CPoint pos )
{
	
}

void CIoViewDuoGuTongLie::OnDestroy()
{
	RegisterHotKeyTarget(false);

	DeleteSubIoViews();

	CIoViewBase::OnDestroy();
}

void CIoViewDuoGuTongLie::DeleteSubIoViews()
{
	int32 i=0;
	for ( i=0; i < m_aIoViewKlines.GetSize() ; ++i )
	{
		m_aIoViewKlines[i]->DestroyWindow();
		delete m_aIoViewKlines[i];
	}
	m_aIoViewKlines.RemoveAll();
	
	for ( i=0; i < m_aIoViewTrends.GetSize() ; ++i )
	{
		m_aIoViewTrends[i]->DestroyWindow();
		delete m_aIoViewTrends[i];
	}
	m_aIoViewTrends.RemoveAll();
}

void CIoViewDuoGuTongLie::OnLButtonDown( UINT nFlags, CPoint point )
{
	// Top��ť������ʱע�͵�
	//int32 iButton = TTopButtonHitTest(point);
	//if (INVALID_ID != iButton)
	//{	
	//	bool32 bCheck = m_mapTopBtn[iButton].GetCheck();
	//	if (!bCheck)
	//	{
	//		m_mapTopBtn[iButton].SetCheck(TRUE);
	//	}

	//	map<int, CNCButton>::iterator iter;
	//	for (iter=m_mapTopBtn.begin(); iter!=m_mapTopBtn.end(); ++iter)
	//	{
	//		CNCButton &btn = iter->second;
	//		if (iButton != iter->first && btn.GetCheck())
	//		{
	//			btn.SetCheckStatus(FALSE);
	//		}
	//	}
	//}

	CArray<CIoViewBase *, CIoViewBase *> aIoViews;
	GetShowIoViews(aIoViews);
	CPoint ptScreen(point);
	ClientToScreen(&ptScreen);
	for ( int32 i=0; i < aIoViews.GetSize() ; ++i )
	{
		CRect rc;
		aIoViews[i]->GetWindowRect(rc);
		if ( rc.PtInRect(ptScreen) )
		{
			if ( i != m_iActiveIoView )
			{
				SetActiveIoView(i);
			}
			break;
		}
	}

	CIoViewBase::OnLButtonDown(nFlags, point);
}

LRESULT CIoViewDuoGuTongLie::OnMouseLeave( WPARAM wParam,LPARAM lParam )
{
	//m_bTracking = false;

	//if (INVALID_ID != m_iTopBtnHovering)
	//{
	//	m_mapTopBtn[m_iTopBtnHovering].MouseLeave();
	//	m_iTopBtnHovering = INVALID_ID;
	//}

	return 0;
}

void CIoViewDuoGuTongLie::OnLButtonUp( UINT nFlags, CPoint point )
{
	// Top��ť����
	//int32 iButton = TTopButtonHitTest(point);
	//if (INVALID_ID != iButton)
	//{
	//	m_mapTopBtn[iButton].LButtonUp();
	//}
}

void CIoViewDuoGuTongLie::OnMouseMove( UINT nFlags, CPoint point )
{
	int i =0;
	++i;
	// top
	//int iButton = TTopButtonHitTest(point);
	//if (iButton != m_iTopBtnHovering)
	//{
	//	if (INVALID_ID != m_iTopBtnHovering)
	//	{
	//		m_mapTopBtn[m_iTopBtnHovering].MouseLeave();
	//		m_iTopBtnHovering = INVALID_ID;
	//	}

	//	if (INVALID_ID != iButton)
	//	{	
	//		m_iTopBtnHovering = iButton;
	//		m_mapTopBtn[m_iTopBtnHovering].MouseHover();
	//	}
	//}

	////
	//if ( !m_bTracking )
	//{
	//	// ע������뿪���¼���־
	//	TRACKMOUSEEVENT tme;
	//	tme.cbSize = sizeof(tme);
	//	tme.hwndTrack = m_hWnd; 
	//	tme.dwFlags = TME_LEAVE ;
	//	tme.dwHoverTime = 1;

	//	m_bTracking = _TrackMouseEvent(&tme);		
	//}	
}

LRESULT CIoViewDuoGuTongLie::OnHotKeyMsg( WPARAM w, LPARAM l )
{
	CHotKey *pHotKey = (CHotKey *)w;
	if (NULL == pHotKey)
	{
		ASSERT(0);
		return 0;
	}

	bool32 bIndexHotKey = pHotKey->m_eHotKeyType == EHKTIndex;
	bIndexHotKey = bIndexHotKey && EDGLT_KLINE == m_DGTLType;

	if ( (pHotKey->m_eHotKeyType != EHKTMerch
		&& !bIndexHotKey) )
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), UM_HOTKEY, w, l);	// �ǹ�ע��Ϣ����mainframe����
		return 0;
	}
	
	CHotKey HotKey = *pHotKey;
	DEL(pHotKey);
	pHotKey = &HotKey;
	RegisterHotKeyTarget(true);	// �ٴ�ע��
	
	if (EHKTMerch == pHotKey->m_eHotKeyType)
	{
		CString StrHotKey ,StrName, StrMerchCode;
		int32   iMarketId;
		StrHotKey		= pHotKey->m_StrKey;
		StrMerchCode	= pHotKey->m_StrParam1;
		StrName			= pHotKey->m_StrSummary;
		iMarketId		= pHotKey->m_iParam1;
		
		CMerch* pMerch = NULL;

		if ( m_pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerch) )
		{
			if ( NULL == pMerch )
			{
				return 0;
			}
			
			CIoViewBase *pActiveIoView = GetActiveIoView();
			if ( NULL != pActiveIoView )
			{
				pActiveIoView->OnVDataMerchChanged(iMarketId, StrMerchCode, pMerch);
				pActiveIoView->OnVDataForceUpdate();
			}
		}
	}
	else if ( EHKTIndex == pHotKey->m_eHotKeyType )
	{
		//ASSERT( ID_PIC_KLINE == m_nShowPicId );
		// ����k����ͼ�л�ָ��
		// �˴�Ӧ����Ȩ�޿���
		if ( !CPluginFuncRight::Instance().IsUserHasIndexRight(pHotKey->m_StrParam1, true) )
		{
			return 0;
		}

		for ( int32 i=0; i < m_aIoViewKlines.GetSize() ; ++i )
		{
			m_aIoViewKlines[i]->AddShowIndex(pHotKey->m_StrParam1, true, false);	// ɾ�����ڵ�ָ�꣬���л���Ʒ
		}
	}

	return 1;
}

void CIoViewDuoGuTongLie::SetBaseMerch( CMerch *pMerch )
{
	if ( NULL == pMerch )
	{
		return;
	}

	{
		// �л�������Ʒ
		m_pMerchBase = pMerch;
		int32 iActiveNew = m_iActiveIoView;
		if ( NULL != m_hWnd )
		{
			CArray<CIoViewBase *, CIoViewBase *> aIoViews;
			GetShowIoViews(aIoViews);
			if ( aIoViews.GetSize() > 0 )
			{
				// 1. �������Ŀ����ṩ��Ʒ˳�����ͼ����ȡ��ͼ����Ʒ˳��
				//    ������ͼռ��һ�����ӣ������һ������
				//    �Ժ�ÿ���¹������һ�����ӵ�Ϊ׼���Ϲ����Ե�һ�����ӵ�Ϊ׼?
				// 2. ���ǲ�ȡ�����г��е�˳��������ʾ�̶���Ʒ�أ�
				// 1.

				// �����ǰ������Ʒ������ע����Ʒ�У���ѡ�����Ĺ�ע��Ʒ��Ϊ���ڵ���Ʒ�б�
				int32 i=0;
				bool32 bFound = false;
				if ( m_iActiveIoView >= 0 && m_iActiveIoView < aIoViews.GetSize() )
				{
					if ( aIoViews[m_iActiveIoView]->GetMerchXml() == m_pMerchBase )
					{
						bFound = true;
					}
				}
				for ( i=0; i < aIoViews.GetSize() && !bFound ; ++i )
				{
					if ( aIoViews[i]->GetMerchXml() == m_pMerchBase )
					{
						bFound = true;
						iActiveNew = i;
						break;
					}
				}

				if ( !bFound )
				{
					CIoViewBase* pIoViewMerchSrc = m_pIoViewMerchChangeSrc;
					if ( NULL == pIoViewMerchSrc )
					{
						pIoViewMerchSrc = this;
					}
					CMerch *pMerchNext = pMerch;
					iActiveNew = 0;	// �����һ��
					if(EDGLT_KLINE == m_DGTLType || EDGLT_TREND == m_DGTLType)
					{
						for ( int32 i=0; i < aIoViews.GetSize() ; ++i )
						{
							aIoViews[i]->OnVDataMerchChanged(pMerchNext->m_MerchInfo.m_iMarketId, pMerchNext->m_MerchInfo.m_StrMerchCode, pMerchNext);
							aIoViews[i]->OnVDataForceUpdate();

							pMerchNext = pIoViewMerchSrc->GetNextMerch(pMerchNext, false);
							if ( NULL == pMerchNext )
							{
								pMerchNext = pMerch;
							}
						}
					}
					else if(EDGLT_KLINE_TREND == m_DGTLType)
					{
						int iHalfSize = aIoViews.GetSize() / 2;
						for ( int32 j = 0; j <  iHalfSize; ++j )
						{
							aIoViews[j]->OnVDataMerchChanged(pMerchNext->m_MerchInfo.m_iMarketId, pMerchNext->m_MerchInfo.m_StrMerchCode, pMerchNext);
							aIoViews[j]->OnVDataForceUpdate();

							aIoViews[j + iHalfSize]->OnVDataMerchChanged(pMerchNext->m_MerchInfo.m_iMarketId, pMerchNext->m_MerchInfo.m_StrMerchCode, pMerchNext);
							aIoViews[j + iHalfSize]->OnVDataForceUpdate();

							pMerchNext = pIoViewMerchSrc->GetNextMerch(pMerchNext, false);
							if ( NULL == pMerchNext )
							{
								pMerchNext = pMerch;
							}
						}
					}

				}//if ( !bFound )
			}
		}
		//SetActiveIoView(iActiveNew);	// �������
	}
}

void CIoViewDuoGuTongLie::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	switch (nChar)
	{
	case VK_UP:
		{
			if ( EDGLT_KLINE == m_DGTLType )
			{
				// K�߷���
				for ( int32 i=0; i < m_aIoViewKlines.GetSize() ; ++i )
				{
					m_aIoViewKlines[i]->OnKeyUp();
				}
			}
			else if ( 0 )	// ������k�ߣ���ʱ�����������û���k����ϰ���԰�
			{
				// ��ʱ�ƶ��۽�, ���ƶ�����һ��
				//if ( m_aIoViewTrends.GetSize() > 0 )
				//{
				//	int32 iActive = m_iActiveIoView;
				//	int32 iRowOld = iActive/m_iSubViewRowCount;
				//	int32 iColOld = iActive%m_iSubViewColCount;
				//	if ( iRowOld == 0 )
				//	{
				//		iColOld = GetLeftColumn(iColOld);
				//	}
				//	iRowOld = GetUpRow(iRowOld);
				//	iActive = iRowOld*m_iSubViewRowCount  + iColOld;
				//	SetActiveIoView(iActive);
				//}
			}
		}
		break;
	case VK_DOWN:
		{
			if ( EDGLT_KLINE == m_DGTLType )
			{
				// K�߷���
				for ( int32 i=0; i < m_aIoViewKlines.GetSize() ; ++i )
				{
					m_aIoViewKlines[i]->OnKeyDown();
				}
			}
			else if ( 0 )	// ������k�ߣ���ʱ�����������û���k����ϰ���԰�
			{
				// ��ʱ�ƶ��۽�, ���ƶ�����һ��
				//if ( m_aIoViewTrends.GetSize() > 0 )
				//{
				//	int32 iActive = m_iActiveIoView;
				//	int32 iRowOld = iActive/m_iSubViewRowCount;
				//	int32 iColOld = iActive%m_iSubViewColCount;
				//	if ( iRowOld == m_iSubViewRowCount-1 )
				//	{
				//		iColOld = GetRightColumn(iColOld);
				//	}
				//	iRowOld = GetDownRow(iRowOld);
				//	iActive = iRowOld*m_iSubViewRowCount  + iColOld;
				//	SetActiveIoView(iActive);
				//}
			}
		}
		break;
	case VK_LEFT:
		{
			// �ƶ��۽�, �����ƶ�������һ��
			if ( m_aIoViewTrends.GetSize() > 0 )
			{
				int32 iActive = m_iActiveIoView;
				int32 iRowOld = iActive/m_iSubViewRowCount;
				int32 iColOld = iActive%m_iSubViewColCount;
				if ( iColOld == 0 )
				{
					iRowOld = GetUpRow(iRowOld);
				}
				iColOld = GetLeftColumn(iColOld);
				iActive = iRowOld*m_iSubViewRowCount  + iColOld;
				SetActiveIoView(iActive);
			}
		}
		break;
	case VK_RIGHT:
		{
			// �ƶ��۽�
			if ( m_aIoViewTrends.GetSize() > 0 )
			{
				int32 iActive = m_iActiveIoView;
				int32 iRowOld = iActive/m_iSubViewRowCount;
				int32 iColOld = iActive%m_iSubViewColCount;
				if ( iColOld == m_iSubViewColCount-1 )
				{
					iRowOld = GetDownRow(iRowOld);
				}
				iColOld = GetRightColumn(iColOld);
				iActive = iRowOld*m_iSubViewRowCount  + iColOld;
				SetActiveIoView(iActive);
			}
		}
		break;
	case VK_PRIOR:
		{
			// �Ϸ�
			OnPageUpDown(1);
		}
		break;
	case VK_NEXT:
		{
			// �·�
			OnPageUpDown(-1);
		}
		break;
	default:
		break;
	}
	CIoViewBase::OnKeyDown(nChar, nRepCnt, nFlags);
}

int32 CIoViewDuoGuTongLie::GetUpRow( int32 iOldRow )
{
	return (iOldRow-1 + m_iSubViewRowCount)%m_iSubViewRowCount;
}

int32 CIoViewDuoGuTongLie::GetDownRow( int32 iOldRow )
{
	return (iOldRow +1)%m_iSubViewRowCount;
}

int32 CIoViewDuoGuTongLie::GetLeftColumn( int32 iOldCol )
{
	return (iOldCol-1 + m_iSubViewColCount)%m_iSubViewColCount;
}

int32 CIoViewDuoGuTongLie::GetRightColumn( int32 iOldCol )
{
	return (iOldCol+1)%m_iSubViewColCount;
}

void CIoViewDuoGuTongLie::OnPageUpDown( int32 iPage )
{
	CArray<CIoViewBase *, CIoViewBase *> aIoViews;
	GetShowIoViews(aIoViews);
	if ( iPage == 0 || aIoViews.GetSize() <= 0 )
	{
		return;
	}
	CIoViewBase* pIoViewMerchSrc = m_pIoViewMerchChangeSrc;
	if ( NULL == pIoViewMerchSrc )
	{
		pIoViewMerchSrc = this;
	}

	if ( iPage > 0 )
	{
		const int32 iMerchsPerPage = m_iSubViewColCount*m_iSubViewRowCount;
		const int32 iMerchCount = iPage*iMerchsPerPage;
		// �Ϸ����ӵ�һ����Ʒ��ʼ����Ҫ����������, �����ĵ�һ����ƷΪ��ʼ��Ʒ
		//ASSERT( iMerchsPerPage == aIoViews.GetSize() );
		CMerch *pMerchNext = aIoViews[0]->GetMerchXml();
		int32 i = 0;
		for ( i=0; i < iMerchCount ; ++i )
		{
			pMerchNext = pIoViewMerchSrc->GetNextMerch(pMerchNext, true);
		}

		if ( NULL == pMerchNext )
		{
			return;
		}

		m_pFirstMerch = pMerchNext;

		if(EDGLT_KLINE == m_DGTLType || EDGLT_TREND == m_DGTLType)
		{
			for ( i=0; i < iMerchsPerPage ; ++i )
			{
				aIoViews[i]->OnVDataMerchChanged(pMerchNext->m_MerchInfo.m_iMarketId, pMerchNext->m_MerchInfo.m_StrMerchCode, pMerchNext);
				aIoViews[i]->OnVDataForceUpdate();

				pMerchNext = pIoViewMerchSrc->GetNextMerch(pMerchNext, false);	// ��һ����Ʒ������
			}
		}
		else if(EDGLT_KLINE_TREND == m_DGTLType)
		{
			for ( i=0; i < iMerchsPerPage / 2 ; ++i )
			{
				aIoViews[i]->OnVDataMerchChanged(pMerchNext->m_MerchInfo.m_iMarketId, pMerchNext->m_MerchInfo.m_StrMerchCode, pMerchNext);
				aIoViews[i]->OnVDataForceUpdate();

				aIoViews[i + iMerchsPerPage / 2]->OnVDataMerchChanged(pMerchNext->m_MerchInfo.m_iMarketId, pMerchNext->m_MerchInfo.m_StrMerchCode, pMerchNext);
				aIoViews[i + iMerchsPerPage / 2]->OnVDataForceUpdate();

				pMerchNext = pIoViewMerchSrc->GetNextMerch(pMerchNext, false);	// ��һ����Ʒ������
			}
		}

	}
	else if ( iPage < 0 )
	{
		const int32 iMerchsPerPage = m_iSubViewColCount*m_iSubViewRowCount;
		const int32 iMerchCount = (-iPage)*iMerchsPerPage;
		// �·��������һ����Ʒ��ʼ����Ҫ����������, �����ĵ�һ����ƷΪ������Ʒ
		//ASSERT( iMerchsPerPage == aIoViews.GetSize() );
		CMerch *pMerchNext = aIoViews[iMerchsPerPage-1]->GetMerchXml();
		int32 i = 0;
		for ( i=0; i < iMerchCount ; ++i )
		{
			pMerchNext = pIoViewMerchSrc->GetNextMerch(pMerchNext, false);
		}
		
		if ( NULL == pMerchNext )
		{
			return;
		}
		
		if(EDGLT_KLINE == m_DGTLType || EDGLT_TREND == m_DGTLType)
		{
			for ( i=iMerchsPerPage-1; i >= 0 ; --i )
			{
				aIoViews[i]->OnVDataMerchChanged(pMerchNext->m_MerchInfo.m_iMarketId, pMerchNext->m_MerchInfo.m_StrMerchCode, pMerchNext);
				aIoViews[i]->OnVDataForceUpdate();

				if(i == 0)
				{
					m_pFirstMerch = pMerchNext;
				}

				pMerchNext = pIoViewMerchSrc->GetNextMerch(pMerchNext, true);	// ���һ����Ʒ������
			}
		}
		else if(EDGLT_KLINE_TREND == m_DGTLType)
		{
			for ( i=iMerchsPerPage-1; i >= iMerchsPerPage / 2 ; --i )
			{
				aIoViews[i]->OnVDataMerchChanged(pMerchNext->m_MerchInfo.m_iMarketId, pMerchNext->m_MerchInfo.m_StrMerchCode, pMerchNext);
				aIoViews[i]->OnVDataForceUpdate();

				aIoViews[i - iMerchsPerPage / 2]->OnVDataMerchChanged(pMerchNext->m_MerchInfo.m_iMarketId, pMerchNext->m_MerchInfo.m_StrMerchCode, pMerchNext);
				aIoViews[i - iMerchsPerPage / 2]->OnVDataForceUpdate();

				if(i == 0)
				{
					m_pFirstMerch = pMerchNext;
				}

				pMerchNext = pIoViewMerchSrc->GetNextMerch(pMerchNext, true);	// ���һ����Ʒ������
			}
		}	
	}
}

void CIoViewDuoGuTongLie::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	OnShowSubIoView();

	CIoViewBase::OnLButtonDblClk(nFlags, point);
}

void CIoViewDuoGuTongLie::OnTimer( UINT nIDEvent )
{
	if ( KTimerIdInit == nIDEvent )
	{
		KillTimer(nIDEvent);
		if ( NULL == m_pMerchBase )
		{
			CMerch *pMerch = GetMerchXml();
			if ( NULL != pMerch )
			{
				SetBaseMerch(pMerch);
			}
		}
	}
	else if ( KTimerIdDoShowAppend == nIDEvent )
	{
		KillTimer(nIDEvent);
		if ( NULL != m_pMerchBase )
		{
			if ( EDGLT_KLINE == m_DGTLType )
			{
				RefreshIndex();
			}
		}
		
	}
	CIoViewBase::OnTimer(nIDEvent);
}

void CIoViewDuoGuTongLie::OnShowSubIoView()
{
	CIoViewBase *pActiveIoView = GetActiveIoView();
	if ( NULL != pActiveIoView )
	{
		CMerch *pMerch = pActiveIoView->GetMerchXml();
		CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		if ( NULL != pMerch && NULL != pMainFrame )
		{
			if ( pActiveIoView->IsKindOf(RUNTIME_CLASS(CIoViewKLine)))
			{
				pMainFrame->OnShowMerchInChart(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, ID_PIC_KLINE);
				// Բ����ɻ���Ҫ���Ŀ������ڣ�ָ������� δʵ��
				// �����Ŀ�����ͼ����ͼʵ�ʴ��ڵ�ָ�꣬�����Ĳ�����
				CIoViewKLine *pKlineSrc = DYNAMIC_DOWNCAST(CIoViewKLine, pActiveIoView);
				CIoViewKLine *pKlineDst = DYNAMIC_DOWNCAST(CIoViewKLine, pMainFrame->FindActiveIoView());
				if ( NULL != pKlineSrc && NULL != pKlineDst && pKlineSrc != pKlineDst )
				{
					// �������
					T_MerchNodeUserData *pMainDataSrc = pKlineSrc->m_MerchParamArray.GetSize() > 0 ? pKlineSrc->m_MerchParamArray[0] : NULL;
					T_MerchNodeUserData *pMainDataDst = pKlineDst->m_MerchParamArray.GetSize() > 0 ? pKlineDst->m_MerchParamArray[0] : NULL;
					if ( NULL != pMainDataSrc 
						&& NULL != pMainDataDst
						)
					{
						// �л�����
						if ( pMainDataDst->m_eTimeIntervalFull != pMainDataSrc->m_eTimeIntervalFull )
						{
							E_NodeTimeInterval eNodeInterval = pMainDataSrc->m_eTimeIntervalFull;
							
							pMainDataDst->m_eTimeIntervalFull = eNodeInterval;
							if ( ENTIDayUser == eNodeInterval )
							{
								pMainDataDst->m_iTimeUserMultipleDays = pMainDataSrc->m_iTimeUserMultipleDays;
							}
							else if ( ENTIMinuteUser == eNodeInterval )
							{
								pMainDataDst->m_iTimeUserMultipleMinutes = pMainDataSrc->m_iTimeUserMultipleMinutes;
							}
							pKlineDst->SetTimeInterval(*pMainDataDst, eNodeInterval);
							pKlineDst->SetCurveTitle(pMainDataDst);
							pKlineDst->RequestViewData();
							pKlineDst->ReDrawAysnc();
						}
					}

					// ���ָ��
					CStringArray aIndexNames;
					pKlineSrc->GetCurrentIndexNameArray(aIndexNames);
					for ( int32 i=0; i < aIndexNames.GetSize() ; ++i )
					{
						pKlineDst->AddShowIndex(aIndexNames[i]);
					}
				}
			}
			else if(pActiveIoView->IsKindOf(RUNTIME_CLASS(CIoViewTrend)))
			{
				pMainFrame->OnShowMerchInChart(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, ID_PIC_TREND);
			}
		}
	}
}

void CIoViewDuoGuTongLie::AddKlineIndex( const CString &StrIndexName )
{
	
}

// ���¶�����ť��״̬����ʱע�͵�
void CIoViewDuoGuTongLie::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	return;
	if ( NULL != pTarget && GetCapture() != this )		// ��갴�²�����
	{
		// K�ߵĴ��� 
		{
			map<int, CNCButton>::iterator iter;
			for (iter=m_mapTopBtn.begin(); iter!=m_mapTopBtn.end(); ++iter)
			{
				CNCButton &btnControl = iter->second;
				int32 ID = btnControl.GetControlId();
				if ( TOP_CYCLE_DAY <= ID && ID <= TOP_CYCLE_YEAR)
				{
					btnControl.SetCheckStatus(ID == m_uCurTimeInterval && m_nShowPicId == ID_PIC_KLINE);
				}
			}
		}

	    // ��ʱ�Ĵ���
		{
			map<int, CNCButton>::iterator iter;
			for (iter=m_mapTopBtn.begin(); iter!=m_mapTopBtn.end(); ++iter)
			{
				CNCButton &btnControl = iter->second;
				int32 ID = btnControl.GetControlId();

				if ( TOP_CYCLE_TREND == ID)
				{
					btnControl.SetCheckStatus(m_nShowPicId == ID_PIC_TREND);
					break;
				}
			}
		}

		map<int, CNCButton>::iterator iter;
		for (iter=m_mapTopBtn.begin(); iter!=m_mapTopBtn.end(); ++iter)
		{
			CNCButton &btnControl = iter->second;
			int32 iBtnID = btnControl.GetControlId();

			if ( TOP_STOCK_THREE <= iBtnID && TOP_STOCK_FIFTEEN >= iBtnID)
			{
				btnControl.SetCheckStatus(FALSE);
			}
		}

		switch(m_iSubViewCount)
		{
		case 3:
			m_mapTopBtn[TOP_STOCK_THREE].SetCheckStatus(TRUE);
			break;

		case 6:
			m_mapTopBtn[TOP_STOCK_SIX].SetCheckStatus(TRUE);
			break;

		case 9:
			m_mapTopBtn[TOP_STOCK_NINE].SetCheckStatus(TRUE);
			break;

		case 12:
			m_mapTopBtn[TOP_STOCK_TWELVE].SetCheckStatus(TRUE);
			break;

		case 15:
			m_mapTopBtn[TOP_STOCK_FIFTEEN].SetCheckStatus(TRUE);
			break;
		}
	}
}

LRESULT CIoViewDuoGuTongLie::OnIdleUpdateCmdUI( WPARAM w, LPARAM l )
{
	//CFrameWnd* pTarget = (CFrameWnd*)GetOwner();
	//if (pTarget == NULL || !pTarget->IsFrameWnd())
	//	pTarget = GetParentFrame();
	//if (pTarget != NULL)
	//{
	//	OnUpdateCmdUI(pTarget, (BOOL)w);
	//}

	return 0;
}

void CIoViewDuoGuTongLie::RefreshIndex()
{
	if(EDGLT_KLINE != m_DGTLType && EDGLT_KLINE_TREND != m_DGTLType)
	{
		return;
	}
	bool32 bHasSub = false;
	try
	{
		bHasSub = m_aIoViewKlines[0]->m_SubRegions.GetSize() > 0 && m_aIoViewKlines[0]->m_SubRegions[0].m_pSubRegionMain->GetCurveNum()>0;
	}
	catch(...)
	{
	}
	if ( !bHasSub )
	{
		// �����ڸ�ͼ���������Ӹ�ͼ
		CString StrIndexName = L"VOL";//CTBWndSpecial::GetMenuIdIndexName(ID_SPECIAL_STOCKTRADE);

		if ( CPluginFuncRight::Instance().IsUserHasIndexRight(StrIndexName, true) )
		{
			for ( int32 i=0; i < m_aIoViewKlines.GetSize() ; ++i )
			{
				m_aIoViewKlines[i]->AddShowIndex(L"MA");
				if ( !m_aIoViewKlines[i]->AddSubRegion(true) )
				{
					break;	// ��Ȼ��Ӳ��˸�ͼ�����ؼ�����
				}
				m_aIoViewKlines[i]->AddShowIndex(StrIndexName, true, false);	// ɾ�����ڵ�ָ�꣬���л���Ʒ
			}
		}
	}
}
CWnd* CIoViewDuoGuTongLie::FromPoint(CPoint pt)
{
	CWnd* pWnd = NULL;
	for ( int32 i=0; i < m_aIoViewKlines.GetSize() ; ++i )
	{
		CIoViewKLine* pIoview = m_aIoViewKlines[i];
		if (NULL != pIoview)
		{
			CRect rt;
			pIoview->GetWindowRect(&rt);
			ScreenToClient(&rt);
			if (rt.PtInRect(pt))
			{
				pWnd = pIoview;
				break;
			}
		}
	}
	for ( int32 i=0; i < m_aIoViewTrends.GetSize() ; ++i )
	{
		CIoViewTrend* pIoview = m_aIoViewTrends[i];
		if (NULL != pIoview)
		{
			CRect rt;
			pIoview->GetWindowRect(&rt);
			ScreenToClient(&rt);
			if (rt.PtInRect(pt))
			{
				pWnd = pIoview;
				break;
			}
		}
	}

	return pWnd;
}
void	CIoViewDuoGuTongLie::DrawViewBorder(CDC* pDC, const CRect& rtSel, const COLORREF& clr)
{	
	if (!rtSel.IsRectEmpty())
	{
		Gdiplus::Color clrPen;
		clrPen.SetFromCOLORREF(clr);
		Gdiplus::Pen pen(clrPen, KiBorderWidth);

		Gdiplus::PointF point1(rtSel.left, rtSel.top);
		Gdiplus::PointF point2(rtSel.right, rtSel.top);
		Gdiplus::PointF point3(rtSel.right, rtSel.bottom);
		Gdiplus::PointF point4(rtSel.left, rtSel.bottom);
		Gdiplus::PointF point5(rtSel.left, rtSel.top);
		Gdiplus::PointF points[5] = {point1, point2, point3, point4, point5};		

		Gdiplus::Graphics gh(pDC->GetSafeHdc());

		gh.DrawLines(&pen, points, 5);
	}	
}
void CIoViewDuoGuTongLie::ChangeDuoGuTongLieType( E_DuoGuTongLieType eType, int iVieRowCount, int iViewColCount)
{
	if(EDGLT_KLINE != eType && EDGLT_TREND != eType && EDGLT_KLINE_TREND != eType)
	{
		return;
	}
	// ������ͼ������Ϣ
	m_DGTLType = eType;
	m_iSubViewRowCount = iVieRowCount;
	m_iSubViewColCount = iViewColCount;
	if(EDGLT_KLINE_TREND == eType)
	{
		m_iSubViewRowCount = 2;
	}
	m_iSubViewCount = iVieRowCount * iViewColCount;
	DeleteSubIoViews();
	CreateSubViews();
	if(m_pFirstMerch)
	{
		SetBaseMerch(m_pFirstMerch);
	}
	else
	{
		CMerch *pMerch = GetMerchXml();
		if ( NULL != pMerch )
		{
			SetBaseMerch(pMerch);
		}
	}

	if(EDGLT_KLINE == m_DGTLType || EDGLT_KLINE_TREND == m_DGTLType)      // K�ߵĴ��� 
	{
		//ChangeKLineCycle(m_uCurTimeInterval);
		RefreshIndex();
	}
}
