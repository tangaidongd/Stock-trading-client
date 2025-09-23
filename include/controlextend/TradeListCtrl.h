#pragma once
#include "dllexport.h"
#include <map>
using std::map;

typedef map<int, COLORREF> ColumnColorMap;
struct T_TradeListRowColor 
{
	//DWORD_PTR		m_dwItemData;		// itemdata
	COLORREF		m_clrRow;			// ��ȱʡcolor
	ColumnColorMap	m_mapColClr;		// ÿ�е������õ���ɫ

	T_TradeListRowColor()
	{
		//m_dwItemData = 0;
		m_clrRow = CLR_DEFAULT;
	}
};

// CTradeHeaderCtrl
class CONTROL_EXPORT CTradeHeaderCtrl : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CTradeHeaderCtrl)

public:
	CTradeHeaderCtrl();
	virtual ~CTradeHeaderCtrl();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnHdmLayout(WPARAM w, LPARAM l);
};

// CTradeListCtrl

class CONTROL_EXPORT CTradeListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CTradeListCtrl)

public:
	CTradeListCtrl();
	virtual ~CTradeListCtrl();

	CTradeHeaderCtrl &GetTraderHeaderCtrl() { return m_wndTradeHeader; }

	//
	virtual	BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DrawItem(/*_In_*/ LPDRAWITEMSTRUCT lpDrawItemStruct);

	void	MeasureItem(LPMEASUREITEMSTRUCT lpItem);
	
	void	SetRowHeight(UINT nRowHeight) { m_nRowHeight = nRowHeight; };
	UINT	GetRowHeight() const { return m_nRowHeight; }

	// �������кŰ�
// 	void		SetSingleRowBkColor(int iRow, COLORREF clrBk);
// 	void		SetSingleRowTextColor(int iRow, COLORREF clrText);
// 	COLORREF	GetSingleRowBkColor(int iRow) const;
// 	COLORREF	GetSingleRowTextColor(int iRow) const; 

	// ��itemData��
	void		SetRowDataBkColor(DWORD_PTR dwItemData, COLORREF clrBk);
	void		SetRowDataTextColor(DWORD_PTR dwItemData, COLORREF clrText);
	COLORREF	GetRowDataBkColor(DWORD_PTR dwItemData, int iRowNow) const;
	COLORREF	GetRowDataTextColor(DWORD_PTR dwItemData, int iRowNow) const; 
	void		SetRowDataColBkColor(DWORD_PTR dwItemData, int iCol, COLORREF clrBk);
	void		SetRowDataColTextColor(DWORD_PTR dwItemData, int iCol, COLORREF clrText);
	COLORREF	GetRowDataColBkColor(DWORD_PTR dwItemData, int iCol, int iRowNow) const;
	COLORREF	GetRowDataColTextColor(DWORD_PTR dwItemData, int iCol, int iRowNow) const;
	void		RemoveRowDataBkColor(DWORD_PTR dwItemData);
	void		RemoveRowDataTextColor(DWORD_PTR dwItemData);

	BOOL		IsAlterBkColor() const { return m_bAlterBkColor; }
	void		SetAlterBkColor(BOOL bAlter) { m_bAlterBkColor = bAlter; }

	BOOL		IsTextEllipsis() const { return m_bTextEllipsis; }
	void		SetTextEllipsis(BOOL bEllipsis) { m_bTextEllipsis = bEllipsis; }

	BOOL		UnionColumn(DWORD dwRowData, int iStartColumn, int iUnionCount);		// dwRowData��־���� iStartColumn�п�ʼ��iUnionCount�кϲ�Ϊһ�У�ֻ��iStartColumn�����ݻ����
	BOOL		UnUnionColumn(DWORD dwRowData, int iStartColumn);						// ȡ��dwRowData��־���� ��iStartColumn�п�ʼ�ĺϲ���
	BOOL		UnUnionColumn(DWORD dwRowData);										// ȡ��dwRowData��־�������кϲ���

	COLORREF	GetGradiantColor(COLORREF clr) const;

	BOOL		IsDrawGridLines() const { return m_bDrawGridLines; }
	void		SetDrawGridLines( BOOL bDraw ) { m_bDrawGridLines = bDraw; }

private:

protected:
	CTradeHeaderCtrl	m_wndTradeHeader;
	UINT				m_nRowHeight;
	typedef	map<DWORD_PTR, T_TradeListRowColor>	RowItemDataColorMap;
	RowItemDataColorMap			m_mapRowDataBkClr;
	RowItemDataColorMap			m_mapRowDataTextClr;

// 	typedef	CMap<int, int, COLORREF, COLORREF> RowColorMap;
// 	RowColorMap			m_mapClrRowBk;
// 	RowColorMap			m_mapClrRowText;
	BOOL				m_bAlterBkColor;		// ���汳��ɫ
	BOOL				m_bTextEllipsis;		// ��ʾ������ʱ���Ƿ��á�������������ʾ
	// row n -> [1,2] [3,2]
	typedef map<int, int>	UnionColMap;
	typedef map<DWORD, UnionColMap> UnionRowDataMap;
	UnionRowDataMap			m_mapUnionRow;

	BOOL				m_bDrawGridLines;		// ����������Ժ󣬾��Բ�Ҫ����LVS_EX_GRIDLINES

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnLvnDeleteItem(NMHDR *pHdr, LRESULT *pResult);
	afx_msg BOOL OnLvnDeleteAllItem(NMHDR *pHdr, LRESULT *pResult);
	afx_msg BOOL OnLvnInsertItem(NMHDR *pHdr, LRESULT *pResult);
	afx_msg LRESULT OnGetHeaderCtrl(WPARAM w, LPARAM l);
	afx_msg void OnPaint();
};


