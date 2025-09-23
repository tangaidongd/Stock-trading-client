#include "stdafx.h"
#include "DataCenterExport.h"


void DataCenterFree(void* pData)
{
	delete pData;
	pData = NULL;
}