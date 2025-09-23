#ifndef __PROXY_AUTH_STRUCT_H__
#define __PROXY_AUTH_STRUCT_H__

#include <vector>
#include "SharePubStruct.h"

namespace auth
{
	static const int g_iAuthVer = 6;	// DLL�汾
	static const int MAXNAME = 64;
	static const int MAXVALUE = 256;
	static const int MAXCONTENT = 1024;

	enum E_ErrType
	{
		enETOther,			// ��������
		enETNetErr,			// �������
	};

	// �������ṩ�����ݷ������
	enum E_DataServiceType
	{
		EDSTUnknown		= 0x00,		// δ֪�����ݷ���

		//
		EDSTGeneral		= 0x01,		// �ṩ��K�ߣ����ƣ����ݡ�5�����鱨�ۡ�level2���ݡ�tick������Ļ������ݷ��� �磺�г��б���Ʒ������Ϣ����

		//
		EDSTKLine		= 0x02,		// ��ʷK�ߣ����ƣ�
		EDSTTimeSale	= 0x04,		// ��ʷ�ֱ�����

		// ʵʱ����
		EDSTPrice		= 0x08,		// ʵʱ��������
		EDSTLevel2		= 0x10,		// ʵʱlevel2���ݷ���
		EDSTTick		= 0x20		// ʵʱtick���ݷ���		
	};

	//enum E_ServerType
	//{
	//	enSTNone	= 0,
	//	enSTQuote	= 1, 	// ���������
	//	enSTTrade	= 2, 	// ���׷�����
	//	enSTNews	= 4,	// ��Ϣ������
	//	enSTWarn	= 8,	// Ԥ��
	//	enSTInfo    = 32,   // ��Ѷ������
	//};

	enum PROXYTYPE 
	{
		NONE, 
		SOCK4, 
		SOCK5, 
		HTTP
	};

	// �û���Ϊ��¼�еĲ�������
	enum E_ProductType
	{
		EPTFun	= 1,		// �����л�
		EPTCode	= 2,		// �����л�
	};

#pragma pack(push, 1)
	/////////////////////////////////////////////////////////////////////////////

	// ��ע����Ʒ��Ϣ
	typedef struct _AttendMerch
	{
		wchar_t		wszMerchCode[MAXNAME];	// ��Ʒ����		
		int			iMarketId;				// ��Ʒ�����г�id
		int			iDataServiceTypes;		// ȡֵ E_DataServiceType�Ļ�ֵ
		_AttendMerch()
		{
			wszMerchCode[0] = 0;
			iMarketId = 0;
			iDataServiceTypes = 0;
		}
	}T_AttendMerch;

	typedef struct _BigMarketInfo
	{
		int		iBigMarket;		// ���г�ID
		int		iShowID;		// ��ʾID
		int		iCode;			// �г�Code
		wchar_t	wszCnName[MAXNAME];
		_BigMarketInfo()
		{
			iBigMarket	 = 0;
			iShowID = 0;
			iCode	= 0;
			wszCnName[0] = 0;
		}
	}T_BigMarketInfo;

	typedef struct _MarketInfo
	{
		int		iBigMarket;		// ���г�ID
		int		iMarket;		// �г�
		wchar_t	wszName[64];	// ����
		int		iShowID;		// ��ʾID
		_MarketInfo()
		{
			iBigMarket	= 0;
			iMarket		= 0;
			wszName[0]	= 0;
			iShowID		= 0;
		}
	}T_MarketInfo;

	

	typedef struct _UserInfo
	{
		int				iUserId;					// �û�id
		wchar_t			wszUserName[MAXNAME];		// �û���
		wchar_t			wszNickName[MAXNAME];		// �ǳ�
		wchar_t			wszQQ[MAXNAME];				// QQ��
		wchar_t			wszMobile[MAXNAME];			// �ֻ���
		wchar_t			wszImg[MAXVALUE];			// ͼƬ����
		int				iUserGroup;					// �û���
		int				iGroupType;					// 1 Ӫ����2 ������Ա 
		unsigned long   dwRegisterTime;				// ע��ʱ��
		wchar_t			wszUserGroup[MAXNAME];		// �û�����
		wchar_t			wszToken[MAXNAME];			// token

		_UserInfo()
		{
			iUserId			= 0;
			wszUserName[0]	= 0;
			wszNickName[0]	= 0;
			wszQQ[0]		= 0;
			wszMobile[0]	= 0;
			wszImg[0]		= 0;
			iUserGroup		= 0;
			iGroupType		= 1;
			dwRegisterTime   = 0;
			wszUserGroup[0] = 0;
			wszToken[0]		= 0;
		};
	}T_UserInfo;

	// ����Ȩ��
	typedef struct _RightInfo
	{
		int			iHasFun;			// �Ƿ���Ȩ��,0û��Ȩ��
		int			iFunID;				// Ȩ��ID
		wchar_t		wszName[MAXNAME];	// ����
		_RightInfo()
		{
			iHasFun	   = 0;
			iFunID	   = 0;
			wszName[0] = 0;
		}
	}T_RightInfo;

	// Ȩ����ʾ����
	typedef struct _RightTip
	{
		int				iFunID;					// Ȩ��ID
		wchar_t			wszTitle[MAXVALUE];		// ��ʾ����
		wchar_t			wszImgUrl[MAXVALUE];	// ͼƬ��ַ
		wchar_t			wszLinkUrl[MAXVALUE];	// �������

		_RightTip()
		{
			iFunID			= 0;
			wszTitle[0]		= 0;
			wszImgUrl[0]	= 0;
			wszLinkUrl[0]	= 0;
		};
	}T_RightTip;

	// ��Ϣ��ʾ
	typedef struct _MsgTip
	{
		wchar_t			wszTitle[MAXVALUE];		// ��Ϣ����
		wchar_t			wszContent[MAXCONTENT];	// ��Ϣ����
		wchar_t			wszLinkUrl[MAXVALUE];	// ��������

		_MsgTip()
		{
			wszTitle[0]		= 0;
			wszContent[0]	= 0;
			wszLinkUrl[0]	= 0;
		};
	}T_MsgTip;

	// �û���Ϊ��¼
	typedef struct _UserDealRecord
	{
		wchar_t		  wszFunName[MAXNAME];	// ��������
		E_ProductType eProductType;			// ����, 01:�����л�; 02:�����л�
		int			  iViewCnt;				// �������

		_UserDealRecord()
		{
			wszFunName[0]	= 0;
			eProductType	= EPTFun;
			iViewCnt		= 0;
		}
	}T_UserDealRecord;

	// �������
	typedef struct _AdvSet
	{
		wchar_t wszAdCode[MAXNAME];	// ������
		wchar_t	wszAdName[MAXNAME];	// �������
		wchar_t wszRemark[MAXVALUE];// ��ע
		wchar_t	wszAdPic[MAXVALUE];	// ���ͼƬ��ַ
		wchar_t	wszAdLink[MAXVALUE];// �������

		_AdvSet()
		{
			wszAdCode[0] = 0; 
			wszAdName[0] = 0; 
			wszRemark[0] = 0; 
			wszAdPic[0]	 = 0; 
			wszAdLink[0] = 0; 
		}
	}T_AdvSet;

	// ���۱�ײ���ʾ��tab�г�
	typedef struct _ReportTabInfo
	{
		int iMarketCode;			// �г�����
		wchar_t	wszMName[MAXNAME];	// �г�����

		_ReportTabInfo()
		{
			iMarketCode = 0; 
			wszMName[0] = 0; 
		}
	}T_ReportTabInfo;

	// ���ֵ�������
	typedef struct _TextBannerConfig
	{
		wchar_t wszServiceTel[MAXNAME];		// �ͷ��绰
		wchar_t wszStatusContent[1024];		// ״̬�������ı�

		_TextBannerConfig()
		{
			wszServiceTel[0]	= 0; 
			wszStatusContent[0] = 0;
		} 
	}T_TextBannerConfig;

#pragma pack(pop)
};

#endif
