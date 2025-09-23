#ifndef _XMLSHARE_H_
#define _XMLSHARE_H_

#include "tinystr.h"
#include "tinyxml.h"
#include "GmtTime.h"
#include <string>
using std::string;

// ֻ�ܱȽ�[0->n]��[С->��]���������м�û����ֵͬ��(�ҵ���ȵ�ʱ����������ظ���ᵼ��small��big���жϳ���)
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


// һЩ��xml��صĺ���

// ��ʼ��һ���յı�׼ggtong xmlͷ���ļ�, ���س�ʼ����ĸ��ڵ�
// �ֱ�ָ���汾���ļ����ʣ��������ʣ��Ƿ�ֻ��
// �粻ָ���汾(NULL����0����), ��ʹ��Ĭ�ϰ汾1.0
// �粻ָ���ļ����ʣ���Ĭ����д ggtong
// �粻ָ���������ʣ����޴��ֶ�
// �粻ָ��ֻ������Ĭ��Ϊ0(�ɶ�д)
//class TiXmlDocument;
//class TiXmlElement;
TiXmlElement		*ConstructGGTongAppXmlDocHeader(INOUT TiXmlDocument &doc, const char *pcszDataName, const char *pcszVersionValue, const char *pcszCodeType, const char *pcszReadOnly);

// ����xml�ĵ�
bool	LoadTiXmlDoc(INOUT TiXmlDocument &doc, LPCTSTR lptszFile, TiXmlEncoding enCode=TIXML_DEFAULT_ENCODING);
bool	SaveTiXmlDoc(const TiXmlDocument &doc, LPCTSTR lptszFile);


// һЩ������Ŀ�ı������ȡ
class CEtcXmlConfig
{
public:
	
	// �κξ�̬�ĺ�����Ҫ�������Ѿ����ػ�ȡ��û������~~
	static CEtcXmlConfig &Instance();

	bool32 Initialize();	// ��ʼ��, ����Ҫ��ʼ����ʱ����ã�Ĭ�Ͻ�����һ��

	// ��ȡ����, sectionΪ�ջ���NULL��ΪĬ��section, key����Ϊ��Чֵ!!!��
	//    pszDefaultΪ�����ڸ�ֵʱ�����Ĭ��ֵ������ΪNULL
	//    �����Ƿ�����ֵ
	bool32 ReadEtcConfig(LPCWSTR pwszSection, LPCWSTR pwszKey, LPCWSTR pwszDefault, OUT CString &StrValue);			// unicode16
	bool32 ReadEtcConfig(const char *pszSection, const char *pszKey, const char *pszDefault,  OUT  string &StrValue);	// utf-8

	// д������, sectionΪ�ջ���NULLʱΪĬ��section��key���ΪNULL���߿�ֵ��������section��ɾ��!!!!
	//    valueΪ�ջ���null����key��ɾ��
	bool32   WriteEtcConfig(LPCWSTR pwszSection, LPCWSTR pwszKey, LPCWSTR pwszValue);				// unicode16
	bool32   WriteEtcConfig(const char *pszSection, const char *pszKey, const char *pszValue);	// utf 8

	static	char *WideToMulti(LPCWSTR pwszSrc, UINT uCodePage =CP_ACP);	// new�����ִ�, ��Ҫdel
	static	wchar_t *MultiToWide(const char *pszSrc, UINT uCodePage = CP_ACP);

protected:
	CEtcXmlConfig();

	CEtcXmlConfig(const CEtcXmlConfig &);
	CEtcXmlConfig &operator=(const CEtcXmlConfig &);

	bool32	ReadXmlContent(const char *pszSection, const char *pszKey, OUT string &StrValue);	// ��ȡxml�е�ֵ
	bool32	ReadXmlContent(TiXmlElement *pRoot, const char *pszSection, const char *pszKey, OUT string &StrValue);	// ��ȡĳ��xml�е�ֵ

	CString	GetPrivateFileName() const;
	CString GetPublicFileName() const;
	
	TiXmlDocument	m_TiDocPrivate;		// ˽�е�xml�ĵ�, д���ֵ���������û�н��и��ĵ�ֵ���ᱻ����
	TiXmlDocument	m_TiDocPublic;		// ���еģ���ȡĳ��ֵʱ�����Դ�˽�еĶ�ȡ��������ܶ�ȡ�������Զ�ȡ������
	TiXmlElement   *m_pRootPrivate;
	TiXmlElement   *m_pRootPublic;
};

// ��ȡָ��TimeDay���ڵı���ʱ�����GMT��ʾ�Ŀ�ʼ����ʱ��
bool32	GetLocalDayGmtRange(IN CGmtTime TimeDay, OUT CGmtTime &TimeLocalStart, OUT CGmtTime &TimeLocalEnd);

#endif