#if !defined(_DLGARBIFORMULAR_H_)
#define _DLGARBIFORMULAR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgarbiformular.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgArbiFormular dialog
#include "DialogEx.h"
#include "ArbitrageFormula.h"
#include "ArbitrageManage.h"
#include <map>
using std::map;

class CMerch;

class CDlgArbiFormular : public CDialogEx
{
	typedef map<CString, CMerch *>	MerchMap;
public:
	CString		GetFormular();
	void		GetFormula(OUT CArbitrageFormula &formula){ formula=m_formula; };
	void		GetFormulaOtherMerchs(OUT CArbitrage::ArbMerchArray &aMerchs);
	void		SetFormular(CMerch* pMerch, CArbitrageFormula* pFormular, const CArbitrage::ArbMerchArray &aOtherMerch);

	static CMerch		*GetMerchByVarName(const CString &StrVar);

private:
	CMerch*		m_pMerch;
	CArbitrageFormula	m_formula;
	CArbitrage::ArbMerchArray m_aOtherMerch;

	static MerchMap	m_smapUserInsertedMerch;

	bool32		CheckFormula(CArbitrageFormula &formula);

// Construction
public:
	CDlgArbiFormular(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgArbiFormular)
	enum { IDD = IDD_DIALOG_ARBITRAGE_FORMULAR };
	CString	m_StrFormular;
	CEdit	m_edtFormula;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgArbiFormular)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgArbiFormular)
	afx_msg void OnButtonValid();
	afx_msg void OnButtonInsertOtherMerch();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DLGARBIFORMULAR_H_)
