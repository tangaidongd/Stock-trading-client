// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__234DC6B0_FD03_4692_B156_E8947A1EB907__INCLUDED_)
#define AFX_MAINFRM_H__234DC6B0_FD03_4692_B156_E8947A1EB907__INCLUDED_
 
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MPIChildFrame.h"
#include "StatusBarEx.h"
#include "CommentStatusBar.h"
#include "IoViewReport.h"
#include "IoViewTrend.h"
#include "IoViewNews.h"
#include "mdiclientex.h"
#include "hotkey.h"

#include "DlgIm.h"
#include "BlockManager.h"
#include "OptimizeServer.h"
//
#include "DlgAdvertise.h"
#include "TradeContainerWnd.h"
#include "NewTBWnd.h"
#include "TBCaptionWnd.h"
#include "DlgJPNews.h"
#include "NewToolBar.h"
#include "DlgWebF10.h"
#include "DlgAccount.h"
#include "DlgWebContent.h"
#include "DlgDefaultUserRegister.h"
#include "DlgPushMessage.h"
#include "MenuBar.h"
#include "DlgNewComment.h"
#include "LeftToolBar.h"
#include "ShortCutMenuToolBar.h"
#include "../../../JsonCpp/json.h"
#include "client_AnalysisWeb.h"

#define NEWWINDOWOFFSETX			24
#define NEWWINDOWOFFSETY			40
#define NEWWINDOWWIDTH				800
#define NEWWINDOWHEIGHT				600

#define USERBLOCKMAXNUMS			10
#define RECENTMERCHMAXNUMS			10

#define RAND_NUMS					500						// 随机数的序列个数

class CDlgBelongBlock;
class CDlgTrace;
class CAdjustViewLayoutDlg;
class CKeyBoardDlg;
class CDlgSpirite;
class CDlgNetWork;
class CDlgSysUpdate;
class CDlgBrokerWatch;
class CDlgAlarm;
class CDlgRecentMerch;
class CDlgIndexPrompt;
class CDlgWait;
class CDlgIm;  // 2013-10-28 add by cym 
class CDlgIndexChooseStock;
class CDlgLoginNotify;
class CDlgNetFlow;
class CHotKey;
class CIoViewBase;
class CGGTongView;
class CKLine;  
// class CDlgF10;
class CRecordDataCenter;

struct T_PhaseOpenBlockParam;
class CArbitrage;

class CMyControlBar;
class CMyContainerBar;
class CDlgTrade;

class CTradeContainerWnd;

enum E_CONN_NOTIFY
{
	en_connect_error,	// 连接出错
	en_connect_success, // 连接成功
};

// 资讯类型（即资讯的消息类型）
enum E_Info_Type
{
	en_monitor_msg=1,	// 预警消息
	en_system_msg,		// 系统消息
	en_optional_change,	// 自选异动
	en_strategy_trade,	// 策略交易
	en_select_stock,    // 智能选股
};

// 消息中心的协议类型
enum E_Fun_Type
{
	en_heartbeat = 1,			// 心跳
	en_set_device_id,			// 设置设备号
	en_query_info_type_list,	// 查询资讯类型列表
	en_query_msg_list,			// 查询消息列表
	en_query_info_content,		// 查询资讯内容
	en_info_push,				// 推送资讯
	en_add_info					// 添加资讯
};

//////////////////////////////////////////////////////////////////////
// 点击网页里的链接弹出对话框
typedef struct T_ShowWebDlg
{	
	T_ShowWebDlg()
	{
		StrUrl   = L"";
		StrWidth = L"0";
		StrHeight= L"0";
	}

	CString		   StrUrl;		// url
	CString	       StrWidth;	// 宽
	CString		   StrHeight;	// 高
}T_ShowWebDlg;

// 网页要打开指定版面的参数
typedef struct T_WebOpenCfm
{	
	T_WebOpenCfm()
	{
		StrCfmName = L"";
		iID = -1;
		StrFlag= L"";
	}

	CString		   StrCfmName;	// 版面名称
	int32	       iID;			// id
	CString		   StrFlag;		// 标志
}T_WebOpenCfm;


// 请求行业数据结构
typedef struct T_IndustryData
{	
	T_IndustryData()
	{
		iMarketId = -1;
		strCode = "";
		strCnName= "";
	}

	int32		   iMarketId;	// 版面名称
	string	       strCode;			// id
	string		   strCnName;		// 标志
	int32		   iMarketVlue;		// 个股市值
}T_IndustryData;



// 网页打开分时页面参数
typedef struct T_WebOpenTrend
{	
	T_WebOpenTrend()
	{
		iMarketId = -1;
		strCode  = L"";
		strCnName = L"";	
	}

	int32	        iMarketId;			// 市场代码
	CString			strCode;			// 商品代码
	CString			strCnName;			// 商品名称
}T_WebOpenTrend;


typedef struct T_BlockMenuInfo
{	
    CString		   StrBlockMenuNames;	
	int32	       iBlockMenuIDs;	
}T_BlockMenuInfo;

typedef struct T_NewsContentInfo
{
public:

	int32		m_iIndexID;			// 资讯id
	CString		m_StrTitle;			// 标题标题
	CString		m_StrContent;		// 标题内容
	CString		m_StrTimeUpdate;	// 更新时间

	CefWindowInfo* m_pWindowInfo;

	T_NewsContentInfo()
	{

		m_iIndexID   = -1;

		m_StrTitle   = L"";
		m_StrContent = L"";
		m_StrTimeUpdate= L"";

		m_pWindowInfo = NULL;
	}

}T_NewsContentInfo;
/////////////////////////////////////////////////////////////////////
struct T_RecentMerch
{
public:
	T_RecentMerch() { m_pMerch = NULL; }

public:
	CMerch			*m_pMerch;
};

///////////////////////////////////////////////////////////////////////
struct T_CompareMerchInfo
{	
public:
	T_CompareMerchInfo()
	{
		m_iGroupId	= 0;
		m_pMerch	= NULL;
	}

public:
	int32			m_iGroupId;
	CMerch			*m_pMerch;
};

struct T_MenuItemData
{
	int32 ID;
	CString strText;
	bool bShowArrow;
	Image* pImgData;

	T_MenuItemData()
	{
		ID = -10000;
		strText = L"";
		bShowArrow = false;
		pImgData = NULL;
	}

	T_MenuItemData(int32 id, CString Tex, bool bShow, Image* pImg)
	{
		ID = id;
		strText = Tex;
		bShowArrow = bShow;
		pImgData = pImg;
	}
};

typedef vector<T_MenuItemData> VecMenuContainer;
//////////////////////////////////////////////////////////////////////

///////////////////////智能选股结构体
struct T_SmartStockPickItem
{
	CString m_strModeName;
	int32   m_iModeId;
	CString m_strModeDetail;
	CString m_strmodeCode;
	int  m_ihasSmartStock;

	T_SmartStockPickItem()
	{
		m_strModeName = L"";
		m_iModeId  = -1;
		m_strmodeCode = L"";
		m_ihasSmartStock = 0;
		m_strModeDetail    = L"";
	}
	T_SmartStockPickItem(int32 iID, CString strModeName, CString strModeDetail=L"", CString strmodeCode = L"", int32 ihasSmartStock =0)
	{
		m_strModeName = strModeName;
		m_iModeId  = iID;
		m_strmodeCode = strmodeCode;
		m_ihasSmartStock = ihasSmartStock;	
		m_strModeDetail    = strModeDetail;
	}
};


const int32 KMainFrameDefaultLeft	= 150;
const int32 KMainFrameDefaultRight	= 950;
const int32 KMainFrameDefaultTop	= 50;
const int32 KMainFrameDefaultBottom	= 650;

extern WORD g_awToolBarIconIDs[];

enum E_UserConnectCmdFlag
{
	EUCCF_ConnectDataServer = 1,		// 行情服务器连接允许, 没设置则禁止
	EUCCF_ConnectNewsServer = 2,		// 资讯服务器连接允许，没设置则禁止
};
 
class CMainFrame : public CNewMDIFrameWnd, public CObserverUserBlock, public COptimizeServerNotify
{	
friend class CStatusBarEx;

friend class CCommentStatusBar;

DECLARE_DYNAMIC(CMainFrame)
public:	
	enum E_CurrentShowState
	{
		ECSSNormal = 0,
			
		ECSSForcusMax,
		ECSSForcusMin,
		ECSSForcusClose,
		
		ECSSPressMax,
		ECSSPressMin,
		ECSSPressClose,
		//
		ECSSCount
			
	};

	enum E_RenameType
	{
		ERTXML = 1,	// 未登录
		ERTWSP,
		ERTCFM,
		ERTVMG,
		ERTCOUNTS,
	};

	enum E_CpsAlgorithmType // 操盘手算法类型
	{
		ECATNONE = 0,		
		ECATSXBD,			// 时序波段算法
		ECATCOUNTS
	};

	enum E_MenuItemType
	{

		EMIT_NONE = 0,

		// 一级菜单类型
		EMIT_FIR_SYSTEM = 1,
		EMIT_FIR_SEL_STOCK,
		EMIT_FIR_TRADE,
		EMIT_FIR_HELP,

		// 二级菜单类型
		EMIT_SEC_MODULESET = 10,
	};

public:
	CMainFrame();
	virtual ~CMainFrame();

#ifdef _DEBUG
	virtual void	AssertValid() const;
	virtual void	Dump(CDumpContext& dc) const;
#endif

public:
	//	初始化登录后的相关动作
	void			Init();
	//	反初始化动作
	void			Uninit();
	//	延时启动登录
	void			ElapseLogin(UINT uSecond);
	//	启动登录
	void			Login(bool32 bCloseExit, bool32 bExitQuery);
	// 自选板块相关
	// from CObserverUserBlock
	
	//
	virtual void	OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType);
	
	//
	void			AppendUserBlockMenu(CNewMenu * pMenu, bool32 bAddToSave = true);

public:
	// 重连服务器的时候, 测速相关	
	// from COptimizeServerNotify

	//
	virtual void	OnFinishSortServer();

	void	SetUserConnectCmdFlag(DWORD dwFlagRemove, DWORD dwFlagAdd, DWORD dwMask = 0xffffffff);		// 设置用户设置的服务器连接状态标志
	DWORD   GetUserConnectCmdFlag() const { return m_dwUserConnectCmdFlag; }

public: 
	static CString	GetDefaultXML();
	bool32			FromXml(const char * pKcXml );
	CString			ToXml(const char * KpcFileName, CString StrFileTitle = L"", CString StrVersion = L"");
	static CString	GetWspFileShowName(const char* KpcFilePath);
	static CString	GetWspFileVersion(const char* KpcFilePath);
	static bool32   IsWspReadOnly(const char *KpcFilePath);
	static bool32   SetWspFileReadOnly(const char *KpcFilePath, bool32 bReadOnly);

public:	
	void			ProcessHotkey ( MSG* pMsg );

	void			SetHotkeyTarget ( CWnd* pParent = NULL, E_HotKeyType eHKT = EHKTCount);		// EHKTCount为所有类别
	void			ShowHotkeyDlg(MSG *pMsg = NULL, CWnd *pParent = NULL, E_HotKeyType eHKT = EHKTCount);	// 直接显示hotkeydlg
	
	void			SetForceFixHotkeyTarget(bool32 bForceFix) { m_bForceFixKeyBoard = bForceFix; }
	bool32			GetForceFixHotkeyTarget()				  { return m_bForceFixKeyBoard; }
	
	CString			ReNameFile(CString StrOldName,E_RenameType eReNameType);   
	CString         GetMoudleAppName();
	void			NewWorkSpace();
	CString			CreatDefaultWorkSpace();

	// 新建窗口，并打开指定工作页面(如果指定了话), 返回新建的窗口
	CMPIChildFrame *OpenChildFrame(const CString &StrXmlCfmName, bool32 bHideNewCfm=false);
	//void			SaveChildFrame(LPCTSTR lpszCfmPath);	// 保存当前窗口至指定

	void			ClearCurrentWorkSpace();
	void			CloseWorkSpace();
	bool32			CanClose();
	void			ClearLogin();

	void			SaveWorkSpace(CString StrXmlName = L"", CString StrVersion = L"");

	CString			GetLastWspFile( CString StrUserName = L"", bool32 bNewUser = false);	
	static CString	GetWorkSpaceDir();

	void			LoadPicMenu();
	void			AppendPicMenu();

	void			InitSpecialMenu();

	bool32			BeMaxSize();
	CRect			CalcActiveChildSize();


public:
	void			NewEmptyReportChildFrame();			 // 板块菜单项,新建一个报价表.
	void			NewDefaultChildFrame(bool32 bBlank);
	void			NewDefaultIoViewChildFrame(int32 iIoViewGroupId);

public:
	void			GetBlockMenuInfo(CArray<T_BlockMenuInfo, T_BlockMenuInfo&>& aBlockMenuInfo);

public:
	void			SetEmptyGGTongView(CGGTongView * pView);			// 空白GGTongView 时,处理右键事件,得到这个View 的指针
	CGGTongView		*GetEmptyGGTongView();								// ChildFrame删除view时，可能需要判断EmptyGGTongView是否需要重置
	void			SetActiveGGTongViewXml(CGGTongView * pView);		// 设置激活视图
	CGGTongView*	GetActiveGGTongViewXml();							// 得到激活视图

public:	
	CIoViewBase *	FindActiveIoView();
	CView *			FindGGTongView();
	bool32			OnProcessF7(CGGTongView* pGGTonView = NULL);
	bool32			OnProcessCFF7(CMPIChildFrame *pChildFrame = NULL);
	bool32			IsFullScreen(CGGTongView* pGGTonView = NULL);		// 指定视图或当前活动视图是否是全屏
	static CGGTongView * FindGGTongView(CWnd *pWnd, CGGTongView *pGGTongViewDst = NULL);
	// 获得某个分组中的商品信息， 在新建一个视图的时候用的上
	bool32			GetMerchInSpecifyIoViewGroup(int32 iIoViewGroupId, IN CMerch *&pMerch);
	
	CIoViewBase *	FindNotReportSameGroupIdVisibleIoview(CIoViewBase *pActiveIoView, bool32 bOnlyInSameChildFrame);
	// 根据pGroupIoView的GroupId查找指定pic id，符合要求的ioView，如果指定要求SameGroup或者SameChild或者bNotInSameManager，则必须要求提供有效的GroupIoView，bNotInSameManager为true，则要求不能在同一个manager中，为false，则不做限制
	CIoViewBase *	FindIoViewByPicMenuId(UINT nPicID, CIoViewBase *pGroupIoView, bool32 bInSameGroup, bool32 bMustVisible, bool32 bOnlyInSameChildFrame, bool32 bCanNotInSameManager, bool32 bEnableSonObject = false);
	CIoViewBase * GetIoViewByPicMenuIdAndActiveIoView(UINT nPicId, bool32 bEnableCreate, bool32 bInSameGroup, bool32 bMustVisible, bool32 bOnlyInSameChildFrame, bool32 bCanNotInSameManager, bool32 bEnableSonObject = false);
	CIoViewBase *	FindIoViewInFrame(UINT nPicID, CMDIChildWnd *pChildFrame, bool32 bMustVisible = false, bool32 bEnableInherit=true, int32 iGroupId=-1);	// 查找位于该frame下的特定视图
	CIoViewBase *	FindIoViewInFrame(UINT nPicID, int32 iGroupId);	// 只查找同组下的特定视图
	BOOL			FindIoViewInFrame(OUT vector<CIoViewBase*>& outVt,  UINT nPicID, CMDIChildWnd *pChildFrame, bool32 bMustVisible = false, bool32 bEnableInherit=true, int32 iGroupId=-1 );	//add by weng.cx


	CIoViewReport *	FindIoViewReport(bool32 bInTopChildFrame);
    CIoViewNews   * FindIoViewNews(bool32 bInTopChildFram);
	CIoViewDuoGuTongLie* FindIoViewDuoGuTongLie(bool32 bInTopChildFram);
	CIoViewBase *	CreateIoViewByPicMenuID(UINT nID, bool32 bShowNow, CIoViewManager* pIoViewManagerIn = NULL);
	void			OnIoViewFaceChange(bool32 bFont,CIoViewBase * pIoViewSrc);
	
public:
	void			OnHotKeyMerch(int32 iMarketId, const CString &StrMerchCode);
	void			OnHotKeyMerch(const CString &StrMerchCode);
	void			OnHotKeyShortCut(const CString &StrKey);
	void			OnHotKeyBlock(const CString &StrBlockName, int32 iBlockType);
	void			OnHotKeyZXG();
	void			OnHotKeyLogicBlock(const CHotKey &hotKey);
	void			OnHotKeyIndex(const CHotKey &hotKey);

	bool32			PreTransGlobalHotKey(int32 iVirtualKeyCode);
	void			OnSpecifyMarketAndFieldReport(int32 iMarketId, int32 iMerchReportField, bool32 bDescSort);
	void			OnSpecifyBlockIdAndFieldReport(int32 iBlockId, int32 iMerchReportField, bool32 bDescSort);	// 打开指定逻辑板块
	//linhc 20100911 修改返回值
	CIoViewReport*	OnSpecifyBlock(T_BlockDesc &BlockDesc, bool32 bOpenExist = true);
	void			OpenSpecifyMarketOrBlock(int32 iMarketID);

	void			OpenSpecialSelectReport();		// 打开条件选股报价表，

	void			OnShowTimeSaleDetail();	// 查看分笔成交明细
	bool32			OnShowF7IoViewInChart(UINT nF7PicId, UINT nChartPicId =0);		// 于指定图表页面中查看F7化的指定业务视图

	void			OnShowMerchInChart(int32 iMarketId, const CString &StrMerchCode, UINT nPicId=0);	// 显示指定chart页面, 0则默认规则
	void			OnShowMerchInChart(CMerch *pMerch, CIoViewBase *pIoViewSrcOrg);		// 完成报价表等视图的dbclick功能, 记录跳转
	void			OnShowMerchInNextChart(CIoViewBase *pIoViewSrcOrg);	// 按下F5，切换到下一个图表页面, src必须为非NULL, 记录跳转
	CMPIChildFrame *FindChartIoViewFrame(bool32 bCreateIfNotExist = false, bool32 bHideNewCfm=false);	// 查找包含有分时或者K线的页面
	CMPIChildFrame *FindAimIoViewFrame(int32 iPicIdToFind, bool32 bCreateIfNotExist = false, bool32 bHideNewFrame=false);	// 查找指定包含pic的页面窗口(目前仅支持k与分时)
	CIoViewBase		*FindChartIoViewInSameFrame(CIoViewBase *pIoViewInSameFrameSrc);	// 查找同指定业务视图同窗口同分组的图表视图
	CIoViewBase		*FindChartIoViewInFrame(CMPIChildFrame *pChildFrame, CIoViewBase *pIoViewSrc=NULL);	// 页面中查找图表
	CIoViewBase		*FindActiveIoViewInFrame(CMPIChildFrame *pChildFrame);	// 指定页面激活的视图
	void			OnEscBackFrame();	// Esc跳转页面
	void			OnBackToHome();		// 跳转到首页
	void			OnBackToUpCfm();	// 返回到上一个操作页面
	void			OnCloseAllNotReserveCfm();	// 关闭所有非保留页面
	void			OnChangeOpenedCfm(int32 iPicId);		// 保留页面是否要重新加入返回列表中
	void			DoIoViewEscBackFrameInActiveFrame(CMPIChildFrame *pFrame, CMerch *pMerch);	// 调用指定页面的所有视图的OnEscBackFrame接口
	bool32			AddToEscBackFrame(CMPIChildFrame *pChildFrame);	// 加入跳转列表
	void			EmptyEscBackFrame();	// 清空跳转记录
	bool32			LoadSystemDefaultCfm(int32 iPicId, CMerch *pMerchToChange = NULL, bool32 bMustSeeChart = false);	// 加载或者置前系统默认版面, merch 为NULL，则以当前激活的, 记录跳转
	CMPIChildFrame *LoadSystemDefaultCfmRetFrame(int32 iPicId, CMerch *pMerchToChange = NULL, bool32 bMustSeeChart = false);	// 加载或者置前系统默认版面, merch 为NULL，则以当前激活的, 记录跳转
	CMPIChildFrame	*LoadSpecialCfm(const CString &StrCfmName);	// 加载特殊的cfm, 主要是要切换一下商品
	bool32			IsIndexInCurrentKLineFrame(const CString &StrIndexName, int32 iIndexFlag = 0);	// K线页面上是否存在该指标, iIndex=0不关注商品是否为大盘或个股 >0必须为大盘 <0必须是个股
	void			GetFrameSortedChartIoViews(OUT CArray<CIoViewBase *, CIoViewBase *> &aIoViews, CMPIChildFrame *pMustInFrame, int32 iGroupId=-1);

	void			ChangeIoViewKlineTimeInterval(int32 iTimeIntervalId);	// 改变k线的周期，iTimeIntervalId指定如IDM_CHART_KMINUTE类似的cmdid,为-1则代表自动选择下一个周期作为当前周期

	bool32			ShowPhaseSort(const T_PhaseOpenBlockParam *pParam = NULL);	// 显示阶段排行参数对话框，如确定，则显示阶段排行页面
	void			ShowHotBlockReport(const CGmtTime *pTimeStart=NULL, const CGmtTime *pTimeEnd=NULL);	// 显示热门板块分析, 两参数无用

	CMPIChildFrame	*CreateNewIoViewFrame(const CString &StrCfmName, UINT nPicId, bool32 bCfmSecret=true, CMerch *pMerchToChange=NULL);	// 创建一个指定业务视图的页面

	void			LoadPicCfmMenu();			// 初始化页面分析菜单
	bool32			OnPicMenuCfm(UINT nPicId);	// 加载指定的pic页面视图

	void			ShowBourseNotification(int32 iBourseId);
	void			PostUpdateWnd();
	void			InitialImageResource();

	void			ChangeAnalysisMenu(bool32 bLockedFrame);		// 根据frame的锁定状态变更分析菜单
	CNewMenu		*GetAnalysisMenu(bool32 bLockedFrame);			// 获取frame的锁定状态对应的分析菜单

	void			OnArbitrageF5( const CArbitrage &arb, CIoViewBase *pIoViewSrc=NULL);	// 套利F5切换
	void			OnArbitrageF5( UINT nDstArbTypeId, const CArbitrage &arb, CIoViewBase *pIoViewSrc=NULL);	// 套利F5切换
	bool32			OnArbitrageEscReturn( const CArbitrage &arb, CIoViewBase *pIoViewSrc=NULL);	// 套利图表esc

public:
	static void	    SaveXmlFile(const char * KpcFileName, const CString &StrContent);

	bool32			BeFromXml()	{ return m_bFromXml; }
	void			KeepRecentMerch(IN CMerch *pMerch);
	void			GetRecentMerchList(OUT CArray<T_RecentMerch, T_RecentMerch&> & RecentMerchList);

	static bool32   GetWspFlagAfterInitialMarketFlag();				  
	static void		SetWspFlagAfterInitialMarketFlag(bool32 bNeedSet); 

	//  处理历史分时走势.
	void			DealHistoryTrend(CIoViewKLine *pIoViewKLineSrc, const CKLine &KLineNow);
	void			DealHistoryTrendDlg(CIoViewKLine *pIoViewKLineSrc, const CKLine &KLineNow);

	CMPIChildFrame *GetHistoryChild()			{ return m_pHistoryChild; }
	void			SetHistoryChildNULL()		{ m_pHistoryChild = NULL; }

	CIoViewTrend *  GetHistoryTrend()			{ return m_pHistoryTrend; }
	void			SetHistoryTrendNULL()		{ m_pHistoryTrend = NULL; }

	int32			GetRandomNum();				// 取一个随机数	
	void			GenerateRandomNums();		// 生产 500 个随机数

	// 显示通知	
	void			ShowWelcome();
    void            ShowUSerRegister();
	bool32			ShowNotifyMsg();
	void			DrawCustomNcClient();
	UINT			OnMyNcHitTest(CPoint pt);			// 所有返回给系统的NCHitTest只有Caption和none，内部使用这个决定ncHitTest
	
	// 完善资料
	void			ShowCompleteMaterialDlg();          //个人完善资料界面
	CString			GetCompleteMaterialUrl();			//完善资料Url

	// 用户详细页面
	void			ShowUserInfoNormal();			// 点金手标准方式显示用户信息页面
	void			ShowUserInfoCfm();				// 版面内置网页显示用户信息页面
	void			ShowUserInfoDialog();			// 对话框弹窗显示用户信息页面
	void			ShowOnlineService();			// 显示在线客户页面
	void			GetUserInfo(T_UserInfo &stUserInfo);					// 获取用户个人信息

	void			ShowRegisterDialog();			// 显示注册对话框

	// TBWnd有关
	bool32			OpenWorkSpace(LPCTSTR lpszPath);
	void			OnChildFrameMaximized(bool32 bMax);		// childframe最大化时，去掉标题栏，需要通知mainFrame将标题栏按钮寄存在TBWnd上
	void			RegisterAcitveIoView(CIoViewBase *pIoView, bool32 bActive);		// 将ioView acitve/deactive消息通知给TBWnd
	bool32			DoF10();
	bool32			DoF11();
	void			ShowConditionWarning();
	

	// 手动重连的标志
	bool32			GetManualReConnectFlag() const { return m_bManualReconnect; }

	int32			DeleteBlockReportItemData(CNewMenu *pPopupMenu);	// 删除板块菜单上附加的资源
 
	// 脱机模式, 调整一些功能开放情况
	void			AdjustByOfflineMode();

	// 辅助区
	void	EnableMyDocking(DWORD dwDockStyle);
	void	AddDockSubWnd(CWnd *pWnd);			// 添加辅助区子窗口，设置该子窗口的父窗口为辅助区
	void	RemoveDockSubWnd(CWnd *pWnd);		// 从辅助区窗口中移除该子窗口 - 现在没有此操作的必要，移出的窗口要手动改变父子关系
	void	ShowDockSubWnd(CWnd *pWnd);			// 显示该辅助区子窗口
	BOOL	IsExistDockSubWnd(CWnd *pWnd);		// 是否存在该辅助子窗口
	void	DoToggleMyDock();					// 浮动/停靠
	
	void    OnShowImMessage();					//显示IM窗体 2013-10-31 add by cym
	void	ControlDisStatus(bool bIsDis);		// 显示隐藏右侧侧边栏

	void	AddToUserDealRecord(CString StrFunName, E_ProductType eType);	// 将记录添加到用户行为记录的vector中
	//设置开机启动
	void    SetAutoRun(BOOL bAutoRun);
	//检查开机是否启动
	void    GetAutoRunKeyValue();



	/////////////网页请求函数 /////////////// 

	// getOptiona函数使用
	void PackUserBlockInfo(string &strUserBlockUtf8);
	// addUserStock函数调用
	void AddOrDelUserblockMerch(CString strMerchCode,CString strMarketid, bool bIsDel);

public: 
	void		AddGGTongView(CGGTongView* p);
	void		DelGGTongView(CGGTongView* p);
	void		GetAllGGTongViews(OUT CArray<CGGTongView*, CGGTongView*>& aGGTongViews);

	void		AddIoView(CIoViewBase* pViewBase);
	void		RemoveIoView(CIoViewBase* pViewBase);
	void	    OnOpenChildframeFromWsp(CString strXmlName,bool bOpenCur);
	void		ShowNewsCountent(T_NewsContentInfo& stNewsContentInfo);

public:
	void		OnQuickTrade(int32 iFlag, CMerch* pMerch, CReportScheme::E_ReportHeader eHeader = CReportScheme::ERHCount);
	bool		HideAllTradeWnd();		// 隐藏所有交易窗口

	int32		GetValidGroupID(); //获取有效的可用分组ID

	//********************************************************
	//这些都是视图管理的管理的方法，最好新建一个管理类 by hx
	//视图控制 
	void		OnViewMerchChanged(IN CIoViewBase *pIoViewSrc, IN CMerch *pMerch);
	void		OnViewMerchChanged(int32 iIoViewGroupId, CMerch *pMerch);

	void		OnViewMerchChangeBegin(IN CIoViewBase *pIoViewSrc, IN CMerch *pMerch);
	void		OnViewMerchChangeBegin(IN int32 iIoViewGroupId, CMerch *pMerch);
	void		OnViewMerchChangeEnd(IN CIoViewBase *pIoViewSrc, IN CMerch *pMerch);
	void		OnViewMerchChangeEnd(IN int32 iIoViewGroupId, CMerch *pMerch);
	
	void		OnViewGridHeaderChanged(E_ReportType eReportType);	
	void		OnViewFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName );

	// 相关商品相关
	bool32		OnViewAddCompareMerch(IN CIoViewBase *pIoViewSrc, IN CMerch *pMerch);
	void		OnViewRemoveCompareMerch(IN CIoViewBase *pIoViewSrc, IN CMerch *pMerch);
	void		OnViewClearCompareMerchs(IN CIoViewBase *pIoViewSrc);

	bool		HasValidViewTextNews();
	
	CMerch		*GetIoViewGroupMerch(int32 iGroupId); // 应当要记录每个group变更，并新增一全局groupid，公共视图属于全局groupid TODO
	//********************************************************

	void		RemoveHotKey(E_HotKeyType eHotKeyType);
	void		BuildHotKeyList();
	//
	void        RedrawAdvertise();
	void        CreateAdvertise();
	void        HideAdvertise();
	void		SetAdvertiseOwner(CWnd *pWnd);
	void        NotifyAdvMerchChange();
	void        SetAdvFont(LOGFONT *pLogFont);
///////////////////////////////////////////////
	// 获取当前鼠标所在菜单的按钮
	int TMenuButtonHitTest(CPoint point);

	// 获取当前鼠标所在系统的按钮
	int TSysButtonHitTest(CPoint point);

	// 在标题栏上添加菜单按钮
	CNCButton *AddTitleMenuButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);

	// 在标题栏上添加系统按钮
	CNCButton *AddTitleSysButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);

	// 改变标题栏上用户名
	void ChangeUserNickName(CString strNickName);

	CMenuBar *GetMenuBar();

	//显示帮助菜单
	void ShowHelpMenu(UINT nID);

	// 外挂交易
	void DoTradePlugin();
	
	bool32	GetTradeExePath(OUT CString &StrExePath, bool32 bEnableReq=false, bool32 bDirtyOld=false);	// 获取交易程序路径，没有设置需要用户选择设置
	void    SetTradeExePath( LPCTSTR lpszExePath );// 设置交易程序路径，NULL或者0长度为删除该配置
	void    ShowRealTradeDlg();

public:
	void handleMsgCenterRes(std::string strJson);// 处理消息中心的响应
	void ShowInfoCenterMsg(T_pushMsg stMsg);
	void ConnNotify(E_CONN_NOTIFY eConnNotify);

	void handleHeartbeat(Json::Value vData);// 处理资讯的心跳
	void handleInfoContentQuery(Json::Value vData);// 处理资讯内容的查询
	void handlePushInfo(Json::Value vData);// 处理资讯的推送
	// 处理需要刷新Url相关版面
	void OpenWebWorkspace(const CString &StrWsp, const CString &StrUrl);

private:
	void ShowMenu(T_MenuItemData tData,E_MenuItemType eType, CRect rect);
	CDlgPullDown* CreateSubMenu(T_MenuItemData &tmenuData, E_MenuItemType eType, int left, int top);		// 更多按钮的二级菜单
	CDlgPullDown* AdjustSubMenuPos(CString strParent, int left, int top);	// 调整二级菜单的显示位置
	void InitMapMenuItem();        // 初始化菜单的数据
	int32 FindFirstVecIDByMenuID(int32 iMenuID);
	CNCButton*	GetCNCButton(int iId);

private:
	void ShowSystemMenu(CRect rect);
	void ShowQuoteMenu(CRect rect);
	void ShowFuncMenu(CRect rect);       // 显示功能菜单


//  web端命令处理函数
private:
	void WebComOpenWebPage(CAnalysisWeb cAnalysisWeb);
	void WebComOpenLocalFile(CAnalysisWeb cAnalysisWeb);
	void WebComOpenView(CAnalysisWeb cAnalysisWeb);
	void WebComOpenLocalApp(CAnalysisWeb cAnalysisWeb);
	void WebComOpenPassMsg(CAnalysisWeb cAnalysisWeb);

	
//   选股模型类别数据
public:
	/////////////////////////////////////////////////////
	//===================CODE CLEAN======================
	//////////////////////////////////////////////////////
	//void QueryPickModelTypeInfo();
	//void QueryPickModelTypeStatus();

	//void PackJsonType(string &strTransDataUtf8);	// 封装请求
	//void UnPackJsonPickModelTypeInfo(string strMerchData);		// 解包选股类型
	//void UnPackJsonPickModelTypeStatus(string strMerchData);		// 解包选股状态
	//map<int32, map<int32,T_SmartStockPickItem>> m_mapPickModelTypeGroup;		// 选股模型数据
	//vector<int>    m_vPickModelTypeGroup;
	//map<int32, map<int32,T_SmartStockPickItem>> m_mapPickModelStatusGroup;		// 返回回来的选股状态


	int				m_iSlectModelId;											// 选择的选股模型组
public:
	bool32			  m_bDoLogin;		//	是否登录过
	bool32			  m_bLoginSucc;		//	是否登录成功
	bool32			  m_bShowLoginDlg;	//	退出时是否显示登录窗体
	int  m_nReqTime;
	bool m_bStartEconoReq;
	std::string m_strFirstEconoData;// 保存第一次请求的财经日历数据
	CDlgTrace*		  m_pDlgTrace;
	CAdjustViewLayoutDlg* m_pDlgAdjustViewLayout;
	CKeyBoardDlg*	  m_pDlgKeyBoard;
	CDlgSpirite*	  m_pDlgSpirite;
	CDlgNetWork*	  m_pDlgNetWork;
	CDlgBrokerWatch*  m_pDlgBrokerWatch;
	CDlgRecentMerch*  m_pDlgRecentMerch;
	CDlgIndexPrompt*  m_pDlgIndexPrompt;
	CDlgAlarm *		  m_pDlgAlarm;
	CDlgWait *		  m_pDlgWait;
	CDlgIm           *m_pDlgIm; // 2013-10-28 add by cym 创建IM对话框
	CDlgIndexChooseStock* m_pDlgIndexChooseStock;
	CDlgNetFlow*	  m_pDlgNetFlow;
	CDlgBelongBlock*  m_pDlgbelong;
	CDlgNewCommentIE  *m_pDlgComment;		// 注册弹窗

	//CDlgF10*		  m_pDlgF10;
	CDlgWebF10*	m_pDlgF10;


	CNewMenu		  m_menuAnalysisLocked;		// 锁定状态下的分析菜单
	CNewMenu		  m_menuAnalysisUnLock;		// 非锁定状态下的分析菜单

	CArray<HWND, HWND>	m_aCfmsReturn;	// 每次跳转时的前激活窗口

	//CDlgLoginNotify*  m_pDlgLoginNotify;

	//CToolBar		m_wndTestToolBar;

	CStatusBarEx	  m_wndStatusBar;
	CCommentStatusBar m_wndCommentSBar; //金评滚动栏 2013-11-12 by cym 
	CNewToolBar		m_wndToolBar;
	CRect			m_RectBBs;
	bool32			m_bOpenLast;
	bool32			m_bMaxForSysSettingChange;	
	CMyContainerBar	*m_pMyControlBar;			// 辅助区主管
	CTradeContainerWnd	*m_pTradeContainer;		// 交易窗口
	CTradeContainerWnd	*m_pSimulateTrade;		// 模拟交易

	CArray<T_RecentMerch,T_RecentMerch&>		m_aRecentMerchList;
	CArray<T_BlockMenuInfo,T_BlockMenuInfo&> m_aBlockMenuInfo;

	CRect			m_rectWindow;
	CRect			m_rectCaption;
	CRect			m_rectMin;
	CRect			m_rectMax;
	CRect			m_rectClose;
	CRect			m_rectLast;
	int32			m_iShowFlag;
	CWnd *			m_pKBParent;
	DWORD			m_dwHotKeyTypeFlag;		// 是否只关注特定类别
	CPoint			m_ptLastNCLButtonDown;
	
	// 键盘精灵
	bool32			m_bForceFixKeyBoard;			// 强制指定一个目标, 除非这个标记是 false,否则不能更改
	
	// 历史分时
	bool32			   m_bShowHistory;
	CMPIChildFrame*    m_pHistoryChild;
	CIoViewTrend*	   m_pHistoryTrend;

	static bool32	   m_bNeedSetWspAfterInitialMarket;		// 市场初始化成功后是否需要设置登录的信息
	//
	CDlgAdvertise *m_pDlgAdvertise;

	CNewTBWnd		   *m_pNewWndTB;
    CNewCaptionTBWnd   *m_pNewCaptionWndTB;
	CLeftToolBar       m_leftToolBar;
	//CShortCutMenuToolBar      m_ShortCutMenuToolBar;
	CDlgJPNews		   *m_pDlgJPNews;

	bool32				m_bQuickOrderDn;
    UINT                m_LastTimeSet;
	CNewMenu			m_sysMenu;
	vector<T_UserDealRecord> m_vUserDealRecord;

	T_WebOpenCfm		m_stWebOpenCfm;		// 网页打开版面的需要的参数
private:
	CGGTongView*	   m_pActiveGGTViewFromXml;	
	CGGTongView*	   m_pEmptyGGTongView;

	Image*			   m_pImgLogo;	// 主页面的标题栏背景图
	Image*			   m_pImgLogoLeft;	// 标题栏的左边logo
    Image*			   m_pImgMenuBtn1;
	Image*			   m_pImgSysBtn;
	Image*			   m_pImgHideBtn;
	Image*			   m_pImgShowBtn;
	Image*			   m_pImgSpliter;
	Image*			   m_pImageCaption;
    Image*             m_pImageUserAccount;
    Image*             m_pImageLogin;
	Image*             m_pImgUserPhoto;
	Image*             m_pImgMenuExit;
	Image*             m_pImgMenuHelp;
	Image*             m_pImgMenuAlarm;
	Image*             m_pImgMenuSelStock;
	Image*             m_pImgMenuSystem;
	Image*             m_pImgMenuTrade;
	Image*             m_pImgLoginSpliter;
	//下拉菜单图片
    Image*             m_pImgCenterSplitBottom;
	Image*             m_pImgCenterSplitTop;
	Image*             m_pImgHelpBottom;
	Image*             m_pImgHelpTop;
	Image*             m_pImgItemArrow;
	Image*             m_pImgItemSimple;
	Image*             m_pImgSysBottom;
	Image*             m_pImgSysTop;
	Image*             m_pImgTradeTop;
	
	E_CurrentShowState m_eCurrentShowState;

private:
	CDlgPullDown *m_pDlgPullDown;                          //  下拉菜单
	vector<CDlgPullDown*> m_vecDlgSubPullDown;             //  二级下拉菜单 

	buttonContainerType m_mapPullDown;						//	下拉菜单的按钮集
	buttonContainerType m_mapMorePullDown;					//	二级下拉菜单的按钮集

	Image*             m_pImgArrowMenu;
	Image*             m_pImgMenu;
	std::map<int,VecMenuContainer>   m_mapMenuCollect;     // 一级菜单的数据集合
	CNCButton::T_NcFont m_fontMenuItem;
	map<int32,int32>    m_mapMenuQuoteID;	               // 菜单id 对应的市场id
	std::map<int, CNCButton>  m_mapPullDownBtns;           // 当前显示的一级菜单和二级菜单的按钮集合，为后面设置按钮是否可用服务

private:
	CMDIClientEx	   m_wndMDIClient;
	CReBar			   m_wndReBar;
    //CReBar			   m_wndReBar2;

	map<int, CNCButton> m_mapMenuBtn;		// 标题栏菜单按钮列表
	map<int, CNCButton> m_mapSysBtn;		// 标题栏系统按钮列表
	int			m_iXMenuBtnHovering;		// 标识鼠标进入菜单按钮区域
	int			m_iXSysBtnHovering;			// 标识鼠标进入系统按钮区域

	//2013-10-29 by cym 记录原框架移动的位置
	CRect m_RectOld;

	int32	m_nFrameWidth;	// 边框宽度
	int32	m_nFrameHeight;	// 边框高度
	int32	m_nTitleLength;	// 标题栏文本宽度
	//CMenuBar	*m_pMenuBar;// 菜单栏

private:
    CDlgAccount  *m_pDlgAccount;
	CDlgWebContent	*m_pDlgWebContent;
    CDlgDefaultUserRegister *m_pDlgRegister;
    CDlgPushMessage *m_pDlgPushMsg;
	
    std::map<int, CNCButton> m_mapHelpBtn;	// 首页工具栏按钮列表
	int32			   m_iRandIndex;
	CArray<int32,int32> m_aiRandomNums;
	
	int32			   m_uiConnectServerTimers;
	bool32			   m_bHaveUpdated;						// 是否更新过

	int32			   m_uiConnectNewsServerTimers;			// 资讯服务器连接记数

	bool32			   m_bFirstDisplayFromXml;				// 标志是否是第一次从Xml读取版面数据，初始化显示，仅一次有效

	bool32			   m_bFromXml;


	CString			   m_StrWindowText;						// 窗口标题文字
	bool32			   m_bWindowTextValid;					// 现在的窗口标题文字是否有效
private:	
	bool32			   m_bManualReconnect;					// 是否在手动重连服务器, 这个时候, 行情服务器连接上了不要发认证请求, 待测速完成再发

	DWORD				m_dwUserConnectCmdFlag;

	T_UserInfo          m_stUserInfo;                       //用户信息

private:
	CRecordDataCenter* m_pRecordDataCenter;					// 记录数据库

private:
	CArray<CGGTongView*, CGGTongView*>	m_aGGTongViews;
	CHotKey m_hotKey;

public:
	CArray<CIoViewBase*,CIoViewBase*>	m_IoViewsPtr;
	CArray<T_CompareMerchInfo, T_CompareMerchInfo&>	m_aCompareMerchList;	// 叠加商品列表
	CArray<CHotKey, CHotKey&>	m_HotKeyList;				// 快捷键

///////////////////////////////////////////////////////////////////////////////////////
//

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual int	 OnToolHitTest(CPoint point, TOOLINFO* pTI)const;
	virtual void RecalcLayout(BOOL bNotify  = TRUE );
	virtual void OnUpdateFrameMenu(HMENU hMenuAlt);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL


// Generated message map functions
public:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWindowNew();
	afx_msg void OnUpdateWindowNew(CCmdUI* pCmdUI);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnSetting();
	afx_msg void OnGridheadSet();
	afx_msg void OnBlockSet();
	afx_msg LRESULT OnMsgHotKey(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgGridHeaderChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgFontChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgColorChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgMainFrameOnStatusBarDataUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnTimerFreeMemory(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnTimerSyncPushing(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnTimerInitializeMarket(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataServiceConnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataServiceDisconnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataRequestTimeOut(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataCommResponse(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataServerConnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataServerDisconnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataServerLongTimeNoRecvData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnGeneralNormalUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnGeneralFinanaceUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnMerchTrendIndexUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnMerchAuctionUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnMerchMinuteBSUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnAuthPlugInHeart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnKLineNodeUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgIndexChsMideCoreKLineUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnTimesaleUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnNewsListUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnF10Update(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnLandMineUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnPublicFileUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnRealtimePriceUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnRealtimeLevel2Update(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnRealtimeTickUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnAllMarketInitializeSuccess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnMsgViewDataOnNewsResponse(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnClientTradeTimeUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgPackageLongTime(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgPackageTimeOut(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgMainFrameKickOut(WPARAM wParam, LPARAM lParam);

	/////////////////////////////////////////////////////
	//===================CODE CLEAN======================
	//////////////////////////////////////////////////////
	/*afx_msg LRESULT OnMsgPickModelTypeInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgPickModelTypeStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgPickModelTypeEvent(WPARAM wParam, LPARAM lParam);*/

	
	afx_msg LRESULT OnUpdateSelfDrawBar(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgMainFrameManualOptimize(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnChooseStockResp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDapanStateResp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChooseStockStatusResp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNewStockResp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnToolbarCommand(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT	OnMsgViewDataNewsServerConnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnMsgViewDataNewsServerDisConnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnMsgDllTraceLog(WPARAM w, LPARAM l);
	afx_msg LRESULT OnMsgViewDataOnAuthFail(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnNewWorkspace();
	afx_msg void OnNewChildframe();
	afx_msg void OnNewCfm();		// 创建新的页面
	afx_msg void OnCloseAllWindow();	// 关闭所有页面(除默认外)
	afx_msg void OnOpenChildframe();
	afx_msg void OnUpdateSaveChildFrame(CCmdUI* pCmdUI);
	afx_msg void OnSaveChildFrame();
	afx_msg void OnUpdateLockChildFrame(CCmdUI* pCmdUI);
	afx_msg void OnLockChildFrame();
	afx_msg void OnOpenWorkspace();
	afx_msg void OnSaveasChildframe();
	afx_msg void OnSaveasWorkspace();
	afx_msg void OnSaveWorkspace();
	afx_msg void OnFormulaMag();
	afx_msg void OnDelView();
	afx_msg void OnUpdateCloseChildFrame(CCmdUI *pCmdUI);
	afx_msg void OnCloseChildFrame();
	afx_msg void OnUpdateViewSplitCmd(CCmdUI *pCmdUI);
	afx_msg void OnAddUpView();
	afx_msg void OnAddDownView();
	afx_msg void OnAddLeftView();
	afx_msg void OnAddRightView();
	afx_msg void OnDisConnectServer();
	afx_msg void OnConnectServer();
	afx_msg void OnDownLoad();
	afx_msg void OnUpdate();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNcPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnNcMouseLeave(WPARAM, LPARAM);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnMenuBlock(UINT nID);
	afx_msg void OnMenuQuoteMarket(UINT nID);
	afx_msg void OnUpdatePicMenu(CCmdUI *pCmdUI);
	afx_msg void OnPictureMenu(UINT nId);	
	afx_msg void OnIoViewMenu (UINT nID);
	afx_msg void OnUpdateIoViewMenu(CCmdUI *pCmdUI);
	afx_msg void OnMenuIoViewReport(UINT nID);
	afx_msg void OnMenuIoViewChart(UINT nID);
	afx_msg void OnMenuIoViewDetail(UINT nID);
	afx_msg void OnMenuIoViewStarry(UINT nID);
	afx_msg void OnMenuUIIoViewStarry(CCmdUI *pCmdUI);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnInfoNews();
	afx_msg void OnNetFlow();
	afx_msg void OnSelfDraw();
	afx_msg void OnInfoShBourse();
	afx_msg void OnInfoSzBourse();
	afx_msg void OnInfoHkBourse();
	afx_msg void OnInfoF10();
	afx_msg void OnBrokerWatch();
	afx_msg void OnSelectStock();
	afx_msg void OnCustomStock();
	afx_msg void OnViewCalc();
	afx_msg void OnIndexChooseStock();	
	afx_msg void OnViewStatusBar();
	afx_msg void OnViewCommentStatusBar(); //金评状态栏 2013-11-14 by cym
	afx_msg void OnViewSpecialBar();
	afx_msg void OnUpdateViewSpecialBar(CCmdUI *pCmdUI); 
	afx_msg void OnUpdateViewStatusBar(CCmdUI *pCmdUI); 
	afx_msg void OnUpdateViewCommentStatusBar( CCmdUI *pCmdUI );
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDlgSyncExcel();			// 同步Excel管理
	afx_msg void OnAddClrKLine();
	afx_msg void OnAddTrade();
	afx_msg void OnDelAllPrompt();
	afx_msg void OnStockSpecial(UINT nId);		// 特色菜单功能全在这里了
	afx_msg void OnStockSpecialUpdateUI(CCmdUI *pCmdUI);
	afx_msg void OnBelongBlockMsg();//linhc 添加右键菜单“所属板块”
	afx_msg void OnVedioTrainning();
	afx_msg LRESULT OnSetWindowText(WPARAM w, LPARAM l); // 拦截设置窗口标题消息 WM_SETTEXT
	afx_msg LRESULT OnGetWindowText(WPARAM w, LPARAM l);
	afx_msg LRESULT OnGetWindowTextLength(WPARAM w, LPARAM l);
	afx_msg LRESULT OnFindGGTongView(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTabRename(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnGetNowUseColor(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnGetSysColor(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnGetSysFontObject(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnGetIDRMainFram(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnIsShowVolBuySellColor(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnGetStaticMainKlineDrawStyle(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnGetCenterManager(WPARAM wParam,LPARAM lParam);
	afx_msg void OnConnectNewsServer();	// 资讯服务器
	afx_msg void OnDisConnectNewsServer();
	afx_msg LRESULT OnMsgViewDataOnNewsTitleUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnNewsContentUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnCodeIndexUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnNewsPushTitle(WPARAM wParam, LPARAM lParam);
	afx_msg void OnConnectSetting();
	afx_msg void OnChooseSimpleStock();
	afx_msg void OnShowShortcutBar();
	afx_msg void OnMerchNotePad();
	afx_msg void OnMarketRadar();
	afx_msg void OnUpdateMarketRadar(CCmdUI *pCmdUI);
	afx_msg void OnMarketRadarAddResult();
	afx_msg LRESULT OnCheckCfmHide(WPARAM w, LPARAM l);		// w要检查的childframe hwnd
	afx_msg void OnUpdateMainMenu(CCmdUI *pCmdUI);
	afx_msg void OnMainMenu(UINT nId);
	afx_msg void OnArbitrage();
	afx_msg void OnDefaultCfmLoad();
	afx_msg void OnUpdateConnectServer(CCmdUI *pCmdUI);
	afx_msg void OnIWantBuy();
	afx_msg void OnMenuInvestmentAdviser(UINT nId);
	afx_msg void OnMenuNews(UINT nId);
	afx_msg void OnMenuTrade(UINT nId);
	afx_msg void OnMenuClassRoom();
	afx_msg void OnMenuPhaseSort();
	afx_msg void OnAutoRun();

	afx_msg void OnFirmTradeShow();
	afx_msg	void OnSimulateTradeShow();
	afx_msg	void OnSimulateOpenAccount();
	afx_msg	void OnSimulateHQRegister();
	afx_msg	void OnSimulateHQLogin();
	afx_msg LRESULT	OnSimulateImageUrl(WPARAM wParam, LPARAM lParam);
    afx_msg void OnHQRegisterSucced();
	afx_msg LRESULT	OnWebReLoginPC(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnWebPersonCenterRsp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnWebCommand(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnWebCallBack(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnWebOpenCfm(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnWebOpenTrend(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnOpenOutWeb(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnShowWebDlg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRestoreWorkSpace();
	afx_msg void OnMenuAutoConnect();
	afx_msg void OnUpdateMenuAutoConnect(CCmdUI* pCmdUI);
	afx_msg void OnMenuShowToolBar();
	afx_msg void OnUpdateMenuShowToolBarStatus(CCmdUI* pCmdUI);
	afx_msg void OnExportData();
	afx_msg LRESULT OnShowRightTip(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddUserDealRecord(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnIsUserHasIndexRight(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnJumpToTradingSoftware(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnShowJPNews(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnBrowerPtr(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnJPNewsTitle(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	afx_msg LRESULT OnReqInfoCenter(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnWebCloseDlg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnCloseOnlineServDlg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnWebThirdLoginRsp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnWebkeyDown(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__234DC6B0_FD03_4692_B156_E8947A1EB907__INCLUDED_)
