#ifndef _MYGROUPBOXBUTTON_H_
#define _MYGROUPBOXBUTTON_H_

#pragma once
#include "dllexport.h"

#define WM_POSCHANGED_GROUPBOX	WM_USER+1000
// CMyButton

class CONTROL_EXPORT CMyGroupBoxButton : public CButton
{
	DECLARE_DYNAMIC(CMyGroupBoxButton)

public:
	CMyGroupBoxButton();
	virtual ~CMyGroupBoxButton();

protected:
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bHtBottom;	//拖动下边框改变大小
	int m_iID;
public:
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	void RegistrID(int iID);
};

#endif