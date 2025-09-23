#ifndef _IO_VIEW_PRICE_H_
#define _IO_VIEW_PRICE_H_

#include "dlgbelongblock.h"
#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "CStaticSD.h"
#include "MarkManager.h"

#include <vector>
#include <utility>
#include <map>
using std::pair;
using std::vector;
using std::map;
/////////////////////////////////////////////////////////////////////////////
// CIoViewPrice
#define  SNAPSHOTTIMER				8456325	
#define  IOVIEWPRICEROWHEIGHT		30

enum E_TickProperty
{
	ETP_DuoKai = 0,		// �࿪
	ETP_DuoPing,		// ��ƽ
	ETP_DuoHuan,		// �໻
	ETP_KongKai,		// �տ�
	ETP_KongPing,		// ��ƽ
	ETP_KongHuan,		// �ջ�
	ETP_ShuangKai,		// ˫��
	ETP_ShuangPing,		// ˫ƽ

	ETP_Count
};

CString	TickPropertyToString(E_TickProperty eProperty);

class CIoViewPrice : public CIoViewBase, public CMerchMarkChangeListener, public CGridCtrlCB
{
// Construction
public:
	CIoViewPrice();
	virtual ~CIoViewPrice();

	DECLARE_DYNCREATE(CIoViewPrice)

	// ��������ָ��������������������
	enum E_MerchExpType
	{
		EMET_ShangZhengZhiShu,	// ��ָ֤��
		EMET_ShenZhenZhiShu,	// ��ָ֤��
		EMET_Count,				// �޷�ʶ��ı�־
	};
	
protected:
	virtual void OnFixedRowClickCB(CCellID& cell){}
	virtual void OnFixedColumnClickCB(CCellID& cell){}
    virtual void OnHScrollEnd(){}
    virtual void OnVScrollEnd(){}
	virtual void OnCtrlMove( int32 x, int32 y ){}
	virtual bool32 OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew );
	virtual void OnGridMouseMove(CPoint pt);

	// from CControlBase
public:
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	// from CIoViewBase
public:
	virtual void	RequestViewData();
	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	//linhc 20100904����һ��˵�
	virtual bool32	GetStdMenuEnable(MSG* pMsg) { return false;}
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();

	virtual void	LockRedraw();
	virtual void	UnLockRedraw();
	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();
	virtual void	OnVDataMarketSnapshotUpdate(int32 iMarketId);
	
	//linhc 20100914 �޸�                  
	virtual void	OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType);
	virtual void	OnVDataMerchKLineUpdate(IN CMerch *pMerch);		// �����ڻ��õ�
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch);	// �����ڻ��õ�
	virtual void	OnIoViewFontChanged();

	virtual void	OnVDataGeneralNormalUpdate(CMerch* pMerch);

	// ���
	virtual void	OnMerchMarkChanged(int32 iMarketId, const CString &StrMerchCode, const T_MarkData &MarkOldData);

public:
	int32		    CalcRiseFallCounts(bool32 bRise);	
	void			ShowRiseFallCounts(int32 iRiseCounts,int32 iFallCounts);
	
	bool32			BeSpecial();

	// from CView	
protected:
	virtual void	Draw();
	void			SetSize();

	CSize			GetProperSize(OUT float *pfCharWidth=NULL);

private:
	void			SetRowHeightAccordingFont();								// ���������С,�����и�
	bool32		    CreateTable(E_ReportType eMerchKind);		

	// ָ�������ֶ����
	typedef vector<CMerch *>    ExpMerchArray;
	typedef	pair<CString,  ExpMerchArray >	ExpMerchPair;
	typedef CArray<ExpMerchPair, const ExpMerchPair & > AppendExpMerchArray;
	E_MerchExpType	GetMerchExpType(CMerch *pMerch);	// ��ȡָ����Ʒ��Ӧ��ָ������
	bool32			GetAppendExpMerchArray(E_MerchExpType eMET, OUT AppendExpMerchArray &aExpMerchs);	// ��ȡָ������Ҫ���ӵ���ʾ��Ʒ�б�
	float			GetAppendExpAmout(const ExpMerchPair &expPair);	// ��ȡ��Ӧ��ĳɽ���

	// �����ڻ�
	bool32			IsFutureCn(CMerch *pMerch);
	bool32			GetFutureCnStartEndTime(CMerch *pMerch, OUT CGmtTime &TimeStart, CGmtTime &TimeEnd);
	bool32			GetFutureCnStartEndTime(CMerch *pMerch, OUT CMsTime &TimeStart, CMsTime &TimeEnd);
	bool32			AnalyzeTickData(const CTick &tick, float fPreHold);	// ͳ�Ʒֱʽ��
	E_TickProperty	AnalyzeTickProperty(const CTick &tick, float fPreHold); // �����ֱ�����
	void			ResetFutureCnData();	// ��������
	void			ShowFutureCnData(bool32 bDraw=true);		// ��������ʾ�����
	void			InitFutureCnTradingDay();
	void			RequestFutureCnTicks();	// ����ֱ�����
	bool32			UpdateFutureCnTick();
	bool32			IsNeedShowFutureCnTick();		// �Ƿ���Ҫ�ֱ�ͳ��

private:
	bool32			BeGeneralIndex(OUT bool32& bH);
	bool32			BeShowBuySell(int32 Marketid);
	int32			GetShowBuySellItem(int32 Marketid);	// ��ȡ��ʾ����
	bool32			SetGridCtrlRow(int32 iItem);		// ����һ��ָ����С�����ı��

		
	bool32			BeSpecialBuySell();
	bool32			BeERTMony(); // �Ƿ���������

	// ���������ַ���
	CString CalculatefVolumnBuy(float fTotalValue, float fBuyNum, float fSellNum,float fRefer,bool32 bTranslate = false,bool32 bZeroAsHLine = true, bool32 bAddPercent = false, bool32 bShowNegative=false);
////////////////////////////////////////////////////////////
//
protected:
	BOOL			m_bIsShowBuySell;	// �Ƿ���ʾ����5��
	int32			m_iBuySellCnt;		// ��ʾ����(1-5)
	CXScrollBar		m_XSBVert;
	CXScrollBar		m_XSBHorz;
	CGridCtrlSys	m_GridCtrl;
	CImageList		m_ImageList;	
	CStaticSD		m_CtrlTitle;
	CRect			m_rcTitleHead;
	bool32			m_bFromXml;
	CGridCtrlSys	m_GridExpRiseFall;	// �����µ�������

	CXTipWnd		m_TipWnd;
	
	int32			m_iRowHeight;

	int32			m_iTitleHeight;
	int32			m_iPerfectHeight;
	
	CRect			m_RectView;
	CRect			m_RectIndexRnG;		// ָ������������ʾ����
	
	E_ReportType     m_eMerchKind;
	CArray<int32,int32>				m_aSeparatorLine;
	CArray<int32,int32>				m_aMarketIdArray;		
		

	AppendExpMerchArray				m_aExpMerchs;

	typedef map<E_ReportType, int32> MerchTypeHeightMap;
	MerchTypeHeightMap				m_mapMerchHeight;	// ������Ʒ���͵ĸ߶�

	// �����ڻ�
	typedef CArray<CTick, CTick>	TickArray;
	CTrendTradingDayInfo			m_TrendTradingDayInfo;	// 
	TickArray						m_aFutureCnTicks;	// �ֱ�����ͳ��
	float							m_fVolDuoKai;		// �࿪
	float							m_fVolDuoPing;		// ��ƽ
	float							m_fVolKongKai;		// �տ�
	float							m_fVolKongPing;		// ��ƽ
	CString							m_StrFutureCnProperty;	// ��ǰһ������
	DWORD							m_dwFutureCnProperty;

	//
	CFont							m_fontBuySell;
	
	// ����������۵�����
	CRect							m_RectSpecialBuy;
	CRect							m_RectSpecialSell;


	int32			m_iScreenHeight;	// ��Ļ�ĸ߶�

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewPrice)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewPrice)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	//}}AFX_MSG
	//linhc 20100907����һ���Ӧ�˵�
    afx_msg void OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridLButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridLDBClick(NMHDR *pHdr, LRESULT *pResult);
	afx_msg void OnGridEditBegin(NMHDR *pHdr, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_CHART_H_
