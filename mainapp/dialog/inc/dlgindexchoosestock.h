#if !defined(AFX_DLGINDEXCHOOSESTOCK_H__)
#define AFX_DLGINDEXCHOOSESTOCK_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgindexchoosestock.h : header file
//
 
#pragma warning(disable: 4786)


#include <vector>
#include <list>
#include <map>
#include "IoViewShare.h"
#include "GridCtrlSys.h"
#include "XScrollBar.h"
#include "MutiTreeCtrl.h"
#include "IndexChsStkMideCore.h"
#include "TextProgressCtrl.h"
#include "DialogEx.h"
#include "GridCtrl.h"
#include "IoViewDetail.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgIndexChooseStock dialog
 
class CMerch;

class CDlgIndexChooseStock : public CDialogEx
{
public:
// ����������
enum E_ProgressType
{
	EPTWeight = 0,
	EPTKLine,
	EPTCalc,
	EPTFinish,

	EPTCount
};

enum E_TravelTreePurpose	// �������Ĺ���ѡ��
{
	ETTPGetCheck = 0,		// �õ�ѡ����
	ETTPUnCheck,			// ȡ��ѡ��

	ETTPCount
};

public:
	bool32					ConstructGrid();									// ����
	void					InitialControls();									// ��ʼ���ؼ�����
	void					ChangeControlState(bool32 bEnable);					// ����/���� �ؼ�
	void					SetStatickKlineNums();								// ����K �߸����ռ�
	void					ResetProgress();									// ���������

	bool32					BeParamsValid();									// �ж�ѡ�ɵ������Ƿ���Ч
	void					TravelTreeItem(HTREEITEM hItem, E_TravelTreePurpose ePurpose);	// �������ڵ�
	bool32					GetSelectedBlocks();								// �õ�ѡ�е���Ʒ

	void					SetIndexChsStkResult(const std::vector<T_IndexChsStkResult>& aIndexChsStkResult); // ����õ����ݸ�ֵ
	void					DisplayDatas();										// ����õ�������ʾ����
	
	void					StartCheckTimeOutTimer();							// ��ʼ��鳬ʱ�Ķ�ʱ��
	void					StopCheckTimeOutTimer();							// �رռ�鳬ʱ�Ķ�ʱ��
private:	
	CGridCtrlSys			m_GridCtrl;											// ���
	CXScrollBar				m_XSBVert;											// �ݹ�����
	CXScrollBar				m_XSBHorz;											// �������	
	CImageList				m_ImageTreeState;									// ������̬ͼ

	int32					m_iRecvDataTimeSpan;								// �������ݵ�ʱ����
	bool32					m_bBeginChoose;										// ��ʼѡ��
	CIndexChsStkMideCore	m_MidCore;											// ���ݴ�������
	E_NodeTimeInterval		m_eTimeInterval;									// ����

	std::vector<CString>	m_aBlocks;											// �û�ѡ�����Ʒ
	std::map<CString, E_NodeTimeInterval>  m_aMapCycles;						// ������������Ķ�Ӧ��ϵ
	std::vector<T_IndexChsStkResult>	   m_aIndexChsStkResult;				// ѡ�ɽ��	

// Construction
public:	
	CDlgIndexChooseStock(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgIndexChooseStock)
	enum { IDD = IDD_DIALOG_INDEX_CHOOSE_STOCK };
	CTextProgressCtrl	m_ProgressWeight;
	CTextProgressCtrl	m_ProgressKLine;
	CTextProgressCtrl	m_ProgressCalc;
	CMutiTreeCtrl	m_Tree;
	CComboBox	m_ComboCycle;
	UINT	m_uiParam1;
	UINT	m_uiParam2;
	UINT	m_uiParam3;
	UINT	m_uiOwn;
	BOOL	m_bCheck1;
	BOOL	m_bCheck3;
	BOOL	m_bCheck2;
	CString	m_StrKLineNums;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgIndexChooseStock)
	protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgIndexChooseStock)
	afx_msg void OnSize(UINT nType, int cx, int cy);	
	afx_msg void OnButtonBegin();
	afx_msg void OnButtonExport();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonExit();
	afx_msg void OnClose();
	afx_msg void OnSelchangeComboCycle();
	afx_msg LRESULT OnMsgProgess(WPARAM wParam, LPARAM lParam);
	afx_msg void OnButtonClear();
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnChangeEditParam1();
	afx_msg void OnChangeEditParam2();
	afx_msg void OnChangeEditParam3();
	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
	afx_msg void OnCheck3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINDEXCHOOSESTOCK_H__)
