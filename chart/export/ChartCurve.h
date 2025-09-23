#ifndef _CHART_CURVE_H_
#define _CHART_CURVE_H_
#include <float.h>
#include "typedef.h"
#include "ShareFun.h"
#include "TrendIndex.h"
#include "DrawingCurve.h"
#include "AxisYObject.h"
#include "TrendIndex.h"
#include "chartexport.h"
#include "ChartCurve.h"
class CChartRegion;
class CChartCurve;

//��type
typedef enum E_LineType
{
	ELTNormal = 0,
	ELTDrawLine,
	ELTDrawPolyLine,
	ELTDrawStickLine,
	ELTDrawIcon,
	ELTStringData,
	ELTLineRt
} E_LineType;

//������ֵ����
typedef enum E_AxisType
{
	EATUnknown = 0,		//δ֪
	EATDate,			//ʱ��(����)
	EATDateEx,			//�ֱʳɽ���X��(ʱ����)
	EATJe,				//���
	EATBfb,				//�ٷֱ�
	EATDs,				//����
} E_AxisType;
/////////////////////////////////////////////////////////////////////////////////////////////////////
const uint32 KDrawXAxisLeft		= 0x01;
const uint32 KDrawXAxisRight	= 0x02;
const uint32 KDrawXAxisCenter	= 0x04;
const uint32 KDrawXAxisVTop		= 0x08;



///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//Ҫ�������ݵ�˳������
//(1)����DrawingCurve�л��Ƶ����ݵ���Դ.
//(2)�����˳�������Լ��,�Ƿ�����Ժ���չ?
//(3)�����趨�û��Զ�������(����������ָ��)
class CHART_EXPORT CNodeSequence
{
public:
	CNodeSequence();
	~CNodeSequence();
	
public:
	bool32			Add(const CNodeData& NodeData);
	bool32			Set(int32 iPos, IN CNodeData& NodeData);
	int32			GetSize()								{ return m_aNodes.GetSize(); }
	bool32			GetAt(int32 iPos, CNodeData& NodeData);
	void			Copy(CNodeSequence* pSrc);
	bool32			Lookup(int32 iID, int32& iPos);
	bool32			LookupEx(int32 iID, int32& iPos);
	void			RemoveAt(int32 iPos);
	void			RemoveAll()								{ m_aNodes.SetSize(0); }
	void			SetUserData(void* pUserData)			{ m_pUserData = pUserData; }
	void*			GetUserData()							{ return m_pUserData; }
	void			SetName(const LPCTSTR StrName)			{ m_StrName = StrName; }
	void			SetWeightName(const LPCTSTR StrName)	{ m_StrWeightName = StrName; }
	CString			GetName()								{ return m_StrName;}
	CString			GetWeightName()							{ return m_StrWeightName; }
	void			SetNamePrefix(const LPCTSTR StrName)	{ m_StrNamePrefix = StrName; }
	CString			GetNamePrefix()							{ return m_StrNamePrefix; }
	void			SetNamePostfix(const LPCTSTR StrName)	{ m_StrNamePostfix = StrName; }	
	CString			GetNamePostfix()						{ return m_StrNamePostfix; }

	int32			AddRef(CDrawingCurve* pCurve);
	int32			Release(CDrawingCurve* pCurve);
	void			DumpAttatchedCurves(CArray<CDrawingCurve*, CDrawingCurve*>& Curves);

	// 
	CArray<CNodeData, CNodeData&>& GetNodes()					{ return m_aNodes; }	
	CArray<CDrawingCurve*, CDrawingCurve*>&	GetAttachedCurves() { return m_aAttatchedCurves; }	

private:
	int32			HalfSearch(int32 id);
	int32			HalfSearchEx(int32 id);
	bool32			HalfSearchEqu(int32 id,int32 t,int32 i, int32& iPos,int32& iSub);

public:
	CString			m_StrName;
	CString         m_StrWeightName;
	CString			m_StrNamePrefix;
	CString			m_StrNamePostfix;

	CArray<CNodeData, CNodeData&>	m_aNodes;			// ���������б�

private:
	void*			m_pUserData;
	CArray<CDrawingCurve*, CDrawingCurve*>	m_aAttatchedCurves;	

	//
friend class CRegionViewParam;
friend class CDrawingCurve;
friend class CIoViewKLine;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//Chart��
/*
(һ)��Ҫ˵��
��ɺܶ����Ĺ���:�����ⲿ��NodeData,���л���.�����ṩʰȡ/����ת���ȸ�������.
(��)����˵��(����)
(1)��������
(2)����NodeData
(3)CalcX/CalcY/CalcTransform
(4)Draw
����,�������趨���־������

*/
class CHART_EXPORT CNodeDrawerData : public CNodeData
{
public:
	CNodeDrawerData();
	CNodeDrawerData(IN const CNodeData& NodeData);
	~CNodeDrawerData();

public:
	void			CalcXPos(int32 &x1, int32 &x2, int32 &x3);

public:
	int32			m_iCurveLoop;
	int32			m_iValidNum;		// Nodes��������

	int32			m_iPos;				// ����Nodes��ǰλ��.
	int32			m_iX;				// ��ǰNode����Ļ��x����
	int32			m_iXLeft;
	int32			m_iXRight;

	// 
	int32			m_iOpen;
	int32			m_iClose;
	int32			m_iHigh;
	int32			m_iLow;
	int32			m_iAvg;

	int32			m_iAmount;
	int32			m_iVolumn;
	int32			m_iHold;
	
	bool32			m_bRise;			// ��ӦK �ߵ��ǵ�

};

class CHART_EXPORT CNodeDrawer
{
public:
	CNodeDrawer();
	virtual ~CNodeDrawer();

public:
	virtual void	DrawNodes(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, IN CChartCurve &ChartCurve, const CArray<CNodeDrawerData, CNodeDrawerData&> &aNodeDrawerDatas) = 0;
	
	int32			CalcNeedCalcNodesOverlapKLine(IN CNodeDrawerData *pNodeDrawerDatas, IN int32 iNodeCount, IN CChartCurve &ChartCurve, 
												  OUT CArray<CRect, CRect &>	&aNeedDrawRects, OUT CArray<CNodeDrawerData *, CNodeDrawerData *>& aNeedDrawNodeDrawerDatas);

	virtual bool32	BeDrawTitleText() { return true; }
	virtual bool32	BeNoDraw()		{ return false; }

protected:
	int32			AddRef ( );
	int32			Release ( );

protected:
	int32			m_iRef;

friend class CChartCurve;
};

class CHART_EXPORT CMinMax
{
public:
	CMinMax() 
	{
		m_bValid = false;
		m_fMin = 0.;
		m_fMax = 0.;
	}

public:
	bool32			m_bValid;
	float			m_fMin;
	float			m_fMax;
};

class CHART_EXPORT CChartCurve : public CDrawingCurve
{
public:
	enum E_ChartCurveFlag
	{
		KDependentCurve				= 0x00000001,		// ��(Dependent Curve)
		KRequestCurve				= 0x00000002,		// �������ݵ�Curve,ȱʡΪK��

		KInVisible					= 0x00000010,		// ����ʾ
		KYInheritTransform			= 0x00000020,		// ʹ��DependentCurve��Transform(translate��scale)
//		KSelect						= 0x00000040,		// ��ѡ��(�ڲ�ʹ��)
		KUseNodesNameAsTitle		= 0x00000080,
		
		KTypeKLine					= 0x00000100,		// ��ʾtooltips�ı�ʶ;
		KTypeIndex					= 0x00000200,		// ֻ��Index����,������Region���϶�.������ɾ��?
		KTypeTrend					= 0x00000400,		// ��ʾtooltips�ı�ʶ;Curve�������Ҳ�,ǿ�ƶ��뵽RectCurve;
		KTypeTick					= 0x00000800,		// ��ʾtooltips�ı�ʶ;

		KYTransformToAlignDependent	= 0x00001000,		// �Ա�(compare)��Ʒ��������.
		KIndex1						= 0x00002000,		// ��һ��Index��,��Curve����ʾindex����,����KDJ
		KindexCmp					= 0x00004000,		// ����ָ��ı�־

		KYTransformByLowHigh		= 0x00010000,		// Y�����Low/High��������
		KYTransformByClose			= 0x00020000,		// Y�����Close��������
		KYTransformByAmount			= 0x00040000,		// Y�����Amount(��)��������
		KYTransformByVolumn			= 0x00080000,		// Y�����Volumn(��)��������
		KYTransformByAvg			= 0x00100000,		// Y�����Avg(����)��������		

		KDonotPick					= 0x00200000,		// �ñ�־���߲��ܱ�pick xl 0214
		KTimeIdAlignToXAxis			= 0x00400000,		// ʱ�����x������־��ʹ�ô˱�־ʱ����ÿ�����x�������Ӧ��������ͬʱ���x��
	};

public:
	CChartCurve(CChartRegion &ChartRegion, uint32 iFlag);
	~CChartCurve();

	// from CDrawingCurve
public:
	virtual void	Draw(IN CMemDCEx* pDC, IN CMemDCEx* pPickDC) { NULL; }
	virtual bool32	CalcY(OUT float &fYMin, OUT float &fYMax);					// ��ֵ���ص���axisY��min, max
	virtual bool32	GetYMinMax(OUT float& fYMin, OUT float& fYMax);				// ͬ��
	virtual void	SetYMinMax(float fMin, float fMax, bool32 bValid = true);	// ͬ��

	virtual bool32	RegionXToCurvePos(int32 x, OUT int32& iPos);	// �˴��޸Ľ�Ӱ��Y�ᣬx�᲻��
	virtual bool32	CurvePosToRegionX(int32 iPos, OUT int32& x);	// ����Region�е�x����,����m_aNodes�ĵ�iPos��NodeData
	virtual bool32	RegionXToCurveID(int32 x, OUT int32& id);

	// ��ͼ�������
public:
	CChartRegion&	GetChartRegion()						{ return m_ChartRegion; }
	
	void			AttatchNodes(CNodeSequence* pNodes);
	void			DetatchNodes();
	CNodeSequence*	GetNodes()								{ return m_pNodes; }
	
	void			AttatchDrawer(CNodeDrawer* pDrawer);
	void			DetatchDrawer();
	CNodeDrawer*	GetDrawer()								{ return m_pDrawer; }

	void SetAxisYType(CPriceToAxisYObject::E_AxisYType eType);	// ������������
	CPriceToAxisYObject::E_AxisYType GetAxisYType() const { return m_eAxisYType; }

	void			SetUserPriceMinMaxBaseValue(float fMin=FLT_MAX, float fMax=-FLT_MAX){ m_fUserMin=fMin; m_fUserMax=fMax; };	// �����û��趨�������Сֵ

	// �������
public:
	void			SetGroupTitle(const LPCTSTR StrTitle)	{ m_StrGroupTitle = StrTitle; }
	CString			GetGroupTitle()							{ return m_StrGroupTitle; }
	void			SetTitle(const LPCTSTR StrTitle)		{ m_StrTitle = StrTitle; } 
	CString			GetTitle()								{ return m_StrTitle; }

	// 
public:
	void			SetDependent();
	
	bool32			IsSelect()								{ return CheckFlag(m_iFlag,CDrawingCurve::KSelect); }
	bool32			IsDependent()							{ return CheckFlag(m_iFlag,CChartCurve::KDependentCurve); }
	
	void			SetSelectNodeID(int32 iID)				{ m_iSelectNodeID = iID; }
	int32			GetSelectNodeID()						{ return m_iSelectNodeID; }

	void			SetOccupyIndexLineClr(bool32 bOccupy) { m_bOccupyIndexLineClr = bOccupy; }
	bool32			GetOccupyIndexLineClr() const { return m_bOccupyIndexLineClr; }

	// ��ͼ���
public:
	void			Draw(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC,int32 iLoop);
	
	bool32			NodePosValue(int32 iPos, OUT float& fValue);		// �۸�

	bool32			RegionYValue(int32 y, float& fValue);				// regiony��۸�
	bool32			ValueRegionY(float fy, int32& iValue);				// regiony��۸�

	// Yֵ���ͷ�Ϊ�۸�ʵ�����ͣ� �������ͣ� RegionY��ʾ����
	// ���м۸�ת��ΪRegionY��ʾ���ͣ����뾭����ǰ������������ʵ��ת��
	// ����ת�����������Ƽ��κ��ⲿ�²��ת��
	// ����AxisYֵ����������������Сֵ
	bool32			PriceYToRegionY(float fPriceY, OUT int32 &iRegionY);
	bool32			RegionYToPriceY(int32 iRegionY, OUT float &fPriceY);
	bool32			PriceYToAxisY(float fPriceY, OUT float &fAxisY);
	bool32			AxisYToPriceY(float fAxisY, OUT float &fPriceY);
	bool32			AxisYToRegionY(float fAxisY, OUT int32 &iRegionY);
	bool32			RegionYToAxisY(int32 iRegionY, OUT float &fAxisY);

	// ����ָ����������
	bool32			PriceYToCmpClientY(float fPriceY, OUT int32& iCmpY);

	// ��չ����ϵ����
	bool32			ExtendPriceYToRegionY(float fPriceY, OUT int32 &iRegionY);
	bool32			ExtendRegionYToPriceY(int32 iRegionY, OUT float &fPriceY);

	bool32			GetAxisBaseY(OUT float &fBaseAxisY);	// ��ȡ������Ļ���ֵ
	void			SetPriceBaseY(float fPriceBaseY, bool32 bUpdateCalcObj = true);		// ֻҪ�趨�˷���ͨ���꣬��������ü۸����ֵ, ���������ü����������
	static    float GetInvalidPriceBaseYValue() { return CPriceToAxisYObject::GetInvalidPriceBaseYValue(); };	// ��������Чֵ
	// �����趨����ֵ
	bool32			GetValidFrontNode(int32 iFrontIndex, OUT CNodeData &node);	// ��ȡ��n����Чnode��0Ϊ��һ��
	bool32			GetValidBackNode(int32 iBackIndex, OUT CNodeData &node);		// ��ȡ������n����Чnode, 0Ϊ������һ��
	bool32			ConvertNodeDataToPrice(const CNodeData &node, OUT float &fPrice);		// �����������ͣ����node��ֵ

	bool32			GetAxisYCalcObject(OUT CPriceToAxisYObject &obj) const { obj = m_AxisYCalcObject; return m_MinMaxY.m_bValid; };	// ��ȡ�ڲ��ļ������, �ڲ�����������δ����ֵ

	//
	bool32			BeShowTitleText();
	bool32			BeNoDraw();

/////////////////////////////////////////
public:
	int32			m_clrTitleText;				// �����趨
	int32			m_iLineWidth;				// �����趨

protected:
	int32			m_iValidNodeNum;
	int32			m_iLevel;					// ���Ƶ��Ⱥ����,����Title�������Ĵ���.
	CString			m_StrTitle;					// ��ǰChart�ߵı���,��������,MA~6,...
	CString			m_StrGroupTitle;
	int32			m_iSelectNodeID;

	bool32			m_bOccupyIndexLineClr;		// �Ƿ�ռ����indexline����ɫ��������ÿ���߶���Ҫ����ɫ����Ҫ�����ô�ֵ�������

	CPriceToAxisYObject::E_AxisYType		m_eAxisYType;				// ����������

	// Yֵ���ͷ�Ϊ�۸�ʵ�����ͣ� �������ͣ� RegionY��ʾ����
	// ���м۸�ת��ΪRect��ʾ���ͣ����뾭����ǰ������������ʵ��ת��
	float			m_fPriceBase;				// ����axisy���õĻ���ֵ
	CPriceToAxisYObject	m_AxisYCalcObject;		// ����
	
	// ��ͼ����
public:	
	

private:
	CChartRegion	&m_ChartRegion;				// ���ڻ�ȡm_RectView,��y�����������.
	CNodeDrawer*	m_pDrawer;
	CNodeSequence*	m_pNodes;					// ��
	CMinMax			m_MinMaxY;					// �������±߽�

	float			m_fUserMin;					// �û������Сֵ
	float			m_fUserMax;					// 

friend class CChartRegion;
};


#endif


