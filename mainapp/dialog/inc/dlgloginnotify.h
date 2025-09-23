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
	
	// ��ʾָ����������Ϣ��modaless�Ի��򣬸÷�����ֻ֤��һ����Ϣ��ǰ��ʾ��bForceDisplayǿ����ʾ�������û�ǰ���Ƿ�ѡȡ��do not tip
	// û�и����ڣ���ʾ��ʱ��BringToTop
	static	void	DisplayNotification(const CString &StrTitle, const CString &StrMsg, bool32 bForceDispaly = false);
	
private:
	// ʵ���Ͼ�����ʾ����ô��ʾ�ķ���
	void		DisplayNotify(const CString &StrTitle, const CString &StrMsg, bool32 bForceDispaly = false);
	
	void		SetNotifyMsg(const CString& StrTitile, const CString& StrMsg);
	void	    InitialDisplay(); // ��ʼ��richedit�ȣ��п��ܸ�������Ҫ��ʾ
	bool32		IsDoNotTipTodayInXml(); // �û��Ƿ���Ը�title,msg���Ƿ�ѡ����don't tip today
	
	void          SaveDoNotTipToday();   // ���Ի�������ʱ������Ҫ�������Ϣ������title��msg
	TiXmlElement  *InitialTiXml(INOUT TiXmlDocument &tiDoc);   // ��ʼ��XML�����Ϣ������rootElement
	TiXmlElement  *FindNoticeElementInXml(INOUT TiXmlDocument &tiDoc, bool32 bAddNoticeIfNotExist = false, bool32 bDefaultDoNotTip = false);				 // ����title, msg����XML��ƥ��Ľڵ㣬���û�У�����bAddIfExist TRUE������ӷ���
	TiXmlElement  *FindNoticeElementInXml();				 // ֻ���Ұ汾�����ص���һ������static�Ľڵ����ݣ��������ֻ��Ϊ�˼򻯵��ã����ܷ��ʷ��ؽڵ�ĸ����ֵܣ��ӵȵ�
	
	//lxp add 2012-11-19
	//����ѡ��Χ���������ɫ
	bool  SetFont(long nbPos,long nePos,CFontNode fontNode,COLORREF clrText,bool IsBold = false,bool isItalic = false,bool isUnderLine = false);
	
	//���ҹؼ��֣����õ�ǰ�����滻�ؼ���,�������滻�������ڵ�id��Χ
	bool		DowithReplaceStr(CString strTemp,CString strText,long &nbPos,long &nePos);
	//lxp add end

private:
	void AddButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	int	 TButtonHitTest(CPoint point);	// ����Ƿ��ڰ�ť��

private:
	CString		m_StrTitle;
	CString		m_StrMsg;
	bool32		m_beDoNotTipToday;
	
	CString     m_StrXMLFileFullName;

	std::map<int, CNCButton> m_mapBtn;
	int			m_iXButtonHovering;			// ��ʶ�����밴ť����

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
