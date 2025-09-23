// DIAEDITZBGSHS.cpp : implementation file
//

#include "stdafx.h"
#include "DIAEDITZBGSHS.h"
//#include "CNavigatorDoc.h"
//
//#include "CTaiKlineIndicatorsManage.h"
//#include "CNavigatorApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogEDITZBGSHS dialog


CDialogEDITZBGSHS::CDialogEDITZBGSHS(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogEDITZBGSHS::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogEDITZBGSHS)
	m_sExplain = _T("");
	m_selecteditem = NULL;
	//}}AFX_DATA_INIT
}


void CDialogEDITZBGSHS::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogEDITZBGSHS)
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDC_TREE1, m_tree);
	DDX_Text(pDX, IDC_EDIT_FUNC_SELECT, m_sExplain);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogEDITZBGSHS, CDialogEx)
	//{{AFX_MSG_MAP(CDialogEDITZBGSHS)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelchangedTree1)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, OnDblclkTree1)
	ON_NOTIFY(NM_RETURN, IDC_TREE1, OnReturnTree1)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogEDITZBGSHS message handlers
dataFunc stritemsArr[] = //196
{
_T("行情函数"),-1,_T(""),//24
/*
_T("ACTIVEBUYVOL---主动性买量"),0,_T("主动性买量"),
_T("ACTIVESELLVOL---主动性卖量"),0,_T("主动性卖量"),
_T("ADVANCE---上涨股票数(大盘指标)"),0,_T("取得该周期上涨家数.\r\n用法:\r\nADVANCE\r\n(本函数仅对大盘有效)"),
_T("AMOUNT---成交额"),0,_T("取得该周期成交额.\r\n用法:\r\nAMOUNT"),
_T("ASKPRICE(1)---委卖价1"),0,_T("委卖价1(本函数仅个股在分笔成交分析周期有效)"),
_T("ASKPRICE(2)---委卖价2"),0,_T("委卖价2(本函数仅个股在分笔成交分析周期有效)"),
_T("ASKPRICE(3)---委卖价3"),0,_T("委卖价3(本函数仅个股在分笔成交分析周期有效)"),
_T("ASKVOL(1)---委卖量1"),0,_T("委卖量1(本函数仅个股在分笔成交分析周期有效)"),
_T("ASKVOL(2)---委卖量2"),0,_T("委卖量2(本函数仅个股在分笔成交分析周期有效)"),
_T("ASKVOL(3)---委卖量3"),0,_T("委卖量3(本函数仅个股在分笔成交分析周期有效)"),
_T("BIDVOL(1)---委买量1"),0,_T("委买量1(本函数仅个股在分笔成交分析周期有效)"),
_T("BIDVOL(2)---委买量2"),0,_T("委买量2(本函数仅个股在分笔成交分析周期有效)"),
_T("BIDVOL(3)---委买量3"),0,_T("委买量3(本函数仅个股在分笔成交分析周期有效)"),
_T("BIDPRICE(1)---委买价1"),0,_T("委买价1(本函数仅个股在分笔成交分析周期有效)"),
_T("BIDPRICE(2)---委买价2"),0,_T("委买价2(本函数仅个股在分笔成交分析周期有效)"),
_T("BIDPRICE(3)---委买价3"),0,_T("委买价3(本函数仅个股在分笔成交分析周期有效)"),
*/

//_T("ADVANCE---上涨股票数(大盘指标)"),0,_T("取得该周期上涨家数.\r\n用法:\r\nADVANCE\r\n(本函数仅对大盘有效)"),
_T("BUYVOL---换手数据的主动性买量"),0,_T("取得主动性买单量.\r\n用法:\r\nBUYVOL\r\n当本笔成交为主动性买盘时,其数值等于成交量,否则为0\r\n(本函数仅个股在分笔成交分析周期有效)\r\n"),
_T("CLOSE---收盘价"),0,_T("取得该周期收盘价.\r\n用法:\r\nCLOSE\r\n"),
_T("DECLINE---下跌股票数(大盘指标)"),0,_T("取得该周期下跌家数.\r\n用法:\r\nDECLINE\r\n(本函数仅对大盘有效)"),
_T("HIGH---最高价"),0,_T("取得该周期最高价.\r\n用法:\r\nHIGH"),
_T("ISBUYORDER---是否是买单"),0,_T("取得该成交是否为主动性买单.\r\n用法:\r\nISBUYORDER\r\n当本笔成交为主动性买盘时,返回1,否则为0\r\n(本函数仅个股在分笔成交分析周期有效)"),
_T("LOW---最低价"),0,_T("取得该周期最低价.\r\n用法:\r\nLOW"),
_T("OPEN---开盘价"),0,_T("取得该周期开盘价.\r\n用法:\r\nOPEN"),
_T("SELLVOL---换手数据的主动性卖量"),0,_T("取得主动性卖单量.\r\n用法:\r\nBUYVOL\r\n当本笔成交为主动性卖盘时,其数值等于成交量,否则为0\r\n(本函数仅个股在分笔成交分析周期有效)\r\n"),
_T("VOL---成交量"),0,_T("取得该周期成交量.\r\n用法:\r\nVOL"),
_T("时间函数"),-1,_T(""),//8
_T("DAY---日期"),0,_T("取得该周期的日期.\r\n用法:\r\nDAY\r\n函数返回有效值范围为(1-31)"),
_T("DATE---年月日,取得该周期从1900以来的年月日"),0,_T("取得该周期从1900以来的年月日.\r\n用法:\r\nDATE\r\n函数返回有效值范围为(700101-1341231),表示19700101-20341231"),
_T("HOUR---小时"),0,_T("取得该周期的小时数.\r\n用法:\r\nHOUR\r\n函数返回有效值范围为(0-23),对于日线及更长的分析周期值为0"),
_T("MINUTE---分钟"),0,_T("取得该周期的分钟数.\r\n用法:\r\nMINUTE\r\n函数返回有效值范围为(0-59),对于日线及更长的分析周期值为0"),
_T("MONTH---月份"),0,_T("取得该周期的月份.\r\n用法:\r\nMONTH\r\n函数返回有效值范围为(1-12)"),
_T("TIME---时分秒"),0,_T("取得该周期的时分秒.\r\n用法:\r\nTIME\r\n函数返回有效值范围为(000000-235959)"),
_T("VALUEDAY(DAY,HOUR,X)---DAY年月或年月日,HOUR日时分或时分的X的值."),0,_T("如：VALUEDAY(200103,210930,CLOSE) 表示2001年3月21日9点30分时的收盘价格."),
_T("WEEK---星期"),0,_T("取得该周期的月份.\r\n用法:\r\nWEEK\r\n函数返回有效值范围为(0-6)"),
_T("YEAR---年份"),0,_T("取得该周期的年份.\r\n用法:\r\nYEAR\r\n函数返回有效值范围为(1970-2038)"),
_T("PERIOD---取得周期类型"),0,_T("取得周期类型.\r\n结果从0到11,依次分别是1/5/15/30/60分钟,日/周/月,多分钟,多日,季,年."),
_T("FROMOPEN---求当前时刻距开盘有多长时间"),0,_T("求当前时刻距开盘有多长时间.\r\n用法:\r\nFROMOPEN.返回当前时刻距开盘有多长时间,单位为分钟.例如:当前时刻为早上十点，则返回31."),

_T("引用函数"),-1,_T(""),//20
_T("BACKSET(X,N)---若X非0,则将当前位置到N周期前的数值设为1"),0,_T("将当前位置到若干周期前的数据设为1.\r\n用法:\r\nBACKSET(X,N),若X非0,则将当前位置到N周期前的数值设为1.\r\n例如：BACKSET(CLOSE>OPEN,2)若收阳则将该周期及前一周期数值设为1,否则为0"),
_T("BARSCOUNT(X)---第一个有效数据到当前的天数"),0,_T("求总的周期数.\r\n用法:\r\nBARSCOUNT(X)第一个有效数据到当前的天数\r\n例如：BARSCOUNT(CLOSE)对于日线数据取得上市以来总交易日数,对于分笔成交取得当日成交笔数,对于1分钟线取得当日交易分钟数"),
_T("BARSLAST(X)---上一次X不为0到现在的天数"),0,_T("上一次条件成立到当前的周期数.\r\n用法:\r\nBARSLAST(X):上一次X不为0到现在的天数\r\n例如：BARSLAST(CLOSE/REF(CLOSE,1)>=1.1)表示上一个涨停板到当前的周期数"),
_T("BARSSINCE(X)---第一次X不为0到现在的天数"),0,_T("例如：BARSSINCE(HIGH>10)表示股价超过10元时到当前的周期数"),
_T("COUNT(X,N)---统计N周期中满足X条件的周期数,若N=0则从第一个有效值开始"),0,_T("第一个条件成立到当前的周期数.\r\n用法:\r\nBARSSINCE(X):第一次X不为0到现在的天数\r\n例如：BARSSINCE(HIGH>10)表示股价超过10元时到当前的周期数"),
_T("DMA(X,A)---求X的动态移动平均"),0,_T("求动态移动平均.\r\n用法:\r\nDMA(X,A),求X的动态移动平均.\r\n算法: 若Y=DMA(X,A)\r\n则 Y=A*X+(1-A)*Y',其中Y'表示上一周期Y值,A必须小于1.\r\n例如：DMA(CLOSE,VOL/CAPITAL)表示求以换手率作平滑因子的平均价"),
_T("EMA(X,N)---计算X的N天指数平滑移动平均值"),0,_T("求指数平滑移动平均.\r\n用法:\r\nEMA(X,N),求X的N日指数平滑移动平均.算法：若Y=EMA(X,N)\r\n则Y=[2*X+(N-1)*Y']/(N+1),其中Y'表示上一周期Y值.\r\n例如：EMA(CLOSE,30)表示求30日指数平滑均价"),
_T("HHV(X,N)---计算N天中X的最高值"),0,_T("求最高值.\r\n用法:\r\nHHV(X,N),求N周期内X最高值,N=0则从第一个有效值开始.\r\n例如：HHV(HIGH,30)表示求30日最高价"),
_T("HHAV(X)---计算当前所有数据(含当前周期以后的周期数据)中X的最高值"),0,_T("求所有数据最高值.\r\n用法:\r\nHHAV(X),求当前所有数据周期内X最高值.\r\n例如：HHAV(HIGH)表示所有数据内最高价"),
_T("LLAV(X)---计算当前所有数据(含当前周期以后的周期数据)中X的最小值"),0,_T("求所有数据最小值.\r\n用法:\r\nLLAV(X),求当前所有数据周期内X最小值.\r\n例如：LLAV(HIGH)表示所有数据内最低价"),
_T("HHVBARS(X,N)---求N周期内X最高值到当前周期数,N=0表示从第一个有效值开始统计"),0,_T("求上一高点到当前的周期数.\r\n用法:\r\nHHVBARS(X,N):求N周期内X最高值到当前周期数,N=0表示从第一个有效值开始统计\r\n例如：HHVBARS(HIGH,0)求得历史新高到到当前的周期数"),
_T("LLV(X,N)---计算N天中X的最低值"),0,_T("求最低值.\r\n用法:\r\nLLV(X,N),求N周期内X最低值,N=0则从第一个有效值开始.\r\n例如：LLV(LOW,0)表示求历史最低价"),
_T("LLVBARS(X,N)---求N周期内X最低值到当前周期数,N=0表示从第一个有效值开始统计"),0,_T("求上一低点到当前的周期数.\r\n用法:\r\nLLVBARS(X,N):求N周期内X最低值到当前周期数,N=0表示从第一个有效值开始统计\r\n例如：LLVBARS(HIGH,20)求得20日最低点到当前的周期数"),
_T("MA(X,N)---计算X的N天移动平均值"),0,_T("求简单移动平均.\r\n用法:\r\nMA(X,N),求X的N日移动平均值.算法：(X1+X2+X3+...+Xn)/N\r\n例如：MA(CLOSE,10)表示求10日均价"),
_T("REF(X,N)---计算X的N天前的值"),0,_T("引用若干周期前的数据.\r\n用法:\r\nREF(X,A),引用A周期前的X值.\r\n例如：REF(CLOSE,1)表示上一周期的收盘价,在日线上就是昨收"),
_T("REFBEGIN(X,N)---计算X的所有数据的第N个数据的值"),0,_T("例如：REFBEGIN(CLOSE,1)表示CLOSE的所有数据的第1个数据的值"),
_T("REFLAST(X,N)---计算X的所有数据的倒数第N个数据的值"),0,_T("例如：REFLAST(CLOSE,1)表示CLOSE的所有数据的倒数第1个数据的值"),
_T("SMA(X,N,M)---求X的N日移动平均,M为权重"),0,_T("求移动平均.\r\n用法:\r\nSMA(X,N,M),求X的N日移动平均,M为权重.\r\n算法: 若Y=SMA(X,N,M)\r\n则 Y=[M*X+(N-M)*Y')/N,其中Y'表示上一周期Y值,N必须大于M.\r\n例如：SMA(CLOSE,30,1)表示求30日移动平均价"),
_T("SUM(X,N)---计算N天中X的总和值"),0,_T("求总和.\r\n用法:\r\nSUM(X,N),统计N周期中X的总和,N=0则从第一个有效值开始.\r\n例如：SUM(VOL,0)表示统计从上市第一天以来的成交量总和"),
_T("SUMBARS(X,A)---将X向前累加直到大于等于A,返回这个区间的周期数"),0,_T("向前累加到指定值到现在的周期数.\r\n用法:\r\nSUMBARS(X,A):将X向前累加直到大于等于A,返回这个区间的周期数\r\n例如：SUMBARS(VOL,CAPITAL)求完全换手到现在的周期数"),
_T("EXTDATA(N)---引用ID号为N的扩展指标数据"),0,_T("取得日线扩展数据(1-999).\r\n用法:\r\nEXTDATA(N),N取1--999\r\n(本函数仅在日线分析周期有效)"),
_T("DRAWNULL---返回无效数"),0,_T("返回无效数\r\n在图形上不会绘制无效数据的点"),
_T("CURRBARSCOUNT---求到最后交易日的周期数"),0,_T("求到最后交易日的周期数.\r\n用法:\r\n  CURRBARSCOUNT 求到最后交易日的周期数"),
//					_T("reflast"),_T("refbegin"),_T("valueaverage")
_T("算术函数"),-1,_T(""),//9
_T("ABS(X)---X的绝对值"),0,_T("求绝对值.\r\n用法:\r\nABS(X)返回X的绝对值\r\n例如：ABS(-34)返回34"),
_T("BETWEEN(A,B,C)---表示A处于B和C之间时返回1,否则返回0"),0,_T("介于两个数之间.\r\n用法:\r\nBETWEEN(A,B,C)表示A处于B和C之间时返回1,否则返回0\r\n例如：BETWEEN(CLOSE,MA(CLOSE,10),MA(CLOSE,5))表示收盘价介于5日均线和10日均线之间"),
_T("CROSS(A,B)---A线向上穿越B线为1,否则为0"),0,_T("两条线交叉.\r\n用法:\r\nCROSS(A,B)表示当A从下方向上穿过B时返回1,否则返回0\r\n例如：CROSS(MA(CLOSE,5),MA(CLOSE,10))表示5日均线与10日均线交金叉"),
_T("FILTER(X,N)---X满足条件后,删除其后N周期内的数据置为0"),0,_T("过滤连续出现的信号.\r\n用法:\r\nFILTER(X,N):X满足条件后,删除其后N周期内的数据置为0\r\n例如：FILTER(CLOSE>OPEN,5)查找阳线,5天内再次出现的阳线不被记录在内"),
_T("LONGCROSS(A,B,N)---表示A在N周期内都小于B,本周期从下方向上穿过B时返回1,否则返回0"),0,_T("例如：LONGCROSS(MA(CLOSE,5),MA(CLOSE,10),5)表示5日均线维持5周期后与10日均线交金叉"),
_T("MAX(X,N)---最大值函数"),0,_T("求最大值.\r\n用法:\r\nMAX(A,B)返回A和B中的较大值\r\n例如：MAX(CLOSE-OPEN,0)表示若收盘价大于开盘价返回它们的差值,否则返回0"),
_T("MIN(X,N)---最小值函数"),0,_T("求最小值.\r\n用法:\r\nMIN(A,B)返回A和B中的较小值\r\n例如：MIN(CLOSE,OPEN)返回开盘价和收盘价中的较小值"),
_T("MOD(A,B)---取得A对B的模"),0,_T("求模运算.\r\n用法:\r\nMOD(A,B)返回A对B求模\r\n例如：MOD(26,10)返回6"),
_T("NOT(X)---取得逻辑非"),0,_T("求逻辑非.\r\n用法:\r\nNOT(X)返回非X,即当X=0时返回1,否则返回0\r\n例如：NOT(ISUP)表示平盘或收阴\r\n"),
_T("RANGE(A,B,C)---表示A大于B同时小于C时返回1,否则返回0"),0,_T("介于某个范围之间.\r\n用法:\r\nRANGE(A,B,C)表示A大于B同时小于C时返回1,否则返回0\r\n例如：RANGE(CLOSE,MA(CLOSE,5),MA(CLOSE,10))表示收盘价大于5日均线并且小于10日均线"),
_T("REVERSE(X)---取得X的相反数-X"),0,_T("求相反数.\r\n用法:\r\nREVERSE(X)返回-X\r\n例如REVERSE(CLOSE)返回-CLOSE"),
_T("SGN(X)---取得X的符号,返回(1,0,-1)"),0,_T("求符号值.\r\n用法:\r\nSGN(X),当X>0,X=0,X<0分别返回1,0,-1\r\n"),
_T("数学函数"),-1,_T(""),//13
_T("ACOS(X)---返回X的反余弦值"),0,_T("反余弦值"),
_T("ASIN(X)---返回X的反正弦值"),0,_T("反正弦值"),
_T("ATAN(X)---返回X的反正切值"),0,_T("反正切值"),
_T("CEILING(X)---向上舍入"),0,_T("例如：CEILING(12.3)求得13,CEILING(-3.5)求得-3"),
_T("COS(X)---返回X的余弦值"),0,_T("余弦值"),
_T("EXP(X)---以e为底的X次幂"),0,_T("EXP(CLOSE)返回e的CLOSE次幂"),
_T("FLOOR(X)---向下舍入"),0,_T("例如：FLOOR(12.3)求得12,FLOOR(-3.5)求得-4"),
_T("INTPART(X)---X的整数"),0,_T("例如：INTPART(12.3)求得12,INTPART(-3.5)求得-3"),
_T("LN(X)---X的以e为底的对数"),0,_T("例如：LN(CLOSE)求收盘价的对数"),
_T("LOG(X)---X的以10为底的对数"),0,_T("例如：LOG(100)等于2"),
_T("POW(A,B)---A的B次幂"),0,_T("例如：POW(CLOSE,3)求得收盘价的3次方"),
_T("SIN(X)---返回X的正弦值"),0,_T("正弦值"),
_T("SQRT(X)---X的平方根"),0,_T("例如：SQRT(CLOSE)收盘价的平方根"),
_T("TAN(X)---返回X的正切值"),0,_T("正切值"),
_T("逻辑函数"),-1,_T(""),//5
_T("IF(X,A,B)---若X则返回A,否则返回B"),0,_T("根据条件求不同的值.\r\n用法:\r\nIF(X,A,B)若X不为0则返回A,否则返回B\r\n例如：IF(CLOSE>OPEN,HIGH,LOW)表示该周期收阳则返回最高值,否则返回最低值"),
_T("ISDOWN---当日下跌则为1,否则为0"),0,_T("当收盘<开盘时,返回值为1,否则为0"),
_T("ISEQUAL---当日平盘则为1,否则为0"),0,_T("当收盘=开盘时,返回值为1,否则为0"),
_T("ISUP---当日上涨则为1,否则为0"),0,_T("当收盘>开盘时,返回值为1,否则为0"),
_T("统计函数"),-1,_T(""),//10
_T("AVEDEV(X,N)---计算X的N天的平均绝对偏差"),0,_T("平均绝对偏差.\r\n用法:\r\nAVEDEV(X,N)\r\n"),
_T("DEVSQ(X,N)---计算X的N天的偏差平方和"),0,_T("数据偏差平方和.\r\n用法:\r\nDEVSQ(X,N)\r\n"),
_T("FORCAST(X,N)---X的N周期线性回归预测值"),0,_T("线性回归预测值.\r\n用法:\r\nFORCAST(X,N)为X的N周期线性回归预测值\r\n例如:FORCAST(CLOSE,10)表示求10周期线性回归预测本周期收盘价"),
_T("STD(X,N)---计算X的N天的估算标准差"),0,_T("估算标准差.\r\n用法:\r\nSTD(X,N)为X的N日估算标准差\r\n"),
_T("STDP(X,N)---计算X的N天的总体标准差"),0,_T("总体标准差.\r\n用法:\r\nSTDP(X,N)为X的N日总体标准差\r\n"),
_T("SLOPE(X,N)---X的N周期线性回归线的斜率"),0,_T("线性回归斜率.\r\n用法:\r\nSLOPE(X,N)为X的N周期线性回归线的斜率\r\n例如:SLOPE(CLOSE,10)表示求10周期线性回归线的斜率\r\n"),
_T("VALUEAVERAGE( PRICE,VOL,PERCENT)---根据VOL、PERCENT计算PRICE的均价"),0,_T(""),
_T("VAR(X,N)---计算X的N天的估算样本方差"),0,_T("估算样本方差.\r\n用法:\r\nVAR(X,N)为X的N日估算样本方差\r\n"),
_T("VARP(X,N)---计算X的N天的总体样本方差"),0,_T("总体样本方差.\r\n用法:\r\nVARP(X,N)为X的N日总体样本方差"),
_T("指标函数"),-1,_T(""),//15
_T("COST(N)---表示N%获利盘的价格是多少"),0,_T("成本分布情况.\r\n用法:\r\nCOST(10),表示10%获利盘的价格是多少,即有10%的持仓量在该价格以下,其余90%在该价格以上,为套牢盘\r\n该函数仅对日线分析周期有效"),
_T("DOWNWARD(X,N)---X连续N日下降"),0,_T("例如：DOWNWARD(CLOSE,5)表示CLOSE连续5日下降"),
_T("PEAK(X,N,M)---前M个ZIG转向波峰值"),0,_T("前M个ZIG转向波峰值.\r\n用法:\r\nPEAK(K,N,M)表示之字转向ZIG(K,N)的前M个波峰的数值,M必须大于等于1\r\n例如：PEAK(1,5,1)表示%5最高价ZIG转向的上一个波峰的数值"),
_T("PEAKBARS(X,N,M)---前M个ZIG转向波峰到当前距离"),0,_T("前M个ZIG转向波峰到当前距离.\r\n用法:\r\nPEAKBARS(K,N,M)表示之字转向ZIG(K,N)的前M个波峰到当前的周期数,M必须大于等于1\r\n例如：PEAK(0,5,1)表示%5开盘价ZIG转向的上一个波峰到当前的周期数"),
_T("SAR(N,S,M)---抛物转向,N:天数 S:步长 M:极值"),0,_T("抛物转向.\r\n用法:\r\nSAR(N,S,M),N为计算周期,S为步长,M为极值\r\n例如SAR(10,2,20)表示计算10日抛物转向,步长为2%,极限值为20%"),
_T("SARTURN(N,S,M)---抛物转向位置,N:天数 S:步长 M:极值"),0,_T("抛物转向点.\r\n用法:\r\nSARTURN(N,S,M),N为计算周期,S为步长,M为极值,若发生向上转向则返回1,若发生向下转向则返回-1,否则为0\r\n其用法与SAR函数相同"),
_T("SLOWDOWNCROSS(N, X1,X2)---X1维持N周期后下穿X2"),0,_T("X1维持N周期后下穿X2"),
_T("SLOWUPCROSS(N, X1,X2)---X1维持N周期后上穿X2"),0,_T("X1维持N周期后上穿X2"),
_T("TROUGH(X,N,M)---表示之字转向ZIG(X,N)的前M个波谷的数值,M必须大于等于1"),0,_T("前M个ZIG转向波谷值.\r\n用法:\r\nTROUGH(K,N,M)表示之字转向ZIG(K,N)的前M个波谷的数值,M必须大于等于1\r\n例如：TROUGH(2,5,2)表示%5最低价ZIG转向的前2个波谷的数值"),
_T("TROUGHBARS(X,N,M)---表示之字转向ZIG(X,N)的前M个波谷到当前的周期数,M必须大于等于1"),0,_T("例如：TROUGH(2,5,2)表示%5最低价ZIG转向的前2个波谷到当前的周期数"),
_T("UPWARD(X,N)---X连续N日上升"),0,_T("X连续N日上升"),
_T("ZIG(X,N)---当价格变化量超过N%时转向,X表示输入数值"),0,_T("之字转向.\r\n用法:\r\nZIG(K,N),当价格变化量超过N%时转向,K表示0:开盘价,1:最高价,2:最低价,3:收盘价\r\n例如：ZIG(3,5)表示收盘价的5%的ZIG转向"),
_T("TOWERVALUE(N)---宝塔线的各种数值,N = 1表示宝塔线的最高值,N = 2表示宝塔线的最低值,\r\nN = 3表示宝塔线的中间值,N = 4表示宝塔线的状态（1为红色,2为绿色,3为即有红又有绿色）"),0,_T("宝塔线的各种数值"),
_T("WINNER(X)---分布在X(指标或行情数据)以上的所有价位的成交量的和占总流通盘的百分比"),0,_T("用法:\r\nWINNER(CLOSE),表示以当前收市价卖出的获利盘比例,例如返回0.1表示10%获利盘；WINNER(10.5)表示10.5元价格的获利盘比例\r\n该函数仅对日线分析周期有效"),
_T("WINNERTYPE(N)---流通盘分布类函数,N=1为最高峰处的价格,N=2为最高峰处的成交量密度（手/分）,N=3为平均成本处的价格"),0,_T("流通盘分布类函数"),

_T("大盘函数"),-1,_T(""),//9 
_T("INDEXA---指数成交额"),0,_T("指数成交额"),
_T("INDEXADV---大盘上涨家数"),0,_T("大盘上涨家数"),
_T("INDEXC---指数收盘价"),0,_T("指数收盘价"),
_T("INDEXDEC---大盘下跌家数"),0,_T("大盘下跌家数"),
_T("INDEXH---指数最高价"),0,_T("指数最高价"),
_T("INDEXL---指数最低价"),0,_T("指数最低价"),
_T("INDEXO---指数开盘价"),0,_T("指数开盘价"),
_T("INDEXV---指数成交量"),0,_T("指数成交量"),

_T("常数函数"),-1,_T(""),//75
_T("CAPITAL---流通盘"),0,_T("流通盘"),
_T("TOTALCAPITAL---当前总股本（手）"),0,_T("当前总股本（手）"),
_T("HYZSCODE---所属行业的板块指数代码 "),0,_T("所属行业的板块指数代码 "),

// 新增函数 新增周期类函数
_T("TOPRANGE(X)---当前值是近多少周期内的最大值"),0,_T("用法：\r\n TOPRANGE(X):X是近多少周期内的X的最大值\r\n 例如：\r\n TOPRANGE(HIGH)表示当前最高价是近多少周期内的最高价的最大值"),
_T("LOWRANGE(X)---当前值是近多少周期内的最小值"),0,_T("用法：\r\n LOWRANGE(X):X是近多少周期内的X的最小值\r\n 例如：\r\n TOPRANGE(LOW)表示当前最低价是近多少周期内的最低价的最小值"),
_T("FINDHIGH(VAR,N,M,T)---N周期前M周期内的第T个最大值"),0,_T("用法：\r\n FINDHIGH(VAR,N,M,T):VAR在N日前的M天内第T个最高价"),
_T("FINDHIGHBARS(VAR,N,M,T)---N周期前M周期内的第T个最大值到当前周期的周期数"),0,_T("用法：\r\n FINDHIGHBARS(VAR,N,M,T):VAR在N日前的M天内第T个最高价到当前周期的周期数"),
_T("FINDLOW(VAR,N,M,T)---N周期前M周期内的第T个最小值"),0,_T("用法：\r\n FINDLOW(VAR,N,M,T):VAR在N日前的M天内第T个最低价"),
_T("FINDLOWBARS(VAR,N,M,T)---N周期前M周期内的第T个最小值到当前周期的周期数"),0,_T("用法：\r\n FINDHIGHBARS(VAR,N,M,T):VAR在N日前的M天内第T个最低价到当前周期的周期数"),




// 
/*
_T("VOLUNIT---每手股数"),0,_T("每手股数"),
_T("DYNAINFO(3)---行情(昨天收盘)"),0,_T("昨天收盘"),
_T("DYNAINFO(4)---行情(今日开盘)"),0,_T("今日开盘"),
_T("DYNAINFO(5)---行情(今日最高)"),0,_T("今日最高"),
_T("DYNAINFO(6)---行情(今日最低)"),0,_T("今日最低"),
_T("DYNAINFO(7)---行情(最新价格)"),0,_T("最新价格"),
_T("DYNAINFO(8)---行情(总成交量)"),0,_T("总成交量"),
_T("DYNAINFO(9)---行情(最新成交量)"),0,_T("最新成交量"),
_T("DYNAINFO(10)---行情(总成交金额)"),0,_T("总成交金额"),
_T("DYNAINFO(11)---行情(均价)"),0,_T("均价"),
_T("DYNAINFO(12)---行情(涨跌)"),0,_T("涨跌"),
_T("DYNAINFO(13)---行情(振幅)"),0,_T("振幅"),
_T("DYNAINFO(14)---行情(涨幅)"),0,_T("涨幅"),
_T("DYNAINFO(15)---行情(委比)"),0,_T("委比"),
_T("DYNAINFO(16)---行情(委差)"),0,_T("委差"),
_T("DYNAINFO(17)---行情(量比)"),0,_T("量比"),
_T("DYNAINFO(18)---行情(委买)"),0,_T("委买"),
_T("DYNAINFO(19)---行情(委卖)"),0,_T("委卖"),
_T("DYNAINFO(20)---行情(委买价)"),0,_T("委买价"),
_T("DYNAINFO(21)---行情(委卖价)"),0,_T("委卖价"),
_T("DYNAINFO(22)---行情(内盘)"),0,_T("内盘"),
_T("DYNAINFO(23)---行情(外盘)"),0,_T("外盘"),
_T("DYNAINFO(24)---行情(涨速)"),0,_T("涨速"),

_T("DYNAINFO(25)---行情(买入量一)"),0,_T("买入量一"),
_T("DYNAINFO(26)---行情(买入量二)"),0,_T("买入量二"),
_T("DYNAINFO(27)---行情(买入量三)"),0,_T("买入量三"),
_T("DYNAINFO(28)---行情(买入价一)"),0,_T("买入价一"),
_T("DYNAINFO(29)---行情(买入价二)"),0,_T("买入价二"),
_T("DYNAINFO(30)---行情(买入价三)"),0,_T("买入价三"),

_T("DYNAINFO(31)---行情(卖出量一)"),0,_T("卖出量一"),
_T("DYNAINFO(32)---行情(卖出量二)"),0,_T("卖出量二"),
_T("DYNAINFO(33)---行情(卖出量三)"),0,_T("卖出量三"),
_T("DYNAINFO(34)---行情(卖出价一)"),0,_T("卖出价一"),
_T("DYNAINFO(35)---行情(卖出价二)"),0,_T("卖出价二"),
_T("DYNAINFO(36)---行情(卖出价三)"),0,_T("卖出价三"),

_T("DYNAINFO(37)---行情(换手率)"),0,_T("换手率"),
_T("DYNAINFO(38)---行情(5天均量)"),0,_T("5天均量"),
*/
/*	_T("FINANCE(1)---财务数据(总股本(万股))"),0,_T(""),
_T("FINANCE(2)---财务数据(国家股(万股))"),0,_T(""),
_T("FINANCE(3)---财务数据(发起人法人股(万股))"),0,_T(""),
_T("FINANCE(4)---财务数据(法人股(万股))"),0,_T(""),
_T("FINANCE(5)---财务数据(职工股(万股))"),0,_T(""),
_T("FINANCE(6)---财务数据(公众A股(万股))"),0,_T(""),
_T("FINANCE(7)---财务数据(流通A股(万股))"),0,_T(""),
_T("FINANCE(8)---财务数据(Ｈ股(万股))"),0,_T(""),
_T("FINANCE(9)---财务数据(B股(万股))"),0,_T(""),
_T("FINANCE(10)---财务数据(转配股(万股))"),0,_T(""),
_T("FINANCE(11)---财务数据(总资产(万元))"),0,_T(""),
_T("FINANCE(12)---财务数据(流动资产(万元))"),0,_T(""),
_T("FINANCE(13)---财务数据(长期投资(万元))"),0,_T(""),
_T("FINANCE(14)---财务数据(固定资产(万元))"),0,_T(""),
_T("FINANCE(15)---财务数据(无形资产(万元))"),0,_T(""),
_T("FINANCE(16)---财务数据(流动负债(万元))"),0,_T(""),
_T("FINANCE(17)---财务数据(长期负债(万元))"),0,_T(""),
_T("FINANCE(18)---财务数据(股东权益(万元))"),0,_T(""),
_T("FINANCE(19)---财务数据(资本公积金(万元))"),0,_T(""),
_T("FINANCE(20)---财务数据(盈余公积金(万元))"),0,_T(""),
_T("FINANCE(21)---财务数据(每股净值(元))"),0,_T(""),
_T("FINANCE(22)---财务数据(股东权益比率(%))"),0,_T(""),
_T("FINANCE(23)---财务数据(每股公积金(元))"),0,_T(""),
_T("FINANCE(24)---财务数据(主营业务收入(万元))"),0,_T(""),
_T("FINANCE(25)---财务数据(主营业务利润(万元))"),0,_T(""),
_T("FINANCE(26)---财务数据(其它业务利润(万元))"),0,_T(""),
_T("FINANCE(27)---财务数据(利润总额(万元))"),0,_T(""),
_T("FINANCE(28)---财务数据(净利润(万元))"),0,_T(""),
_T("FINANCE(29)---财务数据(未分配利润(万元))"),0,_T(""),
_T("FINANCE(30)---财务数据(每股收益(元))"),0,_T(""),
_T("FINANCE(31)---财务数据(净资产收益率(%))"),0,_T(""),
_T("FINANCE(32)---财务数据(每股未分配利润(元))"),0,_T(""),
_T("FINANCE(33)---财务数据(每股净资产(元))"),0,_T(""),
_T("FINANCE(1)---财务(总股本(万股))"),0,_T("总股本(万股)"),
_T("FINANCE(2)---财务(国家股(万股))"),0,_T("国家股(万股)"),
_T("FINANCE(3)---财务(发起人法人股(万股))"),0,_T("发起人法人股(万股)"),
_T("FINANCE(4)---财务(法人股(万股))"),0,_T("法人股(万股)"),
_T("FINANCE(5)---财务(B股(万股))"),0,_T("B股(万股)"),
_T("FINANCE(6)---财务(Ｈ股(万股))"),0,_T("Ｈ股(万股)"),
_T("FINANCE(7)---财务(流通A股(万股))"),0,_T("流通A股(万股)"),
_T("FINANCE(8)---财务(职工股(万股))"),0,_T("职工股(万股)"),
_T("FINANCE(9)---财务(转配股(万股))"),0,_T("转配股(万股)"),
_T("FINANCE(10)---财务(总资产(千元))"),0,_T("总资产(千元)"),
_T("FINANCE(11)---财务(流动资产)"),0,_T("流动资产"),
_T("FINANCE(12)---财务(固定资产)"),0,_T("固定资产"),
_T("FINANCE(14)---财务(无形资产)"),0,_T("无形资产"),
_T("FINANCE(15)---财务(长期投资)"),0,_T("长期投资"),
_T("FINANCE(13)---财务(流动负债)"),0,_T("流动负债"),
_T("FINANCE(16)---财务(长期负债)"),0,_T("长期负债"),
_T("FINANCE(17)---财务(资本公积金)    "),0,_T("(资本公积金)    "),
_T("FINANCE(18)---财务(每股公积金(元))"),0,_T("(每股公积金(元))"),
_T("FINANCE(19)---财务(股东权益)      "),0,_T("(股东权益)      "),
_T("FINANCE(20)---财务(主营业务收入)  "),0,_T("(主营业务收入)  "),
_T("FINANCE(21)---财务(主营业务利润)  "),0,_T("(主营业务利润)  "),
_T("FINANCE(22)---财务(其它业务利润)  "),0,_T("(其它业务利润)  "),
_T("FINANCE(23)---财务(利润总额)      "),0,_T("(利润总额)      "),
_T("FINANCE(24)---财务(投资收益)      "),0,_T("(投资收益)      "),
_T("FINANCE(25)---财务(补贴收入)      "),0,_T("(补贴收入)      "),
_T("FINANCE(26)---财务(营业外收支)    "),0,_T("(营业外收支)    "),
_T("FINANCE(27)---财务(上年损益调整)  "),0,_T("(上年损益调整)  "),
_T("FINANCE(28)---财务(投资收益)      "),0,_T("(投资收益)      "),
_T("FINANCE(29)---财务(利润总额)      "),0,_T("(利润总额)      "),
_T("FINANCE(30)---财务(税后利润)      "),0,_T("(税后利润)      "),
_T("FINANCE(31)---财务(净利润)        "),0,_T("(净利润)        "),
_T("FINANCE(32)---财务(未分配利润)    "),0,_T("(未分配利润)    "),
_T("FINANCE(33)---财务(每股未分配利润(元))"),0,_T("每股未分配利润(元)"),
_T("FINANCE(34)---财务(每股收益(元))"),0,_T("每股收益(元)"),
_T("FINANCE(35)---财务(每股净资产(元))"),0,_T("每股净资产(元)"),
_T("FINANCE(36)---财务(股东权益比率(%))"),0,_T("股东权益比率(%)"),
_T("FINANCE(37)---财务(净资产收益率(%))"),0,_T("净资产收益率(%)"),
*/
_T("绘图函数"),-1,_T(""),//14
_T("BUYCASE---买入条件"),0,_T("买入条件"),
_T("COLORSTICK---彩色条状线"),0,_T("彩色条状线"),
_T("CROSSDOT---圈叉图"),0,_T("圈叉图"),
_T("CIRCLEDOT---圆点图"),0,_T("圆点图"),
_T("LINESTICK---点状、柱状均画"),0,_T("点状、柱状均画"),
_T("SELLCASE---卖出条件"),0,_T("卖出条件"),
_T("STICK---普通条状线"),0,_T("普通条状线"),
_T("VOLSTICK---成交量柱状线"),0,_T("成交量柱状线"),

_T("DRAWICON(CON,PRICE,N)---当条件CON成立时,在价格PRICE处画第N个位图"),0,_T("在图形上绘制小图标.\r\n用法:\r\nDRAWICON(COND,PRICE,TYPE),当COND条件满足时,在PRICE位置画TYPE号图标.\r\n例如：DRAWICON(CLOSE>OPEN,LOW,1)表示当收阳时在最低价位置画1号图标."),
_T("DRAWLINE(CON1,PRICE1,CON2,PRICE2,N)---当条件CON1、CON2成立时,画价格PRICE1、PRICE2两点的连线"),0,_T("在图形上绘制直线段.\r\n用法:\r\nDRAWLINE(COND1,PRICE1,COND2,PRICE2,EXPAND),当COND1条件满足时,在PRICE1位置画直线起点,当COND2条件满足时,在PRICE2位置画直线终点,EXPAND为延长类型.\r\n例如：DRAWILINE(HIGH>=HHV(HIGH,20),HIGH,LOW<=LLV(LOW,20),LOW,1)表示在创20天新高与创20天新低之间画直线并且向右延长."),
_T("DRAWKLINE(HIGH,OPEN,LOW,CLOSE)---K线"),0,_T("DRAWKLINE(HIGH,OPEN,LOW,CLOSE).\r\n用法:以HIGH为最高价,OPEN为开盘价,LOW为最低,CLOSE收盘画K线"),
_T("DRAWKLINE2(HIGH,OPEN,LOW,CLOSE,DrawType)---K线"),0,_T("DRAWKLINE2(HIGH,OPEN,LOW,CLOSE,DrawType).绘制指定DrawType类型K线\r\nDrawType=0普通空心阳线, =1实心阳线, =2美国线\r\n用法:DRAWKLINE2(H,O,L,C,2) 以HIGH为最高价,OPEN为开盘价,LOW为最低,CLOSE收盘画美国K线"),
_T("DRAWTEXT(CON,PRICE,TEXT)---当条件CON成立时,在价格PRICE处画文本TEXT"),0,_T("在图形上显示文字.\r\n用法:\r\nDRAWTEXT(COND,PRICE,TEXT),当COND条件满足时,在PRICE位置书写文字TEXT.\r\n例如：DRAWTEXT(CLOSE/OPEN>1.08,LOW,'大阳线')表示当日涨幅大于8%时在最低价位置显示'大阳线'字样."),
_T("POLYLINE(CON,PRICE)---当CON条件满足时,以PRICE位置为顶点画折线连接"),0,_T("例如：POLYILINE(HIGH>=HHV(HIGH,20),HIGH)表示在创20天新高点之间画折线."),
_T("STICKLINE(COND,PRICE1,PRICE2,WIDTH,EMPTY)---当COND条件满足时,在PRICE1和PRICE2位置之间画柱状线,宽度为WIDTH(10为标准间距),EMPTH不为0则画空心柱"),0,_T("在图形上绘制柱线.\r\n用法:\r\nSTICKLINE(COND,PRICE1,PRICE2,WIDTH,EMPTY),当COND条件满足时,在PRICE1和PRICE2位置之间画柱状线,宽度为WIDTH(10为标准间距),EMPTH不为0则画空心柱.\r\n例如：STICKLINE(CLOSE>OPEN,CLOSE,OPEN,0.8,1)表示画K线中阳线的空心柱体部分."),
_T("NOTITLE---不显示标题"),0,_T("不显示标题."),
_T("NODRAW---不显示线条"),0,_T("不显示线条."),
//new
_T("颜色函数"),-1,_T(""),//9
_T("COLORRED---红色"),0,_T("如 vol,colorred 表示用红色画成交量"),
_T("COLORGREEN---绿色"),0,_T("绿色"),
_T("COLORBLUE---蓝色"),0,_T("蓝色"),
_T("COLORYELLOW---黄色"),0,_T("黄色"),
_T("COLORORANGE---橙色"),0,_T("橙色"),
_T("COLORPURPLE---紫色"),0,_T("紫色"),
_T("COLORWHITE---白色"),0,_T("白色"),
_T("COLORBLACK---黑色"),0,_T("黑色"),

//new
_T("分布展开函数"),-1,_T(""),//4  = EXTDATA
_T("DEPLOYREF(VOL,LOW,HIGH,N)---引用鼠标处的时间的前N周期的,把VOL均匀分布在LOW和HIGH价格之间的数据"),0,_T("把VOL均匀分布在LOW和HIGH价格之间的数据"),
_T("DEPLOYSUM(VOL,N)---引用鼠标处的时间的前N周期内的,VOL的均匀分布数据的累加和"),0,_T("VOL的均匀分布数据的累加和"),
_T("DEPLOYWIN(VOL,N,LTP)---引用鼠标处的时间的前N周期内的、以LTP为流通盘的、数值VOL的移动成本分布数据"),0,_T("引用鼠标处的时间的前N周期内的、以LTP为流通盘的、数值VOL的移动成本分布数据"),

};

int nItemCountText = sizeof(stritemsArr) / sizeof(dataFunc);

dataFunc *CDialogEDITZBGSHS::GetDataFunc()
{
	return stritemsArr;
}
int CDialogEDITZBGSHS::GetDataFuncCount()
{
	return nItemCountText;
}
BOOL CDialogEDITZBGSHS::OnInitDialog() 
{
	CDialog::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,FALSE);
	
	// TODO: Add extra initialization here
//	CMDIChildWnd* pchildfram= ((CMainFrame*)(AfxGetApp()->m_pMainWnd))->MDIGetActive();
//    CTaiShanDoc*	pDoc=CMainFrame::m_taiShanDoc ;//(CTaiShanDoc*)pchildfram->GetActiveDocument();

	CString             stritems[FUNCNUM];
	int                 item;

	//new 
//	m_tree.SetImageList(pDoc->m_imagelist,TVSIL_NORMAL);

	int nItem = nItemCountText;
	HTREEITEM hParent = 0;
	if(stritemsArr[0].nFlag  != -1) return TRUE;
	hParent = m_tree.InsertItem(_T("所有函数"),16,16);
	for(item=0;item<nItem;item++)
	{
		if(stritemsArr[item].nFlag == 0)
		{
			HTREEITEM h1 = m_tree.InsertItem(stritemsArr[item].s,8,8,hParent);
			m_tree.SetItemData (h1,item);
		}
	}
	m_tree.SortChildren(  hParent );

	for(item=0;item<nItem;item++)
	{
		if(stritemsArr[item].nFlag  == -1)
		{
			hParent=m_tree.InsertItem(stritemsArr[item].s ,16,16);
			m_tree.SetItemData (hParent,0);
		}
		else
		{
			HTREEITEM h1 = m_tree.InsertItem(stritemsArr[item].s ,8,8,hParent);
			m_tree.SetItemData (h1,item);
		}
	}
	
	m_ok.EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CDialogEDITZBGSHS::OnOK() 
{
	// TODO: Add extra validation here
	if(	m_selectedstr == _T("")) 
	{
		MessageBox(_T("请选择函数！"), AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return ;
	}
	CDialog::OnOK();
}

void CDialogEDITZBGSHS::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_selecteditem=m_tree.GetSelectedItem();
	if(m_tree.GetParentItem(m_selecteditem)==NULL)
	{
		if(m_ok.IsWindowEnabled())
			m_ok.EnableWindow(FALSE);
	}
	else
		m_ok.EnableWindow();
	m_selectedstr=m_tree.GetItemText(m_selecteditem);
	int nArr = m_tree.GetItemData (m_selecteditem);
	if(nArr>=0 && nArr<nItemCountText)
	{
		m_sExplain = stritemsArr[nArr].s2;
		UpdateData(FALSE);
	}
	
	*pResult = 0;
}

void CDialogEDITZBGSHS::OnDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(m_ok.IsWindowEnabled()) OnOK();
	*pResult = 0;
}

void CDialogEDITZBGSHS::OnReturnTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	if(m_ok.IsWindowEnabled()) OnOK();
	*pResult = 0;
}

BOOL CDialogEDITZBGSHS::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->hwnd == m_tree.m_hWnd)
	{
		if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==13)
		{
			if(m_ok.IsWindowEnabled()) 
			{
				OnOK();
			}
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CDialogEDITZBGSHS::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
//	DoHtmlHelp(this);
	return TRUE;
	
//	HtmlHelp(m_hWnd,_T("stock.chm"),HH_HELP_CONTEXT,CDialogEDITZBGSHS::IDD);
//	return CDialog::OnHelpInfo(pHelpInfo);
}
