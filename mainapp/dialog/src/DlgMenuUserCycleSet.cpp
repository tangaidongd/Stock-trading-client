// dlgusercycleset.cpp : implementation file
//

#include "stdafx.h"
#include "DlgMenuUserCycleSet.h"
#include "IoViewShare.h"
#include "IoViewKLine.h"
#include "IoViewDetail.h"
#include "IoViewKLineArbitrage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgUserCycleSet dialog

CDlgMenuUserCycleSet::CDlgMenuUserCycleSet(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgMenuUserCycleSet::IDD, pParent)
{
    //{{AFX_DATA_INIT(CDlgUserCycleSet)
    m_StrPre = _T("");
    m_StrAft = _T("");
    m_uiUserSet = 0;
    //}}AFX_DATA_INIT
    m_pParentIoView = NULL;
	m_eNodeInterval = ENTIMinute;
}

void CDlgMenuUserCycleSet::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlgUserCycleSet)
    DDX_Text(pDX, IDC_STATIC_CYCLE_TYPE, m_StrPre);
    DDX_Text(pDX, IDC_STATIC_CYCLE_POSTFIX, m_StrAft);
    DDX_Text(pDX, IDC_EDIT_CYCLE_SET, m_uiUserSet);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgMenuUserCycleSet, CDialogEx)
    //{{AFX_MSG_MAP(CDlgUserCycleSet)
    ON_WM_CLOSE()
    ON_COMMAND(IDC_CONFIRM, OnButtonConfirm)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgUserCycleSet message handlers

BOOL CDlgMenuUserCycleSet::PreTranslateMessage(MSG* pMsg) 
{	
    if ( WM_KEYDOWN == pMsg->message)
    {
        if ( VK_ESCAPE == pMsg->wParam )
        {
            SendMessage(WM_CLOSE, 0, 0);
            return TRUE;
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

BOOL CDlgMenuUserCycleSet::OnInitDialog() 
{
    //CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    if ( ENTIDayUser == m_eNodeInterval)
    { 
        m_StrPre = L"多日线:";
        m_StrAft = L"天    ( 1 - 365)";
    }
    else if ( ENTIMinuteUser == m_eNodeInterval)
    {
        m_StrPre = L"多分钟:";
        m_StrAft = L"分    ( 1 - 720)";
    }
    else
    {
        //ASSERT(0);
    }

    UpdateData(FALSE);
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMenuUserCycleSet::OnButtonConfirm()
{
    OnOK();
}

void CDlgMenuUserCycleSet::OnClose() 
{
    CDialog::OnClose();
}

void CDlgMenuUserCycleSet::OnOK()
{
    UINT uiOldUserSet = m_uiUserSet;
    if(!UpdateData(TRUE))
    {
        return;
    }

    UINT uiUserSetBk = m_uiUserSet;
    //
    ASSERT( NULL != m_pParentIoView );

    if ( ENTIDayUser == m_eNodeInterval )
    {
        if(uiUserSetBk < USERSET_DAY_MIN)
        {
            m_uiUserSet = USERSET_DAY_MIN;
        }
        else if(uiUserSetBk > USERSET_DAY_MAX)
        {
            m_uiUserSet = USERSET_DAY_MAX;
        }

        if(uiOldUserSet == m_uiUserSet)
        {
            CDialog::OnOK();
            return;
        }
    }
    else if (ENTIMinuteUser == m_eNodeInterval)
    {
        if(uiUserSetBk < USERSET_MINUET_MIN)
        {
            m_uiUserSet = USERSET_MINUET_MIN;
        }
        else if(uiUserSetBk > USERSET_MINUET_MAX)
        {
            m_uiUserSet = USERSET_MINUET_MAX;
        }

        if(uiOldUserSet == m_uiUserSet)
        {
            CDialog::OnOK();
            return;
        }
    }

    if ( m_pParentIoView->IsKindOf(RUNTIME_CLASS(CIoViewDetail)))
    {
        CIoViewDetail* pDetail = (CIoViewDetail*)m_pParentIoView;
        pDetail->OnUserCircleChanged(m_eNodeInterval, m_uiUserSet);

        CDialog::OnOK();
    }
    else if ( m_pParentIoView->IsKindOf(RUNTIME_CLASS(CIoViewKLine)))
    {
        CIoViewKLine* pKLine = (CIoViewKLine*)m_pParentIoView;
        pKLine->OnUserCircleChanged(m_eNodeInterval, m_uiUserSet);

        CDialog::OnOK();
    }
    else if ( m_pParentIoView->IsKindOf(RUNTIME_CLASS(CIoViewKLineArbitrage)))
    {
        CIoViewKLineArbitrage* pKLineArbitrage = (CIoViewKLineArbitrage*)m_pParentIoView;
        pKLineArbitrage->OnUserCircleChanged(m_eNodeInterval, m_uiUserSet);

        CDialog::OnOK();
    }
}