#ifndef _ICONFIGNOTIFY_H_
#define _ICONFIGNOTIFY_H_

#define IN
#define OUT

#include <tchar.h>

class iConfigNotify
{
public:

	/**********************************************************************
	* ��������:	OnRespAdvInfo
	* ��������:	���������Ϣ����
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			wchar_t *pszMsg: ������Ϣ
	* �������:	NA
	* �� �� ֵ:	NA
	* ����˵��:	NA
	* ��ʷ��¼:	ԭʼ�汾
	***********************************************************************/
	virtual void OnRespAdvInfo(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* ��������:	OnRespCommTextConfig
	* ��������:	ͨ�õ��ı����÷���
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			wchar_t *pszMsg: ������Ϣ
	* �������:	NA
	* �� �� ֵ:	NA
	* ����˵��:	NA
	* ��ʷ��¼:	ԭʼ�汾
	***********************************************************************/
	virtual void OnRespCommTextConfig(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* ��������:	OnRespReportTabList
	* ��������:	���۱�ײ�tab�б���
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			wchar_t *pszMsg: ������Ϣ
	* �������:	NA
	* �� �� ֵ:	NA
	* ����˵��:	NA
	* ��ʷ��¼:	ԭʼ�汾
	***********************************************************************/
	virtual void OnRespReportTabList(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* ��������:	OnRespToolbarFile
	* ��������:	�����������ļ�����
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			wchar_t *pszMsg: ������Ϣ
	* �������:	NA
	* �� �� ֵ:	NA
	* ����˵��:	NA
	* ��ʷ��¼:	ԭʼ�汾
	***********************************************************************/
	virtual void OnRespToolbarFile(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* ��������:	OnRespDownloadUserData
	* ��������:	�û�˽���������ط���
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			wchar_t *pszMsg: ������Ϣ
	* �������:	NA
	* �� �� ֵ:	NA
	* ����˵��:	NA
	* ��ʷ��¼:	ԭʼ�汾
	***********************************************************************/
	virtual void OnRespDownloadUserData(IN bool bSucc, IN const wchar_t *pszMsg) = 0;

	/**********************************************************************
	* ��������:	OnRespUploadUserData
	* ��������:	�û�˽�������ϴ�����
	* �������:	bSucc , true �ɹ�;falseʧ��
	*			wchar_t *pszMsg: ������Ϣ
	* �������:	NA
	* �� �� ֵ:	NA
	* ����˵��:	NA
	* ��ʷ��¼:	ԭʼ�汾
	***********************************************************************/
	virtual void OnRespUploadUserData(IN bool bSucc, IN const wchar_t *pszMsg) = 0;
};

#endif