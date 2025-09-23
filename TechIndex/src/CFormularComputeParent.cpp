//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxtempl.h>
#include <float.h>
#include <MMSystem.h>
#include "GmtTime.h"
#include "XLTimerTrace.h"
#include "coding.h"
#include "CFormularComputeParent.h"
#include "CFormularContent.h"
#include "SelectStockStruct.h"
#include "EngineCenterBase.h"
//#include "FormularEnum.h"
//#include "IoViewShare.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CAbsCenterManager *g_pCenterManager;


CMapStringToPtr CFormularComputeParent::m_LetterTable;				//字母表
CMap<int,int,int,int> CFormularComputeParent::m_ParameterTable;		//函数参数个数表
CMapStringToPtr CFormularComputeParent::m_WordTable;				//函数表
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
 
#define MANY_DAY_K_LINE		15
#define HS_K_LINE			25
#define MINUTE_1_K_LINE		35
#define NUM_MANY_DAY		50
//lint --e{1401,1566}
CFormularComputeParent::CFormularComputeParent()
{
	//ASSERT(0);
	
	m_bToatlVol = false;
	m_pExtraInfo = NULL;
	m_iMinuteFromOpenTime = 1;
	m_iKlinePeriod = -1;
	m_iSreenBegin  = -1;
	m_iSreenEnd  = -1;
}
//lint --e{1401,1566}
CFormularComputeParent::CFormularComputeParent(int pmin,int pmax,Kline* pp,CString& str)
{ 
	
	m_bToatlVol = false;
	m_bShowColor = false;
	m_pExtraInfo = NULL;
	m_iMinuteFromOpenTime = 1;
	m_iKlinePeriod = -1;
	m_iSreenBegin  = -1;
	m_iSreenEnd  = -1;
//	m_stkKind = 0;
	InitEquation( pmin,  pmax, pp, str);
}
//lint --e{1401,1566}
CFormularComputeParent::CFormularComputeParent(int pmin,int pmax,Kline* pp,CFormularContent* pIndex)
{ 
	
	m_bToatlVol = false;
	m_bToatlVol = false;
	m_bShowColor = false;
	m_pExtraInfo = NULL;
	m_iMinuteFromOpenTime = 1;
	m_iKlinePeriod = -1;
	m_iSreenBegin  = -1;
	m_iSreenEnd  = -1;

	CString str = pIndex->formular ;
	str.MakeLower ();
	InitEquation( pmin,  pmax, pp, str);

	m_pIndex = pIndex;
}

CFormularComputeParent::~CFormularComputeParent()
{
	if(m_pKindPos) 
	{		
		DEL_ARRAY(m_pKindPos);
	}

	while(m_table.pop_it()){};

	if(m_pKlineIndex!=NULL)
		DEL_ARRAY(m_pKlineIndex);

	m_bTotalData=false;

	int i = 0;
	for(i=0;i<m_RlineNum;i++)
		DEL_ARRAY(m_Rline[i].line);
	for(i=0;i<m_VarNum;i++)
		DEL_ARRAY(m_VarArray[i].line);
	for(i=0;i<m_MidNum;i++)
		DEL_ARRAY(m_MidArray[i].line);
	m_exec     =0;
}
//lint --e{616,830,525}
int CFormularComputeParent::CheckWord(CString eqm)//词法检查
{
	//eqm = L"汉字:CLOSE";
	if(m_bShowColor == true)
	{
		if(m_pKindPos == NULL) m_pKindPos = new KindPos[MAXB_ALL*4];
		if(m_pKindPos == NULL) return 0;
//		m_nPosPre = 0;
//		m_nSizeKindPos = 0;
	}

	CString temp;
	int Bm =0;
	double x;

	// fangz0926 char 改成 TCHAR
	// char ch,chs;
	// char chl[20],*chj;

	const UINT16 iMax = 20;
	TCHAR ch,chs;
	TCHAR chl[iMax],*chj;

	int  cl,hj,j,cls,fl;
	int  ck;
	int  mj=eqm.GetLength();
	int rtn;//lmb
	int i = 0;
	for(i=0;i<mj;i++)
	{
		if(m_pKindPos != NULL)
		{
			m_pKindPos[m_nSizeKindPos].m_nPos = i+m_nPosPre;
		}

		ch=eqm.GetAt(i);
		cl=GetCharClass(ch);
		switch(cl)
		{
		case 1:
			m_errmsg="非法字符";
			return i;
		case 2:
			break;
		case 3:
			m_Post[Bm] = i;
			fl=0;
			if(ch=='.')
			{
				fl = 1;
//				m_errmsg="非法字符";
//				return i;
			}
			for(ck=0;ck<iMax;ck++)	chl[ck] = ' ';
			j=i;
			chl[0]=ch;
			while(++j<mj)
			{
				chs=eqm.GetAt(j);
				if (chs=='.') fl=1;
				cls=GetCharClass(chs);
				if(cls!=3)  break;

				if (iMax <= (j-i))
				{
					m_errmsg="整数值超过了整数的最大值";
					return i;
				}

				chl[j-i]=chs;				
			};
			i=--j;
			if(fl==1)
			{
				if(m_floatNum <= CEFE-CEFB &&  LESS_ARRARY_INDEX(m_floatArray, m_floatNum))
				{
					// fangz0926 char->TCHAR
					// x = strtod(chl, &chj);
					x = wcstod(chl, &chj);
					m_floatArray[m_floatNum]=(float)x;
					m_table.push_b(m_floatNum+CEFB);
					m_floatNum++;
					Bm++;
				}
				else
				{
					m_errmsg="浮点数个数超出";
					return i;
				}
			}
			else
			{
				if(m_intNum <= CEIE -CEIB && LESS_ARRARY_INDEX(m_intArray, m_intNum))
				{
					// fangz0926 char->TCHAR
					// x = strtod(chl, &chj);			
					x = wcstod(chl,&chj);
					m_intArray[m_intNum]=(int32)x;
					m_table.push_b(m_intNum+CEIB);
					m_intNum++;
					Bm++;
				}
				else
				{
					m_errmsg="整数个数超出";
					return i;
				}
			}
			if(m_pKindPos != NULL)
			{
				m_pKindPos[m_nSizeKindPos].m_nKind = 0;
			}

			break;
		case 4:
			m_Post[Bm] = i;
			temp.Empty();
			temp.Insert(0,ch);
			j=i;
			while(++j<mj)
			{
				chs=eqm.GetAt(j);
				cls=GetCharClass(chs);
				if(cls!=3 && cls!=4) 
					break;
				temp.Insert(j-i,chs);				
			};
			
			if(!LookupWordTable(temp,hj))
			{

				while(j<mj&&eqm.GetAt(j)==' ')
					j++;

				if(j<mj)
				{
					if(Bm==0  && eqm.GetAt(j) == ':') 
					{
						if(temp.GetLength()>36)
						{
							m_errmsg="中间变量名不能大于36位";
							return i;
						}
						if(j == mj-1)
						{
							m_errmsg="结束字符为非法字符！";
							return i;
						}
						else
						{
							if(eqm.GetAt(j+1) == '=')
							{
								if(m_VarNum <= MIDE-MIDB && LESS_ARRARY_INDEX(m_VarArray, m_VarNum) && LESS_ARRARY_INDEX(m_VarName, m_VarNum))
								{
									m_WordTableNew[temp]=(void*)(MIDB+m_VarNum);
									m_VarArray[m_VarNum].line = new float[m_MaxPoint];
									m_table.push_b(MIDB+m_VarNum);
									m_VarName[m_VarNum] = temp;					
									m_VarNum++;
									Bm++;
								}
								else
								{
									m_errmsg="中间变量个数超出";
									return i;
								}
							}
							else
							{
								if(m_RlineNum <= RLINEE-RLINEB && LESS_ARRARY_INDEX(m_Rline, m_RlineNum) &&  LESS_ARRARY_INDEX(m_RlineName, m_RlineNum))
								{
									m_WordTableNew[temp]=(void*)(RLINEB+m_RlineNum);
									m_Rline[m_RlineNum].line = new float[m_MaxPoint];
									m_table.push_b(RLINEB+m_RlineNum);
									m_RlineName[m_RlineNum] = temp;
									m_RlineNum++;
									Bm++;
								}
								else
								{
									m_errmsg="返回变量个数超出";							
									return i;

								}
							}
						}
					}
					else
					{
						if(temp.GetLength () == 11 && temp.Left(5) == "color")
						{
							m_table.push_b(m_strColorNum+CONSCOLORB);
							CString s2 = temp;
							s2.Delete (0,5);
							m_strColor[m_strColorNum] = s2;
							m_strColorNum++;
							Bm++;
							i=j;
							if(m_pKindPos != NULL)
							{
								m_pKindPos[m_nSizeKindPos].m_nKind = 4;
							}
						}
						else
						{
							m_errmsg="非法变量";
							return i;
						}
/**/				}
				}
				else
				{
						if(temp.GetLength () == 11 && temp.Left(5) == "color")
						{
							m_table.push_b(m_strColorNum+CONSCOLORB);
							CString s2 = temp;
							s2.Delete (0,5);
							m_strColor[m_strColorNum] = s2;
							m_strColorNum++;
							Bm++;
							i=j;
							if(m_pKindPos != NULL)
							{
								m_pKindPos[m_nSizeKindPos].m_nKind = 4;
							}
						}
						else
						{
							m_errmsg="非法变量";
							return i;
						}
				}
				if(m_pKindPos != NULL)
				{
					m_pKindPos[m_nSizeKindPos].m_nKind = 2;
				}
				
			}
			else
			{
				while(j<mj&&eqm.GetAt(j)==' ')
					j++;

				if(j<mj && Bm==0  && eqm.GetAt(j) == ':') 
				{
					if(hj>=MIDB)
					{
						m_errmsg="变量重复定义或与参数同名";
						return i;
					}
				}

				if(hj==8034 || hj == 8046 
					|| hj>=FuncDllB+21 && hj<=FuncDllB+24 
					|| hj>=FuncDllB && hj<=FuncDllB+2
					|| hj==FuncDllB+5 || hj==FuncDllB+6
					|| hj>=FuncDllB+10 && hj<=FuncDllB+14
					|| hj == FuncDllB+34
					|| hj == FuncDllB+4)
					m_bTotalData=true;

				

				m_table.push_b(hj);
				Bm++;
				if(m_pKindPos != NULL)
				{
					m_pKindPos[m_nSizeKindPos].m_nKind = 3;
				}
			}
			i=--j;
			break;
		case 5:
			m_Post[Bm] = i;
			switch(ch)
			{
			case '>':
			case '<':
			case '=':
			case '!':
			case ':':
				if(i==eqm.GetLength()-1)
				{
					m_errmsg = "非法结束 ";
					return i;

				}
				chs=eqm.GetAt(i+1);
				if(chs=='=') 
				{
				//	yhh add 995629 
					if(ch=='=')
					{
						m_Post[Bm] = i+1;
						m_errmsg = "非法运算符";
						return i;
					}

					if(ch==':')
					{
						int pre;
						pre=m_table.pop_it();
						if(pre>=MIDB&&pre<=MIDB+m_VarNum)
							m_table.push_b(pre);
						else
						{
							if(pre>=FuncB&&pre<=PAM)
								m_errmsg ="中间变量不能用系统保留字";
							else
								m_errmsg ="中间变量不合法";
							return i;
						}

					}
				//yhh add end 
					temp.Empty();
					i++;
					if(i==mj) 
					{	
						m_errmsg = "非法结束 ";
						return i;
					}
					temp.Insert(0,ch);
					temp.Insert(1,chs);
					LookupWordTable(temp,hj);
					m_table.push_b(hj);
					Bm++;
				}
				else if(chs=='<' ||chs=='>') 
				{
					if(ch!='>' && chs=='<' || ch!='<' &&chs=='>')
					{
						m_Post[Bm] = i+1;
						m_errmsg = "非法运算符";
						return i;
					}

					temp.Empty();
					i++;
					if(i==mj) 
					{	
						m_errmsg = "非法结束 ";
						return i;
					}
					temp.Insert(0,ch);
					temp.Insert(1,chs);
					LookupWordTable(temp,hj);
					m_table.push_b(hj);
					Bm++;
				}
				else
				{
					if(ch=='!')  
					{	
						m_errmsg = "非法结束 ";
						return i;
					}
					else
					{
						//	yhh add 995629 
						if(ch==':')
						{
							int pre;
							pre=m_table.pop_it();
							if(pre>=RLINEB&&pre<=RLINEB+m_RlineNum)
								m_table.push_b(pre);
							else
							{
								if(pre>=FuncB&&pre<=PAM)
									m_errmsg ="中间变量不能用系统保留字";
								else
									m_errmsg ="中间变量不合法";
								return i;
							}

						}
						// yhh add 995629 end
						temp.Empty();
						temp.Insert(0,ch);
						LookupWordTable(temp,hj);
						m_table.push_b(hj);
						Bm++;
					}
				}
				break;
			case '-':
			case '+':
				rtn=m_table.pop_it();
				if( rtn==0 || (rtn==9001) || (rtn==9005)
					|| (rtn>=1001&&rtn<=4008))//lmb6.13
				{
					if(rtn>0)
						m_table.push_b(rtn);
					m_table.push_b(TOZERO);
					Bm++;
				}
				else if( (rtn==6001 || rtn==6002) && ch == '-' )//lmb6.13
				{
					temp.Empty();
					if(rtn == 6001)//*
						temp = "*-";
					else
						temp = "/-";
					if(!LookupWordTable(temp,hj))
					{
						m_errmsg="非法字符";
						return i;
					}
					m_table.push_b(hj);
					break;
				}
				else
				{
					m_table.push_b(rtn);
				}
			default:
					temp.Empty();
					temp.Insert(0,ch);
					if(!LookupWordTable(temp,hj))
					{
						m_errmsg="非法字符";
						return i;
					}
					m_table.push_b(hj);
					Bm++;
					break;
			}
			if(m_pKindPos != NULL)
			{
				m_pKindPos[m_nSizeKindPos].m_nKind = 1;
			}

			break;	
		case 6:
			//处理引用
			m_Post[Bm] = i+1;// m_Post[Bm]记录变量在子串中位置
			m_QuoteArray[m_QuoteNum].Empty();
			j=i;
			fl=0;
			while(++j<mj)//mj字符串总长
			{
				chs=eqm.GetAt(j);
				cls=GetCharClass(chs);
				if(cls>1 && cls<6)
				{
					m_QuoteArray[m_QuoteNum].Insert(j-i,chs);
					continue;
				}
				if(cls==6)
				{
					fl=1;
					break;
				}
				if(cls==1)
				{
					m_errmsg="非法字符";
					return i;
				}
			};
			if(fl==0)
			{
				m_errmsg="引用无匹配符";
				return i;
			}

			m_table.push_b(m_QuoteNum+QUB);
			m_QuoteNum++;
			Bm++;
			i=j;
			if(m_pKindPos != NULL)
			{
				m_pKindPos[m_nSizeKindPos].m_nKind = 4;
			}
			break;
		case 7:
			//处理字符串
			m_Post[Bm] = i+1;// m_Post[Bm]记录变量在子串中位置
			m_stringArray[m_stringNum].Empty();
			j=i;
			fl=0;
			while(++j<mj)//mj字符串总长
			{
				chs=eqm.GetAt(j);
				cls=GetCharClass(chs);
				if(cls>1 && cls<6)
				{
					m_stringArray[m_stringNum].Insert(j-i,chs);
					continue;
				}
				if(cls==7)
				{
					fl=1;
					break;
				}
				if(cls==1)
				{
					m_errmsg="非法字符";
					return i;
				}
			};
			if(fl==0)
			{
				m_errmsg="无匹配符";
				return i;
			}

			m_table.push_b(m_stringNum+CECB);
			m_stringNum++;
			Bm++;
			i=j;
			if(m_pKindPos != NULL)
			{
				m_pKindPos[m_nSizeKindPos].m_nKind = 4;
			}
			break;
		}

		m_nSizeKindPos++;
	}
//============出栈==清空链表===================
	m_SentNum=-1;
	do 
	{	m_SentNum++;
		m_Sent[m_SentNum]=m_table.rec_it();
		i=m_Sent[m_SentNum];
	}while(i);
	m_SentNum--;
//---		lmb
	IsDaPan();
//yhh add
	FindLineType()  ;//计算返回线的类型
	return -1;
}

// fangz0926 char->TCHAR
// int CFormularComputeParent::GetCharClass(char ch)//
int CFormularComputeParent::GetCharClass(TCHAR ch)//
{   
	int hj;
	CString tj(ch);

    if(m_LetterTable.Lookup(tj,(void *&)hj)) 
		return hj;
	else 
	{
//		if((ch&0x80) == 0)
//			return 1;
//		else
			return 4;
	}
}
int CFormularComputeParent::YhhCheck(int j, int k)
{
	if (j < 0 )
	{
		return -1;
	}
	//lint --e{539}
	int tj=0,tu=0;
	int k0=k;
	int i;
  for(i=j;i<=m_SentNum;i++)
	{
		if(tu<0) 
		{
			m_errmsg="缺少参数";    
			return i;
		}
		if(tu>1) 
		{
			m_errmsg="缺少运算符";    
			return i;
		}

		if (LESS_ARRARY_INDEX(m_Sent, i))
		{
			tj=m_Sent[i];
		}
		if(tj==9001)
		{
			k++;   continue;
		}
		if(tj==9002) 
		{
			k--;
//			if(k==k0)            //?
//			tu+=1;//tu 
			
     		if((k==0||k==k0) && tu!=1)        //?    //wjy 的代码 
			{
				m_errmsg="非法运算";              //wjy 的代码
				return i;
			}
				continue;
		}
		if(tj==9005)            //yhh 是","
		{
			if(tu!=1) 
			{
				m_errmsg="非法字符";
				return i;
			}
 			return -1;            //-1代表，退出
		}
		if( (tj>FuncB && tj<=FuncE))     //? k>k0 ||
		{
			if(tj>=QUB && tj<=FuncE && i<=m_SentNum && LESS_ARRARY_INDEX(m_Sent, i+1) )
			{
				if(m_Sent[i+1]!=9001)
					tu+=1;
			}
			continue;
		}

		
   	   if(tj<FuncB)
			tu-=1;
		if(tj>CONSE||tj==TOZERO)
			tu+=1;
		
	}
	if(tu!=1 && j!=m_SentNum)
	{
		if(tu>1)
			m_errmsg="缺少运算符";
		else
			m_errmsg="缺少参数";

		return i-1;
	}
	else 
		return -1;

}

int CFormularComputeParent::CheckSent()
{
	int i=0,k=0;
	int tj=0,tu=0;              //yhh  tu :参数的个数?
//	int fl=0,j=0;
	CString str,cname,cline;

	int start;
	for(start=0;start<=m_SentNum;start++)
	{
		tj=m_Sent[start];
		if(tj==9001)
			k++;
		else if(tj==9002)
		{
			k--;
			if(k<0)
			{
				m_errmsg="括号不匹配,多)";
				return start;
			}
		}
	}
	if(start>=m_SentNum&&k>0)
	{
		m_errmsg="括号不匹配,多(";
		return start;	
	}
	int check=YhhCheck(0,k);
	if(check!=-1)
		return check;
	for(i=0;i<=m_SentNum;i++)
	{	tj=m_Sent[i];
		int m;

		if( tj==9005) //(	,
		{
			int iChk=YhhCheck(i+1,k);
			if(iChk==-1) continue;
			else 
				return iChk;
		}

	                        	
		if(tj==9001 ) 
		{
		   k++;
		  int iChk=YhhCheck(i+1,k);
		  if(iChk==-1) continue;
		  else 
				return iChk;

		}
		if(tj==9002)
		{
		k--;
		}
					
		if(tj>FuncB && tj<=FuncE)//800	890
		{
		
				//---	找参数个数
			{
					if(!LookupParameterTable(tj,tu)) 
					tu=1;
			}

					//---		
			 int   kk=0;//---k	括号匹配标记   	//wjy
			 if(m_Sent[i+1]!=9001)  //yhh 函数后面不是(
			 {
				 if(tj<QUB)
				 {
					m_errmsg="缺括号";
					return i;
				 }
				 //lmb 2000.3.8
				 else
				 {
					 m_ParameterTableNew[tj] = 0;
					 tu =0;
					 continue;
				 }
			 }
			 else 
					kk++;
			 m=i+2;   //YHH  跳过函数名和函数名旁边的(
			 do{    // yhh 这个循环检查函数内的参数是否正确
				tj=m_Sent[m];
				if(tj==9005 && kk==1) //yhh 括号匹配且出现","
					tu-=1;
				if(tj==9001) {kk+=1;}  //yhh 出现(
				if(tj==9002)          //yhh 出现)
				{
					kk-=1;
					if(kk==0 ) //yhh k<0 代表已找到最后的一个)     
					{
						if(tu==1)	break;  //yhh  正常返回
						else
						{
							m_errmsg="参数个数有误";
							return i;
						}
					}
				}
				m++;
			 
			 }while(m<=m_SentNum);//do语句结束
		
		}           //if语句结束
		                           
		
	}
	return -1;
							
		

}

int CFormularComputeParent::Sort(int bi,int ej)//运算顺序确定
{
	if (bi < 0 || ej < 0)
	{
		return 0;
	}
	
	//lmb6.13
	if(bi>ej)
		return 0;

	int tj,tu;
	int lb,le;//lb
	int i=0,j=0,k;
	i=0;//


	//处理括号,
	while(m_Sent[bi]==9001 && m_Sent[ej]==9002)//9001(,	9002)
	{
		i=bi+1;
		k=0;
		do{
			if(m_Sent[i]==9001)  k++;
			if(m_Sent[i]==9002)  k--;
			if(k<0)  break;
		}while(++i<ej);

		if(k==0)
		{
			bi++;
			ej--;  
			if(bi>=ej)  
			{
				if(bi==ej) m_table.push_b(m_Sent[bi]);
				else ASSERT(FALSE);
				return 0;
			}
			else
				continue;
		}
		else
			break;
	}


	j=0; 
	tj = 10;//最高优先级
	//---		判断最低优先级 
/*	for(k=bi;k<=ej;k++)
	{
		if(m_Sent[k]==9001) {j++;continue;}
		if(m_Sent[k]==9002) j--;
		if(j==0)
		{
			tu=m_Sent[k]/100;
			if(tu<tj) {tj=tu; i=k;}//i
		}
	}*/

	//---找到完整的优先级最低的位置
	for(k=bi;k<=ej;k++)
	{
		if(m_Sent[k]==9001) {j++;continue;}
		if(m_Sent[k]==9002) j--;
		if(j==0)
		{
			tu=m_Sent[k]/1000;
			if(tu<=tj) {tj=tu; i=k;}//i
		}
	}
	//	lmb6.13
	if(bi==ej)
	{
		tj=m_Sent[bi];
		m_table.push_b(tj);
		return 0;
	}

    
	tj=m_Sent[i];
	//---		当为函数时
	if(tj>FuncB && tj<FuncE) 
	{
		m_table.push_b(tj);
		if(tj>=QUB)
		{
//			int nNext = 
			tu=GetNum(tj);//lmb	tu参数个数
		}
		else
		{
			if(!LookupParameterTable(tj,tu)) tu=1;//没有找到则参数个数为1
		}
		lb=i+2;//

		if(tu>0)//tu==0时，不需压入参数
		{
			while(tu--)
			{
				k=0;
				le=lb;
				while(1)
				{
					if(m_Sent[le]==9001) k++;
					if(m_Sent[le]==9002) k--;
					if(k<0 || (k==0 && m_Sent[le] ==9005)) 
					{
						le--;
						break;
					}
					le++;
				}
				if(lb==le)
					m_table.push_b(m_Sent[lb]);
				else
					Sort(lb,le);
				lb=le+2;
			}
		}
	}

	//---		双目操作符时
	if(tj<FuncB)
	{
		m_table.push_b(tj);
		lb=bi;//左边开始点
		le=i-1;//左边结束点
		if(lb==le && LESS_ARRARY_INDEX(m_Sent, lb))
			m_table.push_b(m_Sent[lb]);
		else
			Sort(lb,le);
		lb=i+1;//右边开始点
		le=ej;//右边结束点
		if(lb==le)
			m_table.push_b(m_Sent[lb]);
		else
			Sort(lb,le);
	}
	return 0;
}

//===========分割语句==============//
void CFormularComputeParent::Devide()
{
	int i,tu;
	int lb=0;//lb记录上一子句的结束
	int Rvdev,el;
	// fangz0926 char->TCHAR
	// char ch;
	TCHAR ch;
	CString ts;//?
	//lmb 20010907
	m_nPosPre = 0;


	if (m_MaxPoint==0) 
		return;
	tu=m_formuCompute.GetLength();
	m_ComSortNum = -1;
	m_errpos = -1;
	m_tempWhich=-1;
	for(i=0;i<tu;i++)
	{
		ch=m_formuCompute.GetAt(i);
		ts.Insert(i-lb,ch);
		if(ch==';' || i==tu-1)
		{
			if(i-lb==0)
			{
				m_errmsg="非法字符";
				m_errpos= i;
				break;
			}
			if(ch==';')	 ts.Delete(i-lb,1);
			Rvdev = CheckWord(ts);
			if(Rvdev != -1)
			{
				m_errpos= Rvdev+lb;
				break;
			}
			Rvdev = CheckSent();

			//---		lmb
//			IsDaPan();
//			FindLineType();
			//---
			if(Rvdev != -1)
			{
				m_errpos=m_Post[Rvdev]+lb;
				break;
			}		

//===========================================//
			Rvdev = Sort(0,m_SentNum);
			do {
				m_ComSortNum++;
				m_ComSort[m_ComSortNum]=m_table.pop_it();
				el=m_ComSort[m_ComSortNum];
			}while(el);
			m_ComSortNum--;
			ts.Empty();
			lb=i+1;

			//lmb 20010907
			m_nPosPre = i+1;
		}
	}
}
int CFormularComputeParent::AddPara(CString pa,float va)//增加浮点参数
{
	if(m_NumGs >= PARAM_NUM)			// huhe* 加到16个参数设置
		return 1;
//yhh add 990702
	int hj;
	if(LookupWordTable(pa,hj))
		return 2;
		
//yhh add 990702 end
	m_WordTableNew[pa] =(void *)(m_floatNum+CEFB);
	m_floatArray[m_floatNum]=va;
	m_NumName[m_NumGs] = pa;
	m_NumGs++;
	m_floatNum++;
	return 0;
}

int CFormularComputeParent::AddPara(CString pa, int va)//增加整型参数
{
	if(m_NumGs >= PARAM_NUM)			// huhe*　加到16个参数设置
		return 1;
//yhh add 990702
	int hj;
	if(LookupWordTable(pa,hj))
		return 2;
		
//yhh add 990702 end
	m_WordTableNew[pa] =(void *)(m_intNum+CEIB);
	m_intArray[m_intNum]=va;
	m_NumName[m_NumGs] = pa;
	m_NumGs++;
	m_intNum++;
	return 0;
}


void CFormularComputeParent::GetData(int cl,ARRAY_BE& pp)
{
	if (NULL == m_pData)
	{
		return;
	}

	pp.b = m_MinPoint; 
	pp.e = m_MaxPoint-1;
	int i;
	float ltp = 0;

	switch(cl)
	{
	case TOZERO:
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			 pp.line[i] =0;
		break;
	case 9521:	//open
		for(i=m_MinPoint;i<m_MaxPoint;i++)
		   pp.line[i] = m_pData[i].open;
		break;
	case 9522:	//close
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] = m_pData[i].close;
		break;
	case 9523:	//high
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] = m_pData[i].high;
		break;
	case 9524:	//low
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] = m_pData[i].low;
		break;
	case 9525:	//advence
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] = (float)m_pData[i].advance;
		break;
	case 9526:	//dayofweek
		CGmtTime *ptime;
		for(i=m_MinPoint;i<m_MaxPoint;i++)
		{	time_t tm_ = m_pData[i].day;
			ptime = new CGmtTime(tm_);
			pp.line[i] = (float)ptime->GetDayOfWeek()-1;
			DEL(ptime);
		}
		break;
	case 9527:	//day
		for(i=m_MinPoint;i<m_MaxPoint;i++)
		{
			time_t tm_ = m_pData[i].day;
			ptime = new CGmtTime(tm_);
			pp.line[i] = (float)(ptime->GetDay());
			DEL(ptime);
		}
		break;
	case 9528:	//month
		for(i=m_MinPoint;i<m_MaxPoint;i++)
		{
			time_t tm_ = m_pData[i].day;
			ptime = new CGmtTime(tm_);
			pp.line[i] = (float)(ptime->GetMonth());
			DEL(ptime);
		}
		break;
	case 9529:	//year
		for(i=m_MinPoint;i<m_MaxPoint;i++)
		{
			time_t tm_ = m_pData[i].day;
			ptime = new CGmtTime(tm_);
			pp.line[i] = (float)(ptime->GetYear());
			DEL(ptime);
		}
		break;
	case 9530: 	//hour
		for(i=m_MinPoint;i<m_MaxPoint;i++)
		{
			time_t tm_ = m_pData[i].day;
			ptime = new CGmtTime(tm_);
			pp.line[i] = (float)(ptime->GetHour());
			DEL(ptime);
		}
		break;
	case 9531: 	//minute
		for(i=m_MinPoint;i<m_MaxPoint;i++)
		{
			time_t tm_ = m_pData[i].day;
			ptime = new CGmtTime(tm_);
			pp.line[i] = (float)(ptime->GetMinute());
			DEL(ptime);
		}
		break;
	case 9560: 	//time
		for(i=m_MinPoint;i<m_MaxPoint;i++)
		{
			time_t tm_ = m_pData[i].day;
			ptime = new CGmtTime(tm_);
			pp.line[i] = (float)(ptime->GetMinute()*100+ptime->GetHour()*10000+ptime->GetSecond ());
			DEL(ptime);
		}
		break;
	case 9561: 	//date
		for(i=m_MinPoint;i<m_MaxPoint;i++)
		{
			time_t tm_ = m_pData[i].day;
			ptime = new CGmtTime(tm_);
			int nYear = ptime->GetYear();
			if(nYear>1999)
				nYear = 100+nYear%100;
			else
				nYear = nYear%100;
			pp.line[i] = (float)(ptime->GetMonth()*100+nYear*10000+ptime->GetDay ());
			DEL(ptime);
		}
		break;
	case 9532: 	//amount
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] = m_pData[i].amount;
		break;
	
	case 9533: 	//vol		
		{
			if ( m_bToatlVol)
			{
				// 是累积成交量的话,还递减算出每笔成交量
				for ( i = m_MinPoint; i< m_MaxPoint; i++)
				{
					float fVol = m_pData[i].vol;

					if ( i - 1 > 0)
					{
						fVol = m_pData[i].vol - m_pData[i-1].vol;
					}					
					pp.line[i] = fVol;
				}
			}
			else
			{
				for(i=m_MinPoint;i<m_MaxPoint;i++)
					pp.line[i] = m_pData[i].vol;
			}
		}
		break;
	case 9534: 	//isup
		for(i=m_MinPoint;i<m_MaxPoint;i++)
		{	
			if(m_pData[i].close > m_pData[i].open)
				pp.line[i] = 1;
			else
				pp.line[i] =0;
		}
		break;
	
	case 9535: 	//isdown
		for(i=m_MinPoint;i<m_MaxPoint;i++)
		{	
			if(m_pData[i].close < m_pData[i].open)
				pp.line[i] = 1;
			else
				pp.line[i] =0;
		}
		break;
	
	case 9536: 	//isequal
		for(i=m_MinPoint;i<m_MaxPoint;i++)
		{	
			if(m_pData[i].close < m_pData[i].open)
				pp.line[i] = 1;
			else
				pp.line[i] =0;
		}
		break;
	case 9537: 	//decline
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] =(float) m_pData[i].decline;
		break;
	case 9538: 	//capital
		if (m_pExtraInfo)
		{
			ltp = m_SpecialTechIndexManager.GetCapital(m_pExtraInfo->pMerchNode);
		}
		
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] =ltp;
		break;
	case 9539: 	//ltp
		
// 		for(i=m_MinPoint;i<m_MaxPoint;i++)
// 			pp.line[i] =CTdxShanKlineShowView_GetCapital(this->m_cdat1 );
		break;
	case 9540:
		{
			for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] = m_pData[i].position;
		}
		break;
	case 9541:
		/*if(m_pData!=NULL)
			for(i=m_MinPoint;i<m_MaxPoint;i++)
				pp.line[i] = m_pData[i].open;*/

		if (m_pExtraInfo)
		{
			CArray<CKLine, CKLine> pKLine;
			m_SpecialTechIndexManager.GetExpData(m_pExtraInfo->pMainExpMerchNode, m_pExtraInfo->iTimeUserMultipleMinutes,m_pExtraInfo->iTimeUserMultipleDays,m_pExtraInfo->eTimeIntervalFull, &pKLine);
			int iDisLine = pKLine.GetSize();
			for(i = m_MaxPoint - 1; i >= m_MinPoint; i--)
			{
				if (iDisLine <= 0)
				{
					break;
				}
				pp.line[i] =(float) pKLine.GetAt(--iDisLine).m_fPriceOpen;
			}	
		}
		/*else GetHsKlineData(cl,pp);
		if(m_pKlineIndex==NULL)break;
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] =(float) m_pKlineIndex[i].open;*/
		break;
	case 9542:
		if (m_pExtraInfo)
		{
			CArray<CKLine, CKLine> pKLine;
			m_SpecialTechIndexManager.GetExpData(m_pExtraInfo->pMainExpMerchNode, m_pExtraInfo->iTimeUserMultipleMinutes,m_pExtraInfo->iTimeUserMultipleDays,m_pExtraInfo->eTimeIntervalFull, &pKLine);
			int iDisLine = pKLine.GetSize();
			for(i = m_MaxPoint - 1; i >= m_MinPoint; i--)
			{
				if (iDisLine <= 0)
				{
					break;
				}
				pp.line[i] =(float) pKLine.GetAt(--iDisLine).m_fPriceClose;
			}	
		}
		/*if(m_pData!=NULL)
			for(i=m_MinPoint;i<m_MaxPoint;i++)
				pp.line[i] = m_pData[i].close;*/
		/*if(m_pKlineIndex==NULL)break;
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] =(float) m_pKlineIndex[i].close;*/
		break;
	case 9543:
		if (m_pExtraInfo)
		{
			CArray<CKLine, CKLine> pKLine;
			m_SpecialTechIndexManager.GetExpData(m_pExtraInfo->pMainExpMerchNode, m_pExtraInfo->iTimeUserMultipleMinutes,m_pExtraInfo->iTimeUserMultipleDays,m_pExtraInfo->eTimeIntervalFull, &pKLine);
			int iDisLine = pKLine.GetSize();
			for(i = m_MaxPoint - 1; i >= m_MinPoint; i--)
			{
				if (iDisLine <= 0)
				{
					break;
				}
				pp.line[i] =(float) pKLine.GetAt(--iDisLine).m_fPriceHigh;
			}	
		}
		/*if(m_pData!=NULL)
			for(i=m_MinPoint;i<m_MaxPoint;i++)
				pp.line[i] = m_pData[i].high;*/
		/*if(m_pKlineIndex==NULL)break;
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] =(float) m_pKlineIndex[i].high;*/
		break;
	case 9544:
		if (m_pExtraInfo)
		{
			CArray<CKLine, CKLine> pKLine;
			m_SpecialTechIndexManager.GetExpData(m_pExtraInfo->pMainExpMerchNode, m_pExtraInfo->iTimeUserMultipleMinutes,m_pExtraInfo->iTimeUserMultipleDays,m_pExtraInfo->eTimeIntervalFull, &pKLine);
			int iDisLine = pKLine.GetSize();
			for(i = m_MaxPoint - 1; i >= m_MinPoint; i--)
			{
				if (iDisLine <= 0)
				{
					break;
				}
				pp.line[i] =(float) pKLine.GetAt(--iDisLine).m_fPriceLow;
			}	
		}
	/*	for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] = m_pData[i].low;*/
		/*if(m_pKlineIndex==NULL)break;
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] =(float) m_pKlineIndex[i].low;*/
		break;
	case 9545:
		if (m_pExtraInfo)
		{
			CArray<CKLine, CKLine> pKLine;
			m_SpecialTechIndexManager.GetExpData(m_pExtraInfo->pMainExpMerchNode, m_pExtraInfo->iTimeUserMultipleMinutes,m_pExtraInfo->iTimeUserMultipleDays,m_pExtraInfo->eTimeIntervalFull, &pKLine);
			int iDisLine = pKLine.GetSize();
			for(i = m_MaxPoint - 1; i >= m_MinPoint; i--)
			{
				if (iDisLine <= 0)
				{
					break;
				}
				pp.line[i] =(float) pKLine.GetAt(--iDisLine).m_fVolume;
			}	
		}
		/*for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] = m_pData[i].vol;*/
		/*if(m_pKlineIndex==NULL)break;
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] =(float) m_pKlineIndex[i].vol;*/
		break;
	case 9546:
		if (m_pExtraInfo)
		{
			CArray<CKLine, CKLine>  pKLine;
			m_SpecialTechIndexManager.GetExpData(m_pExtraInfo->pMainExpMerchNode, m_pExtraInfo->iTimeUserMultipleMinutes,m_pExtraInfo->iTimeUserMultipleDays,m_pExtraInfo->eTimeIntervalFull, &pKLine);
			int iDisLine = pKLine.GetSize();
			for(i = m_MaxPoint - 1; i >= m_MinPoint; i--)
			{
				if (iDisLine <= 0)
				{
					break;
				}
				pp.line[i] =(float) pKLine.GetAt(--iDisLine).m_fAmount;
			}	
		}
		/*for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] = m_pData[i].amount;*/
		/*if(m_pKlineIndex==NULL)break;
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] =(float) m_pKlineIndex[i].amount;*/
		break;
		
	case 9547:
		if (m_pExtraInfo)
		{
			CArray<CKLine, CKLine>  pKLine;
			m_SpecialTechIndexManager.GetExpData(m_pExtraInfo->pMainExpMerchNode, m_pExtraInfo->iTimeUserMultipleMinutes,m_pExtraInfo->iTimeUserMultipleDays,m_pExtraInfo->eTimeIntervalFull, &pKLine);
			int iDisLine = pKLine.GetSize();
			for(i = m_MaxPoint - 1; i >= m_MinPoint; i--)
			{
				if (iDisLine <= 0)
				{
					break;
				}
				pp.line[i] =(float) pKLine.GetAt(--iDisLine).m_usFallMerchCount;	
			}	
		}
		/*for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] = m_pData[i].decline;*/
		/*if(m_pKlineIndex==NULL)break;
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] =(float) m_pKlineIndex[i].decline;*/
		break;
	case 9548:
		if (m_pExtraInfo)
		{
			CArray<CKLine, CKLine> pKLine;
			m_SpecialTechIndexManager.GetExpData(m_pExtraInfo->pMainExpMerchNode, m_pExtraInfo->iTimeUserMultipleMinutes,m_pExtraInfo->iTimeUserMultipleDays,m_pExtraInfo->eTimeIntervalFull, &pKLine);
			int iDisLine = pKLine.GetSize();
			for(i = m_MaxPoint - 1; i >= m_MinPoint; i--)
			{
				if (iDisLine <= 0)
				{
					break;
				}

				pp.line[i] =(float) pKLine.GetAt(--iDisLine).m_usRiseMerchCount;
	
			}	
		}
		/*for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] = m_pData[i].advance;*/
		/*if(m_pKlineIndex==NULL)break;
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] =(float) m_pKlineIndex[i].advance;*/
		break;
	case 9549:		
		if (NULL==m_pExtraInfo)break;
		for(i=m_MinPoint;i<m_MaxPoint;i++)
		{
			if (0 == m_pExtraInfo->iShowCountInFullList/*m_pMerchUserData->m_iShowCountInFullList*/)
			{
				pp.line[i] = 200;
			}
			else
			{
				pp.line[i] = m_pExtraInfo->iShowCountInFullList/*m_pMerchUserData->m_iShowCountInFullList*/;
			}			
		}
		break;
		//hs data
	case 9550:
		
		break;
	case 9551:
		
		break;
	case 9552:
		
		break;
	case 9553:
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] =(float) ((KlineGeneral*)(m_pData+i))->volPositive ;
		break;
	case 9554:
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] =m_pData[i].vol - (float) ((KlineGeneral*)(m_pData+i))->volPositive ;
		break;//
	case 9580:
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] =100;
		break;//
	case 9581:	// drawnull
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] =FLT_EPSILON;
		break;//
	case 9582:	// currbarscount // 到最后交易日的周期数，最后交易日当天为1
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] = (float)(m_MaxPoint-i);
		break;
	case 9583: // period 当前k线类型，分钟？日？
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] = (float)m_iKlinePeriod;	// 将面临浮点数比较问题
		break;
	case 9584: // fromopen 当前时间距离该商品的今日开盘时间, 单位分钟
		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] = (float)(m_iMinuteFromOpenTime);
		break;
	case 9600: // totalcapital 当前总股本
		if (m_pExtraInfo)
		{
			ltp = m_SpecialTechIndexManager.GetTotalCapital(m_pExtraInfo->pMerchNode);
		}

		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] =ltp;
		break;
	case 9601: // hyzscode  所属行业的板块指数代码
		if (m_pExtraInfo)
		{
			ltp = m_SpecialTechIndexManager.GetHyzsCode(m_pExtraInfo->pMerchNode);
		}

		for(i=m_MinPoint;i<m_MaxPoint;i++)
			pp.line[i] =ltp;
		break;
	default:
		if( cl>=MIDB && cl<=MIDE &&  LESS_ARRARY_INDEX(m_VarArray, cl-MIDB))
		{
/*			int bl=m_VarArray[cl-MIDB].b;
			int be=m_VarArray[cl-MIDB].e;
			pp.b = bl;
			pp.e = be;
			if(bl>=0&& be>=0 && be>=bl)
			{
			for(i=bl;i<=be;i++) 
				pp.line[i] =m_VarArray[cl-MIDB].line[i];
			}*/
			pp = m_VarArray[cl-MIDB];
			break;
		}
		if(cl>=RLINEB && cl<=RLINEE && LESS_ARRARY_INDEX (m_Rline, cl-RLINEB))
		{
			pp =m_Rline[cl-RLINEB];
/*
			int bl=m_Rline[cl-RLINEB].b;
			int be=m_Rline[cl-RLINEB].e;
			pp.b = bl;
			pp.e = be;
			if(bl>=0&& be>=0 && be>=bl)
			{
			for(i=bl;i<=be;i++) 
				pp.line[i] =m_Rline[cl-RLINEB].line[i];
			}*/
			break;
		}
		if(cl>=CEFB && cl<=CEFE && LESS_ARRARY_INDEX(m_floatArray, cl-CEFB))
		{
			for(i=m_MinPoint;i<m_MaxPoint;i++) 
				pp.line[i] =m_floatArray[cl-CEFB];
			break;
		}
		if(cl>=CEIB && cl<=CEIE && LESS_ARRARY_INDEX(m_intArray,cl-CEIB))
		{
			for(i=m_MinPoint;i<m_MaxPoint;i++) 
				pp.line[i] =(float)m_intArray[cl-CEIB];
			break;
		}
		if(cl>=PCMB && cl<=PCME && LESS_ARRARY_INDEX(m_MidArray, cl-PCMB))
		{
			pp =m_MidArray[cl-PCMB];              
/*			int bl=m_MidArray[cl-PCMB].b;
			int be=m_MidArray[cl-PCMB].e;
			pp.b=bl;
			pp.e=be;
			if(bl>=0&& be>=0 && be>=bl)
			{
				for(i=bl;i<=be;i++) 
					pp.line[i] =m_MidArray[cl-PCMB].line[i];
			}*/
			
			// delete[] m_MidArray[cl-PCMB].line;
			m_MidArray[cl-PCMB].Reset();
			m_MidNum--;
			break;
		}
		//---		引用
		if(cl>=QUB && cl <=QUE && LESS_ARRARY_INDEX(m_MidArray, cl-PCMB))
		{
			if(Relolve(cl)==0)//把结果写到中间数组
			{
				//---		从中间数组读到pp中(lmb)
				pp =m_MidArray[m_MidNum-1];
/*				int bl=m_MidArray[m_MidNum-1].b;
				int be=m_MidArray[m_MidNum-1].e;
				pp.b=bl;
				pp.e=be;
				if(bl>=0&& be>=0 && be>=bl)
				{
				for(i=bl;i<=be;i++) 
					pp.line[i] =m_MidArray[m_MidNum-1].line[i];
				}*/
				// delete[] m_MidArray[m_MidNum-1].line;
				m_MidArray[cl-PCMB].Reset();
				m_MidNum--;
			}
			break;
		}
		if(cl>=CECB && cl <CECE && LESS_ARRARY_INDEX(m_stringArray, cl-CECB))
		{
			{
				pp.s = this->m_stringArray [cl-CECB];
				pp.kind = ARRAY_BE::StringData;
			}
			break;
		}
		break;
	}
}
class CAutoFile
{
public:
	CAutoFile(char* strName )
	{
		fp = fopen(strName,"w+");
	}
	FILE* GetFP()
	{
		return fp;
	}
	~CAutoFile()
	{
		if ( NULL != fp )
		{
			fclose(fp);
		}
	}
private:
	FILE* fp;
};
//  正常返回 0 ；错误返回代码,错误信息保存在m_errmsg中// 
int CFormularComputeParent::FormularCompute()
{	

#ifndef _DEBUG
	try
	{
#endif

	if(	m_exec  == 0)
			m_exec     =1;
	else
	{
		m_errmsg = "公式重新调用必须初始化--SETNEW";
		return -100;
	}

	Devide();//分解

	if(m_errpos!=-1) 
		return -100;
	
	int cs,msg=0;
	
	//CAutoFile ftable("c:/table.txt");
	//CAutoFile fsort("c:/sort.txt");

	//---		开始计算
	//DWORD dwTime = timeGetTime();
	int i;
	for( i = 0; i <= m_ComSortNum; i++)
	{
		if(msg!=0)
		{
			m_errmsg = "运行错误";
			if(msg!=999) 
				return msg;
		}
		cs = m_ComSort[i];

		// cs是计算公式的id
// 		POSITION pos = m_WordTable.GetStartPosition();
// 		CString StrFuncName;
// 		while ( pos != NULL )
// 		{
// 			int ii;
// 			m_WordTable.GetNextAssoc(pos, StrFuncName, (void *&)ii);
// 			if ( ii == cs )
// 			{
// 				break;
// 			}
// 		}
		//XLTRACE_PERFORMANCE_AUTO(_T("    "), CFormularComputeParent, FormularCompute, StrFuncName);
		

		//fprintf(fsort.GetFP(),"%d %d\n",i,cs);
		if(cs>=TOZERO)      //是系统参数
		{
			m_table.push_b(cs);
			continue;
		}
		if(cs>=QUB && cs<=QUE) //是引用
		{
			msg = Relolve(cs);
			continue;
		}
		if(cs>=FuncDllB && cs<=FuncDllE) //是dll
		{
			msg = FuncFromDll(cs);
			continue;
		}
		if(cs>=CECB && cs<CECE) //是char
		{
			m_table.push_b(cs);
			continue;
		}
		switch(cs)
		{
		case 1001:    //==  :   ==//
			msg=Func101();
			break;
		case 1002:    //==  :=  ==//
			msg=Func102();
			break;
		case 3001:
			msg=Func301();// and
			break;
		case 2001:
			msg=Func302();// or
			break;
		case 4001:
			msg=Func401();// >
			break;
		case 4002:
			msg=Func402();// <
			break;
		case 4005:
			msg=Func405();// >=
			break;
		case 4006:
			msg=Func406();// <=
			break;
		case 4008:
			msg=Func408();// ==
			break;
		case 4009:
			msg=Func409();// !=
			break;
		case 5001:
			msg=Func501();// +
			break;
		case 5002:
			msg=Func502();// -
			break;
		case 6001:
			msg=Func601();// *
			break;
		case 6002:
			msg=Func602();// /
			break;
		case 6003:
			msg=Func603();// *
			break;
		case 6004:
			msg=Func604();// /
			break;
		case 8001:
			msg=Func801();// /
			break;
		case 8002:
			msg=Func802();// /
			break;
		case 8003:
			msg=Func803();// /
			break;
		case 8004:
			msg=Func804();// /
			break;
		case 8005:
			msg=Func805();// /
			break;
		case 8006:
			msg=Func806();// /
			break;
		case 8007:
			msg=Func807();// /
			break;
		case 8008:
			msg=Func808();// /
			break;
		case 8009:
			msg=Func809();// /
			break;
		case 8010:
			msg=Func810();// /
			break;
		case 8011:
			msg=Func811();// /
			break;
		case 8012:
			msg=Func812();// /
			break;
		case 8013:
			msg=Func813();// /
			break;
		case 8030:
			msg=Func830();// /
			break;
		case 8031:
			msg=Func831();// /
			break;
		case 8032:
			msg=Func832();// /
			break;
		case 8033:
			msg=Func833();// /
			break;
		case 8034:
			msg=Func834();// /
			break;
		case 8035:
			msg=Func835();// /
			break;
		case 8036:
			msg=Func836();// /
			break;
		case 8037:
			msg=Func837();// /
			break;
		case 8038:
			msg=Func838();// /
			break;
		case 8039:
			msg=Func839();// /
			break;
		case 8040:
			msg=Func840();// /
			break;
		case 8041:
			msg=Func841();
			break;
		case 8042:
			msg=Func842();
			break;
		case 8043:
			msg=Func843();
			break;
		case 8044:
			msg=Func844();
			break;
		case 8045:
			msg=Func845();
			break;
		case 8046:
			msg=Func846();
			break;
		case 8047:
			msg=Func847();
			break;
		case 8048:
		case 8049:
		case 8050:
		case 8051:
			
			break;
		case 8052:
			msg=Func852();
			break;
		case 8053:
			msg=Func853();
			break;
		case 8054:
			msg = FuncHHAV();
			break;
		case 8055:
			msg = FuncLLAV();
			break;
		case 8056:
			msg = Func856();
			break;
		default:
			break;
		}

	}

	//---		返回计算结果
	ARRAY_BE  lt;
	lt.line= new float[m_MaxPoint];

	while(cs=m_table.pop_it())
	{
		if(m_RlineNum > RLINEE-RLINEB)
			break;
		if (LESS_ARRARY_INDEX(m_Rline, m_RlineNum))
		{
			m_Rline[m_RlineNum].line= new float[m_MaxPoint];
			if(cs< PAM)//如CLOSE，DAY等
			{
				GetData(cs,lt);
				m_Rline[m_RlineNum].b=lt.b;
				m_Rline[m_RlineNum].e=lt.e;
				if(m_Rline[m_RlineNum].b>=0 && m_Rline[m_RlineNum].e<=m_MaxPoint-1 && m_Rline[m_RlineNum].b<=m_MaxPoint-1 && m_Rline[m_RlineNum].b<=m_Rline[m_RlineNum].e)
					for(i=lt.b;i<=lt.e;i++)
						m_Rline[m_RlineNum].line[i] = lt.line[i];
			}
			else
			{
				if (LESS_ARRARY_INDEX(m_MidArray, cs-PCMB))
				{
					m_Rline[m_RlineNum].b=m_MidArray[cs-PCMB].b;
					m_Rline[m_RlineNum].e=m_MidArray[cs-PCMB].e;
					if(m_Rline[m_RlineNum].b>=0 && m_Rline[m_RlineNum].e<=m_MaxPoint-1 && m_Rline[m_RlineNum].b<=m_MaxPoint-1 && m_Rline[m_RlineNum].b<=m_Rline[m_RlineNum].e)
						for(i=m_MidArray[cs-PCMB].b;i<=m_MidArray[cs-PCMB].e;i++)
							m_Rline[m_RlineNum].line[i] = m_MidArray[cs-PCMB].line[i];
				}			
			}
		}				
		//	fangz0925 测试修改,FSL 指标多返回一条线
		//	m_RlineNum++;
	}
#ifdef TEST_Version1
	CGmtTime tm_ = CGmtTime::GetCurrentTime();
	if(tm_>CGmtTime(2004,03,01,9,30,0))
	{	
		for(int i = 0;i<m_RlineNum;i++)
		{
			float ff = 0.05;
			for(int j = 0;j<m_MaxPoint;j++)
				m_Rline[m_RlineNum].line[j] += ff;
		}
	}
#endif

	DEL(lt.line);
	return 0;
#ifndef _DEBUG
	}
	catch(...)
	{
	}
	return -100;
#endif

}

int CFormularComputeParent::Func101()
{
	
	int cl,cr;
	ARRAY_BE fcl;
	fcl.line = new float[m_MaxPoint];
	cr=m_table.pop_it();
	cl=m_table.pop_it();
	GetData(cl,fcl);
	if(fcl.b>fcl.e||fcl.e>m_MaxPoint-1||fcl.b<0)
	{
		m_Rline[cr-RLINEB].b =fcl.b;
		m_Rline[cr-RLINEB].e =fcl.e;
		DEL_ARRAY(fcl.line);
		return 999;
	}

	m_Rline[cr-RLINEB] = fcl;
	DEL_ARRAY(fcl.line);

	return 0;
}

int CFormularComputeParent::Func102()
{
	int cl,cr;
	ARRAY_BE fcr;
	fcr.line = new float[m_MaxPoint];
	cl=m_table.pop_it();
	cr=m_table.pop_it();
	GetData(cr,fcr);
	if(fcr.b>fcr.e||fcr.e>m_MaxPoint-1||fcr.b<0)
	{
		m_VarArray[cl-MIDB].b =fcr.b;
		m_VarArray[cl-MIDB].e =fcr.e;
		DEL_ARRAY(fcr.line);
		return 999;
	}
	m_VarArray[cl-MIDB] =fcr;
	DEL_ARRAY(fcr.line);
	return 0;
}

//=====and=======//
int CFormularComputeParent::Func301()
{
	int cl,cr;
	int bl,be;
	ARRAY_BE fcl,fcr;

	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e); 

	bl = m_MidArray[m_MidNum].b;
	be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		DEL_ARRAY(fcl.line);
		DEL_ARRAY(fcr.line);
		return 999;
	}


	for(int i=bl;i<=be;i++)
	{
		if(fcl.line[i] && fcr.line[i])
			m_MidArray[m_MidNum].line[i] = 1;
		else 
			m_MidArray[m_MidNum].line[i] = 0;
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//===or==//
int CFormularComputeParent::Func302()
{
	int cl,cr;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e); 

	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		if(fcl.line[i] || fcr.line[i])
			m_MidArray[m_MidNum].line[i] = 1;
		else 
			m_MidArray[m_MidNum].line[i] = 0;
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//== >==//
int CFormularComputeParent::Func401()
{
	int cl,cr;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e); 
	

	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		if(fcl.line[i] > fcr.line[i])
			m_MidArray[m_MidNum].line[i] = 1;
		else 
			m_MidArray[m_MidNum].line[i] = 0;
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//====<===//
int CFormularComputeParent::Func402()
{
	int cl,cr;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e); 
	

	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		if(fcl.line[i] < fcr.line[i])
			m_MidArray[m_MidNum].line[i] = 1;
		else 
			m_MidArray[m_MidNum].line[i] = 0;
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//==== >= =====//
int CFormularComputeParent::Func405()
{
	int cl,cr;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];

	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e); 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		if(fcl.line[i] >= fcr.line[i])
			m_MidArray[m_MidNum].line[i] = 1;
		else 
			m_MidArray[m_MidNum].line[i] = 0;
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//======= <= =========//
int CFormularComputeParent::Func406()
{
	int cl,cr;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e); 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		if(fcl.line[i] <= fcr.line[i])
			m_MidArray[m_MidNum].line[i] = 1;
		else 
			m_MidArray[m_MidNum].line[i] = 0;
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//========= == =======//
int CFormularComputeParent::Func408()
{
	int cl,cr;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e); 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		if(fcl.line[i] == fcr.line[i])
			m_MidArray[m_MidNum].line[i] = 1;
		else 
			m_MidArray[m_MidNum].line[i] = 0;
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//========== != =========//
int CFormularComputeParent::Func409()
{
	int cl,cr;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e); 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		if(fcl.line[i] != fcr.line[i])
			m_MidArray[m_MidNum].line[i] = 1;
		else 
			m_MidArray[m_MidNum].line[i] = 0;
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//=========+=======//
int CFormularComputeParent::Func501()
{
	int cl,cr;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e); 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	m_MidArray[m_MidNum] .Add(fcl,fcr);
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;

}
//======-==========//
int CFormularComputeParent::Func502()
{
	int cl,cr;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e); 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	m_MidArray[m_MidNum] .Sub(fcl,fcr);

	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;

}
//=======*=====/
int CFormularComputeParent::Func601()
{
	int cl,cr;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	cl=m_table.pop_it();
	
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e); 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	m_MidArray[m_MidNum] .Mult(fcl,fcr);
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;

}
//========/==========//
int CFormularComputeParent::Func602()
{
	int cl,cr;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e); 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	m_MidArray[m_MidNum] .Div(fcl,fcr);

	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//=======*-=====/
int CFormularComputeParent::Func603()
{
	int cl,cr;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	cl=m_table.pop_it();
	
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e); 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	m_MidArray[m_MidNum] .Mult(fcl,fcr);
	m_MidArray[m_MidNum] .Mult(m_MidArray[m_MidNum],-1);
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;

}
//========/-==========//
int CFormularComputeParent::Func604()
{
	int cl,cr;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e); 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	m_MidArray[m_MidNum] .Div(fcl,fcr);
	m_MidArray[m_MidNum] .Div(m_MidArray[m_MidNum],-1);

	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//===max====//
int CFormularComputeParent::Func801()
{
	int cl,cr;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e); 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
			m_MidArray[m_MidNum].line[i] =
					max(fcl.line[i],fcr.line[i]) ;

	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//=========min=======//
int CFormularComputeParent::Func804()
{
	int cl,cr;
	ARRAY_BE fcl,fcr;
		
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e); 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		m_MidArray[m_MidNum].line[i] 
			=	min(fcl.line[i],fcr.line[i]) ;
	}

	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//=========mod=======//
int CFormularComputeParent::Func805()
{
	int cl,cr;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e); 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		//m_MidArray[m_MidNum].line[i]
		//		=(float)((int)fcl.line[i]/(int)fcr.line[i]) ;
		m_MidArray[m_MidNum].line[i] = (int)fcl.line[i]%(int)fcr.line[i];
	}
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//=========reverse=======//
int CFormularComputeParent::Func806()
{
	int cl;
	ARRAY_BE fcl;
	
	fcl.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b; 
	m_MidArray[m_MidNum].e =fcl.e; 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		return 999;
	}

	m_MidArray[m_MidNum].Sub (0.0f,fcl );

	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	return 0;
}
//=========if=======//
int CFormularComputeParent::Func807()
{
	int cl,cr,ce, ret;
	ARRAY_BE fcl,fcr,fce;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	fce.line = new float[m_MaxPoint];

	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	ce=m_table.pop_it();
	GetData(ce,fce);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].b =max(m_MidArray[m_MidNum].b ,fce.b); 

	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e);
	m_MidArray[m_MidNum].e =min(m_MidArray[m_MidNum].e,fce.e); 
 
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		delete[] fce.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{	
		ret =(int)fcl.line[i];
		if(ret)
			m_MidArray[m_MidNum].line[i] =fcr.line[i];
		else
			m_MidArray[m_MidNum].line[i] =fce.line[i];
	}

	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	delete[] fce.line;
	return 0;
}
//=========abs=======//
int CFormularComputeParent::Func802()
{
	int cl;
	ARRAY_BE fcl;
	
	fcl.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b; 
	m_MidArray[m_MidNum].e =fcl.e; 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
		m_MidArray[m_MidNum].line[i] 
				=fabsf(fcl.line[i]) ;
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	return 0;
}
//=========ln=======//
int CFormularComputeParent::Func808()
{
	int cl;
	ARRAY_BE fcl;
	
	fcl.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b; 
	m_MidArray[m_MidNum].e =fcl.e; 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		if(fcl.line[i] > 0)
			m_MidArray[m_MidNum].line[i] 
				=(float)log(fcl.line[i]);
		else 
		{
			m_table.push_b(PCMB+m_MidNum);
			m_MidNum++;
			delete[] fcl.line;
			return 8;
		}
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	return 0;
}
//=========not=======//
int CFormularComputeParent::Func809()
{
	int cl,ret;
	ARRAY_BE fcl;
	
	fcl.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b; 
	m_MidArray[m_MidNum].e =fcl.e; 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		ret =(int)fcl.line[i];
		if(ret == 0)
			m_MidArray[m_MidNum].line[i] =1 ;
		else
			m_MidArray[m_MidNum].line[i] =0 ;
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	return 0;
}
//=========sgn=======//
int CFormularComputeParent::Func810()
{
	int cl,ret;
	ARRAY_BE fcl;
	
	fcl.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b; 
	m_MidArray[m_MidNum].e =fcl.e; 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		ret =(int)fcl.line[i];
		if(ret == 0) 
			m_MidArray[m_MidNum].line[i] =0 ;
		else
		{
			if(ret <0) 	m_MidArray[m_MidNum].line[i] =-1 ;
			else	    m_MidArray[m_MidNum].line[i] =1 ;
		}
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	return 0;
}
//=========exp=======//
int CFormularComputeParent::Func803()
{
	int cl;
	ARRAY_BE fcl;
	
	fcl.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b; 
	m_MidArray[m_MidNum].e =fcl.e; 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		m_MidArray[m_MidNum].line[i] 
			=(float)exp(fcl.line[i]);
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	return 0;
}
//=========log=======//
int CFormularComputeParent::Func811()
{
	int cl;
	ARRAY_BE fcl;
	
	fcl.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	
	GetData(cl,fcl);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b; 
	m_MidArray[m_MidNum].e =fcl.e; 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		if(fcl.line[i] > 0)
			m_MidArray[m_MidNum].line[i] 
				=(float)log10(fcl.line[i]);
		else
		{
			m_table.push_b(PCMB+m_MidNum);
			m_MidNum++;
			delete[] fcl.line;
			return 11;
		}

	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	return 0;
}

//=========sqrt=======//
int CFormularComputeParent::Func813()
{
	int cl;
	ARRAY_BE fcl;
	
	fcl.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b; 
	m_MidArray[m_MidNum].e =fcl.e; 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		if(fcl.line[i] >= 0)
			m_MidArray[m_MidNum].line[i]
				=(float)sqrt(fcl.line[i]);
		else
		{
			if(i== bl)
			{
				m_table.push_b(PCMB+m_MidNum);
				m_MidNum++;
				delete[] fcl.line;
				return 12;
			}
			else
				m_MidArray[m_MidNum].line[i]
					=m_MidArray[m_MidNum].line[i-1];
		}
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	return 0;
}
//========== pow =========//
int CFormularComputeParent::Func812()
{
	int cl,cr;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b); 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e); 
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		try
		{
		m_MidArray[m_MidNum].line[i] 
			= (float)pow(fcl.line[i],fcr.line[i]);
		}
		catch(...)
		{
			if(i!=bl)
				m_MidArray[m_MidNum].line[i] 
					= m_MidArray[m_MidNum].line[i-1] ;
		}
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
// avedev
int CFormularComputeParent::Func830()
{
	int cl,cr,ret,j;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	ret = fcr.b;
	ret =(int)fcr.line[ret];
	if(ret<1)
	{
		delete[] fcl.line;
		delete[] fcr.line;
		return 30;
	}
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b+ret-1; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		float ax=0,x=0;
		for(j=i-ret+1;j<=i;j++)
				ax+=fcl.line[j];
		ax/=ret;
		for(j=i-ret+1;j<=i;j++)
				x=(float)(fabsf(fcl.line[j]-ax))+x;
		x/=ret;
		m_MidArray[m_MidNum].line[i] = x;
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
// devsq
int CFormularComputeParent::Func833()
 {
	int cl,cr,ret,j;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	ret = fcr.b;
	ret =(int)fcr.line[ret];
	
	if(ret<1) 
	{
		delete[] fcl.line;
		delete[] fcr.line;
		return 33;
	}
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b+ret-1; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		float ax=0,x=0;
		for(j=i-ret+1;j<=i;j++)
				ax+=fcl.line[j];
		ax/=ret;
		for(j=i-ret+1;j<=i;j++)
				x=(fcl.line[j]-ax)*(fcl.line[j]-ax)+x;
		m_MidArray[m_MidNum].line[i] = x;
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//  
int CFormularComputeParent::Func832() 
{
	int cl,cr;
	ARRAY_BE fcl,fcr;

	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =max(fcl.b,fcr.b)/*+1*/; 
	m_MidArray[m_MidNum].e =min(fcl.e,fcr.e);
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

    if (bl == be)
    {
        m_MidArray[m_MidNum].line[bl] = 0;
    }
    else
    {
        for(int i=bl;i<=be;i++)
        {
            if (i == bl)
            {
                m_MidArray[m_MidNum].line[i] = 0;
            }
            else if(fcl.line[i-1] < fcr.line[i-1] && fcl.line[i] > fcr.line[i])
            {
                m_MidArray[m_MidNum].line[i] = 1;
            }
            else
            {
                m_MidArray[m_MidNum].line[i] = 0;
            }
        }
    }
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
// count
int CFormularComputeParent::Func831() 
{
	int cl,cr,ret,j;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	ret = fcr.b;
	ret =(int)fcr.line[ret];
	if(ret<0) 
	{
		delete[] fcl.line;
		delete[] fcr.line;
		return 31;
	}
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	if(ret == 0)
		m_MidArray[m_MidNum].b =fcl.b; 
	else
		//m_MidArray[m_MidNum].b =fcl.b+ret-1; 
		m_MidArray[m_MidNum].b =fcl.b; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	if(ret>0)
		for(int i=bl;i<=be;i++)
		{
			float x=0;

			j=i-ret+1;
			if (j < 0) j=0;
			if (j < fcl.b) j = fcl.b;

			for(j=j;j<=i;j++)
				if(fcl.line[j] != 0) x+=1;
			m_MidArray[m_MidNum].line[i] = x;
		}
	else
		for(int i=bl;i<=be;i++)
			m_MidArray[m_MidNum].line[i] = i+1;
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
// ema
int CFormularComputeParent::Func834() 
{
	int cl,cr,ret;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	ret = fcr.b;
	ret =(int)fcr.line[ret];

	//
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b/*+ret-1*/; 
	m_MidArray[m_MidNum].e =fcl.e;

	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
//（该日DI －前一日的EMA_M）* 2 / （M＋1） ＋前一日的EMA_M
	//m_pData[i].close 
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}
	m_MidArray[m_MidNum].line[bl]=fcl.line [bl];

	// CAutoFile fMidAarray("c:/ema.txt");

	double db = fcl.line [bl];
	for(int i1=bl+1;i1<=be;i1++)
	{
		//add lmb ;20010301
		ret = (int)fcr.line[i1]+1;

		//
		if(ret<=1)
		{
			// 参数非法的时候, 直接赋值, 不用计算
			db = fcl.line [i1];
			m_MidArray[m_MidNum].line[i1] = db;
		}	
		else
		{
			//end add
			db = (fcl.line [i1]-db)*2.0/(ret) + db;
			m_MidArray[m_MidNum].line[i1]=db;
		}
		// fprintf(fMidAarray.GetFP(),"%d %f \n",i1,db);
	}
	// fprintf(fMidAarray.GetFP(),"%s:%d %s:%d\n","begin",m_MidArray[m_MidNum].b,"end",m_MidArray[m_MidNum].e);

	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
// hhv
int CFormularComputeParent::Func835()
 {
	int cl,cr,ret,j;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
		//change lmb ;20010301
	ret = GetBeginFoot( fcr);
		//end add
//	ret = fcr.b;
//	ret =(int)fcr.line[ret];
	
	if(ret<0) //if(ret == 0) is from the first day
	{
		delete[] fcl.line;
		delete[] fcr.line;
		return 35;
	}
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b; 
//	if(ret ==0) m_MidArray[m_MidNum].b =fcl.b; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	if(ret > 0)
	{
		for(int i=bl;i<=be;i++)
		{
			//add lmb ;20010301
			ret = (int)fcr.line[i];
			if(ret<1) ret = 1;
			//end add
			float ax=fcl.line[i];
			if(i-bl<ret)
			{
				for(j=bl;j<=i;j++)
					ax=max(ax,fcl.line[j]);
			}
			else
			{
				for(j=i-ret+1;j<=i;j++)
					ax=max(ax,fcl.line[j]);
			}
			m_MidArray[m_MidNum].line[i] = ax;
		}
	}
	else
	{
		float ax=fcl.line[bl];
		for(int i=bl;i<=be;i++)
		{
//			float ax=fcl.line[i-ret+1];
			ax=max(ax,fcl.line[i]);
			m_MidArray[m_MidNum].line[i] = ax;
		}
	}

	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//  llv
int CFormularComputeParent::Func836() 
{
	int cl,cr,ret,j;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
		//change lmb ;20010301
	ret = GetBeginFoot( fcr);
		//end add
//	ret = fcr.b;
//	ret =(int)fcr.line[ret];
	
	if(ret<0) 
	{
		delete[] fcl.line;
		delete[] fcr.line;
		return 36;
	}
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b; 
//	if(ret ==0) m_MidArray[m_MidNum].b =fcl.b; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	if(ret > 0)
	{
		for(int i=bl;i<=be;i++)
		{
			//add lmb ;20010301
			ret = (int)fcr.line[i];
			if(ret<1) ret = 1;
			//end add
			float ax=fcl.line[i];
			if(i-bl<ret)
			{
				for(j=bl;j<=i;j++)
					ax=min(ax,fcl.line[j]);
			}
			else
			{
				for(j=i-ret+1;j<=i;j++)
					ax=min(ax,fcl.line[j]);
			}
			m_MidArray[m_MidNum].line[i] = ax;
		}
	}
	else
	{
		float ax=fcl.line[bl];
		for(int i=bl;i<=be;i++)
		{
//			float ax=fcl.line[i-ret+1];
			ax=min(ax,fcl.line[i]);
			m_MidArray[m_MidNum].line[i] = ax;
		}
	}

	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//sar
int CFormularComputeParent::Func837()
{	
	//lint --e{438}
	int cl,cr,ce/*,j*/;
	ARRAY_BE fcl,fcr,fce;
	
	fcl.line = new float[m_MaxPoint];	// 周期
	fcr.line = new float[m_MaxPoint];	// 步长
	fce.line = new float[m_MaxPoint];	// 极值
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	ce=m_table.pop_it();
	GetData(ce,fce);
/*SAR(N,S,M),N为计算周期,S为步长,M为极值
例如SAR(10,2,20)表示计算10日抛物转向，步长为2%，极限值为20%*/

	int iCycle = fcl.b;
	iCycle =(int)fcl.line[iCycle];
	if(iCycle<1)		// 周期
	{
		delete[] fcl.line;
		delete[] fcr.line;
		return 37;
	}

	int nr = fcr.b;
	float fStep =(float)(fcr.line[nr]/100);
	if(fStep<0.001/*||fStep>0.05*/)	// 步长
	{
		delete[] fcl.line;
		delete[] fcr.line;
		return 37;
	}

	int ne = fce.b;
	float fStepMax =(float)(fce.line[ne]/100);
	if(fStepMax<0.01/*||fStepMax>0.8*/) return 37;	// 极限值

	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b+iCycle-1; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		delete[] fce.line;
		return 999;
	}
	// http://zhidao.baidu.com/question/26326791
// 	计算公式：
// 		1、 画SAR之前，首先要决定你开始画的第一天，是属于多头或空头趋势？
// 		2、 如果第一天属于多头，则第一天的SAR一定是4天来的最低点（包括今天在内）。
// 		3、 找出开始第一天的SAR之后，紧接着计算下一日的SAR：
// 		下一日的SAR=第一天SAR+（0·02*XP）
// 		XP=第一天的最高点—第一天的SAR
// 		4、 第二天收盘时，又可以计算出第三天的SAR，如果第二天最高价>第一天最高价，则第三天的SAR=第二天SAR+（0·04*XP）
// 		XP=第二天的最高点—第二天的SAR
// 		只要最高价>前一日最高价，则其乘数因子一律增加0·02，如果一直累增下去，最高只能累增至0·2为止，之后一律以0·2为乘数因子。
// 		如果最高价≤前一日最高价，则第三天的SAR和第二天的SAR相同，而且乘数因子也不必累加。
// 		第三天收盘后，依上述步骤持续在每日价格下方画出SAR，直到某一天收盘价跌破SAR，则代表行情转为空头应卖出股票，而行情转为空头的当天，立即将四天来的最高点，做为次一日的空头SAR。
// 		5、 反转后第二天的最低点如果≥前一天的最低点，则SAR和前一天相同。
// 		注意！SAR虽然和前一天相，也必须在图形上画出来。
// 		6、 反转后第二天的最低点若<前一天的最低点，则
// 		第三天的SAR=第二天的SAR+90·02*XK）。
// 		XK=第二天的最低价—第二天的SAR。
// 		第三天以后持续依照上述步骤，在每日价格上方画出SAR，直到某一天收盘价向上突破SAR，代表行情为多头应买进股票，而行情转为多头的当天，立刻按照2的步骤设定SAR。
// 注意！一般SAR的参数设定为四天
// 
// 	二、 SAR指标的计算方法
// 		
// 		和MACD、DMI等指标相同的是，SAR指标的计算公式相当烦琐。SAR的计算工作主要是针对每个周期不断变化的SAR的计算，也就是停损价位的计算。在计算SAR之前，先要选定一段周期，比如n日或n周等，n天或周的参数一般为4日或4周。接下来判断这个周期的股价是在上涨还是下跌，然后再按逐步推理方法计算SAR值。
// 		
// 		计算日SAR为例，每日SAR的计算公式如下：
// 		
// 		SAR（n）=SAR（n－1）+AF<EP(n－1)—SAR（n－1）>
// 		
// 		其中，SAR（n）为第n日的SAR值，SAR（n－1）为第（n－1）日的值
// 		
// 		AF为加速因子（或叫加速系数），EP为极点价（最高价或最低价）
// 		
// 		在计算SAR值时，要注意以下几项原则：
// 		
// 		1、 一次计算SAR值时须由近期的明显高低点起的第n天开始。
// 		
// 		2、如果是看涨的行情，则SAR（0）为近期底部最低价；如果是看跌行情，则SAR（0）为近期顶部的最高价。
// 		
// 		3、加速因子AF有向上加速因子和向下加速因子的区分。若是看涨行情，则为向上加速因子；若是看跌行情，则为向下加速因子。
// 		
// 		4、加速因子AF的初始值一直是以0.02为基数。如果是在看涨行情中买入股票后，某天的最高价比前一天的最高价还要高，则加速因子AF递增0.02，并入计算。但加速因子AF最高不超过0.2。反之，看跌行情中也以此类推。
// 		
// 		5、如果在看涨行情中，计算出的某日的SAR值比当日或前一日的最低价高，则应以当日或前一日的最低价为该日的SAR值。如果在看跌行情中，计算出的某日的SAR值比当日或前一日的最高价低，则应以当日或前一日的最高价为某日的SAR值。总之，SAR值不得定于当日或前一日的行情价格变动幅度之内。
// 		
// 		6、任何一次行情的转变，加速因子AF都必须重新由0.02起算。
// 		
// 		7、SAR指标周期的计算基准周期的参数为2，如2日、2周、2月等，其计算周期的参数变动范围为2—8。
// 		
//		8、SAR指标的计算方法和过程比较烦琐，对于投资者来说只要掌握其演算过程和原理，在实际操作中并不需要投资者自己计算SAR值，更重要的是投资者要灵活掌握和运用SAR指标的研判方法和功能。

	int flag;  // 0 多头继续 1 空头继续
			//  2 多头开始 3 空头开始
	float fSar=0.0f,fStepSum =0.0f;		// hl为当前sar值，cft累积步进
	int iOrgBegin = bl-iCycle+1;
	float newhigh,newlow;
	const int KIDuoTouJiXu		= 0;
	const int KIKongTouJiXu		= 1;
	const int KIDuoTouKaiShi	= 2;
	const int KIKongTouKaiShi	= 3;
	if((float)((m_pData[bl].close>=m_pData[iOrgBegin].close)))	// 开始计算的第一天如果大于开始数据第一天收盘则为涨，否则跌
		flag = KIDuoTouKaiShi;
	else
		flag = KIKongTouKaiShi;

	//flag = KIDuoTouKaiShi;	// 通达信貌似开始总是看涨

	newhigh = -FLT_MAX;
	newlow  = FLT_MAX;
	int i = 0;
	for ( i=iOrgBegin; i <= bl ; ++i )
	{
		if ( m_pData[i].high > newhigh )
		{
			newhigh = m_pData[i].high;
		}
		if ( m_pData[i].low < newlow )
		{
			newlow = m_pData[i].low;
		}
	}

	if ( KIDuoTouKaiShi == flag )
	{
		fSar = newlow;
	}
	else
	{
		fSar = newhigh;
	}
	for ( i=bl; i<=be ; i++ )
	{
		switch(flag)
		{
		case KIDuoTouJiXu:	
			// 多头继续状态，如果最低价小于sar值，则下一个数据以空头开始状态计算且今天的sar取昨天的最低价
			//     如果最高价大于前一天的最高价，则step递增

			// 转状态只会用到最高价，不会用最低价

			if ( m_pData[i].high > m_pData[i-1].high )
				fStepSum+=fStep;

			if(fStepSum>fStepMax)
				fStepSum =fStepMax;

			m_MidArray[m_MidNum].line[i] =
					m_MidArray[m_MidNum].line[i-1]
					+ (fStepSum* (m_pData[i-1].high-m_MidArray[m_MidNum].line[i-1]) );
// 			m_MidArray[m_MidNum].line[i] =
// 				m_MidArray[m_MidNum].line[i-1]
// 					+ (fStepSum* (m_pData[i].high-m_MidArray[m_MidNum].line[i-1]) );	// 通达信貌似使用今日高价
			if ( m_pData[i].low < m_MidArray[m_MidNum].line[i] )
			{
				// 今日低价小于今日sar值，则马上切换看涨看跌状态
				--i;	// 切换状态后重新计算
				flag=KIKongTouKaiShi;
			}
			break;
		case KIKongTouJiXu:
			// 空头继续状态，如果最高价大于sar值，则下一个数据以多头开始状态计算且今天的sar取昨天的最高价
			//     如果最低价小于前一天的最低价，则step递增
			
			if ( m_pData[i].low < m_pData[i-1].low )
				fStepSum+=fStep;

			if(fStepSum>fStepMax) 
				fStepSum =fStepMax;

			m_MidArray[m_MidNum].line[i] =
					m_MidArray[m_MidNum].line[i-1] + fStepSum*
					(m_pData[i-1].low - m_MidArray[m_MidNum].line[i-1]);
// 			m_MidArray[m_MidNum].line[i] =
// 				m_MidArray[m_MidNum].line[i-1] + fStepSum*
// 				(m_pData[i].low - m_MidArray[m_MidNum].line[i-1]);	// 通达信貌似使用今日低价

			if ( m_pData[i].high > m_MidArray[m_MidNum].line[i])
			{
				//m_MidArray[m_MidNum].line[i] = m_pData[i-1].high;
				// 今日高价大于今日sar值，则马上切换看涨看跌状态
				--i;	// 切换状态后重新计算
				flag=KIDuoTouKaiShi;
			}
			break;
		case KIDuoTouKaiShi:
			// 多头开始: step重置 以前n周期内的最低价作为当前sar
			fSar = m_pData[i-iCycle+1].low;
			{
				for ( int j=i-iCycle+2; j <= i ; ++j )
				{
					if ( m_pData[j].low < fSar )
					{
						fSar = m_pData[j].low;
					}
				}
			}
			
			m_MidArray[m_MidNum].line[i] = fSar;
			flag = KIDuoTouJiXu;
			fStepSum = fStep;
			break;
		case KIKongTouKaiShi:
			// 空头开始: step重置 以前n周期内的最高价作为当前sar
			fSar = m_pData[i-iCycle+1].high;
			{
				for ( int j=i-iCycle+2; j <= i ; ++j )
				{
					if ( m_pData[j].high > fSar )
					{
						fSar = m_pData[j].high;
					}
				}
			}

			m_MidArray[m_MidNum].line[i] =fSar;
			flag	= KIKongTouJiXu;
			fStepSum = fStep;
			break;
		}
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	delete[] fce.line;

	return 0;
}
int CFormularComputeParent::Func838()
 {
	//lint --e{438}
	int cl =0,cr=0,j=0,ce =0;
	ARRAY_BE fcl,fcr,fce;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	fce.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	ce=m_table.pop_it();
	GetData(cr,fce);

	int nl = fcl.b;
	nl =(int)fcl.line[nl];
	if(nl<1) 
	{
		delete[] fcl.line;
		delete[] fcr.line;
		delete[] fce.line;
		return 38;
	}

	int nr = fcr.b;
	float cfr =(float)(fcr.line[nr]/100);
	if(cfr<0.01||cfr>0.05) 
	{
		delete[] fcl.line;
		delete[] fcr.line;
		delete[] fce.line;
		return 38;
	}

	int ne = fce.b;
	float cfe =(float)(fce.line[ne]/100);
	if(cfe<0.1||cfe>0.8) 
	{
		delete[] fcl.line;
		delete[] fcr.line;
		delete[] fce.line;
		return 38;
	}

	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b+nl-1; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		delete[] fce.line;
		return 999;
	}
	int fl=0;  // 0 多头继续 1 空头继续
			//  2 多头开始 3 空头开始
	float hl=0.0f,cft=0.0f;
	float newhigh=0.0f,newlow=0.0f;
	if((float)((m_pData[bl].close>m_pData[bl-nl+1].close)))
		fl=2;
	else
		fl=3;
	for(int i=bl;i<=be;i++)
	{
		switch(fl)
		{
		case 0:
			if(m_pData[i].high>newhigh)
			{
				newhigh =m_pData[i].high;
				cft+=cfr;
			}
			if(cft>cfe)
				cft =cfe;
			if(m_pData[i].high>m_pData[i-1].high)
			{
				m_MidArray[m_MidNum].line[i] =
					m_MidArray[m_MidNum].line[i-1]+cft*
					(m_pData[i-1].high-
					m_MidArray[m_MidNum].line[i-1]);
			}
			else
				m_MidArray[m_MidNum].line[i] =
					m_MidArray[m_MidNum].line[i-1];
			if(m_pData[i].close<=m_MidArray[m_MidNum].line[i-1])
				fl=3;
			break;
		case 1:
			if(m_pData[i].low>newlow)
			{
				newlow =m_pData[i].low;
				cft+=cfr;
			}
			if(cft>cfe) 
				cft =cfe;
			if(m_pData[i].low<m_pData[i-1].low)
			{
				m_MidArray[m_MidNum].line[i] =
					m_MidArray[m_MidNum].line[i-1]+cft*
					(m_pData[i-1].low-
					m_MidArray[m_MidNum].line[i-1]);
			}
			else
				m_MidArray[m_MidNum].line[i] =
					m_MidArray[m_MidNum].line[i-1];
			if(m_pData[i].close>=m_MidArray[m_MidNum].line[i-1])
				fl=3;

			break;
		case 2:
			hl=m_pData[i-nl+1].low;
			newhigh =m_pData[i-nl+1].high;
			for(j=i-nl+2;j<=i;j++)
			{
					if(hl>m_pData[j].low)
						hl=m_pData[j].low;
					if(newhigh<m_pData[j].high)
						newhigh = m_pData[j].high;
			}
			m_MidArray[m_MidNum].line[i] =hl;
			fl = 0;
			cft=0;
			break;
		case 3:
			hl=m_pData[i-nl+1].high;
			newlow =m_pData[i-nl+1].low;
			for(j=i-nl+2;j<=i;j++)
			{
				if(hl<m_pData[j].high)
					hl=m_pData[j].high;
				if(newlow<m_pData[j].low)
					newlow = m_pData[j].low;
			}
			fl=1;
			cft=0;
			m_MidArray[m_MidNum].line[i] =hl;
			break;
		}
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	delete[] fce.line;

	return 0;
}
//  ma
int CFormularComputeParent::Func840() 
{
	int cl,cr,ret,j;
	ARRAY_BE fcl,fcr;

	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
		//change lmb ;20010301
	ret = GetBeginFoot( fcr);
		//end add

	if(ret<1) 
	{
		delete[] fcl.line;
		delete[] fcr.line;
		return 40;
	}
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b/*+ret-1*/; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		float ax=0;
		//add lmb ;20010301
		ret = (int)fcr.line[i];
		if(ret<1) ret = 1;
		//end add

		int iBegin = i - ret + 1;

		if ( iBegin < 0 )
		{
			int iretreal = 0;
			iBegin = 0;

			for ( j = iBegin; j <= i; j++ )
			{
				ax += fcl.line[j];
				iretreal += 1;
			}

			if ( 0 != iretreal )
			{
				ax /= iretreal;
				m_MidArray[m_MidNum].line[i] = ax;
			}			
		}
		else
		{
			int iStart = i-ret+1;
			iStart = max(bl, iStart);	// 必须取有效值
			for(j=iStart;j<=i;j++)
				ax+=fcl.line[j];
			//ax/=ret;
			ax/=(i-iStart+1);
			m_MidArray[m_MidNum].line[i] = ax;
		}		
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}

// expma
int CFormularComputeParent::Func853()
{
	int cl,cr,ret/*,j*/;
	ARRAY_BE fcl,fcr;

	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
		//change lmb ;20010301
	ret = GetBeginFoot( fcr);
		//end add

	if(ret<1) 
	{
		delete[] fcl.line;
		delete[] fcr.line;
		return 40;
	}
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b+ret-1; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	m_MidArray[m_MidNum].line[bl] = fcl.line[bl];

	// CAutoFile fMidAarray("c:/expma.txt");

	double dValue = fcl.line[bl];
	
	for(int i=bl + 1;i<=be;i++)
	{
		//add lmb ;20010301
		ret = (int)fcr.line[i];
		if(ret<1) ret = 1;
		//end add
	
		dValue = (2* fcl.line[i] + ( ret -1 )*dValue) / ( ret + 1 );
			
		m_MidArray[m_MidNum].line[i] = dValue;

		// fprintf(fMidAarray.GetFP(),"%d %f \n",i,dValue);

	}

	// fprintf(fMidAarray.GetFP(),"%s:%d %s:%d\n","begin",m_MidArray[m_MidNum].b,"end",m_MidArray[m_MidNum].e);

/*	
	double db = fcl.line [bl];
	for(int i1=bl+1;i1<=be;i1++)
	{
		//add lmb ;20010301
		ret = fcr.line[i1]+1;
		if(ret<1) ret = 1;
		//end add
		db = (fcl.line [i1]-db)*2.0/(ret) + db;
		m_MidArray[m_MidNum].line[i1]=db;
	}
*/
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}

//  wma
int CFormularComputeParent::Func856() 
{
	int cl,cr,ret,j;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	//change lmb ;20010301
	ret = GetBeginFoot( fcr);
	//end add
	
	if(ret<1) 
	{
		delete[] fcl.line;
		delete[] fcr.line;
		return 40;
	}
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b/*+ret-1*/; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}
	
	for(int i=bl;i<=be;i++)
	{
		float ax=0;
		//add lmb ;20010301
		ret = (int)fcr.line[i];
		if(ret<1) ret = 1;
		//end add
		
		int iBegin = i - ret + 1;
		
		if ( iBegin < 0 )
		{
			int iTimes = 1;
			int iDiv = 0;

			//
			iBegin = 0;
			
			for ( j = iBegin; j <= i; j++, iTimes++ )
			{
				ax += (fcl.line[j] * iTimes);
				iDiv += iTimes;
			}
			
			if ( 0 != iTimes && 0 != iDiv )
			{
				ax /= iDiv;
				m_MidArray[m_MidNum].line[i] = ax;
			}			
		}
		else
		{
			int iStart = i-ret+1;
			iStart = max(bl, iStart);	// 必须取有效值

			int iTimes = 1;
			int iDiv = 0;

			//
			for(j=iStart;j<=i;j++, iTimes++)
			{
				ax+= (fcl.line[j] * iTimes);
				iDiv += iTimes;
			}
			
			if (0 != iTimes && 0 != iDiv)
			{
				ax/=iDiv;
				m_MidArray[m_MidNum].line[i] = ax;
			}			
		}		
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}

// ref
int CFormularComputeParent::Func839() 
{
	int cl,cr,ret;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
		//change lmb ;20010301
	ret = GetBeginFoot( fcr);
		//end add
//	ret = fcr.b;
//	ret =(int)fcr.line[ret];
	
	if(ret<0) 
	{
		delete[] fcl.line;
		delete[] fcr.line;
		return 39;
	}
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b+ret; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		//add lmb ;20010301
		ret = (int)fcr.line[i];
		if(ret<0) ret = 0;
		//end add
		m_MidArray[m_MidNum].line[i] = fcl.line[i-ret];
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
// std
int CFormularComputeParent::Func841()
 {
	int cl,cr,ret,j;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	ret = fcr.b;
	ret =(int)fcr.line[ret];
	
	if(ret<2) 
	{
		delete[] fcl.line;
		delete[] fcr.line;
		return 41;
	}
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b+ret-1; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		float ax=0,af=0;
		for(j=i-ret+1;j<=i;j++)
		{
			ax+=fcl.line[j]*fcl.line[j];
			af+=fcl.line[j];
		}
		ax=(float)sqrt(fabsf(ax*ret-af*af)/ret/(ret-1));
		m_MidArray[m_MidNum].line[i] = ax;
	}
/*	for(int i=bl;i<=be;i++)
	{
		float ax=0,af=0;
		for(j=i-ret+1;j<=i;j++)
		{
			af+=fcl.line[j];
		}
		af/=ret;
		for(j=i-ret+1;j<=i;j++)
		{
			ax+=(fcl.line[j]-af)*(fcl.line[j]-af);
		}
		ax=(float)sqrt(ax/(ret-1));
		m_MidArray[m_MidNum].line[i] = ax;
	}*/	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//  stdp
int CFormularComputeParent::Func842() 
{
	int cl,cr,ret,j;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	ret = fcr.b;
	ret =(int)fcr.line[ret];
	
	if(ret<1) 
	{
		delete[] fcl.line;
		delete[] fcr.line;
		return 42;
	}
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b+ret-1; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}
	
	for(int i=bl;i<=be;i++)
	{
		float ax=0,af=0;
		for(j=i-ret+1;j<=i;j++)
		{
			ax+=fcl.line[j]*fcl.line[j];
			af+=fcl.line[j];
		}
		ax=(float)sqrt((ax*ret-af*af)/ret/ret);
		m_MidArray[m_MidNum].line[i] = ax;
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//sum
int CFormularComputeParent::Func843() 
{
	int cl,cr,ret,j;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
		//change lmb ;20010301
//	ret = GetBeginFoot( fcr);
		//end add
	
	// fangz zhibiao 主力成本指标. 通过 GetBeginFoot 取起点有问题. 先这里这么改一下
	ret = fcr.b;
	ret =(int)fcr.line[ret];
	
	if(ret<0) 
	{
		delete[] fcl.line;
		delete[] fcr.line;
		return 43;//lmb6.15
	}

	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	// m_MidArray[m_MidNum].b =fcl.b+ret-1; 
	m_MidArray[m_MidNum].b =fcl.b; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(ret==0)
	{
		m_MidArray[m_MidNum].b =fcl.b;
		bl = m_MidArray[m_MidNum].b;
	}

	if(be>=bl&&bl>=0&&be>=0)
	{
//		if(0/*ret==0*/)
//		{
//// 			m_MidArray[m_MidNum].line[bl]=fcl.line[bl];
//// 			for(int i=bl+1;i<=be;i++)
//// 			{
//// 				m_MidArray[m_MidNum].line[i]=0;
//// 				float ax=0;
//// 				for(j=bl;j<=i;j++)
//// 				{
//// 					ax+=fcl.line[j];
//// 				}
//// 				m_MidArray[m_MidNum].line[i] = fcl.line[i] + m_MidArray[m_MidNum].line[i-1];
//// 			}
//		}
//		else
		{
			for(int i=bl;i<=be;i++)
			{
				float ax=0;
				//add lmb ;20010301
				ret = (int)fcr.line[i];
				if ( ret <= 0 )
				{
					// 从头加到这里
					for(int iIndex=fcl.b;iIndex<=i;iIndex++)
					{
						ax+=fcl.line[iIndex];
					}
			
					m_MidArray[m_MidNum].line[i] = ax;
				}
				else 
				{
					if(ret<1) ret = 1;
					//end add
					
					j=i-ret+1;
					if (j < 0) j=0;
					if (j < fcl.b) j = fcl.b;
					
					//
					for(j=j;j<=i;j++)
					{					
						ax+=fcl.line[j];
					}
					m_MidArray[m_MidNum].line[i] = ax;
				}
				
			}
		}
	}	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
// varp
int CFormularComputeParent::Func844() 
{
	int cl,cr,ret,j;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	ret = fcr.b;
	ret =(int)fcr.line[ret];
	
	if(ret<2) 
	{
		delete[] fcl.line;
		delete[] fcr.line;
		return 44;
	}
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b+ret-1; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		float ax=0,af=0;
		for(j=i-ret+1;j<=i;j++)
		{
			ax+=fcl.line[j]*fcl.line[j];
			af+=fcl.line[j];
		}
		ax=(ax*ret-af*af)/ret/ret;
		m_MidArray[m_MidNum].line[i] = ax;
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	delete[] fcr.line;
		return 0;
}
//var
int CFormularComputeParent::Func845()
{
	int cl,cr,ret,j;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	ret = fcr.b;
	ret =(int)fcr.line[ret];
	
	if(ret<1)
	{
		delete[] fcl.line;
		delete[] fcr.line;
		return 45;
	}
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b+ret-1; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		return 999;
	}

	for(int i=bl;i<=be;i++)
	{
		float ax=0,af=0;
		for(j=i-ret+1;j<=i;j++)
		{
			ax+=fcl.line[j]*fcl.line[j];
			af+=fcl.line[j];
		}
		if (ret != 0 && ret-1 != 0)
		{
			ax=(ax*ret-af*af)/ret/(ret-1);
		}		
		m_MidArray[m_MidNum].line[i] = ax;
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}
//sma
int CFormularComputeParent::Func846()
{
	int cl,cr,ce,ret/*,j*/;
	ARRAY_BE fcl,fcr,fce;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	fce.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	ce=m_table.pop_it();
	GetData(ce,fce);
	
	ret = fcr.b;
	ret =(int)fcr.line[ret];
	
	if(ret<1)
	{
		delete[] fcl.line;
		delete[] fcr.line;
		delete[] fce.line;
		return 46;
	}
	int ret2 = fce.b;
	ret2 =(int)fce.line[ret2];
	if(ret2>=ret)
	{
		delete[] fcl.line;
		delete[] fcr.line;
		delete[] fce.line;
//		AfxMessageBox("请修改函数“sma”的第2、3个参数，使得第2个参数大于第3个参数");
		return 46;
	}
	if(ret2<1)
	{
		delete[] fcl.line;
		delete[] fcr.line;
		delete[] fce.line;
		return 46;
	}
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b/*+ret-1*/; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		delete[] fcr.line;
		delete[] fce.line;
		return 999;
	}

	if(be>=bl&&bl>=0&&be>=0)
	{
// 		m_MidArray[m_MidNum].line[bl] = 0;
// 		for(j=bl-ret+1;j<=bl;j++)
// 		{
// 			m_MidArray[m_MidNum].line[bl]+=fcl.line[j];
// 		}
// 		m_MidArray[m_MidNum].line[bl] /= ret;
// 
// 		double db = m_MidArray[m_MidNum].line[bl];
// 		for(int i=bl+1;i<=be;i++)
// 		{
// /*//求移动平均。
// 用法:
// SMA(X,N,M),求X的N日移动平均，M为权重。
// 算法: 若Y=SMA(X,N,M)
// 则 Y=[M*X+(N-M)*Y')/N,其中Y'表示上一周期Y值,N必须大于M。
// 例如：SMA(CLOSE,30,1)表示求30日移动平均价*/
// 			db = (ret2*fcl.line[i]+(ret-ret2)*db)/ret;
// 			m_MidArray[m_MidNum].line[i] = db ;
// 		}

		float fTmp = fcl.line[bl];

		for ( int i = bl; i <= be; i++ )
		{
			if ( i == bl )
			{
				m_MidArray[m_MidNum].line[i] = fTmp;
			}
			else
			{
				fTmp = (ret2*fcl.line[i]+(ret-ret2)*fTmp)/ret;
				m_MidArray[m_MidNum].line[i] = fTmp;
			}
		}
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	delete[] fcl.line;
	delete[] fcr.line;
	delete[] fce.line;
	return 0;
}

void CFormularComputeParent::GetMsg(CString& str,int& pos)
{
	if(m_errmsg.IsEmpty ())
		str = " ";
	else
		str=m_errmsg;
	pos=m_errpos;
}


int CFormularComputeParent::GetLine(CString& LineName, ARRAY_BE& pline)
{
	int fl=1;
	for(int i=0;i<m_RlineNum;i++)
	{
		CString s1 = m_RlineName[i];
		s1.MakeLower ();
		CString s2 = LineName;
		s2.MakeLower ();
		if(s1==s2)
		{
			fl=0;
			int bl = m_Rline[i].b;
			int be = m_Rline[i].e;
			if(bl>be||bl<0||be<0||be>=m_MaxPoint)
				return 1;

			pline = m_Rline[i];
/*			pline.b = bl;
			pline.e = be;

			for(int j=bl;j<=be;j++)
				pline.line[j] = m_Rline[i].line[j];*/
			break;
		}
	}
	return fl;
}

int CFormularComputeParent::GetLine(int& index, ARRAY_BE& pline,CString& LineName)
{
	if (LESS_ARRARY_INDEX(m_RlineName, index) && LESS_ARRARY_INDEX(m_Rline, index))
	{
		int bl = m_Rline[index].b;
		int be = m_Rline[index].e;
		pline.b = bl;
		pline.e = be;
		LineName = m_RlineName[index];
		
		if(bl>be||bl<0||be<0||be>=m_MaxPoint)
			return 1;
		pline = m_Rline[index];		
		return 0;
	}

	return 1;
}
int CFormularComputeParent::GetIndexPoint(int &index,float &nPoint)
{
	if (LESS_ARRARY_INDEX(m_Rline, index))
	{		
		int bl = m_Rline[index].b;
		int be = m_Rline[index].e;

		if(bl>be||bl<0||be<0||be>=m_MaxPoint)
			return 1;
		nPoint=m_Rline[index].line[be];
		return 0;
	}
	return 1;
}

int CFormularComputeParent::AddLine(CString& LineName, ARRAY_BE& pline)
{
	if(m_VarNum >=MIDE)
		return 1;
	if (LESS_ARRARY_INDEX(m_VarArray, m_VarNum))
	{
		m_WordTableNew[LineName]=(void*)(MIDB+m_VarNum);

		m_VarArray[m_VarNum].line = new float[m_MaxPoint];

		m_VarArray[m_VarNum].b = pline.b;
		m_VarArray[m_VarNum].e = pline.e;

		for(int i=pline.b;i<=pline.e;i++)
			m_VarArray[m_VarNum].line[i] = pline.line[i];

		m_VarName[m_VarNum] = LineName;					
		m_VarNum++;
		m_exec     =0;
	}

	
	return 0;
}

void CFormularComputeParent::SetScreenBeginEnd(int iBegin, int iEnd)
{
	if (iBegin < 0 || iEnd < 0 || iBegin > iEnd)
	{
		// ASSERT(0);
 		return;
	}

	m_iSreenBegin = iBegin;
	m_iSreenEnd = iEnd;
}

int CFormularComputeParent::Relolve(int cl)//处理引用
//cl为这个引用的ID号
{
	
	return 0;
}


int CFormularComputeParent::GetNum(int tj)
{
//返回值：0为正确，1为没有找到指标，2为没有找到线*/
	int tu;
	if(!LookupParameterTable(tj,tu))
	{
		::MessageBox(GetActiveWindow(), _T("引用的公式的参数个数没有找到"), AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return 0;
	}
	return tu;
}

int CFormularComputeParent::GetRlineNum()
{
	return m_RlineNum;

}

CString CFormularComputeParent::GetRlineName(int foot)
{
	return m_RlineName[foot];
}
int GetInt(CString s)
{
	if(s.GetLength ()<2)
		return 0;
	int n = 0;
	int nRate[2] = {16,1};
	for(int i = 0;i<2;i++)
	{
		int ntmp = 0;
		if(s[i]>='0' && s[i]<='9')
			ntmp = s[i]-'0';
		else
			ntmp = 10+(s[i]-'a');
		n+=(ntmp*nRate[i]);
	}
	return n;
}

//lint --e{661,525}
int CFormularComputeParent::FindLineType()//计算返回线的类型
{
	if( m_SentNum<0)
		return 0;

	int flag=0;

	int nRtn = 0;


	m_tempWhich++;
	int i = 0;
	for(i=0;i<=m_SentNum;i++)
	{
		if(m_Sent[i]==1002)
		{
			m_tempWhich--;
			flag=2;
			break;
		}
		if(m_Sent[i]==1001)
		{
			flag=1;
			break;
		}

		if(m_Sent[i]>=FuncDllB+50 && m_Sent[i]<= FuncDllB+52)
		{
			
		}
	}
	if(flag==0 )//	of no name
	{

		m_SentNum=m_SentNum+2;
		for(i=m_SentNum-2;i>=0;i--)
		{
			m_Sent[i+2]=m_Sent[i];
		}
		CString temp;

		temp.Format(_T("%d"),(int)m_tempWhich);
		// temp="l"+temp;
		temp = " ";
		m_WordTableNew[temp]=(void*)(RLINEB+m_RlineNum);
		m_Rline[m_RlineNum].line = new float[m_MaxPoint];
//		m_table.push_b(RLINEB+m_RlineNum);
		m_RlineName[m_RlineNum] = temp;

		m_Sent[0]=RLINEB+m_RlineNum;
		m_Sent[1]=1001;
		m_Sent[m_SentNum+1]=0;

		m_RlineNum++;

	}

	if(flag == 2) return 0;
	if (m_tempWhich < 0) return 0;
	
	m_RlineType[m_tempWhich]   = 0;
	m_RlineTypeEx[m_tempWhich] = 0;

	for(i=0;i<=m_SentNum;i++)
	{
//		if(m_Sent[i]==1001)
//		{
//			m_tempWhich++;
//			continue;
//		}
		if(m_Sent[i]>=CONSB&&m_Sent[i]<=CONSE && i>0)
		{
			if(m_Sent[i-1]!=9005 )
			{
				for(int k = m_SentNum;k>=i;k--)
					m_Sent[k+1] = m_Sent[k];
				m_Sent[i] = 9005;
				m_SentNum++;
				i--;
				continue;
			}
		}

		if(m_Sent[i+1]>=CONSB&&m_Sent[i+1]<=CONSE)
		{
			if(m_Sent[i]!=9005 && i!=m_SentNum)
			{
				for(int k = m_SentNum;k>i;k--)
					m_Sent[k+1] = m_Sent[k];
				m_Sent[i+1] = 9005;
				m_SentNum++;
				continue;
			}

			if(m_Sent[i+1]>=CONSCOLORB1 && m_Sent[i+1]<CONSLINETHICKB)
			{
				if(m_Sent[i+1]>=CONSCOLORB)
				{
					CString s = m_strColor[m_Sent[i+1]-CONSCOLORB];
					m_RlineColor[m_tempWhich] = RGB(GetInt(s.Right(2)),GetInt(s.Mid(2,2)),GetInt(s.Left (2)));
				}
				else
				{
					switch(m_Sent[i+1])
					{
					case 9100:
						m_RlineColor[m_tempWhich] = RGB(255,0,0);
						break;
					case 9101:
						m_RlineColor[m_tempWhich] = RGB(0,255,0);
						break;
					case 9102:
						m_RlineColor[m_tempWhich] = RGB(0,0,255);
						break;
					case 9103:
						m_RlineColor[m_tempWhich] = RGB(255,255,0);
						break;
					case 9104:
						m_RlineColor[m_tempWhich] = RGB(255,100,0);
						break;
					case 9105:
						m_RlineColor[m_tempWhich] = RGB(255,0,255);
						break;
					case 9106:
						m_RlineColor[m_tempWhich] = RGB(0,0,0);
						break;
					case 9107:
						m_RlineColor[m_tempWhich] = RGB(255,255,255);
						break;
					case 9108:
						m_RlineColor[m_tempWhich] = RGB(255,0,255);
						break;
					case 9109:
						m_RlineColor[m_tempWhich] = RGB(0,255,255);
						break;
//	m_WordTable[_T("colormagenta")]  =(void *)9108;//pink
//	m_WordTable[_T("colorcyan")]  =(void *)9109;
					}
				}
/*	m_WordTable[_T("colorred")]  =(void *)9100;
	m_WordTable[_T("colorgreen")]  =(void *)9101;
	m_WordTable[_T("colorblue")]  =(void *)9102;
	m_WordTable[_T("coloryellow")]  =(void *)9103;
	m_WordTable[_T("colororange")]  =(void *)9104;
	m_WordTable[_T("colorpurple")]  =(void *)9105;
	m_WordTable[_T("colorblack")]  =(void *)9106;
	m_WordTable[_T("colorwhite")]  =(void *)9107;
*/			}
			else if(m_Sent[i+1]>=CONSLINETHICKB)
			{
				if(m_Sent[i+1]!= CONSLINEDOT)
					m_RlineThick[m_tempWhich] = m_Sent[i+1]-CONSLINETHICKB;//+1;	// huhe * (12)增加LIANETHICK0函数
				else
					m_RlineThick[m_tempWhich]+= 1;
			}
 			else
			{
					int iType = m_Sent[i+1]-CONSB+1;

// 					if ( iType == 7 || iType == 8 )
// 					{
// 						iType += 100;						
// 					}
					
					if ( iType == 15 || iType == 16 )
					{
						// nodraw, notitle
						m_RlineTypeEx[m_tempWhich] = iType;
					}	
					else
					{
						m_RlineType[m_tempWhich] = iType;				
					}					
			}

			for(int j=i;j<m_SentNum-1;j++)
			{
				m_Sent[j]=m_Sent[j+2];

			}
			m_SentNum-=2;
			m_Sent[m_SentNum+1]=0;

			i--;
	

		}
	}
	return nRtn;
}


void CFormularComputeParent::IsDaPan()
{
//925,937
	for(int i=0;i<m_SentNum;i++)
	{
		if(m_Sent[i]==9525||m_Sent[i]==9537)
		{
			m_isDaPan=1;
		}
	}
}

//lint --e{423}
void CFormularComputeParent::InitEquation(int pmin, int pmax, Kline *pp, CString &str)
{
	m_nCurrentFoot = 0;
	m_nSizeKindPos = 0;
	m_nPosPre = 0;
	m_pKindPos = NULL;

	//str.Remove('\r');
	//str.Remove('\n');
	str.Replace('\r', ' ');
	str.Replace('\n', ' ');

	m_errmsg = _T(" ");
	m_bTotalData=false;
	m_isDaPan=0;
	for(int k = 0;k<NUM_LINE_TOT;k++)
	{
		m_RlineColor[k] = -1;
		m_RlineThick[k] = 1;
		m_RlineType[k]=0;
		m_RlineTypeEx[k]=0;
	}

	m_pData      = pp;
	m_QuoteNum   =0;
	m_floatNum = 0;
	m_intNum   = 0;
	m_VarNum   = 0;
	m_RlineNum   = 0;
	m_MidNum   = 0;
	m_NumGs    = 0;
	m_stringNum = 0;
	m_strColorNum = 0;

	//eliminate the explain
	int b = 0;
	while(true)
	{
		int e = str.Find (_T("{"),b);
		if(e<0)
			break;
		b = e;
		e = str.Find (_T("}"),b);
		if(e<0)
			break;

		//replace "[]" to " "
		for(int i=b;i<=e;i++)
		{
			str.SetAt (i,' ');
		}
		b = e;
	}
	//end
	m_formuCompute   = str;
	m_exec     =0;

	m_MaxPoint = pmax+1;
	m_MinPoint = pmin;
#ifdef TEST_Version1
//	CGmtTime tm = CGmtTime::GetCurrentTime();
//	if(tm>CGmtTime(2004,01,01,9,30,0))
//		m_MaxPoint = pmax;
#endif
	
	//first init
	InitStatic();

	m_pKlineIndex = NULL;
	m_nKindKline = -1;
	m_bCreateBsList = false;
	m_pIndex = NULL;
}
//////////////////////////////////////////////////////////////////////


//sumbars

int CFormularComputeParent::Func847()
{
	int cl,cr,j;
	ARRAY_BE fcl,fcr;
	
	fcl.line = new float[m_MaxPoint];
	fcr.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	cr=m_table.pop_it();
	GetData(cr,fcr);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	float fVal = fcr.line [bl];

	if(be>=bl&&bl>=0&&be>=0)
	{
		m_MidArray[m_MidNum].line[bl] = 0;
		for(j=bl;j<=be;j++)
		{
			float f=0;
			int i =0;
			for( i = j;i>=bl;i--)
			{
				f+=fcl.line[i];
				if(f>=fVal)
				{
					i--;
					break;
				}

			}
			m_MidArray[m_MidNum].line[j] =float( j-i-1 );
		}
	}
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	delete[] fcl.line;
	delete[] fcr.line;
	return 0;
}

//finance
int CFormularComputeParent::Func852()
{
	int cl,ret,j;
	ARRAY_BE fcl;
	
	fcl.line = new float[m_MaxPoint];
	
	cl=m_table.pop_it();
	GetData(cl,fcl);
	ret = int( fcl.line [fcl.b ] );
	if(ret<1)
		ret = 1;
	if(ret>100)
		ret = 1;
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	
	m_MidArray[m_MidNum].line[bl] = 0;
	const BASEINFO* pBase = CFormulaLib::instance()->GetBaseInfo();
	if(pBase==NULL)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		return 999;
	}
	
	//	float* pf = &pBase->zgb;
	float fVal[50] = {pBase->zgb ,pBase->gjg ,pBase->fqrfrg ,pBase->frg ,pBase->Bg,
		pBase->Hg  ,pBase->ltAg,pBase->zgg    ,pBase->zpg ,pBase->zzc*10 ,
		pBase->ldzc*10  ,pBase->gdzc*10,pBase->wxzc*10   ,pBase->cqtz*10,pBase->ldfz*10 ,
		pBase->cqfz*10  ,pBase->zbgjj*10 ,pBase->mggjj*10,pBase->ggqy*10 ,pBase->zyywsr*10,
		pBase->zyywlr*10,pBase->qtywlr*10,pBase->zyywlr*10,0          ,0,
		0            ,0            ,pBase->lrze*10 ,pBase->jlr*10 ,pBase->jlr*10,
		pBase->wfplr*10 ,pBase->mgwfplr ,pBase->mgsy ,pBase->mgjzc ,0,
		pBase->gdqybl ,pBase->jzcsyl };
	for(j=bl;j<=be;j++)	 m_MidArray[m_MidNum].line[j] = fVal[ret-1];     //all
	//	for(j=bl;j<=be;j++)	 m_MidArray[m_MidNum].line[j] = pf[ret-1];  
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	delete[] fcl.line;

	return 0;
}


void CFormularComputeParent::KlineToLine(Kline *pKline, float & f,int nFlag)
{
	switch(nFlag)
	{
	case 0:
		f = pKline->open;
		break;
	case 1:
		f = pKline->high;
		break;
	case 2:
		f = pKline->low;
		break;
	case 3:
		f = pKline->close;
		break;
	case 4:
		f = pKline->vol;
		break;
	case 5:
		f = pKline->amount;
		break;
	case 6:
		f = pKline->advance;
		break;
	case 7:
		f = pKline->decline;
		break;
	case 8:
		f=  *((float*)&(pKline->advance));
		break;
	case 9:
		f =float(  fabsf(fabsf(pKline->vol) - *((float*)&(pKline->decline)))    );
		break;
	}
}

bool CFormularComputeParent::LookupWordTable(CString sKey, int &n)
{
	if(m_WordTable.Lookup(sKey,(void*&)n))	return true;
	else if(m_WordTableNew.Lookup(sKey,(void*&)n))	return true;
	return false;
}

bool CFormularComputeParent::LookupParameterTable(int nKey, int &nValue)
{
	if(m_ParameterTable.Lookup(nKey,nValue))	return true;
	else if(m_ParameterTableNew.Lookup(nKey,nValue))	return true;
	return false;
}

void CFormularComputeParent::InitStatic()
{
	static bool bExec = false;
	if(bExec==true) return;
	//=================运算符===============
	m_WordTable.InitHashTable( 300 );
	m_LetterTable.InitHashTable( 150 );
	m_ParameterTable.InitHashTable( 100 );

	m_WordTable[_T("*")]	=	(void *)6001;
	m_WordTable[_T("/")]	=	(void *)6002;
	m_WordTable[_T("*-")]	=	(void *)6003;
	m_WordTable[_T("/-")]	=	(void *)6004;
	m_WordTable[_T("+")]	=	(void *)5001;
	m_WordTable[_T("-")]	=	(void *)5002;
	m_WordTable[_T(">")]	=	(void *)4001;
	m_WordTable[_T("<")]	=	(void *)4002;
	m_WordTable[_T(">=")]	=	(void *)4005;
	m_WordTable[_T("<=")]	=	(void *)4006;
	m_WordTable[_T("=")]	=	(void *)4008;
	m_WordTable[_T("and")]	=	(void *)3001;
	m_WordTable[_T("or")]	=	(void *)2001;
	m_WordTable[_T(")")]	=	(void *)9002;
	m_WordTable[_T("(")]	=	(void *)9001;
	m_WordTable[_T(",")]	=	(void *)9005;

	m_WordTable[_T(":")]	=	(void *)1001;
	m_WordTable[_T(":=")]	=	(void *)1002;

	m_WordTable[_T("!=")]	=	(void *)4009;
	m_WordTable[_T("><")]	=	(void *)4009;
	m_WordTable[_T("<>")]	=	(void *)4009;
	
	//---		常数
	m_WordTable[_T("volstick")]		=	(void *)9010;
	m_WordTable[_T("stick")]		=	(void *)9011;
	m_WordTable[_T("circledot")]	=	(void *)9012;	
	m_WordTable[_T("linestick")]	=	(void *)9013;
	m_WordTable[_T("crossdot")]		=	(void *)9014;
	m_WordTable[_T("colorstick")]	=	(void *)9015;
	m_WordTable[_T("buycase")]		=	(void *)9016;
	// m_WordTable[_T("sellcase")]		=	(void *)9017;
	//m_WordTable[_T("pointdot")]		=	(void *)9017;
	m_WordTable[_T("pointdot")]		=	(void *)9012;
	m_WordTable[_T("nodraw")]		=	(void *)9024;
	m_WordTable[_T("notitle")]		=	(void *)9025;
	

	m_WordTable[_T("colorred")]		=	(void *)9100;
	m_WordTable[_T("colorgreen")]	=	(void *)9101;
	m_WordTable[_T("colorblue")]	=	(void *)9102;
	m_WordTable[_T("coloryellow")]  =	(void *)9103;
	m_WordTable[_T("colororange")]  =	(void *)9104;
	m_WordTable[_T("colorpurple")]  =	(void *)9105;
	m_WordTable[_T("colorblack")]	=	(void *)9106;
	m_WordTable[_T("colorwhite")]	=	(void *)9107;
	m_WordTable[_T("colormagenta")] =	(void *)9108;//pink
	m_WordTable[_T("colorcyan")]	=	(void *)9109;

	m_WordTable[_T("linethick0")]	=	(void *)9300;			// huhe + (12)增加LIANETHICK0函数
	m_WordTable[_T("linethick1")]	=	(void *)9301;			// huhe *{ 全+1
	m_WordTable[_T("linethick2")]	=	(void *)9302;
	m_WordTable[_T("linethick3")]	=	(void *)9303;
	m_WordTable[_T("linethick4")]	=	(void *)9304;
	m_WordTable[_T("linethick5")]	=	(void *)9305;
	m_WordTable[_T("linethick6")]	=	(void *)9306;
	m_WordTable[_T("linethick7")]	=	(void *)9307;			// huhe *}

	m_WordTable[_T("linedot")]		=	(void *)CONSLINEDOT;

//	m_WordTable[_T("color")]  =(void *)902;
	//=================参数================
	m_WordTable[_T("open")]		=	(void *)9521;
	m_WordTable[_T("close")]	=	(void *)9522;
	m_WordTable[_T("high")]		=	(void *)9523;
	m_WordTable[_T("low")]		=	(void *)9524;
	m_WordTable[_T("o")]		=	(void *)9521;
	m_WordTable[_T("c")]		=	(void *)9522;
	m_WordTable[_T("h")]		=	(void *)9523;
	m_WordTable[_T("l")]		=	(void *)9524;
	m_WordTable[_T("advance")]	=	(void *)9525;
	m_WordTable[_T("weekday")]	=	(void *)9526;
	m_WordTable[_T("day")]		=	(void *)9527;
	m_WordTable[_T("month")]	=	(void *)9528;
	m_WordTable[_T("year")]		=	(void *)9529;
	m_WordTable[_T("hour")]		=	(void *)9530;
	m_WordTable[_T("minute")]	=	(void *)9531;
	m_WordTable[_T("amount")]	=	(void *)9532;
	m_WordTable[_T("vol")]		=	(void *)9533;
	m_WordTable[_T("volume")]	=	(void *)9533;
//	m_WordTable[_T("a")]		=	(void *)9532;
	m_WordTable[_T("v")]		=	(void *)9533;
	m_WordTable[_T("isup")]		=	(void *)9534;
	m_WordTable[_T("isdown")]	=	(void *)9535;
	m_WordTable[_T("isequal")]	=	(void *)9536;
	m_WordTable[_T("decline")]	=	(void *)9537;
	m_WordTable[_T("capital")]	=	(void *)9538;
	m_WordTable[_T("position")]	=	(void *)9540;
//	m_WordTable[_T("ltp")]		=	(void *)9539;

	//new 参数 in vs2.0
	m_WordTable[_T("indexo")]	=	(void *)9541;//大盘K线数据
	m_WordTable[_T("indexc")]	=	(void *)9542;//大盘K线数据
	m_WordTable[_T("indexh")]	=	(void *)9543;//大盘K线数据
	m_WordTable[_T("indexl")]	=	(void *)9544;//大盘K线数据
	m_WordTable[_T("indexv")]	=	(void *)9545;//大盘K线数据
	m_WordTable[_T("indexa")]	=	(void *)9546;//大盘K线数据
	m_WordTable[_T("indexdec")]	=	(void *)9547;//大盘K线数据
	m_WordTable[_T("indexadv")]	=	(void *)9548;//大盘K线数据
	m_WordTable[_T("screenum")]	=	(void *)9549;//大盘K线数据

	m_WordTable[_T("buyvol")]		=	(void *)9550;//换手数据的主动性买量
	m_WordTable[_T("sellvol")]		=	(void *)9551;//换手数据的主动性卖量
	m_WordTable[_T("isbuyorder")]	=	(void *)9552;//是否是买单
	m_WordTable[_T("activebuyvol")] =	(void *)9553;//主动性买量
	m_WordTable[_T("activesellvol")]=	(void *)9554;//主动性卖量
	m_WordTable[_T("time")]			=	(void *)9560;//是否
	m_WordTable[_T("date")]			=	(void *)9561;//是否

	m_WordTable[_T("volunit")]		=	(void *)9580;//每手股数

	m_WordTable[_T("drawnull")]		=	(void *)9581; // 代表无效数值，不应绘制该点
	m_WordTable[_T("currbarscount")]		=	(void *)9582; // 到最后交易日的周期数，最后交易日当天为1
	m_WordTable[_T("period")]		=	(void *)9583; // 当前k线类型，分钟？日？
	m_WordTable[_T("fromopen")]		=	(void *)9584; // 当前时间距离该商品类型的开盘时间, 单位分钟


	// 天鼎新增参数
	m_WordTable[_T("totalcapital")]		=	(void *)9600; // 当前时间距离该商品类型的开盘时间, 单位分钟
	m_WordTable[_T("hyzscode")]		=	(void *)9601; // 当前时间距离该商品类型的开盘时间, 单位分钟

	//================函数==================
	m_WordTable[_T("max")]		=	(void *)8001;
	m_ParameterTable[8001]		=	2;
	m_WordTable[_T("min")]		=	(void *)8004;
	m_ParameterTable[8004]      =	2;
	m_WordTable[_T("mod")]		=	(void *)8005;
	m_ParameterTable[8005]      =	2;
	m_WordTable[_T("reverse")]	=	(void *)8006;
	m_ParameterTable[8006]      =	1;
	m_WordTable[_T("if")]		=	(void *)8007;
	m_ParameterTable[8007]      =	3;

	m_WordTable[_T("abs")]		=	(void *)8002;
	m_ParameterTable[8002]      =	1;
	m_WordTable[_T("ln")]		=	(void *)8008;
	m_ParameterTable[8008]      =	1;
	m_WordTable[_T("not")]		=	(void *)8009;
	m_ParameterTable[8009]      =	1;
	m_WordTable[_T("sgn")]		=	(void *)8010;
	m_ParameterTable[8010]      =	1;

	m_WordTable[_T("exp")]		=	(void *)8003;
	m_ParameterTable[8003]      =	1;
	m_WordTable[_T("log")]		=	(void *)8011;
	m_ParameterTable[8011]      =	1;
	m_WordTable[_T("pow")]		=	(void *)8012;
	m_ParameterTable[8012]      =	2;
	m_WordTable[_T("sqrt")]     =	(void *)8013;
	m_ParameterTable[8013]      =	1;

	m_WordTable[_T("avedev")]	=	(void *)8030;
	m_ParameterTable[8030]      =	2;
	m_WordTable[_T("count")]    =	(void *)8031;
	m_ParameterTable[8031]      =	2;
	m_WordTable[_T("cross")]    =	(void *)8032;
	m_ParameterTable[8032]      =	2;

	m_WordTable[_T("devsq")]    =	(void *)8033;
	m_ParameterTable[8033]      =	2;
	m_WordTable[_T("ema")]		=	(void *)8034;
	m_ParameterTable[8034]      =	2;
	m_WordTable[_T("hhv")]		=	(void *)8035;
	m_ParameterTable[8035]      =	2;

	m_WordTable[_T("llv")]		=	(void *)8036;
	m_ParameterTable[8036]      =	2;
	m_WordTable[_T("sar")]		=	(void *)8037;
	m_ParameterTable[8037]      =	3;
	m_WordTable[_T("sarturn")]  =	(void *)8038;
	m_ParameterTable[8038]		=	3;


	m_WordTable[_T("ref")]		=	(void *)8039;
	m_ParameterTable[8039]      =	2;
	m_WordTable[_T("ma")]		=	(void *)8040;
	m_ParameterTable[8040]      =	2;
	m_WordTable[_T("std")]		=	(void *)8041;
	m_ParameterTable[8041]      =	2;

	m_WordTable[_T("stdp")]     =	(void *)8042;
	m_ParameterTable[8042]      =	2;
	m_WordTable[_T("sum")]      =	(void *)8043;
	m_ParameterTable[8043]      =	2;
	m_WordTable[_T("varp")]     =	(void *)8044;
	m_ParameterTable[8044]      =	2;
	
	
	m_WordTable[_T("var")]      =	(void *)8045;
	m_ParameterTable[8045]      =	2;
	m_WordTable[_T("sma")]      =	(void *)8046;
	m_ParameterTable[8046]      =	3;
	
	//new function in vs2.0
	m_WordTable[_T("sumbars")]  =	(void *)8047;	//
	m_ParameterTable[8047]      =	2;
	
	m_WordTable[_T("bidprice")] =	(void *)8048;//委买价
	m_ParameterTable[8048]      =	1;
	m_WordTable[_T("bidvol")]   =	(void *)8049;//委买量
	m_ParameterTable[8049]      =	1;
	m_WordTable[_T("askprice")] =	(void *)8050;//委卖价
	m_ParameterTable[8050]      =	1;
	m_WordTable[_T("askvol")]   =	(void *)8051;//委卖价
	m_ParameterTable[8051]      =	1;
	
	m_WordTable[_T("finance")]  =	(void *)8052;//财务数据
	m_ParameterTable[8052]      =	1;
	
	//... fangz0924 新加 EXPMA
	m_WordTable[_T("expma")]    =	(void *)8053; 
	m_ParameterTable[8053]		=	2;	


	m_WordTable[_T("dynainfo")] =	(void *)8053;//实时行情数据
	m_ParameterTable[8053]      =	1;

	m_WordTable[_T("hhav")]		=	(void *)8054; // 所有有效数据最大值
	m_ParameterTable[8054]		=	1;			// 参数 数据vol等
	m_WordTable[_T("llav")]		=	(void *)8055; // 所有有效数据最小值
	m_ParameterTable[8055]		=	1;			// 参数 数据vol等
	
	m_WordTable[_T("wma")]		=	(void *)8056; // 所有有效数据最小值
	m_ParameterTable[8056]		=	2;			// 参数 数据vol等

	int nParam[]={  5,5,5,2,1,
					1,1,2,2,2,
					2,3,3,3,3,
					2,2,2,2,3,
					3,1,1,3,3,
					1,1,1,1,1,
					1,2,1,1,3,
					2,2,3,3,2,
					3,3,3,3,1,
					1,3,5,3,1,
					4,2,4,2,5,
					3,3,3,3,1,
					2,1,3,3,3,
					1,1,4,2,3,
					};

	CString sName[]={_T("beeline"),_T("radial"),_T("wirebar"),_T("backset"),_T("barscount"),//HHVBARS
					_T("barslast"),_T("barssince"),_T("dma"),_T("hhvbars"),_T("llvbars"),
					_T("zig2"),_T("peak2"),_T("peakbars2"),_T("trough2"),_T("troughbars2"),
					_T("forcast"),_T("slope"),_T("upward"),_T("downward"),_T("slowupcross"),
					_T("slowdowncross"),_T("floatpercent"),_T("floattype"),_T("valueday"),_T("selfadd"),
					_T("sin"),_T("cos"),_T("tan"),_T("asin"),_T("acos"),
					_T("atan"),_T("backset2"),_T("towervalue"),_T("intpart"),_T("floatcommerce"),
					_T("reflast"),_T("refbegin"),_T("valueaverage"),_T("floatcommercetype"),_T("zig"),
					_T("peak"),_T("peakbars"),_T("trough"),_T("troughbars"),_T("winner"),
					_T("cost"),	_T("drawicon"),_T("drawline"),_T("drawtext"),_T("extdata"),
					_T("deployref"),_T("deploysum"),_T("deploywin"),_T("polyline"),_T("stickline"),
					_T("SetLineWidth"),_T("polylinevalue"),_T("polylinetime"),_T("between"),_T("ceiling"),
					_T("filter"),_T("floor"),_T("longcross"),_T("range"),_T("drawnumber"),
					_T("showh"),_T("showl"),_T("drawband"),_T("refx"),_T("drawtextex")
					};

	int iSize = sizeof(sName) / sizeof(CString);

	for(int j=0; j < iSize; j++)
	{
		m_WordTable[sName[j]] =(void *)(FuncDllB+j);//直线
		m_ParameterTable[FuncDllB+j] = nParam[j];
	}

	m_ParameterTable[8464] = 3;

	m_WordTable[_T("winnertype")] =(void *)(FuncDllB+14);//直线

	// 天鼎新增函数
	int iFuncId = FuncDllB + 250;
	m_WordTable[_T("toprange")]		 =	(void *)iFuncId; // 当前值是近多少周期内的最大值
	m_ParameterTable[iFuncId]        =	1;
	iFuncId = FuncDllB + 251;
	m_WordTable[_T("lowrange")]		 =	(void *)iFuncId; // 当前值是近多少周期内的最小值
	m_ParameterTable[iFuncId]        =	1;
	iFuncId = FuncDllB + 252;
	m_WordTable[_T("findhigh")]		 =	(void *)iFuncId; // N周期前M周期内的第T个最大值
	m_ParameterTable[iFuncId]        =	4;
	iFuncId = FuncDllB + 253;
	m_WordTable[_T("findhighbars")]		 =	(void *)iFuncId; // N周期前M周期内的第T个最大值到当前周期的周期数
	m_ParameterTable[iFuncId]        =	4;
	iFuncId = FuncDllB + 254;
	m_WordTable[_T("findlow")]		 =	(void *)iFuncId; // N周期前M周期内的第T个最小值
	m_ParameterTable[iFuncId]        =	4;
	iFuncId = FuncDllB + 255;
	m_WordTable[_T("findlowbars")]	 =	(void *)iFuncId; // N周期前M周期内的第T个最小值到当前周期的周期数
	m_ParameterTable[iFuncId]        =	4;



	//=================数字================
	m_LetterTable[_T("0")]  =(void *)3;
	m_LetterTable[_T("1")]  =(void *)3;
	m_LetterTable[_T("2")]  =(void *)3;
	m_LetterTable[_T("3")]  =(void *)3;
	m_LetterTable[_T("4")]  =(void *)3;
	m_LetterTable[_T("5")]  =(void *)3;
	m_LetterTable[_T("6")]  =(void *)3;
	m_LetterTable[_T("7")]  =(void *)3;
	m_LetterTable[_T("8")]  =(void *)3;
	m_LetterTable[_T("9")]  =(void *)3;
	m_LetterTable[_T(".")]  =(void *)3;


	m_LetterTable[_T("a")]  =(void *)4;
	m_LetterTable[_T("b")]  =(void *)4;
	m_LetterTable[_T("c")]  =(void *)4;
	m_LetterTable[_T("d")]  =(void *)4;
	m_LetterTable[_T("e")]  =(void *)4;
	m_LetterTable[_T("f")]  =(void *)4;
	m_LetterTable[_T("g")]  =(void *)4;
	m_LetterTable[_T("h")]  =(void *)4;
	m_LetterTable[_T("i")]  =(void *)4;
	m_LetterTable[_T("j")]  =(void *)4;
	m_LetterTable[_T("k")]  =(void *)4;
	m_LetterTable[_T("l")]  =(void *)4;
	m_LetterTable[_T("m")]  =(void *)4;
	m_LetterTable[_T("n")]  =(void *)4;
	m_LetterTable[_T("o")]  =(void *)4;
	m_LetterTable[_T("p")]  =(void *)4;
	m_LetterTable[_T("q")]  =(void *)4;
	m_LetterTable[_T("r")]  =(void *)4;
	m_LetterTable[_T("s")]  =(void *)4;
	m_LetterTable[_T("t")]  =(void *)4;
	m_LetterTable[_T("u")]  =(void *)4;
	m_LetterTable[_T("v")]  =(void *)4;
	m_LetterTable[_T("w")]  =(void *)4;
	m_LetterTable[_T("x")]  =(void *)4;
	m_LetterTable[_T("y")]  =(void *)4;
	m_LetterTable[_T("z")]  =(void *)4;
	m_LetterTable[_T("_")]  =(void *)4;
	m_LetterTable[_T("#")]  =(void *)4;
	m_LetterTable[_T("$")]  =(void *)4;
	m_LetterTable[_T("@")]  =(void *)4;

	m_LetterTable[_T("+")]  =(void *)5;
	m_LetterTable[_T("-")]  =(void *)5;
	m_LetterTable[_T("*")]  =(void *)5;
	m_LetterTable[_T("/")]  =(void *)5;
	m_LetterTable[_T(">")]  =(void *)5;
	m_LetterTable[_T(",")]  =(void *)5;
	m_LetterTable[_T("<")]  =(void *)5;
	m_LetterTable[_T("=")]  =(void *)5;
	m_LetterTable[_T(")")]  =(void *)5;
	m_LetterTable[_T("(")]  =(void *)5;
	m_LetterTable[_T("!")]  =(void *)5;
	m_LetterTable[_T(":")]  =(void *)5;

	m_LetterTable[_T(" ")]  =(void *)2;
	m_LetterTable[_T("\n")] =(void *)2;
	m_LetterTable[_T("\t")] =(void *)2;
	m_LetterTable[_T("\r")] =(void *)2;
	m_LetterTable[_T("{")] =(void *)1;
	m_LetterTable[_T("}")] =(void *)1;

	m_LetterTable[_T("\"")]  =(void *)6;//"标点符号
	m_LetterTable[_T("'")]  =(void *)7;//'标点符号

	bExec=true;
}

int CFormularComputeParent::FuncFromDll(int n)
{
	INPUT_INFO inputInfo;
	int nParam ;
	LookupParameterTable(n, nParam);
	ASSERT(nParam>=0&&nParam<=10);

	inputInfo.klineType   = (klineKind)m_nKindKline;	
	inputInfo.strSymbol   = "";	// ...fangz zhibiao
	inputInfo.nNumData    = m_MaxPoint;
	inputInfo.pData		  = m_pData;

	//
	int cl;
	int i = 0;
	for( i = 0; i < nParam && i < ARRAY_SIZE(inputInfo.fInputParam); i++)
	{
		inputInfo.fInputParam [i].line = new float[m_MaxPoint];
		cl=m_table.pop_it();
		GetData(cl,inputInfo.fInputParam [i]);
	}

	if( nParam < ARRAY_SIZE(inputInfo.fInputParam) )
		inputInfo.fInputParam [nParam].line = NULL;
	inputInfo.fOutputParam .line =new float[m_MaxPoint];

	//do from dll
	int nRtn;
	nRtn = FuncInDll(&inputInfo, n-FuncDllB);
	//end
	
//	m_MidArray[m_MidNum] = inputInfo.fOutputParam  ;//new float[m_MaxPoint];
	m_MidArray[m_MidNum].line = inputInfo.fOutputParam .line ;//new float[m_MaxPoint];
	m_MidArray[m_MidNum] = inputInfo.fOutputParam  ;
/*	m_MidArray[m_MidNum].b =inputInfo.fOutputParam.b; 
	m_MidArray[m_MidNum].e =inputInfo.fOutputParam.e;
*/	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	for(i=0;i<nParam;i++)
		delete [] inputInfo.fInputParam [i].line;
//	if(inputInfo.pDataEx!=NULL) delete []inputInfo.pDataEx;

	return nRtn;
}

struct ValueFoot{
	int nFoot;
	float fValue;
};



int CFormularComputeParent::RelolveNnet(int cl)
{//
	return 1;
}

int CFormularComputeParent::GetBeginFoot(ARRAY_BE &ln2)
{
	int ret =ln2.b;
	if(ret<0) return -1;
	int nFoot = (int)ln2.line[ret];
	ret =(int)ln2.line[ret];
	for(int i=ln2.b;i<=ln2.e;i++)
	{
		if((int)ln2.line[i]>ret) 
			ret = (int)ln2.line[i];
	}
	if(ret == nFoot) return ret;

	if(ret<1) ret = 1;

	return ret;
}

int CFormularComputeParent::GetIndexPoint(CString sName, float &nPoint)
{
	int fl=1;
	for(int i=0;i<m_RlineNum;i++)
	{
		CString s1 = m_RlineName[i];
		s1.MakeLower ();
		CString s2 = sName;
		s2.MakeLower ();
		if(s1==s2)
		{
			fl=0;
			int bl = m_Rline[i].b;
			int be = m_Rline[i].e;
			if(bl>be||bl<0||be<0||be>=m_MaxPoint)
				return 1;

			nPoint = m_Rline[i].line[be];
			break;
		}
	}
	return fl;

}

void CFormularComputeParent::SetVolType(bool bTotalVol)
{
	m_bToatlVol = bTotalVol;
}

bool CFormularComputeParent::IsDayKline(int nKlineType)
{
	return nKlineType>=5 && nKlineType<= 8
		||(nKlineType==MANY_DAY_K_LINE/* fangz zhibiao &&CMainFrame::m_taiShanDoc ->m_propertyInitiate.daysOfManyKline>0*/);
	
}

void CFormularComputeParent::FuncZigPre(INPUT_INFO *pInput, int nFunc)
{
	if(nFunc>20)
	{
		int j=pInput->fInputParam[0].b;
		int f2 = (int)(pInput->fInputParam[0].line [j]);
		if(f2<0)f2 = 0;
		if(f2>3)f2 = 3;
		
		for(int i=j;i<pInput->nNumData ;i++)
		{
			switch(f2)
			{
			case 0:
				pInput->fInputParam[0].line [i] = pInput->pData [i].open ;
				break;
			case 1:
				pInput->fInputParam[0].line [i] = pInput->pData [i].high ;
				break;
			case 2:
				pInput->fInputParam[0].line [i] = pInput->pData [i].low ;
				break;
			case 3:
				pInput->fInputParam[0].line [i] = pInput->pData [i].close ;
				break;
			}
		}
	}
}

int CFormularComputeParent::FuncZigGroup(INPUT_INFO *pInput, void* pArray)
{
	int i,j,/*k,*/l,nMaxFoot,nMinFoot;
	float /*f,*/fTemp,fMax,fMin,fOld1,fOld2;
	float* fp;

	CArray<ValueFoot,ValueFoot&>* pArr=(CArray<ValueFoot,ValueFoot&>*)pArray;

	j=pInput->fInputParam[0].b;
	fp=pInput->fInputParam[0].line ;
	fTemp=pInput->fInputParam[1].line [pInput->fInputParam[1].b]/(float)100.0;
	l=0;//0 is down ; 1 is up
	ValueFoot valueFo;
	valueFo.fValue = fp[j];
	valueFo.nFoot  = j;
	pArr->Add(valueFo);
	fMax = fp[j];
	fMin = fp[j];
	fOld1=fp[j];//the last one
	fOld2=fp[j];//the last second one
	nMaxFoot = j;
	nMinFoot = j;

	//lmb add 20010326
	for(i=j+1;i<pInput->nNumData ;i++)
	{
		//if max > 5%,
		if(fOld1==0||fOld2==0)
			break;
		//peak
		if((fMax-fp[i])/fp[i] >fTemp && fp[i]<fMax && (l==0 || l==2))
		{
			valueFo.fValue = fMax;//fp[i-1];//max
			valueFo.nFoot  = nMaxFoot;
			pArr->Add(valueFo);

			fMax = fp[i];
			fMin = fp[i];
			nMaxFoot = i;
			nMinFoot = i;
			l=1;

		}

		//trough
		if((fp[i]-fMin)/fp[i] >fTemp && fp[i]>fMin&& (l==0 || l==1))
		{
			valueFo.fValue = fMin;//fp[i-1];//max
			valueFo.nFoot  = nMinFoot;
			pArr->Add(valueFo);

			fMax = fp[i];
			fMin = fp[i];
			nMaxFoot = i;
			nMinFoot = i;
			l=2;

		}
		if(fp[i]>fMax)
		{
			nMaxFoot = i;
			fMax= fp[i];
		}
		if(fp[i]<fMin)
		{
			nMinFoot = i;
			fMin= fp[i];
		}

	}

	//the last to add
	if(pArr->GetSize ()-1>=0)
	{
		valueFo=pArr->GetAt(pArr->GetSize ()-1);
		if(valueFo.nFoot <pInput->nNumData-1)
		{
			valueFo.fValue = fp[pInput->nNumData-1];
			valueFo.nFoot =pInput->nNumData-1;
			pArr->Add(valueFo);
		}
	}

	pInput->fOutputParam.b=j;
	return 0;
}

int CFormularComputeParent::RegularTime(int iTime,bool bDay)
{
	int nRate = 1;//according the kind of kline to compute the foot of kline
	int nBias = 0;

	if(bDay == true)
	{
		nRate = 24*60*60;
		nBias = 8*60*60;
	}
	
	return (iTime+nBias)/nRate;
}

int CFormularComputeParent::LookTwoPath(time_t& tmt ,Kline *pKline, int nMax, bool &bAdd,bool bDay)//look up using 2 path
{
	int l,u,mid;
	l=0;
	bAdd=false;
	u=nMax-1;
	int keyVlu = (int)tmt;
	keyVlu = RegularTime(keyVlu,bDay);
	
	if(keyVlu<RegularTime(pKline[0].day,bDay))
	{
		bAdd=true;
		return 0;
	}
	if(keyVlu>RegularTime(pKline[nMax-1].day,bDay))
	{
		bAdd = true;
		//		ASSERT(FALSE);
		return -1;//over the end foot of the kline
	}
	
	do{
		mid=(l+u)/2;
		if(keyVlu<RegularTime(pKline[mid].day,bDay))
			u=mid;
		else if(keyVlu>RegularTime(pKline[mid].day,bDay))
			l=mid;
		else
			break;
	}while(l<u-1);//if l+1==u ,break;
	
	if(keyVlu!=RegularTime(pKline[mid].day,bDay))
	{
		if(u-l==1)
		{
			if(RegularTime(pKline[l].day,bDay)<keyVlu)
			{
				if(RegularTime(pKline[u].day,bDay)>keyVlu)
				{
					mid=l+1;
					bAdd=true;
				}
				else if(RegularTime(pKline[u].day,bDay)== keyVlu)
				{
					mid=l+1;
				}
				else
					ASSERT(FALSE);
			}
			else if(RegularTime(pKline[l].day,bDay)==keyVlu)
			{
				mid=l;
			}
			else
				ASSERT(FALSE);
		}
		else
			ASSERT(FALSE);
	}
	
	return mid;
}

int CFormularComputeParent::GetFoot(INPUT_INFO *pInput, int nParam)
{
	int i=(int)(pInput->fInputParam[nParam].line [pInput->fInputParam[nParam].b]+0.5);
	if(i<198001||i>203701)
		return -1;
	int j=(int)pInput->fInputParam[nParam+1].line [pInput->fInputParam[nParam+1].b];
	if(j/10000>31||j%100>=60||j/100%100>24||j<0)
		return -1;
	
	CTime tm_(i/100,i%100,j/10000,j/100%100,j%100,0);
	time_t tmt = tm_.GetTime ();
	bool bAdd;
	bool bDay=true;
	if(pInput->klineType <DAY_KLINE || pInput->klineType==HS_KLINE || pInput->klineType==MINUTE_1_K_LINE/* fangz zhibiao */)
		bDay = false;
	
	int nRtn = LookTwoPath(tmt ,pInput->pData , pInput->nNumData , bAdd,bDay);
	
	if(nRtn==-1&&bAdd==true)
		nRtn = 0;
	return nRtn;
	
}

int CFormularComputeParent::FuncInDll(INPUT_INFO *pInput, int nFunc)
{
	//lint --e{438}
	int i = 0,j = 0,k = 0,l = 0,m = 0;
	int nRtn=0;
	float f = 0.0f,fTemp= 0.0f;
	if(pInput->nNumData <=0)
		return 800;
	int nParam ;
	LookupParameterTable(nFunc+FuncDllB, nParam);
	ASSERT(nParam>=0&&nParam<=10);
	j = 0;
	for(i = 0;i<nParam;i++)
	{
		if(j<pInput->fInputParam[i].b)
			j=pInput->fInputParam[i].b;
		if(pInput->fInputParam[i].b<0 ||pInput->fInputParam[i].b>pInput->fInputParam[i].e)
		{
//			ASSERT(FALSE);
			nRtn = 800;
			return nRtn;
		}
	}

	switch(nFunc)
	{
	case 0://beeline
	case 1://radial
	case 2://wirebar
		//GetFoot(INPUT_INFO *pInput, int nParam)
		f=pInput->fInputParam[4].line [pInput->fInputParam[4].b];
		k=GetFoot(pInput,0);
		l=k+(int)pInput->fInputParam[3].line [pInput->fInputParam[3].b];//GetFoot(pInput,3);
		j=max(pInput->fInputParam[4].b,pInput->fInputParam[2].b);
		if(nFunc>1)
			j=max(j,k);
		if(l!=k)
			for(i=j;i<pInput->nNumData ;i++)
			{
				pInput->fOutputParam .line [i] = (f-pInput->fInputParam[2].line [pInput->fInputParam[2].b])
					*(i-l)/(l-k)+f;
			}
		else
			for(i=j;i<pInput->nNumData ;i++)
			{
				pInput->fOutputParam .line [i]=f;
			}
		pInput->fOutputParam.b=j;
		pInput->fOutputParam.e=pInput->nNumData-1;
		break;
	case 3://BACKSET
		k=(int)pInput->fInputParam[1].line [pInput->fInputParam[1].b];
		ASSERT(k>0);
		if(k<0)
			break;
		j=pInput->fInputParam[0].b;
		j+=(k);

		memset(pInput->fOutputParam.line ,0,pInput->nNumData*4);
		for(i=j;i<pInput->nNumData ;i++)
		{
			if(pInput->fInputParam[0].line[i]!=0)
			{
				for(l=0;l<k;l++)
					pInput->fOutputParam .line [i-l] =1;
			}
		} 
		pInput->fOutputParam.b=j;
		pInput->fOutputParam.e=pInput->nNumData-1;
		break;
	case 4://BARSCOUNT
		j=pInput->fInputParam[0].b;
		k=pInput->nNumData-j;
		for(i=j;i<pInput->nNumData ;i++)
		{
			pInput->fOutputParam .line [i] =i-j;
		}
		pInput->fOutputParam.b=j;
		pInput->fOutputParam.e=pInput->nNumData-1;
		break;
		/*
BACKSET 	将当前位置到若干周期前的数据设为1。
用法:
BACKSET(X,N),若X非0,则将当前位置到N周期前的数值设为1。
例如：BACKSET(CLOSE>OPEN,2)若收阳则将该周期及前一周期数值设为1,否则为0

BARSCOUNT		求总的周期数。
用法:
BARSCOUNT(X)第一个有效数据到当前的天数
例如：BARSCOUNT(CLOSE)对于日线数据取得上市以来总交易日数，对于分笔成交取得当日成交笔数，对于1分钟线取得当日交易分钟数
*/
	case 5://BARSLAST
		j=pInput->fInputParam[0].b;
		k=j;
		l=j;
		for(i=j;i<pInput->nNumData ;i++)
		{
			if(pInput->fInputParam[0].line [i]!=0)
			{
				if(j==k)
					l=i;
				k=i;
			}
			pInput->fOutputParam .line [i] =i-k;
		}
		pInput->fOutputParam.b=j;
		pInput->fOutputParam.e=pInput->nNumData-1;
		break;
/*BARSLAST		上一次条件成立到当前的周期数。
用法:
BARSLAST(X):上一次X不为0到现在的天数
例如：BARSLAST(CLOSE/REF(CLOSE,1)>=1.1)表示上一个涨停板到当前的周期数

BARSSINCE		第一个条件成立到当前的周期数。
用法:
BARSSINCE(X):第一次X不为0到现在的天数
例如：BARSSINCE(HIGH>10)表示股价超过10元时到当前的周期数
*/
	case 6://BARSSINCE
		j=pInput->fInputParam[0].b;
		k=j;
		l=j;
		for(i=j;i<pInput->nNumData ;i++)
		{
			pInput->fOutputParam .line [i] =i-k;
			if(pInput->fInputParam[0].line[i]!=0)
			{
				if(j==k)
				{
					l=i;
					k=i;
				}
			}
		}
		pInput->fOutputParam.b=l;
		pInput->fOutputParam.e=pInput->nNumData-1;
		break;
	case 7://DMA
		{
			/* OLD
			j=pInput->fInputParam[0].b;
			if(pInput->fInputParam[1].b>j)
			j=pInput->fInputParam[1].b;
			f=pInput->fInputParam[1].line [pInput->fInputParam[1].b];
			j++;
			pInput->fOutputParam .line [j-1] = pInput->fInputParam[0].line [j];
			for(i=j;i<pInput->nNumData ;i++)
			{
			f=pInput->fInputParam[1].line [i];
			
			  if(f>1)
			  {
			  f = 1;
			  // ASSERT(FALSE);
			  }
			  pInput->fOutputParam .line [i] =f*pInput->fInputParam[0].line [i]+(1-f)*pInput->fOutputParam .line [i-1];
			  }
			  pInput->fOutputParam.b=j;
			  pInput->fOutputParam.e=pInput->nNumData-1;
			*/

			// fangz2011-04-11 Now:
			for ( i = j; i < pInput->nNumData; i++ )
			{
				if ( i == j )
				{
					// 第一个, 直接赋值:
					pInput->fOutputParam.line[i] = pInput->fInputParam[0].line[i];
					continue;
				}

				// 系数不能大于1
				f = pInput->fInputParam[1].line[i];
				
				if( f > 1 )
				{
					f = 1;
				}

				//
				pInput->fOutputParam.line [i] = f * pInput->fInputParam[0].line[i] + (1 - f) * pInput->fOutputParam.line [i-1];
			}

			pInput->fOutputParam.b = j;
			pInput->fOutputParam.e = pInput->nNumData-1;
		}		
		break;
/*DMA	求动态移动平均。
用法:
DMA(X,A),求X的动态移动平均。
算法: 若Y=DMA(X,A)
则 Y=A*X+(1-A)*Y',其中Y'表示上一周期Y值,A必须小于1。
例如：DMA(CLOSE,VOL/CAPITAL)表示求以换手率作平滑因子的平均价

HHVBARS		求上一高点到当前的周期数。
用法:
HHVBARS(X,N):求N周期内X最高值到当前周期数，N=0表示从第一个有效值开始统计
例如：HHVBARS(HIGH,0)求得历史新高到到当前的周期数

LLVBARS		求上一低点到当前的周期数。
用法:
LLVBARS(X,N):求N周期内X最低值到当前周期数，N=0表示从第一个有效值开始统计
例如：LLVBARS(HIGH,20)求得20日最低点到当前的周期数

*/
	case 8://HHVBARS
	case 9://LLVBARS
		j=pInput->fInputParam[0].b;
		k=(int)pInput->fInputParam[1].line [pInput->fInputParam[1].b];
		if(k<0)
			break;
		if(k==0)
		{
			l=j;//max foot
			f=pInput->fInputParam[0].line [j];//max or min
			for(i=j;i<pInput->nNumData ;i++)
			{
				if(pInput->fInputParam[0].line [i]>f&& nFunc==8
					||pInput->fInputParam[0].line [i]<f&& nFunc==9)
				{
					f=pInput->fInputParam[0].line [i];
					l=i;
				}
				pInput->fOutputParam .line [i] = i-l;
			}
		}
		else
		{
			j+=(k-1);
			for(int ii=j;ii<pInput->nNumData ;ii++)
			{
				l=ii;//max foot
				f=pInput->fInputParam[0].line [ii];//max
				for(i=ii-k+1;i<=ii ;i++)
				{
					if(pInput->fInputParam[0].line [i]>f&& nFunc==8
						||pInput->fInputParam[0].line [i]<f&& nFunc==9)
					{
						f=pInput->fInputParam[0].line [i];
						l=i;
					}
				}
				pInput->fOutputParam .line [ii] = ii-l;
			}
		}
		pInput->fOutputParam.b=j;
		pInput->fOutputParam.e=pInput->nNumData-1;
		break;
	case 10://ZIG2
	case 39://ZIG
/*ZIG		之字转向。
用法:
ZIG(K,N),当价格变化量超过N%时转向,K表示0:开盘价,1:最高价,2:最低价,3:收盘价
例如：ZIG(3,5)表示收盘价的5%的ZIG转向
*/
		{
			CArray<ValueFoot,ValueFoot&> fArray;
			FuncZigPre(pInput, nFunc);
			FuncZigGroup(pInput, &fArray);
		//to compute mid value
			for(i=1;i<fArray.GetSize();i++)
			{
				j=fArray[i-1].nFoot ;
				k=fArray[i].nFoot ;
				if(j==k)
				{
					pInput->fOutputParam .line [j]=fArray[i].fValue ;
					continue;
				}
				for(int ii=j;ii<=k;ii++)
				{
					pInput->fOutputParam .line [ii]=(fArray[i].fValue -fArray[i-1].fValue )
						*(ii-j)/(k-j)+fArray[i-1].fValue ;
				}
			}
		//end
		}
		break;
/*TROUGH	前M个ZIG转向波谷值。
用法:
TROUGH(K,N,M)表示之字转向ZIG(K,N)的前M个波谷的数值,M必须大于等于1
例如：TROUGH(2,5,2)表示%5最低价ZIG转向的前2个波谷的数值

TROUGHBARS	前M个ZIG转向波谷到当前距离。
用法:
TROUGHBARS(K,N,M)表示之字转向ZIG(K,N)的前M个波谷到当前的周期数,M必须大于等于1
例如：TROUGH(2,5,2)表示%5最低价ZIG转向的前2个波谷到当前的周期数


PEAK	前M个ZIG转向波峰值。
用法:
PEAK(K,N,M)表示之字转向ZIG(K,N)的前M个波峰的数值,M必须大于等于1
例如：PEAK(1,5,1)表示%5最高价ZIG转向的上一个波峰的数值

PEAKBARS	前M个ZIG转向波峰到当前距离。
用法:
PEAKBARS(K,N,M)表示之字转向ZIG(K,N)的前M个波峰到当前的周期数,M必须大于等于1
例如：PEAK(0,5,1)表示%5开盘价ZIG转向的上一个波峰到当前的周期数

*/
	case 11://PEAK2
	case 12://PEAKBARS2
	case 13://TROUGH2
	case 14://TROUGHBARS2
	case 40://PEAK
	case 41://PEAKBARS
	case 42://TROUGH
	case 43://TROUGHBARS
		{
			CArray<ValueFoot,ValueFoot&> fArr;
			FuncZigPre(pInput, nFunc);
			FuncZigGroup(pInput, &fArr);

// 			{
// 				CAutoFile File("c:\\fArr.txt");
// 				FILE* p = File.GetFP();
// 				if ( NULL != p )
// 				{
// 					for ( int i = 0; i < fArr.GetSize(); i++ )
// 					{
// 						fprintf(p, "%d %.2f\n", fArr.GetAt(i).nFoot, fArr.GetAt(i).fValue);
// 					}					
// 				}
// 			}
			
			if(fArr.GetSize ()==0)
				break;
			j=pInput->fOutputParam .b;
			m=(int)pInput->fInputParam [2].line [pInput->fInputParam [2].b];//m is count
			if(m<1)
				break;
			if(j<0||j>=pInput->nNumData) break;

			float* fp=pInput->fOutputParam.line;
			k=0;
			l=j;//begin point

			CArray<ValueFoot,ValueFoot&> fArray;
//			ValueFoot valueFo;
			fTemp=fArr[0].fValue ;

			//add to array
			for(i=1;i<fArr.GetSize ();i++)
			{
				if(nFunc==11||nFunc==12||nFunc==40)
				{
					if(fArr[i].fValue <fArr[i-1].fValue )
					{
						fArray.Add ( fArr[i-1]);
					}
				}
				else if(fArr[i].fValue > fArr[i-1].fValue )
						fArray.Add ( fArr[i-1]);
			}
			if(fArray.GetSize ()<1 || fArr.GetSize ()<1)
				break;
			if(fArr[fArr.GetSize ()-1].nFoot > fArray[fArray.GetSize ()-1].nFoot )
				fArray.Add(fArr[fArr.GetSize ()-1]);
			
			if(fArray.GetSize ()<m)
				break;
			l=fArray[m-1].nFoot+1 ;

// 			{
// 				CAutoFile File("c:\\fArray.txt");
// 				FILE* p = File.GetFP();
// 				if ( NULL != p )
// 				{
// 					for ( int i = 0; i < fArray.GetSize(); i++ )
// 					{
// 						fprintf(p, "%d %.2f\n", fArray.GetAt(i).nFoot, fArray.GetAt(i).fValue);
// 					}					
// 				}
// 			}

			for(i=m-1;i<fArray.GetSize ()-1;i++)
			{
				for(j=fArray[i].nFoot;j<=fArray[i+1].nFoot ;j++)
				{
					if(nFunc==11||nFunc==13||nFunc==40||nFunc==42)
						fp[j]=fArray[i-m+1].fValue;
					else
						fp[j]=j-fArray[i-m+1].nFoot;
				}
			}
// 
// 			{
// 				CAutoFile File2("c:\\fp.txt");
// 				FILE* p = File2.GetFP();
// 				if ( NULL != p )
// 				{
// 					for ( int i = 0; i < pInput->nNumData; i++ )
// 					{
// 						fprintf(p, "%d %.2f\n", i, fp[i]);
// 					}					
// 				}
// 			}
		}
		pInput->fOutputParam .b=l;
		break;
	case 15://FORCAST
	case 16://SLOPE
		{
			j=pInput->fInputParam[0].b;
			k=(int)pInput->fInputParam[1].line[pInput->fInputParam[1].b];
			if(k<2)
				break;
			j=j+k-1;
	/*根据一元回归分析的最小二乘法求N根日K线的线性回归。
	Yi=K0+K1*Xi    I=1，…n,
	K0=Y-K1*X
	K1=(Σ(Xi-X)Yi/ Σ(Xi-X) (Xi-X)
	其中: X=(1/N) Σxi
		 Y=(1/N) ΣYi
		 N=1,   N
	*/
			//--		计算平均值
			for(m=j;m<pInput->nNumData;m++)
			{
				float x=0;// X=(1/N) Σxi
				float y=0;// Y=(1/N) ΣYi
				float sumTop=0;
				float sumBottom=0;
				float k0=0;
				float k1=0;

				for(i=m-k+1;i<=m;i++)
				{
					x+=i;
					y+=pInput->fInputParam[0].line[i];
				}

				x=x/(k);
				y=y/(k);
				for(i=m-k+1;i<=m;i++)
				{
					sumTop+=(i-x)*pInput->fInputParam[0].line[i];
					sumBottom+=(i-x)*(i-x);
				}
				if(sumBottom!=0)
					k1=sumTop/sumBottom;
				else k1=1;
				k0=y-k1*x;

				if(nFunc==15)
					pInput->fOutputParam.line[m]=m*k1+k0;
				else
					pInput->fOutputParam.line[m]=k1;

			}
		}
		pInput->fOutputParam.b=j;
		break;
/*	UPWARD		连日上升。
                   	用法:
UPWARD (P，D),P指标或行情数据，D为天数，返回是否一直上升。
	DOWNWARD		连日下降。
                   	用法:
DOWNWARD (P，D),P指标或行情数据，D为天数，返回是否一直下降。

*/
	case 17://UPWARD
	case 18://DOWNWARD
		j=pInput->fInputParam[1].b;
		k=(int)pInput->fInputParam[1].line[j];//周期数
		if(pInput->fInputParam[0].b>j)
			j=pInput->fInputParam[0].b;
		l=j+k;
		for(i=l;i<pInput->nNumData ;i++)
		{
			float isOk = 1;
			for(int ii=0;ii<k;ii++)
			{
				if(nFunc ==17)
				{
					if(pInput->fInputParam[0].line[i-ii]<pInput->fInputParam[0].line[i-ii-1])
					{
						isOk = 0;
						break;
					}
				}
				else
				{
					if(pInput->fInputParam[0].line[i-ii]>pInput->fInputParam[0].line[i-ii-1])
					{
						isOk = 0;
						break;
					}
				}
			}
			pInput->fOutputParam .line [i] =(float)isOk;
		}
		pInput->fOutputParam.b=l;
		pInput->fOutputParam.e=pInput->nNumData-1;
		break;
/*	  SLOWUPCROSS	维持几周期后上穿。
                   		用法:
SLOWUPCROSS (D, P1，P2),D维持几天没有上穿，P1指标或行情数据，P2指标或行情数据，返回是否成功。D等于1相当于CROSS(P1,P2)。

	  SLOWDOWNCROSS		维持几周期后下穿。
                   			用法:
SLOWDOWNCROSS (D, P1，P2),D维持几天没有下穿，P1指标或行情数据，P2指标或行情数据，返回是否成功。
		
*/
	case 19://SLOWUPCROSS
	case 20://SLOWDOWNCROSS
		j=pInput->fInputParam[0].b;
		k=(int)pInput->fInputParam[0].line[j];//周期数
		if(pInput->fInputParam[1].b>j)
			j=pInput->fInputParam[1].b;
		if(pInput->fInputParam[2].b>j)
			j=pInput->fInputParam[2].b;
		l=j+k;
		for(i=l;i<pInput->nNumData ;i++)
		{
			float isOk = 1;
			if(nFunc ==19)
			{
				for(int ii=1;ii<k+1;ii++)
				{
					if(pInput->fInputParam[1].line[i-ii]>=pInput->fInputParam[2].line[i-ii])
					{
						isOk = 0;
						break;
					}
				}
				if(pInput->fInputParam[1].line[i]<pInput->fInputParam[2].line[i])
					isOk = 0;
			}
			else
			{
				for(int ii=1;ii<k+1;ii++)
				{
					if(pInput->fInputParam[1].line[i-ii]<=pInput->fInputParam[2].line[i-ii])
					{
						isOk = 0;
						break;
					}
				}
				if(pInput->fInputParam[1].line[i]>pInput->fInputParam[2].line[i])
					isOk = 0;
			}

			pInput->fOutputParam .line [i] =(float)isOk;
		}
		pInput->fOutputParam.b=l;
		pInput->fOutputParam.e=pInput->nNumData-1;
		break;
/*	    FLOATPERCENT		流通盘分布比例函数
                   			用法:
    FLOATPERCENT (P),分布在P(指标或行情数据)以上的所有价位的成交量的和占总流通盘的百分比。
     FLOATPERCENT (P)- FLOATPERCENT (Q)为P与Q之间的百分比。
		  FLOATTAPE	流通盘分布类函数
                   		用法:
FLOATTAPE (N),N=1为最高峰处的价格。
N=2为最高峰处的成交量密度（手/分）。
N=3为平均成本处的价格。
N=4为次高峰处的价格。
N=5为次高峰处的成交量密度（手/分）。

*/
	case 21://FLOATPERCENT
	case 22://FLOATTAPE
	case 45://cost	
	case 44://winner
		if (m_pExtraInfo)
		{
			m_SpecialTechIndexManager.WinnerFun(pInput, m_pExtraInfo->pMerchNode, nFunc);
		}
		break;
	case 23://VALUEDAY 0
		{
		k=1;
//		m_bTotalData = TRUE;
		j=pInput->fInputParam[0].b;
		if(j<pInput->fInputParam[1].b)
			j=pInput->fInputParam[1].b;
		if(j<pInput->fInputParam[2].b)
			j=pInput->fInputParam[2].b;
		if(IsDayKline(pInput->klineType))
			k=0;
		f=0;
		for(i=j;i<pInput->nNumData ;i++)
		{
			f=pInput->fInputParam[2].line[i];
			CString s=CTime(pInput->pData [i].day ).Format (L"%Y%m%d");
			
			std::string sA;
			Unicode2MultiChar(CP_ACP, s, sA);			
			int nDay= atoi(sA.c_str());

			if(k==0)
			{
				if(pInput->fInputParam[0].line[pInput->fInputParam[0].b]<19700101)
				{
					if(nDay>= pInput->fInputParam[0].line[pInput->fInputParam[0].b]*100+pInput->fInputParam[1].line[pInput->fInputParam[1].b]/10000)
						break;
				}
				else if(nDay>= pInput->fInputParam[0].line[pInput->fInputParam[0].b])
					break;
			}
			else
			{
				CString s2=CTime(pInput->pData [i].day ).Format (L"%H%M");

				std::string s2A;
				Unicode2MultiChar(CP_ACP, s2, sA);					
				int nHour= atoi(sA.c_str());

				if(pInput->fInputParam[0].line[pInput->fInputParam[0].b]<19700101)
				{
					if(nDay>= pInput->fInputParam[0].line[pInput->fInputParam[0].b]*100+pInput->fInputParam[1].line[pInput->fInputParam[1].b]/10000
						&&nHour>= (int)(pInput->fInputParam[1].line[pInput->fInputParam[1].b])%10000)
						break;
				}
				else if(nDay>= pInput->fInputParam[0].line[pInput->fInputParam[0].b]
					&& nHour>= pInput->fInputParam[1].line[pInput->fInputParam[1].b])
					break;
			}

		}
		for(i=j;i<pInput->nNumData ;i++)
			pInput->fOutputParam.line [i]=f;

		pInput->fOutputParam.b=j;
		pInput->fOutputParam.e=pInput->nNumData-1;
		}
		break;
/*
	6、时间函数
VALUEDAY	取得该周期的日期。
用法:
VALUEDAY（DAY，HOUR，P）返回DAY年月日HOUR时分的P的值
如：VALUEDAY（19990956，0945，CLOSE）

自累加函数
selfadd	
用法:
selfadd（a，b，isadd）表示 如果isadd 等于0则x[i] = a+b*x[i-1],如果isadd 等于0 则x[i]=x[i-1]
如：selfadd（0，close，1）	*/	
	case 24://selfadd
		j=pInput->fInputParam[0].b;
		if(j<pInput->fInputParam[1].b)
			j=pInput->fInputParam[1].b;
		if(j<pInput->fInputParam[2].b)
			j=pInput->fInputParam[2].b;
		for(i=j;i<pInput->nNumData ;i++)
		{
			if(pInput->fInputParam[2].line [j] == 2 && i==j) 
			{
				pInput->fOutputParam.line [i] = 100;
				continue;
			}
			if(pInput->fInputParam[2].line [i]>0)
			{
				pInput->fOutputParam.line [i]=
					pInput->fInputParam[0].line [i]+pInput->fInputParam[1].line [i]*(i==j?100:pInput->fOutputParam.line [i-1]);
			}
			else
			{
				pInput->fOutputParam.line [i]=(i==j?100:pInput->fOutputParam.line [i-1]);
			}
		}
		pInput->fOutputParam.b=j;
		pInput->fOutputParam.e=pInput->nNumData-1;
		break;
	case 25://sin
		j=pInput->fInputParam[0].b;
		for(i=j;i<pInput->nNumData ;i++)
			pInput->fOutputParam.line [i] = sin(pInput->fInputParam[0].line [i]);
		pInput->fOutputParam.b=j;
		break;
	case 26://cos
		j=pInput->fInputParam[0].b;
		for(i=j;i<pInput->nNumData ;i++)
			pInput->fOutputParam.line [i] = cos(pInput->fInputParam[0].line [i]);
		pInput->fOutputParam.b=j;
		break;
	case 27://tan
		j=pInput->fInputParam[0].b;
		try
		{
		for(i=j;i<pInput->nNumData ;i++)
			pInput->fOutputParam.line [i] = tan(pInput->fInputParam[0].line [i]);
		}
		catch(...)
		{
		for(i=j;i<pInput->nNumData ;i++)
			pInput->fOutputParam.line [i] = 0;
		}
		pInput->fOutputParam.b=j;
		break;
	case 28://asin
		j=pInput->fInputParam[0].b;
		for(i=j;i<pInput->nNumData ;i++)
		{
			f = pInput->fInputParam[0].line [i];
			if(f>1) f=1;
			if(f<-1)f=-1;
			pInput->fOutputParam.line [i] = asin(f);
		}
		pInput->fOutputParam.b=j;
		break;
	case 29://acos
		j=pInput->fInputParam[0].b;
		for(i=j;i<pInput->nNumData ;i++)
		{
			f = pInput->fInputParam[0].line [i];
			if(f>1) f=1;
			if(f<-1)f=-1;
			pInput->fOutputParam.line [i] = acos(f);
		}
		pInput->fOutputParam.b=j;
		break;
	case 30://atan
		j=pInput->fInputParam[0].b;
		for(i=j;i<pInput->nNumData ;i++)
			pInput->fOutputParam.line [i] = atan(pInput->fInputParam[0].line [i]);
		pInput->fOutputParam.b=j;
		break;
	case 31://BACKSET2
		k=(int)pInput->fInputParam[1].line [pInput->fInputParam[1].b];
//		ASSERT(k>0);
		if(k<0) k=0;
//			break;
		{// get the max begin foot
			for(m = pInput->fInputParam[1].b+1;m<pInput->nNumData ;m++)
			{
				int nn = (int)pInput->fInputParam[1].line [m];
				if(nn>m)
					k = nn;
			}
		}
		j=pInput->fInputParam[0].b;
		if(j<pInput->fInputParam[1].b) j= pInput->fInputParam[1].b;
		j+=(k+1);

		for(i=j;i<pInput->nNumData ;i++)
		{
			k=(int)pInput->fInputParam[1].line [i];
			if(k<0) k=0;
			for(l=-1;l<k;l++)
			{
				pInput->fOutputParam .line [i-l-1] =pInput->fInputParam[0].line[i];
			}
		}
		pInput->fOutputParam.b=j;
		break;
		//towervalue
	case 32://towervalue
		j=pInput->fInputParam[0].b;
		j++;
		if(j<0) j=0;
		if(j>=pInput->nNumData) 
		{
			pInput->fOutputParam.b=-1;
			break;
		}

		k=(int)pInput->fInputParam[0].line [j] ;
		if(k>4 || k<1) 
		{
			pInput->fOutputParam.b=-1;
			break;
		}

		{
			float closeYest = pInput->pData [j-1].close;//m_pkline[0].close;//起步价
			float openYest = pInput->pData [j-1].open;
			float f11[4];
			f11[0] = closeYest,f11[1] = closeYest,f11[2] = closeYest,f11[3] = 1;
			for(i=j;i<pInput->nNumData ;i++)
			{
				float close = pInput->pData [i].close;
				if( openYest <= closeYest )
				{
					if(close>=closeYest)//draw red
					{
						f11[0] = close;
						f11[1] = closeYest;
						f11[2] = close;
						f11[3] = 1;

						openYest = closeYest;
						closeYest = close;
					}
					else
					{
						if(close>=openYest)//draw red
						{
							f11[0] = close;
							f11[1] = closeYest;
							f11[2] = close;
							f11[3] = 1;

							openYest = close;
						}
						else//draw red and green
						{
							f11[0] = closeYest;
							f11[1] = close;
							f11[2] = openYest;
							f11[3] = 3;

							openYest = closeYest;
							closeYest = close;
						}
					}
				}
				else
				{
					if(close<=closeYest)//draw green
					{
						f11[0] = closeYest;
						f11[1] = close;
						f11[2] = close;
						f11[3] = 2;

						openYest = closeYest;
						closeYest = close;
					}
					else
					{
						if(close<=openYest)//draw green
						{
							f11[0] = close;
							f11[1] = closeYest;
							f11[2] = closeYest;
							f11[3] = 2;

							openYest = close;
						}
						else//draw red and green
						{
							f11[0] = close;
							f11[1] = closeYest;
							f11[2] = openYest;
							f11[3] = 3;

							openYest = closeYest;
							closeYest = close;
						}
					}
				}
				pInput->fOutputParam .line [i] = f11[k-1];
			}
		}
		pInput->fOutputParam.b=j;
		break;
	case 33://int
		j=pInput->fInputParam[0].b;
		for(i=j;i<pInput->nNumData ;i++)
			pInput->fOutputParam.line [i] = int(pInput->fInputParam[0].line [i]);
		pInput->fOutputParam.b=j;
		break;
	case 34:
	case 38:		
		{
		/* fangz zhibiao
		k=1;
		while(1)
		{
		if(pInput->pData == NULL)
		{
		k=0;
		break;
		}
		j=pInput->fInputParam[0].b;
		l=j;
		float fLTP;
		float fCJL;
		fLTP=pInput->fInputParam[1].line[0];
		fCJL=pInput->fInputParam[2].line[0];
		float fMax=0, float fMin=90000000;
		CTaiKlineMoveChenben::GetMaxMin(pInput->pData , pInput->nNumData , fMax, fMin);
		float fRuler;
		int n =CTaiKlineMoveChenben::GetRuler( fMax,  fMin, fRuler);
		
		  if(n<=0 )
		  {
		  k=0;
		  break;
		  }
		  CReportData* pdt = NULL;
		  if(!CMainFrame::m_taiShanDoc ->m_sharesInformation .Lookup (pInput->strSymbol ,pdt,pInput->m_stkKind))
		  {
		  k=0;
		  break;
		  }
		  float ltp = CTdxShanKlineShowView::GetCapital(pdt);
		  ltp=ltp*fLTP;
		  if(pdt->id [1] == 'A')
		  ltp = ltp/100;
		  if( pdt->pBaseInfo!=NULL && ltp>0 )
		  {
		  //				k=0;
		  //				break;
		  float* cnp = new float[n];
		  memset(cnp,0,n*4);
		  
			if(nFunc == 34)
			{
			for(i=0;i<pInput->nNumData ;i++)
			{
			CTaiKlineMoveChenben::GetMoveCBPerFoot(pInput->pData+i, cnp, ltp, fMax, fMin, i,fCJL);
			if(i<l)
			{
			continue;
			}
			f=pInput->fInputParam[0].line[i];//
			if(f>fMax )
			{
			f=0;
			}
			else if(f<fMin)
			{
			f=1;
			}
			else
			{
			int nSeprate = (int)(f*fRuler)-(int)(fMin*fRuler);
			f=0;
			for(int ii=0;ii<=nSeprate;ii++)
			{
			f+=cnp[ii];
			}
			f/=ltp;
			f=1-f;
			if(f<0)
			f=0;
			}
			pInput->fOutputParam .line [i] =f;
			
			  }
			  }
			  else //38
			  {
			  int nIn =pInput->fInputParam[0].line[l];//
			  for(i=0;i<pInput->nNumData ;i++)
			  {
			  CTaiKlineMoveChenben::GetMoveCBPerFoot(pInput->pData+i, cnp, ltp, fMax, fMin, i,fCJL);
			  if(i<l)
			  {
			  continue;
			  }
			  f=cnp[0];
			  float amount=0;
			  int nFootMax = 0;
			  float max = 0;
			  int nBeginY=fMin*fRuler;
			  fTemp =cnp[0];
			  for(int ii=0;ii<n;ii++)
			  {
			  amount+=(cnp[ii]*(ii+nBeginY)/(float)fRuler);
			  if(cnp[ii]>max)
			  {
			  nFootMax = ii;
			  max=cnp[ii];
			  }
			  
				}
				switch(nIn)
				{
				case 1:
				f=(nFootMax+nBeginY)/(float)fRuler;
				break;
				case 2:
				f=cnp[nFootMax];
				break;
				case 3:
				f=amount/ltp;
				break;
				default:
				//							ASSERT(FALSE);
				k=0;
				break;
				}
				pInput->fOutputParam .line [i] =f;
				
				  }
				  }
				  
					}
					else
					{
					for(i=0;i<pInput->nNumData ;i++)
					pInput->fOutputParam .line [i] = 0;
					}
					break;
					}
					if(k==0)
					pInput->fOutputParam.b=-1;
					else
					pInput->fOutputParam.b=l;
					pInput->fOutputParam.e=pInput->nNumData-1;
*/
		}
		break;
	case 35://reflast
		j=pInput->fInputParam[0].b;
		if( pInput->fInputParam[1].b>=0 && pInput->fInputParam[1].b<pInput->nNumData &&pInput->nNumData>0)
		{
			int nMove = (int)pInput->fInputParam[1].line [ pInput->fInputParam[1].b];
			if(nMove<0) nMove = 0;
			if(nMove>= pInput->nNumData) nMove = pInput->nNumData-1;
			int nFoot = pInput->nNumData-1-nMove;
			if(nFoot<j) nFoot = j;
			for(i=j;i<pInput->nNumData ;i++)
				pInput->fOutputParam.line [i] = (pInput->fInputParam[0].line [nFoot]);
			pInput->fOutputParam.b=j;
		}
		break;
	case 36://refbegin
		j=pInput->fInputParam[0].b;
		if( pInput->fInputParam[1].b>=0 && pInput->fInputParam[1].b<pInput->nNumData &&pInput->nNumData>0)
		{
			int nMove = (int)pInput->fInputParam[1].line [ pInput->fInputParam[1].b];
			if(nMove<0) nMove = 0;
			if(nMove>= pInput->nNumData) nMove = (pInput->nNumData-1);
			int nFoot = nMove;
			if(nFoot<j) nFoot = j;
			for(i=j;i<pInput->nNumData ;i++)
				pInput->fOutputParam.line [i] = (pInput->fInputParam[0].line [nFoot]);
			pInput->fOutputParam.b=j;
		}
		break;
	case 37://valueaverage
		j=pInput->fInputParam[0].b;
		if(pInput->fInputParam[1].b>j)
			j =pInput->fInputParam[1].b;
		if(pInput->fInputParam[2].b>j)
			j =pInput->fInputParam[2].b;
		if(pInput->nNumData>0)
		{
			//总成交量
			float vol = 0;
			for(i=j;i<pInput->nNumData ;i++)
				vol += (pInput->fInputParam[1].line [i]);
			vol *= pInput->fInputParam[2].line [j];

			//排序
			float* fPrice = new float [pInput->nNumData];
			memcpy(fPrice,pInput->fInputParam[0].line ,sizeof(float)*pInput->nNumData);
			float* fVol = new float [pInput->nNumData];
			memcpy(fVol,pInput->fInputParam[1].line ,sizeof(float)*pInput->nNumData);
			for(i=j;i<pInput->nNumData ;i++)
			{
				for(k=j;k<pInput->nNumData-1-(i-j);k++)
				{
					if(fPrice[k]>fPrice[k+1]) 
					{
						float ff = fPrice[k];
						fPrice[k] = fPrice[k+1];
						fPrice[k+1] = ff;
						ff = fVol[k];
						fVol[k] = fVol[k+1];
						fVol[k+1] = ff;
					}
				}
			}

			//计算
			float fTot = 0;
			for(i=j;i<pInput->nNumData ;i++)
			{
				fTot+=fVol[i];
				if(fTot>=vol)
				{
					fTot = fPrice[i];
					break;
				}
			}
			delete [] fPrice;
			delete [] fVol;
			if(i!=pInput->nNumData)
			{
				for(i=j;i<pInput->nNumData ;i++)
				{
					pInput->fOutputParam.line [i] =fTot;
				}
				pInput->fOutputParam.b=j;
			}
		}
		break;
	case 46://drawicon
	case 48://drawtext
	case 64://drawnumber
	case 69://drawtextex
		j=pInput->fInputParam[0].b;
		if(j<pInput->fInputParam[1].b)
			j=pInput->fInputParam[1].b;
		if(j<pInput->fInputParam[2].b)
			j=pInput->fInputParam[2].b;
		{
			if(nFunc == 46)
			{
				CString s;
				s.Format(L"%d",(int)(pInput->fInputParam[2].line[j]));				
				pInput->fOutputParam.s = s;
			}
			else
				pInput->fOutputParam.s = pInput->fInputParam[2].s;

			for(i=j;i<pInput->nNumData ;i++)
			{
				// 输出参数的值 等于 输入数组第 1 个的值
				pInput->fOutputParam.line [i] = pInput->fInputParam[1].line [i];

				// 输入参数第 0 个. line[i] 就是每个节点. 根据这个节点是否大于 0 判断此节点是否满足条件
				// 例如: 条件是 close > 12 . 而且只有第 12 和 99 个节点是满足条件的. 那么 只有当 i == 12 和 i == 99 的时候
				// 才会满足条件, looseArr 才会 add . 最后画指标的时候, 就是根据 loosearr 这个画的.

  				if(pInput->fInputParam[0].line [i]>0)
				{
 					LooseValue val;
					val.nFoot = i;
					val.fVal = pInput->fInputParam[1].line [i];
					
					if (64 == nFunc || 69 ==nFunc)
					{
						val.StrExtraData.Format(L"%g", pInput->fInputParam[2].line[i]);
					}

					//
					pInput->fOutputParam.looseArr.Add(val);
				}
			}

			//
			if(nFunc == 46)
				pInput->fOutputParam.kind = ARRAY_BE::DrawIcon;
			else if(nFunc == 48)
				pInput->fOutputParam.kind = ARRAY_BE::StringData;
			else if (nFunc == 64)
				pInput->fOutputParam.kind = ARRAY_BE::DrawNum;
			else if (nFunc == 69)
				pInput->fOutputParam.kind = ARRAY_BE::StringDataEx;

			//
			pInput->fOutputParam.b=j;
			
			// fangz zhibiao
			pInput->fOutputParam.s.MakeUpper();
		}
		break;
	case 47://drawline/zig
		j=pInput->fInputParam[0].b;
		if(j<pInput->fInputParam[1].b)
			j=pInput->fInputParam[1].b;
		if(j<pInput->fInputParam[2].b)
			j=pInput->fInputParam[2].b;
		if(j<pInput->fInputParam[3].b)
			j=pInput->fInputParam[3].b;
		if(j<pInput->fInputParam[4].b)
			j=pInput->fInputParam[4].b;
		{
			int np = 0;
			CString s;
			s.Format(L"%f",pInput->fInputParam[4].line[j]);
			pInput->fOutputParam.s = s;

			for(i=j;i<pInput->nNumData ;i++)
			{
				pInput->fOutputParam.line [i] =  max(pInput->fInputParam[1].line [i],pInput->fInputParam[3].line [i]);
				if(np ==0)
				{
					if( pInput->fInputParam[0].line [i]>0 )
						{
							LooseValue val;
							val.nFoot = i;
							val.fVal =(pInput->fInputParam[1].line [i]);
							pInput->fOutputParam.looseArr.Add(val);

							np=1;
						}
				}
				else
				{
					if( pInput->fInputParam[2].line [i]>0 )
						{
							LooseValue val;
							val.nFoot = i;
							val.fVal = pInput->fInputParam[3].line [i];
							pInput->fOutputParam.looseArr.Add(val);

							np=0;
						}
				}

			}
			pInput->fOutputParam.b=j;
			pInput->fOutputParam.kind = ARRAY_BE::DrawLine;
		}
		break;
	case 49://extdata
		{
		/*	fangz zhibiao
			j=pInput->fInputParam[0].b;
			if( pInput->fInputParam[0].line[j]>=0)// && pInput->fInputParam[1].b<pInput->nNumData &&pInput->nNumData>0)
			{
				if(pInput->pDat1 == 0 )
					break;
				CString s = XgnExtDataDlg::FormatStr(m_cdat1);
				
				pInput->fOutputParam.b=j;
				if(!XgnExtDataDlg::GetLineBE(pInput->pData ,pInput->nNumData,pInput->fOutputParam,pInput->fInputParam[0].line[j],pInput->pDat1 ))
					pInput->fOutputParam.b = -1;
			}
		*/
		}		
		break;
	case 50://fanref
		{
			j=pInput->fInputParam[0].b;
			if(pInput->fInputParam[1].b>j)
				j =pInput->fInputParam[1].b;
			if(pInput->fInputParam[2].b>j)
				j =pInput->fInputParam[2].b;
			if(pInput->fInputParam[3].b>j)
				j =pInput->fInputParam[3].b;

			k = (int)pInput->fInputParam[3].line [j];
			if(k<0) break;
			j += k;

			if(m_nCurrentFoot<j) break;
			if(m_nCurrentFoot>=pInput->nNumData) break;
			


			{
				// fangz zhibiao
				// ARRAY_Right_Box::FanRect(pInput->fInputParam[0].line [k2],pInput->fInputParam[1].line [k2],pInput->fInputParam[2].line [k2], pInput->fOutputParam.lineRight);
				// pInput->fOutputParam.b=j;
				// pInput->fOutputParam.kind = ARRAY_BE::LineRt;
			}
		}
		break;
	case 51://fansum
		{
			j=pInput->fInputParam[0].b;
			if(pInput->fInputParam[1].b>j)
				j =pInput->fInputParam[1].b;

			k = (int)pInput->fInputParam[1].line [j];
			if(k<0) break;
			j += k;

			if(m_nCurrentFoot<j) break;
			if(m_nCurrentFoot>=pInput->nNumData) break;

		
			{
				// fangz zhibiao
				// for(i = k2 ;i<=m_nCurrentFoot ;i++)
				// {
				//	ARRAY_Right_Box::FanRect(pInput->fInputParam[0].line [i],
				//		pInput->pData [i].low , pInput->pData [i].high , pInput->fOutputParam.lineRight);
				// }
				// pInput->fOutputParam.b=j;
				// pInput->fOutputParam.kind = ARRAY_BE::LineRt;
			}			
		}
		break;
	case 52://fanwin
		{
			k = (int)pInput->fInputParam[1].line [j];
			if(k<0) break;
			j += k;

			int nPre =(int) pInput->fInputParam[3].line[j];
			int nCurrentFoot = m_nCurrentFoot - nPre;
			if(nCurrentFoot<j) break;
			if(nCurrentFoot>=pInput->nNumData) break;

		
 			{
				// fangz zhibiao
// 				ARRAY_Right_Box::FanRect(f3,
// 					pInput->pData [k2].low , pInput->pData [k2].high , pInput->fOutputParam.lineRight);
// 				for(i = k2+1 ;i<=nCurrentFoot ;i++)
// 				{
// 					//FanAllPrice(float fVal, float fInitTotal ,ARRAY_Right_Box &out)
// 					ARRAY_Right_Box::FanAllPrice(pInput->fInputParam[0].line [i],
// 						f3 ,pInput->fOutputParam.lineRight);
// 					ARRAY_Right_Box::FanRect(pInput->fInputParam[0].line [i],
// 						pInput->pData [i].low , pInput->pData [i].high , pInput->fOutputParam.lineRight);
// 				}
// 				pInput->fOutputParam.b=j;
// 				pInput->fOutputParam.kind = ARRAY_BE::LineRt;
 			}
		}
		break;
	case 53://polyline
		j=pInput->fInputParam[0].b;
		if(pInput->fInputParam[1].b>j)
			j =pInput->fInputParam[1].b;
		{
			for(i=j;i<pInput->nNumData ;i++)
			{
				pInput->fOutputParam.line [i] = (pInput->fInputParam[1].line [i]);
				if(pInput->fInputParam[0].line [i]>0)
				{
					LooseValue val;
					val.nFoot = i;
					val.fVal = pInput->fInputParam[1].line [i];
					pInput->fOutputParam.looseArr.Add(val);
				}
			}
			pInput->fOutputParam.kind = ARRAY_BE::DrawPolyLine;
			pInput->fOutputParam.b=j;
		}
		break;
/*在图形上绘制折线段。
用法:
POLYLINE(COND,PRICE),当COND条件满足时,以PRICE位置为顶点画折线连接。
例如：POLYILINE(HIGH>=HHV(HIGH,20),HIGH)表示在创20天新高点之间画折线。*/
	case 54://stickline
		{
			float width = pInput->fInputParam[3].line [j];
			int empty = (int)pInput->fInputParam[4].line [j];
			empty%=10;
			//CString ss ;
			//ss.Format(L"%.2f %d",width,empty);
			TCHAR bufSS[200];
			_sntprintf(bufSS, sizeof(bufSS)/sizeof(bufSS[0]), _T("%.2f %d"), width, empty);
			
			if(!pInput->fOutputParam.lineWidth)  pInput->fOutputParam.lineWidth = new float [pInput->fInputParam[2].e+1];
			for(i=j;i<pInput->nNumData ;i++)
			{
				pInput->fOutputParam.line [i] = (pInput->fInputParam[2].line [i]);
				pInput->fOutputParam.lineWidth[i] = pInput->fInputParam[3].line [i];
				if(pInput->fInputParam[0].line [i]>0)
				{
					LooseValue val;
					val.nFoot = i;
					val.fVal = pInput->fInputParam[1].line [i];
					pInput->fOutputParam.looseArr.Add(val);
					//TRACE(L"STICKLINE = %d\n",i);
				}
			}
			pInput->fOutputParam.s = bufSS;
			pInput->fOutputParam.kind = ARRAY_BE::DrawStickLine;
			pInput->fOutputParam.b=j;
		}
		break;
/*在图形上绘制柱线。
用法:
STICKLINE(COND,PRICE1,PRICE2,WIDTH,EMPTY),当COND条件满足时,在PRICE1和PRICE2位置之间画柱状线，宽度为WIDTH(10为标准间距),EMPTH不为0则画空心柱。
例如：STICKLINE(CLOSE>OPEN,CLOSE,OPEN,0.8,1)表示画K线中阳线的空心柱体部分。*/
	case 55://SetLineWidth
		{
/*			float width = pInput->fInputParam[1].line [j];
			float isVirt = pInput->fInputParam[2].line [j];
			pInput->fOutputParam = pInput->fInputParam[0];
			
			pInput->fOutputParam.nLineWidth = (int)width;
			pInput->fOutputParam.isVirtualLine  = isVirt>0.01?1:0;
			pInput->fOutputParam.b=j;*/
		}
		break;
	case 56://polylinevalue
		{
			int nFirst = 0;
			float fPre = 0;
			int nTime = (int) pInput->fInputParam[2].line [j];
			if(nTime<=0) break;

			for(i=j;i<pInput->nNumData ;i++)
			{
				pInput->fOutputParam.line [i] = (pInput->fInputParam[1].line [i]);
				if(pInput->fInputParam[0].line [i]>0)
				{
					if(nFirst < nTime)
					{
						nFirst++;
						if(nFirst == nTime)
							j = i+1;
						else 
							continue;
					}
					pInput->fOutputParam.line [i] = fPre;
					fPre =  pInput->fInputParam[1].line [i];
				}
				else
					pInput->fOutputParam.line [i] = fPre;
			}
			if(nFirst < nTime) break;
			pInput->fOutputParam.b=j;
		}
		break;
	case 57://polylinetime
		{
			int nFirst = 0;
			int nPre = 0;
			int nTime = (int) pInput->fInputParam[2].line [j];
			if(nTime<=0) break;

			CArray<int ,int&> intArr;
			for(i=j;i<pInput->nNumData ;i++)
			{
				pInput->fOutputParam.line [i] = (pInput->fInputParam[1].line [i]);
				nPre++;
				if(pInput->fInputParam[0].line [i]>0)
				{
					intArr.Add (i);

					if(nFirst < nTime)
					{
						nFirst++;
						if(nFirst == nTime)
							j = i+1;
						else 
							continue;
					}
					pInput->fOutputParam.line [i] = nPre;
					ASSERT(intArr.GetSize ()>=nTime);
					nPre =  i - intArr[intArr.GetSize () - nTime];
				}
				else
					pInput->fOutputParam.line [i] = nPre;
			}
			if(nFirst < nTime) break;
			pInput->fOutputParam.b=j;
		}
		break;
/*在图形上绘制折线段。
用法:
POLYLINE(COND,PRICE),当COND条件满足时,以PRICE位置为顶点画折线连接。
例如：POLYILINE(HIGH>=HHV(HIGH,20),HIGH)表示在创20天新高点之间画折线。*/
	case 58://between
		{
			for(i=j;i<pInput->nNumData ;i++)
			{
				if(pInput->fInputParam[0].line [i]>= pInput->fInputParam[1].line [i] && pInput->fInputParam[0].line [i]<=pInput->fInputParam[2].line [i]
					||pInput->fInputParam[0].line [i]>= pInput->fInputParam[2].line [i] && pInput->fInputParam[0].line [i]<=pInput->fInputParam[1].line [i])
					pInput->fOutputParam.line [i] = 1;
				else
					pInput->fOutputParam.line [i] = 0;
			}
			pInput->fOutputParam.b=j;
		}
		break;
	case 59://CEILING 向上舍入
		{
			for(i=j;i<pInput->nNumData ;i++)
			{
				pInput->fOutputParam.line [i] =(float) ceil(pInput->fInputParam[0].line [i]);
			}
			pInput->fOutputParam.b=j;
		}
		break;
	case 60://FILTER(X,N):X满足条件后，删除其后N周期内的数据置为0
		{
			int n = (int)pInput->fInputParam[1].line [j];
			if(n<0) break;

			memset(pInput->fOutputParam.line, 0, pInput->nNumData*4);
			for(i=j;i<pInput->nNumData ;i++)
			{
				if(pInput->fInputParam[0].line [i]>0.01)
				{
					pInput->fOutputParam.line [i] = 1;
					i+=n;
				}
				else
					pInput->fOutputParam.line [i] = 0;
			}
			pInput->fOutputParam.b=j;
		}
		break;
	case 61://FLOOR 向下舍入
		{
			for(i=j;i<pInput->nNumData ;i++)
			{
				pInput->fOutputParam.line [i] =(float) floor(pInput->fInputParam[0].line [i]);
			}
			pInput->fOutputParam.b=j;
		}
		break;
	case 62://LONGCROSS(A,B,N)表示A在N周期内都小于B，本周期从下方向上穿过B时返回1，否则返回0
		{
			int n = (int)pInput->fInputParam[1].line [j];
			if(n<1) break;
			for(i=j+n;i<pInput->nNumData ;i++)
			{
				bool b = false;
				for(k=0;k<n;k++)
				{
					if(pInput->fInputParam[0].line [i-n+k]>=pInput->fInputParam[1].line [i-n+k])
					{
						b = true;
						break;
					}
				}
				if(!b)
				{
					if(pInput->fInputParam[0].line [i]>pInput->fInputParam[1].line [i])
						pInput->fOutputParam.line [i] =1;
					else
						pInput->fOutputParam.line [i] =0;
				}
				else
					pInput->fOutputParam.line [i] =0;
			}
			pInput->fOutputParam.b=j;
		}
		break;
	case 63://RANGE(A,B,C)表示A大于B同时小于C时返回1，否则返回0
		{
			for(i=j;i<pInput->nNumData ;i++)
			{
				if(pInput->fInputParam[0].line [i]> pInput->fInputParam[1].line [i] 
					&& pInput->fInputParam[0].line [i]<pInput->fInputParam[2].line [i])
					pInput->fOutputParam.line [i] = 1;
				else
					pInput->fOutputParam.line [i] = 0;
			}
			pInput->fOutputParam.b=j;
		}
		break;
	case 65: // showh
		{
			if (m_iSreenBegin < 0 || m_iSreenEnd < 0 || m_iSreenBegin > m_iSreenEnd || m_iSreenBegin < j || m_iSreenEnd >= pInput->nNumData)
			{
				// ASSERT(0);
			}
			else
			{
				float fTmp = FLT_MIN;
				for (i = m_iSreenBegin; i < m_iSreenEnd; i++)
				{
					float fNow = pInput->fInputParam[0].line[i];
					fTmp = fTmp > fNow ? fTmp : fNow;
				}

				//
				for (i = j; i < pInput->nNumData; i++)
				{
					pInput->fOutputParam.line[i] = fTmp;
				}

				pInput->fOutputParam.s = L"SCREEN_ACCORDING";
			}
		}
		break;
	case 66: // showl
		{			
			if (m_iSreenBegin < 0 || m_iSreenEnd < 0 || m_iSreenBegin > m_iSreenEnd || m_iSreenBegin < j || m_iSreenEnd >= pInput->nNumData)
			{
				// ASSERT(0);
			}
			else
			{
				float fTmp = FLT_MAX;
				for (i = m_iSreenBegin; i < m_iSreenEnd; i++)
				{
					float fNow = pInput->fInputParam[0].line[i];
					fTmp = fTmp < fNow ? fTmp : fNow;
				}
				
				//
				for (i = j; i < pInput->nNumData; i++)
				{
					pInput->fOutputParam.line[i] = fTmp;
				}

				pInput->fOutputParam.s = L"SCREEN_ACCORDING";
			}
		}
 		break;
	case 67: // drawband
		{
			CString StrClr1 = L"";
			CString StrClr2 = L"";
			for ( i = j; i < pInput->nNumData; i++ )
			{
				pInput->fOutputParam.line[i] = pInput->fInputParam[0].line[i];
								
				LooseValue Value;
				Value.nFoot = i;
				Value.fVal	= pInput->fInputParam[2].line[i];
				
				pInput->fOutputParam.looseArr.Add(Value);				
				
				if (StrClr1.GetLength() <= 0)
				{
					StrClr1 = pInput->fInputParam[1].s;
				}

				if (StrClr2.GetLength() <= 0)
				{
					StrClr2 = pInput->fInputParam[3].s;
				}				
			}

			pInput->fOutputParam.kind = ARRAY_BE::DrawBand;
 			pInput->fOutputParam.s.Format(L"%s,%s", StrClr1.GetBuffer(), StrClr2.GetBuffer());
		}
		break;
	case 68: // refx
		{
			int iIndex = 0;

			//
			for(i=j;i<pInput->nNumData ;i++)
			{
				iIndex = i + (int)pInput->fInputParam[1].line [i];

				//
				if (iIndex < 0)
				{
					iIndex = 0;
				}

				if (iIndex >= pInput->nNumData)
				{
					iIndex = pInput->nNumData - 1;
				}

				//
				pInput->fOutputParam.line[i] = pInput->fInputParam[0].line [iIndex];
			}
			pInput->fOutputParam.b=j;
		}
		break;
	case 100:
		{
			j=pInput->fInputParam[0].b;
			if(j<pInput->fInputParam[1].b)
				j=pInput->fInputParam[1].b;
			if(j<pInput->fInputParam[2].b)
				j=pInput->fInputParam[2].b;
			{
				if(nFunc == 46)
				{
					CString s;
					s.Format(L"%d",(int)(pInput->fInputParam[2].line[j]));				
					pInput->fOutputParam.s = s;
				}
				else
					pInput->fOutputParam.s = pInput->fInputParam[2].s;
				
				for(i=j;i<pInput->nNumData ;i++)
				{
					// 输出参数的值 等于 输入数组第 1 个的值
					pInput->fOutputParam.line [i] = (pInput->fInputParam[1].line [i]);
					
					// 输入参数第 0 个. line[i] 就是每个节点. 根据这个节点是否大于 0 判断此节点是否满足条件
					// 例如: 条件是 close > 12 . 而且只有第 12 和 99 个节点是满足条件的. 那么 只有当 i == 12 和 i == 99 的时候
					// 才会满足条件, looseArr 才会 add . 最后画指标的时候, 就是根据 loosearr 这个画的.
					
					if(pInput->fInputParam[0].line [i]>0)
					{
						LooseValue val;
						val.nFoot = i;
						val.fVal = pInput->fInputParam[1].line [i];
						pInput->fOutputParam.looseArr.Add(val);
					}
				}
				if(nFunc == 46)
					pInput->fOutputParam.kind = ARRAY_BE::DrawIcon;
				else
					pInput->fOutputParam.kind = ARRAY_BE::StringData;
				pInput->fOutputParam.b=j;
				
				// fangz zhibiao
				pInput->fOutputParam.s.MakeUpper();
			}
		}
		break;
	case 101: // drawkline(a,b,c,d)
		{
			for ( i = j; i < pInput->nNumData; i++ )
			{
				CString StrParam;
				StrParam.Format(L"%.4f,%.4f,%.4f,%.4f", 
					pInput->fInputParam[0].line[i],
					pInput->fInputParam[1].line[i],
					pInput->fInputParam[2].line[i],
					pInput->fInputParam[3].line[i]);
				
				LooseValue Value;
				Value.nFoot = i;
				Value.fVal	= 0.f;
				Value.StrExtraData = StrParam;
				
				pInput->fOutputParam.looseArr.Add(Value);
				pInput->fOutputParam.kind = ARRAY_BE::DrawKLine;
			}
			pInput->fOutputParam.iKlineDrawType = 0xffff;	// 默认为CIndexDrawer::EIKDSByMainKline
		}
		break;
	case 102: // drawkline2(a,b,c,d,type)
		{
			for ( i = j; i < pInput->nNumData; i++ )
			{
				CString StrParam;
				StrParam.Format(L"%.2f,%.2f,%.2f,%.2f", 
					pInput->fInputParam[0].line[i],
					pInput->fInputParam[1].line[i],
					pInput->fInputParam[2].line[i],
					pInput->fInputParam[3].line[i]);
				
				LooseValue Value;
				Value.nFoot = i;
				Value.fVal	= 0.f;
				Value.StrExtraData = StrParam;
				
				pInput->fOutputParam.looseArr.Add(Value);
				pInput->fOutputParam.kind = ARRAY_BE::DrawKLine;
			}
			pInput->fOutputParam.iKlineDrawType = (int)pInput->fInputParam[4].line[j];	// 取第一个就可以了
		}
		break;
	case 250: // 当前值是近多少周期内的最大值
		m_SpecialTechIndexManager.GetTopRange(pInput);
		break;
	case 251: // 当前值是近多少周期内的最小值
		m_SpecialTechIndexManager.GetLowRange(pInput);
		break;
	case 252: // N周期前M周期内的第T个最大值
		m_SpecialTechIndexManager.GetFindHighOrLow(pInput, true);
		break;
	case 253: // // N周期前M周期内的第T个最大值到当前周期的周期数
		m_SpecialTechIndexManager.GetFindHighOrLowBars(pInput, true);
		break;
	case 254: //  N周期前M周期内的第T个最小值
		m_SpecialTechIndexManager.GetFindHighOrLow(pInput, false);
		break;
	case 255: //  N周期前M周期内的第T个最小值
		m_SpecialTechIndexManager.GetFindHighOrLowBars(pInput, false);
		break;
	default:
		// fangz zhibiao
		// FuncInDllExt(pInput,  nFunc,  j);
		break;
	}
	pInput->fOutputParam.e=pInput->nNumData-1;
	if(pInput->fOutputParam.b<0 || pInput->fOutputParam.b>pInput->fOutputParam.e)
		nRtn = 800;
	return nRtn;
/*
获利盘比例。
用法:
44 . WINNER(CLOSE),表示以当前收市价卖出的获利盘比例，例如返回0.1表示10%获利盘；WINNER(10.5)表示10.5元价格的获利盘比例
该函数仅对日线分析周期有效
  
成本分布情况。
用法:
COST(10),表示10%获利盘的价格是多少，即有10%的持仓量在该价格以下，其余90%在该价格以上，为套牢盘
该函数仅对日线分析周期有效
在图形上绘制小图标。
用法:
DRAWICON(COND,PRICE,TYPE),当COND条件满足时,在PRICE位置画TYPE号图标。
例如：DRAWICON(CLOSE>OPEN,LOW,1)表示当收阳时在最低价位置画1号图标。	
	
在图形上绘制直线段。
用法:
DRAWLINE(COND1,PRICE1,COND2,PRICE2,EXPAND),当COND1条件满足时,在PRICE1位置画直线起点，当COND2条件满足时,在PRICE2位置画直线终点，EXPAND为延长类型。
例如：DRAWILINE(HIGH>=HHV(HIGH,20),HIGH,LOW<=LLV(LOW,20),LOW,1)表示在创20天新高与创20天新低之间画直线并且向右延长。

在图形上显示文字。
用法:
DRAWTEXT(COND,PRICE,TEXT),当COND条件满足时,在PRICE位置书写文字TEXT。
例如：DRAWTEXT(CLOSE/OPEN>1.08,LOW,'大阳线')表示当日涨幅大于8%时在最低价位置显示'大阳线'字样。  
	*//*
FORCAST		线性回归预测值。
用法:
FORCAST(X,N)为X的N周期线性回归预测值
例如:FORCAST(CLOSE,10)表示求10周期线性回归预测本周期收盘价

SLOPE	线性回归斜率。
用法:
SLOPE(X,N)为X的N周期线性回归线的斜率
例如:SLOPE(CLOSE,10)表示求10周期线性回归线的斜率

VOLUNIT	每手股数。
用法:
VOLUNIT，返回每手股数，对于股票值为100，债券为1000
注意：该函数返回常数

*/
}

// 计算所有有效数据周期内最大值
int CFormularComputeParent::FuncHHAV()
{
	int cl/*,cr,ret,j*/;
	ARRAY_BE fcl;
	
	fcl.line = new float[m_MaxPoint];
	
	// 参数1个，表示数据成员
	cl=m_table.pop_it();
	GetData(cl,fcl);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b; 
	//	if(ret ==0) m_MidArray[m_MidNum].b =fcl.b; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		return 999;
	}
	
	{
		float ax=fcl.line[bl];
		int i = 0;
		for( i=bl;i<=be;i++)
		{
			//			float ax=fcl.line[i-ret+1];
			ax=max(ax,fcl.line[i]);
		}
		for ( i=bl; i<=be; i++ )
		{
			m_MidArray[m_MidNum].line[i] = ax;
		}
	}
	
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	return 0;
}

// 计算所有有效数据周期内最小值
int CFormularComputeParent::FuncLLAV()
{
	int cl;/*cr,ret,j;*/
	ARRAY_BE fcl;
	
	fcl.line = new float[m_MaxPoint];
	
	// 参数1个，表示数据成员
	cl=m_table.pop_it();
	GetData(cl,fcl);
	
	m_MidArray[m_MidNum].line = new float[m_MaxPoint];
	m_MidArray[m_MidNum].b =fcl.b; 
	//	if(ret ==0) m_MidArray[m_MidNum].b =fcl.b; 
	m_MidArray[m_MidNum].e =fcl.e;
	
	int bl = m_MidArray[m_MidNum].b;
	int be = m_MidArray[m_MidNum].e;
	if(bl>be||be>m_MaxPoint-1||bl<0)
	{
		m_table.push_b(PCMB+m_MidNum);
		m_MidNum++;
		delete[] fcl.line;
		return 999;
	}
	
	{
		float ax=fcl.line[bl];
		int i;
		for( i=bl;i<=be;i++)
		{
			//			float ax=fcl.line[i-ret+1];
			ax=min(ax,fcl.line[i]);
		}
		for ( i=bl; i<=be; i++ )
		{
			m_MidArray[m_MidNum].line[i] = ax;		// 都是同一个数
		}
	}
	
	
	m_table.push_b(PCMB+m_MidNum);
	m_MidNum++;
	
	delete[] fcl.line;
	return 0;
}

void CFormularComputeParent::SetMerchNodeUserData( MERCH_EXTRA_INFO *pExtraInfo )
{
	//m_pMerchUserData = pUserData;
	m_pExtraInfo = pExtraInfo;
	m_nKindKline	 = -1; // 未初始化类型
	m_iMinuteFromOpenTime = 1;
	m_iKlinePeriod	 = -1;
	if ( NULL != m_pExtraInfo /*m_pMerchUserData*/ && NULL != m_pExtraInfo->pMerchNode/*m_pMerchUserData->m_pMerchNode*/ )
	{
		// 初始化距离开盘的分钟数
		//CGGTongDoc *pDoc = AfxGetDocument();
		//ASSERT( NULL != pDoc && NULL != pDoc->m_pViewData );
		//CGmtTime TimeNow = pDoc->m_pViewData->GetServerTime();
		CMarketIOCTimeInfo IOCInfo;
		CGmtTime ctServerTime = CGmtTime::GetCurrentTime();
		if (g_pCenterManager)
		{
			ctServerTime = g_pCenterManager->GetServerTime();
		}
		time_t tNow = ctServerTime.GetTime()/*TimeNow.GetTime()*/;
		if ( /*m_pMerchUserData->m_pMerchNode*/m_pExtraInfo->pMerchNode->m_Market.GetRecentTradingDay(ctServerTime/*TimeNow*/, IOCInfo,m_pExtraInfo->pMerchNode->m_MerchInfo /*m_pMerchUserData->m_pMerchNode->m_MerchInfo*/) )
		{
			if ( tNow > IOCInfo.m_TimeOpen.m_Time.GetTime() )
			{
				// 大于开盘时间
				m_iMinuteFromOpenTime = 0;
				for ( int32 i=0; i < IOCInfo.m_aOCTimes.GetSize() ; i+=2 )
				{
					time_t tOpen  = IOCInfo.m_aOCTimes[i].GetTime();
					time_t tClose = IOCInfo.m_aOCTimes[i+1].GetTime();
					if ( tNow < tClose )
					{
						m_iMinuteFromOpenTime += (tNow-tOpen)/60 + 1;
						break;
					}
					else
					{
						m_iMinuteFromOpenTime += (tClose-tOpen)/60 + 1;
					}
				}
			}
			// 小于等于开盘时间则为1
		}

		// 初始化K线类型
// 		ENTIMinute = 0,			// 分钟线	
// 			ENTIDay,				// 日线
// 			ENTIMinute5,
// 			ENTIMinute15,
// 			ENTIMinute30,
// 			ENTIMinute60,
// 			ENTIMinuteUser,			// 用户自定义数目分钟线	
// 			ENTIDayUser,			// 用户自定义数目日线
// 			ENTIWeek,				// 周线
// 			ENTIMonth,				// 月线
// 			ENTIQuarter,			// 季度
// 	ENTIYear,				// 一年
		
		switch (/*m_pMerchUserData*/m_pExtraInfo->eTimeIntervalFull)
		{
		case ENTIMinute:
			m_nKindKline = MINUTE1_KLINE;
			m_iKlinePeriod = 0;
			break;
		case ENTIMinute5:
			m_nKindKline = MIN5_KLINE;
			m_iKlinePeriod = 1;
			break;
		case ENTIMinute15:
			m_nKindKline = MIN15_KLINE;
			m_iKlinePeriod = 2;
			break;
		case ENTIMinute30:
			m_nKindKline = MIN30_KLINE;
			m_iKlinePeriod = 3;
			break;
		case ENTIMinute60:
			m_nKindKline = MIN60_KLINE;
			m_iKlinePeriod = 4;
			break;
		case ENTIDay:
			m_nKindKline = DAY_KLINE;
			m_iKlinePeriod = 5;
			break;
		case ENTIWeek:
			m_nKindKline = WEEK_KLINE;
			m_iKlinePeriod = 6;
			break;
		case ENTIMonth:
			m_nKindKline = MONTH_KLINE;
			m_iKlinePeriod = 7;
			break;
		case ENTIMinuteUser:
			m_nKindKline = MANY_MINUTE_KLINE;
			m_iKlinePeriod = 8;
			break;
		case ENTIDayUser:
			m_nKindKline = MANY_DAY_KLINE;
			m_iKlinePeriod = 9;
			break;
		case ENTIQuarter:
			m_nKindKline = QUARTER_KLINE;
			m_iKlinePeriod = 10;
			break;
		case ENTIYear:
			m_nKindKline = YEAR_KLINE;
			m_iKlinePeriod = 11;
			break;
		}
	}
}
