#ifndef _HOT_KEY_H_
#define _HOT_KEY_H_

#include "typedef.h"

#define VK_KEYCOUNT 0xFF

// 已经定义的功能键
const UINT s_nKeySpiriteWinAgainst[] = 
{
	VK_LBUTTON,        
		VK_RBUTTON,        	
		VK_CANCEL,         
		VK_MBUTTON,       
		VK_BACK,           
		VK_TAB,           
		VK_CLEAR,        
		VK_RETURN,        
		VK_SHIFT,       
		VK_CONTROL,      
		VK_MENU,       
		VK_PAUSE,      
		VK_CAPITAL,       
		VK_KANA,           
		VK_HANGEUL,        
		VK_HANGUL,         
		VK_JUNJA,          
		VK_FINAL,          
		VK_HANJA,          
		VK_KANJI,          
		VK_ESCAPE,         
		VK_CONVERT,        
		VK_NONCONVERT,     
		VK_ACCEPT,         
		VK_MODECHANGE,    
		VK_SPACE,          
		VK_PRIOR,          
		VK_NEXT,           
		VK_END,            
		VK_HOME,           
		VK_LEFT,           
		VK_UP,             
		VK_RIGHT,          
		VK_DOWN,           
		VK_SELECT,         
		VK_PRINT,          
		VK_EXECUTE,        
		VK_SNAPSHOT,       
		VK_INSERT,         
		VK_DELETE,         
		VK_HELP,           
		VK_LWIN,           
		VK_RWIN,          
		VK_APPS,           
		VK_MULTIPLY,       
		VK_ADD,            
		VK_SEPARATOR,      
		VK_SUBTRACT,       
		VK_DECIMAL,        
		VK_DIVIDE,         
		VK_F1,             
		VK_F2,             
		VK_F3,             
		VK_F4,             
		VK_F5,             
		VK_F6,             
		VK_F7,             
		VK_F8,             
		VK_F9,             
		VK_F10,            
		VK_F11,            
		VK_F12,            
		VK_F13,            
		VK_F14,            
		VK_F15,            
		VK_F16,            
		VK_F17,            
		VK_F18,            
		VK_F19,            
		VK_F20,            
		VK_F21,            
		VK_F22,            
		VK_F23,            
		VK_F24,            
		VK_NUMLOCK,        
		VK_SCROLL,         		
		VK_LSHIFT,         
		VK_RSHIFT,         
		VK_LCONTROL,       
		VK_RCONTROL,       
		VK_LMENU,          
		VK_RMENU,          
		VK_ATTN,           
		VK_CRSEL,          
		VK_EXSEL,          
		VK_EREOF,          
		VK_PLAY,           
		VK_ZOOM,           
		VK_NONAME,         
		VK_PA1,            
		VK_OEM_CLEAR,     
		VK_KEYCOUNT,			
};

// 快捷键
typedef enum E_HotKeyType
{
	EHKTMerch = 0,		// 商品快捷键
	EHKTShortCut,		// 快捷键
	EHKTBlock,			// 板块快捷键
	EHKTIndex,			// 指标快捷键

	EHKTLogicBlock,		// 逻辑板块列表
	//
	EHKTCount
}E_HotKeyType;


class AFX_EXT_CLASS CHotKey
{
public:
	CHotKey();
	~CHotKey();

public:
	int32			CompareNoCase(const CHotKey &HotKeyCompare);
	bool32			IsMatch(LPCTSTR lpszInput,bool32 &bidentical);
public:
	// 检索关键字
	CString			m_StrKey;			// 600000	

	// 注释信息
	CString			m_StrSummary;		// 浦发银行

	// 具体的含义
	E_HotKeyType	m_eHotKeyType;		// EHKTMerch
	CString			m_StrParam1;		// "600000"	
	CString			m_StrParam2;		// ""
	int32			m_iParam1;			// 1		
	int32			m_iParam2;			// 0
};


#endif

