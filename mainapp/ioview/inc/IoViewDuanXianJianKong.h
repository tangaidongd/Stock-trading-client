#ifndef _IOVIEWDUANXIANJIANKONG_H_
#define _IOVIEWDUANXIANJIANKONG_H_

//������ʾ���IoViewReport���Ҳ��Ե��ӿ�������ȫ���İ�ť

#include "IoViewBase.h"
#include "BlockConfig.h"
#include "PlugInStruct.h"

enum E_ShortMonitorType 
{
	ESMBigOrderAmount = 0, //500�����ϳɽ��ԭ����������أ�
	ESMBigOrderVolume, //�ɽ�1000������
	ESMMin5Rate,	   //5�����ǵ���3%����
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
	virtual void     SetChildFrameTitle() {}								 // ��Ҫ�ܸ�IoViewManagerʶ������Ҫʵ��xml
	virtual bool32   FromXml(TiXmlElement *pElement);
	virtual CString  ToXml();
	virtual	CString  GetDefaultXML() { return L""; }
	virtual bool32	 GetStdMenuEnable(MSG* pMsg) { return false; }
	virtual CMerch   *GetMerchXml();										// ת����ioview
	virtual	bool32	IsPluginDataView() { return true; }

	virtual void	 OnVDataPluginResp(const CMmiCommBase *pResp);

	// from CObserverUserBlock ֪ͨ
	virtual void	OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType);	

private:
	CRect			          m_RectTitle;
	CGridCtrlNormalSys		  m_GridCtrl;	
	CXScrollBar				  m_XSBVert;
	CXScrollBar				  m_XSBHorz;
	int32					  m_iMaxGridVisibleRow;						// ������ɼ�������

private:
	typedef CArray<CString, const CString &>HeaderArray;
	static	HeaderArray					m_saHeaders;

	DuanXianJianKongList				m_lstDuanXianJianKongSales;		// �����������ݶ�������
	multimap<CMerchKey, int>			m_multmapReqTheLastPushData;		// ���һ�����͵�ֵ��Ʒ��Ϣ

	bool32								m_bInitialized;					// �Ƿ��ʼ�����
	
private:
	void 	RecalcLayout();
	void 	SetGridHead();
	void 	OnDoInitialize();											// ��ʼ����������

	void    RequestDeletePushMerchNoExist();							// ɾ�������ڵ���Ʒ����
	void	RequestPushViewData(bool32 bForce = false);					// ������������
	void	RequestInitialData();										// �����������

	void	RefreshShortMonitorData ();										// ˢ�¶��߼������
	void	OnShortTimeSaleResp( const CMmiRespShortMonitor *pResp );	// �ظ����ݴ�����
	void	UpdateGridRow( int iRow, const T_DuanXianJianKong &mts );	// ���¿ؼ�����
	void	UpdateTableAllContent();									// ���±���������
	T_DuanXianJianKong  *CheckMTS(T_DuanXianJianKong *pMTS) const;				// ���ö����Ƿ����
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