#if !defined(AFX_DLGFORMULARMANAGER_H_)
#define AFX_DLGFORMULARMANAGER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgformularmanager.h : header file
//

#include "resource.h"
#include "CFormularContent.h"
#include "DialogEx.h"

#define FomularPageNums	4

/////////////////////////////////////////////////////////////////////////////
// CDlgFormularManager dialog

extern const int KiImageClrKLineSys;
extern const int KiImageClrKLineUser;
extern const int KiImageClrKLineSysLock;
extern const int KiImageClrKLineUserLock;

extern const int KiImageIndexTradeChoseGroup1;
extern const int KiImageIndexTradeChoseSys;
extern const int KiImageIndexTradeChoseUser;
extern const int KiImageIndexTradeChoseSysLock;
extern const int KiImageIndexTradeChoseUserLock;

class CDlgFormularManager : public CDialogEx
{
public:
	// ȫ������װ��
	void				ReBuildAll();
	
	// �����б��ʱ��, ����������. �ֱ��ϵͳָ����û�ָ����ȡ�������͵Ĳ���
	void				InsertToListByType(bool32 bUser, IN E_FormularType eType, OUT int32& iCurID);

	// ��ʼ�� "ȫ��" �б�
	void				BuildListAll();

	// ��ʼ�� "�û�" �б�
	void				BuildListUser();

	void				InsertToTreeByType(bool32 bSys, IN E_FormularType eType, OUT HTREEITEM& hItem);

	// ��ʼ�� "����" ��
	void				BuildTreeGroup();

	// ��ʼ�� "ϵͳ" ��
	void				BuildTreeSys();
	 
	// ����ѡ�еĲ�ͬ���ڵ�, �ı䰴ť״̬
	void				UpdateButton(CTreeCtrl* pTree, HTREEITEM hItem);
	
	// ����ѡ�еĲ�ͬ�б�ڵ�, �ı䰴ť״̬
	void				UpdateButton(CListCtrl* pList, int32 iItem);

	// 
	void				UpdateControls();

	//
	bool32				SameCutName(CString Str1, CString Str2);

	// �õ���ǰѡ����Ŀ��ָ������
	CFormularContent*	GetCurrentSelectedItemData();

	// ����ѡ�еĽڵ�����
	void				UpdateSelectedName(const CString& StrSelectedItem, int32 iIndex = -1);

	//
	int32				GetCurSel()		{ return m_iCurSel; }
	//
	int32				GetSortCol()	{ return m_iSortCol; }

	//
	int32				GetSortDir()	{ return m_iSortDir; }

	//
	void				OnNewFormular(E_FormularType eFormularType);

	// ������ѡ��ָ��Ľ���ʱ, ��ش�����
	void				SetToChoose()	{ m_bChooseFormular = true; }

	CFormularContent*	GetChoosedFormular() { return m_pFormularChoose; }

private:
    void SetExportButtonStatus(CFormularContent *pContent);
    void SaveUserFomularToFile(CString filePath, CFormularContent *pContent);
    void ReadUserIndexFromFile();
	
private:
	int32				m_iCurSel;	
	CImageList			m_ImageList;
	CFormulaLib*		m_pLib;
	int32				m_iSortCol;
	int32				m_iSortDir;

	CString				m_aStrSelectName[FomularPageNums];

	bool32				m_bChooseFormular;
	CFormularContent*	m_pFormularChoose;
// Construction
public:
	CDlgFormularManager(CWnd* pParent = NULL);   // standard constructor
	~CDlgFormularManager();
// Dialog Data
	//{{AFX_DATA(CDlgFormularManager)
	enum { IDD = IDD_DIALOG_FORMULA_MANAGER };
	CTreeCtrl	m_TreeSys;
	CTreeCtrl	m_TreeGroup;
	CListCtrl	m_ListUser;
	CListCtrl	m_ListAll;	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFormularManager)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFormularManager)
	afx_msg void OnButtonOften();
	afx_msg void OnButtonClose();
	afx_msg void OnButtonModify();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonNew();
	virtual void OnOK();
	afx_msg void OnRadioAll();
	afx_msg void OnRadioGroup();
	afx_msg void OnRadioSys();
	afx_msg void OnRadioUser();	
	afx_msg void OnMenu(UINT nID);
	afx_msg void OnSelchangedTreeGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTreeSys(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListAll(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListUser(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListAll(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListUser(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListAll(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListUser(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTreeGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTreeSys(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBnClickedButtonUserImport();
    afx_msg void OnBnClickedButtonUserExport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFORMULARMANAGER_H_)
