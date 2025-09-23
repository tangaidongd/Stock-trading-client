#if !defined(AFX_NEWTBCHILDWND__INCLUDED_)
#define AFX_NEWTBCHILDWND__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TBWnd.h : header file
//
#include <map>
using namespace std;

#include "NCButton.h"
#include "ToolBarIDDef.h"
#include "ConfigInfo.h"
#include "DlgPullDown.h"
#include "WndCef.h"

/////////////////////////////////////////////////////////////////////////////
// CNewTBChildWnd window



class CNewTBChildWnd : public CWnd
{
// Construction
public:

	CNewTBChildWnd(CWnd *pWndParent, buttonContainerType &mapBtn);
	virtual ~CNewTBChildWnd();
public:

	// ���õ�ǰ��ʾ��ҳ��
	void    ShowPage(int32 iID);			// ���밴ť��ID
	bool32  InitialCurrentWSP();

	// �ı����ͼ��״̬
	void ChangeGroupStatus(buttonContainerType &mapBtn, int iID);

	// �ָ���ť״̬
	void RecoverToolbarButtons();

	void SetBtnCheckStatus(bool bCheck);

	void RefreshControlBtn();

	void SetCheckStatusByName(CString strCfmName);
	
private:
	
    void DrawToolbar(CDC& dc);
	// ����Ƿ��ڰ�ť��
    int  TButtonHitRect(CPoint point);  

	//////////////////////////////////////////////////////////////////////////���¿��ƹ������ϰ�ť�ƶ�����
	//	��ȡ��ര��߽����򽻽�����ť���Ҳ�����ư�ť���򽻼���ť
	int32	GetIntersectButton(bool bLeft);
	//�жϾ����Ƿ�����Ŀ�ľ��ε��Ӽ�	
	bool32 IsSubRect(const CRect& destRect, const CRect& subRect);
	//	bLeftDirectΪtrue�������������ƣ���֮��������������
	void MoveToolButtons(bool32 bLeftDirect);
	//	��ʾ�����ݸ������С�Զ��ƶ����ư�ť
	void ShowControlButton(ECButtonType type);
	//	���ֹ��������Ҳ�Ŀ��ư�ť��ʽ
	ECButtonType DecideControlButtonType();	
	//	������������£��������й�������ť
	void  AlignToolButton();
	//	��ȡ�Ҳ�����������Ҳ�����ߵĿ��ư�ťֱ������߽����򣬿����������Ϊ��
	CRect GetControlRect(bool32 bLeft=true);

	//	���������е����id��ť������
	#define  FIND_BTN_ITERATOR(container, id) std::find_if(container.begin(), container.end(),vector_finder(id))

	//	���������е����id�İ�ť��
	inline CNCButton&	GetCNCButton(buttonContainerType& container, int iId)
	{
		buttonContainerType::iterator it = container.begin();
		FOR_EACH(container, it)
		{
			if ((*it).GetControlId() == iId)
			{
				return *it;
			}
		}
		//����������Ӧ�ô���
		//ASSERT(0);
		return CNCButton();
	}
	
private:

	Image*			    m_pLeftImage;
	Image*			    m_pRightImage;
	Image*              m_pImgPermission;


	//	����3�а�ť�����࣬��֧���Զ�����֧�������id����
    buttonContainerType &m_mapBtn;							//	��������ť��
	buttonContainerType m_mapControlBtn;					//	���ư�ť����
	bool32		        m_bShowIntersect;					//	�Ƿ���ʾ��������ť����ư�ť�����İ�ť	

private:

	// ��ǰ����һҳ
	int32		            m_iCurrentPage;

	//
	CWnd		           *m_pWndParent;
	int			            m_iXButtonHovering;			// ��ʶ�����밴ť����

	int                     m_iMoveSpace;				// �����ƶ�����
	bool32                  m_bLeftDirect;				// �Ƿ������ƶ�

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewTBChildWnd)
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL


protected:
	//{{AFX_MSG(CNewTBChildWnd)
	afx_msg	int	 OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg	BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg	LRESULT	OnIdleUpdateCmdUI(WPARAM w, LPARAM l);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWTBCHILDWND__INCLUDED_)
