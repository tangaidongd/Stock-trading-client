#ifndef _PUB_FUNCTION_05_19_
#define _PUB_FUNCTION_05_19_

#include "StructDef.h"
#include "ShareFun.h"
#include "GdiPlusTS.h"
#include<stdio.h>
#include<ctype.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>

Image* TransBase64CodeToImage( char* str );
char*  TransImageToBase64Code(CString strImgPath);
bool   IsAllNumbers(CString str);
bool   IsCombineByNum(std::string strParam);
int Encrypt(const char path[]); // 加密函数
int Decrypt(const char path[]);//解密函数

#endif