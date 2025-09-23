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

#include "IoViewSuggestPosition.h"

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
static const int32 KShowTimeSaleCount		 = 10000;
static const int32 KNormalShowTimeSaleCount = 100;

static const int32 KChartCycleDiameter = 150;		// ��ͼֱ��������

static const int32 KTimerIdPushDataReq = 1002;
static const int32 KTimerPeriodPushDataReq = 1000*60*5;	// ��������


#define  FORMULAR_SUGGEST  L"�����λ"
#define  DATA_LINE		L"��λ"

#define  TITLE_TOP		50;

// #define  PAGE_STR_TITLE				 "��������"
// #define  PAGE_STR_STRATEGY_TITLE    "�Ƽ����ԣ�"
// #define  PAGE_STR_STRATEGY			"��ս����"

#define  RGB_COLOR_RED		RGB(0xff,0x38,0x37)
#define  RGB_COLOR_YELLOW	RGB(0xb9,0xfc,0x00)



/////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewSuggestPosition, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewSuggestPosition, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewSuggestPosition)
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
CIoViewSuggestPosition::CIoViewSuggestPosition()
:CIoViewBase()
{
	m_bActive			= false;
	m_RectWindows = CRect(0,0,0,0);
	m_iDataServiceType  = EDSTKLine;	// GeneralΪ�յ���Ȩ�ź�
	m_pDrawImage = NULL;
	m_pImagePercentEight = NULL;
	m_pImagePercentFive = NULL;
	m_pImagePercentThree = NULL;

	m_Font14.CreateFont(14, 0,0,0,FW_NORMAL, 0,0,0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"����");

}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewSuggestPosition::~CIoViewSuggestPosition()
{
	DEL(m_pImagePercentEight);
	DEL(m_pImagePercentFive);
	DEL(m_pImagePercentThree);

	m_Font14.DeleteObject();
}


///////////////////////////////////////////////////////////////////////////////
// OnPaint

void CIoViewSuggestPosition::OnPaint()
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

	COLORREF clrBk = GetIoViewColor(ESCBackground);
	COLORREF clrVol = GetIoViewColor(ESCVolume);
	COLORREF clrText = GetIoViewColor(ESCText);;
	COLORREF clrTitle = RGB_COLOR_RED;
	COLORREF clrStrategy = RGB_COLOR_YELLOW;


	dc.SetBkColor(clrBk);
	CMemDC dcMem(&dc, rcClient);

	CBitmap MemBitmap;
	MemBitmap.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height()); 
	CBitmap *pOldBit=dcMem.SelectObject(&MemBitmap);  

	// �����־
	dcMem->FillSolidRect(rcClient.left + 1, rcClient.top + 1, rcClient.right - 1, rcClient.bottom -1, clrBk);
	int iSaveDC = dcMem->SaveDC();
	if ( m_bActive )
	{
		//dcMem->FillSolidRect(3, 3, 2, 2, clrVol);		
	}
	dcMem->SetBkMode(TRANSPARENT);
	//dcMem->SetBkColor(clrBk);
	dcMem->FillSolidRect(rcClient, clrBk);

	//DrawCorner(dcMem, rcClient);

	//// ����
	//CBrush cbr;
	//cbr.CreateStockObject(NULL_BRUSH); //����һ���ջ�ˢ
	//CBrush *pOldBr = dcMem->SelectObject(&cbr);
	//CPen cpen;
	//cpen.CreatePen(PS_SOLID, 1, RGB(200, 200, 200)); //����һ����ɫ�Ļ���
	//CPen *pOldPen = dcMem->SelectObject(&cpen);
	//dcMem->RoundRect(rcClient, CPoint(10, 10));
	//dcMem->SelectObject(pOldPen);
	//cbr.DeleteObject();
	//dcMem->SelectObject(pOldBr);
	//cbr.DeleteObject();


	CFont *pFontNormal = GetIoViewFontObject(ESFNormal);
	CFont *pFontSmall  = GetIoViewFontObject(ESFSmall);
	// ��Ȩ�޲�����
	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncCapitalFlow, false) )
	{
		dcMem->SelectObject(pFontSmall);
		dcMem->SetTextColor(clrText);
		CString StrRight = CPluginFuncRight::Instance().GetUserRightName(CPluginFuncRight::FuncCapitalFlow);
		CString StrShow = _T("���߱�[")+StrRight+_T("]Ȩ��, ") + CConfigInfo::Instance()->GetPrompt();
		CRect rcDraw(rcClient);
		rcDraw.top = rcClient.CenterPoint().y;
		rcDraw.top -= 15;
		rcDraw.InflateRect(-2, 0);
		dcMem->DrawText(StrShow, rcDraw, DT_CENTER |DT_WORDBREAK );
	}
	else
	{
		// ����
		//if ( !m_RectTitle.IsRectEmpty() )
		//{
		//	CString StrTitle = CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
		//	CFont *pOldFont = dcMem->SelectObject(&m_Font14);
		//	dcMem->SetTextColor(clrTitle);
		//	CRect rcTmp(0,0,0,0);
		//	//int32 iHeight = dcMem->DrawText(StrTitle, rcTmp, DT_SINGLELINE |DT_CALCRECT |DT_VCENTER |DT_LEFT);
		//	//CRect rcTitle(m_RectTitle);
		//	dcMem->DrawText(StrTitle, m_RectTitle, DT_SINGLELINE |DT_VCENTER |DT_LEFT);
		//	//rcTitle.left += rcTmp.Width();
		//	dcMem->SelectObject(pOldFont);
		//}

		// ͼƬ

		if (m_pDrawImage)
		{
			Graphics GraphicImage(dcMem->GetSafeHdc());
			DrawImage(GraphicImage, m_pDrawImage, m_RectImage, 1, 0, false);
		}    
	}

	dc.BitBlt(0,0, rcClient.Width(),rcClient.Height(),&dcMem,0,0,SRCCOPY);  
	//��ͼ��ɺ������  
	MemBitmap.DeleteObject();  
	dcMem->RestoreDC(iSaveDC);

}


void CIoViewSuggestPosition::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();

	Invalidate();
}



void CIoViewSuggestPosition::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{
	if(NULL == pMerch)
	{
		return;
	}

	// ���ڵ�ǰ��Ʒ�Ÿ���K�ߵ�����
	if (pMerch == m_pMerchXml)
	{
		E_KLineTypeBase    eKLineType			    = EKTBDay;
		int32 iPosFound;
		CMerchKLineNode* pKLineRequest = NULL;
		pMerch->FindMerchKLineNode(eKLineType, iPosFound, pKLineRequest);

		pMerch->m_MerchKLineNodesPtr;
		// �����Ҳ���K�����ݣ� �ǾͲ���Ҫ��ʾ��
		if (NULL != pKLineRequest && 0 != pKLineRequest->m_KLines.GetSize())	
		{
			int iIndex = pKLineRequest->m_KLines.GetSize();
			CArray<CKLine, CKLine> aKlineTemp;
			aKlineTemp.Add(pKLineRequest->m_KLines.GetAt(iIndex - 1));
			CFormularContent* pContent = CFormulaLib::instance()->GetFomular(FORMULAR_SUGGEST);
			if ( NULL != pContent )
			{
				T_IndexOutArray* pOut = formula_index(pContent, pKLineRequest->m_KLines);
				if (pOut && pOut->iIndexNum > 0)
				{
					float fYLast = 0.0f;	
					for (int i = 0; i < pOut->iIndexNum; i ++ )
					{
						if (0 != pOut->index[i].StrName.CompareNoCase(DATA_LINE))
						{
							continue;
						}

						// ȡ���һ����Чֵ
						if (pOut->index[i].iPointNum > 0)
						{
							int32 iPointNum = pOut->index[i].iPointNum;
							for (int j = iPointNum - 1; j >= 0; j--)
							{
								float fY = pOut->index[i].pPoint[j];
								if (fY != FLT_MAX &&  fY > 1e-7)
								{
									fYLast = pOut->index[i].pPoint[j];
									break;
								}
							}

							if (fYLast > 1e-1 && fYLast <= 30.0)
							{
								m_pDrawImage = m_pImagePercentThree;
							}
							else if (fYLast > 30.0 && fYLast <= 50.0)
							{
								m_pDrawImage = m_pImagePercentFive;
							}
							else if (fYLast > 50.0 && fYLast <= 100.0)
							{
								m_pDrawImage = m_pImagePercentEight;
							}
							else
							{
								// �쳣���Ҳ��80��
								m_pDrawImage = m_pImagePercentEight;
							}
						}
					}

					// ��ʵû��Ҫ��ˢ��
					SetSize();
					Invalidate();
					UpdateWindow();
				}
			}			
		}
	}
}



void CIoViewSuggestPosition::OnIoViewFontChanged()
{	
	CIoViewBase::OnIoViewFontChanged();
	// ������� - ��ͼ���ò�Э��
	//RecalcRectSize();
	Invalidate();
}

void CIoViewSuggestPosition::LockRedraw()
{
	if ( m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
}

void CIoViewSuggestPosition::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
}

int CIoViewSuggestPosition::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	int iRet = CIoViewBase::OnCreate(lpCreateStruct);
	if ( -1 == iRet )
	{
		return -1;
	}

	//
	InitialIoViewFace(this);

	/*m_Meter.Create(L"",ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,CRect(100,100,100,100),this,  2222);
	m_Meter.SetValueDecimals(0);
	m_Meter.UpdateNeedle(0) ;
	m_Meter.ShowWindow(SW_SHOW);*/

	m_pImagePercentEight = Image::FromFile(_T("image//TrendAnalysis//PercentEight.png"));
	m_pImagePercentFive = Image::FromFile(_T("image//TrendAnalysis//PercentFive.png"));
	m_pImagePercentThree = Image::FromFile(_T("image//TrendAnalysis//PercentThree.png"));


	SetTimer(KTimerIdPushDataReq, KTimerPeriodPushDataReq, NULL);
	//
	return iRet;
}





void CIoViewSuggestPosition::SetSize() 
{
	CRect rcWindows;
	GetClientRect(&rcWindows);

	m_RectTitle = rcWindows;
	m_RectTitle.InflateRect(-5, -5);	// ��϶


	CClientDC dc(this);

	CString StrTitleWidth   = CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;;
// 	CString StrStrategyTitleWidth = PAGE_STR_STRATEGY_TITLE;
// 	CString StrStarategyWidth   = PAGE_STR_STRATEGY;

	/*if (dc.GetTextExtent(StrTitleWidth + StrStrategyTitleWidth + StrStarategyWidth).cx >  rcWindows.Width())
	{
		m_RectTitle = NULL;
	}*/

	// ��ʾ��������
	CSize sizeWidth = dc.GetTextExtent(_T("��"));

	m_RectTitle.right = m_RectTitle.left + sizeWidth.cx* StrTitleWidth.GetLength() *1.2;	// �������壬�̶������
	m_RectTitle.bottom = m_RectTitle.top + sizeWidth.cy + 2*4;



	// ���������ʼλ��
	// ������ʾ���̳������Σ��������
	if (!m_pImagePercentEight)
	{
		return;
	}

	rcWindows.top  +=  5;
	int iLengthDif= rcWindows.Height() - rcWindows.Width();
	if (iLengthDif > 0)
	{
		rcWindows.top     += iLengthDif/2;
		rcWindows.bottom  -= iLengthDif/2;
	}
	else if (iLengthDif < 0)
	{
		iLengthDif = abs(iLengthDif);
		rcWindows.left   += iLengthDif/2;
		rcWindows.right  -= iLengthDif/2;
	}


	int iStartWidth =  m_pImagePercentEight->GetWidth();
	int iStartHeight =  m_pImagePercentEight->GetHeight();
	if (rcWindows.Width() < iStartWidth)
	{
		iStartWidth = rcWindows.Width();
	}
	if (rcWindows.Height() < iStartHeight)
	{
		iStartHeight = rcWindows.Height();
	}

	m_RectImage.top = rcWindows.top + (rcWindows.Height() - iStartHeight - 10)/2;	// ���10�����أ�����ͼƬ���ŵ׿�
	m_RectImage.bottom = m_RectImage.top + iStartHeight;

	m_RectImage.left = rcWindows.left + (rcWindows.Width() - iStartWidth)/2;
	m_RectImage.right = m_RectImage.left + iStartWidth;
}

void CIoViewSuggestPosition::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);
	SetSize();
	//RecalcRectSize();
}


// ֪ͨ��ͼ�ı��ע����Ʒ
void CIoViewSuggestPosition::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// ��ǰ��Ʒ��Ϊ�յ�ʱ��
	if (NULL != pMerch)
	{
		// �޸ĵ�ǰ�鿴����Ʒ
		m_pMerchXml					= pMerch;
		m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
		m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;

		// ���ù�ע����Ʒ��Ϣ
		m_aSmartAttendMerchs.RemoveAll();

		CSmartAttendMerch SmartAttendMerch;
		SmartAttendMerch.m_pMerch = pMerch;
		SmartAttendMerch.m_iDataServiceTypes = EDSTGeneral; //EDSTTimeSale | EDSTTick; // ��ע����������
		m_aSmartAttendMerchs.Add(SmartAttendMerch);
		
		// 
		OnVDataMerchTimeSalesUpdate(pMerch);	// �о���ʾ, ���������ʾ
	}
	else
	{
		// zhangbo 20090824 #�����䣬 ������Ʒ������ʱ����յ�ǰ��ʾ����
		//...
	}
	
	//InvalidateRect(m_RectTitle);	// ���ⲿ����Ҫ�޸ģ�������
}

//
void CIoViewSuggestPosition::OnVDataForceUpdate()
{
	RequestViewData();
}

bool32	CIoViewSuggestPosition::IsAttendData(IN CMerch *pMerch, E_DataServiceType eDataServiceType)
{
	if (eDataServiceType & m_iDataServiceType)
	{
		return TRUE;
	}

	return false;
}


void CIoViewSuggestPosition::OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch)
{
	int a = 10;
}


bool32 CIoViewSuggestPosition::FromXml(TiXmlElement * pElement)
{
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
	
	// ��Ʒ�����ı�
	OnVDataMerchChanged(iMarketId, StrMerchCode, pMerchFound);	

	OnVDataForceUpdate();

	return true;
}

CString CIoViewSuggestPosition::ToXml()
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

CString CIoViewSuggestPosition::GetDefaultXML()
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


void CIoViewSuggestPosition::OnIoViewActive()
{
	SetFocus();
	m_bActive = IsActiveInFrame();

	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	Invalidate();
}

void CIoViewSuggestPosition::OnIoViewDeactive()
{
	m_bActive = false;

	Invalidate();
}

void  CIoViewSuggestPosition::SetChildFrameTitle()
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

void CIoViewSuggestPosition::RequestViewData()
{
	if (NULL == m_pMerchXml)
		return;
}

BOOL CIoViewSuggestPosition::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}



void CIoViewSuggestPosition::OnVDataPluginResp( const CMmiCommBase *pResp )
{
	if ( NULL == pResp )
	{
		return;
	}
}

void CIoViewSuggestPosition::OnTimer( UINT nIDEvent )
{
	if ( KTimerIdPushDataReq == nIDEvent )
	{
		RequestViewData();
	}
	CIoViewBase::OnTimer(nIDEvent);
}

