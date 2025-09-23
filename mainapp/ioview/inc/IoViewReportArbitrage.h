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

// �������۱�ͷ�ṹ
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

	// ��ͷ�����仯

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
	// ��������
	virtual void OnArbitrageAdd(const CArbitrage& stArbitrage);
	
	// ɾ������
	virtual void OnArbitrageDel(const CArbitrage& stArbitrage);
	
	// �޸�����
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
	// ȡ�õ�ǰѡ������ģ��
	CArbitrage*	GetCurrentSelArbitrage();
	
	// ѡ��ָ��������ģ��
	int32	SetCurrentSelArbitrage(const CArbitrage &arbToSel);

private:
	// �������
	bool32	CreateTable();

	// ȡ�е�����
	int32   GetRowIndex(const CArbitrage& stArbitrage);

	// ����������Ʒ
	void	UpdatePushMerch();

	// ��ʼ������
	void	UpdateTableContent();

	// ����ĳһ������
	void	UpdateOneRow(int32 iRow, const CArbitrage& stArbitrage, bool32 bUpdateData = false);
	
	// ����ĳ����Ԫ���ֵ
	void	SetCellValue(int32 iRow, int32 iCol, float fPriceNow, float fAccordingPrice, E_ArbitrageHead eType, int32 iSaveDec);

	void	OnF5();	// �л�
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

	// ������
	CGridCtrlSys					m_GridCtrl;
	CXScrollBar						m_XSBVert;
	CXScrollBar						m_XSBHorz;

	CArray<CArbitrage, CArbitrage&> m_aArbitrages;
	bool32							m_bShowGridLines;
};

#endif // !defined(_IOVIEWREPORTARBITRAGE_H_)
