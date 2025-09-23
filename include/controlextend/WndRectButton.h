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
// ��ͼ�ӿ�
class CONTROL_EXPORT CRectButtonWndInterface
{
public:
	
	virtual	CWnd	&GetWnd() = 0;

	virtual	void	OnNewBtn(CWndRectButton &btn) = 0;	

	virtual bool	RequestMouseCapture(CWndRectButton &btn) = 0;
	virtual	CWndRectButton *GetCaptureBtn() = 0;
	virtual bool	ReleaseMouseCapture(CWndRectButton &btn) = 0;
};

// ������ͼʵ��
class CONTROL_EXPORT CRectButtonWndSimpleImpl : public CWnd, public CRectButtonWndInterface
{
	DECLARE_DYNAMIC(CRectButtonWndSimpleImpl);
public:
	CRectButtonWndSimpleImpl();
	~CRectButtonWndSimpleImpl();

public:
	// ��Ҫʵ�ֵĽӿ�
	typedef vector<CWndRectButton *>	WndRectButtonVector;
	virtual	void	GetAllBtns(WndRectButtonVector &aBtns) = 0;
	virtual void	OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler){};	// ��idleʱ����btn��ʾ��
	virtual void	OnDraw(CDC &dc);			// ����ʱ���̳ж�����ô˽ӿڻ��ư�ť
	virtual	void	RecalcLayout(bool32 bNeedPaint = true) = 0;			// ���������ť���ӿؼ�size

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
	void			SetLastFocusBtn(CWndRectButton *pBtn);	// ����ΪNULL
	CWndRectButton  *GetLastFocusBtn();

protected:
	CWndRectButton		*m_pBtnCurCapture;		// �۽��İ�ť��������;��Ч~
	CWndRectButton		*m_pBtnLastFocus;		// ����ƶ��İ�ť, ������;��Ч~

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
// ��ť���

// ֪ͨ
typedef struct tagRECTBTN_NOTIFY_ITEM
{
	NMHDR	hdr;
	uint16	uBtnId;			// ���崥����btn
	CWndRectButton *pBtn;		

	LPARAM	lParam;			// ��������
}RECTBTN_NOTIFY_ITEM;

// ��ɫ
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
// ��ť
class CONTROL_EXPORT CWndRectButton
{
public:
	enum E_BtnDrawState{		// ��ť����״̬ - 
		Normal,				// ����״̬
		Focus,				// ����ƶ� - ��Select״̬�£��������focus
		Selected,			// selected / pressed - ����Ŀǰʹ��LButtonDown����click��������û��select����������Ҳû����ѹ��Ч��- -
		Disable				// ��ֹ - ��ֹ״̬�£����ᷢ���κ���Ϣ
	};
	// ��ť��Ϊ
	enum E_BtnBehavior{
		BTNB_NORMAL,		// ��ͨ��ť��ֻ��click����check
		BTNB_RADIO,			// radioButton��ͬ����ֻ��һ���ܱ�ѡ��
		BTNB_CHECKBOX,		// CheckBox������һ�£�ѡ��/��ѡ���л�
		BTNB_SEPARATOR,		// �ָ�����û��click�¼���ֻ��һ������
	};

	// ��ɫ���� - ��16λΪ�������ͣ���8λ�������ť�����״̬(NFSD)����8Ϊ�������米��1��2
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
		Draw_Style1,		// ���ֻ��Ƶķ���
		Draw_Style2
	};

	enum E_ClickLBtnDrawStyle{
		ClickAfterLButtonUpDraw,		// �Ƿ��ڷ���click��Ϣ����ư�ť����״̬
		ClickBeforLButtonUpDraw,
	};

	// Marlett char�������õ��ַ�
	enum E_MarlettChar{
		Marlett_Begin = '0'-1,				// '0' Ϊ��һ��(������ǰ�棬����0)
		Marlett_Min = 48,					//  48 Min
		Marlett_MAX,						//  49 Max
		Marlett_RESTORE,					//  50 Restore
		Marlett_CHECKMARK=98,				//  98 Checkmark 
		Marlett_Bullet = 105,				// 105 Bullet
		Marlett_Close = 114,				// 114 Close
		Marlett_Help,						// ?
		Marlett_UpTriangle,					// �������ϵ�����
		Marlett_DownTriangle,				// ���µ�����
		Marlett_End = 'z'					// 'z'Ϊ���һ��(������)
	};
	CWndRectButton(CRectButtonWndInterface &wndTB);
	// hWndOwner - ���������, nID - ��button���е�ID, pszText��button����ʾ����
	// pszTip - ��ʾ����
	CWndRectButton(CRectButtonWndInterface &wndTB, CWnd *pWndOwner, uint16 nID, LPCTSTR pszText, LPCTSTR pszTip = NULL); // �Լ������ֻ���
	CWndRectButton(CRectButtonWndInterface &wndTB, CWnd *pWndOwner, uint16 nID, E_MarlettChar chMarllet, LPCTSTR pszTip = NULL); // �Լ������ֻ��� - ʹ��marllet��������ַ�ch
	// ��ͼ���� - ������ѡ��(ѹ��)������ƶ�����ֹ
	CWndRectButton(CRectButtonWndInterface &wndTB, CWnd *pWndOwner, uint16 nID, Image *pImageNomal, Image *pImageSelected, Image *pImageFocus, Image *pImageDisable, LPCTSTR pszTip = NULL);
	// ��ͼ���� + ���֣����ָ����Ϸ� - ������ѡ��(ѹ��)������ƶ�����ֹ
	CWndRectButton(CRectButtonWndInterface &wndTB, CWnd *pWndOwner, uint16 nID, LPCTSTR pszText, Image *pImageNomal, Image *pImageSelected, Image *pImageFocus, Image *pImageDisable, LPCTSTR pszTip = NULL);

	virtual ~CWndRectButton();

	void		SetBtnBehavior(E_BtnBehavior eNewBehavior, CWndRectRadioBtnGroup *pGroup = NULL); // �ı�button����Ϊ�����������ڳ�ʼ����ʱ����� pGroup - Radio��Ҫһ������Э������radio��check״̬
	void		SetCheck(bool32 bCheck);				// ֻ�� checkbox / radio �ܹ�����
	bool32		GetCheck() const;
	void		SetUseMarlettChar(bool32 bUse = false);			// �Ƿ�ʹ��MarlettChar���������б���ͼ��Ļ��� - lpszText�ᱻ����MarlettChar

	E_BtnBehavior	GetBtnBehavior()const { return m_eBtnBehavior; };
	bool32		IsSeparator()const { return BTNB_SEPARATOR == m_eBtnBehavior;  };

	void		SetBtnText(LPCTSTR pcszText){ m_StrText = pcszText; };		// Ŀǰֻ�ṩ��Щ���Բ���
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

	// Ŀǰ�������click������LButtonDown
	virtual     bool32	DoClicked(CPoint ptClick);		// �û�����˸ð�ť - ����֪ͨ��hWndOwner������hWndOwner�����أ�Ĭ�Ϸ���true����btnState����Ϊselect
	
	virtual		CSize   GetBtnTextExtend(CDC *pDC);		// �������ֻ�ȡ������Ҫ�Ĵ�С
	virtual		void	Draw(CDC &dc);						// ����state����Button�������߿����
	void	DrawNude();

	void	SetFont(CFont &font);

	bool32	GetColor(E_BtnColorType eType, E_BtnDrawState eState, int32 iIndex, COLORREF &clr);		// ���ݵ�ǰ״̬��ȡ�ض����͵���ɫ
	void	SetColor(E_BtnColorType eType, E_BtnDrawState eState, int32 iIndex, COLORREF clr, LPCTSTR lpszDesc=NULL, bool32 bNeedDraw = false);	// ����ǵ�һ�εĻ�����Ҫ���������ִ�
	void	SetColor(E_BtnColorType eType, int32 iIndex, COLORREF clr, LPCTSTR lpszDesc=NULL, bool32 bNeedDraw=false);	// ��4��״̬�Ķ�����ͬ����ɫ
	int		CombineBtnColorIndex(E_BtnColorType eType, E_BtnDrawState eState, int32 iIndex);	// ���ذ��չ�����Ƶ�����

	
	bool32	IsEnable() const;								// �Ƿ�ɽ�����Ϣ - �粻�ܽ�����TBWnd���ᷢ�������Ϣ�������ť
	void	EnableBtn(bool32 bEnable = true);
	// �����Ϣ��click�ɰ�ť�Լ����� - ���治���Լ�����windows button������
	virtual		void	OnMouseMove(UINT nFlag, CPoint point);		// ���TBWnd�յ�mouseleave�����һ���۽���ť���յ�point(-1,-1)��mousemove
	virtual		void	OnLButtonDown(UINT nFlag, CPoint point);	// ��ʱ��Ҫcapturemouse - �Ȳ�ʵ��capture��ֻ����һ����ʾЧ����up��ʱ���ֱ��click
	virtual     void	OnLButtonUp(UINT nFlag, CPoint point);		// ��ʱ��Ҫ����TBWnd�����releasemouse
	virtual		void	OnRButtonUp(UINT nFlag, CPoint point);		// �򵥵㣬ֻ��������Ҽ�������ò��ѹ�²��Ǻܱ�Ҫcapture

	CSize	MeasureBtnRect(CDC *pDC, OUT bool32 &bFixed);			// ��ȡ��Rectϣ����ȡ�Ĵ�С, bFixed��ʾ�Ƿ�Ӧ���̶���С
	void	GetBtnRect(OUT CRect &RectBtn);
	void	MoveBtn(const CRect &RectBtn, bool32 bDraw = true);			// ����recalcLayout���ⲿ���ǵ������������������ΰɣ�������ɫ��ʱ����ѭ��

	void	SetBtnState(E_BtnDrawState eState, bool32 bForce = false);		// ���ð�ť�µ�״̬
	
	void	SetBtnColors(const WndRectButtonColorItemMap &mapColor, bool32 bDrawIfChange = true);			// ���ð�ť��ɫ
	void	GetBtnColors(OUT WndRectButtonColorItemMap &mapColor) const ;

protected:
	friend class CWndRectRadioBtnGroup;

	// button������Ϣ��ʽ SendMessage(hWndOwner, nMessageNotifyId, LPARAM)
	virtual		bool32	NotifyOwner(UINT nNotifyCode = NM_CLICK, LPARAM lParam = NULL);						// ����wndTB��Ϣ֪ͨ��ʽ��֪ͨ hWndOwner click
	
	void	Initial(CWnd *pWndOwner, uint16 nID, LPCTSTR pszText, Image *pImageNomal, Image *pImageSelected, Image *pImageFocus, Image *pImageDisable, LPCTSTR pszTip);

	void	DrawStyle1(CDC &dc);
	void	DrawStyle2(CDC &dc);
	
	//static  void GetDrawTextInRect(CDC &dc, DWORD dwStyle, const CRect &rc, const CString &StrText,OUT CString StrCanDraw);

	CRectButtonWndInterface					&m_wndTB;		// ��TB����

	CWnd					*m_pWndOwner;
	uint16					m_nID;			// ushort 16 λ
	CString					m_StrText;
	CString					m_StrTip;
	Image					*m_pImageNomal;
	Image					*m_pImageSelected;
	Image					*m_pImageFocus;
	Image					*m_pImageDisable;

	CRect					m_RectButton;		// ��button��TBWnd����ľ���(TBWnd��Client����)
	E_BtnDrawState				m_eBtnState;

	E_BtnBehavior			m_eBtnBehavior;		// ����ͨ��ť����check/radio?
	CWndRectRadioBtnGroup		*m_pRadioGroup;		// �����radio��ť���������� - ����ΪNULL
	bool32					m_bCheck;

	CFont					m_font;
	bool32					m_bUseMarllet;		// ʹ����������marllet����һЩϵͳͼ��

	int						m_iGroupId;			// ������group

	E_DrawStyle				m_eDrawStyle;

	E_ClickLBtnDrawStyle			m_eClickStyle;
	bool32					m_bInLButtonUp;

	WndRectButtonColorItemMap		m_mapColorItems;
};


//////////////////////////////////////////////////////////////////////////
// radio���� - ��ʵ�֣�group����Ҫ�������btn�������ڳ�
class CONTROL_EXPORT CWndRectRadioBtnGroup{
public:
	
	void	RemoveAllBtns();
	int32	GetBtnsCount()const;
	void	ResetCheckState();		// �����а�ť��check״̬ȡ��

protected:
	friend class CWndRectButton;
	typedef	CMapPtrToWord	TBBtnMap;

	void	AddRadioBtn(CWndRectButton *pBtn);
	void	RemoveRadioBtn(CWndRectButton *pBtn);
	void	IamCheckBtn(CWndRectButton *pBtn);	// ���buttonһ��Ҫ�Ѿ���������飬����button������Ϊfalse

	TBBtnMap	m_mapRadioBtns;		// ��֤��һ����ֻ��һ��radiobutton��ѡ�У����Դ��ڶ�ûѡ�е���� - TBWndҪЭ������Щָ�������
};

//////////////////////////////////////////////////////////////////////////
// CWndRectButton �������
// CWndRectMenuButton - ����ұߡ�ʱ��������Ϣ��wndTB��wndTB����ѡ��˵�
class CONTROL_EXPORT CWndRectMenuButton : public CWndRectButton
{
public:
	enum E_MenuBtnEvent{
		MENU_BTN_LeftPartClick = 0,		// �Ҳ౻�����, LPARAM�ǵ������
		MENU_BTN_RightPartClick,				// ��ť��౻����� LPARAM�ǵ������
	};
	CWndRectMenuButton(CRectButtonWndInterface &wndTB, CWnd *pWndOwner, uint16 nID, LPCTSTR pszText, LPCTSTR pszTip = NULL, bool32 bLeftIsBtn = false);  // ���ֻ��� nNotifyIDΪ֪ͨwndTB����Ϣ
	
	virtual	bool32 DoClicked( CPoint ptClick );					// ������Ϊ��ť��Ϊ��������ͨ��ť���ͣ�ֻ�е�����ŵ����˵�������ѡ�������
	virtual	void   OnMouseMove(UINT nFlag, CPoint ptMove );		// ���ֻ���ƶ����ұߡ�λ�ò��з�Ӧ
	virtual void   OnLButtonDown( UINT nFlag, CPoint point );	// �������ұ����ǣ���ֱ�ӷ��Ͳ˵���Ϣ��wnd����ߵ���Ӧ��m_bEnableLeftButton������Ϊ
	virtual void   OnLButtonUp( UINT nFlag, CPoint point );		// �����ǲ�����up
	virtual	void   Draw( CDC &dc );

	virtual CSize  GetBtnTextExtend(CDC *pDC);
	
	void	GetRightRect(CRect &rc);
	
private:
	uint32	m_nRightRectWidth;		// �ұߡ����
	bool32	m_bEnableLeftButton;	// ����Ƿ�Ϊ��ť�������֣�false - ����(Ĭ��)���ǰ�ť����wnd���Ͱ�ť�¼�
	CPoint	m_ptLastClick;
	E_MenuBtnEvent		m_eClick;
};

#endif //!_WNDRECTBUTTON_H_