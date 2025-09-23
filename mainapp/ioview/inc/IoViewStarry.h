#ifndef _IO_VIEW_STARRY_H_
#define _IO_VIEW_STARRY_H_
#pragma warning(disable: 4786)
#include <map>
using	std::map;
#include <vector>
using	std::vector;
#include <set>
using	std::set;

#include "IoViewBase.h"
#include "MerchManager.h"
#include "BlockManager.h"
#include "ReportScheme.h"
#include "GridCtrlSys.h"
#include "GuiTabWnd.h"
#include "XScrollBar.h"
#include "tinyxml.h"
#include "ioviewshare.h"
#include "DlgTimeWait.h"
#include "BlockConfig.h"

struct T_MerchAxisNode
{
	CMerch	*m_pMerch;
	CReportScheme::E_ReportHeader	m_eHeader; // 当前使用的字段
	
	T_MerchAxisNode();
	operator double();
};



// 星空图
class CIoViewStarry : public CIoViewBase,  public CGuiTabWndCB, public CBlockConfigListener
{
// Construction 
public:
	
	virtual ~CIoViewStarry();

	DECLARE_DYNCREATE(CIoViewStarry)

public:
	typedef	CArray<CMerch *, CMerch *>	MerchArray;
	typedef CReportScheme::E_ReportHeader	AxisType;

public:
	// from CIoViewBase
	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);		// 实现Xml存储
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	
	virtual void	OnIoViewActive();						
	virtual void	OnIoViewDeactive();
	virtual bool32	GetStdMenuEnable(MSG* pMsg){return false;};

	virtual void	LockRedraw();
	virtual void	UnLockRedraw();

	virtual E_IoViewType GetIoViewType() { return EIVT_MultiMerch; }
	virtual DWORD		GetNeedPushDataType() { return 0; }	// 不需要任何推送数据
	// from CIoViewBase			
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();		// 怎么解决市场初始话呢- -
	virtual void	OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs);
	virtual void	OnVDataGridHeaderChanged(E_ReportType eReportType);
	virtual void	OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType);	// 财务数据更新
	

	virtual void	OnIoViewColorChanged();			
	virtual void	OnIoViewFontChanged();

	// BlockConfig
	virtual void	OnBlockConfigChange(int32 iBlockId, E_BlockNotifyType eNotifyType);		// 板块数据变更
	virtual	void	OnConfigInitialize(E_InitializeNotifyType eInitializeType);				// 整个板块配置变更

	// TabCB - 先用着这个tab吧
	virtual void	OnRButtonDown2(CPoint pt, int32 iTab);
	virtual bool32  OnLButtonDown2(CPoint pt, int32 iTab);	
	virtual bool32	BlindLButtonBeforeRButtonDown(int32 iTab){return false;};	// 没法判断哪个tab的- -
public:
	
	void	DoCmdUIUpdate(CCmdUI *pUI);

	bool32	SetNewBlock(int32 iBlockId, bool32 bReOpen = false, bool32 bRequestData = true);

private:
	enum E_Coordinate{
		COOR_Normal,	// 普通坐标
		COOR_Logarithm,	// 对数坐标
		COOR_END			
	};

private:
	struct T_XYAxisType;

	void	SetNewBlock(const T_Block &block, bool32 bRequestData = true);

	// grid显示
	void	EmptyGridContent();	// 只显示一个框架 和 block内容
	void	UpdateBlockGrid();	// grid显示 - 显示当前block信息
	void	UpdateBlockPaintMerchCount();	// 显示画面星星数量
	void	UpdateMouseMerch(CPoint ptMouse); // 有可能为空
	void	UpdateSelectedRange(CPoint ptMouse);	// 框选范围显示
	void	UpdateSelectedMerchs();	// 框选的商品显示
	int32	InsertUpdateSelectedMerch(CMerch *pMerch);	// 添加一个商品到选择的商品中 加入选择商品列表并显示 - -1失败
	void	UpdateSelectedMerchValue(CMerch *pMerch); // 更新选择列表中商品的数据

	void	CalcXYAllMinMaxAxis(bool32 bNotifyChange = false);		// 重算xy轴 - 根据现有商品，重算XY min max，如果以后框选的画，可以通过直接修改 min max来更改范围
	void	CalcXYMinMaxAxis(CMerch *pMerch, bool32 bNotifyChange = false, bool32 *pbMinMaxChanged = NULL);
	void	UpdateXYMinMax(double fxMin, double fxMax, double fyMin, double fyMax, bool32 bNotifyChange = false);
	void	ChangeAxisType(AxisType eNewXType, AxisType eNewYType);		// 如果该轴没有改变的话，用m_e?Type的值作为eNew?Type
	void	TabXYSelChange();		// xy轴tab条的选择改变
	AxisType	GetXAxisType(OUT CString *pStrName = NULL);
	AxisType	GetYAxisType(OUT CString *pStrName = NULL);
	CString		GetStarryType();
	CString		GetUnitString(double fScale);	// 获取单位描述字串

	void	SetCoordinate(E_Coordinate eCoor, bool32 bNotifyChange = true);		// 设置普通/对数坐标类型
	bool32	GetDivededValueByClientPoint(IN CPoint pt, OUT double *pfx, OUT double *pfy);	// 获取除掉单位后的实际值
	bool32	CalcXYValueByClientPoint(IN CPoint pt, OUT double *pfx, OUT double *pfy);		// 计算坐标点对应的实际值
	bool32	CalcClientPointByXYValue(IN double fx, IN double fy, OUT CPoint &ptClient);	// 计算坐标值与client x,y映射
	
	bool32	CalcClientPointByXYValueNormal(IN double fx, IN double fy, OUT CPoint &ptClient);		// 普通坐标计算
	bool32	CalcXYValueByClientPointNoraml(IN CPoint pt, OUT double *pfx, OUT double *pfy);	// 计算坐标点对应的实际值除以单位刻度后的值
	bool32	CalcClientPointByXYValueLogarithm(IN double fx, IN double fy, OUT CPoint &ptClient);	// 对数坐标计算
	bool32	CalcXYValueByClientPointLogarithm(IN CPoint pt, OUT double *pfx, OUT double *pfy);
	

	void	DrawActiveFlag(CDC &dc);
	int		GetProperChartGridCellSize(OUT double &cx, OUT double &cy, OUT int32 *pColCount = NULL, OUT int32 *pRowCount = NULL);	// 获取chart中每个单元格适当的长宽，返回能够存在的单元格数量
	void	DrawXAxis(CDC &dc);
	void	DrawYAxis(CDC &dc);
	void	DrawChart(CDC &dc);		// 提供单独画商品的？
	void	DrawMerch(CMerch *pMerch, CDC *pDC);	// 绘制单独的商品，与DrawChart区别开 - 暂时不用
	void	DrawMerchAlreadyDrawed(CMerch *pMerch, CDC &dc, int32 iRadius, bool32 bSpecifyColor = false, COLORREF clrSpecify = RGB(255,255,0));	// 不绘制新商品，只绘制在onPaint中计算了矩形的商品
	void	DrawMouseMove(CPoint ptMouse);			// 绘制临时的东西
	void	DrawZoomSelect(CDC &dc, CPoint ptEnd);

	bool32	GetMerchDrawRect(CMerch *pMerch, OUT CRect &rcMerch);
	bool32	CalcDrawMerch(CMerch *pMerch);				// 计算单个商品绘制数据
	int32	CalcDrawMerchs(const MerchArray &aMerchs);	// 计算一组商品的绘制数据

	CDC		&BeginMainDrawDC(CDC &dcSrc);				// paint内存dc
	void	EndMainDrawDC(CDC &dcDst);
	CDC		&BeginTempDraw();
	void	EndTempDraw();
	void	MyDCBitBlt(CDC &dcDst, CDC &dcSrc);

	bool32	CalcMerchXYValue(CMerch *pMerch, OUT double *pfXValue, OUT double *pfYValue);
	double	CalcMerchValueByAxisType(CMerch *pMerch, AxisType axisType, bool32 *pBCalced = NULL);
	bool32	GetMerchRealTimePrice(CMerch *pMerch, OUT CRealtimePrice &RealTimePrice);	// 获取该视图缓存的或者直接real price
	bool32	GetMerchFinanceData(CMerch *pMerch, OUT CFinanceData &FinanceData);
	CMerch *GetMouseMerch(CPoint pt);			// 可能返回NULL
	bool32	TestMerchInChart(CMerch *pMerch, OUT int32 &xClient, OUT int32 &yClient);	// 计算商品是否在可视范围内
	bool32	CalcMerchRect(CMerch *pMerch, OUT CRect &RectMerch);

	void	UpdateAllContent();
	
	void	RefreshBlock();

	bool32	AskUserBlockSelect(OUT T_Block &block, OUT int32 &iBlockId, int32 iCurTab, CPoint ptScr);

	void	RequestViewData();
	void	RequestViewData(CArray<CSmartAttendMerch, CSmartAttendMerch&> &aRequestSmarts);

	void	AddAttendMerchs( const MerchArray &aMerchs, bool bRequestViewData = true );
	void	AddAttendMerch( CMerch *pMerch, bool bRequestViewData = true );
	void	ClearAttendMerchs();
	void	RemoveAttendMerch(CMerch *pMerch, E_DataServiceType eSerivceRemove = EDSTPrice);
	void	RequestBlockViewData();				// 整个请求 - 第一次使用
	void	RequestBlockViewDataByQueue();		// 使用队列一组一组请求
	void	RequestBlockQueueNext();			// 完成该商品请求, 队列中没有请求的继续请求
	void	RequestBlockQueueNextAsync();			// 完成该商品请求, 队列中没有请求的继续请求
	void	RequestViewData(CMmiCommBase &req);

	void	ChangeRadiusByKey(bool32 bAdd);
	
	void	ConvertIntoChartPoint(IN CPoint pt, OUT CPoint &ptInChart);		// 将chart外的坐标转为chart内的坐标
	bool32	IsPtInChart(CPoint pt);

	bool32	GetZoomRect(OUT CRect &rcZoom);		// 返回是否在chart内
	bool32	CalcZoomRect(CPoint ptLast, OUT CRect &rcZoom);	// 计算zoom区域，如果没有开始zoom返回false
	void	EnsureZoom();
	void	CacelZoom();
	void	CancelZoomSelect(CPoint ptMouse, bool32 bDraw = true);
	void	EnsureZoomSelect(CPoint ptMouse);

	void	BlinkUserStock();		// 闪烁/不闪烁自选股
	void	UpdateUserBlockMerchs();		// 当前板块有哪些商品是自选股
	void	DrawBlinkUserStock(CDC *pDC = NULL);

	void	DoMerchSelected(CMerch *pMerch, bool32 bNeedDraw = true);
	void	DoDbClick(CPoint ptClick);		// 跟Report类似
	void	DoDbClick(CMerch *pMerchClick);

	int32	SelectUserAxisType(CPoint ptMenu, INOUT T_XYAxisType &axisType);	// 用户选择自定义坐标轴，返回用户选择X(1), Y(2), 没有选择(0)
	void	MultiColumnMenu(IN CMenu &menu);	// 设置多列菜单

	void	RecreateBlockTab();

	void	RecalcLayout();

	bool32	IsFinanceDataType(CReportScheme::E_ReportHeader eHeader);
	bool32	IsNeedFinanceDataType();

	bool32	IsMerchNeedReqData(CMerch *pMerch);

private:
	CIoViewStarry();
	CIoViewStarry(const CIoViewStarry&);
	CIoViewStarry &operator=(const CIoViewStarry &);

	typedef set<CMerch *> MerchSet;

	int32		m_iBlockId;
	T_Block		m_block;	// 当前关注板块
	MerchSet	m_MerchsCurrent;	// 当前商品集合
	CArray<int32, int32>	m_aBlockCollectionIds;

	bool32		m_bFirstReqEnd;	// 第一次请求数据是否结束

	struct T_XYAxisValue{
		CRealtimePrice m_realTimePrice;		
		//CFinanceData  m_financeData;		// 财务数据始终取merch中的
		T_XYAxisValue(){
			
		}
		void Reset(){
			m_realTimePrice = CRealtimePrice();
		}
		BOOL IsValid(){
			return m_realTimePrice.m_fPriceNew > 0.0;
		}
	};

	typedef map<CMerch *, T_XYAxisValue>	MerchValueMap;
	// 因为这个视图并不在所有的时间关心当前板块数据, 有可能别的视图导致这个视图并不关心的real price更新了，所以视图内部在计算x,y时，将组成xy的price存起来
	// 如果以后有其它非realtime数据加进来，同样可能出现该问题
	MerchValueMap	m_mapMerchValues;			// 缓存商品所有这个视图获取的realtime price，避免其它视图导致的更新real price，而这个视图并没有设置smart merch

	
	bool32		m_bInZoom;		// 在缩放状态 - 注意取消缩放
	CPoint		m_ptZoomStart;		// 选定时的XY窗口坐标
	CPoint		m_ptZoomEnd;

	CGuiTabWnd	m_wndTabBlock;	// 底侧左右的Tab条
	CGuiTabWnd	m_wndTabXYSet;

	CGridCtrlSys	m_GridCtrl;		// 右侧使用表格来显示数据 - 显示上半部分板块的数据 2列
	CGridCtrlSys	m_GridCtrlZoomDesc;	// 显示框选描述段的数据 1列
	CGridCtrlSys	m_GridCtrlZoomMerchs;	// 框选内的商品 2列
	CXScrollBar		m_wndScrollV;

	// 表格数据有关
	CMerch			*m_pMerchLastMouse;				// 最后一个鼠标接触的商品
	CRect			m_RectLastZoomUpdate;			// Grid中最后一次关于有关框选商品更新的矩形
	CMerch			*m_pMerchLastSelected;			// 用户通过表格选择的商品
	MerchArray		m_aZoomMerch;					// 最终框选的商品 - 只有框选结束时才更新

	// 画图有关
	int32		m_iRadius;		// 小原点的半径

	CRect		m_RectLeftY;	// Y轴
	CRect		m_RectBottomX;	// X
	CRect		m_RectChart;	// 图区域

	CRect		m_RectGrid;		// 表格区域

	double		m_fMinY;	// 现只有线性坐标
	double		m_fMaxY;
	double		m_fMinX;
	double		m_fMaxX;
	double		m_fScaleX;		// X轴基本单位
	double		m_fScaleY;		// Y轴基本单位
	bool32		m_bValidMinMaxX;		// 是否为有效X/Y轴最大最小值
	bool32		m_bValidMinMaxY;

	E_Coordinate	m_eCoordinate;

	// XML相关

	// 当前关注的商品矩形
	typedef	map<CMerch *, CRect>	MerchRectMap;
	MerchRectMap	m_mapMerchRect;

	// 请求队列
	typedef	CList<CMerch *, CMerch *> MerchList;
	MerchList		m_lstMerchsWaitToRequest;

	struct T_XYAxisType{
		AxisType	m_eXType;	// 暂时以report header作为坐标选值
		AxisType	m_eYType;
		CString		m_StrName;		// 描述性名字
		T_XYAxisType(){
			m_eYType = CReportScheme::ERHPriceNew;
			m_eXType = CReportScheme::ERHCircAsset;
			m_StrName = _T("价盘");
		}
		
		bool operator==(const T_XYAxisType &axis)
		{
			return m_eXType == axis.m_eXType && m_eYType == axis.m_eYType;
		}

		bool operator!=(const T_XYAxisType &axis)
		{
			return !(*this == axis);
		}
	};
	typedef	CArray<T_XYAxisType, const T_XYAxisType &>	XYAxisTypeArray;
	XYAxisTypeArray		m_aXYTypes;
	T_XYAxisType			m_axisCur;			// 当前的坐标轴 - 名字为选择的名字
	T_XYAxisType			m_axisUserCur;		// 当前自定义坐标轴选择 - 始终为自定义


	// 其它
	CDlgTimeWait		m_dlgWait;
	MerchArray			m_aMerchsNeedInitializedAll;		// 需要初始化商品 - 仅在初始化时有用
	bool32				m_bMerchsRealTimeInitialized;

	CBitmap				m_bmpImage;		// 保存镜像位图
	CDC					m_dcMemPaintDraw;
	CBitmap				m_bmpTempDraw;
	CDC					m_dcMemTempDraw;	// 临时dc
	CPoint				m_ptLastMouseMove;

	MerchArray			m_aMerchsNeedDraw;			// 需要定时器绘制的数据变化了的商品

	bool32		m_bUseCacheRealTimePrices;		// 使用缓存的商品报价计算图形

	bool32		m_bIsIoViewActive;
	
	bool32		m_bBlinkUserStock;						// 闪烁自选股
	bool32		m_bBlinkUserStockDrawSpecifyColor;		// 标志下一次是否使用特定颜色来绘制自选股
	MerchArray	m_aUserStocks;							// 当前板块属于自选股的商品

	typedef map<CMerch *, CGmtTime>	MerchReqTimeMap;
	MerchReqTimeMap		m_mapMerchReqWaitEchoTimes;		// 商品发出了请求，等待返回的列表
	MerchReqTimeMap		m_mapMerchLastReqTimes;			// 商品最后发出了请求的列表
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewStarry)
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewStarry)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSelChange(UINT nId, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM w, LPARAM l);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT	 OnDoInitializeAllRealTimePrice(WPARAM w, LPARAM l);	// 初始化所有商品的行情
	afx_msg void OnUpdateXYAxis();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMerchSelectChange(NMHDR *pNotifyStruct, LRESULT *pResult);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg void OnMenuStarry(UINT nId);
	afx_msg LRESULT OnMsgHotKey(WPARAM w, LPARAM l);
	//}}AFX_MSG

	
 	DECLARE_MESSAGE_MAP()
};

#endif //_IO_VIEW_STARRY_H_