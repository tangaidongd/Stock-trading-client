#include "StdAfx.h"
#include "typedef.h"
#include "chartexport.h"

CHART_EXPORT const char* KIoViewChartSubRegion	= "SubRegionNumber";
CHART_EXPORT const char* KIoViewChartVolRegion	= "VolRegion";
CHART_EXPORT const char* KIoViewChartIndexs		= "Indexs";
CHART_EXPORT const char* KIoViewChartSelfDraws	= "SelfDraws";
CHART_EXPORT const char* KIoViewChartIndex		= "Index";
CHART_EXPORT const char* KIoViewChartIndexWidth		= "width";						//	Index ���Կ�
CHART_EXPORT const char* KIoViewChartIndexCharacter	= "character";					//	Index �ַ���
CHART_EXPORT const char* KIoViewChartIndexBlod		= "blod";						//	Index �ı��Ƿ�Ӵ�
CHART_EXPORT const char* KIoViewChartIndexTextSize		= "size";						//	Index �ı��߶�
CHART_EXPORT const char* KIoViewChartIndexNomarlBkColor	= "nomarlBKColor";			//	Index �ؼ�һ��״̬����ɫ��ֵ����ʽ��#ffffffff
CHART_EXPORT const char* KIoViewChartIndexHotBkColor	= "hotBKColor";				//	Index �ؼ�����״̬����ɫ��ֵ����ʽ��#ffffffff
CHART_EXPORT const char* KIoViewChartIndexPressBkColor	= "pressBKColor";			//	Index �ؼ����״̬����ɫ��ֵ����ʽ��#ffffffff
CHART_EXPORT const char* KIoViewChartIndexNomarlTextColor	= "nomarlTextColor";	//	Index �ؼ�һ��״̬�ı�ɫ��ֵ����ʽ��#ffffffff
CHART_EXPORT const char* KIoViewChartIndexHotTextColor		= "hotTextColor";		//	Index �ؼ�����״̬�ı�ɫ��ֵ����ʽ��#ffffffff
CHART_EXPORT const char* KIoViewChartIndexPressTextColor	= "pressTextColor";		//	Index �ؼ����״̬�ı�ɫ��ֵ����ʽ��#ffffffff
CHART_EXPORT const char* KIoViewChartIndexOpenCfm	= "openCfm";					//	Index �ؼ�����Ƿ�򿪰���

CHART_EXPORT const char* KIoViewChartIndexName	= "name";
CHART_EXPORT const char* KIoViewChartIndexDefName	= "defname";
CHART_EXPORT const char* KIoViewChartRegionId	= "region";
CHART_EXPORT const char* KIoViewChartRegionHeight= "regionheight";
CHART_EXPORT const char* KIoViewChartRegionHeightRatio = "regionHeightRadio";		// ����һ����ͼ�߶���view�߶ȱ���ֵ
CHART_EXPORT const char* KIoViewChartRegionMainIndex = "regionMainIndex";			// ����ָ�껹�ǵ���ָ��
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
CHART_EXPORT const char* KIoViewChartShowExpBtn		= "ShowExpBtn";		// K�߷�ʱ��ͼ�����Ƿ���ʾָ����ť

const int32	KSubRegionMaxNum = 5;
const uint32 KIoViewChartMsg2 = WM_USER + 1022;
const int32  KRequestNodeNumAdd	= 50;
