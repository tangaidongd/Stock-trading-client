#pragma once

#include "resource.h"
#include "DialogEx.h"
#include "AutoRichEditCtrl.h"
#include "WndCef.h"

// CDlgWebF10 �Ի���

class CDlgWebF10 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWebF10)
public:
	//CDlgWebF10(CWnd* pParent = NULL);   // ��׼���캯��
	CDlgWebF10(CIoViewBase* pIoViewActive, CMerch* pMerch, CWnd* pParent /* = NULL */);
	virtual ~CDlgWebF10();

	// ��ȡ��Ʒ
	CIoViewBase* GetIoView() { return m_pIoViewActive; }
	

	void OnMerchChange(CMerch* pMerch);
	
	bool OnTypeChange(CString strType,bool Refresh = true);

	// ������С, ʹ֮���
	void		AdjustToMaxSize();

private:
	void NavigateWebPage();
public:
// �Ի�������
	enum { IDD = IDD_DIALOG_F10 };
	CAutoRichEditCtrl m_RichEdit;
private:
	// �Ƿ�ȫ��
	bool32			m_bMaxed;
	CWndCef			m_wndCef;

	// ViewData
	CAbsCenterManager*		m_pAbsCenterManager;
	// ��ǰ��Ʒ
	CMerch*			m_pMerch;
	// ��ǰ������ͼ
	CIoViewBase*	 m_pIoViewActive;

	CString			m_strUrl;
	CString			m_strType;

	bool			m_bInit;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};