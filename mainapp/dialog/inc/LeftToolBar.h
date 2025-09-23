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
		EST_BTN,		// �л���ť
		EST_WSP,		// �л�����
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
	
	void            DrawToolButton();				         // ���ƹ������ϵİ�ť
	void            ReLayout();					             // ���²��ְ�ť


	// �ڹ���������Ӱ�ť
	void            AddToolButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL, LPCTSTR lpszDecription = NULL);
	int             TButtonHitTest(CPoint point);			// ��ȡ��ǰ������ڵİ�ť
	int             GetBtnIDByName(CString StrName);
	void		    ChangeButtonStateByCfmName(const CString &StrWspName, const E_LeftBarType &eType);
	void            ShowTips();
	void            InitialButtonHoveringParam();	// ��ʼ����ʶ�����밴ť����

	// ����ϵͳĬ�ϵļ���cfm����Ҫ����Ҫ�л�����Ʒ
	bool32		    LoadSystemDefaultCfm(DWORD iIoViewCmd, bool32 bCloseExistence = false);
	bool32		    LoadSystemDefaultCfm(const CString &StrSDCName, bool32 bCloseExistence = false); 
	
	// ��ʾѡ�а�ť
	void            ShowSelectedBtn(int32 iID);
public:
    static	DWORD	GetIoViewCmdIdByCfmName(const CString &StrCfmName);
	void            SetCheckStatus(int iID);
	void            InitialToolBarBtns(E_LeftBarType eType);      //��ʼ����ť
	bool32			IsCurrWspMarket(int32 &iMarketId, CString StrWspName=L"");
	bool32			IsBtnInSameType(int32 iBtnId1, int32 iBtnId2);
	bool32			IsWspInSameType(int32 iBtnId1, int32 iBtnId2);
	void            ChangeLeftBarStatus(CString StrWspName = _T(""), const E_SwitchType &eType = EST_WSP);					// ��ʾ���������������������Ӧ��ť
	void			SetSwitchType(E_SwitchType eType);
	E_ReportType    GetMerchKind(IN CMerch *pMerch);
	void            ShowDuoGuTongLie();
private:
	/////////////////////////////////////////////////////
	//===================CODE CLEAN======================
	//////////////////////////////////////////////////////
	//void			GetSmartSelStockData(OUT vector<T_ButtonItem> &vecItem);//	��ȡ����ѡ����ʾ

// Operations

public:
	void           SetMarkedId(int32 iMarketID);             // �л��г�ID
	void           ChangeMarketId();
	bool32         ChangeLeftBar(const CString &StrCfmName);	     // ���ݰ����ļ��л����������
	void           ChangeDuoGuTongLieMerch(CMerch *pMerch);


public:
	// ����ѡ��״̬�ı�
	void ChangePickModelStatus();

	// Attributes
private:
	std::map<int, CNCButton> m_mapBtn;	 // �������ť����


	COLORREF m_clrTextNor;			     // ��ť�ı�����ɫ
	COLORREF m_clrTextHot;
	COLORREF m_clrTextPress;

	CNCButton::T_NcFont	m_fontFirst;     //��������һ��������

	Image	*m_pImgBk;				     // ����������ͼƬ
	Image   *m_pImgBtnNormal;			 // ����״̬�°�ťͼƬ
	Image   *m_pImgBtnNew;               // ѡ����Ʊ�ǰ�ťͼƬ

	// �ٶ�����ͼƬ��������ťͼƬ���Լ��������°�ťͼƬ�Ŀ����ͬ
	int32	m_iNumShowBtn;				// ��ʾ��߰�ť�ĸ���
	int32   m_iBtnHeight;               // �����������ť�ĸ߶�
	int32   m_iBtnWidth;				// �����������ť�Ŀ��
	int32   m_iToolBarWidth;            // ���������ͼ�Ŀ��
	int32	m_indexFirstShow;			// ��ʾ��ʼ��ťλ��
	int32	m_indexLastShow;			// ��ʾ������ťλ
	int32   m_indexSelShow;             // ��ǰѡ�а�ť��λ��

	CXTipWnd		m_TipWnd;			// ��ť��ʾ����
	CPoint			m_PointLast;

	CNCButton		m_ncBtnName;		// ������ǩ����


private:
	E_LeftBarType m_eLeftBarType;
	E_SwitchType  m_eSwitchType;
	std::map<E_LeftBarType, int32> m_mapButtonHovering;
	bool32        m_bSelUpDownBtn;					 
public:
	int32		  m_iMarketId;						 // �����г�Id

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
