#ifndef _URL_PARSER_H_
#define _URL_PARSER_H_

enum E_Protocol
{
	alias,
	ftp,
	http,
	https,
	files,
	tencent,
	none
};

TCHAR Protocol[][20];

class UrlParser
{
public:
	UrlParser(LPCTSTR url);
	UrlParser();
	~UrlParser(void);

	void Parser(LPCTSTR url);

	E_Protocol GetProtocolType(){return m_protocol;}
	void SetProtocolType(E_Protocol protocol){m_protocol = protocol;}

	CString GetHostname(){return m_strHostname;}
	void SetHostname(LPCTSTR hostname);
	int GetPort(){return m_port;}
	void SetPort(int nPort){m_port = nPort;}

	CString GetPath(){return m_path;}
	void SetPath(LPCTSTR path){ m_path = path;}

	CString GetParameters(){return m_parameters;};
	void SetParameters(LPCTSTR parameters){m_parameters = parameters;}

	CString GetQuery();
	void SetQuery(LPCTSTR query,bool bAdd = false);

	CString GetQueryValue(LPCTSTR key);
	void SetQueryValue(LPCTSTR key, LPCTSTR value);

	CString GetFragment(){return m_fragment;}
	void SetFragment(LPCTSTR fragment){m_fragment = fragment;}
	/* *  */
	CString GetUrl();

public:
	bool        m_bValid;

private:
	int		GetMinPos(int x, int y, int z);	// 获取三个数字中最小的值（但不能小于0，如果结果小于0表示无";?#"这些参数）
	void	SortArry(int arr[], int num);
	CString AddParameters();
	CString AddQuery();
	CString AddFragment();
	void	AddSymbol(CString& strUrl);	// 把三个分隔符，按原有的顺序添加到链接中


private:
	int			m_port;	// 端口号
	E_Protocol	m_protocol;	//
	CString		m_strHostname; //
	CString		m_path;	//
	CString		m_parameters;	//
	CString		m_query;	//
	CString		m_fragment;

	
	// 
	int symbol[2][3];
};
/*
protocol（协议）
指定使用的传输协议，下表列出 protocol 属性的有效方案名称。 最常用的是HTTP协议，它也是目前WWW中应用最广的协议。
file 资源是本地计算机上的文件。格式file:///，注意后边应是三个斜杠。
ftp 通过 FTP访问资源。格式 FTP://
gopher 通过 Gopher 协议访问该资源。
http 通过 HTTP 访问该资源。 格式 HTTP://
https 通过安全的 HTTPS 访问该资源。 格式 HTTPS://
mailto 资源为电子邮件地址，通过 SMTP 访问。 格式 mailto:
MMS 通过 支持MMS（流媒体）协议的播放该资源。（代表软件：Windows Media Player）格式 MMS://
ed2k 通过 支持ed2k（专用下载链接）协议的P2P软件访问该资源。（代表软件：电驴） 格式 ed2k://
Flashget 通过 支持Flashget:（专用下载链接）协议的P2P软件访问该资源。（代表软件：快车） 格式 Flashget://
thunder 通过 支持thunder（专用下载链接）协议的P2P软件访问该资源。（代表软件：迅雷） 格式 thunder://
news 通过 NNTP 访问该资源。
*/


#endif