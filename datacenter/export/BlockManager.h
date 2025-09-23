#ifndef _BLOCK_MANAGER_H_
#define _BLOCK_MANAGER_H_

#pragma warning(disable: 4786)

#if _MSC_VER > 1000
#pragma once
#endif 

#include <vector>
#include <list>
#include <afxtempl.h>


#include "ReportScheme.h"
#include "MerchManager.h"
#include "DataCenterExport.h"
#include "hotkey.h"

//////////////////////////////////////////////////////////////////////////
// 观察者模式, 自选股的任何修改变动, 都通知给相应的模块更新

class CObserverUserBlock;

// 主题类, 用于通知者来继承
class DATACENTER_DLL_EXPORT CSubjectUserBlock
{
public:
	enum E_UserBlockUpdate 
	{
		EUBUNone	= 0,				// 没有更新
		EUBUMerch,						// 更新了商品
		EUBUBlock,						// 更新了板块的信息		
		EUBUIndex,						// 更新了指标信息
	
		EUBUCount
	};
public:
	CSubjectUserBlock();
	~CSubjectUserBlock();
	
	void		AddObserver(CObserverUserBlock* pObserver);
	void		DelObserver(CObserverUserBlock* pObserver);
	
	void		Notify(CSubjectUserBlock::E_UserBlockUpdate eUpdateType);
	
protected:
	CArray<CObserverUserBlock*, CObserverUserBlock*> m_aObservers;
};

// 观察者类
class DATACENTER_DLL_EXPORT CObserverUserBlock
{
public:
	CObserverUserBlock();
	virtual ~CObserverUserBlock();
	virtual void	OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType){}
	
protected:
	CSubjectUserBlock* m_pSubject;
};

//////////////////////////////////////////////////////////////////////////
class TiXmlElement;
class TiXmlDocument;

// 板块
typedef struct T_Block
{
	CString								m_StrName;					// 名称
	CString								m_StrNameOld;				// 前一个名称
	CString								m_StrHotKey;				// 快捷键
	COLORREF							m_clrBlock;					// 板块的颜色
		
	bool32								m_bChangeName;				// 名字是否变了
	E_ReportType			m_eHeadType;				// 表头类型
	CArray<CMerch*, CMerch*>			m_aMerchs;					// 这个板块下面包含的商品

	TiXmlElement*						m_pXmlElement;				// 对应的 XML 节点指针
	std::map<CMerch*, TiXmlElement*>    m_aMapMerchToXml;			// 商品对应的 xml 节点指针

	bool32								m_bServerBlock;				// xl 1109 是否为服务器同步板块

	T_Block()
	{
		m_StrName		= L"";
		m_StrNameOld	= L"";
		m_StrHotKey		= L"";
		m_clrBlock		= 16776960;
		m_bChangeName	= false;
		m_eHeadType		= ERTFuturesCn;
		m_pXmlElement	= NULL;
		m_aMerchs.RemoveAll();
		m_aMapMerchToXml.clear();

		m_bServerBlock = false;
	}

	// 拷贝构造
	T_Block(const T_Block& stBlock)
	{
		m_aMerchs.RemoveAll();
		m_aMerchs.Copy(stBlock.m_aMerchs);
		
		m_StrName		= stBlock.m_StrName;
		m_StrNameOld	= stBlock.m_StrNameOld;
		m_StrHotKey		= stBlock.m_StrHotKey;
		m_clrBlock		= stBlock.m_clrBlock;
		
		m_bChangeName	= stBlock.m_bChangeName;

		m_eHeadType		= stBlock.m_eHeadType;		
		m_pXmlElement	= stBlock.m_pXmlElement;
		m_aMapMerchToXml= stBlock.m_aMapMerchToXml;

		m_bServerBlock	= stBlock.m_bServerBlock;
	}

	// 赋值
	T_Block& operator= (const T_Block& stBlock)
	{
		if ( this == &stBlock )
		{
			return *this;
		}

		m_aMerchs.RemoveAll();
		m_aMerchs.Copy(stBlock.m_aMerchs);

		m_StrName		= stBlock.m_StrName;
		m_StrNameOld	= stBlock.m_StrNameOld;
		m_StrHotKey		= stBlock.m_StrHotKey;
		m_clrBlock		= stBlock.m_clrBlock;

		m_bChangeName	= stBlock.m_bChangeName;

		m_eHeadType		= stBlock.m_eHeadType;		
		m_pXmlElement	= stBlock.m_pXmlElement;
		m_aMapMerchToXml= stBlock.m_aMapMerchToXml;

		m_bServerBlock	= stBlock.m_bServerBlock;

		return *this;
	}

}T_Block;

// 板块管理的基类:
class DATACENTER_DLL_EXPORT CBlockManager
{
public:
	CBlockManager();													// 默认构造
	virtual ~CBlockManager();												// 析构函数				

	// 根据板块名得到板块	
	T_Block*							GetBlock(IN const CString& StrBlockName);

	// 根据商品得到板块
	T_Block*							GetBlock(IN CMerch* pMerch);

	// 取得所有的板块
	void								GetBlocks(OUT CArray<T_Block, T_Block&>& aBlocks);

	// 取得所有的商品
	void								GetMerchs(OUT CArray<CMerch*, CMerch*>& aMerchs);

	// 取得某个板块下的所有商品
	bool32								GetMerchsInBlock(IN const CString& StrBlockName, OUT CArray<CMerch*, CMerch*>& aMerchs);

	// 判断某个商品是否存在于板块中
	bool32								BeMerchInBlock(IN const CMerch* pMerch, IN const CString& StrBlockName);


	void								Construct(CString StrPath);			// 构造
	bool32								LoadXmlFile(CMerchManager* pManager);	// 装载文件
	bool32								SaveXmlFile();						// 保存文件
	virtual bool32						AutoCreateXmlFile() = 0;			// 装载失败的时候,自动生成

protected:
	CString								m_StrPath;							// 文件路径
	TiXmlDocument*						m_pXmlDoc;							// XML 文档对象

	CArray<CMerch*, CMerch*>			m_aMerchsAll;						// 这个文件里包含的所有商品
	CArray<T_Block, T_Block&>			m_aBlocksAll;						// 这个文件里包含的所有板块	

	CMerchManager*						m_pMerchManager;
};

// 系统板块
class DATACENTER_DLL_EXPORT CSysBlockManager : public CBlockManager
{
private:
	CSysBlockManager(){}

public:
	~CSysBlockManager(){}

	static CSysBlockManager*			Instance();					// 单例模式
	static void DelInstance();

	virtual bool32						AutoCreateXmlFile();		// 装载失败的时候,自动生成

	bool								Initialize(CMerchManager* pManager);

private:
	static CSysBlockManager*			m_pThis;					// 静态指针

};

class CHotKey;

// 用户板块,自选股
class DATACENTER_DLL_EXPORT CUserBlockManager : public CBlockManager, public CSubjectUserBlock
{
private:
	CUserBlockManager(){}

public:
	~CUserBlockManager(){}

	static CUserBlockManager*			Instance();					// 单例模式
	static void DelInstance();
	//
	virtual bool32						AutoCreateXmlFile();		// 装载失败的时候,自动生成

	// 加入商品到用户板块
	bool32								AddMerchToUserBlock(IN CMerch* pMerch, IN const CString& StrBlockName, bool32 bAutoSaveAndNotify = true);				

	// 从用户板块删除商品
	bool32								DelMerchFromUserBlock(IN CMerch* pMerch, IN const CString& StrBlockName, bool32 bAutoSaveAndNotify = true);				

	// 替换所有用户板块的商品(是否保留原来的)
	bool32								ReplaceUserBlockMerchs(IN const CArray<CMerch*, CMerch*>& aMerchsNew, IN const CString& StrBlockName, bool32 bSaveBefore = false);

	// 添加自选股
	bool32								AddUserBlock(IN T_Block& BlockInfoNew);				
	
	// 修改自选股信息
	bool32								ModifyUserBlock(IN const CString& StrBlockNameOld, IN const T_Block& BlockInfoNew);				

	// 删除自选股
	bool32								DelUserBlock(IN const CString& StrBlockName);

	// 删除所有自选股
	bool32								DelAllUserBlock(IN const CString& StrBlockName, bool32 bAutoSaveAndNotify = false);	
	
	// 移动自选股的位置
	bool32								ChangeOwnMerchPosition(bool32 bPre, IN const CString& StrBlockName, IN CMerch* pMerch);

	// 鼠标拖动来移动自选股的位置
	bool32								ChangeOwnMerchPosition(IN const CString& StrBlockName, bool32 bPre, IN CMerch* pPreMerch,  IN CMerch* pNextMerch);


	bool								Initialize(CString strUserName,CMerchManager* pManager);

	void								BuildUserBlockHotKeyList(CArray<CHotKey, CHotKey&>&	arrHotKeys);
	void                                RemoveHotKey(E_HotKeyType eHotKeyType, CArray<CHotKey, CHotKey&>&	arrHotKeys);

	// 用于与服务器自选股交互 - 暂时都没有实现，等待确认与手机保持同步的方式
	CString				GetServerBlockName();		// 获取服务器同步的板块名 -我的自选 该板块附加很多不同的特征 不能删除板块
	static CString		GetDefaultServerBlockName();	// 返回默认的名字 - 我的自选
	bool32				IsServerBlock(const T_Block &Block);	// 是否默认板块
	T_Block				*GetServerBlock();			// 获取服务器同步板块

private:
	


private:
	static CUserBlockManager*			m_pThis;					// 静态指针
};

#endif // _BLOCK_MANAGER_H_
