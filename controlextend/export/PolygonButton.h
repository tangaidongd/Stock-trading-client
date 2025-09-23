// NCButton.h: interface for the NCButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLYGONBUTTON_H__085740BE_F070_4C59_A19D_13F51712D45C__INCLUDED_)
#define AFX_POLYGONBUTTON_H__085740BE_F070_4C59_A19D_13F51712D45C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GdiPlusTS.h"
#include "dllexport.h"
/************************************************************************/
/* ����ͼ�ΰ�ť��														*/
/************************************************************************/
class CONTROL_EXPORT CPolygonButton  
{
public:
	typedef struct T_NcFont 
	{
public:
		CString	m_StrName;
		float   m_Size;
		int32	m_iStyle;		
		bool32	m_bHorCenter;
		bool32	m_bVerCenter;
		T_NcFont()
		{
			m_Size=0.0f;
			m_bHorCenter = true;
			m_bVerCenter = true;
		}
	}T_NcFont;

	enum CBtn_Style{ECStyle1=0,ECStyle2=1,ECStyle3};

	CPolygonButton();
	virtual ~CPolygonButton();

	// ���ư�ť
	virtual void DrawButton(Graphics *pGraphics);

	// ������ť
	void CreateButton(LPCTSTR lpszCaption, LPRECT lpRect, CWnd *pParent, UINT nID=0, int32 iShift=20, CBtn_Style style=ECStyle1);

	void Refresh();

	void SetParentWnd(CWnd* pWnd) { m_pParent = pWnd; }

public:
	// ���ð�ť����λ��
	void SetRect(LPCRECT lpRect)
	{
		ASSERT(lpRect);
		m_rcRect = lpRect;
	}

	// ���ð�ť����λ��
	void SetShift(unsigned int uiShift)
	{
		m_iShift = uiShift;
	}

	// ��ȡ��ť����λ��
	void GetRect(CRect &rcRect)
	{
		rcRect = m_rcRect;
	}

	// ���ָ�����Ƿ��ڰ�ť��
	BOOL PtInButton(CPoint &point);

	// ���ð�ť״̬
	void EnableButton(BOOL bEnable, bool32 bRefresh = FALSE)
	{
		m_bDisabled = !bEnable;
	//	m_pParent->InvalidateRect(m_rcRect);
		if (bRefresh)
		{
			Refresh();
		}
	}
	
	// ���ð�ťѡ��״̬
	void SetCheck(BOOL bCheck, BOOL bRefresh = true)
	{
		m_bCheck = bCheck;
		if (m_bSetFocus)
		{
			m_pParent->SetFocus();
		}

		if (!m_bDisabled && m_bVisiable)
		{
			m_pParent->PostMessage(WM_COMMAND, m_nID);
			if (bRefresh)
			{
				Refresh();
			}
		}
	}

	void SetCheckStatus(BOOL bCheck, BOOL bRefresh = true)
	{
		m_bCheck = bCheck;
		if (bRefresh)
		{
			Refresh();
		}
	}

	BOOL GetCheck()
	{
		return m_bCheck;
	}

	// ��ȡ��ťId
	UINT GetControlId()
	{
		return m_nID;
	}

	// ������ť�¼�
	void DoCommand()
	{
		if (!m_bDisabled && m_bVisiable)
		{
			m_bCheck = !m_bCheck;
		//	m_pParent->InvalidateRect(m_rcRect);
			m_pParent->PostMessage(WM_COMMAND, m_nID);
			Refresh();
		}
	}
	
	// ��갴�°�ť�¼�
	void LButtonDown(BOOL bRefresh = true)
	{
		if (!m_bDisabled && m_bVisiable)
		{
			m_bPressed = TRUE;
		//	m_pParent->InvalidateRect(m_rcRect);
			if (m_bSetFocus)
			{
				m_pParent->SetFocus();
			}

			if (bRefresh)
			{
				Refresh();
			}
		}
	}

	// �����°�ť�¼�
	void LButtonUp(BOOL bRefresh = true)
	{
		if (!m_bDisabled && m_bVisiable)
		{
			if (m_bPressed)
			{
			//	m_bCheck   = !m_bCheck;
				m_bPressed = FALSE;
			//	m_pParent->InvalidateRect(m_rcRect);
				m_pParent->PostMessage(WM_COMMAND, m_nID);
				if (bRefresh)
				{
					Refresh();
				}
			}
		}
	}

	// ����Ҽ����°�ť�¼�
	void RButtonUp(BOOL bRefresh = true)
	{
		if (!m_bDisabled && m_bVisiable)
		{
			m_pParent->PostMessage(WM_COMMAND, m_nID, 1);
			if (bRefresh)
			{
				Refresh();
			}
		}
	}

	// �����밴ť�����¼�
	void MouseHover(BOOL bRefresh = true)
	{
		if (!m_bDisabled && m_bVisiable)
		{
			BOOL bOldHover = m_bHovering;
			m_bHovering = TRUE; 
	//		m_pParent->InvalidateRect(m_rcRect);
			if (bRefresh && !bOldHover)
			{
				Refresh();
			}
		
		}
	}

	// ����뿪��ť�����¼�
	void MouseLeave(BOOL bRefresh = true)
	{
		if (!m_bDisabled && m_bVisiable)
		{
			BOOL bOldHover = m_bHovering;
			m_bPressed  = FALSE;
			m_bHovering = FALSE;
		//	m_pParent->InvalidateRect(m_rcRect);
			if (bRefresh && bOldHover)
			{
				Refresh();
			}	
		}
	}

	// ��������뿪״̬
	void SetMouseLeave()
	{
		m_bHovering = FALSE;
		m_bPressed  = FALSE;
	}

	// ��ť�Ƿ�������״̬
	BOOL IsNormal()
	{
		if (m_bHovering || m_bPressed)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}

	void SetPressed(BOOL bPressed)
	{
		m_bPressed = bPressed;
	}

	void SetCaption(CString StrCaption, bool32 bRefresh = true)
	{
		m_strCaption = StrCaption;
		if (bRefresh)
		{
			Refresh();
		}
	}

	CString GetCaption()
	{
		return m_strCaption;
	}

	void SetValue(CString strValue)
	{
		m_strParamValue = strValue;
	}

	CString GetValue()
	{
		return m_strParamValue;
	}

	void SetOpenCfm(int32 iOpenCfm)
	{
		m_iOpenCfm = iOpenCfm;
	}

	int32 GetOpenCfm()
	{
		return m_iOpenCfm;
	}

	void SetTextOffPos(CPoint pt)
	{
		m_point = pt;
	}

	void SetSubTextOffPos(CPoint pt)
	{
		m_subpoint = pt;
	}

	void SetTextColor(Color clrNormal, Color clrHot, Color clrPress)
	{
		m_clrNormalText = clrNormal;
		m_clrHotText = clrHot;
		m_clrPressText = clrPress;
	}

	void SetFont(T_NcFont& stFont)
	{
		m_Font = stFont;
	}

	void SetParentFocus(BOOL bFocus)
	{
		m_bSetFocus = bFocus;
	}

	void SetBtnBkgColor(COLORREF clrBtnBkg)
	{
		m_clrBtnBkg = clrBtnBkg;
	}
	
	void SetVisiable(BOOL bShow)
	{
		m_bVisiable = bShow;
	}

	BOOL IsVisiable()
	{
		return m_bVisiable;
	}

	BOOL IsEnable()
	{
		return m_bDisabled == TRUE ? FALSE : TRUE;
	}

	void SetTextFrameColor(Color clrNormal, Color clrHot, Color clrPress)
	{
		m_clrFrameNormalText = clrNormal;
		m_clrFrameHotText = clrHot;
		m_clrFramePressText = clrPress;
	}

	void SetTextBkgColor(Color clrNormal, Color clrHot, Color clrPress)
	{
		m_clrBkgNormalText = clrNormal;
		m_clrBkgHotText = clrHot;
		m_clrBkgPressText = clrPress;
	}

    void SetChangeTextColor(bool32 bChangeColor = true)
    {
		m_bChangeTextColor = bChangeColor == 0 ? false : true;;
    }

	void SetFont(LPCTSTR pFamily,
		float fFontSize,
		Gdiplus::FontStyle style= Gdiplus::FontStyleRegular,
		bool32 bHor =true,
		bool32 bVer =true)
	{
		if (NULL != pFamily)
		{
			m_Font.m_iStyle = style;
			m_Font.m_Size = fFontSize;
			m_Font.m_StrName = pFamily;
			m_Font.m_bHorCenter = bHor;
			m_Font.m_bVerCenter = bVer;
			::InvalidateRect(m_pParent->GetSafeHwnd(), &m_rcRect, FALSE);
		}		
		
	}

private:
	CRect	m_rcRect;		// ��ť����λ��
	BOOL	m_bHovering;	// ��ʶ�����밴ť����
	BOOL	m_bPressed;		// ��ʶ�Ѿ������ť
	BOOL	m_bDisabled;	// ��ʶ��ť�Ѿ�������
	BOOL	m_bCheck;		// ��ʶ��ť�Ѿ���ѡ��
	int		m_nID;			// ��ťId��
	CWnd	*m_pParent;		// ��ť������
	CString m_strCaption;	// ��ť�����ı�
	CString m_strParamValue;
	int32	m_iOpenCfm;		// ������ �Ƿ�򿪰���
	CPoint	m_point;		// �������ֵ����ƫ��λ��
	CPoint  m_subpoint;		// �ӱ������ֵ����ƫ��λ��
	Color	m_clrNormalText; // ����״̬������ɫ
	Color	m_clrHotText;    // ����״̬������ɫ
	Color	m_clrPressText;  // ѡ��״̬������ɫ
	T_NcFont m_Font;			
	BOOL	m_bSetFocus;	// ѡ��ʱ�������Ƿ��ý���
	BOOL	m_bVisiable;	// �Ƿ���ʾ
	COLORREF	m_clrBtnBkg; // ��ť�ı�����ɫ

	Color	m_clrFrameNormalText; // �ı���ť����״̬�߿���ɫ
	Color	m_clrFrameHotText;    // �ı���ť����״̬�߿���ɫ
	Color	m_clrFramePressText;  // �ı���ťѡ��״̬�߿���ɫ

	Color	m_clrBkgNormalText; // �ı���ť����״̬������ɫ
	Color	m_clrBkgHotText;    // �ı���ť����״̬������ɫ
	Color	m_clrBkgPressText;  // �ı���ťѡ��״̬������ɫ

    bool	m_bChangeTextColor;

	int32	m_iShift;			// ƽ���ı���ƫ����
	CBtn_Style	m_eStyle;		// ��ʽ

private:
	void CheckCaptionLen(CString &StrCaption,RectF& rcShow,RectF& rcStr);
	void ShowText(Graphics *pGraphics,RectF& rcShow);
};

#endif // !defined(AFX_POLYGONBUTTON_H__085740BE_F070_4C59_A19D_13F51712D45C__INCLUDED_)
