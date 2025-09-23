#ifndef _DLGSYNCEXCEL_H_
#define _DLGSYNCEXCEL_H_


#include "IoViewSyncExcel.h"

class CMerch;
/////////////////////////////////////////////////////////////////////////////
// CDlgSyncExcel dialog used 

class CDlgSyncExcel : public CDialog
{
public:
	typedef CArray<CMerch *, CMerch *> MerchPtrArray;
	typedef	CIoViewSyncExcel::BlockArray BlockArray;
	CDlgSyncExcel(CWnd *pParent = NULL);
	
	BOOL	SetSyncExcel(IN const BlockArray &aBlocks, OUT BlockArray &aOutBlocks);	// 设置同步板块商品
	
	void	ShowMerchCount();

private:
	void	InitialTree();
	//void	AddMerchs(const MerchPtrArray &aMerchs);		// 添加商品到Tree和m_aMerchsSync

	BOOL	IsBlockChange(const BlockArray &aBlock1, const BlockArray &aBlock2);
	BOOL	IsMerchArrayChange(const MerchPtrArray &aMerch1, const MerchPtrArray &aMerch2);
	bool	CompareMerch(const CMerch *pMerch1, const CMerch *pMerch2);

	CString				m_StrTitle;

	BlockArray			m_aBlocksSync;
	
	// Dialog Data
	//{{AFX_DATA(CDlgSyncExcel)
	enum { IDD = IDD_DIALOG_SYNC_EXCEL };
	CTreeCtrl		m_CtrlTreeMerch;
	// CString m_StrLMTime;
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSyncExcel)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CDlgSyncExcel)
	afx_msg	void	OnTreeMerchRClick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



#endif