// LeftToolBar.cpp : implementation file
//
#include "stdafx.h"
#include "LeftToolBar.h"
#include "GGTong.h"
#include "PathFactory.h"
#include "WspManager.h"
#include "FontFactory.h"
#include "BlockConfig.h"
#include "IoViewDuoGuTongLie.h"
#include "StockSelectManager.h"
#include "BJSpecialStringImage.h"
#include "ToolBarIDDef.h"
#include "IoViewKLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
	#define INVALID_ID     -1
	#define BUTTON_MAX_CNT   15        // 侧边栏按钮显示的最大的个数
}


#define MARGIN     10

static const int32 s_TopBtnHight = 40;		     //  分析按钮Y轴之间的间距

static const int32 s_AnyYSpace = 14;		     //  分析按钮Y轴之间的间距
static const int32 s_AnyKiGroupSpace = 0;	 // 分组之间的间距
static const int32 s_AnaysisDistanceX = MARGIN + 1;   //分析显示的K线图等按钮距离左边框的像素
static const int32 s_AnaysisDistanceY = 15;   //分析显示的最顶端按钮距离上边框的像素

// 定时器ID
const UINT KTimerIdShowOrHide	  = 60006;
const UINT KTimerPeriodShowOrHide = 200;

// 提示文字定时器ID
const UINT KTimerIdTip	          = 60008;
const UINT KTimerPeriodTip        = 1000;




/////////////////////////////////////////////////////////////////////////////
// CLeftToolBar

CLeftToolBar::CLeftToolBar()
{
	m_eLeftBarType = ELBT_NONE;
	m_iMarketId    = 32767;
	m_pImgBk = NULL;


    m_iNumShowBtn = BUTTON_MAX_CNT;
	
	m_indexFirstShow = 0;
	m_indexLastShow = m_iNumShowBtn - 1;
	m_iBtnHeight = 0;
	m_iBtnWidth = 0;
	m_iToolBarWidth = 0;
	m_indexSelShow = -2;
	m_bSelUpDownBtn = false;

	CChooseStockState::Instance().AddChooseStateNotify(this);
}

CLeftToolBar::~CLeftToolBar()
{
	DEL(m_pImgBk);
	DEL(m_pImgBtnNormal);
	DEL(m_pImgBtnNew);
}

BEGIN_MESSAGE_MAP(CLeftToolBar, CToolBar)
	//{{AFX_MSG_MAP(CLeftToolBar)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeftToolBar message handlers

void CLeftToolBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	DrawToolButton();
	// Do not call CToolBar::OnPaint() for painting messages
}

void CLeftToolBar::DrawToolButton()
{
	CClientDC dc(this);

    CDC memDC;
    memDC.CreateCompatibleDC(&dc);
    CBitmap bmp;
    CRect rcWindow;
    GetWindowRect(&rcWindow);
    rcWindow.OffsetRect(-rcWindow.left, -rcWindow.top);
	

    bmp.CreateCompatibleBitmap(&dc, rcWindow.Width(), rcWindow.Height());
    memDC.SelectObject(&bmp);
    memDC.FillSolidRect(0, 0, rcWindow.Width(), rcWindow.Height(), RGB(26,25,30));
    memDC.SetBkMode(TRANSPARENT);
	//rcWindow.DeflateRect(MARGIN,MARGIN,0,MARGIN);

	CRect rcDest = rcWindow;
	rcDest.DeflateRect(MARGIN,MARGIN,0,MARGIN);
	



    Gdiplus::Graphics graphics(memDC.GetSafeHdc());

    //工具栏背景
    RectF destRect;
    destRect.X =rcDest.left;
    destRect.Y = rcDest.top;
    destRect.Width  = rcDest.Width();
    destRect.Height = rcDest.Height();
	graphics.DrawImage(m_pImgBk, destRect, 0, 0, m_pImgBk->GetWidth(), m_pImgBk->GetHeight() , UnitPixel);


	// 画线
	// 划线
	// 空出线的位置
	rcDest.DeflateRect(1,1,1,1);
	CBrush cbr;
	CPen cpenbr;
	cpenbr.CreatePen(PS_SOLID, 1, RGB(0,0,0)); 
	CPen *pOldPen = memDC.SelectObject(&cpenbr);
	cbr.CreateStockObject(NULL_BRUSH); //创建一个空画刷
	CBrush *pOldBr = memDC.SelectObject(&cbr);
	memDC.RoundRect(rcDest, CPoint(0, 0));
	cbr.DeleteObject();
	memDC.SelectObject(pOldBr);
	memDC.SelectObject(pOldPen);

	

    //SetBitmap((HBITMAP)m_pImgBk);
    CRect rcPaint;
    dc.GetClipBox(&rcPaint);
    map<int, CNCButton>::iterator iter;
    CRect rcControl;

    // 遍历分析工具栏上所有按钮
    for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
    {
        CNCButton &btnControl = iter->second;
        btnControl.GetRect(rcControl);

        // 判断当前按钮是否需要重绘
        if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
        {
            continue;
        }

        // 窗口太小时隐藏一部分按钮
        if (rcControl.right > (rcWindow.right))
        {
            btnControl.SetNotCreate();  // 不创建按钮
            continue;
        }

        btnControl.DrawButton(&graphics);
    }

	m_ncBtnName.DrawButton(&graphics);

	Pen pen(Color(0, 0, 0), 1);
	PointF L_PTTopLeft(rcDest.left, rcDest.top + s_TopBtnHight);  
	PointF L_PTTopRight(rcDest.right - 1, rcDest.top + s_TopBtnHight);  
	graphics.DrawLine(&pen, L_PTTopLeft, L_PTTopRight);

    dc.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &memDC, 0, 0, SRCCOPY);
    dc.SelectClipRgn(NULL);
    memDC.DeleteDC();
    bmp.DeleteObject();
}

void CLeftToolBar::SetMarkedId(int32 iMarketID)
{
	m_iMarketId = iMarketID;
}

void CLeftToolBar::ChangeMarketId()
{
	CMainFrame *pMainFrm = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

	CIoViewBase* pIoView =pMainFrm->FindActiveIoView();
	if ( NULL == pIoView )
	{
		return ;
	}
	if (NULL != pIoView->m_pMerchXml)
	{
		m_iMarketId = pIoView->m_pMerchXml->m_MerchInfo.m_iMarketId;
	}
}

bool32 CLeftToolBar:: ChangeLeftBar(const CString &StrCfmName)
{
	E_LeftBarType eType = ELBT_NONE;
	
	//if(L"沪深A股" == StrCfmName ||  L"综合排名" == StrCfmName  ||  L"资金解密" == StrCfmName )			    // 沪深A股
	//{
	//	eType = ELBT_StockCN;	
	//}
	//else if(L"港股" == StrCfmName)										// 港股
	//{
	//	eType = ELBT_StockHK;     
	//}
	//else if (L"期货" == StrCfmName)										// 期货
	//{
	//	eType = ELBT_Futures;				
	//}
	//else if (L"全球指数" == StrCfmName)										// 现货	
	//{
	//	eType = ELBT_GlobalIndex;                             
	//}
	//else if ( L"外汇市场" == StrCfmName )						    // 全球指数
	//{									 
	//	eType = ELBT_QuoteForeign;                          
	//}	
	//else if ( L"智能选股" == StrCfmName )
	//{
	//	eType = ELBT_SmartSelStock;
	//}
	//else if ( L"题材热点" == StrCfmName )
	//{									 
	//	eType = ELBT_SmartSelStock1;	 
	//}									 
	//else if ( L"跟庄一号" == StrCfmName )
	//{									 
	//	eType = ELBT_SmartSelStock2;	 
	//}
	//else if ( L"CCTV金股" == StrCfmName )
	//{
	//	eType = ELBT_SmartSelStock3;
	//}
	//else if (L"个股决策" == StrCfmName || L"王牌狙击" == StrCfmName 
	//	|| L"决胜金叉" == StrCfmName || L"海底捞月" == StrCfmName
	//	|| L"钱坤挪移" == StrCfmName || L"波段擒牛" == StrCfmName
	//	|| L"猎杀主力" == StrCfmName)
	//{
	//	eType = ELBT_StockDecision;
	//}
	//else if (L"多股同列" == StrCfmName)
	//{

	//	CMainFrame *pMainFrm = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	//	CIoViewDuoGuTongLie *pIoViewDuoGuTongLie = pMainFrm->FindIoViewDuoGuTongLie(true);
	//	if ( pIoViewDuoGuTongLie )
	//	{
	//		CMerch* pMerch = pIoViewDuoGuTongLie->GetFirstMerch();//GetMarketFirstMerch();

	//		if (NULL == pMerch)
	//		{
	//			return FALSE;
	//		}

	//		ChangeDuoGuTongLieMerch(pMerch);

	//		E_ReportType eReportType = GetMerchKind(pMerch);
	//		if (ERTStockCn == eReportType)
	//		{
	//			eType = ELBT_StockCN;
	//		}
	//		else if (ERTStockHk == eReportType)
	//		{
	//			eType = ELBT_StockHK;
	//		}
	//		else if (ERTFuturesCn == eReportType
	//			|| ERTFuturesForeign == eReportType
	//			|| ERTFutureRelaMonth == eReportType
	//			|| ERTMony == eReportType
	//			|| ERTExpForeign == eReportType 
	//			|| ERTFutureGold == eReportType)
	//		{
	//			eType = ELBT_Futures;
	//		}
	//		else if (ERTExp == eReportType || ERTExpForeign == eReportType)
	//		{
	//			eType = ELBT_GlobalIndex;
	//		}
	//	}
	//}


	if ( L"智能选股" == StrCfmName )
	{
		eType = ELBT_SmartSelStock;
	}

	if(eType != m_eLeftBarType)
	{
		SetPageType(eType);
	}	

	return ELBT_NONE != eType;
}

void CLeftToolBar::ChangeDuoGuTongLieMerch(CMerch *pMerch)
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	if(NULL == pMain || NULL == pMerch)
	{
		return;
	}

	CIoViewDuoGuTongLie *pIoViewDuoGuTongLie = NULL;
	CMPIChildFrame *pActiveFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pMain->MDIGetActive());
	if (NULL != pActiveFrame)
	{
		if( L"多股同列" == pActiveFrame->GetIdString())
		{
			pIoViewDuoGuTongLie = pMain->FindIoViewDuoGuTongLie(true);

			if(pIoViewDuoGuTongLie)
			{
				pIoViewDuoGuTongLie->RefreshView(pMerch);
			}
		}
	}
}

void CLeftToolBar::OnSize( UINT nType, int cx, int cy )
{
	CWnd::OnSize(nType, cx, cy);
	ReLayout();
}

/////////////////////////////////////////////////////
//===================CODE CLEAN======================
//////////////////////////////////////////////////////
//void CLeftToolBar::GetSmartSelStockData(OUT vector<T_ButtonItem> &vecItem)
//{
//	CMainFrame* pMainFrm = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
//	if (!pMainFrm)
//	{
//		return;
//	}
//
//	vecItem.clear();
//	int iSelectModeId  =  pMainFrm->m_iSlectModelId;
//	if (iSelectModeId != INVALID_ID)
//	{
//		if (pMainFrm->m_mapPickModelTypeGroup.count(iSelectModeId) > 0)
//		{
//			map<int32,T_SmartStockPickItem>::iterator iter;		
//			for (iter = pMainFrm->m_mapPickModelTypeGroup[iSelectModeId].begin(); iter != pMainFrm->m_mapPickModelTypeGroup[iSelectModeId].end(); ++iter)
//			{
//				T_SmartStockPickItem &stSmartStock = iter->second;
//				bool hasStockNum = false;
//				if (stSmartStock.m_ihasSmartStock > 0)
//				{
//					hasStockNum = true;
//				}
//
//				T_ButtonItem stButtonItem(stSmartStock.m_iModeId, stSmartStock.m_strModeName, "", true, stSmartStock.m_strModeDetail, hasStockNum);
//				vecItem.push_back(stButtonItem);
//			}
//		}
//	}
//	
//}

void CLeftToolBar::InitialToolBarBtns(E_LeftBarType eType)
{
	vector<T_ButtonItem> vecItem; 
	Image	*pImg = NULL;

	CLeftBarItemManager::Instance().GetItemsByType(eType, vecItem);
	/////////////////////////////////////////////////////
	//===================CODE CLEAN======================
	//////////////////////////////////////////////////////
	/*if (ELBT_SmartSelStock == eType)
	{
		GetSmartSelStockData(vecItem);
	}*/
	int32 iCnt = vecItem.size() > BUTTON_MAX_CNT? BUTTON_MAX_CNT:vecItem.size();
	if(iCnt <= 0)
	{
		return;
	}
	
    m_iBtnHeight = m_pImgBtnNormal->GetHeight() / 3;  //  所有侧边栏按钮宽高一致，取第一个的高
	m_iBtnWidth =  m_pImgBtnNormal->GetWidth();

	m_mapBtn.clear();
    int32 iSkip		= s_AnyYSpace;
    int32 iWidth	= 0;
    int32 iHeight	= 0;
    int32 iTopPos	= s_AnaysisDistanceY;
    int32 iLeftPos	= (m_iToolBarWidth - m_iBtnWidth)/2 ;

	// 添加侧边栏按钮
	iTopPos += iSkip;
	//iLeftPos = 0 /*(m_iToolBarWidth - m_iBtnWidth) / 2*/;

	m_indexFirstShow = 0;
	if ((iCnt<BUTTON_MAX_CNT) && (0<iCnt))
	{
		m_indexLastShow = iCnt-1;
		m_iNumShowBtn = iCnt;
	}
	else if (0 == iCnt)
	{
		m_indexFirstShow = -1;
		m_indexLastShow = 0;
		m_iNumShowBtn = 0;
	}
	else
	{
		m_iNumShowBtn = BUTTON_MAX_CNT;
		m_indexLastShow = m_iNumShowBtn-1;
	}

	for (int32 i=0; i<iCnt; i++)
	{
		T_ButtonItem stItem = vecItem[i];
		
		if (stItem.m_bHasResult)
		{
			pImg = m_pImgBtnNew;
		}
		else
		{
			pImg = m_pImgBtnNormal;
		}
	
		if(NULL != pImg)
		{
			iWidth = pImg->GetWidth();
			iHeight = pImg->GetHeight() / 3;

			CRect rcBtn(iLeftPos, iTopPos, iLeftPos+iWidth, iTopPos+iHeight);
			AddToolButton(&rcBtn, pImg, 3, stItem.m_iBtnID, stItem.m_StrName, stItem.m_StrDesc);
			iTopPos += (iHeight + iSkip);
		}	
	}
	

	m_ncBtnName.CreateButton(L"智能选股", CRect(0,0,0,0), this, NULL, 1, 59784);
	m_ncBtnName.SetTextColor(RGB(220,220,220), RGB(220,220,220), RGB(220,220,220));
	m_ncBtnName.SetTextBkgColor(RGB(42,42,50), RGB(42,42,50),RGB(42,42,50));
	m_ncBtnName.SetTextFrameColor(RGB(42,42,50), RGB(42,42,50),RGB(42,42,50));
	CPoint point(-6, 3);
	m_ncBtnName.SetTextOffPos(point);

	CNCButton::T_NcFont m_Font;
	m_Font.m_StrName = gFontFactory.GetExistFontName(L"宋体");
	m_Font.m_Size	 = 10.5;
	m_Font.m_iStyle	 = FontStyleRegular;	
	m_ncBtnName.SetFont(m_Font);
	m_ncBtnName.SetClientDC(TRUE);

//	ReLayout();
}

void CLeftToolBar::ReLayout()
{
	CRect rcWnd;
	GetWindowRect(&rcWnd);
	rcWnd.OffsetRect(-rcWnd.left, -rcWnd.top);
	rcWnd.DeflateRect(MARGIN,MARGIN,0,MARGIN);
	m_iToolBarWidth = rcWnd.Width();
	//rcWnd.DeflateRect(1,1,1,1);
	if (rcWnd.Height()<1 || m_iBtnHeight <= 0)
	{
		return;
	}
	int32 iBtnSize = m_mapBtn.size();
	int32 iMin = iBtnSize>BUTTON_MAX_CNT?BUTTON_MAX_CNT:iBtnSize;

	int32 iBtnLeft = (m_iToolBarWidth-m_iBtnWidth)/2;
	int32 iBtnRight = iBtnLeft + m_iBtnWidth;
	int32 iLeftBarYPos = 20;
	iLeftBarYPos += s_TopBtnHight;

	// 设置标签位置
	CRect rcBtn(rcWnd.left + 2, rcWnd.top + 2, rcWnd.left + 100, rcWnd.top + s_TopBtnHight);
	m_ncBtnName.SetRect(rcBtn);


	// 最大可显示侧边栏按钮的个数
	int32 iMaxCnt =(rcWnd.Height() - s_AnyYSpace - s_TopBtnHight)/(m_iBtnHeight + s_AnyYSpace);
	if (iMaxCnt > iBtnSize)
	{
		iMaxCnt = iBtnSize;
	}

	m_iNumShowBtn = iMaxCnt;
    m_indexLastShow = m_indexFirstShow + m_iNumShowBtn-1;
	if (m_indexLastShow >= iBtnSize)
	{
		m_indexLastShow = iBtnSize - 1;
		m_indexFirstShow = m_indexLastShow - m_iNumShowBtn + 1;
	}

	// 侧边栏按钮
	int32 index = 0;
	CRect rtLeftBar(rcWnd);
	rtLeftBar.left = rtLeftBar.left + iBtnLeft;
	rtLeftBar.right = rtLeftBar.left + m_iBtnWidth;
	rtLeftBar.top += rtLeftBar.top + iLeftBarYPos;
	rtLeftBar.bottom = rtLeftBar.top + m_iBtnHeight;
	//CRect rtLeftBar(rcWnd.left + iBtnLeft, iLeftBarYPos, iBtnRight, iLeftBarYPos+m_iBtnHeight);
	map<int, CNCButton>::iterator iter;
	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		if (index < m_indexFirstShow || index > m_indexLastShow)
		{
			CRect rtTmp(0,0,0,0);
			btnControl.SetRect(&rtTmp);
		}
		else
		{
			btnControl.SetRect(&rtLeftBar);

			rtLeftBar.top = rtLeftBar.bottom + s_AnyYSpace;
			rtLeftBar.bottom = rtLeftBar.top + m_iBtnHeight;
		}

		index++;
	}
}

bool32 CLeftToolBar::IsCurrWspMarket(int32 &iMarketId, CString StrWspName)
{
// 	if(ELBT_HUSHENSTOCK == m_eLeftBarType)
// 	{
// 		std::map<int32, E_LeftBarType>::iterator itType = m_LeftBarItemManager.m_mapMarketList.find(iMarketId);
// 		if (m_LeftBarItemManager.m_mapMarketList.end() != itType || L"多股同列" == StrWspName
// 			|| L"综合排名" == StrWspName || L"资金排名" == StrWspName )
// 		{
// 			return true;
// 		}
// 		else
// 		{
// 			return false;
// 		}
// 	}
// 	else
// 	{
// 		return true;
// 	}
	return true;
}

bool32 CLeftToolBar::IsBtnInSameType(int32 iBtnId1, int32 iBtnId2)
{
	std::map<int, CNCButton>::iterator iter1 = m_mapBtn.find(iBtnId1);
	std::map<int, CNCButton>::iterator iter2 = m_mapBtn.find(iBtnId2);
	bool32 bIsInSameWsp = (m_mapBtn.end() != iter1) && (m_mapBtn.end() != iter2);
	
	return bIsInSameWsp;
}

/************************************************************************/
/* 函数名称：TButtonHitTest                                             */
/* 功能描述：获取当前鼠标所在的按钮										*/
/* 输入参数：point,当前鼠标位置											*/
/* 返回值：  按钮ID														*/
/************************************************************************/
int CLeftToolBar::TButtonHitTest(CPoint point)
{
    map<int, CNCButton>::iterator iter;
	int iBtnId = INVALID_ID;
    // 遍历工具栏上所有按钮
    for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
    {
        CNCButton &btnControl = iter->second;
		iBtnId   = btnControl.GetControlId();
        // 点point是否在已绘制的按钮区域内
        if (btnControl.PtInButton(point) && btnControl.GetCreate())
        {
            return  iBtnId;
        }
    }

    return INVALID_ID;
}

void CLeftToolBar::AddToolButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption, LPCTSTR lpszDecription)
{
    ASSERT(pImage);
    CNCButton btnControl;
    btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
    btnControl.SetTextColor(RGB(220,220,220), RGB(255, 255, 255), RGB(255, 255, 255));
	CPoint point(0, 0);
	btnControl.SetTextOffPos(point);

	CNCButton::T_NcFont m_Font;
	m_Font.m_StrName = gFontFactory.GetExistFontName(L"宋体");
	m_Font.m_Size	 = 10.5;
	m_Font.m_iStyle	 = FontStyleRegular;	
	btnControl.SetFont(m_Font);
	btnControl.SetClientDC(TRUE);
	btnControl.SetDescription(lpszDecription);

    m_mapBtn[nID] = btnControl;
}

void CLeftToolBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	// 改变光标形状
	//int iButton = TButtonHitTest(point);
	//if (INVALID_ID!=iButton || m_UpBtn.PtInButton(point) || m_DwnBtn.PtInButton(point))
	//{
	//	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND))); 	
	//}
	if (m_PointLast != point)
	{
		m_PointLast = point;
		m_TipWnd.Hide();
		SetTimer(KTimerIdTip, KTimerPeriodTip, NULL);
	}
	

	CToolBar::OnMouseMove(nFlags, point);
}

int CLeftToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;

    m_pImgBk = Image::FromFile(L".//image//leftbar//leftBk.png");	
	m_pImgBtnNormal = Image::FromFile(L"./image/leftbar/toolbar.png");			
	m_pImgBtnNew = Image::FromFile(L"./image/leftbar/toolbarNew.png");             

	m_iToolBarWidth = m_pImgBk->GetWidth();

    InitialButtonHoveringParam();
	
	m_TipWnd.Create(this);

// 	SetTimer(KTimerIdShowOrHide, KTimerPeriodShowOrHide, NULL);
	return 0;
}

void CLeftToolBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
    int iButton = 0;
    iButton = TButtonHitTest(point);

    if (INVALID_ID != iButton)
	{
		bool32 bCheck = m_mapBtn[iButton].GetCheck();
		if (!bCheck)
		{
			m_mapBtn[iButton].SetCheck(TRUE);
		}

		SetSwitchType(EST_BTN);
		m_mapButtonHovering[m_eLeftBarType] = iButton;

		SetCheckStatus(iButton);
	}
	
	m_TipWnd.Hide();

	CToolBar::OnLButtonDown(nFlags, point);
}

BOOL CLeftToolBar::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return 1;
//	return CToolBar::OnEraseBkgnd(pDC);
}

BOOL CLeftToolBar::OnStockCNCommand( WPARAM wParam, LPARAM lParam )
{
	int32 iID = (int32)wParam;
	
	switch(iID)
	{
	case EBID_StockCN_HQBJ:
	case EBID_StockCN_JBCW:
		{
			CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
			CMainFrame* pMainFrm = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
			CString StrWspName = _T("沪深A股");
			
			// 设置状态
			CLeftBarItemManager::Instance().ChangeCheckStatus(ELBT_StockCN, iID, StrWspName);

			CIoViewReport * pIoViewReport = pMainFrm->FindIoViewReport(true);
			
			if(NULL == pIoViewReport)
			{
				DoOpenCfm(StrWspName);
				pIoViewReport = pMainFrm->FindIoViewReport(true);
				if (NULL == pIoViewReport)
				{
					break;
				}
			}
			if (EBID_StockCN_HQBJ == iID)
			{
				::PostMessage(pIoViewReport->m_hWnd, UM_IoViewReport_Finance, (WPARAM)FALSE, (LPARAM)0);
			} 
			else
			{
				::PostMessage(pIoViewReport->m_hWnd, UM_IoViewReport_Finance, (WPARAM)TRUE, (LPARAM)0);
			}
		}
		break;
	case EBID_StockCN_ZHPM:
		{
			DoOpenCfm(L"综合排名");
		}
		break;
	case EBID_StockCN_DGTL:
		{
			ShowDuoGuTongLie();
		}	
		break;
	case EBID_StockCN_ZJJM:
		{
			DoOpenCfm(L"资金解密");
		}
		break;
	default:
		break;
	}
	
	return CWnd::OnCommand(wParam, lParam);
}

BOOL CLeftToolBar::OnStockHKCommand(WPARAM wParam, LPARAM lParam)
{
	int32 iID = (int32)wParam;
	
	switch(iID)
	{
	case EBID_StockHK_HQBJ:
		{
			DoOpenCfm(L"港股");
		}
		break;
	case EBID_StockHK_DGTL:
		{
			ShowDuoGuTongLie();
		}
		break;
	default:
		break;
	}

	return CWnd::OnCommand(wParam, lParam);
}

BOOL CLeftToolBar::OnFuturesCommand(WPARAM wParam, LPARAM lParam)
{
	int32 iID = (int32)wParam;

	switch(iID)
	{
	case EBID_Future_HQBJ:
		{
			DoOpenCfm(L"期货");
		}
		break;
	case EBID_Future_DGTL:
		{
			ShowDuoGuTongLie();
		}
		break;
	default:
		break;
	}

	return CWnd::OnCommand(wParam, lParam);
}

BOOL CLeftToolBar::OnGlobalIndexCommand(WPARAM wParam, LPARAM lParam)
{
	int32 iID = (int32)wParam;

	switch(iID)
	{
	case EBID_GlobalIndex_HQBJ:
		{
			DoOpenCfm(L"全球指数");
		}
		break;
	case EBID_GlobalIndex_DGTL:
		{
			ShowDuoGuTongLie();
		}
		break;
	default:
		break;
	}

	return CWnd::OnCommand(wParam, lParam);
}

BOOL CLeftToolBar::OnQuoteForeignCommand(WPARAM wParam, LPARAM lParam)
{
	int32 iID = (int32)wParam;

	switch(iID)
	{
	case EBID_QuoteForeign_HQBJ:
		{
			DoOpenCfm(L"外汇市场");
		}
		break;
	case EBID_QuoteForeign_DGTL:
		{
			ShowDuoGuTongLie();
		}
		break;
	default:
		break;
	}
	
	return CWnd::OnCommand(wParam, lParam);
}

BOOL CLeftToolBar::OnSmartSelStockCommand(WPARAM wParam, LPARAM lParam)
{
	int32 iID = (int32)wParam;
	E_LeftBarType eType = (E_LeftBarType)lParam;

	CMainFrame *pMainFrm = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CString StrCfmName;
	if (ELBT_SmartSelStock2 == eType )
	{
		StrCfmName = L"跟庄一号";
	}
	else if (ELBT_SmartSelStock3 == eType )
	{
		StrCfmName = L"CCTV金股";
	}
	else if(ELBT_SmartSelStock == eType)
	{
		StrCfmName = L"智能选股";
	}
	else
	{
		StrCfmName = L"题材热点";
	}

	// 加载智能选股界面
	pMainFrm->LoadSpecialCfm(StrCfmName);
	
	switch(iID)
	{
	case EBID_SmartSelStock_YJFX:						// 一剑飞仙
	case EBID_SmartSelStock_QYZS:						// 青云直上
	case EBID_SmartSelStock_SLXS:						// 神龙吸水
	case EBID_SmartSelStock_SLQF:		                // 双龙齐飞
	case EBID_SmartSelStock_HFFL:						// 回风拂柳
	case EBID_SmartSelStock_SHJD:						// 三花聚顶
	case EBID_SmartSelStock_SLNT:		                // 升龙逆天
	case EBID_SmartSelStock_XZDF:                       // 吸庄大发
		{
			//int iSlectId = 0;
			//if(iID == EBID_SmartSelStock_QYZS)            // 主力拉升
			//{
			//	iSlectId = ID_SPECIAL_QZHQ;
			//}
			//else if(iID == EBID_SmartSelStock_SLXS)       // 神龙吸水
			//{
			//	iSlectId = ID_SPECIAL_SLXS;
			//}
			//else if(iID == EBID_SmartSelStock_SLQF)       // 双龙齐飞
			//{
			//	iSlectId = ID_SPECIAL_SLQF;
			//}
			//else if(iID == EBID_SmartSelStock_SLNT)       // 逆势强龙
			//{
			//	iSlectId = ID_SPECIAL_NSQL;
			//}
			//else if(iID == EBID_SmartSelStock_HFFL)       // 回风拂柳
			//{
			//	iSlectId = ID_SPECIAL_HFFL;
			//}
			//else if(iID == EBID_SmartSelStock_YJFX)       // 一剑飞仙
			//{
			//	iSlectId = ID_SPECIAL_DXQS;
			//}						
			//else if (iID == EBID_SmartSelStock_SHJD)	  // 三花聚顶-CCTV金股
			//{
			//	iSlectId = ID_SPECIAL_SHJD;
			//}
			//else if (iID == EBID_SmartSelStock_XZDF)	  // 吸庄大法
			//{
			//	iSlectId = ID_SPECIAL_XZDF ;
			//}

			//// 权限控制
			//CNCButton &btnControl = m_mapBtn[iID];
			//CString StrName = btnControl.GetCaption();
			//
			//if ( !CPluginFuncRight::Instance().IsUserHasRight(StrName, true, true) )
			//{
			//	return CWnd::OnCommand(wParam, lParam);
			//}

			//if(0 != iSlectId)
			//{
			//	CStockSelectManager::E_IndexSelectStock eJGLT = CStockSelectManager::StrategyJGLT;
			//	bool32 b = CStockSelectManager::GetIndexSelectStockEnumByCmdId(iSlectId, eJGLT);
			//	ASSERT( b );
			//	DWORD dwSlectFlags = eJGLT;

			//	// 怎么处理这些选股条件呢？互斥，与，并？ TODO
			//	CStockSelectManager::Instance().SetIndexSelectStockFlag( dwSlectFlags );	// 互斥处理
			//	CStockSelectManager::Instance().SetCurrentReqType(CStockSelectManager::IndexShortMidSelect);
			//}
		}
		break;
// 	case EBID_SmartSelStock_XZDF:						// 吸庄大法
// 		{
// 			int iSel = CStockSelectManager::HotTimeSale;
// 
// 			if ( iSel ==  CStockSelectManager::OpenStockPool)
// 			{
// 				CStockSelectManager::Instance().SetCurrentReqType(CStockSelectManager::HotSelect, false, true);
// 			}
// 			else 
// 			{
// 				// ∪处理
// 				DWORD dwFlag = CStockSelectManager::Instance().GetHotSelectStockFlag();
// 				CStockSelectManager::Instance().SetCurrentReqType(CStockSelectManager::HotSelect);
// 				CStockSelectManager::Instance().SetHotSelectStockFlag(/*dwFlag ^ (DWORD)*/iSel);	// XOR
// 			}
// 		}
// 		break;
	default:
		break;
	}
	
	return CWnd::OnCommand(wParam, lParam);
}

BOOL CLeftToolBar::OnStockDecisionCommand(WPARAM wParam, LPARAM lParam)
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if ( !pMainFrm )
	{
		return CWnd::OnCommand(wParam, lParam);
	}

	int32 iID = (int32)wParam;
	CString StrWspName = m_mapBtn[iID].GetCaption();
	
	if (StrWspName.IsEmpty())
	{
		return CWnd::OnCommand(wParam, lParam);
	}

	////权限判断
	//if ( !CPluginFuncRight::Instance().IsUserHasRight(StrWspName, true, true))
	//{
	//	return CWnd::OnCommand(wParam, lParam);
	//}
	CIoViewKLine *pIoKLine = NULL;
	switch(iID)
	{
	case  EBID_StockDecision_XLFD:						// 王牌狙击       
	case  EBID_StockDecision_CHGR:						// 决胜金叉		
	case  EBID_StockDecision_XRZL:						// 海底捞月		
	case  EBID_StockDecision_YYLJ:						// 钱坤挪移		
	case  EBID_StockDecision_MSKK:						// 波段擒牛		
	case  EBID_StockDecision_SJHY:						// 猎杀主力		
		pIoKLine = (CIoViewKLine *)pMainFrm->GetIoViewByPicMenuIdAndActiveIoView(ID_PIC_KLINE, true, true, false, true, false, true);
		if (pIoKLine)// 如果没有找到，那么就创建该对象(注意：第二个参数标示创建的意思)
		{
			if ( IsWindow(pIoKLine->GetSafeHwnd()) )
			{	
				pIoKLine->AddIndexGroup(StrWspName);

				pIoKLine->BringToTop();
			}
		}
		break;
	//case EBID_StockDecision_JDZB:						// 金盾指标
	//	{
	//		CIoViewKLine *pIoKLine = (CIoViewKLine *)pMainFrm->GetIoViewByPicMenuIdAndActiveIoView(ID_PIC_KLINE, false, true, false, true, false, true);
	//		
	//		if (pIoKLine && IsWindow(pIoKLine->GetSafeHwnd()) )
	//		{	
	//			bool32 bClearOldIndex = false;//pItem->uBtnId != TB_RiseFallView;	// 牛熊分界不需要变更指标
	//			{
	//				// 让K线自己选择切换商品否
	//				pIoKLine->ShowJinDunIndex();
	//				pIoKLine->BringToTop();
	//			}
	//		}
	//	}
	//	break;
	default:
		break;
	}
	
	return CWnd::OnCommand(wParam, lParam);
}

BOOL CLeftToolBar::OnCommand( WPARAM wParam, LPARAM lParam )
{
    int32 iID = (int32)wParam;
    CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();

    std::map<int, CNCButton>::iterator itHome = m_mapBtn.find(iID);
    if (m_mapBtn.end() != itHome)
    {
		switch(m_eLeftBarType)
		{
		case ELBT_StockCN:
			{
				OnStockCNCommand((WPARAM)iID, (LPARAM)0);
			}
			break;
		case ELBT_StockHK:
			{
				OnStockHKCommand((WPARAM)iID, (LPARAM)0);
			}
			break;
		case ELBT_Futures:
			{
				OnFuturesCommand((WPARAM)iID, (LPARAM)0);
			}
			break;
		case ELBT_GlobalIndex:
			{
				OnGlobalIndexCommand((WPARAM)iID, (LPARAM)0);
			}
			break;
		case ELBT_QuoteForeign:
			{
				OnQuoteForeignCommand((WPARAM)iID, (LPARAM)0);
			}
			break;
		case ELBT_SmartSelStock:
			{
				/////////////////////////////////////////////////////
				//===================CODE CLEAN======================
				//////////////////////////////////////////////////////
				//pMain->OnMsgPickModelTypeEvent((WPARAM)iID, (LPARAM)0);	


				// ↓这个函数不属于代码清除范围
				//OnSmartSelStockCommand((WPARAM)iID, (LPARAM)ELBT_SmartSelStock);
			}
			break;
		case ELBT_SmartSelStock1:
			{
				OnSmartSelStockCommand((WPARAM)iID, (LPARAM)ELBT_SmartSelStock1);
			}
			break;
		case ELBT_SmartSelStock2:
			{
				OnSmartSelStockCommand((WPARAM)iID, (LPARAM)ELBT_SmartSelStock2);
			}
			break;
		case ELBT_SmartSelStock3:
			{
				OnSmartSelStockCommand((WPARAM)iID, (LPARAM)ELBT_SmartSelStock3);
			}
			break;
		case ELBT_StockDecision:
			{
				OnStockDecisionCommand((WPARAM)iID, (LPARAM)0);
			}
			break;
		default:
			break;
		}
    }

    return CWnd::OnCommand(wParam, lParam);
}

bool32 CLeftToolBar::DoOpenCfm( const CString &StrCfmXmlName )
{
    // 写死的需要授权的列表 是否可变更为cfm增加以属性需要服务器授权，如设置
    // 该属性，则向授权列表咨询是否有权限，无则打开失败(不过这样就外部可控了)
    //	就这样搞
    // 先检查权限
    T_CfmFileInfo cfm;
    const DWORD dwCmd = GetIoViewCmdIdByCfmName(StrCfmXmlName);
    if ( !CCfmManager::Instance().QueryUserCfm(StrCfmXmlName, cfm) && dwCmd == 0 )
    {
        MessageBox(_T("页面文件缺失，请确认是否已经安装!"), _T("错误"), MB_OK |MB_ICONERROR);
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
        return CCfmManager::Instance().LoadCfm(StrCfmXmlName,FALSE) != NULL;	// 普通的
    }
}

bool32 CLeftToolBar::DoCloseCfm( const CString &StrCfmXmlName )
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

const std::pair<DWORD, CCfmManager::E_SystemDefaultCfm> sIoViewCmd2CfmName[] = {
    std::pair<DWORD, CCfmManager::E_SystemDefaultCfm>(TB_IOCMD_REPORT, CCfmManager::ESDC_Report),
    std::pair<DWORD, CCfmManager::E_SystemDefaultCfm>(TB_IOCMD_KLINE, CCfmManager::ESDC_KLine),
    std::pair<DWORD, CCfmManager::E_SystemDefaultCfm>(TB_IOCMD_TREND, CCfmManager::ESDC_Trend),
    std::pair<DWORD, CCfmManager::E_SystemDefaultCfm>(TB_IOCMD_NEWS, CCfmManager::ESDC_News),
    //{CNewTBWnd::TB_IOCMD_F10, _T("F10资料")},
};
const DWORD sIoViewCmd2CfmCount = sizeof(sIoViewCmd2CfmName)/sizeof(sIoViewCmd2CfmName[0]);

bool32 CLeftToolBar::LoadSystemDefaultCfm( const CString &StrSDCName, bool32 bCloseExistence /*= false*/ )
{
    DWORD dwIoCmd = GetIoViewCmdIdByCfmName(StrSDCName);
    if ( dwIoCmd > 0 )
    {
        return LoadSystemDefaultCfm(dwIoCmd, bCloseExistence);
    }
    return false;
}

bool32 CLeftToolBar::LoadSystemDefaultCfm( DWORD iIoViewCmd, bool32 bCloseExistence /*= false*/ )
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

DWORD CLeftToolBar::GetIoViewCmdIdByCfmName( const CString &StrCfmName )
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

void CLeftToolBar::SetCheckStatus( int iID )
{
    map<int, CNCButton>::iterator iter;
    for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
    {
        CNCButton &btn = iter->second;

        if(iID == iter->first )
        {
			if (!iter->second.GetCheck())
			{
				// m_mapBtn[iID].SetCheckStatus(TRUE);
				 m_mapBtn[iID].SetCheck(TRUE);
			}
        }
        else if (btn.GetCheck())
        {
            btn.SetCheckStatus(FALSE);
        }
    }
}

void CLeftToolBar::OnTimer(UINT nIDEvent)
{

	if (KTimerIdTip   == nIDEvent)
	{
		KillTimer(KTimerIdTip);
		// 
		ShowTips();
	}
}

void CLeftToolBar::SetPageType( E_LeftBarType eType )
{
	m_eLeftBarType = eType;
}

BOOL CLeftToolBar::PreCreateWindow( CREATESTRUCT& cs )
{
	if( !CToolBar::PreCreateWindow(cs))
		return FALSE;

	m_dwStyle &= ~CBRS_BORDER_ANY; // 关闭所有border

	return TRUE;
}

int CLeftToolBar::GetBtnIDByName(CString StrName)
{
	int iID = INVALID_ID;
	map<int, CNCButton>::iterator iter;
	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
	{
		if(iter->second.GetCaption() == StrName)
		{
			iID = iter->first;
			break;
		}
	}

	return iID;
}

void CLeftToolBar::ChangeButtonStateByCfmName(const CString &StrWspName, const E_LeftBarType &eType)
{
	int32 iCnt = 0;
	
	vector<T_ButtonItem> vecItem; 
	int32 iBtnId = INVALID_ID;
	CLeftBarItemManager::Instance().GetItemsByType(eType, vecItem);
	std::vector<T_ButtonItem>::iterator iter;
	
	for (iter=vecItem.begin(); iter!=vecItem.end(); ++iter)
	{
		if (StrWspName == iter->m_StrWspName )
		{
			
			if (!iter->m_bChecked)
			{
				continue;
			}
			iBtnId = iter->m_iBtnID;
			break;
		}
	}

	if (INVALID_ID != iBtnId)
	{
		m_mapButtonHovering[eType] = iBtnId;
	}
	else if (ELBT_SmartSelStock == eType || ELBT_SmartSelStock1 == eType||ELBT_SmartSelStock2 == eType||ELBT_SmartSelStock3 == eType)
	{
		// 从其他条件选股界面跳转过来，需要更新下选股标志
		PostMessage(WM_COMMAND, (WPARAM)m_mapButtonHovering[eType], 0);
	}

	if (ELBT_SmartSelStock == eType && INVALID_ID == iBtnId)
	{
		map<int, CNCButton>::iterator iter;
		for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
		{
			iBtnId = iter->first;
			m_mapButtonHovering[eType] = iBtnId;
			break;
		}
	}
	//
	SetCheckStatus(m_mapButtonHovering[eType]);
	DrawToolButton();
}

void CLeftToolBar::ChangeLeftBarStatus(CString StrWspName, const E_SwitchType &eType)
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	
	m_eSwitchType = eType;

	if(EST_WSP == m_eSwitchType)
	{
		// 切换版面
		if ( NULL != pMainFrm && pMainFrm->IsKindOf(RUNTIME_CLASS(CMainFrame)) )
		{
			if (StrWspName.IsEmpty())
			{
				CMPIChildFrame *pChildFrame = (CMPIChildFrame *)pMainFrm->MDIGetActive();
				
				if (NULL == pChildFrame)
				{
					return;
				}
				
				StrWspName = pChildFrame->GetIdString();
			}
		
			bool32 bExist = ChangeLeftBar(StrWspName);;
			
			if( !bExist )
			{
				pMainFrm->ShowControlBar(this, false, false);
			}
			else
			{
				if(!IsVisible())
				{
					pMainFrm->ShowControlBar(this, true, false);
				}

				//OnStockSelectModelQueueItem();
				InitialToolBarBtns(m_eLeftBarType);
				ReLayout();
				Invalidate();
			}

			// 
			ChangeButtonStateByCfmName(StrWspName, m_eLeftBarType);
		}
	}
	else
	{
		// 暂时在OnLButtonDown中处理按钮状态
	}
}


void CLeftToolBar::ChangePickModelStatus()
{
	InitialToolBarBtns(m_eLeftBarType);												
	SetCheckStatus(m_mapButtonHovering[m_eLeftBarType]);
	DrawToolButton();
}

void CLeftToolBar::ShowTips()
{
	if (ELBT_SmartSelStock != m_eLeftBarType && ELBT_SmartSelStock1 != m_eLeftBarType &&ELBT_SmartSelStock2 != m_eLeftBarType &&ELBT_SmartSelStock3 != m_eLeftBarType && ELBT_StockDecision != m_eLeftBarType)
	{
		return;
	}

	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	CRect rectClient;
	GetClientRect(&rectClient);

	if (!PtInRect(&rectClient, point))
	{
		return;
	}

	// 判断需要显示文字
	BOOL bShowTip = FALSE;
	CString StrTipMsg   = L"";

	vector<T_ButtonItem> vecItem; 
	Image	*pImg = NULL;
	CRect rcControl;
	std::map<int, CNCButton>::iterator itBtn = m_mapBtn.begin();

	for (; itBtn != m_mapBtn.end(); itBtn++)
	{
		CString StrDesc  = itBtn->second.GetDescription();

		CNCButton &btnControl = itBtn->second;
		btnControl.GetRect(rcControl);

		if (rcControl.PtInRect(point))
		{
			if (StrDesc.GetLength() > 0 )
			{
				bShowTip = TRUE;
				StrTipMsg  = StrDesc;
			}

			break;
		}
	}

	if (bShowTip)
	{
		StrTipMsg += "\n";
		m_TipWnd.Show(point, StrTipMsg, L"");
	}
	else
	{
		m_TipWnd.Hide();
	}
}

void CLeftToolBar::InitialButtonHoveringParam()
{
	// 初始化默认选中按钮
	m_mapButtonHovering.clear();
	m_mapButtonHovering[ELBT_NONE]          = INVALID_ID;
	m_mapButtonHovering[ELBT_StockCN]	    = EBID_StockCN_HQBJ; 
 	m_mapButtonHovering[ELBT_StockHK]       = EBID_StockHK_HQBJ;
 	m_mapButtonHovering[ELBT_Futures]       = EBID_Future_HQBJ;
 	m_mapButtonHovering[ELBT_GlobalIndex]   = EBID_GlobalIndex_HQBJ;
 	m_mapButtonHovering[ELBT_QuoteForeign]  = EBID_QuoteForeign_HQBJ;
	m_mapButtonHovering[ELBT_SmartSelStock] = EBID_SmartSelStock_YJFX;
	m_mapButtonHovering[ELBT_SmartSelStock1] = EBID_SmartSelStock_YJFX;
	m_mapButtonHovering[ELBT_SmartSelStock2] = EBID_SmartSelStock_QYZS;
	m_mapButtonHovering[ELBT_SmartSelStock3] = EBID_SmartSelStock_SHJD;
 	m_mapButtonHovering[ELBT_StockDecision]  = EBID_StockDecision_XLFD;
}

void CLeftToolBar::ShowSelectedBtn(int32 iID)
{
	std::map<int, CNCButton>::iterator iter = m_mapBtn.begin();
	int32 iBtnId = iter->second.GetControlId();
	if (iID - iBtnId > m_indexLastShow )
	{
		m_indexLastShow  += (iID - iBtnId );
		m_indexFirstShow += (iID - iBtnId );
	}
	else if (iID - iBtnId < m_indexFirstShow)
	{
		m_indexFirstShow -= ( m_indexFirstShow + iBtnId - iID );
		m_indexLastShow  -= ( m_indexFirstShow + iBtnId - iID );
	}
	ReLayout();
	Invalidate(FALSE);
}

CMerch* CLeftToolBar::GetMarketFirstMerch()
{
	
	CMainFrame *pMainFrm = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

	CIoViewBase* pIoView =pMainFrm->FindActiveIoView();
	CMerch* pMerch = NULL;
	if ( NULL == pIoView )
	{
		return false;
	}

	if ( pIoView->IsKindOf(RUNTIME_CLASS(CIoViewReport)) )
	{
		pMerch = ((CIoViewReport*)pIoView)->GetGridSelMerch();
	}
	else if (pIoView->IsKindOf(RUNTIME_CLASS(CIoViewDuoGuTongLie)) && NULL != pIoView->m_pMerchXml)
	{
		pMerch = pIoView->m_pMerchXml;
	}
	else
	{
		pMerch = pIoView->GetMerchXml();	// 默认为pMerchXml
		
		if (!pMerch)
		{
			CIoViewReport *pReport = pMainFrm->FindIoViewReport(false);
			if (pReport && pReport->IsKindOf(RUNTIME_CLASS(CIoViewReport)))
			{
				pMerch = pReport->GetGridSelMerch();
			}
		}
	}

	if (NULL != pMerch)
	{
		m_iMarketId = pMerch->m_MerchInfo.m_iMarketId;
	}
   
	return pMerch;
}

void CLeftToolBar::ShowDuoGuTongLie()
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	
	if(NULL == pMain)
	{
		return;
	}

	CMerch* pMerch = GetMarketFirstMerch();
	CIoViewDuoGuTongLie *pIoViewDuoGuTongLie = NULL;
	CMPIChildFrame *pActiveFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pMain->MDIGetActive());
 	CString StrWspName = _T("多股同列");
	
	if (NULL != pActiveFrame)
	{
		if(pActiveFrame->GetIdString() != StrWspName)
		{
			bool32 bLoad = DoOpenCfm(StrWspName);
			if ( bLoad )
			{
				pIoViewDuoGuTongLie = pMain->FindIoViewDuoGuTongLie(true);
			}
			if(pIoViewDuoGuTongLie)
			{
				pIoViewDuoGuTongLie->RefreshView(pMerch);
			}
		}
		else
		{
			pIoViewDuoGuTongLie = pMain->FindIoViewDuoGuTongLie(true);

			if(pIoViewDuoGuTongLie)
			{
				pIoViewDuoGuTongLie->RefreshView(pMerch);
			}
		}

		// 加载完页面更新下侧边栏状态
		ChangeLeftBarStatus(StrWspName);
	}
}

void CLeftToolBar::SetSwitchType(E_SwitchType eType)
{
	m_eSwitchType = eType;
}

E_ReportType CLeftToolBar::GetMerchKind(IN CMerch *pMerch)
{	
	E_ReportType eReportType = ERTStockHk;	// 默认值

	if (NULL != pMerch)
	{
		eReportType = pMerch->m_Market.m_MarketInfo.m_eMarketReportType;
	}

	return eReportType;
}


void CLeftToolBar::OnDestroy()
{
	CToolBar::OnDestroy();

	m_TipWnd.DestroyWindow();
}

void CLeftToolBar::OnChooseStockStateResp()
{
	return;
	bool32 bChangeStatus = CLeftBarItemManager::Instance().InitItemMap();
	
	// 条件选股按钮状态改变时要刷新下

	if (bChangeStatus && ELBT_SmartSelStock == m_eLeftBarType)
	{
		InitialToolBarBtns(m_eLeftBarType);												

		SetCheckStatus(m_mapButtonHovering[m_eLeftBarType]);
		DrawToolButton();
	}

	if (bChangeStatus && ELBT_SmartSelStock1 == m_eLeftBarType)
	{
		InitialToolBarBtns(m_eLeftBarType);

		SetCheckStatus(m_mapButtonHovering[m_eLeftBarType]);
		DrawToolButton();
	}

	if (bChangeStatus && ELBT_SmartSelStock2 == m_eLeftBarType)
	{
		InitialToolBarBtns(m_eLeftBarType);

		SetCheckStatus(m_mapButtonHovering[m_eLeftBarType]);
		DrawToolButton();
	}

	if (bChangeStatus && ELBT_SmartSelStock3 == m_eLeftBarType)
	{
		InitialToolBarBtns(m_eLeftBarType);

		SetCheckStatus(m_mapButtonHovering[m_eLeftBarType]);
		DrawToolButton();
	}
}
