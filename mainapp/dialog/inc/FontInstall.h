#ifndef _FONT_INSTALL_H_
#define _FONT_INSTALL_H_

class CFontInstallHelper
{
public:
	~CFontInstallHelper();

	static void	InstallFonts();		// 安装字体

	static void SaveDefaultXML();		// 制作一个默认的模板xml文件

protected:
private:

	CFontInstallHelper();

	static CString  GetXmlFileName();		// 获取xml配置文件名称

	void	LoadFromXml();				// 加载配置

	int		AddFonts();					// 安装配置中需要安装的字体
	void	RemoveFonts();				// 取消安装

	CStringArray m_aInstalledFontFiles;	// 安装完毕的字体文件名
	CStringArray m_aToInstallFontFiles;	// 将要安装的字体文件名
};

#endif