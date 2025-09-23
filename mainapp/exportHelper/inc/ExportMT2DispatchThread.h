#ifndef _EXPORTMT2DISPATCHTHREAD_H_
#define _EXPORTMT2DISPATCHTHREAD_H_

#include <vector>
using std::vector;
#include <list>
using std::list;
#include "synch.h"

// 将主线程的消息通知等转接到其它线程调度

class iExportMT2DispatchThreadProcessor
{
public:
	// 调度回调
	// bSucDisp表示此次调度是否是成功的调度
	// 无论是否成功调度，LPARAM中总是保存的对应投递项
	virtual void	DispatchThreadProcess(bool bSucDisp, LPARAM l) = 0;
};


struct T_ExportMT2DispatchThreadProcessorLParam 
{
	int		iEventId;		// 什么事件
	bool	bProcessed;
	WPARAM	wP;				// 参数
	LPARAM	lP;				// 参数

	T_ExportMT2DispatchThreadProcessorLParam()
	{
		iEventId = 0;
		bProcessed = false;
		wP = NULL;
		lP = NULL;
	}
};


class CExportMT2DispatchThread
{
private:
	struct T_PostedItem 
	{
		iExportMT2DispatchThreadProcessor *pProcessor;
		LPARAM	lP;
	};

public:
	~CExportMT2DispatchThread();

	static CExportMT2DispatchThread &Instance();

public:
	// 必须在运行之前就建立好Processor关系，否则会出现同步问题
	void	AddProcessor(iExportMT2DispatchThreadProcessor *pProcessor);
	void	RemoveProcessor(iExportMT2DispatchThreadProcessor *pProcessor);
	void	RemoveAllProcessor();

	// 初始化 - 主线程调用
	bool	InitInstance();
	void	UnInit();

	// 投递 - 将异步线程执行回调
	bool	Post2DispatchThread(iExportMT2DispatchThreadProcessor *pProcessor, LPARAM l);

private:
	CExportMT2DispatchThread();

	// 线程函数
	static UINT AFX_CDECL DispatchThread(LPVOID lParam);
	UINT			DoDispatchThread();
	bool			DoDispatchItem(const T_PostedItem &stItem, bool bSucDisp);

	// 投递
	bool	PushPostedItem(const T_PostedItem &stItem);
	bool	PopPostedItem(T_PostedItem &stItem);

private:
	volatile bool		m_bStop;		// 是否停止线程的运行

	typedef vector<CWinThread *>	WinThreadVector;
	WinThreadVector		m_aThread;

	

	typedef vector<iExportMT2DispatchThreadProcessor *>	ProcessorVector;
	LockSingle			m_LockProcessor;
	ProcessorVector		m_aProcessor;

	LockSingle			m_LockPostedItem;
	typedef list<T_PostedItem>	PostedItemList;
	PostedItemList		m_lstPostedItem;
	HANDLE				m_hSemPostedItem;
};

#endif //!_EXPORTMT2DISPATCHTHREAD_H_