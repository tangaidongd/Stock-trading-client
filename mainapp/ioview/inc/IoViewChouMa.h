#ifndef _IOVIEW_CHOUMA_H_
#define _IOVIEW_CHOUMA_H_

#include "IoViewBase.h"
#include "ChengBenFenBu.h"
#include "AxisYObject.h"

/////////////////////////////////////////////////////////////////////////////
// CIoViewChouMa

#define CHOUMA_BASE_KLINECOUNT (200)			// ��������200��k��
#define CHOUMA_MAX_KLINECOUNT  (600)			// ������n��k��

class CIoViewKLine;

struct T_ChouMaNotify			// ����֪ͨ�ṹ
{
	CIoViewKLine *pIoViewKLine;
	int32 iFlag;
	WPARAM wParam;
	LPARAM lParam;
};

// ��������200����k�����ݿ�
class CIoViewChouMa : public CIoViewBase
{
public:
	struct T_CBFBParam
	{
		float			m_fTradeRateRatio;			// �����ʷŴ�ϵ�� - ��ʷ����˥��ϵ��
		E_ChengBenFenBuSuanFa m_eChengBenFenBuSuanFa;	// �ɱ��ֲ��㷨 Ŀǰֻ��ƽ��
		float		m_fDefaultTradeRate;		// ����ͨ������ʱ��ȱʡ�Ļ�����
		UINT		m_uiDivide;					// �ֳɶ��ٷ�
		ChengBenFenBuCycleArray m_aCBFBRedYellowCycles;	// �ɱ��ֲ���Ƶ�����
		ChengBenFenBuCycleArray m_aCBFBBlueGCycles;		// �ɱ��ֲ���������
		
		T_CBFBParam();
	};
// Construction
public:
	CIoViewChouMa();
	virtual ~CIoViewChouMa();


	DECLARE_DYNCREATE(CIoViewChouMa)
	
	// from CControlBase
public:

	// from CIoViewBase
public:
	virtual void		RequestViewData();	
	virtual void		SetChildFrameTitle();
	virtual bool32		FromXml(TiXmlElement *pElement);
	virtual CString		ToXml();
	virtual	CString		GetDefaultXML();
	
	virtual void		OnIoViewActive();
	virtual void		OnIoViewDeactive();
	virtual bool32		GetStdMenuEnable(MSG* pMsg) { return true; }
	
	virtual void		OnWeightTypeChange();
	virtual void		LockRedraw();
	virtual void		UnLockRedraw();
	
public:
	// from CIoViewBase
	virtual void		OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void		OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void		OnVDataForceUpdate();

	virtual void		OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType);
	virtual	void		OnVDataMerchKLineUpdate(IN CMerch *pMerch);

	virtual void		OnIoViewFontChanged();
	virtual void		OnIoViewColorChanged();

public:

	static void	GetCBFBParam(OUT T_CBFBParam &param);
	static void SetCBFBParam(const T_CBFBParam &param);

	static void OnChouMaNotify(const T_ChouMaNotify &notify);		// ֪ͨ������k�������ͼ�������˱��

	void	OnCBFBParamChanged(); // ���ݲ��������

	void	SetLastFocusPrice(float fPrice);

	void	InitializeFirstKline();		// �����Լ�һ����ʼ����k����ͼ

	// K����ͼ���
	// ͬ��ͬ����k�߼���ʱ��������Ϊ���봰�ڵ��¼�����
	// �������һ�γ�ʼ��ʱ��Ѱ��ͬ����ͬ���k����ͼ��ִ�е�һ�γ�ʼ��
	// ������Ϣ��Ӧ����Ӧ��ǰ��ע��k����ͼ����Ϣ
	void	OnIoViewKLineActive(CIoViewKLine *pIoViewKline);		
	void	OnIoViewKLineYSizeChange(CIoViewKLine *pIoViewKline);	// K����ͼ��y���ֵ�����ܶȷ����˱仯, ��Ҫ���¼���??
	void	OnIoViewKLineMainMerchDataUpdate(CIoViewKLine *pIoViewKline);	// K����ͼ��K�����ݼ��㷢���˱仯, ���Ӧ��k����ͼ����ͬ����
	void	OnIoViewKLineXAxisMayChange(CIoViewKLine *pIoViewKline);
	void	OnKLineMouseMove(CIoViewKLine *pIoViewKline, float fPrice, UINT uTimeId);			// 


	int32	PriceYToClientY(float fPriceY);		// �����ụ��ת��, Ч�ʵ��˵�, ��ʱ����Ҫy���۸�ת
private:
	void	RecalcLayout();
	void	DrawMyText(CDC *pDC);

	// ���ƻ���ͼ��
	void	DrawBasicNodesChart(CDC &dc, const ChengBenFenBuNodeArray &aNodes, float fVolMin, float fVolMax, float fPriceClose, int32 iPriceHalfVol, int32 iPriceHalfVolX, bool32 bUseYellow = true);

	void	CalcShowData();	// ������ʾ����

	// ����ת�����������ת�����򷵻�Ĭ��ֵ
	int32	PriceToChouMaY(float fPrice, int32 iDefault = 0);
	//float   KlineYToPrice(int32 iKlineScreenY, float fDefault = 0.0f);

	static bool32	ReadParamFromFile(OUT T_CBFBParam &param);
	static void		SaveParamToFile(const T_CBFBParam &param);
	
////////////////////////////////////////////////////////////
private:
	
	CRect				m_RectTitle;		// ����
	CRect				m_RectChart;		// ͼ��

	CArray<CRect, const CRect &>  m_RectBtns;	// ��ť����, ��Ļ����
	int32				m_iCurBtn;				// ��״ ��� ���� (���ð�ť����ѡ��)
	CImageList			m_ImageList;

	float				m_fLastPrice;		// ����ע�ļ۸�

	CArray<CKLine, CKLine> m_KLines;		// K������

	ChengBenFenBuNodeMap m_mapNodes;

	void			   *m_pIoViewKlineFocus;	// ��ǰ��ע��k����ͼ, �п��ܸ�ָ��ָ���K���Ѿ����٣����Բ�Ӧ���ڽӿ�֪ͨ������κεط����ʸ�ָ��
	UINT				m_uTimeEnd;				// Ĭ����������ʱ�����������ʱ���ǰ200������������ж�ģ����Ǽ������800��
	CRect				m_RectKlineCurve;		// k����ͼ��ͼk�����ݴ�������
	float				m_fKlineMinY, m_fKlineMaxY;	// k����ͼ��С ���yֵ
	CPriceToAxisYObject m_AxisYCalcObject;		// Y�����

	bool32				m_bNeedCalc;		// �Ƿ���Ҫ���¼���

	static T_CBFBParam s_cbfbParam;		// ȫ�ֹ���һ������
	

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewChouMa)
	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewChouMa)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IOVIEW_CHOUMA_H_