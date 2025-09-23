#ifndef _IOVIEW_MULTI_CYCLE_KLINE_H_
#define _IOVIEW_MULTI_CYCLE_KLINE_H_

//������ʾ���IoViewReport���Ҳ��Ե��ӿ�������ȫ���İ�ť

#include "IoViewBase.h"


class CIoViewKLine;

class CIoViewMultiCycleKline : public CIoViewBase
{

    //Construction
public:
    CIoViewMultiCycleKline();
    virtual ~CIoViewMultiCycleKline();

    DECLARE_DYNCREATE(CIoViewMultiCycleKline)

public:
    virtual void  OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
    virtual void  SetChildFrameTitle() {}                     // ��Ҫ�ܸ�IoViewManagerʶ������Ҫʵ��xml
    virtual bool32  FromXml(TiXmlElement *pElement);
    virtual CString  ToXml();
    virtual	CString  GetDefaultXML() { return L""; }
    virtual bool32	GetStdMenuEnable(MSG* pMsg);
    virtual CMerch *GetMerchXml();						// ת����ioview
	virtual void	OnIoViewActive();					// ת�Ӹ������ʵ��ioview
	virtual void	OnIoViewDeactive();

	CWnd*	FromPoint(CPoint pt);	//	��ѯ����ڵ�ͼ����ͼ
	void	DrawViewBorder(CDC* pDC, const CRect& rt, const COLORREF& clr);		//	���Ʊ߿�

private:

	CArray<CIoViewKLine *, CIoViewKLine *> 	m_aSubIoViews;

	CRect	    m_RectTopBar;

	std::map<int, CNCButton> m_mapTopBtn;			// ������������߹�����ť
	//int32		m_iTopBtnHovering;					// ��ʶ�����밴ť����
	bool32	    m_bTracking;						// ����뿪�ı��

	// Ŀǰ����������ͼΪ�ĸ�������������������������
	int32       m_iSubRowCount;					    // ����ͼ����
	int32       m_iSubColumnCount;					// ����ͼ����

	int32		m_iActiveIoView;		// ��ǰ���������ͼ����

	CRect			m_rtSel;					//	ͼ��ѡ�б߿�λ��
	COLORREF		m_clrSelBorder;				//	ͼ��ѡ�б߿�ɫ

private:
    void RecalcLayout();
    void CreateSubView();
	void DeleteSubIoViews();
	void AddTopButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption);
	int TTopButtonHitTest(CPoint point);
	void DrawTopButton();
	void InitialTopBtns();
	void SetActiveIoView(int32 iIndex);
	void RefreshButtonStatus();
	CIoViewBase * GetActiveIoView();

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    afx_msg void OnPaint();
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);	// �˵�
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg	LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnHotKeyMsg(WPARAM w, LPARAM l);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
    DECLARE_MESSAGE_MAP()
};

#endif