#include "stdafx.h"
#include "GridCellSys.h"
#include "IoViewManager.h"
#include "GridCtrl.h"
#include "facescheme.h"
#include "MPIChildFrame.h"
#include "GridCellSymbol.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
IMPLEMENT_DYNCREATE(CGridCellSymbol, CGridCellSys)

CGridCellSymbol::CGridCellSymbol()
:CGridCellSys()
{
	m_iShowSymbol = 0;
}
// Override draw so that when the cell is selected, a drop arrow is shown in the RHS.
BOOL CGridCellSymbol::Draw(CDC* pDC, int nRow, int nCol, CRect rect,  BOOL bEraseBkgnd /*=TRUE*/)
{
	// 根据字符串首个字符是+或-来判断价格是升/降	
	COLORREF ClrText;
	CIoViewBase * pIoViewParent = NULL; 
	
	CWnd * pParent = GetGrid()->GetParent();
	pIoViewParent = DYNAMIC_DOWNCAST(CIoViewBase, pParent);

// 	CRuntimeClass * pRunTime = pParent->GetRuntimeClass();
// 	
// 	for ( int32 i = 0 ; i < CIoViewManager::GetIoViewObjectCount(); i++)
// 	{
// 		if ( pRunTime == CIoViewManager::GetIoViewObject(i)->m_pIoViewClass)
// 		{
// 			pIoViewParent = (CIoViewBase*)pParent;
// 			break;
// 		}
// 	}
// 	if (pParent->IsKindOf(RUNTIME_CLASS(CIoViewBase)))
// 	{
// 		pIoViewParent = (CIoViewBase *)pIoViewParent;
// 	}

	if ( NULL != pIoViewParent)
	{
		ClrText = pIoViewParent->GetIoViewColor(ESCKeep);
	}
	else
	{
		ClrText = CFaceScheme::Instance()->GetSysColor(ESCKeep);
	}


	bool32 bSkipFirstChar = false;

	CString StrTextBackup = GetText();
	if (StrTextBackup.GetLength() > 0)
	{
		if (StrTextBackup[0] == L'+')
		{			
			if (NULL != pIoViewParent)
			{				
				ClrText =  pIoViewParent->GetIoViewColor(ESCRise);
			}
			else
			{
				ClrText = CFaceScheme::Instance()->GetSysColor(ESCRise);
			}						
			if (!(m_iShowSymbol & ESSRise))
				bSkipFirstChar = true;
		}
		else if (StrTextBackup[0] == L'-')
		{
			if (NULL != pIoViewParent)
			{				
				ClrText =  pIoViewParent->GetIoViewColor(ESCFall);
			}
			else
			{
				ClrText = CFaceScheme::Instance()->GetSysColor(ESCFall);
			}
			
			if (!(m_iShowSymbol & ESSFall))
				bSkipFirstChar = true;
		}
		else
		{
			if (NULL != pIoViewParent)
			{				
				ClrText =  pIoViewParent->GetIoViewColor(ESCKeep);	
			}
			else
			{
				ClrText = CFaceScheme::Instance()->GetSysColor(ESCKeep);
			}						
			if (!(m_iShowSymbol & ESSKeep))
			{
				if (StrTextBackup[0] == L' ')
				{
					bSkipFirstChar = true;
				}
			}
		}		
	}
	
	if (bSkipFirstChar)
	{
		CString StrText = StrTextBackup.Right(StrTextBackup.GetLength() - 1);
		
		SetTextOnly(StrText);
	}	
	
	SetTextClr(ClrText);
	BOOL bResult = CGridCellSys::Draw(pDC, nRow, nCol, rect,  bEraseBkgnd);

	if (bSkipFirstChar)
	{
		SetTextOnly(StrTextBackup);
	}
	return bResult;
}


