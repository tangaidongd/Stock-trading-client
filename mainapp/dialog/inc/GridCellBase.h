/////////////////////////////////////////////////////////////////////////////
// GridCellBase.h : header file
//
// MFC Grid Control - Grid cell base class header file
//
// Written by Chris Maunder <chris@codeproject.com>
// Copyright (c) 1998-2005. All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with CGridCtrl v2.22+
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDCELLBASE_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_)
#define AFX_GRIDCELLBASE_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CGridCtrl;

// Cell states
#define GVIS_FOCUSED            0x0001
#define GVIS_SELECTED           0x0002
#define GVIS_DROPHILITED        0x0004
#define GVIS_READONLY           0x0008
#define GVIS_FIXED              0x0010
#define GVIS_FIXEDROW           0x0020
#define GVIS_FIXEDCOL           0x0040
#define GVIS_MODIFIED           0x0080
#define GVIS_BLINK				0x0100	// zhangbo 0402 #for blink
#define GVIF_FRAMERIGHT			0x0200	// zhangbo 0407 #for draw frame
#define GVIF_FRAMEBOTTOM		0x0400	// zhangbo 0407 #for draw frame
#define GVIS_SHOWTIPS			0x0800	// fangz     0422 #for Tips

// Cell data mask
#define GVIF_TEXT               LVIF_TEXT
#define GVIF_IMAGE              LVIF_IMAGE
#define GVIF_PARAM              LVIF_PARAM
#define GVIF_STATE              LVIF_STATE
#define GVIF_BKCLR              (GVIF_STATE<<1)
#define GVIF_FGCLR              (GVIF_STATE<<2)
#define GVIF_FORMAT             (GVIF_STATE<<3)
#define GVIF_FONT               (GVIF_STATE<<4)
#define GVIF_MARGIN             (GVIF_STATE<<5)
#define GVIF_ALL                (GVIF_TEXT|GVIF_IMAGE|GVIF_PARAM|GVIF_STATE|GVIF_BKCLR|GVIF_FGCLR| \
                                 GVIF_FORMAT|GVIF_FONT|GVIF_MARGIN|GVIF_FRAMERIGHT|GVIF_FRAMEBOTTOM)

// ���ָı��ʱ����
#define BLINK_STEP_COUNT	5

enum E_DrawMarkFlag
{
	EDMF_None = 0,		// ���������±�
	EDMF_Top = 1,		// �����ϱ�����
	EDMF_Bottom = 2,	// �����±�����
};

// �����ϱ꣬�±���Ҫ������
typedef struct _GV_DRAWMARK_ITEM
{
	E_DrawMarkFlag	eDrawMarkFlag;	// ���Ʊ�Ƿ���
	COLORREF		clrMark;		// �����ɫ
	CString			strMark;		// �������
}GV_DRAWMARK_ITEM;


// Used for Get/SetItem calls.
typedef struct _GV_ITEM {
    int      row,col;     // Row and Column of item
    UINT     mask;        // Mask for use in getting/setting cell data
    UINT     nState;      // cell state (focus/hilighted etc)
    DWORD    nFormat;     // Format of cell
    int      iImage;      // index of the list view item�s icon
    COLORREF crBkClr;     // Background colour (or CLR_DEFAULT)
    COLORREF crFgClr;     // Forground colour (or CLR_DEFAULT)
    LPARAM   lParam;      // 32-bit value to associate with item
    LOGFONT  lfFont;      // Cell font
    UINT     nMargin;     // Internal cell margin
    CString  strText;     // Text in cell
	CString  strTip;	  // ��ʾ�����ִ�
	CString  strTipTitle; // ��ʾ����
	CRect	 rtTextPadding;	// �ı���߽���
	GV_DRAWMARK_ITEM markItem;	// �������
} GV_ITEM;


// Each cell contains one of these. Fields "row" and "column" are not stored since we
// will usually have acces to them in other ways, and they are an extra 8 bytes per
// cell that is probably unnecessary.

class CGridCellBase : public CObject
{
    friend class CGridCtrl;
    DECLARE_DYNAMIC(CGridCellBase)

// Construction/Destruction
public:
    CGridCellBase();
    virtual ~CGridCellBase();

// Attributes
public:
    virtual void SetText(LPCTSTR /* szText */)              = 0 ;
	virtual void SetBlinkStep(int32 iBlinkStep)				= 0 ;
	virtual void SetBlinkText(LPCTSTR /*szText*/, bool32 /*bBlinkOnlyChange*/) = 0 ;
    virtual void SetImage(int /* nImage */)                 = 0 ;
    virtual void SetData(LPARAM /* lParam */)               = 0 ;
    virtual void SetState(DWORD nState)                     { m_nState = nState; }
    virtual void SetFormat(DWORD /* nFormat */)             = 0 ;
    virtual void SetTextClr(COLORREF /* clr */)             = 0 ;
    virtual void SetBackClr(COLORREF /* clr */)             = 0 ;
    virtual void SetFont(const LOGFONT* /* plf */)          = 0 ;
    virtual void SetMargin( UINT /* nMargin */)             = 0 ;
    virtual void SetGrid(CGridCtrl* /* pGrid */)            = 0 ;
    virtual void SetCoords( int /* nRow */, int /* nCol */) = 0 ;
	virtual void SetDrawMarkItem(const GV_DRAWMARK_ITEM &markItem) { m_markItem = markItem; }
	virtual void SetTipText(LPCTSTR lpszTip){ m_StrTip = lpszTip; }
	virtual void SetTipTitle(LPCTSTR lpszTipTitle){ m_StrTipTitle = lpszTipTitle; }
	virtual void SetUnderLine(BOOL bUnderLine) {m_bUnderLine = bUnderLine;}
	virtual void  SetTextPadding(CRect rtTextPadding) { m_rtTextPadding = rtTextPadding;}


	virtual LPCTSTR    GetText()       const                = 0 ;
	virtual int32      GetBlinkStep()  const				= 0 ;
    virtual LPCTSTR    GetTipText()    const                { return GetText(); } // may override TitleTip return
	virtual LPCTSTR    GetTipTitle()   const                { return m_StrTipTitle; } // may override TitleTip return
    virtual int        GetImage()      const                = 0 ;
    virtual LPARAM     GetData()       const                = 0 ;
    virtual DWORD      GetState()      const                { return m_nState;  }
    virtual DWORD      GetFormat()     const                = 0 ;
    virtual COLORREF   GetTextClr()    const                = 0 ;
    virtual COLORREF   GetBackClr()    const                = 0 ;
    virtual LOGFONT  * GetFont()       const                = 0 ;
    virtual CFont    * GetFontObject() const                = 0 ;
    virtual CGridCtrl* GetGrid()       const                = 0 ;
    virtual CWnd     * GetEditWnd()    const                = 0 ;
    virtual UINT       GetMargin()     const                = 0 ;
	virtual CRect	   GetTextPadding() {return  m_rtTextPadding;}

	virtual const GV_DRAWMARK_ITEM &GetDrawMarkItem() const { return m_markItem; }
	virtual BOOL		InitDrawMarkFont(OUT CFont &FontMark);		// ��ʼ�������������

    virtual CGridCellBase* GetDefaultCell() const;

    virtual BOOL IsDefaultFont()       const                = 0 ;
    virtual BOOL IsEditing()           const                = 0 ;
    virtual BOOL IsFocused()           const                { return (m_nState & GVIS_FOCUSED);  }
    virtual BOOL IsFixed()             const                { return (m_nState & GVIS_FIXED);    }
    virtual BOOL IsFixedCol()          const                { return (m_nState & GVIS_FIXEDCOL); }
    virtual BOOL IsFixedRow()          const                { return (m_nState & GVIS_FIXEDROW); }
    virtual BOOL IsSelected()          const                { return (m_nState & GVIS_SELECTED); }
    virtual BOOL IsReadOnly()          const                { return (m_nState & GVIS_READONLY); }
    virtual BOOL IsModified()          const                { return (m_nState & GVIS_MODIFIED); }
    virtual BOOL IsDropHighlighted()   const                { return (m_nState & GVIS_DROPHILITED); }

// Operators
public:
    virtual void operator=(const CGridCellBase& cell);

public:
	// ��ȡƽ��Ч������ɫ
	// iStep �� 1��ʼ��ţ� iCount����Ϊ3
	static void GetSmoothColor(COLORREF ClrStart, COLORREF ClrEnd, int iStep, int iCount, COLORREF &ClrSmooth)
	{
		if (iCount < 3 || iStep < 1 || iStep > iCount)
		{
			ClrSmooth = ClrStart;
			return;
		}
		
		if (1 == iStep)	
		{
			ClrSmooth = ClrStart;
			return;
		}
		else if (iCount == iStep)
		{
			ClrSmooth = ClrEnd;
			return;
		}
		else
		{
			int iRStart	= (ClrStart & 0xff);
			int iGStart	= ((ClrStart >> 8) & 0xff);
			int iBStart	= ((ClrStart >> 16) & 0xff);
		
			int iREnd	= (ClrEnd & 0xff);
			int iGEnd	= ((ClrEnd >> 8) & 0xff);
			int iBEnd	= ((ClrEnd >> 16) & 0xff);

			int iRStep	= (iREnd - iRStart) / (iCount - 1);
			int iGStep	= (iGEnd - iGStart) / (iCount - 1);
			int iBStep	= (iBEnd - iBStart) / (iCount - 1);

			int iRSmooth = iRStart;
			int iGSmooth = iGStart;
			int iBSmooth = iBStart;
			for (int i = 1; i < iStep; i++)
			{
				iRSmooth += iRStep;
				iGSmooth += iGStep;
				iBSmooth += iBStep;
			}

			ClrSmooth = RGB(iRSmooth, iGSmooth, iBSmooth);
		}		
	}

// Operations
public:
    virtual void Reset();

    virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
    virtual BOOL GetTextRect( LPRECT pRect);    // i/o:  i=dims of cell rect; o=dims of text rect
    virtual BOOL GetTipTextRect( LPRECT pRect) { return GetTextRect( pRect); }  // may override for btns, etc.
    virtual CSize GetTextExtent(LPCTSTR str, CDC* pDC = NULL);
    virtual CSize GetCellExtent(CDC* pDC);
	virtual CSize GetDrawMarkTextExtent(CDC *pDC);	// ��ȡ������ֵĳ���

    // Editing
    virtual BOOL Edit( int /* nRow */, int /* nCol */, CRect /* rect */, CPoint /* point */, 
                       UINT /* nID */, UINT /* nChar */) { ASSERT( FALSE); return FALSE;}
	virtual BOOL ValidateEdit(LPCTSTR str);
    virtual void EndEdit() {}

    // EFW - Added to print cells properly
    virtual BOOL PrintCell(CDC* pDC, int nRow, int nCol, CRect rect);

    // add additional protected grid members required of cells
    LRESULT SendMessageToParent(int nRow, int nCol, int nMessage);

	float	GetCompareFloat();
	void	SetCompareFloat(float f);

	void	SetOwnCursor(HCURSOR hCursor) { m_hCursor = hCursor; }

	bool	IsVSeparatoLine() { return m_bVSeparatoLine; }
	void	SetVSeparatoLine(bool bVSeparatoLine) { m_bVSeparatoLine = bVSeparatoLine; }

	bool	IsHSeparatoLine() { return m_bHSeparatoLine; }
	void	SetHSeparatoLine(bool bHSeparatoLine) { m_bHSeparatoLine = bHSeparatoLine; }

protected:
    virtual void OnEndEdit();
    virtual void OnMouseEnter();
    virtual void OnMouseOver();
    virtual void OnMouseLeave();
    virtual void OnClick( CPoint PointCellRelative);
    virtual void OnClickDown( CPoint PointCellRelative);
    virtual void OnRClick( CPoint PointCellRelative);
    virtual void OnDblClick( CPoint PointCellRelative);
    virtual BOOL OnSetCursor();

protected:
    DWORD			m_nState;      // Cell state (selected/focus etc)
	int				m_iBlinkStep;
	float			m_fCompare;
	GV_DRAWMARK_ITEM	m_markItem;	// �������, (&��ʾ��ȫ�����ڻ�����~)
	CString			m_StrTip;		// ��ʾ
	CString			m_StrTipTitle;	// ��ʾ����
	HCURSOR			m_hCursor;		// �Զ���������״
	BOOL		m_bUnderLine;		// �Ƿ�Ҫ�����»���
	CFont		m_font;
	bool		m_bVSeparatoLine;	// �Ƿ�Ҫ�Ӵ�ֱ�ָ���
	bool		m_bHSeparatoLine;	// �Ƿ�Ҫ��ˮƽ�ָ���
	CRect	    m_rtTextPadding;	// text arround border
};




//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDCELLBASE_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_)
