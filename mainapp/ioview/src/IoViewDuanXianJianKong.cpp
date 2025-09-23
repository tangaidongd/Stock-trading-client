#include "StdAfx.h"
#include "IoViewDuanXianJianKong.h"
#include "IoViewManager.h"
#include "DlgBlockSet.h"
#include "BlockManager.h"
#include "WspManager.h"


const int32 KUpdatePushMerchsTimerId			= 100005;					// 每隔 n 秒钟, 重新计算一次当前的推送商品
const int32 KTimerPeriodPushMerchs				= 1000 * 40;

const int32	KTimerIdCheckInitData				=	1001;					// 检查初始化数据是否已经请求到
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
	// 初始化列名
	if ( m_saHeaders.GetSize() <= 0 )
	{
		m_saHeaders.Add(_T("异动时间"));
		m_saHeaders.Add(_T("股票代码"));
		m_saHeaders.Add(_T("股票名称"));
		m_saHeaders.Add(_T("异动信号"));
		m_saHeaders.Add(_T("提示数值"));
		m_saHeaders.Add(_T("提示内容"));
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

	// 创建横滚动条
	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
	m_XSBVert.SetScrollRange(0, 10);

	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
	m_XSBHorz.SetScrollRange(0, 10);

	// 创建数据表格
	m_GridCtrl.Create(CRect(0, 0, 0, 0), this, ID_GRID_CTRL);
	m_GridCtrl.SetDefCellWidth(50);
	m_GridCtrl.EnableBlink(FALSE);			// 禁止闪烁
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	// 设置表格图标
	// m_ImageList.Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));

	// 设置相互之间的关联
	m_XSBHorz.SetOwner(&m_GridCtrl);
	m_XSBVert.SetOwner(&m_GridCtrl);
	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
	//m_XSBHorz.AddMsgListener(m_hWnd);	// 侦听滚动消息 - 记录滚动消息

	// 设置表头
	m_GridCtrl.SetHeaderSort(FALSE);

	// 显示表格线的风格
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

	// 判读是不是IoView的节点
	const char *pcValue = pElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;

	// 判断是不是描述自己这个业务视图的节点
	const char *pcAttrValue = pElement->Attribute(GetXmlElementAttrIoViewType());
	CString StrIoViewString = _A2W(pcAttrValue);
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(StrIoViewString) != 0)	// 不是描述自己的业务节点
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
//自选股相关
void CIoViewDuanXianJianKong::OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType)
{
	// 自选股更新的通知:
	// 只更新了商品
	if ( CSubjectUserBlock::EUBUMerch == eUpdateType )
	{	// 删除多余数据
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
	// 时间排序
	if ( NULL == pResp )
	{
		return;
	}

	// 不管什么数据，只要是没有的，全部插入 - 需要提高效率
	if ( pResp->m_eCommTypePlugIn == ECTPIRespShortMonitor )	// 历史数据
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

		//  清除多余数据
		while ( m_lstDuanXianJianKongSales.GetCount() > KIDefaultMaxMonitorCount )	// 总的数据量超标
		{
			T_DuanXianJianKong *pMTS = m_lstDuanXianJianKongSales.RemoveHead();
			delete pMTS;
		}

		if ( bUpdated )
		{
			UpdateTableAllContent();	// 显示数据

			RequestPushViewData(false);	// 请求推送
		}
	}
	else if ( pResp->m_eCommTypePlugIn == ECTPIRespAddPushShortMonitor )	// 推送数据
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

		//  清除多余数据
		while ( m_lstDuanXianJianKongSales.GetCount() > KIDefaultMaxMonitorCount )	// 总的数据量超标
		{
			T_DuanXianJianKong *pMTS = m_lstDuanXianJianKongSales.RemoveHead();
			delete pMTS;
		}

		if ( bUpdate )
		{
			UpdateTableAllContent();	// 显示数据
			m_GridCtrl.EnsureVisible(m_GridCtrl.GetRowCount() - 1, 0);
		}
	}
	else if ( pResp->m_eCommTypePlugIn == ECTPIRespDelPushShortMonitor )	// 推送数据
	{
		RefreshShortMonitorData();	
	}
}

bool32 CIoViewDuanXianJianKong::InsertNewShortTimeSale(CMerch *pMerch,  const T_RespShortMonitor &shortTM , bool32 bAddToGrid/* = false*/)
{
	// 按照时间序，找到对应的位置 - 慢慢找:)
	T_DuanXianJianKong *pDuanXianJianKong = new T_DuanXianJianKong;
	pDuanXianJianKong->m_pMerch = pMerch;
	pDuanXianJianKong->m_Shortonitor = shortTM;
	POSITION pos = m_lstDuanXianJianKongSales.GetHeadPosition();
	int i = 0;
	for (   ; pos != NULL ; m_lstDuanXianJianKongSales.GetNext(pos), i++ )
	{
		const T_DuanXianJianKong &mts = *m_lstDuanXianJianKongSales.GetAt(pos);

		if ( mts.m_Shortonitor.m_Time < shortTM.m_Time )		// 原来的数据比现在的小
		{
			continue;
		}
		else if ( mts.m_Shortonitor.m_Time == shortTM.m_Time )	// 等
		{
			if ( mts.m_pMerch == pMerch &&  mts.m_Shortonitor == shortTM)	// 重复数据不添加，时间相等则插入后面
			{
				DEL(pDuanXianJianKong);		// 释放				
				return false;		        // 相同的
			}
		}
		else	// 大于
		{
			// 如果数量已经最大，且是前面应当抛弃的数据则抛弃
			if ( 0 == i && m_lstDuanXianJianKongSales.GetCount() >= KIDefaultMaxMonitorCount )
			{
				DEL(pDuanXianJianKong);
				return false;
			}

			m_lstDuanXianJianKongSales.InsertBefore(pos, pDuanXianJianKong);
			if ( bAddToGrid /*&& !m_bInitialized*/ )
			{
				// 如果数据是一条一条过来，应当不出现前面数据的
				InsertGridRow(i, *pDuanXianJianKong);
			}
			return true;
		}
	}

	// 添加到最后
	m_lstDuanXianJianKongSales.AddTail(pDuanXianJianKong);
	if ( bAddToGrid )
	{
		InsertGridRow(i, *pDuanXianJianKong);
	}

	// 裁掉多余的数据
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
	// 根据当前的选择内容，在适当位置添加 - 应该只添加到结尾的
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

	// 计算表格最大行 - 粗略计算  - 需要插入足够的数据
	int32 iOldRowCount = m_iMaxGridVisibleRow;
	CClientDC dc(this);
	CFont *pFontOld = dc.SelectObject(GetIoViewFontObject(ESFNormal));
	CSize sizeText = dc.GetTextExtent(_T("行高度测试"));
	dc.SelectObject(pFontOld);
	m_iMaxGridVisibleRow = rcClient.Height() / sizeText.cy + 1;	// 多算了不知道多少个
	if ( m_iMaxGridVisibleRow < 0 )
	{
		m_iMaxGridVisibleRow = 0;
	}

	//if ( iOldRowCount < m_iMaxGridVisibleRow )
	//{
	//	RequestSortViewDataAsync();                    // 重新请求数据
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

	// 画标题
	CString StrTitle = L"短线监控";

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
	// 保存focus cell
	CCellID	cellFocus = m_GridCtrl.GetFocusCell();
	CCellRange cellRange = m_GridCtrl.GetSelectedCellRange();
	m_GridCtrl.DeleteNonFixedRows();	

	// 插入商品信息

	// 裁掉多余的数据
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
		m_GridCtrl.SetFocusCell(cellFocus); // 验证是否还要select
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

	// 检查不存在的自选股
	CString strBlockName = CUserBlockManager::Instance()->GetDefaultServerBlockName();
	T_Block* pBlock = CUserBlockManager::Instance()->GetBlock(strBlockName);
	if (NULL != pBlock)
	{
		// 查找不到的，就准备删除推送
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
		DoRequestViewData(reqDel);		// 删除推送会删除所有关注这个商品此类型的推送，可能其它视图需要~~
	}
}

void CIoViewDuanXianJianKong::RequestPushViewData(bool32 bForce/* = false*/)
{
	// 申请推送数据
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

		// 保留最后一次推送的值得
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
				reqmain.m_iCount = 10;	// 最多最近20个
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

	RequestPushViewData(true);		// 请求推送
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
		StrFlag = mts.m_Shortonitor.m_uBuy != 0 ? _T("500万以上卖出") : _T("500万以上买入");
		StrPrice.Format(_T("%0.3f万"), mts.m_Shortonitor.m_fPrice * mts.m_Shortonitor.m_fCounts / 10000.0);
		StrContent.Format(_T("成交额"));
		break;
	case ESMBigOrderVolume:
		clr = mts.m_Shortonitor.m_uBuy != 0 ? GetIoViewColor(ESCFall) :  GetIoViewColor(ESCRise);
		StrFlag = mts.m_Shortonitor.m_uBuy != 0 ? _T("大单卖出") : _T("大单买入");
		StrPrice.Format(_T("%0.0f股"), mts.m_Shortonitor.m_fCounts);
		StrContent.Format(_T("股数"));
		break;
	case ESMMin5Rate:
		clr = mts.m_Shortonitor.m_fIncRate >  EPSINON ?  GetIoViewColor(ESCRise) :  GetIoViewColor(ESCFall);
		StrFlag = mts.m_Shortonitor.m_fIncRate >  EPSINON ? _T("急速拉升") : _T("急速跳水");
		StrPrice.Format(_T("%0.0f%%"), mts.m_Shortonitor.m_fIncRate);
		StrContent.Format(_T("涨幅"));
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
				CNewTBWnd::DoOpenCfm(L"分时走势");
				CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
				if (NULL != pMainFrame)
				{					
					//	更新分时版面框架下面的所有图标视图商品
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
					//	更新当前框架下面的所有图标视图商品
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

	// 字体放大
	menu.AppendODMenu(L"字体放大 CTRL+↑", MF_STRING, IDM_IOVIEWBASE_FONT_BIGGER);

	// 字体缩小
	menu.AppendODMenu(L"字体缩小 CTRL+↓", MF_STRING, IDM_IOVIEWBASE_FONT_SMALLER);
	// 风格设置
	menu.AppendODMenu(L"风格设置", MF_STRING, ID_SETTING);
	menu.AppendODMenu(L"版面布局", MF_STRING, ID_LAYOUT_ADJUST);

	menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());
	menu.DestroyMenu();
}

void CIoViewDuanXianJianKong::OnTimer(UINT nIDEvent) 
{
	if ( nIDEvent == KUpdatePushMerchsTimerId )
	{
		RequestPushViewData(true);		// 不请求初始 - 定时强制申请推送，否则有可能丢失数据
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
