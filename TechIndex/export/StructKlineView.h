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

/*17．自画线数据的内存中的存储结构：
CArray		lineself		 存放各个自画线数据 ，其元素为INFO_LINESELF结构的对象
INFO_LINESELF结构的成员如下：
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
	int 	type;//线的类型
	int     klineType;//K线类型
	INFO_LINE_POINT linePoint[10];
/*	int		time1;//各个点的时间值
	float	valFlt1;//各个点的浮点数值
	int		time2;//各个点的时间值
	float	valFlt2;//各个点的浮点数值
	int		time3;//各个点的时间值
	float	valFlt3;//各个点的浮点数值*/
	COLORREF	color;//线的颜色
	BYTE	byLineKind;//线的种类:0是实线,1是虚线,2是点横线
	BYTE	byLineWidth;//线的宽度:0是1像素,1是2像素,2是3像素
	BYTE	byAlarm;//是否是条件预警,只对趋势线有效
	BYTE	byWhichFiguer;//预留
	int     nDays;//多日线天数
	int     stored;//预留
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
	int iKlineDrawType;		// drawkline2时绘制K线的方式 0-普通空心阳 1-实心阳 2-美国线

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
							// 当前被除数是 0, 而且是刚起步. 不能取上一个的值. 
							// 需要更新起点, 接着判断
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
							// 当前被除数是 0, 而且是刚起步. 不能取上一个的值. 
							// 需要更新起点, 接着判断
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
	int     day;		// 时间
	float	open;		// 开盘价
	float   high;		// 最高价
	float   low;		// 最低价
	float   close;		// 收盘价
	float   vol;		// 成交量
	float   amount;		// 成交金额
	float	position;	// 持仓
	union
	{
	struct
	{
	WORD    advance;//上涨家数
	WORD    decline;//下跌家数
	};
	float   volPositive;//主动买量
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
	HS_KLINE = 25,//分笔成交
	MANY_DAY_KLINE	=	15,//多日线
	MINUTE1_KLINE	=	35,//1分钟线
	MIN5_KLINE = 1,//5分钟线	
	MIN15_KLINE = 2,//15分钟线
	MIN30_KLINE = 3,//30分钟线
	MIN60_KLINE = 4,//60分钟线
	DAY_KLINE = 5,//日线
	WEEK_KLINE = 6,//周线
	MONTH_KLINE = 7,//月线
	YEAR_KLINE = 8,//年
	MANY_MINUTE_KLINE = 9, // 多分钟线
	QUARTER_KLINE = 10,	// 季度线
} klineKind;


#endif