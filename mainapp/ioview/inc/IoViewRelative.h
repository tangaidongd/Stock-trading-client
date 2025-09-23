#ifndef _IO_VIEW_RELATIVE_H_
#define _IO_VIEW_RELATIVE_H_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "CStaticSD.h"
#include "GuiTabWnd.h"
#include "ReportScheme.h"
#include "MerchManager.h"

///////////////////////////////////////////////////////
struct T_TableCheckInfo
{
public:
	T_TableCheckInfo() { m_pMerch = NULL; }

public:
	CMerch			*m_pMerch;
};

/////////////////////////////////////////////////////////////////////////////
// CIoViewRelative
class CIoViewRelative : public CIoViewBase
{
// Construction 
public:
	CIoViewRelative();
	virtual ~CIoViewRelative();

	DECLARE_DYNCREATE(CIoViewRelative)
	
	// from CControlBase
public:
	virtual BOOL	TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
	// from CIoViewBase
public:
	virtual void	SetChildFrameTitle();
	virtual bool32	FromXml(TiXmlElement *pElement);
	virtual CString	ToXml();
	virtual	CString GetDefaultXML();
	
	virtual void	OnIoViewActive();
	virtual void	OnIoViewDeactive();
	static const char *	GetXmlElementAttrCurTab();		
	static const char * GetXmlElementTabCheckInfo();	
	static const char * GetXmlElementAttrChecked();

public:
	void	    	SendInfomation(bool32 bAdd,int32 iRow,int32 iColoumn);
	// from CIoViewBase
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerchh);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerchh);
	virtual void	OnVDataForceUpdate();
	virtual void	OnVDataGridHeaderChanged(E_ReportType eReportType);
	virtual void	OnIoViewGroupIdChange(int32 iNewGroupId);
	virtual void	OnVDataRelativeMerchsUpdate();

	virtual bool32	OnVDataAddCompareMerch(IN CMerch *pMerchh);
	virtual void	OnVDataRemoveCompareMerch(IN CMerch *pMerchh);

			
	virtual void	OnIoViewColorChanged();
	virtual void    OnIoViewFontChanged();

private:
	void			OnTabSelChange();
	void			DeleteTableContent();
	void			UpdateTableContent();
	void			BuildTabWnd();
	void			SetCurTab(int32 iTab);
	// 根据字体大小,设置行高
	void			SetRowHeightAccordingFont();						
	// 更新指定行
	void			UpdateSpecifyTableRow(int32 iRow, bool32 bShowBlink);
	void			SaveColoumWidth();
	
///////////////////////////////////////////////////////////////////////
//
private:
	CXScrollBar		m_XSBVert;
	CXScrollBar		m_XSBHorz;
	CImageList		m_ImageList;
	CStaticSD		m_CtrlTitle;
	CGuiTabWnd		m_GuiTabWnd;
	CGridCtrlSys	m_GridCtrl;
	int32			m_iFixCol;
	int32			m_iTabFromXml;
	int32			m_iRowHeight;

	CArray<T_HeadInfo,T_HeadInfo>	m_ReportHeadInfoList;
	CArray<T_TableCheckInfo, T_TableCheckInfo&>		m_TableCheckInfo;
	CArray<CRelativeMerchNode, CRelativeMerchNode&>	m_AllRelativeMerchs;

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewRelative)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewRelative)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult);

	DECLARE_MESSAGE_MAP() 
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_RELATIVE_H_
