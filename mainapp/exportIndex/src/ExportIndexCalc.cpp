#include "StdAfx.h"
#include "ExportIndexCalc.h"

//#include "CFormularCompute.h"
#include "CFormularContent.h"
#include "formulaengine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 
namespace
{
	// �̼߳��ͨ���¼� MTP=MainThreadProcess DTP=DispatchThreadProcess
	enum E_ExportIndexCalcEvent
	{
		EEICEIndexChanged = 0,	// ָ�����֪ͨ DTP wP = new T_IndexChangedEvent
		EEICEShowManageDlg,		// ��ʾ��ʽ�Ի��� MTP 
		EEICEGetIndexContent,	// ��ȡ��ʽ���� MTP wP = pwszIndexName lP = &CFormularContent
	};

	struct T_IndexChangedEvent 
	{
		int		eChangeType;
		CString	StrName;
	};
	
	// �����������
	void	ReleaseExpStr(INOUT T_ExportStringW &str)
	{
		delete[] str.pwszString;
		str.pwszString = NULL;
		str.nStringLen = 0;
	}
	
	
	void    CopyCString2ExpStr(IN const CString &strSrc, OUT T_ExportStringW &strDst)
	{
		strDst.nStringLen = strSrc.GetLength();
		ASSERT( strDst.nStringLen >= 0 );
		strDst.pwszString = new wchar_t[strDst.nStringLen+1];
		wcsncpy(strDst.pwszString, strSrc, strDst.nStringLen+1);
	}
	
	void    CopyExportStringW(IN const T_ExportStringW &strSrc, OUT T_ExportStringW &strDst)
	{
		if ( &strDst != &strSrc )
		{
			ReleaseExpStr(strDst);
			strDst.nStringLen = strSrc.nStringLen;
			strDst.pwszString = new wchar_t[strDst.nStringLen+1];
			wcsncpy(strDst.pwszString, strSrc.pwszString, strDst.nStringLen+1);
		}
	}
	
	void	CopyARRAYBE2MyResult(IN const ARRAY_BE &Rline, OUT T_ExportIndexCalcResult &myRes)
	{
		if ( myRes.iLast < Rline.e )
		{
			delete[] myRes.pfLineData;	// ��������line����С�ˣ�����������Դ
			myRes.pfLineData = NULL;
		}
		if ( myRes.nLineWidth < Rline.nLineWidth )
		{
			delete[] myRes.pfLineWidth;
			myRes.pfLineWidth = NULL;
		}
		if ( myRes.nLooseValueCount < Rline.looseArr.GetSize() )
		{
			delete[] myRes.pstLooseValue;
			myRes.pstLooseValue = NULL;
		}
		
		myRes.iFirst	 = Rline.b;
		myRes.iLast		 = Rline.e;
		myRes.eDrawKind	 = (E_ExportLineDrawKind)Rline.kind;
		myRes.nLineWidth = Rline.nLineWidth;
		myRes.isVirtualLine = Rline.isVirtualLine;
		
		CopyCString2ExpStr(Rline.s, myRes.stStrLineName);
		
		if ( NULL == myRes.pfLineData && Rline.e >= Rline.b )
		{
			// ʵ���п��ܲ���Ҫ����~~
			myRes.pfLineData = new float[Rline.e+1];
		}
		if ( NULL != myRes.pfLineData )
		{
			int i=0;
			for ( i=0; i < myRes.iFirst ; i++ )
			{
				myRes.pfLineData[i] = 0.0f;
			}
			for ( ; i <= Rline.e ; i++)
			{
				myRes.pfLineData[i] = Rline.line[i];
			}
		}
		else if (Rline.e >= Rline.b)
		{
			ASSERT( 0 );
			myRes.iLast = myRes.iFirst-1;		// ??�ڴ治��~
		}
		
		ASSERT( Rline.lineWidth == NULL || Rline.nLineWidth == Rline.e + 1 );
		if ( NULL == myRes.pfLineWidth && NULL != Rline.lineWidth )
		{
			myRes.pfLineWidth = new float[Rline.nLineWidth];
		}
		if ( NULL != myRes.pfLineWidth )
		{
			int i=0;
			for ( ; i < Rline.nLineWidth ; i++)
			{
				myRes.pfLineWidth[i] = Rline.lineWidth[i];
			}
		}
		else if ( NULL != Rline.lineWidth )
		{
			ASSERT( 0 );
			myRes.nLineWidth = 0;		// ??�ڴ治��~
		}
		
		if ( NULL == myRes.pstLooseValue && Rline.looseArr.GetSize() > 0 )
		{
			myRes.pstLooseValue = new T_ExportLooseValue[Rline.looseArr.GetSize()];
		}
		if ( NULL != myRes.pstLooseValue )
		{
			int i=0;
			for ( ; i < Rline.looseArr.GetSize() ; i++)
			{
				myRes.pstLooseValue[i].fVal = Rline.looseArr[i].fVal;
				myRes.pstLooseValue[i].nIndexInLine = Rline.looseArr[i].nFoot;
				CopyCString2ExpStr(Rline.looseArr[i].StrExtraData, myRes.pstLooseValue[i].stStrExtraData);
			}
		}
		else if ( Rline.looseArr.GetSize() > 0 )
		{
			ASSERT( 0 );
			myRes.nLooseValueCount = 0;		// ??�ڴ治��~
		}
	}
	
	// �ͷŵ�����Դ
	void ReleaseSingleResultData(INOUT T_ExportIndexCalcResult *pResult)
	{
		if ( NULL == pResult )
		{
			return;
		}
		
		delete[] pResult->pfLineData;
		
		delete[] pResult->pfLineWidth;
		
		
		int i=0;
		for ( i=0; i < pResult->nLooseValueCount ; i++ )
		{
			ReleaseExpStr(pResult->pstLooseValue[i].stStrExtraData);
		}
		delete[] pResult->pstLooseValue;
		
		ReleaseExpStr(pResult->stStrLineName);
		
		pResult->iFirst = 0;
		pResult->iLast = pResult->iFirst-1;
		pResult->nLineWidth = 0;
		pResult->nLooseValueCount = 0;
		pResult->eDrawKind = EELDKNormal;
	}

}

CExportIndexCalc::CExportIndexCalc()
{
	
}

CExportIndexCalc::~CExportIndexCalc()
{
	
}

CExportIndexCalc & CExportIndexCalc::Instance()
{
	static CExportIndexCalc sObj;
	return sObj;
}

bool CExportIndexCalc::InitInstance()
{
	CExportMT2DispatchThread::Instance().AddProcessor(this);

	// �����й�ʽcopyһ��
	CFormulaLib *pLib = CFormulaLib::instance();
	if ( NULL!=pLib )
	{
		RGUARD(LockSingle, m_LockFormula, LockFormula);

		//ϵͳָ��
		POSITION pos = NULL;
		
		for ( pos = pLib->m_SysIndex.m_Names4Index.GetStartPosition(); pos != NULL; )
		{
			T_IndexMapPtr *p = NULL;
			CString StrName;
			pLib->m_SysIndex.m_Names4Index.GetNextAssoc(pos, StrName, (void*&)p);
			if ( NULL == p )
			{
				break;
			}
			
			CFormularContent *pContent = (CFormularContent*)p->p1;
			
			if ( NULL == pContent )
			{
				continue;
			}
			
			CFormularContent forTmp;
			forTmp.Assign(*pContent);
			m_mapFormula[pContent->name] = forTmp;
		}

		for ( pos = pLib->m_ModifyIndex.m_Names4Index.GetStartPosition(); pos != NULL; )
		{
			T_IndexMapPtr *p = NULL;
			CString StrName;
			pLib->m_ModifyIndex.m_Names4Index.GetNextAssoc(pos, StrName, (void*&)p);
			if ( NULL == p )
			{
				break;
			}
			
			CFormularContent *pContent = (CFormularContent*)p->p1;
			
			if ( NULL == pContent )
			{
				continue;
			}
			
			CFormularContentAdp forTmp = *pContent;
			m_mapFormula[pContent->name] = forTmp;
		}

		for ( pos = pLib->m_UserIndex.m_Names4Index.GetStartPosition(); pos != NULL; )
		{
			T_IndexMapPtr *p = NULL;
			CString StrName;
			pLib->m_UserIndex.m_Names4Index.GetNextAssoc(pos, StrName, (void*&)p);
			if ( NULL == p )
			{
				break;
			}
			
			CFormularContent *pContent = (CFormularContent*)p->p1;
			
			if ( NULL == pContent )
			{
				continue;
			}
			
			CFormularContentAdp forTmp = *pContent;
			m_mapFormula[pContent->name] = forTmp;
		}
	}
	else
	{
		ASSERT( 0 );
	}

	return true;
}

void CExportIndexCalc::UnInit()
{
	CExportMT2DispatchThread::Instance().RemoveProcessor(this);
}

void CExportIndexCalc::OnFormulaChanged( int eFormulaChangeType, LPCTSTR ptszName )
{
	ASSERT( NULL!=ptszName );

	m_LockFormula.lock();
	{
		CFormularContent *pContent = CFormulaLib::instance()->GetFomular(ptszName);
		if ( NULL!=pContent )
		{
			m_mapFormula[ptszName] = *pContent;
		}
		else
		{
			ASSERT( 0 );
		}
	}
	m_LockFormula.unlock();

	T_ExportMT2DispatchThreadProcessorLParam *pParam = new T_ExportMT2DispatchThreadProcessorLParam;
	pParam->iEventId = EEICEIndexChanged;

	T_IndexChangedEvent *pEvent = new T_IndexChangedEvent;
	pEvent->eChangeType = eFormulaChangeType;
	pEvent->StrName		= ptszName;

	pParam->wP		= (WPARAM)pEvent;
	pParam->lP		= NULL;
	if ( !CExportMT2DispatchThread::Instance().Post2DispatchThread(this, (LPARAM)pParam) )
	{
		ASSERT( 0 );

		delete pEvent;
		delete pParam;
	}
}

void CExportIndexCalc::AddIndexNotify( iExportIndexCalcNotify *pNotify )
{
	if ( NULL==pNotify )
	{
		ASSERT( 0 );
		return;
	}

	m_LockNotify.lock();
	bool bAdd = false;
	for ( int i=0; i<m_aNotify.size(); ++i )
	{
		NotifyKeeper *pKeeper = m_aNotify[i];
		if ( pNotify==pKeeper->GetPtr() )
		{
			bAdd  = true;
			break;
		}
	}
	if ( !bAdd )
	{
		NotifyKeeper *pKeeper = new NotifyKeeper(pNotify);
		m_aNotify.push_back(pKeeper);
	}
	m_LockNotify.unlock();
}

void CExportIndexCalc::RemoveIndexNotify( iExportIndexCalcNotify *pNotify )
{
	NotifyKeeper *pKeeperFind = NULL;
	m_LockNotify.lock();
	for ( int i=0; i<m_aNotify.size(); ++i )
	{
		NotifyKeeper *pKeeper = m_aNotify[i];
		if ( pNotify==pKeeper->GetPtr() )
		{
			pKeeperFind = pKeeper;
			pKeeperFind->Removing();
			m_aNotify.erase( m_aNotify.begin()+i );			
			break;
		}
	}
	m_LockNotify.unlock();

	if ( NULL!=pKeeperFind )
	{
		while ( !pKeeperFind->CanRemove() )
		{
			Sleep(10);
		}
		pKeeperFind->DecRef();
		delete pKeeperFind;
		pKeeperFind = NULL;
	}
}

void CExportIndexCalc::ShowIndexManagerDlg()
{
	CWnd *pWnd = AfxGetMainWnd();
	if ( NULL!=pWnd )
	{
		pWnd->PostMessage(WM_COMMAND, ID_FORMULA_MAG, NULL);
	}
}

bool CExportIndexCalc::IsIndexExist( const wchar_t *pwszIndexName )
{
	if ( NULL==pwszIndexName )
	{
		ASSERT( 0 );
		return false;
	}

	CFormularContent formula;
	return GetFormulaContent(pwszIndexName, formula);
}

bool CExportIndexCalc::CalcIndex( const wchar_t *pwszIndexName, const T_ExportKLineUnit *pKLine, int iKLineUnitCount, OUT T_ExportIndexCalcResult **ppResult, OUT int &iResultDataCount )
{
	if ( NULL==pwszIndexName || NULL==pKLine || iKLineUnitCount<=0 || NULL==ppResult )
	{
		ASSERT( 0 );
		return false;
	}

	ASSERT( NULL==*ppResult );
	*ppResult = NULL;
	iResultDataCount = 0;

	CFormularContent formula;
	if ( !GetFormulaContent(pwszIndexName, formula) )
	{
		return false;
	}

	CFormularCompute *pEquation = NULL;
	INPUT_PARAM		input = {0};
	input.pIndex = &formula;
	Kline *pKlineCalc = NULL;
	{
		pKlineCalc = new Kline[iKLineUnitCount];
		for ( int32 i=0; i < iKLineUnitCount ; ++i )
		{
			Kline &stDst = pKlineCalc[i];
			const T_ExportKLineUnit &stSrc = pKLine[i];

			stDst.day	= stSrc.lTime;

			stDst.open	= stSrc.fOpen;
			stDst.high	= stSrc.fHigh;
			stDst.low	= stSrc.fLow;
			stDst.close	= stSrc.fClose;

			stDst.vol	= stSrc.fVolume;
			stDst.amount	= stSrc.fAmount;
			stDst.position	= stSrc.fHold;

			stDst.advance	= stSrc.usUpCount;
			stDst.decline	= stSrc.usDownCount;
		}
		input.pp	 = (Kline *)pKlineCalc;		// ʵʱ�ϲ�Ӧ���޸ĵ�
	}
	input.pmax	 = iKLineUnitCount - 1;
	
	if ( ComputeFormu2(pEquation, input) != 0 )
	{
		if ( NULL != pEquation )
		{
			TRACE(_T("ָ��[%s]�������: %s\r\n"), pwszIndexName, pEquation->m_errmsg.GetBuffer());
			delete pEquation;
			pEquation = NULL;
		}
		else
		{
			TRACE(_T("ָ��[%s]�������: δ֪ԭ��\r\n"), pwszIndexName);
		}
		if ( NULL != pKlineCalc )
		{
			delete []pKlineCalc;
			pKlineCalc = NULL;
			input.pp = NULL;
		}
		return 0;
	}
	if ( NULL != pKlineCalc )
	{
		delete []pKlineCalc;
		pKlineCalc = NULL;
		input.pp = NULL;
	}
	
	// ת��������~~
	if ( NULL != pEquation )
	{
		int nResultLine = pEquation->GetRlineNum();
		if ( NULL != ppResult && nResultLine > 0 )
		{
			ASSERT( *ppResult == NULL );	// �������������������ΪNULL�����д��������

			*ppResult = new T_ExportIndexCalcResult[nResultLine];
			iResultDataCount = nResultLine;

			T_ExportIndexCalcResult *pResult = *ppResult;
			ASSERT( NULL != pResult );
			if ( NULL != pResult )
			{
				memset(pResult, 0, sizeof(T_ExportIndexCalcResult)*nResultLine);
				for ( int i=0; i < nResultLine ; i++ )
				{
					// ֱ��͸�����װת��
					ASSERT( i < pEquation->m_RlineNum );
					const ARRAY_BE &Rline = pEquation->m_Rline[i];
					// Rline.s = pEquation->m_RlineName[i];		
					int bl = Rline.b;
					int be = Rline.e;
					if ( bl>be||bl<0||be<0||be>=iKLineUnitCount )
					{
						ASSERT( 0 );		// �������������������ƥ������
						continue;
					}
					
					CopyARRAYBE2MyResult(Rline, pResult[i]);
				}
			}
		}
		delete pEquation;
		pEquation = NULL;
		return nResultLine>0;
	}
	else
	{
		ASSERT( 0 );
	}
	
	return false;
}

void CExportIndexCalc::FreeCalcResultData( T_ExportIndexCalcResult *pResult, int iResultDataCount )
{
	if ( NULL != pResult )
	{
		for ( int i=0; i < iResultDataCount ; i++ )
		{
			ReleaseSingleResultData(pResult+i);
		}
		delete[] pResult;
	}
}

void CExportIndexCalc::DispatchThreadProcess( bool bSucDisp, LPARAM l )
{
	T_ExportMT2DispatchThreadProcessorLParam *pParam = (T_ExportMT2DispatchThreadProcessorLParam *)l;
	if ( NULL==pParam )
	{
		ASSERT( 0 );
		return;
	}

	switch ( pParam->iEventId )
	{
	case EEICEIndexChanged:
		{
			T_IndexChangedEvent *pEvent = (T_IndexChangedEvent *)pParam->wP;
			ASSERT( NULL!=pEvent );
			if ( bSucDisp && NULL!=pEvent )
			{
				// ��keeper����������
				NotifyVector aNotify;

				m_LockNotify.lock();
				{
					for ( int i=0; i<m_aNotify.size(); ++i )
					{
						NotifyKeeper *pKeeper = m_aNotify[i];
						if ( !pKeeper->IsRemoving() )
						{
							pKeeper->AddRef();
							aNotify.push_back(pKeeper);
						}
					}
				}
				m_LockNotify.unlock();

				for ( int i=0; i<aNotify.size(); ++i )
				{
					NotifyKeeper *pKeeper = aNotify[i];
					if ( NULL!=pKeeper->GetPtr() && !pKeeper->IsRemoving() )
					{
						pKeeper->GetPtr()->OnIndexChanged(pEvent->StrName, pEvent->eChangeType);
					}
					pKeeper->DecRef();
				}
				aNotify.clear();

				pParam->bProcessed = true;
			}

			delete pEvent;
			pParam->wP = NULL;
		}
		break;
	default:
		{
			ASSERT( 0 );
		}
		break;
	}

	// new ������
	delete pParam;
}

bool CExportIndexCalc::GetFormulaContent( const wchar_t *pwszIndexName, CFormularContent &formula )
{
	if ( NULL==pwszIndexName )
	{
		ASSERT( 0 );
		return false;
	}

	RGUARD(LockSingle, m_LockFormula, LockFormula);

	String2FormulaMap::iterator itFor = m_mapFormula.find(pwszIndexName);
	if ( itFor!=m_mapFormula.end() )
	{
		formula.Assign( itFor->second );
		return true;
	}

	TRACE(_T("ָ�굼��ģ��: ָ�� [%s] ������!\r\n"), pwszIndexName);
	return false;
}
