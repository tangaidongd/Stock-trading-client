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

// �̶��� 5 ������ (���Ƽ�, ���Ƽ�, �Ƿ��������ʡ���ʱ���,/* ������, ������*/)
#define FIX_ALM_CDS	5

// Ԥ��������
class EXPORT_CLASS CAlarmConditions
{
public:
	// ����
	CAlarmConditions();

	// ��������
	CAlarmConditions(const CAlarmConditions& stSrc);
	
	// ��ֵ
	CAlarmConditions& operator=(const CAlarmConditions& stSrc);


public:
	// �Ƿ��ǳ�ʼ����ʱ������(��֤ÿ����Ʒ��ʼ����ʱ����һ�μ������.���������Ժ󷢲���Ԥ��)
	bool32	m_bInitialCalc;

	// �̶��ļ�������
	float	m_aFixedParams[FIX_ALM_CDS];

	// K ��
	CArray<CKLine, CKLine> m_aKLines;

};

// Ԥ�������ݽṹ
typedef map<CMerch*, CAlarmConditions> mapMerchAlarms;				// ��ƷԤ��
typedef map<CArbitrage*, CAlarmConditions> mapArbitrageAlarms;	// ����Ԥ��
	
typedef CArray<CAlarmNotify*, CAlarmNotify*> listAlarmNotify;

// Ԥ����������
class EXPORT_CLASS CAlarmCenter : public CViewDataListner, public CArbitrageNotify
{
public:
	// Ԥ����ʽ
	enum E_AlarmType
	{
		EATNoSound = 0,		// ����
		EATSysSound,		// ϵͳ����
		EATCustomSound,		// �Զ�������
		EATDialog,			// �Ի���
		EATStatus,			// ״̬��

		EATCounts
	};

public:
	CAlarmCenter(CAbsCenterManager* pCenterManager);
	virtual ~CAlarmCenter();

	// from CViewDataListner
public:

	// �������
	virtual void OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId);

	// from CArbitrageNotify
public:
	
	// ��������
	virtual void OnArbitrageAdd(const CArbitrage& stArbitrage)	{}
	
	// ɾ������
	virtual void OnArbitrageDel(const CArbitrage& stArbitrage);
	
	// �޸�����
	virtual void OnArbitrageModify(const CArbitrage& stArbitrageOld, const CArbitrage& stArbitrageNew);

public:
	
	// ������
	bool32	FromXml();
	bool32	ArbitrageFromXml();

	// д����
	bool32	ToXml();

	// ������Ʒ�̶�����Ԥ��
	void	AddMerchAlarms(CMerch* pMerch, const CAlarmConditions& stAlarmCoditions);

	// �޸���Ʒ�̶�����Ԥ��
	void	ModifyMerchAlarms(CMerch* pMerch, const CAlarmConditions& stAlarmCoditions);

	// ɾ����Ʒ�̶�����Ԥ��
	void	DelMerchAlarms(CMerch* pMerch);

	// ������������Ԥ��
	void	AddArbitrageAlarms(CArbitrage* pstArbitrage, const CAlarmConditions& stAlarmCoditions);
	
	// �޸���������Ԥ��
	void	ModifyArbitrageAlarms(CArbitrage* pstArbitrage, const CAlarmConditions& stAlarmCoditions);
	
	// ɾ����������Ԥ��
	void	DelArbitrageAlarms(CArbitrage* pstArbitrage);

	// ɾ��������ƷԤ��
	void	DelAllMerchAlarms();

	// ɾ����������Ԥ��
	void    DelAllArbitrageAlarms();

	// ���ӹ�ʽԤ��
	void	AddFormularAlarms(CFormularContent* pFormular);
	
	// �޸Ĺ�ʽԤ��
	void	ModifyFormularAlarms(CFormularContent* pFormular);
	
	// ɾ����ʽԤ��
	void	DelFormularAlarms(CFormularContent* pFormular);

	// ɾ�����й�ʽԤ��
	void	DelAllFormularAlarms();

	// ����������
	void	AddNotify(CAlarmNotify* pNotify);
		
	// ɾ��������
	void	DelNotify(CAlarmNotify* pNotify);

	// �õ����ĵ���Ʒ(����ͬ������)
	// iFlag = 0 Ԥ����Ʒ���������еĹ�����Ʒ
	// iFlag = 1 Ԥ����Ʒ���еĹ�����Ʒ
	// iFlag = 2 �������еĹ�����Ʒ
	void	GetAttendMerchs(int32 iFlag, OUT CArray<CMerch*, CMerch*>& aAttendMerchs);

	// �õ���ƷԤ������
	void	GetAlarmMap(OUT mapMerchAlarms& mapAlarms);	

	// �õ�����Ԥ������
	void	GetArbitrageMap(OUT mapArbitrageAlarms& mapArbitrage);

	// �õ���ʽ����
	void	GetAlarmFormulars(OUT CArray<CFormularContent*, CFormularContent*>& aFormulars); 

	// �õ�ĳ��Ʒ��Ԥ����Ϣ
	bool32	GetMerchFixAlarms(IN CMerch* pMerch, OUT CAlarmConditions& stAlarmConditions);

	// �õ�ĳ������Ԥ����Ϣ
	bool32	GetArbitrageFixAlarms(IN CArbitrage* pArbitrage, OUT CAlarmConditions& stAlarmConditions);

	// ���ñ�����־
	void	SetAlarmFlag(bool32 bAlarm);

	// ��ȡ������־
	bool32	GetAlarmFlag() const { return m_bNeedAlarm; }

	// ������ʾ��־
	void	SetPromptFlag(bool32 bPrompt);

	// ��ȡ��ʾ��־
	bool32	GetPromptFlag() const { return m_bPrompt; }

	// ���ñ�����ʽ
	void	SetAlarmType(E_AlarmType eType);
	
	// ��ȡ������ʽ
	E_AlarmType GetAlarmtType() const { return m_eAlarmType; }

	// ���öԻ��򱨾���ʽ
	void	SetAlarmDlgType(bool32 bDlg);

	bool32  GetAlarmDlgType() const {return m_bShowDlg;}	

	// ����״̬��������ʽ
	void	SetAlarmStatusType(bool32 bStatus);

	bool32  GetAlarmStatusType() const {return m_bShowStatus;}	

private:
	// ����ˢ����, ����һ�¿�������
	void    CalcAlarmCdts(CMerch* pMerch);

	// ����ˢ����, ����һ�¿�������
	void    CalcArbitrageAlarmCdts(CMerch* pMerch);	

	// ��������
	void	RequestData(CMerch* pMerch = NULL);

	//
	void	RequestAllArbitrageData();
	void	RequestArbitrageData(CArbitrage* pstArbitrage);

	// �ϲ� K ��, �ѻ�ȡ����K �ߺ�����һ��K �ߺϲ�
	void	CombinKLine(CMerch* pMerch);

	// ֪ͨ
	void	NotifyAlarm(void* pData, CAlarmNotify::E_AlarmNotify eType);
	
private:
	// �Ƿ�ر�Ԥ��
	bool32				m_bNeedAlarm;

	// �Ƿ���ʾ
	bool32				m_bPrompt;

	// ��ʾ��ʽ
	E_AlarmType			m_eAlarmType;

	// �Ƿ�Ի�����ʾ
	bool32				m_bShowDlg;

	// �Ƿ�״̬����ʾ
	bool32				m_bShowStatus;

	// �����ļ�·��
	CString				m_StrCustomAlarm;

	// ViewData ָ��
	CAbsCenterManager*		m_pCenterManager;

	// ֪ͨ
	listAlarmNotify		m_aNotifys;
	
	// N ����Ʒ����Ʒ��Ӧ�Ĺ̶�Ԥ������
	mapMerchAlarms		m_mapAlarams;
	
	// ������Ʒ���õĹ�ʽԤ������
	CArray<CFormularContent*, CFormularContent*> m_aAlarmFormulars;

	// N ��������������Ӧ�Ĺ̶�Ԥ������
	mapArbitrageAlarms	m_mapArbitrageAlarams;
};

#endif // !defined(_ALARMCENTER_H_)
