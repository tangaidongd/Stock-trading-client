#include "StdAfx.h"
#include <atlimage.h>
#include "OptionButton.h"

//CButton
map<COptionButton *,COptionButton *> COptionButton::m_BtnMessage;
Tstring COptionButton::m_srcPath;

#define W_T_A(wsrc,cDes)\
{\
	int nNumNeed = WideCharToMultiByte(CP_ACP,0,wsrc,-1,NULL,0,NULL,FALSE);\
	cDes = new char[nNumNeed + 1];\
	ZeroMemory(cDes,nNumNeed + 1);\
	WideCharToMultiByte(CP_ACP,0,wsrc,-1,cDes,nNumNeed + 1,NULL,FALSE);\
}\

#define DEL_WTA(cDes)\
{\
	delete []cDes;\
	cDes = NULL;\
}\

#define A_T_W(cSrc,wDes)\
{\
	int nNeed = MultiByteToWideChar(CP_ACP,0,cSrc,-1,NULL,0);\
	wDes = new wchar_t[nNeed + 1];\
	MultiByteToWideChar(CP_ACP,0,cSrc,-1,wDes,nNeed + 1);\
}\

#define DEL_ATW(wDes)\
{\
	delete wDes;\
}\

COptionButton::COptionButton()
{
	m_pParent = NULL;
	m_BtnMessage[this] = this;
	m_isHot = FALSE;
	m_isSel = FALSE;
	m_bLoadFramRes = FALSE;
}

COptionButton::~COptionButton()
{
	m_BtnMessage.erase(this);
	m_pParent = NULL;

}

BOOL COptionButton::Create(BTN_Info *pBtn,CWnd *pParent,CRect rtPos,BOOL bLoadFromRes )
{
	ASSERT(pBtn);
	ASSERT(pParent);
	if(!pBtn || !pParent) 
		return  FALSE;

	m_bLoadFramRes = bLoadFromRes;
	SetParent(pParent);
	SetBtnInfo(pBtn,FALSE);
	CopyRect(&rtPos);
	return TRUE;
}
void COptionButton::SetBtnInfo(BTN_Info *pBtn,BOOL IsRedraw)
{
	m_Btn.clr[0] = pBtn->clr[0];
	m_Btn.clr[1] = pBtn->clr[1];
	m_Btn.clr[2] = pBtn->clr[2];
	m_Btn.clr[3] = pBtn->clr[3];
	m_Btn.eimgPlace = pBtn->eimgPlace;
	memcpy(&m_Btn.font,&pBtn->font,sizeof(LOGFONT));
	m_Btn.Img = pBtn->Img;
	m_Btn.szText = pBtn->szText;
	m_Btn.uCountStatus = pBtn->uCountStatus;
	m_Btn.uGroupID = pBtn->uGroupID;
	m_Btn.uID = pBtn->uID;
	if(IsRedraw)
		OnPaint(0,0);
}

BTN_Info * COptionButton::GetBtnInfo()
{
	return &m_Btn;
}

void COptionButton::WindowProc(UINT message,WPARAM wParam,LPARAM lParam)
{
	switch(message)
	{
	case WM_PAINT:OnPaint(wParam,lParam);break;
	case WM_MOUSEMOVE:OnMouseMove(wParam,lParam);break;
	case WM_NCMOUSEMOVE:
	case WM_MOUSELEAVE:OnMouseLeave(wParam,lParam);break;
	case WM_LBUTTONUP:OnLButtonUp(wParam,lParam);break;
	default:
		break;
	}
}

void COptionButton::SetParent(CWnd *pParent)
{
	if(m_pParent == NULL)
	m_pParent = pParent;
}

CWnd *COptionButton::GetParent()
{
	return m_pParent;
}
void COptionButton::SetResourcePath(LPCTSTR srcPth)
{
	m_srcPath = srcPth;
}

void COptionButton::SetSel()
{
	SetSel(this);
}
void COptionButton::SetSel(COptionButton *PCurBtn)
{
	map<COptionButton *,COptionButton *>::iterator it= m_BtnMessage.begin();
	while(m_BtnMessage.end() != it)
	{
		if(it->first->m_isSel 
			&& it->first->m_Btn.uGroupID == PCurBtn->m_Btn.uGroupID 
			&& it->first->m_pParent == PCurBtn->m_pParent)
		{
			it->first->m_isSel = FALSE;
			it->first->OnPaint(0,0);
			break;
		}
		++it;
	}

	if(PCurBtn)
	{
		PCurBtn->m_isSel = TRUE;
		PCurBtn->OnPaint(0,0);
	}
}

void COptionButton::HandleMessage(CWnd *pParent,UINT message,WPARAM wParam,LPARAM lParam)
{
	map<COptionButton *,COptionButton *>::iterator it= m_BtnMessage.begin();
	while(m_BtnMessage.end() != it)
	{
		if(it->first->m_pParent == pParent)
		it->first->WindowProc(message,wParam,lParam);
		++it;
	}
}


void COptionButton::OnPaint(WPARAM wParam,LPARAM lParam)
{
	if(m_pParent == NULL) return;
	HDC hdc = ::GetDC(m_pParent->GetSafeHwnd());
	HDC hMemDC = CreateCompatibleDC(hdc);
	HBITMAP hMemBitmap = CreateCompatibleBitmap(hdc,Width(),Height());
	SelectObject(hMemDC,hMemBitmap);
	OnDraw(hMemDC);
	BitBlt(hdc,left,top,Width(),Height(),hMemDC,0,0,SRCCOPY);
	DeleteObject(hMemBitmap);
	DeleteDC(hMemDC);
	ReleaseDC(m_pParent->GetSafeHwnd(),hdc);
}

void COptionButton::OnDraw(HDC hdc)
{
	if ((0>=Width()) || (0>=Height()))
	{
		return;
	}

	int nSaveDC = SaveDC(hdc);
	Tstring szFilePath = _T("");
	if(!m_bLoadFramRes)
	{
		szFilePath = m_srcPath + m_Btn.Img;
	}
	else
	{
		szFilePath = m_Btn.Img;
	}

	COLORREF clr = m_Btn.clr[0];

	int nPart = 0;
	if(m_isSel)
	{
		nPart = 2;
		clr = m_Btn.clr[2];
	}
	else if(m_isHot)
	{
		nPart = 0;
		clr = m_Btn.clr[1];
	}
	else
	{
		nPart = 0;
		clr = m_Btn.clr[0];
	}

	nPart = min((UINT)nPart,m_Btn.uCountStatus - 1);

	CImage img;
	if(m_bLoadFramRes)
	{
		UINT uIDRes = 0;
		memcpy((void*)&uIDRes,(void*)szFilePath.c_str(),sizeof(uIDRes));
		img.LoadFromResource(AfxGetResourceHandle(),uIDRes);
	}
	else
	{
		img.Load(szFilePath.c_str());
	}

	if(!img.IsNull())
	{
		int nWitdh = (UINT)img.GetWidth()/(m_Btn.eimgPlace == IMAGE_HORIZONTAL ? m_Btn.uCountStatus:1);
		int nHeight= (UINT)img.GetHeight()/(m_Btn.eimgPlace == IMAGE_VERTICAL ? m_Btn.uCountStatus:1);

		if(m_Btn.eimgPlace == IMAGE_HORIZONTAL)
			img.Draw(hdc,0,0,Width(),Height(),nPart*nWitdh,0,nWitdh,nHeight);
		else
			img.Draw(hdc,0,0,Width(),Height(),0,nPart*nHeight,nWitdh,nHeight);
	}
	//»æ»­ÎÄ×Ö
	CRect rt(0,0,Width(),Height());
	CFont font;
	font.CreateFontIndirect(&m_Btn.font);
	SelectObject(hdc,font.m_hObject);
	SetBkMode(hdc,TRANSPARENT);
	SetTextColor(hdc,clr);
	DrawText(hdc,m_Btn.szText.c_str(),m_Btn.szText.size(),&rt,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	RestoreDC(hdc,nSaveDC);
}

void COptionButton::OnLButtonUp(WPARAM wParam,LPARAM lParam)
{
	POINT pt = {LOWORD(lParam),HIWORD(lParam)};
	if(PtInRect(pt))
	{
		SetSel(this);
		if(m_pParent)
		{
			m_pParent->PostMessage(UM_TBNCLICK,m_Btn.uID,(LPARAM)this);
		}
		
	}
}
void COptionButton::OnMouseMove(WPARAM wParam,LPARAM lParam)
{
	POINT pt = {LOWORD(lParam),HIWORD(lParam)};
	if(PtInRect(pt) && !m_isHot)
	{
		m_isHot = TRUE;
		OnPaint(0,0);
	}
	else if(!PtInRect(pt) && m_isHot)
	{
		m_isHot = FALSE;
		OnPaint(0,0);
	}
}
void COptionButton::OnMouseLeave(WPARAM wParam,LPARAM lParam)
{
	m_isHot = FALSE;
	OnPaint(0,0);
}