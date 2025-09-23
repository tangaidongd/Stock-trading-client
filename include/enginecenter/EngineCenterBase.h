#ifndef _ENGINE_CENTER_BASE_H_
#define _ENGINE_CENTER_BASE_H_

#pragma warning(disable: 4786)

#include "typedef.h"
#include "ViewDataListener.h"
#include "DataEngineManager.h"
#include "hotkey.h"
#include "ProxyAuthClientHelper.h"
#include "NewsEngineManager.h"
#include "OptimizeServer.h"
#include "CenterManagerStruct.h"




static int g_iCenterManagerVer = 8;		// DLL


/////////////////////////////////////////////////////////////
//			
class CIndexChsStkMideCore;
class CProxyInfo;

class ENGINECENTER_DLL_EXPORT CAbsCenterManager
{
public: 
	
	/**********************************************************************
		* 函数名称:	Construct
		* 功能描述:	构造内部数据
		* 输入参数:	NA
		* 输出参数:	NA
		* 返 回 值:	bool32
	***********************************************************************/
	virtual bool32			Construct() = 0;

	/**********************************************************************
		* 函数名称:	Release
		* 功能描述:	释放对象
		* 输入参数:	NA
		* 输出参数:	NA
		* 返 回 值:	void
	***********************************************************************/
	virtual void			Release() = 0;

	/**********************************************************************
	* 函数名称:	SetUserInfo
	* 功能描述:	设置用户信息
	* 输入参数: strUserName	用户名
				strPwd		密码
				strProduct  产品
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			SetUserInfo(CString& strUserName,CString& strPwd,CString& strProduct) = 0;

	/**********************************************************************
	* 函数名称:	InitialDataServers
	* 功能描述:	NA
	* 输入参数:	proxyInfo	代理服务器信息
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			InitialDataServers(CProxyInfo& proxyInfo) = 0;

	/**********************************************************************
	* 函数名称:	ConnectServer
	* 功能描述:	手动断开行情连接后， 再连接行情服务器
	* 输入参数: aServer行情服务器列表,已经优选排序好的
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			ConnectQuoteServer(E_NetType eNetType, COptimizeServerNotify *pNotify) = 0;
	
	/**********************************************************************
	* 函数名称:	DisconnectQuoteServer
	* 功能描述:	手动断开行情连接后
	* 输入参数: NA
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			DisconnectQuoteServer() = 0;

	/**********************************************************************
	* 函数名称:	InitPush
	* 功能描述:	市场推送，内存清理，服务时间，商品行情
	* 输入参数:	bEnable  是否初始化推送
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			InitPush(bool32 bEnable) = 0;

public:

	/**********************************************************************
	* 函数名称:	SetOwnerWnd
	* 功能描述:	设置消息通知的句柄
	* 输入参数:	hWnd窗口句柄
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			SetOwnerWnd(HWND hWndOwner) = 0;

	/**********************************************************************
	* 函数名称:	GetDataManager
	* 功能描述:	获取数据管理对象
	* 输出参数: NA
	* 返 回 值:	CAbsDataManager* 获取数据管理对象&
	***********************************************************************/
	virtual CAbsDataManager* GetDataManager() = 0;

	/**********************************************************************
	* 函数名称:	GetProxyAuthManager
	* 功能描述:	获取认证管理对象
	* 输出参数: NA
	* 返 回 值:	CProxyAuthClientHelper* 认证管理接口类实例对象&
	***********************************************************************/
	virtual CProxyAuthClientHelper* GetProxyAuthManager() = 0;

	/**********************************************************************
	* 函数名称:	GetNewsManager
	* 功能描述:	获取资讯数据管理对象
	* 输出参数: NA
	* 返 回 值:	CAbsNewsManager* 资讯管理接口类实例对象&
	***********************************************************************/
	virtual CAbsNewsManager* GetNewsManager() = 0;

	/**********************************************************************
	* 函数名称:	GetMerchManager
	* 功能描述:	获取商品管理对象
	* 输出参数: NA
	* 返 回 值:	CAbsMerchManager* 离线管理接口类实例对象
	***********************************************************************/
//	virtual CAbsMerchManager*  GetMerchManager() = 0;
	virtual CMerchManager&  GetMerchManager() = 0;

public:

	/**********************************************************************
	* 函数名称:	AddViewListener
	* 功能描述:	将指定视图添加到数据监听队列
	* 输入参数:	pIoView 需要监听的视图
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			AddViewListener(IN CIoViewListner *pIoView) = 0;

	/**********************************************************************
	* 函数名称:	RemoveViewListener
	* 功能描述:	将指定视图从数据监听队列移除
	* 输入参数:	pListener 需要移除的视图
	* 输出参数: NA
	* 返 回 值:	void	
	***********************************************************************/
	virtual void			RemoveViewListener(IN CIoViewListner *pIoView) = 0;

	/**********************************************************************
	* 函数名称:	RequestViewData
	* 功能描述:	请求行情相关数据
	* 输入参数:	pMmiCommBase  请求数据类型
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			RequestViewData(IN CMmiCommBase *pMmiCommBase, IN int32 iCommunicationID = -1,IN int eDataSource = EDSAuto) = 0;

	/**********************************************************************
	* 函数名称:	RequestNews
	* 功能描述:	请求资讯数据
	* 输入参数:	IpMmiNewsBase 请求数据类型
	* 输入参数:	IN int32 iCommunicationID:
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			RequestNews(IN CMmiNewsBase* pMmiNewsBase, IN int32 iCommunicationID = -1) = 0;

	/**********************************************************************
	* 函数名称:	GetServerTime
	* 功能描述:	返回当前服务器时间（格林威治时间）
	* 输入参数: NA
	* 输出参数: NA
	* 返 回 值:	CGmtTime
	***********************************************************************/
	virtual CGmtTime		GetServerTime() = 0; 

	/**********************************************************************
	* 函数名称:	GetMarketCurrentTime
	* 功能描述:	获取某个市场所在时区的的当前时间（相对于服务器的格林威治时间加上该市场的时差）
	* 输入参数:	Market	所要获取当前时间的市场
	* 输出参数: NA
	* 返 回 值:	CGmtTime
	***********************************************************************/
	virtual CGmtTime		GetMarketCurrentTime(const CMarket &Market) = 0;

	/**********************************************************************
	* 函数名称:	OnAuthSuccessMain
	* 功能描述:	市场初始化进度
	* 输入参数: NA
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			OnAuthSuccessMain() = 0;

	virtual void			OnMsgCodeIndexUpdate(IN const CString& StrCode) = 0;

public:
    /**********************************************************************
	* 函数名称:	OnMsgServiceConnected
	* 功能描述:	服务连接
	* 输入参数:    iServiceId  服务ID
	* 输出参数:    NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void		OnMsgServiceConnected(int32 iServiceId) = 0;

	/**********************************************************************
	* 函数名称:	OnMsgServiceDisconnected
	* 功能描述:	服务断开连接
	* 输入参数:    iServiceId  服务ID
	* 输出参数:    NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void	  OnMsgServiceDisconnected(int32 iServiceId) = 0;

	/**********************************************************************
	* 函数名称:	OnMsgRequestTimeOut
	* 功能描述:	请求超时
	* 输入参数:    iMmiReqId 请求ID
	* 输出参数:    NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void	OnMsgRequestTimeOut(int32 iMmiReqId) = 0;

	/**********************************************************************
	* 函数名称:	OnMsgCommResponse
	* 功能描述:	行情请求回应
	* 输入参数:    NA
	* 输出参数:    NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void    OnMsgCommResponse() = 0;

	/**********************************************************************
	* 函数名称:	OnMsgServerConnected
	* 功能描述:	服务器连接
	* 输入参数:    iCommunicationId 服务器ID
	* 输出参数:    NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void    OnMsgServerConnected(int32 iCommunicationId) = 0;

	/**********************************************************************
	* 函数名称:	OnMsgServerDisconnected
	* 功能描述:	服务器断开连接
	* 输入参数:    iCommunicationId 服务器ID
	* 输出参数:    NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void    OnMsgServerDisconnected(int32 iCommunicationId) = 0;

	/**********************************************************************
	* 函数名称:	OnMsgServerLongTimeNoRecvData
	* 功能描述:	长时间收不到回复数据  E_ReConnectType 连接类型
	* 输入参数:    iCommunicationId 服务器ID   
	* 输出参数:    NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void	OnMsgServerLongTimeNoRecvData(int32 iCommunicationId, E_ReConnectType eType) = 0;

	/**********************************************************************
	* 函数名称:	UploadUserBlock
	* 功能描述:	自选股上传
	* 输入参数:    NA   
	* 输出参数:    NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void    UploadUserBlock() = 0;	

	/**********************************************************************
	* 函数名称:	UpdateUserBlock
	* 功能描述:	自选股更新
	* 输入参数:    NA  
	* 输出参数:    NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void    UpdateUserBlock() = 0;

	/**********************************************************************
	* 函数名称:	SetRecordDataCenterAttendMerchs
	* 功能描述:	设置记录中心关注商品
	* 输入参数:    aMerchs 关注商品列表
	* 输出参数:    NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void    SetRecordDataCenterAttendMerchs(const CArray<CSmartAttendMerch, CSmartAttendMerch&>& aMerchs) = 0;

	/**********************************************************************
	* 函数名称:	OnWeightTypeChange
	* 功能描述:	权重类型改变
	* 输入参数:    NA  
	* 输出参数:    NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void	OnWeightTypeChange() = 0;

	/**********************************************************************
	* 函数名称:	GetClosePrice
	* 功能描述:	获取收盘价
	* 输入参数:    NA  
	* 输出参数:    NA
	* 返 回 值:	void
	***********************************************************************/
	virtual float    GetClosePrice() = 0;

	/**********************************************************************
	* 函数名称:	GetServiceDispose
	* 功能描述:	获取服务操作指针
	* 输入参数:    NA  
	* 输出参数:    NA
	* 返 回 值:	void
	***********************************************************************/
	virtual iServiceDispose  *GetServiceDispose() = 0;

	/**********************************************************************
	* 函数名称:	SetServiceDispose
	* 功能描述:	设置用户服务信息
	* 输入参数:    NA  
	* 输出参数:    NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void  SetServiceDispose(iServiceDispose *pServiceDispose) = 0;
	
	/**********************************************************************
	* 函数名称:	GetJinpingID
	* 功能描述:	获取今评消息ID
	* 输入参数:    NA  
	* 输出参数:    NA
	* 返 回 值:	void
	***********************************************************************/
	virtual int32			 GetJinpingID() = 0;

public:

	// 除权类型变化
//	virtual void			OnWeightTypeChange() = 0;

	// 板块文件超时
	virtual void			OnBlockFileTranslateTimeout() = 0;
	
	/**********************************************************************
	* 函数名称:	FindRelativeMerchs
	* 功能描述:	根据指定商品， 查找关联商品
	* 输入参数:	pMerch 指定商品指针
	* 输出参数: AllRelativeMerchs 关联商品数组
	* 返 回 值:	void
	***********************************************************************/
	virtual void			FindRelativeMerchs(IN CMerch *pMerch, OUT CArray<CRelativeMerchNode, CRelativeMerchNode&> &AllRelativeMerchs) = 0;


	// 快捷键相关
public:
	/**********************************************************************
	* 函数名称:	BuildMerchHotKeyList
	* 功能描述:	生成商品热键列表
	* 输入参数:	NA
	* 输出参数: arrHotKeys 生成的列表
	* 返 回 值:	void
	***********************************************************************/
	virtual void			BuildMerchHotKeyList(CArray<CHotKey, CHotKey&>&	arrHotKeys) = 0;

	// 条件选股相关
public:
	virtual void			SetIndexChsStkMideCore(CIndexChsStkMideCore* pMidCore) = 0;
	virtual CIndexChsStkMideCore *GetIndexChsStkMideCore() = 0;
	virtual void			SetIndexChsStkReqNums(int32 iNumReqs) = 0;
	virtual bool32			GetRespMerchKLineForIndexChsStk(OUT CArray<T_IndexChsStkRespData, T_IndexChsStkRespData&>& RecvDataForIndexChsStk) = 0;
	virtual bool			IsRevFinishByIndexChsStk() = 0;
	
	/**********************************************************************
	* 函数名称:	SetLoginHWnd
	* 功能描述:	设置登录框句柄
	* 输入参数:	hWnd	窗体句柄
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			SetLoginHWnd(HWND hWnd) = 0;

	/**********************************************************************

	* 函数名称:	SetFrameHwnd
	* 功能描述:	设置主窗体句柄
	* 输入参数:	hWnd	主窗体句柄
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			SetFrameHwnd(HWND hWnd) = 0;

	/**********************************************************************
	* 函数名称:	AddAttendMerch
	* 功能描述:	添加某类关注商品
	* 输出参数:	NA
	* 输入参数: attendMerch 关注的商品集合
	*			eType  这些商品属于的类型 
	*			ePushType这些商品需要的推送数据
	* 返 回 值:	void
	***********************************************************************/
	virtual void			AddAttendMerch(CSmartAttendMerch& attendMerch,E_AttendMerchType type ) = 0;

	/**********************************************************************
	* 函数名称:	GetAttendMerch
	* 功能描述:	获取某类关注商品
	* 输入参数:	attendMerch		关注的商品集合
				type 这些商品属于的类型 
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			GetAttendMerch(SmartAttendMerchArray& attendMerch,E_AttendMerchType type ) = 0;
	
	/**********************************************************************
	* 函数名称:	RemoveAttendMerch
	* 功能描述:	删除某类关注商品
	* 输入参数:	type 删除的类型
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			RemoveAttendMerch(E_AttendMerchType type) = 0;

	/**********************************************************************
	* 函数名称:	ForceUpdateVisibleAttentIoView
	* 功能描述:	强制更新已经监控的可见视图
	* 输入参数: NA
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			ForceUpdateVisibleAttentIoView() = 0;		


	// 处理强弱度相关的几个指数
public:	

	/**********************************************************************
	* 函数名称:	GetMerchPowerValue
	* 功能描述:	处理个股强弱度等xx, 根据商品判断个股的强弱度
	* 输入参数: pMerchStock 商品指针
	* 输出参数: fPowerValue 强弱度值
	* 返 回 值:	成功 true  失败false
	***********************************************************************/
	virtual bool32			GetMerchPowerValue(IN CMerch *pMerchStock, OUT float &fPowerValue) = 0;	

	/**********************************************************************
	* 函数名称:	GetMerchManagerType
	* 功能描述:	返回商品列表中包含商品类型标志，避免每次搜索
	* 输出参数:	NA
	* 返 回 值:	E_MerchManagerType类型变量
	***********************************************************************/
	virtual DWORD			GetMerchManagerType() = 0;				

public:
	typedef CArray<T_GeneralNormal, T_GeneralNormal&> GeneralNormalArray;
	/**********************************************************************
	* 函数名称:	GetGeneralNormalArray
	* 功能描述:	获取大盘普通数据
	* 输入参数:	pMerch 指定商品指针
	* 输出参数: NA
	* 返 回 值:	GeneralNormalArray* 大盘数据数数组
	***********************************************************************/
	virtual const GeneralNormalArray *GetGeneralNormalArray(CMerch *pMerch) = 0;

    /**********************************************************************
	* 函数名称:	GetTradeTimeInfoArray
	* 功能描述:	获取交易时间数组
	* 输出参数: NA
	* 返 回 值:	arrClientTradeTimeInfo* 交易时间数组
	***********************************************************************/
	virtual const arrClientTradeTimeInfo &GetTradeTimeInfoArray() = 0;

	/**********************************************************************
	* 函数名称:	GetGeneralNormalArrayH
	* 功能描述:	获取沪市的大盘普通数据
	* 输出参数: NA
	* 返 回 值:	GeneralNormalArray* 大盘数据数数组
	***********************************************************************/
	virtual const GeneralNormalArray &GetGeneralNormalArrayH() = 0;

	/**********************************************************************
	* 函数名称:	GetGeneralNormalArrayS
	* 功能描述:	获取深市的大盘普通数据
	* 输出参数: NA
	* 返 回 值:	GeneralNormalArray* 大盘数据数数组
	***********************************************************************/
	virtual const GeneralNormalArray &GetGeneralNormalArrayS() = 0;

	/**********************************************************************
	* 函数名称:	GetGeneralNormalHFlag
	* 功能描述:	获取沪市的大盘普通数据买卖标志
	* 输入参数: NA
	* 输出参数:	iFlagBuy  买标示
				iFlagSell 卖标示
	* 返 回 值:	成功true， 失败false
	***********************************************************************/
	virtual bool32			GetGeneralNormalHFlag(OUT int32& iFlagBuy, OUT int32& iFlagSell) = 0;

	/**********************************************************************
	* 函数名称:	GetGeneralNormalSFlag
	* 功能描述:	获取深市的大盘普通数据买卖标志
	* 输入参数:	NA
	* 输出参数:	iFlagBuy 买标示
				iFlagSell 卖标示
	* 返 回 值:	成功true， 失败false
	***********************************************************************/
	virtual bool32			GetGeneralNormalSFlag(OUT int32& iFlagBuy, OUT int32& iFlagSell) = 0;

	/**********************************************************************
	* 函数名称:	GetGeneralFinanceH
	* 功能描述:	获取沪市的大盘财务数据
	* 输入参数: NA
	* 输出参数: NA
	* 返 回 值:	T_GeneralFinance 财务数据结构
	***********************************************************************/
	virtual const T_GeneralFinance& GetGeneralFinanceH() = 0;

	/**********************************************************************
	* 函数名称:	GetGeneralFinanceS
	* 功能描述:	获取深市的大盘财务数据
	* 输入参数: NA
	* 输出参数: NA
	* 返 回 值:	T_GeneralFinance 财务数据结构
	***********************************************************************/
	virtual const T_GeneralFinance& GetGeneralFinanceS() = 0;

	/**********************************************************************
	* 函数名称:	GetFundHold
	* 功能描述:	获取商品的基金持仓信息
	* 输出参数: NA
	* 返 回 值:	返回基金持仓数据
	***********************************************************************/
	virtual const MapFundHold	&GetFundHold() const = 0;

public:
	/**********************************************************************
	* 函数名称:	ReSetLogData
	* 功能描述:	重新登录相关数据数据
	* 输入参数: NA
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			ReSetLogData() = 0;

	// 
public:
	 
	/**********************************************************************
	* 函数名称:	AddViewDataListner
	* 功能描述:	添加数据收听者
	* 输入参数:	pListener	数据监听对象
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			AddViewDataListner(CViewDataListner* pListener) = 0;

	/**********************************************************************
	* 函数名称:	DelViewDataListner
	* 功能描述:	删除数据收听者
	* 输入参数:	pListener	数据监听对象
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			DelViewDataListner(CViewDataListner* pListener) = 0;	

	/**********************************************************************
	* 函数名称:	AddViewNewsListner
	* 功能描述:	添加资讯收听者
	* 输入参数:	pListener	资讯接收对象
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void		    AddViewNewsListner(CViewNewsListner* pListener) = 0;

	/**********************************************************************
	* 函数名称:	DelViewNewsListner
	* 功能描述:	删除资讯收听者
	* 输入参数:	pListener	资讯接收对象
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			DelViewNewsListner(CViewNewsListner* pListener) = 0;

public:

	/**********************************************************************
	* 函数名称:	SetOffLineData
	* 功能描述:	是否离线数据
	* 输入参数: bOffLine	是否离线数据
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			SetOffLineData(bool bOffLine) = 0;	

	/**********************************************************************
	* 函数名称:	OfflineLogin
	* 功能描述:	离线登录
	* 输入参数:	StrUserName:
				StrServerIP:
	* 输入参数:	StrKey:
	* 输入参数:	IN const CString & StrOrgCode:
	* 输出参数: NA
	* 返 回 值:	bool32
	***********************************************************************/
	virtual bool32			OfflineLogin(IN CString& StrUserName, IN const CString& StrServerIP , IN const CString& StrKey, IN const CString& StrOrgCode) = 0;

	/**********************************************************************
	* 函数名称:	OffLineInitial
	* 功能描述:	脱机登录的初始化过程
	* 输入参数:	StrUserName		用户名
				StrErr			错误信息
	* 输出参数: NA
	* 返 回 值:	bool32
	***********************************************************************/
	virtual bool32			OffLineInitial(IN CString& StrUserName, OUT CString& StrErr) = 0;

	/**********************************************************************
	* 函数名称:	IsOffLineLogin
	* 功能描述:	是否脱机扽牢固
	* 输入参数: NA
	* 输出参数: NA
	* 返 回 值:	bool32
	***********************************************************************/
	virtual bool32			IsOffLineLogin() = 0;

public:
	
	/**********************************************************************
	* 函数名称:	BeLocalOfflineDataEnough
	* 功能描述:	判断本地数据是否足够
	* 输入参数:	eType 基本K线类型E_KLineTypeBase
	* 输出参数: NA:
	* 返 回 值:	成功true， 失败false
	***********************************************************************/
	virtual bool32			BeLocalOfflineDataEnough(E_KLineTypeBase eType) = 0;

public:
	/**********************************************************************
	* 函数名称:	BeAuthError
	* 功能描述:	行情认证是否出错
	* 输入参数:	eType 基本K线类型E_KLineTypeBase
	* 返 回 值:	成功true， 失败false
	***********************************************************************/
	virtual bool32			BeAuthError() const = 0;

public:
	
	/**********************************************************************
	* 函数名称:	GetUserName
	* 功能描述:	获取用户名
	* 输入参数: NA
	* 输出参数: NA
	* 返 回 值:	CString
	***********************************************************************/
	virtual CString			GetUserName() = 0;

	/**********************************************************************
	* 函数名称:	GetUserPwd
	* 功能描述:	获取用户密码
	* 输入参数: NA
	* 输出参数: NA
	* 返 回 值:	CString
	***********************************************************************/
	virtual CString			GetUserPwd() = 0;

	/**********************************************************************
	* 函数名称:	GetProduct
	* 功能描述:	认证key
	* 输入参数: NA
	* 输出参数: NA
	* 返 回 值:	CString
	***********************************************************************/
	virtual CString			GetProduct() = 0;

	/**********************************************************************
	* 函数名称:	SetManualReConnectFlag
	* 功能描述:	是否在手动重连服务器
	* 输入参数:	bFlag	是否手动重连
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			SetManualReConnectFlag(bool bFlag) = 0;
	
	/**********************************************************************
	* 函数名称:	GetIoViewList
	* 功能描述:	获取业务视图监听列表
	* 输入参数: NA
	* 输出参数: NA
	* 返 回 值:	返回视图监听列表
	***********************************************************************/
	virtual const CArray<CIoViewListner*, CIoViewListner*> &GetIoViewList() = 0;

	/**********************************************************************
	* 函数名称:	OnRealtimePrice
	* 功能描述:	处理行情数据更新
	* 输入参数:  CRealtimePrice 行情数据  iCommunicationId 连接ID   iReqId 请求ID
	* 输出参数: NA
	* 返 回 值: void
	***********************************************************************/
	virtual void OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId) = 0;

public:

	/**********************************************************************
	* 函数名称:	SetConfigRequstCount
	* 功能描述:	设置配置文件请求数量
	* 输入参数:	iCount		配置文件请求个数
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void            SetConfigRequstCount(int32 iCount) = 0;

	/**********************************************************************
	* 函数名称:	SetBridge
	* 功能描述:	内登录用到的API
	* 输入参数:	HWND hWnd:
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void			SetBridge(HWND hWnd) = 0;

public:
	/**********************************************************************
	* 函数名称:	GetLoginState
	* 功能描述:	获取登录状态
	* 输入参数:	NA
	* 输出参数: NA
	* 返 回 值:	E_LoginState 登录状态枚举值
	***********************************************************************/
	virtual E_LoginState GetLoginState() = 0;

	/**********************************************************************
	* 函数名称:	SetLoginState
	* 功能描述:	eLoginState 要设置的登录状态枚举值
	* 输入参数:	HWND hWnd:
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void SetLoginState(E_LoginState eLoginState)  = 0;

   	/**********************************************************************
	* 函数名称:	GetKickOutState
	* 功能描述:	获取踢线状态
	* 输入参数:	NA
	* 输出参数: NA
	* 返 回 值:	bool32 踢线状态
	***********************************************************************/
	virtual bool32 GetKickOutState() = 0;

	/**********************************************************************
	* 函数名称:	SetKickOutState
	* 功能描述:	设置踢线状态
	* 输入参数:	bKickOut 踢线状态
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void SetKickOutState(bool32 bKickOut) = 0;


public:
	/**********************************************************************
	* 函数名称:	ResetDataByOffLineData
	* 功能描述:	清空下载数据
	* 输入参数:	NA
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/

	virtual void ResetDataByOffLineData() = 0;
	/**********************************************************************
	* 函数名称:	GetLastTimeByOffLineData
	* 功能描述:	取各种类型的最后下载时间. 其实就是 000001 的时间.
	* 输入参数:	eType K线基本类型
	* 输出参数: NA
	* 返 回 值:	CGmtTime 时间
	***********************************************************************/
	virtual CGmtTime GetLastTimeByOffLineData(E_KLineTypeBase eType) = 0;

	/**********************************************************************
	* 函数名称:	DownLoadRealTimePriceDataByOffLineData
	* 功能描述:	下载行情数据
	* 输入参数:	NA
	* 输出参数: stDownLoadOffLine 离线行情数据
	* 返 回 值:	void
	***********************************************************************/
	virtual void DownLoadRealTimePriceDataByOffLineData(const T_DownLoadOffLine& stDownLoadOffLine) = 0;

	/**********************************************************************
	* 函数名称:	DownLoadKLineDataByOffLineData
	* 功能描述:	下载 K 线数据
	* 输入参数:	NA
	* 输出参数: stDownLoadOffLine 离线K线数据
	* 返 回 值:	void
	***********************************************************************/
	virtual void DownLoadKLineDataByOffLineData(const T_DownLoadOffLine& stDownLoadOffLine) = 0;

	/**********************************************************************
	* 函数名称:	DownLoadF10DataByOffLineData
	* 功能描述:	下载F10线数据
	* 输入参数:	NA
	* 输出参数: stDownLoadOffLine F10数据
	* 返 回 值:	void
	***********************************************************************/
	virtual void DownLoadF10DataByOffLineData(const T_DownLoadOffLine& stDownLoadOffLine) = 0;

	/**********************************************************************
	* 函数名称:	StopDownLoadByOffLineData
	* 功能描述:	停止所有下载
	* 输入参数:	NA
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void StopDownLoadByOffLineData() = 0;

	/**********************************************************************
	* 函数名称:	SetNotifyByOffLineData
	* 功能描述:	设置下载通知类
	* 输入参数:	COffLineDataNotify 通知类指针
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void SetNotifyByOffLineData(COffLineDataNotify* pNotify) = 0;

	/**********************************************************************
	* 函数名称:	GetDownLoadStatusByOffLineData
	* 功能描述:	获取是否下载盘后数据状态
	* 输入参数:	NA
	* 输出参数: NA
	* 返 回 值:	bool
	***********************************************************************/
	virtual bool GetDownLoadStatusByOffLineData() = 0;

	/**********************************************************************
	* 函数名称:	SetDownLoadStatusByOffLineData
	* 功能描述:	设置盘后数据下载状态
	* 输入参数:	bDownLoadStatus 是否正在下载
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void SetDownLoadStatusByOffLineData(bool bDownLoadStatus) = 0;

	/**********************************************************************
	* 函数名称:	GetTodayFlag
	* 功能描述:	获取今评标志
	* 输入参数:	NA
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual bool GetTodayFlag() = 0;

	/**********************************************************************
	* 函数名称:	SetTodayFlag
	* 功能描述:	获取金评标志
	* 输入参数:	bFlag  今评标志位
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void SetTodayFlag(bool bFlag) = 0;

	/**********************************************************************
	* 函数名称:	GetQuoteAuthJson
	* 功能描述:	获取用户信息json串
	* 输入参数:	NA
	* 输出参数: strUserInfo	用户信息json串
	* 返 回 值:	void
	***********************************************************************/
	virtual void GetQuoteAuthJson(CString& strUserInfo) = 0;

	/**********************************************************************
	* 函数名称:	RequestStatusBarViewData
	* 功能描述:	右下的商品数据请求
	* 输入参数:	NA
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void RequestStatusBarViewData() = 0;	

public:
	/**********************************************************************
	* 函数名称:	SetDownLoadStatusByOffLineData
	* 功能描述:	多线程并单线程函数响应函数
	* 输入参数:	emMsgType 消息类型
	* 输入参数:	wParam 参数1
	* 输入参数:	lParam 参数2
	* 输出参数: NA
	* 返 回 值:	void
	***********************************************************************/
	virtual void OnMsgTypeResponse(E_MsgTypeResponse emMsgType, WPARAM wParam, LPARAM lParam) = 0;
	
	
};


ENGINECENTER_DLL_EXPORT CAbsCenterManager* GetCenterManagerObj(int iType = 1);

#endif

