// dlgnetwork.cpp : implementation file
//
#include "stdafx.h"
#include "dlgnetwork.h"
#include "GridCellSys.h"
#include "GridCellLevel2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNetWork dialog
const T_ServiceObject KaServiceObject[] =
{
 	T_ServiceObject(L"General",EDSTGeneral),
 	T_ServiceObject(L"KLine",EDSTKLine),	
 	T_ServiceObject(L"TimeSale",EDSTTimeSale),
 	T_ServiceObject(L"Price",EDSTPrice),	
 	T_ServiceObject(L"Level2",EDSTLevel2),	
 	T_ServiceObject(L"Tick",EDSTTick),
};

const int32 KiServiceCounts = sizeof(KaServiceObject)/sizeof(T_ServiceObject);

const T_SpeedShowObject KaSpeedShowObject[] = 
{
	T_SpeedShowObject(PINGMIN,200,RGB(0,255,0)),
	T_SpeedShowObject(200,300,RGB(128,255,128)),
	T_SpeedShowObject(300,400,RGB(255,255,0)),
	T_SpeedShowObject(400,500,RGB(255,128,0)),
	T_SpeedShowObject(500,PINGMAX,RGB(255,0,0)),
};

const int32 KiSpeedShowCounts = sizeof(KaSpeedShowObject)/sizeof(T_SpeedShowObject);

//////////////////////////////////////////////////////////////////////////

CDlgNetWork::CDlgNetWork(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgNetWork::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNetWork)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bShowText = true;
	m_bShowAll	= false;
}

void CDlgNetWork::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNetWork)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgNetWork, CDialogEx)
//{{AFX_MSG_MAP(CDlgNetWork)
	ON_WM_ERASEBKGND()	
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_TIMER()
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNetWork message handlers
BOOL CDlgNetWork::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			PostMessage(WM_CLOSE,0,0);
		}
		else if ( pMsg->wParam == VK_F5)
		{
			m_bShowText = !m_bShowText;
			ChangeShowStyle();
		}
		else if ( pMsg->wParam == VK_F6)
		{
			if (m_GridCtrl.GetSafeHwnd())
			{ 
				m_bShowAll = !m_bShowAll;
				if (m_bShowAll)
				{
					CRect rectMax;
 					SystemParametersInfo(SPI_GETWORKAREA,0,&rectMax,0);	
					MoveWindow(&rectMax);
					CRect rectGrid;
					int32 iHeightPerRow;
					m_GridCtrl.GetClientRect(&rectGrid);
					int32 iFirstRowHeight = m_GridCtrl.GetRowHeight(0);

					if (m_GridCtrl.GetRowCount()>1)
					{
						iHeightPerRow = (rectGrid.Height() - iFirstRowHeight) / (m_GridCtrl.GetRowCount() -1 );
					}
					else
					{
						iHeightPerRow = rectGrid.Height();
						
					}
					for (int32 i = 1; i<m_GridCtrl.GetRowCount(); i++)
					{
						m_GridCtrl.SetRowHeight(i,iHeightPerRow);
					}
				}
				else
				{
					int32 iFirstRowHeight = m_GridCtrl.GetRowHeight(0);
					for (int32 i = 0; i<m_GridCtrl.GetRowCount(); i++)
					{
						m_GridCtrl.SetRowHeight(i,iFirstRowHeight);
					}				
				}
			}			
		}
		if (m_GridCtrl.GetSafeHwnd())
		{
			m_GridCtrl.Refresh();
			m_GridCtrl.Invalidate();			
			m_GridCtrl.SetFocus();
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CDlgNetWork::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CenterWindow();

	CGGTongDoc * pDoc = (CGGTongDoc *)AfxGetDocument();
	ASSERT( NULL!= pDoc );
	if (!pDoc->m_pAbsDataManager)
	{
		return FALSE;
	}
	pDoc->m_pAbsDataManager->GetCommnunicaionList(m_aCommunicationList);

	SetTimer(DLGNETWORKTIMER,1,NULL);
	CreateGrid();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgNetWork::OnSize(UINT nType, int cx, int cy)
{
	CRect rect;
	GetClientRect(&rect);
	if (m_GridCtrl.GetSafeHwnd())
	{
		m_GridCtrl.MoveWindow(&rect);
		m_GridCtrl.ExpandColumnsToFit();		
		//AdjustToBestLayout(rect);
		m_GridCtrl.Invalidate();
	}	

	CDialogEx::OnSize(nType,cx,cy);
}

void CDlgNetWork::OnClose() 
{
	CMainFrame* pWnd    = (CMainFrame*)AfxGetMainWnd();
	pWnd->m_pDlgNetWork = NULL;
	KillTimer(DLGNETWORKTIMER);	
	DestroyWindow();
}

void CDlgNetWork::PostNcDestroy() 
{	
	CDialog::PostNcDestroy();
	delete this;	
}

BOOL CDlgNetWork::OnEraseBkgnd(CDC* pDC)
{
	return true;
}

bool32 CDlgNetWork::CreateGrid()
{
	CRect rect;
	GetClientRect(&rect);

  	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
	m_XSBVert.SetScrollRange(0, 10);
	m_XSBVert.ShowWindow(SW_SHOW);
	
	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
	m_XSBHorz.SetScrollRange(0, 0);
	m_XSBHorz.ShowWindow(SW_SHOW);


	if (NULL == m_GridCtrl.GetSafeHwnd())
	{
		if (!m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 543216))
			return false;
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	}
	// 清空表格内容
	m_GridCtrl.DeleteAllItems();

	// 设置列数
	int32 iColoumCounts = 1 + m_aCommunicationList.GetSize();
	if (!m_GridCtrl.SetColumnCount(iColoumCounts))
		return false;

	//  设置行数
	int32 iRowCounts = 0;
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;	
	ASSERT(NULL!= pDocument);
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT( NULL != pAbsCenterManager );
	int32 iBreedSize = pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize();	

	int32 i = 0;
	for ( i = 0; i< iBreedSize ; i++)
	{
		CBreed * pBreed = pAbsCenterManager->GetMerchManager().m_BreedListPtr[i];
		for (int32 j=0; j< pBreed->m_MarketListPtr.GetSize() ; j++ )
		{
			iRowCounts += KiServiceCounts;
		}
	}
	// (0,0) 这个预留显示其他信息
	iRowCounts += 1;	
	if (!m_GridCtrl.SetRowCount(iRowCounts))
		return false;
	

	// 设置表格图标
	m_ImageList.Create(MAKEINTRESOURCE(IDB_NET_ONOFF), 16, 1, RGB(255,255,255));
	m_GridCtrl.SetImageList(&m_ImageList);
	
	// 设置相互之间的关联			
	m_XSBHorz.SetOwner(&m_GridCtrl);
	m_XSBVert.SetOwner(&m_GridCtrl);
	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);						
	
 	m_GridCtrl.MoveWindow(&rect);
	//AdjustToBestLayout(rect);
	m_GridCtrl.ExpandColumnsToFit();

//  	m_GridCtrl.SetFixedRowCount(1);
//  	m_GridCtrl.SetFixedColumnCount(1);
// 		
//		m_GridCtrl.SetHeaderSort(true);
		
	// 第一行的服务器信息				
	CGridCellSys *pCell = NULL;

	pCell = (CGridCellSys *)m_GridCtrl.GetCell(0,0);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(L"F5 切换显示模式 F6 显示所有/局部");

	for ( i = 1; i < m_GridCtrl.GetColumnCount() ; i++)
	{
		m_GridCtrl.SetCellType(0,i,RUNTIME_CLASS(CGridCellLevel2));
		CGridCellLevel2 * pCellLevel2 = (CGridCellLevel2 *)m_GridCtrl.GetCell(0, i);
		
		pCellLevel2->SetState(pCellLevel2->GetState() | GVIS_SHOWTIPS);					
		pCellLevel2->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
		pCellLevel2->SetTextClr(RGB(255,0,0));

		CString StrText;
		StrText.Format(L"服务器%d",i);
		pCellLevel2->SetText(StrText);		
	}
	// 第一列的所有服务的名称
	for ( i = 1; i < m_GridCtrl.GetRowCount() ; i++)
	{
		CString StrText;				
		for (int32 m = 0; m< iBreedSize ; m++)
		{
			CBreed * pBreed = pAbsCenterManager->GetMerchManager().m_BreedListPtr[m];
			for (int32 n=0; n< pBreed->m_MarketListPtr.GetSize() ; n++ )
			{
				CMarket * pMarket = pBreed->m_MarketListPtr[n];
				// 每个市场 6 种服务
				CString StrMarketName = pMarket->m_MarketInfo.m_StrCnName;
				
				for ( int32 p = 0 ; p < KiServiceCounts ;p++)
				{
					CString StrServiceName = KaServiceObject[p].m_StrDataServiceName;
					StrText = StrMarketName + StrServiceName;
					
					pCell =(CGridCellSys *)m_GridCtrl.GetCell(i, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
					pCell->SetTextClr(16764551);
					pCell->SetText(StrText);
	
					// 判断这一列服务,哪个服务器可以提供
					for (int32 q = 0; q < m_aCommunicationList.GetSize(); q++)
					{
						FillTable(m_aCommunicationList[q],pMarket->m_MarketInfo.m_iMarketId,KaServiceObject[p].m_eDataServiceType,i,q+1);
					}
					i+=1;
				}
			}
		}
	}
	return true;
}

void CDlgNetWork::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == DLGNETWORKTIMER)
	{
		m_aStrTipsText.RemoveAll();
		CServerState  SerVerState;
		CString StrAddr;
		uint32 uiPort;
		CProxyInfo ProxyInfo;
		
		CStringArray  aStrKeepTime;
		CArray<uint32,uint32> auiPing;
		CStringArray  aStrSpeed;	

		aStrKeepTime.RemoveAll();
		auiPing.RemoveAll();
		aStrSpeed.RemoveAll();
		//////////////////////////////////////////////////////////////////////////
		// 得到每台服务器的信息,用于Tips 显示

		int32 i = 0;
		for ( i=0 ;i< m_aCommunicationList.GetSize(); i++ )
		{
			CString StrTips;
			CString StrKeepTime;
			CString StrSpeed;
			uint32 uiTest;
			//////////////////////////////////////////////////////////////////////////
			// ...fangz0821 fortest
			// 得到服务器Ping 值
			uiTest = rand()%PINGMAX + PINGMIN;
			if (uiTest > PINGMAX)
				uiTest -= PINGMIN;				
			//////////////////////////////////////////////////////////////////////////

			m_aCommunicationList[i]->GetServerSummary(StrAddr,uiPort,ProxyInfo);
			m_aCommunicationList[i]->GetServerState(SerVerState);
			
			
			if (SerVerState.m_bConnected)
			{
				// 得到服务器连接时间
				uint32 uiSeconds = SerVerState.m_uiKeepSecond;
				
				uint32 uiDay = uiSeconds / (24*60*60) ;
				
				uiSeconds -= uiDay*24*60*60;
				
				uint32 uiHour = uiSeconds / (60*60) ;
				
				uiSeconds -= uiHour*60*60;
				
				uint32 uiMiniute = uiSeconds / 60;
				
				uiSeconds -= uiMiniute*60;
				
				StrKeepTime.Format(L"%i天%i小时%i分%i秒",uiDay,uiHour,uiMiniute,uiSeconds);
				
				if (SerVerState.m_uiPingValue > 1000)
				{
					SerVerState.m_uiPingValue = 123;
				}
				
				//auiPing.Add(SerVerState.m_uiPingValue);

				// 得到上行速度
				double dSpeed;
				if (SerVerState.m_uiKeepSecond != 0)
				{
					dSpeed = (double)SerVerState.m_uiRecvBytes / (double)SerVerState.m_uiKeepSecond;
					if ( dSpeed <1024 )
					{
						StrSpeed.Format(L"%.2f B/S",dSpeed);
					}
					if (dSpeed >= 1024 && dSpeed < 1024*1024)
					{
						dSpeed /= 1024;
						StrSpeed.Format(L"%.2f KB/S",dSpeed);
					}
					else if ( dSpeed >= 1024*1024)
					{
						dSpeed = dSpeed / (1024*1024);
						StrSpeed.Format(L"%.2f MB/S",dSpeed);
					}					
				}
				else
				{
					StrSpeed = L"-";
				}
				

				if (ProxyInfo.m_eProxyType ==CProxyInfo::EPTNone)
				{
					//StrTips.Format(L"地址:%s::%i\nPing值:%i 速度:%s\n连接时间:%s\n",StrAddr,uiPort,SerVerState.m_uiPingValue,StrSpeed,StrKeepTime);																	
					StrTips.Format(L"地址:%s::%i\n速度:%s\n连接时间:%s\n", StrAddr.GetBuffer(), uiPort, StrSpeed.GetBuffer(), StrKeepTime.GetBuffer());																	
				}
				else
				{	
					//StrTips.Format(L"地址:%s::%i\n代理服务器:%s::%i\nPing值:%i 速度:%s\n连接时间:%s\n",StrAddr,uiPort,ProxyInfo.m_StrProxyAddr,ProxyInfo.m_iProxyPort,SerVerState.m_uiPingValue,StrSpeed,StrKeepTime);
					StrTips.Format(L"地址:%s::%i\n代理服务器:%s::%i\n速度:%s\n连接时间:%s\n", 
						StrAddr.GetBuffer(), uiPort, ProxyInfo.m_StrProxyAddr.GetBuffer(), ProxyInfo.m_iProxyPort, StrSpeed.GetBuffer(), StrKeepTime.GetBuffer());
				}
			}
			else
			{
				StrTips.Format(L"地址:%s::%i\n速度: -\n连接中断...\n",StrAddr.GetBuffer(), uiPort);																	
				StrKeepTime	= L"000";
				uiTest		= 99999;
				StrSpeed	= L"000";
			}
			SetColumnColorAccordPingValue(i+1,uiTest,SerVerState.m_bConnected);	
			//SetColumnColorAccordPingValue(i+1,SerVerState.m_uiPingValue,SerVerState.m_bConnected);

			m_aStrTipsText.Add(StrTips);
			aStrKeepTime.Add(StrKeepTime);
			aStrSpeed.Add(StrSpeed);
			auiPing.Add(uiTest);

		}

		//////////////////////////////////////////////////////////////////////////////////////
		// 设置表头
		if (m_GridCtrl.GetSafeHwnd())
		{
			for ( i = 1 ; i< m_GridCtrl.GetColumnCount();i++)
			{
				CGridCellLevel2 * pCell = (CGridCellLevel2 *)m_GridCtrl.GetCell(0, i);
				pCell->SetTiPTitle(pCell->GetText());
				pCell->SetContent(m_aStrTipsText[i-1]);

				CString StrText;
				pCell->SetTextClr(RGB(100,100,100));
				if (auiPing[i-1] == 99999)
				{					
					pCell->SetImage(0);
					StrText.Format(L"服务器%d: 连接中断...",i-1);
				}
				else
				{					
					if (m_GridCtrl.GetRowCount()>1 && m_GridCtrl.GetColumnCount()>1)
					{
						// 表头颜色和表内容颜色设为一致
						CGridCellSys * pCell11 =(CGridCellSys * )m_GridCtrl.GetCell(1,1);
						COLORREF color;
						if (m_bShowText)
						{
							color = pCell11->GetTextClr();
						}
						else
						{
							color = pCell11->GetBackClr();
						}
						
						pCell->SetTextClr(color);
					}
					
					pCell->SetImage(1);
					StrText.Format(L"服务器%d: 连接时间:%s 速度:%s", i, aStrKeepTime[i-1].GetBuffer(), aStrSpeed[i-1].GetBuffer()/*,auiPing[i-1]*/);
				}
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
				pCell->SetText(StrText);
			}
		}
		KillTimer(DLGNETWORKTIMER);
		SetTimer(DLGNETWORKTIMER,1000,NULL);		
	}
}

void CDlgNetWork::FillTable(CAbsCommunicationShow * pAbsCommunicationShow,int32 iMarketid,E_DataServiceType eDataServiceType,int32 iRow,int32 iCol)
{
	// 已知行列,和这行的服务,这列的服务器,判断这个格子是否打勾	
	//CGridCellSys *	pCell =(CGridCellSys *)m_GridCtrl.GetCell(iRow,iCol);
	//pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	//
	//if ( pAbsCommunicationShow->IsSupportService(iMarketid,eDataServiceType))
	//{				
	//	pCell->SetText(L"YES");		
	//}
	//else
	//{
	//	pCell->SetText(L"NO");	
	//}
}

void CDlgNetWork::SetColumnColorAccordPingValue(int32 iCol,uint32 uiPing,bool32 bConnect)
{	
	if (!m_GridCtrl.GetSafeHwnd())
		return;
	//if (uiPing > PINGMAX || uiPing < PINGMIN)
	//	return;

	uiPing = 200;

	COLORREF ColorThis = RGB(0,0,0);

	for (int32 j = 0 ; j < KiSpeedShowCounts ; j++ )
	{
		if ((int32)uiPing > KaSpeedShowObject[j].m_iPingLow && (int32)uiPing <= KaSpeedShowObject[j].m_iPingHigh)
		{
			ColorThis = KaSpeedShowObject[j].m_Color;
			break;
		}
	}

	for (int32 i = 1 ; i< m_GridCtrl.GetRowCount() ; i++)
	{
		CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(i,iCol);
		if (bConnect)
		{
			if (m_bShowText)
			{				
				pCell->SetTextClr(ColorThis);
			}
			else
			{
				if (pCell->GetBackClr() != RGB(0,0,0))
				{
					pCell->SetBackClr(ColorThis);
				}
				else
				{
					pCell->SetBackClr(RGB(0,0,0));
				}
				
			}			
		}
		else
		{
			// 断线的时候, 文字显示:
			if (m_bShowText)
			{
				pCell->SetTextClr(RGB(100,100,100));				
			}
			else
			{
				if (pCell->GetBackClr() != RGB(0,0,0))
				{
					pCell->SetBackClr(RGB(100,100,100));
				}
				else
				{
					pCell->SetBackClr(RGB(0,0,0));
				}
			}
			
		}		
	}
	m_GridCtrl.Invalidate();
}

void CDlgNetWork::ChangeShowStyle()
{
	if (m_bShowText)
	{
		// 原来是显示图形.现在显示文字
		for (int32 j = 1; j < m_GridCtrl.GetColumnCount(); j++ )
		{
			for (int32 i = 1; i< m_GridCtrl.GetRowCount(); i++)
			{
				CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(i,j);
				COLORREF col = pCell->GetBackClr();
				if ( col != RGB(0,0,0))
				{
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
					pCell->SetTextClr(col);
					pCell->SetText(L"YES");
					pCell->SetBackClr(RGB(0,0,0));
				}
				else if ( col == RGB(0,0,0))
				{
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
					pCell->SetTextClr(col);
					pCell->SetText(L"NO");
					pCell->SetBackClr(RGB(0,0,0));					
				}
			}
		}
	}
	else
	{
		// 原来是显示文字.现在显示图形
		for (int32 j = 1; j < m_GridCtrl.GetColumnCount(); j++ )
		{
			for (int32 i = 1; i< m_GridCtrl.GetRowCount(); i++)
			{
				CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(i,j);
				COLORREF col = pCell->GetTextClr();
				CString StrText = pCell->GetText();				
				if ( StrText == L"YES")
				{
					pCell->SetText(L"");
					pCell->SetBackClr(col);
				}
				else if ( StrText == L"NO")
				{
					pCell->SetText(L"");
					pCell->SetBackClr(RGB(0,0,0));
				}
			}
		}
		
	}
	m_GridCtrl.Invalidate();
}

void CDlgNetWork::AdjustToBestLayout(CRect rect)
{
	// 第一列稍大,后面的列较窄,显示条形图时比较美观
	if (!m_GridCtrl.GetSafeHwnd())
		return;

	int32 iColCounts = m_GridCtrl.GetColumnCount();
	if (iColCounts < 2)
		return;

	
	int32 iServerWidth = 100;
	int32 iServiceWidth = rect.Width() - iServerWidth*m_aCommunicationList.GetSize();
	
	if (iServiceWidth < iServerWidth)
	{
		m_GridCtrl.ExpandToFit();
		m_GridCtrl.Invalidate();
		return;
	}

	m_GridCtrl.SetColumnWidth(0,iServiceWidth);
	
	for (int32 i = 1 ; i< m_GridCtrl.GetColumnCount() -1 ;i++)
	{
		m_GridCtrl.SetColumnWidth(i,iServerWidth);
	}
	m_GridCtrl.Invalidate();
}