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

// ��������Ҫ������Ϣ���׵���ʾ
// ����Ͷ���ռǵ���ʾ
// ����tdx

#define UM_NOTE_MERCHHASCHANGED		(WM_USER+0x123)
#define UM_NOTE_LANDTYPEHASCHANGED	(WM_USER+0x124)		// ��Ϣ���׵����ʸı���



// ��Ϣ���׵ı���
class CLandMineTitle;

// ��Ϣ���׵����ݽṹ, ͬһ����ܶ������, Ŀǰʱ��ľ�ȷ��ֻ����, ����ͬ��ĵ���ʱ������ȫ��ͬ��.
typedef mapLandMine  mapLandMinePseudo;
typedef CArray<CLandMineTitle, const CLandMineTitle &> LandMinePseudoArray;	// ����ԭʼ���ݣ���ֵ������

class CIoViewBase;
class CDlgNotePad : public CDialogEx , public CViewNewsListner
{
public:
	enum E_ShowType
	{
		EST_InfoMine = 0, // ��Ϣ����
		EST_UserNote,	  // Ͷ���ռ�
	};

	// Ŀǰ������ʾ��Ϣ���ף���Ϣ���׵����ݿ�����Ҫһ�������Ĳ�����������ݵĲɼ�
	static void	ShowNotePad(E_ShowType eShowType, CGmtTime *pTimeInit = NULL, CIoViewBase *pEventIoView=NULL);	// ��ʾ��Ϣ���׻���Ͷ���ռ�

	static void OnViewDataInfoMineList(CMerch *pMerch);		// �б������ݶ���ͨ������ӿڵ�

	static void RequestViewDataInfoMineList(CMerch *pMerch, void *);	// �����������������͵� �б�
	static void RequestViewDataInfoMineContent(CMerch *pMerch, const CLandMineTitle &);	// ����

	// ��Ϣ���׷�Ϊ�б����������������б�������K����ͼ���Ѿ�������ˣ�����������Ʒ�л��Ļ�������Ҫ�����б���Ϣ
	// �����û��뿴�ĸ���Ϣ�������������

	virtual void OnDataRespNewsData(int iMmiReqId, IN const CMmiNewsBase *pMmiNewResp, CMerch *pMerch);

private:
	
	// Construction
protected:
	CDlgNotePad(CWnd* pParent = NULL);   // standard constructor

	void	InitShowType(E_ShowType eShowType, CGmtTime *pTimeInit = NULL, CIoViewBase *pEventIoView=NULL);

	void	ShowHtml(T_UserNoteInfo &note, bool32 bTryReadContent = false);	// �����������ʾ���� - �Ƿ����������ʾ
	void	ShowHtml(const CLandMineTitle &title, bool32 bTryReq = false);	// ��ʾ��Ϣ����
	void	ShowList();						// ��ʾ�б�״̬
	void	SwitchShow();					// �л���ʾ����������б�

	void	RecalcLayout();

	bool32	IsInListMode()const;;

	CString	MakeListItemDesc(const T_UserNoteInfo &note);
	CString MakeListItemDesc(const CLandMineTitle &mineTitle);
	CString GetCurDlgTitle();

	bool32	UpdateUserNote(const T_UserNoteInfo &note); // ����ָ��id�ıʼ���ʾ
	bool32  UpdateInfoMine(const CLandMineTitle &title);	// ����ָ���ĵ���, Ӧ�ò�������б���Ϣ

	int		GetListSelUserNote(OUT T_UserNoteInfo &note);
	int		GetListSelInfoMine(OUT CLandMineTitle &title);

	void	WatchMerchIsChanged(bool32 bNotify = true);			// �۲���Ʒ�Ƿ��б��

	void	UpdateBtnStatus();

	void	RefreshList();

	// Ͷ���ռ���ص�
	void	OnNewNote();
	void	OnModifyNote();
	void	OnDelNote();
	void	OnClearNote();

	// ��Ϣ����
	void	FilterInfoMind();	// ���˲�������ʾ����Ϣ����

	void	DoFloating();

	void	SetMyTitle(LPCTSTR lpszTitle);
	
	CSize			m_sizeMin;			// ����̫С
	CMerchKey		m_MerchCur;			// ��ǰ��Ʒ�����ڳ�ʼ��ʱ��mainframe��Ѱ������ͼ����ȡ��ע��Ʒ�����򲻴���
	CMerch			*m_pMerchCur;
	CRect			m_RectSeparator;	// �ָ���λ��

	CArray<int, int>		m_aBtnIds;

	E_ShowType				m_eShowType;
	CUserNoteInfoManager::UserNotesArray	m_aUserNotes;

	mapLandMinePseudo		m_mapInfoMines;		// ��ǰ�б����е���Ϣ����
	LandMinePseudoArray		m_aInfoMines;		// ��ǰ��ʾ����Ϣ���ף���һ�������µ�
	bool32					m_bHistoryMine;	// ��ʷ��Ϣ���ף�falseΪʵʱ����~~
	CGmtTime				m_TimeInit;		// ʵʱ��Ϣ������Ҫһ��ʱ��ָʾ

	//CWebBrowser2	m_WebBrowser2;		// �����������������html�ĵ�������ʾ

	CRichEditCtrl	m_RichEdit;			// �༭�ؼ�

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
	afx_msg void OnMsgTestViewDataListResp();				// �ٶ����ڵ����б�����
	afx_msg void OnMsgTestViewDataContentResp();			// �ٶ����ڵ������ݷ�����
	afx_msg LRESULT OnNcHitTest(CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNOTEPAD_H_)