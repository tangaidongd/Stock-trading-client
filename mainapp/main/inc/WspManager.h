#ifndef _WSP_MANAGER_H_
#define _WSP_MANAGER_H_

#include <map>
using std::map;

class CMPIChildFrame;


//////////////////////////////////////////////////////////////////////////
// 无大小写字串
class CStringLessNoCase
{
public:
	bool operator()(const CString &S1, const CString &S2) const
	{
		return S1.CollateNoCase(S2) < 0;
	}
};


//////////////////////////////////////////////////////////////////////////
// 待完善，现在仅用来处理页面
// typedef struct T_WspFileInfo
// {
// 	CString		m_StrFilePath;			// 文件路径			 (全路径)
// 	CString		m_StrFileName;			// 文件名			 (真正的文件名)
// 	CString		m_StrFileVersion;		// 文件版本			 (版本号第一位大于100的视为特殊工作区，不区别其第一位的大小)
// 	// 如果用户保存了系统工作区，则会在用户私有目录下存在一份新的同名工作区
// 	// 以后如果更新了系统的工作区，则版本号有所不同（存在特殊工作区的版本号有区别）
// 	CString		m_StrFileXmlName;		// Xml 中描述名称(用于显示，用户所能接触到的工作区名称，不能重复)
// 	// 显示名称重复认定:
// 	//     1. 用户工作区不能与用户工作区内wsp同名
// 	//	   2. 新建的用户工作区不能与所有工作区重名，如重则考虑覆盖
// 	//	   3. 另存的工作区动作为先删除以前的用户工作区(如果有)， 再将现在的工作区保存为指定工作区，更改当前工作区为指定工作区
// 	//	   4. 如果有新建xml名称与现有文件名同名的情况，则保存文件为xml_文件名
// 	// 新增工作区的方式有:
// 	//     1. 界面操作(会生成用户工作区)
// 	//		2. 界面操作以外的方式(会有同名工作区, 需要处理)
// 	bool32		m_bReadOnly;			// 只读属性，默认非只读(false) 具备该属性的不能被用户自己保存，且会与用户工作区名称冲突
// 	bool32		m_bSystemWspDirHas;			// 系统工作区目录下是否包含该工作区(默认false)
// 	bool32		m_bInUserPrivateDir;			// 该工作区文件是否在用户私人目录下面(默认true)
// 	
// 	T_WspFileInfo();
// 	
// 	bool32		IsSpecialWsp() const ;			// 是否为特殊工作区
// 	CString		GetNormalWspFileVersion() const;	// 获取文件版本 - 特殊工作区的文件版本不能复制, so 使用该函数做下处理
// }T_WspFileInfo;

// 工作页面 - 单个childframe

typedef struct T_CfmFileInfo
{
	CString		m_StrFilePath;			// 文件路径			 (全路径)(暂时用作id标志)
	CString		m_StrFileName;			// 文件名			 (真正的文件名) - 显示的名称为文件名去掉后缀的名称(!!!)
	CString		m_StrXmlName;			// 文件名去掉后缀, file title
	CString		m_StrFileVersion;		// 文件版本			 
	int32		m_iUpdateMode;			// 升级模式，如果为1，则强制将私有目录下不同版本(!!!)的页面删除, 为0则不处理
	bool32		m_bLockSplit;			// 强制锁定分割，设置此标志则页面被设置强制锁定标志，不允许切割窗口
	bool32		m_bSecret;				// 秘密页面，不保存到工作区，不出现在打开列表中, 只能由功能提供打开的页面
	bool32		m_bReadOnly;			// 只读的，不允许保存动作

	bool32		m_bInUserPrivateDir;	// 位于用户私有目录下
	bool32		m_bSystemDirHas;		// 系统目录下有源
	bool32		m_bNeedServerAuth;		// 需要服务器授权的cfm，打开时看是否存在与服务器授权列表

	int32		m_iSystemShow;			// 工具条是否显示该cfm, 0不显示， 按照从小到大在工具条左到右显示

	bool32		m_bFromSystem;			// cfm是否从系统cfm过来的，设置一次，永不清0

	bool32      m_bNeedReload;			// 再次打开cfm是否需要重新加载

	T_CfmFileInfo();
}T_CfmFileInfo;

enum E_ToXmlType		// 工作xml文件类型
{
	ETXT_Wsp = 0,	// 工作区
	ETXT_Cfm,		// 工作页面

	ETXT_Count
};

// 负责 工作区/页面 管理, 整体的性能不好，不应当频繁调用 (先仅工作页面)
// 是否应当加入监听？
class CCfmManager
{
public:
	//typedef CMap<CString, const CString &, T_CfmFileInfo, const T_CfmFileInfo &> CfmInfoMap;
	typedef map<CString, T_CfmFileInfo, CStringLessNoCase> CfmInfoMap;
	typedef map<CString, CString>	UserLastCfmMap;

	enum E_ConflictType
	{
		EWCT_None = 0,		// 0冲突
		EWCT_CanOverwrite,	// 1 可覆盖冲突
		EWCT_Conflict,		// 已经冲突了，不可解决
	};

	enum E_CfmUserRight		// cfm对应的用户可操作权限
	{
		ECUR_Open = 0x1,		// 可打开，所有页面应该都是可打开的(现在不检查文件存在)
		ECUR_ShowInList = 0x2,	// 可显示于列表中，secret的不能
		ECUR_Delete = 0x4,		// 可删除，纯用户的非默认的可删除
		ECUR_Restore = 0x8,		// 可恢复，系统目录下有对应的可恢复(即非纯用户的页面), 哪怕是默认的也可以恢复
		ECUR_Write	= 0x10,		// 可写，非readonly
	};

	enum E_SystemDefaultCfm		// 系统默认的几个cfm
	{
		ESDC_Report,			// 报价列表
		ESDC_KLine,				// K线分析
		ESDC_Trend,				// 分时走势
		ESDC_News,				// 财富资讯
		ESDC_PhaseSort,			// 阶段排行视图, 阶段排行视图由于涉及到重新计算的问题，所以不关闭
		ESDC_KlineArbitrage,	// 套利k线
		ESDC_TrendArbitrage,	// 套利分时
		ESDC_ReportArbitrage,	// 套利报价表
		ESDC_HomePage,			// 首页

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

	// 初始化用户的工作页面信息, 决定打开合适的工作页面
	void	Initialize(const CString &StrUserName);	
	bool32	IsInitialized() const;

	// 所有使用页面信息的地方尽量都查询使用
	void	ScanUserAllCfm(OUT CfmInfoMap &cfmInfos, OUT CfmInfoMap &cfmInfoSys, OUT CfmInfoMap &cfmInfoUser, LPCTSTR pszUserName = NULL) const ;	// 扫描指定用户的所有工作页面，不创建
	void	ScanUserNormalCfm(OUT CfmInfoMap &cfmInfos, LPCTSTR pszUserName = NULL) const ; // 扫描一般的工作页面，特殊被过滤, 不创建

	bool32	QueryUserCfm(const CString &StrXmlName, OUT T_CfmFileInfo &cfmInfo) const ; // 必须先初始化, 全部工作页面查找
	void	QueryUserSystemCfm(OUT CfmInfoMap &cfmInfos) const;	// 获取用户的系统工作页面(包含特殊工作页面), 如果用户修改保存过，则使用用户的替换系统的
	void	QueryUserPrivateCfm(OUT CfmInfoMap &cfmInfos) const;	// 获取用户私有工作页面列表(不属于系统的)
	void	QueryUserAllCfm(OUT CfmInfoMap &cfmInfos) const;	// 获取用户全部的工作页面列表，私有的覆盖系统的
	//		如果没有指定，则使用已经初始化的用户名，如果也没有初始化用户民，则返回现有的公共目录下的工作区

	bool32	QuerySysCfm(const CString &StrCfmName, OUT T_CfmFileInfo &cfmInfo);	// 找系统目录下的cfm配置

	// 关于默认工作页面，做一个列表维护，并且保存到用户私有的config下的一个xml文件,  如果以后设置可以设置默认，则设置
	// 于init时加载，每次set时保存(应当不是很频繁)
	bool32	GetUserLastSelCfm(OUT T_CfmFileInfo &cfmInfo, LPCTSTR pszUserName = NULL) const;	// 获取用户的最后选择的默认工作页面
	void	SetUserLastSelCfm(const CString &StrXmlName, const CString &StrUserName);	// 设置用户选择的最后选择默认工作页面

	void	RefreshCfm();		// 重新刷新工作区列表, 必须已经初始化好了用户名

	E_ConflictType  IsNewXmlNameConflict(const CString &StrXmlName) const;			// 新建的Xml名称是否与已存在的名称冲突, 新建xml名不能与任意存在的名称冲突
	bool32	IsNewFileNameConflict(const CString &StrFileName) const;			// 新建的文件名称是否与已存在的文件冲突, 仅判断用户私有


	// 要跟视图(mainframe)互动的
	const T_CfmFileInfo &GetUserDefaultCfm() const { return m_cfmUserDefault; };
	bool32  IsUserDefaultCfm(const CString &StrXmlName) const { return m_cfmUserDefault.m_StrXmlName.CompareNoCase(StrXmlName) == 0; }
	bool32  SetUserDefaultCfm(const CString &StrXmlName);	// 一旦程序启动，就不能改变, 此次仅影响以后


	DWORD	GetCfmUserRights(const T_CfmFileInfo &info) const;	// 获取该cfm对应的可操作权限
	DWORD	GetCfmUserRights(const CString &StrXmlName) const;	// 如果不存在的话，返回0

	// 创建动作: 如果存在同名用户工作区，使用存在的wsp属性替换,删除用户工作区, 创建新的工作区
	bool32	AddNewUserCfm(const CString &StrXmlName, bool32 bUserCfm=true);	// 添加新的用户页面记录, 实际运行后只有用此新建用户页面
	void	DelUserCfm(const CString &StrXmlName,bool bNoCheckRestore = false);		// 删除用户页面记录&文件
	// 另存动作: 如果有同名工作区，则删除同名工作区，然后使用当前工作区内容保存到指定xml
	void	CurCfmSaveAs(const CString &StrSaveAsXmlName);	// 将当前页面另存为, 绑定页面至目标

	bool32	ModifyUserCfmAttri(const T_CfmFileInfo &cfmInfo);	// 修改用户私有页面的部分属性，仅有部分属性会被更改，更改的属性不立即生效，也不立即保存

	// 处理xml内容！！！ 调用激活页面的fromxml toxml
	CMPIChildFrame *LoadCfm(const CString &StrXmlName, bool32 bCloseExistence = false, bool32 bHideNewCfm=false, CString StrCfmUrl = L"", bool bDelete = false); // 创建新窗口并fromxml
	bool32	LoadCurCfm();			// 重新加载当前页面，放弃修改(实际为关闭当前页面后重新加载)
	void	SaveCurrentCfm();		// 保存当前页面至绑定的属性，必须要已经绑定了页面属性的
	// 保存: 如果指定路径为非用户路径，则变更为用户路径保存
	void	SaveCfm(const CString &StrXmlName, CMPIChildFrame *pSrcFrame);	// 保存当前的工作页面至指定页面文件(新建或者覆盖), 目标工作页面属性必须先add好了

	void	BindCfm2ChildFrame(const CString &StrXmlName, CMPIChildFrame *pChildFrame, bool32 bBind = true);

	CMPIChildFrame *GetCurrentCfmFrame(); // 获取当前childframe，如果是cfm绑定，则返回此cf，否则NULL
	CMPIChildFrame *GetCfmFrame(const CString &StrXmlName);	// 获取该cfm绑定的窗口
	CMPIChildFrame *GetUserDefaultCfmFrame();		// 用户选择的默认页面窗口
    void			ChangeIoViewStatus(CMPIChildFrame *pChild); //版面切换时，改变视图显示或隐藏状态
	int32			CloseCfmFrame(const CString &StrXmlName, bool32 bSaveCfm = false); // 关闭与cfm绑定的窗口
	bool32			IsCurrentCfm(const CString &StrXmlName); // 当前cfm是否为该cfm
	bool32			IsCfmExist(const CString &StrXmlName);	// 是否存在该cfm

	void			GetReserveCfmNames(OUT CStringArray &aReserveName);
	bool32			IsReserveCfm(const CString &StrXmlName);	// 是否保留的页面
	int32			CloseOtherSameClassCfm(const CString &StrXmlNameReserve);	// 除了指定的cfm，关闭所有打开的同类cfm(系统或者用户)!!!
	static void SortByFileTime(const CfmInfoMap &wspInfos, OUT CStringArray &aCfmXmlNames);	// 根据文件的修改时间戳排序，最新修改的最前
	static void RelativePathToAbsPath(const CString &StrRelative, OUT CString &StrAbs);

	static void	GetSystemDefaultCfmNames(OUT CStringArray &aNames);
	static bool32 GetSystemDefaultCfmName(IN E_SystemDefaultCfm esdc, OUT CString &StrName);
	static E_SystemDefaultCfm GetESDC(const CString &StrName);

	static CString	GetProperFileExt(bool32 bUserCfm);	// 获取合适的文件后缀名，用户的为cfu，系统的为cfm, 包含前面的.

	void	GetOpenedCfm(CfmInfoArray &aStrCfm);
	void	SetOpenedCfm();
	void	ReMoveAllOpendCfm();
	void	ChangeOpenedCfm(CString strCfmName, CString strCfmUrl = L"");
	void	AddOpenedCfm(CString strCfmName, CString strCfmUrl = L"");
	
protected:
	CCfmManager();


	bool32	ReadCfmInfoFromFile(const CString &StrFilePath, OUT T_CfmFileInfo &cfmInfo, bool32 bSureSysteFile = false) const;	// 读取指定页面文件信息

	void	LoadPublicLastCfmList();	// 加载公共目录下的默认的配置
	void	SavePublicLastCfmList();	// 保存公共目录下的默认的配置

	void	LoadPrivateLastCfmList(const CString &StrUserName, OUT UserLastCfmMap &);	// 加载用户自己的配置, 没有实现
	void	SavePrivateLastCfmList(const CString &StrUserName, OUT UserLastCfmMap &);	// 保存用户自己默认的配置

	// 仅用户内部调用
	int		CheckCfmCanAdd(INOUT T_CfmFileInfo &wspInfo, const CfmInfoMap &infoMap, bool32 bEnableRename = false) const;
	bool32	CheckCfmExist(const CString &StrXmlName, const CfmInfoMap &infoMap, OUT T_CfmFileInfo *pCfmInfo = NULL) const;

	void	UpdateUserCfm(const CString &StrXmlName, const T_CfmFileInfo &cfmInfo);

private:
	CString			m_StrUserName;			// 当前成功登录后的用户名

	UserLastCfmMap  m_mapUserLastSelCfm;	// 用户 - 最后选择工作区 列表

	T_CfmFileInfo	m_cfmUserDefault;		// 用户默认版面，不能关闭

	CfmInfoMap	m_mapCfmInfoSystem;			// 所有工作区中系统目录工作区部分，以工作区显示名称为key, 新建时不能重复
	CfmInfoMap	m_mapCfmInfoUser;			// 所有工作区中用户私有目录工作区部分

	CfmInfoArray	m_aStrOpenedCfm;		// 具体数据列表
};

#endif //_WSP_MANAGER_H_