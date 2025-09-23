/*********************************************************
* Splitter Window Extension
* Version: 1.0
* Date: March 10, 2003
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
* Copyright (C) 2003 by Michal Mecinski
*********************************************************/

#pragma once
#include "dllexport.h"

class CONTROL_EXPORT CBarSplitWnd : public CSplitterWnd
{
	DECLARE_DYNAMIC(CBarSplitWnd);
public:
	CBarSplitWnd();
	virtual ~CBarSplitWnd();

public:
	// pane that should be constant and its size
	void SetBarInfo(int nPane, int nSize);

protected:
	virtual int HitTest(CPoint pt) const;
	virtual void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect);

protected:
	int m_nBarPane;
	int m_nBarSize;

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};
