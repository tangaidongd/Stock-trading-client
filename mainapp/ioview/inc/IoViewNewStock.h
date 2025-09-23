#ifndef _IOVIEW_NEWSTOCK_H_
#define _IOVIEW_NEWSTOCK_H_

#include <vector>


// 新股数据
typedef struct T_NewStockInfo
{
	T_NewStockInfo()
	{
		iNetPubTotal = 0;		
		StrPurchaseCode =  _T("");	
		dwPurchaseDate = 0;		
		StrCode = _T("");			
		fPubWinRate = 0.0;		
		dwListedDate = 0;		
		fIndustryWinRate = 0.0;	
		iPubTotal = 0;			
		dwSignDate = 0;			
		StrStockName = _T("");		
		fPubPrice = 0.0;			
		fPurchaseLimit = 0.0;		
		fFirstDayClosePrice = 0.0;
	}

	int32	iNetPubTotal;			// 网上发行总数（万股）
	CString StrPurchaseCode;		// 申购代码
	DWORD   dwPurchaseDate;			// 申购日期	
	CString StrCode;				// 股票代码
	float   fPubWinRate;			// 发行市盈率
	DWORD   dwListedDate;			// 上市日期
	float   fIndustryWinRate;		// 行业市赢率
	int32   iPubTotal;				// 发行总数
	DWORD   dwSignDate;				// 中签公布日期
	CString StrStockName;			// 股票名称
	float   fPubPrice;				// 发行价
	float   fPurchaseLimit;			// 申购上限（万股）
	float   fFirstDayClosePrice;	// 首日收盘价
}T_NewStockInfo;

typedef std::vector<T_NewStockInfo> NewStockArray;



class  CNewStockNotify
{
public:
	virtual void OnNewStockResp() = 0;
};

// 新股 - 新股数据处理
class CNewStockManager : public CWnd
{
public:
	DECLARE_DYNAMIC(CNewStockManager)
	friend class CViewData;

	~CNewStockManager();;

	static CNewStockManager &Instance();

	void	OnNewStockResp(const char *pszRecvData, bool bListedStock);
	void	AddNewStockNotify(CNewStockNotify *pNotify);
	void	RemoveNewStockNotify(CNewStockNotify *pNotify);
	
	CArray<CNewStockNotify *, CNewStockNotify*> m_pNewStockNotifyPtrList;

public:
	void	RequestNewStockData();

	void    PackNewStockJsonData(string &strJsonData, bool bListedStock=true);

	void    UnPackNewStockInfo(const char* pszRecvData, bool32 bListedStock = true);

	NewStockArray		&GetNewStockInfo(bool32 bListedStock = true);

protected:
	CNewStockManager();
	
	CNewStockManager(const CNewStockManager&);
	CNewStockManager &operator=(const CNewStockManager &);

	

	NewStockArray	m_aListedStock;
	NewStockArray	m_aUnlistedStock;
	//{{AFX_MSG(CNewStockManager)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

#endif