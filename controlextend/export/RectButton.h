#ifndef _RECT_BUTTON_H_
#define _RECT_BUTTON_H_

#pragma warning(push)
#pragma warning(disable:4786)

#include <map>
using std::map;
#include <vector>
using std::vector;

#include "TBWndBase.h"
#include "ColorStep.h"

/***********************************
* 示例
class CRectButtonParentExample
{
	// 获取其DC并且释放
	CDC		*GetDrawNudeDC();
	void	ReleaseDrawNudeDC(CDC *pDC);

	// 控件id
	int		GetDlgCtrlID();
	HWND	GetSafeHwnd();

    // Rect转换
	BOOL	ScreenToRectBtnPt(CPoint *pt);		// 屏幕坐标到按钮坐标的转换 !!!绘制时并不做坐标转换，假设按钮坐标与dc坐标一致	

	// focus
	CRectButton *GetCaptureBtn();						// 当前是否有按钮获取了focus
	void	RequestMouseCapture(CRectButton &btn);		// 按钮申请mouse capture
	void	ReleaseMouseCapture(CRectButton &btn);		// 按钮申请释放先前申请的mouse capture
};

  // 实现这些后还需要在:
  //		1. onsize时分配按钮大小
  //		2. onpaint时调用draw绘制
  //		3. PreTranslateMsg中将消息传递给按钮
  // 按钮在触发时通过 UM_RECTBUTTON_NOTIFY WPARAM=id=分配的按钮id   LPARAM = RECT_BTN_NOTIFY_ITEM *
  // 结构 RECT_BTN_NOTIFY_ITEM 通知Owner窗口,notifyCode=NM_CLICK(一般情况下)/NM_RCLICK
**************************************
*/

#define UM_RECTBUTTON_NOTIFY	(WM_USER+0x1222)

template<class ParentType> class CRectRadioBtnGroup;

template<class RectButtonParent>
class CONTROL_EXPORT CRectButton
{
public:
	typedef CRectRadioBtnGroup<RectButtonParent> RectRadioBtnGroup;
	typedef map<int, CString> MapStr;

	enum E_BtnDrawState{		// 按钮绘制状态 - 
		Normal,				// 正常状态
		Focus,				// 鼠标移动 - 在Select状态下，不会进入focus
		Selected,			// selected / pressed - 由于目前使用LButtonDown代替click，所以是没有select，而且整个也没有下压的效果- -
		Disable				// 禁止 - 禁止状态下，不会发送任何消息
	};

	// 按钮行为
	enum E_BtnBehavior{
		BTNB_NORMAL,		// 普通按钮，只有click，无check
		BTNB_RADIO,			// radioButton，同组中只有一个能被选中
		BTNB_CHECKBOX,		// CheckBox，单击一下，选中/非选中切换
		BTNB_SEPARATOR,		// 分割条，没有click事件，只画一个背景
	};

	// 颜色索引 - 高16位为所属类型，中8位分配给按钮具体的状态(NFSD)，低8为索引，如背景1、2
	enum E_BtnColorType{
		BTN_COLOR_Background	=	0,			
		BTN_COLOR_Frame			=   1,
		BTN_COLOR_Text			=	2,
		BTN_COLOR_OTHER_START	=   3,
		BTN_COLOR_OTHER_END     =   0xffff
	};

	enum E_BackgroundColorIndex{
		BTN_Background1,
		BTN_Background2
	};

	enum E_DrawStyle{
		Draw_Style1,		// 几种绘制的方法
		Draw_Style2
	};

	enum E_ClickLBtnDrawStyle{
		ClickAfterLButtonUpDraw,		// 是否在发出click消息后绘制按钮更新状态
		ClickBeforLButtonUpDraw,
	};

	// Marlett char几个常用的字符
	enum E_MarlettChar{
		Marlett_Begin = '0'-1,				// '0' 为第一个(不包含前面，包含0)
		Marlett_Min = 48,					//  48 Min
		Marlett_MAX,						//  49 max
		Marlett_RESTORE,					//  50 Restore
		Marlett_CHECKMARK=98,				//  98 Checkmark 
		Marlett_Bullet = 105,				// 105 Bullet
		Marlett_Close = 114,				// 114 Close
		Marlett_Help,						// ?
		Marlett_UpTriangle,					// 方向向上的三角
		Marlett_DownTroangle,				// 向下的三角
		Marlett_End = 'z'					// 'z'为最后一个(不包含)
	};

	// 通知
	typedef struct tagRECT_BTN_NOTIFY_ITEM
	{
		NMHDR	hdr;			// 这个里面的内容是tbwnd parent内容
		uint16	uBtnId;			// 具体触发的btn
		CRectButton	*pBtn;		
		
		LPARAM	lParam;			// 附带数据 - 比如wsp按钮的文件名...
	}RECT_BTN_NOTIFY_ITEM;


	// 构造s
	CRectButton(RectButtonParent &wndTB)
		: m_wndTB(wndTB)
	{
		Initial(NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL);
		//m_wndTB.OnNewBtn(*this);
	}
	// hWndOwner - 接收命令窗口, nID - 该button特有的ID, pszText该button的显示文字
	// pszTip - 提示文字
	// 自己用文字绘制
	CRectButton(RectButtonParent &wndTB, CWnd *pWndOwner, uint16 nID, LPCTSTR pszText, LPCTSTR pszTip = NULL) 
		:m_wndTB(wndTB)
	{
		Initial(pWndOwner, nID, pszText, NULL, NULL, NULL, NULL, pszTip);
		//m_wndTB.OnNewBtn(*this);
	}

	// 自己用文字绘制 - 使用marllet字体绘制字符ch
	CRectButton(RectButtonParent &wndTB, CWnd *pWndOwner, uint16 nID, E_MarlettChar chMarllet, LPCTSTR pszTip = NULL) 
		:m_wndTB(wndTB)
	{
		TCHAR chs[2];
		chs[0] = (TCHAR)chMarllet;
		chs[1] = '\0';
		Initial(pWndOwner, nID, chs, NULL, NULL, NULL, NULL, pszTip);
		SetUseMarlettChar(true);
	}

	// 贴图绘制 - 正常、选择(压下)、鼠标移动、禁止
	CRectButton(RectButtonParent &wndTB, CWnd *pWndOwner, uint16 nID, Image *pImageNomal, Image *pImageSelected, Image *pImageFocus, Image *pImageDisable, LPCTSTR pszTip = NULL)
		:m_wndTB(wndTB)
	{
		Initial(pWndOwner, nID, NULL, pImageNomal, pImageSelected, pImageFocus, pImageDisable, pszTip);
		//m_wndTB.OnNewBtn(*this);
	}

	// 贴图绘制 + 文字，文字浮于上方 - 正常、选择(压下)、鼠标移动、禁止
	CRectButton(RectButtonParent &wndTB, CWnd *pWndOwner, uint16 nID, LPCTSTR pszText, Image *pImageNomal, Image *pImageSelected, Image *pImageFocus, Image *pImageDisable, LPCTSTR pszTip = NULL)
		:m_wndTB(wndTB)
	{
		Initial(pWndOwner, nID, pszText, pImageNomal, pImageSelected, pImageFocus, pImageDisable, pszTip);
		//m_wndTB.OnNewBtn(*this);
	}

	// 数据初始化
	void	Initial(CWnd *pWndOwner, uint16 nID, LPCTSTR pszText, Image *pImageNomal, Image *pImageSelected, Image *pImageFocus, Image *pImageDisable, LPCTSTR pszTip)
	{
		m_pWndOwner		= pWndOwner;
		m_nID			= nID;
		m_StrText		= pszText;
		m_StrTip		= pszTip;
		m_pImageNomal	= pImageNomal;
		m_pImageSelected = pImageSelected;
		m_pImageFocus	= pImageFocus;
		m_pImageDisable = pImageDisable;
		
		m_RectButton.SetRectEmpty();
		
		m_eBtnState     = Normal;
		
		m_eBtnBehavior	= BTNB_NORMAL;
		m_pRadioGroup	= NULL;
		m_bCheck		= false;
		
		LOGFONT lf = {0};
		lf.lfHeight = 12;
		_tcscpy(lf.lfFaceName, _T("新宋体"));
		m_font.CreateFontIndirect(&lf);
		
		m_bUseMarllet = false;
		
		m_iGroupId = -1;
		
		m_eDrawStyle = Draw_Style1;
		
		m_eClickStyle = ClickAfterLButtonUpDraw;
		
		m_bInLButtonUp = false;
		
		// 初始化各个颜色
		COLORREF clrStartN = RGB(250,250,250);
		COLORREF clrEndN   = RGB(190,190,190);
		COLORREF clrStartS = RGB(140,140,140);
		COLORREF clrEndS   = clrEndN;
		COLORREF clrFrameN = RGB(110,110,110);
		COLORREF clrFrameF = RGB(205,104,65);
		COLORREF clrTextN   = RGB(0,0,0);
		COLORREF clrTextS   = RGB(255,0,0);
		COLORREF clrTextD   = GetSysColor(COLOR_GRAYTEXT);
		SetColor(BTN_COLOR_Background, BTN_Background1, clrStartN, _T("背景边沿"), false);		// 设置所有
		SetColor(BTN_COLOR_Background, BTN_Background2, clrEndN,   _T("背景中间"), false);
		SetColor(BTN_COLOR_Background, Selected, BTN_Background1, clrStartS); // 特例selected
		SetColor(BTN_COLOR_Background, Selected, BTN_Background2, clrEndS);
		
		SetColor(BTN_COLOR_Frame, 0, clrFrameN, _T("边框"), false);
		SetColor(BTN_COLOR_Frame, Focus, 0, clrFrameF);
		
		SetColor(BTN_COLOR_Text, 0, clrTextN, _T("文字"), false);
		SetColor(BTN_COLOR_Text, Focus, 0, clrFrameF);
		SetColor(BTN_COLOR_Text, Selected, 0, clrTextS);
		SetColor(BTN_COLOR_Text, Disable, 0, clrTextD);
	}

	// 析构
	virtual ~CRectButton()
	{
		if ( m_pRadioGroup )
		{
			m_pRadioGroup->RemoveRadioBtn(this);
		}
	}

	// 行为
	// 改变button的行为！！！尽量在初始化的时候调用 pGroup - Radio需要一个组来协调其它radio的check状态
	void		SetBtnBehavior(E_BtnBehavior eNewBehavior, CRectRadioBtnGroup *pGroup = NULL) 
	{
		E_BtnBehavior eOld = m_eBtnBehavior;
		if ( BTNB_RADIO == eOld )
		{
			if ( m_pRadioGroup )
			{
				m_pRadioGroup->RemoveRadioBtn(this);
			}
		}
		m_eBtnBehavior	=	eNewBehavior;
		if ( BTNB_RADIO == eNewBehavior )
		{
			ASSERT( pGroup && !m_pRadioGroup );		// 只能加入一次组
			m_pRadioGroup = pGroup;
			if ( m_pRadioGroup )
			{
				SetCheck(false);		// 新进入组，则强制将check为false
				m_pRadioGroup->AddRadioBtn(this);
			}
		}
	}

	// 只有 checkbox / radio 能够设置
	void		SetCheck(bool32 bCheck)				
	{
		//ASSERT( IsEnable() );
		if ( BTNB_RADIO == m_eBtnBehavior || BTNB_CHECKBOX == m_eBtnBehavior )
		{
			bool32	bOld = m_bCheck;
			m_bCheck = bCheck;
			if ( m_bCheck != bOld )
			{
				DrawNude();
			}
			if ( m_bCheck && BTNB_RADIO == m_eBtnBehavior )
			{
				if ( m_pRadioGroup )
				{
					m_pRadioGroup->IamCheckBtn(this);
				}
			}
		}
		else
		{
			ASSERT( 0 );
		}
	}

	bool32		GetCheck() const
	{
		if ( BTNB_RADIO == m_eBtnBehavior || BTNB_CHECKBOX == m_eBtnBehavior )
		{
		}
		else
		{
			ASSERT( 0 );
		}
		return m_bCheck;
	}

	// 基本属性等
	E_BtnBehavior	GetBtnBehavior()const { return m_eBtnBehavior; };				// 获取按钮行为
	void		SetUseMarlettChar(bool32 bUse = false){ m_bUseMarllet = bUse; }		// 设置是否使用MarlettChar字体来进行标题图标的绘制 - lpszText会被当成MarlettChar
	bool32		IsSeparator()const { return BTNB_SEPARATOR == m_eBtnBehavior;  };	// 是否为分割条

	// 设置
	void		SetBtnText(LPCTSTR pcszText){ m_StrText = pcszText; };		// 目前只提供这些属性操作
	void		SetBtnTextWithDraw(LPCTSTR lpszText)
	{
		if ( m_StrText != lpszText )
		{
			m_StrText = lpszText;
			DrawNude();
		}
	}
	void		SetBtnTip(LPCTSTR pcszTip){ m_StrTip = pcszTip; };
	void		SetBtnGroupId(int iGroupId) { m_iGroupId = iGroupId; };
	void		SetDrawStyle(E_DrawStyle eStyle) { m_eDrawStyle = eStyle; } 
	void		SetClickStyle(E_ClickLBtnDrawStyle eStyle) { m_eClickStyle = eStyle; }
	void		SetBtnImages(Image *pImageNomal, Image *pImageSelected = NULL, Image *pImageFocus = NULL, Image *pImageDisable = NULL)
	{
		m_pImageNomal = pImageNomal;
		m_pImageSelected = pImageSelected;
		m_pImageFocus = pImageFocus;
		m_pImageDisable = pImageDisable;
	}
	void	SetFont(CFont &font)
	{
		LOGFONT lf = {0};
		font.GetLogFont(&lf);
		m_font.DeleteObject();
		m_font.CreateFontIndirect(&lf);
	}
	void	SetBtnState(E_BtnDrawState eState, bool32 bForce = false)		// 设置按钮新的状态
	{
		ASSERT( m_eBtnBehavior != BTNB_SEPARATOR );
		E_BtnDrawState eOld = m_eBtnState;
		if ( Selected == m_eBtnState && Focus == eState && !bForce )
		{
			// focus状态不能改变选择状态
		}
		else
		{
			m_eBtnState = eState;
		}
		
		if ( eOld != m_eBtnState )
		{
			// 		TRACE(_T("SetBtnState: %d->%d\r\n"), eOld, m_eBtnState);
			// 		if ( eOld == Selected )
			// 		{
			// 			int i = 0;
			// 		}
			DrawNude();
		}
	}
	

	// 获取
	CString		GetBtnText() const { return m_StrText; };
	CString		GetBtnTip() const { return m_StrTip; };
	uint16		GetBtnId() const { return m_nID; };
	int			GetBtnGroupId() const  { return m_iGroupId; };
	E_DrawStyle	GetDrawStyle()const { return m_eDrawStyle; }
	E_ClickLBtnDrawStyle GetClickStyle() const { return m_eClickStyle; }
	void		GetBtnImages(Image **ppImageNomal, Image **ppImageSelected = NULL, Image **ppImageFocus = NULL, Image **ppImageDisable = NULL) const;
	const CFont &GetFont() const { return m_font; }
	E_BtnDrawState GetBtnState() const { return m_eBtnState; }
	RectButtonParent	&GetBtnParent() { return m_wndTB; }

	// 测量
	virtual		CSize   GetBtnTextExtend(CDC *pDC)		// 根据文字获取可能需要的大小
	{
		bool32	bNeedReleaseDC = false;
		if ( pDC == NULL )
		{
			pDC = m_wndTB.GetDrawNudeDC();
			if ( NULL == pDC )
			{
				return CSize(0,0);
			}
			bNeedReleaseDC = true;
		}
		CSize sizeText(0,0);
		CFont *pFontOld = pDC->SelectObject(&m_font);
		CString StrText = m_StrText;
		if ( StrText.IsEmpty() )
		{
			StrText = _T("高度测试");
		}
		sizeText = pDC->GetTextExtent(StrText);
		pDC->SelectObject(m_font);
		if ( bNeedReleaseDC )
		{
			m_wndTB.ReleaseDrawNudeDC(pDC);
			pDC = NULL;
		}
		
		sizeText.cy += 2*2;
		if ( m_StrText.IsEmpty() )
		{
			sizeText.cx = 2*2;
		}
		if ( BTNB_SEPARATOR == m_eBtnBehavior )
		{
			sizeText.cx = 2;
		}
		
		return sizeText;
	}
	CSize	MeasureBtnRect(CDC *pDC, OUT bool32 &bFixed)			// 获取该Rect希望获取的大小, bFixed暗示是否应当固定大小
	{
		bFixed  = false;
		bool32 bDeleteDC = false;
		if ( NULL == pDC )
		{
			pDC = m_wndTB.GetDrawNudeDC();
			if ( NULL == pDC )
			{
				ASSERT( 0 );
				return CSize(0,0);
			}
			bDeleteDC = true;
		}
		
		// 根据正常图片或者文字来决定
		CSize sizeRet(0,0);
		Image *pImageNormal = m_pImageNomal;
		if ( NULL != pImageNormal )
		{
			sizeRet.cx = pImageNormal->GetWidth() + 2;	// 边框
			sizeRet.cy = pImageNormal->GetHeight() + 2;
			if ( sizeRet.cy < 20 )
			{
				sizeRet.cy = 20;	// 最小高度20
			}
			bFixed = true;
		}
		else
		{
			CFont *pFontOld = pDC->SelectObject(&m_font);
			sizeRet = pDC->GetTextExtent(m_StrText);
			pDC->SelectObject(&m_font);
			sizeRet.cx += 14;
			sizeRet.cy += 6;
		}
		if ( bDeleteDC )
		{
			m_wndTB.ReleaseDrawNudeDC(pDC);
			pDC = NULL;
		}
		return sizeRet;
	}
	void	GetBtnRect(OUT CRect &RectBtn){RectBtn = m_RectButton;}
	// 现在recalcLayout等外部还是调用这个函数来分配矩形吧，避免填色的时候死循环
	void	MoveBtn(const CRect &RectBtn, bool32 bDraw = true)			
	{
		m_RectButton = RectBtn;
		m_RectButton.NormalizeRect();
		if ( m_RectButton.Width() > 2000 || m_RectButton.Height() > 2000 )
		{
			ASSERT( 0 );
			m_RectButton.SetRectEmpty();
		}
		
		if ( bDraw )
		{
			DrawNude();
		}
	}

	// 颜色管理
	// 存储按钮各种颜色的map
	TBWndColorItemMap &GetBtnColorsRef() { return m_mapColorItems ; };
	bool32	GetColor(E_BtnColorType eType, E_BtnDrawState eState, int32 iIndex, COLORREF &clr)		// 根据当前状态获取特定类型的颜色
	{
		int iColorIndex = CombineBtnColorIndex(eType, eState, iIndex);
		if ( m_mapColorItems.count(iColorIndex) > 0 )
		{
			clr = m_mapColorItems[iColorIndex].m_clr;
			return true;
		}
		else
		{
			// 尝试有没有Normal状态的 / index 0 的
			
			iColorIndex = CombineBtnColorIndex(eType, Normal, iIndex);
			if ( m_mapColorItems.count(iColorIndex) > 0 )
			{
				clr = m_mapColorItems[iColorIndex].m_clr;
				return true;
			}
			else
			{
				iColorIndex = CombineBtnColorIndex(eType, Normal, 0);
				clr = m_mapColorItems[iColorIndex].m_clr;
				return true;
			}
			
			ASSERT( 0 ); // 所有颜色都至少初始化Normal
			return false;
		}
	}
	void	SetColor(E_BtnColorType eType, E_BtnDrawState eState, int32 iIndex, COLORREF clr, LPCTSTR lpszDesc=NULL, bool32 bNeedDraw = false)	// 如果是第一次的话，需要输入描述字串
	{
		int iColorIndex = CombineBtnColorIndex(eType, eState, iIndex);
		if ( m_mapColorItems.count(iColorIndex) > 0 )
		{
			if ( m_mapColorItems[iColorIndex].m_clr != clr )
			{
				m_mapColorItems[iColorIndex].m_clr = clr;
				if ( bNeedDraw && m_eBtnState == eState)
				{
					DrawNude();
				}
			}
		}
		else
		{
			// 初始化
			ASSERT( NULL != lpszDesc && _tcslen(lpszDesc) > 0 );
			CString StrDesc = lpszDesc;
			if ( StrDesc.IsEmpty() )
			{
				StrDesc.Format(_T("颜色%x"), iColorIndex);
			}
			CTBWndColorValue	btnClr(this, StrDesc, clr);
			m_mapColorItems[iColorIndex] = btnClr;
		}
	}
	void	SetColor(E_BtnColorType eType, int32 iIndex, COLORREF clr, LPCTSTR lpszDesc=NULL, bool32 bNeedDraw=false)	// 将4个状态的都设置同样颜色
	{
		MapStr	Strs;
		Strs[Normal]   = _T("(Normal)");
		Strs[Focus]    = _T("(Focus)");
		Strs[Selected] = _T("(Press/Selected)");
		Strs[Disable]  = _T("(Disable)");
		
		
		int iColorIndex = CombineBtnColorIndex(eType, Normal, iIndex);
		
		CString StrDesc = lpszDesc == NULL ? _T("") : lpszDesc;
		if ( StrDesc.IsEmpty() )
		{
			StrDesc.Format(_T("颜色%x"), iColorIndex);
		}
		for ( MapStr::iterator it=Strs.begin(); it!=Strs.end(); it++ )
		{
			SetColor(eType, (E_BtnDrawState)it->first, iIndex, clr, StrDesc+it->second, bNeedDraw);
		}
	}
	int		CombineBtnColorIndex(E_BtnColorType eType, E_BtnDrawState eState, int32 iIndex)	// 返回按照规则编制的索引
	{
		return (int)((eType<<16) | ((eState << 8)&0xff00) | ((iIndex)&0xff));
	}
	void	SetBtnColors(const TBWndColorItemMap &mapColors, bool32 bDrawIfChange = true)			// 设置按钮配色
	{
		bool32 bDraw = false;
		for ( TBWndColorItemMap::const_iterator it=m_mapColorItems.begin();
		it != m_mapColorItems.end();
		it++)
		{
			TBWndColorItemMap::const_iterator itSet = mapColors.find(it->first);
			if ( itSet != mapColors.end() )		// 有就进行更改设置
			{
				m_mapColorItems[it->first] = itSet->second;
				bDraw = true;
			}
		}
		
		if ( bDraw && bDrawIfChange )
		{
			DrawNude();
		}
	}
	void	GetBtnColors(OUT TBWndColorItemMap &mapColor) const {mapColor = m_mapColorItems;}

	// 行为控制
	// 目前并真的是click，而是LButtonDown
	virtual     bool32	DoClicked(CPoint ptClick)		// 用户点击了该按钮 - 发送通知给hWndOwner，返回hWndOwner处理返回，默认返回true，则将btnState设置为select
	{
		// 只有在该矩形范围内的pt才能传进来
		if ( !m_RectButton.PtInRect(ptClick) )
		{
			ASSERT( 0 );
			return false;
		}
		
		bool32 bNotify = false;
		if ( BTNB_CHECKBOX == m_eBtnBehavior )
		{
			bool32 bCheck = !m_bCheck;
			bNotify = NotifyOwner(NM_CLICK, NULL);		// check & radio应该是先check，然后在通知，应该完善很多通知条件
			if ( bNotify )
			{
				SetCheck(bCheck);
			}
		}
		else if ( BTNB_RADIO == m_eBtnBehavior )
		{
			ASSERT( !m_bCheck );		// 只能是uncheck - > check
			if ( !m_bCheck )
			{
				bNotify = NotifyOwner();
				if ( bNotify )
				{
					SetCheck(true);
				}
			}
		}
		else		// 普通按钮
		{
			ASSERT( IsEnable() );
			bNotify =  NotifyOwner();
		}
		
		return bNotify;
	}
	
	bool32	IsEnable() const								// 是否可接收消息 - 如不能接收则TBWnd不会发送鼠标消息给这个按钮
	{
		return Disable != m_eBtnState && BTNB_SEPARATOR != m_eBtnBehavior;		// 分割条相当于禁止了
	}
	void	EnableBtn(bool32 bEnable = true)
	{
		if ( m_wndTB.GetCaptureBtn() == this )
		{
			m_wndTB.ReleaseMouseCapture(*this);		// 申请释放鼠标,wndTB作出释放处理
		}
		
		if ( bEnable && !IsEnable() )
		{
			SetBtnState(Normal);
		}
		else if ( !bEnable && IsEnable() )
		{
			SetBtnState(Disable);
		}
		// else // 已经是要求的状态了，不用处理
	}

	// 鼠标消息，click由按钮自己产生 - 还真不如自己创建windows button到上面
	// 如果Parent收到mouseleave，最后一个聚焦按钮应该收到point(-1,-1)的mousemove用来结束mousemove状态
	virtual		void	OnMouseMove(UINT nFlag, CPoint point)		
	{
		// 普通move， lButton Press后move， mouseLeave后发送的-1-1move
		ASSERT( IsEnable() );
		if ( !IsEnable() )
		{
			return;
		}
		if ( m_RectButton.PtInRect(point) )		// 状态可以尝试状态栈处理
		{
			if ( Selected != m_eBtnState )		
			{
				SetBtnState(Focus);		// 普通move
			}
			// else // lButton press后move - 不改变状态
		}
		else
		{
			if ( Selected != m_eBtnState )
			{
				SetBtnState(Normal);		// 普通离开该区域
			}
			else if ( m_wndTB.GetCaptureBtn() != this )
			{
				ASSERT( m_wndTB.GetCaptureBtn() == NULL );
				if ( !m_bInLButtonUp )
				{
					SetBtnState(Normal);		// 虽然有按下标志，但是这个按钮并没有capture，还原
				}
			}
			// else // lButton后的离开 - 已经在capture了，不需要改变
		}
	}
	virtual		void	OnLButtonDown(UINT nFlag, CPoint point)	// 此时需要capturemouse - 先不实现capture，只设置一下显示效果，up的时候就直接click
	{
		ASSERT( IsEnable() );
		if ( !IsEnable() )
		{
			return;
		}
		if ( m_RectButton.PtInRect(point) )
		{
			SetBtnState(Selected);
			m_wndTB.RequestMouseCapture(*this);				// capture
			ASSERT( m_wndTB.GetCaptureBtn() == this );
		}
		else
		{
			//ASSERT( 0 );  // 不是在该按钮区域，不应当收到down消息 - 全转发
		}
	}
	virtual     void	OnLButtonUp(UINT nFlag, CPoint point)		// 此时需要请求TBWnd依情况releasemouse
	{
		ASSERT( IsEnable() );
		if ( m_wndTB.GetCaptureBtn() == this )
		{
			m_wndTB.ReleaseMouseCapture(*this);		// 申请释放鼠标,wndTB作出释放处理
		}
		if ( !IsEnable() )
		{
			return;
		}
		m_bInLButtonUp = true;
		
		if ( m_RectButton.PtInRect(point) )
		{
			if ( BTNB_RADIO == m_eBtnBehavior && m_bCheck )	// radio check不做click
			{
				
			}
			else if ( Selected == m_eBtnState )	// 必须先按下
			{
				if ( m_eClickStyle == ClickBeforLButtonUpDraw || BTNB_CHECKBOX == m_eBtnBehavior )
				{		
					DoClicked(point);		// 点击事件产生
					SetBtnState(Normal);		// 还在按钮上
				}
				else
				{
					SetBtnState(Normal);		// 还在按钮上
					DoClicked(point);		// 点击事件产生
					
				}
			}
			else
			{
				SetBtnState(Normal);		// 还在按钮上
			}
		}
		else
		{
			SetBtnState(Normal);	// 离开了
		}
		m_bInLButtonUp = false;
	}
	virtual		void	OnRButtonUp(UINT nFlag, CPoint point)		// 简单点，只接收鼠标右键弹出，貌似压下不是很必要capture
	{
		// 不设置任何状态 - 可能将来有对工作区改名呀什么操作的需要这个事件
		ASSERT( IsEnable() );
		
		// 改色操作
		if ( m_RectButton.PtInRect(point) )
		{
			NotifyOwner(NM_RCLICK, NULL);
		}
	}

	// Parent PreTranslateMsg将消息发到这里，这里将分发鼠标按下，弹出，移动，离开并处理
	void	RelayEventMsg(MSG *pMsg)
	{
		ASSERT( NULL!=pMsg );

		// 不能处理或者不是父窗口的消息
		if ( !IsEnable() || pMsg->hwnd != m_wndTB.GetSafeHwnd()  )
		{
			return;
		}
		
		if ( WM_LBUTTONDOWN!=pMsg->message
			&& WM_LBUTTONUP!=pMsg->message
			&& WM_MOUSEMOVE!=pMsg->message
			&& WM_RBUTTONUP!=pMsg->message
			&& WM_MOUSELEAVE!=pMsg->message
			&& WM_NCLBUTTONDOWN!=pMsg->message
			&& WM_NCLBUTTONUP!=pMsg->message
			&& WM_NCMOUSEMOVE!=pMsg->message
			&& WM_NCRBUTTONUP!=pMsg->message
			&& 0x02A2!=pMsg->message	//WM_NCMOUSELEAVE
			)
		{
			return;
		}

		CPoint ptRect = pMsg->pt;
		if ( !m_wndTB.ScreenToRectBtnPt(&ptRect) )
		{
			ASSERT( 0 );
			return;
		}

		switch (pMsg->message)
		{
		case WM_LBUTTONDOWN:
		case WM_NCLBUTTONDOWN:
			OnLButtonDown((UINT)pMsg->wParam, ptRect);
			break;
		case WM_LBUTTONUP:
		case WM_NCLBUTTONUP:
			OnLButtonUp((UINT)pMsg->wParam, ptRect);
			break;
		case WM_MOUSEMOVE:
		case WM_NCMOUSEMOVE:
			OnMouseMove((UINT)pMsg->wParam, ptRect);
			break;
		case WM_MOUSELEAVE:
		case 0x2A2:	//WM_NCMOUSELEAVE
			OnMouseMove(0, CPoint(-1,-1));
			break;
		}
	}

	
	// 绘制
	virtual		void	Draw(CDC &dc)					// 根据state绘制Button，包含边框绘制
	{
		switch (m_eDrawStyle)
		{
		case Draw_Style2:
			DrawStyle2(dc);
			break;
		default:
			DrawStyle1(dc);
		}
	}
	void	DrawNude()
	{
		if ( !m_RectButton.IsRectEmpty() )
		{
			CDC *pDC = m_wndTB.GetDrawNudeDC();
			if ( NULL!=pDC )
			{
				Draw(*pDC);
			}
			m_wndTB.ReleaseDrawNudeDC(pDC);
		}
	}
	

protected:
	friend RectRadioBtnGroup;

	// button发送消息格式 SendMessage(hWndOwner, nMessageNotifyId, LPARAM)
	// 利用wndTB消息通知格式，通知 hWndOwner click
	virtual		bool32	NotifyOwner(UINT nNotifyCode = NM_CLICK, LPARAM lParam = NULL)						
	{
		HWND hWndOwner = m_pWndOwner->GetSafeHwnd();
		if ( IsEnable() && IsWindow(hWndOwner) )
		{
			HWND hWndTB = m_wndTB.GetSafeHwnd();
			RECT_BTN_NOTIFY_ITEM item = {0};
			item.hdr.idFrom = m_wndTB.GetDlgCtrlID();
			item.hdr.hwndFrom = hWndTB;
			item.hdr.code = nNotifyCode;
			item.lParam = lParam;
			item.uBtnId = m_nID;
			item.pBtn = this;
			return SendMessage(m_pWndOwner->GetSafeHwnd(), UM_RECTBUTTON_NOTIFY, m_nID, (LPARAM)&item);
		}
		// 非窗口，不应当有消息发送
		ASSERT( 0 );
		return false;
	}
	
	// 绘制方式1
	void	DrawStyle1(CDC &dc)
	{
		if ( m_RectButton.IsRectEmpty() )
		{
			return;
		}
		
		if ( m_RectButton.Width() <0 || m_RectButton.Width() > 20000
			|| m_RectButton.Height() <0 || m_RectButton.Height() > 20000)
		{
			ASSERT( 0 );
			return;
		}
		int iSave = dc.SaveDC();
		
		CRect rcDraw(m_RectButton);
		//ASSERT( rcDraw.Width() < 200 && rcDraw.Width() >= 0 );
		//ASSERT( rcDraw.Height() < 200 && rcDraw.Height() >= 0 );
		dc.SelectObject(&m_font);
		
		bool32 bColorOK = true;
		COLORREF clrBK1;
		COLORREF clrBK2;
		bColorOK = GetColor(BTN_COLOR_Background, m_eBtnState, BTN_Background1, clrBK1) && bColorOK;
		bColorOK = GetColor(BTN_COLOR_Background, m_eBtnState, BTN_Background2, clrBK2) && bColorOK;
		
		COLORREF clrText;
		COLORREF clrFrame;
		bColorOK = GetColor(BTN_COLOR_Frame, m_eBtnState, 0, clrFrame) && bColorOK;
		bColorOK = GetColor(BTN_COLOR_Text,  m_eBtnState, 0, clrText) && bColorOK;
		ASSERT( bColorOK );
		
		if ( IsSeparator() )	// 仅仅是一个分割条
		{
			rcDraw.InflateRect(0, -rcDraw.Height()/6);
			rcDraw.NormalizeRect();
			CRect rcLeft(rcDraw), rcRight(rcDraw);
			rcLeft.right = rcLeft.left + rcDraw.Width()/2;
			rcRight.left = rcLeft.right;
			
			CColorStep step;
			step.InitColorRange(clrBK1, -50.0, -50.0, -50.0, true);
			clrBK1 = step.GetColor(1);
			step.InitColorRange(clrBK2, 50.0, 50.0, 50.0, true);
			clrBK2 = step.GetColor(1);
			dc.FillSolidRect(rcLeft, clrBK2);
			dc.FillSolidRect(rcRight, clrBK1);
			return;
		}
		
		bool32	bDrawFocus = true;
		bool32	bDrawSelected = false;
		
		DWORD dwTextDT = DT_SINGLELINE | DT_CENTER | DT_END_ELLIPSIS | DT_VCENTER;
		
		Image *pImage = m_pImageNomal;
		if ( Disable == m_eBtnState )
		{
			if ( m_pImageDisable )
			{
				pImage = m_pImageDisable;
			}
		}
		else if ( Selected == m_eBtnState )
		{
			bDrawSelected = true;
			if ( m_pImageSelected )
			{
				pImage = m_pImageSelected;
			}
		}
		else if ( Focus == m_eBtnState )
		{
			if ( m_pImageFocus )
			{
				pImage = m_pImageFocus;
			}
		}
		
		if ( Disable != m_eBtnState )
		{
			if ( (BTNB_RADIO == m_eBtnBehavior || BTNB_CHECKBOX == m_eBtnBehavior) && m_bCheck )
			{
				// check状态下，借用Select状态图片。颜色等
				pImage = m_pImageSelected;
				
				bColorOK = GetColor(BTN_COLOR_Background, Selected, BTN_Background1, clrBK1) && bColorOK;
				bColorOK = GetColor(BTN_COLOR_Background, Selected, BTN_Background2, clrBK2) && bColorOK;
				bColorOK = GetColor(BTN_COLOR_Frame, Selected, 0, clrFrame) && bColorOK;
				bColorOK = GetColor(BTN_COLOR_Text,  Selected, 0, clrText) && bColorOK;
				ASSERT( bColorOK );
				
				bDrawFocus = false;
				bDrawSelected = true;
			}
		}
		
		dc.SetTextColor(clrText);
		
		CRgn rgn;
		rgn.CreateRoundRectRgn(rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom, 3, 3);
		dc.SelectClipRgn(&rgn, RGN_AND);
		
		CBrush brush;
		brush.CreateSolidBrush(clrFrame);
		dc.FrameRgn(&rgn, &brush, 1, 1);
		
		rcDraw.InflateRect(-1, -1);
		rgn.DeleteObject();
		rgn.CreateRoundRectRgn(rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom, 3, 3);
		dc.SelectClipRgn(&rgn, RGN_AND);
		
		if ( bDrawSelected )
		{
			// 		rcDraw.InflateRect(-1, -1);
			// 		dc.Draw3dRect(rcDraw, RGB(127,127,127), RGB(255,255,255));
			// 		rcDraw.InflateRect(-1, -1);
			clrBK1 = clrBK2;
		}
		
		int32 iMid = max(rcDraw.Height()/3, 1);
		CColorStep step(clrBK2, clrBK1, iMid);
		step.SetColorSmooth(true);
		//dc.FillSolidRect(m_RectButton, clrBK); 
		TRIVERTEX tvx[2];
		GRADIENT_RECT grr = {0, 1};
		tvx[0].x	= rcDraw.left;
		tvx[0].y	= rcDraw.top;
		tvx[0].Red	= (GetRValue(clrBK2)<<8);
		tvx[0].Green	= (GetGValue(clrBK2)<<8);
		tvx[0].Blue	= (GetBValue(clrBK2)<<8);

		tvx[1].x	= rcDraw.right;
		tvx[1].y	= rcDraw.top + iMid;
		tvx[1].Red	= (GetRValue(clrBK1)<<8);
		tvx[1].Green	= (GetGValue(clrBK1)<<8);
		tvx[1].Blue	= (GetBValue(clrBK1)<<8);
		
		GradientFill(dc.GetSafeHdc(), tvx, 2, &grr, 1, GRADIENT_FILL_RECT_V);
		grr.LowerRight	= 0;
		grr.UpperLeft	= 1;
		tvx[0].x		= rcDraw.right;
		tvx[0].y		= rcDraw.bottom;
		tvx[1].x		= rcDraw.left;
		GradientFill(dc.GetSafeHdc(), tvx, 2, &grr, 1, GRADIENT_FILL_RECT_V);

// 		for ( int32 iTop=rcDraw.top; iTop <= rcDraw.bottom; iTop++ )
// 		{
// 			dc.FillSolidRect(rcDraw.left, iTop, rcDraw.Width(), 1, step.NextColor());
// 		}
		
		
		if ( pImage )
		{
			Graphics	g(dc.m_hDC);
			g.DrawImage(pImage, rcDraw.left, rcDraw.right, rcDraw.Width(), rcDraw.Height());
		}
		
		rcDraw.InflateRect(-1,-1);
		
		
		// 画focus标志 - 没有图片的情况下
		if ( !pImage && Focus == m_eBtnState && bDrawFocus )
		{
			COLORREF clrFocus = clrFrame;
			
			// 画个小框框 o
			CBrush brhFocus;
			brhFocus.CreateSolidBrush(clrFocus);
			//dc.FrameRgn(&rgn, &brhFocus, 1, 1);
		}
		
		if ( !m_StrText.IsEmpty() && BTNB_SEPARATOR != m_eBtnBehavior )
		{
			dc.SetBkMode(TRANSPARENT);
			// 为了美观和focus留出空白，把rcDraw左右缩小那么一点
			rcDraw.InflateRect(-2, 0);
			if ( m_bUseMarllet )
			{
				rcDraw.InflateRect(-2, -2);
				CFont MyFont;
				LOGFONT logfont;
				logfont.lfHeight = -rcDraw.Height();
				logfont.lfWidth = 0;
				logfont.lfEscapement = 0;
				logfont.lfOrientation = 0;
				logfont.lfWeight = FW_NORMAL;
				logfont.lfItalic = FALSE;
				logfont.lfUnderline = FALSE;
				logfont.lfStrikeOut = FALSE;
				logfont.lfCharSet = DEFAULT_CHARSET;
				logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
				logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
				logfont.lfQuality = DEFAULT_QUALITY;
				logfont.lfPitchAndFamily = DEFAULT_PITCH;
				
				_tcscpy(logfont.lfFaceName,_T("Marlett"));
				
				MyFont.CreateFontIndirect (&logfont);
				
				CFont* pOldFont = dc.SelectObject (&MyFont); 
				int OldMode = dc.SetBkMode(TRANSPARENT);  
				
				dc.DrawText (m_StrText,1,rcDraw,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
				dc.SetBkMode(OldMode);
				dc.SelectObject(pOldFont);
			}
			else
			{
				dc.DrawText(m_StrText, rcDraw, dwTextDT);
			}
		}
		
		dc.RestoreDC(iSave);
	}

	// 绘制方式2
	void	DrawStyle2(CDC &dc)
	{
		if ( m_RectButton.IsRectEmpty() )
		{
			return;
		}
		
		int iSave = dc.SaveDC();
		
		CRect rcDraw(m_RectButton);
		dc.SelectObject(&m_font);
		
		bool32 bColorOK = true;
		COLORREF clrBK1;
		COLORREF clrBK2;
		bColorOK = GetColor(BTN_COLOR_Background, m_eBtnState, BTN_Background1, clrBK1) && bColorOK;
		bColorOK = GetColor(BTN_COLOR_Background, m_eBtnState, BTN_Background2, clrBK2) && bColorOK;
		
		COLORREF clrText;
		COLORREF clrFrame;
		bColorOK = GetColor(BTN_COLOR_Frame, m_eBtnState, 0, clrFrame) && bColorOK;
		bColorOK = GetColor(BTN_COLOR_Text,  m_eBtnState, 0, clrText) && bColorOK;
		ASSERT( bColorOK );
		
		if ( IsSeparator() )	// 仅仅是一个分割条
		{
			rcDraw.InflateRect(0, -rcDraw.Height()/6);
			rcDraw.NormalizeRect();
			CRect rcLeft(rcDraw), rcRight(rcDraw);
			rcLeft.right = rcLeft.left + rcDraw.Width()/2;
			rcRight.left = rcLeft.right;
			
			CColorStep step;
			step.InitColorRange(clrBK1, -50.0, -50.0, -50.0, true);
			clrBK1 = step.GetColor(1);
			step.InitColorRange(clrBK2, 50.0, 50.0, 50.0, true);
			clrBK2 = step.GetColor(1);
			dc.FillSolidRect(rcLeft, clrBK2);
			dc.FillSolidRect(rcRight, clrBK1);
			return;
		}
		
		bool32	bDrawFocus = false;
		bool32	bDrawSelected = false;
		bool32  bGrayImage = false;
		bool32  bSelectedLikePush = false;
		
		DWORD dwTextDT = DT_SINGLELINE | DT_CENTER | DT_END_ELLIPSIS | DT_VCENTER;
		
		Image *pImage = m_pImageNomal;
		if ( Disable == m_eBtnState )
		{
			if ( m_pImageDisable )
			{
				pImage = m_pImageDisable;
			}
			else
			{
				bGrayImage = true;
			}
		}
		else if ( Selected == m_eBtnState )
		{
			bDrawSelected = true;
			if ( m_pImageSelected )
			{
				pImage = m_pImageSelected;
			}
			else
			{
				bSelectedLikePush = true;
			}
		}
		else if ( Focus == m_eBtnState )
		{
			if ( m_pImageFocus )
			{
				pImage = m_pImageFocus;
			}
			bDrawFocus = true;
		}
		
		if ( Disable != m_eBtnState )
		{
			if ( (BTNB_RADIO == m_eBtnBehavior || BTNB_CHECKBOX == m_eBtnBehavior) && m_bCheck )
			{
				// check状态下，借用Select状态图片。颜色等
				if ( m_pImageSelected != NULL )
				{
					pImage = m_pImageSelected;
				}
				else
				{
					bSelectedLikePush = true;
				}
				
				bColorOK = GetColor(BTN_COLOR_Background, Selected, BTN_Background1, clrBK1) && bColorOK;
				bColorOK = GetColor(BTN_COLOR_Background, Selected, BTN_Background2, clrBK2) && bColorOK;
				bColorOK = GetColor(BTN_COLOR_Frame, Selected, 0, clrFrame) && bColorOK;
				bColorOK = GetColor(BTN_COLOR_Text,  Selected, 0, clrText) && bColorOK;
				ASSERT( bColorOK );
				
				bDrawFocus = false;
				bDrawSelected = true;
			}
		}
		
		dc.SetTextColor(clrText);
		
		// 该画法矩形边框，背景采用1/2中间绘制，正常情况下不画边框，focus下绘制凸起边框，select绘制凹陷边框
		bool32 bInflateRectAfterFillBk = false;
		if ( bDrawFocus )
		{
			dc.DrawEdge(rcDraw, EDGE_RAISED, BF_RECT);
			rcDraw.InflateRect(-1, -1);
		}
		else if ( bDrawSelected )
		{
			dc.DrawEdge(rcDraw, EDGE_SUNKEN, BF_RECT);
			rcDraw.InflateRect(-1, -1);
		}
		else
		{
			bInflateRectAfterFillBk = true;
		}
		CRgn rgn;
		rgn.CreateRectRgnIndirect(rcDraw);
		dc.SelectClipRgn(&rgn, RGN_AND);
		
		int32 iMid = max(rcDraw.Height()/2, 1);
		CColorStep step(clrBK2, clrBK1, iMid);
		step.SetColorSmooth(true);
		//dc.FillSolidRect(m_RectButton, clrBK); 
		TRIVERTEX tvx[2];
		GRADIENT_RECT grr = {0, 1};
		tvx[0].x	= rcDraw.left;
		tvx[0].y	= rcDraw.top;
		tvx[0].Red	= GetRValue(clrBK2);
		tvx[0].Green	= GetGValue(clrBK2);
		tvx[0].Blue	= GetBValue(clrBK2);
		
		tvx[1].x	= rcDraw.right;
		tvx[1].y	= rcDraw.top + iMid;
		tvx[1].Red	= GetRValue(clrBK1);
		tvx[1].Green	= GetGValue(clrBK1);
		tvx[1].Blue	= GetBValue(clrBK1);
		
		GradientFill(dc.GetSafeHdc(), tvx, 2, &grr, 1, GRADIENT_FILL_RECT_V);
		grr.LowerRight	= 0;
		grr.UpperLeft	= 1;
		tvx[0].x		= rcDraw.right;
		tvx[0].y		= rcDraw.bottom;
		tvx[1].x		= rcDraw.left;
		GradientFill(dc.GetSafeHdc(), tvx, 2, &grr, 1, GRADIENT_FILL_RECT_V);

// 		for ( int32 iTop=rcDraw.top; iTop <= rcDraw.bottom; iTop++ )
// 		{
// 			dc.FillSolidRect(rcDraw.left, iTop, rcDraw.Width(), 1, step.NextColor());
// 		}
		if ( bInflateRectAfterFillBk )
		{
			rcDraw.InflateRect(-1, -1);
		}
		
		if ( pImage != NULL )
		{
			Graphics	g(dc.m_hDC);
			//g.DrawImage(pImage, rcDraw.left, rcDraw.right, rcDraw.Width(), rcDraw.Height());
			CSize sizeImage(0, 0);
			sizeImage.cx = pImage->GetWidth();
			sizeImage.cy = pImage->GetHeight();
			if ( rcDraw.Width() > sizeImage.cx )
			{
				rcDraw.left += (rcDraw.Width() - sizeImage.cx)/2;
				rcDraw.right = rcDraw.left + sizeImage.cx;
			}
			if ( rcDraw.Height() > sizeImage.cy )
			{
				rcDraw.top += (rcDraw.Height() - sizeImage.cy)/2;
				rcDraw.bottom = rcDraw.top + sizeImage.cy;
			}
			if ( bGrayImage )	// 图片灰度画
			{
				ImageAttributes  imageAttributes;
				UINT             width = pImage->GetWidth();
				UINT             height = pImage->GetHeight();
				Status			 staRet;
				
				ColorMatrix colorMatrix = {
					0.299f,	0.299f, 0.299f, 0.0f, 0.0f,
						0.587f,	0.587f, 0.587f,	0.0f, 0.0f,
						0.144f,	0.144f,	0.144f, 0.0f, 0.0f,
						0.0f,	0.0f,	0.0f,	1.0f, 0.0f,
						0.37f,	0.37f,	0.37f,	0.0f, 1.0f};
					
					COLORREF clrDisable = GetSysColor(COLOR_GRAYTEXT);
					ColorMap	clrMapText[100];
					for ( int iMapItem =0; iMapItem < 100 ; iMapItem++ )
					{
						clrMapText[iMapItem].newColor = Color(255, GetRValue(clrDisable), GetGValue(clrDisable), GetBValue(clrDisable));
						clrMapText[iMapItem].oldColor = Color(255, (BYTE)iMapItem, (BYTE)iMapItem, (BYTE)iMapItem);
					}
					
					// 			staRet = imageAttributes.SetRemapTable(100, clrMapText, ColorAdjustTypeBitmap);
					// 			ASSERT( staRet == Ok );
					staRet = imageAttributes.SetColorMatrix(	&colorMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);
					ASSERT( staRet == Ok );
					
					g.DrawImage(
						pImage, 
						Rect(rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height()),  // destination rectangle 
						0, 0,        // upper-left corner of source rectangle 
						width,       // width of source rectangle
						height,      // height of source rectangle
						UnitPixel,
						&imageAttributes);
			}
			else
			{
				CRect rcImage(rcDraw);
				if ( bSelectedLikePush )
				{
					rcImage.OffsetRect(1, 1);
				}
				DrawImage(g, pImage, rcImage, 1, 0, true);
				// 			UINT             width = pImage->GetWidth();
				// 			UINT             height = pImage->GetHeight();
				// 			g.DrawImage(
				// 				pImage, 
				// 				Rect(rcImage.left, rcImage.top, width, height),  // destination rectangle 
				// 				0, 0,        // upper-left corner of source rectangle 
				// 				width,       // width of source rectangle
				// 				height,      // height of source rectangle
				// 				UnitPixel
				// 					);
			}
		}
		else
		{
			rcDraw.InflateRect(-1,-1);
			
			if ( !m_StrText.IsEmpty() && BTNB_SEPARATOR != m_eBtnBehavior )
			{
				dc.SetBkMode(TRANSPARENT);
				// 为了美观和focus留出空白，把rcDraw左右缩小那么一点
				rcDraw.InflateRect(-2, 0);
				if ( m_bUseMarllet )
				{
					rcDraw.InflateRect(-2, -2);
					CFont MyFont;
					LOGFONT logfont;
					logfont.lfHeight = -rcDraw.Height();
					logfont.lfWidth = 0;
					logfont.lfEscapement = 0;
					logfont.lfOrientation = 0;
					logfont.lfWeight = FW_NORMAL;
					logfont.lfItalic = FALSE;
					logfont.lfUnderline = FALSE;
					logfont.lfStrikeOut = FALSE;
					logfont.lfCharSet = DEFAULT_CHARSET;
					logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
					logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
					logfont.lfQuality = DEFAULT_QUALITY;
					logfont.lfPitchAndFamily = DEFAULT_PITCH;
					
					_tcscpy(logfont.lfFaceName,_T("Marlett"));
					
					MyFont.CreateFontIndirect (&logfont);
					
					CFont* pOldFont = dc.SelectObject (&MyFont); 
					int OldMode = dc.SetBkMode(TRANSPARENT);  
					
					dc.DrawText (m_StrText,1,rcDraw,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
					dc.SetBkMode(OldMode);
					dc.SelectObject(pOldFont);
				}
				else
				{
					dc.DrawText(m_StrText, rcDraw, dwTextDT);
				}
			}
		}
		
		dc.RestoreDC(iSave);
	}

	RectButtonParent					&m_wndTB;		// 父窗口

	CWnd					*m_pWndOwner;
	uint16					m_nID;			// ushort 16 位
	CString					m_StrText;
	CString					m_StrTip;
	Image					*m_pImageNomal;
	Image					*m_pImageSelected;
	Image					*m_pImageFocus;
	Image					*m_pImageDisable;

	CRect					m_RectButton;		// 该button被TBWnd分配的矩形(TBWnd的Client坐标)
	E_BtnDrawState				m_eBtnState;

	E_BtnBehavior			m_eBtnBehavior;		// 是普通按钮还是check/radio?
	RectRadioBtnGroup		*m_pRadioGroup;		// 如果是radio按钮，其所属组 - 可能为NULL
	bool32					m_bCheck;

	CFont					m_font;
	bool32					m_bUseMarllet;		// 使用特殊字体marllet绘制一些系统图标

	int						m_iGroupId;			// 隶属的group

	E_DrawStyle				m_eDrawStyle;

	E_ClickLBtnDrawStyle			m_eClickStyle;
	bool32					m_bInLButtonUp;

	TBWndColorItemMap		m_mapColorItems;
};


//////////////////////////////////////////////////////////////////////////
// radio分组 - 简单实现，group必须要比里面的btn生存周期长
template<class RectButtonParent>
class CRectRadioBtnGroup{
public:
	typedef CRectButton<RectButtonParent> RectButton;
	
	void	RemoveAllBtns(){ m_mapRadioBtns.RemoveAll(); }
	int32	GetBtnsCount()const { return m_mapRadioBtns.GetCount(); }
	void	ResetCheckState()		// 将所有按钮的check状态取消
	{
		POSITION pos = m_mapRadioBtns.GetStartPosition();
		while ( pos != NULL )
		{
			void *pKey;
			WORD v;
			m_mapRadioBtns.GetNextAssoc(pos, pKey, v);
			if ( NULL != pKey )
			{
				((RectButton *)pKey)->SetCheck(false);
			}
		}
	}

protected:
	friend RectButton;
	typedef	CMapPtrToWord	TBBtnMap;

	void	AddRadioBtn(RectButton *pBtn)
	{
		WORD v;
		if ( !m_mapRadioBtns.Lookup(pBtn, v) )
		{
			m_mapRadioBtns[pBtn] = 0;		// 
		}
	}
	void	RemoveRadioBtn(RectButton *pBtn)
	{
		WORD v;
		if ( m_mapRadioBtns.Lookup(pBtn, v) )
		{
			m_mapRadioBtns.RemoveKey(pBtn);		// 
		}
	}
	void	IamCheckBtn(RectButton *pBtn)	// 这个button一定要已经加入这个组，其它button被设置为false
	{
		WORD v;
		if ( m_mapRadioBtns.Lookup(pBtn, v) )
		{
			POSITION pos = m_mapRadioBtns.GetStartPosition();
			while ( pos )
			{
				WORD v;
				void *pKey = NULL;
				m_mapRadioBtns.GetNextAssoc(pos, ((void *)pKey), v);
				if ( pKey && pKey!=pBtn )
				{
					((RectButton *)pKey)->SetCheck(false);
				}
			}
		}
		else
		{
			ASSERT( 0 );
		}
	}

	TBBtnMap	m_mapRadioBtns;		// 保证这一组中只有一个radiobutton被选中，可以存在都没选中的情况 - TBWnd要协调好这些指针的问题
};

// RectButton的伪父窗口
template<class RectButtonParentWndType>
class CRectButtonWndParent
{
public:
	typedef CRectButton<RectButtonParentWndType> RectButton;

	// 构造
	CRectButtonWndParent() { m_pRcBtnCaptured = NULL; }

	// 获取其DC并且释放
	CDC		*GetDrawNudeDC() { return GetDC(); };
	void	ReleaseDrawNudeDC(CDC *pDC) { ReleaseDC(pDC); };
	
	// 控件id
	//int		GetDlgCtrlID();
	//HWND		GetSafeHwnd();
	
    // Rect转换
	BOOL	ScreenToRectBtnPt(CPoint *ppt){ ScreenToClient(ppt); return TRUE; };		// 屏幕坐标到按钮坐标的转换 !!!绘制时并不做坐标转换，假设按钮坐标与dc坐标一致	
	
	// focus
	// 当前是否有按钮获取了focus
	RectButton *GetCaptureBtn()	{ return m_pRcBtnCaptured; }					
	// 按钮申请mouse capture
	void	RequestMouseCapture(RectButton &btn)
	{
		m_pRcBtnCaptured = &btn;
		::SetCapture( btn.GetBtnParent().GetSafeHwnd() );
	}
	// 按钮申请释放先前申请的mouse capture
	void	ReleaseMouseCapture(RectButton &btn)
	{
		if ( &btn == m_pRcBtnCaptured )
		{
			if ( ::GetCapture()==btn.GetBtnParent().GetSafeHwnd() )
			{
				ReleaseCapture();
			}
			m_pRcBtnCaptured = NULL;
		}
	}

protected:
	RectButton		*m_pRcBtnCaptured;
};

#endif // _RECT_BUTTON_H_