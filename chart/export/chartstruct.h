#ifndef __CHART_STRUCT_XKDJKJS_20150831_HH__
#define __CHART_STRUCT_XKDJKJS_20150831_HH__
//#include "EngineCenterBase.h"
#include "TimeInterval.h"
#include "TrendIndex.h"
#include "chartexport.h"

class CNodeSequence;

#define  IMAGEID_NOUSEIMAGE				(UINT) 0   // ����ͼ��
#define  IOVIEW_TITLE_HEIGHT			(int32)18  // �������߶�

#define  IOVIEW_TITILE_LEFT_BUTTON_ID	(int32)-1	// ���ͷID
#define  IOVIEW_TITILE_RIGHT_BUTTON_ID	(int32)-2	// �Ҽ�ͷID
#define  IOVIEW_TITLE_ADD_BUTTON_ID		(int32)-3	// ��Ӱ�ťID
#define  IOVIEW_TITLE_DEL_BUTTON_ID		(int32)-4	// ɾ����ťID
#define  IOVIEW_TITLE_F7_BUTTON_ID		(int32)-5	// F7 ��ťID

class CNodeSequence;
class CChartRegion;
// ����ĳһ��������ʾ�����Ϣ, ��������ͼ������
CHART_EXPORT void KLine2NodeData ( const CKLine& KLine, CNodeData& NodeData);
CHART_EXPORT CString TimeInterval2String ( E_NodeTimeInterval eTimeInterval ,UINT uiUserMinutes = 0 , UINT uiUserDays = 0);
class CHART_EXPORT CTrendTradingDayInfo
{
public:
	CTrendTradingDayInfo();

public:
	void			Set(IN CMerch *pMerch, const CMarketIOCTimeInfo &MarketIOCTime);
	void			RecalcPrice(const CMerch &Merch);

	float			GetPrevReferPrice();		// ȡ��һ�����ղο��۸���ֵ�� �����ڻ��������ۣ� ����������Ʒ������

	void			RecalcHold(const CMerch &Merch);
	float			GetPrevHold();

public:
	bool32			m_bInit;	
	bool32			m_bNeedRecalc;				// ��Щ�ֶ�ֵû��,���ܲ�׼,��Ҫ����.

	// 
	CMerch			*m_pMerch;
	CMarketIOCTimeInfo	m_MarketIOCTime;		// �����ڽ���ʱ��

	float			m_fPricePrevClose;			// ����
	float			m_fPricePrevAvg;			// �����
	float			m_fHoldPrev;				// ǰ�ֲ�

	//
	CGmtTime		m_TimeLastUpdate;			// ��һ�θ��µ�ʱ��
};

//ͬһ��Merch��Nodes��UserData
struct CHART_EXPORT T_MerchNodeUserData
{
public:
	T_MerchNodeUserData(/*CViewData* p*/);
	~T_MerchNodeUserData();

public:
	bool32			UpdateShowData(int32 iShowPosInFullList, int32 iShowCountInFullList);

public:
	CMerch			*m_pMerchNode;

	//////////////////////////////////////////////////////////
	// ԭʼ����
	// ȫ��������
	CArray<CTick, CTick>	m_aTicksCompare;
	CArray<CKLine, CKLine>	m_aKLinesCompare;		// ��ǰ��Ļ��ʾ��k��ԭʼ���ݣ� ���ǵ���ʵ��ʾ�����ݣ� �п��ܻ�Ҫ���мӹ��� ��������ͼ����1����K�ߵĻ�������������
	E_NodeTimeInterval		m_eTimeIntervalCompare;	// ��ǰ��Ļ��ʾ���ݵ�ԭʼ�������ͣ� ���ǵ�15����������5������ƴ������

	CArray<CKLine, CKLine>	m_aKLinesFull;			// ��������ȫ�����ݣ� ���磺�ϲ�N�����ߣ� ���磺��ͼ�������ͼ���ܴ��ڵĸ���ڵ���ɾ���
	E_NodeTimeInterval		m_eTimeIntervalFull;	// 

	CArray<DWORD, DWORD>	m_aKLinesShowDataFlag;	// ����ʾ����ͬ��[PosInFullList=��ߵ�0���]�ģ���־��node�ڵ��־������, ��ͬ������ʹ�ø����飬��������������	xl 0106

	// ��ǰ��Ļ��ʾ��������ԭʼ������λ�ã� ͨ������������m_pKLinesShow
	int32			m_iShowPosInFullList;			// ��ǰ��Ļ��ʾ����ʼλ��
	int32			m_iShowCountInFullList;	

	// ��ǰ��Ļ��ʾ��������
	CNodeSequence	*m_pKLinesShow;					// ��ǰ��Ļ��ʾ��K��

	///////////////////////////////////////////////////////////
	// ����ָ������
	CArray<T_IndexParam*, T_IndexParam*>	aIndexs;	// ָ��������

	///////////////////////////////////////////////////////////
	// 
	int32			m_iTimeUserMultipleMinutes;		// �Զ������������
	int32			m_iTimeUserMultipleDays;		// �Զ�����������

	bool32			bMainMerch;
	bool32			bUpdateOnce;
	bool32			bNew;					// MainMerch���´�����.

public:
	// ��������ͼ������˼
	CTrendTradingDayInfo	m_TrendTradingDayInfo;

private:
	//CViewData*		m_pAbsCenterManager;
};


typedef struct T_FuctionButton 
{
public:
	int32		m_uID;
	UINT		m_uiBmpNormal;
	UINT		m_uiBmpFocus;
	UINT		m_uiBmpSelf;

	CString		m_StrText;
	CString		m_StrTips;

	T_FuctionButton(const CString& StrText, const CString& StrTips, int32 uID,  UINT uiBmpNormal, UINT uiBmpFocus, UINT uiBmpSelf)
	{
		m_uID	  = uID;		
		m_StrText = StrText;
		m_StrTips = StrTips;

		m_uiBmpNormal = uiBmpNormal;
		m_uiBmpFocus  = uiBmpFocus;
		m_uiBmpSelf   = uiBmpSelf;
	};

}T_FuctionButton;

// ÿ����ͼ����Ϣ
typedef struct T_SubRegionParam
{
public:
	T_SubRegionParam()
	{
		m_pSubRegionMain	= NULL;
		m_pSubRegionYLeft	= NULL;
		m_pSubRegionYRight	= NULL;
	}

public:
	CChartRegion*	m_pSubRegionMain;
	CChartRegion*	m_pSubRegionYLeft;
	CChartRegion*	m_pSubRegionYRight;
} T_SubRegionParam;

typedef struct T_KLineID
{
	int32 iNum;
	CKLine KLine0;
	CKLine KLine1;
} T_KLineID;

typedef struct T_IndexPostAdd
{
	CString StrIndexName;
	CChartRegion* pRegion;
	int id;
} T_IndexPostAdd;
#endif