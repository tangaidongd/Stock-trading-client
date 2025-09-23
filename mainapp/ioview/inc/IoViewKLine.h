#ifndef _IO_VIEW_KLINE_H_
#define _IO_VIEW_KLINE_H_

#include "IoViewBase.h"
#include "IoViewChart.h"
#include "MerchManager.h"
#include "Region.h"
#include "ChartRegion.h"
#include "TrendIndex.h"
#include "SaneIndex.h"
#include "ExpertTrade.h"
#include "AxisYObject.h"
#include "LandMineStruct.h"
#include "DlgStrategyInfomation.h"

#include <map>
using std::multimap;

class CMPIChildFrame;
class CIoViewKLine;
class CDlgTrendIndexstatistics;
class CDlgUserCycleSet;

/////////////////////////////////////////////////////////////////////////////
// CIoViewKLine

extern const int32 KTimerIdUpdateSaneValues;

extern void TestKLine(CArray<CKLine,CKLine>& aKLines,const char * pFile,CString StrMerchCode, int32 iMarketId);

// �˴���enum������CIndexDrawer�е�k�߻��Ʒ���һ��
enum E_MainKlineDrawStyle
{
	EMKDSNormal = 0,	// �����߻���
	EMKDSFill,			// ʵ��
	EMKDSAmerica,		// ������
	EMKDSClose,			// ������
	EMKDSTower,			// ������
	EMKDSCount,			// ��Ч
};

int32	MainKlineDrawStyleToChartDrawType(E_MainKlineDrawStyle eStyle); // ��ͼK�������л���ʵ�ʻ�������

class CIoViewKLine : public CIoViewChart, public CChartRegionViewParam, public CRegionDrawNotify
{
// Construction
public:
	CIoViewKLine();
	virtual ~CIoViewKLine();
	
	DECLARE_DYNCREATE(CIoViewKLine)

	// from CControlBase
public:
	virtual void	DoFromXml();
	virtual void	InitialShowNodeNums();
	virtual BOOL	IsKindOfIoViewKLine(){return TRUE;}
	virtual bool32  FromXmlInChild(TiXmlElement *pTiXmlElement);
	virtual CString ToXmlInChild();
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
	virtual void    OnRectIntervalStatistics(int32 iNodeBegin, int32 iNodeEnd);
	
	virtual bool32	GetAdvDlgPosition(IN int32 iWidth, IN int32 iHeight, OUT CRect& rtPos);
	virtual bool32  IsAlreadyToShowAdv();
	//

	virtual void	DragMoveKLine(int32 icnt);	// �϶�K��ƽ��

	void			OnDelAllPrompt();

	//
	void			OnAddClrKLine();
	void			OnDelClrKLine();
	
	void			DoAddClrKLine(CFormularContent *pFormularContent);
	void			AddClrKLine(const CString &StrKLineName);

	//
	void			OnAddTrade();
	void			OnDelTrade();
	void			DoAddTrade(CFormularContent *pFormulaTrade);

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
	virtual void	OnVDataLandMineUpdate(IN CMerch* pMerch);	// ��Ϣ���׸���
	virtual void	OnVDataStrategyData(const char *pszRecvData);	// ����������Ϣ
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
	virtual bool32	GetViewRegionRect(OUT LPRECT pRect);
public:
	// �ɱ��ѻ�
	void			DrawNcp(IN CMemDCEx* pDC);
	 
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

	// ��Ȼ���Ը��Ǹ���CIoViewChart��LoadAllIndex�����������Ժ���ң����¶��庯��LoadAllIndex����
	bool32			KlineLoadAllIndex(CNewMenu* pMenu);
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
	void			OnMenuCnp();
	int32			LoadOftenIndex(CNewMenu* pMenu);
	int32			LoadJinDunSpecialIndex(CNewMenu *pMenu);
	bool32			LoadOwnIndex(CNewMenu* pMenu);
	void			MenuAddRegion();
	void			MenuDelRegion();
	void			MenuDelCurve();
	void			MenuAddIndex ( int32 id);
	

	void			UpdateKLineStyle();
	
	bool32			GetTimeIntervalInfo(IN E_NodeTimeInterval eNodeTimeInterval, OUT E_NodeTimeInterval &eNodeTimeIntervalCompare, OUT E_KLineTypeBase &eKLineTypeCompare, OUT int32 &iScale);
	void			SetTimeInterval(T_MerchNodeUserData &MerchNodeUserData, E_NodeTimeInterval eNodeTimeInterval);
	void			ResetTimeInterval( T_MerchNodeUserData* pData,IN E_NodeTimeInterval eNodeTimeInterval,IN E_NodeTimeInterval eNodeTimeIntervalCompare);
	
	// ��ͼ�Ż� ��ȡSaneIndex Tip
	bool32			GetSaneNodeTip(OUT CString &StrTip, IN int32 iSaneIndex);

	bool32			OnZoomOut();
	bool32			OnZoomIn();

	int32			GetSpaceRightCount();		// �����Ƿ�����ʾ���·���4����0

	void			OnAQExtDoQueueItem();	//�Խ��������Ե�gatewayЭ��
	void			PackJson(string &strTransDataUtf8);//�Խ��������Ե�json
	void			UnPackJson(string strMerchData);//�Խ��������Ե�json

public:
	bool32			SpecialForZLMM(T_MerchNodeUserData* pData);
	bool32			BeNeedZLMM(T_MerchNodeUserData* pData);

public:
	bool32			GetShowClrKLineFlag();
	bool32			BeSysClrKLine();
	CString			GetClrKLineName();

    void            UpdateKeline(E_NodeTimeInterval NodeInterval);
	
		// ָ���Ƿ����
	bool32          CheckIndexExist(const CString &StrIndexName);
	
private:
	void			UpdateShowClrKLine(T_MerchNodeUserData* pData, int32 iShowPosInFullList, int32 iShowCountInFullList);

private:	
	bool32			m_bNeedCnp;
	bool32			m_bRequestNodesByRectView;	// �½�һ��K �ߵ�ʱ��,���ݵ�ǰ��ʾ����Ĵ�С����K ��.������ʾ�Ĳ�Э��(��������)
	CKLine			m_KLineCrossNow;
	
	int32			m_iCurCrossKLineIndex;		// ��ǰ��Ļ��ʮ�ֹ����ʾλ��
	int32			m_iNodeCountPerScreen;		// ������Ļ��һ����ʾK������(������������, ����ʵ�ʸ���. ��������С��ʱ��, û����ô���������ֵ���ʵ����ʾ�Ĳ���.)
	bool32			m_bForceUpdate;				// �Ƿ�ǿ�Ƹ���
	//////////////////////////////////////////////////////////////////////////
	// TrendIndex
private:
	bool32				m_bShowTrendIndexNow;	// ��ǰ�ǲ�������ָ��
	bool32				m_bShowSaneIndexNow;	// ��ǰ�ǲ����Ƚ���ָ��
	bool32				m_bForceCalcSaneIndex;  // ��UpdateMainKLine���Ƿ�ǿ������Sane

private:
	CFormularContent*	m_pFormularClrKLine;	// ��� K ��ָ�깫ʽ
	T_IndexOutArray*	m_pOutArrayClrKLine;	// ��� K �߼�����

	CFormularContent*	m_pFormularTrade;		// ����ָ�깫ʽ
	T_IndexOutArray*	m_pOutArrayTrade;		// ����ָ�������

	int32				m_iLastPickSubRegionIndex;	// ���һ��ѡ���ĸ�ͼ

	CExpertTrade*		m_pExpertTrade;			// ר�ҽ�����ʾ
	// ����ֲ��й�
	CRect				m_RectMainCurve;		// ��ͼcurve�ľ��ο򣬷����仯ʱ��Ҫ֪ͨ����ֲ�
	CPriceToAxisYObject m_AxisYCalcObj;

	E_MainKlineDrawStyle m_eKlineDrawStyle;		// ��ͼK�߻��Ʒ���
	//���dlg ����
	//bool32              m_bMatchMerCode;        
	//bool32              m_bMatchTimer; 
//	bool32              m_bOwnAdvertise;          //ӵ�й��dlg��view,��changesizeʱҪ�ػ����dlg
//	bool32              m_bViewIsvisible;

public:
	void				InitialImageResource();

	bool32				GetShowSaneIndexFlag()		{ return m_bShowSaneIndexNow;  }
	int32				CompareKLinesChange(const CArray<CKLine,CKLine>& KLineBef, const CArray<CKLine,CKLine>& KLineAft);		// �ж�����K �ߵ���ͬ. 0:��ͬ 1:����һ���仯 2:������һ�� 3:�����������,�д�ı仯
	
	//
	void				SetUpdateTime();												// ���ø���ʱ���	
	void				OnTimerUpdateLatestValuse(UINT uID);							// ����ʵʱָ��ֵ

	// �õ�ĳʱ����ӦK �ߵĸ���ʱ��
	static bool32		GetKLineUpdateTime(IN CMerch* pMerch, IN CAbsCenterManager* pAbsCenterManager, IN const CGmtTime& TimeNow, IN E_NodeTimeInterval eTimeInterval, IN int32 iMiniuteUser, OUT bool32& bNeedTimer, OUT CGmtTime& TimeToUpdate, OUT bool32& bPassedUpdateTime, IN bool32 bGetKLineFinishTime = false);
	static CString		GetTimeString(CGmtTime Time, E_NodeTimeInterval eTimeInterval, bool32 bRecorEndTime = false);

	//
	bool32				GetSaneIndexNode(IN int32 iPos,OUT T_SaneIndexNode& Node);		// ȡָ��ڵ�
	void				DrawSaneIndex(CMemDCEx* pDC);									// ���Ƚ���ָ����	
	void				ReSetSaneIndex(bool32 bMerchChange);							// ��ָ����ص���������	
	CString				GetSaneIndexShowName();											// ��ȡ�Ƹ����ָ�������
	void                ShowJinDunIndex();

	// ��Ȩ����
	void				DrawWeightInfo(CMemDCEx *pDC);
	bool32				IsWeightDataCanShow(const CWeightData &wData) const;

	// ����ϵͳ����
	void				DrawTrade(CMemDCEx* pDC);

	// ר����ʾ����
	void				DrawExpertTradePrompt(CMemDCEx* pDC);

	// ���Ʋ����ź�
	void				DrawStrategySignal(CMemDCEx* pDC);
	bool				CompareTime(long long Time1, long long Time2);

	//
	bool32				AddShowIndex(const CString &StrIndexName, bool32 bDelRegionAllIndex = false, bool32 bChangeStockByIndex = false, bool32 bShowUserRightDlg=false);		// ���Ӹ�ָ�꣬������ͼ����ͼ����ʲ�ȡ��ͬ���ж�
	// �û�Ͷ���ռǵ�Ļ���
	void				DrawUserNotes(CMemDCEx *pDC);
	void				DrawLandMines(CMemDCEx *pDC);	// ���Ƶ���

	//void				AddShowIndex(const CString &StrIndexName);		// ���Ӹ�ָ�꣬������ͼ����ͼ����ʲ�ȡ��ͬ���ж�
	void				RemoveShowIndex(const CString &StrIndexName);	// ɾ��ָ�꣬������ͼ �� ��� ��ȡ��ͬ
	void				DelCurrentIndex();
	void				AddCmpIndex();
	void				ChangeIndexToMainRegion(const CString &StrIndexName);				// �����ͼָ������
	void				ReplaceIndex(const CString &StrOldIndex, const CString &StrNewIndex, bool32 bDelRegionAllIndex = false, bool32 bChangeStock = false, bool32 bDonotAddIfNotSameType=true);	// ʹ����index�滻ָ��index
	// �ı�ָ��λ�õĸ�ͼָ��, �����λ�ò����ڣ������һ���µĸ�ͼ��������ָ�����,
	//  bAddSameIfExist ָ�����и�ͼ�м�ʹ���ڸ�ָ�꣬�԰�����������ӣ�����ȡ���˴����
	//  iSubRegionIndex -1 ����µĸ�ͼ���κ���Ч��ͼindexΪָ��λ��
	void				AddIndexToSubRegion(const CString &StrIndexName, bool32 bAddSameIfExist = true, int32 iSubRegionIndex = -1); 
	void				AddIndexToSubRegion(const CString &StrIndexName, CChartRegion *pSubRegion);
	void                AddIndexGroup(CString strIndexGroupName);
	void				ClearRegionIndex(CChartRegion *pRegion, bool32 bDelExpertTip = false, bool32 bDelCmpIndex = false); // ���ָ��region��ָ��
	void				GetCurrentIndexNameArray(OUT CStringArray &aIndexNames); // ��ȡ��ǰ��������ָ������, ָ�����ƶ���Ψһ�ģ�so

	// �õ�ĳ��region ����ĳ���ָ������
	void				GetOftenFormulars(IN CChartRegion* pRegion, OUT CStringArray& aFormulaNames, OUT int32& iSeperatorIndex, bool32 bDelSame = true);
	void				ChangeToNextIndex(bool32 bPre);

	// �򵥵�����Ʒ
	virtual bool32				AddCmpMerch(CMerch *pMerchToAdd, bool32 bPrompt, bool32 bReqData, bool32 bDrawLine = true);
	virtual void				RemoveCmpMerch(CMerch *pMerch);
	virtual void				RemoveAllCmpMerch();
	virtual void				OnShowDataTimeRangeChanged(T_MerchNodeUserData *pData);	// ��ʾ������
	void						AddExpMerch(IN CMerch *pMerch);							// ���Ӵ���ָ�� 

	// ��ͼ��Ʒ����ʾ�����������е��˺���(UpdateMainMerchʱ�������)
	bool32				ChangeMainDataShowData(T_MerchNodeUserData &MainMerchNode, int32 iShowPosInFullList, int32 iShowCountInFullList, bool32 bPreShowNewest, const CGmtTime &TimePreStart, const CGmtTime &TimePreEnd);
	bool32				CalcKLinePriceBaseValue(CChartCurve *pCurve, OUT float &fPriceBase);
	void				RequestSingleCmpMerchViewData(CMerch *pMerch);	// ���󵥸���Ʒ������

	// ����ֲ�
	// iflag=0 ����֪ͨ 1=Y��仯 2=mousemove wparame=price, lparam=timeid 3=updateMainMerchData
	//			4=��������ܱ仯��
	void				FireIoViewChouMa(int32 iFlag, WPARAM w, LPARAM l);	
	void				FireIoViewChouMaAxisChange();	// ����ƶ�����ʱ����仯

	E_MainKlineDrawStyle GetMainKlineDrawStyle();		// ��ȡ��ͼK�߻�������
	void				SetMainKlineDrawStyle(E_MainKlineDrawStyle eStyle); // ������ͼK�߻�������
	int32				GetCurChartKlineDrawType(){ return MainKlineDrawStyleToChartDrawType(GetMainKlineDrawStyle()); }
	void				NotifyIoViewKlineDrawStyleChange(E_MainKlineDrawStyle eNewStyle);	// ֪ͨ����k����ͼ����ͼ���ͱ����
	void				DoIoViewKlineDrawStyleChange(E_MainKlineDrawStyle eNewStyle);		// ��Ӧ��ͼ���͵ı��

	static	E_MainKlineDrawStyle GetStaticMainKlineDrawStyle(){ return sm_eMainKlineDrawStyle; };

	CChartCurve			*GetCurIndexCurve(CChartRegion *pRegionParent);	// ��ȡָ��region�µ�ָ����

public:
	virtual void		OnIntervalTimeChange(const CGmtTime& TimeBegin, const CGmtTime& TimeEnd);
	virtual	void		KLineCycleChange(UINT nID);

	// ��ȡ���K��ָ�깫ʽ
	CFormularContent* GetClrFomular() { return m_pFormularClrKLine; }

public:
	friend	class CIoViewDuoGuTongLie;
	friend  class CDlgKLineQuant;
	friend  class CIoViewMultiCycleKline;
	CDlgTrendIndexstatistics*						m_pDlgTrendIndex;					// ͳ�ƶԻ���

	//////////////////////////////////////////////////////////////////////////	
	// SaneIndex
	bool32											m_bNeedCalcSaneLatestValues;		// �Ƿ���Ҫ��������ֵ
	CGmtTime										m_TimeToUpdateSaneIndex;			// ����ʵʱ���ݵ�ʱ���
	E_SaneIndexState								m_eSaneIndexState;					// ��ǰ���Ƚ�ָ���״̬
	T_IndexOutArray*								m_pIndexEMAForSane;					// GGTEMA ָ��ֵ����
	
	CArray<T_SaneIndexNode,T_SaneIndexNode>			m_aSaneIndexValues;					// �Ƚ�ָ���ֵ
	CArray<T_SaneNodeTip,T_SaneNodeTip&>			m_aSaneNodeTips;					// Tips		
	CArray<CMerch*, CMerch*>						m_aMerchsHaveCalcLatestValues;		// �Ѿ��������ֵ����Ʒ,��ֹ�����л���Ʒ��ʱ���ظ���μ�������ֵ.
	CArray<CKLine, CKLine>							m_aKLineInterval;					// ����ͳ�Ƶ�K��
	float	m_fPriceIntervalPreClose;

	mapLandMine										m_mapLandMine;						// ��Ϣ��������
	typedef map<CMerch *, CGmtTime>	MerchReqTimeMap;
	MerchReqTimeMap		m_mapMerchLandMineHasReqTimes;				// ��Ʒ��������Ϣ���������ʱ��, ������û�л���������ʱ��β����ط�

	typedef multimap<long long, T_StrategyInfo>		StategyInfoMulmap;
	StategyInfoMulmap								m_mulmapStategyInfo;			// �û������ź���Ϣ
	// �û������ź�
	typedef multimap<long long, CRect>			    StategyRectMulmap;
	StategyRectMulmap								m_mulmapStategyRects;			 // �û��û������ź�����
	bool											m_bReqStrategy;


	// ��Ȩ��Ϣ
	CArray<CRect, const CRect &>					m_aWeightRects;						// ��Ȩ��Ϣ��ʾ��������Ʒ�г�Ȩ��Ϣ��Ӧ

	// �û�Ͷ���ռǻ�������
	CMap<int32, int32, CRect, const CRect &>		m_mapUserNotesRect;				// �û�Ͷ���ռǵĻ�������
	typedef multimap<CString, CRect>	LandMineRectMap;
	LandMineRectMap									m_mapLandMinesRect;				// ��Ϣ��������

	// ͼ����Դ
	void											DrawZLMMIndex(CMemDCEx* pDC);		// ��ZLMM ָ����ʾ
	CArray<uint32, uint32>							m_aZLMMCrossID;						// ���������Ľ���
	bool32											m_bZLMMAlarm;						// ��������ָ���Ƿ�Ҫ����
	Image*											m_pImageZLMM;						// ��������

	static	E_MainKlineDrawStyle		sm_eMainKlineDrawStyle;			// ��̬��ʾ��ǰk����ͼ�е���ͼk������
	

/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewKLine)
	virtual BOOL TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewKLine)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnAllIndexMenu(UINT nID);
	afx_msg void OnOftenIndexMenu(UINT nID);
	afx_msg void OnOwnIndexMenu(UINT nID);
	afx_msg void OnMenuExpertTrade(UINT nID);
	afx_msg void OnWeightMenu(UINT nID);
	afx_msg void OnMenuIntervalStatistic();
	afx_msg void OnTest();
	afx_msg void OnMenu (UINT nID);
	afx_msg LRESULT OnMessageTitleButton(WPARAM wParam,LPARAM lParam);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnNotifyChouMa(WPARAM w, LPARAM l);
	afx_msg void OnTimer(UINT nIDEvent);

	//afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	public:
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_KLINE_H_
