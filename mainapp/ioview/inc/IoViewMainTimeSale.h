#ifndef _IO_VIEW_MAIN_TIMESALE_H_
#define _IO_VIEW_MAIN_TIMESALE_H_

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

/////////////////////////////////////////////////////////////////////////////
// CIoViewMainTimeSale
class CIoViewMainTimeSale : public CIoViewBase , public CBlockConfigListener
{
// Construction 
public:
	CIoViewMainTimeSale();
	virtual ~CIoViewMainTimeSale();

	DECLARE_DYNCREATE(CIoViewMainTimeSale)
	
public:
	// from CControlBase
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
public:
	// from CIoViewBase
	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);	// xml����
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	
	virtual void	OnIoViewActive();						
	virtual void	OnIoViewDeactive();
	virtual void	OnIoViewTabShow();
	virtual bool32	GetStdMenuEnable(MSG* pMsg);		// ����Ҫ��׼�Ҽ��˵� - Ĭ��

	virtual void	LockRedraw();
	virtual void	UnLockRedraw();

	virtual CMerch *GetMerchXml();
	virtual	E_IoViewType	GetIoViewType() { return EIVT_MultiMerch; } 
	virtual void	GetAttendMarketDataArray(OUT AttendMarketDataArray &aAttends);
public:	
	void			OnDblClick(CMerch *pMerch);				// ��ĳ����Ʒ����ѡ�ɲ���������

	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);

	// ����timesale�Ľӿ�
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch){}
	// ��ǰ�����ݽӿ�Ӧ�ò����õ� - ������ʲô���ݽӿڰ� - �������κ����ݽṹ
	virtual void	OnVDataForceUpdate();
	
	virtual void	OnIoViewColorChanged();			// ��������Ҫ����
	virtual void	OnIoViewFontChanged();

	// block config
	virtual	void	OnBlockConfigChange(int32 iBlockId, E_BlockNotifyType eNotifyType);	// һ������ѡ�ɱ��
	virtual	void	OnConfigInitialize(E_InitializeNotifyType eInitializeType);			// �ļ�����

	virtual	bool32	IsPluginDataView() { return true; }
	virtual void	OnVDataPluginResp(const CMmiCommBase *pResp);
	// ������ػذ�
	void			OnMainTimeSaleResp(const CMmiRespMainMonitor *pResp);

public:
	void		OpenTab(int32 iTabPress);		//

	bool32		IsDrawTitle()const  { return m_bDrawTitleString; }
	void		EnableDrawTitle(bool32 bDraw = TRUE) { m_bDrawTitleString = bDraw; }

///////////////////////////////////////////////////////////////////////
private:
	struct T_MainTimeSale
	{
		CMerch *m_pMerch;
		T_RespMainMonitor	m_mainMonitor;
	};
	typedef	CArray<CMerch *, CMerch *>		MerchArray;
	typedef CList<T_MainTimeSale *, T_MainTimeSale *>	MainTimeSaleList;
	typedef CArray<CMmiReqMainMonitor, const CMmiReqMainMonitor &>  ReqInitDataArray;

	enum {
		BtnAll = 0,		// ��ť�������
		Btn300,
		BtnUser,
		BtnCur,
		BtnFloat,
	};

	struct T_TabBtn
	{
		CRect	m_RectBtn;
		CString m_StrName;
		int32	m_iBlockId;

		bool32	m_bNeedReqPrivateData;

		T_TabBtn()
		{
			m_RectBtn.SetRectEmpty();
			m_iBlockId = -1;
			m_bNeedReqPrivateData = false;
		}
		T_TabBtn(const T_TabBtn &tab);
		const T_TabBtn &operator=(const T_TabBtn &tab);
	};
	
private:
	void			SetRowHeightAccordingFont();							
	void			SetColWidthAccordingFont();

	void			UpdateTableAllContent();

	void			RecalcLayout();

	void			InitializeBtnId();

public:
	int32           GetCurTab() { return m_iCurTab; }

private:
	bool32			GetCurTab(OUT int32 &iCurTab, OUT int32 &iBlockId);
	bool32			IsMerchInCurTab(CMerch *pMerch);
	
	void			RequestPushViewData(bool32 bForce = false);		// ������������
	void			RequestInitialData();		// �����������

	void			RequestTabSpecialData();	// ����tab��������

	void			RequestUserStockData();		// ������ѡ�������������
	void			RequestCurrentStockData();	// ����ǰ��Ʒ�����������
	
	bool32			GetCurBlockMerchs(OUT MerchArray &aMerchs);
	bool32			GetBlockMerchsByTabIndex(int32 iTab, OUT MerchArray &aMerchs);

	bool32			InsertNewMainTimeSale(CMerch *pMerch, const T_RespMainMonitor &mainTM, bool32 bAddToGrid = false);
	int32			InsertGridRow(int iRow, const T_MainTimeSale &mts, bool32 bDrawRow =false);
	void			UpdateGridRow(int iRow, const T_MainTimeSale &mts);
	void			DeleteRedundantHead(bool32 bDeleteInGrid = false);		// �õ������ͷ����Ϣ
	T_MainTimeSale  *CheckMTS(T_MainTimeSale *pMTS) const;						// ���ö����Ƿ����
	void			UpdateAllGridRow();

	void			RequestData(CMmiCommBase &req);			// ���������������

private:	
	CGridCtrlNormalSys				m_GridCtrl;
	CXScrollBar						m_XSBVert;
	CXScrollBar						m_XSBHorz;
	
	CRect							m_RectBtn;
	CArray<T_TabBtn, const T_TabBtn &>	m_aTitleBtns;		
	
	bool32							m_bInitialized;		// �Ƿ��ʼ�����

	int32							m_iCurTab;			// ��ǰѡ�еİ�ť

	bool32							m_bDrawTitleString;	// �Ƿ���� �������

	bool32							m_bParentIsDialog;	// �Ƿ�Ϊdialog����

	MainTimeSaleList				m_lstMainTimeSales;	// �����������ݶ�������

public:

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewMainTimeSale)
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewMainTimeSale)
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDoInitialize();
	//}}AFX_MSG
	
	afx_msg void OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_MAIN_TIMESALE_H_
