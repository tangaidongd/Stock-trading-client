#ifndef __CONTROL_DLL_EXPORT__H__LIULEI_H__
#define __CONTROL_DLL_EXPORT__H__LIULEI_H__


#ifdef CONTROL_DLL
#ifdef NEW_CONTROL_DLL_EXPORT
#define CONTROL_EXPORT __declspec(dllexport)
#else
#define CONTROL_EXPORT __declspec(dllimport)

#if _MSC_VER >= 1300
// older compiler have a bug. static template member were not exported corectly
// so we need this
#define _GUILIB_
#endif

#endif
#endif

#ifndef CONTROL_EXPORT
#define CONTROL_EXPORT
#endif


#endif