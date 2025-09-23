#include "StdAfx.h"

#include "DlgMarketRadarList.h"

#include "facescheme.h"

#include "UserBlockManager.h"
#include "ReportScheme.h"
#include "DlgAddToBlock.h"
#include "XmlShare.h"

#include "DlgMarketRadarSetting.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int32 KTimerIdUpdateMerchInit = 100;
const int32 KTimerPeriodUpdateMerchInit = 60*1000;

const int32 KTimerIdCheckDelMerch = 101;
const int32 KTimerPeriodCheckDelMerch = 30*1000;	// 检查删除队列

const int32 KTimerIdFillList = 102;
const int32 KTimerPeriodFillList = 200;

/////////////////////////////////////////////////////////////////////////////
// CDlgMarketRadarList dialog

CDlgMarketRadarList::CDlgMarketRadarList(CWnd* pParent /*=NULL*/)
: _MYBASE_DLG_(CDlgMarketRadarList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMarketRadarList)
	//}}AFX_DATA_INIT

	m_rcList.SetRectEmpty();
	m_rcOpenClose.SetRectEmpty();
	m_rcSave.SetRectEmpty();
	m_rcAnalyze.SetRectEmpty();
	m_rcSetting.SetRectEmpty();
	m_rcOrg.SetRectEmpty();
}


void CDlgMarketRadarList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMarketRadarList)
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMarketRadarList, _MYBASE_DLG_)
//{{AFX_MSG_MAP(CDlgMarketRadarList)
ON_WM_DESTROY()
ON_WM_SIZE()
ON_WM_TIMER()
ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST1, OnGetDispInfo)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, OnListCustomDraw)
ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnListDBLClick)
ON_NOTIFY(NM_KEYDOWN, IDC_LIST1, OnListKeyDown)
ON_MESSAGE_VOID(UM_MY_ADD_MERCH, OnMsgAddMerch)
ON_MESSAGE(UM_MY_REMOVE_MERCH, OnMsgRemoveMerch)
ON_BN_CLICKED(IDC_BUTTON_OPENCLOSE, OnBtnOpenClose)
ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBtnSave)
ON_BN_CLICKED(IDC_BUTTON_ANALYZE, OnBtnAnalyse)
ON_BN_CLICKED(IDC_BUTTON_SETTING, OnBtnSetting)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgMarketRadarList::OnInitDialog()
{
	_MYBASE_DLG_::OnInitDialog();

	if ( m_StrTitleOrg.IsEmpty() )
	{
		GetWindowText(m_StrTitleOrg);
	}

	// 设定初始值
	m_List.SetExtendedStyle(m_List.GetExtendedStyle()|LVS_EX_FULLROWSELECT);
	m_List.ModifyStyle(LVS_SHOWSELALWAYS, 0);
	m_List.InsertColumn(0, _T("商品名称"), LVCFMT_LEFT, 80);
	m_List.InsertColumn(1, _T("时间"),	   LVCFMT_LEFT, 50);
	m_List.InsertColumn(2, _T("价格"),     LVCFMT_LEFT, 70);
	m_List.InsertColumn(3, _T("最后成交量"), LVCFMT_LEFT, 80);
	m_List.InsertColumn(4, _T("条件"), LVCFMT_LEFT, 85);

	const COLORREF clrBk = CFaceScheme::Instance()->GetSysColor(ESCBackground);
	const COLORREF clrText = CFaceScheme::Instance()->GetSysColor(ESCText);
	m_List.SetBkColor(clrBk);
	m_List.SetTextBkColor(clrBk);
	m_List.SetTextColor(clrText);
	

	CMarketRadarCalc::Instance().GetResults(m_aResAsync, this);

	FillList();

	SetTimer(KTimerIdUpdateMerchInit, KTimerPeriodUpdateMerchInit, NULL);
	SetTimer(KTimerIdCheckDelMerch, KTimerPeriodCheckDelMerch, NULL);
	
	UpdateOpenCloseString();

	CBlockConfig::Instance()->AddListener(this);
	CMarketRadarCalc::Instance().AddListener(this);	// 在GetResults之后，避免计算线程结果变动时，要占用m_lockResult锁导致死锁

	RecalcLayout();

	return TRUE;
}

void CDlgMarketRadarList::OnOK()
{
	// 检查一下值
	OnEnterKey();

	return;
}

void CDlgMarketRadarList::PromptErrorInput( const CString &StrPrompt, CWnd *pWndFocus /*= NULL*/ )
{
	if ( !StrPrompt.IsEmpty() )
	{
		AfxMessageBox(StrPrompt, MB_ICONWARNING |MB_OK);
	}
	if ( NULL != pWndFocus )
	{
		pWndFocus->SetFocus();
		if ( pWndFocus->IsKindOf(RUNTIME_CLASS(CEdit)) )
		{
			((CEdit *)pWndFocus)->SetSel(0, -1);
		}
		else if ( pWndFocus->IsKindOf(RUNTIME_CLASS(CRichEditCtrl)) )
		{
			((CRichEditCtrl *)pWndFocus)->SetSel(0, -1);
		}
	}
}

void CDlgMarketRadarList::FillList()
{
	KillTimer(KTimerIdFillList);

	CMerch *pMerch = GetSelectMerch();

	CSingleLock lock(&m_LockResult, TRUE);
	m_aResults.Copy(m_aResAsync);
	lock.Unlock();

	m_List.SetItemCount(m_aResults.GetSize());
	m_List.Invalidate();
	if ( NULL != pMerch )
	{
		SelectMerch(pMerch);
	}

	if ( m_List.GetSelectedCount() <= 0
		&& m_List.GetItemCount() > 0 )
	{
		m_List.SetItemState(0, LVIS_SELECTED |LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
	if ( m_aResults.GetSize() > 0 )
	{
		m_List.EnsureVisible(0, FALSE);	// 首行可见
	}
}

bool32 GetResultDesc(const T_MarketRadarResult &Result, OUT CString &StrValue, OUT COLORREF &clrText)
{
	CMerch *pMerch = Result.m_pMerch;
	if ( NULL == pMerch )
	{
		return false;
	}

	StrValue = GetMarketRadarResultString(Result);
	COLORREF clrKeep = CFaceScheme::Instance()->GetSysColor(ESCText);
	COLORREF clrRise = CFaceScheme::Instance()->GetSysColor(ESCRise);
	COLORREF clrFall = CFaceScheme::Instance()->GetSysColor(ESCFall);
	if ( Result.m_iRiseFallFlag > 0 )
	{
		clrText = clrRise;
	}
	else if ( Result.m_iRiseFallFlag < 0 )
	{
		clrText = clrFall;
	}
	else
	{
		clrText = clrKeep;
	}
	return true;
}

void CDlgMarketRadarList::OnGetDispInfo( NMHDR *pHdr, LRESULT *pResult )
{
	NMLVDISPINFO *pdi = (NMLVDISPINFO*)pHdr;
	if ( NULL != pdi )
	{
		int iIndex = pdi->item.iItem;
		const int iCol = pdi->item.iSubItem;
		if ( iIndex >= 0 && iIndex < m_aResults.GetSize()
			&& iCol >= 0 && iCol < 5)
		{
			iIndex = m_aResults.GetUpperBound()-iIndex;		// 从晚显示到早
			
			const T_MarketRadarResult &Result = m_aResults[iIndex];
			pdi->item.lParam = (LPARAM)Result.m_pMerch;
			
			CString StrValue;
			switch ( iCol )
			{
			case 0:	// 名
				StrValue = Result.m_pMerch->m_MerchInfo.m_StrMerchCnName;
				break;
			case 1:	// 时
				{
					tm *pLocal = Result.m_TimeCalc.GetLocalTm();
					StrValue.Format(_T("%02d:%02d"), pLocal->tm_hour, pLocal->tm_min);
				}
				break;
			case 2:	// 价
				{
					StrValue = Float2String(Result.m_TickLast.m_fPrice, Result.m_pMerch->m_MerchInfo.m_iSaveDec, false, false);
				}
				break;
			case 3:	// 量
				{
					//StrValue.Format(_T("%.0f"), Result.m_TickLast.m_fVolume);
					StrValue = Float2String(Result.m_TickLast.m_fVolume, 0);
				}
				break;
			case 4:	// 条件
				{
					StrValue = GetMarketRadarResultString(Result);
					T_Block *pBlock = CUserBlockManager::Instance()->GetBlock(Result.m_pMerch);
					if ( NULL != pBlock )
					{
						StrValue += _T("-自");
					}
				}
				break;
			}
			if ( (pdi->item.mask &LVIF_TEXT) == LVIF_TEXT
				&& NULL != pdi->item.pszText 
				&& pdi->item.cchTextMax > 0 )
			{
				_tcsncpy(pdi->item.pszText, StrValue, pdi->item.cchTextMax-1);
				pdi->item.pszText[pdi->item.cchTextMax-1] = _T('\0');
			}
		}
	}
}

void CDlgMarketRadarList::OnListCustomDraw( NMHDR *pHdr, LRESULT *pResult )
{
	NMLVCUSTOMDRAW *pDrawItem = (NMLVCUSTOMDRAW *)pHdr;
	if ( NULL != pDrawItem )
	{
		DWORD dwCDDS = pDrawItem->nmcd.dwDrawStage;
		if ( dwCDDS&CDDS_PREPAINT )
		{
			*pResult = CDRF_NOTIFYITEMDRAW |CDRF_NOTIFYSUBITEMDRAW;
		}
		if ( (dwCDDS&CDDS_SUBITEM)
			&& (dwCDDS&CDDS_ITEM) )
		{
			DWORD dwIndex = m_aResults.GetUpperBound() -pDrawItem->nmcd.dwItemSpec;
			if (dwIndex < (DWORD)m_aResults.GetSize())
			{
				const T_MarketRadarResult &Result = m_aResults[dwIndex];
				pDrawItem->clrText = CFaceScheme::Instance()->GetSysColor(ESCText);

				switch(pDrawItem->iSubItem)
				{
				case 0:		// 名称
					{
						T_Block* pUserBlock = NULL;
						if ( (pUserBlock = CUserBlockManager::Instance()->GetBlock(Result.m_pMerch)) != NULL  )
						{
							// 自选板块的商品名字颜色特殊显示:
							if ( CLR_DEFAULT != pUserBlock->m_clrBlock )
							{
								pDrawItem->clrText = pUserBlock->m_clrBlock;
							}	
						}
					}
					break;
				case 1:		// 时间
					break;
				case 2:		// 价格
					{
						if ( NULL != Result.m_pMerch
							&& NULL != Result.m_pMerch->m_pRealtimePrice )
						{
							float fPrev;
							if ( ERTFuturesCn == Result.m_pMerch->m_Market.m_MarketInfo.m_eMarketReportType )
							{
								fPrev = Result.m_pMerch->m_pRealtimePrice->m_fPricePrevAvg;
							}
							else
							{
								fPrev = Result.m_pMerch->m_pRealtimePrice->m_fPricePrevClose;
							}
							if ( Result.m_TickLast.m_fPrice > fPrev )
							{
								pDrawItem->clrText = CFaceScheme::Instance()->GetSysColor(ESCRise);
							}
							else if ( Result.m_TickLast.m_fPrice < fPrev )
							{
								pDrawItem->clrText = CFaceScheme::Instance()->GetSysColor(ESCFall);
							}
							else
							{
								pDrawItem->clrText = CFaceScheme::Instance()->GetSysColor(ESCText);
							}
						}
					}
					break;
				case 3:		// 量
					break;
				case 4:		// 条件
					{
						if ( Result.m_iRiseFallFlag > 0 )
						{
							pDrawItem->clrText = CFaceScheme::Instance()->GetSysColor(ESCRise);
						}
						else if ( Result.m_iRiseFallFlag < 0 )
						{
							pDrawItem->clrText = CFaceScheme::Instance()->GetSysColor(ESCFall);
						}
						else
						{
							pDrawItem->clrText = CFaceScheme::Instance()->GetSysColor(ESCText);
						}
					}
					break;
				}
			}
		}
	}
}

void CDlgMarketRadarList::OnMsgAddMerch()
{
	MSG msg;
	while ( PeekMessage(&msg, m_hWnd, UM_MY_ADD_MERCH, UM_MY_ADD_MERCH, PM_REMOVE) )
	{
		// 移出现有队列中的该消息
	}
	SetTimer(KTimerIdFillList, KTimerPeriodFillList, NULL);
}

void CDlgMarketRadarList::OnAddMarketRadarResult( const MarketRadarResultMap &mapResult, E_AddCause eCause )
{
	MarketRadarResultArray aRes;
	CMarketRadarCalc::ResultMapToArray(mapResult, aRes);
	CSingleLock lock(&m_LockResult, TRUE);
	int32 iAdded = 0;
	for ( int32 i=0; i < aRes.GetSize() ; i++ )
	{
		bool32 bAdd = false;
		for ( int32 j=0; j < m_aResAsync.GetSize() ; j++ )
		{
			if ( m_aResAsync[j].m_pMerch == aRes[i].m_pMerch )
			{
				if ( m_aResAsync[j].m_eCondition != aRes[i].m_eCondition
					|| m_aResAsync[j].m_iRiseFallFlag != aRes[i].m_iRiseFallFlag )
				{
					// 以前同样提示过的不重复提示, 
					// 防止其它动作数据变更引起的重计算结果导致抖动
					m_aResAsync.RemoveAt(j);		
					m_aResAsync.Add(aRes[i]);
					iAdded++;
				}
				bAdd = true;
				break;
			}
		}
		if ( !bAdd )
		{
			iAdded++;
			m_aResAsync.Add(aRes[i]);
		}

		m_mapMerchDels.erase(aRes[i].m_pMerch);	// 从删除队列中清除
	}
	lock.Unlock();
	if ( iAdded > 0 )
	{
		PostMessage(UM_MY_ADD_MERCH, 0, 0);
	}
}

void CDlgMarketRadarList::OnRemoveMarketRadarResult( CMerch *pMerch, E_RemoveCause eCause )
{
	//PostMessage(UM_MY_REMOVE_MERCH, (WPARAM)pMerch, (LPARAM)eCause);
	//if ( eCause == EMC_OverInitTime )	// 仅删除超过初始化时间的项
	{
		CSingleLock lock(&m_LockResult, TRUE);
		m_mapMerchDels[pMerch] = CGmtTime::GetCurrentTime();		// 超过指定时间从列表中删除
		lock.Unlock();
	}
}

void CDlgMarketRadarList::OnClearMarketRadarResult()
{
	
}

LRESULT CDlgMarketRadarList::OnMsgRemoveMerch( WPARAM w, LPARAM l )
{
	//m_mapMerchDels[(CMerch *)w] = CGmtTime::GetCurrentTime();
	return 1;
}

void CDlgMarketRadarList::OnTimer( UINT nIDEvent )
{
	if ( KTimerIdUpdateMerchInit == nIDEvent )
	{
		CMarketRadarCalc::Instance().OnTimerFilterNeedInitMerch();
	}
	else if ( KTimerIdCheckDelMerch == nIDEvent )
	{
		CGmtTimeSpan Span(0, 0, 1, 0);		// 超过这个时间则清除
		CGmtTime TimeNow = CGmtTime::GetCurrentTime();

		CSingleLock lock(&m_LockResult, TRUE);
		MerchArray aMerchsDel;
		int32 iDel = 0;
		for ( MerchTimeMap::iterator it=m_mapMerchDels.begin(); it != m_mapMerchDels.end() ; ++it )
		{
			if ( TimeNow-it->second > Span )
			{
				aMerchsDel.Add(it->first);
				for ( int32 i=0; i < m_aResAsync.GetSize() ; i++ )		// 从数据队列中移出
				{
					if ( m_aResAsync[i].m_pMerch == it->first )	
					{
						iDel++;
						m_aResAsync.RemoveAt(i);
						break;
					}
				}
			}
		}
		lock.Unlock();

		if ( iDel > 0 )
		{
			FillList();
		}
	}
	else if ( KTimerIdFillList == nIDEvent )
	{
		FillList();
	}

	_MYBASE_DLG_::OnTimer(nIDEvent);
}

void CDlgMarketRadarList::ShowMarketRadarList()
{
	static CDlgMarketRadarList dlg(AfxGetMainWnd());

	CWnd *pWndFore = CWnd::FromHandlePermanent(::GetForegroundWindow());
	CWnd *pWndMain = AfxGetMainWnd();
	if ( dlg.m_hWnd == NULL )
	{
		dlg.Create(IDD, AfxGetMainWnd());
		//dlg.CenterWindow();
		//dlg.ShowWindow(SW_SHOW);	// 任何xx都是关闭
// 		if ( pWndFore != pWndMain )
// 		{
// 			dlg.ShowWindow(SW_SHOWNA);
// 			pWndMain->ShowWindow(SW_SHOW);
// 		}
// 		else
// 		{
// 			dlg.ShowWindow(SW_SHOW);
// 		}
		bool32 bShow = true;
		if ( pWndFore == NULL )
		{
			// 在最小化的情况下，如果创建非模态对话框，出现激活时仅能激活对话框而不能激活mainframe情况
			WINDOWPLACEMENT wnp;
			pWndMain->GetWindowPlacement(&wnp);
			if ( wnp.showCmd == SW_SHOWMINIMIZED )
			{
				// 非本进程窗口, 如果主窗口被min, 则min的方法
				// 模仿spy++捕获的主窗口最小化时收到的消息- -
// 				CRect rc;
// 				dlg.GetWindowRect(rc);
// 				dlg.SetWindowPos(pWndMain, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOREDRAW);	// 缩小大小 ->显示
// 				dlg.SetWindowPos(pWndMain, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOMOVE|SWP_SHOWWINDOW);
// 				dlg.SendMessage(WM_SHOWWINDOW, 0, SW_PARENTCLOSING);
// 				dlg.SetWindowPos(pWndMain, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOMOVE|SWP_HIDEWINDOW); // 隐藏 -> 还原大小
// 				dlg.SetWindowPos(pWndMain, 0, 0, rc.Width(), rc.Height(), SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOREDRAW|SWP_NOZORDER);
				// 只能隐藏它，有其它方法没

				// 还原主窗口，然后显示对话框方法
				pWndMain->ShowWindow(SW_RESTORE);
				dlg.SetWindowPos(pWndMain, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
				bShow = false;

				// 或者开启定时器，查看主窗口是否脱离了min状态，脱离则显示
			}
			else
			{
				// 其它情况只要调整对话框的z
				dlg.SetWindowPos(pWndMain, 0,0,0,0, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
				bShow = false;
			}
		}
		if ( bShow )
		{
			dlg.ShowWindow(SW_SHOW);
		}
	}
	else if ( !dlg.IsWindowVisible() )
	{
		WINDOWPLACEMENT wnp;
		pWndMain->GetWindowPlacement(&wnp);
		if ( wnp.showCmd != SW_SHOWMINIMIZED )
		{
			dlg.ShowWindow(SW_SHOWNA);
		}
	}

	pWndFore = CWnd::FromHandlePermanent(::GetForegroundWindow());
	if ( pWndFore == NULL )
	{
		::FlashWindow(AfxGetMainWnd()->GetSafeHwnd(), 0x00000002); // taskbar闪烁
	}
	else/* if ( pWndFore == &dlg )*/
	{
		
	}
}

void CDlgMarketRadarList::OnBtnOpenClose()
{
	LONG lFlag = CMarketRadarCalc::Instance().GetRunFlag();
	if ( lFlag == 0 )
	{
		CMarketRadarCalc::Instance().StartCalc();
	}
	else
	{
		CMarketRadarCalc::Instance().StopCalc();
	}
	UpdateOpenCloseString();
	CMarketRadarCalc::Instance().SaveStartFlag();
}

void CDlgMarketRadarList::UpdateOpenCloseString()
{
	if ( CMarketRadarCalc::Instance().GetRunFlag() != 0 )
	{
		SetDlgItemText(IDC_BUTTON_OPENCLOSE, _T("关闭雷达"));
		CString StrTitle;
		SetWindowText(m_StrTitleOrg + _T(" - [正在运行]"));
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_OPENCLOSE, _T("运行雷达"));
		CString StrTitle;
		SetWindowText(m_StrTitleOrg + _T(" - [已关闭]"));
	}
}

void CDlgMarketRadarList::OnDestroy()
{
	CMarketRadarCalc::Instance().RemoveListener(this);
	if ( NULL != CBlockConfig::PureInstance() )
	{
		CBlockConfig::PureInstance()->RemoveListener(this);
	}
	_MYBASE_DLG_::OnDestroy();
}

CDlgMarketRadarList::~CDlgMarketRadarList()
{
	CMarketRadarCalc::Instance().RemoveListener(this);
}

void CDlgMarketRadarList::OnListDBLClick( NMHDR *pHdr, LRESULT *pResult )
{
	LPNMITEMACTIVATE pitem = (LPNMITEMACTIVATE)pHdr;
	if ( NULL != pitem )
	{
		int iIndex = m_List.HitTest(pitem->ptAction);
		if ( iIndex >= 0 )
		{
			iIndex = m_aResults.GetUpperBound()-iIndex;
			if ( iIndex >=0 && iIndex < m_aResults.GetSize() )
			{
				CMerch *pMerch = m_aResults[iIndex].m_pMerch;
				CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
				if ( NULL != pMainFrame && NULL != pMerch )
				{
					pMainFrame->OnShowMerchInChart(pMerch, NULL);
				}
			}
		}
	}
}

void CDlgMarketRadarList::OnListKeyDown( NMHDR *pHdr, LRESULT *pResult )
{
	LPNMLVKEYDOWN pKey = (LPNMLVKEYDOWN)pHdr;
	if ( NULL != pKey )
	{
		if ( VK_RETURN == pKey->wVKey )
		{
			OnEnterKey();
		}
	}
}

void CDlgMarketRadarList::OnEnterKey()
{
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if ( NULL != pos )
	{
		int iIndex = m_List.GetNextSelectedItem(pos);
		iIndex = m_aResults.GetUpperBound()-iIndex;
		if ( iIndex >=0 && iIndex < m_aResults.GetSize() )
		{
			CMerch *pMerch = m_aResults[iIndex].m_pMerch;
			CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
			if ( NULL != pMainFrame && NULL != pMerch )
			{
				pMainFrame->OnShowMerchInChart(pMerch, NULL);
			}
		}
	}
}

void CDlgMarketRadarList::RecalcLayout()
{
	if ( m_List.m_hWnd == NULL )
	{
		return;
	}
	
	CRect rc;
	GetClientRect(rc);
	if ( m_rcOrg.IsRectEmpty() )
	{
		m_rcOrg = rc;
	}
	
	if ( m_rcList.IsRectEmpty() )
	{
		m_List.GetWindowRect(m_rcList);
		ScreenToClient(&m_rcList);
	}
	

	{
		// 给按钮留一栏
		CRect rcList(rc);
		rcList.bottom = m_rcList.bottom + rc.bottom-m_rcOrg.bottom;
		m_List.MoveWindow(rcList);
	}
	
	if ( m_rcOpenClose.IsRectEmpty() )
	{
		::GetWindowRect(GetDlgItem(IDC_BUTTON_OPENCLOSE)->GetSafeHwnd(), &m_rcOpenClose);
		ScreenToClient(&m_rcOpenClose);
	}
	else
	{
		// 左下对齐
		CRect rcBtn(m_rcOpenClose);
		rcBtn.OffsetRect(rc.left-m_rcOrg.left, rc.bottom-m_rcOrg.bottom);
		::MoveWindow(GetDlgItem(IDC_BUTTON_OPENCLOSE)->GetSafeHwnd(), rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height(), TRUE);
	}
	
	if ( m_rcSave.IsRectEmpty() )
	{
		::GetWindowRect(GetDlgItem(IDC_BUTTON_SAVE)->GetSafeHwnd(), &m_rcSave);
		ScreenToClient(&m_rcSave);
	}
	else
	{
		// 右下对齐
		CRect rcBtn(m_rcSave);
		rcBtn.OffsetRect(rc.right-m_rcOrg.right, rc.bottom-m_rcOrg.bottom);
		::MoveWindow(GetDlgItem(IDC_BUTTON_SAVE)->GetSafeHwnd(), rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height(), TRUE);
	}
	
	if ( m_rcAnalyze.IsRectEmpty() )
	{
		::GetWindowRect(GetDlgItem(IDC_BUTTON_ANALYZE)->GetSafeHwnd(), &m_rcAnalyze);
		ScreenToClient(&m_rcAnalyze);
	}
	else
	{
		// 右下对齐
		CRect rcBtn(m_rcAnalyze);
		rcBtn.OffsetRect(rc.right-m_rcOrg.right, rc.bottom-m_rcOrg.bottom);
		::MoveWindow(GetDlgItem(IDC_BUTTON_ANALYZE)->GetSafeHwnd(), rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height(), TRUE);
	}
	
	if ( m_rcSetting.IsRectEmpty() )
	{
		::GetWindowRect(GetDlgItem(IDC_BUTTON_SETTING)->GetSafeHwnd(), &m_rcSetting);
		ScreenToClient(&m_rcSetting);
	}
	else
	{
		// 右下对齐
		CRect rcBtn(m_rcSetting);
		rcBtn.OffsetRect(rc.right-m_rcOrg.right, rc.bottom-m_rcOrg.bottom);
		::MoveWindow(GetDlgItem(IDC_BUTTON_SETTING)->GetSafeHwnd(), rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height(), TRUE);
	}
}

void CDlgMarketRadarList::OnSize( UINT nType, int cx, int cy )
{
	_MYBASE_DLG_::OnSize(nType, cx, cy);

	RecalcLayout();
}

void CDlgMarketRadarList::OnBtnSave()
{
	if ( m_List.GetItemCount() <= 0 )
	{
		return;
	}

	// 添加到板块/保存到文本
	CRect rcBtn(0,0,0,0);
	GetDlgItem(IDC_BUTTON_SAVE)->GetWindowRect(&rcBtn);

	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, 1, _T("将结果添加到板块"));
	menu.AppendMenu(MF_STRING, 2, _T("将结果另存为文本"));
	int iRet = (int)menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_TOPALIGN |TPM_NONOTIFY |TPM_RETURNCMD
						, rcBtn.left
						, rcBtn.bottom
						, this);
	if ( 1==iRet )
	{
		// 添加到板块
		T_Block block;
		if ( CDlgAddToBlock::GetUserSelBlock(block) )
		{
			for ( int32 i=0; i < m_aResults.GetSize() ; i++ )
			{
				CUserBlockManager::Instance()->AddMerchToUserBlock(m_aResults[i].m_pMerch, block.m_StrName, false);
			}
			// 商品多的时候, 提高性能, 只通知和保存文件一次.
			CUserBlockManager::Instance()->SaveXmlFile();
			CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);

			CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
			if ( NULL != pMainFrame )
			{
				T_BlockDesc blockDesc;
				blockDesc.m_eType = T_BlockDesc::EBTUser;
				blockDesc.m_StrBlockName = block.m_StrName;
				pMainFrame->OnSpecifyBlock(blockDesc);
			}
		}
	}
	else if ( 2==iRet )
	{
		// 文本
		CFileDialog dlg(FALSE, _T("txt")
			, NULL
			, OFN_ENABLESIZING|OFN_EXPLORER|OFN_NOCHANGEDIR|OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY
			, _T("文本文件 (*.txt)|*.txt|所有文件 (*.*)|*.*||"));
		if ( dlg.DoModal() == IDOK )
		{
			CString StrPath = dlg.GetPathName();
			CStdioFile file;
			CFileException expFile;
			if ( file.Open(StrPath, CFile::modeCreate |CFile::modeWrite, &expFile) )
			{
				for ( int32 i=0; i < m_List.GetItemCount() ; i++ )
				{
					for ( int32 j=0; j < m_List.GetHeaderCtrl()->GetItemCount() ; j++ )
					{
						CString StrValue;
						StrValue = m_List.GetItemText(i, j);
						StrValue += _T("\t");
						char *psz = CEtcXmlConfig::WideToMulti(StrValue);
						file.Write(psz, strlen(psz));
						delete []psz;
					}
					file.Write("\n", 1);
				}
				file.Close();

				if ( MessageBox(_T("导出成功，是否打开此文件？"), _T("提示"), MB_YESNO |MB_ICONQUESTION)
					== IDYES )
				{
					ShellExecute(NULL, _T("open"), StrPath, NULL, NULL, SW_SHOW);
				}
			}
			else
			{
				expFile.ReportError();
			}
		}
	}
}

void CDlgMarketRadarList::OnBtnAnalyse()
{
	// 调用图表
	OnEnterKey();
}

void CDlgMarketRadarList::OnBtnSetting()
{
	// 设置参数对话框
	CDlgMarketRadarSetting dlg;
	dlg.SetRadarParam(CMarketRadarCalc::Instance().GetParameter());
	if ( dlg.DoModal() == IDOK )
	{
		CMarketRadarCalc::Instance().SetParameter(dlg.GetRadarParam());
	}
}

CMerch		* CDlgMarketRadarList::GetSelectMerch()
{
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if ( NULL != pos )
	{
		int iIndex = m_List.GetNextSelectedItem(pos);
		iIndex = m_aResults.GetUpperBound()-iIndex;
		if ( iIndex >=0 && iIndex < m_aResults.GetSize() )
		{
			return m_aResults[iIndex].m_pMerch;
		}
	}
	return NULL;
}

void CDlgMarketRadarList::SelectMerch( CMerch *pMerch )
{
	if ( NULL == pMerch )
	{
		return;
	}
	for ( int32 i=0; i < m_aResults.GetSize() ; i++ )
	{
		int32 iIndex = m_aResults.GetUpperBound()-i;
		if ( m_aResults[iIndex].m_pMerch == pMerch )
		{
			//m_List.SetItemState(iIndex, LVIS_SELECTED |LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			break;
		}
	}
}

void CDlgMarketRadarList::OnBlockConfigChange( int32 iBlockId, E_BlockNotifyType eNotifyType )
{
	m_List.Invalidate();
}

void CDlgMarketRadarList::OnCancel()
{
	DestroyWindow();	// 销毁窗口
}


