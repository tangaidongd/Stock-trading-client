#ifndef _IO_VIEW_MANAGER_H_
#define _IO_VIEW_MANAGER_H_

#include "GuiTabWnd.h"
#include "IoViewChart.h"
#include "IoViewPrice.h"
#include "ControlBase.h"
#include "tinyxml.h"
#include "AdjustViewLayoutDlg.h"

#define MAXGROUPID 65536

class CIoViewBase;
class CAdjustViewLayoutDlg;
//
enum E_IoViewGroupType
{
	E_IVGTNone = 0,
	E_IVGTChart,
	E_IVGTReport,
//	E_IVGTRelative,
	E_IVGTPrice,
	E_IVGTOther,
	E_IVGTCount,
};

// 视图抢占兄弟视图显示的模式
enum E_IoViewBeyondShow////如果这里修改记得修改facescheme.cpp 对应的结构
{
	EIBSNone = 0,		// 安分守己
	EIBSVertical,		// 通天入地
	EIBSHorizontal,		// 横行霸道
	EIBSBoth,			// 唯我独尊

	EIBSCount,
};

// 视图 是其他视图商品切换依据 属性(类似报价表, 大单排行等等表格类的视图)
enum E_IoViewMerchChangeSrc //如果这里修改记得修改facescheme.cpp 对应的结构
{
	EIMCSFalse = 0,		// 普通视图
	EIMCSTrue,			// 报价表类视图

	EIMCSCount,
};

// 业务视图对照结构 如果这里修改记得修改facescheme.cpp 对应的结构
typedef struct T_IoViewObject
{
	UINT					m_uID;								// 类型ID
	E_IoViewBeyondShow		m_eBeyondShow;						// 是否强行占领同一切分窗口兄弟的区域进行显示
	E_IoViewMerchChangeSrc	m_eMerchChangeSrc;					// 是否是其他视图商品切换源
	CString					m_StrLongName;						// 长名	
 	CString					m_StrShortName;						// 短名
 	CString					m_StrTipMsg;						// Tips
	CString					m_StrXmlName;						// Xml 名
	E_IoViewGroupType		m_eGroupType;						// 版面调整中的视图类型

	CRuntimeClass*			m_pIoViewClass;
	bool32					m_bAllowDragAway;						// 是否允许被拖拉到其他窗口

	DWORD					m_dwAttendMerchType;				// 该业务视图类型所适合处理的商品类型，默认全部

 	T_IoViewObject(const UINT& uID,
				   const E_IoViewBeyondShow& eBeyondShow,
				   const E_IoViewMerchChangeSrc& eMerchChangeSrc,
				   const CString &StrLongName,
				   const CString &StrShortName,
				   const CString &StrTipMsg,
				   const CString &StrXmlName,
				   const E_IoViewGroupType& eGroupType,				   
				   CRuntimeClass *pIoViewClass,
				   bool32	bAllowDragAway,
				   DWORD	dwAttendMerchType = 0xffffffff)
 	{
		m_uID				 = uID;
		m_eBeyondShow		 = eBeyondShow;
		m_eMerchChangeSrc	 = eMerchChangeSrc;
 		m_StrLongName		 = StrLongName;
 		m_StrShortName		 = StrShortName;
 		m_StrTipMsg			 = StrTipMsg;
 		m_StrXmlName		 = StrXmlName;
		m_eGroupType		 = eGroupType;
 		m_pIoViewClass		 = pIoViewClass;
		m_bAllowDragAway	 = bAllowDragAway;
		m_dwAttendMerchType  = dwAttendMerchType;
	};

}T_IoViewObject;

// 业务视图集对照结构

typedef struct T_IoViewGroupObject
{
	CString				m_StrLongName;
	CString				m_StrShortName;
	E_IoViewGroupType	m_eIoViewGroupType;

	T_IoViewGroupObject(const CString& StrLongName,const CString& StrShortName,const E_IoViewGroupType& eIoViewGroupType)
	{
		m_StrLongName	   = StrLongName;
		m_StrShortName	   = StrShortName;
		m_eIoViewGroupType = eIoViewGroupType;
	}

}T_IoViewGroupObject;



// 标签类型,
typedef enum E_ManagerTopbarType
{
	MANAGERBAR_CHANGE_MARKET = 1,			// 切换市场
	MANAGERBAR_OPEN_CFM,					// 打开版面
	MANAGERBARCount,

}E_ManagerTopbarType;


// 按钮对齐方式
typedef enum E_ManagerAlignmentType
{
	ALIGNMENT_LEFT = 1,				// 左对齐
	ALIGNMENT_RIGHT,				// 右对齐
	ALIGNMENT_CENTER,				// 居中对齐
	ALIGNMENTCount,

}E_ManagerAlignmentType;


// 每个顶部标签栏信息
typedef struct T_ManagerTopBar
{
	T_ManagerTopBar()
	{
		m_iBtnType			= MANAGERBARCount;
		m_iAlignmentType    = ALIGNMENT_LEFT;
		m_StrShowName		= L"";
		m_strMerchCode		= L"";
		m_iBtnID			= -1;
		m_iMarketID			= -1;
		m_strCfmName		= L"";
		m_iShrinkSize		= 0;
		m_bSelected			= false;
		m_colNoarml			= RGB(0,0,0);
		m_colPress			= RGB(0,0,0);
		m_colHot			= RGB(0,0,0);
		m_colTextNoarml		= RGB(0,0,0);
		m_colTextPress		= RGB(0,0,0);
		m_colTextHot		= RGB(0,0,0);
		m_colTextframe		= RGB(0,0,0);
	}

	//
	CString			m_StrShowName;		// 显示名称
	int32			m_iBtnID;			// 按钮的ID
	int32			m_iBtnType;			// 数据类型
	int32			m_iMarketID;		// 市场ID
	CString         m_strMerchCode;		// 商品代码
	CString			m_strCfmName;		// 版面名称
	int32			m_iAlignmentType;	// 排列方式
	int32			m_iShrinkSize;		// 内部缩小多少像素
	bool			m_bSelected;		// 是否默认选中
	
	COLORREF		m_colNoarml;
	COLORREF		m_colPress;
	COLORREF		m_colHot;
	COLORREF		m_colTextNoarml;
	COLORREF		m_colTextPress;
	COLORREF		m_colTextHot;
	COLORREF		m_colTextframe;
}T_ManagerTopBar;



// 每个顶部标签栏信息
class CManagerTopBarInfo
{
public:
	CManagerTopBarInfo()
	{
		m_iBarHight			= 40;
		m_iTopBarBtnWidth	= 80;
		m_iTopBarType       = 0;
		m_pImageBk			= NULL;
		m_strImagePath		= _T("");
		m_colBarBKColor		= RGB(42,42,50);
		m_mapTopbarGroup.clear();
		m_mapTopbarBtnLeft.clear();
		m_mapTopbarBtnCenter.clear();
		m_mapTopbarBtnRight.clear();
	}
	~CManagerTopBarInfo()
	{
		DEL(m_pImageBk)
	}

	int32						m_iBarHight	;				// 控件的高度
	COLORREF					m_colBarBKColor;			// 控件的背景色
	int32						m_iTopBarBtnWidth;			// 每个按钮的宽度
	Image*						m_pImageBk;					// 按钮背景图片			// 暂时就右侧使用
	CString						m_strImagePath;
	int32                       m_iTopBarType;				// 标签栏类型 1-已上市新股 2-未上市新股 0-其他类型
	map<int32, T_ManagerTopBar>	m_mapTopbarGroup;			// 顶部标签页信息汇总
	map<int32, CNCButton>		m_mapTopbarBtnLeft;			// 左侧顶部标签按钮
	map<int32, CNCButton>		m_mapTopbarBtnCenter;		// 中部顶部标签按钮
	map<int32, CNCButton>		m_mapTopbarBtnRight;		// 右边顶部标签按钮
};



//

class CIoViewManager : public CControlBase, public CGuiTabWndCB
{
	DECLARE_DYNAMIC(CIoViewManager)
public:
	enum E_ViewTab
	{
		EVTNone = 0,
		EVTLeft,
		EVTRight,
		EVTTop,
		EVTBottom
	};
	static const int32 KiChartBorderTop				= 0;
	static const int32 KiChartBorderLeft			= 0;
	static const int32 KiChartBorderRight			= 0;
	static const int32 KiChartBorderBottom			= 0;
// Construction
public:
	CIoViewManager();
	virtual ~CIoViewManager();

// from CControlBase
	virtual BOOL		TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL		TestChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL		TestKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
//
	void				OnTabSelChange();

public:
	bool32				BeTestValid();
	CString				ToXml();	
	bool32				FromXml(TiXmlElement *pElement);
	CString				ToXmlForWorkSpace(const char * KpcFileName);
	static const char * GetXmlElementValue();
	static CString		GetDefaultXML(int32 iGroupID,bool32 bBlank, CIoViewBase *pIoView = NULL);
	static const T_IoViewObject* GetIoViewObject();
	CIoViewBase*		GetActiveIoView();
	void				OnIoViewManagerActive();
	void				OnIoViewManagerDeactive();

	CIoViewBase*		FindIoViewByPicMenu(UINT nID, bool32 bCreateWhenNotExist=false);	// 查找manager中对应id的ioview，如果允许，在不存在的情况下创建一个

	int32				GetGroupID();
	void				SetGroupID(int32 iID);
	void				OnViewGroupIdChanged(int32 iNewGroupId);	// 每一个实现~~，只有强制了
	void				ChangeGroupId(int32 iNewId, bool32 bChangeMerch = false);	// 避免与以前的功能冲突, 改变视图分组并切换商品
	
	void				OnRButtonDown2(CPoint pt, int32 iTab);
	CString				GetViewTabString(E_ViewTab eViewTab);
	E_ViewTab			GetViewTab(const char *StrViewTab);	
	void				SetTabAllign(E_ViewTab eViewTab);

	void				DealTabKey();
	void				DealF5Key();

	void				OnSizeChange(const CRect& rect);

	void				RegisterActiveIoViewToMainFrame(bool32 bRegister);			// 通知mainframe现在激活/注销的ioview是哪个，方便TBWnd调整ioView显示

	static CString						GetIoViewString(CIoViewBase *pIoView);
	static CString						GetIoViewString(UINT uID);
	static E_IoViewGroupType			GetIoViewGroupType(const CIoViewBase* pIoView);
	static CString						GetIoViewGroupName(const CIoViewBase* pIoView);
	static const T_IoViewObject*		FindIoViewObjectByXmlName(const CString &StrXmlName);
	static const T_IoViewObject*		FindIoViewObjectByIoViewPtr(const CIoViewBase *pIoView);
	static const T_IoViewObject*		FindIoViewObjectByRuntimeClass(CRuntimeClass * pRunTimeClass);
	static const T_IoViewObject*		FindIoViewObjectByPicMenuID(UINT nMenuID);
	static const T_IoViewObject*		GetIoViewObject(int32 iIndex);
	static const int32					GetIoViewObjectCount();
	static void							GetIoViewIDs(IN E_IoViewGroupType eGroupType,OUT CArray<int32,int32>& aID);

	static const T_IoViewGroupObject*	FindIoViewGroupObjectByLongName(const CString& StrLongName);
	static const T_IoViewGroupObject*	GetIoViewGroupObject(int32 iIndex);
	static const int32					GetIoViewGroupObjectCount();
	void				DrawNoChartRgn();
public:
	// 取标志
	bool32			   GetBeyondShowFlag() const { return m_bBeyondShow; }

	// 强制显示
	void			   ForceBeyondShowIoView(CIoViewBase* pIoView = NULL);

	//	强制选择顶部商品按钮
	void				ForceSelectManagerTop(const CMerch* pMerch);
private:
	void			   BeyondShowIoView(bool bBeyond, CIoViewBase* pIoView, bool32 bForceShow = false);

	// 读取顶部标签页相关函数
	bool32	FromXmlTopBtn( TiXmlElement *pTiXmlElement );
	void	InitTopBtn();
	void	ManagerTopBarSizeChange(const CRect& rect);		// 更新顶部按钮坐标
	void	DrawManagerTopBar(CPaintDC &dc);
	void	SetCheckStatus( int iID );
	int		TButtonHitTest(CPoint point);
	void	RestTopBtnStatus(int32 ibtnControlId);			// 去掉右侧按钮的点击状态

public:
	void			   RemoveAllIoView();
	bool			   GetManagerTopbarStatus() { return m_cManagerTopBarInfo.m_mapTopbarGroup.size() > 0;} 		// 获取顶部工具栏显示的状态

public:
	const CManagerTopBarInfo		&GetManagerTopBarInfo(){ return m_cManagerTopBarInfo; }

public:
	CArray<CIoViewBase*, CIoViewBase*> m_IoViewsPtr;

public:
//protected:
//
	E_ViewTab		m_eViewTab;
	bool32			m_bAutoHide;					// 当仅有一个标签页时， 自动隐藏
	CGuiTabWnd		m_GuiTabWnd;
	CRect			m_rcGuiTabWnd;

private:
	static CRuntimeClass	*m_pDefaultIoViewClass;	// 新建窗口是默认显示的业务视图

private:
	int32			m_iGroupId;						// 以IoViewManager为单位分配分组编号， 所有下面的业务视图统一为该编号， <=0表示独立视图

	bool32			m_bFromXmlNeedInit;				// 从XML中初始化，等待初始化

	bool32			m_bIsShowManagerTopbar;			// 是否显示顶部工具栏
	bool32			m_bIsHasTopBarByLayout;			// 板块是否有顶部标签项
	bool32			m_bIsHasBottomBarByLayout;		// 板块是否有底部标签项

	
	CManagerTopBarInfo			m_cManagerTopBarInfo;						// 顶部按钮信息
	CRect						m_rcManagerTopBtn;							// 顶部按钮区域
	int32						m_LastPressTopBtnId;							// 顶部最后点击的按钮，不包括右侧按钮

	
	static CIoViewManager*	m_pThis;

private:
	bool32			m_bBeyondShow;				// 特殊视图抢占兄弟视图显示

	// ...fangz0111 奇怪的问题, 没空搞了, 先补丁一下
	bool32			m_bDelIoView;

	bool32			m_bDrawCorner;		// 是否画边角

	bool32          m_bNeedFixWidth;    // Tab是否固定每一项的宽度
	int32           m_iTabFixWidth;     // Tab固定每一项的宽度
	COLORREF	m_clrTabBk;				// tab条背景颜色
	COLORREF	m_clrTabBkNor;			// tab正常背景颜色
	COLORREF	m_clrTabBkSel;			// tab选中背景颜色
	COLORREF	m_clrTabTextNor;		// tab正常文字颜色
	COLORREF	m_clrTabTextSel;		// tab选中文字颜色
	COLORREF	m_clrTabSpliter;		// tab与视图间分割条颜色

///////////////////////////////////////////////
	//////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewManager)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewManager)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nID);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnIoViewTitleButtonLButtonDown(WPARAM wParam,LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_MANAGER_H_
