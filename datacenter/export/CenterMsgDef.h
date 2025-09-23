#ifndef _CENTER_MSGDEF_H_
#define _CENTER_MSGDEF_H_

#define WM_CENTER_MSGBEGIN WM_USER+0x200	

enum E_CenterMsgDef
{
	// ��ݼ�������Ϣ
	UM_HOTKEY = WM_CENTER_MSGBEGIN,

	// MainFrame
	UM_MainFrame_OnStatusBarDataUpdate,				// ״̬�����ݸ���
	UM_MainFrame_MaxSize,							// ���
	UM_MainFrame_KickOut,
	UM_MainFrame_ManualOptimize,					// �ֶ���ѡ

	// ��ͷ�༭
	UM_GridHeader_Change,							// ��ͷ�����仯

	// �������
	UM_Font_Change,									// ���巢���仯
	UM_Color_Change,								// ��ɫ�����仯

	// ViewDataģ���Զ�����Ϣ
	UM_ViewData_OnTimerFreeMemory,					// ��ʱ�����ڴ�
	UM_ViewData_OnTimerSyncPushing,					// ��ʱͬ�������б�
	UM_ViewData_OnTimerInitializeMarket,			// ��ʼ���г�

	UM_ViewData_OnDataServiceConnected,				// ���ݷ���
	UM_ViewData_OnDataServiceDisconnected,			// 
	UM_ViewData_OnDataRequestTimeOut,				// 	
	UM_ViewData_OnDataCommResponse,					// 	
	UM_ViewData_OnDataServerConnected,				// 
	UM_ViewData_OnDataServerDisconnected,			// 
	UM_ViewData_OnDataServerLongTimeNoRecvData,
	UM_ViewData_OnKLineNodeUpdate,					//
	UM_ViewData_OnTimesaleUpdate,					//
	UM_ViewData_OnPublicFileUpdate,					//
	UM_ViewData_OnRealtimePriceUpdate,				// 
	UM_ViewData_OnRealtimeLevel2Update,				// 
	UM_ViewData_OnRealtimeTickUpdate,				// 
	UM_ViewData_OnNewsListUpdate,					// 
	UM_ViewData_OnF10Update,						// 
	UM_ViewData_OnLandMineUpdate,					// 

	UM_ViewData_OnGeneralNormalUpdate,				//
	UM_ViewData_OnGeneralFinanaceUpdate,			//
	UM_ViewData_OnMerchTrendIndexUpdate,			//
	UM_ViewData_OnMerchAuctionUpdate,				//
	UM_ViewData_OnMerchMinuteBSUpdate,				//
	UM_ViewData_OnClientTradeTimeUpdate,			//

	UM_ViewData_OnAuthSuccess,						// ��֤���
	UM_ViewData_OnAuthFail,
	UM_ViewData_OnMessage,		
	UM_ViewData_OnAllMarketInitializeSuccess,		// ��ʼ���ɹ�

	UM_ViewData_OnAuthSuccessTransToMain,			// ��֤�ɹ���Ѵ���ת�����߳�
	UM_ViewData_OnConfigSuccessTransToMain,			// ������Ϣ��ȡ�ɹ���ת�����߳�

	UM_RecentMerch_Update,							// �����Ʒ�б����

	UM_Dlghotkey_Close,								// ���̾���ر���
	UM_Download_Offlinedata_Progress,				// �ѻ����ݵĽ�����

	UM_IoViewTitle_Button_LButtonDown,				// ҵ����ͼ���ⰴť�������
	UM_IoViewTitle_Button_RButtonDown,				// ҵ����ͼ���ⰴť�Ҽ�����
	//

	UM_ViewData_OnNewsResponse,						// ��Ѷ����	
	UM_ViewData_OnNewsServerConnected,				// ��Ѷ����������
	UM_ViewData_OnNewsServerDisConnected,			// ��Ѷ����������

	UM_ViewData_OnBlockFileTraslateTimeout,			// ����ļ�ͬ����ʱ

	UM_SettingChanged,								// ȫ�����÷����˱��
	UM_ViewData_OnNewsTitleUpdate,                 //�Ƹ���Ѷ2013
	UM_ViewData_OnNewsContentUpdate,
	UM_ViewData_OnCodeIndexUpdate,
	UM_ViewData_OnNewsPushTitle,

	//add by hx
	UM_Package_TimeOut,
	UM_Package_LongTime,
	UM_Right_ShowTip,
	UM_User_Deal_Record,
	UM_ViewData_OnAuthPlugInHeart,					// �������

	UM_HasIndexRight,								// �Ƿ��и�ָ��Ȩ��


	//
	UM_JumpToTradingSoftware,						// �������Խ���ҳ��

	//
	UM_FIRM_TRADE_SHOW = 3000,						// ʵ�̽���

	// ģ�⽻�����
	UM_SIMULATE_TRADE_SHOW,							// ģ�⽻��
	UM_SIMULATE_TRADE_OPENACCOUNT,					// ʵ�̿���
	UM_SIMULATE_TRADE_HQREG,						// ����ע��
	UM_SIMULATE_TRADE_HQLOGIN,						// �����¼
	UM_SIMULATE_TRADE_IMGURL,						// ���ͼƬ
    UM_HQ_REGISTER_SUCCED,                          // ����ע��ɹ�
	UM_CEF_AFTERCREATED,							// cef�����ɹ�

	// 
	UM_ThirdLoginRsp,								// ��������¼
	UM_ReLoginPC,									// PC���µ�¼
	UM_PersonCenterRsp,								// ��������ҳ��
	UM_NativeOpenCfm,								// �򿪰���
	UM_LinkageWebTrend,								// �򿪷�ʱ
	UM_CLOSE_DLG,									// �رնԻ���
	UM_OpenOutWeb,									// �����ҳ��������������������
	UM_ShowWebDlg,									// �����ҳ������ӵ����Ի���
	UM_CloseOnlineServDlg,							// �ر����߿ͷ��Ի���

    //
    UM_News_Push_Message,                           // ������Ѷ��Ϣ

	UM_InfoCenter,									// ��Ѷ����

	UM_MainFrame_ChooseStock_Resp,                  // ѡ�ɻظ�
	UM_MainFrame_DapanState_Resp,                   // ����״̬
	UM_MainFrame_ChooseStockStatus_Resp,            // ѡ��״̬

	//���۱��������
	UM_IoViewReport_SortByRisePercent,              // �Ƿ�����
	UM_IoViewReport_Finance	,						// ��������

	UM_WEB_KEY_DOWN,                                // ��ҳ�Ǳ༭�����
	UM_IndexChsMideCore_KLineUpdate_Resp,			// ֪ͨ����ѡ�ɽ������仯
	UM_DownloadTrade,								// ���ؽ��׳���
	UM_ToolbarCommand,								// �����������Ϣ����

	// ����ѡ�����
	UM_PickModelTypeInfo,							// ����ѡ�����͵�ѡ���б�
	UM_PickModelTypeStatus,							// ����ѡ������ѡ��״̬���Ƿ���ѡ��
	UM_PickModelTypeEvent,							// ֻ��ѡ���¼�
	
	UM_MainFrame_NewStock_Resp,						// �¹���Ϣ

	//	bridge message
	UM_BridgeBegin = 4000,
	UM_BridgeEnd ,
	UM_BridgeGetNetBreedFailed,			
};

#endif