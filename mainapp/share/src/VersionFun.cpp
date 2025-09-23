// VersionFun.cpp: implementation of the CVersionFun class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VersionFun.h"
#include <io.h>
#include <wchar.h>
#include "wininet.h"
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////
BOOL DelTempFiles( CStringArray& ArrFiles)
{

	if ( ArrFiles.GetSize() < 1 )
	{
		return FALSE;
	}
    BOOL bResult = FALSE;
    BOOL bDone = FALSE;
    LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry = NULL;  
 
    DWORD  dwTrySize, dwEntrySize = 4096; // start buffer size    
    HANDLE hCacheDir = NULL;    
    DWORD  dwError = ERROR_INSUFFICIENT_BUFFER;
    
    do
    {                               
        switch (dwError)
        {
            // need a bigger buffer
            case ERROR_INSUFFICIENT_BUFFER: 
                delete [] lpCacheEntry;            
                lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) new char[dwEntrySize];
                lpCacheEntry->dwStructSize = dwEntrySize;
                dwTrySize = dwEntrySize;
                BOOL bSuccess;
                if (hCacheDir == NULL)                
				{ 
                    bSuccess = ( hCacheDir = FindFirstUrlCacheEntry(NULL, lpCacheEntry,&dwTrySize)) != NULL;
				}
                else
				{
					bSuccess = FindNextUrlCacheEntry(hCacheDir, lpCacheEntry, &dwTrySize);
				}

                if (bSuccess)
				{
					dwError = ERROR_SUCCESS;    
				}
                else
                {
                    dwError = GetLastError();
                    dwEntrySize = dwTrySize; // use new size returned
                }
                break;

             // we are done
            case ERROR_NO_MORE_ITEMS:
                bDone = TRUE;
                bResult = TRUE;                
                break;

             // we have got an entry
            case ERROR_SUCCESS:                       
        
                // don''''''''''''''''t delete cookie entry
                if (!(lpCacheEntry->CacheEntryType & COOKIE_CACHE_ENTRY))                
				{
					for ( int loop_entry = 0; loop_entry < ArrFiles.GetSize(); loop_entry ++ )
					{
						CString strDelName = ArrFiles.GetAt(loop_entry);
						CString strEntryName = CString(lpCacheEntry->lpszLocalFileName);
						if ( 0 == strEntryName.CompareNoCase(strDelName) )
						{
							DeleteUrlCacheEntry(lpCacheEntry->lpszSourceUrlName);
						}
					}
				}
                // get ready for next entry
                dwTrySize = dwEntrySize;
                if (FindNextUrlCacheEntry(hCacheDir, lpCacheEntry, &dwTrySize))
				{
					dwError = ERROR_SUCCESS;
				}

                else
                {
                    dwError = GetLastError();
                    dwEntrySize = dwTrySize; // use new size returned
                }                    
                break;

            // unknown error
            default:
                bDone = TRUE;                
                break;
        }

        if (bDone)
        {   
            delete [] lpCacheEntry; 
            if (hCacheDir)
			{
				FindCloseUrlCache(hCacheDir);
			}
                                  
        }
    } while (!bDone);
    
	return bResult;

}

///////////////////////////////////////////////////////////////////////////////
class AfxBindStatusCallback: public IBindStatusCallback
{
private:
	class CInnerUnknown : public IUnknown
	{
	protected:
		AfxBindStatusCallback* m_pThis;
		friend class AfxBindStatusCallback;

	public:
		inline CInnerUnknown() { }
		inline void SetpThis(AfxBindStatusCallback* pThis) { ASSERT(pThis != NULL); m_pThis = pThis; }

		STDMETHOD_(ULONG, AddRef)()
		{
			return InterlockedIncrement((long*)&m_pThis->m_dwRef);
		}

		STDMETHOD_(ULONG, Release)()
		{
			unsigned long lResult = InterlockedDecrement((long*)&m_pThis->m_dwRef);
			return lResult;
		}

		STDMETHOD(QueryInterface)(REFIID iid, void** ppvObject)
		{
			if (!ppvObject)
				return E_POINTER;

			// check for the interfaces this object knows about
			if (iid == IID_IUnknown)
			{
				*ppvObject = (IUnknown*)this;
				AddRef();
				return S_OK;
			}
			if (iid == IID_IBindStatusCallback)
			{
				*ppvObject = (IBindStatusCallback*)m_pThis;
				m_pThis->AddRef();
				return S_OK;
			}

			// otherwise, incorrect IID, and thus error
			return E_NOINTERFACE;
		}
	};

protected:
	DWORD m_dwRef;
private:
	IUnknown* m_pUnkControlling;
	CInnerUnknown m_UnkInner;	
	CBindStatusCB* m_pCB;

public:
	AfxBindStatusCallback( CBindStatusCB* pCB )
		:m_dwRef(0)
	{
		AfxOleLockApp();
		m_UnkInner.SetpThis(this);
		m_pUnkControlling = (IUnknown*)&m_UnkInner;
		m_pCB = pCB;
	}

	virtual ~AfxBindStatusCallback()
	{
		AfxOleUnlockApp();
	}

	STDMETHOD_(ULONG, AddRef)()
		{ return m_pUnkControlling->AddRef(); }
	STDMETHOD_(ULONG, Release)()
		{ return m_pUnkControlling->Release(); }
	STDMETHOD(QueryInterface)(REFIID iid, void** ppvObject)
		{ return m_pUnkControlling->QueryInterface(iid, ppvObject); }

	STDMETHOD(GetBindInfo)(
		DWORD __RPC_FAR *pgrfBINDF, BINDINFO __RPC_FAR *pbindinfo)
	{
		return S_OK;
	}

	STDMETHOD(OnStartBinding)(
		DWORD dwReserved, IBinding __RPC_FAR *pBinding)
	{
		return S_OK;
	}

	STDMETHOD(GetPriority)(LONG __RPC_FAR *pnPriority)
	{
		return S_OK;
	}

	STDMETHOD(OnProgress)(
		ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode,
		LPCOLESTR szStatusText)
	{
		if ( NULL != m_pCB )
		{
			if ( !m_pCB->OnProgress (ulProgress,ulProgressMax) )
			{
				return E_ABORT;
			}
		}
		return S_OK;
	}

	STDMETHOD(OnDataAvailable)(
		DWORD grfBSCF, DWORD dwSize, FORMATETC __RPC_FAR *pformatetc,
		STGMEDIUM __RPC_FAR *pstgmed)
	{
		return S_OK;
	}

	STDMETHOD(OnLowResource)(DWORD dwReserved)
	{
		return S_OK;
	}

	STDMETHOD(OnStopBinding)(HRESULT hresult, LPCOLESTR szError)
	{
		return S_OK;
	}

	STDMETHOD(OnObjectAvailable)(REFIID riid, IUnknown __RPC_FAR *punk)
	{
		UNUSED_ALWAYS(riid);
		UNUSED_ALWAYS(punk);
		return E_UNEXPECTED;
	}
friend class CInnerUnknown;
};

///////////////////////////////////////////////////////////////////////////////
int HttpDownLoad ( TCHAR* StrHttp,TCHAR* StrLocal, CBindStatusCB* pCB )
{
	//检查本地是否创建了对应的目录.
	_tcheck_if_mkdir ( StrLocal );

	//如果文件存在
	if ( 0 == _taccess ( StrLocal, 0 ) )
	{
		_tchmod ( StrLocal,_S_IWRITE );
		if ( !DeleteFile(StrLocal))
		{
			return -1;//文件无法删除,所以当然也无法更新.
		}
	}
	//清除Temporary Internet Files中的同名缓存文件
/*	
	CString StrLocalFile = CString(StrLocal);
	CString StrFile = StrLocalFile.Left(StrLocalFile.Find(_T("."))+1);
	CStringArray ArrStr;
	ArrStr.Add(StrFile);
	DelTempFiles(ArrStr);
*/
	CString StrFile = CString(StrLocal);
	StrFile = StrFile.Right(StrFile.GetLength() - StrFile.ReverseFind('/') -1);
	StrFile = StrFile.Right(StrFile.GetLength() - StrFile.ReverseFind('\\') -1);
	CStringArray ArrStr;
	ArrStr.Add(StrFile);
	DelTempFiles(ArrStr);

	//从http地址下载文件
	AfxBindStatusCallback mon(pCB);
	HRESULT hr = URLDownloadToFile ( NULL,StrHttp,StrLocal,0,&mon );
	if ( FAILED ( hr ) )
	{
		return -2;
	}
	return 1;

}
///////////////////////////////////////////////////////////////////////////////
CString GetFileVersion ( CString StrPath)
{
	DWORD	dwInfoSize;   
	DWORD	dwHandle;
	int		v1 = 0;
	int		v2 = 0;
	int		v3 = 0;
	int		v4 = 0;
	dwInfoSize = ::GetFileVersionInfoSize(StrPath.LockBuffer(),&dwHandle);
	StrPath.UnlockBuffer();
	if(dwInfoSize)
	{
		TCHAR* verInfo = new TCHAR[dwInfoSize];
		BOOL rt = ::GetFileVersionInfo(StrPath.LockBuffer(),NULL,dwInfoSize,verInfo);  //strPath目标文件的绝对路径
		StrPath.UnlockBuffer();
		if(rt)
		{
			UINT size=sizeof(VS_FIXEDFILEINFO);
			VS_FIXEDFILEINFO* pFixedFileInfo=NULL;
			rt=::VerQueryValue(verInfo,_T("\\"),(LPVOID*)&pFixedFileInfo,&size);
			if(rt)
			{
				//取版本数据
				v1=HIWORD(pFixedFileInfo->dwProductVersionMS);
				v2=LOWORD(pFixedFileInfo->dwProductVersionMS);
				v3=HIWORD(pFixedFileInfo->dwProductVersionLS);
				v4=LOWORD(pFixedFileInfo->dwProductVersionLS);
			}
		}
		delete[]verInfo;
	}
	CString StrVersion;
	StrVersion.Format(_T("%d.%d.%d.%d"),v1,v2,v3,v4);
	return StrVersion;
}
time_t GetFileLMTime( CString StrPath )
{
	time_t time = 0;
	struct _stat st;
	if ( 0 == ::_tstat(StrPath,&st))
	{
		time = st.st_mtime;	
	}
	return time;
}
