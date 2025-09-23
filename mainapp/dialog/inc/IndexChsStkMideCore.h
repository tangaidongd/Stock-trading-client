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


// 条件
typedef struct T_IndexChsStkCondition
{
	bool32					m_bNeedCalc;	// 是否要计算
	UINT					m_uiParam;		// 参数值

}T_IndexChsStkCondition;

// 计算参数
typedef struct T_IndexChsStkCalcParam
{
	bool32					m_bFuture;		// 是否期货
	CMerch*					m_pMerch;		// 商品
	int32					m_iRespID;		// 对应的请求 ID
	CArray<CKLine, CKLine>	m_aKLines;		// 用于计算的K 线序列(已除权)	

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

// 选股结果
typedef struct T_IndexChsStkResult
{
	CMerch*  m_pMerch;				// 商品
	float	 m_fAccuracyRate;		// 准确率
	float	 m_fProfitability;		// 收益率

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
	enum E_OfflineDataGetType		// 下载数据方式
	{
		EODGTAll	= 0,			// 全部下载
		EODGTAdd,					// 增量下载
		EODGTNone,					// 不下载

		EODGTCount,
	};

	CIndexChsStkMideCore();
	virtual ~CIndexChsStkMideCore();
//
public:	
	bool32			GetViewData();																		// 得到ViewData
	void			SetParentDlg(CDlgIndexChooseStock* pDlgShow);										// 设置父窗口

	bool32			RequestData(CMerch* pMerch, bool32 bReqAll);										// K 线请求
	void			RequestWeightData();																// 除权请求

	bool32			BeginChooseStock();																	// 开始选股
	void			StopChooseStock(bool32 bErr = false);												// 用户中断选股
	void			OnChooseStockFinish();																// 选股完成,重新初始化环境
	
	void			OnRecvKLineData(int32 iMarketid, CString StrMerchCode, bool32 bFinished = false);	// 收到 K 线数据:
	// 是否是脱机发的请求：
	bool32			BeIndexChsStkReqID(int32 iRespID);

	// 是否满足条件
	bool32			BeMeetCondition(int32 iCondition, UINT uiParam, IN const CArray<T_SaneIndexNode, T_SaneIndexNode>& aSaneNodes);	

	// 根据条件,过滤结果
	bool32			FiltResultWithConditions(IN const CArray<T_SaneIndexNode, T_SaneIndexNode>& aSaneNodes); 
	
	// 设置选股参数
	void			SetChsStkParams(OUT int32& iMerchNums, IN E_OfflineDataGetType eOffLineDataGetType, IN const std::vector<CString>& aBlocks, IN const std::vector<T_IndexChsStkCondition>& aConditions, IN E_NodeTimeInterval eTimeInterval, IN UINT uiUserCycle = 0);	
	
	// 处理请求回来的K 线数据
	bool32			ProcessRespKLine(const T_IndexChsStkRespData& stIndexChsStkRespData);	
	
	// 合并 K 线
	bool32			CombinKLine(IN CMerch* pMerch, IN const CArray<CKLine, CKLine>& aKLineSrc, OUT CArray<CKLine, CKLine>& aKlineDst);

	// 超时的处理
	void			OnRecvTimeOut();

	// 是否收盘
	bool32			BePassedCloseTime(CMerch* pMerch);

	// 请求数据的线程函数
	bool32			BeginReqThread();
	static DWORD	WINAPI ThreadRequestData(LPVOID pParam);
	bool32			ThreadRequestData();

	// 接收数据的线程函数
	bool32			BeginRecvThread();
	static DWORD	WINAPI ThreadRecvData(LPVOID pParam);
	void			ThreadRecvData();

	// 计算指标的线程函数:
	bool32			BeginCalcThread();
	static DWORD	WINAPI ThreadCalcIndexValue(LPVOID pParam);
	void			ThreadCalcIndexValue();

private:
	CDlgIndexChooseStock*			m_pDlgShow;				// 用于显示结果的对话框
	E_OfflineDataGetType			m_eOffLineDataGetType;	// 请求数据的方式	
	UINT							m_uiUserCycle;			// 自定义的分钟/日线

	//
	bool32							m_bThreadReqExit;		// 请求线程退出的标志
	HANDLE							m_hThreadReq;			// 请求线程的句柄
	
	//
	bool32							m_bThreadRecvExit;		// 接收线程退出的标志
	HANDLE							m_hThreadRecv;			// 接收线程的句柄

	//
	bool32							m_bThreadCalcExit;		// 计算线程退出的标志
	HANDLE							m_hThreadCalc;			// 计算的线程

	//
	LockSingle						m_LockCalcParm;			// 参数数组的锁
	LockSingle						m_LockRequestIDs;		// 请求号ID 的锁

	//
	int32							m_iErrTimesRecv;		// 接收数据出错的次数
	//
	CAbsCenterManager*						m_pAbsCenterManager;			// ViewData
	E_NodeTimeInterval				m_eTimeInterval;		// 选股周期

	std::vector<CMerch*>			m_aMerchsToChoose;		// 选股的商品
	std::vector<T_IndexChsStkCondition>	m_aConditions;		// 选股的条件
		
	std::list<int32>				m_aRequestIDs;			// 已发的请求 ID 号数组	
	CArray<T_IndexChsStkCalcParam, T_IndexChsStkCalcParam&> m_aCalcParams;	// 计算的参数

	std::vector<T_IndexChsStkResult> m_aChsStkResults;		// 选股结果
};

#endif // !defined(AFX_INDEXCHSSTKMIDECORE_H__)
