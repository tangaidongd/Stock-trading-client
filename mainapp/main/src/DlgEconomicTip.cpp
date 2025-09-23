// DlgEconomicData.cpp : 实现文件
//

#include "stdafx.h"
#include "GGTong.h"
#include "DlgEconomicTip.h"


// CDlgEconomicTip 对话框

IMPLEMENT_DYNAMIC(CDlgEconomicTip, CDialog)

CDlgEconomicTip::CDlgEconomicTip(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEconomicTip::IDD, pParent)
{
	m_bTracking = FALSE;
	m_bOver = FALSE;
	m_count = 0;
	m_offsetV = 0;
	m_vecChildWindow.reserve(5);

	CString strCountryType[] = {"gbtb01","gbtb02","gbtb03","gbtb04","gbtb05","gbtb06","gbtb07","gbtb08","gbtb09","gbtb10","gbtb11",
								"gbtb12","gbtb13","gbtb14","gbtb15","gbtb16","gbtb17","gbtb18","gbtb19","gbtb20","gbtb21","gbtb22",};
	for (int i = 0 ; i < 22 ; ++i)
	{
		m_mapCountryIcon.insert(make_pair<CString,CString>(strCountryType[i], "./image/economic_icon/" + strCountryType[i]+".png"));
	}
	m_mapLevelIcon.insert(make_pair<CString,CString>("低", "./image/economic_icon/低.png"));
	m_mapLevelIcon.insert(make_pair<CString,CString>("中", "./image/economic_icon/中.png"));
	m_mapLevelIcon.insert(make_pair<CString,CString>("高", "./image/economic_icon/高.png"));
}

CDlgEconomicTip::~CDlgEconomicTip()
{
}

void CDlgEconomicTip::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgEconomicTip, CDialog)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()


// CDlgEconomicTip 消息处理程序



void CDlgEconomicTip::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CDlgEconomicTip::OnMouseHover(UINT nFlags, CPoint point)
{
	m_bOver = TRUE;

	CDialog::OnMouseHover(nFlags, point);
}

void CDlgEconomicTip::OnMouseLeave()
{
	::ShowWindow(m_hWnd, SW_HIDE);

	m_bOver = FALSE;
	m_bTracking = FALSE;

	CDialog::OnMouseLeave();
}

void CDlgEconomicTip::InitChildWindow(int iCount, std::vector<T_EconomicData> &vecData)
{
	CRect rt;
	::GetClientRect(m_hWnd, rt);

	int pos =  vecData.size() - 1;
	for (int i = 0; i < iCount  ; ++i)
	{
		T_EconomicData &stEconomicData = vecData.at(pos);
		CDlgEconomicData *pChildWnd = new CDlgEconomicData;
		pChildWnd->Create(CDlgEconomicData::IDD, this);
		pChildWnd->InitData(vecData.at(pos--));
		
		if (pChildWnd->m_bAll)
		{
			pChildWnd->MoveWindow(0, m_offsetV, rt.Width(), CHILD_HEIGHT_V2);
			m_offsetV += CHILD_HEIGHT_V2;
		}
		else
		{
			pChildWnd->MoveWindow(0, m_offsetV, rt.Width(), CHILD_HEIGHT_V1);
			m_offsetV += CHILD_HEIGHT_V1;
		}
		
		CString strCountry = m_mapCountryIcon[stEconomicData.country_type.c_str()];
		CString strLevel = m_mapLevelIcon[stEconomicData.important_level.c_str()];
		pChildWnd->LoadCountryIcon(strCountry);
		pChildWnd->LoadLevelIcon(strLevel);
		
		pChildWnd->ShowWindow(SW_SHOW);

		m_vecChildWindow.push_back(pChildWnd);
	}
}

void CDlgEconomicTip::ReleaseChildWindow()
{
	std::vector<CDlgEconomicData*>::iterator it;
	for (it = m_vecChildWindow.begin() ; it != m_vecChildWindow.end() ; ++it)
	{
		delete *it;
		*it = NULL;
	}
	std::vector<CDlgEconomicData*>().swap(m_vecChildWindow);
	m_offsetV = 0;
}