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

// 存储结构:
// private/[user]/note/[目录 marketid]/[目录 商品名]/商品名.nhd		笔记的头
// private/[user]/note/[目录 marketid]/[目录 商品名]/id.nct			笔记的内容
// note文件存储结构:
// "note"标志 记录数目，记录每个结构大小
// 若干记录
struct _T_UserNoteSave		// 保存的结构
{
	int32	iUniqueId;		// 标志该笔记唯一性, 同时标注数据文件
	int32	dwTime;			// 对应的K线时间
	int32	iWeather;		// 天气

	TCHAR	szTitle[USERNOTE_MAX_TITLE_LENGTH+1];	// 63+1char的标题
};

// 笔记内容以unicode直接存储

// 使用时，与特定格式串,css文件和html头部,尾部组合成一个html文件，调用iebrower显示

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
	_tcsncpy(saveNote.szTitle, note.m_StrTitle, iMaxLength);	// 拷贝若干字节数据+\0
#ifdef _DEBUG
	if ( note.m_StrTitle.GetLength() > iMaxLength )
	{
		TRACE(_T("投资日记标题长度过长：%d/%d\r\n"), note.m_StrTitle.GetLength(), iMaxLength);
	}
#endif //_DEBUG

	return saveNote;
}

bool32 operator==(const T_UserNoteInfo &note1, const T_UserNoteInfo &note2)
{
	// 内容不比较
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
		return _T("晴");
		break;
	case EUNW_Cloudy:
		return _T("阴");
		break;
	case EUNW_Rain:
		return _T("雨");
		break;
	case EUNW_Snow:
		return _T("雪");
		break;
	default:
		;//ASSERT(0);
	}
	return _T("晴");
}

bool32 CUserNoteInfoManager::GetUserMerchNoteDir( const CMerchKey &merch, OUT CString &StrDir )
{
	StrDir.Empty();

	if ( merch.m_StrMerchCode.IsEmpty() )
	{
		ASSERT( 0 );
		return false;		// 非法商品
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

	// 查看是否已经有该缓存
	if ( m_mapUserNotes.Lookup(merch, aNotes) )
	{
		return;	// 直接使用缓存
	}

	CString StrHeaderFile = GetNoteHeaderFileName(merch);
	if ( StrHeaderFile.IsEmpty() )
	{
		return;
	}

	HANDLE hFile = ::CreateFile(StrHeaderFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		return;	// 无此文件
	}

	// "note"标志 记录数目，记录每个结构大小
	// 若干记录
	DWORD dwFlag;
	DWORD dwRead = 0;
	if ( !ReadFile(hFile, &dwFlag, sizeof(dwFlag), &dwRead, NULL)
		|| dwRead != sizeof(dwFlag)
		|| dwFlag != KDwNoteHeaderFlag )
	{
		DEL_HANDLE(hFile);
		ASSERT( 0 );
		return;	// 头部标志不对
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
		return;		// 错误
	}

	// 判断大小
	if ( dwStructSize < sizeof(_T_UserNoteSave) )
	{
		DEL_HANDLE(hFile);
		ASSERT( 0 );
		return;	// 不兼容此数据
	}

	BYTE *pByte = new BYTE[dwStructSize];
	if ( NULL == pByte )
	{
		DEL_HANDLE(hFile);
		ASSERT( 0 );
		return;	// 无内存
	}

	T_UserNoteInfo note;
	note.m_MerchKey = merch;
	DWORD dw = 0;
	for ( dw=0; dw < dwCount ; dw++ )
	{
		bRead = ReadFile(hFile, pByte, dwStructSize, &dwRead, NULL);
		if ( !bRead || dwStructSize != dwRead )
		{
			break;		// 数据完毕
		}

		// 现在不加载内容，等到实际想加载的时候再加载

		aNotes.push_back(ConvertSaveNote2Note(note, *(_T_UserNoteSave *)pByte));
	}
	// 缓存结果
	m_mapUserNotes[ merch ] = aNotes;
	
	delete []pByte;
	pByte = NULL;

	DEL_HANDLE(hFile);

	ASSERT( dw == dwCount );	// 可能少了数据就算了
	return;
}

bool32 CUserNoteInfoManager::SaveUserNotesHeaderArray( const UserNotesArray &aNotes )
{
	if ( aNotes.size() <= 0 )
	{
		ASSERT( 0 );
		return false;	// 此动作即删除，但是应当由调用时注意
	}

	CMerchKey merch = aNotes[0].m_MerchKey;

	CString StrHeaderFile = GetNoteHeaderFileName(merch);
	if ( StrHeaderFile.IsEmpty() )
	{
		return false;
	}

	{
		// 创建必要的目录结构
		CString StrDir;
		GetUserMerchNoteDir(merch, StrDir);
		if ( _taccess(StrDir, 0) != 0 )
		{
			CString StrMkDir(StrDir);
			_tcheck_if_mkdir(StrMkDir.LockBuffer());	// 创建该目录
			StrMkDir.UnlockBuffer();
		}
	}
	
	HANDLE hFile = ::CreateFile(StrHeaderFile,
		GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		ASSERT( 0 );
		return false;	// 创建失败
	}
	
	// "note"标志 记录数目，记录每个结构大小
	// 若干记录
	DWORD dwWritten = 0;
	BOOL  bWritten = FALSE;
	bWritten = WriteFile(hFile, &KDwNoteHeaderFlag, sizeof(KDwNoteHeaderFlag), &dwWritten, NULL);	// 头部标志
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
		return false;	// 无法保存
	}

	_T_UserNoteSave saveNote;
	
	DWORD dw = 0;
	for ( dw=0; dw < dwCount ; dw++ )
	{
		ConvertNote2SaveNote(saveNote, aNotes[dw]);
		bWritten = WriteFile(hFile, &saveNote, dwStructSize, &dwWritten, NULL);
		if ( !bWritten || dwStructSize != dwWritten )
		{
			break;		// 保存部分失败了，咋办
		}
	}
	DEL_HANDLE(hFile);

	ASSERT( dwCount == dw );	// 可能少了数据就返回失败
	
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
		// 创建必要的目录结构
		CString StrDir;
		GetUserMerchNoteDir(merch, StrDir);
		if ( _taccess(StrDir, 0) != 0 )
		{
			CString StrMkDir(StrDir);
			_tcheck_if_mkdir(StrMkDir.LockBuffer());	// 创建该目录
			StrMkDir.UnlockBuffer();
		}
	}

	// 先保存内容
	// 内容仅保存字串
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

				ASSERT( 0 );	// 不应该出现的位置
				CString StrPrompt;
				StrPrompt.Format(_T("投资日记内容过长：%d/%d, 被截断\r\n"), note.m_StrContent.GetLength(), USERNTOE_MAX_CONTENT_LENGTH);
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
		// 空内容，删除文件
		DeleteFile(StrContentFile);
	}

	// 然后保存列表文件
	// 找到列表文件列表，该项目应当是与此相同的
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
		ASSERT( i < pNotes->size() );	// 应当是先更新了头部信息的
	}
#endif //_DEBUG
	return SaveUserNotesHeaderArray(*pNotes) && bSaveContent;
}

bool32 CUserNoteInfoManager::ReadUserNoteContent( INOUT T_UserNoteInfo &note )
{
	if ( !note.m_StrContent.IsEmpty() )
	{
		return true;		// 如果前面读取过内容，则使用前面读取的
	}

	CMerchKey merch = note.m_MerchKey;
	
	CString StrContentFile = GetNoteContentFileName(note);
	if ( StrContentFile.IsEmpty() )
	{
		return false;
	}

	// 直接读取字串内容
	CStdioFile file;
	if ( file.Open(StrContentFile, CFile::modeRead |CFile::typeBinary) )
	{
		// 读取全部的数据
		DWORD dwLength = file.GetLength();
		const int dwMaxLength = USERNTOE_MAX_CONTENT_LENGTH*sizeof(TCHAR);
		if ( dwLength > dwMaxLength )
		{
			TRACE(_T("读取投资日记文本内容长度过长, 被截断为最大长度: %u/%d(byte)\r\n"), dwLength, dwMaxLength);
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
	// 使用指定标题，天气，内容，时间新建一个投资笔记并创建文件
	// 如果标题变更应当反应到具体的视图上面去 TODO
	ASSERT( note.m_Time.GetTime() != 0 );
	ASSERT( !note.m_StrTitle.IsEmpty() );
	ASSERT( !note.m_MerchKey.m_StrMerchCode.IsEmpty() );
	// 有可能保存空内容
	
	// 添加进队列
	if ( !note.m_MerchKey.m_StrMerchCode.IsEmpty() )
	{
		DWORD dwId = (DWORD)time(NULL);
		dwId <<=4;
		dwId += GetTickCount();
		note.m_iUniqueId = (int32)dwId;		// 新建的note给与新建的id
		UserNotesArray &aNotes = m_mapUserNotes[note.m_MerchKey];
		
		// 正常情况下应当是很难重复了
#ifdef _DEBUG
		{
			for ( int i=0; i < aNotes.size() ; i++ )
			{
				ASSERT( aNotes[i].m_iUniqueId != note.m_iUniqueId );
			}
		}
#endif //_DEBUG

		// 根据时间，插入合适的位置, 第一个是最早的
		bool32 bAdded = false;
		for (UserNotesArray::reverse_iterator it = aNotes.rbegin(); it != aNotes.rend() ; it++ )
		{
			if ( note.m_Time > it->m_Time )
			{
				aNotes.insert(it.base(), note);	// 后面一个
				bAdded = true;
				break;
			}
		}
		if ( !bAdded )
		{
			aNotes.insert(aNotes.begin(), note);	// 最小的一个
		}

		// 此时map中已经有该note全部的信息了(含内容)
		return SaveUserNote(note);
	}
	return false;
}

bool32 CUserNoteInfoManager::ModifyUserNote( INOUT T_UserNoteInfo &note )
{
	// 修改一个存在的笔记信息并保存到文件, id不变
	// 如果标题变更应当反应到具体的视图上面去 TODO
	ASSERT( note.m_iUniqueId != 0 );
	ASSERT( !note.m_MerchKey.m_StrMerchCode.IsEmpty() );
	ASSERT( !note.m_StrTitle.IsEmpty() );
	ASSERT( note.m_Time.GetTime() != 0 );

	// 必须要求存在该note记录
	UserNotesArray *pNotes;	
	if ( (pNotes=m_mapUserNotes.FindValue(note.m_MerchKey)) == NULL )
	{
		ASSERT( 0 );
		return false;	// 无此记录
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
		return false;	// 无此记录
	}

	// 看是否需要重新排序，有可能时间变更了
	if ( it->m_Time == note.m_Time )
	{
		*it = note;	// 更新笔记
	}
	else
	{
		// 重新排序
		pNotes->erase(it.base()-1);	// 删除原记录
		bool32 bAdded = false;
		for ( it = pNotes->rbegin() ; it != pNotes->rend() ; it++ )
		{
			if ( note.m_Time > it->m_Time )
			{
				pNotes->insert(it.base(), note);	// 插入到后面
				bAdded = true;
				break;
			}
		}
		if ( !bAdded )
		{
			pNotes->insert(pNotes->begin(), note);	// 最小的一个
		}
	}
	return SaveUserNote(note);	// 保存该笔记与列表头
}

void CUserNoteInfoManager::DelUserNote( const T_UserNoteInfo &note )
{
	// 删除该笔记的缓存与内容文件，并重新保存头部信息列表
	// 如果标题变更应当反应到具体的视图上面去 TODO
	ASSERT( note.m_iUniqueId != 0 );
	ASSERT( !note.m_MerchKey.m_StrMerchCode.IsEmpty() );
	ASSERT( !note.m_StrTitle.IsEmpty() );
	ASSERT( note.m_Time.GetTime() != 0 );

	CString StrContentFileName = GetNoteContentFileName(note);
	if ( !StrContentFileName.IsEmpty() )
	{
		::DeleteFile(StrContentFileName);	// 内容文件删除
	}
	
	// 必须要求存在该note记录
	UserNotesArray *pNotes;	
	if ( (pNotes=m_mapUserNotes.FindValue(note.m_MerchKey)) == NULL )
	{
		ASSERT( 0 );
		return;	// 无此记录
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
		return;	// 无此记录
	}

	pNotes->erase(it.base()-1);	// 删除该记录

	if ( pNotes->size() <= 0 )
	{
		ClearUserAllNote(note.m_MerchKey);	// 空了，则清除
	}
	else
	{
		SaveUserNotesHeaderArray(*pNotes);	// 重新保存头部列表文件
	}
}

int32 CUserNoteInfoManager::ClearUserAllNote( const CMerchKey &merch )
{
	// 删除该商品相关的笔记的缓存与文件
	// 如果标题变更应当反应到具体的视图上面去 TODO
	CString StrMerchDir;
	if ( GetUserMerchNoteDir(merch, StrMerchDir) )
	{
		StrMerchDir.Replace(_T('/'), _T('\\'));
		StrMerchDir.TrimRight(_T('\\'));
		// 删除商品目录下的文件
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

	m_mapUserNotes.RemoveKey(merch);	// 删除缓存
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
