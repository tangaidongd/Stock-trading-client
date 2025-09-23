#ifndef __COMM_CONFIG_CLIENT_H__
#define __COMM_CONFIG_CLIENT_H__

#include "iConfigNotify.h"
#include <vector>

namespace commconfig
{
static const int g_iAuthVer = 6;	// DLL�汾
static const int MAXNAME = 64;
static const int MAXVALUE = 256;

#pragma pack(push, 1)
/////////////////////////////////////////////////////////////////////////////
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

// ͨ���ı�����
typedef struct _CommonTextConfig
{
	wchar_t wszServiceTel[MAXNAME];		// �ͷ��绰
	wchar_t wszStatusContent[1024];			// ״̬�������ı�

	_CommonTextConfig()
	{
		wszServiceTel[0]	= 0; 
		wszStatusContent[0] = 0;
	} 
}T_CommonTextConfig;

#pragma pack(pop)

class iServiceDispose
{
public:
	// ���ô˺������ͷ��ɴ�DLL���ص���Ҫdelete���ڴ�ָ��
	virtual void ReleaseData(void *pData) = 0;

	// ��ȡ�����Ϣ�б�, ���ص�ָ����Ҫ����ReleaseData��delete []
	virtual void GetAdvList(OUT T_AdvSet **pstAdv, OUT int &iCount) = 0;

	// ��ȡ���۱�tab�б�, ���ص�ָ����Ҫ����ReleaseData��delete []
	virtual void GetReportTabList(OUT T_ReportTabInfo **pstTabInfo, OUT int &iCount) = 0;

	// ��ȡͨ���ı�����
	virtual void GetCommonTextConfig(OUT T_CommonTextConfig &pstCommConfig) = 0;

	// ��ȡ�������ļ��汾
	virtual const char* GetToolBarVer() = 0;

	// ��ȡ�û�˽������
	virtual const char* GetUserData() = 0;

	// �������ļ��Ƿ���
	virtual bool IsToolBarModify() = 0;
};

class iCommConfgClient
{
public:

	// ���÷������ĵ�ַ
	virtual bool AddServer(const wchar_t * StrServerAdd) = 0;

	virtual void Release() = 0;

	// ��ȡ���ݽӿ�
	virtual iServiceDispose * GetServiceDispose() = 0;

	// ���ûص�
	virtual void SetNotifyObj(iConfigNotify *pNotifyObj) = 0;

	/**********************************************************************
	* ��������:	ReqAdvInfo
	* ��������:	������������Ϣ
	* �������:	strToken �Ựtoken
	*			strMdl ��Ҫ���ص�ģ������б�, ���ģ�飬���á�,�� ���
	* �������:	NA
	* �� �� ֵ:	true �ɹ�, false ʧ��
	* ��ʷ��¼:	ԭʼ�汾
	***********************************************************************/
	virtual bool ReqAdvInfo(const wchar_t *strToken, const wchar_t *strMdl) = 0;

	/**********************************************************************
	* ��������:	ReqCommTextConfig
	* ��������:	����ͨ�õ��ı�����
	* �������:	strToken �Ựtoken
	* �������:	NA
	* �� �� ֵ:	true �ɹ�, false ʧ��
	* ��ʷ��¼:	ԭʼ�汾
	***********************************************************************/
	virtual bool ReqCommTextConfig(const wchar_t *strToken) = 0;

	/**********************************************************************
	* ��������:	ReqReportTabList
	* ��������:	���󱨼۱�ײ�tab�б�
	* �������:	strToken �Ựtoken
	* �������:	NA
	* �� �� ֵ:	true �ɹ�, false ʧ��
	* ��ʷ��¼:	ԭʼ�汾
	***********************************************************************/
	virtual bool ReqReportTabList(const wchar_t *strToken) = 0;

	/**********************************************************************
	* ��������:	ReqToolbarFile
	* ��������:	���󹤾��������ļ�
	* �������:	strToken �Ựtoken
	*			strVer �汾��, û�пɲ���д
	* �������:	NA
	* �� �� ֵ:	true �ɹ�, false ʧ��
	* ��ʷ��¼:	ԭʼ�汾
	***********************************************************************/
	virtual bool ReqToolbarFile(const wchar_t *strToken, const wchar_t *strVer) = 0;

	/**********************************************************************
	* ��������:	ReqDownloadUserData
	* ��������:	���������û�˽������
	* �������:	strToken �Ựtoken
	*			strDataType ��������, 1 ��ʾ��ѡ������
	* �������:	NA
	* �� �� ֵ:	true �ɹ�, false ʧ��
	* ��ʷ��¼:	ԭʼ�汾
	***********************************************************************/
	virtual bool ReqDownloadUserData(const wchar_t *strToken, const wchar_t *strDataType) = 0;

	/**********************************************************************
	* ��������:	ReqUploadUserData
	* ��������:	���������û�˽������
	* �������:	strToken �Ựtoken
	*			strDataType ��������, 1 ��ʾ��ѡ������
	* �������:	NA
	* �� �� ֵ:	true �ɹ�, false ʧ��
	* ��ʷ��¼:	ԭʼ�汾
	***********************************************************************/
	virtual bool ReqUploadUserData(const wchar_t *strToken, const wchar_t *strDataType, const wchar_t *strData) = 0;
};

};

#endif
