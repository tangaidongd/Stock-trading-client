#ifndef _USERNOTEINFO_H_
#define _USERNOTEINFO_H_

#include "sharestruct.h"
#include "GmtTime.h"
#include <vector>
using std::vector;

// �û�Ͷ����Ϣ����
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CUserNoteMap : public CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>
{
public:
	typedef CMap<KEY, ARG_KEY, VALUE, ARG_VALUE> _MyBase;

	CUserNoteMap(int nBlockSize = 10) : _MyBase(nBlockSize) {}

	VALUE *FindValue(ARG_KEY key)
	{
		// ����key��Ӧ��value�����򷵻�ָ��value��ָ�룬����NULL
		// ��ָ����ܻ��ڱ��map���ݽṹ�Ĳ�����ʧЧ
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
			__asm ror uu, 1;		// �޷���ѭ������1λ
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
	CMerchKey   m_MerchKey;		// ��Ʒ��־

	int32		m_iUniqueId;	// ��־�ñʼǵ�Ψһ��(ʵ��Ϊ����ʱ����һ����ֵ����)
	CGmtTime	m_Time;			// ��Ӧ����K��ʱ��, ����hms����
	CString		m_StrTitle;		// ����
	//CString		m_StrWeather;	// ����
	int32		m_iWeather;		// ����
	CString		m_StrContent;	// ����(���̶�html��Ϣ�������, ��ʾ��edit�е�)
};

enum E_UserNoteWeather
{
	EUNW_SunShine = 0,		// ����
	EUNW_Cloudy,			// ����
	EUNW_Rain,				// ����
	EUNW_Snow,				// ѩ��

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

	void	GetUserNotesHeaderArray(const CMerchKey &merch, OUT UserNotesArray &aNotes);	// �����û��ù�Ʊ�����бʼ�ͷ

	// �����ӿ�
	bool32	NewUserNote(INOUT T_UserNoteInfo &note);	// ʹ��ָ�����⣬���������ݣ�ʱ�䣬ͬʱ�޸�id�½�һ��Ͷ�ʱʼǲ������ļ�
	bool32	ModifyUserNote(INOUT T_UserNoteInfo &note);	// �޸�һ�����ڵıʼ���Ϣ�����浽�ļ�
	void	DelUserNote(const T_UserNoteInfo &note);		// ɾ��
	int32	ClearUserAllNote(const CMerchKey &merch);							// ���ĳֻ��Ʊ�ʼ�

	// ������Ҫ�û��������Բ���static
	bool32  GetUserMerchNoteDir(const CMerchKey &merch, OUT CString &StrDir);
	CString GetNoteHeaderFileName(const CMerchKey &merch);	// ��ȡ�ʼ�ͷ�ļ���
	CString GetNoteContentFileName(const T_UserNoteInfo &note); // ��ȡ�ñʼǶ�Ӧ�������ļ�
	// ϵͳ������ʼ�ͷ����Ϣ�����û�ж�ȡ�������ݣ���Ԥ�ȶ�ȡ����������Ϣ
	bool32	ReadUserNoteContent(INOUT T_UserNoteInfo &note);	// ��ȡ�ñʼ����ݣ�������Ч�ʼ�ͷ��


	static void InitNewUserNote(INOUT T_UserNoteInfo &note);	// ��ʼ��һ��Ĭ�ϵ�note

private:
	CUserNoteInfoManager() {};
	CUserNoteInfoManager(const CUserNoteInfoManager &);
	const CUserNoteInfoManager &operator=(const CUserNoteInfoManager &);

	bool32	SaveUserNote(const T_UserNoteInfo &note);					// ����ñʼ�ͷ����������Ϣ�����������Ʒ��ͷ�������±���
	bool32	SaveUserNotesHeaderArray(const UserNotesArray &aNotes);		// �������б���Ϣ������������

	UserNotesMap		m_mapUserNotes;		// ����ıʼ�ͷ����Ϣ�����еĶ�ֻɨ��һ�Σ��ڶ��β���ɨ�裬���ڸ������ӿڸ�������Ϣ
};

#endif //_USERNOTEINFO_H_