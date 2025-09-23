#ifndef _ENGINECENTER_EXPORT_H_
#define _ENGINECENTER_EXPORT_H_
//#include "typedef.h"
//#include "EnumType.h"
//#include <map>
//#include <set>
//#include <vector>
//using namespace std;

#ifndef ENGINECENTER_EXPORT
#define ENGINECENTER_DLL_EXPORT __declspec(dllimport)
#else
#define ENGINECENTER_DLL_EXPORT __declspec(dllexport)
#endif


ENGINECENTER_DLL_EXPORT void EngineCenterFree(void* pData);


#endif  //_ENGINECENTER_EXPORT_H_