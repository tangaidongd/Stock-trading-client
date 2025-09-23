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
// �۲���ģʽ, ��ѡ�ɵ��κ��޸ı䶯, ��֪ͨ����Ӧ��ģ�����

class CObserverUserBlock;

// ������, ����֪ͨ�����̳�
class DATACENTER_DLL_EXPORT CSubjectUserBlock
{
public:
	enum E_UserBlockUpdate 
	{
		EUBUNone	= 0,				// û�и���
		EUBUMerch,						// ��������Ʒ
		EUBUBlock,						// �����˰�����Ϣ		
		EUBUIndex,						// ������ָ����Ϣ
	
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

// �۲�����
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

// ���
typedef struct T_Block
{
	CString								m_StrName;					// ����
	CString								m_StrNameOld;				// ǰһ������
	CString								m_StrHotKey;				// ��ݼ�
	COLORREF							m_clrBlock;					// ������ɫ
		
	bool32								m_bChangeName;				// �����Ƿ����
	E_ReportType			m_eHeadType;				// ��ͷ����
	CArray<CMerch*, CMerch*>			m_aMerchs;					// �����������������Ʒ

	TiXmlElement*						m_pXmlElement;				// ��Ӧ�� XML �ڵ�ָ��
	std::map<CMerch*, TiXmlElement*>    m_aMapMerchToXml;			// ��Ʒ��Ӧ�� xml �ڵ�ָ��

	bool32								m_bServerBlock;				// xl 1109 �Ƿ�Ϊ������ͬ�����

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

	// ��������
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

	// ��ֵ
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

// ������Ļ���:
class DATACENTER_DLL_EXPORT CBlockManager
{
public:
	CBlockManager();													// Ĭ�Ϲ���
	virtual ~CBlockManager();												// ��������				

	// ���ݰ�����õ����	
	T_Block*							GetBlock(IN const CString& StrBlockName);

	// ������Ʒ�õ����
	T_Block*							GetBlock(IN CMerch* pMerch);

	// ȡ�����еİ��
	void								GetBlocks(OUT CArray<T_Block, T_Block&>& aBlocks);

	// ȡ�����е���Ʒ
	void								GetMerchs(OUT CArray<CMerch*, CMerch*>& aMerchs);

	// ȡ��ĳ������µ�������Ʒ
	bool32								GetMerchsInBlock(IN const CString& StrBlockName, OUT CArray<CMerch*, CMerch*>& aMerchs);

	// �ж�ĳ����Ʒ�Ƿ�����ڰ����
	bool32								BeMerchInBlock(IN const CMerch* pMerch, IN const CString& StrBlockName);


	void								Construct(CString StrPath);			// ����
	bool32								LoadXmlFile(CMerchManager* pManager);	// װ���ļ�
	bool32								SaveXmlFile();						// �����ļ�
	virtual bool32						AutoCreateXmlFile() = 0;			// װ��ʧ�ܵ�ʱ��,�Զ�����

protected:
	CString								m_StrPath;							// �ļ�·��
	TiXmlDocument*						m_pXmlDoc;							// XML �ĵ�����

	CArray<CMerch*, CMerch*>			m_aMerchsAll;						// ����ļ��������������Ʒ
	CArray<T_Block, T_Block&>			m_aBlocksAll;						// ����ļ�����������а��	

	CMerchManager*						m_pMerchManager;
};

// ϵͳ���
class DATACENTER_DLL_EXPORT CSysBlockManager : public CBlockManager
{
private:
	CSysBlockManager(){}

public:
	~CSysBlockManager(){}

	static CSysBlockManager*			Instance();					// ����ģʽ
	static void DelInstance();

	virtual bool32						AutoCreateXmlFile();		// װ��ʧ�ܵ�ʱ��,�Զ�����

	bool								Initialize(CMerchManager* pManager);

private:
	static CSysBlockManager*			m_pThis;					// ��ָ̬��

};

class CHotKey;

// �û����,��ѡ��
class DATACENTER_DLL_EXPORT CUserBlockManager : public CBlockManager, public CSubjectUserBlock
{
private:
	CUserBlockManager(){}

public:
	~CUserBlockManager(){}

	static CUserBlockManager*			Instance();					// ����ģʽ
	static void DelInstance();
	//
	virtual bool32						AutoCreateXmlFile();		// װ��ʧ�ܵ�ʱ��,�Զ�����

	// ������Ʒ���û����
	bool32								AddMerchToUserBlock(IN CMerch* pMerch, IN const CString& StrBlockName, bool32 bAutoSaveAndNotify = true);				

	// ���û����ɾ����Ʒ
	bool32								DelMerchFromUserBlock(IN CMerch* pMerch, IN const CString& StrBlockName, bool32 bAutoSaveAndNotify = true);				

	// �滻�����û�������Ʒ(�Ƿ���ԭ����)
	bool32								ReplaceUserBlockMerchs(IN const CArray<CMerch*, CMerch*>& aMerchsNew, IN const CString& StrBlockName, bool32 bSaveBefore = false);

	// �����ѡ��
	bool32								AddUserBlock(IN T_Block& BlockInfoNew);				
	
	// �޸���ѡ����Ϣ
	bool32								ModifyUserBlock(IN const CString& StrBlockNameOld, IN const T_Block& BlockInfoNew);				

	// ɾ����ѡ��
	bool32								DelUserBlock(IN const CString& StrBlockName);

	// ɾ��������ѡ��
	bool32								DelAllUserBlock(IN const CString& StrBlockName, bool32 bAutoSaveAndNotify = false);	
	
	// �ƶ���ѡ�ɵ�λ��
	bool32								ChangeOwnMerchPosition(bool32 bPre, IN const CString& StrBlockName, IN CMerch* pMerch);

	// ����϶����ƶ���ѡ�ɵ�λ��
	bool32								ChangeOwnMerchPosition(IN const CString& StrBlockName, bool32 bPre, IN CMerch* pPreMerch,  IN CMerch* pNextMerch);


	bool								Initialize(CString strUserName,CMerchManager* pManager);

	void								BuildUserBlockHotKeyList(CArray<CHotKey, CHotKey&>&	arrHotKeys);
	void                                RemoveHotKey(E_HotKeyType eHotKeyType, CArray<CHotKey, CHotKey&>&	arrHotKeys);

	// �������������ѡ�ɽ��� - ��ʱ��û��ʵ�֣��ȴ�ȷ�����ֻ�����ͬ���ķ�ʽ
	CString				GetServerBlockName();		// ��ȡ������ͬ���İ���� -�ҵ���ѡ �ð�鸽�Ӻܶ಻ͬ������ ����ɾ�����
	static CString		GetDefaultServerBlockName();	// ����Ĭ�ϵ����� - �ҵ���ѡ
	bool32				IsServerBlock(const T_Block &Block);	// �Ƿ�Ĭ�ϰ��
	T_Block				*GetServerBlock();			// ��ȡ������ͬ�����

private:
	


private:
	static CUserBlockManager*			m_pThis;					// ��ָ̬��
};

#endif // _BLOCK_MANAGER_H_
