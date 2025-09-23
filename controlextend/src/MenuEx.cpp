// MenuEx.cpp: implementation of the CMenuEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MenuEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMenuEx::CMenuEx():m_szImage(16,15)
{
	m_colMenu =::GetSysColor(COLOR_MENU);
	m_colText =::GetSysColor(COLOR_MENUTEXT);
	m_colTextSelected =::GetSysColor(COLOR_HIGHLIGHTTEXT);

	m_bInitial =FALSE;	
	m_bHasImageLeft =FALSE;
	m_nSeparator = 10;	//sparator的默认高度
}

CMenuEx::~CMenuEx()
{
	m_ImageList.DeleteImageList();

	while(!m_ListMenu.IsEmpty())
		delete m_ListMenu.RemoveHead();

	if(m_bHasImageLeft)
		m_bmpImageLeft.DeleteObject();
}
/////////////////////////////////////////////////
//当菜单项为不可用时绘制灰色的文本
void CMenuEx::GrayString(CDC *pDC, const CString &str, const CRect rect)
{
	CRect	rt(rect);
	//int nMode =pDC->SetBkMode(TRANSPARENT);

	rt.left +=1;
	rt.top +=1;
		
	pDC->SetTextColor(RGB(255,255,255));
	pDC->DrawText(str,&rt,DT_EXPANDTABS|DT_VCENTER|DT_SINGLELINE);

	rt.left -=1;
	rt.top -=1;
	pDC->SetTextColor(RGB(127,127,127));
	pDC->DrawText(str,&rt,DT_EXPANDTABS|DT_VCENTER|DT_SINGLELINE);

	//pDC->SetBkMode(nMode);
}
/////////////////////////////////////////////////
//绘制菜单项位图
void CMenuEx::DrawMenuItemImage(CDC *pDC, CRect &rect, BOOL bSelected, BOOL bChecked,
								BOOL bGrayed, BOOL bHasImage,LPMENUITEM lpItem)
{
	CRect	rt(rect.left ,rect.top ,rect.left + m_szImage.cx + 4,
									rect.top + m_szImage.cy + 4);

	if(bChecked)
	{	
		if(bGrayed)
		{	
			//菜单不可用
			GrayString(pDC,_T("√"),rt);
		}
		else
		{
			if(bSelected)
			{
				//菜单选中
				//当该项被选中仅多绘制一个立体矩形
				pDC->Draw3dRect(&rt,RGB(255,255,255),RGB(127,127,127));
			}

			rt.InflateRect(-2,-2);
			
			//画出"√"
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_colText);
			pDC->DrawText(_T("√"),&rt,DT_EXPANDTABS|DT_VCENTER|DT_SINGLELINE);
		}
		
		rect.left +=m_szImage.cx + 4 +2 ;
		
		return ;
	}
	

	if(bHasImage)
	{
		CPoint pt(rt.left+2 , rt.top+2 );
		UINT	uStyle =ILD_TRANSPARENT;	//CImageList::Draw()绘制位图的风格
		if(bGrayed)
		{
			uStyle |=ILD_BLEND50;	//菜单不可用所以位图较暗
		}
		else
		{
			if(bSelected)
			{
				//当该项被选中仅多绘制一个立体矩形
				pDC->Draw3dRect(&rt,RGB(255,255,255),RGB(127,127,127));
			}
		}

		m_ImageList.Draw(pDC,lpItem->uIndex,pt,uStyle);	//在菜单项中绘制位图

		//调整可绘制矩形的大小
		//4：位图外接矩形比位图大4
		//2：菜单文本与位图外接矩形的间隔为2
		rect.left  +=m_szImage.cx + 4 + 2;
	}
}
/////////////////////////////////////////////////
//绘制菜单项文本
//参数：rect:立体矩形的RECT
//		rtText:菜单文本的RECT
void CMenuEx::TextMenu(CDC *pDC, CRect &rect,CRect rtText,BOOL bSelected, BOOL bGrayed, LPMENUITEM lpItem)
{
	//选中状态的菜单项要先画出立体矩形
	if(bSelected)
		pDC->Draw3dRect(&rect,RGB(127,127,127),RGB(255,255,255));	

	if(bGrayed)
	{
		GrayString(pDC,lpItem->strText,rtText);
	}
	else
	{
		pDC->DrawText(lpItem->strText,rtText,DT_LEFT|DT_EXPANDTABS|DT_VCENTER);
	}
}

void CMenuEx::DrawImageLeft(CDC *pDC, CRect &rect,LPMENUITEM lpItem)
{
	if(!m_bHasImageLeft || lpItem->uPositionImageLeft ==-1)
		return ;
	
	CDC  memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap	*oldBmp =(CBitmap *) memDC.SelectObject(&m_bmpImageLeft);

	int cy;	//设定该菜单项应从哪画起

	if(m_szImageLeft.cy >= lpItem->uPositionImageLeft + rect.Height())
	{
		cy =(int) m_szImageLeft.cy - lpItem->uPositionImageLeft - rect.Height();
		ASSERT(cy>=0);
	}
	else
		cy =0;

	pDC->BitBlt(rect.left ,rect.top ,m_szImageLeft.cx ,rect.Height(),&memDC,0,cy,SRCCOPY);

	memDC.SelectObject(oldBmp);
	memDC.DeleteDC();

	rect.left +=m_szImageLeft.cx+1;
}

void CMenuEx::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC	dc;
	LPMENUITEM lpItem;
	CRect rect(lpDIS->rcItem);
	dc.Attach(lpDIS->hDC);
	lpItem =(LPMENUITEM)lpDIS->itemData;

	if(lpDIS->itemState & ODS_SELECTED)
		dc.SetTextColor(m_colTextSelected);
	else
		dc.SetTextColor(m_colText);

	//设定背景色
	CBrush brush(m_colMenu);
	dc.FillRect(&rect, &brush);
	
	//设定显示模式
	dc.SetBkMode(TRANSPARENT);

	//绘制侧边位图
	DrawImageLeft(&dc,rect,lpItem);

	if(lpItem->uID==0)//分隔条
	{
		rect.top =rect.Height()/2+rect.top ;
		rect.bottom =rect.top +2;
		rect.left +=2;
		rect.right -=2;

		dc.Draw3dRect(rect,RGB(64,0,128),RGB(255,255,255));
	}
	else
	{

		BOOL	bSelected =lpDIS->itemState & ODS_SELECTED;
		BOOL	bChecked  =lpDIS->itemState & ODS_CHECKED;
		BOOL	bGrayed	  =lpDIS->itemState & ODS_GRAYED;
		BOOL	bHasImage =(lpItem->uIndex!=UINT_MAX);

		//设定菜单文本的区域
		CRect	rtText(rect.left+m_szImage.cx+4+2, rect.top,rect.right ,rect.bottom );
		rtText.InflateRect(-2,-2);
					   
		//绘制菜单位图
		DrawMenuItemImage(&dc,rect,bSelected,bChecked,bGrayed,bHasImage,lpItem);

		//绘制菜单文本
		TextMenu(&dc,rect,rtText,bSelected,bGrayed,lpItem);

#ifdef MENUCHAR
		//加入当前菜单链表
		m_currentListMenu.AddTail(lpItem);
#endif
	}

	dc.Detach();
}
//////////////////////////////////////////////////////////
//改变菜单风格
//注意第二个参数：FALSE：表示pMenu指向的不是第一级菜单
void CMenuEx::ChangeStyle(CMenu *pMenu,CToolBar *pToolBar,BOOL bIsMainMenu)
{
	ASSERT(pMenu);
	if(NULL == pMenu)
	{
		return;
	}
	LPMENUITEM	lpItem = NULL;
	CMenu		*pSubMenu = NULL;
	int			m,nPosition=0;	//该变量用来绘制侧边位图的位置
	int inx;
	UINT	idx,x;

	for(int i=(int)pMenu->GetMenuItemCount()-1 ;i>=0; i--)
	{
		lpItem =new MENUITEM;

		lpItem->uID =pMenu->GetMenuItemID(i);
		if(!bIsMainMenu)	//不是第一级菜单
			lpItem->uPositionImageLeft =-1;//二级以下菜单不支持侧边位图
		else
			lpItem->uPositionImageLeft =nPosition;

		if(lpItem->uID >0)
		{
			if(bIsMainMenu)
				nPosition +=m_szImage.cy+4;

			//保存菜单文本
			pMenu->GetMenuString(i,lpItem->strText,MF_BYPOSITION);

#ifdef MENUCHAR
			//保存菜单文本中&后的字符
			//如果没有则lpItem->uChr为0
			int ret=lpItem->strText.Find('&');
			lpItem->uChr =0;
			if(ret>=0)
				lpItem->uChr =lpItem->strText[ret+1];
			//字符统一成大小
			lpItem->uChr &=~0x20;
#endif
			
			//由工具栏位图中寻找菜单项的位图
			//如果没有则uIndex为UINT_MAX
			lpItem->uIndex =UINT_MAX;
			if(pToolBar)
			{
				for(m=0; m<(pToolBar->GetToolBarCtrl().GetButtonCount()) ;m++)
				{
					pToolBar->GetButtonInfo(m,idx,x,inx);

					if(idx==lpItem->uID)
					{
						lpItem->uIndex=inx;
						break;
					}
				}
			}

			//如果该项下还有子菜单，则递归调用该函数来修改其子菜单的风格
			pSubMenu =pMenu->GetSubMenu(i);
			if(pSubMenu)
				ChangeStyle(pSubMenu,pToolBar);
			
		}
		else
		{
			if(bIsMainMenu)
				nPosition +=m_nSeparator;
		}
		//修改菜单风格为自绘
		pMenu->ModifyMenu(i,MF_BYPOSITION|MF_OWNERDRAW,lpItem->uID,(LPCTSTR)lpItem);
		
		m_ListMenu.AddTail(lpItem);
	}
}
//////////////////////////////////////////////////////////
//由工具栏的位图来产生菜单所用的位图列表m_ImageList
int CMenuEx::GetImageFromToolBar(UINT uToolBar, CToolBar *pToolBar,COLORREF	crMask/*工具栏位图的掩码*/)
{
	if(!pToolBar)
		return 0;

	CBitmap	bmp;
	int nWidth = 0,nHeight = 0;
	BITMAP	bmpInfo;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	bmp.LoadBitmap(uToolBar);
	bmp.GetBitmap(&bmpInfo);
	//得到位图的高度
	nHeight =bmpInfo.bmHeight;

	int nCount=0;
	int	ret =pToolBar->GetToolBarCtrl().GetButtonCount();

	//得到工具栏中位图的个数nCount
	for(int i=0;i<ret;i++)
		if(pToolBar->GetItemID(i)!=ID_SEPARATOR)
			nCount ++;
	
	//计算出位图的宽度
	if (0 != nCount)
	{
		nWidth =bmpInfo.bmWidth/nCount;
	}
	
	bmp.DeleteObject();
	
	//创建位图列表
	m_ImageList.Create(uToolBar,nWidth,nHeight,crMask);
	m_szImage.cx =nWidth;
	m_szImage.cy =nHeight;

	return nCount;
}

void CMenuEx::InitMenu(CMenu *pMenu, UINT uToolBar, CToolBar *pToolBar)
{
	//已设定了风格
	if(m_bInitial)
		return ;

	GetImageFromToolBar(uToolBar,pToolBar);

	CMenu	*pSubMenu = NULL,*pSubsub = NULL;
	MENUITEM	*lpItem = NULL;

	UINT	i;
	int		j,m;
	int 	nPosition;	//该变量用来绘制侧边位图的位置

	for(i=0;i<pMenu->GetMenuItemCount();i++)
	{
		pSubMenu =pMenu->GetSubMenu(i);

		if(pSubMenu)
		{
			nPosition =0;
			
			//注意j一定要为int类型，如果为UINT是检查不出j>=0!
			for(j=(int)pSubMenu->GetMenuItemCount()-1;j>=0;j--)
			{
				lpItem =new MENUITEM;		

				lpItem->uID =pSubMenu->GetMenuItemID(j);
				lpItem->uPositionImageLeft =nPosition;

				if(lpItem->uID>0)
				{
					nPosition +=m_szImage.cy+4;

					pSubMenu->GetMenuString(j,lpItem->strText,MF_BYPOSITION);
#ifdef MENUCHAR
					//保存菜单文本中&后的字符
					//如果没有则lpItem->uChr为0
					int ret =lpItem->strText.Find('&');
					lpItem->uChr=0;

					if(ret>=0)
						lpItem->uChr =lpItem->strText[ret+1];
					//统一大小
					lpItem->uChr &=~0x20;
#endif
					//由工具栏位图中寻找菜单项的位图
					//如果没有则uIndex为UINT_MAX
					lpItem->uIndex =UINT_MAX;
					for(m=0; m<(pToolBar->GetToolBarCtrl().GetButtonCount()) ;m++)
					{
						int inx;
						UINT	idx,x;
						pToolBar->GetButtonInfo(m,idx,x,inx);

						if(idx==lpItem->uID)
						{
							lpItem->uIndex=inx;
							break;
						}
					}

				}
				else
				{
					//separator
					nPosition +=m_nSeparator;
				}
			
				m_ListMenu.AddTail(lpItem);

				pSubMenu->ModifyMenu(j,MF_BYPOSITION|MF_OWNERDRAW,
						lpItem->uID,LPCTSTR(lpItem));

				pSubsub =pSubMenu->GetSubMenu(j);

				//只有第一级菜单才由工具栏获得位图!!
				if(pSubsub)
					ChangeStyle(pSubsub,pToolBar);
			}
		}
	}

	m_bInitial =TRUE;
}

void CMenuEx::MeasureItem(LPMEASUREITEMSTRUCT  lpMIS)
{
	MENUITEM *lpItem =(LPMENUITEM)lpMIS->itemData;

	if(lpItem->uID==0)//分隔条
	{
		lpMIS->itemHeight =m_nSeparator;
		//lpMIS->itemWidth  =50;
	}
	else
	{
		CDC	 *pDC =AfxGetMainWnd()->GetDC();

		CString	strText=lpItem->strText;
		CSize  size;

		size=pDC->GetTextExtent(lpItem->strText);

		lpMIS->itemWidth = size.cx +m_szImage.cx+4;
		
		lpMIS->itemHeight =m_szImage.cy+4;
		
		AfxGetMainWnd()->ReleaseDC(pDC);
	}
}

void CMenuEx::SetImageLeft(UINT idBmpLeft)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_bmpImageLeft.LoadBitmap(idBmpLeft);
	m_bHasImageLeft = TRUE;
	BITMAP	bmpInfo;
	m_bmpImageLeft.GetBitmap(&bmpInfo);

	m_szImageLeft.cx =bmpInfo.bmWidth;
	m_szImageLeft.cy =bmpInfo.bmHeight;
}

#ifdef MENUCHAR
LRESULT CMenuEx::MenuChar(UINT nChar)
{
	nChar &=~0x20;
	MENUITEM *lpItem;

	for(POSITION pos=m_currentListMenu.GetHeadPosition();pos;)
	{
		lpItem =(LPMENUITEM)m_currentListMenu.GetNext(pos);

		if(lpItem->uChr ==nChar)
		{
			AfxGetMainWnd()->SendMessage(WM_COMMAND,lpItem->uID);
			return 1L;
		}
	}
	return 0L;
}
#endif

void CMenuEx::SetTextColor(COLORREF crColor)
{
	m_colText =crColor;
}

void CMenuEx::SetBackColor(COLORREF crColor)
{
	m_colMenu =crColor;
}

void CMenuEx::SetHighLightColor(COLORREF crColor)
{
	m_colTextSelected =crColor;
}

//////////////////////////////////////////////////////
//修改菜单的风格
//与InitMenu不同的是：InitMenu并不修改第一级菜单为自绘风格，而
//该函数有包括第一级菜单
//但必须注意：该类的任一实例都只能调用这两个函数中的一个，不能一同使用
void CMenuEx::InitPopupMenu(CMenu *pPopupMenu,UINT uToolBar, CToolBar *pToolBar)
{
	//已设定了主窗口菜单风格
	if(m_bInitial)
		return ;

	GetImageFromToolBar(uToolBar,pToolBar);

	ChangeStyle(pPopupMenu,pToolBar);

	m_bInitial =TRUE;
}
