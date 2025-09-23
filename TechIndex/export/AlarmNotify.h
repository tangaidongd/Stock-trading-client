#ifndef _ALARM_NOTIFY_H_
#define _ALARM_NOTIFY_H_

#include "MerchManager.h"

class CArbitrage;

// ֪ͨ�ӿ�
class CAlarmNotify 
{
public:
	enum E_AlarmNotify
	{
		EANAddMerch = 0,		// ��Ʒ�̶�����
		EANModifyMerch,
		EANDelMerch,
		EANDelAllMerch,

		EANAddArbitrage,		// �����̶�����
		EANModifyArbitrage,
		EANDelArbitrage,
		EANDelAllArbitrage,

		EANAddFormular,			// Ԥ����ʽ�޸���
		EANModifyFormular,
		EANDelFormular,
		EANDelAllFormular,

		EANAlarmOn,				// ������־�仯
		EANAlarmOff,

		EANPromptOn,			// ��ʾ��־�仯
		EANPromptOff,

		EANCount
	};
	
	// �����иı�
	virtual void OnAlarmsChanged(void* pData, CAlarmNotify::E_AlarmNotify eNotify) = 0;
	
	// �ﵽԤ������, ������
	virtual void Alarm(CMerch* pMerch, const CString& StrPrompt) = 0;
	
	// ������
	virtual void AlarmArbitrage(CArbitrage* pArbitrage, const CString& StrPrompt) = 0;
};

#endif // _ALARM_NOTIFY_H_