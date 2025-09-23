#ifndef _URL_CONFIG_H_
#define _URL_CONFIG_H_

#include <map>
using std::map;

class CUrlConfig
{
	CUrlConfig();

public:
	// �κξ�̬�ĺ�����Ҫ�������Ѿ����ػ�ȡ��û������~~
	static CUrlConfig &Instance();
	CString GetUrl(const LPCTSTR &AliasName);
	
private:
	bool Initialize();	// ��ʼ��, ����Ҫ��ʼ����ʱ����ã�Ĭ�Ͻ�����һ��


private:
	map<CString,CString> m_mapUrlAlias;
	bool m_bInit;
};

#endif