#if !defined(_DLG_F10_H)
#define _DLG_F10_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgf10.h : header file


#include "sharestructnews.h"
#include "AutoRichEditCtrl.h"
#include "DialogEx.h"
#include "ViewDataListener.h"

class CViewData;
class CIoViewBase;
class CMerch;
class CMmiNewsRespF10;


// F10 �Ľڵ�
typedef struct T_NewsNode
{
	T_NewsNode()
	{
		Clear();
	}

	bool32 operator!= (const T_NewsNode& stNewsNode)
	{
		if ( m_iInfoIndex != stNewsNode.m_iInfoIndex 
		  || m_eDataType != stNewsNode.m_eDataType
		  || m_eInfoType != stNewsNode.m_eInfoType
		  || m_eChildDataType != stNewsNode.m_eChildDataType
		  || m_StrContent != stNewsNode.m_StrContent
		  || m_StrTime != stNewsNode.m_StrTime
		  || m_StrCrc32 != stNewsNode.m_StrCrc32
		  || m_RectShow != stNewsNode.m_RectShow 
		   )
		{
			return true;
		}

		return false;
	}

	void Clear()
	{
		m_iInfoIndex	= -1;
		m_eDataType		= EDTCount;
		m_eInfoType		= EITCount;
		m_eChildDataType= EDTCount;
		m_StrContent	= L"";
		m_StrTime		= L"";	
		m_StrCrc32		= L"";
		m_RectShow		= CRect(-1, -1, -1, -1);
	}

	int32			m_iInfoIndex;
	E_DataType		m_eDataType;
	E_InfoType		m_eInfoType;
	E_DataType		m_eChildDataType;
	CString			m_StrContent;
	CString			m_StrTime;
	CString			m_StrCrc32;
	CRect			m_RectShow;

}T_NewsNode;

// �����ֶ�
typedef struct T_SpecialNode
{
	int32			m_iInfoIndex;
	int32			m_iBeginPos;
	int32			m_iTotalCount;
	int32			m_iMarketId;
	CString			m_StrCode;
}T_SpecialNode;

/////////////////////////////////////////////////////////////////////////////
// CDlgF10 dialog
class CDlgF10 : public CDialogEx ,public CViewNewsListner
{
enum E_ReqState
{
	ERSReqing = 0,
	ERSReqFinished,

	ERSCount,
};

public:
	CDlgF10::CDlgF10(CIoViewBase* pIoViewActive, CMerch* pMerch, CWnd* pParent /* = NULL */);

public:
	// �����������ж�
	void		OnNewsServerDisConnected(int32 iCommunicationId);

	// ������������
	void		OnNewsServerConnected(int32 iCommunicationId);

	// �յ� F10 ����
	void		OnNewsRespF10(IN const CMmiNewsRespF10* pMmiNewsRespF10);		

	// ��ȡ��Ʒ
	CIoViewBase* GetIoView() { return m_pIoViewActive; }

	// ��Ʒ�л�
	void		OnMerchChange(IN CMerch* pMerch, bool32 bForceReq = false);

	// ѡ����Ŀ�仯
	void		OnSelectedItemChange(int32 iIndexNow);

	// ������С, ʹ֮���
	void		AdjustToMaxSize();

private:
	
	// ������ʾ
	void		UpdateShow();
	
	// �õ����ڵ�������Ʒ
	void		GetSblingMerch(OUT CMerch*& pMerchPre, OUT CMerch*& pMerchAft);

	// ���� F10 ����
	bool32		RequestF10(CMerch* pMerch, int32 iIndexInfo);
	
	// ���� F10 �ַ��� fangz0612 ��������Ժ�ŵ���������. ����������Ҳ�����Ƶ�
	bool32		ParaseF10String(IN TCHAR* pStrF10, OUT CString& StrErrMsg);
 
	// �Ƿ��ǺϷ��������ֶ���
	bool32		BeValidSpecialName(TCHAR* pStrSpecialName);
	
	// �Ƿ��ǺϷ�����ͨ�ֶ���
	bool32		BeValidNormalName(TCHAR* pStrSpecialName);
	
	// ����������������ڵ���ֶ�ֵ
	void		SetSpecialNodeValue(int iIndex, const CString& StrValue, T_SpecialNode& stSpecialNode);
	
	// ��������������ͨ�ڵ���ֶ�ֵ
	void		SetNewsNodeValue(int iIndex, const CString& StrValue, T_NewsNode& stNewsNode);

	// ת��
	int32		CStringToInt(TCHAR* pStrNum);
	int32		CStringToInt(const CString& StrNum);

	void		OnDataRespNewsData(int iMmiReqId, IN const CMmiNewsBase *pMmiNewResp, CMerch *pMerch);
	
private:
	
	// ViewData
	CAbsCenterManager*							 m_pAbsCenterManager;
	
	// ��ǰ��Ʒ
	CMerch*								 m_pMerch;
	
	// ��ǰ������ͼ
	CIoViewBase*						 m_pIoViewActive;

	// ����״̬(��֤�����Ե�����, һ������û����֮ǰ������һ������, ����ᵼ�� m_iInfoIndexNow ����)
	E_ReqState							m_eReqState;

	// ��ǰѡ�е��ӽڵ�
	int32								 m_iInfoIndexNow;

	// ����
	CRect								 m_RectMore;
	
	// ǰһ����Ʒ
	CRect								 m_RectMerchPre;

	// ��һ����Ʒ
	CRect								 m_RectMerchAft;

	// �Ƿ���ʾ�˸���
	bool32								 m_bNeedMore;

	// �Ƿ�ȫ��
	bool32								 m_bMaxed;

	// �Ƿ����
	bool32								 m_bServerDisConnect;

	// ����ʾ�����ʱ��, �˵�����ʾ�ĵ�һ����Ŀ
	T_NewsNode							 m_stNewsNodeFistAsMore;

	// �����ֶ�
	T_SpecialNode						 m_stSpecialNode;

	// ��ǰ��ʾ�Ľڵ�
	T_NewsNode							 m_stNewsNodeNow;
	
	// ������ʾ, ��˾�ſ�, ������� �ȱ���ڵ�
	CArray<T_NewsNode, T_NewsNode&>		 m_aNewTitles;
	
	// �����ֶ���ͷ��Ӧ������
	CMap<int32, int32, CString, LPCTSTR> m_aMapTableSpecial;

	// F10 �ڵ���ͷ��Ӧ������
	CMap<int32, int32, CString, LPCTSTR> m_aMapTableNewsNode;

// Construction
public:
   // CDlgF10(CWnd* pParent = NULL);   // standard constructor
   ~CDlgF10();
   
// Dialog Data
	//{{AFX_DATA(CDlgF10)
	enum { IDD = IDD_DIALOG_F10 };
	CAutoRichEditCtrl m_RichEdit;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgF10)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();	
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgF10)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnF10Menu (UINT nID);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMsgHotKey(WPARAM wParam, LPARAM lParam);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DLG_F10_H)
