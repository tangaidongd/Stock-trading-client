#ifndef _CONTROL_MSGDEF_H_
#define _CONTROL_MSGDEF_H_

// 非客户区的鼠标离开消息
#ifndef WM_NCMOUSELEAVE			
#define GGT_WM_NCMOUSELEAVE		(0x02A2)
#else
#define GGT_WM_NCMOUSELEAVE		(WM_NCMOUSELEAVE)
#endif

#ifndef TME_NONCLIENT
#define GGT_TME_NONCLIENT		(0x0010)
#else
#define GGT_TME_NONCLIENT		(TME_NONCLIENT)
#endif


#define WM_CONTROL_MSGBEGIN WM_USER+0x300	

enum E_ControlMsgDef
{
	UM_POS_CHANGED=WM_CONTROL_MSGBEGIN,	// msg  wParam:HWND lParam: 0-pos change, 1-range change
	UM_SETCOLOR,						//设置颜色 WPARAM COLORREF lParam 暂时没有使用
	UM_FindGGTongView,					//wparam 参数1 lparam 参数2 返回View
	//
	UM_TAB_RENAME,						//WPARAM OldName,Ret char *NewName 之后需要释放
	UM_SETGROUPID,						//设置GroupID WPARAM id
	UM_DoGGTongViewElement,				// wParam T_DoViewElementItem Ret bool
	UM_DoBiSplitElement,				// wParam T_DoViewElementItem Ret bool
	UM_ISKINDOFCMPIChildFrame,			/// Ret bool
	UM_ISKINDOFCGGTongView,				/// Ret bool
	UM_AddSplit,						///> wparam CMPIChildFrame::SD_TAB
	UM_DelSplit,						///> wparam CWnd *
	UM_TOXML,							//  WPARAM pActiveView Ret char *
	UM_SetBiSplitTrackDelFlag,			//WPARAM bool 
	UM_IsLockedSplit,					//Ret Bool 
	UM_SetF7AutoLock,					// wParam bool
	UM_IsF7AutoLock,					//Ret Bool
	UM_GetNowUseColor,					//> WPARAM 颜色的Index Ret 返回颜色值
	UM_GetSysColor,						//> WPARAM 颜色的Index Ret 返回颜色值
	UM_GetSysFontObject,				//WPARAM 颜色的Index Ret CFont *
	UM_GetIDRMainFram,					//获取主工程的IDR_MAINFRAM Ret UINT	
	UM_GetNodeBeginEnd,					//Wparam Int* LPARAM int *
	
		
};


#endif