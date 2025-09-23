#ifndef _IO_VIEW_KLINEARBITRAGE_H_
#define _IO_VIEW_KLINEARBITRAGE_H_

#include "IoViewBase.h"
#include "IoViewChart.h"
#include "MerchManager.h"
#include "Region.h"
#include "ChartRegion.h"
#include "AxisYObject.h"
#include "IoViewKLine.h"
#include "ArbitrageManage.h"

class CMPIChildFrame;

enum E_ArbitrageChartShowType
{
	EACST_CloseDiff = 0,	// ���̼ۼ۲�ͼ�������̼ۼ۲�������ͼ
	EACST_AvgDiff,			// ���ۼ۲�: ���ۼ۲�������ͼ
	EACST_ArbKLine,			// ����K��: �۲���գ�����ͼ ��>��=�� ��Ϊ�м�Բ��
	EACST_TowLegCmp,		// ���ȵ���ͼ: ��-����ͬʱ��ʾ��ͼ

	EACST_Count
};

/////////////////////////////////////////////////////////////////////////////
// CIoViewKLineArbitrage

// ʵ�������ӿ���~~
class CIoViewKLineArbitrage : public CIoViewChart, public CChartRegionViewParam, public CRegionDrawNotify, public CArbitrageNotify
{
	// Construction
public:
	CIoViewKLineArbitrage();
	virtual ~CIoViewKLineArbitrage();

	DECLARE_DYNCREATE(CIoViewKLineArbitrage)

	// from CControlBase
public:
	virtual bool32	FromXml(TiXmlElement * pTiXmlElement);
	virtual void	DoFromXml();
	virtual void	InitialShowNodeNums();

	virtual bool32  FromXmlInChild(TiXmlElement *pTiXmlElement);
	virtual CString ToXmlInChild();

	virtual CString	ToXmlEleInChild();		// ���ౣ��xmlԪ��

	virtual	CMerch *GetMerchXml();
	virtual	E_IoViewType	GetIoViewType() { return  EIVT_MultiMerch; }

	virtual bool32	OnSpecialEsc();
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
	virtual void	OnRectZoomOut(int32 iNodeBegin, int32 iNodeEnd);

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
	void	RequestViewData(bool32 bForceReq=false);
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
	virtual	void	KLineCycleChange(UINT nID);

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

	void		SetArbShowType(E_ArbitrageChartShowType eType);	// �����ʾ����
	void		AdjustArbCurveByShowType();						// �����ߵ���ʾ����

	void		TestSetArb(bool32 bAsk=false);

	void		OnF5();	// �л�

	int32		GetSubMerchNeedDataCount();			// ��Ʒ�����K������
	void		ResetNodeScreenCount();				// ������Ļ����
public:

	// ��ʷ��ʱ���
	E_NodeTimeInterval GetTimeInterval();
	void			GetCrossKLine(OUT CKLine & KLineIn);

	//
	bool32			IsShowNewestKLine(OUT CGmtTime& TimeStartInFullList, OUT CGmtTime &TimeEndInFullList);	// �жϵ�ǰ�Ƿ���ʾ����һ��K��, ������ǣ� �򴫳���ǰ��ʾ��ʱ�䷶Χ
	bool32			UpdateMainMerchKLine(T_MerchNodeUserData &MerchNodeUserData, bool32 bForceUpdate = false);
	bool32			UpdateSubMerchKLine(T_MerchNodeUserData &MerchNodeUserData, bool32 bForceUpdate=true);

	bool32			CalcMainRegionXAxis(CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aAxisDivide);
	void			CalcMainRegionYAxis(CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);
	void			CalcSubRegionYAxis(CChartRegion* pChartRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);

	// ...fangz0729 ʮ�ֹ���״̬��λ��Ӧ��K �߱���.��Ҫ�ŵ�ChartRegion ��.
	void			SetCrossKLineIndex(int32 iPos)	{ m_iCurCrossKLineIndex = iPos; }
	int32			GetCrossKLineIndex()			{ return m_iCurCrossKLineIndex; }

	void			SetRequestNodeFlag(bool32 bTrue){ m_bRequestNodesByRectView = bTrue;}

	void			OnUserCircleChanged(E_NodeTimeInterval eNodeInterval, int32 iValue);

	// ��Ȩ��Ȩ���
	void			RequestWeightData();

	void			OnKeyF8();

	void            UpdateKelineArbitrage(E_NodeTimeInterval NodeInterval);

	// from CView
protected:
	virtual void	OnRegionDrawNotify(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC,CRegion* pRegion,E_RegionDrawNotifyType eType);
	virtual void	CreateRegion();
	virtual void	SplitRegion();
	virtual void	SetXmlSource ();
	virtual void	SetCurveTitle ( T_MerchNodeUserData* pData );
	virtual void	InitCtrlFloat ( CGridCtrl* pGridCtrl );
	virtual void	SetFloatData ( CGridCtrl* pGridCtrl,CKLine& kLine,float fPricePrevClose,float fCursorValue,CString StrTimeLine1,CString StrTimeLine2,int32 iSaveDec);
	virtual void	SetFloatData ( CGridCtrl* pGridCtrl,float fCursorValue,int32 iSaveDec);
	virtual void	ClipGridCtrlFloat (CRect& rect);

	void			AdjustCtrlFloatContent();	// �������鴰������

private:

	bool32			LoadAllIndex(CNewMenu* pMenu);
	int32			LoadOftenIndex(CNewMenu* pMenu);
	bool32			LoadOwnIndex(CNewMenu* pMenu);
	void			MenuAddRegion();
	void			MenuDelRegion();
	void			MenuDelCurve();
	void			MenuAddIndex ( int32 id);

	void			UpdateKLineStyle();

	bool32			GetTimeIntervalInfo(IN E_NodeTimeInterval eNodeTimeInterval, OUT E_NodeTimeInterval &eNodeTimeIntervalCompare, OUT E_KLineTypeBase &eKLineTypeCompare, OUT int32 &iScale);
	void			SetTimeInterval(T_MerchNodeUserData &MerchNodeUserData, E_NodeTimeInterval eNodeTimeInterval);
	void			ResetTimeInterval( T_MerchNodeUserData* pData,IN E_NodeTimeInterval eNodeTimeInterval,IN E_NodeTimeInterval eNodeTimeIntervalCompare);

	bool32			OnZoomOut();
	bool32			OnZoomIn();

	int32			GetSpaceRightCount();		// �����Ƿ�����ʾ���·���4����0

private:	

	bool32			m_bRequestNodesByRectView;	// �½�һ��K �ߵ�ʱ��,���ݵ�ǰ��ʾ����Ĵ�С����K ��.������ʾ�Ĳ�Э��(��������)
	CKLine			m_KLineCrossNow;
	CStringArray	m_aAllFormulaNames;			// ����ָ��: ÿ��ID ��Ӧ������һ������
	int32			m_iCurCrossKLineIndex;		// ��ǰ��Ļ��ʮ�ֹ����ʾλ��
	int32			m_iNodeCountPerScreen;		// ������Ļ��һ����ʾK������(������������, ����ʵ�ʸ���. ��������С��ʱ��, û����ô���������ֵ���ʵ����ʾ�Ĳ���.)
	bool32			m_bForceUpdate;				// �Ƿ�ǿ�Ƹ���
	//////////////////////////////////////////////////////////////////////////


	// ���� ���������ݸ��£�������ʽ���������ɾ��(!)����ǰ����ע����������ȵȽӿ�
	CArbitrage		m_Arbitrage;		// ��ǰ����ע������ģ�ͣ�ֻ�ܹ�עһ��~~
	// ��������������cmp���cmp��Ʒ��λ�ô�� T_MerchUserDataֻ����������ݣ�MainUserData�������ʵ�ʵĺϳ����ݣ�����Ʒ�޹�

	E_ArbitrageChartShowType		m_eArbitrageShow;	// ����������ʾ

	CChartCurve		*m_pCuveOtherArbLeg;	// ����һ���ȵ���

private:
	int32				m_iLastPickSubRegionIndex;	// ���һ��ѡ���ĸ�ͼ

public:
	void				InitialImageResource();

	int32				CompareKLinesChange(const CArray<CKLine,CKLine>& KLineBef, const CArray<CKLine,CKLine>& KLineAft);		// �ж�����K �ߵ���ͬ. 0:��ͬ 1:����һ���仯 2:������һ�� 3:�����������,�д�ı仯

	//
	void				SetUpdateTime();												// ���ø���ʱ���	
	void				OnTimerUpdateLatestValuse(UINT uID);							// ����ʵʱָ��ֵ

	// �õ�ĳʱ����ӦK �ߵĸ���ʱ��
	static bool32		GetKLineUpdateTime(IN CMerch* pMerch, IN CAbsCenterManager* pAbsCenterManager, IN const CGmtTime& TimeNow, IN E_NodeTimeInterval eTimeInterval, IN int32 iMiniuteUser, OUT bool32& bNeedTimer, OUT CGmtTime& TimeToUpdate, OUT bool32& bPassedUpdateTime, IN bool32 bGetKLineFinishTime = false);
	static CString		GetTimeString(CGmtTime Time, E_NodeTimeInterval eTimeInterval, bool32 bRecorEndTime = false);

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
	virtual bool32		AddCmpMerch(CMerch *pMerchToAdd, bool32 bPrompt, bool32 bReqData){ return false; };
	virtual void		RemoveCmpMerch(CMerch *pMerch){};
	virtual void		RemoveAllCmpMerch(){};
	virtual void		OnShowDataTimeRangeChanged(T_MerchNodeUserData *pData);	// ��ʾ������
	// ��ͼ��Ʒ����ʾ�����������е��˺���(UpdateMainMerchʱ�������)
	bool32				ChangeMainDataShowData(T_MerchNodeUserData &MainMerchNode, int32 iShowPosInFullList, int32 iShowCountInFullList, bool32 bPreShowNewest, const CGmtTime &TimePreStart, const CGmtTime &TimePreEnd);
	bool32				CalcKLinePriceBaseValue(CChartCurve *pCurve, OUT float &fPriceBase);

	CChartCurve			*GetCurIndexCurve(CChartRegion *pRegionParent);	// ��ȡָ��region�µ�ָ����

public:

	/////////////////////////////////////////////////////
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewKLineArbitrage)
	virtual BOOL TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewKLineArbitrage)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnAllIndexMenu(UINT nID);
	afx_msg void OnOftenIndexMenu(UINT nID);
	afx_msg void OnOwnIndexMenu(UINT nID);
	afx_msg void OnTest();
	afx_msg void OnMenu (UINT nID);
	afx_msg LRESULT OnMessageTitleButton(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	//afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_KLINEARBITRAGE_H_
