#ifndef _MAIN_MSG_DEF_H_
#define _MAIN_MSG_DEF_H_


#define WM_MAIN_MSGBEGIN WM_USER+0x100	


enum E_MainMsgDef
{
	UM_MY_ADD_MERCH  = WM_MAIN_MSGBEGIN,
	UM_MY_REMOVE_MERCH,
	TEST_VIEWDATA_INFOMINELIST_RESP,
	TEST_VIEWDATA_INFOMINECONTENT_RESP,
	
	UM_DOINITIALIZE,			// ��Ϊԭʼ���ݲ�����������ݣ����Բ�ȡ�ڳ�ʼ��ʱ������100�����ݣ�Ȼ��ֻ������µĶ�̬����


	TEMP_UM_CHOUMANOTIFY,

	UM_BLOCK_DOINITIALIZE,
	UM_MYMSG_BASE ,
	UM_DOCALC,
	UM_CALCMSG,
	UM_CALCMSG_END,

	UM_DO_INITFROMXML,	// �����ʼ������Ϣ

	UM_STARRY_INITIALIZE_ALLMERCHS,
	UM_STARRY_UPDATEXY,


	UM_DOTRACEMENU,

	UM_UserSelectListItem,

	UM_CHECK_CFM_HIDE,			// ���ҳ���Ƿ������ˣ���������ˣ���ʾ��~~

	UM_DO_ORGDATAUPDATE,

	UM_CHANGE_USERINFO_SUCC,	// �޸��û���Ϣ�ɹ�

	UM_WEB_COMMAND,				// web������
	UM_WEB_CALL_BACK,			// web����PC�Ļص�
};

#endif