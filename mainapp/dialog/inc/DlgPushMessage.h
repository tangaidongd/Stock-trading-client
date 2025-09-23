#ifndef _DLG_SHOW_PUSH_MESSAGE_
#define _DLG_SHOW_PUSH_MESSAGE_


////////////////////////////////////////////////////////////////////////////

struct T_pushMsg
{
    CString m_StrMsgType;
    CString m_StrTitle;
    CString m_StrContent;
    CString m_StrContentUrl;

    T_pushMsg()
    {
        m_StrMsgType = L"";
        m_StrTitle = L"";
        m_StrContent = L"";
        m_StrContentUrl = L"";
    }
};

class CDlgPushMessage : public CDialog
{
    // Construction
public:
    CDlgPushMessage(CWnd* pParent = NULL);   // standard constructor
    ~CDlgPushMessage();

public:
    enum { IDD = IDD_DIALOG_PUSH_MESSAGE};


private:
    void InitButtons();
    int TButtonHitTest(CPoint point);
    void Draw(CDC& dc);
    void DrawText(Graphics *pGraphics);
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

public:
    void InvalidateMsg(T_pushMsg stMsg);
    void SetMsg(T_pushMsg stMsg);
    void ShowDialog();
	void HideDialog();
	void SetMsgTypeString(CString m_strMsgType);
private:
    
    T_pushMsg m_stMsg;   
	Image *m_pImgExit;
    CRect m_rectContent;
	CString  m_strMsgType;		// 增加立即下单按钮类型
	CRect    m_rcOrder;			// 下单按钮的位置

    CNCButton m_btnClose;
    int m_iXButtonHovering;
    std::map<int, CNCButton> m_mapButton;

protected:
    DECLARE_MESSAGE_MAP()

    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg LRESULT OnPrintClient(WPARAM, LPARAM);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

};

#endif