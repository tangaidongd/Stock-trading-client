#ifndef _IO_VIEW_REPORT_H_
#define _IO_VIEW_REPORT_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "IoViewShare.h"
#include "GridCtrlSys.h"
#include "CStaticSD.h"
#include "GuiTabWnd.h"
#include "ReportScheme.h"
#include "MerchManager.h"
#include "BlockManager.h"

#include "MarkManager.h"
#include "PlugInStruct.h"
#include "NCButton.h"
#include <map>
#include <vector>
using std::map;
using std::vector;

/////////////////////////////////////////////////////////////////////////////
// CIoViewReport

typedef struct T_BlockDesc
{
	enum E_BlockType
	{
		EBTPhysical = 0,		// ������
		EBTUser,				// �û����
		EBTLogical,				// �߼����
		EBTSelect,				// ����ѡ��
		EBTBlockLogical,		// �����߼����
		EBTMerchSort,	        // ����ǿ��
		EBTRecentView,			// ������
		EBTOpenBlockLogical,	// �������߼����
		EBTCount
	};

	T_BlockDesc()
	{
		m_eType			= EBTCount;
		m_iMarketId		= -1;
		m_StrBlockName	= L"";
		m_pMarket		= NULL;
		m_aMerchs.RemoveAll();

		m_iFieldAdd		= -1;
		m_iTypeAdd		= -1;
	}

	T_BlockDesc::T_BlockDesc(const T_BlockDesc& stBlockSrc)
	{
		m_eType			= stBlockSrc.m_eType;
		m_iMarketId		= stBlockSrc.m_iMarketId;
		m_StrBlockName	= stBlockSrc.m_StrBlockName;
		m_pMarket		= stBlockSrc.m_pMarket;
		m_aMerchs.Copy(stBlockSrc.m_aMerchs);
		m_iFieldAdd		= stBlockSrc.m_iFieldAdd;
		m_iTypeAdd		= stBlockSrc.m_iTypeAdd;
	}

	T_BlockDesc& T_BlockDesc::operator= (const T_BlockDesc& stBlockSrc)
	{
		if ( this == &stBlockSrc )
		{
			return *this;
		}

		m_eType			= stBlockSrc.m_eType;
		m_iMarketId		= stBlockSrc.m_iMarketId;
		m_StrBlockName	= stBlockSrc.m_StrBlockName;
		m_pMarket		= stBlockSrc.m_pMarket;
		m_aMerchs.Copy(stBlockSrc.m_aMerchs);
		m_iFieldAdd		= stBlockSrc.m_iFieldAdd;
		m_iTypeAdd		= stBlockSrc.m_iTypeAdd;

		return *this;
	}

	E_BlockType	m_eType;				// 0:������; 1:�û����; 2:ϵͳ���(�߼����)

	int32		m_iMarketId;			// �������MarketId
	CString		m_StrBlockName;			// ��������
	CMarket*	m_pMarket;				// ��������г�ָ��			(������ʱ��Ϊ NULL , ����Ϊ NULL)
	CArray<CMerch*, CMerch*> m_aMerchs;	// ����������Ʒ�б�			(������ʱΪ NULL , ����Ϊ�����ļ��ж�ȡ����������)

	int32		m_iFieldAdd;
	int32		m_iTypeAdd;	

} T_BlockDesc;

typedef struct T_UserBlockMerchColor
{
	CString		m_StrName;				// ��ѡ����Ʒ
	COLORREF	m_Color;				// ��Ҫ��ʾ����ɫ

}T_UserBlockMerchColor;

// ��ǩ����, �м�����ĺ�ʵ�����
typedef enum E_TabInfoType
{
	ETITCollection,					// ���������˵�
	ETITEntity,						// ����ֱ�Ӵ򿪶�Ӧ���, ����ǿյ�ʱ��, ��ѡ����Ի���
		
	ETITCount,
		
}E_TabType;

// ������Ϣ
typedef struct T_SimpleTabInfo
{
	T_SimpleTabInfo(int32 iD, int32 iIdDefault, int32 iTabIndex, const CString& StrShowName, E_TabInfoType eTabType, T_BlockDesc::E_BlockType eBlockType)
	{	
		m_iID			= iD;
		m_iIdDefault	= iIdDefault;
		m_iTabIndex		= iTabIndex;
		m_StrShowName	= StrShowName;
		m_eTabType		= eTabType;
		m_eBlockType	= eBlockType;
	}

	T_SimpleTabInfo()
	{
		m_iID			= 0;
		m_iIdDefault	= 0;
		m_iTabIndex		= 0;
		m_StrShowName	= _T("");
		m_eTabType		= ETITCount;
		m_eBlockType	= T_BlockDesc::EBTCount;
	}

	//
	int32					  m_iID;			// ID ��
	int32					  m_iIdDefault;		// ��������Ĭ����ʾ�İ���
	int32					  m_iTabIndex;		// ˳���
	E_TabInfoType			  m_eTabType;		// Tab ����
	CString					  m_StrShowName;	// Tab ��ʾ����
	T_BlockDesc::E_BlockType  m_eBlockType;		// ��������

}T_SimpleTabInfo;
typedef	vector<T_SimpleTabInfo>	SimpleTabInfoArray;

// tabInfo������ - ��ΪĬ����(id=0) ��Ʊ��(id=?) �ڻ���(id=?) �ۺ���(id=?)
#define SIMPLETABINFOCLASS_DEFAULTID	(-1)
struct T_SimpleTabInfoClass
{
	T_SimpleTabInfoClass()
	{
		iId = SIMPLETABINFOCLASS_DEFAULTID;
	}
	int32				iId;
	CString				StrName;
	SimpleTabInfoArray	aTabInfos;
};
typedef map<int, T_SimpleTabInfoClass>	SimpleTabClassMap;

// ÿ��ҳ�����Ϣ
typedef struct T_TabInfo
{
	T_TabInfo()
	{
		m_eTabType			= ETITCount;
		m_iXScrollPos		= 0;
		m_iDataIndexBegin	= 0;
		m_iDataIndexEnd		= 49;
		m_StrShowName		= L"";
		m_bAlignDataEnd		= false;
	}

	//
	E_TabInfoType	m_eTabType;			// ��ǩ����
	T_BlockDesc		m_Block;			// ���ҳ��İ����Ϣ
	CString			m_StrShowName;		// ��ʾ����
	int32			m_iXScrollPos;		// ���汾ҳ��� X ��������Ϣ
	int32			m_iDataIndexBegin;	// ���ݿ�ʼ������
	int32			m_iDataIndexEnd;	// ���ݽ���������

	bool32			m_bAlignDataEnd;	// ��Ҫ�������һ��

}T_TabInfo;


// ��ǩ����, �м�����ĺ�ʵ�����
typedef enum E_TabTopType
{
	ETAB_CHANGE_MARKET = 1,			// �л��г�
	ETAB_OPEN_CFM,					// �򿪰���
	ETAB_PLATE_MENU,				// ���˵�
	ETAB_CUSTOM_ORDER,				// �Զ���
	ETAB_CHANGE_REPORTHEAD,			// �ı䱨�۱��ͷ
	ETAB_UserBlock,					// ��ѡ��
	ETAB_RecentView,	            // ������
	ETABCount,

}E_TabTopType;

typedef enum E_ReportHeadType
{
	ERHT_CapitalFlow = 1,			// �ʽ�����
	ERHT_MainMasukura,              // ��������
	ERHT_FinancialData,				// ��������
	ERHT_Count,
};

// ÿ��������ǩ����Ϣ
typedef struct T_TopTabbar
{
	T_TopTabbar()
	{
		m_iBtnType			= ETABCount;
		m_StrShowName		= L"";
		m_iBtnID			= -1;
		m_iMarketID			= -1;
		m_iBlockid			= -1;
		m_iBlockType		= -1;
		m_strCfmName		= L"";
		m_eReportHeadType   = ERHT_Count;
		m_bSelected         = false;
		m_colNoarml			= RGB(0,0,0);
		m_colPress			= RGB(0,0,0);
		m_colHot			= RGB(0,0,0);
		m_colTextNoarml		= RGB(0,0,0);
		m_colTextPress		= RGB(0,0,0);
		m_colTextHot		= RGB(0,0,0);
	}

	//
	//T_BlockDesc		m_Block;		// ���ҳ��İ����Ϣ
	CString			 m_StrShowName;		// ��ʾ����
	int32			 m_iBtnID;			// ��ť��ID
	int32			 m_iBtnType;		// ��������
	int32			 m_iMarketID;		// �г�ID
	CString			 m_strCfmName;		// ��������
	E_ReportHeadType m_eReportHeadType;	// ���۱��ͷ���� 
	bool32           m_bSelected;		// �Ƿ�ѡ��
	int32			 m_iBlockid;		// ���ID
	int32			 m_iBlockType;		// �������

	COLORREF		 m_colNoarml;
	COLORREF		 m_colPress;
	COLORREF		 m_colHot;
	COLORREF		 m_colTextNoarml;
	COLORREF		 m_colTextPress;
	COLORREF		 m_colTextHot;
}T_TopTabbar;

// ÿ��ҳ�����Ϣ
typedef struct T_SelectBlockInfo
{
	T_SelectBlockInfo()
	{
		m_iMarketID = -1;
		m_StrBlockName = _T("");
		m_eType = T_BlockDesc::EBTCount;
	}
	//
	int32 		m_iMarketID;				// �г�ID
	CString		m_StrBlockName;				// ģ������
	T_BlockDesc::E_BlockType	m_eType;    // �������

}T_SelectBlockInfo;



class CIoViewReport : public CIoViewBase, public CGridCtrlCB, public CGuiTabWndCB, public CObserverUserBlock, public CMerchMarkChangeListener
{
// Construction 
public:
	CIoViewReport();
	virtual ~CIoViewReport();

	DECLARE_DYNCREATE(CIoViewReport)
	
public:
	// from CControlBase
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public:
	// from CObserverUserBlock ֪ͨ
	virtual void	OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType);	
	
public:
	// from CIoViewBase
	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();

	virtual bool32	FromXmlExtra(TiXmlElement *pElement) { return true; };
	virtual CString ToXmlExtra() { return CString(); }; 
	
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();

	virtual CMerch*	GetNextMerch(CMerch* pMerchNow, bool32 bPre);

	virtual bool32	GetStdMenuEnable(MSG* pMsg);

	virtual void	LockRedraw();
	virtual void	UnLockRedraw();

	virtual CMerch *GetMerchXml();
	virtual	void	GetMerchXmls(OUT MerchArray &aMerchs);
	virtual E_IoViewType GetIoViewType() { return EIVT_MultiMerch; }
	//	ָ�����е�cell����������
	void			ResetGridFont();
	//	ָ����ʾ��cell����������
	void			ResetVisibleGridFont();
public:	
	void			OnDblClick();

	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();
	virtual void	OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs);
	virtual void	OnVDataGridHeaderChanged(E_ReportType eReportType);

	virtual void	OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType);

	virtual void	OnIoViewColorChanged();
	virtual void	OnIoViewFontChanged();

	virtual	void	OnVDataReportInBlockUpdate(int32 iBlockId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount, const CArray<CMerch *, CMerch *> &aMerchs);

	virtual	bool32	IsPluginDataView() { return true; }
	virtual	void	OnVDataPluginResp(const CMmiCommBase *pResp);


	// �¹ɽӿ�
	virtual void	OnVDataQueryNewStockInfo(const char *pszRecvData);

	// ���
	virtual void	OnMerchMarkChanged(int32 iMarketId, const CString &StrMerchCode, const T_MarkData &MarkOldData);

	virtual void	OnEscBackFrameMerch(CMerch *pMerch);

	//
	virtual	CMerch*	GetDragMerch();
	virtual	bool32  BeValidDragArea(const CPoint& pt);

public:

	void			ReCreateTabWnd();
	CGuiTabWnd*		GetTabWnd() 				{ return &m_GuiTabWnd;}
	void			AddBlock(const T_BlockDesc& BlockDesc);
	void			OpenBlock(const T_BlockDesc& BlockDesc );
	void			OpenBlock(const T_BlockDesc& BlockDesc, E_MerchReportField eMerchReportField, bool32 bDescSort);
	
	bool32			IsBlockExist(const CString& StrBlockName);
	bool32			SetTabByBlockName(const CString& StrBlockName);
	bool32			SetTabByBlockType(T_BlockDesc::E_BlockType eType);
	
	// ��ѡ�����
 	void			DelUserBlockMerchByRow(int32 iRow);	
	void			AdjustUserMerchPosition(bool32 bUp);
	void			DragUserMerchPosition();		// ����϶��ı��û���Ʒ˳��λ��
	bool32			BeUserBlock();					// ��ǰ����Ƿ����û����
	void			SetDragMerchBegin(bool32 bClear=false);	// ���ÿ�ʼ�϶�����Ʒ
	void			ExportUserBlockToExcel(CString StrDefaultFileName,CString StrTitle1,CString StrTitle2,CString StrTitle3);

	//
	void			SetGridSelByMerch(const CMerch* pMerch);
	CMerch*			GetGridSelMerch();

	bool32			BeSpecial();

	void			StartAutoPage(bool32 bPrompt = false);		// ��ʼ
	void			StopAutoPage();			// ����
	bool32			IsAutoPageStarted();	// �Ƿ�ʼ
	static void		SetAutoPageTimerPeriod(int32 iPeriodSecond);
	static int32    GetAutoPageTimerPeriod();
	// from CView	
public:
	// ���ݱ��۱��е���Ʒ˳��õ���Ʒ�л���ǰ����Ʒ
	bool32			GetPreMerch(IN CMerch *pMerch, OUT CMerch *&pMerchPre);
	bool32			GetAftMerch(IN CMerch *pMerch, OUT CMerch *&pMerchAfter);

	static	const SimpleTabClassMap &GetSimpleTabClassConfig();;

	// from CGuiTabWndCB
protected:
	void			OnRButtonDown2(CPoint pt, int32 iTab);
	bool32			OnLButtonDown2(CPoint pt, int32 iTab);
	bool32			BlindLButtonBeforeRButtonDown(int32 iTab);

///////////////////////////////////////////////////////////////////////
public:
	void			SetTab(int32 iCurTab);

///////////////////////////////////////////////////////////////////////

protected:
	void			OnEsc();
	void			TabChange();
	bool32			ValidBlock(T_BlockDesc& Block);				// ��֤��������Ϣ
	void			ValidateTableInfo(int32 iIndex = -1);		// ��֤ÿ�� Tab ҳ����Ϣ
	void			SetEditAbleCol();							// ֻ����ѡ�ɵ�ʱ��,�������������пɱ༭
	void			SaveColoumWidth();
	bool32			TabIsValid(int32& iTab, T_BlockDesc::E_BlockType& eType);
	void			SetTabParams(int32 iTab = -1);	

	void			ReSetGridCellDataByIndex();
	void			ReSetGridCellDataByMerchs(const CArray<CMerch*, CMerch*>& aMerchs); 

	void			ReSetGridContent();

	void			SetRowHeightAccordingFont();							
	void			SetColWidthAccordingFont();

	bool32			SetHeadInfomationList();
	void			UpdateTableContent( BOOL bUpdateAllVisible, IN CMerch *pMerch, bool32 bBlink);

	bool32			GetCurrentDataIndex(int32& iIndexBegin, int32& iIndexEnd);
	void			UpdateLocalBlock(int32 iIndex, const T_BlockDesc& Block);
	void			UpdateLocalXScrollPos(int32 iIndex, int32 iPos);
	void			UpdateLocalDataIndex(int32 iIndex, int32 iBegin, int32 iEnd);

	void			MenuBlockChange();
	void			MenuBlockDel();
	void			MenuBlockInsert();
	void			MenuBlockSetting();
	void			MenuBlockCancel();
	
	void			OnIoViewReportGridLine();
	void			OnIoViewReportMenuDelOwnSel();	
	void			OnIoViewReportMenuDelALLOwnSel();					// ɾ��������Ʒ	
	void			OnIoViewReportMenuOwnSelUp();
	void			OnIoViewReportMenuOwnSelDown();

	void			OnMouseWheel(short zDelta, bool32 bMainGrid = FALSE);
	virtual void	OnFixedRowClickCB(CCellID& cell);
	virtual void	OnFixedColumnClickCB(CCellID& cell);
    virtual void	OnHScrollEnd();
    virtual void	OnVScrollEnd();
	virtual void	OnCtrlMove( int32 x, int32 y );
	virtual bool32	OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew );
	virtual void	OnGridMouseMove(CPoint pt);
	virtual bool32	BeBlindSelect();

	virtual	void	UpdatePushMerchs();		// ������Ҫ���һЩ���������ݽ�ȥ
	bool32			RequestViewDataSort(bool32 bSendRequest = true);
	bool32			RequestViewDataSortPlugIn(bool32 bSendRequest = true);
	void			RequestViewDataCurrentVisibleRow();
	void			RequestViewDataSortAsync();
	void			RequestViewDataSortTimerBegin();
	void			RequestViewDataSortTimerEnd();
	void			RequestViewDataCurrentVisibleRowAsync();

	// ...fangz 1213 �¼�
	const T_SimpleTabInfo* GetSimpleTabInfo(int32 iTabIndex);
	
	// ���������ͳ�ʼ�������е�Tabs
	void			InitialTabInfos();	// ��ʼ���Լ���tabinfo

	void			ChangeSimpleTabClass(int32 iNewClass);	// �����ǰ���۱�����

	void			InitialSimpleTabInfo();
	static bool32		LoadSimpleTabInfoFromXml(OUT SimpleTabClassMap &mapTabClass);
	static bool32       LoadSimpleTabInfoFromCommCfg(OUT SimpleTabClassMap &mapTabClass);
	static void			SaveSimpleTabInfoToXml();

	

	void			DoReportSizeChangeContent();	// ��С�ı�ʱ���������øô�С�ĵ�������

	void			CalcMaxGridRowCount();		// ���ݵ�ǰ������grid�߶ȣ����������ܵ���ʾ����
	void			ResetTabBeginStartIndex(int32 iTab);	// ����tab�Ŀ�ʼ������������[0-m]

	void			AutoSizeColFromXml();

	bool32			IsInCustomGrid();
	void			InitialCustomMerch();
	void			ReSetCustomGridContent();
	void			UpdateCustomTableContent( BOOL bUpdateAllVisible, IN CMerch *pMerch, bool32 bBlink);
	void			CustomGridMouseWheel(short zDelta);
	bool32			SetCustomHeadInfomationList();
	void			ReSetCustomGridCellDataByIndex(int32 iTab);
	void			ReSetCustomGridCellDataByMerchs(const CArray<CMerch*, CMerch*>& aMerchs); 
	void			UpdateLocalDataIndex2(int32 iBegin, int32 iEnd);
	void			UpdateLocalXScrollPos2(int32 iPos);

	void			ReCalcGridSizeColFromXml();
	void			ReCalcCustomGridSizeColFromXml();

	void			SetPosForScroll(int32 Keyboard);
	void			JudgeScrollVerVisbile(bool bOnSize = false);	// �жϴ�ֱ�������Ƿ���ʾ(bOnSize��OnSize()�е���)
	bool			ShowGridIndex(int32 index);						// ָ����indexΪ��һ����ʾ
	void			ResetSort();	// ����б��������Ϣ
	void			CreateBtnStock();								//����������ѡ�ɰ�ť	
	void			OnBtnResponseEvent(UINT nButtonId);			//��ѡ�ɰ�����Ӧ�¼�
	void            ChangeGridSize(bool32 bHasData = true);
	void			UserBlockPagejump();									// ��ѡ��ҳ����ת



	void			ReCalcSize();											// ����λ��


	bool32			FromXmlTopBtn( TiXmlElement *pTiXmlElement );			//  ��ȡ�Զ��嶥����ť
	void			InitTopBtn();											//  ��ʼ���Զ��嶥����ť
	void			TopTabBarHitTest(CPoint point);							//  �������
	void			OnTopTabBarResponseEvent(UINT nButtonId);				//  �¼���Ӧ
	void			OnTopTabBarDisPlateMenu(UINT nButtonId);				//  �˵���ʾ
private:
	map<int32, T_TopTabbar>		m_mapTopTabbarGroup;								// ������ǩҳ����
	map<int32, CNCButton>		m_mapTopTabbarBtn;							// ������ǩ��ť

	BOOL						m_bIsShowTopTabbar;								// �Ƿ���ʾ������ǩҳ
	CRect						m_rcTopBtn;
	T_TopTabbar					m_CurTopTabbar;									// ��ǰ�����Ϣ		
	T_SelectBlockInfo			m_CurSelectBlockInfo;							// ��ǰѡ������Ϣ

public:
	void            ReSetFinanceFlag();
	void			UseSpecialRecordFont();									//	ʹ���б�ר������
	void            SetIsShowTab(bool32 bShow)  { m_bShowTab = bShow; }

	void            SetReportHeadType(const E_ReportHeadType &eType);    // �ı䱨�۱�ͷ
	bool32          IsShowUserBlock() { return m_bIsUserBlock; }
protected:	        
	int32							m_iSortColumn;

	CMmiReqReport					m_MmiRequestSys;			// ��ͨ�ֶε���������
	CMmiReqBlockReport				m_MmiBlockRequestSys;		// �����������
	CMmiReqMerchSort				m_MmiReqPlugInSort;			// ����ֶε���������

	// ������
	CGridCtrlSys					m_GridCtrl;
	CCellID							m_CellRButtonDown;	
	bool32							m_bShowGridLine;
	CXScrollBar						m_XSBVertical;				// ���ڿ��ƴ�ֱ��������
	CXScrollBar						m_XSBVert;
	CXScrollBar						m_XSBHorz;
	
	// Tab ���
	CGuiTabWnd						m_GuiTabWnd;
	bool32							m_bLockSelChange;

	E_ReportHeadType                m_eReportHeadType;				// ���۱��ͷ����

	int32							m_iCurTab;
	int32							m_iIsReadCurTab;
	int32							m_iCurTabTmp;
	int32							m_iPreTab;

	CSize							m_SizeLast;
	int32							m_iMaxGridRowCount;		// ��ǰ�����ܵĿ�������
	
	// ÿ��Tab �ı�ͷ��Ϣ (�̶���, ��ͷ����, ������λ��)
	int32							m_iFixCol;
	CArray<T_HeadInfo,T_HeadInfo>	m_ReportHeadInfoList;

	//
	bool32							m_bRequestViewSort;
	CGmtTime						m_TimeLast;
	CString							m_StrAsyncExcelFileName;

	E_ReportType					m_eMarketReportType;
	
	// ��ѡ��������
	CStringArray					m_aUserBlockNames;

	bool32							m_bEnableUserBlockChangeResp;	// ������ѡ�ɱ����Ӧ��ĳЩ����²�����Ӧ

	// ����ѡ�ɱ��۱�־ - ��ʱ
	bool32							m_bIsStockSelectedReport;

	// ����ѡ������Ҫ�����������	- ������Ҫ�ϲ����ݣ������Ƶ����������д���
	//CMmiReqMerchIndex				m_mmiReqMerchIndex;			// ѡ�ɱ�����������
	typedef	 map<CMerch *, T_RespMerchIndex>	MerchIndexMap;	
	MerchIndexMap					m_mapMerchIndex;			// �յ���ѡ����������

	typedef map<CMerch *, T_RespMainMasukura>   MainMasukura;	
	MainMasukura                    m_mapMainMasukura;			// �յ���������������

	// ÿ��ҳ���������Ϣ
	CArray<T_TabInfo, T_TabInfo&>	m_aTabInfomations;

	int32							m_iAutoPageTimerPeriod;				// ÿ�����۱������Զ���ҳ���, 
	int32							m_iAutoPageTimerCount;				// ÿ���Ӵ���һ�Σ���0�򴥷���ҳ����������Լ���ȫ�ֲ�ͬ�����������ʱ��
	bool32							m_bAutoPageDirection;				// ����true������
	static int32					s_iAutoPageTimerPeriodSecond;		// ���б��۱��Զ���ҳ�Ķ�ʱ���

//	bool32							m_bInitedFromXml;					// �Ƿ��Ѿ�������ʼ���ˣ�������create->��ʾǰ����������ͳһ�ڳ�ʼ����ɺ�setTab
	
	bool32							m_bAutoSizeColFromXml;				// �����Ƿ���Ҫ����Ӧ�п�
	bool32							m_bSuccessAutoSizeColXml;			// ����ʱ�������, ��һ����Ч��, �Ӹ���־�ж�

	T_SimpleTabInfoClass			m_SimpleTabInfoClass;		// ����������۱�tab����
	static	SimpleTabClassMap		sm_mapSimpleTabConfig;		// ����
	
	//
	CGridCtrlSys					m_CustomGridCtrl;			// �û��Զ�����Ʒ����ʾ
	CRect							m_RctCustom;				// �û��Զ�����Ʒ����ʾ����
	CRect							m_RctSperator;				// �������ķָ�����
	CXScrollBar						m_XSBVertCustom;
	CXScrollBar						m_XSBHorzCustom;
	int32							m_iMaxCustomGridRowCount;	// customGrid��ǰ�����ܵĿ�������
	CNCButton						m_BtnMid;					// �ײ�ָ�깤����
	CNCButton						m_BtnAddStock;				// �ײ�ָ�������ѡ��
	CNCButton						m_BtnDelStock;				// �ײ�ָ��ɾ����ѡ��
	CNCButton						m_BtnCleanStock;			// �ײ�ָ�������ѡ��
	CNCButton						m_BtnImportStock;			// �ײ�ָ�굼����ѡ��
	CNCButton						m_BtnExportStock;			// �ײ�ָ�굼����ѡ��
	CArray<CNCButton,CNCButton>		m_BtnStockList;				// ��ѡ�ɹ��ܰ�ť�б�				
	bool32							m_bShowCustomGrid;			// ��ʾ�û��Զ�����Ʒ
	bool32							m_bShowUserBlock;			// ��ʾ�û���ѡ��ģʽ
	bool32							m_bIsUserBlock;				// �Ƿ�Ϊ��ѡģ�飬�˱�־��m_bShowUserBlock���ʹ��

	bool32                          m_bShowTextTip;				// ѡ��������������ʾ
	bool32							m_bCustomGrid;				// 
	int32							m_iMarketID;				// Ĭ����ʾ���г�id

	T_BlockDesc						m_blockCustom;
	T_TabInfo						m_tabCustom;

	bool32							m_bIsShowGridVertScorll;	// �����Ƿ���ʾ��ֱ������
	bool32							m_bShowTab;				// �Ƿ���ʾtab��ǩ
	bool32                          m_bShowTabByTabbar;		// ֻ��m_bShowTab=falseʱ��Ч
	CRect							m_RectVert;				// ��ֱ��������λ��
	CRect							m_RectGrid;				// �������λ��

	int32							m_sortType;				// ���ڱ�ʶ����б�ͷʱ����������

	bool32							m_bDragZxMerch;
	bool32							m_bDragMerchLBDown;
	CMerch							*m_pDragMerchBegin;		// ��ʼ�϶�����Ʒ
	int32							m_iRowDragBegin;		// ��ʼ�϶�����		
	int32							m_iDragGridType;		// �϶��ı������, ͬһ�������϶�(0,��ѡ�� 1��������Ʒ)

public:
	bool32							m_bLockReport;			// ���۱�������Ͳ���ͨ���������л��г�
	int32                           m_iSelStockTab;			// ��ǰ��۱�����ѡ�ɱ�ǩλ��
	bool32                          m_bShowMoreColumn;         // �Ƿ���ʾ����������
friend class CMainFrame;
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewReport)
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewReport)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	// afx_msg LRESULT OnMsgHotKey(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLayOutAdjust();	
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnAutoPage(UINT nId);
	afx_msg void OnDoInitFromXml();
	//}}AFX_MSG
	afx_msg void OnMenuAddOwnSel(UINT nID);
	afx_msg void OnMenu( UINT nID );
	afx_msg void OnIoViewReportMenuBlockMoreLBtnDown( UINT nID );
	afx_msg void OnIoViewReportMenuBlockMoreRBtnDown( UINT nID );
	afx_msg void OnIoViewReportMenuTabCollection( UINT nID );
	afx_msg void OnIoViewReportMenuTabTopCollection( UINT nID );
	afx_msg void OnMenuExport ( UINT nID );
	afx_msg void OnMenuIoViewReport( UINT nID );	
	afx_msg void OnGridLButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg LRESULT OnMessageTitleButton(WPARAM wParam,LPARAM lParam);
	//linhc 20100909���
    afx_msg void OnGridSelRowChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	//--- wangyongxue 2016/0829
	afx_msg LRESULT OnMessageRisePercent(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnMessageFinance(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
friend class CGridCtrlSortCB;
virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_REPORT_H_
