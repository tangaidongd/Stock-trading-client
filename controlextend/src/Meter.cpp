// Meter.cpp : implementation file
//

#include "stdafx.h"
#include "Meter.h"
#include "math.h"
#include "memdc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PT_NUM 50
/////////////////////////////////////////////////////////////////////////////
// CMeter
IMPLEMENT_DYNAMIC(CMeter, CStatic)

CMeter::CMeter()
{
	m_nStartAngleDeg = 225;
	m_nEndAngleDeg = 315;
	m_nTicks = 10;
	m_nSubTicks = 5;
	m_dMaxValue = 100.0;
	m_dMinValue = 0.0;
	m_dCurrentValue = 50.0;
	m_nScaleDecimals = 0;
	m_nValueDecimals = 0;
	
	m_strUnits = _T("Volts");
	m_bColorTick = FALSE;


	m_rectCtrl = CRect(0,0,0,0);


	// 颜色表格
	m_colorTable[0] = RGB(255, 255,   0);
	m_colorTable[1] = RGB(  0, 255,   0);
	m_colorTable[2] = RGB(255,   0,   0);
	m_colorTable[3] = RGB(255,183, 34);
	m_colorTable[4] = RGB(255, 92, 93);


	m_colorNeedle	 = RGB(255, 0, 0);
	m_colorHighlight = RGB(255,0,0);
	m_colorShadow	 = RGB(255,0,0);
	m_colorText		 = RGB(255,0,0);	
	m_colorBKWindows = RGB(0,0,0);
	m_colorScaleText = RGB(255,255,255);
	m_colorInnerPanel = RGB(70,120,120);



}

CMeter::~CMeter()
{
	if (m_dcBackground.GetSafeHdc())
	{
		m_dcBackground.DeleteDC();
	}

	if (m_bitmapBackground.GetSafeHandle())
	{
		m_bitmapBackground.DeleteObject();
	}
}


BEGIN_MESSAGE_MAP(CMeter, CStatic)
	//{{AFX_MSG_MAP(CMeter)
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeter message handlers

void CMeter::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// 获得控件区域
	GetClientRect (&m_rectCtrl);
	// 保留原始大小
	CRect cBupRect(&m_rectCtrl);

	CMemDC memDC(&dc, &m_rectCtrl);

	// 计算显示表盘出正方形，避免变形
	int iLengthDif= m_rectCtrl.Height() - m_rectCtrl.Width();
	if (iLengthDif > 0)
	{
		m_rectCtrl.top     += iLengthDif/2;
		m_rectCtrl.bottom  -= iLengthDif/2;
	}
	else if (iLengthDif < 0)
	{
		iLengthDif = abs(iLengthDif);
		m_rectCtrl.left   += iLengthDif/2;
		m_rectCtrl.right  -= iLengthDif/2;
	}


	// 选取圆盘边框半径
	m_nRadiusFrame = max(m_rectCtrl.Height(), m_rectCtrl.Width())*9/18;

	// 获得仪表盘中心点
	m_ptMeterCenter = m_rectCtrl.CenterPoint();
	m_ptMeterCenter.y += m_nRadiusFrame/10;
	
	//绘制仪表盘
	if(m_dcBackground.GetSafeHdc()== NULL|| (m_bitmapBackground.m_hObject == NULL))
	{
		m_dcBackground.CreateCompatibleDC(&dc);
		m_bitmapBackground.CreateCompatibleBitmap(&dc, cBupRect.Width(),cBupRect.Height()) ;
		m_pBitmapOldBackground = m_dcBackground.SelectObject(&m_bitmapBackground) ;
		DrawMeterBackground(&m_dcBackground, m_rectCtrl);

	}
	memDC.BitBlt(0, 0, cBupRect.Width(), cBupRect.Height(), 
			           &m_dcBackground, 0, 0, SRCCOPY) ;

	// 绘制指针
	DrawNeedle(&memDC);

	DrawNode(&memDC);

	DrawValue(&memDC);
	// Do not call CStatic::OnPaint() for painting messages
}

void CMeter::DrawMeterBackground(CDC *pDC, CRect &rect)
{
	int nInnerRadius = m_nRadiusFrame*8/10;	// 内圆弧半径

	m_nCenterRadius = m_nRadiusFrame/20;	// 中心园半径大小

	int nFrame = m_nRadiusFrame/18;			// 边框厚度

	double dstepTickDeg = (360.0+m_nStartAngleDeg-m_nEndAngleDeg)/(m_nTicks*m_nSubTicks);	// 刻度步进角度

	int nSubTickR = nInnerRadius+(m_nRadiusFrame-2*nFrame-nInnerRadius)/2;

	double dDeg = (m_nStartAngleDeg+360.0-m_nEndAngleDeg)/(TABNUM*PT_NUM);
	 

	CRect rectPanel,rectInnerPanel;
	CPen penDraw, *pPenOld;
	CFont *pFontOld;
	POINT ptStart, ptEnd, ptInnerStart, ptInnerEnd;	
	CPoint pointInner[BOUNDARY_POINTS], ptGroup1[PT_NUM*TABNUM+1], ptGroup2[PT_NUM*TABNUM+1];
	CPoint ptRgn[PT_NUM*2+2];
	CPoint pttemp;
	CString strtemp;
	double dRadPerDeg;
	double dTickAngleRad;
	double dTemp;
	int nRef = 0;
	int nTickAngle;	
	int nHeight;	// 字体大小
	double dtempangle;
	
	// 计算起始角终止角弧度
	dRadPerDeg = 4.0*atan(1.0)/180.0;
	m_dLeftAngleRad = (m_nStartAngleDeg-180.0)*dRadPerDeg;
	m_dRightAngleRad = (m_nEndAngleDeg-360.0)*dRadPerDeg;
	
	// 计算圆弧起始终止点及区域
	ptStart.x = m_ptMeterCenter.x-(int)(m_nRadiusFrame*cos(m_dLeftAngleRad));
	ptStart.y = m_ptMeterCenter.y+(int)(m_nRadiusFrame*sin(m_dLeftAngleRad));
	ptEnd.x = m_ptMeterCenter.x+(int)(m_nRadiusFrame*cos(-m_dRightAngleRad));
	ptEnd.y = m_ptMeterCenter.y+(int)(m_nRadiusFrame*sin(-m_dRightAngleRad));
	
	// 获取点的位置
	for(int i=0; i<=PT_NUM*TABNUM; i++)
	{
		ptGroup1[i].x = m_ptMeterCenter.x + (int)((m_nRadiusFrame-nFrame)*cos((m_nStartAngleDeg-i*dDeg)*dRadPerDeg));
		ptGroup1[i].y = m_ptMeterCenter.y - (int)((m_nRadiusFrame-nFrame)*sin((m_nStartAngleDeg-i*dDeg)*dRadPerDeg));
		ptGroup2[i].x = m_ptMeterCenter.x + (int)(m_nRadiusFrame*8*cos((m_nStartAngleDeg-i*dDeg)*dRadPerDeg)/10);
		ptGroup2[i].y = m_ptMeterCenter.y - (int)(m_nRadiusFrame*8*sin((m_nStartAngleDeg-i*dDeg)*dRadPerDeg)/10);
	}


	// 绘制内圈
	ptInnerStart.x = m_ptMeterCenter.x-(int)(nInnerRadius*cos(m_dLeftAngleRad));
	ptInnerStart.y = m_ptMeterCenter.y+(int)(nInnerRadius*sin(m_dLeftAngleRad));
	ptInnerEnd.x = m_ptMeterCenter.x+(int)(nInnerRadius*cos(-m_dRightAngleRad));
	ptInnerEnd.y = m_ptMeterCenter.y+(int)(nInnerRadius*sin(-m_dRightAngleRad));
	
	int iInnerRadius = m_nRadiusFrame*5/10;
	rectInnerPanel.SetRect(m_ptMeterCenter.x-iInnerRadius, m_ptMeterCenter.y-iInnerRadius,
						m_ptMeterCenter.x+iInnerRadius,m_ptMeterCenter.y+iInnerRadius );
	penDraw.DeleteObject();
	penDraw.CreatePen(PS_SOLID, 2, m_colorInnerPanel);
	pPenOld = pDC->SelectObject(&penDraw);
	pDC->Arc(&rectInnerPanel, ptInnerEnd, ptInnerStart);
	pDC->SelectObject(pPenOld);	
	

	// 计算刻度点,避免不能整除引起较大误差*100
	for(nTickAngle=m_nStartAngleDeg*100; nTickAngle>=(m_nEndAngleDeg-360)*100; nTickAngle-=(int)(dstepTickDeg*100))
	{
		// 转换成弧度
		dTickAngleRad = (double)nTickAngle/100*dRadPerDeg;	
		// 确定外圈坐标
		// 确定x坐标
		dTemp = m_ptMeterCenter.x + (m_nRadiusFrame-2*nFrame)*cos(dTickAngleRad);
		m_pointBoundary[nRef].x = ROUND(dTemp);
		// 确定y坐标
		dTemp = m_ptMeterCenter.y - (m_nRadiusFrame-2*nFrame)*sin(dTickAngleRad);
		m_pointBoundary[nRef].y = ROUND(dTemp);
		
		// 确定刻度点(主刻度和子刻度)
		//主刻度及文本标注点
		if(nRef%m_nSubTicks == 0)
		{
			dTemp = m_ptMeterCenter.x + nInnerRadius*cos(dTickAngleRad);
			pointInner[nRef].x = ROUND(dTemp);
			dTemp = m_ptMeterCenter.y - nInnerRadius*sin(dTickAngleRad);
			pointInner[nRef].y = ROUND(dTemp);
		}
		// 子刻度
		else
		{

			int nSubTickR = nInnerRadius+(m_nRadiusFrame-1*nFrame-nInnerRadius)/2;

			dTemp = m_ptMeterCenter.x + nSubTickR*cos(dTickAngleRad);
			pointInner[nRef].x = ROUND(dTemp);
			dTemp = m_ptMeterCenter.y - nSubTickR*sin(dTickAngleRad);
			pointInner[nRef].y = ROUND(dTemp);
		}
		nRef++ ;
	}
	// 多边形区域
	m_rgnBoundary.DeleteObject() ;
	m_rgnBoundary.CreatePolygonRgn(m_pointBoundary, nRef, ALTERNATE);
	
	m_rectValue.top = m_ptMeterCenter.y + m_nRadiusFrame/5;
	m_rectValue.bottom = m_ptMeterCenter.y + m_nRadiusFrame/2;
	m_rectValue.left = m_ptMeterCenter.x - m_nRadiusFrame/2;
	m_rectValue.right = m_ptMeterCenter.x + m_nRadiusFrame/2;

	// 绘制刻度
	penDraw.DeleteObject();
	penDraw.CreatePen(PS_SOLID, 2, RGB(255,255,255));
	pPenOld = pDC->SelectObject(&penDraw);
	for(int i=0; i<nRef; i++)
	{
		pDC->MoveTo(m_pointBoundary[i]);
		pDC->LineTo(pointInner[i]);
	}
	pDC->SelectObject(pPenOld);	

	// 刻度标号
	// 确定字体大小
	nHeight = m_nRadiusFrame/6;
	m_font.CreateFont(nHeight, 0, 0, 0, 400, 
							FALSE, FALSE, 0, ANSI_CHARSET,
							OUT_DEFAULT_PRECIS, 
							CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY, 
							DEFAULT_PITCH|FF_SWISS, L"Arial");

	pFontOld = pDC->SelectObject(&m_font);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_colorScaleText);
	for(int i=0; i<=m_nTicks; i++)
	{	
		dtempangle = m_nStartAngleDeg-i*m_nSubTicks*dstepTickDeg;
		strtemp.Format(L"%.*lf", m_nScaleDecimals, (m_dMinValue+(m_dMaxValue-m_dMinValue)*i/m_nTicks));

		if(dtempangle>190)
		{
			pDC->SetTextAlign(TA_BOTTOM|TA_LEFT);
			pDC->TextOut(pointInner[m_nSubTicks*i].x, pointInner[m_nSubTicks*i].y+nHeight/2, strtemp);
		}
		
		else if(dtempangle>170)
		{
			pDC->SetTextAlign(TA_BASELINE|TA_LEFT);
			pDC->TextOut(pointInner[m_nSubTicks*i].x, pointInner[m_nSubTicks*i].y+nHeight/3, strtemp);
		}
		else if(dtempangle>135)
		{
			pDC->SetTextAlign(TA_BASELINE|TA_LEFT);
			pDC->TextOut(pointInner[m_nSubTicks*i].x, pointInner[m_nSubTicks*i].y+nHeight/2, strtemp);
		}
		else if(dtempangle>100)
		{
			pDC->SetTextAlign(TA_TOP|TA_LEFT);
			pDC->TextOut(pointInner[m_nSubTicks*i].x-nHeight/4, pointInner[m_nSubTicks*i].y-nHeight/8, strtemp);
		}

		else if(dtempangle>80)
		{
			pDC->SetTextAlign(TA_TOP|TA_CENTER);
			pDC->TextOut(pointInner[m_nSubTicks*i].x, pointInner[m_nSubTicks*i].y, strtemp);
		}
		else if(dtempangle>45)
		{
			pDC->SetTextAlign(TA_BOTTOM|TA_RIGHT);
			pDC->TextOut(pointInner[m_nSubTicks*i].x+nHeight/3, pointInner[m_nSubTicks*i].y+nHeight, strtemp);
		}
		else if(dtempangle>10)
		{
			pDC->SetTextAlign(TA_RIGHT|TA_BASELINE);
			pDC->TextOut(pointInner[m_nSubTicks*i].x, pointInner[m_nSubTicks*i].y+nHeight/2, strtemp);
		}
		else if(dtempangle>-10)
		{
			pDC->SetTextAlign(TA_RIGHT|TA_BASELINE);
			pDC->TextOut(pointInner[m_nSubTicks*i].x, pointInner[m_nSubTicks*i].y+nHeight/3, strtemp);
		}
		else 
		{
			pDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
			pDC->TextOut(pointInner[m_nSubTicks*i].x, pointInner[m_nSubTicks*i].y+nHeight/2, strtemp);
		}
	}
	pDC->SelectObject(pFontOld);

}

void CMeter::ReconstructControl()
{
	if ((m_pBitmapOldBackground) && 
		  (m_bitmapBackground.GetSafeHandle()) && 
			(m_dcBackground.GetSafeHdc()))
	{
			m_dcBackground.SelectObject(m_pBitmapOldBackground);
			m_dcBackground.DeleteDC() ;
			m_bitmapBackground.DeleteObject();
	}

	Invalidate ();
}

void CMeter::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	ReconstructControl() ;
}

void CMeter::DrawNeedle(CDC *pDC)
{
	int nResult;
	double dRadPerDeg = 4.0*atan(1.0)/180.0;
	double dAngleDeg;
	double dAngleRad ;
	double dTemp ;
	CBrush brushFill, *pBrushOld ;
	CPen penDraw, *pPenOld ;
	CPoint pointNeedle[4] ;	// 指针由四边形组成


	// 计算角度并限定指针走的角度
	dAngleDeg = m_nStartAngleDeg-(360.0+m_nStartAngleDeg-m_nEndAngleDeg)
		*(m_dCurrentValue-m_dMinValue)/(m_dMaxValue-m_dMinValue);
	dAngleDeg = min(dAngleDeg, m_nStartAngleDeg);
	dAngleDeg = max(dAngleDeg, m_nEndAngleDeg-360.0);
	dAngleRad = dAngleDeg*dRadPerDeg;

	// 计算三角形底边两个点
	pointNeedle[0].x = m_ptMeterCenter.x - (int)(m_nCenterRadius*10*sin(dAngleRad)/8);
	pointNeedle[0].y = m_ptMeterCenter.y - (int)(m_nCenterRadius*10*cos(dAngleRad)/8);
	pointNeedle[2].x = m_ptMeterCenter.x + (int)(m_nCenterRadius*10*sin(dAngleRad)/8);
	pointNeedle[2].y = m_ptMeterCenter.y + (int)(m_nCenterRadius*10*cos(dAngleRad)/8);
	
	// 计算指针顶部坐标
	dTemp = m_ptMeterCenter.x + m_nRadiusFrame*cos(dAngleRad)*95/100;
	pointNeedle[1].x = ROUND(dTemp);
	dTemp = m_ptMeterCenter.y - m_nRadiusFrame*sin(dAngleRad)*95/100;
	pointNeedle[1].y = ROUND(dTemp);
	// 计算指针尾部坐标
	dTemp = m_ptMeterCenter.x - m_nRadiusFrame*cos(dAngleRad)/6;
	pointNeedle[3].x = ROUND(dTemp);
	dTemp = m_ptMeterCenter.y + m_nRadiusFrame*sin(dAngleRad)/6;
	pointNeedle[3].y = ROUND(dTemp);

	pDC->SelectClipRgn(&m_rgnBoundary);

	brushFill.CreateSolidBrush(m_colorNeedle);
	penDraw.CreatePen(PS_SOLID, 1, m_colorNeedle);

	pPenOld = pDC->SelectObject(&penDraw) ;
	pBrushOld = pDC->SelectObject(&brushFill) ;

	// 绘制指针
	pDC->Polygon(pointNeedle, 4);

	nResult = pDC->SelectClipRgn(NULL);

	pDC->SelectObject(pPenOld);
	pDC->SelectObject(pBrushOld);

	 // 立体感处理
	if(dAngleDeg>90)
	{
		penDraw.DeleteObject();
		penDraw.CreatePen(PS_SOLID, 2, m_colorShadow);
		pPenOld = pDC->SelectObject(&penDraw);
		pDC->MoveTo(pointNeedle[1]);
		pDC->LineTo(pointNeedle[0]);
		pDC->LineTo(pointNeedle[3]);
		pDC->SelectObject(pPenOld);
		
		penDraw.DeleteObject();
		penDraw.CreatePen(PS_SOLID, 1, m_colorHighlight);
		pPenOld = pDC->SelectObject(&penDraw);
		pDC->MoveTo(pointNeedle[1]);
		pDC->LineTo(pointNeedle[2]);
		pDC->LineTo(pointNeedle[3]);
		pDC->SelectObject(pPenOld);
	}
	else
	{
		penDraw.DeleteObject();
		penDraw.CreatePen(PS_SOLID, 2, m_colorShadow);
		pPenOld = pDC->SelectObject(&penDraw);
		pDC->MoveTo(pointNeedle[1]);
		pDC->LineTo(pointNeedle[2]);
		pDC->LineTo(pointNeedle[3]);
		pDC->SelectObject(pPenOld);
		
		penDraw.DeleteObject();
		penDraw.CreatePen(PS_SOLID, 1, m_colorHighlight);
		pPenOld = pDC->SelectObject(&penDraw);
		pDC->MoveTo(pointNeedle[1]);
		pDC->LineTo(pointNeedle[0]);
		pDC->LineTo(pointNeedle[3]);
		pDC->SelectObject(pPenOld);
	}
}

void CMeter::UpdateNeedle(double dValue)
{
	m_dCurrentValue = dValue;
	Invalidate();
}

void CMeter::SetNeedleColor(COLORREF colorNeedle)
{
	m_colorNeedle = colorNeedle ;
	ReconstructControl() ;
}


void CMeter::SetRange(double dMin, double dMax)
{
	m_dMaxValue = dMax ;
	m_dMinValue = dMin ;
	ReconstructControl() ;
}


void CMeter::SetScaleDecimals(int nDecimals)
{
	m_nScaleDecimals = nDecimals ;
	ReconstructControl() ;
}

void CMeter::SetUnits(CString &strUnits)
{
	m_strUnits = strUnits ;
	ReconstructControl() ;
}

void CMeter::SetValueDecimals(int nDecimals)
{
	m_nValueDecimals = nDecimals ;
	ReconstructControl() ;
}





void CMeter::DrawNode(CDC *pDC)
{
	CPen penDraw, *pPenOld;
	COLORREF cEdge, cMiddle, cNode;
	cMiddle = RGB(255, 255, 255);
	cEdge = RGB(0, 0, 0);
	for(int i=m_nCenterRadius*3/4; i>=0; i--)
	{
		cNode = RGB((GetRValue(cEdge)-GetRValue(cMiddle))*((float)i)*4/(3*m_nCenterRadius)+GetRValue(cMiddle),
			(GetGValue(cEdge)-GetGValue(cMiddle))*((float)i)*4/(3*m_nCenterRadius)+GetGValue(cMiddle),
			(GetBValue(cEdge)-GetBValue(cMiddle))*((float)i)*4/(3*m_nCenterRadius)+GetBValue(cMiddle));
	
		penDraw.DeleteObject();
		penDraw.CreatePen(PS_SOLID, 1, cNode);
		pPenOld = pDC->SelectObject(&penDraw);
		pDC->Arc(m_ptMeterCenter.x-i, m_ptMeterCenter.y-i,m_ptMeterCenter.x+i,m_ptMeterCenter.y+i,
			m_ptMeterCenter.x-i,m_ptMeterCenter.y,m_ptMeterCenter.x-i,m_ptMeterCenter.y);
		pDC->SelectObject(pPenOld);
	}
}

void CMeter::SetColorTick(BOOL bColorTick)
{
	m_bColorTick = bColorTick;
	ReconstructControl();
}


void CMeter::DrawValue(CDC *pDC)
{
	int nHeight;
	CPoint pttemp;
	CString strtemp;
	CFont *pFontOld;

	//  数值显示
	nHeight = m_nRadiusFrame/5;
	pttemp = m_rectValue.CenterPoint();
	strtemp.Format(L"%.*lf%%", m_nValueDecimals, m_dCurrentValue); 
	m_font.DeleteObject() ;
	m_font.CreateFont (nHeight, 0, 0, 0, 400,
						FALSE, FALSE, 0, ANSI_CHARSET,
						OUT_DEFAULT_PRECIS, 
						CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY, 
						DEFAULT_PITCH|FF_SWISS, L"Arial") ;
	pFontOld = pDC->SelectObject(&m_font);
	pDC->SetBkColor(TRANSPARENT);
	pDC->SetBkColor(m_colorBKWindows);
	pDC->SetTextColor(m_colorText);
	pDC->SetTextAlign(TA_TOP|TA_CENTER);
	//pDC->TextOut(pttemp.x, pttemp.y, m_strUnits);
	pDC->TextOut(pttemp.x, pttemp.y+nHeight, strtemp);
	// 恢复字体和背景色
	pDC->SelectObject(pFontOld);
}

void CMeter::SetTicks(int nTicks)
{
	m_nTicks = nTicks;
	ReconstructControl();
}

void CMeter::SetSubTicks(int nSubTicks)
{
	m_nSubTicks = nSubTicks;
	ReconstructControl();
}

void CMeter::SetAngleRange(int nStartAngleDeg, int nEndAngleDeg)
{
	m_nStartAngleDeg = nStartAngleDeg;
	m_nEndAngleDeg = nEndAngleDeg;
	ReconstructControl();
}
