#pragma once


typedef struct T_TBWndFont 
{
public:
	CString	m_StrName;
	float   m_Size;
	int32	m_iStyle;

}T_NcFont;

//	find_if 中的函数对象类
class vector_finder
{		
public:
	vector_finder(int id) 
	{
		m_nID = id;
	}
	~vector_finder()
	{

	}
	bool operator()(vector<CNCButton>::value_type& v)
	{
		return v.GetControlId() == m_nID;
	}
private:
	int		m_nID;
};

typedef struct T_FixedBtnHovering 
{
public:
	bool32  bLeft;
	int32   iButtonId;

	T_FixedBtnHovering()
	{
		bLeft     = true;
		iButtonId = -1;
	}

}T_FixedBtnHovering;

typedef enum
{
	E_CONTROL_MIN,
	E_CONTROL_NONE,
	E_CONTROL_LEFT,
	E_CONTROL_RIGHT,
	E_CONTROL_BOTH,
	E_CONTROL_MAX
}ECButtonType;

enum eNewsType
{
	TB_WSP_BTN_START,

	TB_IOCMD_REPORT,
	TB_IOCMD_KLINE,
	TB_IOCMD_TREND,
	TB_IOCMD_NEWS,
	//TB_IOCMD_F10,
	TB_IOCMD_REPORTARB,
	TB_IOCMD_KLINEARB,
	TB_IOCMD_TRENDARB,
};

enum eEventType
{
	EType_Market_Change = 0,                //市场切换
	EType_Open_Cfm,                         //版面切换
	EType_Inner_WebPage,                    //内置网页
	EType_Dialog_WebPage,                   //弹窗网页
	EType_Special_Function,                 //固定功能处理，如首页
	EType_Index_Change,                     //指标切换
	EType_Self_Defination,                  //自定义

	EType_Category = 100,                   //标签类型，如”行情”
	EType_Advertising,						//广告
};


enum eFuncType
{
	FType_Home_Page,                 //返回首页
	FType_Back,                      //返回上一步
	FType_Full_Screen,               //全屏切换
	FType_Self_Select_Stock,         //自选股
	FType_Internal_Real_Trade,       //内置实盘交易 
	FType_UserDefine_Real_Trade,     //外挂实盘交易
	FType_About_Us,                  //关于我们
	FType_About_Arbitrage,           //套利
	FType_About_Alarm,               //预警
};



// 首页
#define ID_HOME_QUOTE_MORE		2051
#define ID_HOME_INFO_GJS		2052
#define ID_HOME_INFO_JP			2053
#define ID_HOME_INFO_CJRL		2054

#define ID_HOME_INDEX_BEGIN		2055
#define ID_HOME_INDEX_END		2100
#define ID_HOME_INDEX_MORE		2101

#define ID_HOME_WORKSPACE_BEGIN		2102
#define ID_HOME_WORKSPACE_END		2150
#define ID_HOME_WORKSPACE_MORE		2151

#define ID_HOME_TRADE_INTERNAL		2152
#define ID_HOME_TRADE_PLUGIN		2153

#define ID_HOME_FUNC_FULL		2154
#define ID_HOME_FUNC_ARBITRAGE		2155
#define ID_HOME_FUNC_WARNING		2156

#define ID_HOME_HOME			2157
#define ID_HOME_LIVE			2158

#define ID_OTHER_BEGIN			2160
#define ID_OTHER_END			2170

#define BTN_ID_QUOTE_HSAG       5001
#define BTN_ID_QUOTE_XGGS       5002
#define BTN_ID_QUOTE_QHSC       5003
#define BTN_ID_QUOTE_QQZS       5004
#define BTN_ID_QUOTE_WHSC       5005

// 行情页
#define BTN_ID_QUOTE_BASE		20000
#define BTN_ID_QUOTE_END		21000

#define BTN_ID_QUOTE_ZXG		BTN_ID_QUOTE_BASE - 1
#define BTN_ID_QUOTE_ALL		BTN_ID_QUOTE_BASE - 2

#define BTN_ID_QUOTE_GJS_MORE		BTN_ID_QUOTE_BASE - 3
#define BTN_ID_QUOTE_QH_MORE		BTN_ID_QUOTE_BASE - 4
#define BTN_ID_QUOTE_WH_MORE		BTN_ID_QUOTE_BASE - 5
#define BTN_ID_QUOTE_ZS_MORE		BTN_ID_QUOTE_BASE - 6
#define BTN_ID_QUOTE_ZQ_MORE		BTN_ID_QUOTE_BASE - 7
#define BTN_ID_QUOTE_ZXG_MORE		BTN_ID_QUOTE_BASE - 8
#define BTN_ID_QUOTE_ALL_MORE		BTN_ID_QUOTE_BASE - 9

#define BTN_ID_QUOTE_ALL_MENU		BTN_ID_QUOTE_BASE - 10

// 分析页
#define ID_ANALY_BEGIN			2200
#define ID_ANALY_END			2250

// 功能页
//#define ID_FUNCTION_ANALYSTS		2251
#define ID_FUNCTION_FORMULA_MAG		2252
#define ID_FUNCTION_CONDITIONALARM	2253
#define ID_FUNCTION_WND_SEG			2254
#define ID_FUNCTION_SYNCEXCEL		2255
#define ID_FUNCTION_SEL_MERCH		2256

// 系统设置
#define ID_SYS_ABOUT				2260
#define ID_SYS_UPDATE				2261
#define ID_SYS_AUTO_LOGIN			2262
#define ID_SYS_DISCONNECT_SERVER	2263
#define ID_SYS_DISCONNECT_NEWS		2264
#define ID_SYS_RESPONSIBILITY		2265
#define ID_SYS_GRIDHEAD_SET			2266
#define ID_SYS_CONNECT_SET			2267
#define ID_SYS_TRADE_SET			2268
#define ID_SYS_WORKSPACE_SET		2269
#define ID_SYS_SHOW_JP				2270
#define ID_SYS_SHOW_STATUS			2271

