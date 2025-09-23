#if !defined(_DLGLOGINNOTIFY_H_)
#define _DLGLOGINNOTIFY_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgloginnotify.h : header file
//

#include "DialogEx.h"

#include "AutoRichEditCtrl.h"
#include "OleRichEditCtrl.h"
#include "facescheme.h"
#include "NCButton.h"

#include <map>
using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CDlgLoginNotify dialog
class TiXmlElement;
class TiXmlDocument;

class CDlgLoginNotify : public CDialogEx
{
public:
	~CDlgLoginNotify();
	
	// 显示指定标题与信息，modaless对话框，该方法保证只有一条信息当前显示，bForceDisplay强制显示，不论用户前面是否选取了do not tip
	// 没有父窗口，显示的时候BringToTop
	static	void	DisplayNotification(const CString &StrTitle, const CString &StrMsg, bool32 bForceDispaly = false);
	
private:
	// 实际上决定显示与怎么显示的方法
	void		DisplayNotify(const CString &StrTitle, const CString &StrMsg, bool32 bForceDispaly = false);
	
	void		SetNotifyMsg(const CString& StrTitile, const CString& StrMsg);
	void	    InitialDisplay(); // 初始化richedit等，有可能根本不需要显示
	bool32		IsDoNotTipTodayInXml(); // 用户是否针对该title,msg，是否选择了don't tip today
	
	void          SaveDoNotTipToday();   // 当对话框隐藏时，如需要保存该信息，保存title与msg
	TiXmlElement  *InitialTiXml(INOUT TiXmlDocument &tiDoc);   // 初始化XML相关信息，返回rootElement
	TiXmlElement  *FindNoticeElementInXml(INOUT TiXmlDocument &tiDoc, bool32 bAddNoticeIfNotExist = false, bool32 bDefaultDoNotTip = false);				 // 根据title, msg查找XML中匹配的节点，如果没有，并且bAddIfExist TRUE，则添加返回
	TiXmlElement  *FindNoticeElementInXml();				 // 只查找版本，返回的是一个单独static的节点数据，这个函数只是为了简化调用，不能访问返回节点的父，兄弟，子等等
	
	//lxp add 2012-11-19
	//设置选择范围的字体和颜色
	bool  SetFont(long nbPos,long nePos,CFontNode fontNode,COLORREF clrText,bool IsBold = false,bool isItalic = false,bool isUnderLine = false);
	
	//查找关键字，并用当前内容替换关键字,并返回替换内容所在的id范围
	bool		DowithReplaceStr(CString strTemp,CString strText,long &nbPos,long &nePos);
	//lxp add end

private:
	void AddButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	int	 TButtonHitTest(CPoint point);	// 鼠标是否在按钮上

private:
	CString		m_StrTitle;
	CString		m_StrMsg;
	bool32		m_beDoNotTipToday;
	
	CString     m_StrXMLFileFullName;

	std::map<int, CNCButton> m_mapBtn;
	int			m_iXButtonHovering;			// 标识鼠标进入按钮区域

	Image		*m_pImgCheck;
	Image		*m_pImgUnCheck;
	
	enum{
		NOTIFY_BTN_TIPTODAY = 1000,
		NOTIFY_BTN_OK,
	};
	// Construction
private:
	CDlgLoginNotify(CWnd* pParent = NULL);   // standard constructor
	
	
	// Dialog Data
	//{{AFX_DATA(CDlgLoginNotify)
	enum { IDD = IDD_DIALOG_NOTIFY };
	//CAutoRichEditCtrl m_RichEdit;
	COleRichEditCtrl	m_RichEdit;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLoginNotify)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy(); 
	virtual void OnCancel();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgLoginNotify)	
	afx_msg void OnRichEditExLink(NMHDR* in_pNotifyHeader, LRESULT* out_pResult);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DLGLOGINNOTIFY_H_)
