#ifndef __PROXY_AUTH_CLIENT_H__
#define __PROXY_AUTH_CLIENT_H__

#include "iAuthNotify.h"
#include "ProxyAuthStruct.h"

namespace auth
{
	class iServiceDispose
	{
	public:
		// 调用此函数来释放由此DLL返回的需要delete的内存指针
		virtual void ReleaseData(void *pData) = 0;

		// 获取用户信息
		virtual void GetUserInfo(OUT T_UserInfo & stInfo) = 0;

		// 获取大市场列表, 返回的指针需要调用ReleaseData来delete []
		virtual void GetBigMarketList(OUT T_BigMarketInfo **pstBigMarket, OUT int &iCount) = 0;

		// 获取市场列表, 返回的指针需要调用ReleaseData来delete []
		virtual void GetMarketList(OUT T_MarketInfo **pstMarket, OUT int &iCount) = 0;

		// 获取服务器列表, 返回的指针需要调用ReleaseData来delete []
		virtual void GetServerList(OUT T_ServerInfo **pstServer, OUT int &iCount) = 0;

		// 获取功能权限, pstRight需要调用ReleaseData来delete[]
		virtual void GetFuncRightList(OUT T_RightInfo **pstRight, OUT int &iCount) = 0;

		// 获取权限提示内容
		virtual void GetRightTip(OUT T_RightTip **pstRightTip, OUT int &iCount) = 0;

		// 获取消息提示
		virtual void GetMsgTip(OUT T_MsgTip & stInfo) = 0;

		// 获取广告信息列表, 返回的指针需要调用ReleaseData来delete []
		virtual void GetAdvList(OUT T_AdvSet **pstAdv, OUT int &iCount) = 0;

		// 获取报价表tab列表, 返回的指针需要调用ReleaseData来delete []
		virtual void GetReportTabList(OUT T_ReportTabInfo **pstTabInfo, OUT int &iCount) = 0;

		// 获取文字导航
		virtual void GetTextBanner(OUT T_TextBannerConfig &stTextBannerConfig) = 0;

		// 获取工具栏文件版本
		virtual const char* GetToolBarVer() = 0;

		// 获取自选股数据
		virtual const char* GetCusoptional() = 0;

		// 工具栏文件是否变更
		virtual bool IsToolBarModify() = 0;

		// 获取会话token
		virtual const char* GetToken() = 0;

		// 获取资讯平台编码
		virtual const char* GetCodePlatForm() = 0;

		// 获取资讯机构编码
		virtual const char* GetOrgCode() = 0;
	};

	class iProxyAuthClient
	{
	public:

		typedef void (*CALL_ECONO_FUN)(const char*);

		/**********************************************************************
		* 函数名称:	ConnectServer
		* 功能描述:	连接服务器
		* 输入参数:	pwszServerAddr 服务器地址http://192.168.0.1:8088
		*			vServerPort 端口数组(绿色通道)
		* 输出参数:	NA
		* 返 回 值:	bool
		* 历史记录:	原始版本
		***********************************************************************/
		virtual bool ConnectServer(const wchar_t * pwszServerAddr, std::vector<unsigned int> &vServerPort) = 0;

		// 断开连接
		virtual void DisConnectServer() = 0;

		virtual void Release() = 0;

		// 获取数据接口
		virtual iServiceDispose * GetServiceDispose() = 0;

		// 设置回调
		virtual void SetNotifyObj(iAuthNotify *pNotifyObj) = 0;

		/**********************************************************************
		* 函数名称:	Authorize
		* 功能描述:	账号认证
		* 输入参数:	pwszMobile 手机号
		*			pwszPwd 密码
		*			pwszOrgCode 会员编码
		*			pwszVer 版本号
		* 输出参数:	NA
		* 返 回 值:	bool
		* 历史记录:	原始版本
		***********************************************************************/
		virtual bool Authorize(const wchar_t *pwszMobile, const wchar_t *pwszPwd, 
			const wchar_t *pwszOrgCode, const wchar_t *pwszVer) = 0;

		/**********************************************************************
		* 函数名称:	ThirdAuthorize
		* 功能描述:	第三方认证(qq, 微信)
		* 输入参数:	pwszLoginKey openId
		*			pwszOrgCode 会员编码
		*			iChannel 登陆渠道,1 微信 2 QQ
		* 输出参数:	NA
		* 返 回 值:	bool
		* 历史记录:	原始版本
		***********************************************************************/
		virtual bool ThirdAuthorize(const wchar_t *pwszLoginKey, const wchar_t *pwszOrgCode, int iChannel) = 0;

		/**********************************************************************
		* 函数名称:	AddNewUser
		* 功能描述:	添加新用户
		* 输入参数:	pwszOrgCode 机构编码
		*			pwszLoginName 登录名
		*			pwszMobile 手机号码
		*			pwszNickName 昵称
		*			pwszRegfrom 来源
		*			iGroupId 用户组
		* 输出参数:	NA
		* 返 回 值:	bool
		* 历史记录:	原始版本
		***********************************************************************/
		virtual bool AddNewUser(const wchar_t *pwszOrgCode, const wchar_t *pwszLoginName, const wchar_t *pwszMobile, 
			const wchar_t *pwszNickName, const wchar_t *pwszRegfrom, int iGroupId) = 0;

		/**********************************************************************
		* 函数名称:	RefreshToken
		* 功能描述:	刷新Token
		* 输入参数:	NA
		* 输出参数:	NA
		* 返 回 值:	bool
		* 历史记录:	原始版本
		***********************************************************************/
		virtual bool RefreshToken() = 0;

		/**********************************************************************
		* 函数名称:	ReqSaveCusoptional
		* 功能描述:	用户上传自选股
		* 输入参数:	pwszData 数据, Json格式
		*			iType 数据种类, 1 自选股
		* 输出参数:	NA
		* 返 回 值:	true 成功, false 失败
		* 历史记录:	原始版本
		***********************************************************************/
		virtual bool ReqSaveCusoptional(const wchar_t *pwszData, const int iType) = 0;

		/**********************************************************************
		* 函数名称:	ReqQueryCusoptional
		* 功能描述:	用户获取自选股
		* 输入参数:	iType 数据种类, 1 自选股
		* 输出参数:	NA
		* 返 回 值:	true 成功, false 失败
		* 历史记录:	原始版本
		***********************************************************************/
		virtual bool ReqQueryCusoptional(const int iType) = 0;

		/**********************************************************************
		* 函数名称:	ReqAdvInfo
		* 功能描述:	获取广告
		* 输入参数:	pwszMdl 需要返回的模块代码列表, 多个模块，可用”,” 间隔
		*			pwszOrgCode 机构编码
		* 输出参数:	NA
		* 返 回 值:	true 成功, false 失败
		* 历史记录:	原始版本
		***********************************************************************/
		virtual bool ReqAdvInfo(const wchar_t *pwszMdl, const wchar_t *pwszOrgCode) = 0;

		/**********************************************************************
		* 函数名称:	ReqMsgTip
		* 功能描述:	请求消息提示
		* 输入参数:	pwszSendInter 提醒时间（分钟）, 5或10或15或30
		* 输出参数:	NA
		* 返 回 值:	bool
		* 历史记录:	原始版本
		***********************************************************************/
		virtual bool ReqMsgTip(const wchar_t *pwszSendInter) = 0;

		/**********************************************************************
		* 函数名称:	ReqQueryUserRight
		* 功能描述:	获取权限
		* 输入参数:	pwszMdl 需要返回的模块代码列表, 多个模块，可用”,” 间隔
		*			iType 类型 0无权限,1 有权限,2 所有的
		* 输出参数:	NA
		* 返 回 值:	true 成功, false 失败
		* 历史记录:	原始版本
		***********************************************************************/
		virtual bool ReqQueryUserRight(const wchar_t *pwszMdl, const int iType) = 0;

		/**********************************************************************
		* 函数名称:	ReqQueryRightTip
		* 功能描述:	获取权限提示
		* 输入参数:	pwszMdl 需要返回的模块代码列表, 多个模块，可用”,” 间隔
		* 输出参数:	NA
		* 返 回 值:	true 成功, false 失败
		* 历史记录:	原始版本
		***********************************************************************/
		virtual bool ReqQueryRightTip(const wchar_t *pwszMdl) = 0;

		/**********************************************************************
		* 函数名称:	ReqQueryServerInfo
		* 功能描述:	查询服务器市场相关信息
		* 输入参数:	pwszKey 产品key
		*			pwszOrgCode 会员编码
		*			iSerType 服务器类型, 1-行情,2-交易,4-消息,8-预警,16-pc行情,32-咨询服务器,可叠加掩码实现,不填写内容不返回
		*			bBigMarket 大市场，false不返回，true 返回
		*			iMarket	子市场， 1,市场，2板块 可叠加掩码实现，不填写内容不返回
		* 输出参数:	NA
		* 返 回 值:	true 成功, false 失败
		* 历史记录:	原始版本
		***********************************************************************/
		virtual bool ReqQueryServerInfo(const wchar_t *pwszKey, const wchar_t *pwszOrgCode, 
			const int iSerType,const bool bBigMarket, const int iMarket) = 0;

		/**********************************************************************
		* 函数名称:	ReqQueryBanner
		* 功能描述:	获取文字导航
		* 输入参数:	NA
		* 输出参数:	NA
		* 返 回 值:	true 成功, false 失败
		* 历史记录:	原始版本
		***********************************************************************/
		virtual bool ReqQueryBanner() = 0;

		/**********************************************************************
		* 函数名称:	ReqReportTabList
		* 功能描述:	请求报价表底部tab列表
		* 输入参数:	NA
		* 输出参数:	NA
		* 返 回 值:	true 成功, false 失败
		* 历史记录:	原始版本
		***********************************************************************/
		virtual bool ReqReportTabList() = 0;

		/**********************************************************************
		* 函数名称:	ReqStrategyInfo
		* 功能描述:	请求量化信息消息
		* 输入参数:	strAddr IP地址
		* 输入参数:	strHttpVer 协议号
		* 输入参数:	sJsonData  请求数据组包

		* 输出参数:	NA
		* 返 回 值:	true 成功, false 失败
		* 历史记录:	原始版本
		***********************************************************************/

		virtual bool ReqStrategyInfo(const wchar_t * strAddr, const wchar_t * strHttpVer, const char* sJsonData) = 0;

		/**********************************************************************
		* 函数名称:	ReqPickModelInfo
		* 功能描述:	请求请求智能选股类型的选股列表
		* 输入参数:	strAddr IP地址
		* 输入参数:	strHttpVer 协议号
		* 输入参数:	sJsonData  请求数据组包

		* 输出参数:	NA
		* 返 回 值:	true 成功, false 失败
		* 历史记录:	原始版本
		***********************************************************************/

		virtual bool ReqPickModelInfo(const wchar_t * strAddr, const wchar_t * strHttpVer, const char* sJsonData, const wchar_t * wszHead = L"Content-Type: application/json;charset:utf-8;") = 0;

		/**********************************************************************
		* 函数名称:	ReqPickModelStatus
		* 功能描述:	请求请求智能选股类型的选股状态
		* 输入参数:	strAddr IP地址
		* 输入参数:	strHttpVer 协议号
		* 输入参数:	sJsonData  请求数据组包

		* 输出参数:	NA
		* 返 回 值:	true 成功, false 失败
		* 历史记录:	原始版本
		***********************************************************************/

		virtual bool ReqPickModelStatus(const wchar_t * strAddr, const wchar_t * strHttpVer, const char* sJsonData, const wchar_t * wszHead = L"Content-Type: application/json;charset:utf-8;") = 0;


		/**********************************************************************
		* 函数名称:	ReqQueryNewStockInfo
		* 功能描述:	// 请求新股列表
		* 输入参数:	strAddr IP地址
		* 输入参数:	strHttpVer 协议号
		* 输入参数:	sJsonData  请求数据组包

		* 输出参数:	NA
		* 返 回 值:	true 成功, false 失败
		* 历史记录:	原始版本
		***********************************************************************/

		virtual  bool ReqQueryNewStockInfo(const wchar_t * strAddr, const wchar_t * strHttpVer, const char* sJsonData, bool bListedStock) = 0;

		/**********************************************************************
		* 函数名称:	ReqCheckToken
		* 功能描述:	请求检查token是否有效

		* 输出参数:	NA
		* 返 回 值:	true 成功, false 失败
		* 历史记录:	原始版本
		***********************************************************************/

		virtual bool ReqCheckToken() = 0;

		/**********************************************************************
		* 函数名称:	ReqToolbarFile
		* 功能描述:	请求工具栏配置文件
		* 输入参数:	pwszVer 版本号, 没有可不填写
		* 输出参数:	NA
		* 返 回 值:	true 成功, false 失败
		* 历史记录:	原始版本
		***********************************************************************/

		virtual bool ReqToolbarFile(const wchar_t *pwszVer) = 0;

		/**********************************************************************
		* 函数名称:	UserDealRecord
		* 功能描述:	用户行为记录
		* 输入参数:	vRecord 记录的数组
		* 输出参数:	NA
		* 返 回 值:	bool
		* 历史记录:	原始版本
		***********************************************************************/
		virtual bool UserDealRecord(const std::vector<T_UserDealRecord> &vRecord) = 0;

		/**********************************************************************
		* 函数名称:	ReqEconoData
		* 功能描述:	请求财经日历数据
		* 输入参数:	pwszServerAddr 财经日历服务器地址
		*			pszData	发送的数据
		* 输出参数:	NA
		* 返 回 值:	bool
		* 历史记录:	原始版本
		***********************************************************************/
		virtual bool ReqEconoData(const wchar_t *pwszServerAddr, const char *pszData) = 0;

		/**********************************************************************
		* 函数名称:	ReqPushMsgList
		* 功能描述:	请求推送消息列表
		* 输入参数:	pszVersion 版本
		* 输入参数:	pszOrgCode 机构号
		* 输入参数:	pszType 类型，1 代表推送消息
		* 输入参数:	pszCreateUser 消息创建者
		* 输入参数:	pszMsgTitle 消息标题
		* 输入参数:	pszPageIndex 开始页码
		* 输入参数:	pszPageSize 查询的页数
		* 输出参数:	NA
		* 返 回 值:	bool
		* 历史记录:	原始版本
		***********************************************************************/
		virtual bool ReqPushMsgList(const char *pszVersion, const char *pszOrgCode, const char* pszType, const char* pszMsgContent, 
			const char* pszMsgTitle, const char* pszGrooupIds, const char* pszState, const char* pszPageIndex, const char *pszPageSize) = 0;

		/**********************************************************************
		* 函数名称:	ReqPushMsgDetail
		* 功能描述:	请求推送消息详情
		* 输入参数:	pszMsgId消息ID
		* 输出参数:	NA
		* 返 回 值:	bool
		* 历史记录:	原始版本
		***********************************************************************/
		virtual bool ReqPushMsgDetail(const char *pszVersion, const char* pszMsgId) = 0;
	};
};

#endif
