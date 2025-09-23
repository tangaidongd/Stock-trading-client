#ifndef _GRID_CELL_IMAGE_H_
#define _GRID_CELL_IMAGE_H_

#include "GridCell.h"
#include "NCButton.h"

class CGridCellImage : public CGridCell
{
	DECLARE_DYNCREATE(CGridCellImage)

public:
	CGridCellImage();    
	virtual ~CGridCellImage();

	enum CELL_Style{ECSImage=0,ECSAddZiXuan=1,ESCDelZiXuan=2};

	// 设置单元格的图片（uImgID图片资源ID，uCnt图片状态个数， uID用于按钮类型响应事件）
	void			SetImage(UINT uImgID, UINT uCnt=1, CELL_Style eStyle=ECSImage);	
public:
	virtual BOOL	Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
	virtual void	OnMouseOver();
	virtual void	OnClickDown( CPoint PointCellRelative);
	virtual LPCTSTR	GetText() const             { return (m_StrTip.IsEmpty())? _T("") : LPCTSTR(m_StrTip); }

private:
	void			DrawButton(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);

private:
	CELL_Style	m_eCellStyle;

	CNCButton	m_btnCell;			// 单元格中的按钮
	int			m_iBtnHovering;		// 标识鼠标进入按钮区域
	Image		*m_pImgBtn;			// 按钮的图片
	UINT		m_uImgCnt;			// 图片状态的个数
	CRect		m_btnRect;			// 按钮的区域

};

#endif //_GRID_CELL_IMAGE_H_
