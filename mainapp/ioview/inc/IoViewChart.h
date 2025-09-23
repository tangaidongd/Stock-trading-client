#ifndef _IO_VIEW_CHART_H_
#define _IO_VIEW_CHART_H_

#include "XScrollBar.h"
#include "XTipWnd.h"
#include "IoViewBase.h"
#include "IoViewShare.h"
#include "CFormularContent.h"
#include "MerchManager.h"
#include "UserBlockManager.h"
#include "GridCtrlSys.h"
#include "MemdcEx.h"
#include "IoViewShare.h"
#include "PolygonButton.h"
#include "DlgPullDown.h"

///////////////////////////////////////////////////////////////////////////////
class CGridCellSys;
class CChartRegionViewParam;
class CRegionDrawNotify;

//
// extern const int32 KiMenuIDIntervalStatistic;
// 
// extern const int32 KiMenuIDIntervalStatistic;
// extern const char* KIoViewChartKLineInterval;
// extern const char* KIoViewChartUserMinutes;
// extern const char* KIoViewChartUserDays;

#define  TOP_MERCH_HEIHGT  33 
#define  TOP_BUTTON_HEIHGT 21

// 

extern const int32 KiMenuIDIntervalStatistic;
// CIoViewChart
class CIoViewChart : public CIoViewBase, public CGridCtrlCB, public CUpdateShowDataChangeListener,public IChartRegionData
{
public:
	enum E_IoViewChartFlag
	{
		KRegionCreated			= 0x00000001,
		KRegionSplited			= 0x00000002,
		KRegionReady			= 0x00000008,			// SplitRegion() 划分好Region 的区域后, 才能响应右键菜单相关操作
		
		KMerchKLineUpdate		= 0x00000010,
		KMerchTimeSalesUpdate	= 0x00000020,
	};

	enum E_SCROLL_DIRECT
	{
		ESD_know = 0,
		ESD_left = 1,
		ESD_right = 2,
	};
	
	struct T_SpecialBtn
	{
		CString StrName;
		CString StrMerchCode;
		int32   iMarketId;


		T_SpecialBtn()
		{
			StrName = L"";
			StrMerchCode = L"";
			iMarketId   = 0;
		}
	};

	//	add by weng.cx, for limit merch change
	struct T_LimitMerch
	{
		int		iMarketId;
		CString tMerchCode;
		CString tName;
		T_LimitMerch()
		{
			iMarketId = 0;
			tMerchCode = L"";
			tName = L"";
		}		
	};

	struct T_CycleMenuParam
	{
		UINT	uID;
		CString StrName;
		Image   *pImage;

		T_CycleMenuParam(UINT uId, CString strName, Image *pImg)
		{
			uID		= uId;
			StrName = strName;
			pImage  = pImg;
		}
	};

	struct T_IndexGroup
	{
		CString		StrIndexName;				// 指标名称
		bool32		bRegionMainIndex;			// 是否主图指标
		int32		iRegion;					// 指标下标值


		T_IndexGroup()
		{
			StrIndexName = L"";
			bRegionMainIndex = false;
			iRegion = 0;
		}
	};


	struct T_SpecicalIndexAttributes
	{
		CString		StrIndexName;				// 指标名称
		int32		iOpenCfm;					// 是否打开版面
		int32		iBtnWidth;					// 指标按钮宽度
		CString     strCharacter;				// 指标按钮字体名称
		int32		iBlod;						// 指标按钮字体是否加粗
		float		fSize;						// 指标按钮字体大小
		CString     strNomarlBKColor;			// 正常背景色
		CString     strHotBKColor;			    // 正常激活的背景色
		CString     strPressBKColor;			// 按钮按下的背景色
		CString     strNomarlTextColor;			// 字体正常颜色
		CString     strHotTextColor;			// 字体激活颜色
		CString     strPressTextColor;			// 字体按下颜色	
		T_SpecicalIndexAttributes()
		{
			StrIndexName = L"";
			iOpenCfm	 = -1;
			iBtnWidth    = -1;
			strCharacter = L"";
			iBlod        = -1;
			fSize		 = -1.0;
			strNomarlBKColor	= L"";
			strHotBKColor		= L"";
			strPressBKColor		= L"";
			strNomarlTextColor	= L"";
			strHotTextColor		= L"";
			strPressTextColor	= L"";
		}
	};

public:
	CIoViewChart();
	virtual ~CIoViewChart();
	
	DECLARE_DYNCREATE(CIoViewChart)

//////////////////////////////////////////////////////////////////
	// from CIoViewBase
public:
	virtual bool32	FromXml(TiXmlElement * pTiXmlElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();
	virtual bool32	GetStdMenuEnable(MSG* pMsg);
	virtual BOOL IsKindOfIoViewChart() const {return TRUE;}
	// 增加额外IoViewChart提供给子类的特殊XML初始化与保存
	virtual bool32  FromXmlInChild(TiXmlElement *pTiXmlElement) { return true; }
	virtual CString ToXmlInChild() { return CString(); }		// 子类保存xml属性
	virtual CString GetDefaultXMLInChild() { return CString(); }
	virtual CString	ToXmlEleInChild() { return CString(); }		// 子类保存xml元素
	virtual int32 GetChartType(){return m_iChartType;}
	
public:		
	// from CIoViewBase
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual bool32	OnVDataAddCompareMerch(IN CMerch *pMerch);
	virtual void	OnVDataRemoveCompareMerch(IN CMerch *pMerch);
	virtual void	OnVDataClearCompareMerchs();
	virtual void	SetChildFrameTitle(){};
	
	virtual void	OnIoViewColorChanged();
	virtual void	OnIoViewFontChanged();
	virtual void	OnVDataFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName );

	// from CGridCtrlCB
	virtual void	OnFixedRowClickCB(CCellID& cell){}
	virtual void	OnFixedColumnClickCB(CCellID& cell){}
    virtual void	OnHScrollEnd(){}
    virtual void	OnVScrollEnd(){}
	virtual void	OnCtrlMove( int32 x, int32 y );
	virtual bool32	OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew ){return true;}

	// 辅助判断显示区间是否变更的接口
	virtual void    OnShowDataTimeRangeChanged(T_MerchNodeUserData *pData){}

///////////////////////////////////////////////////////////////
// 基类虚接口
protected:
	virtual	void	CalcLayoutRegions(bool bOnlyUpdateMainRegion = true) {}
	virtual void	CalcReginMinMax(CChartRegion* pRegion){}
	virtual void	Draw();
	virtual void	CreateRegion();
	virtual void	SplitRegion();
	virtual bool32	AddSubRegion( bool32 bSilence = true, bool32 bNotify=true );
	virtual bool32	DelSubRegion(CChartRegion* pRegion, bool32 bEnableChangeToPick = true);
  	virtual T_IndexParam*	AddIndex ( CChartRegion* pRegion,CString StrIndexName, bool32 bMainIndex = TRUE);
	virtual void	UpdateAxisSize( bool32 bPreDraw = true );
	virtual void	DoFromXml();
	virtual void	InitialShowNodeNums(){};
	virtual void	SetXmlSource (){};
	virtual void	SetCurveTitle ( T_MerchNodeUserData* pData ){};

	virtual void	InitCtrlFloat ( CGridCtrl* pGridCtrl );
	virtual void	SetFloatData ( CGridCtrl* pGridCtrl,CKLine& KLine,float fPricePrevClose,float fCursorValue,CString StrTimeLine1,CString StrTimeLine2,int32 iSaveDec){};
	virtual void	SetFloatData ( CGridCtrl* pGridCtrl,float fCursorValue,int32 iSaveDec){}
	virtual void	ClipGridCtrlFloat ( CRect& Rect ){};
	virtual void	RequestNodeInit ( T_MerchNodeUserData* pData){}

	virtual void	DrawContentExtraYLine(CMemDCEx* pDC, CChartRegion* pRegion, CFormularContent* pContent);
	virtual void	DrawTitle1 ( CMemDCEx* pDC );
	
	virtual void    OnKeyHome(){}
	virtual void    OnKeyEnd(){}
	virtual void    OnKeyUp(){}
	virtual void	OnKeyDown(){}
	virtual void	OnKeyLeft(){}
	virtual void	OnKeyRight(){}
	virtual void	OnKeyLeftAndCtrl(){}
	virtual void    OnKeyLeftAndCtrl(int32 iRepCnt);
	virtual void	OnKeyRightAndCtrl(){}
	virtual void    OnKeyRightAndCtrl(int32 iRepCnt);
	virtual void	OnKeySpace();

    void            SetTopButtonStatus(E_NodeTimeInterval eNodeInterval, int32 iValue);

public:
	virtual void	AddIndexGroup(CString strIndexGroupName) {};
	virtual void	SetSelectIndexGroupName(CString strSelectIndexGroupName);

public:
	// 获取副图的标题栏按钮:
	virtual int32	GetSubRegionTitleButtonNums() { return 0; }
	virtual const T_FuctionButton*	GetSubRegionTitleButton(int32 iIndex) { return NULL; }
	
	// 代码优化 - 获取X轴slider提示，减轻CalcXAxis负担
	virtual bool32	GetChartXAxisSliderText1(OUT CString &StrSlider, CAxisNode &AxisNode);

	// 框选放大

	// 区间统计

	// 获取YLeft适合显示的字符个数， iWideCharCount 为 需要 调整的汉字等宽字符的数量，默认汉字为 eng 的两倍
	virtual int32   GetYLeftShowCharWidth(int32 iWideCharCount = 0) const;

	// 添加叠加商品的简单接口
	virtual bool32 AddCmpMerch(CMerch *pMerchToAdd, bool32 bPrompt, bool32 bReqData){ return false; };
	virtual void				RemoveCmpMerch(CMerch *pMerch){};
	virtual void				RemoveAllCmpMerch(){};
	//
	virtual bool32	GetAdvDlgPosition(IN int32 iWidth, IN int32 iHeight, OUT CRect& rtPos){return false;}
	virtual bool32  IsAlreadyToShowAdv(){return false;}

	virtual	void	GotoCompletePrice() {}
	virtual	void	SetTrendMultiDay(int iDay, bool32 bClearData, bool32 bReqData){};	// 设置多日分时
	virtual int32   GetTrendMultiDay(){return 1;};
	virtual void	ClearRegionIndex(CChartRegion *pRegion, bool32 bDelExpertTip = false){}; // 清空指定region的指标
	virtual bool32	AddShowIndex(const CString &StrIndexName, bool32 bDelRegionAllIndex = false, bool32 bChangeStockByIndex = false, bool32 bShowUserRightDlg=false){return true;};

	virtual void   SetAvgPriceEvent(){};
////////////////////////////////////////////////////////////
public:
	bool32			LoadAllIndex(CNewMenu* pMenu);
	void			UpdateSelfDrawCurve ();
	void			RemoveAllSelfDrawCurve();
	bool32			DeleteCompareCurve ( CChartCurve* pCurve );

	void			RegionMenu2 ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y );
	void			RegionIndexMenu ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y );
	void			RegionIndexBtn(CChartRegion* pRegion, CDrawingCurve* pCurve,int32 iID);
	void			MouseWheelLoopMerch(UINT nFlags, short zDelta, CPoint pt);
	CString			BaseFloat2String(float fValue,bool32 bZeroAsHLine, bool32 bNeedTerminate = false);
	void			ShowIndex();
	void			ShowIndexParam();
	void			ShowIndexHelp();
	void			Add2Block(const CString &StrBlockName);
	bool32			IsRegionReady();
	int32			ExistAttendMerch (IN CMerch *pMerch);
	T_MerchNodeUserData*	NewMerchData (IN CMerch *pMerch, bool32 bMainMerch, bool32& bInhert );
	bool32			DeleteMerchData ( int32 iPos );
	bool32			DeleteIndexParamData ( T_IndexParam* pParam);
	void			RemoveIndexsNodesData ( T_MerchNodeUserData* pData );
	bool32			DeleteIndexCurve ( CChartCurve* pCurve );
	void			DeleteRegionCurves ( CChartRegion* pRegion );
	void			DragDropCurve ( CChartCurve* pSrcCurve,CChartRegion* pDestChart);
	void			InsertEmptyNodes ( T_MerchNodeUserData* pData,bool32 bFull = true );
	void			GetNodeBeginEnd ( int32& iNodeBegin,int32& iNodeEnd );

	void			UpdateRegionSelfDrawCurve ( CChartRegion* pRegion );
	void			DeleteRegionSelfDrawCurve ( CChartRegion* pRegion);

	void			DrawIndexExtraY ( CMemDCEx* pDC,CChartRegion* pRegion );
	CString			GetChartGuid ( CChartRegion* pRegion );
	void			SliderId ( int32& id, int32 iJump );
	void			OnExport();

	void			DrawInterval(CMemDCEx* pDC, CChartRegion* pRegion);

	void			CalcTodayRise(IN CMerch *pMerch);
	bool32			GetRisFallFlag()	{ return m_bTodayRise; }
	void			AddDrawBaseRegion(CMemDCEx* pDC);
	void			AddDrawCornerRegion(CMemDCEx* pDC);

	void			HideAllTips();		// 隐藏所有tip
					
	void			SetPickedSelfDrawCurve(CSelfDrawCurve* pCurve);
	CSelfDrawCurve* GetPickedSelfDrawCurve();
	
public:
	bool32			BeVolIndex(const CString& StrIndexName,CStringArray& aIndexs);
	void			SortIndexByVol(IN CStringArray& aIndex,OUT int32& iIndex);
	void			SortIndexByAlpha(CStringArray& aIndex); //2013-5-4 按字母排序
	virtual BOOL IsKindOfIoViewKLine();
	T_IndexParam*	FindIndexParamByCurve ( CChartCurve* pCurve );
	void			FindCurvesByIndexParam ( T_IndexParam* pParam,CArray<CChartCurve*,CChartCurve*>& Curves );
	T_IndexParam*	FindIndexParamByName ( CString StrName );

	void			FindRegionFormula ( CChartRegion* pRegion, CStringArray& arr );
	void			DeleteSameString ( CStringArray& arr1,CStringArray& arr2);

	void			FormulaParamTempUpdate ( CString StrName );
	void			FormulaUpdate ( CString StrName );
	void			FormulaDelete ( CString StrName );
	int32			GetDigit(int32 iNum);
	int32			GetInter(int32 iNum);
	int32			MakeMinBigger(int32 iMin, int32 iStep);
	void			OnKeyLeftRightAdjustIndex();
	void			SetGridFloatInitPos();
	void			AdjustGridFloatSize();
	void			SetMainCurveAxisYType(CPriceToAxisYObject::E_AxisYType eType);	// 主图的y坐标类型
	CPriceToAxisYObject::E_AxisYType GetMainCurveAxisYType() const { return m_eMainCurveAxisYType; }

	// 简单叠加商品有关
	void			OnCmpMerchsNeedUpdateAsync();	// 所有叠加商品数据需要更新重计算, Timer时计算
	void			OnCmpMerchsNeedUpdate(bool32 bNeedDraw = true);		// 调用叠加商品数据进行更新
	void			DoCmpMerchsUpdate(bool32 bNeedDraw = true);			// 现有需要更新的叠加商品更新
	int32			RemoveCmpMerchFromNeedUpdate(CMerch *pMerch);	// 从叠加商品待更新队列中移出

	bool32			SetAutoAddExpCmpMerch(bool32 bEnable, bool32 bPrompt=true);	// 返回是否成功设置状态
	bool32			IsAutoAddExpCmpMerch()const { return m_bAutoAddCmpExpMerch; }

	void			EnableRightYRegion(bool32 bEnable);		// 允许右Y轴显示, 不绘制
	bool32			IsEnableRightYRegion() const { return m_bEnableRightYRegion; }
	bool32          IsEnableLeftYRegion() const { return m_bEnableLeftYRegion; }

	// 锁柱十字光标内容的更新 (存在十字光标的时候放大. 这时候不能更新当前的十字光标信息, 除非鼠标移动了)
	void			LockCrossInfo(bool32 bLock) { m_bLockCross = bLock; }
	bool32			GetCrossLockInfo() const	{ return m_bLockCross; }

	static	bool32	IsShowVolBuySellColor();	// 是否显示成交量的买卖颜色信息(红绿柱成交量)
	static  void	SetShowVolBuySellColor(bool32 bShow);	// 设置

	void            InitChartHideMerchButton();             // 当不显示商品名称时，刷新按钮显示

protected:
	// 清除掉当前的本地数据
	virtual	void	ClearLocalData(bool32 bClearAll){}
	void			ReDrawAysnc();

	void			AddSelfDrawNodesFromXML();

public:
	// 区间统计相关
	void			SetIntervalTime(const CGmtTime& Time);
	void			SetIntervalTime(const CGmtTime& tmBegin, const CGmtTime& tmEnd);

	void			SetIntervalBeginTime(const CGmtTime& Time);
	void			SetIntervalEndTime(const CGmtTime& Time);
	
	void			ClearIntervalTime();

	HCURSOR			SetNewCursor(HCURSOR hCursor);

	virtual void	OnIntervalTimeChange(const CGmtTime& TimeBegin, const CGmtTime& TimeEnd) {}
	//广告对话框
public:
	void			GetCharacternNums(const CString& StrIn, int32& iEngNums, int32& iChnNums);
	void			FillCStringWithSpace(int32 iSpaceNum, CString& StrValue );
	CString			MakeMenuString(const CString& StrName, const CString& StrExplain, bool32 bAppendSpace=true);

	void            OnAdvertiseRedarw();
	//
	void            SetAdvOwner(CWnd *pWnd);
	void			HideAdvDlg(); 
	void			RedrawAdvertise();
	void            SetHideFlag();
	void            SetAdvFont(LOGFONT *pLogFont);
// 顶部工具栏
public:
	bool32			BeShowTopToolBar() {return m_bShowTopToolBar;}
	bool            BeInSpecialMerchArray(CMerch *pMerch);
	bool            GetShowExpBtn() { return m_bShowExpBtn;}
	CString			GetCodeByMerchName(OUT CString& StrName);	// 根据配置的指数商品名称查找code

	CMerch*			ToLimitMerch(bool bUp);	//	获限定商品
	bool			HaveLimitMerch();		//	是否有限定商品
	bool			IsLimitMerch(const CMerch* pMerch);			//	是否是限定商品
private:
	void			InitialTopBtns();
	void			AddTopButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	void			DrawTopButton();
	int				TTopButtonHitTest(CPoint point);

	void			InitialTopCenterBtns();
	void			InitialKTopRightBtns();
	void			InitialTrendTopRightBtns();
	void			InitialTopExpBtns();

	int32			DrawKTopRightBtn(Graphics *pGraphics);
	int32			DrawTrendTopRightBtn(Graphics *pGraphics);
	void			DrawTopCenterBtn(Graphics *pGraphics);	
	void			DrawTopExpBtn(Graphics *pGraphics);
	
	void			AddKTopRightBtn(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	void			AddTrendTopRightBtn(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	void			AddTopCenterBtn(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	void			AddTopExpBtn(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	
	int				TKTopRightBtnHitTest(CPoint point);
	int				TTrendTopRightBtnHitTest(CPoint point);
	int				TTopCenterBtnHitTest(CPoint point);
	int				TTopExpBtnHitTest(CPoint point);
	
	void			RecalcKTopBtnRect();	// 计算按钮的区域
	void			RecalcTrendTopBtnRect();
	void			RecalcTopCenterBtnRect();
	void			RecalcTopExpBtnRect();

	// 底部指标工具栏
	void			InitialBottomIndexBtns();
	void			RecalcBottomIndexBtnRect();	// 计算按钮的区域
	void			AddBottomIndexButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL, LPCTSTR lpszDefCaption = NULL);
	void			DrawBottomIndexButton();
	int				TBottomIndexButtonHitTest(CPoint point);

	void			OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	void			UpdateIndexsBtn();
	
	bool32			NeedAddScrollButton(const CRect& rcClient);	// 当K线的底部按钮不能全部显示时，在两边添加滚动按钮
	void			OnLeft();
	void			OnRight();
	void			AddIndexToBottomBtn(CString strIndexName);		

	//	初始化周期菜单显示参数
	void            InitialCycleMenuParam();
	
	// 显示周期菜单
	void			ShowCycleMenu();

	// 切换K线周期
	BOOL            ChangeKLineCycle(UINT uId);
	
protected:
	void			SetTopCenterTitle(CString& StrTitle);
	void			FlashOrder();
	bool32			IsShowFlashOrder();
public:
	virtual CFormularContent* GetClrFomular() { return NULL; }		// 获取五彩K线指标公式
public:
	CStringArray	m_aAllFormulaNames;					// 所有指标: 每个ID 对应数组中一个名字
	std::vector<T_SpecialBtn>    m_aExpBtns;			// 增加的市场按钮
	std::vector<T_LimitMerch>	 m_vtLimitMerch;		//	限制商品的切换范围

	// 
	CDlgPullDown   *m_pDlgPullDown;

	buttonContainerType m_mapPullDown;					//	下拉菜单的按钮集

	vector<T_CycleMenuParam> m_aCycleMenuParam;			// 周期参数

public:
		int32 m_iTopMerchHeight;		// 
		int32 m_iTopButtonHeight;

		bool32	m_bShowAvgPrice;	// 是否显示均价和当前价, 设置准备通过一个IOVIEW_SETTING_CHANGE消息传达变更，然后在更新
		bool32	m_bShowRiseFallMax;	// 是否显示涨跌停价格, 设置准备通过一个IOVIEW_SETTING_CHANGE消息传达变更，然后在更新
		bool32	m_bRiseFallAxis;	// 涨跌停坐标


		// 特殊指标属性保存
private:
		map<int32, T_SpecicalIndexAttributes> m_mapSpecailIndexAttri;

protected:
	friend class CIoViewDuoGuTongLie;
	friend class CIoViewMultiCycleKline;
	//新增
	CGridCtrlSys	m_GridCtrlFloat;
	CGridCellSys*	m_pCell1;
	CGridCellSys*   m_pCell2;
	protected:
	CDelayKeyDown	m_Delay;

	int32	  m_iNumber;	// 从第几个按钮开始显示
	CPolygonButton m_leftScroll;	// 滚动显示。左右指标的按钮
	CPolygonButton m_rightScroll;

	Image*			   m_pImgZixuan;		// 已经是自选的图片
	Image*			   m_pImgAddZixuan;		// 添加自选按钮图片

	//底部中间Scroll
	Image*             m_pImgPrevScroll;
	Image*             m_pImgNextScroll;

	//全屏/恢复按钮
	Image*             m_pImgFull;
	Image*             m_pImgRestore;

	// 周期菜单项
	Image*             m_pImgCycleMenuItem;

	// 分线K时多周期按钮
	Image*             m_pImgCycleBtnItem;

	E_SCROLL_DIRECT	   m_eScorllDirect;

	bool32			   m_bShowWeight;	// 是否显示除权按钮

	CString			   m_strSelectIndexGroupName;
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewChart)
//	BOOL OnCommand(WPARAM wParam,LPARAM lParam ); 
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewChart)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLayOutAdjust();
	afx_msg void OnMenu2 ( UINT nID );
	afx_msg	LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSelfDrawClrChange(WPARAM wParam, LPARAM lParam);
	afx_msg	void OnEsc( UINT nID);
	afx_msg void OnShowWindow( BOOL bShow, UINT nStatus );
	afx_msg	LRESULT	OnIdleUpdateCmdUI(WPARAM w, LPARAM l);
	afx_msg LRESULT	OnISKINDOFCIoViewChart(WPARAM w, LPARAM l);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_CHART_H_