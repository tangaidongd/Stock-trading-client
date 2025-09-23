// SelectStockCenter.h: interface for the CSelectStockCenter class.
//
//////////////////////////////////////////////////////////////////////
// ����ѡ������

#if !defined(_SELECTSTOCKCENTER_H_)
#define _SELECTSTOCKCENTER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SelectStockStruct.h"
#include "synch.h"

#include "EngineCenterBase.h"
using namespace std;


class AFX_EXT_CLASS CSelectStockCenter  
{
public:
	CSelectStockCenter(HWND hOwner);
	virtual ~CSelectStockCenter();

	// ��������ѡ�ɲ���
	void	SetConditionChooseParam(T_ConditionSelectParam& stParam);
	
	// ���ö���ѡ�ɲ���
	void	SetCustomChooseParam(T_CustomSelectParam& stParam);

	// ��ʼѡ��
	void	StartWork();

	// ֹͣѡ��
	void	StopWork();

	// ȡ���
	void	GetSelectResult(set<CMerch*, MerchCmp>& aMerchs);


private:
	// �̺߳���
	static unsigned __stdcall CallBackThreadSelect(void* pVoid);		

	// ʵ�ʼ��������ѡ�ɺ���
	void	ThreadConditionSelect();

	// ʵ�ʼ���Ķ���ѡ�ɺ���
	void	ThreadCustomSelect();
		
private:
	// �Ƿ��˳�
	bool32	m_bStop;

	// ѡ������
	E_SelectStockType	m_eSelectStockType;

	// �Ի�����
	HWND	m_hOwner;

	// ����ѡ�ɲ���
	T_ConditionSelectParam	   m_stConditionParam;

	// ����ѡ�ɲ���
	T_CustomSelectParam			m_stCustomParam;
	
	// ������
	set<CMerch*, MerchCmp> m_aMerchsOUT;	
	LockSingle			   m_LockMerchsOUT;


	// �߳̾��
	HANDLE	m_hThreadSelect;
};


AFX_EXT_API void SetAbsCenterManager(CAbsCenterManager *pCenterManager);
//AFX_EXT_API void SetUserName(CString strUserName);
//AFX_EXT_API void SetServerTime(CGmtTime serverTime);

#endif // !defined(_SELECTSTOCKCENTER_H_)
