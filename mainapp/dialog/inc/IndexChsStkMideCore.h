// IndexChsStkMideCore.h: interface for the CIndexChsStkMideCore class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INDEXCHSSTKMIDECORE_H__)
#define AFX_INDEXCHSSTKMIDECORE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4786)

#include <vector>
#include <list>
#include <afxtempl.h>
#include "IoViewShare.h"
#include "synch.h"

class CKLine;
class CMerch;
class CViewData;
class CMmiRespMerchKLine;
class CDlgIndexChooseStock;
struct T_SaneIndexNode;


// ����
typedef struct T_IndexChsStkCondition
{
	bool32					m_bNeedCalc;	// �Ƿ�Ҫ����
	UINT					m_uiParam;		// ����ֵ

}T_IndexChsStkCondition;

// �������
typedef struct T_IndexChsStkCalcParam
{
	bool32					m_bFuture;		// �Ƿ��ڻ�
	CMerch*					m_pMerch;		// ��Ʒ
	int32					m_iRespID;		// ��Ӧ������ ID
	CArray<CKLine, CKLine>	m_aKLines;		// ���ڼ����K ������(�ѳ�Ȩ)	

public:
	T_IndexChsStkCalcParam()
	{
		m_bFuture	= false;
		m_pMerch	= NULL;
		m_iRespID	= -1;
		m_aKLines.RemoveAll();
	}

	//
	T_IndexChsStkCalcParam(const T_IndexChsStkCalcParam& stIndexChsStkCalcParam)
	{		
		m_bFuture	= stIndexChsStkCalcParam.m_bFuture;
		m_pMerch	= stIndexChsStkCalcParam.m_pMerch;
		m_iRespID	= stIndexChsStkCalcParam.m_iRespID;
		m_aKLines.Copy(stIndexChsStkCalcParam.m_aKLines);		
	}

	T_IndexChsStkCalcParam& operator=(const T_IndexChsStkCalcParam& stIndexChsStkCalcParam)
	{
		if ( &stIndexChsStkCalcParam == this )
		{
			return *this;
		}

		m_bFuture	= stIndexChsStkCalcParam.m_bFuture;
		m_pMerch	= stIndexChsStkCalcParam.m_pMerch;
		m_iRespID	= stIndexChsStkCalcParam.m_iRespID;
		m_aKLines.Copy(stIndexChsStkCalcParam.m_aKLines);

		return *this;
	}

}T_IndexChsStkCalcParam;

// ѡ�ɽ��
typedef struct T_IndexChsStkResult
{
	CMerch*  m_pMerch;				// ��Ʒ
	float	 m_fAccuracyRate;		// ׼ȷ��
	float	 m_fProfitability;		// ������

	bool32 operator==(const T_IndexChsStkResult& stIndexChsStkResult) const 
	{
		if ( m_pMerch == stIndexChsStkResult.m_pMerch && m_fAccuracyRate == stIndexChsStkResult.m_fAccuracyRate && m_fProfitability == stIndexChsStkResult.m_fAccuracyRate )
		{
			return true;
		}

		return false;
	}

}T_IndexChsStkResult;

class CIndexChsStkMideCore  
{
public:
	enum E_OfflineDataGetType		// �������ݷ�ʽ
	{
		EODGTAll	= 0,			// ȫ������
		EODGTAdd,					// ��������
		EODGTNone,					// ������

		EODGTCount,
	};

	CIndexChsStkMideCore();
	virtual ~CIndexChsStkMideCore();
//
public:	
	bool32			GetViewData();																		// �õ�ViewData
	void			SetParentDlg(CDlgIndexChooseStock* pDlgShow);										// ���ø�����

	bool32			RequestData(CMerch* pMerch, bool32 bReqAll);										// K ������
	void			RequestWeightData();																// ��Ȩ����

	bool32			BeginChooseStock();																	// ��ʼѡ��
	void			StopChooseStock(bool32 bErr = false);												// �û��ж�ѡ��
	void			OnChooseStockFinish();																// ѡ�����,���³�ʼ������
	
	void			OnRecvKLineData(int32 iMarketid, CString StrMerchCode, bool32 bFinished = false);	// �յ� K ������:
	// �Ƿ����ѻ���������
	bool32			BeIndexChsStkReqID(int32 iRespID);

	// �Ƿ���������
	bool32			BeMeetCondition(int32 iCondition, UINT uiParam, IN const CArray<T_SaneIndexNode, T_SaneIndexNode>& aSaneNodes);	

	// ��������,���˽��
	bool32			FiltResultWithConditions(IN const CArray<T_SaneIndexNode, T_SaneIndexNode>& aSaneNodes); 
	
	// ����ѡ�ɲ���
	void			SetChsStkParams(OUT int32& iMerchNums, IN E_OfflineDataGetType eOffLineDataGetType, IN const std::vector<CString>& aBlocks, IN const std::vector<T_IndexChsStkCondition>& aConditions, IN E_NodeTimeInterval eTimeInterval, IN UINT uiUserCycle = 0);	
	
	// �������������K ������
	bool32			ProcessRespKLine(const T_IndexChsStkRespData& stIndexChsStkRespData);	
	
	// �ϲ� K ��
	bool32			CombinKLine(IN CMerch* pMerch, IN const CArray<CKLine, CKLine>& aKLineSrc, OUT CArray<CKLine, CKLine>& aKlineDst);

	// ��ʱ�Ĵ���
	void			OnRecvTimeOut();

	// �Ƿ�����
	bool32			BePassedCloseTime(CMerch* pMerch);

	// �������ݵ��̺߳���
	bool32			BeginReqThread();
	static DWORD	WINAPI ThreadRequestData(LPVOID pParam);
	bool32			ThreadRequestData();

	// �������ݵ��̺߳���
	bool32			BeginRecvThread();
	static DWORD	WINAPI ThreadRecvData(LPVOID pParam);
	void			ThreadRecvData();

	// ����ָ����̺߳���:
	bool32			BeginCalcThread();
	static DWORD	WINAPI ThreadCalcIndexValue(LPVOID pParam);
	void			ThreadCalcIndexValue();

private:
	CDlgIndexChooseStock*			m_pDlgShow;				// ������ʾ����ĶԻ���
	E_OfflineDataGetType			m_eOffLineDataGetType;	// �������ݵķ�ʽ	
	UINT							m_uiUserCycle;			// �Զ���ķ���/����

	//
	bool32							m_bThreadReqExit;		// �����߳��˳��ı�־
	HANDLE							m_hThreadReq;			// �����̵߳ľ��
	
	//
	bool32							m_bThreadRecvExit;		// �����߳��˳��ı�־
	HANDLE							m_hThreadRecv;			// �����̵߳ľ��

	//
	bool32							m_bThreadCalcExit;		// �����߳��˳��ı�־
	HANDLE							m_hThreadCalc;			// ������߳�

	//
	LockSingle						m_LockCalcParm;			// �����������
	LockSingle						m_LockRequestIDs;		// �����ID ����

	//
	int32							m_iErrTimesRecv;		// �������ݳ���Ĵ���
	//
	CAbsCenterManager*						m_pAbsCenterManager;			// ViewData
	E_NodeTimeInterval				m_eTimeInterval;		// ѡ������

	std::vector<CMerch*>			m_aMerchsToChoose;		// ѡ�ɵ���Ʒ
	std::vector<T_IndexChsStkCondition>	m_aConditions;		// ѡ�ɵ�����
		
	std::list<int32>				m_aRequestIDs;			// �ѷ������� ID ������	
	CArray<T_IndexChsStkCalcParam, T_IndexChsStkCalcParam&> m_aCalcParams;	// ����Ĳ���

	std::vector<T_IndexChsStkResult> m_aChsStkResults;		// ѡ�ɽ��
};

#endif // !defined(AFX_INDEXCHSSTKMIDECORE_H__)
