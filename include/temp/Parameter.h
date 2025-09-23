
//定义服务器对象结构体
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
} ;// proxytypte:0 无代理 1 HTTP代理 2 socket4代理 

//定义用户对象结构体
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
};// logintype登录环境:0模拟盘 1实盘

