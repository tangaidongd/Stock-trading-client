#ifndef _BJSPECAILSTRINGIMAGE_H_
#define _BJSPECAILSTRINGIMAGE_H_
#include "GdiPlusTS.h"
#include <map>
using std::map;

// 北京项目特殊话的一些字串与Image文件在这里处理
class CBJSpecialStringImage
{
public:
	typedef map<DWORD, CString> Id2StringMap;
	enum E_Id
	{
		ToolBar_DPCD = 1,		// 大盘抄底
		ToolBar_DPTD,			// 大盘逃顶
		ToolBar_NXFJ,			// 牛熊分界
		ToolBar_DXCP,			// 短线操盘
		ToolBar_ZXCP,			// 中线操盘
		ToolBar_ZJLX,			// 资金流向
		ToolBar_YJSD,			// 一箭三雕
		ToolBar_DTXG,			// 动态选股
		ToolBar_RDJJ,			// 热点掘金
		ToolBar_DDPH,			// 大单排行
		ToolBar_RMQS,			// 热门强势 私募强龙


		String_Start = 100,
		String_DPZLCB,			// 大盘机构成本
		String_DPCWTX,			// 大盘仓位提醒
		String_GGZLCB,			// 个股机构成本
		String_GGCWTX,			// 个股仓位提醒
		String_JGLT,			// 机构拉抬
		String_XPJS,			// 洗盘结束
		String_ZJZT,			// 追击涨停
		String_NSQL,			// 逆市强龙
		String_CDFT,			// 超跌反弹

		String_DPCL,			// 大盘策略
		String_DPHSSSCL,		// 大盘红色上升策略
		String_DPLSTZCL,		// 大盘绿色调整策略
		String_DPLSXDCL,		// 大盘蓝色下跌策略
		String_DPHSZDCL,		// 大盘黄色震荡策略
		String_DPBDHQCL,		// 大盘暴跌行情策略

		String_DDQSG,			// 大单强势股
		String_RMQSG,			// 热门强势股
		String_ZJQSG,			// 资金强势股

		String_DDPH_DDTJ,		// 大单排行 - 大单统计
		String_DDPH_DDPH,		// 大单排行 - 大单排行

		String_ZLCP,			// 战略操盘
		String_ZQGZ,			// 周期共振
		String_ZLKP,			// 战略看盘

		String_DPCD,			// 大盘抄底
		String_DPTD,			// 大盘逃顶
		String_NXFJ,			// 牛熊分界
		String_DXCP,			// 短线操盘
		String_ZXCP,			// 中线操盘
		String_GGCP,			// 个股操盘
		String_ZJLX,			// 资金流向
		String_DXXG,			// 短线选股
		String_ZXXG,			// 中线选股
		String_MRJX,			// 买入精选
		String_ZLXG,			// 战略选股
		String_DDPH,			// 大单排行
		String_RDWJ,			// 热点挖掘
		String_ZLJK,			// 主力监控
		String_CPTX,			// 操盘提醒
		String_CWFX,			// 仓位风险
		String_ZDYC,			// 涨跌预测
		String_ZLCB,			// 主力成本
		String_GGCD,			// 个股抄底
		String_GGTD,			// 个股逃顶
		String_NXQR,			// 牛熊强弱
		String_XGXH,			// 选股信号
		String_QSJX,			// 强势精选

		String_DXXG_D,			// 短线选股多
		String_ZXXG_D,			// 中线选股多
		String_ZLCD,			// 战略抄底
		String_BDCD,			// 波段抄底
		//String_CDFT,			// 超跌反弹
		String_JDXG_D,			// 金盾选股多
		String_DXXG_K,			// 短线选股空
		String_ZXXG_K,			// 中线选股空
		String_ZLZK_K,			// 战略做空
		String_BDZK_K,			// 波段做空
		String_CMHT_K,			// 超买回调空
		String_JDXG_K,			// 金盾选股空

		String_WebSiteBtn,			// 网站地址按钮
		String_TradeBtn,			// 交易按钮
		String_TradeMode,			// 交易模式
	};

	static CBJSpecialStringImage &Instance();
	~CBJSpecialStringImage(){}

	CString	GetSpecialString(E_Id eId);		// 获取该自定义字串
	Image *LoadSpecailImage(E_Id eImage);	// 加载自定义的图片
	CString GetSpecialString(const LPCTSTR lpszDefault);	// 根据原来的字串获取现在自定义的字串

	void	GetMenuId2StringMap(OUT Id2StringMap &mapId2String);	// 获取特色菜单下的id到显示字串
	void	SetMenuId2StringMap(const Id2StringMap &mapId2String);	// 设置特色菜单下的id到显示字串

private:
	CBJSpecialStringImage(){ Init(); };
	CBJSpecialStringImage(const CBJSpecialStringImage &);
	CBJSpecialStringImage &operator=(const CBJSpecialStringImage &);

	void	Init();		// 初始化默认显示名称
	
	
	Id2StringMap	m_mapId2String;
	Id2StringMap	m_mapMenuId2String;
};

#endif //_BJSPECAILSTRINGIMAGE_H_