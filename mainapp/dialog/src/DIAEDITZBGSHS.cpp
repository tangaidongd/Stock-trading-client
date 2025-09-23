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
_T("���麯��"),-1,_T(""),//24
/*
_T("ACTIVEBUYVOL---����������"),0,_T("����������"),
_T("ACTIVESELLVOL---����������"),0,_T("����������"),
_T("ADVANCE---���ǹ�Ʊ��(����ָ��)"),0,_T("ȡ�ø��������Ǽ���.\r\n�÷�:\r\nADVANCE\r\n(���������Դ�����Ч)"),
_T("AMOUNT---�ɽ���"),0,_T("ȡ�ø����ڳɽ���.\r\n�÷�:\r\nAMOUNT"),
_T("ASKPRICE(1)---ί����1"),0,_T("ί����1(�������������ڷֱʳɽ�����������Ч)"),
_T("ASKPRICE(2)---ί����2"),0,_T("ί����2(�������������ڷֱʳɽ�����������Ч)"),
_T("ASKPRICE(3)---ί����3"),0,_T("ί����3(�������������ڷֱʳɽ�����������Ч)"),
_T("ASKVOL(1)---ί����1"),0,_T("ί����1(�������������ڷֱʳɽ�����������Ч)"),
_T("ASKVOL(2)---ί����2"),0,_T("ί����2(�������������ڷֱʳɽ�����������Ч)"),
_T("ASKVOL(3)---ί����3"),0,_T("ί����3(�������������ڷֱʳɽ�����������Ч)"),
_T("BIDVOL(1)---ί����1"),0,_T("ί����1(�������������ڷֱʳɽ�����������Ч)"),
_T("BIDVOL(2)---ί����2"),0,_T("ί����2(�������������ڷֱʳɽ�����������Ч)"),
_T("BIDVOL(3)---ί����3"),0,_T("ί����3(�������������ڷֱʳɽ�����������Ч)"),
_T("BIDPRICE(1)---ί���1"),0,_T("ί���1(�������������ڷֱʳɽ�����������Ч)"),
_T("BIDPRICE(2)---ί���2"),0,_T("ί���2(�������������ڷֱʳɽ�����������Ч)"),
_T("BIDPRICE(3)---ί���3"),0,_T("ί���3(�������������ڷֱʳɽ�����������Ч)"),
*/

//_T("ADVANCE---���ǹ�Ʊ��(����ָ��)"),0,_T("ȡ�ø��������Ǽ���.\r\n�÷�:\r\nADVANCE\r\n(���������Դ�����Ч)"),
_T("BUYVOL---�������ݵ�����������"),0,_T("ȡ������������.\r\n�÷�:\r\nBUYVOL\r\n�����ʳɽ�Ϊ����������ʱ,����ֵ���ڳɽ���,����Ϊ0\r\n(�������������ڷֱʳɽ�����������Ч)\r\n"),
_T("CLOSE---���̼�"),0,_T("ȡ�ø��������̼�.\r\n�÷�:\r\nCLOSE\r\n"),
_T("DECLINE---�µ���Ʊ��(����ָ��)"),0,_T("ȡ�ø������µ�����.\r\n�÷�:\r\nDECLINE\r\n(���������Դ�����Ч)"),
_T("HIGH---��߼�"),0,_T("ȡ�ø�������߼�.\r\n�÷�:\r\nHIGH"),
_T("ISBUYORDER---�Ƿ�����"),0,_T("ȡ�øóɽ��Ƿ�Ϊ��������.\r\n�÷�:\r\nISBUYORDER\r\n�����ʳɽ�Ϊ����������ʱ,����1,����Ϊ0\r\n(�������������ڷֱʳɽ�����������Ч)"),
_T("LOW---��ͼ�"),0,_T("ȡ�ø�������ͼ�.\r\n�÷�:\r\nLOW"),
_T("OPEN---���̼�"),0,_T("ȡ�ø����ڿ��̼�.\r\n�÷�:\r\nOPEN"),
_T("SELLVOL---�������ݵ�����������"),0,_T("ȡ��������������.\r\n�÷�:\r\nBUYVOL\r\n�����ʳɽ�Ϊ����������ʱ,����ֵ���ڳɽ���,����Ϊ0\r\n(�������������ڷֱʳɽ�����������Ч)\r\n"),
_T("VOL---�ɽ���"),0,_T("ȡ�ø����ڳɽ���.\r\n�÷�:\r\nVOL"),
_T("ʱ�亯��"),-1,_T(""),//8
_T("DAY---����"),0,_T("ȡ�ø����ڵ�����.\r\n�÷�:\r\nDAY\r\n����������Чֵ��ΧΪ(1-31)"),
_T("DATE---������,ȡ�ø����ڴ�1900������������"),0,_T("ȡ�ø����ڴ�1900������������.\r\n�÷�:\r\nDATE\r\n����������Чֵ��ΧΪ(700101-1341231),��ʾ19700101-20341231"),
_T("HOUR---Сʱ"),0,_T("ȡ�ø����ڵ�Сʱ��.\r\n�÷�:\r\nHOUR\r\n����������Чֵ��ΧΪ(0-23),�������߼������ķ�������ֵΪ0"),
_T("MINUTE---����"),0,_T("ȡ�ø����ڵķ�����.\r\n�÷�:\r\nMINUTE\r\n����������Чֵ��ΧΪ(0-59),�������߼������ķ�������ֵΪ0"),
_T("MONTH---�·�"),0,_T("ȡ�ø����ڵ��·�.\r\n�÷�:\r\nMONTH\r\n����������Чֵ��ΧΪ(1-12)"),
_T("TIME---ʱ����"),0,_T("ȡ�ø����ڵ�ʱ����.\r\n�÷�:\r\nTIME\r\n����������Чֵ��ΧΪ(000000-235959)"),
_T("VALUEDAY(DAY,HOUR,X)---DAY���»�������,HOUR��ʱ�ֻ�ʱ�ֵ�X��ֵ."),0,_T("�磺VALUEDAY(200103,210930,CLOSE) ��ʾ2001��3��21��9��30��ʱ�����̼۸�."),
_T("WEEK---����"),0,_T("ȡ�ø����ڵ��·�.\r\n�÷�:\r\nWEEK\r\n����������Чֵ��ΧΪ(0-6)"),
_T("YEAR---���"),0,_T("ȡ�ø����ڵ����.\r\n�÷�:\r\nYEAR\r\n����������Чֵ��ΧΪ(1970-2038)"),
_T("PERIOD---ȡ����������"),0,_T("ȡ����������.\r\n�����0��11,���ηֱ���1/5/15/30/60����,��/��/��,�����,����,��,��."),
_T("FROMOPEN---��ǰʱ�̾࿪���ж೤ʱ��"),0,_T("��ǰʱ�̾࿪���ж೤ʱ��.\r\n�÷�:\r\nFROMOPEN.���ص�ǰʱ�̾࿪���ж೤ʱ��,��λΪ����.����:��ǰʱ��Ϊ����ʮ�㣬�򷵻�31."),

_T("���ú���"),-1,_T(""),//20
_T("BACKSET(X,N)---��X��0,�򽫵�ǰλ�õ�N����ǰ����ֵ��Ϊ1"),0,_T("����ǰλ�õ���������ǰ��������Ϊ1.\r\n�÷�:\r\nBACKSET(X,N),��X��0,�򽫵�ǰλ�õ�N����ǰ����ֵ��Ϊ1.\r\n���磺BACKSET(CLOSE>OPEN,2)�������򽫸����ڼ�ǰһ������ֵ��Ϊ1,����Ϊ0"),
_T("BARSCOUNT(X)---��һ����Ч���ݵ���ǰ������"),0,_T("���ܵ�������.\r\n�÷�:\r\nBARSCOUNT(X)��һ����Ч���ݵ���ǰ������\r\n���磺BARSCOUNT(CLOSE)������������ȡ�����������ܽ�������,���ڷֱʳɽ�ȡ�õ��ճɽ�����,����1������ȡ�õ��ս��׷�����"),
_T("BARSLAST(X)---��һ��X��Ϊ0�����ڵ�����"),0,_T("��һ��������������ǰ��������.\r\n�÷�:\r\nBARSLAST(X):��һ��X��Ϊ0�����ڵ�����\r\n���磺BARSLAST(CLOSE/REF(CLOSE,1)>=1.1)��ʾ��һ����ͣ�嵽��ǰ��������"),
_T("BARSSINCE(X)---��һ��X��Ϊ0�����ڵ�����"),0,_T("���磺BARSSINCE(HIGH>10)��ʾ�ɼ۳���10Ԫʱ����ǰ��������"),
_T("COUNT(X,N)---ͳ��N����������X������������,��N=0��ӵ�һ����Чֵ��ʼ"),0,_T("��һ��������������ǰ��������.\r\n�÷�:\r\nBARSSINCE(X):��һ��X��Ϊ0�����ڵ�����\r\n���磺BARSSINCE(HIGH>10)��ʾ�ɼ۳���10Ԫʱ����ǰ��������"),
_T("DMA(X,A)---��X�Ķ�̬�ƶ�ƽ��"),0,_T("��̬�ƶ�ƽ��.\r\n�÷�:\r\nDMA(X,A),��X�Ķ�̬�ƶ�ƽ��.\r\n�㷨: ��Y=DMA(X,A)\r\n�� Y=A*X+(1-A)*Y',����Y'��ʾ��һ����Yֵ,A����С��1.\r\n���磺DMA(CLOSE,VOL/CAPITAL)��ʾ���Ի�������ƽ�����ӵ�ƽ����"),
_T("EMA(X,N)---����X��N��ָ��ƽ���ƶ�ƽ��ֵ"),0,_T("��ָ��ƽ���ƶ�ƽ��.\r\n�÷�:\r\nEMA(X,N),��X��N��ָ��ƽ���ƶ�ƽ��.�㷨����Y=EMA(X,N)\r\n��Y=[2*X+(N-1)*Y']/(N+1),����Y'��ʾ��һ����Yֵ.\r\n���磺EMA(CLOSE,30)��ʾ��30��ָ��ƽ������"),
_T("HHV(X,N)---����N����X�����ֵ"),0,_T("�����ֵ.\r\n�÷�:\r\nHHV(X,N),��N������X���ֵ,N=0��ӵ�һ����Чֵ��ʼ.\r\n���磺HHV(HIGH,30)��ʾ��30����߼�"),
_T("HHAV(X)---���㵱ǰ��������(����ǰ�����Ժ����������)��X�����ֵ"),0,_T("�������������ֵ.\r\n�÷�:\r\nHHAV(X),��ǰ��������������X���ֵ.\r\n���磺HHAV(HIGH)��ʾ������������߼�"),
_T("LLAV(X)---���㵱ǰ��������(����ǰ�����Ժ����������)��X����Сֵ"),0,_T("������������Сֵ.\r\n�÷�:\r\nLLAV(X),��ǰ��������������X��Сֵ.\r\n���磺LLAV(HIGH)��ʾ������������ͼ�"),
_T("HHVBARS(X,N)---��N������X���ֵ����ǰ������,N=0��ʾ�ӵ�һ����Чֵ��ʼͳ��"),0,_T("����һ�ߵ㵽��ǰ��������.\r\n�÷�:\r\nHHVBARS(X,N):��N������X���ֵ����ǰ������,N=0��ʾ�ӵ�һ����Чֵ��ʼͳ��\r\n���磺HHVBARS(HIGH,0)�����ʷ�¸ߵ�����ǰ��������"),
_T("LLV(X,N)---����N����X�����ֵ"),0,_T("�����ֵ.\r\n�÷�:\r\nLLV(X,N),��N������X���ֵ,N=0��ӵ�һ����Чֵ��ʼ.\r\n���磺LLV(LOW,0)��ʾ����ʷ��ͼ�"),
_T("LLVBARS(X,N)---��N������X���ֵ����ǰ������,N=0��ʾ�ӵ�һ����Чֵ��ʼͳ��"),0,_T("����һ�͵㵽��ǰ��������.\r\n�÷�:\r\nLLVBARS(X,N):��N������X���ֵ����ǰ������,N=0��ʾ�ӵ�һ����Чֵ��ʼͳ��\r\n���磺LLVBARS(HIGH,20)���20����͵㵽��ǰ��������"),
_T("MA(X,N)---����X��N���ƶ�ƽ��ֵ"),0,_T("����ƶ�ƽ��.\r\n�÷�:\r\nMA(X,N),��X��N���ƶ�ƽ��ֵ.�㷨��(X1+X2+X3+...+Xn)/N\r\n���磺MA(CLOSE,10)��ʾ��10�վ���"),
_T("REF(X,N)---����X��N��ǰ��ֵ"),0,_T("������������ǰ������.\r\n�÷�:\r\nREF(X,A),����A����ǰ��Xֵ.\r\n���磺REF(CLOSE,1)��ʾ��һ���ڵ����̼�,�������Ͼ�������"),
_T("REFBEGIN(X,N)---����X���������ݵĵ�N�����ݵ�ֵ"),0,_T("���磺REFBEGIN(CLOSE,1)��ʾCLOSE���������ݵĵ�1�����ݵ�ֵ"),
_T("REFLAST(X,N)---����X���������ݵĵ�����N�����ݵ�ֵ"),0,_T("���磺REFLAST(CLOSE,1)��ʾCLOSE���������ݵĵ�����1�����ݵ�ֵ"),
_T("SMA(X,N,M)---��X��N���ƶ�ƽ��,MΪȨ��"),0,_T("���ƶ�ƽ��.\r\n�÷�:\r\nSMA(X,N,M),��X��N���ƶ�ƽ��,MΪȨ��.\r\n�㷨: ��Y=SMA(X,N,M)\r\n�� Y=[M*X+(N-M)*Y')/N,����Y'��ʾ��һ����Yֵ,N�������M.\r\n���磺SMA(CLOSE,30,1)��ʾ��30���ƶ�ƽ����"),
_T("SUM(X,N)---����N����X���ܺ�ֵ"),0,_T("���ܺ�.\r\n�÷�:\r\nSUM(X,N),ͳ��N������X���ܺ�,N=0��ӵ�һ����Чֵ��ʼ.\r\n���磺SUM(VOL,0)��ʾͳ�ƴ����е�һ�������ĳɽ����ܺ�"),
_T("SUMBARS(X,A)---��X��ǰ�ۼ�ֱ�����ڵ���A,������������������"),0,_T("��ǰ�ۼӵ�ָ��ֵ�����ڵ�������.\r\n�÷�:\r\nSUMBARS(X,A):��X��ǰ�ۼ�ֱ�����ڵ���A,������������������\r\n���磺SUMBARS(VOL,CAPITAL)����ȫ���ֵ����ڵ�������"),
_T("EXTDATA(N)---����ID��ΪN����չָ������"),0,_T("ȡ��������չ����(1-999).\r\n�÷�:\r\nEXTDATA(N),Nȡ1--999\r\n(�������������߷���������Ч)"),
_T("DRAWNULL---������Ч��"),0,_T("������Ч��\r\n��ͼ���ϲ��������Ч���ݵĵ�"),
_T("CURRBARSCOUNT---��������յ�������"),0,_T("��������յ�������.\r\n�÷�:\r\n  CURRBARSCOUNT ��������յ�������"),
//					_T("reflast"),_T("refbegin"),_T("valueaverage")
_T("��������"),-1,_T(""),//9
_T("ABS(X)---X�ľ���ֵ"),0,_T("�����ֵ.\r\n�÷�:\r\nABS(X)����X�ľ���ֵ\r\n���磺ABS(-34)����34"),
_T("BETWEEN(A,B,C)---��ʾA����B��C֮��ʱ����1,���򷵻�0"),0,_T("����������֮��.\r\n�÷�:\r\nBETWEEN(A,B,C)��ʾA����B��C֮��ʱ����1,���򷵻�0\r\n���磺BETWEEN(CLOSE,MA(CLOSE,10),MA(CLOSE,5))��ʾ���̼۽���5�վ��ߺ�10�վ���֮��"),
_T("CROSS(A,B)---A�����ϴ�ԽB��Ϊ1,����Ϊ0"),0,_T("�����߽���.\r\n�÷�:\r\nCROSS(A,B)��ʾ��A���·����ϴ���Bʱ����1,���򷵻�0\r\n���磺CROSS(MA(CLOSE,5),MA(CLOSE,10))��ʾ5�վ�����10�վ��߽����"),
_T("FILTER(X,N)---X����������,ɾ�����N�����ڵ�������Ϊ0"),0,_T("�����������ֵ��ź�.\r\n�÷�:\r\nFILTER(X,N):X����������,ɾ�����N�����ڵ�������Ϊ0\r\n���磺FILTER(CLOSE>OPEN,5)��������,5�����ٴγ��ֵ����߲�����¼����"),
_T("LONGCROSS(A,B,N)---��ʾA��N�����ڶ�С��B,�����ڴ��·����ϴ���Bʱ����1,���򷵻�0"),0,_T("���磺LONGCROSS(MA(CLOSE,5),MA(CLOSE,10),5)��ʾ5�վ���ά��5���ں���10�վ��߽����"),
_T("MAX(X,N)---���ֵ����"),0,_T("�����ֵ.\r\n�÷�:\r\nMAX(A,B)����A��B�еĽϴ�ֵ\r\n���磺MAX(CLOSE-OPEN,0)��ʾ�����̼۴��ڿ��̼۷������ǵĲ�ֵ,���򷵻�0"),
_T("MIN(X,N)---��Сֵ����"),0,_T("����Сֵ.\r\n�÷�:\r\nMIN(A,B)����A��B�еĽ�Сֵ\r\n���磺MIN(CLOSE,OPEN)���ؿ��̼ۺ����̼��еĽ�Сֵ"),
_T("MOD(A,B)---ȡ��A��B��ģ"),0,_T("��ģ����.\r\n�÷�:\r\nMOD(A,B)����A��B��ģ\r\n���磺MOD(26,10)����6"),
_T("NOT(X)---ȡ���߼���"),0,_T("���߼���.\r\n�÷�:\r\nNOT(X)���ط�X,����X=0ʱ����1,���򷵻�0\r\n���磺NOT(ISUP)��ʾƽ�̻�����\r\n"),
_T("RANGE(A,B,C)---��ʾA����BͬʱС��Cʱ����1,���򷵻�0"),0,_T("����ĳ����Χ֮��.\r\n�÷�:\r\nRANGE(A,B,C)��ʾA����BͬʱС��Cʱ����1,���򷵻�0\r\n���磺RANGE(CLOSE,MA(CLOSE,5),MA(CLOSE,10))��ʾ���̼۴���5�վ��߲���С��10�վ���"),
_T("REVERSE(X)---ȡ��X���෴��-X"),0,_T("���෴��.\r\n�÷�:\r\nREVERSE(X)����-X\r\n����REVERSE(CLOSE)����-CLOSE"),
_T("SGN(X)---ȡ��X�ķ���,����(1,0,-1)"),0,_T("�����ֵ.\r\n�÷�:\r\nSGN(X),��X>0,X=0,X<0�ֱ𷵻�1,0,-1\r\n"),
_T("��ѧ����"),-1,_T(""),//13
_T("ACOS(X)---����X�ķ�����ֵ"),0,_T("������ֵ"),
_T("ASIN(X)---����X�ķ�����ֵ"),0,_T("������ֵ"),
_T("ATAN(X)---����X�ķ�����ֵ"),0,_T("������ֵ"),
_T("CEILING(X)---��������"),0,_T("���磺CEILING(12.3)���13,CEILING(-3.5)���-3"),
_T("COS(X)---����X������ֵ"),0,_T("����ֵ"),
_T("EXP(X)---��eΪ�׵�X����"),0,_T("EXP(CLOSE)����e��CLOSE����"),
_T("FLOOR(X)---��������"),0,_T("���磺FLOOR(12.3)���12,FLOOR(-3.5)���-4"),
_T("INTPART(X)---X������"),0,_T("���磺INTPART(12.3)���12,INTPART(-3.5)���-3"),
_T("LN(X)---X����eΪ�׵Ķ���"),0,_T("���磺LN(CLOSE)�����̼۵Ķ���"),
_T("LOG(X)---X����10Ϊ�׵Ķ���"),0,_T("���磺LOG(100)����2"),
_T("POW(A,B)---A��B����"),0,_T("���磺POW(CLOSE,3)������̼۵�3�η�"),
_T("SIN(X)---����X������ֵ"),0,_T("����ֵ"),
_T("SQRT(X)---X��ƽ����"),0,_T("���磺SQRT(CLOSE)���̼۵�ƽ����"),
_T("TAN(X)---����X������ֵ"),0,_T("����ֵ"),
_T("�߼�����"),-1,_T(""),//5
_T("IF(X,A,B)---��X�򷵻�A,���򷵻�B"),0,_T("����������ͬ��ֵ.\r\n�÷�:\r\nIF(X,A,B)��X��Ϊ0�򷵻�A,���򷵻�B\r\n���磺IF(CLOSE>OPEN,HIGH,LOW)��ʾ�����������򷵻����ֵ,���򷵻����ֵ"),
_T("ISDOWN---�����µ���Ϊ1,����Ϊ0"),0,_T("������<����ʱ,����ֵΪ1,����Ϊ0"),
_T("ISEQUAL---����ƽ����Ϊ1,����Ϊ0"),0,_T("������=����ʱ,����ֵΪ1,����Ϊ0"),
_T("ISUP---����������Ϊ1,����Ϊ0"),0,_T("������>����ʱ,����ֵΪ1,����Ϊ0"),
_T("ͳ�ƺ���"),-1,_T(""),//10
_T("AVEDEV(X,N)---����X��N���ƽ������ƫ��"),0,_T("ƽ������ƫ��.\r\n�÷�:\r\nAVEDEV(X,N)\r\n"),
_T("DEVSQ(X,N)---����X��N���ƫ��ƽ����"),0,_T("����ƫ��ƽ����.\r\n�÷�:\r\nDEVSQ(X,N)\r\n"),
_T("FORCAST(X,N)---X��N�������Իع�Ԥ��ֵ"),0,_T("���Իع�Ԥ��ֵ.\r\n�÷�:\r\nFORCAST(X,N)ΪX��N�������Իع�Ԥ��ֵ\r\n����:FORCAST(CLOSE,10)��ʾ��10�������Իع�Ԥ�Ȿ�������̼�"),
_T("STD(X,N)---����X��N��Ĺ����׼��"),0,_T("�����׼��.\r\n�÷�:\r\nSTD(X,N)ΪX��N�չ����׼��\r\n"),
_T("STDP(X,N)---����X��N��������׼��"),0,_T("�����׼��.\r\n�÷�:\r\nSTDP(X,N)ΪX��N�������׼��\r\n"),
_T("SLOPE(X,N)---X��N�������Իع��ߵ�б��"),0,_T("���Իع�б��.\r\n�÷�:\r\nSLOPE(X,N)ΪX��N�������Իع��ߵ�б��\r\n����:SLOPE(CLOSE,10)��ʾ��10�������Իع��ߵ�б��\r\n"),
_T("VALUEAVERAGE( PRICE,VOL,PERCENT)---����VOL��PERCENT����PRICE�ľ���"),0,_T(""),
_T("VAR(X,N)---����X��N��Ĺ�����������"),0,_T("������������.\r\n�÷�:\r\nVAR(X,N)ΪX��N�չ�����������\r\n"),
_T("VARP(X,N)---����X��N���������������"),0,_T("������������.\r\n�÷�:\r\nVARP(X,N)ΪX��N��������������"),
_T("ָ�꺯��"),-1,_T(""),//15
_T("COST(N)---��ʾN%�����̵ļ۸��Ƕ���"),0,_T("�ɱ��ֲ����.\r\n�÷�:\r\nCOST(10),��ʾ10%�����̵ļ۸��Ƕ���,����10%�ĳֲ����ڸü۸�����,����90%�ڸü۸�����,Ϊ������\r\n�ú����������߷���������Ч"),
_T("DOWNWARD(X,N)---X����N���½�"),0,_T("���磺DOWNWARD(CLOSE,5)��ʾCLOSE����5���½�"),
_T("PEAK(X,N,M)---ǰM��ZIGת�򲨷�ֵ"),0,_T("ǰM��ZIGת�򲨷�ֵ.\r\n�÷�:\r\nPEAK(K,N,M)��ʾ֮��ת��ZIG(K,N)��ǰM���������ֵ,M������ڵ���1\r\n���磺PEAK(1,5,1)��ʾ%5��߼�ZIGת�����һ���������ֵ"),
_T("PEAKBARS(X,N,M)---ǰM��ZIGת�򲨷嵽��ǰ����"),0,_T("ǰM��ZIGת�򲨷嵽��ǰ����.\r\n�÷�:\r\nPEAKBARS(K,N,M)��ʾ֮��ת��ZIG(K,N)��ǰM�����嵽��ǰ��������,M������ڵ���1\r\n���磺PEAK(0,5,1)��ʾ%5���̼�ZIGת�����һ�����嵽��ǰ��������"),
_T("SAR(N,S,M)---����ת��,N:���� S:���� M:��ֵ"),0,_T("����ת��.\r\n�÷�:\r\nSAR(N,S,M),NΪ��������,SΪ����,MΪ��ֵ\r\n����SAR(10,2,20)��ʾ����10������ת��,����Ϊ2%,����ֵΪ20%"),
_T("SARTURN(N,S,M)---����ת��λ��,N:���� S:���� M:��ֵ"),0,_T("����ת���.\r\n�÷�:\r\nSARTURN(N,S,M),NΪ��������,SΪ����,MΪ��ֵ,����������ת���򷵻�1,����������ת���򷵻�-1,����Ϊ0\r\n���÷���SAR������ͬ"),
_T("SLOWDOWNCROSS(N, X1,X2)---X1ά��N���ں��´�X2"),0,_T("X1ά��N���ں��´�X2"),
_T("SLOWUPCROSS(N, X1,X2)---X1ά��N���ں��ϴ�X2"),0,_T("X1ά��N���ں��ϴ�X2"),
_T("TROUGH(X,N,M)---��ʾ֮��ת��ZIG(X,N)��ǰM�����ȵ���ֵ,M������ڵ���1"),0,_T("ǰM��ZIGת�򲨹�ֵ.\r\n�÷�:\r\nTROUGH(K,N,M)��ʾ֮��ת��ZIG(K,N)��ǰM�����ȵ���ֵ,M������ڵ���1\r\n���磺TROUGH(2,5,2)��ʾ%5��ͼ�ZIGת���ǰ2�����ȵ���ֵ"),
_T("TROUGHBARS(X,N,M)---��ʾ֮��ת��ZIG(X,N)��ǰM�����ȵ���ǰ��������,M������ڵ���1"),0,_T("���磺TROUGH(2,5,2)��ʾ%5��ͼ�ZIGת���ǰ2�����ȵ���ǰ��������"),
_T("UPWARD(X,N)---X����N������"),0,_T("X����N������"),
_T("ZIG(X,N)---���۸�仯������N%ʱת��,X��ʾ������ֵ"),0,_T("֮��ת��.\r\n�÷�:\r\nZIG(K,N),���۸�仯������N%ʱת��,K��ʾ0:���̼�,1:��߼�,2:��ͼ�,3:���̼�\r\n���磺ZIG(3,5)��ʾ���̼۵�5%��ZIGת��"),
_T("TOWERVALUE(N)---�����ߵĸ�����ֵ,N = 1��ʾ�����ߵ����ֵ,N = 2��ʾ�����ߵ����ֵ,\r\nN = 3��ʾ�����ߵ��м�ֵ,N = 4��ʾ�����ߵ�״̬��1Ϊ��ɫ,2Ϊ��ɫ,3Ϊ���к�������ɫ��"),0,_T("�����ߵĸ�����ֵ"),
_T("WINNER(X)---�ֲ���X(ָ�����������)���ϵ����м�λ�ĳɽ����ĺ�ռ����ͨ�̵İٷֱ�"),0,_T("�÷�:\r\nWINNER(CLOSE),��ʾ�Ե�ǰ���м������Ļ����̱���,���緵��0.1��ʾ10%�����̣�WINNER(10.5)��ʾ10.5Ԫ�۸�Ļ����̱���\r\n�ú����������߷���������Ч"),
_T("WINNERTYPE(N)---��ͨ�̷ֲ��ຯ��,N=1Ϊ��߷崦�ļ۸�,N=2Ϊ��߷崦�ĳɽ����ܶȣ���/�֣�,N=3Ϊƽ���ɱ����ļ۸�"),0,_T("��ͨ�̷ֲ��ຯ��"),

_T("���̺���"),-1,_T(""),//9 
_T("INDEXA---ָ���ɽ���"),0,_T("ָ���ɽ���"),
_T("INDEXADV---�������Ǽ���"),0,_T("�������Ǽ���"),
_T("INDEXC---ָ�����̼�"),0,_T("ָ�����̼�"),
_T("INDEXDEC---�����µ�����"),0,_T("�����µ�����"),
_T("INDEXH---ָ����߼�"),0,_T("ָ����߼�"),
_T("INDEXL---ָ����ͼ�"),0,_T("ָ����ͼ�"),
_T("INDEXO---ָ�����̼�"),0,_T("ָ�����̼�"),
_T("INDEXV---ָ���ɽ���"),0,_T("ָ���ɽ���"),

_T("��������"),-1,_T(""),//75
_T("CAPITAL---��ͨ��"),0,_T("��ͨ��"),
_T("TOTALCAPITAL---��ǰ�ܹɱ����֣�"),0,_T("��ǰ�ܹɱ����֣�"),
_T("HYZSCODE---������ҵ�İ��ָ������ "),0,_T("������ҵ�İ��ָ������ "),

// �������� ���������ຯ��
_T("TOPRANGE(X)---��ǰֵ�ǽ����������ڵ����ֵ"),0,_T("�÷���\r\n TOPRANGE(X):X�ǽ����������ڵ�X�����ֵ\r\n ���磺\r\n TOPRANGE(HIGH)��ʾ��ǰ��߼��ǽ����������ڵ���߼۵����ֵ"),
_T("LOWRANGE(X)---��ǰֵ�ǽ����������ڵ���Сֵ"),0,_T("�÷���\r\n LOWRANGE(X):X�ǽ����������ڵ�X����Сֵ\r\n ���磺\r\n TOPRANGE(LOW)��ʾ��ǰ��ͼ��ǽ����������ڵ���ͼ۵���Сֵ"),
_T("FINDHIGH(VAR,N,M,T)---N����ǰM�����ڵĵ�T�����ֵ"),0,_T("�÷���\r\n FINDHIGH(VAR,N,M,T):VAR��N��ǰ��M���ڵ�T����߼�"),
_T("FINDHIGHBARS(VAR,N,M,T)---N����ǰM�����ڵĵ�T�����ֵ����ǰ���ڵ�������"),0,_T("�÷���\r\n FINDHIGHBARS(VAR,N,M,T):VAR��N��ǰ��M���ڵ�T����߼۵���ǰ���ڵ�������"),
_T("FINDLOW(VAR,N,M,T)---N����ǰM�����ڵĵ�T����Сֵ"),0,_T("�÷���\r\n FINDLOW(VAR,N,M,T):VAR��N��ǰ��M���ڵ�T����ͼ�"),
_T("FINDLOWBARS(VAR,N,M,T)---N����ǰM�����ڵĵ�T����Сֵ����ǰ���ڵ�������"),0,_T("�÷���\r\n FINDHIGHBARS(VAR,N,M,T):VAR��N��ǰ��M���ڵ�T����ͼ۵���ǰ���ڵ�������"),




// 
/*
_T("VOLUNIT---ÿ�ֹ���"),0,_T("ÿ�ֹ���"),
_T("DYNAINFO(3)---����(��������)"),0,_T("��������"),
_T("DYNAINFO(4)---����(���տ���)"),0,_T("���տ���"),
_T("DYNAINFO(5)---����(�������)"),0,_T("�������"),
_T("DYNAINFO(6)---����(�������)"),0,_T("�������"),
_T("DYNAINFO(7)---����(���¼۸�)"),0,_T("���¼۸�"),
_T("DYNAINFO(8)---����(�ܳɽ���)"),0,_T("�ܳɽ���"),
_T("DYNAINFO(9)---����(���³ɽ���)"),0,_T("���³ɽ���"),
_T("DYNAINFO(10)---����(�ܳɽ����)"),0,_T("�ܳɽ����"),
_T("DYNAINFO(11)---����(����)"),0,_T("����"),
_T("DYNAINFO(12)---����(�ǵ�)"),0,_T("�ǵ�"),
_T("DYNAINFO(13)---����(���)"),0,_T("���"),
_T("DYNAINFO(14)---����(�Ƿ�)"),0,_T("�Ƿ�"),
_T("DYNAINFO(15)---����(ί��)"),0,_T("ί��"),
_T("DYNAINFO(16)---����(ί��)"),0,_T("ί��"),
_T("DYNAINFO(17)---����(����)"),0,_T("����"),
_T("DYNAINFO(18)---����(ί��)"),0,_T("ί��"),
_T("DYNAINFO(19)---����(ί��)"),0,_T("ί��"),
_T("DYNAINFO(20)---����(ί���)"),0,_T("ί���"),
_T("DYNAINFO(21)---����(ί����)"),0,_T("ί����"),
_T("DYNAINFO(22)---����(����)"),0,_T("����"),
_T("DYNAINFO(23)---����(����)"),0,_T("����"),
_T("DYNAINFO(24)---����(����)"),0,_T("����"),

_T("DYNAINFO(25)---����(������һ)"),0,_T("������һ"),
_T("DYNAINFO(26)---����(��������)"),0,_T("��������"),
_T("DYNAINFO(27)---����(��������)"),0,_T("��������"),
_T("DYNAINFO(28)---����(�����һ)"),0,_T("�����һ"),
_T("DYNAINFO(29)---����(����۶�)"),0,_T("����۶�"),
_T("DYNAINFO(30)---����(�������)"),0,_T("�������"),

_T("DYNAINFO(31)---����(������һ)"),0,_T("������һ"),
_T("DYNAINFO(32)---����(��������)"),0,_T("��������"),
_T("DYNAINFO(33)---����(��������)"),0,_T("��������"),
_T("DYNAINFO(34)---����(������һ)"),0,_T("������һ"),
_T("DYNAINFO(35)---����(�����۶�)"),0,_T("�����۶�"),
_T("DYNAINFO(36)---����(��������)"),0,_T("��������"),

_T("DYNAINFO(37)---����(������)"),0,_T("������"),
_T("DYNAINFO(38)---����(5�����)"),0,_T("5�����"),
*/
/*	_T("FINANCE(1)---��������(�ܹɱ�(���))"),0,_T(""),
_T("FINANCE(2)---��������(���ҹ�(���))"),0,_T(""),
_T("FINANCE(3)---��������(�����˷��˹�(���))"),0,_T(""),
_T("FINANCE(4)---��������(���˹�(���))"),0,_T(""),
_T("FINANCE(5)---��������(ְ����(���))"),0,_T(""),
_T("FINANCE(6)---��������(����A��(���))"),0,_T(""),
_T("FINANCE(7)---��������(��ͨA��(���))"),0,_T(""),
_T("FINANCE(8)---��������(�ȹ�(���))"),0,_T(""),
_T("FINANCE(9)---��������(B��(���))"),0,_T(""),
_T("FINANCE(10)---��������(ת���(���))"),0,_T(""),
_T("FINANCE(11)---��������(���ʲ�(��Ԫ))"),0,_T(""),
_T("FINANCE(12)---��������(�����ʲ�(��Ԫ))"),0,_T(""),
_T("FINANCE(13)---��������(����Ͷ��(��Ԫ))"),0,_T(""),
_T("FINANCE(14)---��������(�̶��ʲ�(��Ԫ))"),0,_T(""),
_T("FINANCE(15)---��������(�����ʲ�(��Ԫ))"),0,_T(""),
_T("FINANCE(16)---��������(������ծ(��Ԫ))"),0,_T(""),
_T("FINANCE(17)---��������(���ڸ�ծ(��Ԫ))"),0,_T(""),
_T("FINANCE(18)---��������(�ɶ�Ȩ��(��Ԫ))"),0,_T(""),
_T("FINANCE(19)---��������(�ʱ�������(��Ԫ))"),0,_T(""),
_T("FINANCE(20)---��������(ӯ�๫����(��Ԫ))"),0,_T(""),
_T("FINANCE(21)---��������(ÿ�ɾ�ֵ(Ԫ))"),0,_T(""),
_T("FINANCE(22)---��������(�ɶ�Ȩ�����(%))"),0,_T(""),
_T("FINANCE(23)---��������(ÿ�ɹ�����(Ԫ))"),0,_T(""),
_T("FINANCE(24)---��������(��Ӫҵ������(��Ԫ))"),0,_T(""),
_T("FINANCE(25)---��������(��Ӫҵ������(��Ԫ))"),0,_T(""),
_T("FINANCE(26)---��������(����ҵ������(��Ԫ))"),0,_T(""),
_T("FINANCE(27)---��������(�����ܶ�(��Ԫ))"),0,_T(""),
_T("FINANCE(28)---��������(������(��Ԫ))"),0,_T(""),
_T("FINANCE(29)---��������(δ��������(��Ԫ))"),0,_T(""),
_T("FINANCE(30)---��������(ÿ������(Ԫ))"),0,_T(""),
_T("FINANCE(31)---��������(���ʲ�������(%))"),0,_T(""),
_T("FINANCE(32)---��������(ÿ��δ��������(Ԫ))"),0,_T(""),
_T("FINANCE(33)---��������(ÿ�ɾ��ʲ�(Ԫ))"),0,_T(""),
_T("FINANCE(1)---����(�ܹɱ�(���))"),0,_T("�ܹɱ�(���)"),
_T("FINANCE(2)---����(���ҹ�(���))"),0,_T("���ҹ�(���)"),
_T("FINANCE(3)---����(�����˷��˹�(���))"),0,_T("�����˷��˹�(���)"),
_T("FINANCE(4)---����(���˹�(���))"),0,_T("���˹�(���)"),
_T("FINANCE(5)---����(B��(���))"),0,_T("B��(���)"),
_T("FINANCE(6)---����(�ȹ�(���))"),0,_T("�ȹ�(���)"),
_T("FINANCE(7)---����(��ͨA��(���))"),0,_T("��ͨA��(���)"),
_T("FINANCE(8)---����(ְ����(���))"),0,_T("ְ����(���)"),
_T("FINANCE(9)---����(ת���(���))"),0,_T("ת���(���)"),
_T("FINANCE(10)---����(���ʲ�(ǧԪ))"),0,_T("���ʲ�(ǧԪ)"),
_T("FINANCE(11)---����(�����ʲ�)"),0,_T("�����ʲ�"),
_T("FINANCE(12)---����(�̶��ʲ�)"),0,_T("�̶��ʲ�"),
_T("FINANCE(14)---����(�����ʲ�)"),0,_T("�����ʲ�"),
_T("FINANCE(15)---����(����Ͷ��)"),0,_T("����Ͷ��"),
_T("FINANCE(13)---����(������ծ)"),0,_T("������ծ"),
_T("FINANCE(16)---����(���ڸ�ծ)"),0,_T("���ڸ�ծ"),
_T("FINANCE(17)---����(�ʱ�������)    "),0,_T("(�ʱ�������)    "),
_T("FINANCE(18)---����(ÿ�ɹ�����(Ԫ))"),0,_T("(ÿ�ɹ�����(Ԫ))"),
_T("FINANCE(19)---����(�ɶ�Ȩ��)      "),0,_T("(�ɶ�Ȩ��)      "),
_T("FINANCE(20)---����(��Ӫҵ������)  "),0,_T("(��Ӫҵ������)  "),
_T("FINANCE(21)---����(��Ӫҵ������)  "),0,_T("(��Ӫҵ������)  "),
_T("FINANCE(22)---����(����ҵ������)  "),0,_T("(����ҵ������)  "),
_T("FINANCE(23)---����(�����ܶ�)      "),0,_T("(�����ܶ�)      "),
_T("FINANCE(24)---����(Ͷ������)      "),0,_T("(Ͷ������)      "),
_T("FINANCE(25)---����(��������)      "),0,_T("(��������)      "),
_T("FINANCE(26)---����(Ӫҵ����֧)    "),0,_T("(Ӫҵ����֧)    "),
_T("FINANCE(27)---����(�����������)  "),0,_T("(�����������)  "),
_T("FINANCE(28)---����(Ͷ������)      "),0,_T("(Ͷ������)      "),
_T("FINANCE(29)---����(�����ܶ�)      "),0,_T("(�����ܶ�)      "),
_T("FINANCE(30)---����(˰������)      "),0,_T("(˰������)      "),
_T("FINANCE(31)---����(������)        "),0,_T("(������)        "),
_T("FINANCE(32)---����(δ��������)    "),0,_T("(δ��������)    "),
_T("FINANCE(33)---����(ÿ��δ��������(Ԫ))"),0,_T("ÿ��δ��������(Ԫ)"),
_T("FINANCE(34)---����(ÿ������(Ԫ))"),0,_T("ÿ������(Ԫ)"),
_T("FINANCE(35)---����(ÿ�ɾ��ʲ�(Ԫ))"),0,_T("ÿ�ɾ��ʲ�(Ԫ)"),
_T("FINANCE(36)---����(�ɶ�Ȩ�����(%))"),0,_T("�ɶ�Ȩ�����(%)"),
_T("FINANCE(37)---����(���ʲ�������(%))"),0,_T("���ʲ�������(%)"),
*/
_T("��ͼ����"),-1,_T(""),//14
_T("BUYCASE---��������"),0,_T("��������"),
_T("COLORSTICK---��ɫ��״��"),0,_T("��ɫ��״��"),
_T("CROSSDOT---Ȧ��ͼ"),0,_T("Ȧ��ͼ"),
_T("CIRCLEDOT---Բ��ͼ"),0,_T("Բ��ͼ"),
_T("LINESTICK---��״����״����"),0,_T("��״����״����"),
_T("SELLCASE---��������"),0,_T("��������"),
_T("STICK---��ͨ��״��"),0,_T("��ͨ��״��"),
_T("VOLSTICK---�ɽ�����״��"),0,_T("�ɽ�����״��"),

_T("DRAWICON(CON,PRICE,N)---������CON����ʱ,�ڼ۸�PRICE������N��λͼ"),0,_T("��ͼ���ϻ���Сͼ��.\r\n�÷�:\r\nDRAWICON(COND,PRICE,TYPE),��COND��������ʱ,��PRICEλ�û�TYPE��ͼ��.\r\n���磺DRAWICON(CLOSE>OPEN,LOW,1)��ʾ������ʱ����ͼ�λ�û�1��ͼ��."),
_T("DRAWLINE(CON1,PRICE1,CON2,PRICE2,N)---������CON1��CON2����ʱ,���۸�PRICE1��PRICE2���������"),0,_T("��ͼ���ϻ���ֱ�߶�.\r\n�÷�:\r\nDRAWLINE(COND1,PRICE1,COND2,PRICE2,EXPAND),��COND1��������ʱ,��PRICE1λ�û�ֱ�����,��COND2��������ʱ,��PRICE2λ�û�ֱ���յ�,EXPANDΪ�ӳ�����.\r\n���磺DRAWILINE(HIGH>=HHV(HIGH,20),HIGH,LOW<=LLV(LOW,20),LOW,1)��ʾ�ڴ�20���¸��봴20���µ�֮�仭ֱ�߲��������ӳ�."),
_T("DRAWKLINE(HIGH,OPEN,LOW,CLOSE)---K��"),0,_T("DRAWKLINE(HIGH,OPEN,LOW,CLOSE).\r\n�÷�:��HIGHΪ��߼�,OPENΪ���̼�,LOWΪ���,CLOSE���̻�K��"),
_T("DRAWKLINE2(HIGH,OPEN,LOW,CLOSE,DrawType)---K��"),0,_T("DRAWKLINE2(HIGH,OPEN,LOW,CLOSE,DrawType).����ָ��DrawType����K��\r\nDrawType=0��ͨ��������, =1ʵ������, =2������\r\n�÷�:DRAWKLINE2(H,O,L,C,2) ��HIGHΪ��߼�,OPENΪ���̼�,LOWΪ���,CLOSE���̻�����K��"),
_T("DRAWTEXT(CON,PRICE,TEXT)---������CON����ʱ,�ڼ۸�PRICE�����ı�TEXT"),0,_T("��ͼ������ʾ����.\r\n�÷�:\r\nDRAWTEXT(COND,PRICE,TEXT),��COND��������ʱ,��PRICEλ����д����TEXT.\r\n���磺DRAWTEXT(CLOSE/OPEN>1.08,LOW,'������')��ʾ�����Ƿ�����8%ʱ����ͼ�λ����ʾ'������'����."),
_T("POLYLINE(CON,PRICE)---��CON��������ʱ,��PRICEλ��Ϊ���㻭��������"),0,_T("���磺POLYILINE(HIGH>=HHV(HIGH,20),HIGH)��ʾ�ڴ�20���¸ߵ�֮�仭����."),
_T("STICKLINE(COND,PRICE1,PRICE2,WIDTH,EMPTY)---��COND��������ʱ,��PRICE1��PRICE2λ��֮�仭��״��,���ΪWIDTH(10Ϊ��׼���),EMPTH��Ϊ0�򻭿�����"),0,_T("��ͼ���ϻ�������.\r\n�÷�:\r\nSTICKLINE(COND,PRICE1,PRICE2,WIDTH,EMPTY),��COND��������ʱ,��PRICE1��PRICE2λ��֮�仭��״��,���ΪWIDTH(10Ϊ��׼���),EMPTH��Ϊ0�򻭿�����.\r\n���磺STICKLINE(CLOSE>OPEN,CLOSE,OPEN,0.8,1)��ʾ��K�������ߵĿ������岿��."),
_T("NOTITLE---����ʾ����"),0,_T("����ʾ����."),
_T("NODRAW---����ʾ����"),0,_T("����ʾ����."),
//new
_T("��ɫ����"),-1,_T(""),//9
_T("COLORRED---��ɫ"),0,_T("�� vol,colorred ��ʾ�ú�ɫ���ɽ���"),
_T("COLORGREEN---��ɫ"),0,_T("��ɫ"),
_T("COLORBLUE---��ɫ"),0,_T("��ɫ"),
_T("COLORYELLOW---��ɫ"),0,_T("��ɫ"),
_T("COLORORANGE---��ɫ"),0,_T("��ɫ"),
_T("COLORPURPLE---��ɫ"),0,_T("��ɫ"),
_T("COLORWHITE---��ɫ"),0,_T("��ɫ"),
_T("COLORBLACK---��ɫ"),0,_T("��ɫ"),

//new
_T("�ֲ�չ������"),-1,_T(""),//4  = EXTDATA
_T("DEPLOYREF(VOL,LOW,HIGH,N)---������괦��ʱ���ǰN���ڵ�,��VOL���ȷֲ���LOW��HIGH�۸�֮�������"),0,_T("��VOL���ȷֲ���LOW��HIGH�۸�֮�������"),
_T("DEPLOYSUM(VOL,N)---������괦��ʱ���ǰN�����ڵ�,VOL�ľ��ȷֲ����ݵ��ۼӺ�"),0,_T("VOL�ľ��ȷֲ����ݵ��ۼӺ�"),
_T("DEPLOYWIN(VOL,N,LTP)---������괦��ʱ���ǰN�����ڵġ���LTPΪ��ͨ�̵ġ���ֵVOL���ƶ��ɱ��ֲ�����"),0,_T("������괦��ʱ���ǰN�����ڵġ���LTPΪ��ͨ�̵ġ���ֵVOL���ƶ��ɱ��ֲ�����"),

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
	hParent = m_tree.InsertItem(_T("���к���"),16,16);
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
		MessageBox(_T("��ѡ������"), AfxGetApp()->m_pszAppName, MB_ICONWARNING);
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
