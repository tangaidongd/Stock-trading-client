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

AFX_EXT_API void				DrawImage(Graphics& GraphicsImage,		 // ��ͼ������
							  Image* pImage,				 // ͼƬ
							  const CRect& rectShow,		 // ��ʾ�ľ���
 							  int32 iSubImageNums,			 // ͼƬ���������ͼ��Ŀ( ����һ��ͼ���������ť)
 							  int32 iShowImageIndex,		 // Ҫ��ʾ���ǵڼ���ͼ	
 							  bool32 bStretch);  			 // �Ƿ���Ŀ�����������
AFX_EXT_API void DrawImage2(Graphics& GraphicsImage, Image* pImage, const CRect& rectShow, int32 imagX, int32 imagY, bool32 bStretch);//ʵ��ͼƬ��ȡ	

#endif


