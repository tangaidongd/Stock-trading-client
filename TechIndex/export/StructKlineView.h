#ifndef	_RECT_HEADER_
#define	_RECT_HEADER_

#include "stdafx.h"
#include "define.h"
#include <afxtempl.h>
#include "ShareFun.h"
//#include "CFormularContent.h"

#define VOLSTICK                    1
#define STICK_VOL_FS                2
#define CIRCLE		                3
#define LINESTICK                   4
#define CROSSDOT                    5
#define STICK                       6
#define STICK_LINGXIAN_FS		    7
#define DOT                         8

/*17���Ի������ݵ��ڴ��еĴ洢�ṹ��
CArray		lineself		 ��Ÿ����Ի������� ����Ԫ��ΪINFO_LINESELF�ṹ�Ķ���
INFO_LINESELF�ṹ�ĳ�Ա���£�
	int 	type		
	int	timeFirst
	int 	timeScd
	float		valFir
	float		valScd
*/
struct INFO_LINE_POINT    //cjg
{
	int time;
	float valFlt;
};


struct INFO_LINESELF{           //cjg
	int 	type;//�ߵ�����
	int     klineType;//K������
	INFO_LINE_POINT linePoint[10];
/*	int		time1;//�������ʱ��ֵ
	float	valFlt1;//������ĸ�����ֵ
	int		time2;//�������ʱ��ֵ
	float	valFlt2;//������ĸ�����ֵ
	int		time3;//�������ʱ��ֵ
	float	valFlt3;//������ĸ�����ֵ*/
	COLORREF	color;//�ߵ���ɫ
	BYTE	byLineKind;//�ߵ�����:0��ʵ��,1������,2�ǵ����
	BYTE	byLineWidth;//�ߵĿ��:0��1����,1��2����,2��3����
	BYTE	byAlarm;//�Ƿ�������Ԥ��,ֻ����������Ч
	BYTE	byWhichFiguer;//Ԥ��
	int     nDays;//����������
	int     stored;//Ԥ��
	INFO_LINESELF()
	{
		byWhichFiguer = 0;
	}
};


/*
class ARRAY_Right_Box
{
public:
	ARRAY_Right_Box()
	{
	};
	virtual ~ARRAY_Right_Box()
	{
	};
}*/
struct LooseValue
{
	int nFoot;
	float fVal;
	CString StrExtraData;
};

typedef CArray<LooseValue,LooseValue&> looseValueArray; 
class ARRAY_BE
{ 
public:
	float *line;
	int b;
	int e;
	float *lineWidth;

	enum Kind
	{
		Normal = 0,
		DrawLine,
		DrawPolyLine,
		DrawStickLine,
		DrawIcon,
		StringData,//DrawString
		DrawKLine,
		DrawNum,
		DrawBand,
		StringDataEx,//
	};

	Kind kind;//0 is normal,1 is draw line,3 is line data of right box,2 is string
	WORD nLineWidth;
	WORD isVirtualLine;
	CString s;
	looseValueArray looseArr;
	int iKlineDrawType;		// drawkline2ʱ����K�ߵķ�ʽ 0-��ͨ������ 1-ʵ���� 2-������

	ARRAY_BE()
	{
		s = "";
		line = NULL;
		lineWidth = NULL;
		kind = Normal;
		b=0;
		e = 0;
		nLineWidth = 1;
		isVirtualLine = 0;
		iKlineDrawType = 0;
	};
	virtual ~ARRAY_BE()
	{
		if(lineWidth) delete [] lineWidth;
	};
	void Reset()
	{
		if(line) delete [] line;	
		if(lineWidth) delete [] lineWidth;
		
		s = "";
		line = NULL;
		lineWidth = NULL;
		kind = Normal;
		b=0;
		e = 0;
		nLineWidth = 1;
		isVirtualLine = 0;
		iKlineDrawType = 0;
	}
	ARRAY_BE& operator=(ARRAY_BE& in)
	{
		this->b=in.b;
		e = in.e;
		kind = in.kind;
		nLineWidth = in.nLineWidth;
		isVirtualLine = in.isVirtualLine;
		iKlineDrawType = in.iKlineDrawType;
		s = in.s;
		if(line&&in.line && b>=0)
		{
			for(int i = b;i<=e;i++)
				this->line[i]=in.line[i];
		}

		if(kind	>=	DrawLine)
		{
			looseArr.Copy(in.looseArr );
		}

		if(kind == DrawStickLine)
		{
			if(this->lineWidth) delete [] lineWidth;
			if(in.e>=0)
			{
				lineWidth = new float[in.e+1];
				if(in.lineWidth)
					memcpyex(lineWidth,in.lineWidth ,sizeof(float)*(in.e+1));
			}
			else
				lineWidth = NULL;
		}


		return *this;
	};
	bool Add(ARRAY_BE& in1,ARRAY_BE& in2 )
	{
		 b=max(in1.b,in2.b);
		 e=min(in1.e,in2.e);
		if(!in1.s.IsEmpty())
			 s = in1.s;
		if(!in2.s.IsEmpty())
			 s = in2.s;

		{
			 kind = in1.kind>in2.kind ? in1.kind : in2.kind ;

			if(in1.line&&in2.line &&  line &&  b>=0)
			{
				for(int i =  b;i<= e;i++)
					 line[i]=in1.line[i]+in2.line[i];
			}
		}
		return true;
	};
	bool Add(ARRAY_BE& in1,float fIn2 )
	{
		 b=in1.b;
		 e=in1.e;
		if(!in1.s.IsEmpty())
			 s = in1.s;

		 kind = in1.kind;
		{
			if(in1.line &&  line &&  b>=0)
			{
				for(int i =  b;i<=e;i++)
					 line[i]=in1.line[i]+fIn2;
			}
		}
		return true;
	};
	bool Sub(ARRAY_BE& in1,ARRAY_BE& in2 )
	{
		 b=max(in1.b,in2.b);
		 e=min(in1.e,in2.e);
		if(!in1.s.IsEmpty())
			 s = in1.s;
		if(!in2.s.IsEmpty())
			 s = in2.s;

		{
			 kind = in1.kind>in2.kind ? in1.kind : in2.kind ;

			if(in1.line&&in2.line &&  line &&  b>=0)
			{
				for(int i =  b;i<=e;i++)
					 line[i]=in1.line[i]-in2.line[i];
			}
		}
		return true;
	};
	bool Sub(ARRAY_BE& in1,float fIn2 )
	{
		 b=in1.b;
		 e=in1.e;
		if(!in1.s.IsEmpty())
			 s = in1.s;

		 kind = in1.kind;
		{
			if(in1.line &&  line &&  b>=0)
			{
				for(int i =  b;i<=e;i++)
					 line[i]=in1.line[i]-fIn2;
			}
		}
		return true;
	};
	bool Sub(float fIn2,ARRAY_BE& in1 )
	{
		 b=in1.b;
		 e=in1.e;
		if(!in1.s.IsEmpty())
			 s = in1.s;

		 kind = in1.kind;
		{
			if(in1.line &&  line &&  b>=0)
			{
				for(int i =  b;i<=e;i++)
					 line[i]=fIn2-in1.line[i];
			}
		}
		return true;
	};
	bool Mult(ARRAY_BE& in1,ARRAY_BE& in2 )
	{
		 b=max(in1.b,in2.b);
		 e=min(in1.e,in2.e);
		if(!in1.s.IsEmpty())
			 s = in1.s;
		if(!in2.s.IsEmpty())
			 s = in2.s;

		{
			 kind = in1.kind>in2.kind ? in1.kind : in2.kind ;

			if(in1.line&&in2.line &&  line &&  b>=0)
			{
				for(int i =  b;i<=e;i++)
					 line[i]=in1.line[i]*in2.line[i];
			}
		}
		return true;
	};
	bool Mult(ARRAY_BE& in1,float fIn2 )
	{
		 b=in1.b;
		 e=in1.e;
		if(!in1.s.IsEmpty())
			 s = in1.s;

		 kind = in1.kind;
		{
			if(in1.line &&  line &&  b>=0)
			{
				for(int i =  b;i<=e;i++)
					 line[i]=in1.line[i]*fIn2;
			}
		}
		return true;
	};
	bool Div(ARRAY_BE& in1,ARRAY_BE& in2 )
	{
		 b=max(in1.b,in2.b);
		 e=min(in1.e,in2.e);
		if(!in1.s.IsEmpty())
			 s = in1.s;
		if(!in2.s.IsEmpty())
			 s = in2.s;

		{
			 kind = in1.kind>in2.kind ? in1.kind : in2.kind ;

			if(in1.line&&in2.line &&  line &&  b>=0)
			{
				for(int i =  b;i<=e;i++)
				{
					if(in2.line[i]!=0)
						 line[i]=in1.line[i]/in2.line[i];
					else
					{						
						if ( i == b )
						{
							// ��ǰ�������� 0, �����Ǹ���. ����ȡ��һ����ֵ. 
							// ��Ҫ�������, �����ж�
							b += 1;
							line[i] = 0;
							continue;
						}

						//
						if(i>0)
							 line[i] =  line[i-1];
						else
							 line[i]= 0;
					}
				}
			}
		}
		return true;
	};
	bool Div(ARRAY_BE& in1,float fIn2 )
	{
		if ( 0 == fIn2 )
		{
			////ASSERT(0);
			return false;
		}
		
		b=in1.b;
		e=in1.e;
		if(!in1.s.IsEmpty())
			s = in1.s;
		
		kind = in1.kind;
		{
			if(in1.line &&  line &&  b>=0)
			{
				for(int i =  b;i<=e;i++)
				{
					if(fIn2!=0)
						line[i]=in1.line[i]/fIn2;
					else
					{
						if(i>0)
							line[i] =  line[i-1];
						else
							line[i]= 0;
					}
				}
			}
		}
		return true;
	};
	bool Div(float fIn2,ARRAY_BE& in1 )
	{
		 b=in1.b;
		 e=in1.e;
		if(!in1.s.IsEmpty())
			 s = in1.s;

		 kind = in1.kind;
		{
			if(in1.line &&  line &&  b>=0)
			{
				for(int i =  b;i<=e;i++)
				{
					if(in1.line[i] != 0 )
						 line[i]=fIn2/in1.line[i];
					else
					{
						if ( i == b )
						{
							// ��ǰ�������� 0, �����Ǹ���. ����ȡ��һ����ֵ. 
							// ��Ҫ�������, �����ж�
							b += 1;
							line[i] = 0;
							continue;
						}

						if(i>0)
							 line[i] =  line[i-1];
						else
							 line[i]= 0;
					}
				}
			}
		}
		return true;
	};
};

typedef struct Kline 
{
	int     day;		// ʱ��
	float	open;		// ���̼�
	float   high;		// ��߼�
	float   low;		// ��ͼ�
	float   close;		// ���̼�
	float   vol;		// �ɽ���
	float   amount;		// �ɽ����
	float	position;	// �ֲ�
	union
	{
	struct
	{
	WORD    advance;//���Ǽ���
	WORD    decline;//�µ�����
	};
	float   volPositive;//��������
	};
} Kline;
typedef struct KlineGeneral
{
	int     day;
	float	open;
	float   high;
	float   low;
	float   close;
	float   vol;
	float   amount;
	float   volPositive;
}KlineGeneral;

typedef struct ARRAY_BE_SHOW
{ 
 	ARRAY_BE m_arrBE;
	int bNow;
	int type;
	int lineThick;
	COLORREF	clr;
	ARRAY_BE_SHOW()
	{
		clr = -1;
		lineThick = 1;
	};
//	char name[10];
} ARRAY_BE_SHOW;
typedef struct ARRAY_JISHU{
	 int numLine;
	 ARRAY_BE_SHOW	line[64];
}ARRAY_JISHU;

typedef enum klineKind {
	HS_KLINE = 25,//�ֱʳɽ�
	MANY_DAY_KLINE	=	15,//������
	MINUTE1_KLINE	=	35,//1������
	MIN5_KLINE = 1,//5������	
	MIN15_KLINE = 2,//15������
	MIN30_KLINE = 3,//30������
	MIN60_KLINE = 4,//60������
	DAY_KLINE = 5,//����
	WEEK_KLINE = 6,//����
	MONTH_KLINE = 7,//����
	YEAR_KLINE = 8,//��
	MANY_MINUTE_KLINE = 9, // �������
	QUARTER_KLINE = 10,	// ������
} klineKind;


#endif