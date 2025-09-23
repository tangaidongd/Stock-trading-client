#ifndef _NEW_FUN_TB_WND_05_23_
#define _NEW_FUN_TB_WND_05_23_

#include <map>
using namespace std;

#include "NCButton.h"
#include "ToolBarIDDef.h"
#include "DlgPullDown.h"

/////////////////////////////////////////////////////////////////////////////

enum nStatus
{
    ButtonNormal,		// �����İ�ť״̬
    MosuePass,			// ��꾭��״̬
    ButtonSelected,		// ��ťѡ��״̬
	ButtonMore,			// ���ఴť��ʱ���ı�״̬
};

class CTableLobby
{
public:
	CString GetTableName() {return m_strName;}
	int		GetTableNId() {return m_nID;}
private:
    friend class CNewCaptionTBWnd;
    CTableLobby()
    {
        m_nType = ButtonNormal;
    }
    int		m_nID;
    CString	m_strName;
    CRect	m_rcRect;
    CRect	m_rcCloseRect;
    BYTE	m_nType;
};

class CNewCaptionTBWnd : public CWnd
{
    // Construction
public:

    typedef struct T_TBWndFont 
    {
    public:
        CString m_StrName;
        float   m_Size;
        int32   m_iStyle;

    }T_NcFont;

    CNewCaptionTBWnd(CWnd &wndParent);
    virtual ~CNewCaptionTBWnd();
public:
    static void AddPage(const CString& strName);
    void GetButtonRect();
    bool ShowCfm();
	static void GetCaptionArray(CStringArray &aStrCaptionArray);
	void CloseAllPage();

private:
    void DrawButtons(CDC& dc);
	void ShowMoreBtn();
	int32 GetTableLobbyGetBtnId(int32 &iID);

private:
    CWnd &m_wndParent;

    CRect m_rcButtonRect;	// ���а�ť������
    bool m_bMouseTracking;
    Image* m_pBkImg;  
	Image* m_pImageClose;
	Image* m_pImgMore;
	Image* m_pImgMenu;

	int32 m_iRightBtnPos;	// ����ʾ�����ұ߰�ťλ��	
	CTableLobby *m_pLobbyMore;
	CDlgPullDown *m_pDlgPullDown;

	buttonContainerType m_mapMoreBtn;	// ���ఴť�б�


    static HWND	m_hWndExsit;
    static std::vector<CTableLobby*> m_vecTableLobby;

protected:
    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    //{{AFX_MSG(CNewTBWnd)
    afx_msg	int	 OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg	BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg LRESULT OnAddPage(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};

#endif