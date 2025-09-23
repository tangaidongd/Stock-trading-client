// dlgworkspaceset.cpp : implementation file
//

#include "stdafx.h"
#include "dlgworkspaceset.h"
#include "tinyxml.h"
#include "MPIChildFrame.h"
#include "ShareFun.h"
#include "pathfactory.h"
#include "coding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const CString KStrXmlRootElementAttrDataWorkSpace		= L"WorkSpace";
const CString KStrXmlRootElementAttrDataChildFrame		= L"ChildFrame";
const CString KStrXmlRootElementAttrDataIoViewManager	= L"IoViewManger";

/////////////////////////////////////////////////////////////////////////////
// CDlgWorkSpaceSet dialog

CDlgWorkSpaceSet::CDlgWorkSpaceSet(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgWorkSpaceSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWorkSpaceSet)
	//}}AFX_DATA_INIT
	m_pView = NULL;
	m_eFileType = EFTCount;
}

void CDlgWorkSpaceSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWorkSpaceSet)
	DDX_Control(pDX, IDC_LIST_SHOW, m_ListShow);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgWorkSpaceSet, CDialogEx)
	//{{AFX_MSG_MAP(CDlgWorkSpaceSet)
	ON_LBN_DBLCLK(IDC_LIST_SHOW, OnDblclkListShow)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, OnButtonDefault)
	ON_LBN_SELCHANGE(IDC_LIST_SHOW, OnSelchangeListShow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWorkSpaceSet message handlers
void CDlgWorkSpaceSet::SetFileInfoArray(const CStringArray & aStrFileInfo, const CStringArray & aStrFileTitle)
{
	m_aStrFileInfo.RemoveAll();
	m_aStrFileTitle.RemoveAll();	

	m_aStrFileInfo.Copy(aStrFileInfo);
	m_aStrFileTitle.Copy(aStrFileTitle);
}

void CDlgWorkSpaceSet::OnDblclkListShow() 
{
	int32 iCur = m_ListShow.GetCurSel();

	if ( iCur < 0 || iCur >= m_aStrFileInfo.GetSize() )
		return;

	CString StrFilePath = m_aStrFileInfo.GetAt(iCur);

	if (m_eFileType == CDlgWorkSpaceSet::EFTWorkSpace)
	{
		CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
		pMainFrame->ClearCurrentWorkSpace();
	}

	// �������Լ���������غ���ʾ xl 0531
	//ShowWindow(SW_HIDE); // �ö��Ի�����hide��hide�ᵼ���������л�����̨
	MoveWindow(CRect(0,0,0,0));	// ��С��������ﵽ����Ŀ��

	OpenFile(StrFilePath);

	CDialog::OnOK();

}

void CDlgWorkSpaceSet::OpenFile(CString StrFilePath)
{
	// �������ļ�������·����
	CFileFind file;
	BOOL bExist = file.FindFile(StrFilePath);	
	ASSERT(bExist);	

	// ת����׺��

	int32 iLength = StrFilePath.GetLength();
	CString StrOldName = StrFilePath;
	CString StrNewName = StrFilePath.Left(iLength-4) + L".xml";

	TRY
	{
		CFile::Rename(StrOldName,StrNewName);
	}
	CATCH( CFileException, e )
	{
#ifdef _DEBUG
		afxDump << L"File " << StrOldName << L" not found, cause = "
			<< e->m_cause << L"\n";
#endif
	}
	END_CATCH


		if (bExist)
		{
			char FilePath[MAX_PATH];
			strcpy(FilePath, _Unicode2MultiChar(StrNewName).c_str());

			TiXmlDocument myDocument(FilePath);

			if ( myDocument.LoadFile() == FALSE )
			{
				return;
			}

			TiXmlElement *  pRootElement = myDocument.RootElement();
			CString StrAttridata = pRootElement->Attribute(GetXmlRootElementAttrData());

			pRootElement = pRootElement->FirstChildElement();

			if (  EFTChildFrame == m_eFileType )
			{
				// nothing
			}
			else if (EFTWorkSpace == m_eFileType)
			{
				// ���õ�ǰ������,Ϊ�򿪵��ļ���
				if ( 0 == KStrXmlRootElementAttrDataWorkSpace.CompareNoCase(StrAttridata) )
				{	
					CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
					CGGTongApp * pApp = (CGGTongApp *)AfxGetApp();

					pApp->m_StrCurWsp = StrFilePath;
					pMainFrame->FromXml(FilePath);				
				}
				else
				{
					MessageBox(L"�ļ���������,��ѡ�������������ļ�!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);			
				}
			}
			else if (EFTIoViewManger == m_eFileType)
			{
				if ( 0 == KStrXmlRootElementAttrDataIoViewManager.CompareNoCase(StrAttridata) )
				{
					ASSERT(NULL!=m_pView);
					CString StrTest = pRootElement->Value();
					m_pView->m_IoViewManager.FromXml(pRootElement);	
				}
				else
				{
					MessageBox(L"�ļ���������,��ѡ��������ͼ����ļ�!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);			
				}

			}
			TRY
			{
				CFile::Rename(StrNewName,StrOldName);
			}
			CATCH( CFileException, e )
			{
#ifdef _DEBUG
				afxDump << L"File " << StrOldName << L" not found, cause = "
					<< e->m_cause << L"\n";
#endif
			}
			END_CATCH
		}
		else
		{
			MessageBox(L"�����鿴��ҳ���ļ�������!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
			//... ѡ��һ��Ĭ�ϵ�		
		}
}

void CDlgWorkSpaceSet::SetNewFileType(CDlgWorkSpaceSet::E_FileType eFileType)
{	
	ASSERT(eFileType < CDlgWorkSpaceSet::EFTCount);
	m_eFileType = eFileType;
} 

void CDlgWorkSpaceSet::GetCurrentView(CGGTongView * pView)
{
	ASSERT(NULL!=pView);
	m_pView = pView;
}

void CDlgWorkSpaceSet::OnClose() 
{
	CDialog::OnClose();
}

BOOL CDlgWorkSpaceSet::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_eFileType == CDlgWorkSpaceSet::EFTWorkSpace)
	{
		SetWindowText(L"���й������ļ�");		
	}
	else if (m_eFileType == CDlgWorkSpaceSet::EFTChildFrame )
	{
		SetWindowText(L"����ҳ��ģ���ļ�");
	}
	else if (m_eFileType == CDlgWorkSpaceSet::EFTIoViewManger )
	{
		SetWindowText(L"������ͼģ���ļ�");
	}

	int32 iFileType = (int32)m_eFileType;

	for (int32 i=0; i<m_aStrFileTitle.GetSize(); i++)
	{
		m_ListShow.InsertString(-1, m_aStrFileTitle[i]);
		m_ListShow.SetItemColor(i, 0xff0000, RGB(200,200,200), iFileType);
	}	


	if (0 == m_ListShow.GetCount())
	{
		// ?
		m_ListShow.PostMessage(WM_LBUTTONDOWN,0,0);
	}

	// ����Ĭ��ѡ��Ϊ��ǰ�Ĵ򿪵Ĺ������ļ� xl 0531
	if ( CDlgWorkSpaceSet::EFTWorkSpace == m_eFileType )
	{
		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
		ASSERT( NULL != pApp );
		if ( NULL != pApp )
		{
			CString StrCurWS = pApp->m_StrCurWsp;
			for (int32 i=0; i<m_aStrFileTitle.GetSize(); i++)
			{
				if ( StrCurWS == m_aStrFileInfo[i] )
				{
					m_ListShow.SetCurSel(i);
					break;
				}
			}
		}
	}

	return TRUE;
}

void CDlgWorkSpaceSet::OnOK() 
{		
	int32 iCur = m_ListShow.GetCurSel();

	if (iCur>=0&& iCur<m_aStrFileInfo.GetSize())
	{
		OnDblclkListShow();
	}	

	CDialog::OnOK();	
}

void CDlgWorkSpaceSet::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rect;
	m_ListShow.GetClientRect(&rect);		
	//dc.FillSolidRect(rect,RGB(255,0,0));

	// Do not call CDialog::OnPaint() for painting messages
}

void CDlgWorkSpaceSet::OnButtonDefault() 
{
	// �ָ�Ĭ��,������ workspace�ļ����е��ļ�������˽��workspace ��,�������������
	if ( m_eFileType != EFTWorkSpace)
		return;

	int32 iCurSel = m_ListShow.GetCurSel();
	if (iCurSel<0)
		return;

	CString StrFileInfo = m_aStrFileInfo[iCurSel];

	CString StrFileName;

	int32 iPos  = StrFileInfo.ReverseFind('\\');
	StrFileName = StrFileInfo.Right(StrFileInfo.GetLength()-iPos-1);

	CString StrFileToCopy = CPathFactory::GetPublicWorkspacePath();
	StrFileToCopy += StrFileName;

	CFile::Remove(StrFileInfo);
	::CopyFile(StrFileToCopy,StrFileInfo,false);

	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	pMainFrame->ClearCurrentWorkSpace();
	// xl ���ضԻ�����
	ShowWindow(SW_HIDE);
	OpenFile(StrFileInfo);
	OnCancel();

}
void CDlgWorkSpaceSet::OnSelchangeListShow() 
{
	// �жϹ���workspace ���Ƿ��������ļ�,���ð�ť�Ƿ����
	if ( m_eFileType != EFTWorkSpace)
		return;		

	int32 iCurSel = m_ListShow.GetCurSel();

	if (iCurSel<0)
		return;

	CString StrFileInfo = m_aStrFileInfo[iCurSel];	
	CString StrFileName;

	int32 iPos  = StrFileInfo.ReverseFind('\\');
	StrFileName = StrFileInfo.Right(StrFileInfo.GetLength()-iPos-1);

	CString StrFileToTest = CPathFactory::GetPublicWorkspacePath();
	StrFileToTest += StrFileName;

	CWnd * pWnd = GetDlgItem(IDC_BUTTON_DEFAULT);

	CFileFind filefind;

	bool32 bfind = filefind.FindFile(StrFileToTest);

	if (bfind)
	{
		pWnd->EnableWindow(true);
	}
	else
	{
		pWnd->EnableWindow(false);
	}	
}
