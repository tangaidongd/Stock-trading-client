#ifndef ANALYSISI_WEB_H_
#define ANALYSISI_WEB_H_

#include <map>

class CAnalysisWeb
{
public:
	CAnalysisWeb();
	~CAnalysisWeb();

	enum E_CommandType
	{
		COM_OPEN_WEB_PAGE_HTTP,		// 打开网页http
		COM_OPEN_WEB_PAGE_HTTPS,		// 打开网页https
		COM_OPEN_LOCAL_FILE,	// 打开本地文件
		COM_OPEN_VIEW,			// 打开原生视图
		COM_OPEN_LOCAL_APP,		// 打开本地应用
		COM_OPEN_PASS_MSG,		// 传递消息
		COM_OPEN_INVALID,		// 无效命令
	};

	bool AnalysisJsonCmd(CString strJsonCmd);
	CStringA excuteJSCallbackPackJsonCmd();
	CStringA excuteJS111();
private:
	string					m_strRawdata;
public:
	multimap<CString,CString> m_multimapParamsInfo;		// 应该规定数据类型
	CString				 m_strCommand;			// 命令字
	E_CommandType		 m_enumComType;			// 命令类型
	CString				 m_strCommadID;			// 命令字ID
};



#endif  // CEF_TESTS_CEFCLIENT_CLIENT_APP_H_
