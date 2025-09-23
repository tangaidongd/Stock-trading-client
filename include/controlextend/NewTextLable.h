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
//每一页的间隔距离20像素
#define PAGESPACE  0


//文本中每一个符号的基类
class CGraphTextChar;
//图文混排中每一段的结构
class CGraphTextLine;
//图文混排中字体的结构
class CGraphTextFont;

//图文混排中符号的链表
typedef CTypedPtrList<CObList, CGraphTextChar*> CGraphTextCharList;
//图文混排中段落的链表
typedef CTypedPtrList<CObList, CGraphTextLine*> CGraphTextLineList;
//图文混排中字体的链表
typedef CTypedPtrList<CObList, CGraphTextFont*> CGraphTextFontList;


//符号的基类
class CONTROL_EXPORT CGraphTextChar : public CObject  
{
	DECLARE_SERIAL(CGraphTextChar)
public:
	CGraphTextChar();
	virtual ~CGraphTextChar();
private:
	long	m_nCaret;		//在内存中的id
	int		m_nX;			//对象的开始X坐标
	int		m_nY;			//对象的开始Y坐标
	int		m_nWidth;		//对象的宽度
	int		m_nHeight;		//对象的高度
	int		m_nShowY;		//字符显示的y坐标
	int     m_nShowX;		//字符显示的x坐标
	int     m_nLine;		//每个字行位置
	int		m_nList;		//每个字的列位置
	int		m_nFontNum ;	//引用的字体ID	
	int     m_nFontHeight;	//光标的高度
	int     m_nFontWidth ;	//光标的宽度
	CString m_strText ;		//字的内容
	int     m_nBaseLineHeight;	//最大的高度
public:
/*************************************************************************
 	函数名称：Serialize()
	函数介绍：序列化对象的数据
*************************************************************************/
	virtual void Serialize(CArchive& ar);
	CGraphTextChar &operator = (CGraphTextChar& TextChar);
	//获取对象的排版宽高
	virtual CSize GetTextExtent(CDC *pDC );
public:
/*************************************************************************
 	函数名称：Compare()
	函数介绍：对象数据的比较 相同返回TRUE，不同返回FALSE
*************************************************************************/
	virtual BOOL Compare(CGraphTextChar *pChar);

/*************************************************************************
 	函数名称：CopyFrom()
	函数介绍：对象数据的拷贝用入口指针的数据代替本身的数据
*************************************************************************/
	virtual void CopyFrom(CGraphTextChar *pChar);

/*************************************************************************
 	函数名称：CopyTo()
	函数介绍：对象数据的拷贝用本身的数据代替入口参数指针的数据
*************************************************************************/
	virtual void CopyTo(CGraphTextChar *pChar);

/*************************************************************************
 	函数名称：DrawScale()
	函数介绍：对象的显示
*************************************************************************/
	virtual void DrawScale(CDC *pDC,int nMoveX,int nMoveY, float m_fScaleX = 1.0 ,
							float m_fScaleY = 1.0, int nMode = 0);

/*************************************************************************
 	函数名称：IsMouseInRect()
	函数介绍：鼠标是否在对象上
*************************************************************************/
	virtual BOOL IsMouseInRect(CPoint pt,CPoint ptMove,float fScaleX= 1.0,float fScaleY =1.0,int nMode = -1);

/*************************************************************************
 	函数名称：IsShow()
	函数介绍：对象是否显示
*************************************************************************/
	virtual BOOL IsShow(CRect rect,CPoint ptMove,float fScaleX = 1.0,float fScaleY=1.0,int nMode = -1);

/*************************************************************************
 	函数名称：GetNormalRect()
	函数介绍：或取正常的显示矩形
*************************************************************************/
	virtual CRect GetNormalRect(CPoint ptMove,float fScaleX = 1.0,float fScaleY=1.0,int nMode = -1);

/*************************************************************************
 	函数名称：GetScaleRect()
	函数介绍：获取比例缩放的矩形
*************************************************************************/
	virtual CRect GetScaleRect(float fScaleX = 1.0,float fScaleY=1.0,int nMode = -1);

/*************************************************************************
 	函数名称：GetTureRect()
	函数介绍：获取真实的矩形
*************************************************************************/
	virtual CRect GetTureRect(int nMode = -1);

/*************************************************************************
 	函数名称：GetWidth()
	函数介绍：获取字符的宽度
*************************************************************************/
	virtual int GetWidth();
/*************************************************************************
 	函数名称：SetWidth()
	函数介绍：设置字符的宽度
*************************************************************************/
	void SetWidth( int width );

/*************************************************************************
 	函数名称：GetHeight()
	函数介绍：获取字符的高度
*************************************************************************/
	virtual int GetHeight();
/*************************************************************************
 	函数名称：SetHeight()
	函数介绍：设置字符的高度
*************************************************************************/
	void SetHeight(int height );

/*************************************************************************
 	函数名称：FontHeight()
	函数介绍：获取字高
*************************************************************************/
	virtual int FontHeight();
/*************************************************************************
 	函数名称：SetFontHeight()
	函数介绍：设置字高
*************************************************************************/
    void SetFontHeight(int height);

/*************************************************************************
 	函数名称：GetFontIndex()
	函数介绍：获取字体索引
*************************************************************************/
	virtual int GetFontIndex();
/*************************************************************************
 	函数名称：SetFontIndex()
	函数介绍：设置体索引
*************************************************************************/
    void SetFontIndex(int nIndex);

/*************************************************************************
 	函数名称：GetFontWidth()
	函数介绍：获得字体
*************************************************************************/
	int GetFontWidth();
/*************************************************************************
 	函数名称：SetFontWidth()
	函数介绍：设置字体
*************************************************************************/
	void SetFontWidth(int width);

/*************************************************************************
 	函数名称：GetCaretId()
	函数介绍：取得光标
*************************************************************************/
	long GetCaretId();
/*************************************************************************
 	函数名称：SetCaretId()
	函数介绍：设置光标
*************************************************************************/
	void SetCaretId( long lCaretId );

/*************************************************************************
 	函数名称：GetCharX()
	函数介绍：对象的开始X坐标
*************************************************************************/
	int GetCharX();
/*************************************************************************
 	函数名称：SetCharX()
	函数介绍：对象的开始X坐标
*************************************************************************/
	void SetCharX( int x );

//
/*************************************************************************
 	函数名称：GetCharY()
	函数介绍：对象的开始Y坐标
*************************************************************************/
	int GetCharY();
/*************************************************************************
 	函数名称：SetCharY()
	函数介绍：对象的开始Y坐标
*************************************************************************/
	void SetCharY(int y );

/*************************************************************************
 	函数名称：GetCharShowY()
	函数介绍：字符显示的y坐标
*************************************************************************/
	int GetCharShowY();   
/*************************************************************************
 	函数名称：SetCharShowY()
	函数介绍：字符显示的y坐标
*************************************************************************/
	void SetCharShowY( int showy );

/*************************************************************************
 	函数名称：GetCharShowX()
	函数介绍：字符显示的x坐标
*************************************************************************/
	int   GetCharShowX(); 
/*************************************************************************
 	函数名称：SetCharShowX()
	函数介绍：字符显示的x坐标
*************************************************************************/
	void  SetCharShowX( int showx );

/*************************************************************************
 	函数名称：GetCharLine()
	函数介绍：每个字行位置
*************************************************************************/
	int   GetCharLine();
/*************************************************************************
 	函数名称：SetCharLine()
	函数介绍：每个字行位置
*************************************************************************/
	void  SetCharLine( int line );

/*************************************************************************
 	函数名称：GetCharList()
	函数介绍：每个字的列位置
*************************************************************************/
	int	  GetCharList();
/*************************************************************************
 	函数名称：SetCharList()
	函数介绍：每个字的列位置
*************************************************************************/
	void  SetCharList(int list);

	CString GetStrText();
	void    SetStrText(CString str);

	int GetBaseLineHeight();
	void SetBaseLineHeight(int nMaxHeight);
};

//特殊的东西
class CONTROL_EXPORT CGraphTextCharLineFlag:public CGraphTextChar
{
	DECLARE_SERIAL(CGraphTextCharLineFlag)
public:
	//构造函数一
	CGraphTextCharLineFlag();
	//构造函数二
	virtual ~CGraphTextCharLineFlag();

	virtual void DrawScale(CDC *pDC,int nMoveX,int nMoveY,int nSelectBegin,int nSelectEnd,
		float m_fScaleX = 1.0 ,float m_fScaleY = 1.0, int nMode = 0);

	/*************************************************************************
 	函数名称：DrawScale()
	函数介绍：对象的显示
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
	///排版方式 正常、左对齐、右对齐、居中、绕排。
	enum GrapText_MkMode
	{ 
		GRPAHTXT_MKMODE_NORMALL = 0,
		GRPAHTXT_MKMODE_LEFT,
		GRPAHTXT_MKMODE_RIGHT,
		GRPAHTXT_MKMODE_CENTER,
		GRPAHTXT_MKMODE_AROUND
	};
	//构造函数一
	CGraphTextLine();
	//构造函数二
	CGraphTextLine(long nID,CGraphTextChar *pchar = NULL);
	virtual ~CGraphTextLine();
private:
	GrapText_MkMode m_nMakeUpType; //排版的方式
	int m_nLineSpace; //每个字的行距
	int m_nListSpace; //每个字的列距
	int m_nBeginID;	  //开始Id
	int m_nEndID;	  //结束Id
	int m_nTop;
	int m_nLeft;
	int m_nBottom;
	int m_nRight;
	BOOL m_IsTitle;   //是否为标题
	CString m_strCaption; //标题说明
	long m_nBeginLine; //开始行号
	long m_nEndLine;  //结束行号
	CGraphTextCharList m_CharTextList; //字符链表
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
 	函数名称：Serialize()
	函数介绍：对象数据的序列化
*************************************************************************/
	virtual void Serialize(CArchive& ar);

	void Clear();

	void Init();
public:
/*************************************************************************
 	函数名称：DrawScale()
	函数介绍：数据显示
*************************************************************************/
	virtual void DrawScale(CDC *pDC,int nMoveX,int nMoveY,int nSelectBegin,int nSelectEnd,
		float m_fScaleX = 1.0 ,float m_fScaleY = 1.0, int nMode = 0);

/*************************************************************************
 	函数名称：InsterChar()
	函数介绍：插入一个字符
*************************************************************************/
	virtual void InsterChar(int nId,CGraphTextChar *pchar);

/*************************************************************************
 	函数名称：MoveId()
	函数介绍：修改数据的ID
*************************************************************************/
	virtual void MoveId(int nId,int step= 1);

/*************************************************************************
 	函数名称：DeleteChar()
	函数介绍：删除字符
*************************************************************************/
	virtual int DeleteChar(int nID);

	virtual void CopyFrom(CGraphTextLine *pItem );

/*************************************************************************
 	函数名称：IsShow()
	函数介绍：是否显示
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
	LOGFONT m_LogFont; //逻辑字体
	COLORREF m_nColor; //颜色
	int m_nCharNum; //多少个字用这种字体的统计，当为0是把它从链表中删除。

public:
/*************************************************************************
 	函数名称：Serialize()
	函数介绍：对象数据的序列化
*************************************************************************/
	virtual void Serialize(CArchive& ar);
public:
/*************************************************************************
 	函数名称：Compare()
	函数介绍：对象数据的比较 相同返回TRUE，不同返回FALSE
*************************************************************************/
	virtual BOOL Compare(CGraphTextFont *pFont);

/*************************************************************************
 	函数名称：CopyFrom()
	函数介绍：对象数据的拷贝用入口指针的数据代替本身的数据
*************************************************************************/
	virtual void CopyFrom(CGraphTextFont *pFont);

/*************************************************************************
 	函数名称：CopyTo()
	函数介绍：对象数据的拷贝用本身的数据代替入口参数指针的数据
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
	//清楚所有状态
	void ClearStatus();
	//初始化
	void InitStatus(const CRect &rcPage,const CPoint &ptLefttop=CPoint(10,10),const CPoint &ptRightBottom = CPoint(10,10));

	void SetNewsText(const CString &strText,long nTextId = 0 ,CGraphTextFont *pFont = NULL,CGraphTextLine::GrapText_MkMode nModl =CGraphTextLine::GRPAHTXT_MKMODE_LEFT);

	void AddLineFlag(long nTextId = 0 ,CGraphTextFont *pFont = NULL,int nMode =0);
	//显示排版的效果
	void Draw(CDC *pDC,CRect rect,CPoint ptScroll,int nstar,int nend,
		float fScalex=1.0,float fScaley=1.0,int nMode = 0);

	void MakeUpText();
private:

	//横排从左到右
	void DrawScaleHorFromLeft(CDC *pDC,CGraphTextLine *pTextLine,
		CPoint ptOffSet,CPoint ptScroll,int nSelectBegin,int nSelectEnd,
		float fScaleX,float fScaleY, int nMode = 0);

	POSITION MakeUpTextPositon(CDC *pDC,POSITION pos);

	void MakeUpText(CDC *pDC,long nId,BOOL bFlag );
	//插入一个字符
	void InsterOneChar(CGraphTextFont *pfont,CGraphTextChar *pchar,long nID);

	CGraphTextChar  *InsterOneChar(CString m_text,CGraphTextFont *pfont,long nID);

	//插入一个字符
	CGraphTextLine * InsterChar(int nId,CGraphTextChar *pchar,bool bFlagEnter = TRUE);

	//根据字体的索引号查询字体的指针
	void GetPfsFont(int num,CGraphTextFont *pFont);

	//获取字体的索引
	int  GetFontIndex(CGraphTextFont *pFont);
	
	//检查是否新的字体设置
	int IsNewFont(CGraphTextFont *pfont);

	//查询当前的ID所在的段落，并返回段落的指针
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

	//是否是汉字
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
		
		//汉字是图形字符，区号大于16
		//if (h<=0xa0||h>0xfe||l<0xa0+16||l>0xfe)
		//	return false;
		if( !( l & 0x80) )
			return false;
		
		return true;
	}

	CPoint GetMaxSize( );
// Operations
public:
	//获取最大的滚动区
	CPoint GetTextMaxSize( );
	//获取滚动的距离
	CPoint GetTextScrollPos();
	//设置滚动距离
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
	//图文混排的段落的链表
	CGraphTextLineList m_GraphTextLineList;
	//图文混排的字体的链表
	CGraphTextFontList m_TextFontList;

	int m_nLineNum;
	//当前的列号
	int m_nListNum;	
	
	int m_nPosx;
	//排版的起始点的Y坐标
	int m_nPosy;
	//页面大小
	CRect  m_rcPage;
	CPoint m_ptLeftTop;
	CPoint m_ptRightBottom;
	//滚动
	CPoint m_ptScrollPos;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWTEXTLABLE_H__215FAE05_4F27_4E6D_B96B_F21D0B3ABF17__INCLUDED_)
