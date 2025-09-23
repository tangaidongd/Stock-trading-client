// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__234DC6B0_FD03_4692_B156_E8947A1EB907__INCLUDED_)
#define AFX_MAINFRM_H__234DC6B0_FD03_4692_B156_E8947A1EB907__INCLUDED_
 
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MPIChildFrame.h"
#include "StatusBarEx.h"
#include "CommentStatusBar.h"
#include "IoViewReport.h"
#include "IoViewTrend.h"
#include "IoViewNews.h"
#include "mdiclientex.h"
#include "hotkey.h"

#include "DlgIm.h"
#include "BlockManager.h"
#include "OptimizeServer.h"
//
#include "DlgAdvertise.h"
#include "TradeContainerWnd.h"
#include "NewTBWnd.h"
#include "TBCaptionWnd.h"
#include "DlgJPNews.h"
#include "NewToolBar.h"
#include "DlgWebF10.h"
#include "DlgAccount.h"
#include "DlgWebContent.h"
#include "DlgDefaultUserRegister.h"
#include "DlgPushMessage.h"
#include "MenuBar.h"
#include "DlgNewComment.h"
#include "LeftToolBar.h"
#include "ShortCutMenuToolBar.h"
#include "../../../JsonCpp/json.h"
#include "client_AnalysisWeb.h"

#define NEWWINDOWOFFSETX			24
#define NEWWINDOWOFFSETY			40
#define NEWWINDOWWIDTH				800
#define NEWWINDOWHEIGHT				600

#define USERBLOCKMAXNUMS			10
#define RECENTMERCHMAXNUMS			10

#define RAND_NUMS					500						// ����������и���

class CDlgBelongBlock;
class CDlgTrace;
class CAdjustViewLayoutDlg;
class CKeyBoardDlg;
class CDlgSpirite;
class CDlgNetWork;
class CDlgSysUpdate;
class CDlgBrokerWatch;
class CDlgAlarm;
class CDlgRecentMerch;
class CDlgIndexPrompt;
class CDlgWait;
class CDlgIm;  // 2013-10-28 add by cym 
class CDlgIndexChooseStock;
class CDlgLoginNotify;
class CDlgNetFlow;
class CHotKey;
class CIoViewBase;
class CGGTongView;
class CKLine;  
// class CDlgF10;
class CRecordDataCenter;

struct T_PhaseOpenBlockParam;
class CArbitrage;

class CMyControlBar;
class CMyContainerBar;
class CDlgTrade;

class CTradeContainerWnd;

enum E_CONN_NOTIFY
{
	en_connect_error,	// ���ӳ���
	en_connect_success, // ���ӳɹ�
};

// ��Ѷ���ͣ�����Ѷ����Ϣ���ͣ�
enum E_Info_Type
{
	en_monitor_msg=1,	// Ԥ����Ϣ
	en_system_msg,		// ϵͳ��Ϣ
	en_optional_change,	// ��ѡ�춯
	en_strategy_trade,	// ���Խ���
	en_select_stock,    // ����ѡ��
};

// ��Ϣ���ĵ�Э������
enum E_Fun_Type
{
	en_heartbeat = 1,			// ����
	en_set_device_id,			// �����豸��
	en_query_info_type_list,	// ��ѯ��Ѷ�����б�
	en_query_msg_list,			// ��ѯ��Ϣ�б�
	en_query_info_content,		// ��ѯ��Ѷ����
	en_info_push,				// ������Ѷ
	en_add_info					// �����Ѷ
};

//////////////////////////////////////////////////////////////////////
// �����ҳ������ӵ����Ի���
typedef struct T_ShowWebDlg
{	
	T_ShowWebDlg()
	{
		StrUrl   = L"";
		StrWidth = L"0";
		StrHeight= L"0";
	}

	CString		   StrUrl;		// url
	CString	       StrWidth;	// ��
	CString		   StrHeight;	// ��
}T_ShowWebDlg;

// ��ҳҪ��ָ������Ĳ���
typedef struct T_WebOpenCfm
{	
	T_WebOpenCfm()
	{
		StrCfmName = L"";
		iID = -1;
		StrFlag= L"";
	}

	CString		   StrCfmName;	// ��������
	int32	       iID;			// id
	CString		   StrFlag;		// ��־
}T_WebOpenCfm;


// ������ҵ���ݽṹ
typedef struct T_IndustryData
{	
	T_IndustryData()
	{
		iMarketId = -1;
		strCode = "";
		strCnName= "";
	}

	int32		   iMarketId;	// ��������
	string	       strCode;			// id
	string		   strCnName;		// ��־
	int32		   iMarketVlue;		// ������ֵ
}T_IndustryData;



// ��ҳ�򿪷�ʱҳ�����
typedef struct T_WebOpenTrend
{	
	T_WebOpenTrend()
	{
		iMarketId = -1;
		strCode  = L"";
		strCnName = L"";	
	}

	int32	        iMarketId;			// �г�����
	CString			strCode;			// ��Ʒ����
	CString			strCnName;			// ��Ʒ����
}T_WebOpenTrend;


typedef struct T_BlockMenuInfo
{	
    CString		   StrBlockMenuNames;	
	int32	       iBlockMenuIDs;	
}T_BlockMenuInfo;

typedef struct T_NewsContentInfo
{
public:

	int32		m_iIndexID;			// ��Ѷid
	CString		m_StrTitle;			// �������
	CString		m_StrContent;		// ��������
	CString		m_StrTimeUpdate;	// ����ʱ��

	CefWindowInfo* m_pWindowInfo;

	T_NewsContentInfo()
	{

		m_iIndexID   = -1;

		m_StrTitle   = L"";
		m_StrContent = L"";
		m_StrTimeUpdate= L"";

		m_pWindowInfo = NULL;
	}

}T_NewsContentInfo;
/////////////////////////////////////////////////////////////////////
struct T_RecentMerch
{
public:
	T_RecentMerch() { m_pMerch = NULL; }

public:
	CMerch			*m_pMerch;
};

///////////////////////////////////////////////////////////////////////
struct T_CompareMerchInfo
{	
public:
	T_CompareMerchInfo()
	{
		m_iGroupId	= 0;
		m_pMerch	= NULL;
	}

public:
	int32			m_iGroupId;
	CMerch			*m_pMerch;
};

struct T_MenuItemData
{
	int32 ID;
	CString strText;
	bool bShowArrow;
	Image* pImgData;

	T_MenuItemData()
	{
		ID = -10000;
		strText = L"";
		bShowArrow = false;
		pImgData = NULL;
	}

	T_MenuItemData(int32 id, CString Tex, bool bShow, Image* pImg)
	{
		ID = id;
		strText = Tex;
		bShowArrow = bShow;
		pImgData = pImg;
	}
};

typedef vector<T_MenuItemData> VecMenuContainer;
//////////////////////////////////////////////////////////////////////

///////////////////////����ѡ�ɽṹ��
struct T_SmartStockPickItem
{
	CString m_strModeName;
	int32   m_iModeId;
	CString m_strModeDetail;
	CString m_strmodeCode;
	int  m_ihasSmartStock;

	T_SmartStockPickItem()
	{
		m_strModeName = L"";
		m_iModeId  = -1;
		m_strmodeCode = L"";
		m_ihasSmartStock = 0;
		m_strModeDetail    = L"";
	}
	T_SmartStockPickItem(int32 iID, CString strModeName, CString strModeDetail=L"", CString strmodeCode = L"", int32 ihasSmartStock =0)
	{
		m_strModeName = strModeName;
		m_iModeId  = iID;
		m_strmodeCode = strmodeCode;
		m_ihasSmartStock = ihasSmartStock;	
		m_strModeDetail    = strModeDetail;
	}
};


const int32 KMainFrameDefaultLeft	= 150;
const int32 KMainFrameDefaultRight	= 950;
const int32 KMainFrameDefaultTop	= 50;
const int32 KMainFrameDefaultBottom	= 650;

extern WORD g_awToolBarIconIDs[];

enum E_UserConnectCmdFlag
{
	EUCCF_ConnectDataServer = 1,		// �����������������, û�������ֹ
	EUCCF_ConnectNewsServer = 2,		// ��Ѷ��������������û�������ֹ
};
 
class CMainFrame : public CNewMDIFrameWnd, public CObserverUserBlock, public COptimizeServerNotify
{	
friend class CStatusBarEx;

friend class CCommentStatusBar;

DECLARE_DYNAMIC(CMainFrame)
public:	
	enum E_CurrentShowState
	{
		ECSSNormal = 0,
			
		ECSSForcusMax,
		ECSSForcusMin,
		ECSSForcusClose,
		
		ECSSPressMax,
		ECSSPressMin,
		ECSSPressClose,
		//
		ECSSCount
			
	};

	enum E_RenameType
	{
		ERTXML = 1,	// δ��¼
		ERTWSP,
		ERTCFM,
		ERTVMG,
		ERTCOUNTS,
	};

	enum E_CpsAlgorithmType // �������㷨����
	{
		ECATNONE = 0,		
		ECATSXBD,			// ʱ�򲨶��㷨
		ECATCOUNTS
	};

	enum E_MenuItemType
	{

		EMIT_NONE = 0,

		// һ���˵�����
		EMIT_FIR_SYSTEM = 1,
		EMIT_FIR_SEL_STOCK,
		EMIT_FIR_TRADE,
		EMIT_FIR_HELP,

		// �����˵�����
		EMIT_SEC_MODULESET = 10,
	};

public:
	CMainFrame();
	virtual ~CMainFrame();

#ifdef _DEBUG
	virtual void	AssertValid() const;
	virtual void	Dump(CDumpContext& dc) const;
#endif

public:
	//	��ʼ����¼�����ض���
	void			Init();
	//	����ʼ������
	void			Uninit();
	//	��ʱ������¼
	void			ElapseLogin(UINT uSecond);
	//	������¼
	void			Login(bool32 bCloseExit, bool32 bExitQuery);
	// ��ѡ������
	// from CObserverUserBlock
	
	//
	virtual void	OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType);
	
	//
	void			AppendUserBlockMenu(CNewMenu * pMenu, bool32 bAddToSave = true);

public:
	// ������������ʱ��, �������	
	// from COptimizeServerNotify

	//
	virtual void	OnFinishSortServer();

	void	SetUserConnectCmdFlag(DWORD dwFlagRemove, DWORD dwFlagAdd, DWORD dwMask = 0xffffffff);		// �����û����õķ���������״̬��־
	DWORD   GetUserConnectCmdFlag() const { return m_dwUserConnectCmdFlag; }

public: 
	static CString	GetDefaultXML();
	bool32			FromXml(const char * pKcXml );
	CString			ToXml(const char * KpcFileName, CString StrFileTitle = L"", CString StrVersion = L"");
	static CString	GetWspFileShowName(const char* KpcFilePath);
	static CString	GetWspFileVersion(const char* KpcFilePath);
	static bool32   IsWspReadOnly(const char *KpcFilePath);
	static bool32   SetWspFileReadOnly(const char *KpcFilePath, bool32 bReadOnly);

public:	
	void			ProcessHotkey ( MSG* pMsg );

	void			SetHotkeyTarget ( CWnd* pParent = NULL, E_HotKeyType eHKT = EHKTCount);		// EHKTCountΪ�������
	void			ShowHotkeyDlg(MSG *pMsg = NULL, CWnd *pParent = NULL, E_HotKeyType eHKT = EHKTCount);	// ֱ����ʾhotkeydlg
	
	void			SetForceFixHotkeyTarget(bool32 bForceFix) { m_bForceFixKeyBoard = bForceFix; }
	bool32			GetForceFixHotkeyTarget()				  { return m_bForceFixKeyBoard; }
	
	CString			ReNameFile(CString StrOldName,E_RenameType eReNameType);   
	CString         GetMoudleAppName();
	void			NewWorkSpace();
	CString			CreatDefaultWorkSpace();

	// �½����ڣ�����ָ������ҳ��(���ָ���˻�), �����½��Ĵ���
	CMPIChildFrame *OpenChildFrame(const CString &StrXmlCfmName, bool32 bHideNewCfm=false);
	//void			SaveChildFrame(LPCTSTR lpszCfmPath);	// ���浱ǰ������ָ��

	void			ClearCurrentWorkSpace();
	void			CloseWorkSpace();
	bool32			CanClose();
	void			ClearLogin();

	void			SaveWorkSpace(CString StrXmlName = L"", CString StrVersion = L"");

	CString			GetLastWspFile( CString StrUserName = L"", bool32 bNewUser = false);	
	static CString	GetWorkSpaceDir();

	void			LoadPicMenu();
	void			AppendPicMenu();

	void			InitSpecialMenu();

	bool32			BeMaxSize();
	CRect			CalcActiveChildSize();


public:
	void			NewEmptyReportChildFrame();			 // ���˵���,�½�һ�����۱�.
	void			NewDefaultChildFrame(bool32 bBlank);
	void			NewDefaultIoViewChildFrame(int32 iIoViewGroupId);

public:
	void			GetBlockMenuInfo(CArray<T_BlockMenuInfo, T_BlockMenuInfo&>& aBlockMenuInfo);

public:
	void			SetEmptyGGTongView(CGGTongView * pView);			// �հ�GGTongView ʱ,�����Ҽ��¼�,�õ����View ��ָ��
	CGGTongView		*GetEmptyGGTongView();								// ChildFrameɾ��viewʱ��������Ҫ�ж�EmptyGGTongView�Ƿ���Ҫ����
	void			SetActiveGGTongViewXml(CGGTongView * pView);		// ���ü�����ͼ
	CGGTongView*	GetActiveGGTongViewXml();							// �õ�������ͼ

public:	
	CIoViewBase *	FindActiveIoView();
	CView *			FindGGTongView();
	bool32			OnProcessF7(CGGTongView* pGGTonView = NULL);
	bool32			OnProcessCFF7(CMPIChildFrame *pChildFrame = NULL);
	bool32			IsFullScreen(CGGTongView* pGGTonView = NULL);		// ָ����ͼ��ǰ���ͼ�Ƿ���ȫ��
	static CGGTongView * FindGGTongView(CWnd *pWnd, CGGTongView *pGGTongViewDst = NULL);
	// ���ĳ�������е���Ʒ��Ϣ�� ���½�һ����ͼ��ʱ���õ���
	bool32			GetMerchInSpecifyIoViewGroup(int32 iIoViewGroupId, IN CMerch *&pMerch);
	
	CIoViewBase *	FindNotReportSameGroupIdVisibleIoview(CIoViewBase *pActiveIoView, bool32 bOnlyInSameChildFrame);
	// ����pGroupIoView��GroupId����ָ��pic id������Ҫ���ioView�����ָ��Ҫ��SameGroup����SameChild����bNotInSameManager�������Ҫ���ṩ��Ч��GroupIoView��bNotInSameManagerΪtrue����Ҫ������ͬһ��manager�У�Ϊfalse����������
	CIoViewBase *	FindIoViewByPicMenuId(UINT nPicID, CIoViewBase *pGroupIoView, bool32 bInSameGroup, bool32 bMustVisible, bool32 bOnlyInSameChildFrame, bool32 bCanNotInSameManager, bool32 bEnableSonObject = false);
	CIoViewBase * GetIoViewByPicMenuIdAndActiveIoView(UINT nPicId, bool32 bEnableCreate, bool32 bInSameGroup, bool32 bMustVisible, bool32 bOnlyInSameChildFrame, bool32 bCanNotInSameManager, bool32 bEnableSonObject = false);
	CIoViewBase *	FindIoViewInFrame(UINT nPicID, CMDIChildWnd *pChildFrame, bool32 bMustVisible = false, bool32 bEnableInherit=true, int32 iGroupId=-1);	// ����λ�ڸ�frame�µ��ض���ͼ
	CIoViewBase *	FindIoViewInFrame(UINT nPicID, int32 iGroupId);	// ֻ����ͬ���µ��ض���ͼ
	BOOL			FindIoViewInFrame(OUT vector<CIoViewBase*>& outVt,  UINT nPicID, CMDIChildWnd *pChildFrame, bool32 bMustVisible = false, bool32 bEnableInherit=true, int32 iGroupId=-1 );	//add by weng.cx


	CIoViewReport *	FindIoViewReport(bool32 bInTopChildFrame);
    CIoViewNews   * FindIoViewNews(bool32 bInTopChildFram);
	CIoViewDuoGuTongLie* FindIoViewDuoGuTongLie(bool32 bInTopChildFram);
	CIoViewBase *	CreateIoViewByPicMenuID(UINT nID, bool32 bShowNow, CIoViewManager* pIoViewManagerIn = NULL);
	void			OnIoViewFaceChange(bool32 bFont,CIoViewBase * pIoViewSrc);
	
public:
	void			OnHotKeyMerch(int32 iMarketId, const CString &StrMerchCode);
	void			OnHotKeyMerch(const CString &StrMerchCode);
	void			OnHotKeyShortCut(const CString &StrKey);
	void			OnHotKeyBlock(const CString &StrBlockName, int32 iBlockType);
	void			OnHotKeyZXG();
	void			OnHotKeyLogicBlock(const CHotKey &hotKey);
	void			OnHotKeyIndex(const CHotKey &hotKey);

	bool32			PreTransGlobalHotKey(int32 iVirtualKeyCode);
	void			OnSpecifyMarketAndFieldReport(int32 iMarketId, int32 iMerchReportField, bool32 bDescSort);
	void			OnSpecifyBlockIdAndFieldReport(int32 iBlockId, int32 iMerchReportField, bool32 bDescSort);	// ��ָ���߼����
	//linhc 20100911 �޸ķ���ֵ
	CIoViewReport*	OnSpecifyBlock(T_BlockDesc &BlockDesc, bool32 bOpenExist = true);
	void			OpenSpecifyMarketOrBlock(int32 iMarketID);

	void			OpenSpecialSelectReport();		// ������ѡ�ɱ��۱�

	void			OnShowTimeSaleDetail();	// �鿴�ֱʳɽ���ϸ
	bool32			OnShowF7IoViewInChart(UINT nF7PicId, UINT nChartPicId =0);		// ��ָ��ͼ��ҳ���в鿴F7����ָ��ҵ����ͼ

	void			OnShowMerchInChart(int32 iMarketId, const CString &StrMerchCode, UINT nPicId=0);	// ��ʾָ��chartҳ��, 0��Ĭ�Ϲ���
	void			OnShowMerchInChart(CMerch *pMerch, CIoViewBase *pIoViewSrcOrg);		// ��ɱ��۱����ͼ��dbclick����, ��¼��ת
	void			OnShowMerchInNextChart(CIoViewBase *pIoViewSrcOrg);	// ����F5���л�����һ��ͼ��ҳ��, src����Ϊ��NULL, ��¼��ת
	CMPIChildFrame *FindChartIoViewFrame(bool32 bCreateIfNotExist = false, bool32 bHideNewCfm=false);	// ���Ұ����з�ʱ����K�ߵ�ҳ��
	CMPIChildFrame *FindAimIoViewFrame(int32 iPicIdToFind, bool32 bCreateIfNotExist = false, bool32 bHideNewFrame=false);	// ����ָ������pic��ҳ�洰��(Ŀǰ��֧��k���ʱ)
	CIoViewBase		*FindChartIoViewInSameFrame(CIoViewBase *pIoViewInSameFrameSrc);	// ����ָͬ��ҵ����ͼͬ����ͬ�����ͼ����ͼ
	CIoViewBase		*FindChartIoViewInFrame(CMPIChildFrame *pChildFrame, CIoViewBase *pIoViewSrc=NULL);	// ҳ���в���ͼ��
	CIoViewBase		*FindActiveIoViewInFrame(CMPIChildFrame *pChildFrame);	// ָ��ҳ�漤�����ͼ
	void			OnEscBackFrame();	// Esc��תҳ��
	void			OnBackToHome();		// ��ת����ҳ
	void			OnBackToUpCfm();	// ���ص���һ������ҳ��
	void			OnCloseAllNotReserveCfm();	// �ر����зǱ���ҳ��
	void			OnChangeOpenedCfm(int32 iPicId);		// ����ҳ���Ƿ�Ҫ���¼��뷵���б���
	void			DoIoViewEscBackFrameInActiveFrame(CMPIChildFrame *pFrame, CMerch *pMerch);	// ����ָ��ҳ���������ͼ��OnEscBackFrame�ӿ�
	bool32			AddToEscBackFrame(CMPIChildFrame *pChildFrame);	// ������ת�б�
	void			EmptyEscBackFrame();	// �����ת��¼
	bool32			LoadSystemDefaultCfm(int32 iPicId, CMerch *pMerchToChange = NULL, bool32 bMustSeeChart = false);	// ���ػ�����ǰϵͳĬ�ϰ���, merch ΪNULL�����Ե�ǰ�����, ��¼��ת
	CMPIChildFrame *LoadSystemDefaultCfmRetFrame(int32 iPicId, CMerch *pMerchToChange = NULL, bool32 bMustSeeChart = false);	// ���ػ�����ǰϵͳĬ�ϰ���, merch ΪNULL�����Ե�ǰ�����, ��¼��ת
	CMPIChildFrame	*LoadSpecialCfm(const CString &StrCfmName);	// ���������cfm, ��Ҫ��Ҫ�л�һ����Ʒ
	bool32			IsIndexInCurrentKLineFrame(const CString &StrIndexName, int32 iIndexFlag = 0);	// K��ҳ�����Ƿ���ڸ�ָ��, iIndex=0����ע��Ʒ�Ƿ�Ϊ���̻���� >0����Ϊ���� <0�����Ǹ���
	void			GetFrameSortedChartIoViews(OUT CArray<CIoViewBase *, CIoViewBase *> &aIoViews, CMPIChildFrame *pMustInFrame, int32 iGroupId=-1);

	void			ChangeIoViewKlineTimeInterval(int32 iTimeIntervalId);	// �ı�k�ߵ����ڣ�iTimeIntervalIdָ����IDM_CHART_KMINUTE���Ƶ�cmdid,Ϊ-1������Զ�ѡ����һ��������Ϊ��ǰ����

	bool32			ShowPhaseSort(const T_PhaseOpenBlockParam *pParam = NULL);	// ��ʾ�׶����в����Ի�����ȷ��������ʾ�׶�����ҳ��
	void			ShowHotBlockReport(const CGmtTime *pTimeStart=NULL, const CGmtTime *pTimeEnd=NULL);	// ��ʾ���Ű�����, ����������

	CMPIChildFrame	*CreateNewIoViewFrame(const CString &StrCfmName, UINT nPicId, bool32 bCfmSecret=true, CMerch *pMerchToChange=NULL);	// ����һ��ָ��ҵ����ͼ��ҳ��

	void			LoadPicCfmMenu();			// ��ʼ��ҳ������˵�
	bool32			OnPicMenuCfm(UINT nPicId);	// ����ָ����picҳ����ͼ

	void			ShowBourseNotification(int32 iBourseId);
	void			PostUpdateWnd();
	void			InitialImageResource();

	void			ChangeAnalysisMenu(bool32 bLockedFrame);		// ����frame������״̬��������˵�
	CNewMenu		*GetAnalysisMenu(bool32 bLockedFrame);			// ��ȡframe������״̬��Ӧ�ķ����˵�

	void			OnArbitrageF5( const CArbitrage &arb, CIoViewBase *pIoViewSrc=NULL);	// ����F5�л�
	void			OnArbitrageF5( UINT nDstArbTypeId, const CArbitrage &arb, CIoViewBase *pIoViewSrc=NULL);	// ����F5�л�
	bool32			OnArbitrageEscReturn( const CArbitrage &arb, CIoViewBase *pIoViewSrc=NULL);	// ����ͼ��esc

public:
	static void	    SaveXmlFile(const char * KpcFileName, const CString &StrContent);

	bool32			BeFromXml()	{ return m_bFromXml; }
	void			KeepRecentMerch(IN CMerch *pMerch);
	void			GetRecentMerchList(OUT CArray<T_RecentMerch, T_RecentMerch&> & RecentMerchList);

	static bool32   GetWspFlagAfterInitialMarketFlag();				  
	static void		SetWspFlagAfterInitialMarketFlag(bool32 bNeedSet); 

	//  ������ʷ��ʱ����.
	void			DealHistoryTrend(CIoViewKLine *pIoViewKLineSrc, const CKLine &KLineNow);
	void			DealHistoryTrendDlg(CIoViewKLine *pIoViewKLineSrc, const CKLine &KLineNow);

	CMPIChildFrame *GetHistoryChild()			{ return m_pHistoryChild; }
	void			SetHistoryChildNULL()		{ m_pHistoryChild = NULL; }

	CIoViewTrend *  GetHistoryTrend()			{ return m_pHistoryTrend; }
	void			SetHistoryTrendNULL()		{ m_pHistoryTrend = NULL; }

	int32			GetRandomNum();				// ȡһ�������	
	void			GenerateRandomNums();		// ���� 500 �������

	// ��ʾ֪ͨ	
	void			ShowWelcome();
    void            ShowUSerRegister();
	bool32			ShowNotifyMsg();
	void			DrawCustomNcClient();
	UINT			OnMyNcHitTest(CPoint pt);			// ���з��ظ�ϵͳ��NCHitTestֻ��Caption��none���ڲ�ʹ���������ncHitTest
	
	// ��������
	void			ShowCompleteMaterialDlg();          //�����������Ͻ���
	CString			GetCompleteMaterialUrl();			//��������Url

	// �û���ϸҳ��
	void			ShowUserInfoNormal();			// ����ֱ�׼��ʽ��ʾ�û���Ϣҳ��
	void			ShowUserInfoCfm();				// ����������ҳ��ʾ�û���Ϣҳ��
	void			ShowUserInfoDialog();			// �Ի��򵯴���ʾ�û���Ϣҳ��
	void			ShowOnlineService();			// ��ʾ���߿ͻ�ҳ��
	void			GetUserInfo(T_UserInfo &stUserInfo);					// ��ȡ�û�������Ϣ

	void			ShowRegisterDialog();			// ��ʾע��Ի���

	// TBWnd�й�
	bool32			OpenWorkSpace(LPCTSTR lpszPath);
	void			OnChildFrameMaximized(bool32 bMax);		// childframe���ʱ��ȥ������������Ҫ֪ͨmainFrame����������ť�Ĵ���TBWnd��
	void			RegisterAcitveIoView(CIoViewBase *pIoView, bool32 bActive);		// ��ioView acitve/deactive��Ϣ֪ͨ��TBWnd
	bool32			DoF10();
	bool32			DoF11();
	void			ShowConditionWarning();
	

	// �ֶ������ı�־
	bool32			GetManualReConnectFlag() const { return m_bManualReconnect; }

	int32			DeleteBlockReportItemData(CNewMenu *pPopupMenu);	// ɾ�����˵��ϸ��ӵ���Դ
 
	// �ѻ�ģʽ, ����һЩ���ܿ������
	void			AdjustByOfflineMode();

	// ������
	void	EnableMyDocking(DWORD dwDockStyle);
	void	AddDockSubWnd(CWnd *pWnd);			// ��Ӹ������Ӵ��ڣ����ø��Ӵ��ڵĸ�����Ϊ������
	void	RemoveDockSubWnd(CWnd *pWnd);		// �Ӹ������������Ƴ����Ӵ��� - ����û�д˲����ı�Ҫ���Ƴ��Ĵ���Ҫ�ֶ��ı丸�ӹ�ϵ
	void	ShowDockSubWnd(CWnd *pWnd);			// ��ʾ�ø������Ӵ���
	BOOL	IsExistDockSubWnd(CWnd *pWnd);		// �Ƿ���ڸø����Ӵ���
	void	DoToggleMyDock();					// ����/ͣ��
	
	void    OnShowImMessage();					//��ʾIM���� 2013-10-31 add by cym
	void	ControlDisStatus(bool bIsDis);		// ��ʾ�����Ҳ�����

	void	AddToUserDealRecord(CString StrFunName, E_ProductType eType);	// ����¼��ӵ��û���Ϊ��¼��vector��
	//���ÿ�������
	void    SetAutoRun(BOOL bAutoRun);
	//��鿪���Ƿ�����
	void    GetAutoRunKeyValue();



	/////////////��ҳ������ /////////////// 

	// getOptiona����ʹ��
	void PackUserBlockInfo(string &strUserBlockUtf8);
	// addUserStock��������
	void AddOrDelUserblockMerch(CString strMerchCode,CString strMarketid, bool bIsDel);

public: 
	void		AddGGTongView(CGGTongView* p);
	void		DelGGTongView(CGGTongView* p);
	void		GetAllGGTongViews(OUT CArray<CGGTongView*, CGGTongView*>& aGGTongViews);

	void		AddIoView(CIoViewBase* pViewBase);
	void		RemoveIoView(CIoViewBase* pViewBase);
	void	    OnOpenChildframeFromWsp(CString strXmlName,bool bOpenCur);
	void		ShowNewsCountent(T_NewsContentInfo& stNewsContentInfo);

public:
	void		OnQuickTrade(int32 iFlag, CMerch* pMerch, CReportScheme::E_ReportHeader eHeader = CReportScheme::ERHCount);
	bool		HideAllTradeWnd();		// �������н��״���

	int32		GetValidGroupID(); //��ȡ��Ч�Ŀ��÷���ID

	//********************************************************
	//��Щ������ͼ����Ĺ���ķ���������½�һ�������� by hx
	//��ͼ���� 
	void		OnViewMerchChanged(IN CIoViewBase *pIoViewSrc, IN CMerch *pMerch);
	void		OnViewMerchChanged(int32 iIoViewGroupId, CMerch *pMerch);

	void		OnViewMerchChangeBegin(IN CIoViewBase *pIoViewSrc, IN CMerch *pMerch);
	void		OnViewMerchChangeBegin(IN int32 iIoViewGroupId, CMerch *pMerch);
	void		OnViewMerchChangeEnd(IN CIoViewBase *pIoViewSrc, IN CMerch *pMerch);
	void		OnViewMerchChangeEnd(IN int32 iIoViewGroupId, CMerch *pMerch);
	
	void		OnViewGridHeaderChanged(E_ReportType eReportType);	
	void		OnViewFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName );

	// �����Ʒ���
	bool32		OnViewAddCompareMerch(IN CIoViewBase *pIoViewSrc, IN CMerch *pMerch);
	void		OnViewRemoveCompareMerch(IN CIoViewBase *pIoViewSrc, IN CMerch *pMerch);
	void		OnViewClearCompareMerchs(IN CIoViewBase *pIoViewSrc);

	bool		HasValidViewTextNews();
	
	CMerch		*GetIoViewGroupMerch(int32 iGroupId); // Ӧ��Ҫ��¼ÿ��group�����������һȫ��groupid��������ͼ����ȫ��groupid TODO
	//********************************************************

	void		RemoveHotKey(E_HotKeyType eHotKeyType);
	void		BuildHotKeyList();
	//
	void        RedrawAdvertise();
	void        CreateAdvertise();
	void        HideAdvertise();
	void		SetAdvertiseOwner(CWnd *pWnd);
	void        NotifyAdvMerchChange();
	void        SetAdvFont(LOGFONT *pLogFont);
///////////////////////////////////////////////
	// ��ȡ��ǰ������ڲ˵��İ�ť
	int TMenuButtonHitTest(CPoint point);

	// ��ȡ��ǰ�������ϵͳ�İ�ť
	int TSysButtonHitTest(CPoint point);

	// �ڱ���������Ӳ˵���ť
	CNCButton *AddTitleMenuButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);

	// �ڱ����������ϵͳ��ť
	CNCButton *AddTitleSysButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);

	// �ı���������û���
	void ChangeUserNickName(CString strNickName);

	CMenuBar *GetMenuBar();

	//��ʾ�����˵�
	void ShowHelpMenu(UINT nID);

	// ��ҽ���
	void DoTradePlugin();
	
	bool32	GetTradeExePath(OUT CString &StrExePath, bool32 bEnableReq=false, bool32 bDirtyOld=false);	// ��ȡ���׳���·����û��������Ҫ�û�ѡ������
	void    SetTradeExePath( LPCTSTR lpszExePath );// ���ý��׳���·����NULL����0����Ϊɾ��������
	void    ShowRealTradeDlg();

public:
	void handleMsgCenterRes(std::string strJson);// ������Ϣ���ĵ���Ӧ
	void ShowInfoCenterMsg(T_pushMsg stMsg);
	void ConnNotify(E_CONN_NOTIFY eConnNotify);

	void handleHeartbeat(Json::Value vData);// ������Ѷ������
	void handleInfoContentQuery(Json::Value vData);// ������Ѷ���ݵĲ�ѯ
	void handlePushInfo(Json::Value vData);// ������Ѷ������
	// ������Ҫˢ��Url��ذ���
	void OpenWebWorkspace(const CString &StrWsp, const CString &StrUrl);

private:
	void ShowMenu(T_MenuItemData tData,E_MenuItemType eType, CRect rect);
	CDlgPullDown* CreateSubMenu(T_MenuItemData &tmenuData, E_MenuItemType eType, int left, int top);		// ���ఴť�Ķ����˵�
	CDlgPullDown* AdjustSubMenuPos(CString strParent, int left, int top);	// ���������˵�����ʾλ��
	void InitMapMenuItem();        // ��ʼ���˵�������
	int32 FindFirstVecIDByMenuID(int32 iMenuID);
	CNCButton*	GetCNCButton(int iId);

private:
	void ShowSystemMenu(CRect rect);
	void ShowQuoteMenu(CRect rect);
	void ShowFuncMenu(CRect rect);       // ��ʾ���ܲ˵�


//  web���������
private:
	void WebComOpenWebPage(CAnalysisWeb cAnalysisWeb);
	void WebComOpenLocalFile(CAnalysisWeb cAnalysisWeb);
	void WebComOpenView(CAnalysisWeb cAnalysisWeb);
	void WebComOpenLocalApp(CAnalysisWeb cAnalysisWeb);
	void WebComOpenPassMsg(CAnalysisWeb cAnalysisWeb);

	
//   ѡ��ģ���������
public:
	/////////////////////////////////////////////////////
	//===================CODE CLEAN======================
	//////////////////////////////////////////////////////
	//void QueryPickModelTypeInfo();
	//void QueryPickModelTypeStatus();

	//void PackJsonType(string &strTransDataUtf8);	// ��װ����
	//void UnPackJsonPickModelTypeInfo(string strMerchData);		// ���ѡ������
	//void UnPackJsonPickModelTypeStatus(string strMerchData);		// ���ѡ��״̬
	//map<int32, map<int32,T_SmartStockPickItem>> m_mapPickModelTypeGroup;		// ѡ��ģ������
	//vector<int>    m_vPickModelTypeGroup;
	//map<int32, map<int32,T_SmartStockPickItem>> m_mapPickModelStatusGroup;		// ���ػ�����ѡ��״̬


	int				m_iSlectModelId;											// ѡ���ѡ��ģ����
public:
	bool32			  m_bDoLogin;		//	�Ƿ��¼��
	bool32			  m_bLoginSucc;		//	�Ƿ��¼�ɹ�
	bool32			  m_bShowLoginDlg;	//	�˳�ʱ�Ƿ���ʾ��¼����
	int  m_nReqTime;
	bool m_bStartEconoReq;
	std::string m_strFirstEconoData;// �����һ������Ĳƾ���������
	CDlgTrace*		  m_pDlgTrace;
	CAdjustViewLayoutDlg* m_pDlgAdjustViewLayout;
	CKeyBoardDlg*	  m_pDlgKeyBoard;
	CDlgSpirite*	  m_pDlgSpirite;
	CDlgNetWork*	  m_pDlgNetWork;
	CDlgBrokerWatch*  m_pDlgBrokerWatch;
	CDlgRecentMerch*  m_pDlgRecentMerch;
	CDlgIndexPrompt*  m_pDlgIndexPrompt;
	CDlgAlarm *		  m_pDlgAlarm;
	CDlgWait *		  m_pDlgWait;
	CDlgIm           *m_pDlgIm; // 2013-10-28 add by cym ����IM�Ի���
	CDlgIndexChooseStock* m_pDlgIndexChooseStock;
	CDlgNetFlow*	  m_pDlgNetFlow;
	CDlgBelongBlock*  m_pDlgbelong;
	CDlgNewCommentIE  *m_pDlgComment;		// ע�ᵯ��

	//CDlgF10*		  m_pDlgF10;
	CDlgWebF10*	m_pDlgF10;


	CNewMenu		  m_menuAnalysisLocked;		// ����״̬�µķ����˵�
	CNewMenu		  m_menuAnalysisUnLock;		// ������״̬�µķ����˵�

	CArray<HWND, HWND>	m_aCfmsReturn;	// ÿ����תʱ��ǰ�����

	//CDlgLoginNotify*  m_pDlgLoginNotify;

	//CToolBar		m_wndTestToolBar;

	CStatusBarEx	  m_wndStatusBar;
	CCommentStatusBar m_wndCommentSBar; //���������� 2013-11-12 by cym 
	CNewToolBar		m_wndToolBar;
	CRect			m_RectBBs;
	bool32			m_bOpenLast;
	bool32			m_bMaxForSysSettingChange;	
	CMyContainerBar	*m_pMyControlBar;			// ����������
	CTradeContainerWnd	*m_pTradeContainer;		// ���״���
	CTradeContainerWnd	*m_pSimulateTrade;		// ģ�⽻��

	CArray<T_RecentMerch,T_RecentMerch&>		m_aRecentMerchList;
	CArray<T_BlockMenuInfo,T_BlockMenuInfo&> m_aBlockMenuInfo;

	CRect			m_rectWindow;
	CRect			m_rectCaption;
	CRect			m_rectMin;
	CRect			m_rectMax;
	CRect			m_rectClose;
	CRect			m_rectLast;
	int32			m_iShowFlag;
	CWnd *			m_pKBParent;
	DWORD			m_dwHotKeyTypeFlag;		// �Ƿ�ֻ��ע�ض����
	CPoint			m_ptLastNCLButtonDown;
	
	// ���̾���
	bool32			m_bForceFixKeyBoard;			// ǿ��ָ��һ��Ŀ��, ������������ false,�����ܸ���
	
	// ��ʷ��ʱ
	bool32			   m_bShowHistory;
	CMPIChildFrame*    m_pHistoryChild;
	CIoViewTrend*	   m_pHistoryTrend;

	static bool32	   m_bNeedSetWspAfterInitialMarket;		// �г���ʼ���ɹ����Ƿ���Ҫ���õ�¼����Ϣ
	//
	CDlgAdvertise *m_pDlgAdvertise;

	CNewTBWnd		   *m_pNewWndTB;
    CNewCaptionTBWnd   *m_pNewCaptionWndTB;
	CLeftToolBar       m_leftToolBar;
	//CShortCutMenuToolBar      m_ShortCutMenuToolBar;
	CDlgJPNews		   *m_pDlgJPNews;

	bool32				m_bQuickOrderDn;
    UINT                m_LastTimeSet;
	CNewMenu			m_sysMenu;
	vector<T_UserDealRecord> m_vUserDealRecord;

	T_WebOpenCfm		m_stWebOpenCfm;		// ��ҳ�򿪰������Ҫ�Ĳ���
private:
	CGGTongView*	   m_pActiveGGTViewFromXml;	
	CGGTongView*	   m_pEmptyGGTongView;

	Image*			   m_pImgLogo;	// ��ҳ��ı���������ͼ
	Image*			   m_pImgLogoLeft;	// �����������logo
    Image*			   m_pImgMenuBtn1;
	Image*			   m_pImgSysBtn;
	Image*			   m_pImgHideBtn;
	Image*			   m_pImgShowBtn;
	Image*			   m_pImgSpliter;
	Image*			   m_pImageCaption;
    Image*             m_pImageUserAccount;
    Image*             m_pImageLogin;
	Image*             m_pImgUserPhoto;
	Image*             m_pImgMenuExit;
	Image*             m_pImgMenuHelp;
	Image*             m_pImgMenuAlarm;
	Image*             m_pImgMenuSelStock;
	Image*             m_pImgMenuSystem;
	Image*             m_pImgMenuTrade;
	Image*             m_pImgLoginSpliter;
	//�����˵�ͼƬ
    Image*             m_pImgCenterSplitBottom;
	Image*             m_pImgCenterSplitTop;
	Image*             m_pImgHelpBottom;
	Image*             m_pImgHelpTop;
	Image*             m_pImgItemArrow;
	Image*             m_pImgItemSimple;
	Image*             m_pImgSysBottom;
	Image*             m_pImgSysTop;
	Image*             m_pImgTradeTop;
	
	E_CurrentShowState m_eCurrentShowState;

private:
	CDlgPullDown *m_pDlgPullDown;                          //  �����˵�
	vector<CDlgPullDown*> m_vecDlgSubPullDown;             //  ���������˵� 

	buttonContainerType m_mapPullDown;						//	�����˵��İ�ť��
	buttonContainerType m_mapMorePullDown;					//	���������˵��İ�ť��

	Image*             m_pImgArrowMenu;
	Image*             m_pImgMenu;
	std::map<int,VecMenuContainer>   m_mapMenuCollect;     // һ���˵������ݼ���
	CNCButton::T_NcFont m_fontMenuItem;
	map<int32,int32>    m_mapMenuQuoteID;	               // �˵�id ��Ӧ���г�id
	std::map<int, CNCButton>  m_mapPullDownBtns;           // ��ǰ��ʾ��һ���˵��Ͷ����˵��İ�ť���ϣ�Ϊ�������ð�ť�Ƿ���÷���

private:
	CMDIClientEx	   m_wndMDIClient;
	CReBar			   m_wndReBar;
    //CReBar			   m_wndReBar2;

	map<int, CNCButton> m_mapMenuBtn;		// �������˵���ť�б�
	map<int, CNCButton> m_mapSysBtn;		// ������ϵͳ��ť�б�
	int			m_iXMenuBtnHovering;		// ��ʶ������˵���ť����
	int			m_iXSysBtnHovering;			// ��ʶ������ϵͳ��ť����

	//2013-10-29 by cym ��¼ԭ����ƶ���λ��
	CRect m_RectOld;

	int32	m_nFrameWidth;	// �߿���
	int32	m_nFrameHeight;	// �߿�߶�
	int32	m_nTitleLength;	// �������ı����
	//CMenuBar	*m_pMenuBar;// �˵���

private:
    CDlgAccount  *m_pDlgAccount;
	CDlgWebContent	*m_pDlgWebContent;
    CDlgDefaultUserRegister *m_pDlgRegister;
    CDlgPushMessage *m_pDlgPushMsg;
	
    std::map<int, CNCButton> m_mapHelpBtn;	// ��ҳ��������ť�б�
	int32			   m_iRandIndex;
	CArray<int32,int32> m_aiRandomNums;
	
	int32			   m_uiConnectServerTimers;
	bool32			   m_bHaveUpdated;						// �Ƿ���¹�

	int32			   m_uiConnectNewsServerTimers;			// ��Ѷ���������Ӽ���

	bool32			   m_bFirstDisplayFromXml;				// ��־�Ƿ��ǵ�һ�δ�Xml��ȡ�������ݣ���ʼ����ʾ����һ����Ч

	bool32			   m_bFromXml;


	CString			   m_StrWindowText;						// ���ڱ�������
	bool32			   m_bWindowTextValid;					// ���ڵĴ��ڱ��������Ƿ���Ч
private:	
	bool32			   m_bManualReconnect;					// �Ƿ����ֶ�����������, ���ʱ��, ����������������˲�Ҫ����֤����, ����������ٷ�

	DWORD				m_dwUserConnectCmdFlag;

	T_UserInfo          m_stUserInfo;                       //�û���Ϣ

private:
	CRecordDataCenter* m_pRecordDataCenter;					// ��¼���ݿ�

private:
	CArray<CGGTongView*, CGGTongView*>	m_aGGTongViews;
	CHotKey m_hotKey;

public:
	CArray<CIoViewBase*,CIoViewBase*>	m_IoViewsPtr;
	CArray<T_CompareMerchInfo, T_CompareMerchInfo&>	m_aCompareMerchList;	// ������Ʒ�б�
	CArray<CHotKey, CHotKey&>	m_HotKeyList;				// ��ݼ�

///////////////////////////////////////////////////////////////////////////////////////
//

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual int	 OnToolHitTest(CPoint point, TOOLINFO* pTI)const;
	virtual void RecalcLayout(BOOL bNotify  = TRUE );
	virtual void OnUpdateFrameMenu(HMENU hMenuAlt);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL


// Generated message map functions
public:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWindowNew();
	afx_msg void OnUpdateWindowNew(CCmdUI* pCmdUI);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnSetting();
	afx_msg void OnGridheadSet();
	afx_msg void OnBlockSet();
	afx_msg LRESULT OnMsgHotKey(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgGridHeaderChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgFontChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgColorChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgMainFrameOnStatusBarDataUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnTimerFreeMemory(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnTimerSyncPushing(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnTimerInitializeMarket(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataServiceConnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataServiceDisconnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataRequestTimeOut(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataCommResponse(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataServerConnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataServerDisconnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnDataServerLongTimeNoRecvData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnGeneralNormalUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnGeneralFinanaceUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnMerchTrendIndexUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnMerchAuctionUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnMerchMinuteBSUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnAuthPlugInHeart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnKLineNodeUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgIndexChsMideCoreKLineUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnTimesaleUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnNewsListUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnF10Update(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnLandMineUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnPublicFileUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnRealtimePriceUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnRealtimeLevel2Update(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnRealtimeTickUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnAllMarketInitializeSuccess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnMsgViewDataOnNewsResponse(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnClientTradeTimeUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgPackageLongTime(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgPackageTimeOut(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgMainFrameKickOut(WPARAM wParam, LPARAM lParam);

	/////////////////////////////////////////////////////
	//===================CODE CLEAN======================
	//////////////////////////////////////////////////////
	/*afx_msg LRESULT OnMsgPickModelTypeInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgPickModelTypeStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgPickModelTypeEvent(WPARAM wParam, LPARAM lParam);*/

	
	afx_msg LRESULT OnUpdateSelfDrawBar(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgMainFrameManualOptimize(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnChooseStockResp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDapanStateResp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChooseStockStatusResp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNewStockResp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnToolbarCommand(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT	OnMsgViewDataNewsServerConnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnMsgViewDataNewsServerDisConnected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnMsgDllTraceLog(WPARAM w, LPARAM l);
	afx_msg LRESULT OnMsgViewDataOnAuthFail(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnNewWorkspace();
	afx_msg void OnNewChildframe();
	afx_msg void OnNewCfm();		// �����µ�ҳ��
	afx_msg void OnCloseAllWindow();	// �ر�����ҳ��(��Ĭ����)
	afx_msg void OnOpenChildframe();
	afx_msg void OnUpdateSaveChildFrame(CCmdUI* pCmdUI);
	afx_msg void OnSaveChildFrame();
	afx_msg void OnUpdateLockChildFrame(CCmdUI* pCmdUI);
	afx_msg void OnLockChildFrame();
	afx_msg void OnOpenWorkspace();
	afx_msg void OnSaveasChildframe();
	afx_msg void OnSaveasWorkspace();
	afx_msg void OnSaveWorkspace();
	afx_msg void OnFormulaMag();
	afx_msg void OnDelView();
	afx_msg void OnUpdateCloseChildFrame(CCmdUI *pCmdUI);
	afx_msg void OnCloseChildFrame();
	afx_msg void OnUpdateViewSplitCmd(CCmdUI *pCmdUI);
	afx_msg void OnAddUpView();
	afx_msg void OnAddDownView();
	afx_msg void OnAddLeftView();
	afx_msg void OnAddRightView();
	afx_msg void OnDisConnectServer();
	afx_msg void OnConnectServer();
	afx_msg void OnDownLoad();
	afx_msg void OnUpdate();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNcPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnNcMouseLeave(WPARAM, LPARAM);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnMenuBlock(UINT nID);
	afx_msg void OnMenuQuoteMarket(UINT nID);
	afx_msg void OnUpdatePicMenu(CCmdUI *pCmdUI);
	afx_msg void OnPictureMenu(UINT nId);	
	afx_msg void OnIoViewMenu (UINT nID);
	afx_msg void OnUpdateIoViewMenu(CCmdUI *pCmdUI);
	afx_msg void OnMenuIoViewReport(UINT nID);
	afx_msg void OnMenuIoViewChart(UINT nID);
	afx_msg void OnMenuIoViewDetail(UINT nID);
	afx_msg void OnMenuIoViewStarry(UINT nID);
	afx_msg void OnMenuUIIoViewStarry(CCmdUI *pCmdUI);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnInfoNews();
	afx_msg void OnNetFlow();
	afx_msg void OnSelfDraw();
	afx_msg void OnInfoShBourse();
	afx_msg void OnInfoSzBourse();
	afx_msg void OnInfoHkBourse();
	afx_msg void OnInfoF10();
	afx_msg void OnBrokerWatch();
	afx_msg void OnSelectStock();
	afx_msg void OnCustomStock();
	afx_msg void OnViewCalc();
	afx_msg void OnIndexChooseStock();	
	afx_msg void OnViewStatusBar();
	afx_msg void OnViewCommentStatusBar(); //����״̬�� 2013-11-14 by cym
	afx_msg void OnViewSpecialBar();
	afx_msg void OnUpdateViewSpecialBar(CCmdUI *pCmdUI); 
	afx_msg void OnUpdateViewStatusBar(CCmdUI *pCmdUI); 
	afx_msg void OnUpdateViewCommentStatusBar( CCmdUI *pCmdUI );
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDlgSyncExcel();			// ͬ��Excel����
	afx_msg void OnAddClrKLine();
	afx_msg void OnAddTrade();
	afx_msg void OnDelAllPrompt();
	afx_msg void OnStockSpecial(UINT nId);		// ��ɫ�˵�����ȫ��������
	afx_msg void OnStockSpecialUpdateUI(CCmdUI *pCmdUI);
	afx_msg void OnBelongBlockMsg();//linhc ����Ҽ��˵���������顱
	afx_msg void OnVedioTrainning();
	afx_msg LRESULT OnSetWindowText(WPARAM w, LPARAM l); // �������ô��ڱ�����Ϣ WM_SETTEXT
	afx_msg LRESULT OnGetWindowText(WPARAM w, LPARAM l);
	afx_msg LRESULT OnGetWindowTextLength(WPARAM w, LPARAM l);
	afx_msg LRESULT OnFindGGTongView(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTabRename(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnGetNowUseColor(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnGetSysColor(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnGetSysFontObject(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnGetIDRMainFram(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnIsShowVolBuySellColor(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnGetStaticMainKlineDrawStyle(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnGetCenterManager(WPARAM wParam,LPARAM lParam);
	afx_msg void OnConnectNewsServer();	// ��Ѷ������
	afx_msg void OnDisConnectNewsServer();
	afx_msg LRESULT OnMsgViewDataOnNewsTitleUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnNewsContentUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnCodeIndexUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgViewDataOnNewsPushTitle(WPARAM wParam, LPARAM lParam);
	afx_msg void OnConnectSetting();
	afx_msg void OnChooseSimpleStock();
	afx_msg void OnShowShortcutBar();
	afx_msg void OnMerchNotePad();
	afx_msg void OnMarketRadar();
	afx_msg void OnUpdateMarketRadar(CCmdUI *pCmdUI);
	afx_msg void OnMarketRadarAddResult();
	afx_msg LRESULT OnCheckCfmHide(WPARAM w, LPARAM l);		// wҪ����childframe hwnd
	afx_msg void OnUpdateMainMenu(CCmdUI *pCmdUI);
	afx_msg void OnMainMenu(UINT nId);
	afx_msg void OnArbitrage();
	afx_msg void OnDefaultCfmLoad();
	afx_msg void OnUpdateConnectServer(CCmdUI *pCmdUI);
	afx_msg void OnIWantBuy();
	afx_msg void OnMenuInvestmentAdviser(UINT nId);
	afx_msg void OnMenuNews(UINT nId);
	afx_msg void OnMenuTrade(UINT nId);
	afx_msg void OnMenuClassRoom();
	afx_msg void OnMenuPhaseSort();
	afx_msg void OnAutoRun();

	afx_msg void OnFirmTradeShow();
	afx_msg	void OnSimulateTradeShow();
	afx_msg	void OnSimulateOpenAccount();
	afx_msg	void OnSimulateHQRegister();
	afx_msg	void OnSimulateHQLogin();
	afx_msg LRESULT	OnSimulateImageUrl(WPARAM wParam, LPARAM lParam);
    afx_msg void OnHQRegisterSucced();
	afx_msg LRESULT	OnWebReLoginPC(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnWebPersonCenterRsp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnWebCommand(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnWebCallBack(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnWebOpenCfm(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnWebOpenTrend(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnOpenOutWeb(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnShowWebDlg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRestoreWorkSpace();
	afx_msg void OnMenuAutoConnect();
	afx_msg void OnUpdateMenuAutoConnect(CCmdUI* pCmdUI);
	afx_msg void OnMenuShowToolBar();
	afx_msg void OnUpdateMenuShowToolBarStatus(CCmdUI* pCmdUI);
	afx_msg void OnExportData();
	afx_msg LRESULT OnShowRightTip(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddUserDealRecord(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnIsUserHasIndexRight(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnJumpToTradingSoftware(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnShowJPNews(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnBrowerPtr(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnJPNewsTitle(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	afx_msg LRESULT OnReqInfoCenter(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnWebCloseDlg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnCloseOnlineServDlg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnWebThirdLoginRsp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnWebkeyDown(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__234DC6B0_FD03_4692_B156_E8947A1EB907__INCLUDED_)
