// ChartRegion.h: 
//
//////////////////////////////////////////////////////////////////////

#ifndef _CHARRT_REGION_H_
#define _CHARRT_REGION_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SelfDraw.h"
#include "ChartCurve.h"
#include "GmtTime.h"
#include "XTipWnd.h"
#include "Region.h"
#include "chartstruct.h"
#include "IRegionData.h"
#include "chartexport.h"
#include "NCButton.h"
/*
ChartRegion是空的,设定Curve才执行画.
一个ChartRegion可以包含一个或多个Curve
而一个Curve仅属于一个ChartRegion,Curve创建时指定ChartRegion,运行期间不允许改变.

Curve是空的,设定NodeSequence才画,仅允许设定一个NodeSequence
一个NodeSequence可以属于多个Curve,运行期间Curve的当前NodeSequence可以改变.
*/

class CChartCurve;
class CChartRegionViewParam;
class CChartRegion;
class CIoViewChart;

const float KHZoomFactor		= 0.75f;	// 每次缩放的比例
const int32 KNodeNumberMin		= 5;		// 显示NodeData的最小数目
const int32 KNodeNumberMax		= 10000;	// 显示NodeData的最大数目

const int32 KSensitiveWidth		= 3;		// 拖动时触碰的边缘的敏感程度

#define REGIONTITLEADDBUTTONID		-5		// 增加副图
#define REGIONTITLEDELBUTTONID		-6		// 删除副图

#define REGIONTITLELEFTBUTTONID		-7		// 前一个指标	
#define REGIONTITLERIGHTBUTTONID	-8		// 后一个指标

#define REGIONTITLEFOMULARBUTTONID	-9		// 选择指标

extern CHART_EXPORT const char* KIoViewChartSelfDrawRgn;
extern CHART_EXPORT const char* KIoViewChartSelfDrawRgnID;
extern CHART_EXPORT const char* KIoViewChartSelfDrawNum;

class CHART_EXPORT CChartRegionViewParam : public CRegionViewParam
{
public:
	CChartRegionViewParam ( IN CWnd* pWnd);
	virtual ~CChartRegionViewParam();

public:
	//Curve被删除,导致所绘制的Node被删除,应用层有必要清除Node的源数据.
	virtual void OnNodesRelease ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes ) = 0;
	// 选取了chart
	virtual void OnPickChart(CChartRegion *pRegion, int32 x, int32 y, int32 iFlag){};
	//拾取到了NodeData
	virtual void OnPickNode( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes, CNodeData* pNodeData,int32 x,int32 y,int32 iFlag )= 0;
	//右键点击了Region(如果pCurve不为空,则标明右键点击了pCurve/pNodes/pNodeData)
	virtual void OnRegionMenu ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes,CNodeData* pNodeData, int32 x, int32 y )	= 0;
	virtual void OnRegionMenu2 ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y )	= 0;
	virtual void OnRegionIndexMenu ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y )	= 0;
	//删除Curve
	virtual void OnDropCurve ( CChartRegion* pSrcRegion,CChartCurve* pSrcCurve,CChartRegion* pDestChart) = 0;
	virtual void OnRegionCurvesNumChanged ( CChartRegion* pRegion, int32 iNum ) = 0;
	//十字光标的数据
	virtual void OnCrossData ( CChartRegion* pRegion,int32 iPos,CNodeData& NodeData, float fPricePrevClose, float fYValue, bool32 bShow) = 0;
	// 没有数据的时候只显示一个时间
	virtual void OnCrossNoData (CString StrTime,bool32 bShow = true) = 0;	
	// 双击的通知响应
	virtual void OnChartDBClick(CPoint ptClick, int32 iNodePos) {}
	// 点击了指标相关的按钮（帮助，修改参数，删除）
	virtual void OnRegionIndexBtn (CChartRegion* pRegion, CDrawingCurve* pCurve, int32 iID )	= 0;
	
	virtual IChartBrige  GetRegionParentIoView() = 0;
	virtual bool32 OnCanPanLeft ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum ){return true;}
	virtual bool32 OnCanPanRight ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum ){return true;}
	
	virtual CString OnTime2String ( CGmtTime& Time ) = 0;
	virtual CString OnFloat2String ( float fValue, bool32 bZeroAsHLine = true, bool32 bNeedTerminate = false ) = 0;

	virtual void	OnCalcXAxis ( CChartRegion* pRegion, CDC* pDC, CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide) = 0;
	virtual void	OnCalcYAxis ( CChartRegion* pRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide) = 0;

	virtual CString OnGetChartGuid ( CChartRegion* pRegion ) = 0;
	virtual void	OnSliderId ( int32& id, int32 iJump ) {}

	// 获取线某坐标点的值,充当线的后缀部分
	virtual bool32  OnGetCurveTitlePostfixString(CChartCurve *pCurve, CPoint ptClient, bool32 bHideCross, OUT CString &StrPostfix){ return false; }
	
	virtual bool32 GetTips(IN CPoint pt, OUT CString& StrTips, OUT CString &StrTitle) { return false; }
public:
	CNodeSequence*	CreateNodes ( );
	void			RemoveNodes ( CNodeSequence* pNodes );
	int32			GetNodesNum();
	CNodeSequence*	GetNodes(int32 iPos);

public:
	CChartCurve*	m_pDragCurve;
	CSelfDrawCurve*	m_pDragCurveSD;
	CChartRegion*	m_pDragRegion;
	CPoint			m_PointPress;
	CPoint			m_PointRectZoomOutEnd;
	bool32			m_bPressXAxis;
	bool32			m_bRefreshLast;//最右侧实时更新.

public:
	CArray<CNodeSequence*,CNodeSequence*>	m_Nodes;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum E_NodeReqType
{
	ENRTInit,
	ENRTFrontCount,// 倒数个数, 往时间更早的方向
	ENRTCount// 顺数个数, 往时间更新的方向
} E_NodeReqType;
//ChartRegion
/*
约束说明:
(1)一个CChartRegion(KChart)的多个ChartCurve只能共用一个x轴
(2)一个CChartRegion(KChart)处理自己的OnKeyDown,OnMouseMove,和其它CChartRegion无关.
(3)一个CChartRegion必须依赖一个ChartCurve,才能处理十字光标/漫游/缩放/坐标计算等.
(三)调用说明:
(1)
(2)
(3)
*/

const int32 KMsgInactiveCross = 5;
class CHART_EXPORT CChartRegion  : public CRegion
{
public: 
	enum E_DrawCross 
	{
		EDCMouseMove = 0,
		EDCUpDownKey,
		EDCLeftRightKey,
	};

	enum E_ChartRegionFlag
	{
		KChart					= 0x00000001,			// Chart标志
		KXAxis					= 0x00000002,			// x轴标志
		KYAxis					= 0x00000004,			// y轴标志
		KUserChart				= 0x00000008,			// UserChart标志
		
		KPanAble				= 0x00000010,			// 允许平移(K线可以平移,分时不能平移)
		KHScaleAble				= 0x00000020,			// 允许水平缩放(以最右侧时间为固定点)
		KCrossAble				= 0x00000040,			// 允许显示十字光标
		KDragXAxisPanAble		= 0x00000080,
		
		KYDependent				= 0x00000100,			// 被Y值依赖的Region( 一般是 YRegion)
		KYAxisSpecifyTextColor	= 0x00000200,
		KLockMouseMove			= 0x00000400,			// 内部标志,临时禁止OnMouseMove
		KFixShowYValue			= 0x00000800,			// 子图Y坐标轴固定显示某个值(类似MACD 的0 这个轴线)

		KActiveFlag				= 0x00001000,			// 父窗口激活的时候, 这个region 是否画激活标志
		KDonotShowCurveDataText = 0x00002000,			// 不要显示该region下面线的数据即VOLFS L0:123 L0:123部分不显示
	};

public:
	CChartRegion( IN CRegionViewParam* pViewParam, CChartRegion*& pRegion,CString StrName, uint32 iFlag, CNodeSequence* pDefaultNodes = NULL );
	void    Destruct();
	virtual ~CChartRegion();

public:
	static const T_FuctionButton*	GetRegionButton(int32 iIndex);		// 得到Region按钮
	static const int32	GetRegionButtonNums();							// 得到Region按钮个数

	// 虚接口
public:
	//////////////////////////////////////////////////////////////////////////////
	virtual bool32	OnKeyDown(int32 nChar, int32 nRepCnt, int32 nFlags);//???
	virtual	void	OnMouseMove(int32 x, int32 y); //鼠标移动中绘制十字光标
	virtual void	OnMousePress(int32 x, int32 y, int32 iPass, bool bRightButton = false);
	virtual void	OnMousePressUp(int32 x, int32 y);
	virtual void	OnMouseDBClick(int32 x, int32 y);//鼠标左键双击
	virtual void	OnMouseRClick (int32 x, int32 y);
	virtual void	OnMouseDragMove(int32 x, int32 y, bool bRightButton = false);//拖动
	virtual void	OnMouseDragDrop(int32 x, int32 y, bool bRightButton = false);//左键松开
	
	virtual void	OnSelfDrawPress(int32 x,int32 y);
	virtual void	OnSelfDrawDrag(int32 x,int32 y);
	virtual void	OnSelfDrawRelease(int32 x,int32 y);
	
	virtual void	OnSelfDrawMove(int32 x,int32 y);
	virtual void	OnSelfDrawEnd(int32 x,int32 y);
	
	virtual void	OnPreDraw(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC);
	virtual void	OnDraw(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC);
	virtual void	OnAfterDraw(IN CMemDCEx* pOverlayDC);
	virtual void	OnActive(bool32 bActive);
	virtual void	OnMessage(int32 mid, void* pData);
	virtual void	OnMouseOverAWhile(int32 x, int32 y);//在x,y上停留了一段时间,用于比如:拾取并显示tooltips.应用层实现
	virtual void	OnMouseOverLeave(int32 x, int32 y);//应用层实现
	virtual void	OnKeyDel();//应用层实现,做什么?
	virtual void	OnDrawAxisSliderText(IN CMemDCEx* pDC,CRect Rect,const CPoint pt);//应用层实现
	virtual CChartCurve* CurveNew(int32 iFlag);

public:
	CRegionViewParam*	GetViewParam()								{ return m_pViewParam; }
	CChartRegionViewParam*  GetViewData()							{ return (CChartRegionViewParam*)m_pViewParam; }
	uint32			GetFlag()										{ return m_iFlag; }

	void			SetTitle(const LPCTSTR StrTitle)				{ m_StrTitle = StrTitle; }			// 设置当前ChartRegion的标题,比如:浦发银行,对KChart有效
	CString			GetTitle()										{ return m_StrTitle; }
	void			ActiveCross(bool32 bActive)						{ m_bActiveCross = bActive; }		// 激活显示(或隐藏)十字光标,对KChart有效
	bool32			IsActiveCross()									{ return m_bActiveCross; }

	bool32			AddYDependentRegion(CChartRegion* pRegion);
	bool32			RemoveYDependentRegion(CChartRegion* pRegion);

	void			SetDataSourceRegion(CChartRegion* pRegion)		{ m_pDataSourceRegion = pRegion; }
	CChartRegion*	GetDataSourceRegion ( )							{ return m_pDataSourceRegion; }

	CChartCurve*	GetDependentCurve();							// 对于KChart,用于漫游请求数据,对于Axis,用于计算刻度等.(实际使用Curve内部的Nodes),
	CChartCurve*	CreateCurve(uint32 iFlag);
	bool32			RemoveCurve(CChartCurve* pCurve);
	int32			GetCurveNum()									{ return m_Curves.GetSize(); }
	CChartCurve*	GetCurve(int32 iPos);

	CPoint			GetPtCross() const								{ return m_ptCross; }

public:
	void			SetLeftSkip(int32 iSkip)						{ m_iLeftSkip = iSkip;}
	void			SetRightSkip(int32 iSkip)						{ m_iRightSkip = iSkip;}
	void			SetTopSkip(int32 iSkip)							{ m_iTopSkip = iSkip; }
	void			SetBottomSkip(int32 iSkip)						{ m_iBottomSkip = iSkip; }
	
	int32			GetLeftSkip()									{ return m_iLeftSkip; }
	int32			GetRightSkip()									{ return m_iRightSkip; }
	int32			GetTopSkip()									{ return m_iTopSkip; }
	int32			GetBottomSkip()									{ return m_iBottomSkip; }

	CRect			GetRectCurves();
	void			ClipY(int32& y);
public:
	void			SetTitleTextColor(COLORREF clr)					{ m_clrTitle = clr; }
	void			SetTitleTextFont(CFont* pFont)					{ m_FontTitle = pFont; }
	void			SetAxisTextColor(COLORREF clr)					{ m_clrAxisText = clr; }
	void			SetAxisTextFont(CFont* pFont)					{ m_FontAxisText = pFont; }
	void			SetAxisRuleColor(COLORREF clr)					{ m_clrAxisRule = clr; }
	void			SetAxisSliderBorderColor(COLORREF clr)			{ m_clrSliderBorder = clr; }
	void			SetAxisSliderFillColor(COLORREF clr)			{ m_clrSliderFill = clr; }
	void			SetAxisSliderTextColor(COLORREF clr)			{ m_clrSliderText = clr; }
	void			SetAxisSliderTextFont(CFont* pFont)				{ m_FontSliderText = pFont; }
	void			SetCrossColor(COLORREF clr)						{ m_clrCross = clr; }

	COLORREF		GetTitleTextColor()								{ return m_clrTitle; }
	CFont*			GetTitleTextFont()								{ return m_FontTitle; }
	COLORREF		GetAxisTextColor()								{ return m_clrAxisText; }	
	CFont*			GetAxisTextFont()								{ return m_FontAxisText; }
	COLORREF		GetAxisRuleColor()								{ return m_clrAxisRule; }
	COLORREF		GetAxisSliderBorderColor()						{ return m_clrSliderBorder; }
	COLORREF		GetAxisSliderFillColor()						{ return m_clrSliderFill; }
	COLORREF		GetAxisSliderTextColor()						{ return m_clrSliderText; }
	CFont*			GetAxisSliderTextFont()							{ return m_FontSliderText; }
	COLORREF		GetCrossColor()									{ return m_clrCross; }

public:
	bool32			RegionToSD(int32 x1,int32 y1, int32& x2, int32& y2);
	bool32			RegionToSD(int32 x,int32 y, int32& id,float& fVal);
	bool32			SDToRegion(int32 id, float fVal, int32& x,int32& y);
	bool32			IsSDInRegion(CSelfDrawNode* pSelfDrawNode);
	bool32			MoveSDId(int32 x1,int32 y1,int32 x2,int32 y2,int32& id1,int32& id2);
	bool32			MoveSDY(int32 x1,int32 y1,int32 x2,int32 y2,float& fVal1,float& fVal2);

	bool32			ExtendSDIdToRegionX(int32 id, int32 &x);
	bool32			ExtendRegionXToSDId(int32 x, int32 id);

public:
	static bool32	SetSelfDrawType(E_SelfDrawType eDrawType, IChartRegionData* pChart);

	//////////////////////////////////////////////////////////////////////////
	bool32			bNoData();
	void			DrawIndexValueByMouseMove(int32 x,int32 y,bool32 bHideCross = false);

private:
	void			OnDependentChanged(CChartCurve* pCurve);

	

// 	bool32			HPanLeft();
// 	bool32			HPanRight();
// 
// 	int32			HPanLeft(int32 iDragNum);
// 	int32			HPanRight(int32 iDragNum);

	bool32			FlyCross(int32 iCross, bool32 bLeft);
//	void			DrawTitle(CMemDCEx* pDC,CMemDCEx* pPickDC);
	void			DrawTitle ( CMemDCEx* pDC,CMemDCEx* pPickDC,bool32 bMouseMove = false);
	void			DrawRound(CMemDCEx* pDC);
	void			DrawSaneIndexTitle(CMemDCEx* pDC,int32 iPos,bool32 bMouseMove = false);
	bool32			DrawTitleText(CMemDCEx* pDC,CMemDCEx* pPickDC, CChartCurve* pCurve,CString StrTitle, CRect& Rect, bool32 bAppendSpaceSep=true);
	void			DrawAxisSlider(CMemDCEx* pDC,const CPoint pt);
	void			DrawCross (CMemDCEx* pDC,int32 x, int32 y,E_DrawCross eDrawCross);
	void			DrawCross (CMemDCEx* pDC,int32 iXNodePos, E_DrawCross eDrawCross);
	void			DrawRectZoomOut(CMemDCEx* pDC);
	void            DrawRightDragRect(CMemDCEx* pDC);
	void			DrawVLine (CMemDCEx* pDC, int32 x);

public:
	void			DrawHVLine(CMemDCEx* pDC,int32 iVal, bool32 bHorizontal = true);
	void			DrawAxisSliderRect(CMemDCEx* pDC,CRect Rect);
	void			DrawSelfDrawing(CMemDCEx* pDC,CMemDCEx* pPickDC);
	bool32			PickNode(int32 x,int32 y, OUT CDrawingCurve*& pCurve, OUT CNodeSequence*& pNodes, OUT CNodeData& NodeData,bool32 bClearLast = true);
	void			SetPickFlag(CChartCurve* pCurve, CNodeData& NodeData);
	void			SetPickFlag(CSelfDrawCurve* pCurve);
	void			ClipSelfDrawCursor();

	//
	static E_SelfDrawType GetSelfDrawValue()	{ return m_eSelfDrawType; }
	void			ClearSelfDrawValue();

	CString			GetSelfDrawXmlString(int32 iIndex);

	IChartBrige	GetParentIoViewData();	
	void			SetGGTongESCFlag();				// 告诉GGTongView 当前是否有十字光标
	
public:
	CArray<CSelfDrawCurve*, CSelfDrawCurve*> m_SelfDrawingCurves;

public:
	CArray<CChartCurve*,CChartCurve*>	m_Curves;
	CArray<CChartRegion*,CChartRegion*>	m_YDependentRegions;
	CChartRegion*	m_pDataSourceRegion;

	CString			m_StrTitle;//当前ChartRegion的标题,比如:MerchName
	bool32			m_bActiveCross;//是否激活十字光标,KCrossAble标志存在时有意义.

public:
	int32			m_iNodeCross;			//当前十字光标所在的位置
	bool32			m_bSplitWndDrag;		//分隔条是否在拖动(是的话,在重画的时候,不要设置鼠标位置)
	int32			m_iNodeVPos;			//当前垂直线所在的位置

protected:
	CRect			m_RectCurves;
	int32			m_iLeftSkip;
	int32			m_iRightSkip;
	int32			m_iTopSkip;			// 外面设定
	int32			m_iBottomSkip;		// 外面设定
	CPoint			m_ptCross;
	
public:
	COLORREF		m_clrSliderText;//时间或金额(或值)滑动条的文本色
	CFont*			m_FontSliderText;
	COLORREF		m_clrSliderBorder;//时间或金额(或值)滑动条的边框色
	COLORREF		m_clrSliderFill;////时间或金额(或值)滑动条的填充色
	COLORREF		m_clrTitle;//Region标题的颜色
	CFont*			m_FontTitle;
	COLORREF		m_clrAxisText;//标识时间或金额(或值)的文本色
	CFont*			m_FontAxisText;
	COLORREF		m_clrAxisRule;//标识时间或金额(或值)的刻度的颜色
	COLORREF		m_clrCross;

protected:
	//自画线的变量
	CPoint			m_PtPress;
	CPoint			m_PtRelease;
	CPoint			m_PtEnd;
	int32			m_iIdPress;
	int32			m_iIdRelease;
	int32			m_iIdEnd;
	float			m_fValPress;
	float			m_fValRelease;
	float			m_fValEnd;
	
	static E_SelfDrawType m_eSelfDrawType;
	static int32	m_iSelfDrawAppendOp;

public:
	// x轴y轴相关
	CArray<CAxisNode, CAxisNode&>		m_aXAxisNodes;	// x轴每个点分布的位置
	CArray<CAxisDivide, CAxisDivide&>	m_aXAxisDivide;	// x轴分割线
	
	CArray<CAxisDivide, CAxisDivide&>	m_aYAxisDivide;	// y轴分割线
	
	// 拖动X 轴相关
	int32			m_iJumpNodeID;	// 最开始拖动时定位的那个NodeID
	int32			m_iJumpNodePos; // 最开始拖动时定位的那个NodePos	
	int32			m_iPreNodeID;	// 上一个NodeID,判断是否重复.
	
	Image          *m_pImgSet;

	// 
	int32			m_iAxisTextMaxWidth;
	int32			m_iAxisTextMaxHeight;

	CString			m_StrAxisTextsOfNoData;     // 无数据时,X轴和详情窗口显示的时间值

private:	
	int32			m_iZoomOutWishNums;		 
	int32			m_iMouseMoveTrendIndexPos;
	bool32			m_bAbleToRectZoomOut;
    DWORD           m_dwTickCnt;

private:
	CXTipWnd		m_TipWnd;
	static	int32	m_iPreNodePos; 

	void InitIndexBtn();
	void AddIndexBtn(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption);
	void DrawIndexBtn( CMemDCEx* pDC, int32 ID, CRect &rect);
	int32 TIndexButtonHitTest(CPoint point);
	std::map<int, CNCButton> m_mapIndexBtn;			// 指标区域上添加的按钮
	int32 m_iIndexBtnHovering;

friend class CChartCurve;
friend class CIoViewChart;
friend class CIoViewKLine;
friend class CIoViewTrend;
friend class CIoViewTick;
friend class CIoViewExpTrend;
friend class CIoViewKLineArbitrage;
friend class CIoViewTrendArbitrage;
};

#endif // _CHARRT_REGION_H_

