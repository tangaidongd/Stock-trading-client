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
		BlockAdded,				// �� �� ɾ - ֻ����ѡ��/����ѡ�ɲŻ�������Щ��Ϣ�ķ��ͣ�xml�ļ�����������initialize
		BlockModified,
		BlockDeleted,
	};

	enum E_InitializeNotifyType{
		BeforeInitialize,		// ׼����ʼ��	-	�������֪ͨ�󣬲�Ӧ�������κ�blockconfig�����ݣ�ֱ��initialized
		Initialized,			// ��ʼ�����   -   ���͸���Ϣ�󣬱�־��ʼ������
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
	typedef CMap<int32, int32, int32, int32> MarketMap;	// keyΪ�г�id, valueΪ���г��е���Ʒ���� >0
	enum E_BlockType
	{
		typeNormalBlock = 0,			// ��ͨ�Ĵ�xml�ļ��м��صİ���־Ϊ0
		typeMarketClassBlock = 1,		// ��xml�ļ��м��صİ���־Ϊ1��Լ��Ϊ������
		typeFutureClassBlock = 2,		// ��xml�ļ��м��ص��ڻ�����־Ϊ2��Լ��Ϊ�ڻ����

		// ���°������Ϊ�Զ��壬��Ҫ�����ļ��ж���İ�����ͳ�ͻ
		typeUserBlock = 10000,		// �û����
		typeHSASubBlock,			// ����a���Ӱ�飬ͨ������xml�ļ�����ɻ���a���ʱ��ȡ
	};

	CBlockInfo();
	CBlockInfo(const CBlockInfo &blockInfo);

	const CBlockInfo &operator=(const CBlockInfo &blockInfo);

	bool32	IsValid() const;

	CMerch	*FindMerchInBlock(int32 iMarketId, const CString &StrMerchCode) const;
	bool32	IsMerchInBlock(CMerch *pMerch) const;

	CMerch  *FindMerchByMerchCode(const CString &StrMerchCode) const;
	CMerch  *FindMerchByCnName(const CString &StrMerchCnName) const;

	bool32	IsMarketClassBlock() const;		// �Ƿ����ڷ����飬�����������־1, ��ͨ���Ϊ0
	E_BlockType GetBlockType() const;

	static	int32	GetInvalidId();
	
	bool32			FillBlock(OUT T_Block &block);

	// ����Ʒ����
	void			Copy(const MerchArray &aMerchs); // ��������������Ʒ����Ϊ����Ʒ
	void			Append(const MerchArray &aMerchs);		// �����Ʒ����
	void			Add(CMerch *pMerch);			 // ���ĳ����Ʒ
	void			SetAt(int iIndex, CMerch *pMerch);	// ����ĳ��λ����Ʒֵ
	CMerch			*RemoveAt(int iIndex);	// �Ƴ�ĳλ����Ʒ
	void			RemoveAll();			// �Ƴ�������Ʒ

	void			RecreateMerchIndexs(bool32 bForce = false);		// �ؽ���Ʒ����
	void			AddMerchIndex(CMerch *pMerch);
	void			RemoveMerchIndex(CMerch *pMerch);

	int32 m_iBlockId;								// ���Id - ���id����ȫ��Ψһ
	int32 m_iOrderId;								// ���丸��鼯���е�˳��id
	CString m_StrBlockName;
	CString	m_StrBlockShortCut;						// ���Ŀ�ݼ�
	int32	m_iType;								// ���ͱ�־ Ŀǰ1Ϊ������ 0Ϊ��ͨ���
	CMerch *m_pMerch;                               // ��鿴����Ʒ
	CArray<CMerch *, CMerch *>		m_aSubMerchs;	//	��������Ʒ���� - ����ֱ�Ӷ������
	IdArray	m_aSubBlockIds;							// ĳЩ����������º�������Ӱ�� - ��Щ��Ҫ���⴦��


	MarketMap						m_mapSubMerchMarkets;	// ��Ʒ�����г�����
	MerchStringMap					m_mapCodeMerchs;		// ������Ʒ����
	MerchStringMap					m_mapNameMerchs;		// ������Ʒ����
};

// ����������ֶ�
class DATACENTER_DLL_EXPORT CBlockDataPseudo
{
public:
	enum E_Header
	{
		ShowRowNo,			// ���
		BlockId,			// ������
		BlockName,			// �������
		RiseRate,           // �Ƿ�
		RiseSpeed,          // ����
		PrincipalPureNum,   // ��������
		PrincipalAmount,    // �������
		AmountRate,         // ����  
		RiseMerchCount,		// �Ǽ���
		FallMerchCount,		// ������
		RiseTopestMerch,    // ���ǹ�
		TotalHand,           // ����   
		TotalAmount,        // �ܽ��
		TotalMarketValue,    // ����ֵ
		CircleMarketValue,   // ��ͨ��ֵ

			
		HeaderCount,

		WeightRiseRate,     // Ȩ�Ƿ�
		CapitalFlow,        // �ʽ�����
		MarketRate,         // �г���%
		ChangeRate,         // ����%
		MarketWinRateDync,  // ��ӯ(��)
		RiseMerchRate,      // �ǹɱ�
		RiseDays,			// ��������
		MerchRise,			// ���ǹ�Ʊ�Ƿ�
	};
	
	
	CBlockDataPseudo();
	CBlockDataPseudo(const T_LogicBlock &logicData);

	typedef CArray<E_Header, E_Header>	HeaderArray;

	// ��ͷ���Ӧֵ�������ȡ
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
	// 	float  m_fMarketWinRateDync;	// ��ӯ��
	// 	int32  m_iRiseTopestMerchMarket;		// ���ǹ�Ʊ
	// 	CString m_StrRiseTopestMerchCode;
	// 	float  m_fRiseMerchsRate;		// �ǹɱ�
	// 	float  m_fCapitalFlow;

	T_LogicBlock	m_blockData;
	typedef map<E_Header, CString>	HeaderNameMap;
	static HeaderNameMap	m_smapHeaderNames;
};


// �����г��İ������
class DATACENTER_DLL_EXPORT CBlockLikeMarket
{
public:
	CBlockLikeMarket(const CBlockCollection &collection);

	CMerch			*FindMerch(int32 iMarketId, const CString &StrMerchCode) const;
	bool32			IsMerchInBlock(CMerch *pMerch) const;

	bool32			IsValidBlock() const;

	bool32			IsValidLogicBlockData() const;

	bool32			IsUserBlock() const;		// �Ƿ��û���ѡ���
	

	T_LogicBlock	m_logicBlockData;	// ��������� - ����ʹ��blockInfo�е�block id
	CBlockInfo		m_blockInfo;		// ����xml�ļ��м��ص���Ϣ
	const CBlockCollection &m_blockCollection;		// �����ĸ���鼯��

	bool32			m_bDeleted;			// �ð���Ƿ񱻱��Ϊɾ�������б��Ϊɾ���İ��Ӧ������
protected:
private:
};


// ��鼯��
class DATACENTER_DLL_EXPORT CBlockCollection
{
public:
	typedef CArray<CBlockLikeMarket *, CBlockLikeMarket *> BlockArray;
	typedef CArray<int32, int32>						   IdArray;

	CBlockCollection();
	~CBlockCollection();

	// ��ֹ���� ��ֵ
	bool32	IsValid() const;

	CBlockLikeMarket			 *InsertBlock(const CBlockInfo &blockInfo);	// �������block, ����д��󣬷���NULL

	void			  RemoveAllBlock();

	CBlockLikeMarket  *FindBlock(int32 iBlockId) const;
	void			  GetBlockByName(const CString &StrBlockName, OUT BlockArray &aBlocks) const;

	void			  GetValidBlocks(OUT BlockArray &aBlocks) const;
	int32			  GetValidBlockCount() const;
	void			  GetValidBlockIdArray(OUT IdArray &aIds) const;

	bool32			  IsMarketClassBlockCollection() const;		// ֻҪ��һ���Ӱ����������ͣ��ü��Ͼͱ��ж�Ϊ�����鼯��
	bool32			  IsUserBlockCollection() const;

	void			  GetBlockByMerch(CMerch *pMerch, OUT BlockArray &aBlocks) const;
	void			  GetBlockByMerch(int32 iMarketid, const CString &StrMerchCode, OUT BlockArray &aBlocks) const;
	
	int32		m_iBlockCollectionId;
	int32		m_iOrderId;					// ˳��id
	CString		m_StrName;
	BlockArray	m_aBlocks;
};


// �ϳ���ѡ��1XXX
// ViewData��ֱ������
// ��ѡ�ɼ��
// ���дӸ�����󷽷��õ���ָ�붼��Ӧ�����ñ��棬��Ϊ�п��������ļ����£���ѡ����ɾ�Ȳ�������ָ��ʧЧ��Ӧ������id
class DATACENTER_DLL_EXPORT CBlockConfig : public CObserverUserBlock
{
public:
	typedef CArray<CBlockCollection *, CBlockCollection *> BlockCollectionArray;
	typedef CBlockCollection::BlockArray				   BlockArray;
	typedef CArray<CMerch *, CMerch *>					   MerchArray;
	typedef	map<int32, CBlockLikeMarket *>				   BlockMap;		// block idȫ��Ψһ
	typedef CArray<int32, int32>						   IdArray;

	

	static const int32 KIUserBlockCollectionId; // = -200;	// ��ѡ��collection id
	static const int32 KIUserBlockCollectionOrderId; // = INT_MAX; // ���������
	static const CString KStrUserBlockCollection; // _T("�Զ�����");
	static const int32 KIMinUserBlockId; // = -4000;  �û���ѡ��idȡֵ�ռ�[-4000,-2000]
	static const int32 KIMaxUserBlockId; // = -2000;
	static int32		KIMaxSpecialSHSZAMarketBlockId;	// = -5000; // ����a�����������г� - ��̬�ɱ�
	static const int32 KISpecialSHSZAMarketBlockCollectionId;	// = -201; // ����a�����������г�

// 	static const int32 KISelectStockCollectionId; // = -199; // ����ѡ��collection id
// 	static const int32 KISelectStockBlockId; // = -199; // ����ѡ��ֻ��һ�����

	static	CBlockConfig *Instance();
	static  CBlockConfig *PureInstance() { return m_spThis; }
	static	void	DeleteInstance();		// ɾ����Դ���������ʱӦ���ͷ���Դ

	static  bool32 GetConfigFullPathName(OUT CString &StrPath);
	static  bool32 RequestLogicBlockFile(CViewData *pViewData);	// CViewData�ڳ�ʼ���г����ʱ������
	static  bool32 RespLogicBlockFile(const CMmiRespLogicBlockFile *pResp, bool32 bNeedInit = false);	// �յ�����ʱ���� - ���Դ�NULL����ֹ�ȴ�
	
	bool32	Initialize(const CString &StrXmlFileName);		// ����ָ�����ļ��������ذ�����ã�������ؼ��ػᷢ��֪ͨ
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
	int32	GetBlockCount(DWORD dwFlag = 0) const;		// ���ձ�־������block�������������־�����壬0Ϊ���У��������ܳ��ֲ����A��ѡ��
	void    GetBlockIdArray(OUT IdArray &aIds, DWORD dwFlag = 0) const;
	void	GetBlockNameArray(OUT CStringArray &aNames, DWORD dwFlag = 0) const;
	CString	GetBlockName(int32 iBlockId) const;

	void				GetBlocksByMerch(CMerch *pMerch, OUT BlockArray &aBlocks) const;
	void				GetBlocksByMerch(int32 iMarket, const CString &StrMerchCode, OUT BlockArray &aBlocks) const;
	int32               GetBlockIdByMerch(int32 iMarket, const CString &StrMerchCode) const;
	// ��ѡ��
	CBlockCollection	*GetUserBlockCollection() const;
	void				GetUserBlocks(OUT BlockArray &aUserBlocks) const;
	bool32				IsUserBlock(int32 iBlockId) const;
	bool32				IsUserCollection(int32 iCollectionId) const;
	bool32				IsUserMerch(CMerch *pMerch) const;

	// ������
	static	int32		GetDefaultMarketlClassBlockPseudoId();		// ��ȡ����A�����(����)�ĵ�id, ���ø�id����config��findblock���ܻ�ȡ��ʵ�ʵĻ���A��
	bool32				GetDefaultMarketClassBlockId(OUT int32 &iBlockId) const;				// ��ȡĬ�ϵķ������е�Ĭ���Ӱ�飬��ʵ���ǻ���A�ɰ��
	CBlockLikeMarket	*GetDefaultMarketClassBlock() const;
	bool32				GetMarketClassCollectionId(OUT int32 &iCollectionId) const;		// ��ȡ�����鼯��id - ��������¸ü���Ӧ��ֻ��һ��, ���ص�һ��
	void				GetMarketClassBlocks(OUT BlockArray &aClassBlocks) const;	// ��ȡ������
	void				GetNormalBlocks(OUT BlockArray &aBlocks) const;				// ��ȡ��ͨ��鼯��

	// ���������� - ���ڶ�λԭ��, ֱ��д������A��������г�
	bool32				LoadSepecialSHSZAMarket(CBlockLikeMarket &blockParent, int32 iMarketId);
	
	void				SetViewData(CViewData* pData);

	// ����ѡ�� - δʵ��
// 	void				SetSelectStockMerch(const MerchArray &aMerchs);
// 	CBlockCollection    *GetSelectStockCollection();
// 	CBlockLikeMarket    *GetSelectStockBlock();

	// ������ݼ��б�
	void			BuildHotKeyList(CArray<CHotKey, CHotKey&>&	arrHotKeys);

	// ��ѡ�ɱ仯
	virtual void	OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType);

	void	AddListener(CBlockConfigListener *pListener);
	void	RemoveListener(CBlockConfigListener *pListener);
	
	void	FireBlockListener(int32 iBlockId, CBlockConfigListener::E_BlockNotifyType eType);
	void	FireConfigListener(CBlockConfigListener::E_InitializeNotifyType eType);
	
	void	RemoveAllBlockCollections();

	CBlockCollection	*InsertBlockCollection(int32 iCollectionId, int32 iOrderId, const CString &StrCollectionName);	// �����²���collection��ָ�룬ʧ��NULL
	CBlockCollection	*InsertBlockCollection(const CBlockCollection &collection);	// ������id�����ֲ��룬�����²���collection��ָ�룬ʧ��NULL

	CBlockLikeMarket    *InsertBlockIntoCollection(CBlockCollection *pCollection, const CBlockInfo &blockInfo, bool32 bAddToCache = true);

	bool32	LoadFromXml(TiXmlDocument &tiDoc);
	bool32	LoadFromUserBlock();
//	void	LoadSelectBlock();		// ��������ѡ��

	static	void	GB2312ToUnicode(const char *pcsGB2312, OUT CString &StrWide);




private:
	void	FillMerchByMaskString(const CString &StrMask, int32 iMarketId, OUT MerchArray &aMerchsFind);	// �������� ����г���Ʒ��������




public:
	CArray<CBlockConfigListener *, CBlockConfigListener *>		m_aListeners;
	BlockCollectionArray				m_aBlockCollections;
	BlockMap							m_mapBlocks;

	CString								m_StrXmlVersion;

	bool32								m_bInitialized;

	int32								m_iNewUserBlockId;		// ��ǰ���¿ɷ�����û����id

	CBlockCollection					m_collectionSHSZA;		// �����黦��A����



	static CBlockConfig *m_spThis;
	static bool32		m_bWaitForServerResp;
protected:
	CBlockConfig();
	~CBlockConfig();
private:
	CViewData*							m_pViewData;
};

#endif