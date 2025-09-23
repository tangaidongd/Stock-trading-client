#ifndef _PATH_FACTORY_H_
#define _PATH_FACTORY_H_

#include "typedef.h"

// 路径工厂
// 所有文件的路径都从这里组织， 严禁在外面直接写任何路径， 绝对的、相对的都不可以
// 注：脱机数据的目录结构不在这里管理, 请参考fileengine dll相关内容
class AFX_EXT_CLASS CPathFactory
{
public:	
	static CString	GetRootPath();					// 如见根目录， 根目录下有exe和dll， 以及date.ini文件
	// 公共目录
	static CString	GetPublicConfigPath();						// 公共配置文件目录
	static CString	GetPublicWorkspacePath();					// 公共版面文件目录

	// 常用图片
	static CString  GetImagePath();								// 获取图片文件目录
	static CString  GetImageLogoFilePath();						// 登录图片文件名
	static CString  GetImageTradeLogoFilePath();				// 交易登录图片文件名
	static CString	GetImageMainFrameBkGroundPath();			// MainFrame 的背景图片
	static CString	GetImageMainIcon48Path();					// 主程序图标
	static CString	GetImageMainIcon32Path();					// 主程序图标
	static CString  GetTradeIcoPath();                          //交易窗口logo
	static CString  GetTitleBmpPath();                          //窗体标题栏bmp
	static CString  GetImageMainFrameTitlePath(bool bActive);	// 获取主窗口下标题图片(激活与非激活时)
	static CString  GetImageAlarmPath();						// 获取条件预警图片

	static CString  GetImageLoginCaptionPath();					// 获取登录界面标题栏背景图
	static CString	GetImageLoginLogoPath();					// 获取登录界面标题栏LOGO
	static CString	GetImageLoginTitlePicture();				// 获取登录界面标题栏图标
	static CString  GetImageLoginSettingPath();					// 获取登录界面设置按钮
	static CString  GetImageLoginClosePath();					// 获取登录界面关闭按钮
	static CString  GetImageLoginTopPath();						// 获取登录界面宣传图片
	static CString  GetImageLoginBottomPath();					// 获取登录界面登录框图片
	static CString  GetImageLoginConnectPath();					// 获取登录界面登录按钮图片
	static CString  GetImageLoginCancelPath();					// 获取登录界面取消登录图片
	static CString  GetImageLoginCheckPath();					// 获取登录界面复选框图片
	static CString  GetImageLoginUnCheckPath();					// 获取登录界面复选框没选中图片
	static CString  GetImageNotifyCheckPath();					// 通知界面复选框
	static CString  GetImageNotifyUnCheckPath();				// 通知界面复选框没选中图片
	//
	static CString	GetImageMenuBarBtnPath();					// 获取菜单栏按钮图片
	static CString	GetImageMenuItemPath();						// 菜单项的图片
	static CString	GetImageMenuSeparatorPath();				// 菜单项分割条的图片
	//
	static CString  GetImageKLinePath(); //kline广告图
	static CString  GetImageBtnPaht();   //按钮广告图

	// 金评新闻弹出框
	static CString	GetImageTabOutPath();			// 标题栏tab图片
	static CString	GetImageTabTitleBKPath();		// 标题栏tab背景
	static CString	GetImageTabLogoPath();			// Logo图片
	static CString	GetImageTabSysMenuPath();		// 系统按钮
	static CString	GetImageTabPreBtnPath();		// 前一个
	static CString	GetImageTabNextBtnPath();		// 下一个

	static CString	GetImageTBBkImagePath();		// 工具栏背景图片

	static CString	GetImageWelcomePath();			// 欢迎页图片
	static CString	GetImageQRCodeImgPath();		// 二维码图片
	static CString	GetImageAboutImgPath();			// 关于对话框图片
	static CString	GetImageTradeBgImgPath();		// 模拟交易登录界面背景图片

	// 字体
	static CString  GetFontPath();					// 特有字体目录

	// 模板
	static CString	GetTemplatePath();							// 获取模板目录

	// 加载已存在配置文件层次 先私有目录，如果没有指定用户或者不存在登录的用户，则直接跳入公共目录下找寻
	// 返回是否存在该文件，有可能不存在- -, 输出结果为实际存在的文件路径
	static bool  GetExistConfigFileName(OUT CString &StrFileFind, const CString &StrFileName, const CString &StrUserName = _T(""));
	// 作为上个函数配套保存
	// 如果存在用户，则返回的是用户私有配置文件名，如果不存在，则返回公共目录文件名
	static CString GetSaveConfigFileName(const CString &StrFileName, const CString StrUserName = _T(""));	

	// 可能需要用到的wsp目录, wsp合成wsp列表等等需要处理
	// TODO
	
	// 当前用户的私有目录, 默认不需要给用户名， 取当前用户名， 在未登陆时， 必须取用户名
	static CString	GetPrivateRootPath(const CString &StrUserName);			// 用户私有根目录
	static CString	GetPrivateConfigPath(const CString &StrUserName);			// 用户私有配置文件目录
	static CString	GetPrivateWorkspacePath(const CString &StrUserName);		// 用户私有版面文件目录
	static CString	GetPrivateExcelPath(const CString &StrUserName);			// 用户私有导出Excel 文件目录
	static CString	GetPrivateTradePath(const CString &StrUserName);			// 用户私有交易配置文件目录

	// 常用的配置文件名
	static CString  GetSysConfigFileName();
	static CString	GetIndexFileName();
	static CString	GetSysBlocksFileName();
	static CString	GetRelativeMerchsFileName();
	static CString	GetHkWarrantFileName();

	//////////////////////////////////////////////////////////////////////////
	// fan配置文件路径
	static CString  GetGridHeadFilePath(const CString& StrUserName);
	static CString  GetGridHeadFileName(); // 仅返回名称
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
	static CString	GetCaptionConfigFilePath(); // 获取标题栏里菜单配置文件
	//获取消息或者通知的模板文件
	static CString  GetPublicNotifyMsgTemp();

	//////////////////////////////////////////////////////////////////////////
	// cui配置文件路径

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
	static CString  GetUserBlocksPulbicFilePullName();	// 用户自选公共文件

	static CString  GetUserTradeCfgFileFullName(const CString &StrUserName);

	static CString	GetCustomBlocksFileFullName(const CString &StrUserName);
	static CString  GetCustomBlocksPulbicFilePullName();// 用户自定义公共文件

	static CString	GetUserNoteInfoFilePath(const CString& StrUserName);		// 获取用户投资笔记目录

	static CString	GetPublicTmpFilePath();			// 获取公共的临时目录

	static CString	GetChouMaParamFullName(const CString& StrUserName);

	static CString	GetIoViewFaceFileName();

	static CString	GetOfflineDataPath();		// 离线数据路径
	static CString	GetFundHoldFileFullName();	// 重股持仓文件路径

	//////////////////////////////////////////////////////////////////////////
	// 相对路径转为绝对路径
	static CString	ComPathToAbsPath(const CString& StrComPath);

};

#endif