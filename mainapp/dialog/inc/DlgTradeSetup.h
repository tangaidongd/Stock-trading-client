#pragma once

#include "NCButton.h"
#include "ConfigInfo.h"
#include "DlgTradeSettingData.h"
#include "HButton.h"
#include "afxcmn.h"

#include <map>
#include <vector>
#include "afxwin.h"

using namespace std;



typedef struct _IdleDownloadStatus
{
	CDlgTradeSettingData *pDlg;
	bool32                bIdle;

	_IdleDownloadStatus()
	{
		pDlg   = NULL;
		bIdle  = true;
	}
}T_IdleDownloadStatus;
// CDlgTradeSetup 对话框

class CDlgTradeSetup : public CDialog
{
	DECLARE_DYNAMIC(CDlgTradeSetup)

public:
	CDlgTradeSetup(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgTradeSetup();

private:
	void			AddButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	int				TButtonHitTest(CPoint point);	// 鼠标是否在按钮上

private:

	void            DrawClientButton();

public:
	void			Initialize();           // 初始化数据
	void			InitChildWindow();
	void			ReleaseChildWindow();
	void			ShowChildWindow();
	bool32          LoadXml();
	bool32          SaveXml();
	
public:
	void            AddTradeApp(T_TradeCfgParam &stData);

	void            ResetDownloadFlag();

	void            ResetDefaultTrade();

	void            HideAllChildWnd();

	bool32          GetDownloadStatusFlag();
	
	void			ResetDownloadStatus();

	void            ChangeTradeBtnStatus();

protected:	
	CFont			m_fontStaticText;
	CFont			m_fontCheckText;

private:

	std::map<int, CNCButton> m_mapBtn;
	int			m_iXButtonHovering;					// 标识鼠标进入按钮区域						
	

	T_IdleDownloadStatus      m_stDownloadStatus;	// 是否有其他子窗口在下载
	
	CRect		m_rcCaption;						// 标题栏的区域	

	Image       *m_pImgNextPrev;

	CDlgTradeSettingData *m_pDlgMoreTrade;			// 添加更多交易软件图标
	//////////////////////////////测试
	// first: 图标类型  second: 图标的文件名
	std::map<CString, CString>   m_mapBrokerIcon;   // 存储券商图标文件
	std::map<int32, CString>     m_mapTypeIcon;     // 存储交易软件类型图标文件
	//////////////////////////////
public:
	int32                        m_iCurrPage;				// 当前浏览页
	std::vector<CDlgTradeSettingData *> m_vecChildWnd;
// 对话框数据
	enum { IDD = IDD_DIALOG_TRADE_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
	public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//	
	afx_msg LRESULT OnMsgShowTradeDownload(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgCloseChildWnd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgSetDefaultChildWnd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgChangeIdleStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgHideMainDlg(WPARAM wParam, LPARAM lParam);
};
