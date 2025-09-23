#include "StdAfx.h"

#include "CCodeFile.h"
#include "PathFactory.h"
#include "ConfigInfo.h"

#include <string>
using std::string;
using std::wstring;
#include "tinyxml.h"
#include "coding.h"

#include "VersionFun.h"
#include "XmlShare.h"
#include "ShareFun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// XML �ֶ�����
const char* KStrElementAppName				= "APP";
const char* KStrElementAttriAppName			= "appname";
const char* KStrElementAttriPromptNet		= "promptnet";
const char* KStrElementAttriLoginError		= "loginError";
const char* KStrElementAttriloginTitleText  = "loginTitleText";
const char* KStrElementAttriImVersion		= "Imversion";

//
const char* KStrElementAboutBoxName			= "ABOUTBOX";
const char* KStrElementAttriAboutStr1		= "str1";
const char* KStrElementAttriAboutStr2		= "str2";
const char* KStrElementAttriHttpAdd			= "add";

//������
const char* KStrElementToolBarSetting       = "ToolBar";
const char* KStrElementAttriFunc            = "Func";
const char* KStrElementAttriNeedDecrypt     = "IsNeedDecrypt";

//
const char* KStrElementRegName				= "REG";
const char* KStrElementAttriBtnName			= "btname";
const char* KStrElementAttriRegOn			= "on";
const char* KStrElementAttriRegUrl			= "url";

const char* KStrElementAttriZHMM            = "ZHMM";			  // �һ�����
const char* KStrElementAttriThirdPartyLogin = "ThirdPartyLogin";  // ��������¼
const char* KStrElementAttriBindUser        = "BindUserUrl"; // ��������


const char* KStrElementUserInfo				= "UserInfo";	// �û���ϸ��Ϣ
const char* KStrElementAttriShowType		= "ShowType";	// �û���ϸҳ����ʾ����

const char* KStrElementUserDealRecord		= "UserDealRecord";	// �û���ϸ��Ϣ
const char* KStrElementAttriRecordType		= "RecordType";		// �û���ϸҳ����ʾ����
const char* KStrElementAttriRecordOn		= "On";				// �û���ϸҳ���ַ

const char* KStrElementAdvsCode             = "AdvsCode";	 //������
const char* KStrElementAttriWelcomeAdvsCode = "WelcomAdvs";  //��ӭҳ�������
const char* KStrElementAttriTopRightAdvsCode= "TopRightAdvs";//���Ͻǹ�����
const char* KStrElementAttriAboutUsAdvsCode = "AboutUsAdvs"; //�������ǹ�����
const char* KStrElementAttriLoginLeftCode	= "LoginLeft";   //��¼������������

const char* KStrElementNewsTadId            = "NewsTabId";   //������Ѷ��ǩ
const char* KStrElementAttriSSJPTabId	    = "SSJP";		 //ʵʱ���̱�ǩId 	
const char* KStrElementAttriJPZXTabId	    = "JPZX";		 //��Ʒ��Ѷ��ǩId

//
const char* KStrElementAttriHelpCenter      = "HelpCenter";
const char* kStrElementAdvertiseName		= "Advertise";
const char* KStrElementKLineVip				= "Vip";
const char* KStrElementKLineComment			= "SpecialJiPing";
const char* KstrElementAdvBtnUrl			= "BtnAdveriseUrl";
const char* KstrElementAdvKLineUrl			= "KLineAdveriseUrl";
const char* KStrElementReadIndex			= "ReadIndex";
const char* KStrElementCustomCfg			= "CustomCfg";
const char* KStrElementAttriTradeID			= "trademarketid";
const char* KStrElementAttriQuickTrade		= "quicktrade";
const char* KStrElementAttriXMLUpdate       = "xmlfileUpdate";
const char* KStrElementAttriResponsbility	= "Responsbility";
const char* KStrElementAttriPicUrl			= "picurl";
const char* KStrElementAttriQQRegister      = "QQReg";
const char* KStrElementAttriWeChatRegister  = "WeChatReg";
const char* KStrElementAttriDefUser			= "defuser";
const char* KStrElementAttriDefPwd			= "defpwd";
const char* KStrElementAttriMember			= "member";
const char* KStrElementAttriFirstLogin      = "firstLogin";
const char* KStrElementAttriVersion			= "version";
//
const char* KStrElementShowBuySell			= "ShowBuySell";
const char* KStrElementShowBuySellID		= "ID";
const char* KStrElementShowBuySellMarketid	= "Marketid";
const char* KStrElementShowBuySellItem		= "Item";
const char* KStrElementShowBuySellType		= "TYPE";
const char* KStrElementAttriShowBuySellType = "BuySellType";
const char* KStrElementShowBuySellNewvol	= "NEWVOL";
const char* KStrElementShowBuySellIsShow	= "IsShow";
//

const char* KStrElementNewsCenterTreeID		= "NewsCenterTreeID";
const char* KStrElementNewsItem		        = "Item";
const char* KStrElementNewsMenuID			= "MenuID";
const char* KStrElementTreeID		        = "TreeID";

const char* KStrElementWndSize				= "WndSize";
const char* KStrElementWnd					= "Wnd";
const char* KStrElementAttriWndID			= "ID";
const char* KStrElementAttriWndTitle		= "title";
const char* KStrElementAttriWndWidth		= "width";
const char* KStrElementAttriWndHeight		= "height";


const char* KStrElementQuoteMarket          = "QuoteMarket";
const char* KStrElementQuoteMarketItem      = "market";
const char* KStrElementQuoteMarketName      = "name";
const char* KStrElementQuoteMarketCfmName   = "cfmName";

//
const char* KStrElementHeadName				= "HeadName";
const char* KStrElementAttriBuyPrice		= "BuyPrice";
const char* KStrElementAttriSellPrice		= "SellPrice";
const char* KStrElementAttriNewPrice		= "NewPrice";
const char* KStrElementAttriBuy				= "Buy";
const char* KStrElementAttriSell			= "Sell";

const char* KStrElementAttriCheckCode		= "checkcode";
const char* KStrElementAttriToolBarVer		= "toolbarVer";

const char* KStrElementAttriRihgBarHigh		= "RihgBarHigh";
const char* KStrElementAttriItem			= "Item";
const char* KStrElementAttriRihgBarHighValue= "RihgBarHighValue";



// �汾��ص���Ϣ
static const char KStrXMLElementNodeVersion[]			=	"version";		// ��߽ڵ� �汾 (�ɴ󽫾� xxx��)
static const char KStrXMLElementAttriName[]				=	"name";			// ��������
static const char KStrXMLElementAttriVerNo[]			=	"verNo";		// �汾������
static const char KStrXMLElementAttriModule[]			=	"module";		// ģ��,���ģ�飬���á�,�� ���
static const char KStrXMLElementAttriKey[]				=	"key";			// key����
static const char KStrXMLElementAttriOrgKey[]			=	"orgKey";		// ����key����
static const char KStrXMLElementAttriPlatCode[]			=	"platCode";		// ��Ѷ����code
static const char KStrXMLElementAttriFlag[]				=	"flag";			// ���б�־���� �����־�ǹ�Ʊ�Ļ����ڻ���
static const char KStrXMLElementAttriNewsCenterOrgCode[]  =   "newsCenterOrgCode";      //��Ѷ���ĵĻ������룬Ϊ��ȡ�����Լ���F10�����õ�

const char* KStrElementToolBar            = "toolbar";
const char* KStrElementColumn             = "column";
const char* KStrElementButton             = "button";
const char* KStrID						  = "id";

const char* KStrAtrriBarSimpleFont        = "simpleBtnFont";
const char* KStrAtrriBarCateFont          = "categoryBtnFont";
const char* KStrAtrriBarSimpleFontSize    = "simpleBtnFontSize";
const char* KStrAtrriBarCateFontSize      = "categoryBtnFontSize";
const char* KStrAtrriBarHeight            = "height";
const char* KStrAtrriBarLeftSpace         = "leftSpace";
const char* KStrAtrriBarTopSpace          = "topSpace";
const char* KStrAtrriBarBottomSpace       = "bottomSpace";

const char* KStrAtrriColumnWidth          = "width";
const char* KStrAtrriColumnFixed		  = "fixType";

const char* KStrAtrriBtnText              = "text";
const char* KStrAtrriBtnShowText          = "textShow";
const char* KStrAtrriBtnTextColor         = "textColor";
const char* KStrAtrriBtnFontSize          = "fontSize";
const char* KStrAtrriBtnTextOffset        = "textOffset";
const char* KStrAtrriBtnAreaOffset        = "areaOffset";
const char* KStrAtrriBtnEventType         = "eventType";
const char* KStrAtrriBtnEventParam        = "eventParam";
const char* KStrAtrriBtnDialogWidth       = "dialogWidth";
const char* KStrAtrriBtnDialogHeight      = "dialogHeight";
const char* KStrAtrriBtnBkImage           = "bkImg";
const char* KStrAtrriBtnStatusImage       = "statusImg";
const char* KStrAtrriBtnGroupId           = "groupId";




const char* KStrIndexGroupGroupIndexs		= "GroupIndexs";
const char* KStrIndexGroupIndexname			= "Indexname";
const char* KStrIndexGroupname				= "name";
const char* KStrIndexGroupregionMainIndex   = "regionMainIndex";
const char* KStrIndexGroupregion			= "region";


//////////////////////////////////////////////////////////////////////////
//
CConfigInfo *CConfigInfo::m_spThis = NULL;
CConfigInfo::DefaultNameMap CConfigInfo::m_smapDefNames;
CConfigInfo::DefaultNameMap CConfigInfo::m_smapDefKeys;

static int32 s_iToolbarId = 8000;	//	����toolbar.xml�е�Ĭ����Դid��ʼֵ
// �ɴ󽫾��汾
CConfigInfo::CConfigInfo()
{
	if ( m_smapDefNames.empty() )
	{
		m_smapDefNames[FlagDefault]		= _T("�Ƹ����");
		m_smapDefNames[FlagCaiFuJinDunGuPiao]		= _T("�Ƹ����-��Ʊ��");
		m_smapDefNames[FlagCaiFuJinDunQiHuo]		= _T("�Ƹ����-�ڻ���");
	}

	if ( m_smapDefKeys.empty() )
	{
		m_smapDefKeys[FlagDefault]		= _T("cfjd_fu");		// ���汾��Ӧkey
		m_smapDefKeys[FlagCaiFuJinDunGuPiao]		= _T("cfjd_fu");		// ���汾��Ӧkey
		m_smapDefKeys[FlagCaiFuJinDunQiHuo]		= _T("cfjd_fu");		// ���汾��Ӧkey
	}

	m_dwFlag = FlagDefault;
	m_StrVerName = m_smapDefNames[m_dwFlag];
	m_StrAuthKey = m_smapDefKeys[m_dwFlag];
	m_StrVerNo = CGGTongApp::GetFilesVersion(_T(""));
	m_StrOrgKey = _T("2002");
	m_StrNewsCenterOrgCode = _T("6");
	m_StrPlatCode = _T("dzweb");
	m_StrPrompt = _T("���������Ĳ�ƷȨ�ޣ�");

	m_StrAppNameConfig  = L"";
	m_bPromptNet		= false;
	m_StrAboutStr1		= L"";
	m_StrAboutStr2		= L"";
	
	//
	m_bImVersion = false;

    m_bXmlFileUpdate = true;

	//
	m_bIsVip = false;
	m_strAdvBtnUrl = L"";
	m_strKAdveriseUrl = L"";
	m_strSpecialJiPing = L"";
	m_iReadIndex = 0;
	//
	m_bAdvertiseExist = false;
	m_bQuickTrade	  = true;
	m_bQuickTrade	  = false;

	m_StrPicUrl		  = L"";
	m_StrQQRegUrl     = L"";
	m_StrWeChatUrl    = L"";
	m_bShowThirdLogin = false;

	m_StrDefUser	  = L"";
	m_StrDefPwd		  = L"";

	m_bRegOn		  = true;
	m_StrRegUrl		  = L"";
	m_StrBtName		  = L"";

	m_StrBindUserUrl = L"";

    m_bZHMMOn = true;
    m_StrZHMMUrl = L"";

	m_bMember = true;

	m_strBuyPrice= L"���";
	m_strSellPrice = L"����";
	m_strNewPrice = L"����";
	m_strBuy = L"��";
	m_strSell = L"��";

	m_bShowNewvol = false;

	m_nBuySellPlate = YESTERDAY_SETTLEMENT;// Ĭ��Ϊ1������ᣩ
	m_strCheckCode	= _T("");
	m_StrWelcomeUrl = _T("");
	m_StrToolBarVer	= _T("");
	m_StrLoginLeftCode = _T("");
	m_nSelectNewsID    = 0;
	m_iTradeMarketID   = 0;
	m_CurrentBtnId     = 0;
	m_CurrentColId     = 0;
	m_iRihgBarHighValue = 0;

	m_eVersion = EVT_None;

	m_vNewsCenterTreeID.clear();
}

CConfigInfo::~CConfigInfo()
{
	ReleaseImage();

// 	if ( m_spThis == this )
// 	{
// 		m_spThis = NULL;
// 	}
// 	else
// 	{
// 		TRACE(_T("Warning: single instance not single!!\r\n"));
// 	}
}

bool32 CConfigInfo::LoadSysConfig()
{
	ResetToDefault();
	
	//LoadCaptionConfig();
	CString StrPath = CPathFactory::GetSysConfigFilePath();
	if ( StrPath.IsEmpty() )
	{
		return false;
	}

	string sPath = _Unicode2MultiChar(StrPath);
	const char* strFilePath = sPath.c_str();

	TiXmlDocument Doc(strFilePath);
	if ( !Doc.LoadFile() )
	{
		return false;
	}

	TiXmlElement* pRootElement = Doc.RootElement();
	if ( NULL == pRootElement )
	{
		return false;
	}

	for (TiXmlElement* pElement = pRootElement->FirstChildElement(); NULL != pElement; pElement = pElement->NextSiblingElement())
	{
		//
		const char* pElementValue = pElement->Value();
		if (NULL == pElementValue)
		{
			continue;
		}

		//
		if (0 == strcmp(pElementValue, KStrElementAppName))
		{
			// APP NAME
			TiXmlElement* pNameElement = pElement;

			//
			const char* pStrName = pNameElement->Attribute(KStrElementAttriAppName);
			
			if ( NULL == pStrName || strlen(pStrName) <= 0 )
			{
				return false;
			}
			
			//
			m_StrAppNameConfig = _A2W(pStrName);
			
			const char* pStrPromptNet = pNameElement->Attribute(KStrElementAttriPromptNet);
			
			if ( NULL != pStrPromptNet && strlen(pStrPromptNet) > 0 )
			{
				m_bPromptNet = atoi(pStrPromptNet);
			}
			
			const char* pStrLoginError = pNameElement->Attribute(KStrElementAttriLoginError);
			if ( NULL != pStrLoginError )
			{
				m_StrLoginErrorTip = _A2W(pStrLoginError);
			}

			const char* pStrLoginTitleText = pNameElement->Attribute(KStrElementAttriloginTitleText);
			if ( NULL != pStrLoginTitleText)
			{
				m_StrLoginTitleText = _A2W(pStrLoginTitleText);
			}
			//
			const char* pStrIm = pNameElement->Attribute(KStrElementAttriImVersion);
			if ( NULL != pStrIm && strlen(pStrIm) > 0 )
			{
				m_bImVersion = (atoi(pStrIm) > 0) ? true : false;
			}

			//
			const char* pStrRes = pNameElement->Attribute(KStrElementAttriResponsbility);
			if ( NULL != pStrRes && strlen(pStrRes) > 0 )
			{
				m_StrResponsbility = _A2W(pStrRes);
			}		
		}
		// ������ϸҳ��
		else if (0 == strcmp(pElementValue, KStrElementUserInfo))
		{
			TiXmlElement* pElementUserInfo = pElement;
			const char* pStrValue = pElementUserInfo->Attribute(KStrElementAttriShowType);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_btnUserInfo.m_eShowType = E_UserInfoShowType(atoi(pStrValue));
			}

			pStrValue = pElementUserInfo->Attribute(KStrElementAttriRegUrl);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_btnUserInfo.m_StrUrl = _A2W(pStrValue);
			}
		}
		// �û���Ϊ��¼
		else if (0 == strcmp(pElementValue, KStrElementUserDealRecord))
		{
			TiXmlElement* pElementUserRecord = pElement;
			const char* pStrValue = pElementUserRecord->Attribute(KStrElementAttriRecordType);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_stUserDealRecord.iType = atoi(pStrValue);
			}

			pStrValue = pElementUserRecord->Attribute(KStrElementAttriRecordOn);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_stUserDealRecord.bRecord = atoi(pStrValue);
			}
		}
		//wangyongxue 2016/06/12 ������Ϣ
		else if(0 == strcmp(pElementValue, KStrElementAttriHelpCenter))
		{
			// �����˵�
			m_aHelpMenus.RemoveAll();

			TiXmlElement* pElementHelp = pElement;

			for(TiXmlElement* pElementHelpChild = pElementHelp->FirstChildElement(); NULL != pElementHelpChild; pElementHelpChild = pElementHelpChild->NextSiblingElement())
			{
//				const char* pValueNow = pElementHelpChild->Value();
				CMenuConfig menu;
				if (menu.LoadXml(pElementHelpChild))
				{
					m_aHelpMenus.Add(menu);
				}
			}
		}
	

		else if (0 == strcmp(pElementValue, KStrElementAboutBoxName))
		{
			// ABOUT BOX
			TiXmlElement* pAboutElement = pElement;
			if ( NULL != pAboutElement )
			{
				const char* pStr1 = pAboutElement->Attribute(KStrElementAttriAboutStr1);
				if ( NULL != pStr1 && strlen(pStr1) > 0 )
				{
					m_StrAboutStr1 = _A2W(pStr1);
				}
				
				const char* pStr2 = pAboutElement->Attribute(KStrElementAttriAboutStr2);
				if ( NULL != pStr2 && strlen(pStr2) > 0 )
				{
					m_StrAboutStr2 = _A2W(pStr2);
				}
			}
		}
		else if (0 == strcmp(pElementValue, KStrElementRegName))
		{
			// �����Ƿ��
			TiXmlElement* pRegElement = pElement;
			
			const char* pStrValue = pRegElement->Attribute(KStrElementAttriRegOn);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_bRegOn = atoi(pStrValue);
			}
			
			// ��ַ
			pStrValue = pRegElement->Attribute(KStrElementAttriRegUrl);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_StrRegUrl = _A2W(pStrValue);
			}
			
			// ��ť����
			pStrValue = pRegElement->Attribute(KStrElementAttriBtnName);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_StrBtName = _A2W(pStrValue);
			}
		}
        else if (0 == strcmp(pElementValue, KStrElementAttriZHMM))
        {
            // �����Ƿ��
            TiXmlElement* pRegElement = pElement;

            const char* pStrValue = pRegElement->Attribute(KStrElementAttriRegOn);
            if ( NULL != pStrValue && strlen(pStrValue) > 0 )
            {
                m_bZHMMOn = atoi(pStrValue);
            }

            // ��ַ
            pStrValue = pRegElement->Attribute(KStrElementAttriRegUrl);
            if ( NULL != pStrValue && strlen(pStrValue) > 0 )
            {
                m_StrZHMMUrl = _A2W(pStrValue);
            }

            // ��ť����
            pStrValue = pRegElement->Attribute(KStrElementAttriBtnName);
            if ( NULL != pStrValue && strlen(pStrValue) > 0 )
            {
                m_StrZHMMBtName = _A2W(pStrValue);
            }
        }
		else if(0 == strcmp(pElementValue, KStrElementAdvsCode))	//������
		{
			TiXmlElement* pElementAdvsCode = pElement;
			const char* pValueWelcome = pElementAdvsCode->Attribute(KStrElementAttriWelcomeAdvsCode);;

			if ( NULL != pValueWelcome && strlen(pValueWelcome) > 0 )
			{
				m_StrWelcomeAdvsCode = _A2W(pValueWelcome);
			}
			const char* pValueTopRight= pElementAdvsCode->Attribute(KStrElementAttriTopRightAdvsCode);
			if ( NULL != pValueTopRight && strlen(pValueTopRight) > 0 )
			{
				m_StrTopRightAdvsCode = _A2W(pValueTopRight);
			}
			const char* pValueAboutUs = pElementAdvsCode->Attribute(KStrElementAttriAboutUsAdvsCode);
			if ( NULL != pValueAboutUs && strlen(pValueAboutUs) > 0 )
			{
				m_StrAboutUsAdvsCode = _A2W(pValueAboutUs);
			}
			const char* pValueLogin = pElementAdvsCode->Attribute(KStrElementAttriLoginLeftCode);
			if ( NULL != pValueLogin && strlen(pValueLogin) > 0 )
			{
				m_StrLoginLeftCode = _A2W(pValueLogin);
			}
		}
		//
		else if(0 == strcmp(pElementValue, KStrElementNewsTadId))	//������Ѷ��ǩҳID
		{
			TiXmlElement* pElementNewsTabCode = pElement;
			const char* pValueSSJP = pElementNewsTabCode->Attribute(KStrElementAttriSSJPTabId);;

			if ( NULL != pValueSSJP && strlen(pValueSSJP) > 0 )
			{
				m_StrSSJPTabId = _A2W(pValueSSJP);
			}
			const char* pValueJPZX= pElementNewsTabCode->Attribute(KStrElementAttriJPZXTabId);
			if ( NULL != pValueJPZX && strlen(pValueJPZX) > 0 )
			{
				m_StrJPZXTabId = _A2W(pValueJPZX);
			}
		}
		else if(0 == strcmp(pElementValue,KStrElementAttriThirdPartyLogin)) //��������¼
		{
			m_aThirdPartyLoginButtons.RemoveAll();
			TiXmlElement* pElementHomePage = pElement;
				
			const char* pValue = pElementHomePage->Attribute(KStrElementAttriBindUser);
			if ( NULL != pValue)
			{
				m_StrBindUserUrl = _A2W(pValue);
			}

			for(TiXmlElement* pLoginChild = pElementHomePage->FirstChildElement(); NULL != pLoginChild; pLoginChild = pLoginChild->NextSiblingElement())
			{
				CButtonWebConfig btn;
				if (btn.LoadXml(pLoginChild))
				{
					m_aThirdPartyLoginButtons.Add(btn);
				}
			}

			if (m_aThirdPartyLoginButtons.GetSize() > 0)
			{
				m_bShowThirdLogin = true;
			}
			else
			{
				m_bShowThirdLogin = false;
			}
		}
		else if (0 == strcmp(pElementValue, KStrElementNewsCenterTreeID)) 
		{
			ReadNewsCenterTreeIDConfig(pElement);
		}
		else if ( 0 == strcmp(pElementValue, kStrElementAdvertiseName) )
		{
			// Essence
			TiXmlElement* pEssenceElement = pElement;
			
			// �����Ƿ��
			const char* pStrValue = pEssenceElement->Attribute(KStrElementKLineVip);

			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_bIsVip = (atoi(pStrValue) ? true : false);
			}

			pStrValue = pEssenceElement->Attribute(KStrElementKLineComment);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_strSpecialJiPing = _A2W(pStrValue);
			}
			
			pStrValue = pEssenceElement->Attribute(KstrElementAdvBtnUrl);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_strAdvBtnUrl = _A2W(pStrValue);
			}
			
			pStrValue = pEssenceElement->Attribute(KstrElementAdvKLineUrl);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_strKAdveriseUrl = _A2W(pStrValue);
			}

			pStrValue = pEssenceElement->Attribute(KStrElementReadIndex);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_iReadIndex = atoi(pStrValue);
			}			
		}
		else if (0 == strcmp(pElementValue, KStrElementCustomCfg))
		{
			TiXmlElement* pCustomElement = pElement;
			
			// �����г���
			const char* pStrValue = pCustomElement->Attribute(KStrElementAttriTradeID);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_iTradeMarketID = atoi(pStrValue);
			}
			
			// �Ƿ���������µ�
			pStrValue = pCustomElement->Attribute(KStrElementAttriQuickTrade);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_bQuickTrade = atoi(pStrValue);
			}

            // �Ƿ���¹������ļ�
            pStrValue = pCustomElement->Attribute(KStrElementAttriXMLUpdate);
            if ( NULL != pStrValue && strlen(pStrValue) > 0 )
            {
                m_bXmlFileUpdate = atoi(pStrValue);
            }

			// ��¼���棬����ҳ��url
			pStrValue = pCustomElement->Attribute(KStrElementAttriPicUrl);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_StrPicUrl = _A2W(pStrValue);
			}
			
			// Ĭ���û���
			pStrValue = pCustomElement->Attribute(KStrElementAttriDefUser);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_StrDefUser = _A2W(pStrValue);
			}
			
			// Ĭ������
			pStrValue = pCustomElement->Attribute(KStrElementAttriDefPwd);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_StrDefPwd = _A2W(pStrValue);
			}

			// ��Ա���ǽ�����
			pStrValue = pCustomElement->Attribute(KStrElementAttriMember);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_bMember = atoi(pStrValue);
			}
            //�ǲ��ǵ�һ�ε�¼
            pStrValue = pCustomElement->Attribute(KStrElementAttriFirstLogin);
            if ( NULL != pStrValue && strlen(pStrValue) > 0 )
            {
                m_firstLogin = atoi(pStrValue);
            }
            else
            {
                m_firstLogin = 1;
            }

			pStrValue = pCustomElement->Attribute(KStrElementAttriVersion);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_eVersion = (E_VersionType)atoi(pStrValue);
			}

			// ָ��У����
			pStrValue = pCustomElement->Attribute(KStrElementAttriCheckCode);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_strCheckCode = _A2W(pStrValue);		
				CFormulaLib::instance()->SetCheckCode(m_strCheckCode);
			}

			// �������ļ��İ汾��
			pStrValue = pCustomElement->Attribute(KStrElementAttriToolBarVer);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_StrToolBarVer = _A2W(pStrValue);		
			}
		}
		else if (0 == strcmp(pElementValue, KStrElementHeadName))
		{
			TiXmlElement* pHeadElement = pElement;

			// ���
			const char* pStrValue = pHeadElement->Attribute(KStrElementAttriBuyPrice);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_strBuyPrice = _A2W(pStrValue);
			}

			// ����
			pStrValue = pHeadElement->Attribute(KStrElementAttriSellPrice);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_strSellPrice = _A2W(pStrValue);
			}

			// ����
			pStrValue = pHeadElement->Attribute(KStrElementAttriNewPrice);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_strNewPrice = _A2W(pStrValue);
			}

			// ��
			pStrValue = pHeadElement->Attribute(KStrElementAttriBuy);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_strBuy = _A2W(pStrValue);				
			}

			// ��
			pStrValue = pHeadElement->Attribute(KStrElementAttriSell);
			if ( NULL != pStrValue && strlen(pStrValue) > 0 )
			{
				m_strSell = _A2W(pStrValue);
			}
		}
		else if( 0 == strcmp(pElementValue, KStrElementShowBuySell))
		{
			TiXmlElement* pEle = pElement;
			if ( NULL == pEle )
			{
				return false;
			}

			m_vShowBuySell.clear();
			const char *pMarketid = NULL;
			TiXmlElement *pEleSub = pEle->FirstChildElement(KStrElementShowBuySellID);
			while ( NULL != pEleSub )
			{
				//lint --e{429}
				pMarketid = pEleSub->Attribute(KStrElementShowBuySellMarketid);
				if ( NULL != pMarketid && strlen(pMarketid) > 0 )
				{
					CShowBuySell* p = new CShowBuySell;
					ASSERT(p);
					int iMarketid = atoi(pMarketid);
					p->m_showBuySellId = iMarketid;
					const char *pItem = pEleSub->Attribute(KStrElementShowBuySellItem);
					if(pItem == NULL)
					{
						p->m_showItem = 5;
					}
					else
					{
						int iItem = atoi(pItem);
						if(iItem <= 0)
						{
							p->m_showItem = 1;
						}
						else if(iItem > 5)
						{
							p->m_showItem = 5;
						}
						else
						{
							p->m_showItem = iItem;
						}
					}
					m_vShowBuySell.push_back(p);
				}
				pEleSub = pEleSub->NextSiblingElement(KStrElementShowBuySellID);
			}
			// shensj
			TiXmlElement *pNewvol = pEle->FirstChildElement(KStrElementShowBuySellNewvol);
			if(pNewvol)
			{
				const char* pShow = pNewvol->Attribute(KStrElementShowBuySellIsShow);
				if(pShow)
				{
					m_bShowNewvol = atoi(pShow);
				}
			}
			// zhangtao
			TiXmlElement *pType = pEle->FirstChildElement(KStrElementShowBuySellType);
			if (pType)
			{
				const char *pTypeValue = pType->Attribute(KStrElementAttriShowBuySellType);
				if (pTypeValue)
				{
					if (YESTERDAY_SETTLEMENT == atoi(pTypeValue))
						m_nBuySellPlate = YESTERDAY_SETTLEMENT;
					else if (YESTERDAY_CLOSE == atoi(pTypeValue))
						m_nBuySellPlate = YESTERDAY_CLOSE;
				}
			}
		}
		else if (0 == strcmp(pElementValue, KStrElementWndSize))
		{
			TiXmlElement* pEle = pElement;
			if ( NULL == pEle )
			{
				return false;
			}

			m_aWndSize.RemoveAll();
			const char *pszAttr = NULL;
			TiXmlElement *pEleSub = pEle->FirstChildElement(KStrElementWnd);
			while ( NULL != pEleSub )
			{
				CWndSize wnd;
				pszAttr = pEleSub->Attribute(KStrElementAttriWndID);
				if ( NULL != pszAttr)
				{
					wstring	str;
					MultiChar2Unicode(CP_UTF8, pszAttr, str);
					wnd.m_strID = str.c_str();
				}

				pszAttr = pEleSub->Attribute(KStrElementAttriWndTitle);
				if ( NULL != pszAttr)
				{
					wstring	str;
					MultiChar2Unicode(CP_UTF8, pszAttr, str);
					wnd.m_strTitle = str.c_str();
				}

				pszAttr = pEleSub->Attribute(KStrElementAttriWndWidth);
				if ( NULL != pszAttr && strlen(pszAttr) > 0 )
				{
					wnd.m_iWidth = atoi(pszAttr);
				}

				pszAttr = pEleSub->Attribute(KStrElementAttriWndHeight);
				if ( NULL != pszAttr && strlen(pszAttr) > 0 )
				{
					wnd.m_iHeight = atoi(pszAttr);
				}

				m_aWndSize.Add(wnd);
				pEleSub = pEleSub->NextSiblingElement(KStrElementWnd);
			}
		}
		// �г��˵�
		else if (0 == strcmp(pElementValue, KStrElementQuoteMarket))
		{
			TiXmlElement* pEle = pElement;
			if ( NULL == pEle )
			{
				return false;
			}

			m_vecQuoteMarket.clear();
			const char *pszAttr = NULL;
			TiXmlElement *pEleSub = pEle->FirstChildElement(KStrElementQuoteMarketItem);
			while ( NULL != pEleSub )
			{
				T_QuoteMarketItem quoteMarketItem;
				pszAttr = pEleSub->Attribute(KStrElementQuoteMarketName);
				if ( NULL != pszAttr)
				{
					wstring	str;
					MultiChar2Unicode(CP_UTF8, pszAttr, str);
					quoteMarketItem.StrMarketName = str.c_str();
				}

				pszAttr = pEleSub->Attribute(KStrElementQuoteMarketCfmName);
				if ( NULL != pszAttr)
				{
					wstring	str;
					MultiChar2Unicode(CP_UTF8, pszAttr, str);
					quoteMarketItem.StrMarketCfmName = str.c_str();
				}
				m_vecQuoteMarket.push_back(quoteMarketItem);
				pEleSub = pEleSub->NextSiblingElement(KStrElementQuoteMarketItem);
			}
		}
		// �Ҳ������Ĵ�С
		else if (0 == strcmp(pElementValue, KStrElementAttriRihgBarHigh))
		{
			TiXmlElement* pEle = pElement;
			if ( NULL == pEle )
			{
				return false;
			}

			const char *pszAttr = NULL;
			TiXmlElement *pEleSub = pEle->FirstChildElement(KStrElementAttriItem);
			while ( NULL != pEleSub )
			{
				pszAttr = pEleSub->Attribute(KStrElementAttriRihgBarHighValue);
				if ( NULL != pszAttr)
				{
					m_iRihgBarHighValue = atoi(pszAttr);
				}
				pEleSub = pEleSub->NextSiblingElement(KStrElementAttriItem);
			}
		}
		else if (0 == strcmp(pElementValue, KStrXMLElementNodeVersion))
		{
			TiXmlElement* pEle = pElement;
			if ( NULL == pEle )
			{
				return false;
			}
			
			const char *pszAttr = NULL;
			pszAttr = pEle->Attribute(KStrXMLElementAttriName);	// �汾���� - ȡ��
			if ( NULL != pszAttr )
			{
				wstring	str;
				MultiChar2Unicode(CP_UTF8, pszAttr, str);
				//m_StrVerName = str.c_str();
			}
			// ��������Դ��app��ͬ
			m_StrVerName = AfxGetApp()->m_pszAppName;
			
			pszAttr = pEle->Attribute(KStrXMLElementAttriKey);	// ��֤key
			if ( NULL != pszAttr )
			{
				wstring	str;
				MultiChar2Unicode(CP_UTF8, pszAttr, str);
				m_StrAuthKey = str.c_str();
			}
			
			pszAttr = pEle->Attribute(KStrXMLElementAttriOrgKey);	// ����Key
			if ( NULL != pszAttr )
			{
				wstring	str;
				MultiChar2Unicode(CP_UTF8, pszAttr, str);
				m_StrOrgKey = str.c_str();
			}
            
			pszAttr = pEle->Attribute(KStrXMLElementAttriNewsCenterOrgCode);
			if ( NULL != pszAttr )
			{
				wstring	str;
				MultiChar2Unicode(CP_UTF8, pszAttr, str);
				m_StrNewsCenterOrgCode = str.c_str();
			}
           
			pszAttr = pEle->Attribute(KStrXMLElementAttriModule);	// ����Key
			if ( NULL != pszAttr )
			{
				wstring	str;
				MultiChar2Unicode(CP_UTF8, pszAttr, str);
				m_StrMdl = str.c_str();
			}

			pszAttr = pEle->Attribute(KStrXMLElementAttriPlatCode);	// ��Ѷ����code
			if ( NULL != pszAttr )
			{
				wstring	str;
				MultiChar2Unicode(CP_UTF8, pszAttr, str);
				m_StrPlatCode = str.c_str();
			}
			
			pszAttr = pEle->Attribute(KStrXMLElementAttriVerNo); // �汾��
			if ( NULL != pszAttr )
			{
				wstring	str;
				MultiChar2Unicode(CP_UTF8, pszAttr, str);
				m_StrVerNo = str.c_str();
			}
			pszAttr = pEle->Attribute(KStrXMLElementAttriFlag); // ��־ - ȡ��
			if ( NULL != pszAttr )
			{
				m_dwFlag = (DWORD)atol(pszAttr);
				if ( FlagCaiFuJinDunGuPiao != m_dwFlag 
					&& FlagCaiFuJinDunQiHuo != m_dwFlag 
					&& FlagCaiFuJinDunFree != m_dwFlag
					&& FlagDefault != m_dwFlag )
				{
					m_dwFlag = FlagDefault;	// ��Чֵ����ΪĬ�ϰ汾
				}
			}
			
			if ( m_StrVerName.IsEmpty() )	// ����Ĭ�ϵ����ƣ�����Ҫ����
			{
				DefaultNameMap::const_iterator itName = m_smapDefNames.find(m_dwFlag);
				if ( itName != m_smapDefNames.end() )
				{
					m_StrVerName = itName->second;
				}
				else
				{
					ASSERT( 0 );
				}
			}
			
			if ( m_StrAuthKey.IsEmpty() )	// ����Ĭ�ϵ�key������Ҫ����
			{
				DefaultNameMap::const_iterator itName = m_smapDefKeys.find(m_dwFlag);
				if ( itName != m_smapDefKeys.end() )
				{
					m_StrAuthKey = itName->second;
				}
				else
				{
					ASSERT( 0 );
				}
			}
		}
	}
	
	CReportScheme::Instance()->SetBuySellPriceInfo(m_strBuyPrice, m_strSellPrice, m_strNewPrice);

	return true;
}

void CConfigInfo::SaveSysConfig()
{
	CString StrPath = CPathFactory::GetSysConfigFilePath();
	CGGTongDoc *pDoc = DYNAMIC_DOWNCAST(CGGTongDoc, AfxGetDocument());
	
	if ( StrPath.IsEmpty() || NULL == pDoc)
	{
		return;
	}

	string sPath = _Unicode2MultiChar(StrPath);
	const char* strFilePath = sPath.c_str();
	
	TiXmlDocument Doc(strFilePath);
	if ( !Doc.LoadFile() )
	{
		return;
	}
	
	TiXmlElement* pRootElement = Doc.RootElement();
	if ( NULL == pRootElement )
	{
		return;
	}
	
	TiXmlElement* pAdvertise = pRootElement->FirstChildElement(kStrElementAdvertiseName);
	if( NULL != pAdvertise && NULL != pAdvertise->Value()  )
	{
		ASSERT( 0 == strcmp(pAdvertise->Value(), kStrElementAdvertiseName) );
		//
		pAdvertise->SetAttribute(KStrElementReadIndex, m_iReadIndex);
		Doc.SaveFile();
	}

	TiXmlElement *pCustomCfg = pRootElement->FirstChildElement(KStrElementCustomCfg);
	CString StrFirstLogin = L"";
	StrFirstLogin.Format(L"%d", pDoc->m_bAutoRun);
	
	if (pCustomCfg)
	{
		pCustomCfg->SetAttribute(KStrElementAttriToolBarVer, _W2A(m_StrToolBarVer));
		pCustomCfg->SetAttribute(KStrElementAttriFirstLogin, _W2A(StrFirstLogin));
	}

	Doc.SaveFile();
}

bool32 CButtonConfig::LoadXml(TiXmlElement* p)
{
	if (NULL == p)
	{
		return false;
	}

	lstrcpy(m_StrName,_T(""));
	lstrcpy(m_StrPathPic,_T(""));
		
	//
	const char* pValue = p->Attribute("Name");
	if (NULL != pValue)
	{
		// ����
		lstrcpy(m_StrName,_A2W(pValue));
	}
	
	pValue = p->Attribute("Pic");
	if (NULL != pValue)
	{
		// ����
		lstrcpy(m_StrPathPic,_A2W(pValue));
	}

	pValue = p->Attribute("Pic2");
	if (NULL != pValue)
	{
		// ����
		lstrcpy(m_StrPathPic2,_A2W(pValue));
	}

	pValue = p->Attribute("Pic3");
	if (NULL != pValue)
	{
		// ����
		lstrcpy(m_StrPathPic3,_A2W(pValue));
	}

	return true;
}

bool32 CButtonQuoteConfig::LoadXml(TiXmlElement* p)
{
	if (NULL == p)
	{
		return false;
	}

	CButtonConfig::LoadXml(p);
	
	const char* pValue = p->Attribute("Marketid");
	if (NULL != pValue)
	{
		m_iMarketID = atoi(pValue);
	}
	else
	{
		return false;
	}
	
	return true;
}

bool32 CButtonWspConfig::LoadXml(TiXmlElement* p)
{
	if (NULL == p)
	{
		return false;
	}
	
	CButtonConfig::LoadXml(p);

	const char* pValue = p->Attribute("PageName");
	if (NULL != pValue)
	{
		lstrcpy(m_StrWspName,_A2W(pValue));
	}
	else
	{
		return false;
	}
	
	return true;
}

bool32 CButtonFormularConfig::LoadXml(TiXmlElement* p)
{
	if (NULL == p)
	{
		return false;
	}
	
	CButtonConfig::LoadXml(p);
	
	const char* pValue = p->Attribute("PageName");
	if (NULL != pValue)
	{
		m_StrWspName = _A2W(pValue);
	}

	pValue = p->Attribute("Help");
	if (NULL != pValue)
	{
		m_StrHelpImg = _A2W(pValue);
	}
	
	return true;
}

void GetQuoteButtons(IN TiXmlElement* p, IN CString StrPicPath, OUT arrQuoteConfig& aButtons)
{
	if (NULL == p)
	{
		return;
	}

	//
	CString StrPathPicNow;
	
	if (NULL != p->Attribute("Pic"))
	{
		StrPathPicNow = _A2W(p->Attribute("Pic"));
	}
	
	//
	TiXmlElement * pElementNow = p->FirstChildElement();
	while(pElementNow)
	{
		//
		CButtonQuoteConfig btn;
		if(btn.LoadXml(pElementNow))
		{
			//
			if (lstrcmp(btn.m_StrPathPic,_T("")) == 0)
			{
				if (!StrPathPicNow.IsEmpty())
				{
					lstrcpy(btn.m_StrPathPic, StrPathPicNow);
				}
				else
				{
					lstrcpy(btn.m_StrPathPic,StrPicPath);		
				}
			}
			
			//
			aButtons.Add(btn);
		}
		
		pElementNow = pElementNow->NextSiblingElement();
	}
}

bool32 CConfigInfo::LoadToolBarConfig()
{
	CString StrPath = CPathFactory::GetToolBarConfigFilePath();
	if ( StrPath.IsEmpty() )
	{
		return false;
	}
	
	string sPath = _Unicode2MultiChar(StrPath);
	const char* strFilePath = sPath.c_str();
	LoadFromXml(strFilePath);

	return false; 	
}

bool32 CConfigInfo::LoadCaptionConfig()
{
	CString StrPath = CPathFactory::GetCaptionConfigFilePath();
	if ( StrPath.IsEmpty() )
	{
		return false;
	}

	string sPath = _Unicode2MultiChar(StrPath);
	const char* strFilePath = sPath.c_str();

	TiXmlDocument Doc(strFilePath);
	if ( !Doc.LoadFile() )
	{
		return false;
	}

	TiXmlElement* pRootElement = Doc.RootElement();
	if ( NULL == pRootElement )
	{
		return false;
	}

	for (TiXmlElement* pElement = pRootElement->FirstChildElement(); NULL != pElement; pElement = pElement->NextSiblingElement())
	{
		//
		const char* pElementValue = pElement->Value();
		if (NULL == pElementValue)
		{
			continue;
		}
		
	}
	return true;
}


void CConfigInfo::GetIndexGroupItem(CString strIndexName, vector<T_IndexGroupItem>& varrIndexGroup)
{
	if (m_mapIndexGroup.count(strIndexName) > 0)
	{
		varrIndexGroup.assign(m_mapIndexGroup[strIndexName].m_vGroupIndex.begin(),m_mapIndexGroup[strIndexName].m_vGroupIndex.end());
	}
}


bool32 CConfigInfo::LoadIndexGroupConfig()
{
	m_mapIndexGroup.clear();

	CString SPath = "./config/IndexGroup.xml";
	USES_CONVERSION;
	char* strPath = W2A((CString)SPath);

	TiXmlDocument Doc(strPath);
	if (!Doc.LoadFile())
	{
		DWORD dwRet = GetLastError();
		return false;
	}

	TiXmlElement* pRootElement = Doc.RootElement();
	if ( NULL == pRootElement )
	{
		return false;
	}

	for (TiXmlElement* pElement = pRootElement->FirstChildElement(); NULL != pElement; pElement = pElement->NextSiblingElement())
	{
		const char* pElementValue = pElement->Value();
		if (NULL == pElementValue)
		{
			continue;
		}

		if (0 == strcmp(pElementValue, KStrIndexGroupGroupIndexs))
		{
			CIndexGroupItem cIndexGroupItem;

			// ָ������
			const char* pValue = pElement->Attribute(KStrIndexGroupIndexname);
			if (NULL != pValue)
			{
				cIndexGroupItem.m_strGroupIndexsName = _A2W(pValue);
			}

			TiXmlElement* pElementHomePage = pElement;
			for(TiXmlElement* pHomeChild = pElementHomePage->FirstChildElement(); NULL != pHomeChild; pHomeChild = pHomeChild->NextSiblingElement())
			{
				T_IndexGroupItem IndexItem;

				const char* pValue = pHomeChild->Attribute(KStrIndexGroupname);
				if (NULL != pValue)
				{
					// Name
					IndexItem.strIndexName = _A2W(pValue);
				}

				pValue = pHomeChild->Attribute(KStrIndexGroupregion);
				if (NULL != pValue)
				{
					// region
					IndexItem.iRegion = atoi(pValue);
				}

				pValue = pHomeChild->Attribute(KStrIndexGroupregionMainIndex);
				if (NULL != pValue)
				{
					// region
					IndexItem.bRegionMainIndex = atoi(pValue);
				}

				cIndexGroupItem.m_vGroupIndex.push_back(IndexItem);
			}

			m_mapIndexGroup.insert(make_pair(cIndexGroupItem.m_strGroupIndexsName, cIndexGroupItem));
		}
	}
	return true;
}



CConfigInfo * CConfigInfo::Instance()
{
	if ( NULL == m_spThis )
	{
		m_spThis = new CConfigInfo();
		ASSERT( NULL != m_spThis );
	/*	if ( !m_spThis->LoadSysConfig() )
		{
			m_spThis->ResetToDefault();
		}*/

		
	}
	return m_spThis;
}

void CConfigInfo::DelInstance()
{
	if (m_spThis)
	{
		delete m_spThis;
		m_spThis = NULL;
	}
}

void CConfigInfo::ResetToDefault()
{
	m_dwFlag = FlagDefault;
	m_StrVerName = m_smapDefNames[m_dwFlag];
	m_StrAuthKey = m_smapDefKeys[m_dwFlag];
	m_StrOrgKey = _T("2002");
	m_StrNewsCenterOrgCode = _T("6");
	m_StrPlatCode = _T("dzweb");
	m_StrVerNo = CGGTongApp::GetFilesVersion(_T(""));
	m_StrPrompt = _T("���������Ĳ�ƷȨ�ޣ�");
}

bool32 CConfigInfo::ReadTiEleAttri( TiXmlElement *pEle, const char *pszAttriName, OUT CString &StrValue )
{
	ASSERT( NULL!=pEle && NULL != pszAttriName );
	const char *pszAttr = NULL;
	pszAttr = pEle->Attribute(pszAttriName);	// �汾���� - ȡ��
	if ( NULL != pszAttr )
	{
		wstring	str;
		MultiChar2Unicode(CP_UTF8, pszAttr, str);
		StrValue = str.c_str();
		return true;
	}
	return false;
}

bool32 CButtonWebConfig::LoadXml(TiXmlElement* p)
{
	if (NULL == p)
	{
		return false;
	}

	CButtonConfig::LoadXml(p);

	const char* pValue = p->Attribute("Url");
	if (NULL != pValue)
	{
		m_strUrl = _A2W(pValue);
	}
	else
	{
		//ASSERT(0);
		return false;
	}

	pValue = p->Attribute("PageName");
	if(pValue != NULL)
	{
		m_StrWspName = _A2W(pValue);
	}

	return true;
}

bool32 CMenuConfig::LoadXml(TiXmlElement* p)
{
	if (NULL == p)
	{
		return false;
	}
	//
	const char* pValue = p->Attribute("Name");
	if (NULL != pValue)
	{
		// Name
		m_StrName = _A2W(pValue);
	}

	pValue = p->Attribute("Url");
	if (NULL != pValue)
	{
		// Url
		m_StrUrl = _A2W(pValue);
	}
	return true;
}



bool32 CConfigInfo::LoadFromXml( const char* strXmlPath )
{
    CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
    if(pApp && pApp->m_pConfigInfo->m_bXmlFileUpdate)
    {
        if(Decrypt(strXmlPath) != 0)
        {
            return false;
        }
    }

    TiXmlDocument Doc(strXmlPath);
    if (!Doc.LoadFile())
    {
        return false;
    }

    TiXmlElement* pRootElement = Doc.RootElement();
    if ( NULL == pRootElement )
    {
        return false;
    }

    ResetCurrentBtnID();
    ResetColCurrentID();
    m_vecColumnData.clear();
	

    TiXmlElement* pElement = pRootElement->FirstChildElement();
    const char* pElementValue = pElement->Value();
    if (NULL == pElementValue)
    {
        return false;
    }

    if (0 == strcmp(pElementValue, KStrElementToolBar))
    {
        m_toolBarData.Clear();
        const char* pBarValue = pElement->Attribute(KStrAtrriBarSimpleFont);
        if (NULL != pBarValue)
        {
            m_toolBarData.m_SimpleBtnFont = _A2W(pBarValue);
        }

        pBarValue = pElement->Attribute(KStrAtrriBarSimpleFontSize);
        if (NULL != pBarValue)
        {
            m_toolBarData.m_SimpleFontSize = _A2W(pBarValue);
        }

        pBarValue = pElement->Attribute(KStrAtrriBarCateFont);
        if (NULL != pBarValue)
        {
            m_toolBarData.m_CategoryBtnFont = _A2W(pBarValue);
        }

        pBarValue = pElement->Attribute(KStrAtrriBarCateFontSize);
        if (NULL != pBarValue)
        {
            m_toolBarData.m_CategoryFontSize = _A2W(pBarValue);
        }

        pBarValue = pElement->Attribute(KStrAtrriBarHeight);
        if (NULL != pBarValue)
        {
            m_toolBarData.m_iHeight = atoi(pBarValue);
        }

        pBarValue = pElement->Attribute(KStrAtrriBarLeftSpace);
        if (NULL != pBarValue)
        {
            m_toolBarData.m_iLeftSpace = atoi(pBarValue);
        }

        pBarValue = pElement->Attribute(KStrAtrriBarTopSpace);
        if (NULL != pBarValue)
        {
            m_toolBarData.m_iTopSpace = atoi(pBarValue);
        }

        pBarValue = pElement->Attribute(KStrAtrriBarBottomSpace);
        if (NULL != pBarValue)
        {
            m_toolBarData.m_iBottomSpace = atoi(pBarValue);
        }

        pBarValue = pElement->Attribute(KStrAtrriBtnBkImage);
        if (NULL != pBarValue)
        {
            m_toolBarData.m_pBkImage = TransBase64CodeToImage((char*)pBarValue);
            m_toolBarData.m_strImage = pBarValue;
        }
        else
        {
            m_toolBarData.m_pBkImage = NULL;
            m_toolBarData.m_strImage = "";
        }

        TiXmlElement* pElementToolbar = pElement;
        for(TiXmlElement* pElemColumn= pElementToolbar->FirstChildElement(); NULL != pElemColumn; pElemColumn = pElemColumn->NextSiblingElement())
        {
            const char* pElementTmp = pElemColumn->Value();
            if (0 == strcmp(pElementTmp, KStrElementColumn))
            {
                T_ColumnData colData;
                const char* pColValue = pElemColumn->Attribute(KStrAtrriColumnWidth);
                if (NULL != pColValue)
                {
                    colData.m_iColWidth = atoi(pColValue);
                }

				pColValue = pElemColumn->Attribute(KStrAtrriColumnFixed);
				if (NULL != pColValue)
				{
					colData.m_eFixedType = (E_FixedType)atoi(pColValue);
				}

                colData.m_ID = GetColCurrentID();
                IncreaseColCurrentID();

                for(TiXmlElement* pElemBtn= pElemColumn->FirstChildElement(); NULL != pElemBtn; pElemBtn = pElemBtn->NextSiblingElement())
                {
                    const char* pElemenBtnValue= pElemBtn->Value();
                    if (0 == strcmp(pElemenBtnValue, KStrElementButton))
                    {
                        T_ButtonData btnData;
                        const char* pValue = pElemBtn->Attribute(KStrAtrriBtnText);
                        if (NULL != pValue)
                        {
                            btnData.m_Text = _A2W(pValue);
                        }

						pValue = pElemBtn->Attribute(KStrAtrriBtnShowText);
						if (NULL != pValue)
						{
							btnData.m_bShowText  = atoi((char*)pValue);
						}

						pValue = pElemBtn->Attribute(KStrAtrriBtnEventType);
						if (NULL != pValue)
						{
							btnData.m_EventType = _A2W(pValue);
						}

						pValue = pElemBtn->Attribute(KStrAtrriBtnTextColor);
						if (NULL != pValue)
						{						
							btnData.m_crlTextColor = strtoul(pValue+1, NULL, 16);
						}
						else
						{
							if (5 == _wtoi((LPCTSTR)btnData.m_EventType)) // 5������Ǳ�ǩ��ť
							{
								btnData.m_crlTextColor	= RGB(222, 168, 63);	
							}
						}

						pValue = pElemBtn->Attribute(KStrAtrriBtnFontSize);
						if (NULL != pValue)
						{
							btnData.m_fFontSize = atof(pValue);
						}

                        pValue = pElemBtn->Attribute(KStrAtrriBtnTextOffset);
                        if (NULL != pValue)
                        {
                            btnData.m_TextPos = _A2W(pValue);
                        }

						pValue = pElemBtn->Attribute(KStrAtrriBtnAreaOffset);
						if (NULL != pValue)
						{
							btnData.m_AreaPos = _A2W(pValue);
						}

                        pValue = pElemBtn->Attribute(KStrAtrriBtnEventParam);
                        if (NULL != pValue)
                        {
                            btnData.m_EventParam = _A2W(pValue);
                        }

                        pValue = pElemBtn->Attribute(KStrAtrriBtnDialogWidth);
                        if (NULL != pValue)
                        {
                            btnData.m_DialogWidth = _A2W(pValue);
                        }

                        pValue = pElemBtn->Attribute(KStrAtrriBtnDialogHeight);
                        if (NULL != pValue)
                        {
                            btnData.m_DialogHeight = _A2W(pValue);
                        }

                        pValue = pElemBtn->Attribute(KStrAtrriBtnBkImage);
                        if (NULL != pValue)
                        {
                            btnData.m_pBkImage = TransBase64CodeToImage((char*)pValue);
                            btnData.m_strImage = pValue;
                        }

						pValue = pElemBtn->Attribute(KStrID);
						if (NULL != pValue)
						{
							btnData.m_ID = atoi(pValue);
						}
						else
						{
							btnData.m_ID = ++s_iToolbarId;
						}

						pValue = pElemBtn->Attribute(KStrAtrriBtnGroupId);
						if (NULL != pValue)
						{
							btnData.m_iGroupId = atoi(pValue);
						}

                        for(TiXmlElement* pElemChl= pElemBtn->FirstChildElement(); NULL != pElemChl; pElemChl = pElemChl->NextSiblingElement())
                        {
                            const char* pElemenChlValue= pElemChl->Value();
                            if (0 == strcmp(pElemenChlValue, KStrElementButton))
                            {
                                T_ChildButtonData chlBtnData;
                                LoadChlDataFromXml(chlBtnData, pElemChl);          
                              
                                btnData.m_ChildBtnData.push_back(chlBtnData);
                            }
                        }

                        colData.m_BtnData.push_back(btnData);
                    }
                }

                m_vecColumnData.push_back(colData);
            }
        }
    }

    if(pApp && pApp->m_pConfigInfo->m_bXmlFileUpdate)
    {
        Encrypt(strXmlPath);
    }

    return false;
}

bool32 CConfigInfo::LoadChlDataFromXml( T_ChildButtonData &chlData, TiXmlElement *pElemet )
{
    if (NULL == pElemet)
    {
        return false;
    }

    const char* pValue = pElemet->Attribute(KStrAtrriBtnText);
    if (NULL != pValue)
    {
        chlData.m_Text = _A2W(pValue);
    }

	pValue = pElemet->Attribute(KStrAtrriBtnShowText);
	if (NULL != pValue)
	{
		chlData.m_bShowText  = atoi((char*)pValue);
	}

	pValue = pElemet->Attribute(KStrAtrriBtnTextColor);
	if (NULL != pValue)
	{		
		chlData.m_crlTextColor = strtoul(pValue+1, NULL, 16);
	}

	pValue = pElemet->Attribute(KStrAtrriBtnFontSize);
	if (NULL != pValue)
	{		
		chlData.m_fFontSize = atof(pValue);
	}

    pValue = pElemet->Attribute(KStrAtrriBtnTextOffset);
    if (NULL != pValue)
    {
        chlData.m_TextPos = _A2W(pValue);
    }

    pValue = pElemet->Attribute(KStrAtrriBtnEventType);
    if (NULL != pValue)
    {
        chlData.m_EventType = _A2W(pValue);
    }

    pValue = pElemet->Attribute(KStrAtrriBtnEventParam);
    if (NULL != pValue)
    {
        chlData.m_EventParam = _A2W(pValue);
    }

    pValue = pElemet->Attribute(KStrAtrriBtnDialogWidth);
    if (NULL != pValue)
    {
        chlData.m_DialogWidth = _A2W(pValue);
    }

    pValue = pElemet->Attribute(KStrAtrriBtnDialogHeight);
    if (NULL != pValue)
    {
        chlData.m_DialogHeight = _A2W(pValue);
    }

    pValue = pElemet->Attribute(KStrAtrriBtnBkImage);
    if (NULL != pValue)
    {
        chlData.m_pBkImage = TransBase64CodeToImage((char*)pValue);
        chlData.m_strImage = pValue;
    }

	pValue = pElemet->Attribute(KStrAtrriBtnStatusImage);
	if (NULL != pValue)
	{
		chlData.m_pImgStatus = TransBase64CodeToImage((char*)pValue);
		chlData.m_strStatusImage = pValue;
	}

	pValue = pElemet->Attribute(KStrAtrriBtnGroupId);
	if (NULL != pValue)
	{
		chlData.m_iGroupId = atoi(pValue);
	}

	pValue = pElemet->Attribute(KStrID);
	if (NULL != pValue)
	{
		chlData.m_ID = atoi(pValue);
	}
	else
	{
		chlData.m_ID = ++s_iToolbarId;
	}

	for(TiXmlElement* pElemSubMenu= pElemet->FirstChildElement(); NULL != pElemSubMenu; pElemSubMenu = pElemSubMenu->NextSiblingElement())
	{
		const char* pElemenChlValue= pElemSubMenu->Value();
		if (0 == strcmp(pElemenChlValue, KStrElementButton))
		{
			T_ChildButtonData chlSubMenuData;
			LoadChlDataFromXml(chlSubMenuData, pElemSubMenu);          

			chlData.m_ChildSubMenuData.push_back(chlSubMenuData);
		}
	}
    return true;
}

CString CConfigInfo::GetAdvsCenterUrlByCode(CString& strAdCode, E_AdvsUrlType eAdvsUrlType)
{
	auth::T_AdvSet *pAdv;
	int iCount = 0;
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->GetAdvList(&pAdv, iCount);
	CString strAdvsPicUrl = L"";
	for (int i=0; i<iCount; i++)
	{
		auth::T_AdvSet tAdv = pAdv[i];
		CString strCode = tAdv.wszAdCode;
		if (0 == strCode.CompareNoCase(strAdCode))
		{
			if (EAUTLink == eAdvsUrlType)
			{
				strAdvsPicUrl = tAdv.wszAdLink;
			}
			else
			{
				strAdvsPicUrl = tAdv.wszAdPic;
			}
			break;
		}
	}
	pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->ReleaseData(pAdv);
	return strAdvsPicUrl;
}

void CConfigInfo::ReadNewsCenterTreeIDConfig( TiXmlElement* pTreeIDElement )
{
	if (NULL == pTreeIDElement)
	{
		return;
	}

	m_vNewsCenterTreeID.clear();
	const char *pTableID = NULL;
	const char *pTreeID = NULL;

	TiXmlElement *pEleMarket = pTreeIDElement->FirstChildElement(KStrElementNewsItem);
	while ( NULL != pEleMarket )
	{
		T_NewCenterTabInfo tInfo;
		pTableID = pEleMarket->Attribute(KStrElementNewsMenuID);
		if ( NULL != pTableID && strlen(pTableID) > 0 )
		{
			tInfo.iMenuID = atoi(pTableID);	
		}

		pTreeID = pEleMarket->Attribute(KStrElementTreeID);	// ��Ѷ����code
		if ( NULL != pTreeID && strlen(pTreeID) > 0 )
		{
			tInfo.iTreeID = atoi(pTreeID);
		}
		m_vNewsCenterTreeID.push_back(tInfo);

		pEleMarket = pEleMarket->NextSiblingElement(KStrElementNewsItem);
	}
}

void CConfigInfo::ReleaseImage()
{
	DEL(m_toolBarData.m_pBkImage);

	std::vector<T_ColumnData>::iterator it;
	for (it = m_vecColumnData.begin() ; it != m_vecColumnData.end() ; ++it)
	{
		if (it->m_BtnData.size() > 0)
		{
			it->ReleaseImage();
		}
	}
	std::vector<T_ColumnData>().swap(m_vecColumnData);
}