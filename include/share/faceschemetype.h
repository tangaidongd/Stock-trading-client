#ifndef __FACE_SCHEME_TYPE_LL__H_
#define __FACE_SCHEME_TYPE_LL__H_

// 系统颜色
enum E_SysColor 
{
	ESCBackground = 0,		// 背景色

	ESCText,				// 普通文本颜色，默认文本颜色

	ESCChartAxisLine,		// chart图中坐标线颜色

	ESCKLineRise,			// K线上涨图形颜色
	ESCKLineKeep,			// K线持平图形颜色
	ESCKLineFall,			// K线下跌图形颜色

	ESCRise,				// 上涨颜色
	ESCKeep,				// 平盘颜色
	ESCFall,				// 下跌颜色

	ESCVolume,				// 成交量颜色
	ESCAmount,				// 成交金额颜色

	ESCGridLine,			// 表格分割线
	ESCGridSelected,		// 表格选择项颜色
	//...fangz0531		 xinjia
	ESCGridFixedBk,			// 表格固定项背景色
	//ESCGridFixedText,		// 表格固定项文字颜色
	ESCTitleBkColor,        //标题背景颜色

	ESCSpliter,				// 分割条颜色
	ESCVolume2,				// 报价表显示的成交量颜色

	ESCGuideLine1,			// 指标线1
	ESCGuideLine2,			// 指标线2
	ESCGuideLine3,			// 指标线3
	ESCGuideLine4,			// 指标线4
	ESCGuideLine5,			// 指标线5
	ESCGuideLine6,			// 指标线6
	ESCGuideLine7,			// 指标线7
	ESCGuideLine8,			// 指标线8
	ESCGuideLine9,			// 指标线9
	ESCGuideLine10,			// 指标线10
	ESCGuideLine11,			// 指标线11
	ESCGuideLine12,			// 指标线12
	ESCGuideLine13,			// 指标线13
	ESCGuideLine14,			// 指标线14
	ESCGuideLine15,			// 指标线15
	ESCGuideLine16,			// 指标线16


	// 
	ESCCount
};
// 系统字体
enum E_SysFont
{
	ESFNormal = 0,			// 正常字体
	ESFSmall,				// 小字体
	ESFBig,					// 大字体
	ESFText,				// 文本字体
	ESFCount				
};

#endif