#ifndef _REGION_H_
#define _REGION_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <float.h>
#include "chartexport.h"
/*
�����б�:
(1)x,y������ǹ̶���/��,���ܵȱ�����.------�����
(2)�϶�һ����,��صı�Ҳ��Ҫ�϶�.���������϶�KLine���±�,��ô���Y����±�ҲҪ���Ŷ�.------�����
(3)����һ��Region�����������------δ���
(4)LineType/LineStyle------δ���
(5)CKLine�����ݵĺϷ��Լ��,����>0,High>Low,High>Open��Close,Low<Open��Close

2008-4-19�޸�:����Region�ĳߴ�Ϊ0,���������ڼ��޸�BottomDragAble
*/

#include "typedef.h"
#include "MemDCEx.h"
#include "afxtempl.h"

class CRegion;
class CRegionViewParam;
class CDrawingCurve;

const int32		KRegionMouseOverAWhileTimerId	= 100001;
const uint32	KRegionMaskSegment				= 0xFFFFFFFF;
const int32		KRegionMinWH					= 0;//����Region�ĳߴ�Ϊ0

typedef CArray<CRegion*,CRegion*> RegionArray;

/////////////////////////////////////////////////
// ������ 
const float KAxisFloatRefer = 10000.;	// ���������Pos�ο�����

// �ָ����ϵ�������ʾ
class CHART_EXPORT CAxisDivideText
{
public:
	CAxisDivideText()
	{
		m_uiTextAlign	= DT_CENTER | DT_VCENTER;
		m_lTextColor	= 0x00000000;
	}

public:
	CString			m_StrText;		// ��Ҫ��ʾ�����֣������¡����ң�
	uint32			m_uiTextAlign;	// DT_LEFT | DT_TOP
	long			m_lTextColor;	
};


// ����ָ�
class CHART_EXPORT CAxisDivide
{
public:
	enum E_DivideLevel			// ��ʾ���ȼ��� ��ͼ������� ���ǰ�������࣬ ����ʾָ���������ϵķָ���Ϣ
	{
		EDLLevel1	= 0x00000001,				// ���ȼ����
		EDLLevel2	= 0x00000002 | EDLLevel1,
		EDLLevel3	= 0x00000004 | EDLLevel2,
		EDLLevel4	= 0x00000008 | EDLLevel3
	};

	enum E_LineStyle			// ����
	{
		ELSNone		= 0,		// ����
		
		ELSSolid,				// ʵ��
		ELSDoubleSolid,			// ˫ʵ��
		ELS3LineSolid,			// 3ʵ��

		ELSDot,					// ����
		ELSDoubleDot,			// ˫����

		ELSWave,				// ������
	};

public:
	int32			m_iPosPixel;	// ��ʾλ�ã� ��������

	// ����
	E_DivideLevel	m_eLevelText;	// ������ʾ���ȼ��� �Ȼ����ȼ��ߵģ� ���ȼ�I�ı��뻭�� ��������ʵ����ʱ���ڸ��������
	CAxisDivideText	m_DivideText1;	//�������¡����ң�
	CAxisDivideText	m_DivideText2;
	
	// 
	E_DivideLevel	m_eLevelLine;	// ������ʾ���ȼ������ϸ������ȼ���ʾ��ͷ�ı��
	E_LineStyle		m_eLineStyle;	// ���ߵ���״
};

// ƽ��������
class CHART_EXPORT CAxisSmooth
{
public:
	CAxisSmooth() 
	{
		m_iPixelWidth	= 0;
		m_fMax			= FLT_MIN;
		m_fMin			= FLT_MAX;
		m_iScale		= 1;
	}

	void SetPixelWidth(int32 iPixelWidth) { m_iPixelWidth = iPixelWidth; }
	
	bool32 GetSpecialPixelValue(int32 iPixel, float &fValue);

public:
	// 
	int32			m_iPixelWidth;		// ���ؿ��(��ֵ��ӦKAxisFloatRefer)

	// 
	float			m_fMax;				// ��ʵ�ķ�Χ
	float			m_fMin;
	int32			m_iScale;			// ���ţ� Ĭ��Ϊ1�� �������1�� ��Ҫ��ʾ��ֵ�����ţ� ������Ŀ����ʾ��������Ϣ
};

// ����̶�����
class CHART_EXPORT CAxisNode
{
public:
	CAxisNode()
	{
//		m_fPosStart		= 0.;
//		m_fPosEnd		= 0.;
		m_fPixelWidthPerUnit = 0.;
		m_iTimeId = 0;
	}

public:
	int32			m_iStartPixel;		// ��ʾ��Χ
	int32			m_iCenterPixel;
	int32			m_iEndPixel;
	
	// �ڴ���k����������£�x��m_iEndPixel��m_iStartPixel��ֵ����Ϊ0�����·Ѳ���������ѭ��,��ʱʹ�ø���������k�����ؿ��
	float			m_fPixelWidthPerUnit;
	
	int32			m_iTimeId;			// X�����걣��׼ȷ��id��Ϣ����ֹ�������ܼ�ʱ��X���������ѡȡ��һ��id

	CString			m_StrSliderText1;	// �������ݣ������¡����ң�
	CString			m_StrSliderText2;
};

// ����������
class CHART_EXPORT CAxisCustom
{
public:
	CAxisCustom()
	{
		m_iPixelWidth	= 0;
	}
	
public:
	bool32			GetSpecialPixelInfo(int32 iPixel, int32 &iIndex, int32 &iPixelStart, int32 iPixelEnd, CString &StrSliderText1, CString &StrSliderText2);

public:
	// 
	int32			m_iPixelWidth;			// ���ؿ��
	
	// 
	CArray<CAxisNode, CAxisNode&>	m_aAxisNodes;	// �ָ��ÿ����������Ϣ�� λ����Ϣ�ڷ�Custom����������
};
///////////////////////////////////////////////////////////////////////////////
//�зֵķ���
enum E_Direction 
{
	EDLeft  = 0,
	EDRight,
	EDTop,
	EDBottom
};

//����������
enum E_MouseOpType 
{
	EMOTDefault = 0,//ȱʡ��������
	EMOTPan,//����ƽ��
	EMOTRuler,//���
	EMOTUserDrawing //�Ի���,���ж���������,Ԥ����չ.
};

//����϶�Region�ı߿�(Edge)�Ľṹ
struct T_PickEdge
{
	CRegion*		m_pRegion;//ʰȡ����Region
	E_Direction		m_eDir;//ʰȡ����Region�ı�
};

//����϶�Region�ı߿�(Edge)��ǿ��(Force)�ı�Rect�Ľṹ
struct T_ForceRegion
{
	CRegion*		m_pRegion;//Ҫ�ı��Region
	E_Direction		m_eDir;//Ҫ�ı�ı�
	int32			m_iNewValue;//�ߵ���ֵ
};

/////////////////////////////////////////////////////////////////////////
//�з�Region(���Region)���õ�ȫ�ֲ�����,����Ӧ�ò�����.
class CHART_EXPORT CRegionViewParam
{
public:
	CRegionViewParam ( IN CWnd* pWnd);
	virtual ~CRegionViewParam();

	void			SetParentWnd ( CWnd* pWnd );
	void			UpdateGDIObject();
	void			SetViewToolBarHeight(int32 iViewToolBarHeight);
	CMemDCEx*		GetMainDC();
	CMemDCEx*		GetOverlayDC();
	CMemDCEx*		GetPickDC();
	CWnd*			GetView();
	void			EnableClipDiff ( bool32 bEnable,CRect& Rect );

	// ��ȡ��ͼ����region���������
	virtual bool32	GetViewRegionRect(OUT LPRECT pRect);

	void			BShowViewToolBar(bool32 bShowIndexToolbar, bool32 bShowTopToolbar);
	void			GetShowViewToolBar(OUT bool32& bShowIndexToolbar, OUT bool32& bShowTopToolbar);

	CArray<T_PickEdge,T_PickEdge>			m_PickEdges;
	CArray<T_ForceRegion,T_ForceRegion>		m_ForceRegions;
	
	bool32			m_bDragEdgeIn;
	int32			m_iLevelMax;
	int32			m_iLayerMax;
	CPoint			m_PointMouse;
	CPoint			m_PointLastMouse;
	CRect			m_RectLast;
	uint32			m_iFlag;
	bool32          m_bRightButPress;

protected:
	CRegionViewParam();	
	void ReleaseMemDC( );

	CClientDC*		m_pDC;
	CMemDCEx*		m_pPickDC;
	CMemDCEx*		m_pMainDC;
	CMemDCEx*		m_pOverlayDC;
	CWnd*			m_pWnd;
	CRect			m_RectView;
	int32			m_iCurveID;
	static int32	g_iID;
private:
	bool32			m_bShowTopBtn;
	bool32			m_bShowIndexBtn;
	int32			m_iViewToolBarHeight;

friend class CRegion;
};

typedef enum E_RegionDrawNotifyType
{
	ERDNBeforeDrawCurve					= 0x01,			// ������ǰ
	ERDNAfterDrawCurve					= 0x02,			// �����ߺ�
	ERDNBeforeTransformAllRegion		= 0x04,			// ת������ǰ
//	ERDNAfterDrawAllRegion				= 0x08,			// ������Region��
//	ERDNDrawAllOverlay					= 0x10			// �����ж���
} E_RegionDrawNotifyType;


class CHART_EXPORT CRegionDrawNotify
{
protected:
	virtual void	OnRegionDrawNotify(IN CMemDCEx* pDC, IN CMemDCEx* pPickDC, CRegion* pRegion, E_RegionDrawNotifyType eType ) = 0;
friend class CRegion;
friend class CChartRegion;
};

class CHART_EXPORT CRegionDrawElement
{
public:
	CRegionDrawElement()
	{
		m_pRegionDrawNotify = NULL;
		m_uiDrawFlag		= 0;
	}

public:
	CRegionDrawNotify	*m_pRegionDrawNotify;
	uint32				m_uiDrawFlag;
};

//CRegion������
class CHART_EXPORT CRegion
{
public:
	enum E_RegionFlag
	{
		// ���ⲿ��(16λ)		
		KRectZoomOut				= 0x00008000,			// ���ο�ѡ�Ŵ�

		KFixWidth					= 0x00010000,			// �̶����
		KFixHeight					= 0x00020000,			// �̶��߶�
		KRightDragAble				= 0x00040000,			// �ұ�Ե�����϶�
		KBottomDragAble				= 0x00080000,			// ��Ե�����϶�
		
		// �ڲ���(16λ)
		KMousePress					= 0x00100000,			// ��ǰ���״̬Ϊ�����ѹ
		KMouseDrag					= 0x00200000,			// ��ǰ���״̬Ϊ�϶�
		KMousePressEdge				= 0x00400000,			// ��ǰ���״̬Ϊ�����ѹ��Region�ı�(Edge)
		KMouseDragEdge				= 0x00800000,			// ��ǰ���״̬Ϊ�϶���Region�ı�(Edge)
	
		KMouseOverAWhile			= 0x01000000,
		KMouseOverEdge				= 0x02000000,
		KFrame						= 0x04000000,			// ��ǰRegion����ΪFrame
		KTooSmall					= 0x08000000,			// ��ǰRegion�ĳߴ�̫С(���Ȼ���<=KRegionMinWH)
		
		// ���Ʋ���		
		KDrawTransform				= 0x10000000,
		KDrawNotify					= 0x20000000,
		KDrawCurves					= 0x40000000,
		KDrawNoSwap					= 0x80000000,
		KDrawFull					= KDrawTransform | KDrawCurves | KDrawNotify
	};

public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//����/��������
	CRegion(IN CRegionViewParam* pViewParam, CString StrName, uint32 iFlag = 0);
	virtual ~CRegion ( );
	
public:
	static void		FreeAll(CRegion* pRegion);

	// ��ӿڣ� ����¼��� ��Ҫ����
protected:
	// �ڲ���װ��windows������¼�,����x,y����Ϊwindows��������.
	// ������CRegion::Region2Client/Client2Region������"client<-->Region"֮�������ת��
	virtual void	OnMousePress  ( int32 x, int32 y, int32 iPass, bool bRightButton = false);//��������ѹ
	virtual void	OnMousePressUp( int32 x, int32 y); //����������
	virtual void	OnMouseRClick ( int32 x, int32 y );//����Ҽ����,���絯���˵�
	virtual void	OnMouseDBClick( int32 x, int32 y );//������˫��
	virtual void	OnMouseMove   ( int32 x, int32 y );//����ƶ�
	
	virtual void	OnMouseOverAWhile ( int32 x, int32 y );//��x,y��ͣ����һ��ʱ��,���ڱ���:ʰȡ����ʾtooltips.
	virtual void	OnMouseOverLeave ( int32 x, int32 y );
	
	virtual void	OnMouseDragMove ( int32 x, int32 y , bool bRightButton = false );//�϶�
	virtual void	OnMouseDragDrop ( int32 x, int32 y, bool bRightButton = false );//��갴���ɿ�
	
	virtual void	OnMouseClickN ( int32 n, int32 x, int32 y );//��n�ε��
	virtual void	OnMouseMoveN  ( int32 n, int32 x, int32 y );//
	virtual void	OnMouseEnd    ( int32 n, int32 x, int32 y );
	
	//�����¼�
	virtual	bool32	OnKeyDown(int32 nChar, int32 nRepCnt, int32 nFlags);
	virtual	bool32	OnChar(int32 nChar, int32 nRepCnt, int32 nFlags);
	virtual	bool32	OnKeyUp(int32 nChar, int32 nRepCnt, int32 nFlags);
	
	//ʰȡ����:
	//ʰȡ����PickColor/PickName���߻���������(������/��Ϣ����/�Ի��ߵ�)���¼�֪ͨ
	virtual void	OnPick ( uint32 uiPickColor, int32 x, int32 y );//ʹ��CPixelPickDC�����
	virtual void	OnPick ( CString StrPickName, int32 x, int32 y );//ʹ��virtual bool32 CDrawingCurve::Pick�����
	
	//�ڲ��¼�
	virtual void	OnCreate  ( );//��ǰRegion������(����)����¼�
	virtual void	OnDestroy ( );//��ǰRegion��ɾ��ǰ���¼�
	virtual void	OnPreDraw ( IN CMemDCEx* pDC,IN CMemDCEx* pPickDC );
	virtual void	OnDraw    ( IN CMemDCEx* pDC,IN CMemDCEx* pPickDC );//��ǰRegion��Ҫ�ػ�
	virtual void	OnAfterDraw ( IN CMemDCEx* pOverlayDC );
	virtual void	OnActive  ( bool32 bActive );//֪ͨ��ǰRegion��bActive,����Region��,����һ����Active
	
	virtual void	OnMessage ( int32 mid,void* pData );
	
	//��Ҫ���������Ϸַ���������Region���¼���ɾ��Region!!!
	virtual int32	GetRegionMinWH ();
	
	// Region�Ĳ����ɾ��
public:
	bool32			SplitRegion(IN CRegion* pRegion, E_Direction eDir, CSize SizeWish);	// ����:�ڵ�ǰRegion�Ϸָ��һ���µ�Region,���ȺͿ��>=0
	bool32			Delete();				// ɾ��:ɾ����ǰRegion��ĳ����Region

	void			ReSizeAll();			// �ı��С,��IoViewChart��CRegion(����������)�ڲ�����,������һ��û������.
	void			NestSizeAll();
	void			ReDrawAll();			// ���»���,��IoViewChart��CRegion(����������)�ڲ�����,�ڲ����������CRegion��OnDraw�麯��
	
	// ��ͼ��� 
public:
	bool32			AddDrawNotify(CRegionDrawNotify* pDrawNotify, E_RegionDrawNotifyType eType);
	bool32			RemoveDrawNotify(CRegionDrawNotify* pDrawNotify, E_RegionDrawNotifyType eType);
	void			DrawNotify(CMemDCEx* pDC, IN CMemDCEx* pPickDC, E_RegionDrawNotifyType eType);

public:
	void			SetBottomDragAble(bool32 bAble);	//��������ӿ�
	bool32			RegionTooSmall( );

public:
	// CRegion�Ļ��Ʒ��������������:WindowDC<-->MemDC(Overlay)<-->MemDC(Main)
	void			BeginOverlay ( bool32 bFullClient = true);//��IoViewChart��CRegion(����������)�ڲ�����,��Overlayer�ϻ���һЩ��ʱ����.
	void			EndOverlay ();

	// CIoViewChart���������Region�����ת��,��λΪ����,x��y��ת��
	void			RegionToClient(INOUT CRect& Rect)		{ Rect.InflateRect(m_RectView.left, m_RectView.top, m_RectView.left, m_RectView.top); }
	void			ClientToRegion(INOUT CRect& Rect)		{ Rect.DeflateRect(m_RectView.left, m_RectView.top, m_RectView.left, m_RectView.top); }

	void			RegionToClient(INOUT CPoint& pt)		{ pt += m_RectView.TopLeft(); }
	void			ClientToRegion(INOUT CPoint& pt)		{ pt -= m_RectView.TopLeft(); }	

	void			RegionXToClient(INOUT int32& x)			{ x += m_RectView.left; }	
	void			ClientXToRegion(INOUT int32& x)			{ x -= m_RectView.left; }
	
	void			RegionYToClient(INOUT int32& y)			{ y += m_RectView.top; }
	void			ClientYToRegion(INOUT int32& y)			{ y -= m_RectView.top; }
	
	void			SetLevel(int32 iLevel);
	void			SetBackground(COLORREF color)			{ m_clrBackground = color; }
	void			SetBorderColor(COLORREF color)			{ m_clrBorder = color; if (NULL != m_pRegionParent) { m_pRegionParent->SetBorderColor(color); } }
	
	void			SetDrawFlag(uint32 iFlag);
	void			AddDrawFlag(uint32 iFlag);
	uint32			GetDrawFlag();
	void			ClearDrawFlag();

	void			MessageAll(int32 mid, void* pData );

	CRegion*		GetRegionRoot()							{return m_pRegionRoot;}

	CString			ToString ( );//����ǰRegion����Ҫ��Ա����ת��Ϊ�ַ�������.(������)
	CString			ToStringAll ( );//������Region����Ҫ��Ա����ת��Ϊ�ַ�������.(������)
	CString			DirToString ( E_Direction eDir );//��eDirת��Ϊ�ַ���(������)
	
	//��IoViewChart����
	//����
	bool32			TestKeyDown(int32 nChar, int32 nRepCnt, int32 nFlags);
	bool32			TestChar(int32 nChar, int32 nRepCnt, int32 nFlags);
	bool32			TestKeyUp(int32 nChar, int32 nRepCnt, int32 nFlags);
	
	//��IoViewChart����
	//���
	//�ڲ����˴���Region�߽��resize֮��
	//�������ض����������ж�,Ȼ���͵�OnMouseXXX��.
	int32			MouseMove( int32 iFlags, int32 x, int32 y );
	int32			MouseWheel ( int32 iFlags,int32 x,int32 y );
	int32			LButtonDblClk( int32 iFlags, int32 x, int32 y );
	int32			LButtonDown( int32 iFlags, int32 x, int32 y );
	int32			LButtonUp( int32 iFlags, int32 x, int32 y );
	int32			RButtonDblClk( int32 iFlags, int32 x, int32 y );
	int32			RButtonDown( int32 iFlags, int32 x, int32 y );
	int32			RButtonUp( int32 iFlags, int32 x, int32 y );

	bool32			Timer ( int32 iTimerID );

protected:
	//��������ĵݹ�˳��
	//(1) ����Self
	//(2) ���������� 
	//(3) ����������	
	void			GetRegions ( IN CRegion* pParent,INOUT RegionArray& Regions);//��pParent(����)��Regions����Regions��.
	void			NestedSize ( IN CRegion* pParent );
	void			NestedPickEdge ( int32 x, int32 y, bool32 bClick);

	void			NestedSizeReverse ( );//NestedSize�Ǹ��ݸ��ڵ�����ӽڵ��Rect,���������Ǹ����ӽڵ��Rect,���㸸�ڵ��Rect/m_fSizePercent;
	void			TestDragEdge ( int32 x, int32 y, OUT int32& iNewX, OUT int32& iNewY, bool32 bSave );
	void			SetDragForceSize ( );
	void			FindNeighborRegion ( IN CRegion* pSrc,E_Direction eDir,IN CRegion* pParent,INOUT RegionArray& Regions ); 
	void			AddToPickEdges ( E_Direction eDir );

public:
	CRect			m_RectView;				// ��ǰRegion��Rect
	CString			m_StrName;				// ��ǰRegion������
	CSize			m_SizeWish;				// ���浱ǰRegion�������Ĵ�С,�����KFixXXXX��־,����ʹ�ô˴�С.
	uint32			m_iFlag;				// ��־,��ౣ��32����־λ.	

protected:
	int32			m_iLayer;				// �ڶ������еĲ㼶.
	int32			m_iLevel;				// ���ȼ�:���ڿ��ƻ���/����¼��ȵ��Ⱥ����
	COLORREF		m_clrBackground;
	COLORREF		m_clrBorder;
	bool32			m_bFullOverlay;
	float			m_fSizePercent;			// pChild1ռParent Region��С�İٷֱ�.����KFrame������Ч
	E_MouseOpType	m_eMouseOpType;			// ��ǰ������������:ȱʡ/ƽ�ƻ���/���...
	E_Direction		m_eDirection;			// ��ǰRegion�ķ���

	// 
protected:
	CRegionViewParam	*m_pViewParam;		// ���Region��ɵĶ��������õ�һ������
	CRegion*		m_pRegionParent;		// ���ڵ�
	CRegion*		m_pRegionChild1;		// �ֵ�1
	CRegion*		m_pRegionChild2;		// �ֵ�2
	CRegion*		m_pRegionRoot;			// ���ڵ�

private:	
	CArray<CRegionDrawElement, CRegionDrawElement&>	m_aRegionDrawElements;
};


#endif // _REGION_H_
