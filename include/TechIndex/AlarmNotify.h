#ifndef _ALARM_NOTIFY_H_
#define _ALARM_NOTIFY_H_

#include "MerchManager.h"

class CArbitrage;

// 通知接口
class CAlarmNotify 
{
public:
	enum E_AlarmNotify
	{
		EANAddMerch = 0,		// 商品固定条件
		EANModifyMerch,
		EANDelMerch,
		EANDelAllMerch,

		EANAddArbitrage,		// 套利固定条件
		EANModifyArbitrage,
		EANDelArbitrage,
		EANDelAllArbitrage,

		EANAddFormular,			// 预警公式修改了
		EANModifyFormular,
		EANDelFormular,
		EANDelAllFormular,

		EANAlarmOn,				// 报警标志变化
		EANAlarmOff,

		EANPromptOn,			// 提示标志变化
		EANPromptOff,

		EANCount
	};
	
	// 条件有改变
	virtual void OnAlarmsChanged(void* pData, CAlarmNotify::E_AlarmNotify eNotify) = 0;
	
	// 达到预警条件, 报警了
	virtual void Alarm(CMerch* pMerch, const CString& StrPrompt) = 0;
	
	// 发警报
	virtual void AlarmArbitrage(CArbitrage* pArbitrage, const CString& StrPrompt) = 0;
};

#endif // _ALARM_NOTIFY_H_