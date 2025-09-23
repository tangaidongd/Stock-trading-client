#ifndef _IEXPORTINDEXCALCSTRUCT_H_
#define _IEXPORTINDEXCALCSTRUCT_H_

#include "iExportQuoteStruct.h"

#pragma pack(push, 1)

// 注意命名空间
// 所有出现char字串都是utf8编码，但是由于code一般不出现中文，所以可以当成ascii使用
namespace cfjdExport
{
	typedef struct _ExportStringW
	{
		int		nStringLen;
		wchar_t *pwszString;
	}T_ExportStringW;
	
	// 计算结果的额外补充数据
	typedef struct _ExportLooseValue
	{
		int		nIndexInLine;				// 对应数据线结果的哪个索引节点
		float	fVal;						// 数据值
		T_ExportStringW stStrExtraData;		// 字符串值
	}T_ExportLooseValue;
	
	// K线绘制类型
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
	
	// 输出结果数据
	// 某些指标可能计算结果有多条数据线
	// 单条数据线结果数据
	typedef struct _ExportIndexCalcResult
	{ 
		float *pfLineData;				// 计算出来的数据线结果。索引对应输入的K线数据索引
		int	  iFirst;						// 指标有效开始索引
		int	  iLast;						// 指标有效结束索引(含)
		float *pfLineWidth;				// 保留
		
		E_ExportLineDrawKind	eDrawKind;		// 结果线的绘制方式 E_ExportKLineDrawKind
		unsigned short			nLineWidth;		// 保留
		unsigned short			isVirtualLine;	// 保留
		T_ExportStringW			stStrLineName;	// 计算出来的数据线的名称
		
		int						nLooseValueCount;					// LooseValue的数量
		T_ExportLooseValue		*pstLooseValue;						// 某些指标可能有额外数据，额外数据转成字符串格式
	}T_ExportIndexCalcResult;
}

#pragma pack(pop)

#endif //!_IEXPORTINDEXCALCSTRUCT_H_