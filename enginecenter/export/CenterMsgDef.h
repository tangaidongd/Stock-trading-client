#ifndef _CENTER_MSGDEF_H_
#define _CENTER_MSGDEF_H_

#define WM_CENTER_MSGBEGIN WM_USER+0x200	

enum E_CenterMsgDef
{
	// 快捷键处理消息
	UM_HOTKEY = WM_CENTER_MSGBEGIN,

	// MainFrame
	UM_MainFrame_OnStatusBarDataUpdate,				// 状态栏数据更新
	UM_MainFrame_MaxSize,							// 最大化
	UM_MainFrame_KickOut,
	UM_MainFrame_ManualOptimize,					// 手动优选

	// 表头编辑
	UM_GridHeader_Change,							// 表头发生变化

	// 风格设置
	UM_Font_Change,									// 字体发生变化
	UM_Color_Change,								// 颜色发生变化

	// ViewData模块自定义消息
	UM_ViewData_OnTimerFreeMemory,					// 定时清理内存
	UM_ViewData_OnTimerSyncPushing,					// 定时同步推送列表
	UM_ViewData_OnTimerInitializeMarket,			// 初始化市场

	UM_ViewData_OnDataServiceConnected,				// 数据返回
	UM_ViewData_OnDataServiceDisconnected,			// 
	UM_ViewData_OnDataRequestTimeOut,				// 	
	UM_ViewData_OnDataCommResponse,					// 	
	UM_ViewData_OnDataServerConnected,				// 
	UM_ViewData_OnDataServerDisconnected,			// 
	UM_ViewData_OnDataServerLongTimeNoRecvData,
	UM_ViewData_OnKLineNodeUpdate,					//
	UM_ViewData_OnTimesaleUpdate,					//
	UM_ViewData_OnPublicFileUpdate,					//
	UM_ViewData_OnRealtimePriceUpdate,				// 
	UM_ViewData_OnRealtimeLevel2Update,				// 
	UM_ViewData_OnRealtimeTickUpdate,				// 
	UM_ViewData_OnNewsListUpdate,					// 
	UM_ViewData_OnF10Update,						// 
	UM_ViewData_OnLandMineUpdate,					// 

	UM_ViewData_OnGeneralNormalUpdate,				//
	UM_ViewData_OnGeneralFinanaceUpdate,			//
	UM_ViewData_OnMerchTrendIndexUpdate,			//
	UM_ViewData_OnMerchAuctionUpdate,				//
	UM_ViewData_OnMerchMinuteBSUpdate,				//
	UM_ViewData_OnClientTradeTimeUpdate,			//

	UM_ViewData_OnAuthSuccess,						// 认证相关
	UM_ViewData_OnAuthFail,
	UM_ViewData_OnMessage,		
	UM_ViewData_OnAllMarketInitializeSuccess,		// 初始化成功

	UM_ViewData_OnAuthSuccessTransToMain,			// 认证成功后把处理转到主线程
	UM_ViewData_OnConfigSuccessTransToMain,			// 配置信息读取成功后转到主线程

	UM_RecentMerch_Update,							// 最近商品列表更新

	UM_Dlghotkey_Close,								// 键盘精灵关闭了
	UM_Download_Offlinedata_Progress,				// 脱机数据的进度条

	UM_IoViewTitle_Button_LButtonDown,				// 业务视图标题按钮左键按下
	UM_IoViewTitle_Button_RButtonDown,				// 业务视图标题按钮右键按下
	//

	UM_ViewData_OnNewsResponse,						// 资讯返回	
	UM_ViewData_OnNewsServerConnected,				// 资讯服务器连上
	UM_ViewData_OnNewsServerDisConnected,			// 资讯服务器断线

	UM_ViewData_OnBlockFileTraslateTimeout,			// 板块文件同步超时

	UM_SettingChanged,								// 全局配置发生了变更
	UM_ViewData_OnNewsTitleUpdate,                 //财富资讯2013
	UM_ViewData_OnNewsContentUpdate,
	UM_ViewData_OnCodeIndexUpdate,
	UM_ViewData_OnNewsPushTitle,

	//add by hx
	UM_Package_TimeOut,
	UM_Package_LongTime,
	UM_Right_ShowTip,
	UM_User_Deal_Record,
	UM_ViewData_OnAuthPlugInHeart,					// 插件心跳

	UM_HasIndexRight,								// 是否有该指标权限


	//
	UM_JumpToTradingSoftware,						// 量化策略交易页面

	//
	UM_FIRM_TRADE_SHOW = 3000,						// 实盘交易

	// 模拟交易相关
	UM_SIMULATE_TRADE_SHOW,							// 模拟交易
	UM_SIMULATE_TRADE_OPENACCOUNT,					// 实盘开户
	UM_SIMULATE_TRADE_HQREG,						// 行情注册
	UM_SIMULATE_TRADE_HQLOGIN,						// 行情登录
	UM_SIMULATE_TRADE_IMGURL,						// 点击图片
    UM_HQ_REGISTER_SUCCED,                          // 行情注册成功
	UM_CEF_AFTERCREATED,							// cef创建成功

	// 
	UM_ThirdLoginRsp,								// 第三方登录
	UM_ReLoginPC,									// PC重新登录
	UM_PersonCenterRsp,								// 个人中心页面
	UM_NativeOpenCfm,								// 打开版面
	UM_LinkageWebTrend,								// 打开分时
	UM_CLOSE_DLG,									// 关闭对话框
	UM_OpenOutWeb,									// 点击网页里的链接在外面浏览器打开
	UM_ShowWebDlg,									// 点击网页里的链接弹出对话框
	UM_CloseOnlineServDlg,							// 关闭在线客服对话框

    //
    UM_News_Push_Message,                           // 推送资讯消息

	UM_InfoCenter,									// 资讯中心

	UM_MainFrame_ChooseStock_Resp,                  // 选股回复
	UM_MainFrame_DapanState_Resp,                   // 大盘状态
	UM_MainFrame_ChooseStockStatus_Resp,            // 选股状态

	//报价表排序相关
	UM_IoViewReport_SortByRisePercent,              // 涨幅排名
	UM_IoViewReport_Finance	,						// 财务数据

	UM_WEB_KEY_DOWN,                                // 网页非编辑区点击
	UM_IndexChsMideCore_KLineUpdate_Resp,			// 通知智能选股进度条变化
	UM_DownloadTrade,								// 下载交易程序
	UM_ToolbarCommand,								// 工具栏相关消息处理

	// 智能选股相关
	UM_PickModelTypeInfo,							// 智能选股类型的选股列表
	UM_PickModelTypeStatus,							// 智能选股类型选股状态，是否有选股
	UM_PickModelTypeEvent,							// 只能选股事件
	
	UM_MainFrame_NewStock_Resp,						// 新股信息

	//	bridge message
	UM_BridgeBegin = 4000,
	UM_BridgeEnd ,
	UM_BridgeGetNetBreedFailed,			
};

#endif