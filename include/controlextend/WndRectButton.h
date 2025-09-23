#ifndef _WNDRECTBUTTON_H_
#define _WNDRECTBUTTON_H_

#pragma warning(push)
#pragma warning(disable:4786)
#include "typedef.h"
#include "dllexport.h"
#include "GdiPlusTS.h"
#include <map>
using std::map;
#include <vector>
using std::vector;

class CWndRectRadioBtnGroup;
class CRectButtonWndInterface;
class CWndRectButton;

//////////////////////////////////////////////////////////////////////////
// 视图接口
class CONTROL_EXPORT CRectButtonWndInterface
{
public:
	
	virtual	CWnd	&GetWnd() = 0;

	virtual	void	OnNewBtn(CWndRectButton &btn) = 0;	

	virtual bool	RequestMouseCapture(CWndRectButton &btn) = 0;
	virtual	CWndRectButton *GetCaptureBtn() = 0;
	virtual bool	ReleaseMouseCapture(CWndRectButton &btn) = 0;
};

// 基础视图实现
class CONTROL_EXPORT CRectButtonWndSimpleImpl : public CWnd, public CRectButtonWndInterface
{
	DECLARE_DYNAMIC(CRectButtonWndSimpleImpl);
public:
	CRectButtonWndSimpleImpl();
	~CRectButtonWndSimpleImpl();

public:
	// 需要实现的接口
	typedef vector<CWndRectButton *>	WndRectButtonVector;
	virtual	void	GetAllBtns(WndRectButtonVector &aBtns) = 0;
	virtual void	OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler){};	// 在idle时更新btn显示等
	virtual void	OnDraw(CDC &dc);			// 绘制时，继承对象调用此接口绘制按钮
	virtual	void	RecalcLayout(bool32 bNeedPaint = true) = 0;			// 分配各个按钮，子控件size

public:
	// CRectButtonWndInterface
	virtual	CWnd	&GetWnd(){ return *this; };
	virtual	void	OnNewBtn(CWndRectButton &btn){};	
	virtual bool	RequestMouseCapture(CWndRectButton &btn);
	virtual	CWndRectButton *GetCaptureBtn();
	virtual bool	ReleaseMouseCapture(CWndRectButton &btn);

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual int  OnToolHitTest(CPoint point, TOOLINFO* pTI) const;

private:
	void			SetLastFocusBtn(CWndRectButton *pBtn);	// 可能为NULL
	CWndRectButton  *GetLastFocusBtn();

protected:
	CWndRectButton		*m_pBtnCurCapture;		// 聚焦的按钮，不能中途无效~
	CWndRectButton		*m_pBtnLastFocus;		// 鼠标移动的按钮, 不能中途无效~

protected:
	DECLARE_MESSAGE_MAP();

	// Generated message map functions
	//{{AFX_MSG(CRectButtonWndSimpleImpl)
	afx_msg		int		  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg		void	  OnSize(UINT nType, int cx, int cy);
	afx_msg	    void	  OnPaint();
	afx_msg		void	  OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg		void	  OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg     void	  OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg		void		OnMouseMove(UINT nFlags, CPoint point);
	afx_msg		LRESULT		OnMouseLeave(WPARAM w, LPARAM l);
	afx_msg		LRESULT		OnMouseHover(WPARAM w, LPARAM l);
	afx_msg		BOOL		OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg		void		OnCancelMode();
	afx_msg     void		OnCaptureChanged(CWnd* pWnd);
	afx_msg		LRESULT		OnIdleUpdateCmdUI(WPARAM w, LPARAM l);
	//}}AFX_MSG
};


//////////////////////////////////////////////////////////////////////////
// 按钮相关

// 通知
typedef struct tagRECTBTN_NOTIFY_ITEM
{
	NMHDR	hdr;
	uint16	uBtnId;			// 具体触发的btn
	CWndRectButton *pBtn;		

	LPARAM	lParam;			// 附带数据
}RECTBTN_NOTIFY_ITEM;

// 颜色
class CONTROL_EXPORT CWndRectButtonColorValue
{
public:
	CWndRectButtonColorValue(void  *pLParam = NULL, LPCTSTR lpszDesc=NULL, COLORREF clr=RGB(0,0,0))
	{
		m_pParam = pLParam;
		m_StrDesc = lpszDesc;
		m_clr	  = clr;
	}
	
	bool operator==(const CWndRectButtonColorValue &v){
		return v.m_clr == m_clr && m_StrDesc == m_StrDesc;
	}
	void			*m_pParam;
	CString			m_StrDesc;
	COLORREF		m_clr;
protected:
private:
};
typedef map<int, CWndRectButtonColorValue >		WndRectButtonColorItemMap;


//////////////////////////////////////////////////////////////////////////
// 按钮
class CONTROL_EXPORT CWndRectButton
{
public:
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
		Marlett_MAX,						//  49 Max
		Marlett_RESTORE,					//  50 Restore
		Marlett_CHECKMARK=98,				//  98 Checkmark 
		Marlett_Bullet = 105,				// 105 Bullet
		Marlett_Close = 114,				// 114 Close
		Marlett_Help,						// ?
		Marlett_UpTriangle,					// 方向向上的三角
		Marlett_DownTriangle,				// 向下的三角
		Marlett_End = 'z'					// 'z'为最后一个(不包含)
	};
	CWndRectButton(CRectButtonWndInterface &wndTB);
	// hWndOwner - 接收命令窗口, nID - 该button特有的ID, pszText该button的显示文字
	// pszTip - 提示文字
	CWndRectButton(CRectButtonWndInterface &wndTB, CWnd *pWndOwner, uint16 nID, LPCTSTR pszText, LPCTSTR pszTip = NULL); // 自己用文字绘制
	CWndRectButton(CRectButtonWndInterface &wndTB, CWnd *pWndOwner, uint16 nID, E_MarlettChar chMarllet, LPCTSTR pszTip = NULL); // 自己用文字绘制 - 使用marllet字体绘制字符ch
	// 贴图绘制 - 正常、选择(压下)、鼠标移动、禁止
	CWndRectButton(CRectButtonWndInterface &wndTB, CWnd *pWndOwner, uint16 nID, Image *pImageNomal, Image *pImageSelected, Image *pImageFocus, Image *pImageDisable, LPCTSTR pszTip = NULL);
	// 贴图绘制 + 文字，文字浮于上方 - 正常、选择(压下)、鼠标移动、禁止
	CWndRectButton(CRectButtonWndInterface &wndTB, CWnd *pWndOwner, uint16 nID, LPCTSTR pszText, Image *pImageNomal, Image *pImageSelected, Image *pImageFocus, Image *pImageDisable, LPCTSTR pszTip = NULL);

	virtual ~CWndRectButton();

	void		SetBtnBehavior(E_BtnBehavior eNewBehavior, CWndRectRadioBtnGroup *pGroup = NULL); // 改变button的行为！！！尽量在初始化的时候调用 pGroup - Radio需要一个组来协调其它radio的check状态
	void		SetCheck(bool32 bCheck);				// 只有 checkbox / radio 能够设置
	bool32		GetCheck() const;
	void		SetUseMarlettChar(bool32 bUse = false);			// 是否使用MarlettChar字体来进行标题图标的绘制 - lpszText会被当成MarlettChar

	E_BtnBehavior	GetBtnBehavior()const { return m_eBtnBehavior; };
	bool32		IsSeparator()const { return BTNB_SEPARATOR == m_eBtnBehavior;  };

	void		SetBtnText(LPCTSTR pcszText){ m_StrText = pcszText; };		// 目前只提供这些属性操作
	void		SetBtnTextWithDraw(LPCTSTR lpszText);
	void		SetBtnTip(LPCTSTR pcszTip){ m_StrTip = pcszTip; };
	void		SetBtnGroupId(int iGroupId) { m_iGroupId = iGroupId; };
	void		SetDrawStyle(E_DrawStyle eStyle) { m_eDrawStyle = eStyle; } 
	void		SetClickStyle(E_ClickLBtnDrawStyle eStyle) { m_eClickStyle = eStyle; }
	void		SetBtnImages(Image *pImageNomal, Image *pImageSelected = NULL, Image *pImageFocus = NULL, Image *pImageDisable = NULL);

	CString		GetBtnText() const { return m_StrText; };
	CString		GetBtnTip() const { return m_StrTip; };
	uint16		GetBtnId() const { return m_nID; };
	int			GetBtnGroupId() const  { return m_iGroupId; };
	E_DrawStyle	GetDrawStyle()const { return m_eDrawStyle; }
	E_ClickLBtnDrawStyle GetClickStyle() const { return m_eClickStyle; }
	void		GetBtnImages(Image **ppImageNomal, Image **ppImageSelected = NULL, Image **ppImageFocus = NULL, Image **ppImageDisable = NULL) const;

	WndRectButtonColorItemMap &GetBtnColorsRef() { return m_mapColorItems ; };

	// 目前并真的是click，而是LButtonDown
	virtual     bool32	DoClicked(CPoint ptClick);		// 用户点击了该按钮 - 发送通知给hWndOwner，返回hWndOwner处理返回，默认返回true，则将btnState设置为select
	
	virtual		CSize   GetBtnTextExtend(CDC *pDC);		// 根据文字获取可能需要的大小
	virtual		void	Draw(CDC &dc);						// 根据state绘制Button，包含边框绘制
	void	DrawNude();

	void	SetFont(CFont &font);

	bool32	GetColor(E_BtnColorType eType, E_BtnDrawState eState, int32 iIndex, COLORREF &clr);		// 根据当前状态获取特定类型的颜色
	void	SetColor(E_BtnColorType eType, E_BtnDrawState eState, int32 iIndex, COLORREF clr, LPCTSTR lpszDesc=NULL, bool32 bNeedDraw = false);	// 如果是第一次的话，需要输入描述字串
	void	SetColor(E_BtnColorType eType, int32 iIndex, COLORREF clr, LPCTSTR lpszDesc=NULL, bool32 bNeedDraw=false);	// 将4个状态的都设置同样颜色
	int		CombineBtnColorIndex(E_BtnColorType eType, E_BtnDrawState eState, int32 iIndex);	// 返回按照规则编制的索引

	
	bool32	IsEnable() const;								// 是否可接收消息 - 如不能接收则TBWnd不会发送鼠标消息给这个按钮
	void	EnableBtn(bool32 bEnable = true);
	// 鼠标消息，click由按钮自己产生 - 还真不如自己创建windows button到上面
	virtual		void	OnMouseMove(UINT nFlag, CPoint point);		// 如果TBWnd收到mouseleave，最后一个聚焦按钮会收到point(-1,-1)的mousemove
	virtual		void	OnLButtonDown(UINT nFlag, CPoint point);	// 此时需要capturemouse - 先不实现capture，只设置一下显示效果，up的时候就直接click
	virtual     void	OnLButtonUp(UINT nFlag, CPoint point);		// 此时需要请求TBWnd依情况releasemouse
	virtual		void	OnRButtonUp(UINT nFlag, CPoint point);		// 简单点，只接收鼠标右键弹出，貌似压下不是很必要capture

	CSize	MeasureBtnRect(CDC *pDC, OUT bool32 &bFixed);			// 获取该Rect希望获取的大小, bFixed暗示是否应当固定大小
	void	GetBtnRect(OUT CRect &RectBtn);
	void	MoveBtn(const CRect &RectBtn, bool32 bDraw = true);			// 现在recalcLayout等外部还是调用这个函数来分配矩形吧，避免填色的时候死循环

	void	SetBtnState(E_BtnDrawState eState, bool32 bForce = false);		// 设置按钮新的状态
	
	void	SetBtnColors(const WndRectButtonColorItemMap &mapColor, bool32 bDrawIfChange = true);			// 设置按钮配色
	void	GetBtnColors(OUT WndRectButtonColorItemMap &mapColor) const ;

protected:
	friend class CWndRectRadioBtnGroup;

	// button发送消息格式 SendMessage(hWndOwner, nMessageNotifyId, LPARAM)
	virtual		bool32	NotifyOwner(UINT nNotifyCode = NM_CLICK, LPARAM lParam = NULL);						// 利用wndTB消息通知格式，通知 hWndOwner click
	
	void	Initial(CWnd *pWndOwner, uint16 nID, LPCTSTR pszText, Image *pImageNomal, Image *pImageSelected, Image *pImageFocus, Image *pImageDisable, LPCTSTR pszTip);

	void	DrawStyle1(CDC &dc);
	void	DrawStyle2(CDC &dc);
	
	//static  void GetDrawTextInRect(CDC &dc, DWORD dwStyle, const CRect &rc, const CString &StrText,OUT CString StrCanDraw);

	CRectButtonWndInterface					&m_wndTB;		// 父TB窗口

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
	CWndRectRadioBtnGroup		*m_pRadioGroup;		// 如果是radio按钮，其所属组 - 可能为NULL
	bool32					m_bCheck;

	CFont					m_font;
	bool32					m_bUseMarllet;		// 使用特殊字体marllet绘制一些系统图标

	int						m_iGroupId;			// 隶属的group

	E_DrawStyle				m_eDrawStyle;

	E_ClickLBtnDrawStyle			m_eClickStyle;
	bool32					m_bInLButtonUp;

	WndRectButtonColorItemMap		m_mapColorItems;
};


//////////////////////////////////////////////////////////////////////////
// radio分组 - 简单实现，group必须要比里面的btn生存周期长
class CONTROL_EXPORT CWndRectRadioBtnGroup{
public:
	
	void	RemoveAllBtns();
	int32	GetBtnsCount()const;
	void	ResetCheckState();		// 将所有按钮的check状态取消

protected:
	friend class CWndRectButton;
	typedef	CMapPtrToWord	TBBtnMap;

	void	AddRadioBtn(CWndRectButton *pBtn);
	void	RemoveRadioBtn(CWndRectButton *pBtn);
	void	IamCheckBtn(CWndRectButton *pBtn);	// 这个button一定要已经加入这个组，其它button被设置为false

	TBBtnMap	m_mapRadioBtns;		// 保证这一组中只有一个radiobutton被选中，可以存在都没选中的情况 - TBWnd要协调好这些指针的问题
};

//////////////////////////////////////////////////////////////////////////
// CWndRectButton 派生类别
// CWndRectMenuButton - 点击右边↓时，发送消息给wndTB，wndTB弹出选择菜单
class CONTROL_EXPORT CWndRectMenuButton : public CWndRectButton
{
public:
	enum E_MenuBtnEvent{
		MENU_BTN_LeftPartClick = 0,		// 右侧被点击了, LPARAM是点击坐标
		MENU_BTN_RightPartClick,				// 按钮左侧被点击了 LPARAM是点击坐标
	};
	CWndRectMenuButton(CRectButtonWndInterface &wndTB, CWnd *pWndOwner, uint16 nID, LPCTSTR pszText, LPCTSTR pszTip = NULL, bool32 bLeftIsBtn = false);  // 文字绘制 nNotifyID为通知wndTB的消息
	
	virtual	bool32 DoClicked( CPoint ptClick );					// 如果左边为按钮行为，则按照普通按钮发送，只有点击↓才弹出菜单，根据选择的内容
	virtual	void   OnMouseMove(UINT nFlag, CPoint ptMove );		// 鼠标只有移动到右边↓位置才有反应
	virtual void   OnLButtonDown( UINT nFlag, CPoint point );	// 如果点击右边三角，则直接发送菜单消息给wnd，左边的响应依m_bEnableLeftButton而定行为
	virtual void   OnLButtonUp( UINT nFlag, CPoint point );		// 右三角不接收up
	virtual	void   Draw( CDC &dc );

	virtual CSize  GetBtnTextExtend(CDC *pDC);
	
	void	GetRightRect(CRect &rc);
	
private:
	uint32	m_nRightRectWidth;		// 右边↓宽度
	bool32	m_bEnableLeftButton;	// 左侧是否为按钮还是文字，false - 文字(默认)，是按钮则向wnd发送按钮事件
	CPoint	m_ptLastClick;
	E_MenuBtnEvent		m_eClick;
};

#endif //!_WNDRECTBUTTON_H_