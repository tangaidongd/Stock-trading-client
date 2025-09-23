#pragma once
#ifdef PARAM
#else
#include"Parameter.h"
#define PARAM
#endif

#ifdef MYDLL_EXPORT
 #define MYDLL_API  __declspec(dllexport) 
#else  
 #define MYDLL_API  __declspec(dllimport)
#endif

class MYDLL_API AuthoPlugin
{
public:
	AuthoPlugin(void);
	~AuthoPlugin(void);
	
	 int GetCode(IN ServerObj serverobj,IN char* version,OUT char** pData, OUT int& iLen,OUT char**uuid,OUT int& iLenUuid,char** msg);
	 int login(IN ServerObj serverobj, IN UserInfo userinfo,IN char* validcode,IN char* uuid,IN char* version,OUT char**sessionid,OUT int& iLenSessionid,char** msg);
	 int logout(IN ServerObj serverobj, IN char* userid, IN char* sessionid,IN char* version,char** msg);
	
	 void Release(char* pData);

private:
	 int GetVersion(IN ServerObj serverobj,OUT char** version,OUT char** msg);
	 int SendHeartCon(IN ServerObj serverobj,IN char* userid,IN char* sessionid,IN char* version,char** msg);
};
