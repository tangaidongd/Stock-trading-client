// chart.h : chart DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CchartApp
// �йش���ʵ�ֵ���Ϣ������� chart.cpp
//

class CchartApp : public CWinApp
{
public:
	CchartApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
