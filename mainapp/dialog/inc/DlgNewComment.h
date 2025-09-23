#ifndef _DLG_NEW_COMMENT_H_
#define _DLG_NEW_COMMENT_H_

#include "DialogEx.h"
#include "WndCef.h"


#define IDC_MY_WEB_EXPLORER	4321	// ä¯ÀÀÆ÷¿Ø¼þµÄID

class CDlgNewCommentIE : public CDialogEx
{
    // Construction
public:
    CDlgNewCommentIE(CWnd* pParent = NULL);   // standard constructor
    ~CDlgNewCommentIE();

    virtual void OnClickClose();

protected:
    CWndCef *m_pWndCef;

private:
    CString m_strUrl;
    CString m_strTitle;

public:
    void ShowDlgIEWithSize( const CString &StrTitle, const CString &StrUrl, const CRect& rect, int bFlag = 0);
    void ShowDlgIESpecialSize(const CString &StrTitle,const CString &StrUrl,int bFlag = 0);
    void SetTitleUrl( const CString &StrTitle, const CString &StrUrl);
    void ShowWeb(const CRect& rc);	// ÏÔÊ¾ÍøÒ³ä¯ÀÀÆ÷

public:
    enum { IDD = IDD_DIALOG_NEW_COMMENT};

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:

    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnSize( UINT nType, int cx, int cy );
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    DECLARE_MESSAGE_MAP()
};

#endif