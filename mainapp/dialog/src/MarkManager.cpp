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
const TCHAR KStrFileName[] = _T("marks.xml");		// xml�ļ���

const char KStrXmlEleMark[] = "mark";	// mark�ڵ�
const char KStrXmlAttriMerchId[]	= "merchId";			// ��Ʒid
const char KStrXmlAttriMarkType[]	= "markType";			// �������
const char KStrXmlAttriMarkTip[]	= "markTip";			// �����ʾ, ����EMT_Text��Ч

// ��Ʒ������id�ִ���ת��
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

	ASSERT( m_aListeners.GetSize() == 0 );	// ��ʱӦ����û�м���

	USES_CONVERSION;
	m_bInitedMerchMap = true;
	m_mapMarks.RemoveAll();
	m_mapIdMarks.RemoveAll();
	m_TiDoc.Clear();	// ���ԭ��������
	m_TiDoc.SetValue(_Unicode2MultiChar(StrPath).c_str());
	if ( !m_TiDoc.LoadFile() )
	{
		// ����һ��Ĭ�ϵ�
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
		// ��Ʒ
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
			continue;	// ��Чid
		}

		// ����Ƿ���ڸ�id������һ��id��Ч
		if ( m_mapIdMarks.Lookup(StrMerchId, MarkData) )
		{
			continue;	// ����������
		}

		pszAttri = pEle->Attribute(KStrXmlAttriMarkType);
		if ( NULL == pszAttri )
		{
			continue;	// ������
		}
		int32 iType = atoi(pszAttri);
		if ( iType < EMT_Text && iType >= EMT_Count )
		{
			continue;	// �޷�ʶ������
		}

		if ( iType == EMT_Text )
		{
			MarkData.m_eType = (E_MarkType)iType;
			pszAttri = pEle->Attribute(KStrXmlAttriMarkTip);
			if ( NULL == pszAttri || pszAttri[0] == '\0' )
			{
				continue;	// ���ֱ�������ʾ
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

	m_bInitedMerchMap = m_mapIdMarks.GetCount() <= 0;	// ��������û�оͲ���Ҫ�ڳ�ʼ��������Ʒ�ı����

	return true;
}

bool32 CMarkManager::RebuildMerchMark()
{
	m_mapMarks.RemoveAll();

	// ��ȡ��Ʒ����
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
		// ������ǰ�ı��
		if ( MarkData == MarkTmp )
		{
			return true;	// �ޱ仯
		}
	}

	if ( MarkData.m_eType >= EMT_Count )	
	{
		return false;
	}
	else if ( EMT_Text != MarkData.m_eType
		|| !MarkData.m_StrTip.IsEmpty() )
	{
		// ����ֵ
		m_mapIdMarks[StrId] = MarkData;
		UpdateXmlValue(StrId, MarkData);

		UpdateMerchMap(iMarketId, StrMerchCode, MarkData);	// ���´�����Ʒ�ı��

		// ֪ͨ
		FireChange(iMarketId, StrMerchCode, MarkTmp);

		return true;
	}

	// �����ֵ
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
			// ���´˽ڵ�
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

			break;	// �����µ�һ��
		}
	}

	if ( !bUpdated )
	{
		// ��Ӵ˽ڵ�
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
			// ɾ���˽ڵ�
			pRoot->RemoveChild(pEle);
			break;	// ��ɾ����һ��
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
		return true;	// û�г�ʼ����������Ʒ�ı������Ҫ���£��Ժ����ʱ��ȫ������
	}

	if ( MarkData.m_eType == EMT_Count )
	{
		// ɾ��
		m_mapMarks.RemoveKey(pMerch);
		return true;
	}
	else if ( MarkData.m_eType != EMT_Text || !MarkData.m_StrTip.IsEmpty() )
	{
		// ����
		m_mapMarks[pMerch] = MarkData;
		return true;
	}

	return false;
}

bool32 CMarkManager::UpdateMerchMap( int32 iMarketId, const CString &StrMerchCode, const T_MarkData &MarkData )
{
	// ��ȡ��Ʒ����
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

	// ɾ���ñ��
	m_mapIdMarks.RemoveKey(StrId);
	
	// ɾ��xml
	DeleteXmlValue(StrId);
	
	UpdateMerchMap(iMarketId, StrMerchCode, T_MarkData(EMT_Count));	// ���´�����Ʒ�ı��

	// ֪ͨ
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
			m_aListeners.RemoveAt(i);	// ��ֻ֤��һ��
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
	// �Զ����ע��
	CMarkManager::Instance().RemoveMerchMarkChangeListener(this);
}
