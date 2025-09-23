#if !defined(_DLG_F10_H)
#define _DLG_F10_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgf10.h : header file


#include "sharestructnews.h"
#include "AutoRichEditCtrl.h"
#include "DialogEx.h"
#include "ViewDataListener.h"

class CViewData;
class CIoViewBase;
class CMerch;
class CMmiNewsRespF10;


// F10 的节点
typedef struct T_NewsNode
{
	T_NewsNode()
	{
		Clear();
	}

	bool32 operator!= (const T_NewsNode& stNewsNode)
	{
		if ( m_iInfoIndex != stNewsNode.m_iInfoIndex 
		  || m_eDataType != stNewsNode.m_eDataType
		  || m_eInfoType != stNewsNode.m_eInfoType
		  || m_eChildDataType != stNewsNode.m_eChildDataType
		  || m_StrContent != stNewsNode.m_StrContent
		  || m_StrTime != stNewsNode.m_StrTime
		  || m_StrCrc32 != stNewsNode.m_StrCrc32
		  || m_RectShow != stNewsNode.m_RectShow 
		   )
		{
			return true;
		}

		return false;
	}

	void Clear()
	{
		m_iInfoIndex	= -1;
		m_eDataType		= EDTCount;
		m_eInfoType		= EITCount;
		m_eChildDataType= EDTCount;
		m_StrContent	= L"";
		m_StrTime		= L"";	
		m_StrCrc32		= L"";
		m_RectShow		= CRect(-1, -1, -1, -1);
	}

	int32			m_iInfoIndex;
	E_DataType		m_eDataType;
	E_InfoType		m_eInfoType;
	E_DataType		m_eChildDataType;
	CString			m_StrContent;
	CString			m_StrTime;
	CString			m_StrCrc32;
	CRect			m_RectShow;

}T_NewsNode;

// 特殊字段
typedef struct T_SpecialNode
{
	int32			m_iInfoIndex;
	int32			m_iBeginPos;
	int32			m_iTotalCount;
	int32			m_iMarketId;
	CString			m_StrCode;
}T_SpecialNode;

/////////////////////////////////////////////////////////////////////////////
// CDlgF10 dialog
class CDlgF10 : public CDialogEx ,public CViewNewsListner
{
enum E_ReqState
{
	ERSReqing = 0,
	ERSReqFinished,

	ERSCount,
};

public:
	CDlgF10::CDlgF10(CIoViewBase* pIoViewActive, CMerch* pMerch, CWnd* pParent /* = NULL */);

public:
	// 服务器连接中断
	void		OnNewsServerDisConnected(int32 iCommunicationId);

	// 服务器重连上
	void		OnNewsServerConnected(int32 iCommunicationId);

	// 收到 F10 数据
	void		OnNewsRespF10(IN const CMmiNewsRespF10* pMmiNewsRespF10);		

	// 获取商品
	CIoViewBase* GetIoView() { return m_pIoViewActive; }

	// 商品切换
	void		OnMerchChange(IN CMerch* pMerch, bool32 bForceReq = false);

	// 选中项目变化
	void		OnSelectedItemChange(int32 iIndexNow);

	// 调整大小, 使之最大化
	void		AdjustToMaxSize();

private:
	
	// 更新显示
	void		UpdateShow();
	
	// 得到相邻的两个商品
	void		GetSblingMerch(OUT CMerch*& pMerchPre, OUT CMerch*& pMerchAft);

	// 请求 F10 数据
	bool32		RequestF10(CMerch* pMerch, int32 iIndexInfo);
	
	// 解析 F10 字符串 fangz0612 这个函数以后放到公共部分. 其他的请求也是类似的
	bool32		ParaseF10String(IN TCHAR* pStrF10, OUT CString& StrErrMsg);
 
	// 是否是合法的特殊字段名
	bool32		BeValidSpecialName(TCHAR* pStrSpecialName);
	
	// 是否是合法的普通字段名
	bool32		BeValidNormalName(TCHAR* pStrSpecialName);
	
	// 根据索引设置特殊节点的字段值
	void		SetSpecialNodeValue(int iIndex, const CString& StrValue, T_SpecialNode& stSpecialNode);
	
	// 根据索引设置普通节点的字段值
	void		SetNewsNodeValue(int iIndex, const CString& StrValue, T_NewsNode& stNewsNode);

	// 转换
	int32		CStringToInt(TCHAR* pStrNum);
	int32		CStringToInt(const CString& StrNum);

	void		OnDataRespNewsData(int iMmiReqId, IN const CMmiNewsBase *pMmiNewResp, CMerch *pMerch);
	
private:
	
	// ViewData
	CAbsCenterManager*							 m_pAbsCenterManager;
	
	// 当前商品
	CMerch*								 m_pMerch;
	
	// 当前激活视图
	CIoViewBase*						 m_pIoViewActive;

	// 请求状态(保证是线性的请求, 一个请求没回来之前不发下一个请求, 否则会导致 m_iInfoIndexNow 混乱)
	E_ReqState							m_eReqState;

	// 当前选中的子节点
	int32								 m_iInfoIndexNow;

	// 更多
	CRect								 m_RectMore;
	
	// 前一个商品
	CRect								 m_RectMerchPre;

	// 后一个商品
	CRect								 m_RectMerchAft;

	// 是否显示了更多
	bool32								 m_bNeedMore;

	// 是否全屏
	bool32								 m_bMaxed;

	// 是否掉线
	bool32								 m_bServerDisConnect;

	// 在显示更多的时候, 菜单中显示的第一个项目
	T_NewsNode							 m_stNewsNodeFistAsMore;

	// 特殊字段
	T_SpecialNode						 m_stSpecialNode;

	// 当前显示的节点
	T_NewsNode							 m_stNewsNodeNow;
	
	// 最新提示, 公司概况, 财务分析 等标题节点
	CArray<T_NewsNode, T_NewsNode&>		 m_aNewTitles;
	
	// 特殊字段列头对应的索引
	CMap<int32, int32, CString, LPCTSTR> m_aMapTableSpecial;

	// F10 节点列头对应的索引
	CMap<int32, int32, CString, LPCTSTR> m_aMapTableNewsNode;

// Construction
public:
   // CDlgF10(CWnd* pParent = NULL);   // standard constructor
   ~CDlgF10();
   
// Dialog Data
	//{{AFX_DATA(CDlgF10)
	enum { IDD = IDD_DIALOG_F10 };
	CAutoRichEditCtrl m_RichEdit;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgF10)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();	
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgF10)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnF10Menu (UINT nID);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMsgHotKey(WPARAM wParam, LPARAM lParam);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DLG_F10_H)
