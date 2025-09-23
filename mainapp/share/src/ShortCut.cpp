#include "StdAfx.h"



#include "ShortCut.h"
#include "DlgReportRank.h"
#include "DlgNotePad.h"
#include "BlockConfig.h"
#include "DlgTip.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// 所有的快捷键都在这里签名
static const T_ShortCutObject s_astShortCutObjects[] = 
{
	T_ShortCutObject(L"01",		VK_F1,			OnShortCut01,		L"分笔成交明细"),
	T_ShortCutObject(L"02",		VK_F2,			OnShortCut02,		L"分价表"),
	T_ShortCutObject(L"03",		VK_F3,			OnShortCut03,		L"上证指数"),
	T_ShortCutObject(L"04",		VK_F4,			OnShortCut04,		L"深证成指"),
	T_ShortCutObject(L"05",		VK_F5,			OnShortCut05,		L"图表类型切换"),
	T_ShortCutObject(L"06",		VK_F6,			OnShortCut06,		L"自选"),
	T_ShortCutObject(L"07",		VK_F7,			OnShortCut07,		L"子视图最大化"),
	T_ShortCutObject(L"08",		VK_F8,			OnShortCut08,		L"切换周期"),
	T_ShortCutObject(L"10",		VK_F10,			OnShortCut10,		L"背景资料"),
	T_ShortCutObject(L"12",		VK_F12,			OnShortCut12,		L"快手下单"),

	T_ShortCutObject(L"1",		-1,				OnShortCut1,		L"上证Ａ股"),
	T_ShortCutObject(L"2",		-1,				OnShortCut2,		L"上证Ｂ股"),
	T_ShortCutObject(L"3",		-1,				OnShortCut3,		L"深证Ａ股"),
	T_ShortCutObject(L"4",		-1,				OnShortCut4,		L"深证Ｂ股"),
	T_ShortCutObject(L"5",		-1,				OnShortCut5,		L"上证债券"),
	T_ShortCutObject(L"6",		-1,				OnShortCut6,		L"深证债券"),
	T_ShortCutObject(L"7",		-1,				OnShortCut7,		L"沪深Ａ股"),
	T_ShortCutObject(L"8",		-1,				OnShortCut8,		L"沪深Ｂ股"),
	T_ShortCutObject(L"9",		-1,				OnShortCut9,		L"中小企业"),

	T_ShortCutObject(L"11",		-1,				OnShortCut11,		L"沪深权证"),
//	T_ShortCutObject(L"12",		-1,				OnShortCut12,		L"创业板"),

//	T_ShortCutObject(L"16",		-1,				OnShortCut16,		L"信息地雷"),
//	T_ShortCutObject(L"17",		-1,				OnShortCut17,		L"投资日记"),

//	T_ShortCutObject(L"36",		-1,				OnShortCut36,		L"重股持仓基金"),
	
	T_ShortCutObject(L"60",		-1,				OnShortCut60,		L"沪深A股涨幅排行"),
	T_ShortCutObject(L"61",		-1,				OnShortCut61,		L"上证A股涨幅排行"),
	T_ShortCutObject(L"62",		-1,				OnShortCut62,		L"上证B股涨幅排行"),
	T_ShortCutObject(L"63",		-1,				OnShortCut63,		L"深证A股涨幅排行"),
	T_ShortCutObject(L"64",		-1,				OnShortCut64,		L"深证B股涨幅排行"),
	T_ShortCutObject(L"65",		-1,				OnShortCut65,		L"上证债券涨幅排行"),
	T_ShortCutObject(L"66",		-1,				OnShortCut66,		L"深证债券涨幅排行"),
	T_ShortCutObject(L"67",		-1,				OnShortCut67,		L"沪深A股涨幅排行"),
	//T_ShortCutObject(L"68",		-1,				OnShortCut68,		L"香港创业板涨幅排行"),
	T_ShortCutObject(L"68",		-1,				OnShortCut68,		L"沪深B涨幅排行"),
	//T_ShortCutObject(L"69",		-1,				OnShortCut69,		L"国企H股涨幅排行"),
	T_ShortCutObject(L"69",		-1,				OnShortCut69,		L"中小企业涨幅排行"),

	T_ShortCutObject(L"611",		-1,			OnShortCut611,		L"深沪权证涨幅排行"),
	T_ShortCutObject(L"612",		-1,			OnShortCut612,		L"创业板涨幅排行"),

	T_ShortCutObject(L"71",		-1,				OnShortCut71,		L"上交所公告"),
	T_ShortCutObject(L"72",		-1,				OnShortCut72,		L"深交所公告"),
	T_ShortCutObject(L"73",		-1,				OnShortCut73,		L"港交所公告"),
	T_ShortCutObject(L"74",		-1,				OnShortCut74,		L"即时新闻"),


	T_ShortCutObject(L"80",		-1,				OnShortCut80,		L"深沪A综合排名"),
	T_ShortCutObject(L"81",		-1,				OnShortCut81,		L"沪A综合排名"),
	T_ShortCutObject(L"82",		-1,				OnShortCut82,		L"沪B综合排名"),
	T_ShortCutObject(L"83",		-1,				OnShortCut83,		L"深A综合排名"),
	T_ShortCutObject(L"84",		-1,				OnShortCut84,		L"深B综合排名"),
	T_ShortCutObject(L"85",		-1,				OnShortCut85,		L"沪债综合排名"),
	T_ShortCutObject(L"86",		-1,				OnShortCut86,		L"深债综合排名"),
	T_ShortCutObject(L"88",		-1,				OnShortCut88,		L"深沪B综合排名"),
	T_ShortCutObject(L"89",		-1,				OnShortCut89,		L"中小企业综合排名"),
	T_ShortCutObject(L"811",	-1,				OnShortCut811,		L"深沪权证综合排名"),
	T_ShortCutObject(L"812",	-1,				OnShortCut812,		L"创业板综合排名"),
};

static const int32 KShortCutObjectCount = sizeof(s_astShortCutObjects) / sizeof(T_ShortCutObject);



//----------------------------------------------------------
const T_ShortCutObject* FindShortCutObjectByKey(const CString &StrKey)
{
	for (int32 i = 0; i < KShortCutObjectCount; i++)
	{
		if (s_astShortCutObjects[i].m_StrKey == StrKey)
		{
			return &s_astShortCutObjects[i];
		}
	}

	return NULL;
}

const T_ShortCutObject* FindShortCutObjectByVirtualKeyCode(int32 iVirtualKeyCode)
{
	for (int32 i = 0; i < KShortCutObjectCount; i++)
	{
		if (s_astShortCutObjects[i].m_iVirtualKeyCode == iVirtualKeyCode)
		{
			return &s_astShortCutObjects[i];
		}
	}

	return NULL;
}

const T_ShortCutObject* GetShortCutObject(int32 iIndex)
{
	ASSERT(iIndex >= 0 && iIndex < KShortCutObjectCount);

	return &s_astShortCutObjects[iIndex];
}

const int32 GetShortCutObjectCount()
{
	return KShortCutObjectCount;
}



void OnShortCut02()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// 香港恒生指数
	//pMainFrame->OnHotKeyMerch(2001, L"hsi");

	// 分价表
	pMainFrame->OnShowF7IoViewInChart(ID_PIC_FENJIABIAO);
}


void OnShortCut03()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// 上证指数
	//pMainFrame->OnHotKeyMerch(0, L"000001");
	pMainFrame->OnShowMerchInChart(0, _T("000001"), ID_PIC_TREND);
}

void OnShortCut04()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// 深证指数
	//pMainFrame->OnHotKeyMerch(1000, L"399001");
	pMainFrame->OnShowMerchInChart(1000, L"399001", ID_PIC_TREND);
}

void OnShortCut06()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// 自选股
	if ( pMainFrame && pMainFrame->m_pNewWndTB )
	{
		pMainFrame->m_pNewWndTB->DoOpenCfm(L"自选股");
	}
}

void OnShortCut07()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// IoView最大化
	pMainFrame->OnProcessF7();

}
void OnShortCut10()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	pMainFrame->OnInfoF10();
}

void OnShortCut12()
{
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	
	CIoViewBase* p = pMainFrame->FindActiveIoView();
	if (NULL != p)
	{
		pMainFrame->OnQuickTrade(2, p->GetMerchXml());
	}	
}

void OnShortCut11()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// 深沪权证
	pMainFrame->OnSpecifyBlockIdAndFieldReport(2010, -1, true);
}

void OnShortCut60()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// 深沪A涨幅
	pMainFrame->OnSpecifyBlockIdAndFieldReport(2000, EMRFRisePercent, true);
}


void OnShortCut61()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// 上证A股涨幅排序
	pMainFrame->OnSpecifyMarketAndFieldReport(1, EMRFRisePercent, true);
}

void OnShortCut62()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// 上证B股涨幅排序
	pMainFrame->OnSpecifyMarketAndFieldReport(2, EMRFRisePercent, true);
}

void OnShortCut63()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// 深证A股涨幅排序
	pMainFrame->OnSpecifyMarketAndFieldReport(1001, EMRFRisePercent, true);
}

void OnShortCut64()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// 深证B股涨幅排序
	pMainFrame->OnSpecifyMarketAndFieldReport(1002, EMRFRisePercent, true);
}

void OnShortCut65()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// 上证债券涨幅排序
	pMainFrame->OnSpecifyMarketAndFieldReport(5, EMRFRisePercent, true);
}

void OnShortCut66()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// 深证债券涨幅排序
	pMainFrame->OnSpecifyMarketAndFieldReport(1006, EMRFRisePercent, true);
}

void OnShortCut67()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// 香港主板涨幅排序
	//pMainFrame->OnSpecifyMarketAndFieldReport(2002, EMRFRisePercent, true);

	// 深沪A涨幅
	pMainFrame->OnSpecifyBlockIdAndFieldReport(2000, EMRFRisePercent, true);
}

void OnShortCut68()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// 香港主板涨幅排序
	//pMainFrame->OnSpecifyMarketAndFieldReport(2003, EMRFRisePercent, true);

	// 深沪B涨幅
	pMainFrame->OnSpecifyBlockIdAndFieldReport(2007, EMRFRisePercent, true);
}

void OnShortCut69()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// 香港主板涨幅排序
	//pMainFrame->OnSpecifyMarketAndFieldReport(2004, EMRFRisePercent, true);

	// 中小企业涨幅
	pMainFrame->OnSpecifyMarketAndFieldReport(1004, EMRFRisePercent, true);
}

void OnShortCut71()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// 上交所公告
	pMainFrame->OnInfoShBourse();
}

void OnShortCut72()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// 深交所公告
	pMainFrame->OnInfoSzBourse();
}

void OnShortCut73()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// 港交所公告
	pMainFrame->OnInfoHkBourse();
}

void OnShortCut74()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// 即时新闻
	pMainFrame->OnInfoNews();
}

void OnShortCut611()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// 深沪权证涨幅
	pMainFrame->OnSpecifyBlockIdAndFieldReport(2010, EMRFRisePercent, true);
}

void OnShortCut612()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// 创业板涨幅
	pMainFrame->OnSpecifyMarketAndFieldReport(1008, EMRFRisePercent, true);
}

void OnShortCut80()
{
	CDlgReportRank::ShowPopupReportRank(CBlockConfig::GetDefaultMarketlClassBlockPseudoId());	// 上证A股
}

void OnShortCut81()
{
	CDlgReportRank::ShowPopupReportRank(2003);	// 上证A股
}

void OnShortCut82()
{
	CDlgReportRank::ShowPopupReportRank(2001);	// 上证B股
}

void OnShortCut83()
{
	CDlgReportRank::ShowPopupReportRank(2004);	// 深证A
}

void OnShortCut84()
{
	CDlgReportRank::ShowPopupReportRank(2012);	// 深证B
}

void OnShortCut85()
{
	CDlgReportRank::ShowPopupReportRank(2018);	// 上证债券
}

void OnShortCut86()
{
	CDlgReportRank::ShowPopupReportRank(2018);	// 上证债券
}

void OnShortCut88()
{
	CDlgReportRank::ShowPopupReportRank(2007);	//  深沪B
}

void OnShortCut89()
{
	CDlgReportRank::ShowPopupReportRank(2005);	// 中小企业
}

void OnShortCut811()
{
	CDlgReportRank::ShowPopupReportRank(2010);	// 深沪权证
}

void OnShortCut812()
{
	CDlgReportRank::ShowPopupReportRank(2006);	// 创业板
}

void OnShortCut01()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// 创业板涨幅
	pMainFrame->OnShowTimeSaleDetail();
}
void OnShortCut16()
{
	CDlgNotePad::ShowNotePad(CDlgNotePad::EST_InfoMine);
}


void OnShortCut17()
{
	// 投资日记
	CDlgNotePad::ShowNotePad(CDlgNotePad::EST_UserNote);
}

void OnShortCut1()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// 证A股、上证B股、深圳A股、深圳B股、上证债券、深圳债券、沪深A股、沪深B股、中小板  
	pMainFrame->OnSpecifyMarketAndFieldReport(1, -1, true);
}

void OnShortCut2()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// 上证B
	pMainFrame->OnSpecifyMarketAndFieldReport(2, -1, true);
}

void OnShortCut3()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// 深证A
	pMainFrame->OnSpecifyMarketAndFieldReport(1001, -1, true);
}

void OnShortCut4()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// 深证B
	pMainFrame->OnSpecifyMarketAndFieldReport(1002, -1, true);
}

void OnShortCut5()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// 上证债券
	pMainFrame->OnSpecifyMarketAndFieldReport(5, -1, true);
}

void OnShortCut6()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// 深证债券
	pMainFrame->OnSpecifyMarketAndFieldReport(1006, -1, true);
}

void OnShortCut7()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// 深沪A
	pMainFrame->OnSpecifyBlockIdAndFieldReport(2000, -1, true);
}

void OnShortCut8()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// 深沪B
	pMainFrame->OnSpecifyBlockIdAndFieldReport(2007, -1, true);
}

void OnShortCut9()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// 中小企业
	pMainFrame->OnSpecifyMarketAndFieldReport(1004, -1, true);
}

void OnShortCut05()
{
	// 分/k切换
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	pMainFrame->OnShowMerchInNextChart(NULL);
}

void OnShortCut08()
{
	// k线周期切换
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	pMainFrame->ChangeIoViewKlineTimeInterval(-1);
}

void OnShortCut36()
{
	// 重股持仓基金
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	pMainFrame->PostMessage(WM_COMMAND, ID_ZHONGCANGCHIGU, 0);
}



