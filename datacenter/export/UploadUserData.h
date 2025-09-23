#ifndef _IOVIEWUPLOADUSERSEL_H_
#define _IOVIEWUPLOADUSERSEL_H_
#include "typedef.h"
#include "DataCenterExport.h"

class CMmiReqUploadData;
class CMmiRespUploadData;
class CMmiReqDownloadData;
class CMmiRespDownloadData;
class CViewData;
// ����UserManager�����û���ѡ�����ݵ����س�ʼ�����ϴ���ѡ��

class DATACENTER_DLL_EXPORT CUploadUserDataListener
{
public:
	virtual ~CUploadUserDataListener();
	
	virtual	void	OnUserSelUploadResp(const CMmiRespUploadData *pResp) = 0;		// �ϴ����ݻش�� Ӧ���и�type����
	virtual	void	OnUserSelDownloadResp(const CMmiRespDownloadData *pResp) = 0;	// �������ݻش��
};

class TiXmlElement;
class TiXmlDocument;

class DATACENTER_DLL_EXPORT CUploadUserData
{
	//DECLARE_DYNAMIC(CUploadUserData);	// ��ʶ��
public:
	enum E_UploadDataType{
		EUDT_BEGIN = 0,
		EUDT_MobileUserSel = 1,			// �ֻ���ѡ����ֻ��SetAuthRightParam�ᵽ char *pUserDataType: ��ȡ�����û�˽������ 1�ֻ���ѡ, 2PC��ѡ��, Ҫ�����Ļ��ö��ŷֿ�
		EUDT_PCUserSel = 2,
		EUDT_END
	};

	static CUploadUserData *instance();		// ��ȡʵ��ָ��


	// 
	bool32	Initialize(CViewData *pViewData);		// ��֤�ɹ����˽�����ݲ��ֶ�ȡ���ݳ�ʼ��
	bool32	LoadDataFromViewData();					// ��viewdata�м�������

	bool32	ReqUploadData(const CMmiReqUploadData &uploadData);		// �����ϴ�ĳ����, δ��֤��ʼ�����������
	void	OnRespUploadData(const CMmiRespUploadData *pResp);		// �ϴ����ݻذ�֪ͨ

	bool32	ReqDownloadData(const CMmiReqDownloadData &downloadData);		// ��������ĳ����
	void	OnRespDownloadData(const CMmiRespDownloadData *pResp);		// �������ݻذ�֪ͨ

	// �����û���ѡ�ɴ������
	bool32	UploadUserBlock();		// �ϴ��û���ѡ������
	bool32  DownloadUserBlock();    // �����û���ѡ������

	//
	void	AddRespListener(CUploadUserDataListener *pListener);
	void	RemoveRespListener(CUploadUserDataListener *pListener);

protected:

	void	FireUploadDataListener(const CMmiRespUploadData *pResp);
	void	FireDownloadDataListener(const CMmiRespDownloadData *pResp);

private:
	bool32	ReadUserBlockFromServer(const char *pContent, int nContentLen);		// ��ѡ�����ݴӷ��������ĳ�ʼ��, len ָ�����ݳ���
	int     ParseUserData(const TiXmlElement * pEle);
private:
	CUploadUserData();		// ��ʵ��~

	CViewData		*m_pViewData;		// ͨ��Initialize��ʼ����viewData

	CArray<CUploadUserDataListener *, CUploadUserDataListener *> m_aListeners;
};

#endif //_IOVIEWUPLOADUSERSEL_H_