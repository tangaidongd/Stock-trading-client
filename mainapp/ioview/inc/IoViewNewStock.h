#ifndef _IOVIEW_NEWSTOCK_H_
#define _IOVIEW_NEWSTOCK_H_

#include <vector>


// �¹�����
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

	int32	iNetPubTotal;			// ���Ϸ�����������ɣ�
	CString StrPurchaseCode;		// �깺����
	DWORD   dwPurchaseDate;			// �깺����	
	CString StrCode;				// ��Ʊ����
	float   fPubWinRate;			// ������ӯ��
	DWORD   dwListedDate;			// ��������
	float   fIndustryWinRate;		// ��ҵ��Ӯ��
	int32   iPubTotal;				// ��������
	DWORD   dwSignDate;				// ��ǩ��������
	CString StrStockName;			// ��Ʊ����
	float   fPubPrice;				// ���м�
	float   fPurchaseLimit;			// �깺���ޣ���ɣ�
	float   fFirstDayClosePrice;	// �������̼�
}T_NewStockInfo;

typedef std::vector<T_NewStockInfo> NewStockArray;



class  CNewStockNotify
{
public:
	virtual void OnNewStockResp() = 0;
};

// �¹� - �¹����ݴ���
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