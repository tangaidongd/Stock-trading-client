#include "StdAfx.h"
#include "typedef.h"
#include "chartexport.h"

CHART_EXPORT const char* KIoViewChartSubRegion	= "SubRegionNumber";
CHART_EXPORT const char* KIoViewChartVolRegion	= "VolRegion";
CHART_EXPORT const char* KIoViewChartIndexs		= "Indexs";
CHART_EXPORT const char* KIoViewChartSelfDraws	= "SelfDraws";
CHART_EXPORT const char* KIoViewChartIndex		= "Index";
CHART_EXPORT const char* KIoViewChartIndexWidth		= "width";						//	Index 属性宽
CHART_EXPORT const char* KIoViewChartIndexCharacter	= "character";					//	Index 字符集
CHART_EXPORT const char* KIoViewChartIndexBlod		= "blod";						//	Index 文本是否加粗
CHART_EXPORT const char* KIoViewChartIndexTextSize		= "size";						//	Index 文本高度
CHART_EXPORT const char* KIoViewChartIndexNomarlBkColor	= "nomarlBKColor";			//	Index 控件一般状态背景色，值填充格式：#ffffffff
CHART_EXPORT const char* KIoViewChartIndexHotBkColor	= "hotBKColor";				//	Index 控件滑过状态背景色，值填充格式：#ffffffff
CHART_EXPORT const char* KIoViewChartIndexPressBkColor	= "pressBKColor";			//	Index 控件点击状态背景色，值填充格式：#ffffffff
CHART_EXPORT const char* KIoViewChartIndexNomarlTextColor	= "nomarlTextColor";	//	Index 控件一般状态文本色，值填充格式：#ffffffff
CHART_EXPORT const char* KIoViewChartIndexHotTextColor		= "hotTextColor";		//	Index 控件滑过状态文本色，值填充格式：#ffffffff
CHART_EXPORT const char* KIoViewChartIndexPressTextColor	= "pressTextColor";		//	Index 控件点击状态文本色，值填充格式：#ffffffff
CHART_EXPORT const char* KIoViewChartIndexOpenCfm	= "openCfm";					//	Index 控件点击是否打开版面

CHART_EXPORT const char* KIoViewChartIndexName	= "name";
CHART_EXPORT const char* KIoViewChartIndexDefName	= "defname";
CHART_EXPORT const char* KIoViewChartRegionId	= "region";
CHART_EXPORT const char* KIoViewChartRegionHeight= "regionheight";
CHART_EXPORT const char* KIoViewChartRegionHeightRatio = "regionHeightRadio";		// 保存一个副图高度与view高度比例值
CHART_EXPORT const char* KIoViewChartRegionMainIndex = "regionMainIndex";			// 是主指标还是叠加指标
CHART_EXPORT const char* KIoViewChartCmpMerchs   = "CmpMerchs";
CHART_EXPORT const char* KIoViewChartCmpMerch    = "CmpMerch";
CHART_EXPORT const char* KIoViewChartEnableRightY = "EnableRightY";
CHART_EXPORT const char* KIoViewChartAutoAddCmpExp = "AutoCmpExp";

CHART_EXPORT const char* KIoViewChartKLineInterval = "KLineInterval";
CHART_EXPORT const char* KIoViewChartKLineDrawType = "KLineDrawType";
CHART_EXPORT const char* KIoViewChartKLineTimeBegin= "KLineTimeBegin";
CHART_EXPORT const char* KIoViewChartNodeNumber	  = "NodeNumber";

CHART_EXPORT const char* KIoViewChartDrawTrendIndex= "DrawTrendIndex";
CHART_EXPORT const char* KIoViewChartDrawSaneIndex = "DrawSaneIndex";

CHART_EXPORT const char* KIoViewChartUserMinutes	  = "UserMinutes";
CHART_EXPORT const char* KIoViewChartUserDays	  = "UserDays";

CHART_EXPORT const char* KIoViewChartAxisYType     = "AxisYType";

CHART_EXPORT const char* KIoViewChartToolBarIndex   = "ToolBarIndexs";
CHART_EXPORT const char* KIoViewChartSpecialMerchBtn ="SpecialMerchBtn";
CHART_EXPORT const char* KIoViewChartSpecialMerchBtnItem ="Merch";
CHART_EXPORT const char* KIoViewChartSpecialMerchBtnName ="Name";
////////////////////////////////////////////////////////////////////////// add by weng.cx ,limit merch change
CHART_EXPORT const char* KIoViewChartLimitMerch ="LimitMerch";
CHART_EXPORT const char* KIoViewChartLimitMerchItem ="Merch";
CHART_EXPORT const char* KIoViewChartLimitMerchName ="Name";
CHART_EXPORT const char* KIoViewCharttLimitMarketId ="MarketId";
CHART_EXPORT const char* KIoViewCharttLimitMerchCode ="MerchCode";


CHART_EXPORT const char* KIoViewChartSpecialMerchBtnMerchCode ="MerchCode";
CHART_EXPORT const char* KIoViewChartSpecialMerchBtnMarket ="MarketId";
CHART_EXPORT const char* KIoViewChartTopToolBar     = "ShowTopToolBar";
CHART_EXPORT const char* KIoViewChartShowTopMerchBar= "ShowTopMerchBar";
CHART_EXPORT const char* KIoViewChartIndexToolBar   = "ShowIndexToolBar";
CHART_EXPORT const char* KIoViewChartHelpBtn		= "ShowHelp";
CHART_EXPORT const char* KIoViewChartShowExpBtn		= "ShowExpBtn";		// K线分时等图表上是否显示指数按钮

const int32	KSubRegionMaxNum = 5;
const uint32 KIoViewChartMsg2 = WM_USER + 1022;
const int32  KRequestNodeNumAdd	= 50;
