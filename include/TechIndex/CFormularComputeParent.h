// com.h: interface for the CFormularComputeParent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COM_H__80BFF943_AB2A_11D1_8C43_0000E823CF90__INCLUDED_)
#define AFX_COM_H__80BFF943_AB2A_11D1_8C43_0000E823CF90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include <math.h>
#include "typedef.h"
#include "MerchManager.h"
#include "StructKlineView.h"
//#include "SelectStockStruct.h"
#include "TimeInterval.h"
#include "TechExport.h"
#include "SpecialTechIndexManager.h"

typedef struct MerchExtraInfo
{
	int32 iShowCountInFullList;
	int32 iShowPosInFullList;
	E_NodeTimeInterval eTimeIntervalFull;
	int32	iTimeUserMultipleMinutes;		// �Զ������������
	int32	iTimeUserMultipleDays;			// �Զ�����������
	CMerch *pMerchNode;
	CMerch *pMainExpMerchNode;				// ����
	MerchExtraInfo()
	{
		iShowCountInFullList = 0;
		iShowPosInFullList = 0;
		iTimeUserMultipleMinutes = 0;
		iTimeUserMultipleDays = 0;
		pMerchNode = NULL;
		pMainExpMerchNode = NULL;
	}
}MERCH_EXTRA_INFO;



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

template <class TY> class Intem
{
public:
	Intem(TY v=0){val=v;next=0;prev=0;}
	Intem<TY>* next;
	Intem<TY>* prev;
	TY val;
};

template <class TH> class Intst 
{
public:
	Intst(TH val) { list = new Intem(val);tail=0;};
	Intst() {list = 0;tail=0;};
//===========ѹ��ͷ��=============
	TH push_b(TH val)
	{
		Intem<TH>* pt;
		pt=new Intem<TH>(val);
		pt->next = list;
		if(list!=0) 
			list->prev = pt;
		else
			tail = pt;
		list = pt;
		return val;
	}
//==========ѹ��β��================
/*	TH push_t(TH val)
	{
		Intem<TH> *pi,*pt,*prv;
		for(prv=pi=list;pi;prv=pi,pi=pi->next)  ;
    
		pt=new Intem<TH>(val);	
		if(list==0)
			list=pt;
		else
		{
			pt->prev=prv;
			prv->next=pt;
		}
		return val;
	}
*/
//========����===============
	TH pop_it()
	{
	Intem<TH>* tmp;
	TH val;
	if(list==0)
		return 0;
	tmp=list;
	list=list->next;
	if(list!=0)
		list->prev=0;
	else
		tail=0;
	val = tmp->val;
	delete tmp;
	return val;
	}
//========ѹ��===============
	TH rec_it()
	{
		Intem<TH>* tmp;
		TH val;
		if(tail==0)
			return 0;
		tmp=tail;
		tail=tail->prev;
		if(tail!=0) 
			tail->next=0;
		else
			list=0;
		val = tmp->val;
		delete tmp;
		return val;
	}
private:
	Intem<TH>* list;
	Intem<TH>* tail;
};

class CFormularContent;
struct T_MerchNodeUserData;
struct INPUT_PARAM
{
	int pmax;
	Kline* pp;
	CFormularContent* pIndex;
	//T_MerchNodeUserData *pUserData;	// ���������Ʒ�й�����
	MERCH_EXTRA_INFO *pExtraInfo;
};

class EXPORT_CLASS CFormularComputeParent  
{
public:	
	bool m_bShowColor;//�Ƿ���ʾ��ɫ
	enum ColorKind {
		digit = 0,
		operrator ,
		variable ,
		function,
		quotevar,
		};
	struct KindPos
	{
		int m_nKind;//Ԫ������
		int m_nPos;//Ԫ�ؽ���λ��
	};
	int m_nSizeKindPos;//m_pKindPosԪ�ظ���
	KindPos* m_pKindPos;//
	int m_nPosPre;//ǰһ��Ԫ�ص�λ��

	int GetIndexPoint(CString sName,float &nPoint);
	ARRAY_BE  m_Rline[RLINEE-RLINEB];  //���ر���
	CString m_RlineName[RLINEE-RLINEB];  //���ر�������
	int     m_RlineNum;              //���ر�������

	CString m_formuCompute;   //������ʽ
	CString m_errmsg;   //������Ϣ
	int     m_errpos;	//����λ��

	bool m_bTotalData;
	int m_isDaPan;	//�Ƿ��Ǵ���ָ��
	int m_RlineType[NUM_LINE_TOT];	//���ص��ߵ�����
	int m_RlineColor[NUM_LINE_TOT];	//���ص��ߵ�����
	int m_RlineThick[NUM_LINE_TOT];	//���ص��ߵĿ��
	int m_RlineTypeEx[NUM_LINE_TOT];//���ص��ߵ�����(NODRAW,NOTITLE��) ������ m_RlineType ������ʾ.

	CString GetRlineName(int foot);//�õ��������ݵ�����
	//�������:foot ��ʾָ���ڼ�������
	//�������:����ָ�������ݵ�����
	void IsDaPan();	//�ж��Ƿ��Ǵ���ָ��
	int FindLineType();//���㷵���ߵ�����
	int YhhCheck(int j, int k);
						//rtn = 0 is successful
	//�������:pEqution �����ڼ����ָ����ͼ������,pEqution�ĳ�ʼֵ��NULL,��������ʱ����ɾ��pEqution����;inputʱ����ĳ�ʼ������
	//�������:����0��ʾ�ɹ�
	int GetRlineNum();//�õ�ָ�깫ʽ����������ĸ���
	int GetNum(int tj);//�õ�ĳ��Ԫ�����Ĳ�������
	//�������:tj��ʶ�ض���Ԫ����
	//�������:����Ԫ�����Ĳ�������
	CFormularComputeParent(int pmin,int pmax,Kline* pp,CString& str); 
	CFormularComputeParent(int pmin,int pmax,Kline* pp,CFormularContent* pIndex);
	CFormularComputeParent();
		//pmin ΪK�����ݵ���С�±�,pmaxΪK�����ݵ�����±�,ppΪK�����ݵ�ָ��,pIndexΪ����ָ������ָ��

		//����˵����pmin ΪK�����ݵ���С�±�,pmaxΪK�����ݵ�����±�,ppΪK�����ݵ�ָ��,pIndexΪ����ָ������ָ��

	virtual ~CFormularComputeParent();
	int AddPara(CString pa,int va);   //�������Ͳ���
	int AddPara(CString pa,float va); //���Ӹ������
	int AddLine(CString& LineName,ARRAY_BE& pline);
									  //�����߲���

	void SetScreenBeginEnd(int iBegin, int iEnd);
	void GetMsg(CString& str,int& pos);//ȡ�ô�����Ϣ
//	      ����˵����str�Ǵ�����Ϣ�ַ���,pos �Ǵ���λ��
//        ���˵�����ޡ�
	int GetLine(int& index,ARRAY_BE& pline,CString& name);
									  //ȡ�ü�����
	//�������:index ��ʾָ���ڼ�������
	//�������:pline�����������,name���������������,����ָ�������ݵ�����
	int GetIndexPoint(int &index,float &nPoint);

	int GetLine(CString& LineName,ARRAY_BE& pline);
									  //ȡ�ü�����
	int  FormularCompute();    //����

	int Relolve(int cl);//�Թ�ʽ���ý��д���
	//�������:cl ��ʾ�ض��Ĺ�ʽ����
	//�������:���ش�����,0Ϊ��ȷ,1Ϊ����
	void Devide();     //�ֽ⹫ʽ�ɿ���˳�����Ĺؼ���
	void GetData(int cl,ARRAY_BE& pp);
	int Sort(int bi,int ej);     //����˳��ȷ��
	
	int CheckWord(CString emp);	 //�ʷ����	

	//�������:emp ������Ĺ�ʽ�ַ���
	//�������:���ش�������λ��
	// fangz0926 char->TCHAR
	int GetCharClass(TCHAR ch);   //ȷ���ַ����
	//int GetCharClass(TCHAR ch);   //ȷ���ַ����
	int CheckSent();			 //�﷨���	

	void SetVolType(bool bTotalVol); 

	void SetMerchNodeUserData(MERCH_EXTRA_INFO *pExtraInfo);	// ��ǰ������Ʒ�йص�����
	MERCH_EXTRA_INFO *GetMerchNodeUserData() const {return m_pExtraInfo; }
protected:
	int m_nCurrentFoot;//for right box funcs
	int GetBeginFoot(ARRAY_BE& ln2);
	int RelolveNnet(int cl);
	int m_tempWhich;//��ǰ���㵽�ĸ���

	int m_MaxPoint;    // ������ʼ��
	int m_MinPoint;    // ������ֹ��

	CString  m_QuoteArray[QUE-QUB];	//���õ����Ƶ�����
	int      m_QuoteNum;	//���õĸ���//

	CString  m_stringArray[CECE-CECB];	//�ַ���������
	int      m_stringNum;	//�ַ����ĸ���

	CString m_strColor[CONSCOLORE-CONSCOLORB+1];
	int      m_strColorNum;	//color�ĸ���

	int	 m_Sent[MAXB];  //�ӹ�ʽ����˳��//
	int  m_SentNum;     //�ӹ�ʽ��ʵ�����(-1)//

	int	 m_ComSort[MAXB_ALL];    // ��ʽ����˳��//-------lmb?
	int  m_ComSortNum;		 // ��ʽ��ʵ�����(-1)//
	int  m_Post[MAXB];  // ��¼�������Ӵ���λ��//

	ARRAY_BE  m_VarArray[MIDE-MIDB]; // �м����
	int     m_VarNum;			  // �м��������
	CString m_VarName[MIDE-MIDB];             // �м��������

	int   m_floatNum;			  // ����ϵ������
	float m_floatArray[CEFE-CEFB];//����ϵ��

	CString  m_NumName[PARAM_NUM];    //��������			// huhe* �ӵ�16����������
	int      m_NumGs;		  //��������	

	int   m_intNum;		// ����ϵ������
	int32   m_intArray[CEIE-CEIB]; // ����ϵ��

	ARRAY_BE m_MidArray[PCME-PCMB]; // �м�����
	int    m_MidNum;    // �м����ݸ���

	Kline* m_pData;       // K������ָ��
	Intst<int> m_table;	//��ջ

	bool m_bToatlVol;	// ...fangz0701 �Ƿ����ۼƵĳɽ���

	CMapStringToPtr m_WordTableNew;	//�¼ӵĺ�����
	CMap<int,int,int,int> m_ParameterTableNew;//��������������
	static CMapStringToPtr m_LetterTable;	//��ĸ��
	static CMap<int,int,int,int> m_ParameterTable;//��������������
	static CMapStringToPtr m_WordTable;	//������

	bool IsDayKline(int nKlineType);

	void FuncZigPre(INPUT_INFO *pInput,int nFunc);

	static int FuncZigGroup(INPUT_INFO *pInput, void* pArray);

	int RegularTime(int time,bool bDay);

	int LookTwoPath(time_t& tmt ,Kline* pKline,int nMax,bool& bAdd,bool bDay);

	int GetFoot(INPUT_INFO *pInput, int nParam);

	//�������:pInput �������ԭʼ����,nFunc�Ǳ�ʾ�ڼ�������
	//�������:���ش�����,0��ʾ�ɹ�
	int FuncInDll(INPUT_INFO* pInput,int  nFunc);//����2.0�������ӵ�Ԫ����
	
	int FuncFromDll(int n);

	static void InitStatic();
	bool LookupParameterTable(int nKey,int& nValue);
	bool LookupWordTable(CString sKey,int& n);
	void KlineToLine(Kline* pKline,float& f,int nFlag);
	int Func847();
	Kline* m_pKlineIndex;
	
	
    int Func101();
	int Func102();
	int Func301();
	int Func302();
	int Func401();
	int Func402();
	int Func405();
	int Func406();
	int Func408();
	int Func409();
	int Func501();
	int Func502();
	int Func601();
	int Func602();
	int Func603();
	int Func604();
	int Func801();
	int Func802();
	int Func803();
	int Func804();
	int Func805();
	int Func806();
	int Func807();
	int Func808();
	int Func809();
	int Func810();
	int Func811();
	int Func812();
	int Func813();
	int Func830();
	int Func831();
	int Func832();
	int Func833();
	int Func834();
	int Func835();
	int Func836();
	int Func837();
	int Func838();
	int Func839();
	int Func840();
	int Func841();
	int Func842();
	int Func843();
	int Func844();
	int Func845();
	int Func846();
	int Func852();
	int Func853();
	int	Func856();
	int FuncHHAV();
	int FuncLLAV();
	
	int m_exec;
	int m_nKindKline;				//m_nKindKline 
	bool m_bCreateBsList;
	CFormularContent*	m_pIndex;

	//T_MerchNodeUserData *m_pMerchUserData;
	MERCH_EXTRA_INFO *m_pExtraInfo;
	int	 m_iMinuteFromOpenTime;	// ��ǰʱ���뿪�̺�ķ�����
	int  m_iKlinePeriod;		// K��������

	int  m_iSreenBegin;
	int  m_iSreenEnd;

	bool GetBuySellList();
	void InitEquation(int pmin,int pmax,Kline* pp,CString& str);//��ʼ����������

private:
	//float GetCapital(CReportData *pdt);
	CSpecialTechIndexManager m_SpecialTechIndexManager;
};

#endif // !defined(AFX_COM_H__80BFF943_AB2A_11D1_8C43_0000E823CF90__INCLUDED_)
