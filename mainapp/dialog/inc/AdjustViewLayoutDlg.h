#if !defined(AFX_ADJUSTVIEWLAYOUTDLG_H__ABA3384A_1838_41B6_A246_68E24D1E8A33__INCLUDED_)
#define AFX_ADJUSTVIEWLAYOUTDLG_H__ABA3384A_1838_41B6_A246_68E24D1E8A33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdjustViewLayoutDlg.h : header file
//

#include "DialogEx.h"

class CGGTongView;
class CIoViewManager;
struct T_IoViewObject;
/////////////////////////////////////////////////////////////////////////////
// CAdjustViewLayoutDlg dialog

class CAdjustViewLayoutDlg : public CDialogEx
{
// Construction
public:
	CAdjustViewLayoutDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data	
	//{{AFX_DATA(CAdjustViewLayoutDlg)
	enum { IDD = IDD_LAYOUTADJUST };
	CComboBox	m_ComboGroupId;
	CStatic	m_Static_IoviewInfo;
	CListCtrl	m_List;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdjustViewLayoutDlg)
public:
	enum E_GroupID
	{
		EGI_NONE   = 0x00000000,
		EGI_FIRST  = 0x00000001,
		EGI_SECOND = 0x00000010,
		EGI_THIRD  = 0x00000100,
		EGI_FOUR   = 0x00001000,
		EGI_FIVE   = 0x00010000,
		EGI_SIX    = 0x00000011,
		EGI_SEVEN  = 0x00000101,
		EGI_EIGHT  = 0x00000110,
		EGI_NINE   = 0x00000111,
	};

	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
public:
	void			RefreshChildFrame();
	void			SetView (CGGTongView* pView );
	void			UnSetView();
	CGGTongView*	GetView();
	void			EnableCtrl ( bool32 bEnable );		

	void			AddIoView(const T_IoViewObject* pIoViewObject);
	void			DelIoView(const CString& StrXmlName);	

	void			AddIoViews(int32 iIndex);
	void			DelIoViews(int32 iIndex);

	void			DelAllIoViews();
private:
	CGGTongView		 *m_pView;
	bool32			 m_bClickSetCheck;
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAdjustViewLayoutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnButtonZsplit();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnButtonHsplit();
	afx_msg void OnButtonSave();
	afx_msg void OnRadioTop();
	afx_msg void OnRadioLeft();
	afx_msg void OnRadioBottom();
	afx_msg void OnRadioRight();
	afx_msg void OnRadioNone();
	afx_msg void OnItemchangedListIoview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonDel();
	afx_msg void OnButtonAddtab();
	afx_msg void OnButtonDeltab();
	afx_msg void OnCheckChoose();
	afx_msg void OnItemchangingListIoview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonImport();
	afx_msg void OnButtonRight();
	afx_msg void OnButtonLeft();
	afx_msg void OnButtonTabCtrl();
	afx_msg void OnSelchangeComboGroup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADJUSTVIEWLAYOUTDLG_H__ABA3384A_1838_41B6_A246_68E24D1E8A33__INCLUDED_)
