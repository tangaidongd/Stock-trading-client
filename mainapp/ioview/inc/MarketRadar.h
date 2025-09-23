#ifndef _MARKETRADAR_H_
#define _MARKETRADAR_H_

#include "GmtTime.h"
#include "MerchManager.h"
#include "viewdataListener.h"
#include "BlockConfig.h"
#include <afxmt.h>
#include <set>
#include "IRegionData.h"
using std::set;
#include <map>
using std::map;
//////////////////////////////////////////////////////////////////////////
// �г��״�
enum E_MarketRadarCondition
{
	EMRC_None = 0,				// ��ѡ

	EMRC_TodayNewHigh		= 0x0001,		// �������¸�
	EMRC_TodayNewLow		= 0x0002,			// �������µ�
	EMRC_BigBuy				= 0x0004,				// �������	- ������
	EMRC_BigSell			= 0x0008,				// ��������	- ������
	EMRC_LowOpenRise		= 0x0010,			// �Ϳ�����
	EMRC_HighOpenFall		= 0x0020,			// �߿�����
	EMRC_RiseOver			= 0x0040,				// �Ƿ�ͻ���ٽ���� xx%
	EMRC_FallBelow			= 0x0080,				// �Ƿ������ٽ���� xx%
	EMRC_SingleTradeRiseFall = 0x0100,	// �쳣�����ǵ� xx%
	EMRC_HugeAmount			= 0x0200,			// �޶�ɽ����� xxx��
	EMRC_BigVol				= 0x0400,				// �󵥳ɽ� 	- ������
	EMRC_RiseFallMax		= 0x0800,			// �ǵ�ͣԤ��
	EMRC_BigVolInTime		= 0x1000,			// x �����ڴ󵥳ɽ� > xx�� 	- ������
};

enum E_MarketRadarAlarmVoiceType
{
	// �����澯��ʽ
	EMRAVT_None = 0,		// ������
	EMRAVT_System,			// ϵͳ����
	EMRAVT_User,			// ָ�����������ļ� - �ļ�
};

struct T_MarketRadarSubParameter
{
	DWORD m_dwConditions;			// �������

	// ��������
	float m_fWhatIsBigVol;			// ʲô�Ǵ���(��ɽ���) ��������ͨ�ɱ� xx% (�ްٷ�Ȩ)
	float m_fRiseOverValue;			// �Ƿ��ٽ�ֵ
	float m_fFallBelowValue;		// �����ٽ�ֵ
	float m_fSingleTradeRiseFallValue;		// �쳣�����ǵ�ֵ
	float m_fHugeAmountValue;		// �޶�ɽ�����ֵ
	
	int32	m_iBigVolTimeRange;		// x�����ڴ󵥳ɽ�
	int32	m_iBigVolOverCount;		// > xx��

	// �澯��ʽ ���� & �����б� & ����״̬����˸
	E_MarketRadarAlarmVoiceType m_eVoiceType;
	CString	m_StrVoiceFilePath;		// ָ�������ļ�·�� (*.wav...)

	bool32	m_bShowDlgListAlarm;		// ��ʾ�б�

	T_MarketRadarSubParameter();		// �趨Ĭ�ϳ�ʼֵ
	bool32 operator==(const T_MarketRadarSubParameter &Param) const;
	bool32 CompareCalcParam(const T_MarketRadarSubParameter &Param) const;
};

enum E_MarketRadarMerchRange
{
	EMRMR_HSA = 0,		// ����A
	EMRMR_ZX,			// �ҵ���ѡ
	EMRMR_MerchArray,	// �û�ѡ�����Ʒ����
};


struct T_MarketRadarParameter
{
	T_MarketRadarSubParameter m_ParamSub;	// �Ӳ���
	E_MarketRadarMerchRange	  m_eMerchRange;	// �û�ѡ�����Ʒ��Χ
	MerchArray				m_aMerchs;		// ��Ʒ�б�

	T_MarketRadarParameter(){ m_eMerchRange = EMRMR_HSA; }
	T_MarketRadarParameter(const T_MarketRadarParameter &Param)
		: m_ParamSub(Param.m_ParamSub)
	{ 
		m_eMerchRange = EMRMR_HSA;
		m_aMerchs.Copy( Param.m_aMerchs );
	}
	const T_MarketRadarParameter &operator=(const T_MarketRadarParameter &Param){
		if ( this != &Param )
		{
			m_ParamSub = Param.m_ParamSub;
			m_eMerchRange = Param.m_eMerchRange;
			m_aMerchs.Copy( Param.m_aMerchs );
		}
		return *this;
	}

	bool32	CompareCalcParam(const T_MarketRadarParameter &Param) const;
};

struct T_MarketRadarResult
{
	CMerch *m_pMerch;						// ��Ʒ & ʵʱ��������
	CTick	m_TickLast;						// ���һ�ʳɽ���tick��¼, �б�־��������̣������ɫ
	CGmtTime	m_TimeCalc;					// ����ʱ��
	CGmtTime	m_TimeInit;					// ��Ӧ����Ʒ�ڼ���ʱ�ĳ�ʼ��ʱ��
	int32	m_iRiseFallFlag;				// �ǵ���־, <0 �� 0ƽ >0��
	E_MarketRadarCondition m_eCondition;	// ���������
};
CString	GetMarketRadarResultString(const T_MarketRadarResult &Result);
typedef CMap<CMerch *, CMerch *, T_MarketRadarResult, const T_MarketRadarResult&> MarketRadarResultMap;	// ��������
typedef CArray<T_MarketRadarResult, const T_MarketRadarResult &> MarketRadarResultArray;


class CViewData;

class CMarketRadarListener
{
public:
	virtual	~CMarketRadarListener();

	enum E_AddCause
	{
		EAC_UpdateCondition = 0,	// ����Ʒ�����������Ѿ����
		EAC_EqualCondition,			// ����Ʒ�����������û�б��
	};

	enum E_RemoveCause
	{
		EMC_OverInitTime = 0,		// ������ʼ��ʱ���ɾ��
		EMC_NotSatisfyCondition,	// ���ڷ�������
	};

	// ����֪ͨ�����첽�ģ�����֪ͨ�����������ݿ����Ѿ��ٴα仯��
	virtual void OnAddMarketRadarResult(const MarketRadarResultMap &mapResult, E_AddCause eCause) = 0;	// �̵߳��ý���µĽ����, �첽
	virtual void OnRemoveMarketRadarResult(CMerch *pMerch, E_RemoveCause eCause) = 0;		// ������µ������ų�, �첽
	virtual  void OnClearMarketRadarResult() = 0;			// ����������
};

class CMarketRadarAlarm : public CMarketRadarListener
{
public:
	// Ԥ������
	virtual	 void OnAddMarketRadarResult(const MarketRadarResultMap &mapResult, E_AddCause eCause);	// �̵߳��ý���µĽ����, �첽
	virtual void OnRemoveMarketRadarResult(CMerch *pMerch, E_RemoveCause eCause){};		// ������µ������ų�, �첽
	virtual  void OnClearMarketRadarResult(){};			// ����������
};

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CMRadarMap : public CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>
{
public:
	typedef CMap<KEY, ARG_KEY, VALUE, ARG_VALUE> _MyBase;
	
	CMRadarMap(int nBlockSize = 10) : _MyBase(nBlockSize) {}
	
	VALUE *FindValue(ARG_KEY key)
	{
		// ����key��Ӧ��value�����򷵻�ָ��value��ָ�룬����NULL
		// ��ָ����ܻ��ڱ��map���ݽṹ�Ĳ�����ʧЧ
		ASSERT_VALID(this);
		
		UINT nHash;
		CAssoc* pAssoc = GetAssocAt(key, nHash);
		if (pAssoc == NULL)
			return NULL;  // not in map
		
		return &pAssoc->value;
	}
	
	const VALUE *FindValue(ARG_KEY key) const
	{
		return const_cast<CUserNoteMap *>(this)->FindValue(key);
	}
};

// �״����
// 1.ÿ��t1ʱ�䣬����m�����ݵ����ݣ�ֱ��������Ʒɨ�����
// 2.��Ϣt2ʱ�䣬�ٴο�ʼ1����, ���������δ������Ҹ���Ʒ���ڽ���ʱ����ڣ�����������Ʒ���ظ����������
class CMarketRadarCalc : public CViewDataListner, public CBlockConfigListener
{
public:
	~CMarketRadarCalc();

	static CMarketRadarCalc &Instance();

	bool32	SetParameter(const T_MarketRadarParameter &Param);
	const T_MarketRadarParameter &GetParameter() const { return m_Param; };
	void	GetParameter(OUT T_MarketRadarParameter &Param) { Param = m_Param; };

	bool32	StartCalc();
	void	StopCalc();

	bool32	InitAndStartIf();	// ��ȡ�ϴ����õĽ��������ϴ������еĻ�����ʼ����

	LONG		GetRunFlag(){ return (LONG)InterlockedCompareExchange(/*(PVOID *)&*/&m_lRunFlag, 0, 0); }

	void	RemoveResult(CMerch *pMerch, CMarketRadarListener::E_RemoveCause eCause, CMarketRadarListener *pIgnoreListener = NULL);	// ɾ����Ʒ�ļ���������Ʒ������ʱ������Ʒ���˳�ʼ��ʱ��ʱ��������ü�����
	void	ClearResults(CMarketRadarListener *pIgnoreListener = NULL);		// ������
	void	GetResults(OUT MarketRadarResultMap &mapResults, CMarketRadarListener *pIgnoreListener = NULL); // ��ȡ����Ľ������ʱ���ӽ����ɾ������Ҫ��ʼ������Ʒ
	void	GetResults(OUT MarketRadarResultArray &aResults, CMarketRadarListener *pIgnoreListener = NULL);	// ��ð�ʱ���絽��ļ�����
	void	OnTimerFilterNeedInitMerch(CMarketRadarListener *pIgnoreListener = NULL);		// ���ô˷������˲���Ҫ��ʼ������Ʒ���

	void		SaveStartFlag();		// ����������־

	static	void	ResultMapToArray(const MarketRadarResultMap &mapResult, OUT MarketRadarResultArray &aResults);	// ���ռ���ʱ��������

	static	void		ReadMerchArray(OUT MerchArray &aMerchs);	// ��ȡ������Ʒ�б�, ��xml
	static	void		SaveMerchArray(const MerchArray &aMerchs);	// ���浥����Ʒ�б�, ��xml

	void	AddListener(CMarketRadarListener *pListener);
	void	RemoveListener(CMarketRadarListener *pListener);

	// ����, ��ȡ�����ʵ�����, �޻�����Ӧ�����޳ɽ����޽��ף������Ƿ�����û����ͨ�ɱ��������	
	virtual void OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId);
	
	// k ��, ����ҪK������
	virtual void OnDataRespMerchKLine(int iMmiReqId, IN CMmiRespMerchKLine *pMmiRespMerchKLine){};
	
	// �ֱ�
	virtual void OnDataRespMerchTimeSales(int iMmiReqId, IN const CMmiRespMerchTimeSales *pMmiRespMerchTimeSales);

	virtual bool32 IsAttendCommType( E_CommType eType );

	virtual void  OnBlockConfigChange(int32 iBlockId, E_BlockNotifyType eNotifyType);
	virtual void  OnConfigInitialize(E_InitializeNotifyType eInitializeType){};

private:
	CMarketRadarCalc();
	CMarketRadarCalc(const CMarketRadarCalc &);
	CMarketRadarCalc &operator=(const CMarketRadarCalc &);

	typedef CArray<CTick, const CTick &> TickArray;
	struct T_CalcMerchData
	{
		// ��������Ʒ����
		CMerch			*m_pMerch;	// ��־����Ʒ
		CRealtimePrice	 m_RealTimePrice;	// ʵʱ����
		TickArray		m_aTicks;	// �ֱ�����

		float			m_fRiseFallStopRate;	// ��ͣ����, 0.0fΪû���ǵ�ͣ
		CGmtTime		m_TimeInit;		// ����Ʒ��Ӧ�������ǰ������ʱ��(�������ǰ)

		T_CalcMerchData();
		T_CalcMerchData(const T_CalcMerchData &MerchData);
		const T_CalcMerchData &operator=(const T_CalcMerchData &MerchData);
	};
	typedef CMRadarMap<CMerch *, CMerch *, T_CalcMerchData, const T_CalcMerchData &> CalcMerchDataMap;
	//typedef CMRadarMap<CMerch *, CMerch *, CGmtTime, const CGmtTime &> MerchTimeMap;
	typedef map<CMerch *, CGmtTime>		MerchTimeMap;

	static UINT __cdecl CalcThreadFunc( LPVOID pParam );		// �����߳�
	static UINT __cdecl ReqThreadFunc( LPVOID pParam );			// �����߳�

	void		ReadInitParam();		// ��ȡ��ʼ������, ��Ʒ�б���ô��~~, ������̫��~
	void		SaveInitParam();		// �����ʼ������
	

	LONG		SetRunFlag(LONG flag){ return InterlockedExchange((LPLONG)&m_lRunFlag, flag); }
	

	bool32		AddResult(const T_MarketRadarResult &Result);
	int32		AddResult(const MarketRadarResultMap &Results);

	void FireResultAdd(const MarketRadarResultMap &mapResults, CMarketRadarListener::E_AddCause eCause, CMarketRadarListener *pIgnoreListener = NULL);
	void		FireResultRemove(CMerch *pMerch, CMarketRadarListener::E_RemoveCause eCause, CMarketRadarListener *pIgnoreListener = NULL);
	void		FireResultClear(CMarketRadarListener *pIgnoreListener = NULL);

	void		AddCalcMerchData(CMerch *pMerch, const CRealtimePrice &RealTimePrice);
	void		AddCalcMerchData(CMerch *pMerch, const TickArray &aTicks);
	void		DoAddNewCalcMerchData(INOUT T_CalcMerchData &MerchData);	// ʵ����ӵ������ݶ��У�δͬ��

	bool32		RequestStartViewData();		// ��ʼ������Ʒ���ݣ���ʼ���ȴ������
	bool32		RequestNextViewData();		// ������һ����Ʒ������, true����Ʒ����false�Ѿ�ȫ���������
	bool32		TryRequestWaitEchoData();	// ��������ȴ������е���Ʒ����
	bool32		GetNextNewCalcMerchData(OUT T_CalcMerchData &MerchData, bool32 bPop=true);	// ��ȡ��һ��Ҫ�������Ʒ����, bPopȡ����ɾ��
	void		ClearNewCalcMerchData();	// ���Ҫ�������Ʒ����

	int32		RequestMerchData(const MerchArray &aMerchs);

	CGmtTime	GetNow();

	bool32		IsMerchDataExpire(const T_CalcMerchData &MerchData);	// ��Ʒ�����Ƿ��Ѿ�������Чʱ����

	T_MarketRadarParameter m_Param;
	HANDLE				   m_hThreadCalc;	// �����߳̾��
	HANDLE				   m_hThreadReq;	// �����߳̾��
	volatile	LONG	   m_lRunFlag;	// Ϊ��0���������У�0������

	// ��Ʒ����ֻ��Ҫ�����£�����Ҫǿ��ɾ�����е�����
	CalcMerchDataMap		m_mapCalcMerchDataAll;	// ������Ʒ���ݼ���, ���������ݻ���ʱ��������ȡ��������
	CalcMerchDataMap		m_mapCalcMerchDataNew;	// �·��ص�δ�������Ʒ���ݼ���
	MarketRadarResultMap	m_mapResult;	// �����

	CAbsCenterManager				*m_pAbsCenterManager;

	MerchArray				m_aMerchsWaitReq;	// �ȴ������������Ʒ
	MerchArray				m_aMerchsWaitEcho;	// ��ǰ�ѷ����������ڵȴ����ݻ�������Ʒ
	MerchTimeMap			m_mapMerchReqTimes;	// ÿ����Ʒ����ȥ������ʱ��

	CCriticalSection				m_LockResult;			// �������
	CCriticalSection				m_LockCalcMerchDataNew;	// �½��յ�����Ʒ������
	CCriticalSection				m_LockReq;				// ������
	CEvent							m_EventNewMerchData;		// �Ƿ�����Ʒ���ݿɼ�����

	//CSemaphore						m_SemaphoreMerchData;	// �ж��ٸ�����

	CGmtTime	/*volatile*/			m_TimeCalcStart;		// ���㿪ʼʱ�ķ�����ʱ��
	DWORD								m_dwTimeCalcStart;	// ���㿪ʼʱ�ı��ؼ�ʱ
	CArray<CMarketRadarListener *, CMarketRadarListener *>	m_aListeners;

	CMarketRadarAlarm			m_Alarm;
};


#endif //_MARKETRADAR_H_