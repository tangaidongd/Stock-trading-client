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
/* 绘制图形按钮类														*/
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

	// 绘制按钮
	virtual void DrawButton(Graphics *pGraphics);

	// 创建按钮
	void CreateButton(LPCTSTR lpszCaption, LPRECT lpRect, CWnd *pParent, UINT nID=0, int32 iShift=20, CBtn_Style style=ECStyle1);

	void Refresh();

	void SetParentWnd(CWnd* pWnd) { m_pParent = pWnd; }

public:
	// 设置按钮矩形位置
	void SetRect(LPCRECT lpRect)
	{
		ASSERT(lpRect);
		m_rcRect = lpRect;
	}

	// 设置按钮矩形位置
	void SetShift(unsigned int uiShift)
	{
		m_iShift = uiShift;
	}

	// 获取按钮矩形位置
	void GetRect(CRect &rcRect)
	{
		rcRect = m_rcRect;
	}

	// 检测指定点是否处于按钮内
	BOOL PtInButton(CPoint &point);

	// 设置按钮状态
	void EnableButton(BOOL bEnable, bool32 bRefresh = FALSE)
	{
		m_bDisabled = !bEnable;
	//	m_pParent->InvalidateRect(m_rcRect);
		if (bRefresh)
		{
			Refresh();
		}
	}
	
	// 设置按钮选定状态
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

	// 获取按钮Id
	UINT GetControlId()
	{
		return m_nID;
	}

	// 触发按钮事件
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
	
	// 鼠标按下按钮事件
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

	// 鼠标放下按钮事件
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

	// 鼠标右键按下按钮事件
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

	// 鼠标进入按钮区域事件
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

	// 鼠标离开按钮区域事件
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

	// 设置鼠标离开状态
	void SetMouseLeave()
	{
		m_bHovering = FALSE;
		m_bPressed  = FALSE;
	}

	// 按钮是否处于正常状态
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
	CRect	m_rcRect;		// 按钮矩形位置
	BOOL	m_bHovering;	// 标识鼠标进入按钮区域
	BOOL	m_bPressed;		// 标识已经点击按钮
	BOOL	m_bDisabled;	// 标识按钮已经被禁用
	BOOL	m_bCheck;		// 标识按钮已经被选中
	int		m_nID;			// 按钮Id号
	CWnd	*m_pParent;		// 按钮父窗口
	CString m_strCaption;	// 按钮标题文本
	CString m_strParamValue;
	int32	m_iOpenCfm;		// 参数， 是否打开版面
	CPoint	m_point;		// 标题文字的起点偏移位置
	CPoint  m_subpoint;		// 子标题文字的起点偏移位置
	Color	m_clrNormalText; // 正常状态文字颜色
	Color	m_clrHotText;    // 高亮状态文字颜色
	Color	m_clrPressText;  // 选中状态文字颜色
	T_NcFont m_Font;			
	BOOL	m_bSetFocus;	// 选中时父窗口是否获得焦点
	BOOL	m_bVisiable;	// 是否显示
	COLORREF	m_clrBtnBkg; // 按钮的背景颜色

	Color	m_clrFrameNormalText; // 文本按钮正常状态边框颜色
	Color	m_clrFrameHotText;    // 文本按钮高亮状态边框颜色
	Color	m_clrFramePressText;  // 文本按钮选中状态边框颜色

	Color	m_clrBkgNormalText; // 文本按钮正常状态背景颜色
	Color	m_clrBkgHotText;    // 文本按钮高亮状态背景颜色
	Color	m_clrBkgPressText;  // 文本按钮选中状态背景颜色

    bool	m_bChangeTextColor;

	int32	m_iShift;			// 平行四边形偏移量
	CBtn_Style	m_eStyle;		// 样式

private:
	void CheckCaptionLen(CString &StrCaption,RectF& rcShow,RectF& rcStr);
	void ShowText(Graphics *pGraphics,RectF& rcShow);
};

#endif // !defined(AFX_POLYGONBUTTON_H__085740BE_F070_4C59_A19D_13F51712D45C__INCLUDED_)
