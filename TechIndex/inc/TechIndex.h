// TechIndex.h : TechIndex DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTechIndexApp
// �йش���ʵ�ֵ���Ϣ������� TechIndex.cpp
//

class CTechIndexApp : public CWinApp
{
public:
	CTechIndexApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
