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
	int		GetMinPos(int x, int y, int z);	// ��ȡ������������С��ֵ��������С��0��������С��0��ʾ��";?#"��Щ������
	void	SortArry(int arr[], int num);
	CString AddParameters();
	CString AddQuery();
	CString AddFragment();
	void	AddSymbol(CString& strUrl);	// �������ָ�������ԭ�е�˳����ӵ�������


private:
	int			m_port;	// �˿ں�
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
protocol��Э�飩
ָ��ʹ�õĴ���Э�飬�±��г� protocol ���Ե���Ч�������ơ� ��õ���HTTPЭ�飬��Ҳ��ĿǰWWW��Ӧ������Э�顣
file ��Դ�Ǳ��ؼ�����ϵ��ļ�����ʽfile:///��ע����Ӧ������б�ܡ�
ftp ͨ�� FTP������Դ����ʽ FTP://
gopher ͨ�� Gopher Э����ʸ���Դ��
http ͨ�� HTTP ���ʸ���Դ�� ��ʽ HTTP://
https ͨ����ȫ�� HTTPS ���ʸ���Դ�� ��ʽ HTTPS://
mailto ��ԴΪ�����ʼ���ַ��ͨ�� SMTP ���ʡ� ��ʽ mailto:
MMS ͨ�� ֧��MMS����ý�壩Э��Ĳ��Ÿ���Դ�������������Windows Media Player����ʽ MMS://
ed2k ͨ�� ֧��ed2k��ר���������ӣ�Э���P2P������ʸ���Դ���������������¿�� ��ʽ ed2k://
Flashget ͨ�� ֧��Flashget:��ר���������ӣ�Э���P2P������ʸ���Դ��������������쳵�� ��ʽ Flashget://
thunder ͨ�� ֧��thunder��ר���������ӣ�Э���P2P������ʸ���Դ�������������Ѹ�ף� ��ʽ thunder://
news ͨ�� NNTP ���ʸ���Դ��
*/


#endif