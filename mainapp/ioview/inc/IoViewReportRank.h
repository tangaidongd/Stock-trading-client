#ifndef _IO_VIEW_REPORT_RANK_H_
#define _IO_VIEW_REPORT_RANK_H_

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
// CIoViewReportRank
class CIoViewReportRank : public CIoViewBase, public CBlockConfigListener
{
// Construction 
public:
	CIoViewReportRank();
	virtual ~CIoViewReportRank();

	DECLARE_DYNCREATE(CIoViewReportRank)
	

	enum E_RankType{
		ERT_RisePercent = 0,	// �Ƿ�
		ERT_FallPercent,		// ����
		ERT_Amplitude,			// ���
		ERT_5MinRiseSpeed,		// 5��������
		ERT_5MinFallSpeed,		// 5���ӵ���
		ERT_VolRatio,			// ����
		ERT_BidRatioFirst,		// ί��ǰ
		ERT_BidRatioLast,		// ί�Ⱥ�
		ERT_Amout,				// �ܽ��
	};
public:
	// from CControlBase
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
public:
	// from CIoViewBase
	virtual void	SetChildFrameTitle();  // ��������ʾ �г� - ������
	virtual bool32	FromXml(TiXmlElement *pElement);		// xml����
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	
	virtual void	OnIoViewActive();						
	virtual void	OnIoViewDeactive();
	virtual bool32	GetStdMenuEnable(MSG* pMsg){return false;};		// ����Ҫ��׼�Ҽ��˵�

	virtual void	LockRedraw();				
	virtual void	UnLockRedraw();

	virtual CMerch *GetMerchXml();
	virtual E_IoViewType GetIoViewType() { return EIVT_MultiMerch; }
	virtual void	GetAttendMarketDataArray(OUT AttendMarketDataArray &aAttends);
public:	
	void			OnDblClick();				// ��ĳ����Ʒ����ѡ�ɲ���������

	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	
	// Ӧ������������Ľӿ�
	virtual void	OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs); // ��Ҫ��ɸѡ
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	// ��ǰ�����ݽӿ�Ӧ�ò����õ� - ������ʲô���ݽӿڰ� - �������κ����ݽṹ
	virtual void	OnVDataForceUpdate();
	
	virtual void	OnIoViewColorChanged();			
	virtual void	OnIoViewFontChanged();

	virtual void  OnBlockConfigChange(int32 iBlockId, E_BlockNotifyType eNotifyType);
	virtual void  OnConfigInitialize(E_InitializeNotifyType eInitializeType){};
 
	virtual	void	OnVDataReportInBlockUpdate(int32 iBlockId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount, const CArray<CMerch *, CMerch *> &aMerchs);
 	
public:
	void		OpenBlock(int32 iBlockId);		// �����г�id��Ψһ��
	bool32		ChangeRank(E_RankType eRank);

	void		SetShowBlockNameOnTitle(bool32 bShow = true);		// �������Ƿ���ʾblock

	bool32		GetCellCount(OUT int32 &iRowCount, OUT int32 &iColumnCount);
	bool32		GetFocusCell(OUT int32 &iRow, OUT int32 &iColumn);
	bool32		SetFocusCell(int32 iRow, int32 iColumn, bool32 bSetFocus);

private:
	bool IsHaveKlineTrendFrame();			// ��ǰ�����Ƿ���K�߻��߷�ʱ����

///////////////////////////////////////////////////////////////////////
private:
	struct T_Header		// û���ҵ�ת�������Ƶķ�����ֱ��д���ȣ�������������0��ʼ
	{
		E_MerchReportField	m_eField;
		bool32				m_bDescSort;	// ����_
		CString				m_StrName;

		T_Header()
		{
			m_eField = EMRFRisePercent;
			m_bDescSort = true;
			m_StrName = _T("�����Ƿ�����");
		}
	};
	typedef CArray<T_Header, const T_Header &>	HeaderArray;
	typedef	CArray<CMerch *, CMerch *>		MerchArray;
private:
	void			SetRowHeightAccordingFont();							
	void			SetColWidthAccordingFont();

	void			GetTitleString(OUT CString &StrTitle);

	void			UpdateTableContent( CMerch *pMerch );		// �費��Ҫ������Ʒ������
	void			UpdateTableAllContent();									// ���иı�

	void			RecalcLayout();

	void			DoTrackMenu(CPoint posScreen);

	void			ChangeRank(const T_Header &headerRank);

	CMerch          *GetSelectMerch();

	CBlockLikeMarket *GetCurrentBlock();
	CBlockLikeMarket *GetBlock(int32 iBlockId);
	
	void			RequestSortViewData(bool32 bForce =false);		// 
	void			RequestSortViewDataAsync();

	void			RequestRealTimePrice();

	void			RequestData(CMmiCommBase &req, bool32 bForce = false);			// ���������������

private:	
	CMmiReqBlockReport				m_MmiRequestSys;		// �ܻ���Ҫһ�����������

	CRect							m_RectTitle;
	// ������
	CGridCtrlNormalSys				m_GridCtrl;	
// 	CXScrollBar						m_XSBVert;			// ���ܹ���
// 	CXScrollBar						m_XSBHorz;
	
	bool32							m_bInitialized;		// �Ƿ��ʼ�����

	int32							m_iBlockId;			// ��ǰ�г�id

	int32							m_iMaxGridVisibleRow;		// ������ɼ�������

	MerchArray						m_aMerchs;			// ������Ʒ��������
	
	static HeaderArray				m_saHeaders;
	T_Header						m_headerCur;

	bool32							m_bShowBlockNameOnTitle;

	int32							m_iScreenHeight;	// ��Ļ�ĸ߶�
public:

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewReportRank)
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewReportRank)
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDoInitialize();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	
	afx_msg void OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridSelRowChanged( NMHDR *pNotifyStruct, LRESULT* pResult );

	afx_msg void OnGridGetDispInfo(NMHDR *pNotifyStruct, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_REPORT_RANK_H_
