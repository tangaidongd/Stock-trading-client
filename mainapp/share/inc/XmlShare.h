#ifndef _XMLSHARE_H_
#define _XMLSHARE_H_

#include "tinystr.h"
#include "tinyxml.h"
#include "GmtTime.h"
#include <string>
using std::string;

// 只能比较[0->n]是[小->大]的序列且中间没有相同值点(找到相等点时，如果存在重复点会导致small与big点判断出错)
#define MYHALF_SEARCH(aData, iCount, dataToFind, iFindPos, iNearSmall, iNearBig)	\
{\
	iFindPos = -1;\
	iNearSmall = iNearBig = -1;\
	int32 iHalf, iT, iB;\
	iT = iCount-1;\
	iB = 0;\
	iHalf = -1;\
	while ( iB <= iT )\
	{\
		iHalf = (iT+iB)/2;\
		if ( aData[iHalf] < dataToFind )\
		{\
			iB = iHalf+1;\
		}\
		else if ( dataToFind < aData[iHalf] )\
		{\
			iT = iHalf-1;\
		}\
		else\
		{\
			iFindPos = iHalf;\
			break;\
		}\
	}\
	if ( iHalf >= 0 )\
	{\
		if ( -1 == iFindPos )\
		{\
			iNearSmall = aData[iHalf] < dataToFind ? iHalf : iHalf-1;\
			iNearBig = iNearSmall+1;\
		}\
		else\
		{\
			iNearSmall = iHalf-1;\
			iNearBig = iHalf+1;\
		}\
		iNearBig = iNearBig >= iCount ? -1 : iNearBig;\
	}\
};

#define MYHALF_SEARCH_MEMBER(aData, iCount, dataToFind, iFindPos, iNearSmall, iNearBig, member)\
{\
	iFindPos = -1;\
	iNearSmall = iNearBig = -1;\
	int32 iHalf, iT, iB;\
	iT = iCount-1;\
	iB = 0;\
	iHalf = -1;\
	while ( iB <= iT )\
	{\
		iHalf = (iT+iB)/2;\
		if ( aData[iHalf].##member < dataToFind )\
		{\
			iB = iHalf+1;\
		}\
		else if ( dataToFind < aData[iHalf].##member )\
		{\
			iT = iHalf-1;\
		}\
		else\
		{\
			iFindPos = iHalf;\
			break;\
		}\
	}\
	if ( iHalf >= 0 )\
	{\
		if ( -1 == iFindPos )\
		{\
			iNearSmall = aData[iHalf].##member < dataToFind ? iHalf : iHalf-1;\
			iNearBig = iNearSmall+1;\
		}\
		else\
		{\
			iNearSmall = iHalf-1;\
			iNearBig = iHalf+1;\
		}\
		iNearBig = iNearBig >= iCount ? -1 : iNearBig;\
	}\
};


// 一些与xml相关的函数

// 初始化一个空的标准ggtong xml头部文件, 返回初始化后的根节点
// 分别指出版本，文件性质，加密性质，是否只读
// 如不指出版本(NULL或者0长度), 则使用默认版本1.0
// 如不指出文件性质，则默认填写 ggtong
// 如不指出加密性质，则无此字段
// 如不指出只读，则默认为0(可读写)
//class TiXmlDocument;
//class TiXmlElement;
TiXmlElement		*ConstructGGTongAppXmlDocHeader(INOUT TiXmlDocument &doc, const char *pcszDataName, const char *pcszVersionValue, const char *pcszCodeType, const char *pcszReadOnly);

// 加载xml文档
bool	LoadTiXmlDoc(INOUT TiXmlDocument &doc, LPCTSTR lptszFile, TiXmlEncoding enCode=TIXML_DEFAULT_ENCODING);
bool	SaveTiXmlDoc(const TiXmlDocument &doc, LPCTSTR lptszFile);


// 一些公共项目的保存与读取
class CEtcXmlConfig
{
public:
	
	// 任何静态的函数不要依赖它已经加载获取还没有析构~~
	static CEtcXmlConfig &Instance();

	bool32 Initialize();	// 初始化, 在需要初始化的时候调用，默认仅调用一次

	// 读取配置, section为空或者NULL则为默认section, key必须为有效值!!!，
	//    pszDefault为不存在该值时赋予的默认值，可以为NULL
	//    返回是否赋予了值
	bool32 ReadEtcConfig(LPCWSTR pwszSection, LPCWSTR pwszKey, LPCWSTR pwszDefault, OUT CString &StrValue);			// unicode16
	bool32 ReadEtcConfig(const char *pszSection, const char *pszKey, const char *pszDefault,  OUT  string &StrValue);	// utf-8

	// 写入配置, section为空或者NULL时为默认section，key如果为NULL或者空值，则整个section被删除!!!!
	//    value为空或者null，则key被删除
	bool32   WriteEtcConfig(LPCWSTR pwszSection, LPCWSTR pwszKey, LPCWSTR pwszValue);				// unicode16
	bool32   WriteEtcConfig(const char *pszSection, const char *pszKey, const char *pszValue);	// utf 8

	static	char *WideToMulti(LPCWSTR pwszSrc, UINT uCodePage =CP_ACP);	// new出来字串, 需要del
	static	wchar_t *MultiToWide(const char *pszSrc, UINT uCodePage = CP_ACP);

protected:
	CEtcXmlConfig();

	CEtcXmlConfig(const CEtcXmlConfig &);
	CEtcXmlConfig &operator=(const CEtcXmlConfig &);

	bool32	ReadXmlContent(const char *pszSection, const char *pszKey, OUT string &StrValue);	// 读取xml中的值
	bool32	ReadXmlContent(TiXmlElement *pRoot, const char *pszSection, const char *pszKey, OUT string &StrValue);	// 读取某个xml中的值

	CString	GetPrivateFileName() const;
	CString GetPublicFileName() const;
	
	TiXmlDocument	m_TiDocPrivate;		// 私有的xml文档, 写入的值保存在这里，没有进行更改的值不会被保存
	TiXmlDocument	m_TiDocPublic;		// 共有的，读取某个值时，尝试从私有的读取，如果不能读取到，则尝试读取公共的
	TiXmlElement   *m_pRootPrivate;
	TiXmlElement   *m_pRootPublic;
};

// 获取指定TimeDay所在的本地时间天的GMT表示的开始结束时间
bool32	GetLocalDayGmtRange(IN CGmtTime TimeDay, OUT CGmtTime &TimeLocalStart, OUT CGmtTime &TimeLocalEnd);

#endif