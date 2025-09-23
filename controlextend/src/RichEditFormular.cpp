// RichEditFormular.cpp : implementation file
//

#include "stdafx.h"
#include "RichEditFormular.h"
//#include "DIAEDITZBGSHS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct dataFunc
{
	TCHAR s[512];
	int nFlag;
	TCHAR s2[512];
};

bool IsEn ( TCHAR tc )
{
	if ( tc & 0x80 )
	{ 
		return false;
	}
	return true;
}

// extern dataFunc stritemsArr[];
// extern int nItemCountText;
/////////////////////////////////////////////////////////////////////////////
// CRichEditFormular

const dataFunc * CRichEditFormular::m_pstritemsArr = NULL;
int CRichEditFormular::m_nItemCountText = 0;

CRichEditFormular::CRichEditFormular()
{
}

CRichEditFormular::~CRichEditFormular()
{

}

BEGIN_MESSAGE_MAP(CRichEditFormular, CAutoRichEditCtrl)
	//{{AFX_MSG_MAP(CRichEditFormular)
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRichEditFormular message handlers

void CRichEditFormular::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CAutoRichEditCtrl::OnMouseMove(nFlags, point);

	if(m_TitleTip.m_hWnd == NULL )
	{
		CRect rect(0,0,1,1);
		m_TitleTip.Create (this);
		return;
	}

	CString Text;
	GetWindowText( Text );


	POINTL tmpPoint;
	tmpPoint.x = point.x;
	tmpPoint.y = point.y;

	int nCharOffset = GetCharFormPos( point );
		
	if( nCharOffset < 0 )
	{
		m_TitleTip.Hide();
		return;
	}

	CString strText;
	GetWindowText( strText );
	//strText.Remove('\n');
	//strText.Remove('\r');
	if( nCharOffset >= strText.GetLength() )
	{
		m_TitleTip.Hide ();
		return;
	}
	CString strTmp = GetWordByOffset(nCharOffset);

	strTmp.TrimRight();
	if( strTmp.IsEmpty() )
	{
		m_TitleTip.Hide ();
		return;
	}

	strTmp.MakeUpper ();
	int nLen = strTmp.GetLength ();

	int i = 0;
	for(i = 0;i<m_nItemCountText;i++)
	{
		CString s = m_pstritemsArr[i].s ;
		s = s.Left (nLen);
		if(s == strTmp)
		{
			if(m_pstritemsArr[i].s[nLen] == '-' ||m_pstritemsArr[i].s[nLen] == '(')
				break;
		}
	}
	if(i>=m_nItemCountText)
	{
		m_TitleTip.Hide ();
		return;
	}
	CString s2 = m_pstritemsArr[i].s;
	int nFind = s2.Find (_T("---"),0);
	if(nFind>=0)
	{
		strTmp = s2.Left(nFind);
		s2.Delete (0,nFind+3);
	}
	strTmp = strTmp+_T("\n")+s2+_T("\n");

	m_TitleTip.Show(point,strTmp);

}

void CRichEditFormular::SetDataFunc(const dataFunc *stritemsArr,int nItemCountText)
{
	m_pstritemsArr = stritemsArr;
	m_nItemCountText = nItemCountText;
}
///-----------------------------------------------
/// 2002-11-8
///	功能:
///		根据偏移获取单词
///	入口参数:
///		nOffset			偏移
///	返回参数:
///		空字符串，失败
CString CRichEditFormular::GetWordByOffset(int nOffset)
{
	CString strText;
	GetWindowText( strText );
	//strText.Remove('\n');
	//strText.Remove('\r');
	if( nOffset >= strText.GetLength() )
	{
		strText.Empty();
		return strText;
	}
	int nLeftPos = nOffset;
	while( nLeftPos>=0 )
	{
		TCHAR cLeft = strText[ nLeftPos ];
		if (cLeft< 127)
		{			
			if( IsEn ( cLeft ) && 0 == ::isalnum(cLeft) )             // 不是 0x80 而且是字母数字, ::isalnum 不支持中文
			{
				nLeftPos ++;
				break;				//	找到
			}
		}
		nLeftPos --;
	}
	
	if( nLeftPos < 0 )
		nLeftPos = 0;

	int nMaxLen = strText.GetLength();
	int nRightPos = nOffset;
	while( nRightPos < nMaxLen )
	{
		TCHAR cRight = strText[ nRightPos ];
		if (cRight < 127 )
		{			
			if( IsEn ( cRight ) && 0 == ::isalnum(cRight) )
			{
				nRightPos --;
				CString ss = strText.Mid( nLeftPos, nRightPos-nLeftPos+1 );
				ss.MakeLower ();
				if(ss == _T("finance") || ss == _T("dynainfo")
					|| ss == _T("askvol")
					|| ss == _T("askprice")
					|| ss == _T("bidvol")
					|| ss == _T("bidorice")
					)
				{
					bool bFind = false;
					while( nRightPos < nMaxLen )
					{
						cRight = strText[ nRightPos ];
						if(cRight == ')')
						{
							bFind = true;
							//						nRightPos --;
							break;
						}
						nRightPos ++;
					}
				}
				break;
			}
		}
		nRightPos ++;
	}
	if( nRightPos >= nMaxLen )
		nRightPos = nMaxLen-1;

	CString ss2 = strText.Mid( nLeftPos, nRightPos-nLeftPos+1 );

	ss2.Replace (_T(" "),_T(""));
	
	return ss2;
}

int CRichEditFormular::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CAutoRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rect(0,0,1,1);
	m_TitleTip.Create (this);
	return 0;
}