#ifndef _PATH_FACTORY_H_
#define _PATH_FACTORY_H_

#include "typedef.h"

// ·������
// �����ļ���·������������֯�� �Ͻ�������ֱ��д�κ�·���� ���Եġ���ԵĶ�������
// ע���ѻ����ݵ�Ŀ¼�ṹ�����������, ��ο�fileengine dll�������
class AFX_EXT_CLASS CPathFactory
{
public:	
	static CString	GetRootPath();					// �����Ŀ¼�� ��Ŀ¼����exe��dll�� �Լ�date.ini�ļ�
	// ����Ŀ¼
	static CString	GetPublicConfigPath();						// ���������ļ�Ŀ¼
	static CString	GetPublicWorkspacePath();					// ���������ļ�Ŀ¼

	// ����ͼƬ
	static CString  GetImagePath();								// ��ȡͼƬ�ļ�Ŀ¼
	static CString  GetImageLogoFilePath();						// ��¼ͼƬ�ļ���
	static CString  GetImageTradeLogoFilePath();				// ���׵�¼ͼƬ�ļ���
	static CString	GetImageMainFrameBkGroundPath();			// MainFrame �ı���ͼƬ
	static CString	GetImageMainIcon48Path();					// ������ͼ��
	static CString	GetImageMainIcon32Path();					// ������ͼ��
	static CString  GetTradeIcoPath();                          //���״���logo
	static CString  GetTitleBmpPath();                          //���������bmp
	static CString  GetImageMainFrameTitlePath(bool bActive);	// ��ȡ�������±���ͼƬ(������Ǽ���ʱ)
	static CString  GetImageAlarmPath();						// ��ȡ����Ԥ��ͼƬ

	static CString  GetImageLoginCaptionPath();					// ��ȡ��¼�������������ͼ
	static CString	GetImageLoginLogoPath();					// ��ȡ��¼���������LOGO
	static CString	GetImageLoginTitlePicture();				// ��ȡ��¼���������ͼ��
	static CString  GetImageLoginSettingPath();					// ��ȡ��¼�������ð�ť
	static CString  GetImageLoginClosePath();					// ��ȡ��¼����رհ�ť
	static CString  GetImageLoginTopPath();						// ��ȡ��¼��������ͼƬ
	static CString  GetImageLoginBottomPath();					// ��ȡ��¼�����¼��ͼƬ
	static CString  GetImageLoginConnectPath();					// ��ȡ��¼�����¼��ťͼƬ
	static CString  GetImageLoginCancelPath();					// ��ȡ��¼����ȡ����¼ͼƬ
	static CString  GetImageLoginCheckPath();					// ��ȡ��¼���渴ѡ��ͼƬ
	static CString  GetImageLoginUnCheckPath();					// ��ȡ��¼���渴ѡ��ûѡ��ͼƬ
	static CString  GetImageNotifyCheckPath();					// ֪ͨ���渴ѡ��
	static CString  GetImageNotifyUnCheckPath();				// ֪ͨ���渴ѡ��ûѡ��ͼƬ
	//
	static CString	GetImageMenuBarBtnPath();					// ��ȡ�˵�����ťͼƬ
	static CString	GetImageMenuItemPath();						// �˵����ͼƬ
	static CString	GetImageMenuSeparatorPath();				// �˵���ָ�����ͼƬ
	//
	static CString  GetImageKLinePath(); //kline���ͼ
	static CString  GetImageBtnPaht();   //��ť���ͼ

	// �������ŵ�����
	static CString	GetImageTabOutPath();			// ������tabͼƬ
	static CString	GetImageTabTitleBKPath();		// ������tab����
	static CString	GetImageTabLogoPath();			// LogoͼƬ
	static CString	GetImageTabSysMenuPath();		// ϵͳ��ť
	static CString	GetImageTabPreBtnPath();		// ǰһ��
	static CString	GetImageTabNextBtnPath();		// ��һ��

	static CString	GetImageTBBkImagePath();		// ����������ͼƬ

	static CString	GetImageWelcomePath();			// ��ӭҳͼƬ
	static CString	GetImageQRCodeImgPath();		// ��ά��ͼƬ
	static CString	GetImageAboutImgPath();			// ���ڶԻ���ͼƬ
	static CString	GetImageTradeBgImgPath();		// ģ�⽻�׵�¼���汳��ͼƬ

	// ����
	static CString  GetFontPath();					// ��������Ŀ¼

	// ģ��
	static CString	GetTemplatePath();							// ��ȡģ��Ŀ¼

	// �����Ѵ��������ļ���� ��˽��Ŀ¼�����û��ָ���û����߲����ڵ�¼���û�����ֱ�����빫��Ŀ¼����Ѱ
	// �����Ƿ���ڸ��ļ����п��ܲ�����- -, ������Ϊʵ�ʴ��ڵ��ļ�·��
	static bool  GetExistConfigFileName(OUT CString &StrFileFind, const CString &StrFileName, const CString &StrUserName = _T(""));
	// ��Ϊ�ϸ��������ױ���
	// ��������û����򷵻ص����û�˽�������ļ�������������ڣ��򷵻ع���Ŀ¼�ļ���
	static CString GetSaveConfigFileName(const CString &StrFileName, const CString StrUserName = _T(""));	

	// ������Ҫ�õ���wspĿ¼, wsp�ϳ�wsp�б�ȵ���Ҫ����
	// TODO
	
	// ��ǰ�û���˽��Ŀ¼, Ĭ�ϲ���Ҫ���û����� ȡ��ǰ�û����� ��δ��½ʱ�� ����ȡ�û���
	static CString	GetPrivateRootPath(const CString &StrUserName);			// �û�˽�и�Ŀ¼
	static CString	GetPrivateConfigPath(const CString &StrUserName);			// �û�˽�������ļ�Ŀ¼
	static CString	GetPrivateWorkspacePath(const CString &StrUserName);		// �û�˽�а����ļ�Ŀ¼
	static CString	GetPrivateExcelPath(const CString &StrUserName);			// �û�˽�е���Excel �ļ�Ŀ¼
	static CString	GetPrivateTradePath(const CString &StrUserName);			// �û�˽�н��������ļ�Ŀ¼

	// ���õ������ļ���
	static CString  GetSysConfigFileName();
	static CString	GetIndexFileName();
	static CString	GetSysBlocksFileName();
	static CString	GetRelativeMerchsFileName();
	static CString	GetHkWarrantFileName();

	//////////////////////////////////////////////////////////////////////////
	// fan�����ļ�·��
	static CString  GetGridHeadFilePath(const CString& StrUserName);
	static CString  GetGridHeadFileName(); // ����������
	static CString	GetServerInfoPath();
	static CString  GetSortServerPath();
	static CString	GetUserInfoPath();
	static CString	GetTradeUserInfoPath();
	static CString	GetProxyInfoPath(const CString& StrUserName);
	static CString	GetAlarmMerchPath(const CString& StrUserName);
	static CString	GetCustomAlarmPath(const CString& StrUserName);
	static CString   GetIoViewFacePath(const CString& StrUserName);
	static CString	GetIoViewReportExcelPath();
	static CString	GetSysConfigFilePath();
	static CString	GetToolBarConfigFilePath();
	static CString	GetMarketInfoFilePath();
	static CString	GetIntervalTrendPath(const CString& StrUserName);
	static CString	GetEssenceHttpInfoPath();
	static CString	GetArbitragePath(const CString& StrUserName);
	static CString	GetArbitragePublicPath();
	static CString	GetLoginBackUpPath(const CString& StrUserName);
	static CString	GetLoginErrServerPath();
	static CString	GetCaptionConfigFilePath(); // ��ȡ��������˵������ļ�
	//��ȡ��Ϣ����֪ͨ��ģ���ļ�
	static CString  GetPublicNotifyMsgTemp();

	//////////////////////////////////////////////////////////////////////////
	// cui�����ļ�·��

	static CString	GetSysIndexFileFullName();
	static CString	GetSysIndexParamsFileFullName(const CString &StrUserName);

	static CString	GetModifyIndexFileFullName(const CString &StrUserName);
	static CString	GetModifyIndexParamsFileFullName(const CString &StrUserName);

	static CString	GetUserIndexFileFullName(const CString &StrUserName);
	static CString	GetUserIndexParamsFileFullName(const CString &StrUserName);
	
	static CString	GetFlagIndexFileFullName();

	static CString	GetSelfDrawFileFullName(const CString &StrUserName);

	static CString	GetSysBlocksFileFullName();

	static CString	GetUserBlocksFileFullName(const CString &StrUserName);
	static CString  GetUserBlocksPulbicFilePullName();	// �û���ѡ�����ļ�

	static CString  GetUserTradeCfgFileFullName(const CString &StrUserName);

	static CString	GetCustomBlocksFileFullName(const CString &StrUserName);
	static CString  GetCustomBlocksPulbicFilePullName();// �û��Զ��幫���ļ�

	static CString	GetUserNoteInfoFilePath(const CString& StrUserName);		// ��ȡ�û�Ͷ�ʱʼ�Ŀ¼

	static CString	GetPublicTmpFilePath();			// ��ȡ��������ʱĿ¼

	static CString	GetChouMaParamFullName(const CString& StrUserName);

	static CString	GetIoViewFaceFileName();

	static CString	GetOfflineDataPath();		// ��������·��
	static CString	GetFundHoldFileFullName();	// �عɳֲ��ļ�·��

	//////////////////////////////////////////////////////////////////////////
	// ���·��תΪ����·��
	static CString	ComPathToAbsPath(const CString& StrComPath);

};

#endif