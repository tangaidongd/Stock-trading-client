/*********************************************************
* Multi-Page Interface
* Version: 1.2
* Date: September 2, 2003
* Author: Michal Mecinski
* E-mail: mimec@mimec.w.pl
* WWW: http://www.mimec.w.pl
*
* Copyright (C) 2003 by Michal Mecinski
*********************************************************/

#include "stdafx.h"

#include "MPIChildFrame.h"


#include "MPIDocTemplate.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CMPIDocTemplate, CMultiDocTemplate)

CMPIDocTemplate::CMPIDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass)
	: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, NULL)
{
}

CMPIDocTemplate::~CMPIDocTemplate()
{
	WORD nID;	// destroy shared menus and accelerators
	POSITION pos = m_mapMenu.GetStartPosition();
	while (pos)
	{
		HMENU hMenu;
		m_mapMenu.GetNextAssoc(pos, nID, (void*&)hMenu);
		DestroyMenu(hMenu);
	}
	pos = m_mapAccel.GetStartPosition();
	while (pos)
	{
		HACCEL hAccel;
		m_mapAccel.GetNextAssoc(pos, nID, (void*&)hAccel);
		DestroyAcceleratorTable(hAccel);
	}
}

BEGIN_MESSAGE_MAP(CMPIDocTemplate, CMultiDocTemplate)
END_MESSAGE_MAP()


CDocument* CMPIDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible)
{
	// don't create new document
	// NOTE: no support for serialization
	POSITION pos = GetFirstDocPosition();
	return GetNextDoc(pos);
}

CDocument* CMPIDocTemplate::CreateNewDocument()
{
	CDocument* pDoc = CMultiDocTemplate::CreateNewDocument();
	if (pDoc)
	{
		if (pDoc->OnNewDocument())
		{
			return pDoc;
		}
		delete pDoc;
	}
	return NULL;
}

CMPIChildFrame* CMPIDocTemplate::CreateMPIFrame(int nIDResource, ...)
{
	POSITION pos = GetFirstDocPosition();
	CDocument* pDoc = GetNextDoc(pos);

	if (pDoc)
	{
		m_pViewClass = NULL;

		UINT nLastID = m_nIDResource;
		HMENU hLastMenu = m_hMenuShared;
		HACCEL hLastAccel = m_hAccelTable;
		
		m_nIDResource = nIDResource;

		HMENU hNewMenu;
		// check if menu already created
		if (m_mapMenu.Lookup(nIDResource, (void*&)hNewMenu))
			m_hMenuShared = hNewMenu;
		else
		{	// load menu from resources
			hNewMenu = LoadMenu(AfxGetInstanceHandle(), MAKEINTRESOURCE(nIDResource));
			if (hNewMenu)
			{
				m_hMenuShared = hNewMenu;
				m_mapMenu[nIDResource] = hNewMenu;
			}
		}

		HACCEL hNewAccel;
		// check if accelerators already created
		if (m_mapAccel.Lookup(nIDResource, (void*&)hNewAccel))
			m_hAccelTable = hNewAccel;
		else
		{	// load accelerators from resources
			hNewAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(nIDResource));
			if (hNewAccel)
			{
				m_hAccelTable = hNewAccel;
				m_mapAccel[nIDResource] = hNewAccel;
			}
		}

		// initialize the extented create context
		CMPICreateContext context;
		context.m_pCurrentFrame = NULL;
		context.m_pCurrentDoc = pDoc;
		context.m_pNewViewClass = NULL;
		context.m_pNewDocTemplate = this;
		context.m_pSplitData = &nIDResource + 1;	// remaining function arguments

		// create and load child frame
		CMPIChildFrame* pFrame = (CMPIChildFrame*)m_pFrameClass->CreateObject();
		ASSERT_KINDOF(CMPIChildFrame, pFrame);
		if (!pFrame->LoadFrame(m_nIDResource, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, &context))
			return NULL;

		// restore data (already obtained by the child frame)
		m_nIDResource = nLastID;
		m_hMenuShared = hLastMenu;
		m_hAccelTable = hLastAccel;

		if (pFrame)
		{
			// load frame's title from resources
			CString strTitle;
			strTitle.LoadString(nIDResource);
			pFrame->SetTitle(strTitle);

			// display the window
			// 此处会先显示一会，然后在被隐藏，如果为FALSE调用，则以后需要调用Initiali
			InitialUpdateFrame(pFrame, pDoc, FALSE);
			pFrame->ShowWindow(SW_HIDE);
			
			m_arrChildFrm.Add(pFrame);

			return pFrame;
		}
	}

	return NULL;
}

int CMPIDocTemplate::FindChildFrame(CMPIChildFrame *pFrame)
{
	for (int i=0; i<m_arrChildFrm.GetSize(); i++)
	{
		if (m_arrChildFrm[i] == pFrame)
			return i;
	}
	return -1;
}

CMPIChildFrame* CMPIDocTemplate::GetChildFrame(int nIndex)
{
	return m_arrChildFrm[nIndex];
}


DWORD CMPIDocTemplate::GetViewParam(CCreateContext* pContext)
{
	return ((CMPICreateContext*)pContext)->m_dwViewParam;
}

