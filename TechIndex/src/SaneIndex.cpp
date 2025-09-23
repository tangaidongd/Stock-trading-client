#include "StdAfx.h"
#include "coding.h"
#include "SaneIndex.h"
#include "trendindex.h"
#include "CFormularCompute.h"
#include "LogFunctionTime.h"
#include "formulaengine.h"
#include "mmsystem.h"
#include "EngineCenterBase.h"

extern CAbsCenterManager *g_pCenterManager;

static CString GetServerTimeString()
{
	CString StrTime;
	if (!g_pCenterManager)
	{
		return StrTime; 
	}

	CGmtTime Time = g_pCenterManager->GetServerTime();
		
	int32 iHour = Time.GetHour() + 8;
	if ( iHour >= 24 )
	{
		iHour -= 24;
	}
	
	StrTime.Format(L"%04d-%02d-%02d %02d:%02d:%02d", Time.GetYear(), Time.GetMonth(), Time.GetDay(), iHour, Time.GetMinute(), Time.GetSecond());	
	return StrTime;
} 

static CString NodeToDebugString(const T_SaneIndexNode& Node)
{
	CString StrNode;
	StrNode.Format(L"State = %d MarketInfo = %d Intensity = %.2f Stability = %.2f EmaWeighted = %.2f TimeKLine = %I64d TimeAction = %I64d Price = %.2f TrendValue = %.2f",
				   Node.m_eSaneIndexState, Node.m_eSaneMarketInfo, Node.m_fIntensity, Node.m_fStability, Node.m_fLineData, Node.m_TimeKLine.GetTime(), Node.m_TimeAction.GetTime(), Node.m_fPrice, Node.m_fTrendValue);

	return StrNode;
}

static void DebugLog(const CString& StrLog)
{	
	//
	FILE* pFile = fopen("c:\\IndexDebug.txt", "aw+");
	
	if ( NULL == pFile )
	{
		return;
	}
	
	std::string StrLogA;
	Unicode2MultiChar(CP_ACP, StrLog, StrLogA);		
	
	fprintf(pFile, "%s \r\n", StrLogA.c_str());
	fclose(pFile);
}

//
void split_string(OUT std::vector<std::string>& out, IN const char* in, int delim)
{
	out.clear();
	while (*in)
	{
		const char* next = strchr(in, delim);
		if (next)
		{
			std::string tmp(in, next);
			out.push_back(tmp);
			in = next + 1;
		}
		else
		{
			out.push_back(in);
			break;
		}		
	}
}

int32 aOPen[20]		= {-1};
int32 aClose[20]	= {-1};
int32 aOpen2[20]	= {-1};
int32 aClose2[20]	= {-1};

static bool32 bNeed = true;

void GetCondition()
{
	FILE* pFile = fopen("debugzhibiao.txt", "r+");
	
	if ( NULL != pFile )
	{
		char strOpen[1024];
		memset(strOpen, 0, 1024);
		
		fgets(strOpen, 1024, pFile);
		
		// 多头开仓
		std::vector<std::string> aOut;
		split_string(aOut, strOpen, ',');
		
		//	
		{
			for ( int32 i = 0; i < aOut.size(); i++ )
			{
				string strCondition = aOut[i];
				int32 iCdtn = atoi(strCondition.c_str());
				
				aOPen[i] = iCdtn;
			}
		}
		
		// 多头清仓
		{
			//
			char strClose[1024];
			memset(strClose, 0, 1024);
			fgets(strClose, 1024, pFile);
			
			std::vector<std::string> aOut2;
			split_string(aOut2, strClose, ',');
			
			for ( int32 i = 0; i < aOut2.size(); i++ )
			{
				string strCondition = aOut2[i];
				int32 iCdtn = atoi(strCondition.c_str());
				
				aClose[i] = iCdtn;
			}
		}
		
		// 空头开仓
		{
			//
			char strOpen2[1024];
			memset(strOpen2, 0, 1024);
			fgets(strOpen2, 1024, pFile);
			
			std::vector<std::string> aOut3;
			split_string(aOut3, strOpen2, ',');
			
			for ( int32 i = 0; i < aOut3.size(); i++ )
			{
				string strCondition = aOut3[i];
				int32 iCdtn = atoi(strCondition.c_str());
				
				aOpen2[i] = iCdtn;
			}
		}

		// 空头清仓
		{
			//
			char strClose2[1024];
			memset(strClose2, 0, 1024);
			fgets(strClose2, 1024, pFile);
			
			std::vector<std::string> aOut4;
			split_string(aOut4, strClose2, ',');
			
			for ( int32 i = 0; i < aOut4.size(); i++ )
			{
				string strCondition = aOut4[i];
				int32 iCdtn = atoi(strCondition.c_str());
				
				aClose2[i] = iCdtn;
			}
		}

		fclose(pFile);
	}
	else
	{
		bNeed = false;		
	}
}

bool32 BeNeed(int32 iCondition, int32 iIndex = 1)
{
	if ( !bNeed )
	{
		return true;
	}

	if ( 1 == iIndex )
	{
		for ( int32 i = 0; i < 20; i++ )
		{
			if ( iCondition == aOPen[i] )
			{
				return true;
			}
		}
	}
	else if ( 2 == iIndex )
	{
		for ( int32 i = 0; i < 20; i++ )
		{
			if ( iCondition == aClose[i] )
			{
				return true;
			}
		}
	}
	else if ( 3 == iIndex )
	{
		for ( int32 i = 0; i < 20; i++ )
		{
			if ( iCondition == aOpen2[i] )
			{
				return true;
			}
		}
	}
	else if ( 4 == iIndex )
	{
		for ( int32 i = 0; i < 20; i++ )
		{
			if ( iCondition == aClose2[i] )
			{
				return true;
			}
		}
	}
	return false;
}

struct T_Initial
{
	T_Initial()
	{
		GetCondition();
	}
};

T_Initial st;

//

// 每个开仓最多对应 4 个清仓条件
static const  int32 s_kiMaxCCConditions	 = 12;

// 股票的多头开仓对应的多个多头清仓条件
static const  int32 s_kaiDtocToDtccMapStock[][s_kiMaxCCConditions] =
{
	{1,2,3,4,5,6,7,8,9,10,11,12},
	{1,2,3,4,5,6,7,8,9,10,11,12},
	{1,2,3,4,5,6,7,8,9,10,11,12},
	{1,2,3,4,5,6,7,8,9,10,11,12},
	{1,2,3,4,5,6,7,8,9,10,11,12},
};

// 股票的多头开仓的条件个数
const int32 s_kiDtocConditionNumsStock = sizeof(s_kaiDtocToDtccMapStock) / (s_kiMaxCCConditions * sizeof(int32)) ;

// 期货的多头开仓对应的多个多头清仓条件
static const  int32 s_kaiDtocToDtccMapFuture[][s_kiMaxCCConditions] =
{
	{1,2,3,4,5,6,7,8,9,10,11,12},
	{1,2,3,4,5,6,7,8,9,10,11,12},
	{1,2,3,4,5,6,7,8,9,10,11,12},
	{1,2,3,4,5,6,7,8,9,10,11,12},
	{1,2,3,4,5,6,7,8,9,10,11,12},
};

// 期货的多头开仓的条件个数
const int32 s_kiDtocConditionNumsFuture = sizeof(s_kaiDtocToDtccMapStock) / (s_kiMaxCCConditions * sizeof(int32));

// 空头开仓对应的多个空头清仓条件
static const  int32 s_kaiKtocToKtccMap[][s_kiMaxCCConditions] =
{
	{1,2,3,4,5,6,7,8,9,10,11,12},
	{1,2,3,4,5,6,7,8,9,10,11,12},
	{1,2,3,4,5,6,7,8,9,10,11,12},
	{1,2,3,4,5,6,7,8,9,10,11,12},
	{1,2,3,4,5,6,7,8,9,10,11,12},
};

// 多头开仓的条件个数
const int32 s_kiKtocConditionNums = sizeof(s_kaiKtocToKtccMap) / (s_kiMaxCCConditions * sizeof(int32)) ;


//
CString GetNodeTip(bool32 bFuture, int32 iSaveDec, const T_SaneIndexNode& Node)
{
	CString StrTip;
	
	if ( iSaveDec < 0 )
	{
		iSaveDec = 0;
	}
	//
	CString StrPrompt = GetNodeStateString(bFuture, Node);

	CString StrFormat;
	StrFormat.Format(L"%%.%df", iSaveDec);
	CString StrPrice;
	StrPrice.Format(StrFormat, Node.m_fPrice);

	//	
	if ( ESISDTOC == Node.m_eSaneIndexState || ESISKTOC == Node.m_eSaneIndexState || ESISDTCC == Node.m_eSaneIndexState || ESISKTCC == Node.m_eSaneIndexState  )
	{
		StrTip.Format(L"%s: %s ", StrPrompt.GetBuffer(), StrPrice.GetBuffer());
	}
	else if ( ESISDCKO == Node.m_eSaneIndexState )
	{
		StrTip.Format(L"多头清仓: %s 空头开仓: %s", StrPrice.GetBuffer(), StrPrice.GetBuffer());
	}
	else if ( ESISKCDO == Node.m_eSaneIndexState )
	{
		StrTip.Format(L"空头清仓: %s 多头开仓: %s", StrPrice.GetBuffer(), StrPrice.GetBuffer());
	}
	else
	{
		return L"";
	}
	//
		
	CFileFind filefind;
	if ( filefind.FindFile(L"woyaotiaozhibiao") )
	{
		CString StrDebug;
		StrDebug.Format(L"  【条件号: %d 节点号: %d】", Node.m_stDebugInfo.m_iCondition, Node.m_stDebugInfo.m_iNodePos);
		StrTip += StrDebug;
	}

	return StrTip;
}

bool32 GetIndex(const CString& StrLineName, IN const T_IndexOutArray* pIndexIn, OUT float*& pIndexOut, OUT int32& iNumPoint)
{
	// 
	pIndexOut = NULL;
	iNumPoint = 0;

	//
	if ( NULL == pIndexIn )
	{
		return false;
	}

	int32 iIndexLine = -1;
	pIndexIn->m_aMapNameToIndex.Lookup(StrLineName, iIndexLine);

	if ( iIndexLine < 0 || iIndexLine >= pIndexIn->iIndexNum )
	{
		// 指标线的索引值非法
		return false;
	}
	
	pIndexOut	= pIndexIn->index[iIndexLine].pPoint;
	iNumPoint	= pIndexIn->index[iIndexLine].iPointNum;

	return true;
}

float GetIndexValue(int32 iPos, const CString& StrLineName, IN const T_IndexOutArray* pIndex)
{
	if ( NULL == pIndex )
	{
		return FLT_MAX;
	}
	
	DWORD dwBegin = timeGetTime();

	int32 iIndexLine = -1;
	pIndex->m_aMapNameToIndex.Lookup(StrLineName, iIndexLine);
	
	// std::map<CString, int32>::const_iterator it = pIndex->m_aMapNameToIndex.find(StrLineName);	
	// iIndexLine = it->second;
	DWORD dwEnd = timeGetTime();

	if ( dwEnd - dwBegin > 0 )
	{
	  CString StrTrace;
	  StrTrace.Format(L"m_aMapNameToIndex.Lookup [%d] 耗时 = %d \n", iIndexLine, dwEnd - dwBegin);
	  // TRACE(StrTrace);
	}

	if ( iIndexLine < 0 || iIndexLine >= pIndex->iIndexNum )
	{
		// 指标线的索引值非法
	//	////ASSERT(0);  //test2013
		return FLT_MAX;
	}
	
	if ( pIndex->index[iIndexLine].iPointNum <= 0 || iPos < 0 || iPos >= pIndex->index[iIndexLine].iPointNum )
	{
		// 指标线中这点的索引值非法
	//	////ASSERT(0); //test2013
		return FLT_MAX;
	}
	
	float fReval = pIndex->index[iIndexLine].pPoint[iPos];
	return fReval;
}

float GetchlFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"chl", pIndex);
}	

float Getm1FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"m1", pIndex);
}	

float Getm2FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"m2", pIndex);
}	

float Getm3FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"m3", pIndex);
}	

float Getm4FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"m4", pIndex);
}	

float Getm5FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"m5", pIndex);
}	

float Getm6FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"m6", pIndex);
}	

float Getm7FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"m7", pIndex);
}	

float Getm8FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"m8", pIndex);
}	
							
float Getma5FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"ma5", pIndex);
}	

float Getma8FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"ma8", pIndex);
}	

float Getma13FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"ma13", pIndex);
}	

float Getma21FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"ma21", pIndex);
}	

float Getma34FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"ma34", pIndex);
}	

float Getma55FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"ma55", pIndex);
}	

float Getma89FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"ma89", pIndex);
}

float Getlma5FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"ma5", pIndex);
}	

float Gethma5FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"hma5", pIndex);
}	

float GethighkFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"highk", pIndex);
}	

float GethhcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"hhc", pIndex);
}	

float GetllcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"llc", pIndex);
}	

float GethighkkFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"highkk", pIndex);
}	

float Getkhigh1FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 		
{
	return GetIndexValue(iPos, L"khigh1", pIndex);
}	

float GetkhighFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 		
{
	return GetIndexValue(iPos, L"khigh", pIndex);
}	

float Getmahighk5FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"mahighk5", pIndex);
}	

float	GetCfjd0FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"cfjd0", pIndex);
}	
							
float	GetCfjd1FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)								
{
	return GetIndexValue(iPos, L"cfjd1", pIndex);
}

float	GetCfjd2FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)								
{
	return GetIndexValue(iPos, L"cfjd2", pIndex);
}

float	GetMaxCfjdFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"maxcfjd", pIndex);
}

float	GetMinCfjdFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"mincfjd", pIndex);
}

float	GetMaxMaFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"maxma", pIndex);
}

float	GetMinMaFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"minma", pIndex);
}

float	GetMaxMacFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"maxmac", pIndex);
}	

float	GetZdtj1FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"zdtj1", pIndex);
}	

float	GetZdtj2FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"zdtj2", pIndex);
}	

float	Gethigh16FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"high16", pIndex);
}	

float	Gethigh4FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"high4", pIndex);
}	

float	Getlow16FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"low16", pIndex);
}	

float	Getlow4FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"low4", pIndex);
}

float	Getlow3FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"low3", pIndex);
}

float	Getzsj0FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"zsj0", pIndex);
}

float Getkhigh01FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"khigh01", pIndex);
}	

float Getkhigh02FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"khigh02", pIndex);
}	

float GetqsmaFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"qsma", pIndex);
}	

float Getdif89FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"dif89", pIndex);
}	

float Getdif21FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"dif21", pIndex);
}	

float Getdif89cFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"dif89c", pIndex);
}	

float Getdif21cFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"dif21c", pIndex);
}	

float GetqsqdFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"qsqd", pIndex);
}	

float GetmaqsqdFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"maqsqd", pIndex);
}	

float GetqsqdcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"qsqdc", pIndex);
}	

float GetmaqsqdcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"maqsqdc", pIndex);
}	

float GetzhiyingFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 								
{
	return GetIndexValue(iPos, L"zhiying", pIndex);
}	

float GetzhisunFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 	
{
	return GetIndexValue(iPos, L"zhisun", pIndex);
}	

float GetqsqdzdFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex) 	
{
	return GetIndexValue(iPos, L"qsqdzd", pIndex);
}

float	GetzsjFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"zsj", pIndex);
}

float	GetcfjdcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"cfjdc", pIndex);
}

float	Getma34cFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"ma34c", pIndex);
}

float	Getma89cFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"ma89c", pIndex);
}

float	Getqsma5FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"qsma5", pIndex);
}

float	GetbiasFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"bias", pIndex);
}

float	Getbias1FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"bias1", pIndex);
}

float	GetmabiasFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"mabias", pIndex);
}

float	Getmabias21FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"mabias21", pIndex);
}

float	GetbiascFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"biasc", pIndex);
}

float	GetmabiascFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"mabiasc", pIndex);
}

float	Getllvmabias3FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"llvmabias3", pIndex);
}

float	GetcfjdllvcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"cfjdllvc", pIndex);
}

float	GetcfjdhhvcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"cfjdhhvc", pIndex);
}

float	Getcfjd21cFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"cfjd21c", pIndex);
}

float	Getllvmabias8FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"llvmabias8", pIndex);
}

float GethlmaqsqdcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"hlmaqsqdc", pIndex);
}

float Gethhv13FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"hhv13", pIndex);
}

float Getllv13FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"llv13", pIndex);
}

float Getllv5FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"llv5", pIndex);
}

float Getdif89c6FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"dif89c6", pIndex);
}

float GetqsqdbFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	float fqsqdc = GetqsqdFromIndex(iPos, pIndex);
	float fqsqdb = fqsqdc / 30;

	return fqsqdb;
}

float Gethdif21FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"hdif21", pIndex);
}

float GetdifcdcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"difcdc", pIndex);
}

float GetmaxdifFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"maxdif", pIndex);
}

float Getmindif89FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"mindif", pIndex);
}

float Getlow5FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"low5", pIndex);
}

float Getlow8FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"low8", pIndex);
}

float Gethigh5FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"high5", pIndex);
}

float Gethigh8FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"high8", pIndex);
}

float	Gethhvmabias3FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return GetIndexValue(iPos, L"hhvmabias3", pIndex);
}

float GetHighLowPoint(const float* pData, int32 iCountData, int32 iNum1, bool32 bHigh)
{
	//
	if ( NULL == pData || iCountData < 4 )
	{
		return FLT_MIN;
	}

	if ( iNum1 > iCountData )
	{
		iNum1 = iCountData;
	}

	// 倒数第二个点开始
	const float* pTmp = pData + iCountData - 1;
	int32 iTmp1 = iNum1;

	//
	float fResult = FLT_MIN;

	//
	while( iTmp1 )
	{
		//
		if ( (pTmp == pData + iCountData - 1) || (pTmp == pData) || (pTmp == pData + 1) )
		{
			// 第一个, 第二个, 最后一个点判断不了
			iTmp1--;
			pTmp--;
			continue;
		}
		
		float fNow  = *pTmp;
		float fNext = *(pTmp+1);
		float fPre	= *(pTmp-1);
		float fPre2 = *(pTmp-2);

		if ( bHigh )
		{
			if ( (fNow > fNext) && (fNow > fPre) && (fNow > fPre2) )
			{
				// 保存结果
				fResult = fResult > fNow ? fResult : fNow;
			}
		}
		else
		{
			if ( (fNow < fNext) && (fNow < fPre) && (fNow < fPre2) )
			{
				// 保存结果
				fResult = fResult < fNow ? fResult : fNow;
			}
		}

		iTmp1--;
		pTmp--;
	}

	return fResult;
}

/*
E_SaneIntensity BeStrong(const T_SaneIndexNode& Node)
{
	if ( fabs(Node.m_fIntensity) > 4)
	{
		return ESISTRONG;
	}
	else if ( fabs(Node.m_fIntensity) >= 1 && fabs(Node.m_fIntensity) <= 4)
	{
		return ESINORMAL;
	}
	else if ( fabs(Node.m_fIntensity) < 1 )
	{
		return ESIWEEK;
	}
	
	////ASSERT(0);

	return ESINORMAL;		
}

bool32 BeStabilityValue(bool32 bUp, float fIntensityNow, float fIntensityPre)
{
	bool32 bStability = false;
	if ( bUp )
	{
		// 上升趋势 当前的强度值-前一点的强度值>=0为稳定，否则为不稳定
		bStability = fIntensityNow - fIntensityPre >= 0.0 ? true : false;
	}
	else
	{
		// 下降趋势 当前的强度值-前一点的强度值<=0为稳定，否则为不稳定
		bStability = fIntensityNow - fIntensityPre <= 0.0 ? true : false;
	}
	
	return bStability;
}
*/

E_SaneIntensity BeStrong(const T_SaneIndexNode& Node)
{
	// 强：QSMAC20>=4
	// 一般：1.5< QSMAC20<4
	// 弱：QSMAC20<1.5

	float fqsqd = Node.m_fIntensity;
	fqsqd = fabsf(fqsqd);

	if ( fqsqd >= 30 )
	{
		return ESISTRONG;
	}
	else if ( fqsqd < 30 )
	{
		return ESINORMAL;
	}
	/*else if ( 0 )
	{
		return ESIWEEK;
	}*/	

	return ESICOUNT;		 
}

bool32 BeStabilityValue(int32 iPos, IN const T_IndexOutArray* pIndex)
{

	return false;
}

bool32 BeRightOpenCondition(bool32 bFuture, bool32 bDtoc, int32 iOpenCondition, int32 iClearCondition)
{
	return true;

	// 判断 iClearCondition 是不是 iOpenCondition 对应的清仓条件
	if ( iOpenCondition < 1 )
	{
		////ASSERT(0);
		return false;
	}

	// 方便调试, 序号是从1 开始的, 所以这里要减 1
	int32 iIndex = iOpenCondition - 1;

	if ( bFuture )
	{	
		// 期货
		if ( bDtoc )
		{
			// 期货多头开仓
			if ( iOpenCondition > s_kiDtocConditionNumsFuture )
			{
				////ASSERT(0);
				return false;
			}

			for ( int32 i = 0; i < s_kiMaxCCConditions; i++ )
			{
				if ( iClearCondition == s_kaiDtocToDtccMapFuture[iIndex][i] )
				{
					return true;
				}				
			}
		}
		else
		{
			// 期货空头开仓
			if ( iOpenCondition > s_kiDtocConditionNumsFuture )
			{
				////ASSERT(0);
				return false;
			}
			
			for ( int32 i = 0; i < s_kiMaxCCConditions; i++ )
			{
				if ( iClearCondition == s_kaiKtocToKtccMap[iIndex][i] )
				{
					return true;
				}				
			}
		}
	}
	else
	{
		// 股票
		if ( bDtoc )
		{
			// 股票多头开仓
			if ( iOpenCondition > s_kiDtocConditionNumsStock )
			{
				////ASSERT(0);
				return false;
			}
			
			for ( int32 i = 0; i < s_kiMaxCCConditions; i++ )
			{
				if ( iClearCondition == s_kaiDtocToDtccMapStock[iIndex][i] )
				{
					return true;
				}				
			}
		}
		else
		{
			// 不应该出现
			////ASSERT(0);
		}
	}

	return false;
}

float CalcSaneIndexStabilityValue(float fIntensityNow, float fIntensityPre)
{
	if ( !BeValidFloat(fIntensityNow) || !BeValidFloat(fIntensityPre) )
	{
		return FLT_MAX;
	}

	return (fIntensityNow - fIntensityPre);
}

E_SaneMarketInfo GetMarketInfo(int32 iPos, IN T_IndexOutArray* pIndex)
{
	//  DIF89 >=0 AND DIF21 >=0 红色 多头市场
	//	DIF89 >=0 AND DIF21 <0 黄色  多头回调
	//	DIF89 <0 AND DIF21 <0 绿色	 空头市场
	//  DIF89 <0 AND DIF21 >=0 粉色  空头反弹

	float fDif89 = Getdif89FromIndex(iPos, pIndex);
	float fDif21 = Getdif21FromIndex(iPos, pIndex);
	
	if ( BeValidFloat(fDif89) && BeValidFloat(fDif21) )
	{
		if ( fDif89 >= 0 && fDif21 >= 0 )
		{
			return ESMIDT;
		}
		else if ( fDif89 >= 0 && fDif21 < 0 )
		{
			return ESMIDH;
		}
		else if ( fDif89 < 0 && fDif21 < 0 )
		{
			return ESMIKT;
		}
		else if ( fDif89 < 0 && fDif21 >= 0 )
		{
			return ESMIKH;
		}
	}
	
	return ESMICOUNT;
}

CString GetNodeStateString(bool32 bFuture, const T_SaneIndexNode& Node)
{	
	// 期货和股票分开
	CString StrState;

	if( ESISDTOC == Node.m_eSaneIndexState )
	{
		if ( bFuture )
		{			
			StrState = L"多头开仓";
		}
		else
		{
			StrState = L"股票买入";
		}		
	}
	else if ( ESISDTCC == Node.m_eSaneIndexState )
	{
		if ( bFuture )
		{
			StrState = L"多头清仓";
		}
		else
		{
			StrState = L"股票卖出";
		}		
	}
	else if ( ESISKCDO == Node.m_eSaneIndexState )
	{
		StrState = L"空清多开";
	}
	else if ( ESISDTCY == Node.m_eSaneIndexState )
	{
		if ( bFuture )
		{
			StrState = L"多头持有";
		}
		else
		{
			StrState = L"股票持有";
		}		
	}
	else if ( ESISKTOC == Node.m_eSaneIndexState )
	{
		StrState = L"空头开仓";
	}
	else if ( ESISKTCC == Node.m_eSaneIndexState )
	{
		StrState = L"空头清仓";
	}
	else if ( ESISDCKO == Node.m_eSaneIndexState )
	{
		StrState = L"多清空开";
	}
	else if ( ESISKTCY == Node.m_eSaneIndexState )
	{
		StrState = L"空头持有";
	}
	else if ( ESISKCGW == Node.m_eSaneIndexState || ESISNONE == Node.m_eSaneIndexState )
	{
		StrState = L"空仓观望";
	}

	return StrState;
}

CString GetNodeMarketString(E_SaneMarketInfo eMarketInfo)
{
	CString StrMarket = L" ";
	
	if ( ESMIDT == eMarketInfo)
	{
		StrMarket = L"多头市场";
	}
	else if ( ESMIKT == eMarketInfo )
	{
		StrMarket = L"空头市场";
	}
	else if ( ESMIDH == eMarketInfo )
	{
		StrMarket = L"多头回调";
	}
	else if ( ESMIKH == eMarketInfo )
	{
		StrMarket = L"空头回调";
	}

	return StrMarket;
}

CString GetSaneNodeIntensityString(const T_SaneIndexNode& Node)
{
	CString StrReV = L"";

	if ( ESISTRONG == BeStrong(Node) )
	{
		StrReV = L"强";
	}
	else if ( ESINORMAL == BeStrong(Node) )
	{
		StrReV = L"一般";
	}
	else if ( ESIWEEK == BeStrong(Node) )
	{
		StrReV = L"弱";
	}
	
	return StrReV;
}

CString GetSaneNodeStabilityString(const T_SaneIndexNode& Node)
{
	CString StrReV = L"";
	
	if ( Node.m_bStability )
	{
		StrReV = L"稳定";
	}
	else
	{
		StrReV = L"不稳定";
	}

	return StrReV;
}

CString	GetSaneNodeUpDownString(const T_SaneIndexNode& Node)
{	
	CString StrReV = L"";
 
	if ( ESMIDT == Node.m_eSaneMarketInfo )
	{
		StrReV = L"上涨";
	}
	else if ( ESMIKT == Node.m_eSaneMarketInfo )
	{
		StrReV = L"下跌";
	}
	else 
	{
		StrReV = L"回调";
	}

	return StrReV;
}

CString GetSaneNodeSummary(const T_SaneIndexNode& Node)
{
	CString StrReV = L"";

	if ( ESISTRONG == BeStrong(Node) )
	{
		StrReV = L"强";
	}
	else if ( ESINORMAL == BeStrong(Node) )
	{
		StrReV = L"一般";
	}
	else if ( ESIWEEK == BeStrong(Node) )
	{
		StrReV = L"弱";
	}
	//

	StrReV += L",";

	if ( Node.m_bStability )
	{
		StrReV += L"稳定";
	}
	else
	{
		StrReV += L"不稳定";
	}
	//
	
	StrReV += L",";

	if ( ESISNONE == Node.m_eSaneIndexState || ESISKCGW == Node.m_eSaneIndexState || ESISDTCC == Node.m_eSaneIndexState || ESISKTCC == Node.m_eSaneIndexState )
	{
		StrReV += L"震荡趋势";
	}
	else if ( ESISDTOC == Node.m_eSaneIndexState || ESISDTCY == Node.m_eSaneIndexState )
	{
		StrReV += L"上升趋势";
	}
	else if ( ESISKTOC == Node.m_eSaneIndexState || ESISKTCY == Node.m_eSaneIndexState )
	{
		StrReV += L"下降趋势";
	}     

	return StrReV;
}


/*
// 统计函数
bool32 StatisticaSaneValues( IN const CArray<T_SaneIndexNode, T_SaneIndexNode>& aSaneValues, OUT int32& iTradeTimes, OUT int32& iGoodTrades, OUT int32& iBadTrades, OUT float& fAccuracyRate, OUT float& fProfitability)
{
	iTradeTimes = 0;
	iGoodTrades	= 0;
	iBadTrades	= 0;
	fAccuracyRate  = 0.0;
	fProfitability = 0.0;

	T_SaneIndexNode* pSaneValuse = (T_SaneIndexNode*)aSaneValues.GetData();
	
	if ( NULL == pSaneValuse )
	{
		return false;
	}
	
	int32 iNodeSize = aSaneValues.GetSize();
	//
	for ( int32 i = 0 ; i < iNodeSize; i++ )
	{
		T_SaneIndexNode NodeBegin = pSaneValuse[i];
		
		if ( ESISDTOC == NodeBegin.m_eSaneIndexState )
		{
			// 多头开仓, 找到多头清仓为止

			for ( int32 j = i+1; j < iNodeSize ; j ++ )
			{				
				T_SaneIndexNode NodeEnd = aSaneValues[j];

				if ( ESISDTCC == NodeEnd.m_eSaneIndexState || j == (iNodeSize - 1) )
				{
					// 是多头清仓,或者到了最后一根了,算一次交易
					iTradeTimes += 1;
					
					// 判断清仓价格和开仓价格的差
					if ( NodeEnd.m_fPrice >= NodeBegin.m_fPrice )
					{
						iGoodTrades += 1;
					}
					else
					{
						iBadTrades += 1;
					}
					
					// 收益率:
					if ( 0.0 != NodeBegin.m_fPrice )
					{						
						float fProfitabilityPer = (NodeEnd.m_fPrice - NodeBegin.m_fPrice) / NodeBegin.m_fPrice;
						fProfitability += fProfitabilityPer;
					}
					//					
					i = j;
					break;
					//
				}
			}
		}
		else if ( ESISKTOC == NodeBegin.m_eSaneIndexState )
		{
			// 多头清仓

			for ( int32 j = i+1; j < iNodeSize ; j ++ )
			{				
				T_SaneIndexNode NodeEnd = aSaneValues[j];
				
				if ( ESISKTCC == NodeEnd.m_eSaneIndexState || j == (iNodeSize - 1) )
				{
					// 是多头清仓,或者到了最后一根了,算一次交易
					iTradeTimes += 1;
					
					// 判断清仓价格和开仓价格的差
					if ( NodeBegin.m_fPrice >= NodeEnd.m_fPrice )
					{
						iGoodTrades += 1;
					}
					else
					{
						iBadTrades += 1;
					}
					
					// 收益率:
					if ( 0.0 != NodeBegin.m_fPrice )
					{						
						float fProfitabilityPer = (NodeBegin.m_fPrice - NodeEnd.m_fPrice) / NodeBegin.m_fPrice;
						fProfitability += fProfitabilityPer;
					}
					//					
					i = j;
					break;
					//
				}
			}
		}
	}

	if ( iTradeTimes > 0)
	{
		fAccuracyRate = (float)iGoodTrades / (float)iTradeTimes;
	}
	
	return true;
}
*/

bool32 StatisticaSaneValues2(IN int32 iKLineNums, IN  float fPriceLastClose, OUT int32& iKLineNumsReal, IN const CArray<T_SaneIndexNode, T_SaneIndexNode>& aSaneValues, OUT int32& iTradeTimes, OUT int32& iGoodTrades, OUT int32& iBadTrades, OUT float& fAccuracyRate, OUT float& fProfitability, OUT float& fProfit)
{
	if ( iKLineNums <= 0 ) 
	{
		return false;
	}
	
	if( iKLineNums > aSaneValues.GetSize() )
	{
		iKLineNums = aSaneValues.GetSize();
	}

	iTradeTimes = 0;
	iGoodTrades	= 0;
	iBadTrades	= 0;
	fAccuracyRate  = 0.0;
	fProfitability = 0.0;
	fProfit		   = 0.0;
	iKLineNumsReal = 0;

	T_SaneIndexNode* pSaneValuse = (T_SaneIndexNode*)aSaneValues.GetData();
	
	if ( NULL == pSaneValuse )
	{
		return false;
	}
	
	int32 iNodeSize = aSaneValues.GetSize();

	// 找 iKLineNums 附近的开仓点:
	bool32 bFind = false;
	int32  iBeginIndex = iNodeSize - iKLineNums;
	T_SaneIndexNode	NodeBegin;

	if ( iBeginIndex < 0 || iBeginIndex >= aSaneValues.GetSize())
	{
		return false;
	}
	
	int32 i;
	for ( i = iBeginIndex; i > 0; i-- )
	{
		// 向前找一找
		NodeBegin = pSaneValuse[i];
		
		if ( ESISDTOC == NodeBegin.m_eSaneIndexState || ESISKCDO == NodeBegin.m_eSaneIndexState || ESISKTOC == NodeBegin.m_eSaneIndexState || ESISDCKO == NodeBegin.m_eSaneIndexState)
		{
			// 开仓了
			bFind = true;
			break;
		}
	}

	if ( !bFind )
	{
		for ( i = iBeginIndex; i < iNodeSize; i++)
		{
			// 向后找找
			NodeBegin = pSaneValuse[i];
			
			if ( ESISDTOC == NodeBegin.m_eSaneIndexState || ESISKCDO == NodeBegin.m_eSaneIndexState || ESISKTOC == NodeBegin.m_eSaneIndexState || ESISDCKO == NodeBegin.m_eSaneIndexState)
			{
				// 开仓了
				bFind = true;
				break;
			}
		}
	}

	if ( !bFind )
	{
		return false;			
	}

	if ( bFind && iBeginIndex == (iNodeSize - 1) )
	{
		// 最后一根找到了, 不用计算了, 直接返回true:
		iTradeTimes = 0;
		iGoodTrades	= 0;
		iBadTrades	= 0;
		fAccuracyRate  = 0.0;
		fProfitability = 0.0;
		fProfit		   = 0.0;
		iKLineNumsReal = 1;

		return true;
	}

	//
	iBeginIndex = i;
	iKLineNumsReal = iNodeSize - iBeginIndex;
	// 
	for ( i = iBeginIndex; i < iNodeSize; i++ )
	{
		T_SaneIndexNode* pNodeBegin = NULL; 
		T_SaneIndexNode* pNodeEnd	= NULL;
		
		// 开始统计了:
		if ( ESISDTOC == pSaneValuse[i].m_eSaneIndexState || ESISKCDO == pSaneValuse[i].m_eSaneIndexState )
		{
			// 多头开仓了, 找[多头清仓]或者是[多头清仓空头开仓]
			pNodeBegin = &pSaneValuse[i];

			for ( int32 j = i+1; j < iNodeSize; j++ )
			{
				pNodeEnd = &pSaneValuse[j];
					
				if ( ESISDTCC == pNodeEnd->m_eSaneIndexState || ESISDCKO == pNodeEnd->m_eSaneIndexState || j == (iNodeSize - 1) )
				{
					// 找到了,统计一下这次交易:
				
					iTradeTimes += 1;
					
					// 判断清仓价格和开仓价格的差
					if ( j == (iNodeSize - 1) )
					{
						if ( fPriceLastClose >= pNodeBegin->m_fPrice )
						{
							iGoodTrades += 1;
						}
						else
						{
							iBadTrades += 1;
						}
					}
					else
					{
						if ( pNodeEnd->m_fPrice >= pNodeBegin->m_fPrice )
						{
							iGoodTrades += 1;
						}
						else
						{
							iBadTrades += 1;
						}
					}
					
					// 收益率:
					if ( 0.0 != pNodeBegin->m_fPrice )
					{	
						float fProfitChange;		

						if ( j == (iNodeSize - 1) )
						{
							fProfitChange = fPriceLastClose - pNodeBegin->m_fPrice;
						}
						else
						{
							fProfitChange = pNodeEnd->m_fPrice - pNodeBegin->m_fPrice;
						}
	
						float fProfitabilityPer = fProfitChange / pNodeBegin->m_fPrice;
						
						fProfitability += fProfitabilityPer;
						fProfit		   += fProfitChange;
					}

					// 清仓和开仓可能同一个节点.下一个循环从当前节点开始:
					if ( j != (iNodeSize - 1) )
					{
						i = j - 1;
					}
					else
					{
						i = j;
					}					
					break;
					//
				}
			}			
		}
		else if ( ESISKTOC == pSaneValuse[i].m_eSaneIndexState || ESISDCKO == pSaneValuse[i].m_eSaneIndexState )
		{
			// 空头开仓了
			pNodeBegin = &pSaneValuse[i];

			for ( int32 j = i+1; j < iNodeSize; j++ )
			{
				pNodeEnd = &pSaneValuse[j];
				
				if ( ESISKTCC == pNodeEnd->m_eSaneIndexState || ESISKCDO == pNodeEnd->m_eSaneIndexState || j == (iNodeSize - 1) )
				{
					// 是空头清仓,或者到了最后一根了,算一次交易
					iTradeTimes += 1;
					
					// 判断清仓价格和开仓价格的差
					if ( j == (iNodeSize - 1))
					{
						if ( pNodeBegin->m_fPrice >= fPriceLastClose )
						{
							iGoodTrades += 1;
						}
						else
						{
							iBadTrades += 1;
						}
					}
					else
					{
						if ( pNodeBegin->m_fPrice >= pNodeEnd->m_fPrice )
						{
							iGoodTrades += 1;
						}
						else
						{
							iBadTrades += 1;
						}
					}
					
					// 收益率:
					if ( 0.0 != pNodeBegin->m_fPrice )
					{												
						float fProfitChange;		
						
						if ( j == (iNodeSize - 1) )
						{
							fProfitChange = pNodeBegin->m_fPrice - fPriceLastClose;
						}
						else
						{
							fProfitChange = pNodeBegin->m_fPrice - pNodeEnd->m_fPrice;
						}

						float fProfitabilityPer = fProfitChange / pNodeBegin->m_fPrice;
						
						fProfitability += fProfitabilityPer;
						fProfit		   += fProfitChange;									
					}
					
					//					
					if ( j != (iNodeSize - 1) )
					{
						i = j - 1;
					}
					else
					{
						i = j;
					}
					break;
					//
				}
			}
		}
		else
		{
			;
		}
	}

	if ( iTradeTimes > 0)
	{
		fAccuracyRate = (float)iGoodTrades / (float)iTradeTimes;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
//lint --e{438}
T_IndexOutArray* CalcIndexEMAForSane(const CArray<CKLine, CKLine>& aKLines)
{
	// 注意: 这个返回的指针是 new 出来的. 所以每次调用之前,要把先前的值 DEL 掉!

	if ( aKLines.GetSize() <= 0 )
	{
		////ASSERT(0);
		return NULL;
	}

	// ...fangz0703 直接用 CFormularContent ,不用 T_IndexParam
	T_IndexParam* pIndexEMA = new T_IndexParam;
	pIndexEMA->pRegion = NULL;
	
	// 这里的指标名称就是"SYSTEM"
	CString StrIndexName = g_KaStrIndexNotShow[0];

	//
	if ( StrIndexName.GetLength() <= 0 )
	{
		// 名字的长度
		////ASSERT(0);
	}

	_tcscpy ( pIndexEMA->strIndexName, StrIndexName);	
	pIndexEMA->pContent = CFormulaLib::instance()->AllocContent(pIndexEMA->id, pIndexEMA->strIndexName);	

	if ( NULL == pIndexEMA->pContent )
	{
		////ASSERT(0);
		DEL(pIndexEMA);
		return NULL;
	}

	CArray<CKLine, CKLine> aKLineTemp;
	int32 iSize = aKLines.GetSize();
	aKLineTemp.SetSize(iSize);
	
	memcpyex(aKLineTemp.GetData(), aKLines.GetData(), sizeof(CKLine) * iSize);
	T_IndexOutArray* pIndexEMAForSane = formula_index (pIndexEMA->pContent, aKLineTemp);		
	
	if( NULL == pIndexEMAForSane)
	{
		////ASSERT(0);
		DEL(pIndexEMA);

		return NULL;
	}
	
	DEL(pIndexEMA);
	
	return pIndexEMAForSane;
}

bool32 UpDateLatestIndexForSane(const T_IndexOutArray* pIndex, OUT CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues)
{
	if ( NULL == pIndex || aSaneIndexValues.GetSize() <= 1 )
	{
		////ASSERT(0);
		return false;
	}

	//
	int32 iPos = aSaneIndexValues.GetSize() - 1;
	
	T_SaneIndexNode* pSaneIndex = (T_SaneIndexNode*)aSaneIndexValues.GetData();

	pSaneIndex[iPos].m_fLineData = GetqsmaFromIndex(iPos, pIndex);

	return true;
}
	   
bool32 JudgeShakeOC(bool32 bFuture, int32 iPos, IN const T_IndexOutArray* pIndex)
{
	return false;
}

bool32 CalcHistorySaneIndex(IN bool32 bFuture, IN bool32 bPassedUpdateTime, IN T_IndexOutArray* pIndex, IN const CArray<CKLine, CKLine>& aKLines, OUT CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues)
{
	// CLogFunctionTime Log("CalcHistorySaneIndex");
	int32 iKLineSize = aKLines.GetSize();
	
	if ( iKLineSize <= 0 )
	{
		return false;
	}
	
	if ( NULL == pIndex )
	{
		return false;
	}
	
	//
	aSaneIndexValues.RemoveAll();
	
	//
	if ( iKLineSize <= SANE_INDEX_KLINE_NUMS + 1)
	{
		// 至少需要 SANE_INDEX_KLINE_NUMS + 1 个节点才能算出第一个节点
		return false;
	}
	
	aSaneIndexValues.SetSize(iKLineSize);
	T_SaneIndexNode* pSaneIndexValues = (T_SaneIndexNode*)aSaneIndexValues.GetData();
	memset((void*)pSaneIndexValues, 0, sizeof(T_SaneIndexNode) * iKLineSize);
	
	T_SaneIndexNode NodeDefault;
	
	//
	for( int32 i = 0; i < iKLineSize; i++ )
	{	
		if ( 198 == i )
		{
			i = i;
		}

		if ( 0 == i )
		{
			// 第一个节点, 不算了, 无意义节点						
			memcpyex(pSaneIndexValues + i, &NodeDefault, sizeof(T_SaneIndexNode));
		}
		else if ( ((iKLineSize - 1) == i) && (!bPassedUpdateTime) )
		{
			// 算历史的时候, 最新一个节点没有过更新时间就不要计算最新值, 直接用倒数第二个的赋值
			memcpyex(pSaneIndexValues + iKLineSize - 1, pSaneIndexValues + iKLineSize - 2, sizeof(T_SaneIndexNode));
			T_SaneIndexNode& NodeLast = pSaneIndexValues[iKLineSize - 1];
			
			if ( ESISDTOC == NodeLast.m_eSaneIndexState || ESISKCDO == NodeLast.m_eSaneIndexState )
			{
				NodeLast.m_eSaneIndexState = ESISDTCY;
			}
			else if ( ESISKTOC == NodeLast.m_eSaneIndexState || ESISDCKO == NodeLast.m_eSaneIndexState )
			{
				NodeLast.m_eSaneIndexState = ESISKTCY;
			}
			else if ( ESISDTCC == NodeLast.m_eSaneIndexState || ESISKTCC == NodeLast.m_eSaneIndexState )
			{
				NodeLast.m_eSaneIndexState = ESISKCGW;
			}			

			// 趋势值还是要用自己的, 否则最后一个结点总是平的
			NodeLast.m_fLineData = GetqsmaFromIndex(i, pIndex);
		}
		else
		{
			memcpyex(pSaneIndexValues + i, &NodeDefault, sizeof(T_SaneIndexNode));
			
			// 开始计算
			bool32 bOK = CalcSaneNodeAccordingPreNode(true, bFuture, i, pIndex, aKLines, aSaneIndexValues);
		}	
	}

	return true;
}

/*********************************************************************************************************************************
功能: 计算最新的稳健型指标值
参数:	
bool32 bAddNew:									是否是新增一根
T_IndexOutArray* pIndex:						用于计算的指标序列数据 
const CArray<CKLine, CKLine>& aKLines:			用于计算的K 线序列数据
CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues: (传出参数) 计算完成后的稳健型指标数据
CTime TimeAction								计算的时间( pViewData->GetServerTime() ),记录下动作发生的时间
bool32 bTimeToUpdate							是否到了更新的时机( 如果当前状态是多头持有或者是空头持有,需要实时判断是否清仓. 除此之外的状态,根据外面定时器给的标志.一根K 线只更新一次 )		
bool32& bKillTimer								告诉外部,是否需要杀掉定时器
*********************************************************************************************************************************/

bool32 CalcLatestSaneIndex(IN bool32 bAddNew, IN bool32 bFuture, IN T_IndexOutArray* pIndex, IN const CArray<CKLine, CKLine>& aKLines, OUT CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues, const CTime& TimeAction, OUT bool32& bKillTimer)
{
	// 合法性验证
	if ( NULL == pIndex )
	{
		////ASSERT(0);
		return false;
	}

	if ( aKLines.GetSize() <= 0 )
	{
		////ASSERT(0);
		return false;
	} 

	int32 iSizeKLine = aKLines.GetSize();
	int32 iSizeIndex = aSaneIndexValues.GetSize();

	if ( iSizeKLine <= 1 || iSizeIndex <= 1 )
	{
		////ASSERT(0);
		return false;
	}

	if ( bAddNew )
	{
		if ( (iSizeIndex + 1) != iSizeKLine )
		{
			////ASSERT(0);
			return false;
		}
	}
	else
	{
		if ( iSizeKLine != iSizeIndex )
		{
			////ASSERT(0);
			return false;
		}
	}
	
	//
	
	//
	bKillTimer = false;
	//

	if ( !bAddNew )
	{
		// 最新一根有更新
		int32 iPos = iSizeIndex - 1;
		
		
		if ( !CalcSaneNodeAccordingPreNode(false, bFuture, iPos, pIndex, aKLines, aSaneIndexValues, TimeAction) ) 
		{
			////ASSERT(0);				
		}
		else
		{
			bKillTimer = true;
			
			// 开仓了,报警
			T_SaneIndexNode NodeNow = aSaneIndexValues.GetAt(iPos);
			
			if ( ESISDTOC == NodeNow.m_eSaneIndexState || ESISKTOC == NodeNow.m_eSaneIndexState || ESISKCDO == NodeNow.m_eSaneIndexState || ESISDCKO == NodeNow.m_eSaneIndexState )
			{
				// Beep(500, 1000);
				PlaysWaveSound(L"");					
			}
		}
	}
	else
	{
		// 新加了一根,复制前一根
		T_SaneIndexNode NodePre   = aSaneIndexValues.GetAt(iSizeIndex - 1);
		T_SaneIndexNode NodeToAdd = NodePre;

		// 前一根是开仓的话,这一根的状态设置为持有, 要不画面上连续出现两个开仓信号 -_-#
		if ( ESISDTOC == NodePre.m_eSaneIndexState || ESISKCDO == NodePre.m_eSaneIndexState )
		{
			NodeToAdd.m_eSaneIndexState = ESISDTCY;
		}
		else if ( ESISKTOC == NodePre.m_eSaneIndexState || ESISDCKO == NodePre.m_eSaneIndexState )
		{
			NodeToAdd.m_eSaneIndexState = ESISKTCY;
		}

		// 前一根是清仓的话,这一根的状态设置为观望, 要不画面上连续出现两个清仓信号 -_-#
		if ( ESISDTCC == NodePre.m_eSaneIndexState )
		{
			NodeToAdd.m_eSaneIndexState = ESISKCGW;
		}
		else if ( ESISKTCC == NodePre.m_eSaneIndexState )
		{
			NodeToAdd.m_eSaneIndexState = ESISKCGW;
		}

		aSaneIndexValues.Add(NodeToAdd);
	}

	// 保证两个数组的大小是一样的:
	ASSERT( aKLines.GetSize() == aSaneIndexValues.GetSize() );
	return true;
}

bool32 CalcSaneNodeAccordingPreNode(bool32 bHis, IN bool32 bFuture, int32 iPos,IN T_IndexOutArray* pIndex, IN const CArray<CKLine, CKLine>& aKLines, CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues, const CTime& TimeAction /*= 0*/)
{	
	// CLogFunctionTime Log("CalcSaneNodeAccordingPreNode");

	// 通过前一个节点算后一个的节点
	if ( NULL == pIndex || pIndex->iIndexNum <= 0 )
	{
		return false;
	}
	
	int32 iKLineSize = aKLines.GetSize();
	
	if( iKLineSize <=0 || iPos < 1 || iPos >= iKLineSize || iPos >= pIndex->index[1].iPointNum )
	{
		return false;
	}
	
	// 
	CKLine* pKLine = (CKLine*)aKLines.GetData();
	if (NULL == pKLine)
	{
		return false;
	}
	
	
	T_SaneIndexNode* pNodes = (T_SaneIndexNode*)aSaneIndexValues.GetData();
	if (NULL == pNodes)
	{
		return false;
	}
	
	T_SaneIndexNode& NodeNow = pNodes[iPos];
	T_SaneIndexNode	 NodePre = pNodes[iPos-1];
	
	// 1: 价格
	NodeNow.m_fPrice = pKLine[iPos].m_fPriceClose;
	
	// 2: 对应的K 线节点的时间
	NodeNow.m_TimeKLine = pKLine[iPos].m_TimeCurrent.GetTime();
	
	// 3: 交易时间:
	if ( bHis )
	{
		NodeNow.m_TimeAction = pKLine[iPos].m_TimeCurrent.GetTime();
	}
	else
	{
		NodeNow.m_TimeAction = TimeAction; 
	}
	
	// 10: 加权 MA	
	NodeNow.m_fLineData = GetqsmaFromIndex(iPos, pIndex);
	
	// 13: 算强度:
	NodeNow.m_fIntensity  = GetqsqdFromIndex(iPos, pIndex);
	
	// 15: 节点状态:
	if ( ESISNONE == NodePre.m_eSaneIndexState || ESISKCGW == NodePre.m_eSaneIndexState || ESISDTCC == NodePre.m_eSaneIndexState || ESISKTCC == NodePre.m_eSaneIndexState )
	{
		bool32 bOK = false;
		
		// 前一个是 无状态,空仓观望,多头清仓,空头清仓 . 这个判断开仓条件:
		if ( CalcConditionSaneDTOC(bFuture, iPos, bHis, pIndex, aKLines, aSaneIndexValues) )
		{
			// 满足多头开仓
			bOK = true;
			NodeNow.m_eSaneIndexState = ESISDTOC;
		}		
		else if ( CalcConditionSaneKTOC(bFuture, iPos, bHis, pIndex, aKLines, aSaneIndexValues) )
		{
			// 满足空头开仓
			bOK = true;
			NodeNow.m_eSaneIndexState = ESISKTOC;
		}		
		
		if( !bOK )
		{
			// 空仓观望
			NodeNow.m_eSaneIndexState = ESISKCGW;
		}
	}
	else if ( ESISDTOC == NodePre.m_eSaneIndexState || ESISDTCY == NodePre.m_eSaneIndexState || ESISKCDO == NodePre.m_eSaneIndexState )
	{
		int32 iOpenCondition = 0;

		for (int32 iIndexCondition = iPos - 1; iIndexCondition > 0; iIndexCondition--)
		{
			// 找到前一个开仓点的开仓条件:
			if ( -1 != pNodes[iIndexCondition].m_stDebugInfo.m_iCondition )
			{
				iOpenCondition = pNodes[iIndexCondition].m_stDebugInfo.m_iCondition;
				break;
			}
		}
			 
		// 多头开仓或者多头持有,判断是否多头清仓
		if ( CalcConditionSaneDTCC(bFuture, iPos, iOpenCondition, pIndex, aKLines, aSaneIndexValues) )
		{
			// 满足多头清仓的同时,判断是否满足空头开仓
			bool32 bShake = false;
			
			
			if ( CalcConditionSaneKTOC(bFuture, iPos, bHis, pIndex, aKLines, aSaneIndexValues))
			{
				// 多头清仓空头开仓
				NodeNow.m_eSaneIndexState = ESISDCKO;				
			}
			else
			{
				// 多头清仓
				NodeNow.m_eSaneIndexState = ESISDTCC;
			}
		}
		else 
		{
			NodeNow.m_eSaneIndexState = ESISDTCY;
		}
	}
	else if ( ESISKTOC == NodePre.m_eSaneIndexState || ESISKTCY == NodePre.m_eSaneIndexState || ESISDCKO == NodePre.m_eSaneIndexState )
	{
		int32 iOpenCondition = 0;
		
		for (int32 iIndexCondition = iPos - 1; iIndexCondition > 0; iIndexCondition--)
		{
			// 找到前一个开仓点的开仓条件:
			if ( -1 != pNodes[iIndexCondition].m_stDebugInfo.m_iCondition )
			{
				iOpenCondition = pNodes[iIndexCondition].m_stDebugInfo.m_iCondition;
				break;
			}
		}

		// 空头开仓或者空头持有,判断是否空头清仓
		if ( CalcConditionSaneKTCC(bFuture, iPos, iOpenCondition, pIndex, aKLines, aSaneIndexValues) )
		{
			

			if ( CalcConditionSaneDTOC(bFuture, iPos, bHis, pIndex, aKLines, aSaneIndexValues))
			{
				// 空头清仓,多头开仓
				NodeNow.m_eSaneIndexState = ESISKCDO;
			}
			else
			{
				// 空头清仓
				NodeNow.m_eSaneIndexState = ESISKTCC;
			}
		}
		else 
		{
			NodeNow.m_eSaneIndexState = ESISKTCY;
		}
	}

	// 14: 算稳定性:
	bool32 bUp = false;
	
	if ( ESISDTOC == NodeNow.m_eSaneIndexState || ESISDTCY == NodeNow.m_eSaneIndexState || ESISKCDO == NodeNow.m_eSaneIndexState)
	{
		bUp = true;
	}
	
	//
	NodeNow.m_fStability = GetqsqdcFromIndex(iPos, pIndex);

	//
	NodeNow.m_bStability = NodeNow.m_fStability >= 0 ? true : false;

	// 16: 市场信息
	NodeNow.m_eSaneMarketInfo = GetMarketInfo(iPos, pIndex);	

	// 测试: 趋势值
	NodeNow.m_fTrendValue = 0;

	return true;
}
 
bool32 CalcConditionSaneDTOC(bool32 bFuture, int32 iPos, bool32 bHis, T_IndexOutArray* pIndex, const CArray<CKLine, CKLine>& aKLines, /*const*/ CArray<T_SaneIndexNode, T_SaneIndexNode>& aIndexNodes)
{
	// CLogFunctionTime Log("CalcConditionSaneDTOC");
	
	// debug 
	int32 iCurCondition = 0;
	//

	if ( iPos == 268 )
	{
		iPos = iPos;
	}

	int32 iSizeKLine = aKLines.GetSize();
	int32 iSizeIndex = aIndexNodes.GetSize();
	
	if ( iSizeKLine <= 0 || iSizeIndex <= 0 || NULL == pIndex )
	{
		return false;
	}
	
	CKLine* pKLine = (CKLine*)aKLines.GetData();
	T_SaneIndexNode* pIndexNode = (T_SaneIndexNode*)aIndexNodes.GetData();
	
	if ( NULL == pKLine || NULL == pIndexNode )
	{
		return false;
	}
	
	if ( iPos < 3 )
	{
		// 要判断前一点的强度,所以第一个节点不用管
		return false;
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 期货:
	/*if ( 0 )
	{ 
		return false;
	}
	else*/
	{
		float fDif89	= Getdif89FromIndex(iPos, pIndex);
		float fDif89Pre	= Getdif89FromIndex(iPos - 1, pIndex);
		float fQsqd		= GetqsqdFromIndex(iPos, pIndex);
		float fQsqdPre	= GetqsqdFromIndex(iPos - 1, pIndex);
		float fqsqdzd	= GetqsqdzdFromIndex(iPos, pIndex);		
		float fMaqsqd   = GetmaqsqdFromIndex(iPos, pIndex);
		float fMaqsqdPre= GetmaqsqdFromIndex(iPos - 1, pIndex);
		float fMaqsqdPre3 = GetmaqsqdFromIndex(iPos - 3, pIndex);
		float fDif89c	= Getdif89cFromIndex(iPos, pIndex);
		float fQsqdc	= GetqsqdcFromIndex(iPos, pIndex);
		float fQsqdcPre	= GetqsqdcFromIndex(iPos - 1, pIndex);
		float fQsqdcPre2= GetqsqdcFromIndex(iPos - 2, pIndex);
		float fQsqdcPre3= GetqsqdcFromIndex(iPos - 3, pIndex);
		float fHigh		= pKLine[iPos].m_fPriceHigh;
		float fZhiying  = GetzhiyingFromIndex(iPos, pIndex);
		float fHlmaqsqdc= GethlmaqsqdcFromIndex(iPos, pIndex);
		float fHhv13	= Gethhv13FromIndex(iPos, pIndex);
		float fDif21	= Getdif21FromIndex(iPos, pIndex);		
		float fDif89c6  = Getdif89c6FromIndex(iPos, pIndex);
		float fDifcdc	= GetdifcdcFromIndex(iPos, pIndex);
		float fMaxdif	= GetmaxdifFromIndex(iPos, pIndex);
		float fMindif89 = Getmindif89FromIndex(iPos, pIndex);
		float fLow5		= Getlow5FromIndex(iPos, pIndex);
		float fLow8		= Getlow8FromIndex(iPos, pIndex);
		float fDif21c	= Getdif21cFromIndex(iPos, pIndex);
		float fDif21Pre = Getdif21FromIndex(iPos - 1, pIndex);
		
		/*
		// 1.	QSQD > = DIF21 AND REF(QSQD,1) < REF(DIF21,1)  AND DIF89>=DIF21 AND DIF21>-2 AND DIF89>-10 AND DIFCDC>=13) AND DIF89C > -1
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fDif21) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21Pre) && BeValidFloat(fDif89) && BeValidFloat(fDif21) && BeValidFloat(fDifcdc) && BeValidFloat(fqsqdzd) )
			{
				if ( BeNeed(iCurCondition) && (fQsqd >= fDif21) && (fQsqdPre < fDif21Pre) && (fDif89 >= fDif21) && (fDif21 > -2) && (fDif89 > -10) && (fDifcdc >= 13) && (fDif89c > -1) )
				{			
					goto JUDGE_SHAKE_OPEN;
				}
			}			
		}

		// 2. QSQD > = DIF21 AND REF(QSQD,1) < REF(DIF21,1) AND DIF21>-3 AND ABS(DIF89)<13 AND DIFCDC<13 AND QSQDC>20 AND DIF89C >2 
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fDif21) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21Pre) && BeValidFloat(fDif89) && BeValidFloat(fDif21) && BeValidFloat(fDifcdc) && BeValidFloat(fqsqdzd) )
			{
				if ( BeNeed(iCurCondition) && (fQsqd >= fDif21) && (fQsqdPre < fDif21Pre) && (fDif21 > -3) && (fabs(fDif89) < 13) && (fDifcdc < 13) && (fQsqdc > 20) && (fDif89c > 2) )
				{			
					goto JUDGE_SHAKE_OPEN;
				}
			}
		}

		// 3.	QSQD > = DIF21 AND REF(QSQD,1) < REF(DIF21,1)  AND DIF89<DIF21   AND QSQDC>24 AND DIF21>-2  AND DIF89>-34 AND DIF89<-5	 AND DIF89C>2
		{
			++iCurCondition;

			if ( BeValidFloat(fQsqd) && BeValidFloat(fDif21) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21Pre) && BeValidFloat(fDif89) && BeValidFloat(fQsqdc))
			{
				if ( BeNeed(iCurCondition) && (fQsqd >= fDif21) && (fQsqdPre < fDif21Pre) && (fDif89 < fDif21) && (fQsqdc > 24) && (fDif21 > -2) && (fDif89 > -34) && (fDif89 < -5) && (fDif89c > 2) )
				{
					goto JUDGE_SHAKE_OPEN;
				}
			}
		}

		// 4、MAQSQD > = DIF21 AND REF(MAQSQD,1) < REF(DIF21,1)  AND DIF89>=DIF21 AND DIF21> -1  AND DIFCDC>=13 AND DIF89C >0
		{
			++iCurCondition;
			
			if ( BeValidFloat(fMaqsqd) && BeValidFloat(fDif21) && BeValidFloat(fMaqsqdPre) && BeValidFloat(fDif21Pre) && BeValidFloat(fDif89) && BeValidFloat(fQsqd) )
			{
				if ( BeNeed(iCurCondition) && (fMaqsqd >= fDif21) && (fMaqsqdPre < fDif21Pre) && (fDif89 >= fDif21) && (fDif21 > -1) && (fDifcdc >= 13) && (fDif89c > 0) )
				{
					goto JUDGE_SHAKE_OPEN;
				}
			}
		}

		// 5、MAQSQD>MAXDIF AND QSQDC >0 AND REF(QSQDC,1)<=0 AND DIF89>0 AND  DIF21>3 AND MAQSQD-QSQD<1 AND DIF89C>2
		{
			++iCurCondition;
			
			if ( BeValidFloat(fMaqsqd) && BeValidFloat(fDif21) && BeValidFloat(fMaqsqdPre) && BeValidFloat(fDif21Pre) && BeValidFloat(fDif89) && BeValidFloat(fQsqd) )
			{
				if ( BeNeed(iCurCondition) && (fMaqsqd >= fMaxdif) && (fQsqdc > 5) && (fQsqdcPre <= 0) && (fDif89 > 0) && (fDif21 > 3) && (fMaqsqd - fQsqd < 1) && (fDif89c > 2) )
				{
					goto JUDGE_SHAKE_OPEN;
				}
			}
		}

		// 6.	MAQSQD>MAXDIF AND QSQDC >0 AND REF(QSQDC,1)<=0 AND DIF89>-18 AND DIF21>3  AND DIF89C >2.3 AND DIF89<DIF21 AND QSQD>MAQSQD
		{
			++iCurCondition;
			
			if ( BeValidFloat(fMaqsqd) && BeValidFloat(fDif21) && BeValidFloat(fMaqsqdPre) && BeValidFloat(fDif21Pre) && BeValidFloat(fDif89) && BeValidFloat(fQsqd) )
			{
				if ( BeNeed(iCurCondition) && (fMaqsqd > fMaxdif) && (fQsqdc > 0) && (fQsqdcPre <= 0) && (fDif89 > -18) && (fDif21 > 3) && (fDif89c > 2.3) && (fDif89 < fDif21) && (fQsqd > fMaqsqd) )
				{
					goto JUDGE_SHAKE_OPEN;
				}
			}
		}
		
		// 7.	MAQSQD>MAXDIF AND QSQD >MAQSQD AND REF(QSQD,1)<= REF(MAQSQD,1) AND DIF89>-15 AND DIF21>3 AND MAQSQD-QSQD<1 AND DIF89C>2 
		{
			++iCurCondition;
			
			if ( BeValidFloat(fMaqsqd) && BeValidFloat(fDif21) && BeValidFloat(fMaqsqdPre) && BeValidFloat(fDif21Pre) && BeValidFloat(fDif89) && BeValidFloat(fQsqd) )
			{
				if ( BeNeed(iCurCondition) && (fMaqsqd > fMaxdif) && (fQsqd > fMaqsqd) && (fQsqdPre < fMaqsqdPre) && (fDif89 > -15) && (fDif21 > 3) && (fMaqsqd - fQsqd < 1) && (fDif89c > 2) )
				{
					goto JUDGE_SHAKE_OPEN;
				}
			}
		}
		
		// 8.	MAQSQD > = 13 AND REF(MAQSQD,1) <13  AND DIF21>3 AND DIF89>-13 AND  DIF89C6 >5 AND QSQDC>5
		{
			++iCurCondition;

			if ( BeValidFloat(fMaqsqd) && BeValidFloat(fMaqsqdPre) && BeValidFloat(fDif21) && BeValidFloat(fDif89) && BeValidFloat(fDif89c) && BeValidFloat(fQsqdc) )
			{
				if ( BeNeed(iCurCondition) && (fMaqsqd >= 13) && (fMaqsqdPre < 13) && (fDif21 > 3) && (fDif89 > -13) && (fDif89c6 > 5) && (fQsqdc > 5) )
				{
					goto JUDGE_SHAKE_OPEN;
				}
			}
		}

		// 9.	QSQD > = DIF89 AND REF(QSQD,1) < REF(DIF89,1)  AND DIF89>=DIF21 AND DIF21>0 AND ABS(DIF89)<13 AND DIFCDC<13 AND QSQDC>15 AND DIF89C >1.7
		{
			++iCurCondition;
			
			if ( BeValidFloat(fMaqsqd) && BeValidFloat(fMaqsqdPre) && BeValidFloat(fDif21) && BeValidFloat(fDif89) && BeValidFloat(fDif89c) && BeValidFloat(fQsqdc) )
			{
				if ( BeNeed(iCurCondition) && (fQsqd >= fDif89) && (fQsqdPre < fDif89Pre) && (fDif89 >= fDif21) && (fDif21 > 0) && (fabs(fDif89) < 13) && (fDifcdc < 13) && (fQsqdc > 15) && (fDif89c > 1.7) )
				{
					goto JUDGE_SHAKE_OPEN;
				}
			}
		}
		*/

		// 1、DIF21 上穿 0  
		{
			++iCurCondition;

			if ( BeValidFloat(fDif21Pre) && BeValidFloat(fDif21) )
			{
				if ( fDif21Pre <= 0 && fDif21 > 0 )
				{
					goto JUDGE_SHAKE_OPEN;
				}
			}
		}

		// 2、DIF89 上穿 0
		{
			++iCurCondition;

			if ( BeValidFloat(fDif89Pre) && BeValidFloat(fDif89) )
			{
				if ( fDif89Pre <= 0 && fDif89 > 0 )
				{
					goto JUDGE_SHAKE_OPEN;
				}
			}
		}
	}
		
	return false;
	
JUDGE_SHAKE_OPEN:
	{
		// 判断震荡:
		if ( JudgeShakeOC(bFuture, iPos, pIndex) )
		{
			// debug						
			pIndexNode[iPos].m_stDebugInfo.m_bShake		= true;
			pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
			pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
			
			// 震荡,不开仓
			return false;
		}
		else
		{
			// debug			
			pIndexNode[iPos].m_stDebugInfo.m_bShake		= false;
			pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
			pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
			
			// 记录此时的止损价格:
			pIndexNode[iPos].m_fZhisun = GetzhisunFromIndex(iPos, pIndex);
			
			// 不震荡,开仓
			return true;
		}
	}
	
	return false;
}

bool32 CalcConditionSaneDTCC(bool32 bFuture, int32 iPos, int32 iDtocConditions, T_IndexOutArray* pIndex, const CArray<CKLine, CKLine>& aKLines, /*const */CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues)
{
	// debug
	int32 iCurCondition = 0;
	//

	CKLine* pKLine = (CKLine*)aKLines.GetData();
	T_SaneIndexNode* pIndexNode = (T_SaneIndexNode*)aSaneIndexValues.GetData();

	if ( NULL == pKLine || NULL == pIndexNode )
	{
		return false;
	}

	if ( iPos < 1 || iPos >= aSaneIndexValues.GetSize() )
	{
		return false;
	}
	
	/*if ( 0 )
	{												
		return false;
	}
	else*/
	{	
		float fDif89	= Getdif89FromIndex(iPos, pIndex);
		float fDif89Pre	= Getdif89FromIndex(iPos - 1, pIndex);
		float fDif21	= Getdif21FromIndex(iPos, pIndex);
		float fDif21Pre	= Getdif21FromIndex(iPos - 1, pIndex);
		float fDif21Pre2= Getdif21FromIndex(iPos - 2, pIndex);
		float fQsqd		= GetqsqdFromIndex(iPos, pIndex);
		float fQsqdPre	= GetqsqdFromIndex(iPos - 1, pIndex);
		float fMaqsqd	= GetmaqsqdFromIndex(iPos, pIndex);
		float fMaqsqdPre= GetmaqsqdFromIndex(iPos - 1, pIndex);
		float fMaqsqdc  = GetmaqsqdcFromIndex(iPos, pIndex);
		float fMaqsqdcPre = GetmaqsqdcFromIndex(iPos - 1, pIndex);		
		float fPriceNew = pKLine[iPos].m_fPriceClose;
		float fQsqdc	= GetqsqdcFromIndex(iPos, pIndex);
		float fQsqdcPre	= GetqsqdcFromIndex(iPos - 1, pIndex);
		float fQsqdcPre2= GetqsqdcFromIndex(iPos - 2, pIndex);
		float fHlmaqsqdc= GethlmaqsqdcFromIndex(iPos, pIndex);
		float fLlv5		= Getllv5FromIndex(iPos, pIndex);
		float fLlv13	= Getllv13FromIndex(iPos, pIndex);
		float fDif89c	= Getdif89cFromIndex(iPos, pIndex);
		float fDif21c	= Getdif21cFromIndex(iPos, pIndex);
		float fqsqdzd	= GetqsqdzdFromIndex(iPos, pIndex);
		float fDif89c6  = Getdif89c6FromIndex(iPos, pIndex);
		float fHhvDif21 = Gethdif21FromIndex(iPos, pIndex);
		float fHigh5	= Gethigh5FromIndex(iPos, pIndex);
		float fHigh8	= Gethigh8FromIndex(iPos, pIndex);

		// 1、DIF21 下穿 0  
		{
			++iCurCondition;
			
			if ( BeValidFloat(fDif21Pre) && BeValidFloat(fDif21) )
			{
				if ( fDif21Pre >= 0 && fDif21 < 0 )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}
		}

		// 2、DIF89 下穿 0
		{
			++iCurCondition;
			
			if ( BeValidFloat(fDif89Pre) && BeValidFloat(fDif89) )
			{
				if ( fDif89Pre >= 0 && fDif89 < 0 )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}
		}



		/*
		if ( iPos == 1470 )
		{
			iPos = iPos;
		}


		int32 iPosDTOC	= -1;

		for (int32 iIndexCondition = iPos - 1; iIndexCondition > 0; iIndexCondition--)
		{
			// 找到前一个开仓点的开仓条件:
			if ( ESISDTOC == pIndexNode[iIndexCondition].m_eSaneIndexState || ESISKCDO == pIndexNode[iIndexCondition].m_eSaneIndexState )
			{
				iPosDTOC = iIndexCondition;
				break;
			}
		}
	
		//  1、QSQD < = DIF21 AND REF(QSQD,1) > REF(DIF21,1)  AND DIF89>=DIF21  AND DIF21<3.5 AND QSQDC<-30 AND DIF89C<-3.5
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21) && BeValidFloat(fDif21Pre) && BeValidFloat(fQsqdc) && BeValidFloat(fDif89) )
			{ 
				if ( BeNeed(iCurCondition, 2) && (fQsqd <= fDif21) && (fQsqdPre > fDif21Pre) && (fDif89 >= fDif21) && (fDif21 < 3.5) && (fQsqdc < -30) && (fDif89c < -3.5) )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}			
		}
		
		// 2、QSQD < = DIF21 AND REF(QSQD,1) > REF(DIF21,1)  AND DIF89>=DIF21  AND DIF21<0.5 AND QSQDC<-20 AND DIF21C<-3
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21) && BeValidFloat(fDif21Pre) && BeValidFloat(fQsqdc) && BeValidFloat(fDif89) )
			{ 
				if ( BeNeed(iCurCondition, 2) && (fQsqd <= fDif21) && (fQsqdPre > fDif21Pre) && (fDif89 >= fDif21) && (fDif21 < 0.5) && (fQsqdc < -20) && (fDif21c < -3) )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}			
		}
	
		// 3、QSQD < = DIF21 AND REF(QSQD,1) > REF(DIF21,1)  AND DIF89<DIF21
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21) && BeValidFloat(fDif21Pre) && BeValidFloat(fQsqdc) && BeValidFloat(fDif89) )
			{ 
				if ( BeNeed(iCurCondition, 2) && (fQsqd <= fDif21) && (fQsqdPre > fDif21Pre) && (fDif89 < fDif21) )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}			
		}
	
		// 4、MAQSQD < = DIF21 AND REF(MAQSQD,1) > REF(DIF21,1) AND QSQDZD>14 AND DIF21C<-3
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21) && BeValidFloat(fDif21Pre) && BeValidFloat(fQsqdc) && BeValidFloat(fDif89) )
			{ 
				if ( BeNeed(iCurCondition, 2) && (fMaqsqd <= fDif21) && (fMaqsqdPre > fDif21Pre) && (fqsqdzd > 14) && (fDif21c < -3) )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}
		}

		// 5、	DIF21 < 0  AND REF(DIF21,1) > 0 AND DIF21C<-3
		{
			++iCurCondition;

			if ( BeValidFloat(fDif21) && BeValidFloat(fDif21Pre) && BeValidFloat(fDif21c) )
			{
				if ( BeNeed(iCurCondition, 2) && (fDif21 < 0) && (fDif21Pre > 0) && (fDif21c < -3) )
				{
						pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
						pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
						return true;
				}
			}
		}

		// 6、MAQSQD <=0 AND REF(MAQSQD,1)>0
		{
			++iCurCondition;
			
			if ( BeValidFloat(fMaqsqd) && BeValidFloat(fMaqsqdPre) )
			{
				if ( BeNeed(iCurCondition, 2) && (fMaqsqd <= 0) && (fMaqsqdPre > 0) )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}
		}

		// 7、CLOSE <=ZHISUN
		{
			++iCurCondition;

			if ( iPosDTOC >= 0 && iPosDTOC < aSaneIndexValues.GetSize() )
			{
				float fZhisun = GetzhisunFromIndex(iPosDTOC, pIndex);

				if ( BeValidFloat(fPriceNew) && BeValidFloat(fZhisun) )
				{
					if ( BeNeed(iCurCondition, 2) && (fPriceNew <= fZhisun) )
					{
						pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
						pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
						return true;
					}
				}				
			}
		}

		// 8.	DIF21>DIF89 AND QSQD<=MAQSQD AND REF(QSQD,1)>REF(MAQSQD,1)) AND DIF21<2.5
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21) && BeValidFloat(fDif21Pre) && BeValidFloat(fQsqdc) && BeValidFloat(fDif89) )
			{ 
				if ( BeNeed(iCurCondition, 2) && (fDif21 > fDif89) && (fQsqd <= fMaqsqd) && (fQsqdPre > fMaqsqdPre) && (fDif21 < 2.5) )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}
		}

		// 9.	DIF21<DIF89 AND DIF89<13 AND DIF89C<-1 AND QSQD<=MAQSQD AND REF(QSQD,1)>REF(MAQSQD,1)) AND DIF21<2
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21) && BeValidFloat(fDif21Pre) && BeValidFloat(fQsqdc) && BeValidFloat(fDif89) )
			{ 
				if ( BeNeed(iCurCondition, 2) && (fDif21 < fDif89) && (fDif89 < 13) && (fDif89c < -1) && (fQsqd <= fMaqsqd) && (fQsqdPre > fMaqsqdPre) && (fDif21 < 2) )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}
		}

		// 10.	QSQD < = DIF21 AND REF(QSQD,1) > REF(DIF21,1)  AND MAQSQD<=DIF21 AND MAQSQDC<1
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21) && BeValidFloat(fDif21Pre) && BeValidFloat(fQsqdc) && BeValidFloat(fDif89) )
			{ 
				if ( BeNeed(iCurCondition, 2) && (fQsqd <= fDif21) && (fQsqdPre > fDif21Pre) && (fMaqsqd <= fDif21) && (fMaqsqdc < 1) )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}
		}	

		// 11.	QSQD < = DIF21 AND REF(QSQD,1) > REF(DIF21,1) AND QSQDC+REF(QSQDC,1)<-60
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21) && BeValidFloat(fDif21Pre) && BeValidFloat(fQsqdc) && BeValidFloat(fDif89) )
			{ 
				if ( BeNeed(iCurCondition, 2) && (fQsqd <= fDif21) && (fQsqdPre > fDif21Pre) && (fQsqdc + fQsqdcPre < -60) )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}
		}	
		*/
	}
	
	return false;
}

bool32 CalcConditionSaneKTOC(bool32 bFuture, int32 iPos, bool32 bHis, T_IndexOutArray* pIndex, const CArray<CKLine, CKLine>& aKLines, CArray<T_SaneIndexNode, T_SaneIndexNode>& aIndexNodes)
{
	// debug 
	int32 iCurCondition = 0;
	//

	if ( iPos == 4387 )
	{
		iPos = iPos;
	}

	int32 iSizeKLine = aKLines.GetSize();
	int32 iSizeIndex = aIndexNodes.GetSize();
	
	if ( iSizeKLine <= 0 || iSizeIndex <= 0 || NULL == pIndex )
	{
		return false;
	}
	
	CKLine* pKLine = (CKLine*)aKLines.GetData();
	T_SaneIndexNode* pIndexNode = (T_SaneIndexNode*)aIndexNodes.GetData();

	if ( NULL == pKLine || NULL == pIndexNode )
	{
		return false;
	}

	if ( iPos < 3 )
	{
		// 要判断前一点的强度,所以第一个节点不用管
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 期货:
	if ( !bFuture )
	{ 	
		return false;
	}
	else
	{
		float fDif89	= Getdif89FromIndex(iPos, pIndex);
		float fDif89Pre	= Getdif89FromIndex(iPos - 1, pIndex);
		float fQsqd		= GetqsqdFromIndex(iPos, pIndex);
		float fQsqdPre	= GetqsqdFromIndex(iPos - 1, pIndex);
		float fqsqdzd	= GetqsqdzdFromIndex(iPos, pIndex);		
		float fMaqsqd   = GetmaqsqdFromIndex(iPos, pIndex);
		float fMaqsqdPre= GetmaqsqdFromIndex(iPos - 1, pIndex);
		float fMaqsqdPre3 = GetmaqsqdFromIndex(iPos - 3, pIndex);
		float fDif89c	= Getdif89cFromIndex(iPos, pIndex);
		float fQsqdc	= GetqsqdcFromIndex(iPos, pIndex);
		float fQsqdcPre	= GetqsqdcFromIndex(iPos - 1, pIndex);
		float fQsqdcPre2= GetqsqdcFromIndex(iPos - 2, pIndex);
		float fQsqdcPre3= GetqsqdcFromIndex(iPos - 3, pIndex);
		float fHigh		= pKLine[iPos].m_fPriceHigh;
		float fZhiying  = GetzhiyingFromIndex(iPos, pIndex);
		float fHlmaqsqdc= GethlmaqsqdcFromIndex(iPos, pIndex);
		float fHhv13	= Gethhv13FromIndex(iPos, pIndex);
		float fDif21	= Getdif21FromIndex(iPos, pIndex);
		float fDif89c6  = Getdif89c6FromIndex(iPos, pIndex);
		float fDifcdc	= GetdifcdcFromIndex(iPos, pIndex);
		float fMaxdif	= GetmaxdifFromIndex(iPos, pIndex);
		float fMindif89 = Getmindif89FromIndex(iPos, pIndex);
		float fLow5		= Getlow5FromIndex(iPos, pIndex);
		float fLow8		= Getlow8FromIndex(iPos, pIndex);
		float fDif21c	= Getdif21cFromIndex(iPos, pIndex);
		float fDif21Pre = Getdif21FromIndex(iPos - 1, pIndex);
						
		// 1.	QSQD < = DIF21 AND REF(QSQD,1) > REF(DIF21,1)  AND DIF89<=DIF21 AND DIF21< 2  AND DIF89< 10 AND DIFCDC>=13 AND DIF89C< 0  DIF89C6< 0
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fDif21) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21Pre) && BeValidFloat(fDif89) && BeValidFloat(fDif21) && BeValidFloat(fDifcdc) && BeValidFloat(fqsqdzd) )
			{
				if ( BeNeed(iCurCondition, 3) && (fQsqd <= fDif21) && (fQsqdPre > fDif21Pre) && (fDif89 <= fDif21) && (fDif21 < 2) && (fDif89 < 10) && (fDifcdc >= 13) && (fDif89c < 0) && (fDif89c6 < 0) )
				{			
					goto JUDGE_SHAKE_OPEN;
				}
			}			
		}
		
		// 2. QSQD < = DIF21 AND REF(QSQD,1) > REF(DIF21,1)  AND DIF21<3 AND ABS(DIF89)<13 AND DIFCDC<13 AND QSQDC<-30 AND DIF89C <-1 AND DIF89C6<-2
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fDif21) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21Pre) && BeValidFloat(fDif89) && BeValidFloat(fDif21) && BeValidFloat(fDifcdc) && BeValidFloat(fqsqdzd) )
			{
				if ( BeNeed(iCurCondition, 3) && (fQsqd <= fDif21) && (fQsqdPre > fDif21Pre) && (fDif21 < 3) && (fabs(fDif89) < 13) && (fDifcdc < 13) && (fQsqdc < -30) && (fDif21c < -1) && (fDif89c6 < -2) )
				{			
					goto JUDGE_SHAKE_OPEN;
				}
			}
		}
		
		// 3.	QSQD < = DIF21 AND REF(QSQD,1) > REF(DIF21,1)  AND DIF89>DIF21   AND QSQDC<-30  AND DIF21<2  AND DIF89<30 AND DIF89>5 AND DIF89C<-3		
		{
			++iCurCondition;

			if ( BeValidFloat(fQsqd) && BeValidFloat(fDif21) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21Pre) && BeValidFloat(fDif89) && BeValidFloat(fQsqdc))
			{
				if ( BeNeed(iCurCondition, 3) && (fQsqd <= fDif21) && (fQsqdPre > fDif21Pre) && (fDif89 > fDif21) && (fQsqdc < -30) && (fDif21 < 2) && (fDif89 < 30) && (fDif89 > 5) && (fDif89c < -2) )
				{
					goto JUDGE_SHAKE_OPEN;
				}
			}
		}
		
		// 4. MAQSQD<MINDIF AND DIF21<-3 AND DIF89C <-2 AND DIF89<10 AND ((QSQDC <-5 AND REF(QSQDC,1)>=0 AND MAQSQD-QSQD>-1) OR (QSQD<MAQSQD AND REF(QSQD,1)>REF(MAQSQD,1)))
		{
			++iCurCondition;
			
			if ( BeValidFloat(fMaqsqd) && BeValidFloat(fDif21) && BeValidFloat(fMaqsqdPre) && BeValidFloat(fDif21Pre) && BeValidFloat(fDif89) && BeValidFloat(fQsqd) )
			{
				if ( BeNeed(iCurCondition, 3) && (fMaqsqd < fMindif89) && (fDif21 < -3) && (fDif89c < -2) && (fDif89 < 10) && ((fQsqdc < -5 && fQsqdcPre >= 0 && fMaqsqd - fQsqd > -1) || (fQsqd < fMaqsqd && fQsqdPre > fMaqsqdPre)) )
				{
					goto JUDGE_SHAKE_OPEN;
				}
			}
		}
		
		// 5.	QSQD < = DIF21 AND REF(QSQD,1) > REF(DIF21,1)  AND QSQDC+REF(QSQDC,1) < -60 AND DIF89C<-4
		{
			++iCurCondition;
			
			if ( BeValidFloat(fMaqsqd) && BeValidFloat(fDif21) && BeValidFloat(fMaqsqdPre) && BeValidFloat(fDif21Pre) && BeValidFloat(fDif89) && BeValidFloat(fQsqd) )
			{
				if ( BeNeed(iCurCondition, 3) && (fQsqd <= fDif21) && (fQsqdPre > fDif21Pre) && (fQsqdc + fQsqdcPre < -60) && (fDif89c < -4) )
				{
					goto JUDGE_SHAKE_OPEN;
				}
			}
		}

		// 6.	MAQSQD < = DIF21 AND REF(MAQSQD,1) > REF(DIF21,1)  AND QSQDC+REF(QSQDC,1) < -75 AND DIF89C<-3.5
		{
			++iCurCondition;
			
			if ( BeValidFloat(fMaqsqd) && BeValidFloat(fDif21) && BeValidFloat(fMaqsqdPre) && BeValidFloat(fDif21Pre) && BeValidFloat(fDif89) && BeValidFloat(fQsqd) )
			{
				if ( BeNeed(iCurCondition, 3) && (fMaqsqd <= fDif21) && (fMaqsqdPre > fDif21Pre) && (fQsqdc + fQsqdcPre < -75) && (fDif89c < -3.5) )
				{
					goto JUDGE_SHAKE_OPEN;
				}
			}
		}
		
		// 7.	QSQD < = DIF89 AND REF(QSQD,1) > REF(DIF89,1)  AND DIF21<3 AND ABS(DIF89)<13 AND DIFCDC<13 AND QSQDC<-20 AND DIF89C <-0.4 DIF89 < 0 
		{
			++iCurCondition;
				
			if ( BeValidFloat(fMaqsqd) && BeValidFloat(fMaqsqdPre) && BeValidFloat(fDif21) && BeValidFloat(fDif89) && BeValidFloat(fDif89c) && BeValidFloat(fQsqdc) )
			{
				if ( BeNeed(iCurCondition, 3) && (fQsqd <= fDif89) && (fQsqdPre > fDif89Pre) && (fDif21 < 3) && (fabs(fDif89) < 13) && (fDifcdc < 13) && (fQsqdc < -20) && (fDif89c < -0.4) && (fDif89 < 0) )
				{
					goto JUDGE_SHAKE_OPEN;
				}
			}
		}
	}

	return false;
	
JUDGE_SHAKE_OPEN:
	{
		// 判断震荡:
		if ( JudgeShakeOC(bFuture, iPos, pIndex) )
		{
			// debug						
			pIndexNode[iPos].m_stDebugInfo.m_bShake		= true;
			pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
			pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;

			// 震荡,不开仓
			return false;
		}
		else
		{
			// debug			
			pIndexNode[iPos].m_stDebugInfo.m_bShake		= false;
			pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
			pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;

			// 记录此时的止损价格:
			pIndexNode[iPos].m_fZhisun = GetzhisunFromIndex(iPos, pIndex);

			// 不震荡,开仓
			return true;
		}
	}
		
	return false;
}

bool32 CalcConditionSaneKTCC(bool32 bFuture, int32 iPos, int32 iKtocConditions, T_IndexOutArray* pIndex, const CArray<CKLine, CKLine>& aKLines, CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues)
{
		// debug
	int32 iCurCondition = 0;
	//

	CKLine* pKLine = (CKLine*)aKLines.GetData();
	T_SaneIndexNode* pIndexNode = (T_SaneIndexNode*)aSaneIndexValues.GetData();

	if ( NULL == pKLine || NULL == pIndexNode )
	{
		return false;
	}

	if ( iPos < 1 || iPos >= aSaneIndexValues.GetSize() )
	{
		return false;
	}
	
	if ( 0 )
	{												
		return false;
	}
	else
	{	
		float fDif89	= Getdif89FromIndex(iPos, pIndex);
		float fDif89Pre	= Getdif89FromIndex(iPos - 1, pIndex);
		float fDif21	= Getdif21FromIndex(iPos, pIndex);
		float fDif21Pre	= Getdif21FromIndex(iPos - 1, pIndex);
		float fDif21Pre2= Getdif21FromIndex(iPos - 2, pIndex);
		float fQsqd		= GetqsqdFromIndex(iPos, pIndex);
		float fQsqdPre	= GetqsqdFromIndex(iPos - 1, pIndex);
		float fMaqsqd	= GetmaqsqdFromIndex(iPos, pIndex);
		float fMaqsqdPre= GetmaqsqdFromIndex(iPos - 1, pIndex);
		float fMaqsqdc  = GetmaqsqdcFromIndex(iPos, pIndex);
		float fMaqsqdcPre = GetmaqsqdcFromIndex(iPos - 1, pIndex);		
		float fPriceNew = pKLine[iPos].m_fPriceClose;
		float fQsqdc	= GetqsqdcFromIndex(iPos, pIndex);
		float fQsqdcPre	= GetqsqdcFromIndex(iPos - 1, pIndex);
		float fQsqdcPre2= GetqsqdcFromIndex(iPos - 2, pIndex);
		float fHlmaqsqdc= GethlmaqsqdcFromIndex(iPos, pIndex);
		float fLlv5		= Getllv5FromIndex(iPos, pIndex);
		float fLlv13	= Getllv13FromIndex(iPos, pIndex);
		float fDif89c	= Getdif89cFromIndex(iPos, pIndex);
		float fDif21c	= Getdif21cFromIndex(iPos, pIndex);
		float fqsqdzd	= GetqsqdzdFromIndex(iPos, pIndex);
		float fDif89c6  = Getdif89c6FromIndex(iPos, pIndex);
		float fHhvDif21 = Gethdif21FromIndex(iPos, pIndex);
		float fHigh5	= Gethigh5FromIndex(iPos, pIndex);
		float fHigh8	= Gethigh8FromIndex(iPos, pIndex);

		if ( iPos == 391 )
		{
			iPos = iPos;
		}

		int32 iPosDTOC	= -1;

		for (int32 iIndexCondition = iPos - 1; iIndexCondition > 0; iIndexCondition--)
		{
			// 找到前一个开仓点的开仓条件:
			if ( ESISKTOC == pIndexNode[iIndexCondition].m_eSaneIndexState || ESISDCKO == pIndexNode[iIndexCondition].m_eSaneIndexState )
			{
				iPosDTOC = iIndexCondition;
				break;
			}
		}
		
		//  1. QSQD > = DIF21 AND REF(QSQD,1) < REF(DIF21,1)  AND DIF89<=DIF21  AND DIF21>-3.5 AND QSQDC>30
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21) && BeValidFloat(fDif21Pre) && BeValidFloat(fQsqdc) && BeValidFloat(fDif89) )
			{ 
				if ( BeNeed(iCurCondition, 4) && (fQsqd >= fDif21) && (fQsqdPre < fDif21Pre) && (fDif89 <= fDif21) && (fDif21 > -3.5) && (fQsqdc > 30) )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}			
		}
				
		// 2. QSQD > = DIF21 AND REF(QSQD,1) < REF(DIF21,1)  AND DIF89<=DIF21  AND DIF21>-1 AND QSQDC>20 AND DIF21C>3
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21) && BeValidFloat(fDif21Pre) && BeValidFloat(fQsqdc) && BeValidFloat(fDif89) )
			{ 
				if ( BeNeed(iCurCondition, 4) && (fQsqd >= fDif21) && (fQsqdPre < fDif21Pre) && (fDif89 <= fDif21) && (fDif21 > -1) && (fQsqdc > 20) && (fDif21c > 3) )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}			
		}
			
		// 3. QSQD > = DIF21 AND REF(QSQD,1) < REF(DIF21,1)  AND DIF89>DIF21
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21) && BeValidFloat(fDif21Pre) && BeValidFloat(fQsqdc) && BeValidFloat(fDif89) )
			{ 
				if ( BeNeed(iCurCondition, 4) && (fQsqd >= fDif21) && (fQsqdPre < fDif21Pre) && (fDif89 > fDif21) )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}			
		}
			
		// 4. MAQSQD > = DIF21 AND REF(MAQSQD,1) < REF(DIF21,1)  AND QSQDZD>14  AND DIF21C>3
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21) && BeValidFloat(fDif21Pre) && BeValidFloat(fQsqdc) && BeValidFloat(fDif89) )
			{ 
				if ( BeNeed(iCurCondition, 4) && (fMaqsqd >= fDif21) && (fMaqsqdPre < fDif21Pre) && (fqsqdzd > 14) && (fDif21c > 3) )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}
		}
		
		// 5.	DIF21 > 0  AND REF(DIF21,1) < 0 AND DIF21C>3
		{
			++iCurCondition;

			if ( BeValidFloat(fDif21) && BeValidFloat(fDif21Pre) && BeValidFloat(fDif21c) )
			{
				if ( BeNeed(iCurCondition, 4) && (fDif21 > 0) && (fDif21Pre < 0) && (fDif21c > 3) )
				{
						pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
						pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
						return true;
				}
			}
		}
		
		// 6. MAQSQD >=0 AND REF(MAQSQD,1)<0
		{
			++iCurCondition;
			
			if ( BeValidFloat(fMaqsqd) && BeValidFloat(fMaqsqdPre) )
			{
				if ( BeNeed(iCurCondition, 4) && (fMaqsqd >= 0) && (fMaqsqdPre < 0) )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}
		}

		// 7.	CLOSE >=ZHIYING
		{
			++iCurCondition;

			if ( iPosDTOC >= 0 && iPosDTOC < aSaneIndexValues.GetSize() )
			{
				float fZhiying = GetzhiyingFromIndex(iPosDTOC, pIndex);

				if ( BeValidFloat(fPriceNew) && BeValidFloat(fZhiying) )
				{
					if ( BeNeed(iCurCondition, 4) && (fPriceNew >= fZhiying) )
					{
						pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
						pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
						return true;
					}
				}				
			}
		}
		
		// 8.	DIF21<DIF89 AND QSQD>=MAQSQD AND REF(QSQD,1)<REF(MAQSQD,1)) AND DIF21> - 3
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21) && BeValidFloat(fDif21Pre) && BeValidFloat(fQsqdc) && BeValidFloat(fDif89) )
			{ 
				if ( BeNeed(iCurCondition, 4) && (fDif21 < fDif89) && (fQsqd >= fMaqsqd) && (fQsqdPre < fMaqsqdPre) && (fDif21 > -3) )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}
		}
		
		// 9.	QSQD > = DIF21 AND REF(QSQD,1) < REF(DIF21,1)  AND MAQSQD>=DIF21 AND MAQSQDC>-1
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21) && BeValidFloat(fDif21Pre) && BeValidFloat(fQsqdc) && BeValidFloat(fDif89) )
			{ 
				if ( BeNeed(iCurCondition, 4) && (fQsqd >= fDif21) && (fQsqdPre < fDif21Pre) && (fMaqsqd >= fDif21) && (fMaqsqdc > -1) )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}
		}	
		
		// 10.	QSQD > = DIF21 AND REF(QSQD,1) < REF(DIF21,1) AND QSQDC+REF(QSQDC,1) > 60 DIF89C>3
		{
			++iCurCondition;
			
			if ( BeValidFloat(fQsqd) && BeValidFloat(fQsqdPre) && BeValidFloat(fDif21) && BeValidFloat(fDif21Pre) && BeValidFloat(fQsqdc) && BeValidFloat(fDif89) )
			{ 
				if ( BeNeed(iCurCondition, 4) && (fQsqd >= fDif21) && (fQsqdPre < fDif21Pre) && (fQsqdc + fQsqdcPre > 60) && (fDif89c > 3) )
				{
					pIndexNode[iPos].m_stDebugInfo.m_iNodePos	= iPos;
					pIndexNode[iPos].m_stDebugInfo.m_iCondition	= iCurCondition;
					return true;
				}
			}
		}	
	}
		
	return false;
}