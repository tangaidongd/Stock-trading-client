#ifndef _ICONFIGNOTIFY_H_
#define _ICONFIGNOTIFY_H_

#define IN
#define OUT

#include <tchar.h>

class iConfigNotify
{
public:

	/**********************************************************************
	* 函数名称:	OnRespAdvInfo
	* 功能描述:	广告设置信息返回
	* 输入参数:	bSucc , true 成功;false失败
	*			wchar_t *pszMsg: 错误消息
	* 输出参数:	NA
	* 返 回 值:	NA
	* 其它说明:	NA
	* 历史记录:	原始版本
	***********************************************************************/
	virtual void OnRespAdvInfo(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* 函数名称:	OnRespCommTextConfig
	* 功能描述:	通用的文本配置返回
	* 输入参数:	bSucc , true 成功;false失败
	*			wchar_t *pszMsg: 错误消息
	* 输出参数:	NA
	* 返 回 值:	NA
	* 其它说明:	NA
	* 历史记录:	原始版本
	***********************************************************************/
	virtual void OnRespCommTextConfig(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* 函数名称:	OnRespReportTabList
	* 功能描述:	报价表底部tab列表返回
	* 输入参数:	bSucc , true 成功;false失败
	*			wchar_t *pszMsg: 错误消息
	* 输出参数:	NA
	* 返 回 值:	NA
	* 其它说明:	NA
	* 历史记录:	原始版本
	***********************************************************************/
	virtual void OnRespReportTabList(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* 函数名称:	OnRespToolbarFile
	* 功能描述:	工具栏配置文件返回
	* 输入参数:	bSucc , true 成功;false失败
	*			wchar_t *pszMsg: 错误消息
	* 输出参数:	NA
	* 返 回 值:	NA
	* 其它说明:	NA
	* 历史记录:	原始版本
	***********************************************************************/
	virtual void OnRespToolbarFile(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* 函数名称:	OnRespDownloadUserData
	* 功能描述:	用户私有数据下载返回
	* 输入参数:	bSucc , true 成功;false失败
	*			wchar_t *pszMsg: 错误消息
	* 输出参数:	NA
	* 返 回 值:	NA
	* 其它说明:	NA
	* 历史记录:	原始版本
	***********************************************************************/
	virtual void OnRespDownloadUserData(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* 函数名称:	OnRespUploadUserData
	* 功能描述:	用户私有数据上传返回
	* 输入参数:	bSucc , true 成功;false失败
	*			wchar_t *pszMsg: 错误消息
	* 输出参数:	NA
	* 返 回 值:	NA
	* 其它说明:	NA
	* 历史记录:	原始版本
	***********************************************************************/
	virtual void OnRespUploadUserData(IN bool bSucc, IN const wchar_t *pszMsg) = 0;
};

#endif