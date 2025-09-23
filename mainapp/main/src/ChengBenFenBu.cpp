#include "StdAfx.h"
#include "MerchManager.h"

#include <cmath>
#include <float.h>

#include "ChengBenFenBu.h"
#include "Mmsystem.h"

#include <algorithm>
using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
bool32 CalcChengBenFenBuRedY(const CKLine *pKline, int32 iKlineCount, float fLiuTongGuBenOrg, const ChengBenFenBuCycleArray &aCycles, const T_ChengBenFenBuCalcParam *pCalcParam, OUT ChengBenFenBuNodeMap &mapNodes) // ��ͨ�� - ��
{
#ifdef _DEBUG
	DWORD dwTimeStart = timeGetTime();
#endif
	mapNodes.clear(); // ��ս��

	if ( NULL == pKline )
	{
		ASSERT( 0 );
		return false;
	}

	float fPricePerPixel = 0.0f;
	E_ChengBenFenBuSuanFa eCBFBSuanFa = ECBFB_PingJun;	// ��ʱ������
	float fTradeRateRatio = 1.0f;
	float fDefaultTradeRate = 0.0f;
	UINT uDivide = CBFB_DEFAULT_DIVIDE;
	if ( pCalcParam != NULL )
	{
		eCBFBSuanFa = pCalcParam->eCBFBSuanFa;
		fTradeRateRatio = pCalcParam->fTradeRateRatio;
		fDefaultTradeRate = pCalcParam->fDefaultTradeRate;
		uDivide = pCalcParam->m_uDivide;

		if ( eCBFBSuanFa != ECBFB_PingJun && eCBFBSuanFa != ECBFB_SanJiao )
		{
			ASSERT( 0 );
			return false;
		}

		if ( fTradeRateRatio < CBFB_MIN_TRADERATERADIO || fTradeRateRatio > CBFB_MAX_TRADERATERADIO )
		{
			ASSERT( 0 );
			return false;
		}

		if ( uDivide < CBFB_MIN_DIVIDE || uDivide > CBFB_MAX_DIVIDE )
		{
			ASSERT( 0 );
			return false;
		}
	}

	int32 i = 0;
	float fPriceMin, fPriceMax;
	fPriceMin = FLT_MAX;
	fPriceMax = -FLT_MAX;
	float fLiuTongGuBen = fLiuTongGuBenOrg;
	if ( fLiuTongGuBen <= 1.0 && (fDefaultTradeRate <= 0.0 || fDefaultTradeRate >= 1.0) )
	{
		fLiuTongGuBen = 1e-6;
		for ( int32 i=0; i < iKlineCount ; i++ )
		{
			const CKLine &kline = pKline[i];
			fLiuTongGuBen += kline.m_fVolume;	// ���гɽ���֮��
			
			if ( kline.m_fPriceHigh > fPriceMax )
			{
				fPriceMax = kline.m_fPriceHigh;
			}
			if ( kline.m_fPriceLow < fPriceMin )
			{
				fPriceMin = kline.m_fPriceLow;
			}
		}
	}
	else 
	{
		for ( int32 i=0; i < iKlineCount ; i++ )
		{
			const CKLine &kline = pKline[i];
			
			if ( kline.m_fPriceHigh > fPriceMax )
			{
				fPriceMax = kline.m_fPriceHigh;
			}
			if ( kline.m_fPriceLow < fPriceMin )
			{
				fPriceMin = kline.m_fPriceLow;
			}
		}
	}
	if ( fLiuTongGuBen <= 1.0 )
	{
		fLiuTongGuBen = 1e-6;
	}

	if ( (fLiuTongGuBen <= 1.0 && (fDefaultTradeRate <= 0.0 || fDefaultTradeRate >= 1.0)) )
	{
		return false;	// �޷�������ȥ
	}


	
	// ����������Ҫ��
	// �����Ľڵ㶼��xx��֮ǰ��
	ChengBenFenBuCycleArray aSortKeys;
	{
		ChengBenFenBuCycleArray aTmp = aCycles;
		aTmp.push_back(0);	// �ܼ���һ��0
		sort(aTmp.begin(), aTmp.end());
		// ����ظ�����
		int32 i = 0;
		for ( i=0; i < aTmp.size()-1 ; i++ )
		{
			if ( aTmp.at(i) != aTmp.at(i+1) )
			{
				aSortKeys.push_back(aTmp.at(i));
			}
		}
		aSortKeys.push_back(aTmp.back());

		for ( i = 0; i < aSortKeys.size() ; i++  )
		{
			mapNodes[aSortKeys[i]];	// ����Ĭ��ֵ
		}
		
		ASSERT( aSortKeys[0]==0 && (aSortKeys.size() == 1 || aSortKeys[0] < aSortKeys[1]) );
	}
	// �������ܻ����0
	ChengBenFenBuNodeMap::iterator it = mapNodes.find(0);
	if ( it == mapNodes.end() )
	{
		ASSERT( 0 );
		return false;
	}
	ChengBenFenBuNodeArray &aNodes = it->second;	// ����

	aNodes.clear();
	fPricePerPixel = (fPriceMax-fPriceMin)/uDivide;		// ÿ�ȷֵļ۸�
	aNodes.insert(aNodes.end(), uDivide, T_ChengBenFenBuCalcNode());	// ��ֵĬ��
	
	i = 0;
	for ( ChengBenFenBuNodeArray::iterator itArray=aNodes.begin(); itArray != aNodes.end() ; itArray++ )
	{
		itArray->iPixel = i;
		itArray->fPrice = fPriceMin + fPricePerPixel *i;
		i++;
	}

	// http://hi.baidu.com/lweml/blog/item/e9a3a764ce546cfff736541c.html
	// a)����ÿһ��ĳɱ��ֲ�ͼ���ǵ��ճɱ�������ͼ۵���߼ۼ��һ�����Ρ�	
	// b)���ÿһ��ĳɱ��ֲ�ͼ����Y=��1-A��*Y��+A*B��A��ʾ���ջ��֣�B��ʾ���ճɱ���Y��Y���ֱ��ʾ���պ���һ�յĳɱ��ֲ���ע�⣬�˴�BYY������ʾһ���ֲ������������һ����ֵ��
	// ����:  A*B����Ҫ����A���������Ϊ�����ʵ�����������û�г�
	int32 iIndex = 0;		// ~~
	if ( iKlineCount > 0 )
	{
		// ȡ��һ�����ݵ� - ��һ���ɱ�������ȡ�Ļ���̫����, ȡ����
// 		while ( iIndex < iKlineCount )
// 		{
// 			const CKLine &kline = pKline[iIndex++];	// !!
// 
// 			const int32 iEndPos    = int32((kline.m_fPriceHigh - fPriceMin)/ fPricePerPixel);
// 			const int32 iStartPos = int32((kline.m_fPriceLow - fPriceMin)/fPricePerPixel);
// 			const float fVolPerPixel = kline.m_fVolume / (iEndPos-iStartPos+1);
// 			ASSERT( !_isnan(fVolPerPixel) && _finite(fVolPerPixel) && fVolPerPixel >= 0.0 );
// 			if ( _isnan(fVolPerPixel) || !_finite(fVolPerPixel) || fVolPerPixel < 0.0 )
// 			{
// 				continue;	// �Ƿ�ֵ ��������
// 			}
// 			//ASSERT( aNodes.size() == uDivide );
// 			for ( int32 iNode = iStartPos; iNode < aNodes.size() && iNode <= iEndPos ; iNode++ )
// 			{
// 				aNodes.at(iNode).fVol += fVolPerPixel;	// �����һ�μ����������ɽ���ƽ����ÿ����λ������
// 			}
// 			break;	// ֻȡ��һ����Чֵ
// 		}
	}

	
	
	// �������ݵ� Y = (1-A)Y' + B*A
	
	// �Ӿ�����������Զ�ĵ�����
	for ( vector<ChengBenFenBuNodeMap::key_type>::reverse_iterator itId = aSortKeys.rbegin(); itId != aSortKeys.rend() ; itId++ )
	{
		const int32 iCalcCount = iKlineCount - *itId;	// ����id��������������ڶ�������

		if ( iCalcCount < iIndex )
		{
			continue; // ���ܼ����������
		}

		// ƽ���㷨���������㷨
		// ������ ������� ���ߣ��ͼۼ���
		if ( ECBFB_SanJiao == eCBFBSuanFa )
		{
			for ( ; iIndex < iCalcCount ; iIndex++ )
			{
				const CKLine &kline = pKline[iIndex]; 
				
				float fRate = fDefaultTradeRate;
				if ( fLiuTongGuBen > 1.0 )
				{
					fRate = kline.m_fVolume / fLiuTongGuBen; // ������
				}
				
				fRate = fRate * fTradeRateRatio;	// ����˥��ϵ��
				
				//ASSERT( fRate < 1.0 && fRate > 0.0 );
				if ( fRate <= 0.0 )
				{
					fRate = 0.0001f;
					//continue;	// �������
				}
				else if ( fRate >= 1.0 )
				{
					fRate = 0.999f;		// �������󣿣�
				}
				
				// �������㷨
				//const int32 iCount    = int32((kline.m_fPriceHigh - kline.m_fPriceLow)/ fPricePerPixel) +1;
				const int32 iEndPos    = int32((kline.m_fPriceHigh - fPriceMin)/ fPricePerPixel);
				const int32 iStartPos = int32((kline.m_fPriceLow - fPriceMin)/fPricePerPixel);
				const float fVolPerPixel = kline.m_fVolume / (iEndPos-iStartPos+1);
				int32 iLow2AvgPos = (iEndPos+iStartPos)/2;	// �м��λ��
				if ( !(fVolPerPixel > 0.0) )
				{
					continue;	// �Ƿ�ֵ����
				}
				// ��avgΪƽ�ֵ㣬�ϰ벿�ְ���[1->n]�ۼӷ�̯1/2�ĳɽ��� �°벿��ͬ
				const int32 iLow2AvgCount = (iLow2AvgPos-iStartPos+1);
				const int32 iHigh2AvgCount = (iEndPos-iLow2AvgPos+1);
				const float fLowPerPixel = kline.m_fVolume / (iLow2AvgCount*(iLow2AvgCount+1));
				const float fHighPerPixel = kline.m_fVolume / (iHigh2AvgCount*(iHigh2AvgCount+1));
				
				//ASSERT( aNodes.size() == uDivide );
				const float fRate2 = 1.0f - fRate;
				int32 iNode;
				for ( iNode=0; iNode < aNodes.size() ; iNode++ )
				{
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// ��һ�μ���Ľ��
					node.fVol = (fRate2)*node.fVol;		// ȫ��˥��
				}
				for ( iNode=iStartPos; iNode <= iLow2AvgPos && iNode < aNodes.size() ; iNode++ )
				{
					// �°벿��, ������
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// ��һ�μ���Ľ��
					node.fVol += /*fRate**/fLowPerPixel*(iNode-iStartPos+1);		// ˥��
				}
				for ( iNode=iLow2AvgPos; iNode<=iEndPos&&iNode<aNodes.size() ; iNode++ )
				{
					// �ϰ벿�֣�������
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// ��һ�μ���Ľ��
					node.fVol += /*fRate**/fHighPerPixel*(iEndPos-iNode+1);		// ˥��
				}
				
				// ��ǰ���Ѿ������˵����ڽ���ƽ������
				{
					// �˴�Ӧ�ÿ�����ֲ������ɺ�ƽ����TODO
					for ( vector<ChengBenFenBuNodeMap::key_type>::reverse_iterator itPreId = aSortKeys.rbegin(); itPreId != itId ; itPreId++ )
					{
						if ( iKlineCount - *itPreId <= 0 )
						{
							continue;	// ��Щ�ǲ��ܴ����
						}
						//ASSERT( *itPreId != it->first );
						
						ChengBenFenBuNodeMap::iterator itPre = mapNodes.find(*itPreId);
						//ASSERT( aNodes.size() == itPre->second.size() );
						for ( iNode = 0; iNode < itPre->second.size() ; iNode++ )	// ����ÿһ����λ����
						{
							T_ChengBenFenBuCalcNode &node = itPre->second.at(iNode);	// ��һ�μ���Ľ��
							// ȫ������û�е�
							node.fVol  = (fRate2)*node.fVol;	// �ǲ��������˼��
						}
					}
				}
			}
		}
		else
		{
			for ( ; iIndex < iCalcCount ; iIndex++ )
			{
				const CKLine &kline = pKline[iIndex]; 
				
				float fRate = fDefaultTradeRate;
				if ( fLiuTongGuBen > 1.0 )
				{
					fRate = kline.m_fVolume / fLiuTongGuBen; // ������
				}
				
				fRate = fRate * fTradeRateRatio;	// ����˥��ϵ��
				const float fRate2 = 1.0-fRate;
				
				//ASSERT( fRate < 1.0 && fRate > 0.0 );
				if ( fRate <= 0.0 )
				{
					fRate = 0.0001f;
					//continue;	// �������
				}
				else if ( fRate >= 1.0 )
				{
					fRate = 0.999f;		// �������󣿣�
				}
				
				int32 iEndPos    = int32((kline.m_fPriceHigh - fPriceMin)/ fPricePerPixel);
				const int32 iStartPos = int32((kline.m_fPriceLow - fPriceMin)/fPricePerPixel);
				int32 iNodeSize = aNodes.size() - 1;
				iEndPos = min(iEndPos, iNodeSize);
				const float fVolPerPixel = kline.m_fVolume / (iEndPos-iStartPos+1);
				if ( !(fVolPerPixel > 0.0) )
				{
					continue;	// �Ƿ�ֵ����
				}
				
				//ASSERT( aNodes.size() == uDivide );
				int32 iNode;
				for ( iNode = 0; iNode < aNodes.size() ; iNode++ )	// ����ÿһ����λ����
				{
					// ����ֵ˥��
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// ��һ�μ���Ľ��
					// ��Щ������ֵ�ģ���Щ����û��ֵ��
					node.fVol  = fRate2*node.fVol;	// �ǲ��������˼��
				}
				for ( iNode=iStartPos; iNode <= iEndPos && iNode < aNodes.size() ; iNode++ )
				{
					// ����ֵ�ۼ�
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// ��һ�μ���Ľ��
					node.fVol  +=  fVolPerPixel /** fRate*/;	// �ǲ��������˼��, ����Ҫ�������������Ϊ�����ʵ�����������û�г�
				}
				
				// ��ǰ���Ѿ������˵����ڽ���ƽ������
				{
					// �˴�Ӧ�ÿ�����ֲ������ɺ�ƽ����TODO
					for ( vector<ChengBenFenBuNodeMap::key_type>::reverse_iterator itPreId = aSortKeys.rbegin(); itPreId != itId ; itPreId++ )
					{
						if ( iKlineCount - *itPreId <= 0 )
						{
							continue;	// ��Щ�ǲ��ܴ����
						}
						//ASSERT( *itPreId != it->first );
						
						ChengBenFenBuNodeMap::iterator itPre = mapNodes.find(*itPreId);
						//ASSERT( aNodes.size() == itPre->second.size() );
						for ( iNode = 0; iNode < itPre->second.size() ; iNode++ )	// ����ÿһ����λ����
						{
							T_ChengBenFenBuCalcNode &node = itPre->second.at(iNode);	// ��һ�μ���Ľ��
							// ȫ������û�е�
							node.fVol  = fRate2*node.fVol;	// �ǲ��������˼��
						}
					}
				}
			}
		}

		if ( *itId != it->first )
		{
			mapNodes[ *itId ] = aNodes;	// ������ʱ��״̬�����������Ǿ���n֮ǰ�Ľ��
		}
	}

// 	{
// 		// ����������ļ�
// 		char buf[200];
// 		float fVol = 0.0;
// 		HANDLE hFile = CreateFile(_T("d:\\cbfb.nodes"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
// 		DWORD dw = 0;
// 		for ( int32 iNode = 0; iNode < aNodes.size() ; iNode++ )	// ����ÿһ����λ����
// 		{
// 			T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// ��һ�μ���Ľ��
// 			// ȫ������û�е�
// 			_snprintf(buf, sizeof(buf), "%d - %0.3f - %f\r\n", node.iPixel, node.fPrice, node.fVol);
// 			WriteFile(hFile, buf, strlen(buf), &dw, NULL);
// 			fVol += node.fVol;
// 		}
// 		_snprintf(buf, sizeof(buf), "��: %f\r\n", fVol);
// 		WriteFile(hFile, buf, strlen(buf), &dw, NULL);
// 		DEL_HANDLE(hFile);
// 	}

#ifdef _DEBUG
	dwTimeStart = timeGetTime() - dwTimeStart;
	TRACE(_T("�ɱ��ֲ�����(���,����:%d [%d*%d]): %d ms\r\n"), eCBFBSuanFa, iKlineCount, aNodes.size(), dwTimeStart);
#endif
	

	return true;
}

bool32 CalcChengBenFenBuBlueG( const CKLine *pKline, int32 iKlineCount, float fLiuTongGuBenOrg, const ChengBenFenBuCycleArray &aCycles, const T_ChengBenFenBuCalcParam *pCalcParam, OUT ChengBenFenBuNodeMap &mapNodes )
{
#ifdef _DEBUG
	DWORD dwTimeStart = timeGetTime();
#endif
	

	int32 iNodeSize = 0;
	if ( CalcChengBenFenBuRedY(pKline, iKlineCount, fLiuTongGuBenOrg, aCycles, pCalcParam, mapNodes) )
	{
		// ���ڵĽ����xx��������ݣ�0���������Ϊ��������������
		// ����ͼ��0���������������ݲ��䣬�����������ƻ���
		ChengBenFenBuNodeMap::iterator itTotal = mapNodes.find(0);
		if ( itTotal == mapNodes.end() )
		{
			ASSERT( 0 );
			return false;
		}
		const ChengBenFenBuNodeArray &aTotalNodes = itTotal->second;	// ������
		iNodeSize = aTotalNodes.size();
		for ( ChengBenFenBuNodeMap::iterator itCalc=mapNodes.begin()
			; itCalc != mapNodes.end()
			; ++itCalc )
		{
			if ( itCalc == itTotal )
			{
				continue;	// ����������
			}

			ASSERT( itCalc->second.size()==0 || itCalc->second.size() == aTotalNodes.size() );
			for ( int32 iNode=0; iNode < itCalc->second.size() ; iNode++ )
			{
				T_ChengBenFenBuCalcNode &node = itCalc->second.at(iNode);
				node.fVol = aTotalNodes[iNode].fVol - node.fVol;
			}
		}
	}

#ifdef _DEBUG
	dwTimeStart = timeGetTime() - dwTimeStart;
	TRACE(_T("�ɱ��ֲ�����(����[%d*%d]): %d ms\r\n"), iKlineCount, iNodeSize, dwTimeStart);
#endif
	

	return true;
}


bool32 CalcChengBenFenBuBlueG2( const CKLine *pKline, int32 iKlineCount, float fLiuTongGuBenOrg, const ChengBenFenBuCycleArray &aCycles, const T_ChengBenFenBuCalcParam *pCalcParam, OUT ChengBenFenBuNodeMap &mapNodes )
{
#ifdef _DEBUG
	DWORD dwTimeStart = timeGetTime();
#endif
	mapNodes.clear(); // ��ս��

	if ( NULL == pKline )
	{
		ASSERT( 0 );
		return false;
	}

	float fPricePerPixel = 0.0f;
	E_ChengBenFenBuSuanFa eCBFBSuanFa = ECBFB_PingJun;	// ��ʱ������
	float fTradeRateRatio = 1.0f;
	float fDefaultTradeRate = 0.0f;
	UINT uDivide = CBFB_DEFAULT_DIVIDE;
	if ( pCalcParam != NULL )
	{
		eCBFBSuanFa = pCalcParam->eCBFBSuanFa;
		fTradeRateRatio = pCalcParam->fTradeRateRatio;
		fDefaultTradeRate = pCalcParam->fDefaultTradeRate;
		uDivide = pCalcParam->m_uDivide;

		if ( eCBFBSuanFa != ECBFB_PingJun && eCBFBSuanFa != ECBFB_SanJiao )
		{
			ASSERT( 0 );
			return false;
		}

		if ( fTradeRateRatio < CBFB_MIN_TRADERATERADIO || fTradeRateRatio > CBFB_MAX_TRADERATERADIO )
		{
			ASSERT( 0 );
			return false;
		}

		if ( uDivide < CBFB_MIN_DIVIDE || uDivide > CBFB_MAX_DIVIDE )
		{
			ASSERT( 0 );
			return false;
		}
	}

	int32 i = 0;
	float fPriceMin, fPriceMax;
	fPriceMin = FLT_MAX;
	fPriceMax = -FLT_MAX;
	float fLiuTongGuBen = fLiuTongGuBenOrg;
	if ( fLiuTongGuBen <= 1.0 && (fDefaultTradeRate <= 0.0 || fDefaultTradeRate >= 1.0) )
	{
		fLiuTongGuBen = 1e-6;
		for ( int32 i=0; i < iKlineCount ; i++ )
		{
			const CKLine &kline = pKline[i];
			fLiuTongGuBen += kline.m_fVolume;	// ���гɽ���֮��
			
			if ( kline.m_fPriceHigh > fPriceMax )
			{
				fPriceMax = kline.m_fPriceHigh;
			}
			if ( kline.m_fPriceLow < fPriceMin )
			{
				fPriceMin = kline.m_fPriceLow;
			}
		}
	}
	else 
	{
		for ( int32 i=0; i < iKlineCount ; i++ )
		{
			const CKLine &kline = pKline[i];
			
			if ( kline.m_fPriceHigh > fPriceMax )
			{
				fPriceMax = kline.m_fPriceHigh;
			}
			if ( kline.m_fPriceLow < fPriceMin )
			{
				fPriceMin = kline.m_fPriceLow;
			}
		}
	}
	if ( fLiuTongGuBen <= 1.0 )
	{
		fLiuTongGuBen = 1e-6;
	}

	if ( (fLiuTongGuBen <= 1.0 && (fDefaultTradeRate <= 0.0 || fDefaultTradeRate >= 1.0)) )
	{
		return false;	// �޷�������ȥ
	}

	

	
	// ����������Ҫ��
	// �����Ľڵ㶼��xx��֮ǰ��
	ChengBenFenBuCycleArray aSortKeys;
	{
		ChengBenFenBuCycleArray aTmp = aCycles;
		aTmp.push_back(0);	// �ܼ���һ��0
		sort(aTmp.begin(), aTmp.end());
		// ����ظ�����
		int32 i = 0;
		for ( i=0; i < aTmp.size()-1 ; i++ )
		{
			if ( aTmp.at(i) != aTmp.at(i+1) )
			{
				aSortKeys.push_back(aTmp.at(i));
			}
		}
		aSortKeys.push_back(aTmp.back());

		for ( i = 0; i < aSortKeys.size() ; i++  )
		{
			mapNodes[aSortKeys[i]];	// ����Ĭ��ֵ
		}
		
		ASSERT( aSortKeys[0]==0 && (aSortKeys.size() == 1 || aSortKeys[0] < aSortKeys[1]) );
	}
	// �������ܻ����0
	ChengBenFenBuNodeMap::iterator it = mapNodes.find(0);
	if ( it == mapNodes.end() )
	{
		ASSERT( 0 );
		return false;
	}
	ChengBenFenBuNodeArray &aNodes = it->second;	// ����

	aNodes.clear();
	fPricePerPixel = (fPriceMax-fPriceMin)/uDivide;		// ÿ�ȷֵļ۸�
	aNodes.insert(aNodes.end(), uDivide, T_ChengBenFenBuCalcNode());	// ��ֵĬ��
	
	i = 0;
	for ( ChengBenFenBuNodeArray::iterator itArray=aNodes.begin(); itArray != aNodes.end() ; itArray++ )
	{
		itArray->iPixel = i;
		itArray->fPrice = fPriceMin + fPricePerPixel *i;
		i++;
	}

	// a)����ÿһ��ĳɱ��ֲ�ͼ���ǵ��ճɱ�������ͼ۵���߼ۼ��һ�����Ρ�	
	// b)���ÿһ��ĳɱ��ֲ�ͼ����Y=��1-A��*Y��+A*B��A��ʾ���ջ��֣�B��ʾ���ճɱ���Y��Y���ֱ��ʾ���պ���һ�յĳɱ��ֲ���ע�⣬�˴�BYY������ʾһ���ֲ������������һ����ֵ��
	int32 iIndex = 0;		// ~~
	if ( iKlineCount > 0 )
	{
		// ȡ��һ�����ݵ�, ��һ�����ݵ�Ӧ�ò�����ôȡ�ģ�������ȡ��
// 		while ( iIndex < iKlineCount )
// 		{
// 			const CKLine &kline = pKline[iIndex++];	// !!
// 
// 			const int32 iEndPos    = int32((kline.m_fPriceHigh - fPriceMin)/ fPricePerPixel);
// 			const int32 iStartPos = int32((kline.m_fPriceLow - fPriceMin)/fPricePerPixel);
// 			const float fVolPerPixel = kline.m_fVolume / (iEndPos-iStartPos+1);
// 			ASSERT( !_isnan(fVolPerPixel) && _finite(fVolPerPixel) && fVolPerPixel >= 0.0 );
// 			if ( _isnan(fVolPerPixel) || !_finite(fVolPerPixel) || fVolPerPixel < 0.0 )
// 			{
// 				continue;	// �Ƿ�ֵ ��������
// 			}
// 			//ASSERT( aNodes.size() == uDivide );
// 			for ( int32 iNode = iStartPos; iNode < aNodes.size() && iNode <= iEndPos ; iNode++ )
// 			{
// 				aNodes.at(iNode).fVol += fVolPerPixel;	// �����һ�μ����������ɽ���ƽ����ÿ����λ������
// 			}
// 			break;	// ֻȡ��һ����Чֵ
// 		}
	}

	
	
	// �������ݵ� Y = (1-A)Y' + B*A
	
	// �Ӿ�����������Զ�ĵ�����
	for ( vector<ChengBenFenBuNodeMap::key_type>::reverse_iterator itId = aSortKeys.rbegin(); itId != aSortKeys.rend() ; itId++ )
	{
		const int32 iCalcCount = iKlineCount - *itId;	// ����id��������������ڶ�������

		if ( iCalcCount < iIndex )
		{
			continue; // ���ܼ����������
		}

		// ƽ���㷨���������㷨
		// ������ ������� ���ߣ��ͼۼ���
		if ( ECBFB_SanJiao == eCBFBSuanFa )
		{
			for ( ; iIndex < iCalcCount ; iIndex++ )
			{
				const CKLine &kline = pKline[iIndex]; 
				
				float fRate = fDefaultTradeRate;
				if ( fLiuTongGuBen > 1.0 )
				{
					fRate = kline.m_fVolume / fLiuTongGuBen; // ������
				}
				
				fRate = fRate * fTradeRateRatio;	// ����˥��ϵ��
				
				//ASSERT( fRate < 1.0 && fRate > 0.0 );
				if ( fRate <= 0.0 )
				{
					fRate = 0.0001f;
					//continue;	// �������
				}
				else if ( fRate >= 1.0 )
				{
					fRate = 0.999f;		// �������󣿣�
				}
				
				// �������㷨
				//const int32 iCount    = int32((kline.m_fPriceHigh - kline.m_fPriceLow)/ fPricePerPixel) +1;
				const int32 iEndPos    = int32((kline.m_fPriceHigh - fPriceMin)/ fPricePerPixel);
				const int32 iStartPos = int32((kline.m_fPriceLow - fPriceMin)/fPricePerPixel);
				const float fVolPerPixel = kline.m_fVolume / (iEndPos-iStartPos+1);
				int32 iLow2AvgPos = int32((kline.m_fPriceAvg - fPriceMin)/ fPricePerPixel);	// �м��λ��
				if ( !(fVolPerPixel > 0.0) )
				{
					continue;	// �Ƿ�ֵ����
				}
				// ��avgΪƽ�ֵ㣬�ϰ벿�ְ���[1->n]�ۼӷ�̯1/2�ĳɽ��� �°벿��ͬ
				const int32 iLow2AvgCount = (iLow2AvgPos-iStartPos+1);
				const int32 iHigh2AvgCount = (iEndPos-iLow2AvgPos+1);
				const float fLowPerPixel = kline.m_fVolume / (iLow2AvgCount*(iLow2AvgCount+1));
				const float fHighPerPixel = kline.m_fVolume / (iHigh2AvgCount*(iHigh2AvgCount+1));

				//ASSERT( aNodes.size() == uDivide );
				int32 iNode;
				for ( iNode = 0; iNode < aNodes.size() ; iNode++ )	// ����ÿһ����λ����
				{
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// ��һ�μ���Ľ��
					// ��Щ������ֵ�ģ���Щ����û��ֵ��, �����е������ʷ˥������
					node.fVol  = (1.0-fRate)*node.fVol;	// �ǲ��������˼��
				}
				for ( iNode=iStartPos; iNode <= iLow2AvgPos && iNode < aNodes.size() ; iNode++ )
				{
					// �°벿��, ������
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// ��һ�μ���Ľ��
					node.fVol += /*fRate**/fLowPerPixel*(iNode-iStartPos+1);		// ���Ͻ���ֵ
				}
				for ( iNode=iLow2AvgPos; iNode<=iEndPos&&iNode<aNodes.size() ; iNode++ )
				{
					// �ϰ벿�֣�������
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// ��һ�μ���Ľ��
					node.fVol += /*fRate**/fHighPerPixel*(iEndPos-iNode+1);		// ���Ͻ���ֵ
				}
				
				// ����ͼ����
				// ��ǰ���Ѿ������˵����ڽ��м�ֵ����
				{
					// �˴�Ӧ�ÿ�����ֲ������ɺ�ƽ����TODO
					for ( vector<ChengBenFenBuNodeMap::key_type>::reverse_iterator itPreId = aSortKeys.rbegin(); itPreId != itId ; itPreId++ )
					{
						if ( iKlineCount - *itPreId <= 0 )
						{
							continue;	// ��Щ�ǲ��ܴ����
						}
						//ASSERT( *itPreId != it->first );
						
						ChengBenFenBuNodeMap::iterator itPre = mapNodes.find(*itPreId);
						//ASSERT( aNodes.size() == itPre->second.size() );

						for ( iNode = 0; iNode < itPre->second.size() ; iNode++ )	// ����ÿһ����λ����
						{
							T_ChengBenFenBuCalcNode &node = itPre->second.at(iNode);	// ��һ�μ���Ľ��
							// ��Щ������ֵ�ģ���Щ����û��ֵ��, �����е������ʷ˥������
							node.fVol  = (1.0-fRate)*node.fVol;	// �ǲ��������˼��
						}
						for ( iNode=iStartPos; iNode <= iLow2AvgPos && iNode < itPre->second.size() ; iNode++ )
						{
							// �°벿��, ������
							T_ChengBenFenBuCalcNode &node = itPre->second.at(iNode);	// ��һ�μ���Ľ��
							node.fVol += /*fRate**/fLowPerPixel*(iNode-iStartPos+1);		// ���Ͻ���ֵ
						}
						for ( iNode=iLow2AvgPos; iNode<=iEndPos&&iNode<itPre->second.size() ; iNode++ )
						{
							// �ϰ벿�֣�������
							T_ChengBenFenBuCalcNode &node = itPre->second.at(iNode);	// ��һ�μ���Ľ��
							node.fVol += /*fRate**/fHighPerPixel*(iEndPos-iNode+1);		// ���Ͻ���ֵ
						}
					}
				}
			}
		}
		else
		{
			for ( ; iIndex < iCalcCount ; iIndex++ )
			{
				const CKLine &kline = pKline[iIndex]; 
				
				float fRate = fDefaultTradeRate;
				if ( fLiuTongGuBen > 1.0 )
				{
					fRate = kline.m_fVolume / fLiuTongGuBen; // ������
				}
				
				fRate = fRate * fTradeRateRatio;	// ����˥��ϵ��
				
				//ASSERT( fRate < 1.0 && fRate > 0.0 );
				if ( fRate <= 0.0 )
				{
					fRate = 0.0001f;
					//continue;	// �������
				}
				else if ( fRate >= 1.0 )
				{
					fRate = 0.999f;		// �������󣿣�
				}
				
				int32 iEndPos    = int32((kline.m_fPriceHigh - fPriceMin)/ fPricePerPixel);
				int32 iNodeSize  = aNodes.size();
				iEndPos = min(iEndPos, iNodeSize);
				const int32 iStartPos = int32((kline.m_fPriceLow - fPriceMin)/fPricePerPixel);
				const float fVolPerPixel = kline.m_fVolume / (iEndPos-iStartPos+1);
				if ( !(fVolPerPixel > 0.0) )
				{
					continue;	// �Ƿ�ֵ����
				}
				
				//ASSERT( aNodes.size() == uDivide );
				int32 iNode;
				for ( iNode = 0; iNode < aNodes.size() ; iNode++ )	// ����ÿһ����λ����
				{
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// ��һ�μ���Ľ��
					// ��Щ������ֵ�ģ���Щ����û��ֵ��, �����е������ʷ˥������
					node.fVol  = (1.0-fRate)*node.fVol;	// �ǲ��������˼��
				}
				for ( iNode = iStartPos; iNode < aNodes.size() && iNode <= iEndPos ; iNode++ )	// ����ÿһ����λ����
				{
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// ��һ�μ���Ľ��
					// ��Щ������ֵ�ģ���Щ����û��ֵ��, ����ֵ�ĵ���н��ճɱ��ѻ�����
					node.fVol  += fVolPerPixel * fRate;	// �ǲ��������˼��
					ASSERT( node.fVol >= 0.0 );
				}
				
				// ����ͼ����
				// ��ǰ���Ѿ������˵����ڽ��м�ֵ����
				{
					// �˴�Ӧ�ÿ�����ֲ������ɺ�ƽ����TODO
					for ( vector<ChengBenFenBuNodeMap::key_type>::reverse_iterator itPreId = aSortKeys.rbegin(); itPreId != itId ; itPreId++ )
					{
						if ( iKlineCount - *itPreId <= 0 )
						{
							continue;	// ��Щ�ǲ��ܴ����
						}
						//ASSERT( *itPreId != it->first );
						
						ChengBenFenBuNodeMap::iterator itPre = mapNodes.find(*itPreId);
						//ASSERT( aNodes.size() == itPre->second.size() );
						
						for ( iNode = 0; iNode < itPre->second.size() ; iNode++ )	// ����ÿһ����λ����
						{
							T_ChengBenFenBuCalcNode &node = itPre->second.at(iNode);	// ��һ�μ���Ľ��
							// ��Щ������ֵ�ģ���Щ����û��ֵ��, �����е������ʷ˥������
							node.fVol  = (1.0-fRate)*node.fVol;	// �ǲ��������˼��
						}
						for ( iNode = iStartPos; iNode < itPre->second.size() && iNode < iEndPos ; iNode++ )	// ����ÿһ����λ����
						{
							T_ChengBenFenBuCalcNode &node = itPre->second.at(iNode);	// ��һ�μ���Ľ��
							// ��Щ������ֵ�ģ���Щ����û��ֵ��, ����ֵ�ĵ���н��ճɱ��ѻ�����
							node.fVol  += fVolPerPixel * fRate;	// �ǲ��������˼��
							ASSERT( node.fVol >= 0.0 );
						}
					}
				}
			}
		}

		if ( *itId != it->first )
		{
			//mapNodes[ *itId ] = aNodes;	// ������ʱ��״̬�����������Ǿ���n֮ǰ�Ľ��
			// ��ʱ�Ը�id�ĸ���0ֵ
			ChengBenFenBuNodeMap::iterator itPre = mapNodes.find(*itId);
			itPre->second = aNodes;
			for ( int32 iNode=0; iNode < itPre->second.size() ; iNode++ )
			{
				itPre->second.at(iNode).fVol = 0.0f;
			}
		}
	}

#ifdef _DEBUG
	dwTimeStart = timeGetTime() - dwTimeStart;
	TRACE(_T("�ɱ��ֲ�����(���̷���:%d[%d*%d]): %d ms\r\n"), eCBFBSuanFa, iKlineCount, aNodes.size(), dwTimeStart);
#endif
	

	return true;
}

bool32 CalcChengBenFenBu2( const CKLine *pKline, int32 iKlineCount, float fLiuTongGuBenOrg, const ChengBenFenBuCycleArray &aCycles, const T_ChengBenFenBuCalcParam *pCalcParam, OUT T_ChengBenFenBuResult &ResultNode )
{
#ifdef _DEBUG
	DWORD dwTimeStart = timeGetTime();
#endif
	ResultNode.aResultNodes.clear(); // ��ս��
	ResultNode.aPreVolSums.clear();

	if ( NULL == pKline )
	{
		ASSERT( 0 );
		return false;
	}

	float fPricePerPixel = 0.0f;
	E_ChengBenFenBuSuanFa eCBFBSuanFa = ECBFB_PingJun;	// ��ʱ������
	float fTradeRateRatio = 1.0f;
	float fDefaultTradeRate = 0.0f;
	UINT uDivide = CBFB_DEFAULT_DIVIDE;
	if ( pCalcParam != NULL )
	{
		eCBFBSuanFa = pCalcParam->eCBFBSuanFa;
		fTradeRateRatio = pCalcParam->fTradeRateRatio;
		fDefaultTradeRate = pCalcParam->fDefaultTradeRate;
		uDivide = pCalcParam->m_uDivide;

		if ( eCBFBSuanFa != ECBFB_PingJun && eCBFBSuanFa != ECBFB_SanJiao )
		{
			ASSERT( 0 );
			return false;
		}

		if ( fTradeRateRatio < CBFB_MIN_TRADERATERADIO || fTradeRateRatio > CBFB_MAX_TRADERATERADIO )
		{
			ASSERT( 0 );
			return false;
		}

		if ( uDivide < CBFB_MIN_DIVIDE || uDivide > CBFB_MAX_DIVIDE )
		{
			ASSERT( 0 );
			return false;
		}
	}

	int32 i = 0;
	float fPriceMin, fPriceMax;
	fPriceMin = FLT_MAX;
	fPriceMax = -FLT_MAX;
	float fLiuTongGuBen = fLiuTongGuBenOrg;
	if ( fLiuTongGuBen <= 1.0 && (fDefaultTradeRate <= 0.0 || fDefaultTradeRate >= 1.0) )
	{
		fLiuTongGuBen = 1e-6;
		for ( i=0; i < iKlineCount ; i++ )
		{
			const CKLine &kline = pKline[i];
			fLiuTongGuBen += kline.m_fVolume;	// ���гɽ���֮��
			
			if ( kline.m_fPriceHigh > fPriceMax )
			{
				fPriceMax = kline.m_fPriceHigh;
			}
			if ( kline.m_fPriceLow < fPriceMin )
			{
				fPriceMin = kline.m_fPriceLow;
			}
		}
	}
	else 
	{
		for ( i=0; i < iKlineCount ; i++ )
		{
			const CKLine &kline = pKline[i];
			
			if ( kline.m_fPriceHigh > fPriceMax )
			{
				fPriceMax = kline.m_fPriceHigh;
			}
			if ( kline.m_fPriceLow < fPriceMin )
			{
				fPriceMin = kline.m_fPriceLow;
			}
		}
	}
	if ( fLiuTongGuBen <= 1.0 )
	{
		fLiuTongGuBen = 1e-6;
	}

	if ( (fLiuTongGuBen <= 1.0 && (fDefaultTradeRate <= 0.0 || fDefaultTradeRate >= 1.0)) )
	{
		return false;	// �޷�������ȥ
	}


	
	// ����������Ҫ��
	// �����Ľڵ㶼��xx��֮ǰ��
	ChengBenFenBuCycleArray aSortKeys;
	{
		ChengBenFenBuCycleArray aTmp = aCycles;
		aTmp.push_back(0);	// �ܼ���һ��0
		sort(aTmp.begin(), aTmp.end());
		// ����ظ�����
		for ( i=0; i < aTmp.size()-1 ; i++ )
		{
			if ( aTmp.at(i) != aTmp.at(i+1) )
			{
				aSortKeys.push_back(aTmp.at(i));
			}
		}
		aSortKeys.push_back(aTmp.back());

		for ( i = 0; i < aSortKeys.size() ; i++  )
		{
			T_ChengBenFenBuResult::T_PreVolSum volSum;
			volSum.fPreVolSum = 0.0f;
			volSum.iCycle = aSortKeys.at(i);
			ResultNode.aPreVolSums.push_back(volSum);
		}
		
		ASSERT( aSortKeys[0]==0 && (aSortKeys.size() == 1 || aSortKeys[0] < aSortKeys[1]) );
	}
	// �������ܻ����0
	ChengBenFenBuNodeArray &aNodes = ResultNode.aResultNodes;	// ����

	aNodes.clear();
	fPricePerPixel = (fPriceMax-fPriceMin)/uDivide;		// ÿ�ȷֵļ۸�
	aNodes.insert(aNodes.end(), uDivide, T_ChengBenFenBuCalcNode());	// ��ֵĬ��
	
	i = 0;
	for ( ChengBenFenBuNodeArray::iterator itArray=aNodes.begin(); itArray != aNodes.end() ; itArray++ )
	{
		itArray->iPixel = i;
		itArray->fPrice = fPriceMin + fPricePerPixel *i;
		i++;
	}

	// a)����ÿһ��ĳɱ��ֲ�ͼ���ǵ��ճɱ�������ͼ۵���߼ۼ��һ�����Ρ�	
	// b)���ÿһ��ĳɱ��ֲ�ͼ����Y=��1-A��*Y��+A*B��A��ʾ���ջ��֣�B��ʾ���ճɱ���Y��Y���ֱ��ʾ���պ���һ�յĳɱ��ֲ���ע�⣬�˴�BYY������ʾһ���ֲ������������һ����ֵ��
	int32 iIndex = 0;		// ~~
	if ( iKlineCount > 0 )
	{
		// ȡ��һ�����ݵ�, ������һ������㷨, Ӧ���������㷨�����
// 		while ( iIndex < iKlineCount )
// 		{
// 			const CKLine &kline = pKline[iIndex++];	// !!
// 
// 			const int32 iEndPos    = int32((kline.m_fPriceHigh - fPriceMin)/ fPricePerPixel);
// 			const int32 iStartPos = int32((kline.m_fPriceLow - fPriceMin)/fPricePerPixel);
// 			const float fVolPerPixel = kline.m_fVolume / (iEndPos-iStartPos+1);
// 			ASSERT( !_isnan(fVolPerPixel) && _finite(fVolPerPixel) && fVolPerPixel >= 0.0 );
// 			if ( _isnan(fVolPerPixel) || !_finite(fVolPerPixel) || fVolPerPixel < 0.0 )
// 			{
// 				continue;	// �Ƿ�ֵ ��������
// 			}
// 			//ASSERT( aNodes.size() == uDivide );
// 			for ( int32 iNode = iStartPos; iNode < aNodes.size() && iNode <= iEndPos ; iNode++ )
// 			{
// 				aNodes.at(iNode).fVol += fVolPerPixel;	// �����һ�μ����������ɽ���ƽ����ÿ����λ������
// 			}
// 			break;	// ֻȡ��һ����Чֵ
// 		}
	}

	
	
	// �������ݵ� Y = (1-A)Y' + B*A
	
	// �Ӿ�����������Զ�ĵ�����
	for ( vector<ChengBenFenBuNodeMap::key_type>::reverse_iterator itId = aSortKeys.rbegin(); itId != aSortKeys.rend() ; itId++ )
	{
		const int32 iCalcCount = iKlineCount - *itId;	// ����id��������������ڶ�������

		if ( iCalcCount < iIndex )
		{
			continue; // ���ܼ����������
		}

		if ( ECBFB_SanJiao == eCBFBSuanFa )
		{
			for ( ; iIndex < iCalcCount ; iIndex++ )
			{
				const CKLine &kline = pKline[iIndex]; 
				
				float fRate = fDefaultTradeRate;
				if ( fLiuTongGuBen > 1.0 )
				{
					fRate = kline.m_fVolume / fLiuTongGuBen; // ������
				}
				
				fRate = fRate * fTradeRateRatio;	// ����˥��ϵ��
				
				ASSERT( fRate < 1.0 && fRate > 0.0 );
				if ( fRate <= 0.0 )
				{
					fRate = 0.0001f;
					//continue;	// �������
				}
				else if ( fRate >= 1.0 )
				{
					fRate = 0.999f;		// �������󣿣�
				}
				
				// �������㷨
				//const int32 iCount    = int32((kline.m_fPriceHigh - kline.m_fPriceLow)/ fPricePerPixel) +1;
				const int32 iEndPos    = int32((kline.m_fPriceHigh - fPriceMin)/ fPricePerPixel);
				const int32 iStartPos = int32((kline.m_fPriceLow - fPriceMin)/fPricePerPixel);
				const float fVolPerPixel = kline.m_fVolume / (iEndPos-iStartPos+1);
				int32 iLow2AvgPos = int32((kline.m_fPriceAvg - fPriceMin)/ fPricePerPixel);	// �м��λ��
				if ( !(fVolPerPixel > 0.0) )
				{
					continue;	// �Ƿ�ֵ����
				}
				// ��avgΪƽ�ֵ㣬�ϰ벿�ְ���[1->n]�ۼӷ�̯1/2�ĳɽ��� �°벿��ͬ
				const int32 iLow2AvgCount = (iLow2AvgPos-iStartPos+1);
				const int32 iHigh2AvgCount = (iEndPos-iLow2AvgPos+1);
				const float fLowPerPixel = kline.m_fVolume / (iLow2AvgCount*(iLow2AvgCount+1));
				const float fHighPerPixel = kline.m_fVolume / (iHigh2AvgCount*(iHigh2AvgCount+1));
				
				//ASSERT( aNodes.size() == uDivide );
				int32 iNode;
				for ( iNode = 0; iNode < aNodes.size() ; iNode++ )	// ����ÿһ����λ����
				{
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// ��һ�μ���Ľ��
					// ��Щ������ֵ�ģ���Щ����û��ֵ��, �����е������ʷ˥������
					node.fVol  = (1.0-fRate)*node.fVol;	// �ǲ��������˼��
				}
				for ( iNode=iStartPos; iNode <= iLow2AvgPos && iNode < aNodes.size() ; iNode++ )
				{
					// �°벿��, ������
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// ��һ�μ���Ľ��
					node.fVol += fLowPerPixel*(iNode-iStartPos+1);		// ���Ͻ���ֵ
				}
				for ( iNode=iLow2AvgPos; iNode<=iEndPos&&iNode<aNodes.size() ; iNode++ )
				{
					// �ϰ벿�֣�������
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// ��һ�μ���Ľ��
					node.fVol += fHighPerPixel*(iEndPos-iLow2AvgPos+1);		// ���Ͻ���ֵ
				}
				
				// ��ǰ���Ѿ������˵����ڵĳɱ�ͳ�ƽ�����ʷ˥��
				{
					for ( vector<T_ChengBenFenBuResult::T_PreVolSum>::reverse_iterator itPre = ResultNode.aPreVolSums.rbegin();
					itPre != ResultNode.aPreVolSums.rend() && itPre->iCycle > *itId ;
					itPre++ )
					{
						if ( iKlineCount - itPre->iCycle <= 0 )
						{
							continue;	// ��Щ�ǲ��ܴ����
						}
						
						itPre->fPreVolSum = itPre->fPreVolSum * (1-fRate);
					}
				}
			}
		}
		else
		{
			for ( ; iIndex < iCalcCount ; iIndex++ )
			{
				const CKLine &kline = pKline[iIndex]; 
				
				float fRate = fDefaultTradeRate;
				if ( fLiuTongGuBen > 1.0 )
				{
					fRate = kline.m_fVolume / fLiuTongGuBen; // ������
				}
				
				fRate = fRate * fTradeRateRatio;	// ����˥��ϵ��
				
				ASSERT( fRate < 1.0 && fRate > 0.0 );
				if ( fRate <= 0.0 )
				{
					fRate = 0.0001f;
					//continue;	// �������
				}
				else if ( fRate >= 1.0 )
				{
					fRate = 0.999f;		// �������󣿣�
				}
				
				const int32 iEndPos    = int32((kline.m_fPriceHigh - fPriceMin)/ fPricePerPixel);
				const int32 iStartPos = int32((kline.m_fPriceLow - fPriceMin)/fPricePerPixel);
				const float fVolPerPixel = kline.m_fVolume / (iEndPos-iStartPos+1);
				if ( !(fVolPerPixel > 0.0) )
				{
					continue;	// �Ƿ�ֵ����
				}
				
				int32 iNode;
				for ( iNode = 0; iNode < aNodes.size() ; iNode++ )	// ����ÿһ����λ����
				{
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// ��һ�μ���Ľ��
					// ��Щ������ֵ�ģ���Щ����û��ֵ��, �����е������ʷ˥������
					node.fVol  = (1.0-fRate)*node.fVol;	// �ǲ��������˼��
				}
				for ( iNode = iStartPos; iNode < aNodes.size() && iNode <= iEndPos ; iNode++ )	// ����ÿһ����λ����
				{
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// ��һ�μ���Ľ��
					// ��Щ������ֵ�ģ���Щ����û��ֵ��, ����ֵ�ĵ���н��ճɱ��ѻ�����
					node.fVol  += fVolPerPixel /** fRate*/;	// �ǲ��������˼��
					ASSERT( node.fVol >= 0.0 );
				}
				
				// ��ǰ���Ѿ������˵����ڵĳɱ�ͳ�ƽ�����ʷ˥��
				{
					for ( vector<T_ChengBenFenBuResult::T_PreVolSum>::reverse_iterator itPre = ResultNode.aPreVolSums.rbegin();
					itPre != ResultNode.aPreVolSums.rend() && itPre->iCycle > *itId ;
					itPre++ )
					{
						if ( iKlineCount - itPre->iCycle <= 0 )
						{
							continue;	// ��Щ�ǲ��ܴ����
						}
						
						itPre->fPreVolSum = itPre->fPreVolSum * (1-fRate);
					}
				}
			}
		}

		// �Դ�ʱ��������Ľ�����гɱ��ۼ�
		T_ChengBenFenBuResult::T_PreVolSum &volSum = *(ResultNode.aPreVolSums.rbegin() + (itId-aSortKeys.rbegin()));
		ASSERT( volSum.fPreVolSum == 0.0f && volSum.iCycle == *itId );
		for ( int32 iNode = 0; iNode < aNodes.size() ; iNode++ )	// ����ÿһ����λ����
		{
			volSum.fPreVolSum += aNodes.at(iNode).fVol;
		}
	}

#ifdef _DEBUG
	dwTimeStart = timeGetTime() - dwTimeStart;
	TRACE(_T("�ɱ��ֲ�����(����:%d): %d ms\r\n"), eCBFBSuanFa, dwTimeStart);
#endif
	

	return true;
}

