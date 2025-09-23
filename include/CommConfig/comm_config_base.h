#ifndef __COMM_CONFIG_CLIENT_H__
#define __COMM_CONFIG_CLIENT_H__

#include "iConfigNotify.h"
#include <vector>

namespace commconfig
{
static const int g_iAuthVer = 6;	// DLL版本
static const int MAXNAME = 64;
static const int MAXVALUE = 256;

#pragma pack(push, 1)
/////////////////////////////////////////////////////////////////////////////
// 广告设置
typedef struct _AdvSet
{
	wchar_t wszAdCode[MAXNAME];	// 广告编码
	wchar_t	wszAdName[MAXNAME];	// 广告名称
	wchar_t wszRemark[MAXVALUE];// 备注
	wchar_t	wszAdPic[MAXVALUE];	// 广告图片地址
	wchar_t	wszAdLink[MAXVALUE];// 广告连接

	_AdvSet()
	{
		wszAdCode[0] = 0; 
		wszAdName[0] = 0; 
		wszRemark[0] = 0; 
		wszAdPic[0]	 = 0; 
		wszAdLink[0] = 0; 
	}
}T_AdvSet;

// 报价表底部显示的tab市场
typedef struct _ReportTabInfo
{
	int iMarketCode;			// 市场代码
	wchar_t	wszMName[MAXNAME];	// 市场名称

	_ReportTabInfo()
	{
		iMarketCode = 0; 
		wszMName[0] = 0; 
	}
}T_ReportTabInfo;

// 通用文本配置
typedef struct _CommonTextConfig
{
	wchar_t wszServiceTel[MAXNAME];		// 客服电话
	wchar_t wszStatusContent[1024];			// 状态栏滚动文本

	_CommonTextConfig()
	{
		wszServiceTel[0]	= 0; 
		wszStatusContent[0] = 0;
	} 
}T_CommonTextConfig;

#pragma pack(pop)

class iServiceDispose
{
public:
	// 调用此函数来释放由此DLL返回的需要delete的内存指针
	virtual void ReleaseData(void *pData) = 0;

	// 获取广告信息列表, 返回的指针需要调用ReleaseData来delete []
	virtual void GetAdvList(OUT T_AdvSet **pstAdv, OUT int &iCount) = 0;

	// 获取报价表tab列表, 返回的指针需要调用ReleaseData来delete []
	virtual void GetReportTabList(OUT T_ReportTabInfo **pstTabInfo, OUT int &iCount) = 0;

	// 获取通用文本配置
	virtual void GetCommonTextConfig(OUT T_CommonTextConfig &pstCommConfig) = 0;

	// 获取工具栏文件版本
	virtual const char* GetToolBarVer() = 0;

	// 获取用户私有数据
	virtual const char* GetUserData() = 0;

	// 工具栏文件是否变更
	virtual bool IsToolBarModify() = 0;
};

class iCommConfgClient
{
public:

	// 设置服务器的地址
	virtual bool AddServer(const wchar_t * StrServerAdd) = 0;

	virtual void Release() = 0;

	// 获取数据接口
	virtual iServiceDispose * GetServiceDispose() = 0;

	// 设置回调
	virtual void SetNotifyObj(iConfigNotify *pNotifyObj) = 0;

	/**********************************************************************
	* 函数名称:	ReqAdvInfo
	* 功能描述:	请求广告设置信息
	* 输入参数:	strToken 会话token
	*			strMdl 需要返回的模块代码列表, 多个模块，可用”,” 间隔
	* 输出参数:	NA
	* 返 回 值:	true 成功, false 失败
	* 历史记录:	原始版本
	***********************************************************************/
	virtual bool ReqAdvInfo(const wchar_t *strToken, const wchar_t *strMdl) = 0;

	/**********************************************************************
	* 函数名称:	ReqCommTextConfig
	* 功能描述:	请求通用的文本配置
	* 输入参数:	strToken 会话token
	* 输出参数:	NA
	* 返 回 值:	true 成功, false 失败
	* 历史记录:	原始版本
	***********************************************************************/
	virtual bool ReqCommTextConfig(const wchar_t *strToken) = 0;

	/**********************************************************************
	* 函数名称:	ReqReportTabList
	* 功能描述:	请求报价表底部tab列表
	* 输入参数:	strToken 会话token
	* 输出参数:	NA
	* 返 回 值:	true 成功, false 失败
	* 历史记录:	原始版本
	***********************************************************************/
	virtual bool ReqReportTabList(const wchar_t *strToken) = 0;

	/**********************************************************************
	* 函数名称:	ReqToolbarFile
	* 功能描述:	请求工具栏配置文件
	* 输入参数:	strToken 会话token
	*			strVer 版本号, 没有可不填写
	* 输出参数:	NA
	* 返 回 值:	true 成功, false 失败
	* 历史记录:	原始版本
	***********************************************************************/
	virtual bool ReqToolbarFile(const wchar_t *strToken, const wchar_t *strVer) = 0;

	/**********************************************************************
	* 函数名称:	ReqDownloadUserData
	* 功能描述:	请求下载用户私有数据
	* 输入参数:	strToken 会话token
	*			strDataType 数据类型, 1 表示自选股数据
	* 输出参数:	NA
	* 返 回 值:	true 成功, false 失败
	* 历史记录:	原始版本
	***********************************************************************/
	virtual bool ReqDownloadUserData(const wchar_t *strToken, const wchar_t *strDataType) = 0;

	/**********************************************************************
	* 函数名称:	ReqUploadUserData
	* 功能描述:	请求下载用户私有数据
	* 输入参数:	strToken 会话token
	*			strDataType 数据类型, 1 表示自选股数据
	* 输出参数:	NA
	* 返 回 值:	true 成功, false 失败
	* 历史记录:	原始版本
	***********************************************************************/
	virtual bool ReqUploadUserData(const wchar_t *strToken, const wchar_t *strDataType, const wchar_t *strData) = 0;
};

};

#endif
