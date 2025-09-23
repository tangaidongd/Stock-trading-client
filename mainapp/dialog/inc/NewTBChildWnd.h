#if !defined(AFX_NEWTBCHILDWND__INCLUDED_)
#define AFX_NEWTBCHILDWND__INCLUDED_

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

/////////////////////////////////////////////////////////////////////////////
// CNewTBChildWnd window



class CNewTBChildWnd : public CWnd
{
// Construction
public:

	CNewTBChildWnd(CWnd *pWndParent, buttonContainerType &mapBtn);
	virtual ~CNewTBChildWnd();
public:

	// 设置当前显示的页面
	void    ShowPage(int32 iID);			// 传入按钮的ID
	bool32  InitialCurrentWSP();

	// 改变分组图标状态
	void ChangeGroupStatus(buttonContainerType &mapBtn, int iID);

	// 恢复按钮状态
	void RecoverToolbarButtons();

	void SetBtnCheckStatus(bool bCheck);

	void RefreshControlBtn();

	void SetCheckStatusByName(CString strCfmName);
	
private:
	
    void DrawToolbar(CDC& dc);
	// 鼠标是否在按钮上
    int  TButtonHitRect(CPoint point);  

	//////////////////////////////////////////////////////////////////////////以下控制工具栏上按钮移动处理
	//	获取左侧窗体边界区域交交集按钮，右侧与控制按钮区域交集按钮
	int32	GetIntersectButton(bool bLeft);
	//判断矩形是否属于目的矩形的子集	
	bool32 IsSubRect(const CRect& destRect, const CRect& subRect);
	//	bLeftDirect为true工具栏整体右移，反之工具栏整体左移
	void MoveToolButtons(bool32 bLeftDirect);
	//	显示及根据父窗体大小自动移动控制按钮
	void ShowControlButton(ECButtonType type);
	//	呈现工具栏最右侧的控制按钮形式
	ECButtonType DecideControlButtonType();	
	//	窗体拉伸情况下，重新排列工具栏按钮
	void  AlignToolButton();
	//	获取右侧控制区域，以右侧最左边的控制按钮直到窗体边界区域，可能区域存在为空
	CRect GetControlRect(bool32 bLeft=true);

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
	
private:

	Image*			    m_pLeftImage;
	Image*			    m_pRightImage;
	Image*              m_pImgPermission;


	//	以下3中按钮集合类，不支持自动排序，支持无序的id排序
    buttonContainerType &m_mapBtn;							//	工具栏按钮集
	buttonContainerType m_mapControlBtn;					//	控制按钮集合
	bool32		        m_bShowIntersect;					//	是否显示工具栏按钮与控制按钮交集的按钮	

private:

	// 当前是哪一页
	int32		            m_iCurrentPage;

	//
	CWnd		           *m_pWndParent;
	int			            m_iXButtonHovering;			// 标识鼠标进入按钮区域

	int                     m_iMoveSpace;				// 左右移动距离
	bool32                  m_bLeftDirect;				// 是否向左移动

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewTBChildWnd)
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL


protected:
	//{{AFX_MSG(CNewTBChildWnd)
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

#endif // !defined(AFX_NEWTBCHILDWND__INCLUDED_)
