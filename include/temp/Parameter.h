
//�������������ṹ��
struct ServerObj
{ 
public:
	ServerObj()
	{
		dnsstr = NULL;
		proxydns = NULL;
		proxyloginid = NULL;
		proxypassword = NULL;
		proxytypte = 0;
		dnsstrType = 0;
		linkID = 0;
	}

	char* dnsstr;
	char* proxydns;
	char* proxyloginid;
	char* proxypassword;
	int proxytypte ;
	int dnsstrType;
	int linkID;
} ;// proxytypte:0 �޴��� 1 HTTP���� 2 socket4���� 

//�����û�����ṹ��
struct UserInfo
{
public:
	UserInfo()
	{
		userid = NULL;
		password = NULL;
		logintype = 0;
	}

	char* userid;
	char* password;
	int logintype ;
};// logintype��¼����:0ģ���� 1ʵ��

