#ifndef _FACE_SCHEME_H_
#define _FACE_SCHEME_H_
#pragma   once  

#define STYLECOUNTS  3
#define ERRORNUMBER  9999

#include "ShareFun.h"
#include "faceschemetype.h"
#include "dllexport.h"

typedef struct T_IoViewObject T_IoViewObject;
// 图片序列编号

// 下面的枚举与IDB_GRID_REPORT一一对应， 当增加了图片时， 请一定要记得修改此处
typedef enum E_ReportBmp
{
	ERBNull = 0,	// 不显示任何东西， 仅为了占一个位置
	ERBRise,		// 上升
	ERBKeep,		// 持平
	ERBFall,		// 下跌
	ERBBuy,			// 买
	ERBSell,		// 卖
	//
	ERBCount
}E_ReportBmp;


//////////////////////////////////////////////////////////////////////
//
class CONTROL_EXPORT CFontNode
{
public: 
	CFontNode();
	~CFontNode();

	CFontNode(const CFontNode &node);
	const CFontNode &operator=(const CFontNode &node);

public:
	void			SetFont(const LOGFONT* plf);

public:
	//CFont			m_Font;		// MFC CGdiObject由于对象指针与HANDLE绑定，所以不能放在容器里面，取消保存CFont对象，什么时候需要，什么时候创建
	LOGFONT			m_stLogfont;
};

struct T_SysColorObject;
struct T_SysDefaultFontObject;
struct T_IoViewTypeFaceObject;
struct T_ChildFrameFaceObject;

// 系统面子问题 :）
class CONTROL_EXPORT CFaceScheme
{
public:	


	// 设置业务视图颜色字体时候,影响的范围
	enum E_FaceEffectRange
	{
		EFERSingleIoView = 0,	// 只对当前这个业务视图有效
		EFERSameTypeIoView,		// 对相同类型视图有效
		EFERSameGroupIoView,	// 对相同组的视图有效
		EFERSameTabSplitWnd,	// 对相同标签页的视图有效
		EFERSameChildFrmae,		// 对相同子窗口的视图有效
		EFERCount
	};

	struct T_StyleData	// 每一种风格对应的数据
	{
		CString m_StrSysleName;		// 名称
		int32	m_iIndex;			// 索引, 暂时无用
		COLORREF	m_aColors[ESCCount];		// 颜色数据
		CFontNode	m_aFonts[ESFCount];			// 字体数据
	};
	typedef CArray<T_StyleData, const T_StyleData &> StyleDataArray;

public:
	CFaceScheme();
	~CFaceScheme();
	void							Contruct(CString strUserName);
	const T_IoViewObject* FindIoViewObjectByRuntimeClass(CRuntimeClass * pRunTimeClass);
	bool32							IoViewFaceObjectFromXml();
	CString							IoViewFaceObjectToXml();
	const T_IoViewObject* FindIoViewObjectByXmlName(const CString &StrXmlName);
public:
	static CFaceScheme*				Instance();	// 该类仅允许一个对象
	static void SetIoViewObjects(int nIoViewObjectCount,const T_IoViewObject *pastIoViewObjects);
public:
	static BOOL		CreateFontIndirect(CFont &Font, const LOGFONT* lpLogFont);
public:
	CString							GetSysColorFileDirectory();
	//////////////////////////////////////////////////////////////////////////
	// 颜色
	static	CString					GetKeyNameBackGround();		
	static	CString					GetKeyNameText();		
	static	CString					GetKeyNameRise();		
	static	CString					GetKeyNameKeep();	
	static	CString					GetKeyNameFall();		
	static	CString					GetKeyNameKLineRise();		
	static	CString					GetKeyNameKLineKeep();		
	static	CString					GetKeyNameKLineFall();		
	static	CString					GetKeyNameVolume();		
	static	CString					GetKeyNameAmount();		
	static	CString					GetKeyNameGridLine();		
	static	CString					GetKeyNameGridSelected();		
	static	CString					GetKeyNameChartAxisLine();
	//... fangz0531 xijia
	static  CString					GetKeyNameGridFixedBk();
	//static  CString				GetKeyNameGridFixedText();

	static CString					GetKeyNameGuideLine1();
	static CString					GetKeyNameGuideLine2();
	static CString					GetKeyNameGuideLine3();
	static CString					GetKeyNameGuideLine4();
	static CString					GetKeyNameGuideLine5();
	static CString					GetKeyNameGuideLine6();
	static CString					GetKeyNameGuideLine7();
	static CString					GetKeyNameGuideLine8();
	static CString					GetKeyNameGuideLine9();
	static CString					GetKeyNameGuideLine10();
	static CString					GetKeyNameGuideLine11();
	static CString					GetKeyNameGuideLine12();
	static CString					GetKeyNameGuideLine13();
	static CString					GetKeyNameGuideLine14();
	static CString					GetKeyNameGuideLine15();
	static CString					GetKeyNameGuideLine16();
	//////////////////////////////////////////////////////////////////////////
	/// 字体
	
	static	CString					GetKeyNameBigFontName();
	static	CString					GetKeyNameBigFontHeight();
	static	CString					GetKeyNameBigFontWeight();
	static	CString					GetKeyNameBigFontCharSet();
	static	CString					GetKeyNameBigFontOutPrecision();
	
	static	CString					GetKeyNameNormalFontName();
	static	CString					GetKeyNameNormalFontHeight();
	static	CString					GetKeyNameNormalFontWeight();
	static	CString					GetKeyNameNormalFontCharSet();
	static	CString					GetKeyNameNormalFontOutPrecision();
	
	static	CString					GetKeyNameSmallFontName();
	static	CString					GetKeyNameSmallFontHeight();
	static	CString					GetKeyNameSmallFontWeight();
	static	CString					GetKeyNameSmallFontCharSet();
	static	CString					GetKeyNameSmallFontOutPrecision();

	static	CString					GetKeyNameTextFontName();
	static	CString					GetKeyNameTextFontHeight();
	static	CString					GetKeyNameTextFontWeight();
	static	CString					GetKeyNameTextFontCharSet();
	static	CString					GetKeyNameTextFontOutPrecision();

public:
	void							InitColorsAsDefault(int32 iStyleIndex=1);
	//void							ProviderStyleColorForFrame(int32 iStyleIndex);
	void							InitFontsAsDefault();		// 下次进入时会调用上次设置的属性.不一定是DEFAULT
	void							SetKeyNames(E_SysFont eSysFont,CString& StrName,CString& StrHeight,CString& StrWeight,CString& StrCharSet,CString& StrOutPrecision);

	COLORREF						GetSysColor(E_SysColor eSysColor);
	LOGFONT *						GetSysFont(E_SysFont eSysFont);
	CFont *							GetSysFontObject(E_SysFont eSysFont);
	static const int32				GetSysDefaultFontObjectCount();

	static const T_SysDefaultFontObject * GetSysDefaultFontObject(int32 iCount);

	static const T_SysColorObject *  GetSysColorObject(int32 iCount);		 
	static const int32				 GetSysColorObjectCount();
	static const CString             GetSysStyleArray(int32 iCount);		 
	static const int32				 GetSysStyleArrayCount();
    UINT   							 RedCustomSysStyleData(int32 iIndex,CString StrKeyName);
	CString   						 RedCustomSysStyleDataCString(int32 iIndex, LPCTSTR pwszKeyName, LPCTSTR pwszDefault);

	//
	void							 GetNowUseColors(OUT COLORREF (&aNowUseColors)[ESCCount]);
	void							 SetNowUseColors(const IN  COLORREF (&aNowUseColors)[ESCCount]);
	
	void							 GetNowUseFonts(OUT CFontNode (&aNowUseFonts)[ESFCount]);
	void							 SetNowUseFonts(const IN  CFontNode (&aNowUseFonts)[ESFCount]);
	//
	void							 UpdateChildFrameFaceObjectList(T_ChildFrameFaceObject& Object);
	void							 UpdateIoViewTypeFaceObjectList(T_IoViewTypeFaceObject& Object);

	void							 DelChildFrameFaceObject(CWnd * pChild);	

	bool32							 GetIoViewTypeFaceObject(IN CWnd * pIoView,OUT T_IoViewTypeFaceObject& Object);
	bool32							 GetChildFrameFaceObject(IN CWnd* pChildFrame,OUT T_ChildFrameFaceObject& Object);

	// 
	int32							 GetFontHeight(bool32 bBigger,const int32 iHeightNow);
	COLORREF						 GetActiveColor();

	// 设定风格的作用目前就是将当前的clr font设置为风格的配置
	// 这些风格都是从xml文件中加载的, 不能被修改，在对话框中修改的仅能影响当前的clr font，不影响风格
	// 自定义风格就是设置时，当前的clr font，自定义风格总是处在0号位置
	const StyleDataArray			&GetStyleDataArray() const { return m_aStyleData; }
	bool32							 GetStyleData(int32 iIndex, OUT T_StyleData &StyleData) const;
	bool32							GetCustomStyleColors(OUT T_StyleData &StyleData) const { return GetStyleData(0, StyleData); }
	bool32							SetCustomStyleColors(IN const COLORREF (&aNowUseColors)[ESCCount]); // 设置自定义风格颜色
	bool32							SetCustomStyleFonts(const IN  CFontNode (&aNowUseFonts)[ESFCount]);	// 设置自定义风格字体
	bool32							SetStyleColors(int32 iIndex, IN const COLORREF (&aNowUseColors)[ESCCount]);	// 设置风格颜色
	bool32							SetStyleFonts(int32 iIndex, const IN  CFontNode (&aNowUseFonts)[ESFCount]);	// 设置风格字体
	void							AddStyleData(const T_StyleData &StyleData);		// 新增风格 - TODO
	void							DelStyleData(int32 iIndex);						// 删除风格 - TODO

public:
	E_FaceEffectRange				 m_eFaceEffectRange;
public:	
	
	// 这两个数组是一开始根据整个系统风格从文件中读取的数据(文件不存在或者读取错误时,使用默认值);
	// 程序运行过程中不会更改(设置风格除外),而且是作为取业务视图颜色字体属性错误时,备用的数据;

	COLORREF		m_aSysColors[ESCCount];
	COLORREF		m_ClrOutside;			// 获取系统颜色时， 非正常取值范围内的颜色

	CFontNode		m_aSysFont[ESFCount];
	CFontNode		m_FontOutside;	        // 获取系统字体时， 非正常取值范围内的字体	

	// 这两个数组是用于业务视图颜色字体发生变化时.需要发消息通知给同类型或者同窗口的视图时,其他视图用来更新数据用的.
	// 程序运行中,当颜色,字体发生变化时都会更改;

	COLORREF		m_aNowUseColors[ESCCount];
	CFontNode		m_aNowUseFonts[ESFCount];

	StyleDataArray		m_aStyleData;		// 每种风格对应的颜色与字体
	//int32				m_iCurStyleIndex;	// 当前使用的风格, 每个单位可能使用不同的风格，所以此值无意义

	// 这个数组用来给每个窗口保存一份数据
	CArray<T_IoViewTypeFaceObject,T_IoViewTypeFaceObject&>	m_aIoViewTypeFaceObjectList;
	CArray<T_ChildFrameFaceObject,T_ChildFrameFaceObject&>	m_aChildFrameFaceObjectList;

private:
	static CFaceScheme	*s_Instance;
	static bool32		 s_init;
	static int m_nIoViewObjectCount;
	static const T_IoViewObject *m_pastIoViewObjects;
	CString m_StrUserName;
};
//
typedef struct T_SysColorObject
{
	CString						m_StrName;
	E_SysColor		m_eSysColor;
	COLORREF					m_Color;
	CString						m_StrFileKeyName;	
 	T_SysColorObject(const CString &StrName, const E_SysColor	& eSysColor,const COLORREF & Color,const CString &StrFileKeyName)
 	{
 		m_StrName	 = StrName;
 		m_eSysColor  = eSysColor;
		m_Color      = Color;
		m_StrFileKeyName = StrFileKeyName;
	};

}T_SysColorObject;


typedef struct T_SysDefaultFontObject
{
	CString		m_StrFileKeyName;
	int32		m_iKeyValue;
 	T_SysDefaultFontObject(const CString &StrFileKeyName,const int32 & iKeyValue)
 	{
		m_StrFileKeyName = StrFileKeyName;
		m_iKeyValue		 = iKeyValue;
	};

}T_SysDefaultFontObject;
//

typedef struct T_SysColorSave
{
	CString						m_StrFileKeyName;
	COLORREF					m_Color;
}T_SysColorSave;

typedef struct T_SysFontSave
{	
	E_SysFont		m_eSysFont;
	LOGFONT						m_lfLogFont;
}T_SysFontSave;
//

// 对一种类型的业务视图保存一份颜色字体数据

typedef struct CONTROL_EXPORT T_IoViewTypeFaceObject
{
 	// 构造函数
	CRuntimeClass*				m_pIoViewClass;
	COLORREF					m_aColors[ESCCount];
	
 	T_IoViewTypeFaceObject();
 
	T_IoViewTypeFaceObject(CRuntimeClass* pIoViewClass,const COLORREF (&aColors)[ESCCount],const CFontNode (&aFonts)[ESFCount]);;
 	
	// 拷贝构造函数

	T_IoViewTypeFaceObject(const T_IoViewTypeFaceObject & ObjectSrc);	
	
	// 重载操作符
	
	const T_IoViewTypeFaceObject& operator = (const T_IoViewTypeFaceObject & ObjectSrc);

	CFontNode				*GetFontNodes(){ return m_aFonts; }

	CFontNode					m_aFonts[ESFCount];

}T_IoViewTypeFaceObject;

// 对一个窗口保存一份颜色字体数据

typedef struct CONTROL_EXPORT T_ChildFrameFaceObject
{
	CWnd *						m_pChildFrame;	
	COLORREF					m_aColors[ESCCount];
	CFontNode					m_aFonts[ESFCount];

	// 构造函数
	T_ChildFrameFaceObject(){}

	T_ChildFrameFaceObject(CWnd * pChildFrame,const COLORREF (&aColors)[ESCCount],const CFontNode (&aFonts)[ESFCount]);

	// 拷贝构造函数
	T_ChildFrameFaceObject(T_ChildFrameFaceObject & ObjectSrc);
	
	// 重载操作符
	const T_ChildFrameFaceObject& operator = (const T_ChildFrameFaceObject & ObjectSrc);
}T_ChildFrameFaceObject;

#endif


