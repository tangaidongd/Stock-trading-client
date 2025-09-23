#ifndef EXPORT_H
#define EXPORT_H

#ifdef MY_EXPORT
#define EXPORT_CLASS _declspec(dllexport)
#define EXPORT_API _declspec(dllexport)
#else
#define EXPORT_CLASS _declspec(dllimport)
#define EXPORT_API _declspec(dllimport)
#endif

#endif