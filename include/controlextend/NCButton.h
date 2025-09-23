// NCButton.h: interface for the NCButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NCBUTTON_H__085740BE_F070_4C59_A19D_13F51712D45C__INCLUDED_)
#define AFX_NCBUTTON_H__085740BE_F070_4C59_A19D_13F51712D45C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GdiPlusTS.h"
#include "dllexport.h"
/************************************************************************/
/* 绘制图形按钮类														*/
/************************************************************************/
class CONTROL_EXPORT CNCButton  
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

	typedef enum E_SelectStockType
	{
		ESSTNone = 0,
		ESSTIndexSelStock,
		ESSTHotSelStock,
	} E_SelectStockType;

	typedef struct T_SelectStockParams
	{
		E_SelectStockType eType;
		UINT              uSelectFlags;
		
		T_SelectStockParams()
		{
			eType = ESSTNone;
			uSelectFlags = 0;
		}
	}T_SelectStockParams;

	// 大盘牛熊状态
	enum E_DaPanState
	{
		EDS0 = 0,
		EDS1,
		EDS2,
		EDS3,
		EDS4,

		EDSCount,
	};


	CNCButton();
	virtual ~CNCButton();

	// 绘制按钮
	virtual void DrawButton(Graphics *pGraphics);

	// 绘制系统按钮
	virtual void DrawSysButton(Graphics *pGraphics, int index);

	// 创建按钮
	void CreateButton(LPCTSTR lpszCaption, LPRECT lpRect, CWnd *pParent, Image *pImage, UINT nCount=4, UINT nID=0, Image *pImgHot=NULL, Image *pImgStatus=NULL, CString StrParam = L"");
	void Refresh();

	void SetParentWnd(CWnd* pWnd) { m_pParent = pWnd; }

	void SpliterSelStockFlags(CString StrParams);

	void UpdateSelStockStatus();

	void  SetSelectStockFlagAndDaPanStatus(DWORD dwIndexSelStockFlag, DWORD dwHotSelStockFlag, UINT eDaPanStatus);

public:
	// 设置按钮矩形位置
	void SetRect(LPCRECT lpRect)
	{
		ASSERT(lpRect);
		m_rcRect = lpRect;
	}
	
	// 设置按钮Offset数值
	void SetAreaOffset(LPCRECT lpRect)
	{
		ASSERT(lpRect);
		m_rcAreaOffset = lpRect;
	}

	// 获取按钮矩形位置
	void GetRect(CRect &rcRect)
	{
		rcRect.SetRect(0,0,0,0);
		if (!m_bDisabled && m_bVisiable)
		{
			rcRect = m_rcRect;
		}	
	}

	CRect GetRect()
	{
		return m_rcRect;
	}

	// 获取按钮Offset数值
	void GetAreaOffset(CRect &rcRect)
	{
		rcRect = m_rcAreaOffset;
	}

	// 检测指定点是否处于按钮内
	BOOL PtInButton(CPoint &point)
	{
		if (!m_bVisiable)
		{
			return FALSE;
		}
		else if (m_bDisabled)
		{
			return FALSE;
		}

		CRect rcControl = m_rcRect;
		rcControl.left   += m_rcAreaOffset.left;
		rcControl.right  -= m_rcAreaOffset.right;
		rcControl.top    += m_rcAreaOffset.top;
		rcControl.bottom -= m_rcAreaOffset.bottom;

		return rcControl.PtInRect(point);
	}

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
		m_pParent->PostMessage(WM_COMMAND, m_nID);
		if (bRefresh)
		{
			Refresh();
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

	// 取得按钮是否创建完成
	BOOL GetCreate()
	{
		return m_bCreate;
	}

	// 设置按钮没有创建完成
	void SetNotCreate()
	{
		m_bCreate = FALSE;
	}

	// 设置按钮创建完成
	void SetCreate()
	{
		m_bCreate = TRUE;
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


	void SetSubCaption(CString StrCaption, bool32 bRefresh = true)
	{
		m_strSubCaption = StrCaption;
		if (bRefresh)
		{
			Refresh();
		}
	}
	
	CString GetSubCaption()
	{
		return m_strSubCaption;
	}

	void SetDescription(CString StrDescripion)
	{
		m_StrDescription = StrDescripion;
	}
	
	CString GetDescription()
	{
		return m_StrDescription;
	}

	void SetValue(CString strValue)
	{
		m_strParamValue = strValue;
	}

	CString GetValue()
	{
		return m_strParamValue;
	}

	void SetImage(Image	*pImage, bool32 bRefresh = true)
	{
		if (NULL == pImage)
		{
			return;
		}

		if (m_pImage)
		{
			m_pImgBackup = m_pImage;
		}

		m_pImage = pImage;

		if (bRefresh)
		{
			Refresh();
		}
	}

	void SetBackupImage(Image *pImage = NULL)
	{
		m_pImgBackup = pImage;
	}

	void SetSysIndex(int index)
	{
		m_index = index;
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

	Image* GetImage() const
	{
		return m_pImage;
	}

	Image* GetBackupImage() const
	{
		return m_pImgBackup;
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

	void SetClientDC(bool32 bClient)
	{
		m_bClientDC = bClient;
	}
    
    void SetNeedDBuffering(bool32 bDBuffering)
    {
        m_bNeedDBuffering = bDBuffering;
    }

    void SetTextVertical(bool32 bVVertical=true)
    {
		m_bDirVertical = bVVertical==0 ? false: true;;
    }

    void SetChangeTextColor(bool32 bChangeColor = true)
    {
		m_bChangeTextColor = bChangeColor == 0 ? false : true;;
    }

	void SetImageCount(UINT uCnt, bool32 bRefresh = false)
	{
		m_nImageCount = uCnt;

		if (bRefresh)
		{
			Refresh();
		}
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
	
	void AddChildMenu(CWnd *pWnd)
	{
		m_pSubMenu = pWnd;
	}
	
	CWnd* GetChildMenu()
	{
		return m_pSubMenu;
	}

	void SetChangeStatus(bool bChangeStatus = false)
	{
		m_bChangeStatus = bChangeStatus;
	}

	bool GetChangeStatus()
	{
		return m_bChangeStatus;
	}

	void SetGroupId(int iID)
	{
		m_iGroupId = iID;
	}

	int GetGroupId()
	{
		return m_iGroupId;
	}

	void SetShowCaption(BOOL bShowCaption, bool32 bRefresh = false)
	{
		m_bShowCaption = bShowCaption;

		if (bRefresh)
		{
			Refresh();
		}
	}

	BOOL IsShowCaption() 
	{
		return m_bShowCaption ;
	}

	void SetCustomFrameLineSign(bool bIsCustom = false) { m_bIsCustomFrameLine = bIsCustom;}
	void SetDrawCustomLine(bool bDrawTop = true, bool bDrawLeft = true, bool bDrawRight = true, bool bDrawBottom = true)
	{
		m_bDrawTop		=  bDrawTop;
		m_bDrawLeft		= bDrawLeft;
		m_bDrawRight	= bDrawRight;
		m_bDrawBottom	= bDrawBottom;
	}

private:
	CRect	m_rcRect;		// 按钮矩形位置
	CRect   m_rcAreaOffset; // 按钮Offset数值
	BOOL	m_bHovering;	// 标识鼠标进入按钮区域
	BOOL	m_bPressed;		// 标识已经点击按钮
	BOOL	m_bDisabled;	// 标识按钮已经被禁用
	BOOL	m_bCheck;		// 标识按钮已经被选中
	BOOL    m_bShowCaption; // 是否显示文字
	Image	*m_pImage;		// 按钮图片
	Image	*m_pImgHot;		// 按钮上可以加个标签
	Image	*m_pImgStatus;  // 二级菜单左侧标签
	Image   *m_pImgBackup;  // 备份先前image
	UINT	m_nImageCount;	// 按钮图片中包含的图片个数
	int		m_nID;			// 按钮Id号
	CWnd	*m_pParent;		// 按钮父窗口
	CString m_strCaption;	// 按钮标题文本
	CString m_strSubCaption;// 按钮子标题文本
	CString m_StrDescription;// 按钮文字介绍
	BOOL	m_bCreate;		// 按钮是否创建完成
	int     m_index;
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

	CString m_strParamValue;

	bool32	m_bClientDC;

	// 子菜单的窗口指针（当CNCButton作为菜单项时，才具备该含义）
	CWnd   *m_pSubMenu;
    bool32  m_bNeedDBuffering;
    bool	m_bDirVertical;
    bool    m_bChangeTextColor;
	bool    m_bChangeStatus;

	// 用于保存选股参数
	T_SelectStockParams m_stSelStockParams;

	// 记录分组ID
	int     m_iGroupId;

	//
	DWORD m_dwIndexSelectStockFlag;
	DWORD m_dwHotSelectStockFlag;
	E_DaPanState m_eDaPanStatus;

	// 边框线
	bool m_bIsCustomFrameLine;
	bool m_bDrawTop;
	bool m_bDrawLeft;
	bool m_bDrawRight;
	bool m_bDrawBottom;


private:
	void CheckCaptionLen(CString &StrCaption,RectF& rcShow,RectF& rcStr);
	void ShowText(Graphics *pGraphics,RectF& rcShow);
};

#endif // !defined(AFX_NCBUTTON_H__085740BE_F070_4C59_A19D_13F51712D45C__INCLUDED_)
