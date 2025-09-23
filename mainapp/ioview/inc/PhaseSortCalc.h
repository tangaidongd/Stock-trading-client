#ifndef _PHASESORTCALC_H_
#define _PHASESORTCALC_H_

#include "GmtTime.h"
#include "MerchManager.h"
#include "IoViewBase.h"
//#include "OfflineDataManager.h"
//////////////////////////////////////////////////////////////////////////
// �׶����м���

typedef struct T_PhaseSortCalcReslt
{
	// ���
	float fPreClose;
	float fHigh;
	float fLow;
	float fClose;
	
	float fVol;
	float fAmount;
	float f5DayVolChangeRate;	// 5������
	float fMarketRate;			// �г���
	float fTradeRate;			// ������
}T_PhaseSortCalcReslt;

typedef CArray<T_PhaseSortCalcReslt, const T_PhaseSortCalcReslt &> PhaseSortCalcResultArray;

struct T_PhaseSortCalcRequestData
{
	CMerch *pMerch;		// ��Ҫ�������Ʒ
	float fCircAssert;	// ��ͨ�ɱ� ��
	float fMarketAmount;	// �����г��Ľ��׽��
	CRealtimePrice m_RealTimePrice;		// ���µ�����
};

enum E_PhaseSortCalcResultFlag
{
	EPSCRF_Calculating = 0,	// ���ڼ���
	EPSCRF_OK,	// ��������
	EPSCRF_UserCancel, // �û�ȡ��
	EPSCRF_Error,	// ���д���
};

enum E_PhaseSortCalcMsg
{
	EPSCM_Start = 0,		// ��ʼ���� (w= CalcParam��ַ, l=0
	EPSCM_Pos,				// ���㵽 w= CalcParam��ַ, l=pos
	EPSCM_End,				// ������� w= CalcParam��ַ, l = eResultFlag

	// ����ȱ�ٲ����������г��ȵ����ݣ�������Ҫ��hwndѯ������, send����
	EPSCM_RequestData,		// ����hwnd�����������, ���ڽ�����һ����ͨ�ɱ� w=CalcParam��ַ l=T_PhaseSortCalcRequestData *

	EPSCM_Count
};

typedef struct T_PhaseSortCalcUserSetParam
{
	// ���Թ��û����ò���
	CGmtTime m_TimeStart;
	CGmtTime m_TimeEnd;
	bool32   m_bDoPreWeight;
	CIoViewBase::MerchArray m_aMerchsToCalc;
}T_PhaseSortCalcUserSetParam;


typedef CArray<T_PhaseSortCalcRequestData, const T_PhaseSortCalcRequestData &> PhaseSortCalcDataParamArray;

// ���κ�ͬ����ʩ
// ��װ���� TODO
typedef struct  T_PhaseSortCalcParam
{
	HWND	 m_hWndMsg;	// ������Ϣ�Ĵ���
	UINT	 m_uMsgBase;	// ��Ϣ�Ļ�ֵ

	volatile bool m_bTerminate;	// �жϼ���

	volatile bool m_bAutoDelThread;	// �˳�ʱ�Զ�����߳̾�������û�����ã�������ֶ�ɾ��
	HANDLE   m_hThread;		// �̵߳ľ�� - �ж�ʱ��, ��Ӧ����

	// �������
	CGmtTime m_TimeStart;		// ��ʼʱ��
	CGmtTime m_TimeEnd;		// ����ʱ��

	bool32	m_bDoPreWeight;	// �Ƿ�ǰ��Ȩ

	bool32	m_bNeedRequsetDataParam;		// �Ƿ��ڲ��������ʵ�ʱ������

	CAbsDataManager *m_pAbsDataManager;	// ��������Դ

	// �ݲ�����ͬ���Ĵ���
	CIoViewBase::MerchArray m_aMerchsToCalc;	// Ҫ�������Ʒ�б�
	PhaseSortCalcDataParamArray	m_aDataParamToCalc;	// ����, ���û�л���=0����������

	PhaseSortCalcResultArray m_aCalcResults;	// ����Ľ������, ����Ʒ����һ��

	volatile E_PhaseSortCalcResultFlag	m_eResultFlag;		// ����Ľ��,

	T_PhaseSortCalcParam();

	void CopyResult(const T_PhaseSortCalcParam &Phase);		// �������������ݺ���ƷԴ��������
}T_PhaseSortCalcParam;

// ��ʼ����׶���������
bool32 StartPhaseSortCalc(T_PhaseSortCalcParam *pParam);
void   CancelPhaseSortCalc(T_PhaseSortCalcParam *pParam);	// �ж������
void   ClearPhaseSortCalcParamResource(T_PhaseSortCalcParam *pParam); // ������������, �ͷ��߳���Դ��

#endif //_PHASESORTCALC_H_