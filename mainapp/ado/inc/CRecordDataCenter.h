#ifndef _RECORD_DATA_CENTER_H
#define _RECORD_DATA_CENTER_H

#include <afxtempl.h>
#include "IoViewShare.h"
#include "ViewDataListener.h"
#include "XTimer.h"
#include "synch.h"
#include "DataBase.h"
#include "Recordset.h"
#include <set>
#include <list>

#define MA_NUMS			3		// ma �ߵ������̶�Ϊ 3 ��

using namespace std;

class CDataBaseProcess;
class CViewData;
class CShowTick;

typedef vector<float> ArrayFloat; 

// ��ʱ��
extern const int32 KiTimerIdSaveData;
extern const int32 KiTimerPeriodSaveData;


//
typedef struct T_MerchNode
{
public:
	T_MerchNode()
	{
		m_iMarketId		= -1;
		m_StrMerchCode	= L"";
	}

	bool T_MerchNode::operator< (const T_MerchNode& stOther) const
	{
		if ( m_iMarketId < stOther.m_iMarketId )
		{
			return true;
		}
		else if ( m_iMarketId == stOther.m_iMarketId )
		{
			return (m_StrMerchCode.CompareNoCase(stOther.m_StrMerchCode) <= 0);
		}

		return false;
	}

	int32		m_iMarketId;
	CString		m_StrMerchCode;

}T_MerchNode;

// �ļ��м�¼�����ڽṹ
typedef struct T_Circles
{
	T_Circles()
	{
		m_iCircle		= 0;
		m_ReqTime		= 0;
		m_StrTableName	= L"";
		m_eIntervalOrignal = ENTICount;
		m_eKLineBase	= EKTBCount;
	}

	bool32 T_Circles::operator<(const T_Circles& stOther) const
	{
		if ( m_iCircle < stOther.m_iCircle )
		{
			return true;
		}
		else if ( m_iCircle == stOther.m_iCircle )
		{
			return (m_ReqTime.GetTime() < stOther.m_ReqTime.GetTime());
		}
		
		return false;
	}

	//
	int32		m_iCircle;
	CGmtTime	m_ReqTime;
	CString		m_StrTableName;

	E_NodeTimeInterval	m_eIntervalOrignal;	// ԭʼ������
	E_KLineTypeBase		m_eKLineBase;		// ʵ�ʶ�Ӧ�� K �߻�������
	
}T_Circles;

typedef list<CMmiReqMerchKLine> ListKLineReqs;
typedef map<CMerch*, ListKLineReqs> mapMerchReqs;

//
class CRecordDataCenter : public CViewDataListner
{
public:
	enum E_ErrorType					// ��������
	{
		EETNone = 0,					// �޴���
		EETFile,						// �ļ�������
		EETBlock,						// ���Ƿ�
		EETTimeInterval,				// ���ڴ���
		EETDataBase,					// ���ݿ��������

		EETCount,
	};

	//
	enum E_WorkState
	{
		EWSNone = 0,					// û�иɻ�
		EWSSH,							// �ڴ��������
		EWSHK,							// �ڴ���۹ɵ�
		EWSBoth,						// ͬʱ������������
	};

	// From CViewDataListner
public:
	
	// ����	
	virtual void OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId);
	
	// Tick
	virtual void OnRealtimeTick(const CMmiPushTick &MmiPushTick, int32 iCommunicationId);

	// K ��
	virtual void OnDataRespMerchKLine(int iMmiReqId, IN CMmiRespMerchKLine *pMmiRespMerchKLine);

public:
	CRecordDataCenter(CAbsCenterManager* pAbsCenterManager);
	~CRecordDataCenter();

	bool32				InitialFromFile();										// �����ļ���ʼ��

	// ��ʼ����
	bool32				StartWork();											
	
	// ��������	
	bool32				StopWork();											

	// �õ������ļ���
	static CString		GetSqlConfigName();										

	// ��ʾ����
	void				PromptDbErrMsg();										
	
	// �����ļ���
	std::string			GeneralFileName();
	
	// ��¼��־
	void				DebugLog(LPCTSTR StrFormat, ...);

	// ȡ��ǰ���������Ʒ
	bool32				GetRequest(OUT CMerch*& pMerch, OUT CMmiReqMerchKLine& stReq);

	// ȡ�ñ������ݵ�ʱ��
	CGmtTime			GetSaveTime(const CString& StrTime);

	// ���ô���¼������
	void				SetSaveMerchData(const CMerchKLineNode& stMerchSave, bool32 bJustProcessSelfCircle);

	// ȡ��¼����
	bool32				GetMerchDataToSave(CMerchKLineNode& stMerchSave);

	// ��¼����, ������ص�����
	void				ClearLoacalSaveMerchData();

	// �õ�K �ߵ�ʱ���ַ���(���ݿ��¼��)
	CString				GetKLineTimeString(const CKLine& KLine);
	CString				GetTimeString(const CGmtTime& Time, bool32 bOnlyDay = false);

	// ���ù���״̬
	void				SetWorkState(E_WorkState eState);

	// ��ȡ����״̬
	E_WorkState			GetWorkState();

	// ��һ������ʱ, ��������ݿ�
	bool32				TruncateTableContent(const CString& StrTableName);

	// ��ѯK ���Ƿ��Ѿ��������ݿ�
	
	
	// ��¼���ݿ�
	
	// ���ɲ���K �ߵ����
	CString				GeneralKLineInsertSql(const CString& StrTableName, CMerch* pMerch, const CArray<CKLine, CKLine>& aKLines, const vector<ArrayFloat>& aMaResults);
	CString				GeneralKLineInsertSql(const CString& StrTableName, CMerch* pMerch, const CKLine& KLine, float fMa1, float fMa2, float fMa3);

	CString				QueryKLineSql(const CString& StrTableName, CMerch* pMerch, const CKLine& KLine);

	bool32				BeKlineInDB(const CString& StrTableName, CMerch* pMerch, const CKLine& KLine);

	CString				GeneralKLineUpdateSql(const CString& StrTableName, CMerch* pMerch, const CKLine& KLine, float fMa1, float fMa2, float fMa3);

	void				UpdateKlineInDB(const CString& StrTableName, CMerch* pMerch, const CKLine& KLine, float fMa1, float fMa2, float fMa3);

	bool32				SaveKLineToDB(CMerch* pMerch, const CString& StrTableName, const CArray<CKLine, CKLine>& aKLines, const vector<ArrayFloat>& aMaResults, bool32 bNeedJudgeSame = false);

	// ��������������ݿ����
	bool32				BeRealtimePriceInDB(CMerch* pMerch);

	CString				QueryRealtimePriceSql(CMerch* pMerch);

	CString				GeneralRealtimePriceInsertSql(CMerch* pMerch);

	CString				GeneralRealtimePriceUpdateSql(CMerch* pMerch);

	// Tick ����������ݿ����
	CString				GeneralTickInsertSql(CMerch* pMerch, const CShowTick& Tick);

	// ��ʱ�����
public:
	void				SetRecordDataCenterTimer(int32 iTimerId, int32 iTimerPeriod);

	void				StopRecordDataCenterTimer(int32 iTimerId);

	void				OnRecordDataCenterTimer(int32 iTimerId);

	void				CheckTimeOut();

	// �߳����
public:
	// �������ݵ��̺߳���
	bool32				BeginReqDataThread();
	static DWORD		WINAPI ThreadRequestData(LPVOID pParam);
	bool32				ThreadRequestData();
	
	// �������ݵ��̺߳���(��һ�γ�ʼ����ʵʱ�ķ���K���ݼ�¼)
	bool32				BeginSaveDataThread();
	static DWORD		WINAPI ThreadSaveData(LPVOID pParam);
	bool32				ThreadSaveData();

	// ��ʱ��¼�������ڵ�����
	bool32				BeginTimerRecordThread();
	static DWORD		WINAPI ThreadTimerRecord(LPVOID pParam);
	bool32				ThreadTimerRecord();

private:
	
	// �����Ƿ�Ϸ�
	bool32				BeValidCircle(int32 iCircle);
	
	// MA �����Ƿ�Ϸ�
	void				ValidMaParms(int32& iMA);

	// ȡ������ MA ����
	int32				GetMaxMa();

	// ����ʱ��
	bool32				GetGmtTime(string StrTimeSrc, OUT CGmtTime& Time);

	// ���ɸ����ڵ�����
	bool32				GeneralRequests();

	// test
	void				TestRecordSqlToFile(const CString& StrSql, char* strFileName);

private:
	bool32				MA_Array(IN CKLine* pDataSrc, IN int iCountSrc, IN int iParam, OUT float*& pfResult);	

private:	
	bool32				m_bStart;									// ������־
	bool32				m_bInitialReqRealtime;						// ����� Tick  ��������һ��ʼ��ȫ������ȥ

	int32				m_iReqID;									// ��ǰ����������ID ��
	bool32				m_bTimeToReqNext;							// �Ƿ����������һ����Ʒ

	E_ErrorType			m_eErrType;									// ��������
	
	E_WorkState			m_eWorkState;								// ����״̬
	LockSingle			m_LockWorkState;			

	CAbsCenterManager*			m_pAbsCenterManager;								// CAbsCenterManager

	FILE*				m_pFile;									// ��־	
	LockSingle			m_LockDebugLog;								// д�ļ�����

	set<CMerch*>		m_aMerchs;									// ���е���Ʒ	
	LockSingle			m_LockaMerchs;
	
	DWORD				m_dwTimeRecord;								// ��һ�ζ�ʱ��¼���ݵ�ʱ��
	LockSingle			m_LockTimeRecord;									

	set<T_Circles>		m_aCircles;									// ����
	bool32				m_b1Min;									// �Ƿ��� 1 ���ӵ�����

	int32				m_aMaParams[MA_NUMS];						// �ֶ�
	LockSingle			m_LockaFields;

	bool32				m_bInitialSaveFinish;						// ��ʼ���������ݹ����Ƿ����

	vector<CMmiReqMerchKLine>	m_aReqs;							// ÿ����Ʒ��Ҫ���ļ�������(����ֻ�����˹ؼ����ֶ�ֵ, ʵ�������ʱ����Ҫ������Ʒ��Ϣ��)
	
	map<E_KLineTypeBase, int32> m_mapMaxKLineCount;					// ÿ��������Ҫ��K ����С��Ŀ

	//
	mapMerchReqs		m_mapMerchsForReq;							// �����������Ʒ����(�ʼ����m_aMerchs, ���������ݼ���0)
	LockSingle			m_LockMerchsForReq;								

	bool32				m_bstMerchSaveValid;						// ����¼�������Ƿ�Ϸ�
	LockSingle			m_LockFlagstMerchSaveValid;				

	bool32				m_bJustProcessSelfCircle;					// �Ƿ�ֻ�������ڵ�(��ʼ����ʱ��Ϊ false. ������ʵʱ���ݵ�ʱ��Ϊ true. ��ʱ�� 1 ���Ӽ�¼һ�����ݵ�ʱ��Ϊfalse)
	LockSingle			m_LockJustProcessSelfCircle;				

	CMerchKLineNode		m_stMerchSave;								// ����¼������
	LockSingle			m_LockstMerchSave;							// ��

	DWORD				m_dwTimeSendReq;							// �������ʱ��

	// ���ݿ����:
	CDataBase*			m_pDbProcess;								// ���ݿ⴦������
	CString				m_StrDbAddress;								// ��ַ
	CString				m_StrDbName;								// ����
	DWORD				m_dwDbPort;									// �˿�
	CString				m_StrDbUserName;							// �û���		
	CString				m_StrDbPassword;							// ����

	// �߳����
	HANDLE				m_hThreadReqData;							// �����߳�
	HANDLE				m_hThreadSaveData;							// ȡ�����߳�
	HANDLE				m_hThreadTimerRecord;						// ��ʱ��¼���߳�

	bool32				m_bThreadReqExit;							// �����߳��˳���־
	bool32				m_bThreadSaveExit;							// ȡ�����߳��˳���־
	bool32				m_bThreadTimerRecordExit;					// ��ʱ��¼�߳��˳���־
};

#endif // _RECORD_DATA_CENTER_H