#ifndef _DATA_ENGINE_MANAGER_H_
#define _DATA_ENGINE_MANAGER_H_
#include "DataEngineStruct.h"


////////////////////////////////////////////////////////////////////////////////////
// 所有的数据通知
class  CDataManagerNotify
{
public:
	/**********************************************************************
	* 函数名称:	OnDataServiceConnectStatus
	* 功能描述:	服务连接状态通知
	* 输入参数:	iServiceId			服务ID
	* 输入参数:	eConnectStatus		连接状态EG_CNT_STATUS_CONNECT（连接）、EG_CNT_STATUS_DIS_CONNECT（断开）
	***********************************************************************/
	virtual void	OnDataServiceConnectStatus(int32 iServiceId, E_Connect_Status eConnectStatus) = 0;

	/**********************************************************************
	* 函数名称:	OnDataRequestTimeOut
	* 功能描述:	数据请求超时通知
	* 输入参数:	iMmiReqId		请求命令ID
	***********************************************************************/
	virtual void	OnDataRequestTimeOut(int32 iMmiReqId) = 0;			//处理函数没处理

	/**********************************************************************
	* 函数名称:	OnDataCommResponse
	* 功能描述:	数据请求回复通知
	***********************************************************************/
	virtual void	OnDataCommResponse() = 0;

	/**********************************************************************
	* 函数名称:	OnDataServerConnectStatus
	* 功能描述:	服务器连接状态通知
	* 输入参数:	iCommunicationId	连接服务器通讯ID
	* 输入参数:	eConnectStatus		连接状态EG_CNT_STATUS_CONNECT（连接）、EG_CNT_STATUS_DIS_CONNECT（断开）
	***********************************************************************/
	virtual void	OnDataServerConnectStatus(int32 iCommunicationId, E_Connect_Status eConnectStatus) = 0;

	/**********************************************************************
	* 函数名称:	OnDataSyncServerTime
	* 功能描述:	同步服务器时间到本地
	* 输入参数:	TimeServerNow		服务器时间
	***********************************************************************/
	virtual void	OnDataSyncServerTime(CGmtTime &TimeServerNow) = 0;

	/**********************************************************************
	* 函数名称:	OnDataServerLongTimeNoRecvData
	* 功能描述:	服务器长时间没有数据回复
	* 输入参数:	iCommunicationId		服务器通讯ID
	* 输入参数:	eType					异常连接类型
	***********************************************************************/
	virtual void	OnDataServerLongTimeNoRecvData(int32 iCommunicationId, E_ReConnectType eType) = 0;
};


// 所有数据管理基类， 描述了对外提供的功能接口
class  CAbsDataManager
{

public:

	/**********************************************************************
	* 函数名称:	Release
	* 功能描述:	释放内存空间
	* 输入参数:	
	***********************************************************************/
	virtual void Release() = 0;
	
	/**********************************************************************
	* 函数名称:	RequestData
	* 功能描述:	请求服务器数据
	* 输入参数:	pMmiCommReq			请数据类型
	* 输入参数	aMmiReqNodes		请求节点队列	
	* 输入参数	eDataSource			请求数据源类型
	* 输入参数	iCommunicationId	通讯ID
	***********************************************************************/
	virtual int32	RequestData(CMmiCommBase *pMmiCommReq, OUT CArray<CMmiReqNode*, CMmiReqNode*> &aMmiReqNodes, 
									E_DataSource eDataSource = EDSAuto, int32 iCommunicationId = -1) = 0;

	/**********************************************************************
	* 函数名称:	GetRespCount
	* 功能描述:	服务器应答队列数目
	* 输入参数:
	***********************************************************************/
	virtual int32	GetRespCount() = 0;

	/**********************************************************************
	* 函数名称:	PeekAndRemoveFirstResp
	* 功能描述:	取应答队列数据
	* 输入参数:	
	***********************************************************************/
	virtual CMmiRespNode*	PeekAndRemoveFirstResp() = 0;

	/**********************************************************************
	* 函数名称:	AddDataManagerNotify
	* 功能描述:	注册管理通知类
	* 输入参数:	pDataManagerNotify		通知回调类指针
	***********************************************************************/
	virtual void	AddDataManagerNotify(CDataManagerNotify *pDataManagerNotify) = 0;

	/**********************************************************************
	* 函数名称:	RemoveDataManagerNotify
	* 功能描述:	移除注册管理通知类
	* 输入参数:	pDataManagerNotify		移除通知回调类指针
	***********************************************************************/
	virtual void	RemoveDataManagerNotify(CDataManagerNotify *pDataManagerNotify) = 0;

	/**********************************************************************
	* 函数名称:	RemoveDataManagerNotify
	* 功能描述:	断线清除历史请求记录
	* 输入参数:	iCommunicationId	服务器通讯号
	***********************************************************************/
	virtual void	RemoveHistoryReqOfSpecifyServer(int32 iCommunicationId) = 0;

	/**********************************************************************
	* 函数名称:	ForceRequestType
	* 功能描述:	强制改变请求方式
	* 输入参数:	eDataSource	请求数据类型
	***********************************************************************/
	virtual void	ForceRequestType(E_DataSource eDataSource) = 0;

	/**********************************************************************
	* 函数名称:	GetOfflineKLinesSnapshot
	* 功能描述:	获取离线K线快照
	* 输入参数:	iMarketId		市场ID
	* 输入参数	StrMerchCode	商品代码
	* 输入参数	eKLineTypeBase	K线基本类型
	* 输入参数	eOffKLineType	离线数据类型
	* 输出参数	TimeStart		起始时间
	* 输出参数	TimeEnd			结束时间
	* 输出参数	iKLineCount		获取的K线总数
	***********************************************************************/
	virtual bool32	GetOfflineKLinesSnapshot(int32 iMarketId, const CString &StrMerchCode, E_KLineTypeBase eKLineTypeBase, 
		E_OffKLineType eOffKLineType, OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd, OUT int32 &iKLineCount) = 0;
	
	/**********************************************************************
	* 函数名称:	GetOfflineKLinesSnapshot
	* 功能描述:	写入离线K线数据
	* 输入参数:	iMarketId		市场ID
	* 输入参数	StrMerchCode	商品代码
	* 输入参数	eKLineTypeBase	K线基本类型
	* 输入参数	eOffKLineType	离线K线数据类型
	* 输入参数	aKLines			K线数据缓存
	***********************************************************************/
	virtual void	WriteOfflineKLines(int32 iMarketId, const CString &StrMerchCode, E_KLineTypeBase eKLineTypeBase, 
		E_OffKLineType eOffKLineType, IN const CArray<CKLine, CKLine> &aKLines) = 0;
	
	/**********************************************************************
	* 函数名称:	GetOfflineKLinesSnapshot
	* 功能描述:	读取离线K线数据
	* 输入参数:	iMarketId		市场ID
	* 输入参数	StrMerchCode	商品代码
	* 输入参数	eKLineTypeBase	K线基本类型
	* 输入参数	eOffKLineType	离线K线数据类型
	* 输出参数	aKLines			K线数据缓存区
	***********************************************************************/
	virtual void	ReadOfflineKLines(int32 iMarketId, const CString &StrMerchCode, E_KLineTypeBase eKLineTypeBase, 
		E_OffKLineType eOffKLineType, OUT CArray<CKLine, CKLine> &aKLines) = 0;

	/**********************************************************************
	* 函数名称:	GetOfflineKLinesSnapshot
	* 功能描述:	读取离线K线数据
	* 输入参数:	iMarketId		市场ID
	* 输入参数	StrMerchCode	商品代码
	* 输入参数	eKLineTypeBase	K线基本类型
	* 输入参数	eOffKLineType	离线K线数据类型
	* 输入参数	TimeStart		开始时间
	* 输入参数	TimeEnd			结束时间
	* 输出参数	aKLines			K线数据缓存区
	***********************************************************************/
	virtual void	ReadOfflineKLines(int32 iMarketId, const CString &StrMerchCode, E_KLineTypeBase eKLineTypeBase, 
		E_OffKLineType eOffKLineType, const CGmtTime &TimeStart, const CGmtTime &TimeEnd, OUT CArray<CKLine, CKLine> &aKLines) = 0;

	/**********************************************************************
	* 函数名称:	GetOfflineKLinesSnapshot
	* 功能描述:	读取离线K线数据
	* 输入参数:	iMarketId		市场ID
	* 输入参数	StrMerchCode	商品代码
	* 输入参数	eKLineTypeBase	K线基本类型
	* 输入参数	eOffKLineType	离线K线数据类型
	* 输入参数	TimeSpecify		特殊时间点
	* 输入参数	iCount			K线数目
	* 输出参数	aKLines			K线数据缓存区
	***********************************************************************/
	virtual void	ReadOfflineKLines(int32 iMarketId, const CString &StrMerchCode, E_KLineTypeBase eKLineTypeBase,
		E_OffKLineType eOffKLineType, const CGmtTime &TimeSpecify, int32 iCount, OUT CArray<CKLine, CKLine> &aKLines) = 0;
	
	/**********************************************************************
	* 函数名称:	RemoveOfflineKLines
	* 功能描述:	移除相关离线K线数据
	* 输入参数:	iMarketId		市场ID
	* 输入参数	StrMerchCode	商品代码
	* 输入参数	eKLineTypeBase	K线基本类型
	* 输入参数	eOffKLineType	离线K线数据类型
	***********************************************************************/
	virtual void	RemoveOfflineKLines(int32 iMarketId, const CString &StrMerchCode, E_KLineTypeBase eKLineTypeBase, 
		E_OffKLineType eOffKLineType) = 0;

	/**********************************************************************
	* 函数名称:	WriteOfflineRealtimePrice
	* 功能描述:	写入离线行情交易数据
	* 输入参数:	stRealtimePrice		行情交易数据
	***********************************************************************/
	virtual	void	WriteOfflineRealtimePrice(const CRealtimePrice& stRealtimePrice) = 0;

	/**********************************************************************
	* 函数名称:	ReadOfflineRealtimePrice
	* 功能描述:	读取离线真实价格
	* 输入参数:	iMarketId		市场ID
	* 输入参数	StrMerchCode	商品代码
	* 输出参数	RealtimePrice	行情交易数据
	***********************************************************************/
	virtual void	ReadOfflineRealtimePrice(int32 iMarkerId, const CString& StrMerchCode, OUT CRealtimePrice& RealtimePrice) = 0;

	/**********************************************************************
	* 函数名称:	GetPublishFilePath
	* 功能描述:	获取公共文件路径
	* 输入参数:	ePublicFileType		文件类型
	* 输入参数	iMarketId			商品代码
	* 输入参数	StrMerchCode		商品代码
	* 输出参数	StrFilePath			文件路径
	***********************************************************************/
	virtual bool32	GetPublishFilePath(E_PublicFileType ePublicFileType, int32 iMarketId, const CString &StrMerchCode, 
		OUT CString &StrFilePath) = 0;	

	/**********************************************************************
	* 函数名称:	InitAllService
	* 功能描述:	初始化服务
	* 输入参数:	aServerInfo			服务器列表
	* 输入参数	ProxyInfo			代理服务器信息
	* 输入参数	StrUserName			用户名称
	* 输入参数	StrPassword			用户密码
	***********************************************************************/
	virtual void	InitAllService( IN CArray<T_ServerInfo, T_ServerInfo&> &aServerInfo, IN const CProxyInfo &ProxyInfo, 
		IN const CString &StrUserName, IN const CString &StrPassword) = 0;

	/**********************************************************************
	* 函数名称:	InitAllService
	* 功能描述:	启动所有服务
	* 输入参数:	bOnlyStartFirst		是否启动服务的第一台服务器，默认启动所有
	***********************************************************************/
	virtual void	StartAllService(bool32 bOnlyStartFirst = false) = 0;

	/**********************************************************************
	* 函数名称:	InitAllService
	* 功能描述:	停止所有服务
	***********************************************************************/
	virtual void	StopAllService() = 0; 

	/**********************************************************************
	* 函数名称:	StartService
	* 功能描述:	启动指定业务的通讯服务器
	* 输入参数:	iCommunication		通讯服务器ID
	***********************************************************************/
	virtual	void	StartService(int32 iCommunication) = 0;

	/**********************************************************************
	* 函数名称:	StartService
	* 功能描述:	停止指定业务的通讯服务器
	* 输入参数:	iCommunication		通讯服务器ID
	***********************************************************************/
	virtual void	StopService(int32 iCommunication) = 0;

	/**********************************************************************
	* 函数名称:	GetActiveCommunicationByService
	* 功能描述:	获取当前连接的服务器列表
	* 输入参数:	iServiceId				服务ID
	* 输出参数:	aCommunicationIds		服务器通讯ID
	***********************************************************************/
	virtual void	GetActiveCommunicationByService(IN int32  iServiceId, OUT CArray<int32, int32> &aCommunicationIds) = 0;


	/**********************************************************************
	* 函数名称:	GetCommunicationID
	* 功能描述:	获取当服务器通讯ID
	* 输入参数:	StrAddress	服务ID
	* 输入参数:	iPort		服务器通讯ID
	* 返回值:				服务器通讯ID
	***********************************************************************/
	virtual int32	GetCommunicationID(IN const CString& StrAddress, IN const uint32 iPort) = 0;

	// 额外添加后期验证是否需要
	///////////////////////////////////////////////////////////////
	/**********************************************************************
	* 函数名称:	ReSetCommunicationIndex
	* 功能描述:	通过服务器排序ID重新排序
	* 输出参数:	aServersAccording	服务器信息缓存
	***********************************************************************/
	virtual void	ReSetCommunicationIndex(IN const CArray<CServerState, CServerState&>& aServersAccording) = 0;

	/**********************************************************************
	* 函数名称:	BeFirstCommunication
	* 功能描述:	是否为首个服务器
	* 返回值:	true：是，false：否
	***********************************************************************/
	virtual bool32	BeFirstCommunication(IN const CString& StrAddress, IN const int32 iPort) = 0;

	/**********************************************************************
	* 函数名称:	GetCommnunicaionList
	* 功能描述:	获取通讯服务器信息列表
	* 输出参数:	aCommunicationList	服务器信息列表缓存
	***********************************************************************/
	virtual void GetCommnunicaionList(OUT CArray<CAbsCommunicationShow*, CAbsCommunicationShow*> &aCommunicationList) = 0;

	/**********************************************************************
	* 函数名称:	GetServerServiceList
	* 功能描述:	获取指定服务器的服务列表
	* 输入参数:	StrAddress	服务器地址
	* 输入参数:	iPort		服务器端口
	* 输入参数:	aServiceIDs	服务器服务ID列表
	***********************************************************************/
	virtual void GetServerServiceList(IN const CString& StrAddress, IN const uint32 iPort, OUT CArray<int32,int32>& aServiceIDs) = 0;

	/**********************************************************************
	* 函数名称:	GetStage
	* 功能描述:	获取当前通讯场景信息
	* 返回值:	E_COMMSTAGE	0:处在登录初始化场景中 1:行情提供中
	***********************************************************************/
	virtual DWORD GetStage() = 0;		// 

	/**********************************************************************
	* 函数名称:	SetStage
	* 功能描述:	设置当前通讯场景信息
	* 输入参数:	eStage	ECSTAGE_LoginInit:处在登录初始化场景中 ECSTAGE_QuoteSupplier:行情提供中
	***********************************************************************/
	virtual void  SetStage(E_COMMSTAGE eStage) = 0; // 

	/**********************************************************************
	* 函数名称:	IsServerConneted
	* 功能描述:	 当前服务器是否已连接
	* 输入参数:	true：已连接 false：未连接
	***********************************************************************/
	virtual bool32	IsServerConneted(IN const CString& StrAddress, IN const int32 iPort) = 0;

	/**********************************************************************
	* 函数名称:	StopServiceAfterSortServer
	* 功能描述:	ping 完以后停止其他服务器的服务
	***********************************************************************/
	virtual void StopServiceAfterSortServer() = 0;
};


AFX_API_IMPORT CAbsDataManager * GetDataEngineObj();


#endif