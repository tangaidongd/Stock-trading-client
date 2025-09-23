#ifndef _IOVIEWUPLOADUSERSEL_H_
#define _IOVIEWUPLOADUSERSEL_H_
#include "typedef.h"
#include "DataCenterExport.h"

class CMmiReqUploadData;
class CMmiRespUploadData;
class CMmiReqDownloadData;
class CMmiRespDownloadData;
class CViewData;
// 辅助UserManager处理用户自选股数据的下载初始化，上传自选股

class DATACENTER_DLL_EXPORT CUploadUserDataListener
{
public:
	virtual ~CUploadUserDataListener();
	
	virtual	void	OnUserSelUploadResp(const CMmiRespUploadData *pResp) = 0;		// 上传数据回答包 应该有个type回来
	virtual	void	OnUserSelDownloadResp(const CMmiRespDownloadData *pResp) = 0;	// 下载数据回答包
};

class TiXmlElement;
class TiXmlDocument;

class DATACENTER_DLL_EXPORT CUploadUserData
{
	//DECLARE_DYNAMIC(CUploadUserData);	// 仅识别
public:
	enum E_UploadDataType{
		EUDT_BEGIN = 0,
		EUDT_MobileUserSel = 1,			// 手机自选？？只在SetAuthRightParam提到 char *pUserDataType: 获取哪种用户私有数据 1手机主选, 2PC自选股, 要两个的话用逗号分开
		EUDT_PCUserSel = 2,
		EUDT_END
	};

	static CUploadUserData *instance();		// 获取实例指针


	// 
	bool32	Initialize(CViewData *pViewData);		// 认证成功后从私有数据部分读取数据初始化
	bool32	LoadDataFromViewData();					// 从viewdata中加载数据

	bool32	ReqUploadData(const CMmiReqUploadData &uploadData);		// 请求上传某数据, 未认证初始化不允许调用
	void	OnRespUploadData(const CMmiRespUploadData *pResp);		// 上传数据回包通知

	bool32	ReqDownloadData(const CMmiReqDownloadData &downloadData);		// 请求下载某数据
	void	OnRespDownloadData(const CMmiRespDownloadData *pResp);		// 下载数据回包通知

	// 辅助用户自选股处理相关
	bool32	UploadUserBlock();		// 上传用户自选股数据
	bool32  DownloadUserBlock();    // 下载用户自选股数据

	//
	void	AddRespListener(CUploadUserDataListener *pListener);
	void	RemoveRespListener(CUploadUserDataListener *pListener);

protected:

	void	FireUploadDataListener(const CMmiRespUploadData *pResp);
	void	FireDownloadDataListener(const CMmiRespDownloadData *pResp);

private:
	bool32	ReadUserBlockFromServer(const char *pContent, int nContentLen);		// 自选股数据从服务器来的初始化, len 指明数据长度
	int     ParseUserData(const TiXmlElement * pEle);
private:
	CUploadUserData();		// 单实例~

	CViewData		*m_pViewData;		// 通过Initialize初始化的viewData

	CArray<CUploadUserDataListener *, CUploadUserDataListener *> m_aListeners;
};

#endif //_IOVIEWUPLOADUSERSEL_H_