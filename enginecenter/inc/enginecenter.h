// enginecenter.h : enginecenter DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif




// CEngineCenterApp
// �йش���ʵ�ֵ���Ϣ������� enginecenter.cpp
//

class CEngineCenterApp : public CWinApp
{
public:
	CEngineCenterApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
