#ifndef _IEXPORTINDEXCALC_H_
#define _IEXPORTINDEXCALC_H_

#include "iExportIndexCalcStruct.h"

#ifndef IN
#define IN
#endif
#ifndef INOUT
#define INOUT
#endif
#ifndef OUT
#define OUT
#endif

namespace cfjdExport
{
	class iExportIndexCalc;

	enum E_ExportIndexChangeEvent
	{
		EEFCEAdd = 0,		// 某指标新增
		EEFCEUpdate,		// 某指标修改
		EEFCEDel,			// 某指标删除
	};

	class iExportIndexCalcNotify
	{
	public:
		// 设置指标计算模块
		// virtual void	SetIndexCalc(iExportIndexCalc *pCalc) = 0;
		
		/*****************************************************************************
		* 指标修改了
		* 参数:
		*	const wchar_t *pwszIndexName:		指标名称
		*	int eChangeEvent:					事件类型 E_ExportIndexChangeEvent
		* 返回:
		********************************************************************************
		*/
		virtual void	OnIndexChanged(const wchar_t *pwszIndexName, int eChangeEvent) = 0;
	};

	class iExportIndexCalc
	{
	public:

		// 新增、删除事件关注
		virtual void	AddIndexNotify(iExportIndexCalcNotify *pNotify) = 0;
		virtual void	RemoveIndexNotify(iExportIndexCalcNotify *pNotify) = 0;

		// 显示指标管理对话框
		virtual void	ShowIndexManagerDlg() = 0;
		
		// 检查指标是否存在
		virtual	bool	IsIndexExist(const wchar_t *pwszIndexName) = 0;

		/*****************************************************************************
		* 计算指标
		* 参数:
		*	const wchar_t *pwszIndexName:		指标名称
		*	const T_ExportKLineUnit *pKLine:	计算所需要提供的K线数据
		*	int iKLineUnitCount:				计算所需要提供的K线中K线单元个数
		*	OUT T_ExportIndexCalcResult **ppResult:		输出. 计算的结果集，可能有0-n个(n=iResultDataCount)
		*													(*ppResult)中内容必须为NULL
		*	OUT int &iResultDataCount:						输出. 计算的结果集条数，可能值0-n
		* 返回: true 成功计算
		********************************************************************************
		*/
		virtual bool	CalcIndex(const wchar_t *pwszIndexName, const T_ExportKLineUnit *pKLine, int iKLineUnitCount, OUT T_ExportIndexCalcResult **ppResult, OUT int &iResultDataCount) = 0;

		/*****************************************************************************
		* 释放计算指标结果集数据
		* 参数:
		*	T_ExportIndexCalcResult *pResult:		计算的结果集，由CalcIndex中输出
		*	int iResultDataCount:					计算的结果集条数，由CalcIndex中输出
		* 返回:
		********************************************************************************
		*/
		virtual	void	FreeCalcResultData(T_ExportIndexCalcResult *pResult, int iResultDataCount) = 0;
	};
}

#endif //!_IEXPORTINDEXCALC_H_