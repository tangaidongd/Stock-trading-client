// CustomProgress.cpp : implementation file
//

#include "stdafx.h"
#include "CustomProgress.h"

#define  BKGROUND_CLR	COLORREF(RGB(26,130,240))   //::GetSysColor(COLOR_3DFACE);
#define  START_CLR		COLORREF(RGB(60, 190,250))
#define  END_CLR		COLORREF(RGB(17,130,146))
#define  TEXT_CLR		COLORREF(RGB(255, 255, 255))

// CCustomProgress

IMPLEMENT_DYNAMIC(CCustomProgress, CProgressCtrl)

CCustomProgress::CCustomProgress()
{
	//��ʼ��
	m_nLower = 0;
	m_nUpper = 100;
	m_nCurrentPosition = 0;
	m_nStep = 10;	

	m_clrStart	= START_CLR;
	m_clrEnd =	 END_CLR;
	m_clrBkGround = BKGROUND_CLR;//::GetSysColor(COLOR_3DFACE);
	m_clrText = TEXT_CLR;
	m_bShowPercent = TRUE;
	m_IsErroMsg = FALSE;
	m_szMsg = "";
}

CCustomProgress::~CCustomProgress()
{
}


BEGIN_MESSAGE_MAP(CCustomProgress, CProgressCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()



// CCustomProgress message handlers

/////////////////////////////////////////////////////////////////////////////
// CCustomProgress message handlers

void CCustomProgress::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	m_nCurrentPosition;
	//�����Ҫ���������
	CRect rectClient;
	GetClientRect(&rectClient);
	float maxWidth((float)m_nCurrentPosition/(float)m_nUpper * (float)rectClient.right);


	//����������ĵ�ǰλ�ò���ȷ���򽥱䵽����ɫ
	if (m_IsErroMsg || m_nCurrentPosition <= m_nLower || m_nCurrentPosition > m_nUpper)
	{
		CRect rect;
		GetClientRect(rect);
		CBrush brush;
		brush.CreateSolidBrush(BKGROUND_CLR);
		dc.FillRect(&rect, &brush);
		VERIFY(brush.DeleteObject());
		
		//��д������Ϣ
		if (m_IsErroMsg)
		{
			dc.SetTextColor(m_clrText);
			dc.SetBkMode(TRANSPARENT);
			dc.DrawText(m_szMsg, &rectClient, DT_VCENTER |  DT_CENTER | DT_SINGLELINE);
		}
		return;
	}

	//��������ɫ
	DrawGradient(&dc, rectClient, (int)maxWidth);

	//�����Ҫ��ʾ�ٷ���������ʾ
	if (m_bShowPercent)
	{
		CString percent;
		percent.Format(_T("%.0f%%"), 100.0f*(float)m_nCurrentPosition/(float)m_nUpper);
		dc.SetTextColor(m_clrText);
		dc.SetBkMode(TRANSPARENT);
		dc.DrawText(percent, &rectClient, DT_VCENTER |  DT_CENTER | DT_SINGLELINE);
	}
}




int CCustomProgress::SetErroMsg(char*pMsg)
{
	m_IsErroMsg = TRUE;
	m_szMsg = pMsg;
	UpdateWindow();
	InvalidateRect(NULL);
	return 0;
}
//���ý�������Χ
void CCustomProgress:: SetRange(int nLower, int nUpper)
{
	m_nLower = nLower;
	m_nUpper = nUpper;
	m_nCurrentPosition = nLower;
	CProgressCtrl::SetRange(nLower, nUpper);
}

//���ý�������λ��
int CCustomProgress:: SetPos(int nPos)
{
	m_nCurrentPosition = nPos;
	int nTemp = CProgressCtrl::SetPos(nPos);
	UpdateWindow();
	InvalidateRect(NULL);
	return 0;
}

//���ý������Ĳ���
int CCustomProgress:: SetStep(int nStep)
{
	m_nStep = nStep;
	return (CProgressCtrl::SetStep(nStep));
}

int CCustomProgress:: StepIt(void)
{
	m_nCurrentPosition += m_nStep;
	return (CProgressCtrl::StepIt());
}



//���ʵ����򻭽�����ɫ
void CCustomProgress::DrawGradient(CPaintDC *pDC, const RECT &rectClient, const int &nMaxWidth)
{
	RECT rectFill;			   
	float fStep;              
	CBrush brush;			


	CMemDC111 memDC(pDC);

	//�ҵ���ʵ��ɫ�ͽ�����ɫ֮��������ɫֵ���������䲽����
	int r, g, b;							
	float rStep, gStep, bStep;		

	r = (GetRValue(m_clrEnd) - GetRValue(m_clrStart));
	g = (GetGValue(m_clrEnd) - GetGValue(m_clrStart));
	b =  (GetBValue(m_clrEnd) - GetBValue(m_clrStart));

	int nSteps = max(abs(r), max(abs(g), abs(b)));

	fStep = (float)rectClient.right / (float)nSteps;

	//����ÿ����ɫ�Ĳ���
	rStep = r/(float)nSteps;
	gStep = g/(float)nSteps;
	bStep = b/(float)nSteps;

	r = GetRValue(m_clrStart);
	g = GetGValue(m_clrStart);
	b = GetBValue(m_clrStart);


	// ��ʼ�����ɫ
	for (int iOnBand = 0; iOnBand < nSteps; iOnBand++) 
	{

		::SetRect(&rectFill,
			(int)(iOnBand * fStep),       
			0,									 
			(int)((iOnBand+1) * fStep),         
			rectClient.bottom+1);			

		VERIFY(brush.CreateSolidBrush(RGB(r+rStep*iOnBand, g + gStep*iOnBand, b + bStep *iOnBand)));
		memDC.FillRect(&rectFill,&brush);
		VERIFY(brush.DeleteObject());

		if (rectFill.right > nMaxWidth)
		{
			::SetRect(&rectFill, rectFill.right, 0, rectClient.right, rectClient.bottom);
			VERIFY(brush.CreateSolidBrush(m_clrBkGround));
			memDC.FillRect(&rectFill, &brush);
			VERIFY(brush.DeleteObject());
			return;
		}
	}
}

BOOL CCustomProgress::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}
