// datacenter.h : datacenter DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif




// CdatacenterApp
// �йش���ʵ�ֵ���Ϣ������� datacenter.cpp
//

class CdatacenterApp : public CWinApp
{
public:
	CdatacenterApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
