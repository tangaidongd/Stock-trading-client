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
/* ����ͼ�ΰ�ť��														*/
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

	// ����ţ��״̬
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

	// ���ư�ť
	virtual void DrawButton(Graphics *pGraphics);

	// ����ϵͳ��ť
	virtual void DrawSysButton(Graphics *pGraphics, int index);

	// ������ť
	void CreateButton(LPCTSTR lpszCaption, LPRECT lpRect, CWnd *pParent, Image *pImage, UINT nCount=4, UINT nID=0, Image *pImgHot=NULL, Image *pImgStatus=NULL, CString StrParam = L"");
	void Refresh();

	void SetParentWnd(CWnd* pWnd) { m_pParent = pWnd; }

	void SpliterSelStockFlags(CString StrParams);

	void UpdateSelStockStatus();

	void  SetSelectStockFlagAndDaPanStatus(DWORD dwIndexSelStockFlag, DWORD dwHotSelStockFlag, UINT eDaPanStatus);

public:
	// ���ð�ť����λ��
	void SetRect(LPCRECT lpRect)
	{
		ASSERT(lpRect);
		m_rcRect = lpRect;
	}
	
	// ���ð�ťOffset��ֵ
	void SetAreaOffset(LPCRECT lpRect)
	{
		ASSERT(lpRect);
		m_rcAreaOffset = lpRect;
	}

	// ��ȡ��ť����λ��
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

	// ��ȡ��ťOffset��ֵ
	void GetAreaOffset(CRect &rcRect)
	{
		rcRect = m_rcAreaOffset;
	}

	// ���ָ�����Ƿ��ڰ�ť��
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

	// ȡ�ð�ť�Ƿ񴴽����
	BOOL GetCreate()
	{
		return m_bCreate;
	}

	// ���ð�ťû�д������
	void SetNotCreate()
	{
		m_bCreate = FALSE;
	}

	// ���ð�ť�������
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
	CRect	m_rcRect;		// ��ť����λ��
	CRect   m_rcAreaOffset; // ��ťOffset��ֵ
	BOOL	m_bHovering;	// ��ʶ�����밴ť����
	BOOL	m_bPressed;		// ��ʶ�Ѿ������ť
	BOOL	m_bDisabled;	// ��ʶ��ť�Ѿ�������
	BOOL	m_bCheck;		// ��ʶ��ť�Ѿ���ѡ��
	BOOL    m_bShowCaption; // �Ƿ���ʾ����
	Image	*m_pImage;		// ��ťͼƬ
	Image	*m_pImgHot;		// ��ť�Ͽ��ԼӸ���ǩ
	Image	*m_pImgStatus;  // �����˵�����ǩ
	Image   *m_pImgBackup;  // ������ǰimage
	UINT	m_nImageCount;	// ��ťͼƬ�а�����ͼƬ����
	int		m_nID;			// ��ťId��
	CWnd	*m_pParent;		// ��ť������
	CString m_strCaption;	// ��ť�����ı�
	CString m_strSubCaption;// ��ť�ӱ����ı�
	CString m_StrDescription;// ��ť���ֽ���
	BOOL	m_bCreate;		// ��ť�Ƿ񴴽����
	int     m_index;
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

	CString m_strParamValue;

	bool32	m_bClientDC;

	// �Ӳ˵��Ĵ���ָ�루��CNCButton��Ϊ�˵���ʱ���ž߱��ú��壩
	CWnd   *m_pSubMenu;
    bool32  m_bNeedDBuffering;
    bool	m_bDirVertical;
    bool    m_bChangeTextColor;
	bool    m_bChangeStatus;

	// ���ڱ���ѡ�ɲ���
	T_SelectStockParams m_stSelStockParams;

	// ��¼����ID
	int     m_iGroupId;

	//
	DWORD m_dwIndexSelectStockFlag;
	DWORD m_dwHotSelectStockFlag;
	E_DaPanState m_eDaPanStatus;

	// �߿���
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
