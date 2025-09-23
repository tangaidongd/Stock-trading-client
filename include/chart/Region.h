#ifndef _REGION_H_
#define _REGION_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <float.h>
#include "chartexport.h"
/*
问题列表:
(1)x,y轴必须是固定宽/高,不能等比缩放.------解决了
(2)拖动一个边,相关的边也需要拖动.比如上下拖动KLine的下边,那么左侧Y轴的下边也要跟着动.------解决了
(3)考虑一个Region允许最大的情况------未解决
(4)LineType/LineStyle------未解决
(5)CKLine的数据的合法性检查,比如>0,High>Low,High>Open和Close,Low<Open和Close

2008-4-19修改:允许Region的尺寸为0,允许运行期间修改BottomDragAble
*/

#include "typedef.h"
#include "MemDCEx.h"
#include "afxtempl.h"

class CRegion;
class CRegionViewParam;
class CDrawingCurve;

const int32		KRegionMouseOverAWhileTimerId	= 100001;
const uint32	KRegionMaskSegment				= 0xFFFFFFFF;
const int32		KRegionMinWH					= 0;//允许Region的尺寸为0

typedef CArray<CRegion*,CRegion*> RegionArray;

/////////////////////////////////////////////////
// 单坐标 
const float KAxisFloatRefer = 10000.;	// 坐标里面的Pos参考区间

// 分割线上的文字显示
class CHART_EXPORT CAxisDivideText
{
public:
	CAxisDivideText()
	{
		m_uiTextAlign	= DT_CENTER | DT_VCENTER;
		m_lTextColor	= 0x00000000;
	}

public:
	CString			m_StrText;		// 需要显示的文字（分上下、左右）
	uint32			m_uiTextAlign;	// DT_LEFT | DT_TOP
	long			m_lTextColor;	
};


// 坐标分割
class CHART_EXPORT CAxisDivide
{
public:
	enum E_DivideLevel			// 显示优先级， 当图像放缩后， 考虑版面的整洁， 仅显示指定级别以上的分割信息
	{
		EDLLevel1	= 0x00000001,				// 优先级最高
		EDLLevel2	= 0x00000002 | EDLLevel1,
		EDLLevel3	= 0x00000004 | EDLLevel2,
		EDLLevel4	= 0x00000008 | EDLLevel3
	};

	enum E_LineStyle			// 线型
	{
		ELSNone		= 0,		// 不画
		
		ELSSolid,				// 实线
		ELSDoubleSolid,			// 双实线
		ELS3LineSolid,			// 3实线

		ELSDot,					// 虚线
		ELSDoubleDot,			// 双虚线

		ELSWave,				// 波浪线
	};

public:
	int32			m_iPosPixel;	// 显示位置， 像素坐标

	// 文字
	E_DivideLevel	m_eLevelText;	// 文字显示优先级， 先画优先级高的， 优先级I的必须画， 其它视真实画的时候遮盖情况而定
	CAxisDivideText	m_DivideText1;	//（分上下、左右）
	CAxisDivideText	m_DivideText2;
	
	// 
	E_DivideLevel	m_eLevelLine;	// 格线显示优先级标题上根据优先级显示出头的标记
	E_LineStyle		m_eLineStyle;	// 格线的形状
};

// 平滑坐标轴
class CHART_EXPORT CAxisSmooth
{
public:
	CAxisSmooth() 
	{
		m_iPixelWidth	= 0;
		m_fMax			= FLT_MIN;
		m_fMin			= FLT_MAX;
		m_iScale		= 1;
	}

	void SetPixelWidth(int32 iPixelWidth) { m_iPixelWidth = iPixelWidth; }
	
	bool32 GetSpecialPixelValue(int32 iPixel, float &fValue);

public:
	// 
	int32			m_iPixelWidth;		// 像素宽度(该值对应KAxisFloatRefer)

	// 
	float			m_fMax;				// 真实的范围
	float			m_fMin;
	int32			m_iScale;			// 缩放， 默认为1， 如果不是1， 需要显示的值都缩放， 并且醒目的提示缩放量信息
};

// 坐标刻度内容
class CHART_EXPORT CAxisNode
{
public:
	CAxisNode()
	{
//		m_fPosStart		= 0.;
//		m_fPosEnd		= 0.;
		m_fPixelWidthPerUnit = 0.;
		m_iTimeId = 0;
	}

public:
	int32			m_iStartPixel;		// 显示范围
	int32			m_iCenterPixel;
	int32			m_iEndPixel;
	
	// 在大量k线数据情况下，x轴m_iEndPixel与m_iStartPixel差值可能为0，导致费波拉契线死循环,临时使用浮点数保存k线像素宽度
	float			m_fPixelWidthPerUnit;
	
	int32			m_iTimeId;			// X轴坐标保存准确的id信息，防止在数据密集时，X坐标点总是选取第一个id

	CString			m_StrSliderText1;	// 滑块内容（分上下、左右）
	CString			m_StrSliderText2;
};

// 定制坐标轴
class CHART_EXPORT CAxisCustom
{
public:
	CAxisCustom()
	{
		m_iPixelWidth	= 0;
	}
	
public:
	bool32			GetSpecialPixelInfo(int32 iPixel, int32 &iIndex, int32 &iPixelStart, int32 iPixelEnd, CString &StrSliderText1, CString &StrSliderText2);

public:
	// 
	int32			m_iPixelWidth;			// 像素宽度
	
	// 
	CArray<CAxisNode, CAxisNode&>	m_aAxisNodes;	// 分割的每个点文字信息， 位置信息在非Custom类型无意义
};
///////////////////////////////////////////////////////////////////////////////
//切分的方向
enum E_Direction 
{
	EDLeft  = 0,
	EDRight,
	EDTop,
	EDBottom
};

//鼠标操作类型
enum E_MouseOpType 
{
	EMOTDefault = 0,//缺省的鼠标操作
	EMOTPan,//画面平移
	EMOTRuler,//测距
	EMOTUserDrawing //自画线,会有多种鼠标操作,预留扩展.
};

//鼠标拖动Region的边框(Edge)的结构
struct T_PickEdge
{
	CRegion*		m_pRegion;//拾取到的Region
	E_Direction		m_eDir;//拾取到的Region的边
};

//鼠标拖动Region的边框(Edge)后强制(Force)改变Rect的结构
struct T_ForceRegion
{
	CRegion*		m_pRegion;//要改变的Region
	E_Direction		m_eDir;//要改变的边
	int32			m_iNewValue;//边的新值
};

/////////////////////////////////////////////////////////////////////////
//切分Region(多个Region)共用的全局参数类,考虑应用层派生.
class CHART_EXPORT CRegionViewParam
{
public:
	CRegionViewParam ( IN CWnd* pWnd);
	virtual ~CRegionViewParam();

	void			SetParentWnd ( CWnd* pWnd );
	void			UpdateGDIObject();
	void			SetViewToolBarHeight(int32 iViewToolBarHeight);
	CMemDCEx*		GetMainDC();
	CMemDCEx*		GetOverlayDC();
	CMemDCEx*		GetPickDC();
	CWnd*			GetView();
	void			EnableClipDiff ( bool32 bEnable,CRect& Rect );

	// 获取视图允许region分配的区域
	virtual bool32	GetViewRegionRect(OUT LPRECT pRect);

	void			BShowViewToolBar(bool32 bShowIndexToolbar, bool32 bShowTopToolbar);
	void			GetShowViewToolBar(OUT bool32& bShowIndexToolbar, OUT bool32& bShowTopToolbar);

	CArray<T_PickEdge,T_PickEdge>			m_PickEdges;
	CArray<T_ForceRegion,T_ForceRegion>		m_ForceRegions;
	
	bool32			m_bDragEdgeIn;
	int32			m_iLevelMax;
	int32			m_iLayerMax;
	CPoint			m_PointMouse;
	CPoint			m_PointLastMouse;
	CRect			m_RectLast;
	uint32			m_iFlag;
	bool32          m_bRightButPress;

protected:
	CRegionViewParam();	
	void ReleaseMemDC( );

	CClientDC*		m_pDC;
	CMemDCEx*		m_pPickDC;
	CMemDCEx*		m_pMainDC;
	CMemDCEx*		m_pOverlayDC;
	CWnd*			m_pWnd;
	CRect			m_RectView;
	int32			m_iCurveID;
	static int32	g_iID;
private:
	bool32			m_bShowTopBtn;
	bool32			m_bShowIndexBtn;
	int32			m_iViewToolBarHeight;

friend class CRegion;
};

typedef enum E_RegionDrawNotifyType
{
	ERDNBeforeDrawCurve					= 0x01,			// 画曲线前
	ERDNAfterDrawCurve					= 0x02,			// 画曲线后
	ERDNBeforeTransformAllRegion		= 0x04,			// 转换坐标前
//	ERDNAfterDrawAllRegion				= 0x08,			// 画所有Region后
//	ERDNDrawAllOverlay					= 0x10			// 画所有对象
} E_RegionDrawNotifyType;


class CHART_EXPORT CRegionDrawNotify
{
protected:
	virtual void	OnRegionDrawNotify(IN CMemDCEx* pDC, IN CMemDCEx* pPickDC, CRegion* pRegion, E_RegionDrawNotifyType eType ) = 0;
friend class CRegion;
friend class CChartRegion;
};

class CHART_EXPORT CRegionDrawElement
{
public:
	CRegionDrawElement()
	{
		m_pRegionDrawNotify = NULL;
		m_uiDrawFlag		= 0;
	}

public:
	CRegionDrawNotify	*m_pRegionDrawNotify;
	uint32				m_uiDrawFlag;
};

//CRegion类声明
class CHART_EXPORT CRegion
{
public:
	enum E_RegionFlag
	{
		// 可外部用(16位)		
		KRectZoomOut				= 0x00008000,			// 矩形框选放大

		KFixWidth					= 0x00010000,			// 固定宽度
		KFixHeight					= 0x00020000,			// 固定高度
		KRightDragAble				= 0x00040000,			// 右边缘允许拖动
		KBottomDragAble				= 0x00080000,			// 边缘允许拖动
		
		// 内部用(16位)
		KMousePress					= 0x00100000,			// 当前鼠标状态为左键下压
		KMouseDrag					= 0x00200000,			// 当前鼠标状态为拖动
		KMousePressEdge				= 0x00400000,			// 当前鼠标状态为左键下压着Region的边(Edge)
		KMouseDragEdge				= 0x00800000,			// 当前鼠标状态为拖动着Region的边(Edge)
	
		KMouseOverAWhile			= 0x01000000,
		KMouseOverEdge				= 0x02000000,
		KFrame						= 0x04000000,			// 当前Region类型为Frame
		KTooSmall					= 0x08000000,			// 当前Region的尺寸太小(长度或宽度<=KRegionMinWH)
		
		// 绘制参数		
		KDrawTransform				= 0x10000000,
		KDrawNotify					= 0x20000000,
		KDrawCurves					= 0x40000000,
		KDrawNoSwap					= 0x80000000,
		KDrawFull					= KDrawTransform | KDrawCurves | KDrawNotify
	};

public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//构造/析构函数
	CRegion(IN CRegionViewParam* pViewParam, CString StrName, uint32 iFlag = 0);
	virtual ~CRegion ( );
	
public:
	static void		FreeAll(CRegion* pRegion);

	// 虚接口， 鼠标事件， 需要重载
protected:
	// 内部包装了windows的鼠标事件,参数x,y坐标为windows窗口坐标.
	// 可以用CRegion::Region2Client/Client2Region来进行"client<-->Region"之间的坐标转换
	virtual void	OnMousePress  ( int32 x, int32 y, int32 iPass, bool bRightButton = false);//鼠标左键下压
	virtual void	OnMousePressUp( int32 x, int32 y); //鼠标左键弹起
	virtual void	OnMouseRClick ( int32 x, int32 y );//鼠标右键点击,比如弹出菜单
	virtual void	OnMouseDBClick( int32 x, int32 y );//鼠标左键双击
	virtual void	OnMouseMove   ( int32 x, int32 y );//鼠标移动
	
	virtual void	OnMouseOverAWhile ( int32 x, int32 y );//在x,y上停留了一段时间,用于比如:拾取并显示tooltips.
	virtual void	OnMouseOverLeave ( int32 x, int32 y );
	
	virtual void	OnMouseDragMove ( int32 x, int32 y , bool bRightButton = false );//拖动
	virtual void	OnMouseDragDrop ( int32 x, int32 y, bool bRightButton = false );//鼠标按键松开
	
	virtual void	OnMouseClickN ( int32 n, int32 x, int32 y );//第n次点击
	virtual void	OnMouseMoveN  ( int32 n, int32 x, int32 y );//
	virtual void	OnMouseEnd    ( int32 n, int32 x, int32 y );
	
	//键盘事件
	virtual	bool32	OnKeyDown(int32 nChar, int32 nRepCnt, int32 nFlags);
	virtual	bool32	OnChar(int32 nChar, int32 nRepCnt, int32 nFlags);
	virtual	bool32	OnKeyUp(int32 nChar, int32 nRepCnt, int32 nFlags);
	
	//拾取功能:
	//拾取到了PickColor/PickName的线或其它内容(蜡烛线/信息地雷/自画线等)的事件通知
	virtual void	OnPick ( uint32 uiPickColor, int32 x, int32 y );//使用CPixelPickDC的情况
	virtual void	OnPick ( CString StrPickName, int32 x, int32 y );//使用virtual bool32 CDrawingCurve::Pick的情况
	
	//内部事件
	virtual void	OnCreate  ( );//当前Region被插入(创建)后的事件
	virtual void	OnDestroy ( );//当前Region被删除前的事件
	virtual void	OnPreDraw ( IN CMemDCEx* pDC,IN CMemDCEx* pPickDC );
	virtual void	OnDraw    ( IN CMemDCEx* pDC,IN CMemDCEx* pPickDC );//当前Region需要重绘
	virtual void	OnAfterDraw ( IN CMemDCEx* pOverlayDC );
	virtual void	OnActive  ( bool32 bActive );//通知当前Region是bActive,所有Region中,仅有一个是Active
	
	virtual void	OnMessage ( int32 mid,void* pData );
	
	//不要尝试在以上分发到各个子Region的事件中删除Region!!!
	virtual int32	GetRegionMinWH ();
	
	// Region的插入和删除
public:
	bool32			SplitRegion(IN CRegion* pRegion, E_Direction eDir, CSize SizeWish);	// 插入:在当前Region上分割出一个新的Region,长度和宽度>=0
	bool32			Delete();				// 删除:删除当前Region的某个子Region

	void			ReSizeAll();			// 改变大小,由IoViewChart或CRegion(包括派生类)内部调用,但后者一般没有意义.
	void			NestSizeAll();
	void			ReDrawAll();			// 重新绘制,由IoViewChart或CRegion(包括派生类)内部调用,内部会调用所有CRegion的OnDraw虚函数
	
	// 绘图相关 
public:
	bool32			AddDrawNotify(CRegionDrawNotify* pDrawNotify, E_RegionDrawNotifyType eType);
	bool32			RemoveDrawNotify(CRegionDrawNotify* pDrawNotify, E_RegionDrawNotifyType eType);
	void			DrawNotify(CMemDCEx* pDC, IN CMemDCEx* pPickDC, E_RegionDrawNotifyType eType);

public:
	void			SetBottomDragAble(bool32 bAble);	//开放这个接口
	bool32			RegionTooSmall( );

public:
	// CRegion的绘制方面采用了三缓冲:WindowDC<-->MemDC(Overlay)<-->MemDC(Main)
	void			BeginOverlay ( bool32 bFullClient = true);//由IoViewChart或CRegion(包括派生类)内部调用,在Overlayer上绘制一些临时内容.
	void			EndOverlay ();

	// CIoViewChart窗口坐标和Region坐标的转换,单位为像素,x和y均转换
	void			RegionToClient(INOUT CRect& Rect)		{ Rect.InflateRect(m_RectView.left, m_RectView.top, m_RectView.left, m_RectView.top); }
	void			ClientToRegion(INOUT CRect& Rect)		{ Rect.DeflateRect(m_RectView.left, m_RectView.top, m_RectView.left, m_RectView.top); }

	void			RegionToClient(INOUT CPoint& pt)		{ pt += m_RectView.TopLeft(); }
	void			ClientToRegion(INOUT CPoint& pt)		{ pt -= m_RectView.TopLeft(); }	

	void			RegionXToClient(INOUT int32& x)			{ x += m_RectView.left; }	
	void			ClientXToRegion(INOUT int32& x)			{ x -= m_RectView.left; }
	
	void			RegionYToClient(INOUT int32& y)			{ y += m_RectView.top; }
	void			ClientYToRegion(INOUT int32& y)			{ y -= m_RectView.top; }
	
	void			SetLevel(int32 iLevel);
	void			SetBackground(COLORREF color)			{ m_clrBackground = color; }
	void			SetBorderColor(COLORREF color)			{ m_clrBorder = color; if (NULL != m_pRegionParent) { m_pRegionParent->SetBorderColor(color); } }
	
	void			SetDrawFlag(uint32 iFlag);
	void			AddDrawFlag(uint32 iFlag);
	uint32			GetDrawFlag();
	void			ClearDrawFlag();

	void			MessageAll(int32 mid, void* pData );

	CRegion*		GetRegionRoot()							{return m_pRegionRoot;}

	CString			ToString ( );//将当前Region的重要成员变量转换为字符串返回.(调试用)
	CString			ToStringAll ( );//将所有Region的重要成员变量转换为字符串返回.(调试用)
	CString			DirToString ( E_Direction eDir );//将eDir转换为字符串(调试用)
	
	//由IoViewChart调用
	//键盘
	bool32			TestKeyDown(int32 nChar, int32 nRepCnt, int32 nFlags);
	bool32			TestChar(int32 nChar, int32 nRepCnt, int32 nFlags);
	bool32			TestKeyUp(int32 nChar, int32 nRepCnt, int32 nFlags);
	
	//由IoViewChart调用
	//鼠标
	//内部除了处理Region边界的resize之外
	//还根据特定条件进行判定,然后发送到OnMouseXXX中.
	int32			MouseMove( int32 iFlags, int32 x, int32 y );
	int32			MouseWheel ( int32 iFlags,int32 x,int32 y );
	int32			LButtonDblClk( int32 iFlags, int32 x, int32 y );
	int32			LButtonDown( int32 iFlags, int32 x, int32 y );
	int32			LButtonUp( int32 iFlags, int32 x, int32 y );
	int32			RButtonDblClk( int32 iFlags, int32 x, int32 y );
	int32			RButtonDown( int32 iFlags, int32 x, int32 y );
	int32			RButtonUp( int32 iFlags, int32 x, int32 y );

	bool32			Timer ( int32 iTimerID );

protected:
	//先序遍历的递归顺序
	//(1) 访问Self
	//(2) 遍历左子树 
	//(3) 遍历右子树	
	void			GetRegions ( IN CRegion* pParent,INOUT RegionArray& Regions);//将pParent(包括)子Regions放入Regions中.
	void			NestedSize ( IN CRegion* pParent );
	void			NestedPickEdge ( int32 x, int32 y, bool32 bClick);

	void			NestedSizeReverse ( );//NestedSize是根据父节点计算子节点的Rect,而本函数是根据子节点的Rect,计算父节点的Rect/m_fSizePercent;
	void			TestDragEdge ( int32 x, int32 y, OUT int32& iNewX, OUT int32& iNewY, bool32 bSave );
	void			SetDragForceSize ( );
	void			FindNeighborRegion ( IN CRegion* pSrc,E_Direction eDir,IN CRegion* pParent,INOUT RegionArray& Regions ); 
	void			AddToPickEdges ( E_Direction eDir );

public:
	CRect			m_RectView;				// 当前Region的Rect
	CString			m_StrName;				// 当前Region的名称
	CSize			m_SizeWish;				// 保存当前Region的期望的大小,如果有KFixXXXX标志,则尽量使用此大小.
	uint32			m_iFlag;				// 标志,最多保存32个标志位.	

protected:
	int32			m_iLayer;				// 在二叉树中的层级.
	int32			m_iLevel;				// 优先级:用于控制绘制/鼠标事件等的先后次序
	COLORREF		m_clrBackground;
	COLORREF		m_clrBorder;
	bool32			m_bFullOverlay;
	float			m_fSizePercent;			// pChild1占Parent Region大小的百分比.仅对KFrame类型有效
	E_MouseOpType	m_eMouseOpType;			// 当前鼠标操作的类型:缺省/平移画面/测距...
	E_Direction		m_eDirection;			// 当前Region的方向

	// 
protected:
	CRegionViewParam	*m_pViewParam;		// 多个Region组成的二叉树共用的一个参数
	CRegion*		m_pRegionParent;		// 父节点
	CRegion*		m_pRegionChild1;		// 兄弟1
	CRegion*		m_pRegionChild2;		// 兄弟2
	CRegion*		m_pRegionRoot;			// 根节点

private:	
	CArray<CRegionDrawElement, CRegionDrawElement&>	m_aRegionDrawElements;
};


#endif // _REGION_H_
