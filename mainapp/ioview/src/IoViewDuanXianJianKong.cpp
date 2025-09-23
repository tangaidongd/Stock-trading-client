#include "StdAfx.h"
#include "IoViewDuanXianJianKong.h"
#include "IoViewManager.h"
#include "DlgBlockSet.h"
#include "BlockManager.h"
#include "WspManager.h"


const int32 KUpdatePushMerchsTimerId			= 100005;					// ÿ�� n ����, ���¼���һ�ε�ǰ��������Ʒ
const int32 KTimerPeriodPushMerchs				= 1000 * 40;

const int32	KTimerIdCheckInitData				=	1001;					// ����ʼ�������Ƿ��Ѿ�����
const int32 KTimerPeriodCheckInitData			=   5000;								

const int32 KIDefaultMonitorCount				= 200;
const int32 KIDefaultMaxMonitorCount			= 350;

const float EPSINON = 0.00001; 


#define  ID_GRID_CTRL         12345


CIoViewDuanXianJianKong::HeaderArray CIoViewDuanXianJianKong::m_saHeaders;

IMPLEMENT_DYNCREATE(CIoViewDuanXianJianKong, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewDuanXianJianKong, CIoViewBase)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_NOTIFY(NM_RCLICK,ID_GRID_CTRL,OnGridRButtonDown)
	ON_NOTIFY(NM_CLICK, ID_GRID_CTRL, OnGridlClick)
	ON_NOTIFY(NM_DBLCLK, ID_GRID_CTRL, OnGridlDBClick)
	ON_MESSAGE_VOID(UM_DOINITIALIZE, OnDoInitialize)
END_MESSAGE_MAP()

CIoViewDuanXianJianKong::CIoViewDuanXianJianKong()
{
	// ��ʼ������
	if ( m_saHeaders.GetSize() <= 0 )
	{
		m_saHeaders.Add(_T("�춯ʱ��"));
		m_saHeaders.Add(_T("��Ʊ����"));
		m_saHeaders.Add(_T("��Ʊ����"));
		m_saHeaders.Add(_T("�춯�ź�"));
		m_saHeaders.Add(_T("��ʾ��ֵ"));
		m_saHeaders.Add(_T("��ʾ����"));
	}

	m_iMaxGridVisibleRow = 0;
	m_bInitialized = false;
	m_RectTitle.SetRectEmpty();
	m_multmapReqTheLastPushData.clear();
}

CIoViewDuanXianJianKong::~CIoViewDuanXianJianKong()
{

}

int CIoViewDuanXianJianKong::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	int iRet = CIoViewBase::OnCreate(lpCreateStruct);
	if(-1 == iRet)
	{
		return -1;
	}

	InitialIoViewFace(this);

	// �����������
	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
	m_XSBVert.SetScrollRange(0, 10);

	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
	m_XSBHorz.SetScrollRange(0, 10);

	// �������ݱ��
	m_GridCtrl.Create(CRect(0, 0, 0, 0), this, ID_GRID_CTRL);
	m_GridCtrl.SetDefCellWidth(50);
	m_GridCtrl.EnableBlink(FALSE);			// ��ֹ��˸
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	// ���ñ��ͼ��
	// m_ImageList.Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));

	// �����໥֮��Ĺ���
	m_XSBHorz.SetOwner(&m_GridCtrl);
	m_XSBVert.SetOwner(&m_GridCtrl);
	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
	//m_XSBHorz.AddMsgListener(m_hWnd);	// ����������Ϣ - ��¼������Ϣ

	// ���ñ�ͷ
	m_GridCtrl.SetHeaderSort(FALSE);

	// ��ʾ����ߵķ��
	m_GridCtrl.ShowGridLine(FALSE);
	m_GridCtrl.SetSingleRowSelection(TRUE);
	m_GridCtrl.SetShowSelectWhenLoseFocus(FALSE);

	PostMessage(UM_DOINITIALIZE, 0, 0);

	SetTimer(KUpdatePushMerchsTimerId, KTimerPeriodPushMerchs, NULL);
	SetTimer(KTimerIdCheckInitData, KTimerPeriodCheckInitData, NULL);

	return iRet;
}

void CIoViewDuanXianJianKong::OnDoInitialize()
{
	RequestInitialData();
	m_bInitialized = true;
}

void CIoViewDuanXianJianKong::SetGridHead()
{
	if (NULL == m_GridCtrl.GetSafeHwnd()) 
		return;

	int iCol = m_saHeaders.GetSize();

	if(iCol <= 0)
	{
		return;
	}

	m_GridCtrl.DeleteAllItems();
	m_GridCtrl.SetFixedRowCount(1);
	m_GridCtrl.SetRowCount(1);
	m_GridCtrl.SetFixedColumnCount(5);
	m_GridCtrl.SetColumnCount(iCol);
	m_GridCtrl.SetRowHeight(0, 30);
	for ( int32 i = 0 ; i < iCol; i++ )
	{
		CGridCellSys *pCell = NULL;

		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, i);
		pCell->SetText(m_saHeaders[i]);
		pCell->SetDefaultTextColor(ESCVolume);
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pCell->SetFont(GetColumnExLF());
		//pCell->SetCellRectDrawFlag(pCell->GetCellRectDrawFlag());
	}	

	m_GridCtrl.ExpandColumnsToFit();

	m_GridCtrl.Refresh();
	m_GridCtrl.Invalidate();
}

bool32 CIoViewDuanXianJianKong::FromXml( TiXmlElement *pElement )
{
	if (NULL == pElement)
		return false;

	// �ж��ǲ���IoView�Ľڵ�
	const char *pcValue = pElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;

	// �ж��ǲ��������Լ����ҵ����ͼ�Ľڵ�
	const char *pcAttrValue = pElement->Attribute(GetXmlElementAttrIoViewType());
	CString StrIoViewString = _A2W(pcAttrValue);
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(StrIoViewString) != 0)	// ���������Լ���ҵ��ڵ�
		return false;

	SetFontsFromXml(pElement);
	SetColorsFromXml(pElement);

	SetGridHead();
	RecalcLayout();

	return true;
}

CString CIoViewDuanXianJianKong::ToXml()
{
	CString StrThis;

	StrThis.Format(L"<%s %s=\"%s\" ",
		CString(GetXmlElementValue()), 
		CString(GetXmlElementAttrIoViewType()), 
		CIoViewManager::GetIoViewString(this)
		);

	CString StrFace;
	StrFace  = SaveColorsToXml();
	StrFace += SaveFontsToXml();

	StrThis += StrFace;
	StrThis += L">\n";

	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

CMerch *CIoViewDuanXianJianKong::GetMerchXml()
{
	return CIoViewBase::GetMerchXml();
}

void CIoViewDuanXianJianKong::OnVDataPluginResp( const CMmiCommBase *pResp )
{
	if ( NULL == pResp )
	{
		return;
	}

	if ( pResp->m_eCommType != ECTRespPlugIn )
	{
		return;
	}

	const CMmiCommBasePlugIn *pRespPlugin = (CMmiCommBasePlugIn *)pResp;
	if ( pRespPlugin->m_eCommTypePlugIn == ECTPIRespShortMonitor
		|| pRespPlugin->m_eCommTypePlugIn == ECTPIRespAddPushShortMonitor 
		|| pRespPlugin->m_eCommTypePlugIn == ECTPIRespDelPushShortMonitor)
	{
		OnShortTimeSaleResp((const CMmiRespShortMonitor *)pResp);
	}
}

////////////////////////////////////////////////////////////////////////// 
//��ѡ�����
void CIoViewDuanXianJianKong::OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType)
{
	// ��ѡ�ɸ��µ�֪ͨ:
	// ֻ��������Ʒ
	if ( CSubjectUserBlock::EUBUMerch == eUpdateType )
	{	// ɾ����������
		RequestDeletePushMerchNoExist();
		RequestInitialData();
	}
}

void CIoViewDuanXianJianKong::RefreshShortMonitorData ()
{
	m_lstDuanXianJianKongSales.RemoveAll();
	m_multmapReqTheLastPushData.clear();
	SetGridHead();
	RequestInitialData();
}	

void CIoViewDuanXianJianKong::OnShortTimeSaleResp( const CMmiRespShortMonitor *pResp )
{
	// ʱ������
	if ( NULL == pResp )
	{
		return;
	}

	// ����ʲô���ݣ�ֻҪ��û�еģ�ȫ������ - ��Ҫ���Ч��
	if ( pResp->m_eCommTypePlugIn == ECTPIRespShortMonitor )	// ��ʷ����
	{
		bool32 bUpdated = false;
		for ( mapShortMonitor::const_iterator it = pResp->m_mapShortMonitorData.begin() ; it != pResp->m_mapShortMonitorData.end(); it++ )
		{
			CMerch *pMerch;
			if ( !m_pAbsCenterManager->GetMerchManager().FindMerch(it->m_MerchKey.m_StrMerchCode, it->m_MerchKey.m_iMarketId,  pMerch) )
			{
				continue;
			}
			for ( ShortMonitorArray::const_iterator itSub=it->m_aShortMonitor.begin(); itSub != it->m_aShortMonitor.end(); itSub++ )
			{
				bUpdated = InsertNewShortTimeSale(pMerch, *itSub, false) || bUpdated;
			}
		}

		//  �����������
		while ( m_lstDuanXianJianKongSales.GetCount() > KIDefaultMaxMonitorCount )	// �ܵ�����������
		{
			T_DuanXianJianKong *pMTS = m_lstDuanXianJianKongSales.RemoveHead();
			delete pMTS;
		}

		if ( bUpdated )
		{
			UpdateTableAllContent();	// ��ʾ����

			RequestPushViewData(false);	// ��������
		}
	}
	else if ( pResp->m_eCommTypePlugIn == ECTPIRespAddPushShortMonitor )	// ��������
	{
		bool32 bUpdate = false;
		for ( mapShortMonitor::const_iterator it = pResp->m_mapShortMonitorData.begin() ; it != pResp->m_mapShortMonitorData.end(); it++ )
		{
			CMerch *pMerch;
			if ( !m_pAbsCenterManager->GetMerchManager().FindMerch(it->m_MerchKey.m_StrMerchCode, it->m_MerchKey.m_iMarketId,  pMerch) )
			{
				continue;
			}
			for ( ShortMonitorArray::const_iterator itSub=it->m_aShortMonitor.begin(); itSub != it->m_aShortMonitor.end(); itSub++ )
			{
				bUpdate = InsertNewShortTimeSale(pMerch, *itSub, false) || bUpdate;
			}
		}

		//  �����������
		while ( m_lstDuanXianJianKongSales.GetCount() > KIDefaultMaxMonitorCount )	// �ܵ�����������
		{
			T_DuanXianJianKong *pMTS = m_lstDuanXianJianKongSales.RemoveHead();
			delete pMTS;
		}

		if ( bUpdate )
		{
			UpdateTableAllContent();	// ��ʾ����
			m_GridCtrl.EnsureVisible(m_GridCtrl.GetRowCount() - 1, 0);
		}
	}
	else if ( pResp->m_eCommTypePlugIn == ECTPIRespDelPushShortMonitor )	// ��������
	{
		RefreshShortMonitorData();	
	}
}

bool32 CIoViewDuanXianJianKong::InsertNewShortTimeSale(CMerch *pMerch,  const T_RespShortMonitor &shortTM , bool32 bAddToGrid/* = false*/)
{
	// ����ʱ�����ҵ���Ӧ��λ�� - ������:)
	T_DuanXianJianKong *pDuanXianJianKong = new T_DuanXianJianKong;
	pDuanXianJianKong->m_pMerch = pMerch;
	pDuanXianJianKong->m_Shortonitor = shortTM;
	POSITION pos = m_lstDuanXianJianKongSales.GetHeadPosition();
	int i = 0;
	for (   ; pos != NULL ; m_lstDuanXianJianKongSales.GetNext(pos), i++ )
	{
		const T_DuanXianJianKong &mts = *m_lstDuanXianJianKongSales.GetAt(pos);

		if ( mts.m_Shortonitor.m_Time < shortTM.m_Time )		// ԭ�������ݱ����ڵ�С
		{
			continue;
		}
		else if ( mts.m_Shortonitor.m_Time == shortTM.m_Time )	// ��
		{
			if ( mts.m_pMerch == pMerch &&  mts.m_Shortonitor == shortTM)	// �ظ����ݲ���ӣ�ʱ�������������
			{
				DEL(pDuanXianJianKong);		// �ͷ�				
				return false;		        // ��ͬ��
			}
		}
		else	// ����
		{
			// ��������Ѿ��������ǰ��Ӧ������������������
			if ( 0 == i && m_lstDuanXianJianKongSales.GetCount() >= KIDefaultMaxMonitorCount )
			{
				DEL(pDuanXianJianKong);
				return false;
			}

			m_lstDuanXianJianKongSales.InsertBefore(pos, pDuanXianJianKong);
			if ( bAddToGrid /*&& !m_bInitialized*/ )
			{
				// ���������һ��һ��������Ӧ��������ǰ�����ݵ�
				InsertGridRow(i, *pDuanXianJianKong);
			}
			return true;
		}
	}

	// ��ӵ����
	m_lstDuanXianJianKongSales.AddTail(pDuanXianJianKong);
	if ( bAddToGrid )
	{
		InsertGridRow(i, *pDuanXianJianKong);
	}

	// �õ����������
	return true;
}

CIoViewDuanXianJianKong::T_DuanXianJianKong  * CIoViewDuanXianJianKong::CheckMTS( T_DuanXianJianKong *pMTS ) const
{
	POSITION pos = m_lstDuanXianJianKongSales.GetHeadPosition();
	while ( pos != NULL )
	{
		if ( pMTS == m_lstDuanXianJianKongSales.GetNext(pos) )
		{
			return pMTS;
		}
	}
	return NULL;
}

int32 CIoViewDuanXianJianKong::InsertGridRow( int /*iRow*/, const T_DuanXianJianKong &mts , bool32 bDrawRow/* =false*/ )
{
	// ���ݵ�ǰ��ѡ�����ݣ����ʵ�λ����� - Ӧ��ֻ��ӵ���β��
	int i = 0;
	for ( i=0; i < m_GridCtrl.GetRowCount() ; i++ )
	{
		T_DuanXianJianKong *pMTS = (T_DuanXianJianKong *)m_GridCtrl.GetItemData(i, 0);

		ASSERT( NULL != CheckMTS(pMTS) );
		if ( NULL != pMTS )
		{
			if ( pMTS->m_Shortonitor.m_Time > mts.m_Shortonitor.m_Time )
			{
				break;
			}
		}
	}

	if ( i >= m_GridCtrl.GetRowCount() )
	{
		i = -1;
	}

	int32 iNewRow = i;
	UpdateGridRow(iNewRow, mts);
	m_GridCtrl.EnsureVisible(m_GridCtrl.GetRowCount()-1, 0);

	if ( bDrawRow )
	{
		m_GridCtrl.RedrawRow(iNewRow);
	}

	return iNewRow;
}

void CIoViewDuanXianJianKong::RecalcLayout()
{
	CRect rcClient;
	GetClientRect(rcClient);

	// ����������� - ���Լ���  - ��Ҫ�����㹻������
	int32 iOldRowCount = m_iMaxGridVisibleRow;
	CClientDC dc(this);
	CFont *pFontOld = dc.SelectObject(GetIoViewFontObject(ESFNormal));
	CSize sizeText = dc.GetTextExtent(_T("�и߶Ȳ���"));
	dc.SelectObject(pFontOld);
	m_iMaxGridVisibleRow = rcClient.Height() / sizeText.cy + 1;	// �����˲�֪�����ٸ�
	if ( m_iMaxGridVisibleRow < 0 )
	{
		m_iMaxGridVisibleRow = 0;
	}

	//if ( iOldRowCount < m_iMaxGridVisibleRow )
	//{
	//	RequestSortViewDataAsync();                    // ������������
	//}

	m_RectTitle = rcClient;
	m_RectTitle.top = m_RectTitle.bottom - sizeText.cy - 8;
	rcClient.bottom = m_RectTitle.top;
	m_GridCtrl.MoveWindow(rcClient);
	m_GridCtrl.ExpandColumnsToFit(TRUE);
}

void CIoViewDuanXianJianKong::OnPaint()
{
	CPaintDC dc(this);

	int iSaveDC = dc.SaveDC();

	// ������
	CString StrTitle = L"���߼��";

	dc.SelectObject(GetIoViewFontObject(ESFNormal));
	dc.SetBkColor(GetIoViewColor(ESCBackground));
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(GetIoViewColor(ESCVolume));

	dc.FillSolidRect(m_RectTitle, RGB(53, 54, 57));
	dc.DrawText(StrTitle, m_RectTitle, DT_SINGLELINE |DT_VCENTER |DT_LEFT);

	dc.RestoreDC(iSaveDC);

	m_GridCtrl.RedrawWindow();
}

void CIoViewDuanXianJianKong::OnSize( UINT nType, int cx, int cy )
{
	CIoViewBase::OnSize(nType, cx, cy);
	RecalcLayout();
}

BOOL CIoViewDuanXianJianKong::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}


void CIoViewDuanXianJianKong::UpdateTableAllContent()
{
	// ����focus cell
	CCellID	cellFocus = m_GridCtrl.GetFocusCell();
	CCellRange cellRange = m_GridCtrl.GetSelectedCellRange();
	m_GridCtrl.DeleteNonFixedRows();	

	// ������Ʒ��Ϣ

	// �õ����������
	while ( m_lstDuanXianJianKongSales.GetCount() > KIDefaultMaxMonitorCount )
	{
		delete m_lstDuanXianJianKongSales.RemoveHead();
	}

	m_GridCtrl.InsertRowBatchBegin();

	int irow  =  m_GridCtrl.GetFixedRowCount();
	m_GridCtrl.SetRowCount(m_lstDuanXianJianKongSales.GetSize() + irow);
	irow  =  m_GridCtrl.GetRowCount();

	POSITION pos = m_lstDuanXianJianKongSales.GetHeadPosition();
	int32 iRowCount = 0;
	while ( pos != NULL )
	{
		T_DuanXianJianKong *pMTS = m_lstDuanXianJianKongSales.GetNext(pos);
		ASSERT( NULL != pMTS );

		UpdateGridRow(iRowCount + 1, *pMTS);
		iRowCount++;
	}

	m_GridCtrl.InsertRowBatchEnd();

	if ( m_GridCtrl.IsValid(cellFocus) )
	{
		m_GridCtrl.SetFocusCell(cellFocus); // ��֤�Ƿ�Ҫselect
	}
	if ( m_GridCtrl.IsValid(cellRange) )
	{
		m_GridCtrl.SetSelectedRange(cellRange);
	}

	if ( m_GridCtrl.GetRowCount() > 0 )
	{
		m_GridCtrl.EnsureVisible(m_GridCtrl.GetRowCount()-1, 0);
	}

	m_GridCtrl.Refresh();
}

void CIoViewDuanXianJianKong::RequestDeletePushMerchNoExist()
{
	CMmiReqRemovePushPlugInMerchData reqDel(ECTPIReqDeletePushMerchData);
	reqDel.m_eType = ECSTShortMonitor;
	reqDel.m_aMerchs.RemoveAll();

	// ��鲻���ڵ���ѡ��
	CString strBlockName = CUserBlockManager::Instance()->GetDefaultServerBlockName();
	T_Block* pBlock = CUserBlockManager::Instance()->GetBlock(strBlockName);
	if (NULL != pBlock)
	{
		// ���Ҳ����ģ���׼��ɾ������
		for (int i = 0; i < pBlock->m_aMerchs.GetSize(); ++i)
		{
			CMerchKey cMerchkey;
			cMerchkey.m_iMarketId = pBlock->m_aMerchs.GetAt(i)->m_MerchInfo.m_iMarketId;
			cMerchkey.m_StrMerchCode = pBlock->m_aMerchs.GetAt(i)->m_MerchInfo.m_StrMerchCode;
			
			multimap<CMerchKey, int>::iterator iter = m_multmapReqTheLastPushData.find(cMerchkey);
			if (iter != m_multmapReqTheLastPushData.end())
			{
				m_multmapReqTheLastPushData.erase(iter);
				continue;
			}
		}	

		multimap<CMerchKey, int>::iterator iter;
		for (iter = m_multmapReqTheLastPushData.begin() ;  iter != m_multmapReqTheLastPushData.end(); ++iter)
		{
			CMerchKey cMerchkey;
			cMerchkey.m_iMarketId = iter->first.m_iMarketId;
			cMerchkey.m_StrMerchCode = iter->first.m_StrMerchCode;
			reqDel.m_aMerchs.Add(cMerchkey);
		}
	}

	if ( reqDel.m_aMerchs.GetCount() > 0 )
	{
		DoRequestViewData(reqDel);		// ɾ�����ͻ�ɾ�����й�ע�����Ʒ�����͵����ͣ�����������ͼ��Ҫ~~
	}
}

void CIoViewDuanXianJianKong::RequestPushViewData(bool32 bForce/* = false*/)
{
	// ������������
	CString strBlockName = CUserBlockManager::Instance()->GetDefaultServerBlockName();
	T_Block* pBlock = CUserBlockManager::Instance()->GetBlock(strBlockName);
	if (NULL != pBlock)
	{
		CMmiReqPushPlugInMerchData req;
		req.m_StrMerchCode.Empty();
		req.m_aReqMore.RemoveAll();

		for (int i = 0; i < pBlock->m_aMerchs.GetCount(); ++i)
		{
			if (req.m_StrMerchCode.IsEmpty())
			{
				req.m_iMarketID = pBlock->m_aMerchs.GetAt(i)->m_MerchInfo.m_iMarketId;
				req.m_StrMerchCode = pBlock->m_aMerchs.GetAt(i)->m_MerchInfo.m_StrMerchCode;
			}
			else
			{
				T_ReqPushMerchData PushData;
				PushData.m_iMarket = pBlock->m_aMerchs.GetAt(i)->m_MerchInfo.m_iMarketId;
				PushData.m_StrCode = pBlock->m_aMerchs.GetAt(i)->m_MerchInfo.m_StrMerchCode;
				PushData.m_uType   = ECSTMainMonitor;
				req.m_aReqMore.Add(PushData);
			}
		}

		if (pBlock->m_aMerchs.GetCount() > 0)
		{
			req.m_eCommTypePlugIn = ECTPIReqAddPushShortMonitor;
			DoRequestViewData(req, true);
		}

		// �������һ�����͵�ֵ��
		m_multmapReqTheLastPushData.clear();
		for (int i = 0; i < pBlock->m_aMerchs.GetCount(); ++i)
		{
			CMerchKey key;
			key.m_iMarketId = pBlock->m_aMerchs.GetAt(i)->m_MerchInfo.m_iMarketId;
			key.m_StrMerchCode = pBlock->m_aMerchs.GetAt(i)->m_MerchInfo.m_StrMerchCode;
			m_multmapReqTheLastPushData.insert(pair<CMerchKey, int> (key, 0));
		}	
	}
}

void CIoViewDuanXianJianKong::RequestInitialData()
{
	CString strBlockName = CUserBlockManager::Instance()->GetDefaultServerBlockName();
	T_Block* pBlock = CUserBlockManager::Instance()->GetBlock(strBlockName);
	if (NULL != pBlock)
	{
		CMmiReqShortMonitor reqmain;
		reqmain.m_StrMerchCode.Empty();
		reqmain.m_aReqMore.RemoveAll();

		int iSize = pBlock->m_aMerchs.GetCount();
		for (int i = 0; i < pBlock->m_aMerchs.GetCount(); ++i)
		{
			if (reqmain.m_StrMerchCode.IsEmpty())
			{
				reqmain.m_iCount = 10;	// ������20��
				reqmain.m_StrMerchCode = pBlock->m_aMerchs.GetAt(i)->m_MerchInfo.m_StrMerchCode;
				reqmain.m_iBlockId     = pBlock->m_aMerchs.GetAt(i)->m_MerchInfo.m_iMarketId;
			}
			else
			{
				T_ReqMainMonitor stReqMainMonitor;
				stReqMainMonitor.m_iBlockId = pBlock->m_aMerchs.GetAt(i)->m_MerchInfo.m_iMarketId;
				stReqMainMonitor.m_StrMerchCode = pBlock->m_aMerchs.GetAt(i)->m_MerchInfo.m_StrMerchCode;
				stReqMainMonitor.m_iCount = 10;
				reqmain.m_aReqMore.Add(stReqMainMonitor);
			}	
		}

		if (pBlock->m_aMerchs.GetCount() > 0)
		{
			DoRequestViewData(reqmain, true);
		}
	}

	RequestPushViewData(true);		// ��������
}

void CIoViewDuanXianJianKong::UpdateGridRow( int iRow, const T_DuanXianJianKong &mts )
{
	if ( !m_GridCtrl.IsValid(iRow, 0) )
	{
		return;
	}

	LOGFONT *pFontSmall = CFaceScheme::Instance()->GetSysFont(ESFNormal);

	tm t = {0};
	CGmtTime	GmtTime(mts.m_Shortonitor.m_Time);
	GmtTime.GetLocalTm(&t);
	CString StrTime;
	COLORREF clr;
	COLORREF clrText = COLORNOTCOUSTOM;

	int iCol = 0;
	StrTime.Format(_T("%02d:%02d"), t.tm_hour, t.tm_min);
	m_GridCtrl.SetItemText(iRow, iCol, StrTime);
	m_GridCtrl.SetItemFont(iRow, iCol, pFontSmall);
	m_GridCtrl.SetItemFgColour(iRow, iCol, clrText);
	CGridCellBase* pCellbase = m_GridCtrl.GetCell(iRow, iCol);
	ASSERT(pCellbase);
	if (NULL != pCellbase)
	{
		pCellbase->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCellbase->SetFont(GetIndexRowExLF());
	}

	iCol++;
	m_GridCtrl.SetItemText(iRow, iCol, mts.m_pMerch->m_MerchInfo.m_StrMerchCode);
	m_GridCtrl.SetItemData(iRow, iCol, (LPARAM)mts.m_pMerch);
	m_GridCtrl.SetItemFont(iRow, iCol, pFontSmall);
	m_GridCtrl.SetItemFgColour(iRow, iCol, clrText);
	pCellbase = m_GridCtrl.GetCell(iRow, iCol);
	ASSERT(pCellbase);
	if (NULL != pCellbase)
	{	
		pCellbase->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCellbase->SetFont(GetCodeRowExLF());
		m_GridCtrl.SetItemFgColour(iRow, iCol, RGB(0xe2, 0xe2, 0xe2));
	}

	iCol++;
	m_GridCtrl.SetItemText(iRow, iCol, mts.m_pMerch->m_MerchInfo.m_StrMerchCnName);
	m_GridCtrl.SetItemFont(iRow, iCol, pFontSmall);
	m_GridCtrl.SetItemFgColour(iRow, iCol, clrText);
	pCellbase = m_GridCtrl.GetCell(iRow, iCol);
	ASSERT(pCellbase);
	if (NULL != pCellbase)
	{
		pCellbase->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCellbase->SetFont(GetChineseRowExLF());
		m_GridCtrl.SetItemFgColour(iRow, iCol, RGB(0xff, 0xff, 0xff));
	}


	CString StrFlag,StrPrice,StrContent;
	CRealtimePrice RealTime;
	switch(mts.m_Shortonitor.m_uMonitorType)
	{
	case ESMBigOrderAmount:
		clr = mts.m_Shortonitor.m_uBuy != 0 ? GetIoViewColor(ESCFall) :  GetIoViewColor(ESCRise);
		StrFlag = mts.m_Shortonitor.m_uBuy != 0 ? _T("500����������") : _T("500����������");
		StrPrice.Format(_T("%0.3f��"), mts.m_Shortonitor.m_fPrice * mts.m_Shortonitor.m_fCounts / 10000.0);
		StrContent.Format(_T("�ɽ���"));
		break;
	case ESMBigOrderVolume:
		clr = mts.m_Shortonitor.m_uBuy != 0 ? GetIoViewColor(ESCFall) :  GetIoViewColor(ESCRise);
		StrFlag = mts.m_Shortonitor.m_uBuy != 0 ? _T("������") : _T("������");
		StrPrice.Format(_T("%0.0f��"), mts.m_Shortonitor.m_fCounts);
		StrContent.Format(_T("����"));
		break;
	case ESMMin5Rate:
		clr = mts.m_Shortonitor.m_fIncRate >  EPSINON ?  GetIoViewColor(ESCRise) :  GetIoViewColor(ESCFall);
		StrFlag = mts.m_Shortonitor.m_fIncRate >  EPSINON ? _T("��������") : _T("������ˮ");
		StrPrice.Format(_T("%0.0f%%"), mts.m_Shortonitor.m_fIncRate);
		StrContent.Format(_T("�Ƿ�"));
		break;
	default:
		break;
	}

	iCol++;
	m_GridCtrl.SetItemText(iRow, iCol, StrFlag);
	m_GridCtrl.SetItemFont(iRow, iCol, pFontSmall);
	m_GridCtrl.SetItemFgColour(iRow, iCol, clr);
	pCellbase = m_GridCtrl.GetCell(iRow, iCol);
	ASSERT(pCellbase);
	if (NULL != pCellbase)
	{
		pCellbase->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCellbase->SetFont(GetDigitRowExLF(false));
	}

	iCol++;
	m_GridCtrl.SetItemText(iRow, iCol, StrPrice);
	m_GridCtrl.SetItemFont(iRow, iCol, pFontSmall);
	m_GridCtrl.SetItemFgColour(iRow, iCol, clr);
	pCellbase = m_GridCtrl.GetCell(iRow, iCol);
	ASSERT(pCellbase);
	if (NULL != pCellbase)
	{
		pCellbase->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCellbase->SetFont(GetDigitRowExLF(false));
	}

	iCol++;
	m_GridCtrl.SetItemText(iRow, iCol, StrContent);
	m_GridCtrl.SetItemFont(iRow, iCol, pFontSmall);
	m_GridCtrl.SetItemFgColour(iRow, iCol, clrText);	
	m_GridCtrl.SetItemData(iRow, 0, (LPARAM)&mts);
	pCellbase = m_GridCtrl.GetCell(iRow, iCol);
	ASSERT(pCellbase);
	if (NULL != pCellbase)
	{
		pCellbase->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCellbase->SetFont(GetChineseRowExLF(false));
		m_GridCtrl.SetItemFgColour(iRow, iCol, RGB(0xff, 0xff, 0xff));
	}
}
void CIoViewDuanXianJianKong::OnGridlDBClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW	*pGridView = (NM_GRIDVIEW *)pNotifyStruct;
	if(NULL != pGridView )
	{
		if ( pGridView->iRow >= m_GridCtrl.GetFixedRowCount() && 2 == pGridView->iColumn  )
		{			
			CMerch* pMerch = (CMerch*)m_GridCtrl.GetItemData(pGridView->iRow, 1);
			ASSERT(NULL != pMerch);
			if (NULL != pMerch)
			{
				CNewTBWnd::DoOpenCfm(L"��ʱ����");
				CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
				if (NULL != pMainFrame)
				{					
					//	���·�ʱ���������������ͼ����ͼ��Ʒ
					CArray<CIoViewBase *, CIoViewBase *> arr;
					CMPIChildFrame * pActiveChildFrame = (CMPIChildFrame *)pMainFrame->GetActiveFrame();
					pMainFrame->GetFrameSortedChartIoViews(arr, pActiveChildFrame, GetIoViewGroupId());
					for (int i =0; i < arr.GetCount(); ++i)
					{
						CIoViewBase* pBase = arr[i];
						if (NULL != pBase)
						{
							pBase->OnVDataMerchChanged(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, pMerch);
							pBase->ForceUpdateVisibleIoView();
						}
					}					
				}
			}
		}
	}
}
void CIoViewDuanXianJianKong::OnGridlClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW	*pGridView = (NM_GRIDVIEW *)pNotifyStruct;
	if(NULL != pGridView )
	{
		if ( pGridView->iRow >= m_GridCtrl.GetFixedRowCount()  )
		{			
			CMerch* pMerch = (CMerch*)m_GridCtrl.GetItemData(pGridView->iRow, 1);
			ASSERT(NULL != pMerch);
			if (NULL != pMerch)
			{						
				CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
				if (NULL != pMainFrame)
				{					
					//	���µ�ǰ������������ͼ����ͼ��Ʒ
					CArray<CIoViewBase *, CIoViewBase *> arr;
					CMPIChildFrame * pActiveChildFrame = (CMPIChildFrame *)pMainFrame->GetActiveFrame();
					pMainFrame->GetFrameSortedChartIoViews(arr, pActiveChildFrame, GetIoViewGroupId());
					for (int i =0; i < arr.GetCount(); ++i)
					{
						CIoViewBase* pBase = arr[i];
						if (NULL != pBase)
						{
							pBase->OnVDataMerchChanged(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, pMerch);
							pBase->ForceUpdateVisibleIoView();
						}
					}					
				}
				
			}
		}
	}
}

void CIoViewDuanXianJianKong::OnGridRButtonDown( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	CNewMenu menu;
	menu.CreatePopupMenu();

	CPoint pt;
	GetCursorPos(&pt);

	// ����Ŵ�
	menu.AppendODMenu(L"����Ŵ� CTRL+��", MF_STRING, IDM_IOVIEWBASE_FONT_BIGGER);

	// ������С
	menu.AppendODMenu(L"������С CTRL+��", MF_STRING, IDM_IOVIEWBASE_FONT_SMALLER);
	// �������
	menu.AppendODMenu(L"�������", MF_STRING, ID_SETTING);
	menu.AppendODMenu(L"���沼��", MF_STRING, ID_LAYOUT_ADJUST);

	menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());
	menu.DestroyMenu();
}

void CIoViewDuanXianJianKong::OnTimer(UINT nIDEvent) 
{
	if ( nIDEvent == KUpdatePushMerchsTimerId )
	{
		RequestPushViewData(true);		// �������ʼ - ��ʱǿ���������ͣ������п��ܶ�ʧ����
	}
	else if ( KTimerIdCheckInitData == nIDEvent )
	{
		if ( m_lstDuanXianJianKongSales.GetCount() < 100 )
		{
			RequestInitialData();
		}
		else
		{
			KillTimer(KTimerIdCheckInitData);
		}
	}

	CIoViewBase::OnTimer(nIDEvent);
}
