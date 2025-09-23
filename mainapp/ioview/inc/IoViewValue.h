// IoViewValue.h: interface for the CIoViewValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_IOVIEWVALUE_H_)
#define _IOVIEWVALUE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IoViewBase.h"
#include "CStaticSD.h"
#include "XScrollBar.h"
#include "GridCtrlSys.h"
#include "ReportScheme.h"

class CIoViewValue : public CIoViewBase  
{
public:
	CIoViewValue();
	virtual ~CIoViewValue();

	DECLARE_DYNCREATE(CIoViewValue)

	// virtual from ioviewbase
public: 
	
	virtual bool32	FromXml(TiXmlElement * pElement);
	virtual CString	ToXml();
	virtual CString	GetDefaultXML(){ return L"";}

	virtual void	SetChildFrameTitle();
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();

	virtual void	OnVDataForceUpdate();
	virtual void	RequestViewData();
	
	// ֪ͨ��ͼ�ı��ע����Ʒ
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);

	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch);
	virtual void	OnVDataGeneralFinanaceUpdate(CMerch* pMerch);

	// ���巢���仯
	virtual void	OnIoViewFontChanged();
	virtual void	OnIoViewColorChanged();
					
private:
	// �������
	bool32			CreateTable(E_ReportType eMerchKind);

	// ���û���
	void			SetDrawLineInfo();

	// ���óߴ�
	void			SetSize();

	// �����ǵ�ͣ�۸�
	bool32			CalcRiseFallMaxPrice(OUT float& fRiseMax, OUT float& fFallMax);

	// �Ƿ��Ǵ���ָ��
	bool32			BeGeneralIndex(OUT bool32& bH);

private:

	CXScrollBar		m_XSBVert;
	CXScrollBar		m_XSBHorz;
	CGridCtrlSys	m_GridCtrl;
	CImageList		m_ImageList;	
	CStaticSD		m_CtrlTitle;

	int32			m_iRowHeight;
	int32			m_iTitleHeight;

	E_ReportType     m_eMerchKind;
	CString			m_StrBlockName;
protected:
	//{{AFX_MSG(CIoViewValue)
	afx_msg	void OnPaint();	
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(_IOVIEWVALUE_H_)
