#ifndef _VIEWDATALISTENER_H_
#define _VIEWDATALISTENER_H_

#include "sharestruct.h"
#include "sharestructnews.h"
#include "CenterManagerStruct.h"
#include "proxy_auth_client_base.h"
using namespace auth;
#include "EngineCenterExport.h"
#include "MerchDataManager.h"



////////////////////////////////////////////////////////////////////////////////////////////////////
class ENGINECENTER_DLL_EXPORT  CIoViewListner
{
public:
	//行情更新
	virtual void	OnVDataMarketSnapshotUpdate(int32 iMarketId) = 0;
	virtual void	OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType) = 0;
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch) = 0;
	virtual void	OnVDataRealtimeLevel2Update(IN CMerch *pMerch) = 0;
	virtual void	OnVDataRealtimeTickUpdate(IN CMerch *pMerch) = 0;
	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch) = 0;
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch) = 0;
	virtual void	OnVDataRelativeMerchsUpdate() = 0;	
	virtual void	OnVDataGeneralNormalUpdate(CMerch* pMerch) = 0;
	virtual void	OnVDataGeneralFinanaceUpdate(CMerch* pMerch) = 0;
	virtual void	OnVDataMerchTrendIndexUpdate(CMerch* pMerch) = 0;
	virtual void	OnVDataMerchAuctionUpdate(CMerch* pMerch) = 0;
	virtual void	OnVDataMerchMinuteBSUpdate(CMerch* pMerch) = 0;
	virtual void	OnVDataClientTradeTimeUpdate() = 0;
	virtual void	OnVDataReportInBlockUpdate(int32 iBlockId, E_MerchReportField eMerchReportField, bool32 bDescSort,int32 iPosStart, 
												int32 iOrgMerchCount, const CArray<CMerch *, CMerch *> &aMerchs) = 0;
	virtual void	OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, 
										int32 iOrgMerchCount, const CArray<CMerch*, CMerch*> &aMerchs) = 0;

	virtual void	OnVDataEconoData(const char *pszRecvData) = 0;

	virtual void	OnVDataStrategyData(const char *pszRecvData) = 0;

	virtual void	OnVDataQueryNewStockInfo(const char *pszRecvData) = 0;

	// 资讯更新
	virtual void	OnVDataNewsListUpdate() = 0;
	virtual void	OnVDataF10Update() = 0;
	virtual void	OnVDataLandMineUpdate(IN CMerch* pMerch) = 0;
	virtual void	OnVDataNewsPushTitle(const T_NewsInfo& stNewsInfo) = 0;
	virtual void	OnVDataNewsTitleUpdate(IN listNewsInfo& listTitles)= 0;
	virtual void	OnVDataNewsContentUpdate(const T_NewsInfo& stNewsInfo) = 0;
	virtual void	OnVDataCodeIndexUpdate(IN const CString& StrCode) = 0;


	virtual	DWORD	GetNeedPushDataType() = 0;		// 返回是否需要对应类型的推送，什么都是允许的, 某些视图可能不是需要推送数据的

	
	virtual bool32	IsAttendData(IN CMerch *pMerch, E_DataServiceType eDataServiceType) = 0;
	virtual bool32	IsAttendNews(IN CMerch* pMerch) = 0;
	virtual bool32	IsNowCanReqData() = 0;

	virtual void	OnVDataForceUpdate() = 0;
	virtual void    ForceUpdateVisibleIoView() = 0;
	virtual void	OnWeightTypeChange() = 0;

	virtual void	GetSmartAttendArray(OUT SmartAttendMerchArray &aAttends) = 0; 
	virtual void	GetAttendMarketDataArray(OUT AttendMarketDataArray &aAttends) = 0;

    virtual	void	OnVDataPluginResp(const  CMmiCommBase *pResp) = 0;
    virtual bool32	IsPluginDataView() = 0;
    virtual bool32  IsKindOfReportSelect() = 0;

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

// 视图数据不光是业务试图关心, 其他对话框和模块可能也需要
// ViewData 收到的 K 线数据都发给收听者, 收听者自己接受过滤, 收到所有自己需要的以后取消收听即可

// 暂时不支持同时收听多种类型数据, 待扩展

class ENGINECENTER_DLL_EXPORT  CViewDataListner
{
public:
	
	virtual void OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId) {}			// 行情
	virtual void OnDataRespMerchKLine(int iMmiReqId, IN CMmiRespMerchKLine *pMmiRespMerchKLine) {}						// k 线
	virtual void OnDataRespMerchTimeSales(int iMmiReqId, IN const CMmiRespMerchTimeSales *pMmiRespMerchTimeSales) {}	// 历史分笔
	virtual void OnRealtimeTick(const CMmiPushTick &MmiPushTick, int32 iCommunicationId){}								// 实时分笔

	virtual bool32 IsAttendCommType( E_CommType eType ) { if( ECTCount == m_eListenType)return true; return m_eListenType == eType; }

public:
	E_CommType m_eListenType;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
class ENGINECENTER_DLL_EXPORT  CViewNewsListner
{
public:
	virtual void   OnDataRespNewsData(int iMmiReqId, IN const CMmiNewsBase *pMmiNewResp, CMerch *pMerch){}

	virtual bool32 IsAttendNews(E_NewsType eType) { return m_eNewsType == eType; }		// 具体类型m_eNewsType

public:
	E_NewsType m_eNewsType;

};

#endif _VIEWDATALISTENER_H_