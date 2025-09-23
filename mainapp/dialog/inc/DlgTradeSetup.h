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
// CDlgTradeSetup �Ի���

class CDlgTradeSetup : public CDialog
{
	DECLARE_DYNAMIC(CDlgTradeSetup)

public:
	CDlgTradeSetup(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgTradeSetup();

private:
	void			AddButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	int				TButtonHitTest(CPoint point);	// ����Ƿ��ڰ�ť��

private:

	void            DrawClientButton();

public:
	void			Initialize();           // ��ʼ������
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
	int			m_iXButtonHovering;					// ��ʶ�����밴ť����						
	

	T_IdleDownloadStatus      m_stDownloadStatus;	// �Ƿ��������Ӵ���������
	
	CRect		m_rcCaption;						// ������������	

	Image       *m_pImgNextPrev;

	CDlgTradeSettingData *m_pDlgMoreTrade;			// ��Ӹ��ཻ�����ͼ��
	//////////////////////////////����
	// first: ͼ������  second: ͼ����ļ���
	std::map<CString, CString>   m_mapBrokerIcon;   // �洢ȯ��ͼ���ļ�
	std::map<int32, CString>     m_mapTypeIcon;     // �洢�����������ͼ���ļ�
	//////////////////////////////
public:
	int32                        m_iCurrPage;				// ��ǰ���ҳ
	std::vector<CDlgTradeSettingData *> m_vecChildWnd;
// �Ի�������
	enum { IDD = IDD_DIALOG_TRADE_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
