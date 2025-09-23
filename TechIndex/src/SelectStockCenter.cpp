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
// �������ݶ���
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

		// ����Ϣ��������
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
		// ׼�����ѻ� K ��	
		CArray<CKLine, CKLine> aKLineSrc;		
		g_pCenterManager->GetDataManager()->ReadOfflineKLines(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, eKLineBase, EOKTOfflineData, 0, -1, aKLineSrc);
		if ( aKLineSrc.GetSize() <= 0 )
		{
			TRACE(L"%d-%s û���ѻ�K������\n", pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer());
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

		// ������ǻ������ڵ�, �ϲ�K��
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
		
		// ��ʼ����ָ��:

		// ������ʱ��������������� (��ʷ)
		vector<float> aResults(aKLineFinal.GetSize(), 0.);
		float* pResults = &(*aResults.begin());

		// �û��������ı��� (����ʷ)
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
				// ����ѡ�ɵĹ�ʽֻ����һ�����ֵ����, ������е�Ԫ�ظ���Ӧ����K �߸���һ��
				////ASSERT(0);
				continue;
			}

			//
			int32 iSize = aKLineFinal.GetSize();

			//
			if ( !m_stConditionParam.m_bHistory )
			{
				// ����ʷ�׶�, ֻ�ж�����һ���������Ƿ�����:

				// ��ǰָ���Ƿ�����
				bool32 bSatisfyNow = pIndex->index[0].pPoint[iSize-1] == 1.0 ? true : false;

				if ( itFormular == m_stConditionParam.m_aFormulars.begin() )
				{
					// ��һ��ֱ�Ӹ�ֵ:
					bSatisfy = bSatisfyNow;
				}
				else
				{
					// �Ժ�ĸ������������ǻ�������ȡֵ
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
				// ��ʷ�׶�ѡ��
				if ( itFormular == m_stConditionParam.m_aFormulars.begin() )
				{
					// ��һ��ֱ�Ӹ�ֵ:
					for ( int32 i = 0; i < pIndex->index[0].iPointNum; i++ )
					{
						float fNow = pIndex->index[0].pPoint[i];
						pResults[i] = fNow;
					}
				}
				else 
				{
					// �Ժ�ĸ������������ǻ�������ȡֵ
					if ( m_stConditionParam.m_bAndCondition )
					{
						// ��
						for ( int32 i = 0; i < pIndex->index[0].iPointNum; i++ )
						{
							float fNow	  = pIndex->index[0].pPoint[i];
							float fBefore = pResults[i];
							
							pResults[i] = fNow * fBefore;	
						}	
					}
					else
					{
						// ��
						for ( int32 i = 0; i < pIndex->index[0].iPointNum; i++ )
						{
							float fNow	  = pIndex->index[0].pPoint[i];
							float fBefore = pResults[i];
							
							pResults[i] = fNow + fBefore;	
						}
					}	
				}
			}

			// �ͷ�
			DEL(pIndex);
		}
		
		// ��������Ʒ�Ƿ���������
		if ( !m_stConditionParam.m_bHistory )
		{
			if ( bSatisfy )
			{
				m_aMerchsOUT.insert(pMerch);
			}
		}
		else
		{
			// �ж��ڸ�������ʷʱ����, �Ƿ���������
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
		*pStrMsg = L"ѡ�����";
		
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
		
		

		// ����Ϣ��������
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

		// ȡ�ѻ�������
		CRealtimePrice stRealtimePrice;
		g_pCenterManager->GetDataManager()->ReadOfflineRealtimePrice(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, stRealtimePrice);
		
		// ȡ�ѻ��Ĳ�������
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
				TRACE(L"�������ݶ�ȡʧ��");
				continue;
			}			
		}
		else
		{
			continue;
		}

		FileFinance.Close();

		// �ѻ� K ��
		CArray<CKLine, CKLine> aKLines;
		g_pCenterManager->GetDataManager()->ReadOfflineKLines(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, EKTBDay, EOKTOfflineData, 0, -1, aKLines);

		// �Ƿ���������
		bool32 bSatisfy = false;

		// �Ƿ��ǵ�һ����Ч�ж�
		bool32 bFirstValid = true;

		// �������Ƿ�����
		for ( int32 iIndexCell = 0; iIndexCell < m_stCustomParam.m_aCondtionCells.GetSize(); iIndexCell++ )
		{
			T_CustomCdtCell stConditionCell = m_stCustomParam.m_aCondtionCells.GetAt(iIndexCell);

			// ��Ҫ�жϵ�ֵ
			float fValueNeed = 0.;			
			float fValueAccording = stConditionCell.m_fValue;

			// ȡ����ֵ�Ƿ�����Ч��
			bool32 bValidfValueNeed = false;

			// ���������Ƿ�����
			bool32 bSatisfyThisTime = false;

			// �ж�������:
			switch ( stConditionCell.m_iIndex )
			{
			case 1002:
				{
					// L"DMI�е�ADXֵ"		
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
									
									// ȡ�� adx ��ֵ
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
					// L"RSIָ��ֵ" �� RSI2 ��ֵ						
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
									
									// ȡ�� RSI2 ��ֵ
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
					// L"KDJ�е�Jֵ",	
					// L"DMI�е�ADXֵ"		
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
									
									// ȡ�� adx ��ֵ
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
					// �ܹɱ�
					fValueNeed = stFinance.m_fAllCapical;
					fValueNeed /= 10000.0f;

					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHCircAsset:
				{
					// ��ͨ�ɱ�
					fValueNeed = stFinance.m_fCircAsset;
					fValueNeed /= 10000.0f;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHAllAsset:
				{
					// ���ʲ�
					fValueNeed = stFinance.m_fAllAsset;
					fValueNeed /= 10000.0f;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHFlowDebt:
				{
					// ������ծ
					fValueNeed = stFinance.m_fFlowDebt;
					fValueNeed /= 10000.0f;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHPerFund:
				{
					// ÿ�ɹ�����
					fValueNeed = stFinance.m_fPerFund;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHBusinessProfit:
				{
					// Ӫҵ����
					fValueNeed = stFinance.m_fBusinessProfit;
					fValueNeed /= 10000.0f;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHPerNoDistribute:
				{
					// ÿ��δ����
					fValueNeed = stFinance.m_fPerNoDistribute;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHPerIncomeYear:
				{
					// ÿ������(��)
					fValueNeed = stFinance.m_fPerIncomeYear;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHPerPureAsset:
				{
					// ÿ�ɾ��ʲ�	
					fValueNeed = stFinance.m_fPerPureAsset;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHChPerPureAsset:
				{
					// ����ÿ�ɾ��ʲ�
					fValueNeed = stFinance.m_fChPerPureAsset;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHDorRightRate:
				{
					// �ɶ�Ȩ���	
					fValueNeed = stFinance.m_fDorRightRate;
					//fValueNeed *= 100.0f;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHPriceNew:
				{
					// ����
					fValueNeed = stRealtimePrice.m_fPriceNew;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHPriceOpen:
				{
					// ����
					fValueNeed = stRealtimePrice.m_fPriceOpen;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHPriceHigh:
				{
					// ���
					fValueNeed = stRealtimePrice.m_fPriceHigh;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHPriceLow:
				{
					// ���
					fValueNeed = stRealtimePrice.m_fPriceLow;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHPricePrevClose:
				{
					// ����
					fValueNeed = stRealtimePrice.m_fPricePrevClose;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHVolumeTotal:
				{
					// ����
					fValueNeed = stRealtimePrice.m_fVolumeTotal;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHAmount:
				{
					// �ܶ�
					fValueNeed = stRealtimePrice.m_fAmountTotal;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHRange:
				{
					// �Ƿ�
					if ( 0. != stRealtimePrice.m_fPricePrevClose )
					{
						fValueNeed = 100.0f * (stRealtimePrice.m_fPriceNew - stRealtimePrice.m_fPricePrevClose) / stRealtimePrice.m_fPricePrevClose;
						bValidfValueNeed = true;
					}					
				}
				break;
			case (int32)CReportScheme::ERHSwing:
				{
					// ���
					if ( 0. != stRealtimePrice.m_fPricePrevClose )
					{
						fValueNeed = 100.0f * (stRealtimePrice.m_fPriceHigh - stRealtimePrice.m_fPriceLow) / stRealtimePrice.m_fPricePrevClose;
						bValidfValueNeed = true;
					}
				}
				break;
			case (int32)CReportScheme::ERHMarketWinRate:
				{
					// ��ӯ��
					fValueNeed = /*100.0f **/ stRealtimePrice.m_fPeRate;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHChangeRate:
				{
					// ������
					fValueNeed = /*100.0f * */stRealtimePrice.m_fTradeRate;
					bValidfValueNeed = true;
				}
				break;
			case (int32)CReportScheme::ERHVolumeRate:
				{
					// ����
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

			// �ж��Ƿ��������
			if ( bValidfValueNeed )
			{
				if ( -1 == stConditionCell.m_iCmp )
				{
					// С��
					if ( fValueNeed < fValueAccording )
					{
						bSatisfyThisTime = true;
					}
				}
				else if ( 0 == stConditionCell.m_iCmp )
				{
					// ����
					if ( fValueNeed == fValueAccording )
					{
						bSatisfyThisTime = true;
					}
				}
				else if ( 1 == stConditionCell.m_iCmp )
				{
					// ����
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

				// ֱ�Ӹ�ֵ
				bSatisfy = bSatisfyThisTime;
			}
			else
			{
				if ( m_stCustomParam.m_bAndCondition )
				{
					// ������
					bSatisfy = bSatisfy && bSatisfyThisTime;

					if ( !bSatisfy )
					{
						// �� ����, ֻҪһ��������, ����Ͳ�������
						break;
					}
				}
				else
				{
					// ������
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
		*pStrMsg = L"ѡ�����";
		
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

