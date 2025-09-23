#pragma once
#include "afxwin.h"
#include "StaticEx.h"
#include "EditFlat.h"
#include "NCButton.h"
#include <UrlMon.h>
#include "afxwin.h"

using namespace std;


#pragma comment(lib, "urlmon.lib")
// CDlgTradeSettingData 对话框

typedef enum E_TradeAppType
{
	ETTFutures = 0,			// 期货	
	ETTStock,				// 证券
}E_TradeAppType;




class CDlgTradeSettingData : public CDialog, public IBindStatusCallback
{
	DECLARE_DYNAMIC(CDlgTradeSettingData)

public:
	CDlgTradeSettingData(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgTradeSettingData();
	
	void InitData();
	void LoadBrokerIcon(CString strFileName);
	void DownloadBrokerLog();

private:
	void AddButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption = NULL);
	int	 TButtonHitTest(CPoint point);	                  // 鼠标是否在按钮上

	// 开始下载
	void			StartDownloadTrade();

	// 停止下载
	void			StopDownloadTrade();

	// 请求弹窗资讯
	int				RunDownloadTrade();
	static unsigned int __stdcall DownloadTradeThread(void *pParam);
public:
	//////////////////////////////////////////////////////////////////////////
	// From IBindStatusCallback
	//IBindStatusCallback的方法。除了OnProgress     外的其他方法都返回E_NOTIMPL           
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

	//OnProgress在这里  
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

	// IUnknown方法.IE 不会调用这些方法的  

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

	Image      *m_pImgBkg;				 // 背景图片
	Image      *m_pImgDefaultBkg;        // 默认选择背景图片
	Image      *m_pImgBrokerLogo;		 // 券商Logo
	Image      *m_pImgMoreTradeBk;       // 
	Image      *m_pImgDefaultBrokerLog;  // 默认券商logo

	Image      *m_pImgTradeExit;         // 关闭按钮
	Image      *m_pImgDefaultBtn;		 // 默认按钮
	Image      *m_pImgLoginBtn;			 // 登录按钮


	CStaticEx  m_staticTradeName;	// 交易程序名称
	CStaticEx  m_staticAppType;     // 交易程序类型

	bool32     m_bIsMoreTrade;		// 是否添加更多图标


	T_TradeCfgParam  m_stTradeCfgData;	// 交易窗口所需参数

	int	       m_iXButtonHovering;  // 标识鼠标进入按钮区域

	CFont      m_fontContent;

	HANDLE	   m_hDownloadHandle;	// 下载线程句柄

	std::map<int, CNCButton> m_mapBtn;
public:
// 对话框数据
	enum { IDD = IDD_DIALOG_TRADESETTING_DATA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
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
