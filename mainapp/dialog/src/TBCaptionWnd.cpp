#include "stdafx.h"
#include <algorithm>
#include "TBCaptionWnd.h"
#include "PathFactory.h"
#include "FontFactory.h"
#include "IoViewKLine.h"
#include "WspManager.h"

namespace
{
    #define ID_CAPTION_BEGIN    5000
	#define ID_CAPTION_MORE		6000

    #define BUTTON_WIDTH		110
    #define BUTTON_HEIGHT		20

    #define COLOR_NOARMAL		Color(46, 46, 46)
    #define COLOR_SELECTED		Color(78, 78, 78)
    #define COLOR_MOUSE_PASS	Color(78, 78, 78)

    #define WM_ADD_PAGE			WM_USER + 1001
	#define INVALID_VALUE       (-1)
}

HWND CNewCaptionTBWnd::m_hWndExsit = NULL;
std::vector<CTableLobby*> CNewCaptionTBWnd::m_vecTableLobby;

static const int32 s_iTopSpace  = 4;              // 按钮与背景图片上侧的间距
static const int32 s_iBtnSpace  = 6;              // 按钮之间的间距
static const int32 s_iLeftSpace = 80;              // 按钮与背景图片左侧的间距

CNewCaptionTBWnd::CNewCaptionTBWnd( CWnd &wndParent ):m_wndParent(wndParent)
{
    m_pBkImg = NULL;  
	m_pImageClose = NULL;
	m_pImgMore = NULL;
	m_pImgMenu = NULL;
	m_iRightBtnPos = INVALID_VALUE;
	m_pLobbyMore = NULL;
	m_pDlgPullDown	   = NULL;
}

CNewCaptionTBWnd::~CNewCaptionTBWnd()
{
    for(int i = 0; i < m_vecTableLobby.size(); i++)
    {
        delete m_vecTableLobby[i];
        m_vecTableLobby[i] = NULL;
    }
    m_vecTableLobby.clear();
    DEL(m_pBkImg);  
	DEL(m_pImageClose);
	DEL(m_pImgMenu);
	DEL(m_pImgMore);
	DEL(m_pLobbyMore);	
	DEL(m_pDlgPullDown);
}

BEGIN_MESSAGE_MAP(CNewCaptionTBWnd, CWnd)
    //{{AFX_MSG_MAP(CNewFunTBWnd)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_ADD_PAGE, OnAddPage)
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
    ON_WM_LBUTTONDBLCLK()
    ON_WM_LBUTTONUP()
    ON_WM_SIZE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CNewCaptionTBWnd::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    int iCreate = CWnd::OnCreate(lpCreateStruct);
    if ( -1 == iCreate )
    {
        return iCreate;
    }

	m_pImageClose   = Image::FromFile(L"image//table_close.png");	
    m_pBkImg   = Image::FromFile(L"image//BKImage.png");	
	m_pImgMore = Image::FromFile(L"image//More.png");
	m_pImgMenu = Image::FromFile(L"image//公共下拉菜单.png");
    m_hWndExsit = m_hWnd;
    m_bMouseTracking = false;

	if (NULL == m_pDlgPullDown)
	{
		m_pDlgPullDown = new CDlgPullDown(this);
		m_pDlgPullDown->Create(IDD_DIALOG_PULLDOWN, this);
	}

    return iCreate;
}

void CNewCaptionTBWnd::OnPaint()
{
    CPaintDC dc(this);
	GetButtonRect();

    DrawButtons(dc);
}

BOOL CNewCaptionTBWnd::OnEraseBkgnd( CDC* pDC )
{
    return TRUE;
}

void CNewCaptionTBWnd::DrawButtons( CDC& dc )
{
	return;
    CRect rcClient;
    GetClientRect(&rcClient);
    rcClient.bottom = rcClient.top + m_pBkImg->GetHeight();

	
    HDC hdcMem = CreateCompatibleDC(dc.GetSafeHdc());
    HBITMAP hOldBitmap;
    HBITMAP hBitmap = CreateCompatibleBitmap(dc.GetSafeHdc(), rcClient.Width(), rcClient.Height());
    hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
    /////////////////
    Gdiplus::Graphics graphics(hdcMem);
//  SolidBrush solidBrush(Color(0, 0, 0));
//  graphics.FillRectangle(&solidBrush, 0, 0, rc.Width(), rc.Height());
	
	// 版面标签背景
	RectF destRect;
	destRect.X = 0;
	destRect.Y = 0;
	destRect.Width  = rcClient.Width();
	destRect.Height = rcClient.Height();
	if(NULL != m_pBkImg)
		graphics.DrawImage(m_pBkImg, destRect, 0, 0, m_pBkImg->GetWidth()-1, m_pBkImg->GetHeight(), UnitPixel);
   
	

    CTableLobby *pLobby = NULL;
    for(int i = 0; i < m_vecTableLobby.size(); i++)
    {
		if ( i == m_iRightBtnPos)
		{
			if (NULL == m_pLobbyMore)
			{
				m_pLobbyMore = new CTableLobby;
				m_pLobbyMore->m_nID = ID_CAPTION_MORE;
				m_pLobbyMore->m_nType = ButtonMore;
				m_pLobbyMore->m_strName = L"更多";
			}
			m_pLobbyMore->m_rcRect.SetRect(rcClient.right-30 - 5,rcClient.top+1,rcClient.right - 5,30);
			pLobby = m_pLobbyMore;
		}
		else
		{
			pLobby = m_vecTableLobby[i];
		}
        Rect rc(pLobby->m_rcRect.left, pLobby->m_rcRect.top, pLobby->m_rcRect.Width(), pLobby->m_rcRect.Height());
        if(pLobby->m_nType == ButtonNormal)
        {
            SolidBrush brush(COLOR_NOARMAL);
            graphics.FillRectangle(&brush, rc);
        }
        else if(pLobby->m_nType == MosuePass)
        {
            SolidBrush brush(COLOR_MOUSE_PASS);
            graphics.FillRectangle(&brush, rc);
        }
        else if(pLobby->m_nType == ButtonSelected)
        {
            SolidBrush brush(COLOR_SELECTED);
            graphics.FillRectangle(&brush, rc);
        }

		if(ID_CAPTION_MORE == pLobby->m_nID)
		{
			graphics.DrawImage(m_pImgMore, Rect(pLobby->m_rcRect.left, pLobby->m_rcRect.top , m_pImgMore->GetWidth(), m_pImgMore->GetHeight()));
			break;
		}

        // 给每个按钮绘制一个边框
        //BYTE color = 40;
        Pen penOutline(Color(250, 29, 30, 33));
        graphics.DrawRectangle(&penOutline, rc.X, rc.Y, rc.Width, rc.Height);

        if(pLobby->m_nType == ButtonSelected)
        {
            Pen linePen(Color(238, 69, 2), 2);
            //graphics.DrawLine(&linePen, rc.X + 1, rc.Y + rc.Height - 2, rc.X + rc.Width, rc.Y + rc.Height - 2);
        }

		// 绘制关闭按钮图片
		{		
			ASSERT(NULL != m_pImageClose);
			if (NULL != m_pImageClose && NULL != pLobby)
			{
				CPoint pt;
				GetCursorPos(&pt);
				ScreenToClient(&pt);
				Image* pImage = m_pImageClose;

				CRect rtClose(pLobby->m_rcCloseRect.left,pLobby->m_rcCloseRect.top, 0, 0);
				int iHeight = pImage->GetHeight()/3;
				int iWidth  = pImage->GetWidth();
				int n = pLobby->m_rcCloseRect.Height() - iHeight;
				rtClose.top = (pLobby->m_rcCloseRect.Height() - iHeight) / 2 + 1;
				rtClose.bottom = rtClose.top + iHeight;
				rtClose.left = pLobby->m_rcCloseRect.left + ((pLobby->m_rcCloseRect.Width() - iWidth)/2+1)  ;
				rtClose.right = rtClose.left + iWidth;
				
				bool bHover = false;
				ASSERT(NULL != pImage);				
				if (rtClose.PtInRect(pt))
				{
					bHover = true;
				}	

				RectF grect;
				grect.X = (REAL)rtClose.left;
				grect.Y = (REAL)rtClose.top;
				grect.Width = (REAL)rtClose.Width();
				grect.Height = (REAL)rtClose.Height();
				graphics.DrawImage(pImage, grect, 0 , bHover ? iHeight: 0 , (REAL)pImage->GetWidth(), (REAL)pImage->GetHeight()/3, UnitPixel);	

				//Pen pen(Color::Black,1);
				//PointF pStart(pLobby->m_rcCloseRect.left, pLobby->m_rcCloseRect.top);
				//PointF pEnd(pLobby->m_rcCloseRect.left, pLobby->m_rcCloseRect.bottom);
				//graphics.DrawLine(&pen, pStart, pEnd);
			}
		}		

		/*  RectF(IN REAL x,
          IN REAL y,
          IN REAL width,
          IN REAL height)*/
        // 绘制标签的标题
        RectF rect(rc.X + 5, rc.Y, rc.Width - BUTTON_HEIGHT, rc.Height);
        SolidBrush solidText(Color(179, 180, 181));	//文字颜色
        if(pLobby->m_nType == ButtonSelected || pLobby->m_nType == MosuePass)
        {
            solidText.SetColor(Color(240, 240, 240));
        }

        FontFamily fontFamily(gFontFactory.GetExistFontName(L"微软雅黑"));	///
        Gdiplus::Font fontText(&fontFamily, 9.0f, FontStyleRegular, UnitPoint);
        StringFormat stringFormat;
        stringFormat.SetAlignment(StringAlignmentCenter);
        stringFormat.SetLineAlignment(StringAlignmentCenter);
        CString strName =  pLobby->m_strName;
        if(pLobby->m_strName.GetLength() > 8)
        {
            strName = pLobby->m_strName.Left(2) + L"...";
        }

        rect.Y += 1;
        graphics.DrawString(strName, -1, &fontText, rect, &stringFormat, &solidText);
    }
    /////////////////

    BitBlt(dc.GetSafeHdc(), 0, 0, rcClient.Width(), rcClient.Height(), hdcMem, 0, 0, SRCCOPY);
    SelectObject(dc.GetSafeHdc(), hOldBitmap);
    DeleteObject(hdcMem);
    DeleteObject(hBitmap);
}

void CNewCaptionTBWnd:: ShowMoreBtn()
{
	if (NULL == m_pDlgPullDown)
	{
		ASSERT(0);
		return;
	}

	if (m_pDlgPullDown->IsWindowVisible())
	{
		m_pDlgPullDown->ShowWindow(FALSE);
		return;
	}

	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	int32 iCnt = m_vecTableLobby.size();
	if(iCnt <= 0)
	{
		return;
	}

	int iHeight = 0;
	int iWidth = 0;
	int iBottom = 0;

	if (NULL == m_pImgMenu || Ok != m_pImgMenu->GetLastStatus())
	{
		return;
	}

	m_mapMoreBtn.clear();
	for (int i = m_iRightBtnPos ; i < iCnt; i++)
	{
		CTableLobby* pLobby = m_vecTableLobby.at(i);
		CNCButton btnMore;
		
		iWidth = m_pImgMenu->GetWidth();
		iHeight = m_pImgMenu->GetHeight()/3;

		iBottom = iHeight *(i-m_iRightBtnPos + 1) ;
		CRect rtNow(0, (i-m_iRightBtnPos) * iHeight, iWidth, iBottom);
		btnMore.CreateButton(pLobby->m_strName, rtNow, m_pDlgPullDown, m_pImgMenu, 3, pLobby->m_nID);	
		btnMore.SetTextColor(RGB(224,224,224), RGB(255,255,255), RGB(255,255,255));
		m_mapMoreBtn.push_back(btnMore);
	}
	
	CRect rcMore;
	rcMore = m_pLobbyMore->m_rcRect;

	int iX = rcMore.right - m_pImgMenu->GetWidth();
	int iY = rcMore.bottom  + 1;
	CRect rcWnd(iX, iY, iX + iWidth, iY + iBottom);
	ClientToScreen(&rcWnd);

	int iTmp = rcMore.bottom ;
	rcMore.top = iTmp ;
	rcMore.bottom = iTmp;
	ClientToScreen(&rcMore);

	m_pDlgPullDown->SetShowButtons(m_mapMoreBtn, rcMore);
	m_pDlgPullDown->MoveWindow(rcWnd);
	m_pDlgPullDown->ShowWindow(TRUE);
}

void CNewCaptionTBWnd::OnMouseMove( UINT nFlags, CPoint point )
{
    for(int i = 0; i < m_vecTableLobby.size(); i++)
    {
        if(ButtonSelected != m_vecTableLobby[i]->m_nType)
        {
            if(m_vecTableLobby[i]->m_rcRect.PtInRect(point))
            {
                m_vecTableLobby[i]->m_nType = MosuePass;
            }
            else
            {
                m_vecTableLobby[i]->m_nType = ButtonNormal;
            }
        }
    }

    //捕获鼠标离开窗口的消息
    TRACKMOUSEEVENT tme;   
    tme.cbSize=sizeof(TRACKMOUSEEVENT); //监控鼠标离开   
    tme.dwFlags=TME_LEAVE;   
    tme.hwndTrack=this->m_hWnd;  
    if(::_TrackMouseEvent(&tme))   
    {   
        m_bMouseTracking = true; //其他鼠标进入时的操作      
    }

    this->RedrawWindow(m_rcButtonRect);

    CWnd::OnMouseMove(nFlags, point);
}

LRESULT CNewCaptionTBWnd::OnMouseLeave( WPARAM, LPARAM )
{
    m_bMouseTracking = false;

    for(int i = 0; i < m_vecTableLobby.size(); i++)
    {
        if(ButtonSelected != m_vecTableLobby[i]->m_nType)
        {
            m_vecTableLobby[i]->m_nType = ButtonNormal;
        }
    }
    this->RedrawWindow(m_rcButtonRect);

    return 0;
}

void CNewCaptionTBWnd::OnLButtonUp( UINT nFlags, CPoint point )
{
	GetButtonRect();

    if(m_rcButtonRect.PtInRect(point))
    {
        for(int i = 0; i < m_vecTableLobby.size(); i++)
        {
			if (i == m_iRightBtnPos && m_pLobbyMore->m_rcRect.PtInRect(point))
			{
				ShowMoreBtn();
				break;
			}
			else if (i == m_iRightBtnPos)
			{
				break;
			}

            if(m_vecTableLobby[i]->m_rcCloseRect.PtInRect(point))
            {
                std::vector<CTableLobby*>::iterator it = m_vecTableLobby.begin() + i;
				CString strCloseCfm = (*it)->m_strName;
				if(m_vecTableLobby.size() == 1 && strCloseCfm == CCfmManager::Instance().GetUserDefaultCfm().m_StrXmlName)
				{
					goto END;
				}

                delete m_vecTableLobby[i];	// 释放
                m_vecTableLobby.erase(it);

				// 关闭当前页面
				CCfmManager::Instance().CloseCfmFrame(strCloseCfm);
				// 关闭标签重置下状态
				CLeftBarItemManager::Instance().ResetCheckStatus(strCloseCfm);
				
                if(m_vecTableLobby.size() > 0)
                {
                    for(int j = 0; j < m_vecTableLobby.size(); j++)
                    {
                        m_vecTableLobby[j]->m_nType = ButtonNormal;
                    }
                    GetButtonRect();
                    m_vecTableLobby[m_vecTableLobby.size() - 1]->m_nType = ButtonSelected;
                    ShowCfm();
                    this->RedrawWindow();
                }
                else
                {	/* 如果网页被全部关闭，则返回“首页” */
                    CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
                    pMain->OnBackToHome();
                }
                goto END;
            }

            if(m_vecTableLobby[i]->m_rcRect.PtInRect(point))
            {
                if(m_vecTableLobby[i]->m_nType == ButtonSelected)
                    goto END;
                m_vecTableLobby[i]->m_nType = ButtonSelected;
            }
            else
            {
                m_vecTableLobby[i]->m_nType = ButtonNormal;
            }
        }

        ShowCfm();

        this->RedrawWindow(m_rcButtonRect);
    }
END:	
    CWnd::OnLButtonUp(nFlags, point);
}

void CNewCaptionTBWnd::OnSize( UINT nType, int cx, int cy )
{
    CWnd::OnSize(nType, cx, cy);

    // 计算按钮的位置
	/*if(m_vecTableLobby.size() > 0)
	{
		for(int i = 0; i < m_vecTableLobby.size(); i++)
		{
			m_vecTableLobby[i]->m_nType = ButtonNormal;
		}

		GetButtonRect();
		m_vecTableLobby[m_iRightBtnPos - 1]->m_nType = ButtonSelected;
		ShowCfm();
		RedrawWindow();
	}*/
	GetButtonRect();
    //ShowCfm();
}

BOOL CNewCaptionTBWnd::PreTranslateMessage( MSG* pMsg )
{
    if(pMsg->message == WM_KEYDOWN)
    {
        if(pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
        {
            return true;
        }
    }

    return CWnd::PreTranslateMessage(pMsg);
}

bool CNewCaptionTBWnd::ShowCfm()
{
    CMainFrame *pMain = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());//(CMainFrame*)AfxGetMainWnd();
    if(pMain && pMain->m_pNewWndTB)
    {
        CTableLobby* pLobby = NULL;
        for(int i = 0; i < m_vecTableLobby.size(); i++)
        {
            //pMain->m_pNewWndTB->DoCloseCfm(m_vecTableLobby[i]->m_strName);
            if(m_vecTableLobby[i]->m_nType == ButtonSelected)
            {
                pLobby = m_vecTableLobby[i];
				break;
            }
        }

        if(pLobby)
        {
            T_CfmFileInfo cfm;
            CString StrCfmXmlName = pLobby->m_strName;
            const DWORD dwCmd = CNewTBWnd::GetIoViewCmdIdByCfmName(StrCfmXmlName);
            
			if ( !CCfmManager::Instance().QueryUserCfm(StrCfmXmlName, cfm) && dwCmd == 0 )
            {
                MessageBox(_T("页面文件缺失，请确认是否已经安装!"), _T("错误"), MB_OK |MB_ICONERROR);
                return false;
            }

            pMain->m_pNewWndTB->SetF7(FALSE);
			
			bool32 bOpen = false;
            
			if ( dwCmd != 0 )
            {
				bOpen = pMain->m_pNewWndTB->LoadSystemDefaultCfm(StrCfmXmlName);	// 系统的，不用尝试了
            }
            else
            {
				bOpen = CCfmManager::Instance().LoadCfm(StrCfmXmlName, false, false, L"") != NULL;	// 普通的
            }

			// 
			if (NULL != pMain->m_leftToolBar)
			{
				pMain->m_leftToolBar.ChangeLeftBarStatus(StrCfmXmlName);
			}

			return bOpen;
        }
    }

    return true;
}

void CNewCaptionTBWnd::GetCaptionArray(CStringArray &aStrCaptionArray)
{
	aStrCaptionArray.RemoveAll();
	for (int i = 0; i <  m_vecTableLobby.size(); ++i)
	{
		aStrCaptionArray.Add(m_vecTableLobby.at(i)->GetTableName());
	}
}

void CNewCaptionTBWnd::GetButtonRect()
{
    CRect rc;
    GetClientRect(&rc);
	bool32 bIsFind = false;

    m_rcButtonRect.left = 0;
    m_rcButtonRect.right = 0;
    m_rcButtonRect.top = rc.top;
    m_rcButtonRect.bottom = m_rcButtonRect.top + BUTTON_HEIGHT;

    CTableLobby *pLobby = NULL;
    for(int i = 0; i < m_vecTableLobby.size(); i++)
    {
        pLobby = m_vecTableLobby[i];
        pLobby->m_rcRect.left = i * BUTTON_WIDTH + rc.left/* + i*2+2*/;
        pLobby->m_rcRect.right = pLobby->m_rcRect.left + BUTTON_WIDTH;
        pLobby->m_rcRect.top = rc.top;
        pLobby->m_rcRect.bottom = pLobby->m_rcRect.top + BUTTON_HEIGHT;

        pLobby->m_rcCloseRect = pLobby->m_rcRect;
        pLobby->m_rcCloseRect.left = pLobby->m_rcRect.right - BUTTON_HEIGHT;
		if (pLobby->m_rcRect.right > rc.right - 45 && !bIsFind)
		{
			m_iRightBtnPos = i;
			bIsFind = true;
		}

        m_rcButtonRect.right = pLobby->m_rcCloseRect.right;
    }
	if (!bIsFind)
	{
		m_iRightBtnPos = INVALID_VALUE;
	}
}
void CNewCaptionTBWnd::CloseAllPage()
{
	for(int i = 0; i < m_vecTableLobby.size(); i++)
	{

		if (NULL != m_vecTableLobby[i])
		{
			CCfmManager::Instance().CloseCfmFrame(m_vecTableLobby[i]->m_strName);	
			delete m_vecTableLobby[i];
			m_vecTableLobby[i] = NULL;
		}
		
	}
	m_vecTableLobby.clear();

}
void CNewCaptionTBWnd::AddPage( const CString& strName )
{
    bool bExisted = false;

	CMainFrame *pMainFrm = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
    for(int i = 0; i < m_vecTableLobby.size(); i++)
    {
        if(m_vecTableLobby[i]->m_strName == strName && m_vecTableLobby[i]->m_nType == ButtonSelected)
            return;
        else if(m_vecTableLobby[i]->m_strName == strName)
        {
            bExisted = true;
            m_vecTableLobby[i]->m_nType = ButtonSelected;
        }
        else
        {
            m_vecTableLobby[i]->m_nType = ButtonNormal;
        }
    }

	//if (NULL != pMainFrm)
	//{
	//	pMainFrm->m_leftToolBar.ChangeLeftBarStatus(strName);
 //   }

    if(!bExisted)
    {
        int iSize = m_vecTableLobby.size();
        CTableLobby *pLobby = new CTableLobby;
        pLobby->m_nID = ID_CAPTION_BEGIN + iSize;
        pLobby->m_nType = ButtonSelected;
        pLobby->m_strName = strName;
        m_vecTableLobby.push_back(pLobby);
    }
	
	//if(m_hWndExsit && ::IsWindow(m_hWndExsit))
 //   {
 //       ::SendMessage(m_hWndExsit, WM_ADD_PAGE, bExisted, 0);
 //   }
}

LRESULT CNewCaptionTBWnd::OnAddPage( WPARAM wParam, LPARAM lParam )
{
	GetButtonRect();
	
	CTableLobby* pLobby = NULL;
	for(int i = 0; i < m_vecTableLobby.size(); i++)
	{
		if(m_vecTableLobby[i]->m_nType == ButtonSelected)
		{
			if (i >= m_iRightBtnPos && INVALID_VALUE != m_iRightBtnPos && m_iRightBtnPos >= 1)
			{
				swap(m_vecTableLobby.at(i),m_vecTableLobby.at(m_iRightBtnPos - 1));
				GetButtonRect();
			}
			break;
		}
	}
//--- wangyongxue 2016/10/31 在消息处理函数已经打开版面了，不应该重复打开
//	ShowCfm();

    this->RedrawWindow();
    return 0;
}

void CNewCaptionTBWnd::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	GetButtonRect();

    if(m_rcButtonRect.PtInRect(point))
    {
        for(int i = 0; i < m_vecTableLobby.size(); i++)
        {
            if(m_vecTableLobby[i]->m_rcRect.PtInRect(point))
            {
				
				if (i == m_iRightBtnPos)
				{
					break;
				}
	            std::vector<CTableLobby*>::iterator it = m_vecTableLobby.begin() + i;
                delete m_vecTableLobby[i];	// 释放
                m_vecTableLobby.erase(it);
                if(m_vecTableLobby.size() > 0)
                {
                    for(int j = 0; j < m_vecTableLobby.size(); j++)
                    {
                        m_vecTableLobby[j]->m_nType = ButtonNormal;
                    }
                    GetButtonRect();
                    m_vecTableLobby[m_vecTableLobby.size() - 1]->m_nType = ButtonSelected;
                    ShowCfm();
                    this->RedrawWindow();
                }
                else
                {	
                    /* 如果网页被全部关闭，则返回“首页” */
                    CString StrDefaultCfmName = CCfmManager::Instance().GetUserDefaultCfm().m_StrXmlName;
                    if ( !StrDefaultCfmName.IsEmpty() )
                    {
                        AddPage(StrDefaultCfmName);
                    }
                    CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
                    if(pMain)
                    {
                        pMain->OnBackToHome();
                    }
                }
            }
        }

        ShowCfm();

        this->RedrawWindow(m_rcButtonRect);
    }
}






BOOL CNewCaptionTBWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int32 iID = (int32)wParam;
	
	int32 iIndex = 0;
	
	if(m_vecTableLobby.size() > 0)
	{
		for(int i = 0; i < m_vecTableLobby.size(); i++)
		{
			if (iID == m_vecTableLobby.at(i)->m_nID)
			{
				iIndex = i;
			}
			m_vecTableLobby[i]->m_nType = ButtonNormal;
		}

		swap(m_vecTableLobby.at(m_iRightBtnPos -1), m_vecTableLobby.at(iIndex));
		GetButtonRect();
		m_vecTableLobby[m_iRightBtnPos - 1]->m_nType = ButtonSelected;
		ShowCfm();
		this->RedrawWindow();
	}

	return CWnd::OnCommand(wParam, lParam);
}

int32 CNewCaptionTBWnd::GetTableLobbyGetBtnId(int32 &iID)
{	
	int32 iIndex = 0;
	for (int i=m_iRightBtnPos; i<m_vecTableLobby.size(); i++)
	{
		if (iID == m_vecTableLobby.at(i)->m_nID)
		{
			iIndex = i;
			break;
		}
	}
	return iIndex;
}
