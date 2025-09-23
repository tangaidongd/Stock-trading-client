// dlgdownload.cpp : implementation file
//

#include "stdafx.h"
#include "dlgdownload.h"



#include "MerchManager.h"

#include "DlgChooseStockVar.h"

#include "DlgChooseStockVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDownLoad dialog
// ��ʾ��ʱ�Ķ�ʱ��

static const int32 KTimerIdCheckNet				= 1;		
static const int32 KTimerPeriodCheckNet			= 1000;

static const int32 KiRecvDataTimeOut			= 1000 * 60 * 1;		// 1 ���ӻ�û�յ���һ������,��Ϊ��ʱ


CDlgDownLoad::CDlgDownLoad(bool32 bAutoClose, CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgDownLoad::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDownLoad)
	m_bAllMerch = TRUE;
	m_bDownLoadRtPrice = TRUE;
	//}}AFX_DATA_INIT

	//
	m_bAutoClose	= bAutoClose;
	m_bRtPriceFirst = true;

	//
	m_dwTimeLastRecv= 0;

	//
	m_aMerchsDownload.clear();

	//
	m_pAbsCenterManager = NULL;
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if ( NULL != pDoc )
	{
		m_pAbsCenterManager = pDoc->m_pAbsCenterManager;
		m_pAbsCenterManager->SetDownLoadStatusByOffLineData(false);
	}
	//
	m_pAbsCenterManager->SetNotifyByOffLineData(this);
}

CDlgDownLoad::~CDlgDownLoad()
{
	/*if ( NULL != m_pOffLineData )
	{
		m_pOffLineData->StopDownLoad();
		DEL(m_pOffLineData);
	}*/
}

void CDlgDownLoad::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDownLoad)	
	DDX_Control(pDX, IDC_PROGRESS_PROMPT, m_Progress);
	DDX_Control(pDX, IDC_LIST_MERCH_SELECT, m_ListSelect);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_TimeEnd);
	DDX_Control(pDX, IDC_DATETIMEPICKER_BEGIN, m_TimeBegin);
	DDX_Control(pDX, IDC_LIST_MERCH, m_ListMerch);		
	DDX_Check(pDX, IDC_CHECK_ALL_MERCH, m_bAllMerch);
	DDX_Check(pDX, IDC_CHECK_REALTIME, m_bDownLoadRtPrice);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDownLoad, CDialogEx)
	//{{AFX_MSG_MAP(CDlgDownLoad)
	ON_BN_CLICKED(IDC_RADIO_DAY, OnRadioDay)
	ON_BN_CLICKED(IDC_RADIO_MONTH, OnRadioMonth)
	ON_BN_CLICKED(IDC_RADIO_5MIN, OnRadio5min)
	ON_BN_CLICKED(IDC_RADIO_1MIN, OnRadio1min)
	ON_BN_CLICKED(IDC_RADIO_F10, OnRadioF10)
	ON_BN_CLICKED(IDC_CHECK_REALTIME, OnCheckrealtime)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_BEIGN, OnButtonBegin)
	ON_BN_CLICKED(IDC_CHECK_ALL_MERCH, OnCheckAllMerch)
	ON_MESSAGE(UM_Download_Offlinedata_Progress, OnMsgProgess)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDownLoad message handlers

BOOL CDlgDownLoad::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitialCtrls();

	return TRUE;
}

void CDlgDownLoad::ResetData()
{
	m_pAbsCenterManager->SetDownLoadStatusByOffLineData(false);
	m_bRtPriceFirst = true;
	
	//
	m_dwTimeLastRecv= 0;
	
	//
	m_aMerchsDownload.clear();
	
	//
	if ( NULL == m_pAbsCenterManager )
	{
		m_pAbsCenterManager = NULL;
		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
		if ( NULL != pDoc )
		{
			m_pAbsCenterManager = pDoc->m_pAbsCenterManager;
		}
	}
	else
	{
		m_pAbsCenterManager->ResetDataByOffLineData();
		m_pAbsCenterManager->SetNotifyByOffLineData(this);
	}	
	
	//
	KillTimer(KTimerIdCheckNet);

	//
	ResetProgress();

	//
	GetDlgItem(IDC_BUTTON_BEIGN)->SetWindowText(L"��ʼ����");

	//
	SetCtrlsEnable(TRUE);
}

void CDlgDownLoad::OnRadioDay() 
{	
	GetDlgItem(IDC_DATETIMEPICKER_BEGIN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_DATETIMEPICKER_END)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_CHECK_REALTIME)->ShowWindow(SW_SHOW);

	if ( NULL != m_pAbsCenterManager )
	{
		CGmtTime TimeLast = m_pAbsCenterManager->GetLastTimeByOffLineData(EKTBDay);
		if (TimeLast == 0)
		{
			TimeLast = CGmtTime(2010,12,31,0,0,0);
		}

		CTime Time(TimeLast.GetTime());
		m_TimeBegin.SetTime(&Time);

		if ( 0 == TimeLast.GetTime() )
		{
			GetDlgItem(IDC_STATIC_PROMPT)->ShowWindow(SW_SHOW);
		}
		else
		{
			GetDlgItem(IDC_STATIC_PROMPT)->ShowWindow(SW_HIDE);
		}
	}
}


void CDlgDownLoad::OnRadioMonth()
{
	GetDlgItem(IDC_DATETIMEPICKER_BEGIN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_DATETIMEPICKER_END)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_CHECK_REALTIME)->ShowWindow(SW_SHOW);
	
	if ( NULL != m_pAbsCenterManager )
	{
		CGmtTime TimeLast = m_pAbsCenterManager->GetLastTimeByOffLineData(EKTBMonth);
		if (TimeLast == 0)
		{
			TimeLast = CGmtTime(2010,12,31,0,0,0);
		}

		CTime Time(TimeLast.GetTime());
		m_TimeBegin.SetTime(&Time);
		
		if ( 0 == TimeLast.GetTime() )
		{
			GetDlgItem(IDC_STATIC_PROMPT)->ShowWindow(SW_SHOW);
		}
		else
		{
			GetDlgItem(IDC_STATIC_PROMPT)->ShowWindow(SW_HIDE);
		}
	}
}

void CDlgDownLoad::OnRadio5min() 
{	
	GetDlgItem(IDC_DATETIMEPICKER_BEGIN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_DATETIMEPICKER_END)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_CHECK_REALTIME)->ShowWindow(SW_SHOW);

	if ( NULL != m_pAbsCenterManager )
	{
		CGmtTime TimeLast;
		if (m_pAbsCenterManager)
		{
			TimeLast  = m_pAbsCenterManager->GetLastTimeByOffLineData(EKTB5Min);
		}
		
		if (TimeLast == 0)
		{
			TimeLast = CGmtTime(2010,12,31,0,0,0);
		}

		CTime Time(TimeLast.GetTime());
		m_TimeBegin.SetTime(&Time);

		if ( 0 == TimeLast.GetTime() )
		{
			GetDlgItem(IDC_STATIC_PROMPT)->ShowWindow(SW_SHOW);
		}
		else
		{
			GetDlgItem(IDC_STATIC_PROMPT)->ShowWindow(SW_HIDE);
		}
	}
}

void CDlgDownLoad::OnRadio1min() 
{	
	GetDlgItem(IDC_DATETIMEPICKER_BEGIN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_DATETIMEPICKER_END)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_CHECK_REALTIME)->ShowWindow(SW_SHOW);

	if ( NULL != m_pAbsCenterManager )
	{
		CGmtTime TimeLast; 
		if (m_pAbsCenterManager)
		{
			TimeLast  = m_pAbsCenterManager->GetLastTimeByOffLineData(EKTBMinute);
		}

		if (TimeLast == 0)
		{
			TimeLast = CGmtTime(2010,12,31,0,0,0);
		}

		CTime Time(TimeLast.GetTime());
		m_TimeBegin.SetTime(&Time);

		if ( 0 == TimeLast.GetTime() )
		{
			GetDlgItem(IDC_STATIC_PROMPT)->ShowWindow(SW_SHOW);
		}
		else
		{
			GetDlgItem(IDC_STATIC_PROMPT)->ShowWindow(SW_HIDE);
		}
	}
}

void CDlgDownLoad::OnRadioF10() 
{	
	GetDlgItem(IDC_DATETIMEPICKER_BEGIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_DATETIMEPICKER_END)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_CHECK_REALTIME)->ShowWindow(SW_HIDE);
}

void CDlgDownLoad::OnButtonAdd() 
{
	CDlgChooseStockVar::MerchArray aMerchs;

	if ( CDlgChooseStockVar::ChooseStockVar(aMerchs, true, NULL, this) )
	{
		// �õ�ѡ�����Ʒ		
		vector<CMerch*> aMerchsSelected;
		GetMerchsInList(aMerchsSelected);
		for ( int32 i = 0; i < aMerchs.GetSize(); i++ )
		{
			CMerch* pMerch = aMerchs.GetAt(i);
			if ( NULL != pMerch )
			{
				aMerchsSelected.push_back(pMerch);
			}			
		}

		//
		FillList(&m_ListSelect, aMerchsSelected);
		UpdateData(FALSE);
	}

	//
	m_ListSelect.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);   
	m_ListSelect.EnsureVisible(0, TRUE);
	m_ListSelect.SetFocus();
}

void CDlgDownLoad::OnButtonRemove() 
{
	if ( !m_ListMerch.IsWindowVisible() || m_ListSelect.IsWindowVisible() )
	{
		m_ListMerch.ShowWindow(SW_HIDE);
		m_ListSelect.ShowWindow(SW_SHOW);
	}


	int32 iCurSel = 0;

	//ÿ�λ�ȡ��һ��Select,��ɾ��֮.֪��û�б�Select�Ĵ���.
	POSITION pos = m_ListSelect.GetFirstSelectedItemPosition();
	if ( NULL == pos )
	{
		return;
	}
	else
	{
		while (pos)
		{
			int32 iItem = m_ListSelect.GetNextSelectedItem(pos);
			if ( iItem >= 0 )
			{
				iCurSel = iItem;
				m_ListSelect.DeleteItem(iItem);
			}
			pos = m_ListSelect.GetFirstSelectedItemPosition();
		}
	}
	
	// ѡ���б�������һ��item,�������û������,��ôѡ�����һ��.
	
	int32 iCount = m_ListSelect.GetItemCount();
	
	if ( iCurSel > iCount-1)
	{
		iCurSel = iCount - 1;
	}
	
	m_ListSelect.SetItemState(iCurSel, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	m_ListSelect.EnsureVisible(iCurSel, FALSE); 

	//
	m_ListSelect.SetFocus();
}

void CDlgDownLoad::OnButtonBegin() 
{
	if ( !m_pAbsCenterManager->GetDownLoadStatusByOffLineData() )
	{
		//
		GetDownLoadMerchs();
		
		//
		if ( !ValidateParams() )
		{
			return;
		}
		
		if ( NULL == m_pAbsCenterManager )
		{
			//ASSERT(0);
			return;
		}
		
		//
		GetDlgItem(IDC_BUTTON_BEIGN)->SetWindowText(L"ȡ������");
		
		//
		m_Progress.SetRange(0, m_aMerchsDownload.size());
		ResetProgress();

		//
		m_pAbsCenterManager->SetDownLoadStatusByOffLineData(true);
		m_bRtPriceFirst = m_bDownLoadRtPrice;
		
		//
		SetCtrlsEnable(FALSE);

		//
		if ( m_bRtPriceFirst )
		{
			// �������������ݵ�����
			T_DownLoadOffLine stParma;
			
			stParma.m_eReqType = ECTReqRealtimePrice;
			stParma.m_aMerchs  = m_aMerchsDownload;
			
			//
			if (m_pAbsCenterManager)
			{
				m_pAbsCenterManager->DownLoadRealTimePriceDataByOffLineData(stParma);
			}
			//
			KillTimer(KTimerIdCheckNet);
			SetTimer(KTimerIdCheckNet, KTimerPeriodCheckNet, NULL);
		}
		else
		{
			CGmtTime TimeBegin, TimeEnd;
			E_KLineTypeBase eKlineType;
			
			GetParams(TimeBegin, TimeEnd, eKlineType);
			
			//
			T_DownLoadOffLine stParma;
			
			stParma.m_eReqType	= ECTReqMerchKLine;
			stParma.m_aMerchs	= m_aMerchsDownload;
			stParma.m_TimeBeing	= TimeBegin;
			stParma.m_TimeEnd	= TimeEnd;
			stParma.m_eKLineBase= eKlineType;
			

			if (m_pAbsCenterManager)
			{
				m_pAbsCenterManager->DownLoadKLineDataByOffLineData(stParma);
			}

			//
			KillTimer(KTimerIdCheckNet);
			SetTimer(KTimerIdCheckNet, KTimerPeriodCheckNet, NULL);
		}
	}
	else
	{
		if ( NULL == m_pAbsCenterManager )
		{
			return;
		}

		if ( IDNO == MessageBox(L"�Ƿ�ȡ������?", AfxGetApp()->m_pszAppName, MB_YESNO) )
		{
			return;
		}

		//
		ForceStopDownLoad();
	}
}

void CDlgDownLoad::OnCancel() 
{
	if ( m_pAbsCenterManager->GetDownLoadStatusByOffLineData())
	{
		if ( IDNO == MessageBox(L"��������, �Ƿ��˳�?", AfxGetApp()->m_pszAppName, MB_YESNO) )
		{
			return;
		}
		
		//
		ForceStopDownLoad();	
	}
	
	CDialog::OnCancel();
}

void CDlgDownLoad::OnCheckAllMerch() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if ( m_bAllMerch )
	{
		GetDlgItem(IDC_LIST_MERCH)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST_MERCH_SELECT)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(FALSE);
	}
	else
	{
		vector<CMerch*> aMerchsNull;
		FillList(&m_ListSelect, aMerchsNull);

		GetDlgItem(IDC_LIST_MERCH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_MERCH_SELECT)->ShowWindow(SW_SHOW);

		//
		GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(TRUE);	
	}
}

void CDlgDownLoad::OnCheckrealtime()
{
	UpdateData(TRUE);
	m_bDownLoadRtPrice = m_bDownLoadRtPrice;
}

void CDlgDownLoad::InitialCtrls()
{
	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_DAY);
	if ( NULL != pButton )
	{
		pButton->SetCheck(TRUE);
	}

	// ��ȡ������Ʒ	
	if ( !GetAllMerchs() )
	{
		//ASSERT(0);
	}

	// ��ʼ�� List	
	DWORD styles = LVS_EX_FULLROWSELECT;
	
	//lint --e{522}
	ListView_SetExtendedListViewStyleEx(m_ListMerch.m_hWnd, styles, styles);
	m_ListMerch.InsertColumn(0, L"��Ʒ����", LVCFMT_CENTER, 66);
	m_ListMerch.InsertColumn(1, L"��Ʒ����", LVCFMT_CENTER, 66);
	FillList(&m_ListMerch, m_aMerchsAll);
	
	//
	ListView_SetExtendedListViewStyleEx(m_ListSelect.m_hWnd, styles, styles);
	m_ListSelect.InsertColumn(0, L"��Ʒ����", LVCFMT_CENTER, 66);
	m_ListSelect.InsertColumn(1, L"��Ʒ����", LVCFMT_CENTER, 66);
	vector<CMerch*> aMerchsNull;
	FillList(&m_ListSelect, aMerchsNull);
	
	// ��ʼ�����ڿؼ�:
	if ( NULL != m_pAbsCenterManager )
	{
		CGmtTime TimeLast = m_pAbsCenterManager->GetLastTimeByOffLineData(EKTBDay);
		if (TimeLast == 0)
		{
			TimeLast = CGmtTime(2010,12,31,0,0,0);
		}

		CTime Time(TimeLast.GetTime());
		m_TimeBegin.SetTime(&Time);

		if ( 0 == TimeLast.GetTime() )
		{
			GetDlgItem(IDC_STATIC_PROMPT)->ShowWindow(SW_SHOW);
		}
		else
		{
			GetDlgItem(IDC_STATIC_PROMPT)->ShowWindow(SW_HIDE);
		}
	}

	//
	GetDlgItem(IDC_LIST_MERCH)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_LIST_MERCH_SELECT)->ShowWindow(SW_HIDE);

	//
	UpdateData(FALSE);	

	// ����all״̬��������һ��
	OnCheckAllMerch();
}

bool32 CDlgDownLoad::GetAllMerchs()
{
	m_aMerchsAll.clear();

	// ���⴦��, ��Ʊ�汾, Ĭ�ϻ��� A ����Ʒ
	{
		// 
		m_aMerchsAll.clear();
		
		//
		CBlockLikeMarket* p = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
		if ( NULL != p )
		{
			for (int32 i = 0; i < p->m_blockInfo.m_aSubMerchs.GetSize(); i++)
			{
				if ( NULL != p->m_blockInfo.m_aSubMerchs.GetSize() )
				{
					//
					CMerch* pMerch = p->m_blockInfo.m_aSubMerchs.GetAt(i);
					if ( NULL != pMerch )
					{
						m_aMerchsAll.push_back(pMerch);
					}				
				}
			}
		}

		//
		if ( NULL != m_pAbsCenterManager )
		{
			// ��ָ֤��
			CMerch* pMerchSH = NULL;
			if ( m_pAbsCenterManager->GetMerchManager().FindMerch(L"000001", 0, pMerchSH) && NULL != pMerchSH )
			{
				m_aMerchsAll.push_back(pMerchSH);
			}

			// ����ָ��
			CMerch* pMerchSZ = NULL;
			if ( m_pAbsCenterManager->GetMerchManager().FindMerch(L"399001", 1000, pMerchSZ) && NULL != pMerchSZ )
			{
				m_aMerchsAll.push_back(pMerchSZ);
			}
		}		
	}

	/*
	//
	if ( NULL == m_pAbsCenterManager )
	{
		//ASSERT(0);
		return false;
	}

	//
	for ( int32 i = 0; i < m_pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize(); ++i )
	{
		CBreed* pBreed = m_pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetAt(i);
		
		if ( NULL == pBreed )
		{
			continue;
		}

		//
		for ( int32 j = 0; j < pBreed->m_MarketListPtr.GetSize(); ++j )
		{
			CMarket* pMarket = pBreed->m_MarketListPtr.GetAt(j);

			if ( NULL == pMarket )
			{
				continue;
			}

			//
			for ( int32 k = 0; k < pMarket->m_MerchsPtr.GetSize(); ++k )
			{
				CMerch* pMerch = pMarket->m_MerchsPtr.GetAt(k);

				if ( NULL == pMerch )
				{
					continue;
				}

				//
				m_aMerchsAll.push_back(pMerch);
			}
		}
	}
	*/

	return true;
}

void CDlgDownLoad::GetDownLoadMerchs()
{
	UpdateData(TRUE);

	//
	m_aMerchsDownload.clear();

	//
	if ( m_bAllMerch )
	{
		// fangz 0510 ��Ʊ�涨��, ֻҪ����A����Ʒ		
		m_aMerchsDownload = m_aMerchsAll;		
	}
	else
	{
		GetMerchsInList(m_aMerchsDownload);
	}
}

void CDlgDownLoad::FillList(CListCtrl* pList, const vector<CMerch*>& aMerchs)
{	
	if ( NULL == pList )
	{
		return;
	}

	//
	pList->DeleteAllItems();
	
	//
	for ( int32 i = 0; i < (int32)aMerchs.size(); ++i )
	{
		CMerch* pMerch = aMerchs[i];
		
		//
		if ( NULL == pMerch )
		{
			continue;
		}
		
		//
		int32 iIndex = pList->InsertItem(pList->GetItemCount() + 1, pMerch->m_MerchInfo.m_StrMerchCode);
		pList->SetItemText(iIndex, 1, pMerch->m_MerchInfo.m_StrMerchCnName);
		pList->SetItemData(iIndex, (DWORD)pMerch);
	}

	pList->SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);   
	pList->EnsureVisible(0, TRUE);
}

void CDlgDownLoad::ResetProgress()
{
	if ( m_Progress.GetSafeHwnd() )
	{
		m_Progress.SetForeColour(RGB(0, 255, 0));
		m_Progress.SetPos(0);
		m_Progress.SetWindowText(L"");
		
		RedrawWindow();
	}	
}

void CDlgDownLoad::SetCtrlsEnable(bool32 bEnable)
{
	UpdateData(TRUE);

	GetDlgItem(IDC_RADIO_DAY)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_MONTH)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_5MIN)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_1MIN)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_ALL_MERCH)->EnableWindow(bEnable);
	if ( bEnable && ! m_bAllMerch )
	{		
		GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(FALSE);
	}

	GetDlgItem(IDC_CHECK_REALTIME)->EnableWindow(bEnable);

	GetDlgItem(IDC_DATETIMEPICKER_BEGIN)->EnableWindow(bEnable);
	GetDlgItem(IDC_DATETIMEPICKER_END)->EnableWindow(bEnable);
	GetDlgItem(IDC_LIST_MERCH)->EnableWindow(bEnable);
	GetDlgItem(IDC_LIST_MERCH_SELECT)->EnableWindow(bEnable);
}

void CDlgDownLoad::GetParams(OUT CGmtTime& TimeBegin, OUT CGmtTime& TimeEnd, OUT E_KLineTypeBase& eType)
{
	UpdateData(TRUE);

	//
	CTime Time1, Time2;
	m_TimeBegin.GetTime(Time1);
	m_TimeEnd.GetTime(Time2);
	
	CGmtTime t1(Time1.GetTime());
	CGmtTime t2(Time2.GetTime());

	//--- wangyongxue ����15:45 ������ص�������
	CGmtTime tmCurrent = CGmtTime::GetCurrentTime();
	SaveMinute(tmCurrent);
	CGmtTime tmClose(tmCurrent);
	SaveDay(tmClose);

	tmClose += CGmtTimeSpan(0, 7, 45, 0);

	if ( tmCurrent < tmClose && (t2.GetYear() == tmCurrent.GetYear() && 
		t2.GetMonth() == tmCurrent.GetMonth() && t2.GetDay() == tmCurrent.GetDay()))
	{
		t2 -=  CGmtTimeSpan(1, 0, 0, 0);
	}

	TimeBegin = t1;
	TimeEnd	  = t2;

	//
	eType = EKTBDay;
	if ( ((CButton*)GetDlgItem(IDC_RADIO_DAY))->GetCheck() )
	{
		eType = EKTBDay;
	}
	else if ( ((CButton*)GetDlgItem(IDC_RADIO_1MIN))->GetCheck() )
	{
		eType = EKTBMinute;
	}
	else if ( ((CButton*)GetDlgItem(IDC_RADIO_5MIN))->GetCheck() )
	{
		eType = EKTB5Min;
	}
	else if ( ((CButton*)GetDlgItem(IDC_RADIO_MONTH))->GetCheck() )
	{
		eType = EKTBMonth;
	}
}

bool32 CDlgDownLoad::ValidateParams()
{
	UpdateData(TRUE);

	//
	if ( m_aMerchsDownload.size() <= 0 )
	{
		MessageBox(L"��ѡ����Ҫ���ص���Ʒ", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return false;
	}
	
	//
	CTime TimeBegin, TimeEnd;
	m_TimeBegin.GetTime(TimeBegin);
	m_TimeEnd.GetTime(TimeEnd);
	
	CGmtTime t1(TimeBegin.GetTime());
	CGmtTime t2(TimeEnd.GetTime());

	SaveDay(t1);
	SaveDay(t2);

	if ( t1.GetTime() > t2.GetTime() )
	{
		MessageBox(L"��������ȷ��ʱ���", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return false;
	}

	return true;
}

void CDlgDownLoad::ForceStopDownLoad()
{
	//
	m_pAbsCenterManager->SetDownLoadStatusByOffLineData(false);
	
	if ( NULL != m_pAbsCenterManager )
	{
		m_pAbsCenterManager->StopDownLoadByOffLineData();
	}
	
	//
	GetDlgItem(IDC_BUTTON_BEIGN)->SetWindowText(L"��ʼ����");
	ResetData();	
}

LRESULT CDlgDownLoad::OnMsgProgess(WPARAM wParam, LPARAM lParam)
{
	// ������ʾ:
	CString *pStrMsg = (CString *)wParam;
	CString StrMsg	 = *pStrMsg;
	DEL(pStrMsg);

	int32 iCode = (int32)lParam;

	if ( !m_pAbsCenterManager->GetDownLoadStatusByOffLineData())
	{
		return 0;
	}

	if ( -2 == iCode )
	{
		// K ���������� ��ʼ��ʱ��
		// SetTimer(KTimerIdCheckNet, KTimerPeriodCheckNet, NULL);
	}
	if ( -1 == iCode )
	{
		// ʧ����:		
		ResetData();
		m_Progress.SetWindowText(StrMsg);

		//
		SetCtrlsEnable(TRUE);
	}
	else if ( 0 == iCode )
	{
		// ����������		
		m_Progress.StepIt();
		
		CString StrPos = m_Progress.GetPosText();
		CString StrShow;
		StrShow.Format(L"%s [%s]", StrMsg.GetBuffer(), StrPos.GetBuffer());
		m_Progress.SetWindowText(StrShow);	
		
		//
		m_dwTimeLastRecv = timeGetTime();
	}
	else if ( 1 == iCode )
	{
		// �����º���, ��Ҫ������K ��
		m_Progress.SetRange(0, m_aMerchsDownload.size());
		
		//
		ResetProgress();
		m_Progress.SetWindowText(StrMsg);

		// ��Ҫ�������� K ������
		if ( NULL != m_pAbsCenterManager )
		{
			CGmtTime TimeBegin, TimeEnd;
			E_KLineTypeBase eKlineType;
			
			GetParams(TimeBegin, TimeEnd, eKlineType);
			
			//
			T_DownLoadOffLine stParma;
			
			stParma.m_eReqType	= ECTReqMerchKLine;
			stParma.m_aMerchs	= m_aMerchsDownload;
			stParma.m_TimeBeing	= TimeBegin;
			stParma.m_TimeEnd	= TimeEnd;
			stParma.m_eKLineBase= eKlineType;
			
			m_pAbsCenterManager->DownLoadKLineDataByOffLineData(stParma);

			//
			KillTimer(KTimerIdCheckNet);
			SetTimer(KTimerIdCheckNet, KTimerPeriodCheckNet, NULL);
		}
		
		RedrawWindow();

		//
		m_bRtPriceFirst = false;
	}
	else if ( 2 == iCode )
	{
		// ���ݶ��º���
		ResetData();
		m_Progress.SetWindowText(StrMsg);

		//
		if ( !m_bAutoClose )
		{
			SetCtrlsEnable(TRUE);
		}
		else
		{
			PostMessage(WM_CLOSE, 0, 0);
		}		
	}

	return 0;
}

void CDlgDownLoad::OnAllRequestSended(E_CommType eType)
{
/*	
if ( eType == ECTReqRealtimePrice )
	{
		NULL;
	}
	else if ( eType == ECTReqMerchKLine )
	{
		// ��������, ��ʼ��ʱ��
		// CString* pStr = new CString;
		// *pStr = L"K �����������";
		// 
		// PostMessage(UM_Download_Offlinedata_Progress, WPARAM(pStr), LPARAM(-2));		
		NULL;
	}
	else if ( eType == ECTReqPublicFile )
	{
		NULL;
	}
	*/
}

// ��������
void CDlgDownLoad::OnDataDownLoading(CMerch* pMerch, E_CommType eType)
{
	// ���½�������ʾ:
	CString* pStrMsg = new CString;			
	
	if ( NULL != pMerch )
	{
		//
		if ( ECTReqRealtimePrice == eType )
		{
			*pStrMsg = L"������������: ";
		}
		else if ( ECTReqMerchKLine == eType )
		{
			*pStrMsg = L"����K ������: ";
		}

		//
		*pStrMsg += pMerch->m_MerchInfo.m_StrMerchCnName;
	}
	else
	{
		//ASSERT(0);
		*pStrMsg = L"";
	}
	
	//
	PostMessage(UM_Download_Offlinedata_Progress, WPARAM(pStrMsg),  LPARAM(0));	
}

// ���سɹ�
void CDlgDownLoad::OnDataDownLoadFinished(E_CommType eType)
{
	if ( m_bRtPriceFirst )
	{			
		//
		CString* pStrMsg = new CString;
		*pStrMsg = L"�����������سɹ�";

		//
		PostMessage(UM_Download_Offlinedata_Progress, WPARAM(pStrMsg), LPARAM(1));		
	}
	else 
	{
		CString* pStrMsg = new CString;
		*pStrMsg = L"�������";

		//
		PostMessage(UM_Download_Offlinedata_Progress, WPARAM(pStrMsg), LPARAM(2));
	}
}

// ����ʧ��
void CDlgDownLoad::OnDataDownLoadFailed(const CString& StrErrMsg)
{
	CString* pStrMsg = new CString;
	*pStrMsg = StrErrMsg;
	
	//
	PostMessage(UM_Download_Offlinedata_Progress, WPARAM(pStrMsg), LPARAM(-1));
}

//
void CDlgDownLoad::OnTimer(UINT nIDEvent)
{
	if ( KTimerIdCheckNet == nIDEvent )
	{
		if ( m_pAbsCenterManager->GetDownLoadStatusByOffLineData() && 0 != m_dwTimeLastRecv )
		{
			// ����Ƿ�ʱ:
			DWORD dwTimeNow = timeGetTime();
			
			if ( dwTimeNow - m_dwTimeLastRecv >= KiRecvDataTimeOut ) 
			{
				//
				TRACE(L"��ʱ!!!\n");

				//
				ForceStopDownLoad();
				
				//
				MessageBox(L"����״������, ���س�ʱ, ��������", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
			}
		}		
	}
}

void CDlgDownLoad::GetMerchsInList( OUT vector<CMerch *> &aMerchs )
{
	aMerchs.clear();

	for ( int32 i = 0; i < m_ListSelect.GetItemCount(); i++ )
	{
		CMerch* pMerch = (CMerch*)m_ListSelect.GetItemData(i);
		
		if ( NULL == pMerch )
		{
			continue;
		}
		
		//
		aMerchs.push_back(pMerch);
	}

	sort(aMerchs.begin(),aMerchs.end());
	aMerchs.erase(unique(aMerchs.begin(), aMerchs.end()), aMerchs.end());
}

