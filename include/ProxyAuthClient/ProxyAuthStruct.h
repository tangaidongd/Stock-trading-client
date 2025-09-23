#ifndef __PROXY_AUTH_STRUCT_H__
#define __PROXY_AUTH_STRUCT_H__

#include <vector>
#include "SharePubStruct.h"

namespace auth
{
	static const int g_iAuthVer = 6;	// DLL版本
	static const int MAXNAME = 64;
	static const int MAXVALUE = 256;
	static const int MAXCONTENT = 1024;

	enum E_ErrType
	{
		enETOther,			// 其它错误
		enETNetErr,			// 网络错误
	};

	// 服务器提供的数据服务类别
	enum E_DataServiceType
	{
		EDSTUnknown		= 0x00,		// 未知的数据服务

		//
		EDSTGeneral		= 0x01,		// 提供除K线（走势）数据、5档行情报价、level2数据、tick数据外的基本数据服务， 如：市场列表、商品基本信息数据

		//
		EDSTKLine		= 0x02,		// 历史K线（走势）
		EDSTTimeSale	= 0x04,		// 历史分笔数据

		// 实时数据
		EDSTPrice		= 0x08,		// 实时行情数据
		EDSTLevel2		= 0x10,		// 实时level2数据服务
		EDSTTick		= 0x20		// 实时tick数据服务		
	};

	//enum E_ServerType
	//{
	//	enSTNone	= 0,
	//	enSTQuote	= 1, 	// 行情服务器
	//	enSTTrade	= 2, 	// 交易服务器
	//	enSTNews	= 4,	// 消息服务器
	//	enSTWarn	= 8,	// 预警
	//	enSTInfo    = 32,   // 资讯服务器
	//};

	enum PROXYTYPE 
	{
		NONE, 
		SOCK4, 
		SOCK5, 
		HTTP
	};

	// 用户行为记录中的操作类型
	enum E_ProductType
	{
		EPTFun	= 1,		// 功能切换
		EPTCode	= 2,		// 代码切换
	};

#pragma pack(push, 1)
	/////////////////////////////////////////////////////////////////////////////

	// 关注的商品信息
	typedef struct _AttendMerch
	{
		wchar_t		wszMerchCode[MAXNAME];	// 商品代码		
		int			iMarketId;				// 商品所属市场id
		int			iDataServiceTypes;		// 取值 E_DataServiceType的或值
		_AttendMerch()
		{
			wszMerchCode[0] = 0;
			iMarketId = 0;
			iDataServiceTypes = 0;
		}
	}T_AttendMerch;

	typedef struct _BigMarketInfo
	{
		int		iBigMarket;		// 大市场ID
		int		iShowID;		// 显示ID
		int		iCode;			// 市场Code
		wchar_t	wszCnName[MAXNAME];
		_BigMarketInfo()
		{
			iBigMarket	 = 0;
			iShowID = 0;
			iCode	= 0;
			wszCnName[0] = 0;
		}
	}T_BigMarketInfo;

	typedef struct _MarketInfo
	{
		int		iBigMarket;		// 大市场ID
		int		iMarket;		// 市场
		wchar_t	wszName[64];	// 名字
		int		iShowID;		// 显示ID
		_MarketInfo()
		{
			iBigMarket	= 0;
			iMarket		= 0;
			wszName[0]	= 0;
			iShowID		= 0;
		}
	}T_MarketInfo;

	

	typedef struct _UserInfo
	{
		int				iUserId;					// 用户id
		wchar_t			wszUserName[MAXNAME];		// 用户名
		wchar_t			wszNickName[MAXNAME];		// 昵称
		wchar_t			wszQQ[MAXNAME];				// QQ号
		wchar_t			wszMobile[MAXNAME];			// 手机号
		wchar_t			wszImg[MAXVALUE];			// 图片数据
		int				iUserGroup;					// 用户组
		int				iGroupType;					// 1 营销，2 工作人员 
		unsigned long   dwRegisterTime;				// 注册时间
		wchar_t			wszUserGroup[MAXNAME];		// 用户组名
		wchar_t			wszToken[MAXNAME];			// token

		_UserInfo()
		{
			iUserId			= 0;
			wszUserName[0]	= 0;
			wszNickName[0]	= 0;
			wszQQ[0]		= 0;
			wszMobile[0]	= 0;
			wszImg[0]		= 0;
			iUserGroup		= 0;
			iGroupType		= 1;
			dwRegisterTime   = 0;
			wszUserGroup[0] = 0;
			wszToken[0]		= 0;
		};
	}T_UserInfo;

	// 功能权限
	typedef struct _RightInfo
	{
		int			iHasFun;			// 是否有权限,0没有权限
		int			iFunID;				// 权限ID
		wchar_t		wszName[MAXNAME];	// 名字
		_RightInfo()
		{
			iHasFun	   = 0;
			iFunID	   = 0;
			wszName[0] = 0;
		}
	}T_RightInfo;

	// 权限提示内容
	typedef struct _RightTip
	{
		int				iFunID;					// 权限ID
		wchar_t			wszTitle[MAXVALUE];		// 提示标题
		wchar_t			wszImgUrl[MAXVALUE];	// 图片地址
		wchar_t			wszLinkUrl[MAXVALUE];	// 点击链接

		_RightTip()
		{
			iFunID			= 0;
			wszTitle[0]		= 0;
			wszImgUrl[0]	= 0;
			wszLinkUrl[0]	= 0;
		};
	}T_RightTip;

	// 消息提示
	typedef struct _MsgTip
	{
		wchar_t			wszTitle[MAXVALUE];		// 消息标题
		wchar_t			wszContent[MAXCONTENT];	// 消息内容
		wchar_t			wszLinkUrl[MAXVALUE];	// 附带链接

		_MsgTip()
		{
			wszTitle[0]		= 0;
			wszContent[0]	= 0;
			wszLinkUrl[0]	= 0;
		};
	}T_MsgTip;

	// 用户行为记录
	typedef struct _UserDealRecord
	{
		wchar_t		  wszFunName[MAXNAME];	// 功能名称
		E_ProductType eProductType;			// 类型, 01:功能切换; 02:代码切换
		int			  iViewCnt;				// 浏览次数

		_UserDealRecord()
		{
			wszFunName[0]	= 0;
			eProductType	= EPTFun;
			iViewCnt		= 0;
		}
	}T_UserDealRecord;

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

	// 文字导航配置
	typedef struct _TextBannerConfig
	{
		wchar_t wszServiceTel[MAXNAME];		// 客服电话
		wchar_t wszStatusContent[1024];		// 状态栏滚动文本

		_TextBannerConfig()
		{
			wszServiceTel[0]	= 0; 
			wszStatusContent[0] = 0;
		} 
	}T_TextBannerConfig;

#pragma pack(pop)
};

#endif
