#ifndef _DEFINE_H_
#define _DEFINE_H_

#define BEGIN_NUMBER1	100

#define PARAM_NUM 16					// huhe* 加到16个参数设置
#define EXTRA_Y_NUM 4					// 额外的Y轴坐标数
#define NUM_LINE_TOT						64

#define MAXB 2000
#define MAXB_ALL 2000
#define MIDB 10000   
#define MIDE 10099

#define RLINEB 11000    
#define RLINEE 11000+NUM_LINE_TOT

#define CEFB 12000    //浮点数开始
#define CEFE 12299	 //浮点数结束

#define CEIB 13000    //整数开始
#define CEIE 13299	//整数结束

#define CECB 13500    //字符串开始
#define CECE 13520	//字符串结束

#define FuncB 8000	//函数开始
#define FuncE 8899	//函数结束
#define FuncDllB 8400	//函数开始
#define FuncDllE 8700	//函数结束

#define PAM   9900  // 系统参数的最大值

#define PCMB  14000	//中间变量开始
#define PCME  14199	//中间变量结束

#define QUB  8800  //引用开始
#define QUE  8899  //引用终止

#define TOZERO  9009  //
#define CONSB  9010  //线型常数开始
#define CONSE  9498  //线型常数终止
#define CONSCOLORB1  9100  //color常数开始
#define CONSCOLORB  9200  //color常数开始
#define CONSCOLORE  9280  //color常数开始
#define CONSLINETHICKB  9300  //
#define CONSLINETHICKE  9320  //
#define CONSLINEDOT  9350  //
#define LEN_BYTE	240+136				// huhe* 加到16个参数设置

CString CTaiScreenParent_GetPeriodName(int nKlineType);
int CTaiScreenParent_FromKlineKindToFoot(int nKlineKind);
#endif//_DEFINE_H_