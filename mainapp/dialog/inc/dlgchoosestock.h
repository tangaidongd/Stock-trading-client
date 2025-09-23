#if !defined(AFX_DLGCHOOSESTOCK_H__)
#define AFX_DLGCHOOSESTOCK_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgchoosestock.h : header file
//
#include "DialogEx.h"
#include "CFormularContent.h"
#include <set>
using namespace std;

#include "BlockManager.h"
#include "SelectStockCenter.h"
#include "TextProgressCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgChooseStock dialog
//////////////////////////////////////////////////////////////////////////
class CDlgChooseStock : public CDialogEx
{
private:	
	// ����ѡ�ɹ�ʽ��Ͽ�
    void			FillComBoxFormular();

	// ����ѡ���
	void			FillComBoxCycle();

	// ������Ʒ�б��
	void			FillListBox();

	// ���ڿؼ�
	void			FillDateEdit();

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

	// ���ò���
	void			SetRadioParam(int32 iIndex);

	// �ı������, �޸�ָ�깫ʽ
	void			ModifyFormular(int32 iIndexParam);			

private:
	// �Ƿ��ڹ����ı�־.
	bool32					m_bWorking;

	// ָ�깫ʽ����
	CFormulaLib*			m_pFormulaLib;

	// ѡ������
	CSelectStockCenter*		m_pSelectStockCenter;

	// ��ǰ�Ĺ�ʽ:
	CFormularContent*		m_pFomularNow;

	// �뻹�ǻ� ����	
	bool32					m_bAndCondition;												

	// ��Ʊ����ѡ��İ��
	T_Block					m_BlockFinal;

	// �μ�ѡ�ɵ���Ʒ��Χ
	set<CMerch*, MerchCmp>	m_aMerchToChoose;			

	// ����ȷ���Ĳμ�ѡ�ɵĹ�ʽ
	set<CFormularContent*, FormularCmp> m_aFormularsFinal;

	// Construction
public:
    CDlgChooseStock(CWnd* pParent = NULL);   // standard constructor
	~CDlgChooseStock();
	// Dialog Data
    //{{AFX_DATA(CDlgChooseStock)
	enum { IDD = IDD_DIALOG_SELECT_STOCK };
	CTextProgressCtrl	m_Progress;
	CButton	m_CheckP4;
	CButton	m_CheckP3;
	CButton	m_CheckP2;
	CButton	m_CheckP1;
    CComboBox    m_ComboCondition;
	CComboBox	 m_ComboCycle;
    CListBox     m_ListCondition;
	CListBox	 m_ListMerchAll;	
    int            m_iTotal;
    int            m_iResult;
    BOOL           m_bHistory;	
	CString	m_StrNameP1;
	CString	m_StrNameP2;
	CString	m_StrNameP3;
	CString	m_StrNameP4;
	CTime	m_TimeBegin;
	CTime	m_TimeEnd;
	float	m_fP1;
	float	m_fP2;
	float	m_fP3;
	float	m_fP4;
	CString	m_StrRangeP1;
	CString	m_StrRangeP2;
	CString	m_StrRangeP3;
	CString	m_StrRangeP4;
	int		m_iRadio;
	//}}AFX_DATA
	
	
	// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDlgChooseStock)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL
	
	// Implementation
protected:
	
    // Generated message map functions
    //{{AFX_MSG(CDlgChooseStock)
    afx_msg void OnButtonPrompt();
    afx_msg void OnButtonAdd();
    afx_msg void OnButtonDel();
    afx_msg void OnButtonAddto();
    afx_msg void OnButtonDo();
	afx_msg void OnButtonRight();
	afx_msg void OnButtonLeft();
	afx_msg void OnSelchangeComboCondition();
	afx_msg void OnCheck();
	afx_msg void OnRadioAnd();
	afx_msg void OnRadioOr();
	afx_msg void OnButtonChangemerch();	
	afx_msg void OnRadioP1();
	afx_msg void OnRadioP2();
	afx_msg void OnRadioP3();
	afx_msg void OnRadioP4();
	afx_msg void OnButtonDelMerch();
	afx_msg	LRESULT OnMsgProgress(WPARAM wParam , LPARAM lParam);
	afx_msg void OnChangeEditParam();
	afx_msg void OnChangeEditParam2();
	afx_msg void OnChangeEditParam3();
	afx_msg void OnChangeEditParam4();
	afx_msg void OnClose();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHOOSESTOCK_H__)
