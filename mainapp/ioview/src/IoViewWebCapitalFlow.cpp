#include "stdafx.h"

#include "memdc.h"

//#include <math.h>
#include "MPIChildFrame.h"
#include "IoViewManager.h"
#include "MerchManager.h"

#include "GridCellSymbol.h"
#include "facescheme.h"
#include "ShareFun.h"
#include "ColorStep.h"

#include "PlugInStruct.h"
#include "IoViewWebCapitalFlow.h"

#include "PluginFuncRight.h"
#include "ConfigInfo.h"
#include "IoViewNews.h"
#include "coding.h"
#include "UrlParser.h"
#include <WindowsX.h>

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////
static const int32 KShowTimeSaleCount		 = 10000;
static const int32 KNormalShowTimeSaleCount = 100;

static const int32 KChartCycleDiameter = 150;		// 饼图直径不超过

static const int32 KTimerIdPushDataReq = 1002;
static const int32 KTimerPeriodPushDataReq = 1000*60*5;	// 请求数据

static const char *KStrElementAttrCustomUrl			= "CustomUrl";




enum E_ValueIndex		// 按照出现的顺序组织数组
{
	PureValueHuge = 0,
	PureValueBig,
	PureValueMid,
	PureValueSmall,
	PureValueCount
};


static const string strTransactionDetails[PureValueCount] = 
{
	"超大单（机构）",
	"大单（大户）",
	"中单（中户）",
	"小单（小户）",
};

static const string strTradePureValue[PureValueCount] = 
{
	"超大单",
	"大单",
	"中单",
	"小单"
};



/////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewWebCapitalFlow, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewWebCapitalFlow, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewWebCapitalFlow)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewWebCapitalFlow::CIoViewWebCapitalFlow()
:CIoViewBase()
{
	m_bActive			= false;

	m_RectTitle.SetRectEmpty();
	m_RectButton.SetRectEmpty();
	m_RectChartRate.SetRectEmpty();
	m_RectChartVol.SetRectEmpty();
	m_RectChartAmount.SetRectEmpty();

	m_pWndCef = NULL;
	m_StrCustomUrl = _T("");
	m_RectWeb = CRect(0,0,0,0);

	m_eCycle = Cycle_OneDay;
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewWebCapitalFlow::~CIoViewWebCapitalFlow()
{
	if(m_pWndCef != NULL)
	{
		delete m_pWndCef;
		m_pWndCef = NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////
// OnPaint

void CIoViewWebCapitalFlow::OnPaint()
{
	CPaintDC dc(this); // device context for painting	
	
	if ( GetParentGGTongViewDragFlag() || m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}
	
	//
	UnLockRedraw();

	CRect rcClient;
	GetClientRect(rcClient);
	
	COLORREF clrBk = RGB(35, 34, 40);//GetIoViewColor(ESCBackground);
	COLORREF clrVol = RGB(138, 137, 143);//GetIoViewColor(ESCVolume);
	COLORREF clrRise = GetIoViewColor(ESCRise);
	COLORREF clrFall = GetIoViewColor(ESCFall);
	COLORREF clrText = GetIoViewColor(ESCText);
	COLORREF clrAxis = GetIoViewColor(ESCChartAxisLine);

	dc.SetBkColor(clrBk);
	CMemDC dcMem(&dc, rcClient);

	dcMem->FillSolidRect(rcClient, clrBk);

	int iSaveDC = dcMem->SaveDC();

	if ( m_bActive )
	{
		//dcMem->FillSolidRect(3, 3, 2, 2, clrVol);		// 激活标志
	}

	dcMem->SetBkMode(TRANSPARENT);
	dcMem->SetBkColor(clrBk);

	//DrawCorner(dcMem, rcClient);

	CFont *pFontNormal = GetIoViewFontObject(ESFNormal);
	CFont *pFontSmall  = GetIoViewFontObject(ESFSmall);

	LOGFONT FontHeavy;
	memset(&FontHeavy, 0, sizeof(FontHeavy));

	_tcscpy(FontHeavy.lfFaceName , L"宋体");  
	FontHeavy.lfHeight		 = -14;
	FontHeavy.lfWeight	     = 550;
	FontHeavy.lfCharSet		 = 0;
	FontHeavy.lfOutPrecision = 3;

	CFont FontString;
	CFont* pOldFont = NULL;

	BOOL bOK = FontString.CreateFontIndirect(&FontHeavy);
	if ( bOK )
	{
		pOldFont = dcMem->SelectObject(&FontString);
	}


	// 无权限不绘制
	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncCapitalFlow, false) )
	{
		//dcMem->SelectObject(pFontSmall);
		dcMem->SetTextColor(clrText);
		CString StrRight = CPluginFuncRight::Instance().GetUserRightName(CPluginFuncRight::FuncCapitalFlow);
		CString StrShow = _T("不具备[")+StrRight+_T("]权限, ") + CConfigInfo::Instance()->GetPrompt();
		CRect rcDraw(rcClient);
		rcDraw.top = rcClient.CenterPoint().y;
		rcDraw.top -= 15;
		rcDraw.InflateRect(-2, 0);
		dcMem->DrawText(StrShow, rcDraw, DT_CENTER |DT_WORDBREAK );
	}
	else
	{
		CPen penAxis;
		penAxis.CreatePen(PS_SOLID, 0, clrAxis);
		CPen penBk;
		penBk.CreatePen(PS_SOLID, 0, clrBk);
		
		CRect rcDraw(rcClient);
		rcDraw.InflateRect(-2, -2);	// 边界空白
		
		if ( !m_RectTitle.IsRectEmpty() )
		{
			// 标题 商品名
			CString StrTitle = CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
			//dcMem->SelectObject(pFontNormal);
			dcMem->SetTextColor(clrVol);
			CRect rcTmp(0,0,0,0);
			/*int32 iHeight = */dcMem->DrawText(StrTitle, rcTmp, DT_SINGLELINE |DT_CALCRECT |DT_VCENTER |DT_LEFT);
			CRect rcTitle(m_RectTitle);
			dcMem->DrawText(StrTitle, rcTitle, DT_SINGLELINE |DT_VCENTER |DT_LEFT);
			rcTitle.left += rcTmp.Width();
			
			CString StrMerch;
			if ( NULL != m_pMerchXml )
			{
				StrMerch = m_pMerchXml->m_MerchInfo.m_StrMerchCnName;
				//dcMem->SetTextColor(clrText);
				//dcMem->SelectObject(pFontSmall);
				dcMem->DrawText(StrMerch, rcTitle, DT_SINGLELINE |DT_VCENTER |DT_CENTER);
				//DrawMerchName(dcMem, m_pMerchXml, rcTitle, false);
			}
		}
		
		// 按钮
		if ( !m_RectButton.IsRectEmpty() )
		{
			CRect rcBtn(m_RectButton);
			rcBtn.right = rcBtn.left + rcBtn.Width()/2;
			COLORREF clr1, clr2;
			clr1 = clr2 = clrText;
			if ( Cycle_OneDay == m_eCycle )
			{
				clr1 = clrVol;
			}
			else
			{
				clr2 = clrVol;
			}
			dcMem->SetTextColor(clr2);
			//dcMem->SelectObject(pFontSmall);
			dcMem->DrawText(_T("一日"), rcBtn, DT_SINGLELINE |DT_VCENTER |DT_CENTER); 
			
			rcBtn.left = rcBtn.right;
			rcBtn.right = m_RectButton.right;
			dcMem->SetTextColor(clr1);
			dcMem->DrawText(_T("五日"), rcBtn, DT_SINGLELINE |DT_VCENTER |DT_CENTER);
		}
		
		dcMem->SelectObject(&penAxis);	
		dcMem->RestoreDC(iSaveDC);
	}
}



void CIoViewWebCapitalFlow::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();

	Invalidate();
}


void CIoViewWebCapitalFlow::OnIoViewFontChanged()
{	
	CIoViewBase::OnIoViewFontChanged();
	// 字体调整 - 视图会变得不协调
	RecalcRectSize();
	Invalidate();
}

void CIoViewWebCapitalFlow::LockRedraw()
{
	if ( m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
}

void CIoViewWebCapitalFlow::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
}

int CIoViewWebCapitalFlow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	int iRet = CIoViewBase::OnCreate(lpCreateStruct);
	if ( -1 == iRet )
	{
		return -1;
	}

	//
	InitialIoViewFace(this);

	
	SetTimer(KTimerIdPushDataReq, KTimerPeriodPushDataReq, NULL);
	//
	CRect rcWindows;
	GetClientRect(rcWindows);
	rcWindows.top = rcWindows.top + 100;

	//m_StrCustomUrl =_T("file:///E:/点证智能投顾分析系统V1.0.0PC版需求/资金流向交互格式/dist/fund_flows.html");
	//CreateWeb();
	//ShowCustomWeb();
	return iRet;
}


void CIoViewWebCapitalFlow::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);
	
	RecalcRectSize();
	ShowCustomWeb();
}


void CIoViewWebCapitalFlow::CreateWeb()
{
	CGGTongApp *pApp = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());
	
	if (NULL == pApp || NULL == pApp->m_pConfigInfo)
	{
		return;
	}

	if(m_pWndCef != NULL)
	{
		m_pWndCef->MoveWindow(m_RectWeb);
		m_pWndCef->OpenUrl(m_StrCustomUrl);
		return;
	}

	// 加载文件本地路径
	UrlParser urlParser(L"alias://webCapitalflow");
	m_StrCustomUrl = urlParser.GetUrl();
	
	m_pWndCef = new CWndCef;
	m_pWndCef->CreateWndCef(7507,this, m_RectWeb,m_StrCustomUrl);

//	m_pWndCef->SetUrl(m_StrCustomUrl);
//	m_pWndCef->Create(NULL,NULL, WS_CHILD|WS_VISIBLE,
//		m_RectWeb, this, 7507);
}



void CIoViewWebCapitalFlow::RecalcRectSize()
{
	m_RectTitle.SetRectEmpty();
	m_RectButton.SetRectEmpty();
	m_RectChartRate.SetRectEmpty();
	m_RectChartVol.SetRectEmpty();
	m_RectChartAmount.SetRectEmpty();

	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncCapitalFlow, false) )
	{
		return;
	}
	
	CRect rcClient;
	GetClientRect(rcClient);
	
	CRect rcDraw(rcClient);
	rcDraw.InflateRect(-2, -2);	// 空隙
	
	if ( rcDraw.Width() <=0 || rcDraw.Height() <= 0 )
	{
		return;
	}

	CClientDC dc(this);
	//CFont *pOldFont = dc.SelectObject(GetIoViewFontObject(ESFNormal));
	
	CString StrWidth(_T("资金流向  固定商品标  一日  五日 "));
	CSize sizeWidth = dc.GetTextExtent(StrWidth);
	
	rcDraw.left += 7; // 不要太靠近边缘
	rcDraw.right = rcDraw.left + sizeWidth.cx;		// 依据字体，固定宽绘制
	
	CSize sizeText = dc.GetTextExtent(_T(" 一日 "));		// 以该宽度作为按钮的宽度
	m_RectButton = rcDraw;
	m_RectButton.bottom = m_RectButton.top + sizeText.cy + 2*4;
	m_RectButton.left = m_RectButton.right - 2*sizeText.cx - 13;
	
	if ( m_RectButton.Width() < rcDraw.Width() )
	{
		m_RectTitle.left = rcDraw.left;
		m_RectTitle.right = m_RectButton.left;
		m_RectTitle.top = m_RectButton.top;
		m_RectTitle.bottom = m_RectButton.bottom;
	}
	
	rcDraw.top = m_RectButton.bottom;

	rcDraw.top += 10;	// 分割线下方留一点空格

	
	if ( rcDraw.Height() <= 0 )
	{
		return;
	}

	if(NULL != m_pWndCef)
	{
		// 减去标题栏的高度
		m_RectWeb  = rcClient;
		m_RectWeb.top = m_RectButton.bottom + 3;
		m_pWndCef->MoveWindow(m_RectWeb);
	}
}



// 通知视图改变关注的商品
void CIoViewWebCapitalFlow::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// 当前商品不为空的时候
	if (NULL != pMerch)
	{
		// 修改当前查看的商品
		m_pMerchXml					= pMerch;
		m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
		m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;

		// 设置关注的商品信息
		m_aSmartAttendMerchs.RemoveAll();

		CSmartAttendMerch SmartAttendMerch;
		SmartAttendMerch.m_pMerch = pMerch;
		SmartAttendMerch.m_iDataServiceTypes = EDSTGeneral; //EDSTTimeSale | EDSTTick; // 关注的数据类型
		m_aSmartAttendMerchs.Add(SmartAttendMerch);

		// 
		OnVDataMerchTimeSalesUpdate(pMerch);	// 有就显示, 无则清空显示
	}
	else
	{
		// zhangbo 20090824 #待补充， 当心商品不存在时，清空当前显示数据
		//...
	}
	
	InvalidateRect(m_RectTitle);	// 标题部分需要修改，其它无
	// 数据清空
	if ( m_aTickExData.GetSize() > 0 )
	{
		m_aTickExData.RemoveAll();
		UpdateShowData();
		Invalidate();		// 刷新所有显示
	}
	//RequestViewData();  // change时不申请，等待viewdata调用force申请	
}

//
void CIoViewWebCapitalFlow::OnVDataForceUpdate()
{
	RequestViewData();
}

void CIoViewWebCapitalFlow::OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch)
{
	
}


bool32 CIoViewWebCapitalFlow::FromXml(TiXmlElement * pElement)
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


	// 重新加载url;
	CreateWeb();

	// 商品发生改变
	OnVDataMerchChanged(iMarketId, StrMerchCode, pMerchFound);	

	OnVDataForceUpdate();

	return true;
}



CString CIoViewWebCapitalFlow::ToXml()
{
	CString StrThis;

	CString StrMarketId;
	StrMarketId.Format(L"%d", m_MerchXml.m_iMarketId);

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\"  %s=\"%s\" ",/*>\n*/ 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrShowTabName()).GetBuffer(), 
		m_StrTabShowName.GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(),
		m_MerchXml.m_StrMerchCode.GetBuffer(),
		CString(GetXmlElementAttrMarketId()).GetBuffer(), 
		StrMarketId.GetBuffer(),
		CString(KStrElementAttrCustomUrl).GetBuffer(),
		m_StrCustomUrl.GetBuffer());
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

CString CIoViewWebCapitalFlow::GetDefaultXML()
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


void CIoViewWebCapitalFlow::OnIoViewActive()
{
	SetFocus();
	m_bActive = IsActiveInFrame();

	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	Invalidate();

	//ShowCustomWeb();
}

void CIoViewWebCapitalFlow::OnIoViewDeactive()
{
	m_bActive = false;

	Invalidate();
}

void  CIoViewWebCapitalFlow::SetChildFrameTitle()
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

void CIoViewWebCapitalFlow::RequestViewData()
{
	if (NULL == m_pMerchXml || NULL == m_pAbsCenterManager)
		return;

	// 无权限不请求该数据
	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncCapitalFlow, false) )
	{
		return;
	}

	
	if ( Cycle_OneDay == m_eCycle )
	{
		{
			// 普通情况下，申请最新单个商品 - 需要更新以后的周期数据，暂无
			CMmiReqTickEx info;
			info.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
			info.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
			
			RequestData(info);
		}
		
		// 发RealtimeTick请求
		{
			CMmiReqPushPlugInMerchData Req;
			Req.m_iMarketID			= m_pMerchXml->m_MerchInfo.m_iMarketId;
			Req.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
			Req.m_uType				= ECSTTickEx;
			RequestData(Req);
		}
	}	
	else 
	{
		// 发送历史数据请求
		CMmiReqHistoryTickEx info;
		info.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		RequestData(info);

		// 发RealtimeTick请求
		{
			CMmiReqPushPlugInMerchData Req;
			Req.m_iMarketID			= m_pMerchXml->m_MerchInfo.m_iMarketId;
			Req.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
			Req.m_uType				= ECSTTickEx;
			RequestData(Req);
		}
	}
}

void CIoViewWebCapitalFlow::RequestData( CMmiCommBase &req )
{
	// 隐藏时不要发请求，在视图激活时会根据当前数据与视图的关系，发一次请求
	//if ( IsWindowVisible() )
	{
		DoRequestViewData(req);
	}
}

BOOL CIoViewWebCapitalFlow::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

CString CIoViewWebCapitalFlow::GetCycleString()
{
	switch (m_eCycle)
	{
	case Cycle_OneDay:
		return CString(_T("一日"));
	case Cycle_FiveDay:
		return CString(_T("五日"));
	}
	ASSERT( 0 );
	return _T("");
}

void CIoViewWebCapitalFlow::ShowCustomWeb()
{

	if(m_pWndCef && !m_StrCustomUrl.IsEmpty())
	{
		m_pWndCef->MoveWindow(m_RectWeb);

		Json::Value root;
		Json::Value arrayObj;
		Json::Value item;


		//////////////////////////////////////////////////////////////////////////
		// strPeriod
		string strPeriodTemp;
		strPeriodTemp =  "五日";
		if (Cycle_OneDay == m_eCycle)
		{
			strPeriodTemp =  "一日";
		}
		else if (Cycle_FiveDay == m_eCycle)
		{
			strPeriodTemp =  "五日";
		}
		root["Period"] = Json::Value(strPeriodTemp);



		CString strUnit;
		float fUnit = 1.0;
		float fMax  = 1.0;
		////////////////////////////////////////////////////////////////////////////
		// strTradeComparison
		float fSell = m_capData.GetSellVol();
		float fBuy  = m_capData.GetBuyVol();
		float fOrgBuy, fOrgSell;
		fOrgBuy = fBuy;
		fOrgSell = fSell;
		fUnit = 1.0;		// 服务器传过来的数据单位原始
		if ( fSell >= 10000.0 )
		{
			fUnit = 10000.0;
		}

		float fTotal = fSell + fBuy;
		if ( fTotal == 0.0 )
		{
			fBuy = fSell = 1.0;		// 画一个对分图
			fTotal = 2.0;
		}
		strUnit.Format(_T("%s股"), fUnit <= 1.0 ? _T(""):_T("万"));
		//strTradeComparison.Format(KstrTradeComparison, fBuy/fUnit, fSell/fUnit, strUnit);

		CStringA strPassDataTemp;
		string   strJasonPassData;
		strPassDataTemp.Format("%.0f",fBuy/fUnit);
		strJasonPassData = strPassDataTemp;

		arrayObj.clear();
		item.clear();
		item["BuyAmount"]	= strJasonPassData;
		strPassDataTemp.Format("%.0f",fSell/fUnit);
		strJasonPassData = strPassDataTemp;
		item["SellAmount"]	= strJasonPassData;
		strJasonPassData = CStringA(strUnit);
		item["type"]		= strJasonPassData;
		arrayObj.append(item);
		root["TradeComparison"] = arrayObj;



		////////////////////////////////////////////////////////////////////////////
		// KstrTransactionDetailsOne  KstrTransactionDetailsTwo  KstrTransactionDetailsThree  KstrTransactionDetailsFour
		CArray<float, float> aVolValues;
		m_capData.GetVolValueArray(aVolValues);
		if (aVolValues.GetSize() != T_WebCapitalFlowDataPseudo::ValueCount)
		{
			return;
		}

		// 以最大的作为单位标准
		fMax = 1.0;	
		int i=0;
		for ( i=0 ; i < aVolValues.GetSize() ; i++ )
		{
			fMax = MAX(fMax, aVolValues[i]);
		}
		fUnit = 1.0f;
		if ( fMax >= 10000.0 )
		{
			fUnit = 10000.0f;
		}
		if ( fMax == 0.0 )
		{
			fMax = 1.0f;
		}

		strUnit.Format(_T("%s股"), fUnit <= 1.0 ? _T(""):_T("万"));

		arrayObj.clear();
		for (int i = PureValueHuge; i < PureValueCount && (aVolValues.GetSize() == PureValueCount*2); ++i)
		{
			item.clear();
			item["name"] = strTransactionDetails[i];
			strPassDataTemp.Format("%.0f", aVolValues[i*2]/fUnit);
			strJasonPassData = strPassDataTemp;
			item["buy"] = strJasonPassData;
			strPassDataTemp.Format("%.0f", aVolValues[i*2 + 1]/fUnit);
			strJasonPassData = strPassDataTemp;
			item["sell"] = strJasonPassData;
			strJasonPassData = CStringA(strUnit);
			item["type"] = strJasonPassData;

			arrayObj.append(item);
		}
		root["TransactionDetails"] = arrayObj;


		////////////////////////////////////////////////////////////////////////////
		// KstrTradePureValueOne  KstrTradePureValueTwo  KstrTradePureValueThree  KstrTradePureValueFour
		CArray<float, float> aAmountValues;
		m_capData.GetAmoutValueArray(aAmountValues);
		if (aAmountValues.GetSize() !=  T_WebCapitalFlowDataPseudo::ValueCount)
		{
			return;
		}

		float fMaxBuy = 0.0, fMaxSell = 0.0;	// 以最大的作为单位标准
		CArray<float, float> aValues;
		for (int i=0 ; i < aAmountValues.GetSize() /2 ; i++ )
		{
			aValues.Add( aAmountValues[2*i] - aAmountValues[2*i+1] );	// buy - sell
		}

		for (int i=0; i < aValues.GetSize() ; i++ )
		{
			if ( aValues[i] < 0.0 )
			{
				fMaxSell = MIN(fMaxSell, aValues[i]);
			}
			else
			{
				fMaxBuy = MAX(fMaxBuy, aValues[i]);
			}
		}

		fUnit = 1.0f;
		fMax = MAX(fMaxBuy, -fMaxSell);		// 绝对最大值
		if ( fMax >= 10000.0 )
		{
			fUnit = 10000.0f;
		}
		fMax = fMaxBuy - fMaxSell;		// 总跨度
		if ( fMax == 0.0 )
		{
			ASSERT( fMaxBuy==0.0 && fMaxSell == 0.0 ); // 两个都为0才导致跨度为0
			fMax = 1.0f;
		}
		strUnit.Format(_T("%s元"), fUnit <= 1.0 ? _T(""):_T("万"));

		arrayObj.clear();
		for (int i = PureValueHuge; i < PureValueCount && (aValues.GetSize() == PureValueCount); ++i)
		{
			item.clear();
			item["name"] = strTradePureValue[i];

			long long llValue;
			llValue = fabs(aValues[i]/fUnit);
			item["value"] = llValue;

			llValue = MAX(fMaxBuy, -fMaxSell) / fUnit + 100;
			item["max"] = llValue;
			
			//  是否为正数
			int iIsPositiveNumber = 0;
			llValue = aValues[i]/fUnit;
			if (llValue > 0)
			{
				iIsPositiveNumber = 1;
			}
			item["PositiveNumber"] = iIsPositiveNumber;
			

			strJasonPassData = CStringA(strUnit);
			item["type"] = strJasonPassData;

			arrayObj.append(item);
		}
		root["TradePureValue"] = arrayObj;

		
		string strWebDisData = root.toStyledString();

		int index = 0;
		if( !strWebDisData.empty())
		{
			while( (index = strWebDisData.find('\n',index)) != string::npos)
			{
				strWebDisData.erase(index,1);
			}
		}


		CString strUnicode  = strWebDisData.c_str();
		string strTransDataUtf8;
		UnicodeToUtf8(strUnicode, strTransDataUtf8);

		char *pchData = NULL;
		// 给30个字节给函数名称
		pchData = new char[strTransDataUtf8.length() + 30];
		memset(pchData, 0, strTransDataUtf8.length() + 30);
		sprintf(pchData, "flowStr('%s')", strTransDataUtf8.c_str());

		std::string strPassData = pchData;
		m_pWndCef->ReLoad();
		m_pWndCef->TransferFunAndRefreshWeb(strPassData.c_str());

		if (pchData)
		{
			delete[] pchData;
			pchData = NULL;
		}
	}
}

void CIoViewWebCapitalFlow::UpdateShowData( const T_WebCapitalFlowDataPseudo &cap )	
{
	if ( !(m_capData == cap) )
	{
		m_capData = cap;
		Invalidate(TRUE);

		// 数据更新刷新界面
		ShowCustomWeb();
	}
}

void CIoViewWebCapitalFlow::UpdateShowData()
{
	T_WebCapitalFlowDataPseudo cap;
	if ( m_aTickExData.GetSize() > 0 )
	{
		if ( Cycle_OneDay == m_eCycle )
		{
			cap = m_aTickExData[0];
		}
		else
		{
			// 累加数据
			for ( int i=0; i < m_aTickExData.GetSize() ; i++ )
			{
				cap += m_aTickExData[i];
			}
		}
	}
	UpdateShowData(cap);
}

void CIoViewWebCapitalFlow::EmptyShowData()
{
	m_aTickExData.RemoveAll();
	UpdateShowData();
}

void CIoViewWebCapitalFlow::ChangeCycleType( E_Cycle eCycle )
{
	if ( m_eCycle != eCycle )
	{
		m_eCycle = eCycle;
		RequestViewData();
		//EmptyShowData();
		Invalidate(TRUE);
	}

	
}

void CIoViewWebCapitalFlow::OnLButtonDown( UINT nFlags, CPoint point )
{
	CRect rcCycle1(m_RectButton);
	CRect rcCycle2(m_RectButton);
	rcCycle1.right = rcCycle2.left = rcCycle1.left + m_RectButton.Width()/2;

	if ( rcCycle1.PtInRect(point) )
	{
		ChangeCycleType(Cycle_OneDay);
	}
	else if ( rcCycle2.PtInRect(point) )
	{
		ChangeCycleType(Cycle_FiveDay);
	}

	ShowCustomWeb();
	CIoViewBase::OnLButtonDown(nFlags, point);
}

void CIoViewWebCapitalFlow::OnTickExResp( const CMmiRespTickEx *pResp )
{
	
	CMerch  *pMerch = NULL;
	int iSize = 0;
	iSize  = pResp->m_aTickEx.GetSize() ;
	if ( NULL == pResp || NULL == m_pAbsCenterManager || NULL == m_pMerchXml || pResp->m_aTickEx.GetSize() == 0 )
	{
		return;
	}

	int i = 0;
	for ( i=0; i < pResp->m_aTickEx.GetSize() ; i++ )
	{
		const T_TickEx	&tick = pResp->m_aTickEx[i];
		if ( !m_pAbsCenterManager->GetMerchManager().FindMerch(tick.m_StrMerchCode, tick.m_iMarketId, pMerch) )
		{
			continue;
		}
		if ( pMerch == m_pMerchXml )
		{
			break;
		}
	}
	if ( pMerch != m_pMerchXml || i >= pResp->m_aTickEx.GetSize() )
	{
		return;
	}

	const T_TickEx &tickMy = pResp->m_aTickEx[i];
	
	// 如果此时是5日显示则更新当日数据，再显示，如果是1日数据则直接显示
	TRACE(_T("TickEx %d: %.0f-%.0f\r\n"), i, tickMy.m_fSmallBuyVol, tickMy.m_fSmallSellVol);

	T_WebCapitalFlowDataPseudo cap;
	TickExToShowData(tickMy, cap);

	if ( m_aTickExData.GetSize() > 0 )
	{
		m_aTickExData[0] = cap;		// 替换当日数据
	}
	else
	{
		m_aTickExData.Add(cap);
	}
	
	UpdateShowData();
}

void CIoViewWebCapitalFlow::OnTickExPeriodResp(const CMmiRespPeriodTickEx *pResp)
{
	CMerch  *pMerch = NULL;
	if ( NULL == pResp || NULL == m_pAbsCenterManager || NULL == m_pMerchXml || pResp->m_aTickEx.GetSize() == 0 )
	{
		return;
	}

	int i = 0;
	for ( i=0; i < pResp->m_aTickEx.GetSize() ; i++ )
	{
		const T_TickEx	&tick = pResp->m_aTickEx[i];
		if ( !m_pAbsCenterManager->GetMerchManager().FindMerch(tick.m_StrMerchCode, tick.m_iMarketId, pMerch) )
		{
			continue;
		}
		if ( pMerch == m_pMerchXml )
		{
			break;
		}
	}
	if ( pMerch != m_pMerchXml || i >= pResp->m_aTickEx.GetSize() )
	{
		return;
	}

	const T_TickEx &tickMy = pResp->m_aTickEx[i];

	// 如果此时是5日显示则更新当日数据，再显示，如果是1日数据则直接显示
	TRACE(_T("TickEx %d: %.0f-%.0f\r\n"), i, tickMy.m_fSmallBuyVol, tickMy.m_fSmallSellVol);

	T_WebCapitalFlowDataPseudo cap;
	TickExToShowData(tickMy, cap);

	if ( m_aTickExData.GetSize() > 0 )
	{
		m_aTickExData[0] = cap;		// 替换当日数据
	}
	else
	{
		m_aTickExData.Add(cap);
	}

	UpdateShowData();
}
void CIoViewWebCapitalFlow::OnTickExHistoryResp( const CMmiRespHistoryTickEx *pResp )
{
	CMerch  *pMerch = NULL;
	if ( NULL == pResp || NULL == m_pAbsCenterManager || NULL == m_pMerchXml || pResp->m_aTickEx.GetSize() == 0 )
	{
		return;
	}
	
	int i = 0;
	CArray<T_TickEx, const T_TickEx &>  aTickExHis;
	for ( i=0; i < pResp->m_aTickEx.GetSize() ; i++ )
	{
		const T_TickEx	&tick = pResp->m_aTickEx[i];
		if ( !m_pAbsCenterManager->GetMerchManager().FindMerch(tick.m_StrMerchCode, tick.m_iMarketId, pMerch) )
		{
			continue;
		}
		if ( pMerch == m_pMerchXml )
		{
			aTickExHis.Add(tick);
			TRACE(_T("TickExHis %d: %.0f-%.0f\r\n"), i, tick.m_fSmallBuyVol, tick.m_fSmallSellVol);
		}
	}
	TRACE(_T("\n"));
	if ( pMerch != m_pMerchXml )	// 不是该商品的数据
	{
		return;
	}
	
	// 转换所有数据
	m_aTickExData.RemoveAll();
	for ( i=0; i < aTickExHis.GetSize() ; i++ )
	{
		T_WebCapitalFlowDataPseudo cap;
		TickExToShowData(aTickExHis[i], cap);
		m_aTickExData.Add(cap);
	}

	UpdateShowData();
}


void CIoViewWebCapitalFlow::OnVDataPluginResp( const CMmiCommBase *pResp )
{
	if ( NULL == pResp )
	{
		return;
	}
	
	if ( pResp->m_eCommType != ECTRespPlugIn )
	{
		ASSERT( 0 );
		return;
	}

	// 无权限不接收该数据
	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncCapitalFlow, false) )
	{
		return;
	}
	
	const CMmiCommBasePlugIn *pRespPlugin = (CMmiCommBasePlugIn *)pResp;
	const E_CommTypePlugIn ePlugin = pRespPlugin->m_eCommTypePlugIn;
	if ( ePlugin == ECTPIRespTickEx)
	{
		OnTickExResp((const CMmiRespTickEx *)pResp);
	}
	else if(ePlugin == ECTPIRespAddPushTickEx )
	{
		OnTickExPeriodResp((const CMmiRespPeriodTickEx *)pResp);
	}
	else if ( ePlugin == ECTPIRespHistoryTickEx )
	{
		OnTickExHistoryResp((const CMmiRespHistoryTickEx *)pResp);
	}
}

void CIoViewWebCapitalFlow::OnTimer( UINT nIDEvent )
{
	if ( KTimerIdPushDataReq == nIDEvent )
	{
		RequestViewData();
	}
	CIoViewBase::OnTimer(nIDEvent);
}

void CIoViewWebCapitalFlow::TickExToShowData( const T_TickEx &tickEx, OUT T_WebCapitalFlowDataPseudo &showData )
{
	showData.m_fBuyAmountBig = tickEx.m_fBigBuyAmount;
	showData.m_fBuyAmountHuge = tickEx.m_fExBigBuyAmount;
	showData.m_fBuyAmountMid = tickEx.m_fMidBuyAmount;
	showData.m_fBuyAmountSmall = tickEx.m_fSmallBuyAmount;
	
	showData.m_fSellAmountBig = tickEx.m_fBigSellAmount;
	showData.m_fSellAmountHuge = tickEx.m_fExBigSellAmount;
	showData.m_fSellAmountMid  = tickEx.m_fMidSellAmount;
	showData.m_fSellAmountSmall = tickEx.m_fSmallSellAmount;
	
	showData.m_fBuyVolBig	=	tickEx.m_fBigBuyVol;
	showData.m_fBuyVolHuge	=	tickEx.m_fExBigBuyVol;
	showData.m_fBuyVolMid	=	tickEx.m_fMidBuyVol;
	showData.m_fBuyVolSmall	=	tickEx.m_fSmallBuyVol;
	
	showData.m_fSellVolBig	=	tickEx.m_fBigSellVol;
	showData.m_fSellVolHuge	=	tickEx.m_fExBigSellVol;
	showData.m_fSellVolMid	=	tickEx.m_fMidSellVol;
	showData.m_fSellVolSmall	=	tickEx.m_fSmallSellVol;
	
	showData.m_fAllBuyAmount =	tickEx.m_fAllBuyAmount;
	showData.m_fAllSellAmount=	tickEx.m_fAllSellAmount;
	
	showData.m_fAmountPerTrans = tickEx.m_fAmountPerTrans;
	showData.m_fStocksPerTrans = tickEx.m_fStocksPerTrans;	
}


//////////////////////////////////////////////////////////////////////////
//
const T_WebCapitalFlowDataPseudo & T_WebCapitalFlowDataPseudo::operator+=( const T_WebCapitalFlowDataPseudo &cap )
{
	m_fBuyVolHuge	       += cap.m_fBuyVolHuge;
	m_fBuyVolBig		   += cap.m_fBuyVolBig;
	m_fBuyVolMid		   += cap.m_fBuyVolMid		  ;
	m_fBuyVolSmall         += cap.m_fBuyVolSmall    ;
	m_fSellVolHuge	       += cap.m_fSellVolHuge	  ;
	m_fSellVolBig          += cap.m_fSellVolBig     ;
	m_fSellVolMid          += cap.m_fSellVolMid     ;
	m_fSellVolSmall        += cap.m_fSellVolSmall   ;
	m_fBuyAmountHuge	   += cap.m_fBuyAmountHuge	;
	m_fBuyAmountBig	       += cap.m_fBuyAmountBig	  ;
	m_fBuyAmountMid	       += cap.m_fBuyAmountMid	  ;
	m_fBuyAmountSmall      += cap.m_fBuyAmountSmall ;
	m_fSellAmountHuge      += cap.m_fSellAmountHuge ;
	m_fSellAmountBig       += cap.m_fSellAmountBig  ;
	m_fSellAmountMid       += cap.m_fSellAmountMid  ;
	m_fSellAmountSmall     += cap.m_fSellAmountSmall;

	return *this;
}

const T_WebCapitalFlowDataPseudo T_WebCapitalFlowDataPseudo::operator+( const T_WebCapitalFlowDataPseudo &cap )
{
	T_WebCapitalFlowDataPseudo cap1(*this);
	cap1 += cap;
	return cap1;
}
