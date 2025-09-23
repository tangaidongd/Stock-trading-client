#include "StdAfx.h"
#include "ExportCenter.h"

#include "ExportIndexCalc.h"
//#include "ExportQuoteRequest.h"

#include "ExportMainThreadWnd.h"
#include "ExportMT2DispatchThread.h"

//#include "ExportTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
CExportCenter::CExportCenter()
{
	m_bInited = false;

//	m_pTest = NULL;
}

CExportCenter::~CExportCenter()
{
	StopTest();
	UnInit();
}

CExportCenter & CExportCenter::Instance()
{
	static CExportCenter sObj;
	return sObj;
}

bool CExportCenter::Init()
{
	UnInit();

	bool b1 = CExportMainThreadWnd::Instance().InitInstance();
	bool b2 = CExportMT2DispatchThread::Instance().InitInstance();

	//bool b3 = CExportQuoteRequest::Instance().InitInstance();
	bool b4 = CExportIndexCalc::Instance().InitInstance();

	m_bInited = b1 && b2/* && b3*/ && b4;

	if ( !m_bInited )
	{
		if ( b1 )
		{
			CExportMainThreadWnd::Instance().UnInit();
		}
		if ( b2 )
		{
			CExportMT2DispatchThread::Instance().UnInit();
		}
// 		if ( b3 )
// 		{
// 			CExportQuoteRequest::Instance().UnInit();
// 		}
		if ( b4 )
		{
			CExportIndexCalc::Instance().UnInit();
		}
	}

	return m_bInited;
}

void CExportCenter::UnInit()
{
	if ( m_bInited )
	{
		// 先终止线程
		CExportMT2DispatchThread::Instance().UnInit();
		CExportMainThreadWnd::Instance().UnInit();

		// 在终止数据
		CExportIndexCalc::Instance().UnInit();
	//	CExportQuoteRequest::Instance().UnInit();

		m_bInited = false;
	}
}

void CExportCenter::StartTest()
{
// 	if ( NULL!=m_pTest )
// 	{
// 		return;
// 	}
// 	ASSERT( m_bInited );

//	m_pTest = new CExportTest;
	
//	CExportQuoteRequest::Instance().AddNotify(m_pTest);
//	CExportIndexCalc::Instance().AddIndexNotify(m_pTest);

	// 请求各种数据测试
//	m_pTest->Test1();
}

void CExportCenter::StopTest()
{
// 	if ( NULL==m_pTest )
// 	{
// 		return;
// 	}
// 
// 	ASSERT( m_bInited );
// 
// 	CExportIndexCalc::Instance().RemoveIndexNotify(m_pTest);
// 	CExportQuoteRequest::Instance().RemoveNotify(m_pTest);
// 
// 	delete m_pTest;
// 	m_pTest = NULL;
}

void CExportCenter::DoTest2()
{
// 	if ( NULL!=m_pTest )
// 	{
// 		m_pTest->Test2();
// 	}
}
