// listboxex.cpp : implementation file
//

#include "stdafx.h"
#include "listboxex.h"
#include "GdiPlusTS.h"
#include "resource.h"
#include "TabSplitWnd.h"
#include "ShareFun.h"
//#include "facescheme.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListBoxEx

CListBoxEx::CListBoxEx()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_MERCH, L"PNG", m_pImageMerch))
	{
		m_pImageMerch = NULL;
	}

	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_BLOCK, L"PNG", m_pImageBlock))
	{
		m_pImageBlock = NULL;
	}

	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_INDEX, L"PNG", m_pImageIndex))
	{
		m_pImageIndex = NULL;
	}
	
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_SHORT_CUT, L"PNG", m_pImageShortCut))
	{
		m_pImageShortCut = NULL;
	}

	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_WSP, L"PNG", m_pImageWsp))
	{
		m_pImageWsp = NULL;
	}

	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_CFM, L"PNG", m_pImageCfm))
	{
		m_pImageCfm = NULL;
	}

	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_VMG, L"PNG", m_pImageVmg))
	{
		m_pImageVmg = NULL;
	}
}

CListBoxEx::~CListBoxEx()
{
	DEL(m_pImageWsp);
	DEL(m_pImageCfm);
	DEL(m_pImageVmg);
	DEL(m_pImageMerch);
	DEL(m_pImageBlock);
	DEL(m_pImageIndex);
	DEL(m_pImageShortCut);
}


BEGIN_MESSAGE_MAP(CListBoxEx, CListBox)
	//{{AFX_MSG_MAP(CListBoxEx)
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListBoxEx message handlers

void CListBoxEx::DrawItem(LPDRAWITEMSTRUCT pDIStruct) 
{ 
	
	CDC         dcContext;
 	CRect		rItemRect( pDIStruct -> rcItem );
 	CRect		rBlockRect( rItemRect );
	CRect		rIconRect;
	CRect		rTextRect;
	CBrush      brFrameBrush; 
 	int         iItem = pDIStruct -> itemID; 	
 	int         iState = pDIStruct -> itemState; 
 
 	if( !dcContext.Attach( pDIStruct -> hDC ) )  
 		return;                                  

 	CRect		rClient;
 	GetClientRect(&rClient);

	rIconRect = pDIStruct->rcItem;
	rIconRect.right = 16;

	rTextRect = pDIStruct->rcItem;
	rTextRect.left = rIconRect.right;


	if (iItem == -1)
	{
		dcContext.FillSolidRect(rClient,RGB(200,200,200));
	}
 
	if (iItem>=0)
	{
		T_ItemColor * itemclr = (T_ItemColor *)GetItemData(iItem);
		
		if ( ( NULL != itemclr ) && (0xffffffff != (DWORD)itemclr)  )
		{
			//... 设置了颜色的,画出颜色
			CString  StrText;
			GetText(iItem,StrText);
			
			dcContext.SetTextColor(itemclr->m_clrText);
			dcContext.SetBkMode(TRANSPARENT);
			
			dcContext.FillSolidRect(rTextRect,itemclr->m_clrBack);
			dcContext.DrawText(StrText,rTextRect,DT_LEFT);
			
			HICON hIcon =  AfxGetApp()->LoadIcon(IDB_TAB);
			dcContext.DrawIcon(0,0,hIcon);
			
			
			dcContext.SetTextColor(RGB(200,0,0));
			dcContext.FillSolidRect(rIconRect,RGB(150,150,150));
			
			
			Graphics GraphicImage(dcContext.GetSafeHdc());

			//lint --e{568} suppress "non-negative quantity is never less than zero"
			if (itemclr->m_eHKType>=0 && itemclr->m_eHKType<EHKTCount)
			{
				switch(itemclr->m_eHKType)
				{
				case EHKTMerch:
					{
						// HICON hIcon =  AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_KMERCH));
						// DrawIconEx(dcContext.GetSafeHdc(),rIconRect.left,rIconRect.top,hIcon,16,16,0,NULL,DI_NORMAL);
						
						DrawImage(GraphicImage, m_pImageMerch, rIconRect, 1, 0, false );
					}
					break;
				case EHKTShortCut:	
					{										
						// HICON hIcon =  AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_KSHOTCUT));
						// DrawIconEx(dcContext.GetSafeHdc(),rIconRect.left,rIconRect.top,hIcon,16,16,0,NULL,DI_NORMAL);										
						
						DrawImage(GraphicImage, m_pImageShortCut, rIconRect, 1, 0, false );
					}				
					break;
				case EHKTBlock:
				case EHKTLogicBlock:	// 共用一个嘿
					{					
						// HICON hIcon =  AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_KBLOCK));
						// DrawIconEx(dcContext.GetSafeHdc(),rIconRect.left,rIconRect.top,hIcon,16,16,0,NULL,DI_NORMAL);										
						
						DrawImage(GraphicImage, m_pImageBlock, rIconRect, 1, 0, false );
					}				
					break;
				case EHKTIndex:
					{
						// HICON hIcon =  AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_KKLINE));
						// DrawIconEx(dcContext.GetSafeHdc(),rIconRect.left,rIconRect.top,hIcon,16,16,0,NULL,DI_NORMAL);															
						DrawImage(GraphicImage, m_pImageIndex, rIconRect, 1, 0, false );
					}
					break;
				default:
					break;
				}			
			}
			if (itemclr->m_iFileType>=10 && itemclr->m_iFileType <13)
			{
				switch(itemclr->m_iFileType)
				{
				case 10:
					{
						// HICON hIcon =  AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_WSP));
						// DrawIconEx(dcContext.GetSafeHdc(),rIconRect.left,rIconRect.top,hIcon,16,16,0,NULL,DI_NORMAL);															
						
						DrawImage(GraphicImage, m_pImageWsp, rIconRect, 1, 0, false );
					}
					break;
				case 11:
					{
						// HICON hIcon =  AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_CFM));
						// DrawIconEx(dcContext.GetSafeHdc(),rIconRect.left,rIconRect.top,hIcon,16,16,0,NULL,DI_NORMAL);															

						DrawImage(GraphicImage, m_pImageCfm, rIconRect, 1, 0, false );
					}
					break;
				case 12:
					{
						// HICON hIcon =  AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_VMG));
						// DrawIconEx(dcContext.GetSafeHdc(),rIconRect.left,rIconRect.top,hIcon,16,16,0,NULL,DI_NORMAL);															

						DrawImage(GraphicImage, m_pImageVmg, rIconRect, 1, 0, false );
					}
					break;
				default:
					break;
				}
			}
			if (iState & ODS_SELECTED)
			{
				// CFont* pOldFont = dcContext.SelectObject(CFaceScheme::Instance()->GetSysFontObject ( ESFSmall ));
				
				dcContext.SetTextColor(RGB(255,255,0));
				dcContext.SetBkMode(TRANSPARENT);
				
				CRect rectDraw	 = rTextRect;
				rectDraw.bottom -= 2;

				dcContext.FillSolidRect(&rectDraw,RGB(0,0,0));
				dcContext.DrawText(StrText,rTextRect,DT_LEFT);				
				// dcContext.SelectObject(pOldFont);
				
				// CPen *pOldPen,LinePen;
				// LinePen.CreatePen(PS_SOLID,2,RGB(255,0,0));
				// pOldPen = dcContext.SelectObject(&LinePen);
				// dcContext.MoveTo(pDIStruct->rcItem.left,pDIStruct->rcItem.top);
				// dcContext.LineTo(pDIStruct->rcItem.right,pDIStruct->rcItem.top);
				// dcContext.LineTo(pDIStruct->rcItem.right,pDIStruct->rcItem.bottom);
				// dcContext.LineTo(pDIStruct->rcItem.left,pDIStruct->rcItem.bottom);
				// dcContext.LineTo(pDIStruct->rcItem.left,pDIStruct->rcItem.top);
				// dcContext.SelectObject(pOldPen);
				// LinePen.DeleteObject();
			}
		}
		else
		{
			//...没有设置的,使用默认颜色
			if (iItem>= 0)
			{
				CString  StrText;
				GetText(iItem,StrText);
				
				dcContext.SetTextColor(RGB(0,0,200));
				dcContext.SetBkMode(TRANSPARENT);
				
				dcContext.FillSolidRect(rTextRect,RGB(200,200,200));
				dcContext.DrawText(StrText,rTextRect,DT_LEFT);					
				
				dcContext.SetTextColor(RGB(255,0,0));
				dcContext.FillSolidRect(rIconRect,RGB(150,150,150));
				
				dcContext.DrawText(L"D",rIconRect,DT_CENTER);				
				if (iState & ODS_SELECTED)
				{
					CFont *font = (CFont*)CTabSplitWnd::m_pMainFram->SendMessage(UM_GetSysFontObject,(WPARAM)ESFNormal);
					CFont* pOldFont = dcContext.SelectObject(font/*CFaceScheme::Instance()->GetSysFontObject ( ESFNormal )*/);
					dcContext.SetTextColor(RGB(255,255,0));
					dcContext.SetBkMode(TRANSPARENT);				
					dcContext.FillSolidRect(rTextRect,RGB(0,0,0));
					dcContext.DrawText(StrText,rTextRect,DT_LEFT);
					dcContext.SelectObject(pOldFont);
					
					// 				CPen *pOldPen,LinePen;
					// 				LinePen.CreatePen(PS_DOT,1,RGB(255,0,0));
					// 				pOldPen = dcContext.SelectObject(&LinePen);
					// 				dcContext.MoveTo(pDIStruct->rcItem.left,pDIStruct->rcItem.top);
					// 				dcContext.LineTo(pDIStruct->rcItem.right,pDIStruct->rcItem.top);
					// 				dcContext.LineTo(pDIStruct->rcItem.right,pDIStruct->rcItem.bottom);
					// 				dcContext.LineTo(pDIStruct->rcItem.left,pDIStruct->rcItem.bottom);
					// 				dcContext.LineTo(pDIStruct->rcItem.left,pDIStruct->rcItem.top);
					// 				dcContext.SelectObject(pOldPen);
					// 				LinePen.DeleteObject();
					
				}
			}
		}
	}
	dcContext.Detach(); 		
}
void CListBoxEx::SetItemColor(int32 iIndex,COLORREF  clrText,COLORREF  clrBack,E_HotKeyType eHKType)
{
	T_ItemColor * pItemClr = new T_ItemColor;
	pItemClr->m_clrText = clrText;
	pItemClr->m_clrBack = clrBack;
	pItemClr->m_eHKType = eHKType;
	if(LB_ERR == SetItemData(iIndex,(DWORD)(pItemClr)))
	{
		// AfxMessageBox(L"ERROR");
	}
}
void CListBoxEx::SetItemColor(int32 iIndex,COLORREF  clrText,COLORREF  clrBack,int32  iFileType)
{
	T_ItemColor * pItemClr = new T_ItemColor;
	pItemClr->m_clrText = clrText;
	pItemClr->m_clrBack = clrBack;
	pItemClr->m_iFileType = iFileType;
	if(LB_ERR == SetItemData(iIndex,(DWORD)(pItemClr)))
	{
		// AfxMessageBox(L"ERROR");
	}
}
BOOL CListBoxEx::OnEraseBkgnd(CDC* pDC)
{
	// 
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect,RGB(200,200,200));

	return TRUE;
}