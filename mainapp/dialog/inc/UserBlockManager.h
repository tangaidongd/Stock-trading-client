#if !defined(AFX_USERBLOCKMANAGER_H__2FABE360_B58E_474F_B5D4_1394077E0F15__INCLUDED_)
#define AFX_USERBLOCKMANAGER_H__2FABE360_B58E_474F_B5D4_1394077E0F15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserBlockManager.h : header file
//

#include "XTipWnd.h"
#include "ShareFun.h"
#include "Region.h"
#include "ChartRegion.h"
//#include "MerchBlocks.h"

#include "DialogEx.h"

class CMerch;

/////////////////////////////////////////////////////////////////////////////
// CDlgUserBlockManager dialog

const char* GetUserBlockFileName();

extern const char* KStrElementRootName;
extern const char* KStrElementMerchName;
extern const char* KStrElementMerchAttriMarket;
extern const char* KStrElementMerchAttriCode;

extern const char* KStrElementBlockName;
extern const char* KStrElementBlockAttriName;
extern const char* KStrElementBlockAttriHotKey;
extern const char* KStrElementBlockAttriServer;

extern const char* KStrElementBlockAttriColor;
extern const char* KStrElementBlockAttriHead;

//Breed
extern const int32 KBreedImageId;
extern const int32 KBreedImageSelectId;

//Market
extern const int32 KMarketImageId;
extern const int32 KMarketImageSelectId;

//UserBlock
extern const int32 KUserBlockImageId;
extern const int32 KUserBlockImageSelectId;

//Merch
extern const int32 KMerchImageId;
extern const int32 KMerchImageSelectId;

//Group1
extern const int32 KFormulaGroup1ImageId;
extern const int32 KFormulaGroup1ImageSelectId;

//Group2
extern const int32 KFormulaGroup2ImageId;
extern const int32 KFormulaGroup2ImageSelectId;

//Formula
extern const int32 KFormulaImageId;
extern const int32 KFormulaImageSelectId;

//Formula Protected
extern const int32 KFormulaProtectedImageId;
extern const int32 KFormulaProtectedImageSelectId;

class CDlgUserBlockManager : public CDialogEx
{
// Construction
public:
	CDlgUserBlockManager(CWnd* pParent = NULL);   // standard constructor
	void			InitialShow();
// Dialog Data
	//{{AFX_DATA(CDlgUserBlockManager)
	enum { IDD = IDD_DIALOG_BLOCK_MAG };
	CListCtrl	m_List;
	CTreeCtrl	m_Tree;
	//}}AFX_DATA

	CImageList			m_ImageList;
	CBindHotkey			m_BindHotkey;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgUserBlockManager)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgUserBlockManager)
	afx_msg void OnButtonAddBlock();
	afx_msg void OnButtonDelBlock();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonEditBlock();
	afx_msg void OnSelchangedTreeBlock(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnMsgHotKey(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnButtonAddMerch();
	afx_msg void OnButtonUp();
	afx_msg	void OnButtonDown();
	afx_msg void OnButtonDelMerch();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CToolbarMC : public CToolBar
{
// Construction
public:
	CToolbarMC();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolbarMC)
	//}}AFX_VIRTUAL

	BOOL CreateEx2(CWnd* pParentWnd, DWORD dwCtrlStyle = TBSTYLE_FLAT,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP,
		CRect rcBorders = CRect(0, 0, 0, 0),
		UINT nID = AFX_IDW_TOOLBAR);

	void CToolbarMC::SetColumns(UINT nColumns);
// Implementation
public:
	virtual ~CToolbarMC();

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolbarMC)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

void ShowSelfDrawBar ();
void UpdateSelfDrawBar();
class CDlgSDBar : public CDialogEx
{
// Construction
public:
	void ResetButton(bool32 bLayout);
	CDlgSDBar(CWnd* pParent = NULL);   // standard constructor
private:
	CToolbarMC	m_Toolbar;
	CXTipWnd	m_TipWnd;
// Dialog Data
	//{{AFX_DATA(CDlgSDBar)
	enum { IDD = IDD_DIALOG_SELFDRAW };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSDBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	std::map<int, CNCButton> m_mapBtn;
	int			m_iXButtonHovering;			// 标识鼠标进入按钮区域
	CPoint		m_PointLast;

private:
	void			AddButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	int				TButtonHitTest(CPoint point);	// 鼠标是否在按钮上
    void            InitBtns();
	void            DrawButton();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
public:
	E_SelfDrawType m_eSelfDrawType;
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSDBar)
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERBLOCKMANAGER_H__2FABE360_B58E_474F_B5D4_1394077E0F15__INCLUDED_)
