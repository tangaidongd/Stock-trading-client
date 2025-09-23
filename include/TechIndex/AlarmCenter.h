// AlarmCenter.h: interface for the CAlarmCenter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_ALARMCENTER_H_)
#define _ALARMCENTER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <map>
#include "CFormularContent.h"
#include "ArbitrageManage.h"
#include "TechExport.h"
#include "EngineCenterBase.h"


using namespace std;

class CMerch;
class CViewData;

// 固定的 5 个条件 (上破价, 下破价, 涨幅、换手率、分时金叉,/* 换手率, 折溢率*/)
#define FIX_ALM_CDS	5

// 预警的条件
class EXPORT_CLASS CAlarmConditions
{
public:
	// 构造
	CAlarmConditions();

	// 拷贝构造
	CAlarmConditions(const CAlarmConditions& stSrc);
	
	// 赋值
	CAlarmConditions& operator=(const CAlarmConditions& stSrc);


public:
	// 是否是初始化的时候计算过(保证每个商品初始化的时候有一次计算机会.以免收盘以后发不出预警)
	bool32	m_bInitialCalc;

	// 固定的几个条件
	float	m_aFixedParams[FIX_ALM_CDS];

	// K 线
	CArray<CKLine, CKLine> m_aKLines;

};

// 预警的数据结构
typedef map<CMerch*, CAlarmConditions> mapMerchAlarms;				// 商品预警
typedef map<CArbitrage*, CAlarmConditions> mapArbitrageAlarms;	// 套利预警
	
typedef CArray<CAlarmNotify*, CAlarmNotify*> listAlarmNotify;

// 预警处理中心
class EXPORT_CLASS CAlarmCenter : public CViewDataListner, public CArbitrageNotify
{
public:
	// 预警方式
	enum E_AlarmType
	{
		EATNoSound = 0,		// 无声
		EATSysSound,		// 系统声音
		EATCustomSound,		// 自定义声音
		EATDialog,			// 对话框
		EATStatus,			// 状态栏

		EATCounts
	};

public:
	CAlarmCenter(CAbsCenterManager* pCenterManager);
	virtual ~CAlarmCenter();

	// from CViewDataListner
public:

	// 行情更新
	virtual void OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId);

	// from CArbitrageNotify
public:
	
	// 增加套利
	virtual void OnArbitrageAdd(const CArbitrage& stArbitrage)	{}
	
	// 删除套利
	virtual void OnArbitrageDel(const CArbitrage& stArbitrage);
	
	// 修改套利
	virtual void OnArbitrageModify(const CArbitrage& stArbitrageOld, const CArbitrage& stArbitrageNew);

public:
	
	// 读配置
	bool32	FromXml();
	bool32	ArbitrageFromXml();

	// 写配置
	bool32	ToXml();

	// 增加商品固定条件预警
	void	AddMerchAlarms(CMerch* pMerch, const CAlarmConditions& stAlarmCoditions);

	// 修改商品固定条件预警
	void	ModifyMerchAlarms(CMerch* pMerch, const CAlarmConditions& stAlarmCoditions);

	// 删除商品固定条件预警
	void	DelMerchAlarms(CMerch* pMerch);

	// 增加套利条件预警
	void	AddArbitrageAlarms(CArbitrage* pstArbitrage, const CAlarmConditions& stAlarmCoditions);
	
	// 修改套利条件预警
	void	ModifyArbitrageAlarms(CArbitrage* pstArbitrage, const CAlarmConditions& stAlarmCoditions);
	
	// 删除套利条件预警
	void	DelArbitrageAlarms(CArbitrage* pstArbitrage);

	// 删除所有商品预警
	void	DelAllMerchAlarms();

	// 删除所有套利预警
	void    DelAllArbitrageAlarms();

	// 增加公式预警
	void	AddFormularAlarms(CFormularContent* pFormular);
	
	// 修改公式预警
	void	ModifyFormularAlarms(CFormularContent* pFormular);
	
	// 删除公式预警
	void	DelFormularAlarms(CFormularContent* pFormular);

	// 删除所有公式预警
	void	DelAllFormularAlarms();

	// 增加收听者
	void	AddNotify(CAlarmNotify* pNotify);
		
	// 删除收听者
	void	DelNotify(CAlarmNotify* pNotify);

	// 得到关心的商品(用于同步推送)
	// iFlag = 0 预警商品和套利所有的关心商品
	// iFlag = 1 预警商品所有的关心商品
	// iFlag = 2 套利所有的关心商品
	void	GetAttendMerchs(int32 iFlag, OUT CArray<CMerch*, CMerch*>& aAttendMerchs);

	// 得到商品预警集合
	void	GetAlarmMap(OUT mapMerchAlarms& mapAlarms);	

	// 得到套利预警集合
	void	GetArbitrageMap(OUT mapArbitrageAlarms& mapArbitrage);

	// 得到公式集合
	void	GetAlarmFormulars(OUT CArray<CFormularContent*, CFormularContent*>& aFormulars); 

	// 得到某商品的预警信息
	bool32	GetMerchFixAlarms(IN CMerch* pMerch, OUT CAlarmConditions& stAlarmConditions);

	// 得到某套利的预警信息
	bool32	GetArbitrageFixAlarms(IN CArbitrage* pArbitrage, OUT CAlarmConditions& stAlarmConditions);

	// 设置报警标志
	void	SetAlarmFlag(bool32 bAlarm);

	// 获取报警标志
	bool32	GetAlarmFlag() const { return m_bNeedAlarm; }

	// 设置提示标志
	void	SetPromptFlag(bool32 bPrompt);

	// 获取提示标志
	bool32	GetPromptFlag() const { return m_bPrompt; }

	// 设置报警方式
	void	SetAlarmType(E_AlarmType eType);
	
	// 获取报警方式
	E_AlarmType GetAlarmtType() const { return m_eAlarmType; }

	// 设置对话框报警方式
	void	SetAlarmDlgType(bool32 bDlg);

	bool32  GetAlarmDlgType() const {return m_bShowDlg;}	

	// 设置状态栏报警方式
	void	SetAlarmStatusType(bool32 bStatus);

	bool32  GetAlarmStatusType() const {return m_bShowStatus;}	

private:
	// 行情刷新了, 计算一下看报警不
	void    CalcAlarmCdts(CMerch* pMerch);

	// 行情刷新了, 计算一下看报警不
	void    CalcArbitrageAlarmCdts(CMerch* pMerch);	

	// 请求数据
	void	RequestData(CMerch* pMerch = NULL);

	//
	void	RequestAllArbitrageData();
	void	RequestArbitrageData(CArbitrage* pstArbitrage);

	// 合并 K 线, 脱机取到的K 线和最新一根K 线合并
	void	CombinKLine(CMerch* pMerch);

	// 通知
	void	NotifyAlarm(void* pData, CAlarmNotify::E_AlarmNotify eType);
	
private:
	// 是否关闭预警
	bool32				m_bNeedAlarm;

	// 是否提示
	bool32				m_bPrompt;

	// 提示方式
	E_AlarmType			m_eAlarmType;

	// 是否对话框提示
	bool32				m_bShowDlg;

	// 是否状态栏提示
	bool32				m_bShowStatus;

	// 声音文件路径
	CString				m_StrCustomAlarm;

	// ViewData 指针
	CAbsCenterManager*		m_pCenterManager;

	// 通知
	listAlarmNotify		m_aNotifys;
	
	// N 个商品和商品对应的固定预警条件
	mapMerchAlarms		m_mapAlarams;
	
	// 所有商品公用的公式预警条件
	CArray<CFormularContent*, CFormularContent*> m_aAlarmFormulars;

	// N 个套利和套利对应的固定预警条件
	mapArbitrageAlarms	m_mapArbitrageAlarams;
};

#endif // !defined(_ALARMCENTER_H_)
