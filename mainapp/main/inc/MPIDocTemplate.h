/*********************************************************
* Multi-Page Interface
* Version: 1.2
* Date: September 2, 2003
* Author: Michal Mecinski
* E-mail: mimec@mimec.w.pl
* WWW: http://www.mimec.w.pl
*
* You may freely use and modify this code, but don't remove
* this copyright note.
*
* There is no warranty of any kind, express or implied, for this class.
* The author does not take the responsibility for any damage
* resulting from the use of it.
*
* Let me know if you find this code useful, and
* send me any modifications and bug reports.
*
* Copyright (C) 2003 by Michal Mecinski
*********************************************************/

#pragma once

class CMPIChildFrame;

// Macros describing the layout of the views
#define BEGIN_MPI_FRAME(templ, id)		if (!(templ)->CreateMPIFrame(id,
#define END_MPI_FRAME					MPIT_END)) return FALSE;

#define MPI_VIEW(_class)				MPIT_VIEW, RUNTIME_CLASS(_class), 0,
#define MPI_VIEW_EX(_class, ex)			MPIT_VIEW, RUNTIME_CLASS(_class), ex,

#define MPI_VSPLIT(prop)				MPIT_VSPLIT, prop,
#define MPI_HSPLIT(prop)				MPIT_HSPLIT, prop,

#define MPI_BAR_TOP_A					MPIT_HSPLIT, MPIS_BAR_TOP, MPIBS_AUTO,
#define MPI_BAR_TOP(size)				MPIT_HSPLIT, MPIS_BAR_TOP, size,
#define MPI_BAR_BOTTOM_A				MPIT_HSPLIT, MPIS_BAR_BOTTOM, MPIBS_AUTO,
#define MPI_BAR_BOTTOM(size)			MPIT_HSPLIT, MPIS_BAR_BOTTOM, size,
#define MPI_BAR_LEFT_A					MPIT_VSPLIT, MPIS_BAR_LEFT, MPIBS_AUTO,
#define MPI_BAR_LEFT(size)				MPIT_VSPLIT, MPIS_BAR_LEFT, size,
#define MPI_BAR_RIGHT_A					MPIT_VSPLIT, MPIS_BAR_RIGHT, MPIBS_AUTO,
#define MPI_BAR_RIGHT(size)				MPIT_VSPLIT, MPIS_BAR_RIGHT, size,

#define MPI_TABS(id)					MPIT_TABS, id,


class CMPIDocTemplate : public CMultiDocTemplate
{
	DECLARE_DYNAMIC(CMPIDocTemplate)

public:
	CMPIDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass);

public:
	// Get child frame from index
	CMPIChildFrame* GetChildFrame(int nIndex);
	// Get index of the child frame
	int FindChildFrame(CMPIChildFrame* pFrame);

	// Create child frame with given layout parameters (use macros above)
	CMPIChildFrame* CreateMPIFrame(int nIDResource, ...);

	// Get view param from the create context
	static DWORD GetViewParam(CCreateContext* pContext);

public:
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE);
	virtual CDocument* CreateNewDocument();
protected:
	virtual ~CMPIDocTemplate();

protected:
	CMapWordToPtr m_mapMenu;
	CMapWordToPtr m_mapAccel;
	CArray<CMPIChildFrame*, CMPIChildFrame*> m_arrChildFrm;
		
	DECLARE_MESSAGE_MAP()
};

