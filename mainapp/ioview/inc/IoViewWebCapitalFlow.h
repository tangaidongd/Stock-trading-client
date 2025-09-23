#ifndef _IO_VIEW_WEB_CAPITALFLOW_H_
#define _IO_VIEW_WEB_CAPITALFLOW_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "CStaticSD.h"

//////////////////////////////////////////////////////////////////////////
// 假设资金流向具有这些数据
struct T_WebCapitalFlowDataPseudo 
{
	enum E_ValueIndex		// 按照出现的顺序组织数组
	{
		HugeBuy,
		HugeSell,
		BigBuy,
		BigSell,
		MidBuy,
		MidSell,
		SmallBuy,
		SmallSell,

		ValueCount
	};
	float m_fBuyVolHuge;		// 买量 - 超大单
	float m_fBuyVolBig;			// 大单
	float m_fBuyVolMid;			// 中
	float m_fBuyVolSmall;		// 小

	float m_fSellVolHuge;		// 卖量 - 超大单
	float m_fSellVolBig;
	float m_fSellVolMid;
	float m_fSellVolSmall;

	float m_fBuyAmountHuge;			// 买金额 - 超大单
	float m_fBuyAmountBig;			// 大单
	float m_fBuyAmountMid;			// 中
	float m_fBuyAmountSmall;		// 小
	
	float m_fSellAmountHuge;		// 卖金额 - 超大单
	float m_fSellAmountBig;
	float m_fSellAmountMid;
	float m_fSellAmountSmall;

	// 根据实际数据添加字段
	float	m_fAllBuyAmount;		// 买入总额（万元）
	float	m_fAllSellAmount;		// 卖出总额（万元）
	
	float	m_fAmountPerTrans;		// 每笔金额（万元）
	float	m_fStocksPerTrans;		// 每笔股数（股）

	T_WebCapitalFlowDataPseudo()
	{
		memset(this, 0, sizeof(T_WebCapitalFlowDataPseudo));		// 浮点数0表示为0 000.... 伪结构简单话
	}

	bool32 operator == (const T_WebCapitalFlowDataPseudo &cap) const 
	{
		if ( this == &cap )
		{
			return true;
		}

		return m_fBuyVolHuge	         == cap.m_fBuyVolHuge	    
			&& m_fBuyVolBig		       == cap.m_fBuyVolBig		  
			&& m_fBuyVolMid		       == cap.m_fBuyVolMid		  
			&& m_fBuyVolSmall         == cap.m_fBuyVolSmall                      
			&& m_fSellVolHuge	       == cap.m_fSellVolHuge	  
			&& m_fSellVolBig          == cap.m_fSellVolBig     
			&& m_fSellVolMid          == cap.m_fSellVolMid     
			&& m_fSellVolSmall        == cap.m_fSellVolSmall                     
			&& m_fBuyAmountHuge	     == cap.m_fBuyAmountHuge	
			&& m_fBuyAmountBig	       == cap.m_fBuyAmountBig	  
			&& m_fBuyAmountMid	       == cap.m_fBuyAmountMid	  
			&& m_fBuyAmountSmall      == cap.m_fBuyAmountSmall                   
			&& m_fSellAmountHuge      == cap.m_fSellAmountHuge 
			&& m_fSellAmountBig       == cap.m_fSellAmountBig  
			&& m_fSellAmountMid       == cap.m_fSellAmountMid  
			&& m_fSellAmountSmall     == cap.m_fSellAmountSmall ; 
	}

	const T_WebCapitalFlowDataPseudo &operator+=(const T_WebCapitalFlowDataPseudo &cap);
	const T_WebCapitalFlowDataPseudo operator+(const T_WebCapitalFlowDataPseudo &cap);

	float GetSellVol()
	{
		return m_fSellVolBig + m_fSellVolHuge + m_fSellVolMid + m_fSellVolSmall;
	}
	float GetBuyVol()
	{
		return m_fBuyVolBig + m_fBuyVolHuge + m_fBuyVolMid + m_fBuyVolSmall;
	}
	float GetSellAmount()
	{
		return m_fSellAmountHuge + m_fSellAmountBig + m_fSellAmountMid + m_fSellAmountSmall;
	}
	float GetBuyAmount()
	{
		return m_fBuyAmountHuge + m_fBuyAmountBig + m_fBuyAmountMid + m_fBuyAmountSmall;
	}

	void GetVolValueArray(OUT CArray<float, float> &aVolValues)
	{
		aVolValues.RemoveAll();
		aVolValues.SetSize(ValueCount);

		aVolValues[HugeBuy] = m_fBuyVolHuge;
		aVolValues[HugeSell] = m_fSellVolHuge;

		aVolValues[BigBuy] = m_fBuyVolBig;
		aVolValues[BigSell] = m_fSellVolBig;

		aVolValues[MidBuy] = m_fBuyVolMid;
		aVolValues[MidSell] = m_fSellVolMid;

		aVolValues[SmallBuy] = m_fBuyVolSmall;
		aVolValues[SmallSell] = m_fSellVolSmall;
	}

	void GetAmoutValueArray(OUT CArray<float, float> &aValues)
	{
		aValues.RemoveAll();
		aValues.SetSize(ValueCount);
		
		aValues[HugeBuy] = m_fBuyAmountHuge;
		aValues[HugeSell] = m_fSellAmountHuge;
		
		aValues[BigBuy] = m_fBuyAmountBig;
		aValues[BigSell] = m_fSellAmountBig;
		
		aValues[MidBuy] = m_fBuyAmountMid;
		aValues[MidSell] = m_fSellAmountMid;
		
		aValues[SmallBuy] = m_fBuyAmountSmall;
		aValues[SmallSell] = m_fSellAmountSmall;
	}
};



/////////////////////////////////////////////////////////////////////////////
// CIoViewWebCapitalFlow

class CIoViewWebCapitalFlow : public CIoViewBase
{
// Construction
public:
	CIoViewWebCapitalFlow();
	virtual ~CIoViewWebCapitalFlow();

	DECLARE_DYNCREATE(CIoViewWebCapitalFlow)

	enum E_Cycle{
		Cycle_OneDay,
		Cycle_FiveDay,
	};
	
	// from CControlBase
public:
	//virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	// from CIoViewBase
public:
	virtual void	RequestViewData();
	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();

	virtual void	LockRedraw();
	virtual void	UnLockRedraw();
	// from CIoViewBase
public:
	
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();
	

	virtual void	OnIoViewColorChanged();			
	virtual void	OnIoViewFontChanged();


	virtual	bool32	IsPluginDataView() { return true; }
	virtual void    OnVDataPluginResp(const CMmiCommBase *pResp);

	void			OnTickExResp(const CMmiRespTickEx *pResp);		// 分笔统计回包
	void			OnTickExPeriodResp(const CMmiRespPeriodTickEx *pResp);		// 分笔统计push回包
	void			OnTickExHistoryResp(const CMmiRespHistoryTickEx *pResp);
private:
	//typedef CArray<T_TickEx, T_TickEx&>	TickExArray;
	typedef CArray<T_WebCapitalFlowDataPseudo, const T_WebCapitalFlowDataPseudo &> ShowDataArray;

	void		RequestData(CMmiCommBase &req);

	CString		GetCycleString();


	void		RecalcRectSize();

	void		UpdateShowData(const T_WebCapitalFlowDataPseudo &cap);		// 不应该直接调用
	void		UpdateShowData();		// 根据当前周期，提取合适的数据
	void		EmptyShowData();

	void		ChangeCycleType(E_Cycle eCycle);

	void		TickExToShowData(const T_TickEx &tickEx, OUT T_WebCapitalFlowDataPseudo &showData);

	void		CreateWeb();
	void		ShowCustomWeb();

private:
	
	CRect			m_RectTitle;
	CRect			m_RectButton;		// 一日 五日
	CRect			m_RectWeb;			// web
	CRect			m_RectChartRate;
	CRect			m_RectChartVol;
	CRect			m_RectChartAmount;

	CWndCef			*m_pWndCef;				//页面加载
	CString			m_StrCustomUrl;


	E_Cycle			m_eCycle;

	T_WebCapitalFlowDataPseudo		m_capData;		// 资金流向数据 - 显示数据
	
	ShowDataArray		m_aTickExData;
	
////////////////////////////////////////////////////////////
//
protected:
			
		

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewWebCapitalFlow)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewWebCapitalFlow)
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_CAPITALFLOW_H_
