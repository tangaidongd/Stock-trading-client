#include "StdAfx.h"

#include "DlgNotePad.h"
#include "MerchManager.h"

#include "IoViewBase.h"
#include "ShareFun.h"



#include "DlgNewNote.h"
#include "IoViewTrend.h"


#include "mmsystem.h"
#include "GmtTime.h"
#include "XmlShare.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CString KStrDefaultContent = _T("[无内容]");



/////////////////////////////////////////////////////////////////////////////
// CDlgNotePad dialog

CDlgNotePad::CDlgNotePad(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgNotePad::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNotePad)
	//}}AFX_DATA_INIT

	m_sizeMin.cx = 400;
	m_sizeMin.cy = 300;

	m_eShowType = EST_UserNote;

	m_RectSeparator.SetRectEmpty();

	m_pMerchCur = NULL;

	m_bHistoryMine = true;

	m_aBtnIds.Add(IDC_BUTTON_SWITCH);
	m_aBtnIds.Add(IDC_BUTTON_NEWNOTE);
	m_aBtnIds.Add(IDC_BUTTON_MODIFY);
	m_aBtnIds.Add(IDC_BUTTON_DEL);
	m_aBtnIds.Add(IDC_BUTTON_CLEARNOTES);
// 	m_aBtnIds.Add(IDC_BUTTON_FLOAT);
// 	m_aBtnIds.Add(IDC_BUTTON_HIDE);

	m_eNewsType = ENTReqLandMine;
}


void CDlgNotePad::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNotePad)
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_STATIC_TITLE, m_StaticTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNotePad, CDialogEx)
//{{AFX_MSG_MAP(CDlgNotePad)
ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList)
ON_WM_SIZE()
ON_WM_CONTEXTMENU()
ON_WM_GETMINMAXINFO()
ON_WM_CLOSE()
ON_WM_DESTROY()
ON_WM_LBUTTONDBLCLK()
ON_WM_NCHITTEST()
ON_MESSAGE_VOID(UM_NOTE_MERCHHASCHANGED, OnMerchChanged)
ON_MESSAGE_VOID(UM_NOTE_LANDTYPEHASCHANGED, OnLandTypeChanged)
ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
ON_BN_CLICKED(IDC_BUTTON_NEWNOTE, OnBtnNew)
ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnBtnModify)
ON_BN_CLICKED(IDC_BUTTON_DEL, OnBtnDel)
ON_BN_CLICKED(IDC_BUTTON_CLEARNOTES, OnBtnClear)
ON_BN_CLICKED(IDC_BUTTON_SWITCH, OnBtnSwitch)
ON_BN_CLICKED(IDC_BUTTON_FLOAT, OnBtnFloat)
ON_BN_CLICKED(IDC_BUTTON_HIDE, OnBtnHide)
ON_MESSAGE_VOID(TEST_VIEWDATA_INFOMINELIST_RESP, OnMsgTestViewDataListResp)
ON_MESSAGE_VOID(TEST_VIEWDATA_INFOMINECONTENT_RESP, OnMsgTestViewDataContentResp)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgNotePad::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString StrTitle;
	GetWindowText(StrTitle);
	SetMyTitle(StrTitle);

	m_eShowType = EST_UserNote;
	m_RectSeparator.SetRectEmpty();
	m_aUserNotes.clear();

	CFont *pFontOld = m_StaticTitle.GetFont();
	ASSERT( NULL != pFontOld );
	LOGFONT lf;
	pFontOld->GetLogFont(&lf);
	if ( abs(lf.lfHeight) < 15 )
	{
		lf.lfHeight = -15;
	}
	m_fontEdit.DeleteObject();
	m_fontEdit.CreateFontIndirect(&lf);

	m_MerchCur.m_StrMerchCode.Empty();	// 清除商品信息

	//IDC_STATIC为系统定义的，忽略整形到无符号整形的转换精度问题
	//lint --e(570)
	m_RichEdit.Create(WS_CHILD |ES_MULTILINE |ES_READONLY |WS_TABSTOP  |ES_AUTOVSCROLL |WS_VSCROLL, CRect(0,0,0,0), this, IDC_STATIC);
	//m_RichEdit.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	m_RichEdit.SetFont(&m_fontEdit);

	m_StaticTitle.ModifyStyle(0, SS_WORDELLIPSIS);

	m_List.SetFont(&m_fontEdit);

// 	if (!m_WebBrowser2.Create(L"", WS_CHILD, CRect(0, 0, 0, 0), this, IDC_STATIC))
// 	{
// 		AfxMessageBox(_T("无法启动浏览器!"));
// 		OnClose();
// 	}
// 	else
// 	{
// 		// 等待外部调用初始化数据
// 	}

	RecalcLayout();

	CGGTongDoc *pDoc = AfxGetDocument();
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return FALSE;
	}
	pAbsCenterManager->AddViewNewsListner(this);
	
	
	return TRUE;
}

void CDlgNotePad::OnSize( UINT nType, int cx, int cy )
{
	CDialogEx::OnSize(nType, cx, cy);
	RecalcLayout();
}


void CDlgNotePad::RecalcLayout()
{
	if ( NULL != m_List.m_hWnd )
	{
		if ( m_RectSeparator.IsRectEmpty() )
		{
			CWnd *pWnd = GetDlgItem(IDC_STATIC_SEPARATOR);
			ASSERT( NULL != pWnd );
			if ( NULL != pWnd )
			{
				pWnd->GetWindowRect(m_RectSeparator);
				ScreenToClient(&m_RectSeparator);
				m_RectSeparator.top = m_RectSeparator.bottom;
			}
			else
			{
				// 应该有的，只能赋予默认值了
				m_RectSeparator.top = m_RectSeparator.bottom = 30;
			}
		}
		
		if ( m_RectSeparator.top == 0 )
		{
			return;	// 此时还不能分配
		}

		CRect rcClient;
		GetClientRect(rcClient);

		CWnd *pWnd = GetDlgItem(IDC_STATIC_SEPARATOR);
		ASSERT( NULL != pWnd );
		if ( NULL != pWnd )
		{
			CRect rcSep(0,0,0,0);
			pWnd->GetWindowRect(rcSep);
			ScreenToClient(&rcSep);
			rcSep.right = rcClient.right;
			pWnd->MoveWindow(rcSep);
		}
		
		CRect rcList(rcClient);
		rcList.top = m_RectSeparator.bottom;
		m_List.MoveWindow(rcList);
		//m_WebBrowser2.MoveWindow(rcList);
		m_RichEdit.MoveWindow(rcList);
		
		// 几个按钮的位置
		ASSERT( m_aBtnIds.GetSize() > 0 );
		if ( m_aBtnIds.GetSize() > 0 )
		{
			CWnd *pWndBtn = GetDlgItem(m_aBtnIds[0]);
			ASSERT( NULL !=pWndBtn );
			if ( NULL != pWndBtn )
			{
				CRect rcBtn;
				pWndBtn->GetWindowRect(rcBtn);
				if ( rcBtn.Width() > 0 )
				{
					// 现在可以移动了
					int iHeight = rcBtn.Height();
					int iWidth = rcBtn.Width();
					rcBtn.top = rcClient.top + (m_RectSeparator.top-rcClient.top-rcBtn.Height())/2;
					rcBtn.bottom = rcBtn.top + iHeight;
					rcBtn.right = rcClient.right - rcBtn.Width()/2;
					rcBtn.left = rcBtn.right - iWidth;
					for ( int i=m_aBtnIds.GetUpperBound(); i >= 0 ; i-- )
					{
						pWndBtn = GetDlgItem(m_aBtnIds[i]);
						if ( NULL != pWndBtn )
						{
							pWndBtn->MoveWindow(rcBtn);
							rcBtn -= CPoint(iWidth+4, 0);
						}
					}
					
					// 移动static title
					CRect rcStaticTitle;
					m_StaticTitle.GetWindowRect(rcStaticTitle);
					ScreenToClient(&rcStaticTitle);
					rcStaticTitle.right = rcBtn.right-4;
					m_StaticTitle.MoveWindow(rcStaticTitle);
				}
			}
		}
		
		Invalidate();
	}
}


void CDlgNotePad::OnClose()
{
// 	m_aUserNotes.clear();
// 	DestroyWindow();	// 毁灭窗口
//这里不应该直接销毁，隐藏就好了
	if ( GetStyle()&WS_CHILD )
	{
		CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
		pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, pMainFrame);
		if ( NULL!=pMainFrame )
		{
			pMainFrame->ShowDockSubWnd(NULL);
		}
	}
}

void CDlgNotePad::OnDestroy()
{
	// 清除一些数据
	CGGTongDoc *pDoc = AfxGetDocument();
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return;
	}

	pAbsCenterManager->DelViewNewsListner(this);
	CDialogEx::OnDestroy();
}

void CDlgNotePad::OnGetMinMaxInfo( MINMAXINFO* lpMMI )
{
	lpMMI->ptMinTrackSize.x = m_sizeMin.cx;
	lpMMI->ptMinTrackSize.y = m_sizeMin.cy;
}

void CDlgNotePad::OnDblclkList()
{
	// 切换
	SwitchShow();
}

void CDlgNotePad::SwitchShow()
{
	if ( IsInListMode() )
	{
		// 找到当前选中的项目，调用html显示
		if ( EST_UserNote == m_eShowType )
		{
			T_UserNoteInfo note;
			int iSel = GetListSelUserNote(note);
			if ( LB_ERR != iSel )
			{
				// 有选择的
				ShowHtml(note, true);	// 显示这个内容, 貌似可以不必每次都读取
			}
			else
			{
				// 没有选择项，切换失败
				return;
			}
		}
		else
		{
			CLandMineTitle title;
			if ( GetListSelInfoMine(title) != LB_ERR )
			{
				ShowHtml(title);		// 没有的话，会自动发送, 信息地雷不会出现更改的情况
			}
			else
			{
				return;		// 无选择，切换失败
			}
		}
	}
	else
	{
		// 切换回list显示模式
		ShowList();
	}
}

void CDlgNotePad::ShowList()
{
	if ( EST_UserNote == m_eShowType )
	{	
		//m_WebBrowser2.ShowWindow(SW_HIDE);
		m_RichEdit.ShowWindow(SW_HIDE);
		//m_StaticTitle.ShowWindow(SW_HIDE);
		m_List.ShowWindow(SW_SHOW);
		if ( &m_RichEdit == GetFocus() )
		{
			m_List.SetFocus();
		}
	}
	else
	{
		m_RichEdit.ShowWindow(SW_HIDE);
		//m_StaticTitle.ShowWindow(SW_HIDE);
		m_List.ShowWindow(SW_SHOW);
		if ( &m_RichEdit == GetFocus() )
		{
			m_List.SetFocus();
		}
	}

	CString StrTitle;
	GetWindowText(StrTitle);
	m_StaticTitle.SetWindowText(StrTitle);
}

LRESULT CDlgNotePad::OnIdleUpdateCmdUI( WPARAM w, LPARAM l )
{
	// 进入空闲时间，更新下按钮的状态，查看商品是否更新
	WatchMerchIsChanged();

	UpdateBtnStatus();

	return 0;
}


void CDlgNotePad::RefreshList()
{
	if ( EST_UserNote == m_eShowType )
	{
		// 获取该商品的所有投资日记，填充列表	
		CUserNoteInfoManager::Instance().GetUserNotesHeaderArray(m_MerchCur, m_aUserNotes);
		
		{
			CAutoLockWindowUpdate lockDraw(&m_List);
			int32 iUniqueOld = 0;
			int32 iSelOld = m_List.GetCurSel();
			if ( LB_ERR != iSelOld )
			{
				iUniqueOld = (int32)m_List.GetItemData(iSelOld);
			}
			bool32 bFindOld = false;
			int32 iLineCount = 0;
			
			m_List.ResetContent();
			for ( CUserNoteInfoManager::UserNotesArray::reverse_iterator it = m_aUserNotes.rbegin();
			it != m_aUserNotes.rend() ; ++it)
			{
				int iLine = m_List.AddString(MakeListItemDesc(*it));
				if ( LB_ERR != iLine )
				{
					iLineCount++;
					m_List.SetItemData(iLine, it->m_iUniqueId);	// 还是每次遍历吧
					if ( iSelOld != LB_ERR && iUniqueOld == it->m_iUniqueId )
					{
						m_List.SetCurSel(iLine);
						bFindOld = true;
					}
				}
				else
				{
					ASSERT( 0 );
				}
			}
			if ( !bFindOld )
			{
				if ( iLineCount > 0 )
				{
					m_List.SetCurSel(0);
				}
				else
				{
					// 没有数据
				}
			}
		}
		m_List.Invalidate();

		// 当前模式如果是显示内容的，则看要查看的内容存在否，不存在，则返回列表
		if ( !IsInListMode() )
		{
			T_UserNoteInfo note;
			if ( LB_ERR == GetListSelUserNote(note) )
			{
				// 无内容选定
				ShowList();
			}
			else
			{
				ShowHtml(note, true);	// 重新显示内容
			}
		}

		UpdateBtnStatus();
	}
	else
	{
		// 从当前商品的信息地雷列表中获取信息地雷列表，显示
		int32 iSelOld = m_List.GetCurSel();
		CString StrSel;
		if ( iSelOld != LB_ERR )
		{
			// 找到现在的那个选择的项
			m_List.GetText(iSelOld, StrSel);
		}

		m_mapInfoMines.clear();
		if ( NULL != m_pMerchCur )
		{
			m_mapInfoMines = m_pMerchCur->m_mapLandMine;		// 赋值当前商品的
		}

		CAutoLockWindowUpdate lockDraw(&m_List);
		{
			m_aInfoMines.SetSize(0, m_mapInfoMines.size());	// 重设数组的
			m_List.ResetContent();
			
			// 读取并插入信息
			FilterInfoMind();

			for ( int32 i=0; i < m_aInfoMines.GetSize() ; ++i )
			{
				const CLandMineTitle &title = m_aInfoMines[i];
				
				CString Str = MakeListItemDesc(title);
				m_List.AddString(Str);
			}
			
			if ( iSelOld != LB_ERR )
			{
				// 看是否有相同的字串, 有就选择
				int iSel = m_List.SelectString(-1, StrSel);
				if ( iSel < 0 )
				{
					m_List.SetCurSel(0);
				}
				else
				{
					// 是否需要滚动？
				}
			}
			else
			{
				m_List.SetCurSel(0);
			}
			
		}
		m_List.Invalidate();

		// 当前显示模式
		if ( !IsInListMode() )
		{
			// 如果现在
			CLandMineTitle title;
			if ( GetListSelInfoMine(title) == LB_ERR )
			{
				SwitchShow();	// 切换显示为列表
			}
			else
			{
				ShowHtml(title);	// 更新显示内容
			}
		}
	}
}

void CDlgNotePad::OnMerchChanged()
{
	// 商品更新了，切换所有的显示
	// 当前商品是否是有效商品
	if ( m_MerchCur.m_StrMerchCode.IsEmpty() )
	{
		return;	// 不处理
	}

	//SetWindowText(GetCurDlgTitle());
	SetMyTitle(GetCurDlgTitle());

	ShowList();	// 商品切换显示列表
	RefreshList();

	if ( m_eShowType == EST_InfoMine )
	{
		// 请求该商品的列表数据
		if ( NULL != m_pMerchCur )
		{
			RequestViewDataInfoMineList(m_pMerchCur, NULL);
		}
	}
}

void CDlgNotePad::UpdateBtnStatus()
{
	int iSel = m_List.GetCurSel();
	BOOL bSel = LB_ERR != iSel;
	if ( EST_UserNote == m_eShowType )
	{
		::EnableWindow(GetDlgItem(IDC_BUTTON_SWITCH)->GetSafeHwnd(), bSel);
		::EnableWindow(GetDlgItem(IDC_BUTTON_MODIFY)->GetSafeHwnd(), bSel);
		::EnableWindow(GetDlgItem(IDC_BUTTON_DEL)->GetSafeHwnd(), bSel);
		::EnableWindow(GetDlgItem(IDC_BUTTON_NEWNOTE)->GetSafeHwnd(), TRUE);
		::EnableWindow(GetDlgItem(IDC_BUTTON_CLEARNOTES)->GetSafeHwnd(), m_List.GetCount() > 0);
	}
	else
	{
		// 切换 [发布 修改 删除] 收藏[目前无此功能]
		// 不需要变换名称
		::EnableWindow(GetDlgItem(IDC_BUTTON_SWITCH)->GetSafeHwnd(), bSel);
		::EnableWindow(GetDlgItem(IDC_BUTTON_NEWNOTE)->GetSafeHwnd(), FALSE);
		::EnableWindow(GetDlgItem(IDC_BUTTON_MODIFY)->GetSafeHwnd(), FALSE);
		::EnableWindow(GetDlgItem(IDC_BUTTON_DEL)->GetSafeHwnd(), FALSE);
		
		::EnableWindow(GetDlgItem(IDC_BUTTON_CLEARNOTES)->GetSafeHwnd(), FALSE);
	}
}

CString CDlgNotePad::MakeListItemDesc( const T_UserNoteInfo &note )
{
	static CStringArray aDayInWeek;
	if ( aDayInWeek.GetSize() <= 0 )
	{
		aDayInWeek.Add(_T("星期日"));
		aDayInWeek.Add(_T("星期一"));
		aDayInWeek.Add(_T("星期二"));
		aDayInWeek.Add(_T("星期三"));
		aDayInWeek.Add(_T("星期四"));
		aDayInWeek.Add(_T("星期五"));
		aDayInWeek.Add(_T("星期六"));
	}
	tm ttm;
	note.m_Time.GetLocalTm(&ttm);
	
	CString StrTitle = note.m_StrTitle;
	CString Str;
	Str.Format(
		_T("%04d.%02d.%02d %02d:%02d:%02d [%s %s]  %s"),
		ttm.tm_year + 1900, ttm.tm_mon+1, ttm.tm_mday, ttm.tm_hour, ttm.tm_min, ttm.tm_sec,
		aDayInWeek[ttm.tm_wday].GetBuffer(), GetUserNoteWeatherString((E_UserNoteWeather)note.m_iWeather).GetBuffer(),
		StrTitle.GetBuffer()
		);

	return Str;
}

CString CDlgNotePad::MakeListItemDesc( const CLandMineTitle &mineTitle )
{
	// 2010.01.01[为塞特] xxxxxx
	// 当前显示 2010-01-01 xxxxxx(xxxByte)
	CString Str;
	CString StrTime = mineTitle.m_StrUpdateTime;
	int32 iSep = -1;
	// 历史信息与实时区别 历史显示日期，实时显示时间
	// 此时间为gmt时间
	int32 iYear, iMon, iDay, iHour, iMin, iSec;
	iYear = iMon = iDay = iHour = iMin = iSec = 0;
	int32 iFmt = _stscanf(StrTime, _T("%04d-%02d-%02d %02d:%02d:%02d"), &iYear, &iMon, &iDay, &iHour, &iMin, &iSec);
	ASSERT( iFmt == 6 );
	if ( iFmt >= 3 )
	{
		CGmtTime TimeShowGmt(iYear, iMon, iDay, iHour, iMin, iSec);
		CTime TimeShow(TimeShowGmt.GetTime());
		if ( m_bHistoryMine )
		{
			StrTime.Format(_T("%04d-%02d-%02d"), TimeShow.GetYear(), TimeShow.GetMonth(), TimeShow.GetDay());
		}
		else
		{
			StrTime.Format(_T("%02d:%02d:%02d"), TimeShow.GetHour(), TimeShow.GetMinute(), TimeShow.GetSecond());
		}	
	}
	else
	{
		if ( (iSep=StrTime.Find(_T(' '))) >= 0 )
		{
			if ( m_bHistoryMine )
			{
				StrTime = StrTime.Left(iSep);
			}
			else if ( iSep+1 < StrTime.GetLength() )
			{
				StrTime = StrTime.Mid(iSep+1);
			}
		}
	}

	CString StrText = mineTitle.m_StrText;
	Str.Format(
		_T("%s  %s (%dB)"),
		StrTime.GetBuffer(),
		StrText.GetBuffer(),
		mineTitle.m_iConLen
		);
	
	return Str;
}

void CDlgNotePad::OnNewNote()
{
	// 新建一个日记
	// 应该是交给edit对话框，用户编辑输入，并添加，这里只要返回结果就可以了
	T_UserNoteInfo note;
	
	CUserNoteInfoManager::InitNewUserNote(note);
	note.m_MerchKey = m_MerchCur;
	if ( !CDlgNewNote::ModifyNote(note) )
	{
		return;	// 取消
	}

	bool32 bAdded = FALSE;
	if ( CUserNoteInfoManager::Instance().NewUserNote(note) )
	{
		CUserNoteInfoManager::Instance().GetUserNotesHeaderArray(m_MerchCur, m_aUserNotes);	// 刷新列表信息

		int32 iLine = 0;
		for ( CUserNoteInfoManager::UserNotesArray::reverse_iterator it = m_aUserNotes.rbegin(); it != m_aUserNotes.rend() ; ++it )
		{
			int32 iId = (int32)m_List.GetItemData(iLine);
			if ( iId != it->m_iUniqueId )	// 应当是同序相等的
			{
				// 不等，则到了新添加的位置了
				iLine = m_List.InsertString(iLine, MakeListItemDesc(*it));
				if ( LB_ERR != iLine )
				{
					m_List.SetItemData(iLine, (DWORD)it->m_iUniqueId);
					bAdded = TRUE;
				}
				break;
			}
			iLine++;	// 检查下一个
		}
		
		if ( !bAdded )
		{
			RefreshList();	// 只能刷新了
			bAdded = TRUE;
		}
	}

	if ( bAdded )
	{
		MessageBox(_T("添加投资日记成功"));
	}
	else
	{
		MessageBox(_T("添加投资日记失败！"));
	}
}

void CDlgNotePad::OnModifyNote()
{
	int iSel = m_List.GetCurSel();
	if ( LB_ERR == iSel )
	{
		return;
	}

	int32 iId = (int32)m_List.GetItemData(iSel);
	bool32 bFind = FALSE;
	T_UserNoteInfo note;
	T_UserNoteInfo noteOld;
	for ( CUserNoteInfoManager::UserNotesArray::reverse_iterator it = m_aUserNotes.rbegin(); it != m_aUserNotes.rend() ; ++it )
	{
		if ( iId == it->m_iUniqueId )
		{
			note = *it;
			noteOld = note;
			bFind = true;
			break;
		}
	}

	if ( !bFind )
	{
		return;
	}

	// 尝试加载
	if ( note.m_StrContent.IsEmpty() )
	{
		// 尝试加载
		CUserNoteInfoManager::Instance().ReadUserNoteContent(note);
	}

	ShowHtml(note);	// 先显示

	if ( !CDlgNewNote::ModifyNote(note) )
	{
		return;	// 没有修改
	}

	if ( CUserNoteInfoManager::Instance().ModifyUserNote(note) )
	{
		CUserNoteInfoManager::Instance().GetUserNotesHeaderArray(m_MerchCur, m_aUserNotes);
		if ( note.m_Time != noteOld.m_Time )
		{
			// 需要重新插入队列
			RefreshList();
			
			if ( !IsInListMode() )
			{
				// 重新显示html内容
				ShowHtml(note);
			}
		}
		else
		{
			// 仅更新内容
			UpdateUserNote(note);
		}

		MessageBox(_T("修改投资日记成功"));
	}
	else
	{
		ASSERT( 0 );
	}
}

void CDlgNotePad::OnDelNote()
{
	int iSel = m_List.GetCurSel();
	if ( LB_ERR == iSel )
	{
		return;
	}
	
	int32 iId = (int32)m_List.GetItemData(iSel);
	bool32 bFind = FALSE;
	T_UserNoteInfo note;
	for ( CUserNoteInfoManager::UserNotesArray::reverse_iterator it = m_aUserNotes.rbegin(); it != m_aUserNotes.rend() ; ++it )
	{
		if ( iId == it->m_iUniqueId )
		{
			note = *it;
			bFind = true;
			break;
		}
	}
	
	if ( !bFind )
	{
		return;
	}
	
	m_List.DeleteString(iSel);
	CUserNoteInfoManager::Instance().DelUserNote(note);

	CUserNoteInfoManager::Instance().GetUserNotesHeaderArray(m_MerchCur, m_aUserNotes);
	
	RefreshList();

	ShowList();		// 被删除了，显示list
}

void CDlgNotePad::OnClearNote()
{
	m_List.ResetContent();
	m_aUserNotes.clear();

	CUserNoteInfoManager::Instance().ClearUserAllNote(m_MerchCur);

	ShowList();	// 显示list
}

void CDlgNotePad::WatchMerchIsChanged(bool32 bNotify /*= true*/)
{
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( NULL != pMainFrame )
	{
		CIoViewBase *pIo = pMainFrame->FindActiveIoView();
		if ( NULL != pIo )
		{
			CMerch *pMerch = pIo->GetMerchXml();
			if ( NULL == pMerch )
			{
				return;
			}

			if ( !m_MerchCur.IsSameMerch(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode) )
			{
				m_MerchCur.m_iMarketId    = pMerch->m_MerchInfo.m_iMarketId;
				m_MerchCur.m_StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
				m_pMerchCur = pMerch;

				if ( bNotify )
				{
					PostMessage(UM_NOTE_MERCHHASCHANGED, 0, 0);
				}
			}

			bool32 bRealTimeLandIoView = pIo->IsKindOf(RUNTIME_CLASS(CIoViewTrend));
			if ( EST_InfoMine == m_eShowType && (m_bHistoryMine ? bRealTimeLandIoView : !bRealTimeLandIoView) )
			{
				// 历史与实时变更了, 并且有商品
				ASSERT( pMerch != NULL );
				if ( bNotify )
				{
					PostMessage(UM_NOTE_LANDTYPEHASCHANGED, 0, 0);
				}
			}
		}
	}
}

int CDlgNotePad::GetListSelUserNote( OUT T_UserNoteInfo &note )
{
	int iSel = m_List.GetCurSel();
	if ( LB_ERR == iSel )
	{
		return iSel;	//  LB_ERR 未选择
	}
	
	int32 iId = (int32)m_List.GetItemData(iSel);
	bool32 bFind = FALSE;
	for ( CUserNoteInfoManager::UserNotesArray::reverse_iterator it = m_aUserNotes.rbegin(); it != m_aUserNotes.rend() ; ++it )
	{
		if ( iId == it->m_iUniqueId )
		{
			note = *it;
			bFind = true;
			break;
		}
	}
	
	if ( !bFind )
	{
		return LB_ERR;
	}

	return iSel;	// 返回选择的列表索引
}

void CDlgNotePad::OnBtnNew()
{
	if ( EST_UserNote == m_eShowType )
	{
		OnNewNote();
	}
	else
	{
		ASSERT( 0 );
	}
}

void CDlgNotePad::OnBtnModify()
{
	if ( EST_UserNote == m_eShowType )
	{
		OnModifyNote();
	}
	else
	{
		ASSERT( 0 );
	}
}

void CDlgNotePad::OnBtnDel()
{
	if ( EST_UserNote == m_eShowType )
	{
		if ( AfxMessageBox(_T("您确定删除该投资日记吗？"), MB_ICONQUESTION |MB_YESNO) != IDYES )
		{
			return;
		}

		OnDelNote();
	}
	else
	{
		ASSERT( 0 );
	}
}

void CDlgNotePad::OnBtnClear()
{
	if ( EST_UserNote == m_eShowType )
	{
		if ( AfxMessageBox(_T("您确定清除该商品的所有投资日记吗？"), MB_ICONQUESTION |MB_YESNO) != IDYES )
		{
			return;
		}

		OnClearNote();
	}
	else
	{
		ASSERT( 0 );
	}
}

void CDlgNotePad::OnBtnSwitch()
{
	SwitchShow();
}

void CDlgNotePad::OnOK()
{
	// nothing
}

bool32 CDlgNotePad::UpdateUserNote(const T_UserNoteInfo &note)
{
	// 列表更新
	const int iCount = m_List.GetCount();
	const int iSel = m_List.GetCurSel();
	bool32	bUpdate = false;
	bool32  bCurSel = false;
	for ( int i=0; i < iCount ; i++ )
	{
		int32 iId = (int32)m_List.GetItemData(i);
		if ( iId == note.m_iUniqueId )
		{
			bUpdate = true;

			m_List.DeleteString(i);
			m_List.InsertString(i, MakeListItemDesc(note));
			m_List.SetItemData(i, note.m_iUniqueId);

			if ( iSel == i )
			{
				m_List.SetCurSel(iSel);
				bCurSel = true;
			}

			break;
		}
	}

	if ( bCurSel && bUpdate && !IsInListMode() )
	{
		// 更新html显示
		ShowHtml(const_cast<T_UserNoteInfo &>(note));
	}

	return bUpdate;
}

void CDlgNotePad::ShowHtml(T_UserNoteInfo &note, bool32 bTryReadContent/* = false*/)
{
	m_List.ShowWindow(SW_HIDE);
	m_RichEdit.ShowWindow(SW_SHOW);
	m_StaticTitle.ShowWindow(SW_SHOW);
	if ( &m_List == GetFocus() )
	{
		m_RichEdit.SetFocus();
	}

	if ( bTryReadContent && note.m_StrContent.IsEmpty() )
	{
		// 尝试加载
		CUserNoteInfoManager::Instance().ReadUserNoteContent(note);
	}

	m_StaticTitle.SetWindowText(MakeListItemDesc(note));
	if ( note.m_StrContent.IsEmpty() )
	{
		m_RichEdit.SetWindowText(KStrDefaultContent);
	}
	else
	{
		m_RichEdit.SetWindowText(note.m_StrContent);
	}
}

void CDlgNotePad::ShowHtml(const CLandMineTitle &title, bool32 bTryReq /*= false*/ )
{
	m_List.ShowWindow(SW_HIDE);
	m_RichEdit.ShowWindow(SW_SHOW);
	m_StaticTitle.ShowWindow(SW_SHOW);
	if ( &m_List == GetFocus() )
	{
		m_RichEdit.SetFocus();
	}

	if ( title.m_stContent.m_iConLen == 0 || bTryReq )
	{
		// 请求数据 TODO
		RequestViewDataInfoMineContent(m_pMerchCur, title);
	}

	m_StaticTitle.SetWindowText(MakeListItemDesc(title));
	m_RichEdit.SetWindowText(title.m_stContent.m_StrText);
}

void CDlgNotePad::InitShowType( E_ShowType eShowType, CGmtTime *pTimeInit /*= NULL*/, CIoViewBase *pEventIoView/*=NULL*/ )
{
	if ( EST_UserNote == eShowType )
	{
		m_eShowType = eShowType;
		ShowList();
		WatchMerchIsChanged(false);
		OnMerchChanged();

		// 查找该日期的项，并选择
		if ( NULL != pTimeInit )
		{
			CGmtTime TimeDay(*pTimeInit);
			SaveDay(TimeDay);
			int iLine = 0;
			for ( CUserNoteInfoManager::UserNotesArray::reverse_iterator it = m_aUserNotes.rbegin();
			it != m_aUserNotes.rend() ; ++it)
			{
				CGmtTime TimeC(it->m_Time);
				SaveDay(TimeC);
				if ( TimeC == TimeDay )
				{
					m_List.SetCurSel(iLine);
					m_List.SetTopIndex(iLine);
					break;
				}
				iLine++;
			}
		}

		UpdateBtnStatus();
	}
	else
	{
		m_eShowType = eShowType;
		m_bHistoryMine = true;
		m_TimeInit = CGmtTime(0);
		if ( NULL == pEventIoView )
		{
			CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
			if ( NULL != pMainFrame )
			{
				pEventIoView = pMainFrame->FindActiveIoView();
			}
		}
		if ( NULL != pEventIoView )
		{
			// 分时图下的显示为实时信息地雷
			m_bHistoryMine = !pEventIoView->IsKindOf(RUNTIME_CLASS(CIoViewTrend));
			if ( !m_bHistoryMine )
			{
				if ( NULL != pTimeInit )
				{
					m_TimeInit = *pTimeInit;
				}
				else
				{
					// 分时图的当前时间
					CIoViewTrend *pTrend = DYNAMIC_DOWNCAST(CIoViewTrend, pEventIoView);
					ASSERT( NULL != pTrend );
					CMarketIOCTimeInfo IOCTime;
					if ( pTrend->GetCurrentIOCTime(IOCTime) )
					{
						m_TimeInit = IOCTime.m_TimeOpen.m_Time;	// 取开盘时间
					}
					else
					{
						// 可能这个分时图没有初始化
						// 无法获取实时信息，那么就当作历史的处理了
						m_bHistoryMine = true;
					}
				}
				if ( !m_bHistoryMine )
				{
					// 当前时间
				}
			}
		}
		ShowList();
		WatchMerchIsChanged(false);	// 获取当前的商品
		OnMerchChanged();		// 切换商品

		// 查找该日期的项，并选择
		// 以后切换了商品就不管了
		if ( NULL != pTimeInit )
		{
			CTime TimeDay(pTimeInit->GetTime());

			// 祈祷这个GMT时间不是00分吧~~
			// 目前仅显示历史信息地雷
			
			CString StrTime;
			StrTime.Format(_T("%02d:%02d:%02d")
				, TimeDay.GetHour(), TimeDay.GetMinute(), TimeDay.GetSecond() );
			// 从当前商品的信息地雷列表中找到一个，select
			// 先搜索实时的然后历史的搜索, 现在其实没有实时显示
			if ( m_List.SelectString(-1, StrTime) < 0 )
			{
				StrTime.Format(_T("%04d-%02d-%02d"), TimeDay.GetYear(), TimeDay.GetMonth(), TimeDay.GetDay());
				m_List.SelectString(-1, StrTime);
			}
		}
		
		UpdateBtnStatus();
	}
}

auto_ptr<CDlgNotePad> CDlgNotePad::s_pDlg;		// 静态的对象
void CDlgNotePad::ShowNotePad( E_ShowType eShowType, CGmtTime *pTimeInit /*= NULL*/, CIoViewBase *pEventIoView/*=NULL*/ )
{
	if ( s_pDlg.get() == NULL )
	{
		auto_ptr<CDlgNotePad> ptAuto(new CDlgNotePad);
		s_pDlg = ptAuto;
	}
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( s_pDlg->m_hWnd == NULL )
	{
		s_pDlg->Create(IDD, pMainFrame);
		//s_pDlg->CenterWindow();
	}
	if ( NULL!=pMainFrame )
	{
		pMainFrame->AddDockSubWnd(s_pDlg.get());
		pMainFrame->ShowDockSubWnd(s_pDlg.get());
	}

	s_pDlg->InitShowType(eShowType, pTimeInit, pEventIoView);
	s_pDlg->ShowWindow(SW_SHOW);
}

void CDlgNotePad::OnContextMenu( CWnd* pWnd, CPoint pos )
{
	CRect rcWin;
	m_List.GetWindowRect(rcWin);
	if ( !rcWin.PtInRect(pos) && pos.x >= 0 )
	{
		return;	// 非区域内
	}
	if ( !rcWin.PtInRect(pos) )
	{
		pos = rcWin.TopLeft();
	}

	CMenu menu;
	// 菜单: 切换 撰写 修改 删除 清空
	// 根据情况选择合适的
	menu.CreatePopupMenu();
	for ( int32 i=0; i < m_aBtnIds.GetSize() ; i++ )
	{
		CWnd *pWndBtn = GetDlgItem(m_aBtnIds[i]);
		if ( NULL != pWndBtn )
		{
			CString StrBtn;
			GetDlgItemText(m_aBtnIds[i], StrBtn);
			menu.AppendMenu(MF_STRING, m_aBtnIds[i], StrBtn);
			if ( !pWndBtn->IsWindowEnabled() )
			{
				menu.EnableMenuItem(m_aBtnIds[i], MF_DISABLED|MF_GRAYED |MF_BYCOMMAND);
			}
			if ( IDC_BUTTON_SWITCH == m_aBtnIds[i] )
			{
				menu.AppendMenu(MF_SEPARATOR);
			}
		}
	}
	if ( menu.GetMenuItemCount() > 0 )
	{
		menu.SetDefaultItem(m_aBtnIds[0], MF_BYCOMMAND);

		int iRet = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN |TPM_NONOTIFY |TPM_RETURNCMD, pos.x, pos.y, this);
		if ( iRet > 0 )
		{
			switch (iRet)
			{
			case IDC_BUTTON_SWITCH:
				OnBtnSwitch();
				break;
			case IDC_BUTTON_NEWNOTE:
				OnBtnNew();
				break;
			case IDC_BUTTON_MODIFY:
				OnBtnModify();
				break;
			case IDC_BUTTON_DEL:
				OnBtnDel();
				break;
			case IDC_BUTTON_CLEARNOTES:
				OnBtnClear();
				break;
			default:
				ASSERT( 0 );
			}
		}
	}
}

CString CDlgNotePad::GetCurDlgTitle()
{
	CString Str;
	if ( EST_UserNote == m_eShowType )
	{
		CMerch *pMerch = NULL;
		if ( AfxGetDocument() != NULL && AfxGetDocument()->m_pAbsCenterManager != NULL )
		{
			CAbsCenterManager *pAbsCenterManager = AfxGetDocument()->m_pAbsCenterManager;
			if ( pAbsCenterManager->GetMerchManager().FindMerch(m_MerchCur.m_StrMerchCode, m_MerchCur.m_iMarketId, pMerch) )
			{
				Str.Format(_T("投资日记 [%s-%s]"), pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_MerchCur.m_StrMerchCode.GetBuffer());
			}
			else
			{
				Str.Format(_T("投资日记 [%s]"), m_MerchCur.m_StrMerchCode.GetBuffer());
			}
		}
	}
	else if ( EST_InfoMine == m_eShowType )
	{
		CMerch *pMerch = NULL;
		if ( AfxGetDocument() != NULL && AfxGetDocument()->m_pAbsCenterManager != NULL )
		{
			CAbsCenterManager *pAbsCenterManager = AfxGetDocument()->m_pAbsCenterManager;
			if ( pAbsCenterManager->GetMerchManager().FindMerch(m_MerchCur.m_StrMerchCode, m_MerchCur.m_iMarketId, pMerch) )
			{
				// 分为实时信息地雷 & 历史信息地雷
				// 暂时仅从是不是今天的地雷来区分，都显示日期
				if ( m_bHistoryMine )
				{
					Str.Format(_T("信息地雷 [%s-%s]"), pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_MerchCur.m_StrMerchCode.GetBuffer());
				}
				else
				{
					Str.Format(_T("实时信息地雷 [%s-%s]"), pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_MerchCur.m_StrMerchCode.GetBuffer());
				}
			}
			else
			{
				Str.Format(_T("信息地雷 [%s]"), m_MerchCur.m_StrMerchCode.GetBuffer());
			}
		}
	}
	else
	{
		ASSERT( 0 );
	}

	return Str;
}

bool32 CDlgNotePad::IsInListMode() const
{
	return !IsWindowVisible() || m_List.IsWindowVisible();	// 如果对话框没显示，则是默认list下
}

void CDlgNotePad::OnCancel()
{
	OnClose();
}

BOOL CDlgNotePad::PreTranslateMessage( MSG* pMsg )
{
	if ( WM_LBUTTONDBLCLK == pMsg->message
		&& pMsg->hwnd == m_RichEdit.m_hWnd )
	{
		SwitchShow();
		return FALSE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

//////////////////////////////////////////////////////////////////////////
// 信息地雷
void CDlgNotePad::OnViewDataInfoMineList( CMerch *pMerch )
{
	CDlgNotePad *pDlg = s_pDlg.get();
	if ( NULL == pDlg->GetSafeHwnd() 
		|| NULL == pDlg->m_pMerchCur
		|| pMerch != pDlg->m_pMerchCur
		|| pDlg->m_eShowType != EST_InfoMine )
	{
		return;
	}

	// 有可能是列表更新，也有可能是内容更新
	::PostMessage(pDlg->GetSafeHwnd(), TEST_VIEWDATA_INFOMINELIST_RESP, 0, 0);
}

void CDlgNotePad::RequestViewDataInfoMineList( CMerch *pMerch, void * )
{
	CGGTongDoc *pDoc = AfxGetDocument();
	if ( NULL == pDoc )
	{
		return;
	}
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return;
	}

	CDlgNotePad *pDlg = s_pDlg.get();
	if ( NULL == pDlg->GetSafeHwnd() || NULL == pMerch )
	{
		return;
	}
	
	CMmiNewsReqLandMine reqMine;
	reqMine.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	reqMine.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
	CGmtTime TimeNow = pAbsCenterManager->GetServerTime() + CGmtTimeSpan(1,0,0,0);
	SaveDay(TimeNow);
	reqMine.m_StrTime1.Format(_T("%04d-%02d-%02d 00:00:00"), TimeNow.GetYear(), TimeNow.GetMonth(), TimeNow.GetDay());
	reqMine.m_iCount = 200;
	reqMine.m_iTimeType = 2;	// 结束往前200个
	reqMine.m_iIndexInfo = EITLandmine;
	
	pAbsCenterManager->RequestNews(&reqMine);
}

void CDlgNotePad::RequestViewDataInfoMineContent( CMerch *pMerch, const CLandMineTitle &title )
{
	CGGTongDoc *pDoc = AfxGetDocument();
	if ( NULL == pDoc )
	{
		return;
	}
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return;
	}
	
	CDlgNotePad *pDlg = s_pDlg.get();
	if ( NULL == pDlg->GetSafeHwnd() || NULL == pMerch )
	{
		return;
	}
	
	CMmiNewsReqLandMine reqMine;
	reqMine.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	reqMine.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
	reqMine.m_iCount = 10000;
	reqMine.m_iTimeType = 0;
	reqMine.m_iIndexInfo = title.m_iInfoIndex;
	
	pAbsCenterManager->RequestNews(&reqMine);
}

int CDlgNotePad::GetListSelInfoMine( OUT CLandMineTitle &title )
{
	ASSERT( m_eShowType == EST_InfoMine );
	const int iCount = m_List.GetCount();
	const int iSel = m_List.GetCurSel();
	ASSERT( iCount == m_aInfoMines.GetSize() );
	if ( iSel >= 0 && iSel < m_aInfoMines.GetSize() )
	{
		title = m_aInfoMines[iSel];
		return iSel;
	}
	return LB_ERR;
}

bool32 CDlgNotePad::UpdateInfoMine( const CLandMineTitle &title )
{
	// 列表更新
	ASSERT( m_eShowType == EST_InfoMine );
	const int iCount = m_List.GetCount();
	const int iSel = m_List.GetCurSel();
	bool32	bUpdate = false;
	bool32  bCurSel = false;
	ASSERT( iCount == m_aInfoMines.GetSize() );
	for ( int i=0; i < m_aInfoMines.GetSize() ; i++ )
	{
		CLandMineTitle &title2 = m_aInfoMines[i];
		if ( title2.m_StrUpdateTime == title.m_StrUpdateTime
			&& title2.m_iInfoIndex == title.m_iInfoIndex 
			&& title2.m_StrCrc32 == title.m_StrCrc32
			)
		{
			bUpdate = true;
			
			m_List.DeleteString(i);
			m_List.InsertString(i, MakeListItemDesc(title));
			
			title2 = title;	// 更新原值
			
			if ( iSel == i )
			{
				m_List.SetCurSel(iSel);
				bCurSel = true;
			}
			
			break;
		}
	}
	
	if ( bCurSel && bUpdate && !IsInListMode() )
	{
		// 更新html显示
		ShowHtml(title);
	}
	
	return bUpdate;
}

void CDlgNotePad::OnMsgTestViewDataListResp()
{
	ASSERT( NULL != m_pMerchCur );
	if ( NULL == m_pMerchCur || m_eShowType != EST_InfoMine )
	{
		return;
	}
	bool32 bSame = m_pMerchCur->m_mapLandMine.size() == m_mapInfoMines.size();
	mapLandMine::iterator it2 = m_pMerchCur->m_mapLandMine.begin();
	for ( mapLandMinePseudo::iterator it1 = m_mapInfoMines.begin(); it1 != m_mapInfoMines.end() && bSame ; ++it1 )
	{
		const CLandMineTitle &title1 = it1->second;
		const CLandMineTitle &title2 = it2->second;
		if ( title1.m_iInfoIndex != title2.m_iInfoIndex
			|| title1.m_StrUpdateTime != title2.m_StrUpdateTime
			|| title1.m_StrCrc32 != title2.m_StrCrc32
			|| title1.m_iConLen != title2.m_iConLen )
		{
			// 仅认为这几个 唯一标志了一个地雷列表
			bSame = false;
			break;
		}
		++it2;		// 大小相同，其能+的必定相同
	}
	// 不管是什么，全部刷新
	if ( bSame )
	{
		// 列表信息相同, 是否是内容变更，仅关注当前的内容
		CLandMineTitle titleCur;
		int32 iSel = GetListSelInfoMine(titleCur);

		// 列表信息相同，所以更新内容不更新显示应该无碍
		m_mapInfoMines = m_pMerchCur->m_mapLandMine;
		
		FilterInfoMind();

		if ( !IsInListMode()
			&& iSel >= 0 
			&& iSel < m_aInfoMines.GetSize()
			&& !(titleCur == m_aInfoMines[iSel]) )
		{
			// 需要更新内容
			titleCur = m_aInfoMines[iSel];
			UpdateInfoMine(titleCur);
		}
	}
	else
	{
		// 重新刷新
		RefreshList();
	}
}

void CDlgNotePad::OnMsgTestViewDataContentResp()
{
	
}

void CDlgNotePad::OnDataRespNewsData( int iMmiReqId, IN const CMmiNewsBase *pMmiNewResp, CMerch *pMerch )
{
	if ( NULL != pMmiNewResp 
		&& pMmiNewResp->m_eNewsType == ENTRespLandMine 
		&& pMerch == m_pMerchCur )
	{
		OnViewDataInfoMineList(pMerch);
	}
}

void CDlgNotePad::FilterInfoMind()
{
	// 根据缓冲的信息地雷内容过滤
	m_aInfoMines.SetSize(0, m_mapInfoMines.size());

	if ( m_bHistoryMine )
	{
		for ( mapLandMinePseudo::reverse_iterator it=m_mapInfoMines.rbegin();
			it != m_mapInfoMines.rend();
			++it)
		{
			const CLandMineTitle &title = it->second;
			m_aInfoMines.Add(title);
		}
	}
	else
	{
		CGmtTime TimeDay(m_TimeInit);
		CGmtTime TimeDayLocalStart, TimeDayLocalEnd;
		CString StrToday, StrTodayStart, StrTodayEnd;
		GetLocalDayGmtRange(TimeDay, TimeDayLocalStart, TimeDayLocalEnd);
		// 实时信息地雷不加载非当天的地雷, 当天为本地时间的当天
		ASSERT( m_TimeInit.GetTime() > 0 );
		StrToday.Format(_T("%04d-%02d-%02d"), TimeDay.GetYear(), TimeDay.GetMonth(), TimeDay.GetDay());
		StrTodayStart.Format(_T("%04d-%02d-%02d %02d:%02d:%02d")
			, TimeDayLocalStart.GetYear(), TimeDayLocalStart.GetMonth(), TimeDayLocalStart.GetDay()
			, TimeDayLocalStart.GetHour(), TimeDayLocalStart.GetMinute(), TimeDayLocalStart.GetSecond());
		StrTodayEnd.Format(_T("%04d-%02d-%02d %02d:%02d:%02d")
			, TimeDayLocalEnd.GetYear(), TimeDayLocalEnd.GetMonth(), TimeDayLocalEnd.GetDay()
			, TimeDayLocalEnd.GetHour(), TimeDayLocalEnd.GetMinute(), TimeDayLocalEnd.GetSecond());

		for ( mapLandMinePseudo::reverse_iterator it=m_mapInfoMines.rbegin();
			it != m_mapInfoMines.rend();
			++it)
		{
			const CLandMineTitle &title = it->second;
			
			if ( title.m_StrUpdateTime.IsEmpty()
				/*|| _tcsnccmp(StrToday, title.m_StrUpdateTime, StrToday.GetLength()) != 0*/
				|| title.m_StrUpdateTime < StrTodayStart
				|| title.m_StrUpdateTime > StrTodayEnd )
			{
				continue;	// 忽略非实时当天数据
			}
			
			m_aInfoMines.Add(title);
		}
	}
}

void CDlgNotePad::OnLandTypeChanged()
{
	if ( EST_InfoMine != m_eShowType )
	{
		return;
	}

	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( NULL != pMainFrame )
	{
		CIoViewBase *pIo = pMainFrame->FindActiveIoView();
		if ( NULL != pIo )
		{
			CMerch *pMerch = pIo->GetMerchXml();
			if ( NULL == pMerch )
			{
				return;
			}
			
			bool32 bRealTimeLandIoView = pIo->IsKindOf(RUNTIME_CLASS(CIoViewTrend));
			if ( m_bHistoryMine ? bRealTimeLandIoView : !bRealTimeLandIoView )
			{
				// 历史与实时变更了, 并且有商品
				ASSERT( pMerch != NULL );
				InitShowType(EST_InfoMine, NULL, pIo);
			}
		}
	}
}

void CDlgNotePad::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	int iToolHit = OnToolHitTest(point, NULL);
	if ( iToolHit == -1  || iToolHit == IDC_STATIC_TITLE )
	{
		DoFloating();
		return;
	}

	CDialogEx::OnLButtonDblClk(nFlags, point);
}

void CDlgNotePad::DoFloating()
{
	if ( GetStyle()&WS_CHILD )
	{
		CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
		pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, pMainFrame);
		if ( NULL!=pMainFrame )
		{
			pMainFrame->DoToggleMyDock();
		}
	}
}

void CDlgNotePad::OnBtnFloat()
{
	DoFloating();
}

void CDlgNotePad::SetMyTitle( LPCTSTR lpszTitle )
{
	SetWindowText(lpszTitle);
}

void CDlgNotePad::OnBtnHide()
{
	if ( GetStyle()&WS_CHILD )
	{
		CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
		pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, pMainFrame);
		if ( NULL!=pMainFrame )
		{
			pMainFrame->ShowDockSubWnd(NULL);
		}
	}
}

LRESULT CDlgNotePad::OnNcHitTest(CPoint point)
{
	int ret = __super::OnNcHitTest(point);

	if( HTCAPTION == ret)
		return HTCLIENT;

	return ret;
}
