// dlgloginnotify.cpp : implementation file
//

#include "stdafx.h"
#include "dlgloginnotify.h"


#include "PathFactory.h"
#include "FontFactory.h"
#include "coding.h"

#include <string>
using std::string;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INVALID_ID			-1

/////////////////////////////////////////////////////////////////////////////
// CDlgLoginNotify dialog
static const TCHAR KStrXMLFileName[]				=	_T("user_notifications.xml");
static const char KStrXMLRootName[]					=	"XMLDATA"; // ���ڵ�
static const char KStrXMLRootElementVersion[]		=	"version";			// ���ڵ� �汾
static const char KStrXMLRootElementApp[]			=	"app";			// ���ڵ� app
static const char KStrXMLRootElementData[]			=	"data";			// ���ڵ� data

static const char KStrXMLRootElementVersionDef[]	=	"1.0";			// ���ڵ� �汾 Ĭ��ֵ
static const char KStrXMLRootElementAppDef[]		=	"ggtong";			// ���ڵ� app default
static const char KStrXMLRootElementDataDef[]		=	"notification";			// ���ڵ� data default

static const char KStrXMLElementNodeTime[]			=	"time";		// ʱ��ڵ�����
static const char KStrXMLElementNodeNotice[]		=	"notification";		// �ڵ�����

static const char KStrXMLElementAttriTitle[]		=	"title";		// ��������
static const char KStrXMLElementAttriMsg[]			=	"message";     // ��Ϣ����
static const char KStrXMLElementAttriBeShow[]		=	"isShow";     // �Ƿ���ʾ

static const char KStrXMLElementAttriTime[]			=	"time";     // ʱ������


CDlgLoginNotify::CDlgLoginNotify(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgLoginNotify::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLoginNotify)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_StrMsg	= L"";
	m_StrTitle	= L"";	

	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	m_StrXMLFileFullName = CPathFactory::GetPrivateConfigPath(pDoc->m_pAbsCenterManager->GetUserName());
	if ( !_tcheck_if_mkdir(m_StrXMLFileFullName.GetBuffer(1024)) )
	{
		// ���û�и�Ŀ¼�����ó����Լ�ѡ��current directory
		m_StrXMLFileFullName.ReleaseBuffer();
		m_StrXMLFileFullName.Empty();
	}
	else
	{
		m_StrXMLFileFullName.ReleaseBuffer();
	}
	m_StrXMLFileFullName +=	KStrXMLFileName;

	m_iXButtonHovering = INVALID_ID;
	m_pImgCheck = NULL;
	m_pImgUnCheck = NULL;
}

CDlgLoginNotify::~CDlgLoginNotify()
{
	DEL(m_pImgCheck);
	DEL(m_pImgUnCheck);
} 

void CDlgLoginNotify::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLoginNotify)
	DDX_Control(pDX, IDC_RICHEDIT, m_RichEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLoginNotify, CDialogEx)
	//{{AFX_MSG_MAP(CDlgLoginNotify)
	ON_NOTIFY(EN_LINK, IDC_RICHEDIT, OnRichEditExLink)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLoginNotify message handlers

BOOL CDlgLoginNotify::OnInitDialog()
{
	CDialog::OnInitDialog();

	unsigned int mask = ::SendMessage(m_RichEdit.m_hWnd, EM_GETEVENTMASK, 0, 0);   
	::SendMessage(m_RichEdit.m_hWnd, EM_SETEVENTMASK,  0, mask | ENM_LINK | ENM_MOUSEEVENTS | ENM_SCROLLEVENTS | ENM_KEYEVENTS);   
	::SendMessage(m_RichEdit.m_hWnd, EM_AUTOURLDETECT, true, 0);  

	m_beDoNotTipToday = true; //Ĭ��Ϊdonot tip today

	//lxp modify 2012-11
	//���ÿؼ����ұ߾�
	RECT R; 
	m_RichEdit.SendMessage(EM_GETRECT, 0, (LPARAM)&R); 
	R.left  += 20; 
	R.right -= 20;
	m_RichEdit.SendMessage(EM_SETRECT, 0, (LPARAM)&R); 
	//lxp modify end

	CRect rcClient, rcEdit;
	GetClientRect(&rcClient);
	m_RichEdit.GetClientRect(&rcEdit);
	rcEdit.right = rcClient.right + 19; // ���ع�����
	m_RichEdit.MoveWindow(rcEdit);

	CRect rcHide(0,0,0,0);
	GetDlgItem(IDC_BTN_FOCUS)->MoveWindow(rcHide);  // ��ȡ�Ի��򽹵�Ŀؼ�

	m_pImgCheck = Image::FromFile(CPathFactory::GetImageNotifyCheckPath());
	m_pImgUnCheck = Image::FromFile(CPathFactory::GetImageNotifyUnCheckPath());
	CRect rcBtn;
	rcBtn.left = rcEdit.left;
	rcBtn.right = rcBtn.left + m_pImgCheck->GetWidth();
	rcBtn.top = rcEdit.bottom + 4;
	rcBtn.bottom = rcBtn.top + m_pImgCheck->GetHeight()/3;
	AddButton(rcBtn, m_pImgCheck, 3, NOTIFY_BTN_TIPTODAY, L"���ղ�����ʾ");	// ���ղ�����ʾ

	rcBtn.left = rcEdit.right - 90;
	rcBtn.right = rcBtn.left + 70;
 	AddButton(rcBtn, NULL, 3, NOTIFY_BTN_OK, L"�ر�");	// �ر�

	m_RichEdit.SetBackgroundColor(FALSE, RGB(54,64,76));
	ShowWindow(SW_HIDE); // ������
	return TRUE;
}

void CDlgLoginNotify::OnPaint() 
{
	CPaintDC dc(this);	
	CRect rcClient;
	GetClientRect(&rcClient);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	memDC.SelectObject(&bmp);
	memDC.FillSolidRect(rcClient, 0x3e342c);
	memDC.SetBkMode(TRANSPARENT);
	Gdiplus::Graphics graphics(memDC.GetSafeHdc());
	
	CRect rcPaint;
	dc.GetClipBox(&rcPaint);
	map<int, CNCButton>::iterator iter;
	CRect rcControl;
	
	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		btnControl.GetRect(rcControl);
		
		// �жϵ�ǰ��ť�Ƿ���Ҫ�ػ�
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}
		
		btnControl.DrawButton(&graphics);
	}
	
	dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);
	dc.SelectClipRgn(NULL);
	//
	memDC.DeleteDC();
	bmp.DeleteObject();
}

HBRUSH CDlgLoginNotify::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);	
	return hbr;
}

void CDlgLoginNotify::SetNotifyMsg(const CString& StrTitile, const CString& StrMsg)
{
	CString strReplay = StrMsg;
	strReplay.Replace(L"\\n", L"\r\n");

	m_StrMsg			= strReplay;
	m_StrTitle			= StrTitile;
}

/*
static void GetCharacternNums(const CString& StrIn, int32& iEngNums, int32& iChnNums)
{
	iEngNums = 0;
	iChnNums = 0;
	if ( 0 == StrIn.GetLength() )
	{
		return;
	}
	
	//
	for(int32 i = 0; i < StrIn.GetLength(); i++)   		
	{   	
		TCHAR ch = StrIn.GetAt(i);
		
		if( ch  <   0x80   )      			
		{   
			//�ַ�
			iEngNums += 1;
		}   
		else   
		{
			//����
			//��λ��i��i++Ϊһ������ 		
			iChnNums+= 1;
		}   		
	}		
}
*/

void CDlgLoginNotify::InitialDisplay()
{ 
/*	CString StrEnd = L"\t";

	CString StrTitile(m_StrTitle);
	StrTitile  = L"\r\n" + StrTitile;
	StrTitile += L"\r\n";

	StrEnd =  L"\r\n" + StrEnd;
	StrEnd =  L"\r\n" + StrEnd;

	//
	CString StrShow = StrTitile + m_StrMsg + StrEnd;
	
	int iLen = StrShow.GetLength();

	int iT = StrShow.Find(StrTitile);
	int iM = StrShow.Find(m_StrMsg, iT);
	StrShow.Insert(iM, L"\r\n");
	
	int iE = StrShow.Find(StrEnd, iM);
	StrShow.Insert(iE, L"\r\n");
	
	int iLenNow = StrShow.GetLength();
	
	m_RichEdit.SetWindowText(StrShow);
	UpdateData(TRUE);
	
	m_RichEdit.SetSel(0, -1);
	m_RichEdit.SetParagraphLeft();

	m_RichEdit.SetSel(0, 3);		
	m_RichEdit.SetParagraphCenter();

	int32 iChinese = 0;
	int32 iEnglish = 0;
	GetCharacternNums(StrShow, iEnglish, iChinese);

	int iEnd = StrShow.GetLength() + iChinese;

	m_RichEdit.SetSel(iEnd - 2, iEnd - 1);
	m_RichEdit.SetParagraphRight();

	m_RichEdit.SetSel(0, 0);
*/
	//lxp modify 2012-11-19
		
	//����ģ���ļ�\\CONFIG\\notifyfiletemp_ch.rtf(����)notifyfiletemp_zh.rtf(����)
	m_RichEdit.ReadRtfFile( CPathFactory::GetPublicNotifyMsgTemp() );

// 	CString StrContent;
// 	m_RichEdit.GetWindowText(StrContent);
// 	m_RichEdit.SetWindowText(StrContent);	// �����ִ���Ҳ��������ʽ����

	long nbPos = -1,nePos = -1;
	//�滻ǩ��
	/*int nRet = DowithReplaceStr(L"signature",strSign,nbPos,nePos);
	if( nRet )
	{

	}
	*/
	//�滻����
	int nRet = DowithReplaceStr(L"context",m_StrMsg,nbPos,nePos);
	if( nRet )
	{
		//�����������ɫ
		if( nbPos>=0 && nePos>=0 && nePos > nbPos)
		{
			CFontNode fontNode;
			fontNode.m_stLogfont.lfHeight = 12;
			wcscpy(fontNode.m_stLogfont.lfFaceName, gFontFactory.GetExistFontName(L"΢���ź�"));	//...
			SetFont(nbPos, nePos, fontNode, 0xdbd9d7, FALSE);
		}
	}

	//DowithReplaceStr(L"time",m_StrTitle);
	//�滻����
	nRet = DowithReplaceStr(L"title",m_StrTitle,nbPos,nePos);
	if( nRet )
	{
		//���ñ����������ɫ
		if( nbPos>=0 && nePos>=0 && nePos > nbPos)
		{
			CFontNode fontNode;
			fontNode.m_stLogfont.lfHeight = 14;
			wcscpy(fontNode.m_stLogfont.lfFaceName, gFontFactory.GetExistFontName(L"΢���ź�"));	//..
			SetFont(nbPos, nePos, fontNode, RGB(255,255,255), FALSE);
		}
	}
	//lxp modify end

	UpdateData(FALSE);	
	
	m_beDoNotTipToday = true;
}

void CDlgLoginNotify::PostNcDestroy()
{
	m_StrMsg.Empty();
	m_StrTitle.Empty();
	CDialogEx::PostNcDestroy();
}

void CDlgLoginNotify::OnCancel()
{
	SaveDoNotTipToday();
	ShowWindow(SW_HIDE);
	DestroyWindow();	
}

void CDlgLoginNotify::OnRichEditExLink(NMHDR* in_pNotifyHeader, LRESULT* out_pResult)   
{   
	ENLINK* pENLink = (ENLINK*)in_pNotifyHeader;   
	*out_pResult = 0;   
	
	switch(pENLink->msg)   
	{   
	case WM_LBUTTONDOWN:   
		{   
			CString   StrUrl;   
			CHARRANGE stCharRange;   
			
			CRichEditCtrl * pTempEdit = (CRichEditCtrl*)CRichEditCtrl::FromHandle(pENLink->nmhdr.hwndFrom);
			if ( NULL == pTempEdit )
			{
				return;
			}
			
			pTempEdit->GetSel(stCharRange);   
			pTempEdit->SetSel(pENLink->chrg);
			
			StrUrl = pTempEdit->GetSelText();   
			pTempEdit->SetSel(stCharRange);   
			
			CWaitCursor WaitCursor;   
			
			ShellExecute(this->GetSafeHwnd(), L"open", StrUrl, NULL, NULL, SW_SHOWNORMAL);   
			*out_pResult   =   1   ;   
		}   
		break;   
	}   
}

void CDlgLoginNotify::SaveDoNotTipToday()
{
	// ��ʾ�ˣ���Ҫ֪���û��Ƿ��Ѿ���ѡ�˽��ղ���ʾ�������ѡ�ˣ��������Ϣ��û�еĻ����Ϳ��Բ�������
	if ( m_beDoNotTipToday )
	{
		TiXmlDocument tiDoc;
		TiXmlElement  *pTiEleNotice = FindNoticeElementInXml(tiDoc, m_beDoNotTipToday, m_beDoNotTipToday);
		if ( NULL != pTiEleNotice )
		{
			// �޸ĸýڵ��show����
			pTiEleNotice->SetAttribute(KStrXMLElementAttriBeShow , m_beDoNotTipToday ? "0" : "1");
			string strFileNameUtf8;
			UnicodeToUtf8(m_StrXMLFileFullName, strFileNameUtf8);
			tiDoc.SaveFile(strFileNameUtf8.c_str());
		}
	}
}

TiXmlElement  * CDlgLoginNotify::InitialTiXml( TiXmlDocument &tiDoc )
{
	string	StrFileNameUtf8;
	if ( !UnicodeToUtf8(m_StrXMLFileFullName, StrFileNameUtf8) )
	{
		ASSERT( 0 );
	}

	if ( NULL == StrFileNameUtf8.c_str() )
	{
		uint32 uXMLFileNameLen = sizeof(KStrXMLFileName[0]);
		uint32 uWcharLen = sizeof(wchar_t);
		if (uXMLFileNameLen == uWcharLen)
		{
			UnicodeToUtf8(KStrXMLFileName, StrFileNameUtf8);
		}
		else
		{
			StrFileNameUtf8 = (const char *)KStrXMLFileName;
		}
	}

	if ( !tiDoc.LoadFile(StrFileNameUtf8.c_str(), TIXML_ENCODING_UTF8) )
	{
		tiDoc.Clear();
	}
	
	tiDoc.ClearError();

	TiXmlElement *pTiRoot = (TiXmlElement *)tiDoc.FirstChildElement(KStrXMLRootName);
	if ( NULL == pTiRoot )
	{
		tiDoc.Clear();
		
		// ��Ӹ��ڵ�
		TiXmlDeclaration	tiDecl("1.0", "utf-8", "yes");
		tiDoc.InsertEndChild(tiDecl);
		
		TiXmlElement	tiEleRoot(KStrXMLRootName);
		tiEleRoot.SetAttribute(KStrXMLRootElementVersion, KStrXMLRootElementVersionDef);
		tiEleRoot.SetAttribute(KStrXMLRootElementApp,	   KStrXMLRootElementAppDef);
		tiEleRoot.SetAttribute(KStrXMLRootElementData,    KStrXMLRootElementDataDef);
		pTiRoot = (TiXmlElement *)tiDoc.InsertEndChild(tiEleRoot);
	}

	ASSERT( NULL != pTiRoot );
	return pTiRoot;
}

TiXmlElement  * CDlgLoginNotify::FindNoticeElementInXml(INOUT TiXmlDocument &tiDoc, 
														bool32 bAddNoticeIfNotExist /*= false*/, 
														bool32 bDefaultDoNotTip /*= false*/)
{
	TiXmlElement *pTiRoot = tiDoc.FirstChildElement(KStrXMLRootName);
	if ( NULL == pTiRoot )
	{
		pTiRoot = InitialTiXml(tiDoc);
	}
	if ( NULL == pTiRoot )
	{
		return NULL;
	}

	TiXmlElement *pTiFind = NULL;
	TiXmlElement *pTiToday = NULL;
	
	ULONG32 tNow = time(NULL);
	string strCurTitle, strCurMsg;
	UnicodeToUtf8(m_StrTitle, strCurTitle);
	UnicodeToUtf8(m_StrMsg,   strCurMsg);
	
	TiXmlElement *pTiEle = pTiRoot->FirstChildElement(KStrXMLElementNodeTime);
	while ( NULL != pTiEle )
	{				
		TiXmlElement *pTiTemp = pTiEle->NextSiblingElement(KStrXMLElementNodeTime);
		
		const char *pStrTime = pTiEle->Attribute(KStrXMLElementAttriTime);
		if ( NULL != pStrTime )
		{
			ULONG32 tRecord = atol(pStrTime);
			if ( tNow/(3600*24) != tRecord/(3600*24) )
			{
				// ���ǽ���Ľڵ㣬ɾ����
				pTiRoot->RemoveChild( pTiEle );
				pTiEle = NULL;
			}
			else
			{				
				pTiToday = pTiEle;

				// ����Ľڵ㣬�����Ƿ��Ѿ��иü�¼
				bool32 bNeedAdd = true;
				TiXmlElement *pTiNotice = pTiEle->FirstChildElement(KStrXMLElementNodeNotice);
				while ( NULL != pTiNotice )
				{
					// ��title���ԣ������ͬ���ٻ�ȡCDATA���ݣ����ڲ�ȡȫ�Ƚϲ��ԣ��Ƿ�ɲ���ֻ�Ƚϱ��⣿
					
					// ���title
					const char *pStrTitle = pTiNotice->Attribute(KStrXMLElementAttriTitle);
					if ( NULL != pStrTitle && strcmp(pStrTitle, strCurTitle.c_str())==0 )
					{
						// titile��ͬ��������
						const char *pStrMsg = pTiNotice->GetText();
						if ( NULL!=pStrMsg && strcmp(pStrMsg, strCurMsg.c_str())==0 )
						{
							// �ҵ��ˣ����Բ���ʾ
							bNeedAdd = false;
							pTiFind = pTiNotice;
							break;
						}
					}
					
					pTiNotice = pTiNotice->NextSiblingElement(KStrXMLElementNodeNotice);
				}
				
				if ( bNeedAdd  && bAddNoticeIfNotExist )
				{
					TiXmlElement tiEle(KStrXMLElementNodeNotice);
					
					if ( bDefaultDoNotTip )
					{
						tiEle.SetAttribute(KStrXMLElementAttriBeShow, "0");  // ��Ҫ��ʾ
					}
					else
					{
						tiEle.SetAttribute(KStrXMLElementAttriBeShow, "1");  // ��Ҫ��ʾ��
					}
					tiEle.SetAttribute(KStrXMLElementAttriTitle, strCurTitle.c_str());
					
					// �ⲿ����Ϣ������xml���洢�ģ�Ӧ����Ϊ�������ļ��洢����ʱ��xml
					TiXmlText tiText(strCurMsg.c_str());
				//	tiText.SetCDATA(true);
					tiEle.InsertEndChild(tiText);
					
					pTiFind = (TiXmlElement *)pTiEle->InsertEndChild(tiEle);
				}
				
				// ����ڵ������ˣ����Բ�������
				break;
			}
		}
		else
		{
			ASSERT( 0 );
		}
		pTiEle = pTiTemp;
	}

	if ( NULL == pTiToday && bAddNoticeIfNotExist )
	{
		// ������û�н���� - ���������
		TiXmlElement tiEleToday(KStrXMLElementNodeTime);
		pTiToday = (TiXmlElement *)pTiRoot->InsertEndChild(tiEleToday);
		ASSERT( NULL!=pTiToday );
		
		if ( NULL != pTiToday)
		{
			char szBuf[20];
			_snprintf(szBuf, sizeof(szBuf), "%d", tNow);
			pTiToday->SetAttribute(KStrXMLElementAttriTime, szBuf);
			TiXmlElement tiEle(KStrXMLElementNodeNotice);

			if ( bDefaultDoNotTip )
			{
				tiEle.SetAttribute(KStrXMLElementAttriBeShow, "0");  // ��Ҫ��ʾ
			}
			else
			{
				tiEle.SetAttribute(KStrXMLElementAttriBeShow, "1");  // ��Ҫ��ʾ��
			}
			tiEle.SetAttribute(KStrXMLElementAttriTitle, strCurTitle.c_str());
			
			TiXmlText tiText(strCurMsg.c_str());
			//tiText.SetCDATA(true);
			tiEle.InsertEndChild(tiText);
			
			pTiFind = (TiXmlElement *)pTiToday->InsertEndChild(tiEle);
		}
	}

	ASSERT( !bAddNoticeIfNotExist || NULL!=pTiFind );
	return pTiFind;
}

TiXmlElement  * CDlgLoginNotify::FindNoticeElementInXml()
{
	TiXmlDocument tiDoc;
	static TiXmlElement sTiEleNotice(KStrXMLElementNodeNotice);
	TiXmlElement *pTiEleFind =   FindNoticeElementInXml(tiDoc);
	if ( NULL != pTiEleFind )
	{
		sTiEleNotice = *pTiEleFind;
		return &sTiEleNotice;
	}
	return NULL;
}

bool32 CDlgLoginNotify::IsDoNotTipTodayInXml()
{
	if ( m_StrTitle.IsEmpty() && m_StrMsg.IsEmpty() )
	{
		// ����û����ʾ�����ݣ���Ҫ��ʾ��
		return true;
	}

	TiXmlElement *pTiNotice = FindNoticeElementInXml();
	if ( NULL != pTiNotice )
	{
		const char *pStrBeShow = pTiNotice->Attribute(KStrXMLElementAttriBeShow);
		if ( NULL != pStrBeShow )
		{
			// ֻ�� == 0��ʱ������û�ѡ���do not tip today
			return atoi(pStrBeShow)==0;
		}
	}
	// ����ʱ������Ҫ��ʾ��
	return false;
}

void CDlgLoginNotify::DisplayNotification( const CString &StrTitle, const CString &StrMsg, bool32 bForceDispaly /*= false*/ )
{
	static CDlgLoginNotify sDlgNotify;
	sDlgNotify.DisplayNotify(StrTitle, StrMsg, bForceDispaly);
}

void CDlgLoginNotify::DisplayNotify( const CString &StrTitle, const CString &StrMsg, bool32 bForceDispaly /*= false*/ )
{
	SetNotifyMsg(StrTitle, StrMsg);
	if ( !bForceDispaly && IsDoNotTipTodayInXml() )
	{
		// ����Ҫ��ʾ
		return;
	}

	// ��ʾ
	if ( !::IsWindow(GetSafeHwnd()) )
	{
		Create(IDD, NULL);
	}


	if ( ::IsWindow(GetSafeHwnd()) )
	{
		InitialDisplay();
		
		CenterWindow();
		ShowWindow(SW_SHOW);
		UpdateWindow();
		BringWindowToTop();
		

		HWND hWndMain = AfxGetMainWnd()->GetSafeHwnd();
		if ( ::IsWindow(hWndMain) && ::GetForegroundWindow() != hWndMain )
		{
			::SetForegroundWindow(AfxGetMainWnd()->GetSafeHwnd());
		}
		SetFocus();
	}
}


//lxp add 2012-11-19
//����ģ���еĹؼ���,���õ�ǰ�����滻���������滻��Id��Χ
bool CDlgLoginNotify::DowithReplaceStr(CString strTemp,CString strText,long &nbPos,long &nePos)
{
	FINDTEXTEX f;
	
	long pos = 0;
	f.chrg.cpMin = pos;
	f.chrg.cpMax = -1;  
	
	std::wstring ss = strTemp.GetBuffer(0);
	//Unicode2MultiChar(CP_ACP, strTemp, ss);
	f.lpstrText = new TCHAR[ss.size() + 1];
	//MultiChar2Unicode(CP_UTF8, ss.c_str(), f.lpstrText);
	//strcpy(f.lpstrText,ss.c_str());
	wcscpy(const_cast<wchar_t*>(f.lpstrText),strTemp.GetBuffer(0));
	
	

	//f.lpstrText = "����";
	pos = this->m_RichEdit.FindText(FR_DOWN, &f);
	if (pos != -1)
	{
		//m_RichEditCtrl.SetFocus();
		m_RichEdit.SetSel(pos,pos+ss.size());
		m_RichEdit.ReplaceSel(strText);

		std::string ss1;
		Unicode2MultiChar(CP_ACP, strText, ss1);	
		nbPos = pos;
		nePos = pos + ss1.size();
		return true;
	}

	nbPos = -1;
	nePos = -1;
	return false;
}

//�����������ɫ
bool  CDlgLoginNotify::SetFont(long nbPos,long nePos,CFontNode fontNode,COLORREF clrText,bool IsBold,bool isItalic,bool isUnderLine)
{
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_BOLD|CFM_CHARSET|CFM_COLOR|CFM_FACE|CFM_ITALIC|CFM_SIZE|CFM_UNDERLINE|CFM_STRIKEOUT;

	if (IsBold)
	{
		cf.dwEffects |= CFE_BOLD;
	}
	if (isItalic)
	{
		cf.dwEffects |= CFE_ITALIC;
	}
	if (isUnderLine)
	{
		cf.dwEffects |= CFE_UNDERLINE;
	}
	/*if (m_bIsStrikeOut)
	{
		cf.dwEffects |= CFM_STRIKEOUT;
	}*/

	cf.yHeight = abs(fontNode.m_stLogfont.lfHeight) * 16;
	cf.crTextColor = clrText;
	cf.bCharSet = (BYTE)fontNode.m_stLogfont.lfCharSet;
	_tcscpy((TCHAR *)cf.szFaceName, fontNode.m_stLogfont.lfFaceName);

	m_RichEdit.SetSel(nbPos, nePos);
	m_RichEdit.SetSelectionCharFormat(cf);
	//m_RichEdit.SetDefaultCharFormat(cf);
	//m_RichEdit.SetWordCharFormat(cf);
	m_RichEdit.SetSel(0, 0);

	return true;
}
//lxp add end

void CDlgLoginNotify::AddButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption)
{
//	ASSERT(pImage);
	CNCButton btnControl; 
	btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
	btnControl.SetTextBkgColor(RGB(54,92,149), RGB(54,92,200), RGB(54,92,200));
	btnControl.SetTextFrameColor(RGB(54,92,149), RGB(54,92,200), RGB(54,92,200));
	btnControl.SetTextColor(RGB(190,190,190), RGB(255,255,255), RGB(255,255,255));
	btnControl.SetClientDC(TRUE);

	if (NOTIFY_BTN_TIPTODAY == nID)
	{
		CPoint pt(15, -2);
		btnControl.SetTextOffPos(pt);
	}
	m_mapBtn[nID] = btnControl;
}

int	 CDlgLoginNotify::TButtonHitTest(CPoint point)
{
	map<int, CNCButton>::iterator iter;
	
	// �������а�ť
	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		
		// ��point�Ƿ����ѻ��Ƶİ�ť������
		if (btnControl.PtInButton(point) && btnControl.IsEnable())
		{
			return btnControl.GetControlId();
		}
	}
	
	return INVALID_ID;
}

void CDlgLoginNotify::OnMouseMove(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);
	
	if (iButton != m_iXButtonHovering)
	{
		if (INVALID_ID != m_iXButtonHovering)
		{
			m_mapBtn[m_iXButtonHovering].MouseLeave();
			m_iXButtonHovering = INVALID_ID;
		}
		
		if (INVALID_ID != iButton)
		{	
			m_iXButtonHovering = iButton;
			m_mapBtn[m_iXButtonHovering].MouseHover();
		}
	}
	
	// ��Ӧ WM_MOUSELEAVE��Ϣ
	TRACKMOUSEEVENT csTME;
	csTME.cbSize	= sizeof (csTME);
	csTME.dwFlags	= TME_LEAVE;
	csTME.hwndTrack = m_hWnd ;		// ָ��Ҫ׷�ٵĴ��� 
	::_TrackMouseEvent (&csTME);	// ����Windows��WM_MOUSELEAVE�¼�֧�� 
	
	CDialog::OnMouseMove(nFlags, point);
}

LRESULT CDlgLoginNotify::OnMouseLeave(WPARAM wParam, LPARAM lParam)       
{     
	if (INVALID_ID != m_iXButtonHovering)
	{
		m_mapBtn[m_iXButtonHovering].MouseLeave();
		m_iXButtonHovering = INVALID_ID;
	}
	
	return 0;       
} 

void CDlgLoginNotify::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		m_mapBtn[iButton].LButtonDown();
	}
}

void CDlgLoginNotify::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags, point);
	CDialog::OnLButtonDblClk(nFlags, point);
}

void CDlgLoginNotify::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		m_mapBtn[iButton].LButtonUp();
	}
}

BOOL CDlgLoginNotify::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int32 iID = (int32)wParam;
	if (NOTIFY_BTN_TIPTODAY == iID)
	{
		m_beDoNotTipToday = !m_beDoNotTipToday;
		if (m_beDoNotTipToday)
		{
			m_mapBtn[iID].SetImage(m_pImgCheck);
		}
		else
		{
			m_mapBtn[iID].SetImage(m_pImgUnCheck);
		}
	}
	
	if (NOTIFY_BTN_OK == iID)
	{
		// ������static�ģ�ɾ���Ժ�Ҳ�����ؽ���
		SaveDoNotTipToday();
		ShowWindow(SW_HIDE);
		DestroyWindow();
	}
	return CDialog::OnCommand(wParam, lParam);
}