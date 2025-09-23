#pragma once

#ifndef _CONFIG_INFO_H_
#define _CONFIG_INFO_H_

#include <map>
#include <algorithm>
using std::map;

#include "PluginFuncRight.h"
#include "StructDef.h"
#include "PubFonction.h"
#include "NCButton.h"




#define  INVALID_VALUE -1

enum BUY_SELL_PLATE
{
	YESTERDAY_SETTLEMENT = 1,// 昨结
	YESTERDAY_CLOSE			 // 昨收
};

enum E_PromptType
{
	EPT_MessageBox = 0,	// 对话框提示 xx版本，请联系xxx
	EPT_IE,				// 调用浏览器提示

	EPT_End
};

typedef  std::vector<CNCButton> buttonContainerType;
#define  FOR_EACH(container, it) for(it = container.begin(); it != container.end(); ++it)
#define  FOR_EACH_REVERST(container, it) for(it = container.rbegin(); it != container.rend(); ++it)
class TiXmlElement;

///////////////////////////////////////////////////////////////////////////////////
//
class CButtonConfig
{
public:
	virtual bool32	LoadXml(TiXmlElement* p);
	TCHAR		m_StrName[MAX_PATH];			// 按钮显示的名字
	TCHAR		m_StrPathPic[MAX_PATH];			// 按钮图片的路径
	TCHAR		m_StrPathPic2[MAX_PATH];		// 按钮图片的路径 风格2

	TCHAR		m_StrPathPic3[MAX_PATH];		// 按钮图片的路径 风格3

	CButtonConfig &operator = (CButtonConfig &bf)
	{
		memcpy(m_StrName,bf.m_StrName,sizeof(m_StrName));
		memcpy(m_StrPathPic,bf.m_StrPathPic,sizeof(m_StrPathPic));
		memcpy(m_StrPathPic2,bf.m_StrPathPic2,sizeof(m_StrPathPic2));
		memcpy(m_StrPathPic3,bf.m_StrPathPic3,sizeof(m_StrPathPic3));
		return *this;
	}

	CButtonConfig()
	{
		memset(m_StrName,0,sizeof(m_StrName));
		memset(m_StrPathPic,0,sizeof(m_StrPathPic));
		memset(m_StrPathPic2,0,sizeof(m_StrPathPic2));
		memset(m_StrPathPic3,0,sizeof(m_StrPathPic3));
	}
};
typedef CArray<CButtonConfig, CButtonConfig> arrButtonConfig;
///////////////////////////////////////////////////////////////////////////////////

// 用户详细页面
enum E_UserInfoShowType
{
	EType_Normal = 0,     // 点金手标准方式
	EType_Cfm,            // 版面内置网页
	EType_Dialog,         // 对话框弹窗网页
	EType_Hide,			  // 不显示
};

// 软件版本
enum E_VersionType
{
	EVT_None = 0,
	EVT_Free,             // 免费版
	EVT_Standard,         // 标准版
	EVT_Featured,         // 精选版
	EVT_Advanced,		  // 高级版
		
};

class CUserInfoCfm
{
public:
	CString m_StrUrl;				// 用户详细信息网页地址
	E_UserInfoShowType m_eShowType;	// 网址显示类型

	CUserInfoCfm()
	{
		m_StrUrl = _T("");
		m_eShowType = EType_Normal;
	}
};

struct T_NewCenterTabInfo
{
	int32 iMenuID;
	int32 iTreeID;

	T_NewCenterTabInfo()
	{
		iMenuID = 0;
		iTreeID = 0;
	}
};

struct TUserDealRecord 
{
	bool32 bRecord;		// 是否记录
	int32 iType;		// 记录类型(1只记录功能, 2只记录商品切换, 0记录所有)

	TUserDealRecord()
	{
		bRecord = true;
		iType = 0;
	}
};

// 交易下载信息
typedef struct _TradeCfgParam
{
	CString          StrBrokerName;	    // 券商名称
	CString          StrAppName;		// 交易软件名称
	CString          StrShortcut;		// 券商名称缩写
	int32            iType;			    // 交易软件类型
	bool32           bDefaultTrade;     // 是否默认交易软件
	CString          StrRun;			// 可执行程序名称
	CString          StrUrl;			// 下载目录
	CString          StrLogo;			// logo
	bool32           bNewAdd;           // 新下载的 ？
	bool32           bDldComplete;      // 是否完整下载

	_TradeCfgParam()
	{
		iType         = 1;
		bDefaultTrade = false;
		bNewAdd       = false;
		bDldComplete  = false;
	}
}T_TradeCfgParam;

//导航栏行情菜单
struct T_QuoteMarketItem
{
	CString     StrMarketName;         // 市场名称
	CString     StrMarketCfmName;	   // 版面名称  
};

class CShowBuySell
{
public:
	CShowBuySell(){m_showBuySellId = 0; m_showItem = 5;}
	int m_showBuySellId;
	int m_showItem;
};

//网页按钮
class CButtonWebConfig : public CButtonConfig
{
public:
	virtual bool32	LoadXml(TiXmlElement* p);
	CString			m_strUrl;		// url
	CString			m_StrWspName;		// 工作区的名字	
};
typedef CArray<CButtonWebConfig, CButtonWebConfig> arrWebConfig;
///////////////////////////////////////////////////////////////////////////////////

// 行情按钮
class CButtonQuoteConfig : public CButtonConfig
{
public:
	virtual bool32	LoadXml(TiXmlElement* p);
	int32			m_iMarketID;		// 市场ID	
};
typedef CArray<CButtonQuoteConfig, CButtonQuoteConfig> arrQuoteConfig;
///////////////////////////////////////////////////////////////////////////////////

// 工作区按钮
class CButtonWspConfig : public CButtonConfig
{
public:
	virtual bool32	LoadXml(TiXmlElement* p);
	TCHAR		m_StrWspName[MAX_PATH];		// 工作区的名字	

	CButtonWspConfig &operator = (CButtonWspConfig &bf)
	{
		memcpy(m_StrName,bf.m_StrName,sizeof(m_StrName));
		memcpy(m_StrPathPic,bf.m_StrPathPic,sizeof(m_StrPathPic));
		memcpy(m_StrPathPic2,bf.m_StrPathPic2,sizeof(m_StrPathPic2));
		memcpy(m_StrPathPic3,bf.m_StrPathPic3,sizeof(m_StrPathPic3));
		memcpy(m_StrWspName,bf.m_StrWspName,sizeof(m_StrWspName));
		return *this;
	}

	CButtonWspConfig()
	{
		memset(m_StrWspName,0,sizeof(m_StrWspName));
	}
};

typedef CArray<CButtonWspConfig, CButtonWspConfig> arrWspConfig;
///////////////////////////////////////////////////////////////////////////////////

// 特色指标按钮
class CButtonFormularConfig : public CButtonConfig
{
public:
	virtual bool32	LoadXml(TiXmlElement* p);
	CString		m_StrWspName;		// 工作区的名字	
	CString		m_StrHelpImg;		// 指标帮助说明的图片
};
typedef CArray<CButtonFormularConfig, CButtonFormularConfig> arrFormularConfig;
///////////////////////////////////////////////////////////////////////////////////

//
class CWndSize
{
public:
	CString		m_strID;		// 对话框标识ID
	CString		m_strTitle;		// 对话框标题栏显示的标题
	int32		m_iWidth;		// 对话框的宽度
	int32		m_iHeight;		// 对话框的高度
};
typedef CArray<CWndSize, CWndSize> arrWndSizeConfig;
//////////////////////////////////////////////////////////////////////////////////

class CMenuConfig
{
public:
	virtual bool32	LoadXml(TiXmlElement* p);
	CString      m_StrName;		// Name
	CString      m_StrUrl;      // Url
};
typedef CArray<CMenuConfig, CMenuConfig> arrMenuConfig;
//////////////////////////////////////////////////////////////////////////////////




//指标组文件
struct T_IndexGroupItem
{
	T_IndexGroupItem()
	{
		iRegion= INVALID_VALUE;
		strIndexName = _T("");
		bRegionMainIndex = false;
	}
	int         iRegion;			// 指标区域
	CString     strIndexName;		// 指标名称 
	bool32		bRegionMainIndex;	// 是否为主指标
};

/////////////////////////////////////////////////////////
// 指标文件
////////////////////////////////////////////////////////
class CIndexGroupItem
{
public:
	CIndexGroupItem()
	{
		m_strGroupIndexsName= _T("");
		m_vGroupIndex.clear();
	}
public:
	CString m_strGroupIndexsName;
	std::vector<T_IndexGroupItem> m_vGroupIndex;
};



typedef enum E_AdvsUrlType
{
	EAUTLink,              //广告链接
	EAUTPit				   //广告图片
} E_AdvsUrlType;

class CConfigInfo
{
public:
	enum E_Flag
	{
		FlagDefault,			// 默认标志 - 全能板
		FlagZhanLueJia,			// 战略家
		FlagDaJiangJun,			// 股大将军
		FlagBoHaiQiHuo,			// 渤海期货
		FlagCaiFuJinDunGuPiao,		// 财富金盾股票
		FlagCaiFuJinDunQiHuo,		// 财富金盾期货
		FlagCaiFuJinDunFree,		// 财富金盾免费版

		FlagCount
	};

	static	CConfigInfo *Instance();
	static  void	DelInstance();			// 销毁资源

	CString		GetVersionName() const { return m_StrVerName; };
	CString		GetVersionNo() const	{ return m_StrVerNo; };
	DWORD		GetVersionFlag() const	{ return m_dwFlag; };
	CString		GetModule() const	{ return m_StrMdl; };
	CString		GetAuthKey() const { return m_StrAuthKey; };
	CString		GetPrompt()  const	{ return m_StrPrompt; }
	
	CString		GetOrgKey() const { return m_StrOrgKey; }
	CString     GetNewsCenterOrgCode() const { return m_StrNewsCenterOrgCode; }
	CString		GetCodePlatForm() const { return m_StrPlatCode; }

	// 获取指标组数据
	void GetIndexGroupItem(CString strIndexName, vector<T_IndexGroupItem>& varrIndexGroup);

	// 从配置文件
	bool32		LoadSysConfig();
	void		SaveSysConfig();

	// 工具栏相关
	bool32		LoadToolBarConfig();

	//菜单栏相关配置
	bool32      LoadCaptionConfig();

	// 指标组配置文件
	bool32 LoadIndexGroupConfig();

    //工具栏配置相关
    bool32 LoadChlDataFromXml( T_ChildButtonData &chlData, TiXmlElement *pElemet );
    bool32 LoadFromXml( const char* strXmlPath );


protected:
	CConfigInfo();
	~CConfigInfo();
private:
	void		ResetToDefault();
	bool32		ReadTiEleAttri(TiXmlElement *pEle, const char *pszAttriName, OUT CString &StrValue);

	typedef map<DWORD, CString>		DefaultNameMap;

	CString		m_StrVerName;
	CString		m_StrVerNo;
	CString		m_StrMdl;			// 模块
	CString		m_StrAuthKey;		// 认证key
	CString		m_StrOrgKey;		// 机构数据id
	CString     m_StrNewsCenterOrgCode;    // F10用到的机构编码
	CString		m_StrPlatCode;		// 资讯中心code
	CString		m_StrPrompt;
	DWORD		m_dwFlag;

	static  CConfigInfo *m_spThis;
	static	DefaultNameMap m_smapDefNames;
	static	DefaultNameMap m_smapDefKeys;	// 默认认证key

	map<CString, CIndexGroupItem> m_mapIndexGroup;		// 特殊指标汇总

public:
	std::vector<CShowBuySell*> m_vShowBuySell;	// 指定指标显示买卖5档
	std::vector<T_NewCenterTabInfo> m_vNewsCenterTreeID;
	bool32	m_bShowNewvol;	// 是否显示现手（只支持市商）

	// 配置文件中值一些字段
	CString			 m_StrAppNameConfig;	// 配置文件中指定的程序名
	bool32			 m_bPromptNet;			// 是否提示网络不畅通
	CString			 m_StrAboutStr1;		// 关于对话框的配置文字
	CString			 m_StrAboutStr2;		// 关于对话框的配置文字
	CString          m_StrLoginTitleText;   // 登录标题文字
	CString			 m_StrLoginErrorTip;	// 登录错误提示
	
	// 关于k线的广告显示
	CString          m_strSpecialJiPing;
	bool32           m_bIsVip;
	CString          m_strKAdveriseUrl;

	// 按钮广告
	CString          m_strAdvBtnUrl;
	bool32           m_bAdvertiseExist;
	int				 m_iReadIndex;

	//帮助菜单
	arrMenuConfig    m_aHelpMenus;   

	//IM开关
	bool32           m_bImVersion;

	//
	bool32			 m_bRegOn;
	CString			 m_StrRegUrl;
	CString			 m_StrBtName;

    bool32			 m_bZHMMOn;
    CString			 m_StrZHMMUrl;
    CString			 m_StrZHMMBtName;

	CString          m_StrWelcomeAdvsCode;	// 欢迎广告编码
	CString          m_StrTopRightAdvsCode;	// 右上角广告编码
	CString          m_StrAboutUsAdvsCode;	// 关于我们广告编码
	CString			 m_StrLoginLeftCode;    // 登录界面左侧广告编码

	CString          m_StrSSJPTabId;		// 实时解盘标签页Id
	CString			 m_StrJPZXTabId;		// 精品资讯标签页Id

	int32			 m_iTradeMarketID;
	CString			 m_StrResponsbility;
	bool32			 m_bQuickTrade;			// 是否显示闪电交易
    bool32           m_bXmlFileUpdate;      // 是否更新工具栏XML文件

	CString			 m_StrPicUrl;			// 登录界面，宣传页的url
	CString          m_StrQQRegUrl;			// 登录界面，第三方QQ登录url
	CString          m_StrWeChatUrl;		// 登录界面，第三方微信登录url
	CString			 m_StrBindUserUrl;		// 完善资料页面
	bool32			 m_bShowThirdLogin;		// 是否显示三方登录

	UINT			 m_nSelectNewsID;		// 用以区分精品资讯与实时解盘

	CString			m_StrDefUser;
	CString			m_StrDefPwd;

	bool32			m_bMember;				// 当前版本的类型 true，会员；false，交易所
    bool32          m_firstLogin;			// 是不是第一次登录

	E_VersionType   m_eVersion;             // 版本号

	CString			m_strBuyPrice;			// 做市商报价表和买卖盘中的买价字段
	CString			m_strSellPrice;	
	CString			m_strNewPrice;	
	CString			m_strBuy;				// 做市商买卖盘中的买价字段
	CString			m_strSell;	

	BUY_SELL_PLATE	m_nBuySellPlate;		// 买卖盘类型（1-昨结  2-昨收）

	CString			m_StrWelcomeUrl;		// 欢迎页网址
	CString			m_strCheckCode;			// 指标校验码
	CString			m_StrToolBarVer;		// 工具栏文件的版本号

	TUserDealRecord	m_stUserDealRecord;		// 用户行为记录配置

	arrWebConfig		m_aThirdPartyLoginButtons;	// 登录界面QQ、微信按钮配置
	CUserInfoCfm		m_btnUserInfo;				// 用户详细页面
	arrWndSizeConfig	m_aWndSize;				    // 一些经常改变的窗口的大小

	 int				m_iRihgBarHighValue;
public:
    T_ToolBarData m_toolBarData;
    std::vector<T_ColumnData> m_vecColumnData;
    int m_CurrentBtnId;
    int m_CurrentColId;

	std::vector<T_QuoteMarketItem> m_vecQuoteMarket;

public:
    void ResetCurrentBtnID() { m_CurrentBtnId = 6000;}
    void IncreaseCurrentBtnID() { m_CurrentBtnId++; }
    int  GetCurrentBtnID() { return m_CurrentBtnId; }

    void ResetColCurrentID() { m_CurrentColId = 5000;}
    void IncreaseColCurrentID() { m_CurrentColId++; }
    int  GetColCurrentID() { return m_CurrentColId; }

	//获取广告相关配置
	CString GetAdvsCenterUrlByCode(CString& strAdCode, E_AdvsUrlType eAdvsUrlType = EAUTPit);

private:
	void ReadNewsCenterTreeIDConfig( TiXmlElement* pTreeIDElement );

	// 释放工具栏Image资源
	void ReleaseImage();
};


#endif //_CONFIG_INFO_H_