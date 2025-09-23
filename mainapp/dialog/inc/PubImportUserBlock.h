#ifndef _PUB_IMPORT_USER_BLOCK_H
#define _PUB_IMPORT_USER_BLOCK_H
#include "stdafx.h"
#include <vector>
#include <map>


enum E_UserBlock_SoftType			// 自选股软件来源
{
	ZXG_TDX_SOFT = 0,				// 通达信
	ZXG_THS_SOFT,					// 同花顺
	ZXG_DZH_SOFT,					// 大智慧
	ZXG_Count
};


// 后期可增加，暂时只支持上证，港股
enum E_UserBlock_Breed				// 股票品种类型
{
	ZXG_BREED_SH = 0,				// 上海证券
	ZXG_BREED_SZ,					// 深圳证券
	ZXG_BREED_XG,					// 香港证券
	ZXG_BREED_Count 				// 预留位置，可扩展
};

enum E_REG_SYSTEM_TYPE				
{
	REG_SYSTEM_32 = 0,	
	REG_SYSTEM_64				
};




typedef struct tagMerchInfo
{
	CString strMerchCode;
	CString strMerchChName;

	// 重载操作符  
	bool operator<(const tagMerchInfo& rhs) { return (strMerchCode+strMerchChName) < (rhs.strMerchCode+rhs.strMerchChName);};  
	bool operator>(const tagMerchInfo& rhs) { return (strMerchCode+strMerchChName) > (rhs.strMerchCode+rhs.strMerchChName);};  
	bool operator==(const tagMerchInfo& rhs) { return (strMerchCode+strMerchChName) == (rhs.strMerchCode+rhs.strMerchChName);}  

} ST_MerchInfo;


typedef struct tagSearchRegData
{
	CString strRunPath;
	CString strAppPath;
	CString strDisplayName;
} ST_SEARCH_REG_DATA;


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 商品数据信息 
class  CUserBlockData
{
public:
	CUserBlockData(){ m_vMerchInfo.clear();};
	~CUserBlockData(){};
public:
	CString m_strUser;
	CString m_strSoftName;
	vector<ST_MerchInfo> m_vMerchInfo;
};


class  CPubImportUserBlockFun
{
public:
	CPubImportUserBlockFun();
	~CPubImportUserBlockFun();

public:
	bool AnalysisTdxUserBlockFile();
	bool AnalysisDzhUserBlockFile();
	bool AnalysisThsUserBlockFile();


public:
	vector<CUserBlockData>		m_vUserBlockData;				// 账户不会重复,数据都放一起，不至于放不下
	vector<ST_SEARCH_REG_DATA>  m_vSearchRegData;

private:
	CString GetTdxUserBlockPath(); 
	CString GetDzhUserBlockPath();
	CString GetThsUserBlockPath();


	BOOL	SearchSoftRegKey(E_UserBlock_SoftType eSoftType, E_REG_SYSTEM_TYPE eSystemType);
	void	FindUserBlockFile(CString szSoftPath, CString szFileEXT);
	bool    AnalysisUserBlockFile(CString szSoftPath, CString szBlockPath);

	// 解析自选股文件
	void UserBlockAnalysisiMerch(CString strUserBlockPath, E_UserBlock_SoftType eImPortType, CString strUser);

	// 解析大智慧文件
	void DzhUserBlockAnalysisiMerch(CString strUserBlockPath, E_UserBlock_SoftType eImPortType, CString strUser);

private:
	CArray<CString, CString>	m_aBreedName;
	CString						m_CurSoftPath;					// 当前解析的软件路径
	CString						m_CurSoftName;					// 当前公司的名称
	E_UserBlock_SoftType		m_eUserBlockSoftType;				// 软件类型
};

#endif