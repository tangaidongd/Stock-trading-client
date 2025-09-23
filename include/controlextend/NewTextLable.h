#if !defined(AFX_NEWTEXTLABLE_H__215FAE05_4F27_4E6D_B96B_F21D0B3ABF17__INCLUDED_)
#define AFX_NEWTEXTLABLE_H__215FAE05_4F27_4E6D_B96B_F21D0B3ABF17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewTextLable.h : header file
//
#include "coding.h"
#include "dllexport.h"
#include "typedef.h"
//ÿһҳ�ļ������20����
#define PAGESPACE  0


//�ı���ÿһ�����ŵĻ���
class CGraphTextChar;
//ͼ�Ļ�����ÿһ�εĽṹ
class CGraphTextLine;
//ͼ�Ļ���������Ľṹ
class CGraphTextFont;

//ͼ�Ļ����з��ŵ�����
typedef CTypedPtrList<CObList, CGraphTextChar*> CGraphTextCharList;
//ͼ�Ļ����ж��������
typedef CTypedPtrList<CObList, CGraphTextLine*> CGraphTextLineList;
//ͼ�Ļ��������������
typedef CTypedPtrList<CObList, CGraphTextFont*> CGraphTextFontList;


//���ŵĻ���
class CONTROL_EXPORT CGraphTextChar : public CObject  
{
	DECLARE_SERIAL(CGraphTextChar)
public:
	CGraphTextChar();
	virtual ~CGraphTextChar();
private:
	long	m_nCaret;		//���ڴ��е�id
	int		m_nX;			//����Ŀ�ʼX����
	int		m_nY;			//����Ŀ�ʼY����
	int		m_nWidth;		//����Ŀ��
	int		m_nHeight;		//����ĸ߶�
	int		m_nShowY;		//�ַ���ʾ��y����
	int     m_nShowX;		//�ַ���ʾ��x����
	int     m_nLine;		//ÿ������λ��
	int		m_nList;		//ÿ���ֵ���λ��
	int		m_nFontNum ;	//���õ�����ID	
	int     m_nFontHeight;	//���ĸ߶�
	int     m_nFontWidth ;	//���Ŀ��
	CString m_strText ;		//�ֵ�����
	int     m_nBaseLineHeight;	//���ĸ߶�
public:
/*************************************************************************
 	�������ƣ�Serialize()
	�������ܣ����л����������
*************************************************************************/
	virtual void Serialize(CArchive& ar);
	CGraphTextChar &operator = (CGraphTextChar& TextChar);
	//��ȡ������Ű���
	virtual CSize GetTextExtent(CDC *pDC );
public:
/*************************************************************************
 	�������ƣ�Compare()
	�������ܣ��������ݵıȽ� ��ͬ����TRUE����ͬ����FALSE
*************************************************************************/
	virtual BOOL Compare(CGraphTextChar *pChar);

/*************************************************************************
 	�������ƣ�CopyFrom()
	�������ܣ��������ݵĿ��������ָ������ݴ��汾�������
*************************************************************************/
	virtual void CopyFrom(CGraphTextChar *pChar);

/*************************************************************************
 	�������ƣ�CopyTo()
	�������ܣ��������ݵĿ����ñ�������ݴ�����ڲ���ָ�������
*************************************************************************/
	virtual void CopyTo(CGraphTextChar *pChar);

/*************************************************************************
 	�������ƣ�DrawScale()
	�������ܣ��������ʾ
*************************************************************************/
	virtual void DrawScale(CDC *pDC,int nMoveX,int nMoveY, float m_fScaleX = 1.0 ,
							float m_fScaleY = 1.0, int nMode = 0);

/*************************************************************************
 	�������ƣ�IsMouseInRect()
	�������ܣ�����Ƿ��ڶ�����
*************************************************************************/
	virtual BOOL IsMouseInRect(CPoint pt,CPoint ptMove,float fScaleX= 1.0,float fScaleY =1.0,int nMode = -1);

/*************************************************************************
 	�������ƣ�IsShow()
	�������ܣ������Ƿ���ʾ
*************************************************************************/
	virtual BOOL IsShow(CRect rect,CPoint ptMove,float fScaleX = 1.0,float fScaleY=1.0,int nMode = -1);

/*************************************************************************
 	�������ƣ�GetNormalRect()
	�������ܣ���ȡ��������ʾ����
*************************************************************************/
	virtual CRect GetNormalRect(CPoint ptMove,float fScaleX = 1.0,float fScaleY=1.0,int nMode = -1);

/*************************************************************************
 	�������ƣ�GetScaleRect()
	�������ܣ���ȡ�������ŵľ���
*************************************************************************/
	virtual CRect GetScaleRect(float fScaleX = 1.0,float fScaleY=1.0,int nMode = -1);

/*************************************************************************
 	�������ƣ�GetTureRect()
	�������ܣ���ȡ��ʵ�ľ���
*************************************************************************/
	virtual CRect GetTureRect(int nMode = -1);

/*************************************************************************
 	�������ƣ�GetWidth()
	�������ܣ���ȡ�ַ��Ŀ��
*************************************************************************/
	virtual int GetWidth();
/*************************************************************************
 	�������ƣ�SetWidth()
	�������ܣ������ַ��Ŀ��
*************************************************************************/
	void SetWidth( int width );

/*************************************************************************
 	�������ƣ�GetHeight()
	�������ܣ���ȡ�ַ��ĸ߶�
*************************************************************************/
	virtual int GetHeight();
/*************************************************************************
 	�������ƣ�SetHeight()
	�������ܣ������ַ��ĸ߶�
*************************************************************************/
	void SetHeight(int height );

/*************************************************************************
 	�������ƣ�FontHeight()
	�������ܣ���ȡ�ָ�
*************************************************************************/
	virtual int FontHeight();
/*************************************************************************
 	�������ƣ�SetFontHeight()
	�������ܣ������ָ�
*************************************************************************/
    void SetFontHeight(int height);

/*************************************************************************
 	�������ƣ�GetFontIndex()
	�������ܣ���ȡ��������
*************************************************************************/
	virtual int GetFontIndex();
/*************************************************************************
 	�������ƣ�SetFontIndex()
	�������ܣ�����������
*************************************************************************/
    void SetFontIndex(int nIndex);

/*************************************************************************
 	�������ƣ�GetFontWidth()
	�������ܣ��������
*************************************************************************/
	int GetFontWidth();
/*************************************************************************
 	�������ƣ�SetFontWidth()
	�������ܣ���������
*************************************************************************/
	void SetFontWidth(int width);

/*************************************************************************
 	�������ƣ�GetCaretId()
	�������ܣ�ȡ�ù��
*************************************************************************/
	long GetCaretId();
/*************************************************************************
 	�������ƣ�SetCaretId()
	�������ܣ����ù��
*************************************************************************/
	void SetCaretId( long lCaretId );

/*************************************************************************
 	�������ƣ�GetCharX()
	�������ܣ�����Ŀ�ʼX����
*************************************************************************/
	int GetCharX();
/*************************************************************************
 	�������ƣ�SetCharX()
	�������ܣ�����Ŀ�ʼX����
*************************************************************************/
	void SetCharX( int x );

//
/*************************************************************************
 	�������ƣ�GetCharY()
	�������ܣ�����Ŀ�ʼY����
*************************************************************************/
	int GetCharY();
/*************************************************************************
 	�������ƣ�SetCharY()
	�������ܣ�����Ŀ�ʼY����
*************************************************************************/
	void SetCharY(int y );

/*************************************************************************
 	�������ƣ�GetCharShowY()
	�������ܣ��ַ���ʾ��y����
*************************************************************************/
	int GetCharShowY();   
/*************************************************************************
 	�������ƣ�SetCharShowY()
	�������ܣ��ַ���ʾ��y����
*************************************************************************/
	void SetCharShowY( int showy );

/*************************************************************************
 	�������ƣ�GetCharShowX()
	�������ܣ��ַ���ʾ��x����
*************************************************************************/
	int   GetCharShowX(); 
/*************************************************************************
 	�������ƣ�SetCharShowX()
	�������ܣ��ַ���ʾ��x����
*************************************************************************/
	void  SetCharShowX( int showx );

/*************************************************************************
 	�������ƣ�GetCharLine()
	�������ܣ�ÿ������λ��
*************************************************************************/
	int   GetCharLine();
/*************************************************************************
 	�������ƣ�SetCharLine()
	�������ܣ�ÿ������λ��
*************************************************************************/
	void  SetCharLine( int line );

/*************************************************************************
 	�������ƣ�GetCharList()
	�������ܣ�ÿ���ֵ���λ��
*************************************************************************/
	int	  GetCharList();
/*************************************************************************
 	�������ƣ�SetCharList()
	�������ܣ�ÿ���ֵ���λ��
*************************************************************************/
	void  SetCharList(int list);

	CString GetStrText();
	void    SetStrText(CString str);

	int GetBaseLineHeight();
	void SetBaseLineHeight(int nMaxHeight);
};

//����Ķ���
class CONTROL_EXPORT CGraphTextCharLineFlag:public CGraphTextChar
{
	DECLARE_SERIAL(CGraphTextCharLineFlag)
public:
	//���캯��һ
	CGraphTextCharLineFlag();
	//���캯����
	virtual ~CGraphTextCharLineFlag();

	virtual void DrawScale(CDC *pDC,int nMoveX,int nMoveY,int nSelectBegin,int nSelectEnd,
		float m_fScaleX = 1.0 ,float m_fScaleY = 1.0, int nMode = 0);

	/*************************************************************************
 	�������ƣ�DrawScale()
	�������ܣ��������ʾ
*************************************************************************/
	virtual void DrawScale(CDC *pDC,int nMoveX,int nMoveY, float m_fScaleX = 1.0 ,
							float m_fScaleY = 1.0, int nMode = 0);

	virtual void Serialize(CArchive& ar);

	void SetLineW(const int &nWidth){ m_nLineW = nWidth; };

	void SetDrawModel(const int &nModel){ m_nDrawModel = nModel;};
private:
	int m_nDrawModel;

	int m_nLineW;
};


class CONTROL_EXPORT CGraphTextLine : public CObject
{
	DECLARE_SERIAL(CGraphTextLine)
public:
	///�Ű淽ʽ ����������롢�Ҷ��롢���С����š�
	enum GrapText_MkMode
	{ 
		GRPAHTXT_MKMODE_NORMALL = 0,
		GRPAHTXT_MKMODE_LEFT,
		GRPAHTXT_MKMODE_RIGHT,
		GRPAHTXT_MKMODE_CENTER,
		GRPAHTXT_MKMODE_AROUND
	};
	//���캯��һ
	CGraphTextLine();
	//���캯����
	CGraphTextLine(long nID,CGraphTextChar *pchar = NULL);
	virtual ~CGraphTextLine();
private:
	GrapText_MkMode m_nMakeUpType; //�Ű�ķ�ʽ
	int m_nLineSpace; //ÿ���ֵ��о�
	int m_nListSpace; //ÿ���ֵ��о�
	int m_nBeginID;	  //��ʼId
	int m_nEndID;	  //����Id
	int m_nTop;
	int m_nLeft;
	int m_nBottom;
	int m_nRight;
	BOOL m_IsTitle;   //�Ƿ�Ϊ����
	CString m_strCaption; //����˵��
	long m_nBeginLine; //��ʼ�к�
	long m_nEndLine;  //�����к�
	CGraphTextCharList m_CharTextList; //�ַ�����
public:
	inline void SetBeginLine(const int &nBeginLine){ m_nBeginLine = nBeginLine; };
	inline long GetBeginLine(){ return m_nBeginLine; };

	inline void SetEndLine(const int &nEndLine ){ m_nEndLine = nEndLine; };
	inline long GetEndLine(){ return m_nEndLine; };

	inline CPoint GetLeftTop(){ return CPoint( m_nLeft, m_nTop); };
	inline void SetLeftTop(const CPoint &pt){ m_nLeft = pt.x; m_nTop = pt.y; };

	inline CPoint GetRightBottom(){ return CPoint(m_nRight,m_nBottom); };
	inline void SetRightBottom(const CPoint &pt){ m_nRight = pt.x; m_nBottom = pt.y; };

	inline CRect GetTextLineRect(){ return CRect(m_nLeft,m_nTop,m_nRight,m_nBottom); };

	GrapText_MkMode GetMakeUpType();
	void SetMakeUpType( GrapText_MkMode nType );
public:
/*************************************************************************
 	�������ƣ�Serialize()
	�������ܣ��������ݵ����л�
*************************************************************************/
	virtual void Serialize(CArchive& ar);

	void Clear();

	void Init();
public:
/*************************************************************************
 	�������ƣ�DrawScale()
	�������ܣ�������ʾ
*************************************************************************/
	virtual void DrawScale(CDC *pDC,int nMoveX,int nMoveY,int nSelectBegin,int nSelectEnd,
		float m_fScaleX = 1.0 ,float m_fScaleY = 1.0, int nMode = 0);

/*************************************************************************
 	�������ƣ�InsterChar()
	�������ܣ�����һ���ַ�
*************************************************************************/
	virtual void InsterChar(int nId,CGraphTextChar *pchar);

/*************************************************************************
 	�������ƣ�MoveId()
	�������ܣ��޸����ݵ�ID
*************************************************************************/
	virtual void MoveId(int nId,int step= 1);

/*************************************************************************
 	�������ƣ�DeleteChar()
	�������ܣ�ɾ���ַ�
*************************************************************************/
	virtual int DeleteChar(int nID);

	virtual void CopyFrom(CGraphTextLine *pItem );

/*************************************************************************
 	�������ƣ�IsShow()
	�������ܣ��Ƿ���ʾ
*************************************************************************/
	BOOL IsShow(CRect rect,CPoint ptMove ,float fScaleX=1.0,float fScaleY=1.0,int nMode = -1);

	int GetLineSpace();
	void SetLineSpace(int nLineSpace);

	int GetListSpace();
	void SetListSpace(int nListSpace );

	int  GetBeiginId();
	void SetBeiginId( int nBeigid );

	int  GetEndId();
	void SetEndId(int nEndid);

	BOOL GetbTitle();
	void SetbTitle(BOOL bTitle );

    CString GetCaption();
	void SetCaption(CString strCaption );

	CGraphTextCharList *GetCharList();
};


////////////////////////////////////////////////////////
class CONTROL_EXPORT CGraphTextFont :public CObject
{
	DECLARE_SERIAL(CGraphTextFont)
public:
	CGraphTextFont();
	virtual ~CGraphTextFont();

private:
	LOGFONT m_LogFont; //�߼�����
	COLORREF m_nColor; //��ɫ
	int m_nCharNum; //���ٸ��������������ͳ�ƣ���Ϊ0�ǰ�����������ɾ����

public:
/*************************************************************************
 	�������ƣ�Serialize()
	�������ܣ��������ݵ����л�
*************************************************************************/
	virtual void Serialize(CArchive& ar);
public:
/*************************************************************************
 	�������ƣ�Compare()
	�������ܣ��������ݵıȽ� ��ͬ����TRUE����ͬ����FALSE
*************************************************************************/
	virtual BOOL Compare(CGraphTextFont *pFont);

/*************************************************************************
 	�������ƣ�CopyFrom()
	�������ܣ��������ݵĿ��������ָ������ݴ��汾�������
*************************************************************************/
	virtual void CopyFrom(CGraphTextFont *pFont);

/*************************************************************************
 	�������ƣ�CopyTo()
	�������ܣ��������ݵĿ����ñ�������ݴ�����ڲ���ָ�������
*************************************************************************/
	virtual void CopyTo(CGraphTextFont *pFont);
		
	LOGFONT *GetLogFont ();

	COLORREF GetFontColor();
	void SetFontColor(COLORREF color);

	int GetCharNum();
	void SetCharNum(int nNum );

	static      int32  GetFontlfHeight(const int32 &nPtWight);
	static		int32 GetFontptHeight(const int32 &nlfWight);
};

/////////////////////////////////////////////////////////////////////////////
// CNewTextLable window

class CONTROL_EXPORT CNewTextLable : public CObject
{
	DECLARE_SERIAL(CNewTextLable)
// Construction
public:
	CNewTextLable();

// Attributes
public:
	//�������״̬
	void ClearStatus();
	//��ʼ��
	void InitStatus(const CRect &rcPage,const CPoint &ptLefttop=CPoint(10,10),const CPoint &ptRightBottom = CPoint(10,10));

	void SetNewsText(const CString &strText,long nTextId = 0 ,CGraphTextFont *pFont = NULL,CGraphTextLine::GrapText_MkMode nModl =CGraphTextLine::GRPAHTXT_MKMODE_LEFT);

	void AddLineFlag(long nTextId = 0 ,CGraphTextFont *pFont = NULL,int nMode =0);
	//��ʾ�Ű��Ч��
	void Draw(CDC *pDC,CRect rect,CPoint ptScroll,int nstar,int nend,
		float fScalex=1.0,float fScaley=1.0,int nMode = 0);

	void MakeUpText();
private:

	//���Ŵ�����
	void DrawScaleHorFromLeft(CDC *pDC,CGraphTextLine *pTextLine,
		CPoint ptOffSet,CPoint ptScroll,int nSelectBegin,int nSelectEnd,
		float fScaleX,float fScaleY, int nMode = 0);

	POSITION MakeUpTextPositon(CDC *pDC,POSITION pos);

	void MakeUpText(CDC *pDC,long nId,BOOL bFlag );
	//����һ���ַ�
	void InsterOneChar(CGraphTextFont *pfont,CGraphTextChar *pchar,long nID);

	CGraphTextChar  *InsterOneChar(CString m_text,CGraphTextFont *pfont,long nID);

	//����һ���ַ�
	CGraphTextLine * InsterChar(int nId,CGraphTextChar *pchar,bool bFlagEnter = TRUE);

	//��������������Ų�ѯ�����ָ��
	void GetPfsFont(int num,CGraphTextFont *pFont);

	//��ȡ���������
	int  GetFontIndex(CGraphTextFont *pFont);
	
	//����Ƿ��µ���������
	int IsNewFont(CGraphTextFont *pfont);

	//��ѯ��ǰ��ID���ڵĶ��䣬�����ض����ָ��
	CGraphTextLine *GetGraphTextLine(long nID,POSITION *poscurrent);

	void AddEnterFlag(CGraphTextChar *pchar);
	
	void InsterTextLine(long nID,CGraphTextLine *pTextLine,CGraphTextLine *pTextLineTemp);

	POSITION MakeUpTextHorFromLeft(CDC *pDC,long nId,BOOL bFlag);

	BOOL MakeUpPageHorFromeLeft(CDC *pDC,long nId,CGraphTextLine *pTextLine,CPoint *nPoint);

	CGraphTextCharList * GetGraphTextCharList(CGraphTextLine *pTextLine);

	int GetGraphTextLineListCount();
	
	inline CGraphTextLineList *GetGraphTextLineList(){ return &m_GraphTextLineList; };
	inline CGraphTextFontList *GetGraphTextFont(){ return &m_TextFontList; };

	int CheckPositionHorFromeLeft(CRect rect1, CRect rect2, CPoint *point,
		CString str,int width,int linespace,BOOL bEnglish);

	BOOL MakeUpPageHorFromeLeftTextLineRight(
		POSITION posHead,POSITION posTail,CGraphTextCharList *pTextLineCharList,\
		const int &nListSpace,const int &nLineSpace,const int &nMaxFontHeight );

	BOOL MakeUpPageHorFromeLeftTextLineCenter( POSITION posHead , POSITION posTail,
		CGraphTextCharList *pTextLineCharList,const int &nListSpace,
		const int &nLineSpace,const int &nMaxFontHeight );

	BOOL IsTureFuhao(const CString &str);

	BOOL CheckFrontToken(const CString &str);

	BOOL CheckPositionHorFromeRight(CPoint *point,CRect rect1, int width,int linespace);
	BOOL CheckPositionHorFromeCenter(CPoint *point,CRect rect1, int width,int linespace);

	//�Ƿ��Ǻ���
	bool IsChinese(const CString &str)
	{
		string strAsc;
		bool32 bOK = Unicode2MultiChar(CP_ACP, str, strAsc);
		if(! bOK )
			return false;

		TCHAR  bz1 = 0;
		TCHAR  bz2 = 0;
		if( strAsc.length() >1 ) 
		{
			bz1 = strAsc[0];
			bz2 = strAsc[1];
		}
		else
			bz1 = strAsc[0];
		
		WORD ch = MAKEWORD(bz1,bz2);
		BYTE l,h;
		l=LOBYTE(ch);
		h=HIBYTE(ch);
		
		//������ͼ���ַ������Ŵ���16
		//if (h<=0xa0||h>0xfe||l<0xa0+16||l>0xfe)
		//	return false;
		if( !( l & 0x80) )
			return false;
		
		return true;
	}

	CPoint GetMaxSize( );
// Operations
public:
	//��ȡ���Ĺ�����
	CPoint GetTextMaxSize( );
	//��ȡ�����ľ���
	CPoint GetTextScrollPos();
	//���ù�������
	void SetTextScrollPos(CPoint pt);

	CPoint GetScrollPos();

	void SetScrollPos(CPoint pt);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewTextLable)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNewTextLable();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNewTextLable)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
private:
	//ͼ�Ļ��ŵĶ��������
	CGraphTextLineList m_GraphTextLineList;
	//ͼ�Ļ��ŵ����������
	CGraphTextFontList m_TextFontList;

	int m_nLineNum;
	//��ǰ���к�
	int m_nListNum;	
	
	int m_nPosx;
	//�Ű����ʼ���Y����
	int m_nPosy;
	//ҳ���С
	CRect  m_rcPage;
	CPoint m_ptLeftTop;
	CPoint m_ptRightBottom;
	//����
	CPoint m_ptScrollPos;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWTEXTLABLE_H__215FAE05_4F27_4E6D_B96B_F21D0B3ABF17__INCLUDED_)
