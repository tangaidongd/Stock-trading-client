#ifndef _IO_VIEW_REPORT_SELECT_H_
#define _IO_VIEW_REPORT_SELECT_H_


#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "CStaticSD.h"
#include "GuiTabWnd.h"
#include "ReportScheme.h"
#include "MerchManager.h"
#include "BlockManager.h"
#include "PlugInStruct.h"
#include "StockSelectManager.h"

#include "IoViewReport.h"

#include <map>
using std::map;

//////////////////////////////////////////////////////////////////////////
// 与map合作的数组
template<class TYPE, class ARG_TYPE>
class CArrayWithCopy : public CArray<TYPE, ARG_TYPE>
{
public:
	typedef CArray<TYPE, ARG_TYPE>	ArrayType;
	CArrayWithCopy(){};
	CArrayWithCopy(const ArrayType &aSrc)
	{ 
		Copy(aSrc);
	};
	CArrayWithCopy(const CArrayWithCopy &aSrc)
	{
		Copy(static_cast<const ArrayType &>(aSrc));
	}

	const CArrayWithCopy &operator=(const CArrayWithCopy &aSrc)
	{
		Copy(static_cast<const ArrayType &>(aSrc));
	}
	const CArrayWithCopy &operator=(const ArrayType &aSrc)
	{
		Copy(aSrc);
	}
};


/////////////////////////////////////////////////////////////////////////////
// CIoViewReportSelect - 就是一个选股窗口，取消单实例的规定

class CIoViewReportSelect : public CIoViewReport, public CStockSelectTypeChanged
{

public:
	enum E_ReportSelectType
	{
		ERST_COMPLETE_RESULT,                 // 显示完整的选股结果
	    ERST_FILTER_RESULT,                   // 显示筛选后的选股结果
	};

// Construction 
public:
	DECLARE_DYNCREATE(CIoViewReportSelect)

	typedef CArrayWithCopy<CMerch *, CMerch *>				MerchArray;
	typedef map<DWORD, MerchArray>					SelectResultMap;
	typedef map<CMerch *, UINT>						MerchCountMap;

	CIoViewReportSelect();
	virtual ~CIoViewReportSelect();

public:
	virtual	bool32	FromXmlExtra(TiXmlElement *pElement);		// xml请求参数设置
	virtual CString	ToXmlExtra();

public:
	// 随便发点数据吧
	virtual	void	OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs);
	virtual void    OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual	void	OnVDataForceUpdate();
	virtual void	RequestViewData();

	virtual	bool32	IsPluginDataView() { return true; }
	virtual void	OnVDataPluginResp(const CMmiCommBase *pResp);

	virtual void    OnIoViewActive();

	virtual void    OnFixedRowClickCB(CCellID& cell);

	virtual	void	UpdatePushMerchs();		// 除了原来的数据外，可能还需要一些额外的数据

	virtual void	OnStockSelectTypeChanged();

	void			OnChooseStockData(const CMmiRespCRTEStategyChooseStock *pResp);

	void			OnMerchSortResp(const CMmiRespMerchSort *pResp);	// 排行请求回包

	void			AddMerchArrayToSmartAttend(const MerchArray &aMerchs, bool32 bRequestNow = false, bool32 bNeedF10=false, bool32 bNeedChooseStockData = false);
    bool32          IsKindOfReportSelect() { return true; }
	void            SetReportType(E_ReportSelectType eReportType) { m_eReportType = eReportType; }
	E_ReportSelectType   GetReportType() { return m_eReportType; }
public:
	bool32					m_bIoViewActiveChanging;

	CMmiReqReport			m_mmiTestReq;				// 测试商品请求 - 使用排序请求替换，因为反正回来的都是商品列表
	CMmiReqMerchIndex		m_mmiIndexSelectStock;		// 短线 中线 选股请求 - 其它的暂缺
	CMmiCommBase			*m_pMmiCurrentReq;			// 当前的请求

	CMmiReqCRTEStategyChooseStock		m_mmiSelectStockReq;		// 选股请求
	CMmiReqPushPlugInChooseStock	m_mmiPushSelectStockReq;		// 请求推送条件选股

	typedef	CArray<E_ReportSortEx, E_ReportSortEx>	HotSelectArray;
	typedef CArray<DWORD, DWORD> IndexSelectFlagArray;
	HotSelectArray			m_aHotSelect;
	SelectResultMap			m_mapMerchsHot;

	IndexSelectFlagArray	m_aIndexSelectFlag;
	SelectResultMap			m_mapSelectResult;

	bool32					m_bPromptedByLackOfRight;
	DWORD                   m_dwLastIndexSelectStockFlag;		// 记录选股标志

private:
	E_ReportSelectType      m_eReportType;           
protected:

	void					RequestViewData(CMmiCommBase &req);

	void					PromptLackRight(int eRightDeny);

	bool32					GetChooseStockTab(OUT int32 &iTab, OUT bool32 *pbIsCurrent = NULL);

	void					ResetShowData(const MerchArray &aMerchsShow); // 仅变更选股tab
	void					OpenSelBlock(const T_BlockDesc::E_BlockType &eType);	 // 与报价表匹配，选股窗口有可能不存在选股的tab，所以每次需要置前的时候打开

	bool32					IsAttendResp(E_ReportSortEx eResp);
	bool32					IsAttendResp(DWORD dwTypes);
	void					ClearResultMap();
	bool32					AddMerchsToResultMap(u32 uTypes, const MerchArray &aMerchs);	// 返回来的选股商品, 如果需要运算的请求都已经回来，则运算商品集合
	DWORD					GetHotSelectTempArrayIndex(E_ReportSortEx eRankType);
	bool32					ConvertHotSelectTempArrayToShowArray(E_ReportSortEx eRankType);		// 将临时队列中数据转移到实际队列，如果已经全部ok，则更新显示数据

	/////////////////////////////////////////////////////
	/////////////////////////////////////////////////////
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewReportSelect)
	void PostNcDestroy();
	//}}AFX_VIRTUAL
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewReportSelect)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_REPORT_SELECT_H_
