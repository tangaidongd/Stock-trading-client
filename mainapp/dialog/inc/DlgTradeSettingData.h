#pragma once
#include "afxwin.h"
#include "StaticEx.h"
#include "EditFlat.h"
#include "NCButton.h"
#include <UrlMon.h>
#include "afxwin.h"

using namespace std;


#pragma comment(lib, "urlmon.lib")
// CDlgTradeSettingData �Ի���

typedef enum E_TradeAppType
{
	ETTFutures = 0,			// �ڻ�	
	ETTStock,				// ֤ȯ
}E_TradeAppType;




class CDlgTradeSettingData : public CDialog, public IBindStatusCallback
{
	DECLARE_DYNAMIC(CDlgTradeSettingData)

public:
	CDlgTradeSettingData(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgTradeSettingData();
	
	void InitData();
	void LoadBrokerIcon(CString strFileName);
	void DownloadBrokerLog();

private:
	void AddButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption = NULL);
	int	 TButtonHitTest(CPoint point);	                  // ����Ƿ��ڰ�ť��

	// ��ʼ����
	void			StartDownloadTrade();

	// ֹͣ����
	void			StopDownloadTrade();

	// ���󵯴���Ѷ
	int				RunDownloadTrade();
	static unsigned int __stdcall DownloadTradeThread(void *pParam);
public:
	//////////////////////////////////////////////////////////////////////////
	// From IBindStatusCallback
	//IBindStatusCallback�ķ���������OnProgress     �����������������E_NOTIMPL           
	STDMETHOD(OnStartBinding)
		(DWORD dwReserved,
		IBinding __RPC_FAR *pib)
	{ return E_NOTIMPL; }

	STDMETHOD(GetPriority)
		(LONG __RPC_FAR *pnPriority)
	{ return E_NOTIMPL; }

	STDMETHOD(OnLowResource)
		(DWORD reserved)
	{ return E_NOTIMPL; }

	//OnProgress������  
	STDMETHOD(OnProgress)
		(ULONG ulProgress,
		ULONG ulProgressMax,
		ULONG ulStatusCode,
		LPCWSTR wszStatusText);

	STDMETHOD(OnStopBinding)
		(HRESULT hresult,
		LPCWSTR szError)
	{ return E_NOTIMPL; }

	STDMETHOD(GetBindInfo)
		(DWORD __RPC_FAR *grfBINDF,
		BINDINFO __RPC_FAR *pbindinfo)
	{ return E_NOTIMPL; }

	STDMETHOD(OnDataAvailable)
		(DWORD grfBSCF,
		DWORD dwSize,
		FORMATETC __RPC_FAR *pformatetc,
		STGMEDIUM __RPC_FAR *pstgmed)
	{ return E_NOTIMPL; }

	STDMETHOD(OnObjectAvailable)
		(REFIID riid,
		IUnknown __RPC_FAR *punk)
	{ return E_NOTIMPL; }

	// IUnknown����.IE ���������Щ������  

	STDMETHOD_(ULONG, AddRef)()
	{ return 0; }

	STDMETHOD_(ULONG, Release)()
	{ return 0; }

	STDMETHOD(QueryInterface)
		(REFIID riid,
		void __RPC_FAR *__RPC_FAR *ppvObject)
	{ return E_NOTIMPL; }
	//////////////////////////////////////////////////////////////////////////
public:

	Image      *m_pImgBkg;				 // ����ͼƬ
	Image      *m_pImgDefaultBkg;        // Ĭ��ѡ�񱳾�ͼƬ
	Image      *m_pImgBrokerLogo;		 // ȯ��Logo
	Image      *m_pImgMoreTradeBk;       // 
	Image      *m_pImgDefaultBrokerLog;  // Ĭ��ȯ��logo

	Image      *m_pImgTradeExit;         // �رհ�ť
	Image      *m_pImgDefaultBtn;		 // Ĭ�ϰ�ť
	Image      *m_pImgLoginBtn;			 // ��¼��ť


	CStaticEx  m_staticTradeName;	// ���׳�������
	CStaticEx  m_staticAppType;     // ���׳�������

	bool32     m_bIsMoreTrade;		// �Ƿ���Ӹ���ͼ��


	T_TradeCfgParam  m_stTradeCfgData;	// ���״����������

	int	       m_iXButtonHovering;  // ��ʶ�����밴ť����

	CFont      m_fontContent;

	HANDLE	   m_hDownloadHandle;	// �����߳̾��

	std::map<int, CNCButton> m_mapBtn;
public:
// �Ի�������
	enum { IDD = IDD_DIALOG_TRADESETTING_DATA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	
	afx_msg LRESULT OnMsgProgressSetPos(WPARAM wParam, LPARAM lParam);
public:
	CProgressCtrl m_ProgressDownload;
};
