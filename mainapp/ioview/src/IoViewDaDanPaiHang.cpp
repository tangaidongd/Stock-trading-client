#include "stdafx.h"

#include "memdc.h"
#include <math.h>
#include "IoViewManager.h"
#include "MerchManager.h"
#include "facescheme.h"
#include "IoViewTimeSaleRank.h"
#include "IoViewTimeSaleStatistic.h"
#include "IoViewDaDanPaiHang.h"
#include "ColorStep.h"
#include "BJSpecialStringImage.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////
// id
const UINT KIdXSrollBarHorz = 0x301;
const UINT KIdWndTab		= 0x302;
const UINT KIdWndStatistic  = 0x303;
const UINT KIdWndRank		= 0x304;

// xml
const char KXmlStrCurSelKey[]		= "select";


namespace
{
	#define  INVALID_ID  -1
	#define GRIDCTRL_MOVEDOWN_SIZE	    50

	#define MID_BUTTON_PAIHANG_START	20001
	#define MID_BUTTON_PAIHANG_PROMPT	20002
	#define MID_BUTTON_PAIHANG_ONE		20003
	#define MID_BUTTON_PAIHANG_THREE	20004
	#define MID_BUTTON_PAIHANG_FIVE		20005
	#define MID_BUTTON_PAIHANG_TEN		20006
	#define MID_BUTTON_PAIHANG_END		20010
}

/////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewDaDanPaiHang, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewDaDanPaiHang, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewDaDanPaiHang)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, KIdWndTab, OnSelChange)
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewDaDanPaiHang::CIoViewDaDanPaiHang()
:CIoViewBase()
{
	m_bActive		 = false;
	m_iCurrentPeriod =  1;
	m_pImgBtn = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewDaDanPaiHang::~CIoViewDaDanPaiHang()
{
	DEL(m_pImgBtn);
}


///////////////////////////////////////////////////////////////////////////////
// OnPaint

void CIoViewDaDanPaiHang::OnPaint()
{
	CPaintDC dc(this); // device context for painting	
	
	if ( GetParentGGTongViewDragFlag() || m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}
	
	//
	UnLockRedraw();

	if ( !m_xsbHorzStatistic.IsWindowVisible() && !m_xsbHorzRank.IsWindowVisible() )
	{
		CRect rcXSB;
		m_xsbHorzStatistic.GetWindowRect(rcXSB);
		ScreenToClient(&rcXSB);
		dc.FillSolidRect(rcXSB, GetIoViewColor(ESCBackground));

		// 下面加一条线
		rcXSB.top = rcXSB.bottom-1;
		dc.FillSolidRect(rcXSB, RGB(76, 85, 118));
	}

	RedrawCycleBtn(&dc);

}

void CIoViewDaDanPaiHang::RedrawCycleBtn(CPaintDC *pPainDC)
{
	//调整gridctrl位置
	CRect rcClient;
	GetClientRect(rcClient);

	const CSize sizeTab = m_wndGuiTab.GetWindowWishSize();
	rcClient.bottom  = rcClient.top+ GRIDCTRL_MOVEDOWN_SIZE + sizeTab.cy + 1;

	CDC memDC;
	memDC.CreateCompatibleDC(pPainDC);
	CBitmap bmp;
	int nWidth = rcClient.Width();
	int nHeight = rcClient.Height();
	bmp.CreateCompatibleBitmap(pPainDC, rcClient.Width(), rcClient.Height());
	//memDC.FillSolidRect(rcClient.left ,rcClient.top, rcClient.Width(),rcClient.Height(),RGB(37,40,45));
	memDC.SelectObject(&bmp);
	memDC.SetBkMode(TRANSPARENT);

	Graphics graphics(memDC.GetSafeHdc());

	SolidBrush  brush(Color(37,40,45));
	graphics.FillRectangle(&brush, rcClient.left ,rcClient.top, rcClient.Width(),rcClient.Height());

	btnPrompt.DrawButton(&graphics);
	map<int, CNCButton>::iterator iter;
	for (iter=m_mapBtnCycleList.begin(); iter!=m_mapBtnCycleList.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		btnControl.DrawButton(&graphics);
	}

	pPainDC->BitBlt(rcClient.left ,rcClient.top, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);
	pPainDC->SelectClipRgn(NULL);
	bmp.DeleteObject();
	memDC.DeleteDC();
}

void CIoViewDaDanPaiHang::CreateBtnList()
{
	m_mapBtnCycleList.clear();

	CRect rcClient;
	GetClientRect(rcClient);
	rcClient.bottom = GRIDCTRL_MOVEDOWN_SIZE;

	CRect rcBtn(rcClient);
	
	int iDistance = 15;

	m_pImgBtn = Image::FromFile(_T("image//subbtn.png"));

	const CSize sizeTab = m_wndGuiTab.GetWindowWishSize();
	rcBtn.top	 += sizeTab.cy + 1;
    int iBtnWidth = m_pImgBtn->GetWidth();
	// 因为是5个按钮所以乘以5
	rcBtn.left   =500;//rcClient.left +  (rcClient.Width() - (iBtnWidth + iDistance)* 5)/2;
	rcBtn.right  = rcBtn.left + iBtnWidth;
	rcBtn.bottom = rcBtn.top + m_pImgBtn->GetHeight() / 3;

	btnPrompt.CreateButton(L"可选周期:", rcBtn, this, NULL, 0, MID_BUTTON_PAIHANG_PROMPT);

	//提示按钮提示
	btnPrompt.SetTextBkgColor(RGB(37,40,45), RGB(37,40,45), RGB(37,40,45));
	btnPrompt.SetTextFrameColor(RGB(37,40,45), RGB(37,40,45), RGB(37,40,45));
	btnPrompt.SetTextColor(RGB(190,191,191), RGB(190,191,191), RGB(190,191,191));

	rcBtn.left = rcBtn.right + iDistance;
	rcBtn.right = rcBtn.left + iBtnWidth;
	AddNCButton(&rcBtn, m_pImgBtn, 3,  MID_BUTTON_PAIHANG_ONE, L"单日");

	rcBtn.left = rcBtn.right + iDistance;
	rcBtn.right = rcBtn.left + iBtnWidth;
	AddNCButton(&rcBtn, m_pImgBtn, 3,  MID_BUTTON_PAIHANG_THREE, L"3日");

	rcBtn.left = rcBtn.right + iDistance;
	rcBtn.right = rcBtn.left + iBtnWidth;
	AddNCButton(&rcBtn, m_pImgBtn, 3,  MID_BUTTON_PAIHANG_FIVE, L"5日");

	rcBtn.left = rcBtn.right + iDistance;
	rcBtn.right = rcBtn.left + iBtnWidth;
	AddNCButton(&rcBtn, m_pImgBtn, 3,  MID_BUTTON_PAIHANG_TEN, L"10日");
}

void CIoViewDaDanPaiHang::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();
	// 让子窗口的ioview颜色改变 TODO

	m_wndStatistic.OnIoViewColorChanged();
	m_wndRank.OnIoViewColorChanged();

	ChangeColorFont();
	
	Invalidate();
}


void CIoViewDaDanPaiHang::OnIoViewFontChanged()
{	
	CIoViewBase::OnIoViewFontChanged();
	// 让子窗口的ioview字体改变 TODO
	
	m_wndStatistic.OnIoViewFontChanged();
	m_wndRank.OnIoViewFontChanged();
	
	Invalidate();
}

void CIoViewDaDanPaiHang::LockRedraw()
{
	if ( !m_bLockRedraw )
	{
		if ( IsWindowVisible() )
		{
			SendMessage(WM_SETREDRAW, (WPARAM)FALSE, 0);
			CWnd *pChild = GetWindow(GW_CHILD);
			while ( NULL != pChild )
			{
				if ( pChild->IsWindowVisible() )
				{
					if ( pChild->IsKindOf(RUNTIME_CLASS(CIoViewBase)) )
					{
						(DYNAMIC_DOWNCAST(CIoViewBase, pChild))->LockRedraw();
					}
					pChild->SendMessage(WM_SETREDRAW, (WPARAM)FALSE, 0);
				}
				pChild = pChild->GetWindow(GW_HWNDNEXT);
			}
		}
		m_bLockRedraw = true;
	}
}

void CIoViewDaDanPaiHang::UnLockRedraw()
{
	if ( m_bLockRedraw )
	{
		if ( IsWindowVisible() )
		{
			SendMessage(WM_SETREDRAW, (WPARAM)TRUE, 0);
			Invalidate();
			CWnd *pChild = GetWindow(GW_CHILD);
			while ( NULL != pChild )
			{
				if ( pChild->IsWindowVisible() )
				{
					pChild->SendMessage(WM_SETREDRAW, (WPARAM)TRUE, 0);		// 进行redraw设置会使hide变为show，so需要注意
					if ( pChild->IsKindOf(RUNTIME_CLASS(CIoViewBase)) )
					{
						(DYNAMIC_DOWNCAST(CIoViewBase, pChild))->UnLockRedraw();
					}
				}
				
				pChild = pChild->GetWindow(GW_HWNDNEXT);
			}
		}
		m_bLockRedraw = false;
	}
}

int CIoViewDaDanPaiHang::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	int iRet = CIoViewBase::OnCreate(lpCreateStruct);
	if ( -1 == iRet )
	{
		return -1;
	}

	//
	InitialIoViewFace(this);


	// 初始化各子窗口
	ASSERT( NULL != m_pAbsCenterManager );
	m_wndStatistic.SetCenterManager(m_pAbsCenterManager);
	m_wndRank.SetCenterManager(m_pAbsCenterManager);
	
	m_xsbHorzStatistic.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, this, CRect(0,0,0,0), KIdXSrollBarHorz);
	m_xsbHorzRank.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, this, CRect(0,0,0,0), KIdXSrollBarHorz-1);
	m_wndGuiTab.Create(WS_CHILD |WS_VISIBLE, CRect(0,0,0,0), this, KIdWndTab);
	m_wndGuiTab.SetFontByName(_T("宋体"), GB2312_CHARSET);
	m_wndGuiTab.SetFontHeight(-15);
	m_wndGuiTab.SetTabSpace(4);
	m_wndGuiTab.SetStyle(CGuiTabWnd::S3D);
	m_wndGuiTab.SetTabBkColor(RGB(88,0,0), RGB(128,128,128));
	m_wndStatistic.Create(WS_CHILD |WS_VISIBLE |SS_NOTIFY, this, CRect(0,0,0,0), KIdWndStatistic);
	m_wndRank.Create(WS_CHILD |SS_NOTIFY, this, CRect(0,0,0,0), KIdWndRank);

	m_xsbHorzStatistic.SetScrollBarLeftArrowH(-1);
	m_xsbHorzStatistic.SetScrollBarRightArrowH(-1);
	m_xsbHorzStatistic.SetBorderColor(GetIoViewColor(ESCBackground));

	m_xsbHorzRank.SetScrollBarLeftArrowH(-1);
	m_xsbHorzRank.SetScrollBarRightArrowH(-1);
	m_xsbHorzRank.SetBorderColor(GetIoViewColor(ESCBackground));

	m_wndStatistic.SetXSBHorz(&m_xsbHorzStatistic);		// 设置大单排行的水平滚动条
	m_wndRank.SetXSBHorz(&m_xsbHorzRank);

	m_wndGuiTab.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);
	m_wndGuiTab.SetALingTabs(CGuiTabWnd::ALN_TOP);
	CString Str1 = CBJSpecialStringImage::Instance().GetSpecialString(CBJSpecialStringImage::String_DDPH_DDTJ);
	CString Str2 = CBJSpecialStringImage::Instance().GetSpecialString(CBJSpecialStringImage::String_DDPH_DDPH);
	m_wndGuiTab.Addtab(Str1, Str1, _T(""));
	m_wndGuiTab.Addtab(Str2, Str2, _T(""));

	ChangeColorFont();
	CreateBtnList();

	if(m_mapBtnCycleList.size() > 0)
	{
		int iBtnID = MID_BUTTON_PAIHANG_ONE;
		switch(m_iCurrentPeriod)
		{
		case 1:
			iBtnID = MID_BUTTON_PAIHANG_ONE;
			break;

		case 3:
			iBtnID = MID_BUTTON_PAIHANG_THREE;
			break;

		case 5:
			iBtnID = MID_BUTTON_PAIHANG_FIVE;
			break;

		case 10:
			iBtnID = MID_BUTTON_PAIHANG_TEN;
			break;

		default:
			break;
		}

		map<int, CNCButton>::iterator iter;

		for (iter=m_mapBtnCycleList.begin(); iter!=m_mapBtnCycleList.end(); ++iter)
		{
			CNCButton &btnControl = iter->second;

			if (btnControl.GetControlId() == iBtnID)
			{
				btnControl.SetCheckStatus(TRUE);
			}
			else
			{
				btnControl.SetCheckStatus(FALSE);
			}
		}
	}
	
	SetCurTab(0);		// 默认大单统计
	//
	return iRet;
}

void CIoViewDaDanPaiHang::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);
	
	RecalcLayout();
	Invalidate();
}

// 通知视图改变关注的商品
void CIoViewDaDanPaiHang::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	if (NULL == pMerch)
	{
		return;
	}
	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
}

//
void CIoViewDaDanPaiHang::OnVDataForceUpdate()
{
	RequestViewData();	// 无请求
}

void CIoViewDaDanPaiHang::OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch)
{
	
}

bool32 CIoViewDaDanPaiHang::FromXml(TiXmlElement * pElement)
{
	if (NULL == pElement)
		return false;

	// 判读是不是IoView的节点
	const char *pcValue = pElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;

	// 判断是不是描述自己这个业务视图的节点
	const char *pcAttrValue = pElement->Attribute(GetXmlElementAttrIoViewType());
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(CString(pcAttrValue)) != 0)	// 不是描述自己的业务节点
		return false;
	
	// 读取本业务视图特有的内容
	SetFontsFromXml(pElement);
	SetColorsFromXml(pElement);

	//
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

	pcAttrValue = pElement->Attribute(KXmlStrCurSelKey);
	int32 iCurTab = 0;
	if ( NULL != pcAttrValue )
	{
		iCurTab = atoi(pcAttrValue);
		if ( iCurTab < 0 || iCurTab > m_wndGuiTab.GetCount() )
		{
			iCurTab = 0;
		}
	}
	
	// 商品发生改变
	OnVDataMerchChanged(iMarketId, StrMerchCode, pMerchFound);	

	// 更改子视图的颜色与字体配置
	ChangeColorFont();

	//if ( iCurTab != 0 )
	//{
	//	SetCurTab(iCurTab);		// 数据回来太慢，如果是这个视图在等待，难看
	//}

	return true;
}

CString CIoViewDaDanPaiHang::ToXml()
{
	CString StrThis;

	CString StrMarketId;
	StrMarketId.Format(L"%d", m_MerchXml.m_iMarketId);

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\"  %s=\"%d\" ",
		CString(GetXmlElementValue()), 
		CString(GetXmlElementAttrIoViewType()), 
		CIoViewManager::GetIoViewString(this),
		CString(GetXmlElementAttrMerchCode()),
		m_MerchXml.m_StrMerchCode,
		CString(GetXmlElementAttrMarketId()), 
		StrMarketId,
		CString(KXmlStrCurSelKey), m_wndGuiTab.GetCurtab()
		);
	//
	CString StrFace;
	StrFace  = SaveColorsToXml();
	StrFace += SaveFontsToXml();
	
	StrThis += StrFace;
	StrThis += L">\n";
	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

CString CIoViewDaDanPaiHang::GetDefaultXML()
{
	CString StrThis;

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" >\n", 
		CString(GetXmlElementValue()), 
		CString(GetXmlElementAttrIoViewType()), 
		CIoViewManager::GetIoViewString(this),
		CString(GetXmlElementAttrMerchCode()),
		L"",
		CString(GetXmlElementAttrMarketId()), 
		L"-1");

	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}


void CIoViewDaDanPaiHang::OnIoViewActive()
{
	// 激活子视图获取焦点, 响应active
	m_bActive = IsActiveInFrame();

	CIoViewBase *pActive = GetCurTabIoView();
	if ( NULL != pActive )
	{
		pActive->OnIoViewActive();
	}
	
	CIoViewBase::OnIoViewActive();

	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	Invalidate();
}

void CIoViewDaDanPaiHang::OnIoViewDeactive()
{
	// 激活子视图获取焦点, 响应deactive
	m_bActive = false;

	CIoViewBase *pActive = GetCurTabIoView();
	if ( NULL != pActive )
	{
		pActive->OnIoViewDeactive();
	}
	
	Invalidate();
}

void  CIoViewDaDanPaiHang::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;

	if (NULL != m_pMerchXml)
	{	
		StrTitle += L" ";
		StrTitle += m_pMerchXml->m_MerchInfo.m_StrMerchCnName;
	}
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

BOOL CIoViewDaDanPaiHang::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

void CIoViewDaDanPaiHang::RecalcLayout()
{
	CRect rcClient(0,0,0,0);
	GetClientRect(rcClient);

	if (rcClient.Width() <= 0 || rcClient.Height() <= 0)
	{
		return;
	}
	static const CSize sizeTab = m_wndGuiTab.GetWindowWishSize();
	ASSERT( sizeTab.cy > 0 );
	// 优先满足tab，然后滚动
	CRect rcTab(rcClient), rcSBHorz(0,0,0,0);
	rcTab.bottom = rcTab.top + sizeTab.cy + 5;
	if ( rcTab.Width() > sizeTab.cx )
	{
		rcTab.right = rcTab.left + sizeTab.cx;
	}

	m_wndGuiTab.MoveWindow(rcTab);

	CRect rcIoView(rcClient);
	rcIoView.top = rcTab.bottom;
	rcIoView.top = GRIDCTRL_MOVEDOWN_SIZE;
	rcIoView.bottom = rcIoView.bottom - sizeTab.cy;
	m_wndStatistic.MoveWindow(rcIoView);
	m_wndRank.MoveWindow(rcIoView);

	// 修改滚动条位置 借用切换的高度sizeTab
	rcSBHorz.top = rcIoView.bottom;
	rcSBHorz.bottom = rcSBHorz.top + sizeTab.cy;
	rcSBHorz.left = rcIoView.left;
	rcSBHorz.right = rcIoView.right;

	m_xsbHorzStatistic.SetSBRect(rcSBHorz);
	m_xsbHorzRank.SetSBRect(rcSBHorz);
}

void CIoViewDaDanPaiHang::SetCurTab( int32 iTab )
{
	if ( iTab >=0 && iTab < m_wndGuiTab.GetCount() )
	{
		const int32 iOld = m_wndGuiTab.GetCurtab();
		m_wndGuiTab.SetCurtab(iTab);
		if ( iOld == m_wndGuiTab.GetCurtab() )
		{
			TabChange();
		}
	}
}

CIoViewBase* CIoViewDaDanPaiHang::GetCurTabIoView()
{
	const int32 iTabCur = m_wndGuiTab.GetCurtab();
	if ( iTabCur == 1 )
	{
		return &m_wndRank;
	}
	else
	{
		return &m_wndStatistic;		// 其余任何情况返回默认
	}
}

void CIoViewDaDanPaiHang::TabChange()
{
	if ( m_wndGuiTab.GetCurtab() == 1 )
	{
		m_wndRank.ShowWindow(SW_SHOW);
		m_wndRank.OnIoViewActive();
		m_wndStatistic.OnIoViewDeactive();
		m_wndStatistic.ShowWindow(SW_HIDE);
		m_xsbHorzStatistic.ShowWindow(SW_HIDE);
		m_wndRank.ShowOrHideXHorzBar();
		m_wndRank.RefreshView(m_iCurrentPeriod);
	}
	else
	{
		m_wndRank.OnIoViewDeactive();
		m_wndRank.ShowWindow(SW_HIDE);
		m_wndStatistic.ShowWindow(SW_SHOW);
		m_wndStatistic.OnIoViewActive();
		//m_xsbHorzStatistic.ShowWindow(SW_SHOW);
		m_wndStatistic.ShowOrHideXHorzBar();
		m_xsbHorzRank.ShowWindow(SW_HIDE);
		m_wndStatistic.RefreshView(m_iCurrentPeriod);
	}

	Invalidate();
}

void CIoViewDaDanPaiHang::OnSelChange( NMHDR *pHdr, LRESULT *pResult )
{
	TabChange();
}

CMerch * CIoViewDaDanPaiHang::GetMerchXml()
{
	CIoViewBase *pSubIoView = GetCurTabIoView();
	if ( NULL != pSubIoView )
	{
		return pSubIoView->GetMerchXml();
	}
	return CIoViewBase::GetMerchXml();
}

BOOL CIoViewDaDanPaiHang::PreTranslateMessage( MSG* pMsg )
{
	//return CControlBase::PreTranslateMessage(pMsg);		// 越过ioviewbase
	return CIoViewBase::PreTranslateMessage(pMsg);
}

void CIoViewDaDanPaiHang::ChangeColorFont()
{
	// 更改子视图的颜色与字体配置
	m_wndGuiTab.SetBkGround(false, GetIoViewColor(ESCBackground), 0, 0);
	
	memcpyex(m_wndRank.m_aIoViewColor,		m_aIoViewColor, sizeof(m_aIoViewColor));
	memcpyex(m_wndStatistic.m_aIoViewColor, m_aIoViewColor, sizeof(m_aIoViewColor));
	memcpyex(m_wndRank.m_aIoViewFont, m_aIoViewFont, sizeof(m_aIoViewFont));
	memcpyex(m_wndStatistic.m_aIoViewFont, m_aIoViewFont, sizeof(m_aIoViewFont));
	COLORREF clrBk = GetIoViewColor(ESCBackground);
	COLORREF clrBk1 = RGB(88, 0, 0);
	COLORREF clrBk2 = RGB(128,128,128);
	COLORREF clrText1 = CLR_DEFAULT;
	COLORREF clrText2 = CLR_DEFAULT;
	if ( clrBk != CFaceScheme::Instance()->GetSysColor(ESCBackground) )
	{
		// 灰度
		CColorStep step;
		step.InitColorRange(clrBk, 32.0f, 32.0f, 32.0f);
		clrBk1 = step.GetColor(1);
		clrBk2 = step.GetColor(2);
		
		clrText2 = GetIoViewColor(ESCText);
		step.InitColorRange(clrText1, 40.0f, 40.0f, 40.0f);
		clrText1 = step.GetColor(2);
	}
	
	m_wndGuiTab.SetTabBkColor(RGB(57, 60, 67), RGB(57, 60, 67));
	m_wndGuiTab.SetTabTextColor(clrText1, clrText2);
}

CMerch * CIoViewDaDanPaiHang::GetNextMerch( CMerch* pMerchNow, bool32 bPre )
{
	CIoViewBase *pActive = GetCurTabIoView();
	if ( NULL != pActive )
	{
		return pActive->GetNextMerch(pMerchNow, bPre);
	}
	return CIoViewBase::GetNextMerch(pMerchNow, bPre);
}

void CIoViewDaDanPaiHang::OnEscBackFrameMerch( CMerch *pMerch )
{
	CIoViewBase *pActive = GetCurTabIoView();
	if ( NULL != pActive )
	{
		pActive->OnEscBackFrameMerch(pMerch);
	}
	else
	{
		CIoViewBase::OnEscBackFrameMerch(pMerch);
	}
}

BOOL CIoViewDaDanPaiHang::OnCommand( WPARAM wParam, LPARAM lParam )
{
	int32 iID = (int32)wParam;
	std::map<int, CNCButton>::iterator itHome = m_mapBtnCycleList.find(iID);
	if (m_mapBtnCycleList.end() != itHome)
	{
		switch(iID)
		{
		case  MID_BUTTON_PAIHANG_ONE:
			m_iCurrentPeriod = 1;
			break;

		case  MID_BUTTON_PAIHANG_THREE:
			m_iCurrentPeriod = 3;
			break;

		case  MID_BUTTON_PAIHANG_FIVE:
			m_iCurrentPeriod = 5;
			break;

		case  MID_BUTTON_PAIHANG_TEN:
			m_iCurrentPeriod = 10;
			break;

		default:
			break;
		}
	}

	const int32 iTabCur = m_wndGuiTab.GetCurtab();
	if ( iTabCur == 1 )
	{
		m_wndRank.RefreshView(m_iCurrentPeriod);
	}
	else
	{
		m_wndStatistic.RefreshView(m_iCurrentPeriod);
	}
	
	return CWnd::OnCommand(wParam, lParam);
}

void CIoViewDaDanPaiHang::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	int iButton = TNCButtonHitTest(point);

	if (INVALID_ID != iButton)
	{
		m_mapBtnCycleList[iButton].LButtonUp();
	}	

	//CIoViewBase::OnLButtonUp(nFlags, point);
}

void CIoViewDaDanPaiHang::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	int iButton = TNCButtonHitTest(point);

	if (INVALID_ID != iButton)
	{
		m_mapBtnCycleList[iButton].LButtonDown();
		

		map<int, CNCButton>::iterator iter;

		for (iter=m_mapBtnCycleList.begin(); iter!=m_mapBtnCycleList.end(); ++iter)
		{
			CNCButton &btnControl = iter->second;

			if (btnControl.GetControlId() == iButton)
			{
				btnControl.SetCheckStatus(TRUE);
			}
			else
			{
				btnControl.SetCheckStatus(FALSE);
			}
		}
	}
	//CIoViewBase::OnLButtonDown(nFlags, point);
}

void CIoViewDaDanPaiHang::AddNCButton( LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption )
{
	CNCButton btnControl;
	btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
	btnControl.SetTextBkgColor(RGB(25,25,25), RGB(146,96,0), RGB(25,25,25));
	btnControl.SetTextFrameColor(RGB(100,100,100), RGB(255,255,255), RGB(255,255,255));
	btnControl.SetTextColor(RGB(190,191,191), RGB(255,255,255), RGB(255,255,255));
	m_mapBtnCycleList[nID] = btnControl;
}

int CIoViewDaDanPaiHang::TNCButtonHitTest( CPoint point )
{
	map<int, CNCButton>::iterator iter;

	for (iter=m_mapBtnCycleList.begin(); iter!=m_mapBtnCycleList.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		if (btnControl.PtInButton(point))
		{
			return btnControl.GetControlId();
		}
	}

	return INVALID_ID;
}
