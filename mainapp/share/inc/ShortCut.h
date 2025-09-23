#ifndef _SHORT_CUT_H_
#define _SHORT_CUT_H_



// 回调
typedef void (*FN_ShortCutCallBack)();

typedef struct T_ShortCutObject
{
	CString			m_StrKey;					// 快捷键
	FN_ShortCutCallBack	m_pfnShortCutCallBack;	// 对应的回调

	CString			m_StrSummary;				// 注释

	int32			m_iVirtualKeyCode;			// 键盘快捷键 F1 ~ F12

public:
	T_ShortCutObject(const CString &StrKey, int32 iVirtualKeyCode, FN_ShortCutCallBack pfnShortCutCallBack, const CString &StrSummary)
 	{
 		m_StrKey				= StrKey;
		m_iVirtualKeyCode		= iVirtualKeyCode;
 		m_pfnShortCutCallBack	= pfnShortCutCallBack;
		m_StrSummary			= StrSummary;
	};
}T_ShortCutObject;


//-------------------------------------------------------------------
const T_ShortCutObject*		GetShortCutObject(int32 iIndex);
const int32					GetShortCutObjectCount();

const T_ShortCutObject*		FindShortCutObjectByKey(const CString &StrKey);
const T_ShortCutObject*		FindShortCutObjectByVirtualKeyCode(int32 iVirtualKeyCode);



//-------------------------------------------------------------------
// 所有快捷键回调接口 
void				OnShortCut01();
void				OnShortCut02();
void				OnShortCut03();
void				OnShortCut04();
void				OnShortCut1();
void				OnShortCut2();
void				OnShortCut3();
void				OnShortCut4();
void				OnShortCut5();
void				OnShortCut6();
void				OnShortCut7();
void				OnShortCut8();
void				OnShortCut9();
void				OnShortCut05();
void				OnShortCut06();
void				OnShortCut07();
void				OnShortCut08();
void				OnShortCut10();

void				OnShortCut11();
void				OnShortCut12();
void				OnShortCut16();
void				OnShortCut17();

void				OnShortCut36();

void				OnShortCut60();
void				OnShortCut61();
void				OnShortCut62();
void				OnShortCut63();
void				OnShortCut64();
void				OnShortCut65();
void				OnShortCut66();
void				OnShortCut67();
void				OnShortCut68();
void				OnShortCut69();

void				OnShortCut71();
void				OnShortCut72();
void				OnShortCut73();
void				OnShortCut74();

void				OnShortCut80();
void				OnShortCut81();
void				OnShortCut82();
void				OnShortCut83();
void				OnShortCut84();
void				OnShortCut85();
void				OnShortCut86();
void				OnShortCut88();
void				OnShortCut89();
void				OnShortCut811();
void				OnShortCut812();


void				OnShortCut611();
void				OnShortCut612();

#endif