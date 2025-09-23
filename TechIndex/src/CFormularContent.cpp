// CFormularContent.cpp : implementation file
//

#include "stdafx.h"
#include <afxtempl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <vector>
#include <string>
#include "CFormularComputeParent.h"
#include "ShareFun.h"
#include "PathFactory.h"
#include "CCodeFile.h"
#include "coding.h"
#include "hotkey.h"
#include "EngineCenterBase.h"
#include "CFormularContent.h"
#include "PluginFuncRight.h"
//#include "IoViewShare.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CAbsCenterManager *g_pCenterManager;
//extern CString g_strUserName;
CString	g_strCheckCode = _T("");
////////////////////////////////////////////////////////////////////////////
// 这些指标不让用户看到
const CString g_KaStrIndexNotShow[] = 
{
	L"SYSTEM",
};

const int32 KiNumsIndexNotShow = sizeof(g_KaStrIndexNotShow) / sizeof(CString);

// 这些指标特殊显示
const CString g_KaStrIndexSpecialShow[] =
{
	L"MA",
	L"VOL",
};

const int32 KiNumsIndexSpecialShow = sizeof(g_KaStrIndexSpecialShow) / sizeof(CString);

/////////////////////////////////////////////////////////////////////////////
// CFormularContent
uint32 CFormularContent::KIntervalMask		= 0x000FFFFF;


const char* KStrFormulaGroup1Type		= "type";
const char* KStrFormulaIndexXday		= "xday";
const char* KStrFormulaIndexOften		= "often";
const char* KStrFormulaIndexProtected	= "protected";
const char* KStrFormulaIndexStockType   = "indexStockType";
const char* KStrFormulaExtraY			= "extraY";
const CString KStrUserGroupCdtChose		= L"自编条件选股";
const CString KStrUserGroupTradeChose	= L"自编交易选股";
const CString KStrUserGroupClrKLine		= L"自编五彩K 线";
const CString KStrUserGroup2			= L"自编";

const char* KStrFormulaGroup1			= "group1";
const char* KStrFormulaGroup1Name		= "name";
const char* KStrFormulaGroup2			= "group2";
const char* KStrFormulaGroup2Name		= "name";
const char* KStrFormulaIndex			= "index";
const char* KStrFormulaIndexId			= "id";
const char* KStrFormulaIndexStartTime	= "StartTime";
const char* KStrFormulaIndexEffectiveTime = "EffectiveTime";
const char* KStrFormulaIndexCheckCode	= "checkcode";
const char* KStrFormulaIndexDesc		= "desc";
const char* KStrFormulaIndexPassword	= "password";
const char* KStrFormulaIndexHotkey		= "hotkey";
const char* KStrFormulaIndexFlag		= "flag";
const char* KStrFormulaIndexLastTime	= "time";//new
const char* KStrFormulaIndexContent		= "content";
const char* KStrFormulaIndexHelp		= "help";
const char* KStrFormulaParam			= "param";
const char* KStrFormulaParamName		= "name";
const char* KStrFormulaParamMin			= "min";
const char* KStrFormulaParamMax			= "max";
const char* KStrFormulaParamDefault		= "default";
const char* KStrFormulaParamStep		= "step";
const char* KStrFormulaParamDesc		= "desc";//new
const char* KStrFormulaLine				= "line";
const char* KStrFormulaLineY			= "y";
const CString KStrTradeEnterLong		= L"ENTERLONG";
const CString KStrTradeExitLong			= L"EXITLONG";
const CString KStrTradeEnterShort		= L"ENTERSHORT";
const CString KStrTradeExitShort		= L"EXITSHORT";
const CString KStrTradeDefault = KStrTradeEnterLong + L": ;\n" + KStrTradeExitLong + L": ;\n" + KStrTradeEnterShort + L": ;\n" + KStrTradeExitShort + L": ;";

//lint --e{438,429}
bool32 DeCodeXml(const char* strFile, TiXmlDocument*& pXmlDocument)
{
	// 解密xml 文档
	if ( NULL == strFile || NULL == pXmlDocument )
	{
		return false;
	}

	TiXmlElement* pRoot  = pXmlDocument->RootElement();
	if (NULL == pRoot)
	{
		return false;
	}
	//	
	const char* pStrCodeType = pRoot->Attribute(GetXmlRootElementAttrCodeType());
	
	// 没有加密的,先加密一下
	if ( NULL == pStrCodeType )
	{
		// 
		pRoot->SetAttribute(GetXmlRootElementAttrCodeType(), 0);
		pStrCodeType = pRoot->Attribute(GetXmlRootElementAttrCodeType());
		
		char* pStrContent = CCodeFile::EnCodeXmlNode(pRoot, (CCodeFile::E_CodeFileType)atoi(pStrCodeType));
		
		if ( NULL != pStrContent )
		{
			pRoot->Clear();
			
			TiXmlText* pNewText = new TiXmlText(pStrContent);
			pRoot->LinkEndChild(pNewText);
			
			pXmlDocument->SaveFile();		
			DEL(pStrContent);		
			
			// 重新装载一次
			DEL(pXmlDocument);	
			pXmlDocument = new TiXmlDocument(strFile);
			ASSERT(NULL != pXmlDocument);
			pXmlDocument->LoadFile();
			
			//			
			pRoot  = pXmlDocument->RootElement();			
			if ( NULL != pRoot)
			{
				pStrCodeType = pRoot->Attribute(GetXmlRootElementAttrCodeType());
				ASSERT(NULL != pStrCodeType);
			}
			
			
		}
	}
	
	//
	if ( NULL != pStrCodeType && 0 == atoi(pStrCodeType) )
	{
		// 需要解密:	
		if (NULL == pRoot)
		{
			return false;
		}

		const char* pStrXml = pRoot->GetText();
		
		if ( NULL != pStrXml )
		{
			int32 iLenXml = strlen(pStrXml);
			
			// 得到根节点的数据描述
			const char* pStrVersion = pRoot->Attribute(GetXmlRootElementAttrVersion());
			
			const char* pStrApp		= pRoot->Attribute(GetXmlRootElementAttrApp());
			
			const char* pStrData    = pRoot->Attribute(GetXmlRootElementAttrData());
			
			// 解码以后得到明文, 数据长度会变小, 所以用 iLenXml 足够了		
			char* pStrDeCodeUTF8 = new char[iLenXml];			
			ASSERT(NULL!= pStrDeCodeUTF8);
			memset(pStrDeCodeUTF8, 0, iLenXml);
			
			CCodeFile::E_CodeFileType eCodeType = (CCodeFile::E_CodeFileType)atoi(pStrCodeType);
		

			bool32 bOK = CCodeFile::DeCodeString((char*)pStrXml, iLenXml, pStrDeCodeUTF8, &iLenXml, eCodeType);

			// 解出来的是utf8 转一下			
			int32 iLenOrginalUTF8    = 0;
			int32 iLenOrginalUnicode = 0;
			if(NULL != pStrDeCodeUTF8 && strlen(pStrDeCodeUTF8) != 0)
			{
				iLenOrginalUTF8    = strlen(pStrDeCodeUTF8);
				iLenOrginalUnicode = iLenOrginalUTF8 * 2 + 100;
			}
			else
			{
				DEL_ARRAY(pStrDeCodeUTF8);
				return false;
			}
			TCHAR* pStrDeCodeUnicode = new TCHAR[iLenOrginalUnicode];
			memset(pStrDeCodeUnicode, 0, iLenOrginalUnicode * sizeof(TCHAR));
			
			MultiCharCoding2Unicode(EMCCUtf8, pStrDeCodeUTF8, iLenOrginalUTF8, pStrDeCodeUnicode, iLenOrginalUnicode*sizeof(TCHAR));
			

			//
			if ( !bOK )
			{
				////ASSERT(0);
				DEL_ARRAY(pStrDeCodeUTF8);
				DEL_ARRAY(pStrDeCodeUnicode);
				return false;
			}
			
			// 拼成一个完整的 xml 文档,用于解析
			CString StrTotalUnicode;			
			StrTotalUnicode.Format(L"<?xml version='1.0' encoding='utf-8' ?>\n<XMLDATA %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\">\n%s\n</XMLDATA>",
				CString(GetXmlRootElementAttrVersion()).GetBuffer(), CString(pStrVersion).GetBuffer(),
				CString(GetXmlRootElementAttrApp()).GetBuffer(), CString(pStrApp).GetBuffer(),
				CString(GetXmlRootElementAttrData()).GetBuffer(), CString(pStrData).GetBuffer(),
				CString(GetXmlRootElementAttrCodeType()).GetBuffer(), CString(pStrCodeType).GetBuffer(),
				pStrDeCodeUnicode
				);
			
			int32 iLenTotalUnicode = StrTotalUnicode.GetLength();
			int32 iLenTotalUtf8	   = (iLenTotalUnicode * 2 + 100);
			
			char* pStrTotalUtf8 = new char[iLenTotalUtf8];
			memset(pStrTotalUtf8, 0, iLenTotalUtf8);
			
			Unicode2MultiCharCoding(EMCCUtf8, StrTotalUnicode, StrTotalUnicode.GetLength(), pStrTotalUtf8, iLenTotalUtf8);
			
			// 把以前的删掉
			DEL(pXmlDocument);	
			pXmlDocument = new TiXmlDocument(strFile);
			ASSERT(NULL != pXmlDocument);
	 		
			// 解析xml
			pXmlDocument->Parse(pStrTotalUtf8);

			// 清理内存
			DEL_ARRAY(pStrDeCodeUTF8);			
			DEL_ARRAY(pStrDeCodeUnicode);
			DEL_ARRAY(pStrTotalUtf8);
		}		
	}

	return true;
}

bool32 EnCodeXml(TiXmlDocument* pXmlDocument, TiXmlElement* pRootElementRAM)
{	
	if ( NULL == pXmlDocument || NULL == pRootElementRAM )
	{
		////ASSERT(0);
		return false;
	}
	
	// 加密xml 文档		
	const char* pStrCodeType = pRootElementRAM->Attribute(GetXmlRootElementAttrCodeType());
	
	if ( NULL == pStrCodeType )
	{
		// 如果没有 加密 字段, 加上base64
		pRootElementRAM->SetAttribute(GetXmlRootElementAttrCodeType(), 0);
		pStrCodeType = pRootElementRAM->Attribute(GetXmlRootElementAttrCodeType());
	}
	
	char* pStrContent = CCodeFile::EnCodeXmlNode(pRootElementRAM, (CCodeFile::E_CodeFileType)atoi(pStrCodeType));
	ASSERT(NULL != pStrContent);

// 	CString StrRootElement;
// 	StrRootElement  = L"<?xml version =\"1.0\" encoding=\"UTF-8\"?> \n";
// 	StrRootElement += L"<XMLDATA version=\"1.0\" app = \"ggtong\" data = \"index\" codetype= \"0\"> \n";
	// 得到根节点的数据描述
	const char* pStrVersion = pRootElementRAM->Attribute(GetXmlRootElementAttrVersion());
	const char* pStrApp		= pRootElementRAM->Attribute(GetXmlRootElementAttrApp());
	const char* pStrData    = pRootElementRAM->Attribute(GetXmlRootElementAttrData());
	CString StrRootElement;			
	StrRootElement.Format(L"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<XMLDATA %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\">\n",
		CString(GetXmlRootElementAttrVersion()).GetBuffer(), CString(pStrVersion).GetBuffer(),
		CString(GetXmlRootElementAttrApp()).GetBuffer(), CString(pStrApp).GetBuffer(),
		CString(GetXmlRootElementAttrData()).GetBuffer(), CString(pStrData).GetBuffer(),
		CString(GetXmlRootElementAttrCodeType()).GetBuffer(), CString(pStrCodeType).GetBuffer()
		);	// 保持原来的配置不变化

	int32 iOutBufferSize = StrRootElement.GetLength();
	iOutBufferSize *= 2;
	iOutBufferSize += 100;		// 防止StrContent长度为0
	char *pcOutBuffer = new char[iOutBufferSize];
	memset(pcOutBuffer, 0, iOutBufferSize * sizeof(char));

	Unicode2MultiCharCoding(EMCCUtf8, StrRootElement, StrRootElement.GetLength(), pcOutBuffer, iOutBufferSize);

	CString StrEnd;
	StrEnd = L"</XMLDATA>";
	
	int32 iOutBufferSize2 = StrEnd.GetLength();
	iOutBufferSize2 *= 2;
	iOutBufferSize2 += 10;		// 防止StrContent长度为0
	char *pcOutBuffer2 = new char[iOutBufferSize2];
	memset(pcOutBuffer2, 0, iOutBufferSize2 * sizeof(char));

	Unicode2MultiCharCoding(EMCCUtf8, StrEnd, StrEnd.GetLength(), pcOutBuffer2, iOutBufferSize2);

	//
	CFile File;
	if (File.Open(CString(pXmlDocument->Value()), CFile::modeCreate | CFile::modeWrite))
	{
		if(NULL != pcOutBuffer)
		{
			File.Write(pcOutBuffer, strlen(pcOutBuffer));
		}
		if(NULL != pStrContent)
		{
			File.Write(pStrContent, strlen(pStrContent));
		}	
		if(NULL != pcOutBuffer2)
		{
			File.Write(pcOutBuffer2, strlen(pcOutBuffer2));
		}		
		File.Close();
	}

	DEL_ARRAY(pStrContent);
	DEL_ARRAY(pcOutBuffer);
	DEL_ARRAY(pcOutBuffer2);

	return true;
}

static int32 CStringToInt(TCHAR* pStrNum)
{
	if ( NULL == pStrNum )
	{
		return -1;
	}
	
	for ( int i = 0; i < (int)wcslen(pStrNum); i++ )
	{
		if ( !iswdigit(pStrNum[i]) )
		{
			return -1;
		}
	}
	
	
	std::string StrNumTmpA;
	Unicode2MultiChar(CP_ACP, pStrNum, StrNumTmpA);	


	int iReval = atoi(StrNumTmpA.c_str());	
	
	return iReval;
}

static int32 CStringToInt(const CString& StrNum)
{
	CString StrNumTmp = StrNum;
	
	TCHAR* pStrNum = StrNumTmp.LockBuffer();
	
	int32 iReval = CStringToInt(pStrNum);
	
	StrNumTmp.UnlockBuffer();
	
	return iReval;
}

void CFormularContent::AddOften()
{
	m_bOften = true;
		
	CString StrName = name;
	CIndexContentXml* pIndexContentXml = NULL;

	// 用户自编
	T_IndexMapPtr* p = CFormulaLib::instance()->m_UserIndex.Get(StrName);
	
	if ( NULL != p )
	{
		CFormulaLib::instance()->m_UserIndex.Modify(this);

		pIndexContentXml = &CFormulaLib::instance()->m_UserIndex;
	}
	else
	{
		//用户之前修改过的系统指标
		p = CFormulaLib::instance()->m_ModifyIndex.Get(StrName);
		
		if ( NULL != p )
		{
			CFormulaLib::instance()->m_ModifyIndex.Modify(this);				
		}
		else
		{
			//还未修改过的系统指标
			CString StrGroup1, StrGroup2;
			
			if ( CFormulaLib::instance()->m_SysIndex.GetGroup(this, StrGroup1, StrGroup2) )
			{
				CFormulaLib::instance()->m_ModifyIndex.Add(StrGroup1, StrGroup2, this);
			}
		}

		pIndexContentXml = &CFormulaLib::instance()->m_ModifyIndex;
		p = CFormulaLib::instance()->m_ModifyIndex.Get(StrName);
	}

	//	
	if ( NULL != p )
	{
		TiXmlElement* pElement = (TiXmlElement*)p->p2;
		
		if ( NULL != pElement )
		{
			_ToXml(pElement);
		}
		
		//	
		if ( NULL != pIndexContentXml )
		{
			pIndexContentXml->Save();	
		}
	}		
}

void CFormularContent::ReMoveOften()
{
	m_bOften = false;
	
	CString StrName = name;
	CIndexContentXml* pIndexContentXml = NULL;
	
	// 用户自编
	T_IndexMapPtr* p = CFormulaLib::instance()->m_UserIndex.Get(StrName);
	
	if ( NULL != p )
	{
		CFormulaLib::instance()->m_UserIndex.Modify(this);
		pIndexContentXml = &CFormulaLib::instance()->m_UserIndex;
	}
	else
	{
		//用户之前修改过的系统指标
		p = CFormulaLib::instance()->m_ModifyIndex.Get(StrName);
		
		if ( NULL != p )
		{
			CFormulaLib::instance()->m_ModifyIndex.Modify(this);				
		}
		else
		{
			//还未修改过的系统指标
			CString StrGroup1, StrGroup2;
			
			if ( CFormulaLib::instance()->m_SysIndex.GetGroup(this, StrGroup1, StrGroup2) )
			{
				CFormulaLib::instance()->m_ModifyIndex.Add(StrGroup1, StrGroup2, this);
			}
		}
		
		pIndexContentXml = &CFormulaLib::instance()->m_ModifyIndex;
		p = CFormulaLib::instance()->m_ModifyIndex.Get(StrName);
	}
	
	//
	if ( NULL!= p )
	{
		TiXmlElement* pElement = (TiXmlElement*)p->p2;
		if ( NULL != pElement )
		{
			_ToXml(pElement);
		}
	}
	
	//	
	if ( NULL != pIndexContentXml )
	{
		pIndexContentXml->Save();	
	}
}

//lint --e{423,429}
void CFormularContent::_ToXml(TiXmlElement * pNode)
{
	pNode->SetAttribute(KStrFormulaIndexId,_W2A(name));
	pNode->SetAttribute(KStrFormulaIndexDesc,_W2A(explainBrief));
//	pNode->SetAttribute(KStrFormulaIndexKind,_W2A(name));
	pNode->SetAttribute(KStrFormulaIndexPassword,_W2A(password));

	pNode->SetAttribute(KStrFormulaIndexHotkey,_W2A(hotkey));

	int i;
	char strbuf[256];
	TiXmlElement* pChild = NULL;

	sprintf ( strbuf,"%u",flag);
	pNode->SetAttribute(KStrFormulaIndexFlag,strbuf);

	sprintf ( strbuf,"%d",last_time);
	pNode->SetAttribute(KStrFormulaIndexLastTime,strbuf);

	sprintf(strbuf, "%d", m_bOften);
	pNode->SetAttribute(KStrFormulaIndexOften, strbuf);

	sprintf(strbuf, "%d", m_bProtected);
	pNode->SetAttribute(KStrFormulaIndexProtected, strbuf);

	sprintf(strbuf, "%d", m_byteIndexStockType);					// 指标对应的对象类型
	pNode->SetAttribute(KStrFormulaIndexStockType, strbuf);

	string strFormularUtf8;
	if ( UnicodeToUtf8(formular, strFormularUtf8) )
	{
		pNode->SetAttribute(KStrFormulaIndexContent, strFormularUtf8.c_str());
	}
	else
	{
		pNode->SetAttribute(KStrFormulaIndexContent,_W2A(formular));
	}
	pNode->SetAttribute(KStrFormulaIndexHelp,_W2A(help));

	while ( pNode->FirstChild())
	{
		pNode->RemoveChild(pNode->FirstChild());
	}

	for ( i = 0; i < numPara; i ++ )
	{
		pChild = new TiXmlElement(KStrFormulaParam);
		pChild->SetAttribute(KStrFormulaParamName,_W2A(namePara[i]));
		pChild->SetAttribute(KStrFormulaParamDesc,_W2A(descPara[i]));
		sprintf ( strbuf, "%f", min[i]);
		pChild->SetAttribute(KStrFormulaParamMin,strbuf);
		sprintf ( strbuf, "%f", max[i]);
		pChild->SetAttribute(KStrFormulaParamMax,strbuf);
		sprintf ( strbuf, "%f", defaultVal[i]);
		pChild->SetAttribute(KStrFormulaParamDefault,strbuf);
		sprintf ( strbuf, "%f", stepLen[i]);
		pChild->SetAttribute(KStrFormulaParamStep,strbuf);
		pNode->LinkEndChild(pChild);
	}
	for ( i = 0; i < numLine; i ++ )
	{
		pChild = new TiXmlElement(KStrFormulaLine);
		sprintf ( strbuf, "%f", line[i]);
		pChild->SetAttribute(KStrFormulaLineY,strbuf);
		pNode->LinkEndChild(pChild);
	}
	for ( i = 0; i < numExtraY; i++ )
	{
		pChild = new TiXmlElement(KStrFormulaExtraY);
		sprintf(strbuf, "%f", lineExtraY[i]);
		pChild->SetAttribute(KStrFormulaLineY, strbuf);
		pNode->LinkEndChild(pChild);
	}
}
void CFormularContent::_FromXml ( TiXmlElement * pNode )
{
	const char* strValue = NULL;
	if ( 0 == strcmp(pNode->Value(),KStrFormulaIndex))
	{
		strValue = pNode->Attribute(KStrFormulaIndexId);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			name = _A2W(strValue);
		}
		strValue = pNode->Attribute(KStrFormulaIndexDesc);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			explainBrief = _A2W(strValue);
		}

//		strValue = pNode->Attribute(KStrFormulaIndexKind);
		strValue = pNode->Attribute(KStrFormulaIndexPassword);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			password = _A2W(strValue);
		}

		strValue = pNode->Attribute(KStrFormulaIndexXday);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			CString StrTime = _A2W(strValue);
			if (8 != StrTime.GetLength())
			{
				lXday = -1;
			}
			else
			{
				CString StrYear = StrTime.Mid(0,4);
				CString StrMon  = StrTime.Mid(4,2);
				CString StrDay  = StrTime.Mid(6,2);

				int32 iYear = CStringToInt(StrYear);
				int32 iMon  = CStringToInt(StrMon);
				int32 iDay  = CStringToInt(StrDay);

				CGmtTime tt(iYear, iMon, iDay, 0, 0, 0);
				lXday = tt.GetTime();
			}			
		}
		else
		{
			lXday = -1;
		}

		strValue = pNode->Attribute(KStrFormulaIndexHotkey);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			hotkey = _A2W(strValue);
		}

		strValue = pNode->Attribute(KStrFormulaIndexFlag);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			flag = atoi ( strValue );
		}
		strValue = pNode->Attribute(KStrFormulaIndexLastTime);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			last_time = atoi ( strValue );
		}
		
		strValue = pNode->Attribute(KStrFormulaIndexOften);
		if ( NULL != strValue && strlen(strValue) > 0 )
		{
			m_bOften = atoi(strValue);
		}
		else
		{
			m_bOften = false;
		}

		strValue = pNode->Attribute(KStrFormulaIndexProtected);
		if ( NULL != strValue && strlen(strValue) > 0 )
		{
			m_bProtected = atoi(strValue);
		}
		else
		{
			m_bProtected = false;
		}

		strValue = pNode->Attribute(KStrFormulaIndexContent);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			wstring strFormular;
			if ( Utf8ToUnicode(strValue, strFormular) )
			{
				formular = strFormular.c_str();
			}
			else
			{
				formular = _A2W(strValue);
			}
		}

		strValue = pNode->Attribute(KStrFormulaIndexHelp);
		if ( NULL != strValue && strlen(strValue)>0 )
		{
			help = _A2W(strValue);
		}

		strValue = pNode->Attribute(KStrFormulaIndexStockType);		// 指标与股票类型
		if ( NULL != strValue )
		{
			m_byteIndexStockType = (BYTE)atoi(strValue);
		}
		else
		{
			m_byteIndexStockType = EIST_None;
		}
	}
	numPara = 0;
	numLine = 0;
	for(TiXmlNode *pChild = pNode->FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		if ( pNode->Type() == TiXmlNode::ELEMENT )
		{
			TiXmlElement* pElement = (TiXmlElement*)pChild;	
			if ( 0 == strcmp (pElement->Value(),KStrFormulaParam ) )
			{
				strValue = pElement->Attribute(KStrFormulaParamName);
				if ( NULL != strValue && strlen(strValue)>0 )
				{
					namePara[numPara] = _A2W(strValue);
				}
				strValue = pElement->Attribute(KStrFormulaParamDesc);
				if ( NULL != strValue && strlen(strValue)>0 )
				{
					descPara[numPara] = _A2W(strValue);
				}
				strValue = pElement->Attribute(KStrFormulaParamMin);
				if ( NULL != strValue && strlen(strValue)>0 )
				{
					min[numPara] = atof(strValue);
				}
				strValue = pElement->Attribute(KStrFormulaParamMax);
				if ( NULL != strValue && strlen(strValue)>0 )
				{
					max[numPara] = atof(strValue);
				}
				strValue = pElement->Attribute(KStrFormulaParamDefault);
				if ( NULL != strValue && strlen(strValue)>0 )
				{
					defaultVal[numPara] = atof(strValue);
				}
				strValue = pElement->Attribute(KStrFormulaParamStep);
				if ( NULL != strValue && strlen(strValue)>0 )
				{
					stepLen[numPara] = atof(strValue);
				}
				numPara ++;
			}
			else if ( 0 == strcmp (pElement->Value(),KStrFormulaLine ) )
			{
				strValue = pElement->Attribute(KStrFormulaLineY);
				if ( NULL != strValue && strlen(strValue)>0 )
				{
					line[numLine] = atof(strValue);
				}
				numLine ++;
			}
			else if ( 0 == strcmp(pElement->Value(), KStrFormulaExtraY) )
			{
				strValue = pElement->Attribute(KStrFormulaLineY);
				if ( NULL != strValue && strlen(strValue)>0 )
				{
					lineExtraY[numExtraY] = atof(strValue);
				}
				numExtraY ++;
			}
		}
	}
}

bool32 CFormularContent::BePassedXday()
{
	if (NULL == g_pCenterManager)
	{
		return false;
	}

	if (-1 == lXday) return false;
	ASSERT(g_pCenterManager);


	CGmtTime tmSvr = g_pCenterManager->GetServerTime();
	if (lXday <= tmSvr.GetTime()) return true;
 
	return false;
}

bool32 CFormularContent::BeNeedScreenKLineNum()
{
	if (-1 != formular.Find(L"screenum") || -1 != formular.Find(L"SCREENUM"))
	{
		return true;
	}

	return false;
}

CFormularContent* CFormularContent::Clone ( )
{
	CFormularContent* pContent = new CFormularContent;

	pContent->Assign(*this);

	return pContent;
}
bool32 CFormularContent::EqualFormulaData ( CFormularContent* pCompare )
{
	if ( NULL == pCompare )
	{
		return false;
	}

	//
	int32 i;
	if ( pCompare->formular != formular )
	{
		return false;
	}
	if ( pCompare->flag != flag )
	{
		return false;
	}
	if ( pCompare->numLine != numLine )
	{
		return false;
	}
	if ( pCompare->m_eFormularType != m_eFormularType )
	{
		return false;
	}
	for ( i = 0; i < numLine; i ++ )
	{
		if ( pCompare->line[i] != line[i] )
		{
			return false;
		}
	}
	if ( pCompare->numExtraY != numExtraY )
	{
		return false;
	}

	for ( i = 0; i < numExtraY; i++ )
	{
		if ( pCompare->lineExtraY[i] != lineExtraY[i] )
		{
			return false;
		}
	}
	return true;
}
bool32 CFormularContent::EqualParam ( CFormularContent* pCompare )
{
	int32 i;
	if ( pCompare->numPara != numPara )
	{
		return false;
	}
	for ( i = 0; i < numPara; i ++ )
	{
		if ( pCompare->namePara[i] != namePara[i])
		{
			return false;
		}
		if ( pCompare->max[i] != max[i])
		{
			return false;
		}
		if ( pCompare->min[i] != min[i])
		{
			return false;
		}
		if ( pCompare->defaultVal[i] != defaultVal[i])
		{
			return false;
		}
	}
	return true;
}

CFormularContent::CFormularContent()
{
	bNew = FALSE;
	CTime t = CTime::GetCurrentTime();
	last_time = t.GetTime();

	numPara = 0;
	int i;
	for ( i = 0; i < PARAM_NUM; i ++ )
	{
		min[i] = 0.0f;
		max[i] = 0.0f;
		defaultVal[i] = 0.0f;
		stepLen[i] = 0.0f;
		namePara[i] = _T("");
	}
	
	numLine = 0;
	for ( i = 0; i < PARAM_NUM; i ++ )
	{
		line[i] = 0.0f;
	}
	
	numExtraY = 0;
	for ( i = 0; i < EXTRA_Y_NUM; i++ )
	{
		lineExtraY[i] = 0.0f;
	}

	name = _T("");
	password = _T("");
	explainBrief = _T("");
	hotkey = _T("");
	flag	= 0;

	formular = _T("");
	help = _T("");

	m_bProtected	= false;
	m_bOften		= false;
	m_bSystem		= false;
	m_bCanRestore	= false;
//	m_bNeedRefresh = false;
	m_eFormularType	= EFTNormal;
	lXday			= -1;

	m_byteIndexStockType = EIST_None;
}

CFormularContent::~CFormularContent()
{
	defaultValArray.RemoveAll();
}
void CFormularContent::InitNew()
{
	bNew = TRUE;
	m_bOften = true;
	AddFlag(flag,CFormularContent::KAllowMinute);
	AddFlag(flag,CFormularContent::KAllowDay);
	AddFlag(flag,CFormularContent::KAllowMinute5);
	AddFlag(flag,CFormularContent::KAllowMinute15);
	AddFlag(flag,CFormularContent::KAllowMinute30);
	AddFlag(flag,CFormularContent::KAllowMinute60);
	AddFlag(flag,CFormularContent::KAllowMinute180);
	AddFlag(flag,CFormularContent::KAllowMinute240);
	AddFlag(flag,CFormularContent::KAllowMinuteUser);
	AddFlag(flag,CFormularContent::KAllowDayUser);
	AddFlag(flag,CFormularContent::KAllowWeek);
	AddFlag(flag,CFormularContent::KAllowMonth);
	AddFlag(flag,CFormularContent::KAllowQuarter);
	AddFlag(flag,CFormularContent::KAllowYear);
	AddFlag(flag,CFormularContent::KAllowSub);
}
CString CFormularContent::DefaultParamToString()
{
	CString StrText;
	for ( int32 i = 0; i < numPara; i ++ )
	{
		CString Str;
		Str.Format(_T("%.0f"),defaultVal[i]);
		StrText += Str;
		if ( i != numPara-1)
		{
			StrText += _T(",");
		}
	}
	if ( StrText.GetLength() > 0 )
	{
		StrText = _T("(") + StrText + _T(")");
	}
	return StrText;
}
float CFormularContent::GetParamDataEach(int iIndex, CFormularContent *pJishu)
{
	return pJishu->defaultVal[iIndex];
}

bool CFormularContent::IsValid()
{
	//不必检测序列号
	return true;
}

void CFormularContent::InitDefaultValArray()
{
	 if(defaultValArray.GetSize()!=numPara)
	 {
		 defaultValArray.RemoveAll();
		 for(int j=0;j<numPara;j++)
		 {
			 FLOAT11 f11;
			 for(int i=0;i<11;i++)
			 {
				 f11.fValue [i] = defaultVal [j];
			 }
			 defaultValArray.Add(f11);
		 }
	 }
}
void CFormularContent::AddDefaultValToArray()
{
	defaultValArray.RemoveAll();
	FLOAT11 f11;//defaultVal[8];
	for(int i=0;i<this->numPara;i++)
	{
		for(int j=0;j<11;j++)
		{
			f11.fValue[j]=defaultVal[i];
		}
		defaultValArray.Add(f11);
	}

}
float CFormularContent::GetParamDataEach(int iIndex, int nKlineType, CFormularContent *pJishu)
{
	float f = pJishu->defaultVal[iIndex];

	//使用保存各个周期数据
//	if(((CTaiShanDoc*)(CMainFrame::m_taiShanDoc))->m_propertyInitiate.bSaveParam == TRUE)
	{
		if(pJishu->defaultValArray .GetSize()>iIndex)
		{
			int nInd = CTaiScreenParent_FromKlineKindToFoot(nKlineType);
			if(nInd>=0)
				f = pJishu->defaultValArray[iIndex].fValue[nInd];
		}
	}
	return f;
}

bool32 CFormularContent::IsIndexStockTypeMatched( E_IndexStockType eIST )
{
	if ( EIST_None == eIST || EIST_None == m_byteIndexStockType )
	{
		return true;	// 无限制类型
	}

	return eIST == m_byteIndexStockType;
}

const CFormularContent & CFormularContent::Assign( const CFormularContent &forSrc )
{
	if ( this == &forSrc )
	{
		return *this;
	}

	int32 i, iSize;
	CFormularContent* pContent = this;
	
	pContent->bNew		= forSrc.bNew;
	pContent->last_time = forSrc.last_time;
	
	//
	pContent->numLine = forSrc.numLine;
	for ( i = 0; i < forSrc.numLine; i ++ )
	{
		pContent->line[i] = forSrc.line[i];	
	}
	pContent->numPara = forSrc.numPara;
	for ( i = 0; i < forSrc.numPara; i ++ )
	{
		pContent->namePara[i] = forSrc.namePara[i];
		pContent->descPara[i] = forSrc.descPara[i];
		pContent->max[i] = forSrc.max[i];
		pContent->min[i] = forSrc.min[i];
		pContent->defaultVal[i] = forSrc.defaultVal[i];
		pContent->stepLen[i] = forSrc.stepLen[i];
	}
	
	//
	pContent->numExtraY = forSrc.numExtraY;
	for ( i = 0; i < forSrc.numExtraY; i++ )
	{
		pContent->lineExtraY[i] = forSrc.lineExtraY[i];
	}
	
	//
	iSize = forSrc.defaultValArray.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{	
		//lint --e{1058}
		pContent->defaultValArray.Add((FLOAT11)forSrc.defaultValArray.GetAt(i));
	}
	
	pContent->name		= forSrc.name;
	pContent->password	= forSrc.password;
	pContent->explainBrief	= forSrc.explainBrief;
	pContent->hotkey		= forSrc.hotkey;
	pContent->flag			= forSrc.flag;
	pContent->formular		= forSrc.formular;
	pContent->help			= forSrc.help;
	pContent->m_bProtected	= forSrc.m_bProtected;
	
	pContent->m_bOften		= forSrc.m_bOften;
	pContent->m_bSystem		= forSrc.m_bSystem;
	pContent->m_bCanRestore = forSrc.m_bCanRestore;
	pContent->m_eFormularType = forSrc.m_eFormularType;
	
	//	pContent->m_bNeedRefresh = forSrc.m_bNeedRefresh;
	
	pContent->m_byteIndexStockType = forSrc.m_byteIndexStockType;
	
	return *pContent;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
CIndexParamData::CIndexParamData()
{
	name = _T("");
	numPara = 0;
	MEMSET_ARRAY(defaultVal,0.0f);
	MEMSET_ARRAY(min,0.0f);
	MEMSET_ARRAY(max,0.0f);	
}
CIndexParamData::~CIndexParamData()
{

}
CString CIndexParamData::DefaultParamToString()
{
	CString StrText;
	for ( int32 i = 0; i < numPara; i ++ )
	{
		CString Str;
		Str.Format(_T("%.0f"),defaultVal[i]);
		StrText += Str;
		if ( i != numPara-1)
		{
			StrText += _T(",");
		}
	}
	if ( StrText.GetLength() > 0 )
	{
		StrText = _T("(") + StrText + _T(")");
	}
	return StrText;
}
void CIndexParamData::_ToXml( TiXmlElement * pNode )
{
	pNode->SetAttribute(KStrFormulaIndexId,_W2A(name));

	int i;
	char strbuf[256];
	TiXmlElement* pChild;

	while ( pNode->FirstChild())
	{
		pNode->RemoveChild(pNode->FirstChild());
	}

	for ( i = 0; i < numPara; i ++ )
	{
		pChild = new TiXmlElement(KStrFormulaParam);
		pChild->SetAttribute(KStrFormulaParamName,_W2A(namePara[i]));
		pChild->SetAttribute(KStrFormulaParamDesc,_W2A(descPara[i]));
		sprintf ( strbuf, "%f", min[i]);
		pChild->SetAttribute(KStrFormulaParamMin,strbuf);
		sprintf ( strbuf, "%f", max[i]);
		pChild->SetAttribute(KStrFormulaParamMax,strbuf);
		sprintf ( strbuf, "%f", defaultVal[i]);
		pChild->SetAttribute(KStrFormulaParamDefault,strbuf);
//		sprintf ( strbuf, "%f", stepLen[i]);
		pChild->SetAttribute(KStrFormulaParamStep,strbuf);
		pNode->LinkEndChild(pChild);
	}
// 	for ( i = 0; i < numLine; i ++ )
// 	{
// 		pChild = new TiXmlElement(KStrFormulaLine);
// 		sprintf ( strbuf, "%f", line[i]);
// 		pChild->SetAttribute(KStrFormulaLineY,strbuf);
// 		pNode->LinkEndChild(pChild);
// 	}
}
void CIndexParamData::_FromXml ( TiXmlElement * pNode )
{
	const char* strValue = NULL;
	if ( 0 == strcmp(pNode->Value(),KStrFormulaIndex))
	{
		strValue = pNode->Attribute(KStrFormulaIndexId);
		name = _A2W(strValue);
	}
	else
	{
		return;
	}

	numPara = 0;
	for(TiXmlNode *pChild = pNode->FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		if ( pNode->Type() == TiXmlNode::ELEMENT )
		{
			TiXmlElement* pElement = (TiXmlElement*)pChild;	
			if ( 0 == strcmp (pElement->Value(),KStrFormulaParam ) )
			{
				strValue = pElement->Attribute(KStrFormulaParamName);
				namePara[numPara] = _A2W(strValue);
				strValue = pElement->Attribute(KStrFormulaParamDesc);
				if ( NULL != strValue )
				{
					descPara[numPara] = _A2W(strValue);
				}
				strValue = pElement->Attribute(KStrFormulaParamMin);
				min[numPara] = atof(strValue);
				strValue = pElement->Attribute(KStrFormulaParamMax);
				max[numPara] = atof(strValue);
				strValue = pElement->Attribute(KStrFormulaParamDefault);
				defaultVal[numPara] = atof(strValue);
//				strValue = pElement->Attribute(KStrFormulaParamStep);
//				stepLen[numPara] = atof(strValue);
				numPara ++;

			}
// 			else if ( 0 == strcmp (pElement->Value(),KStrFormulaLine ) )
// 			{
// 				strValue = pElement->Attribute(KStrFormulaLineY);
// 				line[numLine] = atof(strValue);
// 				numLine ++;
// 			}
		}
	}
}
void IndexParamToContent ( CIndexParamData* pParam,CFormularContent* pContent )
{
	ASSERT( pContent->name == pParam->name );
	ASSERT( pContent->numPara == pParam->numPara );
	for ( int32 i = 0; i < pParam->numPara; i ++ )
	{
		pContent->namePara[i] = pParam->namePara[i];
		pContent->descPara[i] = pParam->descPara[i];
		pContent->max[i] = pParam->max[i];
		pContent->min[i] = pParam->min[i];
		pContent->defaultVal[i] = pParam->defaultVal[i];
	}
}

void IndexContentToParam ( CFormularContent* pContent,CIndexParamData* pParam )
{
	ASSERT( pContent->name == pParam->name );
	ASSERT( pContent->numPara == pParam->numPara );
	for ( int32 i = 0; i < pParam->numPara; i ++ )
	{
		pParam->namePara[i] = pContent->namePara[i];
		pParam->descPara[i] = pContent->descPara[i];
		pParam->max[i] = pContent->max[i];
		pParam->min[i] = pContent->min[i];
		pParam->defaultVal[i] = pContent->defaultVal[i];
	}
}

///////////////////////////////////////////////////////////////////////
CIndexGroup2::CIndexGroup2()
{
	m_pXmlElement = NULL;
	m_eFormularType = EFTNormal;
}

CIndexGroup2::~CIndexGroup2()
{
	while ( m_Contents.GetSize() > 0 )
	{
		CFormularContent* pContent = m_Contents.GetAt(0);
		DEL(pContent);
		m_Contents.RemoveAt(0);
	}
}

void CIndexGroup2::_ToXml( TiXmlElement * pNode,CMapStringToPtr& Names )
{

}

void CIndexGroup2::_FromXml ( TiXmlElement * pNode,CMapStringToPtr& Names )
{
	for(TiXmlNode *pChild = pNode->FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		if ( pChild->Type() == TiXmlNode::ELEMENT )
		{
			//已经创建到xml节点
			TiXmlElement* pElement = (TiXmlElement*)pChild;

			// 如果校验码不对，就不添加
			CString strCheckCode = _T("");
			const char* pCheckCode = pElement->Attribute(KStrFormulaIndexCheckCode);
			if ( NULL != pCheckCode && strlen(pCheckCode) > 0 )
			{
				strCheckCode = _A2W(pCheckCode);
			}

			//如果指标有配校验码，那必须校验成功，才能使用该指标
			if ((strCheckCode != g_strCheckCode) && (!strCheckCode.IsEmpty()))
			{
				continue;
			}

			CFormularContent* pContent = new CFormularContent;

			//
			const char* strValue = pElement->Attribute(KStrFormulaIndexId);
			
			pContent->name = _A2W(strValue);
			pContent->m_eFormularType = m_eFormularType;
			//
			T_IndexMapPtr* p = new T_IndexMapPtr;
			p->p1 = pContent;
			p->p2 = pElement;

			//添加到索引
			Names[pContent->name] = p;
			pContent->_FromXml(pElement);

			//添加到数组
			m_Contents.Add(pContent);
		}
	}
}

///////////////////////////////////////////////////////////////////////
CIndexGroup1::CIndexGroup1()
{
	m_pXmlElement = NULL;	
	m_eFormularType = EFTNormal;
	
}
//lint --e{1540}
CIndexGroup1::~CIndexGroup1()
{
	
}

//////////////////////////////////////////////////////////////////////////
// 技术指标
CIndexGroupNormal::CIndexGroupNormal():CIndexGroup1()
{
	m_eFormularType = EFTNormal;
}

CIndexGroupNormal::~CIndexGroupNormal()
{
	//lint --e{438}
	while ( m_Group2.GetSize() > 0 )
	{
		CIndexGroup2* pGroup2 = m_Group2.GetAt(0);
		DEL(pGroup2);
		m_Group2.RemoveAt(0);
	}
}

void CIndexGroupNormal::_FromXml(TiXmlElement * pNode, CMapStringToPtr& Names)
{
	if ( NULL == pNode )
	{
		////ASSERT(0);
		return;
	}

	//
	for(TiXmlNode *pChild = pNode->FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		if ( pChild->Type() == TiXmlNode::ELEMENT )
		{
			// 指标公式
			TiXmlElement* pElement = (TiXmlElement*)pChild;
			CIndexGroup2* pGroup2 = new CIndexGroup2;
			
			//
			const char* strValue = pElement->Attribute(KStrFormulaGroup2Name);
			
			//
			pGroup2->m_StrName = _A2W(strValue);
			pGroup2->m_eFormularType = m_eFormularType;
			
			//
			pGroup2->m_pXmlElement = pElement;
			m_Group2.Add(pGroup2);
			
			//
			pGroup2->_FromXml(pElement,Names);
		}
	}
}

void CIndexGroupNormal::_ToXml(TiXmlElement * pNode, CMapStringToPtr& Names)
{
	if ( NULL == pNode )
	{
		////ASSERT(0);
		return;
	}
	
	//
	for(TiXmlNode *pChild = pNode->FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		if ( pChild->Type() == TiXmlNode::ELEMENT )
		{
			// 指标公式
			TiXmlElement* pElement = (TiXmlElement*)pChild;
			CIndexGroup2* pGroup2 = new CIndexGroup2;
			
			//
			const char* strValue = pElement->Attribute(KStrFormulaGroup2Name);
			
			//
			pGroup2->m_StrName = _A2W(strValue);
			pGroup2->m_eFormularType = m_eFormularType;
			
			//
			pGroup2->m_pXmlElement = pElement;
			m_Group2.Add(pGroup2);
			
			//
			pGroup2->_FromXml(pElement,Names);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 条件选股
CIndexGroupCdtChose::CIndexGroupCdtChose()
{
	m_eFormularType = EFTCdtChose;
}

CIndexGroupCdtChose::~CIndexGroupCdtChose()
{
	//lint --e{438}
	while ( m_Group2.GetSize() > 0 )
	{		
		CIndexGroup2* pGroup2 = m_Group2.GetAt(0);
		DEL(pGroup2);
		m_Group2.RemoveAt(0);
	}
}

void CIndexGroupCdtChose::_FromXml(TiXmlElement * pNode, CMapStringToPtr& Names)
{
	if ( NULL == pNode )
	{
		////ASSERT(0);
		return;
	}
	
	//
	for(TiXmlNode *pChild = pNode->FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		if ( pChild->Type() == TiXmlNode::ELEMENT )
		{
			// 指标公式
			TiXmlElement* pElement = (TiXmlElement*)pChild;

			// 如果校验码不对，就不添加
			CString strCheckCode = _T("");
			const char* pCheckCode = pElement->Attribute(KStrFormulaIndexCheckCode);
			if ( NULL != pCheckCode && strlen(pCheckCode) > 0 )
			{
				strCheckCode = _A2W(pCheckCode);
			}

			//如果指标有配校验码，那必须校验成功，才能使用该指标
			if ((strCheckCode != g_strCheckCode) && (!strCheckCode.IsEmpty()))
			{
				continue;
			}

			CIndexGroup2* pGroup2 = new CIndexGroup2;
			
			//
			const char* strValue = pElement->Attribute(KStrFormulaGroup2Name);
			
			//
			pGroup2->m_StrName = _A2W(strValue);
			pGroup2->m_eFormularType = m_eFormularType;
			
			//
			pGroup2->m_pXmlElement = pElement;
			m_Group2.Add(pGroup2);
			
			//
			pGroup2->_FromXml(pElement,Names);
		}
	}
}

void CIndexGroupCdtChose::_ToXml(TiXmlElement * pNode, CMapStringToPtr& Names)
{
	if ( NULL == pNode )
	{
		////ASSERT(0);
		return;
	}
	
	//
	for(TiXmlNode *pChild = pNode->FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		if ( pChild->Type() == TiXmlNode::ELEMENT )
		{
			// 指标公式
			TiXmlElement* pElement = (TiXmlElement*)pChild;
			CIndexGroup2* pGroup2 = new CIndexGroup2;
			
			//
			const char* strValue = pElement->Attribute(KStrFormulaGroup2Name);
			
			//
			pGroup2->m_StrName = _A2W(strValue);
			pGroup2->m_eFormularType = m_eFormularType;
			
			//
			pGroup2->m_pXmlElement = pElement;
			m_Group2.Add(pGroup2);
			
			//
			pGroup2->_FromXml(pElement,Names);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 交易选股
CIndexGroupTradeChose::CIndexGroupTradeChose()
{
	m_eFormularType = EFTTradeChose;
}

CIndexGroupTradeChose::~CIndexGroupTradeChose()
{
	//lint --e{438}
	for ( int32 i = m_ContentsTrade.GetSize() - 1; i >= 0; i-- )
	{
		CFormularContent* p = m_ContentsTrade.GetAt(i);
		DEL(p);
		m_ContentsTrade.RemoveAt(i);
	}
}


void CIndexGroupTradeChose::_FromXml(TiXmlElement * pNode, CMapStringToPtr& Names)
{
	if ( NULL == pNode )
	{
		////ASSERT(0);
		return;
	}
	
	//
	for(TiXmlNode *pChild = pNode->FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		if ( pChild->Type() == TiXmlNode::ELEMENT )
		{
			// 五彩K 线				
			TiXmlElement* pElement = (TiXmlElement*)pChild;

			// 如果校验码不对，就不添加
			CString strCheckCode = _T("");
			const char* pCheckCode = pElement->Attribute(KStrFormulaIndexCheckCode);
			if ( NULL != pCheckCode && strlen(pCheckCode) > 0 )
			{
				strCheckCode = _A2W(pCheckCode);
			}

			//如果指标有配校验码，那必须校验成功，才能使用该指标
			if ((strCheckCode != g_strCheckCode) && (!strCheckCode.IsEmpty()))
			{
				continue;
			}

			CFormularContent* pContent = new CFormularContent;
			//
			const char* strValue = pElement->Attribute(KStrFormulaIndexId);
			
			pContent->name = _A2W(strValue);
			pContent->m_eFormularType = m_eFormularType;
			
			//
			T_IndexMapPtr* p = new T_IndexMapPtr;
			p->p1 = pContent;
			p->p2 = pElement;
			
			//添加到索引
			Names[pContent->name] = p;
			pContent->_FromXml(pElement);
			
			//添加到数组
			m_ContentsTrade.Add(pContent);
		}
	}
}

void CIndexGroupTradeChose::_ToXml(TiXmlElement * pNode, CMapStringToPtr& Names)
{
	
}
//////////////////////////////////////////////////////////////////////////
// 五彩 k 线
CIndexGroupClrKLine::CIndexGroupClrKLine():CIndexGroup1()
{
	m_eFormularType = EFTClrKLine;
}

CIndexGroupClrKLine::~CIndexGroupClrKLine()
{
	//lint --e{438}
	for ( int32 i = m_ContentsClrKLine.GetSize() - 1; i >= 0; i-- )
	{
		CFormularContent* p = m_ContentsClrKLine.GetAt(i);
		DEL(p);
		m_ContentsClrKLine.RemoveAt(i);
	}
}

void CIndexGroupClrKLine::_FromXml(TiXmlElement * pNode, CMapStringToPtr& Names)
{
	if ( NULL == pNode )
	{
		////ASSERT(0);
		return;
	}
	
	//
	for(TiXmlNode *pChild = pNode->FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		if ( pChild->Type() == TiXmlNode::ELEMENT )
		{
			// 五彩K 线				
			TiXmlElement* pElement = (TiXmlElement*)pChild;

			// 如果校验码不对，就不添加
			CString strCheckCode = _T("");
			const char* pCheckCode = pElement->Attribute(KStrFormulaIndexCheckCode);
			if ( NULL != pCheckCode && strlen(pCheckCode) > 0 )
			{
				strCheckCode = _A2W(pCheckCode);
			}

			//如果指标有配校验码，那必须校验成功，才能使用该指标
			if ((strCheckCode != g_strCheckCode) && (!strCheckCode.IsEmpty()))
			{
				continue;
			}

			CFormularContent* pContent = new CFormularContent;
			
			//
			const char* strValue = pElement->Attribute(KStrFormulaIndexId);
			
			pContent->name = _A2W(strValue);
			pContent->m_eFormularType = m_eFormularType;
			
			//
			T_IndexMapPtr* p = new T_IndexMapPtr;
			p->p1 = pContent;
			p->p2 = pElement;
			
			//添加到索引
			Names[pContent->name] = p;
			pContent->_FromXml(pElement);
			
			//添加到数组
			m_ContentsClrKLine.Add(pContent);
		}
	}
}

void CIndexGroupClrKLine::_ToXml(TiXmlElement * pNode, CMapStringToPtr& Names)
{

}

//////////////////////////////////////////////////////////////////////////
CIndexContentXml::CIndexContentXml()
{
	
	m_pXmlDocument		= NULL;
	m_pRootElementRAM	= NULL;
	m_pGroupNormal		= NULL;
	m_pGroupCdtChose	= NULL;
	m_pGroupTradeChose	= NULL;
	m_pGroupClrKLine	= NULL;
}
//lint --e{1579}
CIndexContentXml::~CIndexContentXml()
{
	Free();
}

bool32 CIndexContentXml::Load(const char* strFile )
{
	Free();
	
	//
	DEL(m_pXmlDocument);

	m_pXmlDocument = new TiXmlDocument(strFile);
	if ( !m_pXmlDocument->LoadFile() )
	{
		Free();
		return false;
	}

	//
	if ( !DeCodeXml(strFile, m_pXmlDocument) )
	{
		Free();
		return false;
	}

	//
	m_pRootElementRAM	= m_pXmlDocument->RootElement();

	//
	for(TiXmlNode *pNode = m_pRootElementRAM->FirstChild(); pNode; pNode = pNode->NextSibling())
	{
		if ( pNode->Type() == TiXmlNode::ELEMENT )
		{
			TiXmlElement* pElement = (TiXmlElement*)pNode;
					
			// 
			const char* strName = pElement->Attribute(KStrFormulaGroup1Name);
			const char* strType = pElement->Attribute(KStrFormulaGroup1Type);
			
			//
			E_FormularType eType = EFTNormal;

			if ( NULL == strType )
			{
				eType = EFTNormal;
			}
			else
			{
				int32 iType = atoi(strType);

				if ( iType < 0 || iType >= EFTCount )
				{
					iType = 0;
				}

				eType = (E_FormularType)iType;
			}

			if ( eType == EFTNormal )
			{
				m_pGroupNormal = new CIndexGroupNormal();
				
				m_pGroupNormal->m_StrName = _A2W(strName);
				m_pGroupNormal->m_pXmlElement = pElement;
				
				m_pGroupNormal->_FromXml(pElement, m_Names4Index);
			}
			else if ( eType == EFTCdtChose )
			{
				m_pGroupCdtChose = new CIndexGroupCdtChose();

				m_pGroupCdtChose->m_StrName = _A2W(strName);
				m_pGroupCdtChose->m_pXmlElement = pElement;

				m_pGroupCdtChose->_FromXml(pElement, m_Names4Index);
			}
			else if ( eType == EFTTradeChose )
			{
				m_pGroupTradeChose = new CIndexGroupTradeChose();

				m_pGroupTradeChose->m_StrName = _A2W(strName);
				m_pGroupTradeChose->m_pXmlElement = pElement;

				m_pGroupTradeChose->_FromXml(pElement, m_Names4Index);
			}
			else if ( eType == EFTClrKLine )
			{
				m_pGroupClrKLine = new CIndexGroupClrKLine();

				m_pGroupClrKLine->m_StrName = _A2W(strName);
				m_pGroupClrKLine->m_pXmlElement = pElement;
				
				m_pGroupClrKLine->_FromXml(pElement, m_Names4Index);
			}			
		}
	}

	return true;
}

void CIndexContentXml::Free()
{
	// 索引要del
	POSITION pos = m_Names4Index.GetStartPosition();
	CString Str;
	while (NULL != pos)
	{
		void *pV = NULL;
		m_Names4Index.GetNextAssoc(pos, Str, pV);
		delete pV;
	}
	m_Names4Index.RemoveAll();
	DEL(m_pXmlDocument);
	m_pRootElementRAM = NULL;
	DEL(m_pGroupNormal);
	DEL(m_pGroupCdtChose);
	DEL(m_pGroupTradeChose);
	DEL(m_pGroupClrKLine);
}

T_IndexMapPtr* CIndexContentXml::Get ( CString StrName )
{
	if ( NULL == m_pXmlDocument )
	{
		return NULL;
	}
	
	T_IndexMapPtr* p = NULL;
	if ( m_Names4Index.Lookup(StrName,(void*&)p) && NULL != p )
	{
		return p;
	}

	return NULL;
}

void CIndexContentXml::Save()
{

	if ( NULL != m_pXmlDocument )
	{
		if (0 == chmod(m_pXmlDocument->Value(),_S_IWRITE))
		{
			// 加密xml 文档		
			EnCodeXml(m_pXmlDocument, m_pRootElementRAM);
		}
	}
		
}

bool32 CIndexContentXml::GetGroup(CFormularContent* pContent, CString& StrGroup1, CString& StrGroup2)
{
	StrGroup1 = L"";
	StrGroup2 = L"";

	if ( NULL == pContent )
	{
		return false;
	}

	//
	if ( EFTNormal == pContent->m_eFormularType )
	{
		if ( NULL == m_pGroupNormal )
		{
			return false;
		}

		//
		for ( int32 i = 0; i < m_pGroupNormal->m_Group2.GetSize(); i++ )
		{
			CIndexGroup2* pGroup2 = m_pGroupNormal->m_Group2.GetAt(i);
			if ( NULL == pGroup2 )
			{
				continue;
			}

			//
			for ( int32 j = 0; j < pGroup2->m_Contents.GetSize(); j++ )
			{
				if ( pGroup2->m_Contents.GetAt(j) == pContent )
				{
					StrGroup1 = m_pGroupNormal->m_StrName;
					StrGroup2 = pGroup2->m_StrName;

					return true;						 
				}
			}
		}
	}
	else if ( EFTClrKLine == pContent->m_eFormularType )
	{
		if ( NULL == m_pGroupClrKLine )
		{
			return false;
		}

		for ( int32 i = 0; i < m_pGroupClrKLine->m_ContentsClrKLine.GetSize(); i++ )
		{
			if ( m_pGroupClrKLine->m_ContentsClrKLine.GetAt(i) == pContent )
			{
				StrGroup1 = m_pGroupClrKLine->m_StrName;
				return true;
			}		
		}
	}
	else if ( EFTTradeChose == pContent->m_eFormularType )
	{
		if ( NULL == m_pGroupTradeChose )
		{
			return false;
		}
		for ( int32 i=0; i < m_pGroupTradeChose->m_ContentsTrade.GetSize() ; i++ )
		{
			if ( m_pGroupTradeChose->m_ContentsTrade[i] == pContent )
			{
				StrGroup1 = m_pGroupTradeChose->m_StrName;
				return true;
			}
		}
	}
	else if ( EFTCdtChose == pContent->m_eFormularType )
	{
		if ( NULL == m_pGroupCdtChose )
		{
			return false;
		}
		for ( int32 i=0; i < m_pGroupCdtChose->m_Group2.GetSize() ; i++ )
		{
			//
			CIndexGroup2 *pGroup2 = m_pGroupCdtChose->m_Group2[i];
			for ( int32 j = 0; j < pGroup2->m_Contents.GetSize(); j++ )
			{
				if ( pGroup2->m_Contents.GetAt(j) == pContent )
				{
					StrGroup1 = m_pGroupCdtChose->m_StrName;
					StrGroup2 = pGroup2->m_StrName;
					
					return true;						 
				}
			}
		}
	}

	return false;
}

CIndexGroupCdtChose* CIndexContentXml::NewGroupCdtChose()
{
	if ( NULL != m_pGroupCdtChose )
	{
		return m_pGroupCdtChose;
	}

	if (NULL == m_pRootElementRAM)
	{
		return m_pGroupCdtChose;
	}

	// group1 
	m_pGroupCdtChose = new CIndexGroupCdtChose();
	m_pGroupCdtChose->m_StrName = KStrUserGroupCdtChose;

	TiXmlElement* pElement = new TiXmlElement(KStrFormulaGroup1);

	// group1 xml 节点
	pElement->SetAttribute(KStrFormulaGroup1Name, _W2A(m_pGroupCdtChose->m_StrName));
	pElement->SetAttribute(KStrFormulaGroup1Type, int32(m_pGroupCdtChose->m_eFormularType));

	m_pGroupCdtChose->m_pXmlElement = pElement;
	m_pRootElementRAM->LinkEndChild(pElement);

	// group2
	CIndexGroup2* pGroup2 = new CIndexGroup2();
	pGroup2->m_StrName = KStrUserGroup2;
	pGroup2->m_eFormularType = m_pGroupCdtChose->m_eFormularType;
	
	// group2 xml 节点
	TiXmlElement* pElementGroup2 = new TiXmlElement(KStrFormulaGroup2);
	pElementGroup2->SetAttribute(KStrFormulaGroup2Name, _W2A(KStrUserGroup2));
	
	pGroup2->m_pXmlElement = pElementGroup2;	

	// 跟父节点关联起来
	m_pGroupCdtChose->m_pXmlElement->LinkEndChild(pElementGroup2);	
	m_pGroupCdtChose->m_Group2.Add(pGroup2);
	
	return m_pGroupCdtChose;
}

CIndexGroupTradeChose* CIndexContentXml::NewGroupTradeChose()
{
	if ( NULL != m_pGroupTradeChose )
	{
		return m_pGroupTradeChose;
	}

	if ( NULL == m_pRootElementRAM )
	{
		return m_pGroupTradeChose;
	}
	

	m_pGroupTradeChose = new CIndexGroupTradeChose();
	m_pGroupTradeChose->m_StrName = KStrUserGroupTradeChose;
	
	//
	TiXmlElement* pElement = new TiXmlElement(KStrFormulaGroup1);	
	
	//
	pElement->SetAttribute(KStrFormulaGroup1Name, _W2A(m_pGroupTradeChose->m_StrName));
	
	//
	pElement->SetAttribute(KStrFormulaGroup1Type, int32(m_pGroupTradeChose->m_eFormularType));
	
	//
	m_pGroupTradeChose->m_pXmlElement = pElement;
	m_pRootElementRAM->LinkEndChild(pElement);
	
	//
	return m_pGroupTradeChose;
}

CIndexGroupClrKLine* CIndexContentXml::NewGroupClrKLine()
{
	if ( NULL != m_pGroupClrKLine )
	{
		return m_pGroupClrKLine;
	}


	if (NULL == m_pRootElementRAM)
	{
		return m_pGroupClrKLine;
	}

	m_pGroupClrKLine = new CIndexGroupClrKLine();
	m_pGroupClrKLine->m_StrName = KStrUserGroupClrKLine;
	
	TiXmlElement* pElement = new TiXmlElement(KStrFormulaGroup1);	

 	//
 	pElement->SetAttribute(KStrFormulaGroup1Name, _W2A(m_pGroupClrKLine->m_StrName));
 
 	//
 	pElement->SetAttribute(KStrFormulaGroup1Type, int32(m_pGroupClrKLine->m_eFormularType));
 
 	//
 	m_pGroupClrKLine->m_pXmlElement = pElement;
 	m_pRootElementRAM->LinkEndChild(pElement);

	//
	return m_pGroupClrKLine;
}

//外部创建pContent,不判断重复
bool32 CIndexContentXml::Add(CString StrGroup1, CString StrGroup2, CFormularContent* pContent)
{	
	//
	if ( NULL == pContent )
	{
		////ASSERT(0);
		return false;
	}

	CFormularContent *pContentAdd = NULL;
	//
	if ( EFTNormal == pContent->m_eFormularType )
	{
		// 普通技术指标
		if ( NULL == m_pGroupNormal )
		{
			////ASSERT(0);
			return false;
		}

		if ( 0 != m_pGroupNormal->m_StrName.CompareNoCase(StrGroup1) )
		{
			// 传入的不一样, Group 只有一个节点. 不要新建
			////ASSERT(0);
			return false;				
		}

		//
		CIndexGroup2* pGroup2 = NULL;
		int32 iSizeGroup2 = m_pGroupNormal->m_Group2.GetSize();

		for ( int32 i = 0; i < iSizeGroup2; i ++ )
		{
			pGroup2 = m_pGroupNormal->m_Group2.GetAt(i);
			
			if ( 0 == pGroup2->m_StrName.CompareNoCase(StrGroup2) )
			{
				break;
			}

			pGroup2 = NULL;
		}

		//
		if ( NULL == pGroup2 )
		{
			pGroup2 = new CIndexGroup2;
			pGroup2->m_StrName = StrGroup2;		

			TiXmlElement* pNode2 = new TiXmlElement(KStrFormulaGroup2);
			pNode2->SetAttribute(KStrFormulaGroup2Name,_W2A(StrGroup2));

			pGroup2->m_pXmlElement = pNode2;
			m_pGroupNormal->m_pXmlElement->LinkEndChild(pNode2);
			m_pGroupNormal->m_Group2.Add(pGroup2);
			Save();
		}		

		//		
	
		
		//创建xml节点
		TiXmlElement* pNode = new TiXmlElement(KStrFormulaIndex);

		//添加到xml
		pGroup2->m_pXmlElement->LinkEndChild(pNode);

		//添加到数组
		pContentAdd = pContent->Clone(); // 添加新的副本content，析构时不至于删除两次
		pGroup2->m_Contents.Add(pContentAdd);
		pContentAdd->_ToXml(pNode);
		Save();

		//添加到索引
		T_IndexMapPtr* p = new T_IndexMapPtr;
		p->p1 = pContentAdd;
		p->p2 = pNode;
		m_Names4Index[pContentAdd->name] = p;
	}
	else if ( EFTCdtChose == pContent->m_eFormularType )
	{
		// 条件选股
		if ( NULL == m_pGroupCdtChose )
		{
			m_pGroupCdtChose = NewGroupCdtChose();					
			
			if ( NULL == m_pGroupCdtChose )
			{
				////ASSERT(0);
				return false;
			}
		}
		
		if ( 0 != m_pGroupCdtChose->m_StrName.CompareNoCase(StrGroup1) )
		{
			// 传入的不一样, Group 只有一个节点. 不要新建
			////ASSERT(0);
			return false;				
		}
		
		//
		CIndexGroup2* pGroup2 = NULL;
		int32 iSizeGroup2 = m_pGroupCdtChose->m_Group2.GetSize();
		
		for ( int32 i = 0; i < iSizeGroup2; i ++ )
		{
			pGroup2 = m_pGroupCdtChose->m_Group2.GetAt(i);
			
			if ( 0 == pGroup2->m_StrName.CompareNoCase(StrGroup2) )
			{
				break;
			}
			
			pGroup2 = NULL;
		}
		
		//
		if ( NULL == pGroup2 )
		{
			pGroup2 = new CIndexGroup2;
			pGroup2->m_StrName = StrGroup2;		
			
			TiXmlElement* pNode2 = new TiXmlElement(KStrFormulaGroup2);
			pNode2->SetAttribute(KStrFormulaGroup2Name,_W2A(StrGroup2));
			
			pGroup2->m_pXmlElement = pNode2;
			m_pGroupCdtChose->m_pXmlElement->LinkEndChild(pNode2);
			m_pGroupCdtChose->m_Group2.Add(pGroup2);
			Save();
		}		
		
		//		
	
		//创建xml节点
		TiXmlElement* pNode = new TiXmlElement(KStrFormulaIndex);
		
		//添加到xml
		pGroup2->m_pXmlElement->LinkEndChild(pNode);
		
		//添加到数组
		pContentAdd = pContent->Clone(); // 添加新的副本content，析构时不至于删除两次
		pGroup2->m_Contents.Add(pContentAdd);
		pContentAdd->_ToXml(pNode);
		Save();
		
		//添加到索引
		T_IndexMapPtr* p = new T_IndexMapPtr;
		p->p1 = pContentAdd;
		p->p2 = pNode;
		m_Names4Index[pContentAdd->name] = p;
	}
	else if ( EFTTradeChose == pContent->m_eFormularType )
	{
		// 交易
		if ( NULL == m_pGroupTradeChose )
		{		
			// 新建一个分组
			m_pGroupTradeChose = NewGroupTradeChose();
			
			//
			if ( NULL == m_pGroupTradeChose )
			{
				////ASSERT(0);
				return false;
			}			
		}

		//创建xml节点
		TiXmlElement* pNode = new TiXmlElement(KStrFormulaIndex);
		
		//添加到xml
		m_pGroupTradeChose->m_pXmlElement->LinkEndChild(pNode);
		
		//添加到数组
		pContentAdd = pContent->Clone(); // 添加新的副本content，析构时不至于删除两次
		m_pGroupTradeChose->m_ContentsTrade.Add(pContentAdd);
		pContentAdd->_ToXml(pNode);
		Save();
		
		//添加到索引
		T_IndexMapPtr* p = new T_IndexMapPtr;
		p->p1 = pContentAdd;
		p->p2 = pNode;
		m_Names4Index[pContentAdd->name] = p;
	}
	else if ( EFTClrKLine == pContent->m_eFormularType )
	{
		// 五彩K 线
		if ( NULL == m_pGroupClrKLine )
		{
			// 新建一个分组
			m_pGroupClrKLine = NewGroupClrKLine();

			//
			if ( NULL == m_pGroupClrKLine )
			{
				////ASSERT(0);
				return false;
			}
		}

		//创建xml节点
		TiXmlElement* pNode = new TiXmlElement(KStrFormulaIndex);
		
		//添加到xml
		m_pGroupClrKLine->m_pXmlElement->LinkEndChild(pNode);
		
		//添加到数组
		pContentAdd = pContent->Clone(); // 添加新的副本content，析构时不至于删除两次
		m_pGroupClrKLine->m_ContentsClrKLine.Add(pContentAdd);
		pContentAdd->_ToXml(pNode);
		Save();
		
		//添加到索引
		T_IndexMapPtr* p = new T_IndexMapPtr;
		p->p1 = pContentAdd;
		p->p2 = pNode;
		m_Names4Index[pContentAdd->name] = p;
	}

	if ( NULL != pContentAdd )
	{
		if ( this == &CFormulaLib::instance()->m_ModifyIndex )
		{
			pContentAdd->m_bCanRestore = true;	// 系统修改指标
		}
	}

	//CFormulaLib::instance()->BuildHotKeyList();	// 新增添加到快捷列表

	return true;
}

//内部删除pContent,内部判断是否存在(当然)
bool32 CIndexContentXml::Del (CString StrName)
{
	T_IndexMapPtr* p = Get(StrName);
	if ( NULL == p )
	{
		return false;
	}

	
	CIndexGroup2* pGroup2 = NULL;
	CFormularContent* pContent = NULL;
	
	if ( NULL != m_pGroupNormal )
	{
		for ( int32 i = 0; i < m_pGroupNormal->m_Group2.GetSize(); i++ )
		{
			pGroup2 = m_pGroupNormal->m_Group2.GetAt(i);
			
			if ( NULL == pGroup2 )
			{
				continue;
			}

			//
			for ( int32 j = 0; j < pGroup2->m_Contents.GetSize(); j++ )
			{
				pContent = pGroup2->m_Contents.GetAt(j);
				if ( NULL == pContent )
				{
					continue;
				}

				//
				if ( pContent->name == StrName )
				{
					//删除xml节点
					pGroup2->m_pXmlElement->RemoveChild((TiXmlElement*)p->p2);					
					Save();
					
					//从索引中删除
					m_Names4Index.RemoveKey(StrName);
					
					//从数组中删除
					pGroup2->m_Contents.RemoveAt(j);
					
					//
					DEL(pContent);
					
					//CFormulaLib::instance()->BuildHotKeyList(); // 重建列表
					return true;
				}
			}
		}
	}

	if ( NULL != m_pGroupCdtChose )
	{
		for ( int32 i = 0; i < m_pGroupCdtChose->m_Group2.GetSize(); i++ )
		{
			pGroup2 = m_pGroupCdtChose->m_Group2.GetAt(i);
			
			if ( NULL == pGroup2 )
			{
				continue;
			}
			
			//
			for ( int32 j = 0; j < pGroup2->m_Contents.GetSize(); j++ )
			{
				pContent = pGroup2->m_Contents.GetAt(j);
				if ( NULL == pContent )
				{
					continue;
				}
				
				//
				if ( pContent->name == StrName )
				{
					//删除xml节点
					pGroup2->m_pXmlElement->RemoveChild((TiXmlElement*)p->p2);					
					Save();
					
					//从索引中删除
					m_Names4Index.RemoveKey(StrName);
					
					//从数组中删除
					pGroup2->m_Contents.RemoveAt(j);
					
					//
					DEL(pContent);
					
					//CFormulaLib::instance()->BuildHotKeyList(); // 重建列表
					return true;
				}
			}
		}
	}

	if ( NULL != m_pGroupTradeChose )
	{
		for ( int32 i = 0; i < m_pGroupTradeChose->m_ContentsTrade.GetSize(); i++ )
		{
			pContent = m_pGroupTradeChose->m_ContentsTrade.GetAt(i);
			
			if ( NULL == pContent )
			{
				continue;
			}
			
			//
			if ( pContent->name == StrName )
			{
				// 删除 xml
				m_pGroupTradeChose->m_pXmlElement->RemoveChild((TiXmlElement*)p->p2);
				Save();
				
				//
				m_Names4Index.RemoveKey(StrName);
				
				//
				m_pGroupTradeChose->m_ContentsTrade.RemoveAt(i);
				
				//
				DEL(pContent);
				//CFormulaLib::instance()->BuildHotKeyList();
				return true;
			}
		}
	}

	if ( NULL != m_pGroupClrKLine )
	{
		for ( int32 i = 0; i < m_pGroupClrKLine->m_ContentsClrKLine.GetSize(); i++ )
		{
			pContent = m_pGroupClrKLine->m_ContentsClrKLine.GetAt(i);
			
			if ( NULL == pContent )
			{
				continue;
			}
			
			//
			if ( pContent->name == StrName )
			{
				// 删除 xml
				m_pGroupClrKLine->m_pXmlElement->RemoveChild((TiXmlElement*)p->p2);
				Save();

				//
				m_Names4Index.RemoveKey(StrName);

				//
				m_pGroupClrKLine->m_ContentsClrKLine.RemoveAt(i);

				//
				DEL(pContent);
				//CFormulaLib::instance()->BuildHotKeyList();
				return true;
			}
		}
	}

	return false;
}

//不能修改pContent的name
bool32 CIndexContentXml::Modify (CFormularContent* pContent)
{
	if (NULL == m_pXmlDocument)
	{
		return false;
	}

	CString StrName = pContent->name;

	T_IndexMapPtr* p = Get(pContent->name);
	if ( NULL == p )
	{
		return false;
	}

	TiXmlElement* pElement = (TiXmlElement*)p->p2;
	pContent->_ToXml(pElement);
	Save();
	
	return true;
}


void CIndexContentXml::CheckIndexTryDate()
{
	if(NULL == m_pXmlDocument)
	{
		return;
	}
	// 获得当前系统时间
	time_t nowtime;
	time(&nowtime);

	const TiXmlElement *pXMLDATA = m_pXmlDocument->RootElement();
	if (pXMLDATA)
	{
		const TiXmlElement *pGroup1 = pXMLDATA->FirstChildElement();
		for ( ; pGroup1 != NULL ; pGroup1 = pGroup1->NextSiblingElement())
		{
			const TiXmlElement *pGroup2 = pGroup1->FirstChildElement();
			for ( ; pGroup2 != NULL ; pGroup2 = pGroup2->NextSiblingElement())
			{
				TiXmlElement *pIndex = const_cast<TiXmlElement*>(pGroup2->FirstChildElement());
				if (pIndex)
				{
					if (strcmp(pIndex->Value(), "index") != 0)
						continue;
				}
				for ( ; pIndex != NULL ; pIndex = pIndex->NextSiblingElement())
				{
					char *pEffectiveTime = const_cast<char*>(pIndex->Attribute(KStrFormulaIndexEffectiveTime));
					if (!pEffectiveTime)
					{
						pIndex->SetAttribute(KStrFormulaIndexEffectiveTime,"");// 这里写入的是天数
					}
					else
					{
						if (strlen(pEffectiveTime) > 0)
						{
							time_t ttStartTime= nowtime;
							const char* pID = pIndex->Attribute(KStrFormulaIndexId);
							char *pStartTime = const_cast<char*>(pIndex->Attribute(KStrFormulaIndexStartTime));
							if (!pStartTime || strlen(pStartTime) <= 0)
							{
								pIndex->SetAttribute(KStrFormulaIndexStartTime,nowtime);// 备注：这里写入的开始时间是秒数
							}
							else
							{
								ttStartTime= _atoi64(pStartTime);
							}
							
							time_t tt = nowtime - ttStartTime;
							time_t tt1 = _atoi64(pEffectiveTime)*24*60*60;// 将有效时间转换为妙
							if (tt >= tt1)
							{
								// 保存超过有效期的指标
								if(pID)
								{
									CString strName;
									TCHAR szbuf[256]={0};
									MultiCharCoding2Unicode(EMCCUtf8,pID,strlen(pID),szbuf,256);
									strName.Format(_T("%s"), szbuf);

									CPluginFuncRight::Instance().AddIndexRight(strName);
								}
							}
							
						}
					}
				}
			}
		}
	}

}



//////////////////////////////////////////////////////////////////////////
static CFormulaLib* sFormulaLib = NULL;
//lint --e{1401}
CFormulaLib::CFormulaLib()
{

}
CFormulaLib::~CFormulaLib()
{
	if ( NULL != sFormulaLib )
	{
		sFormulaLib = NULL;
	}
	POSITION pos = m_AllocContents.GetStartPosition();
	int32 nKey = -1;
	CFormularContent *pValue = NULL;
	while(pos)
	{
		m_AllocContents.GetNextAssoc(pos,nKey,pValue);
		if (pValue)
		{
			delete pValue;
			pValue = NULL;
		}
	}
	m_AllocContents.RemoveAll();
}
CFormulaLib* CFormulaLib::instance()
{
	if ( NULL == sFormulaLib )
	{
		sFormulaLib = new CFormulaLib;
		sFormulaLib->Reload();
	}

	// fangz zhibiao 需要初始化 m_BaseInfo. 静态数据
	
	return sFormulaLib;
}

void CreateFormulaXml ( TiXmlDocument*& pDoc, char* strName )
{
	CString StrDefaultXml = _T("<?xml version='1.0' encoding='utf-8' ?><XMLDATA version='1.0' app='ggtong' data='index'> \
								<group1 name = \"技术指标公式\" type = \"0\" > <group2 name = \"技术指标\"/> \
								</group1> \
								<group1 name = \"条件选股公式\" type = \"2\" > <group2 name = \"条件选股\"/> \
								</group1> \
								<group1 name = \"交易系统公式\" type = \"3\" > \
								</group1> \
								<group1 name = \"五彩K线\" type = \"1\"> \
								</group1> \
								</XMLDATA>");
	pDoc = new TiXmlDocument(strName);
	char* strDefaultXml = _W2A(StrDefaultXml);
	pDoc->Parse(strDefaultXml);
	pDoc->SaveFile();
}
void CreateFormulaXml2 ( TiXmlDocument*& pDoc, char* strName )
{
	CString StrDefaultXml = _T("<?xml version='1.0' encoding='utf-8' ?><XMLDATA version='1.0' app='ggtong' data='index'> \
							   <group1 name = \"自编技术公式\" type = \"0\" > <group2 name = \"自编\"/> \
							   </group1> \
							   <group1 name = \"自编条件选股公式\" type = \"2\" > <group2 name = \"自编\"/> \
							   </group1> \
							   <group1 name = \"自编交易系统公式\" type = \"3\" > \
							   </group1> \
							   <group1 name = \"自编五彩K线\" type = \"1\"> \
							   </group1> \
							   </XMLDATA>");
	pDoc = new TiXmlDocument(strName);
	char* strDefaultXml = _W2A(StrDefaultXml);
	pDoc->Parse(strDefaultXml);
	bool bRet = pDoc->SaveFile();
	DWORD dwErro = GetLastError();
	int a = 10;
}

void CFormulaLib::Reload()
{
	// 系统指标
	CString strFile = CPathFactory::GetSysIndexFileFullName();
	
	//char *pbuf = NULL;
	string str;
#ifdef UNICODE
	Unicode2MultiChar(CP_ACP,strFile,str);
#else
	str = strFile;
#endif
	if ( !m_SysIndex.Load(str.c_str()) )
	{
		CreateFormulaXml(m_SysIndex.m_pXmlDocument,const_cast<char*>(str.c_str()));
		m_SysIndex.Load(str.c_str());
	}


	//检查下指标的试用情况
	m_SysIndex.CheckIndexTryDate();
	// 系统指标自动加密
	m_SysIndex.Save();		

	// 系统修改指标
	CString strUserName;
	if (g_pCenterManager)
	{
		strUserName = 	g_pCenterManager->GetUserName();
	}
	strFile = CPathFactory::GetModifyIndexFileFullName(strUserName).GetBuffer(0);
#ifdef UNICODE
	Unicode2MultiChar(CP_ACP,strFile,str);
#else
	str = strFile;
#endif
	if ( !m_ModifyIndex.Load(str.c_str()) )
	{
		CreateFormulaXml(m_ModifyIndex.m_pXmlDocument,const_cast<char*>(str.c_str()));
		m_ModifyIndex.Load(str.c_str());
	}

	// 用户指标
	strFile = CPathFactory::GetUserIndexFileFullName(strUserName).GetBuffer(0);
#ifdef UNICODE
	Unicode2MultiChar(CP_ACP,strFile,str);
#else
	str = strFile;
#endif
	if ( !m_UserIndex.Load(str.c_str()) )
	{
		CreateFormulaXml2(m_UserIndex.m_pXmlDocument,const_cast<char*>(str.c_str()));
		m_UserIndex.Load(str.c_str());
	}

	// 调整各个指标的system等其它属性
	{
		POSITION pos = NULL;
		T_IndexMapPtr* p = NULL;
		CString StrName;
		CFormularContent *pContent = NULL;
		// 系统
		CIndexContentXml *pIndex = &m_SysIndex;
		for ( pos = pIndex->m_Names4Index.GetStartPosition(); pos != NULL; )
		{
			p = NULL;
			pIndex->m_Names4Index.GetNextAssoc(pos, StrName, (void*&)p);
			
			if ( NULL == p )
			{
				continue;
			}
			pContent = (CFormularContent*)p->p1;
			
			if ( NULL == pContent )
			{
				continue;
			}

			pContent->m_bSystem		= true;
			pContent->m_bCanRestore = false;
			
			if ( EFTNormal != pContent->m_eFormularType )
			{
				pContent->m_bOften = false;	// 非普通指标都不能为常见
			}
		}
		// 修改
		pIndex = &m_ModifyIndex;
		for ( pos = pIndex->m_Names4Index.GetStartPosition(); pos != NULL; )
		{
			p = NULL;
			pIndex->m_Names4Index.GetNextAssoc(pos, StrName, (void*&)p);
			
			if ( NULL == p )
			{
				continue;
			}
			pContent = (CFormularContent*)p->p1;
			
			if ( NULL == pContent )
			{
				continue;
			}

			// 看现在的系统里面有没，没有则不能恢复 OR 删除它？
			pContent->m_bSystem		= true;
			pContent->m_bCanRestore = true;
			
			if ( EFTNormal != pContent->m_eFormularType )
			{
				pContent->m_bOften = false;	// 非普通指标都不能为常见
			}
		}
		// 用户
		pIndex = &m_UserIndex;
		for ( pos = pIndex->m_Names4Index.GetStartPosition(); pos != NULL; )
		{
			p = NULL;
			pIndex->m_Names4Index.GetNextAssoc(pos, StrName, (void*&)p);
			
			if ( NULL == p )
			{
				continue;
			}
			pContent = (CFormularContent*)p->p1;
			
			if ( NULL == pContent )
			{
				continue;
			}
			
			pContent->m_bSystem		= false;
			pContent->m_bCanRestore = false;
			
			if ( EFTNormal != pContent->m_eFormularType )
			{
				pContent->m_bOften = false;	// 非普通指标都不能为常见
			}
		}
	}
}

///////////////////////////////////////////////////////////////
/*void CFormulaLib::AsignFlag ( CIndexContentXml* pIndex,bool32 bSystem,bool32 bCanRestore  )
{
	int32 i,j,k,iSize,iSize2,iSize3;
	iSize = pIndex->m_Group1.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		CIndexGroup1* pGroup1 = pIndex->m_Group1.GetAt(i);
		iSize2 = pGroup1->m_Group2.GetSize();
		for ( j = 0; j < iSize2; j ++ )
		{
			CIndexGroup2* pGroup2 = pGroup1->m_Group2.GetAt(j);
			iSize3 = pGroup2->m_Contents.GetSize();			
			for ( k = 0; k < iSize3; k ++ )
			{
				CFormularContent* pContent = pGroup2->m_Contents.GetAt(k);
				pContent->m_bSystem = bSystem;
				pContent->m_bCanRestore = bCanRestore;

				//加上是否保护
				if ( m_Flags.IsProtected(pContent->name))
				{
					pContent->m_bProtected = true;
				}
		 		//加上是否常用
				if ( m_Flags.IsOften(pContent->name))
		 		{
					pContent->m_bOften = true;
		 		}
			}
		}
	}
}
*/
///////////////////////////////////////////////////////////////

// void CFormulaLib::SetRefresh (CString StrName )
// {
//     for (POSITION pos = m_AllocContents.GetStartPosition(); pos != NULL; )
//     {
// 		int32 id;
//         CFormularContent* pContent = NULL;
//         m_AllocContents.GetNextAssoc(pos, id, (CFormularContent*&)pContent);
// 		if ( NULL == pContent )
// 		{
// 			break;
// 		}
// 		if ( 0 == StrName.GetLength() )
// 		{
// 			pContent->m_bNeedRefresh = true;
// 		}
// 		else
// 		{
// 			if ( pContent->name == StrName )
// 			{
// 				pContent->m_bNeedRefresh = true;
// 			}
// 		}
// 	}
// }

bool32 CFormulaLib::BeIndexShow(const CString& StrName)
{
	// 是否在界面上显示出来

	for ( int32 iIndexShow = 0; iIndexShow < KiNumsIndexNotShow; iIndexShow++)
	{
		if ( 0 == StrName.CompareNoCase(g_KaStrIndexNotShow[iIndexShow]) )
		{
			return false;
		}
	}

	return true;
}

void CFormulaLib::GetVolIndexNames(CStringArray& Names)
{
	Names.RemoveAll();
		
	if ( NULL == m_SysIndex.m_pGroupNormal )
	{
		return;
	}

	//
	for ( int32 j = 0 ; j < m_SysIndex.m_pGroupNormal->m_Group2.GetSize(); j++)
	{
		// 找到成交量型指标这个小分组
		if ( m_SysIndex.m_pGroupNormal->m_Group2[j]->m_StrName == L"成交量型")
		{
			// 取出这个分组所有指标的名字
			for ( int32 k = 0 ; k < m_SysIndex.m_pGroupNormal->m_Group2[j]->m_Contents.GetSize(); k++)
			{
				CString StrFormularName = m_SysIndex.m_pGroupNormal->m_Group2[j]->m_Contents[k]->name;
				Names.Add(StrFormularName);
			}

			return;
		}
	}	
}

void CFormulaLib::GetJinDunSpecialIndexNames( CStringArray& Names )
{
	Names.RemoveAll();
	
	if ( NULL == m_SysIndex.m_pGroupNormal )
	{
		return;
	}
	
	//
	for ( int32 j = 0 ; j < m_SysIndex.m_pGroupNormal->m_Group2.GetSize(); j++)
	{
		// 找到成交量型指标这个小分组
		if ( m_SysIndex.m_pGroupNormal->m_Group2[j]->m_StrName == L"金盾特色型")
		{
			// 取出这个分组所有指标的名字
			for ( int32 k = 0 ; k < m_SysIndex.m_pGroupNormal->m_Group2[j]->m_Contents.GetSize(); k++)
			{
				CString StrFormularName = m_SysIndex.m_pGroupNormal->m_Group2[j]->m_Contents[k]->name;

				if ( BeIndexShow(StrFormularName) )
				{
					Names.Add(StrFormularName);
				}
			}
			
			return;
		}
	}	
}

void CFormulaLib::GetAllowNames ( uint32 iFlag, CStringArray& Names, E_IndexStockType eIST/* = EIST_None*/ )
{
	POSITION pos;
	CString StrName;
	T_IndexMapPtr* p;
    CFormularContent* pContent;
	bool32 bInterval;
	bool32 bRegion;
	uint32 flag1;
	uint32 flag2;
	bool32 bEISTSame  = true;
	//系统指标
    for ( pos = m_SysIndex.m_Names4Index.GetStartPosition(); pos != NULL; )
    {
		p = NULL;
        m_SysIndex.m_Names4Index.GetNextAssoc(pos, StrName, (void*&)p);
		if ( NULL == p )
		{
			break;
		}
		
		pContent = (CFormularContent*)p->p1;
	
		if ( NULL == pContent )
		{
			continue;
		}

		if ( EFTNormal != pContent->m_eFormularType )
		{
			continue;
		}

		bRegion = false;
		if ( CheckFlag(iFlag,CFormularContent::KAllowMain) == CheckFlag(pContent->flag,CFormularContent::KAllowMain))
		{
			bRegion = true;
		}
		
		flag1 = pContent->flag & CFormularContent::KIntervalMask;
		flag2 = iFlag & CFormularContent::KIntervalMask;
		
		bInterval = false;
		
		if ( flag1 & flag2 )
		{
			bInterval = true;
		}

		bEISTSame = pContent->IsIndexStockTypeMatched(eIST);
		
		//
		bool32 bShow = BeIndexShow(pContent->name);	// 是否在界面上显示出来
		//
		// 修改指标的查找优先级高于系统指标，so应当先从修改中查找

		if ( bShow )
		{
			T_IndexMapPtr* tmp = m_ModifyIndex.Get(pContent->name);
			if ( NULL != tmp )
			{
				// 这个指标修改过，则不用查看原来的了
				pContent = (CFormularContent*)p->p1;
				bRegion = false;
				if ( CheckFlag(iFlag,CFormularContent::KAllowMain) == CheckFlag(pContent->flag,CFormularContent::KAllowMain))
				{
					bRegion = true;
				}
				flag1 = pContent->flag & CFormularContent::KIntervalMask;
				flag2 = iFlag & CFormularContent::KIntervalMask;
				bInterval = false;
				if ( flag1 & flag2 )
				{
					bInterval = true;
				}

				bEISTSame =  pContent->IsIndexStockTypeMatched(eIST);
				
				if ( pContent->m_bOften && bRegion && bInterval && bEISTSame )
				{
					Names.Add(pContent->name);
				}
			}
			else if ( pContent->m_bOften && bRegion && bInterval && bEISTSame )
			{
				// 没有修改过，则使用系统默认的
				Names.Add(pContent->name);
			}
			
		}
	}

	//用户自编
    for ( pos = m_UserIndex.m_Names4Index.GetStartPosition(); pos != NULL; )
    {
		p = NULL;
        m_UserIndex.m_Names4Index.GetNextAssoc(pos, StrName, (void*&)p);

		if ( NULL == p )
		{
			break;
		}
		pContent = (CFormularContent*)p->p1;

		if ( NULL == pContent )
		{
			continue;
		}
		
		if ( EFTNormal != pContent->m_eFormularType )
		{
			continue;
		}

		bRegion = false;
		if ( CheckFlag(iFlag,CFormularContent::KAllowMain) == CheckFlag(pContent->flag,CFormularContent::KAllowMain))
		{
			bRegion = true;
		}
		flag1 = pContent->flag & CFormularContent::KIntervalMask;
		flag2 = iFlag & CFormularContent::KIntervalMask;
		bInterval = false;
		if ( flag1 & flag2 )
		{
			bInterval = true;
		}

		bEISTSame =  pContent->IsIndexStockTypeMatched(eIST);
		
		//
		bool32 bShow = BeIndexShow(pContent->name);	// 是否在界面上显示出来
		
		if ( bShow )
		{
			if ( pContent->m_bOften && bRegion && bInterval && bEISTSame )
			{
				Names.Add(pContent->name);
			}
		}		
	}
}

CFormularContent* CFormulaLib::GetFomular(const CString& StrName)
{
	T_IndexMapPtr* pIndexMap = m_ModifyIndex.Get(StrName);
	
	if ( NULL != pIndexMap && NULL != pIndexMap->p1 )
	{
		return (CFormularContent*)pIndexMap->p1;
	}

	pIndexMap = m_SysIndex.Get(StrName);

	if ( NULL != pIndexMap && NULL != pIndexMap->p1 )
	{
		return (CFormularContent*)pIndexMap->p1;
	}
		
	pIndexMap = m_UserIndex.Get(StrName);

	if ( NULL != pIndexMap && NULL != pIndexMap->p1 )
	{
		return (CFormularContent*)pIndexMap->p1;
	}

	return NULL;
}

void CFormulaLib::GetFomulars(IN E_FormularType eType, bool32 bNeedUsers, OUT CArray<CFormularContent*, CFormularContent*>& aFomulars)
{
	aFomulars.RemoveAll();

	//
	if ( EFTNormal == eType )
	{
		// 系统的
		if ( NULL != m_SysIndex.m_pGroupNormal )
		{
			for ( int32 i = 0; i < m_SysIndex.m_pGroupNormal->m_Group2.GetSize(); i++ )
			{
				CIndexGroup2* pGroup2 = m_SysIndex.m_pGroupNormal->m_Group2.GetAt(i);
				if ( NULL == pGroup2 )
				{
					continue;
				}
				
				aFomulars.Append(pGroup2->m_Contents);			
			}
		}
		
		if ( bNeedUsers )
		{
			// 用户的
			if ( NULL != m_UserIndex.m_pGroupNormal )
			{
				for ( int32 i = 0; i < m_UserIndex.m_pGroupNormal->m_Group2.GetSize(); i++ )
				{
					CIndexGroup2* pGroup2 = m_UserIndex.m_pGroupNormal->m_Group2.GetAt(i);
					if ( NULL == pGroup2 )
					{
						continue;
					}
					
					aFomulars.Append(pGroup2->m_Contents);			
				}
			}
		}		
	}
	else if ( EFTCdtChose == eType )
	{
		// 系统的
		if ( NULL != m_SysIndex.m_pGroupCdtChose )
		{
			for ( int32 i = 0; i < m_SysIndex.m_pGroupCdtChose->m_Group2.GetSize(); i++ )
			{
				CIndexGroup2* pGroup2 = m_SysIndex.m_pGroupCdtChose->m_Group2.GetAt(i);
				if ( NULL == pGroup2 )
				{
					continue;
				}
			
				aFomulars.Append(pGroup2->m_Contents);			
			}
		}

		if ( bNeedUsers )
		{
			// 用户的
			if ( NULL != m_UserIndex.m_pGroupCdtChose )
			{
				for ( int32 i = 0; i < m_UserIndex.m_pGroupCdtChose->m_Group2.GetSize(); i++ )
				{
					CIndexGroup2* pGroup2 = m_UserIndex.m_pGroupCdtChose->m_Group2.GetAt(i);
					if ( NULL == pGroup2 )
					{
						continue;
					}
					
					aFomulars.Append(pGroup2->m_Contents);			
				}
			}
		}		
	}
	else if ( EFTTradeChose == eType )
	{
		// 系统的
		if ( NULL != m_SysIndex.m_pGroupTradeChose )
		{			
			aFomulars.Copy(m_SysIndex.m_pGroupTradeChose->m_ContentsTrade);			
		}
		
		if ( bNeedUsers )
		{
			// 用户的
			if ( NULL != m_UserIndex.m_pGroupTradeChose )
			{			
				aFomulars.Copy(m_UserIndex.m_pGroupTradeChose->m_ContentsTrade);			
			}
		}
	}
	else if ( EFTClrKLine == eType )
	{
		// 系统的
		if ( NULL != m_SysIndex.m_pGroupClrKLine )
		{			
			aFomulars.Copy(m_SysIndex.m_pGroupClrKLine->m_ContentsClrKLine);			
		}
		
		if ( bNeedUsers )
		{
			// 用户的
			if ( NULL != m_UserIndex.m_pGroupClrKLine)
			{			
				aFomulars.Copy(m_UserIndex.m_pGroupClrKLine->m_ContentsClrKLine);			
			}
		}
	}
}

bool32 CFormulaLib::BeSpecialSysIndex(CFormularContent* pFormular)
{
	if ( NULL == pFormular )
	{
		return false;
	}

	//
	for ( int32 i = 0; i < KiNumsIndexSpecialShow; i++ )
	{
		if ( 0 == pFormular->name.CompareNoCase(g_KaStrIndexSpecialShow[i]) )
		{
			return true;
		}
	}

	return false;
}

CString CFormulaLib::GetSpecialSysIndexGroupName(CFormularContent* pFormular)
{
	// 这里与常量数组内容按序号的一一对应, 偷个懒, 不定义常量结构数组了

	if ( NULL == pFormular )
	{
		return L"";
	}

	//
	if ( !BeSpecialSysIndex(pFormular) )
	{
		return L"";
	}

	//
	for ( int32 i = 0; i < KiNumsIndexSpecialShow; i++ )
	{
		if ( 0 == pFormular->name.CompareNoCase(g_KaStrIndexSpecialShow[i]) )
		{
			if ( 0 == i )
			{
				return L"";
			}
			else if ( 1 == i )
			{
				return L"VOLUME";
			}
			else
			{
				////ASSERT(0);
				return L"";
			}
		}		
	}

	return L"";
}

void CFormulaLib::SetCheckCode(CString strCheckCode)
{
	g_strCheckCode = strCheckCode;
}

const BASEINFO* CFormulaLib::GetBaseInfo() 
{
	const BASEINFO* pBaseInfo = (const BASEINFO*)&m_BaseInfo;
	return pBaseInfo;
}

CFormularContent* CFormulaLib::AllocContent ( int32 id, CString StrName )
{
	T_IndexMapPtr* p;
	CFormularContent* pContent = NULL;
	CIndexParamData* pParam = NULL;
	//找到合适的指标和参数
	//用户自编
	p = m_UserIndex.Get(StrName);
	if ( NULL != p )
	{
		pContent = (CFormularContent*)p->p1;
		// pParam = m_UserParams.GetParam(StrName);
	}
	else
	{
		//用户之前修改过的系统指标
		p = m_ModifyIndex.Get(StrName);
		if ( NULL != p )
		{
			pContent = (CFormularContent*)p->p1;
			pContent->m_bSystem = true;
			pContent->m_bCanRestore = true;
			// pParam = m_ModifyParams.GetParam(StrName);
		}
		else
		{
			p = m_SysIndex.Get(StrName);
			if ( NULL != p )
			{
				pContent = (CFormularContent*)p->p1;
				pContent->m_bSystem = true;
				// pParam = m_ModifyParams.GetParam(StrName);
			}
		}
	}
	if ( NULL == pContent )
	{
		return NULL;
	}

	CFormularContent* pAlloc = new CFormularContent;
	m_AllocContents[id] = pAlloc;

	pAlloc->bNew = pContent->bNew;
	pAlloc->last_time = pContent->last_time;

	pAlloc->name = pContent->name;
	pAlloc->password = pContent->password;

	pAlloc->explainBrief = pContent->explainBrief;
	pAlloc->hotkey = pContent->hotkey;
	pAlloc->flag = pContent->flag;
	pAlloc->formular = pContent->formular;
	pAlloc->help = pContent->help;

	pAlloc->m_bProtected = pContent->m_bProtected;
	pAlloc->m_bOften = pContent->m_bOften;
	pAlloc->m_bSystem = pContent->m_bSystem;
	pAlloc->m_bCanRestore = pContent->m_bCanRestore;
//	pAlloc->m_bNeedRefresh = pContent->m_bNeedRefresh;

	int32 i, iSize;
	iSize = pContent->defaultValArray.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		pAlloc->defaultValArray.Add(pContent->defaultValArray.GetAt(i));
	}
	
	//
	pAlloc->numLine = pContent->numLine;
	for ( i = 0; i < pContent->numLine; i ++ )
	{
		pAlloc->line[i] = pContent->line[i];
	}
	
	//
	pAlloc->numExtraY = pContent->numExtraY;
	for ( i = 0; i < pAlloc->numExtraY; i++ )
	{
		pAlloc->lineExtraY[i] = pContent->lineExtraY[i];
	}

	//
	pAlloc->numPara = pContent->numPara;
	for ( i = 0; i < pContent->numPara; i ++ )
	{
		pAlloc->namePara[i] = pContent->namePara[i];
		pAlloc->descPara[i] = pContent->descPara[i];
		pAlloc->max[i] = pContent->max[i];
		pAlloc->min[i] = pContent->min[i];
		pAlloc->defaultVal[i] = pContent->defaultVal[i];
		pAlloc->stepLen[i] = pContent->stepLen[i];

	}
	if ( NULL != pParam )
	{
		IndexParamToContent(pParam,pAlloc);
	}
	
	return pAlloc;
}
void CFormulaLib::RemoveAlloc ( int32 id )
{
	CFormularContent* pContent;
	if ( m_AllocContents.Lookup(id,(CFormularContent*&)pContent) && NULL != pContent )
	{
		m_AllocContents.RemoveKey(id);
		delete pContent;
	}
}

void CFormulaLib::BuildHotKeyList(CArray<CHotKey, CHotKey&>&	arrHotKeys)
{
	RemoveHotKey(EHKTIndex, arrHotKeys);

	POSITION pos = m_SysIndex.m_Names4Index.GetStartPosition();
	while ( NULL != pos )
	{
		CString StrName;
		T_IndexMapPtr* p = NULL;
		m_SysIndex.m_Names4Index.GetNextAssoc(pos, StrName, (void *&)p);

		if ( !BeIndexShow(StrName) )
		{
			// 不应当掉出来的指标
			continue;
		}

		CHotKey hotKey;
		hotKey.m_eHotKeyType = EHKTIndex;
		if ( NULL != p )
		{
			CFormularContent *pContent = (CFormularContent*)p->p1;
			if ( NULL != pContent 
				//&& !CheckFlag(pContent->flag, CFormularContent::KAllowTick)
				//&& !CheckFlag(pContent->flag, CFormularContent::KAllowTrend)
				)
			{					
				hotKey.m_StrKey		= ConvertHZToPY(pContent->name);							
				hotKey.m_StrSummary = pContent->name;
				hotKey.m_StrParam1	= pContent->name;
				hotKey.m_iParam1	= pContent->m_eFormularType;
				hotKey.m_iParam2	= pContent->flag;
				arrHotKeys.Add(hotKey);
			}
			else if ( NULL != pContent )
			{
				TRACE(_T("建立指标快捷键，忽略: %s\r\n"), pContent->name.GetBuffer());
			}
		}
	}

	// 修改系统指标中只有修改过的系统指标，so系统指标已经包括了
// 		pos = m_ModifyIndex.m_Names4Index.GetStartPosition();
// 		while ( NULL != pos )
// 		{
// 			CString StrName;
// 			T_IndexMapPtr* p = NULL;
// 			m_ModifyIndex.m_Names4Index.GetNextAssoc(pos, StrName, (void *&)p);
// 			
// 			CHotKey hotKey;
// 			hotKey.m_eHotKeyType = EHKTIndex;
// 			if ( NULL != p )
// 			{
// 				CFormularContent *pContent = (CFormularContent*)p->p1;
// 				if ( NULL != pContent )
// 				{
// 					//hotKey.m_StrKey = pContent->hotkey;
// 					//hotKey.m_StrSummary = pContent->name;
// 					hotKey.m_StrKey		= ConvertHZToPY(pContent->name);
// 					hotKey.m_StrSummary = pContent->explainBrief;
// 					hotKey.m_StrParam1	= pContent->name;
// 					hotKey.m_iParam1	= pContent->m_eFormularType;
// 					hotKey.m_iParam2	= pContent->flag;
// 					pViewData->AddHotKey(hotKey);
// 				}
// 			}
// 		}

	pos = m_UserIndex.m_Names4Index.GetStartPosition();
	while ( NULL != pos )
	{
		CString StrName;
		T_IndexMapPtr* p = NULL;
		m_UserIndex.m_Names4Index.GetNextAssoc(pos, StrName, (void *&)p);
		
		CHotKey hotKey;
		hotKey.m_eHotKeyType = EHKTIndex;
		if ( NULL != p )
		{
			CFormularContent *pContent = (CFormularContent*)p->p1;
			if ( NULL != pContent
				//&& !CheckFlag(pContent->flag, CFormularContent::KAllowTick)
				//&& !CheckFlag(pContent->flag, CFormularContent::KAllowTrend)
				)
			{
				hotKey.m_StrKey		= ConvertHZToPY(pContent->name);
				hotKey.m_StrSummary = pContent->name;
				hotKey.m_StrParam1	= pContent->name;
				hotKey.m_iParam1	= pContent->m_eFormularType;
				hotKey.m_iParam2	= pContent->flag;
				arrHotKeys.Add(hotKey);
			}
			else if ( NULL != pContent )
			{
				TRACE(_T("建立指标快捷键，忽略: %s\r\n"), pContent->name.GetBuffer());
			}
		}
	}
}


void CFormulaLib::RemoveHotKey(E_HotKeyType eHotKeyType, CArray<CHotKey, CHotKey&>&	arrHotKeys)
{
	for (int32 i = arrHotKeys.GetSize() - 1; i >= 0; i--)
	{
		if (arrHotKeys[i].m_eHotKeyType == eHotKeyType)
			arrHotKeys.RemoveAt(i, 1);
	}
}



void CFormulaLib::DelInstance()
{
	delete sFormulaLib;
	sFormulaLib = NULL;
}

//////////////////////////////////////////////////////////////////////////