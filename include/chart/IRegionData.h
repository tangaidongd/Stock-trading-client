#pragma once
#include "chartstruct.h"
#include "NCButton.h"
#include "PolygonButton.h"
#include "SelfDraw.h"
#include "faceschemetype.h"
#include "facescheme.h"
#include "XTipWnd.h"
#include "Region.h"
#include "SaneIndex.h"
#include "AxisYObject.h"
#include "chartexport.h"
#include "ChartCurve.h"
class CChartRegionViewParam;
class CRegionDrawNotify;
typedef vector<CSelfDrawNode*> vecSelfDrawNode;
typedef vector<vecSelfDrawNode> mapSelfDrawNode;
typedef CArray<CMerch *, CMerch *> MerchArray;

// ָ�������ϵİ�ť
#define INDEX_HELP		8888		// ����
#define INDEX_UPDATE	8889		// �޸Ĳ���

class CHART_EXPORT IChartRegionData
{
public:
	CArray<T_MerchNodeUserData*,T_MerchNodeUserData*>	m_MerchParamArray;		// ������Ʒ�� ����[0]һ��������Ʒ

	// region�б�
public:
	virtual void	OnRectIntervalStatistics(int32 iNodeBegin, int32 iNodeEnd){};
	virtual void	OnRectZoomOut(int32 iNodeBegin, int32 iNodeEnd){};
	virtual bool32 GetShowSaneIndexFlag(){return FALSE;};
	virtual void   GetNodeBeginEnd ( int32& iNodeBegin,int32& iNodeEnd ) = 0;
	virtual void   SetPickedSelfDrawCurve(CSelfDrawCurve* pCurve) = 0;
	virtual CSelfDrawCurve* GetPickedSelfDrawCurve() = 0;
	virtual T_IndexParam*	FindIndexParamByCurve ( CChartCurve* pCurve ) = 0;
	virtual void			FindCurvesByIndexParam ( T_IndexParam* pParam,CArray<CChartCurve*,CChartCurve*>& Curves ) = 0;
	virtual bool32	IsShowRiseFallMax() const{return FALSE;}
	virtual bool32	IsShowAvgPrice() const{return FALSE;}
	virtual bool32	CalcRiseFallMaxPrice(OUT float& fRiseMax, OUT float& fFallMax){return FALSE;};
	virtual CString GetSubRegionTitle() {return _T("");}
	virtual void CalcLayoutRegions(bool bOnlyUpdateMainRegion = true)= 0;
	virtual bool32	GetChartXAxisSliderText1(OUT CString &StrSlider, CAxisNode &AxisNode) = 0;
	virtual bool32	GetRisFallFlag()= 0;
	virtual bool32	GetSaneIndexNode(IN int32 iPos,OUT T_SaneIndexNode& Node){return FALSE;}
	virtual bool32			BeShowTopToolBar() = 0;
	virtual bool32			BeSysClrKLine(){return FALSE;}
	virtual CString GetClrKLineName() {return _T("");}
	virtual bool32	GetCrossLockInfo() const	= 0;
	virtual bool32			GetShowClrKLineFlag() {return FALSE;}
	virtual HCURSOR	SetNewCursor(HCURSOR hCursor){return FALSE;}
	virtual	void	DragMoveKLine(int32 icnt){};	// �϶�K��ƽ��
	// ��ͼregion
	CChartRegion*	m_pRegionMain;
	CChartRegion*	m_pRegionSeparator;				// Region�ָ���
	CChartRegion*	m_pRegionXBottom;				// x��Region
	CChartRegion*	m_pRegionYLeft;					// ��y��Region
	CChartRegion*	m_pRegionYRight;				// ��y��Region
	CChartRegion*	m_pRegionLeftBottom;			// ���½�Region
	CChartRegion*	m_pRegionRightBottom;			// ���½�Region

	// ������region
	CArray<T_SubRegionParam, T_SubRegionParam&> m_SubRegions;//��Region

public:
	Image*			m_pImageShake;						// ������
	Image*			m_pImageStrongStabilityUp;			// ǿ,�ȶ�,����
	Image*			m_pImageNormalStabilityUp;			// һ��,�ȶ�,����
	Image*			m_pImageWeekStabilityUp;			// ��,�ȶ�,����

	Image*			m_pImageStrongUnStabilityUp;		// ǿ,���ȶ�,����
	Image*			m_pImageNormalUnStabilityUp;		// һ��,���ȶ�,����
	Image*			m_pImageWeekUnStabilityUp;			// ��,���ȶ�,����

	Image*			m_pImageStrongStabilityDown;		// ǿ,�ȶ�,�½�
	Image*			m_pImageNormalStabilityDown;		// һ��,�ȶ�,�½�
	Image*			m_pImageWeekStabilityDown;			// ��,�ȶ�,�½�

	Image*			m_pImageStrongUnStabilityDown;		// ǿ,���ȶ�,�½�
	Image*			m_pImageNormalUnStabilityDown;		// һ��,���ȶ�,�½�
	Image*			m_pImageWeekUnStabilityDown;		// ��,���ȶ�,�½�
	Image*			m_pImageDTOC;						// ��ͷ����
	Image*			m_pImageDTAC;						// ��ͷ�Ӳ�
	Image*			m_pImageDTRC;						// ��ͷ����
	Image*			m_pImageDTCC;						// ��ͷ���

	Image*			m_pImageKTOC;						// ��ͷ����
	Image*			m_pImageKTAC;						// ��ͷ�Ӳ�
	Image*			m_pImageKTRC;						// ��ͷ����
	Image*			m_pImageKTCC;						// ��ͷ���

	Image*			m_pImageDCKO;						// ����տ�
	Image*			m_pImageKCDO;						// ����࿪

	Image*			m_pImageExpert;						// ����࿪

	Image*			m_pImageRiseFallFlag;				// �ǵ����

	Image*			m_pImageCLBuy;						// �������ź�
	Image*			m_pImageCLSell;						// �������ź�
	Image*			m_pImageCLMul;						// ���Զ��ź�

	CImageList										m_ImageListRiseFallFlag;			// �ǵ����
	CChartRegion*	m_pRegionPick;
	CChartCurve*	m_pCurvePick;
	CNodeSequence*	m_pNodesPick;
	CNodeData		m_NodePick;

	CNodeSequence*	m_pExportNodes;

	CPriceToAxisYObject::E_AxisYType m_eMainCurveAxisYType;		// ��ͼ��Y��������

	//CStringArray	m_aUserBlockNames;		// ���ڶ���Ҫʹ�� ���뵽��ѡ��飬ת��IoViewBase��

public:
	uint32			m_uiFlag;
	int32			m_iChartType;//0:fs 1:kline 2:tick 3: expfs 4: klinearbitrage 5:trendarbitrage 6:���Լͬ��K�� 7:���Լͬ�з�ʱ
	bool			m_bMoreView;// �Ƿ��Ƕ��Լͬ�н���
	CXTipWnd		m_TipWnd;
	CPoint			m_PtGridCtrl;
	int32			m_iDataServiceType;
	CString			m_StrText;
	CStringArray	m_FormulaNames;
	float			m_fPricePrevClose;
	int32			m_iPickSubRegionIndex;	// ѡ���ĸ�ͼ

protected:
	CChartRegionViewParam*	m_pRegionViewData;
	CRegionDrawNotify*	m_pRegionDrawNotify;


	CString			m_StrMerchCodeXml;
	int32			m_iMarketIdXml;

	int32			m_iSubRegionNumXml;
	CStringArray	m_IndexNameXml;
	CArray<int32,int32>	m_IndexRegionXml;
	CArray<int32,int32> m_aRegionHeightXml;
	CArray<double, double> m_aRegionHeightRadioXml;
	CArray<int32,int32> m_aRegionMainIndexXml;
	mapSelfDrawNode	m_mapRegionSelfDrawNodes;

	bool32			m_bShowTopToolBar;
	bool32			m_bShowIndexToolBar;
	bool32			m_bShowHelp;
	bool32			m_bShowExpBtn;			// K�߷�ʱ��ͼ�����Ƿ���ʾָ����ť
	bool32			m_bShowTopMerchBar;		// �Ƿ���ʾ������Ʒ����

	int32			m_iIntervalXml;	//��ͼ����
	bool32			m_bTrendIndex;  // ...fangz 0815
	bool32			m_bSaneIndex;   // ...fangz 0815

	int32			m_iDrawTypeXml;//��������
	int32			m_iNodeNumXml;

	UINT			m_uiUserMinutesXml;
	UINT			m_uiUserDaysXml;

	CSelfDrawCurve*	m_SelfDrawCurvePick;
	bool32			m_bTodayRise;
	bool32			m_bTracking;	// ����뿪�ı��
	bool32			m_bInitialFloat;

	// �򵥵�����Ʒ�й�
	MerchArray	    m_aMerchsCmpNeedUpdateShowData;		// ��ʾ���������µı���Ҫ���µĵ�����Ʒ
	bool32			m_bAutoAddCmpExpMerch;			// �Զ����Ӷ�Ӧָ��

	bool32			m_bEnableRightYRegion;			// ������Y��
	bool32			m_bEnableLeftYRegion;			// ������Y��

	// �Ŵ���С������ģʽ
	// 1: ��ʮ�ֹ���ʱ��, ��������
	// 2: ��ʮ�ֹ��, ��������궨λ��(��ʱ�Զ�λ�����K ��Ϊ��������)
	// 3: ��ʮ�ֹ��, �����Ǽ������Ҽ���λ��(��ʱʮ�ֹ��ʼ�վ�ȷ��λ�����K ����)

	// bool32			m_bAccurateZoom;				// ��Ӧ�����ַ����ı��, ĿǰֻK �����õ�, �Ժ�����ͼҲ������Ҫ

public:
	bool32			m_bFromXml;
	T_IndexPostAdd	m_IndexPostAdd;

	CStringArray	m_aStrVolIndexNames;			// fangz0624.Ŀǰ��д����,ָ��ϵͳ�����Ժ��ٸ�.ϵͳ�����еĳɽ�����ָ��

	bool32			m_bForbidDrag;

	bool32			m_bLockCross;

protected:

	// ����ͳ�Ƶ���ֹʱ��
	CGmtTime		m_TimeIntevalBegin;
	CGmtTime		m_TimeIntevalEnd;

	// ��㽻��
	float			m_fPriceMouseClick;		// �����˵����ļ۸�
	HCURSOR         m_hNewCurosr;

	//k�ߺͷ�ʱͼ ����
	bool32              m_bOwnAdvertise;          //ӵ�й��dlg��view,��changesizeʱҪ�ػ����dlg
	bool32              m_bViewIsvisible;
	bool32              m_bIsHide;
	bool32              m_bIsKLineOrTend;    //K�߻��߷�ʱͼ 

	std::map<int, CNCButton> m_mapTopBtn;			// ������������߹�����ť
	std::map<int, CNCButton> m_mapTopCenterBtn;		// �����������м乫����ť
	std::map<int, CNCButton> m_mapKRightBtn;		// K�߶����������ұ߰�ť
	std::map<int, CNCButton> m_mapTrendRightBtn;	// ��ʱͼ�����������ұ߰�ť
	std::map<int, CNCButton> m_mapTopExpBtn;		// ����������ָ����Ʒ��ť

	std::map<int, CPolygonButton> m_mapBottomBtn;		// �ײ�ָ�깤����
	int32				m_iTopBtnHovering;			// ��ʶ�����밴ť����
	int32				m_iTopCenterHovering;
	int32				m_iKTopHovering;
	int32				m_iTrendTopHovering;
	int32				m_iBottomHovering;
	int32				m_iTopExpHovering;
	int32				m_iLeftWidth;				// top ��߰�ť�ܿ��
	int32				m_iRightWidth;				// top �ұ߰�ť�ܿ��

	CStringArray		m_ToolBarIndexName;			// �ײ�ָ�깤������ʾ��ָ������
	int32				m_iDayIndex;				// ���շ�ʱ������
	CString				m_strMainIndexName;			// ��ͼָ������

public:
	int32				m_iSaneTradeTimes;					// ���״���
	int32				m_iSaneRealKlineNums;				// ��ʵͳ����
	float				m_fSaneAccuracyRate;				// ׼ȷ��
	float				m_fSaneProfitability;				// ������
	float				m_fProfit;							// ����		

	enum{
		// ��ʱ��ť
		TOP_IOCMD_TREND,

		// ��ʱͼ�����ұ߰�ť
		TOP_TREND_SHOW,
		TOP_TREND_BID,
		TOP_TREND_TREND_ADD,
		TOP_TREND_MORE_TREND,
		TOP_TREND_TREND_SUB,
		TOP_TREND_SELFDRAW,
		TOP_TREND_TICK,
		TOP_TREND_MORE_MERCH,
		TOP_TREND_FLASH,	// ���罻��

		// k�߶����ұ߰�ť
		TOP_KINE_SHOW_MAININDEX,
		TOP_KINE_SHOW,
		TOP_KINE_SELFDRAW,
		TOP_KINE_TICK,
		TOP_KINE_ADD_CMP,
		TOP_KINE_FLASH,		// ���罻��
		TOP_KINE_HELP,
		TOP_KINE_WEIGHT,	// ��Ȩ

		// �в��İ�ť
		TOP_CENTER_PRE,			
		TOP_CENTER_MERCH,
		TOP_CENTER_NEXT,

		// ����ָ����Ʒ��ť
		TOP_EXP_SZZS,	// ��ָ֤��
		TOP_EXP_SZCZ,	// ��֤��ָ
		TOP_EXP_CYBZ,	// ��ҵ��ָ
		TOP_EXP_HSSB,	// ����300

		TOP_ADD_ZIXUAN,	// ��ӵ���ѡ

		// ָ��
		BOTTOM_INDEX_BEGIN,
	};
};

class CHART_EXPORT IIoViewBase
{
public:
	virtual COLORREF GetIoViewColor(E_SysColor eSysColor) = 0;
	virtual LOGFONT* GetIoViewFont(E_SysFont eSysFont) = 0;
	virtual CFont*			GetIoViewFontObject(E_SysFont eSysFont) = 0;
	virtual bool32			IsActive() = 0;
	virtual bool32			ParentIsIoView() = 0;
	virtual int32 GetChartType() = 0;
	virtual CMerch* GetMerchXml() = 0;	
	virtual BOOL IsKindOfIoViewKLine() const {return FALSE;}
	virtual BOOL IsKindOfIoViewTrend() const {return FALSE;}
	virtual BOOL IsKindOfIoViewChart() const {return FALSE;}
	// ÿ��ҵ����ͼ����ɫ��������Ϣ:
	CFontNode		m_aIoViewFont[ESFCount];					// ��ǰҵ����ͼʹ�õ�����
	COLORREF		m_aIoViewColor[ESCCount];					// ��ǰҵ����ͼʹ�õ���ɫ	
	
	int32				m_iActiveXpos;
	int32				m_iActiveYpos;
	COLORREF			m_ColorActive;
	CMerch			*m_pMerchXml;
	CMerchKey		m_MerchXml;			// ���m_pMerchXmlΪNULL, ��ȡ�ø��ֶΣ� ��������Ȩ�����⵼����ʱȥ����xml������
};

typedef struct CHART_EXPORT _IChartBrige
{
	CWnd *pWnd;
	IIoViewBase *pIoViewBase;
	IChartRegionData *pChartRegionData;

	_IChartBrige()
	{
		pWnd = NULL;
		pIoViewBase = NULL;
		pChartRegionData = NULL;
	}
}IChartBrige;