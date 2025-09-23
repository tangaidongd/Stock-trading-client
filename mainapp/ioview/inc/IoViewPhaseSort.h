#ifndef _IOVIEWPHASESORT_H_
#define _IOVIEWPHASESORT_H_

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
#include "PhaseSortCalc.h"
#include "IoViewReport.h"
#include "DlgTimeWait.h"
#include <map>

using std::map;

/////////////////////////////////////////////////////////////////////////////
// CIoViewPhaseSort

enum E_PhaseSortType			// �б�ͷ������
{
	EPST_RiseFallPecent = 0,		// �ǵ���
	EPST_TradeRate,				// ������
	EPST_VolChangeRate,			// �������
	EPST_ShakeRate,				// �𵴷���
		
	EPST_Count
};

CString GetPhaseSortTypeName(E_PhaseSortType eType);

struct T_PhaseOpenBlockParam
{
	// �򿪰��Ĳ���
	int32		m_iBlockId;			// ���з�Χ
	CGmtTime	m_TimeStart;		// ��ʼʱ��
	CGmtTime    m_TimeEnd;			// ����ʱ��
	bool32      m_bDoPreWeight;		// �Ƿ�ǰ��Ȩ
	E_PhaseSortType		m_ePhaseSortType;   // �����ֶ�

	T_PhaseOpenBlockParam();
	bool32		IsCalcParamSame(const T_PhaseOpenBlockParam &pa1) const;
};

bool32 operator==(const T_PhaseOpenBlockParam &pa1, const T_PhaseOpenBlockParam &pa2);
bool32 operator!=(const T_PhaseOpenBlockParam &pa1, const T_PhaseOpenBlockParam &pa2);

// Ĭ�ϲ����κΰ��

class CIoViewPhaseSort : public CIoViewBase, public CGridCtrlCB, public CGuiTabWndCB, public CBlockConfigListener
{
// Construction 
public:
	typedef CBlockConfig::IdArray	IdArray;

	CIoViewPhaseSort();
	virtual ~CIoViewPhaseSort();

	DECLARE_DYNCREATE(CIoViewPhaseSort)
	
public:
	// from CControlBase
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public:
	// from CObserverUserBlock ֪ͨ
	//virtual void	OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType);	
	
public:
	// from CIoViewBase
	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();
	virtual bool32	GetStdMenuEnable(MSG* pMsg);

	virtual void	LockRedraw();
	virtual void	UnLockRedraw();

	virtual	CMerch *GetMerchXml();
	virtual	E_IoViewType GetIoViewType() { return EIVT_MultiMerch; }
	virtual DWORD	GetNeedPushDataType() { return 0; } // ����Ҫ�κ���������
	virtual void	GetAttendMarketDataArray(OUT AttendMarketDataArray &aAttends);
public:	
	void			OnDblClick(CMerch *pMerch);

	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();
	virtual void	OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs);
	virtual void	OnVDataGridHeaderChanged(E_ReportType eReportType);


	virtual void	OnIoViewColorChanged();
	virtual void	OnIoViewFontChanged();
public:
	// CBlockConfigListener
	virtual void	OnBlockConfigChange(int32 iBlockId, E_BlockNotifyType eNotifyType);
	virtual void	OnConfigInitialize(E_InitializeNotifyType eInitializeType);


public:
	bool32			OpenBlock(const T_PhaseOpenBlockParam &BlockParam, bool32 bReOpen, bool32 bAskUserSetting = false);		// ��ָ�����, �Ƿ���Ѿ����˵�
	void			ChangeTabToCurrentBlock();
	void			GetPhaseOpenBlockParam(OUT T_PhaseOpenBlockParam &BlockParam)const { BlockParam = m_PhaseOpenBlockParam; }
	
public:
	static	bool	CompareRow(int iRow1, int iRow2);		// ������
	static  CIoViewPhaseSort *m_spThis;

	// from CView	
public:
	const T_SimpleTabInfo* GetSimpleTabInfo(int32 iTabIndex);

protected:

	// from CGuiTabWndCB
private:
	void			OnRButtonDown2(CPoint pt, int32 iTab);
	bool32			OnLButtonDown2(CPoint pt, int32 iTab);
	bool32			BlindLButtonBeforeRButtonDown(int32 iTab);

///////////////////////////////////////////////////////////////////////
private:
	

	enum E_PhaseHeaderType
	{
		EPHT_No = 0,		// ���
		EPHT_Code,			// ����
		EPHT_Name,			// ����
		EPHT_RiseFall,		// �ǵ���
		EPHT_TradeRate,		// ������
		EPHT_VolChangeRate, // ����
		EPHT_ShakeRate,		// ��
		EPHT_PreClose,		// ����
		EPHT_High,			// ��
		EPHT_Low,			// ��
		EPHT_Close,			// ����
		EPHT_Vol,			// ��
		EPHT_Amount,		// ���
		EPHT_MarketRate,	// �г���

		EPHT_Count,			// ��Чֵ
	};

	enum E_Stage
	{
		ES_WaitPrice = 0,		// �ȴ��������ݽ׶�
		ES_WaitCalc,			// �ȴ����ݼ���׶�

		ES_Count				// ������ϻ���������
	};

	typedef map<E_PhaseHeaderType, CString> PhaseHeaderStringMap;
	
	struct T_TitleString
	{
		CString  m_StrTitle;		// ����
		COLORREF m_clr;				// ��ɫ
	};
	typedef CArray<T_TitleString, const T_TitleString &> TitleStringArray;

	struct T_ColumnHeader
	{
		CString m_StrHeader;
		E_PhaseHeaderType  m_eHeaderType;			// ��־����еĶ���
		bool32	m_bNeedSort;		// ��Ҫ���򣬶�������н���һ���ڵ�һ��ʱ��Ч

		float	m_fPreferWidthByChar;		// ϣ���Ŀ�ȣ������ַ��Ŀ��
		float   m_fFixedRowHeightRatio;		// �̶��еĸ߶�ϵ��
		float	m_fNonFixedRowHeightRatio;  // �ǹ̶��еĸ߶���̶��иߵı��� Ĭ��1.0, ��������е���������

		T_ColumnHeader();
		T_ColumnHeader(E_PhaseHeaderType eHeaderType);
		T_ColumnHeader(E_PhaseHeaderType eHeaderType, LPCTSTR pszName, bool32 bSort = false, float fWidthByChar = 0.0f, float fFixedRowHeight = 1.0f, float fNonFixedRowHeight = 1.0f);

		bool32 Initialize(E_PhaseHeaderType eHeaderType);		// ��ʼ��
		bool32 Initialize(const CString &StrHeader);			// ʹ�����Ƴ�ʼ��

		static void InitStatic();
		static CArray<T_ColumnHeader, const T_ColumnHeader &> s_aInitHeaders;
	};
	friend T_ColumnHeader;
	typedef CArray<T_ColumnHeader, const T_ColumnHeader &> ColumnHeaderArray;

	
	
private:
	void			TabChanged();
	void			SetTab(int32 iCurTab);	
	void			SetTabByBlockId(int32 iBlockId);

	void			InitializeTabs();		// ��ʼ��tab���
	
	CBlockLikeMarket *GetCurrentBlock();					// ��ȡ��ǰ���

	void			SetRowHeightAccordingFont();							
	void			SetColWidthAccordingFont();

	void			InitializeTitleString();							// ��ʼ����ǰ��ʾ�ı����ִ�
	void			InitializeColumnHeader(E_PhaseSortType eType);	// ��ʼ����ǰ����������
	void			SetSortColumnBySortType(E_PhaseSortType eType);					// ����������ͣ�����Ҫ�ؼ���

	void			UpdateTableHeader();
	void			UpdateTableAllContent();		// �����������ݣ�ˢ����ʾ

	void			RecalcLayout();

	// �̶����� �������򽻸����
	virtual void	OnFixedRowClickCB(CCellID& cell);
	virtual void	OnFixedColumnClickCB(CCellID& cell);
    virtual void	OnHScrollEnd();
    virtual void	OnVScrollEnd();
	virtual void	OnCtrlMove( int32 x, int32 y );
	virtual bool32	OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew );

	void			DoTrackMenu(CPoint pos);
	
	bool32			CalcPhaseRequestData(INOUT T_PhaseSortCalcRequestData &reqData);
	
	int32			RequestAllPriceData(bool32 bForce = false);					// ����������Ʒ����������
	int32			TryRequestMoreNeedPriceMerchs();						// ��������е���Ʒ
	void			RequestAttendMerchs(bool32 bForce);					// ����ǰ��ע����Ʒ������
	void			RequestAttendMerchs(const CArray<CSmartAttendMerch, CSmartAttendMerch &> &aSmarts, bool32 bForce);
	void			RequestExpKline();
	void			RequestWeightData();

private:	
	int32							m_iSortColumn;

	// ������
	CGridCtrlNormalSys				m_GridCtrl;	
	CXScrollBar						m_XSBVert;
	CXScrollBar						m_XSBHorz;

	CRect							m_RectTitle;				// ���Ű����� -- XXX ����: 2010-08-02,��
	
	tm								m_tmDisplay;
	// Tab ���
	CGuiTabWnd						m_GuiTabWnd;
	
	bool32							m_bBlockReportInitialized;	// �Ƿ��ʼ�����

	T_PhaseOpenBlockParam			m_PhaseOpenBlockParam;		// �򿪸ð��Ĳ���
	T_PhaseOpenBlockParam			m_PhaseBlockParamXml;		// ��xml�л�ȡ�Ĳ���

	int32							m_iPreTab;					// �л�ǰ��һ��Tab - ���������ݽ�Ϊ�����л�״̬��
	int32							m_iCurTab;					// ��ǰTab

	T_PhaseSortCalcParam			m_ParamForDisplay;			// ������Ľ����������ʾ
	T_PhaseSortCalcParam			m_ParamForCalc;				// ���ڼ����

	TitleStringArray				m_aTitleString;
	ColumnHeaderArray				m_aColumnHeaders;			// ��ǰ�м���

	CDlgTimeWait					m_DlgWait;
	E_Stage							m_eStage;

	CGmtTime						m_TimeLastReqPrice;			// ���һ������������Ʒ�������ݵ�ʱ��
	MerchArray						m_aMerchsNeedPrice;			// ��Ҫ�����������Ʒ��ò��һ�β��ܷ�̫��

	typedef map<CMerch *, float>    MerchFloatDataMap;			
	MerchFloatDataMap				m_mapCircAssert;			// ��Ʒ����ͨ�ɱ�
	MerchFloatDataMap				m_mapExpAmount;				// ָ���������ܽ��
	typedef map<CMerch *, CGmtTime> MerchRequestTimeMap;
	static MerchRequestTimeMap		s_mapMerchReqTime;			// ��Ʒ������ˢ���б�

public:

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewPhaseSort)
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewPhaseSort)
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDoBlockReportInitialize();
	afx_msg void OnMsgDoCalc();							// ��ʼ����
	afx_msg LRESULT OnMsgCalcStart(WPARAM w, LPARAM l);	// �߳�֪ͨ�Ѿ���ʼ����
	afx_msg LRESULT OnMsgCalcPos(WPARAM w, LPARAM l);	// �߳�֪ͨ����Ľ���
	afx_msg LRESULT OnMsgCalcEnd(WPARAM w, LPARAM l);	// �߳�֪ͨ�������
	afx_msg LRESULT OnMsgCalcReqData(WPARAM w, LPARAM l); // �߳�֪ͨ����Ҫ��ͼ������
	//}}AFX_MSG
	
	afx_msg void OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridGetDispInfo(NMHDR *pNotifyStruct, LRESULT *pResult);
	afx_msg void OnGridCacheHint(NMHDR *pNotifyStruct, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IOVIEWPHASESORT_H_