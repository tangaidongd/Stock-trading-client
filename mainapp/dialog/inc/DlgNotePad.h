#if !defined(AFX_DLGNOTEPAD_H_)
#define AFX_DLGNOTEPAD_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgnotepad.h : header file
//

#include "DialogEx.h"
#include "GmtTime.h"

#include "UserNoteInfo.h"
#include "webbrowser2.h"

#include "LandMineStruct.h"
#include "ViewDataListener.h"

#include <map>
using std::multimap;
#include <memory>
using std::auto_ptr;

/////////////////////////////////////////////////////////////////////////////
// CDlgNotePad dialog

// 将来估计要处理信息地雷的显示
// 处理投资日记的显示
// 类似tdx

#define UM_NOTE_MERCHHASCHANGED		(WM_USER+0x123)
#define UM_NOTE_LANDTYPEHASCHANGED	(WM_USER+0x124)		// 信息地雷的性质改变了



// 信息地雷的标题
class CLandMineTitle;

// 信息地雷的数据结构, 同一天可能多个地雷, 目前时间的精确度只到天, 所以同天的地雷时间是完全相同的.
typedef mapLandMine  mapLandMinePseudo;
typedef CArray<CLandMineTitle, const CLandMineTitle &> LandMinePseudoArray;	// 按照原始数据，赋值过来的

class CIoViewBase;
class CDlgNotePad : public CDialogEx , public CViewNewsListner
{
public:
	enum E_ShowType
	{
		EST_InfoMine = 0, // 信息地雷
		EST_UserNote,	  // 投资日记
	};

	// 目前不能显示信息地雷，信息地雷的数据可能需要一个单独的部件来完成数据的采集
	static void	ShowNotePad(E_ShowType eShowType, CGmtTime *pTimeInit = NULL, CIoViewBase *pEventIoView=NULL);	// 显示信息地雷或者投资日记

	static void OnViewDataInfoMineList(CMerch *pMerch);		// 列表与内容都是通过这个接口的

	static void RequestViewDataInfoMineList(CMerch *pMerch, void *);	// 假设请求是这样发送的 列表
	static void RequestViewDataInfoMineContent(CMerch *pMerch, const CLandMineTitle &);	// 内容

	// 信息地雷分为列表请求与内容请求，列表请求在K线视图中已经请求好了，但是这里商品切换的话还是需要请求列表信息
	// 具体用户想看哪个信息则请求具体内容

	virtual void OnDataRespNewsData(int iMmiReqId, IN const CMmiNewsBase *pMmiNewResp, CMerch *pMerch);

private:
	
	// Construction
protected:
	CDlgNotePad(CWnd* pParent = NULL);   // standard constructor

	void	InitShowType(E_ShowType eShowType, CGmtTime *pTimeInit = NULL, CIoViewBase *pEventIoView=NULL);

	void	ShowHtml(T_UserNoteInfo &note, bool32 bTryReadContent = false);	// 调用浏览器显示内容 - 是否加入标题的显示
	void	ShowHtml(const CLandMineTitle &title, bool32 bTryReq = false);	// 显示信息地雷
	void	ShowList();						// 显示列表状态
	void	SwitchShow();					// 切换显示浏览器或者列表

	void	RecalcLayout();

	bool32	IsInListMode()const;;

	CString	MakeListItemDesc(const T_UserNoteInfo &note);
	CString MakeListItemDesc(const CLandMineTitle &mineTitle);
	CString GetCurDlgTitle();

	bool32	UpdateUserNote(const T_UserNoteInfo &note); // 更新指定id的笔记显示
	bool32  UpdateInfoMine(const CLandMineTitle &title);	// 更新指定的地雷, 应该不会更新列表信息

	int		GetListSelUserNote(OUT T_UserNoteInfo &note);
	int		GetListSelInfoMine(OUT CLandMineTitle &title);

	void	WatchMerchIsChanged(bool32 bNotify = true);			// 观察商品是否有变更

	void	UpdateBtnStatus();

	void	RefreshList();

	// 投资日记相关的
	void	OnNewNote();
	void	OnModifyNote();
	void	OnDelNote();
	void	OnClearNote();

	// 信息地雷
	void	FilterInfoMind();	// 过滤不合适显示的信息地雷

	void	DoFloating();

	void	SetMyTitle(LPCTSTR lpszTitle);
	
	CSize			m_sizeMin;			// 不能太小
	CMerchKey		m_MerchCur;			// 当前商品，会在初始化时向mainframe找寻激活视图，获取关注商品，空则不处理
	CMerch			*m_pMerchCur;
	CRect			m_RectSeparator;	// 分割条位置

	CArray<int, int>		m_aBtnIds;

	E_ShowType				m_eShowType;
	CUserNoteInfoManager::UserNotesArray	m_aUserNotes;

	mapLandMinePseudo		m_mapInfoMines;		// 当前列表所有的信息地雷
	LandMinePseudoArray		m_aInfoMines;		// 当前显示的信息地雷，第一个是最新的
	bool32					m_bHistoryMine;	// 历史信息地雷，false为实时地雷~~
	CGmtTime				m_TimeInit;		// 实时信息地雷需要一个时间指示

	//CWebBrowser2	m_WebBrowser2;		// 浏览器，将内容生成html文档，并显示

	CRichEditCtrl	m_RichEdit;			// 编辑控件

	CFont			m_fontEdit;

	static	auto_ptr<CDlgNotePad> s_pDlg;
	
	// Dialog Data
	//{{AFX_DATA(CDlgNotePad)
	enum { IDD = IDD_DIALOG_NOTEPAD };
	CListBox	m_List;
	CStatic		m_StaticTitle;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNotePad)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgNotePad)
	afx_msg void OnDblclkList();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg		LRESULT		OnIdleUpdateCmdUI(WPARAM w, LPARAM l);
	afx_msg void OnMerchChanged();
	afx_msg void OnBtnSwitch();
	afx_msg void OnBtnNew();
	afx_msg void OnBtnModify();
	afx_msg void OnBtnDel();
	afx_msg void OnBtnClear();
	afx_msg void OnBtnFloat();
	afx_msg void OnBtnHide();
	afx_msg void OnLandTypeChanged();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMsgTestViewDataListResp();				// 假定现在地雷列表返回了
	afx_msg void OnMsgTestViewDataContentResp();			// 假定现在地雷内容返回了
	afx_msg LRESULT OnNcHitTest(CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNOTEPAD_H_)