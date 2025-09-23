#ifndef _IAUTHNOTIFY_H_
#define _IAUTHNOTIFY_H_

#define IN
#define OUT

#include <tchar.h>

class iAuthNotify
{
public:
	/**********************************************************************
	* 函数名称:	OnRespAuth
	* 功能描述:	认证失败
	* 输入参数:	bSucc , true 成功;false失败
	*			wchar_t *pszMsg: 错误消息
	***********************************************************************/
	virtual void OnRespAuth(bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* 函数名称:	OnRespUserRight
	* 功能描述:	客户权限返回
	* 输入参数:	bSucc , true 成功;false失败
	*			wchar_t *pszMsg: 错误消息
	***********************************************************************/
	virtual void OnRespUserRight(bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* 函数名称:	OnRespRightTip
	* 功能描述:	无权限内容提示返回
	* 输入参数:	bSucc , true 成功;false失败
	*			wchar_t *pszMsg: 错误消息
	* 历史记录:	原始版本
	***********************************************************************/
	virtual void OnRespRightTip(bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* 函数名称:	OnRespMsgTip
	* 功能描述:	消息提示返回
	* 输入参数:	bSucc , true 成功;false失败
	*			wchar_t *pszMsg: 错误消息
	* 输出参数:	NA
	* 返 回 值:	NA
	* 其它说明:	NA
	* 历史记录:	原始版本
	***********************************************************************/
	virtual void OnRespMsgTip(bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* 函数名称:	OnRespUserDealRecord
	* 功能描述:	用户行为记录返回
	* 输入参数:	bSucc , true 成功;false失败
	*			wchar_t *pszMsg: 返回的信息
	***********************************************************************/
	virtual void OnRespUserDealRecord(bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* 函数名称:	OnRespQueryServerInfo
	* 功能描述:	查询服务器市场相关信息返回
	* 输入参数:	bSucc , true 成功;false失败
	*			wchar_t *pszMsg: 错误消息
	***********************************************************************/
	virtual void OnRespQueryServerInfo(bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* 函数名称:	OnRespAdvInfo
	* 功能描述:	广告信息返回
	* 输入参数:	bSucc , true 成功;false失败
	*			wchar_t *pszMsg: 错误消息
	***********************************************************************/
	virtual void OnRespAdvInfo(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* 函数名称:	OnRespTextBannerConfig
	* 功能描述:	文本导航返回
	* 输入参数:	bSucc , true 成功;false失败
	*			wchar_t *pszMsg: 错误消息
	***********************************************************************/
	virtual void OnRespTextBannerConfig(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* 函数名称:	OnRespReportTabList
	* 功能描述:	报价表底部tab列表返回
	* 输入参数:	bSucc , true 成功;false失败
	*			wchar_t *pszMsg: 错误消息
	***********************************************************************/
	virtual void OnRespReportTabList(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* 函数名称:	OnRespToolbarFile
	* 功能描述:	工具栏配置文件返回
	* 输入参数:	bSucc , true 成功;false失败
	*			wchar_t *pszMsg: 错误消息
	***********************************************************************/
	virtual void OnRespToolbarFile(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* 函数名称:	OnRespSaveCusoptional
	* 功能描述:	上传自选股返回
	* 输入参数:	bSucc , true 成功;false失败
	*			wchar_t *pszMsg: 错误消息
	***********************************************************************/
	virtual void OnRespSaveCusoptional(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* 函数名称:	OnRespQueryCusoptional
	* 功能描述:	下载自选股返回
	* 输入参数:	bSucc , true 成功;false失败
	*			wchar_t *pszMsg: 错误消息
	***********************************************************************/
	virtual void OnRespQueryCusoptional(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* 函数名称:	OnEconoData
	* 功能描述:	收到财经日历的响应函数
	* 输入参数:	pszRecvData 收到的财经日历数据
	***********************************************************************/
	virtual void OnEconoData(const char *pszRecvData) = 0;

	/**********************************************************************
	* 函数名称:	OnRespStrategyData
	* 功能描述:	获取量化信号返回
	* 输入参数:	bSucc , true 成功;false失败
	*			const char *pszRecvData:返回的数据
	***********************************************************************/
	virtual void OnRespStrategyData(const char *pszRecvData) = 0;

	/**********************************************************************
	* 函数名称:	OnRespPickModelTypeInfo
	* 功能描述:	请求请求智能选股类型的选股列表
	* 输入参数:	bSucc , true 成功;false失败
	*			const char *pszRecvData:返回的数据
	***********************************************************************/
	virtual void OnRespPickModelTypeInfo(const char *pszRecvData) = 0;

	/**********************************************************************
	* 函数名称:	OnRespPickModelTypeStatus
	* 功能描述:	请求请求智能选股类型选股状态，是否有选股
	* 输入参数:	bSucc , true 成功;false失败
	*			const char *pszRecvData:返回的数据
	***********************************************************************/
	virtual void OnRespPickModelTypeStatus(const char *pszRecvData) = 0;

	/**********************************************************************
	* 函数名称:	OnRespQueryNewStockInfo
	* 功能描述: 请求上市和未上市的股票列表
	* 输入参数:	bSucc , true 成功;false失败
	*			const char *pszRecvData:返回的数据
				const bool bListedStock:是否已上市新股
	***********************************************************************/
	virtual void OnRespQueryNewStockInfo(const char *pszRecvData, const bool bListedStock) = 0;

	/**********************************************************************
	* 函数名称:	OnRespCheckToken
	* 功能描述:	收到token验证结果
	* 输入参数:	const char *pszRecvData:返回的数据
	***********************************************************************/
	virtual void OnRespCheckToken(const char *pszRecvData) = 0;

		/**********************************************************************
	* 函数名称:	OnRespPushMsgList
	* 功能描述:	收到推送消息列表
	* 输入参数:	const char *pszRecvData:返回的数据
	***********************************************************************/
	virtual void OnRespPushMsgList(const char *pszRecvData) = 0;

		/**********************************************************************
	* 函数名称:	OnRespPushMsgDetail
	* 功能描述:	收到推送消息详情
	* 输入参数:	const char *pszRecvData:返回的数据
	***********************************************************************/
	virtual void OnRespPushMsgDetail(const char *pszRecvData) = 0;
};

#endif