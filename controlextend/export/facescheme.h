#ifndef _FACE_SCHEME_H_
#define _FACE_SCHEME_H_
#pragma   once  

#define STYLECOUNTS  3
#define ERRORNUMBER  9999

#include "ShareFun.h"
#include "faceschemetype.h"
#include "dllexport.h"

typedef struct T_IoViewObject T_IoViewObject;
// ͼƬ���б��

// �����ö����IDB_GRID_REPORTһһ��Ӧ�� ��������ͼƬʱ�� ��һ��Ҫ�ǵ��޸Ĵ˴�
typedef enum E_ReportBmp
{
	ERBNull = 0,	// ����ʾ�κζ����� ��Ϊ��ռһ��λ��
	ERBRise,		// ����
	ERBKeep,		// ��ƽ
	ERBFall,		// �µ�
	ERBBuy,			// ��
	ERBSell,		// ��
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
	//CFont			m_Font;		// MFC CGdiObject���ڶ���ָ����HANDLE�󶨣����Բ��ܷ����������棬ȡ������CFont����ʲôʱ����Ҫ��ʲôʱ�򴴽�
	LOGFONT			m_stLogfont;
};

struct T_SysColorObject;
struct T_SysDefaultFontObject;
struct T_IoViewTypeFaceObject;
struct T_ChildFrameFaceObject;

// ϵͳ�������� :��
class CONTROL_EXPORT CFaceScheme
{
public:	


	// ����ҵ����ͼ��ɫ����ʱ��,Ӱ��ķ�Χ
	enum E_FaceEffectRange
	{
		EFERSingleIoView = 0,	// ֻ�Ե�ǰ���ҵ����ͼ��Ч
		EFERSameTypeIoView,		// ����ͬ������ͼ��Ч
		EFERSameGroupIoView,	// ����ͬ�����ͼ��Ч
		EFERSameTabSplitWnd,	// ����ͬ��ǩҳ����ͼ��Ч
		EFERSameChildFrmae,		// ����ͬ�Ӵ��ڵ���ͼ��Ч
		EFERCount
	};

	struct T_StyleData	// ÿһ�ַ���Ӧ������
	{
		CString m_StrSysleName;		// ����
		int32	m_iIndex;			// ����, ��ʱ����
		COLORREF	m_aColors[ESCCount];		// ��ɫ����
		CFontNode	m_aFonts[ESFCount];			// ��������
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
	static CFaceScheme*				Instance();	// ���������һ������
	static void SetIoViewObjects(int nIoViewObjectCount,const T_IoViewObject *pastIoViewObjects);
public:
	static BOOL		CreateFontIndirect(CFont &Font, const LOGFONT* lpLogFont);
public:
	CString							GetSysColorFileDirectory();
	//////////////////////////////////////////////////////////////////////////
	// ��ɫ
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
	/// ����
	
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
	void							InitFontsAsDefault();		// �´ν���ʱ������ϴ����õ�����.��һ����DEFAULT
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

	// �趨��������Ŀǰ���ǽ���ǰ��clr font����Ϊ��������
	// ��Щ����Ǵ�xml�ļ��м��ص�, ���ܱ��޸ģ��ڶԻ������޸ĵĽ���Ӱ�쵱ǰ��clr font����Ӱ����
	// �Զ������������ʱ����ǰ��clr font���Զ��������Ǵ���0��λ��
	const StyleDataArray			&GetStyleDataArray() const { return m_aStyleData; }
	bool32							 GetStyleData(int32 iIndex, OUT T_StyleData &StyleData) const;
	bool32							GetCustomStyleColors(OUT T_StyleData &StyleData) const { return GetStyleData(0, StyleData); }
	bool32							SetCustomStyleColors(IN const COLORREF (&aNowUseColors)[ESCCount]); // �����Զ�������ɫ
	bool32							SetCustomStyleFonts(const IN  CFontNode (&aNowUseFonts)[ESFCount]);	// �����Զ���������
	bool32							SetStyleColors(int32 iIndex, IN const COLORREF (&aNowUseColors)[ESCCount]);	// ���÷����ɫ
	bool32							SetStyleFonts(int32 iIndex, const IN  CFontNode (&aNowUseFonts)[ESFCount]);	// ���÷������
	void							AddStyleData(const T_StyleData &StyleData);		// ������� - TODO
	void							DelStyleData(int32 iIndex);						// ɾ����� - TODO

public:
	E_FaceEffectRange				 m_eFaceEffectRange;
public:	
	
	// ������������һ��ʼ��������ϵͳ�����ļ��ж�ȡ������(�ļ������ڻ��߶�ȡ����ʱ,ʹ��Ĭ��ֵ);
	// �������й����в������(���÷�����),��������Ϊȡҵ����ͼ��ɫ�������Դ���ʱ,���õ�����;

	COLORREF		m_aSysColors[ESCCount];
	COLORREF		m_ClrOutside;			// ��ȡϵͳ��ɫʱ�� ������ȡֵ��Χ�ڵ���ɫ

	CFontNode		m_aSysFont[ESFCount];
	CFontNode		m_FontOutside;	        // ��ȡϵͳ����ʱ�� ������ȡֵ��Χ�ڵ�����	

	// ����������������ҵ����ͼ��ɫ���巢���仯ʱ.��Ҫ����Ϣ֪ͨ��ͬ���ͻ���ͬ���ڵ���ͼʱ,������ͼ�������������õ�.
	// ����������,����ɫ,���巢���仯ʱ�������;

	COLORREF		m_aNowUseColors[ESCCount];
	CFontNode		m_aNowUseFonts[ESFCount];

	StyleDataArray		m_aStyleData;		// ÿ�ַ���Ӧ����ɫ������
	//int32				m_iCurStyleIndex;	// ��ǰʹ�õķ��, ÿ����λ����ʹ�ò�ͬ�ķ�����Դ�ֵ������

	// �������������ÿ�����ڱ���һ������
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

// ��һ�����͵�ҵ����ͼ����һ����ɫ��������

typedef struct CONTROL_EXPORT T_IoViewTypeFaceObject
{
 	// ���캯��
	CRuntimeClass*				m_pIoViewClass;
	COLORREF					m_aColors[ESCCount];
	
 	T_IoViewTypeFaceObject();
 
	T_IoViewTypeFaceObject(CRuntimeClass* pIoViewClass,const COLORREF (&aColors)[ESCCount],const CFontNode (&aFonts)[ESFCount]);;
 	
	// �������캯��

	T_IoViewTypeFaceObject(const T_IoViewTypeFaceObject & ObjectSrc);	
	
	// ���ز�����
	
	const T_IoViewTypeFaceObject& operator = (const T_IoViewTypeFaceObject & ObjectSrc);

	CFontNode				*GetFontNodes(){ return m_aFonts; }

	CFontNode					m_aFonts[ESFCount];

}T_IoViewTypeFaceObject;

// ��һ�����ڱ���һ����ɫ��������

typedef struct CONTROL_EXPORT T_ChildFrameFaceObject
{
	CWnd *						m_pChildFrame;	
	COLORREF					m_aColors[ESCCount];
	CFontNode					m_aFonts[ESFCount];

	// ���캯��
	T_ChildFrameFaceObject(){}

	T_ChildFrameFaceObject(CWnd * pChildFrame,const COLORREF (&aColors)[ESCCount],const CFontNode (&aFonts)[ESFCount]);

	// �������캯��
	T_ChildFrameFaceObject(T_ChildFrameFaceObject & ObjectSrc);
	
	// ���ز�����
	const T_ChildFrameFaceObject& operator = (const T_ChildFrameFaceObject & ObjectSrc);
}T_ChildFrameFaceObject;

#endif


