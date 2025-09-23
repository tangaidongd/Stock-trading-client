#include "StdAfx.h"
#include "dialogchangepassword.h"
#include "XmlShare.h"
#include "facescheme.h"
#include "IoViewShare.h"
#include "DlgTip.h"
#include "DlgCloseTradeTip.h"
#include "FontFactory.h"

//#pragma optimize("g", off)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
	#define		IDC_DEF_STATIC_OLDPWD						WM_USER+170
	#define		IDC_DEF_STATIC_OLDPWD_BK					WM_USER+171
	#define		IDC_DEF_EDIT_OLDPWD							WM_USER+172
	#define		IDC_DEF_STATIC_NEWPWD						WM_USER+173
	#define		IDC_DEF_STATIC_NEWPWD_BK					WM_USER+174
	#define		IDC_DEF_EDIT_NEWPWD							WM_USER+175
	#define		IDC_DEF_STATIC_CHECKPWD						WM_USER+176
	#define		IDC_DEF_STATIC_CHECKPWD_BK					WM_USER+177
	#define		IDC_DEF_EDIT_CHECKPWD						WM_USER+178
	#define		IDC_DEF_STATIC_PWDTYPE						WM_USER+179
	#define		IDC_DEF_DROP_PWDTYPE						WM_USER+180
	#define		IDC_DEF_BUTTON_COMMIT						WM_USER+181
		
	#define TEXT_COLOR_RED			 RGB(255, 0, 0)
	#define TEXT_COLOR_GRE			 RGB(0, 128, 0)
	#define GRID_BK_COLOR			 RGB(255,255,255)
	#define GRID_TEXT_COLOR			 RGB(77,77,77)
		
	#ifndef ArraySize
	#define ArraySize(Array) (sizeof(Array)/sizeof(Array[0]))
	#endif	
}

BEGIN_MESSAGE_MAP(CDlgChangePwd, CDialog)
//{{AFX_MSG_MAP(CDlgTradeQuery)
ON_WM_ERASEBKGND()
ON_WM_SIZE()
ON_WM_DESTROY()
ON_WM_PAINT()
ON_WM_CTLCOLOR()
ON_WM_TIMER()
ON_WM_LBUTTONUP()
ON_WM_LBUTTONDOWN()
ON_WM_MOUSEMOVE()
ON_BN_CLICKED(IDC_DEF_BUTTON_COMMIT,OnBtnCommit)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CDlgChangePwd::CDlgChangePwd( CWnd* pParent ) : CDialog(IDD, pParent)
{
	m_pTradeBid = NULL;	
	m_pTradeLoginInfo = NULL;
}

CDlgChangePwd::~CDlgChangePwd()
{	
}

void CDlgChangePwd::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTradeQuery)
	//DDX_Control(pDX, IDC_STATIC_ACCOUNT, m_StaticAccount);
	//}}AFX_DATA_MAP
}

BOOL CDlgChangePwd::OnInitDialog()
{
	CDialog::OnInitDialog();
	CreateControl();	// 创建控件

	return TRUE;
}

void CDlgChangePwd::OnOK()
{

}

void CDlgChangePwd::OnCancel()
{
	
}

BOOL CDlgChangePwd::OnEraseBkgnd( CDC* pDC )
{
	if ( NULL!=pDC )
	{
		CRect rect(0,0,0,0);
		GetClientRect(rect);
		pDC->FillSolidRect(rect, RGB(0xFC, 0xFD, 0xF5));
		return TRUE;
	}
	return FALSE;
}

void CDlgChangePwd::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);
	RecalcLayout(false);
}

void CDlgChangePwd::RecalcLayout( bool bNeedDraw )
{
	CRect rc(0,0,0,0);
	GetClientRect(rc);
	
	
	int iStaticVSpace	= 10;				// 控件竖向间隔
	int iSpace			= 10;					// 控件横向间隔
	int iInset			= 30;					// 对话框周边间隔
	int iHeight			= 30;
	int iStaticPos		= 5;
	int iStaticWidth	= 70;
	
	
	CRect rectStaticPwdType(rc);
	rectStaticPwdType.left = rc.left+iInset;
	rectStaticPwdType.top = rc.top+iInset + iStaticPos;
	rectStaticPwdType.right = rectStaticPwdType.left+iStaticWidth;
	rectStaticPwdType.bottom = rectStaticPwdType.top+iHeight - iStaticPos;
	if (m_StaticPwdType.m_hWnd)
	{
		m_StaticPwdType.MoveWindow(rectStaticPwdType);
	}
	CRect rectDropPwdType(rectStaticPwdType);
	rectDropPwdType.left = rectStaticPwdType.right+iSpace;
	rectDropPwdType.top = rectStaticPwdType.top-iStaticPos;
	rectDropPwdType.right = rc.right-iInset;
	if (m_DropPwdType.m_hWnd)
	{
		m_DropPwdType.MoveWindow(rectDropPwdType);
	}
	
	CRect rectStaticOldPwd(rectStaticPwdType);
	rectStaticOldPwd.top = rectStaticPwdType.bottom+iStaticVSpace + iStaticPos;
	rectStaticOldPwd.bottom = rectStaticOldPwd.top+iHeight - iStaticPos;
	if (m_StaticOldPwd.m_hWnd)
	{
		m_StaticOldPwd.MoveWindow(rectStaticOldPwd);
	}
	CRect rectStaticBKOldPwd(rectStaticOldPwd);
	rectStaticBKOldPwd.left = rectStaticOldPwd.right+iSpace;
	rectStaticBKOldPwd.top = rectStaticOldPwd.top - iStaticPos;
	rectStaticBKOldPwd.right = rc.right-iInset;
	if (m_StaticOldPwdBK.m_hWnd)
	{
		m_StaticOldPwdBK.MoveWindow(rectStaticBKOldPwd);
	}
	CRect rectEditOldPwd(rectStaticBKOldPwd);
	rectEditOldPwd.InflateRect(-1,-1,-1,-1);
	if (m_editOldPwd.m_hWnd)
	{
		m_editOldPwd.MoveWindow(rectEditOldPwd);
		CRect rect;
		m_editOldPwd.GetClientRect(&rect);
		rect.DeflateRect(0,3,0,3);
		m_editOldPwd.SetRectNP(&rect);
	}
	
	CRect rectStaticNewPwd(rectStaticOldPwd);
	rectStaticNewPwd.top = rectStaticOldPwd.bottom+iStaticVSpace + iStaticPos;
	rectStaticNewPwd.bottom = rectStaticNewPwd.top+iHeight - iStaticPos;
	if (m_StaticNewPwd.m_hWnd)
	{
		m_StaticNewPwd.MoveWindow(rectStaticNewPwd);
	}
	CRect rectStaticBKNewPwd(rectStaticNewPwd);
	rectStaticBKNewPwd.left = rectStaticNewPwd.right+iSpace;
	rectStaticBKNewPwd.top = rectStaticNewPwd.top - iStaticPos;
	rectStaticBKNewPwd.right = rc.right-iInset;
	if (m_StaticNewPwdBK.m_hWnd)
	{
		m_StaticNewPwdBK.MoveWindow(rectStaticBKNewPwd);
	}
	CRect rectEditNewPwd(rectStaticBKNewPwd);
	rectEditNewPwd.InflateRect(-1,-1,-1,-1);
	if (m_editNewPwd.m_hWnd)
	{
		m_editNewPwd.MoveWindow(rectEditNewPwd);
		CRect rt;
		m_editNewPwd.GetClientRect(&rt);
		rt.DeflateRect(0,3,0,3);
		m_editNewPwd.SetRectNP(&rt);
	}
	
	CRect rectStaticCheckPwd(rectStaticNewPwd);
	rectStaticCheckPwd.top = rectStaticNewPwd.bottom+iStaticVSpace + iStaticPos;
	rectStaticCheckPwd.bottom = rectStaticCheckPwd.top+iHeight - iStaticPos;
	if (m_StaticCheckPwd.m_hWnd)
	{
		m_StaticCheckPwd.MoveWindow(rectStaticCheckPwd);
	}
	CRect rectStaticBKCheckPwd(rectStaticCheckPwd);
	rectStaticBKCheckPwd.left = rectStaticCheckPwd.right+iSpace;
	rectStaticBKCheckPwd.top = rectStaticBKCheckPwd.top - iStaticPos;
	rectStaticBKCheckPwd.right = rc.right-iInset;
	if (m_StaticCheckPwdBK.m_hWnd)
	{
		m_StaticCheckPwdBK.MoveWindow(rectStaticBKCheckPwd);
	}
	CRect rectEditCheckPwd(rectStaticBKCheckPwd);
	rectEditCheckPwd.InflateRect(-1,-1,-1,-1);
	if (m_editCheckPwd.m_hWnd)
	{
		m_editCheckPwd.MoveWindow(rectEditCheckPwd);
		CRect rct;
		m_editCheckPwd.GetClientRect(&rct);
		rct.DeflateRect(0,3,0,3);
		m_editCheckPwd.SetRectNP(&rct);
	}
	
	
	CRect rectBtnCommit(rectStaticBKCheckPwd);
	rectBtnCommit.left = rc.left + iInset;
	rectBtnCommit.top = rectStaticBKCheckPwd.bottom + iStaticVSpace;
	rectBtnCommit.bottom = rectBtnCommit.top + iHeight;
	m_btnCommit.SetRect(rectBtnCommit);
	
	
	
	if ( bNeedDraw )
	{
		Invalidate(TRUE);
	}
}

void CDlgChangePwd::SetTradeBid( iTradeBid *pTradeBid )
{
	if ( m_pTradeBid )
	{
		m_pTradeBid->RemoveTradeBidNotify(this);
	}

	m_pTradeBid = pTradeBid;
	if (NULL==m_pTradeBid)
	{
		//ASSERT(0);
		return;
	}
	m_pTradeBid->AddTradeBidNotify(this);		
}

//////////////////////////////////////////////////////////////////////////
// other

void CDlgChangePwd::CreateControl()
{
	CString strFontName = gFontFactory.GetExistFontName(L"微软雅黑");
	
	LOGFONT lgFont = {0};
	lgFont.lfHeight =20;
    lgFont.lfWidth = 0;
    lgFont.lfEscapement = 0;
    lgFont.lfOrientation = 0;
    lgFont.lfWeight =  FW_NORMAL;
    lgFont.lfItalic = 0;
    lgFont.lfUnderline = 0;
    lgFont.lfStrikeOut = 0;
	lgFont.lfCharSet = ANSI_CHARSET;
    lgFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lgFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lgFont.lfQuality = PROOF_QUALITY;
    lgFont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
	_tcscpy(lgFont.lfFaceName, strFontName);///
	

	m_pFont.CreateFontIndirect(&lgFont);

	//修改密码 
	m_StaticPwdType.Create(L"密码类型:", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_PWDTYPE);
	m_StaticPwdType.ShowWindow(SW_SHOW);
	m_StaticPwdType.SetFont(&m_pFont);
	m_DropPwdType.Create( WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED, CRect(0, 0, 0, 200), this, IDC_DEF_DROP_PWDTYPE);
	m_DropPwdType.ShowWindow(SW_SHOW);
	m_DropPwdType.SetFont(&m_pFont);
	m_DropPwdType.AddItemEx(L"交易密码");
	m_DropPwdType.AddItemEx(L"电话密码");
	m_DropPwdType.AddItemEx(L"资金密码");
	m_DropPwdType.SetCurSel(0);
	
	m_StaticOldPwd.Create(L"旧的密码:", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_OLDPWD);
	m_StaticOldPwd.ShowWindow(SW_SHOW);
	m_StaticOldPwd.SetFont(&m_pFont);	
	m_StaticOldPwdBK.Create(L"", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_OLDPWD_BK);
	m_StaticOldPwdBK.ShowWindow(SW_SHOW);
	m_editOldPwd.Create(ES_MULTILINE|ES_AUTOHSCROLL|ES_LEFT|WS_VISIBLE, CRect(0, 0, 100, 30), this, IDC_DEF_EDIT_OLDPWD);
	m_editOldPwd.ShowWindow(SW_SHOW);
	m_editOldPwd.SetFont(&m_pFont);

	m_StaticNewPwd.Create(L"新的密码:", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_NEWPWD);
	m_StaticNewPwd.ShowWindow(SW_SHOW);
	m_StaticNewPwd.SetFont(&m_pFont);	
	m_StaticNewPwdBK.Create(L"", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_NEWPWD_BK);
	m_StaticNewPwdBK.ShowWindow(SW_SHOW);
	m_editNewPwd.Create(ES_MULTILINE|ES_AUTOHSCROLL|ES_LEFT|WS_VISIBLE, CRect(0, 0, 100, 30), this, IDC_DEF_EDIT_NEWPWD);
	m_editNewPwd.ShowWindow(SW_SHOW);
	m_editNewPwd.SetFont(&m_pFont);

	m_StaticCheckPwd.Create(L"确认密码:", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_CHECKPWD);
	m_StaticCheckPwd.ShowWindow(SW_SHOW);
	m_StaticCheckPwd.SetFont(&m_pFont);	
	m_StaticCheckPwdBK.Create(L"", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_CHECKPWD_BK);
	m_StaticCheckPwdBK.ShowWindow(SW_SHOW);
	m_editCheckPwd.Create(ES_MULTILINE|ES_AUTOHSCROLL|ES_LEFT|WS_VISIBLE, CRect(0, 0, 100, 30), this, IDC_DEF_EDIT_CHECKPWD);
	m_editCheckPwd.ShowWindow(SW_SHOW);
	m_editCheckPwd.SetFont(&m_pFont);

	m_btnCommit.CreateButton(L"确 定", CRect(0, 0, 0, 0), this,NULL,3, IDC_DEF_BUTTON_COMMIT);
	m_btnCommit.SetTextBkgColor(Color(237,124,34),Color(200,124,50),Color(237,124,34));
	m_btnCommit.SetTextFrameColor(Color(237,124,34),Color(200,124,50),Color(237,124,34));
}

HBRUSH CDlgChangePwd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO:  在此更改 DC 的任何属性
	switch(nCtlColor)
	{

	case CTLCOLOR_STATIC:
		{

			if ((pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_OLDPWD_BK)
				||(pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_NEWPWD_BK)
				||(pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_CHECKPWD_BK))
			{
				static HBRUSH hbrh = ::CreateSolidBrush( RGB(0xe6,0xe6,0xe6) );
				pDC->SetBkColor( RGB(0xe6,0xe6,0xe6) );
				return hbrh;
			}
			else
			{
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetTextColor(RGB(0x4d,0x4d,0x4d));
			}

			static HBRUSH hBrush = ::CreateSolidBrush(RGB(0xf2,0xf0,0xeb));
			pDC->SetBkColor(RGB(0xf2,0xf0,0xeb));
			return hBrush;
		}
		break;
	default:
		break;
	}
	
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CDlgChangePwd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
		
	if (m_btnCommit.PtInButton(point))
	{
		m_btnCommit.LButtonUp();
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CDlgChangePwd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_btnCommit.PtInButton(point))
	{
		m_btnCommit.LButtonDown();
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgChangePwd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_btnCommit.PtInButton(point))
	{
		m_btnCommit.MouseHover();
	}
	else
	{
		m_btnCommit.MouseLeave();
	}

	CDialog::OnMouseMove(nFlags, point);
}

BOOL CDlgChangePwd::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		return true;
	}

	if (WM_SYSKEYDOWN == pMsg->message || WM_KEYDOWN == pMsg->message)
	{
		if ( VK_F12 == pMsg->wParam )
		{
			CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

			CIoViewBase* p = pMainFrame->FindActiveIoView();
			if (NULL != p)
			{
				pMainFrame->OnQuickTrade(2, p->GetMerchXml());
			}	
			return TRUE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgChangePwd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect;
	GetClientRect(&rect);
	
	
	CBitmap bitmap;	
	CDC MemeDc;
	
	MemeDc.CreateCompatibleDC(&dc);	
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	
	CBitmap *pOldBitmap = MemeDc.SelectObject(&bitmap);
	MemeDc.FillSolidRect(rect, RGB(0xF2, 0xF0, 0xEB));
	
	Graphics graphics(MemeDc.m_hDC);
	Pen pen(Color(0xFF, 0xE6, 0xE6, 0xE6));
	Rect rcOut(rect.left, rect.top, rect.Width() - 1, rect.Height() - 1);
	graphics.DrawRectangle(&pen, rcOut);
	
	m_btnCommit.DrawButton(&graphics);
	
	dc.BitBlt( rect.left, rect.top, rect.Width(), rect.Height(), &MemeDc, 0, 0, SRCCOPY);
	MemeDc.SelectObject(pOldBitmap);
	MemeDc.DeleteDC();
	bitmap.DeleteObject();
	
	CDialog::OnPaint();
}

void CDlgChangePwd::OnBtnCommit()
{
	if (NULL==m_pTradeLoginInfo||NULL==m_pTradeBid)
	{
		//ASSERT(0);
		return;
	}
	
	CString strOldPwd;
	m_editOldPwd.GetWindowText(strOldPwd);
	CString strNewPwd;
	m_editNewPwd.GetWindowText(strNewPwd);
	CString strCheckPwd;
	m_editCheckPwd.GetWindowText(strCheckPwd);

	CString strTip;
	if (strOldPwd.IsEmpty())
	{
		strTip = L"请输入旧的密码";
	}
	else if (strNewPwd.IsEmpty())
	{
		strTip = L"请输入新的密码";
	}
	else if (strCheckPwd.IsEmpty())
	{
		strTip = L"请输入确认密码";
	}
	else if (strNewPwd != strCheckPwd)
	{
		strTip = L"确认密码不正确";
	}
	
	if (!strTip.IsEmpty())
	{
		CDlgTip dlg;
		dlg.m_strTipMsg = strTip;
		dlg.m_eTipType = ETT_ERR;
		dlg.m_pCenterWnd = GetParent();	// 设置要居中到的窗口指针

		dlg.DoModal();
		return;
	}
	
	int nPwdType = m_DropPwdType.GetCurSel();

	CClientReqModifyPwd reqChangePwd;
	reqChangePwd.account = m_pTradeLoginInfo->StrUser;
	reqChangePwd.password = strOldPwd;
	reqChangePwd.new_password = strNewPwd;
	reqChangePwd.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
	reqChangePwd.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;
	reqChangePwd.mid_code = m_QueryUserInfoResult.mid_code;
	reqChangePwd.password_type.Format(_T("%d"),nPwdType);

	if ( !m_pTradeBid->ReqModifyPwd(reqChangePwd, strTip) )
	{
		CDlgTip dlg;
		dlg.m_strTipMsg = strTip;
		dlg.m_eTipType = ETT_ERR;
		dlg.m_pCenterWnd = GetParent();	// 设置要居中到的窗口指针

		dlg.DoModal();
	}
}

void CDlgChangePwd::OnQueryUserInfoResponse()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}
	
	m_QueryUserInfoResult = m_pTradeBid->GetCacheUserInfo();
}

void CDlgChangePwd::OnReqModifyPwdResponse()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}

	/*const CClientRespModifyPwd &respChangePwd =	*/m_pTradeBid->GetCacheReqModifyPwd();
}

