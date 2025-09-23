#pragma once

#include "resource.h"
#include "DialogEx.h"
#include "AutoRichEditCtrl.h"
#include "WndCef.h"

// CDlgWebF10 对话框

class CDlgWebF10 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWebF10)
public:
	//CDlgWebF10(CWnd* pParent = NULL);   // 标准构造函数
	CDlgWebF10(CIoViewBase* pIoViewActive, CMerch* pMerch, CWnd* pParent /* = NULL */);
	virtual ~CDlgWebF10();

	// 获取商品
	CIoViewBase* GetIoView() { return m_pIoViewActive; }
	

	void OnMerchChange(CMerch* pMerch);
	
	bool OnTypeChange(CString strType,bool Refresh = true);

	// 调整大小, 使之最大化
	void		AdjustToMaxSize();

private:
	void NavigateWebPage();
public:
// 对话框数据
	enum { IDD = IDD_DIALOG_F10 };
	CAutoRichEditCtrl m_RichEdit;
private:
	// 是否全屏
	bool32			m_bMaxed;
	CWndCef			m_wndCef;

	// ViewData
	CAbsCenterManager*		m_pAbsCenterManager;
	// 当前商品
	CMerch*			m_pMerch;
	// 当前激活视图
	CIoViewBase*	 m_pIoViewActive;

	CString			m_strUrl;
	CString			m_strType;

	bool			m_bInit;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};