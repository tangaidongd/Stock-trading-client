#include "StdAfx.h"
#include "XmlShare.h"
#include "PathFactory.h"
#include <atlconv.h>
#include "MarkManager.h"
#include "coding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
const TCHAR KStrFileName[] = _T("marks.xml");		// xml文件名

const char KStrXmlEleMark[] = "mark";	// mark节点
const char KStrXmlAttriMerchId[]	= "merchId";			// 商品id
const char KStrXmlAttriMarkType[]	= "markType";			// 标记类型
const char KStrXmlAttriMarkTip[]	= "markTip";			// 标记提示, 仅对EMT_Text有效

// 商品代码与id字串互转换
static CString MerchKeyToIdString(int32 iMarketId, const CString &StrMerchCode)
{
	CString StrCode = StrMerchCode;
	CString Str;
	Str.Format(_T("%08d%s"), iMarketId, StrCode.GetBuffer());
	return Str;
}

static bool32 IdStringToMerchKey(const CString &StrId, OUT int32 &iMarketId, OUT CString &StrMerchCode)
{
	//Size of argument no. 4 inconsistent with format
	//lint --e(559)
	int iRet = _stscanf(StrId, _T("%08d%s"), &iMarketId, StrMerchCode.GetBuffer(200));
	StrMerchCode.ReleaseBuffer();
	return iRet == 2;
}

//////////////////////////////////////////////////////////////////////////
//

CMarkManager::CMarkManager()
{
	m_bInitedMerchMap = false;
}

bool32 CMarkManager::LoadFromXml()
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	CString StrPath = CPathFactory::GetPrivateConfigPath(pDoc->m_pAbsCenterManager->GetUserName());
	StrPath += KStrFileName;

	ASSERT( m_aListeners.GetSize() == 0 );	// 此时应当还没有监听

	USES_CONVERSION;
	m_bInitedMerchMap = true;
	m_mapMarks.RemoveAll();
	m_mapIdMarks.RemoveAll();
	m_TiDoc.Clear();	// 清除原来的内容
	m_TiDoc.SetValue(_Unicode2MultiChar(StrPath).c_str());
	if ( !m_TiDoc.LoadFile() )
	{
		// 创建一个默认的
		TiXmlElement *pRoot = ConstructGGTongAppXmlDocHeader(m_TiDoc, "markData", NULL, NULL, NULL);
		return pRoot != NULL;
	}

	TiXmlElement *pRoot = m_TiDoc.RootElement();
	if ( NULL == pRoot )
	{
		return false;
	}

	for ( TiXmlElement *pEle = pRoot->FirstChildElement(KStrXmlEleMark)
		; NULL != pEle 
		; pEle = pEle->NextSiblingElement(KStrXmlEleMark) )
	{
		// 商品
		int32 iMarketId;
		CString StrMerchCode;
		T_MarkData MarkData;
		const char *pszAttri = pEle->Attribute(KStrXmlAttriMerchId);
		if ( NULL == pszAttri )
		{
			continue;
		}
		wchar_t *pwszMerchId = CEtcXmlConfig::MultiToWide(pszAttri, CP_UTF8);
		CString StrMerchId = pwszMerchId;
		delete []pwszMerchId;
		pwszMerchId = NULL;
		if ( !IdStringToMerchKey(StrMerchId, iMarketId, StrMerchCode) )
		{
			continue;	// 无效id
		}

		// 检查是否存在该id，仅第一个id生效
		if ( m_mapIdMarks.Lookup(StrMerchId, MarkData) )
		{
			continue;	// 后续的跳过
		}

		pszAttri = pEle->Attribute(KStrXmlAttriMarkType);
		if ( NULL == pszAttri )
		{
			continue;	// 不完整
		}
		int32 iType = atoi(pszAttri);
		if ( iType < EMT_Text && iType >= EMT_Count )
		{
			continue;	// 无法识别类型
		}

		if ( iType == EMT_Text )
		{
			MarkData.m_eType = (E_MarkType)iType;
			pszAttri = pEle->Attribute(KStrXmlAttriMarkTip);
			if ( NULL == pszAttri || pszAttri[0] == '\0' )
			{
				continue;	// 文字必须有提示
			}
			wchar_t *pwszTip = CEtcXmlConfig::MultiToWide(pszAttri, CP_UTF8);
			MarkData.m_StrTip = pwszTip;
			delete []pwszTip;
		}
		else
		{
			MarkData = T_MarkData((E_MarkType)iType);	// 
		}

		m_mapIdMarks[StrMerchId] = MarkData;
	}

	m_bInitedMerchMap = m_mapIdMarks.GetCount() <= 0;	// 如果本身就没有就不需要在初始化存在商品的标记了

	return true;
}

bool32 CMarkManager::RebuildMerchMark()
{
	m_mapMarks.RemoveAll();

	// 获取商品管理
	CGGTongDoc *pDoc = AfxGetDocument();
	CMerchManager *pManager = NULL;
	if ( NULL != pDoc && NULL != pDoc->m_pAbsCenterManager )
	{
		pManager = &pDoc->m_pAbsCenterManager->GetMerchManager();
	}
	if ( NULL == pManager )
	{
		return false;
	}

	POSITION pos = m_mapIdMarks.GetStartPosition();
	CString StrId;
	int32 iMarketId;
	CString StrMerchCode;
	T_MarkData MarkData;
	CMerch *pMerch;
	while (NULL != pos)
	{
		m_mapIdMarks.GetNextAssoc(pos, StrId, MarkData);
		if ( IdStringToMerchKey(StrId, iMarketId, StrMerchCode) )
		{
			if ( pManager->FindMerch(StrMerchCode, iMarketId, pMerch) )
			{
				m_mapMarks[pMerch] = MarkData;
			}
		}
	}

	m_bInitedMerchMap = true;
	return true;
}

bool32 CMarkManager::SaveToXml()
{
	return m_TiDoc.SaveFile();
}

const CMarkManager::MarkMap & CMarkManager::GetMarkMap()
{
	if ( !m_bInitedMerchMap )
	{
		bool32 b = RebuildMerchMark();
		ASSERT( b && m_bInitedMerchMap );
	}
	return m_mapMarks;
}

bool32 CMarkManager::SetMark( int32 iMarketId, const CString &StrMerchCode, const T_MarkData &MarkData )
{
	CString StrId = MerchKeyToIdString(iMarketId, StrMerchCode);
	T_MarkData MarkTmp;
	if ( m_mapIdMarks.Lookup(StrId, MarkTmp) )
	{
		// 存在以前的标记
		if ( MarkData == MarkTmp )
		{
			return true;	// 无变化
		}
	}

	if ( MarkData.m_eType >= EMT_Count )	
	{
		return false;
	}
	else if ( EMT_Text != MarkData.m_eType
		|| !MarkData.m_StrTip.IsEmpty() )
	{
		// 更新值
		m_mapIdMarks[StrId] = MarkData;
		UpdateXmlValue(StrId, MarkData);

		UpdateMerchMap(iMarketId, StrMerchCode, MarkData);	// 更新存在商品的标记

		// 通知
		FireChange(iMarketId, StrMerchCode, MarkTmp);

		return true;
	}

	// 错误的值
	return false;
}

bool32 CMarkManager::SetMark( int32 iMarketId, const CString &StrMerchCode, E_MarkType eType )
{
	return SetMark(iMarketId, StrMerchCode, T_MarkData(eType));
}

bool32 CMarkManager::SetMark( CMerch *pMerch, const T_MarkData &MarkData )
{
	if ( NULL != pMerch )
	{
		return SetMark(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, MarkData);
	}
	return false;
}

bool32 CMarkManager::SetMark( CMerch *pMerch, E_MarkType eType )
{
	if ( NULL != pMerch )
	{
		return SetMark(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, eType);
	}
	return false;
}

bool32 CMarkManager::UpdateXmlValue( const CString &StrId, const T_MarkData &MarkData, bool32 bSave /*= true*/ )
{
	if ( StrId.IsEmpty() )
	{
		return false;
	}

	ASSERT( MarkData.m_eType != EMT_Count || !MarkData.m_StrTip.IsEmpty() );

	TiXmlElement *pRoot = m_TiDoc.RootElement();
	if ( NULL == pRoot )
	{
		return false;
	}

	bool32 bUpdated = false;
	char *pszId = CEtcXmlConfig::WideToMulti(StrId, CP_UTF8);
	if ( NULL == pszId )
	{
		return false;
	}

	for ( TiXmlElement *pEle = pRoot->FirstChildElement(KStrXmlEleMark)
		; pEle != NULL
		; pEle = pEle->NextSiblingElement(KStrXmlEleMark))
	{
		const char *pszAttri = pEle->Attribute(KStrXmlAttriMerchId);
		if ( NULL != pszAttri && strcmp(pszAttri, pszId) == 0 )
		{
			// 更新此节点
			char buf[100];

			// type
			_snprintf(buf, sizeof(buf), "%d", MarkData.m_eType);
			pEle->SetAttribute(KStrXmlAttriMarkType, buf);

			// tip
			if ( MarkData.m_eType != EMT_Text )
			{
				pEle->RemoveAttribute(KStrXmlAttriMarkTip);
			}
			else
			{
				char *pszTip = CEtcXmlConfig::WideToMulti(MarkData.m_StrTip, CP_UTF8);
				ASSERT( NULL != pszTip );
				pEle->SetAttribute(KStrXmlAttriMarkTip, pszTip);
				delete []pszTip;
			}
			bUpdated = true;

			break;	// 仅更新第一个
		}
	}

	if ( !bUpdated )
	{
		// 添加此节点
		TiXmlElement ele(KStrXmlEleMark);
		char buf[100];

		// id
		ele.SetAttribute(KStrXmlAttriMerchId, pszId);
		
		// type
		_snprintf(buf, sizeof(buf), "%d", MarkData.m_eType);
		ele.SetAttribute(KStrXmlAttriMarkType, buf);
		
		// tip
		if ( MarkData.m_eType == EMT_Text )
		{
			char *pszTip = CEtcXmlConfig::WideToMulti(MarkData.m_StrTip, CP_UTF8);
			ASSERT( NULL != pszTip );
			ele.SetAttribute(KStrXmlAttriMarkTip, pszTip);
			delete []pszTip;
		}
		pRoot->InsertEndChild(ele);
	}

	delete []pszId;
	pszId = NULL;

	if ( bSave )
	{
		SaveToXml();
	}

	return true;
}

void CMarkManager::DeleteXmlValue( const CString &StrId, bool32 bSave /*= true*/ )
{
	if ( StrId.IsEmpty() )
	{
		return;
	}

	TiXmlElement *pRoot = m_TiDoc.RootElement();
	if ( NULL == pRoot )
	{
		return;
	}
	
	char *pszId = CEtcXmlConfig::WideToMulti(StrId, CP_UTF8);
	if ( NULL == pszId )
	{
		return;
	}
	
	for ( TiXmlElement *pEle = pRoot->FirstChildElement(KStrXmlEleMark)
		; pEle != NULL
		; pEle = pEle->NextSiblingElement(KStrXmlEleMark))
	{
		const char *pszAttri = pEle->Attribute(KStrXmlAttriMerchId);
		if ( NULL != pszAttri && strcmp(pszAttri, pszId) == 0 )
		{
			// 删除此节点
			pRoot->RemoveChild(pEle);
			break;	// 仅删除第一个
		}
	}

	if ( bSave )
	{
		SaveToXml();
	}

	delete []pszId;
	pszId = NULL;
}

bool32 CMarkManager::UpdateMerchMap( CMerch *pMerch, const T_MarkData &MarkData )
{
	if ( NULL == pMerch )
	{
		return false;
	}

	if ( !m_bInitedMerchMap )
	{
		return true;	// 没有初始化过存在商品的标记则不需要更新，以后更新时会全部更新
	}

	if ( MarkData.m_eType == EMT_Count )
	{
		// 删除
		m_mapMarks.RemoveKey(pMerch);
		return true;
	}
	else if ( MarkData.m_eType != EMT_Text || !MarkData.m_StrTip.IsEmpty() )
	{
		// 更新
		m_mapMarks[pMerch] = MarkData;
		return true;
	}

	return false;
}

bool32 CMarkManager::UpdateMerchMap( int32 iMarketId, const CString &StrMerchCode, const T_MarkData &MarkData )
{
	// 获取商品管理
	CGGTongDoc *pDoc = AfxGetDocument();
	CMerchManager *pManager = NULL;
	CMerch *pMerch = NULL;
	if ( NULL != pDoc && NULL != pDoc->m_pAbsCenterManager )
	{
		pManager = &pDoc->m_pAbsCenterManager->GetMerchManager();
	}
	if ( NULL == pManager || !pManager->FindMerch(StrMerchCode, iMarketId, pMerch) )
	{
		return false;
	}

	return UpdateMerchMap(pMerch, MarkData);
}

void CMarkManager::RemoveMark( int32 iMarketId, const CString &StrMerchCode )
{
	CString StrId = MerchKeyToIdString(iMarketId, StrMerchCode);

	T_MarkData MarkTmp;
	BOOL bExist = m_mapIdMarks.Lookup(StrId, MarkTmp);

	// 删除该标记
	m_mapIdMarks.RemoveKey(StrId);
	
	// 删除xml
	DeleteXmlValue(StrId);
	
	UpdateMerchMap(iMarketId, StrMerchCode, T_MarkData(EMT_Count));	// 更新存在商品的标记

	// 通知
	if ( bExist )
	{
		FireChange(iMarketId, StrMerchCode, MarkTmp);
	}
}

void CMarkManager::RemoveMark( CMerch *pMerch )
{
	if ( NULL != pMerch )
	{
		RemoveMark(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode);
	}
}

bool32 CMarkManager::QueryMark( int32 iMarketId, const CString &StrMerchCode, OUT T_MarkData &MarkData ) const
{
	CString StrId = MerchKeyToIdString(iMarketId, StrMerchCode);
	return m_mapIdMarks.Lookup(StrId, MarkData);
}

bool32 CMarkManager::QueryMark( const CMerchKey &MerchKey, OUT T_MarkData &MarkData ) const
{
	return QueryMark(MerchKey.m_iMarketId, MerchKey.m_StrMerchCode, MarkData);
}

bool32 CMarkManager::QueryMark( CMerch *pMerch, OUT T_MarkData &MarkData ) const
{
	if ( NULL != pMerch )
	{
		return QueryMark(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, MarkData);
	}
	return false;
}

CMarkManager & CMarkManager::Instance()
{
	static CMarkManager s_mark;
	static bool bInit = false;
	if ( !bInit )
	{
		bool32 b = s_mark.LoadFromXml();
		ASSERT( b );
		bInit = true;
	}
	return s_mark;
}

void CMarkManager::AddMerchMarkChangeListener( CMerchMarkChangeListener *pListener )
{
	if ( NULL == pListener )
	{
		return;
	}

	for ( int32 i=0; i < m_aListeners.GetSize() ; i++ )
	{
		if ( m_aListeners[i] == pListener )
		{
			return;
		}
	}
	m_aListeners.Add(pListener);
}

void CMarkManager::RemoveMerchMarkChangeListener( CMerchMarkChangeListener *pListener )
{
	if ( NULL == pListener )
	{
		return;
	}

	for ( int32 i=0; i < m_aListeners.GetSize() ; i++ )
	{
		if ( m_aListeners[i] == pListener )
		{
			m_aListeners.RemoveAt(i);	// 保证只有一个
			return;
		}
	}
}

void CMarkManager::FireChange( int32 iMarketId, const CString &StrMerchCode, const T_MarkData &MarkOldData )
{
	for ( int32 i=0; i < m_aListeners.GetSize() ; i++ )
	{
		if ( NULL != m_aListeners[i] )
		{
			m_aListeners[i]->OnMerchMarkChanged(iMarketId, StrMerchCode, MarkOldData);
		}
		else
		{
			ASSERT( 0 );
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//
CMerchMarkChangeListener::~CMerchMarkChangeListener()
{
	// 自动解除注册
	CMarkManager::Instance().RemoveMerchMarkChangeListener(this);
}
