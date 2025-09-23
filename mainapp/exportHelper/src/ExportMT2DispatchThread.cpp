#include "StdAfx.h"
#include "ExportMT2DispatchThread.h"
#include "ShareFun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
CExportMT2DispatchThread::CExportMT2DispatchThread()
{
	m_bStop = false;
	m_hSemPostedItem = NULL;
}

CExportMT2DispatchThread::~CExportMT2DispatchThread()
{
	UnInit();
}

CExportMT2DispatchThread & CExportMT2DispatchThread::Instance()
{
	static CExportMT2DispatchThread sObj;
	return sObj;
}

bool CExportMT2DispatchThread::InitInstance()
{
	if ( m_aThread.empty() )
	{
		m_bStop = false;

		ASSERT( m_hSemPostedItem==NULL );

		m_hSemPostedItem = CreateSemaphore(NULL, 0, 10000, NULL);
		ASSERT( NULL!=m_hSemPostedItem );

		const int iThreadCount = 2;
		for ( int i=0; i<iThreadCount; ++i )
		{
			CWinThread *pThread = AfxBeginThread(DispatchThread, this);
			ASSERT( NULL!=pThread );
			if ( NULL!=pThread )
			{
				pThread->m_bAutoDelete = FALSE;
				m_aThread.push_back(pThread);
			}
		}
	}
	
	return true;
}

void CExportMT2DispatchThread::UnInit()
{
	// Í£Ö¹Ïß³Ì
	m_bStop = true;

	if ( !m_aThread.empty() )
	{
		typedef vector<HANDLE> HANDLEVector;
		HANDLEVector aThread;
		int i;
		for ( i=0; i<m_aThread.size(); ++i )
		{
			CWinThread *pThread = m_aThread[i];
			if ( NULL!=pThread && NULL!=pThread->m_hThread )
			{
				aThread.push_back(pThread->m_hThread);
			}
		}
		if ( aThread.size()>0 )
		{
			LONG lCount = 0;
			ReleaseSemaphore(m_hSemPostedItem, aThread.size(), &lCount);

			WaitForMultipleObjects(aThread.size(), &aThread[0], TRUE, INFINITE);
		}
		for (i=0; i<m_aThread.size(); ++i )
		{
			CWinThread *pThread = m_aThread[i];
			delete pThread;
		}
		m_aThread.clear();
	}

	if ( NULL!=m_hSemPostedItem )
	{
		DEL_HANDLE(m_hSemPostedItem);
		m_hSemPostedItem = NULL;
	}

	PostedItemList lstLeft;
	m_LockPostedItem.lock();
	lstLeft.swap(m_lstPostedItem);
	m_LockPostedItem.unlock();

	for ( PostedItemList::iterator it=lstLeft.begin(); it!=lstLeft.end(); ++it )
	{
		const T_PostedItem &stItem = *it;
		DoDispatchItem(stItem, false);
	}
	lstLeft.clear();
}

void CExportMT2DispatchThread::AddProcessor( iExportMT2DispatchThreadProcessor *pProcessor )
{
	if ( NULL==pProcessor )
	{
		ASSERT( 0 );
		return;
	}

	m_LockProcessor.lock();

	bool bAdd = false;
	for ( int i=0; i<m_aProcessor.size(); ++i )
	{
		if ( pProcessor==m_aProcessor[i] )
		{
			bAdd = true;
			break;
		}
	}
	if ( !bAdd )
	{
		m_aProcessor.push_back(pProcessor);
	}
	m_LockProcessor.unlock();
}

void CExportMT2DispatchThread::RemoveProcessor( iExportMT2DispatchThreadProcessor *pProcessor )
{
	m_LockProcessor.lock();
	
	for ( int i=0; i<m_aProcessor.size(); ++i )
	{
		if ( pProcessor==m_aProcessor[i] )
		{
			m_aProcessor.erase( m_aProcessor.begin()+i );
			break;
		}
	}
	
	m_LockProcessor.unlock();
}

void CExportMT2DispatchThread::RemoveAllProcessor()
{
	m_LockProcessor.lock();
	
	m_aProcessor.clear();
	
	m_LockProcessor.unlock();
}

bool CExportMT2DispatchThread::Post2DispatchThread( iExportMT2DispatchThreadProcessor *pProcessor, LPARAM l )
{
	if ( m_bStop )
	{
		return false;
	}

	T_PostedItem stItem;
	stItem.pProcessor	= pProcessor;
	stItem.lP			= l;
	if ( !PushPostedItem(stItem) )
	{
		return false;
	}

	return true;
}

UINT AFX_CDECL CExportMT2DispatchThread::DispatchThread( LPVOID lParam )
{
	CExportMT2DispatchThread *pThis = (CExportMT2DispatchThread *)lParam;
	ASSERT( pThis );

	return pThis->DoDispatchThread();
}

UINT CExportMT2DispatchThread::DoDispatchThread()
{
	while ( !m_bStop )
	{
		T_PostedItem stItem;
		if ( PopPostedItem(stItem) )
		{
			DoDispatchItem(stItem, true);
		}
	}

	return 0;
}

bool CExportMT2DispatchThread::PushPostedItem( const T_PostedItem &stItem )
{
	if ( NULL==m_hSemPostedItem )
	{
		ASSERT( 0 );
		return false;
	}

	bool bPush = false;

	m_LockPostedItem.lock();
	LONG lPre = 0;
	if ( ReleaseSemaphore(m_hSemPostedItem, 1, &lPre) )
	{
		m_lstPostedItem.push_back( stItem );
		bPush = true;
	}
	m_LockPostedItem.unlock();

	return bPush;
}

bool CExportMT2DispatchThread::PopPostedItem( T_PostedItem &stItem )
{
	if ( NULL==m_hSemPostedItem )
	{
		ASSERT( 0 );
		return false;
	}

	DWORD dwWait = WaitForSingleObject(m_hSemPostedItem, 100);
	if ( WAIT_OBJECT_0!=dwWait )
	{
		return false;
	}

	bool bPop = false;
	m_LockPostedItem.lock();
	if ( !m_lstPostedItem.empty() )
	{
		stItem = m_lstPostedItem.front();
		m_lstPostedItem.pop_front();
		bPop = true;
	}
	m_LockPostedItem.unlock();

	return bPop;
}

bool CExportMT2DispatchThread::DoDispatchItem( const T_PostedItem &stItem, bool bSucDisp )
{
	if ( NULL==stItem.pProcessor )
	{
		ASSERT( 0 );
		return false;
	}

	iExportMT2DispatchThreadProcessor *pProcessor = NULL;
	
	m_LockProcessor.lock();
	for ( int i=0; i<m_aProcessor.size(); ++i )
	{
		if ( stItem.pProcessor==m_aProcessor[i] )
		{
			pProcessor = stItem.pProcessor;
			break;
		}
	}
	m_LockProcessor.unlock();

	if ( NULL==pProcessor )
	{
		ASSERT( 0 );
		return false;
	}

	pProcessor->DispatchThreadProcess(bSucDisp, stItem.lP);

	return true;
}
