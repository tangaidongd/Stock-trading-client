#ifndef _IEXPORTINDEXCALCSTRUCT_H_
#define _IEXPORTINDEXCALCSTRUCT_H_

#include "iExportQuoteStruct.h"

#pragma pack(push, 1)

// ע�������ռ�
// ���г���char�ִ�����utf8���룬��������codeһ�㲻�������ģ����Կ��Ե���asciiʹ��
namespace cfjdExport
{
	typedef struct _ExportStringW
	{
		int		nStringLen;
		wchar_t *pwszString;
	}T_ExportStringW;
	
	// �������Ķ��ⲹ������
	typedef struct _ExportLooseValue
	{
		int		nIndexInLine;				// ��Ӧ�����߽�����ĸ������ڵ�
		float	fVal;						// ����ֵ
		T_ExportStringW stStrExtraData;		// �ַ���ֵ
	}T_ExportLooseValue;
	
	// K�߻�������
	enum E_ExportLineDrawKind
	{
		EELDKNormal = 0,
		EELDKDrawLine,
		EELDKDrawPolyLine,
		EELDKDrawStickLine,
		EELDKDrawIcon,
		EELDKStringData,//DrawString
		EELDKDrawKLine
	};
	
	// ����������
	// ĳЩָ����ܼ������ж���������
	// ���������߽������
	typedef struct _ExportIndexCalcResult
	{ 
		float *pfLineData;				// ��������������߽����������Ӧ�����K����������
		int	  iFirst;						// ָ����Ч��ʼ����
		int	  iLast;						// ָ����Ч��������(��)
		float *pfLineWidth;				// ����
		
		E_ExportLineDrawKind	eDrawKind;		// ����ߵĻ��Ʒ�ʽ E_ExportKLineDrawKind
		unsigned short			nLineWidth;		// ����
		unsigned short			isVirtualLine;	// ����
		T_ExportStringW			stStrLineName;	// ��������������ߵ�����
		
		int						nLooseValueCount;					// LooseValue������
		T_ExportLooseValue		*pstLooseValue;						// ĳЩָ������ж������ݣ���������ת���ַ�����ʽ
	}T_ExportIndexCalcResult;
}

#pragma pack(pop)

#endif //!_IEXPORTINDEXCALCSTRUCT_H_