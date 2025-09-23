#ifndef _IOVIEW_MULTI_RANK_H_
#define _IOVIEW_MULTI_RANK_H_

//������ʾ���IoViewReport���Ҳ��Ե��ӿ�������ȫ���İ�ť

#include "IoViewBase.h"
#include "IoViewReportRank.h"

class CIoViewMultiRank : public CIoViewBase
{

    //Construction
public:
    CIoViewMultiRank();
    virtual ~CIoViewMultiRank();

    DECLARE_DYNCREATE(CIoViewMultiRank)

public:
    virtual void  OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch) {}
    virtual void  SetChildFrameTitle() {}                     // ��Ҫ�ܸ�IoViewManagerʶ������Ҫʵ��xml
    virtual bool32  FromXml(TiXmlElement *pElement);
    virtual CString  ToXml();
    virtual	CString  GetDefaultXML() { return L""; }
    virtual bool32	GetStdMenuEnable(MSG* pMsg);
    virtual CMerch *GetMerchXml();						// ת����ioview

private:
	typedef CArray<CIoViewReportRank *, CIoViewReportRank *>  SubIoViewArray;
	SubIoViewArray	m_aSubIoViews;
	int32		m_iBlockId;		// ���id

	CRect			m_RectTitle;

private:
    void RecalcLayout();
    void CreateRankView();
	bool32 OpenBlock( int32 iBlockId);

protected:
    afx_msg void OnPaint();
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);	// �˵�
    DECLARE_MESSAGE_MAP()
	
};

#endif