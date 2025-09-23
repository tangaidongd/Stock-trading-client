// LeftToolBar.cpp : implementation file
//
#include "stdafx.h"
#include "ShortCutMenuToolBar.h"
#include "IoViewReportArbitrage.h"
#include "IoViewKLineArbitrage.h"
#include "IoViewTrendArbitrage.h"
#include "DlgFormularManager.h"
#include "IoViewDuoGuTongLie.h"
#include "GGTong.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define INVALID_ID     -1
#define MARGIN     5

// ��ʾ���ֶ�ʱ��ID
const UINT KTimerIdTip	          = 60008;
const UINT KTimerPeriodTip        = 1000;


enum E_Btnfunction
{
	BTN_FC_REPORT  = 9000,		// ����б�
	BTN_FC_KLINE,				// K��
	BTN_FC_TREND,				// ��ʱ
	BTN_FC_F10,					// F10
	BTN_FC_NEWS,				// ��Ѷ
	BTN_FC_USER_BLOCK,			// ��ѡ��
	BTN_FC_TRANSACTION,			// ����
	BTN_FC_FOMULAR,				// ��ʽ����
	BTN_FC_DRAW_LINE,			// ���߹���
	BTN_FC_MUL_STOCK_COL,		// ���ͬ��
	BTN_FC_SEPARATE_LINE,		// �ָ���
	BTN_FC_CYCLE_1MIN,			// 1����
	BTN_FC_CYCLE_5MIN,			// 5����
	BTN_FC_CYCLE_15MIN,			// 15����
	BTN_FC_CYCLE_30MIN,			// 30����
	BTN_FC_CYCLE_60MIN,			// 60����
	BTN_FC_CYCLE_DAY,			// ����
	BTN_FC_CYCLE_WEEK,			// ����
	BTN_FC_CYCLE_MONTH,			// ����
	BTN_FC_CYCLE_MUL_MIN,		// �������
	BTN_FC_CYCLE_QUARTERLY,		// ������
	BTN_FC_CYCLE_YEAR,			// ����
};


typedef struct ST_LineFunctionData
{
	unsigned int m_uiFcId;
	CString      m_strDisName;
	CString      m_strDescription;

	ST_LineFunctionData(unsigned int uiFcId,
					    CString      strDisName,
						CString      strDescription)
	{
		m_uiFcId = uiFcId;
		m_strDisName = strDisName;
		m_strDescription = strDescription;
	};
}ST_LineFunctionData;



ST_LineFunctionData g_aShortCutMenuFunction[] = {
	ST_LineFunctionData(BTN_FC_REPORT,			_T(""), _T("�����б�")),			// ����б�
	ST_LineFunctionData(BTN_FC_KLINE,			_T(""), _T("K�߷���")),				// K��
	ST_LineFunctionData(BTN_FC_TREND,			_T(""),	_T("��ʱ����")),			// ��ʱ
	ST_LineFunctionData(BTN_FC_F10,				_T(""),	_T("��������")),				// F10
	ST_LineFunctionData(BTN_FC_NEWS,			_T(""),	_T("��Ѷ����")),			// ��Ѷ
	ST_LineFunctionData(BTN_FC_USER_BLOCK,		_T(""),	_T("��ѡ��")),				// ��ѡ��
	ST_LineFunctionData(BTN_FC_TRANSACTION,		_T(""),	_T("����ί��")),			// ����
	ST_LineFunctionData(BTN_FC_FOMULAR,			_T(""),	_T("��ʽ������")),			// ��ʽ����
	ST_LineFunctionData(BTN_FC_DRAW_LINE,		_T(""),	_T("���߹���")),			// ���߹���
	ST_LineFunctionData(BTN_FC_MUL_STOCK_COL,	_T(""),	_T("���ͬ��")),			// ���ͬ��

	ST_LineFunctionData(BTN_FC_SEPARATE_LINE,	_T(""),		_T("")),				// �ָ���

	ST_LineFunctionData(BTN_FC_CYCLE_1MIN,			_T(""), 	_T("1����ͼ")),		// 1����
	ST_LineFunctionData(BTN_FC_CYCLE_5MIN,			_T(""), 	_T("5����ͼ")),		// 5����
	ST_LineFunctionData(BTN_FC_CYCLE_15MIN,			_T(""),	_T("15����ͼ")),		// 15����
	ST_LineFunctionData(BTN_FC_CYCLE_30MIN,			_T(""),	_T("30����ͼ")),		// 30����
	ST_LineFunctionData(BTN_FC_CYCLE_60MIN,			_T(""),	_T("60����ͼ")),		// 60����
	ST_LineFunctionData(BTN_FC_CYCLE_DAY,			_T(""),	_T("����ͼ")),			// ����
	ST_LineFunctionData(BTN_FC_CYCLE_WEEK,			_T(""),	_T("����ͼ")),			// ����
	ST_LineFunctionData(BTN_FC_CYCLE_MONTH,			_T(""),	_T("����ͼ")),			// ����
	//ST_LineFunctionData(BTN_FC_CYCLE_MUL_MIN,		_T("��"),	_T("�������")),	// �������
	ST_LineFunctionData(BTN_FC_CYCLE_QUARTERLY,		_T(""),	_T("����")),			// ������
	ST_LineFunctionData(BTN_FC_CYCLE_YEAR,			_T(""),	_T("����"))				// ����
	
};


/////////////////////////////////////////////////////////////////////////////
// CShortCutMenuToolBar

CShortCutMenuToolBar::CShortCutMenuToolBar()
{
	m_pImgBk = NULL;
	m_iXButtonHovering = INVALID_ID;
	m_mapSCBarImage.clear();
}

CShortCutMenuToolBar::~CShortCutMenuToolBar()
{
	DEL(m_pImgBk);

	std::map<int, Image*>::iterator iter = m_mapSCBarImage.begin();
	for (; iter != m_mapSCBarImage.end(); iter++)
	{
		DEL(iter->second);
	}
}

BEGIN_MESSAGE_MAP(CShortCutMenuToolBar, CToolBar)
	//{{AFX_MSG_MAP(CShortCutMenuToolBar)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShortCutMenuToolBar message handlers

void CShortCutMenuToolBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.OffsetRect(-rcWindow.left, -rcWindow.top);


	bmp.CreateCompatibleBitmap(&dc, rcWindow.Width(), rcWindow.Height());
	memDC.SelectObject(&bmp);
	memDC.FillSolidRect(0, 0, rcWindow.Width(), rcWindow.Height(), RGB(0,0,0));
	memDC.SetBkMode(TRANSPARENT);


	CRect rcDest = rcWindow;
	rcDest.DeflateRect(MARGIN,MARGIN,0,MARGIN);

	// ����
	// ����
	CBrush cbr;
	CPen cpenbr;
	cpenbr.CreatePen(PS_SOLID, 1, RGB(78,78,78)); 
	CPen *pOldPen = memDC.SelectObject(&cpenbr);
	cbr.CreateStockObject(NULL_BRUSH); //����һ���ջ�ˢ
	CBrush *pOldBr = memDC.SelectObject(&cbr);
	memDC.RoundRect(rcDest, CPoint(0, 0));
	cbr.DeleteObject();
	memDC.SelectObject(pOldBr);
	memDC.SelectObject(pOldPen);

	// �ճ��ߵ�λ��
	rcDest.DeflateRect(1,1,1,1);



	//����������
	Gdiplus::Graphics graphics(memDC.GetSafeHdc());
	RectF destRect;
	destRect.X =rcDest.left;
	destRect.Y = rcDest.top;
	destRect.Width  = rcDest.Width();
	destRect.Height = rcDest.Height();
	graphics.DrawImage(m_pImgBk, destRect, 0, 0, m_pImgBk->GetWidth(), m_pImgBk->GetHeight() , UnitPixel);


	// ����ť
	map<int, CNCButton>::iterator iter;
	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
	{
		CNCButton &btn = iter->second;
		CRect rc; 
		btn.GetRect(rc);
		if (rc.Height() > 0)
		{
			btn.DrawButton(&graphics);
		}
	}

	dc.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &memDC, 0, 0, SRCCOPY);
	dc.SelectClipRgn(NULL);
	memDC.DeleteDC();
	bmp.DeleteObject();



	// TODO: Add your message handler code here
	// Do not call CToolBar::OnPaint() for painting messages
}

void CShortCutMenuToolBar::OnSize( UINT nType, int cx, int cy )
{
	CWnd::OnSize(nType, cx, cy);
	CRect rcToolBar;
	GetWindowRect(&rcToolBar);
	rcToolBar.OffsetRect(-rcToolBar.left, -rcToolBar.top);
	rcToolBar.DeflateRect(MARGIN,MARGIN,0,MARGIN);
	if (rcToolBar.Height()<1)
	{
		return;
	}

	int iInterval = 2;
	int iStartTop = 5;
	CRect rcFirst;
	rcFirst = rcToolBar;

	// ��С���ñ����б�ģ����е�ͼ���С����һ�µ�
	if (m_mapSCBarImage.count(BTN_FC_REPORT) < 1)
	{
		return;
	}
	Image * BtnImage = m_mapSCBarImage[BTN_FC_REPORT];
	int iYMove = 0;
	
	rcFirst.top = rcToolBar.top + iStartTop;
	rcFirst.bottom = rcFirst.top + BtnImage->GetHeight()/3;
	rcFirst.left = rcFirst.left + (rcToolBar.Width() - BtnImage->GetWidth())/2;
	rcFirst.right = rcFirst.left + BtnImage->GetWidth();
	map<int, CNCButton>::iterator iter;
	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
	{
		CNCButton &btn = iter->second;
		// ���⴦���ָ���̫С��
		if (BTN_FC_SEPARATE_LINE == iter->first)
		{
			
			if (m_mapSCBarImage.count(BTN_FC_SEPARATE_LINE) > 0)
			{
				rcFirst.OffsetRect(0, -iInterval);// �ָ��߾��ƻ�ȥ�ϴε�λ��
				CRect rcSeparate = rcFirst;
				rcSeparate.top = rcSeparate.top + 3;	// �������صļ��

				Image *pImage = m_mapSCBarImage[BTN_FC_SEPARATE_LINE];
				rcSeparate.bottom = rcSeparate.top + pImage->GetHeight();

				rcSeparate.left = rcToolBar.left + (rcToolBar.Width() - pImage->GetWidth())/2;
				rcSeparate.right = rcFirst.left + pImage->GetWidth();

				btn.SetRect(rcSeparate);
				iYMove = rcSeparate.Height() + 3;
				rcSeparate.OffsetRect(0, iYMove);

				// ����С����btn��С
				rcFirst.top = rcSeparate.top;
				rcFirst.bottom = rcFirst.top + BtnImage->GetHeight()/3;
				continue;
			}
			else
			{
				continue;
			}
		}

		if (rcFirst.bottom > rcToolBar.bottom)
		{
			btn.SetRect(CRect(0,0,0,0));
		}
		else
		{
			btn.SetRect(rcFirst);
		}
		iYMove = rcFirst.Height() + iInterval;
		rcFirst.OffsetRect(0, iYMove);
	}
}



void CShortCutMenuToolBar::ShowTips()
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	CRect rectClient;
	GetClientRect(&rectClient);

	if (!PtInRect(&rectClient, point))
	{
		return;
	}

	// �ж���Ҫ��ʾ����
	BOOL bShowTip = FALSE;
	CString StrTipMsg   = L"";

	CRect rcControl;
	std::map<int, CNCButton>::iterator itBtn = m_mapBtn.begin();
	for (; itBtn != m_mapBtn.end(); itBtn++)
	{
		CString StrDesc  = itBtn->second.GetDescription();
		CNCButton &btnControl = itBtn->second;
		btnControl.GetRect(rcControl);
		if (rcControl.PtInRect(point))
		{
			if (StrDesc.GetLength() > 0 )
			{
				bShowTip = TRUE;
				StrTipMsg  = StrDesc;
			}
			break;
		}
	}

	if (bShowTip)
	{
		StrTipMsg += "\n";
		m_TipWnd.Show(point, StrTipMsg, L"");
	}
	else
	{
		m_TipWnd.Hide();
	}
}



int  CShortCutMenuToolBar::FindKLineCycleId(unsigned int uiCycle)
{
	int iCycle = INVALID_ID;
	if (BTN_FC_CYCLE_1MIN == uiCycle)
	{
		iCycle = IDM_CHART_KMINUTE;
	}
	else if (BTN_FC_CYCLE_5MIN == uiCycle)
	{
		iCycle = IDM_CHART_KMINUTE5;
	}
	else if (BTN_FC_CYCLE_15MIN == uiCycle)
	{
		iCycle = IDM_CHART_KMINUTE15;
	}
	else if (BTN_FC_CYCLE_30MIN == uiCycle)
	{
		iCycle = IDM_CHART_KMINUTE30;
	}
	else if (BTN_FC_CYCLE_60MIN == uiCycle)
	{
		iCycle = IDM_CHART_KMINUTE60;
	}
	else if (BTN_FC_CYCLE_DAY == uiCycle)
	{
		iCycle = IDM_CHART_KDAY;
	}
	else if (BTN_FC_CYCLE_WEEK == uiCycle)
	{
		iCycle = IDM_CHART_KWEEK;
	}
	else if (BTN_FC_CYCLE_MONTH == uiCycle)
	{
		iCycle = IDM_CHART_KMONTH;
	}
	else if (BTN_FC_CYCLE_MUL_MIN == uiCycle)
	{
		iCycle = IDM_CHART_KMINUTEX;
	}
	else if (BTN_FC_CYCLE_QUARTERLY == uiCycle)
	{
		iCycle = IDM_CHART_KQUARTER;
	}
	else if (BTN_FC_CYCLE_YEAR == uiCycle)
	{
		iCycle = IDM_CHART_KYEAR;
	}
	return iCycle;
}

void CShortCutMenuToolBar::KLineCycleChange(int iKlineType)
{
	int iID = FindKLineCycleId(iKlineType);
	if (INVALID_ID != iID)
	{
		if ( iID >= IDM_CHART_CYCLE_BEGIN && iID <= IDM_CHART_CYCLE_END )
		{	
			CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
			if (!pMain)
			{
				return ;
			}

			CIoViewBase	*pIoView = pMain->FindActiveIoView();
			bool32 bHandled = false;

			// ��������k����ʱ����������ͬ����
			if ( NULL!=pIoView 
				&& ( pIoView->IsKindOf(RUNTIME_CLASS(CIoViewReportArbitrage))
				|| pIoView->IsKindOf(RUNTIME_CLASS(CIoViewKLineArbitrage))
				|| pIoView->IsKindOf(RUNTIME_CLASS(CIoViewTrendArbitrage))) )
			{
				// ������K�ߣ��л�����
				// �����ǰҳ��������K�ߣ���ʹ��ǰҳ���л�����
				// û�У����������K��ҳ��
				CMPIChildFrame *pChartFrame = NULL;


				//CIoViewBase *pIoViewKline = pMain->FindIoViewInFrame(ID_PIC_KLINEARBITRAGE, GetIoViewGroupId());
				CIoViewBase * pIoViewKline = (CIoViewKLine *)pMain->FindIoViewInFrame(ID_PIC_KLINE, NULL, false, false);
				if ( NULL == pIoViewKline )
				{
					CArbitrage arb;
					pMain->OnArbitrageF5(ID_PIC_KLINEARBITRAGE, arb, pIoView);
					//pIoViewKline = pMain->FindIoViewInFrame(ID_PIC_KLINEARBITRAGE, GetIoViewGroupId());
					pIoViewKline = (CIoViewKLine *)pMain->FindIoViewInFrame(ID_PIC_KLINE, NULL, false, false);

				}
				if ( NULL!=pIoViewKline )
				{
					pIoViewKline->KLineCycleChange(iID);
					bHandled = true;
				}
			}
			else
			{
				CIoViewBase* pActiveView =pMain->FindActiveIoView();
				ASSERT(NULL != pActiveView);
				//	��ǰ��ͼ�Ƿ��Ƕ��ͬ��
				{
					CIoViewDuoGuTongLie* pDuoguo = DYNAMIC_DOWNCAST(CIoViewDuoGuTongLie, pActiveView);
					if (NULL != pDuoguo)
					{
						pDuoguo->KLineCycleChange(iID);						
						return;

					}
				}
				
				// ��k�ߣ��л�����
				// �����ǰҳ����K�ߣ���ʹ��ǰҳ���л�����
				// û�У������K��ҳ��
				//CIoViewBase *pIoViewKline = pMain->FindIoViewInFrame(ID_PIC_KLINE, GetIoViewGroupId());
				CIoViewBase * pIoViewKline = (CIoViewKLine *)pMain->FindIoViewInFrame(ID_PIC_KLINE, NULL, false, false);
				const char *pShow = pIoViewKline->GetXmlElementAttrShowTabName();
				if ( NULL == pIoViewKline )
				{
					if (NULL != pMain->m_pNewWndTB)
					{
						//g_bDrawKLineCyc = FALSE;
						pMain->m_pNewWndTB->LoadSystemDefaultCfm(TB_IOCMD_KLINE);
						//pIoViewKline = pMain->FindIoViewInFrame(ID_PIC_KLINE, GetIoViewGroupId());	// ���������ť���л�K�ߵĴ���
						pIoViewKline = (CIoViewKLine *)pMain->FindIoViewInFrame(ID_PIC_KLINE, NULL, false, false);
					}
				}
				if ( NULL!=pIoViewKline )
				{
					pIoViewKline->BringToTop();
					//g_bDrawKLineCyc = TRUE;
					pIoViewKline->KLineCycleChange(iID);
					bHandled = true;
				}
			}
		}
	}
}





void CShortCutMenuToolBar::InitialToolBarImage()
{
	m_mapSCBarImage.clear();
	Image* pImage = NULL;
	int	   iFCId = 0;
	
	iFCId = BTN_FC_REPORT;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_REPORT.png");
	m_mapSCBarImage[iFCId] = pImage;

	iFCId = BTN_FC_KLINE;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_KLINE.png");
	m_mapSCBarImage[iFCId] = pImage;


	iFCId = BTN_FC_TREND;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_TREND.png");
	m_mapSCBarImage[iFCId] = pImage;

	iFCId = BTN_FC_F10;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_F10.png");
	m_mapSCBarImage[iFCId] = pImage;

	iFCId = BTN_FC_NEWS;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_NEWS.png");
	m_mapSCBarImage[iFCId] = pImage;

	iFCId = BTN_FC_USER_BLOCK;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_USER_BLOCK.png");
	m_mapSCBarImage[iFCId] = pImage;

	iFCId = BTN_FC_TRANSACTION;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_TRANSACTION.png");
	m_mapSCBarImage[iFCId] = pImage;

	iFCId = BTN_FC_FOMULAR;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_FOMULAR.png");
	m_mapSCBarImage[iFCId] = pImage;

	iFCId = BTN_FC_DRAW_LINE;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_DRAW_LINE.png");
	m_mapSCBarImage[iFCId] = pImage;

	iFCId = BTN_FC_MUL_STOCK_COL;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_MUL_STOCK_COL.png");
	m_mapSCBarImage[iFCId] = pImage;

	iFCId = BTN_FC_CYCLE_1MIN;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_CYCLE_1MIN.png");
	m_mapSCBarImage[iFCId] = pImage;

	iFCId = BTN_FC_CYCLE_5MIN;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_CYCLE_5MIN.png");
	m_mapSCBarImage[iFCId] = pImage;

	iFCId = BTN_FC_CYCLE_15MIN;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_CYCLE_15MIN.png");
	m_mapSCBarImage[iFCId] = pImage;

	iFCId = BTN_FC_CYCLE_30MIN;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_CYCLE_30MIN.png");
	m_mapSCBarImage[iFCId] = pImage;

	iFCId = BTN_FC_CYCLE_60MIN;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_CYCLE_60MIN.png");
	m_mapSCBarImage[iFCId] = pImage;

	iFCId = BTN_FC_CYCLE_DAY;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_CYCLE_DAY.png");
	m_mapSCBarImage[iFCId] = pImage;

	iFCId = BTN_FC_CYCLE_WEEK;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_CYCLE_WEEK.png");
	m_mapSCBarImage[iFCId] = pImage;

	iFCId = BTN_FC_CYCLE_MONTH;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_CYCLE_MONTH.png");
	m_mapSCBarImage[iFCId] = pImage;

	/*iFCId = BTN_FC_CYCLE_MUL_MIN;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar1.png");
	m_mapSCBarImage[iFCId] = pImage;*/

	iFCId = BTN_FC_CYCLE_QUARTERLY;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_CYCLE_QUARTERLY.png");
	m_mapSCBarImage[iFCId] = pImage;

	iFCId = BTN_FC_CYCLE_YEAR;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_CYCLE_YEAR.png");
	m_mapSCBarImage[iFCId] = pImage;

	iFCId = BTN_FC_SEPARATE_LINE;
	pImage = Image::FromFile(L".//image//leftbar//scBtn//scToolBar_SEPARATE_LINE.png");
	m_mapSCBarImage[iFCId] = pImage;
}


void CShortCutMenuToolBar::InitialToolBarBtns()
{
	int iFcSize = sizeof(g_aShortCutMenuFunction)/sizeof(ST_LineFunctionData);
	for (int i = 0; i < iFcSize; i++)
	{
		CNCButton btnControl;
		if (m_mapSCBarImage.count(g_aShortCutMenuFunction[i].m_uiFcId) > 0)
		{
			if (BTN_FC_SEPARATE_LINE == g_aShortCutMenuFunction[i].m_uiFcId)
			{
				btnControl.CreateButton(g_aShortCutMenuFunction[i].m_strDisName, CRect(0,0,0,0), this, m_mapSCBarImage[g_aShortCutMenuFunction[i].m_uiFcId], 1, g_aShortCutMenuFunction[i].m_uiFcId);
			}
			else
			{
				btnControl.CreateButton(g_aShortCutMenuFunction[i].m_strDisName, CRect(0,0,0,0), this, m_mapSCBarImage[g_aShortCutMenuFunction[i].m_uiFcId], 3, g_aShortCutMenuFunction[i].m_uiFcId);
			}
		}
		else
		{
			btnControl.CreateButton(g_aShortCutMenuFunction[i].m_strDisName, CRect(0,0,0,0), this, NULL, 0, g_aShortCutMenuFunction[i].m_uiFcId);
		}
		btnControl.SetDescription(g_aShortCutMenuFunction[i].m_strDescription);
		/*btnControl.SetTextBkgColor(RGB(57,57,57), RGB(57,57,57), RGB(57,57,57));
		btnControl.SetTextFrameColor(RGB(57,57,57), RGB(57,57,57), RGB(57,57,57));
		btnControl.SetTextColor(RGB(134,154,193), RGB(134,154,193), RGB(134,154,193));*/
		m_mapBtn[g_aShortCutMenuFunction[i].m_uiFcId] = btnControl;
	}
}


int CShortCutMenuToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;

    m_pImgBk = Image::FromFile(L".//image//leftbar//ShortCut.png");	
	m_iToolBarWidth = m_pImgBk->GetWidth();


	m_TipWnd.Create(this);

	InitialToolBarImage();

	InitialToolBarBtns();


	return 0;
}

void CShortCutMenuToolBar::LoadBtnImage() 
{
	m_mapSCBarImage.clear();
	
}


BOOL CShortCutMenuToolBar::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return 1;
//	return CToolBar::OnEraseBkgnd(pDC);
}




BOOL CShortCutMenuToolBar::OnCommand( WPARAM wParam, LPARAM lParam )
{
    int32 iID = (int32)wParam;
    CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	if (!pMain || !pMain->m_pNewWndTB)
	{
		return FALSE;
	}
	


	// ��˵û��Ҫ����һ�㱣����
    std::map<int, CNCButton>::iterator itHome = m_mapBtn.find(iID);
    if (m_mapBtn.end() != itHome)
    {
		int iCmd = itHome->first;
		switch(itHome->first)
		{
		case BTN_FC_REPORT:
			pMain->m_pNewWndTB->DoOpenCfm(_T("�����б�"));
			break;
		case BTN_FC_KLINE:
			pMain->m_pNewWndTB->DoOpenCfm(_T("K�߷���"));
			break;
		case BTN_FC_TREND:
			pMain->m_pNewWndTB->DoOpenCfm(_T("��ʱ����"));
			break;
		case BTN_FC_F10:
			pMain->DoF10();
			break;
		case BTN_FC_NEWS:
			pMain->m_pNewWndTB->DoOpenCfm(_T("��Ѷ"));
			break;
		case BTN_FC_USER_BLOCK:
			pMain->m_pNewWndTB->DoOpenCfm(_T("��ѡ"));
			break;
		case BTN_FC_TRANSACTION:
			pMain->ShowRealTradeDlg();
			break;
		//case BTN_FC_FOMULAR:
		//	break;
		case BTN_FC_DRAW_LINE:
			pMain->OnSelfDraw();
			break;
		case BTN_FC_MUL_STOCK_COL:
			pMain->m_pNewWndTB->DoOpenCfm(_T("���ͬ��"));
			break;
		case BTN_FC_CYCLE_1MIN:
		case BTN_FC_CYCLE_5MIN:
		case BTN_FC_CYCLE_15MIN:
		case BTN_FC_CYCLE_30MIN:
		case BTN_FC_CYCLE_60MIN:
		case BTN_FC_CYCLE_DAY:
		case BTN_FC_CYCLE_WEEK:
		case BTN_FC_CYCLE_MONTH:
		case BTN_FC_CYCLE_MUL_MIN:
		case BTN_FC_CYCLE_QUARTERLY:
		case BTN_FC_CYCLE_YEAR:
			KLineCycleChange(itHome->first);
			break;
		default:
			break;
		}

		if (BTN_FC_FOMULAR == itHome->first)
		{
			CDlgFormularManager dlgFormular;
			dlgFormular.DoModal();
		}

    }

    return CWnd::OnCommand(wParam, lParam);
}


BOOL CShortCutMenuToolBar::PreCreateWindow( CREATESTRUCT& cs )
{
	if( !CToolBar::PreCreateWindow(cs))
		return FALSE;

	m_dwStyle &= ~CBRS_BORDER_ANY; // �ر�����border

	return TRUE;
}


void CShortCutMenuToolBar::OnDestroy()
{
	CToolBar::OnDestroy();
	m_TipWnd.DestroyWindow();
}



/************************************************************************/
/* �������ƣ�TButtonHitTest                                             */
/* ������������ȡ��ǰ������ڵİ�ť										*/
/* ���������point,��ǰ���λ��											*/
/* ����ֵ��  ��ťID														*/
/************************************************************************/
int CShortCutMenuToolBar::TButtonHitTest(CPoint point)
{
	map<int, CNCButton>::iterator iter;
	int iBtnId = INVALID_ID;
	// ���������������а�ť
	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		iBtnId   = btnControl.GetControlId();
		// ��point�Ƿ����ѻ��Ƶİ�ť������
		if (btnControl.PtInButton(point) && btnControl.GetCreate())
		{
			return  iBtnId;
		}
	}

	m_TipWnd.Hide();
	return INVALID_ID;
}



void CShortCutMenuToolBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	int iButton = -1;
	iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		bool32 bCheck = m_mapBtn[iButton].GetCheck();
		if (!bCheck)
		{
			m_mapBtn[iButton].SetCheck(TRUE);
		}
		SetCheckStatus(iButton);
	}
	CToolBar::OnLButtonDown(nFlags, point);
}


void CShortCutMenuToolBar::SetCheckStatus( int iID )
{
	map<int, CNCButton>::iterator iter;
	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
	{
		CNCButton &btn = iter->second;

		if(iID == iter->first )
		{
			if (!iter->second.GetCheck())
			{
				m_mapBtn[iID].SetCheckStatus(TRUE);
			}
		}
		else if (btn.GetCheck())
		{
			btn.SetCheckStatus(FALSE);
		}
	}
}


void CShortCutMenuToolBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	int iButton = -1;
	iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		m_mapBtn[iButton].LButtonUp();
	}
	CToolBar::OnLButtonUp(nFlags, point);
}





int CShortCutMenuToolBar::TButtonHitRect( CPoint point )
{
	// ���������������а�ť
	std::map<int, CNCButton>::iterator iter;
	FOR_EACH(m_mapBtn, iter)    
	{
		CRect rtTool;
		CNCButton &btnControl = iter->second;
		btnControl.GetRect(rtTool);
		// ��point�Ƿ����ѻ��Ƶİ�ť������
		if (btnControl.PtInButton(point) && btnControl.GetCreate())
		{
			//�жϹ�������ť�����Ƿ�����������ص�						
			return btnControl.GetControlId();
		}
	}
	return INVALID_ID;
}



void CShortCutMenuToolBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_PointLast != point)
	{
		m_PointLast = point;
		m_TipWnd.Hide();
		ShowTips();
	}

	//int iButton = TButtonHitRect(point);
	//if (m_iXButtonHovering != INVALID_ID && m_iXButtonHovering != iButton /*&& iButton != INVALID_ID*/)
	//{
	//	if (m_mapBtn.count(m_iXButtonHovering) > 0)
	//	{
	//		m_mapBtn[m_iXButtonHovering].MouseLeave();
	//		m_iXButtonHovering = INVALID_ID;	
	//	}
	//}
	//else
	//{
	//	if (iButton != INVALID_ID )
	//	{
	//		m_mapBtn[iButton].MouseHover();
	//		m_iXButtonHovering = iButton;
	//	}			
	//}


	CToolBar::OnMouseMove(nFlags, point);
}


void CShortCutMenuToolBar::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default
	//if (INVALID_ID != m_iXButtonHovering)
	//{
	//	if(m_mapBtn.count(m_iXButtonHovering) > 0)
	//	{
	//		m_mapBtn[m_iXButtonHovering].MouseLeave();
	//		//m_iXButtonHovering = INVALID_ID;
	//	}
	//}

	CToolBar::OnMouseLeave();
}
