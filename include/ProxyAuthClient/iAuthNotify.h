#ifndef _IAUTHNOTIFY_H_
#define _IAUTHNOTIFY_H_

#define IN
#define OUT

#include <tchar.h>

class iAuthNotify
{
public:
	/**********************************************************************
	* ��������:	OnRespAuth
	* ��������:	��֤ʧ��
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			wchar_t *pszMsg: ������Ϣ
	***********************************************************************/
	virtual void OnRespAuth(bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* ��������:	OnRespUserRight
	* ��������:	�ͻ�Ȩ�޷���
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			wchar_t *pszMsg: ������Ϣ
	***********************************************************************/
	virtual void OnRespUserRight(bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* ��������:	OnRespRightTip
	* ��������:	��Ȩ��������ʾ����
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			wchar_t *pszMsg: ������Ϣ
	* ��ʷ��¼:	ԭʼ�汾
	***********************************************************************/
	virtual void OnRespRightTip(bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* ��������:	OnRespMsgTip
	* ��������:	��Ϣ��ʾ����
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			wchar_t *pszMsg: ������Ϣ
	* �������:	NA
	* �� �� ֵ:	NA
	* ����˵��:	NA
	* ��ʷ��¼:	ԭʼ�汾
	***********************************************************************/
	virtual void OnRespMsgTip(bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* ��������:	OnRespUserDealRecord
	* ��������:	�û���Ϊ��¼����
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			wchar_t *pszMsg: ���ص���Ϣ
	***********************************************************************/
	virtual void OnRespUserDealRecord(bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* ��������:	OnRespQueryServerInfo
	* ��������:	��ѯ�������г������Ϣ����
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			wchar_t *pszMsg: ������Ϣ
	***********************************************************************/
	virtual void OnRespQueryServerInfo(bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* ��������:	OnRespAdvInfo
	* ��������:	�����Ϣ����
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			wchar_t *pszMsg: ������Ϣ
	***********************************************************************/
	virtual void OnRespAdvInfo(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* ��������:	OnRespTextBannerConfig
	* ��������:	�ı���������
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			wchar_t *pszMsg: ������Ϣ
	***********************************************************************/
	virtual void OnRespTextBannerConfig(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* ��������:	OnRespReportTabList
	* ��������:	���۱�ײ�tab�б���
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			wchar_t *pszMsg: ������Ϣ
	***********************************************************************/
	virtual void OnRespReportTabList(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* ��������:	OnRespToolbarFile
	* ��������:	�����������ļ�����
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			wchar_t *pszMsg: ������Ϣ
	***********************************************************************/
	virtual void OnRespToolbarFile(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* ��������:	OnRespSaveCusoptional
	* ��������:	�ϴ���ѡ�ɷ���
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			wchar_t *pszMsg: ������Ϣ
	***********************************************************************/
	virtual void OnRespSaveCusoptional(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* ��������:	OnRespQueryCusoptional
	* ��������:	������ѡ�ɷ���
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			wchar_t *pszMsg: ������Ϣ
	***********************************************************************/
	virtual void OnRespQueryCusoptional(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* ��������:	OnEconoData
	* ��������:	�յ��ƾ���������Ӧ����
	* �������:	pszRecvData �յ��Ĳƾ���������
	***********************************************************************/
	virtual void OnEconoData(const char *pszRecvData) = 0;

	/**********************************************************************
	* ��������:	OnRespStrategyData
	* ��������:	��ȡ�����źŷ���
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			const char *pszRecvData:���ص�����
	***********************************************************************/
	virtual void OnRespStrategyData(const char *pszRecvData) = 0;

	/**********************************************************************
	* ��������:	OnRespPickModelTypeInfo
	* ��������:	������������ѡ�����͵�ѡ���б�
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			const char *pszRecvData:���ص�����
	***********************************************************************/
	virtual void OnRespPickModelTypeInfo(const char *pszRecvData) = 0;

	/**********************************************************************
	* ��������:	OnRespPickModelTypeStatus
	* ��������:	������������ѡ������ѡ��״̬���Ƿ���ѡ��
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			const char *pszRecvData:���ص�����
	***********************************************************************/
	virtual void OnRespPickModelTypeStatus(const char *pszRecvData) = 0;

	/**********************************************************************
	* ��������:	OnRespQueryNewStockInfo
	* ��������: �������к�δ���еĹ�Ʊ�б�
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			const char *pszRecvData:���ص�����
				const bool bListedStock:�Ƿ��������¹�
	***********************************************************************/
	virtual void OnRespQueryNewStockInfo(const char *pszRecvData, const bool bListedStock) = 0;

	/**********************************************************************
	* ��������:	OnRespCheckToken
	* ��������:	�յ�token��֤���
	* �������:	const char *pszRecvData:���ص�����
	***********************************************************************/
	virtual void OnRespCheckToken(const char *pszRecvData) = 0;

		/**********************************************************************
	* ��������:	OnRespPushMsgList
	* ��������:	�յ�������Ϣ�б�
	* �������:	const char *pszRecvData:���ص�����
	***********************************************************************/
	virtual void OnRespPushMsgList(const char *pszRecvData) = 0;

		/**********************************************************************
	* ��������:	OnRespPushMsgDetail
	* ��������:	�յ�������Ϣ����
	* �������:	const char *pszRecvData:���ص�����
	***********************************************************************/
	virtual void OnRespPushMsgDetail(const char *pszRecvData) = 0;
};

#endif