#pragma once
#include "afxcmn.h"


// CDlgUserBlockAddMerch dialog

class CDlgUserBlockAddMerch : public CDialogEx
{
	//DECLARE_DYNAMIC(CDlgUserBlockAddMerch)

public:
	CDlgUserBlockAddMerch(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgUserBlockAddMerch();

// Dialog Data
	enum { IDD = IDD_DIALOG_BLOCK_ADD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	void InitUserBlock();


	CImageList			m_ImageList;

public:
	afx_msg void OnBnClickedButtonAddMerch();
	afx_msg LRESULT OnMsgHotKey(WPARAM wParam, LPARAM lParam);
	CListCtrl m_List;
	afx_msg void OnBnClickedButtonDelMerch();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnLvnItemchangedListMerch(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
};
