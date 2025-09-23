#ifndef _USERNOTEINFO_H_
#define _USERNOTEINFO_H_

#include "sharestruct.h"
#include "GmtTime.h"
#include <vector>
using std::vector;

// 用户投资信息管理
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CUserNoteMap : public CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>
{
public:
	typedef CMap<KEY, ARG_KEY, VALUE, ARG_VALUE> _MyBase;

	CUserNoteMap(int nBlockSize = 10) : _MyBase(nBlockSize) {}

	VALUE *FindValue(ARG_KEY key)
	{
		// 查找key对应的value，有则返回指向value的指针，否则NULL
		// 该指针可能会在变更map数据结构的操作后失效
		ASSERT_VALID(this);
		
		UINT nHash,nHashValue;
		CAssoc* pAssoc = GetAssocAt(key, nHash,nHashValue);
		if (pAssoc == NULL)
			return NULL;  // not in map
		
		return &pAssoc->value;
	}

	const VALUE *FindValue(ARG_KEY key) const
	{
		return const_cast<CUserNoteMap *>(this)->FindValue(key);
	}
};

template<>
AFX_INLINE UINT AFXAPI HashKey<const CMerchKey &>(const CMerchKey & key)
{
	// default identity hash - works for most primitive values
	UINT uu = 0;
	LPCTSTR lpszCode = key.m_StrMerchCode;
	if ( NULL != lpszCode )
	{
		while ( *lpszCode != _T('\0') )
		{
			uu += (USHORT)(*lpszCode++);
			__asm ror uu, 1;		// 无符号循环右移1位
		}
	}
	uu += key.m_iMarketId;
	
	return uu;
} 

inline bool32 operator==(const CMerchKey &key1, const CMerchKey &key2 )
{
	return const_cast<CMerchKey &>(key1).IsSameMerch(key2);
}

#define USERNOTE_MAX_TITLE_LENGTH (63)
#define USERNTOE_MAX_CONTENT_LENGTH (8191)

struct T_UserNoteInfo
{
	CMerchKey   m_MerchKey;		// 商品标志

	int32		m_iUniqueId;	// 标志该笔记的唯一性(实际为创建时间与一个数值产生)
	CGmtTime	m_Time;			// 对应的日K线时间, 保持hms不变
	CString		m_StrTitle;		// 标题
	//CString		m_StrWeather;	// 天气
	int32		m_iWeather;		// 天气
	CString		m_StrContent;	// 内容(除固定html信息外的内容, 显示在edit中的)
};

enum E_UserNoteWeather
{
	EUNW_SunShine = 0,		// 晴天
	EUNW_Cloudy,			// 阴天
	EUNW_Rain,				// 雨天
	EUNW_Snow,				// 雪天

	EUNW_Count
};

CString GetUserNoteWeatherString(E_UserNoteWeather eUNW);

class CUserNoteInfoManager
{
public:
	typedef vector<T_UserNoteInfo>	UserNotesArray;
	typedef CUserNoteMap<CMerchKey, const CMerchKey &, UserNotesArray, const UserNotesArray &> UserNotesMap;
public:
	static CUserNoteInfoManager &Instance();

	void	GetUserNotesHeaderArray(const CMerchKey &merch, OUT UserNotesArray &aNotes);	// 加载用户该股票的所有笔记头

	// 动作接口
	bool32	NewUserNote(INOUT T_UserNoteInfo &note);	// 使用指定标题，天气，内容，时间，同时修改id新建一个投资笔记并创建文件
	bool32	ModifyUserNote(INOUT T_UserNoteInfo &note);	// 修改一个存在的笔记信息并保存到文件
	void	DelUserNote(const T_UserNoteInfo &note);		// 删除
	int32	ClearUserAllNote(const CMerchKey &merch);							// 清空某只股票笔记

	// 由于需要用户名，所以不能static
	bool32  GetUserMerchNoteDir(const CMerchKey &merch, OUT CString &StrDir);
	CString GetNoteHeaderFileName(const CMerchKey &merch);	// 获取笔记头文件名
	CString GetNoteContentFileName(const T_UserNoteInfo &note); // 获取该笔记对应的内容文件
	// 系统仅缓存笔记头部信息，如果没有读取过该内容，则不预先读取具体内容信息
	bool32	ReadUserNoteContent(INOUT T_UserNoteInfo &note);	// 读取该笔记内容，输入有效笔记头部


	static void InitNewUserNote(INOUT T_UserNoteInfo &note);	// 初始化一个默认的note

private:
	CUserNoteInfoManager() {};
	CUserNoteInfoManager(const CUserNoteInfoManager &);
	const CUserNoteInfoManager &operator=(const CUserNoteInfoManager &);

	bool32	SaveUserNote(const T_UserNoteInfo &note);					// 保存该笔记头部和内容信息，所有这个商品的头部会重新保存
	bool32	SaveUserNotesHeaderArray(const UserNotesArray &aNotes);		// 保存其列表信息，不保存内容

	UserNotesMap		m_mapUserNotes;		// 缓存的笔记头部信息，所有的都只扫描一次，第二次不在扫描，并在各操作接口更新其信息
};

#endif //_USERNOTEINFO_H_