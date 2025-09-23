// OffLineData.h: interface for the COffLineData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_OFFLINEDATA_H_)
#define _OFFLINEDATA_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <vector>
#include "GmtTime.h"
#include "MerchManager.h"
#include "ViewDataListener.h"
#include "sharestruct.h"
#include "synch.h"
#include "DataCenterExport.h"

using namespace std;
//

class CViewData;
struct T_OffLineRespKLineData;

typedef CArray<CRealtimePrice, CRealtimePrice> ListRealtimeData;
typedef CArray<T_OffLineRespKLineData, const T_OffLineRespKLineData&> ListKLineData;

// ���صĲ�������ṹ
typedef struct T_DownLoadOffLine
{
public:
	T_DownLoadOffLine()
	{
		Clear();
	}

	void Clear()
	{
		m_aMerchs.clear();
		m_eReqType  = ECTCount;
		m_TimeBeing = 0;
		m_TimeEnd	= 0;
		m_eKLineBase= EKTBCount;
	}

	//
	E_CommType		m_eReqType;		// ����
	vector<CMerch*> m_aMerchs;		// ��Ʒ
	CGmtTime		m_TimeBeing;	// ��ʼʱ��
	CGmtTime		m_TimeEnd;		// ��ֹʱ��
	E_KLineTypeBase m_eKLineBase;	// K ������

}T_DownLoadOffLine;

// ���������k ������
typedef struct T_OffLineRespKLineData
{
public:
	int32					m_iRespID;			// ������ID ��
	int32					m_iMarketId;		// �г����
	CString					m_StrMerchCode;		// ��Ʒ����
	E_KLineTypeBase			m_eKLineTypeBase;	// �����K������
	CArray<CKLine, CKLine>	m_aKLines;			// K ��
	
	//
	T_OffLineRespKLineData()
	{
		m_iRespID		= -1;
		m_iMarketId		= -1;
		m_StrMerchCode	= L"";
		m_eKLineTypeBase= EKTBDay;
		m_aKLines.RemoveAll();
	}
	
	//
	T_OffLineRespKLineData(const T_OffLineRespKLineData& Data)
	{
		m_iRespID		= Data.m_iRespID;
		m_iMarketId		= Data.m_iMarketId;
		m_StrMerchCode	= Data.m_StrMerchCode;
		m_eKLineTypeBase= Data.m_eKLineTypeBase;
		m_aKLines.Copy(Data.m_aKLines);
	}
	
	T_OffLineRespKLineData& operator= (const T_OffLineRespKLineData& Data)
	{
		if ( &Data == this )
		{
			return *this;
		}
		
		m_iRespID		= Data.m_iRespID;
		m_iMarketId		= Data.m_iMarketId;
		m_StrMerchCode	= Data.m_StrMerchCode;
		m_eKLineTypeBase= Data.m_eKLineTypeBase;
		m_aKLines.Copy(Data.m_aKLines);
		
		return *this;
	}
	
}T_OffLineRespKLineData;

// ��������� F10 ����
typedef struct T_OffLineRespF10Data
{
	
}T_OffLineRespF10Data;

// �������ص��¼�֪ͨ
class DATACENTER_DLL_EXPORT COffLineDataNotify
{
public:
	// ���󶼷�����
	virtual void OnAllRequestSended(E_CommType eType) = 0;

	// ��������
	virtual void OnDataDownLoading(CMerch* pMerch, E_CommType eType) = 0;

	// ���سɹ�
	virtual void OnDataDownLoadFinished(E_CommType eType) = 0;

	// ����ʧ��
	virtual void OnDataDownLoadFailed(const CString& StrErrMsg) = 0;
};

// �ѻ�����: �ܹ��� K ��, ����, F10 �⼸������. K �ߺ��������ݽ��� Fileengine ȥ����, F10�Լ���д
class DATACENTER_DLL_EXPORT COffLineData : public CViewDataListner
{
public:
	COffLineData(CViewData* pViewData);
	virtual ~COffLineData();

	// From CViewDataListner
public:
	virtual void OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId);

	virtual void OnDataRespMerchKLine(int iMmiReqId, IN CMmiRespMerchKLine *pMmiRespMerchKLine);

public:
	// ��ձ�������
	void		ResetData();

	// ȡ�������͵��������ʱ��. ��ʵ���� 000001 ��ʱ��.
	CGmtTime	GetLastTime(E_KLineTypeBase eType);

	// ������������
	void		DownLoadRealTimePriceData(const T_DownLoadOffLine& stDownLoadOffLine);
	
	// ���� K ������
	void		DownLoadKLineData(const T_DownLoadOffLine& stDownLoadOffLine);
	
	// ���� F10 ����
	void		DownLoadF10Data(const T_DownLoadOffLine& stDownLoadOffLine);
	
	// ��ʼ����
	void		StartDownLoad(const T_DownLoadOffLine& stDownLoadOffLine);

	// ֹͣ����
	void		StopDownLoad();

public:
	// ����֪ͨ������
	void		SetNotify(COffLineDataNotify* pNotify) { if ( NULL!= pNotify) m_pNotify = pNotify; }

private:
	// һ�ζε�ȡ��Ʒ����
	bool32		GetMerchsBySection(OUT vector<CMerch*>& aMerchs, bool32 bRealTime);		

	// �����������������
	void		SaveDownLoadRealTimePriceData(const CArray<CRealtimePrice*, CRealtimePrice*>& aRealtimePriceListPtr);

	// ��������� K ������
	void		SaveDownLoadKlineNodeData(int32 iReqID, const CMerchKLineNode& MerchKLineNode);

	// �����߳�
	bool32		BeginThreadDownLoad();
	
	void		StopThreadDownLoad();

	static unsigned int __stdcall CallBackThreadDownLoad(LPVOID lpParam);
	
	bool32		ThreadDownLoad();

	// д�ѻ����ݵ��߳�
	bool32		BeginThreadWriteOffLineData();

	void		StopThreadWriteOffLineData();

	static unsigned int __stdcall CallBackThreadWriteOffLineData(LPVOID lpParam);

	bool32		ThreadWriteOffLineData();

	// д��������
	bool32		WriteRealtimePriceData();

	// д K ������
	bool32		WriteKLineData();

	// д F10 ����
	bool32		WriteF10Data();

private:	
	// �����ı�־
	bool32				m_bStopWork;
	
	// �Ƿ���Կ�ʼ������һ��K��
	bool32				m_bTimeToNextSection;

	// viewdata 
	CViewData*			m_pViewData;

	// �ѷ������� ID ������	
	std::list<int32>	m_aRequestIDs;			
	LockSingle			m_LockRequestIDs;

	// ���صĲ����ṹ
	T_DownLoadOffLine	m_DownLoadOffLineParam;
	LockSingle			m_LockDownLoadOffLineParam;

	// ���ص��߳̾��
	HANDLE				m_hThreadDownLoad;

	// д�ļ����߳̾��
	HANDLE				m_hThreadWriteFile;

	// ���ػ�������������
	ListRealtimeData	m_aRealtimePriceToWrite;	
	LockSingle			m_LockRealtimePriceToWrite;

	// ���ػ�����K ������
	// bool32				m_bTimeToWrite;
	ListKLineData		m_aKLinesToWrite;
	LockSingle			m_LockKLinesToWrite;

private:
	COffLineDataNotify* m_pNotify;
};

#endif // !defined(_OFFLINEDATA_H_)
