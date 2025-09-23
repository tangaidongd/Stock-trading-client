#include "stdafx.h"
#include "memdc.h"
#include "MPIChildFrame.h"
#include "IoViewManager.h"
#include "MerchManager.h"
#include "GridCellSymbol.h"
#include "facescheme.h"
#include "ShareFun.h"
#include "IoViewFenJiaBiao.h"
#include "FontFactory.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////
// ��������ר�ŵ��������ͣ�ֻ���������е�TimeSale������
const UINT KGridCtrlId = 0x2020;
const UINT KGridCtrlBasicColumnCount = 4;	// 4�л�������
//lint --e(569)
const TCHAR KBaseChar = _T('��');
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewFenJiaBiao, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewFenJiaBiao, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewFenJiaBiao)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(GVN_GETDISPINFO, KGridCtrlId, OnGridGetDispInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewFenJiaBiao::CIoViewFenJiaBiao()
:CIoViewBase()
{
	m_bActive			= false;
	m_bHistoryTimeSale = false;

	m_RectTitle.SetRectEmpty();
	m_fAmountTotal = m_fVolTotal = 0.0f;
	m_fVolMax = m_fAmountMax = 0.0f;

	m_iPreferWidth = 0;
	m_iDecSave = 0;
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewFenJiaBiao::~CIoViewFenJiaBiao()
{
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
BOOL CIoViewFenJiaBiao::PreTranslateMessage(MSG* pMsg)
{
	if ( m_bHistoryTimeSale )
	{
		// ��ʷʱ����������Щ��Ϣ
		return CControlBase::PreTranslateMessage(pMsg);
	}

	if ( WM_LBUTTONDBLCLK == pMsg->message )
	{
		// ˫����ʱ��,�����ͼ:
		if ( DoShowFullSreen() )
		{
			return TRUE;
		}
	}

	if ( WM_KEYDOWN == pMsg->message || WM_KEYUP == pMsg->message )
	{
		switch ( pMsg->wParam )
		{
		case VK_PRIOR:
		case VK_NEXT:
			return CControlBase::PreTranslateMessage(pMsg);
		}
	}

	return CIoViewBase::PreTranslateMessage(pMsg);
}

void CIoViewFenJiaBiao::OnPaint()
{
	CPaintDC dc(this); // device context for painting	
	
	if ( !IsWindowVisible() )
	{
		return;
	}

	if ( GetParentGGTongViewDragFlag() || m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}
	
	//
	UnLockRedraw();
	
	// ����
	CMemDC dcMem(&dc, m_RectTitle);

	// ���Ʊ���
	if ( m_RectTitle.Height() > 0 )
	{
		COLORREF clrBk = GetIoViewColor(ESCBackground);
		dcMem->FillSolidRect(m_RectTitle, clrBk);

		if ( NULL != m_pMerchXml )
		{
			COLORREF clrText = GetIoViewColor(ESCVolume);
			dcMem->SetTextColor(clrText);
			dcMem->SetBkMode(TRANSPARENT);

			if ( IsActiveInFrame() )
			{
				CRect rcActive(m_iActiveXpos, m_iActiveYpos, m_iActiveXpos+2, m_iActiveYpos+2);
				dcMem->FillSolidRect(rcActive, m_ColorActive);
			}
			
			dcMem->SelectObject(GetIoViewFontObject(ESFNormal));
			CString StrTitle;
			StrTitle.Format(_T("    [%s] %s  �ּ۱�"), m_pMerchXml->m_MerchInfo.m_StrMerchCode.GetBuffer(), m_pMerchXml->m_MerchInfo.m_StrMerchCnName.GetBuffer());
			dcMem->DrawText(StrTitle, m_RectTitle, DT_SINGLELINE |DT_VCENTER |DT_CENTER);

			CSize sizeTitle = dcMem->GetTextExtent(StrTitle);
			dcMem->SetTextColor(GetIoViewColor(ESCKeep));

			//CRect rcNext(m_RectTitle);
			//rcNext.left += sizeTitle.cx + 40;
			//if ( rcNext.Width() > 0 )
			//{
			//	dcMem->DrawText(_T("Up/PageUp:�Ϸ� Down/PageDown:�·�"), rcNext, DT_SINGLELINE |DT_VCENTER |DT_LEFT);
			//}
		}
	}

	CRect rcClient;
	GetClientRect(rcClient);
	//DrawCorner(dcMem, rcClient);
}

void CIoViewFenJiaBiao::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();
}

void CIoViewFenJiaBiao::OnIoViewFontChanged()
{	
	CIoViewBase::OnIoViewFontChanged();
	
	// ��������
	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	// �ذ��Ŵ�С
	m_iPreferWidth = 0;	// ��Ҫ���¼����ʺϵĴ�С
	RecalcLayout();

	m_GridCtrl.Refresh();
}

void CIoViewFenJiaBiao::LockRedraw()
{
	if ( m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
	::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);	
}

void CIoViewFenJiaBiao::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
	::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
	m_GridCtrl.Invalidate();
}

int CIoViewFenJiaBiao::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CIoViewBase::OnCreate(lpCreateStruct);

	//
	InitialIoViewFace(this);

	// �������ݹ�����, ����Ҫˮƽ������
 	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
 	m_XSBVert.SetScrollRange(0, 10);

	// �����ֱ����ݱ��
	{
		m_GridCtrl.Create(CRect(0, 0, 0, 0), this, KGridCtrlId);
		m_GridCtrl.SetTextColor(CLR_DEFAULT);
		m_GridCtrl.SetFixedTextColor(CLR_DEFAULT);
		m_GridCtrl.SetTextBkColor(CLR_DEFAULT);
		m_GridCtrl.SetFixedTextColor(CLR_DEFAULT);
		m_GridCtrl.SetBkColor(CLR_DEFAULT);
		m_GridCtrl.SetFixedBkColor(CLR_DEFAULT);
		
		m_GridCtrl.SetDefCellWidth(60);
		m_GridCtrl.EnableBlink(FALSE);
		m_GridCtrl.SetVirtualMode(TRUE);
		m_GridCtrl.SetDefaultCellType(RUNTIME_CLASS(CGridCellNormalSys));
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

		m_GridCtrl.SetHeaderSort(FALSE);	// �޵������
		
		// �����໥֮��Ĺ���
		m_XSBVert.SetOwner(&m_GridCtrl);
		m_GridCtrl.SetScrollBar(NULL, &m_XSBVert);	// ��ˮƽ��
		
		// ���ñ����
		m_GridCtrl.EnableSelection(false);
		m_GridCtrl.ShowGridLine(false);
		m_GridCtrl.SetColumnResize(FALSE);
		m_GridCtrl.SetDrawFixedCellGridLineAsNormal(TRUE);
		m_GridCtrl.SetDonotScrollToNonExistCell(GVL_BOTH);

		// ��������
		LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
		m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
		m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
		m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);
		m_GridCtrl.EnablePolygonCorner(false);
	}

	// ����Ĭ�ϵı�ͷ
	
	//
	return 0;
}

void CIoViewFenJiaBiao::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);
			
	RecalcLayout();
}

void CIoViewFenJiaBiao::RecalcLayout()
{
	CRect rcClient;
	GetClientRect(rcClient);

	bool32 bMultiOld = IsInMultiColumnMode();

	// �ȼ���������ж�����
	int32 iPreferWidth = 0;
	GetCurrentPerfectWidth(iPreferWidth);
	iPreferWidth = max(1, iPreferWidth);
	int32 iCount = rcClient.Width()/iPreferWidth;
	const int32 iBaseCount = max(1, iCount);

	// ���
	if ( iBaseCount > 1 )
	{
		m_RectTitle = rcClient;
		LOGFONT lf = {0};
		GetIoViewFontObject(ESFNormal)->GetLogFont(&lf);
		m_RectTitle.bottom = abs(lf.lfHeight) + 6;

		CRect rcGrid(rcClient);
		rcGrid.top = m_RectTitle.bottom;
		m_GridCtrl.MoveWindow(rcGrid);
		InvalidateRect(m_RectTitle);
		m_GridCtrl.EnablePolygonCorner(false);
	}
	else
	{
		m_RectTitle.SetRectEmpty();
		m_GridCtrl.MoveWindow(rcClient);
		m_GridCtrl.EnablePolygonCorner(false);
	}

	RecalcTableRowColumn();

	bool32 bMultiNow = IsInMultiColumnMode();
	if ( (bMultiNow && !bMultiOld)
		|| (!bMultiNow && bMultiOld) )
	{
		// ״̬���
		m_GridCtrl.EnsureTopLeftCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
	}
}

int32 CIoViewFenJiaBiao::RecalcTableRowColumn()
{
	CCellID idCell = m_GridCtrl.GetFocusCell();

	// �ȼ��㵱ǰ�ʺϼ���
	int32 iBaseWidth = 0;
	if ( !GetCurrentPerfectWidth(iBaseWidth) )
	{
		ASSERT( 0 );
		iBaseWidth = 100;	// ȡһ��ֵ���
	}

	// ��ȡ�и���Ϣ
	CFont *pFontNormal = GetIoViewFontObject(ESFNormal);
	CClientDC dc(this);
	CFont *pFontOld = dc.SelectObject(pFontNormal);
	CSize sizeText1 = dc.GetTextExtent(_T("���Ը߶�HHH")) + CSize(0, 10);
	CSize sizeText2 = dc.GetTextExtent(_T("1234567890.%")) + CSize(0, 2);
	dc.SelectObject(pFontOld);
	
	CRect rcClient;
	GetClientRect(rcClient);
	if(rcClient.Width() <= 0 )
	{
		return 0;
	}
	int32 iBaseCount = rcClient.Width() / iBaseWidth;
	iBaseCount = max(1, iBaseCount);		// �����ǵ���

	CRect rcGrid;
	m_GridCtrl.GetClientRect(rcGrid);
	
	// ������
	m_GridCtrl.SetColumnCount(iBaseCount * KGridCtrlBasicColumnCount);	// n*4��
	m_GridCtrl.SetRowCount(0);	// ɾ��������
	// ������
	int iRowCount = 0;
	int iFixedRowCount = 0;
	if ( iBaseCount > 1 )
	{
		// �����б�ͷ
		iFixedRowCount = 1;
		// ������������һ������Ȼ��ڶ���....�ķ�ʽ����Ҫ����һ�����Է��ö�����
		// ÿ����������ibaseCount��
		m_GridCtrl.SetDefCellHeight(sizeText1.cy);
		m_GridCtrl.SetFixedRowCount(iFixedRowCount);
		m_GridCtrl.SetRowCount(iFixedRowCount+1);	// ����������
		const int32 iHeight = m_GridCtrl.GetRowHeight(iFixedRowCount);	// �Ե�һ�зǹ̶���Ϊ��׼�и߼���
		ASSERT( iHeight > 0 );
		int32 iNonFixedRowCount = 0;
		if ( iHeight > 0 )
		{
			iNonFixedRowCount = rcGrid.Height()/iHeight - iFixedRowCount;	// ����һ��������
			ASSERT( iNonFixedRowCount > 0 );
			if ( iNonFixedRowCount > 0 )
			{
				// ����һ������������
				// û��һ��������һ��
				// һ����������
				const int32 iDataCountPerScreen = iNonFixedRowCount*iBaseCount;
				const int32 iDataCount = m_aPricesSort.GetSize();
				if ( iDataCount/iDataCountPerScreen > 0 )
				{
					// ��������ʵ�ʿ��ԳŶ�����
					const int32 iReserve = iDataCount%iBaseCount;
					if ( iReserve != 0 )
					{
						iNonFixedRowCount = iDataCount/iBaseCount + iReserve;	// ��һ��
					}
					else
					{
						iNonFixedRowCount = iDataCount/iBaseCount;
					}
				}
				else
				{
					// ����һ����������
				}
			}
		}
		
		if ( iNonFixedRowCount <= 0 )
		{
			// �趨һ����ȱֵ
			const int32 iMod = m_aPricesSort.GetSize() % iBaseCount;
			if ( iMod > 0 )
			{
				iNonFixedRowCount = m_aPricesSort.GetSize()/iBaseCount + 1;	// ���һ��
			}
			else
			{
				iNonFixedRowCount = m_aPricesSort.GetSize()/iBaseCount;
			}
			iNonFixedRowCount = max(50, iNonFixedRowCount);
		}
		iRowCount = iNonFixedRowCount + iFixedRowCount;
	}
	else
	{
		// �����ޱ�ͷ
		iRowCount = m_aPricesSort.GetSize();

		m_GridCtrl.SetDefCellHeight(sizeText2.cy);
	}
	m_GridCtrl.SetRowCount(iRowCount);
	m_GridCtrl.SetFixedRowCount(iFixedRowCount);

	// �趨�п�
	m_GridCtrl.ExpandColumnsToFit(TRUE);
	int32 iColumnWidth = m_GridCtrl.GetColumnWidth(0);
	// ��base����ÿһ��С�о���Ŀ���в�ͬ 0,1����, 3���ڵ���ģʽ���ޱ�ͷ�������Ȳ����Ļ�����������������
	// �����ȳ��������������ȳ����ֿ�ȸ�2��
	// �����ָ�������ڿ��Լ���3�У����Կ��Է����2�У�TODO
	const int32 iBaseSmallColumnWidth = iBaseWidth/(int32)KGridCtrlBasicColumnCount;
	if ( iColumnWidth < iBaseSmallColumnWidth )
	{
		for ( int32 i=0; i < iBaseCount*KGridCtrlBasicColumnCount ; i++ )
		{	
			iColumnWidth = iBaseSmallColumnWidth;
			if ( iBaseCount == 1 )
			{
				if ( 3==i )
				{
					// ��Ȳ���
					//CClientDC tmpDC(this);
					//CFont *pTmpFontNormal = GetIoViewFontObject(ESFNormal);
					//CFont *pTmpFontOld = tmpDC.SelectObject(pTmpFontNormal);
					//CSize sizeText = tmpDC.GetTextExtent(_T("100.00"));
					//tmpDC.SelectObject(pTmpFontOld);
					//iColumnWidth = sizeText.cx + 2;
					iColumnWidth = rcClient.Width() - 3 * iBaseSmallColumnWidth - 5;
				}
			}
			
			m_GridCtrl.SetColumnWidth(i, iColumnWidth);	// ������
		}
	}
	else if ( iColumnWidth > iBaseSmallColumnWidth + 4 )
	{
		// ����, TODO
	}

	// ��ԭcellid
	if ( !m_GridCtrl.IsValid(idCell)  )
	{
		idCell = m_GridCtrl.GetTopleftNonFixedCell();
	}
	m_GridCtrl.SetFocusCell(idCell);

	return iBaseCount;
}
 
BOOL CIoViewFenJiaBiao::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return FALSE;
}

// ֪ͨ��ͼ�ı��ע����Ʒ
void CIoViewFenJiaBiao::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// �޸ĵ�ǰ�鿴����Ʒ
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
	
	// ��ǰ��Ʒ��Ϊ�յ�ʱ��
	if (NULL != pMerch)
	{
		m_iDecSave = pMerch->m_MerchInfo.m_iSaveDec;

		// ���ù�ע����Ʒ��Ϣ
		m_aSmartAttendMerchs.RemoveAll();

		CSmartAttendMerch SmartAttendMerch;
		SmartAttendMerch.m_pMerch = pMerch;
		SmartAttendMerch.m_iDataServiceTypes = EDSTTimeSale | EDSTTick | EDSTKLine | EDSTPrice;
		m_aSmartAttendMerchs.Add(SmartAttendMerch);

		// �������ǰ��ʾ����&����
		DeleteTableContent(true);

		// ȷ��������ʱ���Լ�
		// �����Ҫ�� �ı����
		// ��ȡ����Ʒ������ص�����
		CMarketIOCTimeInfo RecentTradingDay;
		bool32 bTradingDayOk = false;
		if ( m_bHistoryTimeSale )
		{
			bTradingDayOk = pMerch->m_Market.GetSpecialTradingDayTime(m_TimeHistory, RecentTradingDay, pMerch->m_MerchInfo);
		}
		else
		{
			bTradingDayOk = pMerch->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, pMerch->m_MerchInfo);
		}
		if ( bTradingDayOk )
		{
			m_TrendTradingDayInfo.Set(pMerch, RecentTradingDay);
			m_TrendTradingDayInfo.RecalcPrice(*pMerch);
			m_TrendTradingDayInfo.RecalcHold(*pMerch);
		}
		else
		{
			//ASSERT(0);
		}
		
		// 
		OnVDataMerchTimeSalesUpdate(pMerch);
	}
	else
	{
		// zhangbo 20090824 #�����䣬 ������Ʒ������ʱ����յ�ǰ��ʾ����
		//...
	}
	Invalidate();
}

//
void CIoViewFenJiaBiao::OnVDataForceUpdate()
{
	RequestViewData();
}

void CIoViewFenJiaBiao::OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;

	if (!::IsWindow(m_GridCtrl.GetSafeHwnd())) 
		return;

	// 
	CMerch &Merch = *pMerch;
	if (NULL == Merch.m_pMerchTimeSales)
	{
		return;
	}
		
	if ( 0 == Merch.m_pMerchTimeSales->m_Ticks.GetSize() )
	{
		return;
	}

	// �Ƚ����ݱ仯�� ���ڽ���������������ж�
	int32 iPosDayStart = CMerchTimeSales::QuickFindTickWithBigOrEqualReferTime(Merch.m_pMerchTimeSales->m_Ticks, m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit);
	if (iPosDayStart < 0 || iPosDayStart >= Merch.m_pMerchTimeSales->m_Ticks.GetSize())
		return;

	int32 iPosDayEnd = CMerchTimeSales::QuickFindTickWithSmallOrEqualReferTime(Merch.m_pMerchTimeSales->m_Ticks, m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd);
	if (iPosDayEnd < 0 || iPosDayEnd >= Merch.m_pMerchTimeSales->m_Ticks.GetSize())
		return;

	if (iPosDayEnd < iPosDayStart)
	{
		return;
	}

	CTick *pTickShow	= (CTick *)m_aTicksShow.GetData();
	const int32 iCountShow	= m_aTicksShow.GetSize();

	CTick *pTickSrc		= (CTick *)Merch.m_pMerchTimeSales->m_Ticks.GetData();
	pTickSrc		   += iPosDayStart;
	const int32 iCountSrc		= iPosDayEnd - iPosDayStart + 1;

	if ( iCountSrc == iCountShow && 0 == memcmp((void*)pTickShow, (void*)pTickSrc, iCountSrc*sizeof(CTick)) )
	{
		// ��ȫ��ͬ������������
	}
	else
	{	
		// ȫ������
		// �ڴ�ֱ�Ӹ��ƾ�����
 		m_aTicksShow.SetSize(iCountSrc);
 		pTickShow = (CTick *)m_aTicksShow.GetData();
 		int32 iCopyShow = m_aTicksShow.GetSize();
		
		memcpyex(pTickShow, pTickSrc, iCopyShow * sizeof(CTick));

		// �������ݣ�������ʾ
		CalcTableData();
	}
}

void CIoViewFenJiaBiao::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{	
	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;

	// ��Ҫ�������ռ�
	if (0. == m_TrendTradingDayInfo.m_fPricePrevClose && 0. == m_TrendTradingDayInfo.m_fPricePrevAvg && 0. == m_TrendTradingDayInfo.m_fHoldPrev)
	{
		float fBackupPricePrevClose = m_TrendTradingDayInfo.GetPrevReferPrice();

		m_TrendTradingDayInfo.RecalcPrice(*pMerch);
		m_TrendTradingDayInfo.RecalcHold(*pMerch);

		if (fBackupPricePrevClose != m_TrendTradingDayInfo.GetPrevReferPrice())
		{
			// �������ݣ�������ʾ
			CalcTableData();
		}
	}
}

void CIoViewFenJiaBiao::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;

	// ��Ҫ�������ռ�
	if (0. == m_TrendTradingDayInfo.m_fPricePrevClose && 0. == m_TrendTradingDayInfo.m_fPricePrevAvg && 0. == m_TrendTradingDayInfo.m_fHoldPrev)
	{
		float fBackupPricePrevClose = m_TrendTradingDayInfo.GetPrevReferPrice();

		m_TrendTradingDayInfo.RecalcPrice(*pMerch);
		m_TrendTradingDayInfo.RecalcHold(*pMerch);

		if (fBackupPricePrevClose != m_TrendTradingDayInfo.GetPrevReferPrice())
		{
			// �������ݣ�������ʾ
			CalcTableData();
		}
	}
}

bool32 CIoViewFenJiaBiao::FromXml(TiXmlElement * pElement)
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

	// 
	CMerch *pMerchFound = NULL;
	if (!m_pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerchFound))
	{
		pMerchFound = NULL;
	}
	
	// ��Ʒ�����ı�
	OnVDataMerchChanged(iMarketId, StrMerchCode, pMerchFound);	

	return true;
}

CString CIoViewFenJiaBiao::ToXml()
{
	CString StrThis;

	CString StrMarketId;
	StrMarketId.Format(L"%d", m_MerchXml.m_iMarketId);

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" ",/*>\n*/ 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrShowTabName()).GetBuffer(), 
		m_StrTabShowName.GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(),
		m_MerchXml.m_StrMerchCode.GetBuffer(),
		CString(GetXmlElementAttrMarketId()).GetBuffer(), 
		StrMarketId.GetBuffer());
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

CString CIoViewFenJiaBiao::GetDefaultXML()
{
	CString StrThis;

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" >\n", 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(),
		L"",
		CString(GetXmlElementAttrMarketId()).GetBuffer(), 
		L"-1");

	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

void CIoViewFenJiaBiao::DeleteTableContent(bool32 bDelRealData/* = false*/)
{
	// ���ñ��
	m_GridCtrl.DeleteNonFixedRows();	// ��ʾ�����������

	if ( bDelRealData )
	{
		// ���ʵ������
		ClearShowData();
		m_aTicksShow.RemoveAll();
	}
}

void CIoViewFenJiaBiao::ClearShowData()
{
	m_aPricesSort.RemoveAll();
	m_fVolTotal = m_fAmountTotal = 0.0f;
	m_fVolMax = m_fAmountMax = 0.0f;
}

void CIoViewFenJiaBiao::OnIoViewActive()
{
	m_bActive = IsActiveInFrame();
	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	//
	if ( m_RectTitle.Height() > 0 )
	{
		InvalidateRect(m_RectTitle, TRUE);
	}
	else
	{
		// �Ƿ�Ҫ���ñ�־��
	}

	//
	if ( NULL != m_GridCtrl.GetSafeHwnd() )
	{
		m_GridCtrl.SetFocus();
	}
	
	SetChildFrameTitle();
}

void CIoViewFenJiaBiao::OnIoViewDeactive()
{
	m_bActive = false;

	//
	if ( m_RectTitle.Height() > 0 )
	{
		InvalidateRect(m_RectTitle, TRUE);
	}
	else
	{
		// �Ƿ�Ҫ���ñ�־��
	}
}

void  CIoViewFenJiaBiao::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;

	if (NULL != m_pMerchXml)
	{	
		StrTitle += L" ";
		StrTitle += m_pMerchXml->m_MerchInfo.m_StrMerchCnName;
	}
	
	CMPIChildFrame * pParentFrame = (CMPIChildFrame *)GetParentFrame();
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

void CIoViewFenJiaBiao::RequestViewData()
{
	if (NULL == m_pMerchXml || NULL == m_pAbsCenterManager)
		return;

// 	if ( !IsWindowVisible() )
// 	{
// 		//TRACE(_T("---------Ignore Invisible request!!\r\n"));
// 		return;
// 	}
	
	CGmtTime TimeInit	= m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
	CGmtTime TimeEnd	= TimeInit + CGmtTimeSpan(0, 23, 59, 59);
	
	CGmtTime TimeServer = m_pAbsCenterManager->GetServerTime();
	if (!m_bHistoryTimeSale && (TimeServer < TimeInit || TimeServer > TimeEnd))	// ����ͬһ�죬 ��Ҫ������ʱ������ռ۵�
	{
		// �ǽ���ʱ�εĴ���
		
		// ��ȡ����Ʒ������ص�����
		CMarketIOCTimeInfo RecentTradingDay;
		if (m_pMerchXml->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, m_pMerchXml->m_MerchInfo))
		{
			m_TrendTradingDayInfo.Set(m_pMerchXml, RecentTradingDay);
			m_TrendTradingDayInfo.RecalcPrice(*m_pMerchXml);
			m_TrendTradingDayInfo.RecalcHold(*m_pMerchXml);

			// �ж����������Ƿ����ڻ�õ��ٽ��������ڽ���ʱ�ε����ݣ����ǣ�����ո�����
			if ( m_aTicksShow.GetSize() > 0 )
			{
				// �ֱʵ�ʱ��Ӧ�����ڵ����г��ĳ�ʼ��ʱ�� ����С�� ��Endʱ��
				if ( RecentTradingDay.m_TimeInit >  m_aTicksShow[0].m_TimeCurrent || RecentTradingDay.m_TimeEnd < m_aTicksShow[0].m_TimeCurrent )
				{
					// ������ʱ������������� ���� ������������
					// �������ǰ��������
					DeleteTableContent(true);
				}
			}
		}
		else
		{
			//ASSERT(0);
		}
	}
	
	// ����ָ��ʱ�����K�ߣ� �Ա��ȡ��ǰ��ʾ����ͼ�����ռۣ� �񿪼�
	{
		ASSERT(m_TrendTradingDayInfo.m_bInit);
		
		// 
		CGmtTime TimeDay = m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose.m_Time;
		SaveDay(TimeDay);
		
		CMmiReqMerchKLine info;
		info.m_eKLineTypeBase	= EKTBDay;
		info.m_iMarketId		= m_pMerchXml->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		info.m_eReqTimeType		= ERTYFrontCount;		// Ϊ��ֹ���죬��������&����
		info.m_TimeStart		= TimeDay;
		//info.m_TimeEnd			= TimeDay;
		info.m_iFrontCount			= 2;			
		DoRequestViewData(info);
	}
	
	// ��RealtimePrice����, Ϊȡ���ռ�
	{
		CMmiReqRealtimePrice Req;
		Req.m_iMarketId			= m_pMerchXml->m_MerchInfo.m_iMarketId;
		Req.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		DoRequestViewData(Req);
	}
	
	// �������Ʒ������
	{
		// ��ͨ����£�������ʷ�ֱ����ݣ��������������
		CMmiReqMerchTimeSales info;
		info.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		
		info.m_eReqTimeType	= ERTYSpecifyTime;
		info.m_TimeEnd		= TimeEnd;
		info.m_TimeStart	= TimeInit;		// ��ʼ���г�ʱ - �ڶ���ĳ�ʼ��ǰһ�� (24h)
		
		DoRequestViewData(info);
	}
	
	// ��RealtimeTick����
	{
		CMmiReqRealtimeTick Req;
		Req.m_iMarketId			= m_pMerchXml->m_MerchInfo.m_iMarketId;
		Req.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		DoRequestViewData(Req);
	}
}

void CIoViewFenJiaBiao::SetHistoryTime( const CGmtTime &TimeAttend )
{
	m_bHistoryTimeSale	= true;
	m_TimeHistory	= TimeAttend;
	CMarketIOCTimeInfo RecentTradingDay;
	if ( NULL != m_pMerchXml 
		&& m_pMerchXml->m_Market.GetSpecialTradingDayTime(m_TimeHistory, RecentTradingDay, m_pMerchXml->m_MerchInfo)
		)
	{
		m_TrendTradingDayInfo.Set(m_pMerchXml, RecentTradingDay);
		m_TrendTradingDayInfo.RecalcPrice(*m_pMerchXml);
		m_TrendTradingDayInfo.RecalcHold(*m_pMerchXml);

		if ( m_GridCtrl.m_hWnd != NULL )
		{
			if ( m_GridCtrl.GetRowCount() > m_GridCtrl.GetFixedRowCount() )
			{
				m_GridCtrl.EnsureVisible(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());		// ���ܱ�֤���ǵ�һ���ɼ�
			}
			DeleteTableContent();		// �����������
		}
	}
	// ������ʱ����ֻʣ��merchchangedʱ��
}

bool32 CIoViewFenJiaBiao::GetCurrentPerfectWidth( OUT int32 &iWidth )
{
	// 
	if ( m_iPreferWidth < 1 )
	{
		// ����һ��������������
		CString StrBase;
		for ( int32 i=0; i < 6 ; i++ )
		{
			StrBase += KBaseChar;
		}
		// 4�ַ�

		CFont *pFontNormal = GetIoViewFontObject(ESFNormal);
		ASSERT( NULL != pFontNormal );
		CClientDC dc(this);
		dc.SaveDC();

		dc.SelectObject(pFontNormal);
		dc.SetTextJustification(0, 0);
		CSize sizeBase = dc.GetTextExtent(StrBase);
		const int32 iColWidth = (int32)(sizeBase.cx +4) ;

		m_iPreferWidth = iColWidth * KGridCtrlBasicColumnCount;

		dc.RestoreDC(-1);
	}

	if ( m_iPreferWidth > 1 )
	{
		iWidth = m_iPreferWidth;
		return true;
	}
	return false;
}

void CIoViewFenJiaBiao::OnGridGetDispInfo( NMHDR *pNotifyStruct, LRESULT *pResult )
{
	if ( NULL != pNotifyStruct )
	{
		GV_DISPINFO	*pDisp = (GV_DISPINFO *)pNotifyStruct;
		// ����v mode

		// �ж��Ƿ��Ƕ���, �����б�ͷ
		bool32 bHasFixedRow = IsInMultiColumnMode();
		int32 iColCount = m_GridCtrl.GetColumnCount();
		const int32 iFixedRow = m_GridCtrl.GetFixedRowCount();
		const int32 iBaseCount = m_GridCtrl.GetColumnCount() / (int32)KGridCtrlBasicColumnCount;	// ���ٸ�������
		ASSERT( iBaseCount > 0 );
		if ( iColCount > KGridCtrlBasicColumnCount )
		{
			if ( iFixedRow != 1 )
			{
				return;	// ��ʱΪ״̬��ͬ��������Ҫ�����ʱ������
			}
		}
		else if ( iColCount < KGridCtrlBasicColumnCount )
		{
			return;	// ��Ч���
		}
		else
		{
			if ( iFixedRow != 0 )
			{
				return; // ��ʱΪ״̬��ͬ��������Ҫ�����ʱ������
			}
		}

		bool32 bIsExp = false;
		if ( m_pMerchXml != NULL )
		{
			bIsExp = GetMerchKind(m_pMerchXml) == ERTExp;
		}

		CGridCellNormalSys *pCell = DYNAMIC_DOWNCAST(CGridCellNormalSys, m_GridCtrl.GetDefaultVirtualCell());
		ASSERT( pCell != NULL );
		if ( NULL != pCell )
		{
			pCell->SetCellRectDrawFlag(0);
			pCell->SetDrawTextNormalStyle(CGridCellNormalSys::EDTNS_Text);
		}
		
		GV_ITEM &item = pDisp->item;
		item.lParam = NULL;
		if ( item.row  == 0 && bHasFixedRow )
		{
			// ��ͷ���� 
			switch ( item.col%((int32)KGridCtrlBasicColumnCount) )
			{
			case 0:
				{
					item.strText = _T("�۸�");
					if ( NULL != pCell )
					{
						pCell->SetCellRectDrawFlag(DR_TOP_SOILD |DR_BOTTOM_SOILD);
					}
				}
				break;
			case 1:
				{
					// ָ���ɽ�������ɽ���
					if ( !bIsExp )
					{
						item.strText = _T("�ɽ���");
					}
					else
					{
						item.strText = _T("�ɽ���");
					}
					if ( NULL != pCell )
					{
						pCell->SetCellRectDrawFlag(DR_TOP_SOILD |DR_BOTTOM_SOILD);
					}
				}
				break;
			case 2:
				{
					item.strText = _T("����");
					item.nFormat = DT_SINGLELINE |DT_VCENTER |DT_LEFT;
					if ( NULL != pCell )
					{
						pCell->SetCellRectDrawFlag(DR_TOP_SOILD |DR_BOTTOM_SOILD);
					}
				}
				break;
			case 3:
				{
					item.strText = _T("������%");
					if ( NULL != pCell )
					{
						pCell->SetCellRectDrawFlag(DR_TOP_SOILD |DR_BOTTOM_SOILD |DR_RIGHT_SOILD);
					}
				}
				break;
			default:
				ASSERT( 0 );
			}
		}
		else
		{
			// ���������ɼ�������õ����ݣ�����������ֱ���е�������
			CCellRange rangeVisible = m_GridCtrl.GetVisibleNonFixedCellRange();
			if ( !rangeVisible.IsValid() || rangeVisible.GetRowSpan() < 1 )
			{
				return;	// û��Ҫ��ʾ������
			}

			// ʵ��Ҫ��ʾ������ rowSpan * iBaseCount
			// ÿһbase�еĿ�ʼ����startPosΪ (minRow-fixedRow)*iBaseCount + rowSpan*(col/KBaseColumn)
			// ������Ϊ rowSpan
			// ���������Ƿ��ڿ���ʾ��Χ��
			const int32 iDataCount = rangeVisible.GetRowSpan();
			const int32 iDataStartPos = (rangeVisible.GetMinRow()-iFixedRow)*iBaseCount + iDataCount*(item.col/((int32)KGridCtrlBasicColumnCount));

			if ( !rangeVisible.InRange(item.row, item.col) )
			{
				// ������ʾ��
				return;
			}

			const int32 iDataPos = iDataStartPos + item.row - rangeVisible.GetMinRow();
			
			LOGFONT fontNumber;
			memset(&fontNumber, 0, sizeof(fontNumber));
			_tcscpy(fontNumber.lfFaceName, gFontFactory.GetExistFontName(_T("Arial")));///
			fontNumber.lfHeight  = -14;
			fontNumber.lfWeight = 560;

			if ( iDataPos >= 0 && iDataPos < m_aPricesSort.GetSize() )
			{
				const T_TickStatistic &ts = m_aPricesSort[iDataPos];
				
				switch ( item.col%((int32)KGridCtrlBasicColumnCount) )
				{
				case 0:
					{
						// �۸�
						item.rtTextPadding = CRect(15, 0, 0,0);
						item.crFgClr = GetIoViewColor(ts.m_eColor);
						item.strText = Float2String(ts.m_fPrice, m_iDecSave, false, false);
						item.nFormat = DT_SINGLELINE |DT_VCENTER |DT_LEFT;
						item.lfFont = fontNumber;
						
					}
					break;
				case 1:
					{
						// �ɽ���, ָ���ɽ���
						item.crFgClr = RGB(191, 191, 0);
						if ( bIsExp )
						{
							item.strText = Float2String(ts.m_fAmountTotal, 0, true);
						}
						else
						{
							item.strText = Float2String(ts.m_fVolTotal, 0, true);
						}
						item.nFormat = DT_SINGLELINE |DT_VCENTER |DT_RIGHT;
						item.lfFont = fontNumber;
					}
					break;
				case 2:
					{
						// ���� ��ʾ������������ ��ʽ
						double dPec = 0;
						if ( bIsExp )
						{
							dPec = (ts.m_fAmountTotal) / m_fAmountMax;
						}
						else
						{
							dPec = (ts.m_fVolTotal)/m_fVolMax;
						}
						dPec = max(0.0, dPec);

						item.strText.Format(_T("%f"), dPec);
						item.nFormat = DT_SINGLELINE |DT_VCENTER |DT_LEFT;
						
						//if ( ESCFall == ts.m_eColor )
						//{
						//	item.crFgClr = GetIoViewColor(ESCKLineFall);
						//}
						//else
						{
							item.crFgClr = GetIoViewColor(ts.m_eColor);
						}
						if ( NULL != pCell )
						{
							pCell->SetDrawTextNormalStyle(CGridCellNormalSys::EDTNS_HorzPercentStick);
						}
					}
					break;
				case 3:
					{
						// ������
						float fPec = 0.0;
						item.nFormat = DT_SINGLELINE |DT_VCENTER |DT_RIGHT;
						item.rtTextPadding = CRect(0, 0, 15,0);
						if ( bIsExp )
						{
							if ( ts.m_fAmountTotal != 0.0f )
							{
								fPec = 100*ts.m_fAmountBuy/ts.m_fAmountTotal;
							}
							// ָ������ʾ������
							//item.strText = Float2String(fPec, 2, false, false);
						}
						else
						{
							if ( ts.m_fVolTotal != 0.0f )
							{
								fPec = 100*ts.m_fVolBuy/ts.m_fVolTotal;
							}
							item.strText = Float2String(fPec, 2, false, false);
							item.lfFont = fontNumber;
						}
						if ( NULL != pCell && bHasFixedRow )
						{
							pCell->SetCellRectDrawFlag(DR_RIGHT_SOILD);
						}
					}
					break;
				default:
					ASSERT( 0 );
				}
			}
			else
			{
				ASSERT( iDataPos >= 0 );	// �����ڲ��ֶ���Ŀ������������
				ASSERT( item.row < m_GridCtrl.GetRowCount() );
				// ��������
				switch ( item.col%((int32)KGridCtrlBasicColumnCount) )
				{
				case 0:
					{
						// �۸�
					}
					break;
				case 1:
					{
						// �ɽ���, ָ���ɽ���
					}
					break;
				case 2:
					{
						// ���� ��ʾ������������ ��ʽ����10�ȷ�
					}
					break;
				case 3:
					{
						// ������
						if ( NULL != pCell && bHasFixedRow )
						{
							pCell->SetCellRectDrawFlag(DR_RIGHT_SOILD);
						}
					}
					break;
				default:
					ASSERT( 0 );
				}
			}
		}
	}
}

bool32 CIoViewFenJiaBiao::DoShowFullSreen()
{
	CMainFrame* pMainFrame		= (CMainFrame*)AfxGetMainWnd();
	CGGTongView* pGGTongView	= GetParentGGtongView();
	
	//
	if ( NULL != pMainFrame && NULL != pGGTongView )
	{
		pMainFrame->OnProcessF7(pGGTongView);
		return TRUE;
	}

	return FALSE;
}

bool32 CIoViewFenJiaBiao::IsInMultiColumnMode()
{
	return m_GridCtrl.GetColumnCount()/((int32)KGridCtrlBasicColumnCount) > 1;	// �ж��������
}

void CIoViewFenJiaBiao::CalcTableData()
{
	// �����Դ������������ݺ���ʾ
	// ��ס�˴εı��
	CCellID idCell = m_GridCtrl.GetFocusCell();

	m_GridCtrl.DeleteNonFixedRows();
	ClearShowData();

	if ( NULL == m_pMerchXml )
	{
		return;
	}

	// ȡ����
	float fPrevClose = m_TrendTradingDayInfo.GetPrevReferPrice();
	if ( 0.0f == fPrevClose )
	{
		return;	// ������
	}

	// ����
	m_aPricesSort.SetSize(0, 200);
	for ( int32 i=0; i < m_aTicksShow.GetSize() ; i++ )
	{
		const CTick &tick = m_aTicksShow[i];
		float fPrice = tick.m_fPrice;

		// ������������, �����ɸ߼۵�����֯
		bool32 bAdded = FALSE;
		int32 iSort=0;
		T_TickStatistic *pDst = NULL;
		for ( iSort = 0; iSort < m_aPricesSort.GetSize() ; iSort++ )
		{
			if ( m_aPricesSort[iSort].m_fPrice == fPrice )
			{
				pDst = &m_aPricesSort[iSort];	// ֱ�Ӳ���
				bAdded = TRUE;
				break;
			}
			else if ( fPrice > m_aPricesSort[iSort].m_fPrice )
			{
				// ����۸�ߵ�
				break;
			}
		}
		if ( !bAdded )
		{
			// ��Ҫ����µ�ͳ����Ϣ
			// ����
			T_TickStatistic ts;
			ZeroMemory(&ts, sizeof(ts));
			if ( fPrice > fPrevClose )	// �����ж�?
			{
				ts.m_eColor = ESCRise;
			}
			else if ( fPrice < fPrevClose )
			{
				ts.m_eColor = ESCFall;
			}
			else
			{
				ts.m_eColor = ESCKeep;
			}
			ts.m_fPrice = fPrice;
			
			if ( iSort >= m_aPricesSort.GetSize() )
			{
				iSort = m_aPricesSort.Add(ts);
			}
			else
			{
				m_aPricesSort.InsertAt(iSort, ts);
			}

			pDst = &m_aPricesSort[iSort];	// ������ַ
		}
		
		if ( NULL != pDst )
		{
			if ( tick.m_eTradeKind == CTick::ETKBuy )
			{
				pDst->m_fVolSell	+= tick.m_fVolume;
				pDst->m_fAmountSell += tick.m_fAmount;
			}
			else if ( tick.m_eTradeKind == CTick::ETKSell )
			{
				pDst->m_fVolBuy		+= tick.m_fVolume;
				pDst->m_fAmountBuy	+= tick.m_fAmount;
			}
			pDst->m_fVolTotal		+= tick.m_fVolume;
			pDst->m_fAmountTotal	+= tick.m_fAmount;

			m_fAmountTotal	+= tick.m_fAmount;
			m_fVolTotal		+= tick.m_fVolume;

			m_fVolMax		= max(m_fVolMax, pDst->m_fVolTotal);
			m_fAmountMax	= max(m_fAmountMax, pDst->m_fAmountTotal);
		}
		else
		{
			ASSERT( 0 );
		}
	}

	// ���¼�����
	RecalcTableRowColumn();
	
	// ��ԭcellid
	if ( !m_GridCtrl.IsValid(idCell)  )
	{
		idCell = m_GridCtrl.GetTopleftNonFixedCell();
	}
	m_GridCtrl.SetFocusCell(idCell);

	m_GridCtrl.Refresh();	// ����ʾ
}
