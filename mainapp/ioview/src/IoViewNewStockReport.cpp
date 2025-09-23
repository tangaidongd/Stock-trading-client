#include "stdafx.h"
#include "IoViewNewStockReport.h"
#include "FontFactory.h"
#include "IoViewManager.h"

using std::string;

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace
{
	#define NotifyMsg(wNotifyCode,id,memberFxn)\
	if(phdr->code == wNotifyCode && phdr->idFrom == id)\
	{\
		memberFxn(phdr,pResult);\
	}

	#define GRID_HEAD_COLOR		RGB(0x23, 0x22, 0x28)	// 表头们的颜色
	#define GRID_BORDER_COLOR	RGB(0x25, 0x24, 0x2A)
	#define GRID_HEAD_HEIGHT	35	// 表头的高度
	#define GRID_ITEM_HEIGHT	35  // 表格的行高


	#define GRID_BK_COLOR			 RGB(0x23, 0x22, 0x28)
	#define GRID_TEXT_COLOR			 RGB(160,160,160)

	#define GRID_KEEP_COLOR1		 RGB(230, 230, 230)
	#define GRID_KEEP_COLOR2		 RGB(200, 200, 200)
	#define GRID_RISE_COLOR			 RGB(233, 70, 70)
	#define GRID_FALL_COLOR			 RGB(51, 204, 102)
	#define GRID_VOLUME_COLOR        RGB(0, 255, 255)
	#define GRID_CODE_COLOR			 RGB(245, 253, 170)

	enum
	{
		EGID_NEWSTOCK = 0x550,
	};


	const static int s_kiXScrollBarNewStockHID = 0x498;										// 新股水平
	const static int s_kiXScrollBarNewStockVID = 0x499;										// 新股竖直
	

	const int32 KRequestViewDataCurrentVisibleRowTimerId		= 10086;					// 需要更新的时候, 重新计算当前可见的行.请求实时推送.(这个定时器是在发出更新时机后100ms 再请求. 是一次性的)
	const int32 KRequestViewDataCurrentVisibleRowTimerPeriod	= 50;

	enum E_NewStockGridHeadType
	{
		// 成交部分
		ENSGHTRowNo = 0,			// 行号
		ENSGHTMerchCode,			// 代码
		ENSGHTMerchName,			// 名称
		ENSGHTRange,				// 幅度%
		ENSGHTPriceNew,				// 最新价
		ENSGHTListedDate,			// 上市日
		ENSGHTVolumeTotal,			// 成交量
		ENSGHTChangeRate,			// 换手率
		ENSGHTMarketWinRate,		// 市盈率
		ENSGHTAllMarketValue,		// 总市值
		ENSGHTIndustry,				// 所属行业
		ENSGHTTotalRise,			// 上市至今涨幅
		ENSGHTFirstDayRise,			// 首日涨幅
		ENSGHTActualRise,			// 实际涨幅

		ENSGHTNetPubTotal,			// 网上发行总数（万股）
		ENSGHTPurchaseCode,			// 申购代码
		ENSGHTPurchaseDate,			// 申购日期	
		ENSGHTPubWinRate,			// 发行市盈率
		ENSGHTIndustryWinRate,		// 行业市赢率
		ENSGHTPubTotal,				// 发行总数
		ENSGHTSignDate,				// 中签公布日期
		ENSGHTPubPrice,				// 发行价
		ENSGHTPurchaseLimit,		// 申购上限（万股）
		ENSGHTFirstDayClosePrice,	// 首日收盘价
	};

	struct T_NewStockGridHead 
	{
		int		iHeadType;
		TCHAR	szName[30];
		int		iColWidth;
		UINT	nFmt;
	};

#ifndef ArraySize
#define ArraySize(Array) (sizeof(Array)/sizeof(Array[0]))
#endif



	static const T_NewStockGridHead  s_kaListedStockHeader[] = 
	{
		{ENSGHTRowNo,			_T("序"), 60, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTMerchCode,		_T("代码"), 80, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTMerchName,		_T("名称"), 110, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTRange,			_T("涨幅%"), 60, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTPriceNew,		_T("现价"), 110, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTListedDate,		_T("上市日"), 110, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTVolumeTotal,		_T("总手"), 80, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTChangeRate,		_T("换手%"), 80, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTMarketWinRate,	_T("市盈率"), 80, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTAllMarketValue,	_T("总市值"), 110, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTIndustry,		_T("所属行业"), 110, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTTotalRise,		_T("上市至今涨幅"), 110, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTFirstDayRise,	_T("首日涨幅"), 110, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTActualRise,		_T("实际涨幅"), 110, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
	};
	static const int s_kiListedStockHeaderCount = ArraySize(s_kaListedStockHeader);

	static const T_NewStockGridHead  s_kaUnlistedStockHeader[] = 
	{
		{ENSGHTRowNo,				_T("序"), 60, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTMerchCode,			_T("代码"), 80, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTMerchName,			_T("名称"), 110, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
//		{ENSGHTNetPubTotal,			_T("网上发行总数(万股)"), 140, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTPurchaseDate,		_T("申购日"), 100, DT_CENTER|DT_SINGLELINE|DT_VCENTER},	
		{ENSGHTPurchaseCode,		_T("申购代码"), 120, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTListedDate,			_T("上市日"), 100, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTPubTotal,			_T("发行量(万)"), 100, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTPurchaseLimit,		_T("申购限额(万股)"), 150, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTPubWinRate,			_T("发行市盈率"), 130, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ENSGHTSignDate,			_T("中签公布日"), 130, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
//		{ENSGHTIndustryWinRate,		_T("行业市赢率"), 80, DT_CENTER|DT_SINGLELINE|DT_VCENTER},		
//		{ENSGHTPubPrice,			_T("发行价"), 60, DT_CENTER|DT_SINGLELINE|DT_VCENTER},		
//		{ENSGHTFirstDayClosePrice,	_T("首日收盘价"), 110, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
	};
	static const int s_kiUnlistedStockHeaderCount = ArraySize(s_kaUnlistedStockHeader);


	struct T_NewStockGridHeadGroup 
	{
		TCHAR						szGroupName[30];
		const T_NewStockGridHead	*pstGridHead;
		int							iGridHeadCount;
		int							iFixedColumnCount;
		COLORREF					clrFixedRow;
	};


	static const T_NewStockGridHeadGroup s_kstNewStockGridHeadGroupListed = 
	{_T("已上市新股"), s_kaListedStockHeader, s_kiListedStockHeaderCount, 0, RGB(0,0,0)};

	static const T_NewStockGridHeadGroup s_kstNewStockGridHeadGroupUnlisted = 
	{_T("未上市新股"), s_kaUnlistedStockHeader, s_kiUnlistedStockHeaderCount, 0, RGB(0,0,0)};
}

const T_NewStockGridHeadGroup& GetNewStockGridHeadGroup(bool bListedStock)
{
	return (bListedStock ? s_kstNewStockGridHeadGroupListed : s_kstNewStockGridHeadGroupUnlisted);
}



IMPLEMENT_DYNCREATE(CIoViewNewStockReport, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewNewStockReport, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewNewStockReport)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////
CIoViewNewStockReport::CIoViewNewStockReport() : CIoViewBase()
{
	m_pParent		= NULL;
	m_rectClient	= CRect(-1,-1,-1,-1);
	m_iAllNewStockHeight = 0;

	m_iAllColumnWidth    = 0;

	m_eReportHeadType = ENSHT_ListedStock;
	
//	CNewStockManager::Instance().AddNewStockNotify(this);
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewNewStockReport::~CIoViewNewStockReport()
{
	
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CIoViewNewStockReport::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if ( !IsWindowVisible() )
	{
		return;
	}

}

BOOL CIoViewNewStockReport::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类
	LPNMHDR phdr = (LPNMHDR)lParam;
 	NotifyMsg(TCN_SELCHANGE, 0x9999, OnSelChange)
 	NotifyMsg(GVN_SELCHANGED, EGID_NEWSTOCK,OnGridSelRowChanged) 
 	NotifyMsg(NM_RCLICK, EGID_NEWSTOCK,OnGridRButtonDown)
 	NotifyMsg(NM_CLICK, EGID_NEWSTOCK,OnGridLButtonDown)
 	NotifyMsg(NM_DBLCLK, EGID_NEWSTOCK, OnGridDblClick)
 	NotifyMsg(GVN_COLWIDTHCHANGED, EGID_NEWSTOCK, OnGridColWidthChanged)
 	NotifyMsg(GVN_KEYDOWNEND, EGID_NEWSTOCK, OnGridKeyDownEnd)
 	NotifyMsg(GVN_KEYUPEND, EGID_NEWSTOCK, OnGridKeyUpEnd)

	return __super::OnNotify(wParam, lParam, pResult);
}

BOOL CIoViewNewStockReport::PreTranslateMessage(MSG* pMsg)
{
	if ( VK_RIGHT == pMsg->wParam )
	{
		m_XBarNewStockV.ScrollRight();
		return TRUE;
		}
	else if ( VK_LEFT == pMsg->wParam )
	{
		m_XBarNewStockH.ScrollLeft();
		return TRUE;
	}
	else if ( VK_RIGHT == pMsg->wParam )
	{
		m_XBarNewStockH.ScrollRight();
		return TRUE;
	}
	else if ( VK_LEFT == pMsg->wParam )
	{
		m_XBarNewStockH.ScrollLeft();
		return TRUE;
	}

	return CIoViewBase::PreTranslateMessage(pMsg);
}

BOOL CIoViewNewStockReport::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return CIoViewBase::OnCommand(wParam, lParam);
}

int CIoViewNewStockReport::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CIoViewBase::OnCreate(lpCreateStruct);
	
	WCHAR *pFontName = gFontFactory.GetExistFontName(L"宋体");
	
	VERIFY(m_Font.CreateFont(
		-14,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		pFontName));		   // lpszFacename	//...

	// 获取表格类型
	CIoViewManager *pIoViewManage = GetIoViewManager();
	if (pIoViewManage)
	{
		m_eReportHeadType = (E_NewStockReportHeadType)pIoViewManage->GetManagerTopBarInfo().m_iTopBarType;
	}

	// 
	CreateListedStockTable(&m_Font);
	
	RequestViewDataCurrentVisibleRowAsync();

	return 0;
}

void CIoViewNewStockReport::CreateListedStockTable(CFont *pFont)
{
	CGridCtrlNormal *pGrid = &m_GridNewStock;
	CRect rcGrid(0,0,0,0);
	pGrid->Create(rcGrid, this, EGID_NEWSTOCK);
	pGrid->ShowWindow(SW_SHOW);

 	pGrid->SetDonotScrollToNonExistCell(GVL_BOTH);
 	pGrid->SetDefaultCellType(RUNTIME_CLASS(CGridCellTrade));
 	pGrid->SetListMode(TRUE);
 	pGrid->SetSingleRowSelection(TRUE);
 	pGrid->EnableToolTips(FALSE);
 	pGrid->EnableTitleTips(FALSE);
 	pGrid->SetShowSelectWhenLoseFocus(FALSE);
 	pGrid->SetGridBkColor(GRID_BK_COLOR);
 	pGrid->SetFixColBkColor(GRID_BK_COLOR);
 	pGrid->SetRemoveSelectClickBlank(TRUE);
// 	pGrid->SetListHeaderCanClick(FALSE);
 
 	pGrid->GetDefaultCell(FALSE, FALSE)->SetBackClr(GRID_BK_COLOR);
 	pGrid->GetDefaultCell(FALSE, FALSE)->SetTextClr(GRID_TEXT_COLOR);
 	pGrid->GetDefaultCell(TRUE, FALSE)->SetBackClr(GRID_HEAD_COLOR);
 	pGrid->GetDefaultCell(TRUE, FALSE)->SetTextClr(GRID_TEXT_COLOR);
	pGrid->GetDefaultCell(FALSE, TRUE)->SetBackClr(GRID_HEAD_COLOR);
	pGrid->GetDefaultCell(FALSE, TRUE)->SetTextClr(GRID_TEXT_COLOR);
 	pGrid->ShowGridLine(false);
  	pGrid->SetDrawSelectedCellStyle(GVSDS_DEFAULT);
  	pGrid->SetDrawFixedCellGridLineAsNormal(TRUE);
 	pGrid->EnableSelection(TRUE);
	pGrid->SetFrameFocusCell(FALSE);
	pGrid->SetHeaderSort(TRUE);
	pGrid->SetRowResize(FALSE);

	// 设置字体
	pGrid->SetFont(pFont);

	pGrid->SetDefCellMargin(2);

	// 滚动条
	CXScrollBar *pHBar = &m_XBarNewStockH;
	pHBar->Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, this, CRect(0,0,0,0), s_kiXScrollBarNewStockHID);

	CXScrollBar *pVBar = &m_XBarNewStockV;
	pVBar->Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, this, CRect(0,0,0,0), s_kiXScrollBarNewStockVID);

	pHBar->SetOwner(pGrid);
	pVBar->SetOwner(pGrid);
	pHBar->SetBorderColor(RGB(0x0F, 0x0E, 0x14));
	pVBar->SetBorderColor(RGB(0x0F, 0x0E, 0x14));


	pHBar->UseAppRes();
	pVBar->UseAppRes();

	// 设置水平滚动条图片
	pHBar->SetScrollBarLeftArrowH(IDB_HORIZONTAL_SCROLLBAR_LEFTARROW);
	pHBar->SetScrollBarRightArrowH(IDB_HORIZONTAL_SCROLLBAR_RIGHTARROW);
	pHBar->SetScrollBarChannelH(IDB_HORIZONTAL_SCROLLBAR_CHANNEL);
	pHBar->SetScrollBarThumbH(IDB_HORIZONTAL_SCROLLBAR_THUMB);
	pHBar->SetScrollBarThumbNoColorH(IDB_HORIZONTAL_SCROLLBAR_THUMB_NO_COLOR);
	// 设置垂直滚动条图片
	pVBar->SetScrollBarLeftArrowH(IDB_HORIZONTAL_SCROLLBAR_LEFTARROW);
	pVBar->SetScrollBarRightArrowH(IDB_HORIZONTAL_SCROLLBAR_RIGHTARROW);
	pVBar->SetScrollBarChannelH(IDB_HORIZONTAL_SCROLLBAR_CHANNEL);
	pVBar->SetScrollBarThumbH(IDB_HORIZONTAL_SCROLLBAR_THUMB);
	pVBar->SetScrollBarThumbNoColorH(IDB_HORIZONTAL_SCROLLBAR_THUMB_NO_COLOR);

	pGrid->SetScrollBar(pHBar, pVBar);

	// 表头
	bool bListedStock = (ENSHT_ListedStock == m_eReportHeadType);
	const T_NewStockGridHeadGroup &stNewStockGridHeadGroup = GetNewStockGridHeadGroup(bListedStock);

	const int iHeaderCount = stNewStockGridHeadGroup.iGridHeadCount;
	const int iFixedColCount = stNewStockGridHeadGroup.iFixedColumnCount;
	COLORREF clrFixedRow = stNewStockGridHeadGroup.clrFixedRow;
	const T_NewStockGridHead *pstHeader = stNewStockGridHeadGroup.pstGridHead;
	if ( pstHeader!=NULL && iHeaderCount>0 )
	{
		pGrid->SetFixedRowCount(1);
		pGrid->SetColumnCount(iHeaderCount);
		pGrid->SetFixedColumnCount(iFixedColCount);
		
		//
		m_iAllColumnWidth = 0;

		for ( int iCol=0; iCol<iHeaderCount; ++iCol )
		{
			const T_NewStockGridHead &stHeader = pstHeader[iCol];
			pGrid->SetColumnWidth(iCol, stHeader.iColWidth);

			m_iAllColumnWidth += stHeader.iColWidth;

			CGridCellBase *pCell = pGrid->GetCell(0, iCol);
			ASSERT( pCell );
			pCell->SetText(stHeader.szName);
			pCell->SetFormat(stHeader.nFmt);
			pCell->SetData(stHeader.iHeadType);

			if ( CLR_DEFAULT!=clrFixedRow )
			{
				pCell->SetTextClr(clrFixedRow);
			}
		}
	}
	pGrid->AutoSizeRows();
	pGrid->SetRowHeight(0, GRID_HEAD_HEIGHT);
	pGrid->SetBorderColor(GRID_BORDER_COLOR);
	pGrid->BringWindowToTop();
}

void CIoViewNewStockReport::RecalcLayout( bool bNeedDraw )
{
	const int iHorzHeight = 12;			// 滚动条高度
	CRect rc(0,0,0,0);
	GetClientRect(rc);
	CRect rectListNewStock(rc);

	CRect rcNewStockHBar(0,0,0,0);
	if ( rectListNewStock.Width() < m_iAllColumnWidth )
	{
		CRect rcHorz(rectListNewStock);
		rcHorz.top = rectListNewStock.bottom - iHorzHeight;
		rectListNewStock.bottom = rcHorz.top;
		rcNewStockHBar = rcHorz;
	}

	//垂直滚动条
	CRect rcNewStockVBar(0,0,0,0);
	if ( (rectListNewStock.Height()< m_iAllNewStockHeight)&& m_iAllNewStockHeight>20 )
	{
		CRect rcVer(rectListNewStock);
		rcVer.left = rectListNewStock.right - iHorzHeight;
		rectListNewStock.right = rcVer.left;
		rcNewStockVBar = rcVer;
	}

	if (m_GridNewStock.m_hWnd)
	{
		m_GridNewStock.MoveWindow(rectListNewStock);
	}
	if (m_XBarNewStockH.m_hWnd)
	{
		m_XBarNewStockH.SetSBRect(rcNewStockHBar);
	}
	if (m_XBarNewStockV.m_hWnd)
	{
		m_XBarNewStockV.SetSBRect(rcNewStockVBar);
	}

	if ( bNeedDraw )
	{
		Invalidate(TRUE);
	}
}

// 新股表格填充
void CIoViewNewStockReport::ReloadGridNewStock()
{
	CGridCtrlNormal *pGrid = &m_GridNewStock;

	bool bListedStock = (ENSHT_ListedStock == m_eReportHeadType);
	const T_NewStockGridHeadGroup &stGridHeadGroup = GetNewStockGridHeadGroup(bListedStock);

	CCellID stCellTopLeft = pGrid->GetVisibleNonFixedCellRange().GetTopLeft();
	CCellRange cellRangeSel = pGrid->GetSelectedCellRange();

	const NewStockArray &aNewStock = CNewStockManager::Instance().GetNewStockInfo(bListedStock);
	
	int iNewSize = aNewStock.size();
	int iOldSize = pGrid->GetRowCount() - pGrid->GetFixedRowCount();

	pGrid->SetRowCount(iNewSize+pGrid->GetFixedRowCount());

	for ( int i=0; i<iNewSize; ++i )
	{
		const T_NewStockInfo &stOut = aNewStock.at(i);
		int iRow = i + pGrid->GetFixedRowCount();

		UpdateOneNewStockRow(iRow, stOut);
	}

	if (iNewSize > iOldSize)
	{
		pGrid->SetScrollPos32(SB_VERT,0);
		pGrid->SetScrollPos32(SB_HORZ,0);

		//第一次查寻不用设置选中
		if (iOldSize > 0)
		{
			pGrid->SetSelectedSingleRow(1);
		}
	}
	else
	{
		pGrid->SetSelectedRange(cellRangeSel);
	}
	
	//
	ResetGridFont();

	pGrid->Refresh();

	// 新股总记录高度
	m_iAllNewStockHeight = pGrid->GetRowCount() * pGrid->GetRowHeight(0);
}

// 新股一行数据显示
void CIoViewNewStockReport::UpdateOneNewStockRow( int iRow, const T_NewStockInfo &stOut)
{
	CGGTongDoc *pDoc = (CGGTongDoc *)AfxGetDocument();
	
	CMerch* pMerch = NULL;
	if ( m_pAbsCenterManager )
	{
		m_pAbsCenterManager->GetMerchManager().FindMerch(stOut.StrCode, pMerch);
	}
	
	float fPrevReferPrice = 0.0;
	if (pMerch && pMerch->m_pRealtimePrice)
	{
		fPrevReferPrice = pMerch->m_pRealtimePrice->m_fPricePrevClose;
		if (ERTFuturesCn == pMerch->m_Market.m_MarketInfo.m_eMarketReportType)	// 国内期货使用昨结算作为参考价格
		{
			fPrevReferPrice = pMerch->m_pRealtimePrice->m_fPricePrevAvg;
		}
	}

	CGridCtrlNormal *pGrid = &m_GridNewStock;
	bool bListedStock = (ENSHT_ListedStock == m_eReportHeadType);
	const T_NewStockGridHeadGroup &stGridHeadGroup = GetNewStockGridHeadGroup(bListedStock);

//	COLORREF clrRise, clrFall, clrKeep;

	//pGrid->SetRowHeight(iRow, GRID_ITEM_HEIGHT);
	
	// 表头
	const T_NewStockGridHead *pstHeader = stGridHeadGroup.pstGridHead;
	const int iHeaderCount = stGridHeadGroup.iGridHeadCount;
	if ( pstHeader!=NULL && iHeaderCount>0 )
	{
		for ( int iCol=0; iCol<iHeaderCount; ++iCol )
		{
			const T_NewStockGridHead &stHeader = pstHeader[iCol];
			CGridCellBase *pCell = pGrid->GetCell(iRow, iCol);
			if ( !pCell )
			{
				continue;
			}
			
			// 设置水平分割线
//			pCell->SetHSeparatoLine(true);

			if ( pCell->IsFixedCol() )
			{
				pCell->SetTextClr(stGridHeadGroup.clrFixedRow);
			}
			
			// 设置Merch数据
			if (0 == iCol)
			{
				pCell->SetData((LPARAM)pMerch);
			}

			if (1 == iCol)
			{
				pCell->SetData((LPARAM)&stOut);
			}

			pCell->SetFormat(stHeader.nFmt);

			CString StrText = _T("-");

			switch ( stHeader.iHeadType )
			{
			case ENSGHTRowNo:				// 行号
				{
					StrText.Format(L"%d", iRow);
					pCell->SetTextClr(GRID_KEEP_COLOR2);
					pCell->SetText(StrText);
				}
				break;
			case ENSGHTMerchCode:			// 代码
				{
					pCell->SetTextClr(GRID_KEEP_COLOR2);
					pCell->SetText(stOut.StrCode);
				}
				break;
			case ENSGHTMerchName:			// 名称
				{
					pCell->SetTextClr(GRID_CODE_COLOR);
					pCell->SetText(stOut.StrStockName);
				}
				break;
			case ENSGHTRange:				// 幅度%
				{
					float   fRisePercent = 0.0;
					if (pMerch && pMerch->m_pRealtimePrice)
					{
						if (0. != pMerch->m_pRealtimePrice->m_fPriceNew && 0. != fPrevReferPrice)
						{
							fRisePercent = ((pMerch->m_pRealtimePrice->m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
							pCell->SetText(StrText);
							StrText = Float2SymbolString(fRisePercent, 0, 2, false, false);
						}
						else if ( pMerch->m_pRealtimePrice->m_astBuyPrices[0].m_fPrice != 0.0f
							&& fPrevReferPrice != 0.0f )
						{
							// 买一价计算涨跌
							float fPriceNew =  pMerch->m_pRealtimePrice->m_astBuyPrices[0].m_fPrice;
							fRisePercent = ((fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
							StrText = Float2SymbolString(fRisePercent, 0, 2, false, false);	
						}

						if ( 0. == fRisePercent)
						{
							pCell->SetTextClr(GRID_KEEP_COLOR1);
						}
						else if (fRisePercent >0)
						{
							pCell->SetTextClr(GRID_RISE_COLOR);
						}
						else
						{
							pCell->SetTextClr(GRID_FALL_COLOR);
						}
					}
					

					pCell->SetText(StrText);
				}
				break;
			case ENSGHTPriceNew:			// 最新价
				{
					if (pMerch && pMerch->m_pRealtimePrice)
					{
						CString StrFormat = _T("");
						StrFormat.Format(L"%%.%df", pMerch->m_MerchInfo.m_iSaveDec);
						StrText.Format(StrFormat, pMerch->m_pRealtimePrice->m_fPriceNew);
						if (pMerch->m_pRealtimePrice->m_fPriceNew == pMerch->m_pRealtimePrice->m_fPriceOpen
							|| 0. == pMerch->m_pRealtimePrice->m_fPriceNew)
						{
							pCell->SetTextClr(GRID_KEEP_COLOR1);
						}
						else if (pMerch->m_pRealtimePrice->m_fPriceNew > pMerch->m_pRealtimePrice->m_fPriceOpen)
						{
							pCell->SetTextClr(GRID_RISE_COLOR);
						}
						else
						{
							pCell->SetTextClr(GRID_FALL_COLOR);
						}
					}

					pCell->SetText(StrText);
				}
				break;
			case ENSGHTListedDate:			// 上市日
				{
					if (0 != stOut.dwListedDate)
					{
						CTime tm(stOut.dwListedDate);
						StrText.Format(L"%04d%02d%02d", tm.GetYear(), tm.GetMonth(), tm.GetDay());
					}

					pCell->SetTextClr(GRID_KEEP_COLOR1);
					pCell->SetText(StrText);
				}
				break;
			case ENSGHTVolumeTotal:			// 成交量
				{
					if (pMerch && pMerch->m_pRealtimePrice)
					{
						StrText = Float2String(pMerch->m_pRealtimePrice->m_fVolumeTotal, 0, true);
					}
					
					pCell->SetTextClr(GRID_VOLUME_COLOR);
					pCell->SetText(StrText);
				}
				break;
			case ENSGHTChangeRate:			// 换手率
				{
					if (pMerch && pMerch->m_pRealtimePrice)
					{
						 StrText = Float2String(pMerch->m_pRealtimePrice->m_fTradeRate, 2, true);
					}
					
					pCell->SetTextClr(GRID_KEEP_COLOR2);
					pCell->SetText(StrText);
				}
				break;
			case ENSGHTMarketWinRate:		// 市盈率
				{
					if (pMerch && pMerch->m_pRealtimePrice)
					{
						StrText = Float2String(pMerch->m_pRealtimePrice->m_fPeRate, 2, true);
					}

					pCell->SetTextClr(GRID_KEEP_COLOR2);
					pCell->SetText(StrText);
				}
				break;
			case ENSGHTAllMarketValue:		// 总市值
				{
					if (pMerch && pMerch->m_pFinanceData && pMerch->m_pRealtimePrice)
					{	
						float fVal = 0.0;
						if ( pMerch->m_pRealtimePrice->m_fPriceNew == 0.0f )
						{
							fVal = pMerch->m_pFinanceData->m_fAllCapical * fPrevReferPrice;
						}
						else
						{
							fVal =  pMerch->m_pFinanceData->m_fAllCapical *pMerch->m_pRealtimePrice->m_fPriceNew;
						}
						fVal /= 10000.0;


						StrText = Float2String(fVal, 2, false);
					}
					
					pCell->SetTextClr(GRID_VOLUME_COLOR);
					pCell->SetText(StrText);
				}
				break;
			case ENSGHTIndustry:			// 所属行业
				{	
					if (pMerch)
					{
						CBlockCollection::BlockArray aBlocks;
						CBlockConfig::Instance()->GetBlocksByMerch(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, aBlocks);

						bool32 bFind = false;

						for ( int32 i = 0; i < aBlocks.GetSize(); i++ )
						{
							if ( aBlocks[i]->m_blockCollection.m_StrName == L"行业板块" )
							{
								StrText = aBlocks[i]->m_blockInfo.m_StrBlockName;
								bFind = true;
								break;
							}
						}

						if ( !bFind )
						{
							StrText = _T("-");
						}
					}
					
					pCell->SetTextClr(GRID_KEEP_COLOR1);
					pCell->SetText(StrText);
				}
				break;
			case ENSGHTTotalRise:			// 上市至今涨幅
				{
					if ( pMerch && pMerch->m_pRealtimePrice && 0. != stOut.fPubPrice)
					{
						float fTotalRise = 0.0;
						if ( 0. == pMerch->m_pRealtimePrice->m_fPriceNew )
						{
							fTotalRise = (pMerch->m_pRealtimePrice->m_fPricePrevClose - stOut.fPubPrice) / stOut.fPubPrice * 100;
						}
						else
						{
							fTotalRise = (pMerch->m_pRealtimePrice->m_fPriceNew - stOut.fPubPrice) / stOut.fPubPrice * 100;
						}
						
						if ( 0. == fTotalRise)
						{
							pCell->SetTextClr(GRID_KEEP_COLOR1);
						}
						else if (fTotalRise >0)
						{
							pCell->SetTextClr(GRID_RISE_COLOR);
						}
						else
						{
							pCell->SetTextClr(GRID_FALL_COLOR);
						}

						StrText = Float2String(fTotalRise, 2, false);
					}

					pCell->SetText(StrText);
				}
				break;
			case ENSGHTFirstDayRise:		// 首日涨幅
				{
					if ( 0. != stOut.fPubPrice)
					{
						float fFirstDayRise = (stOut.fFirstDayClosePrice - stOut.fPubPrice) / stOut.fPubPrice * 100;
						
						if ( 0. == fFirstDayRise)
						{
							pCell->SetTextClr(GRID_KEEP_COLOR1);
						}
						else if (fFirstDayRise >0)
						{
							pCell->SetTextClr(GRID_RISE_COLOR);
						}
						else
						{
							pCell->SetTextClr(GRID_FALL_COLOR);
						}

						StrText = Float2String(fFirstDayRise, 2, false);
					}

					pCell->SetText(StrText);
				}
				break;
			case ENSGHTActualRise:			// 实际涨幅
				{
					if ( pMerch && pMerch->m_pRealtimePrice && 0. != stOut.fFirstDayClosePrice)
					{
						float fFirstDayRise = 0.0;
						if ( 0. == pMerch->m_pRealtimePrice->m_fPriceNew )
						{
							fFirstDayRise = (pMerch->m_pRealtimePrice->m_fPricePrevClose - stOut.fFirstDayClosePrice) / stOut.fFirstDayClosePrice * 100;
						}
						else
						{
							fFirstDayRise = ( pMerch->m_pRealtimePrice->m_fPriceNew - stOut.fFirstDayClosePrice) / stOut.fFirstDayClosePrice * 100;
						}

						if ( 0. == fFirstDayRise)
						{
							pCell->SetTextClr(GRID_KEEP_COLOR1);
						}
						else if (fFirstDayRise >0)
						{
							pCell->SetTextClr(GRID_RISE_COLOR);
						}
						else
						{
							pCell->SetTextClr(GRID_FALL_COLOR);
						}

						StrText = Float2String(fFirstDayRise, 2, false);
					}

					pCell->SetText(StrText);
				}	
				break;
			case ENSGHTNetPubTotal:			// 网上发行总数（万股）
				{
					StrText.Format(L"%d", stOut.iNetPubTotal);
					pCell->SetText(StrText);
				}
				break;
			case ENSGHTPurchaseCode:		// 申购代码
				{
					pCell->SetTextClr(GRID_CODE_COLOR);
					pCell->SetText(stOut.StrPurchaseCode);
				}
				break;
			case ENSGHTPurchaseDate:		// 申购日期	
				{
					CTime tm(stOut.dwPurchaseDate);
					StrText.Format(L"%04d%02d%02d", tm.GetYear(), tm.GetMonth(), tm.GetDay());
					pCell->SetTextClr(GRID_KEEP_COLOR1);
					pCell->SetText(StrText);
				}
				break;
			case ENSGHTPubWinRate:			// 发行市盈率
				{
					StrText.Format(L"%0.2f", stOut.fPubWinRate);
					pCell->SetTextClr(GRID_KEEP_COLOR1);
					pCell->SetText(StrText);
				}
				break;
			case ENSGHTIndustryWinRate:		// 行业市赢率
				{
					StrText.Format(L"%0.2f", stOut.fIndustryWinRate);
					pCell->SetText(StrText);
				}
				break;
			case ENSGHTPubTotal:			// 发行总数
				{
					pCell->SetTextClr(GRID_VOLUME_COLOR);
					StrText.Format(L"%d", stOut.iPubTotal);
					pCell->SetText(StrText);
				}
				break;
			case ENSGHTSignDate:			// 中签公布日期
				{	
					CTime tm(stOut.dwSignDate);
					StrText.Format(L"%04d%02d%02d", tm.GetYear(), tm.GetMonth(), tm.GetDay());
					pCell->SetTextClr(GRID_KEEP_COLOR1);
					pCell->SetText(StrText);
				}
				break;
			case ENSGHTPubPrice:			// 发行价
				{
					StrText.Format(L"%0.2f", stOut.fPubPrice);
					pCell->SetText(StrText);
				}
				break;
			case ENSGHTPurchaseLimit:		// 申购上限（万股）
				{
					pCell->SetTextClr(GRID_VOLUME_COLOR);
					StrText.Format(L"%0.2f", stOut.fPurchaseLimit);
					pCell->SetText(StrText);
				}
				break;
			case ENSGHTFirstDayClosePrice:	// 首日收盘价
				{
					StrText.Format(L"%0.2f", stOut.fFirstDayClosePrice);
					pCell->SetText(StrText);
				}
				break;
			default:
				break;
			}
		}
	}
}
void CIoViewNewStockReport::OnSize(UINT nType, int cx, int cy)
{
	CIoViewBase::OnSize(nType, cx, cy);
	
	RecalcLayout(false);
	
	// 加载数据
	ReloadGridNewStock();
}

void CIoViewNewStockReport::OnNewStockResp()
{
	ReloadGridNewStock();
}


bool32 CIoViewNewStockReport::FromXml(TiXmlElement * pElement)
{
	
	//
	SetFontsFromXml(pElement);

	return true;
}

CString CIoViewNewStockReport::ToXml()
{
	CString StrDefaultXml = _T("");

	return StrDefaultXml;
}

CString CIoViewNewStockReport::GetDefaultXML()
{
	CString StrDefaultXml = _T("");

	return StrDefaultXml;
}

void CIoViewNewStockReport::SetChildFrameTitle()
{

}


void CIoViewNewStockReport::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch) 		// 通知视图改变关注的商品
{
	if (m_pMerchXml == pMerch)
		return;

	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
}

//////////////////////////////////////////////////////////////////////////
void CIoViewNewStockReport::OnSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	int i = 0;
}

void CIoViewNewStockReport::OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult)
{ 
	int i = 0;
}

void CIoViewNewStockReport::OnGridLButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	int i = 0;
}

void CIoViewNewStockReport::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	int i = 0;
}

void CIoViewNewStockReport::OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{	
	int i = 0;
}

void CIoViewNewStockReport::OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// 向下键按到头了
	int i = 0;
}

void CIoViewNewStockReport::OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// 向上键按到头了
	int i = 0;
}

void CIoViewNewStockReport::OnGridSelRowChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// 查找当前版面是否有图表视图，没有就不响应, 报价列表全屏的时候也不响应
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CIoViewBase *pIoView = NULL;
	pIoView = pMainFrame->FindChartIoViewInFrame(NULL, this);
	if ( (NULL==pIoView) || pMainFrame->IsFullScreen())
	{
		return;
	}

	if ( NULL == m_pAbsCenterManager )
	{
		return;	
	}

	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	int32 iRow = pItem->iRow;
	int32 iCol = pItem->iColumn;

	// 通知商品发生了改变
	CGridCellTrade *pCell;
	
	pCell = (CGridCellTrade *)m_GridNewStock.GetCell(iRow, 0);
	if ( NULL == pCell || iRow < m_GridNewStock.GetFixedRowCount() )
	{
		return;
	}
	

	CMerch *pMerch = (CMerch *)pCell->GetData();
	if ( NULL == pMerch )
	{
		//ASSERT(0);
		return;
	}

	//
	pMainFrame->OnShowMerchInChart(pMerch, this);

	m_GridNewStock.SetFocus();
}

void CIoViewNewStockReport::RequestViewDataCurrentVisibleRowAsync()
{
	KillTimer(KRequestViewDataCurrentVisibleRowTimerId);
	SetTimer(KRequestViewDataCurrentVisibleRowTimerId, KRequestViewDataCurrentVisibleRowTimerPeriod, NULL);
}

void CIoViewNewStockReport::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == KRequestViewDataCurrentVisibleRowTimerId )
	{
		KillTimer(KRequestViewDataCurrentVisibleRowTimerId);
		RequestViewDataCurrentVisibleRow();
	}
	
	CIoViewBase::OnTimer(nIDEvent);
}

void CIoViewNewStockReport::RequestViewDataCurrentVisibleRow()
{
	//	
	UpdatePushMerchs();

	//
	int32 i = 0;
	CMmiReqRealtimePrice Req;
	CMmiRegisterPushPrice ReqPush;

	for ( i= 0; i < m_aSmartAttendMerchs.GetSize(); i++ )
	{
		CSmartAttendMerch SmartAttendMerch = m_aSmartAttendMerchs.GetAt(i);
		if ( NULL == SmartAttendMerch.m_pMerch )
		{
			continue;
		}

		if ( 0 == i )
		{
			Req.m_iMarketId			= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
			Req.m_StrMerchCode		= SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;	

			ReqPush.m_iMarketId		= Req.m_iMarketId;
			ReqPush.m_StrMerchCode	= Req.m_StrMerchCode;
		}
		else
		{
			CMerchKey MerchKey;
			MerchKey.m_iMarketId	= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
			MerchKey.m_StrMerchCode = SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;

			Req.m_aMerchMore.Add(MerchKey);					
			ReqPush.m_aMerchMore.Add(MerchKey);
		}
	}

	DoRequestViewData(Req);
	DoRequestViewData(ReqPush);

	// 请求财务数据
	CMmiReqPublicFile	reqF10;
	reqF10.m_ePublicFileType = EPFTF10;
	for ( i= 0; i < m_aSmartAttendMerchs.GetSize(); i++ )
	{
		CSmartAttendMerch SmartAttendMerch = m_aSmartAttendMerchs.GetAt(i);
		if ( NULL == SmartAttendMerch.m_pMerch || !CheckFlag(SmartAttendMerch.m_iDataServiceTypes, EDSTGeneral) ) // 不关注，不处理
		{
			continue;
		}


		reqF10.m_iMarketId			= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
		reqF10.m_StrMerchCode		= SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;						

		if ( !DoRequestViewData(reqF10) )
		{
			break;		// 如果不能请求就算了
		}
	}
}

void CIoViewNewStockReport::UpdatePushMerchs()
{
	// 只有已上市新股才添加关注
	if (ENSHT_UnlistedStock == m_eReportHeadType)
	{
		return;
	}

	m_aSmartAttendMerchs.RemoveAll();

	if ( !IsWindowVisible() )
	{
		return;
	}
	
	CGGTongDoc *pDoc = (CGGTongDoc *)AfxGetDocument();
	if (!pDoc || !pDoc->m_pAbsCenterManager)
	{
		return;
	}
	
	
	//
	if ( m_GridNewStock.GetRowCount() <= 1 )
	{
		return;
	}

	//	获取新股数据
	bool bListedStock = (ENSHT_ListedStock == m_eReportHeadType);
	const NewStockArray &aNewStock = CNewStockManager::Instance().GetNewStockInfo(bListedStock);
	int iSize = aNewStock.size();

	for (int i =0; i <iSize; ++i)
	{
		const T_NewStockInfo& stNewStock = aNewStock.at(i);
					
		CMerch *pMerch = NULL;

		if ( !pDoc->m_pAbsCenterManager->GetMerchManager().FindMerch(stNewStock.StrCode, pMerch) )
		{
			continue;
		}

		if(NULL == pMerch)
		{
			continue;
		}

		CSmartAttendMerch SmartAttendMerch;
		SmartAttendMerch.m_pMerch = pMerch;
		SmartAttendMerch.m_iDataServiceTypes = EDSTPrice;		// +F10财务数据关注 - 暂不优化
		
		SmartAttendMerch.m_iDataServiceTypes |=  EDSTGeneral;	// 非期货商品增加财务数据

		m_aSmartAttendMerchs.Add(SmartAttendMerch);
	}		
}

void CIoViewNewStockReport::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	if ( ENSHT_ListedStock != m_eReportHeadType)
	{
		return;
	}

	int iRowCnt = m_GridNewStock.GetRowCount();
	
	bool bListedStock = (ENSHT_ListedStock == m_eReportHeadType);
	const NewStockArray &aNewStock = CNewStockManager::Instance().GetNewStockInfo(bListedStock);
	int iSize = aNewStock.size();
	
	bool bUpdate = false;
	for (int iRow = 1; iRow < iRowCnt; iRow++)
	{
		CGridCellBase *pFirstCell = (CGridCellBase *)m_GridNewStock.GetCell(iRow, 0);
		CGridCellBase *pSecondCell = (CGridCellBase *)m_GridNewStock.GetCell(iRow, 1);
		ASSERT(NULL != pFirstCell && NULL != pSecondCell);

		CMerch *pCellMerch = (CMerch*)pFirstCell->GetData();
		T_NewStockInfo &stNewStock = *(T_NewStockInfo *)pSecondCell->GetData();
		
		if ( iRow > iSize )
		{
			return;
		}

		if ( NULL != pCellMerch && pCellMerch == pMerch )
		{
			UpdateOneNewStockRow(iRow, stNewStock);
			bUpdate = true;
		}
	}

	if (bUpdate)
	{
		m_GridNewStock.Refresh();
	}
}

void CIoViewNewStockReport::ResetGridFont()
{
	//	重置所有单元格的字体
	if (NULL == m_GridNewStock.GetSafeHwnd() )
	{
		return;
	}
	

	if(0 == m_GridNewStock.GetRowCount() - m_GridNewStock.GetFixedRowCount())
	{
		return ;
	}


	for (int i =m_GridNewStock.GetFixedRowCount(); i < m_GridNewStock.GetRowCount(); ++i)
	{
		for(int j =0; j < m_GridNewStock.GetColumnCount(); ++j)
		{
			//	根据列类型设置单元格字体
			CGridCellBase *pCell = m_GridNewStock.GetCell(i, j);
			if (NULL != pCell)
			{
				CGridCtrlNormal *pGrid = &m_GridNewStock;
				bool bListedStock = (ENSHT_ListedStock == m_eReportHeadType);
				const T_NewStockGridHeadGroup &stGridHeadGroup = GetNewStockGridHeadGroup(bListedStock);

				// 表头
				const T_NewStockGridHead *pstHeader = stGridHeadGroup.pstGridHead;
				const int iHeaderCount = stGridHeadGroup.iGridHeadCount;
				if ( pstHeader!=NULL && iHeaderCount>0 )
				{
					for ( int iCol=0; iCol<iHeaderCount; ++iCol )
					{
						const T_NewStockGridHead &stHeader = pstHeader[j];
		
						switch (stHeader.iHeadType)
						{					
						case ENSGHTMerchName:
							pCell->SetFont(GetChineseRowExLF());					
							break;				
// 						case CReportScheme::E_ReportHeader::ERHMerchCode:
// 							pCell->SetFont(GetCodeRowExLF(false));				
// 							break;
						default:
							pCell->SetFont(GetDigitRowExLF(false));					
							break;
						}
					}
				}
			}			
		}
	}
}