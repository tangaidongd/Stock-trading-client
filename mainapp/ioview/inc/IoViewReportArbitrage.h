// IoViewReportArbitrage.h: interface for the CIoViewReportArbitrage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_IOVIEWREPORTARBITRAGE_H_)
#define _IOVIEWREPORTARBITRAGE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "XScrollBar.h"
#include "ArbitrageManage.h"

// 套利报价表头结构
typedef struct T_ArbitrageHeadObj
{
	T_ArbitrageHeadObj()
	{
		m_eArbitrageHead = EAHCount;
		m_StrHeadName	 = L"";
	}

	T_ArbitrageHeadObj(E_ArbitrageHead eArbitrageHead, const CString& StrHeadName)
	{
		m_eArbitrageHead = eArbitrageHead;
		m_StrHeadName	 = StrHeadName;
	}

	E_ArbitrageHead		m_eArbitrageHead;
	CString				m_StrHeadName;

}T_ArbitrageHeadObj;

//
class CIoViewReportArbitrage : public CIoViewBase , public CArbitrageNotify, public CGridCtrlCB
{
public:
	CIoViewReportArbitrage();
	virtual ~CIoViewReportArbitrage();

	DECLARE_DYNCREATE(CIoViewReportArbitrage)

	// from CIoViewBase
public:
	virtual bool32	FromXml(TiXmlElement * pElement);
	virtual CString	ToXml();
	virtual CString	GetDefaultXML();
	virtual void	SetChildFrameTitle(){}
	virtual CMerch *GetMerchXml();
	virtual	E_IoViewType	GetIoViewType(){ return EIVT_MultiMerch; };

	// 表头发生变化

	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch){}

	virtual void	OnIoViewColorChanged();
	virtual void	OnIoViewFontChanged();
	virtual bool32	GetStdMenuEnable(MSG* pMsg) { return false;}

	virtual void	RequestViewData();

	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();

	virtual void	LockRedraw();
	virtual void	UnLockRedraw();

	// from CArbitrageNotify
public:
	// 增加套利
	virtual void OnArbitrageAdd(const CArbitrage& stArbitrage);
	
	// 删除套利
	virtual void OnArbitrageDel(const CArbitrage& stArbitrage);
	
	// 修改套利
	virtual void OnArbitrageModify(const CArbitrage& stArbitrageOld, const CArbitrage& stArbitrageNew);
	
	// from  CGridCtrlCB
public:
	virtual void OnFixedRowClickCB(CCellID& cell){}
	virtual void OnFixedColumnClickCB(CCellID& cell){}
    virtual void OnHScrollEnd(){}
    virtual void OnVScrollEnd(){}
	virtual void OnCtrlMove( int32 x, int32 y ){}
	virtual bool32 OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew ){return false;}

	virtual	BOOL	PreTranslateMessage(MSG* pMsg);

public:
	// 取得当前选中套利模型
	CArbitrage*	GetCurrentSelArbitrage();
	
	// 选中指定的套利模型
	int32	SetCurrentSelArbitrage(const CArbitrage &arbToSel);

private:
	// 创建表格
	bool32	CreateTable();

	// 取行的索引
	int32   GetRowIndex(const CArbitrage& stArbitrage);

	// 更新推送商品
	void	UpdatePushMerch();

	// 初始化数据
	void	UpdateTableContent();

	// 更新某一行数据
	void	UpdateOneRow(int32 iRow, const CArbitrage& stArbitrage, bool32 bUpdateData = false);
	
	// 设置某个单元格的值
	void	SetCellValue(int32 iRow, int32 iCol, float fPriceNow, float fAccordingPrice, E_ArbitrageHead eType, int32 iSaveDec);

	void	OnF5();	// 切换
	void	ShowArbitrageChart(const CArbitrage &arb);

protected:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnMenu(UINT nID);
	afx_msg void OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

private:

	// 表格相关
	CGridCtrlSys					m_GridCtrl;
	CXScrollBar						m_XSBVert;
	CXScrollBar						m_XSBHorz;

	CArray<CArbitrage, CArbitrage&> m_aArbitrages;
	bool32							m_bShowGridLines;
};

#endif // !defined(_IOVIEWREPORTARBITRAGE_H_)
