// CDlgImportUserBlockBySoft.cpp : implementation file
//

#include "stdafx.h"
#include "GGTong.h"
#include "DlgImportUserBlockBySoft.h"



#define  DLG_USERBLOCK_DETAILS_ID   543211

#define  IMP_BTN_COLOR	RGB(0,0,255)		// ��ɫ
#define BACKGROUND_COLOR RGB(238,240,248)
#define COLOR_GREEN  RGB(39,142,82)
#define COLOR_RED    RGB(255,0,0)
#define COLOR_BLACK	 RGB(0,0,0)

// CDlgImportUserBlockBySoft dialog

IMPLEMENT_DYNAMIC(CDlgImportUserBlockBySoft, CDialog)

CDlgImportUserBlockBySoft::CDlgImportUserBlockBySoft(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportUserBlockBySoft::IDD, pParent)
{
	m_pDlgImpDetails = NULL;
	m_hHandle = NULL;
	m_bRunThreadStatus = false;
}

CDlgImportUserBlockBySoft::~CDlgImportUserBlockBySoft()
{
	if (m_pDlgImpDetails) 
	{
		delete m_pDlgImpDetails;
		m_pDlgImpDetails = NULL;
	}
}

void CDlgImportUserBlockBySoft::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_SoftList);
	DDX_Control(pDX, IDC_STATIC_IMP_TIPS, m_statcImpTips);
	DDX_Control(pDX, IDC_PROGRESS3, m_CustomPregress);
	//DDX_Control(pDX, IDC_BUTTON_EXCEL_IMP, CButton);
	DDX_Control(pDX, IDC_BUTTON_EXCEL_IMP, m_btnExcelImp);
}


BEGIN_MESSAGE_MAP(CDlgImportUserBlockBySoft, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_IMP, &CDlgImportUserBlockBySoft::OnBnClickedButtonImp)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CDlgImportUserBlockBySoft::OnNMClickList1)
	ON_NOTIFY(NM_HOVER, IDC_LIST1, &CDlgImportUserBlockBySoft::OnNMHoverList1)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CDlgImportUserBlockBySoft::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_EXCEL_IMP, &CDlgImportUserBlockBySoft::OnBnClickedButtonExcelImp)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDlgImportUserBlockBySoft message handlers

void CDlgImportUserBlockBySoft::OnBnClickedButtonImp()
{
	// TODO: Add your control notification handler code here
	int iImpNum = 0;
	bool bIsSelectImp = false;
	for (int i = 0; i < m_SoftList.GetItemCount(); ++i)  
	{
		CUserBlockData* pUserBlockData = NULL;
		if(m_SoftList.GetCheck(i))
		{
			bIsSelectImp = true;

			pUserBlockData = (CUserBlockData*)m_SoftList.GetItemData(i);   
			if (pUserBlockData)
			{
				CString strBlockName = CUserBlockManager::Instance()->GetDefaultServerBlockName();
				// �����ѡ��
				for (int i = 0; i < pUserBlockData->m_vMerchInfo.size(); ++i)
				{
					CMerch* pMerch   = NULL;
					if (FindTradeMerchByCodeAndName(pUserBlockData->m_vMerchInfo.at(i).strMerchCode, pUserBlockData->m_vMerchInfo.at(i).strMerchChName, pMerch))
					{
						CGGTongDoc *pDoc = AfxGetDocument();
						CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
						CUserBlockManager::Instance()->AddMerchToUserBlock(pMerch, strBlockName, false);
						iImpNum++;
					}

					// �����Ż��������һ����Ʒ��λ��ˢ����ѡ���ļ�����֪ͨ����
					if (pUserBlockData->m_vMerchInfo.size() - 1 == i)
					{
						CUserBlockManager::Instance()->SaveXmlFile();
						CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);
					}

				}
			}
		}
	}

	if (!bIsSelectImp)
	{
		MessageBox(_T("δѡ��������"), NULL, MB_OK);
		return;
	}
	else 
	{
		CString strImpNum;
		strImpNum.Format(_T("������ѡ����Ŀ��%d"), iImpNum);
		MessageBox(strImpNum, NULL, MB_OK);
	}
	
}



DWORD  WINAPI SearchUserBlockFile(LPVOID lpParameter)
{
	if (!lpParameter)
	{
		return -1;
	}

	CDlgImportUserBlockBySoft *pDlgImportUserBlockBySoft=(CDlgImportUserBlockBySoft*)lpParameter;
	try
	{ 
		pDlgImportUserBlockBySoft->SetRunThreadStatus(true);
		pDlgImportUserBlockBySoft->ShowAndHideControls(TRUE);
		pDlgImportUserBlockBySoft->m_CustomPregress.SetPos(15);
		pDlgImportUserBlockBySoft->m_PubImportUserBlockFun.AnalysisTdxUserBlockFile();
		pDlgImportUserBlockBySoft->m_CustomPregress.SetPos(45);
		pDlgImportUserBlockBySoft->m_PubImportUserBlockFun.AnalysisDzhUserBlockFile();
		pDlgImportUserBlockBySoft->m_CustomPregress.SetPos(75);
		//pDlgImportUserBlockBySoft->m_PubImportUserBlockFun.AnalysisThsUserBlockFile();
		Sleep(100);
		pDlgImportUserBlockBySoft->m_CustomPregress.SetPos(100);	
		pDlgImportUserBlockBySoft->InitUserData();
		Sleep(200);
		pDlgImportUserBlockBySoft->ShowAndHideControls(false);
		pDlgImportUserBlockBySoft->SetRunThreadStatus(false);
	}
	catch(...)
	{
		// .....
	}
	return TRUE;
}




void CDlgImportUserBlockBySoft::SetRunThreadStatus(bool bStatus)
{
	m_bRunThreadStatus = bStatus;
}


void CDlgImportUserBlockBySoft::ShowAndHideControls(bool bShow)
{
	m_CustomPregress.ShowWindow(bShow);
	m_statcImpTips.ShowWindow(bShow);
}
void CDlgImportUserBlockBySoft::InitUserData()
{
	m_SoftList.DeleteAllItems();
	m_PubImportUserBlockFun.m_vUserBlockData;
	CUserBlockData* pUserBlockData = NULL;
	for (int i = 0; i < m_PubImportUserBlockFun.m_vUserBlockData.size(); ++i)
	{
		pUserBlockData = &(m_PubImportUserBlockFun.m_vUserBlockData.at(i));

		int iRow = 0;
		int iCol = 1;
		CString strDataSize;
		iRow = m_SoftList.InsertItem(m_SoftList.GetItemCount() + 1, pUserBlockData->m_strSoftName);
		m_SoftList.SetItemText(i, iCol++, pUserBlockData->m_strUser);
		strDataSize.Format(_T("%d"), pUserBlockData->m_vMerchInfo.size());
		m_SoftList.SetItemText(i, iCol++, strDataSize);
		m_SoftList.SetItemText(i, iCol++, _T("�鿴"));

		m_SoftList.SetItemData(i, (DWORD)pUserBlockData);
	}

}


BOOL CDlgImportUserBlockBySoft::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD dwStyle = m_SoftList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
	dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl��
	dwStyle |= LVS_EX_CHECKBOXES;//itemǰ����checkbox�ؼ�
	m_SoftList.SetExtendedStyle(dwStyle); //������չ���

	m_SoftList.InsertColumn(0,_T("���"));//�����
	m_SoftList.InsertColumn(1,_T("�û���"));
	m_SoftList.InsertColumn(2,_T("������"));
	m_SoftList.InsertColumn(3,_T("����"));
	m_SoftList.SetColumnWidth(0, 150);//�����п�
	m_SoftList.SetColumnWidth(1, 100);
	m_SoftList.SetColumnWidth(2, 100);
	m_SoftList.SetColumnWidth(3, 100);
	//m_SoftList.SetRedraw(FALSE);//��ֹ�ػ�

	m_pDlgImpDetails = new CDlgImpUserBlockDetails(NULL);
	m_pDlgImpDetails->Create(IDD_DIALOG_IMP_USERBLOCK_DISPLAY);
 
	m_CustomPregress.ShowWindow(SW_HIDE);

	m_btnExcelImp.SetTextColor(IMP_BTN_COLOR);
	
	m_statcImpTips.SetTitleText1(_T(" ����ɨ���Ѱ�װ��Ʊ�������ѡ�ɣ����Ժ�..."));
	m_statcImpTips.SetTitleColor1(COLOR_BLACK);
	m_statcImpTips.SetBkColor(BACKGROUND_COLOR);

	//m_pDlgImpDetails->SetmModeless(TRUE);

	//m_btnExcelImp


	// TODO:  Add extra initialization here

	HANDLE hHandle=CreateThread(NULL, 0,SearchUserBlockFile,this,0, NULL);
	CloseHandle(hHandle);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgImportUserBlockBySoft::WindowsMoveCustom()
{
	if (m_pDlgImpDetails)
	{
		CRect rectKey;
		GetWindowRect(&rectKey);

		CRect rtDetails;
		m_pDlgImpDetails->GetWindowRect(rtDetails);
		int iWidth = rtDetails.Width();
		int iHeight = rtDetails.Height();

		rtDetails.top = rectKey.top;
		rtDetails.left = rectKey.right+ 3;
		rtDetails.right = rtDetails.left + iWidth;
		rtDetails.bottom = rtDetails.top + iHeight;

		m_pDlgImpDetails->MoveWindow(rtDetails);
	}
}
void CDlgImportUserBlockBySoft::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	int nRet = 0;

	//LVHITTESTINFO HitTestInfo;

	LPNMITEMACTIVATE lpnmactive = (LPNMITEMACTIVATE)pNMHDR;
	if (lpnmactive->iItem != -1 && lpnmactive->iSubItem == 3)
	{
		CUserBlockData* pUserBlockData = NULL;
		pUserBlockData = (CUserBlockData*)m_SoftList.GetItemData(lpnmactive->iItem); 
		
 		CRect rectKey;
		GetWindowRect(&rectKey);
		int32 iHeight = rectKey.Height();
		int32 iWidth  = rectKey.Width();

		if (m_pDlgImpDetails)
		{
			m_pDlgImpDetails->SetDefaultBlockData(pUserBlockData);
			WindowsMoveCustom();
			m_pDlgImpDetails->ShowWindow(SW_SHOW);
		}			
	}

	*pResult = 0;
}

void CDlgImportUserBlockBySoft::OnNMHoverList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here

	*pResult = 0;
}

void CDlgImportUserBlockBySoft::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	WindowsMoveCustom();
	if (m_pDlgImpDetails)
	{
		WindowsMoveCustom();
	}	

	// TODO: Add your message handler code here
}

void CDlgImportUserBlockBySoft::OnBnClickedButtonCancel()
{
	// TODO: Add your control notification handler code here
	PostMessage(WM_CLOSE,0,0);
}

void CDlgImportUserBlockBySoft::OnBnClickedButtonExcelImp()
{

	ImportExcelToGrid();
	// TODO: Add your control notification handler code here
}

void CDlgImportUserBlockBySoft::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if (m_bRunThreadStatus)
	{
		AfxMessageBox(_T("�ļ��������������Ժ��˳���"),MB_OK|MB_ICONQUESTION);
		return;
	}

	CDialog::OnClose();
}
