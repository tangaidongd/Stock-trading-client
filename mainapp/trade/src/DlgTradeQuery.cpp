#include "StdAfx.h"

#include "DlgTradeQuery.h"
#include "XmlShare.h"
#include "facescheme.h"
#include "IoViewShare.h"
#include "DlgTip.h"
#include "DlgCloseTradeTip.h"
#include "FontFactory.h"
#include "coding.h"

#define GRID_HEAD_COLOR		RGB(0xF3, 0xF1, 0xEB)	// ��ͷ�ǵ���ɫ
#define GRID_BORDER_COLOR	RGB(0xE6, 0xE6, 0xE6)
#define GRID_HEAD_HEIGHT	35	// ��ͷ�ĸ߶�
#define GRID_ITEM_HEIGHT	35  // �����и�

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
#define		IDC_DEF_GROUPBOX_HOLDDETAIL		WM_USER+100
#define		IDC_DEF_STATIC_HOLDDETAIL		WM_USER+101
#define		IDC_DEF_GROUPBOX_ENTRUST		WM_USER+102
#define		IDC_DEF_STATIC_ENTRUST			WM_USER+103	
#define		IDC_DEF_GROUPBOX_ACCINFO		WM_USER+104
#define		IDC_DEF_STATIC_ACCINFO			WM_USER+105	
#define		IDC_DEF_GROUPBOX_ENTRUSTMAIN	WM_USER+106
#define		IDC_DEF_STATIC_ENTRUSTMAIN		WM_USER+107	
#define		IDC_DEF_GROUPBOX_DEAL			WM_USER+108
#define		IDC_DEF_STATIC_DEAL				WM_USER+109
#define		IDC_DEF_GROUPBOX_HOLDDETAILMAIN	WM_USER+110
#define		IDC_DEF_STATIC_HOLDDETAILMAIN	WM_USER+111
#define		IDC_DEF_GROUPBOX_HOLDTOTAL		WM_USER+112
#define		IDC_DEF_STATIC_HOLDTOTAL		WM_USER+113
#define		IDC_DEF_GROUPBOX_ACCINFOMAIN	WM_USER+114
#define		IDC_DEF_STATIC_ACCINFOMAIN		WM_USER+115
#define		IDC_DEF_GROUPBOX_COMMINFO		WM_USER+116
#define		IDC_DEF_STATIC_COMMINFO			WM_USER+117
	
#define		IDC_DEF_DROP_ENTRUSTMAIN_COMMINFO			WM_USER+120
#define		IDC_DEF_STATIC_ENTRUSTMAIN_COMMINFO			WM_USER+121
#define		IDC_DEF_DROP_ENTRUSTMAIN_TYPE				WM_USER+122
#define		IDC_DEF_STATIC_ENTRUSTMAIN_TYPE				WM_USER+123
#define		IDC_DEF_DROP_ENTRUSTMAIN_STATUS				WM_USER+124
#define		IDC_DEF_STATIC_ENTRUSTMAIN_STATUS			WM_USER+125
#define		IDC_DEF_BUTTON_ENTRUSTMAIN_QUERY			WM_USER+126
#define		IDC_DEF_BUTTON_ENTRUSTMAIN_RESET			WM_USER+127
#define		IDC_DEF_DROP_ENTRUSTMAIN_BUYSELL			WM_USER+128
#define		IDC_DEF_STATIC_ENTRUSTMAIN_BUYSELL			WM_USER+129
#define		IDC_DEF_DROP_ENTRUSTMAIN_OPENCLOSE			WM_USER+130
#define		IDC_DEF_STATIC_ENTRUSTMAIN_OPENCLOSE		WM_USER+131
	
#define		IDC_DEF_DROP_DEAL_COMMINFO					WM_USER+140
#define		IDC_DEF_STATIC_DEAL_COMMINFO				WM_USER+141
#define		IDC_DEF_DROP_DEAL_BUYSELL					WM_USER+142
#define		IDC_DEF_STATIC_DEAL_BUYSELL					WM_USER+143
#define		IDC_DEF_DROP_DEAL_OPENCLOSE					WM_USER+144
#define		IDC_DEF_STATIC_DEAL_OPENCLOSE				WM_USER+145
#define		IDC_DEF_BUTTON_DEAL_QUERY					WM_USER+146
#define		IDC_DEF_BUTTON_DEAL_RESET					WM_USER+147

#define		IDC_DEF_DROP_COMMINFO_TYPE					WM_USER+155
#define		IDC_DEF_STATIC_COMMINFO_TYPE				WM_USER+156
	
#define TIME_REFRESH_QUOTE				WM_USER+3000
#define TIME_REFRESH_HOLDE				WM_USER+3001
#define EM_Message_UpdataCommInfo		WM_USER+3010
#define EM_Message_WndClose				WM_USER+3011
	
#define TEXT_COLOR_RED			 RGB(255, 0, 0)
#define TEXT_COLOR_GRE			 RGB(0, 128, 0)
#define GRID_BK_COLOR			 RGB(255,255,255)
#define GRID_TEXT_COLOR			 RGB(77,77,77)

#define IDC_STATIC_DISENTRUST    1001

	enum
	{
		EGID_ENTRUST = 0x550,
		EGID_DEAL,
		EGID_HOLDDETAILMAIN,
		EGID_USERINFOMAIN,
		EGID_COMMINFO,
		EGID_HOLDTOTAL,
	};
	
	const CString s_TabName[] = {L"�ֲ�11��ϸ",L"ָ��ί�е�",L"�˻���Ϣ",
							L"ƽ�ֵ�",L"��Ʒ��Ϣ"};
	
	static const int s_kiXScrollBarHoldHID = 0x498;		// �ֲ�ˮƽ
	static const int s_kiXScrollBarHoldVID = 0x499;		// �ֲ���ֱ
	
	static const int s_kiXScrollBarBaseID = 0x500;		// ��������ʼid
	static const int s_kiXScrollBarBaseHorzID = 0;		// ˮƽ��������ʼid

	// ���й�����
	static const int s_kiXScrollBarHoldDetaiHID = 0x510;		// �ֲ�ˮƽ
	static const int s_kiXScrollBarHoldDetaiVID = 0x511;		// �ֲ���ֱ
	
	static const int s_kiXScrollBarEntrustHID = 0x512;
	static const int s_kiXScrollBarEntrustVID = 0x513;
	
	static const int s_kiXScrollBarAccInfoHID = 0x514;
	static const int s_kiXScrollBarAccInfoVID = 0x515;
	
	static const int s_kiXScrollBarEntrustMainHID = 0x516;
	static const int s_kiXScrollBarEntrustMainVID = 0x517;
	
	static const int s_kiXScrollBarDealHID = 0x518;
	static const int s_kiXScrollBarDealVID = 0x519;
	
	static const int s_kiXScrollBarHoldDetailMainHID = 0x520;
	static const int s_kiXScrollBarHoldDetailMainVID = 0x521;
	
	static const int s_kiXScrollBarHoldTotalHID = 0x522;
	static const int s_kiXScrollBarHoldTotalVID = 0x523;
	
	static const int s_kiXScrollBarAccInfoMainHID = 0x524;
	static const int s_kiXScrollBarAccInfoMainVID = 0x525;
	
	static const int s_kiXScrollBarCommInfoHID = 0x526;
	static const int s_kiXScrollBarCommInfoVID = 0x527;
	
	enum E_TradeGridHeadType
	{
		// �ɽ�����
		ETGHT_DEAL_BEGIN = 0,
			
			ETGHT_DEAL_TradeNumber,	
			ETGHT_DEAL_CommodityID,	
			ETGHT_DEAL_BuySell,	
			ETGHT_DEAL_Qty,		
			ETGHT_DEAL_TradePrice,
			ETGHT_DEAL_CloseProfit,		
			ETGHT_DEAL_Fee,	
			ETGHT_DEAL_OrderNumber,		
	//		ETGHT_DEAL_HoldNumber,
	//		ETGHT_DEAL_OpenClose,	
	//		ETGHT_DEAL_OtherID,		
	//		ETGHT_DEAL_TradeType,
	//		ETGHT_DEAL_OperateType,	
			ETGHT_DEAL_TradeTime,	
	//		ETGHT_DEAL_TraderID,	
	//		ETGHT_DEAL_DealerID,	
	//		ETGHT_DEAL_OpenPrice,	
	//		ETGHT_DEAL_Margin,	
	//		ETGHT_DEAL_HoldPrice,	
	//		ETGHT_DEAL_OpenTime,	

			// �ֲ���ϸ
			ETGHT_HOLDDETAIL_Begin	= 600,
			ETGHT_HOLDDETAIL_HoldNumber,	
			ETGHT_HOLDDETAIL_CommodityID,
			ETGHT_HOLDDETAIL_BuySell,	
		//	ETGHT_HOLDDETAIL_OpenQty,	
			ETGHT_HOLDDETAIL_HoldQty,	
			ETGHT_HOLDDETAIL_OpenPrice,	
			ETGHT_HOLDDETAIL_HoldPrice,	
			ETGHT_HOLDDETAIL_ClosePrice,	
			ETGHT_HOLDDETAIL_StopLoss,	
			ETGHT_HOLDDETAIL_StopProfit,	
			ETGHT_HOLDDETAIL_FloatPL,	
			ETGHT_HOLDDETAIL_Margin,	
			ETGHT_HOLDDETAIL_MarginRate,		
			ETGHT_HOLDDETAIL_OpenTime,	
			ETGHT_HOLDDETAIL_TradeNumber,
			ETGHT_HOLDDETAIL_Fee,
			ETGHT_HOLDDETAIL_OtherID,
	//		ETGHT_HOLDDETAIL_AgentID,

			ETGHT_HOLDSUMMARY_GoodsName,
			ETGHT_HOLDSUMMARY_BuySell,
			ETGHT_HOLDSUMMARY_AvgHoldPrice,
			ETGHT_HOLDSUMMARY_HoldCount,
			ETGHT_HOLDSUMMARY_FloatProfitAndLoss,
			ETGHT_HOLDSUMMARY_MARGIN,

			// �˻���Ϣ
			ETGHT_ACCINFO_Begin	= 650,
			ETGHT_ACCINFO_TraderID,				
			ETGHT_ACCINFO_TraderName,				
			ETGHT_ACCINFO_InitialRights,			
			ETGHT_ACCINFO_LastMargin,				
			ETGHT_ACCINFO_LastFloatPL,
			ETGHT_ACCINFO_LastDelayFee,			
			ETGHT_ACCINFO_CurrentMargin,
			ETGHT_ACCINFO_AvailableMargin,
			ETGHT_ACCINFO_CurrentFloatPL,			
			ETGHT_ACCINFO_FreezeFund,
			ETGHT_ACCINFO_FreezeMargin,			
			ETGHT_ACCINFO_FreezeFee,				
			ETGHT_ACCINFO_OtherFreeezeMoney,
			ETGHT_ACCINFO_CurrentAvailableMoney,
			ETGHT_ACCINFO_CurrentFee,				
			ETGHT_ACCINFO_CurrentPL,
			ETGHT_ACCINFO_CurrentRights,
			ETGHT_ACCINFO_MoneyInOut,
			ETGHT_ACCINFO_HoldingPL,
			ETGHT_ACCINFO_FundRisks,				
			ETGHT_ACCINFO_OtherChange,
			ETGHT_ACCINFO_MarketState,
			ETGHT_ACCINFO_AccuntState,	
			
			ETGHT_ACCINFO_Name1, // ��Ŀ/��Ŀֵ
			ETGHT_ACCINFO_Value1,
			ETGHT_ACCINFO_Name2, 
			ETGHT_ACCINFO_Value2,
			ETGHT_ACCINFO_Name3,
			ETGHT_ACCINFO_Value3,

			// ί��
			ETGHT_ENTRUST_Begin	= 700,
			ETGHT_ENTRUST_Operator,
			ETGHT_ENTRUST_OrderNumber,	
			ETGHT_ENTRUST_Time,			
//			ETGHT_ENTRUST_State,			
			ETGHT_ENTRUST_BuySell,		
			ETGHT_ENTRUST_OpenClose,		
//			ETGHT_ENTRUST_OrderType,		
			ETGHT_ENTRUST_TraderID,		
			ETGHT_ENTRUST_DealerID,
			ETGHT_ENTRUST_CommodityID,	
			ETGHT_ENTRUST_Price,			
			ETGHT_ENTRUST_FreezeMargin,
			ETGHT_ENTRUST_FreezeFee,		
			ETGHT_ENTRUST_Quantity,		
			ETGHT_ENTRUST_RevokeTime,		
			ETGHT_ENTRUST_StopLoss,		
			ETGHT_ENTRUST_StopProfit,		
			ETGHT_ENTRUST_OrderFirmID,	
	//		ETGHT_ENTRUST_AgentID,
			ETGHT_ENTRUST_HoldingNumber,

			// �ֲֻ���
			ETGHT_HOLDTOTAL_Begin	= 750,
			ETGHT_HOLDTOTAL_CommodityID,		
			ETGHT_HOLDTOTAL_BuySell,			
			ETGHT_HOLDTOTAL_Qty,				
			ETGHT_HOLDTOTAL_OpenAveragePrice,	
			ETGHT_HOLDTOTAL_HoldAveragePrice,	
			ETGHT_HOLDTOTAL_ColsePrice,			
			ETGHT_HOLDTOTAL_FloatPL,			
			ETGHT_HOLDTOTAL_Margin,				
			ETGHT_HOLDTOTAL_FreezeQty,			
			ETGHT_HOLDTOTAL_Fee,

	};
	
	struct T_TradeGridHead 
	{
		int		iHeadType;
		TCHAR	szName[30];
		int		iColWidth;
		UINT	nFmt;
	};
	
#ifndef ArraySize
#define ArraySize(Array) (sizeof(Array)/sizeof(Array[0]))
#endif
	
	static const T_TradeGridHead  s_kaHoldDetailHeader[] = 
	{
		{ETGHT_HOLDDETAIL_HoldNumber, _T("�ֲֵ���"), 110, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDDETAIL_CommodityID, _T("��Ʒ����"), 100, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDDETAIL_BuySell, _T("��/��"), 60, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
//		{ETGHT_HOLDDETAIL_OpenQty, _T("������"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDDETAIL_HoldQty, _T("�ֲ�"), 60, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDDETAIL_FloatPL, _T("���ո���ӯ��"), 100, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDDETAIL_OpenPrice, _T("���ּ�"), 80, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDDETAIL_HoldPrice, _T("�ֲּ�"), 80, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDDETAIL_ClosePrice, _T("ƽ�ּ�"), 80, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDDETAIL_StopLoss, _T("ֹ���"), 80, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDDETAIL_StopProfit, _T("ֹӯ��"), 80, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		
		{ETGHT_HOLDDETAIL_MarginRate, _T("�ֱֲ�֤�����"), 110, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDDETAIL_Margin, _T("ռ�ñ�֤��"), 100, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDDETAIL_OpenTime, _T("����ʱ��"), 150, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
//		{ETGHT_HOLDDETAIL_AgentID, _T("�绰����Ա"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER}
	};
	static const int s_kiHoldDetailHeaderCount = ArraySize(s_kaHoldDetailHeader);

	static const T_TradeGridHead  s_kaHoldSummaryHeader[] = 
	{
		{ETGHT_HOLDSUMMARY_GoodsName, _T("��Ʒ����"), 100, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDSUMMARY_BuySell, _T("��/��"), 100, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDSUMMARY_AvgHoldPrice, _T("ƽ���ֲּ�"), 100, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDSUMMARY_HoldCount, _T("����"), 100, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDSUMMARY_FloatProfitAndLoss, _T("���ո���ӯ��"), 100, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDSUMMARY_MARGIN, _T("ռ�ñ�֤��"), 100, DT_RIGHT|DT_SINGLELINE|DT_VCENTER}
	};
	static const int s_kiHoldSummaryHeaderCount = ArraySize(s_kaHoldSummaryHeader);

	static const T_TradeGridHead  s_kaEntrustHeader[] = 
	{
		{ETGHT_ENTRUST_Operator,	 _T("����"), 50, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_OrderNumber,	 _T("ί�е���"), 110, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_CommodityID,	 _T("��Ʒ����"), 100, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_BuySell,		 _T("��/��"), 60, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_Quantity,	 _T("����"), 60, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_Price,		 _T("ί�м�"), 80, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_StopLoss,	 _T("ֹ���"), 80, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_StopProfit,	 _T("ֹӯ��"), 80, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_FreezeMargin, _T("���ᱣ֤��"), 100, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_FreezeFee,	 _T("����������"), 100, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
//		{ETGHT_ENTRUST_State,		 _T("ί�е�״̬"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
//		{ETGHT_ENTRUST_OrderType,	 _T("ί������"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
// 		{ETGHT_ENTRUST_OpenClose,	 _T("����/ƽ��"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
// 		{ETGHT_ENTRUST_HoldingNumber, _T("ƽָ���ֲֺ�"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_RevokeTime,	 _T("��Ч����"), 80, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
//		{ETGHT_ENTRUST_AgentID,	 _T("�绰����Ա"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER}
// 		{ETGHT_ENTRUST_Time,		 _T("ί��ʱ��"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER}
	};
	static const int s_kiEntrustHeaderCount = ArraySize(s_kaEntrustHeader);

	
	static const T_TradeGridHead  s_kaAccInfoHeader[] = 
	{
		{ETGHT_ACCINFO_TraderName,				_T("�˻�����"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ACCINFO_CurrentRights,			_T("��ǰȨ��"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ACCINFO_CurrentFloatPL,			_T("����ӯ��"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ACCINFO_AvailableMargin,			_T("���ñ�֤��"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ACCINFO_CurrentMargin,			_T("ռ�ñ�֤��"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ACCINFO_FreezeMargin,			_T("���ᱣ֤��"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ACCINFO_FreezeFee,				_T("����������"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ACCINFO_FundRisks,				_T("������"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER}
	};
	static const int s_kiAccInfoHeaderCount = ArraySize(s_kaAccInfoHeader);
	
	static const T_TradeGridHead  s_kaEntrustMainHeader[] = 
	{
		{ETGHT_ENTRUST_OrderNumber,	 _T("ί�е���"), 110, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_CommodityID,	 _T("��Ʒ����"), 100, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_BuySell,		 _T("��/��"), 60, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_Quantity,	 _T("����"), 60, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_Price,		 _T("ί�м�"), 80, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_StopLoss,	 _T("ֹ���"), 80, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_StopProfit,	 _T("ֹӯ��"), 80, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_FreezeMargin, _T("���ᱣ֤��"), 100, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_FreezeFee,	 _T("����������"), 100, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
//		{ETGHT_ENTRUST_State,		 _T("ί�е�״̬"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
//		{ETGHT_ENTRUST_OrderType,	 _T("ί������"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_OpenClose,	 _T("����/ƽ��"), 60, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_HoldingNumber, _T("ƽָ���ֲֺ�"), 110, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ENTRUST_RevokeTime,	 _T("��Ч����"), 80, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
//		{ETGHT_ENTRUST_AgentID,	 _T("�绰����Ա"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER}
// 		{ETGHT_ENTRUST_Time,		 _T("ί��ʱ��"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
// 		{ETGHT_ENTRUST_DealerID, _T("123"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
// 		{ETGHT_ENTRUST_OrderFirmID,	 _T("123"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
// 		{ETGHT_ENTRUST_AgentID, _T("123"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
	};
	static const int s_kiEntrustMainHeaderCount = ArraySize(s_kaEntrustMainHeader);
	
	static const T_TradeGridHead  s_kaDealHeader[] = 
	{
		{ETGHT_DEAL_TradeNumber, _T("����"), 110, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_DEAL_CommodityID, _T("��Ʒ����"), 100, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_DEAL_BuySell,	 _T("��/��"), 60, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_DEAL_Qty,		 _T("����"), 60, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_DEAL_TradePrice, _T("ƽ�ּ۸�"), 80, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_DEAL_CloseProfit, _T("ӯ��"), 120, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_DEAL_Fee,	 _T("������"), 90, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_DEAL_OrderNumber, _T("���ֵ���"), 110, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
//		{ETGHT_DEAL_HoldNumber, _T("�ֲֵ���"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
//		{ETGHT_DEAL_OpenClose,	 _T("����/ƽ��"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
//		{ETGHT_DEAL_OtherID,		 _T("���׶���"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
//		{ETGHT_DEAL_TradeType, _T("�ɽ�����"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
//		{ETGHT_DEAL_OperateType, _T("��������"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_DEAL_TradeTime,	 _T("ƽ��ʱ��"), 150, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
// 		{ETGHT_DEAL_TraderID,	 _T("����ԱID"), 130, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
// 		{ETGHT_DEAL_OpenPrice,	 _T("���ּ�"), 130, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
// 		{ETGHT_DEAL_Margin,	 _T("��֤��"), 130, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
// 		{ETGHT_DEAL_HoldPrice,	 _T("�ֲּ�"), 130, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
// 		{ETGHT_DEAL_OpenTime, _T("����ʱ��"), 130, DT_LEFT|DT_SINGLELINE|DT_VCENTER}
	};
	static const int s_kiDealHeaderCount = ArraySize(s_kaDealHeader);

	static const T_TradeGridHead  s_kaHoldTotalHeader[] = 
	{
		{ETGHT_HOLDTOTAL_CommodityID,	 _T("��Ʒ����"), 100, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDTOTAL_BuySell,		 _T("��/��"), 100, DT_CENTER|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDTOTAL_Qty,			 _T("����"), 100, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDTOTAL_OpenAveragePrice, _T("���־���"), 100, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDTOTAL_HoldAveragePrice, _T("�ֲ־���"), 100, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDTOTAL_ColsePrice,		 _T("ƽ�ּ�"), 100, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDTOTAL_FloatPL,		 _T("����ӯ��"), 100, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_HOLDTOTAL_Margin,			 _T("ռ�ñ�֤��"), 100, DT_RIGHT|DT_SINGLELINE|DT_VCENTER},
// 		{ETGHT_HOLDTOTAL_FreezeQty,		 _T("��������"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
// 		{ETGHT_HOLDTOTAL_Fee,			_T("������"), 100, DT_LEFT|DT_SINGLELINE|DT_VCENTER}
	};
	static const int s_kiHoldTotalHeaderCount = ArraySize(s_kaHoldTotalHeader);

	static const T_TradeGridHead  s_kaAccInfoMainHeader[] = 
	{
		{ETGHT_ACCINFO_Name1,	 _T("��Ŀ"), 165, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ACCINFO_Value1,	 _T("��Ŀֵ"), 165, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ACCINFO_Name2,	 _T("��Ŀ"), 165, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ACCINFO_Value2,	 _T("��Ŀֵ"), 165, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ACCINFO_Name3,	 _T("��Ŀ"), 165, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ACCINFO_Value3,	 _T("��Ŀֵ"), 160, DT_LEFT|DT_SINGLELINE|DT_VCENTER}
	};
	static const int s_kiAccInfoMainHeaderCount = ArraySize(s_kaAccInfoMainHeader);
	
	static const T_TradeGridHead  s_kaCommInfoHeader[] = 
	{
		{ETGHT_ACCINFO_Name1,	 _T("��Ŀ"), 165, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ACCINFO_Value1,	 _T("��Ŀֵ"), 165, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ACCINFO_Name2,	 _T("��Ŀ"), 165, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ACCINFO_Value2,	 _T("��Ŀֵ"), 165, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ACCINFO_Name3,	 _T("��Ŀ"), 165, DT_LEFT|DT_SINGLELINE|DT_VCENTER},
		{ETGHT_ACCINFO_Value3,	 _T("��Ŀֵ"), 160, DT_LEFT|DT_SINGLELINE|DT_VCENTER}
	};
	static const int s_kiCommInfoHeaderCount = ArraySize(s_kaCommInfoHeader);


	struct T_TradeGridHeadGroup 
	{
		TCHAR					szGroupName[30];
		const T_TradeGridHead	*pstGridHead;
		int						iGridHeadCount;
		int						iFixedColumnCount;
		COLORREF				clrFixedRow;
	};

	static const T_TradeGridHeadGroup s_kstTradeGridHeadGroupEntrust = 
	{_T("ί�в�ѯ"), s_kaEntrustHeader, s_kiEntrustHeaderCount, 0, RGB(0,0,0)};
	
	static const T_TradeGridHeadGroup s_kstTradeGridHeadGroupAccInfo = 
	{_T("�˻���Ϣ"), s_kaAccInfoHeader, s_kiAccInfoHeaderCount, 0, RGB(0,0,0)};
	
	static const T_TradeGridHeadGroup s_kstTradeGridHeadGroupEntrustMain = 
	{_T("ί�в�ѯ"), s_kaEntrustMainHeader, s_kiEntrustMainHeaderCount, 0, RGB(0,0,0)};
	
	static const T_TradeGridHeadGroup s_kstTradeGridHeadGroupDeal = 
	{_T("ƽ�ֵ�"), s_kaDealHeader, s_kiDealHeaderCount, 0, RGB(0,0,0)};

	static const T_TradeGridHeadGroup s_kstTradeGridHeadGroupHoldDetailMain = 
	{_T("�ֲ���ϸ"), s_kaHoldDetailHeader, s_kiHoldDetailHeaderCount, 0, RGB(0,0,0)};

	static const T_TradeGridHeadGroup s_kstTradeGridHeadGroupHoldSummary = 
	{_T("�ֲֻ���"), s_kaHoldSummaryHeader, s_kiHoldSummaryHeaderCount, 0, RGB(0,0,0)};
	
	static const T_TradeGridHeadGroup s_kstTradeGridHeadGroupHoldTotal = 
	{_T("�ֲֻ���"), s_kaHoldTotalHeader, s_kiHoldTotalHeaderCount, 0, RGB(0,0,0)};
	
	static const T_TradeGridHeadGroup s_kstTradeGridHeadGroupCommInfo = 
	{_T("��Ʒ��Ϣ"), s_kaCommInfoHeader, s_kiCommInfoHeaderCount, 0, RGB(0,0,0)};

	static const T_TradeGridHeadGroup s_kstTradeGridHeadGroupAccInfoMain = 
	{_T("�˻���Ϣ"), s_kaAccInfoMainHeader, s_kiAccInfoMainHeaderCount, 0, RGB(0,0,0)};

}
/*
	error 552: (Warning -- Symbol 'm_gGridEntrust' (line 457, file .\trade\src\DlgTradeQuery.cpp) not accessed)
*/
//lint --e{552}
CGridCtrlNormal	*m_gGridEntrust = NULL;

BEGIN_MESSAGE_MAP(CDlgTradeQuery, CDialog)
//{{AFX_MSG_MAP(CDlgTradeQuery)
ON_WM_ERASEBKGND()
ON_WM_SIZE()
ON_WM_DESTROY()
ON_COMMAND(IDC_DEF_BUTTON_ENTRUSTMAIN_RESET, OnBtnEntrustMainReset)
ON_COMMAND(IDC_DEF_BUTTON_ENTRUSTMAIN_QUERY, OnBtnEntrustMainQuery)
ON_COMMAND(IDC_DEF_BUTTON_DEAL_RESET, OnBtnDealReset)
ON_COMMAND(IDC_DEF_BUTTON_DEAL_QUERY, OnBtnDealQuery)
ON_CBN_SELCHANGE(IDC_DEF_DROP_COMMINFO_TYPE, OnCbnSelchangeCommInfoType)
ON_WM_CTLCOLOR()
ON_WM_TIMER()
ON_NOTIFY(NM_RCLICK, EGID_ENTRUST, OnGridRButtonDown)
ON_NOTIFY(NM_RCLICK, EGID_DEAL, OnGridRButtonDown)
ON_NOTIFY(NM_RCLICK, EGID_HOLDDETAILMAIN, OnGridRButtonDown)
ON_NOTIFY(NM_RCLICK, EGID_USERINFOMAIN, OnGridRButtonDown)
ON_NOTIFY(NM_RCLICK, EGID_COMMINFO, OnGridRButtonDown)
ON_NOTIFY(NM_RCLICK, EGID_HOLDTOTAL, OnGridRButtonDown)
ON_MESSAGE(EM_Message_UpdataCommInfo,OnMsgUpdataCommIfo)
ON_MESSAGE(EM_Message_WndClose,OnMsgWndClose)
ON_MESSAGE(WM_POSCHANGED_GROUPBOX,OnMsgGroupBoxPosChanged)
ON_NOTIFY(NM_DBLCLK, EGID_ENTRUST, OnGridRDBButtonDown)
ON_NOTIFY(GVN_SELCHANGED, EGID_HOLDDETAILMAIN, OnGridSelChange)
ON_NOTIFY(GVN_SELCHANGED, EGID_HOLDTOTAL, OnGridSelChange)
ON_WM_LBUTTONUP()
ON_MESSAGE(WM_DISENTRUST, OnDisEntrust)
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CDlgTradeQuery::CDlgTradeQuery( CWnd* pParent ) : CDialog(IDD, pParent)
{
	m_pTradeBid = NULL;
	m_eTradeStatus = ETLS_NotLogin;

	m_pGridEvent = NULL;
	m_pTradeLoginInfo = NULL;

	m_lWaitTradeDTPProcess = false;
	m_bInitCommInfo = false;

	m_pHoldDetailSelect = NULL;
	m_pHoldDetailMainSelect = NULL;
	m_pHoldSummarySelect = NULL;
	
	m_bActivaGroupHoldDetail = FALSE;
	m_bActivaGroupEntrust = FALSE;

	m_iAllHoldDetailMainHeight = 0;
	m_iAllHoldSummaryMainHeight = 0;
	m_iAllHoldDetailHeight = 0;
	m_iAllEntrustMainHeight = 0;
	m_iAllEntrustHeight = 0;
	m_iAllDealHeight = 0;
	m_iAllCommInfoHeight = 0;
	m_iAllAccInfoHeight = 0;
	m_bNetWorkStatus = FALSE;
	m_strMidCode = "";

	m_bAccountInfo = true;

	m_eCurQueryType = EQT_QueryNone;

	m_gGridEntrust = &m_GridEntrust;
	m_iReqEntrustCnt = 0;
	m_iRevokeLimitOrderCnt = 0;
	m_dFeeScale = 0.0005;
}

CDlgTradeQuery::~CDlgTradeQuery()
{	
	{
		for ( int i=0; i<m_aScroll.size(); ++i )
		{
			DEL(m_aScroll[i]);
		}
		m_aScroll.clear();
	}
	// 	// �첽�����߳�
 	CExportMT2DispatchThread::Instance().RemoveProcessor(this);
}

void CDlgTradeQuery::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTradeQuery)
	//DDX_Control(pDX, IDC_STATIC_ACCOUNT, m_StaticAccount);
	//}}AFX_DATA_MAP
}

BOOL CDlgTradeQuery::OnInitDialog()
{
	CDialog::OnInitDialog();

	//CFont *pFont = new CFont;
	WCHAR *pFontName = gFontFactory.GetExistFontName(L"΢���ź�");
	//if (pFont->m_hObject != NULL)
	//	pFont->Detach();

	VERIFY(m_Font.CreateFont(
		-14,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		pFontName));		   // lpszFacename	//...
	
	// ���������
	/* ί�в�ѯ */
	CreateEntrustMainTable(&m_Font);
	/* �ſ� ָ��ί�е�*/
	CreateEntrustTable(&m_Font);
	/* ƽ�ֵ���ѯ */
	CreateDealTable(&m_Font);
	/* �ֲ���ϸ */
	CreateHoldDetailMainTable(&m_Font);
	/* �ֲֻ��� */
	CreateHoldSummaryMainTable(&m_Font);
	/* ��Ʒ��Ϣ */
	CreateCommInfoTable(&m_Font);
	/* �˻���Ϣ */
	CreateUserInfoMainTable(&m_Font);

	// ������ҳ
	m_dlgHomePage.Create(this);


	CreateControl();	// �����ؼ�
	InitComBox(&m_Font);		// ��ʼ��combox

	ShowQueryChange(EQT_QueryHome);
	
	// 	// �첽�����߳�
	CExportMT2DispatchThread::Instance().AddProcessor(this);

	return TRUE;
}

void CDlgTradeQuery::OnOK()
{
}

void CDlgTradeQuery::OnCancel()
{
}

BOOL CDlgTradeQuery::OnEraseBkgnd( CDC* pDC )
{
	return true;
}

void CDlgTradeQuery::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);

	RecalcLayout(false);
}

void CDlgTradeQuery::RecalcLayout( bool bNeedDraw )
{
	const int iHorzHeight = 18;			// �������߶�
	const int iGroupSpaceToTable = 10;	// ��ϸgroup���ǩ����
	const int iGroupSpace = 10;			// group top��list����
	//const int iGroupToGroupSpace = 10;	// group ֮�����
	const int iStaticTitleLeft = 20;	// group������group��߾���
	const int iOneWordHeight = 16;		// 1�����ָ߶�
	const int iOneWordWidth = 20;		// 1�����ֿ��
	const int iGroupToDrop = 25;		// group top�����������
	const int iDropTitleLeft = 15;		// drop ��group��߾���
	const int iDropHeight = 30;			// ������߶�
	const int iDropWidth = 150;			// ��������
	const int iDropSpace = 10;			// ������֮��ľ���
	const int iButtonWidth = 80;			// button���
	const int iButtonHeight = 30;			// button�߶�
	const int iHomeWordHeight = 50;

	CRect rc(0,0,0,0);
	GetClientRect(rc);

	if (m_eCurQueryType == EQT_QueryHome)
	{
		CRect rectHomePage(rc);
		rectHomePage.bottom = rectHomePage.top + iHomeWordHeight;
		m_dlgHomePage.MoveWindow(rectHomePage);
		m_dlgHomePage.AdjustAllStaticSize();

		// list
		CRect rectListHoldDetailMain(rc);
		rectListHoldDetailMain.left = rc.left;
		rectListHoldDetailMain.right = rc.right;
		rectListHoldDetailMain.top = rectHomePage.bottom + iGroupSpace;	// list�����������
		rectListHoldDetailMain.bottom = rc.bottom;
		//ˮƽ������
		CRect rcHoldDetailMainHBar(0,0,0,0);
		if ( rectListHoldDetailMain.Height() > iHorzHeight )
		{
			CRect rcHorz(rectListHoldDetailMain);
			rcHorz.top = rectListHoldDetailMain.bottom - iHorzHeight;
			rectListHoldDetailMain.bottom = rcHorz.top;
			rcHoldDetailMainHBar = rcHorz;
		}
		//��ֱ������
		CRect rcHoldDetailMainVBar(0,0,0,0);
		if ( (rectListHoldDetailMain.Height() < m_iAllHoldDetailMainHeight) && m_iAllHoldDetailMainHeight > 20 )
		{
			CRect rcVer(rectListHoldDetailMain);
			rcVer.left = rectListHoldDetailMain.right - iHorzHeight;
			rectListHoldDetailMain.right = rcVer.left;
			rcHoldDetailMainVBar = rcVer;
		}

		if (m_GridHoldDetailMain.m_hWnd)
		{
			m_GridHoldDetailMain.MoveWindow(rectListHoldDetailMain);
		}

		if (m_XBarHoldDetailMainH.m_hWnd)
		{
			m_XBarHoldDetailMainH.SetSBRect(rcHoldDetailMainHBar);
		}

		if (m_XBarHoldDetailMainV.m_hWnd)
		{
			m_XBarHoldDetailMainV.SetSBRect(rcHoldDetailMainVBar);
		}
	}
	else if (m_eCurQueryType == EQT_QueryEntrust)
	{
		/* ί�в�ѯ */
		// groupbox
		CRect rectGroupEntrustMain(rc);
		rectGroupEntrustMain.top = rc.top+iGroupSpaceToTable;
		rectGroupEntrustMain.bottom = rc.bottom;
		rectGroupEntrustMain.right =rc.right;
		rectGroupEntrustMain.left =rc.left;
		m_GroupEntrustMain.MoveWindow(rectGroupEntrustMain);
		// title
		CRect rectStaticEntrustMain(rectGroupEntrustMain);
		rectStaticEntrustMain.top = rectGroupEntrustMain.top;
		rectStaticEntrustMain.bottom = rectStaticEntrustMain.top+iOneWordHeight;
		rectStaticEntrustMain.left = rectGroupEntrustMain.left+iStaticTitleLeft;
		rectStaticEntrustMain.right = rectStaticEntrustMain.left+iOneWordWidth*4;	// 4���ֵĿ��
		m_StaticEntrustMain.MoveWindow(rectStaticEntrustMain);
		// ��Ʒ��������
		CRect rectStaticEntrustMainCommInfo(rectGroupEntrustMain);
		rectStaticEntrustMainCommInfo.top = rectGroupEntrustMain.top+iGroupToDrop;
		rectStaticEntrustMainCommInfo.bottom = rectStaticEntrustMainCommInfo.top+iOneWordHeight;
		rectStaticEntrustMainCommInfo.left = rectGroupEntrustMain.left+iDropTitleLeft;
		rectStaticEntrustMainCommInfo.right = rectStaticEntrustMainCommInfo.left+iOneWordWidth*2;	// 2���ֵĿ��
		m_StaticEntrustMainCommInfo.MoveWindow(rectStaticEntrustMainCommInfo);
		CRect rectDropEntrustMainCommInfo(rectStaticEntrustMainCommInfo);
		rectDropEntrustMainCommInfo.top = rectStaticEntrustMainCommInfo.top-5;
		rectDropEntrustMainCommInfo.bottom = rectDropEntrustMainCommInfo.top+iDropHeight;
		rectDropEntrustMainCommInfo.left = rectStaticEntrustMainCommInfo.right+3;
		rectDropEntrustMainCommInfo.right = rectDropEntrustMainCommInfo.left+iDropWidth;
		m_DropEntrustMainCommInfo.MoveWindow(rectDropEntrustMainCommInfo);
		// ί�е�����(����)
		CRect rectStaticEntrustMainType(0,0,0,0);
		rectStaticEntrustMainType.left = rectDropEntrustMainCommInfo.right+iDropSpace;
		rectStaticEntrustMainType.right = rectStaticEntrustMainType.left+iOneWordWidth*5;
		rectStaticEntrustMainType.top = rectStaticEntrustMainCommInfo.top;
		rectStaticEntrustMainType.bottom = rectStaticEntrustMainCommInfo.bottom;
		m_StaticEntrustMainType.MoveWindow(rectStaticEntrustMainType);
		CRect rectDropEntrustMainType(0,0,0,0);
		rectDropEntrustMainType.left = rectStaticEntrustMainType.right+3;
		rectDropEntrustMainType.right = rectDropEntrustMainType.left+iDropWidth;
		rectDropEntrustMainType.top = rectDropEntrustMainCommInfo.top;
		rectDropEntrustMainType.bottom = rectDropEntrustMainCommInfo.bottom;
		m_DropEntrustMainType.MoveWindow(rectDropEntrustMainType);
		// ί�е�״̬(����)
		CRect rectStaticEntrustMainStatus(0,0,0,0);
		rectStaticEntrustMainStatus.left = rectDropEntrustMainType.right+iDropSpace;
		rectStaticEntrustMainStatus.right = rectStaticEntrustMainStatus.left+iOneWordWidth*5;
		rectStaticEntrustMainStatus.top = rectStaticEntrustMainCommInfo.top;
		rectStaticEntrustMainStatus.bottom = rectStaticEntrustMainCommInfo.bottom;
		m_StaticEntrustMainStatus.MoveWindow(rectStaticEntrustMainStatus);
		CRect rectDropEntrustMainStatus(0,0,0,0);
		rectDropEntrustMainStatus.left = rectStaticEntrustMainStatus.right+3;
		rectDropEntrustMainStatus.right = rectDropEntrustMainStatus.left+iDropWidth;
		rectDropEntrustMainStatus.top = rectDropEntrustMainCommInfo.top;
		rectDropEntrustMainStatus.bottom = rectDropEntrustMainCommInfo.bottom;
		m_DropEntrustMainStatus.MoveWindow(rectDropEntrustMainStatus);
		// ��ѯ����ť��
		CRect rectBtnEntrustMainQuery(0,0,0,0);
		rectBtnEntrustMainQuery.left = rectDropEntrustMainStatus.right+iDropSpace;
		rectBtnEntrustMainQuery.right = rectBtnEntrustMainQuery.left+iButtonWidth;
		rectBtnEntrustMainQuery.top = rectDropEntrustMainStatus.top;
		rectBtnEntrustMainQuery.bottom = rectBtnEntrustMainQuery.top+iButtonHeight;
		m_BtnEntrustMainQuery.MoveWindow(rectBtnEntrustMainQuery);
 		// ���ã���ť��
		CRect rectBtnEntrustMainReset(0,0,0,0);
		rectBtnEntrustMainReset.left = rectBtnEntrustMainQuery.right+iDropSpace;
		rectBtnEntrustMainReset.right = rectBtnEntrustMainReset.left+iButtonWidth;
		rectBtnEntrustMainReset.top = rectBtnEntrustMainQuery.top;
		rectBtnEntrustMainReset.bottom = rectBtnEntrustMainQuery.bottom;
		m_BtnEntrustMainReset.MoveWindow(rectBtnEntrustMainReset);
		// ��/����������
		CRect rectStaticEntrustMainBuySell(rectStaticEntrustMainCommInfo);
		rectStaticEntrustMainBuySell.right = rectStaticEntrustMainCommInfo.right;
		rectStaticEntrustMainBuySell.left = (int32)(rectStaticEntrustMainBuySell.right-2.5*iOneWordWidth);
		rectStaticEntrustMainBuySell.top = (int32)(rectStaticEntrustMainCommInfo.bottom+iDropSpace*1.5);
		rectStaticEntrustMainBuySell.bottom = rectStaticEntrustMainBuySell.top+iOneWordHeight;
		m_StaticEntrustMainBuySell.MoveWindow(rectStaticEntrustMainBuySell);
		CRect rectDropEntrustMainBuySell(rectStaticEntrustMainCommInfo);
		rectDropEntrustMainBuySell.top = rectStaticEntrustMainBuySell.top-5;
		rectDropEntrustMainBuySell.bottom = rectDropEntrustMainBuySell.top+iDropHeight;
		rectDropEntrustMainBuySell.left = rectStaticEntrustMainBuySell.right+3;
		rectDropEntrustMainBuySell.right = rectDropEntrustMainBuySell.left+iDropWidth;
		m_DropEntrustMainBuySell.MoveWindow(rectDropEntrustMainBuySell);
		// ����/ƽ��
		CRect rectStaticEntrustMainOpenClose(0,0,0,0);
		rectStaticEntrustMainOpenClose.right = rectStaticEntrustMainType.right;
		rectStaticEntrustMainOpenClose.left = rectStaticEntrustMainOpenClose.right-5*iOneWordWidth;
		rectStaticEntrustMainOpenClose.top = rectStaticEntrustMainBuySell.top;
		rectStaticEntrustMainOpenClose.bottom = rectStaticEntrustMainBuySell.bottom;
		m_StaticEntrustMainOpenClose.MoveWindow(rectStaticEntrustMainOpenClose);
		CRect rectDropEntrustMainOpenClose(rectStaticEntrustMainOpenClose);
		rectDropEntrustMainOpenClose.top = rectDropEntrustMainBuySell.top;
		rectDropEntrustMainOpenClose.bottom = rectDropEntrustMainBuySell.bottom;
		rectDropEntrustMainOpenClose.left = rectStaticEntrustMainOpenClose.right+3;
		rectDropEntrustMainOpenClose.right = rectDropEntrustMainOpenClose.left+iDropWidth;
		m_DropEntrustMainOpenClose.MoveWindow(rectDropEntrustMainOpenClose);
		// list
		CRect rectListEntrustMain(rc);
		rectListEntrustMain.left = rectGroupEntrustMain.left;
		rectListEntrustMain.right = rectGroupEntrustMain.right;
		rectListEntrustMain.top = rectDropEntrustMainOpenClose.bottom+iGroupSpace;	// list�����������
		rectListEntrustMain.bottom = rectGroupEntrustMain.bottom;
		CRect rcEntrustMainHBar(0,0,0,0);
		if ( rectListEntrustMain.Height() > iHorzHeight )
		{
			CRect rcHorz(rectListEntrustMain);
			rcHorz.top = rectListEntrustMain.bottom - iHorzHeight;
			rectListEntrustMain.bottom = rcHorz.top;
			rcEntrustMainHBar = rcHorz;
		}
		//��ֱ������
		CRect rcEntrustMainVBar(0,0,0,0);
		if ( (rectListEntrustMain.Height()<m_iAllEntrustMainHeight)&&m_iAllEntrustMainHeight>20 )
		{
			CRect rcVer(rectListEntrustMain);
			rcVer.left = rectListEntrustMain.right - iHorzHeight;
			rectListEntrustMain.right = rcVer.left;
			rcEntrustMainVBar = rcVer;
		}
		if (m_GridEntrustMain.m_hWnd)
		{
			m_GridEntrustMain.MoveWindow(rectListEntrustMain);
		}
		if (m_XBarEntrustMainH.m_hWnd)
		{
			m_XBarEntrustMainH.SetSBRect(rcEntrustMainHBar);
		}
		if (m_XBarEntrustMainV.m_hWnd)
		{
			m_XBarEntrustMainV.SetSBRect(rcEntrustMainVBar);
		}

	}
	else if (m_eCurQueryType == EQT_QueryHoldDetail)
	{
		/* �ֲ���ϸ */
		// list
		CRect rectListHoldDetailMain(rc);
		//ˮƽ������
		CRect rcHoldDetailMainHBar(0,0,0,0);
		if ( rectListHoldDetailMain.Height() > iHorzHeight )
		{
			CRect rcHorz(rectListHoldDetailMain);
			rcHorz.top = rectListHoldDetailMain.bottom - iHorzHeight;
			rectListHoldDetailMain.bottom = rcHorz.top;
			rcHoldDetailMainHBar = rcHorz;
		}
		//��ֱ������
		CRect rcHoldDetailMainVBar(0,0,0,0);
		if ( (rectListHoldDetailMain.Height()<m_iAllHoldDetailMainHeight)&&m_iAllHoldDetailMainHeight>20 )
		{
			CRect rcVer(rectListHoldDetailMain);
			rcVer.left = rectListHoldDetailMain.right - iHorzHeight;
			rectListHoldDetailMain.right = rcVer.left;
			rcHoldDetailMainVBar = rcVer;
		}
		if (m_GridHoldDetailMain.m_hWnd)
		{
			m_GridHoldDetailMain.MoveWindow(rectListHoldDetailMain);
		}
		if (m_XBarHoldDetailMainH.m_hWnd)
		{
			m_XBarHoldDetailMainH.SetSBRect(rcHoldDetailMainHBar);
		}
		if (m_XBarHoldDetailMainV.m_hWnd)
		{
			m_XBarHoldDetailMainV.SetSBRect(rcHoldDetailMainVBar);
		}
	}
	else if (m_eCurQueryType == EQT_QueryHoldSummary)
	{
		/* �ֲֻ��� */
		CRect rectListHoldSummaryMain(rc);
		//ˮƽ������
		CRect rcHoldSummaryMainHBar(0,0,0,0);
		if ( rectListHoldSummaryMain.Height() > iHorzHeight )
		{
			CRect rcHorz(rectListHoldSummaryMain);
			rcHorz.top = rectListHoldSummaryMain.bottom - iHorzHeight;
			rectListHoldSummaryMain.bottom = rcHorz.top;
			rcHoldSummaryMainHBar = rcHorz;
		}
		//��ֱ������
		CRect rcHoldDetailSummaryVBar(0,0,0,0);
		if ( (rectListHoldSummaryMain.Height()<m_iAllHoldSummaryMainHeight)&&m_iAllHoldSummaryMainHeight>20 )
		{
			CRect rcVer(rectListHoldSummaryMain);
			rcVer.left = rectListHoldSummaryMain.right - iHorzHeight;
			rectListHoldSummaryMain.right = rcVer.left;
			rcHoldDetailSummaryVBar = rcVer;
		}
		if (m_GridHoldSummary.m_hWnd)
		{
			m_GridHoldSummary.MoveWindow(rectListHoldSummaryMain);
		}
		if (m_XBarHoldTotalH.m_hWnd)
		{
			m_XBarHoldTotalH.SetSBRect(rcHoldSummaryMainHBar);
		}
		if (m_XBarHoldTotalV.m_hWnd)
		{
			m_XBarHoldTotalV.SetSBRect(rcHoldDetailSummaryVBar);
		}
	}
	else if (m_eCurQueryType == EQT_QueryLimitEntrust || m_eCurQueryType == EQT_QueryCancel)	//  ����(OK)
	{
		// list
		CRect rectListEntrust(rc);
		//ˮƽ������
		CRect rcEntrustHBar(0,0,0,0);
		if ( rectListEntrust.Height() > iHorzHeight )
		{
			CRect rcHorz(rectListEntrust);
			rcHorz.top = rectListEntrust.bottom - iHorzHeight;
			rectListEntrust.bottom = rcHorz.top;
			rcEntrustHBar = rcHorz;
		}	
		//��ֱ������
		CRect rcEntrustVBar(0,0,0,0);
		if ( (rectListEntrust.Height()<m_iAllEntrustHeight)&&m_iAllEntrustHeight>20 )
		{
			CRect rcVer(rectListEntrust);
			rcVer.left = rectListEntrust.right - iHorzHeight;
			rectListEntrust.right = rcVer.left;
			rcEntrustVBar = rcVer;
		}
		if (m_GridEntrust.m_hWnd)
		{
			if (m_eCurQueryType == EQT_QueryCancel)
			{
				rectListEntrust.top += 32;
			}
			m_GridEntrust.MoveWindow(rectListEntrust);
		}
		if (m_XBarEntrustH.m_hWnd)
		{
			m_XBarEntrustH.SetSBRect(rcEntrustHBar);
		}
		if (m_XBarEntrustV.m_hWnd)
		{
			m_XBarEntrustV.SetSBRect(rcEntrustVBar);
		}
	}
	else if (m_eCurQueryType == EQT_QueryQuote)
	{
		
	}
	else if (m_eCurQueryType == EQT_QueryCommInfo)	// ��Ʒ��Ϣ
	{
		// ��ѡ����Ʒ(����)
		CRect rectStaticCommInfoType(rc);
		rectStaticCommInfoType.top = rc.top + 2;
		rectStaticCommInfoType.bottom = rectStaticCommInfoType.top + iDropHeight -5;
		rectStaticCommInfoType.left = rc.left;
		rectStaticCommInfoType.right = rectStaticCommInfoType.left + iOneWordWidth * 5;	// 5���ֵĿ��
		m_StaticCommInfoType.MoveWindow(rectStaticCommInfoType);
		CRect rectDropCommInfoType(rectStaticCommInfoType);
		rectDropCommInfoType.top = rc.top;
		rectDropCommInfoType.bottom = rectDropCommInfoType.top + iDropHeight;
		rectDropCommInfoType.left = rectStaticCommInfoType.right;
		rectDropCommInfoType.right = rectDropCommInfoType.left + iDropWidth;
		m_DropCommInfoType.MoveWindow(rectDropCommInfoType);
		// list
		CRect rectListCommInfo(rc);
		rectListCommInfo.left = rc.left;
		rectListCommInfo.right = rc.right;
		rectListCommInfo.top = rectStaticCommInfoType.bottom + iGroupSpace;	// list�����������
		rectListCommInfo.bottom = rc.bottom;
		//��ֱ������
		CRect rcCommInfoVBar(0,0,0,0);
		if ( (rectListCommInfo.Height()<m_iAllCommInfoHeight)&&m_iAllCommInfoHeight>20 )
		{
			CRect rcVer(rectListCommInfo);
			rcVer.left = rectListCommInfo.right - iHorzHeight;
			rectListCommInfo.right = rcVer.left;
			rcCommInfoVBar = rcVer;
		}
		if (m_XBarCommInfoV.m_hWnd)
		{
			m_XBarCommInfoV.SetSBRect(rcCommInfoVBar);
		}

		if (m_GridCommInfo.m_hWnd)
		{
			m_GridCommInfo.MoveWindow(rectListCommInfo);
		}
	}
	else if (m_eCurQueryType == EQT_QueryUserInfo)	// �˻���Ϣ(ok)
	{
		// list
		CRect rectListAccInfoMain(rc);
		//��ֱ������
		CRect rcAccInfoVBar(0,0,0,0);
		if ( (rectListAccInfoMain.Height()<m_iAllAccInfoHeight)&&m_iAllAccInfoHeight>20 )
		{
			CRect rcVer(rectListAccInfoMain);
			rcVer.left = rectListAccInfoMain.right - iHorzHeight;
			rectListAccInfoMain.right = rcVer.left;
			rcAccInfoVBar = rcVer;
		}

		if (m_XBarAccInfoMainV.m_hWnd)
		{
			m_XBarAccInfoMainV.SetSBRect(rcAccInfoVBar);
		}

		if (m_GridAccInfoMain.m_hWnd)
		{
			m_GridAccInfoMain.MoveWindow(rectListAccInfoMain);
		}
	}
	else if (m_eCurQueryType == EQT_QueryDeal)
	{
		/* �ɽ���ѯ */
		// list
		CRect rectListDeal(rc);
		CRect rcDealHBar(0,0,0,0);
		if ( rectListDeal.Height() > iHorzHeight )
		{
			CRect rcHorz(rectListDeal);
			rcHorz.top = rectListDeal.bottom - iHorzHeight;
			rectListDeal.bottom = rcHorz.top;
			rcDealHBar = rcHorz;
		}
		//��ֱ������
		CRect rcDealVBar(0,0,0,0);
		if ( (rectListDeal.Height()<m_iAllDealHeight)&&m_iAllDealHeight>20 )
		{
			CRect rcVer(rectListDeal);
			rcVer.left = rectListDeal.right - iHorzHeight;
			rectListDeal.right = rcVer.left;
			rcDealVBar = rcVer;
		}
		if (m_GridDeal.m_hWnd)
		{
			m_GridDeal.MoveWindow(rectListDeal);
		}
		if (m_XBarDealH.m_hWnd)
		{
			m_XBarDealH.SetSBRect(rcDealHBar);
		}
		if (m_XBarDealV.m_hWnd)
		{
			m_XBarDealV.SetSBRect(rcDealVBar);
		}
	}

	if ( bNeedDraw )
	{
		Invalidate(TRUE);
	}
}


//�˻��ֲ�
void CDlgTradeQuery::SetCurSelToHoldDetail()
{
	ShowQueryChange(EQT_QueryHoldDetail);
}

void CDlgTradeQuery::SetTradeBid( iTradeBid *pTradeBid )
{
	if ( m_pTradeBid )
	{
		m_pTradeBid->RemoveTradeBidNotify(this);
	}

	m_pTradeBid = pTradeBid;
	if (NULL==m_pTradeBid)
	{
		//ASSERT(0);
		return;
	}
	m_pTradeBid->AddTradeBidNotify(this);		
}
// 
void CDlgTradeQuery::OnLoginStatusChanged( int iCurStatus, int iOldStatus )
{
	m_eTradeStatus = (E_TradeLoginStatus)iCurStatus;
	
	if (ETLS_Logined == m_eTradeStatus)
	{
		SetTimer(TIME_REFRESH_QUOTE, 1000, NULL);	// 1sˢ��1������
		SetTimer(TIME_REFRESH_HOLDE, 5000, NULL);   // �ֲֺ��˻���Ϣ5sˢ��1��
		m_bNetWorkStatus = TRUE;

		// ģ�⽻����ˢ��һ���û���Ϣ
		if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
		{
			QueryUserInfo();
		}
	}
	else if (ETLS_NotLogin == m_eTradeStatus)
	{
		KillTimer(TIME_REFRESH_QUOTE);
		KillTimer(TIME_REFRESH_HOLDE);
	}
}

bool32 CDlgTradeQuery::OnQueryUserInfo(CString &strTipMsg)
{
	bool32 bRet = false;
	bRet = QueryUserInfo();		// ���˻���Ϣ
	if (!bRet)
	{
		strTipMsg = _T("�����˻���Ϣ����");
		return bRet;
 	}
	return bRet;
}

bool32 CDlgTradeQuery::OnInitQuery(CString &strTipMsg)	// ��¼�ɹ���ʼ����ѯ
{
	bool32 bRet = false;

 	bRet = QueryCommInfo();	// ����Ʒ��Ϣ
 	if (!bRet)
 	{
 		strTipMsg = _T("������Ʒ��Ϣ����");
 		return bRet;
 	}

	bRet = QueryTraderID();	// ��Է�����ԱID
	if (!bRet)
	{
		strTipMsg = _T("��Է�����ԱID����");
		return bRet;
	}

	bRet = QueryHoldDetail();	// ��ֲ���ϸ
	if (!bRet)
	{
		strTipMsg = _T("���سֲ���ϸ����");
		return bRet;
	}

	bRet = QueryHoldSummary();
	if (!bRet)
	{
		strTipMsg = _T("���سֲֻ��ܳ���");
		return bRet;
	}

	bRet = QueryLimitEntrust();	// ��ָ��ί�е�
	if (!bRet)
	{
		strTipMsg = _T("����ָ��ί����Ϣ����");
		return bRet;
	}

	bRet = QueryDeal();	// ��ƽ�ֵ�
 	if (!bRet)
 	{
 		strTipMsg = _T("����ƽ�ֵ�����");
 		return bRet;
 	}

	//�б��ʼ��
//	ShowQueryChange(EQT_QueryHoldDetail);
//	PostMessage(WM_SIZE,0,0);

	return bRet;
}

void CDlgTradeQuery::OnInitCommInfo()	// ��ȡ����Ʒ�б�֮����������������
{
// 	if (m_bInitCommInfo)
// 	{
// 		return;
// 	}
// 
// 	// ��ʼ��ͷһ��
// 	T_CommInfoType type0 ;
// 	type0.iIndex = 0;
// 	strcpy(type0.chTypeName,"");
// 	strcpy(type0.chTypeValue,"");
// 	_CommInfoType::value_type value0( type0.iIndex, type0 );
// 	 m_CommInfoType.insert( value0 );
// 
// 	m_DropEntrustMainCommInfo.InsertString(0,L"��ʾȫ��");
// 	m_DropDealCommInfo.InsertString(0,L"��ʾȫ��");
// 
// 	const QueryCommInfoResultVector &aQuery = m_pTradeBid->GetCacheCommInfo();
// 	const int iSize = aQuery.size();
// 	for ( int i=0; i<aQuery.size(); ++i )
// 	{
// 		CommodityQueryOut stOut = aQuery[i];
// 		T_CommInfoType type;
// 		type.iIndex = i+1;
// 		strncpy(type.chTypeValue,stOut.commodityID,sizeof(type.chTypeValue)-1);
// 		strncpy(type.chTypeName,stOut.commodityName,sizeof(type.chTypeName)-1);
// 		_CommInfoType::value_type value( type.iIndex, type );
// 	 	m_CommInfoType.insert( value );
// 
// 		wstring wstr;
// 		Gbk32ToUnicode(stOut.commodityName,wstr);
// 		CString str;
// 		str.Format(_T("%s"),wstr.c_str());
// 		m_DropEntrustMainCommInfo.InsertString(i+1,str);
// 		m_DropDealCommInfo.InsertString(i+1,str);
// 		m_DropCommInfoType.InsertString(i,str);
// 	}
// 	m_DropEntrustMainCommInfo.SetCurSel(0);
// 	m_DropDealCommInfo.SetCurSel(0);
// 	m_DropCommInfoType.SetCurSel(0);
// 
// 	m_bInitCommInfo = true;
}

void CDlgTradeQuery::OnQueryHoldDetailResponse()
{
	ReloadGridHoldDetailMain();	// �ֲ���ϸ
}

void CDlgTradeQuery::OnQueryHoldSummaryResponse()
{
	ReloadGridHoldSummary();
}

// �ֲ���ϸ������
void CDlgTradeQuery::ReloadGridHoldDetailMain()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}

	CGridCtrlNormal *pGrid = &m_GridHoldDetailMain;	
	const QueryHoldDetailResultVector &aQuery = m_pTradeBid->GetCacheHoldDetail();
	
	//CCellID stCellTopLeft = pGrid->GetVisibleNonFixedCellRange().GetTopLeft();
	CCellRange cellRangeSel = pGrid->GetSelectedCellRange();
	
//	pGrid->DeleteNonFixedRows();
	int iNewSize = aQuery.size();
	int iOldSize = pGrid->GetRowCount() - pGrid->GetFixedRowCount();

	pGrid->SetRowCount(iNewSize+pGrid->GetFixedRowCount());
	if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
	{
		int index = 0;
		for ( int i=iNewSize-1; i>=0; --i )
		{
			const T_RespQueryHold &stOut = aQuery[i];
			int iRow = index + pGrid->GetFixedRowCount();
			UpdateOneHoldDetailMainRow(iRow, stOut);
			index++;
		}
	}
	else
	{
		for ( int i=0; i<iNewSize; ++i )
		{
			const T_RespQueryHold &stOut = aQuery[i];
			int iRow = i + pGrid->GetFixedRowCount();

			UpdateOneHoldDetailMainRow(iRow, stOut);
		}
	}
	
	if (iNewSize > iOldSize)
	{
		pGrid->SetScrollPos32(SB_VERT,0);
		pGrid->SetScrollPos32(SB_HORZ,0);
		
		//��һ�β�Ѱ��������ѡ��
		if (iOldSize > 0)
		{
			pGrid->SetSelectedSingleRow(1);
		}
	}
	else
	{
	//	pGrid->EnsureTopLeftCell(stCellTopLeft);
		pGrid->SetSelectedRange(cellRangeSel);
	}
	
	pGrid->Refresh();

	
	//�ֲ���ϸ�ܼ�¼�߶�
	m_iAllHoldDetailMainHeight = pGrid->GetRowCount() * pGrid->GetRowHeight(0);
}

// �ֲ���ϸһ��������ʾ
void CDlgTradeQuery::UpdateOneHoldDetailMainRow( int iRow, const T_RespQueryHold &stOut)
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}

	double dBuyPrice = 0;	//���
	double dSellPrice = 0;	//����
	double dClosePrice = 0;	//ƽ�ּ�
	CString dTradeUnit = "";//���׵�λ
	double dMarginRate =0; //��֤��ϵ��

	string strCode;
	UnicodeToUtf8(stOut.stock_code, strCode);
	GetBuySellPrice(strCode.c_str(), dBuyPrice, dSellPrice, dTradeUnit,dMarginRate);//��ȡƽ�ּ۸�
	
	CGridCtrlNormal *pGrid = &m_GridHoldDetailMain;
	const T_TradeGridHeadGroup &stGridHeadGroup = s_kstTradeGridHeadGroupHoldDetailMain;

	COLORREF clrRise, clrFall, clrKeep;
	GetRiseFallColor(clrRise, clrFall, clrKeep);

	pGrid->SetRowHeight(iRow, GRID_ITEM_HEIGHT);
	
	// ��ͷ
	const T_TradeGridHead *pstHeader = stGridHeadGroup.pstGridHead;
	const int iHeaderCount = stGridHeadGroup.iGridHeadCount;
	if ( pstHeader!=NULL && iHeaderCount>0 )
	{
		for ( int iCol=0; iCol<iHeaderCount; ++iCol )
		{
			const T_TradeGridHead &stHeader = pstHeader[iCol];
			CGridCellBase *pCell = pGrid->GetCell(iRow, iCol);
			if ( !pCell )
			{
				continue;
			}

			// ����ˮƽ�ָ���
			pCell->SetHSeparatoLine(true);
			
			if ( pCell->IsFixedCol() )
			{
				pCell->SetTextClr(stGridHeadGroup.clrFixedRow);
			}
			
			pCell->SetFormat(stHeader.nFmt);
			
			switch ( stHeader.iHeadType )
			{
			case ETGHT_HOLDDETAIL_HoldNumber:
				{
					CString Str = stOut.hold_id;
					pCell->SetText(Str);
				}
				break;
			case ETGHT_HOLDDETAIL_CommodityID:
				{
					string strStkCode;
					UnicodeToUtf8(stOut.stock_code, strStkCode);
					CString str = GetCommName(strStkCode.c_str());//stOut.commodityID;
					pCell->SetText(str);
				}
				break;
			case ETGHT_HOLDDETAIL_BuySell:
				{
					CString str = GetBuySellShow(stOut.entrust_bs);
					if (_T("B") == stOut.entrust_bs)
					{
						dClosePrice = dSellPrice;
						pCell->SetTextClr(RGB(255,0,0));
					}
					else
					{
						dClosePrice = dBuyPrice;
						pCell->SetTextClr(RGB(0,129,0));
					}
					pCell->SetText(str);
				}
				break;
// 			case ETGHT_HOLDDETAIL_OpenQty:
// 				{
// 					CString str = _T("");
// 					str.Format(_T("%ld"),stOut.hold_amount);
// 					pCell->SetText(str);
// 				}
// 				break;
			case ETGHT_HOLDDETAIL_HoldQty:
				{
					CString str = _T("");
					str.Format(_T("%ld"),stOut.hold_amount);
					pCell->SetText(str);
				}
				break;
			case ETGHT_HOLDDETAIL_OpenPrice:
				{
					CString str = _T("-");
					if(stOut.avg<0.000001 && stOut.avg>-0.000001)
					{
						str.Format(_T("%.2f"),stOut.open_price);
					}
					else
					{
						str.Format(_T("%.2f"),stOut.avg);
					}
					pCell->SetText(str);
				}
				break;
			case ETGHT_HOLDDETAIL_HoldPrice:
				{
					CString str = _T("-");
					if(stOut.hold_avg<0.000001 && stOut.hold_avg>-0.000001)
					{
						str.Format(_T("%.2f"),stOut.hold_price);
					}
					else
					{
						str.Format(_T("%.2f"),stOut.hold_avg);
					}
					pCell->SetText(str);
				}
				break;
			case ETGHT_HOLDDETAIL_ClosePrice:
				{
					CString str = _T("-");
					str.Format(_T("%.2f"),dClosePrice);
					pCell->SetText(str);
				}
				break;
			case ETGHT_HOLDDETAIL_StopLoss:
				{
					CString str = _T("-");
					str.Format(_T("%.2f"),stOut.stop_loss);
					pCell->SetText(str);
				}
				break;
			case ETGHT_HOLDDETAIL_StopProfit:
				{
					CString str = _T("-");
					str.Format(_T("%.2f"),stOut.stop_profit);
					pCell->SetText(str);
				}
				break;
			case ETGHT_HOLDDETAIL_FloatPL:
				{
// 					CString str = _T("-");
// 					str.Format(_T("%.2f"), stOut.floating_profit);
// 					pCell->SetText(str);
// 					if (stOut.floating_profit>0)
// 					{
// 						pCell->SetTextClr(TEXT_COLOR_RED);
// 					}
// 					else if (stOut.floating_profit<0)
// 					{
// 						pCell->SetTextClr(TEXT_COLOR_GRE);
// 					}
					int buySell=1; // ��������
					double dFloatPL = 0.0;
					
					if (_T("B") == stOut.entrust_bs)
					{
						buySell = 1;
					}
					else
					{
						buySell = -1;
					}
					
					string strFloat;
					UnicodeToUtf8(dTradeUnit, strFloat);
					double dUint = atof(strFloat.c_str());

					dClosePrice = reversef_(dClosePrice, 2);
					if (dClosePrice<0.000001 && dClosePrice>-0.000001)
					{
						dFloatPL = 0.;
					}
					else
					{
						if(stOut.hold_avg<0.000001 && stOut.hold_avg>-0.000001)
						{
							dFloatPL = (dClosePrice - stOut.hold_price)*buySell*dUint*stOut.hold_amount;
						}
						else
						{
							dFloatPL = (dClosePrice - stOut.hold_avg)*buySell*dUint*stOut.hold_amount;
						}
					}

					CString str = _T("-");
 					str.Format(_T("%.2f"), dFloatPL);
 					pCell->SetText(str);
					if (dFloatPL>0)
					{
						pCell->SetTextClr(TEXT_COLOR_RED);
					}
					else if (dFloatPL<0)
					{
						pCell->SetTextClr(TEXT_COLOR_GRE);
					}
					else
					{
						pCell->SetTextClr(GRID_TEXT_COLOR);
					}
				}
				break;
			case ETGHT_HOLDDETAIL_MarginRate:
				{
					CString str = _T("-");
					str.Format(_T("%.2f"), dMarginRate);
					pCell->SetText(str);
				}
				break;
			case ETGHT_HOLDDETAIL_Margin:
				{
					CString str = _T("-");
					str.Format(_T("%.2f"),stOut.margin);
					pCell->SetText(str);
				}
				break;
			case ETGHT_HOLDDETAIL_OpenTime:
				{
					pCell->SetText(stOut.entrust_time);
				}
				break;
 			default:
				break;
			}
		}
	}
}

void CDlgTradeQuery::ReloadGridHoldSummary()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}

	CGridCtrlNormal *pGrid = &m_GridHoldSummary;
	const QueryHoldSummaryResultVector &aQuery = m_pTradeBid->GetCacheHoldSummary();
	/*CCellID stCellTopLeft = */pGrid->GetVisibleNonFixedCellRange().GetTopLeft();
	CCellRange cellRangeSel = pGrid->GetSelectedCellRange();
	int iNewSize = aQuery.size();
	int iOldSize = pGrid->GetRowCount() - pGrid->GetFixedRowCount();
	pGrid->SetRowCount(aQuery.size()+pGrid->GetFixedRowCount());
	for ( int i=0; i<aQuery.size(); ++i )
	{
		const T_RespQueryHoldTotal &stOut = aQuery[i];
		int iRow = i + pGrid->GetFixedRowCount();
		UpdateOneHoldSummaryRow(iRow, stOut);
	}

	if (iNewSize > iOldSize)
	{
		pGrid->SetScrollPos32(SB_VERT,0);
		pGrid->SetScrollPos32(SB_HORZ,0);

		//��һ�β�Ѱ��������ѡ��
		if (iOldSize > 0)
		{
			pGrid->SetSelectedSingleRow(1);
		}
	}
	else
	{
	//	pGrid->EnsureTopLeftCell(stCellTopLeft);
		pGrid->SetSelectedRange(cellRangeSel);
	}
	pGrid->Refresh();

	//�ֲ���ϸ�ܼ�¼�߶�
	m_iAllHoldSummaryMainHeight = pGrid->GetRowCount() * pGrid->GetRowHeight(0);
}

void CDlgTradeQuery::UpdateOneHoldSummaryRow( int iRow, const T_RespQueryHoldTotal &stOut)
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}

	CString str;
	double dBuyPrice = 0;	//���
	double dSellPrice = 0;	//����
	double dClosePrice = 0;	//ƽ�ּ�
	CString dTradeUnit = "";//���׵�λ
	double dFloatPL = 0;	//����ӯ��
	double dMarginRate =0; //��֤��ϵ��

	string strCode;
	UnicodeToUtf8(stOut.stock_code, strCode);
	GetBuySellPrice(strCode.c_str(), dBuyPrice, dSellPrice, dTradeUnit,dMarginRate);//��ȡƽ�ּ۸�


	CGridCtrlNormal *pGrid = &m_GridHoldSummary;
	const T_TradeGridHeadGroup &stGridHeadGroup = s_kstTradeGridHeadGroupHoldSummary;

	COLORREF clrRise, clrFall, clrKeep;
	GetRiseFallColor(clrRise, clrFall, clrKeep);
	pGrid->SetRowHeight(iRow, GRID_ITEM_HEIGHT);

	const T_TradeGridHead *pstHeader = stGridHeadGroup.pstGridHead;
	const int iHeaderCount = stGridHeadGroup.iGridHeadCount;
	if (pstHeader && iHeaderCount > 0)
	{
		for (int nCol = 0 ; nCol < iHeaderCount ; ++nCol)
		{
			const T_TradeGridHead &stHeader = pstHeader[nCol];
			CGridCellBase *pCell = pGrid->GetCell(iRow, nCol);
			if ( !pCell )
				continue;
			// ����ˮƽ�ָ���
			pCell->SetHSeparatoLine(true);
			if ( pCell->IsFixedCol() )
			{
				pCell->SetTextClr(stGridHeadGroup.clrFixedRow);
			}
			pCell->SetFormat(stHeader.nFmt);

			switch (stHeader.iHeadType)
			{
			case ETGHT_HOLDSUMMARY_GoodsName:
				{
					string strStockCode;
					UnicodeToUtf8(stOut.stock_code, strStockCode);
					CString Str = GetCommName(strStockCode.c_str());
					pCell->SetText(Str);
				}
				break;
			case ETGHT_HOLDSUMMARY_BuySell:
				{
					if (_T("B") == stOut.entrust_bs)
					{
						pCell->SetText(_T("����"));
						pCell->SetTextClr(RGB(255,0,0));
					}
					else if (_T("S") == stOut.entrust_bs)
					{
						pCell->SetText(_T("����"));
						pCell->SetTextClr(RGB(0,129,0));
					}
				}
				break;
			case ETGHT_HOLDSUMMARY_AvgHoldPrice:
				{
					str.Format(_T("%.2f"),stOut.hold_avg);
					pCell->SetText(str);
				}
				break;
			case ETGHT_HOLDSUMMARY_HoldCount:
				{
					str.Format(_T("%d"),stOut.hold_amount);
					pCell->SetText(str);
				}
				break;
			case ETGHT_HOLDSUMMARY_FloatProfitAndLoss:
				{
					int buySell=1; // ��������
					if (_T("B") == stOut.entrust_bs)
					{
						buySell = 1;
						dClosePrice = dSellPrice;
					}
					else
					{
						buySell = -1;
						dClosePrice = dBuyPrice;
					}
					string strFloat;
					UnicodeToUtf8(dTradeUnit, strFloat);
					double dUint = atof(strFloat.c_str());

					if (dClosePrice<0.000001 && dClosePrice>-0.000001)
					{
						dFloatPL = 0.;
					}
					else
					{
						double  dHoldFloat = (int)(stOut.hold_avg*buySell*dUint*stOut.hold_amount);
						double  dCurrFloat = reversef_(dClosePrice,2)*buySell*dUint*stOut.hold_amount;
						dFloatPL = dCurrFloat - dHoldFloat;
					}

					CString Str;
					Str.Format(_T("%.2f"), dFloatPL);
					pCell->SetText(Str);
					if (dFloatPL>0)
					{
						pCell->SetTextClr(TEXT_COLOR_RED);
					}
					else if (dFloatPL<0)
					{
						pCell->SetTextClr(TEXT_COLOR_GRE);
					}
					else
					{
						pCell->SetTextClr(GRID_TEXT_COLOR);
					}
				}
				break;
			case ETGHT_HOLDSUMMARY_MARGIN:
				{
					str.Format(_T("%.2f"),stOut.margin);
					pCell->SetText(str);
				}
				break;
			default:
				break;
			}
		}
	}
}

void CDlgTradeQuery::OnQueryLimitEntrustResponse()
{
	ReloadGridEntrust();
}

void CDlgTradeQuery::ReloadGridEntrust()
{
	if ( m_pTradeBid == NULL )
	{
		ASSERT( 0 );
		return;
	}
	
	CGridCtrlNormal *pGrid = &m_GridEntrust;	
	const QueryEntrustResultVector &aQuery= m_pTradeBid->GetCacheEntrust();
	
	/*CCellID stCellTopLeft = */pGrid->GetVisibleNonFixedCellRange().GetTopLeft();
	CCellRange cellRangeSel = pGrid->GetSelectedCellRange();

	int iSize = 0;
	iSize = aQuery.size();
	pGrid->SetRowCount(iSize+pGrid->GetFixedRowCount());

	if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
	{
		int index = 0;
		for ( int i=iSize-1; i>=0; --i )
		{
			const T_RespQueryEntrust &stOut = aQuery[i];
			int iRow = index + pGrid->GetFixedRowCount();

			UpdateOneEntrustRow(iRow, stOut);
			index++;
		}
	}
	else
	{
		for ( int i=0; i<iSize; ++i )
		{
			const T_RespQueryEntrust &stOut = aQuery[i];
			int iRow = i + pGrid->GetFixedRowCount();
			UpdateOneEntrustRow(iRow, stOut);
		}
	}
	
//	pGrid->EnsureTopLeftCell(stCellTopLeft);
	pGrid->SetSelectedRange(cellRangeSel);

	pGrid->Refresh();	
	
//	pGrid->
	//ί���ܼ�¼�߶�
	m_iAllEntrustHeight = pGrid->GetRowCount() * pGrid->GetRowHeight(0);
}

void CDlgTradeQuery::UpdateOneEntrustRow( int iRow, const T_RespQueryEntrust &stOut)
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}

	CGridCtrlNormal *pGrid = &m_GridEntrust;
	const T_TradeGridHeadGroup &stGridHeadGroup = s_kstTradeGridHeadGroupEntrust;


	COLORREF clrRise, clrFall, clrKeep;
	GetRiseFallColor(clrRise, clrFall, clrKeep);

	pGrid->SetRowHeight(iRow, GRID_ITEM_HEIGHT);
	
	// ��ͷ
	const T_TradeGridHead *pstHeader = stGridHeadGroup.pstGridHead;
	const int iHeaderCount = stGridHeadGroup.iGridHeadCount;
	if ( pstHeader!=NULL && iHeaderCount>0 )
	{
		for ( int iCol=0; iCol<iHeaderCount; ++iCol )
		{
			const T_TradeGridHead &stHeader = pstHeader[iCol];
			CGridCellBase *pCell = pGrid->GetCell(iRow, iCol);
			if ( !pCell )
			{
				continue;
			}

			pCell->SetHSeparatoLine(true);
			
			if ( pCell->IsFixedCol() )
			{
				pCell->SetTextClr(stGridHeadGroup.clrFixedRow);
			}
			
			pCell->SetFormat(stHeader.nFmt);
			
			switch ( stHeader.iHeadType )
			{
			case ETGHT_ENTRUST_Operator:
				{
					pCell->SetTextClr(RGB(34,124,236));
					pCell->SetText(_T("����"));
					pCell->SetUnderLine(TRUE);
				}
				break;
			case ETGHT_ENTRUST_OrderNumber:
				{
					CString str = stOut.entrust_no;
					pCell->SetText(str);
				}
				break;
// 			case ETGHT_ENTRUST_Time:
// 				{
// 					char chTime[32] = {0};
// 					char ch[32]={0};
// 
// 					itoa(stOut.entrust_time, ch, 10);
// 					strncpy(chTime, ch, 10);	// ȡ��ǰ10λ����
// 					CString str = UtcTOLocalTime(atol(chTime), chTime);
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_State:
// 				{
// 					CString str = GetEntrustStatusShow(stOut.entrust_status);
// 					pCell->SetText(str);
// 				}
// 				break;
			case ETGHT_ENTRUST_BuySell:
				{
					CString str = GetBuySellShow(stOut.entrust_bs);
					pCell->SetText(str);
				}
				break;
// 			case ETGHT_ENTRUST_OpenClose:
// 				{
// 					CString str = GetOpenCloseShow(stOut.openClose);
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_OrderType:
// 				{
// 					CString str = GetOpenCloseShow(stOut.eo_flag);
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_TraderID:
// 				{
// 					CString str = stOut.traderID;
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_DealerID:
// 				{
// 					CString str = stOut.dealerID;
// 					pCell->SetText(str);
// 				}
// 				break;
			case ETGHT_ENTRUST_CommodityID:
				{
					string strCode;
					UnicodeToUtf8(stOut.stock_code, strCode);
					CString str = GetCommName(strCode.c_str());//stOut.commodityID;
					pCell->SetText(str);
				}
				break;
			case ETGHT_ENTRUST_Price:
				{
					CString str = _T("-");
					str.Format(_T("%.2f"),stOut.entrust_price);
					pCell->SetText(str);
				}
				break;
			case ETGHT_ENTRUST_FreezeMargin:
				{
					CString str = _T("-");
					str.Format(_T("%.2f"),stOut.froze_margin);
					pCell->SetText(str);
				}
				break;
			case ETGHT_ENTRUST_FreezeFee:
				{
					CString str = _T("-");
					str.Format(_T("%.2f"),stOut.froze_fee);
					pCell->SetText(str);
				}
				break;
			case ETGHT_ENTRUST_Quantity:
				{
					CString str = _T("");
					str.Format(_T("%ld"),stOut.entrust_amount);
					pCell->SetText(str);
				}
				break;
			case ETGHT_ENTRUST_RevokeTime:
				{
					CString str = _T("������Ч")/*stOut.revokeTime*/;
					pCell->SetText(str);
				}
				break;
			case ETGHT_ENTRUST_StopLoss:
				{
					CString str = _T("-");
					str.Format(_T("%.2f"),stOut.stop_loss);
					pCell->SetText(str);
				}
				break;
			case ETGHT_ENTRUST_StopProfit:
				{
					CString str = _T("-");
					str.Format(_T("%.2f"),stOut.stop_profit);
					pCell->SetText(str);
				}
				break;
// 			case ETGHT_ENTRUST_OrderFirmID:
// 				{
// 					CString str = stOut.orderFirmID;
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_AgentID: // ��Ϊί��Ա����
// 				{
// 					CString str = stOut.agent_id;
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_HoldingNumber:
// 				{
// 					CString str = stOut.holdingNumber;
// 					pCell->SetText(str);
// 				}
// 				break;
 			default:
				ASSERT( 0 );
				break;
			}
		}
	}	
}

void CDlgTradeQuery::ReloadGridEntrustMain()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}
	
	

	CGridCtrlNormal *pGrid = &m_GridEntrustMain;	
	const QueryEntrustResultVector &aQuery = m_pTradeBid->GetCacheEntrust();
	
	/*CCellID stCellTopLeft = */pGrid->GetVisibleNonFixedCellRange().GetTopLeft();
	CCellRange cellRangeSel = pGrid->GetSelectedCellRange();
	
//	pGrid->DeleteNonFixedRows();
	pGrid->SetRowCount(aQuery.size()+pGrid->GetFixedRowCount());
	for ( int i=0; i<aQuery.size(); ++i )
	{
		const T_RespQueryEntrust &stOut = aQuery[i];
		int iRow = i + pGrid->GetFixedRowCount();
		
		UpdateOneEntrustMainRow(iRow, stOut);
	}
	
//	pGrid->EnsureTopLeftCell(stCellTopLeft);
	pGrid->SetSelectedRange(cellRangeSel);
	pGrid->Refresh();
	
	//ί���ܼ�¼�߶�
	m_iAllEntrustMainHeight = pGrid->GetRowCount() * pGrid->GetRowHeight(0);
}

void CDlgTradeQuery::UpdateOneEntrustMainRow( int iRow, const T_RespQueryEntrust &stOut)
{
// 	if ( m_pTradeBid==NULL )
// 	{
// 		ASSERT( 0 );
// 		return;
// 	}
// 
// 	CGridCtrlNormal *pGrid = &m_GridEntrustMain;
// 	const T_TradeGridHeadGroup &stGridHeadGroup = s_kstTradeGridHeadGroupEntrustMain;
// 
// 
// 	COLORREF clrRise, clrFall, clrKeep;
// 	GetRiseFallColor(clrRise, clrFall, clrKeep);
// 	
// 	// ��ͷ
// 	const T_TradeGridHead *pstHeader = stGridHeadGroup.pstGridHead;
// 	const int iHeaderCount = stGridHeadGroup.iGridHeadCount;
// 	if ( pstHeader!=NULL && iHeaderCount>0 )
// 	{
// 		for ( int iCol=0; iCol<iHeaderCount; ++iCol )
// 		{
// 			const T_TradeGridHead &stHeader = pstHeader[iCol];
// 			CGridCellBase *pCell = pGrid->GetCell(iRow, iCol);
// 			if ( !pCell )
// 			{
// 				continue;
// 			}
// 			
// 			if ( pCell->IsFixedCol() )
// 			{
// 				pCell->SetTextClr(stGridHeadGroup.clrFixedRow);
// 			}
// 			
// 			pCell->SetFormat(stHeader.nFmt);
// 			
// 			switch ( stHeader.iHeadType )
// 			{
// 			case ETGHT_ENTRUST_OrderNumber:
// 				{
// 					CString str = stOut.orderNumber;
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_Time:
// 				{
// 					char chTime[32] = {0};
// 					strncpy(chTime,stOut.time,10);	// ȡ��ǰ10λ����
// 					CString str = UtcTOLocalTime(atol(chTime),chTime);
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_State:
// 				{
// 					CString str = GetEntrustStatusShow(stOut.state);
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_BuySell:
// 				{
// 					CString str = GetBuySellShow(stOut.buySell);
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_OpenClose:
// 				{
// 					CString str = GetOpenCloseShow(stOut.openClose);
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_OrderType:
// 				{
// 					CString str = GetEntrustTypeShow(stOut.orderType);
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_TraderID:
// 				{
// 					CString str = stOut.traderID;
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_DealerID:
// 				{
// 					CString str = stOut.dealerID;
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_CommodityID:
// 				{
// 					CString str = GetCommName(stOut.commodityID);//stOut.commodityID;
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_Price:
// 				{
// 					CString str = _T("-");
// 					str.Format(_T("%.2f"),stOut.price);
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_FreezeMargin:
// 				{
// 					CString str = _T("-");
// 					str.Format(_T("%.2f"),stOut.freezeMargin);
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_FreezeFee:
// 				{
// 					CString str = _T("-");
// 					str.Format(_T("%.2f"),stOut.freezeFee);
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_Quantity:
// 				{
// 					CString str = _T("");
// 					str.Format(_T("%ld"),stOut.quantity);
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_RevokeTime:
// 				{
// 					CString str = _T("������Ч")/*stOut.revokeTime*/;
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_StopLoss:
// 				{
// 					CString str = _T("-");
// 					str.Format(_T("%.2f"),stOut.stopLoss);
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_StopProfit:
// 				{
// 					CString str = _T("-");
// 					str.Format(_T("%.2f"),stOut.stopProfit);
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_OrderFirmID:
// 				{
// 					CString str = stOut.orderFirmID;
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_AgentID:
// 				{
// 					CString str = stOut.agentID;
// 					pCell->SetText(str);
// 				}
// 				break;
// 			case ETGHT_ENTRUST_HoldingNumber:
// 				{
// 					CString str = stOut.holdingNumber;
// 					pCell->SetText(str);
// 				}
// 				break;
// 			default:
// 				ASSERT( 0 );
// 				break;
// 			}
// 		}
// 	}
}

// ��ɽ���Ϣ
void CDlgTradeQuery::OnQueryDealResponse()
{
	ReloadGridDeal();
}

void CDlgTradeQuery::ReloadGridDeal()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}
	

	CGridCtrlNormal *pGrid = &m_GridDeal;	
	const QueryDealResultVector &aQuery = m_pTradeBid->GetCacheDeal();
	
	/*CCellID stCellTopLeft = */pGrid->GetVisibleNonFixedCellRange().GetTopLeft();
	CCellRange cellRangeSel = pGrid->GetSelectedCellRange();
	
	int iSize = 0;
	iSize = aQuery.size();
	pGrid->SetRowCount(iSize + pGrid->GetFixedRowCount());

	if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
	{
		int index = 0;
		for ( int i=iSize-1; i>=0; --i )
		{
			const T_RespQueryDeal &stOut = aQuery[i];
			int iRow = index + pGrid->GetFixedRowCount();
			UpdateOneDealRow(iRow, stOut);
			index++;
		}
	}
	else
	{
		for ( int i=0; i<iSize; ++i )
		{
			const T_RespQueryDeal &stOut = aQuery[i];
			int iRow = i + pGrid->GetFixedRowCount();
			UpdateOneDealRow(iRow, stOut);
		}
	}
	
//	pGrid->EnsureTopLeftCell(stCellTopLeft);
	pGrid->SetSelectedRange(cellRangeSel);
	pGrid->Refresh();
	
	//�ɽ��ܼ�¼�߶�
	m_iAllDealHeight = pGrid->GetRowCount() * pGrid->GetRowHeight(0);
}

void CDlgTradeQuery::UpdateOneDealRow( int iRow, const T_RespQueryDeal &stOut)
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}

	CGridCtrlNormal *pGrid = &m_GridDeal;
	const T_TradeGridHeadGroup &stGridHeadGroup = s_kstTradeGridHeadGroupDeal;


	COLORREF clrRise, clrFall, clrKeep;
	GetRiseFallColor(clrRise, clrFall, clrKeep);

	pGrid->SetRowHeight(iRow, GRID_ITEM_HEIGHT);
	
	// ��ͷ
	const T_TradeGridHead *pstHeader = stGridHeadGroup.pstGridHead;
	const int iHeaderCount = stGridHeadGroup.iGridHeadCount;
	if ( pstHeader!=NULL && iHeaderCount>0 )
	{
		for ( int iCol=0; iCol<iHeaderCount; ++iCol )
		{
			const T_TradeGridHead &stHeader = pstHeader[iCol];
			CGridCellBase *pCell = pGrid->GetCell(iRow, iCol);
			if ( !pCell )
			{
				continue;
			}

			pCell->SetHSeparatoLine(true);
			
			if ( pCell->IsFixedCol() )
			{
				pCell->SetTextClr(stGridHeadGroup.clrFixedRow);
			}
			
			pCell->SetFormat(stHeader.nFmt);
			
			switch ( stHeader.iHeadType )
			{
			case ETGHT_DEAL_TradeNumber:
				{
					CString Str = stOut.deal_no;
					pCell->SetText(Str);
				}
				break;
			case ETGHT_DEAL_CommodityID:
				{
					string strCode;
					UnicodeToUtf8(stOut.stock_code, strCode);
					CString Str = GetCommName(strCode.c_str());//stOut.commodityID;
					pCell->SetText(Str);
				}
				break;
			case ETGHT_DEAL_BuySell:
				{
					CString Str = GetBuySellShow(stOut.entrust_bs);
					pCell->SetText(Str);
				}
				break;
			case ETGHT_DEAL_Qty:
				{
					CString Str = _T("");
					Str.Format(_T("%ld"),stOut.business_amount);
					pCell->SetText(Str);
				}
				break;
			case ETGHT_DEAL_TradePrice:
				{
					CString Str = _T("-");

					// ƽ�ּ۸�Ϊ0 ����ȡ�ɽ��۸�
					if (stOut.close_price>-0.000001 && stOut.close_price<0.000001)
					{
						Str.Format(_T("%.2f"),stOut.business_price);
					}
					else
					{
						Str.Format(_T("%.2f"),stOut.close_price);
					}
					pCell->SetText(Str);
				}
				break;
			case ETGHT_DEAL_CloseProfit:
				{
					CString Str = _T("-");
					Str.Format(_T("%.2f"),stOut.close_profitloss);
					pCell->SetText(Str);
				}
				break;
			case ETGHT_DEAL_Fee:
				{
					//CString Str = _T("");//stOut.fee;
					CString Str = _T("-");
					Str.Format(_T("%.2f"),-stOut.charge);
					pCell->SetText(Str);
				}
				break;
			case ETGHT_DEAL_OrderNumber:
				{
					CString Str = stOut.entrust_no;
					pCell->SetText(Str);
				}
				break;
// 			case ETGHT_DEAL_HoldNumber:
// 				{
// 					CString Str = stOut.hold_id;
// 					pCell->SetText(Str);
// 				}
// 				break;
// 			case ETGHT_DEAL_OpenClose:
// 				{
// 					CString Str = GetOpenCloseShow(stOut.eo_flag);
// 					pCell->SetText(Str);
// 				}
// 				break;
// 			case ETGHT_DEAL_OtherID:
// 				{
// 					CString Str = stOut.otherID;
// 					pCell->SetText(Str);
// 				}
// 				break;
// 			case ETGHT_DEAL_TradeType:
// 				{
// 					CString Str = GetDealTypeShow(stOut.trade_type);
// 					pCell->SetText(Str);
// 				}
// 				break;
// 			case ETGHT_DEAL_OperateType:
// 				{
// 					CString Str = GetDealOperateTypeShow(stOut.tradeType);
// 					pCell->SetText(Str);
// 				}
// 				break;
			case ETGHT_DEAL_TradeTime:
				{
					CString Str = stOut.business_time;
					pCell->SetText(Str);
				}
				break;
// 			case ETGHT_DEAL_TraderID:
// 				{
// 					CString Str = stOut.traderID;
// 					pCell->SetText(Str);
// 				}
// 				break;
// 			case ETGHT_DEAL_DealerID:
// 				{
// 					CString Str = stOut.dealerID;
// 					pCell->SetText(Str);
// 				}
// 				break;
// 			case ETGHT_DEAL_OpenPrice:
// 				{
// 					CString Str = _T("-");
// 					Str.Format(_T("%.2f"),stOut.openPrice);
// 					pCell->SetText(Str);
// 				}
// 				break;
// 			case ETGHT_DEAL_Margin:
// 				{
// 					CString Str = _T("-");
// 					Str.Format(_T("%.2f"),stOut.margin);
// 					pCell->SetText(Str);
// 				}
// 				break;
// 			case ETGHT_DEAL_HoldPrice:
// 				{
// 					CString Str = _T("-");
// 					Str.Format(_T("%.2f"),stOut.holdPrice);
// 					pCell->SetText(Str);
// 				}
// 				break;
// 			case ETGHT_DEAL_OpenTime:
// 				{
// 					CString Str = stOut.openTime;
// 					pCell->SetText(Str);
// 				}
// 				break;
 			default:
				ASSERT( 0 );
				break;
			}
		}
	}
}

// ����Ʒ��Ϣ
void CDlgTradeQuery::OnQueryCommInfoResponse()
{
	ReloadGridCommInfo();
}

void CDlgTradeQuery::OnQueryTraderIDResponse()
{

}

void CDlgTradeQuery::ReloadGridCommInfo()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}
	

	InitCommIfo();

	CGridCtrlNormal *pGrid = &m_GridCommInfo;	
	const QueryCommInfoResultVector &aQuery = m_pTradeBid->GetCacheCommInfo();
	m_CommInfoResultVector = aQuery;
	
	/*CCellID stCellTopLeft = */pGrid->GetVisibleNonFixedCellRange().GetTopLeft();
	CCellRange cellRangeSel = pGrid->GetSelectedCellRange();

	T_TradeMerchInfo stOut/* = aQuery[0]*/;
	T_CommInfoType type;
	FindCommInfoType(m_DropCommInfoType.GetCurSel()+1,type);
	bool bFind = false;
	for ( int i=0; i<aQuery.size(); ++i ) // ֻ��һ��(��ú͵�ǰѡ�еıȽ���)
	{
		stOut = aQuery[i];
		string strCode;
		UnicodeToUtf8(stOut.stock_code, strCode);
		if (_stricmp(strCode.c_str(), type.chTypeValue)==0)	//�Ƚ���Ʒ����
		{
			bFind = true;
			break;
		}
	}
	if (!bFind)
	{
	//	//ASSERT(0);
		return;
	}
	// ��1����Ʒ��Ϣת��Ϊ������
	int iDelayFeeCount = 0;//stOut.delayFeeNum;
	const int iRowCount = 5+iDelayFeeCount;
	T_CommUserInfoList *pList = new T_CommUserInfoList[iRowCount];
	if (pList==NULL)
	{
		//ASSERT(0);
		return;
	}
	
	pList[0].pName1 = _T(" ��Ʒ����");
	pList[0].pValue1 = stOut.stock_code;
	pList[0].pName2 = _T("������С��ί������");
	char singleMinDelegate[32]={0};
	sprintf(singleMinDelegate,"%ld",stOut.min_entrust_amount);
	pList[0].pValue2 = singleMinDelegate;
	pList[0].pName3 =  _T("�������㷨");
	pList[0].pValue3 = GetFeeType(stOut.charge_con_type);
	
	pList[1].pName1 = _T(" ��Ʒ����");
	pList[1].pValue1 = stOut.stock_name;
	pList[1].pName2 = _T("��������ί������");
	char singleMaxDelegate[32]={0};
	sprintf(singleMaxDelegate,"%ld",stOut.max_entrust_amount);
	pList[1].pValue2 = singleMaxDelegate;
	pList[1].pName3 =  _T("������ϵ��");
	char feeValue[16]={0};

	if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
	{
		if ((-0.000001<stOut.charge_ratio) && (0.000001>stOut.charge_ratio))
		{
			sprintf(feeValue,"%.4f",m_dFeeScale);
		}
		else
		{
			sprintf(feeValue,"%.4f",stOut.charge_ratio);
		}
	}
	else
	{
		sprintf(feeValue,"%.4f",stOut.charge_ratio);
	}
	pList[1].pValue3 = feeValue;
	
	pList[2].pName1 = _T(" ��Ʒ״̬");
	pList[2].pValue1 = GetCommInfoStatus(stOut.status);
	pList[2].pName2 = _T("���ֲ���");
	char maxHold[32]={0};
	sprintf(maxHold,"%ld",stOut.max_hold_amount);
	pList[2].pValue2 = maxHold;
	pList[2].pName3 = _T("��������ȡ��ʽ");
	pList[2].pValue3 = GetFeeGetType(stOut.charge_tra_type);

	pList[3].pName1 = _T(" ���׵�λ");
	if (stOut.trade_unit.IsEmpty())
	{
		pList[3].pValue1 = stOut.stock_unit;
	}
	else
	{
		pList[3].pValue1 = stOut.trade_unit;
	}
	pList[3].pName2 = _T("��С�䶯��λ");
	char priceMinChge[16]={0};
	sprintf(priceMinChge,"%.4f", stOut.price_minchange);
	pList[3].pValue2 = priceMinChge;
	//----------
	pList[3].pName3 = _T("��֤���㷨");
	pList[3].pValue3 = GetMarginType(stOut.bail_type);

	pList[4].pName1 = _T(" ���ֱ�֤��ϵ��");
	char marginValue[16]={0};
	sprintf(marginValue,"%.4f",stOut.margin_ratio);
	pList[4].pValue1 = marginValue;
	pList[4].pName2 = _T("���ڷ��㷨");
	pList[4].pValue2 = (char*)GetDeferType(stOut.delay_type);
	pList[4].pName3 = _T("���ڷ���");
	pList[4].pValue3 = _T("����ֵ");

	pGrid->SetRowCount(iRowCount+pGrid->GetFixedRowCount());

	// ����Ҫ����Ϣ��ֳ�4��
	for (int j=0;j<iRowCount;j++)
	{
		int iRow = j + pGrid->GetFixedRowCount();
		
		UpdateOneCommInfoRow(iRow, pList[j]);
	}
	
	//pGrid->EnsureTopLeftCell(stCellTopLeft);//��������������
	pGrid->SetSelectedRange(cellRangeSel);
	pGrid->Refresh();

 	delete []pList;

	//��Ʒ��Ϣ�ܼ�¼�߶�
	m_iAllCommInfoHeight = pGrid->GetRowCount() * pGrid->GetRowHeight(0);
}

void CDlgTradeQuery::UpdateOneCommInfoRow( int iRow, const T_CommUserInfoList lst)
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}

	CGridCtrlNormal *pGrid = &m_GridCommInfo;
	const T_TradeGridHeadGroup &stGridHeadGroup = s_kstTradeGridHeadGroupCommInfo;


	COLORREF clrRise, clrFall, clrKeep;
	GetRiseFallColor(clrRise, clrFall, clrKeep);


	pGrid->SetRowHeight(iRow, GRID_ITEM_HEIGHT);
	
	// ��ͷ
	const T_TradeGridHead *pstHeader = stGridHeadGroup.pstGridHead;
	const int iHeaderCount = stGridHeadGroup.iGridHeadCount;
	if ( pstHeader!=NULL && iHeaderCount>0 )
	{
		for ( int iCol=0; iCol<iHeaderCount; ++iCol )
		{
			const T_TradeGridHead &stHeader = pstHeader[iCol];
			CGridCellBase *pCell = pGrid->GetCell(iRow, iCol);
			if ( !pCell )
			{
				continue;
			}

			if (iRow > 0 && iRow % 2 == 0)
			{
				pCell->SetBackClr(RGB(242,240,235));
			}
			
			if ( pCell->IsFixedCol() )
			{
				pCell->SetTextClr(stGridHeadGroup.clrFixedRow);
			}
			
			pCell->SetFormat(stHeader.nFmt);
			
			switch ( stHeader.iHeadType )
			{
			case ETGHT_ACCINFO_Name1:
				{
					CString Str = lst.pName1;
					pCell->SetText(Str);
				}
				break;
			case ETGHT_ACCINFO_Value1:
				{
					CString Str = lst.pValue1;
					pCell->SetText(Str);
				}
				break;
			case ETGHT_ACCINFO_Name2:
				{
					CString Str = lst.pName2;
					pCell->SetText(Str);
				}
				break;
			case ETGHT_ACCINFO_Value2:
				{
					CString Str = lst.pValue2;
					pCell->SetText(Str);
				}
				break;
			case ETGHT_ACCINFO_Name3:
				{
					CString str = lst.pName3;
					pCell->SetText(str);
				}
				break;
			case ETGHT_ACCINFO_Value3:
				{
					CString str = lst.pValue3;
					pCell->SetText(str);
				}
				break;
 			default:
				ASSERT( 0 );
				break;
			}
		}
	}
}

// ��ͻ���Ϣ
void CDlgTradeQuery::OnQueryUserInfoResponse()
{
//	ReloadGridUserInfo();
	ReloadGridUserInfoMain();
}

void CDlgTradeQuery::ReloadGridUserInfo()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}

	CGridCtrlNormal *pGrid = &m_GridAccInfo;
	const CClientRespUserInfo &aQuery = m_pTradeBid->GetCacheUserInfo();
	
	/*CCellID stCellTopLeft = */pGrid->GetVisibleNonFixedCellRange().GetTopLeft();
	CCellRange cellRangeSel = pGrid->GetSelectedCellRange();
	
//	pGrid->DeleteNonFixedRows();
	pGrid->SetRowCount(1/*aQuery.size()*/+pGrid->GetFixedRowCount());
	for ( int i=0; i<1/*aQuery.size()*/; ++i )
	{
		const CClientRespUserInfo &stOut = aQuery;
		int iRow = i + pGrid->GetFixedRowCount();
		
		UpdateOneUserInfoRow(iRow, stOut);
	}
	
//	pGrid->EnsureTopLeftCell(stCellTopLeft);
	pGrid->SetSelectedRange(cellRangeSel);
	pGrid->Refresh();
}

void CDlgTradeQuery::UpdateOneUserInfoRow( int iRow, const CClientRespUserInfo &stOut)
{
// 	if ( m_pTradeBid==NULL )
// 	{
// 		ASSERT( 0 );
// 		return;
// 	}
// 
// 	CGridCtrlNormal *pGrid = &m_GridAccInfo;
// 	const T_TradeGridHeadGroup &stGridHeadGroup = s_kstTradeGridHeadGroupAccInfo;
// 
// 
// 	COLORREF clrRise, clrFall, clrKeep;
// 	GetRiseFallColor(clrRise, clrFall, clrKeep);
// 	
// 	// ��ͷ
// 	const T_TradeGridHead *pstHeader = stGridHeadGroup.pstGridHead;
// 	const int iHeaderCount = stGridHeadGroup.iGridHeadCount;
// 	if ( pstHeader!=NULL && iHeaderCount>0 )
// 	{
// 		for ( int iCol=0; iCol<iHeaderCount; ++iCol )
// 		{
// 			const T_TradeGridHead &stHeader = pstHeader[iCol];
// 			CGridCellBase *pCell = pGrid->GetCell(iRow, iCol);
// 			if ( !pCell )
// 			{
// 				continue;
// 			}
// 			
// 			if ( pCell->IsFixedCol() )
// 			{
// 				pCell->SetTextClr(stGridHeadGroup.clrFixedRow);
// 			}
// 			
// 			pCell->SetFormat(stHeader.nFmt);
// 			
// 			switch ( stHeader.iHeadType )
// 			{	
// 			case ETGHT_ACCINFO_TraderName:
// 				{
// 					CString Str = stOut.traderName;
// 					pCell->SetText(Str);
// 				}
// 				break;
// 			case ETGHT_ACCINFO_InitialRights:
// 				{
// 					CString Str = _T("-");
// 					Str.Format(_T("%.2f"),stOut.initialRights/*)+atof(stOut.lastMargin)*/);
// 					pCell->SetText(Str);
// 				}
// 				break;
// 			case ETGHT_ACCINFO_CurrentFloatPL:
// 				{
// 					CString Str = _T("-");
// 					double dFloatPL = GetFloatPL();
// 					Str.Format(_T("%.2f"),dFloatPL);
// 					pCell->SetText(Str);
// 					if (dFloatPL>0)
// 					{
// 						pCell->SetTextClr(TEXT_COLOR_RED);
// 					}
// 					else if (dFloatPL<0)
// 					{
// 						pCell->SetTextClr(TEXT_COLOR_GRE);
// 					}
// 				}
// 				break;
// 			case ETGHT_ACCINFO_AvailableMargin:
// 				{
// 					CString Str = _T("-");
// 					Str.Format(_T("%.2f"),stOut.currentAvailableMoney);
// 					pCell->SetText(Str);
// 				}
// 				break;
// 			case ETGHT_ACCINFO_CurrentMargin:
// 				{
// 					CString Str = _T("-");
// 					Str.Format(_T("%.2f"),stOut.currentMargin);
// 					pCell->SetText(Str);
// 				}
// 				break;
// 			case ETGHT_ACCINFO_FreezeMargin:
// 				{
// 					CString Str = _T("-");
// 					Str.Format(_T("%.2f"),stOut.freezeMargin);
// 					pCell->SetText(Str);
// 				}
// 				break;
// 			case ETGHT_ACCINFO_FreezeFee:
// 				{
// 					CString Str = _T("-");
// 					Str.Format(_T("%.2f"),stOut.freezeFee);
// 					pCell->SetText(Str);
// 				}
// 				break;
// 			case ETGHT_ACCINFO_FundRisks:
// 				{
// 					CString Str = _T("-");
// 					Str.Format(_T("%.2f%s"),stOut.fundRisks*100,"%");
// 					pCell->SetText(Str);
// 				}
// 				break;
// 			case ETGHT_ACCINFO_CurrentRights:
// 				{
// 					CString Str = _T("-");
// 					Str.Format(_T("%.2f"),stOut.currentRights);
// 					pCell->SetText(Str);
// 				}
// 				break;
//  			default:
// 				ASSERT( 0 );
// 				break;
// 			}
// 		}
// 	}
}

void CDlgTradeQuery::ReloadGridUserInfoMain()
{
	if ( m_pTradeBid == NULL )
	{
		ASSERT( 0 );
		return;
	}
	
	CGridCtrlNormal *pGrid = &m_GridAccInfoMain;
	
	const CClientRespUserInfo &aQuery = m_pTradeBid->GetCacheUserInfo();
	
	/*CCellID stCellTopLeft = */pGrid->GetVisibleNonFixedCellRange().GetTopLeft();
	CCellRange cellRangeSel = pGrid->GetSelectedCellRange();

	CClientRespUserInfo stOut;
	for ( int i = 0; i < 1; ++i ) // ֻ��һ��
	{
		stOut = aQuery;
		break;
	}
	m_strMidCode = stOut.mid_code;
	// ��1���˻���Ϣת��Ϊ������
	const int iRowCount = 5;
	T_CommUserInfoList *pList = new T_CommUserInfoList[iRowCount];
	if (pList==NULL)
	{
		//ASSERT(0);
		return;
	}

	double dFloatPL=0, dFee=0;
	if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
	{
		GetTodayFloatPL(dFloatPL, dFee);
	}
	else
	{
		dFloatPL = stOut.zt_fund;
		dFee = stOut.today_fee;
	}
	
	pList[0].pName1 = _T("�˻�����");
	if (stOut.trade_name.IsEmpty())
	{
		pList[0].pValue1 =_T("-");
	}
	else
	{
		pList[0].pValue1 = stOut.trade_name;
	}
	pList[0].pName2 = _T("���ճ����");
	char chMoneyInOut[32]={0};
	sprintf(chMoneyInOut,"%.2f",stOut.io_fund);
	pList[0].pValue2 = chMoneyInOut;
	pList[0].pName3 = _T("����ӯ��");
	char chCurrentFloatPL[32]={0};
	sprintf(chCurrentFloatPL,"%.2f", GetFloatPL());
	pList[0].pValue3 = chCurrentFloatPL;

	pList[1].pName1 = _T("��¼�˺�");
	pList[1].pValue1 = m_pTradeLoginInfo->StrLoginUser;
	pList[1].pName2 = _T("����ƽ��ӯ���ϼ�");
	char currentPL[32]={0};
	sprintf(currentPL,"%.2f", dFloatPL);
	pList[1].pValue2 = currentPL;
	pList[1].pName3 = _T("���ñ�֤��");
	char chLastMargin[32]={0};
	sprintf(chLastMargin,"%.2f", (stOut.today_enable + GetFloatPL())); 
	pList[1].pValue3 = chLastMargin;

	pList[2].pName1 = _T("�ڳ�Ȩ��");
	char chInitialRights[32]={0};
	sprintf(chInitialRights,"%.2f",stOut.first_balance);
	pList[2].pValue1 = chInitialRights;
	pList[2].pName2 = _T("���������Ѻϼ�");
	char currentFee[32]={0};
	if (dFee>0.000001)
	{
		dFee = -dFee;
	}
	sprintf(currentFee,"%.2f", dFee);
	pList[2].pValue2 = currentFee;
	pList[2].pName3 = _T("ռ�ñ�֤��");
	char currentMargin[32]={0};
	sprintf(currentMargin,"%.2f",stOut.today_caution);
	pList[2].pValue3 = currentMargin;
	
	pList[3].pName1= _T("��ǰȨ��");
	char chCurrentRights[32]={0};
	sprintf(chCurrentRights,"%.2f",stOut.today_enable+GetFloatPL()+stOut.today_caution+stOut.froze_margin);
	pList[3].pValue1 = chCurrentRights;
	pList[3].pName2 = _T("�������ڷ�");
	char lastDelayFee[32]={0};
	sprintf(lastDelayFee,"%.2f",stOut.yesterday_delay);
	pList[3].pValue2 = lastDelayFee;
	pList[3].pName3 = _T("���ᱣ֤��");
	char freezeMargin[32]={0};
	sprintf(freezeMargin,"%.2f",stOut.froze_margin);
	pList[3].pValue3 = freezeMargin;

	pList[4].pName1 = _T("�˺�״̬");
	// U��δ���Un��N��������Normal��F�����ᣨFrozen��D����ֹ��Demise��Delete��C:  �Ѵ���
	pList[4].pValue1 = GetUserInfoStatus(stOut.status);
	pList[4].pName2 = _T("������");
	char fundRisks[32]={0};
	if ((stOut.today_caution>-0.0000001)&&(stOut.today_caution<0.0000001))
	{
		sprintf(fundRisks,"%.2f%s",0.,"%");
	}
	else
	{
		sprintf(fundRisks,"%.2f%s",atof(chCurrentRights)/stOut.today_caution/*stOut.risk*/*100,"%");
	}
	pList[4].pValue2 = fundRisks;
	pList[4].pName3 = _T("����������");
	char freezeFee[32]={0};
	sprintf(freezeFee,"%.2f",stOut.froze_fee);
	pList[4].pValue3 = freezeFee;

	// ������ҳ�ϵ���ʾ����
	m_dlgHomePage.SetClientRespUserInfo(pList);
	((CDlgTradeBidContainer*)GetParent())->ShowAccountInfo(pList);
	//if(m_bAccountInfo == false)	// ���������ʾ�ʻ�ҳ�棬�ڴ˴��ͷ��ڴ��˳�
	//{
	//	delete []pList;		pList = NULL;
	//	return ;
	//}
 
	pGrid->SetRowCount(iRowCount+pGrid->GetFixedRowCount());

	// ����Ҫ����Ϣ��ֳ�4��
	for (int j=0;j<iRowCount;j++)
	{
		int iRow = j + pGrid->GetFixedRowCount();
		
		UpdateOneUserInfoMainRow(iRow, pList[j]);
	}
	
	//pGrid->EnsureTopLeftCell(stCellTopLeft);//��������������
	pGrid->SetSelectedRange(cellRangeSel);
	pGrid->Refresh();

	delete []pList;	pList = NULL;
	
	//��Ʒ��Ϣ�ܼ�¼�߶�
	m_iAllAccInfoHeight = pGrid->GetRowCount() * pGrid->GetRowHeight(0);
}

void CDlgTradeQuery::UpdateOneUserInfoMainRow( int iRow, const T_CommUserInfoList lst)
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}
	
	CGridCtrlNormal *pGrid = &m_GridAccInfoMain;
	const T_TradeGridHeadGroup &stGridHeadGroup = s_kstTradeGridHeadGroupAccInfoMain;
	
	
	COLORREF clrRise, clrFall, clrKeep;
	GetRiseFallColor(clrRise, clrFall, clrKeep);

	pGrid->SetRowHeight(iRow, GRID_ITEM_HEIGHT);
	
	// ��ͷ
	const T_TradeGridHead *pstHeader = stGridHeadGroup.pstGridHead;
	const int iHeaderCount = stGridHeadGroup.iGridHeadCount;
	if ( pstHeader!=NULL && iHeaderCount>0 )
	{
		for ( int iCol=0; iCol<iHeaderCount; ++iCol )
		{
			const T_TradeGridHead &stHeader = pstHeader[iCol];
			CGridCellBase *pCell = pGrid->GetCell(iRow, iCol);
			if ( !pCell )
			{
				continue;
			}

// 			if (iCol % 2 != 0)
// 			{
// 				pCell->SetSeparatoLine(true);
// 			}

			if (iRow > 0 && iRow % 2 == 0)
			{
				pCell->SetBackClr(RGB(242,240,235));
			}
			
			if ( pCell->IsFixedCol() )
			{
				pCell->SetTextClr(stGridHeadGroup.clrFixedRow);
			}
			
			pCell->SetFormat(stHeader.nFmt);
			
			switch ( stHeader.iHeadType )
			{
			case ETGHT_ACCINFO_Name1:
				{
					CString Str = lst.pName1;
					pCell->SetText(Str);
				}
				break;
			case ETGHT_ACCINFO_Value1:
				{
					CString Str = lst.pValue1;
					pCell->SetText(Str);
				}
				break;
			case ETGHT_ACCINFO_Name2:
				{
					CString Str = lst.pName2;
					pCell->SetText(Str);
				}
				break;
			case ETGHT_ACCINFO_Value2:
				{
					CString Str = lst.pValue2;
					pCell->SetText(Str);
				}
				break;
			case ETGHT_ACCINFO_Name3:
				{
					CString Str = lst.pName3;
					pCell->SetText(Str);
				}
				break;
			case ETGHT_ACCINFO_Value3:
				{
					CString Str = lst.pValue3;
					pCell->SetText(Str);

					if (1==iRow)//����ӯ��
					{
						string strVal;
						UnicodeToUtf8(lst.pValue3, strVal);
						double dFloatPL = atof(strVal.c_str());
						if (dFloatPL>0)
						{
							pCell->SetTextClr(TEXT_COLOR_RED);
						}
						else if (dFloatPL<0)
						{
							pCell->SetTextClr(TEXT_COLOR_GRE);
						}
						else
						{
							pCell->SetTextClr(GRID_TEXT_COLOR);
						}
					}
				}
				break;
			default:
				ASSERT( 0 );
				break;
			}
		}
	}
}

void CDlgTradeQuery::OnReqEntrustResponse(bool32 bShowTip)
{
}

void CDlgTradeQuery::OnReqCancelEntrustResponse()
{
	if (ETLS_NotLogin == m_eTradeStatus)
	{
		return;
	}

	E_TipType eTipType = ETT_WAR;
	
	CString strTip = _T("");
	eTipType = ETT_ERR;
	CDlgTip dlg;

	CClientRespCancelEntrust resp = m_pTradeBid->GetCacheReqCancelEntrust();
	if (0 == resp.error_no) // �ɹ�
	{
		eTipType = ETT_SUC;
		RefreshList();		//��ǰ����ˢ�£���Ҫ����ʾ�Ի��򵯳�
		DoQueryAsy(EQT_QueryUserInfo);
	}

	if (0 < m_iRevokeLimitOrderCnt)
	{
		m_iRevokeLimitOrderCnt--;
		return;
	}

	dlg.m_strTipMsg = resp.error_info;
	dlg.m_eTipType = eTipType;
	dlg.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��
	dlg.DoModal();

}

void CDlgTradeQuery::OnReqModifyPwdResponse()
{
	
}

void CDlgTradeQuery::OnReqSetStopLPResponse()
{
	
}

void CDlgTradeQuery::OnReqCancelStopLPResponse()
{
	if (ETLS_NotLogin == m_eTradeStatus)
	{
		return;
	}

	E_TipType eTipType = ETT_WAR;
	
	CString strTip = _T("");
	eTipType = ETT_ERR;
	CDlgTip dlg;

	CClientRespCancelStopLP resp = m_pTradeBid->GetCacheReqCancelStopLP();

	if (0 == resp.error_no) // �ɹ�
	{
		eTipType = ETT_SUC;
	}
	dlg.m_strTipMsg = resp.error_info;
	
	dlg.m_eTipType = eTipType;
	dlg.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��

	if (IDOK==dlg.DoModal())
	{
	}
	if (0 == resp.error_no) //����һ��
		RefreshList();
}

void CDlgTradeQuery::OnDisConnectResponse()
{
	// 	���ߴ���
	if (ETLS_Logined == m_eTradeStatus)
	{
		// �رս��״���
		m_eTradeStatus = ETLS_NotLogin;
		KillTimer(TIME_REFRESH_QUOTE);
		KillTimer(TIME_REFRESH_HOLDE);
		//���µ�¼orע��
		int iMode = 0;
		CDlgCloseTradeTip dlg;
		dlg.m_strTipMsg = _T("����������ӶϿ�");

		CClientRespError respError = m_pTradeBid->GetCacheError();
		if (-1 == respError.error_no) //�ʺ��������ط���¼
		{
			dlg.m_strTipMsg = respError.error_info;// _T("�����˺�����һ�ص��¼�򱻹���Աǿ�����ߣ�");
		}

		if (IDOK == dlg.DoModal())
		{
			iMode = 0;
		}
		else
		{ 
			iMode = 1;
		}
		PostMessage(EM_Message_WndClose,iMode,0);
		//MessageBox(strErrMsg,L"����",MB_OK|MB_ICONERROR);
		//	WndClose();
	}
	
// 	//֪ͨ����״̬�ı�
// 	NetWorkStatusNotify(1);
// 	//����������Ϣ
// 	if (ETLS_Logined==m_eTradeStatus&&m_bNetWorkStatus)
// 	{
// 		MessageBox(_T("���ӶϿ�"), L"����", MB_OK|MB_ICONERROR);
// 	}
}

// ��ѯ����
void CDlgTradeQuery::OnQueryQuotationResponse()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}
	
	m_QuotationResultVector = m_pTradeBid->GetCacheQuotation();

	// ������£������³ֲ���ϸ���˻���Ϣ,����������
	ReloadGridHoldDetailMain();
 	ReloadGridUserInfoMain();

	DealEntrustNo();
}

//��ȡ��ǰ�����۸�
void CDlgTradeQuery::GetBuySellPrice(const char* pCommID,double &dBuyPrice,double &dSellPrice,CString &dTradeUnit,double &dMarginratio)
{
	if (NULL==pCommID)
	{
		//ASSERT(0);
		return;
	}

	dBuyPrice = 0;
	dSellPrice = 0;
	//���Ӧ������Ϣ
//	m_LockGetPrice.lock();
	int i = 0;
	T_TradeMerchInfo stOutMerch;
	for (i=0; i<m_CommInfoResultVector.size(); i++)// ����Ʒ����
	{
		stOutMerch = m_CommInfoResultVector[i];
		string strCode;
		UnicodeToUtf8(stOutMerch.stock_code, strCode);
		if (_stricmp(pCommID, strCode.c_str())==0) //��Ʒ������ͬ
		{
			if (stOutMerch.trade_unit.IsEmpty())
			{
				dTradeUnit = stOutMerch.stock_unit;	//���׵�λ
			}
			else
			{
				dTradeUnit = stOutMerch.trade_unit;	//���׵�λ
			}

			dMarginratio = stOutMerch.margin_ratio; // ��֤��ϵ��
			break;
		}
	}

	for (i=0; i<m_QuotationResultVector.size(); i++)// ���������
	{
		const T_RespQuote stOut = m_QuotationResultVector[i];
		string strCode;
		UnicodeToUtf8(stOut.stock_code, strCode);
		if (_stricmp(pCommID, strCode.c_str())==0) //��Ʒ������ͬ
		{
			if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
			{
				dBuyPrice = stOut.buy_price;
				dSellPrice = stOut.sell_price;
			}
			else
			{
				if (_T("1") == stOutMerch.price_mode)
				{
					dBuyPrice = stOut.last_price + stOutMerch.buy_pdd*stOutMerch.price_minchange;
					dSellPrice = stOut.last_price + stOutMerch.sell_pdd*stOutMerch.price_minchange;
				}
				else
				{
					dBuyPrice = stOut.buy_price + stOutMerch.buy_pdd*stOutMerch.price_minchange;
					dSellPrice = stOut.sell_price + stOutMerch.sell_pdd*stOutMerch.price_minchange;
				}
			}
			break;
		}
	}

	if (((dBuyPrice<0.0000001&&dBuyPrice>-0.0000001) ||(dSellPrice<0.0000001&&dSellPrice>-0.0000001)) && (ETT_TRADE_SIMULATE!=m_pTradeLoginInfo->eTradeLoginType))
	{
		if (_T("1") == stOutMerch.price_mode)
		{
			dBuyPrice = stOutMerch.last_price + stOutMerch.buy_pdd*stOutMerch.price_minchange;
			dSellPrice = stOutMerch.last_price + stOutMerch.sell_pdd*stOutMerch.price_minchange;
		}
		else
		{
			dBuyPrice = stOutMerch.buy_price + stOutMerch.buy_pdd*stOutMerch.price_minchange;
			dSellPrice = stOutMerch.sell_price + stOutMerch.sell_pdd*stOutMerch.price_minchange;
		}
	}
//	m_LockGetPrice.unlock();
}

double CDlgTradeQuery::GetFloatPL()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return 0;
	}
	
	double dFloatPLRet = 0;
	const QueryHoldDetailResultVector &aQuery = m_pTradeBid->GetCacheHoldDetail();

//	m_LockFloatPL.lock();
	for ( int i=0; i<aQuery.size(); ++i )
	{
		const T_RespQueryHold &stOut = aQuery[i];
		double dBuyPrice = 0;	//���
		double dSellPrice = 0;	//����
		double dClosePrice = 0;	//ƽ�ּ�
		CString dTradeUnit = "";	//���׵�λ
		double dFloatPL = 0;	//����ӯ��
		double dMargin;
		int buySell=1; // ��������
	// 	TradeFunction tradeFunc;
		
		string strCode;
		UnicodeToUtf8(stOut.stock_code, strCode);
		GetBuySellPrice(strCode.c_str(), dBuyPrice, dSellPrice, dTradeUnit, dMargin);//��ȡƽ�ּ۸�

		if (_T("B") == stOut.entrust_bs)
		{
			dClosePrice = dSellPrice;
			buySell = 1;
		}
		else
		{
			dClosePrice = dBuyPrice;
			buySell = -1;
		}

		string strFloat;
		UnicodeToUtf8(dTradeUnit, strFloat);
		double dUint = atof(strFloat.c_str());

		dClosePrice = reversef_(dClosePrice, 2);

		if (dClosePrice<0.000001 && dClosePrice>-0.000001)
		{
		}
		else
		{
			if(stOut.hold_avg<0.000001 && stOut.hold_avg>-0.000001)
			{
				dFloatPL = (dClosePrice - stOut.hold_price)*buySell*dUint*stOut.hold_amount;
			}
			else
			{
				dFloatPL = (dClosePrice - stOut.hold_avg)*buySell*dUint*stOut.hold_amount;
			}
		}

		dFloatPLRet += dFloatPL;
	}
//	m_LockFloatPL.unlock();

	return dFloatPLRet;
}

//ȡ�ɽ�����
long CDlgTradeQuery::GetTotalQty()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return 0;
	}
	
	long lTotalQty = 0;
	const QueryDealResultVector &aQuery = m_pTradeBid->GetCacheDeal();
	
	//	m_LockFloatPL.lock();
	for ( int i=0; i<aQuery.size(); ++i )
	{
		const T_RespQueryDeal &stOut = aQuery[i];
		lTotalQty += stOut.business_amount;
	}

	return lTotalQty;
}

void CDlgTradeQuery::GetTodayFloatPL(OUT double &dFloatPL, OUT double &dFee)
{
	if ( NULL == m_pTradeBid )
	{
		ASSERT( 0 );
		return;
	}

	dFloatPL = 0;
	dFee = 0;
	const QueryDealResultVector &aDeal = m_pTradeBid->GetCacheDeal();
	int32 iDealCnt = 0;
	iDealCnt = aDeal.size();
	for ( int i=0; i<iDealCnt; ++i )
	{
		const T_RespQueryDeal &stDeal = aDeal[i];
		dFloatPL += stDeal.close_profitloss;
		dFee += stDeal.charge;
	}
}

// void CDlgTradeQuery::OnDisConnect( const char *pDisInfo )
// {
// 	// 
// }
// 
// void CDlgTradeQuery::OnStatusMsg( const char *pMsg,E_Init enInit )
// {
// 	// ��¼��Ϣ��������
// }

//////////////////////////////////////////////////////////////////////////
// other

void CDlgTradeQuery::CreateControl()
{
	CString strFontName = gFontFactory.GetExistFontName(L"΢���ź�");
	
	LOGFONT lgFont = {0};
	lgFont.lfHeight =20;
    lgFont.lfWidth = 0;
    lgFont.lfEscapement = 0;
    lgFont.lfOrientation = 0;
    lgFont.lfWeight =  FW_NORMAL;
    lgFont.lfItalic = 0;
    lgFont.lfUnderline = 0;
    lgFont.lfStrikeOut = 0;
	lgFont.lfCharSet = ANSI_CHARSET;
    lgFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lgFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lgFont.lfQuality = PROOF_QUALITY;
    lgFont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
	_tcscpy(lgFont.lfFaceName, strFontName);///
	
	
	m_pFont.CreateFontIndirect(&lgFont);
	/* �ſ� */
	// �ֲ���ϸ
	m_GroupHoldDetail.RegistrID(IDC_DEF_GROUPBOX_HOLDDETAIL);
	m_GroupHoldDetail.Create(L"", WS_CHILD|WS_VISIBLE|BS_GROUPBOX, CRect(0, 0, 0, 0), this, IDC_DEF_GROUPBOX_HOLDDETAIL);
	m_GroupHoldDetail.ShowWindow(SW_SHOW);
	m_StaticHoldDetail.Create(L"�ֲ���ϸ", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_HOLDDETAIL);
	m_StaticHoldDetail.ShowWindow(SW_SHOW);
	m_StaticHoldDetail.SetFont(&m_pFont);

	/*ί�в�ѯ*/
	m_GroupEntrustMain.Create(L"", WS_CHILD|WS_VISIBLE|BS_GROUPBOX, CRect(0, 0, 0, 0), this, IDC_DEF_GROUPBOX_ENTRUSTMAIN);
	m_GroupEntrustMain.ShowWindow(SW_SHOW);
	m_StaticEntrustMain.Create(L"ί�в�ѯ", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_ENTRUSTMAIN);
	m_StaticEntrustMain.ShowWindow(SW_SHOW);
	m_StaticEntrustMain.SetFont(&m_pFont);
	m_DropEntrustMainCommInfo.Create( WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, CRect(0, 0, 0, 200), this, IDC_DEF_DROP_ENTRUSTMAIN_COMMINFO);
	m_DropEntrustMainCommInfo.ShowWindow(SW_SHOW);
	m_StaticEntrustMainCommInfo.Create(L"��Ʒ", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_ENTRUSTMAIN_COMMINFO);
	m_StaticEntrustMainCommInfo.ShowWindow(SW_SHOW);
	m_StaticEntrustMainCommInfo.SetFont(&m_pFont);
	m_DropEntrustMainType.Create( WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, CRect(0, 0, 0, 200), this, IDC_DEF_DROP_ENTRUSTMAIN_TYPE);
	m_DropEntrustMainType.ShowWindow(SW_SHOW);
	m_StaticEntrustMainType.Create(L"ί�е�����", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_ENTRUSTMAIN_TYPE);
	m_StaticEntrustMainType.ShowWindow(SW_SHOW);
	m_StaticEntrustMainType.SetFont(&m_pFont);
	m_DropEntrustMainStatus.Create( WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, CRect(0, 0, 0, 200), this, IDC_DEF_DROP_ENTRUSTMAIN_STATUS);
	m_DropEntrustMainStatus.ShowWindow(SW_SHOW);
	m_StaticEntrustMainStatus.Create(L"ί�е�״̬", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_ENTRUSTMAIN_STATUS);
	m_StaticEntrustMainStatus.ShowWindow(SW_SHOW);
	m_StaticEntrustMainStatus.SetFont(&m_pFont);
	m_BtnEntrustMainQuery.Create(L"��ѯ", WS_CHILD | BS_DEFPUSHBUTTON, CRect(0, 0, 0, 0), this, IDC_DEF_BUTTON_ENTRUSTMAIN_QUERY);
	m_BtnEntrustMainQuery.ShowWindow(SW_SHOW);
	m_BtnEntrustMainQuery.SetFont(&m_pFont);
	m_BtnEntrustMainReset.Create(L"����", WS_CHILD | BS_DEFPUSHBUTTON, CRect(0, 0, 0, 0), this, IDC_DEF_BUTTON_ENTRUSTMAIN_RESET);
	m_BtnEntrustMainReset.ShowWindow(SW_SHOW);
	m_BtnEntrustMainReset.SetFont(&m_pFont);
	m_DropEntrustMainBuySell.Create( WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, CRect(0, 0, 0, 200), this, IDC_DEF_DROP_ENTRUSTMAIN_BUYSELL);
	m_DropEntrustMainBuySell.ShowWindow(SW_SHOW);
	m_StaticEntrustMainBuySell.Create(L"��/��", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_ENTRUSTMAIN_BUYSELL);
	m_StaticEntrustMainBuySell.ShowWindow(SW_SHOW);
	m_StaticEntrustMainBuySell.SetFont(&m_pFont);
	m_DropEntrustMainOpenClose.Create( WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, CRect(0, 0, 0, 200), this, IDC_DEF_DROP_ENTRUSTMAIN_OPENCLOSE);
	m_DropEntrustMainOpenClose.ShowWindow(SW_SHOW);
	m_StaticEntrustMainOpenClose.Create(L"����/ƽ��", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_ENTRUSTMAIN_OPENCLOSE);
	m_StaticEntrustMainOpenClose.ShowWindow(SW_SHOW);
	m_StaticEntrustMainOpenClose.SetFont(&m_pFont);

	/* �ɽ���ѯ */

	m_DropDealCommInfo.Create( WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, CRect(0, 0, 0, 200), this, IDC_DEF_DROP_DEAL_COMMINFO);
	m_DropDealCommInfo.ShowWindow(SW_SHOW);
	m_StaticDealCommInfo.Create(L"��Ʒ", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_DEAL_COMMINFO);
	m_StaticDealCommInfo.ShowWindow(SW_SHOW);
	m_StaticDealCommInfo.SetFont(&m_pFont);
	m_DropDealBuySell.Create( WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, CRect(0, 0, 0, 200), this, IDC_DEF_DROP_DEAL_BUYSELL);
	m_DropDealBuySell.ShowWindow(SW_SHOW);
	m_StaticDealBuySell.Create(L"��/��", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_DEAL_BUYSELL);
	m_StaticDealBuySell.ShowWindow(SW_SHOW);
	m_StaticDealBuySell.SetFont(&m_pFont);
	m_DropDealOpenClose.Create( WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, CRect(0, 0, 0, 200), this, IDC_DEF_DROP_DEAL_OPENCLOSE);
	m_DropDealOpenClose.ShowWindow(SW_SHOW);
	m_StaticDealOpenClose.Create(L"����/ƽ��", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_DEAL_OPENCLOSE);
	m_StaticDealOpenClose.ShowWindow(SW_SHOW);
	m_StaticDealOpenClose.SetFont(&m_pFont);
	m_BtnDealQuery.Create(L"��ѯ", WS_CHILD | BS_DEFPUSHBUTTON, CRect(0, 0, 0, 0), this, IDC_DEF_BUTTON_DEAL_QUERY);
	m_BtnDealQuery.ShowWindow(SW_SHOW);
	m_BtnDealQuery.SetFont(&m_pFont);
	m_BtnDealReset.Create(L"����", WS_CHILD | BS_DEFPUSHBUTTON, CRect(0, 0, 0, 0), this, IDC_DEF_BUTTON_DEAL_RESET);
	m_BtnDealReset.ShowWindow(SW_SHOW);
	m_BtnDealReset.SetFont(&m_pFont);
	
	/* �˻���Ϣ */

	/* ��Ʒ��Ϣ */
	m_GroupCommInfo.Create(L"", WS_CHILD|WS_VISIBLE|BS_GROUPBOX, CRect(0, 0, 0, 0), this, IDC_DEF_GROUPBOX_COMMINFO);
	m_GroupCommInfo.ShowWindow(SW_SHOW);
	m_StaticCommInfo.Create(L"��Ʒ��Ϣ", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_COMMINFO);
	m_StaticCommInfo.ShowWindow(SW_SHOW);
	m_StaticCommInfo.SetFont(&m_pFont);

	m_DropCommInfoType.Create( WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED, CRect(0, 0, 0, 200), this, IDC_DEF_DROP_COMMINFO_TYPE);	
	m_DropCommInfoType.ShowWindow(SW_SHOW);

	m_StaticCommInfoType.Create(L"��ѡ����Ʒ:", WS_CHILD|WS_VISIBLE|SS_LEFT, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_COMMINFO_TYPE);
	m_StaticCommInfoType.ShowWindow(SW_SHOW);
	m_StaticCommInfoType.SetFont(&m_pFont);
}	

void CDlgTradeQuery::InitComBox(CFont *pFont)
{
	//LOGFONT lgFont = {0};
	//CFont *pFont = GetFont();
	//if ( NULL!=pFont )
	//{
	//	pFont->GetLogFont(&lgFont);
	//}
	//else
	//{
	//	LOGFONT *plf = CFaceScheme::Instance()->GetSysFont(ESFSmall);
	//	lgFont = *plf;
	//}

	// ί�в�ѯ��
	// ����
	m_DropEntrustMainType.InsertString(0,L"��ʾȫ��");
	m_DropEntrustMainType.InsertString(1,L"�м�ί��");
	m_DropEntrustMainType.InsertString(2,L"ָ��ί��");
	m_DropEntrustMainType.SetFont(pFont);
	m_DropEntrustMainType.SetCurSel(0);
	// ״̬
	m_DropEntrustMainStatus.InsertString(0,L"��ʾȫ��");
	m_DropEntrustMainStatus.InsertString(1,L"��ί��");
	m_DropEntrustMainStatus.InsertString(2,L"�ѳɽ�");
	m_DropEntrustMainStatus.InsertString(3,L"�ѳ���");
	m_DropEntrustMainStatus.SetFont(pFont);
	m_DropEntrustMainStatus.SetCurSel(0);
	// ��/��
	m_DropEntrustMainBuySell.InsertString(0,L"��ʾȫ��");
	m_DropEntrustMainBuySell.InsertString(1,L"����");
	m_DropEntrustMainBuySell.InsertString(2,L"����");
	m_DropEntrustMainBuySell.SetFont(pFont);
	m_DropEntrustMainBuySell.SetCurSel(0);
	// ����/ƽ��
	m_DropEntrustMainOpenClose.InsertString(0,L"��ʾȫ��");
	m_DropEntrustMainOpenClose.InsertString(1,L"����");
	m_DropEntrustMainOpenClose.InsertString(2,L"ƽ��");
	m_DropEntrustMainOpenClose.SetFont(pFont);
	m_DropEntrustMainOpenClose.SetCurSel(0);
	
	// �ɽ���ѯ��
	// ��/��
	m_DropDealBuySell.InsertString(0,L"��ʾȫ��");
	m_DropDealBuySell.InsertString(1,L"����");
	m_DropDealBuySell.InsertString(2,L"����");
	m_DropDealBuySell.SetFont(pFont);
	m_DropDealBuySell.SetCurSel(0);
	// ����/ƽ��
	m_DropDealOpenClose.InsertString(0,L"��ʾȫ��");
	m_DropDealOpenClose.InsertString(1,L"����");
	m_DropDealOpenClose.InsertString(2,L"ƽ��");
	m_DropDealOpenClose.SetFont(pFont);
	m_DropDealOpenClose.SetCurSel(0);	

	m_DropEntrustMainCommInfo.SetFont(pFont);
	m_DropDealCommInfo.SetFont(pFont);
	m_DropCommInfoType.SetFont(pFont);
}

void CDlgTradeQuery::InitCommIfo()
{
	if (NULL==m_pTradeBid)
	{
		return;
	}

	if (!m_bInitCommInfo)
	{
		// ��ʼ��ͷһ��
		T_CommInfoType type0 ;
		type0.iIndex = 0;
		strcpy(type0.chTypeName,"");
		strcpy(type0.chTypeValue,"");
		_CommInfoType::value_type value0( type0.iIndex, type0 );
		m_CommInfoType.insert( value0 );
		
		m_DropEntrustMainCommInfo.InsertString(0,L"��ʾȫ��");
		m_DropDealCommInfo.InsertString(0,L"��ʾȫ��");
		
		const QueryCommInfoResultVector &aQuery = m_pTradeBid->GetCacheCommInfo();
		for ( int i=0; i<aQuery.size(); ++i )
		{
			T_TradeMerchInfo stOut = aQuery[i];
			T_CommInfoType type;
			type.iIndex = i+1;
			string strCode, strName;
			UnicodeToUtf8(stOut.stock_code, strCode);
			UnicodeToUtf8(stOut.stock_name, strName);
			strncpy(type.chTypeValue, strCode.c_str(), sizeof(type.chTypeValue)-1);
			strncpy(type.chTypeName, strName.c_str(), sizeof(type.chTypeName)-1);
			_CommInfoType::value_type value( type.iIndex, type );
			m_CommInfoType.insert( value );
			
// 			wstring wstr;
// 			Gbk32ToUnicode((char *)(LPCTSTR)stOut.stock_name, wstr);
// 			CString str;
// 			str.Format(_T("%s"),wstr.c_str());
			m_DropEntrustMainCommInfo.InsertString(i+1, (LPCTSTR)stOut.stock_name);
			m_DropDealCommInfo.InsertString(i+1, (LPCTSTR)stOut.stock_name);
			m_DropCommInfoType.AddItemEx(stOut.stock_name);

		}
		m_DropEntrustMainCommInfo.SetCurSel(0);
		m_DropDealCommInfo.SetCurSel(0);
		m_DropCommInfoType.SetCurSel(0);
		
		m_bInitCommInfo = true;
	}	
}

HBRUSH CDlgTradeQuery::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO:  �ڴ˸��� DC ���κ�����

	switch(nCtlColor)
	{
		case CTLCOLOR_STATIC:
			{
				if (pWnd->GetDlgCtrlID() == IDC_STATIC_DISENTRUST)
				{
					pDC->SetTextColor(RGB(34,124,237));
				}
				else
				{
					pDC->SetTextColor(RGB(0x4d, 0x4d, 0x4d));
				}
				pDC->SetBkMode(TRANSPARENT);
				
				static HBRUSH hbrush = ::CreateSolidBrush( RGB(0xFC, 0xFD, 0xF5) );
				pDC->SetBkColor( RGB(0xFC, 0xFD, 0xF5) );
				return hbrush;
			}
			break;
		default:
			break;
	}

	
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������

	return hbr;
}

void CDlgTradeQuery::CreateEntrustMainTable(CFont *pFont)	// ί�в�ѯ
{
	CGridCtrlNormal *pGrid = &m_GridEntrustMain;
	CRect rcGrid(0,0,0,0);
	pGrid->Create(rcGrid, this, EGID_ENTRUST);
	pGrid->ShowWindow(SW_SHOW);
	
	pGrid->SetDonotScrollToNonExistCell(GVL_BOTH);
	pGrid->SetDefaultCellType(RUNTIME_CLASS(CGridCellTrade));
	pGrid->SetSingleRowSelection(TRUE);
	pGrid->SetListMode(TRUE);
	pGrid->EnableToolTips(FALSE);
	pGrid->EnableTitleTips(FALSE);
	pGrid->SetShowSelectWhenLoseFocus(FALSE);
	pGrid->SetGridBkColor(GRID_BK_COLOR);
	pGrid->SetFixColBkColor(GRID_BK_COLOR);
	pGrid->SetRemoveSelectClickBlank(TRUE);
	pGrid->SetListHeaderCanClick(FALSE);

	pGrid->GetDefaultCell(FALSE, FALSE)->SetBackClr(GRID_BK_COLOR);
	pGrid->GetDefaultCell(FALSE, FALSE)->SetTextClr(GRID_TEXT_COLOR);
	pGrid->GetDefaultCell(TRUE, FALSE)->SetBackClr(GRID_HEAD_COLOR);
	pGrid->GetDefaultCell(TRUE, FALSE)->SetTextClr(GRID_TEXT_COLOR);
	pGrid->ShowGridLine(false);
	pGrid->SetDrawSelectedCellStyle(GVSDS_DEFAULT);
	pGrid->SetDrawFixedCellGridLineAsNormal(TRUE);
	
	// ��������
	pGrid->SetFont(pFont);
	
	pGrid->SetDefCellMargin(2);
	
	// ������
	CXScrollBar *pHBar = &m_XBarEntrustMainH;
	pHBar->Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, this, CRect(0,0,0,0), s_kiXScrollBarEntrustMainHID);
	
	CXScrollBar *pVBar = &m_XBarEntrustMainV;
	pVBar->Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, this, CRect(0,0,0,0), s_kiXScrollBarEntrustMainVID);
	
	pHBar->SetOwner(pGrid);
	pVBar->SetOwner(pGrid);

	pHBar->UseAppRes();
	pVBar->UseAppRes();

 	// ����ˮƽ������ͼƬ
 	pHBar->SetScrollBarLeftArrowH(-1);
 	pHBar->SetScrollBarRightArrowH(-1);
 	pHBar->SetScrollBarChannelH(IDB_BITMAP_TRADE_SCROLLBAR_CHANNEL);
 	pHBar->SetScrollBarThumbH(IDB_BITMAP_TRADE_HSCROLL);
 	pHBar->SetScrollBarThumbNoColorH(IDB_BITMAP_TRADE_HSCROLL);
	// ���ô�ֱ������ͼƬ
	pVBar->SetScrollBarLeftArrowH(-1);
	pVBar->SetScrollBarRightArrowH(-1);
	pVBar->SetScrollBarChannelH(IDB_BITMAP_TRADE_SCROLLBAR_CHANNEL);
	pVBar->SetScrollBarThumbH(IDB_BITMAP_TRADE_HSCROLL);
 	pVBar->SetScrollBarThumbNoColorH(IDB_BITMAP_TRADE_HSCROLL);

	pGrid->SetScrollBar(pHBar, pVBar);
	
	// ��ͷ
	const T_TradeGridHead *pstHeader = s_kstTradeGridHeadGroupEntrustMain.pstGridHead;
	const int iHeaderCount = s_kstTradeGridHeadGroupEntrustMain.iGridHeadCount;
	const int iFixedColCount = s_kstTradeGridHeadGroupEntrustMain.iFixedColumnCount;
	COLORREF clrFixedRow = s_kstTradeGridHeadGroupEntrustMain.clrFixedRow;
	if ( pstHeader!=NULL && iHeaderCount>0 )
	{
		pGrid->SetFixedRowCount(1);
		pGrid->SetColumnCount(iHeaderCount);
		pGrid->SetFixedColumnCount(iFixedColCount);
		
		for ( int iCol=0; iCol<iHeaderCount; ++iCol )
		{
			const T_TradeGridHead &stHeader = pstHeader[iCol];
			pGrid->SetColumnWidth(iCol, stHeader.iColWidth);
			
			CGridCellBase *pCell = pGrid->GetCell(0, iCol);
			ASSERT( pCell );
			pCell->SetText(stHeader.szName);
			pCell->SetFormat(stHeader.nFmt);
			pCell->SetData(stHeader.iHeadType);
			
			if ( CLR_DEFAULT!=clrFixedRow )
			{
				pCell->SetTextClr(clrFixedRow);
			}
		}
	}
	pGrid->AutoSizeRows();
	pGrid->SetRowHeight(0, GRID_HEAD_HEIGHT);
	pGrid->SetBorderColor(GRID_BORDER_COLOR);
	pGrid->EnableHiddenColUnhide();
	pGrid->BringWindowToTop();
}

void CDlgTradeQuery::CreateDealTable(CFont *pFont)	// �ɽ���ѯ
{
	CGridCtrlNormal *pGrid = &m_GridDeal;
	CRect rcGrid(0,0,0,0);
	pGrid->Create(rcGrid, this, EGID_DEAL);
	pGrid->ShowWindow(SW_SHOW);
	
	pGrid->SetDonotScrollToNonExistCell(GVL_BOTH);
	pGrid->SetDefaultCellType(RUNTIME_CLASS(CGridCellTrade));
	pGrid->SetSingleRowSelection(TRUE);
	pGrid->SetListMode(TRUE);
	pGrid->EnableToolTips(FALSE);
	pGrid->EnableTitleTips(FALSE);
	pGrid->SetShowSelectWhenLoseFocus(FALSE);
	pGrid->SetGridBkColor(GRID_BK_COLOR);
	pGrid->SetFixColBkColor(GRID_BK_COLOR);
	pGrid->SetRemoveSelectClickBlank(TRUE);
	pGrid->SetListHeaderCanClick(FALSE);

	pGrid->GetDefaultCell(FALSE, FALSE)->SetBackClr(GRID_BK_COLOR);
	pGrid->GetDefaultCell(FALSE, FALSE)->SetTextClr(GRID_TEXT_COLOR);
	pGrid->GetDefaultCell(TRUE, FALSE)->SetBackClr(GRID_HEAD_COLOR);
	pGrid->GetDefaultCell(TRUE, FALSE)->SetTextClr(GRID_TEXT_COLOR);
	pGrid->ShowGridLine(false);
	pGrid->SetDrawSelectedCellStyle(GVSDS_DEFAULT);
	pGrid->SetDrawFixedCellGridLineAsNormal(TRUE);
	
	// ��������
	pGrid->SetFont(pFont);
	// 		pGrid->GetDefaultCell(FALSE, FALSE)->SetFont(&lgFont);
	// 		pGrid->GetDefaultCell(FALSE, TRUE)->SetFont(&lgFont);
	// 		pGrid->GetDefaultCell(TRUE, FALSE)->SetFont(&lgFont);
	// 		pGrid->GetDefaultCell(TRUE, TRUE)->SetFont(&lgFont);
	
	pGrid->SetDefCellMargin(2);
	
	// ������
	CXScrollBar *pHBar = &m_XBarDealH;
	pHBar->Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, this, CRect(0,0,0,0), s_kiXScrollBarDealHID);
	
	CXScrollBar *pVBar = &m_XBarDealV;
	pVBar->Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, this, CRect(0,0,0,0), s_kiXScrollBarDealVID);
	
	pHBar->SetOwner(pGrid);
	pVBar->SetOwner(pGrid);

	pHBar->UseAppRes();
	pVBar->UseAppRes();

 	// ����ˮƽ������ͼƬ
 	pHBar->SetScrollBarLeftArrowH(-1);
 	pHBar->SetScrollBarRightArrowH(-1);
 	pHBar->SetScrollBarChannelH(IDB_BITMAP_TRADE_SCROLLBAR_CHANNEL);
 	pHBar->SetScrollBarThumbH(IDB_BITMAP_TRADE_HSCROLL);
 	pHBar->SetScrollBarThumbNoColorH(IDB_BITMAP_TRADE_HSCROLL);
	// ���ô�ֱ������ͼƬ
	pVBar->SetScrollBarLeftArrowH(-1);
	pVBar->SetScrollBarRightArrowH(-1);
	pVBar->SetScrollBarChannelH(IDB_BITMAP_TRADE_SCROLLBAR_CHANNEL);
	pVBar->SetScrollBarThumbH(IDB_BITMAP_TRADE_HSCROLL);
 	pVBar->SetScrollBarThumbNoColorH(IDB_BITMAP_TRADE_HSCROLL);

	pGrid->SetScrollBar(pHBar, pVBar);
	
	// ��ͷ
	const T_TradeGridHead *pstHeader = s_kstTradeGridHeadGroupDeal.pstGridHead;
	const int iHeaderCount = s_kstTradeGridHeadGroupDeal.iGridHeadCount;
	const int iFixedColCount = s_kstTradeGridHeadGroupDeal.iFixedColumnCount;
	COLORREF clrFixedRow = s_kstTradeGridHeadGroupDeal.clrFixedRow;
	if ( pstHeader!=NULL && iHeaderCount>0 )
	{
		pGrid->SetFixedRowCount(1);
		pGrid->SetColumnCount(iHeaderCount);
		pGrid->SetFixedColumnCount(iFixedColCount);
		
		for ( int iCol=0; iCol<iHeaderCount; ++iCol )
		{
			const T_TradeGridHead &stHeader = pstHeader[iCol];
			pGrid->SetColumnWidth(iCol, stHeader.iColWidth);
			
			CGridCellBase *pCell = pGrid->GetCell(0, iCol);
			ASSERT( pCell );
			pCell->SetText(stHeader.szName);
			pCell->SetFormat(stHeader.nFmt);
			pCell->SetData(stHeader.iHeadType);
			
			if ( CLR_DEFAULT!=clrFixedRow )
			{
				pCell->SetTextClr(clrFixedRow);
			}
		}
	}
	pGrid->AutoSizeRows();
	pGrid->SetRowHeight(0, GRID_HEAD_HEIGHT);
	pGrid->SetBorderColor(GRID_BORDER_COLOR);
	
	pGrid->BringWindowToTop();
}

void CDlgTradeQuery::CreateHoldDetailMainTable(CFont *pFont)	// �ֲ���ϸ
{
	CGridCtrlNormal *pGrid = &m_GridHoldDetailMain;
	CRect rcGrid(0,0,0,0);
	pGrid->Create(rcGrid, this, EGID_HOLDDETAILMAIN);
	pGrid->ShowWindow(SW_SHOW);
	
	// ��������
	pGrid->SetFont(pFont);
	pGrid->SetDonotScrollToNonExistCell(GVL_BOTH);
	pGrid->SetDefCellMargin(2);
	
	// ������ 
	pGrid->SetDefaultCellType(RUNTIME_CLASS(CGridCellTrade));
	pGrid->SetSingleRowSelection(TRUE);
	pGrid->SetListMode(TRUE);
	pGrid->EnableToolTips(FALSE);
	pGrid->EnableTitleTips(FALSE);
	pGrid->SetShowSelectWhenLoseFocus(FALSE);
	pGrid->SetGridBkColor(GRID_BK_COLOR);
	pGrid->SetFixColBkColor(GRID_BK_COLOR);
	pGrid->SetRemoveSelectClickBlank(TRUE);
	pGrid->SetListHeaderCanClick(FALSE);

	pGrid->GetDefaultCell(FALSE, FALSE)->SetBackClr(GRID_BK_COLOR);
	pGrid->GetDefaultCell(FALSE, FALSE)->SetTextClr(GRID_TEXT_COLOR);
	pGrid->GetDefaultCell(TRUE, FALSE)->SetBackClr(GRID_HEAD_COLOR);
	pGrid->GetDefaultCell(TRUE, FALSE)->SetTextClr(GRID_TEXT_COLOR);
	pGrid->ShowGridLine(false);
	pGrid->SetDrawSelectedCellStyle(GVSDS_DEFAULT);
	pGrid->SetDrawFixedCellGridLineAsNormal(TRUE);

	CXScrollBar *pHBar = &m_XBarHoldDetailMainH;
	pHBar->Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, this, CRect(0,0,0,0), s_kiXScrollBarHoldDetailMainHID);
	
	CXScrollBar *pVBar = &m_XBarHoldDetailMainV;
	pVBar->Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, this, CRect(0,0,0,0), s_kiXScrollBarHoldDetailMainVID);
	
	pHBar->SetOwner(pGrid);
	pVBar->SetOwner(pGrid);

	pHBar->UseAppRes();
	pVBar->UseAppRes();

 	// ����ˮƽ������ͼƬ
 	pHBar->SetScrollBarLeftArrowH(-1);
 	pHBar->SetScrollBarRightArrowH(-1);
 	pHBar->SetScrollBarChannelH(IDB_BITMAP_TRADE_SCROLLBAR_CHANNEL);
 	pHBar->SetScrollBarThumbH(IDB_BITMAP_TRADE_HSCROLL);
 	pHBar->SetScrollBarThumbNoColorH(IDB_BITMAP_TRADE_HSCROLL);
	// ���ô�ֱ������ͼƬ
	pVBar->SetScrollBarLeftArrowH(-1);
	pVBar->SetScrollBarRightArrowH(-1);
	pVBar->SetScrollBarChannelH(IDB_BITMAP_TRADE_SCROLLBAR_CHANNEL);
	pVBar->SetScrollBarThumbH(IDB_BITMAP_TRADE_HSCROLL);
 	pVBar->SetScrollBarThumbNoColorH(IDB_BITMAP_TRADE_HSCROLL);

	pGrid->SetScrollBar(pHBar, pVBar);
	
	// ��ͷ
	const T_TradeGridHead *pstHeader = s_kstTradeGridHeadGroupHoldDetailMain.pstGridHead;
	const int iHeaderCount = s_kstTradeGridHeadGroupHoldDetailMain.iGridHeadCount;
	const int iFixedColCount = s_kstTradeGridHeadGroupHoldDetailMain.iFixedColumnCount;
	COLORREF clrFixedRow = s_kstTradeGridHeadGroupHoldDetailMain.clrFixedRow;
	if ( pstHeader!=NULL && iHeaderCount>0 )
	{
		pGrid->SetFixedRowCount(1);
		pGrid->SetColumnCount(iHeaderCount);
		pGrid->SetFixedColumnCount(iFixedColCount);
		pGrid->SetRowHeight(0, 50);
		
		for ( int iCol=0; iCol<iHeaderCount; ++iCol )
		{
			const T_TradeGridHead &stHeader = pstHeader[iCol];
			pGrid->SetColumnWidth(iCol, stHeader.iColWidth);
			
			CGridCellBase *pCell = pGrid->GetCell(0, iCol);
			ASSERT( pCell );
			pCell->SetText(stHeader.szName);
			pCell->SetFormat(stHeader.nFmt);
			pCell->SetData(stHeader.iHeadType);

			if ( CLR_DEFAULT!=clrFixedRow )
			{
				pCell->SetTextClr(clrFixedRow);
			}
		}
	}
	pGrid->AutoSizeRows();
	pGrid->SetRowHeight(0, GRID_HEAD_HEIGHT);
	pGrid->SetBorderColor(GRID_BORDER_COLOR);
	
	pGrid->BringWindowToTop();
}

void CDlgTradeQuery::CreateHoldSummaryMainTable(CFont *pFont)	// �ֲֻ���
{
	CGridCtrlNormal *pGrid = &m_GridHoldSummary;
	CRect rcGrid(0,0,0,0);
	pGrid->Create(rcGrid, this, EGID_HOLDTOTAL);
	pGrid->ShowWindow(SW_SHOW);

	// ��������
	pGrid->SetFont(pFont);
	pGrid->SetDonotScrollToNonExistCell(GVL_BOTH);
	pGrid->SetDefCellMargin(2);

	// ������ 
	pGrid->SetDefaultCellType(RUNTIME_CLASS(CGridCellTrade));
	pGrid->SetSingleRowSelection(TRUE);
	pGrid->SetListMode(TRUE);
	pGrid->EnableToolTips(FALSE);
	pGrid->EnableTitleTips(FALSE);
	pGrid->SetShowSelectWhenLoseFocus(FALSE);
	pGrid->SetGridBkColor(GRID_BK_COLOR);
	pGrid->SetFixColBkColor(GRID_BK_COLOR);
	pGrid->SetRemoveSelectClickBlank(TRUE);
	pGrid->SetListHeaderCanClick(FALSE);

	pGrid->GetDefaultCell(FALSE, FALSE)->SetBackClr(GRID_BK_COLOR);
	pGrid->GetDefaultCell(FALSE, FALSE)->SetTextClr(GRID_TEXT_COLOR);
	pGrid->GetDefaultCell(TRUE, FALSE)->SetBackClr(GRID_HEAD_COLOR);
	pGrid->GetDefaultCell(TRUE, FALSE)->SetTextClr(GRID_TEXT_COLOR);
	pGrid->ShowGridLine(false);
	pGrid->SetDrawSelectedCellStyle(GVSDS_DEFAULT);
	pGrid->SetDrawFixedCellGridLineAsNormal(TRUE);

	CXScrollBar *pHBar = &m_XBarHoldTotalH;
	pHBar->Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, this, CRect(0,0,0,0), s_kiXScrollBarHoldTotalHID);

	CXScrollBar *pVBar = &m_XBarHoldTotalV;
	pVBar->Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, this, CRect(0,0,0,0), s_kiXScrollBarHoldTotalVID);

	pHBar->SetOwner(pGrid);
	pVBar->SetOwner(pGrid);

	pHBar->UseAppRes();
	pVBar->UseAppRes();

	// ����ˮƽ������ͼƬ
	pHBar->SetScrollBarLeftArrowH(-1);
	pHBar->SetScrollBarRightArrowH(-1);
	pHBar->SetScrollBarChannelH(IDB_BITMAP_TRADE_SCROLLBAR_CHANNEL);
	pHBar->SetScrollBarThumbH(IDB_BITMAP_TRADE_HSCROLL);
	pHBar->SetScrollBarThumbNoColorH(IDB_BITMAP_TRADE_HSCROLL);
	// ���ô�ֱ������ͼƬ
	pVBar->SetScrollBarLeftArrowH(-1);
	pVBar->SetScrollBarRightArrowH(-1);
	pVBar->SetScrollBarChannelH(IDB_BITMAP_TRADE_SCROLLBAR_CHANNEL);
	pVBar->SetScrollBarThumbH(IDB_BITMAP_TRADE_HSCROLL);
	pVBar->SetScrollBarThumbNoColorH(IDB_BITMAP_TRADE_HSCROLL);

	pGrid->SetScrollBar(pHBar, pVBar);

	// ��ͷ
	const T_TradeGridHead *pstHeader = s_kstTradeGridHeadGroupHoldSummary.pstGridHead;
	const int iHeaderCount = s_kstTradeGridHeadGroupHoldSummary.iGridHeadCount;
	const int iFixedColCount = s_kstTradeGridHeadGroupHoldSummary.iFixedColumnCount;
	COLORREF clrFixedRow = s_kstTradeGridHeadGroupHoldSummary.clrFixedRow;
	if ( pstHeader!=NULL && iHeaderCount>0 )
	{
		pGrid->SetFixedRowCount(1);
		pGrid->SetColumnCount(iHeaderCount);
		pGrid->SetFixedColumnCount(iFixedColCount);
		pGrid->SetRowHeight(0, 50);

		for ( int iCol=0; iCol<iHeaderCount; ++iCol )
		{
			const T_TradeGridHead &stHeader = pstHeader[iCol];
			pGrid->SetColumnWidth(iCol, stHeader.iColWidth);

			CGridCellBase *pCell = pGrid->GetCell(0, iCol);
			ASSERT( pCell );
			pCell->SetText(stHeader.szName);
			pCell->SetFormat(stHeader.nFmt);
			pCell->SetData(stHeader.iHeadType);

			if ( CLR_DEFAULT!=clrFixedRow )
			{
				pCell->SetTextClr(clrFixedRow);
			}
		}
	}
	pGrid->AutoSizeRows();
	pGrid->SetRowHeight(0, GRID_HEAD_HEIGHT);
	pGrid->SetBorderColor(GRID_BORDER_COLOR);

	pGrid->BringWindowToTop();
}

void CDlgTradeQuery::CreateUserInfoMainTable(CFont *pFont)// �˻���Ϣ
{
	CGridCtrlNormal *pGrid = &m_GridAccInfoMain;
	CRect rcGrid(0,0,0,0);
	pGrid->Create(rcGrid, this, EGID_USERINFOMAIN);
	pGrid->ShowWindow(SW_SHOW);
	
	pGrid->SetDonotScrollToNonExistCell(GVL_BOTH);
	pGrid->SetDefaultCellType(RUNTIME_CLASS(CGridCellTrade));
	pGrid->SetSingleRowSelection(TRUE);
	pGrid->SetListMode(TRUE);
	pGrid->EnableToolTips(FALSE);
	pGrid->EnableTitleTips(FALSE);
	pGrid->SetShowSelectWhenLoseFocus(FALSE);
	pGrid->SetGridBkColor(GRID_BK_COLOR);
	pGrid->SetFixColBkColor(GRID_BK_COLOR);
	pGrid->SetRemoveSelectClickBlank(TRUE);
	pGrid->SetListHeaderCanClick(FALSE);

	pGrid->GetDefaultCell(FALSE, FALSE)->SetBackClr(GRID_BK_COLOR);
	pGrid->GetDefaultCell(FALSE, FALSE)->SetTextClr(GRID_TEXT_COLOR);
	pGrid->GetDefaultCell(TRUE, FALSE)->SetBackClr(GRID_HEAD_COLOR);
	pGrid->GetDefaultCell(TRUE, FALSE)->SetTextClr(GRID_TEXT_COLOR);
	pGrid->ShowGridLine(false);
	pGrid->SetDrawSelectedCellStyle(GVSDS_DEFAULT);
	pGrid->SetDrawFixedCellGridLineAsNormal(TRUE);
	
	// ��������
	pGrid->SetFont(pFont);
	// 		pGrid->GetDefaultCell(FALSE, FALSE)->SetFont(&lgFont);
	// 		pGrid->GetDefaultCell(FALSE, TRUE)->SetFont(&lgFont);
	// 		pGrid->GetDefaultCell(TRUE, FALSE)->SetFont(&lgFont);
	// 		pGrid->GetDefaultCell(TRUE, TRUE)->SetFont(&lgFont);
	
	pGrid->SetDefCellMargin(2);
	
	// ������
	CXScrollBar *pHBar = &m_XBarAccInfoMainH;
	pHBar->Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY, this, CRect(0,0,0,0), s_kiXScrollBarAccInfoMainHID);
	
	CXScrollBar *pVBar = &m_XBarAccInfoMainV;
	pVBar->Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, this, CRect(0,0,0,0), s_kiXScrollBarAccInfoMainVID);
	
//	pHBar->SetOwner(pGrid);
	pVBar->SetOwner(pGrid);

	pHBar->UseAppRes();
	pVBar->UseAppRes();

// 	// ����ˮƽ������ͼƬ
// 	pHBar->SetScrollBarLeftArrowH(IDB_HORIZONTAL_SCROLLBAR_LEFTARROW_BACAI);
// 	pHBar->SetScrollBarRightArrowH(IDB_HORIZONTAL_SCROLLBAR_RIGHTARROW_BACAI);
// 	pHBar->SetScrollBarChannelH(IDB_BITMAP_TRADE_SCROLLBAR_CHANNEL);
// 	pHBar->SetScrollBarThumbH(IDB_HORIZONTAL_SCROLLBAR_THUMB_BACAI);
// 	pHBar->SetScrollBarThumbNoColorH(IDB_HORIZONTAL_SCROLLBAR_THUMB_NO_COLOR_BACAI);
	// ���ô�ֱ������ͼƬ
	pVBar->SetScrollBarLeftArrowH(-1);
	pVBar->SetScrollBarRightArrowH(-1);
	pVBar->SetScrollBarChannelH(IDB_BITMAP_TRADE_SCROLLBAR_CHANNEL);
	pVBar->SetScrollBarThumbH(IDB_BITMAP_TRADE_HSCROLL);
 	pVBar->SetScrollBarThumbNoColorH(IDB_BITMAP_TRADE_HSCROLL);

	pGrid->SetScrollBar(NULL, pVBar);
	
	// ��ͷ
	const T_TradeGridHead *pstHeader = s_kstTradeGridHeadGroupAccInfoMain.pstGridHead;
	const int iHeaderCount = s_kstTradeGridHeadGroupAccInfoMain.iGridHeadCount;
	const int iFixedColCount = s_kstTradeGridHeadGroupAccInfoMain.iFixedColumnCount;
	COLORREF clrFixedRow = s_kstTradeGridHeadGroupAccInfoMain.clrFixedRow;
	if ( pstHeader!=NULL && iHeaderCount>0 )
	{
		pGrid->SetFixedRowCount(1);
		pGrid->SetColumnCount(iHeaderCount);
		pGrid->SetFixedColumnCount(iFixedColCount);
		
		for ( int iCol=0; iCol<iHeaderCount; ++iCol )
		{
			const T_TradeGridHead &stHeader = pstHeader[iCol];
			pGrid->SetColumnWidth(iCol, stHeader.iColWidth);
			
			CGridCellBase *pCell = pGrid->GetCell(0, iCol);
			ASSERT( pCell );
			pCell->SetText(stHeader.szName);
			pCell->SetFormat(stHeader.nFmt);
			pCell->SetData(stHeader.iHeadType);
			
			if ( CLR_DEFAULT!=clrFixedRow )
			{
				pCell->SetTextClr(clrFixedRow);
			}
		}
	}
	pGrid->AutoSizeRows();
	pGrid->SetRowHeight(0, GRID_HEAD_HEIGHT);
	pGrid->SetBorderColor(GRID_BORDER_COLOR);
	
	pGrid->BringWindowToTop();
}

void CDlgTradeQuery::CreateCommInfoTable(CFont *pFont)// ��Ʒ��Ϣ
{
	CGridCtrlNormal *pGrid = &m_GridCommInfo;
	CRect rcGrid(0,0,0,0);
	pGrid->Create(rcGrid, this, EGID_COMMINFO);
	pGrid->ShowWindow(SW_SHOW);
	
	pGrid->SetDonotScrollToNonExistCell(GVL_BOTH);
	pGrid->SetDefaultCellType(RUNTIME_CLASS(CGridCellTrade));
	pGrid->SetSingleRowSelection(TRUE);
	pGrid->SetListMode(TRUE);
	pGrid->EnableToolTips(FALSE);
	pGrid->EnableTitleTips(FALSE);
	pGrid->SetShowSelectWhenLoseFocus(FALSE);
	pGrid->SetGridBkColor(GRID_BK_COLOR);
	pGrid->SetFixColBkColor(GRID_BK_COLOR);
	pGrid->SetRemoveSelectClickBlank(TRUE);
	pGrid->SetListHeaderCanClick(FALSE);

	pGrid->GetDefaultCell(FALSE, FALSE)->SetBackClr(GRID_BK_COLOR);
	pGrid->GetDefaultCell(FALSE, FALSE)->SetTextClr(GRID_TEXT_COLOR);
	pGrid->GetDefaultCell(TRUE, FALSE)->SetBackClr(GRID_HEAD_COLOR);
	pGrid->GetDefaultCell(TRUE, FALSE)->SetTextClr(GRID_TEXT_COLOR);
	pGrid->ShowGridLine(false);
	pGrid->SetDrawSelectedCellStyle(GVSDS_DEFAULT);
	pGrid->SetDrawFixedCellGridLineAsNormal(TRUE);
	
	// ��������
	pGrid->SetFont(pFont);
	// 		pGrid->GetDefaultCell(FALSE, FALSE)->SetFont(&lgFont);
	// 		pGrid->GetDefaultCell(FALSE, TRUE)->SetFont(&lgFont);
	// 		pGrid->GetDefaultCell(TRUE, FALSE)->SetFont(&lgFont);
	// 		pGrid->GetDefaultCell(TRUE, TRUE)->SetFont(&lgFont);
	
	pGrid->SetDefCellMargin(0);
	
	// ������
	CXScrollBar *pHBar = &m_XBarCommInfoH;
	pHBar->Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY, this, CRect(0,0,0,0), s_kiXScrollBarCommInfoHID);
	
	CXScrollBar *pVBar = &m_XBarCommInfoV;
	pVBar->Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, this, CRect(0,0,0,0), s_kiXScrollBarCommInfoVID);
	
//	pHBar->SetOwner(pGrid);
	pVBar->SetOwner(pGrid);

	pHBar->UseAppRes();
	pVBar->UseAppRes();

	// ���ô�ֱ������ͼƬ
	pVBar->SetScrollBarLeftArrowH(-1);
	pVBar->SetScrollBarRightArrowH(-1);
	pVBar->SetScrollBarChannelH(IDB_BITMAP_TRADE_SCROLLBAR_CHANNEL);
	pVBar->SetScrollBarThumbH(IDB_BITMAP_TRADE_HSCROLL);
 	pVBar->SetScrollBarThumbNoColorH(IDB_BITMAP_TRADE_HSCROLL);

	pGrid->SetScrollBar(NULL, pVBar);
	
	// ��ͷ
	const T_TradeGridHead *pstHeader = s_kstTradeGridHeadGroupCommInfo.pstGridHead;
	const int iHeaderCount = s_kstTradeGridHeadGroupCommInfo.iGridHeadCount;
	const int iFixedColCount = s_kstTradeGridHeadGroupCommInfo.iFixedColumnCount;
	COLORREF clrFixedRow = s_kstTradeGridHeadGroupCommInfo.clrFixedRow;
	if ( pstHeader!=NULL && iHeaderCount>0 )
	{
		pGrid->SetFixedRowCount(1);
		pGrid->SetColumnCount(iHeaderCount);
		pGrid->SetFixedColumnCount(iFixedColCount);
		
		for ( int iCol=0; iCol<iHeaderCount; ++iCol )
		{
			const T_TradeGridHead &stHeader = pstHeader[iCol];
			pGrid->SetColumnWidth(iCol, stHeader.iColWidth);
			
			CGridCellBase *pCell = pGrid->GetCell(0, iCol);
			ASSERT( pCell );
			pCell->SetText(stHeader.szName);
			pCell->SetFormat(stHeader.nFmt);
			pCell->SetData(stHeader.iHeadType);
			
			if ( CLR_DEFAULT!=clrFixedRow )
			{
				pCell->SetTextClr(clrFixedRow);
			}
		}
	}
	pGrid->AutoSizeRows();
	pGrid->SetRowHeight(0, GRID_HEAD_HEIGHT);
	pGrid->SetBorderColor(GRID_BORDER_COLOR);
	
	pGrid->BringWindowToTop();
}

void CDlgTradeQuery::CreateEntrustTable(CFont *pFont) // ����ί�б�񣨸ſ���
{
	CGridCtrlNormal *pGrid = &m_GridEntrust;
	CRect rcGrid(0,0,0,0);
	pGrid->Create(rcGrid, this, EGID_ENTRUST);
	pGrid->ShowWindow(SW_SHOW);
	
	pGrid->SetDonotScrollToNonExistCell(GVL_BOTH);
	pGrid->SetDefaultCellType(RUNTIME_CLASS(CGridCellTrade));
	pGrid->SetSingleRowSelection(TRUE);
	pGrid->SetListMode(TRUE);
	pGrid->EnableToolTips(FALSE);
	pGrid->EnableTitleTips(FALSE);
	pGrid->SetShowSelectWhenLoseFocus(FALSE);
	pGrid->SetGridBkColor(GRID_BK_COLOR);
	pGrid->SetFixColBkColor(GRID_BK_COLOR);
	pGrid->SetRemoveSelectClickBlank(TRUE);
	pGrid->SetListHeaderCanClick(FALSE);

	pGrid->GetDefaultCell(FALSE, FALSE)->SetBackClr(GRID_BK_COLOR);
	pGrid->GetDefaultCell(FALSE, FALSE)->SetTextClr(GRID_TEXT_COLOR);
	pGrid->GetDefaultCell(TRUE, FALSE)->SetBackClr(GRID_HEAD_COLOR);
	pGrid->GetDefaultCell(TRUE, FALSE)->SetTextClr(GRID_TEXT_COLOR);
	pGrid->ShowGridLine(false);
	pGrid->SetDrawSelectedCellStyle(GVSDS_DEFAULT);
	pGrid->SetDrawFixedCellGridLineAsNormal(TRUE);

	CRect rt(4,5,265,25);	
	m_staticDisEntrust.Create(_T("��ʾ��˫�����Ҽ�ѡ����Ҳ���Խ��г�������"), WS_CHILD|WS_VISIBLE,rt,this, IDC_STATIC_DISENTRUST);
	m_staticDisEntrust.SetFont(pFont);
	
	// ��������
	pGrid->SetFont(pFont);
	// 		pGrid->GetDefaultCell(FALSE, FALSE)->SetFont(&lgFont);
	// 		pGrid->GetDefaultCell(FALSE, TRUE)->SetFont(&lgFont);
	// 		pGrid->GetDefaultCell(TRUE, FALSE)->SetFont(&lgFont);
	// 		pGrid->GetDefaultCell(TRUE, TRUE)->SetFont(&lgFont);
	
	pGrid->SetDefCellMargin(2);
	
	// ������
	CXScrollBar *pHBar = &m_XBarEntrustH;
	pHBar->Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, this, CRect(0,0,0,0), s_kiXScrollBarEntrustHID);
	
	CXScrollBar *pVBar = &m_XBarEntrustV;
	pVBar->Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, this, CRect(0,0,0,0), s_kiXScrollBarEntrustVID);
	
	pHBar->SetOwner(pGrid);
	pVBar->SetOwner(pGrid);

	pHBar->UseAppRes();
	pVBar->UseAppRes();

	// ����ˮƽ������ͼƬ
	pHBar->SetScrollBarLeftArrowH(-1);
	pHBar->SetScrollBarRightArrowH(-1);
	pHBar->SetScrollBarChannelH(IDB_BITMAP_TRADE_SCROLLBAR_CHANNEL);
	pHBar->SetScrollBarThumbH(IDB_BITMAP_TRADE_HSCROLL);
	pHBar->SetScrollBarThumbNoColorH(IDB_BITMAP_TRADE_HSCROLL);
	// ���ô�ֱ������ͼƬ
	pVBar->SetScrollBarLeftArrowH(-1);
	pVBar->SetScrollBarRightArrowH(-1);
	pVBar->SetScrollBarChannelH(IDB_BITMAP_TRADE_SCROLLBAR_CHANNEL);
	pVBar->SetScrollBarThumbH(IDB_BITMAP_TRADE_HSCROLL);
	pVBar->SetScrollBarThumbNoColorH(IDB_BITMAP_TRADE_HSCROLL);
	
	pGrid->SetScrollBar(pHBar, pVBar);
	
	// ��ͷ
	const T_TradeGridHead *pstHeader = s_kstTradeGridHeadGroupEntrust.pstGridHead;
	const int iHeaderCount = s_kstTradeGridHeadGroupEntrust.iGridHeadCount;
	const int iFixedColCount = s_kstTradeGridHeadGroupEntrust.iFixedColumnCount;
	COLORREF clrFixedRow = s_kstTradeGridHeadGroupEntrust.clrFixedRow;
	if ( pstHeader!=NULL && iHeaderCount>0 )
	{
		pGrid->SetFixedRowCount(1);
		pGrid->SetColumnCount(iHeaderCount);
		pGrid->SetFixedColumnCount(iFixedColCount);
		
		for ( int iCol=0; iCol<iHeaderCount; ++iCol )
		{
			const T_TradeGridHead &stHeader = pstHeader[iCol];
			pGrid->SetColumnWidth(iCol, stHeader.iColWidth);
			
			CGridCellBase *pCell = pGrid->GetCell(0, iCol);
			ASSERT( pCell );
			pCell->SetText(stHeader.szName);
			pCell->SetFormat(stHeader.nFmt);
			pCell->SetData(stHeader.iHeadType);
			
			if ( CLR_DEFAULT!=clrFixedRow )
			{
				pCell->SetTextClr(clrFixedRow);
			}
		}
	}
	pGrid->AutoSizeRows();
	pGrid->SetRowHeight(0, GRID_HEAD_HEIGHT);
	pGrid->SetBorderColor(GRID_BORDER_COLOR);
	pGrid->BringWindowToTop();

	
}

void CDlgTradeQuery::ShowHideLimitEntrust(int nCmdShow)	// ��ʾ/���ظſ�
{
	m_GridEntrust.ShowWindow(nCmdShow);
	m_XBarEntrustH.ShowWindow(nCmdShow);
	m_XBarEntrustV.ShowWindow(nCmdShow);
	m_staticDisEntrust.ShowWindow(nCmdShow);
	
	if (nCmdShow==SW_SHOW)
	{		
		m_GridEntrust.BringWindowToTop();
		m_XBarEntrustH.BringWindowToTop();
		m_XBarEntrustV.BringWindowToTop();
	}
}

void CDlgTradeQuery::ShowHideGeneral(int nCmdShow)	// ��ʾ/���ظſ�
{
// 	m_GroupHoldDetail.ShowWindow(nCmdShow);
// 	m_StaticHoldDetail.ShowWindow(nCmdShow);
// 	m_GroupEntrust.ShowWindow(nCmdShow);
// 	m_StaticEntrust.ShowWindow(nCmdShow);
// 	m_GroupAccInfo.ShowWindow(nCmdShow);
// 	m_StaticAccInfo.ShowWindow(nCmdShow);
// 
// 	m_GridHoldDetail.ShowWindow(nCmdShow);
// 	m_XBarHoldDetailH.ShowWindow(nCmdShow);
// 	m_XBarHoldDetailV.ShowWindow(nCmdShow);
// 
// 	m_GridEntrust.ShowWindow(nCmdShow);
// 	m_XBarEntrustH.ShowWindow(nCmdShow);
// 	m_XBarEntrustV.ShowWindow(nCmdShow);
// 	
// 	m_GridAccInfo.ShowWindow(nCmdShow);
// 	m_XBarAccInfoH.ShowWindow(nCmdShow);
// 	m_XBarAccInfoV.ShowWindow(nCmdShow);
// 
// 	if (nCmdShow==SW_SHOW)
// 	{
// 		m_GridHoldDetail.BringWindowToTop();
// 		m_XBarHoldDetailH.BringWindowToTop();
// 		m_XBarHoldDetailV.BringWindowToTop();
// 		
// 		m_GridEntrust.BringWindowToTop();
// 		m_XBarEntrustH.BringWindowToTop();
// 		m_XBarEntrustV.BringWindowToTop();
// 		
// 		m_GridAccInfo.BringWindowToTop();
// 		m_XBarAccInfoH.BringWindowToTop();
// 		m_XBarAccInfoV.BringWindowToTop();
// 	}
}

// ��ʾ/���ظſ�
void CDlgTradeQuery::ShowHideEntrustMain(int nCmdShow)
{
	m_GroupEntrustMain.ShowWindow(nCmdShow);
	m_StaticEntrustMain.ShowWindow(nCmdShow);
	m_StaticEntrustMainCommInfo.ShowWindow(nCmdShow);
	m_DropEntrustMainCommInfo.ShowWindow(nCmdShow);
	m_StaticEntrustMainType.ShowWindow(nCmdShow);
	m_DropEntrustMainType.ShowWindow(nCmdShow);
	m_StaticEntrustMainStatus.ShowWindow(nCmdShow);
	m_DropEntrustMainStatus.ShowWindow(nCmdShow);
	m_BtnEntrustMainQuery.ShowWindow(nCmdShow);
	m_BtnEntrustMainReset.ShowWindow(nCmdShow);
	m_StaticEntrustMainBuySell.ShowWindow(nCmdShow);
	m_DropEntrustMainBuySell.ShowWindow(nCmdShow);
	m_StaticEntrustMainOpenClose.ShowWindow(nCmdShow);
	m_DropEntrustMainOpenClose.ShowWindow(nCmdShow);
	
	m_GridEntrustMain.ShowWindow(nCmdShow);
	m_XBarEntrustMainH.ShowWindow(nCmdShow);
	m_XBarEntrustMainV.ShowWindow(nCmdShow);
	
	if (nCmdShow==SW_SHOW)
	{
		m_GridEntrustMain.BringWindowToTop();
		m_XBarEntrustMainH.BringWindowToTop();
		m_XBarEntrustMainV.BringWindowToTop();
	}
}

// ��ʾ/���سɽ�
void CDlgTradeQuery::ShowHideDeal(int nCmdShow)
{
	m_StaticDealCommInfo.ShowWindow(nCmdShow);
	m_DropDealCommInfo.ShowWindow(nCmdShow);
	m_StaticDealBuySell.ShowWindow(nCmdShow);
	m_DropDealBuySell.ShowWindow(nCmdShow);
	m_StaticDealOpenClose.ShowWindow(nCmdShow);
	m_DropDealOpenClose.ShowWindow(nCmdShow);
	m_BtnDealQuery.ShowWindow(nCmdShow);
	m_BtnDealReset.ShowWindow(nCmdShow);

	m_GridDeal.ShowWindow(nCmdShow);
	m_XBarDealH.ShowWindow(nCmdShow);
	m_XBarDealV.ShowWindow(nCmdShow);
	
	if (nCmdShow==SW_SHOW)
	{
		m_GridDeal.BringWindowToTop();
		m_XBarDealH.BringWindowToTop();
		m_XBarDealV.BringWindowToTop();
	}
}

// ��ʾ/���� �ֲ���ϸ
void CDlgTradeQuery::ShowHideHoldDetailMain(int nCmdShow)
{
	m_GridHoldDetailMain.ShowWindow(nCmdShow);
	m_XBarHoldDetailMainH.ShowWindow(nCmdShow);
	m_XBarHoldDetailMainV.ShowWindow(nCmdShow);
	
	if (nCmdShow==SW_SHOW)
	{
		m_GridHoldDetailMain.BringWindowToTop();
		m_XBarHoldDetailMainH.BringWindowToTop();
		m_XBarHoldDetailMainV.BringWindowToTop();
	}
}

// ��ʾ/���� �ֲֻ���
void CDlgTradeQuery::ShowHideHoldSummaryMain(int nCmdShow)
{
	m_GridHoldSummary.ShowWindow(nCmdShow);
	m_XBarHoldTotalH.ShowWindow(nCmdShow);
	m_XBarHoldTotalV.ShowWindow(nCmdShow);

	if (nCmdShow==SW_SHOW)
	{
		m_GridHoldSummary.BringWindowToTop();
		m_XBarHoldTotalH.BringWindowToTop();
		m_XBarHoldTotalV.BringWindowToTop();
	}
}

// ��ʾ/���� �˻���Ϣ
void CDlgTradeQuery::ShowHideAccInfoMain(int nCmdShow)
{
	m_GridAccInfoMain.ShowWindow(nCmdShow);
	m_XBarAccInfoMainH.ShowWindow(nCmdShow);
	m_XBarAccInfoMainV.ShowWindow(nCmdShow);
	
	if (nCmdShow==SW_SHOW)
	{
		m_GridAccInfoMain.BringWindowToTop();
		m_XBarAccInfoMainH.BringWindowToTop();
		m_XBarAccInfoMainV.BringWindowToTop();
	}	
}

// ��ʾ/���� ��Ʒ��Ϣ
void CDlgTradeQuery::ShowHideCommInfo(int nCmdShow)
{
	m_StaticCommInfoType.ShowWindow(nCmdShow);
	m_DropCommInfoType.ShowWindow(nCmdShow);

	m_GroupCommInfo.ShowWindow(nCmdShow);
	m_StaticCommInfo.ShowWindow(nCmdShow);
	
	m_GridCommInfo.ShowWindow(nCmdShow);
	m_XBarCommInfoH.ShowWindow(nCmdShow);
	m_XBarCommInfoV.ShowWindow(nCmdShow);
	
	if (nCmdShow==SW_SHOW)
	{
		m_GridCommInfo.BringWindowToTop();
		m_XBarCommInfoH.BringWindowToTop();
		m_XBarCommInfoV.BringWindowToTop();
	}	
}


void CDlgTradeQuery::ShowHideHome(int nCmdShow)
{
	ShowHideHoldDetailMain(nCmdShow);
}

void CDlgTradeQuery::RefreshList()
{
	QueryListData(m_eCurQueryType);
}

void CDlgTradeQuery::SetTradeStatus(E_TradeLoginStatus status)
{
	m_eTradeStatus = status;
}

void CDlgTradeQuery::QueryListData(E_QueryType eQueryType)
{
	if (ETLS_Logined != m_eTradeStatus)	// ���ǵ�½״̬����������
	{
		return;
	}

	if (eQueryType == EQT_QueryHome)
	{
		DoQueryAsy(EQT_QueryHoldDetail);
	}
	else if (eQueryType == EQT_QueryCommInfo)
	{
		QueryCommInfo();
	}
	else
	{
		DoQueryAsy(eQueryType);
	}
	
// 	if (2==iCurSel)//�˻���Ϣ
// 	{
// 		DoQueryAsy(EQT_QueryUserInfo);
// 	}
// 	else if (0==iCurSel)	// �ֲ���ϸ
// 	{
// 		DoQueryAsy(EQT_QueryHoldDetail);
// 	//	DoQueryAsy(EQT_QuerySystemStatus);//��ֲ���ϸʱ���ϵͳ״̬
// 		if (iCurSel==m_wndTab.GetCurSel())//��ǰû��ʾ���б�Ҫˢ��
// 		{
// 			IsCanClose(iCurSel);	//֪ͨƽ�ְ�ť״̬�仯	return;
// 		}
// 	}
// 	else if (1==iCurSel) // ָ��ί��
// 	{
// 		//QueryEntrust();
// 		DoQueryAsy(EQT_QueryLimitEntrust);
// 		IncDTPNeedProcessFlag();
// 		
// 	//	CExportMT2DispatchThread thread = CExportMT2DispatchThread::Instance();
// 		
// 		if (!CExportMT2DispatchThread::Instance().InitInstance() )
// 		{
// 			DecDTPNeedProcessFlag();
// 		}
// 		T_ExportMT2DispatchThreadProcessorLParam *pParam = new T_ExportMT2DispatchThreadProcessorLParam;
// 		pParam->iEventId = EQT_QueryEntrust;
// 		if ( !CExportMT2DispatchThread::Instance().Post2DispatchThread(this, (LPARAM)pParam) )
// 		{
// 			delete pParam;
// 			pParam = NULL;
// 			DecDTPNeedProcessFlag();
// 		}
//	}
// 	else if (3==iCurSel) // �˻���Ϣ
// 	{
// 		//	QueryDeal();
// 		DoQueryAsy(EQT_QueryAccInfo);
// 		IncDTPNeedProcessFlag();
// 		
// 	//	CExportMT2DispatchThread thread = CExportMT2DispatchThread::Instance();
// 		
// 		if (!CExportMT2DispatchThread::Instance().InitInstance() )
// 		{
// 			DecDTPNeedProcessFlag();
// 		}
// 		T_ExportMT2DispatchThreadProcessorLParam *pParam = new T_ExportMT2DispatchThreadProcessorLParam;
// 		pParam->iEventId = EQT_QueryDeal;
// 		if ( !CExportMT2DispatchThread::Instance().Post2DispatchThread(this, (LPARAM)pParam) )
// 		{
// 			delete pParam;
// 			pParam = NULL;
// 			DecDTPNeedProcessFlag();
// 		}
//	}
//	else if (1==iCurSel) // ί�в�ѯ
//	{
		//QueryHoldDetail();
//		DoQueryAsy(EQT_QueryEntrust);
// 		IncDTPNeedProcessFlag();
// 		
// 		if (!CExportMT2DispatchThread::Instance().InitInstance() )
// 		{
// 			DecDTPNeedProcessFlag();
// 		}
// 		T_ExportMT2DispatchThreadProcessorLParam *pParam = new T_ExportMT2DispatchThreadProcessorLParam;
// 		pParam->iEventId = EQT_QueryHoldDetail;
// 		if ( !CExportMT2DispatchThread::Instance().Post2DispatchThread(this, (LPARAM)pParam) )
// 		{
// 			delete pParam;
// 			pParam = NULL;
// 			DecDTPNeedProcessFlag();
// 		}
//	}
// 	else if (4==iCurSel) // �ֲֻ���
// 	{
// 		//QueryHoldTotal();
// 		DoQueryAsy(EQT_QueryHoldTotal);
// 	}
// 	else if (3==iCurSel) // �ɽ���ѯ
// 	{
// 		//QueryAccInfo();	// �˻���Ϣ
// 		DoQueryAsy(EQT_QueryDeal);
// 	}
// 	else if (4==iCurSel) // ��Ʒ��Ϣ
// 	{
// 		QueryCommInfo();
// 		//DoQueryAsy(EQT_QueryCommInfo);
// 	}
// 	else
// 	{
// 		//ASSERT(0);
// 	}
}

void CDlgTradeQuery::GetRiseFallColor( COLORREF &clrRise, COLORREF &clrFall, COLORREF &clrKeep )
{
	clrRise = RGB(255, 0, 0);
	clrFall = RGB(0, 128, 0);
	clrKeep = RGB(0, 0, 0);
}

bool CDlgTradeQuery::FindCommInfoType( int iIndex, T_CommInfoType &commInfoType)
{
	bool bFind = false;
	
	m_LockCommInfoType.lock();
	_CommInfoType::iterator it = m_CommInfoType.find( iIndex );
	if ( it!=m_CommInfoType.end() )
	{
		commInfoType = it->second;
		bFind = true;
	}
	m_LockCommInfoType.unlock();
	
	return bFind;
}

// ��ȡ����
int CDlgTradeQuery::GetCommInfoTypeSize()
{
	return m_CommInfoType.size();		
}

char* CDlgTradeQuery::GetCommInfoTypeName(int iIndex)
{
	m_LockCommInfoType.lock();
	_CommInfoType::iterator it = m_CommInfoType.find( iIndex );
	if ( it!=m_CommInfoType.end() )
	{
		//strcpy(pName,it->second.chTypeName);
	}
	m_LockCommInfoType.unlock();

	return it->second.chTypeName;
}

bool CDlgTradeQuery::FindOtherFirm( int iIndex, T_CommInfoType &commInfoType)
{
	bool bFind = false;
	
	m_LockOtherFirm.lock();
	_CommInfoType::iterator it = m_OtherFirm.find( iIndex );
	if ( it!=m_OtherFirm.end() )
	{
		commInfoType = it->second;
		bFind = true;
	}
	m_LockOtherFirm.unlock();
	
	return bFind;
}

// ��ȡ����
int CDlgTradeQuery::GetOtherFirmSize()
{
	return m_OtherFirm.size();		
}

char* CDlgTradeQuery::GetOtherFirmName(int iIndex)
{
	m_LockOtherFirm.lock();
	_CommInfoType::iterator it = m_OtherFirm.find( iIndex );
	if ( it!=m_OtherFirm.end() )
	{
		//strcpy(pName,it->second.chTypeName);
	}
	m_LockOtherFirm.unlock();

	return it->second.chTypeName;
}

// ȡ��Ʒ����
/*const char**/CString CDlgTradeQuery::GetCommName(const char* pCommID)
{
	if (NULL==pCommID||NULL==m_pTradeBid)
	{
		return L"";
	}

	const QueryCommInfoResultVector &aQuery = m_pTradeBid->GetCacheCommInfo();
//	m_LockGetName.lock();
	T_TradeMerchInfo stOut;
//	memset(&stOut, 0, sizeof(T_TradeMerchInfo));
	for ( int i=0; i<aQuery.size(); ++i )
	{
		stOut = aQuery[i];
		string strCode;
		UnicodeToUtf8(stOut.stock_code, strCode);
		if (_stricmp(strCode.c_str(), pCommID)==0)	//�Ƚϴ���
		{
			break;
		}
	}
//	m_LockGetName.unlock();
	
// 	CString strName = _T(""); 
// 	wstring wstr;
// 	Gbk32ToUnicode((char *)(LPCTSTR)stOut.stock_name,wstr);
// 	strName.Format(_T("%s"),wstr.c_str());

	return (LPCTSTR)stOut.stock_name;
}

const _CommInfoType CDlgTradeQuery::GetCommInfoType()
{
	return m_CommInfoType;
}

const _CommInfoType CDlgTradeQuery::GetOtherFirm()
{
	return m_OtherFirm;
}

// ��ί��-����
void CDlgTradeQuery::OnBtnEntrustMainReset()
{
	m_DropEntrustMainCommInfo.SetCurSel(0);
	m_DropEntrustMainType.SetCurSel(0);
	m_DropEntrustMainStatus.SetCurSel(0);
	m_DropEntrustMainBuySell.SetCurSel(0);
	m_DropEntrustMainOpenClose.SetCurSel(0);
}

// ��ί��-��ѯ
void CDlgTradeQuery::OnBtnEntrustMainQuery()
{
	//QueryEntrust();
//	DoQueryAsy(EQT_QueryEntrust);
}

// ��ɽ�-����
void CDlgTradeQuery::OnBtnDealReset()
{
	m_DropDealCommInfo.SetCurSel(0);
	m_DropDealBuySell.SetCurSel(0);
	m_DropDealOpenClose.SetCurSel(0);	
}

// ��ɽ�-��ѯ
void CDlgTradeQuery::OnBtnDealQuery()
{
	//QueryDeal();
	DoQueryAsy(EQT_QueryDeal);
}

// ����Ʒ��Ϣ-��Ʒ���͸ı�
void CDlgTradeQuery::OnCbnSelchangeCommInfoType()
{
	QueryCommInfo();
	//DoQueryAsy(EQT_QueryCommInfo);	
}

// ��ֲ���ϸ
bool32 CDlgTradeQuery::QueryHoldDetail()
{
	if (NULL==m_pTradeBid||NULL==m_pTradeLoginInfo)
	{
		//ASSERT(0);
		return FALSE;
	}

	bool32 bRet = FALSE;
	CString strErrMsg = _T("");

	m_Lock_HoldDetail.lock();
	
	CClientReqQueryHold  queryHold;
//	memset(&queryHold, 0, sizeof(CClientReqQueryHold));

	queryHold.account = m_pTradeLoginInfo->StrUser;
	queryHold.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
	queryHold.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;
 	queryHold.mid_code = m_strMidCode;
 	queryHold.start_num = 0;
	queryHold.request_num = 0;
	queryHold.query_direction = _T("0");

	bRet = m_pTradeBid->ReqQueryHold(queryHold, strErrMsg);	// ����ֲ���ϸ��Ϣ

	
	m_Lock_HoldDetail.unlock();

	if (!bRet)
	{
		// �رս��״���
		m_eTradeStatus = ETLS_NotLogin;
		KillTimer(TIME_REFRESH_QUOTE);
		KillTimer(TIME_REFRESH_HOLDE);
		//���µ�¼orע��
		int iMode = 0;
		CDlgCloseTradeTip dlg;
		dlg.m_strTipMsg = strErrMsg;
		if (IDOK==dlg.DoModal())
		{
			iMode = 0;
		}
		else
		{
			iMode = 1;
		}
		PostMessage(EM_Message_WndClose,iMode,0);
	}

// 	//���ߴ���
// 	if ((1004==iRet||10001==iRet)&&ETLS_Logined==m_eTradeStatus)
// 	{
// 		// �رս��״���
// 		m_eTradeStatus = ETLS_NotLogin;
// 		KillTimer(TIME_REFRESH_QUOTE);
// 		//���µ�¼orע��
// 		int iMode = 0;
// 		CDlgCloseTradeTip dlg;
// 		dlg.m_strTipMsg = strErrMsg;
// 		if (IDOK==dlg.DoModal())
// 		{
// 			iMode = 0;
// 		}
// 		else
// 		{
// 			iMode = 1;
// 		}
// 		PostMessage(EM_Message_WndClose,iMode,0);
// 	//	MessageBox(strErrMsg,L"����",MB_OK|MB_ICONERROR);
// 	}
// 
// 	//֪ͨ����״̬�ı�
// 	NetWorkStatusNotify(iRet);
// 	//����������Ϣ
// 	if (0!=iRet&&ETLS_Logined==m_eTradeStatus&&m_bNetWorkStatus)
// 	{
// 		MessageBox(strErrMsg,L"����",MB_OK|MB_ICONERROR);
// 	}


	return bRet;
}

bool32 CDlgTradeQuery::QueryHoldSummary()
{
	if (NULL==m_pTradeBid||NULL==m_pTradeLoginInfo)
	{
		//ASSERT(0);
		return FALSE;
	}
	CString strErrMsg = _T("");

	m_Lock_HoldDetail.lock();
	{
		CClientReqQueryHoldTotal queryHoldSummary;
		queryHoldSummary.account = m_pTradeLoginInfo->StrUser;
		queryHoldSummary.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
		queryHoldSummary.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;
		queryHoldSummary.mid_code = m_strMidCode;
		queryHoldSummary.start_num = 0;
		queryHoldSummary.request_num = 0;
		queryHoldSummary.query_direction = _T("0");
		m_pTradeBid->ReqQueryHoldSummary(queryHoldSummary, strErrMsg);
	}
	m_Lock_HoldDetail.unlock();

	return TRUE;
}
bool32 CDlgTradeQuery::QueryLimitEntrust()
{
	if (NULL==m_pTradeBid||NULL==m_pTradeLoginInfo)
	{
		//ASSERT(0);

		//lint --e(570)
		return -1;
	}
	
	bool32 bRet =false;
	CString strErrMsg = _T("");
	
	m_Lock_Entrust.lock();

	if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
	{
		CClientReqQueryCancelEntrust query;
		query.account = m_pTradeLoginInfo->StrUser;
		query.mid_code = m_strMidCode;
		query.query_direction = _T("0");
		query.request_num = 0;
		query.start_num = 0;
		query.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
		query.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;

		bRet = m_pTradeBid->ReqQueryCancelEntrust(query, strErrMsg);	// �����ѯ�ɳ���
	}
	else
	{
		CClientReqQueryEntrust query;
		query.account = m_pTradeLoginInfo->StrUser;
		query.mid_code = m_strMidCode;
		query.query_direction = _T("0");
		query.request_num = 0;
		query.start_num = 0;
		query.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
		query.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;

		bRet = m_pTradeBid->ReqQueryEntrust(query, strErrMsg);	// ����ί��
	}

	m_Lock_Entrust.unlock();
	
	return bRet;
}

// ��ί��
// bool32 CDlgTradeQuery::QueryEntrust()
// {
// 	if (NULL==m_pTradeBid||NULL==m_pTradeLoginInfo)
// 	{
// 		//ASSERT(0);
// 		return -1;
// 	}
// 
// 	int iRet = 1;
// 	CString strErrMsg = _T("");
// 
// 	m_Lock_Entrust.lock();
// 
// 	OrderQueryIn query;
// 	memset(&query,0,sizeof(OrderQueryIn));
// 	
// 	string strUserID;
// 	string strSessionID;
// 	UnicodeToUtf8(m_pTradeLoginInfo->StrUser, strUserID);
// 	UnicodeToUtf8(m_pTradeLoginInfo->StrSessionID, strSessionID);
// 	query.userID = (char*)strUserID.c_str();
// 	query.sessionID = (char*)strSessionID.c_str();
// 
// 	T_CommInfoType type;
// 	int iCur = m_DropEntrustMainCommInfo.GetCurSel();
// 	FindCommInfoType(iCur,type);
// 	query.commodityID = type.chTypeValue;	// ��ƷID
// 	iCur = m_DropEntrustMainType.GetCurSel();
// 	if (0==iCur)// ��ȫ��
// 	{
// 		query.orderType = "";
// 	}
// 	else if (1==iCur)// �м�
// 	{	
// 		query.orderType = "1";
// 	}
// 	else//ָ��
// 	{
// 		query.orderType = "2";
// 	}
// 	iCur = m_DropEntrustMainStatus.GetCurSel();
// 	if (0==iCur)// ��ȫ��
// 	{
// 		query.orderStatue = "";
// 	}
// 	else if (1==iCur)// ��ί��
// 	{
// 		query.orderStatue = "1";
// 	}
// 	else if (2==iCur)// �ѳɽ�
// 	{
// 		query.orderStatue = "2";
// 	}
// 	else// �ѳ���
// 	{
// 		query.orderStatue = "3";
// 	}
// 	iCur = m_DropEntrustMainBuySell.GetCurSel();
// 	if (0==iCur)// ��ȫ��
// 	{
// 		query.buySell = "";
// 	}
// 	else if (1==iCur)//��
// 	{
// 		query.buySell = "1";
// 	}
// 	else// ��
// 	{
// 		query.buySell = "2";
// 	}
// 	iCur = m_DropEntrustMainOpenClose.GetCurSel();
// 	if (0==iCur)//��ȫ��
// 	{
// 		query.orderOpenClose = "";
// 	}
// 	else if (1==iCur)//����
// 	{
// 		query.orderOpenClose = "1";
// 	}
// 	else
// 	{
// 		query.orderOpenClose = "2";		
// 	}
// 
// 	// �����ֶ�Ĭ��Ϊ��
// 	query.orderNumber = "";
// 	query.marketID = "";
// 	query.startNumber = "";
// 	query.needNumber = "";
// 	query.sortField = "";
// 	query.isDesc = "";
// 	query.updateTime = "";
// 	query.agencyNumber = "";
// 	query.agencyPassword = "";
// 
// 	iRet = m_pTradeBid->QueryEntrust(&query,strErrMsg);	// ����ί��
// 
// 	m_Lock_Entrust.unlock();
// 	
// 	//���ߴ���
// 	if ((1004==iRet||10001==iRet)&&ETLS_Logined==m_eTradeStatus)
// 	{
// 		// �رս��״���
// 		m_eTradeStatus = ETLS_NotLogin;
// 		KillTimer(TIME_REFRESH_QUOTE);
// 		//���µ�¼orע��
// 		int iMode = 0;
// 		CDlgCloseTradeTip dlg;
// 		dlg.m_strTipMsg = strErrMsg;
// 		if (IDOK==dlg.DoModal())
// 		{
// 			iMode = 0;
// 		}
// 		else
// 		{
// 			iMode = 1;
// 		}
// 		PostMessage(EM_Message_WndClose,iMode,0);
// 		//MessageBox(strErrMsg,L"����",MB_OK|MB_ICONERROR);
// 	//	WndClose();
// 	}
// 
// 	//֪ͨ����״̬�ı�
// 	NetWorkStatusNotify(iRet);
// 	//����������Ϣ
// 	if (0!=iRet&&ETLS_Logined==m_eTradeStatus&&m_bNetWorkStatus)
// 	{
// 		MessageBox(strErrMsg,L"����",MB_OK|MB_ICONERROR);
// 	}
// 	
// 	return iRet;
// }

// ��ɽ�
bool32 CDlgTradeQuery::QueryDeal()
{
	if (NULL==m_pTradeBid||NULL==m_pTradeLoginInfo)
	{
		//ASSERT(0);
		return FALSE;
	}

	bool32 bRet = FALSE;
	CString strErrMsg = _T("");

	m_Lock_Deal.lock();

	CClientReqQueryDeal query;
//	memset(&query, 0, sizeof(CClientReqQueryDeal));

	query.account = m_pTradeLoginInfo->StrUser;
	query.mid_code = m_strMidCode;
	query.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
	query.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;
	
	bRet = m_pTradeBid->ReqQueryDeal(query, strErrMsg);
	m_Lock_Deal.unlock();
	
// 	//���ߴ���
// 	if ((1004==iRet||10001==iRet)&&ETLS_Logined==m_eTradeStatus)
// 	{
// 		// �رս��״���
// 		m_eTradeStatus = ETLS_NotLogin;
// 		KillTimer(TIME_REFRESH_QUOTE);
// 		//���µ�¼orע��
// 		int iMode = 0;
// 		CDlgCloseTradeTip dlg;
// 		dlg.m_strTipMsg = strErrMsg;
// 		if (IDOK==dlg.DoModal())
// 		{
// 			iMode = 0;
// 		}
// 		else
// 		{
// 			iMode = 1;
// 		}
// 		PostMessage(EM_Message_WndClose,iMode,0);
// 		//MessageBox(strErrMsg,L"����",MB_OK|MB_ICONERROR);
// 		//WndClose();
// 	}
// 
// 	//֪ͨ����״̬�ı�
// 	NetWorkStatusNotify(iRet);
// 	//����������Ϣ
// 	if (0!=iRet&&ETLS_Logined==m_eTradeStatus&&m_bNetWorkStatus)
// 	{
// 		MessageBox(strErrMsg,L"����",MB_OK|MB_ICONERROR);
// 	}
	
	return bRet;
}

bool32 CDlgTradeQuery::QueryTraderID()
{
	if (NULL==m_pTradeBid||NULL==m_pTradeLoginInfo)
	{
		//ASSERT(0);
		return FALSE;
	}

	bool32 bRet = FALSE;
	CString strErrMsg = _T("");

	m_Lock_TraderID.lock();

	CClientQueryTraderID query;

	query.account = m_pTradeLoginInfo->StrUser;
	query.mid_code = m_strMidCode;
	query.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
	query.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;

	bRet = m_pTradeBid->ReqQueryTraderID(query, strErrMsg);
	m_Lock_TraderID.unlock();

	return bRet;
}

// ���˻���Ϣ
bool32 CDlgTradeQuery::QueryUserInfo()
{
	if (NULL==m_pTradeBid||NULL==m_pTradeLoginInfo)
	{
		//ASSERT(0);
		return FALSE;
	}
	
	bool32 bRet = FALSE;
	CString strErrMsg = _T("");

	CClientReqUserInfo query;
//	memset(&query, 0, sizeof(CClientReqUserInfo));
// 	double dFolatPL = GetFloatPL();	// ����ӯ������
	
	m_Lock_UserInfo.lock();
	query.account = m_pTradeLoginInfo->StrUser;
	query.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
	query.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;

	bRet = m_pTradeBid->ReqUserInfo(query, strErrMsg);	// �˻���Ϣ
	m_Lock_UserInfo.unlock();
	
// 	//���ߴ���
// 	if ((1004==iRet||10001==iRet)&&ETLS_Logined==m_eTradeStatus)
// 	{
// 		// �رս��״���
// 		m_eTradeStatus = ETLS_NotLogin;
// 		KillTimer(TIME_REFRESH_QUOTE);
// 		//���µ�¼orע��
// 		int iMode = 0;
// 		CDlgCloseTradeTip dlg;
// 		dlg.m_strTipMsg = strErrMsg;
// 		if (IDOK==dlg.DoModal())
// 		{
// 			iMode = 0;
// 		}
// 		else
// 		{
// 			iMode = 1;
// 		}
// 		PostMessage(EM_Message_WndClose,iMode,0);
// 		//MessageBox(strErrMsg,L"����",MB_OK|MB_ICONERROR);
// 	//	WndClose();
// 	}
// 
// 	//֪ͨ����״̬�ı�
// 	NetWorkStatusNotify(iRet);
// 	//����������Ϣ
// 	if (0!=iRet&&ETLS_Logined==m_eTradeStatus&&m_bNetWorkStatus)
// 	{
// 		MessageBox(strErrMsg,L"����",MB_OK|MB_ICONERROR);
// 	}
	
	return bRet;
}

// ����Ʒ��Ϣ
bool32 CDlgTradeQuery::QueryCommInfo()
{
	if (ETLS_Logined == m_eTradeStatus)	// ��½״̬˵����Ʒ��Ϣ�Ѿ�ȡ��������������
	{
		//ReloadGridCommInfo();
		PostMessage(EM_Message_UpdataCommInfo,0,0);
		return 0;
	}

	if (NULL==m_pTradeBid||NULL==m_pTradeLoginInfo)
	{
		//ASSERT(0);
		return FALSE;
	}

	bool32 bRet = FALSE;
	CString strErrMsg = _T("");

	m_Lock_CommInfo.lock();
	
	CClientReqMerchInfo query;
//	memset(&query,0,sizeof(CClientReqMerchInfo));

	query.account = m_pTradeLoginInfo->StrUser;
	query.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
	query.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;
	
// 	T_CommInfoType type;
// 	int iCur = m_DropCommInfoType.GetCurSel();
// 	FindCommInfoType(iCur+1,type);	// û����ʾȫ��������+1
	query.stock_code =_T("");//��Ʒ��Ϣֻ��һ��//type.chTypeValue;	// ��ƷID
	query.mid_code = m_strMidCode;
	
	bRet = m_pTradeBid->ReqMerchInfo(query, strErrMsg);	// ����Ʒ��Ϣ

	m_Lock_CommInfo.unlock();
	
// 	//���ߴ���
// 	if ((1004==iRet||10001==iRet)&&ETLS_Logined==m_eTradeStatus)
// 	{
// 		// �رս��״���
// 		m_eTradeStatus = ETLS_NotLogin;
// 		KillTimer(TIME_REFRESH_QUOTE);
// 		//���µ�¼orע��
// 		int iMode = 0;
// 		CDlgCloseTradeTip dlg;
// 		dlg.m_strTipMsg = strErrMsg;
// 		if (IDOK==dlg.DoModal())
// 		{
// 			iMode = 0;
// 		}
// 		else
// 		{
// 			iMode = 1;
// 		}
// 		PostMessage(EM_Message_WndClose,iMode,0);
// 		//MessageBox(strErrMsg,L"����",MB_OK|MB_ICONERROR);
// 	//	WndClose();
// 	}
// 
// 	//֪ͨ����״̬�ı�
// 	NetWorkStatusNotify(iRet);
// 	//����������Ϣ
// 	if (0!=iRet&&ETLS_Logined==m_eTradeStatus&&m_bNetWorkStatus)
// 	{
// 		MessageBox(strErrMsg,L"����",MB_OK|MB_ICONERROR);
// 	}
		
	return bRet;
}

// ������
bool32 CDlgTradeQuery::QueryQuotation()
{
	if (NULL==m_pTradeBid||NULL==m_pTradeLoginInfo)
	{
		//ASSERT(0);

		//lint --e(570)
		return -1;
	}

	bool32 bRet = FALSE;
	CString strErrMsg = _T("");

	m_Lock_Quotation.lock();
	
	CClientReqQuote query;
	query.account = m_pTradeLoginInfo->StrUser;
	query.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
	query.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;
	query.stock_code = _T("");
	query.mid_code = m_strMidCode;

	bRet = m_pTradeBid->ReqQute(query,strErrMsg);
	m_Lock_Quotation.unlock();
	
	return bRet;
}

bool32 CDlgTradeQuery::RevokeLimitOrder(CString strNo)
{
	if (NULL==m_pTradeBid)
	{
		//ASSERT(0);

		//lint --e(570)
		return -1 ;
	}

	CClientReqCancelEntrust order;
	order.account = m_pTradeLoginInfo->StrUser;
	order.mid_code = m_strMidCode;
	order.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
	order.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;
	order.entrust_no = strNo;

	bool32 bRet = FALSE;
	CString strTipMsg = _T("");

	bRet = m_pTradeBid->ReqCancelEntrust(order, strTipMsg);

/*	if (!bRet)
	{
		CDlgTip dlg;
		dlg.m_strTipMsg = strTipMsg;
		dlg.m_eTipType = ETT_ERR;
		dlg.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��

		if (IDOK==dlg.DoModal())
		{
		}
	}*/
	return bRet;
}

bool32 CDlgTradeQuery::RevokeLimitOrder()
{
	if (NULL==m_pTradeBid)
	{
		//ASSERT(0);

		//lint --e(570)
		return -1 ;
	}

	CClientReqCancelEntrust order;
//	memset(&order, 0, sizeof(CClientReqCancelEntrust));
	order.account = m_pTradeLoginInfo->StrUser;
	order.mid_code = m_strMidCode;
	order.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
	order.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;

	bool bSelect = false;
	const T_RespQueryEntrust *pQuery = GetLimitOrderSelect();
	if (NULL != pQuery)
	{
		order.entrust_no = pQuery->entrust_no;
		bSelect = true;
	}
	
	if (!bSelect) //û��ѡ����,��ֱ������
	{
		//lint --e(570)
		return -1;
	}

	bool32 bRet = FALSE;
	CString strTipMsg = _T("");

//	m_LockThread.lock();
	bRet = m_pTradeBid->ReqCancelEntrust(order, strTipMsg);
//	m_LockThread.unlock();

	if (!bRet)
	{
		CDlgTip dlg;
		dlg.m_strTipMsg = strTipMsg;
		dlg.m_eTipType = ETT_ERR;
		dlg.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��

		if (IDOK==dlg.DoModal())
		{
		}
	}
	return bRet;
}

bool32 CDlgTradeQuery::SetLossProfit(const CClientReqSetStopLP *pSet,CString &strTipMsg)
{
	if (NULL==pSet)
	{
		//ASSERT(0);
		return FALSE ;
	}
	
	bool32 bRet = FALSE;
	
//	m_LockThread.lock();
	bRet = m_pTradeBid->ReqSetStopLP(*pSet, strTipMsg);
//	m_LockThread.unlock();
	
// 	//���ߴ���
// 	if ((1004==iRet||10001==iRet)&&ETLS_Logined==m_eTradeStatus)
// 	{
// 		// �رս��״���
// 		m_eTradeStatus = ETLS_NotLogin;
// 		KillTimer(TIME_REFRESH_QUOTE);
// 		PostMessage(EM_Message_WndClose,0,0);
// 		//	WndClose();
// 	}
// 	
// 	//֪ͨ����״̬�ı�
// 	NetWorkStatusNotify(iRet);
	
	return bRet;
}

bool32 CDlgTradeQuery::RevokeLossProfit(E_StopLossProfitType eType,bool bMain)
{
	if (NULL==m_pTradeBid)
	{
		//ASSERT(0);
		return FALSE;
	}

	if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
	{
		CClientReqSetStopLP reqSet;
		reqSet.account = m_pTradeLoginInfo->StrUser;
		reqSet.mid_code = m_strMidCode;
		reqSet.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
		reqSet.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;

		bool bSelect = false;
		const T_RespQueryHold *pQuery = NULL; 
		// ѡ������������ϸ���Ǹſ���ϸ
		if (bMain)
		{
			pQuery = GetHoldDetailMainSelect();
		}
		else
		{
			pQuery = GetHoldDetailSelect();
		}
		if (NULL != pQuery)
		{
			reqSet.hold_id = pQuery->hold_id;
			reqSet.stock_code = pQuery->stock_code;
			reqSet.trade_flag = L"2";
			reqSet.entrust_bs = pQuery->entrust_bs;

			double dBuyPrice = 0;	//���
			double dSellPrice = 0;	//����
			CString dTradeUnit = "";//���׵�λ
			double dMarginRate =0; //��֤��ϵ��

			string strCode;
			UnicodeToUtf8(pQuery->stock_code, strCode);
			GetBuySellPrice(strCode.c_str(), dBuyPrice, dSellPrice, dTradeUnit,dMarginRate);//��ȡƽ�ּ۸�
			if (_T("B") == reqSet.entrust_bs)
			{
				reqSet.entrust_price = dBuyPrice;
			}
			else
			{
				reqSet.entrust_price = dSellPrice;
			}

			//1����ֹ���2����ֹӯ��3����ֹ��ֹӯ��
			if (EST_StopLoss == eType)
			{
				reqSet.stop_loss = 0;
				reqSet.stop_profit = pQuery->stop_profit;
			}
			else if (EST_StopProfit==eType)
			{
				reqSet.stop_profit = 0;
				reqSet.stop_loss = pQuery->stop_loss;
			}
			else
			{
				reqSet.stop_loss = 0;
				reqSet.stop_profit = 0;
			}
			bSelect = true;
		}
		if (!bSelect) //û��ѡ����,��ֱ������
		{
			return FALSE;
		}

		bool32 bRet = FALSE;
		CString strTipMsg = _T("");
		SetLossProfit(&reqSet, strTipMsg);
		return bRet;
	}

	CClientReqCancelStopLP reqStop;
	reqStop.account = m_pTradeLoginInfo->StrUser;
	reqStop.mid_code = m_strMidCode;
	reqStop.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
	reqStop.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;
	
	bool bSelect = false;
	const T_RespQueryHold *pQuery = NULL; 
	// ѡ������������ϸ���Ǹſ���ϸ
	if (bMain)
	{
		pQuery = GetHoldDetailMainSelect();
	}
	else
	{
		pQuery = GetHoldDetailSelect();
	}
	if (NULL != pQuery)
	{
		reqStop.hold_id = pQuery->hold_id;
		reqStop.stock_code = pQuery->stock_code;

		//1����ֹ���2����ֹӯ��3����ֹ��ֹӯ��
		if (EST_StopLoss == eType)
		{
			reqStop.cancel_profitloss_type = _T("1");
			reqStop.entrust_no = pQuery->stop_loss_no;
		}
		else if (EST_StopProfit==eType)
		{
			reqStop.cancel_profitloss_type = _T("2");
			reqStop.entrust_no = pQuery->stop_profit_no;
		}
		else
		{
			reqStop.cancel_profitloss_type = _T("3");
		}
		bSelect = true;
	}
	
	if (!bSelect) //û��ѡ����,��ֱ������
	{
		return FALSE;
	}
	
	bool32 bRet = FALSE;
	CString strTipMsg = _T("");
	
//	m_LockThread.lock();
	bRet = m_pTradeBid->ReqCancelStopLP(reqStop, strTipMsg);
//	m_LockThread.unlock();
	
	if (!bRet)
	{
		CDlgTip dlg;
		dlg.m_strTipMsg = strTipMsg;
		dlg.m_eTipType = ETT_ERR;
		dlg.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��

		if (IDOK==dlg.DoModal())
		{
		}
	}

	return bRet;
}

// ί������
bool32 CDlgTradeQuery::ReqEntrust(const CClientReqEntrust& stReq, CString &strTipMsg)
{
	bool32 bRet = FALSE;

	//
	bRet = m_pTradeBid->ReqEntrust(stReq, strTipMsg);
	return bRet;
}

void CDlgTradeQuery::EntrustResultNotify(bool bSuc)
{
	if (bSuc)//ί�гɹ���ˢ����Ӧ�б�
	{
		RefreshList();
		PostMessage(WM_SIZE,NULL,NULL);
	}
}

//��С��
void CDlgTradeQuery::WndMaxMin()
{
	if (NULL==m_pTradeBid)
	{
		return;
	}
	m_pTradeBid->WndMaxMin();
//	m_bWndStatus = m_pTradeBid->GetWndStatus(); // ���ڵ�ǰ״̬ 0-��С 1-���
}

//�ر�
void CDlgTradeQuery::WndClose(int iMode)
{
	if (iMode!=3)
	{
		GetParent()->PostMessage(WM_CLOSETRADE,0,0);	//֪ͨ������������
	}

	if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
	{
		RevokeAllLimitOrder();
		RevokeAllProfitLoss();
		Sleep(50);
	}

	StopTrade(iMode); // �رա�������׵�һ�ж���

	m_iRevokeLimitOrderCnt = 0;
	m_iReqEntrustCnt = 0;

	if (NULL==m_pTradeBid)
	{
		return;
	}
	m_pTradeBid->WndClose(iMode);
}

// ֹͣ����
void CDlgTradeQuery::StopTrade(int iMode)
{
	KillTimer(TIME_REFRESH_QUOTE);
	KillTimer(TIME_REFRESH_HOLDE);
	
	if ( m_lWaitTradeDTPProcess>0 )
	{
		TRACE(_T("���׵�¼�����첽�߳��л���һС��ʱ���У�������ֹͣ���ײ�����\r\n"));
	//	ASSERT( 0 );
	}

	if (3==iMode)
	{
		while ( m_lWaitTradeDTPProcess>0 )
		{
			Sleep(10);
			break;
		}
	}
} 

const QueryCommInfoResultVector CDlgTradeQuery::GetCommInfo()
{
	ASSERT(m_pTradeBid);
	return m_pTradeBid->GetCacheCommInfo();
}

// �Ҽ��˵�
void CDlgTradeQuery::OnGridRButtonDown( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	ASSERT( pNotifyStruct && pResult );

	if ( NULL==m_pTradeBid )
	{
		ASSERT( 0 );
		return;
	}
	if ( pResult )
	{
		*pResult = 1;
	}
	if ( pNotifyStruct )
	{
 		int iGridID = pNotifyStruct->idFrom;

		m_pGridEvent = (CGridCtrlNormal *)GetDlgItem(iGridID);
		m_pGridEvent = DYNAMIC_DOWNCAST(CGridCtrlNormal, m_pGridEvent);

		enum E_QueryMenuID
		{
			EQM_Refresh = 1,		// ˢ��
			EQM_CancelLimitOrder,	// ����ָ�۵�
			EQM_CancelStopLoss,		// ����ֹ��
			EQM_CancelStopProfit,	// ����ֹӯ��
			EQM_CloseMarketOrder,	// �м�ƽ�ֵ�
			EQM_CloseLimitOrder,	// ָ��ƽ�ֵ�
			EQM_CancelStopLoss_Main,	// ����ֹ��(main)
			EQM_CancelStopProfit_Main,	// ����ֹӯ��(main)
			EQM_CloseMarketOrder_Main,	// �м�ƽ�ֵ�(main)
			EQM_CloseLimitOrder_Main,	// ָ��ƽ�ֵ�(main)
		};

		CMenu menu;
		menu.CreatePopupMenu();
 
 		switch ( iGridID )
 		{
		case EGID_ENTRUST:
			{
				const T_RespQueryEntrust *pQuery = GetLimitOrderSelect();

				menu.AppendMenu(MF_BYCOMMAND, EQM_CancelLimitOrder, _T("����ָ�۵�"));
				if (NULL==pQuery)	// ûѡ�У����û�
				{
					EnableMenuItem(menu.m_hMenu,EQM_CancelLimitOrder,MF_BYCOMMAND |MF_DISABLED | MF_GRAYED);
				}
				menu.AppendMenu(MF_SEPARATOR);
 				menu.AppendMenu(MF_BYCOMMAND, EQM_Refresh, _T("ˢ��"));				
			}
			break;
		case EGID_HOLDDETAILMAIN:
			{
				//const HoldingDetailQueryOut *pQuery = GetHoldDetailMainSelect();
				m_pHoldDetailMainSelect = NULL;
				m_pHoldDetailMainSelect = (T_RespQueryHold*)GetHoldDetailMainSelect();
				
				menu.AppendMenu(MF_BYCOMMAND, EQM_CloseMarketOrder_Main, _T("�м�ƽ�ֵ�"));
				menu.AppendMenu(MF_BYCOMMAND, EQM_CloseLimitOrder_Main, _T("ָ��ƽ�ֵ�"));
				menu.AppendMenu(MF_SEPARATOR);
				menu.AppendMenu(MF_BYCOMMAND, EQM_CancelStopLoss_Main, _T("����ֹ��"));
				menu.AppendMenu(MF_BYCOMMAND, EQM_CancelStopProfit_Main, _T("����ֹӯ��"));
				menu.AppendMenu(MF_SEPARATOR);
				menu.AppendMenu(MF_BYCOMMAND, EQM_Refresh, _T("ˢ��"));	

				if (NULL==m_pHoldDetailMainSelect)//ûѡ�У�ȫ��
				{
					EnableMenuItem(menu.m_hMenu,EQM_CloseMarketOrder_Main,MF_BYCOMMAND |MF_DISABLED | MF_GRAYED);
					EnableMenuItem(menu.m_hMenu,EQM_CloseLimitOrder_Main,MF_BYCOMMAND |MF_DISABLED | MF_GRAYED);
					EnableMenuItem(menu.m_hMenu,EQM_CancelStopLoss_Main,MF_BYCOMMAND |MF_DISABLED | MF_GRAYED);
					EnableMenuItem(menu.m_hMenu,EQM_CancelStopProfit_Main,MF_BYCOMMAND |MF_DISABLED | MF_GRAYED);
				}
				else
				{
					//ѡ���ˣ����ж�ֹ��ֹӯ��
					if ((m_pHoldDetailMainSelect->stop_loss)>-0.000001
						&&(m_pHoldDetailMainSelect->stop_loss)<0.000001)
					{//���ܳ�ֹ��
						EnableMenuItem(menu.m_hMenu,EQM_CancelStopLoss_Main,MF_BYCOMMAND |MF_DISABLED | MF_GRAYED);
					}
					if ((m_pHoldDetailMainSelect->stop_profit)>-0.000001
						&&(m_pHoldDetailMainSelect->stop_profit)<0.000001)
					{//���ܳ�ֹӯ
						EnableMenuItem(menu.m_hMenu,EQM_CancelStopProfit_Main,MF_BYCOMMAND |MF_DISABLED | MF_GRAYED);
					}
				}
			}
			break;
		case EGID_HOLDTOTAL:
			{
				menu.AppendMenu(MF_BYCOMMAND, EQM_Refresh, _T("ˢ��"));	
				break;
			}
		case EGID_USERINFOMAIN:
		case EGID_DEAL:
 		default:
 			break;
 		}
 
		if ( menu.GetMenuItemCount()<=0 )
		{
			return;
		}
 		//menu.SetDefaultItem(0, TRUE);

		CPoint ptClick(0,0);
		GetCursorPos(&ptClick);
//		CWnd *pLastFocus = GetFocus();
		int iSel = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_NONOTIFY|TPM_RETURNCMD, ptClick.x, ptClick.y, this);
// 		if ( pLastFocus )
// 		{
// 			pLastFocus->SetFocus();
// 		}
		if ( iSel>0 )
		{
			switch ( iSel )
			{
			case EQM_CancelLimitOrder:
				{
					CDlgTip dlg;
					dlg.m_strTipMsg = _T("ȷ������ָ�۵���");
					dlg.m_eTipType = ETT_TIP;
					dlg.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��

					if (IDOK==dlg.DoModal())
					{
						RevokeLimitOrder();
					}
				}
				break;
			case EQM_Refresh:
				{
					RefreshList();
				}
				break;
			case EQM_CancelStopLoss:		// ����ֹ��
				{
					CDlgTip dlg;
					dlg.m_strTipMsg = _T("ȷ������ֹ����");
					dlg.m_eTipType = ETT_TIP;
					dlg.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��

					if (IDOK==dlg.DoModal())
					{
						RevokeLossProfit(EST_StopLoss,false);
					}
				}
				break;
			case EQM_CancelStopLoss_Main:
				{
					CDlgTip dlg;
					dlg.m_strTipMsg = _T("ȷ������ֹ����");
					dlg.m_eTipType = ETT_TIP;
					dlg.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��

					if (IDOK==dlg.DoModal())
					{
						RevokeLossProfit(EST_StopLoss,true);
					}
				}
				break;
			case EQM_CancelStopProfit:	// ����ֹӯ��
				{
					CDlgTip dlg;
					dlg.m_strTipMsg = _T("ȷ������ֹӯ����");
					dlg.m_eTipType = ETT_TIP;
					dlg.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��

					if (IDOK==dlg.DoModal())
					{
						RevokeLossProfit(EST_StopProfit,false);
					}
				}
				break;
			case EQM_CancelStopProfit_Main:	// ����ֹӯ��
				{
					CDlgTip dlg;
					dlg.m_strTipMsg = _T("ȷ������ֹӯ����");
					dlg.m_eTipType = ETT_TIP;
					dlg.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��

					if (IDOK==dlg.DoModal())
					{
						RevokeLossProfit(EST_StopProfit,true);
					}
				}
				break;
			case EQM_CloseMarketOrder:	// �м�ƽ�ֵ�
				{
					GetParent()->PostMessage(WM_ENTRUSTTYPE_CLOSE,(WPARAM)m_pHoldDetailSelect,ECT_Market);
				}
				break;
			case EQM_CloseMarketOrder_Main:	// �м�ƽ�ֵ�
				{
					GetParent()->PostMessage(WM_ENTRUSTTYPE_CLOSE,(WPARAM)m_pHoldDetailMainSelect,ECT_Market);
				}
				break;
			case EQM_CloseLimitOrder:	// ָ��ƽ�ֵ�
				{
					GetParent()->PostMessage(WM_ENTRUSTTYPE_CLOSE,(WPARAM)m_pHoldDetailSelect,ECT_Limit);
				}
				break;
			case EQM_CloseLimitOrder_Main:	// ָ��ƽ�ֵ�
				{
					GetParent()->PostMessage(WM_ENTRUSTTYPE_CLOSE,(WPARAM)m_pHoldDetailMainSelect,ECT_Limit);
				}
				break;
			}
		}
		else
		{
		}
 	}
}

//˫��
void CDlgTradeQuery::OnGridRDBButtonDown( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	ASSERT( pNotifyStruct && pResult );
	if ( NULL==m_pTradeBid )
	{
		ASSERT( 0 );
		return;
	}
	if ( pResult )
	{
		*pResult = 1;
	}
	if ( pNotifyStruct )
	{
		int iGridID = pNotifyStruct->idFrom;
		m_pGridEvent = (CGridCtrlNormal *)GetDlgItem(iGridID);
		m_pGridEvent = DYNAMIC_DOWNCAST(CGridCtrlNormal, m_pGridEvent);
		switch ( iGridID )
		{
		case EGID_ENTRUST:
			{
				CDlgTip dlg;
				dlg.m_strTipMsg = _T("ȷ������ָ�۵���");
				dlg.m_eTipType = ETT_TIP;
				dlg.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��
				dlg.m_staticTip.SetTipMsgCenter(true);

				if (IDOK==dlg.DoModal())
				{
					RevokeLimitOrder();
				}
				break;
			}
		}
	}
}

void CDlgTradeQuery::OnGridSelChange( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	ASSERT( pNotifyStruct && pResult );
	if ( NULL==m_pTradeBid )
	{
		ASSERT( 0 );
		return;
	}
	if ( pResult )
	{
		*pResult = 1;
	}

	if ( pNotifyStruct )
	{
		int iGridID = pNotifyStruct->idFrom;
		m_pGridEvent = (CGridCtrlNormal *)GetDlgItem(iGridID);
		m_pGridEvent = DYNAMIC_DOWNCAST(CGridCtrlNormal, m_pGridEvent);
		switch ( iGridID )
		{
		case EGID_HOLDDETAILMAIN:
			{
				m_pHoldDetailMainSelect = NULL;
				m_pHoldDetailMainSelect = (T_RespQueryHold*)GetHoldDetailMainSelect();
				if (NULL!=m_pHoldDetailMainSelect)
				{
					GetParent()->PostMessage(WM_HOLDDETAIL_CLICK,0,0);
					GridClickHoldDetailSelect();
				}
			}
			break;
		case EGID_HOLDTOTAL:
			{
				m_pHoldSummarySelect = (T_RespQueryHoldTotal*)GetHoldSummarySelect();
				GridClickHoldSummarySelect();
			}
			break;
		}
	}

}

void CDlgTradeQuery::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	
	CDialog::OnLButtonUp(nFlags, point);
}

// ��ȡָ��ί�е�ĳһ����Ϣ
const T_RespQueryEntrust* CDlgTradeQuery::GetLimitOrderSelect()
{
	CGridCtrlNormal *pGrid = &m_GridEntrust;
	if ( NULL!=pGrid )
	{
		CCellRange cellRange = pGrid->GetSelectedCellRange();
		if ( cellRange.IsValid() )
		{
			const QueryEntrustResultVector &aQuery = m_pTradeBid->GetCacheEntrust();
			int iRow = cellRange.GetMinRow();
			int iIndex = iRow - pGrid->GetFixedRowCount();
			int iSize = aQuery.size();
			if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
			{
				iIndex = iSize-iIndex-1;
			}
			
			if ( iIndex>=0 && iIndex<iSize )
			{
				return &aQuery[iIndex];
			}
		}
	}

	return NULL;
}

// ��ȡ�ֲ���ϸĳһ����Ϣ
const T_RespQueryHold* CDlgTradeQuery::GetHoldDetailSelect()
{
	CGridCtrlNormal *pGrid = &m_GridHoldDetail;
	if ( NULL!=pGrid )
	{
		CCellRange cellRange = pGrid->GetSelectedCellRange();
		if ( cellRange.IsValid() )
		{
			const QueryHoldDetailResultVector &aQuery = m_pTradeBid->GetCacheHoldDetail();
			int iRow = cellRange.GetMinRow();
			int iIndex = iRow - pGrid->GetFixedRowCount();
			int iSize = aQuery.size();
			if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
			{
				iIndex = iSize-iIndex-1;
			}

			if ( iIndex>=0 && iIndex<iSize )
			{
				return &aQuery[iIndex];
			}
		}
	}
	
	return NULL;
}

// ��ȡ�ֲ���ϸĳһ����Ϣ
const T_RespQueryHold* CDlgTradeQuery::GetHoldDetailMainSelect()
{
	CGridCtrlNormal *pGrid = &m_GridHoldDetailMain;
	if ( NULL!=pGrid )
	{
		CCellRange cellRange = pGrid->GetSelectedCellRange();
		if ( cellRange.IsValid() )
		{
			const QueryHoldDetailResultVector &aQuery = m_pTradeBid->GetCacheHoldDetail();
			int iRow = cellRange.GetMinRow();
			int iIndex = iRow - pGrid->GetFixedRowCount();
			int iSize = aQuery.size();
			if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
			{
				iIndex = iSize-iIndex-1;
			}
			
			if ( iIndex>=0 && iIndex<iSize )
			{
				return &aQuery[iIndex];
			}
		}
		// û��ѡ�оͲ�ҪĬ��ѡ���һ����
	/*	else
		{
			const QueryHoldDetailResultVector &aQuery = m_pTradeBid->GetCacheHoldDetail();
			if (0 < aQuery.size())
			{
				return &aQuery[0];
			}
		}*/
	}
	
	return NULL;
}

const T_RespQueryHoldTotal* CDlgTradeQuery::GetHoldSummarySelect()
{
	CGridCtrlNormal *pGrid = &m_GridHoldSummary;
	if ( NULL!=pGrid )
	{
		CCellRange cellRange = pGrid->GetSelectedCellRange();
		if ( cellRange.IsValid() )
		{
			int iRow = cellRange.GetMinRow();
			int iIndex = iRow - pGrid->GetFixedRowCount();
			
			const QueryHoldSummaryResultVector &aQuery = m_pTradeBid->GetCacheHoldSummary();
			if ( iIndex>=0 && iIndex<aQuery.size() )
			{
				return &aQuery[iIndex];
			}
		}
		// û��ѡ�оͲ�ҪĬ��ѡ���һ����
		//else
		//{
		//	const QueryHoldSummaryResultVector &aQuery = m_pTradeBid->GetCacheHoldSummary();
		//	if (0 < aQuery.size())
		//	{
		//		return &aQuery[0];
		//	}
		//}
	}
	return NULL;
}	

void CDlgTradeQuery::DispatchThreadProcess( bool bSucDisp, LPARAM l )
{
	T_ExportMT2DispatchThreadProcessorLParam	*pParam = (T_ExportMT2DispatchThreadProcessorLParam *)l;
	ASSERT( pParam );
	if ( NULL!=pParam )
	{	
		//��¼״̬�²���������
		if (ETLS_Logined!=m_eTradeStatus)
		{
			// �ͷ� new������pParam
			DecDTPNeedProcessFlag();
			delete pParam;
			return;
		}

		switch ( pParam->iEventId )
		{
		case EQT_QueryHoldDetail:
			{
			//	m_LockThread.lock();
				QueryHoldDetail();
			//	m_LockThread.unlock();

				DecDTPNeedProcessFlag();
			}
			break;
// 		case EQT_QueryEntrust:
// 			{
// 			//	m_LockThread.lock();
// 				QueryEntrust();
// 			//	m_LockThread.unlock();
// 
// 				DecDTPNeedProcessFlag();
// 			}
// 			break;
		case EQT_QueryCancel:
		case EQT_QueryLimitEntrust:
			{
				//	m_LockThread.lock();
				QueryLimitEntrust();
				//	m_LockThread.unlock();
				
				DecDTPNeedProcessFlag();
			}
			break;
		case EQT_QueryDeal:
			{
			//	m_LockThread.lock();
				QueryDeal();
			//	m_LockThread.unlock();

				DecDTPNeedProcessFlag();
			}
			break;
		case EQT_QueryUserInfo:
			{
			//	m_LockThread.lock();
				QueryUserInfo();
			//	m_LockThread.unlock();
				
				DecDTPNeedProcessFlag();
			}
			break;
		case EQT_QueryCommInfo:
			{
			//	m_LockThread.lock();
				QueryCommInfo();
			//	m_LockThread.unlock();

				DecDTPNeedProcessFlag();
			}
			break;
		case EQT_QueryQuote:
			{
				QueryQuotation();
				DecDTPNeedProcessFlag();
			}
			break;
		case EQT_QueryHoldSummary:
			{
				QueryHoldSummary();
				DecDTPNeedProcessFlag();
			}
			break;
		default:
			//ASSERT(0);
			break;
		}
		
		// �ͷ� new������pParam
		delete pParam;
	}
}

LRESULT CDlgTradeQuery::OnMsgUpdataCommIfo( WPARAM w, LPARAM l )
{
	ReloadGridCommInfo();
	
	return 1;	
}

LRESULT CDlgTradeQuery::OnMsgWndClose( WPARAM w, LPARAM l )
{
	m_eTradeStatus = ETLS_NotLogin;
	WndClose(w);
	
	return 1;	
}

// �������groupbox��С�ı�֪ͨ
LRESULT CDlgTradeQuery::OnMsgGroupBoxPosChanged( WPARAM w, LPARAM l )
{
	if (IDC_DEF_GROUPBOX_HOLDDETAIL==w)//�ֲ���ϸ
	{
		CRect rect(0,0,0,0);
		m_GroupHoldDetail.GetWindowRect(rect);

		m_bActivaGroupHoldDetail = TRUE;	// ������
	}
	else if (IDC_DEF_GROUPBOX_ENTRUST==w)
	{
		m_bActivaGroupEntrust = TRUE;	// ������
	}
	else
	{
		return 0;
	}

	RecalcLayout(FALSE);	

	return 1;
}

void CDlgTradeQuery::OnTimer(UINT nIDEvent)
{
	BOOL bMainFrm = 1;//!pMainFrame->IsIconic();
	BOOL bQueryFrm = 1;//IsWindowVisible();
	m_bWndStatus = m_pTradeBid->GetWndStatus();
	if (nIDEvent==TIME_REFRESH_QUOTE/* && m_bWndStatus*/ && bMainFrm && bQueryFrm)
	{
		// ���quote_flag=1 ���ײ��ṩ����, ��������ȡ,�������ز�ѯ
		if (_T("1") == m_pTradeLoginInfo->StrQuoteFlag)
		{
			KillTimer(TIME_REFRESH_QUOTE);
		}
		else
		{
			DoQueryAsy(EQT_QueryQuote);
		}
	}

	if (nIDEvent == TIME_REFRESH_HOLDE/* && m_bWndStatus */&& bMainFrm && bQueryFrm)
	{
		if(m_eCurQueryType == EQT_QueryHoldDetail)
		{
			DoQueryAsy(EQT_QueryHoldDetail);
		}

		//if (EQT_QueryHoldSummary == m_eCurQueryType)
		{
			DoQueryAsy(EQT_QueryHoldSummary);
		}
		
		if (m_eCurQueryType == EQT_QueryUserInfo)
		{
			m_bAccountInfo = true;
		}
		else
		{
			m_bAccountInfo = false;
		}
		DoQueryAsy(EQT_QueryUserInfo);
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CDlgTradeQuery::IncDTPNeedProcessFlag()
{
	InterlockedIncrement((LPLONG)&m_lWaitTradeDTPProcess);
}

void CDlgTradeQuery::DecDTPNeedProcessFlag()
{
	InterlockedDecrement((LPLONG)&m_lWaitTradeDTPProcess);
}

// �첽��ѯ
void CDlgTradeQuery::DoQueryAsy(E_QueryType eType)
{
	IncDTPNeedProcessFlag();

//	CExportMT2DispatchThread thread = CExportMT2DispatchThread::Instance();

	if (!CExportMT2DispatchThread::Instance().InitInstance() )
	{
		DecDTPNeedProcessFlag();
	}
	T_ExportMT2DispatchThreadProcessorLParam *pParam = new T_ExportMT2DispatchThreadProcessorLParam;
	pParam->iEventId = eType;
	if ( !CExportMT2DispatchThread::Instance().Post2DispatchThread(this, (LPARAM)pParam) )
	{
		delete pParam;
		pParam = NULL;
		DecDTPNeedProcessFlag();
	}
}

// ��������ʾ
inline const CString CDlgTradeQuery::GetBuySellShow(const CString sBuySell)
{
	if (_T("B") == sBuySell)//��
	{
		return _T("����");
	}
	else if (_T("S") == sBuySell)//��
	{
		return _T("����");
	}

	// ģ�⽻��
	if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
	{
		if ((_T("C")==sBuySell) || (_T("E") == sBuySell)) // ���򣬵���
		{
			return _T("����");
		}
		else if ((_T("D")==sBuySell) || (_T("F") == sBuySell)) // ����������
		{
			return _T("����");
		}
	}
	
	return "-";
}

inline const CString CDlgTradeQuery::GetEntrustStatusShow(const CString sStatus)
{
	if (_T("0") == sStatus)//δ��
	{
		return _T("δ��");
	}
	else if (_T("1")==sStatus)//�ѱ�
	{
		return _T("�ѱ�");
	}
	else if (_T("2")==sStatus)//δ֪
	{
		return _T("δ֪");
	}
	else if (_T("3")==sStatus)//����
	{
		return _T("����");
	}
	else if (_T("4")==sStatus)//����
	{
		return _T("����");
	}
	else if (_T("5")==sStatus)//�ѳ�
	{
		return _T("�ѳ�");
	}
	else if (_T("6")==sStatus)//���ɲ���
	{
		return _T("���ɲ���");
	}
	else if (_T("7")==sStatus)//�ѳ�
	{
		return _T("�ѳ�");
	}
	else if (_T("8")==sStatus)//�ѷ�
	{
		return _T("�ѷ�");
	}
	
	return _T("-");
}

inline const char* CDlgTradeQuery::GetEntrustTypeShow(const short sType)
{
	if (1==sType)//�м�ί��
	{
		return "�м�ί��";
	}
	else if (2==sType)//ָ��ί��
	{
		return "ָ��ί��";
	}
	
	return "-";
}

inline const CString CDlgTradeQuery::GetOpenCloseShow(const CString sType)
{
	if (_T("0")==sType)//����
	{
		return _T("����");
	}
	else if (_T("1")==sType)//ƽ��
	{
		return _T("ƽ��");
	}
	else if(_T("2") == sType) //ָ���ֵ���ƽ��
	{
		return _T("ָ���ֵ���ƽ��");
	}
	
	return _T("-");
}

inline const CString CDlgTradeQuery::GetDealTypeShow(const CString sType)
{
	if (_T("1")==sType||_T("2")==sType||_T("3")==sType)//�м۳ɽ�
	{
		return _T("�м۳ɽ�");
	}
	else if (_T("4")==sType)//�Զ�ǿƽ
	{
		return _T("�Զ�ǿƽ");
	}
	else if (_T("5")==sType)//�ֶ�ǿƽ
	{
		return _T("�ֶ�ǿƽ");
	}
	else if (_T("6")==sType||_T("7")==sType)//ָ�۳ɽ�
	{
		return _T("ָ�۳ɽ�");
	}
	
	return _T("-");
}

inline const char* CDlgTradeQuery::GetDealOperateTypeShow(const int iType)
{
	if (1==iType)//�û��µ�
	{
		return "�û��µ�";
	}
	else if (2==iType||7==iType)//�绰�µ�
	{
		return "�绰�µ�";
	}
	else if (3==iType||4==iType||5==iType||6==iType)//ϵͳ�µ�
	{
		return "ϵͳ�µ�";
	}
	else if (8==iType)//����ָ���µ�
	{
		return "����ָ���µ�";
	}
	
	return "-";
}

inline const char* CDlgTradeQuery::UtcTOLocalTime(time_t t,char *pTime)
{
	struct tm * local_time;
	local_time = localtime ( &t );

	sprintf(pTime,"%04d-%02d-%02d %02d:%02d:%02d",
		local_time->tm_year+1900,local_time->tm_mon+1,local_time->tm_mday,
		local_time->tm_hour,local_time->tm_min,local_time->tm_sec);

	return pTime;
}

inline const CString CDlgTradeQuery::GetUserInfoStatus(const CString chStatus)
{
	if ("0"==chStatus)//δ����
	{
		return "����";
	}
	else if ("1"==chStatus)//����
	{
		return "δ����";
	}
	else if ("2"==chStatus)//����
	{
		return "����";
	}
	else if ("3"==chStatus)//��ֹ
	{
		return "��ֹ";
	}
	else if ("4"==chStatus)//�Ѵ���
	{
		return "�Ѵ���";
	}
	
	return "-";
}

inline const char* CDlgTradeQuery::GetCommInfoStatus(const short sStatus)
{
	
	if (0==sStatus)//�༭��
	{
		return "�༭��";
	}
	else if (1==sStatus)//����
	{
		return "����";
	}
	
	return "����";
}

inline const char* CDlgTradeQuery::GetFeeType(const int iType)
{
	if (1==iType)//���ٷֱ�
	{
		return "������";
	}
	else if (2==iType)//����������
	{
		return "������";
	}

	if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
	{
		return "������";
	}
	
	return "-";
} 

inline const char* CDlgTradeQuery::GetFeeGetType(const int iType)
{
	if (1==iType)//���߽���
	{
		return "���߽���";
	}
	else if (2==iType)//����ƽ��
	{
		return "����ƽ��";
	}
	else if (3==iType)//˫�߽�ƽ
	{
		return "˫�߽�ƽ";
	}
	else if (4==iType)//��ҹ��ƽ
	{
		return "��ҹ��ƽ";
	}

	if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
	{
		return "˫�߽�ƽ";
	}
	
	return "-";
}

inline const char* CDlgTradeQuery::GetMarginType(const int iType)
{	
	if (1==iType)//���ٷֱ�
	{
		return "������";
	}
	else if (2==iType)//����������
	{
		return "������";
	}

	if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
	{
		return "������";
	}
	
	return "-";
}

inline const char* CDlgTradeQuery::GetDeferType(const int iType)
{	
	if (1==iType)//���ٷֱ�
	{
		return "������";
	}
	else if (2==iType)//����������
	{
		return "������";
	}
	
	return "-";
}

//��ȡ����״̬
BOOL CDlgTradeQuery::NetWorkStatusNotify(const int iErrorCode)
{
	if (ETLS_Logined!=m_eTradeStatus)//������δ��¼�ɹ�
	{
		return FALSE;
	}

	BOOL bStatus = FALSE;

	if(1==iErrorCode
		||5==iErrorCode
		||6==iErrorCode
		||7==iErrorCode
		||9==iErrorCode
		||35==iErrorCode
		||55==iErrorCode
		||56==iErrorCode)//����Ͽ�
	{
		bStatus = FALSE;
	}
	else
	{
		bStatus = TRUE;
	}

//	if (m_bNetWorkStatus!=bStatus)//��ǰ״̬��֮ǰ��һ��,֪ͨ״̬�ı�
	{
		m_bNetWorkStatus = bStatus;
		GetParent()->PostMessage(WM_NETWORKSTATUS_CHANGE,m_bNetWorkStatus,0);
	}
	
	return m_bNetWorkStatus;
}

BOOL CDlgTradeQuery::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		return true;
	}

	if (WM_SYSKEYDOWN == pMsg->message || WM_KEYDOWN == pMsg->message)
	{
		if ( VK_F12 == pMsg->wParam )
		{
			CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

			CIoViewBase* p = pMainFrame->FindActiveIoView();
			if (NULL != p)
			{
				pMainFrame->OnQuickTrade(2, p->GetMerchXml());
			}	
			return TRUE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgTradeQuery::ShowQueryChange(E_QueryType eQueryType)
{
	if (m_eCurQueryType == eQueryType)
	{
		return;
	}

	m_dlgHomePage.ShowWindow(SW_HIDE);

	if (eQueryType == EQT_QueryHome)
	{
		ShowHideHoldDetailMain(SW_HIDE);
		ShowHideEntrustMain(SW_HIDE);
		ShowHideHoldSummaryMain(SW_HIDE);
		ShowHideDeal(SW_HIDE);
		ShowHideAccInfoMain(SW_HIDE);
		ShowHideCommInfo(SW_HIDE);
		ShowHideLimitEntrust(SW_HIDE);
		ShowHideHome(SW_SHOW);
		m_dlgHomePage.ShowWindow( SW_SHOW );
	}
	else if (eQueryType == EQT_QueryHoldDetail)
	{
		ShowHideHome(SW_HIDE);
		ShowHideHoldDetailMain(SW_SHOW);
		ShowHideHoldSummaryMain(SW_HIDE);
		ShowHideEntrustMain(SW_HIDE);
		ShowHideDeal(SW_HIDE);
		ShowHideAccInfoMain(SW_HIDE);
		ShowHideCommInfo(SW_HIDE);
		ShowHideLimitEntrust(SW_HIDE);
	}
	else if (eQueryType == EQT_QueryHoldSummary)
	{
		ShowHideHome(SW_HIDE);
		ShowHideHoldDetailMain(SW_HIDE);
		ShowHideHoldSummaryMain(SW_SHOW);
		ShowHideEntrustMain(SW_HIDE);
		ShowHideDeal(SW_HIDE);
		ShowHideAccInfoMain(SW_HIDE);
		ShowHideCommInfo(SW_HIDE);
		ShowHideLimitEntrust(SW_HIDE);
	}
	else if (eQueryType == EQT_QueryLimitEntrust)
	{
		CGridCtrlNormal *pGrid = &m_GridEntrust;
		pGrid->SetColumnWidth(0,0);

		ShowHideHome(SW_HIDE);
		ShowHideEntrustMain(SW_HIDE);
		ShowHideHoldSummaryMain(SW_HIDE);
		ShowHideDeal(SW_HIDE);
		ShowHideHoldDetailMain(SW_HIDE);
		ShowHideAccInfoMain(SW_HIDE);
		ShowHideCommInfo(SW_HIDE);
		ShowHideLimitEntrust(SW_SHOW);
		m_staticDisEntrust.ShowWindow(SW_HIDE);
	}
	else if (eQueryType == EQT_QueryCancel)
	{
		CGridCtrlNormal *pGrid = &m_GridEntrust;
		pGrid->SetColumnWidth(0,s_kaEntrustHeader[0].iColWidth);
		
		ShowHideHome(SW_HIDE);
		ShowHideEntrustMain(SW_HIDE);
		ShowHideHoldSummaryMain(SW_HIDE);
		ShowHideDeal(SW_HIDE);
		ShowHideHoldDetailMain(SW_HIDE);
		ShowHideAccInfoMain(SW_HIDE);
		ShowHideCommInfo(SW_HIDE);
		ShowHideLimitEntrust(SW_SHOW);
		m_staticDisEntrust.ShowWindow(SW_SHOW);
	}
	else if (eQueryType == EQT_QueryUserInfo)
	{
		ShowHideHome(SW_HIDE);
		ShowHideEntrustMain(SW_HIDE);
		ShowHideHoldSummaryMain(SW_HIDE);
		ShowHideDeal(SW_HIDE);
		ShowHideHoldDetailMain(SW_HIDE);
		ShowHideAccInfoMain(SW_SHOW);
		ShowHideCommInfo(SW_HIDE);
		ShowHideLimitEntrust(SW_HIDE);
	}
	else if (eQueryType == EQT_QueryDeal)
	{
		ShowHideHome(SW_HIDE);
		ShowHideEntrustMain(SW_HIDE);
		ShowHideHoldSummaryMain(SW_HIDE);
		ShowHideDeal(SW_SHOW);
		ShowHideHoldDetailMain(SW_HIDE);
		ShowHideAccInfoMain(SW_HIDE);
		ShowHideCommInfo(SW_HIDE);
		ShowHideLimitEntrust(SW_HIDE);
	}
	else if (eQueryType == EQT_QueryCommInfo)
	{
		ShowHideHome(SW_HIDE);
		ShowHideEntrustMain(SW_HIDE);
		ShowHideHoldSummaryMain(SW_HIDE);
		ShowHideDeal(SW_HIDE);
		ShowHideHoldDetailMain(SW_HIDE);
		ShowHideAccInfoMain(SW_HIDE);
		ShowHideCommInfo(SW_SHOW);
		ShowHideLimitEntrust(SW_HIDE);
	}
	else
	{
		//ASSERT(0);
		return;
	}
	m_eCurQueryType = eQueryType;
	
	RecalcLayout(true);
	QueryListData(m_eCurQueryType);	// tab�л�������б���Ϣ	
}

LRESULT CDlgTradeQuery::OnDisEntrust(WPARAM wParam, LPARAM lParam)
{
	CGridCtrlNormal *p = (CGridCtrlNormal*)wParam;
	if (p == &m_GridEntrust)
	{
		ReleaseCapture(); // ���ⵯ������Ҫ��������
		CDlgTip dlg;
		dlg.m_strTipMsg = _T("ȷ������ָ�۵���");
		dlg.m_eTipType = ETT_TIP;
		dlg.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��
		dlg.m_staticTip.SetTipMsgCenter(true);
		
		if (dlg.DoModal() == IDOK)
		{
			RevokeLimitOrder();
		}
	}
	
	return 0;
}

void CDlgTradeQuery::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rect(0,0,0,0);
	GetClientRect(rect);

	CBitmap bitmap;	//����һ��λͼ����
	CDC MemeDc;	//���ȶ���һ����ʾ�豸����
	//���������豸DC��������ʱ�����ܻ�ͼ����Ϊû�еط���
	MemeDc.CreateCompatibleDC(&dc);	
	//����һ������Ļ��ʾ���ݵ�λͼ������λͼ�Ĵ�С������ô��ڵĴ�С��Ҳ�����Լ�����
	//���磺�й�����ʱ��Ҫ���ڵ�ǰ���ڵĴ�С����BitBltʱ���������ڴ���Ĳ��ֵ���Ļ�ϣ�
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	//��λͼѡ�뵽�ڴ���ʾ�豸��(ֻ��ѡ����λͼ���ڴ���ʾ�豸���еط���ͼ������ָ����λͼ)
	CBitmap *pOldBitmap = MemeDc.SelectObject(&bitmap);
	//���ñ���ɫ��λͼ����ɾ�
	MemeDc.FillSolidRect(rect, RGB(0xFC, 0xFD, 0xF5));
	
	dc.BitBlt( rect.left, rect.top, rect.Width(), rect.Height(), &MemeDc, 0, 0, SRCCOPY);
	
	MemeDc.SelectObject(pOldBitmap);
	MemeDc.DeleteDC();
	bitmap.DeleteObject();
}

void CDlgTradeQuery::GridClickHoldDetailSelect()
{
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CGGTongApp *pTongApp = (CGGTongApp*)AfxGetApp();
	CMerch *pMerch = NULL;
	if (pTongApp && pTongApp->m_pDocument && pTongApp->m_pDocument->m_pAbsCenterManager && m_pHoldDetailMainSelect)
	{
		pMerch = pMainFrame->m_pTradeContainer->TradeCode2ClientMerch(m_pHoldDetailMainSelect->stock_code);

		if (pMainFrame && pMerch)
			pMainFrame->OnShowMerchInChart(pMerch,NULL);
		m_GridHoldDetailMain.SetFocus();
	}
}

void CDlgTradeQuery::GridClickHoldSummarySelect()
{
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CGGTongApp *pTongApp = (CGGTongApp*)AfxGetApp();
	CMerch *pMerch = NULL;
	if (pTongApp && pTongApp->m_pDocument && pTongApp->m_pDocument->m_pAbsCenterManager && m_pHoldSummarySelect)
	{
		pMerch = pMainFrame->m_pTradeContainer->TradeCode2ClientMerch(m_pHoldSummarySelect->stock_code);
		if (pMainFrame && pMerch)
			pMainFrame->OnShowMerchInChart(pMerch,NULL);
		m_GridHoldSummary.SetFocus();
	}
}

void CDlgTradeQuery::DealEntrustNo()
{
	// ģ�⽻����ÿ�β�ѯ���鷵��ʱ���д���޼۵��ͳֲֵ��е�ֹ��ֹӯ
	if (ETT_TRADE_FIRM == m_pTradeLoginInfo->eTradeLoginType)
	{
		return;
	}

	//
	double dPrice=0;	//ί�м۸�
	int32 iMerchCnt=0, iHoldCnt=0, iEntrustCnt=0;
	iMerchCnt = m_QuotationResultVector.size();
	QueryEntrustResultVector &aEntrust= m_pTradeBid->GetCacheEntrust();
	iEntrustCnt = aEntrust.size();
	QueryHoldDetailResultVector &aHold = m_pTradeBid->GetCacheHoldDetail();
	iHoldCnt = aHold.size();

	for (int32 i=0; i<iMerchCnt; i++)
	{
		T_RespQuote stQuote = m_QuotationResultVector[i];
		double dBuyPrice = 0;	//���
		double dSellPrice = 0;	//����
		CString dTradeUnit = "";//���׵�λ
		double dMarginRate =0; //��֤��ϵ��

		string strCode;
		UnicodeToUtf8(stQuote.stock_code, strCode);
		GetBuySellPrice(strCode.c_str(), dBuyPrice, dSellPrice, dTradeUnit,dMarginRate);//��ȡƽ�ּ۸�

		// ��ϳֲֵ��е�ֹ��ֹӯ
		vector<T_RespQueryHold>::iterator itrHold = aHold.begin();
		while (itrHold != aHold.end())
		{
			T_RespQueryHold stHold = *itrHold;
			dPrice = 0;
			bool32 bFind = false;
			if (stHold.stock_code == stQuote.stock_code)
			{
				if (_T("B") == stHold.entrust_bs)
				{
					if ((stHold.stop_loss>0.000001) && (dSellPrice<=stHold.stop_loss) && (dSellPrice>0.000001))
					{
						bFind = true;
						dPrice = stHold.stop_loss;
					}

					if ((stHold.stop_profit>0.000001) && (dSellPrice>=stHold.stop_profit))
					{
						bFind = true;
						dPrice = stHold.stop_profit;
					}
				}
				else
				{
					if ((stHold.stop_loss>0.000001) && (dBuyPrice>=stHold.stop_loss))
					{
						bFind = true;
						dPrice = stHold.stop_loss;
					}

					if ((stHold.stop_profit>0.000001) && (dBuyPrice<=stHold.stop_profit) && (dBuyPrice>0.000001))
					{
						bFind = true;
						dPrice = stHold.stop_profit;
					}
				}

				// ���������Ĵ��
				if (bFind)
				{
					// �ڳֲֵ���ɾ����һ��
					itrHold = aHold.erase(itrHold);

					// ���м�ί�е�
					CClientReqEntrust order;
					order.account = m_pTradeLoginInfo->StrUser;
					order.entrust_amount = stHold.hold_amount;

					if (_T("B") == stHold.entrust_bs)
					{
						order.entrust_bs = _T("S");
					}
					else
					{
						order.entrust_bs = _T("B");
					}

					CString strPrice = _T("0");
					strPrice.Format(_T("%.2f"), dPrice);
					order.entrust_price = strPrice;
					order.close_price = dPrice;
					order.entrust_prop = "0";
					order.entrust_type = "0";
					order.eo_flag = "2";
					order.mid_code = m_strMidCode;
					order.other_id = "";
					order.stock_code = stHold.stock_code;
					order.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
					order.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;
					order.hold_id = stHold.hold_id;
					order.dot_diff = 50;

					m_iReqEntrustCnt++;
					CString strTip = _T("");
					ReqEntrust(order, strTip);
				}
				else
				{
					itrHold++;
				}
			}
			else
			{
				itrHold++;
			}
		}

		// ����޼۵�
		vector<T_RespQueryEntrust>::iterator itrEntrust = aEntrust.begin();
		while (itrEntrust != aEntrust.end())
		{
			T_RespQueryEntrust stEntrust = *itrEntrust;

			bool32 bFind = false;
			dPrice = 0;
			if (stEntrust.stock_code == stQuote.stock_code)
			{
				if (_T("C") == stEntrust.entrust_bs) // ����
				{
					if (dBuyPrice <= stEntrust.entrust_price)
					{
						dPrice = stEntrust.entrust_price;
						bFind = true;
					}
				}
				else if (_T("E") == stEntrust.entrust_bs) // ����
				{
					if (dBuyPrice >= stEntrust.entrust_price)
					{
						dPrice = stEntrust.entrust_price;
						bFind = true;
					}
				}
				else if (_T("D") == stEntrust.entrust_bs) // ����
				{
					if (dSellPrice <= stEntrust.entrust_price)
					{
						dPrice = stEntrust.entrust_price;
						bFind = true;
					}
				}
				else if (_T("F") == stEntrust.entrust_bs) // ����
				{
					if (dSellPrice >= stEntrust.entrust_price)
					{
						dPrice = stEntrust.entrust_price;
						bFind = true;
					}
				}
			}

			// ���������Ĵ��
			if (bFind)
			{
				// ɾ����һ��
				itrEntrust = aEntrust.erase(itrEntrust);

				// �����м�ί�е�
				CClientReqEntrust order;
				order.account = m_pTradeLoginInfo->StrUser;
				order.entrust_amount = stEntrust.entrust_amount;
				order.entrust_bs = stEntrust.entrust_bs;
				CString strPrice = _T("0");
				strPrice.Format(_T("%.2f"), dPrice);
				order.entrust_price = strPrice;
				order.entrust_prop = "0";
				order.entrust_type = "0";
				order.eo_flag = "0";
				order.mid_code = m_strMidCode;
				order.other_id = "";
				order.stock_code = stEntrust.stock_code;
				order.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
				order.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;
				order.hold_id = "";
				order.stop_loss = stEntrust.stop_loss;
				order.stop_profit = stEntrust.stop_profit;

				m_iReqEntrustCnt++;
				CString strTip = _T("");
				ReqEntrust(order, strTip);

				// �ٳ���ָ�۵�
				m_iRevokeLimitOrderCnt++;
				RevokeLimitOrder(stEntrust.entrust_no);
			}
			else
			{
				itrEntrust++;
			}
		}
	}
}

void CDlgTradeQuery::RevokeAllLimitOrder()
{
	if (NULL==m_pTradeBid)
	{
		//ASSERT(0);
		return ;
	}

	CClientReqCancelEntrust order;
	order.account = m_pTradeLoginInfo->StrUser;
	order.mid_code = m_strMidCode;
	order.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
	order.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;
	order.entrust_no = _T("0"); // ��0ʱ��������ί�е�
	CString strTipMsg = _T("");
	m_pTradeBid->ReqCancelEntrust(order, strTipMsg);
}

void CDlgTradeQuery::RevokeAllProfitLoss()
{
	if (NULL==m_pTradeBid)
	{
		//ASSERT(0);
		return ;
	}

	CClientReqCancelStopLP reqStop;
	reqStop.account = m_pTradeLoginInfo->StrUser;
	reqStop.mid_code = m_strMidCode;
	reqStop.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
	reqStop.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;
	reqStop.hold_id = _T("0"); // ��0ʱ��������ֹӯֹ��
	CString strTipMsg = _T("");
	m_pTradeBid->ReqCancelStopLP(reqStop, strTipMsg);
}

void CDlgTradeQuery::SetGridHeadColor()
{
	if(m_pTradeLoginInfo->eTradeLoginType == ETT_TRADE_SIMULATE)
	{
		m_GridHoldDetail.GetDefaultCell(TRUE, FALSE)->SetBackClr(RGB(236, 239, 243));	// ��ͷ
		m_GridEntrust.GetDefaultCell(TRUE, FALSE)->SetBackClr(RGB(236, 239, 243));
		m_GridAccInfo.GetDefaultCell(TRUE, FALSE)->SetBackClr(RGB(0236, 239, 243));
		m_GridEntrustMain.GetDefaultCell(TRUE, FALSE)->SetBackClr(RGB(236, 239, 243));
		m_GridDeal.GetDefaultCell(TRUE, FALSE)->SetBackClr(RGB(236, 239, 243));
		m_GridHoldDetailMain.GetDefaultCell(TRUE, FALSE)->SetBackClr(RGB(236, 239, 243));
		m_GridAccInfoMain.GetDefaultCell(TRUE, FALSE)->SetBackClr(RGB(236, 239, 243));
		m_GridCommInfo.GetDefaultCell(TRUE, FALSE)->SetBackClr(RGB(236, 239, 243));
		m_GridHoldSummary.GetDefaultCell(TRUE, FALSE)->SetBackClr(RGB(236, 239, 243));
	}
	else
	{
		m_GridHoldDetail.GetDefaultCell(TRUE, FALSE)->SetBackClr(GRID_HEAD_COLOR);	// ��ͷ
		m_GridEntrust.GetDefaultCell(TRUE, FALSE)->SetBackClr(GRID_HEAD_COLOR);
		m_GridAccInfo.GetDefaultCell(TRUE, FALSE)->SetBackClr(GRID_HEAD_COLOR);
		m_GridEntrustMain.GetDefaultCell(TRUE, FALSE)->SetBackClr(GRID_HEAD_COLOR);
		m_GridDeal.GetDefaultCell(TRUE, FALSE)->SetBackClr(GRID_HEAD_COLOR);
		m_GridHoldDetailMain.GetDefaultCell(TRUE, FALSE)->SetBackClr(GRID_HEAD_COLOR);
		m_GridAccInfoMain.GetDefaultCell(TRUE, FALSE)->SetBackClr(GRID_HEAD_COLOR);
		m_GridCommInfo.GetDefaultCell(TRUE, FALSE)->SetBackClr(GRID_HEAD_COLOR);
		m_GridHoldSummary.GetDefaultCell(TRUE, FALSE)->SetBackClr(GRID_HEAD_COLOR);
	}
}