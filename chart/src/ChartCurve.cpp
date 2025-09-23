#include "stdafx.h"

#include <math.h>
#include <MMSystem.h>
#include "ChartRegion.h"
#include "ShareFun.h"
#include "IndexDrawer.h"
#include "SaneIndex.h"

#include "XLTimerTrace.h"

#include "ChartCurve.h"

/////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
CNodeSequence::CNodeSequence( )
{
	m_aNodes.SetSize(0, 100);
	m_pUserData		= NULL;
	m_StrWeightName = _T("");
}

//lint --e{1579}
CNodeSequence::~CNodeSequence( )
{
	RemoveAll();
}

int32 CNodeSequence::AddRef(CDrawingCurve* pCurve)
{
	m_aAttatchedCurves.Add(pCurve);
	return m_aAttatchedCurves.GetSize();
}

int32 CNodeSequence::Release(CDrawingCurve* pCurve)
{
	int32 i, iSize = m_aAttatchedCurves.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		if ( m_aAttatchedCurves.GetAt(i) == pCurve )
		{
			break;
		}
	}
	ASSERT(i<iSize);
	m_aAttatchedCurves.RemoveAt(i);
	return m_aAttatchedCurves.GetSize();
}


bool32 CNodeSequence::Add(const CNodeData& NodeData)
{
// 	int32 iPos = m_aNodes.GetSize() + 1;//����Ϊ0,������1��ʼ
// 	if ( !m_MapIndex.Add(NodeData.m_iID,(void*)iPos) )
// 	{
// 		////ASSERT(0);
// 		return false;
// 	}

	m_aNodes.SetSize(m_aNodes.GetSize() + 1, 100);
	CNodeData *pNodes = (CNodeData *)m_aNodes.GetData();
	memcpyex(pNodes + m_aNodes.GetSize() - 1, &NodeData, sizeof(CNodeData));
	return true;
}

bool32 CNodeSequence::GetAt( int32 iPos,CNodeData& NodeData)
{
	int32 iSize = m_aNodes.GetSize();
	if ( iPos < 0 || iPos >= iSize ) return false;
	
	CNodeData *pNodes = (CNodeData *)m_aNodes.GetData();
	memcpyex(&NodeData, pNodes + iPos, sizeof(CNodeData));
	return true;
}

void CNodeSequence::Copy ( CNodeSequence* pSrc)
{
	if (NULL != pSrc)
	{
		m_aNodes.SetSize(pSrc->m_aNodes.GetSize() + 100);

		memcpyex(m_aNodes.GetData(), pSrc->m_aNodes.GetData(), pSrc->m_aNodes.GetSize() * sizeof(CNodeData));
		m_aNodes.SetSize(pSrc->m_aNodes.GetSize());
	}
	
	
}

bool32 CNodeSequence::Set( int32 iPos,IN CNodeData& NodeData)
{
	int32 iSize = m_aNodes.GetSize();
	if ( iPos < 0 || iPos >= iSize ) return false;

	CNodeData *pNodes = (CNodeData *)m_aNodes.GetData();

	uint32 uiID = pNodes[iPos].m_iID;		// id�Ų��ܱ�
	NodeData.m_iID = uiID;
	memcpyex(pNodes + iPos, &NodeData, sizeof(CNodeData));
	return true;
}

void CNodeSequence::RemoveAt( int32 iPos )
{
	int32 iSize = m_aNodes.GetSize();
	if ( iPos < 0 || iPos >= iSize ) return;

	if (iPos + 1 != iSize)
	{
		CNodeData *pNodes = (CNodeData *)m_aNodes.GetData();
		memcpyex(pNodes + iPos, pNodes + iPos + 1, (iSize - (iPos + 1)) * sizeof(CNodeData));
	}
	m_aNodes.SetSize(iSize - 1);
}

void CNodeSequence::DumpAttatchedCurves(CArray<CDrawingCurve*,CDrawingCurve*>& Curves)
{
	int32 iSize = m_aAttatchedCurves.GetSize();

	Curves.SetSize(iSize);
	if (iSize > 0)
	{
		memcpyex(Curves.GetData(), m_aAttatchedCurves.GetData(), iSize * sizeof(CDrawingCurve*));
	}
}

int32 CNodeSequence::HalfSearch( int32 id )
{
	CNodeData *pNodes = (CNodeData *)m_aNodes.GetData();

	int32 i, t;
	int32 iStart = 0;
	int32 iEnd = m_aNodes.GetSize()-1;
	while ( iStart <= iEnd )
	{
		i = ( iStart + iEnd ) / 2;
		t = pNodes[i].m_iID;
		if ( id == t ) return i;
		if ( id > t ) iStart = i + 1;
		else iEnd = i - 1;
	}
	return -1;
}

bool32 CNodeSequence::HalfSearchEqu ( int32 id,int32 t,int32 i, int32& iPos,int32& iSub )
{
	if ( id == t )
	{
		return true;
	}
	int32 iSubTemp = abs(id - t);
	if ( iSub > iSubTemp )
	{
		iSub = iSubTemp;
		iPos = i;
	}
	return false;
}

int32 CNodeSequence::HalfSearchEx( int32 id )
{
	CNodeData *pNodes = m_aNodes.GetData();

	int32 i, t;
	int32 iStart = 0;
	int32 iEnd = m_aNodes.GetSize()-1;
	if ( 0 == iEnd ) return 0;
	int32 iPos = -1;
	int32 iSub=LONG_MAX;
	while ( iStart <= iEnd )
	{
		i = ( iStart + iEnd ) / 2;
		t = pNodes[i].m_iID;
		if ( HalfSearchEqu(id,t,i,iPos,iSub ))
		{
			return i;
		}
		if ( id > t ) iStart = i + 1;
		else iEnd = i - 1;
	}
	return iPos;
}

bool32 CNodeSequence::Lookup( int32 iID, int32& iPos )
{
	iPos = HalfSearch(iID);
	if ( -1 != iPos )
	{
		return true;
	}
	return false;
}

bool32 CNodeSequence::LookupEx( int32 iID, int32& iPos )
{
	iPos = HalfSearchEx(iID);
	if ( -1 != iPos )
	{
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////

CNodeDrawerData::CNodeDrawerData()
{
	m_iCurveLoop= 0;
	m_iValidNum	= 0;
	m_iPos		= 0;
	m_iX		= 0;
	m_iXLeft	= 0;
	m_iXRight	= 0;
	m_iOpen		= 0;
	m_iClose	= 0;
	m_iHigh		= 0;
	m_iLow		= 0;
	m_iAmount	= 0;
	m_iVolumn	= 0;
	m_iAvg		= 0;
	m_iHold		= 0;
	m_bRise		= 0;
}

CNodeDrawerData::CNodeDrawerData(IN const CNodeData& NodeData)
:CNodeData(NodeData)
{
	m_iCurveLoop= 0;
	m_iValidNum	= 0;
	m_iPos		= 0;
	m_iX		= 0;
	m_iXLeft	= 0;
	m_iXRight	= 0;
	m_iOpen		= 0;
	m_iClose	= 0;
	m_iHigh		= 0;
	m_iLow		= 0;
	m_iAmount	= 0;
	m_iVolumn	= 0;
	m_iAvg		= 0;
	m_iHold		= 0;
	m_bRise		= 0;
}
CNodeDrawerData::~CNodeDrawerData()
{

}

void CNodeDrawerData::CalcXPos(int32 &x1, int32 &x2, int32 &x3)
{
	x1 = x2 = x3 = m_iX;

	int32 iNodeWidth = m_iXRight - m_iXLeft + 1;
	if (iNodeWidth <= 4)
	{
		x1 = x2 = x3 = m_iX;
	}
	else
	{
		iNodeWidth = (int32)((float)iNodeWidth * 3 / 4);
		if (iNodeWidth >= 3)
		{
			x1 = x2 - iNodeWidth / 2;
			x3 = x2 + iNodeWidth / 2;
		}
	}
}



///////////////////////////////////////////////////////////////////////////////
CNodeDrawer::CNodeDrawer()
{
	m_iRef			= 0;
}

CNodeDrawer::~CNodeDrawer()
{
}

int32 CNodeDrawer::AddRef ( )
{
	return ++m_iRef;
}
int32 CNodeDrawer::Release ( )
{
	return --m_iRef;	
}

//��ͬX�����ص��һ�����ڸ���ǰһ�����ڵķ���������ʵ�����軭�ĵ� - K�߰汾
//ÿ������һ��������ͼ�Σ���ͼ�α����ڵ�xLeft, xRight, iLow, iHigh�����ƣ������ڴ�����ʱ�򣬻��ص���ͬһ������X���ص��ϵ���
//�ȼ��� ��Ҫ���ľ�������Ӧ��pNodeDrawerDatas �� ���������������и��ǿ��� - ��������ֻ����ͬһ��X����ʱ���Ż��γɻ���ʱ�Ķѵ������Դ���1������ʱ���Ͳ���Ҫ������
//����ʵ����Ҫ���ĸ��� - ʵ�ʷ��صľ��κ�ָ�����Դ���ͼ�ұߵ���ߵ�˳������ģ�����ʱ��Ҫ�Ӻ��滭��ǰ��
int32	CNodeDrawer::CalcNeedCalcNodesOverlapKLine(IN CNodeDrawerData *pNodeDrawerDatas, IN int32 iNodeCount, IN CChartCurve &ChartCurve, 
								 OUT CArray<CRect, CRect &>	&aNeedDrawRects, OUT CArray<CNodeDrawerData *, CNodeDrawerData *>& aNeedDrawNodeDrawerDatas)
{
	int32 iNeedDrawNodeCount = 0;
	if ( NULL==pNodeDrawerDatas || iNodeCount < 1 )
	{		
		return 0;
	}
	

	
	aNeedDrawRects.SetSize( iNodeCount );
	aNeedDrawNodeDrawerDatas.SetSize( iNodeCount );
	
	if( ChartCurve.GetChartRegion().m_aXAxisNodes.GetSize() < 1 )
	{		
		return 0;
	}
	
	// ��Сֵ�����ֵ�ĵ����Ҫ��ӽ�ȥ
	// ������ֵ����Сֵ���ƶ�ƽ���ߵ�Ӱ�죬��Ҫ������ȡ
	float fMaxY, fMinY;
	fMaxY = FLT_MIN;
	fMinY = FLT_MAX;
	for ( int i=0; i < iNodeCount ; i++ )
	{
		if ( pNodeDrawerDatas[i].m_fLow < fMinY )
		{
			fMinY = pNodeDrawerDatas[i].m_fLow;
		}
		if ( pNodeDrawerDatas[i].m_fHigh > fMaxY )
		{
			fMaxY = pNodeDrawerDatas[i].m_fHigh;
		}
	}
	
	
	if ( ChartCurve.GetChartRegion().m_aXAxisNodes[0].m_fPixelWidthPerUnit < 1.0 )
	{
		//����1.0�ģ�û��Ҫ�����ص����㣬����С��1.0���ظ���
		float iHighest = 0, iLowest = 0;
		for (int32 i = iNodeCount - 1; i >= 0 ; i--)
		{
			if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
			{
				continue;
			}
			
			
			//�������ڴӺ��濪ʼ���㣬��任����ǰ��ָǰ���¼�ģ���С��ָfValue�Ĵ�С�������������Ͼ���ԽTop(YֵԽС)��Խ��:
			//    1. û������ͬXǰ�����ߵͣ����Ժ��Լ�����
			//    2. ������ǰ��Ĵ���ߵͣ���ӽ�ȥ
			//
			//��Ϊm_iHigh,��m_iLow
			
			CNodeDrawerData &node = pNodeDrawerDatas[i];
			bool32 bAdd = false;
			//������
			if ( 0 == iNeedDrawNodeCount || aNeedDrawNodeDrawerDatas[iNeedDrawNodeCount-1]->m_iX != node.m_iX)
			{
				//û����ӹ� || ����ͬX���
				//��ʼ�����ֵ
				bAdd     = true;
				iHighest = (float)node.m_iHigh;
				iLowest  = (float)node.m_iLow;
			}
			else if ( iHighest > node.m_iHigh )
			{
				//��ǰ���¼����ЩͬX��Ҫ�ߣ������ȣ�����Ҫ��(m_iԽ�󣬾���Խbottom��������value��Խ��)
				bAdd     = true;
				iHighest = (float)node.m_iHigh;
			}
			else if ( iLowest < node.m_iLow )
			{
				//��һЩ
				bAdd     = true;
				iLowest  = (float)node.m_iLow;
			}
			else if ( fMinY >= node.m_fLow )
			{
				bAdd  = true;
				fMinY = FLT_MIN; 
			}
			else if ( fMaxY <= node.m_fHigh )
			{
				bAdd  = true;
				fMaxY = FLT_MAX;
			}
			
			if ( bAdd )
			{
				aNeedDrawNodeDrawerDatas[ iNeedDrawNodeCount ] = pNodeDrawerDatas + i;
				//��������
				int32 x1 = 0, x2 = 0, x3 = 0;
				pNodeDrawerDatas[i].CalcXPos(x1, x2, x3);
				
				aNeedDrawRects[ iNeedDrawNodeCount ].left   = x1;
				aNeedDrawRects[ iNeedDrawNodeCount ].top    = node.m_iHigh;
				aNeedDrawRects[ iNeedDrawNodeCount ].right  = x3;
				aNeedDrawRects[ iNeedDrawNodeCount ].bottom = node.m_iLow;
				
				iNeedDrawNodeCount++;
			}
		}
	}
	else
	{
		//ֱ�Ӹ��ƣ��ų�Invalid
		for (int32 i = iNodeCount - 1; i >= 0 ; i--)
		{
			if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
			{
				continue;
			}
			
			CNodeDrawerData &node = pNodeDrawerDatas[i];
			//������ - ��ǰ�����
			
			
			aNeedDrawNodeDrawerDatas[ iNeedDrawNodeCount ] = pNodeDrawerDatas + i;
			//��������
			int32 x1 = 0, x2 = 0, x3 = 0;
			node.CalcXPos(x1, x2, x3);
			
			
			
			aNeedDrawRects[ iNeedDrawNodeCount ].left   = x1;
			aNeedDrawRects[ iNeedDrawNodeCount ].top    = node.m_iHigh;
			aNeedDrawRects[ iNeedDrawNodeCount ].right  = x3;
			aNeedDrawRects[ iNeedDrawNodeCount ].bottom = node.m_iLow;
			
			iNeedDrawNodeCount++;
		}
	}
	
	aNeedDrawRects.SetSize(iNeedDrawNodeCount);
	aNeedDrawNodeDrawerDatas.SetSize(iNeedDrawNodeCount);
	return iNeedDrawNodeCount;
}

///////////////////////////////////////////////////////////////////////////////
// 
CChartCurve::CChartCurve(CChartRegion &ChartRegion,uint32 iFlag)
:m_ChartRegion(ChartRegion)
{
	int32 iFlagInc = 0;
	if ( CheckFlag(iFlag,CChartCurve::KYTransformByLowHigh))	iFlagInc ++;
	if ( CheckFlag(iFlag,CChartCurve::KYTransformByClose))		iFlagInc ++;
	if ( CheckFlag(iFlag,CChartCurve::KYTransformByAmount))		iFlagInc ++;
	if ( CheckFlag(iFlag,CChartCurve::KYTransformByVolumn))		iFlagInc ++;
	if ( CheckFlag(iFlag,CChartCurve::KYTransformByAvg))		iFlagInc ++;
	
	ASSERT(iFlagInc==1);

	if ( CheckFlag(iFlag,CDrawingCurve::KSelect))
	{
		ASSERT(0);
	}

	m_iFlag			= iFlag;
	m_pNodes		= NULL;
	m_StrTitle		= _T("");
	m_StrGroupTitle	= _T("");
	m_clrTitleText	= RGB(0,0,255);
	m_iLineWidth	= 1;
	m_iSelectNodeID	= -1;
	m_pDrawer		= NULL;
	m_iValidNodeNum = 0;

	m_eAxisYType = CPriceToAxisYObject::EAYT_Normal;
	
	m_bOccupyIndexLineClr = false;

	m_fUserMax = -FLT_MAX;
	m_fUserMin = FLT_MAX;
	m_iLevel	= 0;
	m_fPriceBase = 0.0f;
}

//lint --e{1579}
CChartCurve::~CChartCurve()
{
	DetatchNodes();
	DetatchDrawer();
}

void CChartCurve::SetDependent()
{
	GetChartRegion().OnDependentChanged(this);
}

void CChartCurve::AttatchDrawer(CNodeDrawer* pDrawer)
{
	pDrawer->AddRef();
	DetatchDrawer();
	m_pDrawer = pDrawer;
}

void CChartCurve::DetatchDrawer( )
{
	if (NULL != m_pDrawer)
	{
		int32 iRet = m_pDrawer->Release();
		if ( iRet == 0 )
		{
			DEL(m_pDrawer);
		}
	}
}

void CChartCurve::DetatchNodes()
{
	if ( NULL != m_pNodes )
	{
		int32 iRef = m_pNodes->Release(this);
		if ( iRef == 0 )
		{
			GetChartRegion().GetViewData()->OnNodesRelease(&GetChartRegion(),this,m_pNodes);
			GetChartRegion().GetViewData()->RemoveNodes(m_pNodes);
			m_pNodes->RemoveAll();
			
// #ifdef _DEBUG
// 	CString StrLog;
// 	StrLog.Format(_T("DeleteNodes 0x%08x"),m_pNodes);
// 	_Log ( StrLog);
// #endif// _DEBUG

			DEL(m_pNodes);
		}
		m_pNodes = NULL;
	}
}
void CChartCurve::AttatchNodes (CNodeSequence* pNodes )
{
	pNodes->AddRef(this);
	DetatchNodes();
	m_pNodes = pNodes;
}

extern void TestNodes(CArray<CNodeData,CNodeData&>& aNodes,const char * pFile,CString StrMerchCode, int32 iMarketId);

bool32 CChartCurve::CalcY(OUT float &fYMin, OUT float &fYMax)
{
	if (NULL == m_pNodes || m_pNodes->GetSize() <= 0)
	{
		fYMin	= 0.;
		fYMax	= 0.;
		return false;
	}

	int32 i, iSize;
	CNodeSequence* pNodes = NULL;
	pNodes = m_pNodes;	
	iSize = pNodes->GetSize();
	
	// 
	fYMin	= FLT_MAX;
	fYMax	= -FLT_MAX;

	for (i = 0; i < iSize; i++)
	{
		CNodeData NodeData;
		pNodes->GetAt(i,NodeData);
		 
		if ( CheckFlag(NodeData.m_iFlag,CNodeData::KValueInvalid) || CheckFlag(NodeData.m_iFlag, CNodeData::KDrawNULL))
		{
			continue;
		}
		if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByLowHigh) )
		{
			float fHigh = NodeData.m_fHigh >= NodeData.GetMax() ? NodeData.m_fHigh : NodeData.GetMax();
			float fLow  = NodeData.m_fLow  <= NodeData.GetMin() ? NodeData.m_fLow  : NodeData.GetMin();

			if (fHigh > fYMax)	fYMax = fHigh;
			if (fYMin > fLow)	fYMin = fLow;
		}
		if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByClose))
		{
			if (NodeData.m_fClose > fYMax) fYMax = NodeData.m_fClose;
			if (fYMin > NodeData.m_fClose) fYMin = NodeData.m_fClose;
		}
		if (CheckFlag(m_iFlag, CChartCurve::KYTransformByAvg))
		{
			if (NodeData.m_fAvg > fYMax) fYMax = NodeData.m_fAvg;
			if (fYMin > NodeData.m_fAvg) fYMin = NodeData.m_fAvg;
		}
		if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByAmount))
		{
			if (NodeData.m_fAmount > fYMax) fYMax = NodeData.m_fAmount;
			if (fYMin > NodeData.m_fAmount) fYMin = NodeData.m_fAmount;
		}
		if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByVolumn))
		{
			if (NodeData.m_fVolume > fYMax) fYMax = NodeData.m_fVolume;
			if (fYMin > NodeData.m_fVolume) fYMin = NodeData.m_fVolume;
		}
	}

// 	if (-FLT_MAX == fYMax ||
// 		FLT_MAX == fYMin ||
// 		IsTooBig(fYMax) ||
// 		IsTooSmall(fYMin))
// 	{
// 		return false;
// 	}

	if (-FLT_MAX == fYMax ||
		FLT_MAX == fYMin)
	{
		return false;
	}

	// �Ƿ����û��趨��Сֵ
	fYMax = max(m_fUserMax, fYMax);
	fYMin = min(m_fUserMin, fYMin);

	// ����ָ���ߣ� ���VolStick��Stick���ԣ� ��СֵΪ0.
	if (CheckFlag(m_iFlag, CChartCurve::KTypeIndex))
	{
		if (NULL != m_pDrawer)
		{
			CIndexDrawer *pIndexDrawer = dynamic_cast<CIndexDrawer *>(m_pDrawer);
			if (NULL != pIndexDrawer)
			{
				if (CIndexDrawer::EIDSVolStick == pIndexDrawer->m_eIndexDrawType ||
					CIndexDrawer::EIDSStick == pIndexDrawer->m_eIndexDrawType)
				{
					if (fYMin > 0.)
						fYMin = 0.;
					m_AxisYCalcObject.SetAlignRectTop(false);	// ������ͼ��Ӧ����ͬ�۸�ײ���
				}
			}
		}
	}

	if ( CPriceToAxisYObject::EAYT_Pecent == m_eAxisYType )
	{
		// �ٷֱ����꣬��ת��, �Ե�һ���Ŀ��̼�Ϊ��׼
		if ( m_fPriceBase == GetInvalidPriceBaseYValue() )
		{
			// û�����û���ֵ
			ASSERT( NULL==GetNodes() || GetNodes()->GetSize()==0 );	// �������������
			return false;	// �޷���ȡbase
		}

		// �趨�˻���ֵ���Ϳ��Լ�����
		float fAxisMin, fAxisMax;
		CPriceToAxisYObject::PriceYToAxisYByBase(m_eAxisYType, m_fPriceBase, fYMax, fAxisMax);
		CPriceToAxisYObject::PriceYToAxisYByBase(m_eAxisYType, m_fPriceBase, fYMin, fAxisMin);
		fYMin = fAxisMin;
		fYMax = fAxisMax;
	}
	else if ( CPriceToAxisYObject::EAYT_Log == m_eAxisYType )
	{
		// �������꣬ �Ե����ڶ��������̼���Ϊ��׼
		if ( m_fPriceBase == GetInvalidPriceBaseYValue() ) // ������ô�ɵ�һ��ֵ��
		{
			ASSERT( NULL==GetNodes() || GetNodes()->GetSize()==0 );	// �������������
			return false;	// �޷���ȡbase
		}
		
		// �趨�˻���ֵ���Ϳ��Լ�����
		float fAxisMin, fAxisMax;
		CPriceToAxisYObject::PriceYToAxisYByBase(m_eAxisYType, m_fPriceBase, fYMax, fAxisMax);
		CPriceToAxisYObject::PriceYToAxisYByBase(m_eAxisYType, m_fPriceBase, fYMin, fAxisMin);
		fYMin = fAxisMin;
		fYMax = fAxisMax;
	}
	
	return true;
}

#if 0
// ��ͼ�Ż���������ǰ�Ĵ���
void CChartCurve::Draw(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC,int32 iLoop)
{
	CChartRegion &ChartRegion = GetChartRegion();

	UINT uiTimeStart = timeGetTime();

	// 
	if ( NULL == m_pNodes )
	{
		return;
	}
	
	//
	float fMaxValue = FLT_MIN;
	float fMinValue = FLT_MAX;
	CPoint ptMaxValue;
	CPoint ptMinValue;
	int32 iStart = 0;

	CNodeData *pNodeData = (CNodeData *)m_pNodes->GetNodes().GetData();
	CAxisNode *pAxisNode = (CAxisNode *)ChartRegion.m_aXAxisNodes.GetData();

	int32 iSize = min(m_pNodes->GetNodes().GetSize(), ChartRegion.m_aXAxisNodes.GetSize());
	CArray<CNodeDrawerData, CNodeDrawerData&> aNodeDrawerDatas;
	aNodeDrawerDatas.SetSize(iSize);

	CNodeDrawerData *pNodeDrawerData = (CNodeDrawerData *)aNodeDrawerDatas.GetData();

	for (int32 i = 0; i < iSize; i++)
	{
		CNodeData &NodeData = pNodeData[i];
	
		CNodeDrawerData DrawerData(NodeData);
		int32 x			= 0;
		int32 xLeft		= 0;
		int32 xRight	= 0;
		int32 iHigh		= 0;
		int32 iLow		= 0;
		int32 iOpen		= 0;
		int32 iClose	= 0;
		int32 iAvg		= 0;
		int32 iAmount	= 0;
		int32 iVolumn	= 0;
		int32 iHold		= 0;

		// 
		x		= ChartRegion.m_aXAxisNodes[i].m_iCenterPixel;
		xLeft	= ChartRegion.m_aXAxisNodes[i].m_iStartPixel;
		xRight	= ChartRegion.m_aXAxisNodes[i].m_iEndPixel;
		
		// ת������ֵ��Y����
		if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByLowHigh))
		{
			ValueRegionY(NodeData.m_fHigh, iHigh);
			ValueRegionY(NodeData.m_fLow, iLow);
			ValueRegionY(NodeData.m_fOpen, iOpen);
			ValueRegionY(NodeData.m_fClose, iClose);
			ValueRegionY(NodeData.m_fAvg, iAvg);
			ValueRegionY(NodeData.m_fAmount, iAmount);
			ValueRegionY(NodeData.m_fVolume, iVolumn);
			ValueRegionY(NodeData.m_fHold, iHold);

			ChartRegion.RegionYToClient(iHigh);
			ChartRegion.RegionYToClient(iLow);
			ChartRegion.RegionYToClient(iOpen);
			ChartRegion.RegionYToClient(iAvg);
			ChartRegion.RegionYToClient(iClose);
			ChartRegion.RegionYToClient(iAmount);
			ChartRegion.RegionYToClient(iVolumn);
			ChartRegion.RegionYToClient(iHold);

			ChartRegion.ClipY(iHigh);
			ChartRegion.ClipY(iLow);
			ChartRegion.ClipY(iOpen);
			ChartRegion.ClipY(iClose);
			ChartRegion.ClipY(iAvg);
			ChartRegion.ClipY(iAmount);
			ChartRegion.ClipY(iVolumn);
			ChartRegion.ClipY(iHold);
		}

		if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByClose))
		{
			ValueRegionY(NodeData.m_fClose, iClose);
			ChartRegion.RegionYToClient(iClose);
			ChartRegion.ClipY(iClose);

			iHigh	= iClose;
			iLow	= iClose;
			iOpen	= iClose;
			iAvg	= iClose;
			iAmount	= iClose;
			iVolumn	= iClose;
			iHold	= iClose;
		}

		if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByAmount))
		{
			ValueRegionY(NodeData.m_fAmount, iAmount);
			ChartRegion.RegionYToClient(iAmount);
			ChartRegion.ClipY(iAmount);

			iHigh	= iAmount;
			iLow	= iAmount;
			iOpen	= iAmount;
			iClose	= iAmount;
			iAvg	= iAmount;
			iVolumn	= iAmount;
			iHold	= iAmount;
		}

		if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByVolumn))
		{
			ValueRegionY(NodeData.m_fVolume, iVolumn);
			ChartRegion.RegionYToClient(iVolumn);
			ChartRegion.ClipY(iVolumn);
			iHigh	= iVolumn;
			iLow	= iVolumn;
			iOpen	= iVolumn;
			iClose	= iVolumn;
			iAvg	= iVolumn;
			iAmount	= iVolumn;
			iHold	= iVolumn;
		}

		if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByAvg))
		{
			// �����ߵ�ʱ����������
			ValueRegionY(NodeData.m_fAvg,iAvg);
			ChartRegion.RegionYToClient(iAvg);
			ChartRegion.ClipY(iAvg);

			iHigh	= iAvg;
			iLow	= iAvg;
			iOpen	= iAvg;
			iClose	= iAvg;			
			iAmount	= iAvg;
			iVolumn = iAvg;
			iHold	= iAvg;
		}

		// 
		if ( fMaxValue < NodeData.m_fHigh )
		{
			fMaxValue = NodeData.m_fHigh;
			ptMaxValue = CPoint (x,iHigh);
		}

		if ( fMinValue > NodeData.m_fLow )
		{
			fMinValue = NodeData.m_fLow;
			ptMinValue = CPoint (x,iLow);
		}

		//
		DrawerData.m_iCurveLoop = iLoop;
		DrawerData.m_iValidNum	= m_iValidNodeNum;
		DrawerData.m_iPos		= i;
		DrawerData.m_iX			= x;
		DrawerData.m_iXLeft		= xLeft;
		DrawerData.m_iXRight	= xRight;
		DrawerData.m_iOpen		= iOpen;
		DrawerData.m_iClose		= iClose;
		DrawerData.m_iLow		= iLow;
		DrawerData.m_iHigh		= iHigh;
		DrawerData.m_iAvg		= iAvg;
		DrawerData.m_iAmount	= iAmount;
		DrawerData.m_iVolumn	= iVolumn;
		DrawerData.m_iHold		= iHold;


		// ��Ӧ���K�ߵ��ǵ�(���Ƴɽ�����ָ��ʱ�õ�:):
		if ( 0 == m_ChartRegion.GetParentIoView()->m_iChartType )
		{
			// ��ʱ
			if ( 0 == i )
			{
				DrawerData.m_bRise = true;					 
			}
			else
			{
				// �Ƚϵ�ǰ�۸��ǰһ���ļ۸�
				CNodeData KLineNode;
				CNodeData KLineNodePre;
				m_ChartRegion.GetParentIoView()->m_MerchParamArray[0]->m_pKLinesShow->GetAt(i, KLineNode);
				m_ChartRegion.GetParentIoView()->m_MerchParamArray[0]->m_pKLinesShow->GetAt(i-1, KLineNodePre);

				if ( KLineNode.m_fClose > KLineNodePre.m_fClose )
				{
					DrawerData.m_bRise = true;
				}
				else if ( KLineNode.m_fClose < KLineNodePre.m_fClose )
				{
					DrawerData.m_bRise = false;
				}
				else
				{
					// ����ǰһ��
					DrawerData.m_bRise = pNodeDrawerData[i-1].m_bRise;
				}
			}
		}
		else if ( 1 == m_ChartRegion.GetParentIoView()->m_iChartType )
		{
			// k ��:

			CNodeData KLineNode;
			m_ChartRegion.GetParentIoView()->m_MerchParamArray[0]->m_pKLinesShow->GetAt(i, KLineNode);

			if ( KLineNode.m_fClose > KLineNode.m_fOpen )
			{
				DrawerData.m_bRise = true;
			}
			else if ( KLineNode.m_fClose < KLineNode.m_fOpen) 
			{
				DrawerData.m_bRise = false;
			}
			else
			{
				// ����ǰһ��	
				DrawerData.m_bRise = pNodeDrawerData[i-1].m_bRise;
			}
		}
		else if ( 2 == m_ChartRegion.GetParentIoView()->m_iChartType)
		{
			// ����ͼ:			
			if ( 0 == i )
			{
				DrawerData.m_bRise = true;					 
			}
			else
			{
				// �Ƚϵ�ǰ�۸��ǰһ���ļ۸�
				CNodeData KLineNode;
				CNodeData KLineNodePre;
				m_ChartRegion.GetParentIoView()->m_MerchParamArray[0]->m_pKLinesShow->GetAt(i, KLineNode);
				m_ChartRegion.GetParentIoView()->m_MerchParamArray[0]->m_pKLinesShow->GetAt(i-1, KLineNodePre);
				
				if ( KLineNode.m_fClose > KLineNodePre.m_fClose )
				{
					DrawerData.m_bRise = true;
				}
				else if ( KLineNode.m_fClose < KLineNodePre.m_fClose )
				{
					DrawerData.m_bRise = false;
				}
				else
				{
					// ����ǰһ��
					DrawerData.m_bRise = pNodeDrawerData[i-1].m_bRise;
				}
			}
		}
		
		if ( DrawerData.m_fHigh < DrawerData.m_fOpen || DrawerData.m_fHigh < DrawerData.m_fClose || DrawerData.m_fHigh < DrawerData.m_fLow )			 
		{
			// ��߼۷Ƿ�
			// ASSERT(0);
		}

		if ( DrawerData.m_fLow > DrawerData.m_fOpen || DrawerData.m_fLow > DrawerData.m_fClose || DrawerData.m_fLow > DrawerData.m_fLow )
		{
			// ��ͼ۷Ƿ�
			// ASSERT(0);
		}

		pNodeDrawerData[i] = DrawerData;
	}	

	//
	if ( NULL != m_pDrawer )
	{
		m_pDrawer->DrawNodes(pDC,pPickDC, *this, aNodeDrawerDatas);
	}
	
	UINT uiTimeEnd = timeGetTime();
}

#else
// �Ż��Ĵ���
void CChartCurve::Draw(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC,int32 iLoop)
{
	CChartRegion &ChartRegion = GetChartRegion();	

	// 
	if ( NULL == m_pNodes )
	{
		return;
	}

	const CNodeData *pNodeData = (CNodeData *)m_pNodes->GetNodes().GetData();
	

	int32 iSize = min(m_pNodes->GetNodes().GetSize(), ChartRegion.m_aXAxisNodes.GetSize());//lint !e666
	CArray<CNodeDrawerData, CNodeDrawerData&> aNodeDrawerDatas;
	aNodeDrawerDatas.SetSize(iSize);

	CNodeDrawerData *pNodeDrawerData = (CNodeDrawerData *)aNodeDrawerDatas.GetData();

	// �����˵����ߣ����ڵ���������������Ҫ֪�����ߵ��κ���Ϣ
	// ʹ����ʱ�������־���ߣ�ͬ������ʱ�����������
	if ( CheckFlag(m_iFlag, KYTransformToAlignDependent)
		|| CheckFlag(m_iFlag, KTimeIdAlignToXAxis) )
	{
		// ������, Ŀǰֻ��������
		ASSERT( !CheckFlag(m_iFlag, KYTransformToAlignDependent) || CheckFlag(m_iFlag, KTypeKLine) || CheckFlag(m_iFlag, KTypeTrend) );

		// ���ڵ�����������������ÿ���㶼�������ݺ�ÿ��������ݶ�Ҫ��ʾ������Ҫ�����ﴦ����
		// ������Ӧ��ȡ�������ݶ�
		iSize = m_pNodes->GetNodes().GetSize();
		aNodeDrawerDatas.SetSize(iSize);
		CNodeDrawerData *pTmpNodeDrawerData = (CNodeDrawerData *)aNodeDrawerDatas.GetData();

		int32 iAxisXPos = 0;
		int32 iDrawNodePos = 0;
		for (int32 i = 0; i < iSize && iAxisXPos < ChartRegion.m_aXAxisNodes.GetSize(); i++)
		{
			const CNodeData &NodeData = pNodeData[i];
			
			CNodeDrawerData &DrawerData = pTmpNodeDrawerData[iDrawNodePos];
			DrawerData = NodeData;

			if ( iDrawNodePos == 0 && CheckFlag(NodeData.m_iFlag, CNodeData::KIdInvalid) )
			{
				continue;	// ��һ����Ч�ڵ�ǰ�Ľڵ�ȫ������
			}
			
			// ʱ�������ϸ����
			while ( (uint32)ChartRegion.m_aXAxisNodes[iAxisXPos].m_iTimeId < NodeData.m_iID )
			{
				// ������ʱ�������ݵ����࣬ƫ������
				iAxisXPos++;
				if ( iAxisXPos >= ChartRegion.m_aXAxisNodes.GetSize() )
				{
					break;	// �������������
				}
			}
			if ( iAxisXPos >= ChartRegion.m_aXAxisNodes.GetSize() )
			{
				break;	// ������ʾ�������Ѿ���������
			}

			if ((uint32)ChartRegion.m_aXAxisNodes[iAxisXPos].m_iTimeId > NodeData.m_iID )
			{
				// ������ʱ�������ݵ��Ҳ࣬����㲻����ʾ����һ����
				continue;
			}

			ASSERT( ChartRegion.m_aXAxisNodes[iAxisXPos].m_iTimeId == NodeData.m_iID );

			// ʱ������ȵĵ�
			DrawerData.m_iX			= ChartRegion.m_aXAxisNodes[iAxisXPos].m_iCenterPixel;
			DrawerData.m_iXLeft		= ChartRegion.m_aXAxisNodes[iAxisXPos].m_iStartPixel;
			DrawerData.m_iXRight	= ChartRegion.m_aXAxisNodes[iAxisXPos].m_iEndPixel;
			
			// ת������ֵ��Y����
			if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByLowHigh))
			{
				PriceYToRegionY(NodeData.m_fHigh, DrawerData.m_iHigh);
				PriceYToRegionY(NodeData.m_fLow, DrawerData.m_iLow);
				PriceYToRegionY(NodeData.m_fOpen, DrawerData.m_iOpen);
				PriceYToRegionY(NodeData.m_fClose, DrawerData.m_iClose);
				PriceYToRegionY(NodeData.m_fAvg, DrawerData.m_iAvg);
				PriceYToRegionY(NodeData.m_fAmount, DrawerData.m_iAmount);
				PriceYToRegionY(NodeData.m_fVolume, DrawerData.m_iVolumn);
				PriceYToRegionY(NodeData.m_fHold, DrawerData.m_iHold);
				
				ChartRegion.RegionYToClient(DrawerData.m_iHigh);
				ChartRegion.RegionYToClient(DrawerData.m_iLow);
				ChartRegion.RegionYToClient(DrawerData.m_iOpen);
				ChartRegion.RegionYToClient(DrawerData.m_iAvg);
				ChartRegion.RegionYToClient(DrawerData.m_iClose);
				ChartRegion.RegionYToClient(DrawerData.m_iAmount);
				ChartRegion.RegionYToClient(DrawerData.m_iVolumn);
				ChartRegion.RegionYToClient(DrawerData.m_iHold);
				
				ChartRegion.ClipY(DrawerData.m_iHigh);
				ChartRegion.ClipY(DrawerData.m_iLow);
				ChartRegion.ClipY(DrawerData.m_iOpen);
				ChartRegion.ClipY(DrawerData.m_iClose);
				ChartRegion.ClipY(DrawerData.m_iAvg);
				ChartRegion.ClipY(DrawerData.m_iAmount);
				ChartRegion.ClipY(DrawerData.m_iVolumn);
				ChartRegion.ClipY(DrawerData.m_iHold);
			}
			
			if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByClose))
			{
				int32 iClose, iOpen;
				
				bool32 bOk	= PriceYToRegionY(NodeData.m_fClose, iClose);
				bool32 bOk2	= PriceYToRegionY(NodeData.m_fOpen, iOpen);
				
				if ( bOk )
				{
					ChartRegion.RegionYToClient(iClose);
					ChartRegion.ClipY(iClose);
					
					DrawerData.m_iClose		= iClose;
					DrawerData.m_iHigh		= iClose;
					DrawerData.m_iLow		= iClose;
					DrawerData.m_iOpen		= iClose;
					DrawerData.m_iAvg		= iClose;
					DrawerData.m_iAmount	= iClose;
					DrawerData.m_iVolumn	= iClose;
					DrawerData.m_iHold		= iClose;
				}
				
				if ( bOk2 )
				{
					ChartRegion.RegionYToClient(iOpen);
					ChartRegion.ClipY(iOpen);
					
					DrawerData.m_iOpen = iOpen;
				}
				
				if ( !bOk )
				{
					DrawerData.m_iClose		= -1;
					DrawerData.m_iHigh		= -1;
					DrawerData.m_iLow		= -1;
					DrawerData.m_iOpen		= -1;
					DrawerData.m_iAvg		= -1;
					DrawerData.m_iAmount	= -1;
					DrawerData.m_iVolumn	= -1;
					DrawerData.m_iHold		= -1;
				}			
			}
			
			if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByAmount))
			{
				int32 iAmount;
				PriceYToRegionY(NodeData.m_fAmount, iAmount);
				ChartRegion.RegionYToClient(iAmount);
				ChartRegion.ClipY(iAmount);
				
				DrawerData.m_iAmount	= iAmount;
				DrawerData.m_iHigh		= iAmount;
				DrawerData.m_iLow		= iAmount;
				DrawerData.m_iOpen		= iAmount;
				DrawerData.m_iClose		= iAmount;
				DrawerData.m_iAvg		= iAmount;
				DrawerData.m_iVolumn	= iAmount;
				DrawerData.m_iHold		= iAmount;
			}
			
			if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByVolumn))
			{
				int32 iVolumn;
				PriceYToRegionY(NodeData.m_fVolume, iVolumn);
				ChartRegion.RegionYToClient(iVolumn);
				ChartRegion.ClipY(iVolumn);
				
				DrawerData.m_iVolumn	= iVolumn;
				DrawerData.m_iHigh		= iVolumn;
				DrawerData.m_iLow		= iVolumn;
				DrawerData.m_iOpen		= iVolumn;
				DrawerData.m_iClose		= iVolumn;
				DrawerData.m_iAvg		= iVolumn;
				DrawerData.m_iAmount	= iVolumn;
				DrawerData.m_iHold		= iVolumn;
			}
			
			if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByAvg))
			{
				// �����ߵ�ʱ����������
				int32 iAvg;
				PriceYToRegionY(NodeData.m_fAvg,iAvg);
				ChartRegion.RegionYToClient(iAvg);
				ChartRegion.ClipY(iAvg);
				
				DrawerData.m_iAvg		= iAvg;
				DrawerData.m_iHigh		= iAvg;
				DrawerData.m_iLow		= iAvg;
				DrawerData.m_iOpen		= iAvg;
				DrawerData.m_iClose		= iAvg;			
				DrawerData.m_iAmount	= iAvg;
				DrawerData.m_iVolumn	= iAvg;
				DrawerData.m_iHold		= iAvg;
			}
			
			//
			DrawerData.m_iCurveLoop = iLoop;
			DrawerData.m_iValidNum	= m_iValidNodeNum;
			DrawerData.m_iPos		= i;
			DrawerData.m_bRise = true;
			
			iDrawNodePos++;	// ��������+1
		}	

		// ʵ���������Ƕ���
		aNodeDrawerDatas.SetSize(iDrawNodePos);
	}
	else if (CheckFlag(m_iFlag, KindexCmp))
	{
		// ����ָ����, �Լ���һ��Ū��:
		CArray<CNodeDrawerData, CNodeDrawerData&> aTmpNodeDrawerDatas;

		iSize = m_pNodes->GetNodes().GetSize();
		aTmpNodeDrawerDatas.SetSize(iSize);
		CNodeDrawerData *pTmpNodeDrawerData = (CNodeDrawerData *)aTmpNodeDrawerDatas.GetData();

		int32 iAxisXPos = 0;
		int32 iDrawNodePos = 0;
		for (int32 i = 0; i < iSize && iAxisXPos < ChartRegion.m_aXAxisNodes.GetSize(); i++)
		{
			const CNodeData &NodeData = pNodeData[i];

			CNodeDrawerData &DrawerData = pTmpNodeDrawerData[iDrawNodePos];
			DrawerData = NodeData;

			if ( iDrawNodePos == 0 && CheckFlag(NodeData.m_iFlag, CNodeData::KIdInvalid) )
			{
				continue;	// ��һ����Ч�ڵ�ǰ�Ľڵ�ȫ������
			}

			// ʱ�������ϸ����
			while ( (uint32)ChartRegion.m_aXAxisNodes[iAxisXPos].m_iTimeId < NodeData.m_iID )
			{
				// ������ʱ�������ݵ����࣬ƫ������
				iAxisXPos++;
				if ( iAxisXPos >= ChartRegion.m_aXAxisNodes.GetSize() )
				{
					break;	// �������������
				}
			}
			if ( iAxisXPos >= ChartRegion.m_aXAxisNodes.GetSize() )
			{
				break;	// ������ʾ�������Ѿ���������
			}

			if ( (uint32)ChartRegion.m_aXAxisNodes[iAxisXPos].m_iTimeId > NodeData.m_iID )
			{
				// ������ʱ�������ݵ��Ҳ࣬����㲻����ʾ����һ����
				continue;
			}

			ASSERT( ChartRegion.m_aXAxisNodes[iAxisXPos].m_iTimeId == NodeData.m_iID );

			// ʱ������ȵĵ�
			DrawerData.m_iX			= ChartRegion.m_aXAxisNodes[iAxisXPos].m_iCenterPixel;
			DrawerData.m_iXLeft		= ChartRegion.m_aXAxisNodes[iAxisXPos].m_iStartPixel;
			DrawerData.m_iXRight	= ChartRegion.m_aXAxisNodes[iAxisXPos].m_iEndPixel;

			// ת������ֵ��Y����
			if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByLowHigh))
			{				
				PriceYToCmpClientY(NodeData.m_fHigh, DrawerData.m_iHigh);
				PriceYToCmpClientY(NodeData.m_fLow, DrawerData.m_iLow);
				PriceYToCmpClientY(NodeData.m_fOpen, DrawerData.m_iOpen);
				PriceYToCmpClientY(NodeData.m_fClose, DrawerData.m_iClose);
				PriceYToCmpClientY(NodeData.m_fAvg, DrawerData.m_iAvg);
				PriceYToCmpClientY(NodeData.m_fAmount, DrawerData.m_iAmount);
				PriceYToCmpClientY(NodeData.m_fVolume, DrawerData.m_iVolumn);
				PriceYToCmpClientY(NodeData.m_fHold, DrawerData.m_iHold);

				ChartRegion.ClipY(DrawerData.m_iHigh);
				ChartRegion.ClipY(DrawerData.m_iLow);
				ChartRegion.ClipY(DrawerData.m_iOpen);
				ChartRegion.ClipY(DrawerData.m_iClose);
				ChartRegion.ClipY(DrawerData.m_iAvg);
				ChartRegion.ClipY(DrawerData.m_iAmount);
				ChartRegion.ClipY(DrawerData.m_iVolumn);
				ChartRegion.ClipY(DrawerData.m_iHold);
			}

			if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByClose))
			{
				int32 iClose, iOpen;

				bool32 bOk	= PriceYToCmpClientY(NodeData.m_fClose, iClose);
				bool32 bOk2	= PriceYToCmpClientY(NodeData.m_fOpen, iOpen);

				if ( bOk )
				{
					ChartRegion.ClipY(iClose);

					DrawerData.m_iClose		= iClose;
					DrawerData.m_iHigh		= iClose;
					DrawerData.m_iLow		= iClose;
					DrawerData.m_iOpen		= iClose;
					DrawerData.m_iAvg		= iClose;
					DrawerData.m_iAmount	= iClose;
					DrawerData.m_iVolumn	= iClose;
					DrawerData.m_iHold		= iClose;
				}

				if ( bOk2 )
				{
					ChartRegion.ClipY(iOpen);

					DrawerData.m_iOpen = iOpen;
				}	
			}

			if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByAmount))
			{
				int32 iAmount;
				PriceYToCmpClientY(NodeData.m_fAmount, iAmount);
				ChartRegion.ClipY(iAmount);

				DrawerData.m_iAmount	= iAmount;
				DrawerData.m_iHigh		= iAmount;
				DrawerData.m_iLow		= iAmount;
				DrawerData.m_iOpen		= iAmount;
				DrawerData.m_iClose		= iAmount;
				DrawerData.m_iAvg		= iAmount;
				DrawerData.m_iVolumn	= iAmount;
				DrawerData.m_iHold		= iAmount;
			}

			if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByVolumn))
			{
				int32 iVolumn;
				PriceYToCmpClientY(NodeData.m_fVolume, iVolumn);
				ChartRegion.ClipY(iVolumn);

				DrawerData.m_iVolumn	= iVolumn;
				DrawerData.m_iHigh		= iVolumn;
				DrawerData.m_iLow		= iVolumn;
				DrawerData.m_iOpen		= iVolumn;
				DrawerData.m_iClose		= iVolumn;
				DrawerData.m_iAvg		= iVolumn;
				DrawerData.m_iAmount	= iVolumn;
				DrawerData.m_iHold		= iVolumn;
			}

			if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByAvg))
			{
				// �����ߵ�ʱ����������
				int32 iAvg;
				PriceYToCmpClientY(NodeData.m_fAvg,iAvg);
				ChartRegion.ClipY(iAvg);

				DrawerData.m_iAvg		= iAvg;
				DrawerData.m_iHigh		= iAvg;
				DrawerData.m_iLow		= iAvg;
				DrawerData.m_iOpen		= iAvg;
				DrawerData.m_iClose		= iAvg;			
				DrawerData.m_iAmount	= iAvg;
				DrawerData.m_iVolumn	= iAvg;
				DrawerData.m_iHold		= iAvg;
			}

			//
			DrawerData.m_iCurveLoop = iLoop;
			DrawerData.m_iValidNum	= m_iValidNodeNum;
			DrawerData.m_iPos		= i;
			DrawerData.m_bRise = true;

			iDrawNodePos++;	// ��������+1
		}	

		// ʵ���������Ƕ���
		aTmpNodeDrawerDatas.SetSize(iDrawNodePos);

		//
		if ( NULL != m_pDrawer && aTmpNodeDrawerDatas.GetSize() > 0 )
		{
			m_pDrawer->DrawNodes(pDC,pPickDC, *this, aTmpNodeDrawerDatas);
		}	
	}
	else
	{
		// ��index��chart����Curve
		IChartRegionData *pRegionData = m_ChartRegion.GetParentIoViewData().pChartRegionData;
		CNodeSequence  *pNodeMainShow = pRegionData ->m_MerchParamArray[0]->m_pKLinesShow;
		//ASSERT( NULL != pNodeMainShow && pNodeMainShow->GetSize() == iSize);
		if ( NULL == pNodeMainShow || pNodeMainShow->GetSize() < iSize )
		{
			return;
		}
		
		for (int32 i = 0; i < iSize; i++)
		{
			const CNodeData &NodeData = pNodeData[i];
			
			CNodeDrawerData &DrawerData = pNodeDrawerData[i];
			DrawerData = NodeData;
			
			// 
			DrawerData.m_iX			= ChartRegion.m_aXAxisNodes[i].m_iCenterPixel;
			DrawerData.m_iXLeft		= ChartRegion.m_aXAxisNodes[i].m_iStartPixel;
			DrawerData.m_iXRight	= ChartRegion.m_aXAxisNodes[i].m_iEndPixel;
			
			// ת������ֵ��Y����
			if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByLowHigh))
			{
				PriceYToRegionY(NodeData.m_fHigh, DrawerData.m_iHigh);
				PriceYToRegionY(NodeData.m_fLow, DrawerData.m_iLow);
				PriceYToRegionY(NodeData.m_fOpen, DrawerData.m_iOpen);
				PriceYToRegionY(NodeData.m_fClose, DrawerData.m_iClose);
				PriceYToRegionY(NodeData.m_fAvg, DrawerData.m_iAvg);
				PriceYToRegionY(NodeData.m_fAmount, DrawerData.m_iAmount);
				PriceYToRegionY(NodeData.m_fVolume, DrawerData.m_iVolumn);
				PriceYToRegionY(NodeData.m_fHold, DrawerData.m_iHold);
				
				ChartRegion.RegionYToClient(DrawerData.m_iHigh);
				ChartRegion.RegionYToClient(DrawerData.m_iLow);
				ChartRegion.RegionYToClient(DrawerData.m_iOpen);
				ChartRegion.RegionYToClient(DrawerData.m_iAvg);
				ChartRegion.RegionYToClient(DrawerData.m_iClose);
				ChartRegion.RegionYToClient(DrawerData.m_iAmount);
				ChartRegion.RegionYToClient(DrawerData.m_iVolumn);
				ChartRegion.RegionYToClient(DrawerData.m_iHold);
				
				ChartRegion.ClipY(DrawerData.m_iHigh);
				ChartRegion.ClipY(DrawerData.m_iLow);
				ChartRegion.ClipY(DrawerData.m_iOpen);
				ChartRegion.ClipY(DrawerData.m_iClose);
				ChartRegion.ClipY(DrawerData.m_iAvg);
				ChartRegion.ClipY(DrawerData.m_iAmount);
				ChartRegion.ClipY(DrawerData.m_iVolumn);
				ChartRegion.ClipY(DrawerData.m_iHold);
			}
			
			if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByClose))
			{
				int32 iClose, iOpen;
				
				bool32 bOk	= PriceYToRegionY(NodeData.m_fClose, iClose);
				bool32 bOk2	= PriceYToRegionY(NodeData.m_fOpen, iOpen);
				
				if ( bOk )
				{
					ChartRegion.RegionYToClient(iClose);
					ChartRegion.ClipY(iClose);
					
					DrawerData.m_iClose		= iClose;
					DrawerData.m_iHigh		= iClose;
					DrawerData.m_iLow		= iClose;
					DrawerData.m_iOpen		= iClose;
					DrawerData.m_iAvg		= iClose;
					DrawerData.m_iAmount	= iClose;
					DrawerData.m_iVolumn	= iClose;
					DrawerData.m_iHold		= iClose;
				}
				
				if ( bOk2 )
				{
					ChartRegion.RegionYToClient(iOpen);
					ChartRegion.ClipY(iOpen);
					
					DrawerData.m_iOpen = iOpen;
				}
				
				if ( !bOk )
				{
					DrawerData.m_iClose		= -1;
					DrawerData.m_iHigh		= -1;
					DrawerData.m_iLow		= -1;
					DrawerData.m_iOpen		= -1;
					DrawerData.m_iAvg		= -1;
					DrawerData.m_iAmount	= -1;
					DrawerData.m_iVolumn	= -1;
					DrawerData.m_iHold		= -1;
				}			
			}
			
			if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByAmount))
			{
				int32 iAmount;
				PriceYToRegionY(NodeData.m_fAmount, iAmount);
				ChartRegion.RegionYToClient(iAmount);
				ChartRegion.ClipY(iAmount);
				
				DrawerData.m_iAmount	= iAmount;
				DrawerData.m_iHigh		= iAmount;
				DrawerData.m_iLow		= iAmount;
				DrawerData.m_iOpen		= iAmount;
				DrawerData.m_iClose		= iAmount;
				DrawerData.m_iAvg		= iAmount;
				DrawerData.m_iVolumn	= iAmount;
				DrawerData.m_iHold		= iAmount;
			}
			
			if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByVolumn))
			{
				int32 iVolumn;
				PriceYToRegionY(NodeData.m_fVolume, iVolumn);
				ChartRegion.RegionYToClient(iVolumn);
				ChartRegion.ClipY(iVolumn);
				
				DrawerData.m_iVolumn	= iVolumn;
				DrawerData.m_iHigh		= iVolumn;
				DrawerData.m_iLow		= iVolumn;
				DrawerData.m_iOpen		= iVolumn;
				DrawerData.m_iClose		= iVolumn;
				DrawerData.m_iAvg		= iVolumn;
				DrawerData.m_iAmount	= iVolumn;
				DrawerData.m_iHold		= iVolumn;
			}
			
			if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByAvg))
			{
				// �����ߵ�ʱ����������
				int32 iAvg;
					PriceYToRegionY(NodeData.m_fAvg,iAvg);
				ChartRegion.RegionYToClient(iAvg);
				ChartRegion.ClipY(iAvg);
				
				DrawerData.m_iAvg		= iAvg;
				DrawerData.m_iHigh		= iAvg;
				DrawerData.m_iLow		= iAvg;
				DrawerData.m_iOpen		= iAvg;
				DrawerData.m_iClose		= iAvg;			
				DrawerData.m_iAmount	= iAvg;
				DrawerData.m_iVolumn	= iAvg;
				DrawerData.m_iHold		= iAvg;
			}
			
			//
			DrawerData.m_iCurveLoop = iLoop;
			DrawerData.m_iValidNum	= m_iValidNodeNum;
			DrawerData.m_iPos		= i;
			
			// ��Ӧ���K�ߵ��ǵ�(���Ƴɽ�����ָ��ʱ�õ�:):
			if ( 0 == m_ChartRegion.GetParentIoViewData().pChartRegionData->m_iChartType )
			{
				// ��ʱ
				if ( 0 == i )
				{
					DrawerData.m_bRise = true;					 
				}
				else
				{
					// �Ƚϵ�ǰ�۸��ǰһ���ļ۸�
					const CNodeData &KLineNode       = pNodeMainShow->GetNodes()[i];
					const CNodeData &KLineNodePre	 = pNodeMainShow->GetNodes()[i - 1];
					
					if ( KLineNode.m_fClose > KLineNodePre.m_fClose )
					{
						DrawerData.m_bRise = true;
					}
					else if ( KLineNode.m_fClose < KLineNodePre.m_fClose )
					{
						DrawerData.m_bRise = false;
					}
					else
					{
						// ����ǰһ��
						DrawerData.m_bRise = pNodeDrawerData[i-1].m_bRise;
					}
				}
			}
			else if ( 1 == m_ChartRegion.GetParentIoViewData().pChartRegionData->m_iChartType )
			{
				// k ��:
				const CNodeData &KLineNode = pNodeMainShow->GetNodes()[i];
				
				if ( KLineNode.m_fClose > KLineNode.m_fOpen )
				{
					DrawerData.m_bRise = true;
				}
				else if ( KLineNode.m_fClose < KLineNode.m_fOpen) 
				{
					DrawerData.m_bRise = false;
				}
				else
				{
					// ����ǰһ��	
					DrawerData.m_bRise = pNodeDrawerData[i-1].m_bRise;
				}
			}
			else if ( 2 == m_ChartRegion.GetParentIoViewData().pChartRegionData->m_iChartType)
			{
				// ����ͼ:			
				if ( 0 == i )
				{
					DrawerData.m_bRise = true;					 
				}
				else
				{
					// �Ƚϵ�ǰ�۸��ǰһ���ļ۸�
					const CNodeData &KLineNode       = pNodeMainShow->GetNodes()[i];
					const CNodeData &KLineNodePre	 = pNodeMainShow->GetNodes()[i - 1];
					
					if ( KLineNode.m_fClose > KLineNodePre.m_fClose )
					{
						DrawerData.m_bRise = true;
					}
					else if ( KLineNode.m_fClose < KLineNodePre.m_fClose )
					{
						DrawerData.m_bRise = false;
					}
					else
					{
						// ����ǰһ��
						DrawerData.m_bRise = pNodeDrawerData[i-1].m_bRise;
					}
				}
			}
		}	
	}
	
	//
	if ( NULL != m_pDrawer && aNodeDrawerDatas.GetSize() > 0 )
	{
		m_pDrawer->DrawNodes(pDC,pPickDC, *this, aNodeDrawerDatas);
	}	
}

#endif

//////////////////////////////////////////////////////////////////////
//����ת��
bool32 CChartCurve::RegionXToCurvePos ( int32 x, OUT int32& iPos )
{
	CAxisNode *pAxisNode = (CAxisNode *)GetChartRegion().m_aXAxisNodes.GetData();
	int32 iAxisNodeCount = GetChartRegion().m_aXAxisNodes.GetSize();
	if (iAxisNodeCount <= 0)
		return false;

	// zhangbo 20090711 #��ʱ��������
	x += GetChartRegion().m_RectView.left;

	if ( x < GetChartRegion().GetRectCurves().left )
	{
		return false;	// û�������߷�Χ��
	}
	else if ( x > GetChartRegion().GetRectCurves().right )
	{
		return false; // �������Ҳ���ʾ����
	}
	else if (x < pAxisNode[0].m_iStartPixel)
	{
		iPos = 0;		
		return true;
	}
	else if ( x > pAxisNode[iAxisNodeCount - 1].m_iEndPixel )
	{
		iPos = iAxisNodeCount - 1;
		return true;
	}

	// ���ֲ��ң� ���ٶ�λλ��
	int32 iLow = 0;
	int32 iHigh = iAxisNodeCount - 1;

	iPos = -1;
	while (iLow <= iHigh)
	{
		iPos = (iHigh + iLow) / 2;
		if (x >= pAxisNode[iPos].m_iStartPixel && x <= pAxisNode[iPos].m_iEndPixel)
		{
			return true;
		}
		else if (pAxisNode[iPos].m_iStartPixel > x)
		{
			iHigh = iPos - 1;
		}
		else
		{
			iLow = iPos + 1;
		}
	}

	return false;
}

bool32 CChartCurve::CurvePosToRegionX ( int32 iPos, OUT int32& x )
{
	CAxisNode *pAxisNode = (CAxisNode *)GetChartRegion().m_aXAxisNodes.GetData();
	int32 iAxisNodeCount = GetChartRegion().m_aXAxisNodes.GetSize();
	if (iPos < 0 || iPos >= iAxisNodeCount)
		return false;

	x = pAxisNode[iPos].m_iCenterPixel;

	// zhangbo 20090711 #��ʱ��������
	x -= GetChartRegion().m_RectView.left;
	return true;
}

bool32 CChartCurve::RegionXToCurveID ( int32 x, OUT int32& id )
{
	if (NULL == m_pNodes)
	{	
		return false;
	}

	//����Region�е�x����,����m_pNodes��NodeData��id
	int32 iPos;
	if ( !RegionXToCurvePos(x,iPos))
	{
		return false;
	}
	
	if (iPos < 0 || iPos >= m_pNodes->GetSize())
		return false;

	CNodeData *pNodeData = (CNodeData *)m_pNodes->GetNodes().GetData();
	id = pNodeData[iPos].m_iID;
	return true;
}


bool32 CChartCurve::NodePosValue ( int32 iPos, OUT float& fValue)
{
	// t..fangz0730 �õ��� iPos �������̼�
	if(NULL == m_pNodes)
	{
		return false;
	}
	if ( iPos < 0 || iPos > m_pNodes->GetSize())
	{
		return false;
	}

	CNodeData Node;
	m_pNodes->GetAt(iPos,Node);
	fValue = Node.m_fClose;
	return true;

	// zhangbo 20090706 #��ʱ����
	//...
	
// 	//���ݵ�iPos��NodeData,��ȡNodeData��Yֵ
// 	
// 	// 
// 	if ( NULL == m_pNodes )
// 	{
// 		return false;
// 	}
// 
// 	int32 iSize = m_NodesAlign.GetSize();
// 	if ( iPos < -1 || iPos >= iSize )
// 	{
// 		return false;
// 	}
// 
// 	if( -1 == iPos)
// 	{
// 		// �õ���ǰ�ɼ��ڵ��ǰһ���ڵ�����̼�
// 
// 		CNodeData Node0;
// 		m_pNodes->GetAt(0,Node0);
// 
// 		CChartRegion * pRegion = GetParent();
// 		T_MerchNodeUserData * pUserData = NULL;
// 		CIoViewChart * pIoView = pRegion->GetParentIoView();
// 
// 		if (pIoView->IsKindOf(RUNTIME_CLASS(CIoViewKLine)))
// 		{
// 			CIoViewKLine * pIoViewKline = (CIoViewKLine *)pIoView;
// 			pUserData = pIoViewKline->m_MerchParamArray.GetAt(0);
// 			if (NULL == pUserData)
// 			{
// 				return false;
// 			}			
// 		}
// 		else
// 		{
// 			return false;
// 		}
// 
// 		CNodeSequence * pNodesFull = pUserData->pKLineNodesFull;
// 
// 		for (int32 i = 0 ; i< pNodesFull->GetSize(); i++)
// 		{
// 			CNodeData Node ;
// 			pNodesFull->GetAt(i,Node);
// 
// 			if ( Node.m_iID == Node0.m_iID)
// 			{
// 				if ( 0 == i)
// 				{
// 					return false;
// 				}
// 				else
// 				{
// 					CNodeData PreNode0;
// 					pNodesFull->GetAt(i-1,PreNode0);
// 					fValue = PreNode0.m_fClose;
// 
// 					if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByVolumn))
// 					{
// 						fValue = PreNode0.m_fVolume;
// 					}
// 					return true;
// 				}
// 			}
// 		}
// 		return false;
// 	}
//		
// 
// 	CNodeData NodeData;
// 
// 	if ( !m_NodesAlign.GetAt(iPos,NodeData) )
// 	{
// 		return false;
// 	}
// 	if ( CheckFlag(NodeData.m_iFlag,CNodeData::KValueInvalid ))
// 	{
// 		return false;
// 	}
// 	fValue = NodeData.m_fClose;
// 	if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByVolumn))
// 	{
// 		fValue = NodeData.m_fVolume;
// 	}
// 	return true;
}

///////////////////////////////////////////////////////////////////////////


void CChartCurve::SetYMinMax (float fMin, float fMax, bool32 bValid)
{
	m_MinMaxY.m_bValid	= bValid;
	m_MinMaxY.m_fMin	= fMin;
	m_MinMaxY.m_fMax	= fMax;

	if (m_MinMaxY.m_bValid)
	{
		if (IsTooBig(fMax))
		{
			m_MinMaxY.m_bValid = false;
		}
	}

	// ��ʱ��Ȼ�Ѿ�������base
	if ( m_MinMaxY.m_bValid )
	{
		m_AxisYCalcObject.SetCalcParam(m_eAxisYType, fMin, fMax, m_fPriceBase);
	}
	else
	{
		m_AxisYCalcObject.SetCalcParam(m_eAxisYType, 0, -1, m_fPriceBase);
	}
}

bool32 CChartCurve::GetYMinMax ( OUT float& fYMin, OUT float& fYMax )
{	
	if (!m_MinMaxY.m_bValid)
		return false;

	fYMin = m_MinMaxY.m_fMin;
	fYMax = m_MinMaxY.m_fMax;
	return true;
}

bool32 CChartCurve::PriceYToCmpClientY(float fPriceY, OUT int32& iCmpY)
{
	if (fPriceY > m_MinMaxY.m_fMax || fPriceY < m_MinMaxY.m_fMin || m_MinMaxY.m_fMax == m_MinMaxY.m_fMin)
	{
		return false;
	}

	//
	CRect& rtSrc = m_ChartRegion.m_RectCurves;
	int32 iHeight = (int32)((rtSrc.bottom - rtSrc.top) * (fPriceY - m_MinMaxY.m_fMin) / (m_MinMaxY.m_fMax - m_MinMaxY.m_fMin));

	//
	iCmpY = rtSrc.bottom - iHeight;
	m_ChartRegion.ClipY(iCmpY);

	return true;
}

bool32 CChartCurve::RegionYValue(int32 y, float& fValue)
{
	fValue = 0.0f;
	return RegionYToPriceY(y, fValue);	// ����任	
}

bool32 CChartCurve::ValueRegionY(float fy, int32& iValue)
{
	iValue = 0;
	return PriceYToRegionY(fy, iValue);	

}

bool32 CChartCurve::RegionYToAxisY( int32 iRegionY, OUT float &fAxisY )
{
	CRect rect = m_ChartRegion.GetRectCurves();
	rect.OffsetRect(-rect.left, -rect.top + m_ChartRegion.m_iTopSkip);
	return m_AxisYCalcObject.RectYToAxisY(rect, iRegionY, fAxisY);
}

bool32 CChartCurve::AxisYToRegionY( float fAxisY, OUT int32 &iRegionY )
{
	CRect rect = m_ChartRegion.GetRectCurves();
	rect.OffsetRect(-rect.left, -rect.top + m_ChartRegion.m_iTopSkip);
	return m_AxisYCalcObject.AxisYToRectY(rect, fAxisY, iRegionY);
}

bool32 CChartCurve::AxisYToPriceY( float fAxisY, OUT float &fPriceY )
{
	return m_AxisYCalcObject.AxisYToPriceY(fAxisY, fPriceY);
}

bool32 CChartCurve::PriceYToAxisY( float fPriceY, OUT float &fAxisY )
{
	return m_AxisYCalcObject.PriceYToAxisY(fPriceY, fAxisY);
}

bool32 CChartCurve::RegionYToPriceY( int32 iRegionY, OUT float &fPriceY )
{
	CRect rect = m_ChartRegion.GetRectCurves();
	rect.OffsetRect(-rect.left, -rect.top + m_ChartRegion.m_iTopSkip);
	return m_AxisYCalcObject.RectYToPriceY(rect, iRegionY, fPriceY);
}

bool32 CChartCurve::PriceYToRegionY( float fPriceY, OUT int32 &iRegionY )
{
	CRect rect = m_ChartRegion.GetRectCurves();
	rect.OffsetRect(-rect.left, -rect.top + m_ChartRegion.m_iTopSkip);
	return m_AxisYCalcObject.PriceYToRectY(rect, fPriceY, iRegionY);
}

bool32 CChartCurve::ExtendPriceYToRegionY( float fPriceY, OUT int32 &iRegionY )
{
	CRect rect = m_ChartRegion.GetRectCurves();
	rect.OffsetRect(-rect.left, -rect.top + m_ChartRegion.m_iTopSkip);
	return m_AxisYCalcObject.ExtendPriceYToRectY(rect, fPriceY, iRegionY);
}

bool32 CChartCurve::ExtendRegionYToPriceY( int32 iRegionY, OUT float &fPriceY )
{
	CRect rect = m_ChartRegion.GetRectCurves();
	rect.OffsetRect(-rect.left, -rect.top + m_ChartRegion.m_iTopSkip);
	return m_AxisYCalcObject.ExtendRectYToPriceY(rect, iRegionY, fPriceY);
}

void CChartCurve::SetAxisYType( CPriceToAxisYObject::E_AxisYType eType )
{
	m_eAxisYType = eType;
	m_AxisYCalcObject.SetCalcParam(eType, m_AxisYCalcObject.GetAxisYMin(), m_AxisYCalcObject.GetAxisYMax(), m_AxisYCalcObject.GetPriceBaseY());
}

bool32 CChartCurve::GetAxisBaseY( OUT float &fBaseAxisY )
{
	return m_AxisYCalcObject.GetAxisBaseY(fBaseAxisY);
}

void CChartCurve::SetPriceBaseY( float fPriceBaseY, bool32 bUpdateCalcObj/* = true*/ )
{
	//float fMin, fMax;
	m_fPriceBase = fPriceBaseY;
	if ( bUpdateCalcObj )
	{
		m_AxisYCalcObject.SetCalcParam(m_AxisYCalcObject.GetAxisYType(), m_AxisYCalcObject.GetAxisYMin(), m_AxisYCalcObject.GetAxisYMax(), fPriceBaseY);
	}
}

bool32 CChartCurve::GetValidFrontNode( int32 iFrontIndex, OUT CNodeData &node )
{
	CNodeSequence *pNodes = GetNodes();
	if ( NULL == pNodes || iFrontIndex < 0 || iFrontIndex >= pNodes->GetSize() )
	{
		return false;
	}
	const int32 iSize = pNodes->GetSize();
	int32 iFound = 0;
	for ( int32 i=0; i < iSize ; i++ )
	{
		if ( pNodes->GetAt(i, node) && !CheckFlag(node.m_iFlag,CNodeData::KValueInvalid) )
		{
			if ( iFound == iFrontIndex )
			{
				return true;
			}
			++iFound;
		}
	}
	return false;
}

bool32 CChartCurve::GetValidBackNode( int32 iBackIndex, OUT CNodeData &node )
{
	CNodeSequence *pNodes = GetNodes();
	if ( NULL == pNodes || iBackIndex < 0 || iBackIndex >= pNodes->GetSize() )
	{
		return false;
	}
	const int32 iSize = pNodes->GetSize();
	int32 iFound = 0;
	for ( int32 i=iSize-1; i >= 0 ; i-- )
	{
		if ( pNodes->GetAt(i, node) && !CheckFlag(node.m_iFlag, CNodeData::KValueInvalid) )
		{
			if ( iFound == iBackIndex )
			{
				return true;
			}
			++iFound;
		}
	}
	return false;
}

bool32 CChartCurve::ConvertNodeDataToPrice(const CNodeData &NodeData, OUT float &fPrice)
{
	if ( !CheckFlag(NodeData.m_iFlag, CNodeData::KValueInvalid) )
	{
		return false;
	}

	if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByLowHigh) )
	{
		// �޷������ֻ�ܽ��������ߴ���
		return false;
	}
	if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByClose))
	{
		fPrice = NodeData.m_fClose;
		return true;
	}
	if (CheckFlag(m_iFlag, CChartCurve::KYTransformByAvg))
	{
		fPrice = NodeData.m_fAvg;
		return true;
	}
	if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByAmount))
	{
		fPrice = NodeData.m_fAmount;
		return true;
	}
	if ( CheckFlag(m_iFlag,CChartCurve::KYTransformByVolumn))
	{
		fPrice = NodeData.m_fVolume;
		return true;
	}
	return false;
}

bool32 CChartCurve::BeShowTitleText()
{
	if ( NULL == m_pDrawer )
	{
		return false;
	}

	//
	return m_pDrawer->BeDrawTitleText();
}

bool32 CChartCurve::BeNoDraw()
{
	if ( NULL == m_pDrawer )
	{
		return false;
	}

	//
	return m_pDrawer->BeNoDraw();
}
