#ifndef _SHARE_FUN_H_
#define _SHARE_FUN_H_

#include "typedef.h"
#include "GmtTime.h"
#include <afxsock.h>
#include <string>

///////////////////////////////////////
// 常用宏
///////////////////////////////////////
#ifndef MAX
#define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#endif

#ifndef MIN
#define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#endif

#ifndef ABS
#define ABS(VAL) (((VAL)>0)?(VAL):(-(VAL)))
#endif

#define DEL(p) { if (p) delete(p); p = NULL; }
#define DEL_ARRAY(p) { if (p) delete[] (p); p = NULL; }
#define DEL_HANDLE(p) { if (p) CloseHandle(p); p = NULL; }

////////////////////////////////////////
// 时间相关
////////////////////////////////////////
#define SECOND_ONE_DAY  86400  // 24 * 60 * 60

AFX_EXT_API int32 GetLocalTimeZone();		// 取得当前时区
AFX_EXT_API time_t gm_mktime(struct tm * m);	// mktime 是把 tm 中的时间当做本地时间来得到格林威治时间的,现在要把 tm 中的时间直接当做格林威治时间来算
AFX_EXT_API bool32 IsSameYear(const CGmtTime &Time1, const CGmtTime &Time2);
AFX_EXT_API bool32 IsSameMonth(const CGmtTime &Time1, const CGmtTime &Time2);
AFX_EXT_API bool32 IsSameDay(const CGmtTime &Time1, const CGmtTime &Time2);
AFX_EXT_API bool32 IsSameHour(const CGmtTime &Time1, const CGmtTime &Time2);
AFX_EXT_API bool32 IsSame5Min(const CGmtTime &Time1, const CGmtTime &Time2);
AFX_EXT_API bool32 IsSameMinute(const CGmtTime &Time1, const CGmtTime &Time2);
AFX_EXT_API bool32 IsSameWeek(const CGmtTime &Time1, const CGmtTime &Time2);
AFX_EXT_API void SaveMinute(CGmtTime &Time);
AFX_EXT_API void SaveDay(CGmtTime &Time);
AFX_EXT_API int32 CompareByDay(UINT uiTime1, UINT uiTime2);
// 与证券系统相关
AFX_EXT_API bool32 MaybeFaultTime(const CMsTime &Time);
AFX_EXT_API bool32 MaybeFaultTime(const CGmtTime &Time);
AFX_EXT_API bool32 MaybeFaultPrice(float fPrice);
// 
AFX_EXT_API CString	Float2SymbolString(float fValue, float fRefer, int32 iDec, bool32 bTranslate = false,bool32 bZeroAsHLine = true, bool32 bAddPercent = false, bool32 bShowNegative=false);	// 组织float字符串, 前面带'+'，'-'，' '符号
AFX_EXT_API CString	Float2String(float fValue, int32 iDec, bool32 bTranslate = false,bool32 bZeroAsHLine = true, bool32 bAddPercent = false);		// 组织float字符串

// 浮点数转化为字串 - 限制长度版本
// 原则：如果浮点数按普通规则转化成字串后，长度超出iMaxLength限制，则根据bTerminate，选择是否截断小数点后面的最低位\次低位...来满足iMaxLength限制
// 返回: true - 成功按照以上规则限制住了StrOut长度, false - 没有限制住- -，StrOut中的数据为取整的字串
AFX_EXT_API bool32 Float2SymbolStringLimitMaxLength(OUT CString &StrOut, float fValue, float fRefer, int32 iDec, int32 iMaxLength = INT_MAX, bool32 bTerminate = false, bool32 bTranslate = false,bool32 bZeroAsHLine = true, bool32 bAddPercent = false, bool32 bShowNegative=false);	// 组织float字符串, 前面带'+'，'-'，' '，限制长度时要注意这个符号的长度根据需要要调整下
AFX_EXT_API bool32 Float2StringLimitMaxLength(OUT CString &StrOut, float fValue, int32 iDec, int32 iMaxLength = INT_MAX, bool32 bTerminate = false, bool32 bTranslate = false,bool32 bZeroAsHLine = true, bool32 bAddPercent = false);		// 组织float字符串，除非为负，否则不返回前缀符号

////////////////////////
AFX_EXT_API void* memcpyex(void *, const void *, size_t);

////////////////////////////////////////
// 判定Shift和Ctrl键是否按下
AFX_EXT_API bool32  IsShiftPressed();
AFX_EXT_API bool32 IsCtrlPressed();
AFX_EXT_API bool32 IsAltPressed();

// COLORREF 和字符串之间的转换， 字符串的顺序是RGB
AFX_EXT_API CString ColorRefToStr(COLORREF clr);		// COLORREF转CString
AFX_EXT_API COLORREF StrToColorRef(const char * szClr);	// 字符串转COLORREF

////////////////////////////////////////
AFX_EXT_API int32 roundf_ ( float f );// 四舍五入
AFX_EXT_API double reversef_(double f, int32 n);//保留一个浮点数的小数位为指定.避免-0.001转换为"-0.00"字符串.
AFX_EXT_API bool32 IsTooSmall ( float f );
AFX_EXT_API bool32 IsTooSmall ( double f );
AFX_EXT_API bool32 IsTooBig ( float f );
AFX_EXT_API bool32 IsTooBig ( double f );
AFX_EXT_API bool32 BeValidFloat(float fValue);		  														
AFX_EXT_API void RectValidate ( CRect& Rect);
////////////////////////////////////////
// 位操作
// iSrc中增加iAdd标志
AFX_EXT_API void AddFlag( INOUT uint32& iSrc,uint32 iAdd );
// iSrc中去除iRemove标志
AFX_EXT_API void RemoveFlag ( INOUT uint32& iSrc,uint32 iRemove );
// 返回iSrc中是否包含iCheck标志
AFX_EXT_API bool32  CheckFlag  ( uint32 iSrc,uint32 iCheck );
// 清除iSrc中的所有标志
AFX_EXT_API void ClearFlag  ( INOUT uint32& iSrc );

////////////////////////////////////////
/*	
	删除当前目录及其下所有文件、子目录
*/
AFX_EXT_API void  DeleteDirectory(IN CString &StrDirName);
////////////////////////////////////////
// 获取MAC地址
AFX_EXT_API bool32 GetMacByGetAdaptersAddr( OUT std::string& strMac);
////////////////////////////////////////

const uint32 KFileLogNull = 0x00;
const uint32 KFileLogTime = 0x01;
const uint32 KFileLogSrc  = 0x02;
AFX_EXT_API void FileLog( CString StrContent, uint32 iFlag, CString StrFile,uint32 iLine);
#define _LogSingleString(Str)	FileLog(Str,KFileLogNull,__FILE__,__LINE__);
#define _LogWithTime(Str)		FileLog(Str,KFileLogTime,__FILE__,__LINE__);
#define _LogWithSrc(Str)		FileLog(Str,KFileLogSrc,__FILE__,__LINE__);
#define _LogFull(Str)			FileLog(Str,KFileLogTime|KFileLogSrc,__FILE__,__LINE__);

AFX_EXT_API void _StartTime(int32 iNum);
AFX_EXT_API double _EndTime(int32 iNum, CString StrMsg, CString StrFile,uint32 iLine );

//#define StartTime(n) _StartTime(n);
//#define EndTime(n,s) _EndTime(n,s,__FILE__,__LINE__);

//#define StartTime(n) ;
//#define EndTime(n,s) ;

AFX_EXT_API void LogTime ( CString StrMsg, bool32 bTimeAsInt = true );
AFX_EXT_API void CreateToolTips( HWND hWnd,TCHAR* tips,RECT* prc);
AFX_EXT_API BOOL _tcheck_if_mkdir(TCHAR *dir_or_file);
AFX_EXT_API void _tremove_dir(TCHAR *client_dirn);
//外面释放return value
AFX_EXT_API char *ANSI_TO_UTF8( const char *szANSI);
//外面释放return value
AFX_EXT_API char *UTF8_TO_ANSI(const char *szUTF8);
// 浮点数比较 返回>0 f1>f2; =0 f1=f2; <0 f1<f2
AFX_EXT_API int32  CompareFloat(float f1, float f2, int32 iSaveDec);
AFX_EXT_API char* _W2A ( CString StrIn, int32 iCodePage= CP_UTF8 );
AFX_EXT_API CString _A2W ( const char* strIn );
AFX_EXT_API void DivideStr(CString str,CStringArray& strArray,const CString&DivideCode);
AFX_EXT_API CString ConvertHZToPY(CString StrIn);

////////////////////////////////////////////////////
// 字符串转换
typedef enum E_MultiCharCoding	// 该编码集请参考"控制面板->区域和语言选项->高级->代码页转换表"中的描述， 以下列出常用的字符集
{
	EMCCSystem		= 0,		// 当前系统设置
	EMCCUtf8		= 65001,	
	EMCCGBK			= 936,
	EMCCBIG5		= 950
}E_MultiCharCoding;

AFX_EXT_API int	Unicode2MultiCharCoding(E_MultiCharCoding eMultiCharCoding, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cchMultiByte);
AFX_EXT_API int MultiCharCoding2Unicode(E_MultiCharCoding eMultiCharCoding, LPCSTR lpMultiByteStr, int cchMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
AFX_EXT_API void SaveXmlFile(const char * KpcFileName, const CString &StrContent);
// eMultiCharCoding - E_MultiCharCoding
// StrSrc - CString
// acDst - char[]
#define UNICODE_2_MULTICHAR(eMultiCharCoding, StrSrc, acDst) \
{\
memset(acDst, 0, sizeof(acDst));\
Unicode2MultiCharCoding(eMultiCharCoding, StrSrc, StrSrc.GetLength(), acDst, sizeof(acDst));\
}\

////////////////////////////////////////////////////
// xml相关
AFX_EXT_API const char *GetXmlRootElement();
AFX_EXT_API const char *GetXmlRootElementAttrApp();
AFX_EXT_API const char *GetXmlRootElementAttrAppValue();
AFX_EXT_API const char *GetXmlRootElementAttrVersion();
AFX_EXT_API const char *GetXmlRootElementAttrData();
AFX_EXT_API const char *GetXmlRootElementAttrCodeType();
AFX_EXT_API const char *GetXmlRootElementAttrReadOnly();


//////////////////////////////////////////////////////////////////////////
// 针对读ini 文件 可能出现CString 无用的问题
AFX_EXT_API DWORD GetPrivateProfileStringGGTong(LPCTSTR lpAppName,LPCTSTR lpKeyName,LPCTSTR lpDefault,CString& lpReturnedString,LPCTSTR lpFileName);
AFX_EXT_API CString GetInfoAddressFilePath();

//
AFX_EXT_API void PlaysWaveSound(CString StrPath);								// 播放wav 格式声音文件

//
#define				_LogCheckTime(StrLog, hWnd) {\
												 CString* pStrMsg = new CString(StrLog);\
												 ::PostMessage(hWnd, 0x460, (WPARAM)timeGetTime(), LPARAM(pStrMsg));\
												}

AFX_EXT_API void MultiColumnMenu(IN CMenu &menu, int iRowPerCol);	// 设置多列菜单

class AFX_EXT_CLASS CLoadLibHelper
{
public:
	
	HMODULE m_hModule;
	void* m_pProg;
	
	CLoadLibHelper(LPCTSTR pName,LPCSTR pProgName)
	{
		m_hModule = LoadLibrary(pName);
		m_pProg = m_hModule ? (void*)GetProcAddress (m_hModule,pProgName) : NULL;
		//ASSERT( NULL != m_pProg );
	}
	
	~CLoadLibHelper()
	{
		if(m_hModule)
		{
			FreeLibrary(m_hModule);
			m_hModule = NULL;
		}
	}
};

template<class TWinApiFunc>
class CLoadLibHelperT
{
public:
	
	HMODULE m_hModule;
	TWinApiFunc m_pProg;
	
	CLoadLibHelperT(LPCTSTR pName,LPCSTR pProgName)
	{
		m_hModule = LoadLibrary(pName);
		m_pProg = m_hModule ? (TWinApiFunc)GetProcAddress (m_hModule,pProgName) : NULL;
		//ASSERT( NULL != m_pProg );
	}
	
	~CLoadLibHelperT()
	{
		if(m_hModule)
		{
			FreeLibrary(m_hModule);
			m_hModule = NULL;
		}
	}
};

//
// Ping.h
//

#pragma pack(1)

#define ICMP_ECHOREPLY	0
#define ICMP_ECHOREQ	8

class AFX_EXT_CLASS CPing
{
public:
	HWND m_hWnd;
	int Ping(UINT nRetries,LPCSTR pstrHost,HWND hWnd, char *pInfo);
	int  WaitForEchoReply(SOCKET s);
	// ICMP Echo Request/Reply functions
	int		SendEchoRequest(SOCKET, LPSOCKADDR_IN);
	DWORD	RecvEchoReply(SOCKET, LPSOCKADDR_IN, u_char *);
	u_short in_cksum(u_short *addr, int len);
	
protected:
	void WSAError(LPCSTR pstrFrom);
	
};

//ping info
typedef struct _pingInfo
{
	DWORD dwTime;
	u_char ttl;
} PINGINFO;

// IP Header -- RFC 791
typedef struct tagIPHDR
{
	u_char  VIHL;			// Version and IHL
	u_char	TOS;			// Type Of Service
	short	TotLen;			// Total Length
	short	ID;				// Identification
	short	FlagOff;		// Flags and Fragment Offset
	u_char	TTL;			// Time To Live
	u_char	Protocol;		// Protocol
	u_short	Checksum;		// Checksum
	struct	in_addr iaSrc;	// Internet Address - Source
	struct	in_addr iaDst;	// Internet Address - Destination
}IPHDR, *PIPHDR;


// ICMP Header - RFC 792
typedef struct tagICMPHDR
{
	u_char	Type;			// Type
	u_char	Code;			// Code
	u_short	Checksum;		// Checksum
	u_short	ID;				// Identification
	u_short	Seq;			// Sequence
	char	Data;			// Data
}ICMPHDR, *PICMPHDR;


#define REQ_DATASIZE 32		// Echo Request Data size

// ICMP Echo Request
typedef struct tagECHOREQUEST
{
	ICMPHDR icmpHdr;
	DWORD	dwTime;
	char	cData[REQ_DATASIZE];
}ECHOREQUEST, *PECHOREQUEST;


// ICMP Echo Reply
typedef struct tagECHOREPLY
{
	IPHDR	ipHdr;
	ECHOREQUEST	echoRequest;
	char    cFiller[256];
}ECHOREPLY, *PECHOREPLY;

AFX_EXT_API int WriteDebugInfo(CString strInfo);	// 用于调试使用的一个函数，因为不能使用DEBUG版本，只能如此

#pragma pack()

#endif // _SHARE_FUN_H_