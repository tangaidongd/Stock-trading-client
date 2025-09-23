#include "StdAfx.h"
#include "facescheme.h"
#include "PathFactory.h"
#include "coding.h"
#include "FontFactory.h"
#include <string>
#include "tinystr.h"
#include "tinyxml.h"
using std::wstring;
using std::string;
// 视图抢占兄弟视图显示的模式
enum E_IoViewBeyondShow
{
	EIBSNone = 0,		// 安分守己
	EIBSVertical,		// 通天入地
	EIBSHorizontal,		// 横行霸道
	EIBSBoth,			// 唯我独尊

	EIBSCount,
};

enum E_IoViewMerchChangeSrc
{
	EIMCSFalse = 0,		// 普通视图
	EIMCSTrue,			// 报价表类视图

	EIMCSCount,
};

enum E_IoViewGroupType
{
	E_IVGTNone = 0,
	E_IVGTChart,
	E_IVGTReport,
	//	E_IVGTRelative,
	E_IVGTPrice,
	E_IVGTOther,
	E_IVGTCount,
};

typedef struct T_IoViewObject
{
	UINT					m_uID;								// 类型ID
	E_IoViewBeyondShow		m_eBeyondShow;						// 是否强行占领同一切分窗口兄弟的区域进行显示
	E_IoViewMerchChangeSrc	m_eMerchChangeSrc;					// 是否是其他视图商品切换源
	CString					m_StrLongName;						// 长名	
	CString					m_StrShortName;						// 短名
	CString					m_StrTipMsg;						// Tips
	CString					m_StrXmlName;						// Xml 名
	E_IoViewGroupType		m_eGroupType;						// 版面调整中的视图类型

	CRuntimeClass*			m_pIoViewClass;
	bool32					m_bAllowDragAway;						// 是否允许被拖拉到其他窗口

	DWORD					m_dwAttendMerchType;				// 该业务视图类型所适合处理的商品类型，默认全部

	T_IoViewObject(const UINT& uID,
		const E_IoViewBeyondShow& eBeyondShow,
		const E_IoViewMerchChangeSrc& eMerchChangeSrc,
		const CString &StrLongName,
		const CString &StrShortName,
		const CString &StrTipMsg,
		const CString &StrXmlName,
		const E_IoViewGroupType& eGroupType,				   
		CRuntimeClass *pIoViewClass,
		bool32	bAllowDragAway,
		DWORD	dwAttendMerchType = 0xffffffff)
	{
		m_uID				 = uID;
		m_eBeyondShow		 = eBeyondShow;
		m_eMerchChangeSrc	 = eMerchChangeSrc;
		m_StrLongName		 = StrLongName;
		m_StrShortName		 = StrShortName;
		m_StrTipMsg			 = StrTipMsg;
		m_StrXmlName		 = StrXmlName;
		m_eGroupType		 = eGroupType;
		m_pIoViewClass		 = pIoViewClass;
		m_bAllowDragAway	 = bAllowDragAway;
		m_dwAttendMerchType  = dwAttendMerchType;
	};

}T_IoViewObject;

CFaceScheme * CFaceScheme::s_Instance = NULL;
bool32	CFaceScheme::s_init = false;

int CFaceScheme::m_nIoViewObjectCount = 0;
const T_IoViewObject *CFaceScheme::m_pastIoViewObjects = NULL;

//////////////////////////////////////////////////////////////////////////
// XML

const char * KStrElementIoViewFace			= "IoviewFace";
const char * KStrElementAttriIoViewType		= "IoviewType";
const char * KStrElementStyleData		= "StyleData";	// 风格数据，
const char * KStrElementAttriStyleName		= "SDName";	// 风格数据名称
const char * KStrElementAttriStyleIndex		= "SDIndex";	// 风格数据索引

static const char * KStrElementAttriFaceEffectRange = "FaceEffectRange";
const char *KStrElementAttriCurStyle = "CurStyle";

//////////////////////////////////////////////////////////////////////////
/// 颜色
const CString KStrBackground	= L"ESCBackground";
const CString KStrText			= L"ESCText";
const CString KStrChartAxisLine	= L"ESCChartAxisLine";

const CString KStrRise			= L"ESCRise";
const CString KStrKeep			= L"ESCKeep";
const CString KStrFall			= L"ESCFall";
const CString KStrKLineRise		= L"ESCKLineRise";
const CString KStrKLineKeep		= L"ESCKLineKeep";
const CString KStrKLineFall		= L"ESCKLineFall";
const CString KStrVolume		= L"ESCVolume";


const CString KStrAmount		= L"ESCAmount";
const CString KStrGridLine		= L"ESCGridLine";
const CString KStrGridSelected	= L"ESCGridSelected";

//...fangz0531  xinjia
const CString KStrGridFixedBk   = L"ESCGridFixedBk";
//...wangyx 2016/06/08
const CString KStrTitleBkColor  = L"ESCTitleBkColor";
//const CString KStrGridFixedText = L"ESCGridFixedText";

const CString KStrSpliter		= L"ESCSpliter";	
const CString KStrVolumn2		= L"ESCVolumn2";

const CString KStrGuideLine1    = L"ESCGuideLine1";	
const CString KStrGuideLine2    = L"ESCGuideLine2";	
const CString KStrGuideLine3    = L"ESCGuideLine3";	
const CString KStrGuideLine4    = L"ESCGuideLine4";	
const CString KStrGuideLine5    = L"ESCGuideLine5";	
const CString KStrGuideLine6    = L"ESCGuideLine6";	
const CString KStrGuideLine7    = L"ESCGuideLine7";	
const CString KStrGuideLine8    = L"ESCGuideLine8";	
const CString KStrGuideLine9    = L"ESCGuideLine9";	
const CString KStrGuideLine10   = L"ESCGuideLine10";	
const CString KStrGuideLine11   = L"ESCGuideLine11";	
const CString KStrGuideLine12   = L"ESCGuideLine12";	
const CString KStrGuideLine13   = L"ESCGuideLine13";	
const CString KStrGuideLine14   = L"ESCGuideLine14";	
const CString KStrGuideLine15   = L"ESCGuideLine15";	
const CString KStrGuideLine16   = L"ESCGuideLine16";	

//////////////////////////////////////////////////////////////////////////
/// 字体

const CString KStrBigFontName		  = L"BigFontName";
const CString KStrBigFontHeight		  = L"BigFontHeight";
const CString KStrBigFontWeigth		  = L"BigFontWeigth";
const CString KStrBigFontCharSet	  = L"BigFontCharSet";
const CString KStrBigFontOutPrecision = L"BigFontOutPrecision";

const CString KStrNormalFontName		 = L"NormalFontName";
const CString KStrNormalFontHeight		 = L"NormalFontHeight";
const CString KStrNormalFontWeigth		 = L"NormalFontWeigth";
const CString KStrNormalFontCharSet		 = L"NormalFontCharSet";
const CString KStrNormalFontOutPrecision = L"NormalFontOutPrecision"; 

const CString KStrSmallFontName			= L"SmallFontName";
const CString KStrSmallFontHeight		= L"SmallFontHeight";
const CString KStrSmallFontWeigth		= L"SmallFontWeigth";
const CString KStrSmallFontCharSet		= L"SmallFontCharSet";
const CString KStrSmallFontOutPrecision = L"SmallFontOutPrecision"; 

const CString KStrTextFontName			= L"TextFontName";
const CString KStrTextFontHeight		= L"TextFontHeight";
const CString KStrTextFontWeigth		= L"TextFontWeigth";
const CString KStrTextFontCharSet		= L"TextFontCharSet";
const CString KStrTextFontOutPrecision	= L"TextFontOutPrecision"; 
////////////////////////////////////////////
static const T_SysColorObject s_astSysColorObjects[] = 
{
	//... fangz0430# 列表标题=成交量 股票名称=成交额 

	T_SysColorObject(L"背景",			ESCBackground,			0x000000,		KStrBackground),
  	T_SysColorObject(L"文字",			ESCText,				0xC7C1BB,		KStrText),		
  	T_SysColorObject(L"坐标轴",			ESCChartAxisLine,		RGB(50,50,50),	KStrChartAxisLine),
  	T_SysColorObject(L"阳线",			ESCKLineRise,			RGB(255,50,50),	KStrKLineRise),
  	T_SysColorObject(L"平盘线",			ESCKLineKeep,			RGB(255,255,255),KStrKLineKeep),
  	T_SysColorObject(L"阴线",			ESCKLineFall,			RGB(84,255,255), KStrKLineFall),
  	T_SysColorObject(L"列表上涨字",		ESCRise,				0x3D3DFF,		KStrRise),
  	T_SysColorObject(L"列表平盘字",		ESCKeep,				0xD6D0C9,		KStrKeep),
  	T_SysColorObject(L"列表下跌字",		ESCFall,				0x52df00,		KStrFall),
  	T_SysColorObject(L"列表标题",		ESCVolume,				0xC7C1BB,		KStrVolume),
  	T_SysColorObject(L"股票名称",		ESCAmount,				RGB(255,255,255),KStrAmount),
  	T_SysColorObject(L"列表框画线",		ESCGridLine,			RGB(96,96,96),	KStrGridLine),	
  	T_SysColorObject(L"高亮背景色",		ESCGridSelected,		RGB(42,44,118),		KStrGridSelected),
	//... fangz0531 xinjia
	T_SysColorObject(L"表格固定项背景色",ESCGridFixedBk,		RGB(44,50,55),	KStrGridFixedBk),
	T_SysColorObject(L"标题背景颜色",   ESCTitleBkColor,        RGB(44,50,55),	KStrTitleBkColor),
	//T_SysColorObject(L"表格固定项文字色",ESCGridFixedText,	0x00aaaaaa,		KStrGridFixedText),
	T_SysColorObject(L"分割条颜色",		ESCSpliter,				RGB(0,0,0),		KStrSpliter),	
	T_SysColorObject(L"成交量颜色",		ESCVolume2,				0xD6D0C9,		KStrVolumn2),	

  	T_SysColorObject(L"指标线1",		ESCGuideLine1,			0xffffff,		KStrGuideLine1),	
  	T_SysColorObject(L"指标线2",		ESCGuideLine2,			0xffff,			KStrGuideLine2),	
  	T_SysColorObject(L"指标线3",		ESCGuideLine3,			RGB(255,0,255),	KStrGuideLine3),	
  	T_SysColorObject(L"指标线4",		ESCGuideLine4,			0x10d010,		KStrGuideLine4),	
  	T_SysColorObject(L"指标线5",		ESCGuideLine5,			0xff0000,		KStrGuideLine5),	
  	T_SysColorObject(L"指标线6",		ESCGuideLine6,			0x00aaaaaa,		KStrGuideLine6),	
  	T_SysColorObject(L"指标线7",		ESCGuideLine7,			0x001010d0,		KStrGuideLine7),	
  	T_SysColorObject(L"指标线8",		ESCGuideLine8,			0x001010d0,		KStrGuideLine8),	
  	T_SysColorObject(L"指标线9",		ESCGuideLine9,			0x0010d010,		KStrGuideLine9),	
  	T_SysColorObject(L"指标线10",		ESCGuideLine10,			0x0010d010,		KStrGuideLine10),
  	T_SysColorObject(L"指标线11",		ESCGuideLine11,			0x0010d0d0,		KStrGuideLine11),
  	T_SysColorObject(L"指标线12",		ESCGuideLine12,			0xff0000,		KStrGuideLine12),
  	T_SysColorObject(L"指标线13",		ESCGuideLine13,			0xd05050,		KStrGuideLine13),
  	T_SysColorObject(L"指标线14",		ESCGuideLine14,			0x8b2268,		KStrGuideLine14),
  	T_SysColorObject(L"指标线15",		ESCGuideLine15,			0x0010d010,		KStrGuideLine15),
 	T_SysColorObject(L"指标线16",		ESCGuideLine16,			0xd05050,		KStrGuideLine16),
};
const int KSysColorObjectCount = sizeof(s_astSysColorObjects)/sizeof(T_SysColorObject);

//////////////////////////////////////////////////////////////////////////
//
 static const T_SysDefaultFontObject s_astSysDefaulttFontObjects[] =
 {
 	T_SysDefaultFontObject(KStrBigFontHeight, -16),
	T_SysDefaultFontObject(KStrBigFontWeigth,400),
	T_SysDefaultFontObject(KStrBigFontCharSet,0),
	T_SysDefaultFontObject(KStrBigFontOutPrecision,3),
	
 	T_SysDefaultFontObject(KStrNormalFontHeight,-16),
	T_SysDefaultFontObject(KStrNormalFontWeigth,400),
	T_SysDefaultFontObject(KStrNormalFontCharSet,0),
	T_SysDefaultFontObject(KStrNormalFontOutPrecision,3),

 	T_SysDefaultFontObject(KStrSmallFontHeight,-11),
	T_SysDefaultFontObject(KStrSmallFontWeigth,400),
	T_SysDefaultFontObject(KStrSmallFontCharSet,0),
	T_SysDefaultFontObject(KStrSmallFontOutPrecision,3),

	T_SysDefaultFontObject(KStrTextFontHeight,-12),
	T_SysDefaultFontObject(KStrTextFontWeigth,400),
	T_SysDefaultFontObject(KStrTextFontCharSet,0),
	T_SysDefaultFontObject(KStrTextFontOutPrecision,0),
 };

const int KSysDefaultFontCount = sizeof(s_astSysDefaulttFontObjects)/sizeof(T_SysDefaultFontObject);

//////////////////////////////////////////////////////////////////////////
static const CString s_aStrSysStyle[] =
{
		L"自定义",
		L"标准",
		L"红黑",
		L"绿灰"
};
const int KSysSytleCount = sizeof(s_aStrSysStyle)/sizeof(CString);

//////////////////////////////////////////////////////////////////////////
///
const CString KStrFileDirectory1 = L"./config/";
const CString KStrFileDirectory2 = L"/default_face.ini" ;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///


CFontNode::CFontNode()
{
#ifdef _WIN32_WCE
    LOGFONT lf;
    GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &lf);
    SetFont(&lf);
#else // not CE
	NONCLIENTMETRICS metrics = {0};

	OSVERSIONINFO osvi = {0};  
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);  
	GetVersionEx(&osvi);  

	if(osvi.dwMajorVersion <6 )  
		metrics.cbSize = sizeof(NONCLIENTMETRICS) - sizeof(metrics.iPaddedBorderWidth);  
	else  
		metrics.cbSize = sizeof(NONCLIENTMETRICS); 
    VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &metrics, 0));
    SetFont(&(metrics.lfMessageFont));
#endif
}

CFontNode::CFontNode( const CFontNode &node )
{
	SetFont(&node.m_stLogfont);
}

CFontNode::~CFontNode()
{
}
void CFontNode::SetFont(const LOGFONT* plf)
{
    ASSERT(plf);	
    if (!plf) return;
	if ( plf != &m_stLogfont )
	{
		memcpyex(&m_stLogfont, plf, sizeof(LOGFONT));
	}
	// 不需要处理
}

const CFontNode & CFontNode::operator=( const CFontNode &node )
{
	if ( this == &node )
	{
		return node;
	}
	SetFont(&node.m_stLogfont);
	return *this;
}

////////////////////////////////////////////
CFaceScheme::CFaceScheme()
{
	s_Instance = this;
	m_eFaceEffectRange = EFERSingleIoView;
}
void CFaceScheme::Contruct(CString strUserName)
{
	m_StrUserName = strUserName;
	if ( !CFaceScheme::s_init)
	{
		CFaceScheme::s_init = true;
		InitFontsAsDefault();
		InitColorsAsDefault();

		memset(m_aNowUseColors,0,sizeof(m_aNowUseColors));
		//memset(m_aNowUseFonts,0,sizeof(m_aNowUseColors)); // 这~~

		// 先设置成默认的,再从XML 中读数据 

		m_aIoViewTypeFaceObjectList.RemoveAll();
		m_aIoViewTypeFaceObjectList.SetSize(0, m_nIoViewObjectCount*2);	// MFC GdiObject不能放在容器里面，因为Object地址与Handle绑定
		int32 i = 0;
		for(i = 0 ; i < m_nIoViewObjectCount;i++)
		{			
			T_IoViewTypeFaceObject IoViewTypeFaceObject(m_pastIoViewObjects[i].m_pIoViewClass, m_aSysColors, m_aSysFont);
			m_aIoViewTypeFaceObjectList.Add(IoViewTypeFaceObject);
		}

		// 设置几个默认风格都是系统的
		m_aStyleData.RemoveAll();
		for ( i=0; i < KSysSytleCount ; i++ )
		{
			T_StyleData StyleData;
			StyleData.m_StrSysleName = s_aStrSysStyle[i];
			StyleData.m_iIndex = i;
			memcpyex(StyleData.m_aColors, m_aSysColors, sizeof(m_aSysColors));
			memcpyex(StyleData.m_aFonts, m_aSysFont, sizeof(m_aSysFont));

			m_aStyleData.Add( StyleData );
		}		
		
		bool32 b = IoViewFaceObjectFromXml();
		ASSERT( m_aStyleData.GetSize() > 0 && m_aStyleData[0].m_StrSysleName == _T("自定义") );
		//
		m_aChildFrameFaceObjectList.RemoveAll();
		//
	}
}

CFaceScheme::~CFaceScheme()
{
	s_Instance = NULL;
}

CFaceScheme* CFaceScheme::Instance()
{
	return s_Instance;
}

void CFaceScheme::SetIoViewObjects(int nIoViewObjectCount, const T_IoViewObject *pastIoViewObjects)
{
	m_nIoViewObjectCount = nIoViewObjectCount;
	m_pastIoViewObjects = pastIoViewObjects;
}
BOOL CFaceScheme::CreateFontIndirect(CFont &Font, const LOGFONT* lpLogFont)
{
	if (NULL == lpLogFont)
		return FALSE;
	ASSERT( Font.m_hObject == NULL );	// 必须要释放
	if ( Font.m_hObject != NULL )
	{
		Font.DeleteObject();
	}

	// 
	LOGFONT lfCreate;
	memset(&lfCreate, 0, sizeof(lfCreate));
	
	lfCreate.lfHeight = lpLogFont->lfHeight;	
	lfCreate.lfWeight = lpLogFont->lfWeight;

	memcpyex(lfCreate.lfFaceName, lpLogFont->lfFaceName, sizeof(lpLogFont->lfFaceName));
	if (wcscmp(lfCreate.lfFaceName, L"System") == 0)
		lfCreate.lfCharSet = ANSI_CHARSET;
	else
		lfCreate.lfCharSet = DEFAULT_CHARSET;

	lfCreate.lfQuality = DEFAULT_QUALITY;//ANTIALIASED_QUALITY; // 锯齿
	Font.CreateFontIndirect(&lfCreate);

	return TRUE;
}
//lint --e{568} suppress " non-negative quantity is never less than zero"
COLORREF CFaceScheme::GetSysColor(E_SysColor eSysColor)
{
	if (eSysColor < 0 || eSysColor >= ESCCount)
	{
		return m_ClrOutside;
	}
	return m_aSysColors[eSysColor];
}
//lint --e{568} suppress " non-negative quantity is never less than zero"
LOGFONT* CFaceScheme::GetSysFont(E_SysFont eSysFont)
{
	if (eSysFont < 0 || eSysFont >= ESFCount)
		return &m_FontOutside.m_stLogfont;

	return &m_aSysFont[eSysFont].m_stLogfont;
}
//lint --e{568} suppress " non-negative quantity is never less than zero"
CFont* CFaceScheme::GetSysFontObject(E_SysFont eSysFont)
{
	// 由于是单线程，so可以static
	static CFont sFonts[ESFCount+1];	// 额外一个
	
	const LOGFONT *pNowLogFont = NULL;
	if (eSysFont < 0 || eSysFont >= ESFCount)
	{
		eSysFont = ESFCount;
		pNowLogFont = &m_FontOutside.m_stLogfont;
	}
	else
	{
		pNowLogFont = &m_aSysFont[eSysFont].m_stLogfont;
	}
	
	CFont &sFont = sFonts[eSysFont];
	LOGFONT logFont = {0};
	
	
	if ( NULL != sFont.m_hObject && sFont.GetLogFont(&logFont) != 0 /*&& memcmp(pNowLogFont, &logFont, sizeof(LOGFONT)) == 0 */
		&& (_tcscmp(pNowLogFont->lfFaceName, logFont.lfFaceName) == 0
		&& pNowLogFont->lfHeight == logFont.lfHeight
		&& pNowLogFont->lfWeight == logFont.lfWeight
		&& pNowLogFont->lfCharSet == logFont.lfCharSet
		&& pNowLogFont->lfOutPrecision == logFont.lfOrientation))
	{
		return &sFont;
	}
	else
	{
		sFont.DeleteObject();
		CreateFontIndirect(sFont, pNowLogFont);
		return &sFont;
	}
}

const T_SysDefaultFontObject * CFaceScheme::GetSysDefaultFontObject(int32 iCount)
{
	ASSERT(iCount>=0 && iCount< KSysDefaultFontCount);
	return &s_astSysDefaulttFontObjects[iCount];
}
void CFaceScheme::InitColorsAsDefault(int32 iStyleIndex)
{
	m_ClrOutside	= 0x00ffffff;	
	for (int32 i = 0; i<ESCCount; i++)
	{
		E_SysColor eSysColor = E_SysColor(ESCBackground + i);
		m_aSysColors[eSysColor] = RedCustomSysStyleData(iStyleIndex,s_astSysColorObjects[i].m_StrFileKeyName);
	}
}
/*
void CFaceScheme::ProviderStyleColorForFrame(int32 iStyleIndex)
{
	if (iStyleIndex < 1 || iStyleIndex >= KSysSytleCount)
	iStyleIndex = 1;
	for (int32 i = 0; i<ESCCount; i++)
	{
		E_SysColor eSysColor = E_SysColor(ESCBackground + i);
		m_aSysColorsforFrame[eSysColor] = RedCustomSysStyleData(iStyleIndex,s_astSysColorObjects[i].m_StrFileKeyName);
	}
}
*/

UINT  CFaceScheme::RedCustomSysStyleData(int32 iIndex,CString StrKeyName)
{
	UINT uiExist = 0;
	UINT uiResult=0;
	// 从文件读取,如果出错,从程序中获取默认值.
	uiExist = GetPrivateProfileInt(s_aStrSysStyle[iIndex], StrKeyName ,ERRORNUMBER,  GetSysColorFileDirectory());

	if ( ERRORNUMBER!= uiExist)
	{
		uiResult = GetPrivateProfileInt(s_aStrSysStyle[iIndex], StrKeyName ,ERRORNUMBER, GetSysColorFileDirectory());
	}
	else
	{
		int32 i = 0;
		for ( i = 0; i<ESCCount;i++)
		{
			if ( StrKeyName == s_astSysColorObjects[i].m_StrFileKeyName)
			{
				uiResult = s_astSysColorObjects[i].m_Color;
				break;
			}
		}
		for (i =0; i<KSysDefaultFontCount;i++)
		{
			if (StrKeyName == s_astSysDefaulttFontObjects[i].m_StrFileKeyName)
			{
				uiResult = s_astSysDefaulttFontObjects[i].m_iKeyValue;
				break;
			}
		}				
	}
	return uiResult;
}

CString  CFaceScheme::RedCustomSysStyleDataCString(int32 iIndex, LPCTSTR pwszKeyName, LPCTSTR pwszDefault)
{
	CString StrResult;

	GetPrivateProfileString(s_aStrSysStyle[iIndex], pwszKeyName, pwszDefault, StrResult.GetBuffer(MAX_PATH),MAX_PATH,GetSysColorFileDirectory());
	return StrResult;
}

void CFaceScheme::InitFontsAsDefault()
{
	LOGFONT BigFont,NormalFont,SmallFont, TextFont;
	memset(&BigFont, 0, sizeof(BigFont));
	memset(&NormalFont,0,sizeof(NormalFont));
	memset(&SmallFont,0,sizeof(SmallFont));	
	memset(&TextFont,0,sizeof(TextFont));

   	_tcscpy(BigFont.lfFaceName , RedCustomSysStyleDataCString(1, KStrBigFontName, gFontFactory.GetExistFontName(L"微软雅黑")));	//... 
   	BigFont.lfHeight  = RedCustomSysStyleData(1,KStrBigFontHeight);
   	BigFont.lfWeight  = RedCustomSysStyleData(1,KStrBigFontWeigth);
   	BigFont.lfCharSet = RedCustomSysStyleData(1,KStrBigFontCharSet);
 	BigFont.lfOutPrecision = RedCustomSysStyleData(1,KStrBigFontOutPrecision);
	
   	_tcscpy(NormalFont.lfFaceName , RedCustomSysStyleDataCString(1, KStrNormalFontName, gFontFactory.GetExistFontName(L"微软雅黑")));  //...
   	NormalFont.lfHeight  = RedCustomSysStyleData(1,KStrNormalFontHeight);
   	NormalFont.lfWeight  = RedCustomSysStyleData(1,KStrNormalFontWeigth);
   	NormalFont.lfCharSet = RedCustomSysStyleData(1,KStrNormalFontCharSet);
 	NormalFont.lfOutPrecision = RedCustomSysStyleData(1,KStrNormalFontOutPrecision);

   	_tcscpy(SmallFont.lfFaceName , RedCustomSysStyleDataCString(1, KStrSmallFontName, gFontFactory.GetExistFontName(L"微软雅黑")));  //...
   	SmallFont.lfHeight  = RedCustomSysStyleData(1,KStrSmallFontHeight);
   	SmallFont.lfWeight  = RedCustomSysStyleData(1,KStrSmallFontWeigth);
   	SmallFont.lfCharSet = RedCustomSysStyleData(1,KStrSmallFontCharSet);
 	SmallFont.lfOutPrecision = RedCustomSysStyleData(1,KStrSmallFontOutPrecision);
	
	_tcscpy(TextFont.lfFaceName , RedCustomSysStyleDataCString(1, KStrTextFontName, gFontFactory.GetExistFontName(L"微软雅黑")));  //...
   	TextFont.lfHeight  = RedCustomSysStyleData(1,KStrTextFontHeight);
   	TextFont.lfWeight  = RedCustomSysStyleData(1,KStrTextFontWeigth);
   	TextFont.lfCharSet = RedCustomSysStyleData(1,KStrTextFontCharSet);
	TextFont.lfOutPrecision = RedCustomSysStyleData(1,KStrTextFontOutPrecision);
//	_tcscpy(TextFont.lfFaceName , L"Arial");

	m_aSysFont[ESFNormal].SetFont(&NormalFont);
	m_aSysFont[ESFSmall].SetFont(&SmallFont);
	m_aSysFont[ESFBig].SetFont(&BigFont);
	m_aSysFont[ESFText].SetFont(&TextFont);	
		
}
void CFaceScheme::SetKeyNames(E_SysFont eSysFont,CString& StrName,CString& StrHeight,CString& StrWeight,CString& StrCharSet,CString& StrOutPrecision)
{
	if (ESFNormal == eSysFont)
	{		
		StrName			= CFaceScheme::GetKeyNameNormalFontName();
		StrHeight		= CFaceScheme::GetKeyNameNormalFontHeight(); 
		StrWeight		= CFaceScheme::GetKeyNameNormalFontWeight();
		StrCharSet		= CFaceScheme::GetKeyNameNormalFontCharSet();
		StrOutPrecision = CFaceScheme::GetKeyNameNormalFontOutPrecision();		
	}
	else if (ESFSmall == eSysFont)
	{
		
		StrName			= CFaceScheme::GetKeyNameSmallFontName();
		StrHeight		= CFaceScheme::GetKeyNameSmallFontHeight(); 
		StrWeight		= CFaceScheme::GetKeyNameSmallFontWeight();
		StrCharSet		= CFaceScheme::GetKeyNameSmallFontCharSet();
		StrOutPrecision = CFaceScheme::GetKeyNameSmallFontOutPrecision();		
	}
	else if (ESFBig == eSysFont)
	{
		StrName			= CFaceScheme::GetKeyNameBigFontName();
		StrHeight		= CFaceScheme::GetKeyNameBigFontHeight(); 
		StrWeight		= CFaceScheme::GetKeyNameBigFontWeight();
		StrCharSet		= CFaceScheme::GetKeyNameBigFontCharSet();
		StrOutPrecision = CFaceScheme::GetKeyNameBigFontOutPrecision();	
	}
	else if (ESFText == eSysFont)
	{
		StrName			= CFaceScheme::GetKeyNameTextFontName();
		StrHeight		= CFaceScheme::GetKeyNameTextFontHeight(); 
		StrWeight		= CFaceScheme::GetKeyNameTextFontWeight();
		StrCharSet		= CFaceScheme::GetKeyNameTextFontCharSet();
		StrOutPrecision = CFaceScheme::GetKeyNameTextFontOutPrecision();	
	}
}
const T_SysColorObject* CFaceScheme::GetSysColorObject(int32 iCount)
{
	ASSERT(iCount>=0 && iCount< KSysColorObjectCount);
	return &s_astSysColorObjects[iCount];
}

const int32 CFaceScheme::GetSysColorObjectCount()
{
	return KSysColorObjectCount;
}
const CString CFaceScheme::GetSysStyleArray(int32 iCount)
{
	ASSERT(iCount>=0 && iCount< KSysSytleCount );
	if (iCount>=0 && iCount< KSysSytleCount)
	{
		return s_aStrSysStyle[iCount];
	}
	return s_aStrSysStyle[0];
	
}

const int32 CFaceScheme::GetSysDefaultFontObjectCount()
{
	return KSysDefaultFontCount;
}
const int32 CFaceScheme::GetSysStyleArrayCount()
{
	return KSysSytleCount;
}

CString CFaceScheme::GetSysColorFileDirectory()
{
	CString StrDirectory;
	StrDirectory = KStrFileDirectory1 + m_StrUserName + KStrFileDirectory2;

	return StrDirectory;
}

void CFaceScheme::GetNowUseColors(OUT COLORREF (&aNowUseColors)[ESCCount])
{
	memset(aNowUseColors,0,sizeof(aNowUseColors));
	memcpyex(aNowUseColors,m_aNowUseColors,sizeof(m_aNowUseColors));
}

void CFaceScheme::SetNowUseColors(const IN COLORREF (&aNowUseColors)[ESCCount])
{
	memset(m_aNowUseColors,0,sizeof(m_aNowUseColors));
	memcpyex(m_aNowUseColors,aNowUseColors,sizeof(aNowUseColors));

	// 保存到自定义风格中
	SetCustomStyleColors(aNowUseColors);
}

void CFaceScheme::GetNowUseFonts(OUT CFontNode (&aNowUseFonts)[ESFCount])
{
	// 里面有对象，分开操作
	{
		for (int32 i = 0 ;i < ESFCount ; i++)
		{
			aNowUseFonts[i] = m_aNowUseFonts[i];
		}
	}
}

void CFaceScheme::SetNowUseFonts(const IN  CFontNode (&aNowUseFonts)[ESFCount])
{
	// 里面有对象，分开操作
	{
		for (int32 i = 0 ;i < ESFCount ; i++)
		{
			m_aNowUseFonts[i] = aNowUseFonts[i];
		}

		// 保存到自定义风格中
		SetCustomStyleFonts(aNowUseFonts);
	}
}

void CFaceScheme::UpdateChildFrameFaceObjectList(T_ChildFrameFaceObject& Object)
{
	if( NULL == Object.m_pChildFrame)
	{
		return;
	}
	// 没有的话新加,有的话重设
	for(int32 i = 0; i < m_aChildFrameFaceObjectList.GetSize(); i++)
	{
		const T_ChildFrameFaceObject &ObjectLocal = m_aChildFrameFaceObjectList[i];
		
		if ( ObjectLocal.m_pChildFrame == Object.m_pChildFrame )
		{
			m_aChildFrameFaceObjectList.SetAt(i,Object);
			return;
		}
	}

	m_aChildFrameFaceObjectList.Add(Object);
}

void CFaceScheme::UpdateIoViewTypeFaceObjectList(T_IoViewTypeFaceObject& Object)
{
	if ( NULL == Object.m_pIoViewClass)
	{
		return;
	}

	for ( int32 i = 0 ; i < m_aIoViewTypeFaceObjectList.GetSize(); i ++)
	{
		if ( m_aIoViewTypeFaceObjectList[i].m_pIoViewClass == Object.m_pIoViewClass )
		{
			m_aIoViewTypeFaceObjectList[i] = Object;
			return;
		}
	}

	m_aIoViewTypeFaceObjectList.Add(Object);
}

void CFaceScheme::DelChildFrameFaceObject(CWnd * pChild)
{
	if (NULL == pChild)
	{
		return;
	}

	for ( int32 i = m_aChildFrameFaceObjectList.GetSize() - 1 ; i > 0 ; i--)
	{
		if ( m_aChildFrameFaceObjectList[i].m_pChildFrame == pChild)
		{
			m_aChildFrameFaceObjectList.RemoveAt(i);
		}
	}
}

bool32 CFaceScheme::GetIoViewTypeFaceObject(IN CWnd * pIoView,OUT T_IoViewTypeFaceObject& Object)
{
	if ( NULL == pIoView)
	{
		return false;
	}

	for ( int32 i = 0 ; i < m_aIoViewTypeFaceObjectList.GetSize() ; i++)
	{
		if ( pIoView->GetRuntimeClass() == m_aIoViewTypeFaceObjectList[i].m_pIoViewClass)
		{
			Object = m_aIoViewTypeFaceObjectList[i];
			return true;
		}
	}

	return false;
}

bool32 CFaceScheme::GetChildFrameFaceObject(IN CWnd* pChildFrame,OUT T_ChildFrameFaceObject& Object)
{
	if ( NULL == pChildFrame)
	{
		return false;
	}

	for(int32 i = 0 ; i < m_aChildFrameFaceObjectList.GetSize(); i++)
	{
		if ( pChildFrame == m_aChildFrameFaceObjectList[i].m_pChildFrame)
		{
			Object = m_aChildFrameFaceObjectList[i];
			return true;
		}
	}
	return false;
}

COLORREF CFaceScheme::GetActiveColor()
{
	return RGB(251, 206, 3);
}

int32  CFaceScheme::GetFontHeight(bool32 bBigger,const int32 iHeightNow)
{
	// 字体对话框中从 8 号字开始
	int32 iReturnHeigth = iHeightNow;

	#define ALLFONTNUMS 10
	const int32 aHeights[ALLFONTNUMS] = { -11, -12 ,-13 ,-15 ,-16 ,-19 ,-21 ,-24 ,-27 ,-29};
	
	if ( iHeightNow > aHeights[0])
	{
		return aHeights[0];
	}
	
	if ( iHeightNow < aHeights[ALLFONTNUMS-1])
	{
		return aHeights[ALLFONTNUMS-1];
	}

	for ( int32 i = 0 ; i < ALLFONTNUMS; i++)
	{
		//	特殊情况处理
		if (iHeightNow == aHeights[i] )
		{
			//	放大且最大字体高度
			if (bBigger && ALLFONTNUMS -1 == i)
			{
				return aHeights[i];
			}
			//	缩小且最小字体高度
			if (!bBigger && 0 == i)
			{
				return aHeights[i];
			}
		}

		if ( iHeightNow == aHeights[i])
		{
			//	特殊情况处理
			if (iHeightNow == aHeights[i] )
			{
				//	放大且最大字体高度
				if (bBigger && ALLFONTNUMS -1 == i)
				{
					return aHeights[i];
				}
				//	缩小且最小字体高度
				if (!bBigger && 0 == i)
				{
					return aHeights[i];
				}
			}

			// 正好相等:
			if (bBigger)
			{
				// 放大,取下一个.
				if ( i == ALLFONTNUMS -1 )
				{
					return iReturnHeigth;
				}
				else
				{
					return aHeights[i+1];
				}
			}
			else
			{
				// 缩小,取前一个
				if ( i == 0)
				{
					return iReturnHeigth;
				}
				else
				{
					return aHeights[i-1];
				}
			}
		}
		else
		{
			// 大于当前的,小于下一个

			if ( iHeightNow < aHeights[i] && iHeightNow > aHeights[i+1])
			{
				if ( bBigger )
				{
					return aHeights[i+1];
				}
				else
				{
					return aHeights[i];
				}
			}
		}
	}

	return iReturnHeigth;
}

const T_IoViewObject* CFaceScheme::FindIoViewObjectByXmlName(const CString &StrXmlName)
{
	for (int32 i = 0; i < m_nIoViewObjectCount; i++)
	{
		if (m_pastIoViewObjects[i].m_StrXmlName == StrXmlName)
		{
			return &m_pastIoViewObjects[i];
		}
	}

	return NULL;
}

bool32 CFaceScheme::IoViewFaceObjectFromXml()
{
	// 读同类型的视图数据
	CString StrFileName = CPathFactory::GetIoViewFaceFileName();
	CString StrFilePath;
	if ( !CPathFactory::GetExistConfigFileName(StrFilePath, StrFileName, m_StrUserName) )
	{
		return false;	// 没有文件
	}
	TiXmlDocument TDoc;
	if (!TDoc.LoadFile(_Unicode2MultiChar(StrFilePath.GetBuffer(), CP_ACP).c_str()))
	{
		return false;
	}

	TiXmlElement * pRootElement =TDoc.RootElement();	
	if (NULL == pRootElement)
	{
		return false;
	}

	// 取出effectRange属性
	const char *pszAttri = pRootElement->Attribute(KStrElementAttriFaceEffectRange);
	if ( NULL != pszAttri )
	{
		int32 iEffect = atol(pszAttri);
		if ( iEffect >= 0 && iEffect < EFERCount )
		{
			m_eFaceEffectRange = (E_FaceEffectRange)iEffect;		// 如果是工作区的话这个值应该是与mainframe中的一致
		}
	}

	int32 iCurStyleIndex = 0;
	pszAttri = pRootElement->Attribute(KStrElementAttriCurStyle);
	if ( NULL != pszAttri )
	{
		iCurStyleIndex = atol(pszAttri);
	}

	TiXmlElement * pChildElement = pRootElement->FirstChildElement();

	for ( ; pChildElement != NULL ; pChildElement = pChildElement->NextSiblingElement() )
	{
		T_IoViewTypeFaceObject Object;

		const char * pKcValue = pChildElement->Value();
		
		if ( 0 == strcmp(pKcValue,KStrElementIoViewFace))		
		{
			// IoView的数据 
			const char * pKcAttri = pChildElement->Attribute(KStrElementAttriIoViewType);
			
			if ( NULL == pKcAttri)
			{
				return false;
			}

			// 得到类型
			CString StrIoViewXmlName = (CString)pKcAttri;	
			
			const T_IoViewObject* pIoViewTypeFaceObject = FindIoViewObjectByXmlName(StrIoViewXmlName);

			if ( NULL == pIoViewTypeFaceObject )
			{
				continue;
			}
			else
			{
				CRuntimeClass* pRunTimeClass = pIoViewTypeFaceObject->m_pIoViewClass;
				
				if ( NULL == pRunTimeClass)
				{
					continue;
				}
				else
				{
					Object.m_pIoViewClass = pRunTimeClass;
				}

			}

			// 得到颜色
			
			for ( int32 i = 0 ; i < CFaceScheme::Instance()->GetSysColorObjectCount();i ++)
			{
				CString StrAttriName  = CFaceScheme::Instance()->GetSysColorObject(i)->m_StrFileKeyName;
				
				char	aStrAttriName[100];
				UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
				pKcAttri= pChildElement->Attribute(aStrAttriName);
				if (pKcAttri)
				{
					//Object.m_aColors[i] = atol(pKcAttri);
					Object.m_aColors[i] = StrToColorRef(pKcAttri);
				}	
				else
				{
					return false;
				}
			}

			// 得到字体

			CString StrAttriName = CFaceScheme::Instance()->GetKeyNameNormalFontName();
			
			char	aStrAttriName[100];
			UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
			
			pKcAttri = pChildElement->Attribute(aStrAttriName);
			
			if (NULL != pKcAttri)
			{
				// 从XML 中读取中字体数据
				
				LOGFONT NormalFont;
				memset(&NormalFont, 0, sizeof(NormalFont));
				_tcscpy(NormalFont.lfFaceName,_A2W(pKcAttri));
				LONG KeyValue[4];
				for (int32 i =4; i<8; i++)
				{
					StrAttriName = CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_StrFileKeyName;
					
					memset(aStrAttriName,0,sizeof(aStrAttriName));
					UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
					pKcAttri = pChildElement->Attribute(aStrAttriName);
					KeyValue[i-4] = (pKcAttri == NULL)?(CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_iKeyValue):atol(pKcAttri);			
					
					if ( NULL == pKcAttri)
					{
						// 出错就直接返回,不设置这个ChildFrame 的风格了.避免读出错误数据,或者本来就是没有数据的,非要给他加上默认数据
						return false;
					}
				}
				NormalFont.lfHeight			= KeyValue[0];
				NormalFont.lfWeight			= KeyValue[1];
				NormalFont.lfCharSet		= (BYTE)KeyValue[2];
				NormalFont.lfOutPrecision	= (BYTE)KeyValue[3];
				
				Object.m_aFonts[ESFNormal].m_stLogfont = NormalFont;
				Object.m_aFonts[ESFNormal].SetFont(&NormalFont);
			}
			
			StrAttriName = CFaceScheme::Instance()->GetKeyNameSmallFontName();
			
			memset(aStrAttriName,0,sizeof(aStrAttriName));
			UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
			pKcAttri = pChildElement->Attribute(aStrAttriName);
			if (NULL != pKcAttri)
			{
				// 从XML 中读取小字体数据
				
				LOGFONT SmallFont;
				memset(&SmallFont, 0, sizeof(SmallFont));
				_tcscpy(SmallFont.lfFaceName,_A2W(pKcAttri));
				LONG KeyValue[4];
				for (int32 i =8; i<12; i++)
				{
					StrAttriName = CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_StrFileKeyName;
					
					memset(aStrAttriName,0,sizeof(aStrAttriName));
					UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
					pKcAttri = pChildElement->Attribute(aStrAttriName);
					KeyValue[i-8] = (pKcAttri == NULL)?(CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_iKeyValue):atol(pKcAttri);	
					
					if ( NULL == pKcAttri)
					{			
						return false;
					}
				}
				SmallFont.lfHeight			= KeyValue[0];
				SmallFont.lfWeight			= KeyValue[1];
				SmallFont.lfCharSet			= (BYTE)KeyValue[2];
				SmallFont.lfOutPrecision	= (BYTE)KeyValue[3];
				
				Object.m_aFonts[ESFSmall].m_stLogfont = SmallFont;
				Object.m_aFonts[ESFSmall].SetFont(&SmallFont);
			}
			
			StrAttriName = CFaceScheme::Instance()->GetKeyNameBigFontName();
			
			memset(aStrAttriName,0,sizeof(aStrAttriName));
			UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
			pKcAttri = pChildElement->Attribute(aStrAttriName);
			if (NULL != pKcAttri)
			{
				// 从XML 中读取大字体数据
				LOGFONT BigFont;
				memset(&BigFont, 0, sizeof(BigFont));
				_tcscpy(BigFont.lfFaceName,_A2W(pKcAttri));
				LONG KeyValue[4];
				for (int32 i =0; i<4; i++)
				{
					StrAttriName = CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_StrFileKeyName;
					
					memset(aStrAttriName,0,sizeof(aStrAttriName));
					UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
					pKcAttri = pChildElement->Attribute(aStrAttriName);
					KeyValue[i] = (pKcAttri == NULL)?(CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_iKeyValue):atol(pKcAttri);
					
					if ( NULL == pKcAttri)
					{			
						return false;
					}
				}
				BigFont.lfHeight		= KeyValue[0];
				BigFont.lfWeight		= KeyValue[1];
				BigFont.lfCharSet		= (BYTE)KeyValue[2];
				BigFont.lfOutPrecision	= (BYTE)KeyValue[3];
				
				Object.m_aFonts[ESFBig].m_stLogfont = BigFont;
				Object.m_aFonts[ESFBig].SetFont(&BigFont);
			}

			StrAttriName = CFaceScheme::Instance()->GetKeyNameTextFontName();
			
			memset(aStrAttriName,0,sizeof(aStrAttriName));
			UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
			pKcAttri = pChildElement->Attribute(aStrAttriName);
			if (NULL != pKcAttri)
			{
				// 从XML 中读取文本字体数据
				LOGFONT TextFont;
				memset(&TextFont, 0, sizeof(TextFont));
				_tcscpy(TextFont.lfFaceName,_A2W(pKcAttri));
				LONG KeyValue[4];
				for (int32 i =12; i<16; i++)
				{
					StrAttriName = CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_StrFileKeyName;
					
					memset(aStrAttriName,0,sizeof(aStrAttriName));
					UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
					pKcAttri = pChildElement->Attribute(aStrAttriName);
					KeyValue[i-12] = (pKcAttri == NULL)?(CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_iKeyValue):atol(pKcAttri);
					
					if ( NULL == pKcAttri)
					{			
						return false;
					}
				}
				TextFont.lfHeight		= KeyValue[0];
				TextFont.lfWeight		= KeyValue[1];
				TextFont.lfCharSet		= (BYTE)KeyValue[2];
				TextFont.lfOutPrecision	= (BYTE)KeyValue[3];
				
				Object.m_aFonts[ESFText].m_stLogfont = TextFont;
				Object.m_aFonts[ESFText].SetFont(&TextFont);
			}
				
			// 更新这个视图数据
			UpdateIoViewTypeFaceObjectList(Object);
			
		}
		else if ( 0 == strcmp(pKcValue, KStrElementStyleData) )
		{
			// 风格数据
			// 名称  索引
			T_StyleData StyleData;
			const char * pKcAttri = pChildElement->Attribute(KStrElementAttriStyleName);
			
			wstring strTmp;
			if ( NULL == pKcAttri)
			{
				continue;	// 继续
			}
			MultiChar2Unicode(CP_UTF8, pKcAttri, strTmp);
			StyleData.m_StrSysleName = strTmp.c_str();
			if ( StyleData.m_StrSysleName.IsEmpty() )
			{
				continue;
			}

			pKcAttri = pChildElement->Attribute(KStrElementAttriStyleIndex);
			if ( NULL == pKcAttri )
			{
				continue;
			}
			StyleData.m_iIndex = atoi(pKcAttri);
			if ( StyleData.m_iIndex < 0 )
			{
				continue;
			}

			// 得到颜色
			int32 i = 0;
			for ( i = 0 ; i < CFaceScheme::Instance()->GetSysColorObjectCount();i ++)
			{
				CString StrAttriName  = CFaceScheme::Instance()->GetSysColorObject(i)->m_StrFileKeyName;
				
				char	aStrAttriName[100];
				UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
				pKcAttri= pChildElement->Attribute(aStrAttriName);
				if (pKcAttri)
				{
				//	StyleData.m_aColors[i] = atol(pKcAttri);
					StyleData.m_aColors[i] = StrToColorRef(pKcAttri);
				}	
				else
				{
					break;
				}
			}
			if ( i != CFaceScheme::Instance()->GetSysColorObjectCount() )
			{
				continue;
			}

			// 得到字体

			CString StrAttriName = CFaceScheme::Instance()->GetKeyNameNormalFontName();
			
			char	aStrAttriName[100];
			UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
			
			pKcAttri = pChildElement->Attribute(aStrAttriName);
			
			if (NULL != pKcAttri)
			{
				// 从XML 中读取中字体数据
				
				LOGFONT NormalFont;
				memset(&NormalFont, 0, sizeof(NormalFont));
				_tcscpy(NormalFont.lfFaceName,_A2W(pKcAttri));
				LONG KeyValue[4];
				int32 j = 4;
				for (j =4; j<8; j++)
				{
					StrAttriName = CFaceScheme::Instance()->GetSysDefaultFontObject(j)->m_StrFileKeyName;
					
					memset(aStrAttriName,0,sizeof(aStrAttriName));
					UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
					pKcAttri = pChildElement->Attribute(aStrAttriName);
					KeyValue[j-4] = (pKcAttri == NULL)?(CFaceScheme::Instance()->GetSysDefaultFontObject(j)->m_iKeyValue):atol(pKcAttri);			
					
					if ( NULL == pKcAttri)
					{
						// 出错就直接返回,不设置这个ChildFrame 的风格了.避免读出错误数据,或者本来就是没有数据的,非要给他加上默认数据
						break;
					}
				}
				if ( j != 8  )
				{
					continue;
				}
				NormalFont.lfHeight			= KeyValue[0];
				NormalFont.lfWeight			= KeyValue[1];
				NormalFont.lfCharSet		= (BYTE)KeyValue[2];
				NormalFont.lfOutPrecision	= (BYTE)KeyValue[3];
				
				StyleData.m_aFonts[ESFNormal].m_stLogfont = NormalFont;
				StyleData.m_aFonts[ESFNormal].SetFont(&NormalFont);
			}
			
			StrAttriName = CFaceScheme::Instance()->GetKeyNameSmallFontName();
			
			memset(aStrAttriName,0,sizeof(aStrAttriName));
			UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
			pKcAttri = pChildElement->Attribute(aStrAttriName);
			if (NULL != pKcAttri)
			{
				// 从XML 中读取小字体数据
				
				LOGFONT SmallFont;
				memset(&SmallFont, 0, sizeof(SmallFont));
				_tcscpy(SmallFont.lfFaceName,_A2W(pKcAttri));
				LONG KeyValue[4];
				int32 j = 8;
				for ( j =8; j<12; j++)
				{
					StrAttriName = CFaceScheme::Instance()->GetSysDefaultFontObject(j)->m_StrFileKeyName;
					
					memset(aStrAttriName,0,sizeof(aStrAttriName));
					UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
					pKcAttri = pChildElement->Attribute(aStrAttriName);
					KeyValue[j-8] = (pKcAttri == NULL)?(CFaceScheme::Instance()->GetSysDefaultFontObject(j)->m_iKeyValue):atol(pKcAttri);	
					
					if ( NULL == pKcAttri)
					{			
						break;
					}
				}
				if ( j != 12 )
				{
					continue;
				}
				SmallFont.lfHeight			= KeyValue[0];
				SmallFont.lfWeight			= KeyValue[1];
				SmallFont.lfCharSet			= (BYTE)KeyValue[2];
				SmallFont.lfOutPrecision	= (BYTE)KeyValue[3];
				
				StyleData.m_aFonts[ESFSmall].m_stLogfont = SmallFont;
				StyleData.m_aFonts[ESFSmall].SetFont(&SmallFont);
			}
			
			StrAttriName = CFaceScheme::Instance()->GetKeyNameBigFontName();
			
			memset(aStrAttriName,0,sizeof(aStrAttriName));
			UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
			pKcAttri = pChildElement->Attribute(aStrAttriName);
			if (NULL != pKcAttri)
			{
				// 从XML 中读取大字体数据
				LOGFONT BigFont;
				memset(&BigFont, 0, sizeof(BigFont));
				_tcscpy(BigFont.lfFaceName,_A2W(pKcAttri));
				LONG KeyValue[4];
				int32 j = 0;
				for (j =0; j<4; j++)
				{
					StrAttriName = CFaceScheme::Instance()->GetSysDefaultFontObject(j)->m_StrFileKeyName;
					
					memset(aStrAttriName,0,sizeof(aStrAttriName));
					UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
					pKcAttri = pChildElement->Attribute(aStrAttriName);
					KeyValue[j] = (pKcAttri == NULL)?(CFaceScheme::Instance()->GetSysDefaultFontObject(j)->m_iKeyValue):atol(pKcAttri);
					
					if ( NULL == pKcAttri)
					{			
						break;
					}
				}
				if ( j != 4 )
				{
					continue;
				}
				BigFont.lfHeight		= KeyValue[0];
				BigFont.lfWeight		= KeyValue[1];
				BigFont.lfCharSet		= (BYTE)KeyValue[2];
				BigFont.lfOutPrecision	= (BYTE)KeyValue[3];
				
				StyleData.m_aFonts[ESFBig].m_stLogfont = BigFont;
				StyleData.m_aFonts[ESFBig].SetFont(&BigFont);
			}
				
			// 更新
			bool32 bAppend = true;
			for ( i =0; i < m_aStyleData.GetSize() ; i++ )
			{
				if ( m_aStyleData[i].m_StrSysleName == StyleData.m_StrSysleName )
				{
					m_aStyleData[i] = StyleData;	// 更新它
					bAppend = false;
					break;
				}
			}
			if ( bAppend )
			{
				m_aStyleData.Add(StyleData);
			}
		}
	}
	return true;
}

const T_IoViewObject* CFaceScheme::FindIoViewObjectByRuntimeClass(CRuntimeClass * pRunTimeClass)
{
	if ( NULL == pRunTimeClass)
	{
		return NULL;
	}
	for ( int32 i = 0 ; i < m_nIoViewObjectCount ; i++)
	{
		if ( m_pastIoViewObjects[i].m_pIoViewClass == pRunTimeClass)
		{
			return &m_pastIoViewObjects[i];
		}
	}

	return NULL;
}

CString CFaceScheme::IoViewFaceObjectToXml()
{
	// 保存同类型的视图数据
	CString StrPath = CPathFactory::GetIoViewFacePath(m_StrUserName);

	std::string strFilePath = _Unicode2MultiChar(StrPath);

	CString StrThis;
	StrThis  = L"<?xml version =\"1.0\" encoding=\"utf-8\" ?> \n";
	CString StrHeader;
	StrHeader.Format(L"<XMLDATA version=\"1.0\" app = \"ggtong\" data = \"IoViewFace\" %s=\"%d\" >\n", 
		CString(KStrElementAttriFaceEffectRange).GetBuffer(), m_eFaceEffectRange
		);		// 将这个属性保存在这里
	StrThis += StrHeader;
	
	int32 i = 0;
	for (i = 0 ; i < m_aIoViewTypeFaceObjectList.GetSize(); i++)
	{
		CString StrObject;
		
		T_IoViewTypeFaceObject Object = m_aIoViewTypeFaceObjectList.GetAt(i);

		CString StrIoViewName = FindIoViewObjectByRuntimeClass(Object.m_pIoViewClass)->m_StrXmlName;

		StrObject.Format(L"<%s %s=\"%s\" ",CString(KStrElementIoViewFace).GetBuffer(),CString(KStrElementAttriIoViewType).GetBuffer(),StrIoViewName.GetBuffer());
		
		// 颜色
	
		CString StrColor = L"";
		
		int32 j = 0;
		for ( j = 0 ; j < ESCCount ; j++ )
		{	
			CString StrColorTmp;
			StrColorTmp = ColorRefToStr(Object.m_aColors[j]);

			CString StrKey;		
			StrKey.Format(L" %s=\"%s\" ", CString(CFaceScheme::Instance()->GetSysColorObject(j)->m_StrFileKeyName).GetBuffer(), StrColorTmp.GetBuffer());
			//StrKey.Format(L" %s=\"%i\" ", CFaceScheme::Instance()->GetSysColorObject(j)->m_StrFileKeyName ,Object.m_aColors[j]);		
			StrColor += StrKey;
		}
		
		StrObject += StrColor;

		// 字体

		CString StrFont = L"";

		CString StrFontName,StrHeight,StrWeight,StrCharSet,StrOutPrecision;	
		
		for ( j = 0 ; j < ESFCount ; j ++)
		{					
			CString StrKey;
			
			CFaceScheme::Instance()->SetKeyNames(E_SysFont(j),StrFontName,StrHeight,StrWeight,StrCharSet,StrOutPrecision);
			
			StrKey.Format(L" %s=\"%s\" %s=\"%i\" %s=\"%i\" %s=\"%i\" %s=\"%i\"",
				StrFontName.GetBuffer(), Object.m_aFonts[j].m_stLogfont.lfFaceName,
				StrHeight.GetBuffer(),Object.m_aFonts[j].m_stLogfont.lfHeight,		
				StrWeight.GetBuffer(),Object.m_aFonts[j].m_stLogfont.lfWeight,
				StrCharSet.GetBuffer(),Object.m_aFonts[j].m_stLogfont.lfCharSet,
				StrOutPrecision.GetBuffer(),Object.m_aFonts[j].m_stLogfont.lfOutPrecision);

			StrFont += StrKey;
		}			

		// 
		StrObject += StrFont;
		StrObject += L" />\n ";
		//

		StrThis += StrObject;
	}

	// 风格部分
	for (i = 0 ; i < m_aStyleData.GetSize(); i++)
	{
		CString StrObject;
		const T_StyleData &StyleData = m_aStyleData[i];
		
		StrObject.Format(L"<%s %s=\"%s\" %s=\"%d\" ",CString(KStrElementStyleData).GetBuffer(),
			CString(KStrElementAttriStyleName).GetBuffer(), CString(StyleData.m_StrSysleName).GetBuffer(),
			CString(KStrElementAttriStyleIndex).GetBuffer(), StyleData.m_iIndex);
		
		// 颜色
		CString StrColor = L"";
		
		int32 j = 0;
		for ( j = 0 ; j < ESCCount ; j++ )
		{	
			CString StrColorTmp;
			StrColorTmp = ColorRefToStr(StyleData.m_aColors[j]);

			CString StrKey;		
			StrKey.Format(L" %s=\"%s\" ", CString(CFaceScheme::Instance()->GetSysColorObject(j)->m_StrFileKeyName).GetBuffer() ,StrColorTmp.GetBuffer());	
			//StrKey.Format(L" %s=\"%i\" ", CFaceScheme::Instance()->GetSysColorObject(j)->m_StrFileKeyName ,StyleData.m_aColors[j]);		
			StrColor += StrKey;
		}
		
		StrObject += StrColor;
		
		// 字体
		
		CString StrFont = L"";
		
		CString StrFontName,StrHeight,StrWeight,StrCharSet,StrOutPrecision;	
		
		for ( j = 0 ; j < ESFCount ; j ++)
		{					
			CString StrKey;
			
			CFaceScheme::Instance()->SetKeyNames(E_SysFont(j),StrFontName,StrHeight,StrWeight,StrCharSet,StrOutPrecision);
			
			StrKey.Format(L" %s=\"%s\" %s=\"%i\" %s=\"%i\" %s=\"%i\" %s=\"%i\"",
				StrFontName.GetBuffer(), StyleData.m_aFonts[j].m_stLogfont.lfFaceName,
				StrHeight.GetBuffer(), StyleData.m_aFonts[j].m_stLogfont.lfHeight,		
				StrWeight.GetBuffer(), StyleData.m_aFonts[j].m_stLogfont.lfWeight,
				StrCharSet.GetBuffer(), StyleData.m_aFonts[j].m_stLogfont.lfCharSet,
				StrOutPrecision.GetBuffer(), StyleData.m_aFonts[j].m_stLogfont.lfOutPrecision);
			
			StrFont += StrKey;
		}			
		
		// 
		StrObject += StrFont;
		StrObject += L" />\n ";
		//
		
		StrThis += StrObject;
	}

	StrThis += L"</XMLDATA>";
	// 保存XML
	SaveXmlFile(strFilePath.c_str(), StrThis);
	//
	return StrThis;
}

//////////////////////////////////////////////////////////////////////////

CString CFaceScheme::GetKeyNameBackGround()
{
	return KStrBackground;
}
CString CFaceScheme::GetKeyNameText()
{
	return KStrText;
}
CString CFaceScheme::GetKeyNameRise()
{
	return KStrRise;
}
CString CFaceScheme::GetKeyNameKeep()
{
	return KStrKeep;
}
CString CFaceScheme::GetKeyNameFall()
{
	return KStrFall;
}
CString CFaceScheme::GetKeyNameKLineRise()
{
	return KStrKLineRise;
}
CString CFaceScheme::GetKeyNameKLineKeep()
{
	return KStrKLineKeep;
}
CString CFaceScheme::GetKeyNameKLineFall()
{
	return KStrKLineFall;
}
CString CFaceScheme::GetKeyNameVolume()
{
	return KStrVolume;
}
CString CFaceScheme::GetKeyNameAmount()
{
	return KStrAmount;
}
CString CFaceScheme::GetKeyNameGridLine()
{
	return KStrGridLine;
}
CString CFaceScheme::GetKeyNameGridSelected()
{
	return KStrGridSelected;
}
CString CFaceScheme::GetKeyNameChartAxisLine()
{
	return KStrChartAxisLine;
}
//... fangz0531 xinjia
CString CFaceScheme::GetKeyNameGridFixedBk()
{
	
	return KStrGridFixedBk;
}
// CString CFaceScheme::GetKeyNameGridFixedText()
// {
// 	return KStrGridFixedText;
//}
CString CFaceScheme::GetKeyNameGuideLine1()
{
	return KStrGuideLine1;
}
CString CFaceScheme::GetKeyNameGuideLine2()
{
	return KStrGuideLine2;
}
CString CFaceScheme::GetKeyNameGuideLine3()
{
	return KStrGuideLine3;
}

CString CFaceScheme::GetKeyNameGuideLine4()
{
	return KStrGuideLine4;
}
CString CFaceScheme::GetKeyNameGuideLine5()
{
	return KStrGuideLine5;
}
CString CFaceScheme::GetKeyNameGuideLine6()
{
	return KStrGuideLine6;
}

CString CFaceScheme::GetKeyNameGuideLine7()
{
	return KStrGuideLine7;
}
CString CFaceScheme::GetKeyNameGuideLine8()
{
	return KStrGuideLine8;
}
CString CFaceScheme::GetKeyNameGuideLine9()
{
	return KStrGuideLine9;
}

CString CFaceScheme::GetKeyNameGuideLine10()
{
	return KStrGuideLine10;
}

CString CFaceScheme::GetKeyNameGuideLine11()
{
	return KStrGuideLine11;
}
CString CFaceScheme::GetKeyNameGuideLine12()
{
	return KStrGuideLine12;
}
CString CFaceScheme::GetKeyNameGuideLine13()
{
	return KStrGuideLine13;
}		
CString CFaceScheme::GetKeyNameGuideLine14()
{
	return KStrGuideLine14;
}		
CString CFaceScheme::GetKeyNameGuideLine15()
{
	return KStrGuideLine15;
}		
		
CString CFaceScheme::GetKeyNameGuideLine16()
{
	return KStrGuideLine16;
}
CString CFaceScheme::GetKeyNameBigFontName()
{
	return KStrBigFontName;
}
CString CFaceScheme::GetKeyNameBigFontHeight()
{
	return KStrBigFontHeight;
}
CString CFaceScheme::GetKeyNameBigFontWeight()
{
	return KStrBigFontWeigth;
}
CString CFaceScheme::GetKeyNameBigFontCharSet()
{
	return KStrBigFontCharSet;
}
CString CFaceScheme::GetKeyNameBigFontOutPrecision()
{
	return KStrBigFontOutPrecision;
}
CString CFaceScheme::GetKeyNameNormalFontName()
{
	return KStrNormalFontName;
}
CString CFaceScheme::GetKeyNameNormalFontHeight()
{
	return KStrNormalFontHeight;
}
CString CFaceScheme::GetKeyNameNormalFontWeight()
{
	return KStrNormalFontWeigth;
}
CString CFaceScheme::GetKeyNameNormalFontCharSet()
{
	return KStrNormalFontCharSet;
}
CString CFaceScheme::GetKeyNameNormalFontOutPrecision()
{
	return KStrNormalFontOutPrecision;
}

CString CFaceScheme::GetKeyNameSmallFontName()
{
	return KStrSmallFontName;
}
CString CFaceScheme::GetKeyNameSmallFontHeight()
{
	return KStrSmallFontHeight;
}
CString CFaceScheme::GetKeyNameSmallFontWeight()
{
	return KStrSmallFontWeigth;
}
CString CFaceScheme::GetKeyNameSmallFontCharSet()
{
	return KStrSmallFontCharSet;
}
CString CFaceScheme::GetKeyNameSmallFontOutPrecision()
{
	return KStrSmallFontOutPrecision;
}

bool32 CFaceScheme::GetStyleData( int32 iIndex, OUT T_StyleData &StyleData ) const
{
	if ( iIndex >= 0 && iIndex < m_aStyleData.GetSize() )
	{
		StyleData = m_aStyleData[iIndex];
		return true;
	}
	return false;
}

bool32 CFaceScheme::SetCustomStyleColors( IN const COLORREF (&aNowUseColors)[ESCCount] )
{
	if (  m_aStyleData.GetSize() > 0 )
	{
		ASSERT( m_aStyleData[0].m_StrSysleName == _T("自定义") );
		memcpyex(m_aStyleData[0].m_aColors, aNowUseColors, sizeof(COLORREF)*ESCCount);
		return true;
	}
	ASSERT( 0 );
	return false;
}

bool32 CFaceScheme::SetCustomStyleFonts( const IN CFontNode (&aNowUseFonts)[ESFCount] )
{
	if (  m_aStyleData.GetSize() > 0 )
	{
		ASSERT( m_aStyleData[0].m_StrSysleName == _T("自定义") );
		memcpyex(m_aStyleData[0].m_aFonts, aNowUseFonts, sizeof(CFontNode)*ESFCount);
		return true;
	}
	ASSERT( 0 );
	return false;
}

bool32 CFaceScheme::SetStyleColors( int32 iIndex, IN const COLORREF (&aNowUseColors)[ESCCount] )
{
	if (  m_aStyleData.GetSize() > iIndex && iIndex >= 0 )
	{
		memcpyex(m_aStyleData[iIndex].m_aColors, aNowUseColors, sizeof(COLORREF)*ESCCount);
		return true;
	}
	ASSERT( 0 );
	return false;
}

bool32 CFaceScheme::SetStyleFonts( int32 iIndex, const IN CFontNode (&aNowUseFonts)[ESFCount] )
{
	if (  m_aStyleData.GetSize() > iIndex && iIndex >= 0 )
	{
		memcpyex(m_aStyleData[iIndex].m_aFonts, aNowUseFonts, sizeof(CFontNode)*ESFCount);
		return true;
	}
	ASSERT( 0 );
	return false;
}

T_IoViewTypeFaceObject::T_IoViewTypeFaceObject( CRuntimeClass* pIoViewClass,const COLORREF (&aColors)[ESCCount],const CFontNode (&aFonts)[ESFCount] )
{
	m_pIoViewClass = pIoViewClass;
	
	memset(m_aColors,0,sizeof(m_aColors));
	memcpyex(m_aColors,aColors,sizeof(m_aColors));
	
// 	memset(m_aFonts,0,sizeof(m_aFonts));
// 	memcpyex(m_aFonts,aFonts,sizeof(m_aFonts));
	for ( int i=0; i < ESFCount ; i++ )
	{
		m_aFonts[i] = aFonts[i];
	}
}
//CRuntimeClass对于每个mfc类都具有唯一性，它以静态成员变量存在于类中。所以不应该以new形式构造
//lint --e{1554} suppress "direct point copy of member"
T_IoViewTypeFaceObject::T_IoViewTypeFaceObject( const T_IoViewTypeFaceObject & ObjectSrc )
{
	m_pIoViewClass = ObjectSrc.m_pIoViewClass;
	memcpyex(m_aColors,ObjectSrc.m_aColors,sizeof(m_aColors));
	//memcpyex(m_aFonts,ObjectSrc.m_aFonts,sizeof(m_aFonts));
	for ( int i=0; i < ESFCount ; i++ )
	{
		m_aFonts[i] = ObjectSrc.m_aFonts[i];
	}
}

T_IoViewTypeFaceObject::T_IoViewTypeFaceObject()
{
	m_pIoViewClass = NULL;
	memset(m_aColors, 0, sizeof(m_aColors));
}
//CRuntimeClass对于每个mfc类都具有唯一性，它以静态成员变量存在于类中。所以不应该以new形式构造
//lint --e{1555} suppress "direct point copy of member"
const T_IoViewTypeFaceObject& T_IoViewTypeFaceObject::operator=( const T_IoViewTypeFaceObject & ObjectSrc )
{
	if ( this == &ObjectSrc )
	{
		return ObjectSrc;
	}
	m_pIoViewClass = ObjectSrc.m_pIoViewClass;
	memcpyex(m_aColors,ObjectSrc.m_aColors,sizeof(m_aColors));
	//memcpyex(m_aFonts,ObjectSrc.m_aFonts,sizeof(m_aFonts));	
	for ( int i=0; i < ESFCount ; i++ )
	{
		m_aFonts[i] = ObjectSrc.m_aFonts[i];
	}
	
	return *this;
}

T_ChildFrameFaceObject::T_ChildFrameFaceObject( CWnd * pChildFrame,const COLORREF (&aColors)[ESCCount],const CFontNode (&aFonts)[ESFCount] )
{
	m_pChildFrame = pChildFrame;
	memset(m_aColors,0,sizeof(m_aColors));
	memcpyex(m_aColors,aColors,sizeof(aColors));
	
// 	memset(m_aFonts,0,sizeof(m_aFonts));
// 	memcpyex(m_aFonts,aFonts,sizeof(aFonts));
// 	
// 	for ( int32 i = 0 ; i < ESFCount; i++)
// 	{
// 		m_aFonts[i].m_Font.DeleteObject();
// 		bool32 bSuccess = m_aFonts[i].m_Font.CreateFontIndirect(&(m_aFonts[i].m_stLogfont));
// 	}
	for ( int i=0; i < ESFCount ; i++ )
	{
		m_aFonts[i] = aFonts[i];
	}
}

T_ChildFrameFaceObject::T_ChildFrameFaceObject( T_ChildFrameFaceObject & ObjectSrc )
{
	m_pChildFrame = ObjectSrc.m_pChildFrame;
	memset(m_aColors,0,sizeof(m_aColors));
	memcpyex(m_aColors,ObjectSrc.m_aColors,sizeof(m_aColors));
	
// 	memset(m_aFonts,0,sizeof(m_aFonts));
// 	memcpyex(m_aFonts,ObjectSrc.m_aFonts,sizeof(m_aFonts));	
// 	
// 	for ( int32 i = 0 ; i < ESFCount; i++)
// 	{
// 		m_aFonts[i].m_Font.DeleteObject();
// 		bool32 bSuccess = m_aFonts[i].m_Font.CreateFontIndirect(&(m_aFonts[i].m_stLogfont));
// 	}
	for ( int i=0; i < ESFCount ; i++ )
	{
		m_aFonts[i] = ObjectSrc.m_aFonts[i];
	}
}

const T_ChildFrameFaceObject& T_ChildFrameFaceObject::operator=( const T_ChildFrameFaceObject & ObjectSrc )
{
	if ( this == &ObjectSrc )
	{
		return ObjectSrc;
	}
	m_pChildFrame = ObjectSrc.m_pChildFrame;
	memset(m_aColors,0,sizeof(m_aColors));
	memcpyex(m_aColors,ObjectSrc.m_aColors,sizeof(m_aColors));
	
// 	memset(m_aFonts,0,sizeof(m_aFonts));
// 	memcpyex(m_aFonts,ObjectSrc.m_aFonts,sizeof(m_aFonts));	
// 	
// 	for ( int32 i = 0 ; i < ESFCount; i++)
// 	{
// 		m_aFonts[i].m_Font.DeleteObject();
// 		bool32 bSuccess = m_aFonts[i].m_Font.CreateFontIndirect(&(m_aFonts[i].m_stLogfont));
// 	}
	for ( int i=0; i < ESFCount ; i++ )
	{
		m_aFonts[i] = ObjectSrc.m_aFonts[i];
	}
	
	return *this;
}

CString CFaceScheme::GetKeyNameTextFontName()
{
	return KStrTextFontName;
}
CString CFaceScheme::GetKeyNameTextFontHeight()
{
	return KStrTextFontHeight;
}
CString CFaceScheme::GetKeyNameTextFontWeight()
{
	return KStrTextFontWeigth;
}
CString CFaceScheme::GetKeyNameTextFontCharSet()
{
	return KStrTextFontCharSet;
}
CString CFaceScheme::GetKeyNameTextFontOutPrecision()
{
	return KStrTextFontOutPrecision;
}
