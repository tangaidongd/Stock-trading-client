#if !defined(AFX_LEFTTOOLBAR_H__7665BA12_D9E5_41E6_8784_6FDBA19269D7__INCLUDED_)
#define AFX_LEFTTOOLBAR_H__7665BA12_D9E5_41E6_8784_6FDBA19269D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LeftToolBar.h : header file
//
#include "NCButton.h"
#include "ConfigInfo.h"
#include "XTipWnd.h"
#include "LeftBarItemManager.h"
#include "IoViewChooseStock.h"

/////////////////////////////////////////////////////////////////////////////
// CLeftToolBar window

class CLeftToolBar : public CToolBar, public CChooseStockStateNotify
{
// Construction
public:
	CLeftToolBar();
	virtual ~CLeftToolBar();

public:

	enum E_CfmType
	{
		TB_WSP_BTN_START,

		TB_IOCMD_REPORT,
		TB_IOCMD_KLINE,
		TB_IOCMD_TREND,
		TB_IOCMD_NEWS,
		//TB_IOCMD_F10,
		TB_IOCMD_REPORTARB,
		TB_IOCMD_KLINEARB,
		TB_IOCMD_TRENDARB,
	};

	enum E_SwitchType
	{
		EST_NONE,
		EST_BTN,		// 切换按钮
		EST_WSP,		// 切换版面
		EST_COUNT,
	};
//////////////////////////////////
/// from	CChooseStockStateNotify

	void            OnChooseStockStateResp();

/////////////////////////////////
    virtual BOOL    OnCommand(WPARAM wParam, LPARAM lParam);
    BOOL            OnStockCNCommand(WPARAM wParam, LPARAM lParam);
	BOOL            OnStockHKCommand(WPARAM wParam, LPARAM lParam);
	BOOL            OnFuturesCommand(WPARAM wParam, LPARAM lParam);
	BOOL            OnGlobalIndexCommand(WPARAM wParam, LPARAM lParam);
	BOOL            OnQuoteForeignCommand(WPARAM wParam, LPARAM lParam);
	BOOL            OnSmartSelStockCommand(WPARAM wParam, LPARAM lParam);
	BOOL            OnStockDecisionCommand(WPARAM wParam, LPARAM lParam);

    bool32	        DoOpenCfm(const CString &StrCfmXmlName);
    bool32		    DoCloseCfm(const CString &StrCfmXmlName);
	void            SetPageType(E_LeftBarType eType);

	CMerch*         GetMarketFirstMerch();
	
	void            DrawToolButton();				         // 绘制工具栏上的按钮
	void            ReLayout();					             // 重新布局按钮


	// 在工具栏上添加按钮
	void            AddToolButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL, LPCTSTR lpszDecription = NULL);
	int             TButtonHitTest(CPoint point);			// 获取当前鼠标所在的按钮
	int             GetBtnIDByName(CString StrName);
	void		    ChangeButtonStateByCfmName(const CString &StrWspName, const E_LeftBarType &eType);
	void            ShowTips();
	void            InitialButtonHoveringParam();	// 初始化标识鼠标进入按钮区域

	// 加载系统默认的几个cfm，主要是需要切换下商品
	bool32		    LoadSystemDefaultCfm(DWORD iIoViewCmd, bool32 bCloseExistence = false);
	bool32		    LoadSystemDefaultCfm(const CString &StrSDCName, bool32 bCloseExistence = false); 
	
	// 显示选中按钮
	void            ShowSelectedBtn(int32 iID);
public:
    static	DWORD	GetIoViewCmdIdByCfmName(const CString &StrCfmName);
	void            SetCheckStatus(int iID);
	void            InitialToolBarBtns(E_LeftBarType eType);      //初始化按钮
	bool32			IsCurrWspMarket(int32 &iMarketId, CString StrWspName=L"");
	bool32			IsBtnInSameType(int32 iBtnId1, int32 iBtnId2);
	bool32			IsWspInSameType(int32 iBtnId1, int32 iBtnId2);
	void            ChangeLeftBarStatus(CString StrWspName = _T(""), const E_SwitchType &eType = EST_WSP);					// 显示或者隐藏左侧栏，点亮对应按钮
	void			SetSwitchType(E_SwitchType eType);
	E_ReportType    GetMerchKind(IN CMerch *pMerch);
	void            ShowDuoGuTongLie();
private:
	/////////////////////////////////////////////////////
	//===================CODE CLEAN======================
	//////////////////////////////////////////////////////
	//void			GetSmartSelStockData(OUT vector<T_ButtonItem> &vecItem);//	获取智能选股显示

// Operations

public:
	void           SetMarkedId(int32 iMarketID);             // 切换市场ID
	void           ChangeMarketId();
	bool32         ChangeLeftBar(const CString &StrCfmName);	     // 根据版面文件切换左侧栏类型
	void           ChangeDuoGuTongLieMerch(CMerch *pMerch);


public:
	// 智能选股状态改变
	void ChangePickModelStatus();

	// Attributes
private:
	std::map<int, CNCButton> m_mapBtn;	 // 侧边栏按钮集合


	COLORREF m_clrTextNor;			     // 按钮文本的颜色
	COLORREF m_clrTextHot;
	COLORREF m_clrTextPress;

	CNCButton::T_NcFont	m_fontFirst;     //工具栏第一二行字体

	Image	*m_pImgBk;				     // 工具栏背景图片
	Image   *m_pImgBtnNormal;			 // 正常状态下按钮图片
	Image   *m_pImgBtnNew;               // 选出股票是按钮图片

	// 假定背景图片与侧边栏按钮图片，以及向上向下按钮图片的宽度相同
	int32	m_iNumShowBtn;				// 显示侧边按钮的个数
	int32   m_iBtnHeight;               // 单个侧边栏按钮的高度
	int32   m_iBtnWidth;				// 单个侧边栏按钮的宽度
	int32   m_iToolBarWidth;            // 侧边栏背景图的宽度
	int32	m_indexFirstShow;			// 显示起始按钮位置
	int32	m_indexLastShow;			// 显示结束按钮位
	int32   m_indexSelShow;             // 当前选中按钮的位置

	CXTipWnd		m_TipWnd;			// 按钮提示文字
	CPoint			m_PointLast;

	CNCButton		m_ncBtnName;		// 顶部标签名称


private:
	E_LeftBarType m_eLeftBarType;
	E_SwitchType  m_eSwitchType;
	std::map<E_LeftBarType, int32> m_mapButtonHovering;
	bool32        m_bSelUpDownBtn;					 
public:
	int32		  m_iMarketId;						 // 保存市场Id

	// Generated message map functions
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	//{{AFX_MSG(CLeftToolBar)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEFTTOOLBAR_H__7665BA12_D9E5_41E6_8784_6FDBA19269D7__INCLUDED_)
