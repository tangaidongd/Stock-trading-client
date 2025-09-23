/*********************************************************
* Splitter Window Extension
* Version: 1.3
* Date: March 6, 2003
* Author: Michal Mecinski
* E-mail: mimec@mimec.w.pl
* WWW: http://www.mimec.w.pl
*
* You may freely use and modify this code, but don't remove
* this copyright note.
*
* There is no warranty of any kind, express or implied, for this class.
* The author does not take the responsibility for any damage
* resulting from the use of it.
*
* Let me know if you find this code useful, and
* send me any modifications and bug reports.
*
* Please include the copyright for this code
* in your application's documentation and/or about box.
*
* Copyright (C) 2002-03 by Michal Mecinski
*********************************************************/

#pragma once

#include "tinyxml.h"
#include "GGTongView.h"

const double KdF9Radion = 0.99999999;

class CDualSplitWnd : public CSplitterWnd
{
public:
	enum E_DelChildsplit
	{
		EDCNONE = 0,
		EDCLEFT,
		EDCRIGHT,
		EDCTOP,
		EDCBOTTOM,
		EDCCOUNTS
	};
	DECLARE_DYNAMIC(CDualSplitWnd);
public:
	CDualSplitWnd();
	virtual ~CDualSplitWnd();

public:
	bool32				FromXml(TiXmlElement *pTiXmlElement);
	CString				ToXml(CGGTongView * pActiveView);

	void				SetRadio(double dradio,bool32 bF9 = false);
	double				GetRatio() const						        { return m_dRatio;  }
	double				GetF9Ratio(CWnd * pWnd) const;

	void				SetInitialRatio(double dRatio)					{ m_dRatio = dRatio;}
	void				RecalcRatio();
	
	void				DelChildSplitWnd(E_DelChildsplit eDelChildsplit);
	void				DelAllChildGGTongView(CDualSplitWnd * pDualSplitWnd);

	static const char * GetXmlElementValue();
	static const char * GetXmlElmAttrDualSplitIsH();
	static const char * GetXmlElmAttrDualSplitScale();

	virtual void		OnInvertTracker(const CRect& rect);
	virtual void		SetSplitCursor(int ht);
	virtual void		OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect);

public:
	bool32	m_bChangeingSize;

protected:
	enum HitTestValue
	{
		noHit = 0,
		vSplitterBox = 1,
		hSplitterBox = 2,
		bothSplitterBox = 3,
		vSplitterBar1 = 101,
		vSplitterBar15 = 115,
		hSplitterBar1 = 201,
		hSplitterBar15 = 215,
		splitterIntersection1 = 301,
		splitterIntersection225 = 525
	};
	virtual int			HitTest(CPoint pt) const;
	virtual void		StartTracking(int ht);
	virtual void		StopTracking(BOOL bAccept);
	int					m_nPrev;
	int					m_nTrackPos;
	BOOL				m_bChange;
	BOOL				m_bDragFull;
	double				m_dRatio;
	double				m_dF9Ratio;
	CRect				m_rectToDel;
	E_DelChildsplit		m_eDelChildSplit;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void		OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void		OnSize(UINT nType, int cx, int cy);	
	// afx_msg void		OnPaint();
};

