#pragma once

// CDialogExportSaleData 对话框

#include "XScrollBar.h"
#include "ViewDataListener.h"

#include "NCButton.h"
#include "BtnST.h"
#include "DialogEx.h"
#include "GridCtrlNormal.h"
#include "EngineCenterBase.h"

class CDialogExportSaleData : public CDialogEx,public CViewDataListner 
{
	DECLARE_DYNAMIC(CDialogExportSaleData)
public:
	CDialogExportSaleData(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogExportSaleData();

	void ExportGridToExcel(CGridCtrlNormal* pGridCtrl);
	void	SetMerchInfo(CMerchInfo& mrechInfo);
	virtual void OnDataRespMerchTimeSales(int iMmiReqId, IN const CMmiRespMerchTimeSales *pMmiRespMerchTimeSales);
	CGridCtrlNormal	*m_pGridCtrl;	// 表
// 对话框数据
	enum { IDD = IDD_DIALOG_EXPORT_SALE_DATA };
	CDateTimeCtrl	m_dateBegin;
	CDateTimeCtrl	m_dataEnd;
	CStatic			m_staticProcessInfo;
	CButton			m_buttonExport;
	CButton			m_buutonSearch;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CXScrollBar		m_XSBVert;
	CXScrollBar		m_XSBHorz;
protected:
	afx_msg void OnDatetimechangeDatetimepickerBegin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeDatetimepickerEnd(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonSearch();
	afx_msg void OnButtonExport();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
protected:
	CAbsCenterManager* m_pAbsCenterManager;
	CMerchInfo m_MerchInfo;
	CTime m_timeStart;
	CTime m_time;	// 第一条数据的时间
	CTime m_timeEnd;
};