// CFormularCompute.cpp: implementation of the CFormularCompute class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "afxtempl.h"
#include "StructKlineView.h"
#include "CFormularContent.h"
#include "CFormularCompute.h"
//#include "IoViewShare.h"



CString m_gPeriodName[]={_T("换手"),_T("1分钟线"),_T("5分钟线"),_T("15分钟线"),_T("30分钟线"),
								_T("60分钟线"),_T("日线"),_T("周线"),_T("月线"),_T("年线"),
								_T("多日线")};
int nLKind[]={
HS_KLINE,
MINUTE1_KLINE,
MIN5_KLINE,
MIN15_KLINE,
MIN30_KLINE,
MIN60_KLINE,
DAY_KLINE,
WEEK_KLINE,
MONTH_KLINE,
YEAR_KLINE ,
MANY_DAY_KLINE,
0
};
CString CTaiScreenParent_GetPeriodName(int nKlineType)
{
	for(int i = 0;i<11;i++)
	{
		if(nLKind[i]==nKlineType)
		{
			return m_gPeriodName[i];
		}
	}
	CString s=_T("");
	return s;
	
}
int CTaiScreenParent_FromKlineKindToFoot(int nKlineKind)
{
	int n = -1;
	for(int i=0;i<11;i++)
	{
		if(nLKind[i] == nKlineKind)
		{
			n=i;
			break;
		}

	}
	return n;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFormularCompute::CFormularCompute()
{

}

CFormularCompute::~CFormularCompute()
{

}
CFormularCompute::CFormularCompute(int pmin,int pmax,Kline* pp,CString& str)
:CFormularComputeParent(pmin, pmax, pp, str)
{
	InitStatic2();
}

CFormularCompute::CFormularCompute(int pmin,int pmax,Kline* pp,CFormularContent* pIndex)
:CFormularComputeParent(pmin, pmax, pp, pIndex)
{
	InitStatic2();
}
void CFormularCompute::InitStatic2()
{
	// 扩展CFormularComputeParent::InitStatic()
	// 前面的保证不动, 新增的函数放在这里.

	static bool bExec = false;
	if(bExec==true) return;
	
	int nParam[]={1, 4, 5};
	CString sName[]={_T("sin2"),_T("drawkline"), _T("drawkline2")};

	int nb = FuncDllB+BEGIN_NUMBER1;
	for(int j=0;j<sizeof(nParam)/sizeof(int);j++)
	{
		m_WordTable[sName[j]] =(void *)(nb +j );//
		m_ParameterTable[nb + j] =nParam[j];
	}
}


