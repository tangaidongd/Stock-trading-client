#ifndef _ARBITRAGEFORMULA_H_
#define _ARBITRAGEFORMULA_H_

#include <afxtempl.h>
#include <float.h>
#include <vector>
#include <map>
#include <set>
#include "typedef.h"
#include "TechExport.h"
using std::vector;
using std::map;
using std::multimap;
using std::set;

#define ARB_INVALID_VALUE		(FLT_MAX)		// 用来表示计算的无效值

// 这个里面用的值全部都是float表示的，so值类型约定为float
typedef float			ArbitrageValueType;
typedef vector<ArbitrageValueType>	ArbitrageValueArray;
class CArbitrageFormulaVariableAbs
{
public:
	CArbitrageFormulaVariableAbs() { m_iCurIndex = -1; }
	virtual ~CArbitrageFormulaVariableAbs(){};

	virtual bool32 GetValue(int32 iIndex, OUT ArbitrageValueType &val) = 0;
	virtual bool32 SetValue(int32 iIndex, const ArbitrageValueType &val) = 0;
	virtual bool32 IsEnableSet() = 0;	// 是否可写
	virtual int32  GetMinIndex() = 0;	// 最小索引值
	virtual int32  GetMaxIndex() = 0;	// 最大索引值

	virtual bool32	SetMinIndex(int32 iMinIndex) = 0;
	virtual bool32	SetMaxIndex(int32 iMaxIndex) = 0;

	void	SetVarName(LPCTSTR ptszVarName) { m_StrVarName = ptszVarName; }
	const CString &GetVarName() const { return m_StrVarName; }

	void	SetCurIndex(int32 iCur) { m_iCurIndex = iCur; }
	int32	GetCurIndex() const { return m_iCurIndex; }
	bool32	GetCurValue(OUT ArbitrageValueType &val) { return GetValue(m_iCurIndex, val); }
	bool32	SetCurValue(const ArbitrageValueType &val) { return SetValue(m_iCurIndex, val); }

protected:
	CString		m_StrVarName;	// 变量名称
	int32		m_iCurIndex;		// 当前索引
};

// 可变变量
class CArbitrageFormulaVariableVar : public CArbitrageFormulaVariableAbs
{
public:
	CArbitrageFormulaVariableVar(){ m_iMin=0; m_iMax = -1; };
	CArbitrageFormulaVariableVar(LPCTSTR ptszVarName){ SetVarName(ptszVarName);m_iMin=0; m_iMax = -1; };

	virtual bool32 GetValue(int32 iIndex, OUT ArbitrageValueType &val)
	{
		if ( iIndex>=0 && iIndex >= m_iMin && iIndex <= m_iMax && iIndex < m_aValues.size() )
		{
			val = m_aValues[iIndex];
			return val != FLT_MAX;
		}
		return false;
	}

	virtual bool32 SetValue(int32 iIndex, const ArbitrageValueType &val)
	{
		if ( iIndex>=0 && iIndex >= m_iMin && iIndex <= m_iMax && iIndex < m_aValues.size() )
		{
			m_aValues[iIndex] = val;
			return true;
		}
		else if ( iIndex >= 0 && iIndex >= m_iMin && iIndex <= m_iMax )
		{
			// 扩展
			m_aValues.reserve(m_iMax+1);
			m_aValues.insert(m_aValues.end(), m_iMax+1-m_aValues.size(), ARB_INVALID_VALUE);
			m_aValues[iIndex] = val;
			return true;
		}
		return false;
	}

	virtual bool32 IsEnableSet() { return true; }	// 变量总是可写的

	virtual int32	GetMinIndex() { return m_iMin; }
	virtual int32	GetMaxIndex() { return m_iMax; }
	virtual bool32	SetMinIndex(int32 iMinIndex) { m_iMin = iMinIndex; return false; }
	virtual bool32	SetMaxIndex(int32 iMaxIndex) { m_iMax = iMaxIndex; return false; }

	ArbitrageValueArray m_aValues;
	int32			m_iMin;
	int32			m_iMax;
};

// 常量
class CArbitrageFormulaVariableConst : public CArbitrageFormulaVariableAbs
{
public:
	CArbitrageFormulaVariableConst(){};
	CArbitrageFormulaVariableConst(LPCTSTR ptszVarName){ SetVarName(ptszVarName); };

	virtual bool32 GetValue(int32 iIndex, OUT ArbitrageValueType &val) { val = m_val; return true; }
	virtual bool32 SetValue(int32 iIndex, const ArbitrageValueType &val) { return false; }
	virtual bool32 IsEnableSet() { return false; }	// 常量禁止写

	virtual int32	GetMinIndex() { return 0; }
	virtual int32	GetMaxIndex() { return LONG_MAX; }

	virtual bool32	SetMinIndex(int32 iMinIndex) { return false; }
	virtual bool32	SetMaxIndex(int32 iMaxIndex) { return false; }

	ArbitrageValueType	m_val;
};

typedef map<CString, CArbitrageFormulaVariableVar *>	ArbitrageVarPtrs; // 保证计算时的指针有效性
typedef map<CString, CArbitrageFormulaVariableAbs *>	ArbitrageCalcVarPtrs; // 保证计算时的指针有效性
typedef	set<CString>									ArbitrageVarNameSet;	// 变量名称集

struct T_ArbForOperatorParam
{
	CArbitrageFormulaVariableAbs *pVar1;
	CArbitrageFormulaVariableAbs *pVar2;
	CArbitrageFormulaVariableAbs *pVarOther;
	int32						  iVarOtherCount;
	int32						  iMinIndex;		// 计算开始处索引 - 如果minIndex > maxIndex则使用当前索引，作为计算当前值的标志
	int32						  iMaxIndex;		// 计算结束处索引
};

typedef bool32 (*ARB_FOR_OPERATOR_FUNC)(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
#define ARB_FOR_PRIORITY_MIN  (0)
#define ARB_FOR_PRIORITY_MAX  (9)
struct T_ArbForOperator
{
	TCHAR				  szOp[10];	// 运算符字串
	int32				  iParamCount; // 运算符必须且能接收的参数个数
	int32				  iOpPriority;		// 运算优先级，数值越大优先级越高, 现在预定0-9级，+-为5级 */为6级 =为2级, ,为1级
	bool32				  bLeftToRight;		// 结合性至左向右，如果false则从右到左
	ARB_FOR_OPERATOR_FUNC func;
};


class EXPORT_CLASS CArbitrageFormula
{
public:
	enum E_SymbolType
	{
		EST_Const = 0,	// 常量
		EST_Var,		// 变量
		EST_Operator,	// 运算符

		EST_TmpValue,		// 临时变量，作为存储中间值是用
		EST_Count		// 无效
	};

	struct T_Symbol
	{
		E_SymbolType eSymType;
		LPCTSTR		 pszFormulaPos;	// 所指向的formula中位置
		int32		 iMyPriority;	// 如果是运算符，这个代表运算符在这次运算中的优先级
		CString		 StrVar;	// 否则指向一个变量或者数字
		union U_CONTENT{
			const T_ArbForOperator *pOp;	// 如果为运算符类型，则指向运算符
			DWORD			dwFlag;
			int				iTmpIndex;		// 临时类型对应的是一个临时的索引
			CArbitrageFormulaVariableAbs *pVar;	// 指向变量的指针，计算时用，
		} u_Content;
	};

	// 叶子节点必为变量，带子节点必为运算符，根节点如果是变量则仅有根节点一个节点
	struct T_SymbolTreeNode
	{
		T_Symbol symbol;
		
		struct T_SymbolTreeNode *pParent;
		typedef vector<struct T_SymbolTreeNode *> SymbolNodeArray;
		SymbolNodeArray aChilds;
	};

	typedef T_SymbolTreeNode::SymbolNodeArray  SymbolNodePtrArray;
	struct T_SymFrame
	{
		T_SymbolTreeNode *pRoot;		// 跟节点
		T_SymbolTreeNode *pLastVar;		// 最后一个变量数据(可能已经被运算符结果吃了)，一旦被赋值，就不可能再为空
		T_SymbolTreeNode *pLastWaitOp;	// 最后一个等待右参的运算符
		SymbolNodePtrArray	aCalcSort;	// 运算顺序，只有运算符或者唯一的一个变量
	};
	typedef vector<T_SymFrame> SymFrameArray;

	typedef multimap<CString, const T_ArbForOperator *> OpMap;

	typedef CArray<T_Symbol, const T_Symbol &> SymbolArray;
	

public:
	CArbitrageFormula();
	CArbitrageFormula(LPCTSTR ptszFormula);
	CArbitrageFormula(const CArbitrageFormula &for2);

	~CArbitrageFormula();
	
	const CArbitrageFormula &operator = (const CArbitrageFormula &for2){ Copy(for2); return *this; }

	bool operator==(const CArbitrageFormula &for2) const;
	bool operator!=(const CArbitrageFormula &for2) const { return !(*this==for2); }

	// 公式计算，如果错误，可以通过lasterrortext获取错误信息
	// 通过LastVar获取最后一个表达式的计算结果(可能是输出变量结果)
	// [iMinIndex-iMaxIndex]: [最小索引，最大索引]，如[0-10]范围为0-10(含) 11个索引
	bool32	Calculate(INOUT ArbitrageVarPtrs &vars, int32 iMinIndex, int32 iMaxIndex);	// 一个索引一个索引的计算
	bool32	Calculate2(INOUT ArbitrageVarPtrs &vars, int32 iMinIndex, int32 iMaxIndex);	// 全部索引一起计算，性能好点

	bool32	TestFormula(IN const ArbitrageVarPtrs &vars);	// 测试公式，必须提供所有的变量
	bool32	TestFormula();	// 仅计算式是否成立

	void	SetFormula(LPCTSTR ptszFormula);
	const CString &GetFormula()const { return m_StrFormula; }

	const CArbitrageFormulaVariableVar &GetLastVar() const { return m_VarLast; }

	void	SetLastErrorText(LPCTSTR ptszError) { m_StrLastError = ptszError; }
	const CString &GetLastErrorText() const { return m_StrLastError; }
	void	SetErrorPos(int32 iPos) { m_iErrorPos = iPos; }
	int32	GetErrorPos() const { return m_iErrorPos; }

	// 变量，所有变量名区分大小写
	bool32	GetVariables(OUT ArbitrageVarPtrs &vars);	// 获取所有的可变的变量，不用的时候必须要手动delete所有项，或者调用FreeVariable
	void	FreeVariables(INOUT ArbitrageVarPtrs &vars);	// 释放所有可变变量的指针
	bool32	GetUnInitializedVariables(OUT ArbitrageVarPtrs &vars);	// 获取没有分配指针的可变变量，不必释放

	// 特殊变量，某些含特殊字符的变量可能不能正常解析，通过设置特殊变量，指示这些名称都是变量
	// 设置后需要调用ParseFormula生效
	void	SetSpecialVarNames(IN const ArbitrageVarNameSet &varNames) { m_varSpecialNames = varNames; };	// 设置特殊变量名称，以前的名称会清空
	const ArbitrageVarNameSet &GetSpecialVarNames() const { return m_varSpecialNames; }	// 获取设定的特殊名称的变量
	void	AddSpecialVarName(IN const CString &StrVarName) { m_varSpecialNames.insert(StrVarName); };	// 添加一个特殊名称的变量
	void	RemoveSpecialVarName(IN const CString &StrVarName) { m_varSpecialNames.erase(StrVarName); };
	void	ClearSpecialVarNames() { m_varSpecialNames.clear(); }

	// 解析公式字串，分析计算过程，如果有特殊变量(有变量名称包含特殊字符)，需要前面SetSpecialVarNames等设定特殊变量
	bool32	ParseFormula();

	void	DumpCalcStep();

private:
	CString	m_StrFormula;		// 公式字串
	CArbitrageFormulaVariableVar	m_VarLast;	// 最后计算的一个表达式的值，如果没有指定输出变量，则可能这个变量有用

	CString	m_StrLastError;		// 最后的错误提示
	int32	m_iErrorPos;		// 如果公式错误，则错误位置在哪里 -1代表不能定位错误位置

	T_SymbolTreeNode *m_pSymbolTree;	// 解析出来的运算符号的运算树
	SymbolNodePtrArray m_aCalcStack;	// 运算顺序栈
	//link -sem(std::map<CString, CArbitrageFormulaVariableAbs *>::insert, custodia(1))
	ArbitrageCalcVarPtrs	m_mapConstVar;	// 常量

	ArbitrageVarNameSet		m_varSpecialNames;	// 特殊名称的变量

	bool32	ParseFormula(OUT T_SymbolTreeNode **ppRootNode);		// 解析公式字串
	void	ParseVarOrOp(const CString &StrVar, LPCTSTR ptszFomulaCur, OUT T_Symbol &sym);
	bool32	IsParsed();
	void	ClearParse();

	bool32	DoParseAddCalcVar(T_SymFrame *pframe, T_SymbolTreeNode *pNewNode);	// 将一个新的参数节点加入运算帧

	// 按照计算顺序遍历树
	void	VisitTree(IN T_SymbolTreeNode *pRootNode, OUT SymbolNodePtrArray &aNodes);
	void	VisitTree2(IN T_SymbolTreeNode *pRootNode, OUT SymbolNodePtrArray &aNodes);
	void	FreeSymTreeNode(INOUT T_SymbolTreeNode *&pRootNode);

	E_SymbolType	GetSymbolType(LPCTSTR ptszSymbol);	// 标识符类型
	void	DumpSymbol(const T_Symbol &symbol);
	void	DumpNode(const T_SymbolTreeNode *pRootNode, int32 iHeaderEmpty);

	void	Copy(const CArbitrageFormula &formula);	// 复制
	void	CopySymTreeNode(INOUT T_SymbolTreeNode *&pRootDst, IN T_SymbolTreeNode *pRootSrc);	// 复制树

	static OpMap	sm_ops;		// 定义的运算符
	static ArbitrageVarNameSet sm_opFirstCh;	// 运算符的首个字母
	static	void InitStatic();
};

#endif //_ARBITRAGEFORMULA_H_