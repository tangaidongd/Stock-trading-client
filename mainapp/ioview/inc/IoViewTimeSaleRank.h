#ifndef _IO_VIEW_TIMESALE_RANK_H_
#define _IO_VIEW_TIMESALE_RANK_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "GridCtrlNormal.h"
#include "CStaticSD.h"
#include "GuiTabWnd.h"
#include "ReportScheme.h"
#include "MerchManager.h"
#include "BlockManager.h"
#include "PlugInStruct.h"
#include "BlockConfig.h"

/////////////////////////////////////////////////////////////////////////////
// CIoViewTimeSaleRank

// ��������IoViewBase��С��Ҫ��ȡIoViewManager�ĵ���

class CIoViewTimeSaleRank : public CIoViewBase
{
// Construction 
public:
	CIoViewTimeSaleRank();
	virtual ~CIoViewTimeSaleRank();

	DECLARE_DYNCREATE(CIoViewTimeSaleRank)
	
public:
	// from CControlBase
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
public:
	// from CIoViewBase
	virtual void	SetChildFrameTitle();  // ��������ʾ ������ - �г�
	virtual bool32	FromXml(TiXmlElement *pElement){return true;};		// ��xml����
	virtual CString	ToXml(){return CString("");};
	virtual	CString GetDefaultXML(){return CString("");};
	
	virtual void	OnIoViewActive();						
	virtual void	OnIoViewDeactive();
	virtual bool32	GetStdMenuEnable(MSG* pMsg){return false;};		// ����Ҫ��׼�Ҽ��˵�

	virtual void	LockRedraw(){};				// ����Ҫlock
	virtual void	UnLockRedraw(){};

	virtual CMerch  *GetMerchXml();
public:	
	void			OnDblClick(CMerch *pMerch);				// ��ĳ����Ʒ����ѡ�ɲ���������

	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch){};

	// ��ǰ�����ݽӿ�Ӧ�ò����õ� - ������ʲô���ݽӿڰ� - �������κ����ݽṹ
	virtual void	OnVDataForceUpdate();
	
	virtual void	OnIoViewColorChanged();			// ��������Ҫ����
	virtual void	OnIoViewFontChanged();

	virtual bool32	IsPluginDataView() { return true; };
	virtual void	OnVDataPluginResp(const CMmiCommBase *pResp);

	virtual E_IoViewType GetIoViewType() { return EIVT_MultiMerch; }
	virtual void	GetAttendMarketDataArray(OUT AttendMarketDataArray &aAttends);

	void			OnTickExResp(const CMmiRespPeriodTickEx *pResp);		// �ֱ�ͳ�����ݻذ�
	void			OnMerchSortResp(const CMmiRespPeriodMerchSort *pResp);	// ��������ذ�

public:
	void			OpenBlock(int32 iBlockId);		// ��԰���, ��ֻ�л���A��
	void			RefreshView(int32 iPeriod);

public:
	uint8   m_iPeriod;

	// from CView	
public:
	
	CXScrollBar		*SetXSBHorz(CXScrollBar *pHorz = NULL);		// ����ˮƽ�����������ΪNull��ʹ��Ĭ�Ϲ�����, �����ϴε�
	CXScrollBar		*GetXSBHorz() const { return m_pCurrentXSBHorz; };
	void			ResetXSBHorz(bool32 bResetPos=false);
	
	bool32			ShowOrHideXHorzBar();

protected:
	CSize		GetVirtualSize();	// ��ȡ����Ĵ�С
	int32		GetVirtualLeftPos();	// ��ȡ�����������
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

///////////////////////////////////////////////////////////////////////
public:
	typedef	CArray<CMerch *, CMerch *>		MerchArray;
	typedef map<CMerch *, T_TickEx>		MerchDataMap;	// ���ݻ���
	struct T_Tab : public CGridCtrlCB	// ÿ��С������Ϣ
	{	
		typedef	CArray<E_ReportSortPlugIn, E_ReportSortPlugIn>	SortHeaderArray;
// 		1�����󵥻�����10ǿ�����󵥻���������ǰ10
// 			2���󵥻�����10ǿ���󵥻���������ǰ10
// 			3������������10ǿ�����������ǰ10��
// 			4����������10ǿ���������ǰ10��
// 			5���ɽ�����10ǿ���� 
// 			6��ÿ�ʽ��10ǿ���� 
// 			7��ÿ�ʹ���10ǿ�� ��
// 			8���������10ǿ����

		enum E_HeaderType
		{
			HugeSaleChangeRate = 0,
			BigSaleChangeRate,
			HugeBuyAmount,
			BigBuyAmount,
			SaleCount,
			AmountPerSale,
			UnitPerSale,
			BuyRate,

			HT_COUNT
		};

		E_HeaderType		m_eType;
		CGridCtrlNormalSys		m_wndGrid;
		CXScrollBar						m_XSBVert;
		CXScrollBar						m_XSBHorz;

		CRect				m_RectTab;
		CRect				m_RectTitle;
		CIoViewTimeSaleRank	*m_pwndRank;

		MerchArray						m_aMerchs;			// ������Ʒ��������

		CMmiReqPeriodMerchSort				m_MmiRequestSys;	// ��������
		
		T_Tab();

		~T_Tab();

		BOOL		Create(CIoViewTimeSaleRank *pParent, int nId);
		void		Draw(CDC &dc);
		void		MoveRect(const CRect &rc, bool32 bDraw=TRUE);	// �������

		CString     GetTitleString();
		CString     GetNameString();
		void		UpdateTableHeader();
		void		UpdateTableContent(const MerchArray &aMerchs);
		bool32      GetCellValue(OUT GV_ITEM &item);

		E_ReportSortPlugIn	GetSortHeader();

		void		RequestVisibleData(uint8 iPeriod = 1);		// ������ʾ����
		void		RequestSortData(bool32 bForceReq = false, uint8 iPeriod = 1);			// ������������

		void		OnMerchShowDataUpdate(CMerch *pMerch, bool32 bDrawNow = true);	// ��Ʒ��ʾ����ˢ��
		void		OnMerchSortUpdate(const MerchArray &aMerchsSort, uint8 iPeriod = 1);			// ��Ʒ���б仯 - ��Ҫ���ڽ����жϵ������ĸ�������
		
		virtual void	OnFixedRowClickCB(CCellID& cell){};		// �����ͼ�������ò��û��ʲô����
		virtual void	OnFixedColumnClickCB(CCellID& cell){};
		virtual void	OnHScrollEnd(){};
		virtual void	OnVScrollEnd(){};
		virtual void	OnCtrlMove( int32 x, int32 y ){};
		virtual bool32	OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew ){  return false; };

		static	int32		GetNamesSize() { return  m_sStrHeaderTypeNames.GetSize(); }
		static	int32		GetTitlesSize(){ return  m_sStrHeaderTypeTitles.GetSize(); } 
		static CStringArray m_sStrHeaderTypeNames;
		static CStringArray m_sStrHeaderTypeTitles;
		static SortHeaderArray	m_sSortHeaders;
	};
	
	friend struct T_Tab;
private:
	void			SetRowHeightAccordingFont();							
	void			SetColWidthAccordingFont();

	void			UpdateTableHeader();
	void			UpdateTableContent( CMerch *pMerch, bool32 bBlink );
	void			UpdateTableAllContent();

	void			RecalcLayout();

	CBlockLikeMarket	*GetCurrentBlock();
	CBlockLikeMarket	*GetBlock(int32 iBlockId);
	
	void			RequestViewDataCurrentVisibleRow();	
	void			RequestSortData(bool32 bForceReq = false, uint8 iPeriod = 1);

	void			RequestData(CMmiCommBase &req, bool32 bForce = false);			// ���������������

	void			DoTrackMenu(CPoint pos);

	void			CreateBtnList();								// ��ѡ���ڰ�ť

	void			RedrawCycleBtn(CPaintDC *pPainDC);				// �ػ����ڰ�ť
	void			AddNCButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption);
	int				TNCButtonHitTest(CPoint point);
private:	
	
	bool32							m_bInitialized;			// �Ƿ��ʼ�����

	int32							m_iBlockId;				// ��ǰ���id

	// ÿ��ҳ���������Ϣ
	CArray<T_Tab, const T_Tab &>	m_aTabInfos;			// ����С�����Ϣ

	CGridCtrl						*m_pLastFocusGrid;		// ��ô�ж��أ���

	MerchDataMap					m_mapMerchData;			// ������Ʒ���ݻ���

	CXScrollBar						*m_pCurrentXSBHorz;		// ˮƽ���������Խ����ⲿ����


	CNCButton						btnPrompt;
	std::map<int, CNCButton>	    m_mapBtnCycleList;					// ���ڰ�ť�б�	

	Image		*m_pImgBtn;

public:

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewTimeSaleRank)
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewTimeSaleRank)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDoInitialize();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	
	afx_msg void OnGridRButtonDown(UINT nId, NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClick(UINT nId, NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridColWidthChanged(UINT nId, NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyDownEnd(UINT nId, NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyUpEnd(UINT nId, NMHDR *pNotifyStruct, LRESULT* pResult);
 
	afx_msg void OnGridGetDispInfo(UINT nId, NMHDR *pNotifyStruct, LRESULT *pResult);
    //linhc 20100911���ѡ��ı�ʱ����
    afx_msg void OnGridSelRowChanged(UINT nId, NMHDR *pNotifyStruct, LRESULT* pResult );
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
		
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_TIMESALE_RANK_H_
