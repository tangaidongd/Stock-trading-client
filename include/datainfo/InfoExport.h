#ifndef _DATAINFO_EXPORT_H_
#define _DATAINFO_EXPORT_H_
#include "typedef.h"
#include "EnumType.h"
#include <map>
#include <set>
#include <vector>
using namespace std;

#ifndef DATAINFO_EXPORT
#define DATAINFO_DLL_EXPORT __declspec(dllimport)
#else
#define DATAINFO_DLL_EXPORT __declspec(dllexport)
#endif



#endif  //_DATAINFO_EXPORT_H_