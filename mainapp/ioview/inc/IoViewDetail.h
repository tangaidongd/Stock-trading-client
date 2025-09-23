#ifndef _H_IOVIEW_DETAIL_
#define _H_IOVIEW_DETAIL_

#include "XScrollBar.h"
#include "IoViewBase.h"
#include "GridCtrlSys.h"
#include "TrendIndex.h"
#include "SaneIndex.h"

extern const int32 KiStatisticaKLineNums;
/////////////////////////////////////////////////////////////////////////////
// CIoViewDetail
 
#define	KLINE_NUMS		(int32)3														// �����˼��� K ��

typedef struct T_IoViewDetailCell														// ÿһ�� K �߶�Ӧ��������Ϣ
{
	T_IoViewDetailCell()
	{
		m_iTimeUserMultipleDays			= 10;
		m_iTimeUserMultipleMinutes		= 10;

		m_iSaneStaticsNums				= 0;

		m_bTimeToUpdateLatestValues		= false;
		
		m_bNeedCalcSaneLatestValues		= true;

		m_aKLines.RemoveAll();		
		
		m_pIndexMaForSane				= NULL;
		m_aSaneIndexValues.RemoveAll();	
		m_aMerchsHaveCalcLatestValues.RemoveAll();

		m_eNodeInterval					= ENTIMinute;
		m_eHoldState					= EHSNONE;
		m_eActionPrompt					= EAPNONE;
		
		m_TimeToUpdate					= 0;	

		m_iTradeTimes					= 0;
		m_fAccuracyRate					= 0.0;
		m_fProfitability				= 0.0;

		m_RectShow						= CRect(-1, -1, -1, -1);
		m_bCalcing						= false;

	}

	//
	~T_IoViewDetailCell()
	{
		DEL(m_pIndexMaForSane);
	}
	
	//
	int32											m_iTimeUserMultipleMinutes;			// �Զ��������
	int32											m_iTimeUserMultipleDays;			// �Զ�������
	
	int32											m_iSaneStaticsNums;					// �Ƚ�ͳ�Ƹ���

	bool32											m_bNeedCalcSaneLatestValues;		// �Ƿ�Ҫ��������ָ��ֵ	

	bool32											m_bTimeToUpdateLatestValues;		// �Ƿ��˸������K �ߵ�ָ��ֵ��ʱ��(��������������Щ����.�� 10 �����������Ӧ)	
	CGmtTime										m_TimeToUpdate;						// ����ʵʱ���ݵ�ʱ���	
	CGmtTime										m_TimeToUpdateSane;					
	
	CArray<CKLine, CKLine>							m_aKLines;							// �����ڵ�K ������
	CArray<CMerch*, CMerch*>						m_aMerchsHaveCalcLatestValues;		// �Ѿ��������ֵ����Ʒ,��ֹ�����л���Ʒ��ʱ���ظ���μ�������ֵ.
	
	T_IndexOutArray*								m_pIndexMaForSane;					// GGTEMA ָ������
	CArray<T_SaneIndexNode,T_SaneIndexNode>			m_aSaneIndexValues;					// �Ƚ�ָ���ֵ

	E_NodeTimeInterval								m_eNodeInterval;					// K�ߵ�����
	E_HoldState									    m_eHoldState;						// ��ǰ�ֲ�״̬
	E_ActionPrompt									m_eActionPrompt;					// ��ǰ������ʾ

	int32											m_iTradeTimes;						// ���״���
	float											m_fAccuracyRate;					// ׼ȷ��
	float											m_fProfitability;					// ������

	CRect											m_RectShow;							// ǰ��ĸ�ѡ��
	bool32											m_bCalcing;							// �������ƶ��̵߳�״��?? ������û����,�ֽ�����???

}T_IoViewDetailCell;
 
class CIoViewDetail : public CIoViewBase
{
// Construction
public:
	CIoViewDetail();
	virtual ~CIoViewDetail();

	DECLARE_DYNCREATE(CIoViewDetail)
	
	// from CControlBase
public:
	virtual BOOL		TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	// from CIoViewBase
public:
	virtual void		RequestViewData();	
	virtual void		SetChildFrameTitle();
	virtual bool32		FromXml(TiXmlElement *pElement);
	virtual CString		ToXml();
	virtual	CString		GetDefaultXML();
	
	virtual void		OnIoViewActive();
	virtual void		OnIoViewDeactive();
	virtual bool32		GetStdMenuEnable(MSG* pMsg) { return false; }
	
	virtual void		OnWeightTypeChange();
	virtual void		LockRedraw();
	virtual void		UnLockRedraw();
	// from CIoViewBase
public:
	virtual void		OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void		OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void		OnVDataForceUpdate();
	
	virtual void		OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType);
	virtual	void		OnVDataMerchKLineUpdate(IN CMerch *pMerch);

	virtual void		OnIoViewFontChanged();
	virtual void		OnIoViewColorChanged();

public:
	void				OnUserCircleChanged(E_NodeTimeInterval eNodeInterval, int32 iValue);

private:
	
	void				RequestWeightData();
	bool32				SetTimeInterval( E_NodeTimeInterval eNodeInterval);
	void				SetViewDetailString();
	void				SetViewDetailString(CDC &dc);
	
	bool32				CreateTable();		
	void				DealWithSpecial(const T_SaneIndexNode& Node, const CString& StrTime, const CString& StrPrice);
	void				UpdateOneRow(int32 iUpdateFlag, int32 iRow, int32 iDataIndex);
	void				UpdateTableContent(int32 iUpdateFlag);
	void				SetRowHeightAccordingFont();							

	//
	void				SetUpdateTime(int32 iIndex);									// ���ø���ʱ���
	void				OnTimerUpdateLatestValuse(UINT uID);							// ����ʵʱָ��ֵ		
	
	//
	int32				CompareKLinesChange(const CArray<CKLine,CKLine>& KLineBef, const CArray<CKLine,CKLine>& KLineAft);		// �ж�����K �ߵ���ͬ. 0:��ͬ 1:����һ���仯 2:������һ�� 3:�����������,�д�ı仯
////////////////////////////////////////////////////////////
private:
	CGridCtrlSys									m_GridCtrl;
	CXScrollBar										m_XSBVert;
	CXScrollBar										m_XSBHorz;

	int32											m_iRowHeight;						// �и�	
	CRect											m_RectText;							// ��������
	CRect											m_RectGrid;							// �������

	int32											m_iCurShow;							// ��ǰ��ʾ��������
	CTime											m_TimePre;							// ����һ�еĽڵ��ӦK ��ʱ��
	
	T_IoViewDetailCell								m_aIoViewDetailCells[KLINE_NUMS];	// ָ��,K�� ����Ϣ����

	void											OnConditionOpenClose();				// ����һЩֵ

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewDetail)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewDetail)
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnMessageTitleButton(WPARAM wParam,LPARAM lParam);
	afx_msg void OnMenu(UINT uID);
	afx_msg void OnIndexMenu(UINT uID);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.




#endif  //_H_IOVIEW_DETAIL_