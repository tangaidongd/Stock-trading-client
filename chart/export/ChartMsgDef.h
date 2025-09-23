

#ifndef _CHART_MSGDEF_H_
#define _CHART_MSGDEF_H_


#define WM_CHART_MSGBEGIN WM_USER+0x500	

enum E_ChartMsgDef
{
	UM_SELFDRAW_Color_Change = WM_CHART_MSGBEGIN,						// 自画线颜色设置
	UM_SetPickedSelfDrawCurve,	
	UM_UpdateSelfDrawBar,
	UM_IsShowVolBuySellColor,											//>Ret BOOL ///* Wpara CWnd *
	UM_SetActiveCrossFlag,				///WPARAM bool32
	UM_GetStaticMainKlineDrawStyle,		//Ret E_IndexKlineDrawStyle
	UM_IsHideMode,						///Ret bool
	UM_ISKINDOFCIoViewChart,			///Ret bool
	UM_IsActiveCross,					///Ret bool
	UM_GetCenterManager,						//CViewData *
};


#endif



