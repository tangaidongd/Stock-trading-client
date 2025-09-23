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
// ��map����������
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
// CIoViewReportSelect - ����һ��ѡ�ɴ��ڣ�ȡ����ʵ���Ĺ涨

class CIoViewReportSelect : public CIoViewReport, public CStockSelectTypeChanged
{

public:
	enum E_ReportSelectType
	{
		ERST_COMPLETE_RESULT,                 // ��ʾ������ѡ�ɽ��
	    ERST_FILTER_RESULT,                   // ��ʾɸѡ���ѡ�ɽ��
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
	virtual	bool32	FromXmlExtra(TiXmlElement *pElement);		// xml�����������
	virtual CString	ToXmlExtra();

public:
	// ��㷢�����ݰ�
	virtual	void	OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs);
	virtual void    OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual	void	OnVDataForceUpdate();
	virtual void	RequestViewData();

	virtual	bool32	IsPluginDataView() { return true; }
	virtual void	OnVDataPluginResp(const CMmiCommBase *pResp);

	virtual void    OnIoViewActive();

	virtual void    OnFixedRowClickCB(CCellID& cell);

	virtual	void	UpdatePushMerchs();		// ����ԭ���������⣬���ܻ���ҪһЩ���������

	virtual void	OnStockSelectTypeChanged();

	void			OnChooseStockData(const CMmiRespCRTEStategyChooseStock *pResp);

	void			OnMerchSortResp(const CMmiRespMerchSort *pResp);	// ��������ذ�

	void			AddMerchArrayToSmartAttend(const MerchArray &aMerchs, bool32 bRequestNow = false, bool32 bNeedF10=false, bool32 bNeedChooseStockData = false);
    bool32          IsKindOfReportSelect() { return true; }
	void            SetReportType(E_ReportSelectType eReportType) { m_eReportType = eReportType; }
	E_ReportSelectType   GetReportType() { return m_eReportType; }
public:
	bool32					m_bIoViewActiveChanging;

	CMmiReqReport			m_mmiTestReq;				// ������Ʒ���� - ʹ�����������滻����Ϊ���������Ķ�����Ʒ�б�
	CMmiReqMerchIndex		m_mmiIndexSelectStock;		// ���� ���� ѡ������ - ��������ȱ
	CMmiCommBase			*m_pMmiCurrentReq;			// ��ǰ������

	CMmiReqCRTEStategyChooseStock		m_mmiSelectStockReq;		// ѡ������
	CMmiReqPushPlugInChooseStock	m_mmiPushSelectStockReq;		// ������������ѡ��

	typedef	CArray<E_ReportSortEx, E_ReportSortEx>	HotSelectArray;
	typedef CArray<DWORD, DWORD> IndexSelectFlagArray;
	HotSelectArray			m_aHotSelect;
	SelectResultMap			m_mapMerchsHot;

	IndexSelectFlagArray	m_aIndexSelectFlag;
	SelectResultMap			m_mapSelectResult;

	bool32					m_bPromptedByLackOfRight;
	DWORD                   m_dwLastIndexSelectStockFlag;		// ��¼ѡ�ɱ�־

private:
	E_ReportSelectType      m_eReportType;           
protected:

	void					RequestViewData(CMmiCommBase &req);

	void					PromptLackRight(int eRightDeny);

	bool32					GetChooseStockTab(OUT int32 &iTab, OUT bool32 *pbIsCurrent = NULL);

	void					ResetShowData(const MerchArray &aMerchsShow); // �����ѡ��tab
	void					OpenSelBlock(const T_BlockDesc::E_BlockType &eType);	 // �뱨�۱�ƥ�䣬ѡ�ɴ����п��ܲ�����ѡ�ɵ�tab������ÿ����Ҫ��ǰ��ʱ���

	bool32					IsAttendResp(E_ReportSortEx eResp);
	bool32					IsAttendResp(DWORD dwTypes);
	void					ClearResultMap();
	bool32					AddMerchsToResultMap(u32 uTypes, const MerchArray &aMerchs);	// ��������ѡ����Ʒ, �����Ҫ����������Ѿ���������������Ʒ����
	DWORD					GetHotSelectTempArrayIndex(E_ReportSortEx eRankType);
	bool32					ConvertHotSelectTempArrayToShowArray(E_ReportSortEx eRankType);		// ����ʱ����������ת�Ƶ�ʵ�ʶ��У�����Ѿ�ȫ��ok���������ʾ����

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
