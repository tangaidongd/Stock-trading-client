#ifndef _IOVIEW_CHOUMA_H_
#define _IOVIEW_CHOUMA_H_

#include "IoViewBase.h"
#include "ChengBenFenBu.h"
#include "AxisYObject.h"

/////////////////////////////////////////////////////////////////////////////
// CIoViewChouMa

#define CHOUMA_BASE_KLINECOUNT (200)			// 基本计算200根k线
#define CHOUMA_MAX_KLINECOUNT  (600)			// 最多计算n根k线

class CIoViewKLine;

struct T_ChouMaNotify			// 筹码通知结构
{
	CIoViewKLine *pIoViewKLine;
	int32 iFlag;
	WPARAM wParam;
	LPARAM lParam;
};

// 现在请求200根日k线数据看
class CIoViewChouMa : public CIoViewBase
{
public:
	struct T_CBFBParam
	{
		float			m_fTradeRateRatio;			// 换手率放大系数 - 历史换手衰减系数
		E_ChengBenFenBuSuanFa m_eChengBenFenBuSuanFa;	// 成本分布算法 目前只有平均
		float		m_fDefaultTradeRate;		// 无流通盘数据时，缺省的换手率
		UINT		m_uiDivide;					// 分成多少份
		ChengBenFenBuCycleArray m_aCBFBRedYellowCycles;	// 成本分布红黄的周期
		ChengBenFenBuCycleArray m_aCBFBBlueGCycles;		// 成本分布蓝绿周期
		
		T_CBFBParam();
	};
// Construction
public:
	CIoViewChouMa();
	virtual ~CIoViewChouMa();


	DECLARE_DYNCREATE(CIoViewChouMa)
	
	// from CControlBase
public:

	// from CIoViewBase
public:
	virtual void		RequestViewData();	
	virtual void		SetChildFrameTitle();
	virtual bool32		FromXml(TiXmlElement *pElement);
	virtual CString		ToXml();
	virtual	CString		GetDefaultXML();
	
	virtual void		OnIoViewActive();
	virtual void		OnIoViewDeactive();
	virtual bool32		GetStdMenuEnable(MSG* pMsg) { return true; }
	
	virtual void		OnWeightTypeChange();
	virtual void		LockRedraw();
	virtual void		UnLockRedraw();
	
public:
	// from CIoViewBase
	virtual void		OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void		OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void		OnVDataForceUpdate();

	virtual void		OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType);
	virtual	void		OnVDataMerchKLineUpdate(IN CMerch *pMerch);

	virtual void		OnIoViewFontChanged();
	virtual void		OnIoViewColorChanged();

public:

	static void	GetCBFBParam(OUT T_CBFBParam &param);
	static void SetCBFBParam(const T_CBFBParam &param);

	static void OnChouMaNotify(const T_ChouMaNotify &notify);		// 通知所有与k线相关视图，发生了变更

	void	OnCBFBParamChanged(); // 数据参数变更了

	void	SetLastFocusPrice(float fPrice);

	void	InitializeFirstKline();		// 给与自己一个初始化的k线视图

	// K线视图相关
	// 同组同窗口k线激活时，设置其为筹码窗口的事件窗口
	// 当本身第一次初始化时会寻找同窗口同组的k线视图来执行第一次初始化
	// 其后的消息响应仅响应当前关注的k线视图的消息
	void	OnIoViewKLineActive(CIoViewKLine *pIoViewKline);		
	void	OnIoViewKLineYSizeChange(CIoViewKLine *pIoViewKline);	// K线视图的y轴的值或者密度发生了变化, 需要重新计算??
	void	OnIoViewKLineMainMerchDataUpdate(CIoViewKLine *pIoViewKline);	// K线视图的K线数据计算发生了变化, 与对应的k线视图用相同数据
	void	OnIoViewKLineXAxisMayChange(CIoViewKLine *pIoViewKline);
	void	OnKLineMouseMove(CIoViewKLine *pIoViewKline, float fPrice, UINT uTimeId);			// 


	int32	PriceYToClientY(float fPriceY);		// 坐标轴互相转换, 效率低了点, 暂时不需要y到价格转
private:
	void	RecalcLayout();
	void	DrawMyText(CDC *pDC);

	// 绘制基本图形
	void	DrawBasicNodesChart(CDC &dc, const ChengBenFenBuNodeArray &aNodes, float fVolMin, float fVolMax, float fPriceClose, int32 iPriceHalfVol, int32 iPriceHalfVolX, bool32 bUseYellow = true);

	void	CalcShowData();	// 计算显示数据

	// 坐标转换，如果不能转换，则返回默认值
	int32	PriceToChouMaY(float fPrice, int32 iDefault = 0);
	//float   KlineYToPrice(int32 iKlineScreenY, float fDefault = 0.0f);

	static bool32	ReadParamFromFile(OUT T_CBFBParam &param);
	static void		SaveParamToFile(const T_CBFBParam &param);
	
////////////////////////////////////////////////////////////
private:
	
	CRect				m_RectTitle;		// 标题
	CRect				m_RectChart;		// 图形

	CArray<CRect, const CRect &>  m_RectBtns;	// 按钮区域, 屏幕坐标
	int32				m_iCurBtn;				// 条状 红黄 蓝绿 (设置按钮不能选中)
	CImageList			m_ImageList;

	float				m_fLastPrice;		// 最后关注的价格

	CArray<CKLine, CKLine> m_KLines;		// K线序列

	ChengBenFenBuNodeMap m_mapNodes;

	void			   *m_pIoViewKlineFocus;	// 当前关注的k线视图, 有可能该指针指向的K线已经销毁，所以不应当在接口通知以外的任何地方访问该指针
	UINT				m_uTimeEnd;				// 默认请求数据时，仅请求这个时间点前200根，如果本来有多的，还是计算最多800根
	CRect				m_RectKlineCurve;		// k线视图主图k线数据窗口坐标
	float				m_fKlineMinY, m_fKlineMaxY;	// k线视图最小 最大y值
	CPriceToAxisYObject m_AxisYCalcObject;		// Y轴计算

	bool32				m_bNeedCalc;		// 是否需要重新计算

	static T_CBFBParam s_cbfbParam;		// 全局共用一个参数
	

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewChouMa)
	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewChouMa)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IOVIEW_CHOUMA_H_