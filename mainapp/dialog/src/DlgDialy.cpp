#include "StdAfx.h"

#include "DlgDialy.h"
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

const CString KStrDefaultContent = _T("[������]");



/////////////////////////////////////////////////////////////////////////////
// CDlgDialy dialog

CDlgDialy::CDlgDialy(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgDialy::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDialy)
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
	m_eNewsType = ENTReqLandMine;
}


void CDlgDialy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDialy)
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_STATIC_TITLE, m_StaticTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDialy, CDialogEx)
//{{AFX_MSG_MAP(CDlgDialy)
ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList)
ON_WM_SIZE()
ON_WM_CONTEXTMENU()
ON_WM_GETMINMAXINFO()
ON_WM_CLOSE()
ON_WM_DESTROY()
ON_WM_LBUTTONDBLCLK()
ON_MESSAGE_VOID(UM_NOTE_MERCHHASCHANGED, OnMerchChanged)
ON_MESSAGE_VOID(UM_NOTE_LANDTYPEHASCHANGED, OnLandTypeChanged)
ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
ON_BN_CLICKED(IDC_BUTTON_NEWNOTE, OnBtnNew)
ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnBtnModify)
ON_BN_CLICKED(IDC_BUTTON_DEL, OnBtnDel)
ON_BN_CLICKED(IDC_BUTTON_CLEARNOTES, OnBtnClear)
ON_BN_CLICKED(IDC_BUTTON_SWITCH, OnBtnSwitch)
 ON_MESSAGE_VOID(TEST_VIEWDATA_INFOMINELIST_RESP, OnMsgTestViewDataListResp)
 ON_MESSAGE_VOID(TEST_VIEWDATA_INFOMINECONTENT_RESP, OnMsgTestViewDataContentResp)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgDialy::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString StrTitle;
	GetWindowText(StrTitle);

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

	m_MerchCur.m_StrMerchCode.Empty();	// �����Ʒ��Ϣ

	m_RichEdit.Create(WS_CHILD |ES_MULTILINE |ES_READONLY |WS_TABSTOP  |ES_AUTOVSCROLL |WS_VSCROLL, CRect(0,0,0,0), this, IDC_STATIC);
	m_RichEdit.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	m_RichEdit.SetFont(&m_fontEdit);

	m_StaticTitle.ModifyStyle(0, SS_WORDELLIPSIS);

	m_List.SetFont(&m_fontEdit);

// 	if (!m_WebBrowser2.Create(L"", WS_CHILD, CRect(0, 0, 0, 0), this, IDC_STATIC))
// 	{
// 		AfxMessageBox(_T("�޷����������!"));
// 		OnClose();
// 	}
// 	else
// 	{
// 		// �ȴ��ⲿ���ó�ʼ������
// 	}

	RecalcLayout();

	CGGTongDoc *pDoc = AfxGetDocument();
	CAbsCenterManager *pAbsCenterManager = NULL;
	if ( NULL != pDoc && NULL != (pAbsCenterManager = pDoc->m_pAbsCenterManager) )
	{
		pAbsCenterManager->AddViewNewsListner(this);
	}
	else
	{
		ASSERT( 0 );
	}
	

	return TRUE;
}

void CDlgDialy::OnSize( UINT nType, int cx, int cy )
{
	CDialogEx::OnSize(nType, cx, cy);
	RecalcLayout();
}


void CDlgDialy::RecalcLayout()
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
				m_RectSeparator.bottom = m_RectSeparator.top;
			}
			else
			{
				// Ӧ���еģ�ֻ�ܸ���Ĭ��ֵ��
				m_RectSeparator.top = m_RectSeparator.bottom = 30;
			}
		}
		
		if ( m_RectSeparator.top == 0 )
		{
			return;	// ��ʱ�����ܷ���
		}
		
		CRect rcClient;
		GetClientRect(rcClient);
		
		CRect rcList(rcClient);
		rcList.top = m_RectSeparator.top;
		m_List.MoveWindow(rcList);
		//m_WebBrowser2.MoveWindow(rcList);
		m_RichEdit.MoveWindow(rcList);
		
		// ������ť��λ��
		ASSERT( m_aBtnIds.GetSize() > 0 );
		if ( m_aBtnIds.GetSize() > 0 )
		{
			CWnd *pWnd = GetDlgItem(m_aBtnIds[0]);
			ASSERT( NULL !=pWnd );
			if ( NULL != pWnd )
			{
				CRect rcBtn;
				pWnd->GetWindowRect(rcBtn);
				if ( rcBtn.Width() > 0 )
				{
					// ���ڿ����ƶ���
					int iHeight = rcBtn.Height();
					int iWidth = rcBtn.Width();
					rcBtn.top = rcClient.top + (m_RectSeparator.top-rcClient.top-rcBtn.Height())/2;
					rcBtn.bottom = rcBtn.top + iHeight;
					rcBtn.right = rcClient.right - rcBtn.Width()/2;
					rcBtn.left = rcBtn.right - iWidth;
					for ( int i=m_aBtnIds.GetUpperBound(); i >= 0 ; i-- )
					{
						pWnd = GetDlgItem(m_aBtnIds[i]);
						if ( NULL != pWnd )
						{
							pWnd->MoveWindow(rcBtn);
							rcBtn -= CPoint(iWidth+4, 0);
						}
					}
					
					// �ƶ�static title
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


void CDlgDialy::OnClose()
{
	m_aUserNotes.clear();
	DestroyWindow();	// ���𴰿�
}

void CDlgDialy::OnDestroy()
{
	// ���һЩ����
	CGGTongDoc *pDoc = AfxGetDocument();
	CAbsCenterManager *pAbsCenterManager = NULL;
	if ( NULL != pDoc && NULL != (pAbsCenterManager = pDoc->m_pAbsCenterManager) )
	{
		pAbsCenterManager->DelViewNewsListner(this);
	}
	else
	{
		ASSERT( 0 );
	}

	CDialogEx::OnDestroy();
}

void CDlgDialy::OnGetMinMaxInfo( MINMAXINFO* lpMMI )
{
	lpMMI->ptMinTrackSize.x = m_sizeMin.cx;
	lpMMI->ptMinTrackSize.y = m_sizeMin.cy;
}

void CDlgDialy::OnDblclkList()
{
	// �л�
	SwitchShow();
}

void CDlgDialy::SwitchShow()
{
	if ( IsInListMode() )
	{
		// �ҵ���ǰѡ�е���Ŀ������html��ʾ
		if ( EST_UserNote == m_eShowType )
		{
			T_UserNoteInfo note;
			int iSel = GetListSelUserNote(note);
			if ( LB_ERR != iSel )
			{
				// ��ѡ���
				ShowHtml(note, true);	// ��ʾ�������, ò�ƿ��Բ���ÿ�ζ���ȡ
			}
			else
			{
				ASSERT( 0 );		// û��ѡ����л�ʧ��
				return;
			}
		}
		else
		{
			CLandMineTitle title;
			if ( GetListSelInfoMine(title) != LB_ERR )
			{
				ShowHtml(title);		// û�еĻ������Զ�����, ��Ϣ���ײ�����ָ��ĵ����
			}
			else
			{
				ASSERT( 0 );
				return;		// ��ѡ���л�ʧ��
			}
		}
	}
	else
	{
		// �л���list��ʾģʽ
		ShowList();
	}
}

void CDlgDialy::ShowList()
{
	if ( EST_UserNote == m_eShowType )
	{	
		//m_WebBrowser2.ShowWindow(SW_HIDE);
		m_RichEdit.ShowWindow(SW_HIDE);
		m_StaticTitle.ShowWindow(SW_HIDE);
		m_List.ShowWindow(SW_SHOW);
		if ( &m_RichEdit == GetFocus() )
		{
			m_List.SetFocus();
		}
	}
	else
	{
		m_RichEdit.ShowWindow(SW_HIDE);
		m_StaticTitle.ShowWindow(SW_HIDE);
		m_List.ShowWindow(SW_SHOW);
		if ( &m_RichEdit == GetFocus() )
		{
			m_List.SetFocus();
		}
	}
}

LRESULT CDlgDialy::OnIdleUpdateCmdUI( WPARAM w, LPARAM l )
{
	// �������ʱ�䣬�����°�ť��״̬���鿴��Ʒ�Ƿ����
	WatchMerchIsChanged();		

	UpdateBtnStatus();

	return 0;
}


void CDlgDialy::RefreshList()
{
	if ( EST_UserNote == m_eShowType )
	{
		// ��ȡ����Ʒ������Ͷ���ռǣ�����б�	
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
					m_List.SetItemData(iLine, it->m_iUniqueId);	// ����ÿ�α�����
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
					// û������
				}
			}
		}
		m_List.Invalidate();

		// ��ǰģʽ�������ʾ���ݵģ���Ҫ�鿴�����ݴ��ڷ񣬲����ڣ��򷵻��б�
		if ( !IsInListMode() )
		{
			T_UserNoteInfo note;
			if ( LB_ERR == GetListSelUserNote(note) )
			{
				// ������ѡ��
				ShowList();
			}
			else
			{
				ShowHtml(note, true);	// ������ʾ����
			}
		}

		UpdateBtnStatus();
	}
	else
	{
		// �ӵ�ǰ��Ʒ����Ϣ�����б��л�ȡ��Ϣ�����б���ʾ
		int32 iSelOld = m_List.GetCurSel();
		CString StrSel;
		if ( iSelOld != LB_ERR )
		{
			// �ҵ����ڵ��Ǹ�ѡ�����
			m_List.GetText(iSelOld, StrSel);
		}

		m_mapInfoMines.clear();
		if ( NULL != m_pMerchCur )
		{
			m_mapInfoMines = m_pMerchCur->m_mapLandMine;		// ��ֵ��ǰ��Ʒ��
		}

		CAutoLockWindowUpdate lockDraw(&m_List);
		{
			m_aInfoMines.SetSize(0, m_mapInfoMines.size());	// ���������
			m_List.ResetContent();
			
			// ��ȡ��������Ϣ
			FilterInfoMind();

			for ( int32 i=0; i < m_aInfoMines.GetSize() ; ++i )
			{
				const CLandMineTitle &title = m_aInfoMines[i];
				
				CString Str = MakeListItemDesc(title);
				m_List.AddString(Str);
			}
			
			if ( iSelOld != LB_ERR )
			{
				// ���Ƿ�����ͬ���ִ�, �о�ѡ��
				int iSel = m_List.SelectString(-1, StrSel);
				if ( iSel < 0 )
				{
					m_List.SetCurSel(0);
				}
				else
				{
					// �Ƿ���Ҫ������
				}
			}
			else
			{
				m_List.SetCurSel(0);
			}
			
		}
		m_List.Invalidate();

		// ��ǰ��ʾģʽ
		if ( !IsInListMode() )
		{
			// �������
			CLandMineTitle title;
			if ( GetListSelInfoMine(title) == LB_ERR )
			{
				SwitchShow();	// �л���ʾΪ�б�
			}
			else
			{
				ShowHtml(title);	// ������ʾ����
			}
		}
	}
}

void CDlgDialy::OnMerchChanged()
{
	// ��Ʒ�����ˣ��л����е���ʾ
	// ��ǰ��Ʒ�Ƿ�����Ч��Ʒ
	if ( m_MerchCur.m_StrMerchCode.IsEmpty() )
	{
		return;	// ������
	}


	ShowList();	// ��Ʒ�л���ʾ�б�
	RefreshList();

	SetWindowText(GetCurDlgTitle());

	if ( m_eShowType == EST_InfoMine )
	{
		// �������Ʒ���б�����
		if ( NULL != m_pMerchCur )
		{
			RequestViewDataInfoMineList(m_pMerchCur, NULL);
		}
	}
}

void CDlgDialy::UpdateBtnStatus()
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
		// �л� [���� �޸� ɾ��] �ղ�[Ŀǰ�޴˹���]
		// ����Ҫ�任����
		::EnableWindow(GetDlgItem(IDC_BUTTON_SWITCH)->GetSafeHwnd(), bSel);
		::EnableWindow(GetDlgItem(IDC_BUTTON_NEWNOTE)->GetSafeHwnd(), FALSE);
		::EnableWindow(GetDlgItem(IDC_BUTTON_MODIFY)->GetSafeHwnd(), FALSE);
		::EnableWindow(GetDlgItem(IDC_BUTTON_DEL)->GetSafeHwnd(), FALSE);
		
		::EnableWindow(GetDlgItem(IDC_BUTTON_CLEARNOTES)->GetSafeHwnd(), FALSE);
	}
}

CString CDlgDialy::MakeListItemDesc( const T_UserNoteInfo &note )
{
	static CStringArray aDayInWeek;
	if ( aDayInWeek.GetSize() <= 0 )
	{
		aDayInWeek.Add(_T("������"));
		aDayInWeek.Add(_T("����һ"));
		aDayInWeek.Add(_T("���ڶ�"));
		aDayInWeek.Add(_T("������"));
		aDayInWeek.Add(_T("������"));
		aDayInWeek.Add(_T("������"));
		aDayInWeek.Add(_T("������"));
	}
	tm ttm;
	note.m_Time.GetLocalTm(&ttm);
	
	CString Str;
	Str.Format(
		_T("%04d.%02d.%02d %02d:%02d:%02d [%s %s]  %s"),
		ttm.tm_year + 1900, ttm.tm_mon+1, ttm.tm_mday, ttm.tm_hour, ttm.tm_min, ttm.tm_sec,
		aDayInWeek[ttm.tm_wday], GetUserNoteWeatherString((E_UserNoteWeather)note.m_iWeather),
		note.m_StrTitle
		);

	return Str;
}

CString CDlgDialy::MakeListItemDesc( const CLandMineTitle &mineTitle )
{
	// 2010.01.01[Ϊ����] xxxxxx
	// ��ǰ��ʾ 2010-01-01 xxxxxx(xxxByte)
	CString Str;
	CString StrTime = mineTitle.m_StrUpdateTime;
	int32 iSep = -1;
	// ��ʷ��Ϣ��ʵʱ���� ��ʷ��ʾ���ڣ�ʵʱ��ʾʱ��
	// ��ʱ��Ϊgmtʱ��
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
	Str.Format(
		_T("%s  %s (%dB)"),
		StrTime,
		mineTitle.m_StrText,
		mineTitle.m_iConLen
		);
	
	return Str;
}

void CDlgDialy::OnNewNote()
{
	// �½�һ���ռ�
	// Ӧ���ǽ���edit�Ի����û��༭���룬����ӣ�����ֻҪ���ؽ���Ϳ�����
	T_UserNoteInfo note;
	
	CUserNoteInfoManager::InitNewUserNote(note);
	note.m_MerchKey = m_MerchCur;
	if ( !CDlgNewNote::ModifyNote(note) )
	{
		return;	// ȡ��
	}

	bool32 bAdded = FALSE;
	if ( CUserNoteInfoManager::Instance().NewUserNote(note) )
	{
		CUserNoteInfoManager::Instance().GetUserNotesHeaderArray(m_MerchCur, m_aUserNotes);	// ˢ���б���Ϣ

		int32 iLine = 0;
		for ( CUserNoteInfoManager::UserNotesArray::reverse_iterator it = m_aUserNotes.rbegin(); it != m_aUserNotes.rend() ; ++it )
		{
			int32 iId = (int32)m_List.GetItemData(iLine);
			if ( iId != it->m_iUniqueId )	// Ӧ����ͬ����ȵ�
			{
				// ���ȣ���������ӵ�λ����
				iLine = m_List.InsertString(iLine, MakeListItemDesc(*it));
				if ( LB_ERR != iLine )
				{
					m_List.SetItemData(iLine, (DWORD)it->m_iUniqueId);
					bAdded = TRUE;
				}
				break;
			}
			iLine++;	// �����һ��
		}
		
		if ( !bAdded )
		{
			RefreshList();	// ֻ��ˢ����
			bAdded = TRUE;
		}
	}

	if ( bAdded )
	{
		MessageBox(_T("���Ͷ���ռǳɹ�"));
	}
	else
	{
		MessageBox(_T("���Ͷ���ռ�ʧ�ܣ�"));
	}
}

void CDlgDialy::OnModifyNote()
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
		ASSERT( 0 );
		return;
	}

	// ���Լ���
	if ( note.m_StrContent.IsEmpty() )
	{
		// ���Լ���
		CUserNoteInfoManager::Instance().ReadUserNoteContent(note);
	}

	ShowHtml(note);	// ����ʾ

	if ( !CDlgNewNote::ModifyNote(note) )
	{
		return;	// û���޸�
	}

	if ( CUserNoteInfoManager::Instance().ModifyUserNote(note) )
	{
		CUserNoteInfoManager::Instance().GetUserNotesHeaderArray(m_MerchCur, m_aUserNotes);
		if ( note.m_Time != noteOld.m_Time )
		{
			// ��Ҫ���²������
			RefreshList();
			
			if ( !IsInListMode() )
			{
				// ������ʾhtml����
				ShowHtml(note);
			}
		}
		else
		{
			// ����������
			UpdateUserNote(note);
		}

		MessageBox(_T("�޸�Ͷ���ռǳɹ�"));
	}
	else
	{
		ASSERT( 0 );
	}
}

void CDlgDialy::OnDelNote()
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
		ASSERT( 0 );
		return;
	}
	
	m_List.DeleteString(iSel);
	CUserNoteInfoManager::Instance().DelUserNote(note);

	CUserNoteInfoManager::Instance().GetUserNotesHeaderArray(m_MerchCur, m_aUserNotes);
	
	RefreshList();

	ShowList();		// ��ɾ���ˣ���ʾlist
}

void CDlgDialy::OnClearNote()
{
	m_List.ResetContent();
	m_aUserNotes.clear();

	CUserNoteInfoManager::Instance().ClearUserAllNote(m_MerchCur);

	ShowList();	// ��ʾlist
}

void CDlgDialy::WatchMerchIsChanged(bool32 bNotify /*= true*/)
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
				// ��ʷ��ʵʱ�����, ��������Ʒ
				ASSERT( pMerch != NULL );
				if ( bNotify )
				{
					PostMessage(UM_NOTE_LANDTYPEHASCHANGED, 0, 0);
				}
			}
		}
	}
}

int CDlgDialy::GetListSelUserNote( OUT T_UserNoteInfo &note )
{
	int iSel = m_List.GetCurSel();
	if ( LB_ERR == iSel )
	{
		return iSel;	//  LB_ERR δѡ��
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
		ASSERT( 0 );
		return LB_ERR;
	}

	return iSel;	// ����ѡ����б�����
}

void CDlgDialy::OnBtnNew()
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

void CDlgDialy::OnBtnModify()
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

void CDlgDialy::OnBtnDel()
{
	if ( EST_UserNote == m_eShowType )
	{
		if ( AfxMessageBox(_T("��ȷ��ɾ����Ͷ���ռ���"), MB_ICONQUESTION |MB_YESNO) != IDYES )
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

void CDlgDialy::OnBtnClear()
{
	if ( EST_UserNote == m_eShowType )
	{
		if ( AfxMessageBox(_T("��ȷ���������Ʒ������Ͷ���ռ���"), MB_ICONQUESTION |MB_YESNO) != IDYES )
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

void CDlgDialy::OnBtnSwitch()
{
	SwitchShow();
}

void CDlgDialy::OnOK()
{
	// nothing
}

bool32 CDlgDialy::UpdateUserNote(const T_UserNoteInfo &note)
{
	// �б����
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
		// ����html��ʾ
		ShowHtml(const_cast<T_UserNoteInfo &>(note));
	}

	return bUpdate;
}

void CDlgDialy::ShowHtml(T_UserNoteInfo &note, bool32 bTryReadContent/* = false*/)
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
		// ���Լ���
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

void CDlgDialy::ShowHtml(const CLandMineTitle &title, bool32 bTryReq /*= false*/ )
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
		// �������� TODO
		RequestViewDataInfoMineContent(m_pMerchCur, title);
	}

	m_StaticTitle.SetWindowText(MakeListItemDesc(title));
	m_RichEdit.SetWindowText(title.m_stContent.m_StrText);
}

void CDlgDialy::InitShowType( E_ShowType eShowType, CGmtTime *pTimeInit /*= NULL*/, CIoViewBase *pEventIoView/*=NULL*/ )
{
	if ( EST_UserNote == eShowType )
	{
		m_eShowType = eShowType;
		ShowList();
		WatchMerchIsChanged(false);
		OnMerchChanged();

		// ���Ҹ����ڵ����ѡ��
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
			// ��ʱͼ�µ���ʾΪʵʱ��Ϣ����
			m_bHistoryMine = true/*!pEventIoView->IsKindOf(RUNTIME_CLASS(CIoViewTrend))*/;
			if ( !m_bHistoryMine )
			{
				if ( NULL != pTimeInit )
				{
					m_TimeInit = *pTimeInit;
				}
				else
				{
					// ��ʱͼ�ĵ�ǰʱ��
					CIoViewTrend *pTrend = DYNAMIC_DOWNCAST(CIoViewTrend, pEventIoView);
					ASSERT( NULL != pTrend );
					CMarketIOCTimeInfo IOCTime;
					if ( pTrend->GetCurrentIOCTime(IOCTime) )
					{
						m_TimeInit = IOCTime.m_TimeOpen.m_Time;	// ȡ����ʱ��
					}
					else
					{
						// ���������ʱͼû�г�ʼ��
						// �޷���ȡʵʱ��Ϣ����ô�͵�����ʷ�Ĵ�����
						m_bHistoryMine = true;
					}
				}
				if ( !m_bHistoryMine )
				{
					// ��ǰʱ��
				}
			}
		}
		ShowList();
		WatchMerchIsChanged(false);	// ��ȡ��ǰ����Ʒ
		OnMerchChanged();		// �л���Ʒ

		// ���Ҹ����ڵ����ѡ��
		// �Ժ��л�����Ʒ�Ͳ�����
		if ( NULL != pTimeInit )
		{
			CTime TimeDay(pTimeInit->GetTime());

			// �����GMTʱ�䲻��00�ְ�~~
			// Ŀǰ����ʾ��ʷ��Ϣ����
			
			CString StrTime;
			StrTime.Format(_T("%02d:%02d:%02d")
				, TimeDay.GetHour(), TimeDay.GetMinute(), TimeDay.GetSecond() );
			// �ӵ�ǰ��Ʒ����Ϣ�����б����ҵ�һ����select
			// ������ʵʱ��Ȼ����ʷ������, ������ʵû��ʵʱ��ʾ
			if ( m_List.SelectString(-1, StrTime) < 0 )
			{
				StrTime.Format(_T("%04d-%02d-%02d"), TimeDay.GetYear(), TimeDay.GetMonth(), TimeDay.GetDay());
				m_List.SelectString(-1, StrTime);
			}
		}
		
		UpdateBtnStatus();
	}
}

auto_ptr<CDlgDialy> CDlgDialy::s_pDlg;		// ��̬�Ķ���
void CDlgDialy::ShowNotePad( E_ShowType eShowType, CGmtTime *pTimeInit /*= NULL*/, CIoViewBase *pEventIoView/*=NULL*/ )
{
	if ( s_pDlg.get() == NULL )
	{
		auto_ptr<CDlgDialy> ptAuto(new CDlgDialy);
		s_pDlg = ptAuto;
	}
	if ( s_pDlg->m_hWnd == NULL )
	{
		s_pDlg->Create(IDD, AfxGetMainWnd());
		s_pDlg->CenterWindow();
	}

	s_pDlg->InitShowType(eShowType, pTimeInit, pEventIoView);
	s_pDlg->ShowWindow(SW_SHOW);
}

void CDlgDialy::OnContextMenu( CWnd* pWnd, CPoint pos )
{
	CRect rcWin;
	m_List.GetWindowRect(rcWin);
	if ( !rcWin.PtInRect(pos) && pos.x >= 0 )
	{
		return;	// ��������
	}
	if ( !rcWin.PtInRect(pos) )
	{
		pos = rcWin.TopLeft();
	}

	CMenu menu;
	// �˵�: �л� ׫д �޸� ɾ�� ���
	// �������ѡ����ʵ�
	menu.CreatePopupMenu();
	for ( int32 i=0; i < m_aBtnIds.GetSize() ; i++ )
	{
		CWnd *pWnd = GetDlgItem(m_aBtnIds[i]);
		if ( NULL != pWnd )
		{
			CString StrBtn;
			GetDlgItemText(m_aBtnIds[i], StrBtn);
			menu.AppendMenu(MF_STRING, m_aBtnIds[i], StrBtn);
			if ( !pWnd->IsWindowEnabled() )
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

CString CDlgDialy::GetCurDlgTitle()
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
				Str.Format(_T("Ͷ���ռ� [%s-%s]"), pMerch->m_MerchInfo.m_StrMerchCnName, m_MerchCur.m_StrMerchCode);
			}
			else
			{
				Str.Format(_T("Ͷ���ռ� [%s]"), m_MerchCur.m_StrMerchCode);
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
				// ��Ϊʵʱ��Ϣ���� & ��ʷ��Ϣ����
				// ��ʱ�����ǲ��ǽ���ĵ��������֣�����ʾ����
				if ( m_bHistoryMine )
				{
					Str.Format(_T("��Ѷ [%s-%s]"), pMerch->m_MerchInfo.m_StrMerchCnName, m_MerchCur.m_StrMerchCode);
				}
				else
				{
					Str.Format(_T("��Ѷ [%s-%s]"), pMerch->m_MerchInfo.m_StrMerchCnName, m_MerchCur.m_StrMerchCode);
				}
			}
			else
			{
				Str.Format(_T("��Ϣ���� [%s]"), m_MerchCur.m_StrMerchCode);
			}
		}
	}
	else
	{
		ASSERT( 0 );
	}

	return Str;
}

bool32 CDlgDialy::IsInListMode() const
{
	return !IsWindowVisible() || m_List.IsWindowVisible();	// ����Ի���û��ʾ������Ĭ��list��
}

void CDlgDialy::OnCancel()
{
	OnClose();
}

BOOL CDlgDialy::PreTranslateMessage( MSG* pMsg )
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
// ��Ϣ����
void CDlgDialy::OnViewDataInfoMineList( CMerch *pMerch )
{
	CDlgDialy *pDlg = s_pDlg.get();
	if ( NULL == pDlg->GetSafeHwnd() 
		|| NULL == pDlg->m_pMerchCur
		|| pMerch != pDlg->m_pMerchCur
		|| pDlg->m_eShowType != EST_InfoMine )
	{
		return;
	}

	// �п������б���£�Ҳ�п��������ݸ���
	::PostMessage(pDlg->GetSafeHwnd(), TEST_VIEWDATA_INFOMINELIST_RESP, 0, 0);
}

void CDlgDialy::RequestViewDataInfoMineList( CMerch *pMerch, void * )
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
	
	CDlgDialy *pDlg = s_pDlg.get();
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
	reqMine.m_iTimeType = 2;	// ������ǰ200��
	reqMine.m_iIndexInfo = EITLandmine;
	
	pAbsCenterManager->RequestNews(&reqMine);
}

void CDlgDialy::RequestViewDataInfoMineContent( CMerch *pMerch, const  CLandMineTitle &title )
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

	CDlgDialy *pDlg = s_pDlg.get();
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

int CDlgDialy::GetListSelInfoMine( OUT CLandMineTitle &title )
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

bool32 CDlgDialy::UpdateInfoMine(  const CLandMineTitle &title )
{
	// �б����
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
			
			title2 = title;	// ����ԭֵ
			
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
		// ����html��ʾ
		ShowHtml(title);
	}
	
	return bUpdate;
}

void CDlgDialy::OnMsgTestViewDataListResp()
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
			// ����Ϊ�⼸�� Ψһ��־��һ�������б�
			bSame = false;
			break;
		}
		++it2;		// ��С��ͬ������+�ıض���ͬ
	}
	// ������ʲô��ȫ��ˢ��
	if ( bSame )
	{
		// �б���Ϣ��ͬ, �Ƿ������ݱ��������ע��ǰ������
		CLandMineTitle titleCur;
		int32 iSel = GetListSelInfoMine(titleCur);

		// �б���Ϣ��ͬ�����Ը������ݲ�������ʾӦ���ް�
		m_mapInfoMines = m_pMerchCur->m_mapLandMine;
		
		FilterInfoMind();

		if ( !IsInListMode()
			&& iSel >= 0 
			&& iSel < m_aInfoMines.GetSize()
			&& !(titleCur == m_aInfoMines[iSel]) )
		{
			// ��Ҫ��������
			titleCur = m_aInfoMines[iSel];
			UpdateInfoMine(titleCur);
		}
	}
	else
	{
		// ����ˢ��
		RefreshList();
	}
}

void CDlgDialy::OnMsgTestViewDataContentResp()
{
	
}

void CDlgDialy::OnDataRespNewsData( int iMmiReqId, IN const CMmiNewsBase *pMmiNewResp, CMerch *pMerch )
{
	if ( NULL != pMmiNewResp 
		&& pMmiNewResp->m_eNewsType == ENTRespLandMine 
		&& pMerch == m_pMerchCur )
	{
		OnViewDataInfoMineList(pMerch);
	}
}

void CDlgDialy::FilterInfoMind()
{
	// ���ݻ������Ϣ�������ݹ���
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
		// ʵʱ��Ϣ���ײ����طǵ���ĵ���, ����Ϊ����ʱ��ĵ���
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
				continue;	// ���Է�ʵʱ��������
			}
			
			m_aInfoMines.Add(title);
		}
	}
}

void CDlgDialy::OnLandTypeChanged()
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
				// ��ʷ��ʵʱ�����, ��������Ʒ
				ASSERT( pMerch != NULL );
				InitShowType(EST_InfoMine, NULL, pIo);
			}
		}
	}
}

