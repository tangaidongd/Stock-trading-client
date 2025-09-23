#if !defined(_DLGDOWNLOAD_H_)
#define _DLGDOWNLOAD_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgdownload.h : header file
//

#include <vector>

#include "MerchManager.h"
//#include "OffLineData.h"
#include "TextProgressCtrl.h"
#include "DialogEx.h"
#include "EngineCenterBase.h"

class CMerch;
class CViewData;

using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CDlgDownLoad dialog

class CDlgDownLoad : public CDialogEx, public COffLineDataNotify
{
public:
	enum E_ReqType
	{
		ERTRealtimePrice = 0,
		ERTKLine,
		ERTF10,

		ERTCount
	};
	// From COffLineDataNotify
public:
	// �����Ѿ�����
	virtual void OnAllRequestSended(E_CommType eType);

	// ��������
	virtual void OnDataDownLoading(CMerch* pMerch, E_CommType eType);
	
	// ���سɹ�
	virtual void OnDataDownLoadFinished(E_CommType eType);
	
	// ����ʧ��
	virtual void OnDataDownLoadFailed(const CString& StrErrMsg);

private:
	// �������
	void		ResetData();

	// ��ʼ���ؼ�
	void		InitialCtrls();

	// ��ʼ��ʱ��ȡ��Ʒȫ��
	bool32		GetAllMerchs();

	// ��ʼ����ʱ, ȡһ�����յ���Ʒ����
	void		GetDownLoadMerchs();

	// ����б��
	void		FillList(CListCtrl* pList, const vector<CMerch*>& aMerchs);

	// ���������
	void		ResetProgress();

	// ���ÿؼ��Ƿ����
	void		SetCtrlsEnable(bool32 bEnable);

	// ��ȡ�Ի��������Ĳ���
	void		GetParams(OUT CGmtTime& TimeBegin, OUT CGmtTime& TimeEnd, OUT E_KLineTypeBase& eType);

	// ��֤���صĲ����Ƿ���ȷ
	bool32		ValidateParams();

	// ǿ����ֹ����
	void		ForceStopDownLoad();

	// ��ȡ���ڵ��б��е���Ʒ
	void		GetMerchsInList(OUT vector<CMerch *> &aMerchs);

private:
	// ��������Ƿ��Զ��رնԻ���
	bool32				m_bAutoClose;

	// �Ƿ���Ҫ��������������
	bool32				m_bRtPriceFirst;

	// ������Ʒ����
	vector<CMerch*>		m_aMerchsAll;

	// ��Ҫ���ص���Ʒ����
	vector<CMerch*>		m_aMerchsDownload;

	// viewdata ָ��
	CAbsCenterManager*	m_pAbsCenterManager;

	// �ѻ���������ģ��
	//COffLineData*		m_pOffLineData;

	// ���һ���յ����ݵ�ʱ��
	DWORD				m_dwTimeLastRecv;
	

// Construction
public:
	CDlgDownLoad(bool32 bAutoClose, CWnd* pParent = NULL);   // standard constructor
	~CDlgDownLoad();

// Dialog Data
	//{{AFX_DATA(CDlgDownLoad)
	enum { IDD = IDD_DIALOG_DOWNLOAD };
	CTextProgressCtrl	m_Progress;
	CListCtrl	m_ListSelect;
	CDateTimeCtrl	m_TimeEnd;
	CDateTimeCtrl	m_TimeBegin;
	CListCtrl	m_ListMerch;	
	BOOL	m_bAllMerch;
	BOOL	m_bDownLoadRtPrice;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDownLoad)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDownLoad)
	afx_msg void OnRadioDay();
	afx_msg void OnRadio5min();
	afx_msg void OnRadio1min();
	afx_msg void OnRadioMonth();
	afx_msg void OnRadioF10();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonRemove();
	afx_msg void OnButtonBegin();
	afx_msg void OnCheckAllMerch();	
	afx_msg void OnCheckrealtime();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnMsgProgess(WPARAM wParam, LPARAM lParam);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DLGDOWNLOAD_H_)
