// SelectStockCenter.cpp: implementation of the CSelectStockCenter class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include <process.h>
#include <float.h>
#include "SelectStockCenter.h"
#include "TimeInterval.h"
//#include "CFormularCompute.h"
#include "formulaengine.h"

//#include "viewdata.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CAbsCenterManager *g_pCenterManager = NULL;
// 离线数据对象
//CAbsDataManager *m_pAbsDataManager = NULL;
//CString g_strUserName;
//CGmtTime g_serverTime;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSelectStockCenter::CSelectStockCenter(HWND hOwner)
{
	m_hOwner = hOwner;
	m_bStop  = true;
	m_aMerchsOUT.clear();
	m_hThreadSelect = NULL;
	m_eSelectStockType = ESSTCounts;
}
//lint --e{1540}
CSelectStockCenter::~CSelectStockCenter()
{
	DEL_HANDLE(m_hThreadSelect);
}

//
void CSelectStockCenter::SetConditionChooseParam(T_ConditionSelectParam& stParam)
{
	if ( !stParam.BeValid() )
	{
		////ASSERT(0);
		return;
	}

	//
	m_eSelectStockType = ESSTCondition;
	m_stConditionParam = stParam;
}

void CSelectStockCenter::SetCustomChooseParam(T_CustomSelectParam& stParam)
{
	m_eSelectStockType = ESSTCustom;
	m_stCustomParam = stParam;
}

void CSelectStockCenter::StartWork()
{
	StopWork();

	//
	m_bStop = false;
	m_hThreadSelect = (HANDLE)_beginthreadex(NULL, 0, CallBackThreadSelect, this, NULL, NULL);	
}

void CSelectStockCenter::StopWork()
{
	m_bStop = true;

	if ( NULL!=m_hThreadSelect && WAIT_OBJECT_0 == WaitForSingleObject(m_hThreadSelect, INFINITE) )
	{
		DEL_HANDLE(m_hThreadSelect);
		m_hThreadSelect = NULL;
	}
}

void CSelectStockCenter::GetSelectResult(set<CMerch*, MerchCmp>& aMerchs)
{
	RGUARD(LockSingle, m_LockMerchsOUT, LockMerchsOUT);

	//
	aMerchs = m_aMerchsOUT;
}

unsigned __stdcall CSelectStockCenter::CallBackThreadSelect(void* pVoid)		
{
	CSelectStockCenter* pThis = (CSelectStockCenter*)pVoid;	
	
	if ( ESSTCondition == pThis->m_eSelectStockType ) 
	{
		pThis->ThreadConditionSelect();
	}
	else if ( ESSTCustom == pThis->m_eSelectStockType )
	{
		pThis->ThreadCustomSelect();
	}
	else 
	{
		////ASSERT(0);
		return 0;
	}

	return 1;
}

void CSelectStockCenter::ThreadConditionSelect()
{
	

	RGUARD(LockSingle, m_LockMerchsOUT, LockMerchsOUT);

	//
	E_KLineTypeBase eKLineBase; 
	E_NodeTimeInterval eIntervalCmp;
	int32 iScale;
	if ( !GetTimeIntervalInfo(0, 0, m_stConditionParam.m_eInterval, eIntervalCmp, eKLineBase, iScale) )
	{
		////ASSERT(0);
		return;
	}
		
	// 
	m_aMerchsOUT.clear();

	//
	for ( set<CMerch*, MerchCmp>::iterator itMerch = m_stConditionParam.m_aMerchs.begin(); itMerch != m_stConditionParam.m_aMerchs.end(); ++itMerch )
	{
		if ( m_bStop )
		{
			break;
		}

		//
		CMerch* pMerch = *itMerch;
		if ( NULL == pMerch )
		{
			continue;
		}

		// 发消息给父窗口
		if ( NULL != m_hOwner )
		{
			CString* pStrMsg = new CString();
			*pStrMsg = pMerch->m_MerchInfo.m_StrMerchCnName;

			::PostMessage(m_hOwner, UM_Choose_Stock_Progress, (WPARAM)pStrMsg, 0);
		}

		if(NULL == g_pCenterManager)
		{
			continue;
		}
		// 准备好脱机 K 线	
		CArray<CKLine, CKLine> aKLineSrc;		
		g_pCenterManager->GetDataManager()->ReadOfflineKLines(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, eKLineBase, EOKTOfflineData, 0, -1, aKLineSrc);
		if ( aKLineSrc.GetSize() <= 0 )
		{
			TRACE(L"%d-%s 没有脱机K线数据\n", pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer());
			continue;
		}

		//
		CMarketIOCTimeInfo MarketIOCTimeInfo;
		CGmtTime gtserverTime = CGmtTime::GetCurrentTime();
		gtserverTime = g_pCenterManager->GetServerTime();
		if (!pMerch->m_Market.GetRecentTradingDay(gtserverTime, MarketIOCTimeInfo, pMerch->m_MerchInfo))
		{
			////ASSERT(0);
			return;
		}
		
		//
		CArray<CKLine, CKLine> aKLineFinal;
		aKLineFinal.Copy(aKLineSrc);

		// 如果不是基本周期的, 合并K线
		switch (m_stConditionParam.m_eInterval)
		{
		case ENTIMinute15:
			{
				if (!CMerchKLineNode::CombinMinuteN(pMerch, 15, aKLineSrc, aKLineFinal))					
				{
					////ASSERT(0);
				}	
			}
			break;
		case ENTIMinute30:
			{
				if (!CMerchKLineNode::CombinMinuteN(pMerch, 30, aKLineSrc, aKLineFinal))					
				{
					////ASSERT(0);
				}
			}
			break;					
		case ENTIQuarter:
			{
				if (!CMerchKLineNode::CombinMonthN(aKLineSrc, 3, aKLineFinal))
				{
					////ASSERT(0);
				}
			}
			break;
		case ENTIYear:
			{
				if (!CMerchKLineNode::CombinMonthN(aKLineSrc, 12, aKLineFinal))
				{
					////ASSERT(0);
				}
			}
			break;
		default:
			{
				;
			}			
			break;
		}
		
		// 开始计算指标:

		// 用于临时保存计算结果的数组 (历史)
		vector<float> aResults(aKLineFinal.GetSize(), 0.);
		float* pResults = &(*aResults.begin());

		// 用户计算结果的变量 (非历史)
		bool32 bSatisfy = false;

		//
		for ( set<CFormularContent*, FormularCmp>::iterator itFormular = m_stConditionParam.m_aFormulars.begin(); itFormular != m_stConditionParam.m_aFormulars.end(); ++itFormular )
		{
			//
			CFormularContent* pFormular = *itFormular;
			if ( NULL == pFormular )
			{
				continue;
			}
			
			//lint --e{438}
			T_IndexOutArray* pIndex = formula_index(pFormular, aKLineFinal);
			if ( NULL == pIndex || 1 != pIndex->iIndexNum || pIndex->index[0].iPointNum != aKLineFinal.GetSize() ) 
			{
				// 条件选股的公式只能有一个输出值序列, 这个序列的元素个数应该与K 线根数一样
				////ASSERT(0);
				continue;
			}

			//
			int32 iSize = aKLineFinal.GetSize();

			//
			if ( !m_stConditionParam.m_bHistory )
			{
				// 非历史阶段, 只判断最新一个的条件是否满足:

				// 当前指标是否满足
				bool32 bSatisfyNow = pIndex->index[0].pPoint[iSize-1] == 1.0 ? true : false;

				if ( itFormular == m_stConditionParam.m_aFormulars.begin() )
				{
					// 第一次直接赋值:
					bSatisfy = bSatisfyNow;
				}
				else
				{
					// 以后的根据与条件还是或条件来取值
					if ( m_stConditionParam.m_bAndCondition )
					{
						bSatisfy = (bSatisfy && bSatisfyNow);
					}
					else
					{
						bSatisfy = (bSatisfy || bSatisfyNow);
					}						
				}
			}
			else 
			{
				// 历史阶段选股
				if ( itFormular == m_stConditionParam.m_aFormulars.begin() )
				{
					// 第一次直接赋值:
					for ( int32 i = 0; i < pIndex->index[0].iPointNum; i++ )
					{
						float fNow = pIndex->index[0].pPoint[i];
						pResults[i] = fNow;
					}
				}
				else 
				{
					// 以后的根据与条件还是或条件来取值
					if ( m_stConditionParam.m_bAndCondition )
					{
						// 与
						for ( int32 i = 0; i < pIndex->index[0].iPointNum; i++ )
						{
							float fNow	  = pIndex->index[0].pPoint[i];
							float fBefore = pResults[i];
							
							pResults[i] = fNow * fBefore;	
						}	
					}
					else
					{
						// 或
						for ( int32 i = 0; i < pIndex->index[0].iPointNum; i++ )
						{
							float fNow	  = pIndex->index[0].pPoint[i];
							float fBefore = pResults[i];
							
							pResults[i] = fNow + fBefore;	
						}
					}	
				}
			}

			// 释放
			DEL(pIndex);
		}
		
		// 最后看这个商品是否满足条件
		if ( !m_stConditionParam.m_bHistory )
		{
			if ( bSatisfy )
			{
				m_aMerchsOUT.insert(pMerch);
			}
		}
		else
		{
			// 判断在给定的历史时间内, 是否满足条件
			CGmtTime TimeBegin(m_stConditionParam.m_TimeBegin.GetTime());
			CGmtTime TimeEnd(m_stConditionParam.m_TimeEnd.GetTime());

			//
			int32 iPosBegin = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(aKLineFinal, TimeBegin);
			int32 iPosEnd	= CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(aKLineFinal, TimeEnd);

			if ( iPosBegin <= iPosEnd && iPosBegin >= 0 && iPosBegin < aResults.size() && iPosEnd >= 0 && iPosEnd < aResults.size() )
			{
				//
				for ( int32 i = iPosBegin; i <= iPosEnd; i++ )
				{
					if ( pResults[i] >= 1 )
					{
						m_aMerchsOUT.insert(pMerch);
						break;
					}
				}
			}
		}
	}

	//	
	if ( NULL != m_hOwner )
	{
		CString* pStrMsg = new CString();
		*pStrMsg = L"选股完成";
		
		::PostMessage(m_hOwner, UM_Choose_Stock_Progress, (WPARAM)pStrMsg, 1);
	}
}

void CSelectStockCenter::ThreadCustomSelect()
{
	RGUARD(LockSingle, m_LockMerchsOUT, LockMerchsOUT);

	m_aMerchsOUT.clear();
	for ( set<CMerch*, MerchCmp>::iterator itMerch = m_stCustomParam.m_aMerchs.begin(); itMerch != m_stCustomParam.m_aMerchs.end(); ++itMerch )
	{
		if ( m_bStop )break;
		//
		CMerch* pMerch = *itMerch;
		if ( NULL == pMerch )continue;
		
		

		// 发消息给父窗口
		if ( NULL != m_hOwner )
		{
			CString* pStrMsg = new CString();
			*pStrMsg = pMerch->m_MerchInfo.m_StrMerchCnName;
			
			::PostMessage(m_hOwner, UM_Choose_Stock_Progress, (WPARAM)pStrMsg, 0);
		}

		if (NULL == g_pCenterManager)
		{
			continue;
		}

		// 取脱机的行情
		CRealtimePrice stRealtimePrice;
		g_pCenterManager->GetDataManager()->ReadOfflineRealtimePrice(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, stRealtimePrice);
		
		// 取脱机的财务数据
		CFinanceData stFinance;
		
		CString StrPath;
		g_pCenterManager->GetDataManager()->GetPublishFilePath(EPFTF10, pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, StrPath);
		
		CFile FileFinance;		
		if ( FileFinance.Open(StrPath, CFile::modeRead | CFile::typeBinary) )
		{
			int32 iSize = FileFinance.Read(&stFinance, sizeof(stFinance));
			
			if ( iSize != sizeof(stFinance) )
			{
				FileFinance.Close();
				TRACE(L"财务数据读取失败");
				continue;
			}			
		}
		else
		{
			continue;
		}

		FileFinance.Close();

		// 脱机 K 线
		CArray<CKLine, CKLine> aKLines;
		g_pCenterManager->GetDataManager()->ReadOfflineKLines(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, EKTBDay, EOKTOfflineData, 0, -1, aKLines);

		// 是否满足条件
		bool32 bSatisfy = false;

		// 是否是第一次有效判断
		bool32 bFirstValid = true;

		// 看条件是否满足
		for ( int32 iIndexCell = 0; iIndexCell < m_stCustomParam.m_aCondtionCells.GetSize(); iIndexCell++ )
		{
			T_CustomCdtCell stConditionCell = m_stCustomParam.m_aCondtionCells.GetAt(iIndexCell);

			// 需要判断的值
			float fValueNeed = 0.;			
			float fValueAccording = stConditionCell.m_fValue;

			// 取到的值是否是有效的
			bool32 bValidfValueNeed = false;

			// 当次条件是否满足
			bool32 bSatisfyThisTime = false;

			// 判断条件号:
			switch ( stConditionCell.m_iIndex )
			{
			case 1002:
				{
					// L"DMI中的ADX值"		
					CFormularContent* pContent = CFormulaLib::instance()->GetFomular(L"DMI");
					//lint --e{438}
					if ( NULL != pContent )
					{
						T_IndexOutArray* pIndexOut = formula_index(pContent, aKLines);
						
						if ( NULL != pIndexOut )
						{
							int32 iIndexLine = -1;
							if ( pIndexOut->m_aMapNameToIndex.Lookup(L"ADX", iIndexLine) || pIndexOut->m_aMapNameToIndex.Lookup(L"adx", iIndexLine) )
							{
								if ( iIndexLine >= 0 && iIndexLine < pIndexOut->iIndexNum )
								{
									T_IndexOut* pstIndexAdx = &pIndexOut->index[iIndexLine];
									
									// 取到 adx 的值
									if ( pstIndexAdx->iPointNum >= 1 )
									{
										fValueNeed = pstIndexAdx->pPoint[pstIndexAdx->iPointNum - 1];
										bValidfValueNeed = true;
									}
								}
							}
						}

						//
						DEL(pIndexOut);
					}
				}
				break;
			case 1001:
				{
					// L"RSI指标值" 中 RSI2 的值						
					CFormularContent* pContent = CFormulaLib::instance()->GetFomular(L"RSI");
					//lint --e{438}
					if ( NULL != pContent )
					{
						T_IndexOutArray* pIndexOut = formula_index(pContent, aKLines);
						
						if ( NULL != pIndexOut )
						{
							int32 iIndexLine = -1;
							if ( pIndexOut->m_aMapNameToIndex.Lookup(L"RSI2", iIndexLine) || pIndexOut->m_aMapNameToIndex.Lookup(L"rsi2", iIndexLine) )
							{
								if ( iIndexLine >= 0 && iIndexLine < pIndexOut->iIndexNum )
								{
									T_IndexOut* pstIndexAdx = &pIndexOut->index[iIndexLine];
									
									// 取到 RSI2 的值
									if ( pstIndexAdx->iPointNum >= 1 )
									{
										fValueNeed = pstIndexAdx->pPoint[pstIndexAdx->iPointNum - 1];
										bValidfValueNeed = true;
									}
								}
							}
						}
						
						//
						DEL(pIndexOut);
					}
				}
				break;
			case 1000:
				{
					// L"KDJ中的J值",	
					// L"DMI中的ADX值"		
					CFormularContent* pContent = CFormulaLib::instance()->GetFomular(L"KDJ");
					//lint --e{438}
					if ( NULL != pContent )
					{
						T_IndexOutArray* pIndexOut = formula_index(pContent, aKLines);
						
						if ( NULL != pIndexOut )
						{
							int32 iIndexLine = -1;
							if ( pIndexOut->m_aMapNameToIndex.Lookup(L"J", iIndexLine) || pIndexOut->m_aMapNameToIndex.Lookup(L"j", iIndexLine) )
							{
								if ( iIndexLine >= 0 && iIndexLine < pIndexOut->iIndexNum )
								{
									T_IndexOut* pstIndexJ = &pIndexOut->index[iIndexLine];
									
									// 取到 adx 的值
									if ( pstIndexJ->iPointNum >= 1 )
									{
										fValueNeed = pstIndexJ->pPoint[pstIndexJ->iPointNum - 1];
										bValidfValueNeed = true;
									}
								}
							}
						}
						
						//
						DEL(pIndexOut);
					}
				}
				break;
			case (int32)CReportScheme::ERHAllCapital:
				{
					// 总股本
					fValueNeed = stFinance.m_fAllCapical;
					fValueNeed /= 10000.0f;

					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHCircAsset:
				{
					// 流通股本
					fValueNeed = stFinance.m_fCircAsset;
					fValueNeed /= 10000.0f;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHAllAsset:
				{
					// 总资产
					fValueNeed = stFinance.m_fAllAsset;
					fValueNeed /= 10000.0f;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHFlowDebt:
				{
					// 流动负债
					fValueNeed = stFinance.m_fFlowDebt;
					fValueNeed /= 10000.0f;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHPerFund:
				{
					// 每股公积金
					fValueNeed = stFinance.m_fPerFund;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHBusinessProfit:
				{
					// 营业利益
					fValueNeed = stFinance.m_fBusinessProfit;
					fValueNeed /= 10000.0f;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHPerNoDistribute:
				{
					// 每股未分配
					fValueNeed = stFinance.m_fPerNoDistribute;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHPerIncomeYear:
				{
					// 每股收益(年)
					fValueNeed = stFinance.m_fPerIncomeYear;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHPerPureAsset:
				{
					// 每股净资产	
					fValueNeed = stFinance.m_fPerPureAsset;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHChPerPureAsset:
				{
					// 调整每股净资产
					fValueNeed = stFinance.m_fChPerPureAsset;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHDorRightRate:
				{
					// 股东权益比	
					fValueNeed = stFinance.m_fDorRightRate;
					//fValueNeed *= 100.0f;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHPriceNew:
				{
					// 最新
					fValueNeed = stRealtimePrice.m_fPriceNew;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHPriceOpen:
				{
					// 开盘
					fValueNeed = stRealtimePrice.m_fPriceOpen;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHPriceHigh:
				{
					// 最高
					fValueNeed = stRealtimePrice.m_fPriceHigh;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHPriceLow:
				{
					// 最低
					fValueNeed = stRealtimePrice.m_fPriceLow;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHPricePrevClose:
				{
					// 昨收
					fValueNeed = stRealtimePrice.m_fPricePrevClose;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHVolumeTotal:
				{
					// 总量
					fValueNeed = stRealtimePrice.m_fVolumeTotal;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHAmount:
				{
					// 总额
					fValueNeed = stRealtimePrice.m_fAmountTotal;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHRange:
				{
					// 涨幅
					if ( 0. != stRealtimePrice.m_fPricePrevClose )
					{
						fValueNeed = 100.0f * (stRealtimePrice.m_fPriceNew - stRealtimePrice.m_fPricePrevClose) / stRealtimePrice.m_fPricePrevClose;
						bValidfValueNeed = true;
					}					
				}
				break;
			case (int32)CReportScheme::ERHSwing:
				{
					// 震幅
					if ( 0. != stRealtimePrice.m_fPricePrevClose )
					{
						fValueNeed = 100.0f * (stRealtimePrice.m_fPriceHigh - stRealtimePrice.m_fPriceLow) / stRealtimePrice.m_fPricePrevClose;
						bValidfValueNeed = true;
					}
				}
				break;
			case (int32)CReportScheme::ERHMarketWinRate:
				{
					// 市盈率
					fValueNeed = /*100.0f **/ stRealtimePrice.m_fPeRate;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHChangeRate:
				{
					// 换手率
					fValueNeed = /*100.0f * */stRealtimePrice.m_fTradeRate;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHVolumeRate:
				{
					// 量比
					fValueNeed = /*100.0f **/ stRealtimePrice.m_fVolumeRate;
					bValidfValueNeed = true;
				}
				break;
			default:
				{
					////ASSERT(0);
				}
				break;
			}

			// 判断是否符合条件
			if ( bValidfValueNeed )
			{
				if ( -1 == stConditionCell.m_iCmp )
				{
					// 小于
					if ( fValueNeed < fValueAccording )
					{
						bSatisfyThisTime = true;
					}
				}
				else if ( 0 == stConditionCell.m_iCmp )
				{
					// 等于
					if ( fValueNeed == fValueAccording )
					{
						bSatisfyThisTime = true;
					}
				}
				else if ( 1 == stConditionCell.m_iCmp )
				{
					// 大于
					if ( fValueNeed > fValueAccording )
					{
						bSatisfyThisTime = true;
					}
				}
				else 
				{
					////ASSERT(0);
				}	
			}

			//
			if ( bFirstValid )
			{
				bFirstValid = false;

				// 直接赋值
				bSatisfy = bSatisfyThisTime;
			}
			else
			{
				if ( m_stCustomParam.m_bAndCondition )
				{
					// 与条件
					bSatisfy = bSatisfy && bSatisfyThisTime;

					if ( !bSatisfy )
					{
						// 与 条件, 只要一个不满足, 后面就不用算了
						break;
					}
				}
				else
				{
					// 或条件
					bSatisfy = bSatisfy || bSatisfyThisTime;
				}
			}
		}

		//
		if ( bSatisfy )
		{
			m_aMerchsOUT.insert(pMerch);
		}
	}

	//	
	if ( NULL != m_hOwner )
	{
		CString* pStrMsg = new CString();
		*pStrMsg = L"选股完成";
		
		::PostMessage(m_hOwner, UM_Choose_Stock_Progress, (WPARAM)pStrMsg, 1);
	}
}
//
//void SetServerTime(CGmtTime serverTime)
//{
//	g_serverTime = serverTime;
//}

void SetAbsCenterManager(CAbsCenterManager *pCenterManager)
{
	ASSERT(pCenterManager);
	g_pCenterManager = pCenterManager;
}

//void SetUserName(CString strUserName)
//{
//	g_strUserName = strUserName;
//}

