// IndexExtraData.h: interface for the CIndexExtraData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INDEXEXTRADATA_H__)
#define AFX_INDEXEXTRADATA_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MaxRights 3 
#define MaxStockTYpe 102 

//////////////////////////////////////////////////////////////////////////
//
typedef struct
{
	float 	Price;      // �۸� 
	float 	Volume;     // �� 
	float 	Amount;     // ��    
}Kdata1;
//////////////////////////////////////////////////////////////////////////
//��Ȩ���ݽṹ
typedef struct 
{
	int   nFlags;
	int	  nTime;
	float Give;
	float Allocate;
	float AllocatePrice;
	float Bonus;
	BYTE Free[4];
}Split ,*PSplit;
//////////////////////////////////////////////////////////////////////////
//��Ʊ�����������ݽṹ
typedef struct
{
	char Symbol[10]; //��Ʊ����
	int  NumSplit;   //�Ѿ���Ȩ����
	float zgb;       //�ܹɱ�(���)
	float gjg;       //���ҹ�(���)
	float fqrfrg;    //�����˷��˹�(���)
	float frg;       //���˹�(���)
	float zgg;       //ְ����(���)
	float gzAg;      //����A��(���)    5
	float ltAg;      //��ͨA��(���)
	float Hg;        //�ȹ�(���)
	float Bg;        //B��(���)
	float zpg;       //ת���(���)

	float zzc;       //���ʲ�(��Ԫ)    10
	float ldzc;      //�����ʲ�(��Ԫ)
	float cqtz;      //����Ͷ��(��Ԫ)
	float gdzc;      //�̶��ʲ�(��Ԫ)
	float wxzc;      //�����ʲ�(��Ԫ)
	float ldfz;      //������ծ(��Ԫ)   15
	float cqfz;      //���ڸ�ծ(��Ԫ)
	float ggqy;      //�ɶ�Ȩ��(��Ԫ)
	float zbgjj;     //�ʱ�������(��Ԫ)
	float yygjj;     //ӯ�๫����(��Ԫ)
	float mgjz;      //ÿ�ɾ�ֵ(Ԫ)    20
	float gdqybl;    //�ɶ�Ȩ�����(%)
	float mggjj;     //ÿ�ɹ�����(Ԫ)

	float zyywsr;    //��Ӫҵ������(��Ԫ)
	float zyywlr;    //��Ӫҵ������(��Ԫ)
	float qtywlr;    //����ҵ������(��Ԫ)25
	float lrze;      //�����ܶ�(��Ԫ)
	float jlr;       //������(��Ԫ)
	float wfplr;     //δ��������(��Ԫ)
	float mgsy;      //ÿ������(Ԫ)
	float jzcsyl;    //���ʲ�������(%)  30
	float mgwfplr;   //ÿ��δ��������(Ԫ)

	float mgjzc;     //ÿ�ɾ��ʲ�(Ԫ)

	int m_iPos;
	BYTE  free[8];
	Split m_Split[80];
}BASEINFO;
typedef BASEINFO *PBASEINFO;
//////////////////////////////////////////////////////////////////////////
//��Ʊ�ڰ�������Խṹ
typedef struct
{
	char m_szSymbol[9];			   // ��Ʊ����
	BOOL m_bDeleted;
	int m_iPos;
	float m_ClosePrice;
    float m_fRight[MaxRights];     // ��ƱȨ��
    BYTE  m_btStockType[MaxStockTYpe];    
}STOCK_TYPE_INFO;
//////////////////////////////////////////////////////////////////////////
//CReportData�ṹ˵��
//CReportData�ṹ��Ҫ���ڼ��ռ�ʱ��Ʊ������Ϣ
typedef struct
{
	 BYTE kind;
	 BYTE  rdp;				  // ���ڼ������ǿ��ָ��
	 char id[8];              // ֤ȯ����
	 char name[10];           // ֤ȯ���� 
	 char Gppyjc[6];          // ֤ȯ����ƴ�����
	 int sel;
	 float ystc;              // ��������
	 float opnp;              // ���տ��� 
	 float higp;              // �������
	 float lowp;              // �������
	 float nowp;              // ���¼۸�
	 float nowv;              // ���³ɽ���
	 float totv;              // �ܳɽ���
	 float totp;              // �ܳɽ����
	 float pbuy1;             // �����һ
	 float vbuy1;             // ������һ
	 float pbuy2;             // ����۶�  
	 float vbuy2;             // ��������
	 float pbuy3;             // ������� 
	 float vbuy3;             // ��������      
	 float psel1;             // ������һ
	 float vsel1;             // ������һ
	 float psel2;             // �����۶� 
	 float vsel2;             // ��������   
	 float psel3;             // ��������
	 float vsel3;             // �������� 
	 float accb;              // ����ҵ��ͣ���������ҵ��ͣ�
	 float accs;              // �����ҵ��ͣ����������ҵ��ͣ�
	 float volume5;           // 5�����
	 float rvol;              // ����
	 float dvol;              // ����
	 float pbuy4;             // �����  
	 float vbuy4;             // ������
	 float pbuy5;             // ����� 
	 float vbuy5;             // ������      
	 float psel4;             // ������
	 float vsel4;             // ������
	 float psel5;             // ������ 
	 float vsel5;             // ������   
 
	 short    lastclmin;      // �ϴδ��������
	 short    initdown;       // ��Ʊ��ʼ����־   0 = δ�ܳ�ʼ����1=�ѳ�ʼ��   
	 int      InOut;
	 BOOL     IsDelete;
	 BOOL	  IsMxTj;
 
	 float	  Index[10];
	 Kdata1   m_Kdata1[240];
	 BASEINFO *pBaseInfo;

	 STOCK_TYPE_INFO *pStockTypeInfo;

	 int	m_serialNumber;	  // ���ջ����ļ��й�Ʊ���ݵ����к�
} CReportData;
typedef CReportData *PCdat1;
//////////////////////////////////////////////////////////////////////////

class CIndexExtraData  
{
public:
	CIndexExtraData();
	virtual ~CIndexExtraData();

public:
	CIndexExtraData * NewIndexExtraData();
	void			  FreeIndexExtraData(CIndexExtraData * pIndexExtraData);
private:
	PCdat1			  m_pReportData;
};

#endif // !defined(AFX_INDEXEXTRADATA_H__)
