#ifndef _EXPORTMAINTHREADWND_H_
#define _EXPORTMAINTHREADWND_H_

// 主界面线程创建的一个方便其它线程转到主线程调用方法的窗口
// 属于MessageOnly的一个子窗口，在MainFrame OnCreate时初始化, OnDestroy时消亡

// 处理接口
class iExportMainThreadWndProcessor
{
public:
	virtual LRESULT	MainThreadWndProcess(LPARAM l) = 0;
};

struct T_ExportMainThreadWndProcessorLParam 
{
	int		iEventId;		// 什么事件
	bool	bProcessed;
	WPARAM	wP;				// 参数
	LPARAM	lP;				// 参数

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

	// 主线程调用初始化后，任意线程调用
	static CExportMainThreadWnd &Instance();
	
	// 初始化 - 主线程调用
	bool	InitInstance();
	void	UnInit();

	// 任意线程调用
	bool	SendToMainProcess(iExportMainThreadWndProcessor *pProcessor, LPARAM l);

private:
	CExportMainThreadWnd();

protected:
	DECLARE_MESSAGE_MAP();

	afx_msg LRESULT		OnMsgToMainProcess(WPARAM w, LPARAM l);
};

#endif //!_EXPORTMAINTHREADWND_H_