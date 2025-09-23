#ifndef _SPECIAL_TECH_INDEX_MANAGER_H
#define _SPECIAL_TECH_INDEX_MANAGER_H
#include "StructKlineView.h"
#include "IndexExtraData.h"
#include "MerchManager.h"
#include "BlockConfig.h"
#include "TimeInterval.h"
#include "MerchManager.h"



#ifndef SPECIAL_TECH_INDEX_MANAGER
#define DATACENTER_DLL_EXPORT __declspec(dllimport)
#else
#define DATACENTER_DLL_EXPORT __declspec(dllexport)
#endif


#define Max_MoveChenBen	200000
#define MaxRights 3
#define MaxStockTYpe 102
#define MAXJISHUCOUNT 10

//
//
//typedef struct
//{
//	float 	Price;      // 价格 
//	float 	Volume;     // 量 
//	float 	Amount;     // 额    
//}Kdata1;
//
//
////股票在板块中属性结构
//typedef struct
//{
//	char m_szSymbol[9];	//   股票代码
//	BOOL m_bDeleted;
//	int m_iPos;
//	float m_ClosePrice;
//	float m_fRight[MaxRights];     //   股票权重
//	BYTE  m_btStockType[MaxStockTYpe];    
//}STOCK_TYPE_INFO;
//typedef STOCK_TYPE_INFO *PSTOCK_TYPE_INFO;
//
//
////除权数据结构
//typedef struct 
//{
//	int   nFlags;
//	int	  nTime;
//	float Give;
//	float Allocate;
//	float AllocatePrice;
//	float Bonus;
//	BYTE Free[4];
//}Split ,*PSplit;
//
//
////
//
////////////////////////////////////////////////////////////////////////////
////股票基本资料数据结构
//typedef struct
//{
//	char Symbol[10]; //股票代码
//	int  NumSplit;   //已经除权次数
//	float zgb;       //总股本(万股)
//	float gjg;       //国家股(万股)
//	float fqrfrg;    //发起人法人股(万股)
//	float frg;       //法人股(万股)
//	float zgg;       //职工股(万股)
//	float gzAg;      //公众A股(万股)    5
//	float ltAg;      //流通A股(万股)
//	float Hg;        //Ｈ股(万股)
//	float Bg;        //B股(万股)
//	float zpg;       //转配股(万股)
//
//	float zzc;       //总资产(万元)    10
//	float ldzc;      //流动资产(万元)
//	float cqtz;      //长期投资(万元)
//	float gdzc;      //固定资产(万元)
//	float wxzc;      //无形资产(万元)
//	float ldfz;      //流动负债(万元)   15
//	float cqfz;      //长期负债(万元)
//	float ggqy;      //股东权益(万元)
//	float zbgjj;     //资本公积金(万元)
//	float yygjj;     //盈余公积金(万元)
//	float mgjz;      //每股净值(元)    20
//	float gdqybl;    //股东权益比率(%)
//	float mggjj;     //每股公积金(元)
//
//	float zyywsr;    //主营业务收入(万元)
//	float zyywlr;    //主营业务利润(万元)
//	float qtywlr;    //其它业务利润(万元)25
//	float lrze;      //利润总额(万元)
//	float jlr;       //净利润(万元)
//	float wfplr;     //未分配利润(万元)
//	float mgsy;      //每股收益(元)
//	float jzcsyl;    //净资产收益率(%)  30
//	float mgwfplr;   //每股未分配利润(元)
//
//	float mgjzc;     //每股净资产(元)
//
//	int m_iPos;
//	BYTE  free[8];
//	Split m_Split[80];
//}BASEINFO;
//typedef BASEINFO *PBASEINFO;

//
//
//

//
////======================
////CReportData结构说明
////CReportData结构主要用于即日即时股票行情信息
//typedef struct
//{
//	BYTE kind;
//	BYTE  rdp;           //      用于计算大盘强弱指标
//	char id[8];             //      证券代码
//	char name[10];           //     证券名称 
//	char Gppyjc[6];         //      证券名称拼音简称
//	int sel;
//	float ystc;              //      昨天收盘
//	float opnp;              //      今日开盘 
//	float higp;              //      今日最高
//	float lowp;              //      今日最低
//	float nowp;              //      最新价格
//	float nowv;              //      最新成交量
//	float totv;              //      总成交量
//	float totp;              //      总成交金额
//	float pbuy1;             //      买入价一
//	float vbuy1;             //      买入量一
//	float pbuy2;             //      买入价二  
//	float vbuy2;             //      买入量二
//	float pbuy3;             //      买入价三 
//	float vbuy3;             //      买入量三      
//	float pbuy4;             //      买入价三 
//	float vbuy4;             //      买入量三      
//	float pbuy5;             //      买入价三 
//	float vbuy5;             //      买入量三      
//	float psel1;             //      卖出价一
//	float vsel1;             //      卖出量一
//	float psel2;             //      卖出价二 
//	float vsel2;             //      卖出量二   
//	float psel3;             //      卖出价三
//	float vsel3;             //      卖出量三 
//	float psel4;             //      卖出价三
//	float vsel4;             //      卖出量三 
//	float psel5;             //      卖出价三
//	float vsel5;             //      卖出量三 
//	float accb;              //      买入挂单和（三种买入挂单和）
//	float accs;              //      卖出挂单和（三种卖出挂单和）
//	float volume5;           //      5天均量
//	float rvol;              //      外盘
//	float dvol;              //      内盘
//
//	short lastclmin;         //      上次处理分钟数
//	short initdown;          //      股票初始化标志   0 = 未能初始化，1=已初始化   
//	int  InOut;
//	BOOL  IsDelete;
//	BOOL  IsMxTj;
//
//	float Index[10];
//	Kdata1   m_Kdata1[240];
//	BASEINFO *pBaseInfo;
//
//	STOCK_TYPE_INFO *pStockTypeInfo;
//
//	int	m_serialNumber;		//		当日换手文件中股票数据的序列号
//} CReportData;
//typedef CReportData *PCdat1;


typedef union tagKlineEx
{
	struct
	{
		float buyPrice[5];	//买1--买3价
		float buyVol[5];		//买1--买3量
		float sellPrice[5];	//卖1--卖3价	
		float sellVol[5];		//卖1--卖3量
	};
	float fDataEx[20];			//保留
} KlineEx;

//
//typedef struct tagINPUT_INFO
//{
//	char*			  strSymbol;		//股票代码
//	bool			  bIndex;				//大盘
//
//	int			  nNumData;		//数据数量(pData,pDataEx,pResultBuf数据数量)
//	Kline*		  pData;					//常规数据,注意:当m_nNumData==0时可能为 NULL
//	KlineEx*	      pDataEx;				//扩展数据,分笔成交买卖盘,注意:可能为 NULL
//
//	ARRAY_BE 		fInputParam[10]	;//调用参数
//	ARRAY_BE 		fOutputParam;	//输出参数
//	klineKind	  klineType;			//数据类型
//	BASEINFO*    financeData;			//财务数据
//	CReportData* pDat1;//行情数据
//	int nReserved;
//	void* pBuySellList;
//	int m_stkKind;
//} INPUT_INFO;


#ifndef _TAG_INPUT_INFO
#define _TAG_INPUT_INFO
typedef struct tagINPUT_INFO
{
	char*			strSymbol;			// 股票代码
	bool			bIndex;				// 大盘

	int				nNumData;			// 数据数量(pData,pDataEx,pResultBuf数据数量)
	Kline*			pData;				// 常规数据,注意:当m_nNumData==0时可能为 NULL

	ARRAY_BE 		fInputParam[10];	// 调用参数
	ARRAY_BE 		fOutputParam;		// 输出参数
	klineKind		klineType;			// 数据类型
} INPUT_INFO;
#endif


// 计算周期的结构
typedef struct tagCycle_INFO
{
	float fData;
	int iIndex;

	// 重载操作符  
	bool operator<(const tagCycle_INFO& rhs) { return  fData - rhs.fData < 1e-7; };  
	bool operator>(const tagCycle_INFO& rhs) { return fData - rhs.fData > 1e-7;};  
	bool operator==(const tagCycle_INFO& rhs) { return fabs(fData - rhs.fData) < 1e-7;}  

} ST_CYCLE_INFO;


// 每笔业务请求记录
class DATACENTER_DLL_EXPORT CSpecialTechIndexManager
{
public:
	CSpecialTechIndexManager();
	~CSpecialTechIndexManager();
public:

	//指标
	//21:FLOATPERCENT 22:FLOATTAPE 44/winner  45:cost
	int WinnerFun(INPUT_INFO *pInput, CMerch *pCMerch, int nFunc = 44);	// 函数Winner

	// 公共调用函数
	
public:
	static float GetCapital(CMerch *pCMerch);						// 当前流通股本（手）
	static float GetTotalCapital(CMerch *pCMerch);					// 当前总股本（手）
	static int32 GetHyzsCode(CMerch *pCMerch);					    // 所属行业的板块指数代码
	void CSpecialTechIndexManager::GetExpData(CMerch *pMerch, int32 iMinUser, int32 iDayUser, E_NodeTimeInterval eTimeIntervalFull, CArray<CKLine, CKLine> * pKLine);						// 获取大盘数据+
	static int32 GetTopRange(INPUT_INFO *pInput);					    // 当前值是近多少周期内的最大值
	static int32 GetLowRange(INPUT_INFO *pInput);					    // 当前值是近多少周期内的最小值
	static int32 GetFindHighOrLow(INPUT_INFO *pInput, bool bIsHigh = true);	// N周期前M周期内的第T个最大值或最小值
	static int32 GetFindHighOrLowBars(INPUT_INFO *pInput, bool bIsHigh = true);	 // N周期前M周期内的第T个最大值或最小值到当前周期的周期数

	



private:
	static void GetMaxMin(Kline *pKline, int nKline, float &fMax, float &fMin);
	static int  GetRuler(float fMax, float fMin,float& fRuler);

	bool32 GetTimeIntervalInfo(int32 iMinUser, int32 iDayUser, E_NodeTimeInterval eNodeTimeInterval, E_NodeTimeInterval &eNodeTimeIntervalCompare, E_KLineTypeBase &eKLineTypeCompare,int32 &iScale);

	//float GetCapital(CReportData* pdt);
	
	static void GetMoveCBPerFoot(Kline* pKline,float* cnp,float ltp,float fMax,float fMin,int nFoot = 0,float fRate=1);
};

#endif
