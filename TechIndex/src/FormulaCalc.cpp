// FormulaCalc.cpp: implementation of the CFormulaCalc class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "FormulaCalc.h"
#include "ExpertTrade.h"

using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFormulaCalc::CFormulaCalc()
{

}

CFormulaCalc::~CFormulaCalc()
{

}

//////////////////////////////////////////////////////////////////////////
bool CFormulaCalc::BeNeedUpdateLast(int iParam, int iIndexNow, int iIndexLast)
{
	if ( iParam < 0 )
	{
		return true;
	}

	//
	bool bNeedUpdate = false;

	if ( -1 == iIndexNow )
	{
		// 没有一个满足条件的, 判断最新一个是不是满足
		bNeedUpdate = true;
	}
	else 
	{
		// 有满足条件的点
		if ( iIndexNow >= (iIndexLast - iParam)  && iIndexNow != iIndexLast )
		{
			// 如果上一个满足的点是在最近 iParam 个之内. 而且不是最新一个, 就不用计算了. 这点被跳过			
			bNeedUpdate = false;
		}
		else
		{
			// 不在被跳过的范围之内. 需要判断最新一个的值
			bNeedUpdate = true;
		}
	}

	return bNeedUpdate;
}

bool CFormulaCalc::CalcCycValues(IN KLine* pKLine, IN int iCountKLine, OUT float*& pZJ, OUT float*& pYZ, OUT float*& pCYC0, OUT float*& pCYC12, OUT float*& pCYC25, OUT float*& pCYC50)
{
	if ( NULL == pKLine || iCountKLine <= 1 || NULL == pZJ || NULL == pYZ || NULL == pCYC0 || NULL == pCYC12 || NULL == pCYC25 || NULL == pCYC50 )
	{
		// ////ASSERT(0);
		return false;
	}
	
	/*  
		参数列表
		ZJ:=(O+H+L+C)/4;
		YZ:=IF(BARSCOUNT(C)>60,VOL/SUM(VOL,60),VOL/SUM(VOL,BARSCOUNT(C)));
		CYC12:=DMA(ZJ,YZ/0.125);
		CYC25:=DMA(ZJ,YZ/0.25);
		CYC50:=DMA(ZJ,YZ/0.5);
		CYC0:=DMA(ZJ,YZ);	
	*/

	
	
	// ZJ
	// 后面算 DMA 的时候, 因为每一个值都是根据前一个值得到的. 所以要计算整条 ZJ 的值
	int i;
	for ( i = 0; i < iCountKLine; i++ )
	{
		pZJ[i] = (pKLine[i].m_fPriceOpen  + pKLine[i].m_fPriceHigh + pKLine[i].m_fPriceLow + pKLine[i].m_fPriceClose ) / 4;
	}

	//

	// YZ 这个值也是动态变化的, 不能用最后一根的值
	for ( i = 0; i < iCountKLine; i++ )
	{
		// iBARSCOUNT 是第一个到当天的有效周期数. 所以有N 条K 线, iBARSCOUNT = N - 1;		
		int iBARSCOUNT = i;
		
		// Sum 的参数
		int iParamSum = iBARSCOUNT > 60 ? 60 : iBARSCOUNT;

		float fSum = 0.;
		Sum(pKLine, i + 1, iParamSum, fSum);

		if ( 0. == fSum )
		{
			// ////ASSERT(0);
			continue;
		}
		
		pYZ[i] = pKLine[i].m_fVolume / fSum;
	}

	// CYC0
	if ( !DMA(pZJ, iCountKLine, pYZ, 1.0f, pCYC0) )
	{
		// ////ASSERT(0);
		return false;
	}

	// CYC12	
	if ( !DMA(pZJ, iCountKLine, pYZ, 0.125f, pCYC12) )
	{
		// ////ASSERT(0);
		return false;
	}

	// CYC25
	if ( !DMA(pZJ, iCountKLine, pYZ, 0.25f, pCYC25) )
	{
		// ////ASSERT(0);		
		return false;
	}

	// CYC50
	if ( !DMA(pZJ, iCountKLine, pYZ, 0.5f, pCYC50) )
	{
		// ////ASSERT(0);		
		return false;
	}

	return true;
}

bool CFormulaCalc::UpdateLastCalcCycValue(IN CExpertTrade* pData)
{
	if ( NULL == pData || pData->m_aKLines.GetSize() <= 2 || !pData->m_stCycValues.BeValid() )
	{		
		return false;
	}

	int iCountKlineNow = pData->m_aKLines.GetSize();
	KLine* pKLine = (KLine*)pData->m_aKLines.GetData();

	//
	int iCountCyc = pData->m_stCycValues.m_aYZ.size();

	if ( iCountCyc != iCountKlineNow )
	{
		return false;
	}

	//
	int iIndexLast = iCountKlineNow - 1;

	// ZJ 的最新值
	float fZJ = (pKLine[iIndexLast].m_fPriceOpen + pKLine[iIndexLast].m_fPriceClose + pKLine[iIndexLast].m_fPriceHigh + pKLine[iIndexLast].m_fPriceLow) / 4;

	// YZ 的最新值
	float fYZ = pData->m_stCycValues.m_aYZ[iIndexLast];
	{
		// iBARSCOUNT 是第一个到当天的有效周期数. 所以有N 条K 线, iBARSCOUNT = N - 1;		
		int iBARSCOUNT = iIndexLast;

		// Sum 的参数
		int iParamSum = iBARSCOUNT > 60 ? 60 : iBARSCOUNT;

		float fSum = 0.;
		Sum(pKLine, iCountKlineNow, iParamSum, fSum);

		if ( 0. != fSum )
		{
			fYZ = pKLine[iIndexLast].m_fVolume / fSum;
		}		
	}

	// CYC12:=DMA(ZJ,YZ/0.125);
	float fParam = fYZ / 0.125;
	float fCYC12 = fZJ * fParam + pData->m_stCycValues.m_aCYC12[iIndexLast - 1] * (1.0 - fParam);

	// CYC25:=DMA(ZJ,YZ/0.25);
	fParam = fYZ / 0.25;
	float fCYC25 = fZJ * fParam + pData->m_stCycValues.m_aCYC25[iIndexLast - 1] * (1.0 - fParam);

	// CYC50:=DMA(ZJ,YZ/0.5);
	fParam = fYZ / 0.5;
	float fCYC50 = fZJ * fParam + pData->m_stCycValues.m_aCYC50[iIndexLast - 1] * (1.0 - fParam);

	// CYC0:=DMA(ZJ,YZ);
	fParam = fYZ;
	float fCYC0 = fZJ * fParam + pData->m_stCycValues.m_aCYC0[iIndexLast - 1] * (1.0 - fParam);

	// 更新值
	pData->m_stCycValues.m_aZJ.pop_back();
	pData->m_stCycValues.m_aZJ.push_back(fZJ);

	pData->m_stCycValues.m_aYZ.pop_back();
	pData->m_stCycValues.m_aYZ.push_back(fYZ);

	pData->m_stCycValues.m_aCYC0.pop_back();
	pData->m_stCycValues.m_aCYC0.push_back(fCYC0);

	pData->m_stCycValues.m_aCYC12.pop_back();
	pData->m_stCycValues.m_aCYC12.push_back(fCYC12);

	pData->m_stCycValues.m_aCYC25.pop_back();
	pData->m_stCycValues.m_aCYC25.push_back(fCYC25);

	pData->m_stCycValues.m_aCYC50.pop_back();
	pData->m_stCycValues.m_aCYC50.push_back(fCYC50);

	return true;
}

// 1) 主力拉升公式：
bool CFormulaCalc::CalcJgtlIndex(IN KLine* pKLine, IN int iCountKLine, IN float fCapital, IN float* pCYC0, IN float* pCYC12, IN float* pCYC25, IN float* pCYC50, OUT vector<UINT>& aResult)
{
	if ( NULL == pKLine || iCountKLine <= 1 || fCapital <= 0 )
	{				
		return false;
	}

	if ( NULL == pCYC0 || NULL == pCYC12 || NULL == pCYC25 || NULL == pCYC50 )
	{		
		return false;
	}

	// int iIndexLast = iCountKLine - 1;

	//	  公式
	// 	FILTER(CLOSE>=HHV(CLOSE,60) 
	// 	AND CLOSE/REF(CLOSE,1)>1.03
	// 	AND MA(VOL/CAPITAL,20)<=0.04
	// 	AND LAST(CYC50>CYC0 AND CYC12>CYC0,20,0)
	// 	AND CLOSE/CYC0<=1.6,5) ;

	// 保存计算结果
	aResult.clear();
	
	// 计算condition2 时需要 MA 序列	
	float aMaForCondition3[20];
	memset(aMaForCondition3, 0, 20 * sizeof(float));

	// 过滤参数
	const int KiFilterParam = 5;	
	int  iFilter = 0;

	for ( int i = 1; i < iCountKLine; i++ )
	{
		if ( 56 == i )
		{
			i = i;
		}

		if ( iFilter > 0 )
		{			
			iFilter -= 1;
			continue;
		}

		bool bCondition1 = false;
		bool bCondition2 = false;
		bool bCondition3 = false;
		bool bCondition4 = false;
		bool bCondition5 = false;

		// 当前收盘价和前一点收盘价
		float fClose = pKLine[i].m_fPriceClose;
		float fClosePre = pKLine[i-1].m_fPriceClose;

		// condition1: CLOSE>=HHV(CLOSE,60) 
		{
			bCondition1 = true;

			
			
			int iHHV = 60;
			if ( i < (iHHV - 1) )
			{
				iHHV = i + 1;
			}
			
			// HHV(CLOSE,60);
			for ( int j = i; j > (i - iHHV); j-- )
			{				
				if ( fClose < pKLine[j].m_fPriceClose )
				{
					bCondition1 = false;
					break;
				}
			}			
		}
		
		if ( !bCondition1 )
		{
			continue;
		}

		// condition2: CLOSE/REF(CLOSE,1)>1.03
		{
			if ( 0 != fClosePre )
			{	
				bCondition2 = ((fClose / fClosePre) > 1.03);		
			}
		}
		
		if ( !bCondition2 )
		{
			continue;
		}

		// condition3: MA(VOL/CAPITAL,20)<=0.04
		{
			int iParamMacondition3 = 20;
			memset(aMaForCondition3, 0, 20 * sizeof(float));
			
			if ( i < (iParamMacondition3 - 1) )
			{
				iParamMacondition3 = i + 1;
			}
			
			for ( int j = i, k = (iParamMacondition3 - 1); j > (i - iParamMacondition3); j--, k-- )
			{
				aMaForCondition3[k] = pKLine[j].m_fVolume / fCapital;
			}
			
			float fMaCondition3;
			if( !MA(aMaForCondition3, iParamMacondition3, iParamMacondition3, fMaCondition3) )
			{
				// ////ASSERT(0);
				continue;
			}
			
			bCondition3 = (fMaCondition3 <= 0.04);
		}
		
		if ( !bCondition3 )
		{
			continue;
		}

		// condition4: LAST(CYC50>CYC0 AND CYC12>CYC0,20,0)
		// LAST: 是要向前 20 根. 包含自己当前这根, 总共要 21 根
		{
			int iParamMacondition4 = 20;
			
			if ( i < iParamMacondition4 )
			{
				iParamMacondition4 = i;
			}
			
			bCondition4 = true;

			for ( int j = i; j >= (i - iParamMacondition4); j-- )
			{
				if ( (pCYC50[j] > pCYC0[j]) && (pCYC12[j] > pCYC0[j]) )
				{
					continue;
				}
				else
				{
					bCondition4 = false;
					break;
				}
			}
		}
		
		if ( !bCondition4 )
		{
			continue;
		}

		// condition5: CLOSE/CYC0<=1.6
		{
			if ( 0 != pCYC0[i] )
			{
				bCondition5 = ((fClose / pCYC0[i]) <= 1.6);
			}
		}
						
		if ( !bCondition5 )
		{
			continue;
		}

		//
		if ( bCondition1 && bCondition2 && bCondition3 && bCondition4 && bCondition5 )
		{
			// 保险起见, 加上 if 条件判断才能进来						
			aResult.push_back(pKLine[i].m_TimeCurrent.GetTime());
			iFilter = KiFilterParam;
		}
	}

	return true;
}

bool CFormulaCalc::UpdateLastJgtlIndex(CExpertTrade* pData, OUT bool& bSatisfy)
{
	if ( NULL == pData )
	{
		return false;
	}

	int iCountKLine = pData->m_aKLines.GetSize();
	float fCapital = pData->m_fCircStock;

	if ( iCountKLine <= 1 || fCapital <= 0 )
	{
		return false;
	}

	KLine* pKLine = (KLine*)pData->m_aKLines.GetData();

	if ( !BeValidFloat(fCapital) )
	{		
		return false;
	}

	if ( pData->m_stCycValues.m_aZJ.size() != iCountKLine )
	{
		return false;
	}

	int iIndexLast = iCountKLine - 1;
	
	//
	bool bCondition1 = false;
	bool bCondition2 = false;
	bool bCondition3 = false;
	bool bCondition4 = false;
	bool bCondition5 = false;

	float* pCYC0  = &(*pData->m_stCycValues.m_aCYC0.begin());

	float* pCYC12 = &(*pData->m_stCycValues.m_aCYC12.begin());
	
	float* pCYC50 = &(*pData->m_stCycValues.m_aCYC50.begin());

	//
	float aMaForCondition3[20];
	memset(aMaForCondition3, 0, sizeof(aMaForCondition3));

	// 当前收盘价和前一点收盘价
	float fClose = pKLine[iIndexLast].m_fPriceClose;
	float fClosePre = pKLine[iIndexLast-1].m_fPriceClose;

	// condition1: CLOSE>=HHV(CLOSE,60) 
	{
		bCondition1 = true;

		

		int iHHV = 60;
		if ( iIndexLast < (iHHV - 1) )
		{
			iHHV = iIndexLast + 1;
		}

		// HHV(CLOSE,60);
		for ( int j = iIndexLast; j > (iIndexLast - iHHV); j-- )
		{				
			if ( fClose < pKLine[j].m_fPriceClose )
			{
				bCondition1 = false;
				break;
			}
		}
	}

	if ( !bCondition1 )
	{
		bSatisfy = false;
		return true;
	}

	// condition2: CLOSE/REF(CLOSE,1)>1.03
	{
		if ( 0 != fClosePre )
		{	
			bCondition2 = ((fClose / fClosePre) > 1.03);		
		}
	}

	if ( !bCondition2 )
	{
		bSatisfy = false;
		return true;
	}

	// condition3: MA(VOL/CAPITAL,20)<=0.04
	{
		int iParamMacondition3 = 20;
		memset(aMaForCondition3, 0, 20 * sizeof(float));

		if ( iIndexLast < (iParamMacondition3 - 1) )
		{
			iParamMacondition3 = iIndexLast + 1;
		}

		for ( int j = iIndexLast, k = (iParamMacondition3 - 1); j > (iIndexLast - iParamMacondition3); j--, k-- )
		{
			aMaForCondition3[k] = pKLine[j].m_fVolume / fCapital;
		}

		float fMaCondition3;
		if( MA(aMaForCondition3, iParamMacondition3, iParamMacondition3, fMaCondition3) )
		{			
			bCondition3 = (fMaCondition3 <= 0.04);	
		}	
	}

	if ( !bCondition3 )
	{
		bSatisfy = false;
		return true;
	}

	// condition4: LAST(CYC50>CYC0 AND CYC12>CYC0,20,0)
	// LAST: 是要向前 20 根. 包含自己当前这根, 总共要 21 根
	{
		int iParamMacondition4 = 20;

		if ( iIndexLast < iParamMacondition4 )
		{
			iParamMacondition4 = iIndexLast;
		}

		bCondition4 = true;

		for ( int j = iIndexLast; j >= (iIndexLast - iParamMacondition4); j-- )
		{
			if (j >= 0 && j < pData->m_stCycValues.m_aCYC50.size() && j < pData->m_stCycValues.m_aCYC0.size() && j < pData->m_stCycValues.m_aCYC12.size())
			{
				if ( (pCYC50[j] > pCYC0[j]) && (pCYC12[j] > pCYC0[j]) )
				{
					continue;
				}
				else
				{
					bCondition4 = false;
					break;
				}
			}
			
		}
	}

	if ( !bCondition4 )
	{
		bSatisfy = false;
		return true;
	}

	// condition5: CLOSE/CYC0<=1.6
	{
		if (iIndexLast >= 0 && iIndexLast < pData->m_stCycValues.m_aCYC0.size())
		{
			if (0 != pCYC0[iIndexLast] )
			{
				bCondition5 = ((fClose / pCYC0[iIndexLast]) <= 1.6);
			}
		}
		
	}

	if ( !bCondition5 )
	{
		bSatisfy = false;
		return true;
	}

	bSatisfy = true;
	return true;
}

// 2) 主力洗盘公式：
bool CFormulaCalc::CalcXpjsIndex(IN KLine* pKLine, IN int iCountKLine, IN float fCapital, IN float* pCYC0, IN float* pCYC12, IN float* pCYC25, IN float* pCYC50, OUT vector<UINT>& aResult)
{
	if ( NULL == pKLine || iCountKLine <= 1 || fCapital <= 0 )
	{
		// ////ASSERT(0);
		return false;
	}

	if ( NULL == pCYC0 || NULL == pCYC12 || NULL == pCYC25 || NULL == pCYC50 )
	{
		// ////ASSERT(0);
		return false;
	}

	int iIndexLast = iCountKLine - 1;

	// 保存计算结果
	aResult.clear();

	//
	const int KiFilterParam = 10;	
	int  iFilter = 0;

	for ( int i = 1; i < iCountKLine; i++ )
	{
		if ( iFilter > 0 )
		{			
			iFilter -= 1;
			continue;
		}

		bool bCondition1 = false;
		bool bCondition2 = false;
		bool bCondition3 = false;
		bool bCondition4 = false;
		bool bCondition5 = false;
		bool bCondition6 = false;
		bool bCondition7 = false;

		// 当前收盘价和前一点收盘价
		float fClose = pKLine[i].m_fPriceClose;
		

		// condition1: COUNT(VOL/CAPITAL<0.015,10)>=1
		{
			int iParamCon1 = 10;
			int iCounts = 0;

			if ( i < (iParamCon1 - 1) )
			{
				iParamCon1 = i + 1;
			}

			// 
			for ( int j = i; j > (i - iParamCon1); j-- )
			{
				if ( 0 != fCapital )
				{
					if ( (pKLine[j].m_fVolume / fCapital) < 0.015 )
					{
						iCounts += 1;
						if ( iCounts >= 1 )
						{
							break;
						}
					}
				}
			}

			//
			bCondition1 = (iCounts >= 1);
		}

		if ( !bCondition1 )
		{
			continue;
		}

		// condition2: (MA(VOL/CAPITAL,10)<0.025 OR COUNT(VOL<MA(VOL,10),10)>=7)
		{
			// 
			float aMaVolCapital[10];
			memset(aMaVolCapital, 0, 10 * sizeof(float));

			// 前半部分条件
			int iParamCon2 = 10;

			if ( i < (iParamCon2 - 1) )
			{
				iParamCon2 = i + 1;
			}

			int j,k;
			for ( j = i, k = (iParamCon2 - 1); j > (i - iParamCon2); j--, k-- )
			{				
				if (fCapital != 0.0f)
				{
					aMaVolCapital[k] = pKLine[j].m_fVolume / fCapital;
				}				
			}

			float fMaVolCaptial;
			MA(aMaVolCapital, iParamCon2, iParamCon2, fMaVolCaptial);

			// 后半部分条件				
			float aMaVol[10];
			memset(aMaVol, 0, 10 * sizeof(float));		

			for ( j = i, k = (iParamCon2 - 1); j > (i - iParamCon2); j--, k-- )
			{
				aMaVol[k] = pKLine[j].m_fVolume;
			}

			vector<float> aMaVolResult(10, 0.);
			float* paMaVolResult = &(*aMaVolResult.begin());
			MA_ARRAY(aMaVol, iParamCon2, iParamCon2, paMaVolResult);

			int iCount = 0;
			k = iParamCon2 - 1;
			for ( j = i; j > (i - iParamCon2); j--, k-- )
			{
				if ( k >= 0 )
				{					
					if ( pKLine[j].m_fVolume < aMaVolResult[k] )
					{
						iCount += 1;
					}
				}				
			}

			if ( fMaVolCaptial < 0.025 || iCount >= 7 )
			{
				bCondition2 = true;
			}
		}

		if ( !bCondition2 )
		{
			continue;
		}

		// condition3: CLOSE>=HHV(CLOSE,10)
		{
			int iParamCon3 = 10;

			if ( i < (iParamCon3 - 1) )
			{
				iParamCon3 = i + 1;
			}

			float fCloseMax = 0.;

			for ( int j = i; j > (i - iParamCon3); j-- )
			{
				fCloseMax = pKLine[j].m_fPriceClose > fCloseMax ? pKLine[j].m_fPriceClose : fCloseMax;
			}

			if ( fClose >= fCloseMax )
			{
				bCondition3 = true;
			}
		}

		if ( !bCondition3 )
		{
			continue;
		}

		// condition4: COUNT(CLOSE<CYC0,20)<=4
		{			
			int iParamCon4 = 20;

			if ( i < (iParamCon4 - 1) )
			{
				iParamCon4 = i + 1;
			}

			int iCount = 0;
			for ( int j = i; j > (i - iParamCon4); j-- )
			{
				if ( pKLine[j].m_fPriceClose < pCYC0[j] )
				{
					iCount += 1;
				}					
			}

			if ( iCount <= 4 )
			{
				bCondition4 = true;
			}
		}

		if ( !bCondition4 )
		{
			continue;
		}

		// condition5: CLOSE/CYC12<1.1
		{
			if ( pCYC12[iIndexLast] != 0. )
			{
				if ( (fClose / pCYC12[iIndexLast]) < 1.1 )
				{
					bCondition5 = true;
				}
			}
		}

		if ( !bCondition5 )
		{
			continue;
		}

		// condition6: CLOSE/CYC0<1.3
		{
			if ( pCYC0[iIndexLast] != 0. )
			{
				if ( (fClose / pCYC0[iIndexLast]) < 1.3 )
				{
					bCondition6 = true;
				}
			}
		}

		if ( !bCondition6 )
		{
			continue;
		}

		// condition7: LAST(CYC50>CYC0,30,0)
		// LAST: 是要向前 30 根. 包含自己当前这根, 总共要 31 根
		{
			bCondition7 = true;
			int iParamCon7 = 30;

			if ( i < iParamCon7 )
			{
				iParamCon7 = i;
			}

			for ( int j = i; j >= (i - iParamCon7); j-- )
			{
				if ( pCYC50[j] > pCYC0[j] )
				{
					continue;
				}
				else
				{
					bCondition7 = false;
					break;
				}								
			}			
		}

		if ( !bCondition7 )
		{
			continue;
		}

		//
		if ( bCondition1 && bCondition2 && bCondition3 && bCondition4 && bCondition5 && bCondition6 && bCondition7 )
		{
			// 保险起见, 加上 if 条件判断才能进来
			aResult.push_back(pKLine[i].m_TimeCurrent.GetTime());
			iFilter = KiFilterParam;
		}
	}

	return true;
}

bool CFormulaCalc::UpdateLastXpjsIndex(CExpertTrade* pData, OUT bool& bSatisfy)
{
	if ( NULL == pData )
	{
		return false;
	}

	int iCountKLine = pData->m_aKLines.GetSize();
	float fCapital = pData->m_fCircStock;

	if ( iCountKLine <= 1 || fCapital <= 0 )
	{
		return false;
	}

	KLine* pKLine = (KLine*)pData->m_aKLines.GetData();

	if ( !pData->m_stCycValues.BeValid() )
	{		
		return false;
	}

	if ( pData->m_stCycValues.m_aZJ.size() != iCountKLine )
	{
		return false;
	}

	int iIndexLast = iCountKLine - 1;

	//
	float* pCYC0  = &(*pData->m_stCycValues.m_aCYC0.begin());
	float* pCYC12 = &(*pData->m_stCycValues.m_aCYC12.begin());
	
	float* pCYC50 = &(*pData->m_stCycValues.m_aCYC50.begin());

	//
	bool bCondition1 = false;
	bool bCondition2 = false;
	bool bCondition3 = false;
	bool bCondition4 = false;
	bool bCondition5 = false;
	bool bCondition6 = false;
	bool bCondition7 = false;

	// 当前收盘价和前一点收盘价
	float fClose = pKLine[iIndexLast].m_fPriceClose;
	

	// condition1: COUNT(VOL/CAPITAL<0.015,10)>=1
	{
		int iParamCon1 = 10;
		int iCounts = 0;

		if ( iIndexLast < (iParamCon1 - 1) )
		{
			iParamCon1 = iIndexLast + 1;
		}

		// 
		for ( int j = iIndexLast; j > (iIndexLast - iParamCon1); j-- )
		{
			if ( 0 != fCapital )
			{
				if ( (pKLine[j].m_fVolume / fCapital) < 0.015 )
				{
					iCounts += 1;
					if ( iCounts >= 1)
					{
						break;
					}
				}
			}
		}

		//
		bCondition1 = (iCounts >= 1);
	}

	if ( !bCondition1 )
	{
		bSatisfy = false;
		return true;
	}

	// condition2: (MA(VOL/CAPITAL,10)<0.025 OR COUNT(VOL<MA(VOL,10),10)>=7)
	{
		// 
		float aMaVolCapital[10];
		memset(aMaVolCapital, 0, 10 * sizeof(float));

		// 前半部分条件
		int iParamCon2 = 10;

		if ( iIndexLast < (iParamCon2 - 1) )
		{
			iParamCon2 = iIndexLast + 1;
		}

		int j,k;
		for ( j = iIndexLast, k = (iParamCon2 - 1); j > (iIndexLast - iParamCon2); j--, k-- )
		{				
			if (fCapital != 0.0f)
			{
				aMaVolCapital[k] = pKLine[j].m_fVolume / fCapital;
			}
			
		}

		float fMaVolCaptial;
		MA(aMaVolCapital, iParamCon2, iParamCon2, fMaVolCaptial);

		// 后半部分条件				
		float aMaVol[10];
		memset(aMaVol, 0, 10 * sizeof(float));		

		for ( j = iIndexLast, k = (iParamCon2 - 1); j > (iIndexLast - iParamCon2); j--, k-- )
		{
			aMaVol[k] = pKLine[j].m_fVolume;
		}

		vector<float> aMaVolResult(10, 0.);
		float* paMaVolResult = &(*aMaVolResult.begin());
		MA_ARRAY(aMaVol, iParamCon2, iParamCon2, paMaVolResult);

		int iCount = 0;
		k = iParamCon2 - 1;
		for ( j = iIndexLast; j > (iIndexLast - iParamCon2); j--, k-- )
		{
			if ( k >= 0 )
			{					
				if ( pKLine[j].m_fVolume < aMaVolResult[k] )
				{
					iCount += 1;
				}
			}				
		}

		if ( fMaVolCaptial < 0.025 || iCount >= 7 )
		{
			bCondition2 = true;
		}
	}

	if ( !bCondition2 )
	{
		bSatisfy = false;
		return true;
	}

	// condition3: CLOSE>=HHV(CLOSE,10)
	{
		int iParamCon3 = 10;

		if ( iIndexLast < (iParamCon3 - 1) )
		{
			iParamCon3 = iIndexLast + 1;
		}

		float fCloseMax = 0.;

		for ( int j = iIndexLast; j > (iIndexLast - iParamCon3); j-- )
		{
			fCloseMax = pKLine[j].m_fPriceClose > fCloseMax ? pKLine[j].m_fPriceClose : fCloseMax;
		}

		if ( fClose >= fCloseMax )
		{
			bCondition3 = true;
		}
	}

	if ( !bCondition3 )
	{
		bSatisfy = false;
		return true;
	}

	// condition4: COUNT(CLOSE<CYC0,20)<=4
	{			
		int iParamCon4 = 20;

		if ( iIndexLast < (iParamCon4 - 1) )
		{
			iParamCon4 = iIndexLast + 1;
		}

		int iCount = 0;
		for ( int j = iIndexLast; j > (iIndexLast - iParamCon4); j-- )
		{
			if (j >= 0 && j < pData->m_stCycValues.m_aCYC0.size())
			{
				if ( pKLine[j].m_fPriceClose < pCYC0[j] )
				{
					iCount += 1;
				}		
			}
						
		}

		if ( iCount <= 4 )
		{
			bCondition4 = true;
		}
	}

	if ( !bCondition4 )
	{
		bSatisfy = false;
		return true;
	}

	// condition5: CLOSE/CYC12<1.1
	{
		if (iIndexLast >= 0 && iIndexLast < pData->m_stCycValues.m_aCYC12.size())
		{
			if ( pCYC12[iIndexLast] != 0. )
			{
				if ( (fClose / pCYC12[iIndexLast]) < 1.1 )
				{
					bCondition5 = true;
				}
			}
		}
		
	}

	if ( !bCondition5 )
	{
		bSatisfy = false;
		return true;
	}

	// condition6: CLOSE/CYC0<1.3
	if (iIndexLast >= 0 && iIndexLast < pData->m_stCycValues.m_aCYC0.size())
	{
		if ( pCYC0[iIndexLast] != 0. )
		{
			if ( (fClose / pCYC0[iIndexLast]) < 1.3 )
			{
				bCondition6 = true;
			}
		}
	}	
	

	if ( !bCondition6 )
	{
		bSatisfy = false;
		return true;
	}

	// condition7: LAST(CYC50>CYC0,30,0)
	// LAST: 是要向前 30 根. 包含自己当前这根, 总共要 31 根
	{
		bCondition7 = true;
		int iParamCon7 = 30;

		if ( iIndexLast < iParamCon7 )
		{
			iParamCon7 = iIndexLast;
		}

		for ( int j = iIndexLast; j >= (iIndexLast - iParamCon7); j-- )
		{
			if (j >= 0 && j < pData->m_stCycValues.m_aCYC50.size() && j < pData->m_stCycValues.m_aCYC0.size())
			{		
				if ( pCYC50[j] > pCYC0[j] )
				{
					continue;
				}
				else
				{
					bCondition7 = false;
					break;
				}				
			}
							
		}			
	}

	if ( !bCondition7 )
	{
		bSatisfy = false;
		return true;
	}

	bSatisfy = true;
	return true;
}

// 3) 游击涨停公式：
bool CFormulaCalc::CalcZjztIndex(IN KLine* pKLine, IN int iCountKLine, IN float fCapital, OUT vector<UINT>& aResult)
{
	if ( NULL == pKLine || iCountKLine <= 1 || fCapital <= 0  )
	{		
		return false;
	}

	/*
		CLOSE/REF(CLOSE,1)>=1.099
		AND AMOUNT>40000000 
		AND  VOL/CAPITAL>=0.04
		AND 3*VOL/SUM(V,3)>1.5 
		AND CLOSE/REF(CLOSE,20)<1.7
		AND CAPITAL<5000000;
	*/

	aResult.clear();

	//
	for ( int32 i = 0; i < iCountKLine; i++ )
	{
		if ( 0 == i )
		{
			continue;
		}

		if ( i == iCountKLine - 1 )
		{
			i = i;
		}
		//
		int iIndexNow = i;
		
		float fClose	= pKLine[iIndexNow].m_fPriceClose;
		float fClosePre	= pKLine[iIndexNow - 1].m_fPriceClose;
		
		//
		if ( 0 == fClosePre )
		{
			continue;
		}
		
		if ( 0 == fCapital )
		{
			continue;
		}
		
		// CLOSE/REF(CLOSE,1)>=1.099
		if ( !(fClose / fClosePre >= 1.099) )
		{
			continue;
		}
		
		// AMOUNT>40000000 
		if ( !(pKLine[iIndexNow].m_fAmount > 40000000) )
		{
			continue;
		}
		
		// 3*VOL/SUM(V,3)>1.5 
		float fSum;
		if ( !Sum(pKLine, iCountKLine, 3, fSum) )
		{
			continue;
		}
		
		if ( ((3.0 * pKLine[iIndexNow].m_fVolume) / fSum) > (float)1.5 )
		{
			;
		}
		else
		{
			continue;
		}
		
		if ( 0. != fSum )
		{
			if ( !((3 * pKLine[iIndexNow].m_fVolume / fSum) > 1.5) )
			{
				continue;
			}
		}
		
		// CLOSE/REF(CLOSE,20)<1.7
		float fClose20Pre = pKLine[0].m_fPriceClose;
		if ( iCountKLine > 20 )
		{
			fClose20Pre = pKLine[iIndexNow - 20].m_fPriceClose;
			if ( 0. == fClose20Pre )
			{
				continue;
			}
		}
		
		if ( !(fClose / fClose20Pre < 1.7) )
		{
			continue;
		}
		
		// CAPITAL<5000000;
		if ( !(fCapital < (float)500000000.0) ) 
		{
			continue;
		}

		// VOL/CAPITAL>=0.04
		if ( !(pKLine[iIndexNow].m_fVolume / fCapital >= 0.04) )
		{
			continue;
		}

		//
		aResult.push_back(pKLine[i].m_TimeCurrent.GetTime());
	}
	
	return true;
}

// 4) 抗跌强庄公式：
bool CFormulaCalc::CalcNsqlIndex(IN KLine* pKLine, IN int iCountKLine, IN float fCloseDapan, IN float fCloseDapanPre, IN float fCapital, IN float* pCYC0, IN float* pCYC12, IN float* pCYC25, IN float* pCYC50, OUT vector<UINT>& aResult)
{
	if ( NULL == pKLine || iCountKLine < 1 )
	{
		// ////ASSERT(0);
		return false;
	}

	if ( NULL == pCYC0 || NULL == pCYC12 || NULL == pCYC25 || NULL == pCYC50 )
	{
		// ////ASSERT(0);
		return false;
	}

	//
	aResult.clear();
	aResult.resize(iCountKLine, false);

	/*
		C>=CYC12
		AND CYC12>CYC25 AND CYC25>CYC50 
		AND INDEXC/REF(INDEXC,1)<0.98
		AND C/REF(C,1)>1.03
		AND VOL/CAPITAL>0.05;
	*/
	
	for ( int32 i = 0; i < iCountKLine; i++ )
	{
		int iIndexNow = i;

		// C>=CYC12
		if ( !(pKLine[iIndexNow].m_fPriceClose >= pCYC12[iIndexNow]) )
		{
			continue;
		}
		
		// CYC12>CYC25 AND CYC25>CYC50
		if ( !((pCYC12[iIndexNow] > pCYC25[iIndexNow]) && pCYC25[iIndexNow] > pCYC50[iIndexNow]) )
		{
			continue;
		}
		
		// INDEXC/REF(INDEXC,1)<0.98
		if ( 0 == fCloseDapanPre )
		{			
			continue;
		}
		
		// ...fangz 1210 这个地方待确认. 是不是要用整个大盘的K 线数据, 取当天的来算
		if ( !((fCloseDapan/fCloseDapanPre) < 0.98) )
		{
			continue;
		}
		
		// C/REF(C,1)>1.03
		float fClose	= pKLine[iIndexNow].m_fPriceClose;
		float fClosePre = pKLine[iIndexNow - 1].m_fPriceClose;
		if ( 0. == fClosePre )
		{			
			continue;
		}
		
		if ( !(fClose / fClosePre > 1.03) )
		{
			continue;
		}
		
		// VOL/CAPITAL>0.05;
		if ( 0. == fCapital )
		{
			continue;
		}
		
		if ( !(pKLine[iIndexNow].m_fVolume / fCapital > 0.05) )
		{
			continue;
		}

		//
		aResult.push_back(pKLine[i].m_TimeCurrent.GetTime());
	}

	//
	return true;
}

// 5) 超跌反弹公式：
bool CFormulaCalc::CalcMaVar4ForCdft(IN KLine* pKLine, IN int iCountKLine, OUT float& fMaVar4)
{
	fMaVar4 = 0.;

	//
	if ( NULL == pKLine || iCountKLine <= 4 )
	{
		// ////ASSERT(0);
		return false;
	}

	int iIndexLast = iCountKLine - 1;

	// VAR4:=MA((LLV(LOW,45)-CLOSE)/(HHV(HIGH,45)-LLV(LOW,45))*100,3);
	vector<float> aMaVar4(3, 0.);
	int iParamMa = 3;

	int iParam3 = 45;
	if ( iParam3 > iCountKLine )
	{
		iParam3 = iCountKLine;
	}

	int iIndexBegin  = iIndexLast;
	int iIndexEnd	 = iIndexBegin - iParam3 + 1;

	//
	int32 iIndexTmp = iIndexLast;

	while ( iParamMa > 0 )
	{
		if ( iIndexEnd < 0 )
		{
			iIndexEnd = 0;
		}

		// 
		float fLow  = pKLine[iIndexBegin].m_fPriceLow;
		float fHigh = pKLine[iIndexBegin].m_fPriceHigh;

		//
		for ( int32 i = iIndexBegin; i >= iIndexEnd; i-- )
		{
			if ( 0. == pKLine[i].m_fPriceLow || 0. == pKLine[i].m_fPriceHigh )
			{
				continue;
			}

			//
			fLow  = pKLine[i].m_fPriceLow < fLow ? pKLine[i].m_fPriceLow: fLow;
			fHigh = pKLine[i].m_fPriceHigh > fHigh ? pKLine[i].m_fPriceHigh : fHigh;
		}

		if ( 0. == fLow )
		{
			// ////ASSERT(0);
			return false;
		}

		float fValue = (float)((fLow - pKLine[iIndexTmp].m_fPriceClose) / (fHigh - fLow)) * 100.0f;
		aMaVar4[3 - iParamMa] = fValue;

		//
		iIndexBegin -= 1;
		iIndexEnd	-= 1;
		iIndexTmp	-= 1;
		iParamMa	-= 1;
	}

	//
	if ( !MA(&(*aMaVar4.begin()), aMaVar4.size(), aMaVar4.size(), fMaVar4) )
	{
		// ////ASSERT(0);
		return false;
	}

	return true;
}

// 5) 超跌反弹公式：
bool CFormulaCalc::CalcCdftIndex(IN KLine* pKLine, IN int iCountKLine, OUT bool& bSatisfy)
{
	bSatisfy = false;

	if ( NULL == pKLine || iCountKLine <= 4 )
	{
		//	// ////ASSERT(0);
		return false;
	}

	int iIndexLast = iCountKLine - 1;

	//	VAR3:= { [MA(AMOUNT,5)/MA(VOL,5)] / 100 } - { [MA(AMOUNT,125)/MA(VOL,125)] / 100 } )/ { [(MA(AMOUNT,5)/MA(VOL,5)] / 100 } ;
	int iParam1 = 5;

	if ( iParam1 > iCountKLine )
	{
		iParam1 = iCountKLine;
	}

	int iParam2 = 125;
	if ( iParam2 > iCountKLine )
	{
		iParam2 = iCountKLine;
	}

	// 准备好计算 Ma 的数据
	vector<float> aAmount5(iParam1, 0.);
	vector<float> aVol5(iParam1, 0.);

	//
	vector<float> aAmount125(iParam2, 0.);
	vector<float> aVol125(iParam2, 0.);

	int i,j;
	for ( i = iIndexLast, j = (iParam1 - 1); i >= (iCountKLine - iParam1); i-- , j-- )
	{
		aAmount5[j] = pKLine[i].m_fAmount;
		aVol5[j] = pKLine[i].m_fVolume;
	}

	//
	for ( i = iIndexLast, j = (iParam2 -1); i >= (iCountKLine - iParam2); i-- , j-- )
	{
		aAmount125[j] = pKLine[i].m_fAmount;
		aVol125[j] = pKLine[i].m_fVolume;
	}

	// 计算MA 值
	float fMaAmout5, fMaVol5, fMaAmout125, fMaVol125;

	if ( !MA(&(*aAmount5.begin()), iParam1, iParam1, fMaAmout5) 
	  || !MA(&(*aVol5.begin()), iParam1, iParam1, fMaVol5) 
	  || !MA(&(*aAmount125.begin()), iParam2, iParam2, fMaAmout125) 
	  || !MA(&(*aVol125.begin()), iParam2, iParam2, fMaVol125) 
	   )
	{
		// // ////ASSERT(0);
		return false;
	}

	// 分母不能为 0
	if ( 0. == fMaVol5 || 0. == fMaVol125 )
	{
		// // ////ASSERT(0);
		return false;
	}

	// 计算 VAR3 的值
	float fBlock1 = (float)(fMaAmout5 / fMaVol5) / 100.0f; 
	if ( 0. == fBlock1 )
	{
		// // ////ASSERT(0);
		return false;
	}

	float fBlock2 = (float)(fMaAmout125 / fMaVol125) / 100.0f; 

	// 
	float fVar3 = (fBlock1 - fBlock2) / fBlock1;

	//
	float fVar4;
	if ( !CalcMaVar4ForCdft(pKLine, iCountKLine, fVar4) )
	{
		return false;
	}

	//
	float fVar4Pre;
	//lint --e{438}
	KLine* pKLinePre = new KLine[iCountKLine - 1];
	memcpy((void*)pKLinePre, (void*)pKLine, (iCountKLine - 1) * sizeof(KLine));

	if ( !CalcMaVar4ForCdft(pKLinePre, (iCountKLine - 1), fVar4Pre) )
	{
		DEL_ARRAY(pKLinePre);
		return false;
	}

	// CROSS(-5,VAR4) AND VAR3<-0.4;
	bool bCross = false;
	CFormulaCalc::Cross(-5.0f, -5.0f, fVar4, fVar4Pre, bCross);

	if ( bCross && fVar3 < -0.4f )
	{
		bSatisfy = true;
	}
	
	//
	DEL_ARRAY(pKLinePre);
	return true;
}

bool CFormulaCalc::CalcCdftIndex(IN KLine* pKLine, IN int iCountKLine, OUT vector<UINT>& aResult)
{
	aResult.clear();

	//
	for( int32 i = 0; i < iCountKLine; i++)
	{
		bool bOK = false;
		if ( CalcCdftIndex(pKLine, i + 1, bOK) && bOK )
		{
			aResult.push_back(pKLine[i].m_TimeCurrent.GetTime());
		}
	}

	//
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CFormulaCalc::MA(IN KLine* pDataSrc, IN int iCountSrc, IN int iParam, OUT float& fResult)
{
	fResult = 0.;

	//
	if ( NULL == pDataSrc || iCountSrc <= 0 || iParam <= 0 )
	{
		// ////ASSERT(0);
		return false;
	}

	if ( iCountSrc < iParam )
	{
		iParam = iCountSrc;
	}

	//
	int iTemp  = iParam;	
	float fSum = 0.;
	
	while( iTemp > 0 )
	{
		
		fSum  += pDataSrc[iCountSrc - iTemp].m_fPriceClose;
		iTemp -= 1;
	}

	fResult = fSum / iParam;

	return true;
}

bool CFormulaCalc::MA(IN float* pDataSrc, IN int iCountSrc, IN int iParam, OUT float& fResult)
{
	fResult = 0.;
	
	//
	if ( NULL == pDataSrc || iCountSrc <= 0 || iParam <= 0  )
	{
		return false;
	}
	
	if (iCountSrc < iParam)
	{
		iParam = iCountSrc;
	}

	//
	int iTemp  = iParam;	
	float fSum = 0.;
	
	while( iTemp > 0 )
	{	
		
		fSum  += pDataSrc[iCountSrc - iTemp];
		iTemp -= 1;
	}
	
	fResult = fSum / iParam;
	
	return true;
}

bool CFormulaCalc::MA_ARRAY(IN float* pDataSrc, IN int iCountSrc, IN int iParam, OUT float*& pfResult)
{
	//
	if ( NULL == pDataSrc || iCountSrc <= 0 || iParam <= 0 )
	{	
		return false;
	}

	//
	for ( int i = 0; i < iCountSrc; i++ )
	{
		int iParamNow = iParam;
		int iCountNow = i + 1;

		if ( iParamNow > iCountNow )
		{
			iParamNow = iCountNow;
		}

		//
		int   iIndex = i;
		int	  iLoop  = iParamNow;
		float fSum   = 0.;

		while(iLoop > 0)
		{
			fSum += pDataSrc[iIndex];

			iIndex--;
			iLoop--;
		}

		pfResult[i] = fSum / iParamNow;
	}

	return true;
}

bool CFormulaCalc::DMA(IN float* pDataSrc, IN int iCountSrc, IN float fParam, OUT float*& pResult)
{
	// Y = DMA(X,A); Y = A*X + (1-A)*Y1; Y1 为上一个周期的值
	if ( NULL == pDataSrc || iCountSrc <= 0 || fParam < 0 || NULL == pResult )
	{
		// ////ASSERT(0);
		return false;
	}

	if ( fParam > 1 )
	{
		fParam = 1;
	}
	//
	memset(pResult, 0, iCountSrc * sizeof(float));

	//
	for ( int i = 0; i < iCountSrc; i++ )
	{
		if ( 0 == i )
		{
			pResult[i] = pDataSrc[i];
			continue;
		}

		pResult[i] = pDataSrc[i] * fParam + pResult[i-1] * (1 - fParam);
	}

	//
	return true;
}

bool CFormulaCalc::DMA(IN float* pDataSrc, IN int iCountSrc, IN float* pParam, IN float fParamEx, OUT float*& pResult)
{
	// Y = DMA(X,A); Y = A*X + (1-A)*Y1; Y1 为上一个周期的值
	if ( NULL == pDataSrc || iCountSrc <= 0 || NULL == pParam || NULL == pResult )
	{
		// ////ASSERT(0); 
		return false;
	}

	//
	if ( 0 == fParamEx )
	{
		fParamEx = 1;
	}

	//
	memset(pResult, 0, iCountSrc * sizeof(float));

	//
	for ( int i = 0; i < iCountSrc; i++ )
	{
		if ( 0 == i )
		{
			pResult[i] = pDataSrc[i];
			continue;
		}

		float fParam = pParam[i] / fParamEx;
		if ( fParam > 1 )
		{
			fParam = 1;
		}

		pResult[i] = pDataSrc[i] * fParam + pResult[i-1] * (1 - fParam);
	}

	//
	return true;
}

void CFormulaCalc::Cross(IN float fParam1, IN float fParam1Pre, IN float fParam2, IN float fParam2Pre, OUT bool& bResult)
{
	if ( fParam1Pre <= fParam2Pre && fParam1 > fParam2 )
	{
		bResult = true;
	}
	else
	{
		bResult = false;
	}
}

bool CFormulaCalc::Sum(IN KLine* pDataSrc, IN int iCountSrc, IN int iParam, OUT float& fResult)
{
	fResult = 0.;
	
	//
	if ( NULL == pDataSrc || iCountSrc <= 0 )
	{		
		return false;
	}

	if ( iCountSrc < iParam || iParam <= 0 )
	{
		iParam = iCountSrc;
	}

	//
	int iTemp  = iParam;	
	float fSum = 0.;
	
	while( iTemp > 0 )
	{
		fSum  += pDataSrc[iCountSrc - iTemp].m_fVolume;
		iTemp -= 1;
	}
	
	fResult = fSum;
	
	return true;
}

void CFormulaCalc::If(IN bool bParam1, IN float fParam2, IN float fParam3, OUT float& fResult)
{
	fResult = bParam1 ? fParam2 : fParam3; 
}
