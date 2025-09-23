#include "stdafx.h"
#include "memdc.h"
#include "IoViewManager.h"
#include "IoViewRelative.h"
#include "MerchManager.h"
#include "GridCellSymbol.h"
#include "facescheme.h"
#include "ShareFun.h"
#include "IoViewTimeSale.h"
#include "MPIChildFrame.h"
#include "GridCellLevel2.h"
#include "GridCellCheck.h"
#include "MerchManager.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
IMPLEMENT_DYNCREATE(CIoViewRelative, CStatic)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewRelative, CStatic)
	//{{AFX_MSG_MAP(CIoViewRelative)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, 0x9999, OnSelChange)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_DBLCLK, 0x20205, OnGridDblClick)
	ON_NOTIFY(GVN_COLWIDTHCHANGED, 0x20205, OnGridColWidthChanged)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////
static const char * KStrElementAttrCurTab		= "CurTab";
static const char * KStrElementTabCheckInfo     = "TabCheckInfo";
static const char * KStrElementChecked			= "Checked";

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewRelative::CIoViewRelative()
:CIoViewBase()
{
	// 
	ASSERT(NULL != CReportScheme::Instance());
	CReportScheme::Instance()->GetReportHeadInfoList(ERTCustom,m_ReportHeadInfoList,m_iFixCol);
	m_iRowHeight	= 20;
	m_iTabFromXml  = 0;
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewRelative::~CIoViewRelative()
{
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CIoViewRelative::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonDown
void CIoViewRelative::OnLButtonDown(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonUp
void CIoViewRelative::OnLButtonUp(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonUp(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnMouseMove
void CIoViewRelative::OnMouseMove(UINT nFlags, CPoint point)
{	
	CStatic::OnMouseMove(nFlags, point);
}

int CIoViewRelative::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (-1 == CIoViewBase::OnCreate(lpCreateStruct))
		return -1;
	
	InitialIoViewFace(this);

	//创建Tab 条
	m_GuiTabWnd.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,0x9999);
	m_GuiTabWnd.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);
	// m_GuiTabWnd.SetImageList(IDB_TAB, 16, 16, 0x0000ff);

	// 创建横纵滚动条
	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
	m_XSBVert.SetScrollRange(0, 10);
	
	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
	m_XSBHorz.SetScrollRange(0, 10);

	// 创建数据表格
	m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 0x20205);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	// 设置表格图标
	m_ImageList.Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));
	// m_GridCtrl.SetImageList(&m_ImageList);

	// 设置相互之间的关联
	m_XSBHorz.SetOwner(&m_GridCtrl);
	m_XSBVert.SetOwner(&m_GridCtrl);
	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
		
	// 设置表头
	m_GridCtrl.SetHeaderSort(TRUE);
	ASSERT(m_iFixCol>=0 && m_iFixCol<=GHCOUNTS);
	m_GridCtrl.SetFixedColumnCount(m_iFixCol);

	//
	int32 iCountReportHeader = m_ReportHeadInfoList.GetSize();
	ASSERT(iCountReportHeader > 0);
	
	m_GridCtrl.SetFixedRowCount(1);
	m_GridCtrl.SetColumnCount(iCountReportHeader + 1);
	
	///////////////////////////////////////////////////
	// 根据文件中保存的值设置宽度
	
	CClientDC dc(this);
	CFont * pFontNormal = GetIoViewFontObject(ESFNormal);	
	dc.SelectObject(pFontNormal); 	
	TEXTMETRIC tmFontSize;
	dc.GetTextMetrics(&tmFontSize);
	m_iRowHeight = tmFontSize.tmHeight + 2; 

	//////////////////////////////////////////////////////////////////////////
	m_GridCtrl.SetCellType(0, 0, RUNTIME_CLASS(CGridCellCheck));
	CGridCellCheck *pCell1 = (CGridCellCheck *)m_GridCtrl.GetCell(0, 0);
	pCell1->SetFormat( DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX ); 	
	pCell1->SetText(L"关系");
	pCell1->SetCheck(false);
	
	for (int32 i = 0; i < iCountReportHeader; i++)
	{
		CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, i + 1);
		pCell->SetFormat( DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

		CString StrText = m_ReportHeadInfoList[i].m_StrHeadNameCn;
		ASSERT(StrText.GetLength()>0 && m_ReportHeadInfoList[i].m_iHeadWidth>0 );
		pCell->SetText(StrText);

		m_GridCtrl.SetColumnWidth(i+1,m_ReportHeadInfoList[i].m_iHeadWidth); 		
	}
	
	return 0;
}
void CIoViewRelative::SetRowHeightAccordingFont()
{
	CClientDC dc(this);
	
	// 设置行高
	CFont * pFontNormal = GetIoViewFontObject(ESFNormal);	
	CFont* pOldFont = dc.SelectObject(pFontNormal); 	
	TEXTMETRIC tmTime;
	dc.GetTextMetrics(&tmTime);
	m_iRowHeight = tmTime.tmHeight + 2; 
	
	// 第一行的行高
	CFont * pFontBig = GetIoViewFontObject(ESFBig);
	pOldFont  = dc.SelectObject(pFontBig); // font为要得其高宽的字体	
	
	dc.GetTextMetrics(&tmTime);
	int32 iRowHeight0 = tmTime.tmHeight + 2;  
	
	m_GridCtrl.SetRowHeight(0,iRowHeight0);
	dc.SelectObject(pOldFont);

	// 设置其他行高
	int32 iRowCounts = m_GridCtrl.GetRowCount();
	int32 i = 0;
	for ( i = 1 ; i< iRowCounts ; i++)
	{
		if ( m_GridCtrl.GetRowHeight(i) != m_iRowHeight )
		{
			//SetRowHeight内部会重新设置scrollbar,所以不要过多调用.
			m_GridCtrl.SetRowHeight(i,m_iRowHeight);
		}
	}

	// 设置列宽
	for ( i = 0 ; i < m_GridCtrl.GetColumnCount(); i++)
	{
		// 第一行的表头:
		CGridCellBase * pCell = m_GridCtrl.GetCell(0,i);
		
		if ( NULL == pCell )
		{
			return;
		}
		
		int32 iWidthHead,iWidthBody;
		iWidthHead = 30;
		iWidthBody = 30;
		
		CString StrHead = pCell->GetText();
		if ( 0 == i)
		{
			StrHead = L"一二三";
		}

		CFont * pTmpFontBig = GetIoViewFontObject(ESFBig);	
		CFont * pTmpOldFont = dc.SelectObject(pTmpFontBig);
		
		CSize sizeHead = dc.GetTextExtent(StrHead);
		iWidthHead = sizeHead.cx;
		
		dc.SelectObject(pTmpOldFont);
		
		if ( m_GridCtrl.GetRowCount() > 1)
		{
			CGridCellBase * pCellBody = m_GridCtrl.GetCell(1,i);
			
			if ( NULL == pCellBody)
			{
				return;
			}
			
			CString StrBody = pCellBody->GetText();
			CFont * pTmpFontNormal = GetIoViewFontObject(ESFNormal);
			pTmpOldFont = dc.SelectObject(pTmpFontNormal);
			
			CSize sizeBody = dc.GetTextExtent(StrBody);
			iWidthBody = sizeBody.cx;
			
			dc.SelectObject(pTmpOldFont);
		}
		
		int32 iWidth = iWidthHead > iWidthBody ? iWidthHead : iWidthBody;
		if ( 0 == i)
		{
			iWidth += 25;
		}
		else
		{
			iWidth += 5;						
		}
		
		m_GridCtrl.SetColumnWidth(i,iWidth);		
	}
}

void CIoViewRelative::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();
	m_GuiTabWnd.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);
	m_GuiTabWnd.Invalidate();
}

void CIoViewRelative::OnIoViewFontChanged()
{
	CIoViewBase::OnIoViewFontChanged();
	SetRowHeightAccordingFont();
	m_GridCtrl.Refresh();
	m_GridCtrl.Invalidate();
}

void CIoViewRelative::OnSize(UINT nType, int cx, int cy) 
{
	/*
	CIoViewBase::OnSize(nType, cx, cy);

	CRect RectClient = m_rectClient;
	CRect RectHScroll(RectClient);
	CRect RectVScroll(RectClient);
	CRect RectTabWnd(RectClient);
	CRect RectGrid(RectClient);
	
	int32 iHeightTabBar = m_GuiTabWnd.GetFitHorW();
	int32 iHeightHScroll = m_XSBHorz.GetFitHorW();
	int32 iWidthVScroll = m_XSBVert.GetFitHorW();
	
	RectTabWnd.top	   = m_RectTitle.top;
	RectTabWnd.bottom  = m_RectTitle.bottom;
	RectTabWnd.right   = m_RectTitle.left;


	bool32 bShowScroll = false;
	
	if ( 0 != m_GuiTabWnd.GetCount())
	{
		bShowScroll = true;
	}

	if ( bShowScroll)
	{
		RectVScroll.left   = RectVScroll.right - iWidthVScroll;
		RectVScroll.bottom = RectTabWnd.top;
		
		RectHScroll.bottom = RectTabWnd.top;
		RectHScroll.top    = RectHScroll.bottom - iHeightHScroll;
		RectHScroll.right  = RectVScroll.left;
		
		RectGrid.right	   = RectVScroll.left;
		RectGrid.bottom	   = RectHScroll.top;
				
		m_XSBHorz.SetSBRect(RectHScroll, FALSE);
		m_XSBVert.SetSBRect(RectVScroll, FALSE);
		
		m_XSBHorz.ShowWindow(SW_SHOW);
		m_XSBVert.ShowWindow(SW_SHOW);
		
	}
	else
	{
		RectGrid = m_rectClient;
		
		m_XSBHorz.ShowWindow(SW_HIDE);
		m_XSBVert.ShowWindow(SW_HIDE);
	}
	
	m_GuiTabWnd.SetALingTabs(CGuiTabWnd::ALN_BOTTOM);
	m_GuiTabWnd.MoveWindow(RectTabWnd);
	m_GuiTabWnd.ShowWindow(SW_SHOW);
	
	m_GridCtrl.MoveWindow(RectGrid);
	m_GridCtrl.ShowWindow(SW_SHOW);
	*/
}

BOOL CIoViewRelative::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return FALSE;
}
// 通知视图改变关注的商品
void CIoViewRelative::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch || NULL == pMerch)
		return;
	
	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
	
	// 设置关注的商品信息
	m_aSmartAttendMerchs.RemoveAll();
	
	CSmartAttendMerch SmartAttendMerch;
	SmartAttendMerch.m_pMerch = pMerch;
	SmartAttendMerch.m_iDataServiceTypes = EDSTPrice;
	m_aSmartAttendMerchs.Add(SmartAttendMerch);

	// 当前商品不为空的时候
	if (NULL != pMerch)
	{
		// 获取当前商品的关联商品
		// m_pAbsCenterManager->FindRelativeMerchs(m_StrMerchCode, m_iMarketId, m_AllRelativeMerchs);

		// 清除当前表格显示的内容
		DeleteTableContent();

		// 重建tab条
		BuildTabWnd();

		// 选择tab条第一项
		int32 iCurSelBack = m_GuiTabWnd.GetCurtab();
		SetCurTab(0);

		// 如果没有发生改变， 控件不会自动通知， 需要手动的操作
		if (m_GuiTabWnd.GetCurtab() == iCurSelBack)
		{
			OnTabSelChange();
		}
	}
	else
	{
		// zhangbo 20090824 #待补充， 当心商品不存在时，清空当前显示数据
		//...
	}
}

void CIoViewRelative::OnVDataForceUpdate()
{
	if (NULL == m_pMerchXml || NULL == m_pAbsCenterManager)
		return;

	m_pAbsCenterManager->FindRelativeMerchs(m_pMerchXml, m_AllRelativeMerchs);
		
	DeleteTableContent();	
	BuildTabWnd();	

    int32 iCurSelBack = m_GuiTabWnd.GetCurtab();

	SetCurTab(m_iTabFromXml);
	if (iCurSelBack == m_iTabFromXml)
	{
		OnTabSelChange();
	}		
	if (m_iTabFromXml<0 || m_iTabFromXml > 100)
	{
		OnTabSelChange();
		SetCurTab(0);
	}
}

void CIoViewRelative::OnVDataGridHeaderChanged(E_ReportType eReportType)
{
	ASSERT(NULL != CReportScheme::Instance());
	
	ASSERT(m_GridCtrl.DeleteAllItems());

	m_ReportHeadInfoList.RemoveAll();
	m_iFixCol = -1;

	// 新得到表头数据
	CReportScheme::Instance()->GetReportHeadInfoList(ERTCustom,m_ReportHeadInfoList,m_iFixCol);

	ASSERT(m_ReportHeadInfoList.GetSize()>0);
	ASSERT(m_iFixCol != -1);
	
	m_GridCtrl.SetHeaderSort(TRUE);
	ASSERT(m_iFixCol>=0 && m_iFixCol<=GHCOUNTS);
	m_GridCtrl.SetFixedColumnCount(m_iFixCol);
	
	int32 iCountReportHeader = m_ReportHeadInfoList.GetSize();
	
	ASSERT(iCountReportHeader > 0);
	
	m_GridCtrl.SetFixedRowCount(1);
	m_GridCtrl.SetColumnCount(iCountReportHeader + 1);
	
	///////////////////////////////////////////////// 
	m_GridCtrl.SetCellType(0, 0, RUNTIME_CLASS(CGridCellCheck));
	CGridCellCheck *pCell1 = (CGridCellCheck *)m_GridCtrl.GetCell(0, 0);
	pCell1->SetFormat( DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX ); 	
	pCell1->SetText(L"关系");
	pCell1->SetCheck(false);
	
	for (int32 i = 0; i < iCountReportHeader; i++)
	{
		CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, i + 1);
		pCell->SetFormat( DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		
		CString StrText = m_ReportHeadInfoList[i].m_StrHeadNameCn;
		ASSERT(StrText.GetLength()>0 && m_ReportHeadInfoList[i].m_iHeadWidth>0 );
		pCell->SetText(StrText);

		m_GridCtrl.SetColumnWidth(i+1,m_ReportHeadInfoList[i].m_iHeadWidth); 		
	}
	

	OnTabSelChange();
	//SetRowHeightAccordingFont();
}

void CIoViewRelative::OnVDataRelativeMerchsUpdate()
{
	// zhangbo 0709 #待补充
	//...
}
void CIoViewRelative::OnIoViewGroupIdChange(int32 iNewGroupId)
{
	// 视图分组改变的时候,打勾要发生变化.更新自己的m_TableCheckInfo 数组
	m_TableCheckInfo.RemoveAll();
	ASSERT( NULL!=m_pAbsCenterManager );

	int32 iIoViewGroupId = GetIoViewGroupId();
		
	// 与ViewData 中叠加视图数据进行匹配
	CGridCellCheck *pCell0 = (CGridCellCheck *)m_GridCtrl.GetCell(0, 0);
	pCell0->SetCheck(false);

	for (int32 i = 1 ; i< m_GridCtrl.GetRowCount(); i++)
	{
		CGridCellCheck * pCell = (CGridCellCheck *)m_GridCtrl.GetCell(i,0);
		pCell->SetCheck(false);
		
		CMerch * pMerch = (CMerch *)m_GridCtrl.GetItemData(i,0);
		if ( NULL != pMerch)
		{	
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			for (int32 j = 0; j< pFrame->m_aCompareMerchList.GetSize(); j++)
			{				
				if (iIoViewGroupId == pFrame->m_aCompareMerchList[j].m_iGroupId && pMerch == pFrame->m_aCompareMerchList[j].m_pMerch)
				{
					// 找到匹配的,打勾
					CGridCellCheck * pCheckCell = (CGridCellCheck *)m_GridCtrl.GetCell(i,0);
					pCheckCell->SetCheck(true);					
					pCell0->SetCheck(true);

					T_TableCheckInfo TabCheckInfo;
					TabCheckInfo.m_pMerch	= pMerch;
					m_TableCheckInfo.Add(TabCheckInfo);
				}				
			}
		}
	}
}

void CIoViewRelative::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch)
		return;

	if (NULL == m_GridCtrl.GetSafeHwnd())
		return;

	int iRowFound = -1;

	// 查找该商品是否在显示列表里面
	int32 iRow;
	for (iRow = 1; iRow < m_GridCtrl.GetRowCount(); iRow++)
	{
		CGridCellSys *pFirstCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
		ASSERT(NULL != pFirstCell);

		//
		CMerch *pMerchRow = (CMerch *)pFirstCell->GetData();
		ASSERT(NULL != pMerchRow);
		
		if (pMerchRow == pMerch)
		{
			iRowFound = iRow;
			break;
		}
	}
	
	if (iRow < 0)
		return;

	// 仅更新该行， 提高效率
	UpdateSpecifyTableRow(iRowFound, true);
}

bool32 CIoViewRelative::FromXml(TiXmlElement * pTiXmlElement)
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

	m_TableCheckInfo.RemoveAll();

	SetFontsFromXml(pTiXmlElement);
	SetColorsFromXml(pTiXmlElement);
	//SetRowHeightAccordingFont();

  	// 读取本业务视图特有的内容
	int32   iMarketId		= -1;
	CString StrMerchCode	= L"";
  	m_iTabFromXml			= 0;
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
 	pcAttrValue = pTiXmlElement->Attribute(GetXmlElementAttrCurTab());
 	if (NULL != pcAttrValue)
 	{
 		m_iTabFromXml = atoi(pcAttrValue);
 	}

  	TiXmlElement * pTiXmlElementTabcheckInfo = pTiXmlElement->FirstChildElement();
	if ( NULL == pTiXmlElementTabcheckInfo)
		return false;

	pcValue = pTiXmlElementTabcheckInfo->Value();
	if (NULL == pcValue || strcmp(GetXmlElementTabCheckInfo(), pcValue) != 0)
		return false;

	T_TableCheckInfo TabCheckedItems;
	TiXmlElement * pTiXmlElementChecked = pTiXmlElementTabcheckInfo->FirstChildElement();
	while (pTiXmlElementChecked)
	{
		pcValue = pTiXmlElementChecked->Value();
		if (NULL == pcValue || strcmp(GetXmlElementAttrChecked() , pcValue) != 0)
			return false;

		CString StrCurMerchCode;
		int32 iTmpMarketId = -1;

		pcAttrValue = pTiXmlElementChecked->Attribute(GetXmlElementAttrMerchCode());
		if (NULL != pcAttrValue)
		{
			StrCurMerchCode = pcAttrValue;
		}
		pcAttrValue = pTiXmlElementChecked->Attribute(GetXmlElementAttrMarketId());
		if (NULL != pcAttrValue)
		{
			iTmpMarketId = atoi(pcAttrValue);
		}
		
		CMerch *pMerch = NULL;
		if (m_pAbsCenterManager->GetMerchManager().FindMerch(StrCurMerchCode, iTmpMarketId, pMerch))
		{
			TabCheckedItems.m_pMerch = pMerch;
		}

		//m_TableCheckInfo.Add(TabCheckedItems);			
		pTiXmlElementChecked = pTiXmlElementChecked->NextSiblingElement();
	}	
	
	// 
	CMerch *pMerchFound = NULL;
	if (!m_pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerchFound))
	{
		pMerchFound = NULL;
	}

  	// 商品发生改变
  	OnVDataMerchChanged(iMarketId, StrMerchCode, pMerchFound);
	m_GuiTabWnd.SetCurtab(m_iTabFromXml);	
	
	return true;
}

void CIoViewRelative::SaveColoumWidth()
{
	if ( !m_GridCtrl.GetSafeHwnd())
	{
		return;
	}
	
	// 第一列是复选框那列,不在计算范围内:

	if ( m_GridCtrl.GetColumnCount() < 2 || m_GridCtrl.GetRowCount() < 1)
	{
		return;
	}
	
	for ( int32 i = 1 ; i < m_GridCtrl.GetColumnCount(); i ++)
	{		
		int32 iWidth = m_GridCtrl.GetColumnWidth(i);
		if ( iWidth > 0)
		{
			CReportScheme::Instance()->SaveColumnWidth(ERTCustom,i-1,iWidth);
		}		
	}
}

CString CIoViewRelative::ToXml()
{
	SaveColoumWidth();

	CString StrThis,StrMarketId,StrCurTab;	
	
	StrMarketId.Format(L"%d", m_MerchXml.m_iMarketId);
	StrCurTab.Format(L"%d", m_GuiTabWnd.GetCurtab());

	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" ", /*>\n*/
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(),
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrShowTabName()).GetBuffer(), 
		m_StrTabShowName.GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(), 
		m_MerchXml.m_StrMerchCode.GetBuffer(),
		CString(GetXmlElementAttrMarketId()).GetBuffer(),
		StrMarketId.GetBuffer(),   
		CString(GetXmlElementAttrCurTab()).GetBuffer(),
		StrCurTab.GetBuffer());
	
	CString StrFace;
	StrFace  = SaveColorsToXml();
	StrFace += SaveFontsToXml();
	
	StrThis += StrFace;
	StrThis += L">\n";

	StrThis += L"<";
	StrThis += GetXmlElementTabCheckInfo(); 	
	StrThis += L"> \n";						 

	// 保存check信息
	for (int32 i = 0 ; i < m_TableCheckInfo.GetSize() ; i++)
	{
		CString StrCheckedItem;
		StrCheckedItem.Format(L"<%s %s=\"%s\" %s=\"%d\"></%s>\n",
			CString(GetXmlElementAttrChecked()).GetBuffer(),
			CString(GetXmlElementAttrMerchCode()).GetBuffer(), m_TableCheckInfo[i].m_pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(),
			CString(GetXmlElementAttrMarketId()).GetBuffer(), m_TableCheckInfo[i].m_pMerch->m_MerchInfo.m_iMarketId,
			CString(GetXmlElementAttrChecked()).GetBuffer());

		StrThis += StrCheckedItem;
	}
		
	StrThis += L"</";
	StrThis += GetXmlElementTabCheckInfo(); 	
	StrThis += L"> \n";	

	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";	
	return StrThis;
}

CString CIoViewRelative::GetDefaultXML()
{
	CString StrThis;

	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" >\n", 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(), 
		L"",
		CString(GetXmlElementAttrMarketId()).GetBuffer(), 
		L"-1",
		CString(GetXmlElementAttrCurTab()).GetBuffer(),
		L"0");


	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

void CIoViewRelative::OnSelChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnTabSelChange();

	*pResult = 0;
}

void CIoViewRelative::OnTabSelChange()
{	
	int32 iCurSel = m_GuiTabWnd.GetCurtab();
	if (iCurSel < 0 || iCurSel >= m_GuiTabWnd.GetCount())
		return;

	m_GridCtrl.DeleteNonFixedRows();
	m_XSBVert.SetScrollPos(0);

	//
	ASSERT(m_GridCtrl.GetColumnCount() > 0);

	//
	ASSERT(iCurSel >=0 && iCurSel < m_AllRelativeMerchs.GetSize());

	//
	m_aSmartAttendMerchs.RemoveAll();


	// 根据当前选中的关联商品列表， 设置足够的行数， 并且指向对应的商品
	if (m_AllRelativeMerchs[iCurSel].m_SiblingMerchList.GetSize() > 0)
	{
		CMmiReqRealtimePrice Req;

		// 
		for (int32 iMerchIndex = 0; iMerchIndex < m_AllRelativeMerchs[iCurSel].m_SiblingMerchList.GetSize() ; iMerchIndex++)
		{
			CSiblingMerch &SiblingMerch = m_AllRelativeMerchs[iCurSel].m_SiblingMerchList[iMerchIndex];

			// 确定该商品确实存在后， 即可添加条目
			CMerch *pMerch = SiblingMerch.m_pMerch;
			if (NULL != pMerch)
			{
				// 添加条目
				m_GridCtrl.InsertRow(SiblingMerch.m_StrSiblingName);

				// 设置为checkcell,设置行对应的商品指针,判断这个商品是不是叠加商品,更新m_TableCheckInfo
				 
				m_GridCtrl.SetCellType(m_GridCtrl.GetRowCount() - 1,0, RUNTIME_CLASS(CGridCellCheck));

				CGridCellCheck *pCell = (CGridCellCheck *)m_GridCtrl.GetCell(m_GridCtrl.GetRowCount() - 1, 0);			
				ASSERT(NULL != pCell);
				pCell->SetCheck(false);
				pCell->SetData((LPARAM)pMerch);
				
				int32 iIoViewGroupId = GetIoViewGroupId();

				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				for (int32 i=0; i<pFrame->m_aCompareMerchList.GetSize();i++)
				{
					if (pFrame->m_aCompareMerchList[i].m_iGroupId == iIoViewGroupId && pFrame->m_aCompareMerchList[i].m_pMerch == SiblingMerch.m_pMerch)
					{
						pCell->SetCheck(true);
						CGridCellCheck *pCell0 = (CGridCellCheck *)m_GridCtrl.GetCell(0, 0);
						pCell0->SetCheck(true);

						T_TableCheckInfo TabCheckInfo;
						TabCheckInfo.m_pMerch	= SiblingMerch.m_pMerch;
						m_TableCheckInfo.Add(TabCheckInfo);
					}
				}		
	
				// 设置关注该商品
				CSmartAttendMerch SmartAttendMerch;
				SmartAttendMerch.m_pMerch			= SiblingMerch.m_pMerch;
				SmartAttendMerch.m_iDataServiceTypes = EDSTPrice;
				m_aSmartAttendMerchs.Add(SmartAttendMerch);
			
				// 请求该商品的5档行情
				if (iMerchIndex == 0)
				{
					Req.m_iMarketId		= SiblingMerch.m_pMerch->m_MerchInfo.m_iMarketId;
					Req.m_StrMerchCode	= SiblingMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;
				}
				else
				{
					CMerchKey MerchKey;
					MerchKey.m_iMarketId	= SiblingMerch.m_pMerch->m_MerchInfo.m_iMarketId;
					MerchKey.m_StrMerchCode = SiblingMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;
					Req.m_aMerchMore.Add(MerchKey);
				}
			}

			// 将每行的第一格设置为CheckCell
			m_GridCtrl.SetCellType(m_GridCtrl.GetRowCount() - 1,0, RUNTIME_CLASS(CGridCellCheck));
		}

		DoRequestViewData(Req);
	}
	
	// 更新表格内容
	UpdateTableContent();
	
	CCellID cellID;
	CPoint pt(0,0);
	E_ReportType eMarketReportType = ERTStockCn;

	if (NULL != m_pMerchXml)
	{
		eMarketReportType = m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType;
	}
	
	if (CReportScheme::IsFuture(eMarketReportType))
	{
		// 如果是期货,那么按名称排序		
		for ( int32 i = 0 ; i < m_GridCtrl.GetColumnCount(); i++)
		{
			CGridCellBase* pCell = m_GridCtrl.GetCell(0,i);
			if (NULL != pCell)
			{
				if ( pCell->GetText() == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHMerchName))
				{
					cellID.row = 0;
					cellID.col = i;
					
					m_GridCtrl.OnFixedRowClick(cellID,pt);	
					
					break;
				}
			}
		}
	}
	else
	{
		// 如果是其他的商品,则按代码排序
		for ( int32 i = 0 ; i < m_GridCtrl.GetColumnCount(); i++)
		{
			CGridCellBase* pCell = m_GridCtrl.GetCell(0,i);
			if (NULL != pCell)
			{
				if ( pCell->GetText() == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHMerchCode))
				{
					cellID.row = 0;
					cellID.col = i;
					
					m_GridCtrl.OnFixedRowClick(cellID,pt);	
					break;
				}
			}
		}
	}
}

bool32	CIoViewRelative::OnVDataAddCompareMerch(IN CMerch *pMerch)
{
	if (NULL == pMerch)
		return false;

	// 叠加商品的时候,通知除了本视图以外其他同组的相关视图打勾
	T_TableCheckInfo TabelCheckInfo;				
	TabelCheckInfo.m_pMerch = pMerch;
	m_TableCheckInfo.Add(TabelCheckInfo);	

	for (int32 i=1 ; i<m_GridCtrl.GetRowCount();i++)
	{
		CMerch * pMerchRow = (CMerch *)m_GridCtrl.GetItemData(i,0);
		ASSERT(NULL != pMerch);

		if (pMerch == pMerchRow)
		{
			CGridCellCheck * pCell = (CGridCellCheck *)m_GridCtrl.GetCell(i,0);
			pCell->SetCheck(true);
			pCell = (CGridCellCheck *)m_GridCtrl.GetCell(0,0);
			if (!pCell->GetCheck())
			{
				pCell->SetCheck(true);
			}
		}
	}
	return false;
}

void CIoViewRelative::OnVDataRemoveCompareMerch(IN CMerch *pMerch)
{
	if (NULL == pMerch)
		return;

	int32 i;
	for ( i=0 ;i<m_TableCheckInfo.GetSize();i++)
	{
		if (pMerch == m_TableCheckInfo[i].m_pMerch)
		{
			m_TableCheckInfo.RemoveAt(i);
		}
	}

	for ( i=1 ; i<m_GridCtrl.GetRowCount();i++)
	{
		CMerch * pMerchRow = (CMerch *)m_GridCtrl.GetItemData(i,0);
		ASSERT(NULL != pMerchRow);
		if (pMerch == pMerchRow)
		{
			CGridCellCheck * pCell = (CGridCellCheck *)m_GridCtrl.GetCell(i,0);
			pCell->SetCheck(false);
			break;
		}
	}

	bool32 bCheck = false;
	for ( i=1 ; i< m_GridCtrl.GetRowCount();i++)
	{
		CGridCellCheck * pCell = (CGridCellCheck *)m_GridCtrl.GetCell(i,0);
		if (pCell->GetCheck())
		{
			bCheck = true;
			return;
		}
	}
	if (!bCheck)
	{
		CGridCellCheck * pCell = (CGridCellCheck *)m_GridCtrl.GetCell(0,0);
		pCell->SetCheck(false);
	}
}
void CIoViewRelative::SendInfomation(bool32 bAdd,int32 iRow,int32 iColoumn)
{
	// ...fangz0901 叠加商品暂时没处理
	return;

	// 向所有同组视图发消息.
	
	CMerch * pMerch = (CMerch *)m_GridCtrl.GetItemData(iRow,0);
	ASSERT( NULL!= pMerch );
	ASSERT( NULL != m_pAbsCenterManager);

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (bAdd)
	{

		bool32 bAddSuccess = pFrame->OnViewAddCompareMerch( this, pMerch);	
		if (!bAddSuccess)
		{
			// 添加不成功,再删除这个商品,取消打勾
			pFrame->OnViewRemoveCompareMerch( this, pMerch);			
		}
	}
	else
	{
		pFrame->OnViewRemoveCompareMerch(this, pMerch);
	}
}

void CIoViewRelative::DeleteTableContent()
{
	m_GridCtrl.DeleteNonFixedRows();
}

void CIoViewRelative::BuildTabWnd()
{
	m_GuiTabWnd.DeleteAll();

	//
	for (int32 i = 0; i < m_AllRelativeMerchs.GetSize(); i++)
	{
		CString StrTabText = m_AllRelativeMerchs[i].m_RelativeSummary;

		m_GuiTabWnd.Addtab(StrTabText, StrTabText, StrTabText);
	}

	CRect rect;
	GetClientRect(&rect);
	OnSize(0,rect.Width(),rect.Height());
}

void CIoViewRelative::SetCurTab(int32 iTab)
{
	if (iTab < 0 || iTab >= m_GuiTabWnd.GetCount())
		return;
	m_GuiTabWnd.SetCurtab(iTab);
}

// 更新指定行
void CIoViewRelative::UpdateSpecifyTableRow(int32 iRow, bool32 bShowBlink)
{
	if (NULL == m_GridCtrl.GetSafeHwnd())
		return;

	if (iRow <= 0 || iRow >= m_GridCtrl.GetRowCount())
		return;

	CGridCellSys *pFirstCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
	ASSERT(NULL != pFirstCell);

	//
	CMerch *pMerch = (CMerch *)pFirstCell->GetData();
	ASSERT(NULL != pMerch);

	// 
	CRealtimePrice RealtimePrice;
	if (NULL != pMerch && NULL != pMerch->m_pRealtimePrice)
	{
		RealtimePrice = *pMerch->m_pRealtimePrice;
	}

	// 
	CMerchExtendData ExtendData;
	if (NULL != pMerch && NULL != pMerch->m_pMerchExtendData)
	{
		ExtendData = *pMerch->m_pMerchExtendData;
	}

	// 跟据列表头显示的内容， 显示具体的数据
	for (int32 iCol = 1; iCol < m_GridCtrl.GetColumnCount(); iCol++)
	{
		int32 iSize = m_ReportHeadInfoList.GetSize();
		ASSERT(iCol - 1 < iSize);

		if ( iCol -1 >= iSize )
		{
			iCol = iCol;
		}

		CReportScheme::E_ReportHeader ERHType=CReportScheme::Instance()->GetReportHeaderEType(m_ReportHeadInfoList[iCol-1].m_StrHeadNameCn);
		switch ( ERHType )
		{
		case CReportScheme::ERHRowNo: 
			{
				CGridCellSys *pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
				CString StrRowNo;
				pCell->SetFormat( DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetCompareFloat((float)iRow);
				StrRowNo.Format(L"%d", iRow);				
				pCell->SetText(StrRowNo);				
			}
			break;
		case CReportScheme::ERHMerchCode:
			{
				CGridCellSys *pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
				pCell->SetFormat( DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetText(pMerch->m_MerchInfo.m_StrMerchCode);
			}
			break;
		case CReportScheme::ERHMerchName:
			{
				CGridCellSys *pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
				pCell->SetFormat( DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetText(pMerch->m_MerchInfo.m_StrMerchCnName);
			}
			break;
		case CReportScheme::ERHPricePrevClose:
		case CReportScheme::ERHPricePrevBalance:
			{
				m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
				CGridCellSymbol *pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
				pCellSymbol->SetFormat( DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCellSymbol->SetCompareFloat((float)RealtimePrice.m_fPricePrevClose);
				CString StrPricePrevClose = Float2SymbolString(RealtimePrice.m_fPricePrevClose, RealtimePrice.m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);

				if (bShowBlink)
					pCellSymbol->SetBlinkText(StrPricePrevClose);
				else
					pCellSymbol->SetText(StrPricePrevClose);
			}
			break;
		case CReportScheme::ERHPriceOpen:
			{
				m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
				CGridCellSymbol *pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
				pCellSymbol->SetFormat( DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCellSymbol->SetCompareFloat((float)RealtimePrice.m_fPriceOpen);
				CString StrPriceOpen = Float2SymbolString(RealtimePrice.m_fPriceOpen, RealtimePrice.m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);

				if (bShowBlink)
					pCellSymbol->SetBlinkText(StrPriceOpen);
				else
					pCellSymbol->SetText(StrPriceOpen);
			}
			break;
		case CReportScheme::ERHPriceNew:
		case CReportScheme::ERHPriceBalance:
			{
				m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
				CGridCellSymbol *pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
				pCellSymbol->SetFormat( DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCellSymbol->SetCompareFloat((float)RealtimePrice.m_fPriceNew);
				CString StrPriceNew = Float2SymbolString(RealtimePrice.m_fPriceNew, RealtimePrice.m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);

				if (bShowBlink)
					pCellSymbol->SetBlinkText(StrPriceNew);
				else
					pCellSymbol->SetText(StrPriceNew);
			}
			break;
		case CReportScheme::ERHPriceHigh:
			{
				m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
				CGridCellSymbol *pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
				pCellSymbol->SetFormat( DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCellSymbol->SetCompareFloat((float)RealtimePrice.m_fPriceHigh);
				CString StrPriceHigh = Float2SymbolString(RealtimePrice.m_fPriceHigh, RealtimePrice.m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);

				if (bShowBlink)
					pCellSymbol->SetBlinkText(StrPriceHigh);
				else
					pCellSymbol->SetText(StrPriceHigh);
			}
			break;
		case CReportScheme::ERHPriceLow:
			{
				m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
				CGridCellSymbol *pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
				pCellSymbol->SetFormat( DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCellSymbol->SetCompareFloat((float)RealtimePrice.m_fPriceLow);
				CString StrPriceLow = Float2SymbolString(RealtimePrice.m_fPriceLow, RealtimePrice.m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);

				if (bShowBlink)
					pCellSymbol->SetBlinkText(StrPriceLow);
				else
					pCellSymbol->SetText(StrPriceLow);
			}
			break;
		case CReportScheme::ERHVolumeCur:
			{
				CGridCellSys *pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
				pCell->SetFormat( DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);

				CString StrVolumeCur = Float2String(RealtimePrice.m_fVolumeCur, 0, true);
				pCell->SetText(StrVolumeCur);
			}
			break;
		case CReportScheme::ERHVolumeTotal:
			{
				CGridCellSys *pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
				pCell->SetFormat( DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);
				CString StrVolumeTotal = Float2String(RealtimePrice.m_fVolumeTotal, 0, true);
				pCell->SetText(StrVolumeTotal);
			}
			break;
		case CReportScheme::ERHRiseFall:			// 涨跌
			{
				m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
				CGridCellSymbol *pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);				
				CString StrRiseValue = Float2SymbolString(RealtimePrice.m_fPriceNew - RealtimePrice.m_fPricePrevClose, 0, pMerch->m_MerchInfo.m_iSaveDec);	
				pCellSymbol->SetText(StrRiseValue);
			}
			break; 
		case CReportScheme::ERHRange:				// 幅度%
			{
				m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
				CGridCellSymbol * pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				float fRisePercent = 0.;
				if ( 0. != RealtimePrice.m_fPriceNew )
				{					
					if (0. != RealtimePrice.m_fPricePrevClose)
					{
						fRisePercent = ((RealtimePrice.m_fPriceNew - RealtimePrice.m_fPricePrevClose) / RealtimePrice.m_fPricePrevClose) * 100.;
					}
				}
				CString StrPriceRisePercent = Float2SymbolString(fRisePercent, 0, 2);
				pCellSymbol->SetText(StrPriceRisePercent);
			}
			break;
		case CReportScheme::ERHSwing:				// 振幅%
			{
				m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
				CGridCellSymbol * pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

				float fValue = 0.0;

				if ( 0. != RealtimePrice.m_fPriceHigh || 0. != RealtimePrice.m_fPriceLow)
				{
					fValue = (RealtimePrice.m_fPriceHigh-RealtimePrice.m_fPriceLow) * 100.0f/RealtimePrice.m_fPricePrevClose ;
				}
				CString StrValue = Float2SymbolString(fValue,fValue,2,false,true,true);
				
				pCellSymbol->SetText(StrValue);	
			}
			break;
		case CReportScheme::ERHTime:				// 时间
			{
				CGridCellSys *pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
				pCell->SetFormat( DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				
				CString StrTime;
				StrTime.Format(L"%02d/%02d/%02d %02d:%02d:%02d", RealtimePrice.m_TimeCurrent.m_Time.GetYear(), RealtimePrice.m_TimeCurrent.m_Time.GetMonth(), RealtimePrice.m_TimeCurrent.m_Time.GetDay(), 
					RealtimePrice.m_TimeCurrent.m_Time.GetHour(), RealtimePrice.m_TimeCurrent.m_Time.GetMinute(), RealtimePrice.m_TimeCurrent.m_Time.GetSecond());
				pCell->SetText(StrTime);	
			}
			break;
		case CReportScheme::ERHHold:			
			{
				CGridCellSys* pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
				pCell->SetDefaultTextColor(ESCAmount);
				CString StrHoldTotal = Float2String(RealtimePrice.m_fHoldTotal, 0, true);
				pCell->SetText(StrHoldTotal);
				pCell->SetCompareFloat(RealtimePrice.m_fHoldTotal);					
			}
			break;				
		case CReportScheme::ERHBuyVolumn:
			{
				CGridCellSys *pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
				pCell->SetDefaultTextColor(ESCVolume);
				CString StrBuyVolume = Float2String(RealtimePrice.m_fBuyVolume, 0, true);
				pCell->SetText(StrBuyVolume);
				pCell->SetCompareFloat(RealtimePrice.m_fBuyVolume);
			}
			break;
		case CReportScheme::ERHSellVolumn:
			{
				CGridCellSys *pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
				pCell->SetDefaultTextColor(ESCVolume);
				CString StrSellVolume = Float2String(RealtimePrice.m_fSellVolume, 0, true);
				pCell->SetText(StrSellVolume);
				pCell->SetCompareFloat(RealtimePrice.m_fSellVolume);
			}
			break;
		case CReportScheme::ERHTrend:
			break;
		case CReportScheme::ERHBuild:
			break;
		case CReportScheme::ERHClear:
			break;
		case CReportScheme::ERHBuildPer:
			break;
		case CReportScheme::ERHClearPer:
			break;
		case CReportScheme::ERHAddPer:
			break;
		default:
			{
				m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
				CGridCellSymbol* pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				
				float fdefault = 0.0;					
				CString StrValue = Float2SymbolString(fdefault,0.0,1,false);
				pCellSymbol->SetText(StrValue);		
			}
			break;
		}

		m_GridCtrl.InvalidateCellRect(iRow, iCol);
	}
} 

void CIoViewRelative::UpdateTableContent()
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

/*	for (int32 k = 0 ; k< m_GridCtrl.GetColumnCount() ; k++)
	{
		//LOGFONT * pBigFont =  pParentFrame->GetFrameFont(ESFBig);
		LOGFONT * pBigFont =  GetIoViewFont(ESFBig);
		CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(0,k);
		pCell->SetFont(pBigFont);
	}
*/
	// 设置第一行行高
	CClientDC dc(this);	
	CFont * pFontBig = GetIoViewFontObject(ESFBig);
	CFont* pOldFont = dc.SelectObject(pFontBig); // font为要得其高宽的字体	
	 
	TEXTMETRIC tmTime;
	dc.GetTextMetrics(&tmTime);
	int32 iRowHeight0 = tmTime.tmHeight + 2;  
	
	m_GridCtrl.SetRowHeight(0,iRowHeight0);
	dc.SelectObject(pOldFont);
	//
	bool32 bFound = false;
	CGridCellCheck * pFirstCell = (CGridCellCheck *)m_GridCtrl.GetCell(0,0);

	for (int32 iRow = 1; iRow < m_GridCtrl.GetRowCount(); iRow++)
	{
		UpdateSpecifyTableRow(iRow, false);
		
		CGridCellCheck * pCheckCell = (CGridCellCheck *)m_GridCtrl.GetCell(iRow,0);
		ASSERT(NULL != pCheckCell);
		
		CMerch * pMerch = (CMerch *)pCheckCell->GetData();
		ASSERT(NULL != pMerch);
		
		for (int32 i = 0;i<m_TableCheckInfo.GetSize();i++)
		{
			if (pMerch == m_TableCheckInfo[i].m_pMerch)
			{
				pCheckCell->SetCheck(true);
				pFirstCell->SetCheck(true);
				bFound = true;				
			}
		}		
	}	
	if (bFound)
	{
		pFirstCell->SetCheck(true);
	}
	else
	{
		pFirstCell->SetCheck(false);
	}
	// 刷新表格
	m_GridCtrl.Invalidate();
}

void CIoViewRelative::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	int32 iRow = pItem->iRow;

	if (iRow < m_GridCtrl.GetFixedRowCount() || iRow >= m_GridCtrl.GetRowCount())
		return;

	CGridCellSys *pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
	ASSERT(NULL != pCell);

	CMerch *pMerch = (CMerch *)pCell->GetData();
	ASSERT(NULL != pMerch);

	// 
	ASSERT(NULL != m_pAbsCenterManager);

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnViewMerchChanged(this, pMerch);
}

void CIoViewRelative::OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	int32 iCol = pItem->iColumn;
	int32 iWidth = pItem->iParam1;

	iCol -= 1;
	if (iCol < 0 && iCol >= m_GridCtrl.GetColumnCount())
		return;

	ASSERT(iWidth >= 0);
	if (iCol>=0)
	{
		CReportScheme::Instance()->SaveColumnWidth(ERTCustom,iCol,iWidth);
	}	
}

void CIoViewRelative::OnIoViewActive()
{
	if (NULL != m_GridCtrl.GetSafeHwnd())
	{
		m_GridCtrl.SetFocus();
	}

	SetChildFrameTitle();
}

void CIoViewRelative::OnIoViewDeactive()
{
}

const char * CIoViewRelative::GetXmlElementAttrCurTab()
{
	return KStrElementAttrCurTab;
}
const char * CIoViewRelative::GetXmlElementTabCheckInfo()
{
	return KStrElementTabCheckInfo;
}
const char * CIoViewRelative::GetXmlElementAttrChecked()
{
	return KStrElementChecked;
}

void CIoViewRelative::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;

	if (NULL != m_pMerchXml)
	{	
		StrTitle += L" ";
		StrTitle += m_pMerchXml->m_MerchInfo.m_StrMerchCnName;
	}
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}
