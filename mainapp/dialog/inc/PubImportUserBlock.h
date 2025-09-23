#ifndef _PUB_IMPORT_USER_BLOCK_H
#define _PUB_IMPORT_USER_BLOCK_H
#include "stdafx.h"
#include <vector>
#include <map>


enum E_UserBlock_SoftType			// ��ѡ�������Դ
{
	ZXG_TDX_SOFT = 0,				// ͨ����
	ZXG_THS_SOFT,					// ͬ��˳
	ZXG_DZH_SOFT,					// ���ǻ�
	ZXG_Count
};


// ���ڿ����ӣ���ʱֻ֧����֤���۹�
enum E_UserBlock_Breed				// ��ƱƷ������
{
	ZXG_BREED_SH = 0,				// �Ϻ�֤ȯ
	ZXG_BREED_SZ,					// ����֤ȯ
	ZXG_BREED_XG,					// ���֤ȯ
	ZXG_BREED_Count 				// Ԥ��λ�ã�����չ
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

	// ���ز�����  
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
// ��Ʒ������Ϣ 
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
	vector<CUserBlockData>		m_vUserBlockData;				// �˻������ظ�,���ݶ���һ�𣬲����ڷŲ���
	vector<ST_SEARCH_REG_DATA>  m_vSearchRegData;

private:
	CString GetTdxUserBlockPath(); 
	CString GetDzhUserBlockPath();
	CString GetThsUserBlockPath();


	BOOL	SearchSoftRegKey(E_UserBlock_SoftType eSoftType, E_REG_SYSTEM_TYPE eSystemType);
	void	FindUserBlockFile(CString szSoftPath, CString szFileEXT);
	bool    AnalysisUserBlockFile(CString szSoftPath, CString szBlockPath);

	// ������ѡ���ļ�
	void UserBlockAnalysisiMerch(CString strUserBlockPath, E_UserBlock_SoftType eImPortType, CString strUser);

	// �������ǻ��ļ�
	void DzhUserBlockAnalysisiMerch(CString strUserBlockPath, E_UserBlock_SoftType eImPortType, CString strUser);

private:
	CArray<CString, CString>	m_aBreedName;
	CString						m_CurSoftPath;					// ��ǰ���������·��
	CString						m_CurSoftName;					// ��ǰ��˾������
	E_UserBlock_SoftType		m_eUserBlockSoftType;				// �������
};

#endif