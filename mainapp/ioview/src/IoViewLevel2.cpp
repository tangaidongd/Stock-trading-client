#include "stdafx.h"
#include "memdc.h"
#include "GridCellSymbol.h"
#include "GridCellLevel2.h"
#include "IoViewManager.h"
#include "IoViewLevel2.h"
#include "MerchManager.h"
#include "facescheme.h"
#include "ShareFun.h"
#include "IoViewTimeSale.h"
#include "MPIChildFrame.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CIoViewLevel2, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewLevel2, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewLevel2)
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
ON_WM_CREATE()
ON_WM_SIZE()
ON_MESSAGE(UM_IoViewTitle_Button_LButtonDown,OnMessageTitleButton)
ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewLevel2::CIoViewLevel2()
:CIoViewBase()
{
	m_eMerchKind	= ERTStockHk;
	m_eShowType		= ESTBrokerCode;
	m_bChina		= false;
	m_bHongKong		= false;
	m_bHoerzShow	= false;

	m_iTitleHeight	= -1; 
	m_bTracking		= false;
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewLevel2::~CIoViewLevel2()
{

}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CIoViewLevel2::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

///////////////////////////////////////////////////////////////////////////////
// Draw
void CIoViewLevel2::Draw()
{
	
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonDown
void CIoViewLevel2::OnLButtonDown(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonUp
void CIoViewLevel2::OnLButtonUp(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonUp(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnMouseMove
void CIoViewLevel2::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( !m_bTracking )
	{
		// 注册鼠标离开的事件标志
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd; 
		tme.dwFlags = TME_LEAVE ;
		tme.dwHoverTime = 1;
		
		m_bTracking = _TrackMouseEvent(&tme);	
	}

	CStatic::OnMouseMove(nFlags, point);
}

LRESULT CIoViewLevel2::OnMouseLeave(WPARAM wParam,LPARAM lParam)
{
	m_bTracking = false;
	if ( m_GridCtrlSell.GetSafeHwnd() && m_GridCtrlBuy.GetSafeHwnd())
	{
		m_GridCtrlBuy.m_TipWnd.Hide();
		m_GridCtrlSell.m_TipWnd.Hide();
	}
	return 0;
}

int CIoViewLevel2::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CIoViewBase::OnCreate(lpCreateStruct);
	
	InitialIoViewFace(this);

	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
	m_XSBVert.SetScrollRange(0, 10);
	m_XSBVert.ShowWindow(SW_HIDE);
	
	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
	m_XSBHorz.SetScrollRange(0, 0);
	m_XSBHorz.ShowWindow(SW_HIDE);		

	// 创建当前商品对应的表格
	E_ReportType eMerchKind= GetMerchKind(m_pMerchXml);

 	CreateTable(eMerchKind);
	
	OnVDataRealtimeLevel2Update(m_pMerchXml);
 
	return 0;
}

void CIoViewLevel2::OnIoViewFontChanged()
{
	CIoViewBase::OnIoViewFontChanged();
	SetRowHeightAccordingFont();
}

void CIoViewLevel2::SetRowHeightAccordingFont()
{
	CFont * pFontBig = GetIoViewFontObject(ESFBig);

	CPaintDC dc(this);
	dc.SelectObject(pFontBig); 
	
	TEXTMETRIC tmst;	
	dc.GetTextMetrics(&tmst);
	m_iTitleHeight = tmst.tmHeight + 2; 
}

void CIoViewLevel2::OnSize(UINT nType, int cx, int cy) 
{
	/*
	CIoViewBase::OnSize(nType, cx, cy);

	if (m_bChina)
	{
		CRect rectGrid(0, m_iTitleHeight, cx, cy);	

		m_GridCtrl.MoveWindow(rectGrid);
		m_GridCtrl.ExpandColumnsToFit();
		m_GridCtrl.ExpandToFit();
	}
	else if (m_bHongKong)
	{
		if (!m_bHoerzShow)
		{		
			// 纵向.
			// 买盘
			CRect rectleftTitle(0,0,cx/2,m_iTitleHeight);
			m_CtrlTitleHongKongleft.MoveWindow(rectleftTitle);
			CString a = m_CtrlTitleHongKongleft.GetTitle();

			// 隐藏,不显示
			CRect rectrightTitle(-1,-1,-1,-1);
			m_CtrlTitleHongKongright.MoveWindow(rectrightTitle);
			//
			m_RectTitle = CRect(cx/2,0,cx,m_iTitleHeight);
			
			if (m_Title.GetSafeHwnd())
			{
				m_Title.SetShowRect(m_RectTitle);
				m_Title.MoveWindow(&m_RectTitle);
				m_Title.ShowWindow(SW_SHOW);
			}

			CRect rect(0, m_iTitleHeight, cx, cy/2);	
			m_GridCtrlBuy.MoveWindow(rect);
			m_GridCtrlBuy.ExpandColumnsToFit();
			m_GridCtrlBuy.ExpandToFit();
			
			// 卖盘
					
			CRect rectSellleftTitle(0,cy/2,cx/2,(cy/2)+m_iTitleHeight);
			m_CtrlTitleHongKongSellleft.MoveWindow(rectSellleftTitle);
			
			CRect  rectSellrightTitle(cx/2,cy/2,cx,(cy/2)+m_iTitleHeight);
			m_CtrlTitleHongKongSellright.MoveWindow(rectSellrightTitle);
			m_CtrlTitleHongKongSellright.ShowWindow(SW_SHOW);

			CRect rectsell(0, m_iTitleHeight+(cy/2), cx, cy);	
			m_GridCtrlSell.MoveWindow(rectsell);
			m_GridCtrlSell.ExpandColumnsToFit();
			m_GridCtrlSell.ExpandToFit();
		}
		else
		{
			// 横向			
		
			CRect rectleftTitle(0,0,cx/4,m_iTitleHeight);
			m_CtrlTitleHongKongleft.MoveWindow(rectleftTitle);
			
			CRect  rectrightTitle(cx/4,0,cx/2,m_iTitleHeight);	
			CRect rectrightdraw = rectrightTitle;
			rectrightdraw.right += 1;							// 对齐中间画线.cell 中不好处理,暂时办法
			m_CtrlTitleHongKongright.MoveWindow(rectrightdraw);
			m_CtrlTitleHongKongright.ShowWindow(SW_SHOW);

			CRect rect(0, m_iTitleHeight, cx/2, cy);	
			m_GridCtrlBuy.MoveWindow(rect);
			m_GridCtrlBuy.ExpandColumnsToFit();
			m_GridCtrlBuy.ExpandToFit();						
			
			// 卖盘		
			CRect rectSellleftTitle(cx/2,0,3*cx/4,m_iTitleHeight);
			CRect rectleftdraw = rectSellleftTitle;
			rectleftdraw.left += 1;								// 对齐中间画线.cell 中不好处理,暂时办法
			m_CtrlTitleHongKongSellleft.MoveWindow(rectleftdraw);
			
			// 这块不显示了,区域
			CRect rectSellRigthTitle(-1,-1,-1,-1);
			m_CtrlTitleHongKongSellright.MoveWindow(rectSellRigthTitle);
			//m_CtrlTitleHongKongSellright.ShowWindow(SW_HIDE);
			//
			m_RectTitle = CRect(cx*3/4 ,0,cx,m_iTitleHeight);

			if (m_Title.GetSafeHwnd())
			{
				m_Title.SetShowRect(m_RectTitle);
				m_Title.MoveWindow(&m_RectTitle);
				m_Title.ShowWindow(SW_SHOW);
			}
						
			CRect rectsell(cx/2, m_iTitleHeight, cx, cy);	
			m_GridCtrlSell.MoveWindow(rectsell);
			m_GridCtrlSell.ExpandColumnsToFit();
			m_GridCtrlSell.ExpandToFit();

			if (m_bHoerzShow)
			{
				for (int32 i = 0;i<m_GridCtrlSell.GetRowCount();i++)
				{
					CGridCellSys * pCell =(CGridCellSys *)m_GridCtrlSell.GetCell(i,0);
					if (pCell)
					{
						pCell->DrawLeftLine(true);
					}
				}
			}

		}
	}
*/	
}

BOOL CIoViewLevel2::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return FALSE;
}

// 通知视图改变关注的商品
void CIoViewLevel2::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch || NULL == pMerch )
	{
		return;
	}

	{
		// 设置关注的商品信息
		m_aSmartAttendMerchs.RemoveAll();
		
		CSmartAttendMerch SmartAttendMerch;
		SmartAttendMerch.m_pMerch = pMerch;
		SmartAttendMerch.m_iDataServiceTypes = EDSTLevel2;
		m_aSmartAttendMerchs.Add(SmartAttendMerch);
	}

	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;

	// 当前商品不为空的时候
	if (NULL != pMerch)
	{
		// 清空当前表格信息
		if (m_GridCtrlSell.GetSafeHwnd() && m_GridCtrlBuy.GetSafeHwnd() && m_GridCtrlBuy.GetRowCount()==m_GridCtrlSell.GetRowCount() && m_GridCtrlBuy.GetColumnCount() == m_GridCtrlSell.GetColumnCount())
		{
			for(int32 i = 0;i<m_GridCtrlBuy.GetRowCount(); i++)
			{	
				for (int32 j = 0; j<m_GridCtrlBuy.GetColumnCount(); j++)
				{				
					m_GridCtrlBuy.SetCellType(i,j,RUNTIME_CLASS(CGridCellSys));
					CGridCellSys * pcellBuy = (CGridCellSys*)m_GridCtrlBuy.GetCell(i,j);				
					pcellBuy->SetText(L"");	
					
					m_GridCtrlSell.SetCellType(i,j,RUNTIME_CLASS(CGridCellSys));
					CGridCellSys * pcellSell = (CGridCellSys *)m_GridCtrlSell.GetCell(i,j);
					pcellSell->SetText(L"");
				}			
			}		
		}
		m_xxxBuyLevel2ListBackUp.RemoveAll();
		m_yyySellLevelListBackUp.RemoveAll();
			
		// 设置关注的商品信息
		m_aSmartAttendMerchs.RemoveAll();

		CSmartAttendMerch SmartAttendMerch;
		SmartAttendMerch.m_pMerch = pMerch;
		SmartAttendMerch.m_iDataServiceTypes = EDSTLevel2;
		m_aSmartAttendMerchs.Add(SmartAttendMerch);
		
		// 创建当前商品对应的表格
		E_ReportType eMerchKind = GetMerchKind(pMerch);
		if (m_eMerchKind != eMerchKind )
		{
			if (CreateTable(eMerchKind))
			{
				// 保存当前表格类型
				m_eMerchKind = eMerchKind;
			}
		}

		// 尝试设置表格内容
		OnVDataRealtimeLevel2Update(pMerch);
	}
	else
	{
		// zhangbo 20090824 #待补充， 当心商品不存在时，清空当前显示数据
		//...
	}
}

void CIoViewLevel2::OnVDataForceUpdate()
{
	if (NULL == m_pMerchXml)
	{
		return;
	}

	CMmiReqRealtimeLevel2 Req;
	Req.m_iMarketId = m_pMerchXml->m_MerchInfo.m_iMarketId;
	Req.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;		
	DoRequestViewData(Req);
}

void CIoViewLevel2::OnVDataRealtimeLevel2Update(IN CMerch *pMerch)
{
	if ( GetParentGGTongViewDragFlag() )
	{
		m_GridCtrl.ShowWindow(SW_HIDE);
		return;
	}
	else
	{
		m_GridCtrl.ShowWindow(SW_SHOW);
	}

	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;

	CRealtimePrice  Realtimeprice;
	CRealtimeLevel2 RealtimeLevel2;
	int iSaveDec = 0;
	
	//
	iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;

	//	
	CRealtimePrice * pRealtimePrice = pMerch->m_pRealtimePrice;	
	if (NULL != pRealtimePrice)
	{
		Realtimeprice = * pRealtimePrice;
	}			
	
	//
	CRealtimeLevel2 * pRealtimeLevel2 = pMerch->m_pRealtimeLevel2;
	if ( NULL!= pRealtimeLevel2)
	{
		RealtimeLevel2 = * pRealtimeLevel2 ;				
	}	
	
	// 获取昨收价	
	//float fPricePrevClose = Realtimeprice.m_fPricePrevClose;
	
	// 根据不同市场类型,表格显示不同内容;
	E_ReportType eMerchKind = GetMerchKind(m_pMerchXml);	//... 测试	
	if ( m_eMerchKind  != eMerchKind )
	{
		if (CreateTable(eMerchKind))
		{
			m_eMerchKind  = eMerchKind;
		}
	}
	//... 传进来的数组,做处理.保证不出错
	
	// 	CLevel2Detail Level2Detail;
	// 	Level2Detail.m_fVolume = - 1.0;
	// 	Level2Detail.m_StrBrokerCode = L"00000";
	// 	Level2Detail.m_StrBrokerName = L"ERROR";
	// 	
	// 	CLevel2Node Level2Node;
	// 	Level2Node.m_fPrice = -1.0;
	// 	Level2Node.m_fVolume = -1.0;
	// 	Level2Node.m_eLevel2Type = CLevel2Node::ELTBuy;
	// 	Level2Node.m_Level2Details.Add(Level2Detail);
	// 	
	// 	if ( RealtimeLevel2.m_Level2SellNodes.GetSize() < 10)
	// 	{
	// 			int32 iSize = RealtimeLevel2.m_Level2SellNodes.GetSize();
	// 
	// 		for ( int32 iIndex = iSize ; iIndex < 10 ; iIndex ++ )
	// 		{
	// 			RealtimeLevel2.m_Level2SellNodes.Add(Level2Node);
	// 		}
	// 	}
	// 
	// 	if ( RealtimeLevel2.m_Level2BuyNodes.GetSize() < 10)
	// 	{
	// 			int32 iSize = RealtimeLevel2.m_Level2BuyNodes.GetSize();
	// 
	// 		for ( int32 iIndex = iSize ; iIndex < 10 ; iIndex ++ )
	// 		{
	// 			RealtimeLevel2.m_Level2BuyNodes.Add(Level2Node);
	// 		}
	// 	}
	// 
	
	if (  ERTStockHk == m_eMerchKind || ERTWarrantHk == m_eMerchKind )
	{
		ASSERT( NULL != m_GridCtrlBuy.GetSafeHwnd());
		ASSERT( NULL != m_GridCtrlSell.GetSafeHwnd());
		
		/////////////////////////////////////////////////
		// 买 1 价  ,暂无
		// 		CString StrPriceBuy1;
		// 		if (iBuySize>0)
		// 		{
		// 			  StrPriceBuy1 = Float2String(RealtimeLevel2.m_xxxBuyLevel2List[0].m_fPrice,iSaveDec,false);
		// 		}				
		//  	m_CtrlTitleHongKongright.SetTitle(StrPriceBuy1);

		int32 iBuySize = RealtimeLevel2.m_xxxBuyLevel2List.GetSize();
		if (iBuySize>0)
		{
			m_xxxBuyLevel2ListBackUp.RemoveAll();
			m_xxxBuyLevel2ListBackUp.Copy(RealtimeLevel2.m_xxxBuyLevel2List);
		}
 		m_CtrlTitleHongKongright.ShowWindow(SW_HIDE);
 		m_CtrlTitleHongKongright.ShowWindow(SW_SHOW);
				
		int32 iCount =0;
		int32  iCol;
		for (iCol = 0; iCol<m_GridCtrlBuy.GetColumnCount(); iCol++)
		{
			for (int32 iRow = 0; iRow < m_GridCtrlBuy.GetRowCount(); iRow ++)
			{
				if (iCount<iBuySize)
				{
					m_GridCtrlBuy.SetCellType(iRow,iCol,RUNTIME_CLASS(CGridCellLevel2));
					CGridCellLevel2 * pCell = (CGridCellLevel2 *)m_GridCtrlBuy.GetCell(iRow, iCol);
					
					pCell->SetState(pCell->GetState() | GVIS_SHOWTIPS);	

					if (!RealtimeLevel2.m_xxxBuyLevel2List[iCount].m_bPos)
					{
						pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
 						pCell->SetContent( RealtimeLevel2.m_xxxBuyLevel2List[iCount].m_StrBrokerCode,RealtimeLevel2.m_xxxBuyLevel2List[iCount].m_StrBrokerName,GetShowType()); 
					}
					else
					{
						m_GridCtrlBuy.SetCellType(iRow,iCol,RUNTIME_CLASS(CGridCellSys));
						CGridCellSys* pCellPos = (CGridCellSys*)m_GridCtrlBuy.GetCell(iRow, iCol);
						pCellPos->SetState(pCellPos->GetState() & ~GVIS_SHOWTIPS);
						pCellPos->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
						pCellPos->SetDefaultTextColor(ESCKLineRise);
						pCellPos->SetText(RealtimeLevel2.m_xxxBuyLevel2List[iCount].m_StrPos);

					}
					iCount ++;
				}
			}
		}
		m_GridCtrlBuy.Refresh();

		//////////////////////////////////////////////////////////////////////////
		// 卖盘
		// 		CString StrPriceSell1;
		// 		if (iSellSize>0)
		// 		{
		// 			StrPriceSell1 = Float2String(RealtimeLevel2.m_yyySellLevel2List[0].m_fPrice,iSaveDec,false);
		// 		}
		// 		
		// 		m_CtrlTitleHongKongright.SetTitle(StrPriceSell1);

	
		int32 iSellSize = RealtimeLevel2.m_yyySellLevel2List.GetSize();		
		if (iSellSize>0)
		{
			m_yyySellLevelListBackUp.RemoveAll();
			m_yyySellLevelListBackUp.Copy(RealtimeLevel2.m_yyySellLevel2List);
		}
		m_CtrlTitleHongKongright.ShowWindow(SW_HIDE);
		m_CtrlTitleHongKongright.ShowWindow(SW_SHOW);		

		iCount =0;
		
		for ( iCol = 0; iCol<m_GridCtrlSell.GetColumnCount(); iCol++)
		{
			for (int32 iRow = 0; iRow < m_GridCtrlSell.GetRowCount(); iRow ++)
			{
				if (iCount<iSellSize)
				{
					m_GridCtrlSell.SetCellType(iRow,iCol,RUNTIME_CLASS(CGridCellLevel2));
					CGridCellLevel2 * pCell = (CGridCellLevel2 *)m_GridCtrlSell.GetCell(iRow, iCol);
					
					pCell->SetState(pCell->GetState() | GVIS_SHOWTIPS);	

					if (!RealtimeLevel2.m_yyySellLevel2List[iCount].m_bPos)
					{
						pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
						pCell->SetContent( RealtimeLevel2.m_yyySellLevel2List[iCount].m_StrBrokerCode,RealtimeLevel2.m_yyySellLevel2List[iCount].m_StrBrokerName,GetShowType()); 
					}
					else
					{
						m_GridCtrlSell.SetCellType(iRow,iCol,RUNTIME_CLASS(CGridCellSys));
						CGridCellSys* pCellPos = (CGridCellSys*)m_GridCtrlSell.GetCell(iRow, iCol);
						pCellPos->SetState(pCellPos->GetState() & ~GVIS_SHOWTIPS);
						pCellPos->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
						pCellPos->SetDefaultTextColor(ESCKLineFall);
						pCellPos->SetText(RealtimeLevel2.m_yyySellLevel2List[iCount].m_StrPos);

						
					}
					iCount ++;
				}
			}
		}
 		if (m_bHoerzShow)
 		{
 			for (int32 i = 0;i<m_GridCtrlSell.GetRowCount();i++)
 			{
 				CGridCellSys * pCell =(CGridCellSys *)m_GridCtrlSell.GetCell(i,0);
 				if (pCell)
 				{
 					pCell->DrawLeftLine(true);
 				}
 			}
 		}

		m_GridCtrlSell.Refresh();	
	}
	else
	{
		ASSERT(NULL != m_GridCtrl.GetSafeHwnd());
		// 		/////////////////////////////////////////////////
		// 		// 卖10 价 
		// 		m_GridCtrl.SetCellType(0, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		CGridCellSymbol *pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(0, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceSell10 = Float2SymbolString(RealtimeLevel2.m_Level2SellNodes[9].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceSell10);
		// 		
		// 		/////////////////////////////////////////////////
		// 		//  卖9 
		// 		m_GridCtrl.SetCellType(1, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(1, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceSell9 = Float2SymbolString(RealtimeLevel2.m_Level2SellNodes[8].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceSell9);
		// 
		// 		/////////////////////////////////////////////////
		// 		//  卖8
		// 		m_GridCtrl.SetCellType(2, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(2, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);		
		// 		CString StrPriceSell8 = Float2SymbolString(RealtimeLevel2.m_Level2SellNodes[7].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceSell8);
		// 
		// 		/////////////////////////////////////////////////
		// 		//  卖7
		// 		m_GridCtrl.SetCellType(3, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(3, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceSell7 = Float2SymbolString(RealtimeLevel2.m_Level2SellNodes[6].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceSell7);
		// 
		// 		/////////////////////////////////////////////////
		// 		//  卖6
		// 		m_GridCtrl.SetCellType(4, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(4, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceSell6 = Float2SymbolString(RealtimeLevel2.m_Level2SellNodes[5].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceSell6);
		// 		
		// 		/////////////////////////////////////////////////
		// 		//  卖5
		// 		m_GridCtrl.SetCellType(5, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(5, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);		
		// 		CString StrPriceSell5 = Float2SymbolString(RealtimeLevel2.m_Level2SellNodes[4].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceSell5);
		// 		
		// 		/////////////////////////////////////////////////
		// 		//  卖4
		// 		m_GridCtrl.SetCellType(6, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(6, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceSell4 = Float2SymbolString(RealtimeLevel2.m_Level2SellNodes[3].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceSell4);
		// 
		// 		/////////////////////////////////////////////////
		// 		//  卖3
		// 		m_GridCtrl.SetCellType(7, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(7, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceSell3 = Float2SymbolString(RealtimeLevel2.m_Level2SellNodes[2].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceSell3);
		// 		/////////////////////////////////////////////////
		// 		//  卖2
		// 		m_GridCtrl.SetCellType(8, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(8, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceSell2 = Float2SymbolString(RealtimeLevel2.m_Level2SellNodes[1].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceSell2);
		// 		/////////////////////////////////////////////////
		// 		//  卖1
		// 		m_GridCtrl.SetCellType(9, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(9, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceSell1 = Float2SymbolString(RealtimeLevel2.m_Level2SellNodes[0].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceSell1);
		// 
		// 		/////////////////////////////////////////////////
		// 		//  买1
		// 		m_GridCtrl.SetCellType(10, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(10, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceBuy1 = Float2SymbolString(RealtimeLevel2.m_Level2BuyNodes[0].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceBuy1);
		// 

				// 		/////////////////////////////////////////////////
		// 		//  买2
		// 		m_GridCtrl.SetCellType(11, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(11, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceBuy2 = Float2SymbolString(RealtimeLevel2.m_Level2BuyNodes[1].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceBuy2);
		// 
		// 		/////////////////////////////////////////////////
		// 		//  买3
		// 		m_GridCtrl.SetCellType(12, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(12, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceBuy3 = Float2SymbolString(RealtimeLevel2.m_Level2BuyNodes[2].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceBuy3);
		// 
		// 		/////////////////////////////////////////////////
		// 		//  买4
		// 		m_GridCtrl.SetCellType(13, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(13, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceBuy4 = Float2SymbolString(RealtimeLevel2.m_Level2BuyNodes[3].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceBuy4);
		// 
		// 		/////////////////////////////////////////////////
		// 		//  买5
		// 		m_GridCtrl.SetCellType(14, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(14, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceBuy5 = Float2SymbolString(RealtimeLevel2.m_Level2BuyNodes[4].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceBuy5);
		// 
		// 		/////////////////////////////////////////////////
		// 		//  买6
		// 		m_GridCtrl.SetCellType(15, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(15, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceBuy6 = Float2SymbolString(RealtimeLevel2.m_Level2BuyNodes[5].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceBuy6);
		// 
		// 		/////////////////////////////////////////////////
		// 		//  买7
		// 		m_GridCtrl.SetCellType(16, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(16, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceBuy7 = Float2SymbolString(RealtimeLevel2.m_Level2BuyNodes[6].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceBuy7);
		// 
		// 		/////////////////////////////////////////////////
		// 		//  买8
		// 		m_GridCtrl.SetCellType(17, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(17, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceBuy8 = Float2SymbolString(RealtimeLevel2.m_Level2BuyNodes[7].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceBuy8);
		// 
		// 		/////////////////////////////////////////////////
		// 		//  买9
		// 		m_GridCtrl.SetCellType(18, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(18, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceBuy9 = Float2SymbolString(RealtimeLevel2.m_Level2BuyNodes[8].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceBuy8);
		// 
		// 		/////////////////////////////////////////////////
		// 		//  买10
		// 		m_GridCtrl.SetCellType(19, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(19, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceBuy10 = Float2SymbolString(RealtimeLevel2.m_Level2BuyNodes[9].m_fPrice, fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceBuy10);
		// 
		// 		/////////////////////////////////////////////////
		// 		//  卖均
		// 		m_GridCtrl.SetCellType(10, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(10, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceAveSell = Float2SymbolString(RealtimeLevel2.m_fPriceAveSell , fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceAveSell);
		// 
		// 		/////////////////////////////////////////////////
		// 		//  买均
		// 		m_GridCtrl.SetCellType(11, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(11, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		CString StrPriceAveBuy = Float2SymbolString(RealtimeLevel2.m_fPriceAveBuy , fPricePrevClose, iSaveDec);	
		// 		pCellSymbol->SetText(StrPriceAveBuy);
		// 
		// 		/////////////////////////////////////////////////
		// 		//  卖1
		// 		m_GridCtrl.SetCellType(9, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(9, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCellSymbol->SetText(StrPriceSell1);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 24 行1 列 至 26行 4 列. 共显示 12 笔卖家信息,需要判断是否有15 笔买家信息.否则越界操作报错
		//  		
		//  		int32 iSizeSell = RealtimeLevel2.m_Level2SellNodes[0].m_Level2Details.GetSize();
		//  		
		//  		CString StrVolumeSell;
		//  		int     iIndexSell =0 ;
		// 
		//  		for (int iRowSell = 23 ; iRowSell < 26 ; iRowSell++ )
		//  		{
		//  			for ( int iColumnSell = 0 ; iColumnSell < 4 ;iColumnSell++ )
		//  			{
		//  				if ( iIndexSell < iSizeSell )
		//  				{				
		//  					CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRowSell, iColumnSell);
		//  					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		//  					pCell->SetDefaultTextColor(ESCVolume);
		//  					CString StrVolumeSell = Float2String( RealtimeLevel2.m_Level2SellNodes[0].m_Level2Details[iIndexSell].m_fVolume , 0, false );
		//  					pCell->SetText(StrVolumeSell);
		// 					iIndexSell++;
		//  				}
		//  				else
		//  				{
		//  					CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRowSell, iColumnSell);
		//  					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		//  					pCell->SetDefaultTextColor(ESCVolume);
		//  					pCell->SetText(L"");
		//  				}
		//  			}
		//  		}
		// 		/////////////////////////////////////////////////
		// 		//  买1
		// 		m_GridCtrl.SetCellType(26, 1, RUNTIME_CLASS(CGridCellSymbol));
		// 		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(9, 1);
		// 		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		// 		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCellSymbol->SetText(StrPriceBuy1);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 28 行1 列 至 30 行 4 列. 共显示12 笔买家信息 
		// 
		//  		CString StrVolumeBuy;
		//  		int iIndexBuy = 0  ;
		// 		int32 iSizeBuy = RealtimeLevel2.m_Level2BuyNodes[0].m_Level2Details.GetSize();
		// 		
		// 		for (int iRowBuy = 27 ; iRowBuy < 30 ; iRowBuy++ )
		// 		{
		// 			for ( int iColumnBuy = 0 ; iColumnBuy < 4 ;iColumnBuy++ )
		// 			{
		// 				if ( iIndexBuy < iSizeBuy)
		// 				{
		// 					CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRowBuy, iColumnBuy);
		// 					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 					pCell->SetDefaultTextColor(ESCVolume);
		// 					CString StrVolumeBuy = Float2String( RealtimeLevel2.m_Level2BuyNodes[0].m_Level2Details[iIndexBuy].m_fVolume , 0, false );
		// 					pCell->SetText(StrVolumeBuy);
		// 					iIndexBuy++;
		// 				}
		// 				else
		// 				{
		// 					CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRowBuy, iColumnBuy);
		// 					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 					pCell->SetDefaultTextColor(ESCVolume);
		// 					pCell->SetText(L"");
		// 				}
		// 				
		// 			}
		// 		}		
		// 		/////////////////////////////////////////////////
		// 		// 卖10 量, 黄色
		// 		CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeSell10 = Float2String(RealtimeLevel2.m_Level2SellNodes[9].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeSell10);
		// 	
		// 		/////////////////////////////////////////////////
		// 		// 卖9 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(1, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeSell9 = Float2String(RealtimeLevel2.m_Level2SellNodes[8].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeSell9);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 卖8 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(2, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeSell8 = Float2String(RealtimeLevel2.m_Level2SellNodes[7].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeSell8);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 卖7 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(3, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeSell7 = Float2String(RealtimeLevel2.m_Level2SellNodes[6].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeSell7);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 卖6 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(4, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeSell6 = Float2String(RealtimeLevel2.m_Level2SellNodes[5].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeSell6);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 卖5 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(5, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeSell5 = Float2String(RealtimeLevel2.m_Level2SellNodes[4].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeSell5);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 卖4 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(6, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeSell4 = Float2String(RealtimeLevel2.m_Level2SellNodes[3].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeSell4);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 卖3 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(7, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeSell3 = Float2String(RealtimeLevel2.m_Level2SellNodes[2].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeSell3);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 卖2 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(8, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeSell2 = Float2String(RealtimeLevel2.m_Level2SellNodes[1].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeSell2);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 卖1 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(9, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeSell1 = Float2String(RealtimeLevel2.m_Level2SellNodes[0].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeSell1);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 买 1 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(10, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeBuy1 = Float2String(RealtimeLevel2.m_Level2BuyNodes[0].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeBuy1);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 买 2 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(11, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeBuy2 = Float2String(RealtimeLevel2.m_Level2BuyNodes[1].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeBuy2);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 买 3 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(12, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeBuy3 = Float2String(RealtimeLevel2.m_Level2BuyNodes[2].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeBuy3);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 买 4 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(13, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeBuy4 = Float2String(RealtimeLevel2.m_Level2BuyNodes[3].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeBuy4);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 买 5 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(14, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeBuy5 = Float2String(RealtimeLevel2.m_Level2BuyNodes[4].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeBuy5);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 买 6 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(15, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeBuy6 = Float2String(RealtimeLevel2.m_Level2BuyNodes[5].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeBuy6);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 买 7 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(16, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeBuy7 = Float2String(RealtimeLevel2.m_Level2BuyNodes[6].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeBuy7);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 买 8 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(17, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeBuy8 = Float2String(RealtimeLevel2.m_Level2BuyNodes[7].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeBuy8);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 买 9 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(18, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeBuy9 = Float2String(RealtimeLevel2.m_Level2BuyNodes[8].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeBuy9);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 买 10 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(19, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeBuy10 = Float2String(RealtimeLevel2.m_Level2BuyNodes[9].m_fVolume, 0, true );
		// 		pCell->SetText(StrVolumeBuy10);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 总卖, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(20, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeSellTotal = Float2String(RealtimeLevel2.m_fVolumeSellTotal , 0, true );
		// 		pCell->SetText(StrVolumeSellTotal);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 总买, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(21, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrVolumeBuyTotal = Float2String(RealtimeLevel2.m_fVolumeBuyTotal , 0, true );
		// 		pCell->SetText(StrVolumeBuyTotal);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 卖1 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(22, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrSell1Volume = Float2String(RealtimeLevel2.m_Level2SellNodes[0].m_Level2Details.GetSize(), 0, true );
		// 		StrSell1Volume += L"笔";
		// 		pCell->SetText(StrSell1Volume);
		// 
		// 		/////////////////////////////////////////////////
		// 		// 买1 量, 黄色
		// 		pCell = (CGridCellSys *)m_GridCtrl.GetCell(26, 3);
		// 		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		// 		pCell->SetDefaultTextColor(ESCVolume);
		// 		CString StrBuy1Volume = Float2String(RealtimeLevel2.m_Level2BuyNodes[0].m_Level2Details.GetSize(), 0, true );
		// 		StrBuy1Volume += L"笔";
		// 		pCell->SetText(StrBuy1Volume);		
		m_GridCtrl.Refresh();	

	}
}

bool32 CIoViewLevel2::FromXml(TiXmlElement * pTiXmlElement)
{
	if (NULL == pTiXmlElement)
		return false;

	// 判读是不是IoView的节点
	const char *pcValue = pTiXmlElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;

	// 判断是不是描述自己这个业务视图的节点
	const char *pcAttrValue = pTiXmlElement->Attribute(GetXmlElementAttrIoViewType());
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(CString(pcAttrValue)) != 0)	// 不是描述自己的业务节点
		return false;

	//
	SetFontsFromXml(pTiXmlElement);
	SetColorsFromXml(pTiXmlElement);
	SetRowHeightAccordingFont();

	// 读取本业务视图特有的内容
	int32   iMarketId		= -1;
	CString StrMerchCode	= L"";
	pcAttrValue = pTiXmlElement->Attribute(GetXmlElementAttrMarketId());
	if (NULL != pcAttrValue)
	{
		iMarketId = atoi(pcAttrValue);
	}  	
	pcAttrValue = pTiXmlElement->Attribute(GetXmlElementAttrMerchCode());
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

	// 商品发生改变
	OnVDataMerchChanged(iMarketId, StrMerchCode, pMerchFound);
	return true;
}

CString CIoViewLevel2::ToXml()
{
	CString StrThis;	
	CString StrMarketId;
	StrMarketId.Format(L"%d", m_MerchXml.m_iMarketId);
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\"", /*>\n*/
						CString(GetXmlElementValue()).GetBuffer(),
						CString(GetXmlElementAttrIoViewType()).GetBuffer(), CIoViewManager::GetIoViewString(this).GetBuffer(),
						CString(GetXmlElementAttrShowTabName()).GetBuffer(), m_StrTabShowName.GetBuffer(),
						CString(GetXmlElementAttrMerchCode()).GetBuffer(), 
						m_MerchXml.m_StrMerchCode.GetBuffer(),
						CString(GetXmlElementAttrMarketId()).GetBuffer(),
						StrMarketId.GetBuffer());
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

CString CIoViewLevel2::GetDefaultXML()
{
	CString StrThis = L"";
	
	// zhangbo 0403 #待补充
	//...
	 
	return StrThis;
}

bool32	CIoViewLevel2::CreateTable(E_ReportType eMerchKind)
{	
	if ( ERTStockHk == eMerchKind || ERTWarrantHk == eMerchKind)
	{	
		SetRowHeightAccordingFont();
		//////////////////////////////////////////////////////////////////////////
		// 买盘
		m_bHongKong = true;
		m_bChina	= false;
		//////////////////////////////////////////////////////////////////////////
		// 清空 国内Level2 的表格信息
		if (NULL != m_GridCtrl.GetSafeHwnd())
		{
			m_GridCtrl.DeleteAllItems();
			m_GridCtrl.ShowWindow(SW_HIDE);
		}
		if (NULL != m_CtrlTitleChina.GetSafeHwnd())
		{
			m_CtrlTitleChina.ShowWindow(SW_HIDE);
		}
		//////////////////////////////////////////////////////////////////////////
		// 不存在时建表,存在时显示
		if (NULL == m_GridCtrlBuy.GetSafeHwnd())
		{
			if (!m_GridCtrlBuy.Create(CRect(0, 0, 0, 0), this, 20205))
				return false;		
			
			m_GridCtrlBuy.SetListMode(false);			
			m_GridCtrlBuy.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
			m_GridCtrlBuy.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
			m_GridCtrlBuy.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		}
		m_GridCtrlBuy.DeleteAllItems();
		m_GridCtrlBuy.ShowWindow(SW_SHOW);
		m_GridCtrlBuy.SetColumnCount(4);
		m_GridCtrlBuy.SetRowCount(10);
		//////////////////////////////////////////////////////////////////////////
		// 买盘的表头标题栏
		if (!m_CtrlTitleHongKongleft.GetSafeHwnd())
		{
			if (! m_CtrlTitleHongKongleft.Create(L"", SS_LEFT ,CRect(0,0,0,0),this,IDC_STATIC_BUY))
			return false;
		}
		if (!m_CtrlTitleHongKongright.GetSafeHwnd())
		{
			if (! m_CtrlTitleHongKongright.Create(L"",SS_LEFT,CRect(0,0,0,0),this))
			return false;
		}

		m_GridCtrlBuy.EnableSelection(false);
		m_GridCtrlSell.EnableSelection(false);

		m_CtrlTitleHongKongleft.SetTitle(L"买盘");	
		m_CtrlTitleHongKongleft.ShowWindow(SW_SHOW);
			  
		m_CtrlTitleHongKongright.SetTitle(L"");
		m_CtrlTitleHongKongright.ShowWindow(SW_SHOW);

		//////////////////////////////////////////////////////////////////////////
		// 图标
		if (!m_ImageList.GetSafeHandle())
		{
			m_ImageList.Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));
		}
		// m_GridCtrlBuy.SetImageList(&m_ImageList);
		
		//////////////////////////////////////////////////////////////////////////
		// 滚动条
		m_XSBHorz.SetOwner(&m_GridCtrlBuy);
		m_XSBVert.SetOwner(&m_GridCtrlBuy);
		m_GridCtrlBuy.SetScrollBar(&m_XSBHorz, &m_XSBVert);						

		///////////////////////////////////////////////// 	
		// 卖盘
		if (NULL == m_GridCtrlSell.GetSafeHwnd())
		{
			if (!m_GridCtrlSell.Create(CRect(0, 0, 0, 0), this, 20205))
				return false;					
			m_GridCtrlSell.SetListMode(false);	// 不允许行选择模式			
			m_GridCtrlSell.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
			m_GridCtrlSell.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
			m_GridCtrlSell.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		}	
		m_GridCtrlSell.DeleteAllItems();			
		m_GridCtrlSell.ShowWindow(SW_SHOW);			
		
		if (!m_GridCtrlSell.SetColumnCount(4))
			return false;
		if (!m_GridCtrlSell.SetRowCount(10))
			return false;

		if (!m_CtrlTitleHongKongSellleft.GetSafeHwnd())
		{
			if (! m_CtrlTitleHongKongSellleft.Create(L"",SS_LEFT,CRect(0,0,0,0),this))
				return false;			
		}
		if (!m_CtrlTitleHongKongSellright.GetSafeHwnd())
		{
			if (! m_CtrlTitleHongKongSellright.Create(L"",SS_LEFT,CRect(0,0,0,0),this))
 			return false;
		}

 		m_CtrlTitleHongKongSellleft.SetTitle(L"卖盘");		
 		m_CtrlTitleHongKongSellleft.ShowWindow(SW_SHOW);
 		
 		m_CtrlTitleHongKongSellright.SetTitle(L"");
 		m_CtrlTitleHongKongSellright.ShowWindow(SW_SHOW);
				
		if (!m_ImageList.GetSafeHandle())
		{
			m_ImageList.Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));
		}
		// m_GridCtrlSell.SetImageList(&m_ImageList);
		
		m_XSBHorz.SetOwner(&m_GridCtrlSell);
		m_XSBVert.SetOwner(&m_GridCtrlSell);
		m_GridCtrlSell.SetScrollBar(&m_XSBHorz, &m_XSBVert);						
		///////////////////////////////////////////////// 	
		m_GridCtrlBuy.ExpandToFit();
		m_GridCtrlSell.ExpandToFit();		
		//////////////////////////////////////////////////////////////////////////
		// 刷新问题
		m_GridCtrlSell.GetParent()->ShowWindow(SW_HIDE);
		m_GridCtrlSell.GetParent()->ShowWindow(SW_SHOW);
		ASSERT(m_GridCtrlSell.GetParent()->IsKindOf(RUNTIME_CLASS(CIoViewLevel2)));
		CRect  rect;
		m_GridCtrlSell.GetParent()->GetClientRect(&rect);
		OnSize(0,rect.right,rect.bottom);
	}
	else
	{
		m_bChina    = true;
		m_bHongKong = false;
		//////////////////////////////////////////////////////////////////////////
		// 清除港股的信息
		if (NULL != m_GridCtrlBuy.GetSafeHwnd())
		{
			m_GridCtrlBuy.DeleteAllItems();
			m_GridCtrlBuy.ShowWindow(SW_HIDE);
		}
		if (NULL != m_GridCtrlSell.GetSafeHwnd())
		{
			m_GridCtrlSell.DeleteAllItems();
			m_GridCtrlSell.ShowWindow(SW_HIDE);
		}
		if (NULL != m_CtrlTitleHongKongleft.GetSafeHwnd())
		{
			m_CtrlTitleHongKongleft.ShowWindow(SW_HIDE);
		}
		if (NULL != m_CtrlTitleHongKongright.GetSafeHwnd())
		{
			m_CtrlTitleHongKongright.ShowWindow(SW_HIDE);
		}
		if (NULL != m_CtrlTitleHongKongSellleft.GetSafeHwnd())
		{
			m_CtrlTitleHongKongSellleft.ShowWindow(SW_HIDE);
		}
		if (NULL != m_CtrlTitleHongKongSellright.GetSafeHwnd())
		{
			m_CtrlTitleHongKongSellright.ShowWindow(SW_HIDE);
		}
		//////////////////////////////////////////////////////////////////////////
		// 表格不存在是建表,否则显示出来.
		if (NULL == m_GridCtrl.GetSafeHwnd())
		{
			if (!m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 20205))
				return false;					
			m_GridCtrl.SetListMode(false);		
			m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
			m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
			m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		}

		m_GridCtrl.DeleteAllItems();
		m_GridCtrl.ShowWindow(SW_SHOW);			
        
		// 以前正确的行列数
		// if (!m_GridCtrl.SetColumnCount(4))
		//	return false;
		// if (!m_GridCtrl.SetRowCount(30))
		//	return false;

		// 没有数据,只显示一行一列
		 if (!m_GridCtrl.SetColumnCount(1))
			return false;
		 if (!m_GridCtrl.SetRowCount(1))
			return false;
		//////////////////////////////////////////////////////////////////////////
		// 表头标题栏
		if (!m_CtrlTitleChina.GetSafeHwnd())
		{
			if (! m_CtrlTitleChina.Create(L"",SS_LEFT,CRect(0,0,0,0),this))
			return false;
		}
		m_CtrlTitleChina.ShowWindow(SW_SHOW);

		m_GridCtrl.EnableSelection(false);
		//////////////////////////////////////////////////////////////////////////
		// 图标
		if (!m_ImageList.GetSafeHandle())
		{
			m_ImageList.Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));
		}		
		// m_GridCtrl.SetImageList(&m_ImageList);
		
		//////////////////////////////////////////////////////////////////////////
		// 滚动条
		m_XSBHorz.SetOwner(&m_GridCtrl);
		m_XSBVert.SetOwner(&m_GridCtrl);
		m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);						
		///////////////////////////////////////////////// 
		
		CGridCellSys *pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, 0);
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		pCell->SetText(L"暂无Level2 数据");
		
		//  		CGridCellSys *pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		//  		pCell->SetText(L"卖⑩");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(1, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//  		pCell->SetText(L"卖⑨");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(2, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//  		pCell->SetText(L"卖⑧");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(3, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//  		pCell->SetText( L"卖⑦");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(4, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//  		pCell->SetText( L"卖⑥");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(5, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//  		pCell->SetText(L"卖⑤");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(6, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//  		pCell->SetText(L"卖④");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(7, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//  		pCell->SetText( L"卖③");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(8, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//  		pCell->SetText( L"卖②");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(9, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//  		pCell->SetText(L"卖①");
		//  		
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(10, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		//  		pCell->SetText(L"买①");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(11, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//  		pCell->SetText(L"买②");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(12, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//  		pCell->SetText(L"买③");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(13, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//  		pCell->SetText(L"买④");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(14, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//  		pCell->SetText(L"买⑤");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(15, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//  		pCell->SetText(L"买⑥");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(16, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//  		pCell->SetText(L"买⑦");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(17, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//  		pCell->SetText(L"买⑧");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(18, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//  		pCell->SetText(L"买⑨");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(19, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//  		pCell->SetText(L"买⑩");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(20, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		//  		pCell->SetText(L"卖均");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(21, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		//  		pCell->SetText(L"买均");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(22, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		//  		pCell->SetText(L"卖①");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(26, 0);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		//  		pCell->SetText(L"买①");
		//  		
		//  		pCell = (CGridCellSys *)m_GridCtrl.GetCell(20, 2);
		//  		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		//  		pCell->SetText(L"总卖");
		//  		
		//   		pCell = (CGridCellSys *)m_GridCtrl.GetCell(21, 2);
		//   		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		//  		pCell->SetText(L"总买");

		m_GridCtrl.ExpandToFit();
		CRect  rect;
		m_GridCtrl.GetParent()->GetClientRect(&rect);
		ASSERT(m_GridCtrl.GetParent()->IsKindOf(RUNTIME_CLASS(CIoViewLevel2)));
		OnSize(0,rect.right,rect.bottom);
	}
	m_eMerchKind = eMerchKind;

	return true;
}
 
void CIoViewLevel2::OnIoViewActive()
{
	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	// zhangbo 0527 #待补充, 可以在这里设置该窗口下可以设置为focus的窗口， 以便接受鼠标， 键盘事件
	//...
}

void CIoViewLevel2::OnIoViewDeactive()
{
}

void CIoViewLevel2::ChangeContentRealTime(CIoViewLevel2::E_ShowType e_ShowSignal)
{
	// 
	ASSERT(e_ShowSignal<CIoViewLevel2::ESTCount);
	if (m_xxxBuyLevel2ListBackUp.GetSize()>0)
	{
		int32 iCount =0;
		int32 iBuySize = m_xxxBuyLevel2ListBackUp.GetSize();
		for (int32  iCol = 0; iCol<m_GridCtrlBuy.GetColumnCount(); iCol++)
		{
			for (int32 iRow = 0; iRow < m_GridCtrlBuy.GetRowCount(); iRow ++)
			{
				if (iCount<iBuySize)
				{
					m_GridCtrlBuy.SetCellType(iRow,iCol,RUNTIME_CLASS(CGridCellLevel2));
					CGridCellLevel2 * pCell = (CGridCellLevel2 *)m_GridCtrlBuy.GetCell(iRow, iCol);
					
					pCell->SetState(pCell->GetState() | GVIS_SHOWTIPS);	
					
					if (!m_xxxBuyLevel2ListBackUp[iCount].m_bPos)
					{
						pCell->SetContent( m_xxxBuyLevel2ListBackUp[iCount].m_StrBrokerCode,m_xxxBuyLevel2ListBackUp[iCount].m_StrBrokerName,GetShowType()); 
					}
					else
					{
						pCell->SetState(pCell->GetState() & ~GVIS_SHOWTIPS);
						pCell->SetDefaultTextColor(ESCKLineRise);
						pCell->SetText(m_xxxBuyLevel2ListBackUp[iCount].m_StrPos);
						
					}
					iCount ++;
				}
			}
		}
		m_GridCtrlBuy.Refresh();
	}
		
	if (m_yyySellLevelListBackUp.GetSize()>0)
	{
		int32 iCount =0;
		int32 iSellSize = m_yyySellLevelListBackUp.GetSize();
		for (int32  iCol = 0; iCol<m_GridCtrlSell.GetColumnCount(); iCol++)
		{
			for (int32 iRow = 0; iRow < m_GridCtrlSell.GetRowCount(); iRow ++)
			{
				if (iCount<iSellSize)
				{
					m_GridCtrlSell.SetCellType(iRow,iCol,RUNTIME_CLASS(CGridCellLevel2));
					CGridCellLevel2 * pCell = (CGridCellLevel2 *)m_GridCtrlSell.GetCell(iRow, iCol);
					
					pCell->SetState(pCell->GetState() | GVIS_SHOWTIPS);	

					if (!m_yyySellLevelListBackUp[iCount].m_bPos)
					{
						pCell->SetContent( m_yyySellLevelListBackUp[iCount].m_StrBrokerCode,m_yyySellLevelListBackUp[iCount].m_StrBrokerName,GetShowType()); 
					}
					else
					{
						pCell->SetState(pCell->GetState() & ~GVIS_SHOWTIPS);
						pCell->SetDefaultTextColor(ESCKLineFall);
						pCell->SetText(m_yyySellLevelListBackUp[iCount].m_StrPos);
						
					}
					iCount ++;
				}
			}
		}
		m_GridCtrlSell.Refresh();
	}
}

void CIoViewLevel2::SetShowType(CIoViewLevel2::E_ShowType eShowType)
{
	ASSERT( eShowType < ESTCount);
	m_eShowType = eShowType;
}

CIoViewLevel2::E_ShowType CIoViewLevel2::GetShowType()
{
	return m_eShowType;
}

void CIoViewLevel2::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;

	if ( NULL != m_pMerchXml )
	{
		StrTitle  += L" ";
		StrTitle  += m_pMerchXml->m_MerchInfo.m_StrMerchCnName;
	}
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

LRESULT CIoViewLevel2::OnMessageTitleButton(WPARAM wParam,LPARAM lParam)
{
	/*
	if ( m_bChina)
	{
		return 0;
	}
	
	UINT uID = (UINT)wParam;
	if ( 0 == uID)
	{
		// 名
		if ( CIoViewLevel2::ESTBrokerCode == m_eShowType)
		{
			SetShowType(CIoViewLevel2::ESTBrokerName);
			ChangeContentRealTime(CIoViewLevel2::ESTBrokerName);
			CIoViewTitleButton* pButton = m_Title.GetPrivateButtonByID(uID);
			if (NULL != pButton )
			{
				pButton->SetText(L"码");
			}			
		}
		else if ( CIoViewLevel2::ESTBrokerName == m_eShowType )
		{
			SetShowType(CIoViewLevel2::ESTBrokerCode);
			ChangeContentRealTime(CIoViewLevel2::ESTBrokerCode);

			CIoViewTitleButton* pButton = m_Title.GetPrivateButtonByID(uID);
			if (NULL != pButton )
			{
				pButton->SetText(L"名");
			}
		}
		
		Invalidate();
	}
	else if ( 1 == uID)
	{
		// 横
		m_bHoerzShow = !m_bHoerzShow;

		CIoViewTitleButton* pButton = m_Title.GetPrivateButtonByID(uID);
		if (NULL != pButton )
		{
			if (m_bHoerzShow)
			{
				pButton->SetText(L"横");
			}
			else
			{
				pButton->SetText(L"纵");
			}
			
		}
		
		CMPIChildFrame* pChildFrame = (CMPIChildFrame*)GetParentFrame();
		pChildFrame->ForceRefresh();
	}
	*/
	return 0;
}
