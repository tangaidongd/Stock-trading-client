// CDlgIm.cpp : implementation file
//

#include "stdafx.h"
#include <Tlhelp32.h>
#include "DlgAdvertise.h"
#include "PathFactory.h"
#include "CommentStatusBar.h"
#include "IoViewChart.h"
#include "IoViewKLine.h"
#include "MPIChildFrame.h"
#include "DlgTodayComment.h"

#define  MAXADVLENGTH 20

/////////////////////////////////////////////////////////////////////////////
// CDlgAdvertise dialog

static const int KTimerIsWindowVisibleId  = 100;
static const int KiTimerPeriodIsWindowVisible       = 1000;
static const int KTimerIsAreadlyToShowId  = 101;
static const int KiTimerPeriodIsAreadlyToShowId = 1000 * 2;


CDlgAdvertise::CDlgAdvertise(CWnd* pParent)
	: CDialogEx(CDlgAdvertise::IDD, pParent)
{
	m_pWnd        = NULL;
	m_pKLineImage = NULL;
	m_bUserReaded = false;
	m_StrIconPath = CPathFactory::GetImageKLinePath();

	m_bHaveAdve     = false;
	m_IsAllAreadly  = false;
	m_bGifFileEixst = false;
	m_bShowContent  = false;
	
	m_tKLineNewsInfo.m_iIndexID = -1;
	//
	m_StrWeb1 = "www.";
	m_StrWeb2 = "http";

	m_imageWidth = 0;
	m_imageHeight = 0;
}

CDlgAdvertise::~CDlgAdvertise()
{
	KillTimer(KTimerIsAreadlyToShowId);
	KillTimer(KTimerIsWindowVisibleId);
	
	if ( m_pKLineImage )
	{
		delete m_pKLineImage;
		m_pKLineImage = NULL;
	}

	DEL(m_pWnd);

	SaveSysConfig();
}

void CDlgAdvertise::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgAdvertise, CDialogEx)
	//{{AFX_MSG_MAP(CDlgIm)
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CDlgAdvertise::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	m_strAdvText = L"分析师";
	
	if ( NULL == m_pKLineImage)
	{
		if ( INVALID_FILE_ATTRIBUTES == GetFileAttributes(m_StrIconPath))
			return true;

 		m_pKLineImage = new ImageEx(m_StrIconPath);
 		m_imageHeight = m_pKLineImage->GetHeight();
 		m_imageWidth = m_pKLineImage->GetWidth();
 	
		CRect rt(30, 10, 0, 0);
		Color clr(255, 0, 0, 0);
 		m_pKLineImage->SetContentOnGif(L"分析师", rt, clr);
 		m_pKLineImage->InitAnimation(m_hWnd, CPoint(0,0));
	}

	SetTimer(KTimerIsWindowVisibleId, KiTimerPeriodIsWindowVisible, NULL);
	SetTimer(KTimerIsAreadlyToShowId, KiTimerPeriodIsAreadlyToShowId, NULL);

	m_bGifFileEixst = true;

	return TRUE; 
}

void CDlgAdvertise::OnLButtonUp( UINT nFlags, CPoint pt )
{
    if ( m_strAdvText != L"分析师" )
	{
		m_bShowContent = true;
		ShowKLineAdvertise();
		m_bUserReaded = true;
		ShowWindow(SW_HIDE);
	}
	SaveSysConfig();
}

//广告标题设置
void CDlgAdvertise::SetAdvText(CString strText, int iIndex)
{
	CGGTongApp * app = (CGGTongApp *)AfxGetApp();

	if( iIndex == app->m_pConfigInfo->m_iReadIndex ) 
		 return;

	m_bHaveAdve = true;

	int iSub = strText.Find(app->m_pConfigInfo->m_strSpecialJiPing, 0);
	if ( iSub < 0)
		return;

	m_strAdvText= strText.Mid(iSub + app->m_pConfigInfo->m_strSpecialJiPing.GetLength(), strText.GetLength() - iSub - app->m_pConfigInfo->m_strSpecialJiPing.GetLength());
	
	WCHAR  des[MAXADVLENGTH] = {0};
	if ( !m_strAdvText.IsEmpty() )
	{
		lstrcpyn(des,m_strAdvText, MAXADVLENGTH - 1);
	}

	if ( NULL != m_pKLineImage )
	{
		CRect rt(22, 10, m_imageWidth - 22, 30);
		Color clr(255, 0, 0, 0);
		m_pKLineImage->SetContentOnGif(des, rt, clr);
	}
	m_bUserReaded = false;

	if ( NULL != m_pWnd)
		((CIoViewChart *)m_pWnd)->RedrawAdvertise();

	app->m_pConfigInfo->m_iReadIndex = iIndex;//保存已经读过的特殊今评
}

//对话框所属者
void CDlgAdvertise::SetAdvertiseOwner( CWnd *pWnd )
{
	m_pWnd = pWnd;
}

void CDlgAdvertise::SetImageFont(LOGFONT *pLogFont)
{
	if ( NULL != m_pKLineImage )
	{
		m_pKLineImage->SetFontEx(pLogFont);
	}
}

bool  CDlgAdvertise::RedrawAdvertise()
{
	if ( !m_pWnd || !m_bHaveAdve || !m_IsAllAreadly || m_bUserReaded || !m_bGifFileEixst )
		return false;

	CRect rectPos;
	if ( m_pWnd )
	{
		((CIoViewChart *)m_pWnd)->GetAdvDlgPosition(m_imageWidth, m_imageHeight, rectPos);
		MoveWindow(rectPos);

		SetTimer(KTimerIsWindowVisibleId, KTimerIsWindowVisibleId, NULL);
	}

	return true;
}

void CDlgAdvertise::OnSize(UINT nType, int cx, int cy) 
{
	 CDialogEx::OnSize(nType, cx, cy);

	 CRect rc;
	 GetWindowRect(rc);
	 rc -= rc.TopLeft();
	 
	 CRgn rgn;
	 rgn.CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, 11, 11);
	 SetWindowRgn((HRGN)rgn.Detach(), TRUE);
}
//获取所要显示的位置
void CDlgAdvertise::GetDlgAdvertisePos(CRect &rect)
{
	if (m_pWnd)
		((CIoViewChart *)m_pWnd)->GetAdvDlgPosition(m_imageWidth, m_imageHeight, rect);
}
//是否看过广告
bool CDlgAdvertise::GetReadSTatus()
{
	return m_bUserReaded;
}

void CDlgAdvertise::SetReadStatus(bool bBeRead)
{
	m_bUserReaded = bBeRead;
}

void CDlgAdvertise::OnTimer(UINT nIDEvent)
{
	switch ( nIDEvent )
	{
		case KTimerIsAreadlyToShowId:  //满足显示条件时，才显示
			{
				if ( NULL != m_pWnd && ((CIoViewChart *)m_pWnd)->IsAlreadyToShowAdv() )
				{
					m_IsAllAreadly = true;
					((CIoViewChart *)m_pWnd)->RedrawAdvertise();
					KillTimer(KTimerIsAreadlyToShowId);
				}
			}
			 break;
		case KTimerIsWindowVisibleId: //判断视图是否已经被隐藏
			{
				if ( NULL != m_pWnd && (!m_pWnd->IsWindowVisible() || ( ((CMainFrame *)AfxGetMainWnd())->GetActiveFrame() != m_pWnd->GetParentFrame()) ) )
				{
				   ((CIoViewChart *)m_pWnd)->SetHideFlag();
					KillTimer(KTimerIsWindowVisibleId);
				}
			}
			break;
	}
	
}

void CDlgAdvertise::SetCheckShowTimer()
{
	SetTimer(KTimerIsAreadlyToShowId, KiTimerPeriodIsAreadlyToShowId, NULL);
}

void CDlgAdvertise::SaveSysConfig()
{
	CGGTongApp *app = (CGGTongApp *)AfxGetApp();
	app->m_pConfigInfo->SaveSysConfig();
}


void CDlgAdvertise::AddBatchComment(IN listNewsInfo& listTitles)
{
	CGGTongApp * ggApp = (CGGTongApp *)AfxGetApp();

	for( int i =0 ; i < listTitles.GetSize(); i++)
	{
		T_NewsInfo Temp = listTitles.GetAt(i);
		if ( !ggApp->m_pConfigInfo->m_strSpecialJiPing.IsEmpty() )
		{
			if ( Temp.m_StrContent.Find( ggApp->m_pConfigInfo->m_strSpecialJiPing ) > 0)
			{
				SetAdvText(Temp.m_StrContent, Temp.m_iIndexID);  //将特别的今评加到k线的广告对话框中
				m_tKLineNewsInfo = Temp;
				break;
			}
		}
	}
}

void CDlgAdvertise::InsertAtComment(T_NewsInfo &stFirst)
{
	CGGTongApp * ggApp = (CGGTongApp *)AfxGetApp();
	if ( stFirst.m_StrContent.Find( ggApp->m_pConfigInfo->m_strSpecialJiPing ) > 0)
	{
		SetAdvText(stFirst.m_StrContent, stFirst.m_iIndexID);  //将特别的今评加到k线的广告对话框中
		m_tKLineNewsInfo = stFirst;
	}
}

//显示K线广告内容 2014-03-08 by cym
void CDlgAdvertise::ShowKLineAdvertise()
{
	if ( m_tKLineNewsInfo.m_iIndexID < 0 )
		return;
	
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;	
	ASSERT(NULL!= pDocument);
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;

	if ( NULL == pAbsCenterManager )
	{
		m_bShowContent = false;
		return;
	}
	
	CMmiNewsReqNewsContent NewsReqNewsContent ;
	NewsReqNewsContent.m_iIndexID = m_tKLineNewsInfo.m_iIndexID;
	pAbsCenterManager->RequestNews(&NewsReqNewsContent);

}

void CDlgAdvertise::OnVDataNewsContentUpdate(const T_NewsInfo& stNewsInfo)
{
	
	if (stNewsInfo.m_StrContent.GetLength() <= 0)
	{
		CString StrErr = L"抱歉，您访问的页面已经失效。";
		AfxMessageBox(StrErr);
		m_bShowContent = false;
		return;
	}
	
	if( m_tKLineNewsInfo.m_iIndexID != stNewsInfo.m_iIndexID ||  !m_bShowContent )
		return ;
	
	CString StrContentHead = stNewsInfo.m_StrContent.Left(4);
	
	if ( 0 == StrContentHead.CompareNoCase(m_StrWeb1) || 0 == StrContentHead.CompareNoCase(m_StrWeb2) )
	{
		CDlgTodayCommentIE  dlgToday;
		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
		dlgToday.SetTitleUrl(pApp->m_strNewsInfo1, stNewsInfo.m_StrContent);
		dlgToday.DoModal();

		m_bShowContent = false;
	}
	 
}