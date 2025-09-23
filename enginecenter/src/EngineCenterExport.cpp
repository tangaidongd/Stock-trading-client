#include "stdafx.h"
#include "EngineCenterExport.h"


void EngineCenterFree(void* pData)
{
	if (pData)
	{
		delete pData;
		pData = NULL;
	}
}