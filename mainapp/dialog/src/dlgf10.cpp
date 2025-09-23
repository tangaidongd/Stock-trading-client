// dlgf10.cpp : implementation file
//

#include "stdafx.h"
#include "dlgf10.h"
#include "DlgBelongBlock.h"
#include "IoViewBase.h"
#include "hotkey.h"
#include "MemdcEx.h"
#include "NewMenu.h"
#include "sharestructnews.h"
#include "coding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgF10 dialog

CDlgF10::CDlgF10(CIoViewBase* pIoViewActive, CMerch* pMerch, CWnd* pParent): CDialogEx(CDlgF10::IDD, pParent)
{
	m_eNewsType = ENTReqF10;
	m_pIoViewActive	= pIoViewActive;
	m_pMerch		= pMerch;

	m_eReqState		= ERSReqFinished;
	m_iInfoIndexNow	= -1;
	
	m_aNewTitles.RemoveAll();
	m_aMapTableNewsNode.RemoveAll();
	m_aMapTableNewsNode.RemoveAll();

	m_bMaxed		= false;
	m_bServerDisConnect = false;

	m_bNeedMore		= false;	
	m_RectMore		= CRect(-1, -1, -1, -1);
	m_RectMerchPre	= CRect(-1, -1, -1, -1);
	m_RectMerchAft	= CRect(-1, -1, -1, -1);

	CGGTongDoc* pDoc = AfxGetDocument();
	m_pAbsCenterManager = pDoc->m_pAbsCenterManager;;
}

// m_pMerch,m_pAbsCenterManager,m_pIoViewActive����Ҫ�ͷ�
//lint --e{1540}
CDlgF10::~CDlgF10()
{

}

void CDlgF10::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX); 
	//{{AFX_DATA_MAP(CDlgF10)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_RICHEDIT, m_RichEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgF10, CDialogEx)
	//{{AFX_MSG_MAP(CDlgF10)
		// NOTE: the ClassWizard will add message map macros here
	ON_WM_PAINT()
	ON_WM_ACTIVATE()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(UM_HOTKEY, OnMsgHotKey)
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_GETMINMAXINFO()
	ON_COMMAND_RANGE(ID_MENU_F10_BEGIN, ID_MENU_F10_END, OnF10Menu)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static const int32 KiMaxCount = 100000;

/////////////////////////////////////////////////////////////////////////////
// CDlgF10 message handlers
BOOL CDlgF10::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	if ( NULL != m_pIoViewActive && NULL != m_pMerch )
	{		
		OnMerchChange(m_pMerch, true);
	}
	else
	{
		//ASSERT(0);
	}

	//
	LOGFONT LogFont;
	memset(&LogFont, 0, sizeof(LogFont));
	
	LogFont.lfHeight	= -14;
	LogFont.lfWeight	= 400;
	LogFont.lfCharSet	= 134;
	LogFont.lfQuality	= 1;
	LogFont.lfOutPrecision		= 1;
	LogFont.lfClipPrecision		= 2;
	LogFont.lfPitchAndFamily	= 1;
	
	lstrcpy(LogFont.lfFaceName, L"����");
	
	CFont fontRich;
	fontRich.CreateFontIndirect(&LogFont);	
	m_RichEdit.SetFont(&fontRich);

	//
	m_RichEdit.SetDefaultTextColor(RGB(192, 192, 0));
	m_RichEdit.SetBackgroundColor(false, RGB(0, 0, 0));

	//
	AdjustToMaxSize();
	
	//
//	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
//	pMainFrame->SetHotkeyTarget(this);

	//
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
	
	CenterWindow();

	return TRUE; 
}

BOOL CDlgF10::PreTranslateMessage(MSG* pMsg)
{
	if ( WM_KEYDOWN == pMsg->message )
	{
		
		if ( VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam )
		{
			
			PostMessage(WM_CLOSE,0,0);
			return TRUE;
		}
		else
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->ProcessHotkey(pMsg);
			return TRUE;
		}
	}
	else if ( WM_SYSKEYDOWN == pMsg->message )
	{
		if ( VK_F10 == pMsg->wParam )
		{
			PostMessage(WM_CLOSE,0,0);
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDlgF10::OnPaint() 
{
	CPaintDC dc(this); // device context for painting	
	
	UpdateShow();
	// Do not call CDialogEx::OnPaint() for painting messages
}

BOOL CDlgF10::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CDlgF10::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	UpdateShow();
}

void CDlgF10::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
	
//	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

// 	if ( WA_INACTIVE == nState )
// 	{
// 		pMainFrame->SetHotkeyTarget(NULL);		
// 	}
// 	else if ( WM_ACTIVATE == nState )
// 	{
// 		pMainFrame->SetHotkeyTarget(this);
// 	}
}

void CDlgF10::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
	delete this;
}

void CDlgF10::OnClose()
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

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
//	pMainFrame->SetHotkeyTarget(NULL);
	
	pMainFrame->m_pDlgF10 = NULL;
	DestroyWindow();
}

void CDlgF10::OnF10Menu(UINT nID)
{
	int32 iIndex = nID - ID_MENU_F10_BEGIN;

	if ( iIndex < 0 || iIndex >= m_aNewTitles.GetSize() )
	{
		//ASSERT(0);
		return;
	}

	T_NewsNode stNewsNode = m_aNewTitles.GetAt(iIndex);
	OnSelectedItemChange(stNewsNode.m_iInfoIndex);
}

void CDlgF10::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	m_bMaxed = true;
	//ModifyStyle(WS_CAPTION, NULL);

	AdjustToMaxSize();
}

void CDlgF10::OnLButtonUp(UINT nFlags, CPoint point)
{
	// ���ǲ����л�������Դ
	// ...fangz0618 ��ʱû�д���

	// �Ƿ����"����"
	if ( m_bNeedMore )
	{
		if ( m_RectMore.PtInRect(point) )
		{
			// �����˵�
			CNewMenu Menu;
			Menu.LoadMenu(IDR_MENU_F10);
			Menu.LoadToolBar(g_awToolBarIconIDs);
			
			CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, Menu.GetSubMenu(0));			
			ASSERT(NULL != pPopMenu);
			
			//
			bool32 bBegin = false;

			for( int32 i = 0; i < m_aNewTitles.GetSize(); i++ )
			{
				if ( m_aNewTitles.GetAt(i) != m_stNewsNodeFistAsMore && !bBegin )
				{
					continue;
				}
				else
				{
					bBegin = true;			
					T_NewsNode stNewsNode = m_aNewTitles.GetAt(i);
					pPopMenu->AppendODMenu(stNewsNode.m_StrContent, MF_STRING, ID_MENU_F10_BEGIN + i);

					if ( m_iInfoIndexNow == stNewsNode.m_iInfoIndex )
					{
						pPopMenu->CheckMenuItem(ID_MENU_F10_BEGIN + i, MF_BYCOMMAND | MF_CHECKED);
					}
				}
			}
	
			if ( bBegin )
			{
				CPoint pt;
				GetCursorPos(&pt);
				pPopMenu->RemoveMenu(0, MF_SEPARATOR);
				pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL, pt.x, pt.y, this);				
			}
			
			pPopMenu->DestroyMenu();			
					
			return;
		}
	}

	// �Ƿ��л���Ʒ
	CMerch* pMerchPre = NULL;
	CMerch* pMerchAft = NULL;
	
	GetSblingMerch(pMerchPre, pMerchAft);
	
	// �Ƿ��л�����Ʒ
	if ( m_RectMerchPre.PtInRect(point) )
	{
		// ǰһ����Ʒ
		if ( NULL != pMerchPre )
		{
			OnMerchChange(pMerchPre);
		}
		
		return;
	}
	
	if ( m_RectMerchAft.PtInRect(point) )
	{
		// ��һ����Ʒ
		if ( NULL != pMerchAft )
		{
			OnMerchChange(pMerchAft);
		}
		
		return;
	}

	// �Ƿ��л����ӱ���
	for ( int32 i = 0; i < m_aNewTitles.GetSize(); i++ )
	{
		if ( m_aNewTitles.GetAt(i).m_RectShow.PtInRect(point) )
		{
			//
			OnSelectedItemChange(m_aNewTitles.GetAt(i).m_iInfoIndex);
			return;
		}
	}
}

void CDlgF10::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{	
	CDialogEx::OnGetMinMaxInfo(lpMMI);

	lpMMI->ptMinTrackSize.x=300;   //��   
	lpMMI->ptMinTrackSize.y=300;   //�� 
}

void CDlgF10::AdjustToMaxSize()
{
	if ( !m_bMaxed )
	{
		return;
	}

	CRect RectClient;
	
	//
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();	

	::GetWindowRect(pMainFrame->m_hWndMDIClient, &RectClient);
	
	//pMainFrame->GetClientRect(&RectClient);
	
	//
	//RectClient.bottom -= pMainFrame->m_wndStatusBar.GetStatusBarHeight();
	//pMainFrame->ClientToScreen(&RectClient);
	
	//
	MoveWindow(&RectClient);
}

void CDlgF10::UpdateShow()
{
	if ( !m_RichEdit.GetSafeHwnd() )
	{
		return;
	}

	if ( NULL == m_pMerch )
	{
		// ��ʾѡ����Ʒ		
		m_RichEdit.SetWindowText(L"��ѡ����Ʒ.");
		return;
	}

	if ( m_aNewTitles.GetSize() <= 0 )
	{
		m_RichEdit.SetWindowText(L"��������.");
	}

	if ( m_bServerDisConnect )
	{
		m_RichEdit.SetWindowText(L"����Ѷ�����������ж�, ������...");
	}

	//
	COLORREF ClrFrame	 = RGB(192, 0, 0);
	COLORREF ClrBkGround = RGB(0, 0, 0);
	COLORREF ClrSelected = RGB(84, 255, 255);
	COLORREF ClrNormal	 = RGB(192, 192, 192);
	COLORREF ClrMerch	 = RGB(192, 192, 192);

	// 
	if ( m_stNewsNodeNow.m_StrContent.GetLength() > 0 )
	{
		m_RichEdit.SetWindowText(m_stNewsNodeNow.m_StrContent);	
	}	

	//
	CClientDC dcClient(this);
	CRect RectClient;
	GetClientRect(&RectClient);

	CMemDCEx dc(&dcClient, RectClient);
	dc.FillSolidRect(&RectClient, ClrBkGround);
	dc.SetTextColor(ClrNormal);

	CFont* pFontDlg = GetFont();
	CFont* pFontOld = dc.SelectObject(pFontDlg);

	//
	CSize sizeNormal;
	sizeNormal = dc.GetTextExtent(L"һ�����ġ�");

	// �������Ŀ�϶
	const int32 KiTopSkip	= 3;

	// �������Ŀ�϶
	const int32 KiSideSkip	= 3;

	// �м�Ŀ�϶
	int32 iCenterSkip		= 0;

	// һ����ť����ѿ��
	const int KiButtonWidth		= sizeNormal.cx + 5;

	// һ����ť����Ѹ߶�
	const int KiButtonHeight	= sizeNormal.cy + 20;
	//
	CSize tmpsz; 
	
	// ����Դ, ������ʱ��ʾ��Ʊ����
	tmpsz = dc->GetTextExtent( m_pMerch->m_MerchInfo.m_StrMerchCnName ) + CSize(5,0);

	COLORREF ClrBefore = dc->SetTextColor(ClrMerch);
	CRect RectDataSource = RectClient;

	RectDataSource.top	+= KiTopSkip;
	RectDataSource.left += KiSideSkip;
	RectDataSource.bottom= RectDataSource.top  + KiButtonHeight;
	RectDataSource.right = RectDataSource.left + tmpsz.cx;
	
	dc._DrawRect(RectDataSource, ClrFrame);
	dc.DrawText(m_pMerch->m_MerchInfo.m_StrMerchCnName, &RectDataSource, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// ��ʾ��Ʊ����
	tmpsz = dc->GetTextExtent( m_pMerch->m_MerchInfo.m_StrMerchCode ) + CSize(5,0);

	CRect RectCode	= RectClient;
	
	RectCode.top	= RectClient.top + KiTopSkip + KiButtonHeight + KiTopSkip;
	RectCode.bottom	= RectCode.top + KiButtonHeight;
	RectCode.left	= RectClient.left + KiSideSkip;
	RectCode.right	= RectCode.left + tmpsz.cx;

	dc._DrawRect(RectCode, ClrFrame);
	dc.DrawText(m_pMerch->m_MerchInfo.m_StrMerchCode, &RectCode, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// ������Ʒ
	CMerch* pMerchPre = NULL;
	CMerch* pMerchAft = NULL;

	CString StrMerchPre = L"";
	CString StrMerchAft = L"";

	GetSblingMerch(pMerchPre, pMerchAft);

	if ( NULL != pMerchPre )
	{
		StrMerchPre = pMerchPre->m_MerchInfo.m_StrMerchCnName;
		StrMerchPre += L"��";
	}

	if ( NULL != pMerchAft )
	{
		StrMerchAft = pMerchAft->m_MerchInfo.m_StrMerchCnName;
		StrMerchAft += L"��";
	}

	// ǰһ����Ʒ
	tmpsz = dc->GetTextExtent( StrMerchPre ) + CSize(5,0);

	m_RectMerchPre			= RectClient;

	m_RectMerchPre.top		= RectClient.top + KiTopSkip;
	m_RectMerchPre.bottom	= m_RectMerchPre.top + KiButtonHeight;
	m_RectMerchPre.right	= m_RectMerchPre.right - KiSideSkip;
	m_RectMerchPre.left		= m_RectMerchPre.right - tmpsz.cx;

	dc._DrawRect(m_RectMerchPre, ClrFrame);	
	dc.DrawText(StrMerchPre, &m_RectMerchPre, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

	// ��һ����Ʒ
	tmpsz = dc->GetTextExtent( StrMerchAft ) + CSize(5,0);

	m_RectMerchAft.top		= RectClient.top + KiTopSkip + KiButtonHeight + KiTopSkip;
	m_RectMerchAft.bottom	= m_RectMerchAft.top + KiButtonHeight;
	m_RectMerchAft.right	= RectClient.right - KiSideSkip;
	m_RectMerchAft.left		= m_RectMerchAft.right - tmpsz.cx;
	
	dc._DrawRect(m_RectMerchAft, ClrFrame);
	dc.DrawText(StrMerchAft, &m_RectMerchAft, DT_RIGHT | DT_VCENTER |DT_SINGLELINE);

	//
	dc->SetTextColor(ClrBefore);

	// �м�ı��ⰴť
	if ( m_aNewTitles.GetSize() <= 0 )
	{
		CRect RectRichEdit = RectClient;
		RectRichEdit.top   = m_RectMerchAft.bottom + 3;
		m_RichEdit.MoveWindow(&RectRichEdit);
		
		CRect rectSwap;
		rectSwap = RectClient;
		rectSwap.bottom = RectRichEdit.top;
		
		dc.Swap(rectSwap);
		dc.SelectObject(pFontOld);
		// m_RichEdit.SetFocus();		
		
		UpdateData(FALSE);
		return;
	}

	// �������Ұ�ť�Ϳ�϶, ʣ��Ŀ��
	int32 iWidthLeave = RectClient.Width() - (KiSideSkip * 4) - KiButtonWidth * 2;

	// һ����ť���Ų���, �����ܵ����
	if ( iWidthLeave < KiButtonWidth )
	{
		//ASSERT(0);
		return;			
	}

	// ÿ���ܷ��µİ�ť����
	int32 iBtnNums = iWidthLeave / KiButtonWidth;
	
	// ÿ��ʵ�ʷŵİ�ť����
	int32 iBtnNumsReal = -1;

	// ǰһ����ť�����
	int32 iLeftPre = -1;

	// �Ƿ�ڶ���
	bool32 bRow2 = false;

	// ����
	T_NewsNode* pNewsNode = (T_NewsNode*)m_aNewTitles.GetData();

	if ( iBtnNums * 2 >= m_aNewTitles.GetSize() )
	{
		// ����ȫ�ŵ���
		iBtnNumsReal = (m_aNewTitles.GetSize() + 1) / 2;

		// �м����ߵ�����
		iCenterSkip = (iWidthLeave - iBtnNumsReal * KiButtonWidth) / 2;

		//
		iLeftPre = RectDataSource.right + KiSideSkip + iCenterSkip;

		for ( int32 i = 0; i < m_aNewTitles.GetSize(); i++ )
		{		
			dc->SetTextColor(ClrNormal);

			//
			T_NewsNode* pNode = &pNewsNode[i];
			
			pNode->m_RectShow.left		= iLeftPre;
			pNode->m_RectShow.right		= pNode->m_RectShow.left + KiButtonWidth;

			if ( !bRow2 )
			{
				pNode->m_RectShow.top		= RectDataSource.top;
				pNode->m_RectShow.bottom	= RectDataSource.bottom;
			}
			else
			{
				pNode->m_RectShow.top		= RectCode.top;
				pNode->m_RectShow.bottom	= RectCode.bottom;
			}
						
			//
			iLeftPre = pNode->m_RectShow.right;
			
			//
			if ( pNode->m_iInfoIndex == m_iInfoIndexNow )
			{
				dc.SetTextColor(ClrSelected);
			}
			
			dc._DrawRect(pNode->m_RectShow, ClrFrame);
			dc.DrawText(pNode->m_StrContent, pNode->m_RectShow, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			if ( i + 1 == iBtnNumsReal )
			{
				// �ڶ�����
				bRow2 = true;
				iLeftPre = RectDataSource.right + KiSideSkip + iCenterSkip;
			}
		}
	}
	else
	{
		// ������ȫ����, ��Ҫ���ఴť
		m_bNeedMore = true;
		m_stNewsNodeFistAsMore.Clear();

		//
		iBtnNumsReal = iBtnNums;

		// �м����ߵ�����
		iCenterSkip = (iWidthLeave - iBtnNumsReal * KiButtonWidth) / 2;
		
		//
		iLeftPre = RectDataSource.right + KiSideSkip + iCenterSkip;

		// �Ƿ�ѡ����Ǹ������
		bool32 bSelectedMore = true;

		//
		for ( int32 i = 0; i < m_aNewTitles.GetSize(); i++ )
		{	
			dc->SetTextColor(ClrNormal);

			if ( i == (iBtnNumsReal * 2 - 1) )
			{
				// ���һ����ť, ������
				m_RectMore.left		= iLeftPre;
				m_RectMore.right	= m_RectMore.left + KiButtonWidth;
				m_RectMore.top		= RectCode.top;
				m_RectMore.bottom	= RectCode.bottom;
				
				//
				if ( bSelectedMore )
				{
					dc.SetTextColor(ClrSelected);
				}
				
				dc._DrawRect(m_RectMore, ClrFrame);
				dc.DrawText(L"����...", m_RectMore, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				
				m_stNewsNodeFistAsMore = m_aNewTitles.GetAt(i);
				break;
			}

			// �������
			T_NewsNode* pNode = &pNewsNode[i];
			
			pNode->m_RectShow.left		= iLeftPre;
			pNode->m_RectShow.right		= pNode->m_RectShow.left + KiButtonWidth;
			
			if ( !bRow2 )
			{
				pNode->m_RectShow.top		= RectDataSource.top;
				pNode->m_RectShow.bottom	= RectDataSource.bottom;
			}
			else
			{
				pNode->m_RectShow.top		= RectCode.top;
				pNode->m_RectShow.bottom	= RectCode.bottom;
			}
			
			//
			iLeftPre = pNode->m_RectShow.right;

			if ( pNode->m_iInfoIndex == m_iInfoIndexNow )
			{
				bSelectedMore = false;
				dc.SetTextColor(ClrSelected);
			}
			
			dc._DrawRect(pNode->m_RectShow, ClrFrame);
			dc.DrawText(pNode->m_StrContent, pNode->m_RectShow, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			//
			if ( i + 1 == iBtnNumsReal )
			{
				// �ڶ�����
				bRow2 = true;
				iLeftPre = RectDataSource.right + KiSideSkip + iCenterSkip;
			}
		}		
	}

	//
	CRect RectRichEdit = RectClient;
	RectRichEdit.top   = m_RectMerchAft.bottom + 3;
	m_RichEdit.MoveWindow(&RectRichEdit);

	CRect rectSwap;
	rectSwap = RectClient;
	rectSwap.bottom = RectRichEdit.top;

	dc.Swap(rectSwap);
	dc.SelectObject(pFontOld);

	UpdateData(FALSE);
}

void CDlgF10::GetSblingMerch(OUT CMerch*& pMerchPre, OUT CMerch*& pMerchAft)
{
	pMerchPre = NULL;
	pMerchAft = NULL;

	if ( NULL == m_pMerch || NULL == m_pIoViewActive )
	{
		return;
	}

	//
	bool32 bOK1 = false;	 
	bool32 bOK2 = false;	 

	if ( !bOK1 )
	{			
		m_pAbsCenterManager->GetMerchManager().FindPrevMerch(m_pMerch, pMerchPre);
	}

	if ( !bOK2 )
	{
		m_pAbsCenterManager->GetMerchManager().FindNextMerch(m_pMerch, pMerchAft);
	}
}

bool32 CDlgF10::RequestF10(CMerch* pMerch, int32 iIndexInfo)
{
	if ( NULL == pMerch || NULL == m_pAbsCenterManager )
	{
		return false;
	}
	
	if ( m_bServerDisConnect )
	{
		return false;
	}

	if ( ERSReqing == m_eReqState && pMerch == m_pMerch )
	{
		// ͬһ��Ʒ��������, ��Ҫ�ٷ���
		return false;
	}

	m_eReqState = ERSReqing;

	//
	int32 iCount = 0;

	if ( iIndexInfo == EITF10 )
	{		
		iCount = 100;
	}
	else
	{
		iCount = KiMaxCount;
	}

	m_iInfoIndexNow = iIndexInfo;

	//
	CMmiNewsReqF10 Req;
	Req.m_iBeginPos		= 0;
	Req.m_iCount		= iCount;
	Req.m_iIndexInfo	= m_iInfoIndexNow;
	Req.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	Req.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
	
	m_pAbsCenterManager->RequestNews(&Req);
	
	return true;
}

void CDlgF10::OnNewsServerDisConnected(int32 iCommunicationId)
{
	CString StrTime;
	CTime timeOff(NULL);

	StrTime.Format(L"%02d:%02d:%02d", timeOff.GetHour(), timeOff.GetMinute(), timeOff.GetSecond());

	TRACE(L"ʱ��: %s ��ѯ���������� %d \n", StrTime.GetBuffer(), iCommunicationId);

	m_bServerDisConnect = true;
	m_eReqState = ERSReqFinished;
	m_stNewsNodeNow.Clear();

	UpdateShow();
}

void CDlgF10::OnNewsServerConnected(int32 iCommunicationId)
{
	m_bServerDisConnect = false;

	//
	m_eReqState = ERSReqFinished;
	RequestF10(m_pMerch, m_iInfoIndexNow);
}	


void CDlgF10::OnDataRespNewsData(int iMmiReqId, IN const CMmiNewsBase *pMmiNewResp, CMerch *pMerch)
{
	if (pMmiNewResp->m_eNewsType ==  ENTRespF10)
	{
		OnNewsRespF10((CMmiNewsRespF10* )pMmiNewResp);
	}
}


void CDlgF10::OnNewsRespF10(IN const CMmiNewsRespF10* pMmiNewsRespF10)
{
	//
	if ( NULL == pMmiNewsRespF10 )
	{
		//ASSERT(0);
		return;
	}

	//
	m_eReqState = ERSReqFinished;

	//
	CString StrErrMsg;
	CString StrF10 = pMmiNewsRespF10->m_StrF10;
	TCHAR* pStrF10 = StrF10.LockBuffer();

 	if ( !ParaseF10String(pStrF10, StrErrMsg) )
 	{
		StrF10.UnlockBuffer();
 		//ASSERT(0);
 		return;
 	}

	StrF10.UnlockBuffer();

	//
	if ( EITF10 == m_iInfoIndexNow )
	{
		// �����ǰ�� F10 �Ļ�. �����������������. ������һ��Ĭ�ϵĵ�һ���������������
		if ( m_aNewTitles.GetSize() > 0 )
		{			
			RequestF10(m_pMerch, m_aNewTitles.GetAt(0).m_iInfoIndex);
		}
	}

	//	
	UpdateShow();
}

LRESULT CDlgF10::OnMsgHotKey(WPARAM wParam, LPARAM lParam)
{
	CHotKey *pHotKey = (CHotKey *)wParam;
	ASSERT(pHotKey);
	
	CHotKey HotKey = *pHotKey;
	DEL(pHotKey);
	
	// 
	if (EHKTMerch == HotKey.m_eHotKeyType)
	{
		int32 iMarketId = HotKey.m_iParam1;
		CString StrMerchCode = HotKey.m_StrParam1;

		CMerch* pMerch = NULL;
		m_pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerch);
	
		if ( NULL != pMerch )
		{
			OnMerchChange(pMerch);
		}
	}

	return 0;
}

void CDlgF10::OnMerchChange(IN CMerch* pMerch, bool32 bForceReq /*= false*/)
{
 	if ( NULL == pMerch )
	{
		return;
	}

	if ( !bForceReq && pMerch == m_pMerch )
	{
		return;
	}

	//
	m_stNewsNodeNow.Clear();

	// ���� F10 ����:
	if ( RequestF10(pMerch, EITF10) )
	{
		m_pMerch = pMerch;

		//
		if ( NULL != m_pIoViewActive && NULL != m_pAbsCenterManager )
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			pFrame->OnViewMerchChanged(m_pIoViewActive, pMerch);							
		}
		Invalidate();
	}
}

void CDlgF10::OnSelectedItemChange(int32 iIndexNow)
{
	// �жϺϷ���
	bool32 bValid = false;
	for ( int32 i = 0; i < m_aNewTitles.GetSize(); i++ )
	{
		if ( iIndexNow == m_aNewTitles.GetAt(i).m_iInfoIndex )
		{
			bValid = true;
			break;
		}
	}
	
	//
	if ( !bValid )
	{
		//ASSERT(0);
		return;
	}

	//
	if ( iIndexNow == m_iInfoIndexNow )
	{
		return;
	}

	// 
	RequestF10(m_pMerch, iIndexNow);
}

int32 CDlgF10::CStringToInt(TCHAR* pStrNum)
{
	if ( NULL == pStrNum )
	{
		return -1;
	}
	
	for ( int i = 0; i < (int)wcslen(pStrNum); i++ )
	{
		if ( !iswdigit(pStrNum[i]) )
		{
			return -1;
		}
	}
	 
	string sNum = _Unicode2MultiChar((CString)pStrNum);
	const char* pStrNumTmp = sNum.c_str();
	
	int iReval = 0;
	if (pStrNumTmp != NULL)
	{
		iReval = atoi(pStrNumTmp);
	}
	
	return iReval;
}

int32 CDlgF10::CStringToInt(const CString& StrNum)
{
	CString StrNumTmp = StrNum;

	TCHAR* pStrNum = StrNumTmp.LockBuffer();
	
	int32 iReval = CStringToInt(pStrNum);

	StrNumTmp.UnlockBuffer();

	return iReval;
}

bool32 CDlgF10::BeValidSpecialName(TCHAR* pStrSpecialName)
{
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyInfoIndex) )
	{
		return true;
	}

	if ( 0 == wcscmp(pStrSpecialName, KStrKeyBeginPos) )
	{
		return true;
	}
	
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyTotalCount) )
	{
		return true;
	}

	if ( 0 == wcscmp(pStrSpecialName, KStrKeyMarket) )
	{
		return true;
	}

	if ( 0 == wcscmp(pStrSpecialName, KStrKeyCode) )
	{
		return true;
	}

	return false;
}

bool32 CDlgF10::BeValidNormalName(TCHAR* pStrSpecialName)
{
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyInfoIndex) )
	{
		return true;
	}
	
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyDataType) )
	{
		return true;
	}
	
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyInfoType) )
	{
		return true;
	}
	
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyChildDataType) )
	{
		return true;
	}
	
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyContent) )
	{
		return true;
	}
	
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyUpdateTime) )
	{
		return true;
	}

	if ( 0 == wcscmp(pStrSpecialName, KStrKeyCrc32) )
	{
		return true;
	}

	return false;
}

void CDlgF10::SetSpecialNodeValue(int iIndex, const CString& StrValue, T_SpecialNode& stSpecialNode)
{
	CString StrKey;

	m_aMapTableSpecial.Lookup(iIndex, StrKey);
	
	//
	if ( 0 == wcscmp(StrKey, KStrKeyInfoIndex) )
	{
		// ���� InfoIndex
		int32 iValue = CStringToInt(StrValue);
		if ( -1 == iValue )
		{
			//ASSERT(0);
			return;
		}

		stSpecialNode.m_iInfoIndex = iValue;
		return;
	}
	
	if ( 0 == wcscmp(StrKey, KStrKeyBeginPos) )
	{
		// ���� DataType
		int32 iValue = CStringToInt(StrValue);
		if ( -1 == iValue )
		{
			//ASSERT(0);
			return;
		}
		
		stSpecialNode.m_iBeginPos = iValue;

		return;
	}
	
	if ( 0 == wcscmp(StrKey, KStrKeyTotalCount) )
	{
		// ���� TotalCount
		int32 iValue = CStringToInt(StrValue);
		if ( -1 == iValue )
		{
			//ASSERT(0);
			return;
		}
		
		stSpecialNode.m_iTotalCount = iValue;

		return;
	}
	
	if ( 0 == wcscmp(StrKey, KStrKeyMarket) )
	{
		// ���� Market
		int32 iValue = CStringToInt(StrValue);
		if ( -1 == iValue )
		{
			//ASSERT(0);
			return;
		}
		
		stSpecialNode.m_iMarketId = iValue;

		return;
	}
	
	if ( 0 == wcscmp(StrKey, KStrKeyCode) )
	{
		// ���� Code
		stSpecialNode.m_StrCode = StrValue;

		return;
	}

	//ASSERT(0);
}	

void CDlgF10::SetNewsNodeValue(int iIndex, const CString& StrValue, T_NewsNode& stNewsNode)
{
	CString StrKey;
	
	m_aMapTableNewsNode.Lookup(iIndex, StrKey);
	
	//
	if ( 0 == wcscmp(StrKey, KStrKeyInfoIndex) )
	{
		// ���� InfoIndex
		int32 iValue = CStringToInt(StrValue);
		if ( -1 == iValue )
		{
			//ASSERT(0);
			return;
		}
		
		stNewsNode.m_iInfoIndex = iValue;
		return;
	}
	
	if ( 0 == wcscmp(StrKey, KStrKeyDataType) )
	{
		// ���� DataType
		int32 iValue = CStringToInt(StrValue);
		if ( -1 == iValue )
		{
			//ASSERT(0);
			return;
		}
		
		stNewsNode.m_eDataType = (E_DataType)iValue;
		
		return;
	}
	
	if ( 0 == wcscmp(StrKey, KStrKeyInfoType) )
	{
		// ���� InfoType
		int32 iValue = CStringToInt(StrValue);
		if ( -1 == iValue )
		{
			//ASSERT(0);
			return;
		}
		
		stNewsNode.m_eInfoType = (E_InfoType)iValue;
		
		return;
	}
	
	if ( 0 == wcscmp(StrKey, KStrKeyChildDataType) )
	{
		// ���� ChildDataType
		int32 iValue = CStringToInt(StrValue);
		if ( -1 == iValue )
		{
			//ASSERT(0);
			return;
		}
		
		stNewsNode.m_eChildDataType = (E_DataType)iValue;
		
		return;
	}
	
	if ( 0 == wcscmp(StrKey, KStrKeyContent) )
	{
		// ���� Content
		stNewsNode.m_StrContent = StrValue;
		
		return;
	}

	if ( 0 == wcscmp(StrKey, KStrKeyUpdateTime) )
	{
		// ���� UpdateTime
		stNewsNode.m_StrTime = StrValue;
		
		return;
	}

	if ( 0 == wcscmp(StrKey, KStrKeyCrc32) )
	{
		// ���� Content
		stNewsNode.m_StrCrc32 = StrValue;
		
		return;
	}
	
	//ASSERT(0);
}

bool32 CDlgF10::ParaseF10String(IN TCHAR* pStrF10, OUT CString& StrErrMsg)
{
	StrErrMsg = L"";
	m_aMapTableNewsNode.RemoveAll();
	m_aMapTableSpecial.RemoveAll();

	int32 iStrLen = wcslen(pStrF10);

	//
	if ( iStrLen <= 0  )
	{
		StrErrMsg = L"�����ַ������ȴ���";
		return false;
	}

	// ����ͷ���������ֶ�: �����ֶ���, ����, ����
	int32 iSpecial = 0;
	int32 iRows	 = 0;
	int32 iCols	 = 0;
	int32 iProFeildCount = 0;
	
	TCHAR* pCur = NULL;
	for ( pCur = pStrF10; (pCur - pStrF10)< iStrLen && iProFeildCount < KiProFiledCounts; )
	{
		TCHAR StrTmp[1024] = {0};

		// �ҵ���һ���ָ���
		TCHAR* pNext = wcschr(pCur, KcSeprator);
		if ( NULL == pNext )
		{
			//ASSERT(0);
			StrErrMsg.Format(L"�ڵ�ǰ�ַ��� %s ���Ҳ����ָ���", pCur);
			return false;
		}

		// ���������ָ����м���ַ���
		int32 iLen = pNext - pCur;
		wcsncpy(StrTmp, pCur, iLen);
		StrTmp[iLen] = '\0';

		// �õ�����
		int32 iNum = CStringToInt(StrTmp);
		if ( -1 == iNum )
		{
			//ASSERT(0);
			StrErrMsg.Format(L"�ַ��� %s Ӧ���Ǵ������ַ���", StrTmp);
			return false;
		}

		// ��ֵ
		if ( 0 == iProFeildCount )
		{
			iSpecial = iNum;
		}
		else if ( 1 == iProFeildCount )
		{
			iRows = iNum;
		}
		else if ( 2 == iProFeildCount )
		{
			iCols = iNum;
		}

		//
		++iProFeildCount;
		pCur = pNext + 1;
	}
	
	// ����Э����
	
	// ���������ֶ���
	int32 i = 0;
	for ( i = 0; i < iSpecial; i++ )
	{
		TCHAR StrTmp[1024] = {0};
		TCHAR* pNext = wcschr(pCur, KcSeprator);
		
		if ( NULL == pNext )
		{
			//ASSERT(0);
			StrErrMsg.Format(L"���������ֶ���: �ڵ�ǰ�ַ��� %s ���Ҳ����ָ���", pCur);
			return false;
		}
		
		// ������һ���ֶ�����
		int32 iLen = pNext - pCur;
		wcsncpy(StrTmp, pCur, iLen);
		StrTmp[iLen] = '\0';
		
		// �ж��Ƿ�Ϸ�
		if ( !BeValidSpecialName(StrTmp) )
		{
			//ASSERT(0);
			StrErrMsg.Format(L"�Ƿ��������ֶ���: %s", StrTmp);
			return false;
		}
		
		// ��������ֵ
		m_aMapTableSpecial[i] = StrTmp;
		
		//
		pCur = pNext + 1;
	}
	
	// ������ͨ���ֶ���
	for ( i = 0; i < iCols; i++ )
	{
		TCHAR StrTmp[1024] = {0};
		TCHAR* pNext = wcschr(pCur, KcSeprator);
		
		if ( NULL == pNext )
		{
			//ASSERT(0);
			StrErrMsg.Format(L"������ͨ�ֶ���: �ڵ�ǰ�ַ��� %s ���Ҳ����ָ���", pCur);
			return false;
		}
		
		// ������һ���ֶ�����
		int32 iLen = pNext - pCur;
		wcsncpy(StrTmp, pCur, iLen);
		StrTmp[iLen] = '\0';
		
		// �ж��Ƿ�Ϸ�
		if ( !BeValidNormalName(StrTmp) )
		{
			//ASSERT(0);
			StrErrMsg.Format(L"�Ƿ�����ͨ�ֶ���: %s", StrTmp);
			return false;
		}
		
		// ��������ֵ
		m_aMapTableNewsNode[i] = StrTmp;
		
		//
		pCur = pNext + 1;
	}
	
	// ���������ֶε�ֵ
	for ( i = 0; i < iSpecial; i++ )
	{
		TCHAR StrTmp[1024] = {0};
		TCHAR* pNext = wcschr(pCur, KcSeprator);
		
		if ( NULL == pNext )
		{
			//ASSERT(0);
			StrErrMsg.Format(L"���������ֶ�ֵ: �ڵ�ǰ�ַ��� %s ���Ҳ����ָ���", pCur);
			return false;
		}
		
		// ������һ���ֶ�����
		int32 iLen = pNext - pCur;
		_tcsncpy(StrTmp, pCur, iLen);
		StrTmp[iLen] = '\0';
		
		// ���������ֶε�ֵ
		SetSpecialNodeValue(i, StrTmp, m_stSpecialNode);
		
		//
		pCur = pNext + 1;
	}
		
	// ������ͨ�ֶεĽڵ�ֵ
	if ( EITF10 == m_iInfoIndexNow )
	{
		m_aNewTitles.RemoveAll();

		T_NewsNode stNewsNodeToAdd;
		for ( i = 0; i < iRows; i++ )
		{
			// ����ÿһ��
			for ( int32 j = 0; j < iCols; j++ )
			{
				// ����ÿһ��
				TCHAR* pNext = wcschr(pCur, KcSeprator);
				
				if ( NULL == pNext )
				{
					//ASSERT(0);
					StrErrMsg.Format(L"������ͨ�ֶ�ֵ: �ڵ�ǰ�ַ��� %s ���Ҳ����ָ���, ���� %d ��, %d ��", pCur, i, j);
					return false;
				}
				
				int32 iLen = pNext - pCur;
				
				TCHAR* pStrTmp = new TCHAR[iLen + 10];
				wcsncpy(pStrTmp, pCur, iLen);
				pStrTmp[iLen] = '\0';
				
				// �����ֶ�ֵ
				SetNewsNodeValue(j, pStrTmp, stNewsNodeToAdd);
				
				//
				DEL_ARRAY(pStrTmp);
				
				// 
				pCur = pNext + 1;
			}
			
			// ����
			m_aNewTitles.Add(stNewsNodeToAdd);		
		}
	}
	else
	{
		// ��ǰ�ĸ�ֵ
		ASSERT(iRows == 1);

		for ( i = 0; i < iRows; i++ )
		{
			// ����ÿһ��
			for ( int32 j = 0; j < iCols; j++ )
			{
				// ����ÿһ��
				TCHAR* pNext = wcschr(pCur, KcSeprator);
				
				if ( NULL == pNext )
				{
					//ASSERT(0);
					StrErrMsg.Format(L"������ͨ�ֶ�ֵ: �ڵ�ǰ�ַ��� %s ���Ҳ����ָ���, ���� %d ��, %d ��", pCur, i, j);
					return false;
				}
				
				int32 iLen = pNext - pCur;
				
				TCHAR* pStrTmp = new TCHAR[iLen + 10];
				wcsncpy(pStrTmp, pCur, iLen);
				pStrTmp[iLen] = '\0';
				
				// �����ֶ�ֵ
				SetNewsNodeValue(j, pStrTmp, m_stNewsNodeNow);
				
				//
				DEL_ARRAY(pStrTmp);
				
				// 
				pCur = pNext + 1;
			}						
		}
	}

	return true;
}
