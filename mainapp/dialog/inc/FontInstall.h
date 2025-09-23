#ifndef _FONT_INSTALL_H_
#define _FONT_INSTALL_H_

class CFontInstallHelper
{
public:
	~CFontInstallHelper();

	static void	InstallFonts();		// ��װ����

	static void SaveDefaultXML();		// ����һ��Ĭ�ϵ�ģ��xml�ļ�

protected:
private:

	CFontInstallHelper();

	static CString  GetXmlFileName();		// ��ȡxml�����ļ�����

	void	LoadFromXml();				// ��������

	int		AddFonts();					// ��װ��������Ҫ��װ������
	void	RemoveFonts();				// ȡ����װ

	CStringArray m_aInstalledFontFiles;	// ��װ��ϵ������ļ���
	CStringArray m_aToInstallFontFiles;	// ��Ҫ��װ�������ļ���
};

#endif