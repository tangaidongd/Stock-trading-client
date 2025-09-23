// dlgindexchoosestock.cpp : implementation file
//

#include "StdAfx.h"
#include "dlgindexchoosestock.h"
#include "dlgofflinedatatypechoose.h"
#include "dlgchoosestockblock.h"
#include "GridCellSys.h"
#include "GridCellSymbol.h"
#include "IoViewKLine.h"
#include <algorithm>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 不参加选股的板块
static const CString KaStrMarketExcept[] = 
{
	L"上证指数",
	L"上证基金",
	L"上证国债",
	L"上证债券",
	L"上证基金",
	L"上证其他",

	L"深证指数",
	L"深证基金",
	L"深证国债",
	L"深证债券",
	L"深证基金",
	L"深证其他",
};

static const int KiNumofMarketExcept = sizeof(KaStrMarketExcept) / sizeof (CString);

// 选股超时的定时器
static const int32 KTimerIdRecvChsStkData		= 5;		
static const int32 KTimerPeriodRecvChsStkData	= 1000;

static const int32 KiRecvChsStkDataTimeOut		= (1000 / KTimerPeriodRecvChsStkData) * 60 * 2;		// 2 分钟还没收到下一笔数据,认为超时

/////////////////////////////////////////////////////////////////////////////
// CDlgIndexChooseStock dialog


CDlgIndexChooseStock::CDlgIndexChooseStock(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgIndexChooseStock::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgIndexChooseStock)
	m_uiParam1 = 1;
	m_uiParam2 = 1;
	m_uiParam3 = 1;
	m_uiOwn	   = 10;
	m_bCheck1 = FALSE;
	m_bCheck3 = FALSE;
	m_bCheck2 = FALSE;
	m_StrKLineNums = _T("200");
	//}}AFX_DATA_INIT

	m_iRecvDataTimeSpan = 0;
	m_bBeginChoose	= false;
	m_eTimeInterval	= ENTICount;

	m_aBlocks.clear();
	m_aMapCycles.clear();
	m_aIndexChsStkResult.clear();
}


void CDlgIndexChooseStock::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgIndexChooseStock)
	DDX_Control(pDX, IDC_PROGRESS_REQ_WEIGHT, m_ProgressWeight);
	DDX_Control(pDX, IDC_PROGRESS_DOWN_KLINE, m_ProgressKLine);
	DDX_Control(pDX, IDC_PROGRESS_CALC_INDEX, m_ProgressCalc);
	DDX_Control(pDX, IDC_TREE_BLOCK, m_Tree);
	DDX_Control(pDX, IDC_COMBO_CYCLE, m_ComboCycle);
	DDX_Text(pDX, IDC_EDIT_PARAM1, m_uiParam1);
	DDX_Text(pDX, IDC_EDIT_PARAM2, m_uiParam2);
	DDX_Text(pDX, IDC_EDIT_PARAM3, m_uiParam3);
	DDX_Text(pDX, IDC_EDIT_OWN, m_uiOwn);
	DDX_Check(pDX, IDC_CHECK1, m_bCheck1);
	DDX_Check(pDX, IDC_CHECK3, m_bCheck3);
	DDX_Check(pDX, IDC_CHECK2, m_bCheck2);
	DDX_Text(pDX, IDC_STATIC_KLINENUMS, m_StrKLineNums);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgIndexChooseStock, CDialogEx)
	//{{AFX_MSG_MAP(CDlgIndexChooseStock)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_BEGIN, OnButtonBegin)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, OnButtonExport)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnButtonExit)
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_COMBO_CYCLE, OnSelchangeComboCycle)
	ON_MESSAGE(UM_Index_Choose_Stock_Progress, OnMsgProgess)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_NOTIFY(NM_DBLCLK, 656326, OnGridDblClick)
	ON_EN_CHANGE(IDC_EDIT_PARAM1, OnChangeEditParam1)
	ON_EN_CHANGE(IDC_EDIT_PARAM2, OnChangeEditParam2)
	ON_EN_CHANGE(IDC_EDIT_PARAM3, OnChangeEditParam3)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_BN_CLICKED(IDC_CHECK2, OnCheck2)
	ON_BN_CLICKED(IDC_CHECK3, OnCheck3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgIndexChooseStock message handlers

BOOL CDlgIndexChooseStock::PreTranslateMessage(MSG* pMsg)
{
	if ( WM_KEYDOWN == pMsg->message)
	{
		if (VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam)
		{
			PostMessage(WM_CLOSE,0,0);
		}
	}	
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgIndexChooseStock::OnSize(UINT nType, int cx, int cy)
{
	if ( m_GridCtrl.GetSafeHwnd() )
	{
		CRect rectGrid;
		GetDlgItem(IDC_STATIC_GRID)->GetClientRect(&rectGrid);		
		m_GridCtrl.MoveWindow(rectGrid);
	}
	
	CDialogEx::OnSize(nType, cx, cy);
}

BOOL CDlgIndexChooseStock::OnInitDialog()
{
	CDialog::OnInitDialog();

	if ( !ConstructGrid() )
	{
		//ASSERT(0);
		return FALSE;
	}

	InitialControls();
	
	return TRUE;
}

void CDlgIndexChooseStock::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}

BOOL CDlgIndexChooseStock::DestroyWindow()
{
	StopCheckTimeOutTimer();

	return CDialogEx::DestroyWindow();
}

void CDlgIndexChooseStock::OnButtonClear() 
{
	HTREEITEM hItemRoot = m_Tree.GetRootItem();
	if ( NULL == hItemRoot )
	{
		return;
	}
	
	while( hItemRoot )
	{
		TravelTreeItem(hItemRoot, ETTPUnCheck);
		hItemRoot = m_Tree.GetNextSiblingItem(hItemRoot);
	}		
}

void CDlgIndexChooseStock::OnClose()
{
	bool32 bClose = false;

	if ( m_bBeginChoose )
	{
		if ( IDYES == MessageBox(L"是否退出选股?", L"退出选股", MB_YESNO) )
		{	
			m_MidCore.StopChooseStock();
			bClose = true;
		}
	}
	else
	{	
		bClose = true;
	}		
	
	

	//
	if ( bClose )
	{
		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
		if ( NULL != pDoc->m_pAbsCenterManager )
		{
			pDoc->m_pAbsCenterManager->RemoveAttendMerch(EA_Choose);
		}

		//
		CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
		pWnd->m_pDlgIndexChooseStock = NULL;
		
		DestroyWindow();
	}
}

void CDlgIndexChooseStock::OnTimer(UINT nIDEvent)
{
	if ( KTimerIdRecvChsStkData == nIDEvent )
	{		
		++m_iRecvDataTimeSpan;

		// TRACE(L"计时器: %d. 总数: %d! \n", m_iRecvDataTimeSpan, KiRecvChsStkDataTimeOut);
		if ( m_iRecvDataTimeSpan >= KiRecvChsStkDataTimeOut )
		{
			KillTimer(KTimerIdRecvChsStkData);
			// 超时了, 通知处理错误
			m_MidCore.OnRecvTimeOut();
		}
	}
	else
	{
		CDialogEx::OnTimer(nIDEvent);
	}
}

void CDlgIndexChooseStock::OnSelchangeComboCycle() 
{
	UpdateData(TRUE);

	// 周期选框改变:
	CString StrText;
	int32 iCurSel = m_ComboCycle.GetCurSel();
	m_ComboCycle.GetLBText(iCurSel, StrText);

	std::map<CString, E_NodeTimeInterval>::iterator itFind = m_aMapCycles.find(StrText);
	if ( itFind != m_aMapCycles.end() )
	{
		E_NodeTimeInterval eNodeTimeInterval = itFind->second;

		if ( eNodeTimeInterval == ENTIMinuteUser )
		{
			// 自定义分钟线
			GetDlgItem(IDC_STATIC_OWN1)->EnableWindow(TRUE);			
			GetDlgItem(IDC_STATIC_OWN2)->EnableWindow(TRUE);			
			GetDlgItem(IDC_EDIT_OWN)->EnableWindow(TRUE);	

			GetDlgItem(IDC_STATIC_OWN2)->SetWindowText(L"分");
		}
		else if ( eNodeTimeInterval == ENTIDayUser ) 
		{
			// 自定义日线
			GetDlgItem(IDC_STATIC_OWN1)->EnableWindow(TRUE);			
			GetDlgItem(IDC_STATIC_OWN2)->EnableWindow(TRUE);			
			GetDlgItem(IDC_EDIT_OWN)->EnableWindow(TRUE);	
			
			GetDlgItem(IDC_STATIC_OWN2)->SetWindowText(L"日");
		}
		else
		{
			// 自定义分钟线
			GetDlgItem(IDC_STATIC_OWN1)->EnableWindow(FALSE);			
			GetDlgItem(IDC_STATIC_OWN2)->EnableWindow(FALSE);			
			GetDlgItem(IDC_EDIT_OWN)->EnableWindow(FALSE);						
		}
	}

	UpdateData(FALSE);
}
 
void CDlgIndexChooseStock::OnChangeEditParam1() 
{	
	SetStatickKlineNums();
}

void CDlgIndexChooseStock::OnChangeEditParam2() 
{	
	SetStatickKlineNums();
}

void CDlgIndexChooseStock::OnChangeEditParam3() 
{	
	SetStatickKlineNums();
}

void CDlgIndexChooseStock::OnCheck1() 
{
	SetStatickKlineNums();
}

void CDlgIndexChooseStock::OnCheck2() 
{
	SetStatickKlineNums();
}

void CDlgIndexChooseStock::OnCheck3() 
{
	SetStatickKlineNums();	
}

void CDlgIndexChooseStock::OnButtonBegin() 
{	
	// 判断数据有效性:
	if ( !BeParamsValid() )
	{
		return;
	}

	// 开始选股
	CIndexChsStkMideCore::E_OfflineDataGetType eOfflineDataGetType = CIndexChsStkMideCore::EODGTNone;
	
	CDlgOffLineDataTypeChoose Dlg;
	if ( IDOK == Dlg.DoModal() )
	{
		int32 iType = Dlg.GetDownLoadType();
		eOfflineDataGetType = (CIndexChsStkMideCore::E_OfflineDataGetType)iType;

		if (eOfflineDataGetType >= CIndexChsStkMideCore::EODGTCount)
		{
			return;
		}			
	}
	else
	{
		return;
	}

	// 设置参数
	int32 iMerchNums = 0;

	std::vector<T_IndexChsStkCondition> aConditions;
	
	if ( m_bCheck1 || m_bCheck2 || m_bCheck3 )
	{
		T_IndexChsStkCondition stIndexChsStkCondition;
		
		stIndexChsStkCondition.m_bNeedCalc = m_bCheck1;
		stIndexChsStkCondition.m_uiParam   = m_uiParam1;	
		aConditions.push_back(stIndexChsStkCondition);
		
		stIndexChsStkCondition.m_bNeedCalc = m_bCheck2;
		stIndexChsStkCondition.m_uiParam   = m_uiParam2;	
		aConditions.push_back(stIndexChsStkCondition);
		
		stIndexChsStkCondition.m_bNeedCalc = m_bCheck3;
		stIndexChsStkCondition.m_uiParam   = m_uiParam3;	
		aConditions.push_back(stIndexChsStkCondition);
	}
	
	//
	m_MidCore.SetChsStkParams(iMerchNums, eOfflineDataGetType, m_aBlocks, aConditions, m_eTimeInterval, m_uiOwn);

	// 设置滚动条的范围
	if ( iMerchNums <= 0 )
	{
		//ASSERT(0);
		return;
	}

	//
	m_ProgressWeight.SetRange(0, iMerchNums);
	m_ProgressKLine.SetRange(0, iMerchNums);
	m_ProgressCalc.SetRange(0, iMerchNums);

	//
	ResetProgress();

	// 开始选股:
	m_MidCore.SetParentDlg(this);
	
	if ( m_MidCore.BeginChooseStock() )
	{
		m_bBeginChoose = true;
		//
		ChangeControlState(false);
	}
	else
	{
		// 
		m_MidCore.StopChooseStock();
		MessageBox(L"选股失败!", L"指标选股", MB_ICONWARNING);			
	}
}

void CDlgIndexChooseStock::OnButtonExport() 
{
	if ( m_aIndexChsStkResult.size() <= 0 )
	{
		MessageBox(L"没有可导出的商品", L"导出板块", MB_ICONWARNING);
		return;
	}

	// 打开板块对话框:
	CDlgChooseStockBlock Dlg;
    
	if ( IDOK == Dlg.DoModal() ) 
	{
		T_BlockDesc Block = Dlg.m_BlockFinal;

		// 导出到板块中:
		for( int32 i = 0; i < (int32)m_aIndexChsStkResult.size(); i++ )
		{
			CMerch* pMerch = m_aIndexChsStkResult[i].m_pMerch;
			
			if ( NULL == pMerch )
			{
				continue;
			}

			CUserBlockManager::Instance()->AddMerchToUserBlock(pMerch, Block.m_StrBlockName, false);
		}

		// 商品多的时候, 提高性能, 只通知和保存文件一次.
		CUserBlockManager::Instance()->SaveXmlFile();
		CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);
	
		// 打开相应版块
        CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
        pMainFrame->OnSpecifyBlock(Block, false);
	}
}
 
void CDlgIndexChooseStock::OnButtonExit() 
{
	// 退出选股
	if ( m_bBeginChoose )
	{
		if ( IDYES == MessageBox(L"是否退出选股?", L"退出选股", MB_YESNO) )
		{
			//
			m_bBeginChoose = false;			
			m_MidCore.StopChooseStock();
		}
		
		UpdateData(false);
	}	
}

bool32 CDlgIndexChooseStock::ConstructGrid()
{
	if ( !m_GridCtrl.GetSafeHwnd() )
	{
		if ( !m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 656326) )
		{
			return false;
		}
	}
 
	//
 	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetMargin(0);
 	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetMargin(0);
 	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetMargin(0);
 	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetMargin(0);
 	
 	m_GridCtrl.SetEditable(false);
 	m_GridCtrl.EnableSelection(false);
	m_GridCtrl.SetHeaderSort(true);

 	m_GridCtrl.SetFixedRowCount(1);
 	m_GridCtrl.SetFixedColumnCount(1);
 	
 	m_GridCtrl.SetColumnCount(3);
 	CGridCellBase *pCell = NULL;
 	pCell = m_GridCtrl.GetCell(0, 0);
 	pCell->SetFormat(DT_CENTER);
	pCell->SetText(L"名称");
 	
 	pCell = m_GridCtrl.GetCell(0, 1);
	pCell->SetFormat(DT_RIGHT);
 	pCell->SetText(L"准确率");
 	
 	pCell = m_GridCtrl.GetCell(0, 2);
	pCell->SetFormat(DT_RIGHT);
 	pCell->SetText(L"收益率");
 	
 	// 创建滚动条
 	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
 	m_XSBVert.SetScrollRange(0, 10);
 	
 	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
 	m_XSBHorz.SetScrollRange(0, 0);
 	m_XSBHorz.ShowWindow(SW_HIDE);
 	
 	// 设置相互之间的关联
 	m_XSBHorz.SetOwner(&m_GridCtrl);
 	m_XSBVert.SetOwner(&m_GridCtrl);
 	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);

	// 设置位置:
	CRect rectGrid;
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(&rectGrid);
	ScreenToClient(&rectGrid);	
	m_GridCtrl.MoveWindow(&rectGrid);
	m_GridCtrl.ShowWindow(SW_SHOW);

	// 滚动条
	CRect RectSB(rectGrid);
	RectSB.left = rectGrid.right;
	RectSB.right= RectSB.left + 1;
	m_XSBVert.SetSBRect(RectSB, FALSE);

	//
	m_GridCtrl.AutoSizeRows();	
	m_GridCtrl.ExpandColumnsToFit();
	
	//
	return true;
}

void CDlgIndexChooseStock::InitialControls()
{
	// 树的图片
	m_ImageTreeState.Create(IDB_TREE_STATE, 13, 1, RGB(255, 255, 255));
	m_Tree.SetImageList(&m_ImageTreeState, TVSIL_STATE);

	// 周期(组合框):		
	for ( int32 iTimeInterval = (int32)ENTIMinute; iTimeInterval < ENTICount; iTimeInterval++ )
	{
		CString StrTimeInterval = TimeInterval2String((E_NodeTimeInterval)iTimeInterval, 10, 10);
		m_ComboCycle.InsertString(-1, StrTimeInterval);
		
		//
		m_aMapCycles[StrTimeInterval] = (E_NodeTimeInterval)iTimeInterval;
	}

	m_ComboCycle.SetCurSel((int32)ENTIDay);

	// 板块(树), 除了几个不要选股的板块以外, 插入所有板块:
	CArray<T_BlockMenuInfo, T_BlockMenuInfo&> aBlockMenuInfo; 
    CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
    pMainFrame->GetBlockMenuInfo(aBlockMenuInfo);
	
	HTREEITEM hItemRoot = NULL;

    for ( int32 i = 0 ; i < aBlockMenuInfo.GetSize() ; i ++)
    {
        T_BlockMenuInfo Block = aBlockMenuInfo.GetAt(i);

		if ( 0 == Block.iBlockMenuIDs )
		{
			hItemRoot = m_Tree.InsertItem(Block.StrBlockMenuNames);
		}
        else 
        {
			bool32 bExcept = false;
			
			for ( int32 j = 0; j < KiNumofMarketExcept; j++ )
			{
				if ( 0 == Block.StrBlockMenuNames.CompareNoCase(KaStrMarketExcept[j]) )
				{
					bExcept = true;
					break;
				}
			}
			
			if ( bExcept )
			{
				continue;
			}
			
			//
            m_Tree.InsertItem(Block.StrBlockMenuNames, hItemRoot);
        }
    }
	
	// 进度条
	ResetProgress();
}

void CDlgIndexChooseStock::ChangeControlState(bool32 bEnable)
{
	// 设置控件
	GetDlgItem(IDC_COMBO_CYCLE)->EnableWindow(bEnable);	
	GetDlgItem(IDC_TREE_BLOCK)->EnableWindow(bEnable);	
	GetDlgItem(IDC_CHECK1)->EnableWindow(bEnable);	
	GetDlgItem(IDC_CHECK2)->EnableWindow(bEnable);	
	GetDlgItem(IDC_CHECK3)->EnableWindow(bEnable);	
	GetDlgItem(IDC_EDIT_PARAM1)->EnableWindow(bEnable);	
	GetDlgItem(IDC_EDIT_PARAM2)->EnableWindow(bEnable);	
	GetDlgItem(IDC_EDIT_PARAM3)->EnableWindow(bEnable);	
	GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(bEnable);	
	GetDlgItem(IDC_BUTTON_BEGIN)->EnableWindow(bEnable);	
	GetDlgItem(IDC_BUTTON_EXPORT)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_A)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_B)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_C)->EnableWindow(bEnable);	
	GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(true);


	m_GridCtrl.EnableWindow(bEnable);	
	m_XSBHorz.EnableWindow(bEnable);
	m_XSBVert.EnableWindow(bEnable);
}

void CDlgIndexChooseStock::SetStatickKlineNums()
{
	UpdateData(TRUE);

	if ( m_bCheck1 && !m_bCheck2 && !m_bCheck3 )
	{
		int32 iKLineNums = m_uiParam1;
		m_StrKLineNums.Format(L"%u", iKLineNums);
	}	
	else if ( m_bCheck2 && !m_bCheck1 && !m_bCheck3 )
	{
		int32 iKLineNums = m_uiParam2;
		m_StrKLineNums.Format(L"%u", iKLineNums);
	}	
	else if ( m_bCheck3 && !m_bCheck1 && !m_bCheck2 )
	{
		int32 iKLineNums = m_uiParam3;
		m_StrKLineNums.Format(L"%u", iKLineNums);
	}
	else if ( m_bCheck1 && m_bCheck2 && !m_bCheck3 )
	{
		int32 iKLineNums = m_uiParam1 >= m_uiParam2 ? m_uiParam1 : m_uiParam2;
		m_StrKLineNums.Format(L"%u", iKLineNums);
	}
	else if ( m_bCheck1 && m_bCheck3 && !m_bCheck2 )
	{
		int32 iKLineNums = m_uiParam1 >= m_uiParam3 ? m_uiParam1 : m_uiParam3;
		m_StrKLineNums.Format(L"%u", iKLineNums);
	}
	else if ( m_bCheck2 && m_bCheck3 && !m_bCheck1 )
	{
		int32 iKLineNums = m_uiParam2 >= m_uiParam3 ? m_uiParam2 : m_uiParam3;
		m_StrKLineNums.Format(L"%u", iKLineNums);
	}
	else if ( !m_bCheck1 && !m_bCheck2 && !m_bCheck3 )
	{
		m_StrKLineNums.Format(L"%u", KiStatisticaKLineNums);
	}	
	else if ( m_bCheck1 && m_bCheck2 && m_bCheck3 )
	{
		int32 iParam3 = m_uiParam3;
		int32 iKLineNums = m_uiParam1 >= m_uiParam2 ? m_uiParam1 : m_uiParam2;
		iKLineNums = iKLineNums >= iParam3 ? iKLineNums : iParam3;
		
		m_StrKLineNums.Format(L"%u", iKLineNums);
	}

	UpdateData(FALSE);
}

void CDlgIndexChooseStock::ResetProgress()
{
	// 颜色
	m_ProgressWeight.SetForeColour(RGB(0, 0, 255));
	m_ProgressKLine.SetForeColour(RGB(255, 0, 0));
	m_ProgressCalc.SetForeColour(RGB(0, 255, 0));

	// 进度
	m_ProgressWeight.SetPos(0);
	m_ProgressKLine.SetPos(0);
	m_ProgressCalc.SetPos(0);
	
	// 文字
	m_ProgressWeight.SetWindowText(L"");
	m_ProgressKLine.SetWindowText(L"");
	m_ProgressCalc.SetWindowText(L"");

	RedrawWindow();
}

bool32 CDlgIndexChooseStock::BeParamsValid()
{
	UpdateData(TRUE);

	// 1: 周期:
	int32 iCurSel = m_ComboCycle.GetCurSel();
	if ( iCurSel < 0 )
	{
		MessageBox(L"请选择正确周期!", L"周期错误", MB_ICONWARNING);		
		return false;
	}

	//
	CString StrText;
	m_ComboCycle.GetLBText(iCurSel, StrText);
	std::map<CString, E_NodeTimeInterval>::iterator itFind = m_aMapCycles.find(StrText);

	if ( itFind ==  m_aMapCycles.end() )
	{
		MessageBox(L"请选择正确周期!", L"周期错误", MB_ICONWARNING);
		return false;
	}

	//
	if ( (ENTIMinuteUser == itFind->second && (m_uiOwn < USERSET_MINUET_MIN || m_uiOwn >= USERSET_MINUET_MAX))
	   ||(ENTIDayUser    == itFind->second && (m_uiOwn < USERSET_DAY_MIN || m_uiOwn >= USERSET_DAY_MAX)) )
	{
		MessageBox(L"请选择正确周期!", L"周期错误", MB_ICONWARNING);		
		return false;
	}
	
	m_eTimeInterval = itFind->second;

	// 2: 板块
	if ( !GetSelectedBlocks() )
	{
		MessageBox(L"请选择板块!", L"板块错误", MB_ICONWARNING);
		return false;
	}

	// 3: 选股条件:
	if ( m_bCheck1 && (m_uiParam1 >= 200 || m_uiParam1 < 1) )
	{
		MessageBox(L"K 线根数应在 1 - 200 之间!", L"参数错误", MB_ICONWARNING);
		return false;
	}

	if ( m_bCheck2 && (m_uiParam2 >= 200 || m_uiParam2 < 1) )
	{
		MessageBox(L"K 线根数应在 1 - 200 之间!", L"参数错误", MB_ICONWARNING);
		return false;
	}

	if ( m_bCheck3 && (m_uiParam3 >= 200 || m_uiParam3 < 1) )
	{
		MessageBox(L"K 线根数应在 1 - 200 之间!", L"参数错误", MB_ICONWARNING);
		return false;
	}

	return true;
}

void CDlgIndexChooseStock::TravelTreeItem(HTREEITEM hItem, E_TravelTreePurpose ePurpose)
{
	if ( NULL == hItem )
	{
		return;
	}
	
	CString StrText = m_Tree.GetItemText(hItem);
	
	
	if ( ETTPGetCheck == ePurpose )
	{
		if ( m_Tree.GetCheck(hItem) && NULL != m_Tree.GetParentItem(hItem) )
		{
			m_aBlocks.push_back(StrText);
		}
	}
	else if ( ETTPUnCheck == ePurpose )
	{
		m_Tree.SetCheck(hItem, FALSE);
	}
	else
	{
		//ASSERT(0);
	}
	
	HTREEITEM hItemChild = m_Tree.GetChildItem(hItem);
	
	// 
	while ( hItemChild )
	{	
		TravelTreeItem(hItemChild, ePurpose);	
		hItemChild = m_Tree.GetNextSiblingItem(hItemChild);		
	}
}

bool32 CDlgIndexChooseStock::GetSelectedBlocks()
{
	// 得到选中的板块下的商品:
	m_aBlocks.clear();
	
	HTREEITEM hItemRoot = m_Tree.GetRootItem();
	if ( NULL == hItemRoot )
	{
		return false;
	}

	while( hItemRoot )
	{
		TravelTreeItem(hItemRoot, ETTPGetCheck);
		hItemRoot = m_Tree.GetNextSiblingItem(hItemRoot);
	}

	// 得到所有板块:
	if ( m_aBlocks.size() <= 0 )
	{
		return false;
	}

	return true;
}

LRESULT CDlgIndexChooseStock::OnMsgProgess(WPARAM wParam, LPARAM lParam)
{
	CString *pStrMsg = (CString *)wParam;
	CString StrMsg	 = *pStrMsg;
	DEL(pStrMsg);

	if ( !m_bBeginChoose )
	{
		return 0;
	}

	//
	E_ProgressType eType = (E_ProgressType)lParam;

	//
	if ( EPTWeight == eType )
	{				
		//
		m_ProgressWeight.StepIt();

		CString StrPos = m_ProgressWeight.GetPosText();
		CString StrShow;
		StrShow.Format(L"%s [%s]", StrMsg.GetBuffer(), StrPos.GetBuffer());
		m_ProgressWeight.SetWindowText(StrShow);		
	}
	else if ( EPTKLine == eType )
	{
		// 清空超时计数器
		m_iRecvDataTimeSpan = 0;

		//		
		m_ProgressKLine.StepIt();

		CString StrPos = m_ProgressKLine.GetPosText();
		CString StrShow;
		StrShow.Format(L"%s [%s]", StrMsg.GetBuffer(), StrPos.GetBuffer());
		m_ProgressKLine.SetWindowText(StrShow);
		
		// TRACE(L"下载数据: %s \n", StrShow);		
	}
	else if ( EPTCalc == eType )
	{
		//
		m_ProgressCalc.StepIt();

		CString StrPos = m_ProgressCalc.GetPosText();
		CString StrShow;
		StrShow.Format(L"%s [%s]", StrMsg.GetBuffer(), StrPos.GetBuffer());
		m_ProgressCalc.SetWindowText(StrShow);

		// TRACE(L"计算指标: %s \n", StrShow);
	}
	else if ( EPTFinish == eType )
	{
		// 停止计时器
		// StopCheckTimeOutTimer();

		ResetProgress();
		DisplayDatas();
		m_MidCore.OnChooseStockFinish();
		
		//
		m_bBeginChoose = false;
		ChangeControlState(true);
		
		//
		RedrawWindow();
	}
	else 
	{
		//ASSERT(0);
	}
	
	// 显示数据
	UpdateData(false);

	return 0;
}

void CDlgIndexChooseStock::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	
	int32 iRow = pItem->iRow;
	CGridCellBase* pBaseCell = m_GridCtrl.GetCell(iRow, 0);	
	if ( NULL == pBaseCell )
	{
		return;
	}
	
	//
	CString StrText = pBaseCell->GetText();
	CMerch* pMerch = (CMerch*)pBaseCell->GetData();			
	
	if ( (NULL != pMerch) && (pMerch->m_MerchInfo.m_StrMerchCnName == StrText) )
	{
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
		if ( NULL == pApp )
		{
			return;
		}
		
		//
		CGGTongDoc* pDoc = (CGGTongDoc*)pApp->m_pDocument;
		if ( NULL == pDoc )
		{
			return;
		}
		
		//
		CAbsCenterManager* pAbsCenterManager = pDoc->m_pAbsCenterManager;
		if ( NULL == pAbsCenterManager )
		{
			return;
		}

		// 找一个当前激活的K 线图来响应这个消息
		CIoViewBase* pIoViewBase = NULL;
		for ( int32 i = 0; i < pMainFrame->m_IoViewsPtr.GetSize(); i++ )
		{
			CIoViewBase* pIoView = pMainFrame->m_IoViewsPtr[i];

			if ( NULL != pIoView )
			{
				if ( pIoView->IsWindowVisible() && pIoView->IsKindOf(RUNTIME_CLASS(CIoViewKLine)) )
				{
					pIoViewBase = pIoView;
					break;
				}
			}
		}
		
		//
		if ( NULL == pIoViewBase || !pIoViewBase->IsKindOf(RUNTIME_CLASS(CIoViewKLine)) )
		{
			// 新建一个K 线图:
			pIoViewBase = pMainFrame->CreateIoViewByPicMenuID(ID_PIC_KLINE, true);
		}
		
		pIoViewBase->BringToTop();

		// 切换商品		
		if ( NULL != pIoViewBase && pIoViewBase->IsKindOf(RUNTIME_CLASS(CIoViewKLine)) )
		{
			pMainFrame->OnViewMerchChanged(pIoViewBase, pMerch);
		}
	}		
}

void CDlgIndexChooseStock::SetIndexChsStkResult(const std::vector<T_IndexChsStkResult>& aIndexChsStkResult)
{
	m_aIndexChsStkResult.clear();
	m_aIndexChsStkResult = aIndexChsStkResult;	
	
	// 避免出现重复的数据:
	std::vector<T_IndexChsStkResult>::iterator itEnd = std::unique(m_aIndexChsStkResult.begin(), m_aIndexChsStkResult.end());	
	m_aIndexChsStkResult.erase(itEnd, m_aIndexChsStkResult.end());
}

void CDlgIndexChooseStock::DisplayDatas()
{	
	int32 iTestSYL = 0;
	int32 iTestZQL = 0;

	//
	m_GridCtrl.DeleteNonFixedRows();
	m_GridCtrl.Refresh();
	m_GridCtrl.RedrawWindow();

	// 显示数据
	if ( m_aIndexChsStkResult.empty() )
	{
		return;
	}

	//
	for ( int32 i = m_aIndexChsStkResult.size() - 1; i >= 0; i-- )
	{
		T_IndexChsStkResult stIndexChsStkResult = m_aIndexChsStkResult[i];
		if ( NULL == stIndexChsStkResult.m_pMerch )
		{
			continue;
		}

		// 插入一行
		
		if ( 1 == m_GridCtrl.GetRowCount() )
		{
			m_GridCtrl.InsertRow(L"");
		}
		else
		{
			m_GridCtrl.InsertRow(L"", 1);
		}

		CString StrText;

		// 名称
		CGridCellBase* pCell = m_GridCtrl.GetCell(1, 0);
		StrText = stIndexChsStkResult.m_pMerch->m_MerchInfo.m_StrMerchCnName;
		pCell->SetFormat(DT_LEFT);
		pCell->SetText(StrText);
		pCell->SetData(LPARAM(stIndexChsStkResult.m_pMerch));

		// 准确率		
		pCell = m_GridCtrl.GetCell(1, 1);
		pCell->SetFormat(DT_RIGHT);
		COLORREF clr;
		if ( stIndexChsStkResult.m_fAccuracyRate * 100 >= (float)50.0 )
		{
			clr = RGB(255, 0, 0);
			iTestZQL += 1;
		}
		else
		{
			clr = RGB(224, 128, 224);
		}
		pCell->SetTextClr(clr);
		StrText = Float2String(stIndexChsStkResult.m_fAccuracyRate * 100, 2, false, false, true);
		pCell->SetCompareFloat(stIndexChsStkResult.m_fAccuracyRate);
		pCell->SetText(StrText);

		// 收益率
		m_GridCtrl.SetCellType(1, 2, RUNTIME_CLASS(CGridCellSymbol));
		CGridCellSymbol* pCellSymbol = (CGridCellSymbol*)m_GridCtrl.GetCell(1, 2);
		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
		pCellSymbol->SetFormat(DT_RIGHT);
		StrText = Float2SymbolString(stIndexChsStkResult.m_fProfitability * 100, 0.0, 2, false, false, true);
		pCellSymbol->SetCompareFloat(stIndexChsStkResult.m_fProfitability);
		pCellSymbol->SetText(StrText);

		if ( stIndexChsStkResult.m_fProfitability >= 0 )
		{
			iTestSYL += 1;
		}
	}	

	m_GridCtrl.AutoSizeRows();		
	m_GridCtrl.Refresh();
	m_GridCtrl.RedrawWindow();

	FILE* pFile = fopen("woyaotiaozhibiao", "r+");
	if ( NULL != pFile )
	{
		CString StrTest;
		StrTest.Format(L"准确率 >= 50%% 有 %d 个. 收益率 >= 0 有 %d 个", iTestZQL, iTestSYL);
		SetWindowText(StrTest);
		fclose(pFile);

		UpdateData(FALSE);
	}
}

void CDlgIndexChooseStock::StartCheckTimeOutTimer()
{
	// TRACE(L"设置定时器! \n");
	SetTimer(KTimerIdRecvChsStkData, KTimerPeriodRecvChsStkData, NULL);
}

void CDlgIndexChooseStock::StopCheckTimeOutTimer()
{
	// TRACE(L"停止定时器! \n");
	m_iRecvDataTimeSpan = 0;
	KillTimer(KTimerIdRecvChsStkData);	
}
