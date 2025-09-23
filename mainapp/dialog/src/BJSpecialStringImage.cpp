#include "StdAfx.h"
#include "BJSpecialStringImage.h"
#include "XmlShare.h"
#include "PathFactory.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
static const TCHAR KStrBeiJingStringSection[] = _T("BeiJingString");
static const TCHAR KStrBeiJingImageSection[] = _T("BeiJingImage");

CBJSpecialStringImage & CBJSpecialStringImage::Instance()
{
	static CBJSpecialStringImage sObj;
	return sObj;
}

void CBJSpecialStringImage::Init()
{
#define PutMap_XL(m, k, v) {CString StrTmp;\
							CEtcXmlConfig::Instance().ReadEtcConfig(KStrBeiJingStringSection, v, v, StrTmp);\
							m[k] = StrTmp;}
#define PutMap_XL2(m, k, v, dv) {CString StrTmp;\
	CEtcXmlConfig::Instance().ReadEtcConfig(KStrBeiJingStringSection, v, dv, StrTmp);\
							m[k] = StrTmp;}
#define PutMap_XL_Image(m, k, v) {CString StrTmp;\
	CEtcXmlConfig::Instance().ReadEtcConfig(KStrBeiJingImageSection, v, v, StrTmp);\
							m[k] = StrTmp;}
	PutMap_XL_Image(m_mapId2String, ToolBar_DPCD, _T("大盘抄底"));
	PutMap_XL_Image(m_mapId2String, ToolBar_DPTD, _T("大盘逃顶"));
	PutMap_XL_Image(m_mapId2String, ToolBar_NXFJ, _T("牛熊分界"));
	PutMap_XL_Image(m_mapId2String, ToolBar_DXCP, _T("短线操盘"));
	PutMap_XL_Image(m_mapId2String, ToolBar_ZXCP, _T("中线操盘"));
	PutMap_XL_Image(m_mapId2String, ToolBar_ZJLX, _T("资金流向"));
	PutMap_XL_Image(m_mapId2String, ToolBar_DTXG, _T("动态选股"));
	PutMap_XL_Image(m_mapId2String, ToolBar_DDPH, _T("大单排行"));
	PutMap_XL_Image(m_mapId2String, ToolBar_RDJJ, _T("热点掘金"));
	PutMap_XL_Image(m_mapId2String, ToolBar_RMQS, _T("热门强势"));
	PutMap_XL_Image(m_mapId2String, ToolBar_YJSD, _T("一箭三雕"));

	
	PutMap_XL(m_mapId2String, String_DPZLCB, _T("大盘主力成本"));
	PutMap_XL(m_mapId2String, String_DPCWTX, _T("大盘仓位提醒"));
	PutMap_XL(m_mapId2String, String_GGZLCB, _T("个股主力成本"));
	PutMap_XL(m_mapId2String, String_GGCWTX, _T("个股仓位提醒"));
	PutMap_XL(m_mapId2String, String_JGLT, _T("机构拉抬"));
	PutMap_XL(m_mapId2String, String_XPJS, _T("洗盘结束"));
	PutMap_XL(m_mapId2String, String_ZJZT, _T("追击涨停"));
	PutMap_XL(m_mapId2String, String_NSQL, _T("逆势强龙"));
	PutMap_XL(m_mapId2String, String_CDFT, _T("超跌反弹"));

	PutMap_XL(m_mapId2String, String_DPCL, _T("大盘策略"));
	PutMap_XL(m_mapId2String, String_DPHSSSCL, _T("大盘红色上升策略"));
	PutMap_XL(m_mapId2String, String_DPLSTZCL, _T("大盘绿色调整策略"));
	PutMap_XL(m_mapId2String, String_DPLSXDCL, _T("大盘蓝色下跌策略"));
	PutMap_XL(m_mapId2String, String_DPHSZDCL, _T("大盘黄色震荡策略"));
	PutMap_XL(m_mapId2String, String_DPBDHQCL, _T("大盘暴跌行情策略"));

	PutMap_XL(m_mapId2String, String_DDQSG, _T("大单强势股"));
	PutMap_XL(m_mapId2String, String_RMQSG, _T("热门强势股"));
	PutMap_XL(m_mapId2String, String_ZJQSG, _T("资金强势股"));

	PutMap_XL2(m_mapId2String, String_DDPH_DDPH, _T("大单排行2"), _T("大单排行"));
	PutMap_XL2(m_mapId2String, String_DDPH_DDTJ, _T("大单统计2"), _T("大单统计"));

	PutMap_XL(m_mapId2String, String_DPCD, _T("大盘抄底"));
	PutMap_XL(m_mapId2String, String_DPTD, _T("大盘逃顶"));
	PutMap_XL(m_mapId2String, String_NXFJ, _T("牛熊分界"));
	PutMap_XL(m_mapId2String, String_DXCP, _T("短线操盘"));
	PutMap_XL(m_mapId2String, String_ZXCP, _T("中线操盘"));
	PutMap_XL(m_mapId2String, String_GGCP, _T("个股操盘"));
	PutMap_XL(m_mapId2String, String_ZJLX, _T("资金流向"));
	PutMap_XL(m_mapId2String, String_DXXG, _T("短线选股"));
	PutMap_XL(m_mapId2String, String_ZXXG, _T("中线选股"));
	PutMap_XL(m_mapId2String, String_MRJX, _T("买入精选"));
	PutMap_XL(m_mapId2String, String_ZLXG, _T("战略选股"));
	PutMap_XL(m_mapId2String, String_DDPH, _T("大单排行"));
	PutMap_XL(m_mapId2String, String_RDWJ, _T("热点挖掘"));
	PutMap_XL(m_mapId2String, String_ZLJK, _T("主力监控"));
	PutMap_XL(m_mapId2String, String_CPTX, _T("操盘提醒"));
	PutMap_XL(m_mapId2String, String_CWFX, _T("仓位风险"));
	PutMap_XL(m_mapId2String, String_ZDYC, _T("涨跌预测"));
	PutMap_XL(m_mapId2String, String_ZLCB, _T("主力成本"));
	PutMap_XL(m_mapId2String, String_GGCD, _T("个股抄底"));
	PutMap_XL(m_mapId2String, String_GGTD, _T("个股逃顶"));
	PutMap_XL(m_mapId2String, String_NXQR, _T("牛熊强弱"));
	PutMap_XL(m_mapId2String, String_XGXH, _T("选股信号"));
	PutMap_XL(m_mapId2String, String_QSJX, _T("强势精选"));

	PutMap_XL2(m_mapId2String, String_DXXG_D, _T("短线选股多"), _T("短线选股(多)"));
	PutMap_XL2(m_mapId2String, String_ZXXG_D, _T("中线选股多"), _T("中线选股(多)"));
	PutMap_XL(m_mapId2String, String_ZLCD, _T("战略抄底"));
	PutMap_XL(m_mapId2String, String_BDCD, _T("波段抄底"));
	//PutMap_XL(m_mapId2String, String_CDFT, _T("超跌反弹")); // 以前有，重复
	PutMap_XL2(m_mapId2String, String_JDXG_D, _T("金盾选股多"), _T("金盾选股(多)"));

	PutMap_XL2(m_mapId2String, String_DXXG_K, _T("短线选股空"), _T("短线选股(空)"));
	PutMap_XL2(m_mapId2String, String_ZXXG_K, _T("中线选股空"), _T("中线选股(空)"));
	PutMap_XL(m_mapId2String, String_ZLZK_K, _T("战略做空"));
	PutMap_XL(m_mapId2String, String_BDZK_K, _T("波段做空"));
	PutMap_XL2(m_mapId2String, String_CMHT_K, _T("超买回调空"), _T("超买回调(空)"));
	PutMap_XL2(m_mapId2String, String_JDXG_K, _T("金盾选股空"), _T("金盾选股(空)"));

	PutMap_XL(m_mapId2String, String_WebSiteBtn, _T("网站"));
	PutMap_XL(m_mapId2String, String_TradeBtn, _T("交易"));
	PutMap_XL(m_mapId2String, String_TradeMode, _T("交易模式"));

#undef PutMap_XL
#undef PutMap_XL2
#undef PutMap_XL_Image
}

CString CBJSpecialStringImage::GetSpecialString( const LPCTSTR lpszDefault )
{
	CString StrTmp;
	if ( NULL != lpszDefault )
	{
		CEtcXmlConfig::Instance().ReadEtcConfig(KStrBeiJingStringSection, lpszDefault, lpszDefault, StrTmp);
	}
	return StrTmp;
}

CString CBJSpecialStringImage::GetSpecialString( E_Id eId )
{
	Id2StringMap::iterator it = m_mapId2String.find(eId);
	if ( it != m_mapId2String.end() )
	{
		return it->second;
	}
	return CString();
}

Image * CBJSpecialStringImage::LoadSpecailImage( E_Id eImage )
{
	Id2StringMap::iterator it = m_mapId2String.find(eImage);
	if ( it != m_mapId2String.end() )
	{
		CString StrImagePath = CPathFactory::GetImagePath();
		StrImagePath += it->second;
		StrImagePath += _T(".png");		// image/xx.png
		Image *pImage = Image::FromFile(StrImagePath);	// 从文件中加载
		if ( NULL != pImage && pImage->GetLastStatus() != Ok )
		{
			delete pImage;	// 可以del吗
			pImage = NULL;
		}
		return pImage;
	}
	return NULL;
}

void CBJSpecialStringImage::GetMenuId2StringMap( OUT Id2StringMap &mapId2String )
{
	mapId2String = m_mapMenuId2String;
}

void CBJSpecialStringImage::SetMenuId2StringMap( const Id2StringMap &mapId2String )
{
	m_mapMenuId2String = mapId2String;
}
