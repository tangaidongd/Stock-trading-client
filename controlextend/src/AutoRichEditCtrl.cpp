// AutoRichEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ShareFun.h"
#include "AutoRichEditCtrl.h"
#include "typedef.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool IsWideChar( wchar_t tc )
{
	return tc>0x00ff;
}

/////////////////////////////////////////////////////////////////////////////
// CAutoRichEditCtrl

CAutoRichEditCtrl::CAutoRichEditCtrl()
{
}

CAutoRichEditCtrl::~CAutoRichEditCtrl()
{
}


BEGIN_MESSAGE_MAP(CAutoRichEditCtrl, CRichEditCtrl)
	//{{AFX_MSG_MAP(CAutoRichEditCtrl)
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoRichEditCtrl message handlers

CString CAutoRichEditCtrl::GetRTF()
{
	// Return the RTF string of the text in the control.
	
	// Stream out here.
	EDITSTREAM es;
	es.dwError = 0;
	es.pfnCallback = CBStreamOut;		// Set the callback

	CString sRTF = _T("");

	es.dwCookie = (DWORD) &sRTF;	// so sRTF receives the string
	
	StreamOut(SF_RTF, es);			// Call CRichEditCtrl::StreamOut to get the string.
	///

	return sRTF;

}

void CAutoRichEditCtrl::SetRTF(CString sRTF)
{
	// Put the RTF string sRTF into the rich edit control.

	// Read the text in
	EDITSTREAM es;
	es.dwError = 0;
	es.pfnCallback = CBStreamIn;
	es.dwCookie = (DWORD) &sRTF;
	StreamIn(SF_RTF, es);	// Do it.
	
}

/*
	Callback function to stream an RTF string into the rich edit control.
*/
DWORD CALLBACK CAutoRichEditCtrl::CBStreamIn(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	// We insert the rich text here.

/*	
	This function taken from CodeGuru.com
	http://www.codeguru.com/richedit/rtf_string_streamin.shtml
	Zafir Anjum
*/

	CString *pstr = (CString *) dwCookie;

	if (pstr->GetLength() < cb)
	{
		*pcb = pstr->GetLength();
		
		memcpyex(pbBuff, *pstr, *pcb);//cuipeng2
		pstr->Empty();
	}
	else
	{
		*pcb = cb;
		memcpyex(pbBuff, *pstr, *pcb);//cuipeng2
		*pstr = pstr->Right(pstr->GetLength() - cb);
	}
	///

	return 0;
}

/*
	Callback function to stream the RTF string out of the rich edit control.
*/
DWORD CALLBACK CAutoRichEditCtrl::CBStreamOut(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	// Address of our string var is in psEntry
	CString *psEntry = (CString*) dwCookie;
	

	CString tmpEntry = _T("");
	tmpEntry = (CString) pbBuff;

	// And write it!!!
	*psEntry += tmpEntry.Left(cb);

	return 0;
}

bool CAutoRichEditCtrl::SelectionIsBold()
{
	CHARFORMAT cf = GetCharFormat();	
	
	if (cf.dwEffects & CFM_BOLD)
		return true;
	else
		return false;
}

bool CAutoRichEditCtrl::SelectionIsItalic()
{
	CHARFORMAT cf = GetCharFormat();	
	
	if (cf.dwEffects & CFM_ITALIC)
		return true;
	else
		return false;
}

bool CAutoRichEditCtrl::SelectionIsUnderlined()
{
	CHARFORMAT cf = GetCharFormat();	
	
	if (cf.dwEffects & CFM_UNDERLINE)
		return true;
	else
		return false;
}

CHARFORMAT CAutoRichEditCtrl::GetCharFormat(DWORD dwMask)
{
	CHARFORMAT cf;
	cf.cbSize = sizeof(CHARFORMAT);

	cf.dwMask = dwMask;

	GetSelectionCharFormat(cf);

	return cf;
}

void CAutoRichEditCtrl::SetCharStyle(int MASK, int STYLE, int nStart, int nEnd)
{
	CHARFORMAT cf;
	cf.cbSize = sizeof(CHARFORMAT);
	//cf.dwMask = MASK;
	
	GetSelectionCharFormat(cf);
	
	if (cf.dwMask & MASK)	// selection is all the same
	{
		cf.dwEffects ^= STYLE; 
	}
	else
	{
		cf.dwEffects |= STYLE;
	}
	
	cf.dwMask = MASK;

	SetSelectionCharFormat(cf);

}

void CAutoRichEditCtrl::SetSelectionBold()
{
	long start=0, end=0;
	GetSel(start, end);		// Get the current selection

	SetCharStyle(CFM_BOLD, CFE_BOLD, start, end);	// Make it bold
}

void CAutoRichEditCtrl::SetSelectionItalic()
{
	long start=0, end=0;
	GetSel(start, end);

	SetCharStyle(CFM_ITALIC, CFE_ITALIC, start, end);
}

void CAutoRichEditCtrl::SetSelectionUnderlined()
{
	long start=0, end=0;
	GetSel(start, end);

	SetCharStyle(CFM_UNDERLINE, CFE_UNDERLINE, start, end);
}

void CAutoRichEditCtrl::SetParagraphCenter()
{
	PARAFORMAT paraFormat;    
	paraFormat.cbSize = sizeof(PARAFORMAT);
	paraFormat.dwMask = PFM_ALIGNMENT;    
	paraFormat.wAlignment = PFA_CENTER;
	
	SetParaFormat(paraFormat);	// Set the paragraph.
}

void CAutoRichEditCtrl::SetParagraphLeft()
{
	PARAFORMAT paraFormat;
	paraFormat.cbSize = sizeof(PARAFORMAT);
	paraFormat.dwMask = PFM_ALIGNMENT;    
	paraFormat.wAlignment = PFA_LEFT;
	
	SetParaFormat(paraFormat);
}

void CAutoRichEditCtrl::SetParagraphRight()
{
	PARAFORMAT paraFormat;
	paraFormat.cbSize = sizeof(PARAFORMAT);
	paraFormat.dwMask = PFM_ALIGNMENT;    
	paraFormat.wAlignment = PFA_RIGHT;
	
	SetParaFormat(paraFormat);
}

bool CAutoRichEditCtrl::ParagraphIsCentered()
{
	PARAFORMAT pf = GetParagraphFormat();

	if (pf.wAlignment == PFA_CENTER)
		return true;
	else
		return false;
}

bool CAutoRichEditCtrl::ParagraphIsLeft()
{
	PARAFORMAT pf = GetParagraphFormat();

	if (pf.wAlignment == PFA_LEFT)
		return true;
	else
		return false;
}

bool CAutoRichEditCtrl::ParagraphIsRight()
{
	PARAFORMAT pf = GetParagraphFormat();

	if (pf.wAlignment == PFA_RIGHT)
		return true;
	else
		return false;
}

PARAFORMAT CAutoRichEditCtrl::GetParagraphFormat()
{
	PARAFORMAT pf;
	pf.cbSize = sizeof(PARAFORMAT);

	pf.dwMask = PFM_ALIGNMENT | PFM_NUMBERING;    	

	GetParaFormat(pf);

	return pf;
}

void CAutoRichEditCtrl::SetParagraphBulleted()
{
	PARAFORMAT paraformat = GetParagraphFormat();

	if ( (paraformat.dwMask & PFM_NUMBERING) && (paraformat.wNumbering == PFN_BULLET) )
	{
		paraformat.wNumbering = 0;
		paraformat.dxOffset = 0;
		paraformat.dxStartIndent = 0;
		paraformat.dwMask = PFM_NUMBERING | PFM_STARTINDENT | PFM_OFFSET;
	}
	else
	{
		paraformat.wNumbering = PFN_BULLET;
		paraformat.dwMask = PFM_NUMBERING;
		if (paraformat.dxOffset == 0)
		{
			paraformat.dxOffset = 4;
			paraformat.dwMask = PFM_NUMBERING | PFM_STARTINDENT | PFM_OFFSET;
		}
	}
	
	SetParaFormat(paraformat);

}

bool CAutoRichEditCtrl::ParagraphIsBulleted()
{
	PARAFORMAT pf = GetParagraphFormat();

	if (pf.wNumbering == PFN_BULLET)
		return true;
	else
		return false;
}

void CAutoRichEditCtrl::SelectColor()
{
	CColorDialog dlg;

	CHARFORMAT cf = GetCharFormat();

	if (cf.dwEffects & CFE_AUTOCOLOR) cf.dwEffects -= CFE_AUTOCOLOR;

	// Get a color from the common color dialog.
	if( dlg.DoModal() == IDOK )
	{	
		cf.crTextColor = dlg.GetColor();
	}

	cf.dwMask = CFM_COLOR;

	SetSelectionCharFormat(cf);
}

void CAutoRichEditCtrl::SetDefaultTextColor(COLORREF clr)
{
	CHARFORMAT cf = GetCharFormat();
	
	if (cf.dwEffects & CFE_AUTOCOLOR) cf.dwEffects -= CFE_AUTOCOLOR;
	
	cf.crTextColor = clr;
	cf.dwMask = CFM_COLOR;
	
	SetDefaultCharFormat(cf);
}

void CAutoRichEditCtrl::SetSelectColor(COLORREF & clr)
{
	CColorDialog dlg;

	CHARFORMAT cf = GetCharFormat();

	if (cf.dwEffects & CFE_AUTOCOLOR) cf.dwEffects -= CFE_AUTOCOLOR;

	cf.crTextColor = clr;

	cf.dwMask = CFM_COLOR;

	SetSelectionCharFormat(cf);
}

void CAutoRichEditCtrl::SetFontName(CString sFontName)
{
	CHARFORMAT cf = GetCharFormat();

	// Set the font name.
	for (int i = 0; i <= sFontName.GetLength()-1; i++)
		cf.szFaceName[i] = sFontName[i];


	cf.dwMask = CFM_FACE;

	SetSelectionCharFormat(cf);
}

void CAutoRichEditCtrl::SetFontSize(int nPointSize)
{
	CHARFORMAT cf = GetCharFormat();

	nPointSize *= 20;	// convert from to twips
	cf.yHeight = nPointSize;
	
	cf.dwMask = CFM_SIZE;

	SetSelectionCharFormat(cf);
}

void CAutoRichEditCtrl::GetSystemFonts(CStringArray &saFontList)
{
	CDC *pDC = GetDC ();

	EnumFonts (pDC->GetSafeHdc(),NULL,(FONTENUMPROC) CBEnumFonts,(LPARAM)&saFontList);//Enumerate

	ReleaseDC(pDC);
}

BOOL CALLBACK CAutoRichEditCtrl::CBEnumFonts(LPLOGFONT lplf, LPTEXTMETRIC lptm, DWORD dwType, LPARAM lpData)
{
	// This function was written with the help of CCustComboBox, by Girish Bharadwaj.
	// Available from Codeguru.

	if (dwType == TRUETYPE_FONTTYPE) 
	{
		((CStringArray *) lpData)->Add( lplf->lfFaceName );
	}

	return true;
}

CString CAutoRichEditCtrl::GetSelectionFontName()
{
	CHARFORMAT cf = GetCharFormat();

	CString sName = cf.szFaceName;

	return sName;
}

long CAutoRichEditCtrl::GetSelectionFontSize()
{
	CHARFORMAT cf = GetCharFormat();

	long nSize = cf.yHeight/20;

	return nSize;
}

void CAutoRichEditCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//IDR_MENU_COPY_PASTE
// 	CMenu  mnu;
// 	CPoint posMouse;
// 	::GetCursorPos(&posMouse);
// 	mnu.LoadMenu (IDR_MENU_COPY_PASTE);
// 	CMenu* pMnu=mnu.GetSubMenu(0);
// 	if(pMnu)
// 	{
// 		(pMnu->TrackPopupMenu(TPM_RIGHTALIGN | TPM_RIGHTBUTTON  ,
// 			posMouse.x,posMouse.y,this, NULL));
// 	}

	CRichEditCtrl::OnRButtonUp(nFlags, point);
}

BOOL CAutoRichEditCtrl::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if(nCode!=0)
		return CRichEditCtrl::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	switch(nID)
	{
	case ID_EDIT_COPY:
		this->Copy ();
		break;
	case ID_EDIT_PASTE:
		if(this->CanPaste ())
			this->Paste ();
		break;
	case ID_EDIT_CUT:
		this->Cut ();
		break;
	default:
		return CRichEditCtrl::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	}
	return TRUE;
}

int CAutoRichEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}
///-----------------------------------------------
/// 2002-11-7
///	功能:
///		根据点获取对应的字符
///	入口参数:
///		pt					点位置
///	返回参数:
///		<0					失败
///		其他				最近的位置
int CAutoRichEditCtrl::GetCharFormPos(CPoint pt)
{
	POINTL tmpPoint;
	tmpPoint.x = pt.x;
	tmpPoint.y = pt.y;

	int nChar = ' ';
	CString StrText;
	GetWindowText(StrText);

	try
	{	
		nChar = SendMessage( EM_CHARFROMPOS,0,(LPARAM) &tmpPoint );
		nChar = LOWORD(nChar);
		
		// 如果是汉字,会多返回一个字
		if ( nChar<StrText.GetLength())
		{
			for ( int32 i = 0; i< nChar ; i++)
			{
				TCHAR ch = StrText.GetAt(i);
				if ( ch > 127 )
				{
					nChar -= 1;
				}
			}
		}
		else
		{
			int32 iPos = 0;
			int32 sum = 0;
			for ( int32 i = 0 ; i< StrText.GetLength(); i++)
			{
				if ( StrText.GetAt(i) > 127)
				{
					sum += 2;
				}
				else
				{
					sum += 1;				
				}
				iPos ++;
				if (sum == nChar)
				{
					nChar = iPos;
					break;
				}
			}
		}	
		if(nChar>0)
			nChar = nChar;
	}
	catch(...)
	{
		return -1;
	}
	
/*	if (nChar > StrText.GetLength() -1)
	{
		//AfxMessageBox(L"error!!");
		//return -1;
		nChar = StrText.GetLength() -1;
	}
	 CWnd * pp = GetParent();		
	 CString StrShow;
	 StrShow.Format(L"第%d个字符:%s",nChar,CString(StrText.GetAt(nChar)));

	 pp->SetWindowText(StrShow);
*/

	return nChar;
}

void CAutoRichEditCtrl::GetPosFormChar(CPoint &pt, int nIndex)
{
	try
	{
		DWORD dw = SendMessage( EM_POSFROMCHAR,(WPARAM)nIndex);
		pt.x = LOWORD(dw);
		pt.y = HIWORD(dw);
	}
	catch(...)
	{
		
	}

}

void CAutoRichEditCtrl::SetRESel( long nStart, long nEnd )
{
	if ( nEnd < 0 || nStart < 0 )
	{
		SetSel(nStart, nEnd);
	}
	else
	{
#ifndef UNICODE
		// 多字节字符，直接使用
		SetSel(nStart, nEnd);
#else
		// 宽字符，人为计算
		long nFixS = nStart;
		long nFixE = nEnd;
		
		CString StrText;
		GetWindowText(StrText);
		for ( int i=0; i < StrText.GetLength() && i<nEnd ; ++i )
		{
			TCHAR ch = StrText[i];
			if ( IsWideChar(ch) )
			{
				if ( i<nStart )
				{
					++nFixS;
				}
				++nFixE;
			}
		}
		SetSel(nFixS, nFixE);
#endif
	}
}
