#ifndef _IO_VIEW_MANAGER_H_
#define _IO_VIEW_MANAGER_H_

#include "GuiTabWnd.h"
#include "IoViewChart.h"
#include "IoViewPrice.h"
#include "ControlBase.h"
#include "tinyxml.h"
#include "AdjustViewLayoutDlg.h"

#define MAXGROUPID 65536

class CIoViewBase;
class CAdjustViewLayoutDlg;
//
enum E_IoViewGroupType
{
	E_IVGTNone = 0,
	E_IVGTChart,
	E_IVGTReport,
//	E_IVGTRelative,
	E_IVGTPrice,
	E_IVGTOther,
	E_IVGTCount,
};

// ��ͼ��ռ�ֵ���ͼ��ʾ��ģʽ
enum E_IoViewBeyondShow////��������޸ļǵ��޸�facescheme.cpp ��Ӧ�Ľṹ
{
	EIBSNone = 0,		// �����ؼ�
	EIBSVertical,		// ͨ�����
	EIBSHorizontal,		// ���аԵ�
	EIBSBoth,			// Ψ�Ҷ���

	EIBSCount,
};

// ��ͼ ��������ͼ��Ʒ�л����� ����(���Ʊ��۱�, �����еȵȱ�������ͼ)
enum E_IoViewMerchChangeSrc //��������޸ļǵ��޸�facescheme.cpp ��Ӧ�Ľṹ
{
	EIMCSFalse = 0,		// ��ͨ��ͼ
	EIMCSTrue,			// ���۱�����ͼ

	EIMCSCount,
};

// ҵ����ͼ���սṹ ��������޸ļǵ��޸�facescheme.cpp ��Ӧ�Ľṹ
typedef struct T_IoViewObject
{
	UINT					m_uID;								// ����ID
	E_IoViewBeyondShow		m_eBeyondShow;						// �Ƿ�ǿ��ռ��ͬһ�зִ����ֵܵ����������ʾ
	E_IoViewMerchChangeSrc	m_eMerchChangeSrc;					// �Ƿ���������ͼ��Ʒ�л�Դ
	CString					m_StrLongName;						// ����	
 	CString					m_StrShortName;						// ����
 	CString					m_StrTipMsg;						// Tips
	CString					m_StrXmlName;						// Xml ��
	E_IoViewGroupType		m_eGroupType;						// ��������е���ͼ����

	CRuntimeClass*			m_pIoViewClass;
	bool32					m_bAllowDragAway;						// �Ƿ�������������������

	DWORD					m_dwAttendMerchType;				// ��ҵ����ͼ�������ʺϴ������Ʒ���ͣ�Ĭ��ȫ��

 	T_IoViewObject(const UINT& uID,
				   const E_IoViewBeyondShow& eBeyondShow,
				   const E_IoViewMerchChangeSrc& eMerchChangeSrc,
				   const CString &StrLongName,
				   const CString &StrShortName,
				   const CString &StrTipMsg,
				   const CString &StrXmlName,
				   const E_IoViewGroupType& eGroupType,				   
				   CRuntimeClass *pIoViewClass,
				   bool32	bAllowDragAway,
				   DWORD	dwAttendMerchType = 0xffffffff)
 	{
		m_uID				 = uID;
		m_eBeyondShow		 = eBeyondShow;
		m_eMerchChangeSrc	 = eMerchChangeSrc;
 		m_StrLongName		 = StrLongName;
 		m_StrShortName		 = StrShortName;
 		m_StrTipMsg			 = StrTipMsg;
 		m_StrXmlName		 = StrXmlName;
		m_eGroupType		 = eGroupType;
 		m_pIoViewClass		 = pIoViewClass;
		m_bAllowDragAway	 = bAllowDragAway;
		m_dwAttendMerchType  = dwAttendMerchType;
	};

}T_IoViewObject;

// ҵ����ͼ�����սṹ

typedef struct T_IoViewGroupObject
{
	CString				m_StrLongName;
	CString				m_StrShortName;
	E_IoViewGroupType	m_eIoViewGroupType;

	T_IoViewGroupObject(const CString& StrLongName,const CString& StrShortName,const E_IoViewGroupType& eIoViewGroupType)
	{
		m_StrLongName	   = StrLongName;
		m_StrShortName	   = StrShortName;
		m_eIoViewGroupType = eIoViewGroupType;
	}

}T_IoViewGroupObject;



// ��ǩ����,
typedef enum E_ManagerTopbarType
{
	MANAGERBAR_CHANGE_MARKET = 1,			// �л��г�
	MANAGERBAR_OPEN_CFM,					// �򿪰���
	MANAGERBARCount,

}E_ManagerTopbarType;


// ��ť���뷽ʽ
typedef enum E_ManagerAlignmentType
{
	ALIGNMENT_LEFT = 1,				// �����
	ALIGNMENT_RIGHT,				// �Ҷ���
	ALIGNMENT_CENTER,				// ���ж���
	ALIGNMENTCount,

}E_ManagerAlignmentType;


// ÿ��������ǩ����Ϣ
typedef struct T_ManagerTopBar
{
	T_ManagerTopBar()
	{
		m_iBtnType			= MANAGERBARCount;
		m_iAlignmentType    = ALIGNMENT_LEFT;
		m_StrShowName		= L"";
		m_strMerchCode		= L"";
		m_iBtnID			= -1;
		m_iMarketID			= -1;
		m_strCfmName		= L"";
		m_iShrinkSize		= 0;
		m_bSelected			= false;
		m_colNoarml			= RGB(0,0,0);
		m_colPress			= RGB(0,0,0);
		m_colHot			= RGB(0,0,0);
		m_colTextNoarml		= RGB(0,0,0);
		m_colTextPress		= RGB(0,0,0);
		m_colTextHot		= RGB(0,0,0);
		m_colTextframe		= RGB(0,0,0);
	}

	//
	CString			m_StrShowName;		// ��ʾ����
	int32			m_iBtnID;			// ��ť��ID
	int32			m_iBtnType;			// ��������
	int32			m_iMarketID;		// �г�ID
	CString         m_strMerchCode;		// ��Ʒ����
	CString			m_strCfmName;		// ��������
	int32			m_iAlignmentType;	// ���з�ʽ
	int32			m_iShrinkSize;		// �ڲ���С��������
	bool			m_bSelected;		// �Ƿ�Ĭ��ѡ��
	
	COLORREF		m_colNoarml;
	COLORREF		m_colPress;
	COLORREF		m_colHot;
	COLORREF		m_colTextNoarml;
	COLORREF		m_colTextPress;
	COLORREF		m_colTextHot;
	COLORREF		m_colTextframe;
}T_ManagerTopBar;



// ÿ��������ǩ����Ϣ
class CManagerTopBarInfo
{
public:
	CManagerTopBarInfo()
	{
		m_iBarHight			= 40;
		m_iTopBarBtnWidth	= 80;
		m_iTopBarType       = 0;
		m_pImageBk			= NULL;
		m_strImagePath		= _T("");
		m_colBarBKColor		= RGB(42,42,50);
		m_mapTopbarGroup.clear();
		m_mapTopbarBtnLeft.clear();
		m_mapTopbarBtnCenter.clear();
		m_mapTopbarBtnRight.clear();
	}
	~CManagerTopBarInfo()
	{
		DEL(m_pImageBk)
	}

	int32						m_iBarHight	;				// �ؼ��ĸ߶�
	COLORREF					m_colBarBKColor;			// �ؼ��ı���ɫ
	int32						m_iTopBarBtnWidth;			// ÿ����ť�Ŀ��
	Image*						m_pImageBk;					// ��ť����ͼƬ			// ��ʱ���Ҳ�ʹ��
	CString						m_strImagePath;
	int32                       m_iTopBarType;				// ��ǩ������ 1-�������¹� 2-δ�����¹� 0-��������
	map<int32, T_ManagerTopBar>	m_mapTopbarGroup;			// ������ǩҳ��Ϣ����
	map<int32, CNCButton>		m_mapTopbarBtnLeft;			// ��ඥ����ǩ��ť
	map<int32, CNCButton>		m_mapTopbarBtnCenter;		// �в�������ǩ��ť
	map<int32, CNCButton>		m_mapTopbarBtnRight;		// �ұ߶�����ǩ��ť
};



//

class CIoViewManager : public CControlBase, public CGuiTabWndCB
{
	DECLARE_DYNAMIC(CIoViewManager)
public:
	enum E_ViewTab
	{
		EVTNone = 0,
		EVTLeft,
		EVTRight,
		EVTTop,
		EVTBottom
	};
	static const int32 KiChartBorderTop				= 0;
	static const int32 KiChartBorderLeft			= 0;
	static const int32 KiChartBorderRight			= 0;
	static const int32 KiChartBorderBottom			= 0;
// Construction
public:
	CIoViewManager();
	virtual ~CIoViewManager();

// from CControlBase
	virtual BOOL		TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL		TestChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL		TestKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
//
	void				OnTabSelChange();

public:
	bool32				BeTestValid();
	CString				ToXml();	
	bool32				FromXml(TiXmlElement *pElement);
	CString				ToXmlForWorkSpace(const char * KpcFileName);
	static const char * GetXmlElementValue();
	static CString		GetDefaultXML(int32 iGroupID,bool32 bBlank, CIoViewBase *pIoView = NULL);
	static const T_IoViewObject* GetIoViewObject();
	CIoViewBase*		GetActiveIoView();
	void				OnIoViewManagerActive();
	void				OnIoViewManagerDeactive();

	CIoViewBase*		FindIoViewByPicMenu(UINT nID, bool32 bCreateWhenNotExist=false);	// ����manager�ж�Ӧid��ioview����������ڲ����ڵ�����´���һ��

	int32				GetGroupID();
	void				SetGroupID(int32 iID);
	void				OnViewGroupIdChanged(int32 iNewGroupId);	// ÿһ��ʵ��~~��ֻ��ǿ����
	void				ChangeGroupId(int32 iNewId, bool32 bChangeMerch = false);	// ��������ǰ�Ĺ��ܳ�ͻ, �ı���ͼ���鲢�л���Ʒ
	
	void				OnRButtonDown2(CPoint pt, int32 iTab);
	CString				GetViewTabString(E_ViewTab eViewTab);
	E_ViewTab			GetViewTab(const char *StrViewTab);	
	void				SetTabAllign(E_ViewTab eViewTab);

	void				DealTabKey();
	void				DealF5Key();

	void				OnSizeChange(const CRect& rect);

	void				RegisterActiveIoViewToMainFrame(bool32 bRegister);			// ֪ͨmainframe���ڼ���/ע����ioview���ĸ�������TBWnd����ioView��ʾ

	static CString						GetIoViewString(CIoViewBase *pIoView);
	static CString						GetIoViewString(UINT uID);
	static E_IoViewGroupType			GetIoViewGroupType(const CIoViewBase* pIoView);
	static CString						GetIoViewGroupName(const CIoViewBase* pIoView);
	static const T_IoViewObject*		FindIoViewObjectByXmlName(const CString &StrXmlName);
	static const T_IoViewObject*		FindIoViewObjectByIoViewPtr(const CIoViewBase *pIoView);
	static const T_IoViewObject*		FindIoViewObjectByRuntimeClass(CRuntimeClass * pRunTimeClass);
	static const T_IoViewObject*		FindIoViewObjectByPicMenuID(UINT nMenuID);
	static const T_IoViewObject*		GetIoViewObject(int32 iIndex);
	static const int32					GetIoViewObjectCount();
	static void							GetIoViewIDs(IN E_IoViewGroupType eGroupType,OUT CArray<int32,int32>& aID);

	static const T_IoViewGroupObject*	FindIoViewGroupObjectByLongName(const CString& StrLongName);
	static const T_IoViewGroupObject*	GetIoViewGroupObject(int32 iIndex);
	static const int32					GetIoViewGroupObjectCount();
	void				DrawNoChartRgn();
public:
	// ȡ��־
	bool32			   GetBeyondShowFlag() const { return m_bBeyondShow; }

	// ǿ����ʾ
	void			   ForceBeyondShowIoView(CIoViewBase* pIoView = NULL);

	//	ǿ��ѡ�񶥲���Ʒ��ť
	void				ForceSelectManagerTop(const CMerch* pMerch);
private:
	void			   BeyondShowIoView(bool bBeyond, CIoViewBase* pIoView, bool32 bForceShow = false);

	// ��ȡ������ǩҳ��غ���
	bool32	FromXmlTopBtn( TiXmlElement *pTiXmlElement );
	void	InitTopBtn();
	void	ManagerTopBarSizeChange(const CRect& rect);		// ���¶�����ť����
	void	DrawManagerTopBar(CPaintDC &dc);
	void	SetCheckStatus( int iID );
	int		TButtonHitTest(CPoint point);
	void	RestTopBtnStatus(int32 ibtnControlId);			// ȥ���Ҳఴť�ĵ��״̬

public:
	void			   RemoveAllIoView();
	bool			   GetManagerTopbarStatus() { return m_cManagerTopBarInfo.m_mapTopbarGroup.size() > 0;} 		// ��ȡ������������ʾ��״̬

public:
	const CManagerTopBarInfo		&GetManagerTopBarInfo(){ return m_cManagerTopBarInfo; }

public:
	CArray<CIoViewBase*, CIoViewBase*> m_IoViewsPtr;

public:
//protected:
//
	E_ViewTab		m_eViewTab;
	bool32			m_bAutoHide;					// ������һ����ǩҳʱ�� �Զ�����
	CGuiTabWnd		m_GuiTabWnd;
	CRect			m_rcGuiTabWnd;

private:
	static CRuntimeClass	*m_pDefaultIoViewClass;	// �½�������Ĭ����ʾ��ҵ����ͼ

private:
	int32			m_iGroupId;						// ��IoViewManagerΪ��λ��������ţ� ���������ҵ����ͼͳһΪ�ñ�ţ� <=0��ʾ������ͼ

	bool32			m_bFromXmlNeedInit;				// ��XML�г�ʼ�����ȴ���ʼ��

	bool32			m_bIsShowManagerTopbar;			// �Ƿ���ʾ����������
	bool32			m_bIsHasTopBarByLayout;			// ����Ƿ��ж�����ǩ��
	bool32			m_bIsHasBottomBarByLayout;		// ����Ƿ��еײ���ǩ��

	
	CManagerTopBarInfo			m_cManagerTopBarInfo;						// ������ť��Ϣ
	CRect						m_rcManagerTopBtn;							// ������ť����
	int32						m_LastPressTopBtnId;							// ����������İ�ť���������Ҳఴť

	
	static CIoViewManager*	m_pThis;

private:
	bool32			m_bBeyondShow;				// ������ͼ��ռ�ֵ���ͼ��ʾ

	// ...fangz0111 ��ֵ�����, û�ո���, �Ȳ���һ��
	bool32			m_bDelIoView;

	bool32			m_bDrawCorner;		// �Ƿ񻭱߽�

	bool32          m_bNeedFixWidth;    // Tab�Ƿ�̶�ÿһ��Ŀ��
	int32           m_iTabFixWidth;     // Tab�̶�ÿһ��Ŀ��
	COLORREF	m_clrTabBk;				// tab��������ɫ
	COLORREF	m_clrTabBkNor;			// tab����������ɫ
	COLORREF	m_clrTabBkSel;			// tabѡ�б�����ɫ
	COLORREF	m_clrTabTextNor;		// tab����������ɫ
	COLORREF	m_clrTabTextSel;		// tabѡ��������ɫ
	COLORREF	m_clrTabSpliter;		// tab����ͼ��ָ�����ɫ

///////////////////////////////////////////////
	//////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewManager)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewManager)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nID);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnIoViewTitleButtonLButtonDown(WPARAM wParam,LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_MANAGER_H_
