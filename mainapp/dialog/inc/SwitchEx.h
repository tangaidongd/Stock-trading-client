#ifndef _SWITCHEX_H_
#define _SWITCHEX_H_

// ��ť�л���
// �������ťʱ��������Ӧ�����SendMessage(msgId, myCtrlId, HI: E_btnAction | LO:btnId)������ָ��ģʽ������TRUE��ʾ�����л�,FALSE������

class CSwitchEx : public CWnd
{
	DECLARE_DYNCREATE(CSwitchEx)
public:

	enum E_BtnAction{
		ActionLeftBtnDown,			// �����������¼�
		ActionRightBtnDown,			// ����Ҽ������¼�
	};

	enum E_BtnSwitchMode{
		SwitchByClick,			// ������л� - Ĭ��
		SwitchByEcho,			// ����msg�ķ��ؽ�������Ƿ��л� - TRUE��
	};

	enum {
		DefaultMsgBaseId = (WM_USER + 0x145),		// Ĭ��֪ͨid����
	};

	enum E_MsgId{
		MsgNotifyId,						// ʵ��֪ͨid = + msgBase
		MsgGetBtnToolTipId,					// ��ȡ��ť��ʾ  // SendMessage(msgGetToolTip, HI:bufLen |LO:btnId, LPCTSTR buf), ����tip�ִ�����
	};
	
	enum E_ToolTipMode{
		TipNone,						// ��ʹ��tip
		TipByName,						// ʹ�ð�ť������Ϊtip
		TipByMsgGet,					// ����msg����ȡtip
	};

public:
	CSwitchEx();
	~CSwitchEx();


	void	SetSwitchMode(E_BtnSwitchMode eMode);
	void	SetToolTipMode(E_ToolTipMode eMode);
	void	SetHideSomeBtnWhenTooSmall(BOOL bHide);	// ��̫С���Ƿ����ز��ְ�ť
	void	SetShowScrollWhenSomeBtnHide(BOOL bShow); // ���а�ť����ʱ���Ƿ���ʾ < > ��ť��������ť

	int		SetBtnName(USHORT nId, const CString &StrName);
	



private:
	BOOL	RegisterMyClass();

	struct T_Btn{
		CRect    m_Rect;
		CString  m_StrName;
		USHORT   m_nWeight;
		bool	 m_bFixWidth;		// ��ȸ���name��dc����
		USHORT	 m_nId;				// ����id -
		
		int		 m_iWidth;			// �����õ��Ŀ��
		E_ToolTipMode	m_eTipMode;	// ��ȡtip�ķ�ʽ
		T_Btn();
	};

	typedef CArray<T_Btn, const T_Btn &> BtnArray;


	int			m_iMsgBaseId;

	E_BtnSwitchMode m_eSwitchMode;
	E_ToolTipMode	m_eToolTipMode;

public:
	/////////////////////////////////////////////////////
	/////////////////////////////////////////////////////
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSwitchEx)
	BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, int iMsgBaseId = DefaultMsgBaseId, CCreateContext* pContext  = NULL);
	int  OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	//}}AFX_VIRTUAL

protected:
	
	//{{AFX_MSG(CSwitchEx)

	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

#endif
