#ifndef _CONTROL_BASE_H_
#define _CONTROL_BASE_H_
#include "dllexport.h"
/////////////////////////////////////////////////////////////////////////////
// CControlBase.h
// ������Ϊϵͳ�����пؼ����࣬ ���𽫰�����Ϣ���ϴ��ͣ� ����ͨ�������Ľӿ����϶��µ�ͳһ������

class CONTROL_EXPORT CControlBase : public CStatic
{
	DECLARE_DYNAMIC(CControlBase)

// Construction
public:
	CControlBase();
	virtual ~CControlBase();
	BOOL CreateFromStatic(DWORD dwStyle,
						  CWnd* pParentWnd,
						  UINT nIdStatic,
						  UINT nId);

	BOOL Create(DWORD dwStyle,
						 CWnd* pParentWnd,
						 LPRECT lpRect,
						 UINT nId);

	
public:
	virtual	BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual	BOOL	TestChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual	BOOL	TestKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);


protected:
	CWnd *		m_pParent;				// control parent 
public:	
	CRect		m_rectClient;			// control client rect


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlBase)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL


	// Generated message map functions
protected:
	//{{AFX_MSG(CControlBase)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_CONTROL_BASE_H_
