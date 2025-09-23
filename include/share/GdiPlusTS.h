#ifndef _GDI_PLUS_H_
#define _GDI_PLUS_H_

#include "typedef.h"
#include <GdiPlus.h>
using namespace Gdiplus;


/////////////////////////////////////////////////////////////////////
AFX_EXT_API bool32				ImageFromIDResource(HINSTANCE hInst,UINT nID, LPCTSTR sTR,Image * &pImg);
AFX_EXT_API bool32				ImageFromByte(BYTE* pData, int iLen, Image*& pImg);
AFX_EXT_API Bitmap				*BitmapFromImage(Image *pImage);
AFX_EXT_API int32				GetEncoderClsid(const WCHAR *pszFormat, CLSID *pClsid);

AFX_EXT_API void				DrawImage(Graphics& GraphicsImage,		 // 绘图的神器
							  Image* pImage,				 // 图片
							  const CRect& rectShow,		 // 显示的矩形
 							  int32 iSubImageNums,			 // 图片里包含的子图数目( 类似一张图包含多个按钮)
 							  int32 iShowImageIndex,		 // 要显示的是第几个图	
 							  bool32 bStretch);  			 // 是否在目标矩形内拉伸
AFX_EXT_API void DrawImage2(Graphics& GraphicsImage, Image* pImage, const CRect& rectShow, int32 imagX, int32 imagY, bool32 bStretch);//实现图片截取	

#endif


