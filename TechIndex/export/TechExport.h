#ifndef TECH_EXPORT_H
#define TECH_EXPORT_H

#ifdef MY_EXPORT
#define EXPORT_CLASS _declspec(dllexport)
#define EXPORT_API _declspec(dllexport)
#define EXPORT_VARIABLE _declspec(dllexport)
#else
#define EXPORT_CLASS _declspec(dllimport)
#define EXPORT_API _declspec(dllimport)
#define EXPORT_VARIABLE _declspec(dllimport)
#endif

#endif