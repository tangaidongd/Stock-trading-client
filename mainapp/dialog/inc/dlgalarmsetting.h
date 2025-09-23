#if !defined(_DLGALARMSETTING_H_)
#define _DLGALARMSETTING_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgalarmsetting.h : header file
//

#include "DialogEx.h"


#include "AlarmCenter.h"
#include "ViewDataListener.h"

class CViewData;

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmSetting dialog

class CDlgAlarmSetting : public CDialogEx, public CViewDataListner
{
	// from CViewDataListner
public:
	// 行情
	virtual void OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId);

public:
	//
	void		SetAlarmCenter(CAlarmCenter* pCenter) { if (NULL== pCenter)return; m_pAlarmCenter = pCenter; }

private:

	// 初始化控件
	void		UpdateCtrls(int32 iRadio = -1);
	
	// 填充商品列表
	void		FillListMerch();

	// 填充公式列表
	void		FillListFormular();

	// 填充套利设置
	void        FillListArbitrage();

	// 显示隐藏控件
	void		ShowCtrls(int32 iRadio);
	
	// 提示方式的转换
	CAlarmCenter::E_AlarmType Radio2AlarmType(int32 iRadio);
	
	int32		AlarmType2Radio(CAlarmCenter::E_AlarmType eType);

	// 改变提示方式
	void		OnChangeRadioPrompt();
	
private:
	// 数据中心
	CAlarmCenter*	m_pAlarmCenter;

	CAbsCenterManager*		m_pAbsCenterManager;
// Construction
public:
	CDlgAlarmSetting(CWnd* pParent = NULL);   // standard constructor
	~CDlgAlarmSetting();

// Dialog Data
	//{{AFX_DATA(CDlgAlarmSetting)
	enum { IDD = IDD_DIALOG_ALARM_SET };
	CListCtrl	m_ListMerch;
	CListCtrl	m_ListFormular;
	CListCtrl   m_ListArbitrage;
	int		m_RadioType;
	int		m_RadioPrompt;
	BOOL    m_bCheck7;
	BOOL    m_bCheck8;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAlarmSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAlarmSetting)
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonModify();
	afx_msg void OnButtonRemoveall();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnDblclkListMerch(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListFormular(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRadio4();
	afx_msg void OnRadio5();
	afx_msg void OnRadio6();
	afx_msg void OnRadio7();
	afx_msg void OnRadio8();
	afx_msg void OnRadio9();
	afx_msg void OnMenu(UINT uID);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DLGALARMSETTING_H_)
