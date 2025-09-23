// IndexExtraData.cpp: implementation of the CIndexExtraData class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ShareFun.h"
#include "IndexExtraData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIndexExtraData::CIndexExtraData()
{
	m_pReportData = NULL;
}
//lint --e{1540}
CIndexExtraData::~CIndexExtraData()
{
	
}
CIndexExtraData * CIndexExtraData::NewIndexExtraData()
{
	CIndexExtraData * p = new CIndexExtraData;
	return p;
}
void CIndexExtraData::FreeIndexExtraData(CIndexExtraData * pIndexExtraData)
{
	//lint --e{438}
	DEL(pIndexExtraData);
}