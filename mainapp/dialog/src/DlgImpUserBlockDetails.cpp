// DlgImpUserBlockDetails.cpp : implementation file
//

#include "stdafx.h"
#include "GGTong.h"
#include "DlgImpUserBlockDetails.h"


// CDlgImpUserBlockDetails dialog

IMPLEMENT_DYNAMIC(CDlgImpUserBlockDetails, CDialog)

CDlgImpUserBlockDetails::CDlgImpUserBlockDetails(CUserBlockData* pUserBlockData, CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgImpUserBlockDetails::IDD, pParent)
{
	m_pUserBlockData = pUserBlockData;
	//m_bModeless = false;
}

CDlgImpUserBlockDetails::~CDlgImpUserBlockDetails()
{
}

void CDlgImpUserBlockDetails::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listDetails);
}


BEGIN_MESSAGE_MAP(CDlgImpUserBlockDetails, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CDlgImpUserBlockDetails::OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgImpUserBlockDetails::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDlgImpUserBlockDetails message handlers



BOOL CDlgImpUserBlockDetails::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD dwStyle = m_listDetails.GetExtendedStyle();
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	dwStyle |= LVS_NOCOLUMNHEADER;
	m_listDetails.SetExtendedStyle(dwStyle); //设置扩展风格

	m_listDetails.InsertColumn(0,_T("商品代码"));//添加列
	m_listDetails.InsertColumn(1,_T("商品名称"));

	m_listDetails.SetColumnWidth(0, 140);//设置列宽
	m_listDetails.SetColumnWidth(1, 140);

	BulidUserBlockData();

	ModifyStyleEx(0,0x00400000L,0); 
	//m_SoftList.SetRedraw(FALSE);//防止重绘

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//
//void CDlgImpUserBlockDetails::SetmModeless(bool bModeless)
//{
//	m_bModeless = bModeless;
//}
//void CDlgImpUserBlockDetails::SetDefaultBlockData(CUserBlockData* pUserBlockData)
//{
//	m_pUserBlockData = pUserBlockData;
//}


void CDlgImpUserBlockDetails::SetDefaultBlockData(CUserBlockData* pUserBlockData)
{
	if (!pUserBlockData)
	{
		return;
	}
	
	m_pUserBlockData = pUserBlockData;
	BulidUserBlockData();
	
}

void  CDlgImpUserBlockDetails::BulidUserBlockData()
{
	if (!m_pUserBlockData)
	{
		return;
	}
	CString strDataDis;
	GetDlgItem(IDC_STATIC_NAME)->SetWindowText(m_pUserBlockData->m_strSoftName);
	strDataDis.Format(_T("个股数：%d"), m_pUserBlockData->m_vMerchInfo.size());
	GetDlgItem(IDC_STATIC_NUM)->SetWindowText(strDataDis);
	
	m_listDetails.DeleteAllItems();
	for (int i = 0; i < m_pUserBlockData->m_vMerchInfo.size(); ++i)
	{
		int iRow = 0;
		int iCol = 1;
		iRow = m_listDetails.InsertItem(m_listDetails.GetItemCount() + 1, m_pUserBlockData->m_vMerchInfo.at(i).strMerchCode);
		m_listDetails.SetItemText(i, iCol++, m_pUserBlockData->m_vMerchInfo.at(i).strMerchChName);
		m_listDetails.SetItemData(i, (DWORD)m_pUserBlockData);
	}
}


//
//void CDlgImpUserBlockDetails::OnCancel() // 用户关闭对话框
//{
//	if(m_bModeless)
//		DestroyWindow();
//	else
//		CDialog::OnCancel(); 
//}
//void CDlgImpUserBlockDetails::PostNcDestroy()
//{
//	if(m_bModeless) //这么做可以使模式对话框不调用PostNcDestroy，因为它的实例会自动删除
//		delete this;
//}
void CDlgImpUserBlockDetails::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDlgImpUserBlockDetails::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
}
