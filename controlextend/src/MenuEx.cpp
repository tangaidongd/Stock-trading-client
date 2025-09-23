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
	m_nSeparator = 10;	//sparator��Ĭ�ϸ߶�
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
//���˵���Ϊ������ʱ���ƻ�ɫ���ı�
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
//���Ʋ˵���λͼ
void CMenuEx::DrawMenuItemImage(CDC *pDC, CRect &rect, BOOL bSelected, BOOL bChecked,
								BOOL bGrayed, BOOL bHasImage,LPMENUITEM lpItem)
{
	CRect	rt(rect.left ,rect.top ,rect.left + m_szImage.cx + 4,
									rect.top + m_szImage.cy + 4);

	if(bChecked)
	{	
		if(bGrayed)
		{	
			//�˵�������
			GrayString(pDC,_T("��"),rt);
		}
		else
		{
			if(bSelected)
			{
				//�˵�ѡ��
				//�����ѡ�н������һ���������
				pDC->Draw3dRect(&rt,RGB(255,255,255),RGB(127,127,127));
			}

			rt.InflateRect(-2,-2);
			
			//����"��"
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_colText);
			pDC->DrawText(_T("��"),&rt,DT_EXPANDTABS|DT_VCENTER|DT_SINGLELINE);
		}
		
		rect.left +=m_szImage.cx + 4 +2 ;
		
		return ;
	}
	

	if(bHasImage)
	{
		CPoint pt(rt.left+2 , rt.top+2 );
		UINT	uStyle =ILD_TRANSPARENT;	//CImageList::Draw()����λͼ�ķ��
		if(bGrayed)
		{
			uStyle |=ILD_BLEND50;	//�˵�����������λͼ�ϰ�
		}
		else
		{
			if(bSelected)
			{
				//�����ѡ�н������һ���������
				pDC->Draw3dRect(&rt,RGB(255,255,255),RGB(127,127,127));
			}
		}

		m_ImageList.Draw(pDC,lpItem->uIndex,pt,uStyle);	//�ڲ˵����л���λͼ

		//�����ɻ��ƾ��εĴ�С
		//4��λͼ��Ӿ��α�λͼ��4
		//2���˵��ı���λͼ��Ӿ��εļ��Ϊ2
		rect.left  +=m_szImage.cx + 4 + 2;
	}
}
/////////////////////////////////////////////////
//���Ʋ˵����ı�
//������rect:������ε�RECT
//		rtText:�˵��ı���RECT
void CMenuEx::TextMenu(CDC *pDC, CRect &rect,CRect rtText,BOOL bSelected, BOOL bGrayed, LPMENUITEM lpItem)
{
	//ѡ��״̬�Ĳ˵���Ҫ�Ȼ����������
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

	int cy;	//�趨�ò˵���Ӧ���Ļ���

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

	//�趨����ɫ
	CBrush brush(m_colMenu);
	dc.FillRect(&rect, &brush);
	
	//�趨��ʾģʽ
	dc.SetBkMode(TRANSPARENT);

	//���Ʋ��λͼ
	DrawImageLeft(&dc,rect,lpItem);

	if(lpItem->uID==0)//�ָ���
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

		//�趨�˵��ı�������
		CRect	rtText(rect.left+m_szImage.cx+4+2, rect.top,rect.right ,rect.bottom );
		rtText.InflateRect(-2,-2);
					   
		//���Ʋ˵�λͼ
		DrawMenuItemImage(&dc,rect,bSelected,bChecked,bGrayed,bHasImage,lpItem);

		//���Ʋ˵��ı�
		TextMenu(&dc,rect,rtText,bSelected,bGrayed,lpItem);

#ifdef MENUCHAR
		//���뵱ǰ�˵�����
		m_currentListMenu.AddTail(lpItem);
#endif
	}

	dc.Detach();
}
//////////////////////////////////////////////////////////
//�ı�˵����
//ע��ڶ���������FALSE����ʾpMenuָ��Ĳ��ǵ�һ���˵�
void CMenuEx::ChangeStyle(CMenu *pMenu,CToolBar *pToolBar,BOOL bIsMainMenu)
{
	ASSERT(pMenu);
	if(NULL == pMenu)
	{
		return;
	}
	LPMENUITEM	lpItem = NULL;
	CMenu		*pSubMenu = NULL;
	int			m,nPosition=0;	//�ñ����������Ʋ��λͼ��λ��
	int inx;
	UINT	idx,x;

	for(int i=(int)pMenu->GetMenuItemCount()-1 ;i>=0; i--)
	{
		lpItem =new MENUITEM;

		lpItem->uID =pMenu->GetMenuItemID(i);
		if(!bIsMainMenu)	//���ǵ�һ���˵�
			lpItem->uPositionImageLeft =-1;//�������²˵���֧�ֲ��λͼ
		else
			lpItem->uPositionImageLeft =nPosition;

		if(lpItem->uID >0)
		{
			if(bIsMainMenu)
				nPosition +=m_szImage.cy+4;

			//����˵��ı�
			pMenu->GetMenuString(i,lpItem->strText,MF_BYPOSITION);

#ifdef MENUCHAR
			//����˵��ı���&����ַ�
			//���û����lpItem->uChrΪ0
			int ret=lpItem->strText.Find('&');
			lpItem->uChr =0;
			if(ret>=0)
				lpItem->uChr =lpItem->strText[ret+1];
			//�ַ�ͳһ�ɴ�С
			lpItem->uChr &=~0x20;
#endif
			
			//�ɹ�����λͼ��Ѱ�Ҳ˵����λͼ
			//���û����uIndexΪUINT_MAX
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

			//��������»����Ӳ˵�����ݹ���øú������޸����Ӳ˵��ķ��
			pSubMenu =pMenu->GetSubMenu(i);
			if(pSubMenu)
				ChangeStyle(pSubMenu,pToolBar);
			
		}
		else
		{
			if(bIsMainMenu)
				nPosition +=m_nSeparator;
		}
		//�޸Ĳ˵����Ϊ�Ի�
		pMenu->ModifyMenu(i,MF_BYPOSITION|MF_OWNERDRAW,lpItem->uID,(LPCTSTR)lpItem);
		
		m_ListMenu.AddTail(lpItem);
	}
}
//////////////////////////////////////////////////////////
//�ɹ�������λͼ�������˵����õ�λͼ�б�m_ImageList
int CMenuEx::GetImageFromToolBar(UINT uToolBar, CToolBar *pToolBar,COLORREF	crMask/*������λͼ������*/)
{
	if(!pToolBar)
		return 0;

	CBitmap	bmp;
	int nWidth = 0,nHeight = 0;
	BITMAP	bmpInfo;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	bmp.LoadBitmap(uToolBar);
	bmp.GetBitmap(&bmpInfo);
	//�õ�λͼ�ĸ߶�
	nHeight =bmpInfo.bmHeight;

	int nCount=0;
	int	ret =pToolBar->GetToolBarCtrl().GetButtonCount();

	//�õ���������λͼ�ĸ���nCount
	for(int i=0;i<ret;i++)
		if(pToolBar->GetItemID(i)!=ID_SEPARATOR)
			nCount ++;
	
	//�����λͼ�Ŀ��
	if (0 != nCount)
	{
		nWidth =bmpInfo.bmWidth/nCount;
	}
	
	bmp.DeleteObject();
	
	//����λͼ�б�
	m_ImageList.Create(uToolBar,nWidth,nHeight,crMask);
	m_szImage.cx =nWidth;
	m_szImage.cy =nHeight;

	return nCount;
}

void CMenuEx::InitMenu(CMenu *pMenu, UINT uToolBar, CToolBar *pToolBar)
{
	//���趨�˷��
	if(m_bInitial)
		return ;

	GetImageFromToolBar(uToolBar,pToolBar);

	CMenu	*pSubMenu = NULL,*pSubsub = NULL;
	MENUITEM	*lpItem = NULL;

	UINT	i;
	int		j,m;
	int 	nPosition;	//�ñ����������Ʋ��λͼ��λ��

	for(i=0;i<pMenu->GetMenuItemCount();i++)
	{
		pSubMenu =pMenu->GetSubMenu(i);

		if(pSubMenu)
		{
			nPosition =0;
			
			//ע��jһ��ҪΪint���ͣ����ΪUINT�Ǽ�鲻��j>=0!
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
					//����˵��ı���&����ַ�
					//���û����lpItem->uChrΪ0
					int ret =lpItem->strText.Find('&');
					lpItem->uChr=0;

					if(ret>=0)
						lpItem->uChr =lpItem->strText[ret+1];
					//ͳһ��С
					lpItem->uChr &=~0x20;
#endif
					//�ɹ�����λͼ��Ѱ�Ҳ˵����λͼ
					//���û����uIndexΪUINT_MAX
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

				//ֻ�е�һ���˵����ɹ��������λͼ!!
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

	if(lpItem->uID==0)//�ָ���
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
//�޸Ĳ˵��ķ��
//��InitMenu��ͬ���ǣ�InitMenu�����޸ĵ�һ���˵�Ϊ�Ի��񣬶�
//�ú����а�����һ���˵�
//������ע�⣺�������һʵ����ֻ�ܵ��������������е�һ��������һͬʹ��
void CMenuEx::InitPopupMenu(CMenu *pPopupMenu,UINT uToolBar, CToolBar *pToolBar)
{
	//���趨�������ڲ˵����
	if(m_bInitial)
		return ;

	GetImageFromToolBar(uToolBar,pToolBar);

	ChangeStyle(pPopupMenu,pToolBar);

	m_bInitial =TRUE;
}
