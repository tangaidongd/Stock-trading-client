#ifndef _IO_VIEW_TIMESALE_STATISTIC_H_
#define _IO_VIEW_TIMESALE_STATISTIC_H_


#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "GridCtrlNormal.h"
#include "CStaticSD.h"
#include "GuiTabWnd.h"
#include "ReportScheme.h"
#include "MerchManager.h"
#include "BlockManager.h"
#include "BlockConfig.h"
#include "PlugInStruct.h"

#include <map>
using std::map;

//////////////////////////////////////////////////////////////////////////
// ��ͷ���� ���� �������
enum E_TimeSaleStatisticHeader
{
	ETSSHCode = 0,	//	����
	ETSSHName,	//	����
	ETSSHPriceNew,	//	����
	ETSSHVolBuySuper,		// 		��������������ɣ�
	ETSSHVolSellSuper,		// 		��������������ɣ�
	ETSSHVolNetSuper,		//		���󵥾�����������ɣ�

	ETSSHVolBuyBig,			// 		������������ɣ�
	ETSSHVolSellBig,		// 		������������ɣ�

	ETSSHVolBuyMid,			// 		�е�����������ɣ�
	ETSSHVolSellMid,		// 		�е�����������ɣ�

	ETSSHVolBuySmall,		// 		С������������ɣ�
	ETSSHVolSellSmall,		// 		С������������ɣ�

	ETSSHVolBuyMidSmall,	// 		��С������������ɣ�
	ETSSHVolSellMidSmall,	// 		��С������������ɣ�

	ETSSHAmountBuySuper,	// 		����������Ԫ��
	ETSSHAmountSellSuper,	// 		�����������Ԫ��
	ETSSHNetAmountSuper,	// 		���󵥾���

	ETSSHAmountBuyBig,		// 		��������Ԫ��
	ETSSHAmountSellBig,		// 		���������Ԫ��
	ETSSHNetAmountBig,		// 		�󵥾���

	ETSSHAmountBuyMid,		// 		�е�������Ԫ��
	ETSSHAmountSellMid,		// 		�е��������Ԫ��
	ETSSHNetAmountMid,		// 		�е�����

	ETSSHAmountBuySmall,	// 		С��������Ԫ��
	ETSSHAmountSellSmall,	// 		С���������Ԫ��
	ETSSHNetAmountSmall,	// 		С������

	ETSSHAmountTotalBuy,	// 		�����ܶ��Ԫ��
	ETSSHAmountTotalSell,	// 		�����ܶ��Ԫ��
	ETSSHTradeRateSuper,	// 		���󵥻�����%
	ETSSHTradeRateBig,		// 		�󵥻�����%
	ETSSHTradeRateBuyBig,	//		����󵥻�����% (��ȷ��)
	ETSSHTradeCount,		// 		�ɽ�����
	ETSSHAmountPerTrans,	// 		ÿ�ʽ���Ԫ��
	ETSSHStocksPerTrans,		// 		ÿ�ʹ������ɣ�
	ETSSHCapitalFlow,		// 		�ʽ�����

	ETSSHCount
};

struct T_TimeSaleStatisticHeader
{
	T_TimeSaleStatisticHeader(const CString &StrName, E_TimeSaleStatisticHeader eHeaderShow, E_ReportSortPlugIn eHeaderSort)
	{
		m_StrName = StrName;
		m_eHeaderShow = eHeaderShow;
		m_eHeaderSort = eHeaderSort;
	}
	T_TimeSaleStatisticHeader()
	{
		m_eHeaderShow = ETSSHCode;
		m_eHeaderSort = ERSPIEnd;
	}
	CString	m_StrName;
	E_TimeSaleStatisticHeader	m_eHeaderShow;
	E_ReportSortPlugIn			m_eHeaderSort;		// �������ֶ�ʹ�õ��� ERSPIEnd
};

extern	const T_TimeSaleStatisticHeader KTimeSaleStatisticHeaders[];		// �Ժ�����õĶ࣬�ڶ���
extern	const int32 KTimeSaleStatisticHeadersCount;

/////////////////////////////////////////////////////////////////////////////
// CIoViewTimeSaleStatistic

class CIoViewTimeSaleStatistic : public CIoViewBase, public CGridCtrlCB, public CGuiTabWndCB
{
// Construction 
public:
	CIoViewTimeSaleStatistic();
	virtual ~CIoViewTimeSaleStatistic();

	DECLARE_DYNCREATE(CIoViewTimeSaleStatistic)
	
public:
	// from CControlBase
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
public:
	// from CIoViewBase
	virtual void	SetChildFrameTitle();  // ��������ʾ ��ͳ�� - �г�
	virtual bool32	FromXml(TiXmlElement *pElement){return true;};		// ��xml����
	virtual CString	ToXml(){return CString("");};
	virtual	CString GetDefaultXML(){return CString("");};
	
	virtual void	OnIoViewActive();						
	virtual void	OnIoViewDeactive();
	virtual bool32	GetStdMenuEnable(MSG* pMsg){return false;};		// ����Ҫ��׼�Ҽ��˵�

	virtual void	LockRedraw();			// ����Ҫlock - ���������п���ΪIoViewManager�µ�ioview��so��Ҫʵ�����
	virtual void	UnLockRedraw();

	virtual	CMerch *GetMerchXml();
	
	virtual CMerch*	GetNextMerch(CMerch* pMerchNow, bool32 bPre);
	virtual void	OnEscBackFrameMerch(CMerch *pMerch);
public:	
	void			OnDblClick(CMerch *pMerch);				// ��ĳ����Ʒ����ѡ�ɲ���������

	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);	// ���۱�ʵʱ���¼�
	

	// ��ǰ�����ݽӿ�Ӧ�ò����õ� - ������ʲô���ݽӿڰ� - �������κ����ݽṹ
	virtual void	OnVDataForceUpdate();

	virtual void	OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType);	// ��������
	virtual void    OnVDataReportInBlockUpdate(int32 iBlockId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch *, CMerch *> &aMerchs);

	virtual void	OnIoViewColorChanged();			// ��������Ҫ����
	virtual void	OnIoViewFontChanged();

	virtual	bool32	IsPluginDataView() { return true; } 
	virtual void	OnVDataPluginResp(const CMmiCommBase *pResp);		// ���ݰ��ᷢ�͵�������

	virtual E_IoViewType GetIoViewType() { return EIVT_MultiMerch; }
	virtual void	GetAttendMarketDataArray(OUT AttendMarketDataArray &aAttends);

	// ��Ҫ����3������ - �Ѿ�ȫ����tickex������
	void			OnTickExResp(const CMmiRespPeriodTickEx *pResp);		// �ֱ�ͳ�ƻذ�

	void			OnMerchSortResp(const CMmiRespPeriodMerchSort *pResp);	// ��������ذ�

public:

	void		    OpenBlock(int32 iBlockId);		// �����г�id��Ψһ��

	CXScrollBar		*SetXSBHorz(CXScrollBar *pHorz = NULL);		// ����ˮƽ�����������ΪNull��ʹ��Ĭ�Ϲ�����, �����ϴε�
	CXScrollBar		*GetXSBHorz() const;

	bool32			ShowOrHideXHorzBar();

	static	bool	CompareRow(int iRow1, int iRow2);		// ������
	static  CIoViewTimeSaleStatistic *m_pThis;
	//bool		operator()(int iRow1, int iRow2);  // grid������

	// from CView	
public:
	void RefreshView(int32 iPeriod);
	
private:
	int32 m_iPeriod;

	// from CGuiTabWndCB
private:
	void			OnRButtonDown2(CPoint pt, int32 iTab);
	bool32			OnLButtonDown2(CPoint pt, int32 iTab);
	bool32			BlindLButtonBeforeRButtonDown(int32 iTab);

///////////////////////////////////////////////////////////////////////
private:
	struct T_TimeSaleStatisticData
	{
		T_TickEx		m_tickEx;
		
		T_TimeSaleStatisticData();
	};
	typedef map<CMerch *, T_TimeSaleStatisticData>	DataMap;

	typedef map<CMerch *, int>				MerchMap;
	
	typedef	CArray<CMerch *, CMerch *>		MerchArray;

	typedef CArray<T_TimeSaleStatisticHeader, const T_TimeSaleStatisticHeader &>	HeaderArray;

	struct T_Tab
	{
		enum E_Sort
		{
			SortNone,
			SortAscend,
			SortDescend,
		};
		int32 m_iBlockId;		// ��Ӧ�İ��
		int32 m_iStartRow;		// tab��ʼ���� ���� - ����fixed row
		int32 m_iRowCount;		
		int32 m_iScrollXPos;	//	���
		int32 m_iSortColumn;

		MerchArray						m_aMerchs;			// ������Ʒ��������
		MerchArray						m_aMerchsVisible;	// ��Ч�ɼ���Ʒ���� - ��С��aMerchs��ͬ��������Ч����ʹ��NULL��ʾ��ע�⣡

		E_Sort  m_eSort;

		CString m_StrName;

		T_Tab()
		{
			m_iBlockId = CBlockConfig::GetDefaultMarketlClassBlockPseudoId();	// ����A
			m_iStartRow = 0;
			m_iRowCount = 0;
			m_iScrollXPos = 0;
			m_iSortColumn = -1;
			m_eSort = SortNone;
		}

		bool32 IsValid()
		{
			return /*m_iBlockId >= -1 &&*/ m_iStartRow >= 0 && m_iRowCount >= 0 && !m_StrName.IsEmpty();
		}

		void ResetShowData()
		{
			m_iStartRow = 0;
			m_iRowCount = 0;
			m_iScrollXPos = 0;
			m_eSort = SortNone;
			m_iSortColumn = -1;
			m_aMerchsVisible.Copy(m_aMerchs);
		}

		BOOL IsSort()
		{
			ASSERT( (m_eSort == SortNone&&-1==m_iSortColumn) || (m_eSort!=SortNone&&m_iSortColumn != -1) );
			return m_eSort != SortNone;
		}

		void SetSortVisibleMerchs(const MerchArray &aVisibleMerchs)
		{
			// �ӿ��ӵĵ�һ�п�ʼ��������Ʒ���б��滻������λ�ñ����
			ASSERT( IsSort() );
			ASSERT( m_iStartRow < m_aMerchs.GetSize() && m_aMerchs.GetSize() == m_aMerchsVisible.GetSize() );
			ASSERT( m_iStartRow + aVisibleMerchs.GetSize() <= m_aMerchsVisible.GetSize() );
			ASSERT( m_iStartRow >= 0 );	//
			int i = 0;
			int32 iStartRow = m_iStartRow;	//
			if ( iStartRow < 0 )
			{
				iStartRow = 0;
			}
			for ( i=0; i < iStartRow && i < m_aMerchsVisible.GetSize() ; i++ )
			{
				m_aMerchsVisible[i] = NULL;
			}

			for ( i=0; i < aVisibleMerchs.GetSize() && (iStartRow + i) < m_aMerchsVisible.GetSize() ; i++ )
			{
				ASSERT( aVisibleMerchs[i] != NULL );
				m_aMerchsVisible[ iStartRow + i ] = aVisibleMerchs[i];
			}

			// iָ�����һ��û��յ�
			for ( i = i+iStartRow; i < m_aMerchsVisible.GetSize() ; i++  )
			{
				m_aMerchsVisible[i] = NULL;
			}
		}

		void ClearSort()
		{
			m_eSort = SortNone;
			m_iSortColumn = -1;

			m_aMerchsVisible.Copy(m_aMerchs);	// ԭʼ�������ݰ���ԭʼ˳��ɼ�
		}

		BOOL ChangeSortType(int iCol, BOOL *pbColChanged =NULL)	// �����Ƿ�����
		{
			if ( NULL != pbColChanged )
			{
				*pbColChanged = FALSE;
			}
			if ( iCol != m_iSortColumn )
			{
				m_eSort = SortNone;		// ��ͬ��������ԭ������״̬
				m_iSortColumn = iCol;
				if ( NULL != pbColChanged )
				{
					*pbColChanged = TRUE;
				}
			}
			if ( m_eSort == SortDescend )
			{
				m_eSort = SortAscend;
			}
			else
			{
				m_eSort = SortDescend;
			}
			return m_eSort == SortAscend;
		}
	};

private:
	void			SetRowHeightAccordingFont();							
	void			SetColWidthAccordingFont();

	void			UpdateTableHeader();
	void			UpdateTableRowContent( CMerch *pMerch, bool32 bBlink );
	void			UpdateTableAllContent();

	void			AppendShowMerchs(int32 iMerchAppCount);

	void			RecalcLayout();

	void			OnMouseWheel(short zDelta);

	virtual void	OnFixedRowClickCB(CCellID& cell);
	virtual void	OnFixedColumnClickCB(CCellID& cell);
    virtual void	OnHScrollEnd();
    virtual void	OnVScrollEnd();
	virtual void	OnCtrlMove( int32 x, int32 y );
	virtual bool32	OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew );
	
	void			GetColumnHeaders(OUT HeaderArray &aHeaders);
	E_ReportSortPlugIn	GetColumnSortHeader(int iCol);
	E_TimeSaleStatisticHeader	GetColumnShowHeader(int iCol);

	CBlockLikeMarket			*GetCurrentBlock();
	CBlockLikeMarket			*GetBlock(int32 iBlockId);

	CMerch			*CheckMerchInBlock(CMerch *pMerch);
	bool32			IsMerchInMerchArray(CMerch *pMerch, const MerchArray &aMerchs);

	void			UpdateVisibleMerchs();		// ���µ�ǰ����Щ��Ʒ�ɼ���Ϣ
	
	void			RequestViewDataCurrentVisibleRow();		// û��ʲô���ݿ��������
	void			RequestSortData(bool32 bForce = false);						// ������������

	void			RequestData(CMmiCommBase &req, bool32 bForce = false);			// ���������������

	void			DoTrackMenu(CPoint pos);

	void			CreateBtnList();								// ��ѡ���ڰ�ť

	void			RedrawCycleBtn(CPaintDC *pPainDC);				// �ػ����ڰ�ť
	void			AddNCButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption);
	int				TNCButtonHitTest(CPoint point);

protected:
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

private:	
	
	//CMmiReqReport			m_MmiRequestSys;		// �ܻ���Ҫһ����������� - ����û���κ�����
	CMmiReqPeriodMerchSort	m_MmiRequestSys;
	CMmiReqBlockReport		m_MmiRequestBlockSys;	// ���¼۵İ�����б���

	MerchMap				m_mapLastPushMerchs;	// ���һ����������

	// ������
	CGridCtrlNormalSys		m_GridCtrl;	
	CXScrollBar				m_XSBVert;
	CXScrollBar				m_XSBHorz;

	CXScrollBar				*m_pCurrentXSBHorz;		// ˮƽ���������Խ����ⲿ����
	
	// Tab ���
	//CGuiTabWnd			m_GuiTabWnd;			// �Ҽ��˵�ѡ��
	
	bool32					m_bInitialized;			// �Ƿ��ʼ�����

	int32					m_iMaxGridVisibleRow;	// ������ɼ�������

	DataMap					m_mapMerchData;			// ��Ʒ��ʾ����
	
	T_Tab					m_tabInfo;				// ��ǰ��Ʒ��ʾ��Ϣ

	CNCButton						btnPrompt;
	std::map<int, CNCButton>	    m_mapBtnCycleList;					// ���ڰ�ť�б�	

	Image		*m_pImgBtn;

public:

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewTimeSaleStatistic)
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewTimeSaleStatistic)
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDoInitialize();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnScrollPosChanged(WPARAM w, LPARAM l);
	afx_msg LRESULT OnDoTrackMenu(WPARAM w, LPARAM l);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	
	afx_msg void OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult);

	afx_msg void OnGridGetDispInfo(NMHDR *pNotifyStruct, LRESULT *pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_TIMESALE_STATISTIC_H_
