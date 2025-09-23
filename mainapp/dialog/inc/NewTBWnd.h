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

	// ���õ�ǰ��ʾ��ҳ��
	void ShowPage(int32 iID);				// ���밴ť��ID

	// ��ʾ��������IM����
	void OnShowIM();
	void OnHideIM();

	// �ı�IM����ײ���С
	void SetImBottom(int iBottom);
	// �ƶ�IM����
	void MoveIm(int iLeftOffset, int iTopOffset, int iRightOffset, int iBottomOffset);

	bool32	InitialCurrentWSP();			// ��ʼ����ǰ��������ʾ��Ϣ - �ú�������Ϊ�������Ѿ�������InitialAnalyToolBarBtns�Ļ�����
	bool32	GetCurrentWSP(CString &strWsp);	// ��ȡ��ǰ������

	void	SetF7(bool32 bShow);	// F7

	// �����г��˵�, ���ݲ˵�ID ȡ�г�/���ID
	int32		GetMenuMarketID(int32 iMenuID);

	// ����ϵͳĬ�ϵļ���cfm����Ҫ����Ҫ�л�����Ʒ
	static bool32		LoadSystemDefaultCfm(DWORD iIoViewCmd, bool32 bCloseExistence = false);
	static bool32		LoadSystemDefaultCfm(const CString &StrSDCName, bool32 bCloseExistence = false);

	static bool32	    DoOpenCfm(const CString &StrCfmXmlName, CString StrUrl = L"");
	static bool32		DoCloseCfm(const CString &StrCfmXmlName);

	void		ShowAllMarketMenu(CRect rect);
	void		KLineCycleChange(int iKlineType);
	
	CDlgPullDown* GetPullDown();
	//	�Ҽ���Ϣ����
	bool RbuttonProcess(int iSrcID);

	BOOL OnToolbarEvent(buttonContainerType &mapBtn, int32 iID, WPARAM wParam, LPARAM lParam);

	void SetDefaultCfmStatus();

private:
    void InitToolbarButtons();
    void InitFont();	
    void DrawToolbar(CDC& dc);
	void AddToolbarButton(const T_ColumnData &colData, int& iXStartPos, int &iYStartPos, const int iBtnTotalHeight, E_FixedType eFixedType=EFT_NONE);
    void AddNcButton(LPRECT lpRect, const T_ButtonData&btnData, bool bCategory = false, UINT nCount=3, E_FixedType eFixedType=EFT_NONE);
	// ����Ƿ��ڰ�ť��  
    void ShowMenu(T_ButtonData btnData, int id);
    bool FindBtnData(int id, int& iBtnDataIndex, int& iChlIndex, int& iColumnIndex, bool& bChildBtn,bool &bCol);
    bool FindSubBtnData(int id, int& iBtnDataIndex, int& iChlIndex,int&iSubChlIndex , int& iColumnIndex, bool& bChildBtn,bool &bCol);
	BOOL OnBtnEvent(CString strEventType, CString strEventParam, WPARAM wParam, LPARAM lParam, CString strBtnName = L"", int iWidth = 500, int iHeight = 300);
	BOOL OnSpecialFunc(CString strEventParam, WPARAM wParam, LPARAM lParam);
    BOOL OnIndexChangeFunc(CString strEventParam, WPARAM wParam, LPARAM lParam);
	BOOL OnCustomFunc(CString strEventParam, WPARAM wParam, LPARAM lParam);
	bool IsAllNumbers(CString str);
	//	ʵ�̽��ס�Ħ�������Ҽ�����
	bool RbuttonTradeProcess(int iSrcID);
	//	��ʾ�ļ��Ի���	
	bool ShowFileDlg(const CString& strOpenFilePath, CString& selectPath);
	//	ʵ�̽���"���ѿ��Ҽ��˵���ť����Ӧ�ó���·��"
	void ShowTradeFirstNotify();
	//	���������е����id��ť������
	#define  FIND_BTN_ITERATOR(container, id) std::find_if(container.begin(), container.end(),vector_finder(id))

	//	���������е����id�İ�ť��
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
		//����������Ӧ�ô���
		//ASSERT(0);
		return CNCButton();
	}
	
	CDlgPullDown* CreateSubMenu(T_ChildButtonData &btnData, int left, int top);		// ���ఴť�Ķ����˵�
	CDlgPullDown* AdjustSubMenuPos(CString strParent, int left, int top);	// ���������˵�����ʾλ��

	CString ChooseToLiveUrl();

	std::string GetRandomString(int iCharacterCount );	// ��������ַ���
	
private:

	//	����3�а�ť�����࣬��֧���Զ�����֧�������id����
    buttonContainerType m_mapBtn;							//	��������ť��
	buttonContainerType m_mapLeftFixedBtn;					//  ���������̶��ť��
	buttonContainerType m_mapRightFixedBtn;					//  �������Ҳ�̶��ť��
    buttonContainerType m_mapPullDown;						//	�����˵��İ�ť��
	buttonContainerType m_mapMorePullDown;					//	���������˵��İ�ť��
	buttonContainerType m_mapControlBtn;					//	���ư�ť����
	bool32		        m_bShowIntersect;					//	�Ƿ���ʾ��������ť����ư�ť�����İ�ť	
    CNCButton::T_NcFont m_fontSimpleBtn;			        //  ��ͨ��ť������
    CNCButton::T_NcFont m_fontCategoryBtn;		            //  ���ఴť������

	bool32					m_bPaint;						//	��һ��ˢ��ʱ�����������Ϣ
	CPoint					m_ptToolButtonLeftInset;		//	����������������ʼ�߾�
	int						m_iLeftFixedWidth;				//  ��������߹̶���ť��width
	int                     m_iRightFixedWidth;				//  �������ұ߹̶���ť��width
	int                     m_iRightFixedPos;				//  ��߹̶��ť���ұ�λ��
	int                     m_iLeftFixedPos;				//  �ұ߹̶��ť�����λ��
private:
	// ��ҽ���
	void		DoTradePlugin(int resId, bool32 isReselect);
	// ��ȡ���׳���·����û��������Ҫ�û�ѡ������
	bool32		GetTradeExePath(OUT CString &StrExePath,  IN LPCTSTR pSec, IN  LPCTSTR pKey, bool32 bReselect);
	//  ���ý��׳���·����NULL����0����Ϊɾ��������
	void		SetTradeExePath(LPCTSTR pSec, LPCTSTR pKey,LPCTSTR lpszExePath);	
	//	����toolbar.xml�а�ť���ݣ���ʱ������Ϊ���ʵ�̽������ݵ�id����map�У�ָ����ʱ·������Ϊ���ַ���	
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

	int32		m_indexNews; // ��ǰѡ��ĺ�����Ѷ,0��ƽ��1�������Ѷ��2�ƾ�����
private:

	// ��ǰ����һҳ
	int32		m_iCurrentPage;

	//
	CWnd		&m_wndParent;
	int			            m_iXButtonHovering;			// ��ʶ�����밴ť����
	T_FixedBtnHovering      m_stFixedBtnHovering;       // ��ʶ������̶���ť����

	vector<CDlgPullDown*> m_vecDlgSubPullDown;
	CDlgPullDown *m_pDlgPullDown;
	map<int32,int32>			  m_mapMenuID;	// �˵�id ��Ӧ���г�id
	map<int32, CString>				m_mapRealTrad; //���ʵ������

	// ����IM������λ��
	CRect m_RectIm;
	bool  m_blShowIM;

	COLORREF m_clrTextNor;                  // ��ť�ı�����ɫ
	COLORREF m_clrTextHot;
	COLORREF m_clrTextPress;

	COLORREF m_crlThirdTextNor;             // �����������а�ť�ı�����ɫ
	COLORREF m_crlThirdTextHot;
	COLORREF m_crlThirdTextPress;

	bool32	 m_bMember;                     // ��Ա��������

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
