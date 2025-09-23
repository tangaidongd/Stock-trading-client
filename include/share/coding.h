
#ifndef __CHARACTER_CODING_H__
#define __CHARACTER_CODING_H__

/* This document is captured from http://www.gnu.org/software/libiconv/documentation/libiconv/iconv_open.3.html
The values permitted for fromcode and tocode and the supported combinations are system dependent.
For the libiconv library, the following encodings are supported, in all combinations.

European languages
ASCII, ISO-8859-{1,2,3,4,5,7,9,10,13,14,15,16}, KOI8-R, KOI8-U, KOI8-RU,
CP{1250,1251,1252,1253,1254,1257}, CP{850,866}, Mac{Roman,CentralEurope,Iceland,Croatian,Romania},
Mac{Cyrillic,Ukraine,Greek,Turkish}, Macintosh

Semitic languages
ISO-8859-{6,8}, CP{1255,1256}, CP862, Mac{Hebrew,Arabic}

Japanese
EUC-JP, SHIFT_JIS, CP932, ISO-2022-JP, ISO-2022-JP-2, ISO-2022-JP-1

Chinese
EUC-CN, HZ, GBK, CP936, GB18030, EUC-TW, BIG5, CP950, BIG5-HKSCS, BIG5-HKSCS:2001, BIG5-HKSCS:1999, ISO-2022-CN, ISO-2022-CN-EXT

Korean
EUC-KR, CP949, ISO-2022-KR, JOHAB

Armenian
ARMSCII-8

Georgian
Georgian-Academy, Georgian-PS

Tajik
KOI8-T
Kazakh
PT154, RK1048

Thai
TIS-620, CP874, MacThai

Laotian
MuleLao-1, CP1133

Vietnamese
VISCII, TCVN, CP1258

Platform specifics
HP-ROMAN8, NEXTSTEP

Full Unicode
UTF-8
UCS-2, UCS-2BE, UCS-2LE
UCS-4, UCS-4BE, UCS-4LE
UTF-16, UTF-16BE, UTF-16LE
UTF-32, UTF-32BE, UTF-32LE
UTF-7
C99, JAVA

Full Unicode, in terms of uint16_t or uint32_t
(with machine dependent endianness and alignment)
UCS-2-INTERNAL, UCS-4-INTERNAL

Locale dependent, in terms of char or wchar_t
(with machine dependent endianness and alignment, and with semantics depending on the OS and the current LC_CTYPE locale facet)
char, wchar_t

When configured with the option --enable-extra-encodings, it also provides support for a few extra encodings:
European languages
CP{437,737,775,852,853,855,857,858,860,861,863,865,869,1125}

Semitic languages
CP864
Japanese
EUC-JISX0213, Shift_JISX0213, ISO-2022-JP-3
Chinese
BIG5-2003 (experimental)

Turkmen
TDS565

Platform specifics
ATARIST, RISCOS-LATIN1
*/

/*
	Code Value (Codepage) Alphabet 
	DIN_66003 20106 IA5 (German) 
	NS_4551-1 20108 IA5 (Norwegian) 
	SEN_850200_B 20107 IA5 (Swedish) 
	_autodetect 50932 Japanese (Auto Select) 
	_autodetect_kr 50949 Korean (Auto Select) 
	big5 950 Chinese Traditional (Big5) 
	csISO2022JP 50221 Japanese (JIS-Allow 1 byte Kana) 
	euc-kr 51949 Korean (EUC) 
	gb2312 936 Chinese Simplified (GB2312) 
	hz-gb-2312 52936 Chinese Simplified (HZ) 
	ibm852 852 Central European (DOS) 
	ibm866 866 Cyrillic Alphabet (DOS) 
	irv 20105 IA5 (IRV) 
	iso-2022-jp 50220 Japanese (JIS) 
	iso-2022-jp 50222 Japanese (JIS-Allow 1 byte Kana) 
	iso-2022-kr 50225 Korean (ISO) 
	iso-8859-1 1252 Western Alphabet 
	iso-8859-1 28591 Western Alphabet (ISO) 
	iso-8859-2 28592 Central European Alphabet (ISO) 
	iso-8859-3 28593 Latin 3 Alphabet (ISO) 
	iso-8859-4 28594 Baltic Alphabet (ISO) 
	iso-8859-5 28595 Cyrillic Alphabet (ISO) 
	iso-8859-6 28596 Arabic Alphabet (ISO) 
	iso-8859-7 28597 Greek Alphabet (ISO) 
	iso-8859-8 28598 Hebrew Alphabet (ISO) 
	koi8-r 20866 Cyrillic Alphabet (KOI8-R) 
	ks_c_5601 949 Korean 
	shift-jis 932 Japanese (Shift-JIS) 
	unicode 1200 Universal Alphabet 
	unicodeFEFF 1201 Universal Alphabet (Big-Endian) 
	utf-7 65000 Universal Alphabet (UTF-7) 
	utf-8 65001 Universal Alphabet (UTF-8) 
	windows-1250 1250 Central European Alphabet (Windows) 
	windows-1251 1251 Cyrillic Alphabet (Windows) 
	windows-1252 1252 Western Alphabet (Windows) 
	windows-1253 1253 Greek Alphabet (Windows) 
	windows-1254 1254 Turkish Alphabet 
	windows-1255 1255 Hebrew Alphabet (Windows) 
	windows-1256 1256 Arabic Alphabet (Windows) 
	windows-1257 1257 Baltic Alphabet (Windows) 
	windows-1258 1258 Vietnamese Alphabet (Windows) 
	windows-874 874 Thai (Windows) 
	x-euc 51932 Japanese (EUC) 
	x-user-defined 50000 User Defined
	*/

#ifdef _WIN32
#include <windows.h>
#endif

#include <assert.h>
#include <time.h>
#include <string>

using std::string;
using std::wstring;

static const int UTF8 = 65001;
static const int GBK = 936;
static const int BIG5 = 950;

AFX_EXT_API bool Unicode2MultiChar(int eMultiCharCoding,LPCWSTR lpWideCharStr, string &strOut);
AFX_EXT_API bool MultiChar2Unicode(int eMultiCharCoding,LPCSTR lpMultiByteStr, wstring &strOut);
AFX_EXT_API bool Gbk32ToUnicode(LPCSTR lpMultiByteStr, wstring &strOut);
AFX_EXT_API bool UnicodeToGbk32(LPCWSTR lpWideCharStr, string &strOut);
AFX_EXT_API bool Utf8ToUnicode(LPCSTR lpMultiByteStr, wstring &strOut);
AFX_EXT_API bool UnicodeToUtf8(LPCWSTR lpWideCharStr, string &strOut);
AFX_EXT_API bool Gbk32ToUtf8(LPCSTR lpMultiByteStr, string &strOut);
AFX_EXT_API bool Utf8ToGbk32(LPCSTR lpMultiByteStr, string &strOut);

AFX_EXT_API wstring _Utf8ToUnicode(LPCSTR lpMultiByteStr);
AFX_EXT_API string _UnicodeToUtf8(LPCWSTR lpWideCharStr);
AFX_EXT_API string _Unicode2MultiChar(LPCWSTR lpWideCharStr, int eMultiCharCoding = CP_ACP);
AFX_EXT_API wstring _MultiChar2Unicode(LPCSTR lpMultiByteStr, int eMultiCharCoding = CP_ACP);

#ifdef _POSIX_C_SOURCE
#define UTF8	"utf-8"
#define GBK		"gbk"
#define BIG5	"big5"
#define UTF16LE	"utf_16_le"
AFX_EXT_API int code_convert(char *from_charset, char *to_charset, char *inbuf, int inlen, char *outbuf, int outlen);
#endif


#endif
