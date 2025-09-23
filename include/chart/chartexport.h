#ifndef __CHART_EXPORT_CHART_LL_H__
#define __CHART_EXPORT_CHART_LL_H__
#ifdef _CHART_EXPORT
#define CHART_EXPORT _declspec(dllexport)
#else
#define CHART_EXPORT _declspec(dllimport)
#endif
#endif