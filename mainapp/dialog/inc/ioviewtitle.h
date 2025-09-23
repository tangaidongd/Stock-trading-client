#if !defined(AFX_IOVIEWTITLE_H__)
#define AFX_IOVIEWTITLE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ioviewtitle.h : header file
//
#include <Gdiplus.h>
#include "Region.h"
#include "XTipWnd.h"
using namespace Gdiplus;


/////////////////////////////////////////////////////////////////////////////
// CIoViewTitle window

class CIoViewBase;


class CIoViewTitleButton;

// ...fangz1016 Ӧ��ÿ����ť�м��ֲ�ͬ�Ļ���,���ṩ��־����������,�����ؼ���ҵ���޹�

class CIoViewTitle : public CStatic
{
// Construction
public:
	CIoViewTitle();
	virtual	~CIoViewTitle();

	void		Reset();															// �������
	CRect		GetRect();															// �õ�����
	int32		GetButtonNums();													// �õ����а�ť����
	void		SetShowRect(const CRect& rectSet);									// ������ʾ����
	void		SetOwnerID(const UINT& uID);	
	void		AddButton(bool32 bPublic,CIoViewTitleButton* pButton);				// ��Ӱ�ť
	void		DelButton(bool32 bPublic,UINT iGuid);								// ɾ����ť
	void		AddLeftRightButton(bool32 bLeft,CIoViewTitleButton* pButton);		// ��ӷ����
	void		AddFuctionButton(CIoViewTitleButton* pButton);						// ��ӹ��ܰ�ť	
	void		ClearButtonFocusFlag();												// ȥ������
	void		SetFuctionBtnFlag(bool32 bShowSameTime);							// ���ù��ܰ�ť��ʾ����	
	void		SetMaxFlag(bool32 bMax);											// ��󻯱�־
	void		SetParentDragFlag(bool32 bParentDrag);								// �����϶��ı�־
	void		ForcePaint();														// ˢ��		
	void		SetBkGroundColor(COLORREF clrBk);									// ����ɫ

	void		SetDC(CMemDCEx* pDC);												// ����DC
	void		SetDCTransformParam(int32 ix,int32 iy);								// ����DC ��ƫ����
	void		Transform(int32 ix,int32 iy);										// ����ת��
	

	CIoViewTitleButton*			  GetButtonByFocus();								// �õ�������ť
	CIoViewTitleButton*			  GetButtonByPoint(CPoint point);					// ͨ������õ���ť
	CIoViewTitleButton*			  GetPrivateButtonByID(UINT uID);					// ͨ��ID�õ�˽�а�ť
	CIoViewTitleButton*			  GetFuctionButtonByID(UINT uID);					// ͨ��ID�õ����ܰ�ť

private:
	UINT		m_uIDOwner;															// ������ĸ�IoView �ı�����			
	CRect 		m_RectSet;															// �������ܸ�������
	CRect 		m_RectPublicBtns;													// ������ť������
	CRect 		m_RectPrivateBtns;													// ˽�а�ť����
	CRect		m_RectFuctionBtns;													// ���ܰ�ť����
	bool32		m_bShowLeftRight;													// �Ƿ����Ҽ�ͷ
	bool32		m_bShowFuctionBtnSameTime;											// ���ƹ��ܰ�ť�ǲ���ͬʱ��ʾ.( true: ����ʾ�����еİ�ť����ʾ,����һ��������ʾ false: ����ʾ��������ʾ����)

	bool32		m_bTracking;														// ��������뿪�ı�־
	bool32		m_bParentMaxed;														// ��ͼ�Ƿ������ ...fangz1016 ��ҵ�������,Ҫ��
	bool32		m_bParentDrag;														// �����Ƿ������϶�
	CXTipWnd	m_TipWnd;															// Tip
	
	CMemDCEx*	m_pDCParent;														// �������õ�DC
	int32		m_ixDC;																// ��DC ������ͱ��������x ƫ��	
	int32		m_iyDC;																// ��DC ������ͱ��������y ƫ��
	COLORREF	m_ColorBk;															// ����ɫ
	
	CIoViewTitleButton*	m_pLeftButton;												// ���ͷ
	CIoViewTitleButton*	m_pRightButton;												// �Ҽ�ͷ

	CArray<CIoViewTitleButton*, CIoViewTitleButton*>	m_apPublicBtns;				// ���й�����ť
	CArray<CIoViewTitleButton*, CIoViewTitleButton*>	m_apPrivateBtns;			// ����˽�а�ť
	CArray<CIoViewTitleButton*, CIoViewTitleButton*>	m_apFuctionBtns;			// ���й��ܰ�ť
protected:
	//{{AFX_MSG(CIoViewTitle)
	afx_msg void OnPaint();															// ����
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);							// �������
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);							// �Ҽ�����
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);							// ����ƶ�
	afx_msg LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);						// ����뿪
	afx_msg void OnTimer(UINT nIDEvent);											// ��ʱ��
	// virtual BOOL PreTranslateMessage(MSG* pMsg);					
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
// ��������ť
 
class CIoViewTitleButton
{
friend class CIoViewTitle;

public:
	CIoViewTitleButton();
	
	CIoViewTitleButton(const HWND hOwner, CIoViewBase* pParent, const bool32 bLeft);																															// ���Ҽ�ͷ
	CIoViewTitleButton(const HWND hOwner, CIoViewBase* pParent, const UINT uID, const bool32 bPublic, const CString& StrText, const CString& StrTips, UINT uiBmpNormal, UINT uiBmpFocus, UINT uiBmpSelf);		// ��ͨ��ť
	
	virtual		~CIoViewTitleButton();

public:
	void		FillWithBitmap(CDC* pDC, UINT uiBitmapID, const CRect& rt);		// ��λͼ���
	void		DrawButton(CDC* pDC, COLORREF clrBk, bool32 bMax);					// ����

	CRect		GetShowRect();														// �õ���ʾ����
	void		SetShowRect(const CRect& RectButton);								// ������ʾ����														

	void		OnLButtonDown();													// �������
	void		OnRButtonDown();
	
	UINT		GetButtonID();														// �õ�ID��

	bool32		GetSelfFlag();														// �õ���ʶ
	void		SetSelfFlag(bool32 bSelf);											// ���ñ�ʶ
	
	bool32		BeFocusButton();													// �Ƿ��Ǹ���
	void		SetFocusButton(bool32 bFocus);										// �����Ƿ����	

	void		SetText(const CString& StrText);									// ���ñ���
	void		SetTips(const CString& StrTip);										// ����Tips	
	
	CIoViewBase* TestGetParentIoView();
private:		
	HWND		m_hOwner;															// ������Ϣ��
	CIoViewBase*m_pOwner;
	UINT		m_uID;																// ��������(1,2,3,4,5,6,7..)
	bool32 		m_bPublic;															// �Ƿ񹫹���ť
	CRect		m_RectButton;														// ��ʾ����
	CString		m_StrText;															// ����
	CString		m_StrTips;															// Tips

	bool32		m_bRight;															// ���Ҽ�ͷ��ť
	bool32		m_bLeft;															// �����ͷ��ť
	bool32		m_bSelf;															// �Լ������ť
	bool32		m_bFocus;															// ����ƶ������.����
	
	UINT		m_uiBmpNormal;														// ������λͼID
	UINT		m_uiBmpFocus;														// ����ʱ���λͼID	
	UINT		m_uiBmpSelf;														// ��ʾ�Լ�ʱ���λͼID

	Image*		m_pImageNormal;														// ͼƬ��Դ (����)
	Image*		m_pImageFocus;														// ͼƬ��Դ (����)
	Image*		m_pImageSelf;														// ͼƬ��Դ (�Լ�)
};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_IOVIEWTITLE_H__)

