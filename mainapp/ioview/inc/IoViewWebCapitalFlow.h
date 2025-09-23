#ifndef _IO_VIEW_WEB_CAPITALFLOW_H_
#define _IO_VIEW_WEB_CAPITALFLOW_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "CStaticSD.h"

//////////////////////////////////////////////////////////////////////////
// �����ʽ����������Щ����
struct T_WebCapitalFlowDataPseudo 
{
	enum E_ValueIndex		// ���ճ��ֵ�˳����֯����
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
	float m_fBuyVolHuge;		// ���� - ����
	float m_fBuyVolBig;			// ��
	float m_fBuyVolMid;			// ��
	float m_fBuyVolSmall;		// С

	float m_fSellVolHuge;		// ���� - ����
	float m_fSellVolBig;
	float m_fSellVolMid;
	float m_fSellVolSmall;

	float m_fBuyAmountHuge;			// ���� - ����
	float m_fBuyAmountBig;			// ��
	float m_fBuyAmountMid;			// ��
	float m_fBuyAmountSmall;		// С
	
	float m_fSellAmountHuge;		// ����� - ����
	float m_fSellAmountBig;
	float m_fSellAmountMid;
	float m_fSellAmountSmall;

	// ����ʵ����������ֶ�
	float	m_fAllBuyAmount;		// �����ܶ��Ԫ��
	float	m_fAllSellAmount;		// �����ܶ��Ԫ��
	
	float	m_fAmountPerTrans;		// ÿ�ʽ���Ԫ��
	float	m_fStocksPerTrans;		// ÿ�ʹ������ɣ�

	T_WebCapitalFlowDataPseudo()
	{
		memset(this, 0, sizeof(T_WebCapitalFlowDataPseudo));		// ������0��ʾΪ0 000.... α�ṹ�򵥻�
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

	void			OnTickExResp(const CMmiRespTickEx *pResp);		// �ֱ�ͳ�ƻذ�
	void			OnTickExPeriodResp(const CMmiRespPeriodTickEx *pResp);		// �ֱ�ͳ��push�ذ�
	void			OnTickExHistoryResp(const CMmiRespHistoryTickEx *pResp);
private:
	//typedef CArray<T_TickEx, T_TickEx&>	TickExArray;
	typedef CArray<T_WebCapitalFlowDataPseudo, const T_WebCapitalFlowDataPseudo &> ShowDataArray;

	void		RequestData(CMmiCommBase &req);

	CString		GetCycleString();


	void		RecalcRectSize();

	void		UpdateShowData(const T_WebCapitalFlowDataPseudo &cap);		// ��Ӧ��ֱ�ӵ���
	void		UpdateShowData();		// ���ݵ�ǰ���ڣ���ȡ���ʵ�����
	void		EmptyShowData();

	void		ChangeCycleType(E_Cycle eCycle);

	void		TickExToShowData(const T_TickEx &tickEx, OUT T_WebCapitalFlowDataPseudo &showData);

	void		CreateWeb();
	void		ShowCustomWeb();

private:
	
	CRect			m_RectTitle;
	CRect			m_RectButton;		// һ�� ����
	CRect			m_RectWeb;			// web
	CRect			m_RectChartRate;
	CRect			m_RectChartVol;
	CRect			m_RectChartAmount;

	CWndCef			*m_pWndCef;				//ҳ�����
	CString			m_StrCustomUrl;


	E_Cycle			m_eCycle;

	T_WebCapitalFlowDataPseudo		m_capData;		// �ʽ��������� - ��ʾ����
	
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
