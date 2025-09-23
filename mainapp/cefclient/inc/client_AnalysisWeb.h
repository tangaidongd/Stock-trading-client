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
		COM_OPEN_WEB_PAGE_HTTP,		// ����ҳhttp
		COM_OPEN_WEB_PAGE_HTTPS,		// ����ҳhttps
		COM_OPEN_LOCAL_FILE,	// �򿪱����ļ�
		COM_OPEN_VIEW,			// ��ԭ����ͼ
		COM_OPEN_LOCAL_APP,		// �򿪱���Ӧ��
		COM_OPEN_PASS_MSG,		// ������Ϣ
		COM_OPEN_INVALID,		// ��Ч����
	};

	bool AnalysisJsonCmd(CString strJsonCmd);
	CStringA excuteJSCallbackPackJsonCmd();
	CStringA excuteJS111();
private:
	string					m_strRawdata;
public:
	multimap<CString,CString> m_multimapParamsInfo;		// Ӧ�ù涨��������
	CString				 m_strCommand;			// ������
	E_CommandType		 m_enumComType;			// ��������
	CString				 m_strCommadID;			// ������ID
};



#endif  // CEF_TESTS_CEFCLIENT_CLIENT_APP_H_
