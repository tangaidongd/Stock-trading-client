#pragma once
#include "afxcmn.h"
#include "PubImportUserBlock.h"
#include "DlgImpUserBlockDetails.h"
#include "afxwin.h"
#include "HButton.h"
#include "CustomProgress.h"
#include "StaticEx.h"

// CImportUserBlockBySoft dialog

class CDlgImportUserBlockBySoft : public CDialog
{
	DECLARE_DYNAMIC(CDlgImportUserBlockBySoft)

public:
	CDlgImportUserBlockBySoft(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgImportUserBlockBySoft();

// Dialog Data
	enum { IDD = IDD_DIALOG_IMP_USERBLOCK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonImp();
	virtual BOOL OnInitDialog();

	void InitUserData();
	void ShowAndHideControls(bool bShow);
	void SetRunThreadStatus(bool bStatus);

	CListCtrl m_SoftList;
	// 直接用线程调用，其他的地方也不需要改
	CPubImportUserBlockFun  m_PubImportUserBlockFun;
	//CProgressCtrl m_progress;
	CCustomProgress m_CustomPregress;
	CStaticEx m_statcImpTips;


private:
	void WindowsMoveCustom();


	//CPubImportUserBlockFun  m_PubImportUserBlockFun;
	CDlgImpUserBlockDetails* m_pDlgImpDetails;
	CHButton m_btnExcelImp;
	HANDLE m_hHandle; 
	bool   m_bRunThreadStatus;


public:
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMHoverList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMove(int x, int y);
	
	
	afx_msg void OnBnClickedButtonCancel();
	
	afx_msg void OnBnClickedButtonExcelImp();
	afx_msg void OnClose();
};
