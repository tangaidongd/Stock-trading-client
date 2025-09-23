#include "stdafx.h"
#include <Windows.h>
#include <math.h>
#include <stdio.h>
#include "SpecialTechIndexManager.h"
#include <vector>
#include <algorithm>
using namespace std;


bool cmpUp(const ST_CYCLE_INFO& a,const ST_CYCLE_INFO& b)
{
	// 等于0的时候用索引排序降序
	if (fabs(a.fData - b.fData) < 1e-7)
	{
		return a.iIndex >  b.iIndex;
	}
	return  a.fData - b.fData < 1e-7;
}

bool cmpDown(const ST_CYCLE_INFO& a,const ST_CYCLE_INFO& b)
{
	// 等于0的时候用索引排序升序
	if (fabs(a.fData - b.fData) < 1e-7)
	{
		return a.iIndex <  b.iIndex;
	}
	return a.fData - b.fData > 1e-7; 
}



CSpecialTechIndexManager::CSpecialTechIndexManager()
{
	// ....
}


CSpecialTechIndexManager::~CSpecialTechIndexManager()
{
	// ....
}




void CSpecialTechIndexManager::GetMaxMin(Kline *pKline, int nKline, float &fMax, float &fMin)
{
	int i;
	if(pKline==NULL)
		return;

	for( i=0;i<nKline;i++)
	{
		if((pKline+i)->high >fMax)
		{
			fMax=(pKline+i)->high;
		}
		if((pKline+i)->low<fMin)
		{
			fMin=(pKline+i)->low;
		}
	}
	if(fMax>Max_MoveChenBen) fMax = Max_MoveChenBen;
	if(fMin<0.001) fMin = 0.001;
}



int CSpecialTechIndexManager::GetRuler(float fMax, float fMin, float &fRuler)
{
	fRuler = 100;
	if(fMax-fMin<10)
		fRuler = 100;
	else if(fMax-fMin<20)
		fRuler = 50;
	else if(fMax-fMin<40)
		fRuler = 20;
	else if(fMax-fMin<100)
		fRuler = 10;
	else if(fMax-fMin<1000)
		fRuler = 1;
	else 
		fRuler = 0.01;

	int n =(int)( (fMax - fMin)*fRuler+10);
	return n;
}

float CSpecialTechIndexManager::GetCapital(CMerch *pCMerch)
{
	if (pCMerch && pCMerch->m_pFinanceData)
		return pCMerch->m_pFinanceData->m_fCircAsset;

	return 0.0;
}

float CSpecialTechIndexManager::GetTotalCapital(CMerch *pCMerch)
{
	if (pCMerch && pCMerch->m_pFinanceData)
		return pCMerch->m_pFinanceData->m_fAllCapical;

	return 0.0;
}

int32 CSpecialTechIndexManager::GetHyzsCode(CMerch *pCMerch)
{
	if (!pCMerch)
		return 0.0;

	// 得到所属板块信息
	int32 iBlockid = -1;
	
	CBlockCollection::BlockArray aBlocks;
	CBlockConfig::Instance()->GetBlocksByMerch(pCMerch->m_MerchInfo.m_iMarketId, pCMerch->m_MerchInfo.m_StrMerchCode, aBlocks);
	for ( int32 i = 0; i < aBlocks.GetSize(); i++ )
	{
		if ( aBlocks[i]->m_blockCollection.m_StrName == L"行业板块" )
		{
			iBlockid = aBlocks[i]->m_blockInfo.m_iBlockId;
		}
	}

	return iBlockid;
}


bool32 CSpecialTechIndexManager::GetTimeIntervalInfo(int32 iMinUser, int32 iDayUser, E_NodeTimeInterval eNodeTimeInterval, E_NodeTimeInterval &eNodeTimeIntervalCompare, E_KLineTypeBase &eKLineTypeCompare,int32 &iScale)
{
	eNodeTimeIntervalCompare = ENTIDay;
	iScale = 1;

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
		{
			return false;
		}
		break; 
	}

	return true;
}

//
//
//CMerch * CSpecialTechIndexManager::GetMerchAccordExpMerch( CMerch* pMerch )
//{
//	if ( NULL == pMerch || NULL == m_pViewData )
//	{
//		return NULL;
//	}
//
//	// 默认都调上证指数
//	CMerch* pMerchExp = NULL;	
//	//m_pViewData->m_MerchManager.FindMerch(L"000001", 0, pMerchExp);
//
//	//	
//	if ( 1 == pMerch->m_Market.m_Breed.m_iBreedId )
//	{				
//		if ( 1004 == pMerch->m_Market.m_MarketInfo.m_iMarketId )
//		{
//			// 2、深圳中小板调用中小板指数
//			m_pViewData->m_MerchManager.FindMerch(/*L"399005"*/L"399101", 1000, pMerchExp);
//		}
//		else if ( 1008 == pMerch->m_Market.m_MarketInfo.m_iMarketId )
//		{
//			// 3、深圳创业板调用创业板指数
//			m_pViewData->m_MerchManager.FindMerch(/*L"399006"*/L"399102", 1000, pMerchExp);
//		}
//		else
//		{
//			// 4、其余的全都调用深成指
//			m_pViewData->m_MerchManager.FindMerch(L"399001", 1000, pMerchExp);
//		}	
//	}
//
//	// 默认都调上证指数
//	if ( NULL == pMerchExp )
//	{
//		m_pViewData->m_MerchManager.FindMerch(L"000001", 0, pMerchExp);
//	}
//
//	//
//	return pMerchExp;
//}


void CSpecialTechIndexManager::GetExpData(CMerch *pMerch, int32 iMinUser, int32 iDayUser, E_NodeTimeInterval eTimeIntervalFull, CArray<CKLine, CKLine> * pKLine)
{
	if (!pMerch )
		return;
	
	// 先获取对应的K线
	int32 iPosFound;
	E_NodeTimeInterval eNodeTimeIntervalCompare = ENTIDay;
	E_KLineTypeBase    eKLineType			    = EKTBDay;
	int32 iScale = 1;
	if (!GetTimeIntervalInfo(iMinUser, iDayUser, eTimeIntervalFull, eNodeTimeIntervalCompare, eKLineType, iScale))
	{
		return ;
	}

	CMerchKLineNode *pKLineRequest = NULL;
	CArray<CKLine, CKLine> aKLinesCompare;
	pMerch->FindMerchKLineNode(eKLineType, iPosFound, pKLineRequest);
	// 根本找不到K线数据， 那就不需要显示了
	if (NULL == pKLineRequest || 0 == pKLineRequest->m_KLines.GetSize())	
	{
		return ;
	}

	//////////////////////////////////////////////////////////////////////////			
	// 处理数据， 压缩数据
	switch (eTimeIntervalFull)
	{
	case ENTIMinute:
		{
			aKLinesCompare.SetSize(pKLineRequest->m_KLines.GetSize());	
			memcpyex(aKLinesCompare.GetData(), pKLineRequest->m_KLines.GetData(), sizeof(CKLine) * pKLineRequest->m_KLines.GetSize());						
		}
		break;
	case ENTIMinute5:
		{
			aKLinesCompare.SetSize(pKLineRequest->m_KLines.GetSize());	
			memcpyex(aKLinesCompare.GetData(), pKLineRequest->m_KLines.GetData(), sizeof(CKLine) * pKLineRequest->m_KLines.GetSize());
		}
		break;					
	case ENTIMinute15:
		{
			if (!CMerchKLineNode::CombinMinuteN(pMerch, 15, pKLineRequest->m_KLines, aKLinesCompare))						
			{
				ASSERT(0);
			}	
		}
		break;
	case ENTIMinute30:
		{
			if (!CMerchKLineNode::CombinMinuteN(pMerch, 30, pKLineRequest->m_KLines, aKLinesCompare))						
			{
				ASSERT(0);
			}
		}
		break;
	case ENTIMinute60:
		{	
			aKLinesCompare.SetSize(pKLineRequest->m_KLines.GetSize());	
			memcpyex(aKLinesCompare.GetData(), pKLineRequest->m_KLines.GetData(), sizeof(CKLine) * pKLineRequest->m_KLines.GetSize());
		}	
		break;
	case ENTIMinuteUser:
		{					
			if (!CMerchKLineNode::CombinMinuteN(pMerch, iMinUser, pKLineRequest->m_KLines, aKLinesCompare))						
			{
				ASSERT(0);
			}
		}	
		break;
	case ENTIDay:
		{
			aKLinesCompare.SetSize(pKLineRequest->m_KLines.GetSize());	
			memcpyex(aKLinesCompare.GetData(), pKLineRequest->m_KLines.GetData(), sizeof(CKLine) * pKLineRequest->m_KLines.GetSize());
		}
		break;
	case ENTIWeek:
		{
			if (!CMerchKLineNode::CombinWeek(pKLineRequest->m_KLines, aKLinesCompare))
			{
				ASSERT(0);
			}
		}
		break;
	case ENTIDayUser:
		{
			if (!CMerchKLineNode::CombinDayN(pKLineRequest->m_KLines, iDayUser, aKLinesCompare))
			{
				ASSERT(0);
			}
		}
		break;
	case ENTIMonth:
		{
			aKLinesCompare.SetSize(pKLineRequest->m_KLines.GetSize());	
			memcpyex(aKLinesCompare.GetData(), pKLineRequest->m_KLines.GetData(), sizeof(CKLine) * pKLineRequest->m_KLines.GetSize());
		}
		break;			

	case ENTIQuarter:
		{
			if (!CMerchKLineNode::CombinMonthN(pKLineRequest->m_KLines, 3, aKLinesCompare))
			{
				ASSERT(0);
			}
		}
		break;
	case ENTIYear:
		{
			if (!CMerchKLineNode::CombinMonthN(pKLineRequest->m_KLines, 12, aKLinesCompare))
			{
				ASSERT(0);
			}
		}
		break;
	default:
		ASSERT(0);
		break;
	}


	pKLine->RemoveAll();
	pKLine->SetSize(aKLinesCompare.GetSize());	
	memcpyex(pKLine->GetData(), aKLinesCompare.GetData(), sizeof(CKLine) * aKLinesCompare.GetSize());

	//OutputDebugString(L"======================================================");
	//for (int i = 0; i  < pKLine->GetSize(); ++i)
	//{
	//	float			m_fPriceOpen;		// 开
	//	float			m_fPriceClose;		// 收
	//	float			m_fPriceAvg;		// 结算（均）
	//	float			m_fPriceHigh;		// 高
	//	float			m_fPriceLow;		// 低

	//	CString strTemp;
	//	strTemp.Format(L"m_fPriceOpen = %.2f, m_fPriceClose = %.2f, m_fPriceAvg = %.2f, m_fPriceHigh = %.2f, m_fPriceLow = %.2f  \n",
	//		pKLine->GetAt(i).m_fPriceOpen, pKLine->GetAt(i).m_fPriceClose, pKLine->GetAt(i).m_fPriceAvg, pKLine->GetAt(i).m_fPriceHigh, pKLine->GetAt(i).m_fPriceLow);

	//	CString strTime;
	//	strTime.Format(L"%04d:%02d:%02d:%02d  ", pKLine->GetAt(i).m_TimeCurrent.GetYear(), pKLine->GetAt(i).m_TimeCurrent.GetMonth(), pKLine->GetAt(i).m_TimeCurrent.GetDay(), pKLine->GetAt(i).m_TimeCurrent.GetMinute());
	//	OutputDebugString(strTime);
	//	OutputDebugString(strTemp);
	//}

}


//
//float CSpecialTechIndexManager::GetCapital(CReportData *pdt)
//{
//	float f =0;
//
//	BASEINFO* pBase = NULL;
//	{
//		if(pdt !=NULL)
//		{
//			pBase = pdt->pBaseInfo;
//			if(pBase!=NULL)
//			{
//				if(pdt->kind == 2 || pdt->kind == 5 )
//					f = pBase->Bg *100;
//				else
//					f = pBase->ltAg *100;
//			}
//		}
//	}
//
//	return f;
//}



int32 CSpecialTechIndexManager::GetTopRange(INPUT_INFO *pInput)
{
	if (!pInput || !pInput->pData)
	{
		return 0;
	}

	for (int i = pInput->fInputParam->e; i >= pInput->fInputParam->b; i--)
	{
		float iMaxPos = 0.0;
		for (int j = i - 1; i >= pInput->fInputParam->b; j--)
		{
			if (pInput->fInputParam->line[i] > pInput->fInputParam->line[j])
			{
				iMaxPos++;
			}
			else
			{
				pInput->fOutputParam.line[i] = iMaxPos;
				break;
			}
		}
	}

	return 0;
}

int32 CSpecialTechIndexManager::GetLowRange(INPUT_INFO *pInput)
{

	if (!pInput || !pInput->pData)
	{
		return 0;
	}

	for (int i = pInput->fInputParam->e; i >= pInput->fInputParam->b; i--)
	{
		float iMinPos = 0.0;
		for (int j = i - 1; i >= pInput->fInputParam->b; j--)
		{
			if (pInput->fInputParam->line[i] < pInput->fInputParam->line[j])
			{
				iMinPos++;
			}
			else
			{
				pInput->fOutputParam.line[i] = iMinPos;
				break;
			}
		}
	}

	return 0;
}



int32 CSpecialTechIndexManager::GetFindHighOrLow(INPUT_INFO *pInput, bool bIsHigh)
{

	if (!pInput || !pInput->pData)
	{
		return 0;
	}

	if (!pInput->fInputParam[0].line || !pInput->fInputParam[1].line || !pInput->fInputParam[2].line || !pInput->fInputParam[3].line)
	{
		return 0;
	}

	int iNCycle = int(*pInput->fInputParam[1].line);
	int iMCycle = int(*pInput->fInputParam[2].line);
	int iTpos = int(*pInput->fInputParam[3].line);
	// 肯定是有四个参数的，所以也不用判断
	for (int i = pInput->fInputParam[0].e; i >= pInput->fInputParam[0].b; i--)
	{
		int iStart = i - iNCycle;// 数据其实位置
		if (iStart - iNCycle <= 0)
		{
			// 没有数据的赋空值
			for (int j = i; j >=0; j--)
			{
				pInput->fOutputParam.line[i] = 0.0;
			}
			break;
		}

		//std::sort(vUserBlockData.at(i).vMerchInfo.begin(),vUserBlockData.at(i).m_vMerchInfo.end()); // 先排序

		vector<ST_CYCLE_INFO> vstCycleInfo;
		for (int iPos = 0; iPos < iMCycle ; iPos++ )
		{
			if (iStart - iPos < 0)
			{
				break;
			}
			ST_CYCLE_INFO stInfo;
			stInfo.fData  = pInput->fInputParam[0].line[iStart-iPos];
			stInfo.iIndex = iStart-iPos;
			vstCycleInfo.push_back(stInfo);
		}

		// 通过排序来甄别最大最小
		if (bIsHigh)
		{
			std::sort(vstCycleInfo.begin(),vstCycleInfo.end(),cmpDown); // 先排序
		}
		else
		{
			std::sort(vstCycleInfo.begin(),vstCycleInfo.end(),cmpUp); // 先排序
		}

		if (vstCycleInfo.size() > 0 && iTpos > 0)
		{
			if (iTpos >= vstCycleInfo.size())
			{
				pInput->fOutputParam.line[i] = vstCycleInfo.at(vstCycleInfo.size() -1).fData;
			}	
			else
			{
				pInput->fOutputParam.line[i] = vstCycleInfo.at(iTpos -1).fData;
			}	
		}
		else
		{
			pInput->fOutputParam.line[i] = 0.0;
		}
	}
	return 0;
}



int32 CSpecialTechIndexManager::GetFindHighOrLowBars(INPUT_INFO *pInput, bool bIsHigh)
{

	if (!pInput || !pInput->pData)
	{
		return 0;
	}

	if (!pInput->fInputParam[0].line || !pInput->fInputParam[1].line || !pInput->fInputParam[2].line || !pInput->fInputParam[3].line)
	{
		return 0;
	}

	int iNCycle = int(*pInput->fInputParam[1].line);
	int iMCycle = int(*pInput->fInputParam[2].line);
	int iTpos = int(*pInput->fInputParam[3].line);
	// 肯定是有四个参数的，所以也不用判断
	for (int i = pInput->fInputParam[0].e; i >= pInput->fInputParam[0].b; i--)
	{
		int iStart = i - iNCycle;// 数据其实位置
		if (iStart - iNCycle <= 0)
		{
			// 没有数据的赋空值
			for (int j = i; j >=0; j--)
			{
				pInput->fOutputParam.line[i] = 0.0;
			}
			break;
		}

		//std::sort(vUserBlockData.at(i).vMerchInfo.begin(),vUserBlockData.at(i).m_vMerchInfo.end()); // 先排序

		vector<ST_CYCLE_INFO> vstCycleInfo;
		for (int iPos = 0; iPos < iMCycle ; iPos++ )
		{
			if (iStart - iPos < 0)
			{
				break;
			}
			ST_CYCLE_INFO stInfo;
			stInfo.fData  = pInput->fInputParam[0].line[iStart-iPos];
			stInfo.iIndex = iStart-iPos;
			vstCycleInfo.push_back(stInfo);
		}

		// 通过排序来甄别最大最小
		if (bIsHigh)
		{
			std::sort(vstCycleInfo.begin(),vstCycleInfo.end(),cmpDown); // 先排序
		}
		else
		{
			std::sort(vstCycleInfo.begin(),vstCycleInfo.end(),cmpUp); // 先排序
		}

		if (vstCycleInfo.size() > 0 && iTpos > 0)
		{
			if (iTpos >= vstCycleInfo.size())
			{
				int iCycleNum = i - vstCycleInfo.at(vstCycleInfo.size() -1).iIndex;
				//iCycleNum += vstCycleInfo.size();
				pInput->fOutputParam.line[i] = iCycleNum;
			}	
			else
			{
				int iCycleNum = i - vstCycleInfo.at(iTpos -1).iIndex;
				//iCycleNum += iTpos;
				pInput->fOutputParam.line[i] = iCycleNum;
			}	
		}
		else
		{
			pInput->fOutputParam.line[i] = 0.0;
		}
	}
	return 0;
}





void CSpecialTechIndexManager::GetMoveCBPerFoot(Kline *pKline, float *cnp, float ltp, float fMax, float fMin,int nFoot,float fRate)
{
	float fRuler;
	int n =GetRuler( fMax,  fMin, fRuler);

	int nBeginY =fMin*fRuler;
	int nEndY = fMax*fRuler;

	//--- 计算移动成本
	if(pKline[0].vol<=0)
		return;
	if(pKline[0].high>fMax)
		return;
	{
		int nLow = (pKline[0].low*fRuler+0.5);
		int nFen =( pKline[0].high- pKline[0].low)*fRuler+0.5;
		if(nFen<=0)
			return;
		//ASSERT(nLow>=nBeginY);
		if(nLow<nBeginY)
			nLow=nBeginY;
		nLow-=nBeginY;

		float fvol = pKline[0].vol*fRate/nFen;
		if(nFoot ==0 )
			fvol = ltp/nFen;
		//step 1:delete yestoday
		float rat = 1-pKline[0].vol*fRate/ltp;
		if(rat<0)
			rat = 0;
		if(rat>=1)
			return;

		for(int j=0;j<n;j++)
		{
			cnp[j]*=rat;
		}
		//step 2:add today
		for(int j=0;j<nFen;j++)
		{
			cnp[j+nLow]+=fvol;
		}
	}

	//normalizing
	float volume=0;
	for(int i=0;i<n;i++)
		volume+=cnp[i];
	if(volume<=0)
	{
		return;
	}
	volume=ltp/volume;//rate now;
	for(int i=0;i<n;i++)
	{
		cnp[i]*=volume;
	}

}


int CSpecialTechIndexManager::WinnerFun(INPUT_INFO *pInput, CMerch *pCMerch, int nFunc)
{

	int k=1;
	int l = 0;
	int j = 0;
	float f,fTemp;
	while(1)
	{
		if(pInput->pData == NULL)
		{
			k=0;
			break;
		}
		j=pInput->fInputParam[0].b;
		l=j;

		float fMax=0;
		float fMin=90000000;
		GetMaxMin(pInput->pData , pInput->nNumData , fMax, fMin);
		float fRuler;
		int n =GetRuler( fMax,  fMin, fRuler);

		if(n<=0 )
		{
			k=0;
			break;
		}
		CReportData* pdt = NULL;

		if( pCMerch !=NULL  && pCMerch->m_pFinanceData != NULL)
		{
			float ltp = pCMerch->m_pFinanceData->m_fCircAsset;

			ltp = ltp/100;
			if (ltp > 0.0)
			{
				//				k=0;
				//				break;
				float* cnp = new float[n];
				memset(cnp,0,n*4);
				if(nFunc == 21 ||nFunc == 44 )
				{
					for(int i=0;i<pInput->nNumData ;i++)
					{
						GetMoveCBPerFoot(pInput->pData+i, cnp, ltp, fMax, fMin, i);
						if(i<l)
						{
							continue;
						}
						f=pInput->fInputParam[0].line[i];//
						if(f>fMax )
						{
							f=0;
						}
						else if(f<fMin)
						{
							f=1;
						}
						else
						{
							int nSeprate = (int)(f*fRuler)-(int)(fMin*fRuler);
							f=0;
							for(int ii=0;ii<=nSeprate;ii++)
							{
								f+=cnp[ii];
							}
							f/=ltp;
							f=1-f;
							if(f<0)
								f=0;
						}
						pInput->fOutputParam .line [i] =f;
						if(nFunc == 44) pInput->fOutputParam .line [i] =1-f;

						//CString strTemp;
					//	strTemp.Format(_T("pInput->fOutputParam .line [%d] = %0.6f  \n"), i, pInput->fOutputParam .line [i]);
						//OutputDebugString(strTemp);

					}
				}
				else if(nFunc == 45)
				{
					float fPercent = pInput->fInputParam[0].line[l]/100;
					float fPrc = 0;
					for(int i=0;i<pInput->nNumData ;i++)
					{
						GetMoveCBPerFoot(pInput->pData+i, cnp, ltp, fMax, fMin, i);
						if(i<l)
						{
							continue;
						}
						{
							f=0;
							for(int ii=0;ii<n ;ii++)
							{
								f+=cnp[ii];
								if(f/ltp>=fPercent)
								{
									fPrc = fMin+(float)ii/fRuler;
									break;
								}
							}
						}
						pInput->fOutputParam .line [i] =fPrc;

					}
				}
				else
				{
					int nIn =pInput->fInputParam[0].line[l];//
					for(int i=0;i<pInput->nNumData ;i++)
					{
						GetMoveCBPerFoot(pInput->pData+i, cnp, ltp, fMax, fMin, i);
						if(i<l)
						{
							continue;
						}

						f=cnp[0];
						float amount=0;
						int nFootMax = 0;
						int nFootMax2 = 0;
						int nFootMax3 = 0;
						float max = 0;
						int nBeginY=fMin*fRuler;
						fTemp =cnp[0];
						for(int ii=0;ii<n;ii++)
						{
							amount+=(cnp[ii]*(ii+nBeginY)/(float)fRuler);
							if(cnp[ii]>max)
							{
								nFootMax = ii;
								max=cnp[ii];
							}

							//second max
							if(ii>1)
							{
								if(cnp[ii]<cnp[ii-1] && cnp[ii-1]>=cnp[ii-2])
								{
									if(cnp[ii-1]>f)
									{
										nFootMax3 = nFootMax2;
										nFootMax2 = ii-1;
										fTemp = f;
										f=cnp[ii-1];
									}
								}
							}
						}
						switch(nIn)
						{
						case 1:
							f=(nFootMax+nBeginY)/(float)fRuler;
							break;
						case 2:
							f=cnp[nFootMax];
							break;
						case 3:
							f=amount/ltp;
							break;
							//					case 4:
							//						f=(nFootMax3+nBeginY)/(float)100;
							//						break;
							//					case 5:
							//						f=cnp[nFootMax3];
							//						break;
						default:
							ASSERT(FALSE);
							k=0;
							break;
						}
						pInput->fOutputParam .line [i] =f;

					}
				}
			}
			else
			{
				for(int i=0;i<pInput->nNumData ;i++)
					pInput->fOutputParam .line [i] = 0;
			}
			break;
		}
	}
		
	if(k==0)
		pInput->fOutputParam.b=-1;
	else
		pInput->fOutputParam.b=l;
	pInput->fOutputParam.e=pInput->nNumData-1;


#if 0
	if (!pInput)
	{
		return -1;
	}

	int k=1;
	int j,l = 0;

	float fMax=0;
	float fMin=90000000;
	float f,fTemp = 0.0;
	float fRuler;

	while(1)
	{
		if(pInput->pData == NULL)
		{
			k=0;
			break;
		}
		j=pInput->fInputParam[0].b;
		l=j;

		GetMaxMin(pInput->pData , pInput->nNumData , fMax, fMin);
		int n =GetRuler( fMax,  fMin, fRuler);

		if(n<=0 )
		{
			k=0;
			break;
		}
		CReportData* pdt = NULL;
		if( pCMerch!=NULL || pCMerch->m_pFinanceData != NULL)
		{
			float ltp = pCMerch->m_pFinanceData->m_fCircAsset;

			float* cnp = new float[n];
			memset(cnp,0,n*4);

			if (nFunc == 21 ||nFunc == 44)
			{
				for(int i=0;i<pInput->nNumData ;i++)
				{
					GetMoveCBPerFoot(pInput->pData+i, cnp, ltp, fMax, fMin, i);
					if(i<l)
					{
						continue;
					}
					f=pInput->fInputParam[0].line[i];//
					if(f>fMax )
					{
						f=0;
					}
					else if(f<fMin)
					{
						f=1;
					}
					else
					{
						int nSeprate = (int)(f*fRuler)-(int)(fMin*fRuler);
						f=0;
						for(int ii=0;ii<=nSeprate;ii++)
						{
							f+=cnp[ii];
						}
						f/=ltp;
						f=1-f;
						if(f<0)
							f=0;
					}
					pInput->fOutputParam .line [i] =f;
					if(nFunc == 44)
					{
						pInput->fOutputParam .line [i] =1-f;
					}

					CString strTemp;
					strTemp.Format(_T("pInput->fOutputParam .line [%d] = %0.6f  \n"), i, pInput->fOutputParam .line [i]);
					OutputDebugString(strTemp);
				}


			}
			else if(nFunc == 45)
			{
				float fPercent = pInput->fInputParam[0].line[l]/100;
				float fPrc = 0;
				for(int i=0;i<pInput->nNumData ;i++)
				{
					GetMoveCBPerFoot(pInput->pData+i, cnp, ltp, fMax, fMin, i);
					if(i<l)
					{
						continue;
					}
					{
						f=0;
						for(int ii=0;ii<n ;ii++)
						{
							f+=cnp[ii];
							if(f/ltp>=fPercent)
							{
								fPrc = fMin+(float)ii/fRuler;
								break;
							}
						}
					}
					pInput->fOutputParam .line [i] =fPrc;
				}
			}
			else if (nFunc == 22)
			{
				int nIn =pInput->fInputParam[0].line[l];//
				for(int i=0;i<pInput->nNumData ;i++)
				{
					GetMoveCBPerFoot(pInput->pData+i, cnp, ltp, fMax, fMin, i);
					if(i<l)
					{
						continue;
					}

					f=cnp[0];
					float amount=0;
					int nFootMax = 0;
					int nFootMax2 = 0;
					int nFootMax3 = 0;
					float max = 0;
					int nBeginY=fMin*fRuler;
					fTemp =cnp[0];
					for(int ii=0;ii<n;ii++)
					{
						amount+=(cnp[ii]*(ii+nBeginY)/(float)fRuler);
						if(cnp[ii]>max)
						{
							nFootMax = ii;
							max=cnp[ii];
						}

						//second max
						if(ii>1)
						{
							if(cnp[ii]<cnp[ii-1] && cnp[ii-1]>=cnp[ii-2])
							{
								if(cnp[ii-1]>f)
								{
									nFootMax3 = nFootMax2;
									nFootMax2 = ii-1;
									fTemp = f;
									f=cnp[ii-1];
								}
							}
						}
					}
					switch(nIn)
					{
					case 1:
						f=(nFootMax+nBeginY)/(float)fRuler;
						break;
					case 2:
						f=cnp[nFootMax];
						break;
					case 3:
						f=amount/ltp;
						break;
						//					case 4:
						//						f=(nFootMax3+nBeginY)/(float)100;
						//						break;
						//					case 5:
						//						f=cnp[nFootMax3];
						//						break;
					default:
						ASSERT(FALSE);
						k=0;
						break;
					}
					pInput->fOutputParam .line [i] =f;

				}
			}
		}
		else
		{
			for(int i=0;i<pInput->nNumData ;i++)
				pInput->fOutputParam .line [i] = 0;
		}
		break;
	}
	if(k==0)
	pInput->fOutputParam.b=-1;
	else
	pInput->fOutputParam.b=l;
	pInput->fOutputParam.e=pInput->nNumData-1;
#endif


	return 1;
}
