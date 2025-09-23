// CFormularCompute.h: interface for the CFormularCompute class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CFORMULARCOMPUTEPARENT_H__D8DB589D_6CB1_4A42_B247_8CF240546DAF__INCLUDED_)
#define AFX_CFORMULARCOMPUTEPARENT_H__D8DB589D_6CB1_4A42_B247_8CF240546DAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "define.h"
#include "TechExport.h"
#include "CFormularComputeParent.h"
class CFormularContent;
class EXPORT_CLASS CFormularCompute  : public CFormularComputeParent
{
public:
	CFormularCompute();
	virtual ~CFormularCompute();
	CFormularCompute(int pmin,int pmax,Kline* pp,CString& str); 
	CFormularCompute(int pmin,int pmax,Kline* pp,CFormularContent* pIndex);
protected:
	static void InitStatic2();
};

#endif // !defined(AFX_CFORMULARCOMPUTEPARENT_H__D8DB589D_6CB1_4A42_B247_8CF240546DAF__INCLUDED_)
