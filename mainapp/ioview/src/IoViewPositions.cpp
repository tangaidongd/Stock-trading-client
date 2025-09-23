#include "stdafx.h"
#include "memdc.h"
#include <math.h>
#include "MPIChildFrame.h"
#include "IoViewManager.h"
#include "MerchManager.h"
#include "GridCellSymbol.h"
#include "facescheme.h"
#include "ShareFun.h"
#include "ColorStep.h"
#include "PlugInStruct.h"
#include "IoViewPositions.h"
#include "PluginFuncRight.h"
#include "ConfigInfo.h"
#include "formulaengine.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////

static const int32 KTimerIdPushDataReq = 1002;
static const int32 KTimerPeriodPushDataReq = 1000*60*5;	// 请求数据

#define  FORMULAR_NAME  L"仓位统计"
#define  WORK_LINE		L"工作线"

/////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewPositions, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewPositions, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewPositions)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewPositions::CIoViewPositions()
:CIoViewBase()
{
	m_bActive			= false;
	m_RectWindows = CRect(0,0,0,0);
	m_iDataServiceType  = EDSTKLine;	// General为收到除权信号
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewPositions::~CIoViewPositions()
{
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint

void CIoViewPositions::OnPaint()
{
	CPaintDC dc(this); // device context for painting	
	
	if ( GetParentGGTongViewDragFlag() || m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}
	
	//
	UnLockRedraw();

	CRect rcClient;
	GetClientRect(rcClient);
	
	COLORREF clrBk = GetIoViewColor(ESCBackground);;

	dc.SetBkColor(clrBk);
	CMemDC dcMem(&dc, rcClient);

	dcMem->FillSolidRect(rcClient, clrBk);

	int iSaveDC = dcMem->SaveDC();
	dcMem->RestoreDC(iSaveDC);
}

void CIoViewPositions::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();

	Invalidate();
}

void CIoViewPositions::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{
	E_KLineTypeBase    eKLineType			    = EKTBDay;

	int32 iPosFound;
	CMerchKLineNode* pKLineRequest = NULL;
	pMerch->FindMerchKLineNode(eKLineType, iPosFound, pKLineRequest);


	pMerch->m_MerchKLineNodesPtr;
	// 根本找不到K线数据， 那就不需要显示了
	if (NULL != pKLineRequest && 0 != pKLineRequest->m_KLines.GetSize())	
	{
		int iIndex = pKLineRequest->m_KLines.GetSize();
		CArray<CKLine, CKLine> aKlineTemp;
		aKlineTemp.Add(pKLineRequest->m_KLines.GetAt(iIndex - 1));

		// 取仓位统计的工作线
		CFormularContent* pContent = CFormulaLib::instance()->GetFomular(FORMULAR_NAME);
		if ( NULL != pContent )
		{
			T_IndexOutArray* pOut = formula_index(pContent, pKLineRequest->m_KLines);
			if (pOut && pOut->iIndexNum > 0)
			{
				float fLast  = 0.0f;
				float fYLast = 0.0f;	
				float fMax   = 60.0f;
				for (int i = 0; i < pOut->iIndexNum; i ++ )
				{
					if (0 != pOut->index[i].StrName.CompareNoCase(WORK_LINE))
					{
						continue;
					}

					if (pOut->index[i].iPointNum > 0)
					{
						int32 iPointNum = pOut->index[i].iPointNum;
						int32 iShowCount = 0;

						for (int j = 0; j < iPointNum; j++)
						{
							float fY = pOut->index[i].pPoint[j];

							if ( j <= pOut->index[i].iPointNum -1 )
							{
								fYLast = fY;
							}
							else
							{
								fY = fYLast;
							}
						}
					}

					// 大于60的数字，统一按照60%显示
					if (fYLast > fMax)
					{
						m_Meter.UpdateNeedle(fMax);
					}
					else
					{
						m_Meter.UpdateNeedle(fYLast);
					}
				}
			}
		}			
	}
}

void CIoViewPositions::OnIoViewFontChanged()
{	
	CIoViewBase::OnIoViewFontChanged();
	// 字体调整 - 视图会变得不协调
	//RecalcRectSize();
	Invalidate();
}

void CIoViewPositions::LockRedraw()
{
	if ( m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
}

void CIoViewPositions::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
}

int CIoViewPositions::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	int iRet = CIoViewBase::OnCreate(lpCreateStruct);
	if ( -1 == iRet )
	{
		return -1;
	}

	//
	InitialIoViewFace(this);

	m_Meter.Create(L"",ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,CRect(100,100,100,100),this,  2222);
	m_Meter.SetValueDecimals(0);
	m_Meter.UpdateNeedle(0) ;
	m_Meter.ShowWindow(SW_SHOW);

	SetTimer(KTimerIdPushDataReq, KTimerPeriodPushDataReq, NULL);
	//
	return iRet;
}

void CIoViewPositions::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);
	
	GetClientRect(&m_RectWindows);
	m_Meter.MoveWindow(m_RectWindows);
	m_Meter.ReconstructControl();

	//RecalcRectSize();
}

// 通知视图改变关注的商品
void CIoViewPositions::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
	
	// 当前商品不为空的时候
	if (NULL != pMerch)
	{
		// 设置关注的商品信息
		m_aSmartAttendMerchs.RemoveAll();

		CSmartAttendMerch SmartAttendMerch;
		SmartAttendMerch.m_pMerch = pMerch;
		SmartAttendMerch.m_iDataServiceTypes = EDSTGeneral; //EDSTTimeSale | EDSTTick; // 关注的数据类型
		m_aSmartAttendMerchs.Add(SmartAttendMerch);
		
		// 
		OnVDataMerchTimeSalesUpdate(pMerch);	// 有就显示, 无则清空显示
	}
	else
	{
		// zhangbo 20090824 #待补充， 当心商品不存在时，清空当前显示数据
		//...
	}
	
	//InvalidateRect(m_RectTitle);	// 标题部分需要修改，其它无
}

//
void CIoViewPositions::OnVDataForceUpdate()
{
	RequestViewData();
}

bool32	CIoViewPositions::IsAttendData(IN CMerch *pMerch, E_DataServiceType eDataServiceType)
{
	if (eDataServiceType & m_iDataServiceType)
	{
		return TRUE;
	}

	return false;
}

void CIoViewPositions::OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch)
{
}

bool32 CIoViewPositions::FromXml(TiXmlElement * pElement)
{
	if (NULL == pElement)
		return false;

	// 判读是不是IoView的节点
	const char *pcValue = pElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;

	// 判断是不是描述自己这个业务视图的节点
	const char *pcAttrValue = pElement->Attribute(GetXmlElementAttrIoViewType());
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(CString(pcAttrValue)) != 0)	// 不是描述自己的业务节点
		return false;
	
	// 读取本业务视图特有的内容
	SetFontsFromXml(pElement);
	SetColorsFromXml(pElement);

	//
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
	
	// 商品发生改变
	OnVDataMerchChanged(iMarketId, StrMerchCode, pMerchFound);	

	OnVDataForceUpdate();

	return true;
}

CString CIoViewPositions::ToXml()
{
	CString StrThis;

	CString StrMarketId;
	StrMarketId.Format(L"%d", m_MerchXml.m_iMarketId);

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" ",/*>\n*/ 
		CString(GetXmlElementValue()), 
		CString(GetXmlElementAttrIoViewType()), 
		CIoViewManager::GetIoViewString(this),
		CString(GetXmlElementAttrMerchCode()),
		m_MerchXml.m_StrMerchCode,
		CString(GetXmlElementAttrMarketId()), 
		StrMarketId);
	//
	CString StrFace;
	StrFace  = SaveColorsToXml();
	StrFace += SaveFontsToXml();
	
	StrThis += StrFace;
	StrThis += L">\n";
	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

CString CIoViewPositions::GetDefaultXML()
{
	CString StrThis;

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" >\n", 
		CString(GetXmlElementValue()), 
		CString(GetXmlElementAttrIoViewType()), 
		CIoViewManager::GetIoViewString(this),
		CString(GetXmlElementAttrMerchCode()),
		L"",
		CString(GetXmlElementAttrMarketId()), 
		L"-1");

	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

void CIoViewPositions::OnIoViewActive()
{
	SetFocus();
	m_bActive = IsActiveInFrame();

	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	Invalidate();
}

void CIoViewPositions::OnIoViewDeactive()
{
	m_bActive = false;
	Invalidate();
}

void  CIoViewPositions::SetChildFrameTitle()
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

void CIoViewPositions::RequestViewData()
{
}

BOOL CIoViewPositions::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

void CIoViewPositions::OnVDataPluginResp( const CMmiCommBase *pResp )
{
}

void CIoViewPositions::OnTimer( UINT nIDEvent )
{
	if ( KTimerIdPushDataReq == nIDEvent )
	{
		RequestViewData();
	}

	CIoViewBase::OnTimer(nIDEvent);
}

