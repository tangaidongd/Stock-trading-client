// ArbitrageManage.cpp: implementation of the CArbitrageManage class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "ArbitrageManage.h"
#include "tinyxml.h"
#include "PathFactory.h"
#include "coding.h"
#include "ArbitrageFormula.h"
#include "AlarmCenter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// �Ƿ��ĸ�����
static const float s_KfError = FLT_MAX;

// XML ���
static const char * KStrE_Arbitrage		= "Arbitrage";
static const char * KStrA_Arbitype		= "arbtype";
static const char * KStrA_Pricetype		= "pricetype";
static const char * KStrA_SaveDec		= "savedec";

static const char * KStrA_Alarm			= "alarm";
static const char * KStrA_Up			= "up";
static const char * KStrA_Down			= "down";

static const char * KStrE_Merchs		= "Merch";
static const char * KStrA_Id			= "id";
static const char * KStrA_Code			= "code";
static const char * KStrA_Formular		= "formular";

static const char * KStrE_OtherMerchs		= "OtherMerch";			// �����������Ʒ

//
extern bool32	ArbForOperatorAdd(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
extern bool32	ArbForOperatorSub(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
extern bool32	ArbForOperatorMulti(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
extern bool32	ArbForOperatorDiv(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
extern bool32	ArbForOperatorAssign(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
extern bool32	ArbForOperatorComma(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
extern bool32	ArbForOperatorNegative(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
extern bool32	ArbForOperatorPositive(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);

//////////////////////////////////////////////////////////////////////////
//
CArbitrage::CArbitrage()
{
	m_MerchA.m_pMerch	= NULL;
	m_MerchB.m_pMerch	= NULL;
	m_MerchC.m_pMerch	= NULL;

	m_MerchA.m_pFormular= NULL;
	m_MerchB.m_pFormular= NULL;
	m_MerchC.m_pFormular= NULL;

	m_eArbitrage= EABCount;
	m_eArbPrice	= EAPCount;

	m_iSaveDec	= 2;

	//
	m_bAlarm	= false;
	m_fUp		= 0.;
	m_fDown		= 0.;
}

CArbitrage::~CArbitrage()
{
}




CArbitrage::T_ArbMerch::T_ArbMerch( const T_ArbMerch &other )
{
	m_pMerch = NULL;
	m_pFormular = NULL;
	Copy(other);
}

const CArbitrage::T_ArbMerch & CArbitrage::T_ArbMerch::operator=( const T_ArbMerch &other )
{
	if (&other == this)
	{
		return *this;
	}
	Copy(other);
	return *this;
}

bool CArbitrage::T_ArbMerch::operator==( const T_ArbMerch &other ) const
{
	bool bEqu = true;

	bEqu = bEqu && m_pMerch==other.m_pMerch;
	if ( bEqu )
	{
		bEqu = m_pFormular==other.m_pFormular;
		if ( !bEqu
			&& NULL!=m_pFormular
			&& NULL!=other.m_pFormular )
		{
			bEqu = 0==m_pFormular->GetFormula().CompareNoCase( other.m_pFormular->GetFormula() );
		}
	}

	ASSERT( !bEqu || m_aOtherMerch.size() == other.m_aOtherMerch.size() );
	return bEqu;
}

void CArbitrage::T_ArbMerch::Copy( const T_ArbMerch &other )
{
	if ( this==&other )
	{
		return;
	}

	m_pMerch = other.m_pMerch;
	m_aOtherMerch = other.m_aOtherMerch;

	if ( NULL != other.m_pFormular )
	{
		if ( NULL == m_pFormular )
		{
			m_pFormular = new CArbitrageFormula();
		}
		
		*m_pFormular = *other.m_pFormular;		
	}
	else
	{
		DEL(m_pFormular);		
	}
}

bool CArbitrage::T_ArbMerch::operator<( const T_ArbMerch &other ) const
{
	int iCmp = m_pMerch->m_MerchInfo.m_StrMerchCode.CompareNoCase(other.m_pMerch->m_MerchInfo.m_StrMerchCode);
	if ( iCmp > 0 )
	{
		return false;
	}
	else if ( iCmp < 0 )
	{
		return true;
	}

	// >=
	if ( NULL==m_pFormular )
	{
		if ( NULL==other.m_pFormular )
		{
			return false;	//==
		}
	}
	else
	{
		if ( NULL==other.m_pFormular
			|| m_pFormular->GetFormula().CompareNoCase(other.m_pFormular->GetFormula()) >= 0 )
		{
			return false;
		}
	}
	
	return true;
}

CArbitrage::T_ArbMerch::~T_ArbMerch()
{
	DEL(m_pFormular);
}

//////////////////////////////////////////////////////////////////////////
//
bool CArbitrage::operator ==(const CArbitrage& stOther) const 
{
	// �п����û���������һ��һ����ʽ�ģ����������ڱ��۱����⴦������λ�ò���Ҫ
	if ( m_eArbitrage != stOther.m_eArbitrage )
	{
		return false;
	}
	
	if ( m_eArbPrice != stOther.m_eArbPrice )
	{
		return false;
	}
	
	if ( m_iSaveDec != stOther.m_iSaveDec )
	{
		return false;
	}

	if ( m_MerchA != stOther.m_MerchA )
	{
		return false;
	}
	
	if ( m_MerchB != stOther.m_MerchB )
	{
		return false;
	}
	
	if ( IsNeedCMerch() && m_MerchC != stOther.m_MerchC )
	{
		return false;
	}

	// �����ʽ��ȣ��������Ʒһ��һ����
	ASSERT( GetOtherMerchsCount()== stOther.GetOtherMerchsCount() );
	//
// 	if ( m_bAlarm != stOther.m_bAlarm )
// 	{
// 		return false;
// 	}
// 
// 	//
// 	if ( m_fUp != stOther.m_fUp )
// 	{
// 		return false;
// 	}
// 
// 	//
// 	if ( m_fDown != stOther.m_fDown )
// 	{
// 		return false;
// 	}

	return true;
}

bool CArbitrage::operator <(const CArbitrage& stOther) const
{
	if ( m_eArbitrage < stOther.m_eArbitrage )
	{
		return true;
	}
	else if ( stOther.m_eArbitrage < m_eArbitrage )
	{
		return false;
	}

	if ( m_eArbPrice < stOther.m_eArbPrice )
	{
		return true;
	}
	else if ( stOther.m_eArbPrice < m_eArbPrice )
	{
		return false;
	}

	if ( m_iSaveDec < stOther.m_iSaveDec )
	{
		return true;
	}
	else if ( stOther.m_iSaveDec < m_iSaveDec )
	{
		return false;
	}

	if ( m_MerchA < stOther.m_MerchA )
	{
		return true;
	}
	else if ( stOther.m_MerchA < m_MerchA )
	{
		return false;
	}

	if ( m_MerchB < stOther.m_MerchB )
	{
		return true;
	}
	else if ( stOther.m_MerchB < m_MerchB )
	{
		return false;
	}

	if ( IsNeedCMerch() 
		&& stOther.IsNeedCMerch() )
	{
		if ( m_MerchC < stOther.m_MerchC )
		{
			return true;
		}
		else if ( stOther.m_MerchC < m_MerchC )
		{
			return false;
		}
	}
	else if ( !IsNeedCMerch()
		&& stOther.IsNeedCMerch() )
	{
		return true;
	}
	else if ( IsNeedCMerch()
		&& !stOther.IsNeedCMerch() )
	{
		return false;
	}

	// true��>=
	// false <=
	if ( (m_bAlarm&&!stOther.m_bAlarm) )
	{
		return false;
	}
	else if ( !m_bAlarm&&stOther.m_bAlarm )
	{
		return true;
	}

	if ( m_fUp < stOther.m_fUp )
	{
		return true;
	}
	else if ( stOther.m_fUp < m_fUp )
	{
		return false;
	}

	if ( m_fDown < stOther.m_fDown )
	{
		return true;
	}
	else if ( stOther.m_fDown < m_fDown )
	{
		return false;
	}

	// ==
	return false;
}

CArbitrage& CArbitrage::operator=(const CArbitrage& stOther)
{
	if ( this == &stOther )
	{
		return *this;
	}

	m_MerchA = stOther.m_MerchA;
	m_MerchB = stOther.m_MerchB;
	m_MerchC = stOther.m_MerchC;

	//
	m_eArbitrage = stOther.m_eArbitrage;
	m_eArbPrice	 = stOther.m_eArbPrice;
	m_iSaveDec	 = stOther.m_iSaveDec;

	//
	m_bAlarm	 = stOther.m_bAlarm;
	m_fUp		 = stOther.m_fUp;
	m_fDown		 = stOther.m_fDown;

	//
	return *this;
}

CArbitrage::CArbitrage(const CArbitrage& stOther)
{
	//
	m_MerchA = stOther.m_MerchA;
	m_MerchB = stOther.m_MerchB;
	m_MerchC = stOther.m_MerchC;
	
	
	m_eArbitrage = stOther.m_eArbitrage;
	m_eArbPrice	 = stOther.m_eArbPrice;
	m_iSaveDec	 = stOther.m_iSaveDec;

	//
	m_bAlarm	 = stOther.m_bAlarm;
	m_fUp		 = stOther.m_fUp;
	m_fDown		 = stOther.m_fDown;
}

bool32 CArbitrage::FromXml(TiXmlElement* pElement,CAbsCenterManager* pData)
{
	if ( NULL == pElement )
	{
		return false;
	}

	//
	if ( 0 != strcmp(pElement->Value(), KStrE_Arbitrage) )
	{
		////ASSERT(0);
		return false;
	}

	// ��������
	const char* pStrArrtibute = pElement->Attribute(KStrA_Arbitype);
	if ( NULL == pStrArrtibute )
	{
		////ASSERT(0);
		return false;
	}

	m_eArbitrage = (E_Arbitrage)atoi(pStrArrtibute);

	// �۲�����
	pStrArrtibute = pElement->Attribute(KStrA_Pricetype);
	if ( NULL == pStrArrtibute )
	{
		////ASSERT(0);
		return false;
	}
	
	m_eArbPrice = (E_ArbPrice)atoi(pStrArrtibute);

	// ����λ��
	pStrArrtibute = pElement->Attribute(KStrA_SaveDec);
	if ( NULL == pStrArrtibute )
	{
		m_iSaveDec = 2;
	}
	else
	{
		m_iSaveDec = atoi(pStrArrtibute);

		if ( m_iSaveDec < 1 || m_iSaveDec > 4 )
		{
			////ASSERT(0);
			m_iSaveDec = 2;
		}
	}

	// �Ƿ񱨾�
	pStrArrtibute = pElement->Attribute(KStrA_Alarm);
	if ( NULL == pStrArrtibute )
	{
		m_bAlarm = false;
	}
	else
	{
		m_bAlarm = atoi(pStrArrtibute);
	}

	// ���Ƽ�
	pStrArrtibute = pElement->Attribute(KStrA_Up);
	if ( NULL == pStrArrtibute )
	{
		m_fUp = 0.;
	}
	else
	{
		m_fUp = atof(pStrArrtibute);	
	}

	// ���Ƽ�
	pStrArrtibute = pElement->Attribute(KStrA_Down);
	if ( NULL == pStrArrtibute )
	{
		m_fDown = 0.;
	}
	else
	{
		m_fDown = atof(pStrArrtibute);	
	}

	// ��ƷA
	TiXmlElement* pChild = pElement->FirstChildElement(KStrE_Merchs);
	if ( NULL == pChild )
	{
		////ASSERT(0);
		return false;
	}
	if ( !LoadMerchFromXml(pChild, m_MerchA,pData) )
	{
		return false;
	}

	// ��ƷB
	pChild = pChild->NextSiblingElement(KStrE_Merchs);
	if ( NULL == pChild )
	{
		////ASSERT(0);
		return false;
	}
	if ( !LoadMerchFromXml(pChild, m_MerchB,pData) )
	{
		return false;
	}

	if( EABThreeMerch == m_eArbitrage )
	{
		pChild = pChild->NextSiblingElement(KStrE_Merchs);
		if ( NULL == pChild )
		{
			////ASSERT(0);
			return false;
		}
		if ( !LoadMerchFromXml(pChild, m_MerchC,pData ))
		{
			return false;
		}
	}

	return true;
}

bool32 CArbitrage::LoadMerchFromXml( TiXmlElement *pElement, OUT T_ArbMerch &arbMerch,CAbsCenterManager* pData)
{

	CAbsCenterManager *pCenterManager = pData;
	if (pCenterManager == NULL)
	{
		////ASSERT(0);
		return false;
	}

	const char *pStrArrtibute = NULL;

	// id
	pStrArrtibute = pElement->Attribute(KStrA_Id);
	if ( NULL == pStrArrtibute )
	{
		////ASSERT(0);
		return false;
	}
	
	int32 iID = (E_ArbPrice)atoi(pStrArrtibute);
	
	// code
	pStrArrtibute = pElement->Attribute(KStrA_Code);
	if ( NULL == pStrArrtibute )
	{
		////ASSERT(0);
		return false;
	}
	
	wstring wStrCode;
	if ( !MultiChar2Unicode(EMCCUtf8, pStrArrtibute, wStrCode) )
	{
		////ASSERT(0);
		return false;
	}
	
	CMerch* pMerch = NULL;
	if ( !pCenterManager->GetMerchManager().FindMerch(wStrCode.c_str(), iID, pMerch) || NULL == pMerch )
	{
		// ////ASSERT(0);
		return false;
	}
	
	arbMerch.m_pMerch = pMerch;
	
	// formula
	pStrArrtibute = pElement->Attribute(KStrA_Formular);
	if ( NULL == pStrArrtibute )
	{
		arbMerch.m_pFormular = NULL;
	}
	else
	{
		wstring wStrFormular;
		if ( !MultiChar2Unicode(EMCCUtf8, pStrArrtibute, wStrFormular) || wStrFormular.empty() )
		{
			arbMerch.m_pFormular = NULL;
		}
		else
		{
			arbMerch.m_pFormular = new CArbitrageFormula();
			arbMerch.m_pFormular->SetFormula(wStrFormular.c_str());
			arbMerch.m_pFormular->AddSpecialVarName(arbMerch.m_pMerch->GetMerchVarName());	// ���������
		}
	}

	// �Ƿ��в�����Ʒ
	for ( TiXmlElement* pOther=pElement->FirstChildElement(KStrE_OtherMerchs)
		; NULL!=pOther
		; pOther=pOther->NextSiblingElement(KStrE_OtherMerchs) )
	{
		// id
		pStrArrtibute = pOther->Attribute(KStrA_Id);
		if ( NULL == pStrArrtibute )
		{
			////ASSERT(0);
			return false;
		}
		
		int32 iTmpID = (E_ArbPrice)atoi(pStrArrtibute);
		
		// code
		pStrArrtibute = pOther->Attribute(KStrA_Code);
		if ( NULL == pStrArrtibute )
		{
			////ASSERT(0);
			return false;
		}
		
		wstring wStr;
		if ( !MultiChar2Unicode(EMCCUtf8, pStrArrtibute, wStr) )
		{
			////ASSERT(0);
			return false;
		}
		
		CMerch* pTmpMerch = NULL;
		if ( !pCenterManager->GetMerchManager().FindMerch(wStr.c_str(), iTmpID, pTmpMerch) || NULL == pTmpMerch )
		{
			////ASSERT(0);
			return false;
		}
		
		arbMerch.m_aOtherMerch.push_back( pTmpMerch );

		if ( NULL!=arbMerch.m_pFormular )
		{
			CString StrVarName = pTmpMerch->GetMerchVarName();
			arbMerch.m_pFormular->AddSpecialVarName(StrVarName);
		}
	}

	return true;
}

CString CArbitrage::SaveMerchToXml( const T_ArbMerch &arbMerch ) const
{
	CString StrMerch;

	const T_ArbMerch &Merch = arbMerch;
	if ( NULL == Merch.m_pMerch )
	{
		return L"";
	}
	
	CString StrFormular = L"";
	if ( NULL != Merch.m_pFormular )
	{
		StrFormular = Merch.m_pFormular->GetFormula();
	}
	
	StrMerch.Format(L"<%s %s = \"%d\" %s = \"%s\" %s = \"%s\" >\n", 
		CString(KStrE_Merchs).GetBuffer(),
		CString(KStrA_Id).GetBuffer(),
		Merch.m_pMerch->m_MerchInfo.m_iMarketId, 
		CString(KStrA_Code).GetBuffer(),
		Merch.m_pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(),
		CString(KStrA_Formular).GetBuffer(),
		StrFormular.GetBuffer());
	
	CString StrOtherMerch;
	for ( int32 i=0; i < Merch.m_aOtherMerch.size() ; ++i )
	{
		StrOtherMerch.Format(_T("\t<%s %s=\"%d\" %s=\"%s\" />\n"),
			CString(KStrE_OtherMerchs).GetBuffer(),
			CString(KStrA_Id).GetBuffer(),
			Merch.m_aOtherMerch[i]->m_MerchInfo.m_iMarketId, 
			CString(KStrA_Code).GetBuffer(),
			Merch.m_aOtherMerch[i]->m_MerchInfo.m_StrMerchCode.GetBuffer()
			);
		StrMerch += StrOtherMerch;
	}
	
	StrMerch += _T("</");
	StrMerch += CString(KStrE_Merchs);
	StrMerch += _T(">\n");

	return StrMerch;
}

CString CArbitrage::ToXml()
{
	CString StrXml;
	
	StrXml.Format(L"<%s %s = \"%d\" %s = \"%d\" %s = \"%d\" %s = \"%d\" %s = \"%f\" %s = \"%f\"  >\n", CString(KStrE_Arbitrage).GetBuffer(), CString(KStrA_Arbitype).GetBuffer(), m_eArbitrage, CString(KStrA_Pricetype).GetBuffer(), m_eArbPrice, CString(KStrA_SaveDec).GetBuffer(), m_iSaveDec, CString(KStrA_Alarm).GetBuffer(), m_bAlarm, CString(KStrA_Up).GetBuffer(), m_fUp, CString(KStrA_Down).GetBuffer(), m_fDown);

	// ��ƷA
	{
		CString StrMerch = SaveMerchToXml(m_MerchA);
		if ( StrMerch.IsEmpty() )
		{
			////ASSERT(0);
			return _T("");
		}
		StrXml += StrMerch;
	}

	// ��ƷB
	{
		CString StrMerch = SaveMerchToXml(m_MerchB);
		if ( StrMerch.IsEmpty() )
		{
			////ASSERT(0);
			return _T("");
		}
		
		StrXml += StrMerch;
	}

	// ��ƷC
	if ( EABThreeMerch == m_eArbitrage )
	{
		CString StrMerch = SaveMerchToXml(m_MerchC);
		if ( StrMerch.IsEmpty() )
		{
			////ASSERT(0);
			return _T("");
		}

		StrXml += StrMerch;
	}

	StrXml += L"</";
	StrXml += CString(KStrE_Arbitrage);
	StrXml += L">\n";

	return StrXml;
}

CString CArbitrage::GetShowName() const
{
	if ( NULL == m_MerchA.m_pMerch || NULL == m_MerchB.m_pMerch )
	{
		////ASSERT(0);
		return L"";
	}
	
	// ��ͷ��Ʒ
	CString Str1;
	if ( NULL == m_MerchA.m_pFormular )
	{
		Str1 = m_MerchA.m_pMerch->m_MerchInfo.m_StrMerchCnName;
	}
	else
	{
		Str1 = m_MerchA.m_pFormular->GetFormula();
	}
	
	if ( EABThreeMerch == m_eArbitrage && NULL != m_MerchC.m_pMerch )
	{
		Str1 += L" + ";
		if ( NULL == m_MerchC.m_pFormular )
		{
			Str1 += m_MerchC.m_pMerch->m_MerchInfo.m_StrMerchCnName;
		}	
		else
		{
			Str1 += m_MerchC.m_pFormular->GetFormula();
		}		
	}

	CString StrDuo;
	StrDuo.Format(L"(%s)", Str1.GetBuffer());

	// �۲�����
	CString StrPriceType;
	if ( EAPDiv == m_eArbPrice )
	{
		StrPriceType = L"/";
	}
	else if ( EAPDiff == m_eArbPrice )
	{
		StrPriceType = L"-";
	}

	// ��ͷ��Ʒ
	CString StrKong;
	if ( NULL == m_MerchB.m_pFormular )
	{
		StrKong.Format(L"(%s)", m_MerchB.m_pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer());
	}
	else
	{
		StrKong.Format(L"(%s)", CString(m_MerchB.m_pFormular->GetFormula()).GetBuffer());
	}
	
	//
	CString StrName = StrDuo + StrPriceType + StrKong;

	return StrName;
}

float CArbitrage::FormularPrice(float fPriceOriginal, CArbitrageFormula* pFormular, CMerch *pMerchToCalc, E_ArbitrageHead eHead) const
{
	if (NULL == pMerchToCalc)
	{
		return fPriceOriginal;
	}

	//
	if ( NULL == pFormular )
	{
		// û�й�ʽ, ֱ�ӷ���ԭʼ��ֵ
		return fPriceOriginal;
	}

	//
	// ��ʽ��һ��
	bool32 bOK = pFormular->TestFormula();
	
	if ( !bOK )
	{
		////ASSERT(0); 
		return s_KfError;
	}
	
	//
	ArbitrageVarPtrs vars;
	pFormular->GetVariables(vars);

	int32 iMin = 0;
	int32 iMax = 0;

	// ��Ʒ����
	CString StrVarName = pMerchToCalc->GetMerchVarName();

	ArbitrageVarPtrs::iterator itMerch = vars.find(StrVarName);
	typedef set<CString> VarNameSet;
	VarNameSet		varInited;
	if ( itMerch != vars.end() )
	{
		itMerch->second->SetMinIndex(iMin);
		itMerch->second->SetMaxIndex(iMax);
		itMerch->second->SetValue(iMin, fPriceOriginal);
		varInited.insert( VarNameSet::value_type(itMerch->first) );
	}

	// ������Ʒ����
	bool32 bErr = false;
	ArbMerchArray aOtherMerch;
	GetOtherMerchs(aOtherMerch);
	for ( int i=0; i < aOtherMerch.size() ; ++i )
	{
		CMerch *pMerchOther = aOtherMerch[i];
		CString strName = pMerchOther->GetMerchVarName();
		ArbitrageVarPtrs::iterator itOther = vars.find(strName);
		if ( itOther != vars.end() )
		{
			if ( pMerchOther->m_pRealtimePrice == NULL )
			{
				bErr = true;
				break;
			}
			itOther->second->SetMinIndex(iMin);
			itOther->second->SetMaxIndex(iMax);
			float fPriceNew = GetOtherMerchPrice(pMerchOther, eHead);
			itOther->second->SetValue(iMin,  fPriceNew);

			varInited.insert( VarNameSet::value_type(itOther->first) );
		}
	}

	ASSERT( bErr || varInited.size() == vars.size() );
	bErr = bErr || varInited.size()!=vars.size();
	
	// ��ֵ
	// ������Ʒ������ֵ
	float fResult = s_KfError;
	if ( !bErr )
	{
		bErr = !pFormular->Calculate2(vars, iMin, iMax);
		if ( !bErr )
		{
			CArbitrageFormulaVariableVar stResult = pFormular->GetLastVar();
			stResult.GetValue(iMin, fResult);
		}
	}
	pFormular->FreeVariables(vars);
	
	return fResult;
}

bool32 CArbitrage::FormularPrice( OUT CArbitrageFormulaVariableVar &varResult
								  , INOUT CArbitrageFormulaVariableVar &varIn
								  , CArbitrageFormula *pFormula
								  , CMerch *pMerchToCalc
								  , const ArbVarArray &aOtherVars ) const
{
	if ( NULL == pFormula )
	{
		// Ĭ���޹�ʽ��ֱ�Ӹ�ֵ
		varResult = varIn;
		return true;
	}

	ArbitrageVarPtrs vars;
	if ( !pFormula->TestFormula() || !pFormula->GetUnInitializedVariables(vars) )
	{
		////ASSERT(0);
		return false;
	}

	// ������ֵ
	CString StrVarName = pMerchToCalc->GetMerchVarName();
	ArbitrageVarPtrs::iterator itMerch = vars.find(StrVarName);
	if ( itMerch != vars.end() )
	{
		CArbitrageFormulaVariableVar *pVar = itMerch->second;
		ASSERT( NULL == pVar ); // δ��ʼ����
		itMerch->second = const_cast<CArbitrageFormulaVariableVar *>(&varIn);
	}
	
	// ������Ʒ����
	
	for ( int i=0; i < aOtherVars.size() ; ++i )
	{
		const CArbitrageFormulaVariableVar &var = aOtherVars[i];
		const CString &strName = var.GetVarName();
		ASSERT( !strName.IsEmpty() );
		ArbitrageVarPtrs::iterator itOther = vars.find( strName );
		if ( itOther != vars.end() )
		{
			CArbitrageFormulaVariableVar *pVar = itOther->second;
			ASSERT( NULL == pVar ); // δ��ʼ����
			itOther->second = const_cast<CArbitrageFormulaVariableVar *>(&var);
		}
	}

	bool32 bCalc = pFormula->Calculate2(vars, varIn.GetMinIndex(), varIn.GetMaxIndex());
	if ( bCalc )
	{
		varResult = pFormula->GetLastVar();
	}
	return bCalc;
}

float CArbitrage::GetOtherMerchPrice( CMerch *pOtherMerch, E_ArbitrageHead eHead ) const
{
	if ( NULL==pOtherMerch || NULL==pOtherMerch->m_pRealtimePrice )
	{
		return s_KfError;
	}
	
	float fPrice = s_KfError;
	switch ( eHead)
	{
	case EAHPriceDifPreClose:	// ���ռ۲������ģ�ͼ�����ƷA����ƷB�����ռ�
		fPrice = pOtherMerch->m_pRealtimePrice->m_fPricePrevClose;
		break;
	case EAHPriceDifPreAvg:		// ���۲������ģ�ͼ�����ƷA����ƷB������
		fPrice = pOtherMerch->m_pRealtimePrice->m_fPricePrevAvg;
		break;
	default:
		{
			fPrice = pOtherMerch->m_pRealtimePrice->m_fPriceNew;
			fPrice = 0.0f==fPrice ? pOtherMerch->m_pRealtimePrice->m_fPricePrevClose : fPrice;
		}
		break;
	}

	return fPrice;
}

float CArbitrage::GetPrice(E_ArbitrageHead eHeadType) const 
{
	switch ( eHeadType)
	{
	case EAHPriceDifAccord: // �Լۼ۲������ģ�ͼ�����ƷA����������ƷB�����
		{
			// ��ƷA
			if ( NULL == m_MerchA.m_pMerch || NULL == m_MerchA.m_pMerch->m_pRealtimePrice )
			{
				return s_KfError;
			}

			float fPriceA = FormularPrice(m_MerchA.m_pMerch->m_pRealtimePrice->m_astSellPrices[0].m_fPrice, m_MerchA.m_pFormular, m_MerchA.m_pMerch, eHeadType);
			if ( s_KfError == fPriceA )
			{
				return s_KfError;
			}

			// ��ƷC
			if ( EABThreeMerch == m_eArbitrage )
			{
				if ( NULL != m_MerchC.m_pMerch && NULL != m_MerchC.m_pMerch->m_pRealtimePrice )
				{
					float fPriceC = FormularPrice(m_MerchC.m_pMerch->m_pRealtimePrice->m_astSellPrices[0].m_fPrice, m_MerchC.m_pFormular, m_MerchC.m_pMerch, eHeadType);
					if ( s_KfError != fPriceC )
					{
						fPriceA += fPriceC;
					}
				}
			}

			// ��ƷB
			if ( NULL == m_MerchB.m_pMerch || NULL == m_MerchB.m_pMerch->m_pRealtimePrice )
			{
				return s_KfError;
			}
			
			float fPriceB = FormularPrice(m_MerchB.m_pMerch->m_pRealtimePrice->m_astBuyPrices[0].m_fPrice, m_MerchB.m_pFormular, m_MerchB.m_pMerch, eHeadType);
			if ( s_KfError == fPriceB )
			{
				return s_KfError;
			}

			// �۲�ģ��
			if ( EAPDiv == m_eArbPrice )
			{
				if ( 0. == fPriceB )
				{
					return s_KfError;
				}
				else
				{
					return fPriceA / fPriceB;
				}
			}
			else if ( EAPDiff == m_eArbPrice )
			{
				return (fPriceA - fPriceB);
			}
		}
		break;
	case EAHPriceDifQuote:	// �Ҽۼ۲������ģ�ͼ�����ƷA���������ƷB������
		{
			// ��ƷA
			if ( NULL == m_MerchA.m_pMerch || NULL == m_MerchA.m_pMerch->m_pRealtimePrice )
			{
				return s_KfError;
			}
			
			float fPriceA = FormularPrice(m_MerchA.m_pMerch->m_pRealtimePrice->m_astBuyPrices[0].m_fPrice, m_MerchA.m_pFormular, m_MerchA.m_pMerch, eHeadType);
			if ( s_KfError == fPriceA )
			{
				return s_KfError;
			}
			
			// ��ƷC
			if ( EABThreeMerch == m_eArbitrage )
			{
				if ( NULL != m_MerchC.m_pMerch && NULL != m_MerchC.m_pMerch->m_pRealtimePrice )
				{
					float fPriceC = FormularPrice(m_MerchC.m_pMerch->m_pRealtimePrice->m_astBuyPrices[0].m_fPrice, m_MerchC.m_pFormular, m_MerchC.m_pMerch, eHeadType);
					if ( s_KfError != fPriceC )
					{
						fPriceA += fPriceC;
					}
				}
			}
			
			// ��ƷB
			if ( NULL == m_MerchB.m_pMerch || NULL == m_MerchB.m_pMerch->m_pRealtimePrice )
			{
				return s_KfError;
			}
			
			float fPriceB = FormularPrice(m_MerchB.m_pMerch->m_pRealtimePrice->m_astSellPrices[0].m_fPrice, m_MerchB.m_pFormular, m_MerchB.m_pMerch, eHeadType);
			if ( s_KfError == fPriceB )
			{
				return s_KfError;
			}
			
			// �۲�ģ��
			if ( EAPDiv == m_eArbPrice )
			{
				if ( 0. == fPriceB )
				{
					return s_KfError;
				}
				else
				{
					return fPriceA / fPriceB;
				}
			}
			else if ( EAPDiff == m_eArbPrice )
			{
				return (fPriceA - fPriceB);
			}
		}
		break;
	case EAHPriceDifNew:    // ���¼۲������ģ�ͼ�����ƷA����ƷB�����¼�
		{
			// ���ABC��Ʒ�г���û�����¼�(new=0), ��ʹ��3����Ʒ�����մ���
			// ��ƷA
			bool32 bNeedCMerch = IsNeedCMerch();
			if ( NULL == m_MerchA.m_pMerch || NULL == m_MerchA.m_pMerch->m_pRealtimePrice
				|| NULL==m_MerchB.m_pMerch || NULL==m_MerchB.m_pMerch->m_pRealtimePrice
				|| (bNeedCMerch && ( NULL==m_MerchC.m_pMerch || NULL==m_MerchC.m_pMerch->m_pRealtimePrice )) )
			{
				return s_KfError;
			}

			if ( 0.0f==m_MerchA.m_pMerch->m_pRealtimePrice->m_fPriceNew
				|| 0.0f==m_MerchB.m_pMerch->m_pRealtimePrice->m_fPriceNew
				|| (bNeedCMerch && 0.0f==m_MerchC.m_pMerch->m_pRealtimePrice->m_fPriceNew))
			{
				// �����¼ۣ�ȡ����
				return GetPrice(EAHPriceDifPreClose);
			}
			
			float fPriceA = FormularPrice(m_MerchA.m_pMerch->m_pRealtimePrice->m_fPriceNew, m_MerchA.m_pFormular, m_MerchA.m_pMerch, eHeadType);
			if ( s_KfError == fPriceA )
			{
				return s_KfError;
			}
			
			// ��ƷC
			if ( EABThreeMerch == m_eArbitrage )
			{
				if ( NULL != m_MerchC.m_pMerch && NULL != m_MerchC.m_pMerch->m_pRealtimePrice )
				{
					float fPriceC = FormularPrice(m_MerchC.m_pMerch->m_pRealtimePrice->m_fPriceNew, m_MerchC.m_pFormular, m_MerchC.m_pMerch, eHeadType);
					if ( s_KfError != fPriceC )
					{
						fPriceA += fPriceC;
					}
				}
			}
			
			// ��ƷB
			if ( NULL == m_MerchB.m_pMerch || NULL == m_MerchB.m_pMerch->m_pRealtimePrice )
			{
				return s_KfError;
			}
			
			float fPriceB = FormularPrice(m_MerchB.m_pMerch->m_pRealtimePrice->m_fPriceNew, m_MerchB.m_pFormular, m_MerchB.m_pMerch, eHeadType);
			if ( s_KfError == fPriceB )
			{
				return s_KfError;
			}
			
			// �۲�ģ��
			if ( EAPDiv == m_eArbPrice )
			{
				if ( 0. == fPriceB )
				{
					return s_KfError;
				}
				else
				{
					return fPriceA / fPriceB;
				}
			}
			else if ( EAPDiff == m_eArbPrice )
			{
				return (fPriceA - fPriceB);
			}
		}
		break;
	case EAHRisePriceDif:   // �۲��ǵ������¼۲������ռ۲�֮��ֵ
		{
			float fNew		= GetPrice(EAHPriceDifNew);
			float fPreClose = GetPrice(EAHPriceDifPreAvg);

			if ( s_KfError == fNew || s_KfError == fPreClose )
			{
				return s_KfError;
			}

			//
			return (fNew - fPreClose);
		}
		break;
	case EAHPriceMerchA:	// A �����¼�
		{
			if ( NULL == m_MerchA.m_pMerch || NULL == m_MerchA.m_pMerch->m_pRealtimePrice )
			{
				return s_KfError;
			}

			//
			return FormularPrice(m_MerchA.m_pMerch->m_pRealtimePrice->m_fPriceNew, m_MerchA.m_pFormular, m_MerchA.m_pMerch, eHeadType);
		}
		break;
	case EAHPriceMerchB:	// B �����¼�
		{
			if ( NULL == m_MerchB.m_pMerch || NULL == m_MerchB.m_pMerch->m_pRealtimePrice )
			{
				return s_KfError;
			}

			return FormularPrice(m_MerchB.m_pMerch->m_pRealtimePrice->m_fPriceNew, m_MerchB.m_pFormular, m_MerchB.m_pMerch, eHeadType);
		}
		break;
	case EAHPriceMerchC:	// C �����¼�
		{
			if ( NULL == m_MerchC.m_pMerch || NULL == m_MerchC.m_pMerch->m_pRealtimePrice )
			{
				return s_KfError;
			}
			
			//
			return FormularPrice(m_MerchC.m_pMerch->m_pRealtimePrice->m_fPriceNew, m_MerchC.m_pFormular, m_MerchC.m_pMerch, eHeadType);
		}
		break;
	case EAHPriceDifOpen:	// ���̼۲������ģ�ͼ�����ƷA����ƷB�Ŀ��̼�
		{
			// ��ƷA
			if ( NULL == m_MerchA.m_pMerch || NULL == m_MerchA.m_pMerch->m_pRealtimePrice )
			{
				return s_KfError;
			}
			
			float fPriceA = FormularPrice(m_MerchA.m_pMerch->m_pRealtimePrice->m_fPriceOpen, m_MerchA.m_pFormular, m_MerchA.m_pMerch, eHeadType);
			if ( s_KfError == fPriceA )
			{
				return s_KfError;
			}
			
			// ��ƷC
			if ( EABThreeMerch == m_eArbitrage )
			{
				if ( NULL != m_MerchC.m_pMerch && NULL != m_MerchC.m_pMerch->m_pRealtimePrice )
				{
					float fPriceC = FormularPrice(m_MerchC.m_pMerch->m_pRealtimePrice->m_fPriceOpen, m_MerchC.m_pFormular, m_MerchC.m_pMerch, eHeadType);
					if ( s_KfError != fPriceC )
					{
						fPriceA += fPriceC;
					}
				}
			}
			
			// ��ƷB
			if ( NULL == m_MerchB.m_pMerch || NULL == m_MerchB.m_pMerch->m_pRealtimePrice )
			{
				return s_KfError;
			}
			
			float fPriceB = FormularPrice(m_MerchB.m_pMerch->m_pRealtimePrice->m_fPriceOpen, m_MerchB.m_pFormular, m_MerchB.m_pMerch, eHeadType);
			if ( s_KfError == fPriceB )
			{
				return s_KfError;
			}
			
			// �۲�ģ��
			if ( EAPDiv == m_eArbPrice )
			{
				if ( 0. == fPriceB )
				{
					return s_KfError;
				}
				else
				{
					return fPriceA / fPriceB;
				}
			}
			else if ( EAPDiff == m_eArbPrice )
			{
				return (fPriceA - fPriceB);
			}
		}
		break;
	case EAHPriceDifPreClose:	// ���ռ۲������ģ�ͼ�����ƷA����ƷB�����ռ�
		{
			// ��ƷA
			if ( NULL == m_MerchA.m_pMerch || NULL == m_MerchA.m_pMerch->m_pRealtimePrice )
			{
				return s_KfError;
			}
			
			float fPriceA = FormularPrice(m_MerchA.m_pMerch->m_pRealtimePrice->m_fPricePrevClose, m_MerchA.m_pFormular, m_MerchA.m_pMerch, eHeadType);
			if ( s_KfError == fPriceA )
			{
				return s_KfError;
			}
			
			// ��ƷC
			if ( EABThreeMerch == m_eArbitrage )
			{
				if ( NULL != m_MerchC.m_pMerch && NULL != m_MerchC.m_pMerch->m_pRealtimePrice )
				{
					float fPriceC = FormularPrice(m_MerchC.m_pMerch->m_pRealtimePrice->m_fPricePrevClose, m_MerchC.m_pFormular, m_MerchC.m_pMerch, eHeadType);
					if ( s_KfError != fPriceC )
					{
						fPriceA += fPriceC;
					}
				}
			}
			
			// ��ƷB
			if ( NULL == m_MerchB.m_pMerch || NULL == m_MerchB.m_pMerch->m_pRealtimePrice )
			{
				return s_KfError;
			}
			
			float fPriceB = FormularPrice(m_MerchB.m_pMerch->m_pRealtimePrice->m_fPricePrevClose, m_MerchB.m_pFormular, m_MerchB.m_pMerch, eHeadType);
			if ( s_KfError == fPriceB )
			{
				return s_KfError;
			}
			
			// �۲�ģ��
			if ( EAPDiv == m_eArbPrice )
			{
				if ( 0. == fPriceB )
				{
					return s_KfError;
				}
				else
				{
					return fPriceA / fPriceB;
				}
			}
			else if ( EAPDiff == m_eArbPrice )
			{
				return (fPriceA - fPriceB);
			}
		}
		break;
	case EAHPriceDifPreAvg:		// ���۲������ģ�ͼ�����ƷA����ƷB������
		{
			// ��ƷA
			if ( NULL == m_MerchA.m_pMerch || NULL == m_MerchA.m_pMerch->m_pRealtimePrice )
			{
				return s_KfError;
			}
			
			float fPriceA = FormularPrice(m_MerchA.m_pMerch->m_pRealtimePrice->m_fPricePrevAvg, m_MerchA.m_pFormular, m_MerchA.m_pMerch, eHeadType);
			if ( s_KfError == fPriceA )
			{
				return s_KfError;
			}
			
			// ��ƷC
			if ( EABThreeMerch == m_eArbitrage )
			{
				if ( NULL != m_MerchC.m_pMerch && NULL != m_MerchC.m_pMerch->m_pRealtimePrice )
				{
					float fPriceC = FormularPrice(m_MerchC.m_pMerch->m_pRealtimePrice->m_fPricePrevAvg, m_MerchC.m_pFormular, m_MerchC.m_pMerch, eHeadType);
					if ( s_KfError != fPriceC )
					{
						fPriceA += fPriceC;
					}
				}
			}
			
			// ��ƷB
			if ( NULL == m_MerchB.m_pMerch || NULL == m_MerchB.m_pMerch->m_pRealtimePrice )
			{
				return s_KfError;
			}
			
			float fPriceB = FormularPrice(m_MerchB.m_pMerch->m_pRealtimePrice->m_fPricePrevAvg, m_MerchB.m_pFormular, m_MerchB.m_pMerch, eHeadType);
			if ( s_KfError == fPriceB )
			{
				return s_KfError;
			}
			
			// �۲�ģ��
			if ( EAPDiv == m_eArbPrice )
			{
				if ( 0. == fPriceB )
				{
					return s_KfError;
				}
				else
				{
					return fPriceA / fPriceB;
				}
			}
			else if ( EAPDiff == m_eArbPrice )
			{
				return (fPriceA - fPriceB);
			}
		}
		break;
	case EAHPriceDifAvg:		// ���ۼ۲������ģ�ͼ�����ƷA����ƷB�Ľ����
		{
			// ��ƷA
			if ( NULL == m_MerchA.m_pMerch || NULL == m_MerchA.m_pMerch->m_pRealtimePrice )
			{
				return s_KfError;
			}
			
			float fPriceA = FormularPrice(m_MerchA.m_pMerch->m_pRealtimePrice->m_fPriceAvg, m_MerchA.m_pFormular, m_MerchA.m_pMerch, eHeadType);
			if ( s_KfError == fPriceA )
			{
				return s_KfError;
			}
			
			// ��ƷC
			if ( EABThreeMerch == m_eArbitrage )
			{
				if ( NULL != m_MerchC.m_pMerch && NULL != m_MerchC.m_pMerch->m_pRealtimePrice )
				{
					float fPriceC = FormularPrice(m_MerchC.m_pMerch->m_pRealtimePrice->m_fPriceAvg, m_MerchC.m_pFormular, m_MerchC.m_pMerch, eHeadType);
					if ( s_KfError != fPriceC )
					{
						fPriceA += fPriceC;
					}
				}
			}
			
			// ��ƷB
			if ( NULL == m_MerchB.m_pMerch || NULL == m_MerchB.m_pMerch->m_pRealtimePrice )
			{
				return s_KfError;
			}
			
			float fPriceB = FormularPrice(m_MerchB.m_pMerch->m_pRealtimePrice->m_fPriceAvg, m_MerchB.m_pFormular, m_MerchB.m_pMerch, eHeadType);
			if ( s_KfError == fPriceB )
			{
				return s_KfError;
			}
			
			// �۲�ģ��
			if ( EAPDiv == m_eArbPrice )
			{
				if ( 0. == fPriceB )
				{
					return s_KfError;
				}
				else
				{
					return fPriceA / fPriceB;
				}
			}
			else if ( EAPDiff == m_eArbPrice )
			{
				return (fPriceA - fPriceB);
			}
		}
		break;
	default:
		{
			return s_KfError;
		}
		break;
	}

	return s_KfError;
}



bool32 CArbitrage::IsNeedCMerch() const
{
	return EABThreeMerch == m_eArbitrage;
}

int32 CArbitrage::GetOtherMerchs( OUT ArbMerchArray &aMerchs, bool32 bContainABC/*=false*/ ) const
{
	aMerchs.clear();

	typedef set<CMerch *> MerchSet;
	MerchSet sMerch;
	
	int32 i=0;
	for ( i=0; i<m_MerchA.m_aOtherMerch.size() ; ++i )
	{
		sMerch.insert( MerchSet::value_type(m_MerchA.m_aOtherMerch[i]) );
	}

	for ( i=0; i<m_MerchB.m_aOtherMerch.size() ; ++i )
	{
		sMerch.insert( MerchSet::value_type(m_MerchB.m_aOtherMerch[i]) );
	}

	if ( IsNeedCMerch() )
	{
		for ( i=0; i<m_MerchC.m_aOtherMerch.size() ; ++i )
		{
			sMerch.insert( MerchSet::value_type(m_MerchC.m_aOtherMerch[i]) );
		}
	}

	// �������ABC��Ʒ
	// ��ʹ��߳�����ABC��Ʒ�����ڲ�����Ʒֻ�������̼ۣ���ʵ�ʵ�ABC��Ʒ���㷽ʽ��ͬ
	if ( bContainABC )
	{
		sMerch.insert( MerchSet::value_type(m_MerchA.m_pMerch) );
		sMerch.insert( MerchSet::value_type(m_MerchB.m_pMerch) );
		if ( IsNeedCMerch() )
		{
			sMerch.insert( MerchSet::value_type(m_MerchC.m_pMerch) );
		}
	}

	for ( MerchSet::iterator it=sMerch.begin(); it!=sMerch.end() ; ++it )
	{
		aMerchs.push_back( *it );
	}

	return aMerchs.size();
}

int32 CArbitrage::GetOtherMerchsCount(bool32 bContainABC/*=false*/) const
{
	CArbitrage::ArbMerchArray aMerchs;
	return GetOtherMerchs(aMerchs, bContainABC);
}



//////////////////////////////////////////////////////////////////////////
//
CArbitrageManage* CArbitrageManage::m_pThis = NULL;

CArbitrageManage::CArbitrageManage()
{
	m_bFromXml = false;
	m_pThis = NULL;
	m_aArbitrages.RemoveAll();
	m_apNotifys.RemoveAll();
	m_apMerchsAll.clear();
	m_pCenterManager = NULL;
	m_pAlarmCenter = NULL;

}
//lint --e{1579}
CArbitrageManage::~CArbitrageManage()
{
	
	m_apNotifys.RemoveAll();
	m_aArbitrages.RemoveAll();

	
	if ( NULL != m_pAlarmCenter )
	{
		m_pAlarmCenter->DelNotify(this);
		DelNotify(m_pAlarmCenter);
	}
	
}

CArbitrageManage* CArbitrageManage::Instance()
{
	if ( NULL == m_pThis )
	{
		m_pThis = new CArbitrageManage();
		// ASSERT( NULL!=m_pThis->m_pViewData );
		// ����&����ˮ��Ʒ�ĳ�ʼ��
	}

	return m_pThis;
}

void CArbitrageManage::DelInstance()
{
	DEL(m_pThis);
}

bool CArbitrageManage::Initialize(CAbsCenterManager* pData,CAlarmCenter* pAlarm)
{
	if (pAlarm != NULL)
	{
		m_pAlarmCenter = pAlarm;
		AddNotify(m_pAlarmCenter);
		m_pAlarmCenter->AddNotify(this);
	}
	
	if (pData != NULL)
	{
		m_pCenterManager = pData;
		FromXml();
		return true;
	}

	return false;
}


bool32 CArbitrageManage::FromXml()
{
	m_bFromXml = false;

	if (m_pCenterManager == NULL)
	{
		////ASSERT(0);
		return false;
	}

	m_aArbitrages.RemoveAll();

	// ��ȡ˽��Ŀ¼
	{
		CString StrPathW = CPathFactory::GetArbitragePath(m_pCenterManager->GetUserName());		

	

		std::string StrPathA;
		Unicode2MultiChar(CP_ACP, StrPathW, StrPathA);
		

		TiXmlDocument Doc;		
		if ( Doc.LoadFile(StrPathA.c_str()) )
		{
			
			//
			TiXmlElement* pRoot = Doc.RootElement();
			if ( NULL == pRoot )
			{
				////ASSERT(0);
				return false;
			}
			
			//
			for ( TiXmlElement* pElementArb = pRoot->FirstChildElement(); pElementArb != NULL; pElementArb = pElementArb->NextSiblingElement() )
			{
				CArbitrage stArbitrage;
				if ( stArbitrage.FromXml(pElementArb,m_pCenterManager) )
				{
					m_aArbitrages.Add(stArbitrage);
				}
			}
		}
		else
		{
			// ����Ŀ¼
			CString strPubPathW = CPathFactory::GetArbitragePublicPath();
		
			std::string strPathA;
			Unicode2MultiChar(CP_ACP, strPubPathW, strPathA);

			TiXmlDocument tmpDoc;			
			if ( tmpDoc.LoadFile(StrPathA.c_str()) )
			{
				//
				TiXmlElement* pRoot = tmpDoc.RootElement();
				if ( NULL == pRoot )
				{
					////ASSERT(0);
					return false;
				}
				
				//
				for ( TiXmlElement* pElementArb = pRoot->FirstChildElement(); pElementArb != NULL; pElementArb = pElementArb->NextSiblingElement() )
				{
					CArbitrage stArbitrage;
					if ( stArbitrage.FromXml(pElementArb,m_pCenterManager) )
					{
						m_aArbitrages.Add(stArbitrage);
					}
				}
			}
		}
	}

	//
	m_bFromXml = true;
	RebuildMerchsAllRelationShips();
	return true;
}

bool32 CArbitrageManage::ToXml()
{
	if (m_pCenterManager == NULL)
	{
		////ASSERT(0);
		return false;
	}
	// �����ļ�
	CString StrXml;
	
	CString StrHead;
	StrHead.Format(L"<?xml version =\"%s\" encoding=\"utf-8\" standalone=\"yes\"?> \n", L"1.0");
	StrXml += StrHead;
	
	StrHead.Format(L"<XMLDATA version=\"1.0\" app = \"ggtong\" data = \"Arbitrage\">\n"),
	StrXml += StrHead;

	//
	for ( int32 i = 0; i < m_aArbitrages.GetSize(); i++ )
	{
		StrXml += m_aArbitrages[i].ToXml();
	}

	//
	StrXml += L"</";
	StrXml += L"XMLDATA";
	StrXml += L">";

	// д�ļ�

	CString StrPathW = CPathFactory::GetArbitragePath(m_pCenterManager->GetUserName());	
	std::string strPathA;
	Unicode2MultiChar(CP_ACP, StrPathW, strPathA);


	SaveXmlFile(strPathA.c_str(), StrXml);

	return true;
}

void CArbitrageManage::RebuildMerchsAllRelationShips()
{
	m_apMerchsAll.clear();
	m_mapMerchArbitrage.clear();

	//
	for ( int32 i = 0; i < m_aArbitrages.GetSize(); i++ )
	{
		if ( NULL != m_aArbitrages[i].m_MerchA.m_pMerch )
		{
			m_apMerchsAll.insert(m_aArbitrages[i].m_MerchA.m_pMerch);

			// ���¹�ϵ
			UpdateMap(m_aArbitrages[i].m_MerchA.m_pMerch, m_aArbitrages[i]);
		}

		if ( NULL != m_aArbitrages[i].m_MerchB.m_pMerch )
		{
			m_apMerchsAll.insert(m_aArbitrages[i].m_MerchB.m_pMerch);

			// ���¹�ϵ
			UpdateMap(m_aArbitrages[i].m_MerchB.m_pMerch, m_aArbitrages[i]);
		}

		if ( NULL != m_aArbitrages[i].m_MerchC.m_pMerch )
		{
			m_apMerchsAll.insert(m_aArbitrages[i].m_MerchC.m_pMerch);

			// ���¹�ϵ
			UpdateMap(m_aArbitrages[i].m_MerchC.m_pMerch, m_aArbitrages[i]);
		}

		// ������Ʒ
		CArbitrage::ArbMerchArray aOtherMerch;
		m_aArbitrages[i].GetOtherMerchs(aOtherMerch);
		for ( int32 j=0; j < aOtherMerch.size() ; ++j )
		{
			CMerch *pMerch = aOtherMerch[j];

			m_apMerchsAll.insert( pMerch );
			UpdateMap(pMerch, m_aArbitrages[i]);
		}



		m_pCenterManager->RemoveAttendMerch(EA_Arbitrage);
		for ( set<CMerch*>::iterator it = m_apMerchsAll.begin(); it != m_apMerchsAll.end(); ++it )
		{
			CMerch* pMerch = *it;
			if ( NULL == pMerch )
			{
				continue;
			}

			CSmartAttendMerch SmartAttendMerch;
			SmartAttendMerch.m_pMerch = pMerch;
			SmartAttendMerch.m_iDataServiceTypes = EDSTPrice;

			m_pCenterManager->AddAttendMerch(SmartAttendMerch, EA_Arbitrage);
		}	
	}
}

void CArbitrageManage::UpdateMap(CMerch* pMerch, const CArbitrage& stArbitrage)
{
	if ( NULL == pMerch )
	{
		////ASSERT(0);
		return;
	}

	//
	MerchArbitrageMap::iterator itFind = m_mapMerchArbitrage.find(pMerch);
	if ( itFind != m_mapMerchArbitrage.end() )
	{
		itFind->second.insert(stArbitrage);
	}
	else
	{
		ArbitrageSet aArray;
		aArray.insert(stArbitrage);
		
		MerchArbitrageMap::value_type value(pMerch, aArray);
		m_mapMerchArbitrage.insert(value);
	}
}

void CArbitrageManage::AddNotify(CArbitrageNotify* p)
{
	if ( NULL == p )
	{
		return;
	}

	for( int32 i = 0; i < m_apNotifys.GetSize(); i++ )
	{
		if ( m_apNotifys[i] == p )
		{
			return;
		}
	}

	m_apNotifys.Add(p);
}

void CArbitrageManage::DelNotify(CArbitrageNotify* p)
{
	if ( NULL == p )
	{
		return;
	}

	for( int32 i = 0; i < m_apNotifys.GetSize(); i++ )
	{
		if ( m_apNotifys[i] == p )
		{
			m_apNotifys.RemoveAt(i);
			return;
		}
	}
}

void CArbitrageManage::AddArbitrage(const CArbitrage& stArbitrage)
{
	int32 i;
	for ( i = 0; i < m_aArbitrages.GetSize(); i++ )
	{
		if ( stArbitrage == m_aArbitrages[i] )
		{
			return;
		}
	}

	//
	CArbitrage st = stArbitrage;

	m_aArbitrages.Add(st);
	RebuildMerchsAllRelationShips();

	//
	for ( i = 0; i < m_apNotifys.GetSize(); i++ )
	{
		if ( NULL != m_apNotifys[i] )
		{
			m_apNotifys[i]->OnArbitrageAdd(stArbitrage);
		}
	}

	//
	ToXml();
}

void CArbitrageManage::DelArbitrage(const CArbitrage& stArbitrage)
{
	for ( int32 i = 0; i < m_aArbitrages.GetSize(); i++ )
	{
		if ( stArbitrage == m_aArbitrages[i] )
		{
			m_aArbitrages.RemoveAt(i);
			RebuildMerchsAllRelationShips();
			
			//
			for ( int32 j = 0; j < m_apNotifys.GetSize(); j++ )
			{
				if ( NULL != m_apNotifys[j] )
				{
					m_apNotifys[j]->OnArbitrageDel(stArbitrage);
				}
			}

			//
			ToXml();
			break;
		}
	}
}

void CArbitrageManage::ModifyArbitrage(const CArbitrage& stArbitrageOld, const CArbitrage& stArbitrageNew)
{
	for ( int32 i = 0; i < m_aArbitrages.GetSize(); i++ )
	{
		if ( stArbitrageOld == m_aArbitrages[i] )
		{
			//
			CArbitrage st = stArbitrageNew;
			m_aArbitrages.SetAt(i, st);
			RebuildMerchsAllRelationShips();

			//
			for ( int32 j = 0; j < m_apNotifys.GetSize(); j++ )
			{
				if ( NULL != m_apNotifys[j] )
				{
					m_apNotifys[j]->OnArbitrageModify(stArbitrageOld, stArbitrageNew);
				}
			}
			
			//
			ToXml();
			break;
		}
	}
}

void CArbitrageManage::GetArbitrageMerchs(OUT CArray<CMerch*, CMerch*>& aMerchs)
{
	aMerchs.RemoveAll();

	//
	for ( set<CMerch*>::iterator it = m_apMerchsAll.begin(); it != m_apMerchsAll.end(); ++it )
	{
		CMerch* pMerch = *it;
		if ( NULL == pMerch )
		{
			continue;
		}
		
		//
		aMerchs.Add(pMerch);
	}
}

void CArbitrageManage::GetArbitrageDatasByMerch(IN CMerch* pMerch, OUT CArray<CArbitrage, CArbitrage&>& aArbitrages)
{
	aArbitrages.RemoveAll();

	if ( NULL == pMerch )
	{
		return;
	}

	//
	MerchArbitrageMap::iterator itFind = m_mapMerchArbitrage.find(pMerch);
	if ( m_mapMerchArbitrage.end() == itFind )
	{
		return;
	}

	//
	for ( ArbitrageSet::iterator itSet = itFind->second.begin(); itSet != itFind->second.end(); ++ itSet )
	{
		CArbitrage st = *itSet;
		aArbitrages.Add(st);
	}
}

void CArbitrageManage::GetArbitrageDatasByMerch( IN CMerch* pMerch, OUT ArbitrageSet &sArbitrages )
{
	sArbitrages.clear();
	
	if ( NULL == pMerch )
	{
		return;
	}
	
	//
	MerchArbitrageMap::iterator itFind = m_mapMerchArbitrage.find(pMerch);
	if ( m_mapMerchArbitrage.end() == itFind )
	{
		return;
	}
	
	//
	sArbitrages = itFind->second;
}

void CArbitrageManage::GetArbitrageDatas(OUT CArray<CArbitrage, CArbitrage&>& aArbitrages)
{
	int32 iSize = m_aArbitrages.GetSize();
	aArbitrages.SetSize(0, iSize);

	//
	if ( iSize > 0 )
	{
		for ( int32 i=0; i < iSize ; ++i )
		{
			aArbitrages.Add(m_aArbitrages[i]) ;
		}
	}
}

bool32 CArbitrageManage::CombineArbitrageKLineTime( OUT vector<long> &aCommTimes, /* ��ͬʱ�� */ 
												IN const KLineInMerchArray *paKLineA, /* A�ߣ�����ΪNULL */ 
												IN const KLineInMerchArray *paKLineB, /* B�ߣ�����ΪNULL */ 
												IN const KLineInMerchArray *paKLineC )
{
	aCommTimes.clear();
	if ( NULL!=paKLineA && NULL!=paKLineB )
	{
		const KLineInMerchArray &aKLineA = *paKLineA;
		const KLineInMerchArray &aKLineB = *paKLineB;

		int32 iSizeA = aKLineA.GetSize();
		int32 iSizeB = aKLineB.GetSize();
		
		const CKLine* pTmpKLineA = aKLineA.GetData();
		const CKLine* pTmpKLineB = aKLineB.GetData();

		aCommTimes.reserve(iSizeA);
		// ���뱣֤����
		int32 j=0;
		int32 i;
		for ( i=0; i < iSizeA && j < iSizeB ; ++i )
		{
			CGmtTime TimeA = pTmpKLineA[i].m_TimeCurrent;
			while ( j < iSizeB && pTmpKLineB[j].m_TimeCurrent < TimeA )
			{
				++j;
			}
			if ( j < iSizeB && pTmpKLineB[j].m_TimeCurrent == TimeA )
			{
				aCommTimes.push_back(TimeA.GetTime());
			}
		}
		if ( NULL!=paKLineC )
		{
			const KLineInMerchArray &aKLineC = *paKLineC;
			int32 iSizeC = aKLineC.GetSize();
			const CKLine* pTmpKLineC = aKLineC.GetData();

			j=0;
			vector<long> aTmp;
			aTmp.reserve(aCommTimes.size());
			for ( i=0; i < aCommTimes.size() ; ++i )
			{
				CGmtTime TimeA(aCommTimes[i]);
				while ( j < iSizeC && pTmpKLineC[j].m_TimeCurrent < TimeA )
				{
					++j;
				}
				if ( j < iSizeC && pTmpKLineC[j].m_TimeCurrent == TimeA )
				{
					aTmp.push_back(aCommTimes[i]);
				}
			}
			aCommTimes = aTmp;
		}
		return true;
	}
	return false;
}

bool32 CArbitrageManage::CombineArbitrageKLine(IN const CArbitrage& st,								// ����ģ��, ����ȡ����ָ���㷨
											   IN const Merch2KLineMap &mapMerchKLines,					// ���������й���Ʒ��K������
											   OUT CArray<CKLine, CKLine>& aKLineCombineA,				// �ϳɺõ� A ��
											   OUT CArray<CKLine, CKLine>& aKLineCombineB,				// �ϳɺõ� B ��
											   OUT CArray<CKLine, CKLine>& aKLineCombine)				// �ϳɺõ� K ��
{

	aKLineCombineA.RemoveAll();
	aKLineCombineB.RemoveAll();
	aKLineCombine.RemoveAll();

	Merch2KLineMap::const_iterator itMerchA = mapMerchKLines.find(st.m_MerchA.m_pMerch);
	Merch2KLineMap::const_iterator itMerchB = mapMerchKLines.find(st.m_MerchB.m_pMerch);

	if ( itMerchA==mapMerchKLines.end() || itMerchB==mapMerchKLines.end()
		|| NULL==itMerchA->second || NULL==itMerchB->second )
	{
		////ASSERT(0);
		return false;
	}

	int i = 0;
	

	const KLineInMerchArray &aKLineA = *(itMerchA->second);
	const KLineInMerchArray &aKLineB = *(itMerchB->second);
	const KLineInMerchArray *paKLineC = NULL;
	int32 iSizeA = aKLineA.GetSize();
	int32 iSizeB = aKLineB.GetSize();
	int32 iSizeC = 0;

	const CKLine* pKLineA = (CKLine*)aKLineA.GetData();
	const CKLine* pKLineB = (CKLine*)aKLineB.GetData();
	const CKLine* pKLineC = NULL;

	bool32 bNeedMerchC = st.IsNeedCMerch();
	if ( bNeedMerchC )
	{
		Merch2KLineMap::const_iterator itMerchC = mapMerchKLines.find(st.m_MerchC.m_pMerch);
		if ( itMerchC==mapMerchKLines.end()
			|| NULL==itMerchC->second )
		{
			////ASSERT(0);
			return false;
		}
		const KLineInMerchArray &aKLineC = *(itMerchC->second);
		paKLineC = itMerchC->second;
		iSizeC	= aKLineC.GetSize();
		pKLineC = aKLineC.GetData(); 
	}

	//
	if ( 0>=iSizeA || 0>=iSizeB ||(bNeedMerchC ? 0>=iSizeC : false) )
	{
		return true;	// һ������Ʒ0����������Ҫ����
	}

	// �ҵ�A, B, C ʱ��Ľ���. ������������ݶ���Ҫ, �ٰ���ָ���㷨�������ݼӹ�
				
													
	//
	const CKLine* pTmpKLineA = pKLineA;
	const CKLine* pTmpKLineB = pKLineB;
	const CKLine* pTmpKLineC = pKLineC;

// 	DWORD dwTime = timeGetTime();
// 	DWORD dwTime2 = 0;
	
	// ʱ��:
	vector<long> aTimeCommon;
	aTimeCommon.reserve(iSizeA);
	// ���뱣֤����
	CombineArbitrageKLineTime(aTimeCommon,
							&aKLineA,
							&aKLineB,
							bNeedMerchC ? paKLineC : NULL);
 
 	//
 	if ( aTimeCommon.empty() )
 	{
 		// û�н���
 		//////ASSERT(0);
 		return true;
 	}

// 	dwTime2 = timeGetTime();
// 	TRACE(_T("����-�󽻼�: %d ms\r\n"), dwTime2-dwTime);
// 	dwTime = dwTime2;
 
 	//
 	int32 iSizeCombine = aTimeCommon.size();
 	aKLineCombine.SetSize(iSizeCombine);
 

	// �ȼ���ABC������
	typedef CArray<CArbitrageFormulaVariableVar, const CArbitrageFormulaVariableVar &> ArbVarCArray;
	ArbVarCArray	aVarA, aVarB, aVarC;
	// ���ߵ��վ� - ������ߵ�
	const int32 iDataTypeCount = 5;
	aVarA.SetSize(iDataTypeCount);
	aVarB.SetSize(iDataTypeCount);
	aVarC.SetSize(iDataTypeCount);
	for ( i=0; i < iDataTypeCount ; ++i )
	{
		aVarA[i].SetMinIndex(0);
		aVarA[i].SetMaxIndex(iSizeCombine-1);

		aVarB[i].SetMinIndex(0);
		aVarB[i].SetMaxIndex(iSizeCombine-1);

		aVarC[i].SetMinIndex(0);
		aVarC[i].SetMaxIndex(iSizeCombine-1);
	}

	//
	int32 iTmpSize = iSizeCombine;
	aKLineCombine.SetSize(0, iTmpSize);
	aKLineCombineA.SetSize(0, iTmpSize);
	aKLineCombineB.SetSize(0, iTmpSize);
 	for ( i = 0; i < iTmpSize; i++ )
 	{
 		// �ҵ���Ӧ�� K ��, �ϲ�
 		long lTime = aTimeCommon[i];
		CGmtTime Time(lTime);

		// A �е�λ��
 		int32 iPosA = CMerchKLineNode::QuickFindKLineByTime(aKLineA, Time);
		if ( iPosA < 0 || iPosA >= iSizeA )
		{
			// ��Ӧ�ó����������
			////ASSERT(0);
			iSizeCombine--;
			//
			continue;
		}

		const CKLine &KLineA = pTmpKLineA[iPosA];

		//  - ������ߵ�
		aVarA[0].SetValue(i, KLineA.m_fPriceOpen);
		//aVarA[1].SetValue(i, KLineA.m_fPriceHigh);
		//aVarA[2].SetValue(i, KLineA.m_fPriceLow);
		aVarA[3].SetValue(i, KLineA.m_fPriceClose);
		aVarA[4].SetValue(i, KLineA.m_fPriceAvg);

		// B �е�λ��
		int32 iPosB = CMerchKLineNode::QuickFindKLineByTime(aKLineB, Time);
		if ( iPosB < 0 || iPosB >= iSizeB )
		{
			// ��Ӧ�ó����������
			////ASSERT(0);
			iSizeCombine--;
			//
			continue;
		}

		const CKLine &KLineB = pTmpKLineB[iPosB];
		aVarB[0].SetValue(i, KLineB.m_fPriceOpen);
		//aVarB[1].SetValue(i, KLineB.m_fPriceHigh);
		//aVarB[2].SetValue(i, KLineB.m_fPriceLow);
		aVarB[3].SetValue(i, KLineB.m_fPriceClose);
		aVarB[4].SetValue(i, KLineB.m_fPriceAvg);

		// C �е�λ��
		if ( bNeedMerchC && NULL != pTmpKLineC )
		{
			if (NULL != paKLineC)
			{
				int32 iPosC = CMerchKLineNode::QuickFindKLineByTime(*paKLineC, Time);
				if ( iPosC < 0 || iPosC >= iSizeC )
				{
					// ��Ӧ�ó����������
					////ASSERT(0);
					iSizeCombine--;
					//
					continue;
				}

				const CKLine &KLineC = pTmpKLineC[iPosC];

				aVarC[0].SetValue(i, KLineC.m_fPriceOpen);
				//aVarC[1].SetValue(i, KLineC.m_fPriceHigh);
				//aVarC[2].SetValue(i, KLineC.m_fPriceLow);
				aVarC[3].SetValue(i, KLineC.m_fPriceClose);
				aVarC[4].SetValue(i, KLineC.m_fPriceAvg);
			}			
		}

		// ������֤
		if ( 0. >= KLineA.m_fPriceOpen || /*0. >= KLineA.m_fPriceHigh || 0. >= KLineA.m_fPriceLow ||*/ 0. >= KLineA.m_fPriceClose || 0. >= KLineA.m_fPriceAvg
		  || 0. >= KLineB.m_fPriceOpen || /*0. >= KLineB.m_fPriceHigh || 0. >= KLineB.m_fPriceLow ||*/ 0. >= KLineB.m_fPriceClose || 0. >= KLineB.m_fPriceAvg
		   )
		{
			////ASSERT(0);
			return false;
		}

		// ����ֻ��ռ��λ�ã��ɽ���������������ô����
		aKLineCombineA.Add(KLineA);
		aKLineCombineB.Add(KLineB);
		aKLineCombine.Add(KLineA);
 	}

	// ������Ч����
	if ( iSizeCombine != iTmpSize )
	{
		for ( i=0; i < iDataTypeCount ; ++i )
		{
			aVarA[i].SetMinIndex(0);
			aVarA[i].SetMaxIndex(iSizeCombine-1);
			
			aVarB[i].SetMinIndex(0);
			aVarB[i].SetMaxIndex(iSizeCombine-1);
			
			aVarC[i].SetMinIndex(0);
			aVarC[i].SetMaxIndex(iSizeCombine-1);
		}
	}

// 	dwTime2 = timeGetTime();
// 	TRACE(_T("����-���ݲɼ�: %d ms\r\n"), dwTime2-dwTime);
// 	dwTime = dwTime2;

	// ���������Ʒ���� - ���г��ֵ���Ʒ���п����ǲ�����Ʒ
	CArbitrage::ArbVarArray aOtherMerchVar;
	CArbitrage::ArbMerchArray aOtherMerchs;
	st.GetOtherMerchs(aOtherMerchs);
	aOtherMerchVar.reserve(aOtherMerchs.size());
	for ( i=0; i < aOtherMerchs.size() ; ++i )
	{
		CMerch *pMerchOther = aOtherMerchs[i];
		if ( aOtherMerchVar.size() <= i )
		{
			aOtherMerchVar.push_back( CArbitrageFormulaVariableVar() );
		}
		ASSERT( aOtherMerchVar.size() == i+1 );
		Merch2KLineMap::const_iterator itMerch2KLine = mapMerchKLines.find( pMerchOther );
		if ( itMerch2KLine == mapMerchKLines.end()
			|| NULL==itMerch2KLine->second )
		{
			////ASSERT(0);
			return false;	// ȱ����Ʒ����
		}

		const KLineInMerchArray &OtherKLine = *(itMerch2KLine->second);
		CArbitrageFormulaVariableVar &var = aOtherMerchVar[i];
		CString StrVarName = pMerchOther->GetMerchVarName();
		var.SetVarName(StrVarName);
		var.SetMinIndex(0);
		var.SetMaxIndex(aTimeCommon.size()-1);
		const int32 iOtherSize = OtherKLine.GetSize();
		for ( int32 j=0; j < aTimeCommon.size() ; ++j )
		{
			long lTime = aTimeCommon[j];
			CGmtTime Time(lTime);
			// ����ǰ���ֵ��䣬ǰ��û��ֵ�����ú����ֵ���
			int32 iPosKLine = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(OtherKLine, Time);
			if ( iPosKLine>=0 && iPosKLine < iOtherSize )
			{
				var.SetValue(j, OtherKLine[iPosKLine].m_fPriceClose);
			}
			else
			{
				iPosKLine = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(OtherKLine, Time);
				if ( iPosKLine>=0 && iPosKLine < iOtherSize )
				{
					var.SetValue(j, OtherKLine[iPosKLine].m_fPriceClose);
				}
				else
				{
					// �������û���κ�ֵ���ǿ�����û�취������
					ASSERT( OtherKLine.GetSize() == 0 );
					return false;
				}
			}
		}
	}
	ASSERT( aOtherMerchVar.size() == aOtherMerchs.size() );

	
	// ���㹫ʽ�Ľ��,iDataTypeCount ==1 || iDataTypeCount == 2 ������ߵ�
	
	switch(iDataTypeCount)
	{
	case 0:
	case 1:
	case 2:
		break;
	default:
		for ( i=0; i < iDataTypeCount ; ++i )
		{				
			if ( NULL != st.m_MerchA.m_pFormular )
			{
				if ( !st.FormularPrice(aVarA[i], aVarA[i], st.m_MerchA.m_pFormular, st.m_MerchA.m_pMerch, aOtherMerchVar) )
				{
					////ASSERT(0);
					return false;
				}
			}

			if ( NULL != st.m_MerchB.m_pFormular )
			{
				if ( !st.FormularPrice(aVarB[i], aVarB[i], st.m_MerchB.m_pFormular, st.m_MerchB.m_pMerch, aOtherMerchVar) )
				{
					////ASSERT(0);
					return false;
				}
			}

			if ( bNeedMerchC )
			{
				// C�Ľ���ϲ���A��
				if ( NULL != st.m_MerchC.m_pFormular )
				{
					if ( !st.FormularPrice(aVarC[i], aVarC[i], st.m_MerchC.m_pFormular, st.m_MerchC.m_pMerch, aOtherMerchVar) )
					{
						////ASSERT(0);
						return false;
					}
				}
				T_ArbForOperatorParam param;
				param.iMinIndex = aVarA[i].GetMinIndex();
				param.iMaxIndex = aVarA[i].GetMaxIndex();
				param.iVarOtherCount = 0;
				param.pVarOther = NULL;
				param.pVar1 = &aVarA[i];
				param.pVar2 = &aVarC[i];
				if ( !ArbForOperatorAdd(aVarA[i], param) )
				{
					////ASSERT(0);
					return false;
				}
			}
		}
	}
	

// 	dwTime2 = timeGetTime();
// 	TRACE(_T("����-��������: %d ms\r\n"), dwTime2-dwTime);
// 	dwTime = dwTime2;

	// ABC����Ѿ������������K��
	for ( i=0; i < iSizeCombine ; ++i )
	{
		long lTime = aTimeCommon[i];

		CKLine &KlineA = aKLineCombineA[i];
		CKLine &KlineB = aKLineCombineB[i];
		CKLine &KlineResult = aKLineCombine[i];
		KlineA.m_TimeCurrent = lTime;
		KlineB.m_TimeCurrent = lTime;

		// ���ߵ��վ�
		KlineA.m_fPriceOpen		= aVarA[0].m_aValues[i];
		//KlineA.m_fPriceHigh		= aVarA[1].m_aValues[i];
		//KlineA.m_fPriceLow		= aVarA[2].m_aValues[i];
		KlineA.m_fPriceClose	= aVarA[3].m_aValues[i];
		KlineA.m_fPriceAvg		= aVarA[4].m_aValues[i];
		// �ߵ�Ϊ���վ���ȡ
		KlineA.m_fPriceHigh		= max(KlineA.m_fPriceOpen, KlineA.m_fPriceClose);
		KlineA.m_fPriceHigh		= max(KlineA.m_fPriceHigh, KlineA.m_fPriceAvg);
		KlineA.m_fPriceLow		= min(KlineA.m_fPriceOpen, KlineA.m_fPriceClose);
		KlineA.m_fPriceLow		= min(KlineA.m_fPriceLow, KlineA.m_fPriceAvg);

		KlineB.m_fPriceOpen		= aVarB[0].m_aValues[i];
		//KlineB.m_fPriceHigh		= aVarB[1].m_aValues[i];
		//KlineB.m_fPriceLow		= aVarB[2].m_aValues[i];
		KlineB.m_fPriceClose	= aVarB[3].m_aValues[i];
		KlineB.m_fPriceAvg		= aVarB[4].m_aValues[i];
		// �ߵ�Ϊ���վ���ȡ
		KlineB.m_fPriceHigh		= max(KlineB.m_fPriceOpen, KlineB.m_fPriceClose);
		KlineB.m_fPriceHigh		= max(KlineB.m_fPriceHigh, KlineB.m_fPriceAvg);
		KlineB.m_fPriceLow		= min(KlineB.m_fPriceOpen, KlineB.m_fPriceClose);
		KlineB.m_fPriceLow		= min(KlineB.m_fPriceLow, KlineB.m_fPriceAvg);

// 		aVarA[0].GetValue(i, KlineA.m_fPriceOpen);
// 		aVarA[1].GetValue(i, KlineA.m_fPriceHigh);
// 		aVarA[2].GetValue(i, KlineA.m_fPriceLow);
// 		aVarA[3].GetValue(i, KlineA.m_fPriceClose);
// 		aVarA[4].GetValue(i, KlineA.m_fPriceAvg);
// 
// 		aVarB[0].GetValue(i, KlineB.m_fPriceOpen);
// 		aVarB[1].GetValue(i, KlineB.m_fPriceHigh);
// 		aVarB[2].GetValue(i, KlineB.m_fPriceLow);
// 		aVarB[3].GetValue(i, KlineB.m_fPriceClose);
// 		aVarB[4].GetValue(i, KlineB.m_fPriceAvg);

		// �ϲ���:
		if ( EAPDiv == st.m_eArbPrice )
		{
			// ����
			KlineResult.m_fPriceOpen	= KlineA.m_fPriceOpen  / KlineB.m_fPriceOpen;
			//KlineResult.m_fPriceHigh	= KlineA.m_fPriceHigh  / KlineB.m_fPriceHigh;
			//KlineResult.m_fPriceLow		= KlineA.m_fPriceLow   / KlineB.m_fPriceLow;
			KlineResult.m_fPriceClose	= KlineA.m_fPriceClose / KlineB.m_fPriceClose;
			KlineResult.m_fPriceAvg		= KlineA.m_fPriceAvg   / KlineB.m_fPriceAvg;
			// �ߵ�Ϊ���վ���ȡ
			KlineResult.m_fPriceHigh		= max(KlineResult.m_fPriceOpen, KlineResult.m_fPriceClose);
			KlineResult.m_fPriceHigh		= max(KlineResult.m_fPriceHigh, KlineResult.m_fPriceAvg);
			KlineResult.m_fPriceLow			= min(KlineResult.m_fPriceOpen, KlineResult.m_fPriceClose);
			KlineResult.m_fPriceLow			= min(KlineResult.m_fPriceLow, KlineResult.m_fPriceAvg);
		}
		else if ( EAPDiff == st.m_eArbPrice )
		{
			// ����
			KlineResult.m_fPriceOpen	= KlineA.m_fPriceOpen  - KlineB.m_fPriceOpen;
			//KlineResult.m_fPriceHigh	= KlineA.m_fPriceHigh  - KlineB.m_fPriceHigh;
			//KlineResult.m_fPriceLow		= KlineA.m_fPriceLow   - KlineB.m_fPriceLow;
			KlineResult.m_fPriceClose	= KlineA.m_fPriceClose - KlineB.m_fPriceClose;
			KlineResult.m_fPriceAvg		= KlineA.m_fPriceAvg   - KlineB.m_fPriceAvg;
			// �ߵ�Ϊ���վ���ȡ
			KlineResult.m_fPriceHigh		= max(KlineResult.m_fPriceOpen, KlineResult.m_fPriceClose);
			KlineResult.m_fPriceHigh		= max(KlineResult.m_fPriceHigh, KlineResult.m_fPriceAvg);
			KlineResult.m_fPriceLow			= min(KlineResult.m_fPriceOpen, KlineResult.m_fPriceClose);
			KlineResult.m_fPriceLow			= min(KlineResult.m_fPriceLow, KlineResult.m_fPriceAvg);
		}
		else
		{
			////ASSERT(0);
			return false;
		}
	}

// 	dwTime2 = timeGetTime();
// 	TRACE(_T("����-�����: %d ms\r\n"), dwTime2-dwTime);
// 	dwTime = dwTime2;
	
	return true;		 
}

void CArbitrageManage::OnAlarmsChanged(void* pData, CAlarmNotify::E_AlarmNotify eNotify)
{
	// ֻ��������, �޸�, ɾ��������Ԥ��, �޸Ķ�Ӧ�ֶε�ֵ
	if ( NULL == pData || NULL == m_pCenterManager  )
	{
		return;
	}



	if (m_aArbitrages.IsEmpty())
	{
		return;
	}

	// �������ݺ�������Ӧ�ı�������
	CArbitrage* pArbitrage = (CArbitrage*)pData;
	CAlarmConditions stCondition;

	if (NULL != m_pAlarmCenter)
	{
		m_pAlarmCenter->GetArbitrageFixAlarms(pArbitrage, stCondition);

		for ( int32 i = 0; i < m_aArbitrages.GetSize(); i++ )
		{
			if ( m_aArbitrages[i] == *pArbitrage )
			{
				//
				if ( CAlarmNotify::EANAddArbitrage == eNotify || CAlarmNotify::EANModifyArbitrage == eNotify )
				{
					m_aArbitrages[i].m_bAlarm = true;
					m_aArbitrages[i].m_fUp	   = stCondition.m_aFixedParams[0];
					m_aArbitrages[i].m_fDown  = stCondition.m_aFixedParams[1];
				}
				else if ( CAlarmNotify::EANDelArbitrage == eNotify )
				{
					m_aArbitrages[i].m_bAlarm = false;
					m_aArbitrages[i].m_fUp	   = 0.0;
					m_aArbitrages[i].m_fDown  = 0.0;
				}
			}
		}	

		//
		RebuildMerchsAllRelationShips();
		ToXml();
	}	
}
