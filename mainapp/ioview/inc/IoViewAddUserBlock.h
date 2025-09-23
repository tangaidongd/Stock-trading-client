// IoViewValue.h: interface for the CIoViewAddUserBlock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_IOVIEW_ADD_USER_BLOCK_H_)
#define _IOVIEW_ADD_USER_BLOCK_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IoViewBase.h"
#include "CStaticSD.h"
#include "XScrollBar.h"
#include "GridCtrlSys.h"
#include "ReportScheme.h"

class CIoViewAddUserBlock : public CIoViewBase ,public CObserverUserBlock
{
public:
	CIoViewAddUserBlock();
	virtual ~CIoViewAddUserBlock();

	DECLARE_DYNCREATE(CIoViewAddUserBlock)

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

	// from CObserverUserBlock ֪ͨ
	virtual void	OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType);	

	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch);
	virtual void	OnVDataGeneralFinanaceUpdate(CMerch* pMerch);

	// ���巢���仯
	virtual void	OnIoViewFontChanged();
	virtual void	OnIoViewColorChanged();

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
					
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

	E_MerchReportField ReportHeader2MerchReportField( CReportScheme::E_ReportHeader eReportHeader,bool32& bTrans );
	E_MerchReportField ReportHeader2MerchReportFieldBeiJing( CReportScheme::E_ReportHeader eReportHeader,bool32& bTrans );
	CReportScheme::E_ReportHeader  MerchReportField2ReportHeader(  E_MerchReportField eField,bool32& bTrans );
	void OnBtnResponseEvent(UINT nButtonId);
	void CreateAddImpBtn();

	bool32			SetUserBlockHeadInfomationList();
	void			ReSetGridHead();		
	void			UserBlockPagejump();
	
	//
	int             TButtonHitTest(CPoint point);

public:
	static	const SimpleTabClassMap &GetSimpleTabClassConfig();

private:

	// ÿ��ҳ���������Ϣ
	CArray<T_TabInfo, T_TabInfo&>	m_aTabInfomations;

	// ÿ��Tab �ı�ͷ��Ϣ (�̶���, ��ͷ����, ������λ��)
	int32							m_iFixCol;
	CArray<T_HeadInfo,T_HeadInfo>	m_ReportHeadInfoList;
	int32							m_iCurTab;
	E_ReportType					m_eMarketReportType;
	bool32                          m_bIsFinance;				// �Ƿ�����ࡰ�������񡱰�ť
	T_SimpleTabInfoClass			m_SimpleTabInfoClass;		// ����������۱�tab����
	static	SimpleTabClassMap		sm_mapSimpleTabConfig;		// ����

	bool32							m_bRequestViewSort;

	CXScrollBar						m_XSBVert;
	CXScrollBar						m_XSBHorz;

	CNCButton						m_BtnImpUserBlk;			// ������ѡ��
	CNCButton						m_BtnAddUserBlk;			// ������ѡ��
	std::map<int, CNCButton>		m_mapButton;			   // ���ӵ��밴ť�б�	
	Image							*m_pImgBtn;
	CRect							m_RectGrid;
	
	E_ReportType     m_eMerchKind;

	int			     m_iXButtonHovering;			// ��ʶ�����밴ť����

protected:
	CGridCtrlSys	 m_GridCtrl;

	//{{AFX_MSG(CIoViewAddUserBlock)
	afx_msg	void OnPaint();	
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
};

#endif // !defined(_IOVIEWVALUE_H_)
