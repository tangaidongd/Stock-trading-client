#ifndef _IO_VIEW_REPORT_H_
#define _IO_VIEW_REPORT_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "IoViewShare.h"
#include "GridCtrlSys.h"
#include "CStaticSD.h"
#include "GuiTabWnd.h"
#include "ReportScheme.h"
#include "MerchManager.h"
#include "BlockManager.h"

#include "MarkManager.h"
#include "PlugInStruct.h"
#include "NCButton.h"
#include <map>
#include <vector>
using std::map;
using std::vector;

/////////////////////////////////////////////////////////////////////////////
// CIoViewReport

typedef struct T_BlockDesc
{
	enum E_BlockType
	{
		EBTPhysical = 0,		// 物理板块
		EBTUser,				// 用户板块
		EBTLogical,				// 逻辑板块
		EBTSelect,				// 条件选股
		EBTBlockLogical,		// 新增逻辑板块
		EBTMerchSort,	        // 热门强龙
		EBTRecentView,			// 最近浏览
		EBTOpenBlockLogical,	// 打开新增逻辑板块
		EBTCount
	};

	T_BlockDesc()
	{
		m_eType			= EBTCount;
		m_iMarketId		= -1;
		m_StrBlockName	= L"";
		m_pMarket		= NULL;
		m_aMerchs.RemoveAll();

		m_iFieldAdd		= -1;
		m_iTypeAdd		= -1;
	}

	T_BlockDesc::T_BlockDesc(const T_BlockDesc& stBlockSrc)
	{
		m_eType			= stBlockSrc.m_eType;
		m_iMarketId		= stBlockSrc.m_iMarketId;
		m_StrBlockName	= stBlockSrc.m_StrBlockName;
		m_pMarket		= stBlockSrc.m_pMarket;
		m_aMerchs.Copy(stBlockSrc.m_aMerchs);
		m_iFieldAdd		= stBlockSrc.m_iFieldAdd;
		m_iTypeAdd		= stBlockSrc.m_iTypeAdd;
	}

	T_BlockDesc& T_BlockDesc::operator= (const T_BlockDesc& stBlockSrc)
	{
		if ( this == &stBlockSrc )
		{
			return *this;
		}

		m_eType			= stBlockSrc.m_eType;
		m_iMarketId		= stBlockSrc.m_iMarketId;
		m_StrBlockName	= stBlockSrc.m_StrBlockName;
		m_pMarket		= stBlockSrc.m_pMarket;
		m_aMerchs.Copy(stBlockSrc.m_aMerchs);
		m_iFieldAdd		= stBlockSrc.m_iFieldAdd;
		m_iTypeAdd		= stBlockSrc.m_iTypeAdd;

		return *this;
	}

	E_BlockType	m_eType;				// 0:物理板块; 1:用户板块; 2:系统板块(逻辑板块)

	int32		m_iMarketId;			// 物理板块的MarketId
	CString		m_StrBlockName;			// 板块的名称
	CMarket*	m_pMarket;				// 物理板块的市场指针			(物理板块时不为 NULL , 其他为 NULL)
	CArray<CMerch*, CMerch*> m_aMerchs;	// 其他板块的商品列表			(物理板块时为 NULL , 其他为配置文件中读取出来的数据)

	int32		m_iFieldAdd;
	int32		m_iTypeAdd;	

} T_BlockDesc;

typedef struct T_UserBlockMerchColor
{
	CString		m_StrName;				// 自选股商品
	COLORREF	m_Color;				// 需要显示的颜色

}T_UserBlockMerchColor;

// 标签类型, 有集合类的和实体类的
typedef enum E_TabInfoType
{
	ETITCollection,					// 单击弹出菜单
	ETITEntity,						// 单击直接打开对应板块, 如果是空的时候, 打开选择板块对话框
		
	ETITCount,
		
}E_TabType;

// 简易信息
typedef struct T_SimpleTabInfo
{
	T_SimpleTabInfo(int32 iD, int32 iIdDefault, int32 iTabIndex, const CString& StrShowName, E_TabInfoType eTabType, T_BlockDesc::E_BlockType eBlockType)
	{	
		m_iID			= iD;
		m_iIdDefault	= iIdDefault;
		m_iTabIndex		= iTabIndex;
		m_StrShowName	= StrShowName;
		m_eTabType		= eTabType;
		m_eBlockType	= eBlockType;
	}

	T_SimpleTabInfo()
	{
		m_iID			= 0;
		m_iIdDefault	= 0;
		m_iTabIndex		= 0;
		m_StrShowName	= _T("");
		m_eTabType		= ETITCount;
		m_eBlockType	= T_BlockDesc::EBTCount;
	}

	//
	int32					  m_iID;			// ID 号
	int32					  m_iIdDefault;		// 集合类型默认显示的板块号
	int32					  m_iTabIndex;		// 顺序号
	E_TabInfoType			  m_eTabType;		// Tab 类型
	CString					  m_StrShowName;	// Tab 显示名称
	T_BlockDesc::E_BlockType  m_eBlockType;		// 板块的类型

}T_SimpleTabInfo;
typedef	vector<T_SimpleTabInfo>	SimpleTabInfoArray;

// tabInfo所属类 - 分为默认类(id=0) 股票类(id=?) 期货类(id=?) 综合类(id=?)
#define SIMPLETABINFOCLASS_DEFAULTID	(-1)
struct T_SimpleTabInfoClass
{
	T_SimpleTabInfoClass()
	{
		iId = SIMPLETABINFOCLASS_DEFAULTID;
	}
	int32				iId;
	CString				StrName;
	SimpleTabInfoArray	aTabInfos;
};
typedef map<int, T_SimpleTabInfoClass>	SimpleTabClassMap;

// 每个页面的信息
typedef struct T_TabInfo
{
	T_TabInfo()
	{
		m_eTabType			= ETITCount;
		m_iXScrollPos		= 0;
		m_iDataIndexBegin	= 0;
		m_iDataIndexEnd		= 49;
		m_StrShowName		= L"";
		m_bAlignDataEnd		= false;
	}

	//
	E_TabInfoType	m_eTabType;			// 标签类型
	T_BlockDesc		m_Block;			// 这个页面的板块信息
	CString			m_StrShowName;		// 显示名称
	int32			m_iXScrollPos;		// 保存本页面的 X 滚动条信息
	int32			m_iDataIndexBegin;	// 数据开始的索引
	int32			m_iDataIndexEnd;	// 数据结束的索引

	bool32			m_bAlignDataEnd;	// 需要对齐最后一格

}T_TabInfo;


// 标签类型, 有集合类的和实体类的
typedef enum E_TabTopType
{
	ETAB_CHANGE_MARKET = 1,			// 切换市场
	ETAB_OPEN_CFM,					// 打开版面
	ETAB_PLATE_MENU,				// 板块菜单
	ETAB_CUSTOM_ORDER,				// 自定义
	ETAB_CHANGE_REPORTHEAD,			// 改变报价表表头
	ETAB_UserBlock,					// 自选股
	ETAB_RecentView,	            // 最近浏览
	ETABCount,

}E_TabTopType;

typedef enum E_ReportHeadType
{
	ERHT_CapitalFlow = 1,			// 资金流向
	ERHT_MainMasukura,              // 主力增仓
	ERHT_FinancialData,				// 财务数据
	ERHT_Count,
};

// 每个顶部标签栏信息
typedef struct T_TopTabbar
{
	T_TopTabbar()
	{
		m_iBtnType			= ETABCount;
		m_StrShowName		= L"";
		m_iBtnID			= -1;
		m_iMarketID			= -1;
		m_iBlockid			= -1;
		m_iBlockType		= -1;
		m_strCfmName		= L"";
		m_eReportHeadType   = ERHT_Count;
		m_bSelected         = false;
		m_colNoarml			= RGB(0,0,0);
		m_colPress			= RGB(0,0,0);
		m_colHot			= RGB(0,0,0);
		m_colTextNoarml		= RGB(0,0,0);
		m_colTextPress		= RGB(0,0,0);
		m_colTextHot		= RGB(0,0,0);
	}

	//
	//T_BlockDesc		m_Block;		// 这个页面的板块信息
	CString			 m_StrShowName;		// 显示名称
	int32			 m_iBtnID;			// 按钮的ID
	int32			 m_iBtnType;		// 数据类型
	int32			 m_iMarketID;		// 市场ID
	CString			 m_strCfmName;		// 版面名称
	E_ReportHeadType m_eReportHeadType;	// 报价表表头类型 
	bool32           m_bSelected;		// 是否选中
	int32			 m_iBlockid;		// 板块ID
	int32			 m_iBlockType;		// 板块类型

	COLORREF		 m_colNoarml;
	COLORREF		 m_colPress;
	COLORREF		 m_colHot;
	COLORREF		 m_colTextNoarml;
	COLORREF		 m_colTextPress;
	COLORREF		 m_colTextHot;
}T_TopTabbar;

// 每个页面的信息
typedef struct T_SelectBlockInfo
{
	T_SelectBlockInfo()
	{
		m_iMarketID = -1;
		m_StrBlockName = _T("");
		m_eType = T_BlockDesc::EBTCount;
	}
	//
	int32 		m_iMarketID;				// 市场ID
	CString		m_StrBlockName;				// 模块名称
	T_BlockDesc::E_BlockType	m_eType;    // 板块类型

}T_SelectBlockInfo;



class CIoViewReport : public CIoViewBase, public CGridCtrlCB, public CGuiTabWndCB, public CObserverUserBlock, public CMerchMarkChangeListener
{
// Construction 
public:
	CIoViewReport();
	virtual ~CIoViewReport();

	DECLARE_DYNCREATE(CIoViewReport)
	
public:
	// from CControlBase
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public:
	// from CObserverUserBlock 通知
	virtual void	OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType);	
	
public:
	// from CIoViewBase
	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();

	virtual bool32	FromXmlExtra(TiXmlElement *pElement) { return true; };
	virtual CString ToXmlExtra() { return CString(); }; 
	
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();

	virtual CMerch*	GetNextMerch(CMerch* pMerchNow, bool32 bPre);

	virtual bool32	GetStdMenuEnable(MSG* pMsg);

	virtual void	LockRedraw();
	virtual void	UnLockRedraw();

	virtual CMerch *GetMerchXml();
	virtual	void	GetMerchXmls(OUT MerchArray &aMerchs);
	virtual E_IoViewType GetIoViewType() { return EIVT_MultiMerch; }
	//	指定所有的cell设置新字体
	void			ResetGridFont();
	//	指定显示的cell设置新字体
	void			ResetVisibleGridFont();
public:	
	void			OnDblClick();

	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();
	virtual void	OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs);
	virtual void	OnVDataGridHeaderChanged(E_ReportType eReportType);

	virtual void	OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType);

	virtual void	OnIoViewColorChanged();
	virtual void	OnIoViewFontChanged();

	virtual	void	OnVDataReportInBlockUpdate(int32 iBlockId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount, const CArray<CMerch *, CMerch *> &aMerchs);

	virtual	bool32	IsPluginDataView() { return true; }
	virtual	void	OnVDataPluginResp(const CMmiCommBase *pResp);


	// 新股接口
	virtual void	OnVDataQueryNewStockInfo(const char *pszRecvData);

	// 标记
	virtual void	OnMerchMarkChanged(int32 iMarketId, const CString &StrMerchCode, const T_MarkData &MarkOldData);

	virtual void	OnEscBackFrameMerch(CMerch *pMerch);

	//
	virtual	CMerch*	GetDragMerch();
	virtual	bool32  BeValidDragArea(const CPoint& pt);

public:

	void			ReCreateTabWnd();
	CGuiTabWnd*		GetTabWnd() 				{ return &m_GuiTabWnd;}
	void			AddBlock(const T_BlockDesc& BlockDesc);
	void			OpenBlock(const T_BlockDesc& BlockDesc );
	void			OpenBlock(const T_BlockDesc& BlockDesc, E_MerchReportField eMerchReportField, bool32 bDescSort);
	
	bool32			IsBlockExist(const CString& StrBlockName);
	bool32			SetTabByBlockName(const CString& StrBlockName);
	bool32			SetTabByBlockType(T_BlockDesc::E_BlockType eType);
	
	// 自选股相关
 	void			DelUserBlockMerchByRow(int32 iRow);	
	void			AdjustUserMerchPosition(bool32 bUp);
	void			DragUserMerchPosition();		// 鼠标拖动改变用户商品顺序位置
	bool32			BeUserBlock();					// 当前板块是否是用户板块
	void			SetDragMerchBegin(bool32 bClear=false);	// 设置开始拖动的商品
	void			ExportUserBlockToExcel(CString StrDefaultFileName,CString StrTitle1,CString StrTitle2,CString StrTitle3);

	//
	void			SetGridSelByMerch(const CMerch* pMerch);
	CMerch*			GetGridSelMerch();

	bool32			BeSpecial();

	void			StartAutoPage(bool32 bPrompt = false);		// 开始
	void			StopAutoPage();			// 结束
	bool32			IsAutoPageStarted();	// 是否开始
	static void		SetAutoPageTimerPeriod(int32 iPeriodSecond);
	static int32    GetAutoPageTimerPeriod();
	// from CView	
public:
	// 根据报价表中的商品顺序得到商品切换到前后商品
	bool32			GetPreMerch(IN CMerch *pMerch, OUT CMerch *&pMerchPre);
	bool32			GetAftMerch(IN CMerch *pMerch, OUT CMerch *&pMerchAfter);

	static	const SimpleTabClassMap &GetSimpleTabClassConfig();;

	// from CGuiTabWndCB
protected:
	void			OnRButtonDown2(CPoint pt, int32 iTab);
	bool32			OnLButtonDown2(CPoint pt, int32 iTab);
	bool32			BlindLButtonBeforeRButtonDown(int32 iTab);

///////////////////////////////////////////////////////////////////////
public:
	void			SetTab(int32 iCurTab);

///////////////////////////////////////////////////////////////////////

protected:
	void			OnEsc();
	void			TabChange();
	bool32			ValidBlock(T_BlockDesc& Block);				// 验证这个板块信息
	void			ValidateTableInfo(int32 iIndex = -1);		// 验证每个 Tab 页的信息
	void			SetEditAbleCol();							// 只在自选股的时候,允许代码和名词列可编辑
	void			SaveColoumWidth();
	bool32			TabIsValid(int32& iTab, T_BlockDesc::E_BlockType& eType);
	void			SetTabParams(int32 iTab = -1);	

	void			ReSetGridCellDataByIndex();
	void			ReSetGridCellDataByMerchs(const CArray<CMerch*, CMerch*>& aMerchs); 

	void			ReSetGridContent();

	void			SetRowHeightAccordingFont();							
	void			SetColWidthAccordingFont();

	bool32			SetHeadInfomationList();
	void			UpdateTableContent( BOOL bUpdateAllVisible, IN CMerch *pMerch, bool32 bBlink);

	bool32			GetCurrentDataIndex(int32& iIndexBegin, int32& iIndexEnd);
	void			UpdateLocalBlock(int32 iIndex, const T_BlockDesc& Block);
	void			UpdateLocalXScrollPos(int32 iIndex, int32 iPos);
	void			UpdateLocalDataIndex(int32 iIndex, int32 iBegin, int32 iEnd);

	void			MenuBlockChange();
	void			MenuBlockDel();
	void			MenuBlockInsert();
	void			MenuBlockSetting();
	void			MenuBlockCancel();
	
	void			OnIoViewReportGridLine();
	void			OnIoViewReportMenuDelOwnSel();	
	void			OnIoViewReportMenuDelALLOwnSel();					// 删除所有商品	
	void			OnIoViewReportMenuOwnSelUp();
	void			OnIoViewReportMenuOwnSelDown();

	void			OnMouseWheel(short zDelta, bool32 bMainGrid = FALSE);
	virtual void	OnFixedRowClickCB(CCellID& cell);
	virtual void	OnFixedColumnClickCB(CCellID& cell);
    virtual void	OnHScrollEnd();
    virtual void	OnVScrollEnd();
	virtual void	OnCtrlMove( int32 x, int32 y );
	virtual bool32	OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew );
	virtual void	OnGridMouseMove(CPoint pt);
	virtual bool32	BeBlindSelect();

	virtual	void	UpdatePushMerchs();		// 子类需要添加一些其它的数据进去
	bool32			RequestViewDataSort(bool32 bSendRequest = true);
	bool32			RequestViewDataSortPlugIn(bool32 bSendRequest = true);
	void			RequestViewDataCurrentVisibleRow();
	void			RequestViewDataSortAsync();
	void			RequestViewDataSortTimerBegin();
	void			RequestViewDataSortTimerEnd();
	void			RequestViewDataCurrentVisibleRowAsync();

	// ...fangz 1213 新加
	const T_SimpleTabInfo* GetSimpleTabInfo(int32 iTabIndex);
	
	// 创建出来就初始化好所有的Tabs
	void			InitialTabInfos();	// 初始化自己的tabinfo

	void			ChangeSimpleTabClass(int32 iNewClass);	// 变更当前报价表类型

	void			InitialSimpleTabInfo();
	static bool32		LoadSimpleTabInfoFromXml(OUT SimpleTabClassMap &mapTabClass);
	static bool32       LoadSimpleTabInfoFromCommCfg(OUT SimpleTabClassMap &mapTabClass);
	static void			SaveSimpleTabInfoToXml();

	

	void			DoReportSizeChangeContent();	// 大小改变时，做出适用该大小的调整动作

	void			CalcMaxGridRowCount();		// 根据当前字体与grid高度，计算最大可能的显示行数
	void			ResetTabBeginStartIndex(int32 iTab);	// 重置tab的开始结束数据索引[0-m]

	void			AutoSizeColFromXml();

	bool32			IsInCustomGrid();
	void			InitialCustomMerch();
	void			ReSetCustomGridContent();
	void			UpdateCustomTableContent( BOOL bUpdateAllVisible, IN CMerch *pMerch, bool32 bBlink);
	void			CustomGridMouseWheel(short zDelta);
	bool32			SetCustomHeadInfomationList();
	void			ReSetCustomGridCellDataByIndex(int32 iTab);
	void			ReSetCustomGridCellDataByMerchs(const CArray<CMerch*, CMerch*>& aMerchs); 
	void			UpdateLocalDataIndex2(int32 iBegin, int32 iEnd);
	void			UpdateLocalXScrollPos2(int32 iPos);

	void			ReCalcGridSizeColFromXml();
	void			ReCalcCustomGridSizeColFromXml();

	void			SetPosForScroll(int32 Keyboard);
	void			JudgeScrollVerVisbile(bool bOnSize = false);	// 判断垂直滚动条是否显示(bOnSize在OnSize()中调用)
	bool			ShowGridIndex(int32 index);						// 指定从index为第一行显示
	void			ResetSort();	// 清除列表的排序信息
	void			CreateBtnStock();								//创建创建自选股按钮	
	void			OnBtnResponseEvent(UINT nButtonId);			//自选股按键响应事件
	void            ChangeGridSize(bool32 bHasData = true);
	void			UserBlockPagejump();									// 自选股页面跳转



	void			ReCalcSize();											// 计算位置


	bool32			FromXmlTopBtn( TiXmlElement *pTiXmlElement );			//  读取自定义顶部按钮
	void			InitTopBtn();											//  初始化自定义顶部按钮
	void			TopTabBarHitTest(CPoint point);							//  点击区域
	void			OnTopTabBarResponseEvent(UINT nButtonId);				//  事件响应
	void			OnTopTabBarDisPlateMenu(UINT nButtonId);				//  菜单显示
private:
	map<int32, T_TopTabbar>		m_mapTopTabbarGroup;								// 顶部标签页集合
	map<int32, CNCButton>		m_mapTopTabbarBtn;							// 顶部标签按钮

	BOOL						m_bIsShowTopTabbar;								// 是否显示顶部标签页
	CRect						m_rcTopBtn;
	T_TopTabbar					m_CurTopTabbar;									// 当前板块信息		
	T_SelectBlockInfo			m_CurSelectBlockInfo;							// 当前选择板块信息

public:
	void            ReSetFinanceFlag();
	void			UseSpecialRecordFont();									//	使用列表专用字体
	void            SetIsShowTab(bool32 bShow)  { m_bShowTab = bShow; }

	void            SetReportHeadType(const E_ReportHeadType &eType);    // 改变报价表头
	bool32          IsShowUserBlock() { return m_bIsUserBlock; }
protected:	        
	int32							m_iSortColumn;

	CMmiReqReport					m_MmiRequestSys;			// 普通字段的排行请求
	CMmiReqBlockReport				m_MmiBlockRequestSys;		// 板块排行请求
	CMmiReqMerchSort				m_MmiReqPlugInSort;			// 插件字段的排行请求

	// 表格相关
	CGridCtrlSys					m_GridCtrl;
	CCellID							m_CellRButtonDown;	
	bool32							m_bShowGridLine;
	CXScrollBar						m_XSBVertical;				// 用于控制垂直上下拉动
	CXScrollBar						m_XSBVert;
	CXScrollBar						m_XSBHorz;
	
	// Tab 相关
	CGuiTabWnd						m_GuiTabWnd;
	bool32							m_bLockSelChange;

	E_ReportHeadType                m_eReportHeadType;				// 报价表表头类型

	int32							m_iCurTab;
	int32							m_iIsReadCurTab;
	int32							m_iCurTabTmp;
	int32							m_iPreTab;

	CSize							m_SizeLast;
	int32							m_iMaxGridRowCount;		// 当前最大可能的可视行数
	
	// 每个Tab 的表头信息 (固定列, 表头数组, 滚动条位置)
	int32							m_iFixCol;
	CArray<T_HeadInfo,T_HeadInfo>	m_ReportHeadInfoList;

	//
	bool32							m_bRequestViewSort;
	CGmtTime						m_TimeLast;
	CString							m_StrAsyncExcelFileName;

	E_ReportType					m_eMarketReportType;
	
	// 自选板块的名称
	CStringArray					m_aUserBlockNames;

	bool32							m_bEnableUserBlockChangeResp;	// 允许自选股变更响应，某些情况下不必响应

	// 条件选股报价标志 - 临时
	bool32							m_bIsStockSelectedReport;

	// 条件选股所需要的数据请求包	- 由于需要合并数据，所以移到这里来进行处理
	//CMmiReqMerchIndex				m_mmiReqMerchIndex;			// 选股报价数据请求
	typedef	 map<CMerch *, T_RespMerchIndex>	MerchIndexMap;	
	MerchIndexMap					m_mapMerchIndex;			// 收到的选股数据请求

	typedef map<CMerch *, T_RespMainMasukura>   MainMasukura;	
	MainMasukura                    m_mapMainMasukura;			// 收到的主力增仓请求

	// 每个页面的所有信息
	CArray<T_TabInfo, T_TabInfo&>	m_aTabInfomations;

	int32							m_iAutoPageTimerPeriod;				// 每个报价表单独的自动翻页间隔, 
	int32							m_iAutoPageTimerCount;				// 每秒钟触发一次，到0则触发翻页，如果发现自己和全局不同步，则重设计时器
	bool32							m_bAutoPageDirection;				// 反向，true，向下
	static int32					s_iAutoPageTimerPeriodSecond;		// 所有报价表自动翻页的定时间隔

//	bool32							m_bInitedFromXml;					// 是否已经经过初始化了，避免在create->显示前反复操作，统一在初始化完成后setTab
	
	bool32							m_bAutoSizeColFromXml;				// 配置是否需要自适应列宽
	bool32							m_bSuccessAutoSizeColXml;			// 创建时序的问题, 不一定生效了, 加个标志判断

	T_SimpleTabInfoClass			m_SimpleTabInfoClass;		// 属于这个报价表tab配置
	static	SimpleTabClassMap		sm_mapSimpleTabConfig;		// 配置
	
	//
	CGridCtrlSys					m_CustomGridCtrl;			// 用户自定义商品的显示
	CRect							m_RctCustom;				// 用户自定义商品的显示区域
	CRect							m_RctSperator;				// 两个表格的分割区域
	CXScrollBar						m_XSBVertCustom;
	CXScrollBar						m_XSBHorzCustom;
	int32							m_iMaxCustomGridRowCount;	// customGrid当前最大可能的可视行数
	CNCButton						m_BtnMid;					// 底部指标工具栏
	CNCButton						m_BtnAddStock;				// 底部指标添加自选股
	CNCButton						m_BtnDelStock;				// 底部指标删除自选股
	CNCButton						m_BtnCleanStock;			// 底部指标清空自选股
	CNCButton						m_BtnImportStock;			// 底部指标导入自选股
	CNCButton						m_BtnExportStock;			// 底部指标导出自选股
	CArray<CNCButton,CNCButton>		m_BtnStockList;				// 自选股功能按钮列表				
	bool32							m_bShowCustomGrid;			// 显示用户自定义商品
	bool32							m_bShowUserBlock;			// 显示用户自选股模式
	bool32							m_bIsUserBlock;				// 是否为自选模块，此标志和m_bShowUserBlock组合使用

	bool32                          m_bShowTextTip;				// 选股无数据文字提示
	bool32							m_bCustomGrid;				// 
	int32							m_iMarketID;				// 默认显示的市场id

	T_BlockDesc						m_blockCustom;
	T_TabInfo						m_tabCustom;

	bool32							m_bIsShowGridVertScorll;	// 控制是否显示垂直滚动条
	bool32							m_bShowTab;				// 是否显示tab标签
	bool32                          m_bShowTabByTabbar;		// 只有m_bShowTab=false时有效
	CRect							m_RectVert;				// 垂直滚动条的位置
	CRect							m_RectGrid;				// 保存表格的位置

	int32							m_sortType;				// 用于标识点击列表头时的排序类型

	bool32							m_bDragZxMerch;
	bool32							m_bDragMerchLBDown;
	CMerch							*m_pDragMerchBegin;		// 开始拖动的商品
	int32							m_iRowDragBegin;		// 开始拖动的行		
	int32							m_iDragGridType;		// 拖动的表格类型, 同一表格才能拖动(0,自选； 1，热门商品)

public:
	bool32							m_bLockReport;			// 报价表锁定后就不能通过工具栏切换市场
	int32                           m_iSelStockTab;			// 当前标价表智能选股标签位置
	bool32                          m_bShowMoreColumn;         // 是否显示更多属性列
friend class CMainFrame;
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewReport)
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewReport)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	// afx_msg LRESULT OnMsgHotKey(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLayOutAdjust();	
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnAutoPage(UINT nId);
	afx_msg void OnDoInitFromXml();
	//}}AFX_MSG
	afx_msg void OnMenuAddOwnSel(UINT nID);
	afx_msg void OnMenu( UINT nID );
	afx_msg void OnIoViewReportMenuBlockMoreLBtnDown( UINT nID );
	afx_msg void OnIoViewReportMenuBlockMoreRBtnDown( UINT nID );
	afx_msg void OnIoViewReportMenuTabCollection( UINT nID );
	afx_msg void OnIoViewReportMenuTabTopCollection( UINT nID );
	afx_msg void OnMenuExport ( UINT nID );
	afx_msg void OnMenuIoViewReport( UINT nID );	
	afx_msg void OnGridLButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg LRESULT OnMessageTitleButton(WPARAM wParam,LPARAM lParam);
	//linhc 20100909添加
    afx_msg void OnGridSelRowChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	//--- wangyongxue 2016/0829
	afx_msg LRESULT OnMessageRisePercent(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnMessageFinance(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
friend class CGridCtrlSortCB;
virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_REPORT_H_
