/****************************************************************************\
Datei  : Color.cpp
Projekt: Farbverwaltung
Inhalt : CColor Implementierung
Datum  : 10.01.1999
Autor  : Christian Rodemeyer
Hinweis: ?1999 by Christian Rodemeyer
         Info über HLS Konvertierungsfunktion
         - Foley and Van Dam: "Fundamentals of Interactive Computer Graphics"  
         - MSDN: 'HLS Color Spaces'
         - MSDN: 'Converting Colors Between RGB and HLS' 

Modified:  8 Sep 2004  Hans Dietrich
           - fixed SetRgb() parameter order
		   - moved ToRGB() and ToHLS() to public section

\****************************************************************************/

#include "StdAfx.h"
#include "Color.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************************************\
 CColor: Implementierung
\****************************************************************************/
const CColor::DNamedColor CColor::m_namedColor[CColor::numNamedColors] =
{
  {aliceblue            , L"aliceblue"},
  {antiquewhite         , L"antiquewhite"},
  {aqua                 , L"aqua"},
  {aquamarine           , L"aquamarine"},
  {azure                , L"azure"},
  {beige                , L"beige"},
  {bisque               , L"bisque"},
  {black                , L"black"},
  {blanchedalmond       , L"blanchedalmond"},
  {blue                 , L"blue"},
  {blueviolet           , L"blueviolet"},
  {brown                , L"brown"},
  {burlywood            , L"burlywood"},
  {cadetblue            , L"cadetblue"},
  {chartreuse           , L"chartreuse"},
  {chocolate            , L"chocolate"},
  {coral                , L"coral"},
  {cornflower           , L"cornflower"},
  {cornsilk             , L"cornsilk"},
  {crimson              , L"crimson"},
  {cyan                 , L"cyan"},
  {darkblue             , L"darkblue"},
  {darkcyan             , L"darkcyan"},
  {darkgoldenrod        , L"darkgoldenrod"},
  {darkgray             , L"darkgray"},
  {darkgreen            , L"darkgreen"},
  {darkkhaki            , L"darkkhaki"},
  {darkmagenta          , L"darkmagenta"},
  {darkolivegreen       , L"darkolivegreen"},
  {darkorange           , L"darkorange"},
  {darkorchid           , L"darkorchid"},
  {darkred              , L"darkred"},
  {darksalmon           , L"darksalmon"},
  {darkseagreen         , L"darkseagreen"},
  {darkslateblue        , L"darkslateblue"},
  {darkslategray        , L"darkslategray"},
  {darkturquoise        , L"darkturquoise"},
  {darkviolet           , L"darkviolet"},
  {deeppink             , L"deeppink"},
  {deepskyblue          , L"deepskyblue"},
  {dimgray              , L"dimgray"},
  {dodgerblue           , L"dodgerblue"},
  {firebrick            , L"firebrick"},
  {floralwhite          , L"floralwhite"},
  {forestgreen          , L"forestgreen"},
  {fuchsia              , L"fuchsia"},
  {gainsboro            , L"gainsboro"},
  {ghostwhite           , L"ghostwhite"},
  {gold                 , L"gold"},
  {goldenrod            , L"goldenrod"},
  {gray                 , L"gray"},
  {green                , L"green"},
  {greenyellow          , L"greenyellow"},
  {honeydew             , L"honeydew"},
  {hotpink              , L"hotpink"},
  {indianred            , L"indianred"},
  {indigo               , L"indigo"},
  {ivory                , L"ivory"},
  {khaki                , L"khaki"},
  {lavender             , L"lavender"},
  {lavenderblush        , L"lavenderblush"},
  {lawngreen            , L"lawngreen"},
  {lemonchiffon         , L"lemonchiffon"},
  {lightblue            , L"lightblue"},
  {lightcoral           , L"lightcoral"},
  {lightcyan            , L"lightcyan"},
  {lightgoldenrodyellow , L"lightgoldenrodyellow"},
  {lightgreen           , L"lightgreen"},
  {lightgrey            , L"lightgrey"},
  {lightpink            , L"lightpink"},
  {lightsalmon          , L"lightsalmon"},
  {lightseagreen        , L"lightseagreen"},
  {lightskyblue         , L"lightskyblue"},
  {lightslategray       , L"lightslategray"},
  {lightsteelblue       , L"lightsteelblue"},
  {lightyellow          , L"lightyellow"},
  {lime                 , L"lime"},
  {limegreen            , L"limegreen"},
  {linen                , L"linen"},
  {magenta              , L"magenta"},
  {maroon               , L"maroon"},
  {mediumaquamarine     , L"mediumaquamarine"},
  {mediumblue           , L"mediumblue"},
  {mediumorchid         , L"mediumorchid"},
  {mediumpurple         , L"mediumpurple"},
  {mediumseagreen       , L"mediumseagreen"},
  {mediumslateblue      , L"mediumslateblue"},
  {mediumspringgreen    , L"mediumspringgreen"},
  {mediumturquoise      , L"mediumturquoise"},
  {mediumvioletred      , L"mediumvioletred"},
  {midnightblue         , L"midnightblue"},
  {mintcream            , L"mintcream"},
  {mistyrose            , L"mistyrose"},
  {moccasin             , L"moccasin"},
  {navajowhite          , L"navajowhite"},
  {navy                 , L"navy"},
  {oldlace              , L"oldlace"},
  {olive                , L"olive"},
  {olivedrab            , L"olivedrab"},
  {orange               , L"orange"},
  {orangered            , L"orangered"},
  {orchid               , L"orchid"},
  {palegoldenrod        , L"palegoldenrod"},
  {palegreen            , L"palegreen"},
  {paleturquoise        , L"paleturquoise"},
  {palevioletred        , L"palevioletred"},
  {papayawhip           , L"papayawhip"},
  {peachpuff            , L"peachpuff"},
  {peru                 , L"peru"},
  {pink                 , L"pink"},
  {plum                 , L"plum"},
  {powderblue           , L"powderblue"},
  {purple               , L"purple"},
  {red                  , L"red"},
  {rosybrown            , L"rosybrown"},
  {royalblue            , L"royalblue"},
  {saddlebrown          , L"saddlebrown"},
  {salmon               , L"salmon"},
  {sandybrown           , L"sandybrown"},
  {seagreen             , L"seagreen"},
  {seashell             , L"seashell"},
  {sienna               , L"sienna"},
  {silver               , L"silver"},
  {skyblue              , L"skyblue"},
  {slateblue            , L"slateblue"},
  {slategray            , L"slategray"},
  {snow                 , L"snow"},
  {springgreen          , L"springgreen"},
  {steelblue            , L"steelblue"},
  {tan                  , L"tan"},
  {teal                 , L"teal"},
  {thistle              , L"thistle"},
  {tomato               , L"tomato"},
  {turquoise            , L"turquoise"},
  {violet               , L"violet"},
  {wheat                , L"wheat"},
  {white                , L"white"},
  {whitesmoke           , L"whitesmoke"},
  {yellow               , L"yellow"},
  {yellowgreen          , L"yellowgreen"}
};

LPCTSTR CColor::GetNameFromIndex(int i)
{
	ASSERT(0 <= i && i < numNamedColors);
	if (0 <= i && i < numNamedColors)
	{
		return m_namedColor[i].name;
	}
	return m_namedColor[0].name;

}

CColor CColor::GetColorFromIndex(int i)
{
  ASSERT(0 <= i && i < numNamedColors);
  if (0 <= i && i < numNamedColors)
  {
	   return m_namedColor[i].color;
  }
  return m_namedColor[0].color;
}

CColor CColor::FromString(LPCTSTR pcColor)
{
  CColor t;
  t.SetString(pcColor);
  return t;
}

CColor::CColor(COLORREF cr)
: m_bIsRGB(true), m_bIsHLS(false), m_colorref(cr)
{
	m_hue = 0.0f;
	m_saturation = 0.0f;
	m_luminance = 0.0f;	
}

CColor::operator COLORREF() const
{
  const_cast<CColor*>(this)->ToRGB();
  return m_colorref;
}

// RGB

void CColor::SetRed(int iRed)
{
  ASSERT(0 <= iRed && iRed <= 255);
  ToRGB();
  m_color[c_red] = static_cast<unsigned char>(iRed);
  m_bIsHLS = false;
}

void CColor::SetGreen(int iGreen)
{
  ASSERT(0 <= iGreen && iGreen <= 255);
  ToRGB();
  m_color[c_green] = static_cast<unsigned char>(iGreen);
  m_bIsHLS = false;
}

void CColor::SetBlue(int iBlue)
{
  ASSERT(0 <= iBlue && iBlue <= 255);
  ToRGB();
  m_color[c_blue] = static_cast<unsigned char>(iBlue);
  m_bIsHLS = false;
}

void CColor::SetRGB(int iRed, int iGreen, int iBlue)
{
  ASSERT(0 <= iRed && iRed <= 255);
  ASSERT(0 <= iGreen && iGreen <= 255);
  ASSERT(0 <= iBlue && iBlue <= 255);

  m_color[c_red]   = static_cast<unsigned char>(iRed);
  m_color[c_green] = static_cast<unsigned char>(iGreen);
  m_color[c_blue]  = static_cast<unsigned char>(iBlue);
  m_bIsHLS = false;
  m_bIsRGB = true;
}

int CColor::GetRed() const
{
  const_cast<CColor*>(this)->ToRGB();
  return m_color[c_red];
}

int CColor::GetGreen() const
{
  const_cast<CColor*>(this)->ToRGB();
  return m_color[c_green];
}

int CColor::GetBlue() const
{
  const_cast<CColor*>(this)->ToRGB();
  return m_color[c_blue];
}

// HSL

void CColor::SetHue(float hue)
{
  ASSERT(hue >= 0.0 && hue <= 360.0);

  ToHLS();
  m_hue = hue;
  m_bIsRGB = false;
}

void CColor::SetSaturation(float saturation)
{
  ASSERT(saturation >= 0.0 && saturation <= 1.0); // 0.0 ist undefiniert

  ToHLS();
  m_saturation = saturation;
  m_bIsRGB = false;
}

void CColor::SetLuminance(float luminance)
{
  ASSERT(luminance >= 0.0 && luminance <= 1.0);

  ToHLS();
  m_luminance = luminance;
  m_bIsRGB = false;
}

void CColor::SetHLS(float hue, float luminance, float saturation)
{
  ASSERT(hue >= 0.0 && hue <= 360.0);
  ASSERT(luminance >= 0.0 && luminance <= 1.0);
  ASSERT(saturation >= 0.0 && saturation <= 1.0); // 0.0 ist undefiniert

  m_hue = hue;
  m_luminance = luminance;
  m_saturation = saturation;
  m_bIsRGB = false;
  m_bIsHLS = true;
}

float CColor::GetHue() const
{
  const_cast<CColor*>(this)->ToHLS();
  return m_hue;
}

float CColor::GetSaturation() const
{
  const_cast<CColor*>(this)->ToHLS();
  return m_saturation;
}

float CColor::GetLuminance() const
{
  const_cast<CColor*>(this)->ToHLS();
  return m_luminance;
}

// Konvertierung

void CColor::ToHLS() 
{
  if (!m_bIsHLS)
  {
    // Konvertierung
    unsigned char minval = min(m_color[c_red], min(m_color[c_green], m_color[c_blue]));
    unsigned char maxval = max(m_color[c_red], max(m_color[c_green], m_color[c_blue]));
    float mdiff  = float(maxval) - float(minval);
    float msum   = float(maxval) + float(minval);
   
    m_luminance = msum / 510.0f;

    if (maxval == minval) 
    {
      m_saturation = 0.0f;
      m_hue = 0.0f; 
    }   
    else 
    { 
      float rnorm = (maxval - m_color[c_red]  ) / mdiff;      
      float gnorm = (maxval - m_color[c_green]) / mdiff;
      float bnorm = (maxval - m_color[c_blue] ) / mdiff;   

      m_saturation = (m_luminance <= 0.5f) ? (mdiff / msum) : (mdiff / (510.0f - msum));

      if (m_color[c_red]   == maxval) m_hue = 60.0f * (6.0f + bnorm - gnorm);
      if (m_color[c_green] == maxval) m_hue = 60.0f * (2.0f + rnorm - bnorm);
      if (m_color[c_blue]  == maxval) m_hue = 60.0f * (4.0f + gnorm - rnorm);
      if (m_hue > 360.0f) m_hue = m_hue - 360.0f;
    }
    m_bIsHLS = true;
  }
}

void CColor::ToRGB() 
{
  if (!m_bIsRGB)
  {
    if (m_saturation == 0.0) // Grauton, einfacher Fall
    {
      m_color[c_red] = m_color[c_green] = m_color[c_blue] = 
		  (unsigned char)(m_luminance * 255.0);
    }
    else
    {
      float rm1, rm2;
         
      if (m_luminance <= 0.5f) rm2 = m_luminance + m_luminance * m_saturation;  
      else                     rm2 = m_luminance + m_saturation - m_luminance * m_saturation;
      rm1 = 2.0f * m_luminance - rm2;   
      m_color[c_red]   = ToRGB1(rm1, rm2, m_hue + 120.0f);   
      m_color[c_green] = ToRGB1(rm1, rm2, m_hue);
      m_color[c_blue]  = ToRGB1(rm1, rm2, m_hue - 120.0f);
    }
    m_bIsRGB = true;
  }
}

unsigned char CColor::ToRGB1(float rm1, float rm2, float rh)
{
  if      (rh > 360.0f) rh -= 360.0f;
  else if (rh <   0.0f) rh += 360.0f;
 
  if      (rh <  60.0f) rm1 = rm1 + (rm2 - rm1) * rh / 60.0f;
  else if (rh < 180.0f) rm1 = rm2;
  else if (rh < 240.0f) rm1 = rm1 + (rm2 - rm1) * (240.0f - rh) / 60.0f;

#if 0  // -----------------------------------------------------------
  rm1 = rm1 * 255.;
  rm1 += 0.5;
  if (rm1 > 255.0)
	  rm1 = 255.0;
#endif // -----------------------------------------------------------

  return static_cast<unsigned char>(rm1 * 255);
}

// Stringkonvertierung im Format RRGGBB

CString CColor::GetString() const 
{
  CString color;
  color.Format(L"%02X%02X%02X", GetRed(), GetGreen(), GetBlue());
  return color;
}

bool CColor::SetString(LPCTSTR pcColor) 
{
  ASSERT(pcColor);
  if (NULL == pcColor)
  {
	  return false;
  }
  int r, g, b;
  if (_tscanf(pcColor, L"%2x%2x%2x", &r, &g, &b) != 3) 
  {
    m_color[c_red] = m_color[c_green] = m_color[c_blue] = 0;
    return false;
  }
  else
  {
    m_color[c_red]   = static_cast<unsigned char>(r);
    m_color[c_green] = static_cast<unsigned char>(g);
    m_color[c_blue]  = static_cast<unsigned char>(b);
    m_bIsRGB = true;
    m_bIsHLS = false;
    return true;
  }
}

CString CColor::GetName() const
{
  const_cast<CColor*>(this)->ToRGB();
  int i = numNamedColors; 
  while (i--)
  {
	  if (i >= 0 && i < numNamedColors)
	  {
		  if (m_namedColor[i].color == m_colorref)
		  {
			  break;
		  }
	  }
  }
  if (i < 0) 
  {
	CString strTemp = _T("#");
    return  strTemp + GetString();
  }
  else
  {
	  return m_namedColor[i].name;
  }
}
