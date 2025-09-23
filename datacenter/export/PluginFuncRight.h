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

	bool32		bRight;			// 是否有权限
	int32		iRightCode;		// 权限号
	CString		StrRightName;	// 权限名称
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

	int32	iRightCode;		// 权限号
	CString StrTitle;		// 提示标题
	CString StrImgUrl;		// 图片地址
	CString StrLinkUrl;		// 点击链接
	CString StrMsgTip;		// 如果没有找到对应权限的提示内容
}T_RightPromptCode;

typedef vector<T_RightPromptCode> RightPromptCodeArray;

////////////////////////////////////////////////////////////////////

class DATACENTER_DLL_EXPORT CPluginFuncRight
{
public:
	
	enum E_FuncRight				// 功能号描述
	{
		FuncStart = 0,
		FuncIndexAdviseBuy = 1,		// 001 大盘抄底 -> 大盘战略抄底
		FuncIndexAdviseSell,		// 002 大盘逃顶 -> 大盘战略逃顶
		FuncRiseFallView,			// 003 牛熊分界 -> 牛熊分布
		FuncShortTrade,				// 004 短线操盘 -> 短线操作
		FuncMidTrade,				// 005 中线操盘 -> 中线操作
		FuncCapitalFlow,			// 006 资金流向 -> 主力资金
		FuncShortSelectStock,		// 007 短线选股
		FuncMidSelectStock,			// 008 中线选股
		FuncBuySelectStock,			// 009 买入精选
		FuncStrategySelectStock,	// 010 战略选股
		FuncTimeSaleRank,			// 011 大单排行 -> 大单统计
		FuncDaDanQiangShi,			// 012 大单强势
		FuncReMenQiangShi,			// 013 热门强势
		FuncZiJinQiangShi,			// 014 资金强势
		FuncJiGouLaTai,				// 015 机构拉抬
		FuncXiPanJieShu,			// 016 洗盘结束
		FuncZhuiJiZhangTing,		// 017 追击涨停
		FuncNiShiQiangLong,			// 018 逆势强龙
		FuncChaoDieFanTan,			// 019 超跌反弹
		FuncShenLongXiShui,         // 020 神龙吸水
		FuncHuiFengFuLiu,           // 021 回风拂柳
		FuncSanHuaJuDing,           // 022 三花聚顶

		// id 100以上建议给运维动态分配给指标&页面权限

		FuncEnd = 0x7ffff,
	};

	enum E_ShowRightType
	{
		ESRT_MessageBox = 0,
		ESRT_IE,
	};

	static CPluginFuncRight	&Instance();

	// 设置弹出提示框的父窗口句柄
	void	SetShowTipWnd(HWND hWnd, int iUserGroupType);

	// 添加认证返回的有权限的功能列表
	void	AddUserRight(const T_UserRightInfo& stUserRight);	

	// 添加权限提示列表
	void	AddRightTips(const T_RightPromptCode& stRightTip);

	// 根据权限号获取权限名称
	CString	GetUserRightName(int32 iRightCode);	

	// 根据权限名称获取权限号
	int32	GetUserRightCode(CString StrRightName);

	// 根据权限号检查用户是否具备该权限
	bool32	IsUserHasRight(int32 iRightCode, bool32 bShowTip = false);		

	// 根据权限名称检查用户是否具备该权限
	bool32	IsUserHasRight(CString StrRightName, bool32 bShowTip = false, bool32 bAddUserDealRecord=false);		

	// 用户是否拥有指标权限, 指标名称为用户定义名称
	bool32	IsUserHasIndexRight(CString StrIndexName, bool32 bShowTip = false);

	// 用户是否具备金盾指标权限
	bool32	IsUserHasSaneIndexRight(bool32 bShowTip = false);	

	// 财富金盾显示名称
	static CString	GetSaneIndexShowName();								

	// 显示提示对话框，传入权限号
	void	ShowPromptDialog(int32 iRightCode);

	// 添加指标权限
	void	AddIndexRight(CString StrIndexName);

	//	清空数据
	void	Clear();
	//	清空tip
	void	ClearRightTip();
	//	清空权限
	void	ClearRight();
protected:
	CPluginFuncRight();
	~CPluginFuncRight();

private:
	UserRightInfoArray	 m_arrUserRights;	// 用户权限列表(所有的包括有权限和无权限)
	RightPromptCodeArray m_arrRightTips;	// 权限提示列表

	vector<CString>		m_vecIndexName;		// 存储超试用过期的指标名称
	HWND				m_hWndShowTip;		// 弹出提示框的父窗口句柄	
    int                 m_iUserGroupType;   // 用户类型 1:用户 2:工作人员
};

#endif //_PLUGIN_FUNC_RIGHT_H_