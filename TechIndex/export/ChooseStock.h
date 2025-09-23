#ifndef _H_CHOOSE_STOCK_
#define _H_CHOOSE_STOCK_

//#include "IoViewShare.h"

//////////////////////////////////////////////////////////////////////////

enum E_StockSelCondition
{
	ESSC0 = 0,
	ESSC1,
	ESSC2,
	ESSC3,
	ESSC4,
	ESSC5,
	ESSC6,
	ESSC7,
	ESSCCOUNT,
};

// ��Ͽ����г�������: ö����,����,��������,��ע

typedef struct T_StockSelCondition
{
    E_StockSelCondition    m_eStockSelCondition;
    CString                m_StrName;
    int32                  m_iParamNums;
    CString                m_StrSummary;
	
	// ���캯��
    T_StockSelCondition() 
	{
		m_eStockSelCondition = ESSCCOUNT;
		m_StrName			 = L"";
		m_iParamNums		 = 1;
		m_StrSummary		 = L"";
	};
	
    T_StockSelCondition(const E_StockSelCondition & eStockSelCondition,const CString & StrName,const int32 & iParamNums,const CString & StrSummary)
    {
        m_eStockSelCondition = eStockSelCondition;
        m_StrName            = StrName;
        m_iParamNums         = iParamNums;
        m_StrSummary         = StrSummary;
    }

// 	// ���ز�����
// 	const T_StockSelCondition & operator= (const T_StockSelCondition & ConditionSrc)
// 	{
// 		m_eStockSelCondition = ConditionSrc.m_eStockSelCondition;
// 		m_StrName			 = ConditionSrc.m_StrName;
// 		m_iParamNums		 = ConditionSrc.m_iParamNums;
// 		m_StrSummary		 = ConditionSrc.m_StrSummary;
// 
// 		return *this;
// 	}

	bool32 operator== ( const T_StockSelCondition & ConditionSrc) const
	{
		if (   m_eStockSelCondition == ConditionSrc.m_eStockSelCondition
			&& m_StrName			== ConditionSrc.m_StrName
			&& m_iParamNums			== ConditionSrc.m_iParamNums
			&& m_StrSummary			== ConditionSrc.m_StrSummary)
		{
			return true;
		}
		return false;
	}

}T_StockSelCondition,*PStockSelCondition;

const T_StockSelCondition KaSelStockConditonObject[] =
{
	T_StockSelCondition(ESSC0,L"UPN      -����N��",				1,	L"���������������ʾ�˶෽�Ϲ��ĺۼ�."),
	T_StockSelCondition(ESSC1,L"DOWNN    -����N��",				1,	L"��������ĵ�����ʾ�˿շ��´�ĺۼ�."),
	T_StockSelCondition(ESSC2,L"BIAS���� -��������������ѡ��",	2,	L"���÷�ע��."),
	T_StockSelCondition(ESSC3,L"KDJ����  -KDJ��������ѡ��",		3,	L"���÷�ע��."),
	T_StockSelCondition(ESSC4,L"MSTART   -�糿֮��",			0,	L"K��ģʽ�糿֮��,ָʾ���׷���."),
	T_StockSelCondition(ESSC5,L"W&R����  -����ָ����������ѡ��",2,	L"���÷�ע��"),
	T_StockSelCondition(ESSC6,L"MTM����  -MTM��������ѡ��",		1,	L"���÷�ע��"),
};

const KiStockConditionObjectNum = sizeof(KaSelStockConditonObject)/sizeof(T_StockSelCondition);

// ���ռ����б��е�����: ��Ͽ��е�����,��������,����

typedef struct T_StockConditionFinal
{
	T_StockSelCondition		m_SelCondition;
	CArray<int32,int32&>	m_aParams;
	E_NodeTimeInterval		m_eCycle;

	// ���캯��

	T_StockConditionFinal()
	{		
		m_aParams.RemoveAll();
		m_eCycle = ENTIDay;
	}

	// �������캯��

	T_StockConditionFinal(const T_StockConditionFinal & ConditionSrc)
	{
		if ( ConditionSrc == *this)
		{
			return ;
		}
		
		m_SelCondition = ConditionSrc.m_SelCondition;
		
		m_aParams.RemoveAll();
		m_aParams.Copy(ConditionSrc.m_aParams);
		
		m_eCycle = ConditionSrc.m_eCycle;		
	}	
	
	// ���ز�����

	const T_StockConditionFinal& operator = (const T_StockConditionFinal & ConditionSrc)
	{
		if ( ConditionSrc == *this)
		{
			return *this;
		}

		m_SelCondition = ConditionSrc.m_SelCondition;
		
		m_aParams.RemoveAll();
		m_aParams.Copy(ConditionSrc.m_aParams);
		
		m_eCycle = ConditionSrc.m_eCycle;

		return *this;
	}


	bool32 operator== ( const T_StockConditionFinal & ConditionSrc) const
	{
		if (   m_SelCondition		== ConditionSrc.m_SelCondition	
			&& m_aParams.GetSize()  == ConditionSrc.m_aParams.GetSize()
			&& m_eCycle				== m_eCycle)								
		{			
			int32 iSize = m_aParams.GetSize();
			for (int32 i = 0 ; i < iSize; i++)
			{
				if ( m_aParams[i] != ConditionSrc.m_aParams[i])
				{
					return false;
				}
			}
			return true;
		}
		return false;
	}

}T_StockConditionFinal,*PStockConditionFinal;

#endif