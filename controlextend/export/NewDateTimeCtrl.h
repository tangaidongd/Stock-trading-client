#pragma once

#include "dllexport.h"

class CONTROL_EXPORT CNewDateTimeCtrl : public CDateTimeCtrl
{
	DECLARE_DYNAMIC(CNewDateTimeCtrl)

public:
	CNewDateTimeCtrl();
	virtual ~CNewDateTimeCtrl();

	void	UpdateRect(void);

private:
	BOOL	m_bOver;
	CBitmap m_bmapArrow;
	CFont	m_font;

protected:
	DECLARE_MESSAGE_MAP()
	void OnDatetimeChange(NMHDR*   pNMHDR,   LRESULT*   pResult);

	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave( WPARAM wParam,LPARAM lParam );
};


