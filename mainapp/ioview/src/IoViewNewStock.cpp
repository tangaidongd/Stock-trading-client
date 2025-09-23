#include "StdAfx.h"
#include "IoViewNewStock.h"
#include "coding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
const int32 	KIRequestDataTimerId = 1008;						// 定时获取新股状态
const int32	    KIRequestDataTimerPeriod = 1;

IMPLEMENT_DYNAMIC(CNewStockManager, CWnd)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CNewStockManager, CWnd)
//{{AFX_MSG_MAP(CNewStockManager)
ON_WM_TIMER()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////

CNewStockManager::CNewStockManager()
{
	m_aListedStock.clear();
	m_aUnlistedStock.clear();
}

CNewStockManager & CNewStockManager::Instance()
{
	static CNewStockManager newStock;
	if ( NULL == newStock.m_hWnd )
	{
		newStock.CreateEx(0, _T("Static"), _T("ChooseStock"), WS_POPUP, CRect(0,0,0,0), NULL, 0, NULL );

// 		if (newStock.m_hWnd != NULL)
// 		{
// 			newStock.SetTimer(KIRequestDataTimerId, KIRequestDataTimerPeriod, NULL);
// 		}
	}
	return newStock;
}

void CNewStockManager::OnTimer( UINT nIDEvent )
{
// 	if ( KIRequestDataTimerId == nIDEvent )
// 	{
// 		RequestViewData();
// 		KillTimer(KIRequestDataTimerId);
// 	}
	CWnd::OnTimer(nIDEvent);
}

void CNewStockManager::RequestNewStockData()
{
	CGGTongDoc	*pDoc =  (CGGTongDoc*)AfxGetDocument();
	CString strQueryStockIp;
	int		iPort;	
	if ( pDoc && pDoc->m_pAutherManager)
	{
		strQueryStockIp = pDoc->GetQueryStockIP();
		iPort		 =  pDoc->GetQueryStockPort();
		if (strQueryStockIp.IsEmpty())
		{
			return;
		}
		CString strAddrFormat;
		wstring strAddr;
		strAddrFormat.Format(_T("%s:%d"), strQueryStockIp, iPort);
		int iIndex = strAddrFormat.Find(_T("http://"));
		if (-1 == iIndex)
		{
			strAddrFormat = _T("http://") + strAddrFormat;
		}
		strAddr = strAddrFormat;
		string strJson;
		
		// 已上市新股数据请求
		PackNewStockJsonData(strJson, true);
		pDoc->m_pAutherManager->GetInterface()->ReqQueryNewStockInfo(strAddr.c_str(), L"/newStock/queryStockList", strJson.c_str(), true);

		// 未上市新股数据请求
		PackNewStockJsonData(strJson, false);
		pDoc->m_pAutherManager->GetInterface()->ReqQueryNewStockInfo(strAddr.c_str(), L"/newStock/queryStockList", strJson.c_str(), false);

	}
}

CNewStockManager::~CNewStockManager()
{
	m_pNewStockNotifyPtrList.RemoveAll();

	CWnd::DestroyWindow();
}

void CNewStockManager::OnNewStockResp(const char *pszRecvData, bool bListedStock)
{ 
	UnPackNewStockInfo(pszRecvData, bListedStock);
//  for (int32 i = 0; i < m_pNewStockNotifyPtrList.GetSize(); i++)
//  {
//  	CNewStockNotify *pNotify = m_pNewStockNotifyPtrList[i];
//  	if (NULL != pNotify)
//  	{
//  		pNotify->OnNewStockResp();
//  	}
//  }
}

void CNewStockManager::AddNewStockNotify(CNewStockNotify *pNotify)
{
	if (NULL == pNotify)
		return;

	bool32 bFind = FALSE;
	for (int32 i = 0; i < m_pNewStockNotifyPtrList.GetSize(); i++)
	{
		if (m_pNewStockNotifyPtrList[i] == pNotify)
		{
			bFind = TRUE;
			break;
		}
	}

	if (!bFind)
	{
		m_pNewStockNotifyPtrList.Add(pNotify);
	}
}

void CNewStockManager::RemoveNewStockNotify(CNewStockNotify *pNotify)
{
	if (NULL == pNotify)
		return;

	// 找到所有的, 剔除（可能有重复的情况）
	while (1)
	{
		int32 iFindPos = -1;
		for (int32 i = 0; i < m_pNewStockNotifyPtrList.GetSize(); i++)
		{
			if (m_pNewStockNotifyPtrList[i] == pNotify)
			{
				iFindPos = i;
				break;
			}
		}

		if (iFindPos >= 0)
		{
			m_pNewStockNotifyPtrList.RemoveAt(iFindPos, 1);
		}
		else
		{
			break;
		}
	}
}

void CNewStockManager::PackNewStockJsonData(string &strJsonData, bool bListedStock/*=true*/)
{
	Json::Value root;
	Json::Value jsValueCmd;
	CString strToken;
	string strFieldTemp;

	root.clear();
	jsValueCmd.clear();
	root["version"]		= "2.0";
	
	jsValueCmd["listedDays"] = (bListedStock ? "30" : "0");
	jsValueCmd["pageIndex"]	= "1";
	jsValueCmd["pageSize"]	= "30";	
	root["cmd"] = jsValueCmd;

	Json::FastWriter jsonWriter;
	string sInput = jsonWriter.write(root);

	Gbk32ToUtf8(sInput.c_str(), strJsonData);
}

void CNewStockManager::UnPackNewStockInfo(const char* pszRecvData, bool32 bListedStock)
{
	NewStockArray &arrayNewStock = GetNewStockInfo(bListedStock);
	arrayNewStock.clear();

	int iStatus = -1;	// 状态 
	Json::Reader jsonReader;
	Json::Value	 jsonValue;
	if (jsonReader.parse(pszRecvData, jsonValue))
	{
		iStatus = jsonValue["status"].asInt();

		if (0 == iStatus) // 正常
		{
			// 服务器信息
			
			Json::Value vData = jsonValue["data"];
			if (!vData.isNull())
			{
				int iDataCnt = vData.size();
				for (int indexData=0; indexData<iDataCnt; ++indexData)
				{
					T_NewStockInfo newStockInfo;
					newStockInfo.iNetPubTotal = vData[indexData]["netPubTotal"].asInt();

					string strStockCode = vData[indexData]["code"].asString();
					newStockInfo.StrCode = _MultiChar2Unicode(strStockCode.c_str()).c_str();

					string strPurchaseCode = vData[indexData]["purchaseCode"].asString();
 					newStockInfo.StrPurchaseCode = _MultiChar2Unicode(strPurchaseCode.c_str()).c_str();
					
					newStockInfo.dwPurchaseDate = vData[indexData]["purchaseDate"].asLargestInt() / 1000;

					newStockInfo.fPubWinRate = vData[indexData]["pubWinRate"].asFloat();
					
					__int64 iListedData = vData[indexData]["listedDate"].asLargestInt() / 1000;
					if (iListedData < 0)		// 无效时间
					{
						newStockInfo.dwListedDate = 0;
					}
					else
					{
						newStockInfo.dwListedDate = vData[indexData]["listedDate"].asLargestInt() / 1000;
					}
					
					newStockInfo.fIndustryWinRate = vData[indexData]["industryWinRate"].asFloat();

					newStockInfo.iPubTotal = vData[indexData]["pubTotal"].asInt();

					newStockInfo.dwSignDate = vData[indexData]["signDate"].asLargestInt() / 1000;
					
					string strStockName = vData[indexData]["stockName"].asString();
					newStockInfo.StrStockName = _MultiChar2Unicode(strStockName.c_str()).c_str();

					newStockInfo.fPubPrice = vData[indexData]["pubPrice"].asFloat();

					newStockInfo.fPurchaseLimit = vData[indexData]["purchaseLimit"].asFloat();

					newStockInfo.fFirstDayClosePrice = vData[indexData]["firstDayClosePrice"].asFloat();

					arrayNewStock.push_back(newStockInfo);
				}
			}
		}
	}
}

NewStockArray& CNewStockManager::GetNewStockInfo(bool32 bListedStock/* = true*/)
{
	return (bListedStock ? m_aListedStock : m_aUnlistedStock);
}
