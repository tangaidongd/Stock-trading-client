#ifndef _URL_CONFIG_H_
#define _URL_CONFIG_H_

#include <map>
using std::map;

class CUrlConfig
{
	CUrlConfig();

public:
	// 任何静态的函数不要依赖它已经加载获取还没有析构~~
	static CUrlConfig &Instance();
	CString GetUrl(const LPCTSTR &AliasName);
	
private:
	bool Initialize();	// 初始化, 在需要初始化的时候调用，默认仅调用一次


private:
	map<CString,CString> m_mapUrlAlias;
	bool m_bInit;
};

#endif