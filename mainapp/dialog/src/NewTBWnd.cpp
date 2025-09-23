// NewTBWnd.cpp : implementation file
//
#include "stdafx.h"
#include <sstream>
#include "NewTBWnd.h"
#include "PathFactory.h"
#include "XmlShare.h"
#include "WspManager.h"
#include "CommentStatusBar.h"
#include "StatusBarEx.h"
#include "NewMenu.h"
#include "BlockConfig.h"
#include "DlgTodayComment.h"
#include "DlgNewLogin.h"
#include "PluginFuncRight.h"
#include "FontFactory.h"
#include "UrlParser.h"
#include "CCodeFile.h"
#include "coding.h"
#include "IoViewNews.h"
#include "StockSelectManager.h"
#include "IoViewKLine.h"
#include "sha1.h"

#include "IoViewReportArbitrage.h"
#include "IoViewKLineArbitrage.h"
#include "IoViewTrendArbitrage.h"
#include "IoViewDuoGuTongLie.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INVALID_ID -1
#define FONT_NAME_LENGTH     100
#define IDC_HAND MAKEINTRESOURCE(32649)	//手型光标

#define IDC_CONTROL_LEFT_ID		1000001//左侧控制id
#define IDC_CONTROL_RIGHT_ID	1000002//右侧控制id

#define CONTROL_WIDTH			20		//控制按钮

#define NUM_HOME_QUOTE		4		//首页中行情显示的个数
#define NUM_HOME_FORMULAS	2		// 
#define NUM_HOME_PAGES		2

#define    COLOR_NORMAL        RGB(219, 211, 74)
#define    COLOR_HOVER         RGB(239, 123, 54)
#define    COLOR_SELECT        RGB(140, 171, 122)

const int32 KTimeId				= 10000;		// 定时检测
const int32 KTimePeriod			= 1000;			// 每秒检测一次行情,资讯连接情况

const TCHAR KStrTradeSecPrefix[]	= _T("Trade");
const TCHAR KStrTradeKey[]	= _T("InternalTrade");

static const int32 s_KiYSpace = 0;			// top上的间距
static const int32 s_KiBottomSpace = 0;		// bottom上的间距
static const int32 s_KiXSpace = 0;			// left上的间距
static const int32 s_KiGroupSpace = 0;		// 分组之间的间距
static const int32 s_KiBtnSpace = 0;		// 分析，功能，系统设置三个页面 按钮之间的间距


/////////////////////////////////////////////////////////////////////////////
// CNewTBWnd

CNewTBWnd::CNewTBWnd(CWnd &wndParent):m_wndParent(wndParent)
{

	m_iXButtonHovering = INVALID_ID;
	m_iCurrentPage	   = IDC_BTN_MAIN;
	m_pDlgPullDown	   = NULL;

	m_indexNews = 0;

	m_blShowIM = false;
	m_RectIm = CRect(0,0,0,0);

    m_clrTextNor	= /*0xc3c4c4;*/RGB(195,195,196);
    m_clrTextHot	= /*0xff3837;*/RGB(255,56,55);
    m_clrTextPress	= /*0xff3837;*/RGB(255,56,55);

    m_crlThirdTextNor	= RGB(222,168,63);
    m_crlThirdTextHot	= RGB(222,168,63);
    m_crlThirdTextPress	= RGB(222,168,63);

	m_bMember = TRUE;
	m_bPaint = false;
	m_bShowIntersect = false;
	m_mapControlBtn.clear();
    InitFont();

	m_iLeftFixedWidth = 0;
	m_iRightFixedWidth = 0;
	m_iLeftFixedPos = 0;
	m_iRightFixedPos = 0;
	m_pChildWnd = NULL;
	m_mapMorePullDown.clear();
}

CNewTBWnd::~CNewTBWnd()
{
	DEL(m_pDlgPullDown);
	DEL(m_pChildWnd);

	vector<CDlgPullDown*>::iterator iter;
	for (iter = m_vecDlgSubPullDown.begin() ; iter != m_vecDlgSubPullDown.end() ; ++iter)
	{
		DEL(*iter);
	}
	vector<CDlgPullDown*>().swap(m_vecDlgSubPullDown);
}


BEGIN_MESSAGE_MAP(CNewTBWnd, CWnd)
	//{{AFX_MSG_MAP(CNewTBWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_RBUTTONUP()
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewTBWndmessage handlers
int CNewTBWnd::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	int iCreate = CWnd::OnCreate(lpCreateStruct);
	if ( -1 == iCreate )
	{
		return iCreate;
	}

	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if (pApp)
	{
		m_bMember = pApp->m_pConfigInfo->m_bMember;
	}

	if (!m_bMember)
	{
		m_indexNews = 1;
	}

	int iToolBarHeight = 0;
	if(pApp)
	{
		iToolBarHeight = pApp->m_pConfigInfo->m_toolBarData.m_iHeight;
	}

	if (NULL == m_pChildWnd)
	{
		m_pChildWnd = new CNewTBChildWnd(this, m_mapBtn);
		m_pChildWnd->Create(NULL, _T("tbsub"), WS_CHILD | WS_VISIBLE, CRect(0,0,iToolBarHeight,iToolBarHeight), this, 0x1688);
	}

    InitToolbarButtons();	

	if (NULL == m_pDlgPullDown)
	{
		m_pDlgPullDown = new CDlgPullDown(this);
		m_pDlgPullDown->Create(IDD_DIALOG_PULLDOWN, this);
	}
	
	SetTimer(KTimeId, KTimePeriod, NULL);

	return iCreate;
}

void CNewTBWnd::OnTimer(UINT nIDEvent)
{
	if ( KTimeId == nIDEvent)
	{	

	}
}

BOOL CNewTBWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    int32 iID = (int32)wParam;
    CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
    if(NULL == pApp)
    {
        return CWnd::OnCommand(wParam, lParam);
    }

	if (lParam == 1)//代表右键消息到来
	{
		RbuttonProcess(iID);
		return true;
	}

    //
	OnToolbarEvent(m_mapBtn, iID, wParam, lParam);

	// 左边固定按钮消息处理
	OnToolbarEvent(m_mapLeftFixedBtn, iID, wParam, lParam);
	
	// 右边固定按钮消息处理
	OnToolbarEvent(m_mapRightFixedBtn, iID, wParam, lParam);
    
	buttonContainerType::iterator itMore = FIND_BTN_ITERATOR(m_mapPullDown, iID);
	if (m_mapPullDown.end() != itMore)
	{
		CNCButton &btnControl = *itMore;
		int btnDataIndex = -1;
		int chlIndex = -1;
		int columnIndex = -1;
		bool bChild = false;
		bool bCol = false;

		if(FindBtnData(btnControl.GetControlId(), btnDataIndex, chlIndex, columnIndex, bChild, bCol))
		{
			T_ChildButtonData tmpChlData = pApp->m_pConfigInfo->m_vecColumnData[columnIndex].m_BtnData[btnDataIndex].m_ChildBtnData[chlIndex];

			if ( !CPluginFuncRight::Instance().IsUserHasRight(tmpChlData.m_Text, true, true))
			{
				m_pChildWnd->ChangeGroupStatus(m_mapPullDown, iID);
				return CWnd::OnCommand(wParam, lParam);
			}

			int iChlWidth = IsAllNumbers(tmpChlData.m_DialogWidth)?_ttoi(tmpChlData.m_DialogWidth):0;
			int iChlHeight = IsAllNumbers(tmpChlData.m_DialogHeight)?_ttoi(tmpChlData.m_DialogHeight):0;
			OnBtnEvent(tmpChlData.m_EventType, tmpChlData.m_EventParam, wParam, lParam, tmpChlData.m_Text, iChlWidth, iChlHeight);
		}
	}

	buttonContainerType::iterator itMoreSub = FIND_BTN_ITERATOR(m_mapMorePullDown, iID);
	if (m_mapMorePullDown.end() != itMoreSub)
	{
		CNCButton &btnControl = *itMoreSub;
		int btnDataIndex = -1;
		int subChlIndex = -1;
		int chlIndex = -1;
		int columnIndex = -1;
		bool bChild = false;
		bool bCol = false;

		if(FindSubBtnData(btnControl.GetControlId(), btnDataIndex, chlIndex, subChlIndex, columnIndex, bChild, bCol))
		{
			T_ChildButtonData tmpChlData = pApp->m_pConfigInfo->m_vecColumnData[columnIndex].m_BtnData[btnDataIndex].m_ChildBtnData[chlIndex].m_ChildSubMenuData[subChlIndex];

			if ( !CPluginFuncRight::Instance().IsUserHasRight(tmpChlData.m_Text, true, true))
			{
				m_pChildWnd->ChangeGroupStatus(m_mapMorePullDown, iID);
				return CWnd::OnCommand(wParam, lParam);
			}

			int iChlWidth = IsAllNumbers(tmpChlData.m_DialogWidth)?_ttoi(tmpChlData.m_DialogWidth):0;
			int iChlHeight = IsAllNumbers(tmpChlData.m_DialogHeight)?_ttoi(tmpChlData.m_DialogHeight):0;
			OnBtnEvent(tmpChlData.m_EventType, tmpChlData.m_EventParam, wParam, lParam, tmpChlData.m_Text, iChlWidth, iChlHeight);
		}
	}

	return CWnd::OnCommand(wParam, lParam);
}

BOOL CNewTBWnd::OnToolbarEvent(buttonContainerType &mapBtn, int32 iID,  WPARAM wParam, LPARAM lParam)
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CMainFrame *pMainFrm = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	
	if(NULL == pApp || NULL == pMainFrm)
	{
		return CWnd::OnCommand(wParam, lParam);
	}

	buttonContainerType::iterator itToolbar = FIND_BTN_ITERATOR(mapBtn, iID);
	if (mapBtn.end() != itToolbar)
	{
		CNCButton &btnControl = *itToolbar;
		int btnDataIndex = -1;
		int chlIndex = -1;
		int columnIndex = -1;
		bool bChild = false;
		bool bCol = false;

		if(FindBtnData(btnControl.GetControlId(), btnDataIndex, chlIndex, columnIndex, bChild, bCol))
		{
			T_ButtonData tmpData = pApp->m_pConfigInfo->m_vecColumnData[columnIndex].m_BtnData[btnDataIndex];

			CString StrText = tmpData.m_Text;
			StrText.Replace(_T(" "), _T(""));

			if ( !CPluginFuncRight::Instance().IsUserHasRight(StrText, true, true))
			{
				return CWnd::OnCommand(wParam, lParam);
			}

			if(tmpData.m_ChildBtnData.size() > 0)
			{
				ShowMenu(tmpData, btnControl.GetControlId());
				return CWnd::OnCommand(wParam, lParam);
			}
			else
			{
				int iWidth = IsAllNumbers(tmpData.m_DialogWidth)?_ttoi(tmpData.m_DialogWidth):0;
				int iHeight = IsAllNumbers(tmpData.m_DialogHeight)?_ttoi(tmpData.m_DialogHeight):0;
				OnBtnEvent(tmpData.m_EventType, tmpData.m_EventParam, wParam, lParam, tmpData.m_Text, iWidth, iHeight);
			}
		}
	}

	return CWnd::OnCommand(wParam, lParam);
}

BOOL CNewTBWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
/*	return CWnd::OnEraseBkgnd(pDC);*/
}

void CNewTBWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
    if(m_iCurrentPage == IDC_BTN_MAIN)
    {
		// 更新固定项按钮状态
		UpdateFixedStatus(point);
    }

    // 响应 WM_MOUSELEAVE消息
    TRACKMOUSEEVENT csTME;
    csTME.cbSize	= sizeof (csTME);
    csTME.dwFlags	= TME_LEAVE;
    csTME.hwndTrack = m_hWnd ;		// 指定要追踪的窗口 
    ::_TrackMouseEvent (&csTME);	// 开启Windows的WM_MOUSELEAVE事件支持 

    CWnd::OnMouseMove(nFlags, point);
}

LRESULT CNewTBWnd::OnMouseLeave(WPARAM wParam, LPARAM lParam)       
{     
    if(m_iCurrentPage == IDC_BTN_MAIN)
    {
		// 更新固定项按钮状态
		if (INVALID_ID != m_stFixedBtnHovering.iButtonId)
		{
			if (m_stFixedBtnHovering.bLeft)
			{
				GetCNCButton(m_mapLeftFixedBtn, m_stFixedBtnHovering.iButtonId).MouseLeave();
			}
			else
			{
				GetCNCButton(m_mapRightFixedBtn, m_stFixedBtnHovering.iButtonId).MouseLeave();
			}
			
			m_stFixedBtnHovering.bLeft     = true;
			m_stFixedBtnHovering.iButtonId = INVALID_ID;
		}
    }

    return 0;       
}   

void CNewTBWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
    if(m_iCurrentPage == IDC_BTN_MAIN)
    {
		// 固定项按钮区域
		bool32 bLeft = false;
		int iButton = TFixedButtonHitTest(point, bLeft);
		if (INVALID_ID != iButton)
		{
			if (bLeft)
			{
				GetCNCButton(m_mapLeftFixedBtn, iButton).LButtonDown();
				GetCNCButton(m_mapLeftFixedBtn, iButton).SetCheckStatus(TRUE);
			}
			else
			{
				GetCNCButton(m_mapRightFixedBtn, iButton).LButtonDown();
				GetCNCButton(m_mapRightFixedBtn, iButton).SetCheckStatus(TRUE);
			}
			if(m_pChildWnd)
			{
				m_pChildWnd->SetBtnCheckStatus(false);
			}
			SetFixBtnCheckStatus(iButton);
		}
    }
}

void CNewTBWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
    if(m_iCurrentPage == IDC_BTN_MAIN)
    {
		// 固定项按钮区域
		bool32 bLeft = false;
		int iButton = TFixedButtonHitTest(point, bLeft);
		if (INVALID_ID != iButton)
		{
			if (bLeft)
			{
				GetCNCButton(m_mapLeftFixedBtn, iButton).LButtonUp();
			}
			else
			{
				GetCNCButton(m_mapRightFixedBtn, iButton).LButtonUp();
			}
		}
    }
}

void CNewTBWnd::OnPaint() 
{
	CPaintDC dc(this);

	if (!m_bPaint)
	{
	//	InitToolbarButtons();
		m_bPaint = true;
	}

    DrawToolbar(dc);
}


CRect GetBtnRect(CRect rtParent, int32 iWidth, int32 iHeight, int32 iCols, int32 iIndex)
{
	CRect rtRet;

// 	int32 iTopSkip = (iIndex / iCols) * iHeight;
// 	int32 iLeftSkip = (iIndex % iCols) * iWidth;

	int32 iTopSkip = (iIndex % 2) * iHeight;
	int32 iLeftSkip = (iIndex / 2) * iWidth;

	rtRet = rtParent;
	
	rtRet.top  += iTopSkip;
	rtRet.bottom = rtRet.top + iHeight;

	rtRet.left += iLeftSkip;
	rtRet.right = rtRet.left + iWidth;

	return rtRet;
}


bool32 CNewTBWnd::DoOpenCfm( const CString &StrCfmXmlName , CString StrUrl)
{
	// 写死的需要授权的列表 是否可变更为cfm增加以属性需要服务器授权，如设置
	// 该属性，则向授权列表咨询是否有权限，无则打开失败(不过这样就外部可控了)
	//	就这样搞
	// 先检查权限
	T_CfmFileInfo cfm;
	const DWORD dwCmd = GetIoViewCmdIdByCfmName(StrCfmXmlName);
	if ( !CCfmManager::Instance().QueryUserCfm(StrCfmXmlName, cfm) && dwCmd == 0 )
	{
		::MessageBox(AfxGetMainWnd()->m_hWnd, _T("页面文件缺失，请确认是否已经安装!"), _T("错误"), MB_OK |MB_ICONERROR);
		return false;
	}
	
	//SetF7(FALSE);

	if ( dwCmd != 0 )
	{
		bool32 bOpen = LoadSystemDefaultCfm(StrCfmXmlName);	// 系统的，不用尝试了
		return bOpen;
	}
	else
	{
		return CCfmManager::Instance().LoadCfm(StrCfmXmlName, false, false, StrUrl) != NULL;	// 普通的
	}
}

bool32 CNewTBWnd::DoCloseCfm( const CString &StrCfmXmlName )
{
	// 是关闭呢，还是切换 系统默认(图表和报价表)不关，其它关闭
	const DWORD dwCmd = GetIoViewCmdIdByCfmName(StrCfmXmlName);
	if ( dwCmd != 0 )
	{
		CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		if ( NULL != pMainFrame  )
		{
			//pMainFrame->MDINext();
			CMDIChildWnd *pWnd = pMainFrame->MDIGetActive();
			CWnd  *pWndNext = pWnd;
			while ( NULL != pWndNext )
			{
				pWndNext = pWndNext->GetNextWindow();
				CMDIChildWnd *pChild = DYNAMIC_DOWNCAST(CMDIChildWnd, pWndNext);
				if ( NULL != pChild )
				{
					pMainFrame->MDIActivate(pChild);
					break;
				}
			}
		}
		return true;
	}
	return CCfmManager::Instance().CloseCfmFrame(StrCfmXmlName, true) > 0;
}

bool32 CNewTBWnd::LoadSystemDefaultCfm( DWORD iIoViewCmd, bool32 /*bCloseExistence = false*/ )
{
	int32 iPicId = 0;
	switch (iIoViewCmd)
	{
	case TB_IOCMD_REPORT:
		iPicId = ID_PIC_REPORT;
		break;
	case TB_IOCMD_TREND:
		iPicId = ID_PIC_TREND;
		break;
	case TB_IOCMD_KLINE:
		iPicId = ID_PIC_KLINE;
		break;
	case TB_IOCMD_NEWS:
		iPicId = ID_PIC_NEWS;
		break;
	case TB_IOCMD_REPORTARB:
		iPicId = ID_PIC_REPORT_ARBITRAGE;
		break;
	case TB_IOCMD_KLINEARB:
		iPicId = ID_PIC_KLINEARBITRAGE;
		break;
	case TB_IOCMD_TRENDARB:
		iPicId = ID_PIC_TRENDARBITRAGE;
		break;
	default:
		ASSERT( 0 );
		return false;
	}
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( NULL != pMainFrame && 0 != iPicId )
	{
		// 以当前激活窗口为商品源，加载或置前cfm
		return pMainFrame->LoadSystemDefaultCfm(iPicId, NULL, true);
	}
	return false;
}

bool32 CNewTBWnd::LoadSystemDefaultCfm( const CString &StrSDCName, bool32 bCloseExistence /*= false*/ )
{
	DWORD dwIoCmd = GetIoViewCmdIdByCfmName(StrSDCName);
	if ( dwIoCmd > 0 )
	{
		return LoadSystemDefaultCfm(dwIoCmd, bCloseExistence);
	}
	return false;
}

const std::pair<DWORD, CCfmManager::E_SystemDefaultCfm> sIoViewCmd2CfmName[] = {
	std::pair<DWORD, CCfmManager::E_SystemDefaultCfm>(TB_IOCMD_REPORT, CCfmManager::ESDC_Report),
		std::pair<DWORD, CCfmManager::E_SystemDefaultCfm>(TB_IOCMD_KLINE, CCfmManager::ESDC_KLine),
		std::pair<DWORD, CCfmManager::E_SystemDefaultCfm>(TB_IOCMD_TREND, CCfmManager::ESDC_Trend),
		std::pair<DWORD, CCfmManager::E_SystemDefaultCfm>(TB_IOCMD_NEWS, CCfmManager::ESDC_News),
		//{CNewTBWnd::TB_IOCMD_F10, _T("F10资料")},
};
const DWORD sIoViewCmd2CfmCount = sizeof(sIoViewCmd2CfmName)/sizeof(sIoViewCmd2CfmName[0]);

DWORD CNewTBWnd::GetIoViewCmdIdByCfmName( const CString &StrCfmName )
{
	CCfmManager::E_SystemDefaultCfm esdc = CCfmManager::GetESDC(StrCfmName);
	for ( int i=0; i < sIoViewCmd2CfmCount; i++ )
	{
		if ( sIoViewCmd2CfmName[i].second == esdc )
		{
			return sIoViewCmd2CfmName[i].first;
		}
	}
	return 0;
}

void CNewTBWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags, point);
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CNewTBWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);	

	if (nType == SIZE_MINIMIZED)
	{
		return;
	}

	// 为啥OnSize时候工具栏按钮要好久才刷新？暂时在这里处理下
	CPaintDC dc(this);
	DrawToolbar(dc);

	if ( NULL != m_pChildWnd )
	{
		CRect rcWindow;
		GetClientRect(rcWindow);

		CRect rc;
		if ( ::GetWindowRect(m_pChildWnd->m_hWnd, &rc) )
		{
			ScreenToClient(&rc);
			int iWndWidth = rcWindow.Width() - m_iRightFixedWidth - m_iLeftFixedWidth;
			::MoveWindow(m_pChildWnd->m_hWnd, m_iLeftFixedWidth, rc.top, iWndWidth, rc.Height(), TRUE);
			m_pChildWnd->RedrawWindow();
		}
	}
}

bool32 CNewTBWnd::GetTradeExePath(OUT CString &StrExePath, IN LPCTSTR pSec, IN  LPCTSTR pKey, bool32 bReselect)
{
	if (  CEtcXmlConfig::Instance().ReadEtcConfig(pSec, pKey, NULL, StrExePath) )
	{
		CFileFind FileFind;
		if ( FileFind.FindFile(StrExePath) )
		{
			//
		}
	}
	
	if ( !bReselect && !StrExePath.IsEmpty())
	{
		return true;
	}

	// 提示选择

	CString selectPath;
	if (ShowFileDlg(StrExePath, selectPath))
	{
		SetTradeExePath(pSec, pKey, selectPath);
		StrExePath = selectPath;	
		return true;
	}

	return false;
}

void CNewTBWnd::SetTradeExePath( LPCTSTR pSec, LPCTSTR pKey, LPCTSTR lpszExePath )
{
	CEtcXmlConfig::Instance().WriteEtcConfig(pSec, pKey, lpszExePath);
}

void CNewTBWnd::DoTradePlugin(int resId, bool32 bReselect)
{
	CString tSection, StrExePath ;
	GetTradeSection(resId, tSection);
	LPCTSTR pKey = KStrTradeKey;
	if ( GetTradeExePath(StrExePath, (LPCTSTR)tSection, pKey, bReselect)
		&& !StrExePath.IsEmpty() )
	{
		CFileFind FileFind;
		if ( FileFind.FindFile(StrExePath) )
		{
			FileFind.FindNextFile();
			STARTUPINFO stinfo = {0};
			stinfo.cb = sizeof(stinfo);
			PROCESS_INFORMATION pi = {0};
			
			CString StrDir = FileFind.GetFilePath();
			int iDirPos = StrDir.ReverseFind(_T('\\'));
			if ( iDirPos >= 0 )
			{
				StrDir = StrDir.Left(iDirPos);
			}
			else
			{
				ASSERT( 0 );
				StrDir.Empty();
			}
			
			LPCTSTR pszDir = StrDir.IsEmpty()? NULL : StrDir.operator LPCTSTR();
			
			if ( CreateProcess(StrExePath, NULL, NULL, NULL, FALSE, 0, NULL, pszDir, &stinfo, &pi) )
			{
				DEL_HANDLE(pi.hThread);
				DEL_HANDLE(pi.hProcess);
			}
			else
			{
				TCHAR szErr[100];
				_sntprintf(szErr, 100, _T("执行交易程序失败(%u)！"), GetLastError());
				MessageBox(szErr, _T("错误"));
				SetTradeExePath((LPCTSTR)tSection,pKey, NULL);	// 清除不能启动的交易配置
			}
		}
		else
		{
			MessageBox(_T("交易程序已不存在！"), _T("错误"));
			SetTradeExePath((LPCTSTR)tSection,pKey, NULL);	// 清除不能启动的交易配置
		}
	}
}

// 显示Im窗体
void CNewTBWnd::OnShowIM()
{
	m_blShowIM = true; // 给状态栏点击时用的

	CMainFrame* pWnd  = (CMainFrame*)AfxGetMainWnd();
	
	if ( NULL != pWnd && NULL != pWnd->m_pDlgIm)
	{
		if ( pWnd->m_pDlgIm->IsExist() )
		{
			pWnd->m_pDlgIm->MoveWindow(&m_RectIm, TRUE);
			pWnd->m_pDlgIm->ShowIm(m_blShowIM);
		}

	}
	/*
	CGGTongDoc* pDoc = AfxGetDocument();
	if ( NULL == pDoc || NULL == pDoc->m_pAbsCenterManager )
	{
		return;
	}

	if ( pDoc->m_pAbsCenterManager->GetMessageFlag() )  
		pWnd->m_pDlgIm->OpenMessageWindow();

	pDoc->m_pAbsCenterManager->SetMessageFlag(false);
	*/
}

// 隐藏Im窗体
void CNewTBWnd::OnHideIM()
{
	//m_blShowIM = false;
	CMainFrame* pWnd  = (CMainFrame*)AfxGetMainWnd();
	
	if ( NULL != pWnd && NULL != pWnd->m_pDlgIm)
	{
		if ( pWnd->m_pDlgIm->IsExist() )
		{
			pWnd->m_pDlgIm->MoveWindow(&m_RectIm, TRUE);
			pWnd->m_pDlgIm->ShowIm(false);
		}
	}
}

// 改变IM的底部大小给mainframe调整bottom时用
void CNewTBWnd::SetImBottom(int iBottom)
{
	if ( iBottom - 73 > 415 )
	{
		m_RectIm.bottom = m_RectIm.top + 415;
	}
	else
		m_RectIm.bottom = m_RectIm.top + iBottom - 98; //73


	if (m_blShowIM)
		OnShowIM();
//	else
//		OnHideIM();
}

// 移动IM窗体
void CNewTBWnd::MoveIm(int iLeftOffset, int iTopOffset, int iRightOffset, int iBottomOffset)
{
	m_RectIm.left   += iLeftOffset;
	m_RectIm.top    += iTopOffset;
	m_RectIm.right  += iLeftOffset;
	m_RectIm.bottom += iTopOffset;

	if (m_blShowIM)
		OnShowIM();
//	else
//		OnHideIM();
}


bool32 CNewTBWnd::GetCurrentWSP(CString &strWsp)
{
	CMPIChildFrame *pCurFrame = CCfmManager::Instance().GetCurrentCfmFrame();
	if ( NULL != pCurFrame )
	{
		T_CfmFileInfo cfm;
		if ( CCfmManager::Instance().QueryUserCfm(pCurFrame->GetIdString(), cfm) )
		{
			strWsp = cfm.m_StrXmlName;
			return true;
		}
	}
	return false;
}

void CNewTBWnd::SetF7(bool32 bShow)
{
	//if (IDC_BTN_MAIN == m_iCurrentPage)
	//{
	//	if (bShow)
	//	{
	//		m_mapHomeBtn[ID_HOME_FUNC_FULL].SetCaption(L"恢复");
	//	}
	//	else
	//	{
	//		m_mapHomeBtn[ID_HOME_FUNC_FULL].SetCaption(L"全屏");
	//	}
	//}
	//else
	//{
	//	if (bShow)
	//	{
	//		m_mapHomeBtn[ID_HOME_FUNC_FULL].SetCaption(L"恢复", false);
	//	}
	//	else
	//	{
	//		m_mapHomeBtn[ID_HOME_FUNC_FULL].SetCaption(L"全屏", false);
	//	}
	//}
}

void CNewTBWnd::ShowAllMarketMenu(CRect rect)
{
	CAbsCenterManager *pAbsCenterManager = AfxGetDocument()->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return;
	}


	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

	CNewMenu* pPopMenu = NULL;
	int32 iMenuId = 0;
	pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pMainFrame->m_sysMenu.GetSubMenu(iMenuId));

	int32 iCount = pPopMenu->GetMenuItemCount();
	for ( int32 iDel=0; iDel < iCount ; iDel++  )
	{
		pPopMenu->RemoveMenu(0, MF_BYPOSITION);		// 清除以前的菜单项目
	}

	m_mapMenuID.clear();

	// 菜单
	T_BlockMenuInfo  BlockMenuInfoToAdd;
	int32 i, iSize = pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize();		

	int32 iMenuID = IDM_BLOCKMENU_BEGIN;

	if (iSize <= 0)
	{
		return;
	}


 	// 物理市场
	bool32 bShangHaiZhengQuan = (pAbsCenterManager->GetMerchManagerType() & 1) != 0;

	//
	for (int32 iIndex = 0 ;iIndex<iSize; iIndex++ )
	{
		CString StrBreedName= pAbsCenterManager->GetMerchManager().m_BreedListPtr[iIndex]->m_StrBreedCnName;
		CNewMenu* SubPop =pPopMenu->AppendODPopupMenu(StrBreedName);

		//////////////////////////////////////////////////////////////////////////
		if (StrBreedName == L"香港证券" || StrBreedName == L"新加坡期货" || StrBreedName == L"全球指数")
		{
			pPopMenu->AppendODMenu(L"",MF_SEPARATOR);
		}

		CBreed* pBreed = pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetAt(iIndex);
		ASSERT( NULL != pBreed);

		for (i = 0; i<pBreed->m_MarketListPtr.GetSize(); i++)
		{
			CMarket * pMarket = pBreed->m_MarketListPtr.GetAt(i);
			ASSERT( NULL!= pMarket); 

			CString StrName = pMarket->m_MarketInfo.m_StrCnName;
			SubPop->AppendODMenu(StrName, MF_STRING, iMenuID);
			m_mapMenuID[iMenuID] = pMarket->m_MarketInfo.m_iMarketId;

			iMenuID++;
		}	
	}	

	// 增加逻辑板块
	if ( bShangHaiZhengQuan && CConfigInfo::FlagCaiFuJinDunQiHuo != CConfigInfo::Instance()->GetVersionFlag() )
	{
		CBlockConfig::IdArray aColIds;
		CBlockConfig::Instance()->GetCollectionIdArray(aColIds);
		CNewMenu *pLogicMenu = NULL;
		if ( aColIds.GetSize() > 0 )
		{
			pLogicMenu = pPopMenu->AppendODPopupMenu(_T("股票板块"));
			ASSERT( NULL != pLogicMenu );
		}

		for ( int i=0; i < aColIds.GetSize() && pLogicMenu != NULL ; i++ )
		{
			CBlockCollection *pCol = CBlockConfig::Instance()->GetBlockCollectionById(aColIds[i]);
			CBlockConfig::BlockArray aBlocks;
			pCol->GetValidBlocks(aBlocks);

			if ( aBlocks.GetSize() > 0 
				&& 
				(aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeNormalBlock				// 普通 || 分类
				|| aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeMarketClassBlock )
				)
			{
				CNewMenu *pColMenu = pLogicMenu->AppendODPopupMenu(pCol->m_StrName);
				ASSERT( NULL != pColMenu );
				for ( int iBlock=0; iBlock < aBlocks.GetSize() ; iBlock++ )
				{
					CString StrItem = aBlocks[iBlock]->m_blockInfo.m_StrBlockName;
					if ( StrItem.GetLength() > 7 )
					{
						StrItem = StrItem.Left(6) + _T("...");
					}

					pColMenu->AppendODMenu(StrItem, MF_STRING, iMenuID);
					m_mapMenuID[iMenuID] = aBlocks[iBlock]->m_blockInfo.m_iBlockId;

					iMenuID++;
				}
				MultiColumnMenu(*pColMenu, 20);
			}
		}
	}

	// 期货板块，如果存在的话
	{
		CBlockConfig::IdArray aColIds;
		CBlockConfig::Instance()->GetCollectionIdArray(aColIds);
		CNewMenu *pLogicMenu = NULL;
		bool32 bFuture = FALSE;
		for ( int i=0; i < aColIds.GetSize() ; ++i )
		{
			CBlockCollection *pCol = CBlockConfig::Instance()->GetBlockCollectionById(aColIds[i]);
			CBlockConfig::BlockArray aBlocks;
			pCol->GetValidBlocks(aBlocks);
			if ( aBlocks.GetSize() > 0 
				&& aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeFutureClassBlock				// 期货 )
				)
			{
				bFuture = true;
				break;
			}
		}

		if ( bFuture && CConfigInfo::FlagCaiFuJinDunGuPiao != CConfigInfo::Instance()->GetVersionFlag() )
		{
			pLogicMenu = pPopMenu->AppendODPopupMenu(_T("期货板块"));
			ASSERT( NULL != pLogicMenu );
			for ( int i=0; i < aColIds.GetSize() && pLogicMenu != NULL ; i++ )
			{
				CBlockCollection *pCol = CBlockConfig::Instance()->GetBlockCollectionById(aColIds[i]);
				CBlockConfig::BlockArray aBlocks;
				pCol->GetValidBlocks(aBlocks);
				if ( aBlocks.GetSize() > 0 
					&& aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeFutureClassBlock				// 期货 )
					)
				{
					CNewMenu *pColMenu = pLogicMenu->AppendODPopupMenu(pCol->m_StrName);
					ASSERT( NULL != pColMenu );
					for ( int iBlock=0; iBlock < aBlocks.GetSize() ; iBlock++ )
					{
						CString StrItem = aBlocks[iBlock]->m_blockInfo.m_StrBlockName;
						if ( StrItem.GetLength() > 7 )
						{
							StrItem = StrItem.Left(6) + _T("...");
						}

						pColMenu->AppendODMenu(StrItem, MF_STRING, iMenuID);
						m_mapMenuID[iMenuID] = aBlocks[iBlock]->m_blockInfo.m_iBlockId;

						iMenuID++;
					}

					MultiColumnMenu(*pColMenu, 20);
				}
			}
		}
	}

	pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,rect.left, rect.bottom - 39, AfxGetMainWnd());
}

int32 CNewTBWnd::GetMenuMarketID(int32 iMenuID)
{
	int32 iRet = -1;
	
	if (m_mapMenuID.empty())
	{
		return iRet;
	}

	//
	iRet = m_mapMenuID[iMenuID];
	return iRet;
}

bool32 CNewTBWnd::InitialCurrentWSP()
{
    CString StrCurCfmName;
    CString StrName;
    if (!GetCurrentWSP(StrCurCfmName) )
    {
        return FALSE;
    }

    // 按钮区
    //map<int, CNCButton>::iterator iter;
    //for (iter=m_mapAnalyBtn.begin(); iter!=m_mapAnalyBtn.end(); ++iter)
    //{
    //    CNCButton &btn = iter->second;
    //    StrName = btn.GetCaption();
    //    if (0 == StrCurCfmName.CompareNoCase(StrName))
    //    {
    //        btn.SetCheckStatus(TRUE);
    //    }
    //    else if (btn.GetCheck())
    //    {
    //        btn.SetCheckStatus(FALSE);
    //    }
    //}

    return true;
}

LRESULT CNewTBWnd::OnIdleUpdateCmdUI( WPARAM w, LPARAM l )
{
	// 应该考虑标志过滤掉不用的时机
	CFrameWnd* pTarget = (CFrameWnd*)GetOwner();
	if (pTarget == NULL || !pTarget->IsFrameWnd())
		pTarget = GetParentFrame();
	if (pTarget != NULL && (IDC_BTN_ANALYSIS == m_iCurrentPage))
	{
		InitialCurrentWSP();
	}

	if (pTarget != NULL)
	{
		CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		CMPIChildFrame *pActiveFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pMainFrame->MDIGetActive());
		if ( NULL != pActiveFrame && pActiveFrame->IsF7AutoLock() )
		{
			SetF7(1);
		}
		else
		{
			SetF7(0);
		}
	}
	
	return 0;
}

void CNewTBWnd::InitToolbarButtons()
{
    CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
    
	if(NULL == pApp)
    {
        return;
    }

    std::vector<T_ColumnData> vecColumnData = pApp->m_pConfigInfo->m_vecColumnData;
    int colSize = vecColumnData.size();
    if(colSize <= 0)
    {
        m_mapBtn.clear();
		m_mapLeftFixedBtn.clear();
		m_mapRightFixedBtn.clear();
        return;
    }

    m_mapBtn.clear();
	m_mapLeftFixedBtn.clear();
	m_mapRightFixedBtn.clear();
	m_mapRealTrad.clear();
    int iXStartPos = pApp->m_pConfigInfo->m_toolBarData.m_iLeftSpace;
	m_iLeftFixedWidth  = iXStartPos;
	m_iRightFixedWidth = 5;
    int iYStartPos = pApp->m_pConfigInfo->m_toolBarData.m_iTopSpace;
    T_ToolBarData toobarData = pApp->m_pConfigInfo->m_toolBarData;
    int iBtnTotalHeight = toobarData.m_iHeight - toobarData.m_iTopSpace - toobarData.m_iBottomSpace;
	m_ptToolButtonLeftInset.SetPoint( pApp->m_pConfigInfo->m_toolBarData.m_iLeftSpace,pApp->m_pConfigInfo->m_toolBarData.m_iTopSpace); 
    for (int Index = 0; Index < colSize ; Index++)
    {
        T_ColumnData tmpColData = vecColumnData[Index];
		
		// 
		switch(tmpColData.m_eFixedType)
		{
		case EFT_NONE:
			{
				AddToolbarButton(tmpColData, iXStartPos, iYStartPos, iBtnTotalHeight, EFT_NONE);
			}
			break;
		case EFT_LEFT:
			{
				AddToolbarButton(tmpColData, m_iLeftFixedWidth, iYStartPos, iBtnTotalHeight, EFT_LEFT);
			}
			break;
		case EFT_RIGHT:
			{
				AddToolbarButton(tmpColData, m_iRightFixedWidth, iYStartPos, iBtnTotalHeight, EFT_RIGHT);
			}
			break;
		default:
			break;
		}
    }	
}

void CNewTBWnd::InitFont()
{
    CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
    if(NULL == pApp)
    {
        return;
    }

    TCHAR TStrName[FONT_NAME_LENGTH];
    memset(TStrName,0,sizeof(TStrName));
    lstrcpy(TStrName, pApp->m_pConfigInfo->m_toolBarData.m_SimpleBtnFont);
    m_fontSimpleBtn.m_StrName = gFontFactory.GetExistFontName(TStrName);      //.一般按钮的字体
    CString strSize = pApp->m_pConfigInfo->m_toolBarData.m_SimpleFontSize;
    m_fontSimpleBtn.m_Size    = IsAllNumbers(strSize)?_ttoi(strSize):10;
    m_fontSimpleBtn.m_iStyle  = FontStyleRegular;

    memset(TStrName,0,sizeof(TStrName));
    lstrcpy(TStrName, pApp->m_pConfigInfo->m_toolBarData.m_CategoryBtnFont);
    m_fontCategoryBtn.m_StrName = gFontFactory.GetExistFontName(TStrName);   //分类按钮的字体
    strSize = pApp->m_pConfigInfo->m_toolBarData.m_CategoryFontSize;
    m_fontCategoryBtn.m_Size     = IsAllNumbers(strSize)?_ttoi(strSize):10;;
    m_fontCategoryBtn.m_iStyle  = FontStyleRegular;

}

void CNewTBWnd::DrawToolbar( CDC& dc )
{
    CDC memDC;
    memDC.CreateCompatibleDC(&dc);
    CBitmap bmp;
    CRect rcWindow;
    GetWindowRect(&rcWindow);
    rcWindow.OffsetRect(-rcWindow.left, -rcWindow.top);

    int iToolBarHeight = 100;
    CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();


	if (NULL == pApp)
	{
		return;
	}

    if(pApp->m_pConfigInfo->m_toolBarData.m_pBkImage)
    {
        iToolBarHeight = pApp->m_pConfigInfo->m_toolBarData.m_iHeight;
    }

    bmp.CreateCompatibleBitmap(&dc, rcWindow.Width(), iToolBarHeight);
    memDC.SelectObject(&bmp);
    memDC.SetBkMode(TRANSPARENT);
    Gdiplus::Graphics graphics(memDC.GetSafeHdc());

    if(pApp->m_pConfigInfo->m_toolBarData.m_pBkImage)
    {
        // 工具栏背景
        RectF destRect;
        destRect.X = 0;
        destRect.Y = 0;
        destRect.Width  = (REAL)rcWindow.Width();
        destRect.Height = (REAL)iToolBarHeight;



		ImageAttributes  imAtt;
		imAtt.SetWrapMode(WrapModeTile);    //拉伸图片
        graphics.DrawImage(pApp->m_pConfigInfo->m_toolBarData.m_pBkImage, destRect, 0, 0, pApp->m_pConfigInfo->m_toolBarData.m_pBkImage->GetWidth(), iToolBarHeight, UnitPixel,&imAtt);
    }

    CRect rcPaint;
    dc.GetClipBox(&rcPaint);
    buttonContainerType::iterator iter;
    CRect rcControl;

	/*
		//以下代码为裁剪区域控制按钮区域
		CRgn rgn;
		rgn.CreateRectRgn(rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
		for (iter = m_mapControlBtn.begin(); iter != m_mapControlBtn.end(); ++iter)
		{
		CRgn rgnChild;
		CRect rt;
		iter->second.GetRect(rt);
		rgnChild.CreateRectRgn(rt.left, 0, rt.right, rcWindow.bottom);
		rgn.CombineRgn(&rgn, &rgnChild, RGN_DIFF);
		rgnChild.DeleteObject();
		}

		memDC.SelectClipRgn(&rgn);
		//do something
		memDC.SelectClipRgn(NULL);
		rgn.DeleteObject();
	*/

	// 左边固定项按钮
	FOR_EACH(m_mapLeftFixedBtn, iter)	
	{
		(*iter).DrawButton(&graphics);
	}

	// 右边固定项按钮位置适配
	buttonContainerType::iterator it = m_mapRightFixedBtn.begin();
	CRect rcFixed(0, 0, 0, 0);
	if (it != m_mapRightFixedBtn.end())
	{
		(*it).GetRect(rcFixed);
	}

	FOR_EACH(m_mapRightFixedBtn, iter)	
	{
		CNCButton &btnControl = *iter;
		btnControl.GetRect(rcControl);  
		int iWidth = rcControl.Width();
		rcControl.OffsetRect(-rcFixed.left, 0);
		rcControl.left  += rcWindow.right - m_iRightFixedWidth;
		rcControl.right = rcControl.left + iWidth;
		btnControl.SetRect(rcControl);

		(*iter).DrawButton(&graphics);
	}
	
	
    dc.BitBlt(0, 0, rcWindow.Width(), iToolBarHeight, &memDC, 0, 0, SRCCOPY);
    dc.SelectClipRgn(NULL);
    memDC.DeleteDC();
    bmp.DeleteObject();
}

void CNewTBWnd::AddNcButton( LPRECT lpRect, const T_ButtonData&btnData, bool bCategory /*= false*/, UINT nCount/*=3*/, E_FixedType eFixedType/*=EFT_NONE*/ )
{
	CRect rt;
	GetClientRect(&rt);
    CNCButton btnControl;
    btnControl.CreateButton(btnData.m_Text, lpRect, this, btnData.m_pBkImage, nCount, btnData.m_ID);
    btnControl.SetClientDC(TRUE);
	btnControl.SetGroupId(btnData.m_iGroupId);
	btnControl.SetShowCaption(btnData.m_bShowText);
    int iPos = btnData.m_TextPos.Find(':');
    if(iPos != -1)
    {
        CString strXOffset = btnData.m_TextPos.Left(iPos);
        CString strYOffset = btnData.m_TextPos.Right(btnData.m_TextPos.GetLength() - iPos - 1);
        int iXOffset = IsAllNumbers(strXOffset)? _ttoi(strXOffset):0;
        int iYOffset = IsAllNumbers(strYOffset)? _ttoi(strYOffset):0;
        CPoint pointOffset(iXOffset, iYOffset);
        btnControl.SetTextOffPos(pointOffset);
    }
	
	int iAreaPos = btnData.m_AreaPos.Find(',');
	int iOffset = 0;
	
	if ( -1 != iAreaPos )
	{
		std:string strAreaPos = _Unicode2MultiChar(btnData.m_AreaPos);
		istringstream is(strAreaPos);
		
		std::vector<int> vecInt;
		char ch;
		while (is >> iOffset)
		{
			vecInt.push_back(iOffset);
			is >> ch;
		}
		
		if (4 == vecInt.size())
		{
			CRect rcAreaOffset(vecInt.at(0), vecInt.at(1), vecInt.at(2), vecInt.at(3));
			btnControl.SetAreaOffset(rcAreaOffset);
		}
	}

    if(bCategory)
    {
        if(!m_fontCategoryBtn.m_StrName.IsEmpty())
        {
            btnControl.SetFont(m_fontCategoryBtn);
        }
        btnControl.SetTextVertical();
        btnControl.SetTextColor(m_crlThirdTextNor, m_crlThirdTextHot, m_crlThirdTextPress);
        btnControl.EnableButton(FALSE);
       /* btnControl.SetChangeTextColor(false);*/
    }
    else
    {
        if(!m_fontSimpleBtn.m_StrName.IsEmpty())
        {
            btnControl.SetFont(m_fontSimpleBtn);
        }
        btnControl.SetTextColor(btnData.m_crlTextColor, btnData.m_crlTextColor, btnData.m_crlTextColor);
    }

	if (!CRect().SubtractRect(&rt, lpRect))
	{
		btnControl.SetNotCreate();
	}

	if(btnData.m_EventType == L"4" && btnData.m_EventParam == L"0")
	{
		btnControl.SetCheckStatus(TRUE);
	}
	else
	{
		btnControl.SetCheckStatus(FALSE);
	}

	//
	switch(eFixedType)
	{
	case EFT_NONE:
		{
			btnControl.SetParentWnd(m_pChildWnd);
			m_mapBtn.push_back(btnControl);
		}
		break;
	case EFT_LEFT:
		{
			m_mapLeftFixedBtn.push_back(btnControl);
		}
		break;
	case EFT_RIGHT:
		{
			m_mapRightFixedBtn.push_back(btnControl);
		}
		break;
	default:
		break;
	}
}

void CNewTBWnd::ShowMenu( T_ButtonData btnData, int id )
{
    if (NULL == m_pDlgPullDown)
    {
        //ASSERT(0);
        return;
    }

    if (m_pDlgPullDown->IsWindowVisible())
    {
        m_pDlgPullDown->ShowWindow(FALSE);
        return;
    }
	
	CGGTongApp *pApp = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());
	
	if (!pApp)
	{
		return;
	}

	bool bCreateSubMenu = false;
	
	if (!m_vecDlgSubPullDown.empty())
	{
		if (btnData.m_Text == m_pDlgPullDown->GetParentTitle())
		{
			bCreateSubMenu = true;
		}
	}

    int iHeight = 0;
    int iWidth = 0;
	int iTop = 0;
    int iBottom = 0;
    Image* pImage = NULL, *pImgStatus = NULL;	

    m_mapPullDown.clear();
    int chlSize = btnData.m_ChildBtnData.size();

	CRect rcMore;
	GetCNCButton(m_mapBtn, id).GetRect(rcMore);
	ClientToScreen(&rcMore);
	
	int iBtnWidth = rcMore.Width();
	rcMore.left += m_iLeftFixedWidth;
	rcMore.right = rcMore.left + iBtnWidth;

    for (int32 i=0; i<chlSize; i++)
    {
        T_ChildButtonData tempChlData = btnData.m_ChildBtnData[i];

        CNCButton btnNow;
        pImage = tempChlData.m_pBkImage;
		pImgStatus = tempChlData.m_pImgStatus;

        if(pImage)
        {
            iWidth = pImage->GetWidth();
            iHeight = pImage->GetHeight()/3;
        }
        else
        {
            iWidth = 90;
            iHeight = 31;
        }

        iBottom += iHeight;
        CRect rtNow(0, iTop, iWidth, iBottom);
        btnNow.CreateButton(tempChlData.m_Text,  rtNow, m_pDlgPullDown, pImage, 3, tempChlData.m_ID, NULL, pImgStatus, tempChlData.m_EventParam);
        btnNow.SetGroupId(tempChlData.m_iGroupId);
		btnNow.SetShowCaption(tempChlData.m_bShowText);
		if(!m_fontSimpleBtn.m_StrName.IsEmpty())
        {
			if (0. != tempChlData.m_fFontSize)
			{
				m_fontSimpleBtn.m_Size = tempChlData.m_fFontSize;
			}
			else
			{
				CString strSize           = pApp->m_pConfigInfo->m_toolBarData.m_SimpleFontSize;
				m_fontSimpleBtn.m_Size    = IsAllNumbers(strSize)?_tstof(strSize):10;
			}

            btnNow.SetFont(m_fontSimpleBtn);
        }

        if(NULL == pImage)
        {
            btnNow.SetTextBkgColor(COLOR_NORMAL, COLOR_HOVER, COLOR_SELECT);
        }
		btnNow.SetTextColor(tempChlData.m_crlTextColor, tempChlData.m_crlTextColor, Color(70,70,230));


		// 如果子菜单已经创建，那么只需要调整子菜单的位置即可
		if (bCreateSubMenu)
		{
			CWnd *pSubMenu = AdjustSubMenuPos(btnNow.GetCaption(), (rcMore.left+iWidth + 1), iTop + rcMore.bottom - 5);
			if (pSubMenu)
			{
				btnNow.AddChildMenu(pSubMenu);
			}
		}
		else// 创建子菜单
		{
			CWnd *pSubMenu = CreateSubMenu(tempChlData, (rcMore.left+iWidth + 1), iTop + rcMore.bottom - 5);
			if (pSubMenu)
			{
				btnNow.AddChildMenu(pSubMenu);
			}
		}

		m_mapPullDown.push_back(btnNow);
		iTop += iHeight;
    }

    int iX = rcMore.left+5;
    int iY = rcMore.bottom -5;
    CRect rcWnd(iX, iY, iX + iWidth, iY + iBottom);

	m_pDlgPullDown->SetParentTitle(btnData.m_Text);
	m_pDlgPullDown->MoveWindow(rcWnd);
    m_pDlgPullDown->SetShowButtons(m_mapPullDown, rcWnd);
   
    m_pDlgPullDown->ShowWindow(TRUE);
}

bool CNewTBWnd::FindBtnData( int id, int& iBtnDataIndex, int& iChlIndex, int& iColumnIndex, bool& bChildBtn,bool &bCol)
{
    CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
    if(NULL == pApp)
    {
        return false;
    }

    int ArrColSize = pApp->m_pConfigInfo->m_vecColumnData.size();
    if(ArrColSize <= 0)
    {
        return false;
    }


    for (int colIndex = 0; colIndex < ArrColSize; colIndex++)
    {
        T_ColumnData tempColData = pApp->m_pConfigInfo->m_vecColumnData.at(colIndex);
        if(tempColData.m_ID == id)
        {
            iColumnIndex = colIndex;
            bChildBtn = false;
            bCol = true;
            return true;
        }

        int btnDataSize = tempColData.m_BtnData.size();
        if(btnDataSize <= 0)
        {
            continue;
        }

        for (int btnIndex = 0; btnIndex < btnDataSize; btnIndex++)
        {
            T_ButtonData tmpBtnData =tempColData.m_BtnData.at(btnIndex);
            if(tmpBtnData.m_ID == id)
            {
                iBtnDataIndex = btnIndex;
                iColumnIndex = colIndex;
                bChildBtn = false;
                bCol = false;
                return true;
            }

            int chlBtnSize = tmpBtnData.m_ChildBtnData.size();
            if(chlBtnSize <= 0)
            {
                continue;
            }

            for(int childIndex = 0; childIndex < chlBtnSize; childIndex++)
            {
                T_ChildButtonData tmpChlBtnData = tmpBtnData.m_ChildBtnData.at(childIndex);
                if(tmpChlBtnData.m_ID == id)
                {
                    iChlIndex = childIndex;
                    iBtnDataIndex = btnIndex;
                    iColumnIndex = colIndex;
                    bCol = false;
                    bChildBtn = true;
                    return true;
                }
            }
        }
    }

    return false;
}

bool CNewTBWnd::FindSubBtnData(int id, int& iBtnDataIndex, int& iChlIndex,int&iSubChlIndex , int& iColumnIndex, bool& bChildBtn,bool &bCol)
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if(NULL == pApp)
	{
		return false;
	}

	int ArrColSize = pApp->m_pConfigInfo->m_vecColumnData.size();
	if(ArrColSize <= 0)
	{
		return false;
	}


	for (int colIndex = 0; colIndex < ArrColSize; colIndex++)
	{
		T_ColumnData tempColData = pApp->m_pConfigInfo->m_vecColumnData.at(colIndex);
		if(tempColData.m_ID == id)
		{
			iColumnIndex = colIndex;
			bChildBtn = false;
			bCol = true;
			return true;
		}

		int btnDataSize = tempColData.m_BtnData.size();
		if(btnDataSize <= 0)
		{
			continue;
		}

		for (int btnIndex = 0; btnIndex < btnDataSize; btnIndex++)
		{
			T_ButtonData tmpBtnData =tempColData.m_BtnData.at(btnIndex);
			if(tmpBtnData.m_ID == id)
			{
				iBtnDataIndex = btnIndex;
				iColumnIndex = colIndex;
				bChildBtn = false;
				bCol = false;
				return true;
			}

			int chlBtnSize = tmpBtnData.m_ChildBtnData.size();
			if(chlBtnSize <= 0)
			{
				continue;
			}

			for(int childIndex = 0; childIndex < chlBtnSize; childIndex++)
			{
				T_ChildButtonData tmpChlBtnData = tmpBtnData.m_ChildBtnData.at(childIndex);

				if(tmpChlBtnData.m_ID == id)
				{
					iChlIndex = childIndex;
					iBtnDataIndex = btnIndex;
					iColumnIndex = colIndex;
					bCol = false;
					bChildBtn = true;
					return true;
				}

				int iSubChlBtnSize = tmpChlBtnData.m_ChildSubMenuData.size();
				if(iSubChlBtnSize <= 0)
				{
					continue;
				}

				for(int SubChildIndex = 0; SubChildIndex < iSubChlBtnSize; SubChildIndex++)
				{
					T_ChildButtonData SubChlBtnData = tmpChlBtnData.m_ChildSubMenuData.at(SubChildIndex);

					if(SubChlBtnData.m_ID == id)
					{
						iChlIndex = childIndex;
						iSubChlIndex = SubChildIndex;
						iBtnDataIndex = btnIndex;
						iColumnIndex = colIndex;
						bCol = false;
						bChildBtn = true;
						return true;
					}
				}
			}
		}
	}

	return false;
}

BOOL CNewTBWnd::OnSpecialFunc( CString strEventParam, WPARAM wParam, LPARAM lParam )
{
    int iEventFunc = IsAllNumbers(strEventParam)?_ttoi(strEventParam):-1;
    if(iEventFunc == -1)
    {
        return CWnd::OnCommand(wParam, lParam);
    }

    CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
    switch(iEventFunc)
    {
    case FType_Home_Page:                 //返回首页
        {
            if(pMain)
            {
                pMain->OnBackToHome();
            }
        }
        break;

    case FType_Back:                      //返回上一步
        {
            if(pMain)
            {
                pMain->OnBackToUpCfm();
            }
        }
        break;

    case FType_Full_Screen:               //全屏切换
        {
            if(pMain)
            {
                pMain->OnProcessF7();
            }
        }
        break;

    case FType_Self_Select_Stock:         //自选股
        {
            if(pMain)
            {
                pMain->OnHotKeyZXG();
            }
        }
        break;

    case FType_Internal_Real_Trade:       //内置实盘交易 
        {
            if (pMain)
            {
                pMain->m_bQuickOrderDn = FALSE;
                pMain->PostMessage(WM_COMMAND, UM_FIRM_TRADE_SHOW);
            }
        }
        break;

    case FType_UserDefine_Real_Trade:     //外挂实盘交易
        {
			//非右键触发询问是否出"您可右键重新选择交易实盘"对话框
			if (lParam != 1)
			{
				CString tSec, tPath;
				GetTradeSection(wParam, tSec);
				CEtcXmlConfig::Instance().ReadEtcConfig((LPCTSTR)tSec, KStrTradeKey, NULL, tPath);//读取改节点下的URL信息
				if (tPath.IsEmpty())
				{
					ShowTradeFirstNotify();
				}				
			}
			
			//实盘交易路径选择，或者执行实盘交易程序
            DoTradePlugin(wParam, false);
        }
        break;

    case FType_About_Us:                  //关于我们
        {
            CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
            if(pApp)
            {
                pApp->OnAppAbout();
            }
            
        }
        break;

    case FType_About_Arbitrage:            //套利
        {
            if (pMain)
            {
                pMain->OnArbitrage();
            }
        }
        break;

    case FType_About_Alarm:                //预警
        {
            if (pMain)
            {
                pMain->ShowConditionWarning();
            }
        }
        break;

    default:
        break;
    }

    return CWnd::OnCommand(wParam, lParam);
}

BOOL CNewTBWnd::OnIndexChangeFunc(CString strEventParam, WPARAM wParam, LPARAM lParam)
{
	//
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	
	if ( !pMainFrm )
	{
		return CWnd::OnCommand(wParam, lParam);
	}

	// 大盘战略抄底、大盘战略逃顶及牛熊分布打开大势研判版面
	bool32 bDaShiYanPan = false;
	if (L"大盘战略抄底" == strEventParam || L"大盘战略逃顶" == strEventParam
		|| L"牛熊分布" == strEventParam)
	{
		DoOpenCfm(L"大势研判");
		bDaShiYanPan = true;
	}
	
	if ( !strEventParam.IsEmpty() )
	{
		CIoViewKLine *pIoKLine = (CIoViewKLine *)pMainFrm->GetIoViewByPicMenuIdAndActiveIoView(ID_PIC_KLINE, false, true, false, true, false, true);
		if (!pIoKLine)// 如果没有找到，那么就创建该对象(注意：第二个参数标示创建的意思)
		{
			CHotKey *pNewHotKey = new CHotKey;
			pNewHotKey->m_eHotKeyType = EHKTIndex;
			pNewHotKey->m_StrParam1 = strEventParam;
			pMainFrm->PostMessage(UM_HOTKEY, (WPARAM)pNewHotKey, 0);
		}
		else
		{
			if ( IsWindow(pIoKLine->GetSafeHwnd()) )
			{	
				bool32 bClearOldIndex = false;//pItem->uBtnId != TB_RiseFallView;	// 牛熊分界不需要变更指标
				{
					// 当前指标已存在，直接返回
					bool32 bExist = pIoKLine->CheckIndexExist(strEventParam);					
					if ( bExist )
					{
						if ( !bDaShiYanPan )
						{
							pIoKLine->ReplaceIndex(strEventParam, _T("MA"), false, true);
						}
						
						return CWnd::OnCommand(wParam, lParam);
					}
				
					// 让K线自己选择切换商品否
					pIoKLine->AddShowIndex(strEventParam, bClearOldIndex, true, true);
					pIoKLine->BringToTop();
				}
			}
		}
	}
	
	return CWnd::OnCommand(wParam, lParam);
}

BOOL CNewTBWnd::OnCustomFunc(CString strEventParam, WPARAM wParam, LPARAM lParam)
{
	int iLeftPos = strEventParam.Find('(');
	int iRightPos = strEventParam.Find(')');
	
	CString StrSelType = _T(""), StrSelParam = _T("");
	int  iSlectId = 0;

	if (-1 != iLeftPos && -1 != iRightPos )
	{
		int iCount = iRightPos - iLeftPos - 1;
		StrSelType = strEventParam.Left(iLeftPos);
		StrSelParam = strEventParam.Mid(iLeftPos + 1, iCount);
	}
	else if (!strEventParam.IsEmpty())
	{
		StrSelType = strEventParam;
	}
	else
	{
		return CWnd::OnCommand(wParam, lParam); 
	}

	CMainFrame *pMainFrm = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	
	if (L"message" == StrSelType)
	{
		if (pMainFrm)
		{
			pMainFrm->PostMessage(UM_ToolbarCommand, (WPARAM)ID_DELEGATE_TRADE, (LPARAM)0);
		}
	}
	else if (L"imchat" == StrSelType)
	{
		UrlParser urlPaser(L"alias://onlineIm");
		ShellExecute(0, L"open", urlPaser.GetUrl(), NULL, NULL, SW_NORMAL);
	}
	else if (L"drawline" == StrSelType)
	{
		if (pMainFrm)
		{
			pMainFrm->OnSelfDraw();
		}
	}
	else if (L"智能选股" == StrSelType)
	{
		if (pMainFrm)
		{
			pMainFrm->m_iSlectModelId = _ttoi(StrSelParam);
		}
		DoOpenCfm(StrSelType);	
	}
	else if (L"cycle" == StrSelType)
	{
		if (L"fs" == StrSelParam)
		{
			DoOpenCfm(L"分时走势");
		}
		else if (L"more" == StrSelParam)
		{
			DoOpenCfm(L"多周期同列");
		}
		else
		{
			DoOpenCfm(L"K线分析");
			if (L"kx01" == StrSelParam)
			{
				KLineCycleChange(IDM_CHART_KMINUTE);
			}
			else if (L"kx05" == StrSelParam)
			{
				KLineCycleChange(IDM_CHART_KMINUTE5);
			}
			else if (L"kx15" == StrSelParam)
			{
				KLineCycleChange(IDM_CHART_KMINUTE15);
			}
			else if (L"kx30" == StrSelParam)
			{
				KLineCycleChange(IDM_CHART_KMINUTE30);
			}
			else if (L"kx60" == StrSelParam)
			{
				KLineCycleChange(IDM_CHART_KMINUTE60);
			}
			else if (L"kxday" == StrSelParam)
			{
				KLineCycleChange(IDM_CHART_KDAY);
			}
			else if (L"kxmonth" == StrSelParam)
			{
				KLineCycleChange(IDM_CHART_KMONTH);
			}
			else if (L"kxquarterly" == StrSelParam)
			{
				KLineCycleChange(IDM_CHART_KQUARTER);
			}
			else if (L"kxyear" == StrSelParam)
			{
				KLineCycleChange(IDM_CHART_KYEAR);
			}

		}
	}
	else if(L"fs" == StrSelType || L"kline" == StrSelType || L"fskline" == StrSelType)
	{
		CIoViewDuoGuTongLie *pIoViewDuoGuTongLie = NULL;
		CMPIChildFrame *pActiveFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pMainFrm->MDIGetActive());
		CString StrWspName = _T("多股同列");

		if (NULL != pActiveFrame)
		{
			if(pActiveFrame->GetIdString() != StrWspName)
			{
				bool32 bLoad = DoOpenCfm(StrWspName);
				if ( bLoad )
				{
					pIoViewDuoGuTongLie = pMainFrm->FindIoViewDuoGuTongLie(true);
				}
			}
			else
			{
				pIoViewDuoGuTongLie = pMainFrm->FindIoViewDuoGuTongLie(true);
			}
		}
		if(pIoViewDuoGuTongLie)
		{
			if(L"fs" == StrSelType)
			{
				if(L"4" == StrSelParam)
				{
					pIoViewDuoGuTongLie->ChangeDuoGuTongLieType(CIoViewDuoGuTongLie::EDGLT_TREND, 2, 2);
				}
				else if(L"9" == StrSelParam)
				{
					pIoViewDuoGuTongLie->ChangeDuoGuTongLieType(CIoViewDuoGuTongLie::EDGLT_TREND, 3, 3);
				}
			}
			else if(L"kline" == StrSelType)
			{
				if(L"4" == StrSelParam)
				{
					pIoViewDuoGuTongLie->ChangeDuoGuTongLieType(CIoViewDuoGuTongLie::EDGLT_KLINE, 2, 2);
				}
				else if(L"9" == StrSelParam)
				{
					pIoViewDuoGuTongLie->ChangeDuoGuTongLieType(CIoViewDuoGuTongLie::EDGLT_KLINE, 3, 3);
				}
			}
			else if(L"fskline" == StrSelType)
			{
				if(L"2" == StrSelParam)
				{
					pIoViewDuoGuTongLie->ChangeDuoGuTongLieType(CIoViewDuoGuTongLie::EDGLT_KLINE_TREND, 2, 2);
				}
				else if(L"3" == StrSelParam)
				{
					pIoViewDuoGuTongLie->ChangeDuoGuTongLieType(CIoViewDuoGuTongLie::EDGLT_KLINE_TREND, 2, 3);
				}
				else if(L"4" == StrSelParam)
				{
					pIoViewDuoGuTongLie->ChangeDuoGuTongLieType(CIoViewDuoGuTongLie::EDGLT_KLINE_TREND, 2, 4);
				}
			}
		}
	}

	return CWnd::OnCommand(wParam, lParam);
}

BOOL CNewTBWnd::OnBtnEvent( CString strEventType, CString strEventParam, WPARAM wParam, LPARAM lParam, CString strBtnName, int iWidth, int iHeight)
{
    int iEventType = IsAllNumbers(strEventType)?_ttoi(strEventType):-1;
    if(iEventType == -1)
    {
        return CWnd::OnCommand(wParam, lParam);
    }

    switch(iEventType)
    {
    case EType_Market_Change:
        {
            CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
            int32 iMarketID = IsAllNumbers(strEventParam)? _ttoi(strEventParam): -1;
            if(pMain && iMarketID != -1)
            {
                pMain->OpenSpecifyMarketOrBlock(iMarketID);
            }
        }
        break;

    case EType_Open_Cfm:
        {
            if(!strEventParam.IsEmpty())
            {
                DoOpenCfm(strEventParam);
            }
        }
        break;

    case EType_Inner_WebPage:
        {
            if(!strEventParam.IsEmpty())
            {
                CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
				if(pMain)
				{
					CMPIChildFrame *pActiveFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pMain->MDIGetActive());
					CIoViewNews *pIoViewNews = NULL;
					if (NULL != pActiveFrame)
					{
						if(pActiveFrame->GetIdString() != L"网页资讯")
						{
							int iLeftPos = strEventParam.Find('(');
							int iRightPos = strEventParam.Find(')');
							if(iLeftPos != -1)
							{
								bool32 bLoad = DoOpenCfm(L"网页资讯");
								if ( bLoad )
								{
									pIoViewNews = pMain->FindIoViewNews(true);
								}

								if(pIoViewNews)
								{
									CString strLeft = strEventParam.Left(iLeftPos);
									CString StrUrl = strEventParam.Mid(iLeftPos + 1, iRightPos - iLeftPos -1);
									CString StrSpeUrl = L"";
									if(strLeft == L"NewsCenter")
									{
										StrSpeUrl = pIoViewNews->GetWebNewsUrl(StrUrl); //pIoViewNews->GetFullScreenNewsUrl(StrUrl);
									}
									else if(strLeft == L"LiveVideo")
									{
										StrSpeUrl = StrUrl;//pIoViewNews->GetLiveVideoUrl(StrUrl);
									}
									pIoViewNews->RefreshWeb(StrSpeUrl);
								}
							}
							else
							{

								UrlParser urlParser(strEventParam);
								bool32 bLoadcfm = DoOpenCfm(L"网页资讯", urlParser.GetUrl());
								if ( bLoadcfm )
								{
									pIoViewNews = pMain->FindIoViewNews(true);
								}
								if(pIoViewNews)
								{
									pIoViewNews->RefreshWeb(urlParser.GetUrl());
								}
							}
						}
						else
						{
							pIoViewNews = pMain->FindIoViewNews(true);
							int iLeftPosNC = strEventParam.Find('(');
							int iRightPosNC = strEventParam.Find(')');
							if(iLeftPosNC != -1)
							{
								CString strLeft = strEventParam.Left(iLeftPosNC);
								CString StrUrl = strEventParam.Mid(iLeftPosNC + 1, iRightPosNC - iLeftPosNC -1);
								CString StrSpecialUrl = L"";
								if(strLeft == L"NewsCenter")
								{
									StrSpecialUrl = pIoViewNews->GetWebNewsUrl(StrUrl);//pIoViewNews->GetFullScreenNewsUrl(StrUrl);
								}
								else if(strLeft == L"LiveVideo")
								{
									StrSpecialUrl = StrUrl;//pIoViewNews->GetLiveVideoUrl(StrUrl);
								}

								CCfmManager::Instance().AddOpenedCfm(L"网页资讯", StrSpecialUrl);
								if(pIoViewNews)
								{
									pIoViewNews->RefreshWeb(StrSpecialUrl);
								}
							}
							else
							{
								UrlParser urlParser(strEventParam);
								CCfmManager::Instance().AddOpenedCfm(L"网页资讯",  urlParser.GetUrl());
								if(pIoViewNews)
								{
									pIoViewNews->RefreshWeb( urlParser.GetUrl());
								}
							}
						}
					}
				}
            }
        }
        break;

    case EType_Dialog_WebPage:
        {
			if (!strEventParam.IsEmpty())
			{
				 CDlgTodayCommentIE::ShowDlgIEWithSize(strBtnName, strEventParam, CRect(0,0,iWidth,iHeight));
			}
           
        }
        break;

    case EType_Special_Function:
        {
            OnSpecialFunc(strEventParam, wParam, lParam);
        }
        break;
	case EType_Index_Change:				// 指标切换
		{
			OnIndexChangeFunc(strEventParam, wParam, lParam);
		}
		break;
	case EType_Self_Defination:				// 自定义切换
		{
			OnCustomFunc(strEventParam, wParam, lParam);
		}
		break;
    default:
        break;
    }

    return CWnd::OnCommand(wParam, lParam);
}

bool CNewTBWnd::IsAllNumbers( CString str )
{
    if(str.IsEmpty())
    {
        return false;
    }
	
	if (str.SpanIncluding(_T(".0123456789")) == str)
	{
		return true;
	}

    return false;
}

void CNewTBWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	if(m_iCurrentPage == IDC_BTN_MAIN)
	{
		if (INVALID_ID != m_iXButtonHovering)
		{
			GetCNCButton(m_mapBtn, m_iXButtonHovering).RButtonUp();
			m_iXButtonHovering = INVALID_ID;
		}
	}
}

bool CNewTBWnd::RbuttonProcess(int iSrcID)
{
	//处理交易消息
	if (RbuttonTradeProcess(iSrcID))
	{
		return true;
	}
	return false;
}

//	显示对话框
bool CNewTBWnd::ShowFileDlg(const CString& strOpenFilePath, CString& selectPath)
{
	// 提示选择
	CFileDialog dlg(TRUE, NULL, strOpenFilePath, OFN_FILEMUSTEXIST|OFN_NOCHANGEDIR, _T("应用程序|*.exe|所有|*.*||"), this);
	dlg.m_ofn.lpstrTitle = _T("请选择要设定的交易程序");
	if ( dlg.DoModal() == IDOK )
	{
		selectPath = dlg.GetPathName();		
		return true;
	}
	return false;
}

//	实盘交易"提醒可右键菜单按钮更改应用程序路径"对话框显示
void CNewTBWnd::ShowTradeFirstNotify()
{
	//TCHAR* pSec = L"TradeFirstNotify";
	//TCHAR* pKey = L"on";
	//CString strVal(L"1");
	//if (!CEtcXmlConfig::Instance().ReadEtcConfig(pSec, pKey, L"1", strVal) )
	//{
	//	//在配置信息中如果查找不到该键值，则默认为第一次
	//	strVal = L"1";
	//}

	//bool32 bShowTradeFirstNotify = _wtoi((LPCTSTR)strVal) == 0 ? false : true;
	//if (bShowTradeFirstNotify)
	{
		::MessageBox(AfxGetMainWnd()->GetSafeHwnd(), L"您可以右键单击实盘交易重新选择", L"实盘交易", MB_OK);
		//CEtcXmlConfig::Instance().WriteEtcConfig(pSec, pKey, L"0");//更新配置信息，下次不需要弹框提醒
	}

}
//模拟交易处理
bool CNewTBWnd::RbuttonTradeProcess(int iSrcID)
{
	if( m_mapRealTrad.find(iSrcID) != m_mapRealTrad.end())//是否是实盘交易类型
	{

		CString tSec, tPath;
		GetTradeSection(iSrcID, tSec);
		CEtcXmlConfig::Instance().ReadEtcConfig((LPCTSTR)tSec, KStrTradeKey, NULL, tPath);//读取改节点下的URL信息
		if (tPath.IsEmpty())
		{
			DoTradePlugin(iSrcID, false);//第一次打开
		}
		else
		{
			if (::MessageBox(AfxGetMainWnd()->GetSafeHwnd(), L"是否重新选择交易?", L"交易", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				DoTradePlugin(iSrcID, true);//重复打开
			}
			InvalidateRect(NULL, FALSE);

		}

		//菜单状态复位
		buttonContainerType::iterator it = FIND_BTN_ITERATOR(m_mapBtn, iSrcID);
		if (it != m_mapBtn.end())
		{
			CRect rtBtn;
			(*it).GetRect(rtBtn);
			(*it).SetMouseLeave();
			InvalidateRect(&rtBtn, FALSE);
		}
		else
		{
			InvalidateRect(NULL, FALSE);
		}
		return true;
	}	
	return true;

}


/*
	监视toolbar.xml中按钮数据，将事件类型为外挂实盘交易数据的id存入map中，
	指定临时路劲数据为空字符串,目前map中的第2个字段暂时无用，
	每次点击菜单总是实时从配置中读取
*/
void CNewTBWnd::AddRealTrade(const T_ColumnData &data)
{	
	int id = 0;
	int iCount = data.m_BtnData.size();
	for (int i =0; i < iCount; i++)
	{
		if (data.m_BtnData[i].m_ChildBtnData.size() != 0)
		{
			for (int j =0; j < data.m_BtnData[i].m_ChildBtnData.size(); j++)
			{
				if ( _wtoi(data.m_BtnData[i].m_ChildBtnData[j].m_EventParam) == FType_UserDefine_Real_Trade)
				{
					id = data.m_BtnData[i].m_ChildBtnData[j].m_ID;
					m_mapRealTrad[id] = L"";
				}
			}
			
		}
		if ( _wtoi(data.m_BtnData[i].m_EventParam) == FType_UserDefine_Real_Trade)
		{
			id = data.m_BtnData[i].m_ID;
			m_mapRealTrad[id] = L"";			
		}
	}
	
}

void		CNewTBWnd::GetTradeSection(int iTradeId, OUT CString& tSec)
{
	tSec.Format(_T("%s_%d"), KStrTradeSecPrefix, iTradeId);
}

CDlgPullDown* CNewTBWnd::GetPullDown()
{
	return m_pDlgPullDown;
}

CDlgPullDown* CNewTBWnd::AdjustSubMenuPos(CString strParent, int left, int top)
{
	CDlgPullDown* pDlgPullDown = NULL;

	vector<CDlgPullDown*>::iterator iter;
	for (iter = m_vecDlgSubPullDown.begin() ; iter != m_vecDlgSubPullDown.end() ; ++iter)
	{
		if ((*iter)->GetParentTitle() == strParent)
		{
			int nHeight = (*iter)->GetHeight();

			CRect rt(left + 3, top, left+(*iter)->GetWidth(), top+nHeight);

			(*iter)->MoveWindow(rt);
			pDlgPullDown = *iter;
			break;
		}
	}

	return pDlgPullDown;
}

CDlgPullDown* CNewTBWnd::CreateSubMenu(T_ChildButtonData &btnData, int left, int top)
{
	vector<T_ChildButtonData> &vecMenu = btnData.m_ChildSubMenuData;

	CDlgPullDown *pDlgPullDown = NULL;

	CGGTongApp *pApp = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());

	if (!pApp)
	{
		return pDlgPullDown;
	}

	int iSize = vecMenu.size();

	if (iSize > 0)
	{
		static int index = 0;

		buttonContainerType mapBtn;
		mapBtn.clear();
		pDlgPullDown = new CDlgPullDown(m_pDlgPullDown);
		pDlgPullDown->Create(IDD_DIALOG_PULLDOWN, m_pDlgPullDown);
		m_vecDlgSubPullDown.push_back(pDlgPullDown);

		int pos = 0;
		vector<T_ChildButtonData>::iterator iterSub;
		int nWidth = 0, nHeight = 0;
		
		for (iterSub = vecMenu.begin() ; iterSub != vecMenu.end() ; ++iterSub)
		{
			Image *pImg = NULL, *pImgStatus = NULL;
			CNCButton btnSub;
			pImg = iterSub->m_pBkImage;
			pImgStatus = iterSub->m_pImgStatus;

			if (pImg && Ok == pImg->GetLastStatus())
			{
				nWidth = pImg->GetWidth();
				nHeight = pImg->GetHeight()/3;
				CRect rtNew(0, nHeight * pos, nWidth, nHeight * (1 + pos));
				++pos;
				btnSub.CreateButton(iterSub->m_Text, rtNew, pDlgPullDown, pImg, 3, iterSub->m_ID, NULL, pImgStatus, iterSub->m_EventParam);	
				btnSub.SetTextColor(iterSub->m_crlTextColor, iterSub->m_crlTextColor, Color(70,70,230));
				btnSub.SetGroupId(iterSub->m_iGroupId);

				if (0. != iterSub->m_fFontSize)
				{
					m_fontSimpleBtn.m_Size = iterSub->m_fFontSize;
				}
				else
				{
					CString strSize           = pApp->m_pConfigInfo->m_toolBarData.m_SimpleFontSize;
					m_fontSimpleBtn.m_Size    = IsAllNumbers(strSize)?_tstof(strSize):10;
				}
				
				btnSub.SetFont(m_fontSimpleBtn);
				btnSub.SetTextOffPos(CPoint(0,2));

				mapBtn.push_back(btnSub);
				m_mapMorePullDown.push_back(btnSub);

				pDlgPullDown->IncrementHeightAndWidth(nHeight, nWidth);
			}
		}

		nHeight = pDlgPullDown->GetHeight();
		CRect rt(left + 3, top, left+nWidth, top+nHeight);

		pDlgPullDown->MoveWindow(rt);
		pDlgPullDown->SetShowButtons(mapBtn, rt);
		pDlgPullDown->SetParentTitle(btnData.m_Text);
	}

	return pDlgPullDown;
}

void CNewTBWnd::UpdateFixedStatus(CPoint& point)
{
	bool32 bLeft = false;
	int iButton = TFixedButtonHitTest(point, bLeft);

	if (iButton != m_stFixedBtnHovering.iButtonId)
	{
		if (INVALID_ID != m_stFixedBtnHovering.iButtonId)
		{
			if (m_stFixedBtnHovering.bLeft)
			{
				GetCNCButton(m_mapLeftFixedBtn,m_stFixedBtnHovering.iButtonId).MouseLeave();
			}
			else
			{
				GetCNCButton(m_mapRightFixedBtn,m_stFixedBtnHovering.iButtonId).MouseLeave();
			}
			
			m_stFixedBtnHovering.bLeft     = true;
			m_stFixedBtnHovering.iButtonId = INVALID_ID;
		}

		if (INVALID_ID != iButton)
		{	
			m_stFixedBtnHovering.bLeft     = bLeft;
			m_stFixedBtnHovering.iButtonId = iButton;

			if (bLeft)
			{
				GetCNCButton(m_mapLeftFixedBtn,m_stFixedBtnHovering.iButtonId).MouseHover();
			}	
			else
			{
				GetCNCButton(m_mapRightFixedBtn,m_stFixedBtnHovering.iButtonId).MouseHover();
			}
		}
	}
}

int CNewTBWnd::TFixedButtonHitTest(CPoint point, bool32 &bLeft)
{
	buttonContainerType::iterator iter;
   
	if (point.x > m_iLeftFixedWidth)
	{
		// 遍历右边固定所有按钮
		FOR_EACH(m_mapRightFixedBtn, iter) 
		{
			CNCButton &btnControl = *iter;

			// 点point是否在已绘制的按钮区域内
			if (btnControl.PtInButton(point) && btnControl.GetCreate())
			{
				bLeft = false;
				return btnControl.GetControlId();
			}
		}
	}
	else
	{
		// 遍历左边固定所有按钮
		FOR_EACH(m_mapLeftFixedBtn, iter) 
		{
			CNCButton &btnControl = *iter;

			// 点point是否在已绘制的按钮区域内
			if (btnControl.PtInButton(point) && btnControl.GetCreate())
			{
				bLeft = true;
				return btnControl.GetControlId();
			}
		}
	}

	return INVALID_ID;
}

void CNewTBWnd::AddToolbarButton(const T_ColumnData &colData, int& iXStartPos, int &iYStartPos, const int iBtnTotalHeight, E_FixedType eFixedType/*=EFT_NONE*/)
{
	CGGTongApp *pApp = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());

	if (!pApp)
	{
		return;
	}

	//添加外置实盘
	AddRealTrade(colData);

	//工具按钮布局
	int iColWidth  = colData.m_iColWidth;
	int iBtnHeight = iBtnTotalHeight;
	int iBtnDataSize = colData.m_BtnData.size();

	if(iBtnDataSize <= 0)
	{
		return;
	}

	if(iBtnDataSize == 1)
	{
		T_ButtonData tempBtnData = colData.m_BtnData[0];
	
		// 改变字体大小
		if (0. != tempBtnData.m_fFontSize)
		{
			m_fontSimpleBtn.m_Size = tempBtnData.m_fFontSize;
		}
		else
		{
			CString strSize           = pApp->m_pConfigInfo->m_toolBarData.m_SimpleFontSize;
			m_fontSimpleBtn.m_Size    = IsAllNumbers(strSize)?_tstof(strSize):10;
		}

		CRect rcBtn(iXStartPos, iYStartPos, iXStartPos+iColWidth, iYStartPos+iBtnHeight);
		bool bCategory = false;
		int iType = IsAllNumbers(tempBtnData.m_EventType)?_ttoi(tempBtnData.m_EventType):0;
		if((eEventType)iType == EType_Category)
		{
			bCategory = true;
		}

		if (/*tempBtnData.m_Text.GetLength() == 0 &&*/ NULL != tempBtnData.m_pBkImage && tempBtnData.m_EventType == L"101")	//	代表广告
		{
			AddNcButton(&rcBtn, tempBtnData, bCategory, 1, eFixedType);
		}
		else
		{
			AddNcButton(&rcBtn, tempBtnData, bCategory, 3, eFixedType);
		}

		iXStartPos += iColWidth;
	}
	else
	{			
		T_ButtonData tempBtnData1 = colData.m_BtnData[0];			
		T_ButtonData tempBtnData2 = colData.m_BtnData[1];
		
		// 改变字体大小
		if (0. != tempBtnData1.m_fFontSize)
		{
			m_fontSimpleBtn.m_Size = tempBtnData1.m_fFontSize;
		}
		else
		{
 			CString strSize           = pApp->m_pConfigInfo->m_toolBarData.m_SimpleFontSize;
			m_fontSimpleBtn.m_Size    = IsAllNumbers(strSize)?_tstof(strSize):10;
		}

		CRect rcBtn1(iXStartPos, iYStartPos, iXStartPos+iColWidth, iYStartPos+iBtnHeight / 2);
		AddNcButton(&rcBtn1, tempBtnData1, false, 3, eFixedType);
		CRect rcBtn2(iXStartPos, iYStartPos+iBtnHeight / 2, iXStartPos+iColWidth, iYStartPos+iBtnHeight);
		AddNcButton(&rcBtn2, tempBtnData2, false, 3, eFixedType);

		iXStartPos += iColWidth;
	}
}


void CNewTBWnd::SetFixBtnCheckStatus(int iButton)
{
	for (int i =0; i < m_mapLeftFixedBtn.size(); i++)
	{
		if(m_mapLeftFixedBtn[i].GetControlId() != iButton)
		{
			m_mapLeftFixedBtn[i].SetCheckStatus(FALSE);
		}
	}

	for (int i =0; i < m_mapRightFixedBtn.size(); i++)
	{
		if(m_mapRightFixedBtn[i].GetControlId() != iButton)
		{
			m_mapRightFixedBtn[i].SetCheckStatus(FALSE);
		}
	}
}


void CNewTBWnd::SetChildCheckStatusByName(CString strCfmName)
{
	if (m_pChildWnd)
	{
		m_pChildWnd->SetCheckStatusByName(strCfmName);
	}
}




void CNewTBWnd::KLineCycleChange(int iKlineType)
{
	int iID = iKlineType;
	if (INVALID_ID != iID)
	{
		if ( iID >= IDM_CHART_CYCLE_BEGIN && iID <= IDM_CHART_CYCLE_END )
		{	
			CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
			if (!pMain)
			{
				return ;
			}

			CIoViewBase	*pIoView = pMain->FindActiveIoView();
			bool32 bHandled = false;

			// 对于套利k，分时，报价做不同处理
			if ( NULL!=pIoView 
				&& ( pIoView->IsKindOf(RUNTIME_CLASS(CIoViewReportArbitrage))
				|| pIoView->IsKindOf(RUNTIME_CLASS(CIoViewKLineArbitrage))
				|| pIoView->IsKindOf(RUNTIME_CLASS(CIoViewTrendArbitrage))) )
			{
				// 打开套利K线，切换周期
				// 如果当前页面有套利K线，则使当前页面切换周期
				// 没有，则调用套利K线页面
				CMPIChildFrame *pChartFrame = NULL;


				//CIoViewBase *pIoViewKline = pMain->FindIoViewInFrame(ID_PIC_KLINEARBITRAGE, GetIoViewGroupId());
				CIoViewBase * pIoViewKline = (CIoViewKLine *)pMain->FindIoViewInFrame(ID_PIC_KLINE, NULL, false, false);
				if ( NULL == pIoViewKline )
				{
					CArbitrage arb;
					pMain->OnArbitrageF5(ID_PIC_KLINEARBITRAGE, arb, pIoView);
					//pIoViewKline = pMain->FindIoViewInFrame(ID_PIC_KLINEARBITRAGE, GetIoViewGroupId());
					pIoViewKline = (CIoViewKLine *)pMain->FindIoViewInFrame(ID_PIC_KLINE, NULL, false, false);

				}
				if ( NULL!=pIoViewKline )
				{
					pIoViewKline->KLineCycleChange(iID);
					bHandled = true;
				}
			}
			else
			{
				CIoViewBase* pActiveView =pMain->FindActiveIoView();
				ASSERT(NULL != pActiveView);
				//	当前视图是否是多股同列
				{
					CIoViewDuoGuTongLie* pDuoguo = DYNAMIC_DOWNCAST(CIoViewDuoGuTongLie, pActiveView);
					if (NULL != pDuoguo)
					{
						pDuoguo->KLineCycleChange(iID);						
						return;

					}
				}

				// 打开k线，切换周期
				// 如果当前页面有K线，则使当前页面切换周期
				// 没有，则调用K线页面
				//CIoViewBase *pIoViewKline = pMain->FindIoViewInFrame(ID_PIC_KLINE, GetIoViewGroupId());
				CIoViewBase * pIoViewKline = (CIoViewKLine *)pMain->FindIoViewInFrame(ID_PIC_KLINE, NULL, false, false);
				const char *pShow = pIoViewKline->GetXmlElementAttrShowTabName();
				if ( NULL == pIoViewKline )
				{
					if (NULL != pMain->m_pNewWndTB)
					{
						//g_bDrawKLineCyc = FALSE;
						pMain->m_pNewWndTB->LoadSystemDefaultCfm(TB_IOCMD_KLINE);
						//pIoViewKline = pMain->FindIoViewInFrame(ID_PIC_KLINE, GetIoViewGroupId());	// 点击顶部按钮，切换K线的代码
						pIoViewKline = (CIoViewKLine *)pMain->FindIoViewInFrame(ID_PIC_KLINE, NULL, false, false);
					}
				}
				if ( NULL!=pIoViewKline )
				{
					pIoViewKline->BringToTop();
					//g_bDrawKLineCyc = TRUE;
					pIoViewKline->KLineCycleChange(iID);
					bHandled = true;
				}
			}
		}
	}
}

void CNewTBWnd::SetDefaultCfmStatus()
{
	int iID = INVALID_ID;
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if(NULL == pApp)
	{
		return ;
	}

	int ArrColSize = pApp->m_pConfigInfo->m_vecColumnData.size();
	if(ArrColSize <= 0)
	{
		return ;
	}

	for (int colIndex = 0; colIndex < ArrColSize; colIndex++)
	{
		T_ColumnData tempColData = pApp->m_pConfigInfo->m_vecColumnData.at(colIndex);
		int btnDataSize = tempColData.m_BtnData.size();
		if(btnDataSize <= 0)
		{
			continue;
		}

		for (int btnIndex = 0; btnIndex < btnDataSize; btnIndex++)
		{
			T_ButtonData tmpBtnData =tempColData.m_BtnData.at(btnIndex);
			if(tmpBtnData.m_EventType == L"4" && tmpBtnData.m_EventParam == L"0")
			{
				iID = tmpBtnData.m_ID;
				break;
			}
		}
	}

	if(iID != INVALID_ID)
	{
		buttonContainerType::iterator itToolbar = FIND_BTN_ITERATOR(m_mapBtn, iID);
		if(m_mapBtn.end() != itToolbar)
		{
			for (int i =0; i < m_mapBtn.size(); i++)
			{
				if(m_mapBtn[i].GetControlId() == iID)
				{
					m_mapBtn[i].SetCheckStatus(TRUE);
				}
				else
				{
					m_mapBtn[i].SetCheckStatus(FALSE);
				}
			}
			SetFixBtnCheckStatus(FALSE);

			return;
		}

		buttonContainerType::iterator itLeftToolbar = FIND_BTN_ITERATOR(m_mapLeftFixedBtn, iID);
		if(m_mapLeftFixedBtn.end() != itLeftToolbar)
		{
			for (int i =0; i < m_mapLeftFixedBtn.size(); i++)
			{
				if(m_mapLeftFixedBtn[i].GetControlId() == iID)
				{
					m_mapLeftFixedBtn[i].SetCheckStatus(TRUE);
				}
				else
				{
					m_mapLeftFixedBtn[i].SetCheckStatus(FALSE);
				}
			}

			for (int i =0; i < m_mapRightFixedBtn.size(); i++)
			{
				m_mapRightFixedBtn[i].SetCheckStatus(FALSE);	
			}

			for (int i =0; i < m_mapBtn.size(); i++)
			{
				m_mapBtn[i].SetCheckStatus(FALSE);
			}

			return;
		}

		buttonContainerType::iterator itRightToolbar = FIND_BTN_ITERATOR(m_mapRightFixedBtn, iID);
		if(m_mapRightFixedBtn.end() != itRightToolbar)
		{
			for (int i =0; i < m_mapRightFixedBtn.size(); i++)
			{
				if(m_mapRightFixedBtn[i].GetControlId() == iID)
				{
					m_mapRightFixedBtn[i].SetCheckStatus(TRUE);
				}
				else
				{
					m_mapRightFixedBtn[i].SetCheckStatus(FALSE);
				}
			}

			for (int i =0; i < m_mapLeftFixedBtn.size(); i++)
			{
				m_mapLeftFixedBtn[i].SetCheckStatus(FALSE);	
			}

			for (int i =0; i < m_mapBtn.size(); i++)
			{
				m_mapBtn[i].SetCheckStatus(FALSE);
			}

			return;
		}
	}
}
