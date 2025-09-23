// IoViewValue.cpp: implementation of the CIoViewAddUserBlock class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "IoViewAddUserBlock.h"
#include "GridCellSymbol.h"
#include "GridCellSys.h"
#include "BlockConfig.h"
#include "IoViewManager.h"
#include "PathFactory.h"
#include "XmlShare.h"
#include "BuySellPriceExchange.h"
#include "facescheme.h"
#include "DlgUserBlockAddMerch.h"
#include "DlgImportUserBlockBySoft.h"
#include "IoViewManager.h"
#include "IoViewReport.h"
#include "GGTongView.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//
static const int KiTimerIdFiniance		= 1;
static const int KiTimerPeriodFiniance	= 1000 * 5;

#define MID_BUTTON_USER_BLOCK_ADD_IMG	20007
#define MID_BUTTON_USER_BLOCK_IMP_IMG	20008


#define INVALID_ID              (-1)
#define ID_VERTICAL_REPORT		12345
#define ID_REPORT_GRID          10205


IMPLEMENT_DYNCREATE(CIoViewAddUserBlock, CIoViewBase) 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIoViewAddUserBlock::CIoViewAddUserBlock()
:CIoViewBase()
{
	m_eMerchKind	= ERTCount;
	m_pImgBtn       = NULL;
	m_iXButtonHovering = INVALID_ID;
}

CIoViewAddUserBlock::~CIoViewAddUserBlock()
{
	DEL(m_pImgBtn);
}

BEGIN_MESSAGE_MAP(CIoViewAddUserBlock, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewAddUserBlock)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
END_MESSAGE_MAP()

int CIoViewAddUserBlock::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CIoViewBase::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	// 获得颜色和字体
	InitialIoViewFace(this);

	//
	E_ReportType eMerchKind = GetMerchKind(m_pMerchXml);
	CreateTable(eMerchKind);

	m_iFixCol = 0;
	SetUserBlockHeadInfomationList();

	CreateAddImpBtn();

	return 0;
}

void CIoViewAddUserBlock::OnBtnResponseEvent(UINT nButtonId)
{
	if (MID_BUTTON_USER_BLOCK_ADD_IMG == nButtonId)
	{
		CDlgUserBlockAddMerch cDlgAddMerch;
		cDlgAddMerch.DoModal();
	}
	else if (MID_BUTTON_USER_BLOCK_IMP_IMG == nButtonId)
	{
		CDlgImportUserBlockBySoft cdlgImportUserBlockBySoft;
		cdlgImportUserBlockBySoft.DoModal();
	}

	CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);
}

void CIoViewAddUserBlock::CreateAddImpBtn()
{
	m_mapButton.clear();
	CPoint point(8, 0);

	m_pImgBtn = Image::FromFile(_T("image//UserBlockAdd.png"));	
	m_BtnAddUserBlk.CreateButton(L"添加自选股", CRect(0,0,0,0), this, m_pImgBtn, 3, MID_BUTTON_USER_BLOCK_ADD_IMG);
	m_BtnImpUserBlk.CreateButton(L"导入自选股", CRect(0,0,0,0), this, m_pImgBtn, 3, MID_BUTTON_USER_BLOCK_IMP_IMG);

	m_BtnAddUserBlk.SetTextOffPos(point);
	m_BtnImpUserBlk.SetTextOffPos(point);

	m_mapButton[MID_BUTTON_USER_BLOCK_ADD_IMG] = m_BtnAddUserBlk;
	m_mapButton[MID_BUTTON_USER_BLOCK_IMP_IMG] = m_BtnImpUserBlk;
}

void CIoViewAddUserBlock::OnPaint()
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

	if ( m_GridCtrl.GetSafeHwnd() )
	{
		m_GridCtrl.RedrawWindow();
	}

	CRect rcCustom;
	GetClientRect(&rcCustom);

	rcCustom .top = m_RectGrid.bottom;
	dc.FillSolidRect(&rcCustom, GetIoViewColor(ESCBackground));

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;

	memDC.FillSolidRect(rcCustom, ESCBackground);
	bmp.CreateCompatibleBitmap(&dc, rcCustom.Width(), rcCustom.Height());
	memDC.SelectObject(&bmp);
	memDC.SetBkMode(TRANSPARENT);


	Gdiplus::Graphics graphics(memDC.GetSafeHdc());
	CRect rcDis;
	map<int, CNCButton>::iterator iter;

	for (iter=m_mapButton.begin(); iter!=m_mapButton.end(); ++iter)
	{
		iter->second.DrawButton(&graphics);
		iter->second.GetRect(rcDis);
		dc.BitBlt(rcDis.left, rcDis.top, rcDis.Width(), rcDis.Height(), &memDC, rcDis.left, rcDis.top, SRCCOPY);
	}

	dc.SelectClipRgn(NULL);
	bmp.DeleteObject();
	memDC.DeleteDC();

	//UserBlockPagejump();
}

E_MerchReportField CIoViewAddUserBlock::ReportHeader2MerchReportFieldBeiJing( CReportScheme::E_ReportHeader eReportHeader,bool32& bTrans )
{
	typedef map<CReportScheme::E_ReportHeader, E_MerchReportField>	TransMap;
	static TransMap mapTrans;
	if ( mapTrans.empty() )		// 初始化转换数据
	{
		mapTrans[ CReportScheme::ERHPricePrevClose ]	= EMRFPricePrevClose;	// 昨收
		mapTrans[ CReportScheme::ERHPricePrevBalance ]	= EMRFPricePrevClose;

		mapTrans[ CReportScheme::ERHPriceOpen ]	= EMRFPriceOpen;	// 开盘价

		mapTrans[ CReportScheme::ERHPriceNew ]	= EMRFPriceNew;

		mapTrans[ CReportScheme::ERHPriceHigh ]	= EMRFPriceHigh;

		mapTrans[ CReportScheme::ERHPriceLow ]	= EMRFPriceLow;

		mapTrans[ CReportScheme::ERHVolumeCur ]	= EMRFVolumeCur;

		mapTrans[ CReportScheme::ERHVolumeTotal ]	= EMRFVolumeTotal;

		mapTrans[ CReportScheme::ERHAmount ]	= EMRFAmountTotal;

		mapTrans[ CReportScheme::ERHRiseFall ]	= EMRFRiseValue;

		mapTrans[ CReportScheme::ERHRange ]	= EMRFRisePercent;

		mapTrans[ CReportScheme::ERHSwing ]	= EMRFAmplitude;

		mapTrans[ CReportScheme::ERHVolumeRate ]	= EMRFVolRatio;

		mapTrans[ CReportScheme::ERHRate ]	= EMRFBidRatio;

		mapTrans[ CReportScheme::ERHSpeedRiseFall ]	= EMRFRiseRate;

		mapTrans[ CReportScheme::ERHBuyPrice ]	= EMRFPriceBuy;

		mapTrans[ CReportScheme::ERHSellPrice ]	= EMRFPriceSell;

		mapTrans[ CReportScheme::ERHPriceBalance ]	= EMRFPriceAverage;

		mapTrans[ CReportScheme::ERHHold ]	= EMRFHold;

		mapTrans[ CReportScheme::ERHAddPer ]	= EMRFCurHold;

		// xl 0821 新增		
		mapTrans[ CReportScheme::ERHCapitalFlow ]	= EMRFCapticalFlow;

		mapTrans[ CReportScheme::ERHChangeRate ]	= EMRFTradeRate;

		mapTrans[ CReportScheme::ERHMarketWinRate ]	= EMRFPeRate;

		mapTrans[ CReportScheme::ERHVolumeRate ]	= EMRFVolRatio;

		// 强弱度 
		mapTrans[ CReportScheme::ERHPowerDegree ]	= EMRFIntensity;

		mapTrans[ CReportScheme::ERHBuySellRate ]	= EMRFInOutRatio;
	}

	const TransMap &mapC = mapTrans;

	TransMap::const_iterator it = mapC.find(eReportHeader);
	if ( it != mapC.end() )
	{
		bTrans = true;
		return it->second;
	}

	return EMRFCount;
}

E_MerchReportField CIoViewAddUserBlock::ReportHeader2MerchReportField( CReportScheme::E_ReportHeader eReportHeader,bool32& bTrans )
{
	bTrans = false;

	E_MerchReportField eRet = EMRFPriceNew;

	switch ( eReportHeader )
	{
	case CReportScheme::ERHRowNo:				// 行号
	case CReportScheme::ERHMerchCode:			// 代码
	case CReportScheme::ERHMerchName:			// 名称
	case CReportScheme::ERHTime:				// 时间
		break;	
		// 
	case CReportScheme::ERHPricePrevClose:		// 昨收价
		eRet = EMRFPricePrevClose;
		bTrans = true;
		break;
	case CReportScheme::ERHPricePrevBalance:	// 昨结收（期货中名称， 等同于股票中昨收价）
		eRet = EMRFPricePreAvg;
		bTrans = true;
		break;
		// 
	case CReportScheme::ERHPriceOpen:			// 开盘价
		eRet = EMRFPriceOpen;
		bTrans = true;
		break;
		// 
	case CReportScheme::ERHPriceNew:			// 最新价	
		eRet = EMRFPriceNew;
		bTrans = true;
		break;		
		// 
	case CReportScheme::ERHPriceHigh:			// 最高 
		eRet = EMRFPriceHigh;
		bTrans = true;
		break;
	case CReportScheme::ERHPriceLow:			// 最低
		eRet = EMRFPriceLow;
		bTrans = true;
		break;
		//
	case CReportScheme::ERHVolumeCur:			// 现手
		eRet = EMRFVolumeCur;
		bTrans = true;
		break;

		//
	case CReportScheme::ERHVolumeTotal:			// 成交量
		eRet = EMRFVolumeTotal;
		bTrans = true;
		break;

		// 
	case CReportScheme::ERHAmount:				// 金额
		eRet = EMRFAmountTotal;
		bTrans = true;
		break;
	case CReportScheme::ERHRiseFall:			// 涨跌
		eRet = EMRFRiseValue;
		bTrans = true;
		break;
	case CReportScheme::ERHRange:				// 幅度%
		eRet = EMRFRisePercent;
		bTrans = true;
		break;
	case CReportScheme::ERHSwing:				// 振幅
		eRet = EMRFAmplitude;
		bTrans = true;
		break;
	case CReportScheme::ERHVolumeRate:			// 量比
		eRet = EMRFVolRatio;
		bTrans = true;
		break;
	case CReportScheme::ERHPowerDegree:			// 强弱度
	case CReportScheme::ERHDKBallance:			// 多空平衡
	case CReportScheme::ERHDWin:				// 多头获利
	case CReportScheme::ERHDLose:				// 多头停损
	case CReportScheme::ERHKWin:				// 空头回补
	case CReportScheme::ERHKLose:				// 空头停损
		break;
	case CReportScheme::ERHRate:				// 委比
		eRet = EMRFBidRatio;
		bTrans = true;
		break;
	case CReportScheme::ERHSpeedRiseFall:		// 快速涨跌
		eRet = EMRFRiseRate;
		bTrans = true;
		break;
	case CReportScheme::ERHChangeRate:			// 换手率
	case CReportScheme::ERHMarketWinRate:		// 市盈率		
		break;
	case CReportScheme::ERHBuyPrice:			// 买入价
		eRet = EMRFPriceBuy;
		bTrans = true;
		break;
	case CReportScheme::ERHSellPrice:			// 卖出价
		eRet = EMRFPriceSell;
		bTrans = true;
		break;
	case CReportScheme::ERHBuyAmount:			// 买入量
	case CReportScheme::ERHSellAmount:			// 卖出量
	case CReportScheme::ERHDifferenceHold:		// 持仓差
	case CReportScheme::ERHBuySellPrice:		// 买入价/卖出价
	case CReportScheme::ERHBuySellVolume:		// 买/卖量
	case CReportScheme::ERHPreDone1:			// 前成1
	case CReportScheme::ERHPreDone2:			// 前成2
	case CReportScheme::ERHPreDone3:			// 前成3
	case CReportScheme::ERHPreDone4:			// 前成4
	case CReportScheme::ERHPreDone5:			// 前成5
		break;

	case CReportScheme::ERHPriceBalance:		// 结算
		eRet = EMRFPriceAverage;
		bTrans = true;
		break;
	case CReportScheme::ERHHold:
		eRet = EMRFHold;
		bTrans = true;
		break;
	case CReportScheme::ERHAddPer:
		eRet = EMRFCurHold;
		bTrans = true;
		break;


	case CReportScheme::ERHAllCapital:
		eRet = EMRFAllStock;
		bTrans = true;
		break;
	case CReportScheme::ERHCircAsset:
		eRet = EMRFCircStock;
		bTrans = true;
		break;
	case CReportScheme::ERHAllAsset:
		eRet = EMRFAllAsset;
		bTrans = true;
		break;
	case CReportScheme::ERHFlowDebt:
		eRet = EMRFFlowDebt;
		bTrans = true;
		break;
	case CReportScheme::ERHPerFund:
		eRet = EMRFPerFund;
		bTrans = true;
		break;
	case CReportScheme::ERHBusinessProfit:
		eRet = EMRFBusinessProfit;
		bTrans = true;
		break;
	case CReportScheme::ERHPerNoDistribute:
		eRet = EMRFPerNoDistribute;
		bTrans = true;
		break;
	case CReportScheme::ERHPerIncomeYear:
		eRet = EMRFPerIncomeYear;
		bTrans = true;
		break;
	case CReportScheme::ERHPerPureAsset:
		eRet = EMRFPerPureAsset;
		bTrans = true;
		break;
	case CReportScheme::ERHChPerPureAsset:
		eRet = EMRFChPerPureAsset;
		bTrans = true;
		break;
	case CReportScheme::ERHDorRightRate:
		eRet = EMRFDorRightRate;
		bTrans = true;
		break;
	case CReportScheme::ERHCircMarketValue:
		eRet = EMRFCircMarketValue;
		bTrans = true;
		break;
	case CReportScheme::ERHAllMarketValue:
		eRet = EMRFAllMarketValue;
		bTrans = true;
		break;	
	default:
		break;
	}

	if ( !bTrans )
	{
		eRet = ReportHeader2MerchReportFieldBeiJing(eReportHeader, bTrans);		// 进行新增的查看，case晕了
	}

	return eRet;
}

void CIoViewAddUserBlock::UserBlockPagejump()
{
	// 得到这个板块
	CString StrBlockName = CUserBlockManager::Instance()->GetDefaultServerBlockName();
	T_Block* pBlock	= CUserBlockManager::Instance()->GetBlock(StrBlockName);
	if ( NULL == pBlock )
	{
		return;
	}

	// 得到板块下所有的商品
	CArray<CMerch*, CMerch*> aMerchs;
	if ( !CUserBlockManager::Instance()->GetMerchsInBlock(StrBlockName, aMerchs) )
	{
		return;
	}

	if (aMerchs.GetSize() > 0)
	{
		CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
		if (pMainFrame)
		{
			CIoViewManager  *pIoViewManager	    = GetIoViewManager();
			if (pIoViewManager->GetActiveIoView() == this && pIoViewManager->m_GuiTabWnd.GetCount() > 1)
			{
				pIoViewManager->DealTabKey();
			}
		}
	}
}

void CIoViewAddUserBlock::OnSize(UINT nType, int cx, int cy)
{
	CIoViewBase::OnSize(nType,cx,cy);
	SetSize();	
	ReSetGridHead();
}

CReportScheme::E_ReportHeader  CIoViewAddUserBlock::MerchReportField2ReportHeader(  E_MerchReportField eField,bool32& bTrans )
{
	for ( int32 i = (int32)CReportScheme::ERHRowNo; i < (int32)CReportScheme::ERHCount; i++ )
	{
		CReportScheme::E_ReportHeader eHeader = (CReportScheme::E_ReportHeader)i;
		E_MerchReportField eField2 = ReportHeader2MerchReportField (eHeader, bTrans);

		if ( bTrans && eField2 == eField )
		{
			return eHeader;
		}
	}

	bTrans = false;
	return CReportScheme::ERHRowNo;
}

void CIoViewAddUserBlock::ReSetGridHead()
{
	ASSERT(NULL != CReportScheme::Instance());

	// 设置滚动条位置:
	m_XSBHorz.ShowWindow(SW_HIDE);

	CGmtTime m_TimeLast = CGmtTime(0);
	m_bRequestViewSort = false;

	// 清除	
	m_ReportHeadInfoList.RemoveAll();

	int32 i,iSize,iTab=0;

	iSize = m_aTabInfomations.GetSize();
	for (i=0; i<iSize; i++)
	{
		if (m_aTabInfomations[i].m_Block.m_eType == T_BlockDesc::EBTUser)
		{
			iTab = i;
			break;
		}
	}


	// 得到表头信息
	if ( !SetUserBlockHeadInfomationList() )
	{
		return;
	}

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();

	//设定表头
	m_GridCtrl.SetFixedColumnCount(m_iFixCol);
	iSize = m_ReportHeadInfoList.GetSize();


	bool32 bFoundForceField = false;
	CStringArray HeaderNames;
	CArray<int32,int32> HeaderFields;	
	CArray<int32,int32> HeaderWidths;
	// 转换
	for ( i = 0; i < iSize; i++ )
	{
		CString StrText = m_ReportHeadInfoList[i].m_StrHeadNameCn;
		ASSERT(StrText.GetLength() > 0 && m_ReportHeadInfoList[i].m_iHeadWidth > 0);

		HeaderNames.Add(StrText);
		int iWidth = m_ReportHeadInfoList[i].m_iHeadWidth;
		HeaderWidths.Add(m_ReportHeadInfoList[i].m_iHeadWidth);

		bool32 bTrans;
		CReportScheme::E_ReportHeader eHeadType = CReportScheme::Instance()->GetReportHeaderEType(StrText);
		E_MerchReportField eField = ReportHeader2MerchReportField(eHeadType, bTrans);
		// 将自选股的买卖价做强制显示调换 xl 1220
		// 所有出现了指定市商市场商品的东西都一概交换显示
		bool32 bBuySellPriceExchange = false;

		if ( bBuySellPriceExchange )
		{
			if ( CReportScheme::ERHBuyPrice == eHeadType )
			{
				HeaderNames[ HeaderNames.GetUpperBound() ] = CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHSellPrice);
			}
			else if ( CReportScheme::ERHSellPrice == eHeadType )
			{
				HeaderNames[ HeaderNames.GetUpperBound() ] = CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHBuyPrice);
			}
		}
	}

	// 设置表格固定项
	m_GridCtrl.SetFixedRowCount(1);
	m_GridCtrl.SetColumnCount(HeaderNames.GetSize());

	// 设置列宽	
	for (int iCol = 0; iCol < HeaderNames.GetSize(); iCol++)
	{
		
		m_GridCtrl.SetCellType(0, iCol, RUNTIME_CLASS(CGridCellSys));
		CGridCellSys *pCellSymbol = (CGridCellSys *)m_GridCtrl.GetCell(0, iCol);
		DWORD dwFmt = DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;
		pCellSymbol->SetFormat(dwFmt);	
		pCellSymbol->SetDefaultTextColor(ESCVolume);
		CString strHead = HeaderNames.GetAt(iCol);

		pCellSymbol->SetText(strHead);
		m_GridCtrl.SetColumnWidth(iCol, HeaderWidths.GetAt(iCol));

		CReportScheme::E_ReportHeader eHeaderType = CReportScheme::Instance()->GetReportHeaderEType(strHead);
		if ( CReportScheme::ERHMerchName == eHeaderType
			|| CReportScheme::ERHMerchCode == eHeaderType
			|| CReportScheme::ERHRowNo == eHeaderType )
		{
			pCellSymbol->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		}
		pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);	
		pCellSymbol->SetFont(GetColumnExLF());		
	}

	m_GridCtrl.SetRowHeight(0, 30);
	m_GridCtrl.SetColumnWidth(0, 54);
	// 设置滚动条位置:
	m_XSBHorz.Invalidate();
	m_XSBHorz.ShowWindow(SW_SHOW);


	// 数据填充不够，就平分当前视图
	int32 iWidthGrid = m_GridCtrl.GetVirtualWidth();
	CRect rt;
	GetClientRect(rt);
	if (rt.IsRectEmpty() && NULL != m_pIoViewManager)
	{
		m_pIoViewManager->GetClientRect(rt);
	}
	//
	int32 iWidthClient = rt.Width();
	if (iWidthGrid >0 && iWidthClient > 0 && iWidthGrid < iWidthClient)
	{
		m_GridCtrl.ExpandColumnsToFit(FALSE);
	}

	//	m_GridCtrl.InsertRowBatchEnd();	
	//m_GridCtrl.AutoSizeRows();	
}

void CIoViewAddUserBlock::SetSize()
{
	CRect RectClient;
	GetClientRect(RectClient);
	m_RectGrid		= RectClient;
	m_RectGrid.bottom     = m_RectGrid.Height()/4;
	m_GridCtrl.MoveWindow(&m_RectGrid);
	//m_GridCtrl.ExpandColumnsToFit(TRUE);
	//m_GridCtrl.AutoSizeRows();	

	int32 iHeightScroll = 12;
	CRect RectHScroll(RectClient);	
	RectHScroll.top    = RectHScroll.bottom - iHeightScroll;
	m_XSBHorz.SetSBRect(RectHScroll, TRUE);
	m_XSBHorz.ShowWindow(SW_SHOW);

	CRect rcAddImpBt(RectClient);
	map<int, CNCButton>::iterator iter;
	
	// 居中显示
	
	if ( m_pImgBtn )
	{
		int iHeight = m_pImgBtn->GetHeight() / 3;
		int iWidth  = m_pImgBtn->GetWidth();
		int iStartHeightPos = (rcAddImpBt.Height() -iHeight) / 2;
		int iStartWidthPos = rcAddImpBt.Width() / 2;

		CRect rcLeftBtn, rcRightBtn;
		rcLeftBtn.top    = rcAddImpBt.top + iStartHeightPos;
		rcLeftBtn.bottom = rcLeftBtn.top + iHeight;
		rcLeftBtn.right	 =iStartWidthPos  - 20;
		rcLeftBtn.left   = rcLeftBtn.right - iWidth;
		m_mapButton[MID_BUTTON_USER_BLOCK_ADD_IMG].SetRect(rcLeftBtn);

		rcRightBtn = rcLeftBtn;
		rcRightBtn.left	 =iStartWidthPos  + 20;
		rcRightBtn.right = rcRightBtn.left + iWidth;
		m_mapButton[MID_BUTTON_USER_BLOCK_IMP_IMG].SetRect(rcRightBtn);
		
	}
}

bool32 CIoViewAddUserBlock::CreateTable(E_ReportType eMerchKind)
{
	if ( eMerchKind == m_eMerchKind )
	{
		return true;
	}

	// 尝试创建表格
	if (NULL == m_GridCtrl.GetSafeHwnd())
	{
		//
		if (!m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 13456))
		{
			return false;
		}

		//
		// 创建数据表格
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		m_GridCtrl.SetAutoHideFragmentaryCell(false);
		m_GridCtrl.EnablePolygonCorner(false);


		m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
		m_XSBVert.SetScrollRange(0, 10);

		m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
		m_XSBHorz.SetScrollRange(0, 10);
		m_XSBHorz.SetScrollRange(0, 10);
		//m_XSBHorz.SetScrollBarLeftArrowH(-1);
		//m_XSBHorz.SetScrollBarRightArrowH(-1);
		m_XSBHorz.SetBorderColor(GetIoViewColor(ESCBackground));


		m_XSBHorz.SetOwner(&m_GridCtrl);
		m_XSBVert.SetOwner(&m_GridCtrl);
		m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);



		m_XSBHorz.SetOwner(&m_GridCtrl);
		m_XSBHorz.AddMsgListener(m_hWnd);	// 侦听滚动消息 - 记录滚动消息

	}

	// 清空表格内容
	m_GridCtrl.DeleteAllItems();

	//
	m_GridCtrl.EnableSelection(false);

	// 设置字体
	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	//m_GridCtrl.AutoSizeRows();

	m_eMerchKind = eMerchKind;

	return true;
}

void CIoViewAddUserBlock::SetDrawLineInfo()
{
	if ( !m_GridCtrl.GetSafeHwnd() )
	{
		return;
	}

	//
	if ( ERTExp == m_eMerchKind || ERTExpForeign == m_eMerchKind )
	{
		for ( int32 i = 0; i < m_GridCtrl.GetColumnCount(); i++ )
		{
			CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(1,i);
			pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);		
		}
	}
	else
	{
		for ( int32 i = 0; i < m_GridCtrl.GetColumnCount(); i++ )
		{
			CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, i);
			pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

			pCell = (CGridCellSys *)m_GridCtrl.GetCell(2, i);
			pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

			pCell = (CGridCellSys *)m_GridCtrl.GetCell(5, i);
			pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

			pCell = (CGridCellSys *)m_GridCtrl.GetCell(8, i);
			pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
		}
	}
}

bool32 CIoViewAddUserBlock::FromXml(TiXmlElement * pElement)
{
	//
	SetFontsFromXml(pElement);
	SetColorsFromXml(pElement);

	//
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

CString CIoViewAddUserBlock::ToXml()
{
	CString StrThis;

	CString StrMarketId;
	StrMarketId.Format(L"%d", m_MerchXml.m_iMarketId);

	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" ", 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(),
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrShowTabName()).GetBuffer(), m_StrTabShowName.GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(), 
		m_MerchXml.m_StrMerchCode.GetBuffer(),
		CString(GetXmlElementAttrMarketId()).GetBuffer(),
		StrMarketId.GetBuffer());

	CString StrFace;

	//
	StrFace  = SaveColorsToXml();
	StrFace += SaveFontsToXml();

	//
	StrThis += StrFace;
	//
	StrThis += L">\n";
	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

void CIoViewAddUserBlock::SetChildFrameTitle()
{

}

void CIoViewAddUserBlock::OnIoViewActive()
{
	UserBlockPagejump();
}

void CIoViewAddUserBlock::OnIoViewDeactive()
{

}

void CIoViewAddUserBlock::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();

	//
	RedrawWindow();
}

void CIoViewAddUserBlock::OnIoViewFontChanged()
{
	CIoViewBase::OnIoViewFontChanged();

	//	
	//m_GridCtrl.AutoSizeColumns(GVS_BOTH);
	//m_GridCtrl.AutoSizeRows();

	//
	//m_GridCtrl.RedrawWindow();
}

void CIoViewAddUserBlock::RequestViewData()
{
	if ( NULL == m_pMerchXml )
	{
		return;
	}

	// 发行情数据请求
	CMmiReqRealtimePrice ReqPrice;

	ReqPrice.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
	ReqPrice.m_StrMerchCode	= m_pMerchXml->m_MerchInfo.m_StrMerchCode;

	DoRequestViewData(ReqPrice);

	// 发 F10 数据请求
	CMmiReqPublicFile ReqF10;

	ReqF10.m_iMarketId		= m_pMerchXml->m_MerchInfo.m_iMarketId;
	ReqF10.m_StrMerchCode	= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
	ReqF10.m_ePublicFileType= EPFTF10;

	DoRequestViewData(ReqF10);

	// 发 K 线请求, 今天和昨天两天的就行了
	CMmiReqMerchKLine ReqKLine;

	ReqKLine.m_eKLineTypeBase = EKTBDay;
	ReqKLine.m_eReqTimeType	  = ERTYFrontCount;
	ReqKLine.m_TimeSpecify	  = m_pAbsCenterManager->GetServerTime();
	ReqKLine.m_iFrontCount	  = 2;

	DoRequestViewData(ReqKLine);

	//
	bool32 bH = false;
	if ( BeGeneralIndex(bH) )
	{
		if ( bH )
		{
			//
			CMmiReqGeneralFinance ReqGeneralFinance;
			ReqGeneralFinance.m_iMarketId = 0;

			DoRequestViewData(ReqGeneralFinance);
		}
		else
		{
			//
			CMmiReqGeneralFinance ReqGeneralFinance;
			ReqGeneralFinance.m_iMarketId = 1000;

			DoRequestViewData(ReqGeneralFinance);
		}
	}	
}

void CIoViewAddUserBlock::OnVDataGeneralFinanaceUpdate(CMerch* pMerch)
{

}

void CIoViewAddUserBlock::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{
	if ( NULL == pMerch || NULL == m_pMerchXml || pMerch != m_pMerchXml || NULL == pMerch->m_pRealtimePrice )
	{
		return;
	}

	SetDrawLineInfo();
	m_GridCtrl.RedrawWindow();
}

void CIoViewAddUserBlock::OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType)
{
	// 财务数据更新
	if ( NULL == pMerch || NULL == m_pMerchXml || pMerch != m_pMerchXml || EPFTF10 != ePublicFileType )
	{
		return;
	}

	//
	OnVDataRealtimePriceUpdate(pMerch);
}

void CIoViewAddUserBlock::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	// 更新涨跌停价格
	if ( NULL == pMerch || NULL == m_pMerchXml || pMerch != m_pMerchXml )
	{
		return;
	}
	//
	SetDrawLineInfo();
	m_GridCtrl.RedrawWindow();
}

void CIoViewAddUserBlock::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch || NULL == pMerch)
		return;

	//// 修改当前查看的商品
	//m_pMerchXml					= pMerch;
	//m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	//m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
	//
	//// 设置关注的商品信息
	//m_aSmartAttendMerchs.RemoveAll();
	//
	//CSmartAttendMerch SmartAttendMerch;
	//SmartAttendMerch.m_pMerch = pMerch;
	//SmartAttendMerch.m_iDataServiceTypes = EDSTPrice | EDSTKLine | EDSTGeneral;        
	//m_aSmartAttendMerchs.Add(SmartAttendMerch);

	////
	//E_ReportType eMerchKind = GetMerchKind(pMerch);
	//CreateTable(eMerchKind);

	////
	//if ( ERTExp != eMerchKind && ERTExpForeign != eMerchKind )
	//{
	//	// 得到所属板块信息
	//	CBlockCollection::BlockArray aBlocks;
	//	CBlockConfig::Instance()->GetBlocksByMerch(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, aBlocks);
	//	
	//	bool32 bFind = false;

	//	for ( int32 i = 0; i < aBlocks.GetSize(); i++ )
	//	{
	//		if ( aBlocks[i]->m_blockCollection.m_StrName == L"行业板块" )
	//		{
	//			m_StrBlockName = aBlocks[i]->m_blockInfo.m_StrBlockName;
	//			bFind = true;
	//		}
	//	}
	//	
	//	if ( !bFind )
	//	{
	//		m_StrBlockName.Empty();
	//	}
	//}

	// 所属板块
	/*if ( m_GridCtrl.GetSafeHwnd() )
	{
	if ( ERTExp != eMerchKind && ERTExpForeign != eMerchKind )
	{
	m_GridCtrl.SetCellType(2, 1, RUNTIME_CLASS(CGridCellSys));
	CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(1, 3);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);			
	pCell->SetText(m_StrBlockName);
	}		
	}
	*/
	//
	//OnVDataRealtimePriceUpdate(pMerch);
	//OnVDataMerchKLineUpdate(pMerch);
	SetSize();
	ReSetGridHead();

	//
	bool32 bH = false;
	if ( BeGeneralIndex(bH) )
	{
		KillTimer(KiTimerIdFiniance);
		SetTimer(KiTimerIdFiniance, KiTimerPeriodFiniance, NULL);
	}
	else
	{
		KillTimer(KiTimerIdFiniance);
	}
}



////////////////////////////////////////////////////////////////////////// 
//自选股相关
void CIoViewAddUserBlock::OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType)
{
	// 自选股更新的通知:
	// 只更新了商品
	// 1: 非自选板块,看是否包含自选板块的商品,设置不同的颜色
	// 2: 自选板块,更新商品顺序:TabChange()刷新表格, 更新表头	
	UserBlockPagejump();
}


void CIoViewAddUserBlock::OnVDataForceUpdate()
{
	RequestViewData();
}

bool32 CIoViewAddUserBlock::CalcRiseFallMaxPrice(OUT float& fRiseMax, OUT float& fFallMax)
{
	fRiseMax = 0.;
	fFallMax = 0.;

	//
	if ( NULL == m_pMerchXml || NULL == m_pMerchXml->m_pRealtimePrice || m_pMerchXml->m_MerchInfo.m_StrMerchCnName.GetLength() <= 0 )
	{
		return false;
	}

	/*
	涨跌停计算：
	1、所有的股票+指数+基金  
	涨停价： 昨收+昨收*10%（最后一位四舍五入）
	跌停价： 昨收-昨收*10%（最后一位四舍五入）
	2、名称第一个字母是"N "
	不计算涨跌停
	3、名称前两位“XR、XD、DR”的股票是除权除息的，昨收盘必须用报价表中的昨收盘计算，不能用K线图中的昨收计算
	涨停价： 昨收+昨收*10%（最后一位四舍五入）
	跌停价： 昨收-昨收*10%（最后一位四舍五入）
	4、名称包含“ST”(ST、*ST、 SST、S*ST)
	涨停价： 昨收+昨收*5%（最后一位四舍五入）
	跌停价： 昨收-昨收*5%（最后一位四舍五入）
	*/

	float fRate		= 0.1f;
	float fClosePre	= m_pMerchXml->m_pRealtimePrice->m_fPricePrevClose;

	//
	if ( 'N' == m_pMerchXml->m_MerchInfo.m_StrMerchCnName[0] )
	{
		// 新股 不用计算涨跌停
		return true;
	}


	//
	if ( -1 != m_pMerchXml->m_MerchInfo.m_StrMerchCnName.Find(L"ST") )
	{
		fRate = 0.05f;
	}

	// 
	float fTmp = fClosePre * fRate;

	// 四舍五入, 保留两位小数
	fTmp = (int32)((fTmp * 100) + 0.5) / 100.0f;

	//
	fRiseMax = fClosePre + fTmp;
	fFallMax = fClosePre - fTmp;

	return true;
}

bool32 CIoViewAddUserBlock::BeGeneralIndex(OUT bool32& bH)
{
	bH = false;

	if ( NULL == m_pMerchXml )
	{
		return false;
	}

	if ( 0 == m_pMerchXml->m_MerchInfo.m_iMarketId && L"000001" == m_pMerchXml->m_MerchInfo.m_StrMerchCode )
	{
		bH = true;
		return true;
	}
	else if ( 1000 == m_pMerchXml->m_MerchInfo.m_iMarketId && L"399001" == m_pMerchXml->m_MerchInfo.m_StrMerchCode )
	{
		bH = false;
		return true;
	}

	return false;
}

bool32 CIoViewAddUserBlock::SetUserBlockHeadInfomationList()
{
	//获取要显示的表头
	m_ReportHeadInfoList.RemoveAll();
	CString StrBlockName   = CUserBlockManager::Instance()->GetDefaultServerBlockName();

	T_Block* pBlock = CUserBlockManager::Instance()->GetBlock(StrBlockName);
	if ( NULL != pBlock )
	{
		m_eMarketReportType = pBlock->m_eHeadType;
	}
	else
	{
		m_eMarketReportType = ERTCustom;
	}
	//	
	CReportScheme::Instance()->GetReportHeadInfoList(m_eMarketReportType,m_ReportHeadInfoList,m_iFixCol);
	return true;
}

BOOL CIoViewAddUserBlock::OnCommand(WPARAM wParam, LPARAM lParam)
{
	OnBtnResponseEvent(wParam);
	return CIoViewBase::OnCommand(wParam, lParam);
}

void CIoViewAddUserBlock::OnTimer(UINT nIDEvent)
{	
	if ( nIDEvent == KiTimerIdFiniance )
	{
		//
		bool32 bH = false;
		if ( BeGeneralIndex(bH) )
		{
			if ( bH )
			{
				//
				CMmiReqGeneralFinance ReqGeneralFinance;
				ReqGeneralFinance.m_iMarketId = 0;

				DoRequestViewData(ReqGeneralFinance);
			}
			else
			{
				//
				CMmiReqGeneralFinance ReqGeneralFinance;
				ReqGeneralFinance.m_iMarketId = 1000;

				DoRequestViewData(ReqGeneralFinance);
			}
		}
	}
	CIoViewBase::OnTimer(nIDEvent);
}

void CIoViewAddUserBlock::OnLButtonDown(UINT nFlags, CPoint point)
{
	int iButton = TButtonHitTest(point);
	
	if (INVALID_ID != iButton)
	{
		m_mapButton[iButton].LButtonDown();
	}
}

void CIoViewAddUserBlock::OnLButtonUp(UINT nFlags, CPoint point)
{
	int iButton = TButtonHitTest(point);

	if (INVALID_ID != iButton)
	{
		m_mapButton[iButton].LButtonUp();
	}
}

void CIoViewAddUserBlock::OnMouseMove(UINT nFlags, CPoint point) 
{
	int iButton = TButtonHitTest(point);

	if (iButton != m_iXButtonHovering)
	{
		if (INVALID_ID != m_iXButtonHovering)
		{
			m_mapButton[m_iXButtonHovering].MouseLeave();
			m_iXButtonHovering = INVALID_ID;
		}

		if (INVALID_ID != iButton)
		{	
			m_iXButtonHovering = iButton;
			m_mapButton[m_iXButtonHovering].MouseHover();
		}
	}
}

LRESULT CIoViewAddUserBlock::OnMouseLeave(WPARAM wParam, LPARAM lParam)       
{     
	if (INVALID_ID != m_iXButtonHovering)
	{
		m_mapButton[m_iXButtonHovering].MouseLeave();
		m_iXButtonHovering = INVALID_ID;
	}

	return 0;            
}  

int CIoViewAddUserBlock::TButtonHitTest(CPoint point)
{
	map<int, CNCButton>::iterator iter;

	// 遍历所有按钮
	for (iter=m_mapButton.begin(); iter!=m_mapButton.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		// 点point是否在已绘制的按钮区域内
		if (btnControl.PtInButton(point) && btnControl.GetCreate())
		{
			return btnControl.GetControlId();
		}
	}

	return INVALID_ID;
}

