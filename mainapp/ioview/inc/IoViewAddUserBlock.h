// IoViewValue.h: interface for the CIoViewAddUserBlock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_IOVIEW_ADD_USER_BLOCK_H_)
#define _IOVIEW_ADD_USER_BLOCK_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IoViewBase.h"
#include "CStaticSD.h"
#include "XScrollBar.h"
#include "GridCtrlSys.h"
#include "ReportScheme.h"

class CIoViewAddUserBlock : public CIoViewBase ,public CObserverUserBlock
{
public:
	CIoViewAddUserBlock();
	virtual ~CIoViewAddUserBlock();

	DECLARE_DYNCREATE(CIoViewAddUserBlock)

	// virtual from ioviewbase
public: 
	
	virtual bool32	FromXml(TiXmlElement * pElement);
	virtual CString	ToXml();
	virtual CString	GetDefaultXML(){ return L"";}

	virtual void	SetChildFrameTitle();
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();

	virtual void	OnVDataForceUpdate();
	virtual void	RequestViewData();
	
	// 通知视图改变关注的商品
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);

	// from CObserverUserBlock 通知
	virtual void	OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType);	

	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch);
	virtual void	OnVDataGeneralFinanaceUpdate(CMerch* pMerch);

	// 字体发生变化
	virtual void	OnIoViewFontChanged();
	virtual void	OnIoViewColorChanged();

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
					
private:
	// 创建表格
	bool32			CreateTable(E_ReportType eMerchKind);

	// 设置画线
	void			SetDrawLineInfo();

	// 设置尺寸
	void			SetSize();

	// 计算涨跌停价格
	bool32			CalcRiseFallMaxPrice(OUT float& fRiseMax, OUT float& fFallMax);

	// 是否是大盘指数
	bool32			BeGeneralIndex(OUT bool32& bH);

	E_MerchReportField ReportHeader2MerchReportField( CReportScheme::E_ReportHeader eReportHeader,bool32& bTrans );
	E_MerchReportField ReportHeader2MerchReportFieldBeiJing( CReportScheme::E_ReportHeader eReportHeader,bool32& bTrans );
	CReportScheme::E_ReportHeader  MerchReportField2ReportHeader(  E_MerchReportField eField,bool32& bTrans );
	void OnBtnResponseEvent(UINT nButtonId);
	void CreateAddImpBtn();

	bool32			SetUserBlockHeadInfomationList();
	void			ReSetGridHead();		
	void			UserBlockPagejump();
	
	//
	int             TButtonHitTest(CPoint point);

public:
	static	const SimpleTabClassMap &GetSimpleTabClassConfig();

private:

	// 每个页面的所有信息
	CArray<T_TabInfo, T_TabInfo&>	m_aTabInfomations;

	// 每个Tab 的表头信息 (固定列, 表头数组, 滚动条位置)
	int32							m_iFixCol;
	CArray<T_HeadInfo,T_HeadInfo>	m_ReportHeadInfoList;
	int32							m_iCurTab;
	E_ReportType					m_eMarketReportType;
	bool32                          m_bIsFinance;				// 是否点击左侧“基本财务”按钮
	T_SimpleTabInfoClass			m_SimpleTabInfoClass;		// 属于这个报价表tab配置
	static	SimpleTabClassMap		sm_mapSimpleTabConfig;		// 配置

	bool32							m_bRequestViewSort;

	CXScrollBar						m_XSBVert;
	CXScrollBar						m_XSBHorz;

	CNCButton						m_BtnImpUserBlk;			// 导入自选股
	CNCButton						m_BtnAddUserBlk;			// 增加自选股
	std::map<int, CNCButton>		m_mapButton;			   // 增加导入按钮列表	
	Image							*m_pImgBtn;
	CRect							m_RectGrid;
	
	E_ReportType     m_eMerchKind;

	int			     m_iXButtonHovering;			// 标识鼠标进入按钮区域

protected:
	CGridCtrlSys	 m_GridCtrl;

	//{{AFX_MSG(CIoViewAddUserBlock)
	afx_msg	void OnPaint();	
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
};

#endif // !defined(_IOVIEWVALUE_H_)
