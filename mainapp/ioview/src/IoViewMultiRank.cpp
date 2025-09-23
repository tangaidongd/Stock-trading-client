#include "stdafx.h"
#include "IoViewManager.h"
#include "IoViewMultiRank.h"
#include "XmlShare.h"

#define  RANK_NUMBER      9
#define  SPLITER_LINE_WIDTH 1

IMPLEMENT_DYNCREATE(CIoViewMultiRank, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewMultiRank, CIoViewBase)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_CREATE()
    ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

CIoViewMultiRank::CIoViewMultiRank()
{
	m_iBlockId = CBlockConfig::GetDefaultMarketlClassBlockPseudoId();
	CBlockConfig::Instance()->GetDefaultMarketClassBlockId(m_iBlockId);

	m_RectTitle.SetRectEmpty();
}

CIoViewMultiRank::~CIoViewMultiRank()
{
	for ( int i=0; i < m_aSubIoViews.GetSize() ; i++ )
	{
		delete m_aSubIoViews[i];
	}
	m_aSubIoViews.RemoveAll();
}

int CIoViewMultiRank::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    int iRet = CIoViewBase::OnCreate(lpCreateStruct);
    if(-1 == iRet)
    {
        return -1;
    }

    InitialIoViewFace(this);

    return iRet;
}

void CIoViewMultiRank::CreateRankView()
{	
	CArray<int, int> aRankTypes;
  	aRankTypes.Add(CIoViewReportRank::ERT_RisePercent);
	aRankTypes.Add(CIoViewReportRank::ERT_5MinRiseSpeed);
	aRankTypes.Add(CIoViewReportRank::ERT_BidRatioFirst);

	aRankTypes.Add(CIoViewReportRank::ERT_FallPercent);
	aRankTypes.Add(CIoViewReportRank::ERT_5MinFallSpeed);
	aRankTypes.Add(CIoViewReportRank::ERT_BidRatioLast);

	aRankTypes.Add(CIoViewReportRank::ERT_Amplitude);
	aRankTypes.Add(CIoViewReportRank::ERT_VolRatio);
	aRankTypes.Add(CIoViewReportRank::ERT_Amout);
	ASSERT( aRankTypes.GetSize() == RANK_NUMBER );

	// 创建n个小排行窗口
	for ( int i=0; i < RANK_NUMBER ; i++ )
	{
		CIoViewReportRank *pRank = new CIoViewReportRank();
		if ( NULL != pRank )
		{
			BOOL b = pRank->Create(WS_VISIBLE |WS_CHILD |SS_NOTIFY, this, CRect(0,0,0,0), 1000+i);
			ASSERT( b );
			pRank->SetShowBlockNameOnTitle(false);
			pRank->ChangeRank((CIoViewReportRank::E_RankType)aRankTypes[i]);
		}
		m_aSubIoViews.Add(pRank);
	}
	m_aSubIoViews[0]->OnIoViewActive();

	RecalcLayout();
}

void CIoViewMultiRank::OnSize( UINT nType, int cx, int cy )
{
    CIoViewBase::OnSize(nType, cx, cy);

    RecalcLayout();

    RedrawWindow();             // 立即更新, 避免子窗口刷新占用太长时间导致自己显示不全
}

void CIoViewMultiRank::RecalcLayout()
{
	CRect rc(0,0,0,0);
	GetClientRect(rc);

	m_RectTitle = rc;

	CClientDC dcFont(this);
	CFont *pFontOld = dcFont.SelectObject(CFaceScheme::Instance()->GetSysFontObject(ESFNormal));
	CSize sizeFont = dcFont.GetTextExtent(_T("测试高度"));
	dcFont.SelectObject(pFontOld);
	m_RectTitle.bottom = m_RectTitle.top + sizeFont.cy + 6;

	if ( m_aSubIoViews.GetSize() <= 0 )
	{
		return;
	}

	// 需要留出一个画线的位置
	rc.top =  m_RectTitle.bottom + SPLITER_LINE_WIDTH;	// 留一个画线
	const int32 iWidth = (rc.Width()-2 * SPLITER_LINE_WIDTH)/3;
	const int32 iHeight = (rc.Height()-1 * SPLITER_LINE_WIDTH)/3;
	for ( int y=0; y < 3 ; y++ )
	{
		int32 iY = rc.top + y*(iHeight+SPLITER_LINE_WIDTH);
		int32 iYHeight = iHeight;
		if ( 2== y )
		{
			iYHeight = rc.Height() - y*(iHeight+SPLITER_LINE_WIDTH);
		}
		for ( int x=0; x < 3 ; x++ )
		{
			const int32 iPos = y*3 + x;
			CIoViewReportRank *pRank;
			if ( iPos < m_aSubIoViews.GetSize() && (pRank=m_aSubIoViews[iPos])!=NULL )
			{
				int32 iX = rc.left + (iWidth+SPLITER_LINE_WIDTH)*x;
				int32 iXWidth = iWidth;
				if ( 2 == x )
				{
					iXWidth = rc.Width() - x*(iWidth+SPLITER_LINE_WIDTH);
				}

				pRank->MoveWindow(iX, iY, iXWidth, iYHeight, TRUE);
			}
		}
	}
}

void CIoViewMultiRank::OnPaint()
{
	CPaintDC dc(this);
	dc.SaveDC();

	COLORREF clrTitle = RGB(255, 212, 79);
	COLORREF clrAxis = CFaceScheme::Instance()->GetSysColor(ESCChartAxisLine);
	dc.SelectObject(CFaceScheme::Instance()->GetSysFontObject(ESFNormal));
	dc.SetTextColor(clrTitle);
	dc.FillSolidRect(m_RectTitle, RGB(42,43,45));

	CString StrTitle;
	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(m_iBlockId);
	if ( NULL != pBlock )
	{
		StrTitle.Format(_T("综合排名 - %s"), pBlock->m_blockInfo.m_StrBlockName);
	}
	else
	{
		StrTitle = _T("综合排名");
	}
	dc.DrawText(StrTitle, m_RectTitle, DT_CENTER |DT_SINGLELINE |DT_VCENTER);

	CPen penAxis;
	penAxis.CreatePen(PS_SOLID, 1, RGB(83,83,83));
	dc.SelectObject(&penAxis);
	dc.MoveTo(m_RectTitle.left, m_RectTitle.bottom);
	dc.LineTo(m_RectTitle.BottomRight());

	CRect rc(0,0,0,0);
	GetClientRect(rc);

	// 需要留出一个画线的位置
	rc.top =  m_RectTitle.bottom;	// 留一个画线
	const int32 iWidth = (rc.Width()-2*SPLITER_LINE_WIDTH)/3;
	const int32 iHeight = (rc.Height()-2*SPLITER_LINE_WIDTH)/3;
	for ( int y=0; y < 3 ; y++ )
	{
		int32 iY = rc.top + y*iHeight + y * SPLITER_LINE_WIDTH;
		dc.MoveTo(rc.left, iY);
		dc.LineTo(rc.right, iY);
	}

	for ( int x=0; x < 3 ; x++ )
	{
		int32 iX = 0;
		if(x == 0)
		{
			iX = 0;
		}
		else if(x == 1)
		{
			iX = iWidth;
		}
		else
		{
			iX = iWidth * 2 + SPLITER_LINE_WIDTH;
		}

		dc.MoveTo(iX, rc.top);
		dc.LineTo(iX, rc.bottom);
	}

	dc.RestoreDC(-1);
}

BOOL CIoViewMultiRank::OnEraseBkgnd( CDC* pDC )
{
    return TRUE;
}

bool32 CIoViewMultiRank::FromXml( TiXmlElement *pElement )
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

    CreateRankView();

    // 读取本业务视图特有的内容
    SetFontsFromXml(pElement);
    SetColorsFromXml(pElement);

	if(m_aSubIoViews.GetSize() > 0)
	{
		RecalcLayout();
		RedrawWindow();
	}

    return true;
}

CString CIoViewMultiRank::ToXml()
{
    CString StrThis;

    StrThis.Format(L"<%s %s=\"%s\" ",
        CString(GetXmlElementValue()), 
        CString(GetXmlElementAttrIoViewType()), 
        CIoViewManager::GetIoViewString(this)
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

bool32 CIoViewMultiRank::GetStdMenuEnable( MSG* pMsg )
{
    return false;
}

CMerch * CIoViewMultiRank::GetMerchXml()
{
    return CIoViewBase::GetMerchXml();
}

void CIoViewMultiRank::OnContextMenu( CWnd* pWnd, CPoint pos )
{
	CPoint posScreen = pos;
	CRect rcWindow;
	GetWindowRect(rcWindow);
	if ( !rcWindow.PtInRect(posScreen) )
	{
		posScreen = rcWindow.TopLeft();
	}
	CBlockConfig::BlockArray aBlockPhys;
	CBlockConfig::Instance()->GetMarketClassBlocks(aBlockPhys);	// 可能以后要过滤掉一些板块
	int32 i=0;
	for ( i=0; i < aBlockPhys.GetSize() ; i++ )
	{
		if ( aBlockPhys[i]->m_blockInfo.m_iBlockId == m_iBlockId )
		{
			aBlockPhys.RemoveAt(i);
			break;
		}
	}

	// 菜单板块
	int iCmd = 1;
	const int iMarketCmdBase = 10;
	const int iRankCmdBase = 1000;
	CNewMenu menuMarket;
	menuMarket.CreatePopupMenu();

	iCmd = iMarketCmdBase;
	for ( i=0; i < aBlockPhys.GetSize() ; i++ )
	{
		menuMarket.AppendMenu(MF_STRING, iCmd++, aBlockPhys[i]->m_blockInfo.m_StrBlockName);
	}

	int iRet = menuMarket.TrackPopupMenu(TPM_TOPALIGN |TPM_LEFTALIGN |TPM_NONOTIFY |TPM_RETURNCMD, posScreen.x, posScreen.y, AfxGetMainWnd());
	if ( iRet > 0 )
	{
		if ( iRet < iRankCmdBase )
		{
			iRet -= iMarketCmdBase;
			if ( iRet < aBlockPhys.GetSize() )
			{
				OpenBlock(aBlockPhys[iRet]->m_blockInfo.m_iBlockId);
			}
		}
	}
}

bool32 CIoViewMultiRank::OpenBlock( int32 iBlockId )
{
	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iBlockId);
	if ( pBlock != NULL )
	{
		m_iBlockId = iBlockId;
	}
	else
	{
		return false;
	}

	for ( int32 i=0; i < m_aSubIoViews.GetSize() ; i++ )
	{
		CIoViewReportRank *pRank = m_aSubIoViews[i];
		if ( NULL != pRank )
		{
			pRank->OpenBlock(m_iBlockId);
			RedrawWindow();
		}
	}

	return true;
}