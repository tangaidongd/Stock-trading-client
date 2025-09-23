#ifndef _PLUGIN_FUNC_RIGHT_H_
#define _PLUGIN_FUNC_RIGHT_H_


#include "typedef.h"
#include <map>
using std::map;
#include <vector>
using std::vector;

#include "DataCenterExport.h"

/////////////////////////////////////////////////////////////
typedef struct _UserRightInfo
{
	_UserRightInfo()
	{
		bRight		 = false;
		iRightCode	 = -1;
		StrRightName = _T("");
	}

	bool32		bRight;			// �Ƿ���Ȩ��
	int32		iRightCode;		// Ȩ�޺�
	CString		StrRightName;	// Ȩ������
}T_UserRightInfo;

typedef vector<T_UserRightInfo> UserRightInfoArray;

//////////////////////////////////////////////////////////////////
typedef struct _RightPromptCode 
{
	_RightPromptCode()
	{
		iRightCode	 = -1;
		StrTitle	 = _T("");
		StrImgUrl	 = _T("");
		StrLinkUrl	 = _T("");
		StrMsgTip	 = _T("");
	}

	int32	iRightCode;		// Ȩ�޺�
	CString StrTitle;		// ��ʾ����
	CString StrImgUrl;		// ͼƬ��ַ
	CString StrLinkUrl;		// �������
	CString StrMsgTip;		// ���û���ҵ���ӦȨ�޵���ʾ����
}T_RightPromptCode;

typedef vector<T_RightPromptCode> RightPromptCodeArray;

////////////////////////////////////////////////////////////////////

class DATACENTER_DLL_EXPORT CPluginFuncRight
{
public:
	
	enum E_FuncRight				// ���ܺ�����
	{
		FuncStart = 0,
		FuncIndexAdviseBuy = 1,		// 001 ���̳��� -> ����ս�Գ���
		FuncIndexAdviseSell,		// 002 �����Ӷ� -> ����ս���Ӷ�
		FuncRiseFallView,			// 003 ţ�ֽܷ� -> ţ�ֲܷ�
		FuncShortTrade,				// 004 ���߲��� -> ���߲���
		FuncMidTrade,				// 005 ���߲��� -> ���߲���
		FuncCapitalFlow,			// 006 �ʽ����� -> �����ʽ�
		FuncShortSelectStock,		// 007 ����ѡ��
		FuncMidSelectStock,			// 008 ����ѡ��
		FuncBuySelectStock,			// 009 ���뾫ѡ
		FuncStrategySelectStock,	// 010 ս��ѡ��
		FuncTimeSaleRank,			// 011 ������ -> ��ͳ��
		FuncDaDanQiangShi,			// 012 ��ǿ��
		FuncReMenQiangShi,			// 013 ����ǿ��
		FuncZiJinQiangShi,			// 014 �ʽ�ǿ��
		FuncJiGouLaTai,				// 015 ������̧
		FuncXiPanJieShu,			// 016 ϴ�̽���
		FuncZhuiJiZhangTing,		// 017 ׷����ͣ
		FuncNiShiQiangLong,			// 018 ����ǿ��
		FuncChaoDieFanTan,			// 019 ��������
		FuncShenLongXiShui,         // 020 ������ˮ
		FuncHuiFengFuLiu,           // 021 �ط����
		FuncSanHuaJuDing,           // 022 �����۶�

		// id 100���Ͻ������ά��̬�����ָ��&ҳ��Ȩ��

		FuncEnd = 0x7ffff,
	};

	enum E_ShowRightType
	{
		ESRT_MessageBox = 0,
		ESRT_IE,
	};

	static CPluginFuncRight	&Instance();

	// ���õ�����ʾ��ĸ����ھ��
	void	SetShowTipWnd(HWND hWnd, int iUserGroupType);

	// �����֤���ص���Ȩ�޵Ĺ����б�
	void	AddUserRight(const T_UserRightInfo& stUserRight);	

	// ���Ȩ����ʾ�б�
	void	AddRightTips(const T_RightPromptCode& stRightTip);

	// ����Ȩ�޺Ż�ȡȨ������
	CString	GetUserRightName(int32 iRightCode);	

	// ����Ȩ�����ƻ�ȡȨ�޺�
	int32	GetUserRightCode(CString StrRightName);

	// ����Ȩ�޺ż���û��Ƿ�߱���Ȩ��
	bool32	IsUserHasRight(int32 iRightCode, bool32 bShowTip = false);		

	// ����Ȩ�����Ƽ���û��Ƿ�߱���Ȩ��
	bool32	IsUserHasRight(CString StrRightName, bool32 bShowTip = false, bool32 bAddUserDealRecord=false);		

	// �û��Ƿ�ӵ��ָ��Ȩ��, ָ������Ϊ�û���������
	bool32	IsUserHasIndexRight(CString StrIndexName, bool32 bShowTip = false);

	// �û��Ƿ�߱����ָ��Ȩ��
	bool32	IsUserHasSaneIndexRight(bool32 bShowTip = false);	

	// �Ƹ������ʾ����
	static CString	GetSaneIndexShowName();								

	// ��ʾ��ʾ�Ի��򣬴���Ȩ�޺�
	void	ShowPromptDialog(int32 iRightCode);

	// ���ָ��Ȩ��
	void	AddIndexRight(CString StrIndexName);

	//	�������
	void	Clear();
	//	���tip
	void	ClearRightTip();
	//	���Ȩ��
	void	ClearRight();
protected:
	CPluginFuncRight();
	~CPluginFuncRight();

private:
	UserRightInfoArray	 m_arrUserRights;	// �û�Ȩ���б�(���еİ�����Ȩ�޺���Ȩ��)
	RightPromptCodeArray m_arrRightTips;	// Ȩ����ʾ�б�

	vector<CString>		m_vecIndexName;		// �洢�����ù��ڵ�ָ������
	HWND				m_hWndShowTip;		// ������ʾ��ĸ����ھ��	
    int                 m_iUserGroupType;   // �û����� 1:�û� 2:������Ա
};

#endif //_PLUGIN_FUNC_RIGHT_H_