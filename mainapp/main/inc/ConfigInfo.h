#pragma once

#ifndef _CONFIG_INFO_H_
#define _CONFIG_INFO_H_

#include <map>
#include <algorithm>
using std::map;

#include "PluginFuncRight.h"
#include "StructDef.h"
#include "PubFonction.h"
#include "NCButton.h"




#define  INVALID_VALUE -1

enum BUY_SELL_PLATE
{
	YESTERDAY_SETTLEMENT = 1,// ���
	YESTERDAY_CLOSE			 // ����
};

enum E_PromptType
{
	EPT_MessageBox = 0,	// �Ի�����ʾ xx�汾������ϵxxx
	EPT_IE,				// �����������ʾ

	EPT_End
};

typedef  std::vector<CNCButton> buttonContainerType;
#define  FOR_EACH(container, it) for(it = container.begin(); it != container.end(); ++it)
#define  FOR_EACH_REVERST(container, it) for(it = container.rbegin(); it != container.rend(); ++it)
class TiXmlElement;

///////////////////////////////////////////////////////////////////////////////////
//
class CButtonConfig
{
public:
	virtual bool32	LoadXml(TiXmlElement* p);
	TCHAR		m_StrName[MAX_PATH];			// ��ť��ʾ������
	TCHAR		m_StrPathPic[MAX_PATH];			// ��ťͼƬ��·��
	TCHAR		m_StrPathPic2[MAX_PATH];		// ��ťͼƬ��·�� ���2

	TCHAR		m_StrPathPic3[MAX_PATH];		// ��ťͼƬ��·�� ���3

	CButtonConfig &operator = (CButtonConfig &bf)
	{
		memcpy(m_StrName,bf.m_StrName,sizeof(m_StrName));
		memcpy(m_StrPathPic,bf.m_StrPathPic,sizeof(m_StrPathPic));
		memcpy(m_StrPathPic2,bf.m_StrPathPic2,sizeof(m_StrPathPic2));
		memcpy(m_StrPathPic3,bf.m_StrPathPic3,sizeof(m_StrPathPic3));
		return *this;
	}

	CButtonConfig()
	{
		memset(m_StrName,0,sizeof(m_StrName));
		memset(m_StrPathPic,0,sizeof(m_StrPathPic));
		memset(m_StrPathPic2,0,sizeof(m_StrPathPic2));
		memset(m_StrPathPic3,0,sizeof(m_StrPathPic3));
	}
};
typedef CArray<CButtonConfig, CButtonConfig> arrButtonConfig;
///////////////////////////////////////////////////////////////////////////////////

// �û���ϸҳ��
enum E_UserInfoShowType
{
	EType_Normal = 0,     // ����ֱ�׼��ʽ
	EType_Cfm,            // ����������ҳ
	EType_Dialog,         // �Ի��򵯴���ҳ
	EType_Hide,			  // ����ʾ
};

// ����汾
enum E_VersionType
{
	EVT_None = 0,
	EVT_Free,             // ��Ѱ�
	EVT_Standard,         // ��׼��
	EVT_Featured,         // ��ѡ��
	EVT_Advanced,		  // �߼���
		
};

class CUserInfoCfm
{
public:
	CString m_StrUrl;				// �û���ϸ��Ϣ��ҳ��ַ
	E_UserInfoShowType m_eShowType;	// ��ַ��ʾ����

	CUserInfoCfm()
	{
		m_StrUrl = _T("");
		m_eShowType = EType_Normal;
	}
};

struct T_NewCenterTabInfo
{
	int32 iMenuID;
	int32 iTreeID;

	T_NewCenterTabInfo()
	{
		iMenuID = 0;
		iTreeID = 0;
	}
};

struct TUserDealRecord 
{
	bool32 bRecord;		// �Ƿ��¼
	int32 iType;		// ��¼����(1ֻ��¼����, 2ֻ��¼��Ʒ�л�, 0��¼����)

	TUserDealRecord()
	{
		bRecord = true;
		iType = 0;
	}
};

// ����������Ϣ
typedef struct _TradeCfgParam
{
	CString          StrBrokerName;	    // ȯ������
	CString          StrAppName;		// �����������
	CString          StrShortcut;		// ȯ��������д
	int32            iType;			    // �����������
	bool32           bDefaultTrade;     // �Ƿ�Ĭ�Ͻ������
	CString          StrRun;			// ��ִ�г�������
	CString          StrUrl;			// ����Ŀ¼
	CString          StrLogo;			// logo
	bool32           bNewAdd;           // �����ص� ��
	bool32           bDldComplete;      // �Ƿ���������

	_TradeCfgParam()
	{
		iType         = 1;
		bDefaultTrade = false;
		bNewAdd       = false;
		bDldComplete  = false;
	}
}T_TradeCfgParam;

//����������˵�
struct T_QuoteMarketItem
{
	CString     StrMarketName;         // �г�����
	CString     StrMarketCfmName;	   // ��������  
};

class CShowBuySell
{
public:
	CShowBuySell(){m_showBuySellId = 0; m_showItem = 5;}
	int m_showBuySellId;
	int m_showItem;
};

//��ҳ��ť
class CButtonWebConfig : public CButtonConfig
{
public:
	virtual bool32	LoadXml(TiXmlElement* p);
	CString			m_strUrl;		// url
	CString			m_StrWspName;		// ������������	
};
typedef CArray<CButtonWebConfig, CButtonWebConfig> arrWebConfig;
///////////////////////////////////////////////////////////////////////////////////

// ���鰴ť
class CButtonQuoteConfig : public CButtonConfig
{
public:
	virtual bool32	LoadXml(TiXmlElement* p);
	int32			m_iMarketID;		// �г�ID	
};
typedef CArray<CButtonQuoteConfig, CButtonQuoteConfig> arrQuoteConfig;
///////////////////////////////////////////////////////////////////////////////////

// ��������ť
class CButtonWspConfig : public CButtonConfig
{
public:
	virtual bool32	LoadXml(TiXmlElement* p);
	TCHAR		m_StrWspName[MAX_PATH];		// ������������	

	CButtonWspConfig &operator = (CButtonWspConfig &bf)
	{
		memcpy(m_StrName,bf.m_StrName,sizeof(m_StrName));
		memcpy(m_StrPathPic,bf.m_StrPathPic,sizeof(m_StrPathPic));
		memcpy(m_StrPathPic2,bf.m_StrPathPic2,sizeof(m_StrPathPic2));
		memcpy(m_StrPathPic3,bf.m_StrPathPic3,sizeof(m_StrPathPic3));
		memcpy(m_StrWspName,bf.m_StrWspName,sizeof(m_StrWspName));
		return *this;
	}

	CButtonWspConfig()
	{
		memset(m_StrWspName,0,sizeof(m_StrWspName));
	}
};

typedef CArray<CButtonWspConfig, CButtonWspConfig> arrWspConfig;
///////////////////////////////////////////////////////////////////////////////////

// ��ɫָ�갴ť
class CButtonFormularConfig : public CButtonConfig
{
public:
	virtual bool32	LoadXml(TiXmlElement* p);
	CString		m_StrWspName;		// ������������	
	CString		m_StrHelpImg;		// ָ�����˵����ͼƬ
};
typedef CArray<CButtonFormularConfig, CButtonFormularConfig> arrFormularConfig;
///////////////////////////////////////////////////////////////////////////////////

//
class CWndSize
{
public:
	CString		m_strID;		// �Ի����ʶID
	CString		m_strTitle;		// �Ի����������ʾ�ı���
	int32		m_iWidth;		// �Ի���Ŀ��
	int32		m_iHeight;		// �Ի���ĸ߶�
};
typedef CArray<CWndSize, CWndSize> arrWndSizeConfig;
//////////////////////////////////////////////////////////////////////////////////

class CMenuConfig
{
public:
	virtual bool32	LoadXml(TiXmlElement* p);
	CString      m_StrName;		// Name
	CString      m_StrUrl;      // Url
};
typedef CArray<CMenuConfig, CMenuConfig> arrMenuConfig;
//////////////////////////////////////////////////////////////////////////////////




//ָ�����ļ�
struct T_IndexGroupItem
{
	T_IndexGroupItem()
	{
		iRegion= INVALID_VALUE;
		strIndexName = _T("");
		bRegionMainIndex = false;
	}
	int         iRegion;			// ָ������
	CString     strIndexName;		// ָ������ 
	bool32		bRegionMainIndex;	// �Ƿ�Ϊ��ָ��
};

/////////////////////////////////////////////////////////
// ָ���ļ�
////////////////////////////////////////////////////////
class CIndexGroupItem
{
public:
	CIndexGroupItem()
	{
		m_strGroupIndexsName= _T("");
		m_vGroupIndex.clear();
	}
public:
	CString m_strGroupIndexsName;
	std::vector<T_IndexGroupItem> m_vGroupIndex;
};



typedef enum E_AdvsUrlType
{
	EAUTLink,              //�������
	EAUTPit				   //���ͼƬ
} E_AdvsUrlType;

class CConfigInfo
{
public:
	enum E_Flag
	{
		FlagDefault,			// Ĭ�ϱ�־ - ȫ�ܰ�
		FlagZhanLueJia,			// ս�Լ�
		FlagDaJiangJun,			// �ɴ󽫾�
		FlagBoHaiQiHuo,			// �����ڻ�
		FlagCaiFuJinDunGuPiao,		// �Ƹ���ܹ�Ʊ
		FlagCaiFuJinDunQiHuo,		// �Ƹ�����ڻ�
		FlagCaiFuJinDunFree,		// �Ƹ������Ѱ�

		FlagCount
	};

	static	CConfigInfo *Instance();
	static  void	DelInstance();			// ������Դ

	CString		GetVersionName() const { return m_StrVerName; };
	CString		GetVersionNo() const	{ return m_StrVerNo; };
	DWORD		GetVersionFlag() const	{ return m_dwFlag; };
	CString		GetModule() const	{ return m_StrMdl; };
	CString		GetAuthKey() const { return m_StrAuthKey; };
	CString		GetPrompt()  const	{ return m_StrPrompt; }
	
	CString		GetOrgKey() const { return m_StrOrgKey; }
	CString     GetNewsCenterOrgCode() const { return m_StrNewsCenterOrgCode; }
	CString		GetCodePlatForm() const { return m_StrPlatCode; }

	// ��ȡָ��������
	void GetIndexGroupItem(CString strIndexName, vector<T_IndexGroupItem>& varrIndexGroup);

	// �������ļ�
	bool32		LoadSysConfig();
	void		SaveSysConfig();

	// ���������
	bool32		LoadToolBarConfig();

	//�˵����������
	bool32      LoadCaptionConfig();

	// ָ���������ļ�
	bool32 LoadIndexGroupConfig();

    //�������������
    bool32 LoadChlDataFromXml( T_ChildButtonData &chlData, TiXmlElement *pElemet );
    bool32 LoadFromXml( const char* strXmlPath );


protected:
	CConfigInfo();
	~CConfigInfo();
private:
	void		ResetToDefault();
	bool32		ReadTiEleAttri(TiXmlElement *pEle, const char *pszAttriName, OUT CString &StrValue);

	typedef map<DWORD, CString>		DefaultNameMap;

	CString		m_StrVerName;
	CString		m_StrVerNo;
	CString		m_StrMdl;			// ģ��
	CString		m_StrAuthKey;		// ��֤key
	CString		m_StrOrgKey;		// ��������id
	CString     m_StrNewsCenterOrgCode;    // F10�õ��Ļ�������
	CString		m_StrPlatCode;		// ��Ѷ����code
	CString		m_StrPrompt;
	DWORD		m_dwFlag;

	static  CConfigInfo *m_spThis;
	static	DefaultNameMap m_smapDefNames;
	static	DefaultNameMap m_smapDefKeys;	// Ĭ����֤key

	map<CString, CIndexGroupItem> m_mapIndexGroup;		// ����ָ�����

public:
	std::vector<CShowBuySell*> m_vShowBuySell;	// ָ��ָ����ʾ����5��
	std::vector<T_NewCenterTabInfo> m_vNewsCenterTreeID;
	bool32	m_bShowNewvol;	// �Ƿ���ʾ���֣�ֻ֧�����̣�

	// �����ļ���ֵһЩ�ֶ�
	CString			 m_StrAppNameConfig;	// �����ļ���ָ���ĳ�����
	bool32			 m_bPromptNet;			// �Ƿ���ʾ���粻��ͨ
	CString			 m_StrAboutStr1;		// ���ڶԻ������������
	CString			 m_StrAboutStr2;		// ���ڶԻ������������
	CString          m_StrLoginTitleText;   // ��¼��������
	CString			 m_StrLoginErrorTip;	// ��¼������ʾ
	
	// ����k�ߵĹ����ʾ
	CString          m_strSpecialJiPing;
	bool32           m_bIsVip;
	CString          m_strKAdveriseUrl;

	// ��ť���
	CString          m_strAdvBtnUrl;
	bool32           m_bAdvertiseExist;
	int				 m_iReadIndex;

	//�����˵�
	arrMenuConfig    m_aHelpMenus;   

	//IM����
	bool32           m_bImVersion;

	//
	bool32			 m_bRegOn;
	CString			 m_StrRegUrl;
	CString			 m_StrBtName;

    bool32			 m_bZHMMOn;
    CString			 m_StrZHMMUrl;
    CString			 m_StrZHMMBtName;

	CString          m_StrWelcomeAdvsCode;	// ��ӭ������
	CString          m_StrTopRightAdvsCode;	// ���Ͻǹ�����
	CString          m_StrAboutUsAdvsCode;	// �������ǹ�����
	CString			 m_StrLoginLeftCode;    // ��¼������������

	CString          m_StrSSJPTabId;		// ʵʱ���̱�ǩҳId
	CString			 m_StrJPZXTabId;		// ��Ʒ��Ѷ��ǩҳId

	int32			 m_iTradeMarketID;
	CString			 m_StrResponsbility;
	bool32			 m_bQuickTrade;			// �Ƿ���ʾ���罻��
    bool32           m_bXmlFileUpdate;      // �Ƿ���¹�����XML�ļ�

	CString			 m_StrPicUrl;			// ��¼���棬����ҳ��url
	CString          m_StrQQRegUrl;			// ��¼���棬������QQ��¼url
	CString          m_StrWeChatUrl;		// ��¼���棬������΢�ŵ�¼url
	CString			 m_StrBindUserUrl;		// ��������ҳ��
	bool32			 m_bShowThirdLogin;		// �Ƿ���ʾ������¼

	UINT			 m_nSelectNewsID;		// �������־�Ʒ��Ѷ��ʵʱ����

	CString			m_StrDefUser;
	CString			m_StrDefPwd;

	bool32			m_bMember;				// ��ǰ�汾������ true����Ա��false��������
    bool32          m_firstLogin;			// �ǲ��ǵ�һ�ε�¼

	E_VersionType   m_eVersion;             // �汾��

	CString			m_strBuyPrice;			// �����̱��۱���������е�����ֶ�
	CString			m_strSellPrice;	
	CString			m_strNewPrice;	
	CString			m_strBuy;				// �������������е�����ֶ�
	CString			m_strSell;	

	BUY_SELL_PLATE	m_nBuySellPlate;		// ���������ͣ�1-���  2-���գ�

	CString			m_StrWelcomeUrl;		// ��ӭҳ��ַ
	CString			m_strCheckCode;			// ָ��У����
	CString			m_StrToolBarVer;		// �������ļ��İ汾��

	TUserDealRecord	m_stUserDealRecord;		// �û���Ϊ��¼����

	arrWebConfig		m_aThirdPartyLoginButtons;	// ��¼����QQ��΢�Ű�ť����
	CUserInfoCfm		m_btnUserInfo;				// �û���ϸҳ��
	arrWndSizeConfig	m_aWndSize;				    // һЩ�����ı�Ĵ��ڵĴ�С

	 int				m_iRihgBarHighValue;
public:
    T_ToolBarData m_toolBarData;
    std::vector<T_ColumnData> m_vecColumnData;
    int m_CurrentBtnId;
    int m_CurrentColId;

	std::vector<T_QuoteMarketItem> m_vecQuoteMarket;

public:
    void ResetCurrentBtnID() { m_CurrentBtnId = 6000;}
    void IncreaseCurrentBtnID() { m_CurrentBtnId++; }
    int  GetCurrentBtnID() { return m_CurrentBtnId; }

    void ResetColCurrentID() { m_CurrentColId = 5000;}
    void IncreaseColCurrentID() { m_CurrentColId++; }
    int  GetColCurrentID() { return m_CurrentColId; }

	//��ȡ����������
	CString GetAdvsCenterUrlByCode(CString& strAdCode, E_AdvsUrlType eAdvsUrlType = EAUTPit);

private:
	void ReadNewsCenterTreeIDConfig( TiXmlElement* pTreeIDElement );

	// �ͷŹ�����Image��Դ
	void ReleaseImage();
};


#endif //_CONFIG_INFO_H_