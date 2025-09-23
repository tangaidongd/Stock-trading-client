#ifndef _DLG_DEFAULT_USER_REGISTER_01_29_
#define _DLG_DEFAULT_USER_REGISTER_01_29_

#include "NCButton.h"
#include <map>


////////////////////////////////////////////////////////////////////////////

class CDlgDefaultUserRegister : public CDialog
{
    // Construction
public:
    CDlgDefaultUserRegister(CWnd* pParent = NULL);   // standard constructor
    ~CDlgDefaultUserRegister();


public:
    enum { IDD = IDD_DIALOG_REGISTER};

private:
    void InitButtons();
    void AddCNButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
    void Draw(CDC& dc);
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    int TButtonHitTest(CPoint point);

    void ShowNewsDlg();

private:

    CNCButton m_btnClose;	// 首页工具栏按钮列表
    T_USERINFO m_UserInfo;
    int m_iXButtonHovering;		    // 标识鼠标进入按钮区域
    std::map<int, CNCButton> m_mapButton;

    Image           *m_pImgBk;
	Image           *m_pImgBtn;
protected:

    DECLARE_MESSAGE_MAP()

    afx_msg void OnPaint();
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

};

#endif