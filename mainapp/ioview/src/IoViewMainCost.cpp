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
#include "IoViewMainCost.h"
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
static const int32 KTimerPeriodPushDataReq = 1000*60*5;	// ��������


CPoint			m_PointUlShortTermCost;	   // �����ڳɱ�
CPoint			m_PointShortTermCost;	   // ���ڳɱ�
CPoint			m_PointMediumermCost;	   // ���ڳɱ�
CPoint			m_PointLongTermCost;	   // ���ڳɱ�


#define  FORMULAR_MAIN_COST  L"���ɻ����ɱ�"
#define  LINE_UL_SHORT_COST		L"������"
#define  LINE_SHORT_COST		L"����"
#define  LINE_MEDIUMERM_COST	L"����"
#define  LINE_LONG_COST			L"����"

#define  TITLE_TOP		50;

#define  PAGE_STR_TITLE				 "����ʽ�"
#define  PAGE_STR_STRATEGY_TITLE    "�Ƽ����ԣ�"
#define  PAGE_STR_STRATEGY			"��ս����"

#define  RGB_COLOR_RED		RGB(0xff,0x38,0x37)
#define  RGB_COLOR_YELLOW	RGB(0xb9,0xfc,0x00)
#define  FONT_SPACING		1.2				// ������
#define  BIG_ROUND_SIZE     5				// ��Բ�Ĵ�С
#define  SMALL_ROUND_SIZE   2				// СԲ�Ĵ�С
#define  SMALL_ROUND_GAP	6				// СԲ�ļ��

/////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewMainCost, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewMainCost, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewMainCost)
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
CIoViewMainCost::CIoViewMainCost()
:CIoViewBase()
{
	m_bActive			= false;
	m_iDataServiceType  = EDSTKLine;	// GeneralΪ�յ���Ȩ�ź�

	m_stMainCostData.fPriceNow        = 16.66;
	m_stMainCostData.fUlShortTermCost =16.20;
	m_stMainCostData.fShortTermCost = 17.52;
	m_stMainCostData.fMediumermCost = 14.20;
	m_stMainCostData.fLongTermCost  = 19.36;

	m_Font14.CreateFont(14, 0,0,0,FW_NORMAL, 0,0,0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"����");
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewMainCost::~CIoViewMainCost()
{
	m_Font14.DeleteObject();
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint

void CIoViewMainCost::OnPaint()
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
	COLORREF clrText = GetIoViewColor(ESCText);
	COLORREF clrHighValue = RGB(185,250,0);
	COLORREF clrLowValue =  RGB(255,17,50);
	COLORREF clrTitle = RGB_COLOR_RED;


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
	dcMem->FillSolidRect(rcClient, clrBk);

	//DrawCorner(dcMem, rcClient);

	//// ����
	//CBrush cbr;
	//CPen cpenbr;
	//cpenbr.CreatePen(PS_SOLID, 1, clrVol); 
	//CPen *pOldPen = dcMem->SelectObject(&cpenbr);
	//cbr.CreateStockObject(NULL_BRUSH); //����һ���ջ�ˢ
	//CBrush *pOldBr = dcMem->SelectObject(&cbr);
	//dcMem->RoundRect(rcClient, CPoint(10, 10));
	//cbr.DeleteObject();
	//dcMem->SelectObject(pOldBr);
	//dcMem->SelectObject(pOldPen);

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
		if ( !m_RectTitle.IsRectEmpty() )
		{
			CString StrTitle = CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
			CFont *pOldPen = dcMem->SelectObject(&m_Font14);
			dcMem->SetTextColor(clrTitle);
			CRect rcTmp(0,0,0,0);
			dcMem->DrawText(StrTitle, m_RectTitle, DT_SINGLELINE |DT_VCENTER |DT_LEFT);
			dcMem->SelectObject(pOldPen);
		}

		DrawLineAndPriceNow(dcMem, clrHighValue, clrLowValue);
		DrawBigRound(dcMem, clrHighValue,  clrLowValue);
		DrawSmallRound(dcMem, clrHighValue,  clrLowValue);
		DrawMainText(dcMem, RGB(185,252,0), RGB(255,71,90));
	}
	
	dc.BitBlt(0,0, rcClient.Width(),rcClient.Height(),&dcMem,0,0,SRCCOPY);  

	//��ͼ��ɺ������  
	MemBitmap.DeleteObject();  
	dcMem->RestoreDC(iSaveDC);
}

// �����뵱ǰ��
void CIoViewMainCost::DrawLineAndPriceNow(CDC &dc,COLORREF clrHighValue, COLORREF clrLowValue)
{
	int iSaveDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);
	dc.SetBkColor(GetIoViewColor(ESCBackground));

	// ����
	COLORREF clrVol = GetIoViewColor(ESCVolume);
	CPen penLine(PS_SOLID, 1, clrVol);
	CPen *pOldPen;
	pOldPen = dc.SelectObject(&penLine);
	CPoint pStart(m_RectLine.left, m_RectLine.top);
	CPoint pEnd(m_RectLine.right, m_RectLine.top);
	dc.MoveTo(pStart);
	dc.LineTo(pEnd);
	dc.SelectObject(pOldPen);

	// ��ǰ��;
	CString strPriceNow;
	strPriceNow.Format(_T("%0.2f"), m_stMainCostData.fPriceNow);

	CPen penMain(PS_SOLID, 1, clrHighValue), penOther(PS_SOLID, 1, clrLowValue), penBlack(PS_SOLID, 1, RGB(0,0,0));
	pOldPen = dc.SelectObject(&penMain);
	CFont *pFontSmall = GetIoViewFontObject(ESFSmall);
	CFont *pOldFont;
	pOldFont = dc.SelectObject(pFontSmall);
	CSize sizeWidth = dc.GetTextExtent(strPriceNow);

	CString strPrice;
	CRect rcPrice;
	rcPrice.top = m_RectLine.top;
	rcPrice.bottom = rcPrice.top + sizeWidth.cy + 2*4;
	rcPrice.left = 0;
	rcPrice.right = rcPrice.left + sizeWidth.cx*1.3;

	dc.SetTextColor(clrHighValue);
	if (m_aPointTerm[BIG_ULSHORT_TERM].y > m_RectLine.top)
	{
		rcPrice.OffsetRect(CPoint(0,  - rcPrice.Height()));
		dc.SetTextColor(clrLowValue);
	}
	dc.DrawText(strPriceNow, rcPrice, DT_SINGLELINE |DT_VCENTER |DT_RIGHT);


	CRect rcNowPrice(0,0,0,0);
	dc.DrawText(_T(" ���¼�"), rcNowPrice, DT_SINGLELINE |DT_CALCRECT |DT_VCENTER |DT_LEFT);
	rcPrice.left = rcPrice.right;
	rcPrice.right = rcPrice.left + rcNowPrice.Width();
	dc.DrawText(_T(" ���¼�"), rcPrice, DT_SINGLELINE  |DT_VCENTER |DT_LEFT);

	dc.SelectObject(pOldFont);
	dc.SelectObject(pOldPen);
}

// ��������
void CIoViewMainCost::DrawMainText(CDC &dc,COLORREF clrHighValue, COLORREF clrLowValue)
{
	int iSaveDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);
	dc.SetBkColor(GetIoViewColor(ESCBackground));

	m_RectMainText;
	CSize sizeWidth = dc.GetTextExtent(_T("��"));
	CRect rcTextLeft, rcTextRight;
	// �����������ĵ�
	rcTextLeft.top = m_RectMainText.top;
	rcTextLeft.left = m_RectMainText.left;
	rcTextLeft.right = rcTextLeft.left + m_RectMainText.Width()/5;
	rcTextLeft.bottom = rcTextLeft.top + sizeWidth.cy + 2*4;

	rcTextRight = rcTextLeft;
	rcTextRight.top = rcTextRight.top + (rcTextRight.Height() - sizeWidth.cy)/2;
	rcTextRight.bottom = rcTextRight.top + sizeWidth.cy;
	rcTextRight.left = rcTextLeft.right ;
	rcTextRight.right = rcTextRight.left + m_RectMainText.Width()/5;

	float aMainData[BIG_TERM_COUNT];
	aMainData[BIG_ULSHORT_TERM]		= m_stMainCostData.fUlShortTermCost;
	aMainData[BIG_SHORT_TERM]		= m_stMainCostData.fShortTermCost;
	aMainData[BIG_MEDIUMERM_TERM]	= m_stMainCostData.fMediumermCost;
	aMainData[BIG_LONG_TERM]		= m_stMainCostData.fLongTermCost;

	CPen penMain(PS_SOLID, 1, clrHighValue), penOther(PS_SOLID, 1, clrLowValue), penBlack(PS_SOLID, 1, RGB(0,0,0));
	CBrush brMain(clrHighValue), brOther(clrLowValue);
	CPen   *pOldPen = dc.SelectObject(&penMain);
	CBrush *pOldBrush = dc.SelectObject(&brMain);

	CFont *pFontSmall  = GetIoViewFontObject(ESFSmall);
	CFont *pOldFont = NULL;
	pOldFont = dc.SelectObject(pFontSmall);

	for (int i = BIG_ULSHORT_TERM; i < BIG_TERM_COUNT; ++i)
	{
		CRect rcLeftTemp = rcTextLeft;
		CRect rcRightTemp = rcTextRight;

		CString strCostName;
		CString strCostValue;
		if (BIG_ULSHORT_TERM == i)
		{
			strCostName = _T("������");
			
		}
		else if (BIG_SHORT_TERM == i)
		{
			strCostName = _T("����");
			rcLeftTemp.OffsetRect(CPoint(m_RectMainText.Width()/2, 0));
			rcRightTemp.OffsetRect(CPoint(m_RectMainText.Width()/2, 0));	
		}
		else if (BIG_MEDIUMERM_TERM == i)
		{
			strCostName = _T("����");
			rcLeftTemp.OffsetRect(CPoint(0, rcLeftTemp.Height()));
			rcRightTemp.OffsetRect(CPoint(0, rcLeftTemp.Height()));	
		}
		else if (BIG_LONG_TERM == i)
		{
			strCostName = _T("����");
			rcLeftTemp.OffsetRect(CPoint(m_RectMainText.Width()/2, rcLeftTemp.Height()));
			rcRightTemp.OffsetRect(CPoint(m_RectMainText.Width()/2, rcLeftTemp.Height()));
		}

		if (aMainData[i] >= m_stMainCostData.fPriceNow)
		{
			dc.SelectObject(&brMain);
			dc.SelectObject(&penMain);
			dc.SetTextColor(clrHighValue);
		}
		else
		{
			dc.SelectObject(&penOther);
			dc.SelectObject(&brOther);
			dc.SetTextColor(clrLowValue);
		}
		
		dc.DrawText(strCostName, rcLeftTemp, DT_SINGLELINE |DT_VCENTER |DT_CENTER);
		dc.Rectangle(rcRightTemp);

		strCostValue.Format(_T("%0.2f"), aMainData[i]);
		dc.SetTextColor(RGB(0,0,0));
		dc.DrawText(strCostValue, rcRightTemp, DT_SINGLELINE |DT_VCENTER |DT_CENTER);
	}		

	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldFont);
}

void CIoViewMainCost::DrawBigRound(CDC &dc,COLORREF clrHighValue, COLORREF clrLowValue)
{
	int iSaveDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);
	dc.SetBkColor(GetIoViewColor(ESCBackground));

	// ��Բ
	CPen penMain(PS_SOLID, 1, clrHighValue), penOther(PS_SOLID, 1, clrLowValue);
	CPen *pOldPen;
	CBrush brMain(clrHighValue), brOther(clrLowValue);
	pOldPen = dc.SelectObject(&penMain);
	CBrush *pOldBrush = dc.SelectObject(&brMain);
	for (int i = BIG_ULSHORT_TERM; i < BIG_TERM_COUNT; ++i)
	{
		
		if (m_aPointTerm[i].y <= m_RectLine.bottom)
		{
			CRect rcDot(-BIG_ROUND_SIZE, -BIG_ROUND_SIZE, BIG_ROUND_SIZE, BIG_ROUND_SIZE);
			rcDot.OffsetRect(m_aPointTerm[i]);
			dc. Ellipse(rcDot);
		}
	}

	dc.SelectObject(&penOther);
	dc.SelectObject(&brOther);
	for (int i = BIG_ULSHORT_TERM; i < BIG_TERM_COUNT; ++i)
	{
		if (m_aPointTerm[i].y >  m_RectLine.bottom)
		{
			CRect rcDot(-BIG_ROUND_SIZE, -BIG_ROUND_SIZE, BIG_ROUND_SIZE, BIG_ROUND_SIZE);
			rcDot.OffsetRect(m_aPointTerm[i]);
			dc. Ellipse(rcDot);
		}
	}
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
}

void CIoViewMainCost::DrawSmallRound(CDC &dc,COLORREF clrHighValue, COLORREF clrLowValue)
{
	int iSaveDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);
	dc.SetBkColor(GetIoViewColor(ESCBackground));

	// ��Բ
	CPen penMain(PS_SOLID, 1, clrHighValue), penOther(PS_SOLID, 1, clrLowValue);
	CPen *pOldPen;
	CBrush brMain(clrHighValue), brOther(clrLowValue);
	pOldPen = dc.SelectObject(&penMain);
	CBrush *pOldBrush = dc.SelectObject(&brMain);

	CPoint PointStart , PointEnd;
	PointStart.x = m_RectLine.left;
	PointStart.y = m_RectLine.bottom;

	for (int i = BIG_ULSHORT_TERM; i < BIG_TERM_COUNT; ++i)
	{
		PointEnd = m_aPointTerm[i];
		int iSmallRoundSize = 0;
		iSmallRoundSize = (PointEnd.x - PointStart.x)/(SMALL_ROUND_SIZE + SMALL_ROUND_GAP);

		// �������������ˣ�ֱ���üӼ���
		float fGapX = 0;
		float fGapY = 0;
		fGapX = (fabsf(PointEnd.x - PointStart.x)) / iSmallRoundSize;
		fGapY = (fabsf(PointEnd.y - PointStart.y)) / iSmallRoundSize;

		float fPosX = PointStart.x;
		float fPosY = PointStart.y;
		for (int j = 0; j < iSmallRoundSize; ++j)
		{
			// ��������ֵС��1��ʱ��Ҳ�������,�����ٴ����һ��������
			// ������ӣ��½����
			if (PointStart.y > PointEnd.y)
			{
				fPosX += fGapX;
				fPosY -= fGapY;
			}
			else
			{
				fPosX  += fGapX;
				fPosY  += fGapY;
			}

			PointStart.x = fPosX;
			PointStart.y = fPosY;

			// ���⴦����X�����ص��ĵ㣬�ô�����ж�
			if (PointStart.y == m_RectLine.bottom)
			{
				if (PointEnd.y <=  m_RectLine.bottom)
				{
					CRect rcDot(-SMALL_ROUND_SIZE, -SMALL_ROUND_SIZE, SMALL_ROUND_SIZE, SMALL_ROUND_SIZE);
					rcDot.OffsetRect(PointStart);
					dc. Ellipse(rcDot);
				}
			}
			else
			{
				if (PointStart.y <=  m_RectLine.bottom)
				{
					CRect rcDot(-SMALL_ROUND_SIZE, -SMALL_ROUND_SIZE, SMALL_ROUND_SIZE, SMALL_ROUND_SIZE);
					rcDot.OffsetRect(PointStart);
					dc. Ellipse(rcDot);
				}
			}
		}
		PointStart = m_aPointTerm[i];
	}

	dc.SelectObject(&penOther);
	dc.SelectObject(&brOther);
	PointStart.x = m_RectLine.left;
	PointStart.y = m_RectLine.bottom;
	for (int i = BIG_ULSHORT_TERM; i < BIG_TERM_COUNT; ++i)
	{
		PointEnd = m_aPointTerm[i];
		int iSmallRoundSize = 0;
		iSmallRoundSize = (PointEnd.x - PointStart.x)/(SMALL_ROUND_SIZE + SMALL_ROUND_GAP);

		// �������������ˣ�ֱ���üӼ���
		float fGapX = 0;
		float fGapY = 0;
		fGapX = (fabsf(PointEnd.x - PointStart.x)) / iSmallRoundSize;
		fGapY = (fabsf(PointEnd.y - PointStart.y)) / iSmallRoundSize;

		float fPosX = PointStart.x;
		float fPosY = PointStart.y;
		for (int j = 0; j < iSmallRoundSize; ++j)
		{
			// ��������ֵС��1��ʱ��Ҳ�������,�����ٴ����һ��������
			// ������ӣ��½����
			if (PointStart.y > PointEnd.y)
			{
				fPosX += fGapX;
				fPosY -= fGapY;
			}
			else
			{
				fPosX  += fGapX;
				fPosY  += fGapY;
			}

			PointStart.x = fPosX;
			PointStart.y = fPosY;

			// ���⴦����X�����ص��ĵ㣬�ô�����ж�
			if (PointStart.y == m_RectLine.bottom)
			{
				if (PointEnd.y >  m_RectLine.bottom)
				{
					CRect rcDot(-SMALL_ROUND_SIZE, -SMALL_ROUND_SIZE, SMALL_ROUND_SIZE, SMALL_ROUND_SIZE);
					rcDot.OffsetRect(PointStart);
					dc. Ellipse(rcDot);
				}
			}
			else
			{
				if (PointStart.y >  m_RectLine.bottom)
				{
					CRect rcDot(-SMALL_ROUND_SIZE, -SMALL_ROUND_SIZE, SMALL_ROUND_SIZE, SMALL_ROUND_SIZE);
					rcDot.OffsetRect(PointStart);
					dc. Ellipse(rcDot);
				}
			}
		}
		PointStart = m_aPointTerm[i];
	}

	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
}

void CIoViewMainCost::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();

	Invalidate();
}

void CIoViewMainCost::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch)
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

			// ȡ�������еĹ�����
			CFormularContent* pContent = CFormulaLib::instance()->GetFomular(FORMULAR_MAIN_COST);
			if ( NULL != pContent )
			{
				float fRed		= 0.0;
				float fGreen	= 0.0;
				float fYelloew  = 0.0;
				T_IndexOutArray* pOut = formula_index(pContent, pKLineRequest->m_KLines);
				if (pOut && pOut->iIndexNum > 0)
				{
					float fUlShortCost	 = 0.0;
					float fShortCost	 = 0.0;
					float fMediumermCost = 0.0;
					float fLongCost		 = 0.0;
					for (int i = 0; i < pOut->iIndexNum; i ++ )
					{
						if (0 == pOut->index[i].StrName.CompareNoCase(LINE_UL_SHORT_COST))
						{
							if (pOut->index[i].iPointNum > 0)
							{
								int32 iPointNum = pOut->index[i].iPointNum;

								m_stMainCostData.fUlShortTermCost  =  pOut->index[i].pPoint[iPointNum - 1];
							}											
						}

						if (0 == pOut->index[i].StrName.CompareNoCase(LINE_SHORT_COST))
						{
							if (pOut->index[i].iPointNum > 0)
							{
								int32 iPointNum = pOut->index[i].iPointNum;

								m_stMainCostData.fShortTermCost  =  pOut->index[i].pPoint[iPointNum - 1];
							}											
						}

						if (0 == pOut->index[i].StrName.CompareNoCase(LINE_MEDIUMERM_COST))
						{
							if (pOut->index[i].iPointNum > 0)
							{
								int32 iPointNum = pOut->index[i].iPointNum;

								m_stMainCostData.fMediumermCost  =  pOut->index[i].pPoint[iPointNum - 1];
							}											
						}


						if (0 == pOut->index[i].StrName.CompareNoCase(LINE_LONG_COST))
						{
							if (pOut->index[i].iPointNum > 0)
							{
								int32 iPointNum = pOut->index[i].iPointNum;

								m_stMainCostData.fLongTermCost  =  pOut->index[i].pPoint[iPointNum - 1];
							}											
						}
					}

					// �������ݶ���Ҫʹ�� m_pMerchXml������
					CRealtimePrice * pRealtimePrice = m_pMerchXml->m_pRealtimePrice;	
					if (NULL != pRealtimePrice)
					{
						m_stMainCostData.fPriceNow = pRealtimePrice->m_fPriceNew;
						if (fabs(m_stMainCostData.fPriceNow) < 1e-6)
						{
							m_stMainCostData.fPriceNow = pRealtimePrice->m_fPricePrevClose;
						}

					}

					// ��ʵû��Ҫ�������
					SetSize();
					Invalidate();
					UpdateWindow();
				}
			}
		}		
	}
}

void CIoViewMainCost::OnIoViewFontChanged()
{	
	CIoViewBase::OnIoViewFontChanged();
	// ������� - ��ͼ���ò�Э��
	//RecalcRectSize();
	Invalidate();
}

void CIoViewMainCost::LockRedraw()
{
	if ( m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
}

void CIoViewMainCost::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
}

int CIoViewMainCost::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	int iRet = CIoViewBase::OnCreate(lpCreateStruct);
	if ( -1 == iRet )
	{
		return -1;
	}

	InitialIoViewFace(this);
	SetTimer(KTimerIdPushDataReq, KTimerPeriodPushDataReq, NULL);

	return iRet;
}

void CIoViewMainCost:: GetMaxGapAndPriceLow(const ST_MAIN_COST_DATA& stMainCostData, float &fMaxGap, float &fPriceLow)
{
	float  fGap = 0; 
	fMaxGap = fabsf(stMainCostData.fUlShortTermCost - stMainCostData.fPriceNow);

	fGap = fabsf(stMainCostData.fShortTermCost - stMainCostData.fPriceNow);
	if (fGap > fMaxGap)
	{
		fMaxGap = fGap;
	}
	fGap = fabsf(stMainCostData.fMediumermCost - stMainCostData.fPriceNow);
	if (fGap > fMaxGap)
	{
		fMaxGap = fGap;
	}
	fGap = fabsf(stMainCostData.fLongTermCost - stMainCostData.fPriceNow);
	if (fGap > fMaxGap)
	{
		fMaxGap = fGap;
	}
	fMaxGap = fMaxGap * 2;


	fPriceLow = stMainCostData.fUlShortTermCost;
	if (stMainCostData.fShortTermCost < fPriceLow)
	{
		fPriceLow = stMainCostData.fShortTermCost;
	}

	if (stMainCostData.fMediumermCost < fPriceLow)
	{
		fPriceLow = stMainCostData.fMediumermCost;
	}

	if (stMainCostData.fLongTermCost < fPriceLow)
	{
		fPriceLow = stMainCostData.fLongTermCost;
	}
}

float CIoViewMainCost::GetHeightPercent(float fPriceNow, float fCalPrice, float fMaxGap)
{
	float fPosIndex = 0;
	float fPrice;
	fPrice = fCalPrice - fPriceNow;

	if (fPrice < 1e-7 && fPrice > -1e-7)
	{
		fPosIndex = 0.5; 
	}
	else if (fPrice > 1e-7)
	{	
		fPrice = fMaxGap/2 - fabsf(fPrice);
		fPosIndex = fPrice / fMaxGap;
	}
	else
	{
		fPrice	= fabsf(fPrice) + fMaxGap/2;
		fPosIndex = fPrice / fMaxGap;
	}

	return fPosIndex;
}

void CIoViewMainCost::SetSize()
{
	CRect rcWindows;
	GetClientRect(&rcWindows);
	m_RectTitle = rcWindows;
	m_RectTitle.InflateRect(-10, -10);	// ��϶

	CClientDC dc(this);

	CString StrTitleWidth   = CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;;

	CSize sizeWidth = dc.GetTextExtent(_T("��"));
	m_RectTitle.right = m_RectTitle.left + sizeWidth.cx* StrTitleWidth.GetLength() *FONT_SPACING;	// �������壬�̶������
	m_RectTitle.bottom = m_RectTitle.top + sizeWidth.cy + 2*4;

	// ȥ������ĸ߶ȣ�����Ԥ��һ���ֵĸ߶�
	rcWindows.top  += m_RectTitle.bottom + sizeWidth.cy;

	// ��ʾ����λ�ã������������������꣬��ʱ����С������Ͳ��ü�����
	CFont *pFontNormal = GetIoViewFontObject(ESFNormal);
	CFont *pFontSmall  = GetIoViewFontObject(ESFSmall);
	CFont *pOldFont = NULL;
	pOldFont = dc.SelectObject(pFontNormal);
	CSize sizeWidthNormal = dc.GetTextExtent(_T("��"));
	dc.SelectObject(pFontSmall);
	CSize sizeWidthSmall = dc.GetTextExtent(_T("��"));
	dc.SelectObject(pOldFont);
	m_RectMainText = rcWindows;
	m_RectMainText.bottom = m_RectMainText.bottom - 10;		// ���ⶥ���׿�
	m_RectMainText.top = m_RectMainText.bottom -  sizeWidthNormal.cy *3 + 3*4 ;	// Ԥ��һ����Բ�Ŀռ�

	m_RectMainRound = rcWindows;
	m_RectMainRound.top    = m_RectTitle.bottom + sizeWidth.cy;
	m_RectMainRound.bottom = m_RectMainText.top - 10; 

	int iMainRoundHeight = m_RectMainRound.Height();
	m_RectLine.top = m_RectMainRound.top + iMainRoundHeight/2;
	m_RectLine.left = m_RectMainRound.left + m_RectMainRound.Width()/10;
	m_RectLine.right = m_RectMainRound.right - m_RectMainRound.Width()/20;
	m_RectLine.bottom = m_RectMainRound.top + iMainRoundHeight/2;

	// ��ΪЧ��ͼ��ԭ��,��һ����Ϊ1/3,�ڶ�����1/2����������2/3�����ĸ���4/4
	float fMaxGap = 0;
	float fPriceLow = 0;
	float fPos;

	GetMaxGapAndPriceLow(m_stMainCostData, fMaxGap, fPriceLow);
	fPos = GetHeightPercent(m_stMainCostData.fPriceNow, m_stMainCostData.fUlShortTermCost, fMaxGap);
	m_PointUlShortTermCost.x = m_RectMainRound.left + m_RectMainRound.Width()/3;
	m_PointUlShortTermCost.y = m_RectMainRound.top + iMainRoundHeight * fPos;
	m_aPointTerm[BIG_ULSHORT_TERM] = m_PointUlShortTermCost;

	fPos = GetHeightPercent(m_stMainCostData.fPriceNow, m_stMainCostData.fShortTermCost, fMaxGap);
	m_PointShortTermCost.x = m_RectMainRound.left + m_RectMainRound.Width()/2;
	m_PointShortTermCost.y = m_RectMainRound.top + iMainRoundHeight * fPos;
	m_aPointTerm[BIG_SHORT_TERM] = m_PointShortTermCost;

	fPos = GetHeightPercent(m_stMainCostData.fPriceNow, m_stMainCostData.fMediumermCost, fMaxGap);
	m_PointMediumermCost.x = m_RectMainRound.left + m_RectMainRound.Width()/10 * 7;
	m_PointMediumermCost.y = m_RectMainRound.top + iMainRoundHeight * fPos;
	m_aPointTerm[BIG_MEDIUMERM_TERM] = m_PointMediumermCost;

	fPos = GetHeightPercent(m_stMainCostData.fPriceNow, m_stMainCostData.fLongTermCost, fMaxGap);
	m_PointLongTermCost.x = m_RectMainRound.left + m_RectMainRound.Width()/10*9;
	m_PointLongTermCost.y = m_RectMainRound.top + iMainRoundHeight * fPos;
	m_aPointTerm[BIG_LONG_TERM] = m_PointLongTermCost;
}

void CIoViewMainCost::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);
	SetSize();
}

// ֪ͨ��ͼ�ı��ע����Ʒ
void CIoViewMainCost::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
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
		SmartAttendMerch.m_iDataServiceTypes = EDSTKLine; //EDSTTimeSale | EDSTTick; // ��ע����������
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
void CIoViewMainCost::OnVDataForceUpdate()
{
	RequestViewData();
}

bool32	CIoViewMainCost::IsAttendData(IN CMerch *pMerch, E_DataServiceType eDataServiceType)
{
	if (eDataServiceType & m_iDataServiceType)
	{
		return TRUE;
	}

	return false;
}

void CIoViewMainCost::OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch)
{

}

bool32 CIoViewMainCost::FromXml(TiXmlElement * pElement)
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

CString CIoViewMainCost::ToXml()
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

CString CIoViewMainCost::GetDefaultXML()
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

void CIoViewMainCost::OnIoViewActive()
{
	SetFocus();
	m_bActive = IsActiveInFrame();

	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	Invalidate();
}

void CIoViewMainCost::OnIoViewDeactive()
{
	m_bActive = false;
	Invalidate();
}

void  CIoViewMainCost::SetChildFrameTitle()
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

void CIoViewMainCost::RequestViewData()
{
}

BOOL CIoViewMainCost::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

void CIoViewMainCost::OnVDataPluginResp( const CMmiCommBase *pResp )
{
}

void CIoViewMainCost::OnTimer( UINT nIDEvent )
{
	if ( KTimerIdPushDataReq == nIDEvent )
	{
		RequestViewData();
	}
	CIoViewBase::OnTimer(nIDEvent);
}

