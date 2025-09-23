#ifndef _IO_VIEW_TREND_H_
#define _IO_VIEW_TREND_H_

#include "IoViewBase.h"
#include "IoViewChart.h"
#include "IoViewShare.h"
#include "MerchManager.h"
#include "Region.h"
#include "ChartRegion.h"
#include "LandMineStruct.h"
#include "DlgEconomicTip.h"
#include "PolygonButton.h"
#include <map>
#include <vector>
// �ƾ������ϲ�����ʱ�������ķ�����
#define CONTINUEATION_MINUTES 10
using std::map;
using std::vector;

class CMerch;
class CChartRegion;
class CIoViewTrend;
class CMPIChildFrame;
class CChartDrawer;

#ifndef _DLL_SAMPLE
#define _DLL_SAMPLE
#endif

/////////////////////////////////////////////////////////////////////////////
// CIoViewTrend
const int32 KTrendHeartBeatTimerId = 10007;

typedef struct _XAxisInfo
{
	CPoint pt;		// ʱ��̶ȵ���ʾλ��
	int hour;		// Сʱ
	int mintues;	// ����
	_XAxisInfo()
	{
		hour = mintues = -1;
	}
}T_XAxisInfo;

typedef struct _CircleInfo
{
	CRect rt;		 // Բ����ʾ����
	CString strTime; // Բ���Ӧx���ʱ��̶�
}T_CircleInfo;

enum E_TrendBtnType
{
	ETBT_Volumn = 0,		// �ɽ���		- �� ��
	ETBT_Amount,			// �ɽ���		- ָ
	ETBT_Index,				// ָ��			- ָ �� ��
	ETBT_VolRate,			// ����			- ��
	ETBT_WindWarningLine,   // �羯��		- ��
	ETBT_BuySellForce,		// ��������		- ��
	ETBT_CompetePrice,		// ����ͼ		- ��
	ETBT_DuoKong,			// ���ָ��		- ָ
	ETBT_RiseFallRate,		// �ǵ���		- ָ
	ETBT_PriceModel,		// �۸�ģ��		- ��

	ETBT_Count
};

struct T_TrendBtn
{
	int32			m_iID;				// ��ťID
	CPolygonButton  m_btnPolygon;		// ��ť
	E_TrendBtnType	m_eTrendBtnType;	// ��ť����
	E_ReportType	m_eMerchType;		// �ʺϵ���Ʒ����
	
	T_TrendBtn(E_TrendBtnType eBtnType, E_ReportType eMerchType, CWnd *pParent=NULL);
	T_TrendBtn();
	void SetParent(CWnd *pParent);
};

class CIoViewTrend : public CIoViewChart, public CChartRegionViewParam, public CRegionDrawNotify
{
// Construction
public:
	CIoViewTrend();
	virtual ~CIoViewTrend();
	
	DECLARE_DYNCREATE(CIoViewTrend)

/////////////////////////////////////////////////////
// from CControlBase
public:
	virtual	CString GetDefaultXML();	
	virtual void	DoFromXml();
	virtual void	SetChildFrameTitle();

	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);			// ֪ͨ��ͼ�ı��ע����Ʒ
	virtual bool32	OnVDataAddCompareMerch(IN CMerch *pMerch);		// ������ͼ���
	virtual void	OnVDataForceUpdate();															// ֪ͨ���ݸ���	
	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);	
	virtual void	OnVDataFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName );

	virtual void	OnVDataLandMineUpdate(IN CMerch* pMerch);	// ��Ϣ���׸���
	virtual void	OnVDataClientTradeTimeUpdate();
	// ������������������
	virtual void	OnVDataGeneralNormalUpdate(CMerch *pMerch);
	virtual void	OnVDataGeneralFinanaceUpdate(CMerch *pMerch);
	virtual void	OnVDataMerchTrendIndexUpdate(CMerch *pMerch);
	virtual void	OnVDataMerchAuctionUpdate(CMerch *pMerch);
	virtual void	OnVDataMerchMinuteBSUpdate(CMerch* pMerch);
	virtual BOOL IsKindOfIoViewTrend() const {return TRUE;}

	// from IoViewChart
public:
	virtual void	CalcLayoutRegions(bool bOnlyUpdateMainRegion = true);	
	virtual void	RequestViewData();
	
	virtual void	OnKeyHome();
	virtual void	OnKeyEnd();
	virtual void	OnKeyLeftAndCtrl(){};
	virtual void	OnKeyLeft();
	virtual void	OnKeyRightAndCtrl(){};
	virtual void	OnKeyRight();
	virtual void	OnKeyUp();
	virtual void	OnKeyDown();
	virtual void	OnKeySpace();

	virtual void	OnIoViewActive();
	virtual void    OnIoViewDeactive();
	virtual void	UpdateAxisSize(bool32 bPreDraw = true);

	// ��ȡ��ͼ�ı�������ť:
	virtual int32	GetSubRegionTitleButtonNums();
	virtual const T_FuctionButton*	GetSubRegionTitleButton(int32 iIndex);

	//
	virtual void	ClearLocalData(bool32 bClearAll = true);
	// �򵥵�����Ʒ
	virtual bool32				AddCmpMerch(CMerch *pMerchToAdd, bool32 bPrompt, bool32 bReqData);
	virtual void				RemoveCmpMerch(CMerch *pMerch);
	virtual void				RemoveAllCmpMerch();
	virtual void				OnShowDataTimeRangeChanged(T_MerchNodeUserData *pData);
	
	//
	virtual void    OnRectIntervalStatistics(int32 iNodeBegin, int32 iNodeEnd);
	
	virtual void	Draw();

	virtual	bool32	FromXmlInChild(TiXmlElement *pTiXmlElement);
	virtual CString	ToXmlInChild();
	virtual CString ToXmlEleInChild();

	virtual bool32	AddShowIndex(const CString &StrIndexName, bool32 bDelRegionAllIndex = false, bool32 bChangeStockByIndex = false, bool32 bShowUserRightDlg=false);
	// from CRegionViewParam

	virtual	void	GotoCompletePrice();
public:
	virtual void	OnRequestPreNodeData ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes, int32 id, int32 iNum, bool32 bSendRequest );
	virtual void	OnRequestNextNodeData ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes, int32 id, int32 iNum, bool32 bSendRequest );
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
	virtual IChartBrige GetRegionParentIoView();
	virtual int32 GetChartType(){return m_iChartType;}
	virtual CString OnTime2String ( CGmtTime& Time );
	virtual CString OnFloat2String ( float fValue, bool32 bZeroAsHLine = true, bool32 bNeedTerminate = false );

	virtual void	OnCalcXAxis ( CChartRegion* pRegion, CDC* pDC, CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide);
	virtual void	OnCalcYAxis ( CChartRegion* pRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide);

	virtual CString OnGetChartGuid ( CChartRegion* pRegion );
	virtual void	OnSliderId ( int32& id, int32 iJump );//û��

	virtual bool32  GetViewRegionRect(OUT LPRECT pRect);

	virtual bool32  OnGetCurveTitlePostfixString(CChartCurve *pCurve, CPoint ptClient, bool32 bHideCross, OUT CString &StrPostfix);

	virtual	void	OnChartDBClick(CPoint ptClick, int32 iNodePos);	// ˫���¼�
	virtual bool32	GetTips(IN CPoint pt, OUT CString& StrTips, OUT CString &StrTitle);

	//
	virtual bool32	GetAdvDlgPosition(IN int32 iWidth, IN int32 iHeight, OUT CRect& rtPos);
	virtual bool32  IsAlreadyToShowAdv();
//////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////// �����Ƿ���ʾ���ۺ͵�ǰ��
	virtual void   SetAvgPriceEvent();
// 
public:
	CGmtTime		GetRequestNodeTime(IN CMerch *pMerch);

	bool32			ServerTimeInOpenCloseTime ( CMerch* pMerch );
//	bool32			ServerTimeAfterCloseTime ( CMerch* pMerch );


	//
	void			DrawAlarmMerchLine();
	
	void			DrawRealtimePriceLine();
	void			DrawRealTimeSlider(CChartRegion* pRegion, CMemDCEx* pDC);

	void			GetDrawCrossInfoNoData(int32 &iOpenTime,int32 &iCloseTime);		// ������ʱ��,��X ������鴰��ʱ�õ�

	// ������һ�Ű�ť���
	void			DrawBottomBtns();
	void			InitMerchBtns();	// ��ʼ����ǰ��Ʒ�İ�ť
	void			InitNoShowBtns();	// ��ʼ������ʾ��ť
	void			RecalcBtnRect(bool32 bNestRegion = false);	// ���㰴ť������
	void			OnTrendBtnSelChanged(bool32 bReqData = true);	// ��ǰ��ť���ͱ����
	E_TrendBtnType	GetCurTrendBtnType();	// ��ǰ�İ�ť����
	bool32			InitRedGreenCurve();	// ��ʼ��������״��
	void			CalcRedGreenValue();	// ���ݵ�ǰ��Ʒ���������״��
	bool32			InitBtnIndex();			// ��ʼ����ť ָ�� ��ָ��
	bool32			IsRedGreenMerch(CMerch *pMerch);		// ָ����Ʒ�Ƿ��Ǻ�����Ҫ�����Ʒ

	void			DrawCompetePriceChart(CMemDCEx *pDC, CChartRegion &ChartRegion);	// �����ڵ�region���ƾ���ͼ����ͼ&�ɽ�����ͼ����, ����ͼ�β���
	void			CalcCompetePriceValue();	// ���㾺������
	bool32			InitCompetePriceNodes();	// ��ʼ������ͼ
	bool32			CalcCompetePriceMinMax(CChartCurve *pDepCurve, OUT float &fMin, OUT float &fMax);	// ���㾺�����ݵ������Сֵ, ��Ϊ��ͼ����ɽ�����ͼ�ߵļ���
	bool32			CalcCompetePriceMinMaxVol(bool32 bVol, OUT float &fMin, OUT float &fMax);	// ���㾺��ͼ�������С�ɽ���ֵ
	bool32			GetCompetePriceMinMax(bool32 bMainRegion, OUT CPriceToAxisYObject &AxisObj);	// ��ȡ����layout�󾺼�ͼ�����������(!!�������layout����)��true��ͼ, false�ɽ���ͼ

	bool32			RequestTrendPlusData();	// �����ʱ�Ķ�������
	bool32			RequestTrendPlusPushData();	// �����ʱ�Ķ������������, �����Ƿ�����������������

	void			CalcTrendPlusSubRegionData();		// ����һ�·�ʱ��ͼ���������
	void			CalcDuoKongData();			// ���
	void			CalcExpRiseFallRateData();	// �ǵ�����
	void			CalcVolRateData();			// ����
	void			CalcBuySellForceData();		// ��������
	void			CalcNoWeightExpLine();		// ����Ȩָ������
	void			ShowProperAvgLine();		// ���ߵ�ǰ��Ʒ����ʾ���ʵľ���

	void			ShowProperTrendPlusCurve();	// ��ʾ�������ط�ʱ�Ķ������
	bool32			IsShowTrendFirstOpenPrice();	// �Ƿ���ʾǰ��ĵ�һ����������
	void			DrawTrendFirstOpenPrice(CMemDCEx *pDC);	// ���Ƶ�һ��������

	bool32			GetCallAuctionTime(OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd);	// ��ȡ��ǰ�ľ���ʱ���
	bool32			IsTimeInCallAuction(const CGmtTime &TimeNow);	// ָ��ʱ���Ƿ��ھ���ʱ�����

	void			TrimPlusShowNodeData(INOUT CNodeSequence *pNodes);	// �ض���ʾ�ߣ����ʺϵ�ǰ����ʾ����

	void			DrawLandMines(CMemDCEx *pDC);	// ���Ƶ���

	bool            IsCompetePriceShow();           // ����ͼ�Ƿ���ʾ

private:
	void			MenuAddRegion();
	void			MenuDelRegion();
	void			MenuDelCurve();
	void			MenuAddIndex ( int32 id);

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
	
	// ��ʷ��ʱ
public:
	void			SetHistoryTime(CGmtTime TimeHistory);
	void			SetHistoryFlag(bool32 bHistroy)	
	{
		if (bHistroy)
		{
			m_bShowTopToolBar = FALSE;
			m_bShowIndexToolBar = FALSE;
		}
		m_bHistoryTrend = bHistroy; 
	}
	void			OnCtrlLeftRight(bool32 bLeft) ;
	CString			GetHistroyTitle(CGmtTime Time);


public:
	bool32			UpdateMainMerchKLine(T_MerchNodeUserData &MerchNodeUserData);
	bool32			UpdateSubMerchKLine(T_MerchNodeUserData &MerchNodeUserData);
	bool32			FillTrendNodes(T_MerchNodeUserData &MerchNodeUserData, const CGmtTime &TimeNow, const CKLine *pKLines, int32 iCountKLine);			

	bool32			CalcMainRegionXAxis(CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aAxisDivide);
	void			CalcMainRegionYAxis(CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);

	void			CalcSubRegionYAxis(CChartRegion* pChartRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide);
	
	void			ValueYAsNodata(float fYMin, float fYMax, const CRect& Rect,int32 iSkip, float fy,int32& iValue);

	// �������ֲ���
	void			DrawHoldLine(IN CMemDCEx* pDC, IN CMemDCEx* pPickDC, IN CChartRegion* pRegion);

	void			RequestMinuteKLineData(T_MerchNodeUserData &MerchData);		// �����յķ���k������
	void			TryReqMoreMultiDayMinuteKLineData();			// �����������ǰ���͵ĸ����Ҫ��Ķ���k������
	void			SetTrendMultiDay(int iDay, bool32 bClearData, bool32 bReqData);		// ���ö��շ�ʱ
	void			UpdateTrendMultiDayChange();	// �����
	int32			GetTrendMultiDay(){return m_iTrendDayCount;}


	bool32			IsEnableSingleSection();			// �Ƿ�����볡����
	void			SetSingleSection(bool32 bSingle);	// ���ð볡����

	bool32			GetTrendStartEndTime(OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd);	// ��ȡ��ʱͼ�Ŀ�ʼ&����ʱ��
	bool32			GetTrendSingleSectionOCTime(OUT CGmtTime &TimeOpen, OUT CGmtTime &TimeClose); // ��ȡ�볡������ʱ��
	bool32			GetTrendFirstDayOpenCloseTime(OUT CGmtTime &TimeOpen, OUT CGmtTime &TimeClose);	// ��ȡ��ʱͼ��һ��ĵ�һ�����̣����һ������ʱ��

	// ��Ʒ����
	void				RequestSingleCmpMerchViewData(CMerch *pMerch);	// ���󵥸���Ʒ������
	bool32				CalcKLinePriceBaseValue(CChartCurve *pCurve, OUT float &fPriceBase);

	bool32				CalcRiseFallMaxPrice(OUT float& fRiseMax, OUT float& fFallMax);	// �ǵ�ͣ

	bool32				IsShowRiseFallMax() const { return m_bShowRiseFallMax; }
	bool32				IsShowAvgPrice() const { return m_bShowAvgPrice; }

	float				GetTrendPrevClose();		// �������ռ�ȡ��

	void				AddSpecialCurveToSubRegion();	// ���ݵ�ǰѡ���ڸ�ͼ�������
	CChartCurve			*GetSpecialCurve(int32 iSpecailId);	// ��ȡ���������ߣ������������������
	void				SetSpecialCurvePickColor(int32 iSpecialId, uint32 uPickColor);		// ���������ߵ�ƥ����ɫ
	bool32				GetSpecialCurvePickColor(int32 iSpecialId, OUT uint32 &uPickColor);	// ��ȡ�����ߵ�ƥ����ɫ

	bool32				AddIndexToSubRegion(const CString &StrIndexName, bool32 bShowRightDlg=false);	// ���ָ�굽��ͼ��������ӵ��ǳɽ�����ͼ
	void				RemoveSubRegion();		// ɾ���ǳɽ�����ͼ
	CString				GetSubRegionTitle();	// ��ȡ�ǳɽ�����ͼ������
	bool32				LoadAllIndex(CNewMenu *pMenu);		// �������в˵���
	int32				LoadOftenIndex(CNewMenu *pMenu);	// ���س��ò˵���
	void				ShowIndexMenu();					//��ʾָ��˵�
	void				RequestVolBuySellData();		// �����ʱ��ÿ������������Ϣ

	bool32				IsSuspendedMerch();				// �Ƿ�Ϊͣ����Ʒ��ͣ����Ʒ����������
	void				CreateSuspendedVirtualNoOpenKlineData();		// ����ʵʱ�������ݣ����������޿��̣���ʵ�ʷ���k�´�����ٵķ���k������
	void				CreateSuspendedVirtualNoOpenCmpKlineData();		// ����ͣ����Ʒ������cmpk��
	void				TrimSuspendedIndexData();		// �ض�ͣ�����ݵ�ָ������

	bool32				GetCurrentIOCTime(OUT CMarketIOCTimeInfo &IOCTime);	// ��ȡ�����ʱͼ�ĵ���������Ϣ

private:
	
	//					������Ʒ�����ж��Ƿ���Ҫ�ɽ�����ͼ
	void				AddOrDelVolumeRegion(E_ReportType eBefore, E_ReportType eNow);

	//
	void				SetIndexToRegion(const CString& StrIndexName, CChartRegion* pRegion, bool32 bShowRightDlg=false);

	//
	int					TBottomBtnHitTest(CPoint point);
	int32				FindIndexByBtnID(int32 iID);		// ���ݰ�ťId�Ҷ�Ӧ��arrary�е�����
	
public:
	friend	class CIoViewDuoGuTongLie;
	CChartDrawer*		m_pTrendDrawer;
	CArray<CGmtTime,CGmtTime>	m_TimesCombin;
	CArray<int32,int32>	m_PosCombin;
	
	CGmtTime			m_CornerTime;

	CNodeSequence		*m_pHoldNodes;		// �ֲ�������

	// ����x���ʱ��̶���Ϣ
	std::vector<T_XAxisInfo> m_vecXAxisInfo;

	std::string m_strEconoData;

	// СԲ������ԣ������뾶����ʾ���򡢵�����ʾ��
	int m_radius;
	std::vector<T_CircleInfo> m_CircleInfo;
	CDlgEconomicTip m_dlgEconomicTip;

	// 
	BOOL m_bTracking;
	BOOL m_bOver;

public:
	std::vector<int> m_vecTimePos;

	virtual void	OnVDataEconoData(const char *pszRecvData);

	void StartReqEconoData();

	/*
	 * ParseEonomicData
	 * �����ƾ���������
	 * 
	 */
	void ParseEonomicData(const char *pszData);

	void ParseLDorLK(IN std::string &str, OUT std::string &strLK, OUT std::string &strLD, OUT std::string &strPlaceholder, OUT bool &bBoth);

	/*
	 * SaveTimeDegreeScale
	 * �����ʱͼx���ʱ��̶�
	 * @param xPos ʱ��strTime��x����
	 * @param strTime ʱ��̶�
	 */
	void SaveTimeDegreeScale(int xPos, CString strTime);

	/*
	 * ComputeCircleInfo
	 * ����ƾ����������ڷ�ʱͼ��СԲ���λ��
	 */
	void ComputeCircleInfo();// ����СԲ������

	/*
	 * DrawCircleDot
	 * ���Ʋƾ�������СԲ��
	 */
	void DrawCircleDot(CMemDCEx * pDC);	// ���Ʋƾ�������СԲ��
	double myFabs(double dbNum);
	

	std::map<int, CString> m_mapIndexName;  //ָ��˵����ӦID
private:
	bool32				m_bHistoryTrend;
	CGmtTime			m_TimeHistory;

private:
	int32				m_iOpenTimeForNoData;
	int32				m_iCloseTimeForNoData;

	bool m_bSizeChange;

	// ������1��ʱ����Ҫ�ȵ���k�߷��ز���ȷ��Ҫ���������[ʱ���]�ķ���k������
	int32				m_iTrendDayCount;		// Ҫ�鿴����ķ�ʱ, Ĭ��1
	// ��ÿ����������ʱ�����¼��������
	CArray<CMarketIOCTimeInfo, const CMarketIOCTimeInfo &>	m_aTrendMultiDayIOCTimes; // ÿ��Ŀ����̣���һ��Ϊ���죬����Ϊǰn��

	// ��ǰʱ�����ڵĿ�����ʱ��, init-��һ���������ڵ�һ������
	//      �м������ǿ���ʱ��������һ������
	//		���һ���ճ���Endʱ���������һ��������
	// �������ڹ�Ʊ&ָ���ṩ
	bool32				m_bSingleSectionTrend;		// �볡����

	float				m_fMinHold;
	float				m_fMaxHold;
	
	
	// t..fangz1010
	CArray<CAxisDivide, CAxisDivide&>	m_aYAxisDivideBkMain;	// ��ͼ y��ָ��߱���,�����л�ʱ��������Ʒ����ʱ��ͼ
	CArray<CAxisDivide, CAxisDivide&>	m_aYAxisDivideBkSub;	// ��ͼ y��ָ��߱���,�����л�ʱ��������Ʒ����ʱ��ͼ

	//
	CArray<CKLine, CKLine>							m_aKLineInterval;					// ����ͳ�Ƶ�K��
	float m_fPriceIntervalPreClose;
	// ���·���ť���
	typedef CArray<T_TrendBtn, const T_TrendBtn &>	TrendBtnArray;
	static	TrendBtnArray	s_aInitTrendBtns;	// ��ʼ����ť
	TrendBtnArray	m_aTrendBtnCur;	// ��ǰ��ť
	int32	m_iBtnCur;		// ��ǰ��ť
	typedef CMap<int32, int32, CString , CString> TrendMerchTypeIndexNameMap;
	typedef CMap<int32, int32, int32, int32>	TrendMerchTypeBtnIndexMap;
	TrendMerchTypeIndexNameMap	m_mapMerchTypeIndexName;	// ��Ʒ������ָ����ѡ���ָ������
	TrendMerchTypeBtnIndexMap	m_mapMerchTypeBtnIndex;		// ÿ����Ʒ���Ͷ�Ӧѡ��İ�ť����
	CString			m_StrBtnIndexName;		// �л���ָ�갴ťʱ����ǰ��ָ������

	CStringArray	m_aAllFormulaNames;		// ����ָ����������

	CChartCurve		*m_pRedGreenCurve;	// ������
	CNodeSequence	*m_pCompetePriceNodes;	// ��������, �����ݲ���Ӧ�κ��ߣ��Լ�����

	CChartCurve		*m_pNoWeightExpCurve;	// ����Ȩָ����

	typedef map<int32, uint32>	SpecialCurvePickColorMap;
	SpecialCurvePickColorMap	m_mapSpecialCurves;		// �÷�ʱͼ����������������Ӧ��Ψһ����ֵ, �����߿��ܻᱻɾ�������Ա�������ֵ����

	mapLandMine		m_mapLandMine;						// ��Ϣ��������
	typedef multimap<CString, CRect>	LandMineRectMap;
	LandMineRectMap	m_mapLandMinesRect;					// ��Ϣ��������

	CArray<double, double>	m_aSubRegionHeightRatios;	// ��ͼ�ĸ߶ȱ���

	bool32			m_bDrawYRigthPrice;					// �Ƿ����ұ߻����¼۵�

	int32			m_iBtnHovering;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewTrend)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewTrend)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMenu ( UINT nID );
	afx_msg void OnMenuIntervalStatistic();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMySettingChange();
	afx_msg void OnTrendIndexMenu(UINT nId);
	afx_msg void OnUpdateTrendIndexMenu(CCmdUI *pCmdUI);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_TREND_H_
