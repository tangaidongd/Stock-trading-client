#ifndef _DEFINE_H_
#define _DEFINE_H_

#define BEGIN_NUMBER1	100

#define PARAM_NUM 16					// huhe* �ӵ�16����������
#define EXTRA_Y_NUM 4					// �����Y��������
#define NUM_LINE_TOT						64

#define MAXB 2000
#define MAXB_ALL 2000
#define MIDB 10000   
#define MIDE 10099

#define RLINEB 11000    
#define RLINEE 11000+NUM_LINE_TOT

#define CEFB 12000    //��������ʼ
#define CEFE 12299	 //����������

#define CEIB 13000    //������ʼ
#define CEIE 13299	//��������

#define CECB 13500    //�ַ�����ʼ
#define CECE 13520	//�ַ�������

#define FuncB 8000	//������ʼ
#define FuncE 8899	//��������
#define FuncDllB 8400	//������ʼ
#define FuncDllE 8700	//��������

#define PAM   9900  // ϵͳ���������ֵ

#define PCMB  14000	//�м������ʼ
#define PCME  14199	//�м��������

#define QUB  8800  //���ÿ�ʼ
#define QUE  8899  //������ֹ

#define TOZERO  9009  //
#define CONSB  9010  //���ͳ�����ʼ
#define CONSE  9498  //���ͳ�����ֹ
#define CONSCOLORB1  9100  //color������ʼ
#define CONSCOLORB  9200  //color������ʼ
#define CONSCOLORE  9280  //color������ʼ
#define CONSLINETHICKB  9300  //
#define CONSLINETHICKE  9320  //
#define CONSLINEDOT  9350  //
#define LEN_BYTE	240+136				// huhe* �ӵ�16����������

CString CTaiScreenParent_GetPeriodName(int nKlineType);
int CTaiScreenParent_FromKlineKindToFoot(int nKlineKind);
#endif//_DEFINE_H_