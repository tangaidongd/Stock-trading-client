#if !defined(AFX_LJISHU_H__A3E1ADE5_B7F2_11D1_83EC_0000E8593F1A__INCLUDED_)
#define AFX_LJISHU_H__A3E1ADE5_B7F2_11D1_83EC_0000E8593F1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CFormularContent.h : header file
#include "stdafx.h"
#include "define.h"
#include "tinyxml.h"
#include "CFormularComputeParent.h"
#include "IndexExtraData.h"
#include "TechExport.h"
#include "BlockManager.h"

extern const CString g_KaStrIndexNotShow[];

struct FLOAT11
{
	float fValue[11];
};


//extern const char* KStrFormulaGroup1;
//extern const char* KStrFormulaGroup1Name;
//extern const char* KStrFormulaGroup2;
//extern const char* KStrFormulaGroup2Name;
//extern const char* KStrFormulaIndex;
//extern const char* KStrFormulaIndexId;
//extern const char* KStrFormulaIndexDesc;
//extern const char* KStrFormulaIndexPassword;
//extern const char* KStrFormulaIndexHotkey;
//extern const char* KStrFormulaIndexFlag;
//extern const char* KStrFormulaIndexLastTime;//new
//extern const char* KStrFormulaIndexContent;
//extern const char* KStrFormulaIndexHelp;
//extern const char* KStrFormulaParam;
//extern const char* KStrFormulaParamName;
//extern const char* KStrFormulaParamMin;
//extern const char* KStrFormulaParamMax;
//extern const char* KStrFormulaParamDefault;
//extern const char* KStrFormulaParamStep;
//extern const char* KStrFormulaParamDesc;//new
//extern const char* KStrFormulaLine;
//extern const char* KStrFormulaLineY;
//extern const CString KStrTradeEnterLong;
//extern const CString KStrTradeExitLong;
//extern const CString KStrTradeEnterShort;
//extern const CString KStrTradeExitShort;
//extern const CString KStrTradeDefault;

// ��ʽ����
enum E_FormularType
{
	EFTNormal = 0,			// ��ͨ����ָ��
	EFTClrKLine,			// ��� K ��
	EFTCdtChose,			// ����ѡ��
	EFTTradeChose,			// ����ѡ��

	EFTCount,			
};

// ָ�����Ʊ��������
enum E_IndexStockType
{
	EIST_None = 0,	// ����������
	EIST_Index = 1,	// ָ��ָ�꣬����ʾ��ָ��
	EIST_Stock = 2,	// ������ָ�꣬����ʾ�ڸ���
};

class EXPORT_CLASS CFormularContent
{
// Attributes
public:
	//////////////////////////////////////////////////////////////////
	//���ӵĲ���3��,���ò���2��,�ϼ�5��
	BOOL	bNew;
	long	last_time;
	CString	descPara[PARAM_NUM];
	
	int		numLine;			// Ĭ�ϵ� Y ֵ�������		
	float	line[PARAM_NUM];	// Ĭ�ϵ� Y ֵ�����, ���� KDJ �е� 20,50,80

	int     numExtraY;					// ����� Y ��������
	float	lineExtraY[EXTRA_Y_NUM];	// ����� Y �������

	//////////////////////////////////////////////////////////////////
	int		numPara;//��������
	CString	namePara[PARAM_NUM];//��������(���15���ַ�)
	float	max[PARAM_NUM];	//���ֵ
	float	min[PARAM_NUM];//��Сֵ
	float	defaultVal[PARAM_NUM];//ȱʡֵ
	float	stepLen[PARAM_NUM];//����

	CArray<FLOAT11,FLOAT11&>	defaultValArray;
	CString	name;//��������ָ������(���9���ַ�)
	CString	password;//����
	CString	explainBrief;//	ָ��˵��
	CString hotkey;
	uint32	flag;
	long	lXday; // ������

	CString	formular;//��ʽ
	CString	help;//����ע��

// Operations
public:

	CFormularContent* Clone ( );
	const CFormularContent &Assign(const CFormularContent &forSrc);
	bool32 EqualFormulaData ( CFormularContent* pCompare );
	bool32 EqualParam ( CFormularContent* pCompare );

	CFormularContent();
	virtual ~CFormularContent();
	void InitNew();

	CString DefaultParamToString();
	
	void InitDefaultValArray();
	void AddDefaultValToArray();
	bool IsValid();
	static float GetParamDataEach(int iIndex,CFormularContent* pJishu);
	static float GetParamDataEach(int iIndex,int nKlineType,CFormularContent* pJishu);
	
	void AddOften();
	void ReMoveOften();

	void AddProtected();
	void RemoveProtected();

	bool32 IsIndexStockTypeMatched(E_IndexStockType eIST);

	void _ToXml(TiXmlElement * pNode);
	void _FromXml(TiXmlElement * pNode);

	bool32	BePassedXday();
	bool32  BeNeedScreenKLineNum();

public:	
	/*static uint32 KAllowMinute;
	static uint32 KAllowDay;
	static uint32 KAllowMinute5;
	static uint32 KAllowMinute15;
	static uint32 KAllowMinute30;
	static uint32 KAllowMinute60;
	static uint32 KAllowMinuteUser;
	static uint32 KAllowDayUser;
	static uint32 KAllowWeek;
	static uint32 KAllowMonth;
	static uint32 KAllowQuarter;
	static uint32 KAllowYear;

	static uint32 KAllowTrend;
	static uint32 KAllowTick;*/

	enum E_AllowType
	{
		KAllowMinute		= 0x00000001,
		KAllowDay			= 0x00000002,
		KAllowMinute5		= 0x00000004,
		KAllowMinute15		= 0x00000008,
		KAllowMinute30		= 0x00000010,
		KAllowMinute60		= 0x00000020,
		KAllowMinuteUser	= 0x00000040,
		KAllowDayUser		= 0x00000080,
		KAllowWeek			= 0x00000100,
		KAllowMonth			= 0x00000200,
		KAllowQuarter		= 0x00000400,
		KAllowYear			= 0x00001000,
		KAllowMinute180		= 0x00002000,
		KAllowMinute240		= 0x00004000,
		KAllowTrend			= 0x00010000,
		KAllowTick			= 0x00020000,
		KAllowMain			= 0x00100000,
		KAllowSub			= 0x00200000,
	};
	
	static uint32 KIntervalMask;

	//����
	bool32 m_bProtected;
	bool32 m_bOften;
	bool32 m_bSystem;
	bool32 m_bCanRestore;
	E_FormularType m_eFormularType;
	//bool32 m_bNeedRefresh;
	BYTE	m_byteIndexStockType;	// ָ�����Ʊ����
};


/////////////////////////////////////////////////////////////
typedef struct T_IndexMapPtr
{
	void* p1;
	void* p2;
}	T_IndexMapPtr;

//����
class EXPORT_CLASS CIndexParamData
{
public:
	CIndexParamData();
	~CIndexParamData();
	
	void	_ToXml(TiXmlElement * pNode);
	void	_FromXml(TiXmlElement * pNode);

	CString DefaultParamToString();

	//
	CString name;//ָ������
	int		numPara;//��������
	CString	namePara[PARAM_NUM];//��������(���15���ַ�)
	CString descPara[PARAM_NUM];
	float	max[PARAM_NUM];	//���ֵ
	float	min[PARAM_NUM];//��Сֵ
	float	defaultVal[PARAM_NUM];//ȱʡֵ

};

void EXPORT_CLASS IndexParamToContent(CIndexParamData* pParam,CFormularContent* pContent);
void EXPORT_CLASS IndexContentToParam(CFormularContent* pContent,CIndexParamData* pParam);

const uint32 KIndexFlagOften		= 0x01;
const uint32 KIndexFlagProtected	= 0x02;

//С����:����/����/����
class CIndexGroup2
{
public:
	CIndexGroup2();
	~CIndexGroup2();

	//
	void _ToXml(TiXmlElement * pNode,CMapStringToPtr& Names);
	void _FromXml(TiXmlElement * pNode,CMapStringToPtr& Names);

	//
	CString			m_StrName;
	TiXmlElement*	m_pXmlElement;
	E_FormularType	m_eFormularType;

	CArray<CFormularContent*,CFormularContent*> m_Contents;	
};

// �����:����ָ��/����ѡ��/���K��
class CIndexGroup1
{
public:
	CIndexGroup1();
	virtual ~CIndexGroup1();

	//
	virtual void _ToXml(TiXmlElement * pNode, CMapStringToPtr& Names) = 0;
	virtual void _FromXml(TiXmlElement * pNode, CMapStringToPtr& Names) = 0;

	//
	CString			m_StrName;
	TiXmlElement*	m_pXmlElement;
	E_FormularType	m_eFormularType;
};

// ����ָ��
class CIndexGroupNormal : public CIndexGroup1
{
public:
	CIndexGroupNormal();
	~CIndexGroupNormal();

	virtual void _ToXml(TiXmlElement * pNode, CMapStringToPtr& Names);
	virtual void _FromXml(TiXmlElement * pNode, CMapStringToPtr& Names);

public:
	//
	CArray<CIndexGroup2*,CIndexGroup2*> m_Group2;	
};

// ����ѡ��
class CIndexGroupCdtChose : public CIndexGroup1
{
public:
	CIndexGroupCdtChose();
	~CIndexGroupCdtChose();

	virtual void _ToXml(TiXmlElement * pNode, CMapStringToPtr& Names);
	virtual void _FromXml(TiXmlElement * pNode, CMapStringToPtr& Names);

public:
	//
	CArray<CIndexGroup2*, CIndexGroup2*> m_Group2;
};

// ����ѡ��
class CIndexGroupTradeChose : public CIndexGroup1
{
public:
	CIndexGroupTradeChose();
	~CIndexGroupTradeChose();

	virtual void _ToXml(TiXmlElement * pNode, CMapStringToPtr& Names);
	virtual void _FromXml(TiXmlElement * pNode, CMapStringToPtr& Names);
	
public:
	// �ӽڵ�ֱ����ָ�깫ʽ
	CArray<CFormularContent*,CFormularContent*> m_ContentsTrade;
};

// ��� K ��
class CIndexGroupClrKLine : public CIndexGroup1
{
public:
	CIndexGroupClrKLine();
	~CIndexGroupClrKLine();

	virtual void _ToXml(TiXmlElement * pNode, CMapStringToPtr& Names);
	virtual void _FromXml(TiXmlElement * pNode, CMapStringToPtr& Names);

public:
	// ��ô�ӽڵ�ֱ����ָ�깫ʽ
	CArray<CFormularContent*,CFormularContent*> m_ContentsClrKLine;
};

//
class EXPORT_CLASS CIndexContentXml
{
public:
	CIndexContentXml();
	~CIndexContentXml();

	//
	void Free();
	void Save();
	bool32 Load(const char* strFile );
	T_IndexMapPtr* Get(CString StrName);

	//
	bool32 Add(CString StrGroup1, CString StrGroup2, CFormularContent* pContent);
	bool32 Del(CString StrName);
	bool32 Modify(CFormularContent* pContent);	
	bool32 GetGroup(CFormularContent* pContent, CString& StrGroup1, CString& StrGroup2);
	
	//���ָ����������
	void CheckIndexTryDate();


	// �½�һ������ѡ�ɵĽڵ�
	CIndexGroupCdtChose* NewGroupCdtChose();

	// �½�һ������ѡ�ɵĽڵ�
	CIndexGroupTradeChose* NewGroupTradeChose();
	
	// �½�һ�����K �ߵĽڵ�
	CIndexGroupClrKLine* NewGroupClrKLine();
		
	// 
	CMapStringToPtr		m_Names4Index;	
	TiXmlDocument*		m_pXmlDocument;
	TiXmlElement*		m_pRootElementRAM;			// �ڴ��е� xml ���ڵ�.

	// ָ�깫ʽ
	CIndexGroupNormal*		m_pGroupNormal;

	// ����ѡ�ɹ�ʽ:
	CIndexGroupCdtChose*	m_pGroupCdtChose;

	// ����ѡ�ɹ�ʽ:
	CIndexGroupTradeChose*	m_pGroupTradeChose;

	// ���K ��
	CIndexGroupClrKLine*	m_pGroupClrKLine;
};

class CHotKey;

class AFX_EXT_CLASS CFormulaLib: public CObserverUserBlock
{
public:
	CFormulaLib();
	~CFormulaLib();
	
	static CFormulaLib* instance();
	static void DelInstance();		// ɾ����ʽ��Դ
	static bool32		BeIndexShow(const CString& StrName);

	void	Reload();
	void	GetVolIndexNames(CStringArray& Names);
	void	GetJinDunSpecialIndexNames(CStringArray& Names);
	void	GetAllowNames(uint32 iFlag, CStringArray& Names, E_IndexStockType eIST = EIST_None);	

	void	BuildHotKeyList(CArray<CHotKey, CHotKey&>&	arrHotKeys);
	
	//ɾ��ָ���ĵ�ǰ�õ�.
	void	RemoveAlloc(int32 id);
	
	//���ݴ�xml�д�����content,ѡȡ��newһ����ǰ�õ�.
	CFormularContent* AllocContent(int32 id, CString StrName);

	//
	CFormularContent* GetFomular(const CString& StrName);

	//
	void	GetFomulars(IN E_FormularType eType, bool32 bNeedUsers, OUT CArray<CFormularContent*, CFormularContent*>& aFomulars);
	
	// �����ϵͳָ��, ��ʾ��ʱ���е����
	bool32	BeSpecialSysIndex(CFormularContent* pFormular);
	
	CString	GetSpecialSysIndexGroupName(CFormularContent* pFormular);
	void	SetCheckCode(CString strCheckCode);		// ����ָ��У����

	// 
	const BASEINFO*	GetBaseInfo();

	CIndexContentXml	m_SysIndex;
	CIndexContentXml	m_ModifyIndex;
	CIndexContentXml	m_UserIndex;
private:
	void RemoveHotKey(E_HotKeyType eHotKeyType, CArray<CHotKey, CHotKey&>&	arrHotKeys);

private:
	CMap<int32,int32,CFormularContent*,CFormularContent*> m_AllocContents;

	// ��̬����
	BASEINFO			m_BaseInfo;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LJISHU_H__A3E1ADE5_B7F2_11D1_83EC_0000E8593F1A__INCLUDED_)
