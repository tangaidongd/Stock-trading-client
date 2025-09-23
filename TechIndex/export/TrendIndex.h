#ifndef _TRENDINDEX_H
#define _TRENDINDEX_H

#include <float.h>
//#include "IoViewShare.h"
#include "ShareFun.h"
#include "CFormularContent.h"
#include "SelectStockStruct.h"
#include "TechExport.h"


//#define	STRONGTREND		0.6		// ǿ������ֵ
//#define	WEEKTREND		0.3		// ��������ֵ
//#define	STABILITYTREND	15		// �ȶ���
#define VALIDTIMES			3		// �ܹ����̵Ĳ����ϵ�ǰ������ʾ�Ĵ���
#define TRENDINTENSITYNUMS  13	    // ������ǿ�ȵ�ʱ��,ȡ�Ľڵ����
#define SECONDSTOUPDATE		10		// K �߽���ǰ���������ָ��ֵ
//#define MA5MOVENUMS		3		// MA5 ���ƶ������
//#define MA8MOVENUMS		5		// MA8 ���ƶ������
//#define MA13MOVENUMS		8		// MA13 ���ƶ������

//#define MAXMOVENUMS	    8		// ���ļ����(���漸��ֵ�����ֵ)

const CString gkStrTrendIndexFileName = _T("c:/TrendIndex.ini");

typedef struct T_TrendIndexParam
{
	float	m_fStrongTrend;		// ǿ������ֵ
	float	m_fWeekTrend;		// ��������ֵ
	float	m_fOCTrend;			// ����������ʾ�������ǿ��ֵ

	float	m_fStabilityTrend;	// �ȶ���
	
	int32	m_iMa5MoveNums;		// MA5 ���ƶ������
	int32	m_iMa8MoveNums;		// MA8 ���ƶ������
	int32	m_iMa13MoveNums;	// MA13 ���ƶ������

	float	m_fMA5Weight;		// MA5 Ȩ��
	float	m_fMA8Weight;		// MA8 Ȩ��
	float	m_fMA13Weight;		// MA13 Ȩ��

	int32	m_iMaxMoveNums;		// ���ļ����(���漸��ֵ�����ֵ)

	int32	m_iCdt5KLineNums;	// ����5 ��K �߸���
	float	m_fCdt5Times;		// ����5 �б���

}T_TrendIndexParam;

// ��ͼʱ�Ĳ����ṹ
typedef struct T_DrawTrendParam
{
	CPoint  m_CenterPoint;		// K �ߵ����ĵ�
	int32   m_iLow;				// K �ߵ��±߽�
	int32	m_iHigh;			// K �ߵ��ϱ߽�

}T_DrawTrendParam;

extern T_TrendIndexParam g_TrendIndexParam;
//
enum E_TrendIntensity			// �ڵ�ǿ��
{
	ETISTRONG = 0,				// ǿ
	ETINORMAL,					// һ��
	ETIWEEK,					// ��
};
//
enum E_HoldState				// �ֲ�״̬
{
	EHSNONE = 0,				// ��״̬,����
	EHSKCGW,					// �ղֹ���
	EHSDTCY,					// ��ͷ����
	EHSDTJC,					// ��ͷ����
	EHSKTCY,					// ��ͷ����
	EHSKTJC,					// ��ͷ����
	EHSCOUNT,
};

enum E_ActionPrompt				// ������ʾ
{
	EAPNONE = 0,				// ����ʾ
	EAPDTOC,					// ��ͷ����		
	EAPDTAC,					// ��ͷ�Ӳ�
	EAPDTRC,					// ��ͷ����	
	EAPDTCC,					// ��ͷ���	
	EAPKTOC,					// ��ͷ����
	EAPKTAC,					// ��ͷ�Ӳ�
	EAPKTRC,					// ��ͷ����
	EAPKTCC,					// ��ͷ���	
	EAPCOUNT,
};

typedef struct T_TrendIndexNode 
{
	E_HoldState		m_eHoldState;			// �ֲ� ״̬
	E_ActionPrompt  m_eActionPrompt;		// ���� ��ʾ
	float			m_fTrendIntensity;		// ���� ǿ��
	float			m_fTrendStability;		// ���� �ȶ���
	float			m_fMAWeighted;			// ���� MA ֵ(��ȨMA)
	float			m_fMAWeightedDif;		// ���� MA ��Ȩ��
	
	CTime			m_TimeKLine;			// ��Ӧ��K �߽ڵ��ʱ��
	CTime			m_TimeAction;			// ��ʾ���׵�ʱ��
	float			m_fPrice;				// ��Ӧ�ļ۸�
	float			m_fMA5;					// MA5 ��ֵ(ƫ�ƺ��)
	float			m_fMA8;					// MA8 ��ֵ
	float			m_fMA13;				// MA13 ��ֵ
	
	T_TrendIndexNode()
	{
		m_eHoldState		= EHSNONE;
		m_eActionPrompt		= EAPNONE;
		m_fTrendIntensity	= FLT_MAX;
		m_fTrendStability	= FLT_MAX;
		m_fMAWeighted		= FLT_MAX;
		m_fMAWeightedDif	= FLT_MAX;

		m_TimeKLine			= -1;
		m_TimeAction		= -1;
		m_fPrice			= FLT_MAX;		
		m_fMA5 				= FLT_MAX;
		m_fMA8				= FLT_MAX;
		m_fMA13				= FLT_MAX;
	};
	
}T_TrendIndexNode;

//Ҫ���Ƶ�����. 
class AFX_EXT_CLASS CNodeData
{
public:
	enum E_NodeDataFlag
	{
		KValueInvalid		= 0x00000001,			// ��Ч��ȡֵ
		KValueInhert		= 0x00000002,			// ������һ������
		KIdInvalid			= 0x00000004,			// ��Чid
		KSelect				= 0x00000008,			// ѡ��
		KValueDivide		= 0x00000010,			// �ָ�㣬��ֵ����ĵ㲻��õ����ӣ����ڶ��շ�ʱ��
		KDrawNULL			= 0x00000020,			// ��Ч�㣬ָ�깫ʽ��ָ����һЩ��, ��Щ����Ϊ��Ч, ��������, ���ߵ�ʱ��Ҳ�ж�
	};

	enum E_NodeColorFlag
	{
		ENCFUseDefault = 0xff000000,	// Ĭ��
		ENCFColorRise  = 0xff000001,	// ʹ����
		ENCFColorKeep  = 0xff000002,	// ʹ��ƽ
		ENCFColorFall  = 0xff000003,	// ʹ�õ�
	};

public:
	CNodeData();
	~CNodeData();

public:
	void	ClearValue( bool32 bClearID, bool32 bClearFlag);
	float   GetMax();
	float	GetMin();

	CNodeData(IN const CNodeData& NodeData);

public:
	uint32			m_iID;
	float			m_fOpen;
	float			m_fClose;
	float			m_fHigh;
	float			m_fLow;
	float			m_fAvg;
	float			m_fVolume;
	float			m_fAmount;
	float			m_fHold;

	uint32			m_iFlag;

	bool32			m_bClrValid;	// ��ɫ�Ƿ���Ч
	COLORREF		m_clrNode;		// node��ʹ�õ���ɫ��������λΪ0x0�������ʹ�ø���ɫ
	// 0xff�����loword��һ����־��0Ϊ��ǰ����ɫ��1Ϊ��ɫ��2Ϊƽ�̣�3Ϊ��
private:
	void Set ( IN const CNodeData& NodeData);
};

class EXPORT_CLASS CIndexNodeList
{
public:
	CIndexNodeList();
	CIndexNodeList(const CIndexNodeList &src);

	~CIndexNodeList();

public:
	CIndexNodeList& operator=(const CIndexNodeList &src);

public:
	CString	m_StrExtraData;
	CStringArray m_aStrExtraData;
	void* m_pCurve;// ��֮ǰ���������ж���ʱ��CChartCurve���ͣ�
	CArray<CNodeData, CNodeData&> m_aNodesFull;
	void	*m_pNodesShow;// ��֮ǰ���������ж���ʱ��CNodeSequence���ͣ�
};

struct EXPORT_CLASS T_IndexParam
{
public:
	T_IndexParam();
	~T_IndexParam();

public:
	static int32	sIndexId;

public:
	TCHAR				strIndexName[256];					// ָ������
	int32				id;									// 
	void				*pRegion;							// ����Region ��֮ǰ���������ж���ʱ��CChartRegion���ͣ�
	CFormularContent	*pContent;							// ָ�깫ʽ�ַ���
	bool32				bMainIndex;							// �Ƿ�����ָ��, false ���ǵ���ָ��	
	bool32				bScreenAccroding;					// �Ƿ����Ļ��ǰ��ʾ��K����أ���ʾK�߱仯��Ҫ���¼���
	///////////////////////////////////////////////////
	// ȫ��������
	CArray<CIndexNodeList, CIndexNodeList&> m_aIndexLines;

	// ��ǰ��Ļ��ʾ��������ԭʼ������λ�ã� ͨ������������NodesArray
	int32			m_iIndexShowPosInFullList;					// ��ǰ��Ļ��ʾ����ʼλ��
	int32			m_iIndexShowCountInFullList;	
	bool32			m_bRight;								// ��ǰָ���Ƿ���Ȩ��
};

float	 AFX_EXT_API 	GetMax(float f1,float f2,float f3);
float	 AFX_EXT_API 	GetMin(float f1,float f2,float f3);
float	 AFX_EXT_API 	GetMiddle(float f1,float f2,float f3);

void	 AFX_EXT_API 	InitialTrendIndexParam();																// ��ʼ��ָ��Ĳ���
void	 AFX_EXT_API 	SetTrendIndexParam(float fStrongTrend,float fWeekTrend,float fOCTrend,float fStabilityTrend,int32 iMa5MoveNums,int32 iMa8MoveNums,int32 iMa13MoveNums,float fMa5Weight,float fMa8Weight,float fMa13Weight,int32 iCdt5KLineNums,float fCdt5Times); // ���ñ���ָ�����

// ȡ����ֵ
float AFX_EXT_API GetStrongTrend();																		// ǿ������ֵ
float AFX_EXT_API 		GetWeekTrend();																			// ��������ֵ
float AFX_EXT_API 		GetOCTrend();																			// ���������жϵ�����ֵ
float AFX_EXT_API 		GetStabilityTrend();																	// �ȶ���ֵ
int32 AFX_EXT_API 		GetMA5MoveNums();																		// MA5 ƫ��
int32 AFX_EXT_API 		GetMA8MoveNums();																		// MA8 ƫ��	
int32 AFX_EXT_API 		GetMA13MoveNums();																		// MA13 ƫ��			
int32 AFX_EXT_API 		GetMaxMoveNums();																		// ���ƫ��
float AFX_EXT_API 		GetMA5Weight();																			// MA5 Ȩ��
float AFX_EXT_API 		GetMA8Weight();																			// MA8 Ȩ��	
float AFX_EXT_API 		GetMA13Weight();																		// MA13 Ȩ��
int32 AFX_EXT_API 		GetCdt5KLineNums();																		// ����5 ��K�߸���
float AFX_EXT_API 		GetCdt5Times();																			// ����5 �еı���
//		
bool32 AFX_EXT_API 		BeStability(const T_TrendIndexNode& Node,const T_TrendIndexNode& NodePre);				// �ж��ȶ���
E_TrendIntensity  AFX_EXT_API  BeStrong(const T_TrendIndexNode& Node);											// �ж�ǿ��	
//
float  AFX_EXT_API 		GetWeightedMAValue(float fMA5,float fMA8,float fMA13);									// �õ���ȨMA ֵ
CString  AFX_EXT_API 		GetActionPromptString(E_ActionPrompt eActionPrompt,COLORREF& clr);						// �õ��ַ���,˳���һ����ɫ
CString	 AFX_EXT_API 	GetActionPromptString(E_ActionPrompt eActionPrompt);									// �õ�������ʾ�ַ���
CString	 AFX_EXT_API 	GetHoldStateString(E_HoldState eHoldState);												// �ֲ�״̬�ַ���
CString	 AFX_EXT_API 	GetIntensityString(const T_TrendIndexNode& Node);										// ǿ���ַ���
CString	 AFX_EXT_API 	GetStabilityString(const T_TrendIndexNode& Node,const T_TrendIndexNode& NodePre);		// �ȶ����ַ���
CString	 AFX_EXT_API 	GetTrendUpDownString(const T_TrendIndexNode& Node);										// �����ַ���
CString  AFX_EXT_API   GetNodeSummary(const T_TrendIndexNode& Node,const T_TrendIndexNode& NodePre);			// ����һ���ڵ���ַ���
//
float	 AFX_EXT_API 	CalcWeightMAWeightDif(int32 iPos, const CArray<T_TrendIndexNode, T_TrendIndexNode>& aNodeValues); // ���Ȩ MA ��ֵ
//float		CalcIntensityVaule(float fMANow,float fMAPre,float fMAMax,float fMAMin);				// ��ǿ��ֵ(�ϵ��㷨)
float	 AFX_EXT_API 	CalcIntensityVauleNew(int32 iPos, float fEMAWeightedDif, T_IndexOutArray* pIndexEMA, const CArray<CKLine,CKLine>& aKLines);				// ��ǿ��ֵ(�µ�)
float	 AFX_EXT_API 	CalcStabilityValue(float fIntensityNow,float fIntensityPre);							// ���ȶ���ֵ	
//
void	 AFX_EXT_API 	SortChoice(float* aValues,int32 iLength,OUT float& fMax,OUT float& fMin);				// ѡ������
bool32	 AFX_EXT_API	GetBigCyleNode(IN const CTime& TimeNow,IN const CArray<T_TrendIndexNode,T_TrendIndexNode>& aBigCyleNodes,IN CTimeSpan TimeSpanBigCyle,OUT T_TrendIndexNode& NodeBigCyle); // �õ��뱾�����Ӧ�Ĵ����ڵĽڵ�

// �жϽ�����:
bool32	 AFX_EXT_API 	BeLastTradeDayInaMounth(const CGmtTime& Time);												// �Ƿ��ǵ����������
bool32	 AFX_EXT_API 	BeLastTradeDayInaQuarter(const CGmtTime& Time);												// �Ƿ��ǵ����������
bool32	 AFX_EXT_API 	BeLastTradeDayInaYear(const CGmtTime& Time);												// �Ƿ��ǵ����������
	
// ͳ�ƺ���
bool32	 AFX_EXT_API 	StatisticaTrendValues( IN int32 iKLineNums,													// ����ٸ�K ��
								   OUT int32& iKLineNumsReal,											// ʵ�����˶��ٸ�
								   IN const CArray<T_TrendIndexNode,T_TrendIndexNode>& aTrendValuesOUT, // ����ָ��ڵ�����
								   OUT int32& iTradeTimes,												// �ܽ��״���		
								   OUT int32& iGoodTrades,												// ӯ������
								   OUT int32& iBadTrades,											    // ������	
								   OUT float& fAccuracyRate,											// ׼ȷ��
								   OUT float& fProfitability);

bool32	 AFX_EXT_API 	StatisticaTrendValues2( IN const CArray<T_TrendIndexNode,T_TrendIndexNode>& aTrendValuesOUT, // ����ָ��ڵ�����
								    OUT int32& iTradeTimes,												// �ܽ��״���		
								    OUT int32& iGoodTrades,												// ӯ������
								    OUT int32& iBadTrades,											    // ������	
								    OUT float& fAccuracyRate,											// ׼ȷ��
								    OUT float& fProfitability);
#endif