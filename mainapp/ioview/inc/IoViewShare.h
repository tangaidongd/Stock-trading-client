#ifndef _IO_VIEW_SHARE_H_
#define _IO_VIEW_SHARE_H_

#include "IoViewBase.h"
#include "XTipWnd.h"
#include "MerchManager.h"
#include "Region.h"
#include "ChartRegion.h"
#include "StructKlineView.h"
#include "ChartCurve.h"
#include "CFormularContent.h"

#define  USERSET_MINUET_MIN		(UINT)1
#define  USERSET_MINUET_MAX		(UINT)720
#define  USERSET_DAY_MIN		(UINT)1
#define  USERSET_DAY_MAX		(UINT)365

#define	 MOUSEWHEEL_UP			-1
#define	 MOUSEWHEEL_DOWN		-2

class CMerch;
class CGridCtrlSys;
class CChartRegion;
class CIoViewChart;



// 
typedef struct StrategyInfo
{
	CString			strStrategyName;	// 策略名称
	long long 		llStrategyTime;		// 成交时间			
	UINT			ulSignalType;		// 开平仓种类	0为卖1为买
	float			SugPrice;			// 建议价格

	StrategyInfo()
	{
		strStrategyName = _T("");
		llStrategyTime  = 0;
		ulSignalType	= 0;
		SugPrice		= 0.0;
	}
} T_StrategyInfo;

//同一种Merch的Nodes的UserData
//////////////////////////////////////////////////////////////////////////
// 辅助判断 MerchData中 showData显示时间区域是否变更, 如果变更的话，需要更新叠加商品
class CUpdateShowDataChangeListener
{
public:
	virtual ~CUpdateShowDataChangeListener(){};

	virtual void OnShowDataTimeRangeChanged(T_MerchNodeUserData *pData) = 0;	// 时间区域变化了
};

class CUpdateShowDataChangeHelper
{
public:
	CUpdateShowDataChangeHelper(CUpdateShowDataChangeListener *pListener, T_MerchNodeUserData *pData);
	~CUpdateShowDataChangeHelper();

	CUpdateShowDataChangeListener *m_pListener;
	T_MerchNodeUserData         *m_pMerchData;
	CGmtTime					m_TimeStartOld;
	CGmtTime					m_TimeEndOld;
};

bool32 g_formula_compute ( CIoViewChart* pIoViewChart,
						    CChartRegionViewParam* pRegionViewData,
							CChartRegion* pChartRegion,
							T_MerchNodeUserData* pUserData,
							T_IndexParam* pIndexs,
							int32 iNodeBegin,
							int32 iNodeEnd);

uint32 NodeTimeIntervalToFlag ( E_NodeTimeInterval eTimeInterval );
CString GetWeekString(CTime Time);
CString Time2String ( CGmtTime& Time, E_NodeTimeInterval eTimeInterval,bool32 bDetail=true,bool32 bForeign = true/* 使用外国的计算星期方法(周日为第一天)*/);
void	Time2String (CGmtTime & Time,E_NodeTimeInterval eTimeInterval,CString & StrTimeLine1,CString &StrTimeLine2,bool32 bIoViewTick = false);
//CString TimeInterval2String ( E_NodeTimeInterval eTimeInterval, UINT uiUserMinutes = 0, UINT uiUserDays = 0);
CString KLineTypeBaseToString ( E_KLineTypeBase eKTBase );
CString Time2StringEX(CGmtTime& TimeNow,CGmtTime& TimePre,E_NodeTimeInterval eTimeInterval);

//void KLine2NodeData ( const CKLine& KLine, CNodeData& NodeData);
void NodeData2KLine ( const CNodeData& NodeData, CKLine& kLine);
void Tick2NodeData ( const CTick& Tick, CNodeData& NodeData);
void NodeData2Tick ( const CNodeData& NodeData, CTick& Tick);
void Tick2KLine ( const CTick& Tick, CKLine& kLine);
void KLine2Tick ( const CKLine& kLine, CTick& Tick);

//k线合并
void CombinKLineDayN (  IN CArray<CKLine, CKLine>& KLinesIn, int32 iNum, INOUT CArray<CKLine, CKLine>& KLinesOut);
void CombinKLineDayToWeek (  IN CArray<CKLine, CKLine>& KLinesIn, INOUT CArray<CKLine, CKLine>& KLinesOut);
void CombinKLineMonthN (  IN CArray<CKLine, CKLine>& KLinesIn, int32 iNum, INOUT CArray<CKLine, CKLine>& KLinesOut);

int32 HalfSearch ( CArray<CKLine,CKLine>& KLines,int32 id );
bool32 HalfSearchEqu ( int32 id,int32 t,int32 i, int32& iPos,int32& iSub );
int32 HalfSearchEx ( CArray<CKLine,CKLine>& KLines,int32 id );

int32 HalfSearch ( CArray<CTick,CTick>& Ticks,int32 id );
int32 HalfSearchEx ( CArray<CTick,CTick>& Ticks,int32 id );
void DivAlign ( float fMin, float fMax, int32 iMaxNum, int32& iNum, float* fOut, bool32 bFromMinToMax = true);

//for debug
CString RequestToString ( CMmiReqMerchKLine* pInfo );
CString RequestToString ( CMmiReqMerchTimeSales* pInfo );

void CountNumber(CString StrName);

void SetHourToLast(CGmtTime& cgtTime);
void ExportChartToExcel ( T_MerchNodeUserData* pData, CNodeSequence* pNodes,int32 iType );

void ExportGridToExcel ( CGridCtrlSys* pGridCtrl,CGridCtrlSys* pGridBottomCtrl,CString StrDefaultFileName,CString StrTitle1,CString StrTitle2,CString StrTitle3,bool32 bExportOnlyVisible = true,bool32 bDiscardCol0 = false);
void ImportExcelToGrid ();

bool32 FindTradeMerchByBreedNameAndCode(IN CString strBreedName, IN const CString strMerchCode, OUT CMerch *&pMerchFound);
bool32 FindTradeMerchByCodeAndName(IN const CString &StrTradeCode, IN const CString strMerchName, OUT CMerch *&pMerchFound);

void	DrawTradeImage(Graphics& GraphicsImage,		 // 绘图的神器
							  int32 iTradeFlag,				// 交易标记 0-买 1-卖
							  const CRect& rectShow,		 // 显示的矩形	
 							  bool32 bStretch);  			 // 是否在目标矩形内拉伸
Image	*GetTradeImage(int32 iTradeFlag);		// 交易标记，获取图标

void    ReleaseTradeImage();

class CAutoLockWindowUpdate
{
public:
	CAutoLockWindowUpdate(CWnd *pWndLock);
	~CAutoLockWindowUpdate();

private:
	CWnd *m_pWndLock;
};

class CDelayKeyDown
{
public:
	CDelayKeyDown();
	~CDelayKeyDown();

	void	InitialKeyDelayValue(int32* pKeyValue, int32 iKeyNums);
	bool32  TestKeyDelay(int32 iKeyValue);
	//linhc 20100917添加两个方法
	void    SetiDelayTimesKeyDown(int32 iDelayTimesKeyDown){ m_iDelayTimesKeyDown = iDelayTimesKeyDown; };
	void    SetiDelayTimesMouseWheel(int32 iDelayTimesMouseWheel) { m_iDelayTimesMouseWheel = iDelayTimesMouseWheel; };

private:
	int32	m_iDelayTimesKeyDown;
	int32	m_iDelayTimesMouseWheel;

	CMap<int32, int32, DWORD, DWORD> m_aMapKeyDelay;
};

#endif//_IO_VIEW_SHARE_H_