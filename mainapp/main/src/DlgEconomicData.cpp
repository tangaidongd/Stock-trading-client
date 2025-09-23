// DlgEconomicData.cpp : 实现文件
//

#include "stdafx.h"
#include "GGTong.h"
#include "DlgEconomicData.h"
#include <gdiplus.h>
using namespace Gdiplus;


#define BACKGROUND_COLOR RGB(238,240,248)
#define COLOR_GREEN  RGB(39,142,82)
#define COLOR_RED    RGB(255,0,0)
#define COLOR_BLACK	 RGB(0,0,0)
#define COLOR_WHITE	 RGB(255,255,255)


// CDlgEconomicData 对话框

IMPLEMENT_DYNAMIC(CDlgEconomicData, CDialog)

CDlgEconomicData::CDlgEconomicData(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEconomicData::IDD, pParent)
{
	m_bAll = false;
	m_imgCountry = NULL;
	m_imgLevel = NULL;

	LOGFONT lf = {0};
	lf.lfHeight = 12;
	lf.lfWeight = FW_BOLD;
	_tcscpy(lf.lfFaceName, _T("微软雅黑"));
	m_font_content.CreateFontIndirect(&lf);

}

CDlgEconomicData::~CDlgEconomicData()
{
	DEL(m_imgCountry);
	DEL(m_imgLevel);
}

void CDlgEconomicData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TIME, m_label_time);
	DDX_Control(pDX, IDC_STATIC_FORMER, m_lable_former);
	DDX_Control(pDX, IDC_STATIC_FORMER_VALUE, m_lable_former_value);
	DDX_Control(pDX, IDC_STATIC_FORECAST, m_lable_forecast);
	DDX_Control(pDX, IDC_STATIC_FORECAST_VALUE, m_lable_forecast_value);
	DDX_Control(pDX, IDC_STATIC_ACTUAL, m_lable_actual);
	DDX_Control(pDX, IDC_STATIC_ACTUAL_VALUE, m_lable_actual_valule);
}


BEGIN_MESSAGE_MAP(CDlgEconomicData, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDlgEconomicData 消息处理程序

void CDlgEconomicData::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rt;
	GetClientRect(rt);

	CBrush br(BACKGROUND_COLOR);
	dc.FillRect(rt, &br);

	Graphics graphic(dc.GetSafeHdc());

	if (m_imgCountry)
	{
		RectF rtCountry(10, 25, 50, 50);
		graphic.DrawImage(m_imgCountry, rtCountry, 0, 0, m_imgCountry->GetWidth(), m_imgCountry->GetHeight(), UnitPixel);
	}

	if (m_imgLevel)
	{
		RectF rtLevel(15, 80, 42, 12);
		graphic.DrawImage(m_imgLevel, rtLevel, 0, 0, m_imgLevel->GetWidth(), m_imgLevel->GetHeight(), UnitPixel);
	}

	if (!m_strContent.IsEmpty())
	{
		SolidBrush  brush(Color(255, 0,0,0));
		FontFamily  fontFamily(L"微软雅黑");
		Gdiplus::Font font(&fontFamily, 12, FontStyleBold, UnitPixel);
		StringFormat stringFormat;
		stringFormat.SetLineAlignment(StringAlignmentCenter);

		CSize size = dc.GetTextExtent(m_strContent);
		int cy = 55 - size.cy -10;
		int cx = 73;
		int pos = rt.Width()-73;
		if (size.cx > pos)
		{
			cy = 55 - size.cy * 2 -5;
			RectF rect(cx, cy, 15 * 18, size.cy * 2);
			graphic.DrawString(m_strContent, -1, &font, rect, &stringFormat, &brush);
		}
		else
		{
			PointF pt(cx, cy);
			graphic.DrawString(m_strContent, m_strContent.GetLength(), &font, pt, &brush);
		}
	}

	DrawLD(graphic, dc);
	DrawLK(graphic, dc);
	DrawPlaceholder(graphic, dc);
}

void CDlgEconomicData::DrawLD(Graphics &graphic, CPaintDC &dc)
{
	if (!m_strLD.IsEmpty())
	{
		StringFormat stringFormat;
		stringFormat.SetAlignment(StringAlignmentCenter);
		stringFormat.SetLineAlignment(StringAlignmentCenter);

		FontFamily  fontFamily(L"宋体");
		Gdiplus::Font font(&fontFamily, 12, FontStyleBold, UnitPixel);

		int posX = 70, posY = 0;
		if (m_bAll)
			posY = 72;
		else
			posY = 80;

		
		int height = 22;

		// 利多
		{
			CSize size = dc.GetTextExtent(m_strLD);
			RectF rt(posX, posY, size.cx+20, height);
			SolidBrush  brLDText(Color(255, 255,255,255));
			SolidBrush  brLDBk(Color(255, 255,0,0));
			graphic.FillRectangle(&brLDBk, rt);
			graphic.DrawString(m_strLD, -1, &font, rt, &stringFormat, &brLDText);
			posX = 70 + size.cx+20; 
		}

		// 利多商品
		{
			CSize size = dc.GetTextExtent(m_strLDName);
			SolidBrush  brLDName(Color(255, 0,0,0));
			SolidBrush  brLDBk(Color(255, 255,255,255));
			RectF rt1(posX, posY, size.cx+20, height);
			graphic.FillRectangle(&brLDBk, rt1);
			graphic.DrawString(m_strLDName, -1, &font, rt1, &stringFormat, &brLDName);
		}
	}
}

void CDlgEconomicData::DrawLK(Graphics &graphic, CPaintDC &dc)
{
	if (!m_strLK.IsEmpty())
	{
		StringFormat stringFormat;
		stringFormat.SetAlignment(StringAlignmentCenter);
		stringFormat.SetLineAlignment(StringAlignmentCenter);

		FontFamily  fontFamily(L"宋体");
		Gdiplus::Font font(&fontFamily, 12, FontStyleBold, UnitPixel);

		int posX = 70, posY = 98;
		if (m_bAll)
			posY = 98;
		else
			posY = 80;
		
		int height = 22;

		// 利空
		{
			CSize size = dc.GetTextExtent(m_strLK);
			RectF rt(posX, posY, size.cx+20, height);
			SolidBrush  brLKText(Color(255, 255,255,255));
			SolidBrush  brLKBk(Color(255, 39,142,82));
			graphic.FillRectangle(&brLKBk, rt);
			graphic.DrawString(m_strLK, -1, &font, rt, &stringFormat, &brLKText);
			posX = 70 + size.cx+20; 
		}

		// 利空商品
		{
			CSize size = dc.GetTextExtent(m_strLKName);
			SolidBrush  brLKName(Color(255, 0,0,0));
			SolidBrush  brLKBk(Color(255, 255,255,255));
			RectF rt1(posX, posY, size.cx+20, height);
			graphic.FillRectangle(&brLKBk, rt1);
			graphic.DrawString(m_strLKName, -1, &font, rt1, &stringFormat, &brLKName);
		}
	}
}

void CDlgEconomicData::DrawPlaceholder(Graphics &graphic, CPaintDC &dc)
{
	if (m_strPlaceholder.IsEmpty())
		return;

//	CSize size = dc.GetTextExtent(m_strLKName);
	SolidBrush  brText(Color(255, 0,0,0));
	FontFamily  fontFamily(L"宋体");
	Gdiplus::Font font(&fontFamily, 12, FontStyleBold, UnitPixel);
	PointF pt(70, 80);
	graphic.DrawString(m_strPlaceholder, -1, &font, pt, &brText);
}
void CDlgEconomicData::InitData(T_EconomicData &stData)
{
	// 时间
	CString str = stData.release_time.c_str();
	m_label_time.SetTitleText1(str);
	m_label_time.SetTitleColor1(COLOR_BLACK);
	m_label_time.SetBkColor(BACKGROUND_COLOR);

	m_strContent = stData.content.c_str();

	m_lable_former.SetTitleText1("前值: ");
	m_lable_former.SetTitleColor1(COLOR_BLACK);
	m_lable_former.SetBkColor(BACKGROUND_COLOR);

	// 前值
	str = stData.former_value.c_str();
	m_lable_former_value.SetTitleText1(str);
	if (atoi(stData.former_value.c_str()) > 0)
		m_lable_former_value.SetTitleColor1(COLOR_RED);
	else
		m_lable_former_value.SetTitleColor1(COLOR_GREEN);
	m_lable_former_value.SetBkColor(BACKGROUND_COLOR);

	m_lable_forecast.SetTitleText1("预期: ");
	m_lable_forecast.SetTitleColor1(COLOR_BLACK);
	m_lable_forecast.SetBkColor(BACKGROUND_COLOR);

	// 预期
	str = stData.forecast_value.c_str();
	m_lable_forecast_value.SetTitleText1(str);
	if (atoi(stData.forecast_value.c_str()) > 0)
		m_lable_forecast_value.SetTitleColor1(COLOR_RED);
	else
		m_lable_forecast_value.SetTitleColor1(COLOR_GREEN);
	m_lable_forecast_value.SetBkColor(BACKGROUND_COLOR);

	m_lable_actual.SetTitleText1("实际: ");
	m_lable_actual.SetTitleColor1(COLOR_BLACK);
	m_lable_actual.SetBkColor(BACKGROUND_COLOR);

	// 实际
	str = stData.actual_value.c_str();
	m_lable_actual_valule.SetTitleText1(str);
	if (atoi(stData.actual_value.c_str()) > 0)
		m_lable_actual_valule.SetTitleColor1(COLOR_RED);
	else
		m_lable_actual_valule.SetTitleColor1(COLOR_GREEN);
	m_lable_actual_valule.SetBkColor(BACKGROUND_COLOR);

	m_bAll = stData.bBoth;

	if (!stData.strLD.empty())
	{
		m_strLD = stData.strLD.substr(0, 4).c_str();
		m_strLDName = stData.strLD.substr(4).c_str();
	}

	if (!stData.strLK.empty())
	{
		m_strLK = stData.strLK.substr(0, 4).c_str();
		m_strLKName = stData.strLK.substr(4).c_str();
	}

	if (!stData.strPlaceholder.empty())
	{
		m_strPlaceholder = stData.strPlaceholder.c_str();
	}
}

void CDlgEconomicData::LoadCountryIcon(CString strFileName)
{
	m_imgCountry = Image::FromFile(strFileName);
}

void CDlgEconomicData::LoadLevelIcon(CString strFileName)
{
	m_imgLevel = Image::FromFile(strFileName);
}

