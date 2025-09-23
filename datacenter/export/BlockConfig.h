#ifndef _BLOCK_CONFIG_H_
#define _BLOCK_CONFIG_H_

#include <map>
using std::map;

#include "sharestruct.h"
#include "PlugInStruct.h"
#include "BlockManager.h"
#include "DataCenterExport.h"

class TiXmlDocument;
class CViewData;
class DATACENTER_DLL_EXPORT CBlockConfigListener
{
public:
	virtual ~CBlockConfigListener();

	enum E_BlockNotifyType{
		BlockAdded,				// 增 修 删 - 只有自选股/条件选股才会引发这些消息的发送，xml文件更新引发的initialize
		BlockModified,
		BlockDeleted,
	};

	enum E_InitializeNotifyType{
		BeforeInitialize,		// 准备初始化	-	发送这个通知后，不应该依赖任何blockconfig的数据，直至initialized
		Initialized,			// 初始化完毕   -   发送该消息后，标志初始化结束
	};

	virtual void  OnBlockConfigChange(int32 iBlockId, E_BlockNotifyType eNotifyType) = 0;
	virtual void  OnConfigInitialize(E_InitializeNotifyType eInitializeType) = 0;
protected:
private:
};

class CBlockConfig;
class CBlockLikeMarket;
class CBlockCollection;
class DATACENTER_DLL_EXPORT CBlockInfo 
{
public:
	typedef	CArray<CMerch *, CMerch *>	MerchArray;
	typedef CArray<int32, int32>		IdArray;
	typedef map<CString, CMerch *>		MerchStringMap;
	typedef CMap<int32, int32, int32, int32> MarketMap;	// key为市场id, value为该市场中的商品数量 >0
	enum E_BlockType
	{
		typeNormalBlock = 0,			// 普通的从xml文件中加载的板块标志为0
		typeMarketClassBlock = 1,		// 从xml文件中加载的板块标志为1，约定为分类板块
		typeFutureClassBlock = 2,		// 从xml文件中加载的期货板块标志为2，约定为期货板块

		// 以下板块类型为自定义，不要与板块文件中定义的板块类型冲突
		typeUserBlock = 10000,		// 用户板块
		typeHSASubBlock,			// 沪深a下子板块，通过解析xml文件，组成沪深a板块时获取
	};

	CBlockInfo();
	CBlockInfo(const CBlockInfo &blockInfo);

	const CBlockInfo &operator=(const CBlockInfo &blockInfo);

	bool32	IsValid() const;

	CMerch	*FindMerchInBlock(int32 iMarketId, const CString &StrMerchCode) const;
	bool32	IsMerchInBlock(CMerch *pMerch) const;

	CMerch  *FindMerchByMerchCode(const CString &StrMerchCode) const;
	CMerch  *FindMerchByCnName(const CString &StrMerchCnName) const;

	bool32	IsMarketClassBlock() const;		// 是否属于分类板块，分类板块特殊标志1, 普通板块为0
	E_BlockType GetBlockType() const;

	static	int32	GetInvalidId();
	
	bool32			FillBlock(OUT T_Block &block);

	// 子商品操作
	void			Copy(const MerchArray &aMerchs); // 拷贝整个输入商品数组为子商品
	void			Append(const MerchArray &aMerchs);		// 添加商品数组
	void			Add(CMerch *pMerch);			 // 添加某个商品
	void			SetAt(int iIndex, CMerch *pMerch);	// 设置某个位置商品值
	CMerch			*RemoveAt(int iIndex);	// 移出某位置商品
	void			RemoveAll();			// 移出所有商品

	void			RecreateMerchIndexs(bool32 bForce = false);		// 重建商品索引
	void			AddMerchIndex(CMerch *pMerch);
	void			RemoveMerchIndex(CMerch *pMerch);

	int32 m_iBlockId;								// 板块Id - 板块id保持全局唯一
	int32 m_iOrderId;								// 在其父板块集合中的顺序id
	CString m_StrBlockName;
	CString	m_StrBlockShortCut;						// 板块的快捷键
	int32	m_iType;								// 类型标志 目前1为分类板块 0为普通板块
	CMerch *m_pMerch;                               // 板块看成商品
	CArray<CMerch *, CMerch *>		m_aSubMerchs;	//	包含的商品集合 - 不能直接对其操作
	IdArray	m_aSubBlockIds;							// 某些特殊板块可能下含特殊的子板块 - 这些需要特殊处理


	MarketMap						m_mapSubMerchMarkets;	// 商品所属市场集合
	MerchStringMap					m_mapCodeMerchs;		// 代码商品索引
	MerchStringMap					m_mapNameMerchs;		// 名称商品索引
};

// 板块表格数据字段
class DATACENTER_DLL_EXPORT CBlockDataPseudo
{
public:
	enum E_Header
	{
		ShowRowNo,			// 序号
		BlockId,			// 板块代码
		BlockName,			// 板块名称
		RiseRate,           // 涨幅
		RiseSpeed,          // 涨速
		PrincipalPureNum,   // 主力净量
		PrincipalAmount,    // 主力金额
		AmountRate,         // 量比  
		RiseMerchCount,		// 涨家数
		FallMerchCount,		// 跌家数
		RiseTopestMerch,    // 领涨股
		TotalHand,           // 总手   
		TotalAmount,        // 总金额
		TotalMarketValue,    // 总市值
		CircleMarketValue,   // 流通市值

			
		HeaderCount,

		WeightRiseRate,     // 权涨幅
		CapitalFlow,        // 资金流向
		MarketRate,         // 市场比%
		ChangeRate,         // 换手%
		MarketWinRateDync,  // 市盈(动)
		RiseMerchRate,      // 涨股比
		RiseDays,			// 连涨天数
		MerchRise,			// 领涨股票涨幅
	};
	
	
	CBlockDataPseudo();
	CBlockDataPseudo(const T_LogicBlock &logicData);

	typedef CArray<E_Header, E_Header>	HeaderArray;

	// 表头与对应值在这里获取
	bool32 GetColValue(E_Header eHeader, IN CViewData *pViewData, OUT void *pValue) const;
	static bool32 GetColName(E_Header eHeader, OUT CString &StrName);
	static void   GetDefaultColArray(OUT HeaderArray &aHeaders);
	static int32  GetColNames(IN const HeaderArray &aHeaders, OUT CStringArray &aStrNames);
	static void	  GetDefaultColNames(OUT CStringArray &aStrNames);

	static bool32 ConvertNativeHeaderToSortHeader(IN E_Header eNativeHeader, OUT E_BlockSort &eSortHeader );
	
	static	void	InitializeHeaderNameMap();
	

public:
	// 	float  m_fRiseRate;
	// 	float  m_fWeightRiseRate;
	// 	float  m_fAmount;
	// 	float  m_fMarketRate;
	// 	float  m_fChangeRate;
	// 	float  m_fMarketWinRateDync;	// 市盈动
	// 	int32  m_iRiseTopestMerchMarket;		// 领涨股票
	// 	CString m_StrRiseTopestMerchCode;
	// 	float  m_fRiseMerchsRate;		// 涨股比
	// 	float  m_fCapitalFlow;

	T_LogicBlock	m_blockData;
	typedef map<E_Header, CString>	HeaderNameMap;
	static HeaderNameMap	m_smapHeaderNames;
};


// 类似市场的板块数据
class DATACENTER_DLL_EXPORT CBlockLikeMarket
{
public:
	CBlockLikeMarket(const CBlockCollection &collection);

	CMerch			*FindMerch(int32 iMarketId, const CString &StrMerchCode) const;
	bool32			IsMerchInBlock(CMerch *pMerch) const;

	bool32			IsValidBlock() const;

	bool32			IsValidLogicBlockData() const;

	bool32			IsUserBlock() const;		// 是否用户自选板块
	

	T_LogicBlock	m_logicBlockData;	// 板块表格数据 - 尽量使用blockInfo中的block id
	CBlockInfo		m_blockInfo;		// 板块从xml文件中加载的信息
	const CBlockCollection &m_blockCollection;		// 隶属的父板块集合

	bool32			m_bDeleted;			// 该板块是否被标记为删除，所有标记为删除的板块应当废弃
protected:
private:
};


// 板块集合
class DATACENTER_DLL_EXPORT CBlockCollection
{
public:
	typedef CArray<CBlockLikeMarket *, CBlockLikeMarket *> BlockArray;
	typedef CArray<int32, int32>						   IdArray;

	CBlockCollection();
	~CBlockCollection();

	// 禁止拷贝 赋值
	bool32	IsValid() const;

	CBlockLikeMarket			 *InsertBlock(const CBlockInfo &blockInfo);	// 按序插入block, 如果有错误，返回NULL

	void			  RemoveAllBlock();

	CBlockLikeMarket  *FindBlock(int32 iBlockId) const;
	void			  GetBlockByName(const CString &StrBlockName, OUT BlockArray &aBlocks) const;

	void			  GetValidBlocks(OUT BlockArray &aBlocks) const;
	int32			  GetValidBlockCount() const;
	void			  GetValidBlockIdArray(OUT IdArray &aIds) const;

	bool32			  IsMarketClassBlockCollection() const;		// 只要有一个子板块是这个类型，该集合就被判定为分类板块集合
	bool32			  IsUserBlockCollection() const;

	void			  GetBlockByMerch(CMerch *pMerch, OUT BlockArray &aBlocks) const;
	void			  GetBlockByMerch(int32 iMarketid, const CString &StrMerchCode, OUT BlockArray &aBlocks) const;
	
	int32		m_iBlockCollectionId;
	int32		m_iOrderId;					// 顺序id
	CString		m_StrName;
	BlockArray	m_aBlocks;
};


// 合成自选股1XXX
// ViewData中直接侦听
// 自选股监控
// 所有从该类对象方法得到的指针都不应当长久保存，因为有可能由于文件更新，自选股增删等操作导致指针失效，应当保存id
class DATACENTER_DLL_EXPORT CBlockConfig : public CObserverUserBlock
{
public:
	typedef CArray<CBlockCollection *, CBlockCollection *> BlockCollectionArray;
	typedef CBlockCollection::BlockArray				   BlockArray;
	typedef CArray<CMerch *, CMerch *>					   MerchArray;
	typedef	map<int32, CBlockLikeMarket *>				   BlockMap;		// block id全局唯一
	typedef CArray<int32, int32>						   IdArray;

	

	static const int32 KIUserBlockCollectionId; // = -200;	// 自选股collection id
	static const int32 KIUserBlockCollectionOrderId; // = INT_MAX; // 排在最后面
	static const CString KStrUserBlockCollection; // _T("自定义板块");
	static const int32 KIMinUserBlockId; // = -4000;  用户自选股id取值空间[-4000,-2000]
	static const int32 KIMaxUserBlockId; // = -2000;
	static int32		KIMaxSpecialSHSZAMarketBlockId;	// = -5000; // 沪深a下面子物理市场 - 动态可变
	static const int32 KISpecialSHSZAMarketBlockCollectionId;	// = -201; // 沪深a下面子物理市场

// 	static const int32 KISelectStockCollectionId; // = -199; // 条件选股collection id
// 	static const int32 KISelectStockBlockId; // = -199; // 条件选股只有一个板块

	static	CBlockConfig *Instance();
	static  CBlockConfig *PureInstance() { return m_spThis; }
	static	void	DeleteInstance();		// 删除资源，程序结束时应当释放资源

	static  bool32 GetConfigFullPathName(OUT CString &StrPath);
	static  bool32 RequestLogicBlockFile(CViewData *pViewData);	// CViewData在初始化市场完成时，调用
	static  bool32 RespLogicBlockFile(const CMmiRespLogicBlockFile *pResp, bool32 bNeedInit = false);	// 收到数据时调用 - 可以传NULL，终止等待
	
	bool32	Initialize(const CString &StrXmlFileName);		// 根据指定的文件名，加载板块配置，如果是重加载会发出通知
	bool32  IsInitialized() const;

	int32   GetCollectionCount() const;
	void	GetCollectionIdArray(OUT IdArray &aIds) const;
	bool32	GetCollectionName(int32 iCollectionId, OUT CString &StrCollectionName) const;
	void	GetBlockCollectionNames(OUT CStringArray &aNames) const;
	CBlockCollection	*GetBlockCollectionByName(const CString &StrCollectionName) const;
	CBlockCollection	*GetBlockCollectionById(int32 iBlockCollectionId) const ;
	void				GetBlocksByName(const CString &StrBlockName, OUT BlockArray &aBlocks) const;
	CBlockLikeMarket    *FindBlock(int32 iBlockId) const;
	CBlockLikeMarket    *FindBlock(const CString &StrBlockName) const;
	int32	GetBlockCount(DWORD dwFlag = 0) const;		// 按照标志，返回block的数量，具体标志待定义，0为所有，将来可能出现不含深沪A等选项
	void    GetBlockIdArray(OUT IdArray &aIds, DWORD dwFlag = 0) const;
	void	GetBlockNameArray(OUT CStringArray &aNames, DWORD dwFlag = 0) const;
	CString	GetBlockName(int32 iBlockId) const;

	void				GetBlocksByMerch(CMerch *pMerch, OUT BlockArray &aBlocks) const;
	void				GetBlocksByMerch(int32 iMarket, const CString &StrMerchCode, OUT BlockArray &aBlocks) const;
	int32               GetBlockIdByMerch(int32 iMarket, const CString &StrMerchCode) const;
	// 自选股
	CBlockCollection	*GetUserBlockCollection() const;
	void				GetUserBlocks(OUT BlockArray &aUserBlocks) const;
	bool32				IsUserBlock(int32 iBlockId) const;
	bool32				IsUserCollection(int32 iCollectionId) const;
	bool32				IsUserMerch(CMerch *pMerch) const;

	// 分类板块
	static	int32		GetDefaultMarketlClassBlockPseudoId();		// 获取沪深A股虚假(别名)的的id, 利用该id仅在config的findblock中能获取到实际的沪深A股
	bool32				GetDefaultMarketClassBlockId(OUT int32 &iBlockId) const;				// 获取默认的分类板块中的默认子板块，其实就是沪深A股板块
	CBlockLikeMarket	*GetDefaultMarketClassBlock() const;
	bool32				GetMarketClassCollectionId(OUT int32 &iCollectionId) const;		// 获取分类板块集合id - 正常情况下该集合应该只有一个, 返回第一个
	void				GetMarketClassBlocks(OUT BlockArray &aClassBlocks) const;	// 获取分类板块
	void				GetNormalBlocks(OUT BlockArray &aBlocks) const;				// 获取普通板块集合

	// 特殊物理板块 - 由于定位原因, 直接写死沪深A股下面的市场
	bool32				LoadSepecialSHSZAMarket(CBlockLikeMarket &blockParent, int32 iMarketId);
	
	void				SetViewData(CViewData* pData);

	// 条件选股 - 未实现
// 	void				SetSelectStockMerch(const MerchArray &aMerchs);
// 	CBlockCollection    *GetSelectStockCollection();
// 	CBlockLikeMarket    *GetSelectStockBlock();

	// 建立快捷键列表
	void			BuildHotKeyList(CArray<CHotKey, CHotKey&>&	arrHotKeys);

	// 自选股变化
	virtual void	OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType);

	void	AddListener(CBlockConfigListener *pListener);
	void	RemoveListener(CBlockConfigListener *pListener);
	
	void	FireBlockListener(int32 iBlockId, CBlockConfigListener::E_BlockNotifyType eType);
	void	FireConfigListener(CBlockConfigListener::E_InitializeNotifyType eType);
	
	void	RemoveAllBlockCollections();

	CBlockCollection	*InsertBlockCollection(int32 iCollectionId, int32 iOrderId, const CString &StrCollectionName);	// 返回新插入collection的指针，失败NULL
	CBlockCollection	*InsertBlockCollection(const CBlockCollection &collection);	// 仅按照id，名字插入，返回新插入collection的指针，失败NULL

	CBlockLikeMarket    *InsertBlockIntoCollection(CBlockCollection *pCollection, const CBlockInfo &blockInfo, bool32 bAddToCache = true);

	bool32	LoadFromXml(TiXmlDocument &tiDoc);
	bool32	LoadFromUserBlock();
//	void	LoadSelectBlock();		// 加载条件选股

	static	void	GB2312ToUnicode(const char *pcsGB2312, OUT CString &StrWide);




private:
	void	FillMerchByMaskString(const CString &StrMask, int32 iMarketId, OUT MerchArray &aMerchsFind);	// 根据掩码 填充市场商品于数组中




public:
	CArray<CBlockConfigListener *, CBlockConfigListener *>		m_aListeners;
	BlockCollectionArray				m_aBlockCollections;
	BlockMap							m_mapBlocks;

	CString								m_StrXmlVersion;

	bool32								m_bInitialized;

	int32								m_iNewUserBlockId;		// 当前最新可分配的用户板块id

	CBlockCollection					m_collectionSHSZA;		// 特殊板块沪深A集合



	static CBlockConfig *m_spThis;
	static bool32		m_bWaitForServerResp;
protected:
	CBlockConfig();
	~CBlockConfig();
private:
	CViewData*							m_pViewData;
};

#endif