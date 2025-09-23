// IoViewValue.cpp: implementation of the CIoViewValue class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "IoViewValue.h"
#include "GridCellSymbol.h"
#include "GridCellSys.h"

#include "BlockConfig.h"
#include "IoViewManager.h"
#include "FontFactory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//
static const int KiTimerIdFiniance		= 1;
static const int KiTimerPeriodFiniance	= 1000 * 5;

IMPLEMENT_DYNCREATE(CIoViewValue, CIoViewBase) 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIoViewValue::CIoViewValue()
:CIoViewBase()
{
	m_iRowHeight	= -1;
	m_iTitleHeight	= -1;
	m_eMerchKind	= ERTCount;
}

CIoViewValue::~CIoViewValue()
{

}

BEGIN_MESSAGE_MAP(CIoViewValue, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewValue)
ON_WM_PAINT()
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_TIMER()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CIoViewValue::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CIoViewBase::OnCreate(lpCreateStruct);
	
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// �����ɫ������
	InitialIoViewFace(this);

	//
	E_ReportType eMerchKind = GetMerchKind(m_pMerchXml);
	CreateTable(eMerchKind);

	//
	if ( m_CtrlTitle.GetSafeHwnd() )
	{
		m_CtrlTitle.SetDrawActiveParams(m_iActiveXpos, m_iActiveXpos, m_ColorActive);
		m_CtrlTitle.SetTitle(L"��ʾ:�������ݽ����ο�");
	}

	//
	if ( m_CtrlTitle.GetSafeHwnd() )
	{
		m_CtrlTitle.SetDrawActiveParams(m_iActiveXpos, m_iActiveXpos, m_ColorActive);
	}


	//
	return 0;
}

void CIoViewValue::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	if ( !IsWindowVisible() )
	{
		return;
	}
	
	if ( GetParentGGTongViewDragFlag() || m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}
	
	//
	UnLockRedraw();
	
	if ( m_GridCtrl.GetSafeHwnd() )
	{
		m_GridCtrl.RedrawWindow();
	}
	
	if ( m_CtrlTitle.GetSafeHwnd() )
	{		
		m_CtrlTitle.RedrawWindow();			
	}

}

void CIoViewValue::OnSize(UINT nType, int cx, int cy)
{
	CIoViewBase::OnSize(nType,cx,cy);
	SetSize();	
}

void CIoViewValue::SetSize()
{
	CClientDC dc(this);
	
	int32 iRowCount = m_GridCtrl.GetRowCount();
	int32 iColCount = m_GridCtrl.GetColumnCount();
	if ( 0 == iRowCount || 0 == iColCount)
	{
		return;
	}

	//
	CFont * pFontNormal = GetIoViewFontObject(ESFBig);
	CFont *pOldFont = dc.SelectObject(pFontNormal);
	CSize size = dc.GetTextExtent(L"һ����");
	dc.SelectObject(pOldFont);

	//	
	CRect RectTitle;
	RectTitle.top		= m_rectClient.top;
	RectTitle.bottom	= RectTitle.top + 62;
	RectTitle.left		= m_rectClient.left;
	RectTitle.right		= m_rectClient.right;

	m_CtrlTitle.MoveWindow(&RectTitle);
	m_CtrlTitle.SetDrawCornerFlag(false);


	
	CRect RectClient;
	GetClientRect(RectClient);
	
	CRect RectGrid		= RectClient;
	RectGrid.top		= RectTitle.bottom;

	//
	m_GridCtrl.MoveWindow(&RectGrid);
	m_GridCtrl.ExpandColumnsToFit(TRUE);
}

bool32 CIoViewValue::CreateTable(E_ReportType eMerchKind)
{
	if ( eMerchKind == m_eMerchKind )
	{
		return true;
	}

	// ���Դ������
	if (NULL == m_GridCtrl.GetSafeHwnd())
	{
		//
		if (!m_CtrlTitle.Create(_T("��ʾ:�������ݽ����ο�"),SS_LEFT,CRect(0, 0, 0, 0), this))
		{
			return false;
		}

		//
		m_CtrlTitle.ShowWindow(SW_SHOW);

		//
		if (!m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 13456))
		{
			return false;
		}
		
		//
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

		//
		//
		m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
		m_XSBVert.SetScrollRange(0, 10);
		m_XSBVert.SetOwner(&m_GridCtrl);
		
		m_GridCtrl.SetScrollBar(NULL, &m_XSBVert);
	}
	
	// ��ձ������
	m_GridCtrl.DeleteAllItems();
	
	//
	m_GridCtrl.EnableSelection(false);

	// ��������
	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	LOGFONT fontChinese;
	memset(&fontChinese, 0, sizeof(fontChinese));
	_tcscpy(fontChinese.lfFaceName, gFontFactory.GetExistFontName(_T("����")));///
	fontChinese.lfHeight  = -14;
	COLORREF clrChinese = RGB(160,160,160);

	//
	if ( ERTExp == eMerchKind || ERTExpForeign == eMerchKind )
	{
		// ָ��		
		m_GridCtrl.SetColumnCount(2);
		m_GridCtrl.SetRowCount(7);

		//
		CGridCellSys* pCell = (CGridCellSys* )m_GridCtrl.GetCell(0, 0);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"�ܹɱ�");
		
		//
		pCell = (CGridCellSys* )m_GridCtrl.GetCell(1, 0);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"����ͨ��");
		
		//
		pCell = (CGridCellSys* )m_GridCtrl.GetCell(2, 0);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"���¹�����ֵ");
		
		//
		pCell = (CGridCellSys* )m_GridCtrl.GetCell(3, 0);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"��ͨ��ֵ");
		
		//
		pCell = (CGridCellSys* )m_GridCtrl.GetCell(4, 0);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"����ƽ����ӯ��");
		
		//
		pCell = (CGridCellSys* )m_GridCtrl.GetCell(5, 0);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"����ƽ���о���");
		
		//
		pCell = (CGridCellSys* )m_GridCtrl.GetCell(6, 0);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"����ƽ��������");	
	}
	else
	{
		// ��ͨ��Ʒ
		m_GridCtrl.SetColumnCount(4);
		m_GridCtrl.SetRowCount(9);
		
		//
		CGridCellSys* pCell = (CGridCellSys* )m_GridCtrl.GetCell(0, 0);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"��ͣ��");
		
		//
		pCell = (CGridCellSys* )m_GridCtrl.GetCell(0, 2);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"��ͣ��");

		//
		pCell = (CGridCellSys* )m_GridCtrl.GetCell(1, 0);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"������ҵ");
		
		//
		pCell = (CGridCellSys* )m_GridCtrl.GetCell(2, 0);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"��ͨ��ֵ");
		
		//
		pCell = (CGridCellSys* )m_GridCtrl.GetCell(3, 0);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"�ãģ�");

		//
		pCell = (CGridCellSys* )m_GridCtrl.GetCell(4, 0);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"����");
		
		//
		pCell = (CGridCellSys* )m_GridCtrl.GetCell(4, 2);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"�Σ�");
			
		//
		pCell = (CGridCellSys* )m_GridCtrl.GetCell(5, 0);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"�Σ�");
		
		//
		pCell = (CGridCellSys* )m_GridCtrl.GetCell(5, 2);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"����");
		
		//
		pCell = (CGridCellSys* )m_GridCtrl.GetCell(6, 0);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"���ƽ��");
		
		//
		pCell = (CGridCellSys* )m_GridCtrl.GetCell(7, 0);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"��ͷ����");
		
		//
		pCell = (CGridCellSys* )m_GridCtrl.GetCell(8, 0);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
		pCell->SetFont(&fontChinese);
		pCell->SetTextClr(clrChinese);
		pCell->SetText(L"��ͷֹ��");
	}

	for (int i =0; i < m_GridCtrl.GetRowCount(); ++i)
	{
		for (int j =0; j < m_GridCtrl.GetColumnCount(); ++j)
		{				
			CGridCellBase* pCell = m_GridCtrl.GetCell(i, j);
			if (NULL != pCell)
			{
				if (0 == j)
				{
					pCell->SetTextPadding(CRect(15,0,0,0));
				}

				if (m_GridCtrl.GetColumnCount()- 1 == j)
				{
					pCell->SetTextPadding(CRect(0,0,15,0));
				}
			}
		}
	}

	for (int i =0; i < m_GridCtrl.GetRowCount(); ++i)
	{
		m_GridCtrl.SetRowHeight(i, 30);
	}

	//		
	//m_GridCtrl.AutoSizeRows();

	m_eMerchKind = eMerchKind;

	return true;
}

void CIoViewValue::SetDrawLineInfo()
{
	if ( !m_GridCtrl.GetSafeHwnd() )
	{
		return;
	}

	//
	if ( ERTExp == m_eMerchKind || ERTExpForeign == m_eMerchKind )
	{
		for ( int32 i = 0; i < m_GridCtrl.GetColumnCount(); i++ )
		{
			CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(1,i);
			pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);		
		}
	}
	else
	{
		for ( int32 i = 0; i < m_GridCtrl.GetColumnCount(); i++ )
		{
			CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, i);
			pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(2, i);
			pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(5, i);
			pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(8, i);
			pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
		}
	}
}

bool32 CIoViewValue::FromXml(TiXmlElement * pElement)
{
	//
	SetFontsFromXml(pElement);
	SetColorsFromXml(pElement);

	//
	if (NULL == pElement)
		return false;
	
	// �ж��ǲ���IoView�Ľڵ�
	const char *pcValue = pElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;
	
	// �ж��ǲ��������Լ����ҵ����ͼ�Ľڵ�
	const char *pcAttrValue = pElement->Attribute(GetXmlElementAttrIoViewType());
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(CString(pcAttrValue)) != 0)	// ���������Լ���ҵ��ڵ�
		return false;
	
	// ��ȡ��ҵ����ͼ���е�����
	int32 iMarketId			= -1;
	CString StrMerchCode	= L"";
	
	pcAttrValue = pElement->Attribute(GetXmlElementAttrMarketId());
	if (NULL != pcAttrValue)
	{
		iMarketId = atoi(pcAttrValue);
	}
	
	pcAttrValue = pElement->Attribute(GetXmlElementAttrMerchCode());
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
	
	// ��Ʒ�����ı�
	OnVDataMerchChanged(iMarketId, StrMerchCode, pMerchFound);	
	
	return true;
}

CString CIoViewValue::ToXml()
{
	CString StrThis;

	CString StrMarketId;
	StrMarketId.Format(L"%d", m_MerchXml.m_iMarketId);
	
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" ", 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(),
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrShowTabName()).GetBuffer(), m_StrTabShowName.GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(), 
		m_MerchXml.m_StrMerchCode.GetBuffer(),
		CString(GetXmlElementAttrMarketId()).GetBuffer(),
		StrMarketId.GetBuffer());
	
	CString StrFace;

	//
	StrFace  = SaveColorsToXml();
	StrFace += SaveFontsToXml();
	
	//
	StrThis += StrFace;
	//
	StrThis += L">\n";
	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";
	
	return StrThis;
}

void CIoViewValue::SetChildFrameTitle()
{

}

void CIoViewValue::OnIoViewActive()
{
	if ( m_CtrlTitle.GetSafeHwnd() )
	{
		m_CtrlTitle.SetActiveFlag(true);
		m_CtrlTitle.RedrawWindow();
	}
}

void CIoViewValue::OnIoViewDeactive()
{
	if ( m_CtrlTitle.GetSafeHwnd() )
	{
		m_CtrlTitle.SetActiveFlag(false);
		m_CtrlTitle.RedrawWindow();
	}
}

void CIoViewValue::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();

	//
	RedrawWindow();
}

void CIoViewValue::OnIoViewFontChanged()
{
	CIoViewBase::OnIoViewFontChanged();
	
	//	
	m_GridCtrl.AutoSizeColumns(GVS_BOTH);
	m_GridCtrl.AutoSizeRows();

	//
	m_GridCtrl.RedrawWindow();
}

void CIoViewValue::RequestViewData()
{
	if ( NULL == m_pMerchXml )
	{
		return;
	}

	// ��������������
	CMmiReqRealtimePrice ReqPrice;
	
	ReqPrice.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
	ReqPrice.m_StrMerchCode	= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
	
	DoRequestViewData(ReqPrice);

	// �� F10 ��������
	CMmiReqPublicFile ReqF10;

	ReqF10.m_iMarketId		= m_pMerchXml->m_MerchInfo.m_iMarketId;
	ReqF10.m_StrMerchCode	= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
	ReqF10.m_ePublicFileType= EPFTF10;

	DoRequestViewData(ReqF10);

	// �� K ������, �������������ľ�����
	CMmiReqMerchKLine ReqKLine;

	ReqKLine.m_eKLineTypeBase = EKTBDay;
	ReqKLine.m_eReqTimeType	  = ERTYFrontCount;
	ReqKLine.m_TimeSpecify	  = m_pAbsCenterManager->GetServerTime();
	ReqKLine.m_iFrontCount	  = 2;
	
	DoRequestViewData(ReqKLine);

	//
	bool32 bH = false;
	if ( BeGeneralIndex(bH) )
	{
		if ( bH )
		{
			//
			CMmiReqGeneralFinance ReqGeneralFinance;
			ReqGeneralFinance.m_iMarketId = 0;
			
			DoRequestViewData(ReqGeneralFinance);
		}
		else
		{
			//
			CMmiReqGeneralFinance ReqGeneralFinance;
			ReqGeneralFinance.m_iMarketId = 1000;
			
			DoRequestViewData(ReqGeneralFinance);
		}
	}	
}

void CIoViewValue::OnVDataGeneralFinanaceUpdate(CMerch* pMerch)
{
	if ( NULL == m_pAbsCenterManager )
	{
		return;
	}
	
	if ( ERTExp != m_eMerchKind && ERTExpForeign != m_eMerchKind )
	{
		return;
	}

	//
	bool32 bH = false;

	if ( !BeGeneralIndex(bH) )
	{
		return;
	}

	//
	const T_GeneralFinance* pstGeneralFinance = NULL;
	if ( bH )
	{
		pstGeneralFinance = &m_pAbsCenterManager->GetGeneralFinanceH();
	}
	else
	{
		pstGeneralFinance = &m_pAbsCenterManager->GetGeneralFinanceS();
	}

	// ���±������

	LOGFONT fontNumber;
	memset(&fontNumber, 0, sizeof(fontNumber));
	_tcscpy(fontNumber.lfFaceName, gFontFactory.GetExistFontName(_T("Arial")));///
	fontNumber.lfHeight  = -14;
	fontNumber.lfWeight = 560;

	// �ܹɱ�
	m_GridCtrl.SetCellType(0, 1, RUNTIME_CLASS(CGridCellSys));
	CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	CString StrText = Float2String(pstGeneralFinance->m_fAllStock, 0, true);
	pCell->SetFont(&fontNumber);
	pCell->SetText(StrText);

	// ����ͨ��
	m_GridCtrl.SetCellType(1, 1, RUNTIME_CLASS(CGridCellSys));
	pCell = (CGridCellSys*)m_GridCtrl.GetCell(1, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	StrText = Float2String(pstGeneralFinance->m_fCircStock, 0, true);
	pCell->SetFont(&fontNumber);
	pCell->SetText(StrText);

	// ����ֵ
	m_GridCtrl.SetCellType(2, 1, RUNTIME_CLASS(CGridCellSys));
	pCell = (CGridCellSys*)m_GridCtrl.GetCell(2, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	StrText = Float2String(pstGeneralFinance->m_fAllValue, 0, true);
	pCell->SetFont(&fontNumber);
	pCell->SetText(StrText);

	// ��ͨ��ֵ
	m_GridCtrl.SetCellType(3, 1, RUNTIME_CLASS(CGridCellSys));
	pCell = (CGridCellSys*)m_GridCtrl.GetCell(3, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	StrText = Float2String(pstGeneralFinance->m_fCircValue, 0, true);
	pCell->SetFont(&fontNumber);
	pCell->SetText(StrText);

	// ƽ����ӯ��
	m_GridCtrl.SetCellType(4, 1, RUNTIME_CLASS(CGridCellSys));
	pCell = (CGridCellSys*)m_GridCtrl.GetCell(4, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	StrText = Float2String(pstGeneralFinance->m_fAvgPe, 2);
	pCell->SetFont(&fontNumber);
	pCell->SetText(StrText);

	// ƽ���о���
	m_GridCtrl.SetCellType(5, 1, RUNTIME_CLASS(CGridCellSys));
	pCell = (CGridCellSys*)m_GridCtrl.GetCell(5, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	StrText = Float2String(pstGeneralFinance->m_fAvgPureRate, 2);
	pCell->SetText(StrText);

	// ƽ��������
	m_GridCtrl.SetCellType(6, 1, RUNTIME_CLASS(CGridCellSys));
	pCell = (CGridCellSys*)m_GridCtrl.GetCell(6, 1);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	StrText = Float2String(pstGeneralFinance->m_fAvgPureGainRate, 2, false, true, true);
	pCell->SetFont(&fontNumber);
	pCell->SetText(StrText);

	m_GridCtrl.Refresh();
}

void CIoViewValue::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{
	if ( NULL == pMerch || NULL == m_pMerchXml || pMerch != m_pMerchXml || NULL == pMerch->m_pRealtimePrice )
	{
		return;
	}

	// �Ȼ�ȡ��Ӧ��K��
	int32 iPosFound;
	CMerchKLineNode* pKLineRequest = NULL;
	pMerch->FindMerchKLineNode(EKTBDay, iPosFound, pKLineRequest);

	if ( iPosFound < 0 || NULL == pKLineRequest )
	{
		return;
	}

	int32 iSizeKline = pKLineRequest->m_KLines.GetSize();
	if (  iSizeKline <= 0 )
	{
		return;
	}

	CKLine KLineNow = pKLineRequest->m_KLines.GetAt(iSizeKline - 1); 
	
	CKLine KLinePre = KLineNow;	
	if ( iSizeKline >= 2 )
	{
		KLinePre = pKLineRequest->m_KLines.GetAt(iSizeKline - 2);
	}

	//
	if ( ERTExp == m_eMerchKind || ERTExpForeign == m_eMerchKind )
	{
		//NULL;
	}
	else
	{
		// CDP =(H+L+C)/3  
		float fCDP = (KLineNow.m_fPriceHigh + KLineNow.m_fPriceLow + KLineNow.m_fPriceClose) / float(3.0);

		// AH=CDP+(H-L) 
		float fAH = fCDP + (KLineNow.m_fPriceHigh - KLineNow.m_fPriceLow);

		// NH=CDP*2-L 
		float fNH = fCDP * (float)2.0 - KLineNow.m_fPriceLow;

		// NL=CDP*2-H
		float fNL = fCDP * (float)2.0 - KLineNow.m_fPriceHigh;

		// AL=CDP-(H-L) 
		float fAL = fCDP - (KLineNow.m_fPriceHigh - KLineNow.m_fPriceLow);

		//	
		double p1,p2,a1,a2,v1,v2;
		int dk = 0;
		a2 = m_pMerchXml->m_pRealtimePrice->m_fSellAmount;
		v2 = m_pMerchXml->m_pRealtimePrice->m_fSellVolume;
		a1 = m_pMerchXml->m_pRealtimePrice->m_fAmountTotal - a2;
		v1 = m_pMerchXml->m_pRealtimePrice->m_fVolumeTotal - v2;

		if( v1 >0.0 && v2 >0.0 )
		{
			p2 = a2/(v2*100); //Ԫ
			p1 = a1/(v1*100); //Ԫ

			if( p1<p2 && a1<a2 ) //��ͷ
			{
				dk = 1;
				a1 = (p2+KLineNow.m_fPriceClose)/2;
				a2 = p2*1.015;
				v1 = p1*0.985;
			}
			else if( p1>=p2 && a1>=a2 ) //��ͷ
			{
				dk = 2;
				a1 = (p1+KLineNow.m_fPriceClose)/2;
				a2 = p1*0.985;
				v1 = p2*1.015;
			}
		}

		CString Str1 = L"-";
		CString Str2 = L"-";
		CString Str3 = L"-";

		float fValue1 = 0.;
		float fValue2 = 0.;
		float fValue3 = 0.;

		switch( dk )
		{
		case 0: //û��
			{
				Str1 = L"���ƽ��";
				Str2 = L"��ͷ����";
				Str3 = L"��ͷͣ��";
			}			
			break;
		case 1: //��ͷ
			{
				Str1 = L"���ƽ��";
				Str2 = L"��ͷ����";
				Str3 = L"��ͷͣ��";
				
				fValue1 = a1;
				fValue2 = a2;
				fValue3 = v1;
			}			
			break;
		case 2: //��ͷ
			{
				Str1 = L"���ƽ��";
				Str2 = L"��ͷ�ز�";
				Str3 = L"��ͷͣ��";
				
				fValue1 = a1;
				fValue2 = a2;
				fValue3 = v1;
			}
			break;
		}		
	
		// ���±������

		LOGFONT fontNumber;
		memset(&fontNumber, 0, sizeof(fontNumber));
		_tcscpy(fontNumber.lfFaceName, gFontFactory.GetExistFontName(_T("Arial")));///
		fontNumber.lfHeight  = -14;
		fontNumber.lfWeight = 560;

		// CDP
		m_GridCtrl.SetCellType(3, 3, RUNTIME_CLASS(CGridCellSymbol));
		CGridCellSymbol *pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(3, 3);
		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		CString StrText = Float2SymbolString(fCDP, pMerch->m_pRealtimePrice->m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);
		pCellSymbol->SetFont(&fontNumber);
		pCellSymbol->SetText(StrText);
		
		// AH
		m_GridCtrl.SetCellType(4, 1, RUNTIME_CLASS(CGridCellSymbol));
		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(4, 1);
		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		StrText = Float2SymbolString(fAH, pMerch->m_pRealtimePrice->m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);
		pCellSymbol->SetFont(&fontNumber);
		pCellSymbol->SetText(StrText);

		// NH
		m_GridCtrl.SetCellType(4, 3, RUNTIME_CLASS(CGridCellSymbol));
		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(4, 3);
		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		StrText = Float2SymbolString(fNH, pMerch->m_pRealtimePrice->m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);
		pCellSymbol->SetFont(&fontNumber);
		pCellSymbol->SetText(StrText);

		// NL
		m_GridCtrl.SetCellType(5, 1, RUNTIME_CLASS(CGridCellSymbol));
		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(5, 1);
		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		StrText = Float2SymbolString(fNL, pMerch->m_pRealtimePrice->m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);
		pCellSymbol->SetFont(&fontNumber);
		pCellSymbol->SetText(StrText);

		// AL
		m_GridCtrl.SetCellType(5, 3, RUNTIME_CLASS(CGridCellSymbol));
		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(5, 3);
		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		StrText = Float2SymbolString(fAL, pMerch->m_pRealtimePrice->m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);
		pCellSymbol->SetFont(&fontNumber);
		pCellSymbol->SetText(StrText);
	
		// ���ñ�����		
		{
			CGridCellSys* pCell = (CGridCellSys* )m_GridCtrl.GetCell(6, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
			pCell->SetText(Str1);
			
			//
			pCell = (CGridCellSys* )m_GridCtrl.GetCell(7, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
			pCell->SetText(Str2);
			
			//
			pCell = (CGridCellSys* )m_GridCtrl.GetCell(8, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
			pCell->SetText(Str3);
		}
	
		// ���ƽ��
		m_GridCtrl.SetCellType(6, 3, RUNTIME_CLASS(CGridCellSymbol));
		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(6, 3);
		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		StrText = Float2SymbolString(fValue1, pMerch->m_pRealtimePrice->m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);
		pCellSymbol->SetFont(&fontNumber);
		pCellSymbol->SetText(StrText);

		// ��ͷ����
		m_GridCtrl.SetCellType(7, 3, RUNTIME_CLASS(CGridCellSymbol));
		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(7, 3);
		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		StrText = Float2SymbolString(fValue2, pMerch->m_pRealtimePrice->m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);
		pCellSymbol->SetFont(&fontNumber);
		pCellSymbol->SetText(StrText);

		// ��ͷֹ��
		m_GridCtrl.SetCellType(8, 3, RUNTIME_CLASS(CGridCellSymbol));
		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(8, 3);
		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		StrText = Float2SymbolString(fValue3, pMerch->m_pRealtimePrice->m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);
		pCellSymbol->SetFont(&fontNumber);
		pCellSymbol->SetText(StrText);

		//		
		m_GridCtrl.Refresh();
	}

	//
	SetDrawLineInfo();
	m_GridCtrl.RedrawWindow();
}

void CIoViewValue::OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType)
{
	// �������ݸ���
	if ( NULL == pMerch || NULL == m_pMerchXml || pMerch != m_pMerchXml || EPFTF10 != ePublicFileType )
	{
		return;
	}

	//
	OnVDataRealtimePriceUpdate(pMerch);
}

void CIoViewValue::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	// �����ǵ�ͣ�۸�
	if ( NULL == pMerch || NULL == m_pMerchXml || pMerch != m_pMerchXml )
	{
		return;
	}

	//
	if ( NULL == pMerch->m_pRealtimePrice )
	{
		return;
	}

	//
	float fRiseMax, fFallMax;

	if ( CalcRiseFallMaxPrice(fRiseMax, fFallMax) )
	{
		// ����ɹ�, ����ֵ	
		
		//
		RedrawWindow();
	}


	if ( NULL == pMerch->m_pFinanceData )
	{
		return;
	}

	// ������ͨ��ֵ: ��ͨ�ɱ� * ��ǰ�۸�
	float fCircValue = pMerch->m_pFinanceData->m_fCircAsset * pMerch->m_pRealtimePrice->m_fPriceNew;	

	LOGFONT fontNumber;
	memset(&fontNumber, 0, sizeof(fontNumber));
	_tcscpy(fontNumber.lfFaceName, gFontFactory.GetExistFontName(_T("Arial")));///
	fontNumber.lfHeight  = -14;
	fontNumber.lfWeight = 560;

	// ���±������
	if ( ERTExp == m_eMerchKind || ERTExpForeign == m_eMerchKind )
	{
		//NULL;
	}
	else
	{
		// ��ͣ
		m_GridCtrl.SetCellType(0, 1, RUNTIME_CLASS(CGridCellSymbol));
		CGridCellSymbol *pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(0, 1);
		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		CString StrText = Float2SymbolString(fRiseMax, pMerch->m_pRealtimePrice->m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);
		pCellSymbol->SetFont(&fontNumber);
		pCellSymbol->SetText(StrText);

		// ��ͣ
		m_GridCtrl.SetCellType(0, 3, RUNTIME_CLASS(CGridCellSymbol));
		pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(0, 3);
		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		StrText = Float2SymbolString(fFallMax, pMerch->m_pRealtimePrice->m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);
		pCellSymbol->SetFont(&fontNumber);
		pCellSymbol->SetText(StrText);
	
		// ��ͨ��ֵ		
		m_GridCtrl.SetCellType(2, 1, RUNTIME_CLASS(CGridCellSys));
		CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(2, 3);
		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		StrText = Float2String(fCircValue, pMerch->m_MerchInfo.m_iSaveDec, true);
		pCell->SetFont(&fontNumber);
		pCell->SetText(StrText);

		//
		m_GridCtrl.Refresh();
	}

	//
	SetDrawLineInfo();
	m_GridCtrl.RedrawWindow();
}

void CIoViewValue::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch || NULL == pMerch)
		return;
	
	// �޸ĵ�ǰ�鿴����Ʒ
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
	
	// ���ù�ע����Ʒ��Ϣ
	m_aSmartAttendMerchs.RemoveAll();
	
	CSmartAttendMerch SmartAttendMerch;
	SmartAttendMerch.m_pMerch = pMerch;
	SmartAttendMerch.m_iDataServiceTypes = EDSTPrice | EDSTKLine | EDSTGeneral;        
	m_aSmartAttendMerchs.Add(SmartAttendMerch);

	//
	E_ReportType eMerchKind = GetMerchKind(pMerch);
	CreateTable(eMerchKind);

	//
	if ( ERTExp != eMerchKind && ERTExpForeign != eMerchKind )
	{
		// �õ����������Ϣ
		CBlockCollection::BlockArray aBlocks;
		CBlockConfig::Instance()->GetBlocksByMerch(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, aBlocks);
		
		bool32 bFind = false;

		for ( int32 i = 0; i < aBlocks.GetSize(); i++ )
		{
			if ( aBlocks[i]->m_blockCollection.m_StrName == L"��ҵ���" )
			{
				m_StrBlockName = aBlocks[i]->m_blockInfo.m_StrBlockName;
				bFind = true;
			}
		}
		
		if ( !bFind )
		{
			m_StrBlockName.Empty();
		}
	}

	LOGFONT fontChinese;
	memset(&fontChinese, 0, sizeof(fontChinese));
	_tcscpy(fontChinese.lfFaceName, gFontFactory.GetExistFontName(_T("Arial")));///
	fontChinese.lfHeight  = -14;
    fontChinese.lfWeight = 560;
	COLORREF clrChinese = RGB(254,254,254);

	// �������
	if ( m_GridCtrl.GetSafeHwnd() )
	{
		if ( ERTExp != eMerchKind && ERTExpForeign != eMerchKind )
		{
			m_GridCtrl.SetCellType(2, 1, RUNTIME_CLASS(CGridCellSys));
			CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(1, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);		
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(m_StrBlockName);
		}		
	}
	
	//
	OnVDataRealtimePriceUpdate(pMerch);
	OnVDataMerchKLineUpdate(pMerch);
	SetSize();

	//
	bool32 bH = false;
	if ( BeGeneralIndex(bH) )
	{
		KillTimer(KiTimerIdFiniance);
		SetTimer(KiTimerIdFiniance, KiTimerPeriodFiniance, NULL);
	}
	else
	{
		KillTimer(KiTimerIdFiniance);
	}
}

void CIoViewValue::OnVDataForceUpdate()
{
	RequestViewData();
}

bool32 CIoViewValue::CalcRiseFallMaxPrice(OUT float& fRiseMax, OUT float& fFallMax)
{
	fRiseMax = 0.;
	fFallMax = 0.;

	//
	if ( NULL == m_pMerchXml || NULL == m_pMerchXml->m_pRealtimePrice || m_pMerchXml->m_MerchInfo.m_StrMerchCnName.GetLength() <= 0 )
	{
		return false;
	}

	/*
	�ǵ�ͣ���㣺
	1�����еĹ�Ʊ+ָ��+����  
	��ͣ�ۣ� ����+����*10%�����һλ�������룩
	��ͣ�ۣ� ����-����*10%�����һλ�������룩
	2�����Ƶ�һ����ĸ��"N "
	�������ǵ�ͣ
	3������ǰ��λ��XR��XD��DR���Ĺ�Ʊ�ǳ�Ȩ��Ϣ�ģ������̱����ñ��۱��е������̼��㣬������K��ͼ�е����ռ���
	��ͣ�ۣ� ����+����*10%�����һλ�������룩
	��ͣ�ۣ� ����-����*10%�����һλ�������룩
	4�����ư�����ST��(ST��*ST�� SST��S*ST)
	��ͣ�ۣ� ����+����*5%�����һλ�������룩
	��ͣ�ۣ� ����-����*5%�����һλ�������룩
	*/

	float fRate		= 0.1f;
	float fClosePre	= m_pMerchXml->m_pRealtimePrice->m_fPricePrevClose;

	//
	if ( 'N' == m_pMerchXml->m_MerchInfo.m_StrMerchCnName[0] )
	{
		// �¹� ���ü����ǵ�ͣ
		return true;
	}


	//
	if ( -1 != m_pMerchXml->m_MerchInfo.m_StrMerchCnName.Find(L"ST") )
	{
		fRate = 0.05f;
	}

	// 
	float fTmp = fClosePre * fRate;

	// ��������, ������λС��
	fTmp = (int32)((fTmp * 100) + 0.5) / 100.0f;

	//
	fRiseMax = fClosePre + fTmp;
	fFallMax = fClosePre - fTmp;

	return true;
}

bool32 CIoViewValue::BeGeneralIndex(OUT bool32& bH)
{
	bH = false;

	if ( NULL == m_pMerchXml )
	{
		return false;
	}

	if ( 0 == m_pMerchXml->m_MerchInfo.m_iMarketId && L"000001" == m_pMerchXml->m_MerchInfo.m_StrMerchCode )
	{
		bH = true;
		return true;
	}
	else if ( 1000 == m_pMerchXml->m_MerchInfo.m_iMarketId && L"399001" == m_pMerchXml->m_MerchInfo.m_StrMerchCode )
	{
		bH = false;
		return true;
	}

	return false;
}

void CIoViewValue::OnTimer(UINT nIDEvent)
{	
	if(!m_bShowNow)
	{
		return;
	}

	if ( nIDEvent == KiTimerIdFiniance )
	{
		//
		bool32 bH = false;
		if ( BeGeneralIndex(bH) )
		{
			if ( bH )
			{
				//
				CMmiReqGeneralFinance ReqGeneralFinance;
				ReqGeneralFinance.m_iMarketId = 0;
				
				DoRequestViewData(ReqGeneralFinance);
			}
			else
			{
				//
				CMmiReqGeneralFinance ReqGeneralFinance;
				ReqGeneralFinance.m_iMarketId = 1000;
				
				DoRequestViewData(ReqGeneralFinance);
			}
		}
	}
	CIoViewBase::OnTimer(nIDEvent);
}