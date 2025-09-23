// DlgZhongCangChiGu.cpp : implementation file
//

#include "stdafx.h"

#include "DlgZhongCangChiGu.h"
#include "MerchManager.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgZhongCangChiGu dialog
struct _ZhongCangChiGuStaticId
{
	int			iTextId;		// 相关文字控件id
	int			iColorFlagId;	// 颜色显示id
	COLORREF	clr;			// 绑定的颜色
};

static _ZhongCangChiGuStaticId s_StaticIds[] = 
{
	{IDC_STATIC_1, IDC_STATIC_PIC1, RGB(230,20,20) },
	{IDC_STATIC_2, IDC_STATIC_PIC2, RGB(230,175,20) },
	{IDC_STATIC_3, IDC_STATIC_PIC3, RGB(43,61,6) },
	{IDC_STATIC_4, IDC_STATIC_PIC4, RGB(54,203,39) },
	{IDC_STATIC_5, IDC_STATIC_PIC5, RGB(39,205,164) },
	{IDC_STATIC_6, IDC_STATIC_PIC6, RGB(39,149,205) },
	{IDC_STATIC_7, IDC_STATIC_PIC7, RGB(27,69,228) },
	{IDC_STATIC_8, IDC_STATIC_PIC8, RGB(136,20,228) },
	{IDC_STATIC_9, IDC_STATIC_PIC9, RGB(230,20,228) },
	{IDC_STATIC_10, IDC_STATIC_PIC10, RGB(0,0,255) }
};
const int32 s_KStaticIdCount = sizeof(s_StaticIds)/sizeof(s_StaticIds[0]);

CDlgZhongCangChiGu::CDlgZhongCangChiGu(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgZhongCangChiGu::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgZhongCangChiGu)
	//}}AFX_DATA_INIT

	m_pMerch = NULL;
	m_iSortCol = -1;
	m_bSortDesc = TRUE;
	m_bDataAlready = false;
	m_iStage = 0;
}

void CDlgZhongCangChiGu::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgZhongCangChiGu)
	DDX_Control(pDX, IDC_LIST1, m_WndList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgZhongCangChiGu, CDialogEx)
	//{{AFX_MSG_MAP(CDlgZhongCangChiGu)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, OnColumnClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// 看数据是否返回了
static const int s_KiTimerIDGetData			= 10000;
static const int s_KiTimerPeriodGetData		= 500;

/////////////////////////////////////////////////////////////////////////////
// CDlgZhongCangChiGu message handlers
BOOL CDlgZhongCangChiGu::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 请求数据
	{
		SetTimer(s_KiTimerIDGetData, s_KiTimerPeriodGetData, NULL);
	}

	//
	m_WndPie.SubclassDlgItem(IDC_STATIC_PIE, this);
	
	if ( NULL == m_pMerch )
	{
		EndDialog(IDCANCEL);
	}

	srand(time(NULL));

	InitShow();

	RequestViewData();

	CString StrTilte;
	GetWindowText(StrTilte);
	CString StrMerch;
	StrMerch.Format(_T("[%s-%s] "), m_pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), m_pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer());
	SetWindowText(StrMerch + StrTilte);

	//
	return TRUE;
}

BOOL CDlgZhongCangChiGu::PreTranslateMessage(MSG* pMsg)
{
	if ( WM_KEYDOWN == pMsg->message)
	{
		if (VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam)
		{
			//OnClose();
		}
	}	
	
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDlgZhongCangChiGu::OnDestroy()
{
	m_WndPie.UnsubclassWindow();

	CDialogEx::OnDestroy();
}

void CDlgZhongCangChiGu::SetMerch( CMerch *pMerch )
{
	ASSERT( NULL != pMerch );
	m_pMerch = pMerch;
}

void CDlgZhongCangChiGu::OnDataUpdate( const ZhongCangChiGuArray &aData )
{
	m_aZhongCangChiGu.Copy(aData);
	int32 i=0;

	ClearSortFlag();

	m_WndList.DeleteAllItems();
	m_WndPie.Reset();

	// 表格数据更新
	CString StrTmp;
	float fTotalChiGu = 0.0f;
	for ( i=0; i < m_aZhongCangChiGu.GetSize() ; ++i )
	{
		const T_ZhongCangChiGuPseudo &Data = m_aZhongCangChiGu[i];
		StrTmp.Format(_T("%d"), i+1);
		int32 iItem = m_WndList.InsertItem(i, StrTmp);
		ASSERT( iItem == i );
		m_WndList.SetItemText(iItem, 1, Data.StrMerchName);

		StrTmp.Format(_T("%0.2f"), Data.fBenQiChiGu/10000.0f);
		m_WndList.SetItemText(iItem, 2, StrTmp);
		fTotalChiGu += Data.fBenQiChiGu;

		StrTmp.Format(_T("%0.2f"), Data.fShangShiGongSi*100);
		m_WndList.SetItemText(iItem, 3, StrTmp);

		StrTmp.Format(_T("%0.2f"), Data.fJiJinJingZhi*100);
		m_WndList.SetItemText(iItem, 4, StrTmp);

		StrTmp.Format(_T("%0.2f"), Data.fShangQiChiGu/10000);
		m_WndList.SetItemText(iItem, 5, StrTmp);
	}

	// 前10数据描述更新，超过10的，第10用其它代替
	// 不足10的，后面的隐藏
	ASSERT( 10 == s_KStaticIdCount );
	if ( m_aZhongCangChiGu.GetSize() > s_KStaticIdCount )
	{
		// 最后一行是其它
		ASSERT( fTotalChiGu > 1.0f );
		float fRatePre10 = 0.0f;
		for ( i=0; i < s_KStaticIdCount-1 ; ++i )
		{
			const T_ZhongCangChiGuPseudo &Data = m_aZhongCangChiGu[i];
			//float fRate = Data.fBenQiChiGu / fTotalChiGu;
			float fRate = Data.fPercent;
			fRatePre10 += fRate;
			CString StrMerchName = Data.StrMerchName;
			StrTmp.Format(_T("%s %0.2f%%"), StrMerchName.GetBuffer(), fRate*100);

// 			SetDlgItemText(s_StaticIds[i].iTextId, StrTmp);
// 			::ShowWindow(::GetDlgItem(m_hWnd, s_StaticIds[i].iTextId), SW_SHOW);
// 			::ShowWindow(::GetDlgItem(m_hWnd, s_StaticIds[i].iColorFlagId), SW_SHOW);

			// 饼图更新
			m_WndPie.AddPiece(s_StaticIds[i].clr, RGB(255,255,255), fRate, StrTmp);
		}
		StrTmp.Format(_T("其它 %0.2f%%"), (1.0f-fRatePre10)*100);
		SetDlgItemText(s_StaticIds[i].iTextId, StrTmp);
// 		::ShowWindow(::GetDlgItem(m_hWnd, s_StaticIds[i].iTextId), SW_SHOW);
// 		::ShowWindow(::GetDlgItem(m_hWnd, s_StaticIds[i].iColorFlagId), SW_SHOW);

		// 饼图更新
		m_WndPie.AddPiece(s_StaticIds[i].clr, RGB(255,255,255), (1.0f-fRatePre10), StrTmp);
	}
	else
	{
		// 不足的行隐藏
		int32 iZhongCangChiGuSize = (int32)m_aZhongCangChiGu.GetSize();
		const int32 iTextCount = min(s_KStaticIdCount, iZhongCangChiGuSize);
		for ( i=0; i < iTextCount ; ++i )
		{
			const T_ZhongCangChiGuPseudo &Data = m_aZhongCangChiGu[i];
			//float fRate = Data.fBenQiChiGu / fTotalChiGu;
			float fRate = Data.fPercent;
			CString StrMerchName = Data.StrMerchName;
			StrTmp.Format(_T("%s %0.2f%%"), StrMerchName.GetBuffer(), fRate*100);
			
//			SetDlgItemText(s_StaticIds[i].iTextId, StrTmp);
// 			::ShowWindow(::GetDlgItem(m_hWnd, s_StaticIds[i].iTextId), SW_SHOW);
// 			::ShowWindow(::GetDlgItem(m_hWnd, s_StaticIds[i].iColorFlagId), SW_SHOW);

			// 饼图更新
			m_WndPie.AddPiece(s_StaticIds[i].clr, RGB(255,255,255), fRate, StrTmp);
		}
		for ( ; i < s_KStaticIdCount ; ++i )
		{
// 			::ShowWindow(::GetDlgItem(m_hWnd, s_StaticIds[i].iTextId), SW_HIDE);
// 			::ShowWindow(::GetDlgItem(m_hWnd, s_StaticIds[i].iColorFlagId), SW_HIDE);
		}
	}

	m_iStage = 1;
	UpdateStageTip();
	Invalidate();
}

void CDlgZhongCangChiGu::OnTimer( UINT nIDEvent )
{
	if ( s_KiTimerIDGetData == nIDEvent )
	{
		if ( BeDataAlready() )
		{
			KillTimer(s_KiTimerIDGetData);
			
			//
			RequestViewData();
		}
	}
}

void CDlgZhongCangChiGu::InitShow()
{
	m_WndList.DeleteAllItems();
	int nColumnCount = m_WndList.GetHeaderCtrl()->GetItemCount();
	
	// Delete all of the columns.
	int i = 0;
	for (i=0; i < nColumnCount; i++)
	{
		m_WndList.DeleteColumn(0);
	}

	m_WndList.SetExtendedStyle(m_WndList.GetExtendedStyle()|LVS_EX_FULLROWSELECT |LVS_EX_GRIDLINES);
	
	m_WndList.InsertColumn(0, _T("序号"), LVCFMT_LEFT, 50);
	m_WndList.InsertColumn(1, _T("基金名称"), LVCFMT_LEFT, 90);
	m_WndList.InsertColumn(2, _T("本期持股(万)"), LVCFMT_LEFT, 100);
	m_WndList.InsertColumn(3, _T("占上市公司%"), LVCFMT_LEFT, 90);
	m_WndList.InsertColumn(4, _T("占基金净值%"), LVCFMT_LEFT, 90);
	m_WndList.InsertColumn(5, _T("上期持股(万)"), LVCFMT_LEFT, 100);

	m_WndList.GetHeaderCtrl()->ModifyStyle(0, HDS_BUTTONS, 0);

	for ( i=0; i < s_KStaticIdCount ; ++i )
	{
		::ShowWindow(::GetDlgItem(m_hWnd, s_StaticIds[i].iTextId), SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, s_StaticIds[i].iColorFlagId), SW_HIDE);
	}

	m_WndPie.Reset();
}

void CDlgZhongCangChiGu::RequestViewData()
{
	m_iStage = 0;
	
	ZhongCangChiGuArray aData;
	FetchDataFromViewData(aData);
	OnDataUpdate(aData);

	UpdateStageTip();
}

void CDlgZhongCangChiGu::UpdateStageTip()
{
	CString Str;
	switch (m_iStage)
	{
	case 0:
		Str = _T("数据更新，请稍候...");
		break;
	default:
		Str = _T("重仓持股基金情况(以列表中列出的持仓总数为基准)");
		break;
	}
	SetDlgItemText(IDC_STATIC_TIP, Str);
	m_WndPie.SetTitle(Str);
}

void CDlgZhongCangChiGu::OnColumnClick( NMHDR *pHdr, LRESULT *pResult )
{
	if ( NULL != pHdr )
	{
		NMLISTVIEW *pList = (NMLISTVIEW *)pHdr;
		CHeaderCtrl &Header = *m_WndList.GetHeaderCtrl();
		HDITEM hdItem = {0}; 
		if ( m_iSortCol == pList->iSubItem )
		{
			m_bSortDesc = !m_bSortDesc;
		}
		else
		{
			if ( -1 != m_iSortCol )
			{
				// 清空上一个标志
				hdItem.mask = HDI_FORMAT;
				if ( Header.GetItem(m_iSortCol, &hdItem) )
				{
					//hdItem.fmt &= !(HDF_SORTDOWN|HDF_SORTUP);
					hdItem.fmt &= ~(0x600);
					Header.SetItem(m_iSortCol, &hdItem);
				}
			}
			m_bSortDesc = TRUE;
		}
		m_iSortCol = pList->iSubItem;

		//m_WndList.SendMessage(LVM_SORTITEMSEX, (WPARAM)this, (LPARAM)MyCompareRow);
		m_WndList.SendMessage((LVM_FIRST + 81), (WPARAM)(LPARAM)this, (LPARAM)(PFNLVCOMPARE)MyCompareRow);

		hdItem.mask = HDI_FORMAT;
		if ( Header.GetItem(m_iSortCol, &hdItem) )
		{
			//hdItem.fmt |= m_bSortDesc?HDF_SORTDOWN:HDF_SORTUP;
			hdItem.fmt &= ~(0x600);
			hdItem.fmt |= m_bSortDesc?0x0200:0x0400;
			Header.SetItem(m_iSortCol, &hdItem);
		}
	}
	if ( NULL != pResult )
	{
		*pResult = 1;
	}
}

int CALLBACK CDlgZhongCangChiGu::MyCompareRow( LPARAM lp1, LPARAM lp2, LPARAM lpThis )
{
	int iRow1 = (int)lp1;
	int iRow2 = (int)lp2;
	CDlgZhongCangChiGu *pThis = (CDlgZhongCangChiGu *)lpThis;
	ASSERT( NULL != pThis );
	ASSERT( pThis->m_iSortCol >= 0 && pThis->m_iSortCol < pThis->m_WndList.GetHeaderCtrl()->GetItemCount() );

	CString Str1;
	Str1 = pThis->m_WndList.GetItemText(iRow1, pThis->m_iSortCol);
	CString Str2;
	Str2 = pThis->m_WndList.GetItemText(iRow2, pThis->m_iSortCol);

	int iCmp;
	if ( 1 != pThis->m_iSortCol )
	{
		// 全部都用数字比较
		float f1, f2;
		f1 = f2 = 0.0f;
		_stscanf(Str1, _T("%f"), &f1);
		_stscanf(Str2, _T("%f"), &f2);
		iCmp = f1>f2? 1 : (f2>f1?-1:0);
	}
	else
	{
		// 名称
		iCmp = CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, Str1, Str1.GetLength(), Str2, Str2.GetLength()) - CSTR_EQUAL;
	}
	if ( pThis->m_bSortDesc )
	{
		return -iCmp;
	}
	return iCmp;
}

void CDlgZhongCangChiGu::ClearSortFlag()
{
	if ( -1 != m_iSortCol )
	{
		CHeaderCtrl *pheader = m_WndList.GetHeaderCtrl();
		if ( NULL != pheader )
		{
			const int32 iCount = pheader->GetItemCount();
			HDITEM hdItem = {0};
			hdItem.mask = HDI_FORMAT;
			for ( int32 i=0; i < iCount ; ++i )
			{
				// 清空上一个标志
				if ( pheader->GetItem(i, &hdItem) )
				{
					//hdItem.fmt &= !(HDF_SORTDOWN|HDF_SORTUP);
					hdItem.fmt &= ~(0x600);
					pheader->SetItem(i, &hdItem);
				}
			}
		}
		m_iSortCol = -1;
	}
}

void CDlgZhongCangChiGu::FetchDataFromViewData( OUT ZhongCangChiGuArray &aData )
{
	CGGTongDoc *pDoc = AfxGetDocument();
	ASSERT( NULL != pDoc );
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return;
	}
	ASSERT( NULL != pAbsCenterManager );
	ASSERT( NULL != m_pMerch );

	aData.RemoveAll();

	const MapFundHold &holds = pAbsCenterManager->GetFundHold();
	MapFundHold::const_iterator it = holds.find(m_pMerch);
	if ( it != holds.end() )
	{
		for ( ListFundHold::const_iterator itItem=it->second.begin()
			; itItem != it->second.end()
			; ++itItem )
		{
			T_ZhongCangChiGuPseudo ChiGu;
			ChiGu.iMarketId = -1;
			ChiGu.StrMerchName	= itItem->m_StrName;
			ChiGu.fBenQiChiGu	= itItem->m_fHold;
			ChiGu.fShangShiGongSi	= itItem->m_fAllPercent;
			ChiGu.fJiJinJingZhi	= itItem->m_fFundPercent;
			ChiGu.fShangQiChiGu	= itItem->m_fPreHold;
			ChiGu.fPercent		= itItem->m_fPerCent;

			aData.InsertAt(0, ChiGu);
		}
	}
}

bool32 CDlgZhongCangChiGu::BeDataAlready()
{
	CGGTongDoc *pDoc = AfxGetDocument();
	ASSERT( NULL != pDoc );
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return false;
	}
	ASSERT( NULL != pAbsCenterManager );
	
	const MapFundHold &holds = pAbsCenterManager->GetFundHold();
	
	m_bDataAlready = !holds.empty();

	static bool bReq = false;
	if (!m_bDataAlready && !bReq)
	{
		// 请求数据
		// 更新一次基金数据	
		unsigned long uiCrc32 = 0;
		CMmiReqFundHold ReqFundHold;
		ReqFundHold.m_uiCRC32 = uiCrc32;
		pAbsCenterManager->RequestViewData(&ReqFundHold);
		bReq = true;
	}

	return m_bDataAlready;
}