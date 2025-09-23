// ChartRegion.h: 
//
//////////////////////////////////////////////////////////////////////

#ifndef _CHARRT_REGION_H_
#define _CHARRT_REGION_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SelfDraw.h"
#include "ChartCurve.h"
#include "GmtTime.h"
#include "XTipWnd.h"
#include "Region.h"
#include "chartstruct.h"
#include "IRegionData.h"
#include "chartexport.h"
#include "NCButton.h"
/*
ChartRegion�ǿյ�,�趨Curve��ִ�л�.
һ��ChartRegion���԰���һ������Curve
��һ��Curve������һ��ChartRegion,Curve����ʱָ��ChartRegion,�����ڼ䲻����ı�.

Curve�ǿյ�,�趨NodeSequence�Ż�,�������趨һ��NodeSequence
һ��NodeSequence�������ڶ��Curve,�����ڼ�Curve�ĵ�ǰNodeSequence���Ըı�.
*/

class CChartCurve;
class CChartRegionViewParam;
class CChartRegion;
class CIoViewChart;

const float KHZoomFactor		= 0.75f;	// ÿ�����ŵı���
const int32 KNodeNumberMin		= 5;		// ��ʾNodeData����С��Ŀ
const int32 KNodeNumberMax		= 10000;	// ��ʾNodeData�������Ŀ

const int32 KSensitiveWidth		= 3;		// �϶�ʱ�����ı�Ե�����г̶�

#define REGIONTITLEADDBUTTONID		-5		// ���Ӹ�ͼ
#define REGIONTITLEDELBUTTONID		-6		// ɾ����ͼ

#define REGIONTITLELEFTBUTTONID		-7		// ǰһ��ָ��	
#define REGIONTITLERIGHTBUTTONID	-8		// ��һ��ָ��

#define REGIONTITLEFOMULARBUTTONID	-9		// ѡ��ָ��

extern CHART_EXPORT const char* KIoViewChartSelfDrawRgn;
extern CHART_EXPORT const char* KIoViewChartSelfDrawRgnID;
extern CHART_EXPORT const char* KIoViewChartSelfDrawNum;

class CHART_EXPORT CChartRegionViewParam : public CRegionViewParam
{
public:
	CChartRegionViewParam ( IN CWnd* pWnd);
	virtual ~CChartRegionViewParam();

public:
	//Curve��ɾ��,���������Ƶ�Node��ɾ��,Ӧ�ò��б�Ҫ���Node��Դ����.
	virtual void OnNodesRelease ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes ) = 0;
	// ѡȡ��chart
	virtual void OnPickChart(CChartRegion *pRegion, int32 x, int32 y, int32 iFlag){};
	//ʰȡ����NodeData
	virtual void OnPickNode( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes, CNodeData* pNodeData,int32 x,int32 y,int32 iFlag )= 0;
	//�Ҽ������Region(���pCurve��Ϊ��,������Ҽ������pCurve/pNodes/pNodeData)
	virtual void OnRegionMenu ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes,CNodeData* pNodeData, int32 x, int32 y )	= 0;
	virtual void OnRegionMenu2 ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y )	= 0;
	virtual void OnRegionIndexMenu ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y )	= 0;
	//ɾ��Curve
	virtual void OnDropCurve ( CChartRegion* pSrcRegion,CChartCurve* pSrcCurve,CChartRegion* pDestChart) = 0;
	virtual void OnRegionCurvesNumChanged ( CChartRegion* pRegion, int32 iNum ) = 0;
	//ʮ�ֹ�������
	virtual void OnCrossData ( CChartRegion* pRegion,int32 iPos,CNodeData& NodeData, float fPricePrevClose, float fYValue, bool32 bShow) = 0;
	// û�����ݵ�ʱ��ֻ��ʾһ��ʱ��
	virtual void OnCrossNoData (CString StrTime,bool32 bShow = true) = 0;	
	// ˫����֪ͨ��Ӧ
	virtual void OnChartDBClick(CPoint ptClick, int32 iNodePos) {}
	// �����ָ����صİ�ť���������޸Ĳ�����ɾ����
	virtual void OnRegionIndexBtn (CChartRegion* pRegion, CDrawingCurve* pCurve, int32 iID )	= 0;
	
	virtual IChartBrige  GetRegionParentIoView() = 0;
	virtual bool32 OnCanPanLeft ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum ){return true;}
	virtual bool32 OnCanPanRight ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum ){return true;}
	
	virtual CString OnTime2String ( CGmtTime& Time ) = 0;
	virtual CString OnFloat2String ( float fValue, bool32 bZeroAsHLine = true, bool32 bNeedTerminate = false ) = 0;

	virtual void	OnCalcXAxis ( CChartRegion* pRegion, CDC* pDC, CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide) = 0;
	virtual void	OnCalcYAxis ( CChartRegion* pRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide) = 0;

	virtual CString OnGetChartGuid ( CChartRegion* pRegion ) = 0;
	virtual void	OnSliderId ( int32& id, int32 iJump ) {}

	// ��ȡ��ĳ������ֵ,�䵱�ߵĺ�׺����
	virtual bool32  OnGetCurveTitlePostfixString(CChartCurve *pCurve, CPoint ptClient, bool32 bHideCross, OUT CString &StrPostfix){ return false; }
	
	virtual bool32 GetTips(IN CPoint pt, OUT CString& StrTips, OUT CString &StrTitle) { return false; }
public:
	CNodeSequence*	CreateNodes ( );
	void			RemoveNodes ( CNodeSequence* pNodes );
	int32			GetNodesNum();
	CNodeSequence*	GetNodes(int32 iPos);

public:
	CChartCurve*	m_pDragCurve;
	CSelfDrawCurve*	m_pDragCurveSD;
	CChartRegion*	m_pDragRegion;
	CPoint			m_PointPress;
	CPoint			m_PointRectZoomOutEnd;
	bool32			m_bPressXAxis;
	bool32			m_bRefreshLast;//���Ҳ�ʵʱ����.

public:
	CArray<CNodeSequence*,CNodeSequence*>	m_Nodes;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum E_NodeReqType
{
	ENRTInit,
	ENRTFrontCount,// ��������, ��ʱ�����ķ���
	ENRTCount// ˳������, ��ʱ����µķ���
} E_NodeReqType;
//ChartRegion
/*
Լ��˵��:
(1)һ��CChartRegion(KChart)�Ķ��ChartCurveֻ�ܹ���һ��x��
(2)һ��CChartRegion(KChart)�����Լ���OnKeyDown,OnMouseMove,������CChartRegion�޹�.
(3)һ��CChartRegion��������һ��ChartCurve,���ܴ���ʮ�ֹ��/����/����/��������.
(��)����˵��:
(1)
(2)
(3)
*/

const int32 KMsgInactiveCross = 5;
class CHART_EXPORT CChartRegion  : public CRegion
{
public: 
	enum E_DrawCross 
	{
		EDCMouseMove = 0,
		EDCUpDownKey,
		EDCLeftRightKey,
	};

	enum E_ChartRegionFlag
	{
		KChart					= 0x00000001,			// Chart��־
		KXAxis					= 0x00000002,			// x���־
		KYAxis					= 0x00000004,			// y���־
		KUserChart				= 0x00000008,			// UserChart��־
		
		KPanAble				= 0x00000010,			// ����ƽ��(K�߿���ƽ��,��ʱ����ƽ��)
		KHScaleAble				= 0x00000020,			// ����ˮƽ����(�����Ҳ�ʱ��Ϊ�̶���)
		KCrossAble				= 0x00000040,			// ������ʾʮ�ֹ��
		KDragXAxisPanAble		= 0x00000080,
		
		KYDependent				= 0x00000100,			// ��Yֵ������Region( һ���� YRegion)
		KYAxisSpecifyTextColor	= 0x00000200,
		KLockMouseMove			= 0x00000400,			// �ڲ���־,��ʱ��ֹOnMouseMove
		KFixShowYValue			= 0x00000800,			// ��ͼY������̶���ʾĳ��ֵ(����MACD ��0 �������)

		KActiveFlag				= 0x00001000,			// �����ڼ����ʱ��, ���region �Ƿ񻭼����־
		KDonotShowCurveDataText = 0x00002000,			// ��Ҫ��ʾ��region�����ߵ����ݼ�VOLFS L0:123 L0:123���ֲ���ʾ
	};

public:
	CChartRegion( IN CRegionViewParam* pViewParam, CChartRegion*& pRegion,CString StrName, uint32 iFlag, CNodeSequence* pDefaultNodes = NULL );
	void    Destruct();
	virtual ~CChartRegion();

public:
	static const T_FuctionButton*	GetRegionButton(int32 iIndex);		// �õ�Region��ť
	static const int32	GetRegionButtonNums();							// �õ�Region��ť����

	// ��ӿ�
public:
	//////////////////////////////////////////////////////////////////////////////
	virtual bool32	OnKeyDown(int32 nChar, int32 nRepCnt, int32 nFlags);//???
	virtual	void	OnMouseMove(int32 x, int32 y); //����ƶ��л���ʮ�ֹ��
	virtual void	OnMousePress(int32 x, int32 y, int32 iPass, bool bRightButton = false);
	virtual void	OnMousePressUp(int32 x, int32 y);
	virtual void	OnMouseDBClick(int32 x, int32 y);//������˫��
	virtual void	OnMouseRClick (int32 x, int32 y);
	virtual void	OnMouseDragMove(int32 x, int32 y, bool bRightButton = false);//�϶�
	virtual void	OnMouseDragDrop(int32 x, int32 y, bool bRightButton = false);//����ɿ�
	
	virtual void	OnSelfDrawPress(int32 x,int32 y);
	virtual void	OnSelfDrawDrag(int32 x,int32 y);
	virtual void	OnSelfDrawRelease(int32 x,int32 y);
	
	virtual void	OnSelfDrawMove(int32 x,int32 y);
	virtual void	OnSelfDrawEnd(int32 x,int32 y);
	
	virtual void	OnPreDraw(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC);
	virtual void	OnDraw(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC);
	virtual void	OnAfterDraw(IN CMemDCEx* pOverlayDC);
	virtual void	OnActive(bool32 bActive);
	virtual void	OnMessage(int32 mid, void* pData);
	virtual void	OnMouseOverAWhile(int32 x, int32 y);//��x,y��ͣ����һ��ʱ��,���ڱ���:ʰȡ����ʾtooltips.Ӧ�ò�ʵ��
	virtual void	OnMouseOverLeave(int32 x, int32 y);//Ӧ�ò�ʵ��
	virtual void	OnKeyDel();//Ӧ�ò�ʵ��,��ʲô?
	virtual void	OnDrawAxisSliderText(IN CMemDCEx* pDC,CRect Rect,const CPoint pt);//Ӧ�ò�ʵ��
	virtual CChartCurve* CurveNew(int32 iFlag);

public:
	CRegionViewParam*	GetViewParam()								{ return m_pViewParam; }
	CChartRegionViewParam*  GetViewData()							{ return (CChartRegionViewParam*)m_pViewParam; }
	uint32			GetFlag()										{ return m_iFlag; }

	void			SetTitle(const LPCTSTR StrTitle)				{ m_StrTitle = StrTitle; }			// ���õ�ǰChartRegion�ı���,����:�ַ�����,��KChart��Ч
	CString			GetTitle()										{ return m_StrTitle; }
	void			ActiveCross(bool32 bActive)						{ m_bActiveCross = bActive; }		// ������ʾ(������)ʮ�ֹ��,��KChart��Ч
	bool32			IsActiveCross()									{ return m_bActiveCross; }

	bool32			AddYDependentRegion(CChartRegion* pRegion);
	bool32			RemoveYDependentRegion(CChartRegion* pRegion);

	void			SetDataSourceRegion(CChartRegion* pRegion)		{ m_pDataSourceRegion = pRegion; }
	CChartRegion*	GetDataSourceRegion ( )							{ return m_pDataSourceRegion; }

	CChartCurve*	GetDependentCurve();							// ����KChart,����������������,����Axis,���ڼ���̶ȵ�.(ʵ��ʹ��Curve�ڲ���Nodes),
	CChartCurve*	CreateCurve(uint32 iFlag);
	bool32			RemoveCurve(CChartCurve* pCurve);
	int32			GetCurveNum()									{ return m_Curves.GetSize(); }
	CChartCurve*	GetCurve(int32 iPos);

	CPoint			GetPtCross() const								{ return m_ptCross; }

public:
	void			SetLeftSkip(int32 iSkip)						{ m_iLeftSkip = iSkip;}
	void			SetRightSkip(int32 iSkip)						{ m_iRightSkip = iSkip;}
	void			SetTopSkip(int32 iSkip)							{ m_iTopSkip = iSkip; }
	void			SetBottomSkip(int32 iSkip)						{ m_iBottomSkip = iSkip; }
	
	int32			GetLeftSkip()									{ return m_iLeftSkip; }
	int32			GetRightSkip()									{ return m_iRightSkip; }
	int32			GetTopSkip()									{ return m_iTopSkip; }
	int32			GetBottomSkip()									{ return m_iBottomSkip; }

	CRect			GetRectCurves();
	void			ClipY(int32& y);
public:
	void			SetTitleTextColor(COLORREF clr)					{ m_clrTitle = clr; }
	void			SetTitleTextFont(CFont* pFont)					{ m_FontTitle = pFont; }
	void			SetAxisTextColor(COLORREF clr)					{ m_clrAxisText = clr; }
	void			SetAxisTextFont(CFont* pFont)					{ m_FontAxisText = pFont; }
	void			SetAxisRuleColor(COLORREF clr)					{ m_clrAxisRule = clr; }
	void			SetAxisSliderBorderColor(COLORREF clr)			{ m_clrSliderBorder = clr; }
	void			SetAxisSliderFillColor(COLORREF clr)			{ m_clrSliderFill = clr; }
	void			SetAxisSliderTextColor(COLORREF clr)			{ m_clrSliderText = clr; }
	void			SetAxisSliderTextFont(CFont* pFont)				{ m_FontSliderText = pFont; }
	void			SetCrossColor(COLORREF clr)						{ m_clrCross = clr; }

	COLORREF		GetTitleTextColor()								{ return m_clrTitle; }
	CFont*			GetTitleTextFont()								{ return m_FontTitle; }
	COLORREF		GetAxisTextColor()								{ return m_clrAxisText; }	
	CFont*			GetAxisTextFont()								{ return m_FontAxisText; }
	COLORREF		GetAxisRuleColor()								{ return m_clrAxisRule; }
	COLORREF		GetAxisSliderBorderColor()						{ return m_clrSliderBorder; }
	COLORREF		GetAxisSliderFillColor()						{ return m_clrSliderFill; }
	COLORREF		GetAxisSliderTextColor()						{ return m_clrSliderText; }
	CFont*			GetAxisSliderTextFont()							{ return m_FontSliderText; }
	COLORREF		GetCrossColor()									{ return m_clrCross; }

public:
	bool32			RegionToSD(int32 x1,int32 y1, int32& x2, int32& y2);
	bool32			RegionToSD(int32 x,int32 y, int32& id,float& fVal);
	bool32			SDToRegion(int32 id, float fVal, int32& x,int32& y);
	bool32			IsSDInRegion(CSelfDrawNode* pSelfDrawNode);
	bool32			MoveSDId(int32 x1,int32 y1,int32 x2,int32 y2,int32& id1,int32& id2);
	bool32			MoveSDY(int32 x1,int32 y1,int32 x2,int32 y2,float& fVal1,float& fVal2);

	bool32			ExtendSDIdToRegionX(int32 id, int32 &x);
	bool32			ExtendRegionXToSDId(int32 x, int32 id);

public:
	static bool32	SetSelfDrawType(E_SelfDrawType eDrawType, IChartRegionData* pChart);

	//////////////////////////////////////////////////////////////////////////
	bool32			bNoData();
	void			DrawIndexValueByMouseMove(int32 x,int32 y,bool32 bHideCross = false);

private:
	void			OnDependentChanged(CChartCurve* pCurve);

	

// 	bool32			HPanLeft();
// 	bool32			HPanRight();
// 
// 	int32			HPanLeft(int32 iDragNum);
// 	int32			HPanRight(int32 iDragNum);

	bool32			FlyCross(int32 iCross, bool32 bLeft);
//	void			DrawTitle(CMemDCEx* pDC,CMemDCEx* pPickDC);
	void			DrawTitle ( CMemDCEx* pDC,CMemDCEx* pPickDC,bool32 bMouseMove = false);
	void			DrawRound(CMemDCEx* pDC);
	void			DrawSaneIndexTitle(CMemDCEx* pDC,int32 iPos,bool32 bMouseMove = false);
	bool32			DrawTitleText(CMemDCEx* pDC,CMemDCEx* pPickDC, CChartCurve* pCurve,CString StrTitle, CRect& Rect, bool32 bAppendSpaceSep=true);
	void			DrawAxisSlider(CMemDCEx* pDC,const CPoint pt);
	void			DrawCross (CMemDCEx* pDC,int32 x, int32 y,E_DrawCross eDrawCross);
	void			DrawCross (CMemDCEx* pDC,int32 iXNodePos, E_DrawCross eDrawCross);
	void			DrawRectZoomOut(CMemDCEx* pDC);
	void            DrawRightDragRect(CMemDCEx* pDC);
	void			DrawVLine (CMemDCEx* pDC, int32 x);

public:
	void			DrawHVLine(CMemDCEx* pDC,int32 iVal, bool32 bHorizontal = true);
	void			DrawAxisSliderRect(CMemDCEx* pDC,CRect Rect);
	void			DrawSelfDrawing(CMemDCEx* pDC,CMemDCEx* pPickDC);
	bool32			PickNode(int32 x,int32 y, OUT CDrawingCurve*& pCurve, OUT CNodeSequence*& pNodes, OUT CNodeData& NodeData,bool32 bClearLast = true);
	void			SetPickFlag(CChartCurve* pCurve, CNodeData& NodeData);
	void			SetPickFlag(CSelfDrawCurve* pCurve);
	void			ClipSelfDrawCursor();

	//
	static E_SelfDrawType GetSelfDrawValue()	{ return m_eSelfDrawType; }
	void			ClearSelfDrawValue();

	CString			GetSelfDrawXmlString(int32 iIndex);

	IChartBrige	GetParentIoViewData();	
	void			SetGGTongESCFlag();				// ����GGTongView ��ǰ�Ƿ���ʮ�ֹ��
	
public:
	CArray<CSelfDrawCurve*, CSelfDrawCurve*> m_SelfDrawingCurves;

public:
	CArray<CChartCurve*,CChartCurve*>	m_Curves;
	CArray<CChartRegion*,CChartRegion*>	m_YDependentRegions;
	CChartRegion*	m_pDataSourceRegion;

	CString			m_StrTitle;//��ǰChartRegion�ı���,����:MerchName
	bool32			m_bActiveCross;//�Ƿ񼤻�ʮ�ֹ��,KCrossAble��־����ʱ������.

public:
	int32			m_iNodeCross;			//��ǰʮ�ֹ�����ڵ�λ��
	bool32			m_bSplitWndDrag;		//�ָ����Ƿ����϶�(�ǵĻ�,���ػ���ʱ��,��Ҫ�������λ��)
	int32			m_iNodeVPos;			//��ǰ��ֱ�����ڵ�λ��

protected:
	CRect			m_RectCurves;
	int32			m_iLeftSkip;
	int32			m_iRightSkip;
	int32			m_iTopSkip;			// �����趨
	int32			m_iBottomSkip;		// �����趨
	CPoint			m_ptCross;
	
public:
	COLORREF		m_clrSliderText;//ʱ�����(��ֵ)���������ı�ɫ
	CFont*			m_FontSliderText;
	COLORREF		m_clrSliderBorder;//ʱ�����(��ֵ)�������ı߿�ɫ
	COLORREF		m_clrSliderFill;////ʱ�����(��ֵ)�����������ɫ
	COLORREF		m_clrTitle;//Region�������ɫ
	CFont*			m_FontTitle;
	COLORREF		m_clrAxisText;//��ʶʱ�����(��ֵ)���ı�ɫ
	CFont*			m_FontAxisText;
	COLORREF		m_clrAxisRule;//��ʶʱ�����(��ֵ)�Ŀ̶ȵ���ɫ
	COLORREF		m_clrCross;

protected:
	//�Ի��ߵı���
	CPoint			m_PtPress;
	CPoint			m_PtRelease;
	CPoint			m_PtEnd;
	int32			m_iIdPress;
	int32			m_iIdRelease;
	int32			m_iIdEnd;
	float			m_fValPress;
	float			m_fValRelease;
	float			m_fValEnd;
	
	static E_SelfDrawType m_eSelfDrawType;
	static int32	m_iSelfDrawAppendOp;

public:
	// x��y�����
	CArray<CAxisNode, CAxisNode&>		m_aXAxisNodes;	// x��ÿ����ֲ���λ��
	CArray<CAxisDivide, CAxisDivide&>	m_aXAxisDivide;	// x��ָ���
	
	CArray<CAxisDivide, CAxisDivide&>	m_aYAxisDivide;	// y��ָ���
	
	// �϶�X �����
	int32			m_iJumpNodeID;	// �ʼ�϶�ʱ��λ���Ǹ�NodeID
	int32			m_iJumpNodePos; // �ʼ�϶�ʱ��λ���Ǹ�NodePos	
	int32			m_iPreNodeID;	// ��һ��NodeID,�ж��Ƿ��ظ�.
	
	Image          *m_pImgSet;

	// 
	int32			m_iAxisTextMaxWidth;
	int32			m_iAxisTextMaxHeight;

	CString			m_StrAxisTextsOfNoData;     // ������ʱ,X������鴰����ʾ��ʱ��ֵ

private:	
	int32			m_iZoomOutWishNums;		 
	int32			m_iMouseMoveTrendIndexPos;
	bool32			m_bAbleToRectZoomOut;
    DWORD           m_dwTickCnt;

private:
	CXTipWnd		m_TipWnd;
	static	int32	m_iPreNodePos; 

	void InitIndexBtn();
	void AddIndexBtn(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption);
	void DrawIndexBtn( CMemDCEx* pDC, int32 ID, CRect &rect);
	int32 TIndexButtonHitTest(CPoint point);
	std::map<int, CNCButton> m_mapIndexBtn;			// ָ����������ӵİ�ť
	int32 m_iIndexBtnHovering;

friend class CChartCurve;
friend class CIoViewChart;
friend class CIoViewKLine;
friend class CIoViewTrend;
friend class CIoViewTick;
friend class CIoViewExpTrend;
friend class CIoViewKLineArbitrage;
friend class CIoViewTrendArbitrage;
};

#endif // _CHARRT_REGION_H_

