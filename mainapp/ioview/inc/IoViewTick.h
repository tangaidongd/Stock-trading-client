#ifndef _IO_VIEW_TICK_H_
#define _IO_VIEW_TICK_H_

#include "IoViewBase.h"
#include "IoViewChart.h"
#include "MerchManager.h"
#include "Region.h"
#include "ChartRegion.h"
#include "TrendIndex.h"

class CMPIChildFrame;
class CIoViewTick;
class CDlgTrendIndexstatistics;

/////////////////////////////////////////////////////////////////////////////
// CIoViewTick

class CIoViewTick : public CIoViewChart, public CChartRegionViewParam, public CRegionDrawNotify
{
// Construction
public:
	CIoViewTick();
	virtual ~CIoViewTick();
	
	DECLARE_DYNCREATE(CIoViewTick)

	// from CControlBase
public:
	virtual void	DoFromXml();
	virtual void	InitialShowNodeNums();	
// from CIoViewBase
public:	
	virtual void	SetChildFrameTitle();

	// from IoViewChart
public:
	virtual void	CalcLayoutRegions(bool bOnlyUpdateMainRegion = true);
	virtual void	OnIoViewActive();

	virtual void	OnKeyHome();
	virtual void	OnKeyEnd();
	virtual void	OnKeyLeftAndCtrl();
	virtual void    OnKeyLeftAndCtrl(int32 iRepCnt);
	virtual void	OnKeyLeft();
	virtual void	OnKeyRightAndCtrl();
	virtual void    OnKeyRightAndCtrl(int32 iRepCnt);
	virtual void	OnKeyRight();
	virtual void	OnKeyUp();
	virtual void	OnKeyDown();
	
	// 
	virtual void	ClearLocalData(bool32 bClearAll = true);

	// ��ȡX Slider text
	virtual bool32	GetChartXAxisSliderText1(OUT CString &StrSlider, CAxisNode &AxisNode);  
public:
	////////////////////////////////////////////////////////////////////////////////////
	// CIoViewBase�¼�
	// ֪ͨ��ͼ�ı��ע����Ʒ
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	// ������ͼ���
	virtual bool32	OnVDataAddCompareMerch(IN CMerch *pMerch);
	// ֪ͨ���ݸ���
	virtual void	OnVDataForceUpdate();
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch);
	virtual void	OnVDataFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName );
	/////////////////////////////////////////////////////////////////////////////////////
	// CRegionViewParam�¼�
	virtual bool32  OnCanPanLeft ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum );
	virtual bool32  OnCanPanRight ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum );

	//Curve��ɾ��,���������Ƶ�Node��ɾ��,Ӧ�ò��б�Ҫ���Node��Դ����.
	virtual void	OnNodesRelease ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes );
	virtual void	OnPickNode( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes, CNodeData* pNodeData,int32 x,int32 y,int32 iFlag );
	virtual void	OnRegionMenu ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes,CNodeData* pNodeData, int32 x, int32 y );
	virtual void	OnRegionMenu2 ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y );
	virtual void	OnRegionIndexMenu ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y );
	virtual void	OnRegionIndexBtn (CChartRegion* pRegion, CDrawingCurve* pCurve, int32 iID );
	virtual void	OnDropCurve ( CChartRegion* pSrcRegion,CChartCurve* pSrcCurve,CChartRegion* pDestChart);
	virtual void	OnRegionCurvesNumChanged ( CChartRegion* pRegion, int32 iNum );
	virtual void	OnCrossData ( CChartRegion* pRegion,int32 iPos,CNodeData& NodeData, float fPricePrevClose, float fYValue, bool32 bShow);
	virtual void	OnCrossNoData (CString StrTime,bool32 bShow = true);
	virtual CString OnTime2String ( CGmtTime& Time );
	virtual CString OnFloat2String ( float fValue, bool32 bZeroAsHLine = true, bool32 bNeedTerminate = false);

	virtual void	OnCalcXAxis ( CChartRegion* pRegion, CDC* pDC, CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide);
	virtual void	OnCalcYAxis ( CChartRegion* pRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);

	virtual CString OnGetChartGuid ( CChartRegion* pRegion );

	virtual void	OnSliderId ( int32& id, int32 iJump );
	virtual IChartBrige GetRegionParentIoView();
	virtual int32 GetChartType(){return m_iChartType;}
	virtual	void	OnRectZoomOut(int32 iNodeBegin, int32 iNodeEnd);

public:
	// �ɱ��ѻ�
	void			DrawNcp(IN CMemDCEx* pDC);
	
	// ��ʷ��ʱ���
	void			GetCrossTick(OUT CTick & TickIn);

	//
	bool32			IsShowNewestTick(OUT CGmtTime& TimeStartInFullList, OUT CGmtTime &TimeEndInFullList);	// �жϵ�ǰ�Ƿ���ʾ����һ��K��, ������ǣ� �򴫳���ǰ��ʾ��ʱ�䷶Χ
	bool32			UpdateMainMerchTick(T_MerchNodeUserData &MerchNodeUserData);
	bool32			UpdateSubMerchTick(T_MerchNodeUserData &MerchNodeUserData);

	bool32			UpdateMainMerchTick2(T_MerchNodeUserData &MerchNodeUserData);

	bool32			CalcMainRegionXAxis(CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aAxisDivide);
	void			CalcMainRegionYAxis(CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);
	void			CalcSubRegionYAxis(CChartRegion* pChartRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);

	// ...fangz0729 ʮ�ֹ���״̬��λ��Ӧ��K �߱���.��Ҫ�ŵ�ChartRegion ��.
	void			SetCrossTickIndex(int32 iPos)	{ m_iCurCrossTickIndex = iPos; }
	int32			GetCrossTickIndex()			{ return m_iCurCrossTickIndex; }

	void			SetRequestNodeFlag(bool32 bTrue){ m_bRequestNodesByRectView = bTrue;}

	// ����tick
	void			SetTickMulitDay(int32 iMultiDay, bool32 bReqData = true);		// ���ö��շ�ʱ
	bool32			GetTickMultiDayTimeRange(OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd);	// ��ȡ���շ�ʱ��ʱ���
	bool32			IsTickZoomed();	// �Ƿ�������
	void			CancelZoom();	// ȡ������
	bool32			GetZoomedTimeRange(OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd);	// ��ȡ����״̬�µ�ʱ���
	bool32			GetShowTimeRange(OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd);	// ���ݵ�ǰ״̬��ȡ��ʾ��ʱ���
	
	void			FillMainVirtualKLineData(const CArray<CKLine, CKLine> &aSrc, OUT CArray<CKLine, CKLine> &aDst, OUT CArray<DWORD, DWORD> &aDstFlag);		// �������Ʒ���������
	void			FillMainVirtualShowData();		// �������Ʒ���������

	void			CalcKLineAvgValue(INOUT CArray<CKLine, CKLine> &aKlines);
	
	// from CView
protected:
	virtual void	OnRegionDrawNotify(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC,CRegion* pRegion,E_RegionDrawNotifyType eType);
	virtual void	CreateRegion();
	virtual void	SplitRegion();
	virtual void	SetXmlSource ();
	virtual void	SetCurveTitle ( T_MerchNodeUserData* pData );
	virtual void	InitCtrlFloat ( CGridCtrl* pGridCtrl );
	virtual void	SetFloatData ( CGridCtrl* pGridCtrl,CTick& Tick,float fPricePrevClose,float fCursorValue,CString StrTimeLine1,CString StrTimeLine2,int32 iSaveDec);
	virtual void	SetFloatData ( CGridCtrl* pGridCtrl,float fCursorValue,int32 iSaveDec);
	virtual void	ClipGridCtrlFloat (CRect& rect);

	void	SetFloatData ( CGridCtrl* pGridCtrl,CKLine& KLine,float fPricePrevClose,float fCursorValue,CString StrTimeLine1,CString StrTimeLine2,int32 iSaveDec);

private:
	void			MenuAddRegion();
	void			MenuDelRegion();
	void			MenuDelCurve();
	void			MenuAddIndex ( int32 id);
	
	void			UpdateTickStyle();
	bool32			OnZoomOut();
	bool32			OnZoomIn();

	void			RequestViewData();			// �ж��Ƿ��б�Ҫ����������ݣ� ��һ������

	void			RequestSequenceTickStart();	// ��ʼһ��һ������tick����
	void			RequestSequenceTickNext();	// ����һ��һ������tick����

	void			UpdateTickMultiDayTitle();	// ����tick�Ķ��ձ������� 
	float			GetTickPrevClose();			// ��ȡtickͼ�����ռ�
	bool32			CalcKLinePriceBaseValue(CChartCurve *pCurve, OUT float &fPriceBase);

	void			FillAppendMainXPosNode(const CArray<CNodeData, CNodeData&> &aSrc, OUT CArray<CNodeData, CNodeData&> &aNewNodes);	// ����ʱ�����Ϣ�����ĩβ��ռλ�ڵ�
	bool32			FindPosInAllTimeIds(int32 iTimerId, OUT int32 &iPos);

private:
	bool32			m_bRequestNodesByRectView;	// �½�һ��K �ߵ�ʱ��,���ݵ�ǰ��ʾ����Ĵ�С����K ��.������ʾ�Ĳ�Э��(��������)
	CTick			m_TickCrossNow;

	int32			m_iCurCrossTickIndex;		// ��ǰ��Ļ��ʮ�ֹ����ʾλ��
	int32			m_iNodeCountPerScreen;		// ������Ļ��һ����ʾK������

	int32			m_iTickMultiDay;				// ����Ҫ����
	// ��ÿ����������ʱ�����¼��������
	CArray<CMarketIOCTimeInfo, const CMarketIOCTimeInfo &>	m_aTickMultiDayIOCTimes; // ÿ��Ŀ����̣���һ��Ϊ���죬����Ϊǰn��
	CGmtTime		m_TimeZoomStart;				// ���ŵĿ�ʼʱ��, ��Χ����ѡ���Ľ����յ�ʱ�����
	CGmtTime		m_TimeZoomEnd;					// ���ŵĽ���ʱ��
	CArray<int32, int32>	m_aAllXTimeIds;			// ����x��(������ǰû��ʾ�ĵ������޶���ʱ����)��ʱ��ڵ�

	CMmiReqMerchTimeSales	m_MmiReqTimeSales;			// ������ʷ�ֱ�

/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewTick)
	virtual BOOL TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewTick)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnMenu ( UINT nID );
	afx_msg LRESULT OnMessageTitleButton(WPARAM wParam,LPARAM lParam);
	afx_msg void OnMySettingChanged();
	//afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_TICK_H_
