#ifndef _EXPORTMT2DISPATCHTHREAD_H_
#define _EXPORTMT2DISPATCHTHREAD_H_

#include <vector>
using std::vector;
#include <list>
using std::list;
#include "synch.h"

// �����̵߳���Ϣ֪ͨ��ת�ӵ������̵߳���

class iExportMT2DispatchThreadProcessor
{
public:
	// ���Ȼص�
	// bSucDisp��ʾ�˴ε����Ƿ��ǳɹ��ĵ���
	// �����Ƿ�ɹ����ȣ�LPARAM�����Ǳ���Ķ�ӦͶ����
	virtual void	DispatchThreadProcess(bool bSucDisp, LPARAM l) = 0;
};


struct T_ExportMT2DispatchThreadProcessorLParam 
{
	int		iEventId;		// ʲô�¼�
	bool	bProcessed;
	WPARAM	wP;				// ����
	LPARAM	lP;				// ����

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
	// ����������֮ǰ�ͽ�����Processor��ϵ����������ͬ������
	void	AddProcessor(iExportMT2DispatchThreadProcessor *pProcessor);
	void	RemoveProcessor(iExportMT2DispatchThreadProcessor *pProcessor);
	void	RemoveAllProcessor();

	// ��ʼ�� - ���̵߳���
	bool	InitInstance();
	void	UnInit();

	// Ͷ�� - ���첽�߳�ִ�лص�
	bool	Post2DispatchThread(iExportMT2DispatchThreadProcessor *pProcessor, LPARAM l);

private:
	CExportMT2DispatchThread();

	// �̺߳���
	static UINT AFX_CDECL DispatchThread(LPVOID lParam);
	UINT			DoDispatchThread();
	bool			DoDispatchItem(const T_PostedItem &stItem, bool bSucDisp);

	// Ͷ��
	bool	PushPostedItem(const T_PostedItem &stItem);
	bool	PopPostedItem(T_PostedItem &stItem);

private:
	volatile bool		m_bStop;		// �Ƿ�ֹͣ�̵߳�����

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