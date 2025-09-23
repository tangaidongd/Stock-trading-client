#include "StdAfx.h"

#pragma warning(disable:4786)

#include <ctype.h>
#include "ArbitrageFormula.h"

// �����: ������ ����������Ҳ���������������������������������
// �����㸡��������������������
// ������+-*/����4�������������������������
// �޶�·����
// () �������Ϊ���⴦��

bool32	ArbForOperatorAdd(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
bool32	ArbForOperatorSub(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
bool32	ArbForOperatorMulti(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
bool32	ArbForOperatorDiv(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
bool32	ArbForOperatorAssign(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
bool32	ArbForOperatorComma(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
bool32	ArbForOperatorNegative(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
bool32	ArbForOperatorPositive(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);

// ���˳��Ҫ�������ȼ�����
static const T_ArbForOperator s_arbOps[] = 
{
	{_T("+"), 1, 8, false, ArbForOperatorPositive},
	{_T("-"), 1, 8, false, ArbForOperatorNegative},
	{_T("*"), 2, 6, true, ArbForOperatorMulti},
	{_T("/"), 2, 6, true, ArbForOperatorDiv},
	{_T("+"), 2, 5, true, ArbForOperatorAdd},
	{_T("-"), 2, 5, true, ArbForOperatorSub},
	{_T("="), 2, 2, false, ArbForOperatorAssign},
	{_T(","), 2, 1, true, ArbForOperatorComma},
};

static const int32	s_iArbOpsCount = sizeof(s_arbOps)/sizeof(s_arbOps[0]);

//////////////////////////////////////////////////////////////////////////
//
CArbitrageFormula::OpMap CArbitrageFormula::sm_ops;
ArbitrageVarNameSet		CArbitrageFormula::sm_opFirstCh;

CArbitrageFormula::CArbitrageFormula()
{
	m_pSymbolTree = NULL;
	m_iErrorPos = 0;
	InitStatic();
}

CArbitrageFormula::CArbitrageFormula( LPCTSTR ptszFormula )
{
	m_pSymbolTree = NULL;
	m_iErrorPos = 0;
	InitStatic();
}

CArbitrageFormula::CArbitrageFormula( const CArbitrageFormula &for2 )
{
	m_pSymbolTree = NULL;
	m_iErrorPos = 0;
	InitStatic();
	Copy(for2);
}
//lint --e{1579}
CArbitrageFormula::~CArbitrageFormula()
{
	
	ClearParse();
}

void CArbitrageFormula::InitStatic()
{
	
	if ( sm_ops.empty() )
	{
		for ( int32 i=0; i < s_iArbOpsCount ; ++i )
		{
			const T_ArbForOperator *pOp = s_arbOps+i;
			// ֻ֧��һ����ĸ������� - ������
			//ASSERT( pOp->szOp[0] != _T('\0') && pOp->szOp[1] == _T('\0') );
			ASSERT( pOp->iOpPriority >= ARB_FOR_PRIORITY_MIN && pOp->iOpPriority <= ARB_FOR_PRIORITY_MAX );
			ASSERT( pOp->iParamCount == 1 || pOp->iParamCount == 2 );	// ��֧�����������͵�����
			sm_ops.insert(OpMap::value_type(pOp->szOp, pOp));

			sm_opFirstCh.insert(CString(pOp->szOp[0]));
		}
	}
}

//lint --e{429,593}
bool32 CArbitrageFormula::ParseFormula(OUT T_SymbolTreeNode **ppRootNode)
{
	if ( NULL != ppRootNode )
	{
		ASSERT( NULL == *ppRootNode );	// ����Ҫ��֤����������һ��ָ��յ�ָ��
	}
	if ( m_StrFormula.IsEmpty() )
	{
		return false;
	}

	bool32 bParseOk = true;
	LPCTSTR ptszFormula=m_StrFormula;
	LPCTSTR ptszOrg = ptszFormula;
	CString StrVar;
	int32	i = 0;

	// ��Ϊ
	// ��� 1: �ո� Tab�� ��֧�ֻ��� �ָ�
	// ����� 2: () Ԥ����������   �ָ�
	// ���� 3: 0-9 .
	// ʣ�µĶ��Ǳ��� 4: ���ձ��������������ж�

	// ����������:


	SymbolArray aSyms;
	aSyms.SetSize(0, 100);
	T_Symbol sym;
	sym.eSymType = EST_Count;
	sym.u_Content.pOp = (T_ArbForOperator *)10;
	sym.pszFormulaPos = NULL;

	// ���жϱ���, �����
	SymbolArray aParenthesis;
	aParenthesis.SetSize(0, 40);
	for ( ptszFormula=ptszOrg; *ptszFormula != _T('\0') ; ptszFormula=_tcsinc(ptszFormula) )
	{	
		TCHAR ch = *ptszFormula;

		// �κ��ַ����п�������������Ŀ�ʼ��so���������������̰��
		bool32 bMatchSpe = false;
		for (ArbitrageVarNameSet::reverse_iterator itSpe=m_varSpecialNames.rbegin(); itSpe != m_varSpecialNames.rend() ; ++itSpe)
		{
			LPCTSTR pszSpe = *itSpe;
			int iSpeLen = _tcslen(pszSpe);
			if ( _tcsncmp(pszSpe, ptszFormula, iSpeLen) == 0 )
			{
				// ƥ����������������setĬ��ʹ��С->�����������������ģ�ƥ��������һ����������ʵ�
				// ԭ���ı��������
				// �������ӵ�������жϵ�������ȼ���
				// ����������������ư�������������������ô������������ᱻ�ֿ�Ϊ�����������������һ�����
				if ( !StrVar.IsEmpty() )
				{
					ParseVarOrOp(StrVar, ptszFormula-StrVar.GetLength(), sym);
					aSyms.Add(sym);
					StrVar.Empty();
				}

				// �������������
				sym.eSymType = EST_Var;
				sym.StrVar = pszSpe;
				sym.pszFormulaPos = ptszFormula;
				sym.u_Content.dwFlag = 8;	// ����������
				sym.iMyPriority = 0;
				aSyms.Add(sym);

				ptszFormula += iSpeLen;
				ptszFormula = _tcsdec(ptszOrg, ptszFormula);	// ��continue inc����һ��
				bMatchSpe = true;
				break;
			}
		}
		if ( bMatchSpe )
		{
			ASSERT( StrVar.IsEmpty() );
			continue;	// ƥ���������������ȥƥ����һ����
		}
		
		if ( _T(' ') == ch || _T('\t') == ch || _T('\r') == ch || _T('\n') == ch )
		{
			if ( !StrVar.IsEmpty() )
			{
				ParseVarOrOp(StrVar, ptszFormula-StrVar.GetLength(), sym);
				aSyms.Add(sym);
				StrVar.Empty();
			}
			continue;	// �ָ������ˣ�����������
		}

		if ( _T('(') == ch )
		{
			// ���ŵ������ȼ�
			// ǰ��������������ջ
			if ( !StrVar.IsEmpty() )
			{
				ParseVarOrOp(StrVar, ptszFormula-StrVar.GetLength(), sym);
				aSyms.Add(sym);
				StrVar.Empty();
			}

			// ѹ��һ������symbol, �Ժ����е����㣬�������ȼ��ٵ�Ҳ������뵽���֮��
			sym.eSymType = EST_Count;
			sym.u_Content.dwFlag = 0;
			sym.pszFormulaPos = ptszFormula;
			aSyms.Add(sym);

			aParenthesis.Add(sym);

			continue;
		}
		else if ( _T(')') == ch )
		{
			// ���ŵ������ȼ�����
			// ǰ��������������ջ
			if ( !StrVar.IsEmpty() )
			{
				ParseVarOrOp(StrVar, ptszFormula-StrVar.GetLength(), sym);
				aSyms.Add(sym);
				StrVar.Empty();
			}
			
			// ѹ��һ������symbol, �Ժ����е����㣬��������ŵ���������ڵ�
			sym.eSymType = EST_Count;
			sym.u_Content.dwFlag = 1;
			sym.pszFormulaPos = ptszFormula;
			aSyms.Add(sym);

			if ( aParenthesis.GetSize() <= 0 )
			{
				// ���Ų�ƥ��, ���ü�����
				bParseOk = false;
				CString StrErr;
				StrErr.Format(_T("���� ) �޷�ƥ��"));
				SetLastErrorText(StrErr);
				SetErrorPos(sym.pszFormulaPos - ptszOrg);
				break;
			}
			aParenthesis.RemoveAt(aParenthesis.GetUpperBound());
			
			continue;
		}

		// ʣ�µ�: ����+����� ���� �����+����
		// �������ж�
		// �����������������ʼ����ĸ��ʼ
		CString StrOp;
		StrOp.Format(_T("%c"),ch);

		ArbitrageVarNameSet::iterator itOp = sm_opFirstCh.find(StrOp);
		if ( itOp != sm_opFirstCh.end() )
		{
			// ���ch�������������ʼ��־��������ƥ�����ĸ������
			StrOp.Empty();	// ���¿�ʼƥ�������

			LPCTSTR ptsz1 = ptszFormula;
			while ( _T('\0') != *ptszFormula )
			{
				StrOp += *ptszFormula;
				OpMap::iterator it = sm_ops.find(StrOp);
				if ( it == sm_ops.end() )
				{
					StrOp.Delete(StrOp.GetLength()-1);
					ptszFormula = _tcsdec(ptsz1, ptszFormula);	// ����һ���ַ�
					ASSERT( NULL != ptszFormula );
					break;	// ��ѡ�����ĸƥ����Ǹ������
				}
				ptszFormula = _tcsinc(ptszFormula);
			}
			if ( _T('\0') == *ptszFormula )
			{
				ptszFormula = _tcsdec(ptsz1, ptszFormula);	// ���ڵ��ִ�β�˳����ģ�����Ҫ����һ���ַ�
				ASSERT( NULL != ptszFormula );
			}

			// �³��ֵ��ִ����������ǰ�����Ϊ��������
			if ( !StrVar.IsEmpty() )
			{
				ParseVarOrOp(StrVar, ptsz1-StrVar.GetLength(), sym);
				aSyms.Add(sym);
				StrVar.Empty();
			}
			
			// ������ִ�����
			if ( !StrOp.IsEmpty() )
			{
				ParseVarOrOp(StrOp, ptsz1, sym);
				aSyms.Add(sym);
			}
		}
		else
		{
			// �³��ֵ���ĸ�����������������Ϊ����һ���ְ�
			//unsigned int tch = _tcsnextc(ptszFormula);
			StrVar += ch;
		}

		// �ȴ���һ���ַ��жϰ�
	}

	if ( !StrVar.IsEmpty() )
	{
		ParseVarOrOp(StrVar, ptszFormula-StrVar.GetLength(), sym);
		aSyms.Add(sym);
		StrVar.Empty();
	}

	// ���������
	for ( i=0; i < aSyms.GetSize() ; ++i )
	{
		T_Symbol &tmpSym = aSyms[i];
		if ( EST_Var == tmpSym.eSymType )
		{
			// �����������������򲻼�������ƵĺϷ���
			if ( tmpSym.u_Content.dwFlag == 8 )
			{
				tmpSym.u_Content.dwFlag = 0;	// ���������
				continue;
			}

			ASSERT( !tmpSym.StrVar.IsEmpty() );
			TCHAR chFirst = tmpSym.StrVar[0];
			const TCHAR chDot = _T('.');
			if ( chDot == chFirst || _istdigit(chFirst) )
			{
				bool32 bHasDot = false;
				// �����֣������������������,���һ��С����
				for ( int32 j=0; j < tmpSym.StrVar.GetLength() ; ++j )
				{
					TCHAR ch = tmpSym.StrVar.GetAt(j);
					if ( (bHasDot && ch==chDot)
						|| (!_istdigit(ch) && ch != chDot)
						|| (tmpSym.StrVar.GetLength()==1&&bHasDot) )
					{
						// ���С������߷����ֻ���û��һ�����֣�����
						bParseOk = false;
						ASSERT( tmpSym.pszFormulaPos != NULL );
						SetErrorPos(tmpSym.pszFormulaPos - ptszOrg);
						CString StrError;
						StrError.Format(_T("����Ĳ����������߷Ƿ�������[%s]!"), tmpSym.StrVar.GetBuffer());
						SetLastErrorText(StrError);
						break;
					}
					bHasDot = bHasDot || ch == chDot;
				}
				if ( !bParseOk )
				{
					// �Ѿ������ˣ�����������ı������ж���
					break;
				}

				tmpSym.eSymType = EST_Const;	// ��������
			}
			else
			{
				// ����

				// ���Ǳ���- -
				// ����Ҫÿ��char������ĸ�����ֻ���_������ĸ����Ϊ����
				//lint --e{571}
				for ( LPCTSTR psz=tmpSym.StrVar; *psz != _T('\0') ; psz = _tcsinc(psz) )
				{					
					unsigned int ch = (unsigned int)(_tcsnextc(psz));
					bool32 bOk = (ch>=_T('A') && ch <= _T('Z'))
						|| (ch>=_T('a') && ch <= _T('z'))
						|| (ch>=_T('0') && ch <= _T('9'))
						|| ch == _T('_')
						|| ch > 255;
					// ���ı�Ȼ����255
					if ( !bOk )
					{
						// ���С������߷����ֻ���û��һ�����֣�����
						bParseOk = false;
						ASSERT( tmpSym.pszFormulaPos != NULL );
						SetErrorPos(tmpSym.pszFormulaPos - ptszOrg);
						CString StrError;
						StrError.Format(_T("����Ĳ�������[%s]!"), tmpSym.StrVar.GetBuffer());
						SetLastErrorText(StrError);
						break;
					}
				}
				if ( !bParseOk )
				{
					// �Ѿ������ˣ�����������ı������ж���
					break;
				}
			}
		}
		else if ( EST_Operator == sym.eSymType )
		{
			const T_ArbForOperator *pOp = sym.u_Content.pOp;
			if ( pOp->iParamCount != 1 && pOp->iParamCount != 2 )
			{
				// ��֧��1������2�������������
				bParseOk = false;
				ASSERT( tmpSym.pszFormulaPos != NULL );
				SetErrorPos(tmpSym.pszFormulaPos - ptszOrg);
				CString StrError;
				StrError.Format(_T("��֧�ֵ������[%s]!"), tmpSym.StrVar.GetBuffer());
				SetLastErrorText(StrError);
				break;
			}
		}
	}
	// �������Ų����ҵ������ŵ�����
	if ( bParseOk && aParenthesis.GetSize() != 0 )
	{
		ASSERT( aParenthesis.GetSize() > 0 );
		// ���Ų�ƥ��, ���ü�����
		bParseOk = false;
		CString StrErr;
		SetErrorPos(-1);
		StrErr.Format(_T("���� ( û�к��ʵ� ) ƥ��"));
		SetLastErrorText(StrErr);
		if ( aParenthesis.GetSize() > 0 )
		{
			SetErrorPos(aParenthesis[aParenthesis.GetUpperBound()].pszFormulaPos - ptszOrg);
		}
	}
	
	if ( !bParseOk )
	{
		return false;	// ����Ĺ�ʽ
	}

	// ĳЩ����������ж��(+-)���ܣ���Ҫ��Բ����ı仯ѡ�ú��ʵ�
	// ѡ�ú��ʵ������
	// �������Ҳ²�
	for ( i=0; i < aSyms.GetSize() ; ++i )
	{
		T_Symbol &tmpSym = aSyms[i];
		if ( tmpSym.eSymType == EST_Operator )
		{
			ASSERT( NULL != tmpSym.u_Content.pOp );
			int iOpCount = sm_ops.count(tmpSym.u_Content.pOp->szOp);
			if ( iOpCount > 1 )
			{
				OpMap::_Pairii itRange = sm_ops.equal_range(tmpSym.u_Content.pOp->szOp);
				const T_ArbForOperator *pOld = tmpSym.u_Content.pOp;
				tmpSym.u_Content.pOp = NULL;	// �����
				// �����ȼ��ߵĿ�ʼƥ��
				for ( OpMap::iterator it=itRange.first; it != itRange.second ; ++it )
				{
					const T_ArbForOperator *pOp = it->second;
					ASSERT( NULL != pOp );
					ASSERT( pOp->iParamCount == 1 || pOp->iParamCount == 2 );	// ��֧��<=2��������
					bool32 bMatch = false;
					switch ( pOp->iParamCount )
					{
					case 1: // ��Ŀ�����
						{
							if ( pOp->bLeftToRight )
							{
								// ��Ŀ�������, 
								// ������Ϊһ����
								// ���� �ұ�Ϊ���㿪ʼ�����������һ����Ҫ����������������
								// ��������Ӧ��ѡΪ��Ŀ
								if ( i+1 == aSyms.GetSize()
									|| aSyms[i+1].eSymType == EST_Count
									)
								{
									// ��λ��λ�ڱ��ʽ��������������Ƕ�Ԫ�����
									bMatch = true;
								}
								else if ( i!=0 )
								{
									// ��ǰ���Ҫ��: ��һ���Ǳ��� ���� �ӱ��ʽ���� ���� ��һ������������ϸ�������ܵ�����������
									// �Ժ����Ҫ��: ��һ���ǲ��������Һ�һ��������, �������ں����ǲ�ȷ���ģ�����ֻ��ǰ���жϲ²�
									bool32 bPreMatch = aSyms[i-1].eSymType == EST_Var 
										|| aSyms[i-1].eSymType == EST_Const
										|| (aSyms[i-1].eSymType == EST_Count && aSyms[i-1].u_Content.pOp == (T_ArbForOperator *)1)
										|| (aSyms[i-1].eSymType==EST_Operator && (aSyms[i-1].u_Content.pOp->iParamCount==1&&aSyms[i-1].u_Content.pOp->bLeftToRight))
										;
									bMatch = bPreMatch;
								}
							}
							else
							{
								// ��Ŀ�Ҳ�����
								// ���Ϊ���ʽ��ʼ
								// �����һ����Ҫ�Ҳ������������
								// ��������Ӧ��ѡΪ��Ŀ
								if ( 0 == i
									|| (aSyms[i-1].eSymType == EST_Count&&aSyms[i-1].u_Content.dwFlag==0)
									)
								{
									// ��λ��λ�ڱ��ʽ���㿪ʼ�������Ƕ�Ԫ�����
									bMatch = true;
								}
								else
								{
									// �����Ҫ��������������������
									bool32 bPreMatch = (aSyms[i-1].eSymType==EST_Operator && (aSyms[i-1].u_Content.pOp->iParamCount>1||!aSyms[i-1].u_Content.pOp->bLeftToRight))
										;
									bMatch = bPreMatch;
								}
							}
							
						}
						break;
					case 2: // ˫Ŀ�����
						bMatch = true;	// ���������Ƿ���:)
						break;
					}

					if ( bMatch )
					{
						// �������ȼ����ʵ�ƥ�������
						if ( tmpSym.u_Content.pOp == NULL || tmpSym.u_Content.pOp->iOpPriority < pOp->iOpPriority )
						{
							tmpSym.u_Content.pOp = pOp;
						}
					}
				}

				if ( tmpSym.u_Content.pOp == NULL )
				{
					// û���ҵ�ƥ�������, �����ڽ��������ƥ����
					bParseOk = false;
					CString StrErr;
					StrErr.Format(_T("����� %s �����壬�޷�ƥ��"), pOld->szOp);
					SetLastErrorText(StrErr);
					SetErrorPos(tmpSym.pszFormulaPos - ptszOrg);
					break;
				}

				tmpSym.iMyPriority = tmpSym.u_Content.pOp->iOpPriority;
			}
			else
			{
				ASSERT( 1 == iOpCount );
				tmpSym.iMyPriority = tmpSym.u_Content.pOp->iOpPriority;
			}			
		}
	}
	if ( !bParseOk )
	{
		return false;
	}

	// ���Ľṹ�ǲ�����������ṹ����������˳��ṹ
	// ��Ҫ�����˳����Ҫ���ܹ���ȷ�Ľ���������������
	// ���ȼ���ߵ������ֻ������ͬ���������������������������ȼ���
	// ����������
	
	SymFrameArray	aFrames;

	SymbolNodePtrArray aDels, aSorts;
	int32 iInParenthesisIndex = 0;	// Ĭ����һ������������
	T_SymFrame frameDefault;
	frameDefault.pRoot = frameDefault.pLastVar = frameDefault.pLastWaitOp = NULL;
	aFrames.push_back(frameDefault);

	for ( i=0; i < aSyms.GetSize() ; ++i )
	{
		T_Symbol &tmpSym = aSyms[i];
		T_SymbolTreeNode *pNewNode = new T_SymbolTreeNode;
		pNewNode->pParent = NULL;
		pNewNode->symbol = tmpSym;
		aDels.push_back(pNewNode);

		ASSERT( iInParenthesisIndex == aFrames.size()-1 );
		T_SymFrame *pframe = &aFrames[iInParenthesisIndex]; // ��ǰframe

		if ( tmpSym.eSymType == EST_Const || tmpSym.eSymType == EST_Var )
		{
			if ( !DoParseAddCalcVar(pframe, pNewNode) )
			{
				bParseOk = false;
				break;	// ���ִ���
			}
		}
		else if ( tmpSym.eSymType == EST_Operator )
		{
			// �����
			const T_ArbForOperator *pOp = tmpSym.u_Content.pOp;
			bool32 bEatLeft = false;
			bool32 bWaitRight = false;
			if ( pOp->bLeftToRight )
			{
				// �������
				if ( pOp->iParamCount == 1 )
				{
					// ���Ե����ľͿ�����
					bEatLeft = true;
				}
				else if ( 2 == pOp->iParamCount )
				{
					// �ȳ���࣬�ȴ��Ҳ�
					bEatLeft = true;
					bWaitRight = true;
				}
				else
				{
					////ASSERT(0);	// ��֧�������ģ�ǰ�����жϣ����ﲻӦ�ó���
				}
			}
			else
			{
				// �Ҳ�����
				if ( pOp->iParamCount == 1 )
				{
					// ���ȴ��Ҳ�ľͿ�����
					bWaitRight = true;
				}
				else if ( 2 == pOp->iParamCount )
				{
					// �ȵȴ��Ҳ࣬�Ե����
					bEatLeft = true;
					bWaitRight = true;
				}
				else
				{
					////ASSERT(0);	// ��֧�������ģ�ǰ�����жϣ����ﲻӦ�ó���
				}
			}

			// �������ṹ
			if ( bEatLeft )
			{
				// ���ڵȴ���������ȴҪ�Ե���ߵĲ������������~~
				if ( NULL != pframe->pLastWaitOp )
				{	
					////ASSERT(0);
					bParseOk = false;
					CString StrErr;
					StrErr.Format(_T("����� %s �Ҳ�����������"), pframe->pLastWaitOp->symbol.StrVar.GetBuffer());
					SetLastErrorText(StrErr);
					SetErrorPos(pframe->pLastWaitOp->symbol.pszFormulaPos - ptszOrg);
					break;	// �˳�����ѭ��
				}

				if ( NULL == pframe->pLastVar )
				{
					bParseOk = false;
					CString StrErr;
					StrErr.Format(_T("����� %s �Ҳ�����������"), pOp->szOp);
					SetLastErrorText(StrErr);
					SetErrorPos(tmpSym.pszFormulaPos - ptszOrg);
					break;	// �˳�����ѭ��
				}

				if ( NULL == pframe->pLastVar->pParent )
				{
					// ȡ�������������
					// ��Ȼ������������������ڵ��ʱ��ȻҪΪNULL��������Ǵ���
					T_SymbolTreeNode *pVar = pframe->pLastVar;
					ASSERT( pframe->pRoot == pVar );	// ���ڵ��Ȼָ�������������
					pNewNode->aChilds.push_back(pVar);
					pVar->pParent = pNewNode;
					
					if ( pframe->pRoot == pVar )
					{
						pframe->pRoot = pNewNode;	// ���ȡ����һ�������������޸ĸ�Ϊ�Լ�
					}
				}
				else
				{
					// ȡ�����ı��������Ľṹ��ϵ
					// ���������һ�����������һ��������������Ӧ�ö��ܲ���һ��������ֵ
					T_SymbolTreeNode *pLeft = pframe->pLastVar;
					// ����ҵ����ȼ������ĸ��׸ߣ����������������������Ϊ��������Ĵ��ԣ��˳��ж�
					// ������ȼ�<=���ĸ��ף������ĸ�����Ϊ��������Ĵ��ԣ��ظ��ж�
					T_SymbolTreeNode *pParent  = pLeft->pParent;
					while ( NULL != pParent )
					{
						ASSERT( pParent->symbol.eSymType == EST_Operator );
						// �������ҽ���ԣ��ж�ͬ�����Ȳ��
						bool32 bLessThanMe = pParent->symbol.iMyPriority < tmpSym.iMyPriority;
						if ( !bLessThanMe && pParent->symbol.iMyPriority == tmpSym.iMyPriority )
						{
							// �����ĸ�һ�������ȼ������������������Ϊ������ң�����ͬ���ȼ����ҳԵ��������������
							// �³����������ȻΪ�Ҳ�
							for ( int32 j=0; j < pParent->aChilds.size() ; ++j )
							{
								if ( pParent->aChilds[j] == pLeft )
								{
									if ( pParent->symbol.u_Content.pOp->bLeftToRight )
									{
										// �����, �³��ֵ��������Ȼλ���Ҳ�
										bLessThanMe = false;
										ASSERT( !bLessThanMe );	// Ŀǰ���ֵ��������������������
									}
									else
									{
										// �Ҳ����ȼ���
										bLessThanMe = true;
									}
									break;
								}
							}
						}
						if ( bLessThanMe )
						{
							bool32 bOk = false;
							for ( int32 j=0; j < pParent->aChilds.size() ; ++j )
							{
								if ( pParent->aChilds[j] == pLeft )
								{
									bOk = true;
									pParent->aChilds[j] = pNewNode;	// ����
									pNewNode->pParent = pParent;
									pLeft->pParent = pNewNode;
									pNewNode->aChilds.push_back(pLeft);
								}
							}
							ASSERT( bOk );
							break;
						}
						pLeft = pParent;
						pParent = pParent->pParent;
					}
					
					if ( NULL == pParent )
					{
						// �������ҵͣ�o(����)o �����Ҿ��������
						pLeft->pParent = pNewNode;
						pNewNode->aChilds.push_back(pLeft);

						pframe->pRoot = pNewNode;	// ������ģ��޸ĸ�
					}
					else
					{
						ASSERT( pNewNode->aChilds.size() > 0 && pNewNode->pParent == pParent && pLeft->pParent == pNewNode );
					}
				}
			} // �����
			else
			{
				// ������࣬�����һ��Ҫ�����������ҵ��������������
				if ( NULL == pframe->pLastWaitOp && NULL != pframe->pLastVar )
				{
					bParseOk = false;
					CString StrErr;
					StrErr.Format(_T("���� %s �Ҳ������������"), pframe->pLastVar->symbol.StrVar.GetBuffer());
					SetLastErrorText(StrErr);
					SetErrorPos(pframe->pLastVar->symbol.pszFormulaPos - ptszOrg);
					break; // �˳�����
				}
			}

			if ( bWaitRight )
			{
				if ( NULL == pframe->pLastWaitOp )
				{
					// ������ڻ�û�еȴ��ģ���ʾ���ڵȴ�
					pframe->pLastWaitOp = pNewNode;
				}
				else
				{
					// �Ѿ��еȴ����ˣ���ʾ��ȴ����ң����ڵȴ�(�Ҵ������ڵȴ�)
					// �Ƿ�Ҫ�����ȼ����жϣ�
					T_SymbolTreeNode *pLastWait = pframe->pLastWaitOp;
					pLastWait->aChilds.push_back(pNewNode);
					pNewNode->pParent = pLastWait;
					pframe->pLastWaitOp = pNewNode;
				}
			}
		}
		else if ( tmpSym.eSymType == EST_Count )
		{
			// ���ŵ���
			bool32 bErr = false;
			if ( tmpSym.u_Content.dwFlag == 0 )
			{
				// ( ��ʼ
				// ����һ���¿ո��ڵ㣬��־���ڵ���һ���µ�����
				if ( pframe->pLastVar != NULL && pframe->pLastWaitOp == NULL )
				{
					// ���һ������������û�в��������ĵ���Ҫ���ֵ�()����������²��ܿ�ʼ(�����ǽ����������������������
					bParseOk = false;
					CString StrErr;
					StrErr.Format(_T("( ����ֱ�ӳ����ڱ��������ұ�"));
					SetLastErrorText(StrErr);
					SetErrorPos(tmpSym.pszFormulaPos - ptszOrg);
					break; // �˳�����
				}

				++iInParenthesisIndex;
				if ( iInParenthesisIndex > 0 && iInParenthesisIndex <= 200 ) // ����̫����
				{
					aFrames.push_back(frameDefault);
					pframe = NULL;	// ��ǰ֡�Ѿ����ʧЧ
					if ( iInParenthesisIndex == aFrames.size()-1 )
					{
						// �Ѿ������µĻ�����
					}
					else
					{
						////ASSERT(0);	// ���������)�������ط����ܱ仯
					}
				}
				else
				{
					////ASSERT(0);
					bErr = true;
				}
			}
			else if ( tmpSym.u_Content.dwFlag == 1 )
			{
				// ) ����
				// ������õ��µĿո��ڵ㣬������һ�����������Ĵ���
				if ( NULL == pframe->pRoot )
				{
					// û�м���Ľ��������
					bParseOk = false;
					CString StrErr;
					StrErr.Format(_T(") �����������ܼ�������ݽ��"));
					SetLastErrorText(StrErr);
					SetErrorPos(tmpSym.pszFormulaPos - ptszOrg);
					break; // �˳�����
				}
				else if ( NULL != pframe->pLastWaitOp )
				{
					// �������ȱ�ٲ���������
					bParseOk = false;
					CString StrErr;
					StrErr.Format(_T("����� %s ȱ�ٲ���"), pframe->pLastWaitOp->symbol.StrVar.GetBuffer());
					SetLastErrorText(StrErr);
					SetErrorPos(pframe->pLastWaitOp->symbol.pszFormulaPos - ptszOrg);
					break; // �˳�����
				}
				else if ( NULL != pframe->pLastVar && pframe->pLastVar->pParent == NULL )
				{
					// ������������������������Ȼ��root
					ASSERT( pframe->pLastVar == pframe->pRoot );
				}

				--iInParenthesisIndex;
				if ( iInParenthesisIndex >=0 && iInParenthesisIndex < 200 )
				{
					// ��������������һ��������ֵ����һ��֡�ĵȴ��������������
					T_SymFrame *pPreFrame = &aFrames[iInParenthesisIndex];
					T_SymbolTreeNode *pResultNode = pframe->pRoot;
					ASSERT( pResultNode->pParent == NULL );
					if ( NULL == pPreFrame->pRoot )
					{
						pPreFrame->pRoot = pResultNode;	// ԭ��û�и��ڵ㣬�ǽ���ڵ����ԭ���ĸ��ڵ���
						ASSERT( NULL == pPreFrame->pLastWaitOp && NULL == pPreFrame->pLastVar );
					}

					if ( !DoParseAddCalcVar(pPreFrame, pResultNode) )
					{
						bParseOk = false;
						break;	// ���ִ���
					}

					aFrames.pop_back();
					pframe = NULL;	// ��ǰ֡�Ѿ������ʧЧ
					if ( iInParenthesisIndex == aFrames.size()-1 )
					{
						
					}
					else
					{
						////ASSERT(0);	// �����ط����ܱ�������ֵ
					}
				}
				else
				{
					////ASSERT(0);
					bErr = true;
				}
			}
			else
			{
				bErr = true;
			}

			if ( bErr )
			{
				// �������йصĴ���
				bParseOk = false;
				CString StrErr;
				StrErr.Format(_T("���� %s ���������ܽ����Ĵ���"), (LPCTSTR)tmpSym.StrVar);
				SetLastErrorText(StrErr);
				SetErrorPos(tmpSym.pszFormulaPos - ptszOrg);
				break; // �˳�����
			}
		}
		else
		{
			////ASSERT(0);
			// ������ʶ�������
		}

		if ( tmpSym.eSymType == EST_Const || tmpSym.eSymType == EST_Var || tmpSym.eSymType == EST_Operator )
		{
			if (NULL != pframe && NULL == pframe->pRoot )
			{
				pframe->pRoot = pNewNode;	// û�и�����ô������������������������ĸ���
			}
		}
	}
	if ( bParseOk )
	{
		if ( aFrames.size() != 1 )
		{
			////ASSERT(0);
			bParseOk = false;
			CString StrErr;
			StrErr.Format(_T("() �Բ�����������"));
			SetLastErrorText(StrErr);
			SetErrorPos(-1);
		}
		else if ( aFrames[0].pRoot == NULL )
		{
			////ASSERT(0);
			bParseOk = false;
			CString StrErr;
			StrErr.Format(_T("û�п��Խ���������"));
			SetLastErrorText(StrErr);
			SetErrorPos(-1);
		}
		else if ( aFrames[0].pLastVar == NULL )
		{
			////ASSERT(0);
			bParseOk = false;
			CString StrErr;
			StrErr.Format(_T("û�п��Խ����Ĳ���"));
			SetLastErrorText(StrErr);
			SetErrorPos(-1);
		}
		else if ( aFrames[0].pLastVar->pParent == NULL && aFrames[0].pLastVar != aFrames[0].pRoot )
		{
			////ASSERT(0);
			bParseOk = false;
			CString StrErr;
			StrErr.Format(_T("���� %s ȱ�������"), aFrames[0].pLastVar->symbol.StrVar.GetBuffer());
			SetLastErrorText(StrErr);
			SetErrorPos(aFrames[0].pLastVar->symbol.pszFormulaPos - ptszOrg);
		}
		else if ( aFrames[0].pLastWaitOp != NULL )
		{
			////ASSERT(0);
			bParseOk = false;
			CString StrErr;
			StrErr.Format(_T("����� %s ȱ�ٲ���"), aFrames[0].pLastWaitOp->symbol.StrVar.GetBuffer());
			SetLastErrorText(StrErr);
			SetErrorPos(aFrames[0].pLastWaitOp->symbol.pszFormulaPos - ptszOrg);
		}
		else
		{
			// ���з���Ľڵ㶼Ӧ�ÿ������ݵ��� - ���˷�����������
			for (  i=0; i < aDels.size() ; ++i )
			{
				T_SymbolTreeNode *pNode = aDels[i];
				if ( NULL!=pNode 
					&& pNode->symbol.eSymType != EST_Operator 
					&& pNode->symbol.eSymType != EST_Var
					&& pNode->symbol.eSymType != EST_Const )
				{
					ASSERT( pNode->symbol.eSymType == EST_Count );
					continue;
				}
				ASSERT( NULL != pNode );
				T_SymbolTreeNode *pParent = pNode;
				while ( NULL != pParent && NULL != pNode )
				{
					pNode = pParent;
					pParent = pParent->pParent;
				}
				if ( pNode != aFrames[0].pRoot )
				{
					// �нڵ�û����Ƕ��ȥ����Ȼ��û��Ԥ�ϵ��Ĵ�������:)
					bParseOk = false;
					////ASSERT(0);
					ASSERT( pNode != NULL ); // ��ʱpNodeָ���������
					if ( NULL == pNode )
					{
						////ASSERT(0);
						
						CString StrErr;
						StrErr.Format(_T("δ֪�Ľ�������"));
						SetLastErrorText(StrErr);
						SetErrorPos(-1);
					}
					else
					{

						CString StrErr;
						StrErr.Format(_T("δ֪�Ľ�������"));
						switch (pNode->symbol.eSymType)
						{
						case EST_Var:
						case EST_Const:
							StrErr.Format(_T("���� %s ȱ�����������"), pNode->symbol.StrVar.GetBuffer());
							break;
						case EST_Operator:
							StrVar.Format(_T("����� %s ȱ���������"), pNode->symbol.StrVar.GetBuffer());
							break;
						default:
							////ASSERT(0);
							break;
						}
						SetLastErrorText(StrErr);
						SetErrorPos( pNode->symbol.pszFormulaPos-ptszOrg );
					}
					break;
				}
			}
		}
	}
	if ( !bParseOk )
	{
		// �ͷ�û�г���������
		for ( int32 j=0; j < aDels.size() ; ++j )
		{
			delete aDels[j];
		}
		aDels.clear();
		return false;
	}

	T_SymbolTreeNode *pRoot = aFrames[0].pRoot;
	//DumpNode(pRoot, 0);

	// ��ʽ���������
	if ( NULL != ppRootNode )
	{
		ASSERT( NULL == *ppRootNode );	// ����Ҫ��֤����������һ��ָ��յ�ָ��
		*ppRootNode = pRoot;	// ����ȥ
	}
	else
	{
		FreeSymTreeNode(pRoot);	// û���������Ȼֻ���ͷſ�
	}
	return true;
}

bool32 CArbitrageFormula::ParseFormula()
{
	ClearParse();
	if (ParseFormula(&m_pSymbolTree))
	{
		VisitTree2(m_pSymbolTree, m_aCalcStack);	// ��������ջ��Ϣ
		return true;
	}
	return false;
}

void CArbitrageFormula::ParseVarOrOp( const CString &StrVar, LPCTSTR ptszFomulaCur, OUT T_Symbol &sym )
{
	ASSERT( !StrVar.IsEmpty() && NULL != ptszFomulaCur );
	OpMap::iterator it = sm_ops.find(StrVar);
	if ( it != sm_ops.end() )
	{
		// ����������п�����ƥ������ģ��п���ƥ��ͬ�������������
		sym.eSymType = EST_Operator;
		sym.u_Content.pOp = it->second;
		sym.pszFormulaPos = ptszFomulaCur;
		sym.StrVar = StrVar;
		sym.iMyPriority = sym.u_Content.pOp->iOpPriority;
	}
	else
	{
		sym.eSymType = EST_Var;
		sym.StrVar = StrVar;
		sym.pszFormulaPos = ptszFomulaCur;
		sym.u_Content.dwFlag = 0;
		sym.iMyPriority = 0;
	}
}

void CArbitrageFormula::SetFormula( LPCTSTR ptszFormula )
{
	CString StrOld = m_StrFormula;
	m_StrFormula = ptszFormula;
	if ( StrOld.CollateNoCase(m_StrFormula) != 0 )
	{
		SetErrorPos(-1);
		SetLastErrorText(_T(""));
		ClearParse();
		ASSERT( NULL == m_pSymbolTree );
	}
}

void CArbitrageFormula::FreeSymTreeNode( INOUT T_SymbolTreeNode *&pRootNode )
{
	if ( NULL == pRootNode )
	{
		return;
	}
	
	SymbolNodePtrArray aStack;
	VisitTree2(pRootNode, aStack);
	for ( int i=0; i < aStack.size() ; ++i )
	{
		delete aStack[i];
	}
	aStack.clear();

	pRootNode = NULL;
}

void CArbitrageFormula::VisitTree( IN T_SymbolTreeNode *pRootNode, OUT SymbolNodePtrArray &aNodes )
{
	if ( NULL == pRootNode )
	{
		return;
	}

	// ������� ���ҡ�����
	for ( int i=0; i < pRootNode->aChilds.size() ; ++i )
	{
		VisitTree(pRootNode->aChilds[i], aNodes);
	}

	aNodes.push_back(pRootNode);
	//DumpSymbol(pRootNode->symbol);
}

void CArbitrageFormula::VisitTree2( IN T_SymbolTreeNode *pRootNode, OUT SymbolNodePtrArray &aNodes )
{
	aNodes.clear();
	aNodes.reserve(100);
	if ( NULL == pRootNode )
	{
		////ASSERT(0);
		return;
	}
	// ������� ���ҡ�����
	SymbolNodePtrArray aStack;
	typedef map<T_SymbolTreeNode *, int> NodeVisitCountMap;
	NodeVisitCountMap mapCount;
	aStack.reserve(100);
	T_SymbolTreeNode *pNode = pRootNode;
	while ( NULL != pNode )
	{
		// ���ӽڵ㣬�ȱ��������ӽڵ�
		while ( NULL != pNode )
		{
			NodeVisitCountMap::iterator it = mapCount.find(pNode);
			// ��һ�η��ʣ�������ڵ㣬������нڵ㶼�������ˣ�����Լ�
			if ( it == mapCount.end() )
			{
				it = mapCount.insert(NodeVisitCountMap::value_type(pNode, 0)).first;
			}

			if ( it->second < pNode->aChilds.size() )
			{
				// ����Ҫ���ʵ�n���ӽڵ���
				aStack.push_back(pNode);
				pNode = pNode->aChilds[it->second];
				it->second++;	// �´η�����һ��
			}
			else
			{
				// �����ӽڵ㶼��������
				// �ֵ�����Լ���
				ASSERT( it->second == pNode->aChilds.size() );
				break;
			}
		}

		// �ڵ����
		aNodes.push_back(pNode);	// 

		// ����Ҫ���ʵ���һ���ڵ�

		if ( aStack.empty() )
		{
			break;
		}
		else
		{
			pNode = aStack.back();
			aStack.pop_back();
		}
	}
}

void CArbitrageFormula::DumpCalcStep()
{
#ifdef _DEBUG
	TRACE(_T("ArbitrageFormula: %p\r\n"), this);
	TRACE(_T("  Formula: %s\r\n"), m_StrFormula.GetBuffer());
	TRACE(_T("  Error: %s\r\n"), CString(GetLastErrorText()).GetBuffer());
	TRACE(_T("  ErrPos: %d\r\n"), GetErrorPos());
	TRACE(_T("  �������(%d): "), m_varSpecialNames.size());
	for ( ArbitrageVarNameSet::reverse_iterator itSpe=m_varSpecialNames.rbegin(); itSpe != m_varSpecialNames.rend() ; ++itSpe )
	{
		TRACE(_T("%s "), (*itSpe).GetBuffer());
	}
	TRACE(_T("\r\n"));
	if ( !IsParsed() )
	{
		TRACE(_T("  δ����\r\n"));
	}
	else
	{
		SymbolNodePtrArray aTrack;
		VisitTree2(m_pSymbolTree, aTrack);
		int i;
		for ( i=0; i < aTrack.size() ; ++i )
		{
			T_SymbolTreeNode *pNode = aTrack[i];
			ASSERT( NULL != pNode );
			//DumpSymbol(pNode->symbol);
		}
		TRACE(_T("���㲽��:\r\n"));
		int iStep = 0;
		SymbolArray aVarSyms;
		T_SymbolTreeNode nodeResult;
		nodeResult.symbol.eSymType = EST_Var;
		nodeResult.symbol.iMyPriority = 0;
		nodeResult.symbol.pszFormulaPos = NULL;
		nodeResult.symbol.StrVar = _T("?");
		for ( i=0; i < aTrack.size() ; ++i )
		{
			T_SymbolTreeNode *pNode = aTrack[i];
			if ( pNode->symbol.eSymType == EST_Var || pNode->symbol.eSymType == EST_Const )
			{
				aVarSyms.Add(pNode->symbol);	// �������β
			}
			else if ( pNode->symbol.eSymType == EST_Operator )
			{
				// ���ò�������һԪ���Ƕ�Ԫ������
				// �����ض����������ݣ�ѹ��һ������				
				const T_ArbForOperator *pOp = pNode->symbol.u_Content.pOp;
				TRACE(_T("  ����%d.  ������:%s ����: "), iStep, pOp->szOp);
				int32 iPos = aVarSyms.GetSize() - pOp->iParamCount;
				iPos = max(0, iPos);
				for ( ; iPos < aVarSyms.GetSize() ; ++iPos )
				{
					T_Symbol &sym = aVarSyms[iPos];
					TRACE(_T("%s "), sym.StrVar.GetBuffer());
				}

				if ( aVarSyms.GetSize() < pOp->iParamCount )
				{
					////ASSERT(0);
					TRACE(_T("�޷����м��㣡\r\n"));
					break;
				}

				aVarSyms.RemoveAt(aVarSyms.GetSize()-pOp->iParamCount, pOp->iParamCount);

				// ѹ�������
				nodeResult.symbol.StrVar.Format(_T("?%d"), iStep);
				aVarSyms.Add(nodeResult.symbol);
				TRACE(_T(" ���: %s\r\n"), nodeResult.symbol.StrVar.GetBuffer());

				iStep++;
			}
			else
			{
				////ASSERT(0);	// ��Ӧ����������
				TRACE(_T("��������ͣ�\r\n"));
				DumpSymbol(pNode->symbol);
				break;
			}
		}
		ASSERT( aVarSyms.GetSize()==1 && (aVarSyms[0].StrVar==nodeResult.symbol.StrVar || (aTrack.size()==1 && aTrack[0]->symbol.pszFormulaPos==aVarSyms[0].pszFormulaPos)) );
		if ( aVarSyms.GetSize() == 1 )
		{
			TRACE(_T("���ս��: %s\r\n"), iStep==0?aVarSyms[0].StrVar.GetBuffer() : nodeResult.symbol.StrVar.GetBuffer());
		}
	}
#endif
}

bool32 CArbitrageFormula::IsParsed()
{
	return NULL != m_pSymbolTree;
}

void CArbitrageFormula::DumpSymbol( const T_Symbol &symbol )
{
#ifdef _DEBUG
	int32 iPos = symbol.pszFormulaPos-m_StrFormula.operator LPCTSTR();
	switch ( symbol.eSymType )
	{
	case EST_Const:
		TRACE(_T(" ����-����:%s λ��:%d ���ȼ�:%d\r\n"), CString(symbol.StrVar).GetBuffer(), iPos, symbol.iMyPriority);
		break;
	case EST_Var:
		TRACE(_T(" ����-����:%s λ��:%d ���ȼ�:%d\r\n"), CString(symbol.StrVar).GetBuffer(), iPos, symbol.iMyPriority);
		break;
	case EST_Operator:
		TRACE(_T(" ����-�����:%s λ��:%d ���ȼ�:%d\r\n"), CString(symbol.StrVar).GetBuffer(), iPos, symbol.iMyPriority);
		break;
	case EST_TmpValue:
		TRACE(_T(" ����-��ʱֵ:%s ��ʱ����:%d λ��:%d ���ȼ�:%d\r\n"), symbol.u_Content.iTmpIndex, CString(symbol.StrVar).GetBuffer(), iPos, symbol.iMyPriority);
		break;
	case EST_Count:
		if ( symbol.u_Content.dwFlag == 0 )
		{
			TRACE(_T(" ���(:%s λ��:%d \r\n"), CString(symbol.StrVar).GetBuffer(), iPos);
		}
		else if ( symbol.u_Content.dwFlag == 1 )
		{
			TRACE(_T(" ���):%s λ��:%d \r\n"), CString(symbol.StrVar).GetBuffer(), iPos);
		}
		else
		{
			TRACE(_T(" �޷�ʶ����:%s λ��:%d \r\n"), CString(symbol.StrVar).GetBuffer(), iPos);
		}
		break;
	default:
		////ASSERT(0);
		break;
	}
#endif
}

void CArbitrageFormula::ClearParse()
{
	FreeSymTreeNode(m_pSymbolTree);
	ASSERT( NULL == m_pSymbolTree );
	m_aCalcStack.clear();
	ArbitrageCalcVarPtrs::iterator it;
	for ( it=m_mapConstVar.begin(); it != m_mapConstVar.end() ; ++it )
	{
		delete (CArbitrageFormulaVariableAbs*)it->second;		
	}
	m_mapConstVar.clear();
}

void CArbitrageFormula::DumpNode( const T_SymbolTreeNode *pRootNode, int32 iHeaderEmpty)
{
	if ( NULL == pRootNode )
	{
		return;
	}

	TRACE(_T("%d�� �ڵ� %p, �ӽڵ�: %d\r\n"), iHeaderEmpty, pRootNode, pRootNode->aChilds.size());
	DumpSymbol(pRootNode->symbol);
	++iHeaderEmpty;
	for ( int32 i=0; i < pRootNode->aChilds.size() ; ++i )
	{
		DumpNode(pRootNode->aChilds[i], iHeaderEmpty);
	}
}

bool32 CArbitrageFormula::DoParseAddCalcVar( T_SymFrame *pframe, T_SymbolTreeNode *pNewNode )
{
	// 
	if (NULL == pframe || NULL == pNewNode)
	{
		return false;
	}

	if (NULL == pNewNode->pParent)
	{
		return false;
	}
	
	bool32 bParseOk = true;
	T_Symbol &sym = pNewNode->symbol;
	LPCTSTR ptszOrg = m_StrFormula.operator LPCTSTR();
	if ( pframe->pLastWaitOp != NULL )
	{
		// �еȴ��Ҳ�ļ��㣬��ôǰ������б��������������Ҫ������
		if ( NULL != pframe->pLastVar )
		{
			if ( NULL == pframe->pLastVar->pParent )
			{
				// û�б��Ե���������������ֻ�Ժ���ģ�����
				bParseOk = false;
				CString StrErr;
				if ( pframe->pLastVar->symbol.eSymType == EST_Const || pframe->pLastVar->symbol.eSymType == EST_Var )
				{
					StrErr.Format(_T("���� %s ȱ�����������"), pframe->pLastVar->symbol.StrVar.GetBuffer());
					SetErrorPos(pframe->pLastVar->symbol.pszFormulaPos - ptszOrg);
				}
				else
				{
					StrErr.Format(_T("���� %s ����һ���������ȱ�����������"), sym.StrVar.GetBuffer());
					SetErrorPos(sym.pszFormulaPos - ptszOrg);
				}
				SetLastErrorText(StrErr);
				return bParseOk; // �˳�����
			}
		}
		ASSERT( pframe->pLastWaitOp->symbol.eSymType == EST_Operator );
		pframe->pLastWaitOp->aChilds.push_back(pNewNode);
		pNewNode->pParent = pframe->pLastWaitOp;
		if ( pframe->pLastWaitOp->aChilds.size() == pframe->pLastWaitOp->symbol.u_Content.pOp->iParamCount )
		{
			pframe->pLastWaitOp = NULL;	// ��������ok��
		}
		else
		{
			// ��ȱ���߳����ˣ���֧�ֳ���1�������ĵȴ������˳�
			////ASSERT(0);
			bParseOk = false;
			CString StrErr;
			StrErr.Format(_T("���ܴ��������� %s"), pframe->pLastWaitOp->symbol.StrVar.GetBuffer());
			SetLastErrorText(StrErr);
			SetErrorPos(pframe->pLastWaitOp->symbol.pszFormulaPos - ptszOrg);
			return bParseOk; // �˳�����
		}
		pframe->pLastVar = pNewNode;	// ����������Ҫ����������ȥ����
	}
	else
	{
		// û�����ȴ��ģ�
		if ( NULL == pframe->pLastVar )
		{
			pframe->pLastVar = pNewNode;	// ��ô��������������Ĳ���
		}
		else
		{
			// ��ǰ�б�����û�еȴ���op��������������û�б�����(���㱻���ˣ�Ҳû���������������������ϵ)�������������һ������
			// ͬframe����������֮��û��������Ǵ����
			////ASSERT(0);
			bParseOk = false;
			CString StrErr;
			StrErr.Format(_T("���� %s �� ���� %s ֮��ȱ�������"), sym.StrVar.GetBuffer(), pframe->pLastVar->symbol.StrVar.GetBuffer());
			SetLastErrorText(StrErr);
			SetErrorPos(sym.pszFormulaPos - ptszOrg);
			return bParseOk; // �˳�����
		}
	}
	return bParseOk;
}

void CArbitrageFormula::Copy( const CArbitrageFormula &formula )
{
	if ( this != &formula )
	{
		m_StrFormula = formula.m_StrFormula;
		m_VarLast	 = formula.m_VarLast;
		m_StrLastError	= formula.m_StrLastError;
		m_iErrorPos		= formula.m_iErrorPos;

		m_varSpecialNames = formula.m_varSpecialNames;

		ClearParse();
		ASSERT( NULL == m_pSymbolTree );
		ASSERT( m_aCalcStack.size() == 0 && m_mapConstVar.size() == 0 );

		CopySymTreeNode(m_pSymbolTree, formula.m_pSymbolTree);
		if ( NULL != m_pSymbolTree )
		{
			VisitTree2(m_pSymbolTree, m_aCalcStack);
		}
		// ��ʱ���б���node��pvarӦ����ΪNULL

		for ( ArbitrageCalcVarPtrs::const_iterator it2=formula.m_mapConstVar.begin(); it2 != formula.m_mapConstVar.end() ; ++it2 )
		{
			CArbitrageFormulaVariableConst *pConst = new CArbitrageFormulaVariableConst(it2->first);
			CArbitrageFormulaVariableConst *pOther = (CArbitrageFormulaVariableConst *)it2->second;
			*pConst = *pOther;
			m_mapConstVar.insert(ArbitrageCalcVarPtrs::value_type(it2->first, pConst));
		}
	}
}

void CArbitrageFormula::CopySymTreeNode( INOUT T_SymbolTreeNode *&pRootDst, IN T_SymbolTreeNode *pRootSrc )
{
	if ( NULL != pRootDst )
	{
		FreeSymTreeNode(pRootDst);
	}
	ASSERT( NULL == pRootDst );
	if ( NULL == pRootSrc )
	{
		return;
	}

	SymbolNodePtrArray aStackDst;
	SymbolNodePtrArray aStackSrc;
	aStackSrc.reserve(100);
	aStackDst.reserve(100);
	
	pRootDst = new T_SymbolTreeNode;
	pRootDst->symbol = pRootSrc->symbol;
	if ( pRootDst->symbol.eSymType == EST_Const )
	{
		pRootDst->symbol.u_Content.pVar = NULL;	// ָ���ر��ָ�벻copy
	}
	pRootDst->pParent = NULL;

	aStackSrc.push_back(pRootSrc);
	aStackDst.push_back(pRootDst);
	while ( !aStackSrc.empty() )
	{
		ASSERT( aStackDst.size() == aStackSrc.size() );
		const T_SymbolTreeNode *pSrc = aStackSrc.back();
		T_SymbolTreeNode *pDst = aStackDst.back();
		
		if ( pDst->aChilds.size() < pSrc->aChilds.size() )
		{
			T_SymbolTreeNode *pSubSrc = pSrc->aChilds[pDst->aChilds.size()];
			T_SymbolTreeNode *pSubDst = new T_SymbolTreeNode;
			pSubDst->symbol = pSubSrc->symbol;
			if ( pSubDst->symbol.eSymType == EST_Const )
			{
				pSubDst->symbol.u_Content.pVar = NULL;	// ָ���ر��ָ�벻copy
			}
			pSubDst->pParent = pDst;
			pDst->aChilds.push_back(pSubDst);

			// ��ʱ���ڵ㻹������
			aStackSrc.push_back(pSubSrc);
			aStackDst.push_back(pSubDst);
		}
		else
		{
			ASSERT( pDst->aChilds.size() == pSrc->aChilds.size() );
			aStackSrc.pop_back();	// �Ѿ���ɣ�ȡ�߸��ڵ�
			aStackDst.pop_back();
		}
	}

	SymbolNodePtrArray aTestDst, aTestSrc;
	VisitTree2(pRootDst, aTestDst);
	VisitTree2(pRootSrc, aTestSrc);
	if ( aTestSrc.size() != aTestDst.size() )
	{
		////ASSERT(0);
	}
	else
	{
		ASSERT( aTestDst[0]->symbol.pszFormulaPos == aTestSrc[0]->symbol.pszFormulaPos );
	}
}

bool32 CArbitrageFormula::TestFormula()
{
	if ( !IsParsed() )
	{
		return ParseFormula();
	}
	return true;	// ��Ȼ�н������ݣ����ܳɹ�����
}

bool32 CArbitrageFormula::TestFormula( IN const ArbitrageVarPtrs &vars )
{
	if ( !TestFormula() )
	{
		return false;
	}

	ASSERT( NULL != m_pSymbolTree );
	ASSERT( !m_aCalcStack.empty() );
	for ( int i=0; i < m_aCalcStack.size() ; ++i )
	{
		T_SymbolTreeNode *pNode = m_aCalcStack[i];
		if (NULL == pNode)
		{
			continue;
		}
		if ( pNode->symbol.eSymType == EST_Var )
		{
			ArbitrageVarPtrs::const_iterator it = vars.find(pNode->symbol.StrVar);
			if ( it == vars.end() || it->second == NULL )
			{
				CString StrErr;
				StrErr.Format(_T("û���ṩ���� %s"), pNode->symbol.StrVar.GetBuffer());
				SetLastErrorText(StrErr);
				SetErrorPos(pNode->symbol.pszFormulaPos - m_StrFormula.operator LPCTSTR());
				return false;	// û���ṩ�����������֪����ô����
			}
		}
	}
	return true;
}

bool32 CArbitrageFormula::Calculate( INOUT ArbitrageVarPtrs &vars, int32 iMinIndex, int32 iMaxIndex )
{
	m_VarLast.m_aValues.clear();
	m_VarLast.SetCurIndex(-1);

	if ( iMinIndex > iMaxIndex || iMinIndex < 0 )
	{
		SetErrorPos(-1);
		CString StrErr;
		StrErr.Format(_T("��Ч���㷶Χ[%d-%d]"), iMinIndex, iMaxIndex);
		SetLastErrorText(StrErr);
		return false;
	}

	if ( !TestFormula(vars) )
	{
		return false;
	}

	ASSERT( NULL != m_pSymbolTree && m_aCalcStack.size() > 0 );
	// ���͸�����
	// ��������ָ��ŵ�symbol��
	for ( int32 i=0; i < m_aCalcStack.size() ; ++i )
	{
		T_SymbolTreeNode *pNode = m_aCalcStack[i];
		if (NULL == pNode)
		{
			continue;
		}
		if ( pNode->symbol.eSymType == EST_Const )
		{
			ArbitrageCalcVarPtrs::iterator it = m_mapConstVar.find(pNode->symbol.StrVar);
			if ( it == m_mapConstVar.end() )
			{
				// û�г�ʼ���ó�����Ϣ�������ʼ��
				CArbitrageFormulaVariableConst *pVar = new CArbitrageFormulaVariableConst(pNode->symbol.StrVar);
				TCHAR *p;
				pVar->m_val = (float)_tcstod(pNode->symbol.StrVar, &p);	// �����������Чû
				it = m_mapConstVar.insert(ArbitrageCalcVarPtrs::value_type(pNode->symbol.StrVar, pVar)).first;
			}
			CArbitrageFormulaVariableAbs *pVar = it->second;
			ASSERT( NULL != pVar );
			ASSERT( pNode->symbol.u_Content.pVar == NULL || pNode->symbol.u_Content.pVar == pVar );	// ��Ȼͬʱ��ʧ
			pNode->symbol.u_Content.pVar = pVar;
		}
		else if ( pNode->symbol.eSymType == EST_Var )
		{
			// �����ɲ����ṩ�������������
			ArbitrageVarPtrs::iterator it = vars.find(pNode->symbol.StrVar);
			if ( it == vars.end() || NULL == it->second )
			{
				// û�������
				CString StrErr;
				StrErr.Format(_T("����δ��ʼ�� %s"), pNode->symbol.StrVar.GetBuffer());
				SetLastErrorText(StrErr);
				SetErrorPos(pNode->symbol.pszFormulaPos - m_StrFormula.operator LPCTSTR());
				return false;
			}
			CArbitrageFormulaVariableAbs *pVar = it->second;
			ASSERT( NULL != pVar );
			pNode->symbol.u_Content.pVar = pVar;
		}
	}

	m_VarLast.m_aValues.reserve(iMaxIndex+1);
	
	// ���ռ���ջ���㣬����ջΪ�������У�����������ߵ����Ҳ�����
	// ˳��ΪcalcStack 0..n ������ ������ �����
	// Ĭ�ϷǷ����Ϊ ARB_INVALID_VALUE
	SymbolArray	aResultStack;
	aResultStack.SetSize(0, m_aCalcStack.size());
#ifdef _DEBUGARB
	TRACE(_T("������ʽ����1: %s ����[%d-%d]\r\n"), m_StrFormula, iMinIndex, iMaxIndex);
#endif
	T_Symbol symTmp;
	symTmp.eSymType = EST_TmpValue;
	symTmp.u_Content.iTmpIndex = -1;

	ASSERT( m_VarLast.m_aValues.empty() );
	m_VarLast.m_aValues.insert(m_VarLast.m_aValues.end(), iMaxIndex+1, ARB_INVALID_VALUE);
	m_VarLast.m_iMax = iMinIndex;
	m_VarLast.m_iMax = iMaxIndex;

	bool32 bErr = false;
	typedef vector<CArbitrageFormulaVariableVar> VarArray;
	VarArray aTmpVar;	// ��ʱ�����
	aTmpVar.reserve(20);
	T_ArbForOperatorParam param = {0};
	param.iMinIndex = 0;	// ��ǰ����
	param.iMaxIndex = -1;

	try
	{
		for ( int32 iIndex=iMinIndex; iIndex <= iMaxIndex && !bErr ; ++iIndex )
		{
			int32 iStep = 0;
#ifdef _DEBUGARB
			if ( iIndex-iMinIndex < 3 )
			{
				TRACE(_T("  ����%d �������:\r\n"), iIndex);
			}
#endif
			// ���¿�ʼ������һ��
			aResultStack.SetSize(0);
			
			m_VarLast.SetValue(iIndex, ARB_INVALID_VALUE);	// ���ս����Ч��
			m_VarLast.SetCurIndex(iIndex);
			
			// ���õ�ǰ����
			for (ArbitrageVarPtrs::iterator it=vars.begin(); it != vars.end() ; ++it )
			{
				if ( NULL != it->second )
				{
					CArbitrageFormulaVariableAbs *p = it->second;
					p->SetCurIndex(iIndex);
				}
			}
			
			for ( int32 i=0; i < m_aCalcStack.size() && !bErr ; ++i )
			{	
				T_SymbolTreeNode *pNode = m_aCalcStack[i];
				if (NULL == pNode)
				{
					continue;
				}
				if ( EST_Var == pNode->symbol.eSymType || EST_Const == pNode->symbol.eSymType )
				{
					// ������ջ
					aResultStack.Add(pNode->symbol);
				}
				else if ( EST_Operator == pNode->symbol.eSymType )
				{
					if ( aTmpVar.size() < iStep+1 )
					{
						ASSERT( iStep == aTmpVar.size() );
						aTmpVar.insert(aTmpVar.end(), iStep+1-aTmpVar.size(), CArbitrageFormulaVariableVar());	// ����һ��
					}
					CArbitrageFormulaVariableVar &varTmp = aTmpVar[iStep];	// ��μ������ʱ����, ��ʱ�����еĵ�ǰֵΪ0
					varTmp.m_iMin = 0;
					varTmp.m_iMax = 1;
					varTmp.SetCurIndex(0);
					varTmp.SetCurValue(ARB_INVALID_VALUE);

					// ��������㣬������������ջ�����Ϊ������
					const T_ArbForOperator *pOp = pNode->symbol.u_Content.pOp;
					if (NULL == pOp)
					{
						continue;
					}
					
					// ����ջ�в���0λ��Ϊ����Ĳ���
					if ( aResultStack.GetSize() < pOp->iParamCount )
					{
						////ASSERT(0);
						CString StrErr;
						StrErr.Format(_T("���� %s �����ľ�"), pOp->szOp);
						SetLastErrorText(StrErr);
						SetErrorPos(pNode->symbol.pszFormulaPos-m_StrFormula.operator LPCTSTR());
						bErr = true;
						break;	// ���ܼ�����
					}
					
					if ( pOp->iParamCount > 2 || pOp->iParamCount <= 0 )
					{
						////ASSERT(0);
						CString StrErr;
						StrErr.Format(_T("��֧�ֵ������ %s"), pOp->szOp);
						SetLastErrorText(StrErr);
						SetErrorPos(pNode->symbol.pszFormulaPos-m_StrFormula.operator LPCTSTR());
						bErr = true;
						break;	// ���ܼ�����
					}
					int32 iPStart = aResultStack.GetSize()-pOp->iParamCount;
					int32 iVPos = 0;
					
					CArbitrageFormulaVariableAbs *v[4] = {0};
					ASSERT( pOp->iParamCount == 2 || pOp->iParamCount == 1 );
					for ( ; iPStart < aResultStack.GetSize() ; ++iPStart, ++iVPos )
					{
						const T_Symbol &sym = aResultStack[iPStart];
						if ( sym.eSymType == EST_Var || sym.eSymType == EST_Const )
						{
							ASSERT( sym.u_Content.pVar != NULL );
							v[iVPos] = sym.u_Content.pVar;
						}
						else if ( sym.eSymType == EST_TmpValue )
						{
							// ��ʱ��������
							ASSERT( sym.u_Content.iTmpIndex >= 0 && sym.u_Content.iTmpIndex < aTmpVar.size() );
							v[iVPos] = &aTmpVar[sym.u_Content.iTmpIndex];
						}
						else
						{
							////ASSERT(0);	// ����ջ�в�������������
						}
					}
					
					param.pVar1 = v[0];
					param.pVar2 = v[1];
					float fV;
					bool32 bOk = false;
					if ( (*pOp->func)(varTmp, param) && varTmp.GetCurValue(fV) )
					{
						bOk = _finite(fV) && fV != ARB_INVALID_VALUE;
					}
					if ( !bOk )
					{
						varTmp.SetCurValue(ARB_INVALID_VALUE);
					}
					T_Symbol symTmp2 = symTmp;
					symTmp2.u_Content.iTmpIndex = iStep;
#ifdef _DEBUGARB
					if ( iIndex-iMinIndex < 3 )
					{
						
						TRACE(_T("    ���㲽�� %d: �����:%s λ��:%d ����: "), iStep, pOp->szOp, pNode->symbol.pszFormulaPos-m_StrFormula.operator LPCTSTR());
						symTmp2.StrVar.Format(_T("?%d"), iStep);
						iPStart = aResultStack.GetSize()-pOp->iParamCount;
						for ( ; iPStart < aResultStack.GetSize() ; ++iPStart )
						{
							const T_Symbol &sym = aResultStack[iPStart];
							TRACE(_T(" %s"), sym.StrVar);
							CArbitrageFormulaVariableAbs *pVar = NULL;
							if ( sym.eSymType == EST_Var || sym.eSymType == EST_Const )
							{
								ASSERT( sym.u_Content.pVar != NULL );
								pVar = sym.u_Content.pVar;
							}
							else if ( sym.eSymType == EST_TmpValue )
							{
								// ��ʱ��������
								ASSERT( sym.u_Content.iTmpIndex >= 0 && sym.u_Content.iTmpIndex < aTmpVar.size() );
								pVar = &aTmpVar[sym.u_Content.iTmpIndex];
							}
							else
							{
								////ASSERT(0);	// ����ջ�в�������������
							}
							ASSERT( NULL != pVar );
							float fV = ARB_INVALID_VALUE;
							if ( pVar->GetCurValue(fV) && fV != ARB_INVALID_VALUE )
							{
								TRACE(_T("(%0.2f) "), fV);
							}
							else
							{
								TRACE(_T("(��Ч) "));
							}
						}
						
						float fV = ARB_INVALID_VALUE;
						TRACE(_T("  ���:%s"), symTmp2.StrVar);
						if ( varTmp.GetCurValue(fV) && fV != ARB_INVALID_VALUE )
						{
							TRACE(_T("(%0.2f)"), fV);
						}
						else
						{
							TRACE(_T("(��Ч)"));
						}
						
						TRACE(_T("\r\n"));
					}
#endif
					aResultStack.RemoveAt(aResultStack.GetSize()-pOp->iParamCount, pOp->iParamCount);
					aResultStack.Add(symTmp2);
					
					++iStep;	// ����+1
				}
				else
				{
					////ASSERT(0);	// ��Ӧ�ó�����������
				}
			}// ���в�������
			if ( aResultStack.GetSize() != 1 )
			{
				////ASSERT(0);
				CString StrErr;
				StrErr.Format(_T("�������������ߺľ�(%d)"), aResultStack.GetSize());
				SetLastErrorText(StrErr);
				SetErrorPos(-1);
				bErr = true;
				break;	// ���ܼ�����
			}
			else
			{
				// ���ձ�������������ʽ���
				const T_Symbol &sym = aResultStack[0];
				CArbitrageFormulaVariableAbs *pVar = NULL;
				if ( sym.eSymType == EST_Var || sym.eSymType == EST_Const )
				{
					ASSERT( sym.u_Content.pVar != NULL );
					pVar = sym.u_Content.pVar;
				}
				else if ( sym.eSymType == EST_TmpValue )
				{
					// ��ʱ��������
					ASSERT( sym.u_Content.iTmpIndex >= 0 && sym.u_Content.iTmpIndex < aTmpVar.size() );
					pVar = &aTmpVar[sym.u_Content.iTmpIndex];
				}				
				if (NULL != pVar)
				{
					float fV = ARB_INVALID_VALUE;
					bool32 bV = pVar->GetCurValue(fV) && fV != ARB_INVALID_VALUE;
					m_VarLast.SetCurValue(bV ? fV : ARB_INVALID_VALUE);
				}
				

#ifdef _DEBUGARB
				if ( iIndex-iMinIndex < 10 )
				{
					TRACE(_T("  ����%d���ձ���: %s "), iIndex, sym.StrVar);
					if ( bV )
					{
						TRACE(_T("(%0.2f)"), fV);
					}
					else
					{
						TRACE(_T("(��Ч)"));
					}
					ASSERT( b );
					TRACE(_T("\r\n"));
				}
#endif
			}
		}
	}
	catch (std::exception &e)
	{
		bErr = true;
		CString Str(e.what());
		TRACE(_T("���������쳣��") + Str + _T("\r\n"));

		CString StrErr;
		StrErr.Format(_T("�����쳣��"));
		SetLastErrorText(StrErr);
		SetErrorPos(-1);
	}
	catch (...)
	{
		bErr = true;
		TRACE(_T("��������δ֪�쳣\r\n"));

		CString StrErr;
		StrErr.Format(_T("����δ֪�쳣��"));
		SetLastErrorText(StrErr);
		SetErrorPos(-1);
	}

	if ( bErr )
	{
		if ( GetLastErrorText().IsEmpty() )
		{
			SetLastErrorText(_T("��������λ�ô���"));
			SetErrorPos(-1);
		}
		return false;
	}

	return true;
}

bool32 CArbitrageFormula::Calculate2( INOUT ArbitrageVarPtrs &vars, int32 iMinIndex, int32 iMaxIndex )
{
	m_VarLast.m_aValues.clear();
	m_VarLast.SetCurIndex(-1);
	m_VarLast.SetMinIndex(0);
	m_VarLast.SetMaxIndex(-1);

	if ( iMinIndex > iMaxIndex || iMinIndex < 0 )
	{
		SetErrorPos(-1);
		CString StrErr;
		StrErr.Format(_T("��Ч���㷶Χ[%d-%d]"), iMinIndex, iMaxIndex);
		SetLastErrorText(StrErr);
		return false;
	}

	if ( !TestFormula(vars) )
	{
		return false;
	}

	ASSERT( NULL != m_pSymbolTree && m_aCalcStack.size() > 0 );
	// ���͸�����
	// ��������ָ��ŵ�symbol��
	for ( int32 i=0; i < m_aCalcStack.size() ; ++i )
	{
		T_SymbolTreeNode *pNode = m_aCalcStack[i];
		if (NULL == pNode)
		{
			return false;
		}
		if ( pNode->symbol.eSymType == EST_Const )
		{
			ArbitrageCalcVarPtrs::iterator it = m_mapConstVar.find(pNode->symbol.StrVar);
			if ( it == m_mapConstVar.end() )
			{
				// û�г�ʼ���ó�����Ϣ�������ʼ��
				CArbitrageFormulaVariableConst *pVar = new CArbitrageFormulaVariableConst(pNode->symbol.StrVar);
				TCHAR *p;
				pVar->m_val = (float)_tcstod(pNode->symbol.StrVar, &p);	// �����������Чû
				it = m_mapConstVar.insert(ArbitrageCalcVarPtrs::value_type(pNode->symbol.StrVar, pVar)).first;
			}
			CArbitrageFormulaVariableAbs *pVar = it->second;
			ASSERT( NULL != pVar );
			ASSERT( pNode->symbol.u_Content.pVar == NULL || pNode->symbol.u_Content.pVar == pVar );	// ��Ȼͬʱ��ʧ
			pNode->symbol.u_Content.pVar = pVar;
		}
		else if ( pNode->symbol.eSymType == EST_Var )
		{
			// �����ɲ����ṩ�������������
			ArbitrageVarPtrs::iterator it = vars.find(pNode->symbol.StrVar);
			if ( it == vars.end() || NULL == it->second )
			{
				// û�������
				CString StrErr;
				StrErr.Format(_T("����δ��ʼ�� %s"), pNode->symbol.StrVar.GetBuffer());
				SetLastErrorText(StrErr);
				SetErrorPos(pNode->symbol.pszFormulaPos - m_StrFormula.operator LPCTSTR());
				return false;
			}
			CArbitrageFormulaVariableAbs *pVar = it->second;
			ASSERT( NULL != pVar );
			pNode->symbol.u_Content.pVar = pVar;
		}
	}

	m_VarLast.m_aValues.reserve(iMaxIndex+1);
	
	// ���ռ���ջ���㣬����ջΪ�������У�����������ߵ����Ҳ�����
	// ˳��ΪcalcStack 0..n ������ ������ �����
	// Ĭ�ϷǷ����Ϊ ARB_INVALID_VALUE

	// �����޸�symbol�ṹ�壬ʹÿ�������symbol����ƥ��һ����ʱ�������
	// ���ڵ�������������������������������ڵ㲻�������������ڵ�����������������
	// δʵ��

	SymbolArray	aResultStack;
	aResultStack.SetSize(0, m_aCalcStack.size());
#ifdef _DEBUGARB
	TRACE(_T("������ʽ����2: %s ����[%d-%d]\r\n"), m_StrFormula, iMinIndex, iMaxIndex);
#endif
	T_Symbol symTmp;
	symTmp.eSymType = EST_TmpValue;
	symTmp.u_Content.iTmpIndex = -1;

	ASSERT( m_VarLast.m_aValues.empty() );
	m_VarLast.m_aValues.insert(m_VarLast.m_aValues.end(), iMaxIndex+1, ARB_INVALID_VALUE);
	m_VarLast.SetMinIndex(iMinIndex);
	m_VarLast.SetMaxIndex(iMaxIndex);
	m_VarLast.SetCurIndex(iMinIndex);

	bool32 bErr = false;
	typedef vector<CArbitrageFormulaVariableVar> VarArray;
	VarArray aTmpVar;	// ��ʱ�����
	aTmpVar.reserve(100);
	T_ArbForOperatorParam param = {0};
	param.iMinIndex = iMinIndex;	// ��������
	param.iMaxIndex = iMaxIndex;

	try
	{
		int32 iStep = 0;
		// ���¿�ʼ������һ��
		aResultStack.SetSize(0);

#ifdef _DEBUGARB
		// �������б�����cur indexΪ��Сֵ
		for ( ArbitrageCalcVarPtrs::iterator it=m_mapConstVar.begin(); it != m_mapConstVar.end() ; ++it )
		{
			if ( NULL != it->second )
			{
				CArbitrageFormulaVariableAbs *pVar = it->second;
				pVar->SetCurIndex(iMinIndex);
			}
		}
		for ( ArbitrageVarPtrs::iterator it2=vars.begin(); it2 != vars.end() ; ++it2 )
		{
			if ( NULL != it2->second )
			{
				CArbitrageFormulaVariableAbs *pVar = it2->second;
				pVar->SetCurIndex(iMinIndex);
			}
		}
#endif
		
		
		for ( int32 i=0; i < m_aCalcStack.size() && !bErr ; ++i )
		{
			
			T_SymbolTreeNode *pNode = m_aCalcStack[i];
			if(NULL == pNode)
			{
				continue;
			}
			if ( EST_Var == pNode->symbol.eSymType || EST_Const == pNode->symbol.eSymType )
			{
				// ������ջ
				aResultStack.Add(pNode->symbol);
			}
			else if ( EST_Operator == pNode->symbol.eSymType )
			{
				if ( aTmpVar.size() < iStep+1 )
				{
					ASSERT( iStep == aTmpVar.size() );
					aTmpVar.insert(aTmpVar.end(), iStep+1-aTmpVar.size(), CArbitrageFormulaVariableVar());	// ����һ��
				}
				CArbitrageFormulaVariableVar &varTmp = aTmpVar[iStep];	// ��μ������ʱ����, ��ʱ�����д�����н��
				varTmp.SetMinIndex(iMinIndex);
				varTmp.SetMaxIndex(iMaxIndex);
				varTmp.SetCurIndex(iMinIndex);
				
				// ��������㣬������������ջ�����Ϊ������
				const T_ArbForOperator *pOp = pNode->symbol.u_Content.pOp;
				if (NULL == pOp)
				{
					continue;
				}
				
				// ����ջ�в���0λ��Ϊ����Ĳ���
				if ( aResultStack.GetSize() < pOp->iParamCount )
				{
					////ASSERT(0);
					CString StrErr;
					StrErr.Format(_T("���� %s �����ľ�"), pOp->szOp);
					SetLastErrorText(StrErr);
					SetErrorPos(pNode->symbol.pszFormulaPos-m_StrFormula.operator LPCTSTR());
					bErr = true;
					break;	// ���ܼ�����
				}
				
				if ( pOp->iParamCount > 2 || pOp->iParamCount <= 0 )
				{
					////ASSERT(0);
					CString StrErr;
					StrErr.Format(_T("��֧�ֵ������ %s"), pOp->szOp);
					SetLastErrorText(StrErr);
					SetErrorPos(pNode->symbol.pszFormulaPos-m_StrFormula.operator LPCTSTR());
					bErr = true;
					break;	// ���ܼ�����
				}
				int32 iPStart = aResultStack.GetSize()-pOp->iParamCount;
				int32 iVPos = 0;
				
				CArbitrageFormulaVariableAbs *v[4] = {0};
				ASSERT( pOp->iParamCount == 2 || pOp->iParamCount == 1 );
				for ( ; iPStart < aResultStack.GetSize() ; ++iPStart, ++iVPos )
				{
					const T_Symbol &sym = aResultStack[iPStart];
					if ( sym.eSymType == EST_Var || sym.eSymType == EST_Const )
					{
						ASSERT( sym.u_Content.pVar != NULL );
						v[iVPos] = sym.u_Content.pVar;
					}
					else if ( sym.eSymType == EST_TmpValue )
					{
						// ��ʱ��������
						ASSERT( sym.u_Content.iTmpIndex >= 0 && sym.u_Content.iTmpIndex < aTmpVar.size() );
						v[iVPos] = &aTmpVar[sym.u_Content.iTmpIndex];
					}
					else
					{
						////ASSERT(0);	// ����ջ�в�������������
					}
				}
				
				param.pVar1 = v[0];
				param.pVar2 = v[1];
				float fV;
				bool32 bOk = false;
				if ( (*pOp->func)(varTmp, param) && varTmp.GetCurValue(fV) )
				{
					bOk = _finite(fV) && fV != ARB_INVALID_VALUE;
				}
				if ( !bOk )
				{
					fV = ARB_INVALID_VALUE;
				}
				T_Symbol symTmp2 = symTmp;
				symTmp2.u_Content.iTmpIndex = iStep;
#ifdef _DEBUGARB
				{
					
					TRACE(_T("    ���㲽�� %d: �����:%s λ��:%d ����: "), iStep, pOp->szOp, pNode->symbol.pszFormulaPos-m_StrFormula.operator LPCTSTR());
					symTmp2.StrVar.Format(_T("?%d"), iStep);
					iPStart = aResultStack.GetSize()-pOp->iParamCount;
					for ( ; iPStart < aResultStack.GetSize() ; ++iPStart )
					{
						const T_Symbol &sym = aResultStack[iPStart];
						TRACE(_T(" %s"), sym.StrVar);
						CArbitrageFormulaVariableAbs *pVar = NULL;
						if ( sym.eSymType == EST_Var || sym.eSymType == EST_Const )
						{
							ASSERT( sym.u_Content.pVar != NULL );
							pVar = sym.u_Content.pVar;
						}
						else if ( sym.eSymType == EST_TmpValue )
						{
							// ��ʱ��������
							ASSERT( sym.u_Content.iTmpIndex >= 0 && sym.u_Content.iTmpIndex < aTmpVar.size() );
							pVar = &aTmpVar[sym.u_Content.iTmpIndex];
						}
						else
						{
							////ASSERT(0);	// ����ջ�в�������������
						}
						ASSERT( NULL != pVar );
						float fV = ARB_INVALID_VALUE;
						if ( pVar->GetCurValue(fV) && fV != ARB_INVALID_VALUE )
						{
							TRACE(_T("(%0.2f)"), fV);
						}
						else
						{
							TRACE(_T("(��Ч)"));
						}
					}
					
					float fV = ARB_INVALID_VALUE;
					TRACE(_T("  ���:%s"), symTmp2.StrVar);
					if ( varTmp.GetCurValue(fV) && fV != ARB_INVALID_VALUE )
					{
						TRACE(_T("(%0.2f)"), fV);
					}
					else
					{
						TRACE(_T("(��Ч)"));
					}
					
					TRACE(_T("\r\n"));
				}
#endif
				aResultStack.RemoveAt(aResultStack.GetSize()-pOp->iParamCount, pOp->iParamCount);
				aResultStack.Add(symTmp2);
				
				++iStep;	// ����+1
			}
			else
			{
				////ASSERT(0);	// ��Ӧ�ó�����������
			}
		}// ���в�������
		
		if ( aResultStack.GetSize() != 1 )
		{
			////ASSERT(0);
			CString StrErr;
			StrErr.Format(_T("�������������ߺľ�(%d)"), aResultStack.GetSize());
			SetLastErrorText(StrErr);
			SetErrorPos(-1);
			bErr = true;
		}
		else
		{
			// ���ձ�������������ʽ���
			const T_Symbol &sym = aResultStack[0];
			CArbitrageFormulaVariableAbs *pVar = NULL;
			if ( sym.eSymType == EST_Var || sym.eSymType == EST_Const )
			{
				ASSERT( sym.u_Content.pVar != NULL );
				pVar = sym.u_Content.pVar;
			}
			else if ( sym.eSymType == EST_TmpValue )
			{
				// ��ʱ��������
				ASSERT( sym.u_Content.iTmpIndex >= 0 && sym.u_Content.iTmpIndex < aTmpVar.size() );
				pVar = &aTmpVar[sym.u_Content.iTmpIndex];
			}

			if (NULL != pVar)
			{
				// ��ֵ�����Ľ����
				for ( int32 i=iMinIndex; i <= iMaxIndex ; ++i )
				{
					float fV;
					bool32 bV = pVar->GetValue(i, fV) && fV != ARB_INVALID_VALUE;
					bool32 b = m_VarLast.SetValue(i, bV ? fV : ARB_INVALID_VALUE);
					ASSERT( b );
				}
			}			
			
#ifdef _DEBUGARB
			{
				float fV;
				bool32 bV = pVar->GetCurValue(fV) && fV != ARB_INVALID_VALUE;
				TRACE(_T("  ����%d ���ձ���: %s "), pVar->GetCurIndex(), sym.StrVar);
				if ( bV )
				{
					TRACE(_T("(%0.2f)"), fV);
				}
				else
				{
					TRACE(_T("(��Ч)"));
				}
				TRACE(_T("\r\n"));
			}
#endif
		}
		
	}
	catch (std::exception &e)
	{
		bErr = true;
		CString Str(e.what());
		TRACE(_T("���������쳣��") + Str + _T("\r\n"));

		CString StrErr;
		StrErr.Format(_T("�����쳣��"));
		SetLastErrorText(StrErr);
		SetErrorPos(-1);
	}
	catch (...)
	{
		bErr = true;
		TRACE(_T("��������δ֪�쳣\r\n"));

		CString StrErr;
		StrErr.Format(_T("����δ֪�쳣��"));
		SetLastErrorText(StrErr);
		SetErrorPos(-1);
	}

	if ( bErr )
	{
		if ( GetLastErrorText().IsEmpty() )
		{
			SetLastErrorText(_T("��������λ�ô���"));
			SetErrorPos(-1);
		}
		return false;
	}

	return true;
}

bool32 CArbitrageFormula::GetVariables( OUT ArbitrageVarPtrs &vars )
{
	if ( !IsParsed() )
	{
		////ASSERT(0);
		return false;	// �����Ƚ�����ʽ���б���
	}
	ASSERT( NULL != m_pSymbolTree && m_aCalcStack.size() > 0 );
	// �������ǰ�ı���
	for ( int32 i=0; i < m_aCalcStack.size() ; ++i )
	{
		const T_SymbolTreeNode *pNode = m_aCalcStack[i];
		if (NULL != pNode)
		{
			const T_Symbol &sym = pNode->symbol;
			if ( sym.eSymType == EST_Var )
			{
				ArbitrageVarPtrs::iterator it = vars.find(sym.StrVar);
				if ( it == vars.end() || NULL == it->second )
				{
					// û�иñ����򴴽�
					CArbitrageFormulaVariableVar *pVar = new CArbitrageFormulaVariableVar;
					vars[sym.StrVar] = pVar;
				}
			}
		}
		
	}

	for ( ArbitrageVarPtrs::iterator it=vars.begin(); it != vars.end() ; ++it )
	{
		if ( NULL != it->second )
		{
			CArbitrageFormulaVariableVar *pVar = it->second;
			pVar->SetVarName( it->first );
			pVar->SetCurIndex(-1);
			pVar->SetMinIndex(0);
			pVar->SetMaxIndex(-1);
		}
	}

	return true;
}

void CArbitrageFormula::FreeVariables( INOUT ArbitrageVarPtrs &vars )
{
	for ( ArbitrageVarPtrs::iterator it=vars.begin(); it != vars.end() ; ++it )
	{
		if ( NULL != it->second )
		{
			CArbitrageFormulaVariableVar *pVar = it->second;
			delete pVar;
			it->second = NULL;	// ��ոñ�����ָ��
		}
	}
	vars.clear();	// ������н��
}

bool32 CArbitrageFormula::GetUnInitializedVariables( OUT ArbitrageVarPtrs &vars )
{
	if ( !IsParsed() )
	{
		////ASSERT(0);
		return false;	// �����Ƚ�����ʽ���б���
	}
	ASSERT( NULL != m_pSymbolTree && m_aCalcStack.size() > 0 );
	// �������ǰ�ı���
	for ( int32 i=0; i < m_aCalcStack.size() ; ++i )
	{
		const T_SymbolTreeNode *pNode = m_aCalcStack[i];
		if(NULL != pNode)
		{
			const T_Symbol &sym = pNode->symbol;
			if ( sym.eSymType == EST_Var )
			{
				ArbitrageVarPtrs::iterator it = vars.find(sym.StrVar);
				if ( it == vars.end()  )
				{
					// ����δ��ʼ��ֵ
					vars[sym.StrVar] = NULL;
				}
			}
		}		
	}
	
	return true;
}

bool CArbitrageFormula::operator==( const CArbitrageFormula &for2 ) const
{
	// ���жϹ�ʽ�����Ƿ���ͬ
	bool bEqu = true;
	bEqu = bEqu && m_StrFormula.CompareNoCase(for2.m_StrFormula)==0;
	
	return bEqu;
}


//////////////////////////////////////////////////////////////////////////
//

bool32 ArbForOperatorAdd( OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param)
{
	if ( param.iMinIndex > param.iMaxIndex )
	{
		float fV1, fV2;		
		if (NULL != param.pVar1 && NULL != param.pVar2 )
		{
			if ( param.pVar1->GetCurValue(fV1) && param.pVar2->GetCurValue(fV2) )
			{
				return valResult.SetCurValue(fV1+fV2);
			}
		}
		
	}
	else
	{
		// ��������
		valResult.SetMinIndex(param.iMinIndex);
		valResult.SetMaxIndex(param.iMaxIndex);
		float fV1, fV2;
		if (NULL != param.pVar1 && NULL != param.pVar2 )
		{
			for ( int32 i=param.iMinIndex; i <= param.iMaxIndex ; ++i )
			{
				if ( param.pVar1->GetValue(i, fV1) && param.pVar2->GetValue(i, fV2) )
				{
					valResult.SetValue(i, fV1+fV2);
				}
				else
				{
					valResult.SetValue(i, ARB_INVALID_VALUE);
				}
			}
			return true;
		}		
	}
	return false;
}

bool32 ArbForOperatorSub( OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param)
{
	if ( param.iMinIndex > param.iMaxIndex )
	{
		float fV1, fV2;	
		if (NULL != param.pVar1 && NULL != param.pVar2)
		{
			if ( param.pVar1->GetCurValue(fV1) && param.pVar2->GetCurValue(fV2) )
			{
				return valResult.SetCurValue(fV1-fV2);
			}
		}
		
	}
	else
	{
		// ��������
		valResult.SetMinIndex(param.iMinIndex);
		valResult.SetMaxIndex(param.iMaxIndex);
		float fV1, fV2;		
		if (NULL != param.pVar1 && NULL != param.pVar2)
		{
			for ( int32 i=param.iMinIndex; i <= param.iMaxIndex ; ++i )
			{
				if ( param.pVar1->GetValue(i, fV1) && param.pVar2->GetValue(i, fV2) )
				{
					valResult.SetValue(i, fV1-fV2);
				}
				else
				{
					valResult.SetValue(i, ARB_INVALID_VALUE);
				}
			}
			return true;
		}
		
	}
	return false;
}

bool32 ArbForOperatorMulti( OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param)
{
	if ( param.iMinIndex > param.iMaxIndex )
	{
		float fV1, fV2;		
		if ( NULL != param.pVar1 && NULL != param.pVar2)
		{
			if ( param.pVar1->GetCurValue(fV1) && param.pVar2->GetCurValue(fV2) )
			{
				return valResult.SetCurValue(fV1*fV2);
			}
		}
		
	}
	else
	{
		// ��������
		valResult.SetMinIndex(param.iMinIndex);
		valResult.SetMaxIndex(param.iMaxIndex);
		float fV1, fV2;
		if ( NULL != param.pVar1 && NULL != param.pVar2)
		{
			for ( int32 i=param.iMinIndex; i <= param.iMaxIndex ; ++i )
			{
				if ( param.pVar1->GetValue(i, fV1) && param.pVar2->GetValue(i, fV2) )
				{
					valResult.SetValue(i, fV1*fV2);
				}
				else
				{
					valResult.SetValue(i, ARB_INVALID_VALUE);
				}
			}
			return true;
		}		
	}
	return false;
}

bool32 ArbForOperatorDiv( OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param)
{
	if ( param.iMinIndex > param.iMaxIndex )
	{
		float fV1, fV2;		
		if ( NULL != param.pVar1 && NULL != param.pVar2)
		{
			if ( param.pVar1->GetCurValue(fV1) && param.pVar2->GetCurValue(fV2) )
			{
				return valResult.SetCurValue(fV1/fV2);
			}
		}
		
	}
	else
	{
		// ��������
		valResult.SetMinIndex(param.iMinIndex);
		valResult.SetMaxIndex(param.iMaxIndex);
		float fV1, fV2;		
		if (NULL != param.pVar1 && NULL != param.pVar2)
		{
			for ( int32 i=param.iMinIndex; i <= param.iMaxIndex ; ++i )
			{
				if ( param.pVar1->GetValue(i, fV1) && param.pVar2->GetValue(i, fV2) )
				{
					valResult.SetValue(i, fV1/fV2);
				}
				else
				{
					valResult.SetValue(i, ARB_INVALID_VALUE);
				}
			}
			return true;
		}
		
	}
	return false;
}

bool32 ArbForOperatorAssign( OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param)
{
	if ( param.iMinIndex > param.iMaxIndex )
	{
		float fV2;
	
		if (NULL != param.pVar1 && NULL != param.pVar2)
		{
			if ( param.pVar2->GetCurValue(fV2) )
			{
				return valResult.SetCurValue(fV2) && param.pVar1->SetCurValue(fV2);
			}
		}
		
	}
	else
	{
		// ��������
		valResult.SetMinIndex(param.iMinIndex);
		valResult.SetMaxIndex(param.iMaxIndex);
		float fV2;		
		if (NULL != param.pVar1 && NULL != param.pVar2)
		{
			param.pVar1->SetMinIndex(param.iMinIndex);
			param.pVar2->SetMaxIndex(param.iMaxIndex);
			for ( int32 i=param.iMinIndex; i <= param.iMaxIndex ; ++i )
			{
				if ( param.pVar2->GetValue(i, fV2) )
				{
					valResult.SetValue(i, fV2);
					param.pVar1->SetValue(i, fV2);
				}
				else
				{
					valResult.SetValue(i, ARB_INVALID_VALUE);
					param.pVar1->SetValue(i, ARB_INVALID_VALUE);
				}
			}
			return true;
		}	
	}
	return false;
}

bool32 ArbForOperatorComma( OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param)
{
	if ( param.iMinIndex > param.iMaxIndex )
	{
		float fV2;
		ASSERT( NULL != param.pVar1 && NULL != param.pVar2 );
		if ( param.pVar2->GetCurValue(fV2) )
		{
			return valResult.SetCurValue(fV2);
		}
	}
	else
	{
		// ��������
		valResult.SetMinIndex(param.iMinIndex);
		valResult.SetMaxIndex(param.iMaxIndex);
		float fV2;
		ASSERT( NULL != param.pVar1 && NULL != param.pVar2 );
		for ( int32 i=param.iMinIndex; i <= param.iMaxIndex ; ++i )
		{
			if ( param.pVar2->GetValue(i, fV2) )
			{
				valResult.SetValue(i, fV2);
			}
			else
			{
				valResult.SetValue(i, ARB_INVALID_VALUE);
			}
		}
		return true;
	}
	return false;
}

bool32 ArbForOperatorNegative( OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param)
{
	if ( param.iMinIndex > param.iMaxIndex )
	{
		float fV1;		
		if (NULL != param.pVar1)
		{
			if ( param.pVar1->GetCurValue(fV1) )
			{
				return valResult.SetCurValue(-fV1);
			}
		}
		
	}
	else
	{
		// ��������
		valResult.SetMinIndex(param.iMinIndex);
		valResult.SetMaxIndex(param.iMaxIndex);
		float fV1;		
		if ( NULL != param.pVar1)
		{
			for ( int32 i=param.iMinIndex; i <= param.iMaxIndex ; ++i )
			{
				if ( param.pVar1->GetValue(i, fV1) )
				{
					valResult.SetValue(i, -fV1);
				}
				else
				{
					valResult.SetValue(i, ARB_INVALID_VALUE);
				}
			}
			return true;
		}
		
	}
	return false;
}

bool32 ArbForOperatorPositive( OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param)
{
	if ( param.iMinIndex > param.iMaxIndex )
	{
		float fV1;		
		if (NULL != param.pVar1 )
		{
			if ( param.pVar1->GetCurValue(fV1) )
			{
				return valResult.SetCurValue(fV1);
			}
		}
		
	}
	else
	{
		// ��������
		valResult.SetMinIndex(param.iMinIndex);
		valResult.SetMaxIndex(param.iMaxIndex);
		float fV1;	
		if ( NULL != param.pVar1)
		{
			for ( int32 i=param.iMinIndex; i <= param.iMaxIndex ; ++i )
			{
				if ( param.pVar1->GetValue(i, fV1) )
				{
					valResult.SetValue(i, fV1);
				}
				else
				{
					valResult.SetValue(i, ARB_INVALID_VALUE);
				}
			}
			return true;
		}
		
	}
	return false;
}
