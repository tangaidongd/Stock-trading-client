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
// 进度条类型
enum E_ProgressType
{
	EPTWeight = 0,
	EPTKLine,
	EPTCalc,
	EPTFinish,

	EPTCount
};

enum E_TravelTreePurpose	// 遍历树的功能选项
{
	ETTPGetCheck = 0,		// 得到选中项
	ETTPUnCheck,			// 取消选择

	ETTPCount
};

public:
	bool32					ConstructGrid();									// 建表
	void					InitialControls();									// 初始化控件内容
	void					ChangeControlState(bool32 bEnable);					// 禁用/启用 控件
	void					SetStatickKlineNums();								// 设置K 线个数空间
	void					ResetProgress();									// 清零进度条

	bool32					BeParamsValid();									// 判断选股的数据是否有效
	void					TravelTreeItem(HTREEITEM hItem, E_TravelTreePurpose ePurpose);	// 遍历树节点
	bool32					GetSelectedBlocks();								// 得到选中的商品

	void					SetIndexChsStkResult(const std::vector<T_IndexChsStkResult>& aIndexChsStkResult); // 计算好的数据赋值
	void					DisplayDatas();										// 计算好的数据显示出来
	
	void					StartCheckTimeOutTimer();							// 开始检查超时的定时器
	void					StopCheckTimeOutTimer();							// 关闭检查超时的定时器
private:	
	CGridCtrlSys			m_GridCtrl;											// 表格
	CXScrollBar				m_XSBVert;											// 纵滚动条
	CXScrollBar				m_XSBHorz;											// 横滚动条	
	CImageList				m_ImageTreeState;									// 树的三态图

	int32					m_iRecvDataTimeSpan;								// 接收数据的时间间隔
	bool32					m_bBeginChoose;										// 开始选股
	CIndexChsStkMideCore	m_MidCore;											// 数据处理中心
	E_NodeTimeInterval		m_eTimeInterval;									// 周期

	std::vector<CString>	m_aBlocks;											// 用户选择的商品
	std::map<CString, E_NodeTimeInterval>  m_aMapCycles;						// 周期与下拉框的对应关系
	std::vector<T_IndexChsStkResult>	   m_aIndexChsStkResult;				// 选股结果	

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
