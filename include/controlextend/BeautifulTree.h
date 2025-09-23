#if !defined(AFX_BEAUTIFULTREE_H__356EF6F8_65CD_4D45_BD18_8570A707E3EF__INCLUDED_)
#define AFX_BEAUTIFULTREE_H__356EF6F8_65CD_4D45_BD18_8570A707E3EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BeautifulTree.h : header file

#include "GdiPlusTS.h"
#include <vector>
#include "dllexport.h"

using namespace std;
//
struct itemPicture
{
	Image *pImageBlack;			// 黑白图片
	Image *pImageColor;			// 彩色图片

	itemPicture(){pImageBlack = NULL; pImageColor = NULL;}
};

enum E_BTreeStyle
{
	EBT_Style1 = 0, 	// 实盘交易
	EBT_Style2,			// 模拟交易
};

/////////////////////////////////////////////////////////////////////////////
// CBeautifulTree window

class CONTROL_EXPORT CBeautifulTree : public CTreeCtrl
{
// Construction
public:
	CBeautifulTree();
	virtual ~CBeautifulTree();
	void ToggleNode();
	void ExpandAllNode();	// 展开所有有结点的分支
	HTREEITEM InsertItem(LPCTSTR lpszItem,UINT idBlack,UINT idColor,HTREEITEM hParent=TVI_ROOT,HTREEITEM hInsertAfter=TVI_LAST);
	BOOL SelectItem(int iIndex);	

	void SetTreeStyle(int iStyle);
private:
	int m_iTreeStyle;
	
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBeautifulTree)
	protected:
	//}}AFX_VIRTUAL
private:
	void	Calculate();	//计算
	void	DrawTreeItem( CDC* pDC );	//绘制树的项
	void	DrawGradient(Graphics& graphics, COLORREF Color1, COLORREF Color2, CRect rect, int iRotation);
	void	DrawItemText( CRect rect, HTREEITEM hItem, CDC *pDc = NULL );
	void	LoadExpandItemImage();	// 加载可展开项标识的图片
	int		AddItemPicture(UINT idBlack, UINT idColor);
private:
	Image	*m_pImageExpandBlack;	// 展开图标黑白
	Image	*m_pImageExpandColor;	// 展开图标彩色
	Image	*m_pImageFoldBlack;		// 折叠图标黑白
	Image	*m_pImageFoldColor;		// 折叠图标彩色
	HTREEITEM	m_hItemSelect;		//当前选中的项目句柄
	HTREEITEM	m_hItemMouseMove;	//当前鼠标所在的句柄
	BYTE m_IsInRegion;				//鼠标停留标识
	//itemPicture	itemExpand;			// 可展开的项使用的图结构体
	//lint -sem(vector::push_back,custodial(1))
	vector<itemPicture*> m_vItemPicture;	// 所有项的图标结构体

	CRect		m_rect;				// The client rect when drawing
	int			m_h_offset;			// 0... -x (scroll offset)
	int			m_h_size;			// width of unclipped window
	int			m_v_offset;			// 0... -y (scroll offset)
	int			m_v_size;			// height of unclipped window
	// Generated message map functions
protected:
	//{{AFX_MSG(CBeautifulTree)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BEAUTIFULTREE_H__356EF6F8_65CD_4D45_BD18_8570A707E3EF__INCLUDED_)
