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

	// ���õ�Ԫ���ͼƬ��uImgIDͼƬ��ԴID��uCntͼƬ״̬������ uID���ڰ�ť������Ӧ�¼���
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

	CNCButton	m_btnCell;			// ��Ԫ���еİ�ť
	int			m_iBtnHovering;		// ��ʶ�����밴ť����
	Image		*m_pImgBtn;			// ��ť��ͼƬ
	UINT		m_uImgCnt;			// ͼƬ״̬�ĸ���
	CRect		m_btnRect;			// ��ť������

};

#endif //_GRID_CELL_IMAGE_H_
