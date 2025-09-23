#include "StdAfx.h"

#include "DlgReportRank.h"

#include "facescheme.h"
#include "MerchManager.h"
#include "NewMenu.h"
#include "IoViewReportRank.h"
#include "BlockConfig.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
CDlgReportRank *CDlgReportRank::m_spThis = NULL;

IMPLEMENT_DYNCREATE(CDlgReportRank, CDialogEx)


CDlgReportRank::CDlgReportRank( CWnd *pParent /*= NULL*/ )
:CDialogEx(IDD, pParent)
{
	m_iBlockId = CBlockConfig::GetDefaultMarketlClassBlockPseudoId();
	CBlockConfig::Instance()->GetDefaultMarketClassBlockId(m_iBlockId);

	m_RectTitle.SetRectEmpty();
}


void CDlgReportRank::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgReportRank)
	//}}AFX_DATA_MAP
}

BOOL CDlgReportRank::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(_T("综合排名"));

	ASSERT( m_aSubIoViews.GetSize() == 0 );

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
	ASSERT( aRankTypes.GetSize() == MAX_REPORTRANK_COUNT );
	
	// 创建n个小排行窗口
	for ( int i=0; i < MAX_REPORTRANK_COUNT ; i++ )
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
	//
	return FALSE;
}

BEGIN_MESSAGE_MAP(CDlgReportRank, CDialogEx)
//{{AFX_MSG_MAP(CDlgReportRank)
ON_WM_GETMINMAXINFO()
ON_WM_SIZE()
ON_WM_DESTROY()
ON_WM_ERASEBKGND()
ON_WM_CONTEXTMENU()
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDlgReportRank::OnSize( UINT nType, int cx, int cy )
{
	CDialogEx::OnSize(nType, cx, cy);

	RecalcLayout();
	

	Invalidate(TRUE);		// dlg 和 static都没有CS_HREDRAW...
}

void CDlgReportRank::RecalcLayout()
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
	ASSERT( m_aSubIoViews.GetSize() == MAX_REPORTRANK_COUNT );

	

	// 需要留出一个画线的位置
	rc.top = m_RectTitle.bottom + 1;	// 留一个画线
	const int32 iWidth = (rc.Width()-2)/3;
	const int32 iHeight = (rc.Height()-2)/3;
	for ( int y=0; y < 3 ; y++ )
	{
		int32 iY = rc.top + y*(iHeight+1);
		int32 iYHeight = iHeight;
		if ( 2== y )
		{
			iYHeight = rc.Height() - y*(iHeight+1);
		}
		for ( int x=0; x < 3 ; x++ )
		{
			const int32 iPos = y*3 + x;
			CIoViewReportRank *pRank;
			if ( iPos < m_aSubIoViews.GetSize() && (pRank=m_aSubIoViews[iPos])!=NULL )
			{
				int32 iX = rc.left + (iWidth+1)*x;
				int32 iXWidth = iWidth;
				if ( 2 == x )
				{
					iXWidth = rc.Width() - x*(iWidth+1);
				}

				pRank->MoveWindow(iX, iY, iXWidth, iYHeight, TRUE);
			}
			else
			{
				ASSERT( 0 );
			}
		}
	}
}

void CDlgReportRank::PostNcDestroy()
{
	if ( m_spThis == this )
	{
		m_spThis = NULL;
	}
	for ( int i=0; i < m_aSubIoViews.GetSize() ; i++ )
	{
		delete m_aSubIoViews[i];
	}
	m_aSubIoViews.RemoveAll();
	delete this;
}

void CDlgReportRank::OnDestroy()
{
	CDialogEx::OnDestroy();
}

void CDlgReportRank::OnPaint()
{
	CPaintDC dc(this);
	dc.SaveDC();

	COLORREF clrTitle = RGB(255, 255, 0);
	COLORREF clrAxis = CFaceScheme::Instance()->GetSysColor(ESCChartAxisLine);
	dc.SelectObject(CFaceScheme::Instance()->GetSysFontObject(ESFNormal));
	dc.SetTextColor(clrTitle);
	dc.FillSolidRect(m_RectTitle, RGB(0,0,0));

	CString StrTitle;
	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(m_iBlockId);
	if ( NULL != pBlock )
	{
		StrTitle.Format(_T("综合排名 - %s"), pBlock->m_blockInfo.m_StrBlockName.GetBuffer());
	}
	else
	{
		StrTitle = _T("综合排名");
	}
	dc.DrawText(StrTitle, m_RectTitle, DT_CENTER |DT_SINGLELINE |DT_VCENTER);
	
	CPen penAxis;
	penAxis.CreatePen(PS_SOLID, 0, clrAxis);
	dc.SelectObject(&penAxis);
	dc.MoveTo(m_RectTitle.left, m_RectTitle.bottom);
	dc.LineTo(m_RectTitle.BottomRight());

	if ( m_aSubIoViews.GetSize() <= 0 )
	{
		dc.RestoreDC(-1);
		return;
	}

	ASSERT( m_aSubIoViews.GetSize() == MAX_REPORTRANK_COUNT );
	
	
	CRect rc(0,0,0,0);
	GetClientRect(rc);
	
	// 需要留出一个画线的位置
	rc.top = m_RectTitle.bottom + 1;	// 留一个画线
	const int32 iWidth = (rc.Width()-2)/3;
	const int32 iHeight = (rc.Height()-2)/3;
	for ( int y=0; y < 3 ; y++ )
	{
		int32 iY = rc.top + y*iHeight + y -1;// = (y-1)*(iHeight+1) + iHeight ;

		dc.MoveTo(rc.left, iY);
		dc.LineTo(rc.right, iY);
	}

	for ( int x=0; x < 3 ; x++ )
	{
		int32 iX = rc.left + (iWidth)*x +x -1; // = (x-1)(iWidth+1) + iWidth;
		
		dc.MoveTo(iX, rc.top);
		dc.LineTo(iX, rc.bottom);
	}

	dc.RestoreDC(-1);
}

BOOL CDlgReportRank::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;	
}

void CDlgReportRank::OnCancel()
{
	// 销毁还是隐藏呢？
	DestroyWindow();
}

void CDlgReportRank::OnContextMenu( CWnd* pWnd, CPoint pos )
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
			else
			{
				ASSERT( 0 );
			}
		}
		else
		{
			ASSERT( 0 );
		}
	}
}

void CDlgReportRank::OnGetMinMaxInfo( MINMAXINFO* lpMMI )
{
	if ( NULL != lpMMI )
	{
		lpMMI->ptMinTrackSize.x = 640;
		lpMMI->ptMinTrackSize.y = 480;
	}
}

bool32 CDlgReportRank::OpenBlock( int32 iBlockId )
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
			m_spThis->Invalidate(TRUE);
		}
	}

	return true;
}

int32 CDlgReportRank::ShowPopupReportRank( int32 iBlockId )
{
    if ( NULL == m_spThis )
    {
        m_spThis = new CDlgReportRank(AfxGetMainWnd());
        m_spThis->Create(IDD, AfxGetMainWnd());
        m_spThis->CenterWindow();
        CRect rect(275, 100, 1075, 700);
        m_spThis->MoveWindow(&rect);
        m_spThis->ShowWindow(SW_SHOW);
        m_spThis->UpdateWindow();
    }
    if ( m_spThis != NULL )
    {
        CRect rect(275, 100, 1075, 700);
        m_spThis->MoveWindow(&rect);
        m_spThis->ShowWindow(SW_SHOW);
        m_spThis->OpenBlock(iBlockId);
        return m_spThis->m_iBlockId;
    }

	return CBlockConfig::GetDefaultMarketlClassBlockPseudoId();
}

int32 CDlgReportRank::ShowPopupReportRank( const CString &StrBlockName )
{
	int32 iRet = CBlockConfig::GetDefaultMarketlClassBlockPseudoId();
	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(StrBlockName);
	if ( NULL != pBlock )
	{
		iRet = pBlock->m_blockInfo.m_iBlockId;
	}
	return ShowPopupReportRank(iRet);
}

BOOL CDlgReportRank::PreTranslateMessage( MSG* pMsg )
{
	const int32 iRankColumnCount = 3;
	if ( WM_KEYDOWN == pMsg->message )
	{
		CIoViewReportRank *pRank = NULL;
		CWnd *pWnd = GetFocus();
		if ( pWnd != NULL )
		{
			pRank = DYNAMIC_DOWNCAST(CIoViewReportRank, pWnd->GetParent());
		}
		int32 iRank = 0;
		if ( NULL == pRank && m_aSubIoViews.GetSize() > 0 )
		{
			pRank = m_aSubIoViews[0];
		}
		else
		{
			for ( int32 i=0; i < m_aSubIoViews.GetSize() ; i++ )
			{
				if ( m_aSubIoViews[i] == pRank )
				{
					iRank = i;
					break;
				}
			}
		}
		if ( NULL != pRank )
		{
			ASSERT( iRank >=0 && iRank < m_aSubIoViews.GetSize() );
			int32 iRowCountOld, iColumnCountOld;
			int32 iRow = 0, iColumn = 0;
			bool32 bValid = false;
			if ( pRank->GetCellCount(iRowCountOld, iColumnCountOld) )
			{
				bValid = true;
				if ( !pRank->GetFocusCell(iRow, iColumn) )
				{
					iRow = 0;
					iColumn = 0;
				}
			}

			if ( !bValid )
			{
				return CDialogEx::PreTranslateMessage(pMsg);
			}

			bool32 bEatMsg = false;
			
			int iRankRow = iRank/iRankColumnCount;
			int iRankColumn = iRank%iRankColumnCount;
			const int32 iRankRowCount = (m_aSubIoViews.GetSize()+iRankColumnCount-1)/iRankColumnCount;
			if ( VK_DOWN == pMsg->wParam )
			{
				// 一个一个循环向下
				++iRow;
				if ( iRow >= iRowCountOld )	// 表格内的让表格自己处理，外的则转接到另外的视图
				{
					int32 iOldRank = iRank;
					do
					{
						++iRankRow;
						if ( iRankRow >= iRankColumnCount )
						{
							++iRankColumn;
							iRankColumn = iRankColumn % iRankColumnCount;	// 另起一列
							iRankRow = 0;
						}
						iRank = (iRankRow*iRankColumnCount) + iRankColumn;
						ASSERT( iRank < m_aSubIoViews.GetSize() );
						if ( iRank == iOldRank )
						{
							break;
						}

						int32 iNewR, iNewC;
						if ( m_aSubIoViews[iRank]->GetCellCount(iNewR, iNewC)
							&& iNewR > 0 )
						{
							bEatMsg = m_aSubIoViews[iRank]->SetFocusCell(0, iColumn, true);
						}
					}while( !bEatMsg );	// 设置成功或者又返回原点了
				}
			}
			else if ( VK_UP == pMsg->wParam )
			{
				--iRow;
				if ( iRow < 0 )
				{
					int32 iOldRank = iRank;
					do
					{
						--iRankRow;
						if ( iRankRow < 0 )
						{
							--iRankColumn;
							iRankColumn = (iRankColumn + iRankColumnCount) % iRankColumnCount;	// 另起一列
							iRankRow = iRankRowCount-1;
						}
						iRank = (iRankRow*iRankColumnCount) + iRankColumn;
						ASSERT( iRank < m_aSubIoViews.GetSize() );
						if ( iRank == iOldRank )
						{
							break;
						}

						int32 iNewR, iNewC;
						if ( m_aSubIoViews[iRank]->GetCellCount(iNewR, iNewC)
							&& iNewR > 0 )
						{
							bEatMsg = m_aSubIoViews[iRank]->SetFocusCell(iNewR-1, iColumn, true);
						}
					} while( !bEatMsg );
				}
			}
			else if ( VK_LEFT == pMsg->wParam )
			{
				//--iColumn;
				//if ( iColumn < 0 )
				{
					int32 iOldRank = iRank;
					do 
					{
						--iRankColumn;
						iRankColumn = (iRankColumn+iRankColumnCount) % iRankColumnCount;
						iRank = iRankColumnCount*iRankRow + iRankColumn;
						if ( iRank == iOldRank )
						{
							break;
						}
						int32 iNewR, iNewC;
						if ( m_aSubIoViews[iRank]->GetCellCount(iNewR, iNewC)
							&& iNewR > 0 )
						{
							//bEatMsg = m_aSubIoViews[iRank]->SetFocusCell(iRow<iNewR?iRow :iNewR-1, iNewC-1, true);
							bEatMsg = m_aSubIoViews[iRank]->SetFocusCell(iRow<iNewR?iRow :iNewR-1, iColumn, true);
						}
					} while ( !bEatMsg );
				}
			}
			else if ( VK_RIGHT == pMsg->wParam )
			{
				//++iColumn;
				//if ( iColumn >= iColumnCountOld )
				{
					int32 iOldRank = iRank;
					do 
					{
						++iRankColumn;
						iRankColumn = iRankColumn % iRankColumnCount;
						iRank = iRankColumnCount*iRankRow + iRankColumn;
						if ( iRank == iOldRank )
						{
							break;
						}
						int32 iNewR, iNewC;
						if ( m_aSubIoViews[iRank]->GetCellCount(iNewR, iNewC)
							&& iNewR > 0 )
						{
							//bEatMsg = m_aSubIoViews[iRank]->SetFocusCell(iRow<iNewR?iRow :iNewR-1, 0, true);
							bEatMsg = m_aSubIoViews[iRank]->SetFocusCell(iRow<iNewR?iRow :iNewR-1, iColumn, true);
						}
					} while ( !bEatMsg );
				}
			}
			else if ( VK_RETURN == pMsg->wParam )
			{
				// 回车键打开商品
				pRank->OnDblClick();
				BringWindowToTop();
				bEatMsg = true;
			}

			if ( bEatMsg )
			{
				return TRUE;
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
