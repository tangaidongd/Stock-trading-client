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

	// �����ɫ������
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
	m_BtnAddUserBlk.CreateButton(L"�����ѡ��", CRect(0,0,0,0), this, m_pImgBtn, 3, MID_BUTTON_USER_BLOCK_ADD_IMG);
	m_BtnImpUserBlk.CreateButton(L"������ѡ��", CRect(0,0,0,0), this, m_pImgBtn, 3, MID_BUTTON_USER_BLOCK_IMP_IMG);

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
	if ( mapTrans.empty() )		// ��ʼ��ת������
	{
		mapTrans[ CReportScheme::ERHPricePrevClose ]	= EMRFPricePrevClose;	// ����
		mapTrans[ CReportScheme::ERHPricePrevBalance ]	= EMRFPricePrevClose;

		mapTrans[ CReportScheme::ERHPriceOpen ]	= EMRFPriceOpen;	// ���̼�

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

		// xl 0821 ����		
		mapTrans[ CReportScheme::ERHCapitalFlow ]	= EMRFCapticalFlow;

		mapTrans[ CReportScheme::ERHChangeRate ]	= EMRFTradeRate;

		mapTrans[ CReportScheme::ERHMarketWinRate ]	= EMRFPeRate;

		mapTrans[ CReportScheme::ERHVolumeRate ]	= EMRFVolRatio;

		// ǿ���� 
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
	case CReportScheme::ERHRowNo:				// �к�
	case CReportScheme::ERHMerchCode:			// ����
	case CReportScheme::ERHMerchName:			// ����
	case CReportScheme::ERHTime:				// ʱ��
		break;	
		// 
	case CReportScheme::ERHPricePrevClose:		// ���ռ�
		eRet = EMRFPricePrevClose;
		bTrans = true;
		break;
	case CReportScheme::ERHPricePrevBalance:	// ����գ��ڻ������ƣ� ��ͬ�ڹ�Ʊ�����ռۣ�
		eRet = EMRFPricePreAvg;
		bTrans = true;
		break;
		// 
	case CReportScheme::ERHPriceOpen:			// ���̼�
		eRet = EMRFPriceOpen;
		bTrans = true;
		break;
		// 
	case CReportScheme::ERHPriceNew:			// ���¼�	
		eRet = EMRFPriceNew;
		bTrans = true;
		break;		
		// 
	case CReportScheme::ERHPriceHigh:			// ��� 
		eRet = EMRFPriceHigh;
		bTrans = true;
		break;
	case CReportScheme::ERHPriceLow:			// ���
		eRet = EMRFPriceLow;
		bTrans = true;
		break;
		//
	case CReportScheme::ERHVolumeCur:			// ����
		eRet = EMRFVolumeCur;
		bTrans = true;
		break;

		//
	case CReportScheme::ERHVolumeTotal:			// �ɽ���
		eRet = EMRFVolumeTotal;
		bTrans = true;
		break;

		// 
	case CReportScheme::ERHAmount:				// ���
		eRet = EMRFAmountTotal;
		bTrans = true;
		break;
	case CReportScheme::ERHRiseFall:			// �ǵ�
		eRet = EMRFRiseValue;
		bTrans = true;
		break;
	case CReportScheme::ERHRange:				// ����%
		eRet = EMRFRisePercent;
		bTrans = true;
		break;
	case CReportScheme::ERHSwing:				// ���
		eRet = EMRFAmplitude;
		bTrans = true;
		break;
	case CReportScheme::ERHVolumeRate:			// ����
		eRet = EMRFVolRatio;
		bTrans = true;
		break;
	case CReportScheme::ERHPowerDegree:			// ǿ����
	case CReportScheme::ERHDKBallance:			// ���ƽ��
	case CReportScheme::ERHDWin:				// ��ͷ����
	case CReportScheme::ERHDLose:				// ��ͷͣ��
	case CReportScheme::ERHKWin:				// ��ͷ�ز�
	case CReportScheme::ERHKLose:				// ��ͷͣ��
		break;
	case CReportScheme::ERHRate:				// ί��
		eRet = EMRFBidRatio;
		bTrans = true;
		break;
	case CReportScheme::ERHSpeedRiseFall:		// �����ǵ�
		eRet = EMRFRiseRate;
		bTrans = true;
		break;
	case CReportScheme::ERHChangeRate:			// ������
	case CReportScheme::ERHMarketWinRate:		// ��ӯ��		
		break;
	case CReportScheme::ERHBuyPrice:			// �����
		eRet = EMRFPriceBuy;
		bTrans = true;
		break;
	case CReportScheme::ERHSellPrice:			// ������
		eRet = EMRFPriceSell;
		bTrans = true;
		break;
	case CReportScheme::ERHBuyAmount:			// ������
	case CReportScheme::ERHSellAmount:			// ������
	case CReportScheme::ERHDifferenceHold:		// �ֲֲ�
	case CReportScheme::ERHBuySellPrice:		// �����/������
	case CReportScheme::ERHBuySellVolume:		// ��/����
	case CReportScheme::ERHPreDone1:			// ǰ��1
	case CReportScheme::ERHPreDone2:			// ǰ��2
	case CReportScheme::ERHPreDone3:			// ǰ��3
	case CReportScheme::ERHPreDone4:			// ǰ��4
	case CReportScheme::ERHPreDone5:			// ǰ��5
		break;

	case CReportScheme::ERHPriceBalance:		// ����
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
		eRet = ReportHeader2MerchReportFieldBeiJing(eReportHeader, bTrans);		// ���������Ĳ鿴��case����
	}

	return eRet;
}

void CIoViewAddUserBlock::UserBlockPagejump()
{
	// �õ�������
	CString StrBlockName = CUserBlockManager::Instance()->GetDefaultServerBlockName();
	T_Block* pBlock	= CUserBlockManager::Instance()->GetBlock(StrBlockName);
	if ( NULL == pBlock )
	{
		return;
	}

	// �õ���������е���Ʒ
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

	// ���ù�����λ��:
	m_XSBHorz.ShowWindow(SW_HIDE);

	CGmtTime m_TimeLast = CGmtTime(0);
	m_bRequestViewSort = false;

	// ���	
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


	// �õ���ͷ��Ϣ
	if ( !SetUserBlockHeadInfomationList() )
	{
		return;
	}

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();

	//�趨��ͷ
	m_GridCtrl.SetFixedColumnCount(m_iFixCol);
	iSize = m_ReportHeadInfoList.GetSize();


	bool32 bFoundForceField = false;
	CStringArray HeaderNames;
	CArray<int32,int32> HeaderFields;	
	CArray<int32,int32> HeaderWidths;
	// ת��
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
		// ����ѡ�ɵ���������ǿ����ʾ���� xl 1220
		// ���г�����ָ�������г���Ʒ�Ķ�����һ�Ž�����ʾ
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

	// ���ñ��̶���
	m_GridCtrl.SetFixedRowCount(1);
	m_GridCtrl.SetColumnCount(HeaderNames.GetSize());

	// �����п�	
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
	// ���ù�����λ��:
	m_XSBHorz.Invalidate();
	m_XSBHorz.ShowWindow(SW_SHOW);


	// ������䲻������ƽ�ֵ�ǰ��ͼ
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
	
	// ������ʾ
	
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

	// ���Դ������
	if (NULL == m_GridCtrl.GetSafeHwnd())
	{
		//
		if (!m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 13456))
		{
			return false;
		}

		//
		// �������ݱ��
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
		m_XSBHorz.AddMsgListener(m_hWnd);	// ����������Ϣ - ��¼������Ϣ

	}

	// ��ձ������
	m_GridCtrl.DeleteAllItems();

	//
	m_GridCtrl.EnableSelection(false);

	// ��������
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

	// �ж��ǲ���IoView�Ľڵ�
	const char *pcValue = pElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;

	// �ж��ǲ��������Լ����ҵ����ͼ�Ľڵ�
	const char *pcAttrValue = pElement->Attribute(GetXmlElementAttrIoViewType());
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(CString(pcAttrValue)) != 0)	// ���������Լ���ҵ��ڵ�
		return false;

	// ��ȡ��ҵ����ͼ���е�����
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

	// ��������������
	CMmiReqRealtimePrice ReqPrice;

	ReqPrice.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
	ReqPrice.m_StrMerchCode	= m_pMerchXml->m_MerchInfo.m_StrMerchCode;

	DoRequestViewData(ReqPrice);

	// �� F10 ��������
	CMmiReqPublicFile ReqF10;

	ReqF10.m_iMarketId		= m_pMerchXml->m_MerchInfo.m_iMarketId;
	ReqF10.m_StrMerchCode	= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
	ReqF10.m_ePublicFileType= EPFTF10;

	DoRequestViewData(ReqF10);

	// �� K ������, �������������ľ�����
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
	// �������ݸ���
	if ( NULL == pMerch || NULL == m_pMerchXml || pMerch != m_pMerchXml || EPFTF10 != ePublicFileType )
	{
		return;
	}

	//
	OnVDataRealtimePriceUpdate(pMerch);
}

void CIoViewAddUserBlock::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	// �����ǵ�ͣ�۸�
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

	//// �޸ĵ�ǰ�鿴����Ʒ
	//m_pMerchXml					= pMerch;
	//m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	//m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
	//
	//// ���ù�ע����Ʒ��Ϣ
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
	//	// �õ����������Ϣ
	//	CBlockCollection::BlockArray aBlocks;
	//	CBlockConfig::Instance()->GetBlocksByMerch(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, aBlocks);
	//	
	//	bool32 bFind = false;

	//	for ( int32 i = 0; i < aBlocks.GetSize(); i++ )
	//	{
	//		if ( aBlocks[i]->m_blockCollection.m_StrName == L"��ҵ���" )
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

	// �������
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
//��ѡ�����
void CIoViewAddUserBlock::OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType)
{
	// ��ѡ�ɸ��µ�֪ͨ:
	// ֻ��������Ʒ
	// 1: ����ѡ���,���Ƿ������ѡ������Ʒ,���ò�ͬ����ɫ
	// 2: ��ѡ���,������Ʒ˳��:TabChange()ˢ�±��, ���±�ͷ	
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
	�ǵ�ͣ���㣺
	1�����еĹ�Ʊ+ָ��+����  
	��ͣ�ۣ� ����+����*10%�����һλ�������룩
	��ͣ�ۣ� ����-����*10%�����һλ�������룩
	2�����Ƶ�һ����ĸ��"N "
	�������ǵ�ͣ
	3������ǰ��λ��XR��XD��DR���Ĺ�Ʊ�ǳ�Ȩ��Ϣ�ģ������̱����ñ��۱��е������̼��㣬������K��ͼ�е����ռ���
	��ͣ�ۣ� ����+����*10%�����һλ�������룩
	��ͣ�ۣ� ����-����*10%�����һλ�������룩
	4�����ư�����ST��(ST��*ST�� SST��S*ST)
	��ͣ�ۣ� ����+����*5%�����һλ�������룩
	��ͣ�ۣ� ����-����*5%�����һλ�������룩
	*/

	float fRate		= 0.1f;
	float fClosePre	= m_pMerchXml->m_pRealtimePrice->m_fPricePrevClose;

	//
	if ( 'N' == m_pMerchXml->m_MerchInfo.m_StrMerchCnName[0] )
	{
		// �¹� ���ü����ǵ�ͣ
		return true;
	}


	//
	if ( -1 != m_pMerchXml->m_MerchInfo.m_StrMerchCnName.Find(L"ST") )
	{
		fRate = 0.05f;
	}

	// 
	float fTmp = fClosePre * fRate;

	// ��������, ������λС��
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
	//��ȡҪ��ʾ�ı�ͷ
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

	// �������а�ť
	for (iter=m_mapButton.begin(); iter!=m_mapButton.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		// ��point�Ƿ����ѻ��Ƶİ�ť������
		if (btnControl.PtInButton(point) && btnControl.GetCreate())
		{
			return btnControl.GetControlId();
		}
	}

	return INVALID_ID;
}

