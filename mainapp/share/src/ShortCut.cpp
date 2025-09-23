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
// ���еĿ�ݼ���������ǩ��
static const T_ShortCutObject s_astShortCutObjects[] = 
{
	T_ShortCutObject(L"01",		VK_F1,			OnShortCut01,		L"�ֱʳɽ���ϸ"),
	T_ShortCutObject(L"02",		VK_F2,			OnShortCut02,		L"�ּ۱�"),
	T_ShortCutObject(L"03",		VK_F3,			OnShortCut03,		L"��ָ֤��"),
	T_ShortCutObject(L"04",		VK_F4,			OnShortCut04,		L"��֤��ָ"),
	T_ShortCutObject(L"05",		VK_F5,			OnShortCut05,		L"ͼ�������л�"),
	T_ShortCutObject(L"06",		VK_F6,			OnShortCut06,		L"��ѡ"),
	T_ShortCutObject(L"07",		VK_F7,			OnShortCut07,		L"����ͼ���"),
	T_ShortCutObject(L"08",		VK_F8,			OnShortCut08,		L"�л�����"),
	T_ShortCutObject(L"10",		VK_F10,			OnShortCut10,		L"��������"),
	T_ShortCutObject(L"12",		VK_F12,			OnShortCut12,		L"�����µ�"),

	T_ShortCutObject(L"1",		-1,				OnShortCut1,		L"��֤����"),
	T_ShortCutObject(L"2",		-1,				OnShortCut2,		L"��֤�¹�"),
	T_ShortCutObject(L"3",		-1,				OnShortCut3,		L"��֤����"),
	T_ShortCutObject(L"4",		-1,				OnShortCut4,		L"��֤�¹�"),
	T_ShortCutObject(L"5",		-1,				OnShortCut5,		L"��֤ծȯ"),
	T_ShortCutObject(L"6",		-1,				OnShortCut6,		L"��֤ծȯ"),
	T_ShortCutObject(L"7",		-1,				OnShortCut7,		L"�������"),
	T_ShortCutObject(L"8",		-1,				OnShortCut8,		L"����¹�"),
	T_ShortCutObject(L"9",		-1,				OnShortCut9,		L"��С��ҵ"),

	T_ShortCutObject(L"11",		-1,				OnShortCut11,		L"����Ȩ֤"),
//	T_ShortCutObject(L"12",		-1,				OnShortCut12,		L"��ҵ��"),

//	T_ShortCutObject(L"16",		-1,				OnShortCut16,		L"��Ϣ����"),
//	T_ShortCutObject(L"17",		-1,				OnShortCut17,		L"Ͷ���ռ�"),

//	T_ShortCutObject(L"36",		-1,				OnShortCut36,		L"�عɳֲֻ���"),
	
	T_ShortCutObject(L"60",		-1,				OnShortCut60,		L"����A���Ƿ�����"),
	T_ShortCutObject(L"61",		-1,				OnShortCut61,		L"��֤A���Ƿ�����"),
	T_ShortCutObject(L"62",		-1,				OnShortCut62,		L"��֤B���Ƿ�����"),
	T_ShortCutObject(L"63",		-1,				OnShortCut63,		L"��֤A���Ƿ�����"),
	T_ShortCutObject(L"64",		-1,				OnShortCut64,		L"��֤B���Ƿ�����"),
	T_ShortCutObject(L"65",		-1,				OnShortCut65,		L"��֤ծȯ�Ƿ�����"),
	T_ShortCutObject(L"66",		-1,				OnShortCut66,		L"��֤ծȯ�Ƿ�����"),
	T_ShortCutObject(L"67",		-1,				OnShortCut67,		L"����A���Ƿ�����"),
	//T_ShortCutObject(L"68",		-1,				OnShortCut68,		L"��۴�ҵ���Ƿ�����"),
	T_ShortCutObject(L"68",		-1,				OnShortCut68,		L"����B�Ƿ�����"),
	//T_ShortCutObject(L"69",		-1,				OnShortCut69,		L"����H���Ƿ�����"),
	T_ShortCutObject(L"69",		-1,				OnShortCut69,		L"��С��ҵ�Ƿ�����"),

	T_ShortCutObject(L"611",		-1,			OnShortCut611,		L"�Ȩ֤�Ƿ�����"),
	T_ShortCutObject(L"612",		-1,			OnShortCut612,		L"��ҵ���Ƿ�����"),

	T_ShortCutObject(L"71",		-1,				OnShortCut71,		L"�Ͻ�������"),
	T_ShortCutObject(L"72",		-1,				OnShortCut72,		L"�������"),
	T_ShortCutObject(L"73",		-1,				OnShortCut73,		L"�۽�������"),
	T_ShortCutObject(L"74",		-1,				OnShortCut74,		L"��ʱ����"),


	T_ShortCutObject(L"80",		-1,				OnShortCut80,		L"�A�ۺ�����"),
	T_ShortCutObject(L"81",		-1,				OnShortCut81,		L"��A�ۺ�����"),
	T_ShortCutObject(L"82",		-1,				OnShortCut82,		L"��B�ۺ�����"),
	T_ShortCutObject(L"83",		-1,				OnShortCut83,		L"��A�ۺ�����"),
	T_ShortCutObject(L"84",		-1,				OnShortCut84,		L"��B�ۺ�����"),
	T_ShortCutObject(L"85",		-1,				OnShortCut85,		L"��ծ�ۺ�����"),
	T_ShortCutObject(L"86",		-1,				OnShortCut86,		L"��ծ�ۺ�����"),
	T_ShortCutObject(L"88",		-1,				OnShortCut88,		L"�B�ۺ�����"),
	T_ShortCutObject(L"89",		-1,				OnShortCut89,		L"��С��ҵ�ۺ�����"),
	T_ShortCutObject(L"811",	-1,				OnShortCut811,		L"�Ȩ֤�ۺ�����"),
	T_ShortCutObject(L"812",	-1,				OnShortCut812,		L"��ҵ���ۺ�����"),
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

	// ��ۺ���ָ��
	//pMainFrame->OnHotKeyMerch(2001, L"hsi");

	// �ּ۱�
	pMainFrame->OnShowF7IoViewInChart(ID_PIC_FENJIABIAO);
}


void OnShortCut03()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// ��ָ֤��
	//pMainFrame->OnHotKeyMerch(0, L"000001");
	pMainFrame->OnShowMerchInChart(0, _T("000001"), ID_PIC_TREND);
}

void OnShortCut04()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// ��ָ֤��
	//pMainFrame->OnHotKeyMerch(1000, L"399001");
	pMainFrame->OnShowMerchInChart(1000, L"399001", ID_PIC_TREND);
}

void OnShortCut06()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// ��ѡ��
	if ( pMainFrame && pMainFrame->m_pNewWndTB )
	{
		pMainFrame->m_pNewWndTB->DoOpenCfm(L"��ѡ��");
	}
}

void OnShortCut07()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// IoView���
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
	
	// �Ȩ֤
	pMainFrame->OnSpecifyBlockIdAndFieldReport(2010, -1, true);
}

void OnShortCut60()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// �A�Ƿ�
	pMainFrame->OnSpecifyBlockIdAndFieldReport(2000, EMRFRisePercent, true);
}


void OnShortCut61()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// ��֤A���Ƿ�����
	pMainFrame->OnSpecifyMarketAndFieldReport(1, EMRFRisePercent, true);
}

void OnShortCut62()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// ��֤B���Ƿ�����
	pMainFrame->OnSpecifyMarketAndFieldReport(2, EMRFRisePercent, true);
}

void OnShortCut63()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// ��֤A���Ƿ�����
	pMainFrame->OnSpecifyMarketAndFieldReport(1001, EMRFRisePercent, true);
}

void OnShortCut64()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// ��֤B���Ƿ�����
	pMainFrame->OnSpecifyMarketAndFieldReport(1002, EMRFRisePercent, true);
}

void OnShortCut65()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// ��֤ծȯ�Ƿ�����
	pMainFrame->OnSpecifyMarketAndFieldReport(5, EMRFRisePercent, true);
}

void OnShortCut66()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// ��֤ծȯ�Ƿ�����
	pMainFrame->OnSpecifyMarketAndFieldReport(1006, EMRFRisePercent, true);
}

void OnShortCut67()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// ��������Ƿ�����
	//pMainFrame->OnSpecifyMarketAndFieldReport(2002, EMRFRisePercent, true);

	// �A�Ƿ�
	pMainFrame->OnSpecifyBlockIdAndFieldReport(2000, EMRFRisePercent, true);
}

void OnShortCut68()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// ��������Ƿ�����
	//pMainFrame->OnSpecifyMarketAndFieldReport(2003, EMRFRisePercent, true);

	// �B�Ƿ�
	pMainFrame->OnSpecifyBlockIdAndFieldReport(2007, EMRFRisePercent, true);
}

void OnShortCut69()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// ��������Ƿ�����
	//pMainFrame->OnSpecifyMarketAndFieldReport(2004, EMRFRisePercent, true);

	// ��С��ҵ�Ƿ�
	pMainFrame->OnSpecifyMarketAndFieldReport(1004, EMRFRisePercent, true);
}

void OnShortCut71()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// �Ͻ�������
	pMainFrame->OnInfoShBourse();
}

void OnShortCut72()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// �������
	pMainFrame->OnInfoSzBourse();
}

void OnShortCut73()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// �۽�������
	pMainFrame->OnInfoHkBourse();
}

void OnShortCut74()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);

	// ��ʱ����
	pMainFrame->OnInfoNews();
}

void OnShortCut611()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// �Ȩ֤�Ƿ�
	pMainFrame->OnSpecifyBlockIdAndFieldReport(2010, EMRFRisePercent, true);
}

void OnShortCut612()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// ��ҵ���Ƿ�
	pMainFrame->OnSpecifyMarketAndFieldReport(1008, EMRFRisePercent, true);
}

void OnShortCut80()
{
	CDlgReportRank::ShowPopupReportRank(CBlockConfig::GetDefaultMarketlClassBlockPseudoId());	// ��֤A��
}

void OnShortCut81()
{
	CDlgReportRank::ShowPopupReportRank(2003);	// ��֤A��
}

void OnShortCut82()
{
	CDlgReportRank::ShowPopupReportRank(2001);	// ��֤B��
}

void OnShortCut83()
{
	CDlgReportRank::ShowPopupReportRank(2004);	// ��֤A
}

void OnShortCut84()
{
	CDlgReportRank::ShowPopupReportRank(2012);	// ��֤B
}

void OnShortCut85()
{
	CDlgReportRank::ShowPopupReportRank(2018);	// ��֤ծȯ
}

void OnShortCut86()
{
	CDlgReportRank::ShowPopupReportRank(2018);	// ��֤ծȯ
}

void OnShortCut88()
{
	CDlgReportRank::ShowPopupReportRank(2007);	//  �B
}

void OnShortCut89()
{
	CDlgReportRank::ShowPopupReportRank(2005);	// ��С��ҵ
}

void OnShortCut811()
{
	CDlgReportRank::ShowPopupReportRank(2010);	// �Ȩ֤
}

void OnShortCut812()
{
	CDlgReportRank::ShowPopupReportRank(2006);	// ��ҵ��
}

void OnShortCut01()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// ��ҵ���Ƿ�
	pMainFrame->OnShowTimeSaleDetail();
}
void OnShortCut16()
{
	CDlgNotePad::ShowNotePad(CDlgNotePad::EST_InfoMine);
}


void OnShortCut17()
{
	// Ͷ���ռ�
	CDlgNotePad::ShowNotePad(CDlgNotePad::EST_UserNote);
}

void OnShortCut1()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// ֤A�ɡ���֤B�ɡ�����A�ɡ�����B�ɡ���֤ծȯ������ծȯ������A�ɡ�����B�ɡ���С��  
	pMainFrame->OnSpecifyMarketAndFieldReport(1, -1, true);
}

void OnShortCut2()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// ��֤B
	pMainFrame->OnSpecifyMarketAndFieldReport(2, -1, true);
}

void OnShortCut3()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// ��֤A
	pMainFrame->OnSpecifyMarketAndFieldReport(1001, -1, true);
}

void OnShortCut4()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// ��֤B
	pMainFrame->OnSpecifyMarketAndFieldReport(1002, -1, true);
}

void OnShortCut5()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// ��֤ծȯ
	pMainFrame->OnSpecifyMarketAndFieldReport(5, -1, true);
}

void OnShortCut6()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// ��֤ծȯ
	pMainFrame->OnSpecifyMarketAndFieldReport(1006, -1, true);
}

void OnShortCut7()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// �A
	pMainFrame->OnSpecifyBlockIdAndFieldReport(2000, -1, true);
}

void OnShortCut8()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// �B
	pMainFrame->OnSpecifyBlockIdAndFieldReport(2007, -1, true);
}

void OnShortCut9()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	// ��С��ҵ
	pMainFrame->OnSpecifyMarketAndFieldReport(1004, -1, true);
}

void OnShortCut05()
{
	// ��/k�л�
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	pMainFrame->OnShowMerchInNextChart(NULL);
}

void OnShortCut08()
{
	// k�������л�
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	pMainFrame->ChangeIoViewKlineTimeInterval(-1);
}

void OnShortCut36()
{
	// �عɳֲֻ���
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	pMainFrame->PostMessage(WM_COMMAND, ID_ZHONGCANGCHIGU, 0);
}



