// formulaengine.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <io.h>
#include <float.h>
#include <mmsystem.h>
#include "ShareFun.h"
#include "coding.h"
#include "MerchManager.h"
#include "StructKlineView.h"
#include "CFormularContent.h"
#include "CFormularCompute.h"
#include "SelectStockStruct.h"
#include "formulaengine.h"
//#include "FormularEnum.h"
//#include "IoViewChart.h"
//#include "IoViewShare.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CString StrUsesEnumMsg;

void GetParams(IN char* StrParam, OUT float& fValue1, OUT float& fValue2, OUT float& fValue3, OUT float& fValue4)
{
	fValue1 = 0.f;
	fValue2 = 0.f;
	fValue3 = 0.f;
	fValue4 = 0.f;

	if ( NULL == StrParam || strlen(StrParam) <= 0 )
	{
		return;
	}
	
	char acTmp[200];
	memset(acTmp, 0, sizeof(char)*200);
	
	char* pStrCur = StrParam;					
	char* pNext = strchr(StrParam, ',');
	
	if ( NULL == pNext )
	{
		return;
	}

	int iLen = pNext - pStrCur;
	strncpy(acTmp, pStrCur, iLen);
	acTmp[iLen] = '\0';
	pNext += 1;
	
	fValue1 = atof(acTmp);
	
	//
	pStrCur = pNext;
	memset(acTmp, 0, sizeof(char)*200);
	
	pNext = strchr(pStrCur, ',');
	
	if ( NULL == pNext )
	{
		return;
	}

	iLen = pNext - pStrCur;
	strncpy(acTmp, pStrCur, iLen);
	acTmp[iLen] = '\0';
	pNext += 1;
	
	fValue2 = atof(acTmp);
	
	//
	pStrCur = pNext;
	memset(acTmp, 0, sizeof(char)*200);
	
	pNext = strchr(pStrCur, ',');
	
	if ( NULL == pNext )
	{
		return;
	}

	iLen = pNext - pStrCur;
	strncpy(acTmp, pStrCur, iLen);
	pNext += 1;
	acTmp[iLen] = '\0';
	fValue3 = atof(acTmp);
	
	//
	if ( NULL == pNext )
	{
		return;
	}

	fValue4 = atof(pNext);
}

int ComputeFormu2(CFormularCompute*& pEqution,INPUT_PARAM& input)// need to delete pEqution outside
{
	ASSERT(pEqution == NULL);
	pEqution = new CFormularCompute(0,input.pmax,input.pp,input.pIndex);
	pEqution->SetMerchNodeUserData(input.pExtraInfo/*input.pUserData*/);
	int numPam=input.pIndex->numPara;
	if(numPam > PARAM_NUM)					
		numPam = PARAM_NUM;				
	if(numPam<0)
		numPam = 0;
	for(int j=0;j<numPam;j++)
	{
		input.pIndex->namePara[j].MakeLower();
		pEqution->AddPara (input.pIndex->namePara[j],
			CFormularContent::GetParamDataEach(j,input.pIndex));
	}

	if (NULL != input.pExtraInfo /*input.pUserData*/)
	{
		int iBegin = input.pExtraInfo->iShowPosInFullList; /*input.pUserData->m_iShowPosInFullList;*/
		int iEnd   = input.pExtraInfo->iShowPosInFullList /*input.pUserData->m_iShowPosInFullList*/ + input.pExtraInfo->iShowCountInFullList /*input.pUserData->m_iShowCountInFullList*/ - 1;
		pEqution->SetScreenBeginEnd(iBegin, iEnd);
	}

	return pEqution->FormularCompute();
}

T_IndexOutArray* formula_index( CFormularContent* pContent, CArray<CKLine,CKLine>& KLines, MERCH_EXTRA_INFO *pExtraInfo/*T_MerchNodeUserData *pUserData=NULL*/)
{
	//DWORD dwTime = timeGetTime();
	int i,j;
	int klineNum = KLines.GetSize();
	if ( klineNum < 1 ) return NULL;
	Kline* pkline = new Kline[klineNum];
	for ( i = 0; i < klineNum; i ++ )
	{
		CKLine KLine	   = KLines.GetAt(i);
		//pkline[i].day	   = KLine.m_TimeCurrent.GetTime();
		pkline[i].open	   = KLine.m_fPriceOpen;
		pkline[i].close	   = KLine.m_fPriceClose;
		pkline[i].high	   = KLine.m_fPriceHigh;
		pkline[i].low	   = KLine.m_fPriceLow;
		pkline[i].amount   = KLine.m_fAmount;
		pkline[i].vol	   = KLine.m_fVolume;
		pkline[i].position = KLine.m_fHold;

		//  fangz 0925# BTIָ�� ָ���������µ���������,��ֵ����������
		pkline[i].advance = KLine.m_usRiseMerchCount;
		pkline[i].decline = KLine.m_usFallMerchCount;
	}

	INPUT_PARAM input;
	input.pmax			= klineNum-1;
	input.pp			= pkline;
	input.pIndex		= pContent;
	input.pExtraInfo	= pExtraInfo;
	//input.pUserData		= pUserData;

	//lint --e{438}
	T_IndexOutArray* pOutParam = new T_IndexOutArray;
	CFormularCompute* pCompute = NULL;

	//TRACE(_T("  ָ�����ComputeFormu2ǰ: %s %dms\r\n"), input.pIndex->name, timeGetTime()-dwTime);
	//dwTime = timeGetTime();
	if( ComputeFormu2(pCompute, input) == 0 )
	{
		//TRACE(_T("  ָ�����ComputeFormu2����: %s %dms\r\n"), input.pIndex->name, timeGetTime()-dwTime);
		//dwTime = timeGetTime();

		int numLine = pCompute->GetRlineNum ();
		pOutParam->iIndexNum = numLine;
		for( i=0; i< numLine; i++)
		{
			
			CString namePer;
			ARRAY_BE arr;

			arr.line = new float[klineNum];
			memset(arr.line,0,sizeof(float)*klineNum);

			int iRet = pCompute->GetLine(i,arr,namePer);
			if ( 1 == iRet || arr.b > arr.e )
			{
				//_LogFull(_T("ָ�깫ʽ����ʧ��?"));// zhangtao
				pOutParam->index[i].iPointNum = klineNum;
				pOutParam->index[i].pPoint = new float[klineNum];

				for ( j = 0; j < klineNum; j ++ )
				{
					pOutParam->index[i].pPoint[j] = 0.0f;
				}

				pOutParam->index[i].StrName = namePer;
				pOutParam->index[i].iThick = pCompute->m_RlineThick[i];
				pOutParam->index[i].iType = pCompute->m_RlineType[i];
				pOutParam->index[i].iColor = pCompute->m_RlineColor[i];
			}
			else
			{
				//lint --e{438}
				float fb = 0.0f;
				float fe = 0.0f;
				if (arr.b < klineNum && arr.e-1 < klineNum)
				{
					fb = arr.line[arr.b];
					fe = arr.line[arr.e-1];
				}
				pOutParam->index[i].iPointNum = klineNum;
				pOutParam->index[i].pPoint = new float[klineNum];

				pOutParam->index[i].iThick  = pCompute->m_RlineThick[i];
				pOutParam->index[i].StrName = namePer;

				if ( ARRAY_BE::Normal == arr.kind )
				{
					pOutParam->index[i].iType   = pCompute->m_RlineType[i];	
					pOutParam->index[i].iTypeEx = pCompute->m_RlineTypeEx[i];	

					for ( j = 0; j < klineNum; j ++ )
					{
						if ( j < arr.b )
						{
							pOutParam->index[i].pPoint[j] = FLT_MAX;
						}
						else if( j > arr.e )
						{
							pOutParam->index[i].pPoint[j] = FLT_MAX;
						}
						else
						{
							pOutParam->index[i].pPoint[j] = arr.line[j];
						}
					}
				}
				else
				{
					if ( ARRAY_BE::DrawLine == arr.kind )
					{
						pOutParam->index[i].iType = 9/*CIndexDrawer::EIDSDrawLine*/;
						pOutParam->index[i].iTypeEx = pCompute->m_RlineTypeEx[i];	

						for ( j = 0; j < klineNum; j++ )
						{							
							pOutParam->index[i].pPoint[j] = FLT_MAX;

							for ( int32 k = 0; k < arr.looseArr.GetSize(); k++ )
							{
								// arr.looseArr �����˻��Ƶ������λ��: nFoot: ���x���� fVal: ���y����
								// ���������ڵ���ȷ��ֵ, �����ڵ㶼��ֵΪ FLT_MAX �Ƿ�ֵ
								if ( j == arr.looseArr.GetAt(k).nFoot )
								{
									pOutParam->index[i].pPoint[j] = arr.looseArr.GetAt(k).fVal;
									break;
								}
							}						
						}
					}
					else if ( ARRAY_BE::DrawPolyLine == arr.kind )
					{
						pOutParam->index[i].iType = 10/*CIndexDrawer::EIDSPolyLine*/;
						pOutParam->index[i].iTypeEx = pCompute->m_RlineTypeEx[i];	
					}
					else if ( ARRAY_BE::DrawBand == arr.kind )
					{
						pOutParam->index[i].iTypeEx = pCompute->m_RlineTypeEx[i];	
						pOutParam->index[i].pPointExtra1 = new float[klineNum];
						memset(pOutParam->index[i].pPointExtra1, 0, klineNum * sizeof(float));

						//
						pOutParam->index[i].iType = 20 /*CIndexDrawer::EIDSDrawBand*/;
						pOutParam->index[i].StrExtraData = arr.s;	

						//
						for ( j = 0; j < klineNum; j++ )
						{
							pOutParam->index[i].pPoint[j]		= arr.line[j];
							pOutParam->index[i].pPointExtra1[j]	= arr.looseArr[j].fVal;							
						}
					}
					else if ( ARRAY_BE::DrawStickLine == arr.kind )
					{
						// ������Ҫ�õ� pPointExtra1
						//DWORD dwTime = timeGetTime();

						pOutParam->index[i].iTypeEx = pCompute->m_RlineTypeEx[i];	
						pOutParam->index[i].pPointExtra1 = new float[klineNum];
						memset(pOutParam->index[i].pPointExtra1, 0, klineNum * sizeof(float));

						//
						pOutParam->index[i].iType = 11/*CIndexDrawer::EIDSStickLine*/;
						pOutParam->index[i].StrExtraData = arr.s;	

						//
						// ����ڵ���Ǵӿ�ʼʱ�䵽����ʱ������
						int32 k = 0;
						for ( j = 0; j < klineNum; j++ )
						{
							pOutParam->index[i].pPoint[j]		= FLT_MAX;
							pOutParam->index[i].pPointExtra1[j]	= FLT_MAX;

							//
							// 							for ( int32 k=0; k < arr.looseArr.GetSize(); k++ )
							// 							{
							// 								// arr.looseArr �����˻������ֵ�λ��: nFoot: �ĸ�K ��λ��(x����) fVal: �۸�(y����)
							// 								// ���˸�������ֵĽڵ���ȷ��ֵ, �����ڵ㶼��ֵΪ FLT_MAX �Ƿ�ֵ
							// 								if ( j == arr.looseArr[k].nFoot )
							// 								{
							// 									pOutParam->index[i].pPoint[j]		= arr.looseArr[k].fVal;
							// 									pOutParam->index[i].pPointExtra1[j]	= arr.line[j];
							// 									break;
							// 								}
							// 							}
							// �������У���Ȼ��һ����ʼ��ֵ�������
							if ( k < arr.looseArr.GetSize() && j == arr.looseArr[k].nFoot )
							{
								pOutParam->index[i].pPoint[j]		= arr.looseArr[k].fVal;
								pOutParam->index[i].pPointExtra1[j]	= arr.line[j];
								k++;	// ��ֵ�Ѿ�ȡ����
							}
						}

						//TRACE(_T("     stickline: %d\r\n"), timeGetTime()-dwTime);
					}
					else if ( ARRAY_BE::DrawIcon == arr.kind )
					{
						pOutParam->index[i].iType = 12/*CIndexDrawer::EIDSDrawIcon*/;
						pOutParam->index[i].iTypeEx = pCompute->m_RlineTypeEx[i];	
						pOutParam->index[i].StrExtraData = arr.s;

						for ( j = 0; j < klineNum; j++ )
						{							
							pOutParam->index[i].pPoint[j] = FLT_MAX;

							for ( int32 k = 0; k < arr.looseArr.GetSize(); k++ )
							{
								// arr.looseArr �����˻������ֵ�λ��: nFoot: �ĸ�K ��λ��(x����) fVal: �۸�(y����)
								// ���˸�������ֵĽڵ���ȷ��ֵ, �����ڵ㶼��ֵΪ FLT_MAX �Ƿ�ֵ
								if ( j == arr.looseArr.GetAt(k).nFoot )
								{
									pOutParam->index[i].pPoint[j] = arr.looseArr.GetAt(k).fVal;
									break;
								}
							}						
						}
					}				
					else if ( ARRAY_BE::StringData == arr.kind )
					{
						// ������
						pOutParam->index[i].iType   = 13/*CIndexDrawer::EIDSDrawText*/;		
						pOutParam->index[i].iTypeEx = pCompute->m_RlineTypeEx[i];	
						pOutParam->index[i].StrExtraData = arr.s;

						for ( j = 0; j < klineNum; j++ )
						{							
							pOutParam->index[i].pPoint[j] = FLT_MAX;

							for ( int32 k = 0; k < arr.looseArr.GetSize(); k++ )
							{
								// arr.looseArr �����˻������ֵ�λ��: nFoot: �ĸ�K ��λ��(x����) fVal: �۸�(y����)
								// ���˸�������ֵĽڵ���ȷ��ֵ, �����ڵ㶼��ֵΪ FLT_MAX �Ƿ�ֵ
								if ( j == arr.looseArr.GetAt(k).nFoot )
								{
									pOutParam->index[i].pPoint[j] = arr.looseArr.GetAt(k).fVal;
									break;
								}
							}						
						}
					}			
					else if ( ARRAY_BE::StringDataEx == arr.kind )
					{
						// ������
						pOutParam->index[i].iType   = 21/*CIndexDrawer::EIDSDrawTextEx*/;		
						pOutParam->index[i].iTypeEx = pCompute->m_RlineTypeEx[i];	

						//
						for ( j = 0; j < klineNum; j++ )
						{							
							pOutParam->index[i].pPoint[j] = FLT_MAX;
							CString StrValue = L"";

							for ( int32 k = 0; k < arr.looseArr.GetSize(); k++ )
							{
								// arr.looseArr �����˻������ֵ�λ��: nFoot: �ĸ�K ��λ��(x����) fVal: �۸�(y����)
								// ���˸�������ֵĽڵ���ȷ��ֵ, �����ڵ㶼��ֵΪ FLT_MAX �Ƿ�ֵ
								if ( j == arr.looseArr.GetAt(k).nFoot )
								{
									pOutParam->index[i].pPoint[j] = arr.looseArr.GetAt(k).fVal;		
									StrValue = arr.looseArr.GetAt(k).StrExtraData;
									break;
								}
							}	

							//
							pOutParam->index[i].aStrExtraData.Add(StrValue);
						}
					}			
					else if ( ARRAY_BE::DrawKLine == arr.kind )
					{
						//DWORD dwTime = timeGetTime();

						pOutParam->index[i].iTypeEx = pCompute->m_RlineTypeEx[i];	

						// �����߶�Ҫ��ֵ, ���ո�, ��, ��, �յ�˳��
						pOutParam->index[i].pPointExtra1 = new float[klineNum];
						pOutParam->index[i].pPointExtra2 = new float[klineNum];
						pOutParam->index[i].pPointExtra3 = new float[klineNum];

						memset(pOutParam->index[i].pPointExtra1, 0, klineNum * sizeof(float));
						memset(pOutParam->index[i].pPointExtra2, 0, klineNum * sizeof(float));
						memset(pOutParam->index[i].pPointExtra3, 0, klineNum * sizeof(float));

						// ���� DrawKLine
						pOutParam->index[i].iType   = 14 /*CIndexDrawer::EIDSDrawKLine*/;
						pOutParam->index[i].iKlineDrawType = arr.iKlineDrawType;	// k�߻�������

						if ( klineNum != arr.looseArr.GetSize() )
						{
							////ASSERT(0);

							delete[] pkline;
							delete pCompute;							
							DEL(pOutParam);

							return NULL;
						}

						for ( j = 0; j < klineNum; j++ )
						{
							CString StrParam = arr.looseArr.GetAt(j).StrExtraData;							

							if ( StrParam.GetLength() <= 0 )
							{
								////ASSERT(0);

								pOutParam->index[i].pPoint[j]		= 0.f;
								pOutParam->index[i].pPointExtra1[j]	= 0.f;
								pOutParam->index[i].pPointExtra2[j]	= 0.f;
								pOutParam->index[i].pPointExtra3[j]	= 0.f;

								continue;
							}

							std::string StrParamA;
							Unicode2MultiChar(CP_ACP, StrParam, StrParamA);	
							

							float fValue1 = 0.f;
							float fValue2 = 0.f;
							float fValue3 = 0.f;							
							float fValue4 = 0.f;

							//
							GetParams((char*)StrParamA.c_str(), fValue1, fValue2, fValue3, fValue4);

							pOutParam->index[i].pPoint[j]		= fValue1;
							pOutParam->index[i].pPointExtra1[j]	= fValue2;
							pOutParam->index[i].pPointExtra2[j]	= fValue3;
							pOutParam->index[i].pPointExtra3[j]	= fValue4;
						}

						//TRACE(_T("     drawkline: %d\r\n"), timeGetTime()-dwTime);
					}
					else if (ARRAY_BE::DrawNum == arr.kind)
					{
						pOutParam->index[i].iType   = 15/*CIndexDrawer::EIDSDrawNum*/;		
						pOutParam->index[i].iTypeEx = pCompute->m_RlineTypeEx[i];	
						pOutParam->index[i].StrExtraData = arr.s;

						//
						for ( j = 0; j < klineNum; j++ )
						{			
							CString StrNumText = L"";
							pOutParam->index[i].pPoint[j] = FLT_MAX;

							//
							for ( int32 k = 0; k < arr.looseArr.GetSize(); k++ )
							{
								// arr.looseArr �����˻������ֵ�λ��: nFoot: �ĸ�K ��λ��(x����) fVal: �۸�(y����)
								// ���˸�������ֵĽڵ���ȷ��ֵ, �����ڵ㶼��ֵΪ FLT_MAX �Ƿ�ֵ
								if ( j == arr.looseArr.GetAt(k).nFoot )
								{
									pOutParam->index[i].pPoint[j] = arr.looseArr.GetAt(k).fVal;
									StrNumText = arr.looseArr.GetAt(k).StrExtraData;

									//
									break;
								}
							}					

							//
							pOutParam->index[i].aStrExtraData.Add(StrNumText);
						}
					}
				}

				pOutParam->index[i].iColor  = pCompute->m_RlineColor[i];
			}

			// ����
			pOutParam->m_aMapNameToIndex[namePer] = i;

			DEL_ARRAY(arr.line);

			//TRACE(_T("   �ߣ�%s %dms\r\n"), namePer, timeGetTime()-dwTime);
		}
	}
	else
	{
		delete[] pkline;
		delete pCompute;		
		DEL(pOutParam);

		return NULL;
	}

	delete[] pkline;

	//TRACE(_T("  ָ�����ComputeFormu2�������,��ʼ�����Դ: %s %dms\r\n"), input.pIndex->name, timeGetTime()-dwTime);
	//dwTime = timeGetTime();

	delete pCompute;

	//TRACE(_T("  ָ�����ComputeFormu2�������-����: %s %dms\r\n"), input.pIndex->name, timeGetTime()-dwTime);
	return pOutParam;
}

