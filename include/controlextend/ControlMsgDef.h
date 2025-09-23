#ifndef _CONTROL_MSGDEF_H_
#define _CONTROL_MSGDEF_H_

// �ǿͻ���������뿪��Ϣ
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
	UM_SETCOLOR,						//������ɫ WPARAM COLORREF lParam ��ʱû��ʹ��
	UM_FindGGTongView,					//wparam ����1 lparam ����2 ����View
	//
	UM_TAB_RENAME,						//WPARAM OldName,Ret char *NewName ֮����Ҫ�ͷ�
	UM_SETGROUPID,						//����GroupID WPARAM id
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
	UM_GetNowUseColor,					//> WPARAM ��ɫ��Index Ret ������ɫֵ
	UM_GetSysColor,						//> WPARAM ��ɫ��Index Ret ������ɫֵ
	UM_GetSysFontObject,				//WPARAM ��ɫ��Index Ret CFont *
	UM_GetIDRMainFram,					//��ȡ�����̵�IDR_MAINFRAM Ret UINT	
	UM_GetNodeBeginEnd,					//Wparam Int* LPARAM int *
	
		
};


#endif