// BeautifulTree.cpp : implementation file
//
#include "stdafx.h"
#include "BeautifulTree.h"
#include "FontFactory.h"
//#include "PathFactory.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BACKGROUND_COR		RGB(0xFA, 0xF6, 0xED)			// ���ؼ��ı���ɫ
#define HIGHT_LIGHT_EDGE	RGB(245, 245, 220)				// ��꾭��ʱ�ĸ߶���ʾ��Ե��ɫ
#define HIGHT_LIGHT_MID		RGB(240, 230, 140)				// ��꾭��ʱ�ĸ߶���ʾ�м���ɫ
#define COLOR_TEXT			Color(0xFF, 0x4D, 0x4D, 0x4D)	// ������ɫ
#define COLOR_TEXT_SELECT	Color(0xFF, 0x22, 0x7C, 0xED)	// ѡ�����������ɫ

#define PICTURE_WIDTH			16							// ͼ��Ŀ��
#define WIDTH_SELECT_LEFT		4							// ѡ�������ɫ��Ŀ��
#define ReleasePicturMemory(p){if(p){delete p; p = NULL;}}

/////////////////////////////////////////////////////////////////////////////
// CBeautifulTree

CBeautifulTree::CBeautifulTree()
{
	m_hItemSelect     = NULL;
	m_hItemMouseMove  = NULL;
	m_IsInRegion = false;

	m_pImageExpandBlack = NULL;
	m_pImageExpandColor = NULL;
	m_pImageFoldBlack = NULL;
	m_pImageFoldColor = NULL;

	LoadExpandItemImage();

	m_iTreeStyle = EBT_Style1;
}

CBeautifulTree::~CBeautifulTree()
{
	ReleasePicturMemory(m_pImageExpandBlack);
	ReleasePicturMemory(m_pImageExpandColor);
	ReleasePicturMemory(m_pImageFoldBlack);
	ReleasePicturMemory(m_pImageFoldColor);
	m_hItemSelect = NULL;
	m_hItemMouseMove = NULL;
}

BEGIN_MESSAGE_MAP(CBeautifulTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CBeautifulTree)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBeautifulTree message handlers

void CBeautifulTree::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	Calculate();	//

	CBitmap bitmap;	// ����һ��λͼ����
	CDC MemeDc;	// ���ȶ���һ����ʾ�豸����
	// ���������豸DC��������ʱ�����ܻ�ͼ����Ϊû�еط���
	MemeDc.CreateCompatibleDC(&dc);	
	// ����һ������Ļ��ʾ���ݵ�λͼ������λͼ�Ĵ�С������ô��ڵĴ�С��Ҳ�����Լ�����
	//���磺�й�����ʱ��Ҫ���ڵ�ǰ���ڵĴ�С����BitBltʱ���������ڴ���Ĳ��ֵ���Ļ�ϣ�
	bitmap.CreateCompatibleBitmap(&dc, m_rect.Width(), m_rect.Height());
	// ��λͼѡ�뵽�ڴ���ʾ�豸��(ֻ��ѡ����λͼ���ڴ���ʾ�豸���еط���ͼ������ָ����λͼ)
	CBitmap *pOldBitmap = MemeDc.SelectObject(&bitmap);
	// ���ñ���ɫ��λͼ����ɾ�
	if(m_iTreeStyle == EBT_Style2)	// ����ģ�⽻��
	{
		MemeDc.FillSolidRect(m_rect, RGB(236, 239, 243));
	}
	else
	{
		MemeDc.FillSolidRect(m_rect, BACKGROUND_COR); 
	}
 	//������Ҫ��ʾ��ͼƬ
 	DrawTreeItem(&MemeDc);
	
	dc.BitBlt( m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), &MemeDc, 0, 0, SRCCOPY);
	
	MemeDc.SelectObject(pOldBitmap);
	MemeDc.DeleteDC();
}
//����ƫ����
void CBeautifulTree::Calculate()
{
	GetClientRect(&m_rect);
	SCROLLINFO scroll_info;
	// Determine window portal to draw into taking into account
	// scrolling position
	if ( GetScrollInfo( SB_HORZ, &scroll_info, SIF_POS | SIF_RANGE ) )
	{
		m_h_offset = -scroll_info.nPos;
		int iWndWidth = m_rect.Width();
		m_h_size = max( scroll_info.nMax + 1, iWndWidth);
	}
	else
	{
		m_h_offset = m_rect.left;
		m_h_size = m_rect.Width();
	}
	if ( GetScrollInfo( SB_VERT, &scroll_info, SIF_POS | SIF_RANGE ) )
	{
		if ( scroll_info.nMin == 0 && scroll_info.nMax == 100)
			scroll_info.nMax = 0;
		m_v_offset = -scroll_info.nPos * GetItemHeight();
		int iWndHeight = m_rect.Height() ;
		int iWndAndScrollHeight = (scroll_info.nMax+2)*((int)GetItemHeight()+1);
		m_v_size = max( iWndAndScrollHeight, iWndHeight );
	}
	else
	{
		m_v_offset = m_rect.top;
		m_v_size = m_rect.Height();
	}
}

void CBeautifulTree::OnSize( UINT nType, int cx, int cy) 
{
	CTreeCtrl::OnSize(nType, cx, cy);
	Calculate();
}

BOOL CBeautifulTree::OnEraseBkgnd( CDC* pDC ) 
{
	return true;
}
//������
void CBeautifulTree::DrawTreeItem( CDC *pDC )
{
	HTREEITEM show_item = GetFirstVisibleItem();	// ��ȡ��һ���ɼ���ľ��
	if(show_item == NULL) return;
	CRect rc_item;		// �������
	do
	{
		if(GetItemRect(show_item, rc_item, TRUE))	// ��ȡĳһ��ľ�������
		{
			if (rc_item.top > m_rect.bottom)		// ���ɼ���ʱ��,�Ͳ���Ҫ�ٻ���
			{
				break;
			}
			rc_item.right = m_rect.right;			// ʹ���ֻ����������ұߺ������ڵ����ұߵ�ֵ
			CRect rect;
			rect.top	= rc_item.top;
			rect.bottom = rc_item.bottom - 1;
			rect.right	= m_h_size + m_h_offset;
			rect.left	= m_h_offset;
			//
			if(show_item == GetSelectedItem())		// ѡ�������ʾЧ��
			{
				m_hItemSelect = show_item;
				if(m_iTreeStyle == EBT_Style2)
				{
					CBrush brushRight(RGB(228, 228, 228));	// �ұߵ���ɫ
					pDC->FillRect(rect, &brushRight);
				}
				else
				{
					CBrush brushRight(RGB(224, 221, 213));	// �ұߵ���ɫ
					pDC->FillRect(rect, &brushRight);
				}
				CBrush brushLeft(RGB(34, 124, 237));	// ��ߵ���ɫ
				CRect rectLeft(rect);
				rectLeft.right = rectLeft.left + WIDTH_SELECT_LEFT;	
				pDC->FillRect(rectLeft, &brushLeft);
			}

			if (m_hItemMouseMove == show_item)		//��꾭����ʱ���ı����ı�����ɫ
			{
				if(m_hItemSelect != show_item)		// �����ѡ�������꾭�����ı���ɫ
				{
					Graphics graphics(pDC->m_hDC);
					if(m_iTreeStyle == EBT_Style2)
					{
						DrawGradient(graphics, RGB(173, 207, 228), RGB(173, 207, 228), rect ,0);
					}
					else
					{
						DrawGradient(graphics, HIGHT_LIGHT_EDGE, HIGHT_LIGHT_MID, rect, 0);
					}
				}
			}
			//�����ı�
			DrawItemText(rc_item, show_item, pDC);
		}
	} while ((show_item = GetNextVisibleItem(show_item)) != NULL);
}
//��������
void CBeautifulTree::DrawItemText(CRect rect, HTREEITEM hItem, CDC *pDc /* = NULL */)
{
	if (NULL == pDc) {	CClientDC dc(this);		pDc = &dc;	}
	
	// ���ƿ�չ����ı�ʶͼ��
	Graphics graphics(pDc->m_hDC);
	if (ItemHasChildren(hItem))	// ������ӽڵ㣬�����һ����ʶ
	{
		RectF rcPicF( rect.right - rect.Width() / 3.0f - PICTURE_WIDTH, 
					  rect.top + (rect.Height() - PICTURE_WIDTH) / 2.0f,
					  PICTURE_WIDTH, PICTURE_WIDTH );
		if(TVIS_EXPANDED & GetItemState(hItem, TVIS_EXPANDED))	//���Ѿ�չ��
		{
			if(m_hItemSelect == hItem)	// ѡ����ʾ��ɫ
			{
				graphics.DrawImage(m_pImageExpandColor, rcPicF);
			}
			else	// δѡ����ʾ�ڰ�
			{
				graphics.DrawImage(m_pImageExpandBlack, rcPicF);
			}
		}
		else	// û��չ��
		{
			if(m_hItemSelect == hItem)	// ѡ����ʾ��ɫ
			{
				graphics.DrawImage(m_pImageFoldColor, rcPicF);
			}
			else	// δѡ����ʾ�ڰ�
			{
				graphics.DrawImage(m_pImageFoldBlack, rcPicF);
			}
		}
	}
	// ��������
	CString strText = GetItemText(hItem);
	CFontFactory fontFactory;	
	FontFamily fontFamily(fontFactory.GetExistFontName( L"΢���ź�" ));//...
	Gdiplus::Font font(&fontFamily, 13, FontStyleRegular, UnitPixel);	//��ʾName������
	StringFormat stringFormat;
	stringFormat.SetLineAlignment(StringAlignmentCenter);
	stringFormat.SetTrimming(StringTrimmingEllipsisCharacter);

	Color colorText;	// ������ɫ
	Image *pImage = NULL;
	DWORD nIndex = GetItemData(hItem);
	if(m_hItemSelect == hItem)
	{
		colorText = COLOR_TEXT_SELECT;
		if(m_vItemPicture.size() > nIndex && m_vItemPicture[nIndex])
		{
			pImage = m_vItemPicture[nIndex]->pImageColor;
		}
	}
	else
	{
		colorText = COLOR_TEXT;
		if(m_vItemPicture.size() > nIndex && m_vItemPicture[nIndex])
		{
			pImage = m_vItemPicture[nIndex]->pImageBlack;
		}
	}

	if(pImage)
	{
		RectF rcPic(18.0f, rect.top + (rect.Height() - PICTURE_WIDTH) / 2.0f, PICTURE_WIDTH, PICTURE_WIDTH);
		graphics.DrawImage(pImage, rcPic);
	}

	rect.left += (18 + PICTURE_WIDTH + 2);	// 2�ǿ�϶ 18��ͼ�굽��ߵľ���
	SolidBrush solidBrush(colorText);		//�������ֵ���ɫ
	RectF rectF(float(rect.left), float(rect.top), float(rect.Width()), float(rect.Height()));
	graphics.DrawString(strText, -1, &font, rectF, &stringFormat, &solidBrush);
}
//���ƽ���ɫ
void CBeautifulTree::DrawGradient(Graphics& graphics, COLORREF Color1, COLORREF Color2, CRect rect, int iRotation)
{
	//  ȡ�õ�һ����ɫ��R��G��Bֵ
	int r1 = GetRValue(Color1);
	int g1 = GetGValue(Color1);
	int b1 = GetBValue(Color1);	
	//  ȡ�õڶ�����ɫ��R��G��Bֵ
	int r2 = GetRValue(Color2);
	int g2 = GetGValue(Color2);
	int b2 = GetBValue(Color2);

	//  ˢ��
	Gdiplus::LinearGradientBrush linGrBrush(Gdiplus::Rect(rect.left, rect.top, rect.Width(), rect.Height()),  //  ��������
		Gdiplus::Color(255, r1, g1, b1),  //  ��һ����ɫ
		Gdiplus::Color(255, r2, g2, b2),  //  �ڶ�����ɫ 
		(Gdiplus::REAL)(90 - iRotation));  //  ����ɫ�ĽǶ�

	graphics.FillRectangle(&linGrBrush, Gdiplus::Rect(rect.left, rect.top, rect.Width(), rect.Height()));
}
//����ƶ�����Ϣ
void CBeautifulTree::OnMouseMove(UINT nFlags, CPoint point)
{
	CTreeCtrl::OnMouseMove(nFlags, point);
	HTREEITEM hItem = HitTest(point);
	if (hItem != NULL/* && hItem != m_hItemMouseMove*/)
	{
		m_hItemMouseMove = hItem;
		this->RedrawWindow();
		return ;
	}

	if(m_IsInRegion == false)
	{
		m_IsInRegion = true;
		//���������ͣ���뿪��Ϣ
		TRACKMOUSEEVENT  tme = {0};
		tme.cbSize  = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;	//ͣ�����뿪������ڿؼ����������ͣ�����뿪
		tme.dwHoverTime = 150;	//ͣ��XXX���룬������Ч��
		tme.hwndTrack = this->m_hWnd;
		_TrackMouseEvent(&tme);
	}
}

void CBeautifulTree::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	
}

LRESULT CBeautifulTree::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_hItemMouseMove = NULL;
	this->RedrawWindow();
	m_IsInRegion = false;

	return 0;
}

void CBeautifulTree::ExpandAllNode()
{
	HTREEITEM show_item = GetFirstVisibleItem();	// ��ȡ��һ���ɼ���ľ��
	if(show_item == NULL) return;
	do
	{
		if(ItemHasChildren(show_item))
		{
			Expand(show_item, TVE_EXPAND);
		}
	} while ((show_item = GetNextVisibleItem(show_item)) != NULL);
}

void CBeautifulTree::ToggleNode()
{
	CPoint point;
	GetCursorPos(&point); 
	ScreenToClient(&point);
	
	HTREEITEM hItem = HitTest(point);
	if (hItem != NULL)
	{
		Expand(hItem, TVE_TOGGLE);
	}
}

void CBeautifulTree::LoadExpandItemImage()
{
	if (!ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_MENU_EXPAND_COLOR, L"PNG", m_pImageExpandColor))
	{
		m_pImageExpandColor = NULL;
	}

	if (!ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_MENU_FOLD_COLOR, L"PNG", m_pImageFoldColor))
	{
		m_pImageFoldColor = NULL;
	}

	if (!ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_MENU_EXPAND_BLACK, L"PNG", m_pImageExpandBlack))
	{
		m_pImageExpandColor = NULL;
	}
	
	if (!ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_MENU_FOLD_BLACK, L"PNG", m_pImageFoldBlack))
	{
		m_pImageFoldColor = NULL;
	}
}
////lint �Ѽ�����ondestroy���Ѷ�������ͷ�,��ͷ�ļ����Ѷ���-sem�й�,��δ������.ֱ�Ӷ���429������ԡ� modifyer weng.cx
//lint --e{429} suppress "Custodial pointer 'p' (line 389) has not been freed or returned "
int CBeautifulTree::AddItemPicture(UINT idBlack, UINT idColor)
{
	itemPicture *p = new itemPicture;;
	
	if (idBlack == 0 || !ImageFromIDResource(AfxGetResourceHandle(),idBlack, L"PNG", p->pImageBlack))
	{
		p->pImageBlack = NULL;
	}

	if (idColor == 0 || !ImageFromIDResource(AfxGetResourceHandle(),idColor, L"PNG", p->pImageColor))
	{
		p->pImageColor = NULL;
	}

	m_vItemPicture.push_back(p);
	
	return m_vItemPicture.size();
}

void CBeautifulTree::OnDestroy()
{
	for(unsigned short i = 0; i < m_vItemPicture.size(); i++)
	{
		if(m_vItemPicture[i] == NULL)
		{
			continue;
		}

		if(m_vItemPicture[i]->pImageBlack)
		{
			ReleasePicturMemory(m_vItemPicture[i]->pImageBlack);
		}

		if(m_vItemPicture[i]->pImageColor)
		{
			ReleasePicturMemory(m_vItemPicture[i]->pImageColor);
		}
		delete m_vItemPicture[i];
		m_vItemPicture[i] = NULL;
	}

	CTreeCtrl::OnDestroy();
}

HTREEITEM CBeautifulTree::InsertItem(LPCTSTR lpszItem,UINT idBlack, UINT idColor,HTREEITEM hParent,HTREEITEM hInsertAfter)
{
	AddItemPicture(idBlack, idColor);

	return CTreeCtrl::InsertItem(lpszItem, hParent, hInsertAfter);
}

BOOL CBeautifulTree::SelectItem(int iIndex)
{
	if(iIndex < 0 || iIndex > (int)GetCount())
	{
		return FALSE;
	}

	HTREEITEM hItem = GetFirstVisibleItem();	// ��ȡ��һ���ɼ���ľ��
	if(hItem == NULL)	return FALSE;

	HTREEITEM hItemCur = GetSelectedItem();
	int iItem = -1;
	do
	{
		iItem = GetItemData(hItem);
		if(iIndex == iItem )
		{
			if (hItemCur != hItem)
			{
				BOOL bFlage = CTreeCtrl::SelectItem(hItem);
				if(bFlage)
				{
					this->RedrawWindow();
				}

				return bFlage;
			}
			else
			{
				return TRUE;
			}
			
		}
	} while ((hItem = GetNextVisibleItem(hItem)) != NULL);

	return FALSE;
}

void CBeautifulTree::SetTreeStyle(int iStyle)
{
	m_iTreeStyle = iStyle;
}