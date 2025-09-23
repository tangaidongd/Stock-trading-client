#include "stdafx.h"
#include <math.h>
#include "TrendIndex.h"
#include "coding.h"

T_TrendIndexParam g_TrendIndexParam;

//lint --e{1566}
CNodeData::CNodeData()
{
	ClearValue(true,true);
}
CNodeData::~CNodeData()
{
}
void CNodeData::ClearValue( bool32 bClearID, bool32 bClearFlag )
{
	if ( bClearID )
	{
		m_iID = 0;
	}
	if ( bClearFlag )
	{
		m_iFlag = 0;
	}
	m_fOpen		= 0.0f;
	m_fClose	= 0.0f;
	m_fHigh		= 0.0f;
	m_fLow		= 0.0f;
	m_fAvg		= 0.0f;
	m_fVolume	= 0.0f;
	m_fAmount	= 0.0f;
	m_fHold		= 0.0f;

	m_bClrValid = false;
	m_clrNode	= 0xff000000;
}

float CNodeData::GetMax()
{
	float fMax = m_fOpen;

	fMax = fMax >= m_fClose ? fMax : m_fClose;
	fMax = fMax >= m_fHigh ? fMax : m_fHigh;
	fMax = fMax >= m_fLow ? fMax : m_fLow;

	return fMax;
}

float CNodeData::GetMin()
{
	float fMin = m_fOpen;

	fMin = fMin <= m_fClose ? fMin : m_fClose;
	fMin = fMin <= m_fHigh ? fMin : m_fHigh;
	fMin = fMin <= m_fLow ? fMin : m_fLow;

	return fMin;
}

CNodeData::CNodeData(IN const CNodeData& NodeData)
{
	Set(NodeData);//lint -sem(CNodeData::Set,initializer)
}

void CNodeData::Set(IN const CNodeData& NodeData)
{
	m_iID		= NodeData.m_iID;

	m_fOpen		= NodeData.m_fOpen;
	m_fClose	= NodeData.m_fClose;
	m_fHigh		= NodeData.m_fHigh;
	m_fLow		= NodeData.m_fLow;
	m_fAvg		= NodeData.m_fAvg;
	m_fVolume	= NodeData.m_fVolume;
	m_fAmount	= NodeData.m_fAmount;
	m_fHold		= NodeData.m_fHold;

	m_iFlag		= NodeData.m_iFlag;
	m_bClrValid = NodeData.m_bClrValid;
	m_clrNode	= NodeData.m_clrNode;
}

CIndexNodeList::CIndexNodeList()
{
	m_pNodesShow = NULL;
	m_pCurve = NULL;
}

CIndexNodeList::CIndexNodeList(const CIndexNodeList &src)
{
	*this = src;
}

CIndexNodeList::~CIndexNodeList() 
{
	m_aNodesFull.RemoveAll();
}

CIndexNodeList& CIndexNodeList::operator=(const CIndexNodeList &src)
{
	//lint --e{1555}
	if (this == &src)
	{
		return *this;
	}
	int32 iNewSize = src.m_aNodesFull.GetSize();
	m_aNodesFull.SetSize(iNewSize);

	if (iNewSize > 0)
	{
		memcpyex(m_aNodesFull.GetData(), src.m_aNodesFull.GetData(), sizeof(CNodeData) * iNewSize);
	}
	m_pNodesShow = src.m_pNodesShow;

	m_StrExtraData = src.m_StrExtraData;
	m_aStrExtraData.Copy(src.m_aStrExtraData);

	m_pCurve = src.m_pCurve;

	return *this;
}

int32 T_IndexParam::sIndexId = 0;
T_IndexParam::T_IndexParam()
{
	T_IndexParam::sIndexId++;
	id = T_IndexParam::sIndexId;
	pContent = NULL;
	pRegion = NULL;
	bMainIndex = true;

	m_iIndexShowPosInFullList = 0;
	m_iIndexShowCountInFullList = 0;
	bScreenAccroding = false;
	m_bRight = TRUE;
	memset(strIndexName, 0, sizeof(strIndexName));
}
//lint --e{1540}
T_IndexParam::~T_IndexParam()
{
	
	//>�ظ��ͷţ�ֻ��ʹ�����ڴ�
	//DEL(pContent);
}

float GetMax(float f1,float f2,float f3)
{
	float fTemp = f1 >= f2? f1 : f2;
	fTemp = fTemp >= f3 ? fTemp : f3;
	return fTemp;
}

float GetMin(float f1,float f2,float f3)
{
	float fTemp = f1 <= f2? f1 : f2;
	fTemp = fTemp <= f3 ? fTemp : f3;
	return fTemp;
}

float GetMiddle(float f1,float f2,float f3)
{
	float fValues[3];
	fValues[0] = f1;
	fValues[1] = f2;
	fValues[2] = f3;
	
	float fMax,fMin;
	SortChoice(fValues,3,fMax,fMin);
	
	return fValues[1];
}

float GetWeightedMAValue(float fMA5,float fMA8,float fMA13)
{
	if ( !BeValidFloat(fMA5) || !BeValidFloat(fMA8) || !BeValidFloat(fMA13))
	{
		return FLT_MAX;
	}
	else
	{
		float fWeighetMA = GetMA5Weight() *fMA5 + GetMA8Weight() *fMA8 + GetMA13Weight() *fMA13;
		return fWeighetMA;	
	}	
}

bool32  BeStability(const T_TrendIndexNode& Node,const T_TrendIndexNode& NodePre)
{
	if (  EHSDTCY == Node.m_eHoldState || EHSDTJC == Node.m_eHoldState )
	{
		// ��ͷ����
		if ( Node.m_fTrendIntensity - NodePre.m_fTrendIntensity >= 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if( EHSKTCY == Node.m_eHoldState || EHSKTJC == Node.m_eHoldState)
	{
		// ��ͷ����
		if ( Node.m_fTrendIntensity - NodePre.m_fTrendIntensity <= 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return true;
}

E_TrendIntensity BeStrong(const T_TrendIndexNode& Node)
{	
	if ( fabs(Node.m_fTrendIntensity) > GetStrongTrend())
	{
		return ETISTRONG;
	}
	else if ( fabs(Node.m_fTrendIntensity) >= GetWeekTrend() && fabs(Node.m_fTrendIntensity) <= GetStrongTrend())
	{
		return ETINORMAL;
	}
	else
	{
		return ETIWEEK;
	}
}

float CalcWeightMAWeightDif(int32 iPos, const CArray<T_TrendIndexNode, T_TrendIndexNode>& aNodeValues)
{
	// ����EMA��Ȩ��ֵ��(������EMA��2��ǰ������EMA�Ĳ�ֵ��*0.2+������EMA��3��ǰ������EMA�Ĳ�ֵ��*0.3+������EMA��5��ǰ������EMA�Ĳ�ֵ��*0.5 ) / ���� EMA
	int32 iSizeNode = aNodeValues.GetSize();
	
	float fReVal = FLT_MAX;
	
	if ( iSizeNode <= 0 || iPos < 5 || iPos >= iSizeNode )
	{
		return fReVal;
	}
	
	T_TrendIndexNode* pSaneIndexNode = (T_TrendIndexNode*)aNodeValues.GetData();
	if ( NULL == pSaneIndexNode )
	{
		return fReVal;
	}
	
	fReVal = (pSaneIndexNode[iPos].m_fMAWeighted - pSaneIndexNode[iPos-2].m_fMAWeighted) * (float)0.2 + (pSaneIndexNode[iPos].m_fMAWeighted - pSaneIndexNode[iPos-3].m_fMAWeighted) * (float)0.3 + (pSaneIndexNode[iPos].m_fMAWeighted - pSaneIndexNode[iPos-5].m_fMAWeighted) * (float)0.5;
	
	// 
	if ( 0.0 == pSaneIndexNode[iPos].m_fMAWeighted || !BeValidFloat(pSaneIndexNode[iPos].m_fMAWeighted))
	{
		return FLT_MAX;
	}

	fReVal = fReVal / pSaneIndexNode[iPos].m_fMAWeighted;
 
	return fReVal;
}
/*
float CalcIntensityVaule(float fMANow,float fMAPre,float fMAMax,float fMAMin)
{
	if ( !BeValidFloat(fMANow) || !BeValidFloat(fMAPre) || !BeValidFloat(fMAMax) || !BeValidFloat(fMAMin))
	{
		return FLT_MAX;
	}
	//
	if ( fMAMax == fMAMin)
	{
		return 0.0f;
	}
	else
	{
		float fReVal = 100 *(fMANow - fMAPre) / (fMAMax - fMAMin);
		return fReVal;
	}
}
*/
float CalcIntensityVauleNew( int32 iPos, float fEMAWeightedDif, T_IndexOutArray* pIndexEMA, const CArray<CKLine,CKLine>& aKLines)
{
	// ��¼2�����13��K�ߵĸ߶�֮��/13��EMA �����13�յ�K��ƽ���߶�Ϊ0���ֱֵ�� ��¼Ϊ0.05��
	float fRecord = FLT_MAX;

	int32 iSizeKLine = aKLines.GetSize();
	
	if ( NULL == pIndexEMA || iPos < 13 || iPos >= iSizeKLine )
	{
		return FLT_MAX;
	}
	
	//
	CKLine* pKline = (CKLine*)aKLines.GetData();
	if ( NULL == pKline )
	{
		return FLT_MAX;
	}
	
	//
	int32 iTemp			= 13;
	int32 iPosTemp		= iPos;	
	float fHeightTotal	= 0.0;
	
	while( iTemp )
	{
		if ( iPosTemp >=0 )
		{
			float fHeightPer = pKline[iPosTemp].m_fPriceHigh - pKline[iPosTemp].m_fPriceLow;
			
			if ( fHeightPer < 0.0)
			{
				fHeightPer *= -1;
			}
			
			fHeightTotal += fHeightPer;
		}
		else
		{
			break;
		}
		
		iPosTemp--;
		iTemp--;
	}
	
	if( 0.0 == fHeightTotal )
	{
		// �߶�Ϊ 0 , ȡĬ��ֵ
		fRecord = (float)0.05;
	}
	else
	{		
		if (NULL != pIndexEMA->index[2].pPoint)
		{
			float fEMA13 = pIndexEMA->index[2].pPoint[iPos];

			if ( !BeValidFloat(fEMA13) || 0.0 == fEMA13 )
			{
				return FLT_MAX;
			}
			else
			{
				fRecord = fHeightTotal / fEMA13;
			}
		}
		
	}
	
	float fReVal = fEMAWeightedDif * 100 / fRecord;
	
	return fReVal;
}

float CalcStabilityValue(float fIntensityNow,float fIntensityPre)
{
	if ( !BeValidFloat(fIntensityNow) || !BeValidFloat(fIntensityPre) )
	{
		return FLT_MAX;
	}
	else
	{
		return ( fIntensityNow - fIntensityPre );
	}	
}

CString GetIntensityString(const T_TrendIndexNode& Node)
{
	CString StrIntensity;

	E_TrendIntensity eTrendIntensity = BeStrong(Node);

	if( ETISTRONG == eTrendIntensity )
	{
		StrIntensity = L"ǿ";	
	}
	else if ( ETINORMAL == eTrendIntensity )
	{
		StrIntensity = L"һ��";
	}
	else if ( ETIWEEK == eTrendIntensity)
	{	
		StrIntensity = L"��";
	}

	return StrIntensity;
}

CString GetStabilityString(const T_TrendIndexNode& Node,const T_TrendIndexNode& NodePre)
{
	CString StrStability;

	bool32 bStability = BeStability(Node, NodePre);
	
	if ( bStability )
	{
		StrStability = L"�ȶ�";
	}
	else
	{
		StrStability = L"���ȶ�";
	}

	return StrStability;
}

CString GetTrendUpDownString(const T_TrendIndexNode& Node)
{
	CString StrTrend = L"";
	
	if ( EHSDTCY == Node.m_eHoldState || EHSDTJC == Node.m_eHoldState)
	{
		// ��ͷ
		StrTrend = L"��������";
	}
	else if ( EHSKTCY == Node.m_eHoldState || EHSKTJC == Node.m_eHoldState )
	{
		// ��ͷ
		StrTrend = L"�½�����";
	}
	else if ( EHSNONE == Node.m_eHoldState || EHSKCGW == Node.m_eHoldState )
	{
		// ������
		StrTrend = L"������";
	}
	
	return StrTrend;
}

CString GetActionPromptString(E_ActionPrompt eActionPrompt,COLORREF& clr)
{
	CString StrText = L"����";
	
	switch ( eActionPrompt)
	{
	case EAPNONE:
		{
			StrText = L"����ʾ";				
		}
		break;
	case EAPDTOC:
		{
			StrText = L"��ͷ����";	
			clr = RGB(255,0,0);
		}
		break;
	case EAPDTAC:
		{
			StrText = L"��ͷ�Ӳ�";		
			clr = RGB(255,0,0);
		}
		break;
	case EAPDTRC:
		{
			StrText = L"��ͷ����";		
			clr = RGB(255,0,0);
		}
		break;
	case EAPDTCC:
		{
			StrText = L"��ͷ���";		
			clr = RGB(255,0,0);
		}
		break;
	case EAPKTOC:
		{
			StrText = L"��ͷ����";		
			clr = RGB(0,255,0);
		}
		break;
	case EAPKTAC:
		{
			StrText = L"��ͷ�Ӳ�";		
			clr = RGB(0,255,0);
		}
		break;
	case EAPKTRC:
		{
			StrText = L"��ͷ����";		
			clr = RGB(0,255,0);
		}
		break;
	case EAPKTCC:
		{
			StrText = L"��ͷ���";		
			clr = RGB(0,255,0);
		}
		break;
	}
	
	return StrText;
}

CString GetActionPromptString(E_ActionPrompt eActionPrompt)
{
	CString StrText = L"����";
	
	switch ( eActionPrompt)
	{
	case EAPNONE:
		{
			StrText = L"����ʾ";				
		}
		break;
	case EAPDTOC:
		{
			StrText = L"��ͷ����";	
		}
		break;
	case EAPDTAC:
		{
			StrText = L"��ͷ�Ӳ�";		
		}
		break;
	case EAPDTRC:
		{
			StrText = L"��ͷ����";		
		}
		break;
	case EAPDTCC:
		{
			StrText = L"��ͷ���";		
		}
		break;
	case EAPKTOC:
		{
			StrText = L"��ͷ����";		
		}
		break;
	case EAPKTAC:
		{
			StrText = L"��ͷ�Ӳ�";		
		}
		break;
	case EAPKTRC:
		{
			StrText = L"��ͷ����";		
		}
		break;
	case EAPKTCC:
		{
			StrText = L"��ͷ���";		
		}
		break;
	}
	
	return StrText;
}

CString GetHoldStateString(E_HoldState eHoldState)
{
	CString StrText = L"����";
	
	switch ( eHoldState)
	{
	case EHSNONE:
		{
			StrText = L"��״̬";		
		}
		break;
	case EHSDTCY:
		{
			StrText = L"��ͷ����";		
		}
		break;
	case EHSDTJC:
		{
			StrText = L"��ͷ����";		
		}
		break;
	case EHSKTCY:
		{
			StrText = L"��ͷ����";		
		}
		break;
	case EHSKTJC:
		{
			StrText = L"��ͷ����";		
		}
		break;
	case EHSKCGW:
		{
			StrText = L"�ղֹ���";
		}
	}
	
	return StrText;
}

CString GetNodeSummary(const T_TrendIndexNode& Node,const T_TrendIndexNode& NodePre)
{
	CString StrTrend = L"";

	if ( EHSDTCY == Node.m_eHoldState || EHSDTJC == Node.m_eHoldState)
	{
		// ��ͷ
		StrTrend = L"��������";
	}
	else if ( EHSKTCY == Node.m_eHoldState || EHSKTJC == Node.m_eHoldState )
	{
		// ��ͷ
		StrTrend = L"�½�����";
	}
	else if ( EHSNONE == Node.m_eHoldState || EHSKCGW == Node.m_eHoldState )
	{
		// ������
		StrTrend = L"������";
		return StrTrend;
	}


	// 
	CString StrText;

	StrText =  GetIntensityString(Node);
	StrText += L" ";
	StrText += GetStabilityString(Node, NodePre);
	StrText += L" ";
	StrText += StrTrend;

	return StrText;

}

void SortChoice(float* aValues,int32 iLength,OUT float& fMax,OUT float& fMin)
{
	fMax = 0.0;
	fMin = 0.0;

	if ( iLength <= 0)
	{
		return;
	}

	int iMin;
	for(int i=0;i<iLength-1;i++)
	{
		iMin=i;
		for(int j=i+1;j<iLength;j++)
		{
			if(aValues[j]<aValues[iMin])
				iMin=j;
		}
	    float t=aValues[iMin];
	    aValues[iMin]=aValues[i];
	    aValues[i]=t;
	}

	//	
	fMin = aValues[0];
	fMax = aValues[iLength-1];
	//
}

bool32 GetBigCyleNode(const CTime& TimeNow,IN const CArray<T_TrendIndexNode,T_TrendIndexNode>& aBigCyleNodes,IN CTimeSpan TimeSpanBigCyle,OUT T_TrendIndexNode& NodeBigCyle)
{
	if ( aBigCyleNodes.GetSize() <= 0)
	{		
		return false;
	}
	
	CTime TimeFirst = aBigCyleNodes.GetAt(0).m_TimeKLine;	
	CTime TimeLast  = aBigCyleNodes.GetAt(aBigCyleNodes.GetSize()-1).m_TimeKLine;		
	// CTime TimeNow   = NodeNow.m_TimeKLine;

	if ( TimeNow < TimeFirst )
	{
		NodeBigCyle = aBigCyleNodes.GetAt(0);
		return true;
	}
	
	if ( TimeNow > TimeLast)
	{
		NodeBigCyle = aBigCyleNodes.GetAt(aBigCyleNodes.GetSize() - 1);
		return true;
	}

	if ( 30 == TimeSpanBigCyle.GetDays())
	{
		// �ҵ�ͬ�µľ���
		for ( int32 i = 0 ; i < aBigCyleNodes.GetSize(); i++)
		{
			CTime TimeBigCyleBegin = aBigCyleNodes.GetAt(i).m_TimeKLine;

			if ( TimeNow.GetYear() == TimeBigCyleBegin.GetYear() && TimeNow.GetMonth() == TimeBigCyleBegin.GetMonth())
			{
				NodeBigCyle = aBigCyleNodes.GetAt(i);

				return true;
			}
		}
	}
	else if ( 30*3 == TimeSpanBigCyle.GetDays())
	{
		// �ҵ�ͬ������
		
		for ( int32 i = 0 ; i < aBigCyleNodes.GetSize(); i++)
		{
			CTime TimeBigCyleBegin = aBigCyleNodes.GetAt(i).m_TimeKLine;
			
			if ( TimeNow.GetYear() == TimeBigCyleBegin.GetYear())
			{
				// ��ݱ�����ͬ,�ж�ͬһ������(ͷ��.��ʱ���д��д��������):

				// һ����
				if ( TimeNow.GetMonth() >= 1 && TimeNow.GetMonth() <= 3)
				{
					if ( TimeBigCyleBegin.GetMonth() >= 1 && TimeBigCyleBegin.GetMonth() <= 3)
					{
						NodeBigCyle = aBigCyleNodes.GetAt(i);
						return true;
					}						
				}
				
				// ������
				if ( TimeNow.GetMonth() >= 4 && TimeNow.GetMonth() <= 6)
				{
					if ( TimeBigCyleBegin.GetMonth() >= 4 && TimeBigCyleBegin.GetMonth() <= 6)
					{
						NodeBigCyle = aBigCyleNodes.GetAt(i);
						return true;
					}						
				}
				
				// ������
				if ( TimeNow.GetMonth() >= 7 && TimeNow.GetMonth() <= 9)
				{
					if ( TimeBigCyleBegin.GetMonth() >= 7 && TimeBigCyleBegin.GetMonth() <= 9)
					{
						NodeBigCyle = aBigCyleNodes.GetAt(i);
						return true;
					}						
				}
				
				// �ļ���
				if ( TimeNow.GetMonth() >= 10 && TimeNow.GetMonth() <= 12)
				{
					if ( TimeBigCyleBegin.GetMonth() >= 10 && TimeBigCyleBegin.GetMonth() <= 12)
					{
						NodeBigCyle = aBigCyleNodes.GetAt(i);
						return true;
					}						
				}				
			}
		}
	}
	else if ( 365 == TimeSpanBigCyle.GetDays() )
	{
		// �ҵ�ͬ��ľ���
		for ( int32 i = 0 ; i < aBigCyleNodes.GetSize(); i++)
		{
			CTime TimeBigCyleBegin = aBigCyleNodes.GetAt(i).m_TimeKLine;

			if ( TimeNow.GetYear() == TimeBigCyleBegin.GetYear())
			{
				NodeBigCyle = aBigCyleNodes.GetAt(i);
				  return true;
			}
		}
	}
	else
	{
		for ( int32 i = 0 ; i < aBigCyleNodes.GetSize(); i++)
		{		
			// �����ڵ�ʱ��:
			CTime TimeBigCyleBegin = aBigCyleNodes.GetAt(i).m_TimeKLine;
			CTime TimeBigCyleEnd   = TimeBigCyleBegin + TimeSpanBigCyle;
			
			if ( TimeNow >= TimeBigCyleBegin && TimeNow < TimeBigCyleEnd)
			{
				NodeBigCyle = aBigCyleNodes.GetAt(i);
				
				return true;
			}
		}
	}

	return false;
}

bool32 BeLastTradeDayInaMounth(const CGmtTime& Time)
{	
	// ...fangz0723 ����֤
	int32 iMonth = Time.GetMonth();
	int32 iMonthNext;

	if ( 12 != iMonth)
	{
		iMonthNext = iMonth + 1;
	}
	else
	{
		iMonthNext = 1;
	}
	
	// ��һ���µ�һ��:
	CGmtTime TimeNextMonthDay(Time.GetYear(),iMonthNext,1,0,0,0);
	CGmtTimeSpan TimeSpan(1,0,0,0);
	
	// ��������һ��:
	CGmtTime TimeMonthLastDay = TimeNextMonthDay - TimeSpan;

	if ( 7 == TimeMonthLastDay.GetDayOfWeek() )
	{
		// �������������,�����������ǰ��һ��
		TimeMonthLastDay -= TimeSpan;
	}
	else if ( 0 == TimeMonthLastDay.GetDayOfWeek())
	{
		// ���������,�����������ǰ������:
		TimeMonthLastDay -= TimeSpan;
		TimeMonthLastDay -= TimeSpan;
	}
	
	if ( Time == TimeMonthLastDay )
	{
		return true;
	}

	return false;
}	

bool32 BeLastTradeDayInaQuarter(const CGmtTime& Time)
{
	if ( 3 != Time.GetMonth() && 6 != Time.GetMonth() && 9 != Time.GetMonth() && 12 != Time.GetMonth() )
	{
		// ����ÿ�����ȵ����һ����.ֱ�ӷ���false
		return false;
	}

	if ( BeLastTradeDayInaMounth(Time) )
	{
		return true;
	}

	return false;
}

bool32 BeLastTradeDayInaYear(const CGmtTime& Time)
{
	if ( 12 != Time.GetMonth() )
	{
		// ����12 ��.ֱ�ӷ���false
		return false;
	}
	
	if ( BeLastTradeDayInaMounth(Time) )
	{
		return true;
	}
	
	return false;
}

bool32 StatisticaTrendValues(IN int32 iKLineNums, OUT int32& iKLineNumsReal, IN const CArray<T_TrendIndexNode,T_TrendIndexNode>& aTrendValues, OUT int32& iTradeTimes, OUT int32& iGoodTrades, OUT int32& iBadTrades, OUT float& fAccuracyRate, OUT float& fProfitability)
{
	// �� ����->��� Ϊһ�ν��׼�¼ 
	int32 iSize = aTrendValues.GetSize();
	
	if ( iSize <= 0 )
	{
		return false;
	}
	
	if ( iKLineNums <= 0 || iKLineNums > iSize )
	{
		return false;
	}

	iTradeTimes		= 0;
	iGoodTrades		= 0;
	iBadTrades		= 0;
	fAccuracyRate	= 0.0;
	fProfitability	= 0.0;
	iKLineNumsReal  = 0;

	// �� iKLineNums �����Ŀ��ֵ�:
	bool32 bFind = false;
	int32  iBeginIndex = iSize - iKLineNums;
	T_TrendIndexNode NodeBegin;
	
	if ( iBeginIndex < 0 || iBeginIndex >= iSize )
	{
		return false;
	}
	
	int32 i;
	for ( i = iBeginIndex; i > 0; i-- )
	{
		// ��ǰ��һ��
		NodeBegin = aTrendValues[i];
		
		if ( EAPDTOC == NodeBegin.m_eActionPrompt || EAPKTOC == NodeBegin.m_eActionPrompt )
		{
			// ������
			bFind = true;
			break;
		}
	}
	
	if ( !bFind )
	{
		for ( i = iBeginIndex; i < iSize; i++)
		{
			// �������
			NodeBegin = aTrendValues[i];
			
			if ( EAPDTOC == NodeBegin.m_eActionPrompt || EAPKTOC == NodeBegin.m_eActionPrompt)
			{
				// ������
				bFind = true;
				break;
			}
		}
	}
	
	if ( !bFind || i == (iSize - 1) )
	{
		return false;			
	}
	
	iBeginIndex = i;
	iKLineNumsReal = iSize - iBeginIndex;
	//

	for ( i = iBeginIndex ; i < iSize; i++)
	{
		T_TrendIndexNode* pNodeBegin = NULL; 
		T_TrendIndexNode* pNodeEnd = NULL;
		
		if ( EAPDTOC == aTrendValues.GetAt(i).m_eActionPrompt )
		{
			pNodeBegin = (T_TrendIndexNode*)&aTrendValues.GetAt(i);
			
			for ( int32 j = i; j < aTrendValues.GetSize(); j++)
			{
				if ( EAPDTCC == aTrendValues.GetAt(j).m_eActionPrompt)
				{
					// �ҵ���һ����������
					pNodeEnd = (T_TrendIndexNode*)&aTrendValues.GetAt(j);
					i = j;
					break;
				}
			}
		}
		else if ( EAPKTOC == aTrendValues.GetAt(i).m_eActionPrompt)
		{	
			pNodeBegin = (T_TrendIndexNode*)&aTrendValues.GetAt(i);
			
			for ( int32 j = i; j < aTrendValues.GetSize(); j++)
			{
				if ( EAPKTCC == aTrendValues.GetAt(j).m_eActionPrompt)
				{
					// �ҵ���һ����������
					pNodeEnd = (T_TrendIndexNode*)&aTrendValues.GetAt(j);
					
					i = j;
					break;
				}
			}
		}

		if ( NULL == pNodeBegin || NULL == pNodeEnd)
		{
			continue;
		}
		
		// ׬Ǯ���ǿ�Ǯ��?
		
		float fPriceChange = 0.0;			// ��ʽ��׵�ӯ��	
		float fProfitabilityPerTrade = 0.0; // ��ʽ��׵�ӯ���� 

		if ( EAPDTOC == pNodeBegin->m_eActionPrompt)
		{
			// ��ͷ����
			fPriceChange = pNodeEnd->m_fPrice - pNodeBegin->m_fPrice;						
		}
		else if ( EAPKTOC == pNodeBegin->m_eActionPrompt)
		{
			// ��ͷ����
			fPriceChange = pNodeBegin->m_fPrice - pNodeEnd->m_fPrice;	
		}
		
		if ( 0.0 != pNodeBegin->m_fPrice)
		{
			fProfitabilityPerTrade = fPriceChange / pNodeBegin->m_fPrice ;	//  ��ӯ/���ּ�
		}
	
		// �ܴ����ĸ���
		iTradeTimes += 1;

		if ( fPriceChange >= 0.0 )
		{
			// ׬Ǯ��
			iGoodTrades += 1;
		}
		else if ( fPriceChange < 0.0)
		{
			// ��Ǯ��		
		   iBadTrades += 1;
		}

		// ��ӯ���ʵĸ���
		fProfitability += fProfitabilityPerTrade;
	}

	// ���һ�����������������,�����ֲ������״̬. ��ô������ڵ�����¼���Ϊ��ּ�,���Ҳ��Ϊһ����������:
	T_TrendIndexNode NodeLast = aTrendValues.GetAt(iSize-1);
	
	float fPriceChange = 0.0;			// ��ʽ��׵�ӯ��	
	float fProfitabilityPerTrade = 0.0; // ��ʽ��׵�ӯ���� 
	
	if ( EHSDTCY == NodeLast.m_eHoldState || EHSDTJC == NodeLast.m_eHoldState)
	{
		// �Ƕ��,����, ��ǰ�ҵ��࿪�Ǹ��ڵ�:
		for ( i = iSize - 2; i > 0 ; i--)
		{
			T_TrendIndexNode tmpNodeBegin = aTrendValues.GetAt(i);

			if ( EAPDTOC == tmpNodeBegin.m_eActionPrompt)
			{
				if ( i - 1 >= 0 )
				{
					T_TrendIndexNode NodeBeginPre = aTrendValues.GetAt(i-1);
					
					if ( EAPDTOC == NodeBeginPre.m_eActionPrompt)
					{
						// ������ǿ�ͷ����,������ǰ����
						continue;
					}
					
					iTradeTimes += 1;
					
					fPriceChange = NodeLast.m_fPrice - tmpNodeBegin.m_fPrice;
					if ( fPriceChange >= 0.0 )
					{
						iGoodTrades += 1;	
					}
					else
					{
						iBadTrades += 1;
					}
					
					if ( 0.0 != tmpNodeBegin.m_fPrice)
					{
						fProfitabilityPerTrade = fPriceChange / tmpNodeBegin.m_fPrice;
					}
					
					fProfitability += fProfitabilityPerTrade;	

					break;
				}
				else
				{
					break;
				}
			}
		}

	}
	else if ( EHSKTCY == NodeLast.m_eHoldState || EHSKTJC == NodeLast.m_eHoldState )
	{
		// �ǿռ�,��ռ�,��ǰ�ҵ��տ��Ǹ��ڵ�:
		for ( i = iSize - 2; i > 0 ; i--)
		{
			T_TrendIndexNode tmpNodeBegin = aTrendValues.GetAt(i);

			if ( EAPKTOC == tmpNodeBegin.m_eActionPrompt)
			{
				// ����ÿ�����״̬��Ҫ��ֵ������.��������һ�����.�������ﻹҪ�ж�һ��ǰһ���ǲ���Ҳ�ǿ���
				
				if ( i - 1 >= 0 )
				{
					T_TrendIndexNode NodeBeginPre = aTrendValues.GetAt(i-1);

					if ( EAPKTOC == NodeBeginPre.m_eActionPrompt)
					{
						// ������ǿ�ͷ����,������ǰ����
						continue;
					}

					iTradeTimes += 1;
					
					fPriceChange =  tmpNodeBegin.m_fPrice - NodeLast.m_fPrice;
					
					if ( fPriceChange >= 0.0 )
					{
						iGoodTrades += 1;	
					}
					else
					{
						iBadTrades += 1;
					}
					
					if ( 0.0 != tmpNodeBegin.m_fPrice)
					{
						fProfitabilityPerTrade = fPriceChange / tmpNodeBegin.m_fPrice;
					}
					
					fProfitability += fProfitabilityPerTrade;	
					break;
				}
				else
				{
					break;
				}				
			}
		}
	} 

	if ( 0 != iTradeTimes )
	{
		fAccuracyRate = (float)iGoodTrades / iTradeTimes;
	}	
	
	return true;
}

bool32 StatisticaTrendValues2(IN const CArray<T_TrendIndexNode,T_TrendIndexNode>& aTrendValues, OUT int32& iTradeTimes, OUT int32& iGoodTrades, OUT int32& iBadTrades, OUT float& fAccuracyRate, OUT float& fProfitability)
{
	// �� ����->���� ���� ����-> ��� ���� �Ӳ�->���� ���� �Ӳ�->��� Ϊһ�ν��׼�¼
	int32 iSize = aTrendValues.GetSize();
	
	if ( iSize <= 0 )
	{
		return false;
	}
	
	iTradeTimes		= 0;
	iGoodTrades		= 0;
	iBadTrades		= 0;
	fAccuracyRate	= 0.0;
	fProfitability	= 0.0;
	
	for ( int32 i = 0 ; i < iSize; i++)
	{
		T_TrendIndexNode* pNodeBegin = NULL; 
		T_TrendIndexNode* pNodeEnd = NULL;
		
		if ( EAPDTOC == aTrendValues.GetAt(i).m_eActionPrompt || EAPDTAC == aTrendValues.GetAt(i).m_eActionPrompt || EAPKTOC == aTrendValues.GetAt(i).m_eActionPrompt || EAPKTAC == aTrendValues.GetAt(i).m_eActionPrompt)
		{
			// ���ֻ��߼Ӳ�Ϊ���.�ҵ���һ�����ǵ�ǰ������ʾ��.��Ȼ�Ǽ��ֻ��������.��Ϊһ�������Ľ��׼�¼:

			pNodeBegin = (T_TrendIndexNode*)&aTrendValues.GetAt(i);

			for ( int32 j = i ; j < iSize ; j++)
			{
				if ( j == iSize - 1)
				{
					// �����һ���ڵ���.�Ͱ���������ǽ����ڵ�:
					pNodeEnd = (T_TrendIndexNode*)&aTrendValues.GetAt(j);	
					i = j;
					break;
				}
				else
				{
					if ( aTrendValues.GetAt(j).m_eActionPrompt != pNodeBegin->m_eActionPrompt )
					{
						// һ����������:
						pNodeEnd = (T_TrendIndexNode*)&aTrendValues.GetAt(j);
						
						i = j;
						break;						
					}
				}								
			}

			if ( NULL == pNodeBegin || NULL == pNodeEnd)
			{
				continue;
			}

			// ׬Ǯ���ǿ�Ǯ��?
			
			float fPriceChange = 0.0;			// ��ʽ��׵�ӯ��	
			float fProfitabilityPerTrade = 0.0; // ��ʽ��׵�ӯ���� 
			
			if ( EAPDTOC == pNodeBegin->m_eActionPrompt || EAPDTAC == pNodeBegin->m_eActionPrompt)
			{
				// ��ͷ����
				fPriceChange = pNodeEnd->m_fPrice - pNodeBegin->m_fPrice;						
			}
			else if ( EAPKTOC == pNodeBegin->m_eActionPrompt || EAPKTAC == pNodeBegin->m_eActionPrompt)
			{
				// ��ͷ����
				fPriceChange = pNodeBegin->m_fPrice - pNodeEnd->m_fPrice;	
			}
			
			if ( 0.0 != pNodeBegin->m_fPrice)
			{
				fProfitabilityPerTrade = fPriceChange / pNodeBegin->m_fPrice ;	//  ��ӯ/���ּ�
			}
			
			// �ܴ����ĸ���
			iTradeTimes += 1;
			
			if ( fPriceChange >= 0.0 )
			{
				// ׬Ǯ��
				iGoodTrades += 1;
			}
			else if ( fPriceChange < 0.0)
			{
				// ��Ǯ��		
				iBadTrades += 1;
			}
			
			// ��ӯ���ʵĸ���
			fProfitability += fProfitabilityPerTrade;
		}
	}

	if ( 0 != iTradeTimes )
	{
		fAccuracyRate = (float)iGoodTrades / iTradeTimes;
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////////////
void InitialTrendIndexParam()
{
	// Ĭ��ֵ
	g_TrendIndexParam.m_fStrongTrend	= 4.0f; 
	g_TrendIndexParam.m_fWeekTrend		= 1.0f;
	g_TrendIndexParam.m_fOCTrend		= 10.0f;
	g_TrendIndexParam.m_fStabilityTrend = 0.15f;
	g_TrendIndexParam.m_iMa5MoveNums	= 3;
	g_TrendIndexParam.m_iMa8MoveNums	= 5;
	g_TrendIndexParam.m_iMa13MoveNums	= 8;
	g_TrendIndexParam.m_iMaxMoveNums	= 8;
	g_TrendIndexParam.m_fMA5Weight		= 0.2f;
	g_TrendIndexParam.m_fMA8Weight		= 0.3f;
	g_TrendIndexParam.m_fMA13Weight		= 0.5f;
	g_TrendIndexParam.m_iCdt5KLineNums	= 8;
	g_TrendIndexParam.m_fCdt5Times		= 3.0f;
				
	// ���ļ�

	CString StrValue;
	//
	GetPrivateProfileStringGGTong(L"param",L"strong",L"",StrValue,gkStrTrendIndexFileName);
	if ( L"" != StrValue)
	{
		std::string StrValueA;
		Unicode2MultiChar(CP_ACP, StrValue, StrValueA);			
		g_TrendIndexParam.m_fStrongTrend = (float)atof(StrValueA.c_str());
	}
	//
	GetPrivateProfileStringGGTong(L"param",L"week",L"",StrValue,gkStrTrendIndexFileName);
	if ( L"" != StrValue)
	{		
		std::string StrValueA;
		Unicode2MultiChar(CP_ACP, StrValue, StrValueA);			
		g_TrendIndexParam.m_fWeekTrend = (float)atof(StrValueA.c_str());
	}
	//	
	GetPrivateProfileStringGGTong(L"param",L"oc",L"",StrValue,gkStrTrendIndexFileName);
	if ( L"" != StrValue)
	{
		std::string StrValueA;
		Unicode2MultiChar(CP_ACP, StrValue, StrValueA);				
		g_TrendIndexParam.m_fOCTrend = (float)atof(StrValueA.c_str());
	}
	//	
	GetPrivateProfileStringGGTong(L"param",L"stability",L"",StrValue,gkStrTrendIndexFileName);
	if ( L"" != StrValue)
	{
		std::string StrValueA;
		Unicode2MultiChar(CP_ACP, StrValue, StrValueA);			
		g_TrendIndexParam.m_fStabilityTrend = (float)atof(StrValueA.c_str());
	}
	//	
	int32 iReVal = ::GetPrivateProfileInt(L"param",L"ma5",0,gkStrTrendIndexFileName);
	if ( 0 != iReVal)
	{
		g_TrendIndexParam.m_iMa5MoveNums = iReVal;
	}
	//	
	iReVal = ::GetPrivateProfileInt(L"param",L"ma8",0,gkStrTrendIndexFileName);
	if ( 0 != iReVal)
	{
		g_TrendIndexParam.m_iMa8MoveNums = iReVal;
	}
	//	
	iReVal = ::GetPrivateProfileInt(L"param",L"ma13",0,gkStrTrendIndexFileName);
	if ( 0 != iReVal)
	{
		g_TrendIndexParam.m_iMa13MoveNums = iReVal;
	}
	//	
	iReVal = ::GetPrivateProfileInt(L"param",L"max",0,gkStrTrendIndexFileName);
	if ( 0 != iReVal)
	{
		g_TrendIndexParam.m_iMaxMoveNums = iReVal;
	}
	//
	GetPrivateProfileStringGGTong(L"param",L"ma5weight",L"",StrValue,gkStrTrendIndexFileName);
	if ( L"" != StrValue)
	{
		std::string StrValueA;
		Unicode2MultiChar(CP_ACP, StrValue, StrValueA);		
		g_TrendIndexParam.m_fMA5Weight = (float)atof(StrValueA.c_str());
		
	}
	//		
	GetPrivateProfileStringGGTong(L"param",L"ma8weight",L"",StrValue,gkStrTrendIndexFileName);
	if ( L"" != StrValue)
	{	
		std::string StrValueA;
		Unicode2MultiChar(CP_ACP, StrValue, StrValueA);		
		g_TrendIndexParam.m_fMA8Weight = (float)atof(StrValueA.c_str());		
	}
	//
	GetPrivateProfileStringGGTong(L"param",L"ma13weight",L"",StrValue,gkStrTrendIndexFileName);
	if ( L"" != StrValue)
	{
		std::string StrValueA;
		Unicode2MultiChar(CP_ACP, StrValue, StrValueA);		
		g_TrendIndexParam.m_fMA13Weight = (float)atof(StrValueA.c_str());
		
	}
	//
	iReVal = ::GetPrivateProfileInt(L"param",L"cdt5klinenums",0,gkStrTrendIndexFileName);
	if ( 0 != iReVal)
	{
		g_TrendIndexParam.m_iCdt5KLineNums= iReVal;
	}
	//
	GetPrivateProfileStringGGTong(L"param",L"cdt5times",L"",StrValue,gkStrTrendIndexFileName);
	if ( L"" != StrValue)
	{
		std::string StrValueA;
		Unicode2MultiChar(CP_ACP, StrValue, StrValueA);		
		g_TrendIndexParam.m_fCdt5Times = (float)atof(StrValueA.c_str());
		
	}
	//
}

void SetTrendIndexParam(float fStrongTrend,float fWeekTrend,float fOCTrend,float fStabilityTrend,int32 iMa5MoveNums,int32 iMa8MoveNums,int32 iMa13MoveNums,float fMa5Weight,float fMa8Weight,float fMa13Weight,int32 iCdt5KLineNums,float fCdt5Times)
{
	g_TrendIndexParam.m_fStrongTrend	= fStrongTrend;
	g_TrendIndexParam.m_fWeekTrend		= fWeekTrend;
	g_TrendIndexParam.m_fOCTrend		= fOCTrend;
	g_TrendIndexParam.m_fStabilityTrend = fStabilityTrend;
	g_TrendIndexParam.m_iMa5MoveNums	= iMa5MoveNums;
	g_TrendIndexParam.m_iMa8MoveNums	= iMa8MoveNums;
	g_TrendIndexParam.m_iMa13MoveNums	= iMa13MoveNums;
	g_TrendIndexParam.m_fMA5Weight		= fMa5Weight;
	g_TrendIndexParam.m_fMA8Weight		= fMa8Weight;
	g_TrendIndexParam.m_fMA13Weight		= fMa13Weight;
	g_TrendIndexParam.m_iCdt5KLineNums	= iCdt5KLineNums;
	g_TrendIndexParam.m_fCdt5Times		= fCdt5Times;

	int32 iTemp;
	iTemp = iMa5MoveNums > iMa8MoveNums? iMa5MoveNums : iMa8MoveNums;
	iTemp = iTemp > iMa13MoveNums? iTemp : iMa13MoveNums;

	g_TrendIndexParam.m_iMaxMoveNums	= iTemp;

	// �����ļ�:
	CString StrSave;
	StrSave.Format(L"%.2f",fStrongTrend);
	::WritePrivateProfileString(L"param",L"strong",StrSave,gkStrTrendIndexFileName);
	//
	StrSave.Format(L"%.2f",fWeekTrend);
	::WritePrivateProfileString(L"param",L"week",StrSave,gkStrTrendIndexFileName);
	//
	StrSave.Format(L"%.2f",fOCTrend);
	::WritePrivateProfileString(L"param",L"oc",StrSave,gkStrTrendIndexFileName);
	//
	StrSave.Format(L"%.2f",fStabilityTrend);
	::WritePrivateProfileString(L"param",L"stability",StrSave,gkStrTrendIndexFileName);
	//
	StrSave.Format(L"%d",iMa5MoveNums);
	::WritePrivateProfileString(L"param",L"ma5",StrSave,gkStrTrendIndexFileName);
	//
	StrSave.Format(L"%d",iMa8MoveNums);
	::WritePrivateProfileString(L"param",L"ma8",StrSave,gkStrTrendIndexFileName);
	//
	StrSave.Format(L"%d",iMa13MoveNums);
	::WritePrivateProfileString(L"param",L"ma13",StrSave,gkStrTrendIndexFileName);
	//
	StrSave.Format(L"%d",iTemp);
	::WritePrivateProfileString(L"param",L"max",StrSave,gkStrTrendIndexFileName);
	//
	StrSave.Format(L"%.2f",fMa5Weight );
	::WritePrivateProfileString(L"param",L"ma5weight",StrSave,gkStrTrendIndexFileName);
	//
	StrSave.Format(L"%.2f",fMa8Weight );
	::WritePrivateProfileString(L"param",L"ma8weight",StrSave,gkStrTrendIndexFileName);
	//
	StrSave.Format(L"%.2f",fMa13Weight );
	::WritePrivateProfileString(L"param",L"ma13weight",StrSave,gkStrTrendIndexFileName);
	//
	StrSave.Format(L"%d",iCdt5KLineNums);
	::WritePrivateProfileString(L"param",L"cdt5klinenums",StrSave,gkStrTrendIndexFileName);
	//
	StrSave.Format(L"%.2f",fCdt5Times);
	::WritePrivateProfileString(L"param",L"cdt5times",StrSave,gkStrTrendIndexFileName);
	//	
}

float GetStrongTrend()
{
	return g_TrendIndexParam.m_fStrongTrend;
}

float GetWeekTrend()
{
	return g_TrendIndexParam.m_fWeekTrend;
}

float GetOCTrend()
{
	return g_TrendIndexParam.m_fOCTrend;
}

float GetStabilityTrend()
{
	return g_TrendIndexParam.m_fStabilityTrend;
}

int32 GetMA5MoveNums()
{
	return g_TrendIndexParam.m_iMa5MoveNums;
}

int32 GetMA8MoveNums()
{
	return g_TrendIndexParam.m_iMa8MoveNums;
}

int32 GetMA13MoveNums()
{
	return g_TrendIndexParam.m_iMa13MoveNums;
}

int32 GetMaxMoveNums()
{
	return g_TrendIndexParam.m_iMaxMoveNums;
}

float GetMA5Weight()
{
	return g_TrendIndexParam.m_fMA5Weight;
}

float GetMA8Weight()
{
	return g_TrendIndexParam.m_fMA8Weight;
}

float GetMA13Weight()
{
	return g_TrendIndexParam.m_fMA13Weight;
}

int32 GetCdt5KLineNums()
{
	return g_TrendIndexParam.m_iCdt5KLineNums;
}

float GetCdt5Times()
{
	return g_TrendIndexParam.m_fCdt5Times;
}