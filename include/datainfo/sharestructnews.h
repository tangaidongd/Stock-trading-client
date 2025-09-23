#ifndef _SHARE_STUCT_NEWS_H
#define _SHARE_STUCT_NEWS_H

#include "InfoExport.h"




// 协议中的字段值
const TCHAR KcSeprator			= 0x01;
const TCHAR	KcEnd				= 0x00;

const CString   KStrKeyInfoIndex		= L"InfoIndex";
const CString  KStrKeyParentIndex	= L"ParentIndex";
const CString  KStrKeyBeginPos		= L"BeginPos";
const CString  KStrKeyTotalCount		= L"TotalCount";
const CString  KStrKeyMarket			= L"Market";
const CString  KStrKeyCode			= L"Code";
const CString  KStrKeyDataType		= L"DataType";
const CString  KStrKeyInfoType		= L"InfoType";
const CString  KStrKeyChildDataType	= L"ChildDataType";
const CString  KStrKeyTitle			= L"Title";

const CString  KStrKeyContent		= L"Content";
const CString  KStrKeyUpdateTime		= L"UpdateTime";
const CString  KStrKeyCrc32			= L"CRC32";
const CString  KStrKeyAuthor			= L"Author";
const CString  KStrKeyCategory		= L"Category";
const CString  KStrKeyConLen			= L"ConLen";

// 两种类型的资讯index ID
// AFX_EXT_DATA const int32	  KiIndexJinping		= 100;
const int32 KiIndexNormal			= 3;

// 协议中前面的特殊字段个数
const int32	KiProFiledCounts		= 3;

// 资讯类型
enum E_InfoType
{
	EITDataSource = 1,			// 数据源
	EITF10,						// F10
	EITWeight,					// 权息信息	
	EITLandmine,				// 信息地雷
	EITCodeIndex,				// 代码对应资讯
	EITTitleList,				// 标题列表
	EITBulletin,				// 交易所公告
	EITNewsTree,				// 资讯树

	EITCount,
};

// 数据类型
enum E_DataType
{
	EDTTitle = 1,				// 新闻标题
	EDTContent,					// 新闻内容

	EDTCount,
};

// 描述请求包和应答包中的类型
enum E_NewsType
{
	ENTNetTest,					// 测试测速请求, 不需要登录既可发送, 数据长度在0-1K之间	
	
	ENTReqHeart,				// 存活包
	ENTRespHeart,				// 应答存活包

	ENTReqAuth,					// 认证包, 暂时不用
	ENTRespAuth,				// 应答认证包

	ENTReqInfoList,				// 请求资讯列表
	ENTRespInfoList,			// 应答咨询列表

	ENTReqF10,					// 请求 F10
	ENTRespF10,					// 应答 F10		
	
	ENTReqLandMine,				// 请求信息地雷
	ENTRespLandMine,			// 应答信息地雷

	ENTReqCodeIndex,			// 请求代码对应的资讯
	ENTRespCodeIndex,			// 应答代码对应的资讯

	ENTReqTitleList,			// 请求新闻标题列表
	ENTRespTitleList,			// 应答新闻标题列表

	ENTReqNewsContent,			// 请求新闻内容
	ENTRespNewsContent,			// 应答新闻内容

	ENTReqAddPush,				// 添加推送
	ENTReqDeletePush,			// 删除推送
	ENTReqUpdatePush,			// 同步推送
		
	ENTPushInfo,				// 推送资讯

	//
	ENTPushTitle,				// 推送新闻标题

	ENTErrMsg,					// 错误消息, 只会是服务端发向客户端
};
 
//////////////////////////////////////////////////////////////////////////
class DATAINFO_DLL_EXPORT CMmiNewsBase
{
public:
	CMmiNewsBase::CMmiNewsBase(){}
	virtual CMmiNewsBase::~CMmiNewsBase(){}

	virtual CString GetSummary() = 0;

public:
	E_NewsType	m_eNewsType;
};

// 请求资讯列表
class DATAINFO_DLL_EXPORT CMmiNewsReqNewsList : public CMmiNewsBase
{
public:
	CMmiNewsReqNewsList::CMmiNewsReqNewsList() {m_eNewsType = ENTReqInfoList;}
	virtual CMmiNewsReqNewsList::~CMmiNewsReqNewsList() {}
	
	virtual CString GetSummary()
	{
		CString StrSummary;
		StrSummary.Format(L"请求咨询列表 Type = %d", m_eNewsType);

		return StrSummary;
	}
};

class DATAINFO_DLL_EXPORT CMmiNewsRespNewsList : public CMmiNewsBase
{
public:
	CMmiNewsRespNewsList::CMmiNewsRespNewsList() {m_eNewsType = ENTRespInfoList;}
	virtual CMmiNewsRespNewsList::~CMmiNewsRespNewsList() {}
	
	virtual CString GetSummary()
	{
		CString StrSummary;
		StrSummary.Format(L"应答咨询列表 Type = %d, 内容: %s", m_eNewsType, m_StrNews);

		return StrSummary;
	}

public:
	CString	m_StrNews;
};

// 请求 F10
class DATAINFO_DLL_EXPORT CMmiNewsReqF10 : public CMmiNewsBase
{
public:
	CMmiNewsReqF10::CMmiNewsReqF10() {m_eNewsType = ENTReqF10;}
	virtual CMmiNewsReqF10::~CMmiNewsReqF10() {}
	
	virtual CString GetSummary()
	{
		CString StrSummary;

		StrSummary.Format(L"请求资讯:F10 Type = %d [%d, %s] InfoIndex: %d BeginPos: %d m_iCount: %d",
			m_eNewsType, m_iMarketId, m_StrMerchCode, m_iIndexInfo, m_iBeginPos, m_iCount);	
		return StrSummary;
	}

public:
	int32		m_iMarketId;
	CString		m_StrMerchCode;

	int32		m_iIndexInfo;
	int32		m_iBeginPos;
	int32		m_iCount;
};

class DATAINFO_DLL_EXPORT CMmiNewsRespF10 : public CMmiNewsBase
{
public:
	CMmiNewsRespF10::CMmiNewsRespF10() {m_eNewsType = ENTRespF10;}
	virtual CMmiNewsRespF10::~CMmiNewsRespF10() {}
	
	virtual CString GetSummary()
	{
		CString StrSummary;

		StrSummary.Format(L"应答资讯:F10 Type = %d F10: %s",
			m_eNewsType, m_StrF10);	
		return StrSummary;
	}

public:
	CString	m_StrF10;
};

// 请求信息地雷
class DATAINFO_DLL_EXPORT CMmiNewsReqLandMine : public CMmiNewsBase
{
public:
	CMmiNewsReqLandMine::CMmiNewsReqLandMine() {m_eNewsType = ENTReqLandMine;}
	virtual CMmiNewsReqLandMine::~CMmiNewsReqLandMine() {}
	
	virtual CString GetSummary(){return L"";}
	
public:
	int32		m_iMarketId;
	CString		m_StrMerchCode;
	
	int32		m_iIndexInfo;	
	int32		m_iCount;
	
	int32		m_iTimeType;		
	CString		m_StrTime0;
	CString		m_StrTime1;

	/*
		TimeType: 时间类型, 可选(信息地雷有)
			  0 Time0开始时间 Time1结束时间
			  1 Time0 开始时间和个数(m_iCount, 此时 time1 无效)
			  2 Time1 结束时间和个数(m_iCount, 此时 time0 无效)	


		// 请求的规则:
		1: 取标题: iIndexInfo 为 EITLandmine. 返回的是标题数据
		2: 取内容: 取某个信息地雷标题对应的内容, iIndexInfo 为这个标题的 iIndexInfo. timetype 和 time0 , time1 字段都 0. 不需关心
	*/
};

// 回包信息地雷
class DATAINFO_DLL_EXPORT CMmiNewsRespLandMine : public CMmiNewsBase
{
public:
	CMmiNewsRespLandMine::CMmiNewsRespLandMine() {m_eNewsType = ENTRespLandMine;}
	virtual CMmiNewsRespLandMine::~CMmiNewsRespLandMine() {}
	
	virtual CString GetSummary(){return L"";}
	
public:
	CString	m_StrLandMine;
};

// 请求代码对应资讯
class DATAINFO_DLL_EXPORT CMmiNewsReqCodeIndex : public CMmiNewsBase
{
public:
	CMmiNewsReqCodeIndex() { m_eNewsType = ENTReqCodeIndex; }
	~CMmiNewsReqCodeIndex(){}
	
	virtual CString GetSummary(){return L"";}

public:
	CString m_StrCode;
};

class DATAINFO_DLL_EXPORT CMmiNewsRespCodeIndex : public CMmiNewsBase
{
public:
	CMmiNewsRespCodeIndex::CMmiNewsRespCodeIndex() {m_eNewsType = ENTRespCodeIndex;}
	virtual CMmiNewsRespCodeIndex::~CMmiNewsRespCodeIndex() {}
	
	virtual CString GetSummary(){return L"";}
	
public:
	CString	m_StrCodeIndex;
};

// 请求新闻标题列表
class DATAINFO_DLL_EXPORT CMmiNewsReqTitleList : public CMmiNewsBase
{
public: 
	CMmiNewsReqTitleList() { m_eNewsType = ENTReqTitleList; m_iInfoIndex = KiIndexNormal; m_iBeginPos = 0; m_iNeedCounts = 20; }
	~CMmiNewsReqTitleList(){}

	virtual CString GetSummary() { return L"请求新闻标题列表"; }

public:
	int32		m_iInfoIndex;	// 第二级的新闻索引 , 3 是普通新闻, 100 是金评
	int32		m_iBeginPos;
	int32		m_iNeedCounts;	// 需要请求多少条标题
};

//
class DATAINFO_DLL_EXPORT CMmiNewsRespTitleList : public CMmiNewsBase
{
public:
	CMmiNewsRespTitleList() { m_eNewsType = ENTRespTitleList; }
	virtual ~CMmiNewsRespTitleList(){}

	virtual CString GetSummary() { return L"应答新闻标题列表"; }

public:
	// mapNewsTitles	m_mapNewsTitles;
	CString			m_StrTitles;
};

//
class DATAINFO_DLL_EXPORT CMmiNewsPushNewsTitle : public CMmiNewsBase
{
public:
	CMmiNewsPushNewsTitle() { m_eNewsType = ENTPushTitle; }
	virtual ~CMmiNewsPushNewsTitle(){}
	
	virtual CString GetSummary() { return L"推送新闻标题"; }
	
public:
	CString			m_StrTitle;
};

// 
class DATAINFO_DLL_EXPORT CMmiNewsReqNewsContent : public CMmiNewsBase
{
public:
	CMmiNewsReqNewsContent() { m_eNewsType = ENTReqNewsContent; }
	virtual CString GetSummary() { return L"请求新闻内容"; }

public:
	int32	m_iIndexID;
};

// 
class DATAINFO_DLL_EXPORT CMmiNewsRespNewsContent : public CMmiNewsBase
{
public:
	CMmiNewsRespNewsContent() { m_eNewsType = ENTRespNewsContent; }
	virtual CString GetSummary() { return L"应答新闻内容"; }
	
public:
	CString	m_StrContent;
};

// 请求推送
class DATAINFO_DLL_EXPORT CMmmiNewsReqAddPush : public CMmiNewsBase
{
public:
	CMmmiNewsReqAddPush() { m_eNewsType = ENTReqAddPush; m_iIndexID = KiIndexNormal; }

	virtual CString GetSummary() { return L"请求推送资讯"; }

public:

	int32	m_iIndexID; // 目前资讯树的根ID 是2, 没特殊需求, 暂时不需要设置这个值
};

// 同步推送
class DATAINFO_DLL_EXPORT CMmmiNewsReqUpdatePush : public CMmiNewsBase
{
public:
	CMmmiNewsReqUpdatePush() { m_eNewsType = ENTReqUpdatePush; m_iIndexID = KiIndexNormal; }
	
	virtual CString GetSummary() { return L"请求推送资讯"; }
	
public:
	
	int32	m_iIndexID; // 目前资讯树的根ID 是2, 没特殊需求, 暂时不需要设置这个值
};

// 删除推送
class DATAINFO_DLL_EXPORT CMmmiNewsReqDelPush : public CMmiNewsBase
{
public:
	CMmmiNewsReqDelPush() { m_eNewsType = ENTReqDeletePush; m_iIndexID = KiIndexNormal; }
	
	virtual CString GetSummary() { return L"请求推送资讯"; }
	
public:
	
	int32	m_iIndexID; // 目前资讯树的根ID 是2, 没特殊需求, 暂时不需要设置这个值
};

// 心跳包
class DATAINFO_DLL_EXPORT CMmiNewsReqHeart : public CMmiNewsBase
{
public:
	CMmiNewsReqHeart::CMmiNewsReqHeart() {m_eNewsType = ENTReqHeart;}
	virtual CMmiNewsReqHeart::~CMmiNewsReqHeart() {}
	
	virtual CString GetSummary()
	{
		CString StrSummary;

		StrSummary.Format(L"请求心跳包: Type = %d ", m_eNewsType);	
		return StrSummary;
	}
};

class DATAINFO_DLL_EXPORT CMmiNewsRespHeart : public CMmiNewsBase
{
public:
	CMmiNewsRespHeart::CMmiNewsRespHeart() {m_eNewsType = ENTRespHeart;}
	virtual CMmiNewsRespHeart::~CMmiNewsRespHeart() {}
	
	virtual CString GetSummary()
	{
		CString StrSummary;

		StrSummary.Format(L"应答心跳包: Type = %d ", m_eNewsType);	
		return StrSummary;
	}
};

// 错误包
class DATAINFO_DLL_EXPORT CMmiNewsRespErrMsg : public CMmiNewsBase
{
public:
	CMmiNewsRespErrMsg::CMmiNewsRespErrMsg() {m_eNewsType = ENTRespHeart;}
	virtual CMmiNewsRespErrMsg::~CMmiNewsRespErrMsg() {}
	
	virtual CString GetSummary()
	{
		CString StrSummary;

		StrSummary.Format(L"收到错误包: Type = %d 错误消息: %s", m_eNewsType, m_StrErrMsg);	
		return StrSummary;
	}

public:
	CString		m_StrErrMsg;
};


#endif _SHARE_STUCT_NEWS_H