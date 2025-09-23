// Trade.cpp : implementation file
//

#include "stdafx.h"

#include "DlgQuickOrder.h"
#include "coding.h"
#include "DlgTip.h"
#include "ShareFun.h"
#include <sstream>
#include "XmlShare.h"
#include <WINUSER.H>
#include "PathFactory.h"
#include "FontFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgQuickOrder dialog

const TCHAR	KStrTradeOpenSection[]	= _T("TradeOpen");
const TCHAR	KStrTradeOpenKeyCheck[]	= _T("quickcheck");

const TCHAR	KStrTradeCloseSection[]	= _T("TradeClose");
const TCHAR	KStrTradeCloseKeyCheck[] = _T("quickcheck");


namespace
{
#define TIME_CLEAR_TIP	WM_USER+6001

#define ID_TITLE_HEIGHT 32
#define IDC_TAB_HEIGHT	26

// �ؼ�ID
#define IDC_BTN_TITLE_EXIT			1996
#define IDC_BTN_TITLE_BK			1997
#define IDC_BTN_TITLE_ICON			1998
#define IDC_BTN_TITLE_TEXT			1999
#define IDC_BTN_QUICK_ORDER_OPEN	2001
#define IDC_BTN_QUICK_ORDER_CLOSE	2002
#define IDC_BTN_GOODS_NAME_TIP		2003
#define IDC_BTN_GOODS_NAME			2004
#define IDC_BTN_QUICK_ORDER_HAND	2005
#define IDC_BTN_QUICK_ORDER_SUB		2006
#define IDC_BTN_QUICK_ORDER_ADD		2007
#define IDC_BTN_QUICK_ORDER_LEFT	2008
#define IDC_BTN_QUICK_ORDER_RIGHT	2009
#define IDC_BTN_HANDING_EMPTY		2010
#define IDC_BTN_HANDING_MANY		2011
#define IDC_CHECK_QUICK_ORDER_TIP	2012
#define IDC_EDIT_QUICK_ORDER_HAND	2013
#define IDC_BTN_CHECKBOX			2014
#define IDC_BTN_EDIT_BK				2015
}

CDlgQuickOrder::CDlgQuickOrder(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgQuickOrder::IDD, pParent)
{
	m_bOpen = TRUE;
	
	m_pTradeBid = NULL;
	m_pTradeQuery = NULL;
	memset(m_chCommID,0,sizeof(m_chCommID));
	
	m_iHand = 1; // Ĭ�Ͻ�����1��
	m_iHandMax = 0;
	m_iHandMin = 0;
	m_iPointDef = 50;
	m_dPriceStep = 0;
	m_iPointCount = 0;

	m_strBuyPrice = "";
	m_strSellPrice = "";
	m_strQty = "";
	m_strPoint = "";

	m_imgTitleBK = NULL;	
	m_imgTitleIcon = NULL;	
	m_imgTitleText = NULL;	
	m_imgTitleExit = NULL;	
	m_imgStatic = NULL;
	m_imgSub = NULL;		
	m_imgAdd = NULL;		
	m_imgLeftbtn = NULL;	
	m_imgRightbtn = NULL;	
	
	m_dCurrentMargin = 0;
	m_dMarginMarket = 0;
	m_iHodingEmptyMax = 0;
	m_iHodingManyMax = 0;

	m_bmpbkTitle.m_hObject = NULL;
	m_bmpTitleClose.m_hObject = NULL;
	m_bTradeComm = FALSE;

	LOGFONT lgFont = {0};
	::GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT),sizeof(lgFont),&lgFont);
	CFontFactory fontFacotry;
	_tcscpy(lgFont.lfFaceName, fontFacotry.GetExistFontName(L"΢���ź�"));
	lgFont.lfHeight = 18;
	lgFont.lfWeight = 400;
	m_fontEdit.CreateFontIndirect(&lgFont);

	m_BrushStaticBack = CreateSolidBrush(RGB(230,230,230));
}


void CDlgQuickOrder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrade)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgQuickOrder, CDialog)
	//{{AFX_MSG_MAP(CTrade)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_WM_NCHITTEST()
	ON_EN_CHANGE(IDC_EDIT_QUICK_ORDER_HAND, OnEnChangeEditPoint)
	ON_COMMAND(IDC_BTN_TITLE_EXIT, OnBtnClose)
	ON_MESSAGE(UM_TBNCLICK, OnTabExchange)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BTN_CHECKBOX,OnCheckBoxClicked)
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrade message handlers
CDlgQuickOrder::~CDlgQuickOrder()
{
	DEL(m_imgTitleBK);	
	DEL(m_imgTitleIcon);	
	DEL(m_imgTitleText);	
	DEL(m_imgTitleExit);	
	DEL(m_imgStatic);
	DEL(m_imgSub);		
	DEL(m_imgAdd);		
	DEL(m_imgLeftbtn);	
	DEL(m_imgRightbtn);	
	
	m_fontButtonSmall.DeleteObject();
	m_fontButtonBig.DeleteObject();
	m_fontEditHand.DeleteObject();
	DeleteObject(m_BrushStaticBack);
}

BOOL CDlgQuickOrder::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	//m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_WINLOGO);
	CString StrIconPath = CPathFactory::GetImageMainIcon32Path();
	
	HICON hIcon = (HICON)LoadImage(AfxGetInstanceHandle(), StrIconPath, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE|LR_LOADFROMFILE);
	
	if ( NULL == hIcon )
	{
		hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	}

	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// ��ʼ���ؼ�
	InitControl();
	

	// �������ڴ�С
	SetWindowSize();

	AdjustControlPos();

	CFontFactory fontFactory;

	LOGFONT logfont;
	::ZeroMemory(&logfont,sizeof(LOGFONT));
	_tcscpy(logfont.lfFaceName,fontFactory.GetExistFontName(_T("΢���ź�")));
	logfont.lfWeight = 400;
	logfont.lfWidth = 9;
	logfont.lfHeight = 18;
	m_font.CreateFontIndirect(&logfont);

	//���ó�ʼλ��
	//int cx = GetSystemMetrics(SM_CXSCREEN);
	//int cy = GetSystemMetrics(SM_CYSCREEN);

	SetBkTitleBitmap(IDB_BITMAP_QUICK_ORDER__BK_TITLE);
	SetTitleIconRes(IDB_BITMAP_QUICK_ORDER_ICON);
	SetTitleCloseRes(IDB_BITMAP_QUICK_ORDER_CLOSE);

	return TRUE;
}

void CDlgQuickOrder::SetWindowSize(int nLeft, int nTop, int nWidth, int nHeight)
{
	CRect rect(0,0,0,0);
	rect.left = nLeft;
	rect.right = nLeft + 219;
	rect.top = nTop;
	rect.bottom = nTop + 237;
	MoveWindow(rect);
	CenterWindow();
}

void CDlgQuickOrder::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);

	//if (m_hWnd)
	//{
	//	AdjustControlPos();
	//}
}

void CDlgQuickOrder::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

void CDlgQuickOrder::OnPaint() 
{
	CPaintDC  dc(this); 

	CRect rtClient;
	GetClientRect(rtClient);

	CDC  memDC;
	memDC.CreateCompatibleDC(&dc);

	CBitmap bitmap;	
	bitmap.CreateCompatibleBitmap(&dc, rtClient.Width(), rtClient.Height());

	/*CBitmap *pOldBitmap =*/ memDC.SelectObject(&bitmap);
	memDC.FillSolidRect(rtClient, RGB(0xF2, 0xF0, 0xEB));
	memDC.SetBkMode(TRANSPARENT);

	HGDIOBJ hOldBmp = NULL;

	Graphics graphics(memDC.m_hDC);

	// ���Ʊ��ⱳ��ɫ
	m_btnTitleBK.DrawButton(&graphics);
	m_btnIcon.DrawButton(&graphics);
	m_btnTitleText.DrawButton(&graphics);
	m_btnGoodsNameTip.DrawButton(&graphics);
	m_btnHandTip.DrawButton(&graphics);
	m_btnEditBK.DrawButton(&graphics);
	m_btnSub.DrawButton(&graphics);
	m_btnAdd.DrawButton(&graphics);
	m_btnLeft.DrawButton(&graphics);
	m_btnRight.DrawButton(&graphics);

	dc.BitBlt( rtClient.left, rtClient.top, rtClient.Width(), rtClient.Height(), &memDC, 0, 0, SRCCOPY);

	if (hOldBmp)
		memDC.SelectObject(hOldBmp);
	memDC.DeleteDC();
	bitmap.DeleteObject();
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDlgQuickOrder::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


HBRUSH CDlgQuickOrder::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

LRESULT CDlgQuickOrder::OnNcHitTest( CPoint point )
{	
	RECT rtClient;
	GetClientRect(&rtClient);
	rtClient.bottom = rtClient.top + ID_TITLE_HEIGHT;
	ScreenToClient(&point);
	if(PtInRect(&rtClient,point))
	{
		return HTCAPTION;
	}
	return HTCLIENT;	
}


void CDlgQuickOrder::InitControl()
{
	ImageFromIDResource(AfxGetResourceHandle(),IDB_PNG_QUICK_ORDER_TITLE_BK, L"PNG", m_imgTitleBK);
	ImageFromIDResource(AfxGetResourceHandle(),IDB_PNG_QUICK_ORDER_TITLE_CION, L"PNG", m_imgTitleIcon);
	ImageFromIDResource(AfxGetResourceHandle(),IDB_PNG_QUICK_ORDER_TITLE_BK, L"PNG", m_imgTitleText);
	ImageFromIDResource(AfxGetResourceHandle(),IDB_PNG_QUICK_ORDER_SUB, L"PNG", m_imgSub);
	ImageFromIDResource(AfxGetResourceHandle(),IDB_PNG_QUICK_ORDER_ADD, L"PNG", m_imgAdd);
	ImageFromIDResource(AfxGetResourceHandle(),IDB_PNG_QUICK_ORDER_RED, L"PNG", m_imgLeftbtn);
	ImageFromIDResource(AfxGetResourceHandle(),IDB_PNG_QUICK_ORDER_GREEN, L"PNG", m_imgRightbtn);

	m_btnTitleBK.CreateButton(L"", CRect(0, 0, 0, 0), this,m_imgTitleBK,1, IDC_BTN_TITLE_BK);
	m_btnIcon.CreateButton(L"", CRect(0, 0, 0, 0), this,m_imgTitleIcon,1, IDC_BTN_TITLE_ICON);
	m_btnTitleText.CreateButton(L"���罻��", CRect(0, 0, 0, 0), this,m_imgTitleText,1, IDC_BTN_TITLE_TEXT);
	m_btnGoodsNameTip.CreateButton(L"", CRect(0, 0, 0, 0), this,NULL,0, IDC_BTN_GOODS_NAME_TIP);
	m_btnGoodsName.Create(_T(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_GOODS_NAME);
	m_btnHandTip.CreateButton(L"", CRect(0, 0, 0, 0), this,NULL,0, IDC_BTN_QUICK_ORDER_HAND);
	m_btnEditBK.CreateButton(L"", CRect(0, 0, 0, 0), this,NULL,0, IDC_BTN_EDIT_BK);
	m_btnSub.CreateButton(L"", CRect(0, 0, 0, 0), this,m_imgSub,2, IDC_BTN_QUICK_ORDER_SUB);
	m_btnAdd.CreateButton(L"", CRect(0, 0, 0, 0), this,m_imgAdd,2, IDC_BTN_QUICK_ORDER_ADD);
	m_btnLeft.CreateButton(L"", CRect(0, 0, 0, 0), this,m_imgLeftbtn,4, IDC_BTN_QUICK_ORDER_LEFT);
	m_btnRight.CreateButton(L"", CRect(0, 0, 0, 0), this,m_imgRightbtn,4, IDC_BTN_QUICK_ORDER_RIGHT);
	m_staticHandingEmpty.Create(_T(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_HANDING_EMPTY);
	m_staticHandingMany.Create(_T(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_HANDING_MANY);
	m_staticHandingEmpty.SetAlign(DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	m_staticHandingMany.SetAlign(DT_CENTER|DT_VCENTER|DT_SINGLELINE);

	// �߿���ɫ
	m_btnEditBK.SetTextFrameColor(Color(RGB(255,255,255)),Color(RGB(255,255,255)),Color(RGB(255,255,255)));
	m_btnGoodsNameTip.SetTextFrameColor(Color(RGB(242,240,235)),Color(RGB(242,240,235)),Color(RGB(242,240,235)));
	m_btnHandTip.SetTextFrameColor(Color(RGB(242,240,235)),Color(RGB(242,240,235)),Color(RGB(242,240,235)));
	m_btnTitleText.SetTextFrameColor(Color(RGB(242,240,235)),Color(RGB(242,240,235)),Color(RGB(242,240,235)));


	// �ı�����ɫ
	m_btnGoodsNameTip.SetTextBkgColor(RGB(242,240,235),RGB(242,240,235),RGB(242,240,235));
	m_btnHandTip.SetTextBkgColor(RGB(242,240,235),RGB(242,240,235),RGB(242,240,235));

	// �ı�ɫ
	m_btnTitleText.SetTextColor(RGB(100,100,100),RGB(100,100,100),RGB(100,100,100));
	m_btnGoodsNameTip.SetTextColor(RGB(100,100,100),RGB(100,100,100),RGB(100,100,100));
	m_btnHandTip.SetTextColor(RGB(100,100,100),RGB(100,100,100),RGB(100,100,100));

	m_editHand.Create(WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_CENTER,CRect(0,0,0,0),this,IDC_EDIT_QUICK_ORDER_HAND);
	m_editHand.SetWindowText(_T("1"));
	m_editHand.SetFont(&m_fontEdit);

	CFontFactory fontFacotry;
	CNCButton::T_NcFont	fontTradeBtn;
	fontTradeBtn.m_StrName = fontFacotry.GetExistFontName(_T("΢���ź�"));
	fontTradeBtn.m_Size	 = 12;
	fontTradeBtn.m_iStyle = FontStyleRegular;

	m_btnGoodsNameTip.SetCaption(_T("��Ʒ:"));
	m_btnHandTip.SetCaption(_T("����:"));

	m_btnLeft.SetSubCaption(_T("0.0"));
	m_btnLeft.SetCaption(_T("����"));
	m_btnLeft.SetFont(fontTradeBtn);

	m_btnRight.SetSubCaption(_T("0.0"));
	m_btnRight.SetCaption(_T("����"));
	m_btnRight.SetFont(fontTradeBtn);

	// ����
	BTN_Info btnOpenInfo;
	btnOpenInfo.Img = IDB_BITMAP_QUICK_ORDER_TAB;
	btnOpenInfo.uCountStatus = 2;
	btnOpenInfo.uID = IDC_BTN_QUICK_ORDER_OPEN;
	btnOpenInfo.uGroupID = 2;
	btnOpenInfo.szText = _T("����");
	m_btnOpen.Create(&btnOpenInfo,this,CRect(0,0,0,0),TRUE);
	m_btnOpen.SetSel();

	// ƽ��
	BTN_Info btnCloseInfo;
	btnCloseInfo.Img = IDB_BITMAP_QUICK_ORDER_TAB;
	btnCloseInfo.uCountStatus = 2;
	btnCloseInfo.uID = IDC_BTN_QUICK_ORDER_CLOSE;
	btnCloseInfo.uGroupID = 2;
	btnCloseInfo.szText = _T("ƽ��");
	m_btnClose.Create(&btnCloseInfo,this,CRect(0,0,0,0),TRUE);

	m_btnTitleExit.SetBkBitmap(IDB_BITMAP_QUICK_ORDER_EXIT,2);
	m_btnTitleExit.Create(_T(""),WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,IDC_BTN_TITLE_EXIT);

	m_btnCheckbox.Create(_T("�µ�ǰȷ��"),WS_CHILD|WS_VISIBLE|BS_CHECKBOX,CRect(0,0,0,0),this,IDC_BTN_CHECKBOX);
	m_btnCheckbox.SetTitleColor(RGB(100,100,100));
	m_btnCheckbox.SetBkFromResource(IDB_BITMAP_QUICK_ORDER_CHECKBOX,2);
}

void CDlgQuickOrder::OnBtnSub()
{
	CString strText;
	m_editHand.GetWindowText(strText);
	int n = _ttoi(strText);
	if (n > 1)
		--n;
	strText.Format(_T("%d"),n);
	m_editHand.SetWindowText(strText);
	m_iHand = n;
}

void CDlgQuickOrder::OnBtnAdd()
{
	// ���û��ѡ����Ʒ ���� ѡ�����Ʒ���ǽ�����Ʒ������������
	if (m_strCommID.IsEmpty() || !m_bTradeComm)
		return;

	CString strText;
	m_editHand.GetWindowText(strText);
	int n = _ttoi(strText);
	++n;
	// ����ǽ��֣������������50
	if (m_bOpen)
	{
		if (n > 50)
			return;
	}
	else// �����ƽ�֣���������������ǡ��ֿա��͡��ֶࡰ�Ľϴ���
	{
		int nMax = (m_iHodingEmptyMax > m_iHodingManyMax) ? m_iHodingEmptyMax : m_iHodingManyMax;
		if (n > nMax)
		{
			n = nMax;
		}
	}
	strText.Format(_T("%d"),n);
	m_editHand.SetWindowText(strText);
	m_iHand = n;
}

void CDlgQuickOrder::BuySell(E_QuickEntrustType eEntrustType)
{
	if (NULL==m_pTradeLoginInfo||NULL==m_pTradeBid||NULL==m_pTradeQuery)
	{
		//ASSERT(0);
		return;
	}

	//���ж��Ƿ�����µ�����
	CString strWarn = _T("");
	if (0!=WarningMsg(strWarn, eEntrustType))
	{
		CDlgTip dlg;
		dlg.m_strTipMsg = strWarn;
		dlg.m_eTipType = ETT_WAR;
		dlg.m_pCenterWnd = this;//GetParent();	// ����Ҫ���е��Ĵ���ָ��

		dlg.DoModal();
		return;
	}

	CClientReqEntrust order;
	
	order.account = m_pTradeLoginInfo->StrUser;
	order.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
	order.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;
	order.mid_code = m_QueryUserInfoResult.mid_code;

	std::stringstream stream;
	if (EET_OPEN_BUY == eEntrustType) //���
	{
		order.entrust_bs =_T("B");
		order.eo_flag = "0";
		stream << m_iHand;
		order.entrust_price = (char*)m_strBuyPrice.c_str();
	}
	else if (EET_OPEN_SELL == eEntrustType) //���
	{
		order.entrust_bs =_T("S");
		order.eo_flag = "0";
		stream << m_iHand;
		order.entrust_price = (char*)m_strSellPrice.c_str();
	}
	else if (EET_CLOSE_BUY == eEntrustType) //ƽ��
	{
		order.entrust_bs =_T("B");
		order.eo_flag = "1";
		stream << m_iHand;
		order.entrust_price = (char*)m_strBuyPrice.c_str();
	}
	else if (EET_CLOSE_SELL == eEntrustType) //ƽ��
	{
		order.entrust_bs =_T("S");
		order.eo_flag = "1";
		stream << m_iHand;
		order.entrust_price = (char*)m_strSellPrice.c_str();
	}
	order.stock_code = m_chCommID;
	m_strQty =stream.str();
	order.entrust_amount = atoi((char*)m_strQty.c_str());

	std::stringstream stream2;
	stream2 << m_iPointDef;
	m_strPoint =stream2.str();
	order.dot_diff = atof((char*)m_strPoint.c_str());

	order.entrust_prop = "0";
	order.entrust_type = "0";
	order.hold_id = "";
	order.other_id = "";

	if (m_QueryTraderIDVector.size() > 0)
	{
		order.other_id = m_QueryTraderIDVector[0].other_id;
	}
	
	BOOL bSuc = FALSE;

	CString strTip;

	// �µ���ʾ
	CDlgTip dlg;

	// �µ�ǰ���Ƿ���ʾ�û�Ҫ�µ�
	if (m_btnCheckbox.GetChecked())
	{
		CRect rtWindowPos;
		GetWindowRect(rtWindowPos);
		CPoint pt(rtWindowPos.left,rtWindowPos.top);
		dlg.AdjustWindowPos(pt.x, pt.y);

		dlg.m_strTipMsg = GetTradeTipMsg(eEntrustType);
		dlg.m_eTipType = ETT_TIP;
		dlg.m_pCenterWnd = this;//GetParent();
		//ShowWindow(SW_HIDE);
		if (dlg.DoModal() == IDOK)// �����ȷ����ť
		{
			if (m_pTradeQuery->ReqEntrust(order, strTip))
			{
				bSuc = TRUE;
				m_pTradeBid->m_bReqQuick = TRUE;
			}
		}
		else// ����˹رհ�ť
		{
			return;
		}
	}
	else
	{
		if (m_pTradeQuery->ReqEntrust(order, strTip))
		{
			bSuc = TRUE;
			m_pTradeBid->m_bReqQuick = TRUE;
		}
	}

	/************�����µ������ʾ****************/
	// �µ�ʧ��
	if (!bSuc)
	{
		CDlgTip dlgTip;
		dlgTip.m_strTipMsg = strTip;
		dlgTip.m_eTipType = ETT_ERR;
		dlgTip.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��
		dlgTip.DoModal();
	}
}

void CDlgQuickOrder::OnEnChangeEditPoint()
{
	CString str = _T("");
	m_editHand.GetWindowText(str);
	int n = _ttoi(str);

	if (n < 1)
	{
		m_editHand.SetWindowText(_T("1"));
		str = _T("1");
	}

	if (m_bOpen)
	{
		if (n > 50)
		{
			m_editHand.SetWindowText(_T("50"));
			str.Format(_T("%d"),50);
		}
	}
	else// �����ƽ�֣���������������ǡ��ֿա��͡��ֶࡰ�Ľϴ���
	{
		int nMax = (m_iHodingEmptyMax > m_iHodingManyMax) ? m_iHodingEmptyMax : m_iHodingManyMax;
		if (n > nMax)
		{
			str.Format(_T("%d"),nMax);
			m_editHand.SetWindowText(str);
		}
	}

	string strHand;
	UnicodeToUtf8(str, strHand);
	m_iHand = atoi(strHand.c_str());

	UpdateData(FALSE);
}

void CDlgQuickOrder::SetTradeBid( iTradeBid *pTradeBid )
{
	if ( m_pTradeBid )
	{
		m_pTradeBid->RemoveTradeBidNotify(this);
	}
	
	m_pTradeBid = pTradeBid;
	if (NULL==m_pTradeBid)
	{
		//ASSERT(0);
		return;
	}
	m_pTradeBid->AddTradeBidNotify(this);		
}

bool CDlgQuickOrder::SetCommID(CString strCommID)
{
	m_strCommID = strCommID;
	if (strCommID.IsEmpty())
	{
		m_iHodingEmptyMax = 0;
		m_iHodingManyMax = 0;
		m_iHandMax = 0;
		strncpy(m_chCommID, "",sizeof(m_chCommID)-1);
		EnableTradeButton(m_bOpen);
		m_staticHandingEmpty.SetTitleText1(_T("����0��"));
		m_staticHandingMany.SetTitleText1(_T("����0��"));
		EnableTradeButton(FALSE);
		m_editHand.SetWindowText(_T("1"));
		return false;
	}
	m_btnGoodsName.SetTitleText2(_T(""));

	bool bFind = false;

	string str;
	UnicodeToUtf8(strCommID, str);

	for (int i=0;i<m_QueryCommInfoResultVector.size();i++)// �鿴��Ʒ�����Ƿ����
	{
		T_TradeMerchInfo stOutComm = m_QueryCommInfoResultVector[i];
		string strCode;
		UnicodeToUtf8(stOutComm.stock_code, strCode);
		if (strcmp(str.c_str(), strCode.c_str())==0) //����ʹ��붼��ͬ
		{
			bFind = true;
			break;
		}
	}
	
	if (bFind)
	{
		strncpy(m_chCommID,str.c_str(),sizeof(m_chCommID)-1);

		//���ݴ����ȡ����
		CString strName = GetCommName(m_chCommID);
		m_btnGoodsName.SetTitleText1(strName);
		EnableTradeButton(TRUE);
	}
	else
	{
		m_iHodingEmptyMax = 0;
		m_iHodingManyMax = 0;
		m_iHandMax = 0;
		m_iHandMin = 0;
		strncpy(m_chCommID, "",sizeof(m_chCommID)-1);
		m_btnGoodsName.SetTitleText1(_T("-"));
		m_staticHandingEmpty.SetTitleText1(_T("����0��"));
		m_staticHandingMany.SetTitleText1(_T("����0��"));
		EnableTradeButton(FALSE);
	}
	m_bTradeComm = bFind;
	SetVisiableHoding(m_bOpen);

	return bFind;
}

void CDlgQuickOrder::OnLoginStatusChanged(int iCurStatus, int iOldStatus)
{

}

bool32 CDlgQuickOrder::OnInitQuery(CString &strTipMsg)
{
	return TRUE;
}

bool32 CDlgQuickOrder::OnQueryUserInfo(CString &strTipMsg)
{
	return TRUE;
}

void CDlgQuickOrder::OnInitCommInfo()
{
}

void CDlgQuickOrder::OnQueryHoldDetailResponse()
{

}

// �ֲֻ���
void CDlgQuickOrder::OnQueryHoldSummaryResponse()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}

	m_QueryHoldSummaryVector = m_pTradeBid->GetCacheHoldSummary();
}
// void CDlgQuickOrder::OnQueryEntrustResponse()
// {
// 
// }

void CDlgQuickOrder::OnQueryLimitEntrustResponse()
{
	
}

void CDlgQuickOrder::OnQueryDealResponse()
{

}

void CDlgQuickOrder::OnQueryCommInfoResponse()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}

	m_QueryCommInfoResultVector = m_pTradeBid->GetCacheCommInfo();	
}

void CDlgQuickOrder::OnQueryTraderIDResponse()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}

	m_QueryTraderIDVector = m_pTradeBid->GetCacheTraderID();
}

void CDlgQuickOrder::OnQueryUserInfoResponse()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}

	m_QueryUserInfoResult = m_pTradeBid->GetCacheUserInfo();
	m_dCurrentMargin = m_QueryUserInfoResult.today_enable;	//���ñ�֤��
}

void CDlgQuickOrder::OnDisConnectResponse()
{
	
}

void CDlgQuickOrder::OnReqEntrustResponse(bool32 bShowTip)
{
	if (!m_pTradeBid->m_bReqQuick)
	{
		return;
	}
	else
	{
		m_pTradeBid->m_bReqQuick = FALSE;
	}

	//if(!IsWindowVisible())
	//	return;
	E_TipType eTipType = ETT_WAR;
	
	//CString strTip = _T("");
	BOOL bSuc = FALSE;
	eTipType = ETT_TIP;
	
	
	CClientRespEntrust resp = m_pTradeBid->GetCacheReqEntrust();
	if (0 == resp.error_no) // �ɹ�
	{
		bSuc = TRUE;
		eTipType = ETT_SUC;
	//	OnOK();	
	}

	if (bSuc)
	{
		OnBtnClose();
	}

	if (bShowTip)
	{
		// ��ʾ�µ����
		CDlgTip dlg;
		dlg.m_strTipMsg = resp.error_info;
		dlg.m_eTipType = eTipType;
		dlg.m_pCenterWnd = this;//GetParent();
		dlg.DoModal();
	}

	//SetTimer(TIME_CLEAR_TIP, 6000, NULL);  
	
	// ֪ͨҳ��ˢ��
	if (NULL!=m_pTradeQuery)
	{
		m_pTradeQuery->EntrustResultNotify(true);
		m_pTradeQuery->DoQueryAsy(EQT_QueryHoldSummary);
	}
}

void CDlgQuickOrder::OnReqCancelEntrustResponse()
{

}

void CDlgQuickOrder::OnReqModifyPwdResponse()
{

}

void CDlgQuickOrder::OnReqSetStopLPResponse()
{

}

void CDlgQuickOrder::OnReqCancelStopLPResponse()
{

}

// ��ѯ����
void CDlgQuickOrder::OnQueryQuotationResponse()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}
	
	m_QuotationResultVector = m_pTradeBid->GetCacheQuotation();

 	SetPrice();	// ������£�˳������±༭��
}

void CDlgQuickOrder::SetPrice()
{
	if (0 == _stricmp(m_chCommID, ""))
	{
		return;
	}

	// ���Ӧ��Ʒ��Ϣ
	int i = 0;
	int iPointCount = 0;
	T_TradeMerchInfo stOutComm;
	for (i=0; i<m_QueryCommInfoResultVector.size(); i++)// ����Ʒ��Ϣ���˼۸񲨶�
	{
		stOutComm = m_QueryCommInfoResultVector[i];
		string strCode;
		UnicodeToUtf8(stOutComm.stock_code, strCode);
		if (_stricmp(m_chCommID, strCode.c_str())==0) //����ʹ��붼��ͬ
		{
			m_iHandMax = stOutComm.max_entrust_amount; // �������ί��
			m_iHandMin = stOutComm.min_entrust_amount;// ������Сί��
			
			if(0 < stOutComm.default_pdd)
			{
				m_iPointDef = (int)(stOutComm.default_pdd);//�û����۵��Ĭ��ֵ
			}
				
			m_dPriceStep = stOutComm.price_minchange;
  			iPointCount = GetPointCount(stOutComm.price_minchange);			
			m_iPointCount = iPointCount;	//С���������λ

			break;
		}
	}
	
	//���Ӧ������Ϣ
	T_RespQuote stOutQuotation;
	double dBuyPrice = 0;	// ���
	double dSellPrice = 0;	// ����
	for (i=0;i<m_QuotationResultVector.size();i++)// ���������
	{
		stOutQuotation = m_QuotationResultVector[i];
		string strCode;
		UnicodeToUtf8(stOutQuotation.stock_code, strCode);
		if (_stricmp(m_chCommID, strCode.c_str())==0) //��Ʒ������ͬ
		{
			if (_T("1") == stOutComm.price_mode)
			{
				dBuyPrice = stOutQuotation.last_price + stOutComm.buy_pdd*stOutComm.price_minchange;
				dSellPrice = stOutQuotation.last_price + stOutComm.sell_pdd*stOutComm.price_minchange;
			}
			else
			{
				dBuyPrice = stOutQuotation.buy_price + stOutComm.buy_pdd*stOutComm.price_minchange;
				dSellPrice = stOutQuotation.sell_price + stOutComm.sell_pdd*stOutComm.price_minchange;
			}
			break;
		}
	}

	if ((dBuyPrice<0.0000001&&dBuyPrice>-0.0000001) ||(dSellPrice<0.0000001&&dSellPrice>-0.0000001))
	{
		if (_T("1") == stOutComm.price_mode)
		{
			dBuyPrice = stOutComm.last_price + stOutComm.buy_pdd*stOutComm.price_minchange;
			dSellPrice = stOutComm.last_price + stOutComm.sell_pdd*stOutComm.price_minchange;
		}
		else
		{
			dBuyPrice = stOutComm.buy_price + stOutComm.buy_pdd*stOutComm.price_minchange;
			dSellPrice = stOutComm.sell_price + stOutComm.sell_pdd*stOutComm.price_minchange;
		}
	}

	CString strPrice = _T("");
	CString strRight = _T("");
	CString strLeft = _T("");
	//��
	m_strBuyPrice = DoubleToString(dBuyPrice);
	strPrice = Float2String(dBuyPrice, iPointCount);
	if (iPointCount==0)//��������ӿո������Ӳźÿ�
	{
		strLeft += _T(" ");
	}
	m_btnLeft.SetSubCaption(strPrice);
	//��
	m_strSellPrice = DoubleToString(dSellPrice);
	strPrice = Float2String(dSellPrice, iPointCount);
	if (iPointCount==0)//��������ӿո������Ӳźÿ�
	{
		strLeft += _T(" ");
	}
	m_btnRight.SetSubCaption(strPrice);

	bool32 bFindHodeEmpty = FALSE;
	bool32 bFindHodeMany = FALSE;
	T_RespQueryHoldTotal stOutHoldTotal;
	m_iHodingEmptyMax = 0;
	m_iHodingManyMax = 0;
	for (i=0;i<m_QueryHoldSummaryVector.size();i++)
	{
		stOutHoldTotal = m_QueryHoldSummaryVector[i];
		string strCode;
		UnicodeToUtf8(stOutHoldTotal.stock_code, strCode);
		if (_stricmp(m_chCommID, strCode.c_str())==0) //��Ʒ������ͬ
		{
			if (bFindHodeMany && bFindHodeEmpty)
				break;
			CString strTip=L"";
			if (L"S" == stOutHoldTotal.entrust_bs)  
			{
				m_iHodingEmptyMax = stOutHoldTotal.hold_amount;
				strTip.Format(L"����%d��", m_iHodingEmptyMax);
				m_staticHandingEmpty.SetTitleText1(strTip);
				bFindHodeEmpty = TRUE;
			}
			else
			{
				m_iHodingManyMax = stOutHoldTotal.hold_amount;
				strTip.Format(L"����%d��", m_iHodingManyMax);
				m_staticHandingMany.SetTitleText1(strTip);
				bFindHodeMany = TRUE;
			}
		}
	}
	AdjustControlStatus(bFindHodeMany,bFindHodeEmpty);
	SetVisiableHoding(m_bOpen);
	
	UpdateData(FALSE);	
	//Invalidate();
}

//��ȡС�����ȷ��λ��
int CDlgQuickOrder::GetPointCount(const double dSpread)
{
	CString strSpread = _T("");
	strSpread.Format(_T("%f"),dSpread);
	
	int iIndex = -1;
	int iRet = 0;
	iIndex=strSpread.Find('.');
	if (iIndex>=0)
	{
		strSpread.TrimRight('0');
		int iLen = strSpread.GetLength();
		if (iLen==iIndex+1)
		{
			//����
            iRet = 0;	
		}
		else
		{
			//С��
			iRet = iLen-2;
		}
	}
	
	return iRet;
}

string CDlgQuickOrder::DoubleToString(double d)
{
    //Need #include <sstream> 
    using namespace std;
    string str;
    stringstream ss;
    ss<<d;
    ss>>str;
    return str;
}

BOOL CDlgQuickOrder::PreTranslateMessage(MSG* pMsg)
{
	if (WM_SYSKEYDOWN == pMsg->message || WM_KEYDOWN == pMsg->message)
	{
		if ( VK_F12 == pMsg->wParam )
		{
			CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

			CIoViewBase* p = pMainFrame->FindActiveIoView();
			if (NULL != p)
			{
				pMainFrame->OnQuickTrade(2, p->GetMerchXml());
			}	
			return TRUE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

//��ȡ��Ʒ��Ϣ����
CString CDlgQuickOrder::GetCommName(const char* pCommID)
{
	if (NULL==pCommID)
	{
		//ASSERT(0);
		return _T("");
	}
	for (int i=0;i<m_QueryCommInfoResultVector.size();i++)// ����Ʒ��Ϣ���˼۸񲨶�
	{
		T_TradeMerchInfo stOutComm = m_QueryCommInfoResultVector[i];
		string strCode;
		UnicodeToUtf8(stOutComm.stock_code, strCode);
		if (_stricmp(pCommID, strCode.c_str())==0) //����ʹ��붼��ͬ
		{
			return stOutComm.stock_name;
		}
	}
	return _T("");
}

//�رհ�ť
void CDlgQuickOrder::OnBtnClose()
{
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrTradeOpenSection, KStrTradeOpenKeyCheck, m_bOpenCheck ? _T("1") : _T("0"));
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrTradeCloseSection, KStrTradeCloseKeyCheck, m_bCloseCheck ? _T("1") : _T("0"));

	ShowWindow(SW_HIDE);
}

int CDlgQuickOrder::WarningMsg(CString &strWarn, E_QuickEntrustType eType)
{
	CString str = _T("");
	string strTemp;
	if ((EET_OPEN_BUY==eType) || (EET_OPEN_SELL==eType))  // ����
	{
		//�ж�����
		m_editHand.GetWindowText(str);
		UnicodeToUtf8(str, strTemp);
		int iQty = atoi(strTemp.c_str());
		if (iQty>m_iHandMax||iQty<m_iHandMin)
		{
			strWarn.Format(_T("��������ȷ�Ľ��������� \r\n����������С%d�֡����%d�֣�"),m_iHandMin,m_iHandMax);
			return -1;
		}
	}
	else if (EET_CLOSE_BUY==eType) // ƽ��
	{
		if (0>=m_iHodingEmptyMax)
		{
			strWarn.Format(_T("��ǰ��������Ϊ0��"));
			return -1;
		}
		int iMinHand = 1;
		m_editHand.GetWindowText(str);
		UnicodeToUtf8(str, strTemp);
		int iQty = atoi(strTemp.c_str());
		if (iQty>m_iHodingEmptyMax || iQty<iMinHand)
		{
			strWarn.Format(_T("��������ȷ�Ľ��������� \r\n����������С%d�֡����%d�֣�"), iMinHand, m_iHodingEmptyMax);
			return -1;
		}
	}
	else if (EET_CLOSE_SELL==eType) // ƽ��
	{
		if (0>=m_iHodingManyMax)
		{
			strWarn.Format(_T("��ǰ��������Ϊ0��"));
			return -1;
		}
		int iMinHand = 1;
		m_editHand.GetWindowText(str);
		UnicodeToUtf8(str, strTemp);
		int iQty = atoi(strTemp.c_str());
		if (iQty>m_iHodingManyMax || iQty<iMinHand)
		{
			strWarn.Format(_T("��������ȷ�Ľ��������� \r\n����������С%d�֡����%d�֣�"), iMinHand, m_iHodingManyMax);
			return -1;
		}
	}
			
	return 0;
}

BOOL CDlgQuickOrder::SetBkTitleBitmap(int nResourceID)
{
	BOOL bRet = FALSE;
	if (nResourceID > 0)
	{
		bRet = m_bmpbkTitle.LoadBitmap(nResourceID);
		if (bRet)
		{
			m_bmpbkTitle.GetBitmap(&m_bmpbkInfo);
		}
	}
	return bRet;
}

BOOL CDlgQuickOrder::SetTitleIconRes(int nResourceID)
{
	BOOL bRet = FALSE;
	if (nResourceID > 0)
	{
		bRet = m_bmpTitleIcon.LoadBitmap(nResourceID);
		if (bRet)
		{
			m_bmpTitleIcon.GetBitmap(&m_bmpIconInfo);
		}
	}
	return bRet;
}

BOOL CDlgQuickOrder::SetTitleCloseRes(int nResourceID)
{
	BOOL bRet = FALSE;
	if (nResourceID > 0)
	{
		bRet = m_bmpTitleClose.LoadBitmap(nResourceID);
		if (bRet)
		{
			m_bmpTitleClose.GetBitmap(&m_bmpIconCloseInfo);
		}
	}
	return bRet;
}

void CDlgQuickOrder::OnTimer(UINT nIDEvent)
{
	if (TIME_CLEAR_TIP == nIDEvent)  // ���5s�ر�tip
	{
		KillTimer(TIME_CLEAR_TIP);
	}
	CDialog::OnTimer(nIDEvent);
}

LRESULT CDlgQuickOrder::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���
	LRESULT lRet =  CDialog::WindowProc(message, wParam, lParam);
	COptionButton::HandleMessage(this,message,wParam,lParam);
	return lRet;
}

LRESULT CDlgQuickOrder::OnTabExchange(WPARAM wParam, LPARAM lParam)
{
	int nID = wParam;
	switch (nID)
	{
	case IDC_BTN_QUICK_ORDER_OPEN:
		m_bOpen = TRUE;
		SetOpen(m_bOpen);
		break;
	case IDC_BTN_QUICK_ORDER_CLOSE:
		m_bOpen = FALSE;
		SetOpen(m_bOpen);
		break;
	}

	return 0;
}

void CDlgQuickOrder::AdjustControlPos()
{
	CRect rtClient;
	GetClientRect(rtClient);
	int nVOffset = 0;
	int nHOffset = 0;
	int nMiddlePos = rtClient.Width()/2;

	CRect rtTitle(rtClient.left,rtClient.top,rtClient.right,rtClient.top+32);
	m_btnTitleBK.SetRect(rtTitle);

	CRect rtIcon(rtClient.left+5,rtClient.top+5,rtClient.left+5+12,rtClient.top+5+17);
	m_btnIcon.SetRect(rtIcon);

	CRect rtTitleText(rtIcon.right,rtIcon.top,rtIcon.right+60,rtIcon.top+20);
	m_btnTitleText.SetRect(rtTitleText);

	// �رհ�ť
	m_btnTitleExit.MoveWindow(rtClient.right-11-10,rtClient.top+10,11,11);
	nVOffset = rtClient.top+ID_TITLE_HEIGHT;

	// tab��ǩ��ť
	m_btnOpen.SetRect(rtClient.left,nVOffset,nMiddlePos,nVOffset+IDC_TAB_HEIGHT);
	m_btnClose.SetRect(nMiddlePos,nVOffset,rtClient.right,nVOffset+IDC_TAB_HEIGHT);
	nVOffset += IDC_TAB_HEIGHT;

	// ��Ʒ��̬��
	nVOffset += 15;
	nHOffset = rtClient.left+20;
	CRect rtGoodsNameTip(nHOffset,nVOffset,nHOffset+40,nVOffset+20);
	m_btnGoodsNameTip.SetRect(rtGoodsNameTip);

	nHOffset = nHOffset + 45;
	CRect rtGoodsName(nHOffset,nVOffset,nHOffset+150,nVOffset+20);
	m_btnGoodsName.MoveWindow(rtGoodsName);

	// ������̬��
	nHOffset = rtClient.left + 20;
	CRect rtHandTip(rtGoodsNameTip.left,rtGoodsNameTip.bottom+10,rtGoodsNameTip.left+40,rtGoodsNameTip.bottom+10+20);
	m_btnHandTip.SetRect(rtHandTip);

	// ����ť
	CRect rtSub(rtGoodsNameTip.right+5,rtGoodsNameTip.bottom+5,rtGoodsNameTip.right+5+23,rtGoodsNameTip.bottom+5+26);
	m_btnSub.SetRect(rtSub);

	// ����򱳾�ͼ
	CRect rtHandBK(rtSub.right,rtSub.top,rtSub.right+30,rtSub.bottom);
	m_btnEditBK.SetTextBkgColor(RGB(255,255,255),RGB(255,255,255),RGB(255,255,255));
	m_btnEditBK.SetRect(rtHandBK);
	
	// �����
	CRect rtEdit(rtSub.right,rtSub.top+3,rtSub.right+30,rtSub.bottom+1);
	//int nHeight = rtEdit.Height();
	m_editHand.MoveWindow(rtEdit);
	CRect rcEditClient;
	m_editHand.GetClientRect(&rcEditClient);
	rcEditClient.DeflateRect(0,3,0,3);
	m_editHand.SetRectNP(&rcEditClient);
	

	// �Ӱ�ť
	CRect rtAdd(rtSub.right+30,rtSub.top,rtSub.right+30+26,rtSub.bottom);
	m_btnAdd.SetRect(rtAdd);

	// ��߽��װ�ť
	CRect rtLeftBtn(rtHandTip.left,rtHandTip.bottom+10,rtHandTip.left+85,rtHandTip.bottom+10+46);
	m_btnLeft.SetRect(rtLeftBtn);

	// �ұ߽��װ�ť
	CRect rtRightBtn(rtLeftBtn.right+5,rtLeftBtn.top,rtLeftBtn.right+5+85,rtLeftBtn.bottom);
	m_btnRight.SetRect(rtRightBtn);

	// �ֿ�
	CRect rtHodingEmpty(rtLeftBtn.left,rtLeftBtn.bottom+5,rtLeftBtn.right,rtLeftBtn.bottom+5+20);
	m_staticHandingEmpty.MoveWindow(rtHodingEmpty);
	//m_staticHandingEmpty.MoveWindow(rtHodingEmpty);

	// �ֶ�
	CRect rtHodingMany(rtHodingEmpty.right+5,rtHodingEmpty.top,rtHodingEmpty.right+5+87,rtHodingEmpty.bottom);
	m_staticHandingMany.MoveWindow(rtHodingMany);



	//// �ֶ�
	//CRect rtHodingMany(rtLeftBtn.left,rtLeftBtn.bottom+5,rtLeftBtn.right,rtLeftBtn.bottom+5+20);
	//m_staticHandingMany.MoveWindow(rtHodingMany);

	//// �ֿ�
	//CRect rtHodingEmpty(rtHodingMany.right+5,rtHodingMany.top,rtHodingMany.right+5+87,rtHodingMany.bottom);
	//m_staticHandingEmpty.MoveWindow(rtHodingEmpty);

	// checkbox
	m_btnCheckbox.MoveWindow(20,rtHodingEmpty.bottom+10,90,13);
	
}

void CDlgQuickOrder::SetOpen( BOOL bOpen )
{
	// �����ǰû��ѡ���κ���Ʒ����m_strCommID��ֵΪ�գ����� ѡ�����Ʒ���ǽ�����Ʒ����m_bTradeComm��ֵΪFALSE�������װ�ťҪ����
	if (m_strCommID.IsEmpty() || m_bTradeComm == FALSE)
	{
		EnableTradeButton(FALSE);
		if (bOpen)
		{
			m_staticHandingEmpty.ShowWindow(SW_HIDE);
			m_staticHandingMany.ShowWindow(SW_HIDE);
			m_btnCheckbox.SetChecked(m_bOpenCheck);
		}
		else
		{
			m_staticHandingEmpty.ShowWindow(SW_SHOW);
			m_staticHandingMany.ShowWindow(SW_SHOW);
			m_btnCheckbox.SetChecked(m_bCloseCheck);
		}
		return;
	}

	if (bOpen)
	{
		m_btnCheckbox.SetChecked(m_bOpenCheck);
		m_btnLeft.SetCaption(_T("����"));
		m_btnRight.SetCaption(_T("����"));
		m_staticHandingEmpty.ShowWindow(SW_HIDE);
		m_staticHandingMany.ShowWindow(SW_HIDE);
		m_editHand.SetWindowText(_T("1"));
		EnableTradeButton(TRUE);
	}
	else
	{
		m_btnCheckbox.SetChecked(m_bCloseCheck);
		m_editHand.SetWindowText(_T("1"));
		if (m_iHodingEmptyMax == 0)
			m_btnLeft.EnableButton(FALSE,TRUE);
		else
			m_btnLeft.EnableButton(TRUE,TRUE);

		if (m_iHodingManyMax == 0)
			m_btnRight.EnableButton(FALSE,TRUE);
		else
			m_btnRight.EnableButton(TRUE,TRUE);

		m_btnLeft.SetCaption(_T("����"));
		m_btnRight.SetCaption(_T("����"));
		m_staticHandingEmpty.ShowWindow(SW_SHOW);
		m_staticHandingMany.ShowWindow(SW_SHOW);
	}
	m_bOpen = bOpen;
}

BOOL CDlgQuickOrder::OnCommand( WPARAM wParam, LPARAM lParam )
{
	int32 iID = (int32)wParam;

	if (iID == IDC_BTN_QUICK_ORDER_ADD)
		OnBtnAdd();
	else if (iID == IDC_BTN_QUICK_ORDER_SUB)
		OnBtnSub();

	if (m_bOpen)
	{
		if (iID == IDC_BTN_QUICK_ORDER_LEFT)// ���
			BuySell(EET_OPEN_BUY);
		else if (iID == IDC_BTN_QUICK_ORDER_RIGHT)// ���
			BuySell(EET_OPEN_SELL);
	}
	else
	{
		if (iID == IDC_BTN_QUICK_ORDER_LEFT)// ƽ��
			BuySell(EET_CLOSE_BUY);
		else if (iID == IDC_BTN_QUICK_ORDER_RIGHT)// ƽ��
			BuySell(EET_CLOSE_SELL);
	}

	return CDialog::OnCommand(wParam,lParam);
}

void CDlgQuickOrder::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_btnLeft.PtInButton(point))
		m_btnLeft.LButtonDown();
	else if (m_btnRight.PtInButton(point))
		m_btnRight.LButtonDown();
	else if (m_btnSub.PtInButton(point))
		m_btnSub.LButtonDown();
	else if (m_btnAdd.PtInButton(point))
		m_btnAdd.LButtonDown();

	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgQuickOrder::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_btnLeft.PtInButton(point))
		m_btnLeft.LButtonUp();
	else if (m_btnRight.PtInButton(point))
		m_btnRight.LButtonUp();
	else if (m_btnSub.PtInButton(point))
		m_btnSub.LButtonUp();
	else if (m_btnAdd.PtInButton(point))
		m_btnAdd.LButtonUp();

	CDialog::OnLButtonUp(nFlags, point);
}

void CDlgQuickOrder::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_btnLeft.PtInButton(point))
		m_btnLeft.MouseHover();
	else
		m_btnLeft.MouseLeave();

	if (m_btnRight.PtInButton(point))
		m_btnRight.MouseHover();
	else
		m_btnRight.MouseLeave();

	if (m_btnSub.PtInButton(point))
		m_btnSub.MouseHover();
	else
		m_btnSub.MouseLeave();

	if (m_btnAdd.PtInButton(point))
		m_btnAdd.MouseHover();
	else
		m_btnAdd.MouseLeave();

	CDialog::OnMouseMove(nFlags, point);
}

void CDlgQuickOrder::EnableTradeButton( BOOL bEnable )
{
	m_btnLeft.EnableButton(bEnable,TRUE);
	m_btnRight.EnableButton(bEnable,TRUE);
}

void CDlgQuickOrder::SetVisiableHoding(BOOL bOpen)
{
	if (bOpen)
	{
		m_staticHandingEmpty.ShowWindow(SW_HIDE);
		m_staticHandingMany.ShowWindow(SW_HIDE);
	}
	else
	{
		m_staticHandingEmpty.ShowWindow(SW_SHOW);
		m_staticHandingMany.ShowWindow(SW_SHOW);
	}
}

CString CDlgQuickOrder::GetTradeTipMsg( E_QuickEntrustType entrustType )
{
	CString strTipMsg;
	CString strGoodsName,strPriceType,strPrice,strDirection,strCount;
	m_btnGoodsName.GetTitleText1(strGoodsName);
	m_editHand.GetWindowText(strCount);

	TCHAR strName[50]={0};
	switch (entrustType)
	{
	case EET_OPEN_BUY:
		MultiCharCoding2Unicode(EMCCUtf8,m_strBuyPrice.c_str(),m_strBuyPrice.length(),strName,sizeof(strName)/sizeof(TCHAR));
		strPrice.Format(_T("%s"),strName);
		strDirection = _T("��������");
		break;
	case EET_OPEN_SELL:
		MultiCharCoding2Unicode(EMCCUtf8,m_strSellPrice.c_str(),m_strSellPrice.length(),strName,sizeof(strName)/sizeof(TCHAR));
		strPrice.Format(_T("%s"),strName);
		strDirection = _T("��������");
		break;
	case EET_CLOSE_BUY:
		MultiCharCoding2Unicode(EMCCUtf8,m_strBuyPrice.c_str(),m_strBuyPrice.length(),strName,sizeof(strName)/sizeof(TCHAR));
		strPrice.Format(_T("%s"),strName);
		strDirection = _T("ƽ������");
		break;
	case EET_CLOSE_SELL:
		MultiCharCoding2Unicode(EMCCUtf8,m_strSellPrice.c_str(),m_strSellPrice.length(),strName,sizeof(strName)/sizeof(TCHAR));
		strPrice.Format(_T("%s"),strName);
		strDirection = _T("ƽ������");
		break;
	}
	strTipMsg.Format(_T("\r\n��Ʒ: %s \r\n��Ʒ�۸�: %s \r\n��Ʒ����: %s \r\n������ʽ: %s \r\n\r\nȷ���µ���?"),
		strGoodsName.GetBuffer(),strPrice.GetBuffer(),strCount.GetBuffer(),strDirection.GetBuffer());
	strGoodsName.ReleaseBuffer();
	strPrice.ReleaseBuffer();
	strCount.ReleaseBuffer();
	strDirection.ReleaseBuffer();

	return strTipMsg;
}

void CDlgQuickOrder::AdjustControlStatus(BOOL bFindMany,BOOL bFindEmpty)
{
	if (!m_bOpen)
	{
		// �ֿ�
		if (bFindEmpty)
		{
			m_btnLeft.EnableButton(TRUE,TRUE);
			CString str;
			str.Format(_T("����%d��"),m_iHodingEmptyMax);
			m_staticHandingEmpty.SetTitleText1(str);
		}
		else
		{
			m_btnLeft.EnableButton(FALSE,TRUE);
			m_staticHandingEmpty.SetTitleText1(_T("����0��"));
			m_staticHandingEmpty.ShowWindow(SW_SHOW);
		}

		// �ֶ�
		if (bFindMany)
		{
			CString str;
			str.Format(_T("����%d��"),m_iHodingManyMax);
			m_staticHandingMany.SetTitleText1(str);
			m_btnRight.EnableButton(TRUE,TRUE);
		}
		else
		{
			m_btnRight.EnableButton(FALSE,TRUE);
			m_staticHandingMany.SetTitleText1(_T("����0��"));
			m_staticHandingMany.ShowWindow(SW_SHOW);
		}
	}
}

void CDlgQuickOrder::ReadCheck()
{
	CString strCheck;
	CEtcXmlConfig::Instance().ReadEtcConfig(KStrTradeOpenSection, KStrTradeOpenKeyCheck, NULL, strCheck);
	if (strCheck.IsEmpty())
	{
		m_bOpenCheck = 1;
	}
	else
	{
		if (strCheck == _T("0"))
			m_bOpenCheck = FALSE;
		else
			m_bOpenCheck = TRUE;
	}

	CEtcXmlConfig::Instance().ReadEtcConfig(KStrTradeCloseSection, KStrTradeCloseKeyCheck, NULL, strCheck);
	if (strCheck.IsEmpty())
	{
		m_bCloseCheck = 1;
	}
	else
	{
		if (strCheck == _T("0"))
			m_bCloseCheck = FALSE;
		else
			m_bCloseCheck = TRUE;
	}
	if (m_bOpen)
		m_btnCheckbox.SetChecked(m_bOpenCheck);
	else
		m_btnCheckbox.SetChecked(m_bCloseCheck);
}

void CDlgQuickOrder::OnCheckBoxClicked()
{
	BOOL bChecked = m_btnCheckbox.GetChecked();
	if (m_bOpen)
		m_bOpenCheck = bChecked;
	else
		m_bCloseCheck = bChecked;
}
