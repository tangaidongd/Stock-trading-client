#ifndef _STOCK_SELECT_MANAGER_H_
#define _STOCK_SELECT_MANAGER_H_

#include "MerchManager.h"

class CStockSelectTypeChanged
{
public:
	CStockSelectTypeChanged();		// �Զ�ע������
	virtual ~CStockSelectTypeChanged();

	virtual	void	OnStockSelectTypeChanged() = 0;	// ����ı���ѡ�����ͣ���ͼ��Ҫ���¸�������ѡ������ͷ�������
};

// ѡ�����ݹ��� ����ѡ�ɵı�־��Ȩ��
class CStockSelectManager
{
public:
	enum E_IndexSelectStock		// ��ѡ������ֻ��ȡcheck��ťģʽ��ѡ��ʵ�ʽ������ɸѡ ����ѡ��е���Ʒ����ѡ��
	{
		ShortLine		= 0x0001,	// ����ѡ��
		MidLine			= 0x0002,	// ����ѡ��
		BuySelect = 4,				// ���뾫ѡ
 		
		StrategySelectBegin,
		StrategyJGLT = 0x8,		    // ������̧
		StrategyXPJS = 0x10,		// ����ϴ��
		StrategyYJZT = 0x20,		// �λ���ͣ
		StrategyNSQL = 0x40,		// ����ǿׯ
		StrategyCDFT = 0x80,		// ��������
		StrategySLXS = 0x100,	    // ������ˮ
		StrategyHFFL = 0x200,	    // �ط����
		StrategySHJD = 0x400,	    // �����۶�
		StrategyKTXJ = 0x800,		// ��ͷ�������
		StrategyZDSD = 0x1000,		// ���ж������
		StrategyDXQS = 0x2000,		// ����ǿ�Ʋ���
		StrategyQZHQ = 0x4000,	    // ǿ�ߺ�ǿ����
		StrategyBDCZ = 0x8000,	    // ���β�������
		StrategySelectEnd,

		IndexStrategySelectStart,
 		IndexStrategy  = 0x10000,  // ս��ѡ��
 		RedStrategy    = 0x20000  | IndexStrategy,	// ���̺�/��/��/��/��������
 		GreenStrategy  = 0x40000  | IndexStrategy,
 		BlueStrategy   = 0x80000  | IndexStrategy,
 		YellowStrategy = 0x100000 | IndexStrategy,
 		FallStrategy   = 0x200000 | IndexStrategy,
		IndexStrategySelectEnd,

		EISS_End,
 	};
	
	enum E_HotSelectStock
	{
		HotTimeSale = 1,		// ��ǿ�� ����ǿ�� �ʽ�ǿ�� ����ϵ - ���ɸѡģʽ
		HotForce = 2,
		HotCapital = 4,
	};
	
	enum{
		OpenStockPool = 0xFFFF,			// ע�Ᵽ�ִ򿪹�Ʊ��
	};
	
	enum E_CurrentSelect
	{
		IndexShortMidSelect,			// ���� ���� ... ����
		HotSelect,						// ����ǿ������
	};
	
	static	CStockSelectManager &Instance();
	virtual ~CStockSelectManager();
	
	
	DWORD	GetIndexSelectStockFlag() const;
	// 	DWORD	GetStrategySelectStockFlag() const  { return m_dwStrategySelectStockFlag; }
	// 	DWORD	GetIndexStrategySelectFlag() const { return m_dwIndexStrategySelectFlag; }
	DWORD	GetHotSelectStockFlag() const { return m_dwHotSelectStockFlag; }
	
	bool32	SetIndexSelectStockFlag(DWORD dwIndexFlag, bool32 bAlarmIfNoRight = false, bool32 bForceFire = false);
	// 	void	SetStrategySelectStockFlag(DWORD dwStrategyFlag) { m_dwStrategySelectStockFlag = dwStrategyFlag; }
	// 	void	SetIndexStrategySelectFlag(DWORD dwIndexFlag) { m_dwIndexStrategySelectFlag = dwIndexFlag; }
	bool32	SetHotSelectStockFlag(DWORD dwHotFlag, bool32 bAlarmIfNoRight = false, bool32 bForceFire = false);
	
	void	SetCurrentReqType(E_CurrentSelect eSel, bool32 bAlarmIfNoRight = false, bool32 bForceFire = false);		// ���õ�ǰѡ����������, ֪ͨ������Ӧ����
	// Ӧ���ڵ��ô˺���ǰ�����ú��������
	E_CurrentSelect	GetCurrentReqType() const { return m_eCurrentSelect; }
	
	bool32	IsCurrentReqUnderUserRight(int *pRightDeny = NULL);		// ��ǰѡ�������Ƿ����û�Ȩ��������
	bool32  IsCurrentReqUnderUserRight(E_CurrentSelect eSelectType, int *pRightDeny = NULL);
	
	void	PromptLackRight(int iRightDeny);

	void	AddListener(CStockSelectTypeChanged *pListener);
	void	RemoveListener(CStockSelectTypeChanged *pListener);
	
	static bool32 IsDefaultReqUnderUserRight(int *pRightDeny = NULL);	// Ĭ��ѡ���Ƿ����û�Ȩ����
	static bool32 IsReqUnderUserRight(E_CurrentSelect eSelectType, DWORD dwFlag, int *pRightDeny = NULL);		// ָ��ѡ��
	
	static bool32 GetIndexSelectStockEnumByCmdId(UINT nId, OUT E_IndexSelectStock &eJGXG);

private:
	CStockSelectManager();

	// ��ʵ�ָ�ֵ
	CStockSelectManager(const CStockSelectManager &);
	CStockSelectManager &operator=(const CStockSelectManager &);

	void	FireChanged();

	DWORD	m_dwIndexSelectStockFlag;		// ����  // ս�� ����һ��
	// 	DWORD	m_dwStrategySelectStockFlag;	// ս��
	// 	DWORD	m_dwIndexStrategySelectFlag;	// ���� - ��ţ���й�
	DWORD	m_dwHotSelectStockFlag;			// ����ǿ��
	
	
	E_CurrentSelect			m_eCurrentSelect;

	CArray<CStockSelectTypeChanged *, CStockSelectTypeChanged *> m_aListeners;
};


#endif //_STOCK_SELECT_MANAGER_H_