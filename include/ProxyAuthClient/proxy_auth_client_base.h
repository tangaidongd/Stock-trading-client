#ifndef __PROXY_AUTH_CLIENT_H__
#define __PROXY_AUTH_CLIENT_H__

#include "iAuthNotify.h"
#include "ProxyAuthStruct.h"

namespace auth
{
	class iServiceDispose
	{
	public:
		// ���ô˺������ͷ��ɴ�DLL���ص���Ҫdelete���ڴ�ָ��
		virtual void ReleaseData(void *pData) = 0;

		// ��ȡ�û���Ϣ
		virtual void GetUserInfo(OUT T_UserInfo & stInfo) = 0;

		// ��ȡ���г��б�, ���ص�ָ����Ҫ����ReleaseData��delete []
		virtual void GetBigMarketList(OUT T_BigMarketInfo **pstBigMarket, OUT int &iCount) = 0;

		// ��ȡ�г��б�, ���ص�ָ����Ҫ����ReleaseData��delete []
		virtual void GetMarketList(OUT T_MarketInfo **pstMarket, OUT int &iCount) = 0;

		// ��ȡ�������б�, ���ص�ָ����Ҫ����ReleaseData��delete []
		virtual void GetServerList(OUT T_ServerInfo **pstServer, OUT int &iCount) = 0;

		// ��ȡ����Ȩ��, pstRight��Ҫ����ReleaseData��delete[]
		virtual void GetFuncRightList(OUT T_RightInfo **pstRight, OUT int &iCount) = 0;

		// ��ȡȨ����ʾ����
		virtual void GetRightTip(OUT T_RightTip **pstRightTip, OUT int &iCount) = 0;

		// ��ȡ��Ϣ��ʾ
		virtual void GetMsgTip(OUT T_MsgTip & stInfo) = 0;

		// ��ȡ�����Ϣ�б�, ���ص�ָ����Ҫ����ReleaseData��delete []
		virtual void GetAdvList(OUT T_AdvSet **pstAdv, OUT int &iCount) = 0;

		// ��ȡ���۱�tab�б�, ���ص�ָ����Ҫ����ReleaseData��delete []
		virtual void GetReportTabList(OUT T_ReportTabInfo **pstTabInfo, OUT int &iCount) = 0;

		// ��ȡ���ֵ���
		virtual void GetTextBanner(OUT T_TextBannerConfig &stTextBannerConfig) = 0;

		// ��ȡ�������ļ��汾
		virtual const char* GetToolBarVer() = 0;

		// ��ȡ��ѡ������
		virtual const char* GetCusoptional() = 0;

		// �������ļ��Ƿ���
		virtual bool IsToolBarModify() = 0;

		// ��ȡ�Ựtoken
		virtual const char* GetToken() = 0;

		// ��ȡ��Ѷƽ̨����
		virtual const char* GetCodePlatForm() = 0;

		// ��ȡ��Ѷ��������
		virtual const char* GetOrgCode() = 0;
	};

	class iProxyAuthClient
	{
	public:

		typedef void (*CALL_ECONO_FUN)(const char*);

		/**********************************************************************
		* ��������:	ConnectServer
		* ��������:	���ӷ�����
		* �������:	pwszServerAddr ��������ַhttp://192.168.0.1:8088
		*			vServerPort �˿�����(��ɫͨ��)
		* �������:	NA
		* �� �� ֵ:	bool
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/
		virtual bool ConnectServer(const wchar_t * pwszServerAddr, std::vector<unsigned int> &vServerPort) = 0;

		// �Ͽ�����
		virtual void DisConnectServer() = 0;

		virtual void Release() = 0;

		// ��ȡ���ݽӿ�
		virtual iServiceDispose * GetServiceDispose() = 0;

		// ���ûص�
		virtual void SetNotifyObj(iAuthNotify *pNotifyObj) = 0;

		/**********************************************************************
		* ��������:	Authorize
		* ��������:	�˺���֤
		* �������:	pwszMobile �ֻ���
		*			pwszPwd ����
		*			pwszOrgCode ��Ա����
		*			pwszVer �汾��
		* �������:	NA
		* �� �� ֵ:	bool
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/
		virtual bool Authorize(const wchar_t *pwszMobile, const wchar_t *pwszPwd, 
			const wchar_t *pwszOrgCode, const wchar_t *pwszVer) = 0;

		/**********************************************************************
		* ��������:	ThirdAuthorize
		* ��������:	��������֤(qq, ΢��)
		* �������:	pwszLoginKey openId
		*			pwszOrgCode ��Ա����
		*			iChannel ��½����,1 ΢�� 2 QQ
		* �������:	NA
		* �� �� ֵ:	bool
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/
		virtual bool ThirdAuthorize(const wchar_t *pwszLoginKey, const wchar_t *pwszOrgCode, int iChannel) = 0;

		/**********************************************************************
		* ��������:	AddNewUser
		* ��������:	������û�
		* �������:	pwszOrgCode ��������
		*			pwszLoginName ��¼��
		*			pwszMobile �ֻ�����
		*			pwszNickName �ǳ�
		*			pwszRegfrom ��Դ
		*			iGroupId �û���
		* �������:	NA
		* �� �� ֵ:	bool
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/
		virtual bool AddNewUser(const wchar_t *pwszOrgCode, const wchar_t *pwszLoginName, const wchar_t *pwszMobile, 
			const wchar_t *pwszNickName, const wchar_t *pwszRegfrom, int iGroupId) = 0;

		/**********************************************************************
		* ��������:	RefreshToken
		* ��������:	ˢ��Token
		* �������:	NA
		* �������:	NA
		* �� �� ֵ:	bool
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/
		virtual bool RefreshToken() = 0;

		/**********************************************************************
		* ��������:	ReqSaveCusoptional
		* ��������:	�û��ϴ���ѡ��
		* �������:	pwszData ����, Json��ʽ
		*			iType ��������, 1 ��ѡ��
		* �������:	NA
		* �� �� ֵ:	true �ɹ�, false ʧ��
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/
		virtual bool ReqSaveCusoptional(const wchar_t *pwszData, const int iType) = 0;

		/**********************************************************************
		* ��������:	ReqQueryCusoptional
		* ��������:	�û���ȡ��ѡ��
		* �������:	iType ��������, 1 ��ѡ��
		* �������:	NA
		* �� �� ֵ:	true �ɹ�, false ʧ��
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/
		virtual bool ReqQueryCusoptional(const int iType) = 0;

		/**********************************************************************
		* ��������:	ReqAdvInfo
		* ��������:	��ȡ���
		* �������:	pwszMdl ��Ҫ���ص�ģ������б�, ���ģ�飬���á�,�� ���
		*			pwszOrgCode ��������
		* �������:	NA
		* �� �� ֵ:	true �ɹ�, false ʧ��
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/
		virtual bool ReqAdvInfo(const wchar_t *pwszMdl, const wchar_t *pwszOrgCode) = 0;

		/**********************************************************************
		* ��������:	ReqMsgTip
		* ��������:	������Ϣ��ʾ
		* �������:	pwszSendInter ����ʱ�䣨���ӣ�, 5��10��15��30
		* �������:	NA
		* �� �� ֵ:	bool
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/
		virtual bool ReqMsgTip(const wchar_t *pwszSendInter) = 0;

		/**********************************************************************
		* ��������:	ReqQueryUserRight
		* ��������:	��ȡȨ��
		* �������:	pwszMdl ��Ҫ���ص�ģ������б�, ���ģ�飬���á�,�� ���
		*			iType ���� 0��Ȩ��,1 ��Ȩ��,2 ���е�
		* �������:	NA
		* �� �� ֵ:	true �ɹ�, false ʧ��
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/
		virtual bool ReqQueryUserRight(const wchar_t *pwszMdl, const int iType) = 0;

		/**********************************************************************
		* ��������:	ReqQueryRightTip
		* ��������:	��ȡȨ����ʾ
		* �������:	pwszMdl ��Ҫ���ص�ģ������б�, ���ģ�飬���á�,�� ���
		* �������:	NA
		* �� �� ֵ:	true �ɹ�, false ʧ��
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/
		virtual bool ReqQueryRightTip(const wchar_t *pwszMdl) = 0;

		/**********************************************************************
		* ��������:	ReqQueryServerInfo
		* ��������:	��ѯ�������г������Ϣ
		* �������:	pwszKey ��Ʒkey
		*			pwszOrgCode ��Ա����
		*			iSerType ����������, 1-����,2-����,4-��Ϣ,8-Ԥ��,16-pc����,32-��ѯ������,�ɵ�������ʵ��,����д���ݲ�����
		*			bBigMarket ���г���false�����أ�true ����
		*			iMarket	���г��� 1,�г���2��� �ɵ�������ʵ�֣�����д���ݲ�����
		* �������:	NA
		* �� �� ֵ:	true �ɹ�, false ʧ��
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/
		virtual bool ReqQueryServerInfo(const wchar_t *pwszKey, const wchar_t *pwszOrgCode, 
			const int iSerType,const bool bBigMarket, const int iMarket) = 0;

		/**********************************************************************
		* ��������:	ReqQueryBanner
		* ��������:	��ȡ���ֵ���
		* �������:	NA
		* �������:	NA
		* �� �� ֵ:	true �ɹ�, false ʧ��
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/
		virtual bool ReqQueryBanner() = 0;

		/**********************************************************************
		* ��������:	ReqReportTabList
		* ��������:	���󱨼۱�ײ�tab�б�
		* �������:	NA
		* �������:	NA
		* �� �� ֵ:	true �ɹ�, false ʧ��
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/
		virtual bool ReqReportTabList() = 0;

		/**********************************************************************
		* ��������:	ReqStrategyInfo
		* ��������:	����������Ϣ��Ϣ
		* �������:	strAddr IP��ַ
		* �������:	strHttpVer Э���
		* �������:	sJsonData  �����������

		* �������:	NA
		* �� �� ֵ:	true �ɹ�, false ʧ��
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/

		virtual bool ReqStrategyInfo(const wchar_t * strAddr, const wchar_t * strHttpVer, const char* sJsonData) = 0;

		/**********************************************************************
		* ��������:	ReqPickModelInfo
		* ��������:	������������ѡ�����͵�ѡ���б�
		* �������:	strAddr IP��ַ
		* �������:	strHttpVer Э���
		* �������:	sJsonData  �����������

		* �������:	NA
		* �� �� ֵ:	true �ɹ�, false ʧ��
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/

		virtual bool ReqPickModelInfo(const wchar_t * strAddr, const wchar_t * strHttpVer, const char* sJsonData, const wchar_t * wszHead = L"Content-Type: application/json;charset:utf-8;") = 0;

		/**********************************************************************
		* ��������:	ReqPickModelStatus
		* ��������:	������������ѡ�����͵�ѡ��״̬
		* �������:	strAddr IP��ַ
		* �������:	strHttpVer Э���
		* �������:	sJsonData  �����������

		* �������:	NA
		* �� �� ֵ:	true �ɹ�, false ʧ��
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/

		virtual bool ReqPickModelStatus(const wchar_t * strAddr, const wchar_t * strHttpVer, const char* sJsonData, const wchar_t * wszHead = L"Content-Type: application/json;charset:utf-8;") = 0;


		/**********************************************************************
		* ��������:	ReqQueryNewStockInfo
		* ��������:	// �����¹��б�
		* �������:	strAddr IP��ַ
		* �������:	strHttpVer Э���
		* �������:	sJsonData  �����������

		* �������:	NA
		* �� �� ֵ:	true �ɹ�, false ʧ��
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/

		virtual  bool ReqQueryNewStockInfo(const wchar_t * strAddr, const wchar_t * strHttpVer, const char* sJsonData, bool bListedStock) = 0;

		/**********************************************************************
		* ��������:	ReqCheckToken
		* ��������:	������token�Ƿ���Ч

		* �������:	NA
		* �� �� ֵ:	true �ɹ�, false ʧ��
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/

		virtual bool ReqCheckToken() = 0;

		/**********************************************************************
		* ��������:	ReqToolbarFile
		* ��������:	���󹤾��������ļ�
		* �������:	pwszVer �汾��, û�пɲ���д
		* �������:	NA
		* �� �� ֵ:	true �ɹ�, false ʧ��
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/

		virtual bool ReqToolbarFile(const wchar_t *pwszVer) = 0;

		/**********************************************************************
		* ��������:	UserDealRecord
		* ��������:	�û���Ϊ��¼
		* �������:	vRecord ��¼������
		* �������:	NA
		* �� �� ֵ:	bool
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/
		virtual bool UserDealRecord(const std::vector<T_UserDealRecord> &vRecord) = 0;

		/**********************************************************************
		* ��������:	ReqEconoData
		* ��������:	����ƾ���������
		* �������:	pwszServerAddr �ƾ�������������ַ
		*			pszData	���͵�����
		* �������:	NA
		* �� �� ֵ:	bool
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/
		virtual bool ReqEconoData(const wchar_t *pwszServerAddr, const char *pszData) = 0;

		/**********************************************************************
		* ��������:	ReqPushMsgList
		* ��������:	����������Ϣ�б�
		* �������:	pszVersion �汾
		* �������:	pszOrgCode ������
		* �������:	pszType ���ͣ�1 ����������Ϣ
		* �������:	pszCreateUser ��Ϣ������
		* �������:	pszMsgTitle ��Ϣ����
		* �������:	pszPageIndex ��ʼҳ��
		* �������:	pszPageSize ��ѯ��ҳ��
		* �������:	NA
		* �� �� ֵ:	bool
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/
		virtual bool ReqPushMsgList(const char *pszVersion, const char *pszOrgCode, const char* pszType, const char* pszMsgContent, 
			const char* pszMsgTitle, const char* pszGrooupIds, const char* pszState, const char* pszPageIndex, const char *pszPageSize) = 0;

		/**********************************************************************
		* ��������:	ReqPushMsgDetail
		* ��������:	����������Ϣ����
		* �������:	pszMsgId��ϢID
		* �������:	NA
		* �� �� ֵ:	bool
		* ��ʷ��¼:	ԭʼ�汾
		***********************************************************************/
		virtual bool ReqPushMsgDetail(const char *pszVersion, const char* pszMsgId) = 0;
	};
};

#endif
