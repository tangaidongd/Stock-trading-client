#if !defined(AFX_DLGREMOVEFROMBLOCK_H_)
#define AFX_DLGREMOVEFROMBLOCK_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgremovefromblock.h : header file
//

#include "DialogEx.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgRemoveFromBlock dialog

// ����ѡ�����ɾ���Ի���
class CMerch;

class CDlgRemoveFromBlock : public CDialogEx
{
public:
	
	static bool32 RemoveFromBlock(CMerch *pMerchDel, OUT CStringArray &aBlockNames); // �����Ի��� ѡ��Ҫ����Щ�����ɾ������Ʒ
	
private:
	
	// Construction
protected:
	CDlgRemoveFromBlock(CWnd* pParent = NULL);   // standard constructor
	
	void	GetCheckBlocks();
	void	CheckAllBlocks(bool32 bCheck);

	int		FillList();		// ����б�

	CStringArray	m_aBlockNames;
	CMerch			*m_pMerchDel;
	
	// Dialog Data
	//{{AFX_DATA(CDlgRemoveFromBlock)
	enum { IDD = IDD_DIALOG_REMOVEFROMBLOCK };
	CListCtrl	  m_List;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRemoveFromBlock)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgRemoveFromBlock)
	afx_msg void OnBtnCheckAll();
	afx_msg void OnBtnUnCheckAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGREMOVEFROMBLOCK_H_)