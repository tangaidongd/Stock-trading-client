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

#define BACKGROUND_COR		RGB(0xFA, 0xF6, 0xED)			// 树控件的背景色
#define HIGHT_LIGHT_EDGE	RGB(245, 245, 220)				// 鼠标经过时的高度显示边缘颜色
#define HIGHT_LIGHT_MID		RGB(240, 230, 140)				// 鼠标经过时的高度显示中间颜色
#define COLOR_TEXT			Color(0xFF, 0x4D, 0x4D, 0x4D)	// 文字颜色
#define COLOR_TEXT_SELECT	Color(0xFF, 0x22, 0x7C, 0xED)	// 选中项的文字颜色

#define PICTURE_WIDTH			16							// 图标的宽度
#define WIDTH_SELECT_LEFT		4							// 选中项左侧色块的宽度
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

	CBitmap bitmap;	// 定义一个位图对象
	CDC MemeDc;	// 首先定义一个显示设备对象
	// 创建兼容设备DC，不过这时还不能绘图，因为没有地方画
	MemeDc.CreateCompatibleDC(&dc);	
	// 建立一个与屏幕显示兼容的位图，至于位图的大小嘛，可以用窗口的大小，也可以自己定义
	//（如：有滚动条时就要大于当前窗口的大小，在BitBlt时决定拷贝内存的哪部分到屏幕上）
	bitmap.CreateCompatibleBitmap(&dc, m_rect.Width(), m_rect.Height());
	// 将位图选入到内存显示设备中(只有选入了位图的内存显示设备才有地方绘图，画到指定的位图)
	CBitmap *pOldBitmap = MemeDc.SelectObject(&bitmap);
	// 先用背景色将位图清除干净
	if(m_iTreeStyle == EBT_Style2)	// 若是模拟交易
	{
		MemeDc.FillSolidRect(m_rect, RGB(236, 239, 243));
	}
	else
	{
		MemeDc.FillSolidRect(m_rect, BACKGROUND_COR); 
	}
 	//绘制想要显示的图片
 	DrawTreeItem(&MemeDc);
	
	dc.BitBlt( m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), &MemeDc, 0, 0, SRCCOPY);
	
	MemeDc.SelectObject(pOldBitmap);
	MemeDc.DeleteDC();
}
//计算偏移量
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
//绘制项
void CBeautifulTree::DrawTreeItem( CDC *pDC )
{
	HTREEITEM show_item = GetFirstVisibleItem();	// 获取第一个可见项的句柄
	if(show_item == NULL) return;
	CRect rc_item;		// 项的区域
	do
	{
		if(GetItemRect(show_item, rc_item, TRUE))	// 获取某一项的矩形区域
		{
			if (rc_item.top > m_rect.bottom)		// 不可见的时候,就不需要再绘制
			{
				break;
			}
			rc_item.right = m_rect.right;			// 使文字绘制区域最右边和树窗口的最右边等值
			CRect rect;
			rect.top	= rc_item.top;
			rect.bottom = rc_item.bottom - 1;
			rect.right	= m_h_size + m_h_offset;
			rect.left	= m_h_offset;
			//
			if(show_item == GetSelectedItem())		// 选中项的显示效果
			{
				m_hItemSelect = show_item;
				if(m_iTreeStyle == EBT_Style2)
				{
					CBrush brushRight(RGB(228, 228, 228));	// 右边的颜色
					pDC->FillRect(rect, &brushRight);
				}
				else
				{
					CBrush brushRight(RGB(224, 221, 213));	// 右边的颜色
					pDC->FillRect(rect, &brushRight);
				}
				CBrush brushLeft(RGB(34, 124, 237));	// 左边的颜色
				CRect rectLeft(rect);
				rectLeft.right = rectLeft.left + WIDTH_SELECT_LEFT;	
				pDC->FillRect(rectLeft, &brushLeft);
			}

			if (m_hItemMouseMove == show_item)		//鼠标经过项时，改变的项的背景颜色
			{
				if(m_hItemSelect != show_item)		// 如果是选中项，则鼠标经过不改变颜色
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
			//绘制文本
			DrawItemText(rc_item, show_item, pDC);
		}
	} while ((show_item = GetNextVisibleItem(show_item)) != NULL);
}
//绘制文字
void CBeautifulTree::DrawItemText(CRect rect, HTREEITEM hItem, CDC *pDc /* = NULL */)
{
	if (NULL == pDc) {	CClientDC dc(this);		pDc = &dc;	}
	
	// 绘制可展开项的标识图标
	Graphics graphics(pDc->m_hDC);
	if (ItemHasChildren(hItem))	// 如果有子节点，则绘制一个标识
	{
		RectF rcPicF( rect.right - rect.Width() / 3.0f - PICTURE_WIDTH, 
					  rect.top + (rect.Height() - PICTURE_WIDTH) / 2.0f,
					  PICTURE_WIDTH, PICTURE_WIDTH );
		if(TVIS_EXPANDED & GetItemState(hItem, TVIS_EXPANDED))	//树已经展开
		{
			if(m_hItemSelect == hItem)	// 选中显示彩色
			{
				graphics.DrawImage(m_pImageExpandColor, rcPicF);
			}
			else	// 未选中显示黑白
			{
				graphics.DrawImage(m_pImageExpandBlack, rcPicF);
			}
		}
		else	// 没有展开
		{
			if(m_hItemSelect == hItem)	// 选中显示彩色
			{
				graphics.DrawImage(m_pImageFoldColor, rcPicF);
			}
			else	// 未选中显示黑白
			{
				graphics.DrawImage(m_pImageFoldBlack, rcPicF);
			}
		}
	}
	// 绘制文字
	CString strText = GetItemText(hItem);
	CFontFactory fontFactory;	
	FontFamily fontFamily(fontFactory.GetExistFontName( L"微软雅黑" ));//...
	Gdiplus::Font font(&fontFamily, 13, FontStyleRegular, UnitPixel);	//显示Name的字体
	StringFormat stringFormat;
	stringFormat.SetLineAlignment(StringAlignmentCenter);
	stringFormat.SetTrimming(StringTrimmingEllipsisCharacter);

	Color colorText;	// 文字颜色
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

	rect.left += (18 + PICTURE_WIDTH + 2);	// 2是空隙 18是图标到左边的距离
	SolidBrush solidBrush(colorText);		//上面文字的颜色
	RectF rectF(float(rect.left), float(rect.top), float(rect.Width()), float(rect.Height()));
	graphics.DrawString(strText, -1, &font, rectF, &stringFormat, &solidBrush);
}
//绘制渐变色
void CBeautifulTree::DrawGradient(Graphics& graphics, COLORREF Color1, COLORREF Color2, CRect rect, int iRotation)
{
	//  取得第一种颜色的R，G，B值
	int r1 = GetRValue(Color1);
	int g1 = GetGValue(Color1);
	int b1 = GetBValue(Color1);	
	//  取得第二种颜色的R，G，B值
	int r2 = GetRValue(Color2);
	int g2 = GetGValue(Color2);
	int b2 = GetBValue(Color2);

	//  刷子
	Gdiplus::LinearGradientBrush linGrBrush(Gdiplus::Rect(rect.left, rect.top, rect.Width(), rect.Height()),  //  绘制区域
		Gdiplus::Color(255, r1, g1, b1),  //  第一种颜色
		Gdiplus::Color(255, r2, g2, b2),  //  第二种颜色 
		(Gdiplus::REAL)(90 - iRotation));  //  渐变色的角度

	graphics.FillRectangle(&linGrBrush, Gdiplus::Rect(rect.left, rect.top, rect.Width(), rect.Height()));
}
//鼠标移动的消息
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
		//开启鼠标悬停和离开消息
		TRACKMOUSEEVENT  tme = {0};
		tme.cbSize  = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;	//停留或离开这个窗口控件的区域才算停留或离开
		tme.dwHoverTime = 150;	//停留XXX毫秒，算是有效的
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
	HTREEITEM show_item = GetFirstVisibleItem();	// 获取第一个可见项的句柄
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
////lint 已检查过在ondestroy中已对其进行释放,在头文件中已对其-sem托管,仍未起作用.直接对其429错误忽略。 modifyer weng.cx
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

	HTREEITEM hItem = GetFirstVisibleItem();	// 获取第一个可见项的句柄
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