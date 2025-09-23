#include "stdafx.h"
#include "MPIChildFrame.h"
#include "IoViewManager.h"
#include "MerchManager.h"
#include "facescheme.h"
#include "ShareFun.h"
#include "IoViewF10.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

// 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CIoViewF10, CIoViewBase)


///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewF10, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewF10)
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
ON_WM_CREATE()
ON_WM_SIZE()
ON_MESSAGE(UM_IoViewTitle_Button_LButtonDown,OnMessageTitleButton)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewF10::CIoViewF10()
:CIoViewBase()
{
}


///////////////////////////////////////////////////////////////////////////////
// 
CIoViewF10::~CIoViewF10()
{
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CIoViewF10::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

///////////////////////////////////////////////////////////////////////////////
// Draw
void CIoViewF10::Draw()
{	

}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonDown
void CIoViewF10::OnLButtonDown(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonUp
void CIoViewF10::OnLButtonUp(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonUp(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnMouseMove
void CIoViewF10::OnMouseMove(UINT nFlags, CPoint point)
{	
	CStatic::OnMouseMove(nFlags, point);
}

int CIoViewF10::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CIoViewBase::OnCreate(lpCreateStruct);
	
	// TODO: Add your specialized creation code here
	if (!m_WebBrowser2.Create(L"", SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, CRect(0, 0, 0, 0), this, 1234))
		return -1;
	
	return 0;
}

void CIoViewF10::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);

	m_WebBrowser2.MoveWindow(m_rectClient);	
}

BOOL CIoViewF10::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return FALSE;
}

// 通知视图改变关注的商品
void CIoViewF10::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
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
		
		// 
		CString StrUrl;
		GetPrivateProfileStringGGTong(L"info_address", L"f10_url", L"", StrUrl, GetInfoAddressFilePath());

		if (StrUrl.GetLength() > 0)
		{
			CString StrUrlParam;
			StrUrlParam.Format(L"?bigmarketid=%d&symbolname=%s", pMerch->m_Market.m_Breed.m_iBreedId, pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer());

			StrUrl += StrUrlParam;

			m_WebBrowser2.Navigate(StrUrl, NULL, NULL, NULL, NULL);
		}
	}
	else
	{
		// zhangbo 20090824 #待补充， 当心商品不存在时，清空当前显示数据
		//...
	}
}

//
void CIoViewF10::OnVDataForceUpdate()
{
	
}

bool32 CIoViewF10::FromXml(TiXmlElement * pElement)
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
	if (m_pAbsCenterManager && !m_pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerchFound))
	{
		pMerchFound = NULL;
	}

	// 商品发生改变
	OnVDataMerchChanged(iMarketId, StrMerchCode, pMerchFound);		
	return true;
}

CString CIoViewF10::ToXml()
{
	CString StrThis;

	CString StrMarketId;
	StrMarketId.Format(L"%d", m_MerchXml.m_iMarketId);

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" >\n", 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(),
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrShowTabName()).GetBuffer(),
		m_StrTabShowName.GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(), 
		m_MerchXml.m_StrMerchCode.GetBuffer(),
		CString(GetXmlElementAttrMarketId()).GetBuffer(),
		StrMarketId.GetBuffer());

	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

CString CIoViewF10::GetDefaultXML()
{
	CString StrThis;

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" >\n", 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(), 
		L"",
		CString(GetXmlElementAttrMarketId()).GetBuffer(), 
		L"-1");

	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

void CIoViewF10::OnIoViewActive()
{
	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	
	if (NULL != m_WebBrowser2.GetSafeHwnd())
		m_WebBrowser2.SetFocus();
}

void CIoViewF10::OnIoViewDeactive()
{
}

void CIoViewF10::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
	
	if (NULL != m_pMerchXml)
	{
		StrTitle  += L" ";
		StrTitle  += m_pMerchXml->m_MerchInfo.m_StrMerchCnName;
	}
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

LRESULT CIoViewF10::OnMessageTitleButton(WPARAM wParam,LPARAM lParam)
{
	UINT uID = (UINT)wParam;
	if ( 0 == uID)
	{
		// 前进
		try
		{
			m_WebBrowser2.GoForward();
		}
		catch (CException* e)
		{
			e->GetRuntimeClass();
		}
		
	}
	else if ( 1 == uID)
	{
		// 后退
		try
		{
			m_WebBrowser2.GoBack();
		}
		catch (CException* e)
		{
			e->GetRuntimeClass();
		}
		
	}
	else if ( 2 == uID)
	{
		// 刷新
		try
		{
			m_WebBrowser2.Refresh();
		}
		catch (CException* e)
		{
			e->GetRuntimeClass();
		}
	}

	return 0;
}
