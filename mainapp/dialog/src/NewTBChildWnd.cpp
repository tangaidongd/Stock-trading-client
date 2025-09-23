// NewTBWnd.cpp : implementation file
//
#include "stdafx.h"
#include "NewTBChildWnd.h"
#include "NewTBWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INVALID_ID -1

#define IDC_CONTROL_LEFT_ID		1000001//������id
#define IDC_CONTROL_RIGHT_ID	1000002//�Ҳ����id

const int32 KTimeRecoverId				= 10000;		    // �ָ���ť״̬
const int32 KTimeRecoverPeriod			= 5000;			    // ÿ5��ָ���ť״̬

const int32 KTimeAnimationId			= 10001;		    // ����Ч��
const int32 KTimeAnimationPeriod		= 50;				// ÿ100ms���¶���Ч��

/////////////////////////////////////////////////////////////////////////////
// CNewTBChildWnd

CNewTBChildWnd::CNewTBChildWnd(CWnd *pWndParent, buttonContainerType &mapBtn):m_pWndParent(pWndParent), m_mapBtn(mapBtn)
{
	m_iXButtonHovering = INVALID_ID;
	m_iCurrentPage	   = IDC_BTN_MAIN;

	m_iMoveSpace = 0;

	m_bShowIntersect = false;
	m_bLeftDirect     = true;
	m_mapControlBtn.clear();

	m_pLeftImage = Image::FromFile(L"./image/arrow_l.png");
	assert(Ok == m_pLeftImage->GetLastStatus());
	m_pRightImage = Image::FromFile(L"./image/arrow_r.png");
	assert(Ok == m_pRightImage->GetLastStatus());
	m_pImgPermission = Image::FromFile(L"./image/permission.png");
	assert(Ok == m_pImgPermission->GetLastStatus());
}

CNewTBChildWnd::~CNewTBChildWnd()
{
	DEL(m_pLeftImage);
	DEL(m_pRightImage);
	DEL(m_pImgPermission);
}


BEGIN_MESSAGE_MAP(CNewTBChildWnd, CWnd)
	//{{AFX_MSG_MAP(CNewTBChildWnd)
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
// CNewTBChildWnd message handlers
int CNewTBChildWnd::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	int iCreate = CWnd::OnCreate(lpCreateStruct);
	if ( -1 == iCreate )
	{
		return iCreate;
	}

	return iCreate;
}

void CNewTBChildWnd::OnTimer(UINT nIDEvent)
{
	if ( KTimeRecoverId == nIDEvent)
	{	
		RecoverToolbarButtons();
		//
		KillTimer(KTimeRecoverId);
	}
	else if (KTimeAnimationId == nIDEvent)
	{
		//�����ƶ���������ť
		int iMove = (m_iMoveSpace > 0 ? 50 : -50);
		
		if (abs(m_iMoveSpace) < abs(iMove))
		{
			iMove = m_iMoveSpace;
			m_bLeftDirect = !m_bLeftDirect;
			KillTimer(KTimeAnimationId);
		}
		
		buttonContainerType::iterator it = m_mapBtn.begin();
		FOR_EACH(m_mapBtn, it)	
		{
			CRect rt;
			(*it).GetRect(rt);
			rt.left += iMove;
			rt.right = rt.right + iMove;		
			(*it).SetRect(&rt);				
		}
		//���ˢ�¹���������
		Invalidate(FALSE);

		ShowControlButton(DecideControlButtonType());

		m_iMoveSpace -= iMove;
	}
}

BOOL CNewTBChildWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    int32 iID = (int32)wParam;

	CNewTBWnd *pParent = NULL;

	if ( NULL != m_pWndParent && m_pWndParent->IsKindOf(RUNTIME_CLASS(CNewTBWnd)))
	{
		pParent = (CNewTBWnd *)m_pWndParent;
	}

	if (!pParent)
	{
		return CWnd::OnCommand(wParam, lParam);
	}

	if (lParam == 1)//�����Ҽ���Ϣ����
	{
		pParent->RbuttonProcess(iID);
		return true;
	}

    //
	pParent->OnToolbarEvent(m_mapBtn, iID, wParam, lParam);

	return CWnd::OnCommand(wParam, lParam);
}

BOOL CNewTBChildWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CNewTBChildWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
    if(m_iCurrentPage == IDC_BTN_MAIN)
    {
		int iButton = TButtonHitRect(point);
		if (m_iXButtonHovering != INVALID_ID && m_iXButtonHovering != iButton /*&& iButton != INVALID_ID*/)
		{
			if (IDC_CONTROL_RIGHT_ID == m_iXButtonHovering || IDC_CONTROL_LEFT_ID == m_iXButtonHovering)
			{				
				GetCNCButton(m_mapControlBtn, m_iXButtonHovering).MouseLeave();
				m_iXButtonHovering = INVALID_ID;		
			}
			else
			{
				GetCNCButton(m_mapBtn, m_iXButtonHovering).MouseLeave();		
				m_iXButtonHovering = INVALID_ID;		
			}
		}
       
		if(IDC_CONTROL_RIGHT_ID == iButton || IDC_CONTROL_LEFT_ID == iButton)
		{
			GetCNCButton(m_mapControlBtn, iButton).MouseHover();	
			m_iXButtonHovering = iButton;
		}
		else
		{
			if (iButton != INVALID_ID )
			{
				GetCNCButton(m_mapBtn, iButton).MouseHover();	
				m_iXButtonHovering = iButton;
			}			
		}
    }

    // ��Ӧ WM_MOUSELEAVE��Ϣ
    TRACKMOUSEEVENT csTME;
    csTME.cbSize	= sizeof (csTME);
    csTME.dwFlags	= TME_LEAVE;
    csTME.hwndTrack = m_hWnd ;		// ָ��Ҫ׷�ٵĴ��� 
    ::_TrackMouseEvent (&csTME);	// ����Windows��WM_MOUSELEAVE�¼�֧�� 

    CWnd::OnMouseMove(nFlags, point);
}

LRESULT CNewTBChildWnd::OnMouseLeave(WPARAM wParam, LPARAM lParam)       
{     
    if(m_iCurrentPage == IDC_BTN_MAIN)
    {
		if(IDC_CONTROL_RIGHT_ID == m_iXButtonHovering || IDC_CONTROL_LEFT_ID == m_iXButtonHovering)
		{
			GetCNCButton(m_mapControlBtn, m_iXButtonHovering).MouseLeave();
			m_iXButtonHovering = INVALID_ID;	
			return 0;
		}
		else
		{
			if (INVALID_ID != m_iXButtonHovering)
			{
				GetCNCButton(m_mapBtn, m_iXButtonHovering).MouseLeave();
				m_iXButtonHovering = INVALID_ID;				
			}
		}
    }

    return 0;       
}   

void CNewTBChildWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
    if(m_iCurrentPage == IDC_BTN_MAIN)
    {
        int iButton = TButtonHitRect(point);
		if(IDC_CONTROL_RIGHT_ID == iButton || IDC_CONTROL_LEFT_ID == iButton)
		{
			GetCNCButton(m_mapControlBtn, iButton).LButtonDown();			
		}
		else
		{
			if (INVALID_ID != iButton )
			{
				GetCNCButton(m_mapBtn, iButton).LButtonDown();			

				GetCNCButton(m_mapBtn, iButton).SetCheckStatus(TRUE);
				for (int i =0; i < m_mapBtn.size(); i++)
				{
					if(m_mapBtn[i].GetControlId() != iButton)
					{
						m_mapBtn[i].SetCheckStatus(FALSE);
					}
				}

				RefreshControlBtn();

				CNewTBWnd *pParent = NULL;
				if ( NULL != m_pWndParent && m_pWndParent->IsKindOf(RUNTIME_CLASS(CNewTBWnd)))
				{
					pParent = (CNewTBWnd *)m_pWndParent;
					pParent->SetFixBtnCheckStatus(-1);
				}
			}
		}
    }
}

void CNewTBChildWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
    if(m_iCurrentPage == IDC_BTN_MAIN)
    {
        int iButton = TButtonHitRect(point);
		if(IDC_CONTROL_RIGHT_ID == iButton)
		{
			MoveToolButtons(true);
			GetCNCButton(m_mapControlBtn, iButton).LButtonUp();
			return;
		}
		if (IDC_CONTROL_LEFT_ID == iButton)
		{
			MoveToolButtons(false);
			GetCNCButton(m_mapControlBtn, iButton).LButtonUp();
			return;
		}
        if (INVALID_ID != iButton)
        {
            GetCNCButton(m_mapBtn, iButton).LButtonUp();			
        }
    }
}

void CNewTBChildWnd::OnPaint() 
{
	CPaintDC dc(this);

    DrawToolbar(dc);
}

void CNewTBChildWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags, point);
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CNewTBChildWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);	

	if (nType == SIZE_MINIMIZED)
	{
		return;
	}
	
	// ��С�ı�ʱ��ȡ���������ƶ�����Ч��
	KillTimer(KTimeAnimationId);

	//�������й�����˳������m_ptToolButtonLeftInsetֵ����
	AlignToolButton();
	
	ShowControlButton(DecideControlButtonType());
	
	// ΪɶOnSizeʱ�򹤾�����ťҪ�þò�ˢ�£���ʱ�����ﴦ����
	CPaintDC dc(this);
	DrawToolbar(dc);
}

bool32 CNewTBChildWnd::InitialCurrentWSP()
{
    CString StrCurCfmName;
    CString StrName;

    // ��ť��
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

LRESULT CNewTBChildWnd::OnIdleUpdateCmdUI( WPARAM w, LPARAM l )
{
	// Ӧ�ÿ��Ǳ�־���˵����õ�ʱ��
	CFrameWnd* pTarget = (CFrameWnd*)GetOwner();
	if (pTarget == NULL || !pTarget->IsFrameWnd())
		pTarget = GetParentFrame();
	if (pTarget != NULL && (IDC_BTN_ANALYSIS == m_iCurrentPage))
	{
		InitialCurrentWSP();
	}
	
	return 0;
}

void CNewTBChildWnd::DrawToolbar( CDC& dc )
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
        // ����������
        RectF destRect;
        destRect.X = 0;
        destRect.Y = 0;
        destRect.Width  = (REAL)rcWindow.Width();
        destRect.Height = (REAL)iToolBarHeight;

		ImageAttributes  imAtt;
		imAtt.SetWrapMode(WrapModeTile);
        graphics.DrawImage(pApp->m_pConfigInfo->m_toolBarData.m_pBkImage, destRect, 0, 0, pApp->m_pConfigInfo->m_toolBarData.m_pBkImage->GetWidth(), iToolBarHeight, UnitPixel,&imAtt);
    }

    CRect rcPaint;
    dc.GetClipBox(&rcPaint);
    buttonContainerType::iterator iter;
    CRect rcControl;
	
    // ������ҳ�����������а�ť
	FOR_EACH(m_mapBtn, iter)   
    {
        CNCButton &btnControl = *iter;
        btnControl.GetRect(rcControl); 

		// ����̫Сʱ����һ���ְ�ť
		if (rcControl.right > (rcWindow.right))
		{
			//continue;  // ��������ť			
		}
	
		// �жϵ�ǰ��ť�Ƿ���Ҫ�ػ�
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;     
		}
		
		//�Ƿ���ʾ��������ť����ư�ť��������
		if (m_bShowIntersect)
		{
			buttonContainerType::iterator it = m_mapControlBtn.begin();
			CRect rt;
			FOR_EACH(m_mapControlBtn, it)			
			{
				(*it).GetRect(rt);
				if (CRect().IntersectRect(&rt, &rcControl))
				{
					break;
				}
			}
			if (it != m_mapControlBtn.end())
			{
				continue;
			}
		}

        btnControl.DrawButton(&graphics);
    }

	FOR_EACH(m_mapControlBtn, iter)	
	{
		(*iter).DrawButton(&graphics);
	}
	
    dc.BitBlt(0, 0, rcWindow.Width(), iToolBarHeight, &memDC, 0, 0, SRCCOPY);
    dc.SelectClipRgn(NULL);
    memDC.DeleteDC();
    bmp.DeleteObject();
}

int CNewTBChildWnd::TButtonHitRect( CPoint point )
{
  
	buttonContainerType::iterator iter;

	//��ȡ�����������
	CRect rtControlRegion(0, 0, 0, 0);
	
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.OffsetRect(-rcWindow.left, -rcWindow.top);

	int iLeftButtonID  = GetIntersectButton(false);	
	int iRightButtonID = GetIntersectButton(true); 

	CRect rcLeft, rcRight;
	GetCNCButton(m_mapBtn, iLeftButtonID).GetRect(rcLeft);
	GetCNCButton(m_mapBtn, iRightButtonID).GetRect(rcRight);

	rcLeft.left = 0;
	rcLeft.top  = rcWindow.top;
	rcLeft.bottom = rcWindow.bottom;
	rcRight.right = rcWindow.right ;
	rcRight.top  = rcWindow.top;
	rcRight.bottom = rcWindow.bottom;
	
	BOOL bShow = FALSE;
	if (rcLeft.PtInRect(point))
	{
		bShow = GetCNCButton(m_mapControlBtn, IDC_CONTROL_LEFT_ID).IsVisiable();
		rtControlRegion.UnionRect(&rtControlRegion, &GetControlRect(false));
	}
	else if (rcRight.PtInRect(point))
	{
		bShow = GetCNCButton(m_mapControlBtn, IDC_CONTROL_RIGHT_ID).IsVisiable();
		rtControlRegion.UnionRect(&rtControlRegion, &GetControlRect(true));
	}
	
	if (!rtControlRegion.IsRectEmpty() && rtControlRegion.PtInRect(point))
	{
		//�������ư�ť		
		FOR_EACH(m_mapControlBtn, iter)	
		{
			CNCButton &btnControl = *iter;		

			// ��point�Ƿ����ѻ��Ƶİ�ť������
			if (btnControl.PtInButton(point) && btnControl.GetCreate())
			{
				return btnControl.GetControlId();
			}
		}

		//�ڿ��������ڣ����ڿ��ư�ť��
		return INVALID_ID;
	}
	
    // ���������������а�ť
	FOR_EACH(m_mapBtn, iter)    
    {
		CRect rtTool;
        CNCButton &btnControl = *iter;
		btnControl.GetRect(rtTool);
        // ��point�Ƿ����ѻ��Ƶİ�ť������
        if (btnControl.PtInButton(point) && btnControl.GetCreate())
        {
			//�жϹ�������ť�����Ƿ�����������ص�
			if ( (CRect().IntersectRect(&rtTool, &rtControlRegion) && bShow ) )
			{
				return INVALID_ID;
			}							
            return btnControl.GetControlId();
        }
    }

    return INVALID_ID;
}

void CNewTBChildWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	TRACE("r\n");
	if(m_iCurrentPage == IDC_BTN_MAIN)
	{
		if (INVALID_ID != m_iXButtonHovering)
		{
			GetCNCButton(m_mapBtn, m_iXButtonHovering).RButtonUp();
			m_iXButtonHovering = INVALID_ID;
		}
	}
}

void CNewTBChildWnd::MoveToolButtons(bool32 bLeftDirect)
{	
	if (m_mapBtn.size() == 0 || m_mapControlBtn.size() == 0)
	{
		return;
	}	

	int iMove  =0;

	m_bLeftDirect = bLeftDirect;

 	CRect rcWindow, rcControl;
	GetWindowRect(&rcWindow);
	rcWindow.OffsetRect(-rcWindow.left, -rcWindow.top);
	
	//�����ƶ�ƫ��������
	if (bLeftDirect)
	{
		buttonContainerType::reverse_iterator iter = m_mapBtn.rbegin();
		(*iter).GetRect(rcControl);
		m_iMoveSpace = -(rcControl.right - rcWindow.right );			
	}
	else
	{	
		buttonContainerType::iterator iter = m_mapBtn.begin();
		(*iter).GetRect(rcControl);
		m_iMoveSpace = rcWindow.left-rcControl.left ;//������Ϊ��ֵ��ֻ��תΪ��ֱ����	
	}

	if ( 0 != m_iMoveSpace )
	{
		SetTimer(KTimeAnimationId, KTimeAnimationPeriod, NULL);
	}
}

//���ֹ��������Ҳ�Ŀ��ư�ť��ʽ
ECButtonType CNewTBChildWnd::DecideControlButtonType()
{
	if (m_mapBtn.size() == 0 || m_mapBtn.size() < 2)
	{
		return E_CONTROL_NONE;
	}
	
	buttonContainerType::iterator itBegin = m_mapBtn.begin();//����һ����ť
	buttonContainerType::reverse_iterator  itTail = m_mapBtn.rbegin();//�Ҳ����һ����ť
	if (itTail == m_mapBtn.rend())
	{
		//ASSERT(0);
	}

	CRect rtHead, rtTail, rtWindow;
	GetClientRect(&rtWindow);	
	(*itBegin).GetRect(rtHead);
	(*itTail).GetRect(rtTail);

	// ��Ҫ�������ƶ���ť
	int iRightPos = rtWindow.right;
	if (rtTail.right > iRightPos && m_bLeftDirect)	
	{
		return E_CONTROL_RIGHT;
	}
	
	// ��Ҫ�������ƶ���ť
	if ( rtHead.left < 0 && !m_bLeftDirect)			
	{
		return E_CONTROL_LEFT;
	}

	return E_CONTROL_NONE;
}

//���ܻ���ʾ���������ư�ť,�ƶ���ť
void CNewTBChildWnd::ShowControlButton(ECButtonType type)
{
	if (type == E_CONTROL_MIN || type == E_CONTROL_MAX)
	{
		return;
	}	
	
	if(type == E_CONTROL_NONE)
	{
		m_mapControlBtn.clear();
		return;
	}

	ASSERT(m_pLeftImage && m_pRightImage);

	CRect rtWindow,rtControl;
	GetClientRect(rtWindow);
	
	//�������м��㣬�Ҳ����������ư�ť���д�ֱ����
	int iBetweenYSpace = 0;
	int iLeftButtonW = m_pLeftImage->GetWidth();
	int iRightButtonW = m_pRightImage->GetWidth();
	int iButtonH = m_pLeftImage->GetHeight()/3;
	int iBaseTop = iBetweenYSpace/*rtWindow.top + (rtWindow.Height() - 2 * iButtonH - iBetweenYSpace) / 2*/;
	int iLeftButtonLeft  =  0;
	int iLeftButtonRight = iLeftButtonLeft + iLeftButtonW;	
	int iLeftButtonTop   = iBaseTop ;
	int iLeftButtonButtom = iLeftButtonTop + iButtonH;
	
	//
	int iRightButtonRight  = rtWindow.right ;
	int iRightButtonLeft   = iRightButtonRight - iRightButtonW;
	int iRightButtonTop    = iBaseTop;
	int iRightButtonButtom = iRightButtonTop + iButtonH;

	
	if (m_mapControlBtn.size() != 2)
	{				
		m_mapControlBtn.clear();
		
		//�������ư�ť
		CNCButton btnRight;
		rtControl.SetRect(iRightButtonLeft, iRightButtonTop, iRightButtonRight, iRightButtonButtom);
		btnRight.CreateButton(L"", rtControl, this, m_pRightImage, 3, IDC_CONTROL_RIGHT_ID);		
		btnRight.SetNeedDBuffering(false);
		btnRight.SetClientDC(TRUE);
		m_mapControlBtn.push_back(btnRight);
		CNCButton btnLeft;
		rtControl.SetRect(iLeftButtonLeft, iLeftButtonTop, iLeftButtonRight,  iLeftButtonButtom);		
		btnLeft.CreateButton(L"", rtControl, this, m_pLeftImage, 3, IDC_CONTROL_LEFT_ID);
		btnLeft.SetClientDC(TRUE);
		btnLeft.SetNeedDBuffering(false);
		m_mapControlBtn.push_back(btnLeft);
	}
	else
	{
		//�Զ�У�����ư�ťλ��
		
		GetCNCButton(m_mapControlBtn, IDC_CONTROL_LEFT_ID).GetRect(rtControl);
		GetCNCButton(m_mapControlBtn, IDC_CONTROL_LEFT_ID).SetRect(CRect(iLeftButtonLeft, iLeftButtonTop, iLeftButtonRight, iLeftButtonButtom));
		GetCNCButton(m_mapControlBtn, IDC_CONTROL_RIGHT_ID).GetRect(rtControl);
		GetCNCButton(m_mapControlBtn, IDC_CONTROL_RIGHT_ID).SetRect(CRect(iRightButtonLeft, iRightButtonTop, iRightButtonRight , iRightButtonButtom));
	}

	//���ֲ���Ϊ2�����ư�ť����£��������ֻ�Բ���Ҫ�İ�ť���н���	
	if (type == E_CONTROL_BOTH)
	{
// 		GetCNCButton(m_mapControlBtn, IDC_CONTROL_RIGHT_ID).EnableButton(true);
// 		GetCNCButton(m_mapControlBtn, IDC_CONTROL_LEFT_ID).EnableButton(true);			
	}	
	
	if (type == E_CONTROL_LEFT )
	{
		GetCNCButton(m_mapControlBtn, IDC_CONTROL_LEFT_ID).SetVisiable(TRUE);
		GetCNCButton(m_mapControlBtn, IDC_CONTROL_RIGHT_ID).SetVisiable(FALSE);		
	}

	if(type == E_CONTROL_RIGHT)
	{
		GetCNCButton(m_mapControlBtn, IDC_CONTROL_RIGHT_ID).SetVisiable(TRUE);
		GetCNCButton(m_mapControlBtn, IDC_CONTROL_LEFT_ID).SetVisiable(FALSE);
	}
}

bool32 CNewTBChildWnd::IsSubRect(const CRect& destRect, const CRect& subRect)
{
	if (subRect.left >= destRect.left && subRect.right <= destRect.right && subRect.top >= destRect.top && subRect.bottom <= destRect.bottom)
	{
		return true;
	}
	return false;

}

CRect CNewTBChildWnd::GetControlRect(bool32 bLeft/*=true*/)
{
	CRect rtRegon;
	rtRegon.SetRectEmpty();
	if (m_mapControlBtn.size() != 0)
	{
		if (bLeft)
		{
			buttonContainerType::iterator it = m_mapControlBtn.begin();
			(*it).GetRect(rtRegon);	
		}
		else
		{
			buttonContainerType::reverse_iterator it = m_mapControlBtn.rbegin();
			(*it).GetRect(rtRegon);
		}

		if (!rtRegon.IsRectEmpty() && !rtRegon.IsRectNull())
		{
			CRect rtWindow;
			GetClientRect(&rtWindow);
			rtRegon.top = rtWindow.top;
			rtRegon.bottom = rtWindow.bottom;
		}
	}	

	return rtRegon;
}


int32	CNewTBChildWnd::GetIntersectButton(bool bLeft)
{
	const int32 iKTabMaxWidth = 5;	//	ռλ��ǩ��ť����
	if (m_mapBtn.size() == 0 )
	{
		return INVALID_ID;
	}	
	if (m_mapControlBtn.size() == 0)
	{
		return INVALID_ID;
	}

	CRect rtButton, rtControl;

	if(bLeft)
	{
		//����ư�����ť����
		CRect rtControlRegion = GetControlRect();	
		if (rtControlRegion.IsRectEmpty())
		{
			return INVALID_ID;
		}			
		buttonContainerType::iterator it = m_mapBtn.begin();
		FOR_EACH(m_mapBtn, it)		
		{
			CRect rt;
			(*it).GetRect(rt);
			if (CRect().IntersectRect(&rtControlRegion, &rt))
			{
				if (rt.Width() < iKTabMaxWidth && it->GetCaption() == L"��ǩ") //	������С��iKTabMaxWidth �� ����Ϊ����ǩ�������϶�Ϊ��ǩ��ť
				{
					continue;
				}

				return (*(--it)).GetControlId();
				
			}
		}
	}
	else
	{
		//�봰����߽罻��
		buttonContainerType::reverse_iterator it = m_mapBtn.rbegin();
		FOR_EACH_REVERST(m_mapBtn, it)
		{
			CRect rt;
			(*it).GetRect(rt);
			if (rt.Width() < iKTabMaxWidth && it->GetCaption() == L"��ǩ") //	������С��iKTabMaxWidth �� ����Ϊ����ǩ�������϶�Ϊ��ǩ��ť
			{
				continue;
			}

			if (rt.left < 0 )
			{
				return (*(--it)).GetControlId();
			}
		}
		
	}
	return INVALID_ID;	
}
//������������£��������й�������ť
void  CNewTBChildWnd::AlignToolButton()
{
	if(m_mapBtn.size() == 0)
	{
		return;
	}

	buttonContainerType::iterator it = m_mapBtn.begin();
	CRect rt;
	(*it).GetRect(rt);
	if (rt.left >=0)
	{
		return;
	}
	int iMove =  - rt.left;
	FOR_EACH(m_mapBtn, it)	
	{
		(*it).GetRect(rt);
		rt.OffsetRect(iMove, 0);		
		(*it).SetRect(&rt);
	}

	m_bLeftDirect = true;
}

void CNewTBChildWnd::ChangeGroupStatus(buttonContainerType &mapBtn, int iID)
{
	int iGroupId = GetCNCButton(mapBtn, iID).GetGroupId();
	
	if (INVALID_ID != iGroupId && m_pLeftImage && 
		Ok == m_pImgPermission->GetLastStatus())
	{
		CNCButton &btnControl = GetCNCButton(m_mapBtn, iGroupId);
		btnControl.SetShowCaption(FALSE);
		Image *pImage = btnControl.GetBackupImage();

		if (!pImage)
		{
			btnControl.SetImage(m_pImgPermission, true);
			KillTimer(KTimeRecoverId);
			SetTimer(KTimeRecoverId, KTimeRecoverPeriod, NULL);
		}
	}
}

void CNewTBChildWnd::RecoverToolbarButtons()
{
	buttonContainerType::iterator iter;
	FOR_EACH(m_mapBtn, iter)   
	{
		Image *pImage = iter->GetBackupImage();
		if (pImage)
		{
			iter->SetShowCaption(TRUE);
			iter->SetImage(pImage, true);
		}

		iter->SetBackupImage(NULL); 
	}
}

void CNewTBChildWnd::SetBtnCheckStatus(bool bCheck)
{
	for (int i =0; i < m_mapBtn.size(); i++)
	{
		m_mapBtn[i].SetCheckStatus(bCheck);	
	}

	RefreshControlBtn();
}

void CNewTBChildWnd::RefreshControlBtn()
{
	if(m_mapControlBtn.size() > 0)
	{
		ECButtonType eType = DecideControlButtonType();
		CClientDC dc(this);
		Gdiplus::Graphics graphics(dc.GetSafeHdc());
		if(eType == E_CONTROL_LEFT)
		{
			GetCNCButton(m_mapControlBtn, IDC_CONTROL_LEFT_ID).DrawButton(&graphics);
		}
		else if(eType == E_CONTROL_RIGHT)
		{
			GetCNCButton(m_mapControlBtn, IDC_CONTROL_RIGHT_ID).DrawButton(&graphics);
		}	
	}
}



void CNewTBChildWnd::SetCheckStatusByName(CString strCfmName)
{
	for (int i = 0; i < m_mapBtn.size(); i++)
	{
		if (m_mapBtn.at(i).GetCaption() == strCfmName)
		{
			m_mapBtn.at(i).SetCheckStatus(TRUE);
		}
		else
		{
			m_mapBtn.at(i).SetCheckStatus(FALSE);
		}
		
	}
}

