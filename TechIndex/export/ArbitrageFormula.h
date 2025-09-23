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

#define ARB_INVALID_VALUE		(FLT_MAX)		// ������ʾ�������Чֵ

// ��������õ�ֵȫ������float��ʾ�ģ�soֵ����Լ��Ϊfloat
typedef float			ArbitrageValueType;
typedef vector<ArbitrageValueType>	ArbitrageValueArray;
class CArbitrageFormulaVariableAbs
{
public:
	CArbitrageFormulaVariableAbs() { m_iCurIndex = -1; }
	virtual ~CArbitrageFormulaVariableAbs(){};

	virtual bool32 GetValue(int32 iIndex, OUT ArbitrageValueType &val) = 0;
	virtual bool32 SetValue(int32 iIndex, const ArbitrageValueType &val) = 0;
	virtual bool32 IsEnableSet() = 0;	// �Ƿ��д
	virtual int32  GetMinIndex() = 0;	// ��С����ֵ
	virtual int32  GetMaxIndex() = 0;	// �������ֵ

	virtual bool32	SetMinIndex(int32 iMinIndex) = 0;
	virtual bool32	SetMaxIndex(int32 iMaxIndex) = 0;

	void	SetVarName(LPCTSTR ptszVarName) { m_StrVarName = ptszVarName; }
	const CString &GetVarName() const { return m_StrVarName; }

	void	SetCurIndex(int32 iCur) { m_iCurIndex = iCur; }
	int32	GetCurIndex() const { return m_iCurIndex; }
	bool32	GetCurValue(OUT ArbitrageValueType &val) { return GetValue(m_iCurIndex, val); }
	bool32	SetCurValue(const ArbitrageValueType &val) { return SetValue(m_iCurIndex, val); }

protected:
	CString		m_StrVarName;	// ��������
	int32		m_iCurIndex;		// ��ǰ����
};

// �ɱ����
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
			// ��չ
			m_aValues.reserve(m_iMax+1);
			m_aValues.insert(m_aValues.end(), m_iMax+1-m_aValues.size(), ARB_INVALID_VALUE);
			m_aValues[iIndex] = val;
			return true;
		}
		return false;
	}

	virtual bool32 IsEnableSet() { return true; }	// �������ǿ�д��

	virtual int32	GetMinIndex() { return m_iMin; }
	virtual int32	GetMaxIndex() { return m_iMax; }
	virtual bool32	SetMinIndex(int32 iMinIndex) { m_iMin = iMinIndex; return false; }
	virtual bool32	SetMaxIndex(int32 iMaxIndex) { m_iMax = iMaxIndex; return false; }

	ArbitrageValueArray m_aValues;
	int32			m_iMin;
	int32			m_iMax;
};

// ����
class CArbitrageFormulaVariableConst : public CArbitrageFormulaVariableAbs
{
public:
	CArbitrageFormulaVariableConst(){};
	CArbitrageFormulaVariableConst(LPCTSTR ptszVarName){ SetVarName(ptszVarName); };

	virtual bool32 GetValue(int32 iIndex, OUT ArbitrageValueType &val) { val = m_val; return true; }
	virtual bool32 SetValue(int32 iIndex, const ArbitrageValueType &val) { return false; }
	virtual bool32 IsEnableSet() { return false; }	// ������ֹд

	virtual int32	GetMinIndex() { return 0; }
	virtual int32	GetMaxIndex() { return LONG_MAX; }

	virtual bool32	SetMinIndex(int32 iMinIndex) { return false; }
	virtual bool32	SetMaxIndex(int32 iMaxIndex) { return false; }

	ArbitrageValueType	m_val;
};

typedef map<CString, CArbitrageFormulaVariableVar *>	ArbitrageVarPtrs; // ��֤����ʱ��ָ����Ч��
typedef map<CString, CArbitrageFormulaVariableAbs *>	ArbitrageCalcVarPtrs; // ��֤����ʱ��ָ����Ч��
typedef	set<CString>									ArbitrageVarNameSet;	// �������Ƽ�

struct T_ArbForOperatorParam
{
	CArbitrageFormulaVariableAbs *pVar1;
	CArbitrageFormulaVariableAbs *pVar2;
	CArbitrageFormulaVariableAbs *pVarOther;
	int32						  iVarOtherCount;
	int32						  iMinIndex;		// ���㿪ʼ������ - ���minIndex > maxIndex��ʹ�õ�ǰ��������Ϊ���㵱ǰֵ�ı�־
	int32						  iMaxIndex;		// �������������
};

typedef bool32 (*ARB_FOR_OPERATOR_FUNC)(OUT CArbitrageFormulaVariableAbs &valResult, const T_ArbForOperatorParam &param);
#define ARB_FOR_PRIORITY_MIN  (0)
#define ARB_FOR_PRIORITY_MAX  (9)
struct T_ArbForOperator
{
	TCHAR				  szOp[10];	// ������ִ�
	int32				  iParamCount; // ������������ܽ��յĲ�������
	int32				  iOpPriority;		// �������ȼ�����ֵԽ�����ȼ�Խ��, ����Ԥ��0-9����+-Ϊ5�� */Ϊ6�� =Ϊ2��, ,Ϊ1��
	bool32				  bLeftToRight;		// ������������ң����false����ҵ���
	ARB_FOR_OPERATOR_FUNC func;
};


class EXPORT_CLASS CArbitrageFormula
{
public:
	enum E_SymbolType
	{
		EST_Const = 0,	// ����
		EST_Var,		// ����
		EST_Operator,	// �����

		EST_TmpValue,		// ��ʱ��������Ϊ�洢�м�ֵ����
		EST_Count		// ��Ч
	};

	struct T_Symbol
	{
		E_SymbolType eSymType;
		LPCTSTR		 pszFormulaPos;	// ��ָ���formula��λ��
		int32		 iMyPriority;	// ����������������������������������е����ȼ�
		CString		 StrVar;	// ����ָ��һ��������������
		union U_CONTENT{
			const T_ArbForOperator *pOp;	// ���Ϊ��������ͣ���ָ�������
			DWORD			dwFlag;
			int				iTmpIndex;		// ��ʱ���Ͷ�Ӧ����һ����ʱ������
			CArbitrageFormulaVariableAbs *pVar;	// ָ�������ָ�룬����ʱ�ã�
		} u_Content;
	};

	// Ҷ�ӽڵ��Ϊ���������ӽڵ��Ϊ����������ڵ�����Ǳ�������и��ڵ�һ���ڵ�
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
		T_SymbolTreeNode *pRoot;		// ���ڵ�
		T_SymbolTreeNode *pLastVar;		// ���һ����������(�����Ѿ���������������)��һ������ֵ���Ͳ�������Ϊ��
		T_SymbolTreeNode *pLastWaitOp;	// ���һ���ȴ��Ҳε������
		SymbolNodePtrArray	aCalcSort;	// ����˳��ֻ�����������Ψһ��һ������
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

	// ��ʽ���㣬������󣬿���ͨ��lasterrortext��ȡ������Ϣ
	// ͨ��LastVar��ȡ���һ�����ʽ�ļ�����(����������������)
	// [iMinIndex-iMaxIndex]: [��С�������������]����[0-10]��ΧΪ0-10(��) 11������
	bool32	Calculate(INOUT ArbitrageVarPtrs &vars, int32 iMinIndex, int32 iMaxIndex);	// һ������һ�������ļ���
	bool32	Calculate2(INOUT ArbitrageVarPtrs &vars, int32 iMinIndex, int32 iMaxIndex);	// ȫ������һ����㣬���ܺõ�

	bool32	TestFormula(IN const ArbitrageVarPtrs &vars);	// ���Թ�ʽ�������ṩ���еı���
	bool32	TestFormula();	// ������ʽ�Ƿ����

	void	SetFormula(LPCTSTR ptszFormula);
	const CString &GetFormula()const { return m_StrFormula; }

	const CArbitrageFormulaVariableVar &GetLastVar() const { return m_VarLast; }

	void	SetLastErrorText(LPCTSTR ptszError) { m_StrLastError = ptszError; }
	const CString &GetLastErrorText() const { return m_StrLastError; }
	void	SetErrorPos(int32 iPos) { m_iErrorPos = iPos; }
	int32	GetErrorPos() const { return m_iErrorPos; }

	// ���������б��������ִ�Сд
	bool32	GetVariables(OUT ArbitrageVarPtrs &vars);	// ��ȡ���еĿɱ�ı��������õ�ʱ�����Ҫ�ֶ�delete��������ߵ���FreeVariable
	void	FreeVariables(INOUT ArbitrageVarPtrs &vars);	// �ͷ����пɱ������ָ��
	bool32	GetUnInitializedVariables(OUT ArbitrageVarPtrs &vars);	// ��ȡû�з���ָ��Ŀɱ�����������ͷ�

	// ���������ĳЩ�������ַ��ı������ܲ�������������ͨ���������������ָʾ��Щ���ƶ��Ǳ���
	// ���ú���Ҫ����ParseFormula��Ч
	void	SetSpecialVarNames(IN const ArbitrageVarNameSet &varNames) { m_varSpecialNames = varNames; };	// ��������������ƣ���ǰ�����ƻ����
	const ArbitrageVarNameSet &GetSpecialVarNames() const { return m_varSpecialNames; }	// ��ȡ�趨���������Ƶı���
	void	AddSpecialVarName(IN const CString &StrVarName) { m_varSpecialNames.insert(StrVarName); };	// ���һ���������Ƶı���
	void	RemoveSpecialVarName(IN const CString &StrVarName) { m_varSpecialNames.erase(StrVarName); };
	void	ClearSpecialVarNames() { m_varSpecialNames.clear(); }

	// ������ʽ�ִ�������������̣�������������(�б������ư��������ַ�)����Ҫǰ��SetSpecialVarNames���趨�������
	bool32	ParseFormula();

	void	DumpCalcStep();

private:
	CString	m_StrFormula;		// ��ʽ�ִ�
	CArbitrageFormulaVariableVar	m_VarLast;	// �������һ�����ʽ��ֵ�����û��ָ���������������������������

	CString	m_StrLastError;		// ���Ĵ�����ʾ
	int32	m_iErrorPos;		// �����ʽ���������λ�������� -1�����ܶ�λ����λ��

	T_SymbolTreeNode *m_pSymbolTree;	// ����������������ŵ�������
	SymbolNodePtrArray m_aCalcStack;	// ����˳��ջ
	//link -sem(std::map<CString, CArbitrageFormulaVariableAbs *>::insert, custodia(1))
	ArbitrageCalcVarPtrs	m_mapConstVar;	// ����

	ArbitrageVarNameSet		m_varSpecialNames;	// �������Ƶı���

	bool32	ParseFormula(OUT T_SymbolTreeNode **ppRootNode);		// ������ʽ�ִ�
	void	ParseVarOrOp(const CString &StrVar, LPCTSTR ptszFomulaCur, OUT T_Symbol &sym);
	bool32	IsParsed();
	void	ClearParse();

	bool32	DoParseAddCalcVar(T_SymFrame *pframe, T_SymbolTreeNode *pNewNode);	// ��һ���µĲ����ڵ��������֡

	// ���ռ���˳�������
	void	VisitTree(IN T_SymbolTreeNode *pRootNode, OUT SymbolNodePtrArray &aNodes);
	void	VisitTree2(IN T_SymbolTreeNode *pRootNode, OUT SymbolNodePtrArray &aNodes);
	void	FreeSymTreeNode(INOUT T_SymbolTreeNode *&pRootNode);

	E_SymbolType	GetSymbolType(LPCTSTR ptszSymbol);	// ��ʶ������
	void	DumpSymbol(const T_Symbol &symbol);
	void	DumpNode(const T_SymbolTreeNode *pRootNode, int32 iHeaderEmpty);

	void	Copy(const CArbitrageFormula &formula);	// ����
	void	CopySymTreeNode(INOUT T_SymbolTreeNode *&pRootDst, IN T_SymbolTreeNode *pRootSrc);	// ������

	static OpMap	sm_ops;		// ����������
	static ArbitrageVarNameSet sm_opFirstCh;	// ��������׸���ĸ
	static	void InitStatic();
};

#endif //_ARBITRAGEFORMULA_H_