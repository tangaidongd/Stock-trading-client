// NewTextLable.cpp : implementation file
//

#include "stdafx.h"
#include "NewTextLable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CNewTextLable, CObject,VERSIONABLE_SCHEMA | 1)
/////////////////////////////////////////////////////////////////////////////
// CNewTextLable

CNewTextLable::CNewTextLable()
{
	m_rcPage = CRect(0,0,100,100);
	m_ptLeftTop = CPoint(10,10);
	m_ptRightBottom = CPoint(10,10);

	//ͼ�Ļ��ŵĶ��������
	m_GraphTextLineList.RemoveAll();	
	//ͼ�Ļ��ŵ����������
	m_TextFontList.RemoveAll();
	m_nPosx =0;
	m_nPosy = 0;
	m_nLineNum = 0;
	m_nListNum = 0;	
}

CNewTextLable::~CNewTextLable()
{
	POSITION pos = m_GraphTextLineList.GetHeadPosition();
	while (pos != NULL)
	{
		CGraphTextLine *pGtLine = m_GraphTextLineList.GetNext(pos);
		if( pGtLine )
		{
			delete pGtLine;
			pGtLine = NULL;
		}
	}
	m_GraphTextLineList.RemoveAll();
	
	
	pos = m_TextFontList.GetHeadPosition();
	while (pos != NULL)
	{
		CGraphTextFont *pGtFont = m_TextFontList.GetNext(pos);
		if( pGtFont )
		{
			delete pGtFont;
			pGtFont = NULL;
		}
	}
	m_TextFontList.RemoveAll();
}

/*
BEGIN_MESSAGE_MAP(CNewTextLable, CWnd)
	//{{AFX_MSG_MAP(CNewTextLable)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()*/


/////////////////////////////////////////////////////////////////////////////
// CNewTextLable message handlers
//��ȡ���Ĺ�����
CPoint CNewTextLable::GetMaxSize( )
{
	CPoint pos(0,0);
	POSITION postemp = m_GraphTextLineList.GetTailPosition();
	if( postemp )
	{
		CGraphTextLine *pTextLine = m_GraphTextLineList.GetAt( postemp );
		if( pTextLine )
		{
			POSITION postemp1 = pTextLine->GetCharList()->GetTailPosition();
			if( postemp1 )
			{
				CGraphTextChar *pChar = pTextLine->GetCharList()->GetAt( postemp1);
				if( pChar)
				{
					pos.y = pChar->GetCharShowY() + pChar->FontHeight();
				}
			}
		}
	}	

	pos.x = m_rcPage.Width() - m_ptLeftTop.x - m_ptRightBottom.x; 

	return pos;
}

//��ȡ���Ĺ�����
CPoint CNewTextLable::GetTextMaxSize( )
{
	CPoint ptSize = GetMaxSize( );
	return ptSize;
}


//��ȡ�����ľ���
CPoint CNewTextLable::GetTextScrollPos()
{
	//��ȡ�����ľ���
	CPoint ptScroll = GetScrollPos();
	return ptScroll;
}

//��ȡ�����ľ���
CPoint CNewTextLable::GetScrollPos()
{
	return m_ptScrollPos;
}

//���ù�������

void CNewTextLable::SetScrollPos(CPoint pt)
{
	m_ptScrollPos = pt ;
}

//���ù�������
void CNewTextLable::SetTextScrollPos(CPoint pt)
{
	SetScrollPos( pt );
}

void CNewTextLable::Draw(CDC *pDC,CRect rect,CPoint ptScroll,
						int nstar,int nend,
						float fScalex,float fScaley,int nMode)
{
	ASSERT(pDC);

	CPoint pt = rect.TopLeft();
	CPoint ptOffset = pt;

    CRect rcSroll = rect;
	rcSroll += ptScroll;

	CPoint ptTextLeftTop = m_ptLeftTop;	
	ptOffset.x += (int)(ptTextLeftTop.x * fScalex );


	POSITION position=NULL;
	//��ʾ���ţ�����ͼƬ����ʽ���ַ���Ole��
	for( position = m_GraphTextLineList.GetHeadPosition() ; position != NULL ; )
	{
		CGraphTextLine* pTextLine=m_GraphTextLineList.GetAt(position);
		if(pTextLine)
		{
			if( pTextLine->IsShow(rcSroll, ptOffset ,fScalex,fScaley))
			{	
				DrawScaleHorFromLeft(pDC,pTextLine,ptOffset,ptScroll,nstar,nend,fScalex,fScaley,nMode);	
			}
		}
		m_GraphTextLineList.GetNext(position);
	}	
}

void CNewTextLable::DrawScaleHorFromLeft(CDC *pDC,CGraphTextLine *pTextLine,
					CPoint ptOffSet,CPoint ptScroll,int nSelectBegin,int nSelectEnd,
					float fScaleX,float fScaleY, int nMode )
{
	ASSERT(pDC);
	ASSERT(pTextLine);
	if(NULL == pDC || NULL == pTextLine)
	{
		return;
	}
	


	int nListSpace = (int)(pTextLine->GetListSpace() * fScaleX); //�о�

	CGraphTextCharList *pCharList = GetGraphTextCharList (pTextLine);  //��ȡ��������

	if(!pCharList->GetCount()) return ; //��ѯ���ŵ������Ƿ�Ϊ0

	int cyPixels = 96;

	//if( pDC->IsKindOf( RUNTIME_CLASS( CMetaFileDC )))
	//	cyPixels = 96;
	//else
	cyPixels = pDC->GetDeviceCaps (LOGPIXELSY);

	CString strFontName = _T("����");
//	strFontName.LoadString(IDS_FONT_NAME);
	LOGFONT  LogFontDef = { 16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
					 DEFAULT_CHARSET,//GB2312_CHARSET,
					 OUT_DEFAULT_PRECIS,
					 CLIP_DEFAULT_PRECIS,
					 DEFAULT_QUALITY,
					 DEFAULT_PITCH | FF_DONTCARE,
					 //"����" 
				}; //�����ʼ��
	wcscpy(LogFontDef.lfFaceName,strFontName);
	//LogFontDef.lfHeight= -MulDiv (12, cyPixels, 72);

	CFont						m_font;
	CFont*                      oldFont=NULL;

	int nFontNum=-1;
	m_font.DeleteObject();
	m_font.CreateFontIndirect (&LogFontDef);
	oldFont=pDC->SelectObject(&m_font);//���ó�ʼ����
	pDC->SetBkMode(TRANSPARENT);
	
	for(POSITION position=pCharList->GetHeadPosition();position!=NULL;)
	{
		CGraphTextChar* pchar=pCharList->GetAt(position);
		if( pchar )
		{
			CString strText = pchar->GetStrText() ;
			int nX = (int)(pchar->GetCharX() * fScaleX+0.5);
			int nY = (int)(pchar->GetCharY() * fScaleY+0.5);
			int nShowY = (int)(pchar ->GetCharShowY() * fScaleY +0.5);		
			int nWidth = (int) (pchar ->GetWidth() * fScaleX+0.5);		
			int nFontH = (int)( pchar->FontHeight() * fScaleY+0.5);
			{
				if( nFontNum != pchar->GetFontIndex() )//��������ĸı�
				{
					CGraphTextFont font;
					GetPfsFont(pchar->GetFontIndex(),&font);
					font.GetLogFont()->lfHeight = (int)(font.GetLogFont()->lfHeight * fScaleY );

					nFontNum=pchar->GetFontIndex();		

					if(oldFont)
						pDC->SelectObject(oldFont);

					m_font.DeleteObject();
					
					m_font.CreateFontIndirect ( font.GetLogFont() );
					oldFont=pDC->SelectObject(&m_font);
					
					pDC->SetTextColor( font.GetFontColor() );  //����ı䴦�����
				}//end if //�����������ɫ
			}

			CPoint ptDetaOffSet = ptOffSet - ptScroll;

			if(nSelectBegin < nSelectEnd) //����ѡ�е����
			{
				if( pchar->GetCaretId() >= nSelectBegin && pchar->GetCaretId() < nSelectEnd)
				{
					if(  strText == '\r' || strText == '\n')
					{
						
					}
					else
					{
						COLORREF oldcolor=pDC->GetTextColor();
						COLORREF oldbkcolor=pDC->GetBkColor();

						pDC->SetBkColor(oldbkcolor& 0x000000);
						COLORREF colorshade=(oldbkcolor& 0x000000); //��ɫ����
						CBrush brushshade(colorshade);

						CRect rectshade( nX - nListSpace -1,nY,
							nX + nWidth + nListSpace + 1,
							nShowY + nFontH );

						rectshade += ptDetaOffSet;

						pDC->FillRect(rectshade,&brushshade);
						pDC->SetTextColor((oldcolor+0xffffff)& 0xffffff);
						
						//pDC->TextOut(nShowX+ptDetaOffSet.x,nShowY+ptDetaOffSet.y,strText);
						
						pchar->DrawScale(pDC,ptDetaOffSet.x,ptDetaOffSet.y,fScaleX,fScaleY,1);

						pDC->SetBkMode(TRANSPARENT);
						pDC->SetBkColor(oldbkcolor);
						pDC->SetTextColor(oldcolor);
					}
				}
				else  //������ʾ
				{
					pchar->DrawScale(pDC,ptDetaOffSet.x,ptDetaOffSet.y,fScaleX,fScaleY,0);
				}
			}				
			else  //������ʾ
			{
				pchar->DrawScale(pDC,ptDetaOffSet.x,ptDetaOffSet.y,fScaleX,fScaleY,0);
			}
		} //end if pchar
		pCharList->GetNext(position); //����ѭ��
	}//end for

	if(oldFont) //�ָ�dc������
	{
		pDC->SelectObject(oldFont);
		m_font.DeleteObject();
	}
}


void CNewTextLable::InitStatus(const CRect &rcPage,const CPoint &ptLefttop,const CPoint &ptRightBottom)
{
	m_rcPage = rcPage;
	m_ptLeftTop = ptLefttop;
	m_ptRightBottom = ptRightBottom;

	int numcount = m_TextFontList.GetCount();
	if(numcount == 0)
	{
		CGraphTextFont *pFont=new CGraphTextFont;
		if(pFont)
			m_TextFontList.AddTail(pFont);
	}

	numcount = m_GraphTextLineList.GetCount();	
	if(numcount == 0)
	{
		CGraphTextLine *pTextLine=new CGraphTextLine;
		if(pTextLine)
		{
			CGraphTextChar *pchar=new CGraphTextChar;
			if(pchar)
			{
				pchar->SetCaretId( 0 );
				pchar->SetFontIndex( 1 );
				pchar->SetCharLine( 1 );
				pchar->SetCharList( 1 );
				pchar->SetStrText( _T("\n" ));
				AddEnterFlag(pchar);
				pTextLine->GetCharList()->AddTail(pchar);
			}
			pTextLine->SetBeiginId ( 0 );
			pTextLine->SetEndId ( 0 ) ;
			m_GraphTextLineList.AddTail(pTextLine);
		}
	}
}

void CNewTextLable::SetNewsText(const CString &strText,long nTextId,CGraphTextFont *pFont,CGraphTextLine::GrapText_MkMode nModl)
{
	CGraphTextFont font;
	if( pFont )
		font.CopyFrom(pFont);

	CString strTextTemp = strText;
	if( strTextTemp.IsEmpty())
		return;

	long nId = 0;
	if( nTextId >= 0)
		nId = nTextId;

	bool bReturnFlag = FALSE;
	POSITION currentpos=NULL;
	CGraphTextLine *pTextLine = GetGraphTextLine(nId,&currentpos);
	if( pTextLine == NULL )
	{
		if( strText == "\n" /*|| pchar->GetStrText() == "\r"*/)
		{
			CGraphTextChar *pchar=new CGraphTextChar();
			if(pchar)
			{
				////////////////////////////���崦��
				CGraphTextFont hfont;
				int numfont = GetFontIndex(&hfont);
				pchar->SetFontIndex( numfont );
				///////////////////////////////���崦�����
				pchar->SetStrText( strText );
				pTextLine = InsterChar(nId,pchar);
				ASSERT(pTextLine);
				bReturnFlag = TRUE;
			}
		}
	}
	if( pTextLine )
	{
		pTextLine->SetMakeUpType( nModl );
		pTextLine->SetLineSpace(15);	// ������Ʒ���о�
		pTextLine->SetListSpace(1);	// �ּ��
	}

	if( bReturnFlag )
		return ;

	int nLength = strTextTemp.GetLength();
	for( int i = 0; i < nLength; i++ )
	{
		InsterOneChar( CString(strTextTemp[i]), &font , nId );
		nId+=1;
	}
}

void CNewTextLable::AddLineFlag(long nTextId,CGraphTextFont *pFont,int nMode)
{
	CGraphTextFont font;
	if( pFont )
		font.CopyFrom(pFont);
	
	CString strTextTemp = _T("\n");
	if( strTextTemp.IsEmpty())
		return;
	
	long nId = 0;
	if( nTextId >= 0)
		nId = nTextId;
	
	POSITION currentpos=NULL;
	CGraphTextLine *pTextLine = GetGraphTextLine(nId,&currentpos);
	if( pTextLine )
		pTextLine->SetMakeUpType( CGraphTextLine::GRPAHTXT_MKMODE_LEFT );
	
	int nLength = strTextTemp.GetLength();
	for( int i = 0; i < nLength; i++ )
	{
		CGraphTextCharLineFlag *pchar=new CGraphTextCharLineFlag();
		if(pchar)
		{
			pchar->SetDrawModel(nMode);
			pchar->SetLineW(m_rcPage.Width()-m_ptLeftTop.x - m_ptRightBottom.x );
			////////////////////////////���崦��
			int numfont= GetFontIndex(&font);
			pchar->SetFontIndex( numfont );
			///////////////////////////////���崦�����
			CString strTemp;
			strTemp.Format(_T("%c"),strTextTemp[i]);
			pchar->SetStrText( strTemp );
			InsterChar(nId,pchar,FALSE);
		}
		nId+=1;
	}
}


void CNewTextLable::MakeUpText()
{
	HDC hDC = ::GetDC(NULL);
	if( hDC )
	{
		MakeUpText(CDC::FromHandle(hDC),0,FALSE);
		::ReleaseDC(NULL,hDC);
	}
}

//����һ���ַ�
void CNewTextLable::InsterOneChar(CGraphTextFont *pfont,CGraphTextChar *pchar,long nID)
{
	ASSERT( pfont );
	ASSERT( pchar );
	
	if( nID < 0 )
		nID = 0;
	
	if(pchar)
	{
		int numfont= GetFontIndex (pfont);
		pchar->SetFontIndex( numfont );
		pchar->SetCaretId ( nID );
		InsterChar(nID,pchar);
	}
}

//����һ���ַ�
CGraphTextChar  *CNewTextLable::InsterOneChar(CString m_text,CGraphTextFont *pfont,long nID)
{
	ASSERT(m_text.GetLength() > 0);
	ASSERT(pfont);
	
	if( nID < 0 ) 
		nID = 0;
	
    CGraphTextChar *pchar=new CGraphTextChar();
	if(pchar)
	{
		////////////////////////////���崦��
		int numfont= GetFontIndex(pfont);
		pchar->SetFontIndex( numfont );
		///////////////////////////////���崦�����
		pchar->SetStrText( m_text );
		InsterChar(nID,pchar);
	}
	return pchar;
}

//����һ���ַ�
CGraphTextLine * CNewTextLable::InsterChar(int nId,CGraphTextChar *pchar,bool bFlagEnter)
{
	ASSERT(pchar);
	if( nId < 0 )
		nId = 0;
	if (NULL == pchar)
	{
		return NULL;
	}
	int numcount=m_GraphTextLineList.GetCount();
	ASSERT(numcount);

	POSITION currentpos=NULL;
	
	CGraphTextLine *pRetTextLine = NULL;
	CGraphTextLine *pTextLine = GetGraphTextLine(nId,&currentpos);
	
	if( pTextLine == NULL )
	{
		//CHaGraphTextBaseChar *ptempchar = (CHpGraphTextBaseChar *)pchar;
		if( pchar->GetStrText() == "\n" /*|| pchar->GetStrText() == "\r"*/)
		{
			if( bFlagEnter )
				AddEnterFlag(pchar);
			pTextLine=new CGraphTextLine(nId,pchar);
			if(pTextLine)
			{
				currentpos = m_GraphTextLineList.AddTail( pTextLine );
				pRetTextLine = pTextLine;
			}
		}
	}
	else if( pTextLine )
	{
		//CHaGraphTextPfsChar *ptempchar = (CHpGraphTextPfsChar *)pchar;
		if( pchar->GetStrText() == "\n"/* || pchar->GetStrText() == "\r"*/)
		{
			if( bFlagEnter )
				AddEnterFlag(pchar);
			
			CGraphTextLine *pTextLineTemp=new CGraphTextLine;//(nId,ptempchar);
			ASSERT(pTextLineTemp);
			pTextLine->InsterChar(nId,pchar);

			InsterTextLine(nId,pTextLine,pTextLineTemp);

			pTextLineTemp->SetBeiginId ( nId+1 );
			int count = pTextLineTemp->GetCharList()->GetCount();
			pTextLineTemp->SetEndId ( nId + count );
			pTextLineTemp->SetLineSpace ( pTextLine->GetLineSpace()) ;
			pTextLineTemp->SetListSpace ( pTextLine->GetListSpace()) ;

			currentpos = m_GraphTextLineList.InsertAfter(currentpos,pTextLineTemp);
			pRetTextLine = pTextLineTemp;
		}
		else
		{
			pchar->SetCaretId ( nId );
			pTextLine->InsterChar(nId,pchar);	
		}
	}
	
	if(	currentpos)
	{
		m_GraphTextLineList.GetNext(currentpos);
		while(currentpos != NULL)
		{	
			m_GraphTextLineList.GetNext(currentpos)->MoveId(nId);
		}
	}

	return pRetTextLine;
}

void CNewTextLable::AddEnterFlag(CGraphTextChar *pchar)
{
	ASSERT ( pchar );
	if(NULL != pchar)
	{
		if( pchar->GetStrText() == "\n" || pchar->GetStrText() == "\r")
		{
			CGraphTextFont hfont;
			int numfont = GetFontIndex(&hfont);
			pchar->SetFontIndex ( numfont );
		}
	}	
}

void CNewTextLable::InsterTextLine(long nID,CGraphTextLine *pTextLine,CGraphTextLine *pTextLineTemp)
{
	ASSERT ( pTextLine );
	ASSERT ( pTextLineTemp );
	if (NULL == pTextLineTemp || NULL == pTextLine)
	{
		return;
	}
	if( nID< 0 )
		nID = 0;
	
	if ( pTextLine->GetBeiginId() <= nID && pTextLine->GetEndId() > nID )
	{
		CGraphTextCharList *pCharList = pTextLine->GetCharList();
		if ( pCharList == NULL )
			return;
		
		for( POSITION posTail = pCharList->GetTailPosition(); posTail!=NULL; )
		{
			CGraphTextChar *pChar = pCharList->GetAt(posTail);
			if(pChar)
			{
				if( pChar->GetCaretId() > nID )
				{
					int nEnd = pTextLine->GetEndId() ;
					nEnd--;
					pTextLine->SetEndId( nEnd );
					pTextLineTemp->GetCharList()->AddHead(pChar);
					pCharList->RemoveAt(posTail);
				}
				else
				{
					if( pChar->GetCaretId() == nID )
					{
						pTextLineTemp->SetBeginLine( pChar->GetCharLine() + 1 );
						pTextLineTemp->SetEndLine( pTextLine->GetEndLine() );
						
						pTextLine->SetEndLine( pChar->GetCharLine()+1 );
					}
					return ;
				}
			}
			pCharList->GetPrev(posTail);
		}
	}
}

//���䴦�����鿪ʼ
CGraphTextLine *CNewTextLable::GetGraphTextLine(long nID,POSITION *poscurrent)
{
	CGraphTextLine *pTextLine = NULL;
	
	if ( nID == -1)
		nID =0;
	
	for(POSITION pos = m_GraphTextLineList.GetHeadPosition();pos!=NULL;)
	{
		pTextLine = m_GraphTextLineList.GetAt(pos);
		if(pTextLine)
		{
			if(pTextLine->GetBeiginId() <= nID && pTextLine->GetEndId() >= nID)
			{
				*poscurrent = pos;
				return pTextLine;
			}
		}
		m_GraphTextLineList.GetNext(pos);
	}
	return NULL;
}


//��������������Ų�ѯ�����ָ��
void CNewTextLable::GetPfsFont(int num,CGraphTextFont *pFont)
{
	ASSERT(num);
	ASSERT(pFont);
	if (NULL == pFont)
	{
		return;
	}
	int numfont=m_TextFontList.GetCount();
	
	if( num > numfont ) return;
	
	POSITION position = NULL;
	//lint --e{441} suppress "for clause irregularity: loop variable 'numfont' not found in 2nd for expression: ��forѭ���еĵ�2�����ʽδ����numfont"
	for(position=m_TextFontList.GetHeadPosition(),numfont=1;position!=NULL;numfont++)
	{
		CGraphTextFont* pBlock=m_TextFontList.GetAt(position);
		if( NULL != pBlock && numfont == num )
		{
			pFont->CopyFrom(pBlock);
			break;
		}
		m_TextFontList.GetNext(position);
	}
}

int  CNewTextLable::GetFontIndex(CGraphTextFont *pFont)
{
	////////////////////////////���崦��
	int numfont = IsNewFont( pFont );
	if(numfont==-1)
	{
		CGraphTextFont *ptfont=new CGraphTextFont;
		if(ptfont)
		{
			ptfont->CopyFrom( pFont );
			m_TextFontList.AddTail( ptfont );
			numfont = m_TextFontList.GetCount();
		}
	}
	
	return numfont;
}

//����Ƿ��µ���������
int CNewTextLable::IsNewFont(CGraphTextFont *pfont)
{
	ASSERT(pfont);
	
	int num=0;
	for(POSITION position = m_TextFontList.GetHeadPosition();position!=NULL;)
	{
		CGraphTextFont* pBlock = m_TextFontList.GetAt(position);
		if(pBlock)
		{
			num+=1;
			if(pBlock->Compare(pfont))
				return num;
		}
		m_TextFontList.GetNext(position);
	}
	return -1;
}


/*************************************************************************
 �������ƣ�MakeUpTextHorFromLeft()
 ��    �������Ŵ������Ű�
*************************************************************************/
POSITION CNewTextLable::MakeUpTextHorFromLeft(CDC *pDC,long nId,BOOL bFlag)
{
	ASSERT ( pDC );
	if( !pDC )
		return NULL;

	if( nId < 0 ) 
		nId = 0;

	int nCount = GetGraphTextLineListCount();
	if( nCount <= 0 )
		return NULL;
	
	CPoint ptMakeup = m_ptLeftTop ; //
	m_nPosx = ptMakeup.x ; 
	m_nPosy = ptMakeup.y ; //����ҳ���ƫ����
	m_nListNum = 0;
	m_nLineNum = 0;

	POSITION currentpos=NULL,postemp=NULL;
	
	CGraphTextLine *pTextLine = GetGraphTextLine(nId,&currentpos); // ��ȡ���ڵĶ���

	ASSERT(pTextLine);
	if( !pTextLine )
		return NULL;

	postemp = currentpos;

	CGraphTextCharList *pCharList = GetGraphTextCharList( pTextLine ); //��ȡ��������

	ASSERT( pCharList );
    if(NULL == pCharList)
	{
		return currentpos;
	}
	int nx =0 ,ny =0;

	//�����Ű�Ŀ�ʼλ��
	//��ȡǰһ��Position
	GetGraphTextLineList()->GetPrev(currentpos);
	if( currentpos )
	{
		//��ȡǰһ��
		CGraphTextLine *pTextLinePre = GetGraphTextLineList()->GetAt(currentpos);
		if( pTextLinePre )
		{  
			//��ȡǰһ�ε�����
			CGraphTextCharList *pCharListPre = GetGraphTextCharList( pTextLinePre );
			if( pCharListPre )
			{
				//��ȡǰ���ֵ����һ���ַ�
				CGraphTextChar *pChar = pCharListPre->GetTail();
				if( pChar )
				{
					nx = m_nPosx ;
					ny = pChar->GetCharY() + pChar->GetHeight() ;//+ pTextLinePre->m_nLineSpace;
					m_nLineNum = pChar->GetCharLine()+1;
				}
				else
				{
					ASSERT(FALSE);
					return currentpos;
				}
			}
		}
	} //�����Ű�Ŀ�ʼλ�ý���
	else
	{
		m_nListNum = 0;
		m_nLineNum = 1;
	}
	//////////////////////////////�����Ű��е�λ�ÿ�ʼ
	
	CPoint point( nx , ny );
	
	if( nx == 0 && ny == 0 )
	{
		point.x = m_nPosx;
		point.y = m_nPosy;
	}
	
	MakeUpPageHorFromeLeft(pDC,nId,pTextLine,&point);
	
	currentpos = postemp;
	//��һ��
	GetGraphTextLineList()->GetNext(postemp);

	while(postemp!=NULL)  //���������Ű�
	{
		currentpos = postemp;
		pTextLine = GetGraphTextLineList()->GetNext(postemp);
		ASSERT(pTextLine);
		if( !pTextLine )
			return currentpos;

		pCharList = GetGraphTextCharList(pTextLine);		
		ASSERT( pCharList );
		if (NULL == pCharList)
		{
			return currentpos;
		}
		CGraphTextChar *pchar = pCharList->GetHead( );		
		if(pchar)
		{
			nx = pchar ->GetCharX() ;
			ny = pchar ->GetCharY() ;
		}
		else 
		{
			ASSERT( FALSE);
			return currentpos;
		}

		if( point.x == nx && point.y == ny && bFlag == FALSE) //��һ�����ŵ�λ��û�иı�ʱ�����Ű�
		{
			return currentpos;
		}
		else
		{
			//�Ű���һ��
			MakeUpPageHorFromeLeft(pDC,nId,pTextLine,&point);
		}
	}

	return postemp;
}

//����һ�εĴ���
//lint --e{613} suppress "Possible use of null pointer 'pCharList' in left argument to operator '->'"
//�Ѿ���pCharList���п�ָ���жϣ�pclint��Ȼ����ֱ�Ӷ������ mofiyer: wengcx
BOOL CNewTextLable::MakeUpPageHorFromeLeft(CDC *pDC,long nId,CGraphTextLine *pTextLine,CPoint *nPoint)
{
	if ( NULL == nPoint || NULL == pDC || NULL == pTextLine)
	{
		return FALSE;
	}
	CGraphTextCharList *pCharList = GetGraphTextCharList(pTextLine);
	ASSERT( pCharList );
	if (NULL == pCharList)
	{
		return FALSE;
	}
	
	int nFontNum=-1;
	int cyPixels = 96;

	//if( pDC->IsKindOf( RUNTIME_CLASS( CMetaFileDC )))
	//	cyPixels = 96;
	//else
	cyPixels = pDC->GetDeviceCaps (LOGPIXELSY);

	CString strFontName = _T("����");
	//strFontName.LoadString(IDS_FONT_NAME);
	LOGFONT  LogFontDef = { -16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
				 DEFAULT_CHARSET,
				 OUT_DEFAULT_PRECIS,
				 CLIP_DEFAULT_PRECIS,
				 DEFAULT_QUALITY,
				 DEFAULT_PITCH | FF_DONTCARE,
				 //"����" 
			};  //��ʼ������
	wcscpy(LogFontDef.lfFaceName,strFontName);
	//LogFontDef.lfHeight= MulDiv (12, cyPixels, 72);//12*4/3;
	
	CFont						m_font;
	CFont*                      oldFont=NULL;

	//�Ű浱ǰ�е����߶�
	int nMaxCharHeight=0;
	//��ǰ����ĸ߶�
	int nFontHeight=0;
	//��ǰ�и�
	int nLineHeight = 0;
	//��ǰ�ַ��ĸ߶�
	int nCharHeight = 0;
	//��ǰ�ַ��Ŀ��
	int nCharWidth = 0;

	//�ַ��ڵ�ǰ�豸�Ĵ�С
	CSize SizeWidth;
	//�ַ�����
	CString strbuf=_T("");
	
	//���崦��
	m_font.DeleteObject();
	m_font.CreateFontIndirect (&LogFontDef);
	oldFont = pDC->SelectObject(&m_font); //��������

	/////////////////////////////////////////////////////////////��ʼ�Ű�
	//CHaGraphTextLine
	CGraphTextLine::GrapText_MkMode nMakeUpType = pTextLine->GetMakeUpType() ; //�Ű�����ͣ������Ҷ��룩

	int nLineSpace = pTextLine->GetLineSpace();	 //�о�
	int nListSpace = pTextLine->GetListSpace();	 //�о�
	pTextLine->SetBeginLine( m_nLineNum );
	pTextLine->SetLeftTop(CPoint(m_ptLeftTop.x,nPoint->y ));

	//ÿ����λ��
	POSITION posLineHead = NULL;
	//ÿ��ĩβλ��
	POSITION posLimeTail = NULL;

	BOOL bEnglish = FALSE ;
//	BOOL bLineHead = TRUE ;

	for(POSITION position = pCharList->GetHeadPosition() ; position != NULL ; )
	{
		CGraphTextChar *pchar = pCharList->GetAt(position);
		if(NULL != pchar)
		{
			//�������׵�λ��
			if( !posLineHead )
			{
				posLineHead = position;
				//ȡ��β��λ��
				posLimeTail = position;
			}

			CString strText = pchar->GetStrText();
			if( nFontNum != pchar->GetFontIndex() ) //���������Ҫ�ı���������������
			{
				CGraphTextFont font;
				GetPfsFont( pchar->GetFontIndex() ,&font);
				nFontNum = pchar->GetFontIndex();		
				if(oldFont)
				{
					pDC->SelectObject(oldFont);
				}
				
				nFontHeight = MulDiv (font.GetLogFont()->lfHeight, cyPixels, 72);
				m_font.DeleteObject();
				m_font.CreateFontIndirect ( font.GetLogFont() );
				oldFont=pDC->SelectObject(&m_font);
			}//��������

			SizeWidth = pchar->GetTextExtent( pDC );

			nCharHeight = SizeWidth.cy ;

			nLineHeight  = nMaxCharHeight > nCharHeight ? nMaxCharHeight : nCharHeight;
			nLineHeight += nLineSpace;

			nCharWidth = SizeWidth.cx;

			//��ǰ�ַ����Ƽ�λ��
			CRect rcCurrent(nPoint->x ,nPoint->y,nPoint->x + nCharWidth, nPoint->y + nLineHeight );
           
			//��һ���ַ��Ŀ���λ��
			CRect rcPrev( rcCurrent.right + nListSpace , nPoint->y ,
				            rcCurrent.right + nListSpace + nCharWidth ,
							nPoint->y + nLineHeight );

			int check = CheckPositionHorFromeLeft( rcCurrent,rcPrev , nPoint, strText , 
				nCharWidth , nListSpace , bEnglish );
			
			bEnglish = FALSE;
			if( check )
			{
				if( check == 1 ) //����
				{	
					if( !IsChinese(strText) ) //Ӣ���ַ��������ʴ���
					{
						int oldx = 0,oldy = 0 ,oldwidth = 0,oldheight = -1;
						POSITION tmpPos = NULL;
						POSITION position3 = position;
						pCharList->GetPrev( position3 );
						for(;position3!=NULL;)
						{
							CGraphTextChar *pBlockchar = pCharList->GetAt(position3);
							if( pBlockchar &&  pBlockchar->GetStrText() != _T(" ") )
							{	
								oldx = pBlockchar->GetCharX();
								oldy = pBlockchar->GetCharY();
								oldwidth = pBlockchar->GetWidth();
								if( oldheight == -1 )
									oldheight = oldy ;
								if( !IsChinese( pBlockchar->GetStrText()) )
									tmpPos=position3;
								else
									break ;
							}
							else
								break;

							if( oldheight != -1 && oldheight != oldy )
							{
								break;
							}
							pCharList->GetPrev(position3);
						}
						if( tmpPos )
						{
							if( oldx > ( m_nPosx + oldwidth + nListSpace ) && oldheight == oldy )
							{
								nPoint->x = oldx ;
								nPoint->y = oldy ;
								position = tmpPos ;
								bEnglish = TRUE ;
								continue ;
							}
						}
						//else  //��س�����
						{	
							//ȡ��β��λ��
							//posLimeTail = position;
							//һ�н����Ĵ���
							if( nMakeUpType == CGraphTextLine::GRPAHTXT_MKMODE_RIGHT )  //�Ҷ��봦��
							{
								if( posLineHead && posLimeTail && NULL != pCharList )
									MakeUpPageHorFromeLeftTextLineRight( posLineHead,posLimeTail,pCharList,nListSpace,nLineSpace,nMaxCharHeight );
							}
							else if( nMakeUpType == CGraphTextLine::GRPAHTXT_MKMODE_CENTER )  //���ж��봦��
							{
								if( posLineHead && posLimeTail && pCharList )
									MakeUpPageHorFromeLeftTextLineCenter(posLineHead,posLimeTail,pCharList,nListSpace,nLineSpace,nMaxCharHeight );
							}
							posLimeTail = NULL;
							//���н����������

							//��һ�п�ʼ
							//�������׵�λ��
							posLineHead = position;

							m_nLineNum += 1 ;
							m_nListNum =  0 ;
							nMaxCharHeight = nCharHeight;

							nLineHeight  = nCharHeight;
							nLineHeight += nLineSpace;

							strbuf.Empty();
						}
					}
					else  //��س�����
					{
						//ȡ��β��λ��(�ǵ�ǰ�ַ���ǰһ���ַ�)
						//posLimeTail = position;
						//һ�н����Ĵ���
						if( nMakeUpType == CGraphTextLine::GRPAHTXT_MKMODE_RIGHT )  //�Ҷ��봦��
						{
							if( posLineHead && posLimeTail && pCharList )
								MakeUpPageHorFromeLeftTextLineRight(posLineHead,posLimeTail,pCharList,nListSpace,nLineSpace,nMaxCharHeight );
						}
						else if( nMakeUpType == CGraphTextLine::GRPAHTXT_MKMODE_CENTER )  //���ж��봦��
						{
							if( posLineHead && posLimeTail && pCharList )
								MakeUpPageHorFromeLeftTextLineCenter(posLineHead,posLimeTail,pCharList,nListSpace,nLineSpace,nMaxCharHeight );
						}
					    posLimeTail = NULL;
						//���н����������

						//��һ�п�ʼ
						//�������׵�λ��
						posLineHead = position;

						m_nLineNum += 1;
						m_nListNum = 0;
						nMaxCharHeight = nCharHeight;

						nLineHeight  = nCharHeight;
						nLineHeight += nLineSpace;

						strbuf.Empty();
					}
				} //end if check == 1
			} //end if check
			////////////////////////////����λ��
			//posbak = position;
			if( nCharHeight > nMaxCharHeight)
			{
				if( nMaxCharHeight >0 )  //����ͬ��������ĸı�
				{
					POSITION tmpPos=NULL;
					POSITION position3=position;

					pCharList->GetPrev(position3);
					
					for(;position3!=NULL;)
					{
						CGraphTextChar *pBlockchar=pCharList->GetAt(position3);
						if(pBlockchar->GetCharY()==nPoint->y)
						{
							tmpPos=position3;
						}
						else
							break;
						pCharList->GetPrev(position3);
					}
					CPoint ptSave = *nPoint;
					if(tmpPos!=NULL)
					{
						CGraphTextChar * pBlockchar=pCharList->GetAt(tmpPos);
						nPoint->x=pBlockchar->GetCharX() ;
						nPoint->y=pBlockchar->GetCharY();
					}
					if(tmpPos!=NULL)
					{
						for(;tmpPos!=NULL && tmpPos!=position;)
						{							
							CGraphTextChar * pBlockchar = pCharList->GetAt(tmpPos);
							pBlockchar->SetCharY ( nPoint->y );
							pBlockchar->SetCharShowY ( nPoint->y + nCharHeight - pBlockchar->FontHeight() );
							pBlockchar->SetBaseLineHeight( nPoint->y + nCharHeight  );
							pBlockchar->SetHeight( nLineHeight );
							pCharList->GetNext(tmpPos);
						}
					}
					*nPoint = ptSave;
				}

				nMaxCharHeight = nCharHeight;
			}
		
			m_nListNum++;
			pchar->SetBaseLineHeight( nPoint->y + nMaxCharHeight );

			//if(nMakeUpType == CHaGraphTextLine::GRPAHTXT_MKMODE_NORMALL)  //��������ʽ
			{
				pchar->SetCharX (nPoint->x );
				pchar->SetCharY (nPoint->y );
				pchar->SetCharShowY ( nPoint->y + nMaxCharHeight - nCharHeight ); 
				pchar->SetCharShowX ( nPoint->x );
				pchar->SetCharLine ( m_nLineNum );
				pchar->SetCharList ( m_nListNum );
				pchar->SetFontHeight( nCharHeight );
				pchar->SetHeight( nLineHeight );
				pchar->SetWidth( nCharWidth );
				pchar->SetFontWidth( nCharWidth );
				nPoint->x += nListSpace + nCharWidth;

				//�س�����
				if(strText =='\n' /*|| strText =='\r'*/)
				{
					//ȡ��β��λ��
					posLimeTail = position;
					//һ�н����Ĵ���
					if( nMakeUpType == CGraphTextLine::GRPAHTXT_MKMODE_RIGHT )  //�Ҷ��봦��
					{
						if( posLineHead && posLimeTail && pCharList )
							MakeUpPageHorFromeLeftTextLineRight(posLineHead,posLimeTail,pCharList,nListSpace,nLineSpace,nMaxCharHeight );
					}
					else if( nMakeUpType == CGraphTextLine::GRPAHTXT_MKMODE_CENTER )  //���ж��봦��
					{
						if( posLineHead && posLimeTail && pCharList )
							MakeUpPageHorFromeLeftTextLineCenter(posLineHead,posLimeTail,pCharList,nListSpace,nLineSpace,nMaxCharHeight );
					}
					posLimeTail = NULL;
					//���н����������
					//��һ�п�ʼ
					//��һ��ͷ����
					posLineHead = NULL;

					m_nLineNum+=1;
					m_nListNum=0;
					nPoint->y+= nLineHeight ;//+ nLineSpace;
					nMaxCharHeight = nCharHeight;

					nLineHeight  = nCharHeight;
					nLineHeight += nLineSpace;

					nPoint->x = m_nPosx;
				}
			}  //�����Ű����
			

			//ȡ��β��λ��
			posLimeTail = position;
			//��ȡ��һ���ַ�
			pCharList->GetNext( position );
		}		
	}

	//���ö��������һЩ��Ϣ
	int nPageWidth = m_rcPage.Width() - m_ptLeftTop.x - m_ptRightBottom.x;
	pTextLine->SetRightBottom(CPoint(nPageWidth,nPoint->y ));
	pTextLine->SetEndLine( m_nLineNum );
	/////////////////////////////////////////////////////////////�Ű����
	if(oldFont)
	{
		pDC->SelectObject(oldFont);
		m_font.DeleteObject();
	}
	return TRUE;
}

//��ȡ������ַ�����
CGraphTextCharList * CNewTextLable::GetGraphTextCharList(CGraphTextLine *pTextLine)
{
	ASSERT(pTextLine);
	if (NULL != pTextLine)
	{
		return pTextLine->GetCharList();
	}
	return NULL;
	
}

//�������Ŀ
int CNewTextLable::GetGraphTextLineListCount()
{
	return m_GraphTextLineList.GetCount();
}


BOOL CNewTextLable::CheckPositionHorFromeRight(CPoint *point,CRect rect1, int width,int linespace)
{
	//	����飬bFlag��ԶΪTRUE�����벻��whileѭ����ͬʱҲ����Ϊpoint��ֵ
	//	�˺���ʲô���鶼û����ֱ�ӷ���true
	//	mofiyer: wengcx

	return TRUE;

	//BOOL  bFlag =TRUE;
	//CRect rcImage;
	//CRect rcDst;

	//int nPageWidth = m_rcPage.Width() - m_ptLeftTop.x - m_ptRightBottom.x;
	//CPoint ptMakeUpLt =  m_ptLeftTop;

	////�ж��Ƿ������ͼƬ���ͼƬ��λ���г�ͻ
	//bFlag = TRUE;//IsInSpeciRectHorFromeLeft( rect1, rcImage );
	//if( bFlag )
	//	return TRUE;
	//else
	//{
	//	CRect rect2 = rect1;
	//	//����λ�øı��Ĵ���
	//	while( !bFlag )
	//	{
	//		int rcwidth = rect1.right - rcImage.left;
	//		int nleft = rect1.left - rcwidth - width - linespace;
	//		rect2.SetRect( nleft ,rect1.top, nleft + width ,rect1.bottom);
	//		if( rect2.right > (nPageWidth + ptMakeUpLt.x ))
	//		{
	//			ASSERT( FALSE );
	//		}

	//		bFlag = TRUE;//IsInSpeciRectHorFromeLeft( rect2 , rcImage);
	//		rect1 = rect2;
	//	}

	//	*point = rect2.TopLeft();

	//	return TRUE;
	//}
}


//�����Ҷ���ķ�ʽ
BOOL CNewTextLable::MakeUpPageHorFromeLeftTextLineRight(
		POSITION posHead,POSITION posTail,CGraphTextCharList *pTextLineCharList,\
		const int &nListSpace,const int &nLineSpace,const int &nMaxFontHeight )
{
	ASSERT( posHead );
	ASSERT( posTail );
	ASSERT( pTextLineCharList );

	if( !posHead || !posTail || !pTextLineCharList )
		return FALSE;

	CPoint ptPosXY = m_ptLeftTop;
	int nPageWidth = m_rcPage.Width() -  m_ptLeftTop.x - m_ptRightBottom.x;
	//int nPageHeight = 0;

	int nTempX = ptPosXY.x ; //��ʱXλ��
	int nTempWidth = 0; //��ʱ�й���
	//��һ���Ŷӽ�һ�н�������
	POSITION pos;
	for( pos = posHead ; pos != NULL ; )
	{
		CGraphTextChar * pBlockchar = pTextLineCharList->GetAt( pos);
		if( pBlockchar )
		{
			pBlockchar->SetCharX( nTempX );
			pBlockchar->SetCharShowX(nTempX );
			nTempX += pBlockchar->GetWidth() + nListSpace ;
			nTempWidth += pBlockchar->GetWidth() + nListSpace ;
		}
		if( pos == posTail )
			break;
		pTextLineCharList->GetNext( pos );
	} 
	//���������Ҷ���

	nTempX = ptPosXY.x + nPageWidth;
	pos = NULL;
	for( pos = posTail ; pos != NULL ; )
	{
		CGraphTextChar * pBlockchar = pTextLineCharList->GetAt( pos);
		if( pBlockchar )
		{
			nTempX -= (pBlockchar->GetWidth() + nListSpace);
			pBlockchar->SetCharX( nTempX );
			pBlockchar->SetCharShowX(nTempX );
		}
		if( pos == posHead )
			break;
		pTextLineCharList->GetPrev( pos );
	} 

	CRect rcShow(0,0,0,0);
	//�޳�ͼƬ������
	nTempX = ptPosXY.x + nPageWidth;
	CPoint ptShow(nTempX,0);
	for( pos = posTail ; pos != NULL ; )
	{
		CGraphTextChar * pBlockchar = pTextLineCharList->GetAt( pos);
		if( pBlockchar )
		{
			ptShow.x -= (pBlockchar->GetWidth() + nListSpace );
			rcShow.SetRect( ptShow.x, pBlockchar->GetCharY(),
				ptShow.x + pBlockchar->GetWidth(),pBlockchar->GetCharY() + pBlockchar->GetHeight() );
			
			CheckPositionHorFromeRight( &ptShow, rcShow ,pBlockchar->GetWidth(),nListSpace);
	
			pBlockchar->SetCharX( ptShow.x );
			pBlockchar->SetCharShowX( ptShow.x );
		}
		if( pos == posHead )
			break;
		pTextLineCharList->GetPrev( pos );
	} 

	return TRUE;
}

//������ж���ķ�ʽ
BOOL CNewTextLable::MakeUpPageHorFromeLeftTextLineCenter( POSITION posHead , POSITION posTail,
		CGraphTextCharList *pTextLineCharList,const int &nListSpace,
		const int &nLineSpace,const int &nMaxFontHeight )
{
	ASSERT( posHead );
	ASSERT( posTail );
	ASSERT( pTextLineCharList );

	if( NULL == posHead || NULL == posTail || NULL ==pTextLineCharList )
		return FALSE;

	CPoint ptPosXY = m_ptLeftTop;
	int nPageWidth = m_rcPage.Width() -  m_ptLeftTop.x - m_ptRightBottom.x;
	//int nPageHeight = 0;

	int nTempX = ptPosXY.x ; //��ʱXλ��
	int nTempWidth = 0; //��ʱ�й���
	//��һ���Ŷӽ�һ�н�������
	POSITION pos = NULL;
	for( pos = posHead ; pos != NULL ; )
	{
		CGraphTextChar * pBlockchar = pTextLineCharList->GetAt( pos);
		if( NULL != pBlockchar )
		{
			pBlockchar->SetCharX( nTempX );
			pBlockchar->SetCharShowX(nTempX );
			nTempX += pBlockchar->GetWidth() + nListSpace ;
			nTempWidth += pBlockchar->GetWidth() + nListSpace ;
			if( pos == posTail )
			{
				nTempWidth += pBlockchar->GetWidth() + nListSpace ;
				break;
			}
		}
		
		pTextLineCharList->GetNext( pos );
	} 
	//�����Ǿ��ж���

	CRect rcShow(0,0,0,0); //һ�е�����
	nTempX = ptPosXY.x + (nPageWidth - nTempWidth )/2;
	for( pos = posHead ; pos != NULL ; )
	{
		CGraphTextChar * pBlockchar = pTextLineCharList->GetAt( pos);
		if( pBlockchar )
		{
			if( pos == posHead )
			{
				rcShow.left = nTempX;
				rcShow.top = pBlockchar->GetCharY();
			}

			pBlockchar->SetCharX( nTempX );
			pBlockchar->SetCharShowX(nTempX );
			nTempX += (pBlockchar->GetWidth() + nListSpace);

			if( pos == posTail )
			{	
				rcShow.right  = nTempX;
				rcShow.bottom = rcShow.top + pBlockchar->GetHeight() ;
			}
		}
		if( pos == posTail )
			break;
		pTextLineCharList->GetNext( pos );
	} 

/*	rcShow.left = ptPosXY.x;
	rcShow.right = ptPosXY.x + nPageWidth ;
	
	CArray<CRect,CRect> rcArrayRet;
	rcArrayRet.RemoveAll();
	BOOL bRet = FALSE;//IsInSpeciRectHorFromeCenter( rcShow, rcArrayRet );

	//һ�п��
	int nWidthInSec = 0;
	int nCount = rcArrayRet.GetSize();
	if( bRet && nCount )
	{
		for( int i = 0; i < nCount ; i++ )
		{
			CRect rc = rcArrayRet.GetAt( i );
			nWidthInSec += rc.Width();
		}
	}

	//�ٴξ��ش���
	nWidthInSec += nTempWidth;
	nTempX = ptPosXY.x + (nPageWidth - nWidthInSec )/2;
	for( pos = posHead ; pos != NULL ; )
	{
		CGraphTextChar * pBlockchar = pTextLineCharList->GetAt( pos);
		if( pBlockchar )
		{
			pBlockchar->SetCharX( nTempX );
			pBlockchar->SetCharShowX(nTempX );
			nTempX += (pBlockchar->GetWidth() + nListSpace);
		}
		if( pos == posTail )
			break;
		pTextLineCharList->GetNext( pos );
	} 

	nTempX = ptPosXY.x + (nPageWidth - nWidthInSec )/2;
	CPoint ptShow( nTempX , 0); 
	//�޳��������
	for( pos = posHead ; pos != NULL ; )
	{
		CGraphTextChar * pBlockchar = pTextLineCharList->GetAt( pos);
		if( pBlockchar )
		{
			
			rcShow.SetRect( ptShow.x,pBlockchar->GetCharY(),
				ptShow.x + pBlockchar->GetWidth(),pBlockchar->GetCharY()+pBlockchar->GetHeight() );

			CheckPositionHorFromeCenter( &ptShow,rcShow ,pBlockchar->GetWidth(),nListSpace);

			pBlockchar->SetCharX( ptShow.x  );
			pBlockchar->SetCharShowX(ptShow.x );

			ptShow.x += (pBlockchar->GetWidth() + nListSpace );
		}
		if( pos == posTail )
			break;
		pTextLineCharList->GetNext( pos );
	} 
*/
	
	return TRUE;
}


//���ж��봦��
BOOL CNewTextLable::CheckPositionHorFromeCenter(CPoint *point,CRect rect1, int width,int linespace)
{	
	//	����飬bFlag��ԶΪTRUE�����벻��whileѭ����ͬʱҲ����Ϊpoint��ֵ
	//	�˺���ʲô���鶼û����ֱ�ӷ���true
	//	mofiyer: wengcx

	return TRUE;

	//BOOL bFlag =TRUE;
	//CRect rcImage;
	//CRect rcDst;

	//int nPageWidth = m_rcPage.Width() -  m_ptLeftTop.x - m_ptRightBottom.x;
	//CPoint ptMakeUpLt =  m_ptLeftTop;	
	
	
	//�ж��Ƿ������ͼƬ���ͼƬ��λ���г�ͻ
	//bFlag = TRUE;//IsInSpeciRectHorFromeLeft( rect1, rcImage );
	//if( bFlag )
	//	return TRUE;
	//else
	//{
	//	CRect rect2 = rect1;
	//	//����λ�øı��Ĵ���
	//	while( !bFlag )
	//	{
	//		int rcwidth = rcImage.right - rect1.left ;
	//		int nleft = rect1.left + rcwidth ;
	//		rect2.SetRect( nleft ,rect1.top, nleft + width + linespace , rect1.bottom);
	//		if( rect2.right > (nPageWidth + ptMakeUpLt.x ))
	//		{
	//			//ASSERT( FALSE );
	//		}

	//		bFlag = TRUE;//IsInSpeciRectHorFromeLeft( rect2 , rcImage);
	//		rect1 = rect2;
	//	}

	//	*point = rect2.TopLeft();

	//	return TRUE;
	//}
}



int CNewTextLable::CheckPositionHorFromeLeft(CRect rect1, CRect rect2, CPoint *point,
		CString str,int width,int linespace,BOOL bEnglish)
{
	BOOL bRet = FALSE, bFlag =TRUE;
	CRect rcImage;
	CRect rcDst;

	int nPageWidth = m_rcPage.Width() -  m_ptLeftTop.x - m_ptRightBottom.x;
	CPoint ptMakeUpLt =  m_ptLeftTop;

	BOOL bHouzhi = IsTureFuhao(str);
	BOOL bQianzhi= CheckFrontToken(str);

	//�ж��Ƿ������ͼƬ���ͼƬ��λ���г�ͻ
	bFlag = TRUE;//IsInSpeciRectHorFromeLeft( rect1, rcImage );

	if( rect1.right > (nPageWidth + ptMakeUpLt.x) )
	{
		//�ж��Ƿ��Ǻ����ַ�
		if( !bHouzhi )
		{
			rcImage = rect1;
			bFlag = FALSE;
			bRet = TRUE;
		}
	}
	
	if(( rect1.right + width) > ( nPageWidth + ptMakeUpLt.x ) )
	{
		//�ж��Ƿ���ǰ���ַ�
		if( bQianzhi )
		{
			rect2.right += width;
			rcImage = rect2;
			bFlag = FALSE;
			bRet = FALSE;
		}
	}
	
	if( bEnglish )
	{
		int nw = rect2.Width();
		rect2.right = nPageWidth + ptMakeUpLt.x + 1;
		rect2.left = nPageWidth + ptMakeUpLt.x + 1 - nw ;
		rcImage = rect2;
		bFlag = FALSE;
		bRet = FALSE;
	}

	if( bFlag )
		return 0;
	else
	{
		//����λ�øı��Ĵ���
		while( !bFlag )
		{
			int rcwidth = rect2.Width();
			int rchight = rect2.Height();
			rect2.SetRect(rcImage.right+1,rect2.top,rcImage.right+1+  rcwidth ,rect2.bottom);
			if( rect2.right > (nPageWidth + ptMakeUpLt.x) )
			{
				if(str=='\r'||str=='\n')
				{
					bRet = false;
					rect2.left -= rect2.Width();
					rect2.right -= rect2.Width();
				}
				else
				{
					rcwidth = rect2.Width();
					rchight = rect2.Height();
					rect2.SetRect(ptMakeUpLt.x,rect2.bottom,
						ptMakeUpLt.x + rcwidth,rect2.bottom + rchight);
					bRet = TRUE;
				}
			}

			bFlag = TRUE;//IsInSpeciRectHorFromeLeft( rect2 , rcImage);
		}

		point->x = rect2.left;
		point->y = rect2.top;

		if( bRet == TRUE )
			return 1;
		else
			return 2;
	}
}

//
//**********************************************************************//
//   �������ƣ�IsTureFuhao()
//----------------------------------------------------------------------//
//   �������Ƿ�Ϊ���ñ��
//**********************************************************************//
BOOL CNewTextLable::IsTureFuhao(const CString &str)
{
	if(str.IsEmpty()) return FALSE;
	if(	str.CompareNoCase(_T("��")) == 0||
		str.CompareNoCase(_T("��")) == 0||
		str.CompareNoCase(L"��") == 0||
		str.CompareNoCase(L"��") == 0||      
		str.CompareNoCase(L"��") == 0||
		str.CompareNoCase(L"��") == 0||
		str.CompareNoCase(L"��") == 0||
		str.CompareNoCase(L"\"") == 0||
		str.CompareNoCase(L"��") == 0||
		str.CompareNoCase(L"��") == 0||
		str.CompareNoCase(L"��") == 0||
		str.CompareNoCase(L"��") == 0||
		str.CompareNoCase(L"'") == 0||
		str.CompareNoCase(L"��") == 0||
		str.CompareNoCase(L"��") == 0||   	  	
		str.CompareNoCase(L",") == 0||
		str.CompareNoCase(L".") == 0||
		str.CompareNoCase(L";") == 0||
		str.CompareNoCase(L"?") == 0||
		str.CompareNoCase(L"]") == 0||
		str.CompareNoCase(L"}") == 0||
		str.CompareNoCase(L">") == 0 ) //ȫ�ǰ��
	{
		return TRUE;          //���ñ����ţ���������"������
	}                     
	else 
		return FALSE;
}

//
//**********************************************************************//
//   �������ƣ�CheckFrontToken()
//----------------------------------------------------------------------//
//   ����������Ƿ�Ϊǰ�ñ��
//**********************************************************************//
BOOL CNewTextLable::CheckFrontToken(const CString &str)
{                
	if(str.CompareNoCase(L"��") == 0||
		str.CompareNoCase(L"\"") == 0||
		str.CompareNoCase(L"'") == 0||	
		str.CompareNoCase(L"��") == 0||     
		str.CompareNoCase(L"��") == 0||	
		str.CompareNoCase(L"��") == 0||	
		str.CompareNoCase(L"��") == 0||	
		str.CompareNoCase(L"(") == 0||
		str.CompareNoCase(L"<") == 0||		
		str.CompareNoCase(L"[") == 0||	
		str.CompareNoCase(L"{") == 0) //ȫ�ǰ��
	{
		return TRUE;          //ǰ�ñ�����
	}                     
	else 
		return FALSE;
}  


//����λ���Ű�
POSITION CNewTextLable::MakeUpTextPositon(CDC *pDC,POSITION pos)
{
	ASSERT ( pDC);
	ASSERT ( pos );

	if( !pDC  || !pos )
		return NULL;

	int nCount = GetGraphTextLineListCount();
	if( nCount <= 0 )
		return NULL;

	POSITION currentpos = pos;
	    
	CGraphTextLine *pTextLine = GetGraphTextLineList()->GetAt(pos); // ��ȡ���ڵĶ���

	ASSERT(pTextLine);
	if( !pTextLine )
		return NULL;

	CGraphTextCharList *pCharList =  GetGraphTextCharList( pTextLine ); //��ȡ��������
	ASSERT( pCharList );
    
	if( !pCharList )
		return NULL;

	CGraphTextChar *pChar = pCharList->GetHead();
	if( !pChar )
		return NULL;

	long nCaret = pChar->GetCaretId();

	BOOL bFlag = FALSE;
	//���Ŵ�����
	currentpos = MakeUpTextHorFromLeft( pDC, nCaret, bFlag );

	return currentpos;
}

//�����Ű�
void CNewTextLable::MakeUpText(CDC *pDC,long nId,BOOL bFlag )
{
	int nCount = GetGraphTextLineListCount();
	if( nCount <= 0 )
		return ;

	MakeUpTextHorFromLeft( pDC, nId, bFlag );
}


///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


IMPLEMENT_SERIAL(CGraphTextChar, CObject, VERSIONABLE_SCHEMA | 1 )

CGraphTextChar::CGraphTextChar()
{
	m_nCaret = 0;
	m_nX = 0;
	m_nY = 0;
	m_nWidth = 0;
	m_nHeight = 0;

	m_nShowY = 0; 
	m_nShowX = 0;
	m_nLine = 0;
	m_nList = 0;
	m_nFontNum = 0;
	m_nFontHeight = 0;
	m_nFontWidth = 0;
	m_nBaseLineHeight = 0;

	m_strText = _T("");
}

CGraphTextChar::~CGraphTextChar()
{

}

int CGraphTextChar::GetWidth()
{
	return m_nWidth;
}

void CGraphTextChar::SetWidth( int width )
{
	m_nWidth = width ;
}

int CGraphTextChar::GetHeight()
{
	return m_nHeight;
}

void CGraphTextChar::SetHeight(int height )
{
	m_nHeight = height ;
}

int CGraphTextChar::FontHeight()
{
	return m_nFontHeight;
}

void CGraphTextChar::SetFontHeight(int height)
{
	m_nFontHeight = height ;
}

int CGraphTextChar::GetFontIndex()
{
	return m_nFontNum;
}

void CGraphTextChar::SetFontIndex(int nIndex)
{
	m_nFontNum = nIndex ;
}

int CGraphTextChar::GetFontWidth()
{
	return m_nFontWidth ;
}

void CGraphTextChar::SetFontWidth(int width)
{
	m_nFontWidth = width ;
}

long  CGraphTextChar::GetCaretId()
{
	return m_nCaret;
}

void  CGraphTextChar::SetCaretId( long lCaretId )
{
	m_nCaret = lCaretId ;
}

int CGraphTextChar::GetCharX()
{
	return m_nX ;
}

void CGraphTextChar::SetCharX( int x )
{
	m_nX = x ;
}

int CGraphTextChar::GetCharY()
{
	return m_nY ;
}

void CGraphTextChar::SetCharY(int y )
{
	m_nY = y;
}


int CGraphTextChar::GetCharShowY()
{
	return  m_nShowY ;
}

void CGraphTextChar::SetCharShowY( int showy )
{
	m_nShowY = showy ;
}

int CGraphTextChar::GetCharShowX()
{
	return m_nShowX ;
}

void CGraphTextChar::SetCharShowX( int showx )
{
	m_nShowX = showx ;
}

int   CGraphTextChar::GetCharLine()
{
	return m_nLine ;
}

void  CGraphTextChar::SetCharLine( int line )
{
	m_nLine = line ;
}

int	  CGraphTextChar::GetCharList()
{
	return m_nList;
}

void  CGraphTextChar::SetCharList(int list)
{
	m_nList = list ;
}

CString CGraphTextChar::GetStrText()
{
	return m_strText ;
}

void    CGraphTextChar::SetStrText(CString str)
{
	m_strText = str ;
}


BOOL CGraphTextChar::IsMouseInRect(CPoint pt,CPoint ptMove,float fScaleX,float fScaleY,int nMode )
{
	CRect rect = GetNormalRect(ptMove,fScaleX,fScaleY,nMode );

	rect.NormalizeRect();
	if( rect.PtInRect ( pt ))
		return TRUE;
	return FALSE;
}

BOOL CGraphTextChar::IsShow(CRect rect,CPoint ptMove,float fScaleX,float fScaleY,int nMode )
{
	int x1=(int )(m_nX * fScaleX);
	int y1=(int)(m_nY *fScaleY);
	int x2=(int)(m_nWidth * fScaleX) ;
	int y2 = (int)(m_nHeight*fScaleY);

	CRect rectshow;
	if( nMode == 3 )
		rectshow.SetRect(x1,y1,x1-x2,y1 + y2);
	else
		rectshow.SetRect(x1,y1,x1+x2,y1+y2);

	rectshow+= ptMove;

	rectshow.NormalizeRect();
	CRect inrc;
	if(inrc.IntersectRect(rect,rectshow))
		return TRUE;

	return FALSE;
}

CRect CGraphTextChar::GetNormalRect(CPoint ptMove,float fScaleX,float fScaleY,int nMode )
{
	int x1=(int )(m_nX * fScaleX);
	int y1=(int)(m_nY *fScaleY);
	int x2=(int)(m_nWidth * fScaleX) ;
	int y2 = (int)(m_nHeight*fScaleY);
	
	CRect rectshow;
	if( nMode == 3 )
		rectshow.SetRect(x1,y1,x1-x2,y1 + y2);
	else
		rectshow.SetRect(x1,y1,x1+x2,y1+y2);

	rectshow+= ptMove;

	return rectshow;
}

//��ȡ�������ŵľ���
CRect CGraphTextChar::GetScaleRect(float fScaleX ,float fScaleY,int nMode )
{
	int x1=(int )(m_nX * fScaleX);
	int y1=(int)(m_nY *fScaleY);
	int x2=(int)(m_nWidth * fScaleX) ;//+ abs( x3 - x1 );
	int y2 = (int)(m_nHeight*fScaleY);//+ abs( y3 - y1 );

	CRect rectshow;
	if( nMode == 3 )
		rectshow.SetRect(x1,y1,x1-x2,y1 + y2);
	else
		rectshow.SetRect(x1,y1,x1+x2,y1+y2);

	return rectshow;
}

//��ȡ��ʵ�ľ���
CRect CGraphTextChar::GetTureRect(int nMode )
{
	int x1= m_nX ;
	int y1= m_nY ;
	int x2= m_nWidth ;
	int y2 = m_nHeight ;

	CRect rect;
	if( nMode == 3)
		rect.SetRect(x1,y1, x1 - x2,y1 + y2);
	else
		rect.SetRect(x1,y1,x1 + x2,y1 + y2);

	return rect;
}

void CGraphTextChar::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_nCaret;
		ar << m_nX;
		ar << m_nY;
		ar << m_nWidth;
		ar << m_nHeight;
		ar << m_nShowY;
		ar << m_nShowX;
		ar << m_nLine;
		ar << m_nList;
		ar << m_nFontNum;
		ar << m_nFontHeight;
		ar << m_nFontWidth;
		ar << m_nBaseLineHeight;
		ar << m_strText;
	}
	else
	{
		ar >> m_nCaret;
		ar >> m_nX;
		ar >> m_nY;
		ar >> m_nWidth;
		ar >> m_nHeight;
		ar >> m_nShowY;
		ar >> m_nShowX;
		ar >> m_nLine;
		ar >> m_nList;
		ar >> m_nFontNum;
		ar >> m_nFontHeight;
		ar >> m_nFontWidth;
		ar >>m_nBaseLineHeight;
		ar >> m_strText;
	}
}

CGraphTextChar &CGraphTextChar::operator = (CGraphTextChar& TextChar)
{
	if ( this == &TextChar )
	{
		return TextChar;
	}

	CopyFrom (&TextChar);
	
	return *this;
}


int CGraphTextChar::GetBaseLineHeight()
{
	return m_nBaseLineHeight;
}

void CGraphTextChar::SetBaseLineHeight(int nMaxHeight)
{
	m_nBaseLineHeight = nMaxHeight ;
}


BOOL CGraphTextChar::Compare(CGraphTextChar *pChar)
{
	ASSERT(pChar);
	ASSERT(this);

	if(NULL != pChar)
	{
		if( m_nCaret != pChar->m_nCaret )
			return FALSE;
		if( m_nX != pChar->m_nX )
			return FALSE;
		if( m_nWidth != pChar->m_nWidth)
			return FALSE;
		if( m_nHeight != pChar->m_nHeight)
			return FALSE;

		if( m_nShowY != pChar->m_nShowY )
			return FALSE;
		if( m_nShowX != pChar->m_nShowX)
			return FALSE;
		if( m_nLine!= pChar->m_nLine )
			return FALSE;
		if( m_nList != pChar->m_nList )
			return FALSE;
		if( m_nFontNum != pChar->m_nFontNum)
			return FALSE;
		if( m_nFontHeight != pChar->m_nFontHeight)
			return FALSE;
		if( m_nFontWidth != pChar->m_nFontWidth )
			return FALSE;
		if(m_strText != pChar->m_strText)
			return FALSE;
		if( m_nBaseLineHeight != pChar->m_nBaseLineHeight )
			return FALSE;
	}
	

	return TRUE;
}

void CGraphTextChar::CopyFrom(CGraphTextChar *pChar)
{
	ASSERT(pChar);
	ASSERT(this);
	if (NULL != pChar)
	{
		m_nCaret = pChar->m_nCaret;

		m_nX = pChar->m_nX;

		m_nY = pChar->m_nY ;

		m_nWidth = pChar->m_nWidth;

		m_nHeight = pChar->m_nHeight;

		m_nShowY = pChar->m_nShowY;

		m_nShowX = pChar->m_nShowX;

		m_nLine = pChar->m_nLine;

		m_nList = pChar->m_nList;

		m_nFontNum = pChar->m_nFontNum;

		m_nFontHeight = pChar->m_nFontHeight;

		m_nFontWidth = pChar->m_nFontWidth;

		m_strText = pChar->m_strText;

		m_nBaseLineHeight = pChar->m_nBaseLineHeight;
	}
	

}

void CGraphTextChar::CopyTo(CGraphTextChar *pChar)
{
	ASSERT(pChar);
	ASSERT(this);
	
	if(NULL != pChar)
	{
		pChar->m_nCaret = m_nCaret ;

		pChar->m_nX = m_nX;

		pChar->m_nY = m_nY ;

		pChar->m_nWidth = m_nWidth;

		pChar->m_nHeight = m_nHeight;

		pChar->m_nShowY = m_nShowY ;

		pChar->m_nShowX = m_nShowX ;

		pChar->m_nLine = m_nLine;

		pChar->m_nList = m_nList;

		pChar->m_nFontNum = m_nFontNum;

		pChar->m_nFontHeight = m_nFontHeight;

		pChar->m_nFontWidth = m_nFontWidth;

		pChar->m_strText = m_strText;

		pChar->m_nBaseLineHeight = m_nBaseLineHeight;
	}
	
}

void CGraphTextChar::DrawScale(CDC *pDC,int nMoveX,int nMoveY, float m_fScaleX,float m_fScaleY, int nMode)
{
	ASSERT(this);

	int nsaveDc = pDC->SaveDC();

/*	int nRop = pDC->SetROP2(R2_NOT);

	int nX =(int)(GetCharX() * m_fScaleX);
	int nY =(int)(GetCharY() * m_fScaleY);
	int nYshow = (int)(GetCharShowY() * m_fScaleY);
	int nXShow = (int)(GetCharShowX() * m_fScaleX);
	CString strText = GetStrText();
	if( strText == "\r" || strText == "\n") // ��ʾ�س���
	{
		COLORREF color = pDC->GetTextColor();
		CPen m_PenCurNew;
		m_PenCurNew.DeleteObject();
		m_PenCurNew.CreatePen(0,1,RGB(192,192,192));
		CPen* pOldPen=pDC->SelectObject(&m_PenCurNew);
		int height = (int)( GetHeight() *m_fScaleY );
		int width = (int)( GetWidth() *m_fScaleX);
		CRect rect;
		rect.SetRect(nMoveX + nX, nMoveY + nYshow + height - 8 ,
				nMoveX + nX + (int)(8*m_fScaleX) ,nMoveY + nYshow + height );

		CBrush brush(RGB(192,192,192));
		pDC->FillRect(rect,&brush);

		if(pOldPen)
			pDC->SelectObject(pOldPen);

		m_PenCurNew.DeleteObject();
		pDC->SetTextColor(color);
	}
	else
	{
		pDC->TextOut(x,y,strText);
	}
	pDC->SetROP2(nRop);
	*/
	
	CString strText = GetStrText();
	if( /*strText == "\r" || */strText == "\n") // ��ʾ�س���
	{
		/*int nX =(int)(GetCharX() * m_fScaleX);
		int nY =(int)(GetCharY() * m_fScaleY);
		int nYshow = (int)(GetCharShowY() * m_fScaleY);
		int nXShow = (int)(GetCharShowX() * m_fScaleX);

		COLORREF color = pDC->GetTextColor();
		CPen m_PenCurNew;
		m_PenCurNew.DeleteObject();
		m_PenCurNew.CreatePen(0,1,RGB(192,192,192));
		CPen* pOldPen=pDC->SelectObject(&m_PenCurNew);
		int height = (int)( GetHeight() *m_fScaleY );
		int width = (int)( GetWidth() *m_fScaleX);
		CRect rect;
		rect.SetRect(nMoveX + nX, nMoveY + nYshow ,
			nMoveX + nX +width ,nMoveY + nYshow + height );
		
		CBrush brush(RGB(192,192,192));
		pDC->FillRect(rect,&brush);
		
		if(pOldPen)
			pDC->SelectObject(pOldPen);
		
		m_PenCurNew.DeleteObject();
		pDC->SetTextColor(color);*/
	}
	else
	{		
		int nYshow = (int)(GetCharShowY() * m_fScaleY);
		int nXShow = (int)(GetCharShowX() * m_fScaleX);
		int x = nXShow + nMoveX;//nX + nMoveX;
		int y = nYshow + nMoveY;//nY + nMoveY;
		pDC->TextOut(x,y,strText);
	}
	pDC->RestoreDC(nsaveDc);
}

//��ȡ������Ű���
CSize CGraphTextChar::GetTextExtent(CDC *pDC )
{
	CSize SizeWidth(0,0);
	if( m_strText =='\n' || m_strText =='\r' )
	{
		SizeWidth = pDC->GetTextExtent( _T("a") );
	}
	else
	{
		SizeWidth = pDC->GetTextExtent(m_strText);
	}

	return SizeWidth;
}



//////////////////////////////////////////////////////////////////////////
////////////////CHaGraphTextLine
IMPLEMENT_SERIAL(CGraphTextLine, CObject, VERSIONABLE_SCHEMA | 1 )

CGraphTextLine::CGraphTextLine()
{
	m_nLineSpace = 2;
	m_nListSpace = 0;
	m_nBeginID = 0;
	m_nEndID = 0;

	m_nTop = 0 ;
	m_nLeft = 0;
	m_nBottom = 0;
	m_nRight = 0;

	m_nBeginLine = 0;
	m_nEndLine = 0;

	m_IsTitle = FALSE;
	m_strCaption = _T("");
	m_CharTextList.RemoveAll();

	m_nMakeUpType = GRPAHTXT_MKMODE_LEFT;
}

void CGraphTextLine::Clear()
{

}

void CGraphTextLine::Init()
{

}

CGraphTextLine::GrapText_MkMode CGraphTextLine::GetMakeUpType()
{
	return m_nMakeUpType;
}

void CGraphTextLine::SetMakeUpType( GrapText_MkMode nType )
{
	m_nMakeUpType = nType ;
}

int CGraphTextLine::GetLineSpace()
{
	return m_nLineSpace;
}

void CGraphTextLine::SetLineSpace(int nLineSpace)
{
	m_nLineSpace = nLineSpace ;
}

int CGraphTextLine::GetListSpace()
{
	return m_nListSpace;
}

void CGraphTextLine::SetListSpace(int nListSpace )
{
	m_nListSpace = nListSpace ;
}

int  CGraphTextLine::GetBeiginId()
{
	return m_nBeginID;
}

void CGraphTextLine::SetBeiginId( int nBeigid )
{
	m_nBeginID = nBeigid ;
}

int  CGraphTextLine::GetEndId()
{
	return m_nEndID;
}

void CGraphTextLine::SetEndId(int nEndid)
{
	m_nEndID = nEndid ;
}

BOOL CGraphTextLine::GetbTitle()
{
	return  m_IsTitle ;
}

void CGraphTextLine::SetbTitle(BOOL bTitle )
{
	m_IsTitle = bTitle ;
}

CString CGraphTextLine::GetCaption()
{
	return m_strCaption;
}

void CGraphTextLine::SetCaption(CString strCaption )
{
	m_strCaption = strCaption ;
}
//lint --e{1536} suppress "Exposing low access member 'CGraphTextLine::m_CharTextList' : ���س�Ա������Ҫ���ͷ��ʵȼ�������const���Σ�"
CGraphTextCharList *CGraphTextLine::GetCharList()
{
	return &m_CharTextList;
}

BOOL CGraphTextLine::IsShow(CRect rect,CPoint ptMove ,float fScaleX,float fScaleY,int nMode )
{
	CRect rcTextLine = GetTextLineRect();
	rcTextLine.NormalizeRect();
	if( rcTextLine.IsRectEmpty() )
	{
		CPoint pt1(0,0),pt2(0,0);
		CRect rect1(0,0,0,0),rect2(0,0,0,0);
		POSITION pos = m_CharTextList.GetHeadPosition();
		if( pos != NULL )
		{
			CGraphTextChar *pchar = m_CharTextList.GetAt( pos);
			if( pchar )
			{
				rect1 = pchar->GetNormalRect(ptMove,fScaleX,fScaleY);
			}
		}
		pos = m_CharTextList.GetTailPosition();
		if( pos != NULL )
		{
			CGraphTextChar *pchar = m_CharTextList.GetAt( pos);
			if( pchar )
			{
				rect2 = pchar->GetNormalRect(ptMove,fScaleX,fScaleY);
			}
		}

		rect1.NormalizeRect();
		rect2.NormalizeRect();
		CRect rect3;
		rect3.SetRect(rect1.TopLeft(),rect2.BottomRight());

		if( rect3.left == rect3.right )
			rect3.right +=1 ;
		if( rect3.top == rect3.bottom )
			rect3.bottom +=1;

		rect3.NormalizeRect();
		CRect inrc;
		if( inrc.IntersectRect(rect3,rect))
			return TRUE;
	}
	else
	{
		rcTextLine.NormalizeRect();
		rcTextLine.left = (int )(rcTextLine.left * fScaleX);
		rcTextLine.top = (int)( rcTextLine.top *fScaleY);
		rcTextLine.right = (int)(rcTextLine.right * fScaleX) ;
		rcTextLine.bottom = (int)( rcTextLine.bottom * fScaleY);
		rcTextLine += ptMove;
		CRect inrc;
		if( inrc.IntersectRect(rcTextLine,rect))
			return TRUE;
	}
	return FALSE;
}

CGraphTextLine::CGraphTextLine(long nID,CGraphTextChar *pchar)
{
	m_nLineSpace = 2;
	m_nListSpace = 0;
	m_nBeginID = nID+1;
	m_nEndID = nID+1;
	m_nBeginLine= nID+1;
	m_nEndLine = nID+1;
	m_IsTitle = FALSE;
	m_strCaption = _T("");
	m_nMakeUpType = GRPAHTXT_MKMODE_LEFT;
	
	m_nTop = 0 ;
	m_nLeft = 0;
	m_nBottom = 0;
	m_nRight = 0;

	m_CharTextList.RemoveAll();
	if(pchar==NULL)
	{
		pchar =new CGraphTextChar;
		if(pchar)
		{
			pchar->SetCaretId( nID+1 );
			pchar->SetStrText(_T("\n"));
			m_CharTextList.AddTail(pchar);
		}
	}
	else if(pchar)
	{
		pchar->SetCaretId( nID+1 );
		pchar->SetStrText( _T("\n") );
		m_CharTextList.AddTail(pchar);
	}
}

CGraphTextLine::~CGraphTextLine()
{
	POSITION pos=m_CharTextList.GetHeadPosition();
	while (pos != NULL)
		delete m_CharTextList.GetNext(pos);
	m_CharTextList.RemoveAll();
}


void CGraphTextLine::CopyFrom(CGraphTextLine *pItem )
{
	ASSERT( pItem );
	if( pItem == NULL )
		return ;

	m_nMakeUpType = pItem->m_nMakeUpType ;
	m_nLineSpace = pItem->m_nLineSpace;
	m_nListSpace = pItem->m_nListSpace;
	m_nBeginID = pItem->m_nBeginID;
	m_nEndID = pItem->m_nEndID;
	m_IsTitle = pItem->m_IsTitle;
	m_strCaption = pItem->m_strCaption ;
	
	m_nBeginLine= pItem->m_nBeginLine;
	m_nEndLine = pItem->m_nEndLine;

	m_nTop =  pItem->m_nTop;
	m_nLeft = pItem->m_nLeft;
	m_nBottom = pItem->m_nBottom;
	m_nRight = pItem->m_nRight;

}

void CGraphTextLine::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << (BYTE)m_nMakeUpType;
		ar << m_nLineSpace;
		ar << m_nListSpace;
		ar << m_nBeginID;
		ar << m_nEndID;
		ar << m_IsTitle;
		ar << m_nBeginLine;
		ar << m_nEndLine;
		ar << m_strCaption;
		ar << m_nTop ;
		ar << m_nLeft;
		ar << m_nBottom;
		ar << m_nRight ;
	}
	else
	{
		BYTE byt = 0;
		ar >> byt ;
		m_nMakeUpType = (GrapText_MkMode)byt;
		ar >> m_nLineSpace;
		ar >> m_nListSpace;
		ar >> m_nBeginID;
		ar >> m_nEndID;
		ar >> m_IsTitle;
		ar >> m_nBeginLine;
		ar >> m_nEndLine;
		ar >> m_strCaption;
		ar >> m_nTop ;
		ar >> m_nLeft;
		ar >> m_nBottom;
		ar >> m_nRight ;
	}
	m_CharTextList.Serialize(ar);
}

void CGraphTextLine::DrawScale(CDC *pDC,int nMoveX,int nMoveY,int nSelectBegin,
								 int nSelectEnd,float m_fScaleX,float m_fScaleY, int nMode)
{
	ASSERT(pDC);
	ASSERT(this);

	if(NULL != pDC)
	{
		if(!m_CharTextList.GetCount()) 
			return ;

		CString strFontName;
		strFontName = _T("����");//.LoadString(IDS_FONT_NAME);
		LOGFONT  LogFontDef = { -12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET,//GB2312_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE,
		};
		wcscpy(LogFontDef.lfFaceName,strFontName);

		CFont						m_font;
		CFont*                      oldFont=NULL;		
		m_font.DeleteObject();
		m_font.CreateFontIndirect (&LogFontDef);
		oldFont=pDC->SelectObject(&m_font);
		pDC->SetBkMode(TRANSPARENT);

		if(oldFont)
		{
			pDC->SelectObject(oldFont);
			m_font.DeleteObject();
		}
	}
	
}

void CGraphTextLine::InsterChar(int nId,CGraphTextChar *pchar)
{
	ASSERT(this);

	m_nEndID += 1;

	POSITION poscurent=NULL;
	for(POSITION pos = m_CharTextList.GetHeadPosition();pos!=NULL;)
	{
		CGraphTextChar *ptempchar = m_CharTextList.GetAt(pos);
		if(ptempchar)
		{
			if(ptempchar->GetCaretId() == nId)
			{
				poscurent = pos;

				if( pchar->GetStrText() == '\n' || pchar->GetStrText() == '\r')
				{
					pchar->SetCharX( ptempchar->GetCharX() );
					pchar->SetCharY( ptempchar->GetCharY() );
					pchar->SetWidth( ptempchar->GetWidth()) ;
					pchar->SetHeight( ptempchar->GetHeight()) ;
					pchar->SetCharShowX( ptempchar->GetCharShowX() );
					pchar->SetCharShowY( ptempchar->GetCharShowY() );
					pchar->SetFontHeight ( ptempchar->FontHeight()) ;
					pchar->SetFontWidth( ptempchar->GetFontWidth()) ;
					pchar->SetCharLine( ptempchar->GetCharLine()) ;
					pchar->SetCharList( ptempchar->GetCharList()) ;
				}
			}
			if( ptempchar->GetCaretId() >= nId)
			{
				long nCaret = ptempchar->GetCaretId() ;
				ptempchar->SetCaretId( nCaret + 1 );
			}
		}
		m_CharTextList.GetNext(pos);
	}
	if(poscurent)
	{
		pchar->SetCaretId( nId );
		m_CharTextList.InsertBefore(poscurent,pchar);
	}
}

void CGraphTextLine::MoveId(int nId,int step)
{
	ASSERT(this);
	
	m_nBeginID +=step;
	m_nEndID += step;

	for(POSITION pos = m_CharTextList.GetHeadPosition();pos!=NULL;)
	{
		CGraphTextChar *ptempchar = m_CharTextList.GetAt(pos);
		if(ptempchar)
		{
			long nCaret = ptempchar->GetCaretId();
			if( nCaret >= nId)
			{
				ptempchar->SetCaretId( nCaret+ step );
			}
		}
		m_CharTextList.GetNext(pos);
	}
}

int CGraphTextLine::DeleteChar(int nID)
{
	ASSERT(this);
	ASSERT( m_nBeginID <= nID && m_nEndID >= nID);

	int ret=0;
	
	if(m_nBeginID==m_nEndID&&m_nEndID==nID)
	{
		ret = 2; //��һ����ɾ��
	}
	else if(m_nBeginID !=m_nEndID && m_nEndID == nID)
	{
		m_nEndID-=1;
		ret = 1; //ɾ���س���
	}
	else
	{
		m_nEndID-=1;  //ɾ��һ���м������
		ret = 0;
	}

	POSITION poscurent=NULL;
	CGraphTextChar *pchar = NULL;
	for(POSITION pos = m_CharTextList.GetHeadPosition();pos!=NULL;)
	{
		CGraphTextChar *ptempchar = m_CharTextList.GetAt(pos);
		if(ptempchar)
		{
			long nCaret = ptempchar->GetCaretId() ;
			if( nCaret == nID)
			{
				poscurent = pos;
				pchar = ptempchar;
			}
			if( nCaret > nID)
			{
				ptempchar->SetCaretId( nCaret-1 );
			}
		}
		m_CharTextList.GetNext(pos);
	}
	if(poscurent&&pchar)
	{
		delete pchar;
		pchar=NULL;

		m_CharTextList.RemoveAt(poscurent);
	}
	return ret;
}



////////////////////////////////////////////////////////////////////////////
//////////////////////CHaGraphTextFont
IMPLEMENT_SERIAL(CGraphTextFont, CObject, VERSIONABLE_SCHEMA | 1 )

CGraphTextFont::CGraphTextFont()
{
	m_nColor = 0;
	m_nCharNum = 0;

	m_LogFont.lfHeight = 16; 
	m_LogFont.lfWidth = 0; 
	m_LogFont.lfEscapement = 0; 
	m_LogFont.lfOrientation = 0; 
	m_LogFont.lfWeight = FW_NORMAL; 
	m_LogFont.lfItalic = FALSE; 
	m_LogFont.lfUnderline = FALSE; 
	m_LogFont.lfStrikeOut = FALSE; 
	m_LogFont.lfCharSet = DEFAULT_CHARSET;//GB2312_CHARSET; 
	m_LogFont.lfOutPrecision = OUT_DEFAULT_PRECIS; 
	m_LogFont.lfClipPrecision = CLIP_DEFAULT_PRECIS; 
	m_LogFont.lfQuality = DEFAULT_QUALITY; 
	m_LogFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE; 
                     
	CString strFontName;
//#ifdef _HA_CHINESE
	wcscpy(m_LogFont.lfFaceName, _T("����")); 
//#else
//	strcpy(m_LogFont.lfFaceName, "Arial"); 
//#endif
}

CGraphTextFont::~CGraphTextFont()
{
}
//lint --e{1536} suppress "Exposing low access member 'CGraphTextFont::m_LogFont' : ���س�Ա������Ҫ���ͷ��ʵȼ�������const���Σ�"
LOGFONT *CGraphTextFont::GetLogFont ()
{
	return &m_LogFont;
}

COLORREF CGraphTextFont::GetFontColor()
{
	return m_nColor ;
}

void CGraphTextFont::SetFontColor(COLORREF color)
{
	m_nColor = color ;
}


int CGraphTextFont::GetCharNum()
{
	return m_nCharNum ;
}

void CGraphTextFont::SetCharNum(int nNum )
{
	m_nCharNum = nNum ;
}

void CGraphTextFont::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_nColor;
		ar << m_nCharNum;
		ar.Write(&m_LogFont,sizeof(m_LogFont));
	}
	else
	{
		ar >> m_nColor;
		ar >> m_nCharNum;
		ar.Read(&m_LogFont,sizeof(m_LogFont));
	}
}

BOOL CGraphTextFont::Compare(CGraphTextFont *pFont)
{
	ASSERT(pFont);
	ASSERT(this);
	if (NULL != pFont)
	{
		if( m_nColor!= pFont->m_nColor)
			return FALSE;
		if( m_nCharNum!=pFont->m_nCharNum )
			return FALSE;
		//if(memcmp(&m_LogFont,&pFont->m_LogFont,sizeof(m_LogFont))!=0)
		//	return FALSE;
		if( m_LogFont.lfHeight != pFont->m_LogFont.lfHeight)
			return FALSE;

		if( m_LogFont.lfWidth != pFont->m_LogFont.lfWidth )
			return FALSE ;

		if( m_LogFont.lfEscapement != pFont->m_LogFont.lfEscapement )
			return FALSE;

		if( m_LogFont.lfOrientation != pFont->m_LogFont.lfOrientation )
			return FALSE;

		if( m_LogFont.lfWeight != pFont->m_LogFont.lfWeight )
			return FALSE;

		if( m_LogFont.lfItalic != pFont->m_LogFont.lfItalic )
			return FALSE ;

		if( m_LogFont.lfUnderline != pFont->m_LogFont.lfUnderline )
			return FALSE ;

		if( m_LogFont.lfStrikeOut != pFont->m_LogFont.lfStrikeOut )
			return FALSE;

		if( m_LogFont.lfCharSet != pFont->m_LogFont.lfCharSet )
			return FALSE ;

		if( m_LogFont.lfOutPrecision != pFont->m_LogFont.lfOutPrecision )
			return FALSE ;

		if( m_LogFont.lfClipPrecision != pFont->m_LogFont.lfClipPrecision )
			return FALSE ;

		if( m_LogFont.lfQuality != pFont->m_LogFont.lfQuality )
			return FALSE ;

		if( m_LogFont.lfPitchAndFamily != pFont->m_LogFont.lfPitchAndFamily)
			return FALSE ;

		if( wcscmp( m_LogFont.lfFaceName ,pFont->m_LogFont.lfFaceName) != 0 )
			return FALSE;
		return TRUE;
	}
	
	return FALSE;
}

void CGraphTextFont::CopyFrom(CGraphTextFont *pFont)
{
	ASSERT(pFont);
	ASSERT(this);
	if (NULL != pFont)
	{
		m_nColor = pFont->m_nColor;

		m_nCharNum = pFont->m_nCharNum;

		memcpy(&m_LogFont,&pFont->m_LogFont,sizeof(m_LogFont));
	}
	
	
}

void CGraphTextFont::CopyTo(CGraphTextFont *pFont)
{
	ASSERT(pFont);
	ASSERT(this);
	if (NULL != pFont)
	{
		pFont->m_nColor = m_nColor;

		pFont->m_nCharNum = m_nCharNum;

		memcpy(&pFont->m_LogFont,&m_LogFont,sizeof(m_LogFont));
	}

}

int32  CGraphTextFont::GetFontlfHeight(const int32 &nPtWight)
{
	HDC hDC = ::GetDC(NULL);
	int32 nHeight=-MulDiv(nPtWight, GetDeviceCaps(hDC, LOGPIXELSY),72);
	if( hDC != NULL )
		::ReleaseDC(NULL,hDC);
	
	return nHeight;
}

int32 CGraphTextFont::GetFontptHeight(const int32 &nlfWight)
{
	HDC hDC = ::GetDC(NULL);
	int32 nHeight=-MulDiv(nlfWight, 72,GetDeviceCaps(hDC, LOGPIXELSY));
	if( hDC != NULL )
		::ReleaseDC(NULL,hDC);
	
	return nHeight;
}

////////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CGraphTextCharLineFlag, CGraphTextChar, VERSIONABLE_SCHEMA | 1 )

//���캯��һ
CGraphTextCharLineFlag::CGraphTextCharLineFlag()
{
	m_nDrawModel = 0;
	m_nLineW = 100;
}

CGraphTextCharLineFlag::~CGraphTextCharLineFlag()
{

}

void CGraphTextCharLineFlag::DrawScale(CDC *pDC,int nMoveX,int nMoveY,int nSelectBegin,int nSelectEnd,
					   float m_fScaleX,float m_fScaleY, int nMode)
{
	int nsaveDc = pDC->SaveDC();
	
	COLORREF crl = pDC->GetTextColor();
	CString strText = GetStrText();
	if( strText == "\r" || strText == "\n") // ��ʾ�س���
	{	
		int nYshow = (int)(GetCharShowY() * m_fScaleY);
		int nXShow = (int)(GetCharShowX() * m_fScaleX);
		int x = nXShow + nMoveX;//nX + nMoveX;
		int y = nYshow + nMoveY;//nY + nMoveY;
		CPen pen(PS_SOLID, 1, crl);
		CPen* pPenOld = pDC->SelectObject(&pen);
		
		pDC->MoveTo(x,y);
		pDC->LineTo(x+100,y);

		pDC->SelectObject(pPenOld);
		pen.DeleteObject();
	}
	pDC->RestoreDC(nsaveDc);
}

void CGraphTextCharLineFlag::Serialize(CArchive& ar)
{
	CGraphTextChar::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_nDrawModel;
	}
	else
	{
		ar >> m_nDrawModel;
	}
}

void CGraphTextCharLineFlag::DrawScale(CDC *pDC,int nMoveX,int nMoveY, float m_fScaleX ,float m_fScaleY, int nMode)
{
	int nsaveDc = pDC->SaveDC();
	
	COLORREF crl = pDC->GetTextColor();
	CString strText = GetStrText();
	if( strText == "\r" || strText == "\n") // ��ʾ�س���
	{	
		int nYshow = (int)(GetCharShowY() * m_fScaleY);
		int nXShow = (int)(GetCharShowX() * m_fScaleX);
		int x = nXShow + nMoveX;//nX + nMoveX;
		int y = nYshow + nMoveY;//nY + nMoveY;
		CPen pen(PS_SOLID, 1, crl);
		CPen* pPenOld = pDC->SelectObject(&pen);
		
		if( m_nDrawModel == 0 )
		{
			y +=3;
			pDC->MoveTo(x,y);
			pDC->LineTo(m_nLineW,y);
		}
		else if( m_nDrawModel == 1 )
		{
			y = y+GetHeight()/2;
			pDC->MoveTo(x,y);
			pDC->LineTo(m_nLineW,y);
		}
		else
		{
			y = y+GetHeight()-3;
			pDC->MoveTo(x,y);
			pDC->LineTo(m_nLineW,y);
		}
		
		pDC->SelectObject(pPenOld);
		pen.DeleteObject();
	}
	pDC->RestoreDC(nsaveDc);
}
