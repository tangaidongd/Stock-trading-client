#include "stdafx.h"
#include "IoViewManager.h"
#include "IoViewKLine.h"
#include "IoViewMultiCycleKline.h"
#include "XmlShare.h"

const int32 KICycleTopSkip            = 2;                  // ��ť���뵽�������ϱ߾࣬��ť�밴ť�ļ��Ҳȡ��ֵ
const int32 KICycleTopBtnHeight       = 20;                 // ������ť�ĸ߶�
const int32 KICycleTopBtnWidth        = 70;                 // ������ť�Ŀ��

const char* KStrViewSubViewRowCount    = "RowCount";
const char* KStrViewSubViewColumnCount    = "ColumnCount";

const int32 KTimerIdAddSubRegionAfterShow = 3200;	// show������ɺ������⴦���timer

const int32	KiBorderWidth = 1;							//	�߿���

namespace
{
	#define  INVALID_ID             -1
	#define  TOP_CYCLE_NUMBER       6000
	#define  TOP_CYCLE_FOUR         6001
	#define  TOP_CYCLE_SIX          6002

    #define  ID_KLINE_VIEW_START     0x500

	#define  SPLITER_LINE_WIDTH 1
}

IMPLEMENT_DYNCREATE(CIoViewMultiCycleKline, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewMultiCycleKline, CIoViewBase)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_CREATE()
    ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)	
	ON_MESSAGE(UM_HOTKEY, OnHotKeyMsg)
	ON_WM_TIMER()
END_MESSAGE_MAP()

CIoViewMultiCycleKline::CIoViewMultiCycleKline()
{
	m_bTracking			= false;
	//m_iTopBtnHovering = INVALID_ID;
	m_iSubRowCount = 0;
	m_iSubColumnCount = 0;
	m_iActiveIoView	= 0;
	m_RectTopBar.SetRectEmpty();
	m_rtSel.SetRectEmpty();
	m_aSubIoViews.RemoveAll();
	m_clrSelBorder = RGB(0, 251, 246);
}

CIoViewMultiCycleKline::~CIoViewMultiCycleKline()
{
	RegisterHotKeyTarget(false);
	DeleteSubIoViews();
}

int CIoViewMultiCycleKline::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    int iRet = CIoViewBase::OnCreate(lpCreateStruct);
    if(-1 == iRet)
    {
        return -1;
    }

    InitialIoViewFace(this);

	//InitialTopBtns();
	
    return iRet;
}

void CIoViewMultiCycleKline::RefreshButtonStatus()
{
	int iID = INVALID_ID;
	if(4 == m_aSubIoViews.GetSize())
	{
		iID = TOP_CYCLE_FOUR;
	}
	else if(6 == m_aSubIoViews.GetSize())
	{
		iID = TOP_CYCLE_SIX;
	}

	map<int, CNCButton>::iterator iter;
	for (iter=m_mapTopBtn.begin(); iter!=m_mapTopBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		btnControl.SetCheckStatus(iID == btnControl.GetControlId());
	}
}

void CIoViewMultiCycleKline::DeleteSubIoViews()
{
	for ( int i=0; i < m_aSubIoViews.GetSize() ; i++ )
	{
		m_aSubIoViews[i]->DestroyWindow();
		delete m_aSubIoViews[i];
	}
	m_aSubIoViews.RemoveAll();
}

void CIoViewMultiCycleKline::CreateSubView()
{	
	if(m_iSubRowCount <= 0 || m_iSubColumnCount <= 0)
	{
		return;
	}

	// ����n��С���д���
	for(int i = 0; i < m_iSubRowCount; i++)
	{
		for ( int j = 0; j < m_iSubColumnCount; j++ )
		{
			CIoViewKLine *pKline = new CIoViewKLine();
			pKline->SetCenterManager(m_pAbsCenterManager);
			pKline->Create(WS_CHILD, this, CRect(0,0,0,0), ID_KLINE_VIEW_START + i * m_iSubColumnCount + j );
			pKline->m_bShowTopToolBar = TRUE;
			pKline->m_bShowTopMerchBar = FALSE;
			pKline->m_bShowIndexToolBar = FALSE;
			
			pKline->m_bShowExpBtn = FALSE;
            pKline->m_bEnableLeftYRegion = FALSE;
			pKline->m_bEnableRightYRegion = TRUE;
			pKline->InitChartHideMerchButton();
			m_aSubIoViews.Add(pKline);
		}
	}

	/*m_aSubIoViews[0]->OnIoViewActive();*/

	SetTimer(KTimerIdAddSubRegionAfterShow, 10, NULL);

	SetActiveIoView(m_iActiveIoView);
	RecalcLayout();
	Invalidate();
}

void CIoViewMultiCycleKline::OnSize( UINT nType, int cx, int cy )
{
    CIoViewBase::OnSize(nType, cx, cy);

    RecalcLayout();
    RedrawWindow();             // ��������, �����Ӵ���ˢ��ռ��̫��ʱ�䵼���Լ���ʾ��ȫ
}

void CIoViewMultiCycleKline::RecalcLayout()
{
	CRect rcClient(0,0,0,0);
	GetClientRect(rcClient);
	if(m_iSubColumnCount <= 0 || m_iSubRowCount <= 0)
	{
		return;
	}

	if (rcClient.Height()/2 != 0)
	{
		rcClient.bottom -= 1;	//	��֤��ż���߶�
	}

	if(rcClient.Width()/2 != 0)
	{
		rcClient.right -= 1;	//	��֤��ż�����
	}

	//	���㵥��ͼ��ƽ�ָ߶�,���.Ԥ���߾��ȣ�ͼ��֮��Ŀ��*2(�м��ͼ���ռһ����Ԫ���)
	//	��Ԫͼ��� = �ܸ� - 2 * ���ұ߾�� - �������� - 1��* 2 * ��Ԫ�߾�� / ����
	int32 iHeightPer = (rcClient.Height()  - 2*KiBorderWidth - ((m_iSubRowCount - 1) * 2 *KiBorderWidth))/m_iSubRowCount;
	int32 iWidthPer = (rcClient.Width() - 2*KiBorderWidth - (m_iSubRowCount - 1) * 2 * KiBorderWidth)/m_iSubColumnCount;
	CRect rc(rcClient);	
	rc.bottom = rc.top;
	rc.right = rc.left;
	
	for ( int32 i=0; i < m_iSubRowCount ; ++i )
	{
		rc.top = rc.bottom + KiBorderWidth;	
		rc.bottom = rc.top + iHeightPer;	

		rc.right = rcClient.left;
		for ( int32 j=0; j < m_iSubColumnCount ; ++j )
		{
			rc.left = rc.right + KiBorderWidth;	//	��ʼλ��Ӧ��ռ��һ���߾��
			rc.right = rc.left + iWidthPer;					

			m_aSubIoViews[i*m_iSubColumnCount+j]->MoveWindow(rc);
			
			rc.right += KiBorderWidth;	//	ĩβ��Ҫռ��һ���߾��

		}	
		rc.bottom += KiBorderWidth;	//	ĩβ��Ҫռ��һ���߾��
	}
}


void CIoViewMultiCycleKline::OnPaint()
{
	CPaintDC dc(this); // device context for painting	

	if ( GetParentGGTongViewDragFlag() || m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}

	//
	UnLockRedraw();

	if(m_iSubRowCount <= 0 || m_iSubColumnCount <= 0)
	{
		return;
	}

	// ���Ʒָ���
	CRect rcClient(0,0,0,0);
	GetClientRect(rcClient);
    CMemDC memdc(&dc, rcClient);
	COLORREF clrAxis = GetIoViewColor(ESCChartAxisLine);
	//DrawTopButton();

	memdc.FillSolidRect(&rcClient,GetIoViewColor(ESCBackground) );		//	ͼ�����ķ�϶���ɫ


	DrawViewBorder(&memdc, m_rtSel, m_clrSelBorder);
}

BOOL CIoViewMultiCycleKline::OnEraseBkgnd( CDC* pDC )
{
    return TRUE;
}

bool32 CIoViewMultiCycleKline::FromXml( TiXmlElement *pElement )
{
    if (NULL == pElement)
        return false;

    // �ж��ǲ���IoView�Ľڵ�
    const char *pcValue = pElement->Value();
    if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
        return false;

    // �ж��ǲ��������Լ����ҵ����ͼ�Ľڵ�
    const char *pcAttrValue = pElement->Attribute(GetXmlElementAttrIoViewType());
    if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(CString(pcAttrValue)) != 0)	// ���������Լ���ҵ��ڵ�
        return false;


	pcAttrValue = pElement->Attribute(KStrViewSubViewRowCount);
	if (NULL != pcAttrValue)
	{
		m_iSubRowCount = atoi(pcAttrValue);
	}

	pcAttrValue = pElement->Attribute(KStrViewSubViewColumnCount);
	if (NULL != pcAttrValue)
	{
		m_iSubColumnCount = atoi(pcAttrValue);
	}

	int32 iMarketId			= -1;
	CString StrMerchCode	= L"";

	pcAttrValue = pElement->Attribute(GetXmlElementAttrMarketId());
	if (NULL != pcAttrValue)
	{
		iMarketId = atoi(pcAttrValue);
	}

	pcAttrValue = pElement->Attribute(GetXmlElementAttrMerchCode());
	if (NULL != pcAttrValue)
	{
		StrMerchCode = pcAttrValue;
	}

	// 
	CMerch *pMerchFound = NULL;
	if (!m_pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerchFound))
	{
		pMerchFound = NULL;
	}

	DeleteSubIoViews();
	CreateSubView();
	if(pMerchFound)
	{
		// ��Ʒ�����ı�
		OnVDataMerchChanged(pMerchFound->m_MerchInfo.m_iMarketId, pMerchFound->m_MerchInfo.m_StrMerchCode, pMerchFound);
	}
	
	RefreshButtonStatus();

    // ��ȡ��ҵ����ͼ���е�����
    SetFontsFromXml(pElement);
    SetColorsFromXml(pElement);

    return true;
}

CString CIoViewMultiCycleKline::ToXml()
{
    CString StrThis;

	CString StrMarketId;
	StrMarketId.Format(L"%d", m_pMerchXml->m_MerchInfo.m_iMarketId);

    StrThis.Format(L"<%s %s=\"%s\" %s=\"%d\" %s=\"%d\" %s=\"%s\"  %s=\"%s\" ",
        CString(GetXmlElementValue()), 
        CString(GetXmlElementAttrIoViewType()), 
        CIoViewManager::GetIoViewString(this),
		CString(KStrViewSubViewRowCount),
		m_iSubRowCount,
		CString(KStrViewSubViewColumnCount),
		m_iSubColumnCount,
		CString(GetXmlElementAttrMerchCode()),
		m_pMerchXml->m_MerchInfo.m_StrMerchCode,
		CString(GetXmlElementAttrMarketId()), 
		StrMarketId
        );

    CString StrFace;
    StrFace  = SaveColorsToXml();
    StrFace += SaveFontsToXml();

    StrThis += StrFace;
    StrThis += L">\n";

    StrThis += L"</";
    StrThis += GetXmlElementValue();
    StrThis += L">\n";

    return StrThis;
}

void CIoViewMultiCycleKline::OnVDataMerchChanged( int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch )
{
	if (/*m_pMerchXml == pMerch || */NULL == pMerch)
		return;

	// �����Ʒ�����������л�����֪ͨframe����rect���ؼ���
	CMerch *pMerchOld = m_pMerchXml;

	// �޸ĵ�ǰ�鿴����Ʒ
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;

	int32 iSubViewCount = m_aSubIoViews.GetSize();

	 //�޸�����ͼ����,�������������ʾ�����ڻ��������ڣ��ֱ���
	E_NodeTimeInterval eNodeInterval = ENTIMinute;
	if(4 == iSubViewCount)         // ��ʾ30���ӣ�60���ӣ� ���ߣ� ����
	{
		// �л�����
		if ( m_aSubIoViews.GetSize() > 0 && m_aSubIoViews[0]->m_MerchParamArray.GetSize() > 0 )
		{
			for ( int32 i=0; i < iSubViewCount ; ++i )
			{
				T_MerchNodeUserData *pData = m_aSubIoViews[i]->m_MerchParamArray.GetSize() > 0 ? m_aSubIoViews[i]->m_MerchParamArray[0] : NULL;
				if ( NULL == pData )
				{
					continue;
				}
				if(0 == i)
				{
					eNodeInterval = ENTIMinute30;
				}
				else if(1 == i)
				{
					eNodeInterval = ENTIMinute60;
				}
				else if(2 == i)
				{
					eNodeInterval = ENTIDay;
				}
				else 
				{
					eNodeInterval = ENTIWeek;
				}

				pData->m_eTimeIntervalFull = eNodeInterval;

				m_aSubIoViews[i]->SetTimeInterval(*pData, eNodeInterval);
				m_aSubIoViews[i]->SetCurveTitle(pData);
				//m_aSubIoViews[i]->ReDrawAysnc();
				//m_aSubIoViews[i]->RequestViewData();		
			}
		}
	}
	else if(6 == iSubViewCount)     // ��ʾ 1���ӣ�5���ӣ�15���ӣ�30���ӣ�60���ӣ�����
	{
		// �л�����
		if ( m_aSubIoViews.GetSize() > 0 && m_aSubIoViews[0]->m_MerchParamArray.GetSize() > 0 )
		{
			for ( int32 i=0; i < iSubViewCount ; ++i )
			{
				T_MerchNodeUserData *pData = m_aSubIoViews[i]->m_MerchParamArray.GetSize() > 0 ? m_aSubIoViews[i]->m_MerchParamArray[0] : NULL;
				if ( NULL == pData )
				{
					continue;
				}
				if(0 == i)
				{
					eNodeInterval = ENTIMinute;
				}
				else if(1 == i)
				{
					eNodeInterval = ENTIMinute5;
				}
				else if(2 == i)
				{
					eNodeInterval = ENTIMinute15;
				}
				else if(3 == i) 
				{
					eNodeInterval = ENTIMinute30;
				}
				else if(4 == i) 
				{
					eNodeInterval = ENTIMinute60;
				}
				else
				{
					eNodeInterval = ENTIDay;
				}

				pData->m_eTimeIntervalFull = eNodeInterval;

				m_aSubIoViews[i]->SetTimeInterval(*pData, eNodeInterval);
				m_aSubIoViews[i]->SetCurveTitle(pData);
				//m_aSubIoViews[i]->ReDrawAysnc();
				//m_aSubIoViews[i]->RequestViewData();
			}
		}
	}

	//��ʾ����Ʒ�ı�
	for (int i=0; i < iSubViewCount; ++i )
	{
		m_aSubIoViews[i]->ShowWindow(SW_SHOW);
		m_aSubIoViews[i]->OnVDataMerchChanged(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, pMerch);
		if(m_bShowNow)
		{
			m_aSubIoViews[i]->OnVDataForceUpdate();
		}
	}

	//SetTimer(KTimerIdAddSubRegionAfterShow, 10, NULL);

}

bool32 CIoViewMultiCycleKline::GetStdMenuEnable( MSG* pMsg )
{
    return false;
}

CMerch * CIoViewMultiCycleKline::GetMerchXml()
{
	return CIoViewBase::GetMerchXml();
}

void CIoViewMultiCycleKline::OnContextMenu( CWnd* pWnd, CPoint pos )
{
	CPoint pt;
	GetCursorPos(&pt);

	CNewMenu menu;	
	menu.LoadMenu(IDR_MENU_CHG);

	CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));

	pPopMenu->RemoveMenu(_T("�Զ���1"));
	pPopMenu->RemoveMenu(_T("�Զ���2"));
	pPopMenu->RemoveMenu(_T("�Զ���3"));
	pPopMenu->RemoveMenu(_T("�Զ���4"));

	//
	pPopMenu->LoadToolBar(g_awToolBarIconIDs);
	pPopMenu->AppendODMenu(L"����", MF_STRING, IDM_IOVIEWBASE_RETURN);
	// ѡ����Ʒ
	pPopMenu->AppendODMenu(L"ѡ����Ʒ", MF_STRING, IDM_IOVIEWBASE_MERCH_CHANGE);

	pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());
	menu.DestroyMenu();
}

void CIoViewMultiCycleKline::OnLButtonDown( UINT nFlags, CPoint point )
{
	//// Top��ť����
	//int32 iButton = TTopButtonHitTest(point);
	//if (INVALID_ID != iButton)
	//{	
	//	bool32 bCheck = m_mapTopBtn[iButton].GetCheck();
	//	if (!bCheck)
	//	{
	//		m_mapTopBtn[iButton].SetCheck(TRUE);
	//	}

	//	map<int, CNCButton>::iterator iter;
	//	for (iter=m_mapTopBtn.begin(); iter!=m_mapTopBtn.end(); ++iter)
	//	{
	//		CNCButton &btn = iter->second;
	//		if (iButton != iter->first && btn.GetCheck())
	//		{
	//			btn.SetCheckStatus(FALSE);
	//		}
	//	}
	//}

	CPoint ptScreen(point);
	ClientToScreen(&ptScreen);
	for ( int32 i=0; i < m_aSubIoViews.GetSize() ; ++i )
	{
		CRect rc;
		m_aSubIoViews[i]->GetWindowRect(rc);
		ScreenToClient(&rc);
		if ( rc.PtInRect(ptScreen) )
		{
			//if ( i != m_iActiveIoView )
			{
				SetActiveIoView(i);
				m_aSubIoViews[i]->OnLButtonDown(nFlags, point);
				
				break;
			}
		}
	}

	//CIoViewBase::OnLButtonDown(nFlags, point);
}

void CIoViewMultiCycleKline::OnLButtonUp( UINT nFlags, CPoint point )
{
	// Top��ť����
	//int32 iButton = TTopButtonHitTest(point);
	//if (INVALID_ID != iButton)
	//{
	//	m_mapTopBtn[iButton].LButtonUp();
	//}
}

void CIoViewMultiCycleKline::OnMouseMove( UINT nFlags, CPoint point )
{
	// top
	//int iButton = TTopButtonHitTest(point);
	//if (iButton != m_iTopBtnHovering)
	//{
	//	if (INVALID_ID != m_iTopBtnHovering)
	//	{
	//		m_mapTopBtn[m_iTopBtnHovering].MouseLeave();
	//		m_iTopBtnHovering = INVALID_ID;
	//	}

	//	if (INVALID_ID != iButton)
	//	{	
	//		m_iTopBtnHovering = iButton;
	//		m_mapTopBtn[m_iTopBtnHovering].MouseHover();
	//	}
	//}

	////
	//if ( !m_bTracking )
	//{
	//	// ע������뿪���¼���־
	//	TRACKMOUSEEVENT tme;
	//	tme.cbSize = sizeof(tme);
	//	tme.hwndTrack = m_hWnd; 
	//	tme.dwFlags = TME_LEAVE ;
	//	tme.dwHoverTime = 1;

	//	m_bTracking = _TrackMouseEvent(&tme);		
	//}	
}

LRESULT CIoViewMultiCycleKline::OnMouseLeave( WPARAM wParam,LPARAM lParam )
{
	//m_bTracking = false;

	//if (INVALID_ID != m_iTopBtnHovering)
	//{
	//	m_mapTopBtn[m_iTopBtnHovering].MouseLeave();
	//	m_iTopBtnHovering = INVALID_ID;
	//}

	return 0;
}

void CIoViewMultiCycleKline::AddTopButton( LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption )
{
	CNCButton btnControl;
	btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
	btnControl.SetTextFrameColor(RGB(100,100,100), RGB(255,255,255), RGB(238,69,2));
	btnControl.SetTextColor(RGB(190,190,190), RGB(255,255,255), RGB(190,190,190));

	m_mapTopBtn[nID] = btnControl;
}

int CIoViewMultiCycleKline::TTopButtonHitTest( CPoint point )
{
	map<int, CNCButton>::iterator iter;

	for (iter=m_mapTopBtn.begin(); iter!=m_mapTopBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		// ��point�Ƿ����ѻ��Ƶİ�ť������
		if (btnControl.PtInButton(point))
		{
			return btnControl.GetControlId();
		}
	}

	return INVALID_ID;
}

void CIoViewMultiCycleKline::DrawTopButton()
{
	CClientDC dc(this);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	CRect rcWnd;
	GetClientRect(&rcWnd);
	rcWnd.bottom = rcWnd.top + 25;

	bmp.CreateCompatibleBitmap(&dc, rcWnd.Width(), rcWnd.Height());
	memDC.SelectObject(&bmp);
	memDC.FillSolidRect(0, 0, rcWnd.Width(), rcWnd.Height(), RGB(0,0,0));
	memDC.SetBkMode(TRANSPARENT);
	Gdiplus::Graphics graphics(memDC.GetSafeHdc());

	//OnUpdateCmdUI(GetParentFrame(), FALSE);

	CRect rcPaint;
	dc.GetClipBox(&rcPaint);
	map<int, CNCButton>::iterator iter;
	CRect rcControl;

	for (iter=m_mapTopBtn.begin(); iter!=m_mapTopBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		btnControl.GetRect(rcControl);

		if(TOP_CYCLE_NUMBER == btnControl.GetControlId())
		{
			btnControl.EnableButton(FALSE);
		}

		// �жϵ�ǰ��ť�Ƿ���Ҫ�ػ�
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}

		btnControl.DrawButton(&graphics);
	}

	//
	dc.BitBlt(0, 0, rcWnd.Width(), rcWnd.Height(), &memDC, 0, 0, SRCCOPY);
	dc.SelectClipRgn(NULL);
	memDC.DeleteDC();
	bmp.DeleteObject();
}

void CIoViewMultiCycleKline::InitialTopBtns()
{
	// ���������еİ�ť
	CRect rcBtn(KICycleTopSkip, KICycleTopSkip, KICycleTopSkip + 70, KICycleTopSkip + KICycleTopBtnHeight);    // ��Ʊ������ť����ݶ�Ϊ70
	AddTopButton(&rcBtn, NULL, 3, TOP_CYCLE_NUMBER, L"ѡ������:");

	rcBtn.left += (KICycleTopSkip + 80);
	rcBtn.right = rcBtn.left + KICycleTopBtnWidth;
	AddTopButton(&rcBtn, NULL, 3, TOP_CYCLE_FOUR, L"4����");

	rcBtn.left += (KICycleTopSkip + KICycleTopBtnWidth);
	rcBtn.right = rcBtn.left + KICycleTopBtnWidth;
	AddTopButton(&rcBtn, NULL, 3, TOP_CYCLE_SIX, L"6����");
}

BOOL CIoViewMultiCycleKline::OnCommand( WPARAM wParam, LPARAM lParam )
{
	int32 iID = (int32)wParam;

	//std::map<int, CNCButton>::iterator itHome = m_mapTopBtn.find(iID);
	//if (m_mapTopBtn.end() != itHome)
	//{	
	//	if(TOP_CYCLE_FOUR == iID)
	//	{
	//		m_iSubRowCount = 2;
	//		m_iSubColumnCount = 2;
	//		if(m_iActiveIoView > 3)
	//		{
	//			m_iActiveIoView = 0;
	//		}
	//	}
	//	else if(TOP_CYCLE_SIX == iID)
	//	{
	//		m_iSubRowCount = 2;
	//		m_iSubColumnCount = 3;
	//		if(m_iActiveIoView > 5)
	//		{
	//			m_iActiveIoView = 0;
	//		}
	//	}

	//	DeleteSubIoViews();
	//	CreateSubView();
	//	if(m_pMerchXml)
	//	{
	//		OnVDataMerchChanged(m_pMerchXml->m_MerchInfo.m_iMarketId, m_pMerchXml->m_MerchInfo.m_StrMerchCode, m_pMerchXml);
	//	}
	//	RefreshButtonStatus();
	//	SetActiveIoView(m_iActiveIoView);	
	//}

	return CWnd::OnCommand(wParam, lParam);
}

void CIoViewMultiCycleKline::SetActiveIoView( int32 iIndex )
{
	if(m_aSubIoViews.GetSize() <= 0)
	{
		return;
	}

	if ( iIndex >=0 && iIndex < m_aSubIoViews.GetSize() )
	{
		if ( m_iActiveIoView >= 0 && m_iActiveIoView < m_aSubIoViews.GetSize() )
		{
			m_aSubIoViews[m_iActiveIoView]->SetActiveFlag(false);

			CIoViewChart *pChart = DYNAMIC_DOWNCAST(CIoViewChart, m_aSubIoViews[m_iActiveIoView]);
			if ( NULL != pChart )
			{
				pChart->ReDrawAysnc();
			}
			else
			{
				m_aSubIoViews[m_iActiveIoView]->Invalidate();
			}
		}

		m_aSubIoViews[iIndex]->SetActiveFlag(IsActiveInFrame());
		m_iActiveIoView = iIndex;

		CIoViewChart *pChart = DYNAMIC_DOWNCAST(CIoViewChart, m_aSubIoViews[m_iActiveIoView]);
		if ( NULL != pChart )
		{
			pChart->ReDrawAysnc();
		}
		else
		{
			m_aSubIoViews[m_iActiveIoView]->Invalidate();
		}
	}
}

BOOL CIoViewMultiCycleKline::PreTranslateMessage( MSG* pMsg )
{
	if ( WM_MOUSEWHEEL == pMsg->message )
	{
		if(m_aSubIoViews.GetSize() <= 0)
		{
			return TRUE;
		}

		CIoViewBase* pIoViewMerchSrc = m_pIoViewMerchChangeSrc;
		if ( NULL == pIoViewMerchSrc )
		{
			pIoViewMerchSrc = this;
		}

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		
		CMerch *pMerchNext = m_aSubIoViews[0]->GetMerchXml();
		
		
		pMerchNext = pIoViewMerchSrc->GetNextMerch(pMerchNext, true);
		pFrame->OnViewMerchChangeBegin(this, pMerchNext);
		pFrame->OnViewMerchChangeEnd(this, m_pMerchXml);
		
		if ( NULL == pMerchNext )
		{
			ASSERT( 0 );
			return TRUE;
		}

		for (int  i=0; i < m_aSubIoViews.GetSize() ; ++i )
		{
			m_aSubIoViews[i]->OnVDataMerchChanged(pMerchNext->m_MerchInfo.m_iMarketId, pMerchNext->m_MerchInfo.m_StrMerchCode, pMerchNext);
			m_aSubIoViews[i]->OnVDataForceUpdate();
		}
		
	}
	if (WM_MOUSEMOVE == pMsg->message)
	{
		//	��ѯ���������ͼ
		CPoint pt = CPoint(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));
		CWnd* pFromWnd = CWnd::FromHandle(pMsg->hwnd);
		pFromWnd->ClientToScreen(&pt);
		ScreenToClient(&pt);
		CWnd* pWnd = FromPoint(pt);
		if (NULL != pWnd)
		{
			CRect rt;
			pWnd->GetWindowRect(&rt);
			ScreenToClient(&rt);
			rt.InflateRect(KiBorderWidth,KiBorderWidth,KiBorderWidth,KiBorderWidth);	//	�߿�λ������һ����Ԫ��

			if (rt != m_rtSel)
			{
				CClientDC dc(this);
				DrawViewBorder(&dc, m_rtSel, GetIoViewColor(ESCBackground));			//	�����ɵı߿�
				DrawViewBorder(&dc, rt, m_clrSelBorder);								//	�����µı߿�
				m_rtSel = rt;															//	��¼�µı߿�
			}	
		}

		// ע������뿪���¼���־
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd; 
		tme.dwFlags = TME_LEAVE ;
		tme.dwHoverTime = 100;

		m_bTracking = _TrackMouseEvent(&tme);		

	}
	if (WM_MOUSELEAVE == pMsg->message)
	{

		if (pMsg->hwnd == m_hWnd && !m_rtSel.IsRectEmpty())
		{
			CClientDC dc(this);
			DrawViewBorder(&dc, m_rtSel, GetIoViewColor(ESCBackground));			//	�����ɵı߿�
			m_rtSel.SetRectEmpty();
		}
	}

	return CIoViewBase::PreTranslateMessage(pMsg);
}

LRESULT CIoViewMultiCycleKline::OnHotKeyMsg( WPARAM w, LPARAM l )
{
	CHotKey *pHotKey = (CHotKey *)w;
	if (NULL == pHotKey)
	{
		ASSERT(0);
		return 0;
	}

	bool32 bIndexHotKey = pHotKey->m_eHotKeyType == EHKTIndex;

	if ( (pHotKey->m_eHotKeyType != EHKTMerch
		&& !bIndexHotKey) )
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), UM_HOTKEY, w, l);	// �ǹ�ע��Ϣ����mainframe����
		return 0;
	}

	CHotKey HotKey = *pHotKey;
	DEL(pHotKey);
	pHotKey = &HotKey;
	RegisterHotKeyTarget(true);	// �ٴ�ע��

	if (EHKTMerch == pHotKey->m_eHotKeyType)
	{
		CString StrHotKey ,StrName, StrMerchCode;
		int32   iMarketId;
		StrHotKey		= pHotKey->m_StrKey;
		StrMerchCode	= pHotKey->m_StrParam1;
		StrName			= pHotKey->m_StrSummary;
		iMarketId		= pHotKey->m_iParam1;

		CMerch* pMerch = NULL;

		if ( m_pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerch) )
		{
			if ( NULL == pMerch )
			{
				return 0;
			}

			CIoViewBase *pActiveIoView = GetActiveIoView();
			if ( NULL != pActiveIoView )
			{
				pActiveIoView->OnVDataMerchChanged(iMarketId, StrMerchCode, pMerch);
				pActiveIoView->OnVDataForceUpdate();
			}

			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			pFrame->OnViewMerchChangeBegin(this, pMerch);
			pFrame->OnViewMerchChangeEnd(this, pMerch);
			
		}
	}
	else if ( EHKTIndex == pHotKey->m_eHotKeyType )
	{
		// ����k����ͼ�л�ָ��
		// �˴�Ӧ����Ȩ�޿���
		if ( !CPluginFuncRight::Instance().IsUserHasIndexRight(pHotKey->m_StrParam1, true) )
		{
			return 0;
		}

		for ( int32 i=0; i < m_aSubIoViews.GetSize() ; ++i )
		{
			m_aSubIoViews[i]->AddShowIndex(pHotKey->m_StrParam1, true, false);	// ɾ�����ڵ�ָ�꣬���л���Ʒ
		}
	}

	return 1;
}

void CIoViewMultiCycleKline::OnIoViewActive()
{
	m_bActive = IsActiveInFrame();

	CIoViewBase *pActive = GetActiveIoView();
	if ( NULL != pActive )
	{
		pActive->SetActiveFlag(m_bActive);	// �����������־
		CIoViewChart *pChart = DYNAMIC_DOWNCAST(CIoViewChart, pActive);
		if ( NULL != pChart )
		{
			pChart->ReDrawAysnc();
		}
	}

	CIoViewBase::OnIoViewActive();

	if ( m_bActive )
	{
		RegisterHotKeyTarget(true) ;
	}

	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	Invalidate();

	SetFocus();	// �Լ���ȡ���������Ϣ
}

void CIoViewMultiCycleKline::OnIoViewDeactive()
{
	// ��������ͼ��ȡ����, ��Ӧdeactive
	m_bActive = false;

	CIoViewBase *pActive = GetActiveIoView();
	if ( NULL != pActive )
	{
		pActive->SetActiveFlag(false);
	}

	RegisterHotKeyTarget(false);

	Invalidate();
}

void CIoViewMultiCycleKline::OnTimer( UINT nIDEvent )
{
	if ( KTimerIdAddSubRegionAfterShow == nIDEvent )
	{
		KillTimer(nIDEvent);
		if ( NULL != m_pMerchXml )
		{

			bool32 bHasSub = false;
			try
			{
				bHasSub = m_aSubIoViews[0]->m_SubRegions.GetSize() > 0 && m_aSubIoViews[0]->m_SubRegions[0].m_pSubRegionMain->GetCurveNum()>0;
			}
			catch(...)
			{
			}
			if ( !bHasSub )
			{
				//�����ڸ�ͼ���������Ӹ�ͼ
				CString StrIndexName = L"VOL"/*CTBWndSpecial::GetMenuIdIndexName(ID_SPECIAL_STOCKTRADE)*/;

				if ( !CPluginFuncRight::Instance().IsUserHasIndexRight(StrIndexName, true) )
				{

				}
				else
				{
					for ( int32 i=0; i < m_aSubIoViews.GetSize() ; ++i )
					{
						m_aSubIoViews[i]->AddShowIndex(_T("MA"));
						if ( !m_aSubIoViews[i]->AddSubRegion(true) )
						{
							break;	// ��Ȼ��Ӳ��˸�ͼ�����ؼ�����
						}
						m_aSubIoViews[i]->AddShowIndex(StrIndexName, true, false);	// ɾ�����ڵ�ָ�꣬���л���Ʒ
					}
				}
			}
		}
	}
}

CIoViewBase    * CIoViewMultiCycleKline::GetActiveIoView()
{
	if ( m_iActiveIoView >=0 && m_iActiveIoView < m_aSubIoViews.GetSize() )
	{
		return m_aSubIoViews[m_iActiveIoView];
	}
	return NULL;
}


void CIoViewMultiCycleKline::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	switch (nChar)
	{
	case VK_UP:
		{
			CIoViewKLine* pKline = (CIoViewKLine*)GetActiveIoView();
			if(pKline)
			{
				pKline->OnKeyUp();
			}

			// K�߷���
			//for ( int32 i=0; i < m_aSubIoViews.GetSize() ; ++i )
			//{
			//	m_aSubIoViews[i]->OnKeyUp();
			//}
		}
		break;

	case VK_DOWN:
		{
			CIoViewKLine* pKline = (CIoViewKLine*)GetActiveIoView();
			if(pKline)
			{
				pKline->OnKeyDown();
			}

			// K�߷���
			//for ( int32 i=0; i < m_aSubIoViews.GetSize() ; ++i )
			//{
			//	m_aSubIoViews[i]->OnKeyDown();
			//}
		}
		break;

	default:
		break;
	}
	CIoViewBase::OnKeyDown(nChar, nRepCnt, nFlags);
}

CWnd* CIoViewMultiCycleKline::FromPoint(CPoint pt)
{
	CWnd* pWnd = NULL;
	for ( int32 i=0; i < m_aSubIoViews.GetSize() ; ++i )
	{
		CIoViewKLine* pIoview = m_aSubIoViews[i];
		if (NULL != pIoview)
		{
			CRect rt;
			pIoview->GetWindowRect(&rt);
			ScreenToClient(&rt);
			if (rt.PtInRect(pt))
			{
				pWnd = pIoview;
				break;
			}
		}
	}

	return pWnd;
}
void	CIoViewMultiCycleKline::DrawViewBorder(CDC* pDC, const CRect& rtSel, const COLORREF& clr)
{	
	if (!rtSel.IsRectEmpty())
	{
		Gdiplus::Color clrPen;
		clrPen.SetFromCOLORREF(clr);
		Gdiplus::Pen pen(clrPen, KiBorderWidth);

		Gdiplus::PointF point1(rtSel.left, rtSel.top);
		Gdiplus::PointF point2(rtSel.right, rtSel.top);
		Gdiplus::PointF point3(rtSel.right, rtSel.bottom);
		Gdiplus::PointF point4(rtSel.left, rtSel.bottom);
		Gdiplus::PointF point5(rtSel.left, rtSel.top);
		Gdiplus::PointF points[5] = {point1, point2, point3, point4, point5};		

		Gdiplus::Graphics gh(pDC->GetSafeHdc());

		gh.DrawLines(&pen, points, 5);
	}	
}