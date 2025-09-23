// DlgUserBlockAddMerch.cpp : implementation file
//

#include "stdafx.h"
#include "GGTong.h"
#include "GridCellSys.h"
#include "GridCellSymbol.h"
#include "DlgStrategyInfomation.h"


#define  LIST_DATA_COLORR RGB(0,0,0)
// CDlgStrategyInfomation dialog

//IMPLEMENT_DYNAMIC(CDlgStrategyInfomation, CDialog)

CDlgStrategyInfomation::CDlgStrategyInfomation(CWnd* pParent /*=NULL*/)
:  CDialogEx(CDlgStrategyInfomation::IDD, pParent)
{
	m_pmulmapStrategyInfo = NULL;
	m_llStrategyTime = 0;
}

CDlgStrategyInfomation::CDlgStrategyInfomation(long long llStrategyTime, multimap<long long, T_StrategyInfo> *mulmapStrategyInfo, CWnd* pParent /*=NULL*/ )
:  CDialogEx(CDlgStrategyInfomation::IDD, pParent)
{
	m_llStrategyTime = llStrategyTime;
	m_pmulmapStrategyInfo = NULL;
	if (mulmapStrategyInfo)
	{
		m_pmulmapStrategyInfo = mulmapStrategyInfo;
	}
}

CDlgStrategyInfomation::~CDlgStrategyInfomation()
{
}

void CDlgStrategyInfomation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgStrategyInfomation, CDialogEx)
	ON_WM_MOVE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()




bool32 CDlgStrategyInfomation::ConstructGrid()
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
		if (!m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 54321))
			return false;
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(255,0,0));
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

		LOGFONT* pLogFt = CFaceScheme::Instance()->GetSysFont(ESFSmall);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pLogFt);
	}

	// 清空表格内容
	m_GridCtrl.DeleteAllItems();

	// 设置列数
	if (!m_GridCtrl.SetColumnCount(4))
		return false;

	//  设置行数
	if (!m_GridCtrl.SetFixedRowCount(1))	
		return false;

	// 设置相互之间的关联			
	m_XSBHorz.SetOwner(&m_GridCtrl);
	m_XSBVert.SetOwner(&m_GridCtrl);
	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);						

	CFont* pFont = CFaceScheme::Instance()->GetSysFontObject(ESFSmall);
	m_GridCtrl.SetFont(pFont);

	CGridCellSys *	pCell =(CGridCellSys *)m_GridCtrl.GetCell(0, 0);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	pCell->SetText(L"策略名称");

	pCell =(CGridCellSys *)m_GridCtrl.GetCell(0, 1);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	pCell->SetText(L"时间");

	pCell =(CGridCellSys *)m_GridCtrl.GetCell(0, 2);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	pCell->SetText(L"交易方向");

	pCell =(CGridCellSys *)m_GridCtrl.GetCell(0, 3);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	pCell->SetText(L"建议价格");

	if (m_pmulmapStrategyInfo)
	{
		int iDataRow = 0;
		int iStategySize = m_pmulmapStrategyInfo->count(m_llStrategyTime);
		// 设置列数
		if (!m_GridCtrl.SetRowCount(iStategySize+ m_GridCtrl.GetFixedRowCount()))
			return false;
		for ( multimap<long long, T_StrategyInfo>::iterator it = m_pmulmapStrategyInfo->begin(); it != m_pmulmapStrategyInfo->end() ; ++it )
		{
			if ( m_llStrategyTime == it->first )
			{
				T_StrategyInfo stStrategy = it->second;
				CString strDis;
				iDataRow++;
				if (iStategySize >= iDataRow  && m_GridCtrl.GetRowCount() > iDataRow)
				{
					CFont* pFont = CFaceScheme::Instance()->GetSysFontObject(ESFSmall);
					m_GridCtrl.SetFont(pFont);
					CGridCellSys *	pCell =(CGridCellSys *)m_GridCtrl.GetCell(iDataRow, 0);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
					pCell->SetText(stStrategy.strStrategyName);

					pCell =(CGridCellSys *)m_GridCtrl.GetCell(iDataRow, 1);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
					CTime time(stStrategy.llStrategyTime);
					strDis.Format(_T("%02d-%02d %02d:%02d\n"), time.GetMonth(), time.GetDay(),time.GetHour(),time.GetSecond());
					pCell->SetText(strDis);

					pCell =(CGridCellSys *)m_GridCtrl.GetCell(iDataRow, 2);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
					CString strSignalType;
					strDis = ((stStrategy.ulSignalType==0)?_T("卖出"):_T("买入"));
					pCell->SetText(strDis);

					pCell =(CGridCellSys *)m_GridCtrl.GetCell(iDataRow, 3);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
					strDis.Format(_T("%.2f\n"), stStrategy.SugPrice);
					pCell->SetText(strDis);
				}
			}
		}
	}

	m_GridCtrl.MoveWindow(&rect);
	m_GridCtrl.ExpandColumnsToFit();
	m_GridCtrl.AutoSizeRows();
	m_GridCtrl.Invalidate();

	return true;
}


// CDlgStrategyInfomation message handlers

int CDlgStrategyInfomation::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


BOOL CDlgStrategyInfomation::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,FALSE);
	//	CreateToolTips ( m_Tree.GetSafeHwnd(),"双击进行编辑.",NULL);
	ConstructGrid();


	return FALSE;
}




void CDlgStrategyInfomation::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnClose();
}
