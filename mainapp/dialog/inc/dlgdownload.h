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
	// 请求都已经发出
	virtual void OnAllRequestSended(E_CommType eType);

	// 正在下载
	virtual void OnDataDownLoading(CMerch* pMerch, E_CommType eType);
	
	// 下载成功
	virtual void OnDataDownLoadFinished(E_CommType eType);
	
	// 下载失败
	virtual void OnDataDownLoadFailed(const CString& StrErrMsg);

private:
	// 清空数据
	void		ResetData();

	// 初始化控件
	void		InitialCtrls();

	// 初始化时获取商品全集
	bool32		GetAllMerchs();

	// 开始下载时, 取一次最终的商品集合
	void		GetDownLoadMerchs();

	// 填充列表框
	void		FillList(CListCtrl* pList, const vector<CMerch*>& aMerchs);

	// 重设进度条
	void		ResetProgress();

	// 设置控件是否可用
	void		SetCtrlsEnable(bool32 bEnable);

	// 获取对话框描述的参数
	void		GetParams(OUT CGmtTime& TimeBegin, OUT CGmtTime& TimeEnd, OUT E_KLineTypeBase& eType);

	// 验证下载的参数是否正确
	bool32		ValidateParams();

	// 强制中止下载
	void		ForceStopDownLoad();

	// 获取现在的列表中的商品
	void		GetMerchsInList(OUT vector<CMerch *> &aMerchs);

private:
	// 下载完成是否自动关闭对话框
	bool32				m_bAutoClose;

	// 是否需要先下载行情数据
	bool32				m_bRtPriceFirst;

	// 所有商品集合
	vector<CMerch*>		m_aMerchsAll;

	// 需要下载的商品集合
	vector<CMerch*>		m_aMerchsDownload;

	// viewdata 指针
	CAbsCenterManager*	m_pAbsCenterManager;

	// 脱机数据下载模块
	//COffLineData*		m_pOffLineData;

	// 最后一次收到数据的时间
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
