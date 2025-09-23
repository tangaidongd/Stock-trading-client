#ifndef _CHART_CURVE_H_
#define _CHART_CURVE_H_
#include <float.h>
#include "typedef.h"
#include "ShareFun.h"
#include "TrendIndex.h"
#include "DrawingCurve.h"
#include "AxisYObject.h"
#include "TrendIndex.h"
#include "chartexport.h"
#include "ChartCurve.h"
class CChartRegion;
class CChartCurve;

//线type
typedef enum E_LineType
{
	ELTNormal = 0,
	ELTDrawLine,
	ELTDrawPolyLine,
	ELTDrawStickLine,
	ELTDrawIcon,
	ELTStringData,
	ELTLineRt
} E_LineType;

//坐标轴值类型
typedef enum E_AxisType
{
	EATUnknown = 0,		//未知
	EATDate,			//时间(日期)
	EATDateEx,			//分笔成交的X轴(时间轴)
	EATJe,				//金额
	EATBfb,				//百分比
	EATDs,				//对数
} E_AxisType;
/////////////////////////////////////////////////////////////////////////////////////////////////////
const uint32 KDrawXAxisLeft		= 0x01;
const uint32 KDrawXAxisRight	= 0x02;
const uint32 KDrawXAxisCenter	= 0x04;
const uint32 KDrawXAxisVTop		= 0x08;



///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//要绘制数据的顺序序列
//(1)是在DrawingCurve中绘制的数据的来源.
//(2)这里对顺序进行了约束,是否便于以后扩展?
//(3)允许设定用户自定义数据(比如上下文指针)
class CHART_EXPORT CNodeSequence
{
public:
	CNodeSequence();
	~CNodeSequence();
	
public:
	bool32			Add(const CNodeData& NodeData);
	bool32			Set(int32 iPos, IN CNodeData& NodeData);
	int32			GetSize()								{ return m_aNodes.GetSize(); }
	bool32			GetAt(int32 iPos, CNodeData& NodeData);
	void			Copy(CNodeSequence* pSrc);
	bool32			Lookup(int32 iID, int32& iPos);
	bool32			LookupEx(int32 iID, int32& iPos);
	void			RemoveAt(int32 iPos);
	void			RemoveAll()								{ m_aNodes.SetSize(0); }
	void			SetUserData(void* pUserData)			{ m_pUserData = pUserData; }
	void*			GetUserData()							{ return m_pUserData; }
	void			SetName(const LPCTSTR StrName)			{ m_StrName = StrName; }
	void			SetWeightName(const LPCTSTR StrName)	{ m_StrWeightName = StrName; }
	CString			GetName()								{ return m_StrName;}
	CString			GetWeightName()							{ return m_StrWeightName; }
	void			SetNamePrefix(const LPCTSTR StrName)	{ m_StrNamePrefix = StrName; }
	CString			GetNamePrefix()							{ return m_StrNamePrefix; }
	void			SetNamePostfix(const LPCTSTR StrName)	{ m_StrNamePostfix = StrName; }	
	CString			GetNamePostfix()						{ return m_StrNamePostfix; }

	int32			AddRef(CDrawingCurve* pCurve);
	int32			Release(CDrawingCurve* pCurve);
	void			DumpAttatchedCurves(CArray<CDrawingCurve*, CDrawingCurve*>& Curves);

	// 
	CArray<CNodeData, CNodeData&>& GetNodes()					{ return m_aNodes; }	
	CArray<CDrawingCurve*, CDrawingCurve*>&	GetAttachedCurves() { return m_aAttatchedCurves; }	

private:
	int32			HalfSearch(int32 id);
	int32			HalfSearchEx(int32 id);
	bool32			HalfSearchEqu(int32 id,int32 t,int32 i, int32& iPos,int32& iSub);

public:
	CString			m_StrName;
	CString         m_StrWeightName;
	CString			m_StrNamePrefix;
	CString			m_StrNamePostfix;

	CArray<CNodeData, CNodeData&>	m_aNodes;			// 具体数据列表

private:
	void*			m_pUserData;
	CArray<CDrawingCurve*, CDrawingCurve*>	m_aAttatchedCurves;	

	//
friend class CRegionViewParam;
friend class CDrawingCurve;
friend class CIoViewKLine;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//Chart线
/*
(一)简要说明
完成很独立的功能:引用外部的NodeData,进行绘制.对外提供拾取/坐标转换等辅助功能.
(二)调用说明(次序)
(1)创建对象
(2)引用NodeData
(3)CalcX/CalcY/CalcTransform
(4)Draw
此外,还可以设定组标志和组名

*/
class CHART_EXPORT CNodeDrawerData : public CNodeData
{
public:
	CNodeDrawerData();
	CNodeDrawerData(IN const CNodeData& NodeData);
	~CNodeDrawerData();

public:
	void			CalcXPos(int32 &x1, int32 &x2, int32 &x3);

public:
	int32			m_iCurveLoop;
	int32			m_iValidNum;		// Nodes可用总数

	int32			m_iPos;				// 所在Nodes当前位置.
	int32			m_iX;				// 当前Node在屏幕的x坐标
	int32			m_iXLeft;
	int32			m_iXRight;

	// 
	int32			m_iOpen;
	int32			m_iClose;
	int32			m_iHigh;
	int32			m_iLow;
	int32			m_iAvg;

	int32			m_iAmount;
	int32			m_iVolumn;
	int32			m_iHold;
	
	bool32			m_bRise;			// 对应K 线的涨跌

};

class CHART_EXPORT CNodeDrawer
{
public:
	CNodeDrawer();
	virtual ~CNodeDrawer();

public:
	virtual void	DrawNodes(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, IN CChartCurve &ChartCurve, const CArray<CNodeDrawerData, CNodeDrawerData&> &aNodeDrawerDatas) = 0;
	
	int32			CalcNeedCalcNodesOverlapKLine(IN CNodeDrawerData *pNodeDrawerDatas, IN int32 iNodeCount, IN CChartCurve &ChartCurve, 
												  OUT CArray<CRect, CRect &>	&aNeedDrawRects, OUT CArray<CNodeDrawerData *, CNodeDrawerData *>& aNeedDrawNodeDrawerDatas);

	virtual bool32	BeDrawTitleText() { return true; }
	virtual bool32	BeNoDraw()		{ return false; }

protected:
	int32			AddRef ( );
	int32			Release ( );

protected:
	int32			m_iRef;

friend class CChartCurve;
};

class CHART_EXPORT CMinMax
{
public:
	CMinMax() 
	{
		m_bValid = false;
		m_fMin = 0.;
		m_fMax = 0.;
	}

public:
	bool32			m_bValid;
	float			m_fMin;
	float			m_fMax;
};

class CHART_EXPORT CChartCurve : public CDrawingCurve
{
public:
	enum E_ChartCurveFlag
	{
		KDependentCurve				= 0x00000001,		// 主(Dependent Curve)
		KRequestCurve				= 0x00000002,		// 请求数据的Curve,缺省为K线

		KInVisible					= 0x00000010,		// 不显示
		KYInheritTransform			= 0x00000020,		// 使用DependentCurve的Transform(translate和scale)
//		KSelect						= 0x00000040,		// 被选中(内部使用)
		KUseNodesNameAsTitle		= 0x00000080,
		
		KTypeKLine					= 0x00000100,		// 显示tooltips的标识;
		KTypeIndex					= 0x00000200,		// 只有Index类型,才能在Region间拖动.才允许删除?
		KTypeTrend					= 0x00000400,		// 显示tooltips的标识;Curve的左侧和右侧,强制对齐到RectCurve;
		KTypeTick					= 0x00000800,		// 显示tooltips的标识;

		KYTransformToAlignDependent	= 0x00001000,		// 对比(compare)商品的曲线用.
		KIndex1						= 0x00002000,		// 第一条Index线,该Curve会显示index名称,比如KDJ
		KindexCmp					= 0x00004000,		// 叠加指标的标志

		KYTransformByLowHigh		= 0x00010000,		// Y轴基于Low/High计算坐标
		KYTransformByClose			= 0x00020000,		// Y轴基于Close计算坐标
		KYTransformByAmount			= 0x00040000,		// Y轴基于Amount(额)计算坐标
		KYTransformByVolumn			= 0x00080000,		// Y轴基于Volumn(量)计算坐标
		KYTransformByAvg			= 0x00100000,		// Y轴基于Avg(均价)计算坐标		

		KDonotPick					= 0x00200000,		// 该标志的线不能被pick xl 0214
		KTimeIdAlignToXAxis			= 0x00400000,		// 时间点与x轴对齐标志，使用此标志时，将每个点的x轴坐标对应到具有相同时间点x轴
	};

public:
	CChartCurve(CChartRegion &ChartRegion, uint32 iFlag);
	~CChartCurve();

	// from CDrawingCurve
public:
	virtual void	Draw(IN CMemDCEx* pDC, IN CMemDCEx* pPickDC) { NULL; }
	virtual bool32	CalcY(OUT float &fYMin, OUT float &fYMax);					// 该值返回的是axisY的min, max
	virtual bool32	GetYMinMax(OUT float& fYMin, OUT float& fYMax);				// 同上
	virtual void	SetYMinMax(float fMin, float fMax, bool32 bValid = true);	// 同上

	virtual bool32	RegionXToCurvePos(int32 x, OUT int32& iPos);	// 此次修改仅影响Y轴，x轴不变
	virtual bool32	CurvePosToRegionX(int32 iPos, OUT int32& x);	// 根据Region中的x坐标,计算m_aNodes的第iPos个NodeData
	virtual bool32	RegionXToCurveID(int32 x, OUT int32& id);

	// 绘图相关数据
public:
	CChartRegion&	GetChartRegion()						{ return m_ChartRegion; }
	
	void			AttatchNodes(CNodeSequence* pNodes);
	void			DetatchNodes();
	CNodeSequence*	GetNodes()								{ return m_pNodes; }
	
	void			AttatchDrawer(CNodeDrawer* pDrawer);
	void			DetatchDrawer();
	CNodeDrawer*	GetDrawer()								{ return m_pDrawer; }

	void SetAxisYType(CPriceToAxisYObject::E_AxisYType eType);	// 设置坐标类型
	CPriceToAxisYObject::E_AxisYType GetAxisYType() const { return m_eAxisYType; }

	void			SetUserPriceMinMaxBaseValue(float fMin=FLT_MAX, float fMax=-FLT_MAX){ m_fUserMin=fMin; m_fUserMax=fMax; };	// 设置用户设定的最大最小值

	// 标题相关
public:
	void			SetGroupTitle(const LPCTSTR StrTitle)	{ m_StrGroupTitle = StrTitle; }
	CString			GetGroupTitle()							{ return m_StrGroupTitle; }
	void			SetTitle(const LPCTSTR StrTitle)		{ m_StrTitle = StrTitle; } 
	CString			GetTitle()								{ return m_StrTitle; }

	// 
public:
	void			SetDependent();
	
	bool32			IsSelect()								{ return CheckFlag(m_iFlag,CDrawingCurve::KSelect); }
	bool32			IsDependent()							{ return CheckFlag(m_iFlag,CChartCurve::KDependentCurve); }
	
	void			SetSelectNodeID(int32 iID)				{ m_iSelectNodeID = iID; }
	int32			GetSelectNodeID()						{ return m_iSelectNodeID; }

	void			SetOccupyIndexLineClr(bool32 bOccupy) { m_bOccupyIndexLineClr = bOccupy; }
	bool32			GetOccupyIndexLineClr() const { return m_bOccupyIndexLineClr; }

	// 绘图相关
public:
	void			Draw(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC,int32 iLoop);
	
	bool32			NodePosValue(int32 iPos, OUT float& fValue);		// 价格

	bool32			RegionYValue(int32 y, float& fValue);				// regiony与价格
	bool32			ValueRegionY(float fy, int32& iValue);				// regiony与价格

	// Y值类型分为价格实际类型， 坐标类型， RegionY显示类型
	// 所有价格转换为RegionY显示类型，必须经过当前的坐标类型来实现转换
	// 坐标转换函数，不推荐任何外部猜测的转换
	// 根据AxisY值决定坐标轴的最大最小值
	bool32			PriceYToRegionY(float fPriceY, OUT int32 &iRegionY);
	bool32			RegionYToPriceY(int32 iRegionY, OUT float &fPriceY);
	bool32			PriceYToAxisY(float fPriceY, OUT float &fAxisY);
	bool32			AxisYToPriceY(float fAxisY, OUT float &fPriceY);
	bool32			AxisYToRegionY(float fAxisY, OUT int32 &iRegionY);
	bool32			RegionYToAxisY(int32 iRegionY, OUT float &fAxisY);

	// 叠加指标的坐标计算
	bool32			PriceYToCmpClientY(float fPriceY, OUT int32& iCmpY);

	// 扩展坐标系计算
	bool32			ExtendPriceYToRegionY(float fPriceY, OUT int32 &iRegionY);
	bool32			ExtendRegionYToPriceY(int32 iRegionY, OUT float &fPriceY);

	bool32			GetAxisBaseY(OUT float &fBaseAxisY);	// 获取坐标轴的基础值
	void			SetPriceBaseY(float fPriceBaseY, bool32 bUpdateCalcObj = true);		// 只要设定了非普通坐标，则必需设置价格基础值, 会重新设置计算坐标参数
	static    float GetInvalidPriceBaseYValue() { return CPriceToAxisYObject::GetInvalidPriceBaseYValue(); };	// 基础的无效值
	// 帮助设定基础值
	bool32			GetValidFrontNode(int32 iFrontIndex, OUT CNodeData &node);	// 获取第n的有效node，0为第一个
	bool32			GetValidBackNode(int32 iBackIndex, OUT CNodeData &node);		// 获取倒数第n的有效node, 0为倒数第一个
	bool32			ConvertNodeDataToPrice(const CNodeData &node, OUT float &fPrice);		// 根据曲线类型，填充node的值

	bool32			GetAxisYCalcObject(OUT CPriceToAxisYObject &obj) const { obj = m_AxisYCalcObject; return m_MinMaxY.m_bValid; };	// 获取内部的计算对象, 内部计算对象可能未附初值

	//
	bool32			BeShowTitleText();
	bool32			BeNoDraw();

/////////////////////////////////////////
public:
	int32			m_clrTitleText;				// 外面设定
	int32			m_iLineWidth;				// 外面设定

protected:
	int32			m_iValidNodeNum;
	int32			m_iLevel;					// 绘制的先后次序,绘制Title向左对齐的次序.
	CString			m_StrTitle;					// 当前Chart线的标题,比如日线,MA~6,...
	CString			m_StrGroupTitle;
	int32			m_iSelectNodeID;

	bool32			m_bOccupyIndexLineClr;		// 是否占用了indexline的颜色，并不是每根线都需要此颜色，需要则设置此值，否则非

	CPriceToAxisYObject::E_AxisYType		m_eAxisYType;				// 坐标轴类型

	// Y值类型分为价格实际类型， 坐标类型， RegionY显示类型
	// 所有价格转换为Rect显示类型，必须经过当前的坐标类型来实现转换
	float			m_fPriceBase;				// 计算axisy有用的基础值
	CPriceToAxisYObject	m_AxisYCalcObject;		// 计算
	
	// 绘图属性
public:	
	

private:
	CChartRegion	&m_ChartRegion;				// 用于获取m_RectView,对y方向进行缩放.
	CNodeDrawer*	m_pDrawer;
	CNodeSequence*	m_pNodes;					// 线
	CMinMax			m_MinMaxY;					// 纵向上下边界

	float			m_fUserMin;					// 用户最大最小值
	float			m_fUserMax;					// 

friend class CChartRegion;
};


#endif


