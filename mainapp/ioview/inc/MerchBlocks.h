#ifndef _MERCH_BLOCKS_H_
#define _MERCH_BLOCKS_H_


#include "tinyxml.h"
#include "ReportScheme.h"
#include "facescheme.h"
class TiXmlDocument;
class CMerchBlocks;
class CMerch;

//MerchBlocks.xml解析类

//xml的node的类型
enum EMBXmlNodeType
{
	XNBlock = 0,
		XNMerch
};

//xml的node的基类
class CMBXmlNode
{
public:
	EMBXmlNodeType m_eType;
	void  SetUserData(void* p );
	void* GetUserData ();
protected:
	void* m_pUserData;
	friend class CMerchBlocks;
};

//xml的block node
class CMBXmlBlock : public CMBXmlNode
{
public:
	CMBXmlBlock();
	CString m_StrName;
	CString m_StrHotkey;
};

// 用户板块,信息比一般板块多
class CMBXmlUserBlock: public CMBXmlNode
{
public:
	CMBXmlUserBlock();
	
	CString			m_StrName;
	CString			m_StrHotkey;
	COLORREF		m_ClrUserBlock;					// 每个自选股板块对应一种颜色,用于在正常报价表中显示的时候提示用户. 默认金额颜色
	E_ReportType m_eReportHeadTpye;  // 每个自选股板块对应一种表头.默认自定义表头									 
};

class CMBXmlMerch : public CMBXmlNode
{
public:
	CMBXmlMerch()
	{
		m_pMerch	= NULL;
		m_eType		= XNMerch;
		m_pUserData = NULL;
	}
	
public:
	CMerch			*m_pMerch;
};

//具体进行解析的类,请派生使用.
class CMerchBlocks
{
public:
	CMerchBlocks();
	virtual ~CMerchBlocks();
	//加载MerchBlocks.xml
	bool32 Load ( const char* strFile );
	//释放
	void   Free ( );
	void   Browser ( TiXmlNode* pStartNode = NULL,bool32 bVersion2 = false);
	bool32 GetBlockMerchs ( CString StrBlock, CStringArray& MerchsCode,CArray<int,int>& MerchsMakretId);
	void   Save();
protected:
	virtual bool32 OnBrowserSub ( CMBXmlNode* pNode,void* pParentUserData );
	virtual bool32 OnBrowserSub2 ( TiXmlNode* pRawNode,CMBXmlNode* pNode,void* pParentUserData );
protected:
	void   DoNode ( TiXmlElement* pNode );
	void   DoBrowser ( TiXmlNode* pStartNode);
	TiXmlDocument* m_pXmlDocument;
	bool32		   m_bContinueBrowser;
	bool32		   m_bVersion2;
};


#endif // _MERCH_BLOCKS_H_
