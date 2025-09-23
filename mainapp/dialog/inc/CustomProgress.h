#pragma once


// CCustomProgress



class CMemDC111 : public CDC
{
public:

	//构造内存设备场景
	CMemDC111(CDC* pDC) : CDC()
	{
		ASSERT(pDC != NULL);

		m_pDC = pDC;
		m_pOldBitmap = NULL;
		m_bMemDC = !pDC->IsPrinting();

		if (m_bMemDC)
		{
			pDC->GetClipBox(&m_rect);
			CreateCompatibleDC(pDC);
			m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
			m_pOldBitmap = SelectObject(&m_bitmap);
			SetWindowOrg(m_rect.left, m_rect.top);
		}
		else	
		{
			m_bPrinting = pDC->m_bPrinting;
			m_hDC		= pDC->m_hDC;
			m_hAttribDC = pDC->m_hAttribDC;
		}
	}

	//析构
	~CMemDC111()
	{
		if (m_bMemDC) {	
			m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
				this, m_rect.left, m_rect.top, SRCCOPY);

			SelectObject(m_pOldBitmap);
			DeleteDC();
			m_bitmap.DeleteObject();
		} else {
			m_hDC = m_hAttribDC = NULL;
		}
	}

	CMemDC111* operator->() {return this;}

	operator CMemDC111*() {return this;}

private:
	CBitmap  m_bitmap;		
	CBitmap* m_pOldBitmap;	
	CDC*     m_pDC;		
	CRect    m_rect;		
	BOOL     m_bMemDC;		
};



//进度条控件类
class CCustomProgress : public CProgressCtrl
{
	DECLARE_DYNAMIC(CCustomProgress)

public:
	CCustomProgress();
	virtual ~CCustomProgress();

	void SetRange(int nLower, int nUpper);
	int SetPos(int nPos);
	int SetStep(int nStep);
	int StepIt(void);

	//设置错误信息之后，需要复位错误信息
	int SetErroMsg(char*pMsg);
	void ResetErroMsg()					{m_IsErroMsg = FALSE;}
 
	// Set Functions
	void SetTextColor(COLORREF color)	{m_clrText = color;}
	void SetBkColor(COLORREF color)		{m_clrBkGround = color;}
	void SetStartColor(COLORREF color)	{m_clrStart = color;}
	void SetEndColor(COLORREF color)	{m_clrEnd = color;}

	// Show the percent caption
	void ShowPercent(BOOL bShowPercent = TRUE)	{m_bShowPercent = bShowPercent;}

	// Get Functions
	COLORREF GetTextColor(void)	{return m_clrText;}
	COLORREF GetBkColor(void)		 {return m_clrBkGround;}
	COLORREF GetStartColor(void)	{return m_clrStart;}
	COLORREF GetEndColor(void)	{return m_clrEnd;}

protected:
	void DrawGradient(CPaintDC *pDC, const RECT &rectClient, const int &nMaxWidth);	
	int m_nLower, m_nUpper, m_nStep, m_nCurrentPosition;
	COLORREF	m_clrStart, m_clrEnd, m_clrBkGround, m_clrText;
	BOOL m_bShowPercent;
	BOOL m_IsErroMsg;
	CString  m_szMsg;
	//{{AFX_MSG(CGradientProgressCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()


};


