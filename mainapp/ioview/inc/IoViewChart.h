#ifndef _IO_VIEW_CHART_H_
#define _IO_VIEW_CHART_H_

#include "XScrollBar.h"
#include "XTipWnd.h"
#include "IoViewBase.h"
#include "IoViewShare.h"
#include "CFormularContent.h"
#include "MerchManager.h"
#include "UserBlockManager.h"
#include "GridCtrlSys.h"
#include "MemdcEx.h"
#include "IoViewShare.h"
#include "PolygonButton.h"
#include "DlgPullDown.h"

///////////////////////////////////////////////////////////////////////////////
class CGridCellSys;
class CChartRegionViewParam;
class CRegionDrawNotify;

//
// extern const int32 KiMenuIDIntervalStatistic;
// 
// extern const int32 KiMenuIDIntervalStatistic;
// extern const char* KIoViewChartKLineInterval;
// extern const char* KIoViewChartUserMinutes;
// extern const char* KIoViewChartUserDays;

#define  TOP_MERCH_HEIHGT  33 
#define  TOP_BUTTON_HEIHGT 21

// 

extern const int32 KiMenuIDIntervalStatistic;
// CIoViewChart
class CIoViewChart : public CIoViewBase, public CGridCtrlCB, public CUpdateShowDataChangeListener,public IChartRegionData
{
public:
	enum E_IoViewChartFlag
	{
		KRegionCreated			= 0x00000001,
		KRegionSplited			= 0x00000002,
		KRegionReady			= 0x00000008,			// SplitRegion() ���ֺ�Region �������, ������Ӧ�Ҽ��˵���ز���
		
		KMerchKLineUpdate		= 0x00000010,
		KMerchTimeSalesUpdate	= 0x00000020,
	};

	enum E_SCROLL_DIRECT
	{
		ESD_know = 0,
		ESD_left = 1,
		ESD_right = 2,
	};
	
	struct T_SpecialBtn
	{
		CString StrName;
		CString StrMerchCode;
		int32   iMarketId;


		T_SpecialBtn()
		{
			StrName = L"";
			StrMerchCode = L"";
			iMarketId   = 0;
		}
	};

	//	add by weng.cx, for limit merch change
	struct T_LimitMerch
	{
		int		iMarketId;
		CString tMerchCode;
		CString tName;
		T_LimitMerch()
		{
			iMarketId = 0;
			tMerchCode = L"";
			tName = L"";
		}		
	};

	struct T_CycleMenuParam
	{
		UINT	uID;
		CString StrName;
		Image   *pImage;

		T_CycleMenuParam(UINT uId, CString strName, Image *pImg)
		{
			uID		= uId;
			StrName = strName;
			pImage  = pImg;
		}
	};

	struct T_IndexGroup
	{
		CString		StrIndexName;				// ָ������
		bool32		bRegionMainIndex;			// �Ƿ���ͼָ��
		int32		iRegion;					// ָ���±�ֵ


		T_IndexGroup()
		{
			StrIndexName = L"";
			bRegionMainIndex = false;
			iRegion = 0;
		}
	};


	struct T_SpecicalIndexAttributes
	{
		CString		StrIndexName;				// ָ������
		int32		iOpenCfm;					// �Ƿ�򿪰���
		int32		iBtnWidth;					// ָ�갴ť���
		CString     strCharacter;				// ָ�갴ť��������
		int32		iBlod;						// ָ�갴ť�����Ƿ�Ӵ�
		float		fSize;						// ָ�갴ť�����С
		CString     strNomarlBKColor;			// ��������ɫ
		CString     strHotBKColor;			    // ��������ı���ɫ
		CString     strPressBKColor;			// ��ť���µı���ɫ
		CString     strNomarlTextColor;			// ����������ɫ
		CString     strHotTextColor;			// ���弤����ɫ
		CString     strPressTextColor;			// ���尴����ɫ	
		T_SpecicalIndexAttributes()
		{
			StrIndexName = L"";
			iOpenCfm	 = -1;
			iBtnWidth    = -1;
			strCharacter = L"";
			iBlod        = -1;
			fSize		 = -1.0;
			strNomarlBKColor	= L"";
			strHotBKColor		= L"";
			strPressBKColor		= L"";
			strNomarlTextColor	= L"";
			strHotTextColor		= L"";
			strPressTextColor	= L"";
		}
	};

public:
	CIoViewChart();
	virtual ~CIoViewChart();
	
	DECLARE_DYNCREATE(CIoViewChart)

//////////////////////////////////////////////////////////////////
	// from CIoViewBase
public:
	virtual bool32	FromXml(TiXmlElement * pTiXmlElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();
	virtual bool32	GetStdMenuEnable(MSG* pMsg);
	virtual BOOL IsKindOfIoViewChart() const {return TRUE;}
	// ���Ӷ���IoViewChart�ṩ�����������XML��ʼ���뱣��
	virtual bool32  FromXmlInChild(TiXmlElement *pTiXmlElement) { return true; }
	virtual CString ToXmlInChild() { return CString(); }		// ���ౣ��xml����
	virtual CString GetDefaultXMLInChild() { return CString(); }
	virtual CString	ToXmlEleInChild() { return CString(); }		// ���ౣ��xmlԪ��
	virtual int32 GetChartType(){return m_iChartType;}
	
public:		
	// from CIoViewBase
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual bool32	OnVDataAddCompareMerch(IN CMerch *pMerch);
	virtual void	OnVDataRemoveCompareMerch(IN CMerch *pMerch);
	virtual void	OnVDataClearCompareMerchs();
	virtual void	SetChildFrameTitle(){};
	
	virtual void	OnIoViewColorChanged();
	virtual void	OnIoViewFontChanged();
	virtual void	OnVDataFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName );

	// from CGridCtrlCB
	virtual void	OnFixedRowClickCB(CCellID& cell){}
	virtual void	OnFixedColumnClickCB(CCellID& cell){}
    virtual void	OnHScrollEnd(){}
    virtual void	OnVScrollEnd(){}
	virtual void	OnCtrlMove( int32 x, int32 y );
	virtual bool32	OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew ){return true;}

	// �����ж���ʾ�����Ƿ����Ľӿ�
	virtual void    OnShowDataTimeRangeChanged(T_MerchNodeUserData *pData){}

///////////////////////////////////////////////////////////////
// ������ӿ�
protected:
	virtual	void	CalcLayoutRegions(bool bOnlyUpdateMainRegion = true) {}
	virtual void	CalcReginMinMax(CChartRegion* pRegion){}
	virtual void	Draw();
	virtual void	CreateRegion();
	virtual void	SplitRegion();
	virtual bool32	AddSubRegion( bool32 bSilence = true, bool32 bNotify=true );
	virtual bool32	DelSubRegion(CChartRegion* pRegion, bool32 bEnableChangeToPick = true);
  	virtual T_IndexParam*	AddIndex ( CChartRegion* pRegion,CString StrIndexName, bool32 bMainIndex = TRUE);
	virtual void	UpdateAxisSize( bool32 bPreDraw = true );
	virtual void	DoFromXml();
	virtual void	InitialShowNodeNums(){};
	virtual void	SetXmlSource (){};
	virtual void	SetCurveTitle ( T_MerchNodeUserData* pData ){};

	virtual void	InitCtrlFloat ( CGridCtrl* pGridCtrl );
	virtual void	SetFloatData ( CGridCtrl* pGridCtrl,CKLine& KLine,float fPricePrevClose,float fCursorValue,CString StrTimeLine1,CString StrTimeLine2,int32 iSaveDec){};
	virtual void	SetFloatData ( CGridCtrl* pGridCtrl,float fCursorValue,int32 iSaveDec){}
	virtual void	ClipGridCtrlFloat ( CRect& Rect ){};
	virtual void	RequestNodeInit ( T_MerchNodeUserData* pData){}

	virtual void	DrawContentExtraYLine(CMemDCEx* pDC, CChartRegion* pRegion, CFormularContent* pContent);
	virtual void	DrawTitle1 ( CMemDCEx* pDC );
	
	virtual void    OnKeyHome(){}
	virtual void    OnKeyEnd(){}
	virtual void    OnKeyUp(){}
	virtual void	OnKeyDown(){}
	virtual void	OnKeyLeft(){}
	virtual void	OnKeyRight(){}
	virtual void	OnKeyLeftAndCtrl(){}
	virtual void    OnKeyLeftAndCtrl(int32 iRepCnt);
	virtual void	OnKeyRightAndCtrl(){}
	virtual void    OnKeyRightAndCtrl(int32 iRepCnt);
	virtual void	OnKeySpace();

    void            SetTopButtonStatus(E_NodeTimeInterval eNodeInterval, int32 iValue);

public:
	virtual void	AddIndexGroup(CString strIndexGroupName) {};
	virtual void	SetSelectIndexGroupName(CString strSelectIndexGroupName);

public:
	// ��ȡ��ͼ�ı�������ť:
	virtual int32	GetSubRegionTitleButtonNums() { return 0; }
	virtual const T_FuctionButton*	GetSubRegionTitleButton(int32 iIndex) { return NULL; }
	
	// �����Ż� - ��ȡX��slider��ʾ������CalcXAxis����
	virtual bool32	GetChartXAxisSliderText1(OUT CString &StrSlider, CAxisNode &AxisNode);

	// ��ѡ�Ŵ�

	// ����ͳ��

	// ��ȡYLeft�ʺ���ʾ���ַ������� iWideCharCount Ϊ ��Ҫ �����ĺ��ֵȿ��ַ���������Ĭ�Ϻ���Ϊ eng ������
	virtual int32   GetYLeftShowCharWidth(int32 iWideCharCount = 0) const;

	// ��ӵ�����Ʒ�ļ򵥽ӿ�
	virtual bool32 AddCmpMerch(CMerch *pMerchToAdd, bool32 bPrompt, bool32 bReqData){ return false; };
	virtual void				RemoveCmpMerch(CMerch *pMerch){};
	virtual void				RemoveAllCmpMerch(){};
	//
	virtual bool32	GetAdvDlgPosition(IN int32 iWidth, IN int32 iHeight, OUT CRect& rtPos){return false;}
	virtual bool32  IsAlreadyToShowAdv(){return false;}

	virtual	void	GotoCompletePrice() {}
	virtual	void	SetTrendMultiDay(int iDay, bool32 bClearData, bool32 bReqData){};	// ���ö��շ�ʱ
	virtual int32   GetTrendMultiDay(){return 1;};
	virtual void	ClearRegionIndex(CChartRegion *pRegion, bool32 bDelExpertTip = false){}; // ���ָ��region��ָ��
	virtual bool32	AddShowIndex(const CString &StrIndexName, bool32 bDelRegionAllIndex = false, bool32 bChangeStockByIndex = false, bool32 bShowUserRightDlg=false){return true;};

	virtual void   SetAvgPriceEvent(){};
////////////////////////////////////////////////////////////
public:
	bool32			LoadAllIndex(CNewMenu* pMenu);
	void			UpdateSelfDrawCurve ();
	void			RemoveAllSelfDrawCurve();
	bool32			DeleteCompareCurve ( CChartCurve* pCurve );

	void			RegionMenu2 ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y );
	void			RegionIndexMenu ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y );
	void			RegionIndexBtn(CChartRegion* pRegion, CDrawingCurve* pCurve,int32 iID);
	void			MouseWheelLoopMerch(UINT nFlags, short zDelta, CPoint pt);
	CString			BaseFloat2String(float fValue,bool32 bZeroAsHLine, bool32 bNeedTerminate = false);
	void			ShowIndex();
	void			ShowIndexParam();
	void			ShowIndexHelp();
	void			Add2Block(const CString &StrBlockName);
	bool32			IsRegionReady();
	int32			ExistAttendMerch (IN CMerch *pMerch);
	T_MerchNodeUserData*	NewMerchData (IN CMerch *pMerch, bool32 bMainMerch, bool32& bInhert );
	bool32			DeleteMerchData ( int32 iPos );
	bool32			DeleteIndexParamData ( T_IndexParam* pParam);
	void			RemoveIndexsNodesData ( T_MerchNodeUserData* pData );
	bool32			DeleteIndexCurve ( CChartCurve* pCurve );
	void			DeleteRegionCurves ( CChartRegion* pRegion );
	void			DragDropCurve ( CChartCurve* pSrcCurve,CChartRegion* pDestChart);
	void			InsertEmptyNodes ( T_MerchNodeUserData* pData,bool32 bFull = true );
	void			GetNodeBeginEnd ( int32& iNodeBegin,int32& iNodeEnd );

	void			UpdateRegionSelfDrawCurve ( CChartRegion* pRegion );
	void			DeleteRegionSelfDrawCurve ( CChartRegion* pRegion);

	void			DrawIndexExtraY ( CMemDCEx* pDC,CChartRegion* pRegion );
	CString			GetChartGuid ( CChartRegion* pRegion );
	void			SliderId ( int32& id, int32 iJump );
	void			OnExport();

	void			DrawInterval(CMemDCEx* pDC, CChartRegion* pRegion);

	void			CalcTodayRise(IN CMerch *pMerch);
	bool32			GetRisFallFlag()	{ return m_bTodayRise; }
	void			AddDrawBaseRegion(CMemDCEx* pDC);
	void			AddDrawCornerRegion(CMemDCEx* pDC);

	void			HideAllTips();		// ��������tip
					
	void			SetPickedSelfDrawCurve(CSelfDrawCurve* pCurve);
	CSelfDrawCurve* GetPickedSelfDrawCurve();
	
public:
	bool32			BeVolIndex(const CString& StrIndexName,CStringArray& aIndexs);
	void			SortIndexByVol(IN CStringArray& aIndex,OUT int32& iIndex);
	void			SortIndexByAlpha(CStringArray& aIndex); //2013-5-4 ����ĸ����
	virtual BOOL IsKindOfIoViewKLine();
	T_IndexParam*	FindIndexParamByCurve ( CChartCurve* pCurve );
	void			FindCurvesByIndexParam ( T_IndexParam* pParam,CArray<CChartCurve*,CChartCurve*>& Curves );
	T_IndexParam*	FindIndexParamByName ( CString StrName );

	void			FindRegionFormula ( CChartRegion* pRegion, CStringArray& arr );
	void			DeleteSameString ( CStringArray& arr1,CStringArray& arr2);

	void			FormulaParamTempUpdate ( CString StrName );
	void			FormulaUpdate ( CString StrName );
	void			FormulaDelete ( CString StrName );
	int32			GetDigit(int32 iNum);
	int32			GetInter(int32 iNum);
	int32			MakeMinBigger(int32 iMin, int32 iStep);
	void			OnKeyLeftRightAdjustIndex();
	void			SetGridFloatInitPos();
	void			AdjustGridFloatSize();
	void			SetMainCurveAxisYType(CPriceToAxisYObject::E_AxisYType eType);	// ��ͼ��y��������
	CPriceToAxisYObject::E_AxisYType GetMainCurveAxisYType() const { return m_eMainCurveAxisYType; }

	// �򵥵�����Ʒ�й�
	void			OnCmpMerchsNeedUpdateAsync();	// ���е�����Ʒ������Ҫ�����ؼ���, Timerʱ����
	void			OnCmpMerchsNeedUpdate(bool32 bNeedDraw = true);		// ���õ�����Ʒ���ݽ��и���
	void			DoCmpMerchsUpdate(bool32 bNeedDraw = true);			// ������Ҫ���µĵ�����Ʒ����
	int32			RemoveCmpMerchFromNeedUpdate(CMerch *pMerch);	// �ӵ�����Ʒ�����¶������Ƴ�

	bool32			SetAutoAddExpCmpMerch(bool32 bEnable, bool32 bPrompt=true);	// �����Ƿ�ɹ�����״̬
	bool32			IsAutoAddExpCmpMerch()const { return m_bAutoAddCmpExpMerch; }

	void			EnableRightYRegion(bool32 bEnable);		// ������Y����ʾ, ������
	bool32			IsEnableRightYRegion() const { return m_bEnableRightYRegion; }
	bool32          IsEnableLeftYRegion() const { return m_bEnableLeftYRegion; }

	// ����ʮ�ֹ�����ݵĸ��� (����ʮ�ֹ���ʱ��Ŵ�. ��ʱ���ܸ��µ�ǰ��ʮ�ֹ����Ϣ, ��������ƶ���)
	void			LockCrossInfo(bool32 bLock) { m_bLockCross = bLock; }
	bool32			GetCrossLockInfo() const	{ return m_bLockCross; }

	static	bool32	IsShowVolBuySellColor();	// �Ƿ���ʾ�ɽ�����������ɫ��Ϣ(�������ɽ���)
	static  void	SetShowVolBuySellColor(bool32 bShow);	// ����

	void            InitChartHideMerchButton();             // ������ʾ��Ʒ����ʱ��ˢ�°�ť��ʾ

protected:
	// �������ǰ�ı�������
	virtual	void	ClearLocalData(bool32 bClearAll){}
	void			ReDrawAysnc();

	void			AddSelfDrawNodesFromXML();

public:
	// ����ͳ�����
	void			SetIntervalTime(const CGmtTime& Time);
	void			SetIntervalTime(const CGmtTime& tmBegin, const CGmtTime& tmEnd);

	void			SetIntervalBeginTime(const CGmtTime& Time);
	void			SetIntervalEndTime(const CGmtTime& Time);
	
	void			ClearIntervalTime();

	HCURSOR			SetNewCursor(HCURSOR hCursor);

	virtual void	OnIntervalTimeChange(const CGmtTime& TimeBegin, const CGmtTime& TimeEnd) {}
	//���Ի���
public:
	void			GetCharacternNums(const CString& StrIn, int32& iEngNums, int32& iChnNums);
	void			FillCStringWithSpace(int32 iSpaceNum, CString& StrValue );
	CString			MakeMenuString(const CString& StrName, const CString& StrExplain, bool32 bAppendSpace=true);

	void            OnAdvertiseRedarw();
	//
	void            SetAdvOwner(CWnd *pWnd);
	void			HideAdvDlg(); 
	void			RedrawAdvertise();
	void            SetHideFlag();
	void            SetAdvFont(LOGFONT *pLogFont);
// ����������
public:
	bool32			BeShowTopToolBar() {return m_bShowTopToolBar;}
	bool            BeInSpecialMerchArray(CMerch *pMerch);
	bool            GetShowExpBtn() { return m_bShowExpBtn;}
	CString			GetCodeByMerchName(OUT CString& StrName);	// �������õ�ָ����Ʒ���Ʋ���code

	CMerch*			ToLimitMerch(bool bUp);	//	���޶���Ʒ
	bool			HaveLimitMerch();		//	�Ƿ����޶���Ʒ
	bool			IsLimitMerch(const CMerch* pMerch);			//	�Ƿ����޶���Ʒ
private:
	void			InitialTopBtns();
	void			AddTopButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	void			DrawTopButton();
	int				TTopButtonHitTest(CPoint point);

	void			InitialTopCenterBtns();
	void			InitialKTopRightBtns();
	void			InitialTrendTopRightBtns();
	void			InitialTopExpBtns();

	int32			DrawKTopRightBtn(Graphics *pGraphics);
	int32			DrawTrendTopRightBtn(Graphics *pGraphics);
	void			DrawTopCenterBtn(Graphics *pGraphics);	
	void			DrawTopExpBtn(Graphics *pGraphics);
	
	void			AddKTopRightBtn(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	void			AddTrendTopRightBtn(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	void			AddTopCenterBtn(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	void			AddTopExpBtn(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	
	int				TKTopRightBtnHitTest(CPoint point);
	int				TTrendTopRightBtnHitTest(CPoint point);
	int				TTopCenterBtnHitTest(CPoint point);
	int				TTopExpBtnHitTest(CPoint point);
	
	void			RecalcKTopBtnRect();	// ���㰴ť������
	void			RecalcTrendTopBtnRect();
	void			RecalcTopCenterBtnRect();
	void			RecalcTopExpBtnRect();

	// �ײ�ָ�깤����
	void			InitialBottomIndexBtns();
	void			RecalcBottomIndexBtnRect();	// ���㰴ť������
	void			AddBottomIndexButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL, LPCTSTR lpszDefCaption = NULL);
	void			DrawBottomIndexButton();
	int				TBottomIndexButtonHitTest(CPoint point);

	void			OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	void			UpdateIndexsBtn();
	
	bool32			NeedAddScrollButton(const CRect& rcClient);	// ��K�ߵĵײ���ť����ȫ����ʾʱ����������ӹ�����ť
	void			OnLeft();
	void			OnRight();
	void			AddIndexToBottomBtn(CString strIndexName);		

	//	��ʼ�����ڲ˵���ʾ����
	void            InitialCycleMenuParam();
	
	// ��ʾ���ڲ˵�
	void			ShowCycleMenu();

	// �л�K������
	BOOL            ChangeKLineCycle(UINT uId);
	
protected:
	void			SetTopCenterTitle(CString& StrTitle);
	void			FlashOrder();
	bool32			IsShowFlashOrder();
public:
	virtual CFormularContent* GetClrFomular() { return NULL; }		// ��ȡ���K��ָ�깫ʽ
public:
	CStringArray	m_aAllFormulaNames;					// ����ָ��: ÿ��ID ��Ӧ������һ������
	std::vector<T_SpecialBtn>    m_aExpBtns;			// ���ӵ��г���ť
	std::vector<T_LimitMerch>	 m_vtLimitMerch;		//	������Ʒ���л���Χ

	// 
	CDlgPullDown   *m_pDlgPullDown;

	buttonContainerType m_mapPullDown;					//	�����˵��İ�ť��

	vector<T_CycleMenuParam> m_aCycleMenuParam;			// ���ڲ���

public:
		int32 m_iTopMerchHeight;		// 
		int32 m_iTopButtonHeight;

		bool32	m_bShowAvgPrice;	// �Ƿ���ʾ���ۺ͵�ǰ��, ����׼��ͨ��һ��IOVIEW_SETTING_CHANGE��Ϣ��������Ȼ���ڸ���
		bool32	m_bShowRiseFallMax;	// �Ƿ���ʾ�ǵ�ͣ�۸�, ����׼��ͨ��һ��IOVIEW_SETTING_CHANGE��Ϣ��������Ȼ���ڸ���
		bool32	m_bRiseFallAxis;	// �ǵ�ͣ����


		// ����ָ�����Ա���
private:
		map<int32, T_SpecicalIndexAttributes> m_mapSpecailIndexAttri;

protected:
	friend class CIoViewDuoGuTongLie;
	friend class CIoViewMultiCycleKline;
	//����
	CGridCtrlSys	m_GridCtrlFloat;
	CGridCellSys*	m_pCell1;
	CGridCellSys*   m_pCell2;
	protected:
	CDelayKeyDown	m_Delay;

	int32	  m_iNumber;	// �ӵڼ�����ť��ʼ��ʾ
	CPolygonButton m_leftScroll;	// ������ʾ������ָ��İ�ť
	CPolygonButton m_rightScroll;

	Image*			   m_pImgZixuan;		// �Ѿ�����ѡ��ͼƬ
	Image*			   m_pImgAddZixuan;		// �����ѡ��ťͼƬ

	//�ײ��м�Scroll
	Image*             m_pImgPrevScroll;
	Image*             m_pImgNextScroll;

	//ȫ��/�ָ���ť
	Image*             m_pImgFull;
	Image*             m_pImgRestore;

	// ���ڲ˵���
	Image*             m_pImgCycleMenuItem;

	// ����Kʱ�����ڰ�ť
	Image*             m_pImgCycleBtnItem;

	E_SCROLL_DIRECT	   m_eScorllDirect;

	bool32			   m_bShowWeight;	// �Ƿ���ʾ��Ȩ��ť

	CString			   m_strSelectIndexGroupName;
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewChart)
//	BOOL OnCommand(WPARAM wParam,LPARAM lParam ); 
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewChart)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLayOutAdjust();
	afx_msg void OnMenu2 ( UINT nID );
	afx_msg	LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSelfDrawClrChange(WPARAM wParam, LPARAM lParam);
	afx_msg	void OnEsc( UINT nID);
	afx_msg void OnShowWindow( BOOL bShow, UINT nStatus );
	afx_msg	LRESULT	OnIdleUpdateCmdUI(WPARAM w, LPARAM l);
	afx_msg LRESULT	OnISKINDOFCIoViewChart(WPARAM w, LPARAM l);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_CHART_H_