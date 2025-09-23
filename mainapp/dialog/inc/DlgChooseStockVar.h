#if !defined(AFX_DLGCHOOSESTOCKVAR_H_)
#define AFX_DLGCHOOSESTOCKVAR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgChooseStockVar.h : header file
//

#include "DialogEx.h"
#include "BlockConfig.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgChooseStockVar dialog

// ѡ��Ʒ�ֶԻ���

class CDlgChooseStockVar : public CDialogEx
{
public:
	typedef CBlockConfig::MerchArray MerchArray;
	
	// �����Ի��� ѡ��Ҫ����Щ�����ɾ������Ʒ
	// pInputMerch���һ���������ָ����Ʒ��صİ���б�
	static bool32 ChooseStockVar(OUT MerchArray &aMerchSel, bool32 bMultiMerchSel = true, CMerch *pInputMerch = NULL, CWnd *pWndParent=NULL); 
	
private:
	
	// Construction
protected:
	CDlgChooseStockVar(CWnd* pParent = NULL);   // standard constructor
	
	void	GetSelMerchs();			// ��ȡѡ�����Ʒ

	void	SetInputMerch(CMerch *pMerch);
	
	void		FillBlockList();		// ���������б�
	void		FillMerchList();		// ����Ҳ���Ʒ�б�

	void	UpdateMerchSrcChange();	// ���ѡ��״̬���
	void	ClearMerchSrc();		// �����Ʒ�б������
	
	bool32	m_bMultiMerchSel;		// �Ƿ�����ѡ������Ʒ
	MerchArray	m_aMerchSel;		// ѡ�����Ʒ�б���

	CMerch	*m_pInputMerch;			// �������Ʒ�����½�һ���������tab
	CBlockConfig::IdArray		m_aInputBelongToBlocks; // ������Ʒ��ص�block��id

	// ����п����������г��б����߰���б�
	bool32		m_bLeftIsMarket;	// trueΪ�����г���falseΪ���
	MerchArray  m_aMerchSrc;		// �Ҳ���Ʒ�б�����Դ

	CImageList	m_ImageList;
	
	// Dialog Data
	//{{AFX_DATA(CDlgChooseStockVar)
	enum { IDD = IDD_DIALOG_CHOOSESTOCK };
	CListCtrl	  m_ListBlock;
	CListCtrl	  m_ListMerch;
	CTabCtrl	  m_Tab;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChooseStockVar)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgChooseStockVar)
	afx_msg void OnTabChanged(NMHDR *pHdr, LRESULT *pResult);			// tab�л�
	afx_msg void OnLeftListSelChanged(NMHDR *pHdr, LRESULT *pResult);	// ���ѡ����
	afx_msg void OnGetDispInfo(NMHDR *pHdr, LRESULT *pResult);
	afx_msg void OnBtnSelAll();
	afx_msg LRESULT OnMsgHotKey(WPARAM w, LPARAM l);
	afx_msg void OnRightListLDbClick(NMHDR *pHdr, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHOOSESTOCKVAR_H_)