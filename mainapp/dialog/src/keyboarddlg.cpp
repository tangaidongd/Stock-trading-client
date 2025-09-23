// keyboarddlg.cpp : implementation file
//

#include "stdafx.h"
#include "keyboarddlg.h"
#include "StatusBarEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define DIS_KEYBOARD_COUNT		60

/////////////////////////////////////////////////////////////////////////////
// CKeyBoardDlg dialog
CKeyBoardDlg::CKeyBoardDlg(CArray<CHotKey, CHotKey&>& arrHotKeys, CWnd* pParent /*= NULL*/,E_HotKeyType eHotKeyType /*= EHKTCount*/ )
: CDialogEx(CKeyBoardDlg::IDD, pParent),m_bAutoClose(true)
{
	if ( eHotKeyType < EHKTCount )
	{
		m_HotKeyListFromType.SetSize(0, 300);
		for ( int i=0; i < arrHotKeys.GetSize() ; i++ )
		{
			if ( eHotKeyType == arrHotKeys[i].m_eHotKeyType )
			{
				m_HotKeyListFromType.Add(arrHotKeys[i]);
			}
		}
	}
	else
	{
		// ȫ������
		m_HotKeyListFromType.Copy(arrHotKeys);
	}

	memset(&m_Msg, 0, sizeof(m_Msg));
	m_StrText = _T("");

	m_bStatusBarSign = false;
}


void CKeyBoardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyBoardDlg)
	DDX_Control(pDX, IDC_EDIT_INPUTINFO, m_edit);
	DDX_Control(pDX, IDC_LIST, m_CtrlList);
	DDX_Text(pDX, IDC_EDIT_INPUTINFO, m_StrText);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CKeyBoardDlg, CDialogEx)
	//{{AFX_MSG_MAP(CKeyBoardDlg)
	ON_WM_CLOSE()
	ON_WM_ACTIVATE()
	ON_EN_CHANGE(IDC_EDIT_INPUTINFO, OnChangeText)
	ON_WM_CTLCOLOR()
	ON_MESSAGE_VOID(UM_UserSelectListItem, OnUserSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyBoardDlg message handlers

BOOL CKeyBoardDlg::PreTranslateMessage(MSG* pMsg) 
{
	if ( WM_KEYDOWN == pMsg->message)
	{
		if ( pMsg->wParam == VK_UP)
		{
			if (0 != m_CtrlList.GetCurSel())
			{			
				m_CtrlList.SetCurSel(m_CtrlList.GetCurSel()-1);								
			}
			m_edit.PostMessage(WM_KEYDOWN,VK_RIGHT,0);
		}
		else if (pMsg->wParam ==VK_DOWN )
		{
			m_CtrlList.SetCurSel(m_CtrlList.GetCurSel()+1);
		}
		else if (pMsg->wParam == VK_RETURN )
		{
			// ״̬�����͹����Ļس����Ž��д��� �����ظ�����SendInfomation
			if (m_bStatusBarSign)
			{
				int32 iCur= m_CtrlList.GetCurSel();
				if (iCur >= 0 && iCur < m_HotKeyListSave.GetSize())
				{
					SendInfomation(m_HotKeyListSave[iCur]);
				}

				m_StrText = L"";
				UpdateData(false);
			}
		}
	}
	else if ( WM_LBUTTONDBLCLK == pMsg->message && pMsg->hwnd == m_CtrlList.GetSafeHwnd())
	{
		int32 iCur= m_CtrlList.GetCurSel();
		if (iCur >= 0 && iCur < m_HotKeyListSave.GetSize())
		{
			//SendInfomation(m_HotKeyListSave[iCur]);
			PostMessage(UM_UserSelectListItem);
		}
		else
		{
			PostMessage(WM_CLOSE,0,0);
		}
	}		
	return CDialog::PreTranslateMessage(pMsg);
}

void CKeyBoardDlg::OnClose() 
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	pWnd->m_pDlgKeyBoard = NULL;
	
	//�ر�֮ǰ�����statusbar��edit
	pWnd->m_wndStatusBar.SetKeyBoarEditText("");

	m_pParentWnd->PostMessage(UM_Dlghotkey_Close,0,0);

	DestroyWindow();	
}

void CKeyBoardDlg::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	delete this;
}

void CKeyBoardDlg::SetStatusBarSign(bool bSign)
{
	m_bStatusBarSign = bSign;
}


void CKeyBoardDlg::SetEditText(CString szDipData)
{
	m_edit.SetWindowText(szDipData);
}


BOOL CKeyBoardDlg::OnInitDialog() 
{	
	// ���ô���λ��;
	
	CRect rectIn,rectOut,rectMain,rectStatus ,rectKeyBoardEdit;
	
	GetClientRect(rectIn);
	AfxGetMainWnd()->GetClientRect(rectMain); 
	AfxGetMainWnd()->ClientToScreen(rectMain);
	((CMainFrame *)AfxGetMainWnd())->m_wndStatusBar.GetWindowRect(rectStatus);
	
	//��ȡ��������������ť����
	((CMainFrame *)AfxGetMainWnd())->m_wndStatusBar.GetKeyBoarEditRect(rectKeyBoardEdit);
	AfxGetMainWnd()->ClientToScreen(rectKeyBoardEdit);
	
	rectOut.left = rectKeyBoardEdit.left;
	rectOut.top  = rectMain.bottom - rectIn.Height() - rectStatus.Height() - 18  ;
	
	rectOut.right = rectOut.left + rectIn.Width();
	rectOut.bottom = rectOut.top + rectIn.Height() +15;
	MoveWindow(rectOut);
	
	// ... ���������ߴ緶Χ�Ĵ���. �õ���Ļ�ߴ�ĺ���?
	// GetSystemMetrics(),SM_CXFULLSCREEN,SM_CYFULLSCREEN ȫ��Ļ���ڵĴ�������Ŀ�Ⱥ͸߶�
	
	int32 iCx = GetSystemMetrics(SM_CXFULLSCREEN);
	int32 iCy = GetSystemMetrics(SM_CYFULLSCREEN);
	
	if( rectOut.top < 0 )
	{
		rectOut.top=0;
	}
	if( rectOut.left < 0 )
	{
		rectOut.left=0;
		rectOut.right = rectOut.left + rectIn.Width();
	}
	if( rectOut.right > iCx ) 
	{
		rectOut.left  = iCx - rectIn.Width();
		rectOut.right = iCx-2;
	}
	if( rectOut.bottom > iCy ) 
	{	
		rectOut.top = iCy - rectIn.Height(); 
		rectOut.bottom = iCy + 20;
	}
	
	MoveWindow(rectOut);
	CDialog::OnInitDialog();
	
	// ����Ϣ�ṹ�б�������ݷ��͵�EDIT �ؼ�.
	if ( m_Msg.hwnd != NULL )	// ���ⷢ�ʹ����msg 0707 xl
	{
		m_edit.PostMessage(m_Msg.message,m_Msg.wParam,m_Msg.lParam);
	}
	
	//#BUG_MARK_BEGIN [NO=XL0003 AUTHOR=xiali DATE=2010/04/10]
	//DESCRIPTION:	Ӧ�ñ�֤Edit�ռ��ȡfocus����Ϊ���뷨��Ϣ��Ҫ��������
	//#BUG_MARK_END [NO=XL0003]
	
	return TRUE;  
}


void CKeyBoardDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	// ʧȥ���㣬�رնԻ���
	CDialog::OnActivate(nState, pWndOther, bMinimized);	
	if ( nState == WA_INACTIVE )
	{
		//ͨ��statusbar���ã�������ʧȥ����ر�
		if (m_bStatusBarSign || m_bIsActiveByClickSign)
		{
			return;
		}

		PostMessage(WM_CLOSE,0,0);
		return;
	}
	else if (WA_ACTIVE == nState)
	{
		m_bIsActiveByClickSign = false;
		// WA_ACTIVE
	}
	else if (WA_CLICKACTIVE == nState)
	{
		m_bIsActiveByClickSign = false;
		// WA_CLICKACTIVE
	}
}

void CKeyBoardDlg::OnCancel() 
{
	PostMessage(WM_CLOSE,0,0);
}

void CKeyBoardDlg::OnOK() 
{
	int32 iCur= m_CtrlList.GetCurSel();
	if (iCur >= 0 && iCur < m_HotKeyListSave.GetSize())
	{
		SendInfomation(m_HotKeyListSave[iCur]);
	}
	
	m_StrText = L"";
	UpdateData(false);

	if ( m_bAutoClose )
	{
		OnClose();
	}		
}

void CKeyBoardDlg::OnChangeText() 
{
	UpdateData(TRUE);
	m_HotKeyListSave.RemoveAll();

	//
	m_StrText.TrimLeft();
	m_StrText.TrimRight();
	int32 iStrLen = m_StrText.GetLength();

	//�������ܣ����ı���statusbar��keyboardEdit��Ϣͬ����ϸ�ڣ����̾�����statusbar���ó����ģ��Ͳ�����ͬ��
	if (!m_bStatusBarSign || m_bIsActiveByClickSign)
	{
		if (NULL != ((CMainFrame *)AfxGetMainWnd()))
		{
			((CMainFrame *)AfxGetMainWnd())->m_wndStatusBar.SetKeyBoarEditText(m_StrText);
		}		
	}
	
	if ( 0 == m_StrText.GetLength() )
	{
		if (m_bStatusBarSign || m_bIsActiveByClickSign)
		{
			return;
		}

		PostMessage(WM_CLOSE, 0, 0);
	}

	if ( 0 == iStrLen )
	{
		for (int32 i = m_CtrlList.GetCount(); i >= 0; i--)
		{
			m_CtrlList.DeleteString(i);
		}
	}
	else
	{
		int32 i = 0;
		for (i = m_CtrlList.GetCount();i>=0;i--)
		{
			m_CtrlList.DeleteString(i);
		}
		
		// ��������ƥ���Ԫ��		
		CArray<CHotKey, CHotKey> aHotKeyIdentical;

		CHotKey* pHotKey = (CHotKey*)m_HotKeyListFromType.GetData();
		int32 iHotKeyNum = m_HotKeyListFromType.GetSize();

		if( NULL == pHotKey || iHotKeyNum <= 0 )
		{
			//ASSERT(0);
			return;
		}

		// ͨ��ƴ���������Ʒ����
		set<T_MerchSortByPY> aMerchSortByPY;

		// ����ƴ���� key �����ô����� key			
		bool32 bUseCodeKey = true;

		// fangz0216 ���,�ڻ��Ȳ������ִ������Ʒ��������.
		for ( i = 0; i < iStrLen; i++ )
		{
			TCHAR chNow = m_StrText.GetAt(i);
			if ( chNow < '0' || chNow > '9' )
			{
				bUseCodeKey = false;
				break;
			}
		}

		//
		int32 iUserData = 1;

		//
		for ( i = 0; i < iHotKeyNum; i++)
		{
			if ( i == 4877 )
			{
				i = i;
			}

			bool32 bidentical = false;
			 
			if ( pHotKey[i].IsMatch(m_StrText, bidentical) )
			{
				if ( EHKTMerch == pHotKey[i].m_eHotKeyType )
				{
					// ��Ʒ��, �ŵ�������
					if ( !bidentical )
					{
						T_MerchSortByPY stMerchSortByPY;
						stMerchSortByPY.m_bUserCode = bUseCodeKey;

						if ( !bUseCodeKey )
						{
							// ��ƴ��ƥ��
							stMerchSortByPY.m_iPos = pHotKey[i].m_StrParam2.Find(m_StrText);
						}
						else
						{
							// �ô���ƥ��
							stMerchSortByPY.m_iPos = pHotKey[i].m_StrKey.Find(m_StrText);
						}
						
						if ( stMerchSortByPY.m_iPos < 0 )
						{							
							stMerchSortByPY.m_iPos = 100000;
						}

						stMerchSortByPY.m_HotKey = pHotKey[i];

						//
						int32 iCount = aMerchSortByPY.count(stMerchSortByPY);
						
						if ( iCount > 0 )
						{
							stMerchSortByPY.m_iUserData = iUserData;			
							iUserData += 1;
						}

						aMerchSortByPY.insert(stMerchSortByPY);
					}
					else
					{												
						aHotKeyIdentical.Add(pHotKey[i]);
					}					
				}
				else 
				{
					//
					if (!bidentical)
					{
						m_HotKeyListSave.Add(pHotKey[i]);
					} 
					else
					{
						m_HotKeyListSave.InsertAt(0, pHotKey[i]);
					}	
				}							
			}
		} 
	
		// ��Ʒ��Ҫ���� m_HotKeyListSave ��Ĭ��˳��������, ����ƴ��ƥ���˳��
		{
			for ( int32 indexHotKey=0; indexHotKey<aHotKeyIdentical.GetSize(); indexHotKey++ )
			{
				m_HotKeyListSave.InsertAt(0, aHotKeyIdentical.GetAt(indexHotKey));
			}

			for ( set<T_MerchSortByPY>::iterator it = aMerchSortByPY.begin(); it != aMerchSortByPY.end(); ++it )
			{
				T_MerchSortByPY stMerchSortByPY = *it;	
				m_HotKeyListSave.Add(stMerchSortByPY.m_HotKey);
			}
		}

		//	��ʱ����ӵ���ͼ��ʱ���Ż������ڽ���������Ҳ�Ż�һ��
		m_CtrlList.SetRedraw(false);
		for ( i=0; i < m_HotKeyListSave.GetSize() && i < DIS_KEYBOARD_COUNT;i++)
		{
			CString StrKey = m_HotKeyListSave[i].m_StrKey;
			CString StrSpace = L" ";
			int32   iLenKey = StrKey.GetLength();
			int32   iLenFix = 6;
			
			if (iLenKey < iLenFix) // 61 �ȿ�ݼ���������̫��,��Ҫ����
			{
				int32 iSpaceCount = iLenFix - iLenKey;
				for (int32 indexSpaceCnt=0; indexSpaceCnt<iSpaceCount; indexSpaceCnt++)
				{
					StrKey += StrSpace;
				}					
			}
			CString StrToAdd =StrKey + L"  " +m_HotKeyListSave[i].m_StrSummary;			
			if ( m_HotKeyListSave[i].m_eHotKeyType == EHKTLogicBlock )
			{
				StrToAdd = m_HotKeyListSave[i].m_StrSummary;	// ����ʾ������ƣ�����ʾ��ݼ�
			}
			m_CtrlList.InsertString(i,StrToAdd);
			
			switch(m_HotKeyListSave[i].m_eHotKeyType )
			{
			case EHKTMerch:
				m_CtrlList.SetItemColor(i,0xff0000,RGB(200,200,200),m_HotKeyListSave[i].m_eHotKeyType);
				break;
			case EHKTShortCut:
				m_CtrlList.SetItemColor(i,0x400080,RGB(200,200,200),m_HotKeyListSave[i].m_eHotKeyType);
				break;
			case EHKTBlock:
				m_CtrlList.SetItemColor(i,0xff,RGB(200,200,200),m_HotKeyListSave[i].m_eHotKeyType);
				break;
			case EHKTIndex:
				m_CtrlList.SetItemColor(i, RGB(222,75,12) /*0x80ff*/,RGB(200,200,200),m_HotKeyListSave[i].m_eHotKeyType);
				break;
			case EHKTLogicBlock:
				m_CtrlList.SetItemColor(i, RGB(140,23,210), RGB(200,200,200), m_HotKeyListSave[i].m_eHotKeyType);
				break;
			default:
				break;
			}
		}
		m_CtrlList.SetRedraw(true);


		//////////////////////////////////////////////////////////////////////////		
		m_CtrlList.SetCurSel(0);
	}	
}
#ifdef _DEBUG
void CKeyBoardDlg::AssertValid() const
{
    // call inherited AssertValid first
    CObject::AssertValid();
} 
#endif


bool32 CKeyBoardDlg::SendInfomation(CHotKey &HotKey)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CWnd *pParent = pFrame->m_pKBParent;
	if (NULL != pParent)
	{
		CHotKey *pNewHotKey = new CHotKey;

		*pNewHotKey = HotKey;
		//--- wangyongxue ��ʱ��EHKTMerch��ʱ�����LeftBar���г� 2016/09/13
		if (EHKTMerch == HotKey.m_eHotKeyType /*|| EHKTBlock == HotKey.m_eHotKeyType
			EHKTLogicBlock == HotKey.m_eHotKeyType*/)
		{
			//pFrame->m_leftToolBar.m_iMarketId = HotKey.m_iParam1;
		}	
		pParent->PostMessage(UM_HOTKEY, (WPARAM)pNewHotKey, 0);
	}
	return true;
}


/* MainFrame ����Ӧ����������Ի��򲻴��ھ��½��Ի���.��ʱ��¼�°�����Ϣ���ڳ�ʼ���Ի���ʱ������λ�ã�ͬʱ������õ���Ϣ���͵�
EDIT �С�ʧȥ������Զ����ٶԻ��� */

HBRUSH CKeyBoardDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	static	CBrush	sBrush;

	if (NULL == sBrush.m_hObject)
	{
		sBrush.CreateSolidBrush(RGB(200,200,200));
	}
	if (nCtlColor == CTLCOLOR_EDIT)
	{		
		//  		CRect rectEdit;
		//  		m_edit.GetClientRect(&rectEdit);
		//  		pDC->FillSolidRect(&rectEdit,RGB(200,200,200));
		
		pDC->SetTextColor(RGB(255,0,0));
		pDC->SetBkColor(RGB(200,200,200));
		
	}
	else if (nCtlColor == CTLCOLOR_LISTBOX)
	{		
		// 		CRect rect,rectNeedDraw;
		// 
		// 		m_CtrlList.GetClientRect(&rect);
		// 		int  iHeight = m_CtrlList.GetItemHeight(0);
		// 		int  iCount = m_CtrlList.GetCount();
		// 	    int  iCountMax = rect.Height()/iHeight;
		// 		if ( iCount < iCountMax )
		// 		{
		// 			rectNeedDraw = rect;
		// 			rectNeedDraw.top = rect.top + iCount*iHeight;
		// 		}	
		// 		if ( iCount == 0)
		// 		{	
		// 			// ʹ�б��ı���ɫ�ػ�
		// 			m_CtrlList.PostMessage(WM_LBUTTONDOWN,0,0);	
		// 			m_CtrlList.PostMessage(WM_LBUTTONUP,0,0);	
		// 			// ���¼���༭��,��������õ����
		// 			m_edit.PostMessage(WM_LBUTTONDOWN,0,0);			
		// 			m_edit.PostMessage(WM_LBUTTONUP,0,0);	
		// 			m_edit.PostMessage(WM_KEYDOWN,VK_END,0);
		// 		}
		//pDC->FillSolidRect(&rectNeedDraw,RGB(200,200,200));		
	}	

	return sBrush;
}

void CKeyBoardDlg::OnUserSelect()
{
	int32 iCur= m_CtrlList.GetCurSel();
	if (iCur >= 0 && iCur < m_HotKeyListSave.GetSize())
	{
		SendInfomation(m_HotKeyListSave[iCur]);
		m_StrText = L"";
		UpdateData(false);
		
		if ( m_bAutoClose )
		{
			OnClose();
		}
	}
}


bool CKeyBoardDlg::CustomClose(CString strEditData)
{
	CString strData = strEditData;
	// �ж��Ƿ�Ϊ���
	CRect rcClient;
	GetWindowRect(&rcClient);
	DWORD dwPos1 = GetMessagePos(); 
	CPoint point( LOWORD(dwPos1), HIWORD(dwPos1) ); 
	BOOL bIsSide = rcClient.PtInRect(point);
	if (bIsSide)
	{
		m_edit.SetWindowText(strData);
		m_edit.SetSel(m_edit.GetWindowTextLength(), m_edit.GetWindowTextLength());

		// ��ȡ����
		DWORD dwPos = GetMessagePos(); 
		CPoint pt( LOWORD(dwPos), HIWORD(dwPos) ); 
		m_CtrlList.ScreenToClient(&pt); 

		// �ж��Ƿ�ΪClistbox��������ѡ��
		BOOL bOutside = TRUE;
		UINT uItem = m_CtrlList.ItemFromPoint(pt, bOutside);
		if (!bOutside)
		{
			m_CtrlList.SetCurSel(uItem);
		}
		else
		{
			m_edit.SetFocus();
		}

		// �ж��Ƿ�Ϊedit
		DWORD dwPosEdit = GetMessagePos(); 
		CPoint pointEdit( LOWORD(dwPosEdit), HIWORD(dwPosEdit) );
		CRect cRectEdit;
		m_edit.GetClientRect(&cRectEdit);
		ClientToScreen(&cRectEdit);
		bOutside = cRectEdit.PtInRect(pointEdit);
		if (bOutside)
		{
			m_edit.SetFocus();
		}
	}
	else
	{
		OnClose();
	}
	return 0;
}

