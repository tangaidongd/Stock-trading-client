// DlgBelongBlock.cpp : implementation file
//

#include "stdafx.h"


#include "GGTongView.h"
#include "IoViewTimeSaleRank.h"
#include "IoViewTimeSaleStatistic.h"
#include "GridCtrlSys.h"
#include "GridCellSymbol.h"
#include "DlgBelongBlock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDlgBelongBlock dialog
//linhc 20100918�����Ϣ����
#define UWM_PROCESSCHANGEMERCH (WM_USER+58)
//linhc 20100915�����������
const int32 KUpdateBelongDlgMerchsTimerId				    = 1000001;					// ÿ�� 300����, ����ȡһ����Ʒ��������
const int32 KTimerPeriodBelongMerchs						= 300;

//linhc 20100904 ��ӶԻ���
CDlgBelongBlock::CDlgBelongBlock(IN CMerch* pMerchXml,CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgBelongBlock::IDD, pParent)
	,m_bIsChangeBySel(true)
	,m_pMerchXml(pMerchXml)
{
	//{{AFX_DATA_INIT(CDlgBelongBlock)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
CDlgBelongBlock::~CDlgBelongBlock()
{
    m_ArriBlock.RemoveAll();
}

void CDlgBelongBlock::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBelongBlock)
	DDX_Control(pDX, IDC_LIST_BL_RIGHT, m_ListRight);
	DDX_Control(pDX, IDC_LIST_BL_LEFT, m_ListLeft);
	DDX_Check(pDX, IDC_CHECK_CHANGE_SEL, m_bIsChangeBySel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBelongBlock, CDialogEx)
	//{{AFX_MSG_MAP(CDlgBelongBlock)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_BL_LEFT, OnDblclkListBlLeft)
	ON_BN_CLICKED(IDC_CHECK_CHANGE_SEL, OnCheckButSel)
	ON_WM_CLOSE()
	ON_WM_PAINT()
    ON_MESSAGE(UWM_PROCESSCHANGEMERCH,OnProcessIoViewMerchChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBelongBlock message handlers

void CDlgBelongBlock::OnCancel() 
{
	OnClose();
}


void CDlgBelongBlock::OnPaint() 
{
    CPaintDC dc(this);
	//::ShowScrollBar(m_ListLeft.m_hWnd,SB_HORZ,FALSE);
	//m_ListLeft.ModifyStyle(WS_HSCROLL, 0);
	//m_ListLeft.Invalidate(true);
	//CDialogEx::OnNcPaint();
}


BOOL CDlgBelongBlock::OnInitDialog() 
{
	CDialog::OnInitDialog();
	InitializeLeftBlockList();
	InitializeRightList();
	Initialize();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgBelongBlock::Initialize()
{
	HICON hicon = NULL;
	hicon = AfxGetApp()->LoadIcon(IDI_ICON_FOLDER);
	m_ImageList.Create(16,16,0,7,7);
	m_ImageList.SetBkColor (RGB(255,255,255));
	m_ImageList.Add(hicon);
	m_ListLeft.SetImageList(&m_ImageList,LVSIL_SMALL);
	//linhc 20100918
	StartTimer(KUpdateBelongDlgMerchsTimerId,KTimerPeriodBelongMerchs);
	if ( NULL == m_pMerchXml )
	{
		DoDisplayEmpty();
	}
	else
	{
		//linhc2010909�����ʾ�Ի������ͷ
	    DisplayDlgTitle();
        InitializeListData();
	}
}

void CDlgBelongBlock::InitializeLeftBlockList()
{
	m_ListLeft.ModifyStyle(0,LVS_SHOWSELALWAYS);
	m_ListLeft.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT);
	CString header[]={L"�������",L"������",L"��Ʊ��"};
	
	m_ListLeft.InsertColumn(0,header[0],LVCFMT_CENTER,110);
	m_ListLeft.InsertColumn(1,header[1],LVCFMT_CENTER,125);
	m_ListLeft.InsertColumn(2,header[2],LVCFMT_CENTER,55);

	m_ListLeft.SetColumnWidth(2,LVSCW_AUTOSIZE_USEHEADER);
	//::ShowScrollBar(m_ListLeft.m_hWnd,SB_HORZ,FALSE);
}

void CDlgBelongBlock::InitializeRightList()
{
   	CString header[]={L"���Ƶ�����Ϣ",L"������"};
    //�����ʾ�г���ͷ
	m_ListRight.InsertColumn(0,header[0],LVCFMT_CENTER,100);
	m_ListRight.InsertColumn(1,header[1],LVCFMT_CENTER,70);
}

void CDlgBelongBlock::InitializeListData()
{
	m_ListLeft.DeleteAllItems();
	m_ArriBlock.RemoveAll();
	CBlockConfig::BlockArray aBlocks;
	if ( NULL == m_pMerchXml )
	{
		return;
	}
	CBlockConfig::Instance()->GetBlocksByMerch(m_pMerchXml->m_MerchInfo.m_iMarketId,
		                            m_pMerchXml->m_MerchInfo.m_StrMerchCode,aBlocks);
 
	int32 iSizeBlock = aBlocks.GetSize();
	for ( int32 i = 0; i < iSizeBlock; i++ )
	{	
		if ( NULL != aBlocks[i] )
		{
			int  Etype= aBlocks[i]->m_blockInfo.GetBlockType();
			
			if ( Etype == CBlockInfo::typeNormalBlock
				 || Etype == CBlockInfo::typeUserBlock )
			{
				InsertSingleBlock((*aBlocks.GetAt(i)));
			}
		}
	}

	m_ListLeft.SetColumnWidth(2,LVSCW_AUTOSIZE_USEHEADER);
}

void CDlgBelongBlock::InsertSingleBlock(const CBlockLikeMarket& oBlock)
{
	//����Ѿ����ھͲ��ڲ�����ʾ�б�
	if ( ExistBlockPre(oBlock.m_blockInfo.m_StrBlockName) )
	{
		return;
	}

	int32 nIndex = m_ListLeft.GetItemCount();
	CString strNum = L"";
	//ȷ����ʾIDΨһ
	m_ArriBlock.Add(oBlock.m_blockInfo.m_StrBlockName);
    m_ListLeft.InsertItem(nIndex,oBlock.m_blockInfo.m_StrBlockName);
	m_ListLeft.SetItemText(nIndex,1,oBlock.m_blockCollection.m_StrName);
	strNum.Format(L"%d",oBlock.m_blockInfo.m_aSubMerchs.GetSize());
	m_ListLeft.SetItemText(nIndex,2,strNum);
	m_ListLeft.SetItemData(nIndex,(DWORD)oBlock.m_blockInfo.m_iBlockId);
}

void CDlgBelongBlock::OnDblclkListBlLeft(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int32 nItem = -1;
	LPNMITEMACTIVATE lpNMItemActivate = (LPNMITEMACTIVATE)pNMHDR;
	if( NULL != lpNMItemActivate )
	{
		nItem = lpNMItemActivate->iItem;
        if ( nItem != -1 )
        {   
			DWORD DwBlockId  = m_ListLeft.GetItemData(nItem);
			DealDbClick(DwBlockId);
        }
	}
	*pResult = 0;
}

BOOL CDlgBelongBlock::PreTranslateMessage(MSG* pMsg) 
{
	if ( WM_KEYDOWN == pMsg->message )
	{
		if ( VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam )
		{
			PostMessage(WM_CLOSE,0,0);
		}
		else
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			if ( NULL != pMainFrame )
			{	
				pMainFrame->ProcessHotkey(pMsg);
			    return TRUE;
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgBelongBlock::DealDbClick(const int32& iBlockId)
{
	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iBlockId);
	if ( NULL != pBlock )
	{
		CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
		if ( NULL == pMainFrame )
		{
			//ASSERT(0);
			return;
		}
		
		T_BlockDesc	desc;
		desc.m_StrBlockName = pBlock->m_blockInfo.m_StrBlockName;
		desc.m_aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);
		if ( CBlockConfig::Instance()->IsUserBlock(iBlockId) )	// ֻ֧�������ְ��
		{
			desc.m_eType = T_BlockDesc::EBTUser;
		}
		else
		{
			desc.m_eType = T_BlockDesc::EBTBlockLogical;
			desc.m_iMarketId = pBlock->m_blockInfo.m_iBlockId;	// ���id��Ϊ�г�id
		}
		
		CIoViewReport* pIoView = pMainFrame->OnSpecifyBlock(desc);
		
		if ( NULL != pIoView )
		{   
            pIoView->SetGridSelByMerch(m_pMerchXml);
		}
	}
}

bool32 CDlgBelongBlock::ExistBlockPre(const CString &strBlockName)
{
    for ( int32 i = 0;i < m_ArriBlock.GetSize();i++ )
    {
		if ( strBlockName == m_ArriBlock.GetAt(i) )
		{
			return true;
		}
    }
	return false;
}

void CDlgBelongBlock::PostNcDestroy() 
{   
	delete this;
	CDialog::PostNcDestroy();
}

void CDlgBelongBlock::NotifyChangeMerch(IN CMerch& oMerch )
{
	if ( m_bIsChangeBySel )
	{
		m_pMerchXml = & oMerch;
		//��ǰһ����ʾ��Ʒ��ͬ������ʾ��
		if ( !IsSameMerch(oMerch) )
		{
		    m_oMerchInfo = m_pMerchXml->m_MerchInfo;
		    DisplayDlgTitle();
		    InitializeListData();
		    UpdateData(false);
		}
	}
}

void CDlgBelongBlock::DisplayDlgTitle()
{
	if ( NULL == m_pMerchXml )
	{
		//ASSERT(0);
		return;
	}
	CString strHeadTitle = L"";
	strHeadTitle = L"(" + m_pMerchXml->m_MerchInfo.m_StrMerchCode + L")";
    strHeadTitle = m_pMerchXml->m_MerchInfo.m_StrMerchCnName + strHeadTitle + L"�������";
	CDialogEx::SetWindowTextEx(strHeadTitle);
}

void CDlgBelongBlock::OnCheckButSel()
{
	m_bIsChangeBySel = !m_bIsChangeBySel;
}

void CDlgBelongBlock::OnClose() 
{   //linhc 20100915���
	StopTimer(KUpdateBelongDlgMerchsTimerId);
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	if ( NULL != pMainFrame )
	{	
		pMainFrame->m_pDlgbelong = NULL;
		DestroyWindow();
	}
	else
	{
		//ASSERT(0);
		return;
	}
}

bool CDlgBelongBlock::IsSameMerch( const CMerch& oMerchXml )
{
	if ( oMerchXml.m_MerchInfo.m_iMarketId == m_oMerchInfo.m_iMarketId &&
		oMerchXml.m_MerchInfo.m_StrMerchCode == m_oMerchInfo.m_StrMerchCode )
	{
		return true;
	}
	return false;
}

void CDlgBelongBlock::DoDisplayEmpty()
{
	if ( m_bIsChangeBySel )
	{
	    m_ListLeft.DeleteAllItems();
	    SetWindowTextEx(L"����ͼ��û�й�ע��Ʒ");
	    m_ArriBlock.RemoveAll();
	    m_oMerchInfo.m_iMarketId = -1;
	    m_oMerchInfo.m_StrMerchCode = L"";
		UpdateData(false);
	}
}

LRESULT CDlgBelongBlock::OnProcessIoViewMerchChange(WPARAM wParam,LPARAM lParam)
{ 
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

	if ( NULL == pMainFrame || NULL == pMainFrame->m_pDlgbelong )
	{  
		return 1L;
	}

	CIoViewBase* pIoViewActive = pMainFrame->FindActiveIoView();
	
	if ( NULL == pIoViewActive )
	{
		DoDisplayEmpty();
		return 1L;
	}

	CMerch* pMerch = NULL;
    //linhc 20100916�������⴦���ˣ����ɴ˷�������
	pMerch = pIoViewActive->GetMerchXml();

	if ( NULL != pMerch )
	{
		NotifyChangeMerch(*pMerch);
	}
	else
	{
		DoDisplayEmpty();
	}
	UpdateWindow();
	return 1L;
}
//linhc 20100918��Ӷ�ʱ��
void CDlgBelongBlock::OnXTimer( int32 iTimerId )
{
	if ( iTimerId == KUpdateBelongDlgMerchsTimerId )
	{
		SendMessage(UWM_PROCESSCHANGEMERCH,0,0);
	}
}
