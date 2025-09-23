// SelectStockCenter.h: interface for the CSelectStockCenter class.
//
//////////////////////////////////////////////////////////////////////
// 条件选股中心

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

	// 设置条件选股参数
	void	SetConditionChooseParam(T_ConditionSelectParam& stParam);
	
	// 设置定制选股参数
	void	SetCustomChooseParam(T_CustomSelectParam& stParam);

	// 开始选股
	void	StartWork();

	// 停止选股
	void	StopWork();

	// 取结果
	void	GetSelectResult(set<CMerch*, MerchCmp>& aMerchs);


private:
	// 线程函数
	static unsigned __stdcall CallBackThreadSelect(void* pVoid);		

	// 实际计算的条件选股函数
	void	ThreadConditionSelect();

	// 实际计算的定制选股函数
	void	ThreadCustomSelect();
		
private:
	// 是否退出
	bool32	m_bStop;

	// 选股类型
	E_SelectStockType	m_eSelectStockType;

	// 对话框句柄
	HWND	m_hOwner;

	// 条件选股参数
	T_ConditionSelectParam	   m_stConditionParam;

	// 定制选股参数
	T_CustomSelectParam			m_stCustomParam;
	
	// 计算结果
	set<CMerch*, MerchCmp> m_aMerchsOUT;	
	LockSingle			   m_LockMerchsOUT;


	// 线程句柄
	HANDLE	m_hThreadSelect;
};


AFX_EXT_API void SetAbsCenterManager(CAbsCenterManager *pCenterManager);
//AFX_EXT_API void SetUserName(CString strUserName);
//AFX_EXT_API void SetServerTime(CGmtTime serverTime);

#endif // !defined(_SELECTSTOCKCENTER_H_)
