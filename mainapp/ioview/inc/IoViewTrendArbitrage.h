#ifndef _IO_VIEW_TRENDARBITRAGE_H_
#define _IO_VIEW_TRENDARBITRAGE_H_

#include "IoViewBase.h"
#include "IoViewChart.h"
#include "MerchManager.h"
#include "Region.h"
#include "ChartRegion.h"
#include "AxisYObject.h"

#include "ArbitrageManage.h"

class CMPIChildFrame;

/////////////////////////////////////////////////////////////////////////////
// CIoViewTrendArbitrage

// ʵ�������ӿ���~~
class CIoViewTrendArbitrage : public CIoViewChart, public CChartRegionViewParam, public CRegionDrawNotify, public CArbitrageNotify
{
// Construction
public:
	CIoViewTrendArbitrage();
	virtual ~CIoViewTrendArbitrage();
	
	DECLARE_DYNCREATE(CIoViewTrendArbitrage)

	// from CControlBase
public:
	virtual bool32	FromXml(TiXmlElement * pTiXmlElement);
	virtual void	DoFromXml();
	virtual void	InitialShowNodeNums(){};

	virtual bool32  FromXmlInChild(TiXmlElement *pTiXmlElement);
	virtual CString ToXmlInChild();
	virtual CString CIoViewTrendArbitrage::ToXmlEleInChild();
	virtual	CMerch *GetMerchXml();
	virtual	E_IoViewType	GetIoViewType() { return  EIVT_MultiMerch; }

	virtual	bool32	OnSpecialEsc();
// from CIoViewBase
public:	
	virtual void	SetChildFrameTitle();
	// from IoViewChart
public:
	virtual void	CalcLayoutRegions(bool bOnlyUpdateMainRegion = true);
	virtual void	OnIoViewActive();	
	virtual void 	OnIoViewDeactive();

	virtual void    OnKeyHome();
	virtual void    OnKeyEnd();
	virtual void	OnKeyLeftAndCtrl();
	virtual void    OnKeyLeftAndCtrl(int32 iRepCnt);
	virtual void	OnKeyLeft();
	virtual void	OnKeyRightAndCtrl();
	virtual void    OnKeyRightAndCtrl(int32 iRepCnt);
	virtual void	OnKeyRight();
	virtual void	OnKeyUp();
	virtual void	OnKeyDown();
	virtual void	OnKeySpace();

	// ��ȡ��ͼ�ı�������ť:
	virtual int32   GetSubRegionTitleButtonNums();
	virtual const T_FuctionButton*	GetSubRegionTitleButton(int32 iIndex);

	// ��ͼ�Ż� K�߻�ȡX��������ʾ
	virtual bool32	GetChartXAxisSliderText1(OUT CString &StrSlider, CAxisNode &AxisNode);

	// 
	virtual void	OnRectZoomOut(int32 iNodeBegin, int32 iNodeEnd){};

	//
	virtual	void	ClearLocalData(bool32 bClearAll = true); 
	virtual void    OnRectIntervalStatistics(int32 iNodeBegin, int32 iNodeEnd){};

public:
	////////////////////////////////////////////////////////////////////////////////////
	// CIoViewBase�¼�
	// ֪ͨ��ͼ�ı��ע����Ʒ
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	
	// ������ͼ���
	virtual bool32	OnVDataAddCompareMerch(IN CMerch *pMerch);

	// ֪ͨ���ݸ���
	virtual void	RequestViewData();
	virtual void	OnVDataForceUpdate();
	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName );
	virtual void	OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType);	
	void			OnWeightTypeChange();
	virtual void	OnVDataLandMineUpdate(IN CMerch* pMerch){};	// ��Ϣ���׸���
	/////////////////////////////////////////////////////////////////////////////////////
	// CRegionViewParam�¼�
	virtual bool32  OnCanPanLeft ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum );
	virtual bool32  OnCanPanRight ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum );
	virtual bool32	GetTips(IN CPoint pt, OUT CString& StrTips, OUT CString &StrTitle);

	//Curve��ɾ��,���������Ƶ�Node��ɾ��,Ӧ�ò��б�Ҫ���Node��Դ����.
	virtual void	OnNodesRelease ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes );
	virtual void	OnPickChart(CChartRegion *pRegion, int32 x, int32 y, int32 iFlag);
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
	virtual CString OnFloat2String ( float fValue,bool32 bZeroAsHLine = true, bool32 bNeedTerminate = false );

	virtual void	OnCalcXAxis ( CChartRegion* pRegion, CDC* pDC, CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide);
	virtual void	OnCalcYAxis ( CChartRegion* pRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);

	virtual CString OnGetChartGuid ( CChartRegion* pRegion );

	virtual void	OnSliderId ( int32& id, int32 iJump );
	virtual IChartBrige GetRegionParentIoView();
	virtual int32 GetChartType(){return m_iChartType;}
	virtual	void	OnChartDBClick(CPoint ptClick, int32 iNodePos);	// ˫���¼�
	virtual bool32	OnGetCurveTitlePostfixString(CChartCurve *pCurve, CPoint ptClient, bool32 bHideCross, OUT CString &StrPostfix);

	virtual	void	OnIoViewColorChanged();

public:
	// ��������
	virtual void OnArbitrageAdd(const CArbitrage& stArbitrage);
	
	// ɾ������
	virtual void OnArbitrageDel(const CArbitrage& stArbitrage);
	
	// �޸�����
	virtual void OnArbitrageModify(const CArbitrage& stArbitrageOld, const CArbitrage& stArbitrageNew);


	// ���õ�ǰ����
	bool32		SetArbitrage(IN const CArbitrage &arb);
	bool32		GetArbitrage(OUT CArbitrage &arb);

	// ��������
	void		CalcArbitrageAsyn();
	void		CancelCalcArbAsyn();
	bool32		CalcArbitrage();

	bool32		InitMainUserData();	// ��ʼ��mainuserdata���ݣ����й�ע����Ʒ����������userdata�е�λ�ã�����main���ݱ����˺ϳɺ�Ĳ������
	T_MerchNodeUserData	*GetMainData(bool32 bCreateIfNotExist=false);	// main�д�ŵ���Ʒָ��������
	T_MerchNodeUserData *GetMerchData(CMerch *pMerch, bool32 bCreateIfNotExist=false);	// ��ȡ��Ʒ��Ӧ������
	bool32		ResetMerchUserData(T_MerchNodeUserData *pMerchData);	// ������Ʒ��ص�����

	void		TestSetArb(bool32 bAsk=false);

	void		UpdateMerchDataTradeTime(T_MerchNodeUserData *pMerchData);	// ���½���ʱ��
	void		UpdateAllMerchDataTradeTimes();
	void		CombineMainDataTradeTime();			// �ϲ�main�Ľ���ʱ�䣬ȡ�����Ʒ�Ľ���

	bool32			FillTrendNodes(T_MerchNodeUserData &MerchNodeUserData, const CGmtTime &TimeNow, const CKLine *pKLines, int32 iCountKLine);			
	bool32			FillSubMerchTrendNodes(T_MerchNodeUserData &MerchNodeUserData, const CGmtTime &TimeNow, const CKLine *pKLines, int32 iCountKLine);
	float			GetMerchTrendPrevClose(T_MerchNodeUserData &MerchNodeUserData);

	void		OnF5();	// �л�
public:
	 
	// ��ʷ��ʱ���
	E_NodeTimeInterval GetTimeInterval();
	void			GetCrossKLine(OUT CKLine & KLineIn);

	//
	bool32			GetTrendOpenCloseTime(OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd);	// ��ȡ��ʱͼ�Ŀ���ʱ��[������]

	bool32			UpdateMainMerchKLine(T_MerchNodeUserData &MerchNodeUserData, bool32 bForceUpdate = false);
	bool32			UpdateSubMerchKLine(T_MerchNodeUserData &MerchNodeUserData, bool32 bForceUpdate=true);

	bool32			CalcMainRegionXAxis(CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aAxisDivide);
	void			CalcMainRegionYAxis(CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);
	void			CalcSubRegionYAxis(CChartRegion* pChartRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);

	// from CView
protected:
	virtual void	OnRegionDrawNotify(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC,CRegion* pRegion,E_RegionDrawNotifyType eType);
	virtual void	CreateRegion();
	virtual void	SplitRegion();
	virtual void	SetXmlSource ();
	virtual void	SetCurveTitle ( T_MerchNodeUserData* pData );
	virtual void	InitCtrlFloat ( CGridCtrl* pGridCtrl );
	virtual void	SetFloatData ( CGridCtrl* pGridCtrl,CKLine& KLine,float fPricePrevClose,float fCursorValue,CString StrTimeLine1,CString StrTimeLine2,int32 iSaveDec);
	virtual void	SetFloatData ( CGridCtrl* pGridCtrl,float fCursorValue,int32 iSaveDec);
	virtual void	ClipGridCtrlFloat (CRect& Rect);

	void			AdjustCtrlFloatContent();	// �������鴰������

private:
	
	bool32			LoadAllIndex(CNewMenu* pMenu);
	int32			LoadOftenIndex(CNewMenu* pMenu);
	bool32			LoadOwnIndex(CNewMenu* pMenu);
	void			MenuAddRegion();
	void			MenuDelRegion();
	void			MenuDelCurve();
	void			MenuAddIndex ( int32 id);
	
	
	bool32			OnZoomOut(){ return false; };
	bool32			OnZoomIn(){ return false; };

public:

public:
	
private:

private:	
	
	CKLine			m_KLineCrossNow;
	CStringArray	m_aAllFormulaNames;			// ����ָ��: ÿ��ID ��Ӧ������һ������

	// t..fangz1010
	CArray<CAxisDivide, CAxisDivide&>	m_aYAxisDivideBkMain;	// ��ͼ y��ָ��߱���,�����л�ʱ��������Ʒ����ʱ��ͼ
	CArray<CAxisDivide, CAxisDivide&>	m_aYAxisDivideBkSub;	// ��ͼ y��ָ��߱���,�����л�ʱ��������Ʒ����ʱ��ͼ
	
	//////////////////////////////////////////////////////////////////////////
	

	// ���� ���������ݸ��£�������ʽ���������ɾ��(!)����ǰ����ע����������ȵȽӿ�
	CArbitrage		m_Arbitrage;		// ��ǰ����ע������ģ�ͣ�ֻ�ܹ�עһ��~~
	// ��������������cmp���cmp��Ʒ��λ�ô�� T_MerchUserDataֻ����������ݣ�MainUserData�������ʵ�ʵĺϳ����ݣ�����Ʒ�޹�

private:
	
private:

public:
	
	//
	
	//
	bool32				AddShowIndex(const CString &StrIndexName, bool32 bDelRegionAllIndex = false, bool32 bChangeStockByIndex = false, bool32 bShowUserRightDlg=false);		// ���Ӹ�ָ�꣬������ͼ����ͼ��ȡ��ͬ���ж�

	void				RemoveShowIndex(const CString &StrIndexName);	// ɾ��ָ�꣬������ͼ �� ��ȡ��ͬ
	void				DelCurrentIndex();
	void				ChangeIndexToMainRegion(const CString &StrIndexName);				// �����ͼָ������
	void				ReplaceIndex(const CString &StrOldIndex, const CString &StrNewIndex, bool32 bDelRegionAllIndex = false, bool32 bChangeStock = false);	// ʹ����index�滻ָ��index
	// �ı�ָ��λ�õĸ�ͼָ��, �����λ�ò����ڣ������һ���µĸ�ͼ��������ָ�����,
	//  bAddSameIfExist ָ�����и�ͼ�м�ʹ���ڸ�ָ�꣬�԰�����������ӣ�����ȡ���˴����
	//  iSubRegionIndex -1 ����µĸ�ͼ���κ���Ч��ͼindexΪָ��λ��
	void				AddIndexToSubRegion(const CString &StrIndexName, bool32 bAddSameIfExist = true, int32 iSubRegionIndex = -1); 
	void				AddIndexToSubRegion(const CString &StrIndexName, CChartRegion *pSubRegion);
	void				ClearRegionIndex(CChartRegion *pRegion, bool32 bDelExpertTip = false); // ���ָ��region��ָ��
	void				GetCurrentIndexNameArray(OUT CStringArray &aIndexNames); // ��ȡ��ǰ��������ָ������, ָ�����ƶ���Ψһ�ģ�so

	// �õ�ĳ��region ����ĳ���ָ������
	void				GetOftenFormulars(IN CChartRegion* pRegion, OUT CStringArray& aFormulaNames, OUT int32& iSeperatorIndex, bool32 bDelSame = true);
	void				ChangeToNextIndex(bool32 bPre);

	// �򵥵�����Ʒ
	virtual bool32				AddCmpMerch(CMerch *pMerchToAdd, bool32 bPrompt, bool32 bReqData){ return false; };
	virtual void				RemoveCmpMerch(CMerch *pMerch){};
	virtual void				RemoveAllCmpMerch(){};
	virtual void				OnShowDataTimeRangeChanged(T_MerchNodeUserData *pData);	// ��ʾ������
	// ��ͼ��Ʒ����ʾ�����������е��˺���(UpdateMainMerchʱ�������)
	bool32				ChangeMainDataShowData(T_MerchNodeUserData &MainMerchNode, int32 iShowPosInFullList, int32 iShowCountInFullList);

	bool32				CalcKLinePriceBaseValue(CChartCurve *pCurve, OUT float &fPriceBase);
	float				GetTrendPrevClose();		// �������ռ�ȡ��

	CChartCurve			*GetCurIndexCurve(CChartRegion *pRegionParent);	// ��ȡָ��region�µ�ָ����

	int32				CompareKLinesChange(const CKLine *pKLineBef, int32 iBefCount, const CKLine *pKLineAft, int32 iAftCount);		// �ж�����K �ߵ���ͬ. 0:��ͬ 1:����һ���仯 2:������һ�� 3:�����������,�д�ı仯

public:

public:

/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewTrendArbitrage)
	virtual BOOL TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewTrendArbitrage)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnAllIndexMenu(UINT nID);
	afx_msg void OnOftenIndexMenu(UINT nID);
	afx_msg void OnOwnIndexMenu(UINT nID);
	afx_msg void OnMenu (UINT nID);
	afx_msg LRESULT OnMessageTitleButton(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_TRENDARBITRAGE_H_
