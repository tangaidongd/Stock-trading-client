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
//	float 	Price;      // �۸� 
//	float 	Volume;     // �� 
//	float 	Amount;     // ��    
//}Kdata1;
//
//
////��Ʊ�ڰ�������Խṹ
//typedef struct
//{
//	char m_szSymbol[9];	//   ��Ʊ����
//	BOOL m_bDeleted;
//	int m_iPos;
//	float m_ClosePrice;
//	float m_fRight[MaxRights];     //   ��ƱȨ��
//	BYTE  m_btStockType[MaxStockTYpe];    
//}STOCK_TYPE_INFO;
//typedef STOCK_TYPE_INFO *PSTOCK_TYPE_INFO;
//
//
////��Ȩ���ݽṹ
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
////��Ʊ�����������ݽṹ
//typedef struct
//{
//	char Symbol[10]; //��Ʊ����
//	int  NumSplit;   //�Ѿ���Ȩ����
//	float zgb;       //�ܹɱ�(���)
//	float gjg;       //���ҹ�(���)
//	float fqrfrg;    //�����˷��˹�(���)
//	float frg;       //���˹�(���)
//	float zgg;       //ְ����(���)
//	float gzAg;      //����A��(���)    5
//	float ltAg;      //��ͨA��(���)
//	float Hg;        //�ȹ�(���)
//	float Bg;        //B��(���)
//	float zpg;       //ת���(���)
//
//	float zzc;       //���ʲ�(��Ԫ)    10
//	float ldzc;      //�����ʲ�(��Ԫ)
//	float cqtz;      //����Ͷ��(��Ԫ)
//	float gdzc;      //�̶��ʲ�(��Ԫ)
//	float wxzc;      //�����ʲ�(��Ԫ)
//	float ldfz;      //������ծ(��Ԫ)   15
//	float cqfz;      //���ڸ�ծ(��Ԫ)
//	float ggqy;      //�ɶ�Ȩ��(��Ԫ)
//	float zbgjj;     //�ʱ�������(��Ԫ)
//	float yygjj;     //ӯ�๫����(��Ԫ)
//	float mgjz;      //ÿ�ɾ�ֵ(Ԫ)    20
//	float gdqybl;    //�ɶ�Ȩ�����(%)
//	float mggjj;     //ÿ�ɹ�����(Ԫ)
//
//	float zyywsr;    //��Ӫҵ������(��Ԫ)
//	float zyywlr;    //��Ӫҵ������(��Ԫ)
//	float qtywlr;    //����ҵ������(��Ԫ)25
//	float lrze;      //�����ܶ�(��Ԫ)
//	float jlr;       //������(��Ԫ)
//	float wfplr;     //δ��������(��Ԫ)
//	float mgsy;      //ÿ������(Ԫ)
//	float jzcsyl;    //���ʲ�������(%)  30
//	float mgwfplr;   //ÿ��δ��������(Ԫ)
//
//	float mgjzc;     //ÿ�ɾ��ʲ�(Ԫ)
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
////CReportData�ṹ˵��
////CReportData�ṹ��Ҫ���ڼ��ռ�ʱ��Ʊ������Ϣ
//typedef struct
//{
//	BYTE kind;
//	BYTE  rdp;           //      ���ڼ������ǿ��ָ��
//	char id[8];             //      ֤ȯ����
//	char name[10];           //     ֤ȯ���� 
//	char Gppyjc[6];         //      ֤ȯ����ƴ�����
//	int sel;
//	float ystc;              //      ��������
//	float opnp;              //      ���տ��� 
//	float higp;              //      �������
//	float lowp;              //      �������
//	float nowp;              //      ���¼۸�
//	float nowv;              //      ���³ɽ���
//	float totv;              //      �ܳɽ���
//	float totp;              //      �ܳɽ����
//	float pbuy1;             //      �����һ
//	float vbuy1;             //      ������һ
//	float pbuy2;             //      ����۶�  
//	float vbuy2;             //      ��������
//	float pbuy3;             //      ������� 
//	float vbuy3;             //      ��������      
//	float pbuy4;             //      ������� 
//	float vbuy4;             //      ��������      
//	float pbuy5;             //      ������� 
//	float vbuy5;             //      ��������      
//	float psel1;             //      ������һ
//	float vsel1;             //      ������һ
//	float psel2;             //      �����۶� 
//	float vsel2;             //      ��������   
//	float psel3;             //      ��������
//	float vsel3;             //      �������� 
//	float psel4;             //      ��������
//	float vsel4;             //      �������� 
//	float psel5;             //      ��������
//	float vsel5;             //      �������� 
//	float accb;              //      ����ҵ��ͣ���������ҵ��ͣ�
//	float accs;              //      �����ҵ��ͣ����������ҵ��ͣ�
//	float volume5;           //      5�����
//	float rvol;              //      ����
//	float dvol;              //      ����
//
//	short lastclmin;         //      �ϴδ��������
//	short initdown;          //      ��Ʊ��ʼ����־   0 = δ�ܳ�ʼ����1=�ѳ�ʼ��   
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
//	int	m_serialNumber;		//		���ջ����ļ��й�Ʊ���ݵ����к�
//} CReportData;
//typedef CReportData *PCdat1;


typedef union tagKlineEx
{
	struct
	{
		float buyPrice[5];	//��1--��3��
		float buyVol[5];		//��1--��3��
		float sellPrice[5];	//��1--��3��	
		float sellVol[5];		//��1--��3��
	};
	float fDataEx[20];			//����
} KlineEx;

//
//typedef struct tagINPUT_INFO
//{
//	char*			  strSymbol;		//��Ʊ����
//	bool			  bIndex;				//����
//
//	int			  nNumData;		//��������(pData,pDataEx,pResultBuf��������)
//	Kline*		  pData;					//��������,ע��:��m_nNumData==0ʱ����Ϊ NULL
//	KlineEx*	      pDataEx;				//��չ����,�ֱʳɽ�������,ע��:����Ϊ NULL
//
//	ARRAY_BE 		fInputParam[10]	;//���ò���
//	ARRAY_BE 		fOutputParam;	//�������
//	klineKind	  klineType;			//��������
//	BASEINFO*    financeData;			//��������
//	CReportData* pDat1;//��������
//	int nReserved;
//	void* pBuySellList;
//	int m_stkKind;
//} INPUT_INFO;


#ifndef _TAG_INPUT_INFO
#define _TAG_INPUT_INFO
typedef struct tagINPUT_INFO
{
	char*			strSymbol;			// ��Ʊ����
	bool			bIndex;				// ����

	int				nNumData;			// ��������(pData,pDataEx,pResultBuf��������)
	Kline*			pData;				// ��������,ע��:��m_nNumData==0ʱ����Ϊ NULL

	ARRAY_BE 		fInputParam[10];	// ���ò���
	ARRAY_BE 		fOutputParam;		// �������
	klineKind		klineType;			// ��������
} INPUT_INFO;
#endif


// �������ڵĽṹ
typedef struct tagCycle_INFO
{
	float fData;
	int iIndex;

	// ���ز�����  
	bool operator<(const tagCycle_INFO& rhs) { return  fData - rhs.fData < 1e-7; };  
	bool operator>(const tagCycle_INFO& rhs) { return fData - rhs.fData > 1e-7;};  
	bool operator==(const tagCycle_INFO& rhs) { return fabs(fData - rhs.fData) < 1e-7;}  

} ST_CYCLE_INFO;


// ÿ��ҵ�������¼
class DATACENTER_DLL_EXPORT CSpecialTechIndexManager
{
public:
	CSpecialTechIndexManager();
	~CSpecialTechIndexManager();
public:

	//ָ��
	//21:FLOATPERCENT 22:FLOATTAPE 44/winner  45:cost
	int WinnerFun(INPUT_INFO *pInput, CMerch *pCMerch, int nFunc = 44);	// ����Winner

	// �������ú���
	
public:
	static float GetCapital(CMerch *pCMerch);						// ��ǰ��ͨ�ɱ����֣�
	static float GetTotalCapital(CMerch *pCMerch);					// ��ǰ�ܹɱ����֣�
	static int32 GetHyzsCode(CMerch *pCMerch);					    // ������ҵ�İ��ָ������
	void CSpecialTechIndexManager::GetExpData(CMerch *pMerch, int32 iMinUser, int32 iDayUser, E_NodeTimeInterval eTimeIntervalFull, CArray<CKLine, CKLine> * pKLine);						// ��ȡ��������+
	static int32 GetTopRange(INPUT_INFO *pInput);					    // ��ǰֵ�ǽ����������ڵ����ֵ
	static int32 GetLowRange(INPUT_INFO *pInput);					    // ��ǰֵ�ǽ����������ڵ���Сֵ
	static int32 GetFindHighOrLow(INPUT_INFO *pInput, bool bIsHigh = true);	// N����ǰM�����ڵĵ�T�����ֵ����Сֵ
	static int32 GetFindHighOrLowBars(INPUT_INFO *pInput, bool bIsHigh = true);	 // N����ǰM�����ڵĵ�T�����ֵ����Сֵ����ǰ���ڵ�������

	



private:
	static void GetMaxMin(Kline *pKline, int nKline, float &fMax, float &fMin);
	static int  GetRuler(float fMax, float fMin,float& fRuler);

	bool32 GetTimeIntervalInfo(int32 iMinUser, int32 iDayUser, E_NodeTimeInterval eNodeTimeInterval, E_NodeTimeInterval &eNodeTimeIntervalCompare, E_KLineTypeBase &eKLineTypeCompare,int32 &iScale);

	//float GetCapital(CReportData* pdt);
	
	static void GetMoveCBPerFoot(Kline* pKline,float* cnp,float ltp,float fMax,float fMin,int nFoot = 0,float fRate=1);
};

#endif
