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
		// 全部拷贝
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
			// 状态栏发送过来的回车，才进行处理。 避免重复调用SendInfomation
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
	
	//关闭之前，清空statusbar的edit
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
	// 设置窗体位置;
	
	CRect rectIn,rectOut,rectMain,rectStatus ,rectKeyBoardEdit;
	
	GetClientRect(rectIn);
	AfxGetMainWnd()->GetClientRect(rectMain); 
	AfxGetMainWnd()->ClientToScreen(rectMain);
	((CMainFrame *)AfxGetMainWnd())->m_wndStatusBar.GetWindowRect(rectStatus);
	
	//获取任务栏的搜索按钮坐标
	((CMainFrame *)AfxGetMainWnd())->m_wndStatusBar.GetKeyBoarEditRect(rectKeyBoardEdit);
	AfxGetMainWnd()->ClientToScreen(rectKeyBoardEdit);
	
	rectOut.left = rectKeyBoardEdit.left;
	rectOut.top  = rectMain.bottom - rectIn.Height() - rectStatus.Height() - 18  ;
	
	rectOut.right = rectOut.left + rectIn.Width();
	rectOut.bottom = rectOut.top + rectIn.Height() +15;
	MoveWindow(rectOut);
	
	// ... 超出正常尺寸范围的处理. 得到屏幕尺寸的函数?
	// GetSystemMetrics(),SM_CXFULLSCREEN,SM_CYFULLSCREEN 全屏幕窗口的窗口区域的宽度和高度
	
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
	
	// 将消息结构中保存的内容发送到EDIT 控件.
	if ( m_Msg.hwnd != NULL )	// 避免发送错误的msg 0707 xl
	{
		m_edit.PostMessage(m_Msg.message,m_Msg.wParam,m_Msg.lParam);
	}
	
	//#BUG_MARK_BEGIN [NO=XL0003 AUTHOR=xiali DATE=2010/04/10]
	//DESCRIPTION:	应该保证Edit空间获取focus，因为输入法信息需要它来接收
	//#BUG_MARK_END [NO=XL0003]
	
	return TRUE;  
}


void CKeyBoardDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	// 失去焦点，关闭对话框
	CDialog::OnActivate(nState, pWndOther, bMinimized);	
	if ( nState == WA_INACTIVE )
	{
		//通过statusbar调用，不适用失去焦点关闭
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

	//新增功能，讲文本与statusbar的keyboardEdit信息同步。细节，键盘精灵是statusbar调用出来的，就不用做同步
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
		
		// 查找所有匹配的元素		
		CArray<CHotKey, CHotKey> aHotKeyIdentical;

		CHotKey* pHotKey = (CHotKey*)m_HotKeyListFromType.GetData();
		int32 iHotKeyNum = m_HotKeyListFromType.GetSize();

		if( NULL == pHotKey || iHotKeyNum <= 0 )
		{
			//ASSERT(0);
			return;
		}

		// 通过拼音排序的商品集合
		set<T_MerchSortByPY> aMerchSortByPY;

		// 是用拼音做 key 还是用代码做 key			
		bool32 bUseCodeKey = true;

		// fangz0216 外汇,期货等不是数字代码的商品会有问题.
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
					// 商品的, 放到集合里
					if ( !bidentical )
					{
						T_MerchSortByPY stMerchSortByPY;
						stMerchSortByPY.m_bUserCode = bUseCodeKey;

						if ( !bUseCodeKey )
						{
							// 用拼音匹配
							stMerchSortByPY.m_iPos = pHotKey[i].m_StrParam2.Find(m_StrText);
						}
						else
						{
							// 用代码匹配
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
	
		// 商品不要按照 m_HotKeyListSave 的默认顺序来排列, 按照拼音匹配的顺序
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

		//	暂时在添加到视图的时候优化，后期将搜索数据也优化一下
		m_CtrlList.SetRedraw(false);
		for ( i=0; i < m_HotKeyListSave.GetSize() && i < DIS_KEYBOARD_COUNT;i++)
		{
			CString StrKey = m_HotKeyListSave[i].m_StrKey;
			CString StrSpace = L" ";
			int32   iLenKey = StrKey.GetLength();
			int32   iLenFix = 6;
			
			if (iLenKey < iLenFix) // 61 等快捷键的中文名太长,不要对齐
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
				StrToAdd = m_HotKeyListSave[i].m_StrSummary;	// 仅显示板块名称，不显示快捷键
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
		//--- wangyongxue 暂时是EHKTMerch的时候才向LeftBar传市场 2016/09/13
		if (EHKTMerch == HotKey.m_eHotKeyType /*|| EHKTBlock == HotKey.m_eHotKeyType
			EHKTLogicBlock == HotKey.m_eHotKeyType*/)
		{
			//pFrame->m_leftToolBar.m_iMarketId = HotKey.m_iParam1;
		}	
		pParent->PostMessage(UM_HOTKEY, (WPARAM)pNewHotKey, 0);
	}
	return true;
}


/* MainFrame 中响应按键，如果对话框不存在就新建对话框.此时记录下按键消息。在初始化对话框时设置其位置，同时将保存好的消息发送到
EDIT 中。失去焦点后，自动销毁对话框 */

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
		// 			// 使列表框的背景色重画
		// 			m_CtrlList.PostMessage(WM_LBUTTONDOWN,0,0);	
		// 			m_CtrlList.PostMessage(WM_LBUTTONUP,0,0);	
		// 			// 重新激活编辑框,将光标设置到最后
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
	// 判断是否为点击
	CRect rcClient;
	GetWindowRect(&rcClient);
	DWORD dwPos1 = GetMessagePos(); 
	CPoint point( LOWORD(dwPos1), HIWORD(dwPos1) ); 
	BOOL bIsSide = rcClient.PtInRect(point);
	if (bIsSide)
	{
		m_edit.SetWindowText(strData);
		m_edit.SetSel(m_edit.GetWindowTextLength(), m_edit.GetWindowTextLength());

		// 获取坐标
		DWORD dwPos = GetMessagePos(); 
		CPoint pt( LOWORD(dwPos), HIWORD(dwPos) ); 
		m_CtrlList.ScreenToClient(&pt); 

		// 判断是否为Clistbox，并设置选中
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

		// 判断是否为edit
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

