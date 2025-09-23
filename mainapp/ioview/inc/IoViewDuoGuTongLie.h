#ifndef _IOVIEWDUOGUTONGLIE_H_
#define _IOVIEWDUOGUTONGLIE_H_

// ���ͬ����ͼ
// ���ͬ���·�Ϊ���ͬ��K�����ʱ, �ڼ���Ϊk���£����õĶ��ͬ��ΪK�ߣ��ڼ���Ϊ��������£����õļ���Ϊ��ʱ
// ����µ�С��ͼ��groupȫ��Ϊ-1, ˫�������Ӧ��k����ʱҳ��, �����Ҽ��˵�
// ���K:
//		ͳһ����ָ�꣬PageUp/Downͳһ��Ʒ�л������������л���ǰ�۽�������ͼ��Ʒ
// ��ɷ�ʱ:
//		PageUp/Downͳһ��Ʒ�л������������л���ǰ�۽�������ͼ��Ʒ

#include "IoViewBase.h"

class CIoViewKLine;
class CIoViewTrend;
/////////////////////////////////////////////////////////////////////////////
// CIoViewDuoGuTongLie

class CIoViewDuoGuTongLie : public CIoViewBase
{

public:
	// ���ͬ�е����ֱ�����ʽ
	enum E_DuoGuTongLieType
	{
		EDGLT_KLINE,                    // K�߶��ͬ��
		EDGLT_TREND,					// ��ʱ���ͬ��
		EDGLT_KLINE_TREND,				// ��ʱK�߶��ͬ��
	};

	// Construction
public:
	CIoViewDuoGuTongLie();
	virtual ~CIoViewDuoGuTongLie();

	DECLARE_DYNCREATE(CIoViewDuoGuTongLie)

	// from CIoViewBase
public:
	virtual void	RequestViewData(){};
	virtual void	SetChildFrameTitle();				// ��Ҫ�ܸ�IoViewManagerʶ������Ҫʵ��xml
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();

	virtual bool32	GetStdMenuEnable(MSG* pMsg) { return true; };

	virtual void	OnIoViewActive();					// ת�Ӹ������ʵ��ioview
	virtual void	OnIoViewDeactive();

	virtual void	LockRedraw();						// ���������ioview
	virtual void	UnLockRedraw();

	virtual CMerch *GetMerchXml();						// ת����ioview
	virtual E_IoViewType GetIoViewType() { return EIVT_MultiMerch; }

	virtual	CMerch *GetNextMerch(CMerch* pMerchNow, bool32 bPre);

	virtual void	OnEscBackFrameMerch(CMerch *pMerch);

	// from CIoViewBase
public:

	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();

	virtual void	OnIoViewColorChanged();			
	virtual void	OnIoViewFontChanged();

	virtual	bool32	IsIoViewContainer() { return true; }		// ��IoView������ͼ�����滹��������ͼ
	virtual	CIoViewBase    *GetActiveIoView();

public:
	void		ShowViews(int iPicId, bool32 bChangeMerchByOldShow = false);	// ��ʾk�߻��߷�ʱ��ͼ�Ķ��ͬ��

	void		ShowViews(CIoViewBase *pIoViewSrc);		// ����ǰioview��ʾ���ڵĶ��ͬ����ͼ
	void		ShowNextViews();		// ��ʾ����һ�ֶ��ͬ����ͼ k->��ʱ ��ʱ->k

	void		SetBaseMerch(CMerch *pMerch);	// �趨��������Ʒ��ȷ�� pageup/down��mousewheel������Χ

	void		OnShowSubIoView();	// ������ʾ����ͼ, ��ת��ͼ����

	void		RefreshView(CMerch* pMerch);

	CMerch*     GetFirstMerch() { return m_pFirstMerch; }

	void		KLineCycleChange(UINT nID);

	void        ChangeDuoGuTongLieType(E_DuoGuTongLieType eType, int iVieRowCount, int iViewColCount);      // �л����ͬ�еı������ͣ�������Ҫ��ʾ����ͼ����
private:

	void			RecalcLayout();

	void			ChangeColorFont();

	void			SetActiveIoView(int32 iIndex);
	void			SetActiveIoView(CIoViewBase *pIoViewActive);

	void			GetShowIoViews(OUT CArray<CIoViewBase *, CIoViewBase *> &aIoViews);

	void			DeleteSubIoViews();

	void			AddKlineIndex(const CString &StrIndexName);

	void			OnPageUpDown(int32 iPage);	// �����Ϲ��������¹�

	// ���㴿���ַ���
	int32			GetUpRow(int32 iOldRow);
	int32			GetDownRow(int32 iOldRow);
	int32			GetLeftColumn(int32 iOldCol);
	int32			GetRightColumn(int32 iOldCol);

private:
	void AddTopButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption);
	int TTopButtonHitTest(CPoint point);
	void DrawTopButton();
	void InitialTopBtns();

	void  CreateSubViews();
	void ResetView(int32 iBtnID);
	void ChangeKLineCycle(int32 iBtnID);
	int32 FindKLineCycle( int32 eTimeInterval );
	void RefreshIndex();

	
	CWnd*	FromPoint(CPoint pt);	//	��ѯ����ڵ�ͼ����ͼ
	void	DrawViewBorder(CDC* pDC, const CRect& rt, const COLORREF& clr);		//	���Ʊ߿�

private:

	CArray<CIoViewKLine *, CIoViewKLine *>	m_aIoViewKlines;		// ���е�k����ͼ
	CArray<CIoViewTrend *, CIoViewTrend *>	m_aIoViewTrends;		// ���еķ�ʱ��ͼ

	int32			m_iActiveIoView;		// ��ǰ���������ͼ����
	UINT			m_nShowPicId;			// ��ǰ��ʾ��������ͼ

	CMerch			*m_pMerchBase;			// ������Ʒ

	E_DuoGuTongLieType  m_DGTLType;         // ���ͬ�е�����
	int32           m_iSubViewCount;        // һ����ʾ������ͼ����
	int32           m_iSubViewRowCount;     // һ������ͼ����
	int32           m_iSubViewColCount;     // һ������ͼ����

	CRect			m_RectTopBar;           // ����������ʾ��ʱ��K�����ڡ���ʾ������ͼ����
	std::map<int, CNCButton> m_mapTopBtn;			// ������������߹�����ť
	int32		    m_iTopBtnHovering;			// ��ʶ�����밴ť����
	bool32			m_bTracking;	// ����뿪�ı��
	UINT            m_uCurTimeInterval;        // ��ǰ��������

	CMerch         *m_pFirstMerch;  

	CRect			m_rtSel;					//	ͼ��ѡ�б߿�λ��
	COLORREF		m_clrSelBorder;				//	ͼ��ѡ�б߿�ɫ
	////////////////////////////////////////////////////////////
	//
protected:

	/////////////////////////////////////////////////////
	/////////////////////////////////////////////////////
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewDuoGuTongLie)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void	OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewDuoGuTongLie)
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg	LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnHotKeyMsg(WPARAM w, LPARAM l);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg	LRESULT	OnIdleUpdateCmdUI(WPARAM w, LPARAM l);

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif //_IOVIEWDUOGUTONGLIE_H_