#ifndef _MERCH_BLOCKS_H_
#define _MERCH_BLOCKS_H_


#include "tinyxml.h"
#include "ReportScheme.h"
#include "facescheme.h"
class TiXmlDocument;
class CMerchBlocks;
class CMerch;

//MerchBlocks.xml������

//xml��node������
enum EMBXmlNodeType
{
	XNBlock = 0,
		XNMerch
};

//xml��node�Ļ���
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

//xml��block node
class CMBXmlBlock : public CMBXmlNode
{
public:
	CMBXmlBlock();
	CString m_StrName;
	CString m_StrHotkey;
};

// �û����,��Ϣ��һ�����
class CMBXmlUserBlock: public CMBXmlNode
{
public:
	CMBXmlUserBlock();
	
	CString			m_StrName;
	CString			m_StrHotkey;
	COLORREF		m_ClrUserBlock;					// ÿ����ѡ�ɰ���Ӧһ����ɫ,�������������۱�����ʾ��ʱ����ʾ�û�. Ĭ�Ͻ����ɫ
	E_ReportType m_eReportHeadTpye;  // ÿ����ѡ�ɰ���Ӧһ�ֱ�ͷ.Ĭ���Զ����ͷ									 
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

//������н�������,������ʹ��.
class CMerchBlocks
{
public:
	CMerchBlocks();
	virtual ~CMerchBlocks();
	//����MerchBlocks.xml
	bool32 Load ( const char* strFile );
	//�ͷ�
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
