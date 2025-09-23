#ifndef _PHASESORTCALC_H_
#define _PHASESORTCALC_H_

#include "GmtTime.h"
#include "MerchManager.h"
#include "IoViewBase.h"
//#include "OfflineDataManager.h"
//////////////////////////////////////////////////////////////////////////
// 阶段排行计算

typedef struct T_PhaseSortCalcReslt
{
	// 结果
	float fPreClose;
	float fHigh;
	float fLow;
	float fClose;
	
	float fVol;
	float fAmount;
	float f5DayVolChangeRate;	// 5日量变
	float fMarketRate;			// 市场比
	float fTradeRate;			// 换手率
}T_PhaseSortCalcReslt;

typedef CArray<T_PhaseSortCalcReslt, const T_PhaseSortCalcReslt &> PhaseSortCalcResultArray;

struct T_PhaseSortCalcRequestData
{
	CMerch *pMerch;		// 需要请求的商品
	float fCircAssert;	// 流通股本 股
	float fMarketAmount;	// 整个市场的交易金额
	CRealtimePrice m_RealTimePrice;		// 最新的行情
};

enum E_PhaseSortCalcResultFlag
{
	EPSCRF_Calculating = 0,	// 正在计算
	EPSCRF_OK,	// 正常结束
	EPSCRF_UserCancel, // 用户取消
	EPSCRF_Error,	// 运行错误
};

enum E_PhaseSortCalcMsg
{
	EPSCM_Start = 0,		// 开始计算 (w= CalcParam地址, l=0
	EPSCM_Pos,				// 计算到 w= CalcParam地址, l=pos
	EPSCM_End,				// 计算结束 w= CalcParam地址, l = eResultFlag

	// 由于缺少财务数据与市场比的数据，所以需要向hwnd询问数据, send发出
	EPSCM_RequestData,		// 请求hwnd给与财务数据, 现在仅请求一个流通股本 w=CalcParam地址 l=T_PhaseSortCalcRequestData *

	EPSCM_Count
};

typedef struct T_PhaseSortCalcUserSetParam
{
	// 可以供用户设置参数
	CGmtTime m_TimeStart;
	CGmtTime m_TimeEnd;
	bool32   m_bDoPreWeight;
	CIoViewBase::MerchArray m_aMerchsToCalc;
}T_PhaseSortCalcUserSetParam;


typedef CArray<T_PhaseSortCalcRequestData, const T_PhaseSortCalcRequestData &> PhaseSortCalcDataParamArray;

// 无任何同步措施
// 封装成类 TODO
typedef struct  T_PhaseSortCalcParam
{
	HWND	 m_hWndMsg;	// 接收消息的窗口
	UINT	 m_uMsgBase;	// 消息的基值

	volatile bool m_bTerminate;	// 中断计算

	volatile bool m_bAutoDelThread;	// 退出时自动清除线程句柄，如果没有设置，则必须手动删除
	HANDLE   m_hThread;		// 线程的句柄 - 中断时用, 不应访问

	// 计算参数
	CGmtTime m_TimeStart;		// 开始时间
	CGmtTime m_TimeEnd;		// 结束时间

	bool32	m_bDoPreWeight;	// 是否前复权

	bool32	m_bNeedRequsetDataParam;		// 是否在参数不合适的时候请求

	CAbsDataManager *m_pAbsDataManager;	// 离线数据源

	// 暂不进行同步的处理
	CIoViewBase::MerchArray m_aMerchsToCalc;	// 要计算的商品列表
	PhaseSortCalcDataParamArray	m_aDataParamToCalc;	// 参数, 如果没有或者=0，将会请求

	PhaseSortCalcResultArray m_aCalcResults;	// 计算的结果队列, 与商品队列一致

	volatile E_PhaseSortCalcResultFlag	m_eResultFlag;		// 计算的结果,

	T_PhaseSortCalcParam();

	void CopyResult(const T_PhaseSortCalcParam &Phase);		// 拷贝计算结果数据和商品源到这里来
}T_PhaseSortCalcParam;

// 开始计算阶段排行数据
bool32 StartPhaseSortCalc(T_PhaseSortCalcParam *pParam);
void   CancelPhaseSortCalc(T_PhaseSortCalcParam *pParam);	// 中断其计算
void   ClearPhaseSortCalcParamResource(T_PhaseSortCalcParam *pParam); // 计算结束后清空, 释放线程资源等

#endif //_PHASESORTCALC_H_