#ifndef _EXPORTMAINTHREADWND_H_
#define _EXPORTMAINTHREADWND_H_

// �������̴߳�����һ�����������߳�ת�����̵߳��÷����Ĵ���
// ����MessageOnly��һ���Ӵ��ڣ���MainFrame OnCreateʱ��ʼ��, OnDestroyʱ����

// ����ӿ�
class iExportMainThreadWndProcessor
{
public:
	virtual LRESULT	MainThreadWndProcess(LPARAM l) = 0;
};

struct T_ExportMainThreadWndProcessorLParam 
{
	int		iEventId;		// ʲô�¼�
	bool	bProcessed;
	WPARAM	wP;				// ����
	LPARAM	lP;				// ����

	T_ExportMainThreadWndProcessorLParam()
	{
		iEventId = 0;
		bProcessed = false;
		wP = NULL;
		lP = NULL;
	}
};

class CExportMainThreadWnd : public CWnd
{
DECLARE_DYNAMIC(CExportMainThreadWnd);

public:
	~CExportMainThreadWnd();

	// ���̵߳��ó�ʼ���������̵߳���
	static CExportMainThreadWnd &Instance();
	
	// ��ʼ�� - ���̵߳���
	bool	InitInstance();
	void	UnInit();

	// �����̵߳���
	bool	SendToMainProcess(iExportMainThreadWndProcessor *pProcessor, LPARAM l);

private:
	CExportMainThreadWnd();

protected:
	DECLARE_MESSAGE_MAP();

	afx_msg LRESULT		OnMsgToMainProcess(WPARAM w, LPARAM l);
};

#endif //!_EXPORTMAINTHREADWND_H_