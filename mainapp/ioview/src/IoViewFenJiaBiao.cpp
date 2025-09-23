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
// 依现在无专门的数据类型，只有请求所有的TimeSale数据了
const UINT KGridCtrlId = 0x2020;
const UINT KGridCtrlBasicColumnCount = 4;	// 4列基本数据
//lint --e(569)
const TCHAR KBaseChar = _T('■');
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
		// 历史时，不处理这些消息
		return CControlBase::PreTranslateMessage(pMsg);
	}

	if ( WM_LBUTTONDBLCLK == pMsg->message )
	{
		// 双击的时候,最大化视图:
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
	
	// 标题
	CMemDC dcMem(&dc, m_RectTitle);

	// 绘制标题
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
			StrTitle.Format(_T("    [%s] %s  分价表"), m_pMerchXml->m_MerchInfo.m_StrMerchCode.GetBuffer(), m_pMerchXml->m_MerchInfo.m_StrMerchCnName.GetBuffer());
			dcMem->DrawText(StrTitle, m_RectTitle, DT_SINGLELINE |DT_VCENTER |DT_CENTER);

			CSize sizeTitle = dcMem->GetTextExtent(StrTitle);
			dcMem->SetTextColor(GetIoViewColor(ESCKeep));

			//CRect rcNext(m_RectTitle);
			//rcNext.left += sizeTitle.cx + 40;
			//if ( rcNext.Width() > 0 )
			//{
			//	dcMem->DrawText(_T("Up/PageUp:上翻 Down/PageDown:下翻"), rcNext, DT_SINGLELINE |DT_VCENTER |DT_LEFT);
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
	
	// 设置字体
	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	// 重安排大小
	m_iPreferWidth = 0;	// 需要重新计算适合的大小
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

	// 创建横纵滚动条, 不需要水平滚动条
 	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
 	m_XSBVert.SetScrollRange(0, 10);

	// 创建分笔数据表格
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

		m_GridCtrl.SetHeaderSort(FALSE);	// 无点击排序
		
		// 设置相互之间的关联
		m_XSBVert.SetOwner(&m_GridCtrl);
		m_GridCtrl.SetScrollBar(NULL, &m_XSBVert);	// 无水平条
		
		// 设置表格风格
		m_GridCtrl.EnableSelection(false);
		m_GridCtrl.ShowGridLine(false);
		m_GridCtrl.SetColumnResize(FALSE);
		m_GridCtrl.SetDrawFixedCellGridLineAsNormal(TRUE);
		m_GridCtrl.SetDonotScrollToNonExistCell(GVL_BOTH);

		// 设置字体
		LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
		m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
		m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
		m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);
		m_GridCtrl.EnablePolygonCorner(false);
	}

	// 设置默认的表头
	
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

	// 先计算表格的内有多少列
	int32 iPreferWidth = 0;
	GetCurrentPerfectWidth(iPreferWidth);
	iPreferWidth = max(1, iPreferWidth);
	int32 iCount = rcClient.Width()/iPreferWidth;
	const int32 iBaseCount = max(1, iCount);

	// 面积
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
		// 状态变更
		m_GridCtrl.EnsureTopLeftCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
	}
}

int32 CIoViewFenJiaBiao::RecalcTableRowColumn()
{
	CCellID idCell = m_GridCtrl.GetFocusCell();

	// 先计算当前适合几列
	int32 iBaseWidth = 0;
	if ( !GetCurrentPerfectWidth(iBaseWidth) )
	{
		ASSERT( 0 );
		iBaseWidth = 100;	// 取一个值替代
	}

	// 获取行高信息
	CFont *pFontNormal = GetIoViewFontObject(ESFNormal);
	CClientDC dc(this);
	CFont *pFontOld = dc.SelectObject(pFontNormal);
	CSize sizeText1 = dc.GetTextExtent(_T("测试高度HHH")) + CSize(0, 10);
	CSize sizeText2 = dc.GetTextExtent(_T("1234567890.%")) + CSize(0, 2);
	dc.SelectObject(pFontOld);
	
	CRect rcClient;
	GetClientRect(rcClient);
	if(rcClient.Width() <= 0 )
	{
		return 0;
	}
	int32 iBaseCount = rcClient.Width() / iBaseWidth;
	iBaseCount = max(1, iBaseCount);		// 至少是单列

	CRect rcGrid;
	m_GridCtrl.GetClientRect(rcGrid);
	
	// 计算列
	m_GridCtrl.SetColumnCount(iBaseCount * KGridCtrlBasicColumnCount);	// n*4列
	m_GridCtrl.SetRowCount(0);	// 删除所有行
	// 计算行
	int iRowCount = 0;
	int iFixedRowCount = 0;
	if ( iBaseCount > 1 )
	{
		// 多列有表头
		iFixedRowCount = 1;
		// 采用先整屏第一列满，然后第二列....的方式，需要计算一屏可以放置多少列
		// 每行数量包含ibaseCount个
		m_GridCtrl.SetDefCellHeight(sizeText1.cy);
		m_GridCtrl.SetFixedRowCount(iFixedRowCount);
		m_GridCtrl.SetRowCount(iFixedRowCount+1);	// 至少有两行
		const int32 iHeight = m_GridCtrl.GetRowHeight(iFixedRowCount);	// 以第一行非固定行为标准行高计算
		ASSERT( iHeight > 0 );
		int32 iNonFixedRowCount = 0;
		if ( iHeight > 0 )
		{
			iNonFixedRowCount = rcGrid.Height()/iHeight - iFixedRowCount;	// 计算一屏的行数
			ASSERT( iNonFixedRowCount > 0 );
			if ( iNonFixedRowCount > 0 )
			{
				// 超过一屏，则算整行
				// 没过一屏，则算一屏
				// 一屏的数据数
				const int32 iDataCountPerScreen = iNonFixedRowCount*iBaseCount;
				const int32 iDataCount = m_aPricesSort.GetSize();
				if ( iDataCount/iDataCountPerScreen > 0 )
				{
					// 超过就算实际可以撑多少行
					const int32 iReserve = iDataCount%iBaseCount;
					if ( iReserve != 0 )
					{
						iNonFixedRowCount = iDataCount/iBaseCount + iReserve;	// 多一行
					}
					else
					{
						iNonFixedRowCount = iDataCount/iBaseCount;
					}
				}
				else
				{
					// 就是一屏，不用了
				}
			}
		}
		
		if ( iNonFixedRowCount <= 0 )
		{
			// 设定一个空缺值
			const int32 iMod = m_aPricesSort.GetSize() % iBaseCount;
			if ( iMod > 0 )
			{
				iNonFixedRowCount = m_aPricesSort.GetSize()/iBaseCount + 1;	// 多空一行
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
		// 单列无表头
		iRowCount = m_aPricesSort.GetSize();

		m_GridCtrl.SetDefCellHeight(sizeText2.cy);
	}
	m_GridCtrl.SetRowCount(iRowCount);
	m_GridCtrl.SetFixedRowCount(iFixedRowCount);

	// 设定列宽
	m_GridCtrl.ExpandColumnsToFit(TRUE);
	int32 iColumnWidth = m_GridCtrl.GetColumnWidth(0);
	// 单base列下每一个小列具体的宽度有不同 0,1正常, 3列在单列模式下无表头，如果宽度不够的话，将减少它的需求
	// 如果宽度超出正常需求，则匀出部分宽度给2列
	// 如果是指数，由于可以减少3列，所以可以分配给2列，TODO
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
					// 宽度不够
					//CClientDC tmpDC(this);
					//CFont *pTmpFontNormal = GetIoViewFontObject(ESFNormal);
					//CFont *pTmpFontOld = tmpDC.SelectObject(pTmpFontNormal);
					//CSize sizeText = tmpDC.GetTextExtent(_T("100.00"));
					//tmpDC.SelectObject(pTmpFontOld);
					//iColumnWidth = sizeText.cx + 2;
					iColumnWidth = rcClient.Width() - 3 * iBaseSmallColumnWidth - 5;
				}
			}
			
			m_GridCtrl.SetColumnWidth(i, iColumnWidth);	// 变更宽度
		}
	}
	else if ( iColumnWidth > iBaseSmallColumnWidth + 4 )
	{
		// 超出, TODO
	}

	// 还原cellid
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

// 通知视图改变关注的商品
void CIoViewFenJiaBiao::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
	
	// 当前商品不为空的时候
	if (NULL != pMerch)
	{
		m_iDecSave = pMerch->m_MerchInfo.m_iSaveDec;

		// 设置关注的商品信息
		m_aSmartAttendMerchs.RemoveAll();

		CSmartAttendMerch SmartAttendMerch;
		SmartAttendMerch.m_pMerch = pMerch;
		SmartAttendMerch.m_iDataServiceTypes = EDSTTimeSale | EDSTTick | EDSTKLine | EDSTPrice;
		m_aSmartAttendMerchs.Add(SmartAttendMerch);

		// 先清掉当前显示内容&数据
		DeleteTableContent(true);

		// 确定开收盘时间以及
		// 如果需要， 改变标题
		// 获取该商品走势相关的数据
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
		// zhangbo 20090824 #待补充， 当心商品不存在时，清空当前显示数据
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

	// 比较数据变化， 对于仅更后面的数据做判断
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
		// 完全相同，可跳过计算
	}
	else
	{	
		// 全部更新
		// 内存直接复制就行了
 		m_aTicksShow.SetSize(iCountSrc);
 		pTickShow = (CTick *)m_aTicksShow.GetData();
 		int32 iCopyShow = m_aTicksShow.GetSize();
		
		memcpyex(pTickShow, pTickSrc, iCopyShow * sizeof(CTick));

		// 计算数据，更新显示
		CalcTableData();
	}
}

void CIoViewFenJiaBiao::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{	
	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;

	// 需要更新昨收价
	if (0. == m_TrendTradingDayInfo.m_fPricePrevClose && 0. == m_TrendTradingDayInfo.m_fPricePrevAvg && 0. == m_TrendTradingDayInfo.m_fHoldPrev)
	{
		float fBackupPricePrevClose = m_TrendTradingDayInfo.GetPrevReferPrice();

		m_TrendTradingDayInfo.RecalcPrice(*pMerch);
		m_TrendTradingDayInfo.RecalcHold(*pMerch);

		if (fBackupPricePrevClose != m_TrendTradingDayInfo.GetPrevReferPrice())
		{
			// 计算数据，更新显示
			CalcTableData();
		}
	}
}

void CIoViewFenJiaBiao::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;

	// 需要更新昨收价
	if (0. == m_TrendTradingDayInfo.m_fPricePrevClose && 0. == m_TrendTradingDayInfo.m_fPricePrevAvg && 0. == m_TrendTradingDayInfo.m_fHoldPrev)
	{
		float fBackupPricePrevClose = m_TrendTradingDayInfo.GetPrevReferPrice();

		m_TrendTradingDayInfo.RecalcPrice(*pMerch);
		m_TrendTradingDayInfo.RecalcHold(*pMerch);

		if (fBackupPricePrevClose != m_TrendTradingDayInfo.GetPrevReferPrice())
		{
			// 计算数据，更新显示
			CalcTableData();
		}
	}
}

bool32 CIoViewFenJiaBiao::FromXml(TiXmlElement * pElement)
{
	if (NULL == pElement)
		return false;

	// 判读是不是IoView的节点
	const char *pcValue = pElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;

	// 判断是不是描述自己这个业务视图的节点
	const char *pcAttrValue = pElement->Attribute(GetXmlElementAttrIoViewType());
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(CString(pcAttrValue)) != 0)	// 不是描述自己的业务节点
		return false;
	
	// 读取本业务视图特有的内容
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
	
	// 商品发生改变
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
	// 重置表格
	m_GridCtrl.DeleteNonFixedRows();	// 显示的数据行清除

	if ( bDelRealData )
	{
		// 清除实际数据
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
		// 是否还要设置标志？
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
		// 是否还要设置标志？
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
	if (!m_bHistoryTimeSale && (TimeServer < TimeInit || TimeServer > TimeEnd))	// 不是同一天， 需要重新算时间和昨收价等
	{
		// 非交易时段的处理
		
		// 获取该商品走势相关的数据
		CMarketIOCTimeInfo RecentTradingDay;
		if (m_pMerchXml->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, m_pMerchXml->m_MerchInfo))
		{
			m_TrendTradingDayInfo.Set(m_pMerchXml, RecentTradingDay);
			m_TrendTradingDayInfo.RecalcPrice(*m_pMerchXml);
			m_TrendTradingDayInfo.RecalcHold(*m_pMerchXml);

			// 判断现有数据是否属于获得的临近交易日内交易时段的数据，不是，则清空该数据
			if ( m_aTicksShow.GetSize() > 0 )
			{
				// 分笔的时间应当大于当天市场的初始化时间 并且小于 其End时间
				if ( RecentTradingDay.m_TimeInit >  m_aTicksShow[0].m_TimeCurrent || RecentTradingDay.m_TimeEnd < m_aTicksShow[0].m_TimeCurrent )
				{
					// 交易日时间比现有数据早 或者 比现有数据晚
					// 先清掉当前所有数据
					DeleteTableContent(true);
				}
			}
		}
		else
		{
			//ASSERT(0);
		}
	}
	
	// 请求指定时间的日K线， 以便获取当前显示走势图的昨收价， 今开价
	{
		ASSERT(m_TrendTradingDayInfo.m_bInit);
		
		// 
		CGmtTime TimeDay = m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose.m_Time;
		SaveDay(TimeDay);
		
		CMmiReqMerchKLine info;
		info.m_eKLineTypeBase	= EKTBDay;
		info.m_iMarketId		= m_pMerchXml->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		info.m_eReqTimeType		= ERTYFrontCount;		// 为防止跨天，请求昨天&今天
		info.m_TimeStart		= TimeDay;
		//info.m_TimeEnd			= TimeDay;
		info.m_iFrontCount			= 2;			
		DoRequestViewData(info);
	}
	
	// 发RealtimePrice请求, 为取昨收价
	{
		CMmiReqRealtimePrice Req;
		Req.m_iMarketId			= m_pMerchXml->m_MerchInfo.m_iMarketId;
		Req.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		DoRequestViewData(Req);
	}
	
	// 请求该商品的数据
	{
		// 普通情况下，申请历史分笔数据，这个估计有蛮多
		CMmiReqMerchTimeSales info;
		info.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		
		info.m_eReqTimeType	= ERTYSpecifyTime;
		info.m_TimeEnd		= TimeEnd;
		info.m_TimeStart	= TimeInit;		// 初始化市场时 - 第二天的初始化前一刻 (24h)
		
		DoRequestViewData(info);
	}
	
	// 发RealtimeTick请求
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
				m_GridCtrl.EnsureVisible(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());		// 不能保证总是第一个可见
			}
			DeleteTableContent();		// 清除现有数据
		}
	}
	// 错过这次时机就只剩下merchchanged时机
}

bool32 CIoViewFenJiaBiao::GetCurrentPerfectWidth( OUT int32 &iWidth )
{
	// 
	if ( m_iPreferWidth < 1 )
	{
		// 计算一个基本列所需宽度
		CString StrBase;
		for ( int32 i=0; i < 6 ; i++ )
		{
			StrBase += KBaseChar;
		}
		// 4字符

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
		// 尝试v mode

		// 判断是否是多列, 多列有表头
		bool32 bHasFixedRow = IsInMultiColumnMode();
		int32 iColCount = m_GridCtrl.GetColumnCount();
		const int32 iFixedRow = m_GridCtrl.GetFixedRowCount();
		const int32 iBaseCount = m_GridCtrl.GetColumnCount() / (int32)KGridCtrlBasicColumnCount;	// 多少个基本列
		ASSERT( iBaseCount > 0 );
		if ( iColCount > KGridCtrlBasicColumnCount )
		{
			if ( iFixedRow != 1 )
			{
				return;	// 此时为状态不同步，不需要处理此时的请求
			}
		}
		else if ( iColCount < KGridCtrlBasicColumnCount )
		{
			return;	// 无效表格
		}
		else
		{
			if ( iFixedRow != 0 )
			{
				return; // 此时为状态不同步，不需要处理此时的请求
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
			// 表头文字 
			switch ( item.col%((int32)KGridCtrlBasicColumnCount) )
			{
			case 0:
				{
					item.strText = _T("价格");
					if ( NULL != pCell )
					{
						pCell->SetCellRectDrawFlag(DR_TOP_SOILD |DR_BOTTOM_SOILD);
					}
				}
				break;
			case 1:
				{
					// 指数成交额，其它成交量
					if ( !bIsExp )
					{
						item.strText = _T("成交量");
					}
					else
					{
						item.strText = _T("成交额");
					}
					if ( NULL != pCell )
					{
						pCell->SetCellRectDrawFlag(DR_TOP_SOILD |DR_BOTTOM_SOILD);
					}
				}
				break;
			case 2:
				{
					item.strText = _T("比例");
					item.nFormat = DT_SINGLELINE |DT_VCENTER |DT_LEFT;
					if ( NULL != pCell )
					{
						pCell->SetCellRectDrawFlag(DR_TOP_SOILD |DR_BOTTOM_SOILD);
					}
				}
				break;
			case 3:
				{
					item.strText = _T("竞买率%");
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
			// 计算整个可见区域可用的数据，按照左列竖直排列到次左列
			CCellRange rangeVisible = m_GridCtrl.GetVisibleNonFixedCellRange();
			if ( !rangeVisible.IsValid() || rangeVisible.GetRowSpan() < 1 )
			{
				return;	// 没有要显示的数据
			}

			// 实际要显示的数据 rowSpan * iBaseCount
			// 每一base列的开始数据startPos为 (minRow-fixedRow)*iBaseCount + rowSpan*(col/KBaseColumn)
			// 数据量为 rowSpan
			// 检查该数据是否在可显示范围内
			const int32 iDataCount = rangeVisible.GetRowSpan();
			const int32 iDataStartPos = (rangeVisible.GetMinRow()-iFixedRow)*iBaseCount + iDataCount*(item.col/((int32)KGridCtrlBasicColumnCount));

			if ( !rangeVisible.InRange(item.row, item.col) )
			{
				// 不必显示了
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
						// 价格
						item.rtTextPadding = CRect(15, 0, 0,0);
						item.crFgClr = GetIoViewColor(ts.m_eColor);
						item.strText = Float2String(ts.m_fPrice, m_iDecSave, false, false);
						item.nFormat = DT_SINGLELINE |DT_VCENTER |DT_LEFT;
						item.lfFont = fontNumber;
						
					}
					break;
				case 1:
					{
						// 成交量, 指数成交额
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
						// 比例 显示■■■■■■ 样式
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
						// 竞买率
						float fPec = 0.0;
						item.nFormat = DT_SINGLELINE |DT_VCENTER |DT_RIGHT;
						item.rtTextPadding = CRect(0, 0, 15,0);
						if ( bIsExp )
						{
							if ( ts.m_fAmountTotal != 0.0f )
							{
								fPec = 100*ts.m_fAmountBuy/ts.m_fAmountTotal;
							}
							// 指数不显示竞买率
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
				ASSERT( iDataPos >= 0 );	// 还存在部分多余的空行是正常情况
				ASSERT( item.row < m_GridCtrl.GetRowCount() );
				// 无数据下
				switch ( item.col%((int32)KGridCtrlBasicColumnCount) )
				{
				case 0:
					{
						// 价格
					}
					break;
				case 1:
					{
						// 成交量, 指数成交额
					}
					break;
				case 2:
					{
						// 比例 显示■■■■■■ 样式，共10等分
					}
					break;
				case 3:
					{
						// 竞买率
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
	return m_GridCtrl.GetColumnCount()/((int32)KGridCtrlBasicColumnCount) > 1;	// 有多个基本列
}

void CIoViewFenJiaBiao::CalcTableData()
{
	// 清除除源数据以外的数据和显示
	// 记住此次的表格
	CCellID idCell = m_GridCtrl.GetFocusCell();

	m_GridCtrl.DeleteNonFixedRows();
	ClearShowData();

	if ( NULL == m_pMerchXml )
	{
		return;
	}

	// 取昨收
	float fPrevClose = m_TrendTradingDayInfo.GetPrevReferPrice();
	if ( 0.0f == fPrevClose )
	{
		return;	// 无昨收
	}

	// 计算
	m_aPricesSort.SetSize(0, 200);
	for ( int32 i=0; i < m_aTicksShow.GetSize() ; i++ )
	{
		const CTick &tick = m_aTicksShow[i];
		float fPrice = tick.m_fPrice;

		// 插入有序数组, 按照由高价到低组织
		bool32 bAdded = FALSE;
		int32 iSort=0;
		T_TickStatistic *pDst = NULL;
		for ( iSort = 0; iSort < m_aPricesSort.GetSize() ; iSort++ )
		{
			if ( m_aPricesSort[iSort].m_fPrice == fPrice )
			{
				pDst = &m_aPricesSort[iSort];	// 直接操作
				bAdded = TRUE;
				break;
			}
			else if ( fPrice > m_aPricesSort[iSort].m_fPrice )
			{
				// 插入价格高的
				break;
			}
		}
		if ( !bAdded )
		{
			// 需要添加新的统计信息
			// 新增
			T_TickStatistic ts;
			ZeroMemory(&ts, sizeof(ts));
			if ( fPrice > fPrevClose )	// 绝对判断?
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

			pDst = &m_aPricesSort[iSort];	// 操作地址
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

	// 重新计算表格
	RecalcTableRowColumn();
	
	// 还原cellid
	if ( !m_GridCtrl.IsValid(idCell)  )
	{
		idCell = m_GridCtrl.GetTopleftNonFixedCell();
	}
	m_GridCtrl.SetFocusCell(idCell);

	m_GridCtrl.Refresh();	// 重显示
}
