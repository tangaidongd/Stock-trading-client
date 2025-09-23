#include "StdAfx.h"

#include "SwitchEx.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const TCHAR KStrSwitchExClassName[] = _T("SwitchEx");

IMPLEMENT_DYNCREATE(CSwitchEx, CWnd)

BEGIN_MESSAGE_MAP(CSwitchEx, CWnd)
	//{{AFX_MSG_MAP(CSwitchEx)
	
	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
	
CSwitchEx::CSwitchEx()
{
	m_iMsgBaseId = DefaultMsgBaseId;
	
	if ( !RegisterMyClass() )
	{
		ASSERT( 0 );
	}
}

CSwitchEx::~CSwitchEx()
{
	
}

BOOL CSwitchEx::RegisterMyClass()
{
	WNDCLASS cls;
	memset(&cls, 0, sizeof(cls));
	HINSTANCE hInst = AfxGetInstanceHandle();
	if ( ::GetClassInfo(hInst, KStrSwitchExClassName, &cls) )
	{
		return TRUE;
	}
	cls.cbClsExtra = cls.cbWndExtra = 0;
	cls.hInstance = hInst;
	cls.hIcon = NULL;
	cls.hCursor = NULL;
	cls.hbrBackground = (HBRUSH)COLOR_WINDOW;
	cls.style = CS_HREDRAW |CS_VREDRAW;
	cls.lpszMenuName = NULL;
	cls.lpszClassName = KStrSwitchExClassName;
	cls.lpfnWndProc = ::DefWindowProc;
	return AfxRegisterClass(&cls);
}

BOOL CSwitchEx::Create( LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, int iMsgBaseId/* = DefaultMsgBaseId*/,  CCreateContext* pContext /*= NULL*/ )
{
	m_iMsgBaseId = iMsgBaseId;
	return CWnd::Create(KStrSwitchExClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

int CSwitchEx::OnToolHitTest( CPoint point, TOOLINFO* pTI ) const
{
	return CWnd::OnToolHitTest(point, pTI);
}


//////////////////////////////////////////////////////////////////////////
// T_Btn
CSwitchEx::T_Btn::T_Btn()
{
	m_Rect.SetRect(0, 0, 0, 0);
	m_nWeight = 1;
	m_bFixWidth = true;
	m_nId = 0;
	m_iWidth = 0;
	m_eTipMode = TipNone;
}
