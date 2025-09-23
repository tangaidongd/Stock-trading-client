#ifndef _INDEX_MSGDEF_H_
#define _INDEX_MSGDEF_H_


#define WM_INDEX_MSGBEGIN WM_USER+0x400	

enum E_IndexMsgDef
{
	UM_Choose_Stock_Progress = WM_INDEX_MSGBEGIN,	// 条件选股时候的进度条
	UM_Index_Choose_Stock_Progress,					// 金盾选股时候的进度条
};


#endif