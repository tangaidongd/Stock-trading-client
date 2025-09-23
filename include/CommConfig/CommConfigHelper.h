#ifndef _COMMCONFIGCLIENTHELPER_H_
#define _COMMCONFIGCLIENTHELPER_H_

#include "comm_config_base.h"
#include <windows.h>
#include "Module.h"

class CCommConfigHelper: public CTempldateHelperCheck<commconfig::iCommConfgClient>
{
public:
public:
#ifdef _DEBUG
	CCommConfigHelper(void) : CTempldateHelperCheck<commconfig::iCommConfgClient>(TEXT("CommConfigd.dll"), "GetCommConfigObj", commconfig::g_iAuthVer) { }
#else
	CCommConfigHelper(void) : CTempldateHelperCheck<commconfig::iCommConfgClient>(TEXT("CommConfig.dll"), "GetCommConfigObj", commconfig::g_iAuthVer) { }
#endif
};

#endif