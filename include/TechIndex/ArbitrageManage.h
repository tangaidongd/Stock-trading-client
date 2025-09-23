// ArbitrageManage.h: interface for the CArbitrageManage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_ARBITRAGEMANAGE_H_)
#define _ARBITRAGEMANAGE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include <set>
#include <map>
#include <vector>
#include "AlarmNotify.h"
#include "ShareFun.h"
#include "TechExport.h"
#include "EngineCenterBase.h"


using namespace std;

class CAlarmCenter;
//class CMerch;
//class CViewData;
class TiXmlElement;
class CArbitrageFormula;
class CArbitrageFormulaVariableVar;

// ��������
typedef enum E_Arbitrage
{
	EABTwoMerch = 0,		// ����, ������Ʒ
	EABThreeMerch,			// ��ʽ, ������Ʒ

	EABCount,
		
}E_Arbitrage;

// �۲�����
typedef enum E_ArbPrice
{
	EAPDiv = 0,				// ����
	EAPDiff,				// ����

	EAPCount,				
		
}E_ArbPrice;

// �������۱�ͷ����
typedef enum E_ArbitrageHead
{
	EAHName = 0,		// ����
	EAHPriceDifAccord,	// �Լۼ۲�
	EAHPriceDifQuote,	// ���ۼ۲�
	EAHPriceDifNew,		// ���¼۲�
	EAHRisePriceDif,	// �۲��ǵ�
	EAHPriceMerchA,		// ��Ʒ A ���¼�
	EAHPriceMerchB,		// ��Ʒ B ���¼�
	EAHPriceMerchC,		// ��Ʒ C ���¼�
	EAHPriceDifOpen,	// ���̼۲�
	EAHPriceDifPreClose,// ���ռ۲�
	EAHPriceDifPreAvg,	// ���۲�
	EAHPriceDifAvg,		// ����۲�

	EAHCount,		
		
}E_ArbitrageHead;

class CArbitrageManage;

// �����ṹ
class EXPORT_CLASS CArbitrage
{
public:
	typedef vector<CArbitrageFormulaVariableVar> ArbVarArray;
	typedef vector<CMerch *>	ArbMerchArray;

	typedef struct T_ArbMerch
	{
		CMerch				*m_pMerch;
		ArbMerchArray		m_aOtherMerch;				// ����������Ʒ���������κ���Ʒ
		CArbitrageFormula	*m_pFormular;				// ��ʼ��NULL����㿽��������delete

		T_ArbMerch()
		{
			m_pMerch = NULL;
			m_pFormular = NULL;
		}
		~T_ArbMerch();
		T_ArbMerch(const T_ArbMerch &other);
		const T_ArbMerch &operator=(const T_ArbMerch &other);

		bool operator==(const T_ArbMerch &other) const;
		bool operator!=(const T_ArbMerch &other) const { return !(*this==other); }
		bool operator<(const T_ArbMerch &other) const;

		void Copy(const T_ArbMerch &other);
	}T_ArbMerch;

public:
	CArbitrage();
	~CArbitrage();

	bool operator==(const CArbitrage& stOther) const;
	bool operator<(const CArbitrage& stOther) const;
	CArbitrage& operator=(const CArbitrage& stOther);
	CArbitrage(const CArbitrage& stOther);

	// ������
	bool32	FromXml(TiXmlElement* pElement,CAbsCenterManager* pData);
	bool32	LoadMerchFromXml(TiXmlElement *pElement, OUT T_ArbMerch &arbMerch,CAbsCenterManager* pData);
	
	// �����ļ�
	CString ToXml();
	CString SaveMerchToXml(const T_ArbMerch &arbMerch) const;

	// ȡ����ṹ������ʾ������
	CString	GetShowName() const;

	// ȡ���ּ۲�
	float	GetPrice(E_ArbitrageHead eHeadType) const;

	// ��۸�
	float	FormularPrice(float fPriceOriginal, CArbitrageFormula* pFormular, CMerch *pMerchToCalc, E_ArbitrageHead eHead) const;
	// ����K��
	//  varResult - OUT������Ľ����
	//  varIn - IN, ����Ʒ��K�߲���
	//  pFormula - IN, ��ʽ
	//  pMerchToCalc - IN, ����Ʒ
	//  aOtherVars - IN, ������Ʒ��K�߲���, ��Щ���������е�ÿһ�����������趨���ƣ�������Ϊ��Ӧ�Ĳ�����Ʒ��
	bool32	FormularPrice(OUT CArbitrageFormulaVariableVar &varResult
						, INOUT CArbitrageFormulaVariableVar &varIn
						, CArbitrageFormula *pFormula
						, CMerch *pMerchToCalc
						, const ArbVarArray &aOtherVars) const;

	bool32	IsNeedCMerch() const;

	// �ϲ��Ĳ�����Ʒ
	int32	GetOtherMerchs(OUT ArbMerchArray &aMerchs, bool32 bContainABC=false) const;
	int32	GetOtherMerchsCount(bool32 bContainABC=false) const;

	float	GetOtherMerchPrice(CMerch *pOtherMerch, E_ArbitrageHead eHead) const;

public:

// 	CMerch*		m_pMerchA;
// 	CMerch*		m_pMerchB;
// 	CMerch*		m_pMerchC;

	T_ArbMerch	m_MerchA;
	T_ArbMerch	m_MerchB;
	T_ArbMerch	m_MerchC;
	
	E_Arbitrage	m_eArbitrage;
	E_ArbPrice	m_eArbPrice;

	int32		m_iSaveDec;
	// Ԥ����ص� xml �ֶ�, ���������ﱣ�洦���Щ
public:
	bool32		m_bAlarm;				// �Ƿ�Ԥ��	
	float		m_fUp;					// ���Ƽ�
	float		m_fDown;				// ���Ƽ�

};

// ֪ͨ��
class CArbitrageNotify
{
public:
	// ��������
	virtual void OnArbitrageAdd(const CArbitrage& stArbitrage)	= 0;

	// ɾ������
	virtual void OnArbitrageDel(const CArbitrage& stArbitrage)	= 0;

	// �޸�����
	virtual void OnArbitrageModify(const CArbitrage& stArbitrageOld, const CArbitrage& stArbitrageNew) = 0;
};

// ��������������
class EXPORT_CLASS CArbitrageManage : public CAlarmNotify
{
public:
	typedef CArray<CKLine, CKLine >	KLineInMerchArray;
	typedef vector<KLineInMerchArray *> KLineInMerchPtrArray;
	typedef map<CMerch *, KLineInMerchArray *> Merch2KLineMap;
	typedef set<CArbitrage> ArbitrageSet;

private:
	typedef CArray<CArbitrage, CArbitrage&> ArbitrageArray;
	typedef map<CMerch*, ArbitrageSet> MerchArbitrageMap;
	typedef CArray<CKLine, CKLine&> KLineArray;

public:	
	virtual ~CArbitrageManage();

public:
	// �����иı�
	virtual void OnAlarmsChanged(void* pData, CAlarmNotify::E_AlarmNotify eNotify);
	
	// �ﵽԤ������, ������
	virtual void Alarm(CMerch* pMerch, const CString& StrPrompt){}
	
	// ������
	virtual void AlarmArbitrage( CArbitrage* pArbitrage, const CString& StrPrompt){}

public:
	// ����
	static CArbitrageManage*	Instance();

	static void		DelInstance();

	bool			Initialize(CAbsCenterManager* pData,CAlarmCenter* pAlarm);

	//
	bool32		BeLoadXmlSuccessed() const	{ return m_bFromXml; }
	void		ReLoadXml() { FromXml(); }

	// ֪ͨ
	void		AddNotify(CArbitrageNotify* p);

	void		DelNotify(CArbitrageNotify* p);

	// ��������
	void		AddArbitrage(const CArbitrage& stArbitrage);

	void		DelArbitrage(const CArbitrage& stArbitrage);

	void		ModifyArbitrage(const CArbitrage& stArbitrageOld, const CArbitrage& stArbitrageNew);

	// ��ȡ������������Ʒ
	void		GetArbitrageMerchs(OUT CArray<CMerch*, CMerch*>& aMerchs);

	// ��ȡ���е���������
	void		GetArbitrageDatas(OUT CArray<CArbitrage, CArbitrage&>& aArbitrages);
	const CArray<CArbitrage, CArbitrage&>& GetArbitrageDataRef() { return m_aArbitrages; }

	// ��ȡ��Ʒ��Ӧ������ģ��
	void		GetArbitrageDatasByMerch(IN CMerch* pMerch, OUT CArray<CArbitrage, CArbitrage&>& aArbitrages);
	void		GetArbitrageDatasByMerch(IN CMerch* pMerch, OUT ArbitrageSet	&sArbitrages);

	// �ϳ�K ��, 
// 	static bool32 CombineArbitrageKLine(IN const CArbitrage& st,								// ����ģ��, ����ȡ����ָ���㷨
// 										IN const CArray<CKLine, CKLine>& aKLineA,				// ��Ʒ A ��ԭʼ K ��
// 										IN const CArray<CKLine, CKLine>& aKLineB,				// ��Ʒ B ��ԭʼ K ��
// 										IN const CArray<CKLine, CKLine>& aKLineC,				// ��Ʒ C ��ԭʼ K ��
// 										IN const KLineInMerchPtrArray	&aOtherKLinePtrs,		// ����������Ʒ��ԭʼK��
// 										OUT CArray<CKLine, CKLine>& aKLineCombineA,				// �ϳɺõ� A ��
// 										OUT CArray<CKLine, CKLine>& aKLineCombineB,				// �ϳɺõ� B ��
// 										OUT CArray<CKLine, CKLine>& aKLineCombine);				// �ϳɺõ����� K ��
	static bool32 CombineArbitrageKLine(IN const CArbitrage& st,								// ����ģ��, ����ȡ����ָ���㷨
										IN const Merch2KLineMap &mapMerchKLines,				// ���������й���Ʒ��K������
										OUT CArray<CKLine, CKLine>& aKLineCombineA,				// �ϳɺõ� A ��
										OUT CArray<CKLine, CKLine>& aKLineCombineB,				// �ϳɺõ� B ��
										OUT CArray<CKLine, CKLine>& aKLineCombine);				// �ϳɺõ����� K ��

	// �ϲ���ͬʱ��
	static bool32 CombineArbitrageKLineTime(OUT vector<long> &aCommTimes,						// ��ͬʱ��
											IN const KLineInMerchArray *pKLineA,				// A�ߣ�����ΪNULL
											IN const KLineInMerchArray *pKLineB,				// B�ߣ�����ΪNULL
											IN const KLineInMerchArray *pKLineC);				// C�ߣ���ΪNULL

private:
	CArbitrageManage();

	// Xml ����
	bool32		FromXml();

	bool32		ToXml();

	// ����������Ʒ���ϺͶ�Ӧ��ϵ
	void		RebuildMerchsAllRelationShips();

	// ���¶�Ӧ��ϵ
	void		UpdateMap(CMerch* pMerch, const CArbitrage& stArbitrage);


private:
	// 
	bool32			m_bFromXml;

	// ��������
	CAbsCenterManager*		m_pCenterManager;

	//Ԥ������
	CAlarmCenter*   m_pAlarmCenter; 

	// ֪ͨ����
	CArray<CArbitrageNotify*, CArbitrageNotify*>	m_apNotifys;

	// ����ģ������
	ArbitrageArray	m_aArbitrages;

	// �Լ���ָ��
	static CArbitrageManage*	m_pThis;

	// ���е���Ʒ����
	set<CMerch*>	m_apMerchsAll;

	// ÿ����Ʒ��Ӧ������ģ�ͼ���
	MerchArbitrageMap	m_mapMerchArbitrage;

};

#endif // !defined(_ARBITRAGEMANAGE_H_)
