#include "StdAfx.h"
#include "SelectStockStruct.h"

T_IndexOut::T_IndexOut()
{
	iColor = 0;
	iThick = 0;
	iType = ESSTCondition;
	iPointNum = 0;
	iTypeEx = 0;
	pPoint = NULL;
	pPointExtra1 = NULL;
	pPointExtra2 = NULL;
	pPointExtra3 = NULL;
	iKlineDrawType = 0xffff/*CIndexDrawer::EIKDSByMainKline*/;	// 跟主图一样的绘制默认
}

T_IndexOut::~T_IndexOut()
{
	if ( NULL != pPoint )
	{
		DEL_ARRAY(pPoint);		
	}

	if ( NULL != pPointExtra1 )
	{
		DEL_ARRAY(pPointExtra1);		
	}

	DEL_ARRAY(pPointExtra2);
	DEL_ARRAY(pPointExtra3);

}

CString T_CustomCdtCell::GetName()
{
	CString StrName; 
	if ( 1000 == m_iIndex )
	{
		StrName = s_KaValues2[0];
	}
	else if ( 1001 == m_iIndex )
	{
		StrName = s_KaValues2[1];
	}
	else if ( 1002 == m_iIndex )
	{
		StrName = s_KaValues2[2];
	}
	else
	{
		StrName = CReportScheme::Instance()->GetReportHeaderCnName((CReportScheme::E_ReportHeader)m_iIndex);
	}
	
	return StrName;
}

CString T_CustomCdtCell::GetPostName()
{
	if ( -1 == m_iIndex || -2 == m_iIndex || -3 == m_iIndex )
	{
		return L"";
	}
	
	int32 i;
	for ( i = 0; i < s_KiCountsValues0; i++ )
	{
		if ( m_iIndex == (int32)s_KaValues0[i].m_eHeadType )
		{
			return s_KaValues0[i].m_StrPostName;
		}
	}
	
	//
	for ( i = 0; i < s_KiCountsValues1; i++ )
	{
		if ( m_iIndex == (int32)s_KaValues1[i].m_eHeadType )
		{
			return s_KaValues1[i].m_StrPostName;
		}
	}
	
	return L"";
}

CString T_CustomCdtCell::GetString()
{
	CString StrReturn;
	
	CString StrName = GetName();
	
	CString StrCmp;
	if ( -1 == m_iCmp )
	{
		StrCmp = L"<";
	}
	else if ( 0 == m_iCmp )
	{
		StrCmp = L"=";
	}
	else if ( 1 == m_iCmp )
	{
		StrCmp = L">";
	}
	else
	{
		////ASSERT(0);
	}
	
	//
	StrReturn.Format(L"%s%s %.2f %s", StrName.GetBuffer(), StrCmp.GetBuffer(), m_fValue, GetPostName().GetBuffer());
	
	//
	return StrReturn;
}

bool32 GetTimeIntervalInfo(int32 iMinUser, int32 iDayUser, E_NodeTimeInterval eNodeTimeInterval, E_NodeTimeInterval &eNodeTimeIntervalCompare, E_KLineTypeBase &eKLineTypeCompare,int32 &iScale)
{
	eNodeTimeIntervalCompare = ENTIDay;
	iScale = 1;

	bool32 bRet = true;
	// 
	switch (eNodeTimeInterval)
	{
	case ENTIMinute:
		{
			eNodeTimeIntervalCompare	= ENTIMinute;
			eKLineTypeCompare			= EKTBMinute;
			iScale						= 1;
		}
		break;
	case ENTIMinute5:
		{
			eNodeTimeIntervalCompare	= ENTIMinute5;
			eKLineTypeCompare			= EKTB5Min;
			iScale						= 1;
		}
		break;
	case ENTIMinute15:
		{
			eNodeTimeIntervalCompare	= ENTIMinute5;
			eKLineTypeCompare			= EKTB5Min;
			iScale						= 3;
		}			
		break;
	case ENTIMinute30:
		{
			eNodeTimeIntervalCompare	= ENTIMinute5;
			eKLineTypeCompare			= EKTB5Min;
			iScale						= 6;
		}
		break;
	case ENTIMinute60:
		{
			eNodeTimeIntervalCompare	= ENTIMinute60;
			eKLineTypeCompare			= EKTBHour;
			iScale						= 1;
		}
		break;
	case ENTIMinuteUser:			// 自定义周期
		{
			if ( iMinUser <= 0)		// 错误的周期
				return false;

			if ( iMinUser % 60 == 0)
			{
				eNodeTimeIntervalCompare= ENTIMinute60;
				eKLineTypeCompare		= EKTBHour;
				iScale					= iMinUser / 60;
			}
			else if ( iMinUser % 5 == 0)
			{
				eNodeTimeIntervalCompare= ENTIMinute5;
				eKLineTypeCompare		= EKTB5Min;
				iScale					= iMinUser / 5;
			}
			else
			{
				eNodeTimeIntervalCompare= ENTIMinute;
				eKLineTypeCompare		= EKTBMinute;
				iScale					= iMinUser;
			}
		}
		break;
	case ENTIDay:
		{
			eNodeTimeIntervalCompare	= ENTIDay;
			eKLineTypeCompare			= EKTBDay;
			iScale						= 1;
		}
		break;
	case ENTIWeek:
		{
			eNodeTimeIntervalCompare	= ENTIDay;
			eKLineTypeCompare			= EKTBDay;
			iScale						= 6;
		}
		break;
	case ENTIMonth:
		{
			eNodeTimeIntervalCompare	= ENTIMonth;
			eKLineTypeCompare			= EKTBMonth;
			iScale						= 1;
		}
		break;
	case ENTIQuarter:
		{
			eNodeTimeIntervalCompare	= ENTIMonth;
			eKLineTypeCompare			= EKTBMonth;
			iScale						= 3;
		}
		break;
	case ENTIYear:
		{
			eNodeTimeIntervalCompare	= ENTIMonth;
			eKLineTypeCompare			= EKTBMonth;
			iScale						= 12;
		}
		break;
	case ENTIDayUser:
		{
			if ( iDayUser <= 0)		// 错误的周期
				return false;

			eNodeTimeIntervalCompare	= ENTIDay;
			eKLineTypeCompare			= EKTBDay;
			iScale						= iDayUser;
		}
		break;		
	default:
			bRet = false;
		break; 
	}

	return bRet;
}