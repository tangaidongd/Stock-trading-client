// dlgsetmerch.cpp : implementation file
//
#include "stdafx.h"
#include "dlgsetmerch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetMerch dialog

CDlgSetMerch::CDlgSetMerch(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetMerch::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetMerch)
	m_StrName = _T("");
	m_uiWidth = 0;
	m_uiNum = 0;
	m_iFixCol = 0;
	m_iFixColBk = 0;
	//}}AFX_DATA_INIT
//	ASSERT(NULL != CReportScheme::Instance());
//	CReportScheme::Instance()->GetReportHeadInfoList(ERTFuturesCn,m_aHeadInfoList);
}

void CDlgSetMerch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetMerch)
	DDX_Control(pDX, IDC_SPIN_NUM, m_SpinNum);
	DDX_Control(pDX, IDC_SPIN_WIDTH, m_SpinWidth);
	DDX_Control(pDX, IDC_LIST_ALL, m_CtrlListAll);
	DDX_Control(pDX, IDC_LIST_EXIST, m_CtrlListExist);
	DDX_Text(pDX, IDC_EDIT_NAME, m_StrName);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_uiWidth);
	DDV_MinMaxUInt(pDX, m_uiWidth, 0, 500);
	DDX_Text(pDX, IDC_EDIT_NUM, m_uiNum);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgSetMerch, CDialog)
	//{{AFX_MSG_MAP(CDlgSetMerch)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnButtonDown)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WIDTH, OnDeltaposSpinWidth)
	ON_LBN_SELCHANGE(IDC_LIST_EXIST, OnSelchangeListExist)
	ON_EN_CHANGE(IDC_EDIT_WIDTH, OnChangeEditWidth)
	ON_EN_CHANGE(IDC_EDIT_NUM, OnChangeEditNum)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetMerch message handlers
 
BOOL CDlgSetMerch::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_StrName = m_StrTitle;
	UpdateData(false);
	
	m_SpinWidth.SetBuddy(GetDlgItem(IDC_EDIT_WIDTH));
	m_SpinWidth.SetRange(0,500);
	m_SpinWidth.SetPos(100);
		
	m_SpinNum.SetBuddy(GetDlgItem(IDC_EDIT_NUM));
	m_SpinNum.SetRange(0,GHCOUNTS);
	m_SpinNum.SetPos(3);

	m_CtrlListExist.SetFocus();
	//m_CtrlListExist.SetCurSel(1);

	UpdateData(false);
	return TRUE;  
}

void CDlgSetMerch::GetOwnHeadInfo(CArray<T_HeadInfo,T_HeadInfo>& aHeadInfoList,int32 iFixCol)
{
	int32 iSize =  aHeadInfoList.GetSize();
	ASSERT(iSize>0);
	m_aHeadInfoList.RemoveAll();
	for (int32 i=0;i<iSize;i++)
	{	
		T_HeadInfo HeadInfoToAdd;
		CString StrSummaryName = CReportScheme::Instance()->HeadCnName2SummaryName(aHeadInfoList[i].m_StrHeadNameCn);
		HeadInfoToAdd.m_StrHeadNameCn = StrSummaryName;
		HeadInfoToAdd.m_iHeadWidth	  = aHeadInfoList[i].m_iHeadWidth;
		m_aHeadInfoList.Add(HeadInfoToAdd);
	}

	m_iFixCol = iFixCol;
	m_iFixColBk = m_iFixCol;
	m_aHeadInfoListBk.Copy(m_aHeadInfoList);
}

void CDlgSetMerch::Show(E_ReportType  eReportType)
{
	// 显示这个Page 的表头项目
	// 清空所有内容
 	CStringArray aStrHead,aStrAnotherHead;
 
	int32 iCount = 0;
 	for (iCount=m_CtrlListExist.GetCount()-1;iCount>=0;iCount--)
 	{
 		m_CtrlListExist.DeleteString(iCount);
 	}
	int32 iCount2 = 0;
 	for (iCount2=m_CtrlListAll.GetCount()-1;iCount2>=0;iCount2--)
 	{
 		m_CtrlListAll.DeleteString(iCount2);
	}	

	// 显示自己的数据

 	CStringArray aHeadInfoList,aAnotherHeadInfoList;
 
 	for (int32 i = 0 ;i<m_aHeadInfoList.GetSize(); i++) 
 	{
		CString StrName = m_aHeadInfoList[i].m_StrHeadNameCn;

 		aHeadInfoList.Add(m_aHeadInfoList[i].m_StrHeadNameCn);
 	}
 	
  	CReportScheme::Instance()->GetHead(aHeadInfoList,aAnotherHeadInfoList,eReportType);
 
 	for (iCount = 0;iCount < aHeadInfoList.GetSize() ; iCount++ )
 	{
		CString StrName = aHeadInfoList[iCount];
		m_CtrlListExist.InsertString(iCount,aHeadInfoList[iCount]);
 	}

 	for (iCount2 = 0;iCount2 < aAnotherHeadInfoList.GetSize(); iCount2++)
 	{
		CString StrName = aAnotherHeadInfoList[iCount2];
 		m_CtrlListAll.InsertString(iCount2,aAnotherHeadInfoList[iCount2]);
	}

	// 宽度
	m_uiWidth = m_aHeadInfoList[0].m_iHeadWidth;

	// 固定项个数
	m_uiNum = m_iFixCol;

	UpdateData(false);
}

bool32 CDlgSetMerch::BeModify()
{
	int32 iCurSize = m_aHeadInfoList.GetSize();
	int32 iSizeBk  = m_aHeadInfoListBk.GetSize();

	if (iCurSize != iSizeBk)
	return true;

	if (m_iFixCol != m_iFixColBk)
	return true;

	for (int32 i = 0 ; i< iCurSize; i++)
	{
		if (m_aHeadInfoList[i].m_iHeadWidth != m_aHeadInfoListBk[i].m_iHeadWidth || m_aHeadInfoList[i].m_StrHeadNameCn != m_aHeadInfoListBk[i].m_StrHeadNameCn)
			return true;
	}	
	return false;
}

//////////////////////////////////////////////////////////////////////////
void CDlgSetMerch::OnButtonAdd() 
{
	int32 iSelCounts = m_CtrlListAll.GetSelCount();	
	if ( iSelCounts < 1 )
	{
		return;
	}
	INT32 *plselectedItem = new INT32[iSelCounts];
	CString StrHead;
	m_CtrlListAll.GetSelItems(iSelCounts,plselectedItem);
	
	for (int32 j=0;j<iSelCounts;j++)
	{
		m_CtrlListAll.GetText(plselectedItem[j],StrHead);
		m_CtrlListExist.InsertString(m_CtrlListExist.GetCount(),StrHead);	
		
		T_HeadInfo HeadInfo;
		HeadInfo.m_StrHeadNameCn = StrHead;
		//HeadInfo.m_iHeadWidth	 = DEFUALTWIDTH;

		CReportScheme::E_ReportHeader eHeader = CReportScheme::GetReportHeaderEType(StrHead);
		HeadInfo.m_eReportHeader = eHeader;
		HeadInfo.m_iHeadWidth	 = CReportScheme::GetReportColDefaultWidth(eHeader);


		m_aHeadInfoList.Add(HeadInfo);
	}
	for (int32 i=iSelCounts-1;i>=0;i--)
	{		
		m_CtrlListAll.GetText(plselectedItem[i],StrHead);
		m_CtrlListAll.DeleteString(plselectedItem[i]);
	}	

	delete [](plselectedItem);
}

void CDlgSetMerch::OnButtonDel() 
{
	int32 iSelCounts = m_CtrlListExist.GetSelCount();
	if ( iSelCounts < 1 )
	{
		return;
	}

	INT32 *plselectedItem = new INT32[iSelCounts];
	CString StrHead;
	m_CtrlListExist.GetSelItems(iSelCounts,plselectedItem);
	if (iSelCounts == m_CtrlListExist.GetCount())
	{
		// 不允许全部删除,保留第一项
		for (int32 j=1;j<iSelCounts;j++)
		{
			m_CtrlListExist.GetText(plselectedItem[j],StrHead);
			m_CtrlListAll.InsertString(m_CtrlListAll.GetCount(),StrHead);	
		}
		//lint --e{661}
		for (int32 i=iSelCounts-1;i>=1;i--)
		{	
			m_CtrlListExist.GetText(plselectedItem[i],StrHead);
			m_CtrlListExist.DeleteString(plselectedItem[i]);			
			m_aHeadInfoList.RemoveAt(plselectedItem[i]);
		}			
	}	
	else
	{
		// 得到选中,左边删除,右边添加. 
		for (int32 j=0;j<iSelCounts;j++)
		{
			m_CtrlListExist.GetText(plselectedItem[j],StrHead);
			m_CtrlListAll.InsertString(m_CtrlListAll.GetCount(),StrHead);	
		}
		for (int32 i=iSelCounts-1;i>=0;i--)
		{		
			m_CtrlListExist.GetText(plselectedItem[i],StrHead);
			m_CtrlListExist.DeleteString(plselectedItem[i]);			
			m_aHeadInfoList.RemoveAt(plselectedItem[i]);
		}	
	}
	
	delete []plselectedItem;
}

void CDlgSetMerch::OnButtonUp() 
{
	int32 iCurSel = m_CtrlListExist.GetCurSel();
	int32 iSelCount = m_CtrlListExist.GetSelCount();
	CString StrText;
	if (1 == iSelCount)
	{
		if (iCurSel!=0)
		{		
			m_CtrlListExist.GetText(iCurSel,StrText);
			m_CtrlListExist.DeleteString(iCurSel);
			m_CtrlListExist.InsertString(iCurSel-1,StrText);
			m_CtrlListExist.SetSel(iCurSel-1,true);

			T_HeadInfo HeadInfo =  m_aHeadInfoList.GetAt(iCurSel);
			m_aHeadInfoList.RemoveAt(iCurSel);
			m_aHeadInfoList.InsertAt((iCurSel-1),HeadInfo);
		}
	}	
}

void CDlgSetMerch::OnButtonDown() 
{
	int32 iCurSel = m_CtrlListExist.GetCurSel();
	int32 iSelCount = m_CtrlListExist.GetSelCount();
	CString StrText;
	if (1 == iSelCount)
	{
		if (iCurSel!=(m_CtrlListExist.GetCount()-1))
		{		
			m_CtrlListExist.GetText(iCurSel,StrText);
			m_CtrlListExist.DeleteString(iCurSel);
			m_CtrlListExist.InsertString(iCurSel+1,StrText);
			m_CtrlListExist.SetSel(iCurSel+1,true);
			
			T_HeadInfo HeadInfo =  m_aHeadInfoList.GetAt(iCurSel);
			m_aHeadInfoList.RemoveAt(iCurSel);
			m_aHeadInfoList.InsertAt((iCurSel+1),HeadInfo);
			
		}
	}
}

BOOL CDlgSetMerch::PreTranslateMessage(MSG* pMsg) 
{
	if ( WM_KEYDOWN == pMsg->message )
	{
		if (VK_ESCAPE == pMsg->wParam )
		{
			return false;
		}
		else if (VK_RETURN == pMsg->wParam)
		{
			return true;
		}
	}		
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgSetMerch::OnDeltaposSpinWidth(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	*pResult = 0;
}

void CDlgSetMerch::OnSelchangeListExist() 
{
	ASSERT(m_aHeadInfoList.GetSize()>0);
	int32 iCurSel = m_CtrlListExist.GetCurSel();
	
	m_uiWidth = m_aHeadInfoList[iCurSel].m_iHeadWidth;

	CEdit * EditWidth = (CEdit *)GetDlgItem(IDC_EDIT_WIDTH);
	CString StrShow;
	StrShow.Format(L"%d",m_uiWidth);
	EditWidth->SetWindowText(StrShow);
}

void CDlgSetMerch::OnChangeEditWidth() 
{
	if (m_CtrlListExist.GetSafeHwnd())
	{
		UpdateData(true);
		int32 iCurSel = m_CtrlListExist.GetCurSel();
		ASSERT(m_uiWidth<500);
		if (m_aHeadInfoList.GetSize()>0)
		{
			m_aHeadInfoList[iCurSel].m_iHeadWidth = m_uiWidth; 
		}		
	}	
}

void CDlgSetMerch::OnChangeEditNum()
{
	if (m_CtrlListExist.GetSafeHwnd())
	{		
		UpdateData(true);
		m_iFixCol = m_uiNum;
	}
}

HBRUSH CDlgSetMerch::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if (nCtlColor == CTLCOLOR_STATIC)
	{	
		pDC->SetTextColor(RGB(0,0,255));
	}

	return hbr;
}
