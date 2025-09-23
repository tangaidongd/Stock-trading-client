// dlgsystemface.cpp : implementation file
//
#include "StdAfx.h"
#include "dlgsystemface.h"
#include "facescheme.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GROUPCOLORS 20

/////////////////////////////////////////////////////////////////////////////
// CDlgSystemFace dialog

CDlgSystemFace::CDlgSystemFace(CWnd* pParent /*=NULL*/)
: CDialog(CDlgSystemFace::IDD, pParent)
{
	m_CurrentColor  = 0x12345678;
	m_pIoViewActive = NULL;
	m_iRadio = 0;
	m_iStyleIndexBk = 0;
	m_iStyleIndex = 0;

	for(int i=0; i<ESCCount; i++)
	{ 
		m_aSysColor[i] = RGB(0,0,0);
		m_aSysColorBk[i] = RGB(0,0,0);
	}
}

void CDlgSystemFace::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSystemFace)
	DDX_Control(pDX, IDC_STATIC_PREVIEW, m_CtrlStaticPreView);
	DDX_Control(pDX, IDC_STATIC_COLOR, m_CtrlStaticColor);
	DDX_Control(pDX, IDC_STATIC_SKYBLUE, m_CtrlStaticColorSkyBlue);
	DDX_Control(pDX, IDC_STATIC_PINK, m_CtrlStaticColorPink);
	DDX_Control(pDX, IDC_STATIC_CHARTAXISLINE, m_CtrlStaticChartAxisLine);
	DDX_Control(pDX, IDC_STATIC_NAVYBLUE, m_CtrlStaticColorNavyBlue);
	DDX_Control(pDX, IDC_STATIC_DODGERBLUE, m_CtrlStaticColorDodgerBlue);
	DDX_Control(pDX, IDC_STATIC_GREENYELLOW, m_CtrlStaticColorGreenYellow);
	DDX_Control(pDX, IDC_STATIC_FALL, m_CtrlStaticKLineFall);
	DDX_Control(pDX, IDC_STATIC_PURPLE, m_CtrlStaticColorPurple);
	DDX_Control(pDX, IDC_STATIC_TEXT, m_CtrlStaticText);
	DDX_Control(pDX, IDC_STATIC_KEEP, m_CtrlStaticKLineKeep);
	DDX_Control(pDX, IDC_STATIC_GRIDLINE, m_CtrlStaticGridLine);
	DDX_Control(pDX, IDC_STATIC_AMOUNT, m_CtrlStaticAmount);
	DDX_Control(pDX, IDC_STATIC_VOLUME, m_CtrlStaticVolume);
	DDX_Control(pDX, IDC_STATIC_MAGENTA, m_CtrlStaticColorMagenta);
	DDX_Control(pDX, IDC_STATIC_CADETBLUE, m_CtrlStaticColorCadetblue);
	DDX_Control(pDX, IDC_STATIC_BACKGROUND, m_CtrlStaticBackground);
	DDX_Control(pDX, IDC_STATIC_BLUE, m_CtrlStaticColorBlue);
	DDX_Control(pDX, IDC_STATIC_GRIDSELECTED, m_CtrlStaticGridSelected);
	DDX_Control(pDX, IDC_STATIC_YELLOW, m_CtrlStaticColorYellow);
	DDX_Control(pDX, IDC_STATIC_RISE, m_CtrlStaticKLineRise);
	DDX_Control(pDX, IDC_LIST1, m_CtrlList);
	DDX_Control(pDX, IDC_COMBO1, m_CtrlCombo);
	DDX_Radio(pDX, IDC_RADIO1, m_iRadio);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgSystemFace, CDialog)
//{{AFX_MSG_MAP(CDlgSystemFace)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnButtonColor)
	ON_BN_CLICKED(IDC_BUTTON_FONT_RESET, OnButtonFontReset)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_MESSAGE(UM_SETCOLOR,OnSetColor)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON_SET, OnButtonSet)
	ON_BN_CLICKED(IDC_BUTTON_BIGFONT, OnButtonBigfont)
	ON_BN_CLICKED(IDC_BUTTON_NORMALFONT, OnButtonNormalfont)
	ON_BN_CLICKED(IDC_BUTTON_SMALLFONT, OnButtonSmallfont)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSystemFace message handlers

BOOL CDlgSystemFace::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( NULL == m_pIoViewActive )
	{
		//ASSERT(0);
		return TRUE;
	}

	for ( int iIndex = 0; iIndex < CFaceScheme::GetSysColorObjectCount() ; iIndex++)
	{		
		m_CtrlList.InsertString(iIndex,CFaceScheme::GetSysColorObject(iIndex)->m_StrName);
	}
	
// 	for ( int iCount = 0; iCount < CFaceScheme::GetSysStyleArrayCount() ; iCount++)
// 	{
// 		m_CtrlCombo.InsertString(iCount,CFaceScheme::GetSysStyleArray(iCount));
// 	}
	const CFaceScheme::StyleDataArray &aStyles = CFaceScheme::Instance()->GetStyleDataArray();
	for ( int iCount = 0; iCount < aStyles.GetSize() ; iCount++)
	{
		m_CtrlCombo.InsertString(iCount, aStyles[iCount].m_StrSysleName);
	}
	
	m_CtrlCombo.SetCurSel(0);
	m_CtrlList.SetCurSel(0);
	
	// 色块的上色
	m_CtrlStaticBackground.SetColor(0x00000000);					// 1:ESCBackground 	
	m_CtrlStaticText.SetColor(0x00aaaaaa);							// 2:ESCText
	m_CtrlStaticChartAxisLine.SetColor(0x20e7b);					// 3:ESCChartAxisLine 
	m_CtrlStaticKLineRise.SetColor(0x001010d0 );					// 4:ESCKLineRise 
	m_CtrlStaticKLineKeep.SetColor(0x00d0d0d0);						// 5:ESCKLineKeep 
	m_CtrlStaticKLineFall.SetColor(0x0010d010);						// 6:ESCKLineFall 		
	m_CtrlStaticVolume.SetColor(0x0010d0d0);						// 10:ESCVolume 
	m_CtrlStaticAmount.SetColor(0x00d0d010);						// 11:ESCAmount
	m_CtrlStaticGridLine.SetColor(0x00606060);						// 12:ESCGridLine
	m_CtrlStaticGridSelected.SetColor(0x00d05050);					// 13:ESCGridSelected


	// 清空所有色块上的选中标记
	{
		CWnd * pWndGridLine  = GetDlgItem(IDC_STATIC_GRIDLINE);
		int32 iCount = 0;
		
		while ( NULL != pWndGridLine )
		{
			CStaticSetColor * pStaticSetColor = (CStaticSetColor *)pWndGridLine;
			pStaticSetColor->SetDefaultText();
			pWndGridLine = GetNextDlgGroupItem(pWndGridLine,false);		
			iCount++;		
			if (iCount >= GROUPCOLORS)
			{
				break;
			}
		}
	}
	
	
 	m_CtrlStaticColorYellow.SetColor(RGB(255,255,0));
 	m_CtrlStaticColorBlue.SetColor(RGB(0,0,255));
 	m_CtrlStaticColorMagenta.SetColor(RGB(255,0,255));
 	m_CtrlStaticColorCadetblue.SetColor(RGB(95 ,158 ,160));
 	m_CtrlStaticColorPurple.SetColor(RGB(104 ,34 ,139));
 	m_CtrlStaticColorGreenYellow.SetColor(RGB(173 ,255 ,47));
 	m_CtrlStaticColorDodgerBlue.SetColor(RGB(30 ,144 ,255 ));
 	m_CtrlStaticColorNavyBlue.SetColor(RGB(0 ,0 ,128 ));
 	m_CtrlStaticColorSkyBlue.SetColor(RGB(135 ,206 ,255 ));
 	m_CtrlStaticColorPink.SetColor(RGB(255, 192 ,203));

	//
	memcpyex(m_aSysColor, m_pIoViewActive->m_aIoViewColor, sizeof(m_pIoViewActive->m_aIoViewColor));
	m_aSysColorSave.Copy(m_pIoViewActive->m_aIoViewColorSave);
	//m_iStyleIndex = pIoViewBase->m_iStyleIndex;
	m_iStyleIndex = 1;
	
	// BackUp data
	memcpyex(m_aSysColorBk,m_aSysColor,sizeof(m_aSysColor));
	m_aSysColorSaveBk.Copy(m_aSysColorSave);
	m_iStyleIndexBk = m_iStyleIndex;

	// 字体
// 	memcpyex(m_aSysFont,m_pIoViewActive->m_aIoViewFont,sizeof(m_pIoViewActive->m_aIoViewFont));
// 	for (int32 i = 0 ;i < ESFCount ; i++)
// 	{
// 		m_aSysFont[i].m_Font.DeleteObject();
// 		bool32 bSuccess = m_aSysFont[i].m_Font.CreateFontIndirect(&(m_aSysFont[i].m_stLogfont));
// 	}

	int32 i = 0;
	for ( i=0; i < ESFCount ; i++ )
	{
		m_aSysFont[i] = m_pIoViewActive->m_aIoViewFont[i];
	}
	
	m_aSysFontSave.Copy(m_pIoViewActive->m_aIoViewFontSave);


	// 得到绘图区域

	GetDlgItem(IDC_STATIC_PREVIEW)->GetWindowRect(&m_rectPreView);
	ScreenToClient(&m_rectPreView);
	m_rectPreView.top += 10;
	
	// 刚进来时色块的显示
  	CWnd * pWnd  = GetDlgItem(IDC_STATIC_GRIDLINE);
  	
  	while ( NULL != pWnd )
  	{
  		CStaticSetColor * pStaticSetColor = (CStaticSetColor *)pWnd;
  		if (pStaticSetColor->m_color == m_aSysColor[0])
  		{			
  			pStaticSetColor->SetText(m_aSysColor[0]);
  			break;
  		}
  		pWnd = GetNextDlgGroupItem(pWnd,false);		
  		i++;		
  		if (i >= GROUPCOLORS)
  		{
  			break;
  		}
  	}	

	m_iRadio = (int32)CFaceScheme::Instance()->m_eFaceEffectRange;

	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CDlgSystemFace::OnButtonColor() 
{
	CColorDialog dlgColor;
	if (IDOK == dlgColor.DoModal())
	{
		COLORREF color = dlgColor.GetColor();	
		SetCurrentColor(color);		
	}
}
void CDlgSystemFace::OnPaint() 
{
	CPaintDC dc(this); 

	//----------------------------------------填充背景颜色
	CBrush brush;                      
	CBrush *poldbrush;
	brush.CreateSolidBrush(m_aSysColor[ESCBackground]);
	
	poldbrush=dc.SelectObject(&brush);
	dc.FillRect(&m_rectPreView,&brush);
	dc.SelectObject(poldbrush);
	brush.DeleteObject();


	//-----------------------------------------画坐标线
	CPen pen;                          
	CPen *poldpen; 
	pen.CreatePen(PS_SOLID,1,m_aSysColor[ESCChartAxisLine]);
    poldpen=dc.SelectObject(&pen);
	int m_rect_middle=(m_rectPreView.left+m_rectPreView.right)/2; 
	dc.MoveTo(m_rectPreView.left+20,m_rectPreView.top+5);    //画垂直线
	dc.LineTo(m_rectPreView.left+20,(m_rectPreView.bottom/2)+42);
	
	dc.MoveTo(m_rectPreView.left+20,m_rectPreView.top+30);   //画水平线
	dc.LineTo(m_rect_middle+70,m_rectPreView.top+30);
	
	dc.MoveTo(m_rectPreView.left+20,m_rectPreView.top+75);
	dc.LineTo(m_rect_middle+70,m_rectPreView.top+75);
	
	dc.MoveTo(m_rectPreView.left+20,m_rectPreView.top+120);	
	dc.LineTo(m_rect_middle+70,m_rectPreView.top+120);
	dc.SelectObject(poldpen);
	pen.DeleteObject();

	//-----------------------------------------画图形文字
	CFont  FontSmall,*pOldFont;
    FontSmall.CreateFontIndirect(&m_aSysFont[ESFSmall].m_stLogfont);
	pOldFont = dc.SelectObject(&FontSmall);

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(m_aSysColor[ESCText]);	
	dc.TextOut(m_rectPreView.left+100,m_rectPreView.top+2,L"88.88");
	dc.SelectObject(pOldFont);
	FontSmall.DeleteObject();
	
	//-----画K线
	//... 第一个完整的阳线
    CBrush brushRiseKLine;                   //画阳线实体                  
	brushRiseKLine.CreateSolidBrush(m_aSysColor[ESCKLineRise]);    
	dc.SelectObject(&brushRiseKLine);
	
	RECT rectRiseKLine;
	
	rectRiseKLine.left  = m_rectPreView.left + 20;
	rectRiseKLine.right = rectRiseKLine.left + 11;
	rectRiseKLine.top   = m_rectPreView.top  + 18;
	rectRiseKLine.bottom= rectRiseKLine.top  + 24;
	dc.FillRect(&rectRiseKLine,&brushRiseKLine);

	CPen penRiseKLine;						//画阳线上下影线     
	penRiseKLine.CreatePen(PS_SOLID,1,m_aSysColor[ESCKLineRise]);
	poldpen=dc.SelectObject(&penRiseKLine);
	dc.MoveTo(int(m_rectPreView.left+25.5), m_rectPreView.top+13);
	dc.LineTo(int(m_rectPreView.left+25.5), m_rectPreView.top+52);

	//... 向右平移,第二个完整的阳线
	RECT   rectRiseKLine2;
	rectRiseKLine2.left  = rectRiseKLine.left + 30;
	rectRiseKLine2.right = rectRiseKLine.right + 30;
	rectRiseKLine2.top   = m_rectPreView.top  + 18;
	rectRiseKLine2.bottom= rectRiseKLine.top  + 24;
	dc.FillRect(&rectRiseKLine2,&brushRiseKLine);
	
	dc.MoveTo(int(m_rectPreView.left+55.5), m_rectPreView.top+13);
	dc.LineTo(int(m_rectPreView.left+55.5), m_rectPreView.top+52);
	
	// 3
	RECT   rectRiseKLine3;
	rectRiseKLine3.left  = rectRiseKLine2.left + 30;
	rectRiseKLine3.right = rectRiseKLine2.right + 30;
	rectRiseKLine3.top   = m_rectPreView.top  + 18;
	rectRiseKLine3.bottom= rectRiseKLine.top  + 24;
	dc.FillRect(&rectRiseKLine3,&brushRiseKLine);
	
	dc.MoveTo(int(m_rectPreView.left+85.5), m_rectPreView.top+13);
	dc.LineTo(int(m_rectPreView.left+85.5), m_rectPreView.top+52);
	
	// 4
	RECT   rectRiseKLine4;
	rectRiseKLine4.left  = rectRiseKLine3.left + 30;
	rectRiseKLine4.right = rectRiseKLine3.right + 30;
	rectRiseKLine4.top   = m_rectPreView.top  + 18;
	rectRiseKLine4.bottom= rectRiseKLine.top  + 24;
	dc.FillRect(&rectRiseKLine4,&brushRiseKLine);
	
	dc.MoveTo(int(m_rectPreView.left+115.5), m_rectPreView.top+13);
	dc.LineTo(int(m_rectPreView.left+115.5), m_rectPreView.top+52);
	
	// 5
	RECT   rectRiseKLine5;
	rectRiseKLine5.left  = rectRiseKLine4.left + 30;
	rectRiseKLine5.right = rectRiseKLine4.right + 30;
	rectRiseKLine5.top   = m_rectPreView.top  + 18;
	rectRiseKLine5.bottom= rectRiseKLine.top  + 24;
	dc.FillRect(&rectRiseKLine5,&brushRiseKLine);

	dc.SelectObject(poldbrush);
	brushRiseKLine.DeleteObject();
	
	dc.MoveTo(int(m_rectPreView.left+145.5), m_rectPreView.top+13);
	dc.LineTo(int(m_rectPreView.left+145.5), m_rectPreView.top+52);
	dc.SelectObject(poldpen);
	penRiseKLine.DeleteObject();
	
	// 阴线实体
	CBrush brushFallKLine;                                     
	brushFallKLine.CreateSolidBrush(m_aSysColor[ESCKLineFall]);    
	poldbrush=dc.SelectObject(&brushFallKLine);
	
	RECT rectFallKLine;
	
	rectFallKLine.left  = (rectRiseKLine.right + rectRiseKLine2.left -11)/2;
	rectFallKLine.right = rectFallKLine.left + 11;
	rectFallKLine.top   = m_rectPreView.top  + 27;
	rectFallKLine.bottom= rectFallKLine.top  + 24;
	dc.FillRect(&rectFallKLine,&brushFallKLine);
	dc.SelectObject(poldbrush);
	brushFallKLine.DeleteObject();
	
	CPen penFallKLine;						    
	penFallKLine.CreatePen(PS_SOLID,1,m_aSysColor[ESCKLineFall]);
	poldpen=dc.SelectObject(&penFallKLine);
	dc.MoveTo((rectRiseKLine.right + rectRiseKLine2.left)/2,m_rectPreView.top + 23);
	dc.LineTo((rectRiseKLine.right + rectRiseKLine2.left)/2,m_rectPreView.top+65);
	dc.SelectObject(poldpen);
	
	//2
	CBrush brushFallKLine2;                                     
	brushFallKLine2.CreateSolidBrush(m_aSysColor[ESCKLineFall]);    
	poldbrush=dc.SelectObject(&brushFallKLine2);
	
	RECT rectFallKLine2;
	
	rectFallKLine2.left  = rectFallKLine.left + 30;
	rectFallKLine2.right = rectFallKLine.right + 30;
	rectFallKLine2.top   = m_rectPreView.top  + 27;
	rectFallKLine2.bottom= rectFallKLine.top  + 24;
	dc.FillRect(&rectFallKLine2,&brushFallKLine2);
	dc.SelectObject(poldbrush);
	brushFallKLine2.DeleteObject();
	
	poldpen =  dc.SelectObject(&penFallKLine);
	dc.MoveTo(30+(rectRiseKLine.right + rectRiseKLine2.left)/2,m_rectPreView.top + 23);
	dc.LineTo(30+(rectRiseKLine.right + rectRiseKLine2.left)/2,m_rectPreView.top+65);
	dc.SelectObject(poldpen);
	
	// 3
	CBrush brushFallKLine3;                                     
	brushFallKLine3.CreateSolidBrush(m_aSysColor[ESCKLineFall]);    
	poldbrush=dc.SelectObject(&brushFallKLine3);
		
	RECT rectFallKLine3;
	
	rectFallKLine3.left  = rectFallKLine2.left + 30;
	rectFallKLine3.right = rectFallKLine2.right + 30;
	rectFallKLine3.top   = m_rectPreView.top  + 27;
	rectFallKLine3.bottom= rectFallKLine.top  + 24;
	dc.FillRect(&rectFallKLine3,&brushFallKLine3);
	dc.SelectObject(poldbrush);
	brushFallKLine3.DeleteObject();
	
	poldpen =  dc.SelectObject(&penFallKLine);
	dc.MoveTo(60+(rectRiseKLine.right + rectRiseKLine2.left)/2,m_rectPreView.top + 23);
	dc.LineTo(60+(rectRiseKLine.right + rectRiseKLine2.left)/2,m_rectPreView.top+65);
	dc.SelectObject(poldpen);

	// 4
	CBrush brushFallKLine4;                                     
	brushFallKLine4.CreateSolidBrush(m_aSysColor[ESCKLineFall]);    
	poldbrush=dc.SelectObject(&brushFallKLine4);
	
	RECT rectFallKLine4;
	
	rectFallKLine4.left  = rectFallKLine3.left + 30;
	rectFallKLine4.right = rectFallKLine3.right + 30;
	rectFallKLine4.top   = m_rectPreView.top  + 27;
	rectFallKLine4.bottom= rectFallKLine.top  + 24;
	dc.FillRect(&rectFallKLine4,&brushFallKLine4);
	dc.SelectObject(poldbrush);
	brushFallKLine4.DeleteObject();
	
	poldpen =  dc.SelectObject(&penFallKLine);
	dc.MoveTo(90+(rectRiseKLine.right + rectRiseKLine2.left)/2,m_rectPreView.top + 23);
	dc.LineTo(90+(rectRiseKLine.right + rectRiseKLine2.left)/2,m_rectPreView.top+65);
	dc.SelectObject(poldpen);
	penFallKLine.DeleteObject();
	
	// 平线
	CPen penKeepKLine;						    
	penKeepKLine.CreatePen(PS_SOLID,1,m_aSysColor[ESCKLineKeep]);
	poldpen=dc.SelectObject(&penKeepKLine);
	dc.MoveTo(m_rectPreView.left+20,100);
	dc.LineTo(m_rectPreView.left+150,100);
	dc.SelectObject(poldpen);
	penKeepKLine.DeleteObject();
	
	//------------画16 条指标线
	int beep = 27;
	int lx= m_rectPreView.left+20;
    int ly= 63 ;
	for(int j=ESCGuideLine1;j<ESCGuideLine16+1;j++)
	{
		CPen pennow;
		pennow.CreatePen(PS_SOLID,1,m_aSysColor[j]);
		poldpen=dc.SelectObject(&pennow);
		dc.MoveTo(lx,ly+j*4);
		dc.LineTo(lx+beep,ly+8+j*4);
		dc.LineTo(lx+2*beep,ly-8+j*4);
		dc.LineTo(lx+3*beep,ly+8+j*4);
		dc.LineTo(lx+4*beep,ly-8+j*4);
		dc.LineTo(lx+5*beep,ly+8+j*4);
		dc.SelectObject(poldpen);
		pennow.DeleteObject();
	}
	
	// 列表框,3 行,4列
	// 循环画线.
	CPen pentableline;     
	int32 i;
	
	pentableline.CreatePen(PS_SOLID,1,m_aSysColor[ESCGridLine]);
	poldpen=dc.SelectObject(&pentableline);
	for( i=0;i<5;i++)
	{
		dc.MoveTo(m_rectPreView.left+10,m_rectPreView.top+i*23+170);
		dc.LineTo(m_rectPreView.right-10,m_rectPreView.top+i*23+170);
	}
	
	dc.MoveTo(m_rectPreView.left+10,m_rectPreView.top+170);
	dc.LineTo(m_rectPreView.left+10,m_rectPreView.top+262);

	dc.MoveTo(m_rectPreView.left+64,m_rectPreView.top+193);
	dc.LineTo(m_rectPreView.left+64,m_rectPreView.top+262);

	dc.MoveTo(m_rectPreView.left+118,m_rectPreView.top+193);
	dc.LineTo(m_rectPreView.left+118,m_rectPreView.top+262);

	dc.MoveTo(m_rectPreView.left+174,m_rectPreView.top+170);
	dc.LineTo(m_rectPreView.left+174,m_rectPreView.top+262);

	dc.SelectObject(poldpen);
	pentableline.DeleteObject();

	// 列表框文字
	// dc.SelectObject(&m_aSysFont[ESFNormal].m_Font);
	// 不能实时显示,因为m_font 没有更改,需要通过LOGFONG 得到FONT 才能马上更新
 	CFont  FontBig;
	FontBig.CreateFontIndirect(&m_aSysFont[ESFBig].m_stLogfont);
 	pOldFont = dc.SelectObject(&FontBig);	
  	FontBig.DeleteObject();	
	dc.SetTextColor(m_aSysColor[ESCVolume]);	
 	dc.TextOut(m_rectPreView.left+33,m_rectPreView.top+171,L"0000 TEST");
	dc.SelectObject(pOldFont);
	FontBig.DeleteObject();

	CFont  FontNormal;
    FontNormal.CreateFontIndirect(&m_aSysFont[ESFNormal].m_stLogfont);
	pOldFont = dc.SelectObject(&FontNormal);

	// 上涨字
	dc.SetTextColor(m_aSysColor[ESCRise]);
	dc.TextOut(m_rectPreView.left+13,m_rectPreView.top+197,L"上涨字");
	dc.TextOut(m_rectPreView.left+73,m_rectPreView.top+197,L"99.99");
	dc.TextOut(m_rectPreView.left+123,m_rectPreView.top+197,L"99.99%");

	// 平盘字
	dc.SetTextColor(m_aSysColor[ESCKeep]);
	dc.TextOut(m_rectPreView.left+13,m_rectPreView.top+219,L"平盘字");
	dc.TextOut(m_rectPreView.left+73,m_rectPreView.top+219,L"66.66");
	dc.TextOut(m_rectPreView.left+123,m_rectPreView.top+219,L"66.66%");

	// 下跌字
	dc.SetTextColor(m_aSysColor[ESCFall]);
	dc.TextOut(m_rectPreView.left+13,m_rectPreView.top+241,L"下跌字");
	dc.TextOut(m_rectPreView.left+73,m_rectPreView.top+241,L"33.33");
	dc.TextOut(m_rectPreView.left+123,m_rectPreView.top+241,L"33.33%");
	
	dc.SelectObject(pOldFont);
	FontNormal.DeleteObject();
}

void CDlgSystemFace::OnSelchangeList1() 
{
	int32 i = m_CtrlList.GetCurSel();
	
	// 选中第i 个,显示 数组中第i 个的颜色.通过颜色得到控件.
	
	COLORREF clr =  m_aSysColor[i];
	
	CWnd * pWnd = GetDlgItem(IDC_STATIC_GRIDLINE);
	int32 iCount = 0;
	
	while ( NULL != pWnd )
	{
		CStaticSetColor * pStaticSetColor = (CStaticSetColor *)pWnd;
		pStaticSetColor->SetDefaultText();
				
		if (clr == pStaticSetColor->m_color)
		{
			pStaticSetColor->SetText(clr);			
		}
		pWnd = GetNextDlgGroupItem(pWnd,false);
		
		iCount++;		
		if (iCount >= GROUPCOLORS)
		{
			break;
		}
	}
}
void CDlgSystemFace::OnSelchangeCombo1() 
{
	// 重置List 和 颜色框 的内容
	m_CtrlList.SetCurSel(0);
	
	m_aSysColorSave.RemoveAll();

	// 读取当前风格的颜色设置
	// 将当前的颜色,保存到CARRAY 中.如果用户不再更改,就保存这个数组,再有点击动作,会覆盖这个数组中的数据.
			
	int32 iCurSel = m_CtrlCombo.GetCurSel();
	if (iCurSel == 0)
	{
		// 选择自定义风格时,从备份中将原始的颜色拷贝过来
		memcpyex(m_aSysColor,m_aSysColorBk,sizeof(m_aSysColor));
		m_aSysColorSave.Copy(m_aSysColorSaveBk);
		m_iStyleIndex = m_iStyleIndexBk;
	}
	else
	{	
		// 选择的是其他风格的,从文件中读取.测试状态,目前只有标准,红黑,绿灰 三种风格
		CFaceScheme::T_StyleData StyleData;
		if ( CFaceScheme::Instance()->GetStyleData(iCurSel, StyleData) )
		{
			memcpyex(m_aSysColor, StyleData.m_aColors, sizeof(m_aSysColor));
		}
			
		m_iStyleIndex = iCurSel;
	}

	// 刷新绘图区域	
	RefreshPicture();		
	
	OnSelchangeList1();
}
void CDlgSystemFace::SetCurrentColor(COLORREF clr)
{
	m_CurrentColor = clr ;	
	// 清空所有色块上的选中标记
	CWnd * pWnd  = GetDlgItem(IDC_STATIC_GRIDLINE);
	int32 iCount = 0;
	
	while ( NULL != pWnd )
	{
		CStaticSetColor * pStaticSetColor = (CStaticSetColor *)pWnd;
		pStaticSetColor->SetDefaultText();
		pWnd = GetNextDlgGroupItem(pWnd,false);		
		iCount++;		
		if (iCount >= GROUPCOLORS)
		{
			break;
		}
	}	
	int32 iCurSel = m_CtrlList.GetCurSel();
	
	if ( 0x12345678 != m_CurrentColor)
	{
		// 只允许修改[自定义]这项
		// 只有在这里修改需要保存的用户自定义的颜色,同时判断重复,看是否对同一个项目多次设置,只取最后一次内容
		
		T_SysColorSave  SysColorSave;
		CString			StrStyleName;
		
		StrStyleName = CFaceScheme::GetSysStyleArray(1);   
		SysColorSave.m_StrFileKeyName	= CFaceScheme::GetSysColorObject(iCurSel)->m_StrFileKeyName;
		SysColorSave.m_Color			= m_CurrentColor;
		if ( 0 == m_aSysColorSave.GetSize())
		{
			// 第一次,空数组,直接添加
			m_aSysColorSave.Add(SysColorSave); 				
		}
		else
		{
			bool32 bExist = false;
			int32  iIndex = -1;
			for (int32 i =0 ;i <m_aSysColorSave.GetSize();i++)
			{
				if (m_aSysColorSave[i].m_StrFileKeyName == CFaceScheme::GetSysColorObject(iCurSel)->m_StrFileKeyName)
				{
					bExist = true;
					iIndex = i;
					break;
				}
			}
			if (bExist)
			{
				// 有已存在的,就替换
				m_aSysColorSave.SetAt(iIndex,SysColorSave);				
			}
			else
			{
				// 没有就添加
				m_aSysColorSave.Add(SysColorSave); 				
			}
			
		}
	}	
	// 通知绘图区域实时重绘	
	m_aSysColor[iCurSel] = clr;
	RefreshPicture();
	m_CurrentColor = 0x12345678;
	
}

const int32 CDlgSystemFace::GetArraySysColorSaveSize()
{
	return m_aSysColorSave.GetSize();
}
const T_SysColorSave & CDlgSystemFace::GetArraySysColorSave(int32 iIndex)
{
	ASSERT(iIndex<m_aSysColorSave.GetSize());
	return m_aSysColorSave[iIndex];
}

void CDlgSystemFace::SaveSysFace()
{	
	// 颜色改变了,设置CFaceSchema 中的数据,发消息,让相关的视图自己去取数据更新
	if ( NULL == m_pIoViewActive )
	{
		return;
	}
		
	UpdateData(true);

	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();

	CFaceScheme::Instance()->SetNowUseColors(m_aSysColor);
	CFaceScheme::Instance()->m_eFaceEffectRange = CFaceScheme::E_FaceEffectRange(m_iRadio);

	pMainFrame->PostMessage(UM_Color_Change, WPARAM(m_pIoViewActive), 0);
}

void CDlgSystemFace::SaveSysFont(E_SysFont eFontType)
{	
	// 字体改变了,设置CFaceSchema 中的数据,发消息,让相关的视图自己去取数据更新
	if ( NULL == m_pIoViewActive )
	{
		//ASSERT(0);
		return;
	}

	CFontDialog dlgFont(&m_aSysFont[eFontType].m_stLogfont);	
	if (IDOK == dlgFont.DoModal())
	{
		UpdateData(true);

		dlgFont.GetCurrentFont(&m_aSysFont[eFontType].m_stLogfont);
		LOGFONT lgFont = m_aSysFont[eFontType].m_stLogfont;		

		T_SysFontSave FontSave;		
		FontSave.m_eSysFont  = eFontType;
		FontSave.m_lfLogFont = lgFont;

		bool32 bExist = false;
		int32  iIndex = 0;
		int32 i = 0;
 		for (i =0 ; i< m_aSysFontSave.GetSize(); i++)
 		{
			if ( FontSave.m_eSysFont == m_aSysFontSave[i].m_eSysFont)
			{
				bExist = true;
				iIndex = i;
				break;
			}
 		}
		if (bExist)
		{
			m_aSysFontSave.SetAt(iIndex,FontSave);
		}
		else
		{
			m_aSysFontSave.Add(FontSave);
		}
	
		CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();

		CFontNode	 aSysFont[ESFCount];
//		memcpyex(aSysFont,m_aSysFont,sizeof(m_aSysFont));
// 		for ( i = 0 ;i < ESFCount ; i++)
// 		{
// 			if ( i != int32(eFontType))
// 			{
// 				aSysFont[i].m_Font.DeleteObject();
// 				bool32 bSuccess = aSysFont[i].m_Font.CreateFontIndirect(&(aSysFont[i].m_stLogfont));
// 			}
// 			else
// 			{
// 				aSysFont[eFontType].SetFont(&FontSave.m_lfLogFont);
// 			}			
// 		}
		for ( i=0; i < ESFCount ; i++ )
		{
			aSysFont[i] = m_aSysFont[i];
		}
		aSysFont[eFontType].SetFont(&FontSave.m_lfLogFont);

		

		CFaceScheme::Instance()->SetNowUseFonts(aSysFont);
		CFaceScheme::Instance()->m_eFaceEffectRange = CFaceScheme::E_FaceEffectRange(m_iRadio);
		
		pMainFrame->PostMessage(UM_Font_Change, (WPARAM)m_pIoViewActive, 0);
	}
}

void CDlgSystemFace::OnButtonSet() 
{
	SaveSysFace();	
}
void CDlgSystemFace::RefreshPicture()
{
	InvalidateRect(&m_rectPreView);
	UpdateWindow();
}
void CDlgSystemFace::OnButtonBigfont() 
{
	SaveSysFont(ESFBig);
	RefreshPicture();
}
void CDlgSystemFace::OnButtonNormalfont() 
{	
	SaveSysFont(ESFNormal);
	RefreshPicture();		
}
void CDlgSystemFace::OnButtonSmallfont() 
{
	SaveSysFont(ESFSmall);
	RefreshPicture();	
}
void CDlgSystemFace::OnButtonFontReset() 
{
	if ( NULL == m_pIoViewActive )
	{
		//ASSERT(0);
		return;
	}

	if ( IDYES == MessageBox(L"是否恢复默认字体?", AfxGetApp()->m_pszAppName, MB_YESNO))
	{
		// 读取默认数据.	
		CFaceScheme::Instance()->InitFontsAsDefault();

// 		memcpyex(m_aSysFont,CFaceScheme::Instance()->m_aSysFont,sizeof(CFaceScheme::Instance()->m_aSysFont));
// 
// 		for (int32 i = 0 ;i < ESFCount ; i++)
// 		{
// 			m_aSysFont[i].m_Font.DeleteObject();
// 			bool32 bSuccess = m_aSysFont[i].m_Font.CreateFontIndirect(&(m_aSysFont[i].m_stLogfont));
// 		}
		for (int32 i = 0 ;i < ESFCount ; i++)
		{
			m_aSysFont[i] = CFaceScheme::Instance()->m_aSysFont[i];
 		}
		
		// 
		CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
		
		//
		CFaceScheme::Instance()->SetNowUseFonts(m_aSysFont);
		CFaceScheme::Instance()->m_eFaceEffectRange = CFaceScheme::E_FaceEffectRange(m_iRadio);
		pMainFrame->PostMessage(UM_Font_Change, (WPARAM)m_pIoViewActive, 0);

		RefreshPicture();

	}	
}
BOOL CDlgSystemFace::PreTranslateMessage(MSG* pMsg) 
{
	if ( WM_KEYDOWN == pMsg->message )
	{
		if (VK_ESCAPE == pMsg->wParam )
		{
			return false;
		}
		else if (VK_RETURN == pMsg->wParam)
		{
			return true;
		}
	}		
	return CDialog::PreTranslateMessage(pMsg);
}
CMPIChildFrame * CDlgSystemFace::GetCurrentChildFrame()
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CMPIChildFrame * pActiveChild =(CMPIChildFrame *)pMainFrame->GetActiveFrame();
	return 	pActiveChild;
}

HBRUSH CDlgSystemFace::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if (nCtlColor == CTLCOLOR_STATIC)
	{	
		pDC->SetTextColor(RGB(0,0,255));
	}
	
	return hbr;
}

LRESULT CDlgSystemFace::OnSetColor(WPARAM wParam,LPARAM lParam)
{
	COLORREF clr = COLORREF(wParam);
	SetCurrentColor(clr);
	return true;
}
// CString CDlgSystemFace::TranslateToESysColorString(UINT uiSysColor)
// {	
// 	ASSERT( 0 );	// 被抛弃
// 	switch(uiSysColor)
// 	{
// 	case 0:
// 		return CFaceScheme::GetKeyNameBackGround();
// 	case 1:
// 		return CFaceScheme::GetKeyNameText();
// 	case 2:
// 		return CFaceScheme::GetKeyNameChartAxisLine();
// 	case 3:
// 		return CFaceScheme::GetKeyNameKLineRise();
// 	case 4:
// 		return CFaceScheme::GetKeyNameKLineKeep();
// 	case 5:
// 		return CFaceScheme::GetKeyNameKLineFall();
// 	case 6:
// 		return CFaceScheme::GetKeyNameRise();
// 	case 7:
// 		return CFaceScheme::GetKeyNameKeep();
// 	case 8:
// 		return CFaceScheme::GetKeyNameFall();
// 	case 9:
// 		return CFaceScheme::GetKeyNameVolume();
// 	case 10:
// 		return CFaceScheme::GetKeyNameAmount();
// 	case 11:
// 		return CFaceScheme::GetKeyNameGridLine();
// 	case 12:
// 		return CFaceScheme::GetKeyNameGridSelected();
// 	case 13:
// 		return CFaceScheme::GetKeyNameGridFixedBk();
// 	case 14:
// 		return CFaceScheme::GetKeyNameGuideLine1();
// 	case 15:
// 		return CFaceScheme::GetKeyNameGuideLine2();
// 	case 16:
// 		return CFaceScheme::GetKeyNameGuideLine3();
// 	case 17:
// 		return CFaceScheme::GetKeyNameGuideLine4();
// 	case 18:
// 		return CFaceScheme::GetKeyNameGuideLine5();
// 	case 19:
// 		return CFaceScheme::GetKeyNameGuideLine6();
// 	case 20:
// 		return CFaceScheme::GetKeyNameGuideLine7();
// 	case 21:
// 		return CFaceScheme::GetKeyNameGuideLine8();
// 	case 22:
// 		return CFaceScheme::GetKeyNameGuideLine9();
// 	case 23:
// 		return CFaceScheme::GetKeyNameGuideLine10();
// 	case 24:
// 		return CFaceScheme::GetKeyNameGuideLine11();
// 	case 25:
// 		return CFaceScheme::GetKeyNameGuideLine12();
// 	case 26:
// 		return CFaceScheme::GetKeyNameGuideLine13();
// 	case 27:
// 		return CFaceScheme::GetKeyNameGuideLine14();
// 	case 28:
// 		return CFaceScheme::GetKeyNameGuideLine15();
// 	case 29:
// 		return CFaceScheme::GetKeyNameGuideLine16();
// 	default:
// 		break;
// 	}
// 	return L"";
// }
