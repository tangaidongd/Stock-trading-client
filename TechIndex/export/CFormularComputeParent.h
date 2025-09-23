// com.h: interface for the CFormularComputeParent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COM_H__80BFF943_AB2A_11D1_8C43_0000E823CF90__INCLUDED_)
#define AFX_COM_H__80BFF943_AB2A_11D1_8C43_0000E823CF90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include <math.h>
#include "typedef.h"
#include "MerchManager.h"
#include "StructKlineView.h"
//#include "SelectStockStruct.h"
#include "TimeInterval.h"
#include "TechExport.h"
#include "SpecialTechIndexManager.h"

typedef struct MerchExtraInfo
{
	int32 iShowCountInFullList;
	int32 iShowPosInFullList;
	E_NodeTimeInterval eTimeIntervalFull;
	int32	iTimeUserMultipleMinutes;		// 自定义分钟线周期
	int32	iTimeUserMultipleDays;			// 自定义日线周期
	CMerch *pMerchNode;
	CMerch *pMainExpMerchNode;				// 大盘
	MerchExtraInfo()
	{
		iShowCountInFullList = 0;
		iShowPosInFullList = 0;
		iTimeUserMultipleMinutes = 0;
		iTimeUserMultipleDays = 0;
		pMerchNode = NULL;
		pMainExpMerchNode = NULL;
	}
}MERCH_EXTRA_INFO;



#ifndef _TAG_INPUT_INFO
#define _TAG_INPUT_INFO
typedef struct tagINPUT_INFO
{
	char*			strSymbol;			// 股票代码
	bool			bIndex;				// 大盘

	int				nNumData;			// 数据数量(pData,pDataEx,pResultBuf数据数量)
	Kline*			pData;				// 常规数据,注意:当m_nNumData==0时可能为 NULL

	ARRAY_BE 		fInputParam[10];	// 调用参数
	ARRAY_BE 		fOutputParam;		// 输出参数
	klineKind		klineType;			// 数据类型
} INPUT_INFO;
#endif

template <class TY> class Intem
{
public:
	Intem(TY v=0){val=v;next=0;prev=0;}
	Intem<TY>* next;
	Intem<TY>* prev;
	TY val;
};

template <class TH> class Intst 
{
public:
	Intst(TH val) { list = new Intem(val);tail=0;};
	Intst() {list = 0;tail=0;};
//===========压入头部=============
	TH push_b(TH val)
	{
		Intem<TH>* pt;
		pt=new Intem<TH>(val);
		pt->next = list;
		if(list!=0) 
			list->prev = pt;
		else
			tail = pt;
		list = pt;
		return val;
	}
//==========压入尾部================
/*	TH push_t(TH val)
	{
		Intem<TH> *pi,*pt,*prv;
		for(prv=pi=list;pi;prv=pi,pi=pi->next)  ;
    
		pt=new Intem<TH>(val);	
		if(list==0)
			list=pt;
		else
		{
			pt->prev=prv;
			prv->next=pt;
		}
		return val;
	}
*/
//========弹出===============
	TH pop_it()
	{
	Intem<TH>* tmp;
	TH val;
	if(list==0)
		return 0;
	tmp=list;
	list=list->next;
	if(list!=0)
		list->prev=0;
	else
		tail=0;
	val = tmp->val;
	delete tmp;
	return val;
	}
//========压出===============
	TH rec_it()
	{
		Intem<TH>* tmp;
		TH val;
		if(tail==0)
			return 0;
		tmp=tail;
		tail=tail->prev;
		if(tail!=0) 
			tail->next=0;
		else
			list=0;
		val = tmp->val;
		delete tmp;
		return val;
	}
private:
	Intem<TH>* list;
	Intem<TH>* tail;
};

class CFormularContent;
struct T_MerchNodeUserData;
struct INPUT_PARAM
{
	int pmax;
	Kline* pp;
	CFormularContent* pIndex;
	//T_MerchNodeUserData *pUserData;	// 额外的与商品有关数据
	MERCH_EXTRA_INFO *pExtraInfo;
};

class EXPORT_CLASS CFormularComputeParent  
{
public:	
	bool m_bShowColor;//是否显示彩色
	enum ColorKind {
		digit = 0,
		operrator ,
		variable ,
		function,
		quotevar,
		};
	struct KindPos
	{
		int m_nKind;//元素种类
		int m_nPos;//元素结束位置
	};
	int m_nSizeKindPos;//m_pKindPos元素个数
	KindPos* m_pKindPos;//
	int m_nPosPre;//前一个元素的位置

	int GetIndexPoint(CString sName,float &nPoint);
	ARRAY_BE  m_Rline[RLINEE-RLINEB];  //返回变量
	CString m_RlineName[RLINEE-RLINEB];  //返回变量名称
	int     m_RlineNum;              //返回变量个数

	CString m_formuCompute;   //计算表达式
	CString m_errmsg;   //错误信息
	int     m_errpos;	//错误位置

	bool m_bTotalData;
	int m_isDaPan;	//是否是大盘指标
	int m_RlineType[NUM_LINE_TOT];	//返回的线的类型
	int m_RlineColor[NUM_LINE_TOT];	//返回的线的类型
	int m_RlineThick[NUM_LINE_TOT];	//返回的线的宽度
	int m_RlineTypeEx[NUM_LINE_TOT];//返回的线的类型(NODRAW,NOTITLE等) 可以与 m_RlineType 叠加显示.

	CString GetRlineName(int foot);//得到返回数据的名称
	//输入参数:foot 表示指定第几组数据
	//输出参数:返回指定组数据的名称
	void IsDaPan();	//判断是否是大盘指标
	int FindLineType();//计算返回线的类型
	int YhhCheck(int j, int k);
						//rtn = 0 is successful
	//输入参数:pEqution 是用于计算的指标解释计算对象,pEqution的初始值是NULL,函数结束时必须删除pEqution对象;input时输入的初始化数据
	//输出参数:返回0表示成功
	int GetRlineNum();//得到指标公式的数据数组的个数
	int GetNum(int tj);//得到某个元函数的参数个数
	//输入参数:tj标识特定的元函数
	//输出参数:返回元函数的参数个数
	CFormularComputeParent(int pmin,int pmax,Kline* pp,CString& str); 
	CFormularComputeParent(int pmin,int pmax,Kline* pp,CFormularContent* pIndex);
	CFormularComputeParent();
		//pmin 为K线数据的最小下标,pmax为K线数据的最大下标,pp为K线数据的指针,pIndex为技术指标数据指针

		//输入说明：pmin 为K线数据的最小下标,pmax为K线数据的最大下标,pp为K线数据的指针,pIndex为技术指标数据指针

	virtual ~CFormularComputeParent();
	int AddPara(CString pa,int va);   //增加整型参数
	int AddPara(CString pa,float va); //增加浮点参数
	int AddLine(CString& LineName,ARRAY_BE& pline);
									  //增加线参数

	void SetScreenBeginEnd(int iBegin, int iEnd);
	void GetMsg(CString& str,int& pos);//取得错误信息
//	      输入说明：str是错误信息字符串,pos 是错误位置
//        输出说明：无。
	int GetLine(int& index,ARRAY_BE& pline,CString& name);
									  //取得计算结果
	//输入参数:index 表示指定第几组数据
	//输出参数:pline是输出的数据,name是输出的数据名称,返回指定组数据的名称
	int GetIndexPoint(int &index,float &nPoint);

	int GetLine(CString& LineName,ARRAY_BE& pline);
									  //取得计算结果
	int  FormularCompute();    //计算

	int Relolve(int cl);//对公式引用进行处理
	//输入参数:cl 表示特定的公式引用
	//输出参数:返回错误码,0为正确,1为错误
	void Devide();     //分解公式成可以顺序计算的关键码
	void GetData(int cl,ARRAY_BE& pp);
	int Sort(int bi,int ej);     //运算顺序确定
	
	int CheckWord(CString emp);	 //词法检查	

	//输入参数:emp 是输入的公式字符串
	//输出参数:返回错误发生的位置
	// fangz0926 char->TCHAR
	int GetCharClass(TCHAR ch);   //确定字符类别
	//int GetCharClass(TCHAR ch);   //确定字符类别
	int CheckSent();			 //语法检查	

	void SetVolType(bool bTotalVol); 

	void SetMerchNodeUserData(MERCH_EXTRA_INFO *pExtraInfo);	// 当前计算商品有关的数据
	MERCH_EXTRA_INFO *GetMerchNodeUserData() const {return m_pExtraInfo; }
protected:
	int m_nCurrentFoot;//for right box funcs
	int GetBeginFoot(ARRAY_BE& ln2);
	int RelolveNnet(int cl);
	int m_tempWhich;//当前计算到哪个线

	int m_MaxPoint;    // 计算起始点
	int m_MinPoint;    // 计算终止点

	CString  m_QuoteArray[QUE-QUB];	//引用的名称的数组
	int      m_QuoteNum;	//引用的个数//

	CString  m_stringArray[CECE-CECB];	//字符串的数组
	int      m_stringNum;	//字符串的个数

	CString m_strColor[CONSCOLORE-CONSCOLORB+1];
	int      m_strColorNum;	//color的个数

	int	 m_Sent[MAXB];  //子公式计算顺序//
	int  m_SentNum;     //子公式中实体个数(-1)//

	int	 m_ComSort[MAXB_ALL];    // 公式计算顺序//-------lmb?
	int  m_ComSortNum;		 // 公式中实体个数(-1)//
	int  m_Post[MAXB];  // 记录变量在子串中位置//

	ARRAY_BE  m_VarArray[MIDE-MIDB]; // 中间变量
	int     m_VarNum;			  // 中间变量个数
	CString m_VarName[MIDE-MIDB];             // 中间变量名称

	int   m_floatNum;			  // 浮点系数个数
	float m_floatArray[CEFE-CEFB];//浮点系数

	CString  m_NumName[PARAM_NUM];    //参数名称			// huhe* 加到16个参数设置
	int      m_NumGs;		  //参数个数	

	int   m_intNum;		// 整数系数个数
	int32   m_intArray[CEIE-CEIB]; // 整数系数

	ARRAY_BE m_MidArray[PCME-PCMB]; // 中间数据
	int    m_MidNum;    // 中间数据个数

	Kline* m_pData;       // K线数据指针
	Intst<int> m_table;	//堆栈

	bool m_bToatlVol;	// ...fangz0701 是否是累计的成交量

	CMapStringToPtr m_WordTableNew;	//新加的函数表
	CMap<int,int,int,int> m_ParameterTableNew;//函数参数个数表
	static CMapStringToPtr m_LetterTable;	//字母表
	static CMap<int,int,int,int> m_ParameterTable;//函数参数个数表
	static CMapStringToPtr m_WordTable;	//函数表

	bool IsDayKline(int nKlineType);

	void FuncZigPre(INPUT_INFO *pInput,int nFunc);

	static int FuncZigGroup(INPUT_INFO *pInput, void* pArray);

	int RegularTime(int time,bool bDay);

	int LookTwoPath(time_t& tmt ,Kline* pKline,int nMax,bool& bAdd,bool bDay);

	int GetFoot(INPUT_INFO *pInput, int nParam);

	//输入参数:pInput 是输入的原始数据,nFunc是表示第几个函数
	//输出参数:返回错误码,0表示成功
	int FuncInDll(INPUT_INFO* pInput,int  nFunc);//计算2.0版新增加的元函数
	
	int FuncFromDll(int n);

	static void InitStatic();
	bool LookupParameterTable(int nKey,int& nValue);
	bool LookupWordTable(CString sKey,int& n);
	void KlineToLine(Kline* pKline,float& f,int nFlag);
	int Func847();
	Kline* m_pKlineIndex;
	
	
    int Func101();
	int Func102();
	int Func301();
	int Func302();
	int Func401();
	int Func402();
	int Func405();
	int Func406();
	int Func408();
	int Func409();
	int Func501();
	int Func502();
	int Func601();
	int Func602();
	int Func603();
	int Func604();
	int Func801();
	int Func802();
	int Func803();
	int Func804();
	int Func805();
	int Func806();
	int Func807();
	int Func808();
	int Func809();
	int Func810();
	int Func811();
	int Func812();
	int Func813();
	int Func830();
	int Func831();
	int Func832();
	int Func833();
	int Func834();
	int Func835();
	int Func836();
	int Func837();
	int Func838();
	int Func839();
	int Func840();
	int Func841();
	int Func842();
	int Func843();
	int Func844();
	int Func845();
	int Func846();
	int Func852();
	int Func853();
	int	Func856();
	int FuncHHAV();
	int FuncLLAV();
	
	int m_exec;
	int m_nKindKline;				//m_nKindKline 
	bool m_bCreateBsList;
	CFormularContent*	m_pIndex;

	//T_MerchNodeUserData *m_pMerchUserData;
	MERCH_EXTRA_INFO *m_pExtraInfo;
	int	 m_iMinuteFromOpenTime;	// 当前时间离开盘后的分钟数
	int  m_iKlinePeriod;		// K线周期数

	int  m_iSreenBegin;
	int  m_iSreenEnd;

	bool GetBuySellList();
	void InitEquation(int pmin,int pmax,Kline* pp,CString& str);//初始化输入数据

private:
	//float GetCapital(CReportData *pdt);
	CSpecialTechIndexManager m_SpecialTechIndexManager;
};

#endif // !defined(AFX_COM_H__80BFF943_AB2A_11D1_8C43_0000E823CF90__INCLUDED_)
