#include "StdAfx.h"

#include "shellapi.h"
#include "UserNoteInfo.h"
#include "PathFactory.h"
#include "ShareFun.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 
#pragma comment(lib, "shell32.lib")

// �洢�ṹ:
// private/[user]/note/[Ŀ¼ marketid]/[Ŀ¼ ��Ʒ��]/��Ʒ��.nhd		�ʼǵ�ͷ
// private/[user]/note/[Ŀ¼ marketid]/[Ŀ¼ ��Ʒ��]/id.nct			�ʼǵ�����
// note�ļ��洢�ṹ:
// "note"��־ ��¼��Ŀ����¼ÿ���ṹ��С
// ���ɼ�¼
struct _T_UserNoteSave		// ����Ľṹ
{
	int32	iUniqueId;		// ��־�ñʼ�Ψһ��, ͬʱ��ע�����ļ�
	int32	dwTime;			// ��Ӧ��K��ʱ��
	int32	iWeather;		// ����

	TCHAR	szTitle[USERNOTE_MAX_TITLE_LENGTH+1];	// 63+1char�ı���
};

// �ʼ�������unicodeֱ�Ӵ洢

// ʹ��ʱ�����ض���ʽ��,css�ļ���htmlͷ��,β����ϳ�һ��html�ļ�������iebrower��ʾ

const CString KStrNoteHeaderExt = _T(".nhd");
const CString KStrNoteContentExt = _T(".nct");
const DWORD	  KDwNoteHeaderFlag = 0x65746f6e;		// "note"

const T_UserNoteInfo ConvertSaveNote2Note(T_UserNoteInfo &note, const _T_UserNoteSave &saveNote)
{
	note.m_StrContent.Empty();

	note.m_iUniqueId	= saveNote.iUniqueId;
	//note.m_StrWeather	= GetUserNoteWeatherString((E_UserNoteWeather)saveNote.iWeather);
	note.m_iWeather		= saveNote.iWeather;
	note.m_StrTitle		= saveNote.szTitle;
	note.m_Time			= (time_t)saveNote.dwTime;
	return note;
}

const _T_UserNoteSave &ConvertNote2SaveNote(_T_UserNoteSave &saveNote, const T_UserNoteInfo &note)
{
	saveNote.iUniqueId = note.m_iUniqueId;
	saveNote.iWeather  = note.m_iWeather;
	saveNote.dwTime		= (DWORD)note.m_Time.GetTime();

	ZeroMemory(saveNote.szTitle, sizeof(saveNote.szTitle));
	const int iMaxLength = sizeof(saveNote.szTitle)/sizeof(saveNote.szTitle[0]) -1;
	_tcsncpy(saveNote.szTitle, note.m_StrTitle, iMaxLength);	// ���������ֽ�����+\0
#ifdef _DEBUG
	if ( note.m_StrTitle.GetLength() > iMaxLength )
	{
		TRACE(_T("Ͷ���ռǱ��ⳤ�ȹ�����%d/%d\r\n"), note.m_StrTitle.GetLength(), iMaxLength);
	}
#endif //_DEBUG

	return saveNote;
}

bool32 operator==(const T_UserNoteInfo &note1, const T_UserNoteInfo &note2)
{
	// ���ݲ��Ƚ�
	return note1.m_iUniqueId == note2.m_iUniqueId
		&& note1.m_iWeather == note2.m_iWeather
		&& note1.m_Time == note2.m_Time
		&& note1.m_StrTitle == note2.m_StrTitle
		&& const_cast<CMerchKey &>(note1.m_MerchKey).IsSameMerch(note2.m_MerchKey);
}

/*
	error 568: (Warning -- non-negative quantity is never less than zero)
*/
//lint --e{568}
CString GetUserNoteWeatherString( E_UserNoteWeather eUNW )
{
	if ( eUNW >= EUNW_Count || eUNW < EUNW_SunShine )
	{
		eUNW = EUNW_SunShine;
	}

	switch ( eUNW )
	{
	case EUNW_SunShine:
		return _T("��");
		break;
	case EUNW_Cloudy:
		return _T("��");
		break;
	case EUNW_Rain:
		return _T("��");
		break;
	case EUNW_Snow:
		return _T("ѩ");
		break;
	default:
		;//ASSERT(0);
	}
	return _T("��");
}

bool32 CUserNoteInfoManager::GetUserMerchNoteDir( const CMerchKey &merch, OUT CString &StrDir )
{
	StrDir.Empty();

	if ( merch.m_StrMerchCode.IsEmpty() )
	{
		ASSERT( 0 );
		return false;		// �Ƿ���Ʒ
	}

	/*
		error 437: (Warning -- Passing struct 'CStringT' to ellipsis)
	*/
	//lint --e{437}
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	CString StrPrivate = CPathFactory::GetUserNoteInfoFilePath(pDoc->m_pAbsCenterManager->GetUserName());
	StrPrivate.Replace(_T('\\'), _T('/'));
	StrDir.Format(_T("%s%d/%s/"), StrPrivate, merch.m_iMarketId, merch.m_StrMerchCode);

	CString StrDir2 = CPathFactory::ComPathToAbsPath(StrDir);
	if ( !StrDir2.IsEmpty() )
	{
		StrDir = StrDir2;
	}
	StrDir.Replace(_T('\\'), _T('/'));
	if ( !StrDir.IsEmpty() && StrDir.Right(1) != _T("/") )
	{
		StrDir += _T('/');
	}

	return true;
}

CString CUserNoteInfoManager::GetNoteHeaderFileName( const CMerchKey &merch )
{
	CString StrPath;
	if ( !GetUserMerchNoteDir(merch, StrPath) )
	{
		return _T("");
	}
	StrPath.TrimRight(_T('/'));
	return StrPath + _T("/") + merch.m_StrMerchCode + KStrNoteHeaderExt;
}

/*
	error 437: (Warning -- Passing struct 'CStringT' to ellipsis)
*/
//lint --e{437}
CString CUserNoteInfoManager::GetNoteContentFileName( const T_UserNoteInfo &note )
{
	CString StrPath;
	if ( !GetUserMerchNoteDir(note.m_MerchKey, StrPath) )
	{
		return _T("");
	}
	StrPath.TrimRight(_T('/'));

	CString StrName;
	StrName.Format(_T("/%u%s"), note.m_iUniqueId, KStrNoteContentExt);
	
	return StrPath + StrName;
}


void CUserNoteInfoManager::GetUserNotesHeaderArray( const CMerchKey &merch, OUT UserNotesArray &aNotes )
{
	aNotes.clear();

	// �鿴�Ƿ��Ѿ��иû���
	if ( m_mapUserNotes.Lookup(merch, aNotes) )
	{
		return;	// ֱ��ʹ�û���
	}

	CString StrHeaderFile = GetNoteHeaderFileName(merch);
	if ( StrHeaderFile.IsEmpty() )
	{
		return;
	}

	HANDLE hFile = ::CreateFile(StrHeaderFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		return;	// �޴��ļ�
	}

	// "note"��־ ��¼��Ŀ����¼ÿ���ṹ��С
	// ���ɼ�¼
	DWORD dwFlag;
	DWORD dwRead = 0;
	if ( !ReadFile(hFile, &dwFlag, sizeof(dwFlag), &dwRead, NULL)
		|| dwRead != sizeof(dwFlag)
		|| dwFlag != KDwNoteHeaderFlag )
	{
		DEL_HANDLE(hFile);
		ASSERT( 0 );
		return;	// ͷ����־����
	}

	DWORD dwCount;
	DWORD dwStructSize;
	dwCount = dwStructSize = 0;
	BOOL bRead = FALSE;
	bRead = ReadFile(hFile, &dwCount, sizeof(dwCount), &dwRead, NULL);
	bRead = bRead && ReadFile(hFile, &dwStructSize, sizeof(dwStructSize), &dwRead, NULL);
	if ( !bRead || dwRead != sizeof(dwStructSize) )
	{
		DEL_HANDLE(hFile);
		ASSERT( 0 );
		return;		// ����
	}

	// �жϴ�С
	if ( dwStructSize < sizeof(_T_UserNoteSave) )
	{
		DEL_HANDLE(hFile);
		ASSERT( 0 );
		return;	// �����ݴ�����
	}

	BYTE *pByte = new BYTE[dwStructSize];
	if ( NULL == pByte )
	{
		DEL_HANDLE(hFile);
		ASSERT( 0 );
		return;	// ���ڴ�
	}

	T_UserNoteInfo note;
	note.m_MerchKey = merch;
	DWORD dw = 0;
	for ( dw=0; dw < dwCount ; dw++ )
	{
		bRead = ReadFile(hFile, pByte, dwStructSize, &dwRead, NULL);
		if ( !bRead || dwStructSize != dwRead )
		{
			break;		// �������
		}

		// ���ڲ��������ݣ��ȵ�ʵ������ص�ʱ���ټ���

		aNotes.push_back(ConvertSaveNote2Note(note, *(_T_UserNoteSave *)pByte));
	}
	// ������
	m_mapUserNotes[ merch ] = aNotes;
	
	delete []pByte;
	pByte = NULL;

	DEL_HANDLE(hFile);

	ASSERT( dw == dwCount );	// �����������ݾ�����
	return;
}

bool32 CUserNoteInfoManager::SaveUserNotesHeaderArray( const UserNotesArray &aNotes )
{
	if ( aNotes.size() <= 0 )
	{
		ASSERT( 0 );
		return false;	// �˶�����ɾ��������Ӧ���ɵ���ʱע��
	}

	CMerchKey merch = aNotes[0].m_MerchKey;

	CString StrHeaderFile = GetNoteHeaderFileName(merch);
	if ( StrHeaderFile.IsEmpty() )
	{
		return false;
	}

	{
		// ������Ҫ��Ŀ¼�ṹ
		CString StrDir;
		GetUserMerchNoteDir(merch, StrDir);
		if ( _taccess(StrDir, 0) != 0 )
		{
			CString StrMkDir(StrDir);
			_tcheck_if_mkdir(StrMkDir.LockBuffer());	// ������Ŀ¼
			StrMkDir.UnlockBuffer();
		}
	}
	
	HANDLE hFile = ::CreateFile(StrHeaderFile,
		GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		ASSERT( 0 );
		return false;	// ����ʧ��
	}
	
	// "note"��־ ��¼��Ŀ����¼ÿ���ṹ��С
	// ���ɼ�¼
	DWORD dwWritten = 0;
	BOOL  bWritten = FALSE;
	bWritten = WriteFile(hFile, &KDwNoteHeaderFlag, sizeof(KDwNoteHeaderFlag), &dwWritten, NULL);	// ͷ����־
	if ( !bWritten || dwWritten != sizeof(KDwNoteHeaderFlag) )
	{
		DEL_HANDLE(hFile);
		ASSERT( 0 );
		return false;
	}
	
	
	const DWORD dwCount = (DWORD)aNotes.size();
	const DWORD dwStructSize = sizeof(_T_UserNoteSave);

	bWritten = WriteFile(hFile, &dwCount, sizeof(dwCount), &dwWritten, NULL);
	bWritten = bWritten && WriteFile(hFile, &dwStructSize, sizeof(dwStructSize), &dwWritten, NULL);
	if ( !bWritten || dwWritten != sizeof(dwStructSize) )
	{
		DEL_HANDLE(hFile);
		ASSERT( 0 );
		return false;	// �޷�����
	}

	_T_UserNoteSave saveNote;
	
	DWORD dw = 0;
	for ( dw=0; dw < dwCount ; dw++ )
	{
		ConvertNote2SaveNote(saveNote, aNotes[dw]);
		bWritten = WriteFile(hFile, &saveNote, dwStructSize, &dwWritten, NULL);
		if ( !bWritten || dwStructSize != dwWritten )
		{
			break;		// ���沿��ʧ���ˣ�զ��
		}
	}
	DEL_HANDLE(hFile);

	ASSERT( dwCount == dw );	// �����������ݾͷ���ʧ��
	
	return dwCount == dw;
}

bool32 CUserNoteInfoManager::SaveUserNote( const T_UserNoteInfo &note )
{
	CMerchKey merch = note.m_MerchKey;
	
	CString StrContentFile = GetNoteContentFileName(note);
	if ( StrContentFile.IsEmpty() )
	{
		return false;
	}

	{
		// ������Ҫ��Ŀ¼�ṹ
		CString StrDir;
		GetUserMerchNoteDir(merch, StrDir);
		if ( _taccess(StrDir, 0) != 0 )
		{
			CString StrMkDir(StrDir);
			_tcheck_if_mkdir(StrMkDir.LockBuffer());	// ������Ŀ¼
			StrMkDir.UnlockBuffer();
		}
	}

	// �ȱ�������
	// ���ݽ������ִ�
	bool32 bSaveContent = true;
	if ( !note.m_StrContent.IsEmpty() )
	{
		CStdioFile file;
		if ( !file.Open(StrContentFile, CFile::modeCreate |CFile::modeWrite |CFile::typeBinary) )
		{
			ASSERT( 0 );
			bSaveContent = false;
		}
		else
		{
			if ( note.m_StrContent.GetLength() > USERNTOE_MAX_CONTENT_LENGTH )
			{
				CString StrBuf = note.m_StrContent.Left(USERNTOE_MAX_CONTENT_LENGTH);
				file.WriteString(StrBuf);
				file.Close();

				ASSERT( 0 );	// ��Ӧ�ó��ֵ�λ��
				CString StrPrompt;
				StrPrompt.Format(_T("Ͷ���ռ����ݹ�����%d/%d, ���ض�\r\n"), note.m_StrContent.GetLength(), USERNTOE_MAX_CONTENT_LENGTH);
				AfxMessageBox(StrPrompt);
			}
			else
			{
				file.WriteString(note.m_StrContent);
				file.Close();
			}
			
		}
	}
	else
	{
		// �����ݣ�ɾ���ļ�
		DeleteFile(StrContentFile);
	}

	// Ȼ�󱣴��б��ļ�
	// �ҵ��б��ļ��б�����ĿӦ���������ͬ��
	UserNotesArray *pNotes = NULL;
	if ( NULL == (pNotes=m_mapUserNotes.FindValue(merch)) )
	{
		return false;
	}
#ifdef _DEBUG
	{
		int32 i = 0;
		for ( i=0; i < pNotes->size() ; i++ )
		{
			if ( (*pNotes)[i] == note )
			{
				break;
			}
		}
		ASSERT( i < pNotes->size() );	// Ӧ�����ȸ�����ͷ����Ϣ��
	}
#endif //_DEBUG
	return SaveUserNotesHeaderArray(*pNotes) && bSaveContent;
}

bool32 CUserNoteInfoManager::ReadUserNoteContent( INOUT T_UserNoteInfo &note )
{
	if ( !note.m_StrContent.IsEmpty() )
	{
		return true;		// ���ǰ���ȡ�����ݣ���ʹ��ǰ���ȡ��
	}

	CMerchKey merch = note.m_MerchKey;
	
	CString StrContentFile = GetNoteContentFileName(note);
	if ( StrContentFile.IsEmpty() )
	{
		return false;
	}

	// ֱ�Ӷ�ȡ�ִ�����
	CStdioFile file;
	if ( file.Open(StrContentFile, CFile::modeRead |CFile::typeBinary) )
	{
		// ��ȡȫ��������
		DWORD dwLength = file.GetLength();
		const int dwMaxLength = USERNTOE_MAX_CONTENT_LENGTH*sizeof(TCHAR);
		if ( dwLength > dwMaxLength )
		{
			TRACE(_T("��ȡͶ���ռ��ı����ݳ��ȹ���, ���ض�Ϊ��󳤶�: %u/%d(byte)\r\n"), dwLength, dwMaxLength);
			dwLength = USERNTOE_MAX_CONTENT_LENGTH*sizeof(TCHAR);
		}
		const int dwBufferChCount = (dwLength+1)/sizeof(TCHAR) +1;
		UINT uRead = 0;
		if ( (uRead =file.Read(note.m_StrContent.GetBuffer(dwBufferChCount), dwBufferChCount*sizeof(TCHAR))) > 0 )
		{
			note.m_StrContent.ReleaseBuffer(uRead/sizeof(TCHAR));
			file.Close();
			return true;
		}
		else
		{
			note.m_StrContent.ReleaseBuffer();
			note.m_StrContent.Empty();
			file.Close();
		}
	}
	return false;
}

bool32 CUserNoteInfoManager::NewUserNote( INOUT T_UserNoteInfo &note )
{
	// ʹ��ָ�����⣬���������ݣ�ʱ���½�һ��Ͷ�ʱʼǲ������ļ�
	// ���������Ӧ����Ӧ���������ͼ����ȥ TODO
	ASSERT( note.m_Time.GetTime() != 0 );
	ASSERT( !note.m_StrTitle.IsEmpty() );
	ASSERT( !note.m_MerchKey.m_StrMerchCode.IsEmpty() );
	// �п��ܱ��������
	
	// ��ӽ�����
	if ( !note.m_MerchKey.m_StrMerchCode.IsEmpty() )
	{
		DWORD dwId = (DWORD)time(NULL);
		dwId <<=4;
		dwId += GetTickCount();
		note.m_iUniqueId = (int32)dwId;		// �½���note�����½���id
		UserNotesArray &aNotes = m_mapUserNotes[note.m_MerchKey];
		
		// ���������Ӧ���Ǻ����ظ���
#ifdef _DEBUG
		{
			for ( int i=0; i < aNotes.size() ; i++ )
			{
				ASSERT( aNotes[i].m_iUniqueId != note.m_iUniqueId );
			}
		}
#endif //_DEBUG

		// ����ʱ�䣬������ʵ�λ��, ��һ���������
		bool32 bAdded = false;
		for (UserNotesArray::reverse_iterator it = aNotes.rbegin(); it != aNotes.rend() ; it++ )
		{
			if ( note.m_Time > it->m_Time )
			{
				aNotes.insert(it.base(), note);	// ����һ��
				bAdded = true;
				break;
			}
		}
		if ( !bAdded )
		{
			aNotes.insert(aNotes.begin(), note);	// ��С��һ��
		}

		// ��ʱmap���Ѿ��и�noteȫ������Ϣ��(������)
		return SaveUserNote(note);
	}
	return false;
}

bool32 CUserNoteInfoManager::ModifyUserNote( INOUT T_UserNoteInfo &note )
{
	// �޸�һ�����ڵıʼ���Ϣ�����浽�ļ�, id����
	// ���������Ӧ����Ӧ���������ͼ����ȥ TODO
	ASSERT( note.m_iUniqueId != 0 );
	ASSERT( !note.m_MerchKey.m_StrMerchCode.IsEmpty() );
	ASSERT( !note.m_StrTitle.IsEmpty() );
	ASSERT( note.m_Time.GetTime() != 0 );

	// ����Ҫ����ڸ�note��¼
	UserNotesArray *pNotes;	
	if ( (pNotes=m_mapUserNotes.FindValue(note.m_MerchKey)) == NULL )
	{
		ASSERT( 0 );
		return false;	// �޴˼�¼
	}

	UserNotesArray::reverse_iterator it;
	for ( it= pNotes->rbegin(); it != pNotes->rend() ; it++ )
	{
		if ( it->m_iUniqueId == note.m_iUniqueId )
		{
			break;
		}
	}
	if ( it == pNotes->rend() )
	{
		ASSERT( 0 );
		return false;	// �޴˼�¼
	}

	// ���Ƿ���Ҫ���������п���ʱ������
	if ( it->m_Time == note.m_Time )
	{
		*it = note;	// ���±ʼ�
	}
	else
	{
		// ��������
		pNotes->erase(it.base()-1);	// ɾ��ԭ��¼
		bool32 bAdded = false;
		for ( it = pNotes->rbegin() ; it != pNotes->rend() ; it++ )
		{
			if ( note.m_Time > it->m_Time )
			{
				pNotes->insert(it.base(), note);	// ���뵽����
				bAdded = true;
				break;
			}
		}
		if ( !bAdded )
		{
			pNotes->insert(pNotes->begin(), note);	// ��С��һ��
		}
	}
	return SaveUserNote(note);	// ����ñʼ����б�ͷ
}

void CUserNoteInfoManager::DelUserNote( const T_UserNoteInfo &note )
{
	// ɾ���ñʼǵĻ����������ļ��������±���ͷ����Ϣ�б�
	// ���������Ӧ����Ӧ���������ͼ����ȥ TODO
	ASSERT( note.m_iUniqueId != 0 );
	ASSERT( !note.m_MerchKey.m_StrMerchCode.IsEmpty() );
	ASSERT( !note.m_StrTitle.IsEmpty() );
	ASSERT( note.m_Time.GetTime() != 0 );

	CString StrContentFileName = GetNoteContentFileName(note);
	if ( !StrContentFileName.IsEmpty() )
	{
		::DeleteFile(StrContentFileName);	// �����ļ�ɾ��
	}
	
	// ����Ҫ����ڸ�note��¼
	UserNotesArray *pNotes;	
	if ( (pNotes=m_mapUserNotes.FindValue(note.m_MerchKey)) == NULL )
	{
		ASSERT( 0 );
		return;	// �޴˼�¼
	}
	
	UserNotesArray::reverse_iterator it;
	for ( it= pNotes->rbegin(); it != pNotes->rend() ; it++ )
	{
		if ( it->m_iUniqueId == note.m_iUniqueId )
		{
			break;
		}
	}
	if ( it == pNotes->rend() )
	{
		ASSERT( 0 );
		return;	// �޴˼�¼
	}

	pNotes->erase(it.base()-1);	// ɾ���ü�¼

	if ( pNotes->size() <= 0 )
	{
		ClearUserAllNote(note.m_MerchKey);	// ���ˣ������
	}
	else
	{
		SaveUserNotesHeaderArray(*pNotes);	// ���±���ͷ���б��ļ�
	}
}

int32 CUserNoteInfoManager::ClearUserAllNote( const CMerchKey &merch )
{
	// ɾ������Ʒ��صıʼǵĻ������ļ�
	// ���������Ӧ����Ӧ���������ͼ����ȥ TODO
	CString StrMerchDir;
	if ( GetUserMerchNoteDir(merch, StrMerchDir) )
	{
		StrMerchDir.Replace(_T('/'), _T('\\'));
		StrMerchDir.TrimRight(_T('\\'));
		// ɾ����ƷĿ¼�µ��ļ�
		SHFILEOPSTRUCT sfos;
		ZeroMemory(&sfos, sizeof(sfos));
		sfos.wFunc = FO_DELETE;
		sfos.fFlags = FOF_NOCONFIRMATION |FOF_NOERRORUI |FOF_SILENT;
		LPTSTR pBuff = new TCHAR[MAX_PATH];
		ZeroMemory(pBuff, MAX_PATH);
		ASSERT( StrMerchDir.GetLength() <= MAX_PATH-2 );
		_tcsncpy(pBuff, StrMerchDir, MAX_PATH-2);
		sfos.pFrom = pBuff;

		SHFileOperation(&sfos);

		delete []pBuff;
	}
	
	int32 iDel = 0;
	UserNotesArray *pNotes = m_mapUserNotes.FindValue(merch);
	if ( NULL != pNotes )
	{
		iDel = pNotes->size();
	}

	m_mapUserNotes.RemoveKey(merch);	// ɾ������
	return iDel;
}

CUserNoteInfoManager & CUserNoteInfoManager::Instance()
{
	static CUserNoteInfoManager sInstace;
	return sInstace;
}

void CUserNoteInfoManager::InitNewUserNote( INOUT T_UserNoteInfo &note )
{
	T_UserNoteInfo noteNew;
	note = noteNew;
	note.m_iUniqueId = 0;
	note.m_iWeather = EUNW_SunShine;
	note.m_Time = CGmtTime::GetCurrentTime();
}
