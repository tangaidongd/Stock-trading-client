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

// 下载的参数保存结构
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
	E_CommType		m_eReqType;		// 类型
	vector<CMerch*> m_aMerchs;		// 商品
	CGmtTime		m_TimeBeing;	// 起始时间
	CGmtTime		m_TimeEnd;		// 终止时间
	E_KLineTypeBase m_eKLineBase;	// K 线类型

}T_DownLoadOffLine;

// 请求回来的k 线数据
typedef struct T_OffLineRespKLineData
{
public:
	int32					m_iRespID;			// 回来的ID 号
	int32					m_iMarketId;		// 市场编号
	CString					m_StrMerchCode;		// 商品代码
	E_KLineTypeBase			m_eKLineTypeBase;	// 请求的K线类型
	CArray<CKLine, CKLine>	m_aKLines;			// K 线
	
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

// 请求回来的 F10 数据
typedef struct T_OffLineRespF10Data
{
	
}T_OffLineRespF10Data;

// 数据下载的事件通知
class DATACENTER_DLL_EXPORT COffLineDataNotify
{
public:
	// 请求都发完了
	virtual void OnAllRequestSended(E_CommType eType) = 0;

	// 正在下载
	virtual void OnDataDownLoading(CMerch* pMerch, E_CommType eType) = 0;

	// 下载成功
	virtual void OnDataDownLoadFinished(E_CommType eType) = 0;

	// 下载失败
	virtual void OnDataDownLoadFailed(const CString& StrErrMsg) = 0;
};

// 脱机数据: 总共有 K 线, 行情, F10 这几种数据. K 线和行情数据交给 Fileengine 去处理, F10自己读写
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
	// 清空本地数据
	void		ResetData();

	// 取各种类型的最后下载时间. 其实就是 000001 的时间.
	CGmtTime	GetLastTime(E_KLineTypeBase eType);

	// 下载行情数据
	void		DownLoadRealTimePriceData(const T_DownLoadOffLine& stDownLoadOffLine);
	
	// 下载 K 线数据
	void		DownLoadKLineData(const T_DownLoadOffLine& stDownLoadOffLine);
	
	// 下载 F10 数据
	void		DownLoadF10Data(const T_DownLoadOffLine& stDownLoadOffLine);
	
	// 开始下载
	void		StartDownLoad(const T_DownLoadOffLine& stDownLoadOffLine);

	// 停止下载
	void		StopDownLoad();

public:
	// 设置通知接收者
	void		SetNotify(COffLineDataNotify* pNotify) { if ( NULL!= pNotify) m_pNotify = pNotify; }

private:
	// 一段段的取商品处理
	bool32		GetMerchsBySection(OUT vector<CMerch*>& aMerchs, bool32 bRealTime);		

	// 保存回来的行情数据
	void		SaveDownLoadRealTimePriceData(const CArray<CRealtimePrice*, CRealtimePrice*>& aRealtimePriceListPtr);

	// 保存回来的 K 线数据
	void		SaveDownLoadKlineNodeData(int32 iReqID, const CMerchKLineNode& MerchKLineNode);

	// 下载线程
	bool32		BeginThreadDownLoad();
	
	void		StopThreadDownLoad();

	static unsigned int __stdcall CallBackThreadDownLoad(LPVOID lpParam);
	
	bool32		ThreadDownLoad();

	// 写脱机数据的线程
	bool32		BeginThreadWriteOffLineData();

	void		StopThreadWriteOffLineData();

	static unsigned int __stdcall CallBackThreadWriteOffLineData(LPVOID lpParam);

	bool32		ThreadWriteOffLineData();

	// 写行情数据
	bool32		WriteRealtimePriceData();

	// 写 K 线数据
	bool32		WriteKLineData();

	// 写 F10 数据
	bool32		WriteF10Data();

private:	
	// 工作的标志
	bool32				m_bStopWork;
	
	// 是否可以开始处理下一批K线
	bool32				m_bTimeToNextSection;

	// viewdata 
	CViewData*			m_pViewData;

	// 已发的请求 ID 号数组	
	std::list<int32>	m_aRequestIDs;			
	LockSingle			m_LockRequestIDs;

	// 下载的参数结构
	T_DownLoadOffLine	m_DownLoadOffLineParam;
	LockSingle			m_LockDownLoadOffLineParam;

	// 下载的线程句柄
	HANDLE				m_hThreadDownLoad;

	// 写文件的线程句柄
	HANDLE				m_hThreadWriteFile;

	// 下载回来的行情数据
	ListRealtimeData	m_aRealtimePriceToWrite;	
	LockSingle			m_LockRealtimePriceToWrite;

	// 下载回来的K 线数据
	// bool32				m_bTimeToWrite;
	ListKLineData		m_aKLinesToWrite;
	LockSingle			m_LockKLinesToWrite;

private:
	COffLineDataNotify* m_pNotify;
};

#endif // !defined(_OFFLINEDATA_H_)
