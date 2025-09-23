#if !defined(AFX_IOVIEWTEXTNEWS_H__EC88B3DD_75A6_4BB0_A900_8DF4545DF62D__INCLUDED_)
#define AFX_IOVIEWTEXTNEWS_H__EC88B3DD_75A6_4BB0_A900_8DF4545DF62D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IoViewTextNews.h : header file
//
#include "IoViewBase.h"
#include "XScrollBar.h"
#include "ListEx.h"
#include "NCButton.h"
#include "WndCef.h"

/////////////////////////////////////////////////////////////////////////////
// CIoViewTextNews view
typedef CArray<T_NewsInfo, T_NewsInfo&> listNewsInfoPtr;

//���Ÿ��½ṹ
typedef struct T_NewsInfoSAUpdate
{
public:
	int32		m_iIndexID;			// ��Ѷid
	//CString	m_StrContent;		// ��������
	//CString	m_StrTimeUpdate;	// ����ʱ��
	int32       m_curUdateTimes;    // ��˸����
	int32       m_nRow;
	int32       m_iInfoIndex;
}T_NewsInfoSAUpdate;

typedef CArray<T_NewsInfoSAUpdate, T_NewsInfoSAUpdate&> listNewsInfoUpdate;


class CIoViewTextNews : public CIoViewBase
{
protected:
	CIoViewTextNews();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CIoViewTextNews)

// Attributes
public:
	// from CControlBase
public:
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
	// from CIoViewBase:
public:
	virtual bool32	FromXml(TiXmlElement * pElement);
	virtual CString	ToXml();
	virtual CString	GetDefaultXML();
	virtual void	SetChildFrameTitle();
	
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();
	
	virtual void	LockRedraw();
	virtual void	UnLockRedraw();
	
	virtual void	OnVDataForceUpdate();
	virtual void	RequestViewData();
	
	void            ReuestPushData(bool bPushUpdate = true);
	// ���巢���仯
	virtual void	OnIoViewFontChanged();
	virtual void	OnIoViewColorChanged();
	//lxp ֪ͨ�������
	virtual bool UiNotifyUpdate();

	virtual void	AdditionalAfterPaste();

	virtual void	OnVDataNewsTitleUpdate(IN listNewsInfo& listTitles);
	virtual void	OnVDataNewsContentUpdate(const T_NewsInfo& stNewsInfo);
	virtual void	OnVDataNewsPushTitle(const T_NewsInfo& stNewsInfo);

	// ֪ͨ��ͼ�ı��ע����Ʒ
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	
	virtual bool32	GetStdMenuEnable(MSG* pMsg) { return false;}
private:
	void			RequestViewDatFrom(int nBegin);

	CSize			GetMinGridPriceSize(const bool &bSingle = FALSE);
	
	void			ReaclcLayOut();
	
	void			CreateTables();
	
	void			DeleteTableContent();

	void			SetTableDataContent(IN listNewsInfoPtr& listTitles);

	void			SetRowValue(T_NewsInfo *pNewInfo, int32 iRowIndex,int32 iColInex, int32 iInfoIndex);

	void			SetRowHeightAccordingFont();
	void			SetGridColumnsWidth();

	void            SetColorAndFont(bool bFont = true);

	CSize           GetGridSize();

	void            UpdateDataFromScroll(int nPos,bool bForceUpdate = FALSE);

	void            ClearData();
	void            ClearDataToday();

	void			SetNewUpateTimes(int nRow,int nTimes, int32 iInfoIndex);

	COLORREF		GetAniMovingColor(COLORREF ClrStart, COLORREF ClrEnd,int nStep);

	CSize           GetFontSize(CString strValue, CFont *pFont);

	void	        SetTableDataContentTodayComment( IN listNewsInfoPtr& listTitles);

	void            CalcMaxGridRowCount();

	void			InitialTopBtns();
	void			AddTopButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	void			DrawTopButton();
	int				TTopButtonHitTest(CPoint point);

private:

	CXScrollBar		   m_XSBVert;
	CXScrollBar		   m_XSBHorz;
	CXScrollBar		   m_XSBVertToday;
	CXScrollBar		   m_XSBHorzToday;

	CListEx			   m_GridNewList;	
	CListEx			   m_GridTodayList;

	T_NewsInfo         m_nDeitaNew;

	int32              m_nOldID;

	CPoint             m_ptScroll;

	bool               m_bAddItem;
	bool               m_bAddItemToday;//����
	UINT               m_nTiemrID;
	listNewsInfoPtr	   m_listNewsTitle;
	listNewsInfoPtr    m_listTodayComment;//����
	UINT               m_unPushTimer;

	CSize              m_szTextSize;

	listNewsInfoUpdate m_listNewUpdate;
	listNewsInfoUpdate m_listNewUpdateToday;//����

	int             m_nNewUpdateList;

	bool            m_bUpdating;

	DWORD           m_dwGetDataTick;
	DWORD           m_dwGetDataTickToday;

	listNewsInfoPtr	 m_SearchNewsTitle;

	int              m_iCtlSpace; //�ؼ�����
	int              m_iStartPost;

	CRect            m_rectTitle;//�����

	LOGFONT          m_TodayFont;
	int32			 m_iIDJingping;

	std::map<int, CNCButton> m_mapTopBtn;	// ������ť
	int32			m_iTopBtnHovering;		// ��ʶ�����밴ť����
	int32			m_iLeftWidth;			// top ��߰�ť�ܿ��
	int32			m_iTabHeight;			// Tab�ĸ߶� 
	bool32			m_bShowTab;				// �Ƿ���ʾtab
	int32			m_iCurrJpSelRow;		// ��ǰ����ѡ�е���

	CWndCef			m_wndCef;

	enum{
		NEWS_JP_BTN,
		NEWS_GJS_BTN,
		NEWS_CJRL_BTN,
		NEWS_MORE_BTN,
	};

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewTextNews)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CIoViewTextNews();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewTextNews)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg LRESULT OnListItemClick( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnListIsFocus( WPARAM wParam, LPARAM lParam );
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg LRESULT OnScrollPosChange(WPARAM w, LPARAM l);
	//}}AFX_MSG
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClickTodayComment(NMHDR *pNotifyStruct, LRESULT* /*pResult*/);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IOVIEWTEXTNEWS_H__EC88B3DD_75A6_4BB0_A900_8DF4545DF62D__INCLUDED_)
