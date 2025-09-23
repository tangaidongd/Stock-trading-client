#pragma once
#include "afxcmn.h"
#include "PubImportUserBlock.h"


// CDlgImpUserBlockDetails dialog

class CDlgImpUserBlockDetails : public  CDialogEx
{
	DECLARE_DYNAMIC(CDlgImpUserBlockDetails)

public:
	CDlgImpUserBlockDetails(CUserBlockData* pUserBlockData, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgImpUserBlockDetails();

// Dialog Data
	enum { IDD = IDD_DIALOG_IMP_USERBLOCK_DISPLAY };

public:
	virtual BOOL OnInitDialog();
	//void SetmModeless(bool bModeless);
	//bool	m_bModeless;
	void	SetDefaultBlockData(CUserBlockData* pUserBlockData);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//virtual void OnCancel();
	//virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()

private:
	CListCtrl m_listDetails;

private:
	void  BulidUserBlockData();

	CUserBlockData* m_pUserBlockData;
public:
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1();
};
