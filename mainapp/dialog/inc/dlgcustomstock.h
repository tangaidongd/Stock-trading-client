#if !defined(_DLGCUSTOMSTOCK_H_)
#define _DLGCUSTOMSTOCK_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgcustomstock.h : header file
//

#include "DialogEx.h"
#include "ReportScheme.h"
#include "SelectStockCenter.h"
#include "BlockManager.h"
#include "TextProgressCtrl.h"
#include <set>
#include <vector>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CDlgCustomStock dialog
class CDlgCustomStock : public CDialogEx
{
private:		
	// ����ֶ��б�ؼ�
    void			FillListValue();
	
	// ������Ʒ�б��
	void			FillListBox();

	// ���������б��
	void			ReSetListBoxCdt();

	// ��ս�����
	void			ReSetProgress();
	
	// ѡ�ɿ�ʼ
	bool32			BeginChooseStock();																	
	
	// ֹͣѡ��(����ֹͣ)
	bool32			StopChooseStock();
	
	// ѡ�����(������ѡ�����ܵ�֪ͨ)
	void			OnFinishChooseStock();
	
	// ��֤�����Ϸ���
	bool32			ValidChooseParams(OUT CString& StrErr);

private:
	// �Ƿ��ڹ����ı�־.
	bool32					m_bWorking;
	
	// ָ�깫ʽ����
	CFormulaLib*			m_pFormulaLib;
	
	// ѡ������
	CSelectStockCenter*		m_pSelectStockCenter;

	// ��Ʊ����ѡ��İ��
	T_Block					m_BlockFinal;
	
	// �μ�ѡ�ɵ���Ʒ��Χ
	set<CMerch*, MerchCmp>	m_aMerchToChoose;			
	
	// ����ȷ���Ĳμ�ѡ�ɵĸ�������
	CArray<T_CustomCdtCell, T_CustomCdtCell&>	m_aCustomConditions;

	// ͼ��
	CImageList				m_ImageList;

// Construction
public:
	CDlgCustomStock(CWnd* pParent = NULL);   // standard constructor
	~CDlgCustomStock();
// Dialog Data
	//{{AFX_DATA(CDlgCustomStock)
	enum { IDD = IDD_DIALOG_CUSTOM_STOCK };
	CTextProgressCtrl	m_Progress;
	CListCtrl	m_ListValues;
	CListBox	m_ListMerchs;
	CListBox	m_ListCondition;
	UINT	m_iMerchAll;
	UINT	m_iMerchChoose;
	int		m_iRadioAnd;
	int		m_iRadioValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCustomStock)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCustomStock)
	afx_msg void OnRadioValue();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonModify();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonChangeMerch();
	afx_msg void OnButtonDelAllMerch();
	afx_msg void OnButtonAddTo();
	afx_msg void OnButtonDo();	
	afx_msg void OnDblclkListCondition();
	afx_msg void OnDblclkListValue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg	LRESULT OnMsgProgress(WPARAM wParam , LPARAM lParam);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DLGCUSTOMSTOCK_H_)
