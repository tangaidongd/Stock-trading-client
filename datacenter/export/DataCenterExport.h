#ifndef _DATACENTER_EXPORT_H_
#define _DATACENTER_EXPORT_H_
//#include "typedef.h"
//#include "EnumType.h"
//#include <map>
//#include <set>
//#include <vector>
//using namespace std;

#ifndef DATACENTER_EXPORT
#define DATACENTER_DLL_EXPORT __declspec(dllimport)
#else
#define DATACENTER_DLL_EXPORT __declspec(dllexport)
#endif



DATACENTER_DLL_EXPORT void DataCenterFree(void* pData);


#endif  //_DATACENTER_EXPORT_H_