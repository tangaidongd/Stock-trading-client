#ifndef _IO_VIEW_STARRY_H_
#define _IO_VIEW_STARRY_H_
#pragma warning(disable: 4786)
#include <map>
using	std::map;
#include <vector>
using	std::vector;
#include <set>
using	std::set;

#include "IoViewBase.h"
#include "MerchManager.h"
#include "BlockManager.h"
#include "ReportScheme.h"
#include "GridCtrlSys.h"
#include "GuiTabWnd.h"
#include "XScrollBar.h"
#include "tinyxml.h"
#include "ioviewshare.h"
#include "DlgTimeWait.h"
#include "BlockConfig.h"

struct T_MerchAxisNode
{
	CMerch	*m_pMerch;
	CReportScheme::E_ReportHeader	m_eHeader; // ��ǰʹ�õ��ֶ�
	
	T_MerchAxisNode();
	operator double();
};



// �ǿ�ͼ
class CIoViewStarry : public CIoViewBase,  public CGuiTabWndCB, public CBlockConfigListener
{
// Construction 
public:
	
	virtual ~CIoViewStarry();

	DECLARE_DYNCREATE(CIoViewStarry)

public:
	typedef	CArray<CMerch *, CMerch *>	MerchArray;
	typedef CReportScheme::E_ReportHeader	AxisType;

public:
	// from CIoViewBase
	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);		// ʵ��Xml�洢
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	
	virtual void	OnIoViewActive();						
	virtual void	OnIoViewDeactive();
	virtual bool32	GetStdMenuEnable(MSG* pMsg){return false;};

	virtual void	LockRedraw();
	virtual void	UnLockRedraw();

	virtual E_IoViewType GetIoViewType() { return EIVT_MultiMerch; }
	virtual DWORD		GetNeedPushDataType() { return 0; }	// ����Ҫ�κ���������
	// from CIoViewBase			
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();		// ��ô����г���ʼ����- -
	virtual void	OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs);
	virtual void	OnVDataGridHeaderChanged(E_ReportType eReportType);
	virtual void	OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType);	// �������ݸ���
	

	virtual void	OnIoViewColorChanged();			
	virtual void	OnIoViewFontChanged();

	// BlockConfig
	virtual void	OnBlockConfigChange(int32 iBlockId, E_BlockNotifyType eNotifyType);		// ������ݱ��
	virtual	void	OnConfigInitialize(E_InitializeNotifyType eInitializeType);				// ����������ñ��

	// TabCB - ���������tab��
	virtual void	OnRButtonDown2(CPoint pt, int32 iTab);
	virtual bool32  OnLButtonDown2(CPoint pt, int32 iTab);	
	virtual bool32	BlindLButtonBeforeRButtonDown(int32 iTab){return false;};	// û���ж��ĸ�tab��- -
public:
	
	void	DoCmdUIUpdate(CCmdUI *pUI);

	bool32	SetNewBlock(int32 iBlockId, bool32 bReOpen = false, bool32 bRequestData = true);

private:
	enum E_Coordinate{
		COOR_Normal,	// ��ͨ����
		COOR_Logarithm,	// ��������
		COOR_END			
	};

private:
	struct T_XYAxisType;

	void	SetNewBlock(const T_Block &block, bool32 bRequestData = true);

	// grid��ʾ
	void	EmptyGridContent();	// ֻ��ʾһ����� �� block����
	void	UpdateBlockGrid();	// grid��ʾ - ��ʾ��ǰblock��Ϣ
	void	UpdateBlockPaintMerchCount();	// ��ʾ������������
	void	UpdateMouseMerch(CPoint ptMouse); // �п���Ϊ��
	void	UpdateSelectedRange(CPoint ptMouse);	// ��ѡ��Χ��ʾ
	void	UpdateSelectedMerchs();	// ��ѡ����Ʒ��ʾ
	int32	InsertUpdateSelectedMerch(CMerch *pMerch);	// ���һ����Ʒ��ѡ�����Ʒ�� ����ѡ����Ʒ�б���ʾ - -1ʧ��
	void	UpdateSelectedMerchValue(CMerch *pMerch); // ����ѡ���б�����Ʒ������

	void	CalcXYAllMinMaxAxis(bool32 bNotifyChange = false);		// ����xy�� - ����������Ʒ������XY min max������Ժ��ѡ�Ļ�������ͨ��ֱ���޸� min max�����ķ�Χ
	void	CalcXYMinMaxAxis(CMerch *pMerch, bool32 bNotifyChange = false, bool32 *pbMinMaxChanged = NULL);
	void	UpdateXYMinMax(double fxMin, double fxMax, double fyMin, double fyMax, bool32 bNotifyChange = false);
	void	ChangeAxisType(AxisType eNewXType, AxisType eNewYType);		// �������û�иı�Ļ�����m_e?Type��ֵ��ΪeNew?Type
	void	TabXYSelChange();		// xy��tab����ѡ��ı�
	AxisType	GetXAxisType(OUT CString *pStrName = NULL);
	AxisType	GetYAxisType(OUT CString *pStrName = NULL);
	CString		GetStarryType();
	CString		GetUnitString(double fScale);	// ��ȡ��λ�����ִ�

	void	SetCoordinate(E_Coordinate eCoor, bool32 bNotifyChange = true);		// ������ͨ/������������
	bool32	GetDivededValueByClientPoint(IN CPoint pt, OUT double *pfx, OUT double *pfy);	// ��ȡ������λ���ʵ��ֵ
	bool32	CalcXYValueByClientPoint(IN CPoint pt, OUT double *pfx, OUT double *pfy);		// ����������Ӧ��ʵ��ֵ
	bool32	CalcClientPointByXYValue(IN double fx, IN double fy, OUT CPoint &ptClient);	// ��������ֵ��client x,yӳ��
	
	bool32	CalcClientPointByXYValueNormal(IN double fx, IN double fy, OUT CPoint &ptClient);		// ��ͨ�������
	bool32	CalcXYValueByClientPointNoraml(IN CPoint pt, OUT double *pfx, OUT double *pfy);	// ����������Ӧ��ʵ��ֵ���Ե�λ�̶Ⱥ��ֵ
	bool32	CalcClientPointByXYValueLogarithm(IN double fx, IN double fy, OUT CPoint &ptClient);	// �����������
	bool32	CalcXYValueByClientPointLogarithm(IN CPoint pt, OUT double *pfx, OUT double *pfy);
	

	void	DrawActiveFlag(CDC &dc);
	int		GetProperChartGridCellSize(OUT double &cx, OUT double &cy, OUT int32 *pColCount = NULL, OUT int32 *pRowCount = NULL);	// ��ȡchart��ÿ����Ԫ���ʵ��ĳ��������ܹ����ڵĵ�Ԫ������
	void	DrawXAxis(CDC &dc);
	void	DrawYAxis(CDC &dc);
	void	DrawChart(CDC &dc);		// �ṩ��������Ʒ�ģ�
	void	DrawMerch(CMerch *pMerch, CDC *pDC);	// ���Ƶ�������Ʒ����DrawChart���� - ��ʱ����
	void	DrawMerchAlreadyDrawed(CMerch *pMerch, CDC &dc, int32 iRadius, bool32 bSpecifyColor = false, COLORREF clrSpecify = RGB(255,255,0));	// ����������Ʒ��ֻ������onPaint�м����˾��ε���Ʒ
	void	DrawMouseMove(CPoint ptMouse);			// ������ʱ�Ķ���
	void	DrawZoomSelect(CDC &dc, CPoint ptEnd);

	bool32	GetMerchDrawRect(CMerch *pMerch, OUT CRect &rcMerch);
	bool32	CalcDrawMerch(CMerch *pMerch);				// ���㵥����Ʒ��������
	int32	CalcDrawMerchs(const MerchArray &aMerchs);	// ����һ����Ʒ�Ļ�������

	CDC		&BeginMainDrawDC(CDC &dcSrc);				// paint�ڴ�dc
	void	EndMainDrawDC(CDC &dcDst);
	CDC		&BeginTempDraw();
	void	EndTempDraw();
	void	MyDCBitBlt(CDC &dcDst, CDC &dcSrc);

	bool32	CalcMerchXYValue(CMerch *pMerch, OUT double *pfXValue, OUT double *pfYValue);
	double	CalcMerchValueByAxisType(CMerch *pMerch, AxisType axisType, bool32 *pBCalced = NULL);
	bool32	GetMerchRealTimePrice(CMerch *pMerch, OUT CRealtimePrice &RealTimePrice);	// ��ȡ����ͼ����Ļ���ֱ��real price
	bool32	GetMerchFinanceData(CMerch *pMerch, OUT CFinanceData &FinanceData);
	CMerch *GetMouseMerch(CPoint pt);			// ���ܷ���NULL
	bool32	TestMerchInChart(CMerch *pMerch, OUT int32 &xClient, OUT int32 &yClient);	// ������Ʒ�Ƿ��ڿ��ӷ�Χ��
	bool32	CalcMerchRect(CMerch *pMerch, OUT CRect &RectMerch);

	void	UpdateAllContent();
	
	void	RefreshBlock();

	bool32	AskUserBlockSelect(OUT T_Block &block, OUT int32 &iBlockId, int32 iCurTab, CPoint ptScr);

	void	RequestViewData();
	void	RequestViewData(CArray<CSmartAttendMerch, CSmartAttendMerch&> &aRequestSmarts);

	void	AddAttendMerchs( const MerchArray &aMerchs, bool bRequestViewData = true );
	void	AddAttendMerch( CMerch *pMerch, bool bRequestViewData = true );
	void	ClearAttendMerchs();
	void	RemoveAttendMerch(CMerch *pMerch, E_DataServiceType eSerivceRemove = EDSTPrice);
	void	RequestBlockViewData();				// �������� - ��һ��ʹ��
	void	RequestBlockViewDataByQueue();		// ʹ�ö���һ��һ������
	void	RequestBlockQueueNext();			// ��ɸ���Ʒ����, ������û������ļ�������
	void	RequestBlockQueueNextAsync();			// ��ɸ���Ʒ����, ������û������ļ�������
	void	RequestViewData(CMmiCommBase &req);

	void	ChangeRadiusByKey(bool32 bAdd);
	
	void	ConvertIntoChartPoint(IN CPoint pt, OUT CPoint &ptInChart);		// ��chart�������תΪchart�ڵ�����
	bool32	IsPtInChart(CPoint pt);

	bool32	GetZoomRect(OUT CRect &rcZoom);		// �����Ƿ���chart��
	bool32	CalcZoomRect(CPoint ptLast, OUT CRect &rcZoom);	// ����zoom�������û�п�ʼzoom����false
	void	EnsureZoom();
	void	CacelZoom();
	void	CancelZoomSelect(CPoint ptMouse, bool32 bDraw = true);
	void	EnsureZoomSelect(CPoint ptMouse);

	void	BlinkUserStock();		// ��˸/����˸��ѡ��
	void	UpdateUserBlockMerchs();		// ��ǰ�������Щ��Ʒ����ѡ��
	void	DrawBlinkUserStock(CDC *pDC = NULL);

	void	DoMerchSelected(CMerch *pMerch, bool32 bNeedDraw = true);
	void	DoDbClick(CPoint ptClick);		// ��Report����
	void	DoDbClick(CMerch *pMerchClick);

	int32	SelectUserAxisType(CPoint ptMenu, INOUT T_XYAxisType &axisType);	// �û�ѡ���Զ��������ᣬ�����û�ѡ��X(1), Y(2), û��ѡ��(0)
	void	MultiColumnMenu(IN CMenu &menu);	// ���ö��в˵�

	void	RecreateBlockTab();

	void	RecalcLayout();

	bool32	IsFinanceDataType(CReportScheme::E_ReportHeader eHeader);
	bool32	IsNeedFinanceDataType();

	bool32	IsMerchNeedReqData(CMerch *pMerch);

private:
	CIoViewStarry();
	CIoViewStarry(const CIoViewStarry&);
	CIoViewStarry &operator=(const CIoViewStarry &);

	typedef set<CMerch *> MerchSet;

	int32		m_iBlockId;
	T_Block		m_block;	// ��ǰ��ע���
	MerchSet	m_MerchsCurrent;	// ��ǰ��Ʒ����
	CArray<int32, int32>	m_aBlockCollectionIds;

	bool32		m_bFirstReqEnd;	// ��һ�����������Ƿ����

	struct T_XYAxisValue{
		CRealtimePrice m_realTimePrice;		
		//CFinanceData  m_financeData;		// ��������ʼ��ȡmerch�е�
		T_XYAxisValue(){
			
		}
		void Reset(){
			m_realTimePrice = CRealtimePrice();
		}
		BOOL IsValid(){
			return m_realTimePrice.m_fPriceNew > 0.0;
		}
	};

	typedef map<CMerch *, T_XYAxisValue>	MerchValueMap;
	// ��Ϊ�����ͼ���������е�ʱ����ĵ�ǰ�������, �п��ܱ����ͼ���������ͼ�������ĵ�real price�����ˣ�������ͼ�ڲ��ڼ���x,yʱ�������xy��price������
	// ����Ժ���������realtime���ݼӽ�����ͬ�����ܳ��ָ�����
	MerchValueMap	m_mapMerchValues;			// ������Ʒ���������ͼ��ȡ��realtime price������������ͼ���µĸ���real price���������ͼ��û������smart merch

	
	bool32		m_bInZoom;		// ������״̬ - ע��ȡ������
	CPoint		m_ptZoomStart;		// ѡ��ʱ��XY��������
	CPoint		m_ptZoomEnd;

	CGuiTabWnd	m_wndTabBlock;	// �ײ����ҵ�Tab��
	CGuiTabWnd	m_wndTabXYSet;

	CGridCtrlSys	m_GridCtrl;		// �Ҳ�ʹ�ñ������ʾ���� - ��ʾ�ϰ벿�ְ������� 2��
	CGridCtrlSys	m_GridCtrlZoomDesc;	// ��ʾ��ѡ�����ε����� 1��
	CGridCtrlSys	m_GridCtrlZoomMerchs;	// ��ѡ�ڵ���Ʒ 2��
	CXScrollBar		m_wndScrollV;

	// ��������й�
	CMerch			*m_pMerchLastMouse;				// ���һ�����Ӵ�����Ʒ
	CRect			m_RectLastZoomUpdate;			// Grid�����һ�ι����йؿ�ѡ��Ʒ���µľ���
	CMerch			*m_pMerchLastSelected;			// �û�ͨ�����ѡ�����Ʒ
	MerchArray		m_aZoomMerch;					// ���տ�ѡ����Ʒ - ֻ�п�ѡ����ʱ�Ÿ���

	// ��ͼ�й�
	int32		m_iRadius;		// Сԭ��İ뾶

	CRect		m_RectLeftY;	// Y��
	CRect		m_RectBottomX;	// X
	CRect		m_RectChart;	// ͼ����

	CRect		m_RectGrid;		// �������

	double		m_fMinY;	// ��ֻ����������
	double		m_fMaxY;
	double		m_fMinX;
	double		m_fMaxX;
	double		m_fScaleX;		// X�������λ
	double		m_fScaleY;		// Y�������λ
	bool32		m_bValidMinMaxX;		// �Ƿ�Ϊ��ЧX/Y�������Сֵ
	bool32		m_bValidMinMaxY;

	E_Coordinate	m_eCoordinate;

	// XML���

	// ��ǰ��ע����Ʒ����
	typedef	map<CMerch *, CRect>	MerchRectMap;
	MerchRectMap	m_mapMerchRect;

	// �������
	typedef	CList<CMerch *, CMerch *> MerchList;
	MerchList		m_lstMerchsWaitToRequest;

	struct T_XYAxisType{
		AxisType	m_eXType;	// ��ʱ��report header��Ϊ����ѡֵ
		AxisType	m_eYType;
		CString		m_StrName;		// ����������
		T_XYAxisType(){
			m_eYType = CReportScheme::ERHPriceNew;
			m_eXType = CReportScheme::ERHCircAsset;
			m_StrName = _T("����");
		}
		
		bool operator==(const T_XYAxisType &axis)
		{
			return m_eXType == axis.m_eXType && m_eYType == axis.m_eYType;
		}

		bool operator!=(const T_XYAxisType &axis)
		{
			return !(*this == axis);
		}
	};
	typedef	CArray<T_XYAxisType, const T_XYAxisType &>	XYAxisTypeArray;
	XYAxisTypeArray		m_aXYTypes;
	T_XYAxisType			m_axisCur;			// ��ǰ�������� - ����Ϊѡ�������
	T_XYAxisType			m_axisUserCur;		// ��ǰ�Զ���������ѡ�� - ʼ��Ϊ�Զ���


	// ����
	CDlgTimeWait		m_dlgWait;
	MerchArray			m_aMerchsNeedInitializedAll;		// ��Ҫ��ʼ����Ʒ - ���ڳ�ʼ��ʱ����
	bool32				m_bMerchsRealTimeInitialized;

	CBitmap				m_bmpImage;		// ���澵��λͼ
	CDC					m_dcMemPaintDraw;
	CBitmap				m_bmpTempDraw;
	CDC					m_dcMemTempDraw;	// ��ʱdc
	CPoint				m_ptLastMouseMove;

	MerchArray			m_aMerchsNeedDraw;			// ��Ҫ��ʱ�����Ƶ����ݱ仯�˵���Ʒ

	bool32		m_bUseCacheRealTimePrices;		// ʹ�û������Ʒ���ۼ���ͼ��

	bool32		m_bIsIoViewActive;
	
	bool32		m_bBlinkUserStock;						// ��˸��ѡ��
	bool32		m_bBlinkUserStockDrawSpecifyColor;		// ��־��һ���Ƿ�ʹ���ض���ɫ��������ѡ��
	MerchArray	m_aUserStocks;							// ��ǰ���������ѡ�ɵ���Ʒ

	typedef map<CMerch *, CGmtTime>	MerchReqTimeMap;
	MerchReqTimeMap		m_mapMerchReqWaitEchoTimes;		// ��Ʒ���������󣬵ȴ����ص��б�
	MerchReqTimeMap		m_mapMerchLastReqTimes;			// ��Ʒ��󷢳���������б�
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewStarry)
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewStarry)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSelChange(UINT nId, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM w, LPARAM l);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT	 OnDoInitializeAllRealTimePrice(WPARAM w, LPARAM l);	// ��ʼ��������Ʒ������
	afx_msg void OnUpdateXYAxis();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMerchSelectChange(NMHDR *pNotifyStruct, LRESULT *pResult);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg void OnMenuStarry(UINT nId);
	afx_msg LRESULT OnMsgHotKey(WPARAM w, LPARAM l);
	//}}AFX_MSG

	
 	DECLARE_MESSAGE_MAP()
};

#endif //_IO_VIEW_STARRY_H_