#include "StdAfx.h"

#pragma warning(disable:4786)

#include <ctype.h>
#include "ArbitrageFormula.h"

// 运算符: 运算结果 左操作数，右操作数，其它操作数，其它操作数个数
// 仅计算浮点数，不考虑其它类型
// 仅考虑+-*/这种4则运算符，不考虑其它操作符
// 无短路运算
// () 运算符作为特殊处理

bool32	ArbForOperatorAdd(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
bool32	ArbForOperatorSub(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
bool32	ArbForOperatorMulti(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
bool32	ArbForOperatorDiv(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
bool32	ArbForOperatorAssign(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
bool32	ArbForOperatorComma(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
bool32	ArbForOperatorNegative(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
bool32	ArbForOperatorPositive(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);

// 这个顺序要按照优先级排列
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
			// 只支持一个字母的运算符 - 补充了
			//ASSERT( pOp->szOp[0] != _T('\0') && pOp->szOp[1] == _T('\0') );
			ASSERT( pOp->iOpPriority >= ARB_FOR_PRIORITY_MIN && pOp->iOpPriority <= ARB_FOR_PRIORITY_MAX );
			ASSERT( pOp->iParamCount == 1 || pOp->iParamCount == 2 );	// 仅支持这两种类型的运算
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
		ASSERT( NULL == *ppRootNode );	// 必须要保证传过来的是一个指向空的指针
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

	// 分为
	// 标点 1: 空格 Tab， 不支持换行 分割
	// 运算符 2: () 预定义的运算符   分割
	// 数字 3: 0-9 .
	// 剩下的都是变量 4: 按照变量命名规则来判断

	// 变量名规则:


	SymbolArray aSyms;
	aSyms.SetSize(0, 100);
	T_Symbol sym;
	sym.eSymType = EST_Count;
	sym.u_Content.pOp = (T_ArbForOperator *)10;
	sym.pszFormulaPos = NULL;

	// 先判断变量, 运算符
	SymbolArray aParenthesis;
	aParenthesis.SetSize(0, 40);
	for ( ptszFormula=ptszOrg; *ptszFormula != _T('\0') ; ptszFormula=_tcsinc(ptszFormula) )
	{	
		TCHAR ch = *ptszFormula;

		// 任何字符都有可能是特殊变量的开始，so这里用特殊变量来贪婪
		bool32 bMatchSpe = false;
		for (ArbitrageVarNameSet::reverse_iterator itSpe=m_varSpecialNames.rbegin(); itSpe != m_varSpecialNames.rend() ; ++itSpe)
		{
			LPCTSTR pszSpe = *itSpe;
			int iSpeLen = _tcslen(pszSpe);
			if ( _tcsncmp(pszSpe, ptszFormula, iSpeLen) == 0 )
			{
				// 匹配了这个特殊变量，set默认使用小->大排序，则最后的是最大的，匹配的逆序第一个就是最合适的
				// 原来的变量先添加
				// 特殊变量拥有所有判断的最高优先级，
				// 如果有其它变量名称包含了这个特殊变量，那么这个其它变量会被分开为特殊变量与其它符号一起解析
				if ( !StrVar.IsEmpty() )
				{
					ParseVarOrOp(StrVar, ptszFormula-StrVar.GetLength(), sym);
					aSyms.Add(sym);
					StrVar.Empty();
				}

				// 添加这个特殊变量
				sym.eSymType = EST_Var;
				sym.StrVar = pszSpe;
				sym.pszFormulaPos = ptszFormula;
				sym.u_Content.dwFlag = 8;	// 特殊变量标记
				sym.iMyPriority = 0;
				aSyms.Add(sym);

				ptszFormula += iSpeLen;
				ptszFormula = _tcsdec(ptszOrg, ptszFormula);	// 给continue inc留的一个
				bMatchSpe = true;
				break;
			}
		}
		if ( bMatchSpe )
		{
			ASSERT( StrVar.IsEmpty() );
			continue;	// 匹配了特殊变量，再去匹配下一个吧
		}
		
		if ( _T(' ') == ch || _T('\t') == ch || _T('\r') == ch || _T('\n') == ch )
		{
			if ( !StrVar.IsEmpty() )
			{
				ParseVarOrOp(StrVar, ptszFormula-StrVar.GetLength(), sym);
				aSyms.Add(sym);
				StrVar.Empty();
			}
			continue;	// 分割内容了，忽略这个标点
		}

		if ( _T('(') == ch )
		{
			// 括号调整优先级
			// 前面变量或运算符入栈
			if ( !StrVar.IsEmpty() )
			{
				ParseVarOrOp(StrVar, ptszFormula-StrVar.GetLength(), sym);
				aSyms.Add(sym);
				StrVar.Empty();
			}

			// 压入一个特殊symbol, 以后所有的运算，哪怕优先级再低也不会插入到这个之后
			sym.eSymType = EST_Count;
			sym.u_Content.dwFlag = 0;
			sym.pszFormulaPos = ptszFormula;
			aSyms.Add(sym);

			aParenthesis.Add(sym);

			continue;
		}
		else if ( _T(')') == ch )
		{
			// 括号调整优先级结束
			// 前面变量或运算符入栈
			if ( !StrVar.IsEmpty() )
			{
				ParseVarOrOp(StrVar, ptszFormula-StrVar.GetLength(), sym);
				aSyms.Add(sym);
				StrVar.Empty();
			}
			
			// 压入一个特殊symbol, 以后所有的运算，都不会干扰到这个括号内的
			sym.eSymType = EST_Count;
			sym.u_Content.dwFlag = 1;
			sym.pszFormulaPos = ptszFormula;
			aSyms.Add(sym);

			if ( aParenthesis.GetSize() <= 0 )
			{
				// 括号不匹配, 不用继续了
				bParseOk = false;
				CString StrErr;
				StrErr.Format(_T("括号 ) 无法匹配"));
				SetLastErrorText(StrErr);
				SetErrorPos(sym.pszFormulaPos - ptszOrg);
				break;
			}
			aParenthesis.RemoveAt(aParenthesis.GetUpperBound());
			
			continue;
		}

		// 剩下的: 变量+运算符 或者 运算符+变量
		// 操作符判定
		// 变量不能以运算符开始的字母开始
		CString StrOp;
		StrOp.Format(_T("%c"),ch);

		ArbitrageVarNameSet::iterator itOp = sm_opFirstCh.find(StrOp);
		if ( itOp != sm_opFirstCh.end() )
		{
			// 这个ch满足了运算符开始标志，看到底匹配了哪个运算符
			StrOp.Empty();	// 重新开始匹配运算符

			LPCTSTR ptsz1 = ptszFormula;
			while ( _T('\0') != *ptszFormula )
			{
				StrOp += *ptszFormula;
				OpMap::iterator it = sm_ops.find(StrOp);
				if ( it == sm_ops.end() )
				{
					StrOp.Delete(StrOp.GetLength()-1);
					ptszFormula = _tcsdec(ptsz1, ptszFormula);	// 回退一个字符
					ASSERT( NULL != ptszFormula );
					break;	// 挑选最多字母匹配的那个运算符
				}
				ptszFormula = _tcsinc(ptszFormula);
			}
			if ( _T('\0') == *ptszFormula )
			{
				ptszFormula = _tcsdec(ptsz1, ptszFormula);	// 由于到字串尾退出来的，必须要回退一个字符
				ASSERT( NULL != ptszFormula );
			}

			// 新出现的字串是运算符，前面的作为变量加入
			if ( !StrVar.IsEmpty() )
			{
				ParseVarOrOp(StrVar, ptsz1-StrVar.GetLength(), sym);
				aSyms.Add(sym);
				StrVar.Empty();
			}
			
			// 运算符字串加入
			if ( !StrOp.IsEmpty() )
			{
				ParseVarOrOp(StrOp, ptsz1, sym);
				aSyms.Add(sym);
			}
		}
		else
		{
			// 新出现的字母不是运算符，把它作为变量一部分吧
			//unsigned int tch = _tcsnextc(ptszFormula);
			StrVar += ch;
		}

		// 等待下一个字符判断吧
	}

	if ( !StrVar.IsEmpty() )
	{
		ParseVarOrOp(StrVar, ptszFormula-StrVar.GetLength(), sym);
		aSyms.Add(sym);
		StrVar.Empty();
	}

	// 变量名检查
	for ( i=0; i < aSyms.GetSize() ; ++i )
	{
		T_Symbol &tmpSym = aSyms[i];
		if ( EST_Var == tmpSym.eSymType )
		{
			// 如果是特殊变量名，则不检查其名称的合法性
			if ( tmpSym.u_Content.dwFlag == 8 )
			{
				tmpSym.u_Content.dwFlag = 0;	// 清除这个标记
				continue;
			}

			ASSERT( !tmpSym.StrVar.IsEmpty() );
			TCHAR chFirst = tmpSym.StrVar[0];
			const TCHAR chDot = _T('.');
			if ( chDot == chFirst || _istdigit(chFirst) )
			{
				bool32 bHasDot = false;
				// 是数字，则必须整个都是数字,最多一个小数点
				for ( int32 j=0; j < tmpSym.StrVar.GetLength() ; ++j )
				{
					TCHAR ch = tmpSym.StrVar.GetAt(j);
					if ( (bHasDot && ch==chDot)
						|| (!_istdigit(ch) && ch != chDot)
						|| (tmpSym.StrVar.GetLength()==1&&bHasDot) )
					{
						// 多个小数点或者非数字或者没有一个数字，错误
						bParseOk = false;
						ASSERT( tmpSym.pszFormulaPos != NULL );
						SetErrorPos(tmpSym.pszFormulaPos - ptszOrg);
						CString StrError;
						StrError.Format(_T("错误的参数声明或者非法的数字[%s]!"), tmpSym.StrVar.GetBuffer());
						SetLastErrorText(StrError);
						break;
					}
					bHasDot = bHasDot || ch == chDot;
				}
				if ( !bParseOk )
				{
					// 已经错误了，不进行下面的变量名判断了
					break;
				}

				tmpSym.eSymType = EST_Const;	// 常量数字
			}
			else
			{
				// 变量

				// 还是变量- -
				// 必须要每个char都是字母或数字或者_，首字母不能为数字
				//lint --e{571}
				for ( LPCTSTR psz=tmpSym.StrVar; *psz != _T('\0') ; psz = _tcsinc(psz) )
				{					
					unsigned int ch = (unsigned int)(_tcsnextc(psz));
					bool32 bOk = (ch>=_T('A') && ch <= _T('Z'))
						|| (ch>=_T('a') && ch <= _T('z'))
						|| (ch>=_T('0') && ch <= _T('9'))
						|| ch == _T('_')
						|| ch > 255;
					// 中文必然大于255
					if ( !bOk )
					{
						// 多个小数点或者非数字或者没有一个数字，错误
						bParseOk = false;
						ASSERT( tmpSym.pszFormulaPos != NULL );
						SetErrorPos(tmpSym.pszFormulaPos - ptszOrg);
						CString StrError;
						StrError.Format(_T("错误的参数声明[%s]!"), tmpSym.StrVar.GetBuffer());
						SetLastErrorText(StrError);
						break;
					}
				}
				if ( !bParseOk )
				{
					// 已经错误了，不进行下面的变量名判断了
					break;
				}
			}
		}
		else if ( EST_Operator == sym.eSymType )
		{
			const T_ArbForOperator *pOp = sym.u_Content.pOp;
			if ( pOp->iParamCount != 1 && pOp->iParamCount != 2 )
			{
				// 仅支持1个或者2个参数的运算符
				bParseOk = false;
				ASSERT( tmpSym.pszFormulaPos != NULL );
				SetErrorPos(tmpSym.pszFormulaPos - ptszOrg);
				CString StrError;
				StrError.Format(_T("不支持的运算符[%s]!"), tmpSym.StrVar.GetBuffer());
				SetLastErrorText(StrError);
				break;
			}
		}
	}
	// 有左括号不能找到右括号的问题
	if ( bParseOk && aParenthesis.GetSize() != 0 )
	{
		ASSERT( aParenthesis.GetSize() > 0 );
		// 括号不匹配, 不用继续了
		bParseOk = false;
		CString StrErr;
		SetErrorPos(-1);
		StrErr.Format(_T("括号 ( 没有合适的 ) 匹配"));
		SetLastErrorText(StrErr);
		if ( aParenthesis.GetSize() > 0 )
		{
			SetErrorPos(aParenthesis[aParenthesis.GetUpperBound()].pszFormulaPos - ptszOrg);
		}
	}
	
	if ( !bParseOk )
	{
		return false;	// 错误的公式
	}

	// 某些运算符由于有多个(+-)可能，还要针对参数的变化选用合适的
	// 选用合适的运算符
	// 从左向右猜测
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
				tmpSym.u_Content.pOp = NULL;	// 先清空
				// 从优先级高的开始匹配
				for ( OpMap::iterator it=itRange.first; it != itRange.second ; ++it )
				{
					const T_ArbForOperator *pOp = it->second;
					ASSERT( NULL != pOp );
					ASSERT( pOp->iParamCount == 1 || pOp->iParamCount == 2 );	// 仅支持<=2个参数的
					bool32 bMatch = false;
					switch ( pOp->iParamCount )
					{
					case 1: // 单目运算符
						{
							if ( pOp->bLeftToRight )
							{
								// 单目左操作数, 
								// 如果左边为一个数
								// 或者 右边为运算开始或结束（或者一个需要左操作数的运算符）
								// 则该运算符应当选为单目
								if ( i+1 == aSyms.GetSize()
									|| aSyms[i+1].eSymType == EST_Count
									)
								{
									// 该位置位于表达式计算结束处，则不是二元运算符
									bMatch = true;
								}
								else if ( i!=0 )
								{
									// 对前面的要求: 上一个是变量 或者 子表达式结束 或者 上一个是运算符且上个运算符能单独产生数据
									// 对后面的要求: 后一个是操作符，且后一个操作符, 但是由于后面是不确定的，所以只能前面判断猜测
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
								// 单目右操作数
								// 左边为表达式开始
								// 左边是一个需要右操作数的运算符
								// 则该运算符应当选为单目
								if ( 0 == i
									|| (aSyms[i-1].eSymType == EST_Count&&aSyms[i-1].u_Content.dwFlag==0)
									)
								{
									// 该位置位于表达式计算开始处，则不是二元运算符
									bMatch = true;
								}
								else
								{
									// 左侧需要这个运算符单独产生数据
									bool32 bPreMatch = (aSyms[i-1].eSymType==EST_Operator && (aSyms[i-1].u_Content.pOp->iParamCount>1||!aSyms[i-1].u_Content.pOp->bLeftToRight))
										;
									bMatch = bPreMatch;
								}
							}
							
						}
						break;
					case 2: // 双目运算符
						bMatch = true;	// 在这里总是符合:)
						break;
					}

					if ( bMatch )
					{
						// 赋予优先级合适的匹配运算符
						if ( tmpSym.u_Content.pOp == NULL || tmpSym.u_Content.pOp->iOpPriority < pOp->iOpPriority )
						{
							tmpSym.u_Content.pOp = pOp;
						}
					}
				}

				if ( tmpSym.u_Content.pOp == NULL )
				{
					// 没有找到匹配运算符, 不用在进行运算符匹配了
					bParseOk = false;
					CString StrErr;
					StrErr.Format(_T("运算符 %s 有歧义，无法匹配"), pOld->szOp);
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

	// 树的结构是参数与操作符结构，并非运算顺序结构
	// 即要求计算顺序又要求能够正确的将参数传给操作符
	// 优先级最高的运算符只能依赖同级的运算符结果，不能依赖低优先级的
	// 编制运算树
	
	SymFrameArray	aFrames;

	SymbolNodePtrArray aDels, aSorts;
	int32 iInParenthesisIndex = 0;	// 默认有一个基础的括号
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
		T_SymFrame *pframe = &aFrames[iInParenthesisIndex]; // 当前frame

		if ( tmpSym.eSymType == EST_Const || tmpSym.eSymType == EST_Var )
		{
			if ( !DoParseAddCalcVar(pframe, pNewNode) )
			{
				bParseOk = false;
				break;	// 出现错误
			}
		}
		else if ( tmpSym.eSymType == EST_Operator )
		{
			// 运算符
			const T_ArbForOperator *pOp = tmpSym.u_Content.pOp;
			bool32 bEatLeft = false;
			bool32 bWaitRight = false;
			if ( pOp->bLeftToRight )
			{
				// 左侧优先
				if ( pOp->iParamCount == 1 )
				{
					// 仅吃掉左侧的就可以了
					bEatLeft = true;
				}
				else if ( 2 == pOp->iParamCount )
				{
					// 先吃左侧，等待右侧
					bEatLeft = true;
					bWaitRight = true;
				}
				else
				{
					////ASSERT(0);	// 不支持其它的，前面有判断，这里不应该出现
				}
			}
			else
			{
				// 右侧优先
				if ( pOp->iParamCount == 1 )
				{
					// 仅等待右侧的就可以了
					bWaitRight = true;
				}
				else if ( 2 == pOp->iParamCount )
				{
					// 先等待右侧，吃掉左侧
					bEatLeft = true;
					bWaitRight = true;
				}
				else
				{
					////ASSERT(0);	// 不支持其它的，前面有判断，这里不应该出现
				}
			}

			// 构建树结构
			if ( bEatLeft )
			{
				// 你在等待参数，我却要吃掉左边的参数，互相瞪眼~~
				if ( NULL != pframe->pLastWaitOp )
				{	
					////ASSERT(0);
					bParseOk = false;
					CString StrErr;
					StrErr.Format(_T("运算符 %s 找不到参数运算"), pframe->pLastWaitOp->symbol.StrVar.GetBuffer());
					SetLastErrorText(StrErr);
					SetErrorPos(pframe->pLastWaitOp->symbol.pszFormulaPos - ptszOrg);
					break;	// 退出构建循环
				}

				if ( NULL == pframe->pLastVar )
				{
					bParseOk = false;
					CString StrErr;
					StrErr.Format(_T("运算符 %s 找不到参数运算"), pOp->szOp);
					SetLastErrorText(StrErr);
					SetErrorPos(tmpSym.pszFormulaPos - ptszOrg);
					break;	// 退出构建循环
				}

				if ( NULL == pframe->pLastVar->pParent )
				{
					// 取走这个无主参数
					// 既然存在无主参数，则根节点此时必然要为NULL，否则就是错误
					T_SymbolTreeNode *pVar = pframe->pLastVar;
					ASSERT( pframe->pRoot == pVar );	// 根节点必然指向这个无主参数
					pNewNode->aChilds.push_back(pVar);
					pVar->pParent = pNewNode;
					
					if ( pframe->pRoot == pVar )
					{
						pframe->pRoot = pNewNode;	// 如果取的是一个根参数，则修改根为自己
					}
				}
				else
				{
					// 取走左侧的变量，更改结构关系
					// 不论左侧是一个运算符还是一个变量，现在它应该都能产生一个变量的值
					T_SymbolTreeNode *pLeft = pframe->pLastVar;
					// 如果我的优先级比它的父亲高，则抢了这个变量并将我作为这个变量的代言，退出判断
					// 如果优先级<=它的父亲，则将它的父亲作为这个变量的代言，重复判断
					T_SymbolTreeNode *pParent  = pLeft->pParent;
					while ( NULL != pParent )
					{
						ASSERT( pParent->symbol.eSymType == EST_Operator );
						// 根据左右结合性，判断同级优先差别
						bool32 bLessThanMe = pParent->symbol.iMyPriority < tmpSym.iMyPriority;
						if ( !bLessThanMe && pParent->symbol.iMyPriority == tmpSym.iMyPriority )
						{
							// 和它的父一样的优先级，而它父视这个变量为左或者右，则让同优先级的我吃掉这个变量先算完
							// 新出的运算符必然为右侧
							for ( int32 j=0; j < pParent->aChilds.size() ; ++j )
							{
								if ( pParent->aChilds[j] == pLeft )
								{
									if ( pParent->symbol.u_Content.pOp->bLeftToRight )
									{
										// 左侧先, 新出现的运算符必然位于右侧
										bLessThanMe = false;
										ASSERT( !bLessThanMe );	// 目前出现的运算符不会出现这种情况
									}
									else
									{
										// 右侧优先级高
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
									pParent->aChilds[j] = pNewNode;	// 抢走
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
						// 都不比我低，o(︶︿︶)o 唉，我就是最顶层了
						pLeft->pParent = pNewNode;
						pNewNode->aChilds.push_back(pLeft);

						pframe->pRoot = pNewNode;	// 顶层更改，修改根
					}
					else
					{
						ASSERT( pNewNode->aChilds.size() > 0 && pNewNode->pParent == pParent && pLeft->pParent == pNewNode );
					}
				}
			} // 吃左侧
			else
			{
				// 不吃左侧，则左侧一定要存在能消化我的运算结果的运算符
				if ( NULL == pframe->pLastWaitOp && NULL != pframe->pLastVar )
				{
					bParseOk = false;
					CString StrErr;
					StrErr.Format(_T("参数 %s 找不到运算符运算"), pframe->pLastVar->symbol.StrVar.GetBuffer());
					SetLastErrorText(StrErr);
					SetErrorPos(pframe->pLastVar->symbol.pszFormulaPos - ptszOrg);
					break; // 退出构建
				}
			}

			if ( bWaitRight )
			{
				if ( NULL == pframe->pLastWaitOp )
				{
					// 这个域内还没有等待的，表示我在等待
					pframe->pLastWaitOp = pNewNode;
				}
				else
				{
					// 已经有等待的了，表示你等待的我，我在等待(我代替你在等待)
					// 是否要做优先级等判断？
					T_SymbolTreeNode *pLastWait = pframe->pLastWaitOp;
					pLastWait->aChilds.push_back(pNewNode);
					pNewNode->pParent = pLastWait;
					pframe->pLastWaitOp = pNewNode;
				}
			}
		}
		else if ( tmpSym.eSymType == EST_Count )
		{
			// 括号调整
			bool32 bErr = false;
			if ( tmpSym.u_Content.dwFlag == 0 )
			{
				// ( 开始
				// 创建一个新空根节点，标志现在到了一个新的括号
				if ( pframe->pLastVar != NULL && pframe->pLastWaitOp == NULL )
				{
					// 最后一个变量存在且没有操作符消耗掉将要出现的()变量的情况下不能开始(，这是将会是两个变量相连的情况
					bParseOk = false;
					CString StrErr;
					StrErr.Format(_T("( 不能直接出现在变量或常数右边"));
					SetLastErrorText(StrErr);
					SetErrorPos(tmpSym.pszFormulaPos - ptszOrg);
					break; // 退出解析
				}

				++iInParenthesisIndex;
				if ( iInParenthesisIndex > 0 && iInParenthesisIndex <= 200 ) // 不能太多了
				{
					aFrames.push_back(frameDefault);
					pframe = NULL;	// 当前帧已经变更失效
					if ( iInParenthesisIndex == aFrames.size()-1 )
					{
						// 已经进入新的环境了
					}
					else
					{
						////ASSERT(0);	// 除了这里和)，其它地方不能变化
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
				// ) 结束
				// 将处理好的新的空根节点，类似于一个变量出来的处理
				if ( NULL == pframe->pRoot )
				{
					// 没有计算的结果，错误
					bParseOk = false;
					CString StrErr;
					StrErr.Format(_T(") 结束处并不能计算出数据结果"));
					SetLastErrorText(StrErr);
					SetErrorPos(tmpSym.pszFormulaPos - ptszOrg);
					break; // 退出解析
				}
				else if ( NULL != pframe->pLastWaitOp )
				{
					// 有运算符缺少参数，错误
					bParseOk = false;
					CString StrErr;
					StrErr.Format(_T("运算符 %s 缺少参数"), pframe->pLastWaitOp->symbol.StrVar.GetBuffer());
					SetLastErrorText(StrErr);
					SetErrorPos(pframe->pLastWaitOp->symbol.pszFormulaPos - ptszOrg);
					break; // 退出解析
				}
				else if ( NULL != pframe->pLastVar && pframe->pLastVar->pParent == NULL )
				{
					// 有无主变量，该无主变量必然是root
					ASSERT( pframe->pLastVar == pframe->pRoot );
				}

				--iInParenthesisIndex;
				if ( iInParenthesisIndex >=0 && iInParenthesisIndex < 200 )
				{
					// 将计算结果当作是一个变量赋值给上一个帧的等待运算符和最后变量
					T_SymFrame *pPreFrame = &aFrames[iInParenthesisIndex];
					T_SymbolTreeNode *pResultNode = pframe->pRoot;
					ASSERT( pResultNode->pParent == NULL );
					if ( NULL == pPreFrame->pRoot )
					{
						pPreFrame->pRoot = pResultNode;	// 原来没有根节点，那结果节点就是原来的根节点了
						ASSERT( NULL == pPreFrame->pLastWaitOp && NULL == pPreFrame->pLastVar );
					}

					if ( !DoParseAddCalcVar(pPreFrame, pResultNode) )
					{
						bParseOk = false;
						break;	// 出现错误
					}

					aFrames.pop_back();
					pframe = NULL;	// 当前帧已经变更，失效
					if ( iInParenthesisIndex == aFrames.size()-1 )
					{
						
					}
					else
					{
						////ASSERT(0);	// 其它地方不能变更这个的值
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
				// 与括号有关的错误
				bParseOk = false;
				CString StrErr;
				StrErr.Format(_T("符号 %s 处发生不能解析的错误"), (LPCTSTR)tmpSym.StrVar);
				SetLastErrorText(StrErr);
				SetErrorPos(tmpSym.pszFormulaPos - ptszOrg);
				break; // 退出解析
			}
		}
		else
		{
			////ASSERT(0);
			// 其它不识别的类型
		}

		if ( tmpSym.eSymType == EST_Const || tmpSym.eSymType == EST_Var || tmpSym.eSymType == EST_Operator )
		{
			if (NULL != pframe && NULL == pframe->pRoot )
			{
				pframe->pRoot = pNewNode;	// 没有根，那么这个变量或者运算符就是这个的根了
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
			StrErr.Format(_T("() 对不能正常解析"));
			SetLastErrorText(StrErr);
			SetErrorPos(-1);
		}
		else if ( aFrames[0].pRoot == NULL )
		{
			////ASSERT(0);
			bParseOk = false;
			CString StrErr;
			StrErr.Format(_T("没有可以解析的数据"));
			SetLastErrorText(StrErr);
			SetErrorPos(-1);
		}
		else if ( aFrames[0].pLastVar == NULL )
		{
			////ASSERT(0);
			bParseOk = false;
			CString StrErr;
			StrErr.Format(_T("没有可以解析的参数"));
			SetLastErrorText(StrErr);
			SetErrorPos(-1);
		}
		else if ( aFrames[0].pLastVar->pParent == NULL && aFrames[0].pLastVar != aFrames[0].pRoot )
		{
			////ASSERT(0);
			bParseOk = false;
			CString StrErr;
			StrErr.Format(_T("参数 %s 缺少运算符"), aFrames[0].pLastVar->symbol.StrVar.GetBuffer());
			SetLastErrorText(StrErr);
			SetErrorPos(aFrames[0].pLastVar->symbol.pszFormulaPos - ptszOrg);
		}
		else if ( aFrames[0].pLastWaitOp != NULL )
		{
			////ASSERT(0);
			bParseOk = false;
			CString StrErr;
			StrErr.Format(_T("运算符 %s 缺少参数"), aFrames[0].pLastWaitOp->symbol.StrVar.GetBuffer());
			SetLastErrorText(StrErr);
			SetErrorPos(aFrames[0].pLastWaitOp->symbol.pszFormulaPos - ptszOrg);
		}
		else
		{
			// 所有分配的节点都应该可以上溯到跟 - 除了非运算符与变量
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
					// 有节点没有镶嵌进去，必然是没有预料到的错误类型:)
					bParseOk = false;
					////ASSERT(0);
					ASSERT( pNode != NULL ); // 此时pNode指向它的最顶层
					if ( NULL == pNode )
					{
						////ASSERT(0);
						
						CString StrErr;
						StrErr.Format(_T("未知的解析错误"));
						SetLastErrorText(StrErr);
						SetErrorPos(-1);
					}
					else
					{

						CString StrErr;
						StrErr.Format(_T("未知的解析错误"));
						switch (pNode->symbol.eSymType)
						{
						case EST_Var:
						case EST_Const:
							StrErr.Format(_T("变量 %s 缺少运算符运算"), pNode->symbol.StrVar.GetBuffer());
							break;
						case EST_Operator:
							StrVar.Format(_T("运算符 %s 缺少运算参数"), pNode->symbol.StrVar.GetBuffer());
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
		// 释放没有成树的数据
		for ( int32 j=0; j < aDels.size() ; ++j )
		{
			delete aDels[j];
		}
		aDels.clear();
		return false;
	}

	T_SymbolTreeNode *pRoot = aFrames[0].pRoot;
	//DumpNode(pRoot, 0);

	// 公式字面检查完毕
	if ( NULL != ppRootNode )
	{
		ASSERT( NULL == *ppRootNode );	// 必须要保证传过来的是一个指向空的指针
		*ppRootNode = pRoot;	// 传出去
	}
	else
	{
		FreeSymTreeNode(pRoot);	// 没有输出，自然只能释放咯
	}
	return true;
}

bool32 CArbitrageFormula::ParseFormula()
{
	ClearParse();
	if (ParseFormula(&m_pSymbolTree))
	{
		VisitTree2(m_pSymbolTree, m_aCalcStack);	// 建立运算栈信息
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
		// 运算符，仅有可能是匹配这个的，有可能匹配同名的其它运算符
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

	// 后序遍历 左右。。中
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
	// 后序遍历 左右。。中
	SymbolNodePtrArray aStack;
	typedef map<T_SymbolTreeNode *, int> NodeVisitCountMap;
	NodeVisitCountMap mapCount;
	aStack.reserve(100);
	T_SymbolTreeNode *pNode = pRootNode;
	while ( NULL != pNode )
	{
		// 有子节点，先遍历所有子节点
		while ( NULL != pNode )
		{
			NodeVisitCountMap::iterator it = mapCount.find(pNode);
			// 第一次访问，给与左节点，如果所有节点都遍历过了，输出自己
			if ( it == mapCount.end() )
			{
				it = mapCount.insert(NodeVisitCountMap::value_type(pNode, 0)).first;
			}

			if ( it->second < pNode->aChilds.size() )
			{
				// 现在要访问第n个子节点了
				aStack.push_back(pNode);
				pNode = pNode->aChilds[it->second];
				it->second++;	// 下次访问下一个
			}
			else
			{
				// 所有子节点都访问完了
				// 轮到输出自己了
				ASSERT( it->second == pNode->aChilds.size() );
				break;
			}
		}

		// 节点访问
		aNodes.push_back(pNode);	// 

		// 弹出要访问的上一个节点

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
	TRACE(_T("  特殊变量(%d): "), m_varSpecialNames.size());
	for ( ArbitrageVarNameSet::reverse_iterator itSpe=m_varSpecialNames.rbegin(); itSpe != m_varSpecialNames.rend() ; ++itSpe )
	{
		TRACE(_T("%s "), (*itSpe).GetBuffer());
	}
	TRACE(_T("\r\n"));
	if ( !IsParsed() )
	{
		TRACE(_T("  未解析\r\n"));
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
		TRACE(_T("计算步骤:\r\n"));
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
				aVarSyms.Add(pNode->symbol);	// 变量入队尾
			}
			else if ( pNode->symbol.eSymType == EST_Operator )
			{
				// 看该操作符是一元还是二元操作符
				// 弹出特定数量的数据，压入一个数据				
				const T_ArbForOperator *pOp = pNode->symbol.u_Content.pOp;
				TRACE(_T("  步骤%d.  操作符:%s 参数: "), iStep, pOp->szOp);
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
					TRACE(_T("无法进行计算！\r\n"));
					break;
				}

				aVarSyms.RemoveAt(aVarSyms.GetSize()-pOp->iParamCount, pOp->iParamCount);

				// 压入计算结果
				nodeResult.symbol.StrVar.Format(_T("?%d"), iStep);
				aVarSyms.Add(nodeResult.symbol);
				TRACE(_T(" 结果: %s\r\n"), nodeResult.symbol.StrVar.GetBuffer());

				iStep++;
			}
			else
			{
				////ASSERT(0);	// 不应该有其他的
				TRACE(_T("错误的类型：\r\n"));
				DumpSymbol(pNode->symbol);
				break;
			}
		}
		ASSERT( aVarSyms.GetSize()==1 && (aVarSyms[0].StrVar==nodeResult.symbol.StrVar || (aTrack.size()==1 && aTrack[0]->symbol.pszFormulaPos==aVarSyms[0].pszFormulaPos)) );
		if ( aVarSyms.GetSize() == 1 )
		{
			TRACE(_T("最终结果: %s\r\n"), iStep==0?aVarSyms[0].StrVar.GetBuffer() : nodeResult.symbol.StrVar.GetBuffer());
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
		TRACE(_T(" 符号-常量:%s 位置:%d 优先级:%d\r\n"), CString(symbol.StrVar).GetBuffer(), iPos, symbol.iMyPriority);
		break;
	case EST_Var:
		TRACE(_T(" 符号-变量:%s 位置:%d 优先级:%d\r\n"), CString(symbol.StrVar).GetBuffer(), iPos, symbol.iMyPriority);
		break;
	case EST_Operator:
		TRACE(_T(" 符号-运算符:%s 位置:%d 优先级:%d\r\n"), CString(symbol.StrVar).GetBuffer(), iPos, symbol.iMyPriority);
		break;
	case EST_TmpValue:
		TRACE(_T(" 符号-临时值:%s 临时索引:%d 位置:%d 优先级:%d\r\n"), symbol.u_Content.iTmpIndex, CString(symbol.StrVar).GetBuffer(), iPos, symbol.iMyPriority);
		break;
	case EST_Count:
		if ( symbol.u_Content.dwFlag == 0 )
		{
			TRACE(_T(" 标记(:%s 位置:%d \r\n"), CString(symbol.StrVar).GetBuffer(), iPos);
		}
		else if ( symbol.u_Content.dwFlag == 1 )
		{
			TRACE(_T(" 标记):%s 位置:%d \r\n"), CString(symbol.StrVar).GetBuffer(), iPos);
		}
		else
		{
			TRACE(_T(" 无法识别标记:%s 位置:%d \r\n"), CString(symbol.StrVar).GetBuffer(), iPos);
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

	TRACE(_T("%d级 节点 %p, 子节点: %d\r\n"), iHeaderEmpty, pRootNode, pRootNode->aChilds.size());
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
		// 有等待右侧的计算，那么前面如果有变量，则变量必须要被吃了
		if ( NULL != pframe->pLastVar )
		{
			if ( NULL == pframe->pLastVar->pParent )
			{
				// 没有被吃掉，但是这个运算符只吃后面的，错误
				bParseOk = false;
				CString StrErr;
				if ( pframe->pLastVar->symbol.eSymType == EST_Const || pframe->pLastVar->symbol.eSymType == EST_Var )
				{
					StrErr.Format(_T("参数 %s 缺少运算符运算"), pframe->pLastVar->symbol.StrVar.GetBuffer());
					SetErrorPos(pframe->pLastVar->symbol.pszFormulaPos - ptszOrg);
				}
				else
				{
					StrErr.Format(_T("参数 %s 的上一个运算参数缺少运算符运算"), sym.StrVar.GetBuffer());
					SetErrorPos(sym.pszFormulaPos - ptszOrg);
				}
				SetLastErrorText(StrErr);
				return bParseOk; // 退出解析
			}
		}
		ASSERT( pframe->pLastWaitOp->symbol.eSymType == EST_Operator );
		pframe->pLastWaitOp->aChilds.push_back(pNewNode);
		pNewNode->pParent = pframe->pLastWaitOp;
		if ( pframe->pLastWaitOp->aChilds.size() == pframe->pLastWaitOp->symbol.u_Content.pOp->iParamCount )
		{
			pframe->pLastWaitOp = NULL;	// 他的数据ok了
		}
		else
		{
			// 还缺或者超过了，不支持超过1个参数的等待错误，退出
			////ASSERT(0);
			bParseOk = false;
			CString StrErr;
			StrErr.Format(_T("不能处理的运算符 %s"), pframe->pLastWaitOp->symbol.StrVar.GetBuffer());
			SetLastErrorText(StrErr);
			SetErrorPos(pframe->pLastWaitOp->symbol.pszFormulaPos - ptszOrg);
			return bParseOk; // 退出解析
		}
		pframe->pLastVar = pNewNode;	// 后面的运算符要抢就找它父去抢吧
	}
	else
	{
		// 没有最后等待的，
		if ( NULL == pframe->pLastVar )
		{
			pframe->pLastVar = pNewNode;	// 那么这个参数就是最后的参数
		}
		else
		{
			// 以前有变量且没有等待的op，则代表这个变量没有被消化(就算被吃了，也没有运算符来计算两变量关系)的情况下又来了一个变量
			// 同frame中两个变量之间没有运算符是错误的
			////ASSERT(0);
			bParseOk = false;
			CString StrErr;
			StrErr.Format(_T("参数 %s 与 参数 %s 之间缺少运算符"), sym.StrVar.GetBuffer(), pframe->pLastVar->symbol.StrVar.GetBuffer());
			SetLastErrorText(StrErr);
			SetErrorPos(sym.pszFormulaPos - ptszOrg);
			return bParseOk; // 退出解析
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
		// 此时所有变量node的pvar应当均为NULL

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
		pRootDst->symbol.u_Content.pVar = NULL;	// 指向特别的指针不copy
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
				pSubDst->symbol.u_Content.pVar = NULL;	// 指向特别的指针不copy
			}
			pSubDst->pParent = pDst;
			pDst->aChilds.push_back(pSubDst);

			// 此时父节点还在下面
			aStackSrc.push_back(pSubSrc);
			aStackDst.push_back(pSubDst);
		}
		else
		{
			ASSERT( pDst->aChilds.size() == pSrc->aChilds.size() );
			aStackSrc.pop_back();	// 已经完成，取走父节点
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
	return true;	// 既然有解析数据，则能成功解析
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
				StrErr.Format(_T("没有提供参数 %s"), pNode->symbol.StrVar.GetBuffer());
				SetLastErrorText(StrErr);
				SetErrorPos(pNode->symbol.pszFormulaPos - m_StrFormula.operator LPCTSTR());
				return false;	// 没有提供这个变量，则不知道怎么处理
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
		StrErr.Format(_T("无效计算范围[%d-%d]"), iMinIndex, iMaxIndex);
		SetLastErrorText(StrErr);
		return false;
	}

	if ( !TestFormula(vars) )
	{
		return false;
	}

	ASSERT( NULL != m_pSymbolTree && m_aCalcStack.size() > 0 );
	// 解释各变量
	// 将变量的指针放到symbol中
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
				// 没有初始化该常量信息，这里初始化
				CArbitrageFormulaVariableConst *pVar = new CArbitrageFormulaVariableConst(pNode->symbol.StrVar);
				TCHAR *p;
				pVar->m_val = (float)_tcstod(pNode->symbol.StrVar, &p);	// 这个浮点数有效没
				it = m_mapConstVar.insert(ArbitrageCalcVarPtrs::value_type(pNode->symbol.StrVar, pVar)).first;
			}
			CArbitrageFormulaVariableAbs *pVar = it->second;
			ASSERT( NULL != pVar );
			ASSERT( pNode->symbol.u_Content.pVar == NULL || pNode->symbol.u_Content.pVar == pVar );	// 必然同时消失
			pNode->symbol.u_Content.pVar = pVar;
		}
		else if ( pNode->symbol.eSymType == EST_Var )
		{
			// 变量由参数提供，不论输入输出
			ArbitrageVarPtrs::iterator it = vars.find(pNode->symbol.StrVar);
			if ( it == vars.end() || NULL == it->second )
			{
				// 没有则错误
				CString StrErr;
				StrErr.Format(_T("参数未初始化 %s"), pNode->symbol.StrVar.GetBuffer());
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
	
	// 按照计算栈计算，计算栈为后序排列，最靠近运算符左边的是右操作数
	// 顺序为calcStack 0..n 参数左 参数右 运算符
	// 默认非法结果为 ARB_INVALID_VALUE
	SymbolArray	aResultStack;
	aResultStack.SetSize(0, m_aCalcStack.size());
#ifdef _DEBUGARB
	TRACE(_T("套利公式计算1: %s 索引[%d-%d]\r\n"), m_StrFormula, iMinIndex, iMaxIndex);
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
	VarArray aTmpVar;	// 临时结果集
	aTmpVar.reserve(20);
	T_ArbForOperatorParam param = {0};
	param.iMinIndex = 0;	// 当前索引
	param.iMaxIndex = -1;

	try
	{
		for ( int32 iIndex=iMinIndex; iIndex <= iMaxIndex && !bErr ; ++iIndex )
		{
			int32 iStep = 0;
#ifdef _DEBUGARB
			if ( iIndex-iMinIndex < 3 )
			{
				TRACE(_T("  索引%d 计算过程:\r\n"), iIndex);
			}
#endif
			// 重新开始计算另一个
			aResultStack.SetSize(0);
			
			m_VarLast.SetValue(iIndex, ARB_INVALID_VALUE);	// 最终结果无效话
			m_VarLast.SetCurIndex(iIndex);
			
			// 设置当前索引
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
					// 参数入栈
					aResultStack.Add(pNode->symbol);
				}
				else if ( EST_Operator == pNode->symbol.eSymType )
				{
					if ( aTmpVar.size() < iStep+1 )
					{
						ASSERT( iStep == aTmpVar.size() );
						aTmpVar.insert(aTmpVar.end(), iStep+1-aTmpVar.size(), CArbitrageFormulaVariableVar());	// 增加一个
					}
					CArbitrageFormulaVariableVar &varTmp = aTmpVar[iStep];	// 这次计算的临时变量, 临时变量中的当前值为0
					varTmp.m_iMin = 0;
					varTmp.m_iMax = 1;
					varTmp.SetCurIndex(0);
					varTmp.SetCurValue(ARB_INVALID_VALUE);

					// 运算符运算，并将计算结果入栈，结果为浮点数
					const T_ArbForOperator *pOp = pNode->symbol.u_Content.pOp;
					if (NULL == pOp)
					{
						continue;
					}
					
					// 参数栈中参数0位置为最左的参数
					if ( aResultStack.GetSize() < pOp->iParamCount )
					{
						////ASSERT(0);
						CString StrErr;
						StrErr.Format(_T("计算 %s 参数耗尽"), pOp->szOp);
						SetLastErrorText(StrErr);
						SetErrorPos(pNode->symbol.pszFormulaPos-m_StrFormula.operator LPCTSTR());
						bErr = true;
						break;	// 不能继续了
					}
					
					if ( pOp->iParamCount > 2 || pOp->iParamCount <= 0 )
					{
						////ASSERT(0);
						CString StrErr;
						StrErr.Format(_T("不支持的运算符 %s"), pOp->szOp);
						SetLastErrorText(StrErr);
						SetErrorPos(pNode->symbol.pszFormulaPos-m_StrFormula.operator LPCTSTR());
						bErr = true;
						break;	// 不能继续了
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
							// 临时变量索引
							ASSERT( sym.u_Content.iTmpIndex >= 0 && sym.u_Content.iTmpIndex < aTmpVar.size() );
							v[iVPos] = &aTmpVar[sym.u_Content.iTmpIndex];
						}
						else
						{
							////ASSERT(0);	// 参数栈中不会有其它类型
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
						
						TRACE(_T("    计算步骤 %d: 运算符:%s 位置:%d 参数: "), iStep, pOp->szOp, pNode->symbol.pszFormulaPos-m_StrFormula.operator LPCTSTR());
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
								// 临时变量索引
								ASSERT( sym.u_Content.iTmpIndex >= 0 && sym.u_Content.iTmpIndex < aTmpVar.size() );
								pVar = &aTmpVar[sym.u_Content.iTmpIndex];
							}
							else
							{
								////ASSERT(0);	// 参数栈中不会有其它类型
							}
							ASSERT( NULL != pVar );
							float fV = ARB_INVALID_VALUE;
							if ( pVar->GetCurValue(fV) && fV != ARB_INVALID_VALUE )
							{
								TRACE(_T("(%0.2f) "), fV);
							}
							else
							{
								TRACE(_T("(无效) "));
							}
						}
						
						float fV = ARB_INVALID_VALUE;
						TRACE(_T("  输出:%s"), symTmp2.StrVar);
						if ( varTmp.GetCurValue(fV) && fV != ARB_INVALID_VALUE )
						{
							TRACE(_T("(%0.2f)"), fV);
						}
						else
						{
							TRACE(_T("(无效)"));
						}
						
						TRACE(_T("\r\n"));
					}
#endif
					aResultStack.RemoveAt(aResultStack.GetSize()-pOp->iParamCount, pOp->iParamCount);
					aResultStack.Add(symTmp2);
					
					++iStep;	// 步骤+1
				}
				else
				{
					////ASSERT(0);	// 不应该出现其它类型
				}
			}// 所有步骤完了
			if ( aResultStack.GetSize() != 1 )
			{
				////ASSERT(0);
				CString StrErr;
				StrErr.Format(_T("计算参数多余或者耗尽(%d)"), aResultStack.GetSize());
				SetLastErrorText(StrErr);
				SetErrorPos(-1);
				bErr = true;
				break;	// 不能继续了
			}
			else
			{
				// 最终变量保存进最后表达式结果
				const T_Symbol &sym = aResultStack[0];
				CArbitrageFormulaVariableAbs *pVar = NULL;
				if ( sym.eSymType == EST_Var || sym.eSymType == EST_Const )
				{
					ASSERT( sym.u_Content.pVar != NULL );
					pVar = sym.u_Content.pVar;
				}
				else if ( sym.eSymType == EST_TmpValue )
				{
					// 临时变量索引
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
					TRACE(_T("  索引%d最终变量: %s "), iIndex, sym.StrVar);
					if ( bV )
					{
						TRACE(_T("(%0.2f)"), fV);
					}
					else
					{
						TRACE(_T("(无效)"));
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
		TRACE(_T("获利计算异常：") + Str + _T("\r\n"));

		CString StrErr;
		StrErr.Format(_T("计算异常！"));
		SetLastErrorText(StrErr);
		SetErrorPos(-1);
	}
	catch (...)
	{
		bErr = true;
		TRACE(_T("获利计算未知异常\r\n"));

		CString StrErr;
		StrErr.Format(_T("计算未知异常！"));
		SetLastErrorText(StrErr);
		SetErrorPos(-1);
	}

	if ( bErr )
	{
		if ( GetLastErrorText().IsEmpty() )
		{
			SetLastErrorText(_T("获利计算位置错误"));
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
		StrErr.Format(_T("无效计算范围[%d-%d]"), iMinIndex, iMaxIndex);
		SetLastErrorText(StrErr);
		return false;
	}

	if ( !TestFormula(vars) )
	{
		return false;
	}

	ASSERT( NULL != m_pSymbolTree && m_aCalcStack.size() > 0 );
	// 解释各变量
	// 将变量的指针放到symbol中
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
				// 没有初始化该常量信息，这里初始化
				CArbitrageFormulaVariableConst *pVar = new CArbitrageFormulaVariableConst(pNode->symbol.StrVar);
				TCHAR *p;
				pVar->m_val = (float)_tcstod(pNode->symbol.StrVar, &p);	// 这个浮点数有效没
				it = m_mapConstVar.insert(ArbitrageCalcVarPtrs::value_type(pNode->symbol.StrVar, pVar)).first;
			}
			CArbitrageFormulaVariableAbs *pVar = it->second;
			ASSERT( NULL != pVar );
			ASSERT( pNode->symbol.u_Content.pVar == NULL || pNode->symbol.u_Content.pVar == pVar );	// 必然同时消失
			pNode->symbol.u_Content.pVar = pVar;
		}
		else if ( pNode->symbol.eSymType == EST_Var )
		{
			// 变量由参数提供，不论输入输出
			ArbitrageVarPtrs::iterator it = vars.find(pNode->symbol.StrVar);
			if ( it == vars.end() || NULL == it->second )
			{
				// 没有则错误
				CString StrErr;
				StrErr.Format(_T("参数未初始化 %s"), pNode->symbol.StrVar.GetBuffer());
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
	
	// 按照计算栈计算，计算栈为后序排列，最靠近运算符左边的是右操作数
	// 顺序为calcStack 0..n 参数左 参数右 运算符
	// 默认非法结果为 ARB_INVALID_VALUE

	// 可以修改symbol结构体，使每个运算符symbol都能匹配一个临时变量结果
	// 根节点的运算符结果就是最后变量结果，如果根节点不是运算符，则根节点变量本身就是运算结果
	// 未实现

	SymbolArray	aResultStack;
	aResultStack.SetSize(0, m_aCalcStack.size());
#ifdef _DEBUGARB
	TRACE(_T("套利公式计算2: %s 索引[%d-%d]\r\n"), m_StrFormula, iMinIndex, iMaxIndex);
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
	VarArray aTmpVar;	// 临时结果集
	aTmpVar.reserve(100);
	T_ArbForOperatorParam param = {0};
	param.iMinIndex = iMinIndex;	// 所有索引
	param.iMaxIndex = iMaxIndex;

	try
	{
		int32 iStep = 0;
		// 重新开始计算另一个
		aResultStack.SetSize(0);

#ifdef _DEBUGARB
		// 设立所有变量的cur index为最小值
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
				// 参数入栈
				aResultStack.Add(pNode->symbol);
			}
			else if ( EST_Operator == pNode->symbol.eSymType )
			{
				if ( aTmpVar.size() < iStep+1 )
				{
					ASSERT( iStep == aTmpVar.size() );
					aTmpVar.insert(aTmpVar.end(), iStep+1-aTmpVar.size(), CArbitrageFormulaVariableVar());	// 增加一个
				}
				CArbitrageFormulaVariableVar &varTmp = aTmpVar[iStep];	// 这次计算的临时变量, 临时变量中存放所有结果
				varTmp.SetMinIndex(iMinIndex);
				varTmp.SetMaxIndex(iMaxIndex);
				varTmp.SetCurIndex(iMinIndex);
				
				// 运算符运算，并将计算结果入栈，结果为浮点数
				const T_ArbForOperator *pOp = pNode->symbol.u_Content.pOp;
				if (NULL == pOp)
				{
					continue;
				}
				
				// 参数栈中参数0位置为最左的参数
				if ( aResultStack.GetSize() < pOp->iParamCount )
				{
					////ASSERT(0);
					CString StrErr;
					StrErr.Format(_T("计算 %s 参数耗尽"), pOp->szOp);
					SetLastErrorText(StrErr);
					SetErrorPos(pNode->symbol.pszFormulaPos-m_StrFormula.operator LPCTSTR());
					bErr = true;
					break;	// 不能继续了
				}
				
				if ( pOp->iParamCount > 2 || pOp->iParamCount <= 0 )
				{
					////ASSERT(0);
					CString StrErr;
					StrErr.Format(_T("不支持的运算符 %s"), pOp->szOp);
					SetLastErrorText(StrErr);
					SetErrorPos(pNode->symbol.pszFormulaPos-m_StrFormula.operator LPCTSTR());
					bErr = true;
					break;	// 不能继续了
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
						// 临时变量索引
						ASSERT( sym.u_Content.iTmpIndex >= 0 && sym.u_Content.iTmpIndex < aTmpVar.size() );
						v[iVPos] = &aTmpVar[sym.u_Content.iTmpIndex];
					}
					else
					{
						////ASSERT(0);	// 参数栈中不会有其它类型
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
					
					TRACE(_T("    计算步骤 %d: 运算符:%s 位置:%d 参数: "), iStep, pOp->szOp, pNode->symbol.pszFormulaPos-m_StrFormula.operator LPCTSTR());
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
							// 临时变量索引
							ASSERT( sym.u_Content.iTmpIndex >= 0 && sym.u_Content.iTmpIndex < aTmpVar.size() );
							pVar = &aTmpVar[sym.u_Content.iTmpIndex];
						}
						else
						{
							////ASSERT(0);	// 参数栈中不会有其它类型
						}
						ASSERT( NULL != pVar );
						float fV = ARB_INVALID_VALUE;
						if ( pVar->GetCurValue(fV) && fV != ARB_INVALID_VALUE )
						{
							TRACE(_T("(%0.2f)"), fV);
						}
						else
						{
							TRACE(_T("(无效)"));
						}
					}
					
					float fV = ARB_INVALID_VALUE;
					TRACE(_T("  输出:%s"), symTmp2.StrVar);
					if ( varTmp.GetCurValue(fV) && fV != ARB_INVALID_VALUE )
					{
						TRACE(_T("(%0.2f)"), fV);
					}
					else
					{
						TRACE(_T("(无效)"));
					}
					
					TRACE(_T("\r\n"));
				}
#endif
				aResultStack.RemoveAt(aResultStack.GetSize()-pOp->iParamCount, pOp->iParamCount);
				aResultStack.Add(symTmp2);
				
				++iStep;	// 步骤+1
			}
			else
			{
				////ASSERT(0);	// 不应该出现其它类型
			}
		}// 所有步骤完了
		
		if ( aResultStack.GetSize() != 1 )
		{
			////ASSERT(0);
			CString StrErr;
			StrErr.Format(_T("计算参数多余或者耗尽(%d)"), aResultStack.GetSize());
			SetLastErrorText(StrErr);
			SetErrorPos(-1);
			bErr = true;
		}
		else
		{
			// 最终变量保存进最后表达式结果
			const T_Symbol &sym = aResultStack[0];
			CArbitrageFormulaVariableAbs *pVar = NULL;
			if ( sym.eSymType == EST_Var || sym.eSymType == EST_Const )
			{
				ASSERT( sym.u_Content.pVar != NULL );
				pVar = sym.u_Content.pVar;
			}
			else if ( sym.eSymType == EST_TmpValue )
			{
				// 临时变量索引
				ASSERT( sym.u_Content.iTmpIndex >= 0 && sym.u_Content.iTmpIndex < aTmpVar.size() );
				pVar = &aTmpVar[sym.u_Content.iTmpIndex];
			}

			if (NULL != pVar)
			{
				// 赋值给最后的结果集
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
				TRACE(_T("  索引%d 最终变量: %s "), pVar->GetCurIndex(), sym.StrVar);
				if ( bV )
				{
					TRACE(_T("(%0.2f)"), fV);
				}
				else
				{
					TRACE(_T("(无效)"));
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
		TRACE(_T("获利计算异常：") + Str + _T("\r\n"));

		CString StrErr;
		StrErr.Format(_T("计算异常！"));
		SetLastErrorText(StrErr);
		SetErrorPos(-1);
	}
	catch (...)
	{
		bErr = true;
		TRACE(_T("获利计算未知异常\r\n"));

		CString StrErr;
		StrErr.Format(_T("计算未知异常！"));
		SetLastErrorText(StrErr);
		SetErrorPos(-1);
	}

	if ( bErr )
	{
		if ( GetLastErrorText().IsEmpty() )
		{
			SetLastErrorText(_T("获利计算位置错误"));
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
		return false;	// 必须先解析公式才有变量
	}
	ASSERT( NULL != m_pSymbolTree && m_aCalcStack.size() > 0 );
	// 不清除以前的变量
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
					// 没有该变量则创建
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
			it->second = NULL;	// 清空该变量的指针
		}
	}
	vars.clear();	// 清空所有结果
}

bool32 CArbitrageFormula::GetUnInitializedVariables( OUT ArbitrageVarPtrs &vars )
{
	if ( !IsParsed() )
	{
		////ASSERT(0);
		return false;	// 必须先解析公式才有变量
	}
	ASSERT( NULL != m_pSymbolTree && m_aCalcStack.size() > 0 );
	// 不清除以前的变量
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
					// 给与未初始化值
					vars[sym.StrVar] = NULL;
				}
			}
		}		
	}
	
	return true;
}

bool CArbitrageFormula::operator==( const CArbitrageFormula &for2 ) const
{
	// 仅判断公式内容是否相同
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
		// 集合运算
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
		// 集合运算
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
		// 集合运算
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
		// 集合运算
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
		// 集合运算
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
		// 集合运算
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
		// 集合运算
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
		// 集合运算
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
