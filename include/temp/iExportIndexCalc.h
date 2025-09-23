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
		EEFCEAdd = 0,		// ĳָ������
		EEFCEUpdate,		// ĳָ���޸�
		EEFCEDel,			// ĳָ��ɾ��
	};

	class iExportIndexCalcNotify
	{
	public:
		// ����ָ�����ģ��
		// virtual void	SetIndexCalc(iExportIndexCalc *pCalc) = 0;
		
		/*****************************************************************************
		* ָ���޸���
		* ����:
		*	const wchar_t *pwszIndexName:		ָ������
		*	int eChangeEvent:					�¼����� E_ExportIndexChangeEvent
		* ����:
		********************************************************************************
		*/
		virtual void	OnIndexChanged(const wchar_t *pwszIndexName, int eChangeEvent) = 0;
	};

	class iExportIndexCalc
	{
	public:

		// ������ɾ���¼���ע
		virtual void	AddIndexNotify(iExportIndexCalcNotify *pNotify) = 0;
		virtual void	RemoveIndexNotify(iExportIndexCalcNotify *pNotify) = 0;

		// ��ʾָ�����Ի���
		virtual void	ShowIndexManagerDlg() = 0;
		
		// ���ָ���Ƿ����
		virtual	bool	IsIndexExist(const wchar_t *pwszIndexName) = 0;

		/*****************************************************************************
		* ����ָ��
		* ����:
		*	const wchar_t *pwszIndexName:		ָ������
		*	const T_ExportKLineUnit *pKLine:	��������Ҫ�ṩ��K������
		*	int iKLineUnitCount:				��������Ҫ�ṩ��K����K�ߵ�Ԫ����
		*	OUT T_ExportIndexCalcResult **ppResult:		���. ����Ľ������������0-n��(n=iResultDataCount)
		*													(*ppResult)�����ݱ���ΪNULL
		*	OUT int &iResultDataCount:						���. ����Ľ��������������ֵ0-n
		* ����: true �ɹ�����
		********************************************************************************
		*/
		virtual bool	CalcIndex(const wchar_t *pwszIndexName, const T_ExportKLineUnit *pKLine, int iKLineUnitCount, OUT T_ExportIndexCalcResult **ppResult, OUT int &iResultDataCount) = 0;

		/*****************************************************************************
		* �ͷż���ָ����������
		* ����:
		*	T_ExportIndexCalcResult *pResult:		����Ľ��������CalcIndex�����
		*	int iResultDataCount:					����Ľ������������CalcIndex�����
		* ����:
		********************************************************************************
		*/
		virtual	void	FreeCalcResultData(T_ExportIndexCalcResult *pResult, int iResultDataCount) = 0;
	};
}

#endif //!_IEXPORTINDEXCALC_H_