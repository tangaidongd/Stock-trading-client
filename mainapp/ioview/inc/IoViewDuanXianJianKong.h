#ifndef _IOVIEWDUANXIANJIANKONG_H_
#define _IOVIEWDUANXIANJIANKONG_H_

//设置显示多个IoViewReport，右侧边缘添加可缩放至全屏的按钮

#include "IoViewBase.h"
#include "BlockConfig.h"
#include "PlugInStruct.h"

enum E_ShortMonitorType 
{
	ESMBigOrderAmount = 0, //500万以上成交额（原来的主力监控）
	ESMBigOrderVolume, //成交1000手以上
	ESMMin5Rate,	   //5分钟涨跌幅3%以上
	ESMEnd
};

class CIoViewDuanXianJianKong : public CIoViewBase ,public CObserverUserBlock
{
	//Construction

private:
	struct T_DuanXianJianKong
	{
		CMerch *m_pMerch;
		T_RespShortMonitor 	m_Shortonitor;
	};
	typedef	CArray<CMerch *, CMerch *>		MerchArray;
	typedef CList<T_DuanXianJianKong *, T_DuanXianJianKong *>	DuanXianJianKongList;
	typedef CArray<CMmiReqShortMonitor, const CMmiReqShortMonitor &>  ReqInitDataArray;

public:
	CIoViewDuanXianJianKong();
	virtual ~CIoViewDuanXianJianKong();

	DECLARE_DYNCREATE(CIoViewDuanXianJianKong)

public:
	virtual void     OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch) {};
	virtual void     SetChildFrameTitle() {}								 // 需要能给IoViewManager识别，所以要实现xml
	virtual bool32   FromXml(TiXmlElement *pElement);
	virtual CString  ToXml();
	virtual	CString  GetDefaultXML() { return L""; }
	virtual bool32	 GetStdMenuEnable(MSG* pMsg) { return false; }
	virtual CMerch   *GetMerchXml();										// 转到子ioview
	virtual	bool32	IsPluginDataView() { return true; }

	virtual void	 OnVDataPluginResp(const CMmiCommBase *pResp);

	// from CObserverUserBlock 通知
	virtual void	OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType);	

private:
	CRect			          m_RectTitle;
	CGridCtrlNormalSys		  m_GridCtrl;	
	CXScrollBar				  m_XSBVert;
	CXScrollBar				  m_XSBHorz;
	int32					  m_iMaxGridVisibleRow;						// 表格最大可见的数据

private:
	typedef CArray<CString, const CString &>HeaderArray;
	static	HeaderArray					m_saHeaders;

	DuanXianJianKongList				m_lstDuanXianJianKongSales;		// 所有有序数据都在这里
	multimap<CMerchKey, int>			m_multmapReqTheLastPushData;		// 最后一次推送的值商品信息

	bool32								m_bInitialized;					// 是否初始化完毕
	
private:
	void 	RecalcLayout();
	void 	SetGridHead();
	void 	OnDoInitialize();											// 初始化请求数据

	void    RequestDeletePushMerchNoExist();							// 删除不存在的商品推送
	void	RequestPushViewData(bool32 bForce = false);					// 请求推送数据
	void	RequestInitialData();										// 请求最近数据

	void	RefreshShortMonitorData ();										// 刷新短线监控数据
	void	OnShortTimeSaleResp( const CMmiRespShortMonitor *pResp );	// 回复数据处理函数
	void	UpdateGridRow( int iRow, const T_DuanXianJianKong &mts );	// 更新控件数据
	void	UpdateTableAllContent();									// 更新表所有内容
	T_DuanXianJianKong  *CheckMTS(T_DuanXianJianKong *pMTS) const;				// 检查该对象是否存在
	int32	InsertGridRow(int iRow, const T_DuanXianJianKong &mts, bool32 bDrawRow =false);
	void	OnGridlClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/);
	void	OnGridlDBClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/);
	bool32 InsertNewShortTimeSale(CMerch *pMerch,  const T_RespShortMonitor &shortTM , bool32 bAddToGrid/* = false*/);
protected:
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

};

#endif