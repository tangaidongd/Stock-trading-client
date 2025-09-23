#if !defined(AFX_NENTBWND_H__3E5ECEAD_317A_4CA3_88F3_E29F460329DC__INCLUDED_)
#define AFX_NEWTBWND_H__3E5ECEAD_317A_4CA3_88F3_E29F460329DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TBWnd.h : header file
//
#include <map>
using namespace std;

#include "NCButton.h"
#include "ToolBarIDDef.h"
#include "ConfigInfo.h"
#include "DlgPullDown.h"
#include "WndCef.h"
#include "NewTBChildWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CNewTBWnd window



class CNewTBWnd : public CWnd
{
// Construction
public:

	CNewTBWnd(CWnd &wndParent);
	virtual ~CNewTBWnd();
public:

	// 设置当前显示的页面
	void ShowPage(int32 iID);				// 传入按钮的ID

	// 显示和者隐藏IM窗体
	void OnShowIM();
	void OnHideIM();

	// 改变IM窗体底部大小
	void SetImBottom(int iBottom);
	// 移动IM窗体
	void MoveIm(int iLeftOffset, int iTopOffset, int iRightOffset, int iBottomOffset);

	bool32	InitialCurrentWSP();			// 初始化当前工作区显示信息 - 该函数的行为建立在已经调用了InitialAnalyToolBarBtns的基础上
	bool32	GetCurrentWSP(CString &strWsp);	// 获取当前工作区

	void	SetF7(bool32 bShow);	// F7

	// 所有市场菜单, 根据菜单ID 取市场/板块ID
	int32		GetMenuMarketID(int32 iMenuID);

	// 加载系统默认的几个cfm，主要是需要切换下商品
	static bool32		LoadSystemDefaultCfm(DWORD iIoViewCmd, bool32 bCloseExistence = false);
	static bool32		LoadSystemDefaultCfm(const CString &StrSDCName, bool32 bCloseExistence = false);

	static bool32	    DoOpenCfm(const CString &StrCfmXmlName, CString StrUrl = L"");
	static bool32		DoCloseCfm(const CString &StrCfmXmlName);

	void		ShowAllMarketMenu(CRect rect);
	void		KLineCycleChange(int iKlineType);
	
	CDlgPullDown* GetPullDown();
	//	右键消息处理
	bool RbuttonProcess(int iSrcID);

	BOOL OnToolbarEvent(buttonContainerType &mapBtn, int32 iID, WPARAM wParam, LPARAM lParam);

	void SetDefaultCfmStatus();

private:
    void InitToolbarButtons();
    void InitFont();	
    void DrawToolbar(CDC& dc);
	void AddToolbarButton(const T_ColumnData &colData, int& iXStartPos, int &iYStartPos, const int iBtnTotalHeight, E_FixedType eFixedType=EFT_NONE);
    void AddNcButton(LPRECT lpRect, const T_ButtonData&btnData, bool bCategory = false, UINT nCount=3, E_FixedType eFixedType=EFT_NONE);
	// 鼠标是否在按钮上  
    void ShowMenu(T_ButtonData btnData, int id);
    bool FindBtnData(int id, int& iBtnDataIndex, int& iChlIndex, int& iColumnIndex, bool& bChildBtn,bool &bCol);
    bool FindSubBtnData(int id, int& iBtnDataIndex, int& iChlIndex,int&iSubChlIndex , int& iColumnIndex, bool& bChildBtn,bool &bCol);
	BOOL OnBtnEvent(CString strEventType, CString strEventParam, WPARAM wParam, LPARAM lParam, CString strBtnName = L"", int iWidth = 500, int iHeight = 300);
	BOOL OnSpecialFunc(CString strEventParam, WPARAM wParam, LPARAM lParam);
    BOOL OnIndexChangeFunc(CString strEventParam, WPARAM wParam, LPARAM lParam);
	BOOL OnCustomFunc(CString strEventParam, WPARAM wParam, LPARAM lParam);
	bool IsAllNumbers(CString str);
	//	实盘交易。摩西交易右键处理
	bool RbuttonTradeProcess(int iSrcID);
	//	显示文件对话框	
	bool ShowFileDlg(const CString& strOpenFilePath, CString& selectPath);
	//	实盘交易"提醒可右键菜单按钮更改应用程序路径"
	void ShowTradeFirstNotify();
	//	查找容器中的相关id按钮迭代器
	#define  FIND_BTN_ITERATOR(container, id) std::find_if(container.begin(), container.end(),vector_finder(id))

	//	查找容器中的相关id的按钮类
	inline CNCButton&	GetCNCButton(buttonContainerType& container, int iId)
	{
		buttonContainerType::iterator it = container.begin();
		FOR_EACH(container, it)
		{
			if ((*it).GetControlId() == iId)
			{
				return *it;
			}
		}
		//理论容器中应该存在
		//ASSERT(0);
		return CNCButton();
	}
	
	CDlgPullDown* CreateSubMenu(T_ChildButtonData &btnData, int left, int top);		// 更多按钮的二级菜单
	CDlgPullDown* AdjustSubMenuPos(CString strParent, int left, int top);	// 调整二级菜单的显示位置

	CString ChooseToLiveUrl();

	std::string GetRandomString(int iCharacterCount );	// 产生随机字符串
	
private:

	//	以下3中按钮集合类，不支持自动排序，支持无序的id排序
    buttonContainerType m_mapBtn;							//	工具栏按钮集
	buttonContainerType m_mapLeftFixedBtn;					//  工具栏左侧固定项按钮集
	buttonContainerType m_mapRightFixedBtn;					//  工具栏右侧固定项按钮集
    buttonContainerType m_mapPullDown;						//	下拉菜单的按钮集
	buttonContainerType m_mapMorePullDown;					//	二级下拉菜单的按钮集
	buttonContainerType m_mapControlBtn;					//	控制按钮集合
	bool32		        m_bShowIntersect;					//	是否显示工具栏按钮与控制按钮交集的按钮	
    CNCButton::T_NcFont m_fontSimpleBtn;			        //  普通按钮的字体
    CNCButton::T_NcFont m_fontCategoryBtn;		            //  分类按钮的字体

	bool32					m_bPaint;						//	第一次刷新时候加载数据信息
	CPoint					m_ptToolButtonLeftInset;		//	工具栏与最左侧的起始边距
	int						m_iLeftFixedWidth;				//  工具栏左边固定按钮集width
	int                     m_iRightFixedWidth;				//  工具栏右边固定按钮集width
	int                     m_iRightFixedPos;				//  左边固定项按钮最右边位置
	int                     m_iLeftFixedPos;				//  右边固定项按钮最左边位置
private:
	// 外挂交易
	void		DoTradePlugin(int resId, bool32 isReselect);
	// 获取交易程序路径，没有设置需要用户选择设置
	bool32		GetTradeExePath(OUT CString &StrExePath,  IN LPCTSTR pSec, IN  LPCTSTR pKey, bool32 bReselect);
	//  设置交易程序路径，NULL或者0长度为删除该配置
	void		SetTradeExePath(LPCTSTR pSec, LPCTSTR pKey,LPCTSTR lpszExePath);	
	//	监视toolbar.xml中按钮数据，将时间类型为外挂实盘交易数据的id存入map中，指定临时路劲数据为空字符串	
	void		AddRealTrade(const T_ColumnData &data);
	void		GetTradeSection(int iTradeId, OUT CString& tSec);
public:
	static	DWORD	GetIoViewCmdIdByCfmName(const CString &StrCfmName);

    int         GetPublicButtonLastPos();

	void		UpdateFixedStatus(CPoint& point);
	int         TFixedButtonHitTest(CPoint point, bool32 &bLeft);
	void        SetFixBtnCheckStatus(int iButton);

	void		SetChildCheckStatusByName(CString strCfmName);
public:

	int32		m_indexNews; // 当前选择的何种资讯,0金平，1贵金属资讯，2财经日历
private:

	// 当前是哪一页
	int32		m_iCurrentPage;

	//
	CWnd		&m_wndParent;
	int			            m_iXButtonHovering;			// 标识鼠标进入按钮区域
	T_FixedBtnHovering      m_stFixedBtnHovering;       // 标识鼠标进入固定按钮区域

	vector<CDlgPullDown*> m_vecDlgSubPullDown;
	CDlgPullDown *m_pDlgPullDown;
	map<int32,int32>			  m_mapMenuID;	// 菜单id 对应的市场id
	map<int32, CString>				m_mapRealTrad; //外挂实盘数据

	// 保存IM按键的位置
	CRect m_RectIm;
	bool  m_blShowIM;

	COLORREF m_clrTextNor;                  // 按钮文本的颜色
	COLORREF m_clrTextHot;
	COLORREF m_clrTextPress;

	COLORREF m_crlThirdTextNor;             // 工具栏第三行按钮文本的颜色
	COLORREF m_crlThirdTextHot;
	COLORREF m_crlThirdTextPress;

	bool32	 m_bMember;                     // 会员，交易所

	CNewTBChildWnd		   *m_pChildWnd;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewTBWnd)
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL


protected:
	//{{AFX_MSG(CNewTBWnd)
	afx_msg	int	 OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg	BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg	LRESULT	OnIdleUpdateCmdUI(WPARAM w, LPARAM l);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWTBWND_H__3E5ECEAD_317A_4CA3_88F3_E29F460329DC__INCLUDED_)
