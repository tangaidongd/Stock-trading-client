#ifndef _WSP_MANAGER_H_
#define _WSP_MANAGER_H_

#include <map>
using std::map;

class CMPIChildFrame;


//////////////////////////////////////////////////////////////////////////
// �޴�Сд�ִ�
class CStringLessNoCase
{
public:
	bool operator()(const CString &S1, const CString &S2) const
	{
		return S1.CollateNoCase(S2) < 0;
	}
};


//////////////////////////////////////////////////////////////////////////
// �����ƣ����ڽ���������ҳ��
// typedef struct T_WspFileInfo
// {
// 	CString		m_StrFilePath;			// �ļ�·��			 (ȫ·��)
// 	CString		m_StrFileName;			// �ļ���			 (�������ļ���)
// 	CString		m_StrFileVersion;		// �ļ��汾			 (�汾�ŵ�һλ����100����Ϊ���⹤���������������һλ�Ĵ�С)
// 	// ����û�������ϵͳ��������������û�˽��Ŀ¼�´���һ���µ�ͬ��������
// 	// �Ժ����������ϵͳ�Ĺ���������汾��������ͬ���������⹤�����İ汾��������
// 	CString		m_StrFileXmlName;		// Xml ����������(������ʾ���û����ܽӴ����Ĺ��������ƣ������ظ�)
// 	// ��ʾ�����ظ��϶�:
// 	//     1. �û��������������û���������wspͬ��
// 	//	   2. �½����û����������������й������������������Ǹ���
// 	//	   3. ���Ĺ���������Ϊ��ɾ����ǰ���û�������(�����)�� �ٽ����ڵĹ���������Ϊָ�������������ĵ�ǰ������Ϊָ��������
// 	//	   4. ������½�xml�����������ļ���ͬ����������򱣴��ļ�Ϊxml_�ļ���
// 	// �����������ķ�ʽ��:
// 	//     1. �������(�������û�������)
// 	//		2. �����������ķ�ʽ(����ͬ��������, ��Ҫ����)
// 	bool32		m_bReadOnly;			// ֻ�����ԣ�Ĭ�Ϸ�ֻ��(false) �߱������ԵĲ��ܱ��û��Լ����棬�һ����û����������Ƴ�ͻ
// 	bool32		m_bSystemWspDirHas;			// ϵͳ������Ŀ¼���Ƿ�����ù�����(Ĭ��false)
// 	bool32		m_bInUserPrivateDir;			// �ù������ļ��Ƿ����û�˽��Ŀ¼����(Ĭ��true)
// 	
// 	T_WspFileInfo();
// 	
// 	bool32		IsSpecialWsp() const ;			// �Ƿ�Ϊ���⹤����
// 	CString		GetNormalWspFileVersion() const;	// ��ȡ�ļ��汾 - ���⹤�������ļ��汾���ܸ���, so ʹ�øú������´���
// }T_WspFileInfo;

// ����ҳ�� - ����childframe

typedef struct T_CfmFileInfo
{
	CString		m_StrFilePath;			// �ļ�·��			 (ȫ·��)(��ʱ����id��־)
	CString		m_StrFileName;			// �ļ���			 (�������ļ���) - ��ʾ������Ϊ�ļ���ȥ����׺������(!!!)
	CString		m_StrXmlName;			// �ļ���ȥ����׺, file title
	CString		m_StrFileVersion;		// �ļ��汾			 
	int32		m_iUpdateMode;			// ����ģʽ�����Ϊ1����ǿ�ƽ�˽��Ŀ¼�²�ͬ�汾(!!!)��ҳ��ɾ��, Ϊ0�򲻴���
	bool32		m_bLockSplit;			// ǿ�������ָ���ô˱�־��ҳ�汻����ǿ��������־���������и��
	bool32		m_bSecret;				// ����ҳ�棬�����浽���������������ڴ��б���, ֻ���ɹ����ṩ�򿪵�ҳ��
	bool32		m_bReadOnly;			// ֻ���ģ��������涯��

	bool32		m_bInUserPrivateDir;	// λ���û�˽��Ŀ¼��
	bool32		m_bSystemDirHas;		// ϵͳĿ¼����Դ
	bool32		m_bNeedServerAuth;		// ��Ҫ��������Ȩ��cfm����ʱ���Ƿ�������������Ȩ�б�

	int32		m_iSystemShow;			// �������Ƿ���ʾ��cfm, 0����ʾ�� ���մ�С�����ڹ�����������ʾ

	bool32		m_bFromSystem;			// cfm�Ƿ��ϵͳcfm�����ģ�����һ�Σ�������0

	bool32      m_bNeedReload;			// �ٴδ�cfm�Ƿ���Ҫ���¼���

	T_CfmFileInfo();
}T_CfmFileInfo;

enum E_ToXmlType		// ����xml�ļ�����
{
	ETXT_Wsp = 0,	// ������
	ETXT_Cfm,		// ����ҳ��

	ETXT_Count
};

// ���� ������/ҳ�� ����, ��������ܲ��ã���Ӧ��Ƶ������ (�Ƚ�����ҳ��)
// �Ƿ�Ӧ�����������
class CCfmManager
{
public:
	//typedef CMap<CString, const CString &, T_CfmFileInfo, const T_CfmFileInfo &> CfmInfoMap;
	typedef map<CString, T_CfmFileInfo, CStringLessNoCase> CfmInfoMap;
	typedef map<CString, CString>	UserLastCfmMap;

	enum E_ConflictType
	{
		EWCT_None = 0,		// 0��ͻ
		EWCT_CanOverwrite,	// 1 �ɸ��ǳ�ͻ
		EWCT_Conflict,		// �Ѿ���ͻ�ˣ����ɽ��
	};

	enum E_CfmUserRight		// cfm��Ӧ���û��ɲ���Ȩ��
	{
		ECUR_Open = 0x1,		// �ɴ򿪣�����ҳ��Ӧ�ö��ǿɴ򿪵�(���ڲ�����ļ�����)
		ECUR_ShowInList = 0x2,	// ����ʾ���б��У�secret�Ĳ���
		ECUR_Delete = 0x4,		// ��ɾ�������û��ķ�Ĭ�ϵĿ�ɾ��
		ECUR_Restore = 0x8,		// �ɻָ���ϵͳĿ¼���ж�Ӧ�Ŀɻָ�(���Ǵ��û���ҳ��), ������Ĭ�ϵ�Ҳ���Իָ�
		ECUR_Write	= 0x10,		// ��д����readonly
	};

	enum E_SystemDefaultCfm		// ϵͳĬ�ϵļ���cfm
	{
		ESDC_Report,			// �����б�
		ESDC_KLine,				// K�߷���
		ESDC_Trend,				// ��ʱ����
		ESDC_News,				// �Ƹ���Ѷ
		ESDC_PhaseSort,			// �׶�������ͼ, �׶�������ͼ�����漰�����¼�������⣬���Բ��ر�
		ESDC_KlineArbitrage,	// ����k��
		ESDC_TrendArbitrage,	// ������ʱ
		ESDC_ReportArbitrage,	// �������۱�
		ESDC_HomePage,			// ��ҳ

		ESDC_Count
	};

	struct T_CfmInfo
	{
		CString StrCfmName;
		CString StrCfmUrl;

		T_CfmInfo()
		{
			StrCfmName = L"";
			StrCfmUrl = L"";
		}
	};

	typedef CArray<T_CfmInfo, T_CfmInfo&>  CfmInfoArray;

	static CCfmManager &Instance();


	~CCfmManager();

	// ��ʼ���û��Ĺ���ҳ����Ϣ, �����򿪺��ʵĹ���ҳ��
	void	Initialize(const CString &StrUserName);	
	bool32	IsInitialized() const;

	// ����ʹ��ҳ����Ϣ�ĵط���������ѯʹ��
	void	ScanUserAllCfm(OUT CfmInfoMap &cfmInfos, OUT CfmInfoMap &cfmInfoSys, OUT CfmInfoMap &cfmInfoUser, LPCTSTR pszUserName = NULL) const ;	// ɨ��ָ���û������й���ҳ�棬������
	void	ScanUserNormalCfm(OUT CfmInfoMap &cfmInfos, LPCTSTR pszUserName = NULL) const ; // ɨ��һ��Ĺ���ҳ�棬���ⱻ����, ������

	bool32	QueryUserCfm(const CString &StrXmlName, OUT T_CfmFileInfo &cfmInfo) const ; // �����ȳ�ʼ��, ȫ������ҳ�����
	void	QueryUserSystemCfm(OUT CfmInfoMap &cfmInfos) const;	// ��ȡ�û���ϵͳ����ҳ��(�������⹤��ҳ��), ����û��޸ı��������ʹ���û����滻ϵͳ��
	void	QueryUserPrivateCfm(OUT CfmInfoMap &cfmInfos) const;	// ��ȡ�û�˽�й���ҳ���б�(������ϵͳ��)
	void	QueryUserAllCfm(OUT CfmInfoMap &cfmInfos) const;	// ��ȡ�û�ȫ���Ĺ���ҳ���б�˽�еĸ���ϵͳ��
	//		���û��ָ������ʹ���Ѿ���ʼ�����û��������Ҳû�г�ʼ���û����򷵻����еĹ���Ŀ¼�µĹ�����

	bool32	QuerySysCfm(const CString &StrCfmName, OUT T_CfmFileInfo &cfmInfo);	// ��ϵͳĿ¼�µ�cfm����

	// ����Ĭ�Ϲ���ҳ�棬��һ���б�ά�������ұ��浽�û�˽�е�config�µ�һ��xml�ļ�,  ����Ժ����ÿ�������Ĭ�ϣ�������
	// ��initʱ���أ�ÿ��setʱ����(Ӧ�����Ǻ�Ƶ��)
	bool32	GetUserLastSelCfm(OUT T_CfmFileInfo &cfmInfo, LPCTSTR pszUserName = NULL) const;	// ��ȡ�û������ѡ���Ĭ�Ϲ���ҳ��
	void	SetUserLastSelCfm(const CString &StrXmlName, const CString &StrUserName);	// �����û�ѡ������ѡ��Ĭ�Ϲ���ҳ��

	void	RefreshCfm();		// ����ˢ�¹������б�, �����Ѿ���ʼ�������û���

	E_ConflictType  IsNewXmlNameConflict(const CString &StrXmlName) const;			// �½���Xml�����Ƿ����Ѵ��ڵ����Ƴ�ͻ, �½�xml��������������ڵ����Ƴ�ͻ
	bool32	IsNewFileNameConflict(const CString &StrFileName) const;			// �½����ļ������Ƿ����Ѵ��ڵ��ļ���ͻ, ���ж��û�˽��


	// Ҫ����ͼ(mainframe)������
	const T_CfmFileInfo &GetUserDefaultCfm() const { return m_cfmUserDefault; };
	bool32  IsUserDefaultCfm(const CString &StrXmlName) const { return m_cfmUserDefault.m_StrXmlName.CompareNoCase(StrXmlName) == 0; }
	bool32  SetUserDefaultCfm(const CString &StrXmlName);	// һ�������������Ͳ��ܸı�, �˴ν�Ӱ���Ժ�


	DWORD	GetCfmUserRights(const T_CfmFileInfo &info) const;	// ��ȡ��cfm��Ӧ�Ŀɲ���Ȩ��
	DWORD	GetCfmUserRights(const CString &StrXmlName) const;	// ��������ڵĻ�������0

	// ��������: �������ͬ���û���������ʹ�ô��ڵ�wsp�����滻,ɾ���û�������, �����µĹ�����
	bool32	AddNewUserCfm(const CString &StrXmlName, bool32 bUserCfm=true);	// ����µ��û�ҳ���¼, ʵ�����к�ֻ���ô��½��û�ҳ��
	void	DelUserCfm(const CString &StrXmlName,bool bNoCheckRestore = false);		// ɾ���û�ҳ���¼&�ļ�
	// ��涯��: �����ͬ������������ɾ��ͬ����������Ȼ��ʹ�õ�ǰ���������ݱ��浽ָ��xml
	void	CurCfmSaveAs(const CString &StrSaveAsXmlName);	// ����ǰҳ�����Ϊ, ��ҳ����Ŀ��

	bool32	ModifyUserCfmAttri(const T_CfmFileInfo &cfmInfo);	// �޸��û�˽��ҳ��Ĳ������ԣ����в������Իᱻ���ģ����ĵ����Բ�������Ч��Ҳ����������

	// ����xml���ݣ����� ���ü���ҳ���fromxml toxml
	CMPIChildFrame *LoadCfm(const CString &StrXmlName, bool32 bCloseExistence = false, bool32 bHideNewCfm=false, CString StrCfmUrl = L"", bool bDelete = false); // �����´��ڲ�fromxml
	bool32	LoadCurCfm();			// ���¼��ص�ǰҳ�棬�����޸�(ʵ��Ϊ�رյ�ǰҳ������¼���)
	void	SaveCurrentCfm();		// ���浱ǰҳ�����󶨵����ԣ�����Ҫ�Ѿ�����ҳ�����Ե�
	// ����: ���ָ��·��Ϊ���û�·��������Ϊ�û�·������
	void	SaveCfm(const CString &StrXmlName, CMPIChildFrame *pSrcFrame);	// ���浱ǰ�Ĺ���ҳ����ָ��ҳ���ļ�(�½����߸���), Ŀ�깤��ҳ�����Ա�����add����

	void	BindCfm2ChildFrame(const CString &StrXmlName, CMPIChildFrame *pChildFrame, bool32 bBind = true);

	CMPIChildFrame *GetCurrentCfmFrame(); // ��ȡ��ǰchildframe�������cfm�󶨣��򷵻ش�cf������NULL
	CMPIChildFrame *GetCfmFrame(const CString &StrXmlName);	// ��ȡ��cfm�󶨵Ĵ���
	CMPIChildFrame *GetUserDefaultCfmFrame();		// �û�ѡ���Ĭ��ҳ�洰��
    void			ChangeIoViewStatus(CMPIChildFrame *pChild); //�����л�ʱ���ı���ͼ��ʾ������״̬
	int32			CloseCfmFrame(const CString &StrXmlName, bool32 bSaveCfm = false); // �ر���cfm�󶨵Ĵ���
	bool32			IsCurrentCfm(const CString &StrXmlName); // ��ǰcfm�Ƿ�Ϊ��cfm
	bool32			IsCfmExist(const CString &StrXmlName);	// �Ƿ���ڸ�cfm

	void			GetReserveCfmNames(OUT CStringArray &aReserveName);
	bool32			IsReserveCfm(const CString &StrXmlName);	// �Ƿ�����ҳ��
	int32			CloseOtherSameClassCfm(const CString &StrXmlNameReserve);	// ����ָ����cfm���ر����д򿪵�ͬ��cfm(ϵͳ�����û�)!!!
	static void SortByFileTime(const CfmInfoMap &wspInfos, OUT CStringArray &aCfmXmlNames);	// �����ļ����޸�ʱ������������޸ĵ���ǰ
	static void RelativePathToAbsPath(const CString &StrRelative, OUT CString &StrAbs);

	static void	GetSystemDefaultCfmNames(OUT CStringArray &aNames);
	static bool32 GetSystemDefaultCfmName(IN E_SystemDefaultCfm esdc, OUT CString &StrName);
	static E_SystemDefaultCfm GetESDC(const CString &StrName);

	static CString	GetProperFileExt(bool32 bUserCfm);	// ��ȡ���ʵ��ļ���׺�����û���Ϊcfu��ϵͳ��Ϊcfm, ����ǰ���.

	void	GetOpenedCfm(CfmInfoArray &aStrCfm);
	void	SetOpenedCfm();
	void	ReMoveAllOpendCfm();
	void	ChangeOpenedCfm(CString strCfmName, CString strCfmUrl = L"");
	void	AddOpenedCfm(CString strCfmName, CString strCfmUrl = L"");
	
protected:
	CCfmManager();


	bool32	ReadCfmInfoFromFile(const CString &StrFilePath, OUT T_CfmFileInfo &cfmInfo, bool32 bSureSysteFile = false) const;	// ��ȡָ��ҳ���ļ���Ϣ

	void	LoadPublicLastCfmList();	// ���ع���Ŀ¼�µ�Ĭ�ϵ�����
	void	SavePublicLastCfmList();	// ���湫��Ŀ¼�µ�Ĭ�ϵ�����

	void	LoadPrivateLastCfmList(const CString &StrUserName, OUT UserLastCfmMap &);	// �����û��Լ�������, û��ʵ��
	void	SavePrivateLastCfmList(const CString &StrUserName, OUT UserLastCfmMap &);	// �����û��Լ�Ĭ�ϵ�����

	// ���û��ڲ�����
	int		CheckCfmCanAdd(INOUT T_CfmFileInfo &wspInfo, const CfmInfoMap &infoMap, bool32 bEnableRename = false) const;
	bool32	CheckCfmExist(const CString &StrXmlName, const CfmInfoMap &infoMap, OUT T_CfmFileInfo *pCfmInfo = NULL) const;

	void	UpdateUserCfm(const CString &StrXmlName, const T_CfmFileInfo &cfmInfo);

private:
	CString			m_StrUserName;			// ��ǰ�ɹ���¼����û���

	UserLastCfmMap  m_mapUserLastSelCfm;	// �û� - ���ѡ������ �б�

	T_CfmFileInfo	m_cfmUserDefault;		// �û�Ĭ�ϰ��棬���ܹر�

	CfmInfoMap	m_mapCfmInfoSystem;			// ���й�������ϵͳĿ¼���������֣��Թ�������ʾ����Ϊkey, �½�ʱ�����ظ�
	CfmInfoMap	m_mapCfmInfoUser;			// ���й��������û�˽��Ŀ¼����������

	CfmInfoArray	m_aStrOpenedCfm;		// ���������б�
};

#endif //_WSP_MANAGER_H_