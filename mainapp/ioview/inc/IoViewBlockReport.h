#ifndef _IO_VIEW_BLOCK_REPORT_H_
#define _IO_VIEW_BLOCK_REPORT_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "CStaticSD.h"
#include "GuiTabWnd.h"
#include "ReportScheme.h"
#include "MerchManager.h"
#include "BlockManager.h"
#include "GridCtrlNormal.h"
#include "BlockConfig.h"
#include "IoViewTimeSaleStatistic.h"

/////////////////////////////////////////////////////////////////////////////
// CIoViewBlockReport
class CIoViewBlockReport : public CIoViewBase, public CGridCtrlCB, public CGuiTabWndCB, public CBlockConfigListener
{
// Construction 
public:
	typedef CBlockConfig::IdArray	IdArray;

	CIoViewBlockReport();
	virtual ~CIoViewBlockReport();

	DECLARE_DYNCREATE(CIoViewBlockReport)
	
public:
	// from CControlBase
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public:
	// from CObserverUserBlock ֪ͨ
	virtual void	OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType);	
	
public:
	// from CIoViewBase
	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();
	virtual bool32	GetStdMenuEnable(MSG* pMsg);

	virtual	CMerch *GetMerchXml();		// ��ȡѡ������ǹ�Ʊ
	virtual	E_IoViewType GetIoViewType() { return EIVT_MultiBlock; }
	virtual void	GetAttendMarketDataArray(OUT AttendMarketDataArray &aAttends);
public:	
	void			OnDblClick(int32 iBlockId);

	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();
	virtual void	OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs);
	virtual void	OnVDataGridHeaderChanged(E_ReportType eReportType);

	virtual void	OnIoViewColorChanged();
	virtual void	OnIoViewFontChanged();

	virtual	bool32	IsPluginDataView() { return true; }
	virtual	void	OnVDataPluginResp(const CMmiCommBase *pResp);

	void			OnLogicBlockDataUpdate(const IdArray &aUpdatedBlockIds);
	void			OnBlockSortDataUpdate(const CMmiRespBlockSort *pRespBlockSort);
	void			OnBlockDataResp(const CMmiRespLogicBlock *pResp);
public:
	// CBlockConfigListener
	virtual void	OnBlockConfigChange(int32 iBlockId, E_BlockNotifyType eNotifyType);
	virtual void	OnConfigInitialize(E_InitializeNotifyType eInitializeType);

	void			SortColumn(int32 iSortCol=-1, int32 iSortType=0);	// iSortType=0���� >0 Desc < 0 Ascend
	
public:
	static	bool	CompareRow(int iRow1, int iRow2);		// ������
	static  CIoViewBlockReport *m_spThis;

	// from CView	
public:
	
protected:

	// from CGuiTabWndCB
private:
	void			OnRButtonDown2(CPoint pt, int32 iTab);
	bool32			OnLButtonDown2(CPoint pt, int32 iTab);
	bool32			BlindLButtonBeforeRButtonDown(int32 iTab);

///////////////////////////////////////////////////////////////////////
private:
	

	struct T_BlockReportTab
	{
		enum{
			BCAll  = -2
		};
		int32 m_iBlockCollectionId;	// ��Ӧ�İ�����
		int32 m_iStartRow;		// tab��ʼ���� - �����л�ʱ����
		int32 m_iRowCount;
		int32 m_iColLeftVisible;

		IdArray	m_aSubBlockIds;

		CString m_StrName;

		T_BlockReportTab()
		{
			m_iBlockCollectionId = BCAll;
			m_iStartRow = 0;
			m_iRowCount = 0;
			m_iColLeftVisible = 0;
		}
		T_BlockReportTab(const T_BlockReportTab &tab);
		const T_BlockReportTab &operator=(const T_BlockReportTab &tab);

		bool32 IsValid()
		{
			return m_iStartRow >= 0 && m_iRowCount >= 0 && !m_StrName.IsEmpty();
		}
	};
private:									// esc��ÿ��tab֮���л���
	void			TabChanged();
	void			SetTab(int32 iCurTab, int32 iSortCol=-1, int32 iSortType=0);	
	void			SetTabByClassId(int32 iClassId, int32 iSortCol=-1, int32 iSortType=0);

	void			InitializeTabs();		// ��ʼ��tab���
	
	bool32			TabIsValid(int32 &iTab);					// ��ȡ��ǰ��tab
	CBlockCollection *GetCurrentCollection();					// ��ȡ��ǰ����
	
	CMerch*			GetBlockMerchXml();							//��ȡ�����Ϣ
		

	void			SetRowHeightAccordingFont();							
	void			SetColWidthAccordingFont();

	void			UpdateTableHeader();
	void			UpdateTableContent( int32 iClassId, int32 iBlockId, bool32 bBlink );
	void			UpdateTableAllContent();		// �����������ݣ�ˢ����ʾ

	void			ResetBlockShowData(bool32 bResetTopLeft=true);

	bool32			GetCurrentRowInfo(OUT T_BlockReportTab &tab);		// �п��ܳ���rowcount����ͼ̫С���������Ҫ����
	void			SetCurrentRowInfo(int32 iRowBegin, int32 iColLeft);
	bool32			GetTabInfoByIndex(int32 iIndex, OUT T_BlockReportTab &tabInfo);
	bool32			UpdateTabInfo(int32 iTabIndex, int32 iRowFirst, int32 iColLeft);

	void			RecalcLayout();

	void			OnMouseWheel(short zDelta);

	virtual void	OnFixedRowClickCB(CCellID& cell);
	virtual void	OnFixedColumnClickCB(CCellID& cell);
    virtual void	OnHScrollEnd();
    virtual void	OnVScrollEnd();
	virtual void	OnCtrlMove( int32 x, int32 y );
	virtual bool32	OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew );

	void			DoTrackMenu(CPoint pos);

	void			DoFixedRowClickCB(CCellID& cell, int32 iSortType = 0);	// iSortType=0���� >0 Desc; < 0 Ascend
	void			DoLocalSort();	// ����������
	
	void			RequestViewDataCurrentVisibleRow();		// û��ʲô���ݿ��������
	void			RequestSortData();

	void			RequestData(CMmiCommBase &req);			// ���������������

	void			WatchDayChange(bool32 bDrawIfChange = false);			// ��ʼ�����������Ľ�����
	bool32			IsPointInGrid();

private:	
	int32							m_iSortColumn;
	bool32							m_bRequestViewSort;

	int32							m_iSortColumnFromXml;	// ��xml�л�ȡ��sortcol	
	int32							m_iSortTypeFromXml;		// ��xml�л�ȡ��sortType, ��һ�ε���sortcol������0
	bool32                          m_bShowTitle;			// ��xml�л�ȡ���Ƿ���ʾ����
	bool32							m_bHideTab;			    // ��xml�л�ȡ���Ƿ�����һЩ����ǩ 
	bool32                          m_bUpdateKline;         // ��xml�л�ȡ���Ƿ����K��
	bool32                          m_bUpdateTrend;			// ��xml�л�ȡ���Ƿ���·�ʱ
	bool32                          m_bUpdateReport;		// ��xml�л�ȡ���Ƿ���±��۱�

	CMmiReqBlockSort				m_MmiRequestBlockSort;		// �ܻ���Ҫһ����������� - ����û���κ�����

	// ������
	CGridCtrlNormalSys				m_GridCtrl;	
	CXScrollBar						m_XSBVert;
	CXScrollBar						m_XSBHorz;

	bool32							m_bIsShowGridVertScorll;	// �����Ƿ���ʾ��ֱ������

	CRect							m_RectTitle;				// ���Ű����� -- XXX ����: 2010-08-02,��
	
	// Tab ���
	CGuiTabWnd						m_GuiTabWnd;
	
	bool32							m_bBlockReportInitialized;	// �Ƿ��ʼ�����

	int32							m_iBlockClassId;

	int32							m_iMaxGridVisibleRow;		// ������ɼ�������

	int32							m_iPreTab;					// �л�ǰ��һ��Tab - ���������ݽ�Ϊ�����л�״̬��
	int32							m_iCurTab;					// ��ǰTab

	tm								m_tmDisplay;

	CMmiReqPushPlugInBlockData		m_mmiReqBlockData;			// ��鱨��

	// ��ѡ�ɵ����ݻ�ȡ��Ҫȷ��
	// �������ֶ� ���, ���������ֶ� ����, �����ֶ� ����������  -- ��ѡ�������ֶ� ��������
	// ���������ʱ������id���ᱻ����Ϊ��Чid���ȵ������б�������򽫶�Ӧλ�õ�blockid����Ϊʵ��id
	IdArray							m_aShowBlockIds;			// ���пɼ���block
	
	// ÿ��ҳ���������Ϣ
	CArray<T_BlockReportTab, const T_BlockReportTab&>	m_aTabInfomations;

public:

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewBlockReport)
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewBlockReport)
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);	// ���� - �ǲ���һ��Ҫ��¼�ˣ�ò�Ʋ���¼ҲûʲôӰ��
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);	// ������ֱ�����������ʱ������������
	afx_msg void OnDoBlockReportInitialize();

	afx_msg LRESULT OnScrollPosChange(WPARAM w, LPARAM l);
	//}}AFX_MSG
	
	afx_msg void OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridGetDispInfo(NMHDR *pNotifyStruct, LRESULT *pResult);
	afx_msg void OnGridCacheHint(NMHDR *pNotifyStruct, LRESULT *pResult);
	afx_msg void OnGridSelRowChanged( NMHDR *pNotifyStruct, LRESULT* pResult );

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_BLOCK_REPORT_H_
