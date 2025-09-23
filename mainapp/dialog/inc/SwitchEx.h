#ifndef _SWITCHEX_H_
#define _SWITCHEX_H_

// 按钮切换类
// 当点击按钮时，发送响应的命令，SendMessage(msgId, myCtrlId, HI: E_btnAction | LO:btnId)，根据指定模式，返回TRUE表示允许切换,FALSE不允许

class CSwitchEx : public CWnd
{
	DECLARE_DYNCREATE(CSwitchEx)
public:

	enum E_BtnAction{
		ActionLeftBtnDown,			// 鼠标左键按下事件
		ActionRightBtnDown,			// 鼠标右键按下事件
	};

	enum E_BtnSwitchMode{
		SwitchByClick,			// 点击即切换 - 默认
		SwitchByEcho,			// 根据msg的返回结果决定是否切换 - TRUE切
	};

	enum {
		DefaultMsgBaseId = (WM_USER + 0x145),		// 默认通知id基数
	};

	enum E_MsgId{
		MsgNotifyId,						// 实际通知id = + msgBase
		MsgGetBtnToolTipId,					// 获取按钮提示  // SendMessage(msgGetToolTip, HI:bufLen |LO:btnId, LPCTSTR buf), 返回tip字串长度
	};
	
	enum E_ToolTipMode{
		TipNone,						// 不使用tip
		TipByName,						// 使用按钮名字作为tip
		TipByMsgGet,					// 发送msg，获取tip
	};

public:
	CSwitchEx();
	~CSwitchEx();


	void	SetSwitchMode(E_BtnSwitchMode eMode);
	void	SetToolTipMode(E_ToolTipMode eMode);
	void	SetHideSomeBtnWhenTooSmall(BOOL bHide);	// 当太小是是否隐藏部分按钮
	void	SetShowScrollWhenSomeBtnHide(BOOL bShow); // 当有按钮隐藏时，是否显示 < > 按钮，滚动按钮

	int		SetBtnName(USHORT nId, const CString &StrName);
	



private:
	BOOL	RegisterMyClass();

	struct T_Btn{
		CRect    m_Rect;
		CString  m_StrName;
		USHORT   m_nWeight;
		bool	 m_bFixWidth;		// 宽度根据name和dc计算
		USHORT	 m_nId;				// 命令id -
		
		int		 m_iWidth;			// 计算后得到的宽度
		E_ToolTipMode	m_eTipMode;	// 获取tip的方式
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
