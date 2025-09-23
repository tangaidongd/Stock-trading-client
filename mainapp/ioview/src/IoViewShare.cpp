#include "stdafx.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <locale.h>

#include "MPIChildFrame.h"
#include "MerchManager.h"
#include "IoViewManager.h"
#include "IoViewChart.h"
#include "IoViewShare.h"
#include "IoViewTrend.h"
#include "facescheme.h"
#include "IoViewKLine.h"
#include "IoViewTrend.h"
#include "ShareFun.h"
#include "PathFactory.h"
#include "Region.h"
#include "ChartRegion.h"
#include "CFormularContent.h"
#include "float.h"
#include "GmtTime.h"
#include "StructKlineView.h"
#include "limits.h"
#include "GridCtrl.h"
#include "IndexDrawer.h"
#include "GridCellSymbol.h"
#include "formulaengine.h"
#include "CFormularComputeParent.h"
#include <io.h>

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  STOCK_NAME _T("名称")
#define  STOCK_CODE _T("代码")
#define  STOCK_START_ROW		1		//手机开在哪一行
#define  STOCK_START_COL		1		//数据多少列开始

static Image *g_pImageSale = NULL;
static Image *g_pImageBuy = NULL;

//折半查找(精确)
int32 HalfSearch ( CArray<CKLine,CKLine>& KLines,int32 id )
{
	int32 iStart = 0;
	int32 iEnd = KLines.GetSize()-1;
	while ( iStart <= iEnd )
	{
		int32 i = ( iStart + iEnd ) / 2;
		CKLine kLine = KLines.GetAt(i);
		int32 t = kLine.m_TimeCurrent.GetTime();
		if ( id == t ) return i;
		if ( id > t ) iStart = i + 1;
		else iEnd = i - 1;
	}
	return -1;
}

//折半查找(精确)
int32 HalfSearch ( CArray<CTick,CTick>& Ticks,int32 id )
{
	int32 iStart = 0;
	int32 iEnd = Ticks.GetSize()-1;
	while ( iStart <= iEnd )
	{
		int32 i = ( iStart + iEnd ) / 2;
		CTick Tick = Ticks.GetAt(i);
		int32 t = Tick.m_TimeCurrent.m_Time.GetTime();
		if ( id == t ) return i;
		if ( id > t ) iStart = i + 1;
		else iEnd = i - 1;
	}
	return -1;
}

bool32 HalfSearchEqu ( int32 id,int32 t,int32 i, int32& iPos,int32& iSub )
{
	if ( id == t )
	{
		return true;
	}
	int32 iSubTemp = abs(id - t);
	if ( iSub > iSubTemp )
	{
		iSub = iSubTemp;
		iPos = i;
	}
	return false;
}

//折半查找(大约)
int32 HalfSearchEx ( CArray<CKLine,CKLine>& KLines,int32 id )
{
	int32 iStart = 0;
	int32 iEnd = KLines.GetSize()-1;
	if ( 0 == iEnd )
	{
		return 0;
	}
	int32 iPos = -1;
	int32 iSub=LONG_MAX;
	while ( iStart <= iEnd )
	{
		int32 i = ( iStart + iEnd ) / 2;
		CKLine kLine = KLines.GetAt(i);
		int32 t = kLine.m_TimeCurrent.GetTime();

		if ( HalfSearchEqu(id,t,i,iPos,iSub ))
		{
			return i;
		}
		if ( id > t )
		{
			iStart = i + 1;
		}
		else
		{
			iEnd = i - 1;
		}
	}
	return iPos;
}

int32 HalfSearchEx ( CArray<CTick,CTick>& Ticks,int32 id )
{
	int32 iStart = 0;
	int32 iEnd = Ticks.GetSize()-1;
	if ( 0 == iEnd )
	{
		return 0;
	}
	int32 iPos = -1;
	int32 iSub=LONG_MAX;
	while ( iStart <= iEnd )
	{
		int32 i = ( iStart + iEnd ) / 2;
		CTick Tick = Ticks.GetAt(i);
		int32 t = Tick.m_TimeCurrent.m_Time.GetTime();

		if ( HalfSearchEqu(id,t,i,iPos,iSub ))
		{
			return i;
		}
		if ( id > t )
		{
			iStart = i + 1;
		}
		else
		{
			iEnd = i - 1;
		}
	}
	return iPos;
}

CString RequestToString ( CMmiReqMerchKLine* pInfo )
{
	CString StrLog;
	switch(pInfo->m_eReqTimeType)
	{
	case ERTYSpecifyTime:
		StrLog.Format(_T(" RequestKLine MerchCode=%s MarketId=%d TypeBase=%d TimeStart=%s TimeEnd=%s"),
					   pInfo->m_StrMerchCode.GetBuffer(),
					   pInfo->m_iMarketId,
					   pInfo->m_eKLineTypeBase,
					   Time2String ( pInfo->m_TimeStart,ENTIMinute).GetBuffer(),
					   Time2String ( pInfo->m_TimeEnd,ENTIMinute).GetBuffer());
		break;
	case ERTYFrontCount:
		StrLog.Format(_T(" RequestKLine MerchCode=%s MarketId=%d TypeBase=%d TimeSpecify=%s FrontCount=%d"),
					   pInfo->m_StrMerchCode.GetBuffer(),
					   pInfo->m_iMarketId,
					   pInfo->m_eKLineTypeBase,
					   Time2String ( pInfo->m_TimeSpecify,ENTIMinute).GetBuffer(),
					   pInfo->m_iFrontCount);
		break;
	case ERTYCount:
		StrLog.Format(_T(" RequestKLine MerchCode=%s MarketId=%d TypeBase=%d TimeSpecify=%s Count=%d"),
					   pInfo->m_StrMerchCode.GetBuffer(),
					   pInfo->m_iMarketId,
					   pInfo->m_eKLineTypeBase,
					   Time2String ( pInfo->m_TimeSpecify,ENTIMinute).GetBuffer(),
					   pInfo->m_iCount);
		break;
	default:
		break;
	}

	return StrLog;
}

CString RequestToString ( CMmiReqMerchTimeSales* pInfo )
{
	CString StrLog;
	switch(pInfo->m_eReqTimeType)
	{
	case ERTYSpecifyTime:
		StrLog.Format(_T("RequestTimeSales MerchCode=%s MarketId=%d TimeStart=%s TimeEnd=%s"),
					   pInfo->m_StrMerchCode.GetBuffer(),
					   pInfo->m_iMarketId,
					   Time2String ( pInfo->m_TimeStart,ENTIMinute).GetBuffer(),
					   Time2String ( pInfo->m_TimeEnd,ENTIMinute).GetBuffer());
		break;
	case ERTYFrontCount:
		StrLog.Format(_T("RequestTimeSales MerchCode=%s MarketId=%d TimeSpecify=%s FrontCount=%d"),
					   pInfo->m_StrMerchCode.GetBuffer(),
					   pInfo->m_iMarketId,
					   Time2String ( pInfo->m_TimeSpecify,ENTIMinute).GetBuffer(),
					   pInfo->m_iFrontCount);
		break;
	case ERTYCount:
		StrLog.Format(_T("RequestTimeSales MerchCode=%s MarketId=%d TimeSpecify=%s Count=%d"),
					   pInfo->m_StrMerchCode.GetBuffer(),
					   pInfo->m_iMarketId,
					   Time2String ( pInfo->m_TimeSpecify,ENTIMinute).GetBuffer(),
					   pInfo->m_iCount);
		break;
	default:
		break;
	}

	return StrLog;
}

void CountNumber(CString StrName)
{
	static int32 sCountNumber = 0;
	sCountNumber++;
	CString StrText;
	StrText.Format(_T("%s=%d"),StrName.GetBuffer(),sCountNumber);
	AfxGetMainWnd()->SetWindowText(StrText);
}

void SetHourToLast(CGmtTime& cgtTime)
{
	cgtTime = CGmtTime(cgtTime.GetYear(),cgtTime.GetMonth(),cgtTime.GetDay(),23,59,59);
}

uint32 NodeTimeIntervalToFlag ( E_NodeTimeInterval eTimeInterval )
{
	switch ( eTimeInterval )
	{
	case ENTIMinute:
		return CFormularContent::KAllowMinute;
	case ENTIDay:
		return CFormularContent::KAllowDay;
	case ENTIMinute5:
		return CFormularContent::KAllowMinute5;
	case ENTIMinute15:
		return CFormularContent::KAllowMinute15;
	case ENTIMinute30:
		return CFormularContent::KAllowMinute30;
	case ENTIMinute60:
		return CFormularContent::KAllowMinute60;
	case ENTIMinute180:
		return CFormularContent::KAllowMinute180;
	case ENTIMinute240:
		return CFormularContent::KAllowMinute240;
	case ENTIMinuteUser:
		return CFormularContent::KAllowMinuteUser;
	case ENTIDayUser:
		return CFormularContent::KAllowDayUser;
	case ENTIWeek:
		return CFormularContent::KAllowWeek;
	case ENTIMonth:
		return CFormularContent::KAllowMonth;
	case ENTIQuarter:
		return CFormularContent::KAllowQuarter;
	case ENTIYear:
		return CFormularContent::KAllowYear;
	default:
		return CFormularContent::KAllowMinute;
	}
}

CString KLineTypeBaseToString ( E_KLineTypeBase eKTBase )
{
	CString StrText;
	switch ( eKTBase )
	{
	case EKTBMinute:
		StrText = _T("EKTBMinute");
		break;
	case EKTB5Min:
		StrText = _T("EKTB5Min");
		break;
	case EKTBHour:
		StrText = _T("EKTBHour");
		break;
	case EKTBDay:
		StrText = _T("EKTBDay");
		break;
	case EKTBMonth:
		StrText = _T("EKTBMonth");
		break;
	default:
		break;
	}
	return StrText;
}

CString GetWeekString(CTime Time)
{
	// ...fangz0817 test
	if (Time < 0)
	{
		return L"";
	}
	
	int32	 iWeek = Time.GetDayOfWeek();	
	
	CString StrWeek;
	
	switch(iWeek)
	{
	case 1:	 StrWeek = L"日";
		break;
	case 2:	 StrWeek = L"一";
		break;
	case 3:	 StrWeek = L"二";
		break;
	case 4:	 StrWeek = L"三";
		break;
	case 5:	 StrWeek = L"四";
		break;
	case 6:	 StrWeek = L"五";
		break;
	case 7:	 StrWeek = L"六";
		break;
	}
	
	return StrWeek;
}

void Time2String(CGmtTime & Time1,E_NodeTimeInterval eTimeInterval,CString & StrTimeLine1,CString &StrTimeLine2,bool32 bIoViewTick)
{
	StrTimeLine1 = L"-";
	StrTimeLine2 = L"-";
	
	if (Time1 <= 0)
	{
		return;
	}

	CTime Time(Time1.GetTime());

	int32 iWeek = Time.GetDayOfWeek();	
	
	CString StrWeek;
	switch(iWeek)
	{
	case 1:	 StrWeek = L"日";
		break;
	case 2:	 StrWeek = L"一";
		break;
	case 3:	 StrWeek = L"二";
		break;
	case 4:	 StrWeek = L"三";
		break;
	case 5:	 StrWeek = L"四";
		break;
	case 6:	 StrWeek = L"五";
		break;
	case 7:	 StrWeek = L"六";
		break;
	}

	switch ( eTimeInterval )
	{
		// 首行:  YYYY/MM/DD/WW
	case ENTIMinute:	
	case ENTIMinute5:
	case ENTIMinute15:
	case ENTIMinute30:
	case ENTIMinute60:
	case ENTIMinute180:
	case ENTIMinute240:
	case ENTIMinuteUser:
		{
			StrTimeLine1.Format(L"%02d/%02d/%s",Time.GetMonth(),Time.GetDay(),StrWeek.GetBuffer());
			if (!bIoViewTick)
			{
				// 次行:  hh:mm
				StrTimeLine2.Format(L"%02d:%02d",Time.GetHour(),Time.GetMinute());			
			}
			else
			{
				// 次行:  hh:mm:ss
				StrTimeLine2.Format(L"%02d:%02d:%02d",Time.GetHour(),Time.GetMinute(),Time.GetSecond());			
			}
		}
		break;
		//首行: YYYY
		//次行: MM/DD/WW
	case ENTIDay:
	case ENTIDayUser:
	case ENTIWeek:		
	case ENTIMonth:
	case ENTIQuarter:
	case ENTIYear:
		{
			StrTimeLine1.Format(L"%04d",Time.GetYear());
			StrTimeLine2.Format(L"%02d/%02d/%s",Time.GetMonth(),Time.GetDay(),StrWeek.GetBuffer());						
		}
		break;
	default:
		break;
	}
}

CString Time2String ( CGmtTime& Time1, E_NodeTimeInterval eTimeInterval,bool32 bDetail ,bool32 bForeign)
{
	CString StrText;
	CTime Time(Time1.GetTime());

	if (Time <= 0)
	{
		AfxMessageBox(L"BUG!");
		return L"";
	}

	//此处不用Time.Format...!!!
	if ( bDetail )
	{
		switch ( eTimeInterval )
		{
		case ENTIMinute:// 04/30 14:20
			StrText.Format(_T("%d/%d %d:%02d"),Time.GetMonth(),Time.GetDay(),Time.GetHour(),Time.GetMinute());
			break;
		case ENTIDay:
			if (!bForeign)
			{
				// 非国外计算方法,星期数减一
				int32 iDayOfWeek = Time.GetDayOfWeek();
				iDayOfWeek -= 1;
				if ( 0 == iDayOfWeek)
				{
					iDayOfWeek = 7;
				}
				StrText.Format(_T("%d/%d/%d/%d"),Time.GetYear(),Time.GetMonth(),Time.GetDay(),iDayOfWeek);
			}
			else
			{
				StrText.Format(_T("%d/%d/%d/%d"),Time.GetYear(),Time.GetMonth(),Time.GetDay(),Time.GetDayOfWeek());
			}			
			break;
		case ENTIMinute5:
			StrText.Format(_T("%d/%d %d:%02d"),Time.GetMonth(),Time.GetDay(),Time.GetHour(),Time.GetMinute());
			break;
		case ENTIMinute15:
			StrText.Format(_T("%d/%d %d:%02d"),Time.GetMonth(),Time.GetDay(),Time.GetHour(),Time.GetMinute());
			break;
		case ENTIMinute30:
			StrText.Format(_T("%d/%d %d:%02d"),Time.GetMonth(),Time.GetDay(),Time.GetHour(),Time.GetMinute());
			break;
		case ENTIMinute60:
			StrText.Format(_T("%d/%d %d:%02d"),Time.GetMonth(),Time.GetDay(),Time.GetHour(),Time.GetMinute());
			break;
		case ENTIMinuteUser:
			StrText.Format(_T("%d/%d %d:%02d"),Time.GetMonth(),Time.GetDay(),Time.GetHour(),Time.GetMinute());
			break;
		case ENTIDayUser:
			StrText.Format(_T("%d/%d/%d"),Time.GetYear(),Time.GetMonth(),Time.GetDay());
			break;
		case ENTIWeek:
			StrText.Format(_T("%d/%d/%d"),Time.GetYear(),Time.GetMonth(),Time.GetDay());
			break;
		case ENTIMonth:
			StrText.Format(_T("%d/%d/%d"),Time.GetYear(),Time.GetMonth(),Time.GetDay());
			break;
		case ENTIQuarter:
			StrText.Format(_T("%d/%d/%d"),Time.GetYear(),Time.GetMonth(),Time.GetDay());
			break;
		case ENTIYear:
			StrText.Format(_T("%d/%d/%d"),Time.GetYear(),Time.GetMonth(),Time.GetDay());
			break;
		default:
			break;
		}
	}
	else
	{
		switch ( eTimeInterval )
		{
		case ENTIMinute:// 04/30 14:20
		case ENTIMinute5:
		case ENTIMinute15:
		case ENTIMinute30:
		case ENTIMinute60:
		case ENTIMinuteUser:
			StrText.Format(_T("%d:%02d"),Time.GetHour(),Time.GetMinute());
			break;

		case ENTIDay:
		case ENTIDayUser:
		case ENTIWeek:
			StrText.Format(_T("%d/%d"),Time.GetMonth(),Time.GetDay());
			break;

		case ENTIMonth:
		case ENTIQuarter:
			StrText.Format(_T("%d/%d"),Time.GetYear(),Time.GetMonth());
			break;
		case ENTIYear:
			StrText.Format(_T("%d"),Time.GetYear());
			break;
		default:
			break;
		}

	}
	return StrText;
}

CString Time2StringEX(CGmtTime& TimeNow,CGmtTime& TimePre,E_NodeTimeInterval eTimeInterval)
{
	// 现在的算法: 分钟K 线不显示日期.日K 线,第一个显示年份.年份没有变化的时候,不要重复显示
	// TimePre 为 0 的时候,显示详细时间
	
	CString StrText = L"";
	
	switch ( eTimeInterval )
	{
	case ENTIMinute:// 04/30 14:20
	case ENTIMinute5:
	case ENTIMinute15:
	case ENTIMinute30:
	case ENTIMinute60:
	case ENTIMinuteUser:
		{
			if ( 0 == TimePre.GetTime())
			{
				// 只显示 小时:分钟
				StrText.Format(_T("%02d:%02d"),TimeNow.GetHour(),TimeNow.GetMinute());
			}			
			else if( /*0 == TimePre.GetTime() || */TimeNow.GetDay() != TimePre.GetDay())
			{
				// 详细时间
				StrText.Format(_T("%04d/%02d"),TimeNow.GetMonth(),TimeNow.GetDay());
			}
			else
			{
				// 只显示 小时:分钟
				StrText.Format(_T("%02d:%02d"),TimeNow.GetHour(),TimeNow.GetMinute());
			}
		}			
		break;			
	case ENTIDay:
	case ENTIDayUser:
	case ENTIWeek:
		{
			
			if ( 0 == TimePre.GetTime() )
			{
				// 只显示月份 (第一个节点的处理,还是不要显示那么麻烦算了.不好看)
				StrText.Format(_T("%02d"),TimeNow.GetMonth());
			}
			else if ( /*0 == TimePre.GetTime() ||*/ TimeNow.GetYear() != TimePre.GetYear())
			{
				// 详细时间:
				StrText.Format(_T("%02d/%d/%d"),TimeNow.GetYear(),TimeNow.GetMonth(),TimeNow.GetDay());
			}
			else if ( TimeNow.GetYear() == TimePre.GetYear() && TimeNow.GetMonth() != TimePre.GetMonth() )
			{
				// 只显示月份
				StrText.Format(_T("%02d"),TimeNow.GetMonth());
			}
			else
			{
				// 不显示
				StrText = L"";
			}			
		}
		break;			
	case ENTIMonth:
	case ENTIQuarter:
		{
			if ( 0 == TimePre.GetTime() || TimeNow.GetYear() != TimePre.GetYear())
			{
				// 详细时间: 年
				StrText.Format(_T("%04d"),TimeNow.GetYear());
			}
			else
			{
				// 不显示
				StrText = L"";
			}			
		}
		break;
	case ENTIYear:
		{
			StrText.Format(_T("%04d"),TimeNow.GetYear());
		}
		break;
	default:
		break;
	}
	
	return StrText;
}

void NodeData2KLine ( const CNodeData& NodeData, CKLine& kLine)
{
	kLine.m_TimeCurrent	= CGmtTime(NodeData.m_iID);
	kLine.m_fPriceOpen	= NodeData.m_fOpen;
	kLine.m_fPriceClose	= NodeData.m_fClose;
	kLine.m_fPriceLow	= NodeData.m_fLow;
	kLine.m_fPriceHigh	= NodeData.m_fHigh;
	kLine.m_fPriceAvg	= NodeData.m_fAvg;
	kLine.m_fVolume		= NodeData.m_fVolume;
	kLine.m_fAmount		= NodeData.m_fAmount;
	kLine.m_fHold		= NodeData.m_fHold;
}

void Tick2NodeData ( const CTick& Tick, CNodeData& NodeData)
{
	NodeData.m_iID		= Tick.m_TimeCurrent.m_Time.GetTime();
	NodeData.m_fOpen	= Tick.m_fPrice;
	NodeData.m_fClose	= Tick.m_fPrice;
	NodeData.m_fLow		= Tick.m_fPrice;
	NodeData.m_fHigh	= Tick.m_fPrice;
	NodeData.m_fAvg		= Tick.m_fPrice;
	NodeData.m_fVolume	= Tick.m_fVolume;
	NodeData.m_fAmount	= Tick.m_fVolume*Tick.m_fPrice;
	NodeData.m_fHold	= Tick.m_fHold;
}

void NodeData2Tick ( const CNodeData& NodeData, CTick& Tick)
{
	Tick.m_TimeCurrent.m_Time	= CGmtTime(NodeData.m_iID);
	Tick.m_eTickType	= ETTTrade;
	Tick.m_fPrice		= NodeData.m_fClose;
	Tick.m_fVolume		= NodeData.m_fVolume;
	Tick.m_fHold		= NodeData.m_fHold;
}

void Tick2KLine ( const CTick& Tick, CKLine& kLine)
{
	kLine.m_TimeCurrent	= Tick.m_TimeCurrent.m_Time;
	kLine.m_fPriceOpen	= Tick.m_fPrice;
	kLine.m_fPriceClose	= Tick.m_fPrice;
	kLine.m_fPriceLow	= Tick.m_fPrice;
	kLine.m_fPriceHigh	= Tick.m_fPrice;
	kLine.m_fPriceAvg	= Tick.m_fPrice;
	kLine.m_fVolume		= Tick.m_fVolume;
	kLine.m_fAmount		= Tick.m_fVolume*Tick.m_fPrice;
	kLine.m_fHold		= Tick.m_fHold;

	// 由于kline缺少描述内外盘的数据，用usrisecount来顶替下，仅闪电图有影响？
	kLine.m_usRiseMerchCount = Tick.m_eTickType;
	kLine.m_usFallMerchCount = Tick.m_eTradeKind;
}

void KLine2Tick ( const CKLine& kLine, CTick& Tick)
{
	Tick.m_TimeCurrent.m_Time	= kLine.m_TimeCurrent;
	Tick.m_eTickType	= ETTTrade;
	Tick.m_fPrice		= kLine.m_fPriceClose;
	Tick.m_fVolume		= kLine.m_fVolume;
	Tick.m_fHold		= kLine.m_fHold;
}

///////////////////////////////////////////////////////////////////////////////
T_IndexOut::T_IndexOut()
{
	iTypeEx = 0;
	pPoint = NULL;
	pPointExtra1 = NULL;
	pPointExtra2 = NULL;
	pPointExtra3 = NULL;
	iKlineDrawType = CIndexDrawer::EIKDSByMainKline;	// 跟主图一样的绘制默认
	iColor = 0;
	iThick = 0;
	iType = 0;
	iPointNum = 0;
}

T_IndexOut::~T_IndexOut()
{
	if ( NULL != pPoint )
	{
		DEL_ARRAY(pPoint);		
	}

	if ( NULL != pPointExtra1 )
	{
		DEL_ARRAY(pPointExtra1);		
	}
}




CMerch  * GetMerchAccordExpMerch( CMerch* pMerch )
{

	if ( NULL == pMerch )
	{
		return NULL;
	}

	CGGTongDoc *pDoc = AfxGetDocument();
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return NULL;
	}

	// 默认都调上证指数
	CMerch* pMerchExp = NULL;	
	//m_pAbsCenterManager->GetMerchManager().FindMerch(L"000001", 0, pMerchExp);

	//	
	if ( 1 == pMerch->m_Market.m_Breed.m_iBreedId )
	{				
		if ( 1004 == pMerch->m_Market.m_MarketInfo.m_iMarketId )
		{
			// 2、深圳中小板调用中小板指数
			pAbsCenterManager->GetMerchManager().FindMerch(/*L"399005"*/L"399101", 1000, pMerchExp);
		}
		else if ( 1008 == pMerch->m_Market.m_MarketInfo.m_iMarketId )
		{
			// 3、深圳创业板调用创业板指数
			pAbsCenterManager->GetMerchManager().FindMerch(/*L"399006"*/L"399102", 1000, pMerchExp);
		}
		else
		{
			// 4、其余的全都调用深成指
			pAbsCenterManager->GetMerchManager().FindMerch(L"399001", 1000, pMerchExp);
		}	
	}

	// 默认都调上证指数
	if ( NULL == pMerchExp )
	{
		pAbsCenterManager->GetMerchManager().FindMerch(L"000001", 0, pMerchExp);
	}

	//
	return pMerchExp;
}



//lint --e{429}
bool32 g_formula_compute (  CIoViewChart* pIoViewChart,
						    CChartRegionViewParam* pRegionViewData,
						    CChartRegion* pChartRegion,
							T_MerchNodeUserData* pUserData,
							T_IndexParam* pIndexs,
							int32 iNodeBegin,
							int32 iNodeEnd)
{
	ASSERT(NULL!=pChartRegion);		//创建Curve的Region
	ASSERT(NULL!=pRegionViewData);	//用于创建Nodes
	ASSERT(NULL!=pUserData);		//用于设置给Nodes
	ASSERT(NULL!=pIndexs);			//指定了公式名称等.

	if (!pIndexs->m_bRight)
	{
		return FALSE;
	}

	// iNodeEnd为不可取的终止位置，访问范围为[iNodeBegin, iNodeEnd)
	if ( iNodeEnd < iNodeBegin )
	{		
		return false;
	}

	if ( pUserData->m_aKLinesFull.GetSize() == 0 )
	{	
		return false;
	}

	int32 i,j,iSize;
	CArray<CChartCurve*,CChartCurve*> Curves;
	pIoViewChart->FindCurvesByIndexParam (pIndexs, Curves);
	while ( Curves.GetSize() > 0 )
	{
		CChartCurve* pCurve = Curves.GetAt(0);
		Curves.RemoveAt(0);
		pCurve->GetChartRegion().RemoveCurve(pCurve);
	}

	pIndexs->m_aIndexLines.RemoveAll();

	//////////////////////////////////////////////////////////////////////////////
	//设定初始值
	CArray<CIndexNodeList, CIndexNodeList&>& m_aIndexLines = pIndexs->m_aIndexLines;

	CMerch* pMerch = pUserData->m_pMerchNode;
	if ( NULL == pMerch )
	{
		//ASSERT(0);//没碰到过?
		return false;
	}
	int32 iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;

	TCHAR* strIndexName				= pIndexs->strIndexName;

	//////////////////////////////////////////////////////////////////////////////
	//数据转换
	if ( NULL == pIndexs->pContent )
	{		
		return false;
	}

	// fangz0422 补丁: MA 值非法时候程序崩溃:	
	if (pIndexs->pContent->name == L"MA")
	{
		for (int32 i = 0 ; i < pIndexs->pContent->numPara ; i++)
		{
			if ( pIndexs->pContent->defaultVal[i] <= float(1.0) || pIndexs->pContent->defaultVal[i] >= (float)1000)
			{
				// 超出范围			
				pIndexs->pContent->defaultVal[i] = float(2.0);
			}
		}
	}

	//
	bool32 bSpecialShow = CFormulaLib::instance()->BeSpecialSysIndex(pIndexs->pContent);

	//
	CString StrDefaultParam = pIndexs->pContent->DefaultParamToString();

	//////////////////////////////////////////////////////////////////////////////
	//计算指标

	// 不是该指标的可用周期，不显示指标 chenfj
	uint32 iFlag = 0;
	CFormularContent *pFormulaContent = CFormulaLib::instance()->GetFomular(pIndexs->strIndexName);
	iFlag = NodeTimeIntervalToFlag ( pUserData->m_eTimeIntervalFull );
	if (!CheckFlag(pFormulaContent->flag, iFlag))
	{
		return false;
	}

	//DWORD dwTime = timeGetTime();
	//计算指标
	//DWORD dwTime = timeGetTime();
	CMerch *pExpMerch = GetMerchAccordExpMerch(pMerch);

	MERCH_EXTRA_INFO mei;
	mei.iShowCountInFullList = pUserData->m_iShowCountInFullList;
	mei.iShowPosInFullList = pUserData->m_iShowPosInFullList;
	mei.eTimeIntervalFull = pUserData->m_eTimeIntervalFull;
	mei.iTimeUserMultipleMinutes = pUserData->m_iTimeUserMultipleMinutes;
	mei.iTimeUserMultipleDays = pUserData->m_iTimeUserMultipleDays;
	mei.pMainExpMerchNode = pExpMerch;
	mei.pMerchNode = pUserData->m_pMerchNode;
	T_IndexOutArray* pOut = formula_index(pIndexs->pContent, pUserData->m_aKLinesFull, &mei/*pUserData*/);// zhangtao
	//TRACE(_T("指标计算: %s %dms\r\n"), pIndexs->strIndexName, timeGetTime()-dwTime);
	if ( NULL == pOut )
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////////
	//创建Curve/Nodes
	CString StrGroup = CString(strIndexName) + StrDefaultParam;
	
	// 给分时与闪电图的VOLFS的补丁：如果不是期货，且有POSITION，则忽略POSITION那行
	bool32 bIgnorePosition = false;
	bool32 bIsVOLFSIndex = false;	// 分时闪电的volfs指标，需要特殊处理一些数据
	bool32 bIsTickChart = pIoViewChart->m_iChartType == 2;	// tick图的数据volfs特殊处理
	if ( pUserData->m_pMerchNode != NULL )
	{
		bIsVOLFSIndex = StrGroup.CompareNoCase(_T("VOLFS"))==0;
		bIgnorePosition = bIsVOLFSIndex && !CReportScheme::IsFuture(pUserData->m_pMerchNode->m_Market.m_MarketInfo.m_eMarketReportType);
	}

	if ( bSpecialShow )
	{
		StrGroup = CFormulaLib::instance()->GetSpecialSysIndexGroupName(pIndexs->pContent);
	}

	pIndexs->bScreenAccroding = false;

	if (pOut->iIndexNum > 0)
	{
		CKLine *pKLineDataFullList = (CKLine *)pUserData->m_aKLinesFull.GetData();

		//
		int32 iSpecialParamIndex = 0;

		//
		for ( i = 0; i < pOut->iIndexNum; i ++ )
		{
			CChartCurve* pCurve = NULL;
			CNodeSequence* pNodes = NULL;

			if ( bIgnorePosition )
			{
				// VOLFS补丁，判断中间用到了持仓数据的行, 有则忽略该行 目前仅判断HOLD名称 xl 1103
				CString StrName = pOut->index[i].StrName;
				StrName.MakeUpper();
				if ( StrName.Find(_T("HOLD"), 0) == 0 )
				{
					continue;
				}
			}

			uint32 iCurFlag = CChartCurve::KTypeIndex|CChartCurve::KUseNodesNameAsTitle|CChartCurve::KYTransformByClose;

			if (!pIndexs->bMainIndex)
			{
				AddFlag(iCurFlag, CChartCurve::KindexCmp);
			}

			if ( 0 == i )
			{
				AddFlag(iCurFlag,CChartCurve::KIndex1);
			}

			if (pOut->index[i].StrExtraData == L"SCREEN_ACCORDING" || pIndexs->pContent->BeNeedScreenKLineNum())
			{
				pIndexs->bScreenAccroding = true;
			}

			// 没有线 或者当前这个是主指标的第一条线, 就是dependetcurve
			if ( pChartRegion->GetCurveNum() == 0 || (pChartRegion != pIoViewChart->m_pRegionMain  && pIndexs->bMainIndex && CheckFlag(iCurFlag, CChartCurve::KIndex1)))
			{
				AddFlag(iCurFlag,CChartCurve::KDependentCurve);
			}
			
			//
			pCurve = pChartRegion->CreateCurve(iCurFlag);
			if ( NULL == pCurve )
			{
				return false;
			}
			pCurve->SetGroupTitle(StrGroup);
			CIndexDrawer* pDrawer = new CIndexDrawer(*pIoViewChart, (CIndexDrawer::E_IndexDrawStyle)pOut->index[i].iType, (CIndexDrawer::E_IndexDrawStyleEx)pOut->index[i].iTypeEx, pOut->index[i].iThick, pOut->index[i].iColor, (CIndexDrawer::E_IndexKlineDrawStyle)pOut->index[i].iKlineDrawType);
			pCurve->AttatchDrawer(pDrawer);
			
			//
			pNodes = pRegionViewData->CreateNodes();
			pNodes->SetUserData(pUserData);
			pCurve->AttatchNodes(pNodes);
			
			m_aIndexLines.Add(CIndexNodeList());

			CIndexNodeList &IndexList = m_aIndexLines[m_aIndexLines.GetSize() - 1];
			IndexList.m_pNodesShow = pNodes;
			IndexList.m_pCurve = pCurve;

			iSize = pUserData->m_aKLinesFull.GetSize();
			ASSERT( iNodeBegin >= 0 && iNodeBegin <= iNodeEnd );
			ASSERT( iNodeEnd <= iSize );
		
			float fLast = 0.0f;
			float fYLast = 0.0f;	
			
			if (pOut->index[i].iPointNum > 0)
			{
				int32 iPointNum = pOut->index[i].iPointNum;
				
				IndexList.m_aNodesFull.SetSize(iPointNum);
				CNodeData *IndexListNodeFull = (CNodeData *)IndexList.m_aNodesFull.GetData();
				
				pNodes->m_aNodes.SetSize(iNodeEnd - iNodeBegin);
				CNodeData *pKLineNode = (CNodeData *)pNodes->m_aNodes.GetData();
				int32 iShowCount = 0;

				for (j = 0; j < iPointNum; j++)
				{
					CNodeData NodeData;
					KLine2NodeData(pKLineDataFullList[j], NodeData);

					float fY = pOut->index[i].pPoint[j];

					if ( j <= pOut->index[i].iPointNum -1 )
					{
						fYLast = fY;
					}
					else
					{
						fY = fYLast;
					}

					if ( bIsTickChart && bIsVOLFSIndex )
					{
						// 闪电图的vol颜色特殊处理
						NodeData.m_bClrValid = true;
						switch ( pKLineDataFullList[j].m_usFallMerchCount )
						{
						case CTick::ETKBuy:
							NodeData.m_clrNode = (DWORD)(CNodeData::ENCFColorFall);
							break;
						case CTick::ETKSell:
							NodeData.m_clrNode = (DWORD)(CNodeData::ENCFColorRise);
							break;
						default:
							NodeData.m_clrNode = (DWORD)(CNodeData::ENCFColorKeep);
							break;
						}
					}
					
					if ( pOut->index[i].iType == 14 )
					{
						RemoveFlag(pCurve->m_iFlag, CChartCurve::KYTransformByClose);
						AddFlag(pCurve->m_iFlag, CChartCurve::KYTransformByLowHigh);

						// CIndexDrawer::EIDSDrawKLine
						NodeData.m_fHigh  = pOut->index[i].pPoint[j];
						NodeData.m_fOpen  = pOut->index[i].pPointExtra1[j];
						NodeData.m_fLow   = pOut->index[i].pPointExtra2[j];
						NodeData.m_fClose = pOut->index[i].pPointExtra3[j];

						//
						NodeData.m_fAvg = fY;
						NodeData.m_fVolume = fY;
						NodeData.m_fAmount = fY;
						NodeData.m_fHold = fY;
					}
					else if (pOut->index[i].iType == 20)
					{
						// CIndexDrawer::EIDSDrawBand
						RemoveFlag(pCurve->m_iFlag, CChartCurve::KYTransformByClose);
						AddFlag(pCurve->m_iFlag, CChartCurve::KYTransformByLowHigh);
						
						// CIndexDrawer::EIDSDrawKLine
						NodeData.m_fHigh  = pOut->index[i].pPoint[j];
						NodeData.m_fOpen  = pOut->index[i].pPointExtra1[j];
						
						//
						NodeData.m_fLow   = fY;
						NodeData.m_fClose = fY;
						NodeData.m_fAvg = fY;
						NodeData.m_fVolume = fY;
						NodeData.m_fAmount = fY;
						NodeData.m_fHold = fY;
					}
					else if ( pOut->index[i].iType == 11/*CIndexDrawer::EIDSStickLine*/ )
					{
						RemoveFlag(pCurve->m_iFlag, CChartCurve::KYTransformByClose);
						AddFlag(pCurve->m_iFlag, CChartCurve::KYTransformByLowHigh);
						
						// CIndexDrawer::EIDSDrawKLine
						NodeData.m_fHigh  = max(pOut->index[i].pPoint[j], pOut->index[i].pPointExtra1[j]);
						NodeData.m_fLow   = min(pOut->index[i].pPoint[j], pOut->index[i].pPointExtra1[j]);
						
						//
						NodeData.m_fOpen = fY;
						NodeData.m_fClose = pOut->index[i].pPointExtra1[j];
						NodeData.m_fAvg = fY;
						NodeData.m_fVolume = fY;
						NodeData.m_fAmount = fY;
						NodeData.m_fHold = fY;
						NodeData.m_clrNode = pOut->index[i].iColor;
					}
					else
					{
						NodeData.m_fOpen = fY;
						
						if ( pOut->index[i].iType == 11 /*CIndexDrawer::EIDSStickLine*/ )
						{
							NodeData.m_fClose = pOut->index[i].pPointExtra1[j];
						}
						else
						{
							NodeData.m_fClose = fY;
						}

						NodeData.m_fLow = fY;
						NodeData.m_fHigh = fY;
						NodeData.m_fAvg = fY;
						NodeData.m_fVolume = fY;
						NodeData.m_fAmount = fY;
						NodeData.m_fHold = fY;
					}
					
					if ( !BeValidFloat(fY))
					{
						AddFlag(NodeData.m_iFlag,CNodeData::KValueInvalid);
					}

					if (fY == FLT_EPSILON)
					{
						AddFlag(NodeData.m_iFlag, CNodeData::KDrawNULL);
					}
					
					if (fY == FLT_EPSILON)
					{
						AddFlag(NodeData.m_iFlag, CNodeData::KDrawNULL);
					}

					IndexListNodeFull[j] = NodeData;

					if ( j >= iNodeBegin && j < iNodeEnd )
					{
						pKLineNode[iShowCount++] = NodeData;
						if ( !IsTooSmall(NodeData.m_fClose) && !IsTooBig(NodeData.m_fClose))
						{
							fLast = NodeData.m_fClose;
						}
					}
				}
			}
			
			IndexList.m_StrExtraData = pOut->index[i].StrExtraData;
			
			//
			if (pOut->index[i].iType == 15 || pOut->index[i].iType == 21)
			{		
				IndexList.m_aStrExtraData.Copy(pOut->index[i].aStrExtraData);
			}

			if ( pOut->index[i].iType == 16 )
			{
				// NoTitle
				pNodes->SetName(L"");
				pNodes->SetNamePostfix(L"");
				pNodes->SetNamePrefix(L"");
			}
			else
			{
				CString StrTitle = pOut->index[i].StrName;
				StrTitle.MakeUpper();
				pNodes->SetName(StrTitle);
				StrTitle = _T(":") + Float2String(fLast, iSaveDec, true);
				pNodes->SetNamePostfix(StrTitle);
				
				if ( bSpecialShow )
				{
					if ( 0 == pOut->index[i].iType )
					{
						// 正常的画线
						
						// 按照参数给命名, 没有参数的时候就用默认的方式
						if ( iSpecialParamIndex < pIndexs->pContent->numPara )
						{
							StrTitle.Format(L"MA%.0f", pIndexs->pContent->defaultVal[iSpecialParamIndex]);
							StrTitle.MakeUpper();
							pNodes->SetName(StrTitle);
							
							StrTitle = _T(":") + Float2String(fLast, iSaveDec, true);
							pNodes->SetNamePostfix(StrTitle);
						}
						
						iSpecialParamIndex += 1;
					}
				}
			}			
		}
	}

	DEL(pOut);
	return true;
}

CDelayKeyDown::CDelayKeyDown()
{
	m_iDelayTimesKeyDown	= 100;
	m_iDelayTimesMouseWheel	= 200;

	m_aMapKeyDelay.RemoveAll();
}

CDelayKeyDown::~CDelayKeyDown()
{

}

void CDelayKeyDown::InitialKeyDelayValue(int32* pKeyValue, int32 iKeyNums)
{
	if ( NULL == pKeyValue || 0 == iKeyNums )
	{
		return;
	}

	//
	for ( int32 i = 0; i < iKeyNums; i++ )
	{
		int32 iKeyValue = pKeyValue[i];

		m_aMapKeyDelay[iKeyValue] = (DWORD)(-1);
	}
}

bool32 CDelayKeyDown::TestKeyDelay(int32 iKeyValue)
{
	DWORD dwTimePre;
	if ( !m_aMapKeyDelay.Lookup(iKeyValue, dwTimePre) )
	{
		// 不是同道中人
		return false;
	}

	DWORD dwTimeNow = timeGetTime();

	if ( ((DWORD)(-1)) == dwTimePre )
	{
		// 第一次
		m_aMapKeyDelay.SetAt(iKeyValue, dwTimeNow);
		return false;
	}

	if ( MOUSEWHEEL_DOWN == iKeyValue || MOUSEWHEEL_UP == iKeyValue )
	{
		if ( (dwTimeNow - dwTimePre) <= m_iDelayTimesMouseWheel )
		{
			// 小于时间间隔
			return true;
		}
	}
	else
	{
		if ( (dwTimeNow - dwTimePre) <= m_iDelayTimesKeyDown )
		{
			// 小于时间间隔
			return true;
		}
	}

	m_aMapKeyDelay.SetAt(iKeyValue, dwTimeNow);
	return false;
}

#include "excel.h"
using namespace excel;
#include "comdef.h"
#include "atlbase.h"
static const CString s_ExcelRowNames[] = { 
_T("A"), _T("B"), _T("C"), _T("D"), _T("E"), _T("F"), _T("G"), _T("H"), _T("I"), _T("J"), _T("K"), 
_T("L"), _T("M"), _T("N"), _T("O"), _T("P"), _T("Q"), _T("R"), _T("S"), _T("T"), _T("U"), _T("V"),
_T("W"), _T("X"), _T("Y"), _T("Z"),
"AA", "AB", "AC", "AD", "AE", "AF", "AG", "AH", "AI", "AJ", "AK", "AL", "AM", "AN", "AO", "AP", "AQ",
"AR", "AS", "AT", "AU", "AV", "AW", "AX", "AY", "AZ"
};
void ExportChartToExcel ( T_MerchNodeUserData* pData,CNodeSequence* pNodes,int32 iType )
{
	CMerch* pMerch = pData->m_pMerchNode;
	if ( NULL == pMerch )
	{
		return;
	}
	int32 iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;

	_Application ExcelApp;
	Workbooks workBooks;
	_Workbook workBook;
	Worksheets workSheets;
	_Worksheet workSheet;
	Range CurrRange;

	if (!ExcelApp.CreateDispatch(_T("Excel.Application"), NULL))
	{		
		::MessageBox(GetActiveWindow(), _T("启动Excel程序失败."), AfxGetApp()->m_pszAppName, MB_OK);
		return;
	}
	CString StrFileName;
	CTime curTime = CTime::GetCurrentTime();
	CString StrTime;
	StrTime.Format(_T("%02d年%d月%d日%d时%d分"),curTime.GetYear(),curTime.GetMonth(),curTime.GetDay(),curTime.GetHour(),curTime.GetMinute());

	StrFileName.Format(_T("%s(%s)%s.xls"),
		pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(),
		pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(),
		StrTime.GetBuffer());
	//文件名及路径选择
	CFileDialog dlg(FALSE,_T("xls"),StrFileName,NULL,
		_T("Microsoft Office Excel工作簿(*.xls)|*.xls||"),NULL);
	if(GetVersion()<0x80000000)
	{
		dlg.m_ofn.lStructSize=88;
	}
	else
	{
		dlg.m_ofn.lStructSize=76;
	}

	TCHAR strDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH-1,strDir);
	int32 ret = dlg.DoModal();
	::SetCurrentDirectory(strDir);
	if(ret!=IDOK)
	{
		return;
	}
	StrFileName=dlg.GetPathName();

	//设置为显示
	//ExcelApp.SetVisible(TRUE);

	workBooks.AttachDispatch(ExcelApp.GetWorkbooks());
	workBook.AttachDispatch(workBooks.Add(_variant_t(vtMissing)));
	workSheets=workBook.GetSheets();

	workSheet=workSheets.GetItem(COleVariant((short)1));
//	workSheet.Activate();
	
	//得到全部Cells，此时,CurrRange是cells的集合
	CurrRange.AttachDispatch(workSheet.GetCells());
	
	
	CString StrTitle1 = _T("代码") + pMerch->m_MerchInfo.m_StrMerchCode;
	CString StrTitle2 = pMerch->m_MerchInfo.m_StrMerchCnName;

	CurrRange.SetItem(_variant_t((long)(1)),_variant_t((long)(1)),_variant_t(StrTitle1));
	CurrRange.SetItem(_variant_t((long)(1)),_variant_t((long)(2)),_variant_t(StrTitle2));
	//分时或分笔
	if ( 0 == iType || 2 == iType)
	{
		CurrRange.SetItem(_variant_t((long)(2)),_variant_t((long)(1)),_variant_t(_T("时间")));
		if ( 0 == iType )
		{
			CurrRange.SetItem(_variant_t((long)(2)),_variant_t((long)(2)),_variant_t(_T("均价")));
		}
		{
			CurrRange.SetItem(_variant_t((long)(2)),_variant_t((long)(2)),_variant_t(_T("价格")));
		}
		CurrRange.SetItem(_variant_t((long)(2)),_variant_t((long)(3)),_variant_t(_T("成交量")));

		//导出数据
		for( int32 i = 0; i < pNodes->GetSize(); i ++)
		{
			CNodeData NodeData;
			pNodes->GetAt(i,NodeData);

            CGmtTime cgTmpTime(NodeData.m_iID);
			CString StrText = Time2String(cgTmpTime,pData->m_eTimeIntervalFull,true);
			CurrRange.SetItem(_variant_t((long)(i + 3)),_variant_t((long)(1)),_variant_t(StrText));
	
			StrText = Float2String(NodeData.m_fClose,iSaveDec);
			CurrRange.SetItem(_variant_t((long)(i + 3)),_variant_t((long)(2)),_variant_t(StrText));

			StrText = Float2String(NodeData.m_fVolume,iSaveDec);
			CurrRange.SetItem(_variant_t((long)(i + 3)),_variant_t((long)(3)),_variant_t(StrText));
		}
		Range Col0(CurrRange.GetRange(_variant_t(_T("A1")),_variant_t(_T("A1"))));
		Col0.SetColumnWidth(_variant_t((long)20));

	}

	//K线
	if ( 1 == iType )
	{
		bool32 bNeedVolume = true;

		//
		if (NULL != pData->m_pMerchNode)
		{
			if(ERTMony == pData->m_pMerchNode->m_Market.m_MarketInfo.m_eMarketReportType)
			{
				bNeedVolume = false;
			}
		}
		
		//
		CString StrTitle3 = TimeInterval2String(pData->m_eTimeIntervalFull) + _T("K线");
		CurrRange.SetItem(_variant_t((long)(1)),_variant_t((long)(3)),_variant_t(StrTitle3));

		CurrRange.SetItem(_variant_t((long)(2)),_variant_t((long)(1)),_variant_t(_T("时间")));
		CurrRange.SetItem(_variant_t((long)(2)),_variant_t((long)(2)),_variant_t(_T("开盘价")));
		CurrRange.SetItem(_variant_t((long)(2)),_variant_t((long)(3)),_variant_t(_T("最高价")));
		CurrRange.SetItem(_variant_t((long)(2)),_variant_t((long)(4)),_variant_t(_T("最低价")));
		CurrRange.SetItem(_variant_t((long)(2)),_variant_t((long)(5)),_variant_t(_T("收盘价")));

		if (bNeedVolume)
		{
			CurrRange.SetItem(_variant_t((long)(2)),_variant_t((long)(6)),_variant_t(_T("成交量")));
			CurrRange.SetItem(_variant_t((long)(2)),_variant_t((long)(7)),_variant_t(_T("成交额")));
		}

		//导出数据, 需要截断	
		int32 iBeginIndex = 0;
		int32 iEndIndex = pNodes->GetSize() - 1;
		//
		if (ENTIDay == pData->m_eTimeIntervalCompare 
		 || ENTIDayUser == pData->m_eTimeIntervalCompare 
		 || ENTIWeek == pData->m_eTimeIntervalCompare
		 || ENTIMonth == pData->m_eTimeIntervalCompare
		 || ENTIQuarter == pData->m_eTimeIntervalCompare
		 || ENTIYear == pData->m_eTimeIntervalCompare)		  
		{
			if (pNodes->GetSize() > 200)
			{
				iBeginIndex = iEndIndex - 200 + 1;
				if (iBeginIndex < 0)
				{
					iBeginIndex = 0;
				}
			}
		}

		if (ENTIMinute == pData->m_eTimeIntervalCompare 
		 || ENTIMinute5 == pData->m_eTimeIntervalCompare 
		 || ENTIMinute15 == pData->m_eTimeIntervalCompare
		 || ENTIMinute30 == pData->m_eTimeIntervalCompare
		 || ENTIMinute60 == pData->m_eTimeIntervalCompare
		 || ENTIMinuteUser == pData->m_eTimeIntervalCompare)		  
		{
			if (pNodes->GetSize() > 1500)
			{
				iBeginIndex = iEndIndex - 1500 + 1;
			}

			if (iBeginIndex < 0)
			{
				iBeginIndex = 0;
			}
		}
		
		//
		for( int32 i = iBeginIndex; i <= iEndIndex; i ++)
		{
			CNodeData NodeData;
			pNodes->GetAt(i,NodeData);

			int32 iCellIndex = i - iBeginIndex + 3;

			//
			CGmtTime cgTime(NodeData.m_iID);
			CString StrText = Time2String(cgTime,pData->m_eTimeIntervalFull,true);
			CurrRange.SetItem(_variant_t((long)(iCellIndex)),_variant_t((long)(1)),_variant_t(StrText));
	
			StrText = Float2String(NodeData.m_fOpen,iSaveDec);
			CurrRange.SetItem(_variant_t((long)(iCellIndex)),_variant_t((long)(2)),_variant_t(StrText));

			StrText = Float2String(NodeData.m_fHigh,iSaveDec);
			CurrRange.SetItem(_variant_t((long)(iCellIndex)),_variant_t((long)(3)),_variant_t(StrText));

			StrText = Float2String(NodeData.m_fLow,iSaveDec);
			CurrRange.SetItem(_variant_t((long)(iCellIndex)),_variant_t((long)(4)),_variant_t(StrText));

			StrText = Float2String(NodeData.m_fClose,iSaveDec);
			CurrRange.SetItem(_variant_t((long)(iCellIndex)),_variant_t((long)(5)),_variant_t(StrText));

			if (bNeedVolume)
			{
				StrText = Float2String(NodeData.m_fVolume,iSaveDec);
				CurrRange.SetItem(_variant_t((long)(iCellIndex)),_variant_t((long)(6)),_variant_t(StrText));
				
				StrText = Float2String(NodeData.m_fAmount,iSaveDec);
				CurrRange.SetItem(_variant_t((long)(iCellIndex)),_variant_t((long)(7)),_variant_t(StrText));
			}

			if ( NodeData.m_fOpen > NodeData.m_fClose )
			{
				CString StrCell1,StrCell2;
				StrCell1.Format(_T("B%d"),iCellIndex);
				if (bNeedVolume)
				{
					StrCell2.Format(_T("G%d"),iCellIndex);
				}
				else
				{
					StrCell2.Format(_T("E%d"),iCellIndex);
				}
				
				Range cell(CurrRange.GetRange(_variant_t(StrCell1),_variant_t(StrCell2)));
				Interior it ( cell.GetInterior() );
				it.SetColor(_variant_t((long)RGB(255,0,0)));
			}
			if ( NodeData.m_fOpen < NodeData.m_fClose )
			{
				CString StrCell1,StrCell2;
				StrCell1.Format(_T("B%d"),iCellIndex);
				if (bNeedVolume)
				{
					StrCell2.Format(_T("G%d"),iCellIndex);
				}
				else
				{
					StrCell2.Format(_T("E%d"),iCellIndex);
				}
				Range cell(CurrRange.GetRange(_variant_t(StrCell1),_variant_t(StrCell2)));
				Interior it ( cell.GetInterior() );
				it.SetColor(_variant_t((long)RGB(0,255,0)));
			}
		}
		Range Col0(CurrRange.GetRange(_variant_t(_T("A1")),_variant_t(_T("A1"))));
		Col0.SetColumnWidth(_variant_t((long)20));

	}
	workBook.Close(_variant_t(true),_variant_t(StrFileName),_variant_t(false));
    workBooks.Close();
	
    ExcelApp.Quit();

	::MessageBox(GetActiveWindow(), _T("导出EXCEL成功."), AfxGetApp()->m_pszAppName, MB_OK);
	
}

void ImportExcelToGrid ()
{
	COleVariant covTrue((short)TRUE), 
				covFalse((short)FALSE), 
				covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR); 

	_Application ExcelApp;
	_Workbook workBook;
	Workbooks workBooks;
	_Worksheet workSheet;
	Worksheets workSheets;
	LPDISPATCH lpDisp;
	Range CurrRange;

	long   lUsedRows  = 0;
	long   lUserCols  = 0;
	CString StrFileName;
	CString StrBlockName = _T("");
	StrBlockName = CUserBlockManager::Instance()->GetDefaultServerBlockName();	
	

	if (!ExcelApp.CreateDispatch(_T("Excel.Application")))
	{
		MessageBox(NULL, _T("Error!Creat Excel Application Server Faile!"), NULL, MB_OK);
		return;
	}

	ExcelApp.SetVisible(FALSE); 

	CFileDialog dlg(TRUE,_T("xls"),StrFileName,NULL,_T("Microsoft Office Excel工作簿(*.xls)|*.xls||"),NULL);
	int32 ret = dlg.DoModal();
	if(ret!=IDOK)
	{
		return;
	}
	StrFileName=dlg.GetPathName();

	//加载文件
	workBooks.AttachDispatch(ExcelApp.GetWorkbooks());
	lpDisp = workBooks.Open(StrFileName.GetBuffer(MAX_PATH),covOptional,covOptional,
							covOptional,covOptional,covOptional,covOptional,
							covOptional,covOptional,covOptional,covOptional,
							covOptional,covOptional,covOptional,covOptional);

	//得到Workbook
	workBook.AttachDispatch(lpDisp);
	//得到Worksheets
	workSheets.AttachDispatch(workBook.GetWorksheets());
	//获取_Worksheet
	workSheet.AttachDispatch(workBook.GetActiveSheet());

	//获取已使用单元格矩阵
	CurrRange.AttachDispatch(workSheet.GetUsedRange());//加载已使用的单元格
	CurrRange   =   CurrRange.GetRows();         
	lUsedRows   =   CurrRange.GetCount();     
	CurrRange   =   CurrRange.GetColumns(); 
	lUserCols	=   CurrRange.GetCount();

	//重新加载已使用的单元格 获取数据
	COleVariant vResult;
	Range RangeData;
	RangeData.AttachDispatch(workSheet.GetUsedRange());

	// 其实用软件类型去区分暂时没必要，将来可以用这个来区分
	// 获取代码和名称的列索引
	int iStockNameIndex = 0;
	int iStockCodeIndex = 0;
	for (int i = STOCK_START_ROW; i < lUsedRows; ++i)
	{
		for (int iCol = 1; iCol < lUserCols + 1; ++iCol)
		{
			CString str;
			CurrRange.AttachDispatch(RangeData.GetItem (COleVariant((long)i),COleVariant((long)iCol)).pdispVal );
			vResult =CurrRange.GetValue2();
			if(vResult.vt == VT_BSTR) //字符串
			{
				str=vResult.bstrVal;
				if (0 == str.CompareNoCase(STOCK_NAME))
				{
					iStockNameIndex = iCol;
				}
				else if (0 == str.CompareNoCase(STOCK_CODE))
				{
					iStockCodeIndex = iCol;
				}
			}
		}

		// 拿到代码和名称的列数就没必要继续循环了
		if (0 != iStockCodeIndex && 0 != iStockNameIndex)
		{
			break;
		}
	}	

	if (0 != iStockCodeIndex && 0 != iStockNameIndex)
	{
		// 获取商品名称与代码
		CString  strComp;
		for (int iRow = STOCK_START_COL; iRow < lUsedRows + 1; ++iRow)
		{
			CString strName = _T("");
			CString strCode = _T("");
			CurrRange.AttachDispatch(RangeData.GetItem (COleVariant((long)iRow),COleVariant((long)iStockNameIndex)).pdispVal );
			vResult =CurrRange.GetValue2();

			if(VT_BSTR == vResult.vt) //字符串
			{
				strName = vResult.bstrVal;
			}
			else if (VT_R8 == vResult.vt) //8字节的数字
			{
				strName.Format(L"%06f",vResult.dblVal);
			}

			// 有些人的代码是字符串的，去掉字符串里面的所有非数字
			CurrRange.AttachDispatch(RangeData.GetItem (COleVariant((long)iRow),COleVariant((long)iStockCodeIndex)).pdispVal );
			vResult =CurrRange.GetValue2();
			if(VT_BSTR == vResult.vt) //字符串
			{
				// 这个循环不会跑
				strCode = vResult.bstrVal;
				if (0 == strCode.Find(L"SZ") || 0 == strCode.Find(L"SH"))
				{
					// 去掉SZ,SH字样
					strCode = strCode.Mid(2);
				}
			}
			else if (VT_R8 == vResult.vt) //8字节的数字
			{
				strCode.Format(L"%06.0f",vResult.dblVal);
			}

			// 添加自选股
			CMerch* pMerch   = NULL;
			if (FindTradeMerchByCodeAndName(strCode, strName, pMerch))
			{
				CGGTongDoc *pDoc = AfxGetDocument();
				CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
				CUserBlockManager::Instance()->AddMerchToUserBlock(pMerch, StrBlockName);
			}
		}

		::MessageBox(GetActiveWindow(), _T("导入EXCEL成功."), AfxGetApp()->m_pszAppName, MB_OK);
	}
	else
	{
		::MessageBox(GetActiveWindow(), _T("导入自选股失败"), AfxGetApp()->m_pszAppName, MB_OK);
	}

	//释放
	workBook.Close(_variant_t(true),_variant_t(StrFileName),_variant_t(false));
	workBooks.Close();
	ExcelApp.Quit();
	ExcelApp.ReleaseDispatch();
	workBooks.ReleaseDispatch();
	workBook.ReleaseDispatch();
	workSheets.ReleaseDispatch();
	workSheet.ReleaseDispatch();
	CurrRange.ReleaseDispatch();
}

void ExportGridToExcel ( CGridCtrlSys* pGridCtrl,CGridCtrlSys* pGridBottomCtrl,CString StrDefaultFileName,CString StrTitle1,CString StrTitle2,CString StrTitle3,bool32 bExportOnlyVisible,bool32 bDiscardCol0)
{
	_Application ExcelApp;
	Workbooks workBooks;
	_Workbook workBook;
	Worksheets workSheets;
	_Worksheet workSheet;
	Range CurrRange;

	if (!ExcelApp.CreateDispatch(_T("Excel.Application"), NULL))
	{		
		::MessageBox(GetActiveWindow(), _T("启动Excel程序失败."), AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return;
	}

	CString StrFileName;
	CTime tmpTime = CTime::GetCurrentTime();
	CString StrTime;
	StrTime.Format(_T("%02d年%d月%d日%d时%d分"),tmpTime.GetYear(),tmpTime.GetMonth(),tmpTime.GetDay(),tmpTime.GetHour(),tmpTime.GetMinute());

	StrFileName.Format(_T("%s%s.xls"),StrTitle1.GetBuffer(),StrTime.GetBuffer());

	//文件名及路径选择
	CFileDialog dlg(FALSE,_T("xls"),StrFileName,NULL,_T("Microsoft Office Excel工作簿(*.xls)|*.xls||"),NULL);

	if(GetVersion()<0x80000000)
	{
		dlg.m_ofn.lStructSize=88;
	}
	else
	{
		dlg.m_ofn.lStructSize=76;
	}

	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	CString StrFilePath = CPathFactory::GetPrivateExcelPath(pDoc->m_pAbsCenterManager->GetUserName());
	StrFilePath += StrTitle1;	
	StrFilePath += L".xls";
	
	// 得到Excel 文件夹路径
	TCHAR TStrFilePath[MAX_PATH];
	lstrcpy(TStrFilePath, StrFilePath);
	_tcheck_if_mkdir(TStrFilePath);

	// 备份当前路径
	TCHAR TStrFilePathBk[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH-1, TStrFilePathBk);

	// 设置为Excel 路径
	::SetCurrentDirectory(TStrFilePath);
	dlg.m_ofn.lpstrInitialDir = TStrFilePath;

	int32 ret = dlg.DoModal();

	if(ret!=IDOK)
	{
		return;
	}
	
	// 路径设回去
	::SetCurrentDirectory(TStrFilePathBk);
	StrFileName=dlg.GetPathName();

	//设置为显示
	//ExcelApp.SetVisible(TRUE);

	TRY 
	{
		workBooks.AttachDispatch(ExcelApp.GetWorkbooks());
		workBook.AttachDispatch(workBooks.Add(_variant_t(vtMissing)));
		workSheets=workBook.GetSheets();
		
		workSheet=workSheets.GetItem(COleVariant((short)1));
		//	workSheet.Activate();
		
		//得到全部Cells，此时,CurrRange是cells的集合
		CurrRange.AttachDispatch(workSheet.GetCells());
		
		CurrRange.SetItem(_variant_t((long)(1)),_variant_t((long)(1)),_variant_t(StrTitle1));
		CurrRange.SetItem(_variant_t((long)(1)),_variant_t((long)(2)),_variant_t(StrTitle2));
		CurrRange.SetItem(_variant_t((long)(1)),_variant_t((long)(3)),_variant_t(StrTitle3));
		
		int iLastRow = 0;
		if ( pGridCtrl->GetRowCount() > 0 )
		{
			//表头
			int32 i, iSize = pGridCtrl->GetColumnCount();
			iSize = min(iSize, sizeof(s_ExcelRowNames)/sizeof(s_ExcelRowNames[0]));
			if ( bDiscardCol0 )
			{
				for ( i = 1; i < iSize; i ++ )
				{
					CString StrText = pGridCtrl->GetCell(0,i)->GetText();
					CurrRange.SetItem(_variant_t((long)(2)),_variant_t((long)(i)),_variant_t(StrText));
				}
			}
			else
			{
				for ( i = 0; i < iSize; i ++ )
				{
					CString StrText = pGridCtrl->GetCell(0,i)->GetText();
					CurrRange.SetItem(_variant_t((long)(2)),_variant_t((long)(i+1)),_variant_t(StrText));
				}
			}
			
			int32 iRowStart = 1;
			int32 iRowEnd = pGridCtrl->GetRowCount();
			if ( bExportOnlyVisible )
			{
				iRowStart = 1;
				iRowEnd   = 1;
				if ( pGridCtrl->GetRowCount() > 1 )
				{
					
					CRect RectGrid;
					pGridCtrl->GetClientRect(&RectGrid);
					CCellID cell = pGridCtrl->GetTopleftNonFixedCell();
					if ( cell.row > 0 )
					{
						
						iRowStart = cell.row;
						int32 iVisibleRowNum = 0 ;
						if( pGridCtrl->GetRowCount() <= 1)
						{
							iVisibleRowNum = ( RectGrid.Height() - pGridCtrl->GetRowHeight(0) ) / pGridCtrl->GetRowHeight(0) + 1;
						}
						else
						{
							iVisibleRowNum = ( RectGrid.Height() - pGridCtrl->GetRowHeight(0) ) / pGridCtrl->GetRowHeight(1) + 1;
						}
						
						iRowEnd = iRowStart + iVisibleRowNum;
						if ( iRowEnd > pGridCtrl->GetRowCount() )
						{
							iRowEnd = pGridCtrl->GetRowCount();
						}
					}
				}
				
			}
			int32 iRowCell = 1;
			iLastRow = iRowEnd;
			for ( i = iRowStart; i < iRowEnd; i ++ )
			{
				if ( bDiscardCol0 )
				{
					for ( int32 j = 1; j < pGridCtrl->GetColumnCount(); j ++ )
					{
						CGridCellBase* pCell = pGridCtrl->GetCell(i,j);
						CString StrText = pCell->GetText();
						StrText.TrimLeft(_T("-"));
						COLORREF clr = pCell->GetTextClr();
						CurrRange.SetItem(_variant_t((long)(iRowCell+2)),_variant_t((long)(j)),_variant_t(StrText));					
						CString StrCell;
						StrCell.Format(_T("%s%d"),CString(s_ExcelRowNames[j-1]).GetBuffer(),iRowCell+2);
						Range cell(CurrRange.GetRange(_variant_t(StrCell),_variant_t(StrCell)));
						FontExecl font(cell.GetFont());
						font.SetColor(_variant_t((long)clr));
					}
				}
				else
				{
					for ( int32 j = 0; j < pGridCtrl->GetColumnCount(); j++ )
					{
						if ( j >= sizeof(s_ExcelRowNames)/sizeof(s_ExcelRowNames[0]) )
						{
							break;	// 列太多了，无法继续
						}
						CGridCellBase* pCell = pGridCtrl->GetCell(i,j);
						CString StrText = pCell->GetText();
						COLORREF clr =  RGB(0,0,0);//pCell->GetTextClr();
						if ( pCell->IsKindOf(RUNTIME_CLASS(CGridCellSymbol)) )
						{
							if ( StrText.Find(_T("-")) == 0 )
							{
								clr = RGB(0,127,0);	// 跌
							}
							else if ( StrText.Find(_T("+")) == 0 )
							{
								clr = RGB(255,0,0);	// 涨
							}
							StrText.TrimLeft(_T("-+"));
						}
						
						//StrText.TrimLeft(_T("-+"));
						CurrRange.SetItem(_variant_t((long)(iRowCell+2)),_variant_t((long)(j+1)),_variant_t(StrText));
						CString StrCell;
						StrCell.Format(_T("%s%d"),CString(s_ExcelRowNames[j]).GetBuffer(),iRowCell+2);
						Range cell(CurrRange.GetRange(_variant_t(StrCell),_variant_t(StrCell)));
						FontExecl font(cell.GetFont());
						font.SetColor(_variant_t((long)clr));
					}
				}
				iRowCell ++;
			}
		}

		///////////////////
		if (pGridBottomCtrl && pGridBottomCtrl->GetRowCount() > 0 )
		{
			CString strTitle = _T("自选股");
			CurrRange.SetItem(_variant_t((long)(iLastRow+2)),_variant_t((long)(1)),_variant_t(strTitle));

			//表头
			int32 i, iSize = pGridBottomCtrl->GetColumnCount();
			iSize = min(iSize, sizeof(s_ExcelRowNames)/sizeof(s_ExcelRowNames[0]));
			if ( bDiscardCol0 )
			{
				for ( i = 1; i < iSize; i ++ )
				{
					CString StrText = pGridBottomCtrl->GetCell(0,i)->GetText();
					CurrRange.SetItem(_variant_t((long)(iLastRow+3)),_variant_t((long)(i)),_variant_t(StrText));
				}
			}
			else
			{
				for ( i = 0; i < iSize; i ++ )
				{
					CString StrText = pGridBottomCtrl->GetCell(0,i)->GetText();
					CurrRange.SetItem(_variant_t((long)(iLastRow+3)),_variant_t((long)(i+1)),_variant_t(StrText));
				}
			}
			
			int32 iRowStart = 1;
			int32 iRowEnd = pGridBottomCtrl->GetRowCount();
			if ( bExportOnlyVisible )
			{
				iRowStart = 1;
				iRowEnd   = 1;
				if ( pGridBottomCtrl->GetRowCount() > 1 )
				{
					
					CRect RectGrid;
					pGridBottomCtrl->GetClientRect(&RectGrid);
					CCellID cell = pGridBottomCtrl->GetTopleftNonFixedCell();
					if ( cell.row > 0 )
					{
						
						iRowStart = cell.row;
						int32 iVisibleRowNum = 0 ;
						if( pGridBottomCtrl->GetRowCount() <= 1)
						{
							iVisibleRowNum = ( RectGrid.Height() - pGridBottomCtrl->GetRowHeight(0) ) / pGridBottomCtrl->GetRowHeight(0) + 1;
						}
						else
						{
							iVisibleRowNum = ( RectGrid.Height() - pGridBottomCtrl->GetRowHeight(0) ) / pGridBottomCtrl->GetRowHeight(1) + 1;
						}
						
						iRowEnd = iRowStart + iVisibleRowNum;
						if ( iRowEnd > pGridBottomCtrl->GetRowCount() )
						{
							iRowEnd = pGridBottomCtrl->GetRowCount();
						}
					}
				}
				
			}
			int32 iRowCell = 2 + iLastRow;
			for ( i = iRowStart; i < iRowEnd; i ++ )
			{
				if ( bDiscardCol0 )
				{
					for ( int32 j = 1; j < pGridBottomCtrl->GetColumnCount(); j ++ )
					{
						CGridCellBase* pCell = pGridBottomCtrl->GetCell(i,j);
						CString StrText = pCell->GetText();
						StrText.TrimLeft(_T("-"));
						COLORREF clr = pCell->GetTextClr();
						CurrRange.SetItem(_variant_t((long)(iRowCell+2)),_variant_t((long)(j)),_variant_t(StrText));					
						CString StrCell;
						StrCell.Format(_T("%s%d"),CString(s_ExcelRowNames[j-1]).GetBuffer(),iRowCell+2);
						Range cell(CurrRange.GetRange(_variant_t(StrCell),_variant_t(StrCell)));
						FontExecl font(cell.GetFont());
						font.SetColor(_variant_t((long)clr));
					}
				}
				else
				{
					for ( int32 j = 0; j < pGridBottomCtrl->GetColumnCount(); j ++ )
					{
						if ( j >= sizeof(s_ExcelRowNames)/sizeof(s_ExcelRowNames[0]) )
						{
							break;	// 列太多了，无法继续
						}
						CGridCellBase* pCell = pGridBottomCtrl->GetCell(i,j);
						CString StrText = pCell->GetText();
						COLORREF clr =  RGB(0,0,0);//pCell->GetTextClr();
						if ( pCell->IsKindOf(RUNTIME_CLASS(CGridCellSymbol)) )
						{
							if ( StrText.Find(_T("-")) == 0 )
							{
								clr = RGB(0,127,0);	// 跌
							}
							else if ( StrText.Find(_T("+")) == 0 )
							{
								clr = RGB(255,0,0);	// 涨
							}
							StrText.TrimLeft(_T("-+"));
						}
						
						//StrText.TrimLeft(_T("-+"));
						CurrRange.SetItem(_variant_t((long)(iRowCell+2)),_variant_t((long)(j+1)),_variant_t(StrText));
						CString StrCell;
						StrCell.Format(_T("%s%d"),CString(s_ExcelRowNames[j]).GetBuffer(),iRowCell+2);
						Range cell(CurrRange.GetRange(_variant_t(StrCell),_variant_t(StrCell)));
						FontExecl font(cell.GetFont());
						font.SetColor(_variant_t((long)clr));
					}
				}
				iRowCell ++;
			}
		}
	}
	CATCH_ALL (e)
	{
		// 异常，关闭，提示错误
		workBook.Close(_variant_t(true),_variant_t(StrFileName),_variant_t(false));
		workBooks.Close();
		
		ExcelApp.Quit();
		
		ExcelApp.ReleaseDispatch();
		workBooks.ReleaseDispatch();
		workBook.ReleaseDispatch();
		workSheets.ReleaseDispatch();
		workSheet.ReleaseDispatch();
		CurrRange.ReleaseDispatch();
		
		CString StrErr;
		if ( e->GetErrorMessage(StrErr.GetBuffer(1024), 1024) )
		{
		}
		StrErr.ReleaseBuffer();
		if ( StrErr.IsEmpty() )
		{
			StrErr = _T("非常抱歉！由于未知的原因导致excel导出数据失败");
		}
		AfxMessageBox(StrErr, MB_OK|MB_ICONERROR);
		return;
	}
	END_CATCH_ALL

	
	workBook.Close(_variant_t(true),_variant_t(StrFileName),_variant_t(false));
    workBooks.Close();
	
    ExcelApp.Quit();

	ExcelApp.ReleaseDispatch();
	workBooks.ReleaseDispatch();
	workBook.ReleaseDispatch();
	workSheets.ReleaseDispatch();
	workSheet.ReleaseDispatch();
	CurrRange.ReleaseDispatch();

	::MessageBox(GetActiveWindow(), _T("导出EXCEL成功."), AfxGetApp()->m_pszAppName, MB_OK);	
}

bool32 FindTradeMerchByBreedNameAndCode(IN CString strBreedName, IN const CString strMerchCode, OUT CMerch *&pMerchFound)
{
	pMerchFound = NULL;

	CGGTongDoc *pDoc = AfxGetDocument();
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return false;
	}

	CBreed* pBreed = NULL;
	pAbsCenterManager->GetMerchManager().FindBreed(strBreedName, pBreed);
	if ( NULL == pBreed )
	{
		return false;
	}

	for ( int32 j = 0; j < pBreed->m_MarketListPtr.GetSize(); j++ )
	{
		CMarket* pMarket = pBreed->m_MarketListPtr[j];
		if ( NULL == pMarket )
		{
			continue;
		}

		if (pAbsCenterManager->GetMerchManager().FindMerch(strMerchCode, pMarket->m_MarketInfo.m_iMarketId, pMerchFound))
		{
			return true;
		}
	}

	return false;
}

bool32 FindTradeMerchByCodeAndName(IN const CString &StrTradeCode, IN const CString strMerchName, OUT CMerch *&pMerchFound)
{
	pMerchFound = NULL;

	CGGTongDoc *pDoc = AfxGetDocument();
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return false;
	}

	for ( int32 i = 0; i < pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize(); i++ )
	{
		CBreed* pBreed = pAbsCenterManager->GetMerchManager().m_BreedListPtr[i];
		if ( NULL == pBreed )
		{
			continue;
		}

		for ( int32 j = 0; j < pBreed->m_MarketListPtr.GetSize(); j++ )
		{
			CMarket* pMarket = pBreed->m_MarketListPtr[j];
			if ( NULL == pMarket )
			{
				continue;
			}

			if (pAbsCenterManager->GetMerchManager().FindMerch(StrTradeCode, pMarket->m_MarketInfo.m_iMarketId, pMerchFound))
			{
				if (0 == pMerchFound->m_MerchInfo.m_StrMerchCnName.CompareNoCase(strMerchName))
				{
					return true;
				}
				else
				{
					pMerchFound = NULL;
				}
			}
		}
	}

	return false;
}

static CMapStringToPtr s_ExcelObjects;

typedef struct T_ExcelObjects
{
	_Application ExcelApp;
	Workbooks workBooks;
	_Workbook workBook;
	Worksheets workSheets;
	_Worksheet workSheet;
	int32 iRowStartLast;
	int32 iRowEndLast;
	
	T_ExcelObjects()
	{		
		iRowStartLast = 0;
		iRowEndLast   = 0;
	}
} T_ExcelObjects;

void DealPathChar(CString& StrDir)
{
	for ( int32 iTemp = 0 ; iTemp < StrDir.GetLength(); iTemp++)
	{
		char cEach = StrDir.GetAt(iTemp);
		if ( cEach == '/' || cEach == '\\')
		{
			StrDir.SetAt(iTemp, '\\');	
		}
	}	
}

void StartAsyncExcel ( CGridCtrlSys* pGridCtrl,CString StrGuid, OUT CString& StrFileName)
{
	T_ExcelObjects* pExcel = NULL;
	s_ExcelObjects.Lookup(StrGuid,(void*&)pExcel);
	
	TCHAR TStrCurrentDir[MAX_PATH]	 = {'0'};	
	TCHAR TStrCurrentDirBk[MAX_PATH] = {'0'};

	::GetCurrentDirectory(MAX_PATH-1, TStrCurrentDir);
	::GetCurrentDirectory(MAX_PATH-1, TStrCurrentDirBk);
	
	if ( NULL == pExcel )
	{
		// 文件是否存在
		bool32 bFileExit = true;

		// 新建 excel 对象
		pExcel = new T_ExcelObjects;
		if ( !pExcel->ExcelApp.CreateDispatch(_T("Excel.Application"), NULL) )
		{
			::MessageBox(GetActiveWindow(), _T("启动Excel程序失败."), AfxGetApp()->m_pszAppName, MB_ICONWARNING);			
			DEL(pExcel);
			return;
		}
		
		pExcel->ExcelApp.SetUserControl(FALSE);

		// excel 文件路径
		
		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
		// 相对路径,相对文件名
		CString StrFilePath = CPathFactory::GetPrivateExcelPath(pDoc->m_pAbsCenterManager->GetUserName());	
		DealPathChar(StrFilePath);
		// 确保路径存在,不在的话新建目录
		TCHAR TStrFilePath[MAX_PATH] = {'0'};
		lstrcpy(TStrFilePath, StrFilePath);
		_tcheck_if_mkdir(TStrFilePath);
	
 		CString StrTmpFileName = StrFilePath + StrGuid + L".xls";
		
		// 判断文件是否存在,不在就新建
		if ( -1 == _taccess(StrTmpFileName,0) )
		{			
			bFileExit = false;
		}

		if ( bFileExit )
		{
			// 文件存在的时候,弹出打开文件对话框:

			CString StrFilter = L"xsl文件 (*.xls)|*.xls|All Files (*.*)|*.*||";
			CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, StrFilter, NULL);
			
			if(GetVersion()<0x80000000)
			{
				dlg.m_ofn.lStructSize=88;
			}
			else
			{
				dlg.m_ofn.lStructSize=76;
			}
			
			// 得到的路径字符串中有 '\' 和 '/' ,统一一下,要不设置路径不成功
			CString StrInitialDir = StrFilePath;
			DealPathChar(StrInitialDir);
			dlg.m_ofn.lpstrInitialDir = StrInitialDir;

			int32 ret = dlg.DoModal();
			::SetCurrentDirectory(TStrCurrentDirBk);

			if(ret != IDOK)
			{
				DEL(pExcel);
				return;
			}
			
			StrTmpFileName = dlg.GetPathName();	
			CFile::Remove(StrTmpFileName);
		}

		pExcel->ExcelApp.SetCaption(StrGuid);
		s_ExcelObjects[StrGuid] = pExcel;
	
		//设置为显示
		pExcel->ExcelApp.SetVisible(TRUE);
		pExcel->workBooks.AttachDispatch(pExcel->ExcelApp.GetWorkbooks(),true);

		// 总是要执行这句，先注释判断条件，后面再做区分
		//if ( 0/*bFileExit*/ )
		//{
		//	pExcel->workBook.AttachDispatch(pExcel->workBooks.Add(_variant_t(StrFileName)));
		//}
		//else
		{
			pExcel->workBook.AttachDispatch(pExcel->workBooks.Add(_variant_t(vtMissing)));
		}
		
		pExcel->workSheets=pExcel->workBook.GetSheets();
		pExcel->workSheet=pExcel->workSheets.GetItem(COleVariant((short)1));
		pExcel->workSheet.SetName(StrGuid);
		pExcel->workSheet.Activate();
			
		while ( pExcel->workSheets.GetCount() > 1)
		{
			// 只要一个sheet
			_Worksheet wsSheetToDel = pExcel->workSheets.GetItem(COleVariant((short)2));
			wsSheetToDel.Delete();
		}

		Range CurrRange;
		//得到全部Cells，此时,CurrRange是cells的集合
		CurrRange.AttachDispatch(pExcel->workSheet.GetCells());
		//CurrRange.SetItem(_variant_t((long)(1)),_variant_t((long)(1)),_variant_t(StrTitle));

		//表头
		int32 i, iSize = pGridCtrl->GetColumnCount();
		
		// 总是要执行这句，先注释判断条件，后面再做区分
		//if ( 0 )
		//{
		//	for ( i = 1; i < iSize; i ++ )
		//	{
		//		CString StrText = pGridCtrl->GetCell(0,i)->GetText();
		//		CurrRange.SetItem(_variant_t((long)(1)),_variant_t((long)(i)),_variant_t(StrText));
		//	}
		//}
		//else
		{
			for ( i = 0; i < iSize; i ++ )
			{
				CString StrText = pGridCtrl->GetCell(0,i)->GetText();
				CurrRange.SetItem(_variant_t((long)(1)),_variant_t((long)(i+1)),_variant_t(StrText));
			}
		}

		//		
		// 这个 SaveAs. 使用的当前路径,不是程序的当前路径. 变成了D:/MY DOCUMENTS/...
		// 每次到这里会出现路径问题. 所以这个地方还是用绝对路径好了:
		// (文件不存在的时候就要这样特殊处理,文件存在的时候,从打开对话框中得到的已经是绝对路径了)
		
		CString StrFileNameFull;
		if ( !bFileExit )
		{
			StrFileNameFull= CString(TStrCurrentDirBk);
			CString StrTemp1 = StrTmpFileName.Right(StrTmpFileName.GetLength()-1);
			StrFileNameFull += StrTemp1;
		}
		else
		{
			StrFileNameFull = StrTmpFileName;
		}

		pExcel->workSheet.SaveAs(StrFileNameFull,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing);
		
// 		//pExcel->workBook.Close(_variant_t(true),_variant_t(StrFileName),_variant_t(false));
// 		pExcel->workBooks.Close();
// 		
// 		pExcel->ExcelApp.Quit();
// 		
// 		pExcel->ExcelApp.ReleaseDispatch();
// 		pExcel->workBooks.ReleaseDispatch();
// 		pExcel->workBook.ReleaseDispatch();
// 		pExcel->workSheets.ReleaseDispatch();
// 		pExcel->workSheet.ReleaseDispatch();	
// 		
// 		s_ExcelObjects.RemoveKey(StrGuid);
// 		DEL(pExcel);
	}
}

void ClearAllRows ( CGridCtrlSys* pGridCtrl,T_ExcelObjects* pExcel )
{
	int32 i,j, iStart = 2;
	Range CurrRange;
	//得到全部Cells，此时,CurrRange是cells的集合
	CurrRange.AttachDispatch(pExcel->workSheet.GetCells());
	
	for ( i = pExcel->iRowStartLast; i <= pExcel->iRowEndLast; i ++ )
	{
		for ( j = 1; j < pGridCtrl->GetColumnCount(); j ++ )
		{
			CurrRange.SetItem(_variant_t((long)(iStart)),_variant_t((long)(j)),_variant_t(_T("")));
		}
		iStart ++;
	}
}

void GetVisibleRows ( CGridCtrlSys* pGridCtrl, int32& iRowStart,int32& iRowEnd )
{
	iRowStart = 1;
	iRowEnd   = 1;
	if ( pGridCtrl->GetRowCount() > 1 )
	{
		CRect RectGrid;
		pGridCtrl->GetClientRect(&RectGrid);
		CCellID cell = pGridCtrl->GetTopleftNonFixedCell();
		if ( cell.row > 0 )
		{
			int32 iVisibleRowNum;
			
			if( pGridCtrl->GetRowCount() <= 1)
			{
				iVisibleRowNum = ( RectGrid.Height() - pGridCtrl->GetRowHeight(0) ) / pGridCtrl->GetRowHeight(0) + 1;
			}
			else
			{
				iVisibleRowNum = ( RectGrid.Height() - pGridCtrl->GetRowHeight(0) ) / pGridCtrl->GetRowHeight(1) + 1;
			}
			iRowStart = cell.row;
			iRowEnd = iRowStart + iVisibleRowNum;
			if ( iRowEnd > pGridCtrl->GetRowCount() )
			{
				iRowEnd = pGridCtrl->GetRowCount();
			}
		}
	}
}

bool32 FindExceWindow( CString StrGuid )
{
// 	CWnd *pWndPrev=CWnd::FindWindow(_T("XLMAIN"),NULL);
// 	if ( NULL == pWndPrev )
// 	{
// 		return false;
// 	}
// 	while(pWndPrev)  
// 	{  
// 		pWndPrev=CWnd::FindWindowEx(NULL,hWnd,"","");  
// 	}
	return true;
}

void AsyncGridToExcel(CGridCtrlSys* pGridCtrl,CString StrGuid, const CString& StrFileName, int32 iRow,COLORREF clrRise,COLORREF clrFall,COLORREF clrKeep  )
{
	T_ExcelObjects* pExcel = NULL;
	s_ExcelObjects.Lookup(StrGuid,(void*&)pExcel);
	if ( NULL == pExcel )
	{
		return;
	}

	Range CurrRange;
	TRY
	{
		//得到全部Cells，此时,CurrRange是cells的集合
		LPDISPATCH pActiveWindow = pExcel->ExcelApp.GetActiveWindow();
		
		if ( NULL == pActiveWindow )
		{
			// EXCEL 已经退出了!
			
			pExcel->workBooks.Close();		
			pExcel->workBooks.ReleaseDispatch();
			pExcel->workBook.ReleaseDispatch();
			pExcel->workSheets.ReleaseDispatch();
			pExcel->workSheet.ReleaseDispatch();
			pExcel->ExcelApp.SetUserControl(FALSE);
			
			pExcel->ExcelApp.Quit();
			pExcel->ExcelApp.ReleaseDispatch();
			
			s_ExcelObjects.RemoveKey(StrGuid);
			DEL(pExcel);
			
			{
				// 进程不退,干死你,shit!
				HWND hWnd; 
				hWnd = ::FindWindow(L"XLMain", NULL);  		
				
				if(NULL!=hWnd) 
				{
					DWORD   dwThread; 				
					DWORD	dwPid;
					dwThread = GetWindowThreadProcessId(hWnd, &dwPid );
					HANDLE hPross = OpenProcess(SYNCHRONIZE | PROCESS_ALL_ACCESS, TRUE, dwPid);
					
					if ( NULL != hPross )
					{
						TerminateProcess(hPross, 0);
					}
				} 								
			}
			return;
		}
		else
		{
			pActiveWindow->Release();
			pActiveWindow = NULL;
		}

		CurrRange.AttachDispatch(pExcel->workSheet.GetCells());
		
		//更新全部
		if ( -1 == iRow )
		{
			//清空
			ClearAllRows ( pGridCtrl,pExcel);
			//设置值
			if ( pGridCtrl->GetRowCount() < 2 )
			{
				return;
			}
			int32 i = pGridCtrl->GetColumnCount();
			int32 iRowStart = 1;
			int32 iRowEnd = pGridCtrl->GetRowCount();
			// 总是要执行这句，先注释判断条件，后面再做区分
			//if ( 1 ) //bExportOnlyVisible )
			{
				GetVisibleRows ( pGridCtrl,iRowStart,iRowEnd);
			}
			pExcel->iRowStartLast = iRowStart;
			pExcel->iRowEndLast = iRowEnd;
			int32 iRowCell = 1;
			for ( i = iRowStart; i < iRowEnd; i++ )
			{
				for ( int32 j = 0; j < pGridCtrl->GetColumnCount(); j ++ )
				{
					CGridCellBase* pCell = pGridCtrl->GetCell(i,j);
					CString StrText = pCell->GetText();
					COLORREF clr = pCell->GetTextClr();
					if ( StrText.GetLength() > 0 )
					{
						if ( L'-' == StrText[0] )
						{
							StrText.TrimLeft(_T("-"));
							clr = clrFall;
						}
						
						if ( StrText.GetLength() > 0 )
						{
							if ( L'+' == StrText[0] )
							{
								StrText.TrimLeft(_T("+"));
								clr = clrRise;
							}
						}							
					}
					CurrRange.SetItem(_variant_t((long)(iRowCell+1)),_variant_t((long)(j+1)),_variant_t(StrText));
					CString StrCell;
					StrCell.Format(_T("%s%d"),CString(s_ExcelRowNames[j]).GetBuffer(),iRowCell+1);
					Range cell(CurrRange.GetRange(_variant_t(StrCell),_variant_t(StrCell)));
					FontExecl font(cell.GetFont());
					font.SetColor(_variant_t((long)clr));
				}

				iRowCell ++;
			}
		}
		else
		{
			//更新一行
			if ( iRow >= pExcel->iRowStartLast && iRow < pExcel->iRowEndLast )
			{
				for ( int32 j = 0; j < pGridCtrl->GetColumnCount(); j++ )
				{
					CGridCellBase* pCell = pGridCtrl->GetCell(iRow,j);
					CString StrText = pCell->GetText();
					COLORREF clr = pCell->GetTextClr();
					if ( StrText.GetLength() > 0 )
					{
						if ( L'-' == StrText[0] )
						{
							StrText.TrimLeft(_T("-"));
							clr = clrFall;
						}
						if ( StrText.GetLength() > 0)
						{
							if ( L'+' == StrText[0] )
							{
								StrText.TrimLeft(_T("+"));
								clr = clrRise;
							}
						}						
					}
					CurrRange.SetItem(_variant_t((long)(iRow+1)),_variant_t((long)(j+1)),_variant_t(StrText));
					CString StrCell;
					StrCell.Format(_T("%s%d"),CString(s_ExcelRowNames[j]).GetBuffer(),iRow+1);
					Range cell(CurrRange.GetRange(_variant_t(StrCell),_variant_t(StrCell)));
					FontExecl font(cell.GetFont());
					font.SetColor(_variant_t((long)clr));
				}
			}
		}

		CurrRange.ReleaseDispatch();
	}
	CATCH_ALL (e) 
	{
		
	}
	END_CATCH_ALL
}

void DivAlign ( float fMin, float fMax, int32 iMaxNum, int32& iNum, float* fOut, bool32 bFromMinToMax )
{
	iNum = 0;
	if ( fMin >= fMax ) return;
	if ( iMaxNum < 1 ) return;

	float fSkip = ( fMax - fMin ) / iMaxNum;
	int32 iTemp,iMul = 0;

	if ( fSkip > 1.0 )
	{
		while ( fSkip > 10.0 )
		{
			iMul ++;
			fSkip /= 10.0;
		}
		iTemp = (int32)ceil(fSkip);
		fSkip = (float)iTemp;
		while ( iMul > 0 )
		{
			iMul --;
			fSkip *= 10.0;
		}
	}
	else
	{
		while ( fSkip < 1.0 )
		{
			iMul ++;
			fSkip *= 10.0;
		}
		iTemp = (int32)ceil(fSkip);
		fSkip = (float)iTemp;
		while ( iMul > 0 )
		{
			iMul --;
			fSkip /= 10.0;
		}
		fSkip = fSkip;
	}

	float fStart;
	if ( bFromMinToMax )
	{
		fStart = fMin;
		while ( fStart > fSkip )
		{
			fStart -= fSkip;
		}
		fStart = fMin - fStart + fSkip;
		while ( fStart < fMax && iNum < iMaxNum)
		{
			fOut[iNum] = fStart;
			iNum ++;
			fStart += fSkip;
		}
	}
	else
	{
		fStart = fMax;
		while ( fStart > fSkip )
		{
			fStart -= fSkip;
		}
		fStart = fMax - fStart;
		while ( fStart > fMin  && iNum < iMaxNum)
		{
			fOut[iNum] = fStart;
			iNum ++;
			fStart -= fSkip;
		}
	}

	if (fMin < 0.)
	{
		// 如果最小值小于0,把负数部分的刻度也加上,显示出来
		int32 iNumNew = iNum*2 + 1;
		float fOutNegative [1000];
		
		int32 i = 0;
		for ( i = 0 ; i< iNum; i++)
		{
			fOutNegative[i] = (-1.0)*fOut[iNum-1-i];
			fOutNegative[i+iNum+1] = fOut[i];
		}
		fOutNegative[iNum] = 0.;
		
		iNum = iNumNew;
		for (i = 0 ; i< iNum ; i++)
		{
			fOut[i] = fOutNegative[i];
		}
	}
}

CAutoLockWindowUpdate::CAutoLockWindowUpdate( CWnd *pWndLock )
{
	m_pWndLock = NULL;

	HWND hWnd = pWndLock->GetSafeHwnd();
	ASSERT( IsWindow(hWnd) );
	if ( IsWindow(hWnd) )
	{
		// 		  if (::LockWindowUpdate(hWnd))
		// 		  {
		// 			  m_pWndLock = pWndLock;
		// 		  }
		SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
		m_pWndLock = pWndLock;
	}
}
  
//lint --e{1540}
CAutoLockWindowUpdate::~CAutoLockWindowUpdate()
{
	if ( NULL != m_pWndLock && IsWindow(m_pWndLock->GetSafeHwnd()) )
	{
		//		  ::LockWindowUpdate(NULL);
		SendMessage(m_pWndLock->GetSafeHwnd(), WM_SETREDRAW, TRUE, 0);
	}
}

//////////////////////////////////////////////////////////////////////////
  
CUpdateShowDataChangeHelper::CUpdateShowDataChangeHelper( CUpdateShowDataChangeListener *pListener, T_MerchNodeUserData *pData )
{
	m_pMerchData = pData;
	m_pListener = pListener;
	ASSERT( NULL != m_pListener );
	ASSERT( NULL != m_pMerchData );
	if ( NULL != m_pMerchData && NULL != m_pMerchData->m_pKLinesShow )
	{
		// 获取显示区间的最开始&结束时间
		if ( m_pMerchData->m_pKLinesShow->GetSize() > 0 )
		{
			CNodeData node;
			const int32 iSize = m_pMerchData->m_pKLinesShow->GetSize();
			int32 i=0;
			while ( i < iSize )
			{
				if ( m_pMerchData->m_pKLinesShow->GetAt(i, node) && !CheckFlag(node.m_iFlag, CNodeData::KIdInvalid) )
				{
					m_TimeStartOld = CGmtTime(node.m_iID);
					break;
				}
			}

			i = iSize-1;
			while ( i >= 0 )
			{
				if ( m_pMerchData->m_pKLinesShow->GetAt(i, node) && !CheckFlag(node.m_iFlag, CNodeData::KIdInvalid) )
				{
					m_TimeEndOld = CGmtTime(node.m_iID);
					break;
				}
			}
		}
	}
}

CUpdateShowDataChangeHelper::~CUpdateShowDataChangeHelper()
{
	if ( NULL != m_pListener )
	{
		// 获取现在时间范围
		CGmtTime TimeStart, TimeEnd;
		if ( NULL != m_pMerchData && NULL != m_pMerchData->m_pKLinesShow )
		{
			// 获取显示区间的最开始&结束时间
			if ( m_pMerchData->m_pKLinesShow->GetSize() > 0 )
			{
				CNodeData node;
				const int32 iSize = m_pMerchData->m_pKLinesShow->GetSize();
				int32 i=0;
				while ( i < iSize )
				{
					if ( m_pMerchData->m_pKLinesShow->GetAt(i, node) && !CheckFlag(node.m_iFlag, CNodeData::KIdInvalid) )
					{
						TimeStart = CGmtTime(node.m_iID);
						break;
					}
				}
				
				i = iSize-1;
				while ( i >= 0 )
				{
					if ( m_pMerchData->m_pKLinesShow->GetAt(i, node) && !CheckFlag(node.m_iFlag, CNodeData::KIdInvalid) )
					{
						TimeEnd = CGmtTime(node.m_iID);
						break;
					}
				}
			}
		}

		if ( m_TimeStartOld != TimeStart
			|| m_TimeEndOld != TimeEnd )
		{
			try
			{
				m_pListener->OnShowDataTimeRangeChanged(m_pMerchData); // 变更了
			}
			catch(...)
			{
				TRACE(_T("显示区域变更通知有异常!!"));
			}
		}
	}
}

void DrawTradeImage( Graphics& GraphicsImage, int32 iTradeFlag, const CRect& rectShow, bool32 bStretch )
{
	Image *pImage = GetTradeImage(iTradeFlag);
	if ( NULL != pImage )
	{
		DrawImage(GraphicsImage, pImage, rectShow, 1, 0, bStretch);
	}
}

Image* GetTradeImage( int32 iTradeFlag )
{
	if ( NULL == g_pImageBuy )
	{
		ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TRADEBUY, _T("PNG"), g_pImageBuy);
	}
	if ( NULL == g_pImageSale )
	{
		ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TRADESALE, _T("PNG"), g_pImageSale);
	}
	if ( 1 == iTradeFlag )
	{
		return g_pImageSale;
	}
	return g_pImageBuy;
}

void ReleaseTradeImage()
{
	DEL(g_pImageSale);
	DEL(g_pImageBuy);
}
