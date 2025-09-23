
#include "StdAfx.h"
#include "ColorStep.h"

#include "math.h"

CColorStep::CColorStep()
{
	InitColorRange(RGB(255,255,255), RGB(0,0,0), 256);
}

CColorStep::CColorStep( COLORREF clrStart, COLORREF clrEnd, int iRange, bool bSmooth/* = true*/)
{
	InitColorRange(clrStart, clrEnd, iRange, bSmooth);
}

CColorStep::CColorStep( COLORREF clrStart, float fRStep/*=1.0*/, float fGStep/*=1.0*/, float fBStep/*=1.0*/, bool bSmooth /*= true*/ )
{
	InitColorRange(clrStart, fRStep, fGStep, fBStep, bSmooth);
}

CColorStep::~CColorStep()
{
	
}

void CColorStep::InitColorRange( COLORREF clrStart, COLORREF clrEnd, int iRange, bool bSmooth/* = true*/ )
{
	m_clrStart = clrStart;
	m_clrEnd   = clrEnd;
	m_iRange   = iRange;
	ASSERT( m_iRange!=0 );
	if ( m_iRange==0 )
	{
		m_iRange = INT_MAX; // ����ֵ- -
	}

	m_fRStepUnit = (GetRValue(clrEnd) - GetRValue(clrStart)) / (float)iRange;
	m_fGStepUnit = (GetGValue(clrEnd) - GetGValue(clrStart)) / (float)iRange;
	m_fBStepUnit = (GetBValue(clrEnd) - GetBValue(clrStart)) / (float)iRange;

	m_iPos = 0;
	m_bColorSmooth = bSmooth;
}

void CColorStep::InitColorRange( COLORREF clrStart, float fRStep/*=1.0*/, float fGStep/*=1.0*/, float fBStep/*=1.0*/, bool bSmooth /*= true*/ )
{
	m_iRange = INT_MAX;
	m_fRStepUnit = fRStep;
	m_fGStepUnit = fGStep;
	m_fBStepUnit = fBStep;
	m_iPos =0;
	m_clrEnd = RGB(0,0,0);
	m_clrStart = clrStart;
	m_bColorSmooth = bSmooth;
}

void CColorStep::InitColorRange( COLORREF clrBase, float fRatioClrEnd, int iRange, bool bSmooth /*= true*/ )
{
	m_iRange = iRange;
	ASSERT( m_iRange!=0 );
	if ( m_iRange==0 )
	{
		m_iRange = INT_MAX; // ����ֵ- -
	}

	m_clrStart = clrBase;
	int ir, ig, ib;
	ir = GetRValue(clrBase);
	ig = GetGValue(clrBase);
	ib = GetBValue(clrBase);

	int ir2, ig2, ib2;
	fRatioClrEnd = fabsf(fRatioClrEnd);
	ir2 = min(255, ((int)(ir * fRatioClrEnd)));
	ig2 = min(255, ((int)(ig * fRatioClrEnd)));
	ib2 = min(255, ((int)(ib * fRatioClrEnd)));
	
	m_clrEnd = RGB(ir2, ig2, ib2);

	m_fRStepUnit = (ir2 - ir) / (float)iRange;
	m_fGStepUnit = (ig2 - ig) / (float)iRange;
	m_fBStepUnit = (ib2 - ib) / (float)iRange;
	
	m_iPos = 0;
	m_bColorSmooth = bSmooth;
}

void CColorStep::InitColorRange(float fRatioClrBase, COLORREF clrEndBase, int iRange, bool bSmooth /*= true*/ )
{
	m_iRange = iRange;
	ASSERT( m_iRange!=0 );
	if ( m_iRange==0 )
	{
		m_iRange = INT_MAX; // ����ֵ- -
	}
	
	m_clrEnd = clrEndBase;
	int ir, ig, ib;
	ir = GetRValue(clrEndBase);
	ig = GetGValue(clrEndBase);
	ib = GetBValue(clrEndBase);
	
	int ir2, ig2, ib2;
	fRatioClrBase = fabsf(fRatioClrBase);
	ir2 = min(255, ((int)(ir * fRatioClrBase)));
	ig2 = min(255, ((int)(ig * fRatioClrBase)));
	ib2 = min(255, ((int)(ib * fRatioClrBase)));
	
	m_clrStart = RGB(ir2, ig2, ib2);
	
	m_fRStepUnit = (ir - ir2) / (float)iRange;
	m_fGStepUnit = (ig - ig2) / (float)iRange;
	m_fBStepUnit = (ib - ib2) / (float)iRange;
	
	m_iPos = 0;
	m_bColorSmooth = bSmooth;
}

void CColorStep::ResetStep()
{
	m_iPos = 0;	
}

COLORREF CColorStep::NextColor()
{
	ASSERT( m_iRange );

	COLORREF clrRet = GetColor(m_iPos);
	m_iPos++;
	return clrRet;
}

COLORREF CColorStep::GetColor( int iPos ) const
{
	ASSERT( m_iRange != 0 );
	byte r, g, b;
	if ( m_iRange != INT_MAX )
	{
		// ���޷�Χ�ڽ���
		//int iPosRemainer = iPos % m_iRange;
		int iPosRemainer = iPos % (m_iRange+1);		// ����ȡ���յ�ֵ
		
		bool	bReverse = false;
		if ( m_bColorSmooth )
		{
			// ƽ��Խ�����ɫ
			int	iOdd = (iPos / (m_iRange+1)) % 2;  // ż������ԭ���ģ������򷴷����ȡ��ɫ
			if ( iOdd )
			{
				bReverse = true;		
			}
		}
		
		
		if ( bReverse )
		{
			r = GetRValue(m_clrEnd) - (byte)(m_fRStepUnit * iPosRemainer);
			g = GetGValue(m_clrEnd) - (byte)(m_fGStepUnit * iPosRemainer);
			b = GetBValue(m_clrEnd) - (byte)(m_fBStepUnit * iPosRemainer);
		}
		else
		{
			r = GetRValue(m_clrStart) + (byte)(m_fRStepUnit * iPosRemainer);
			g = GetGValue(m_clrStart) + (byte)(m_fGStepUnit * iPosRemainer);
			b = GetBValue(m_clrStart) + (byte)(m_fBStepUnit * iPosRemainer);
		}
	}
	else
	{
		// ����255��Χ�ڽ��� ���������ɰ� ���䵽�� Ȼ���ɺ� ���� ���ף���������ͻȻ���ɰ׵��� - ��ʱ��֪����ôʵ��- -
		byte rs, gs, bs;
		rs = GetRValue(m_clrStart);
		gs = GetGValue(m_clrStart);
		bs = GetBValue(m_clrStart);
		
		int ir, ig, ib;
		ir = GetRValue(m_clrStart) + (int)(m_fRStepUnit * iPos);
		ig = GetGValue(m_clrStart) + (int)(m_fGStepUnit * iPos);
		ib = GetBValue(m_clrStart) + (int)(m_fBStepUnit * iPos);
		
		if ( m_bColorSmooth )
		{
			ir = max(ir, -ir);	// ������ô���� ��ʱ�취
			ig = max(ig, -ig);
			ib = max(ib, -ib);

			if ( (ir/256)%2 ) // �Ǹ������γ���255 - ����255���ֲ���
			{
				ir = 255 - ir%256;
			}
			if ( (ig/256)%2 ) // �Ǹ������γ���255 - ����255���ֲ���
			{
				ig = 255 - ig%256;
			}
			if ( (ib/256)%2 ) // �Ǹ������γ���255 - ����255���ֲ���
			{
				ib = 255 - ib%256;
			}
		}

		r = (byte)ir;
		g = (byte)ig;
		b = (byte)ib;
	}
	return RGB(r,g,b);
}

// ���佥��
CColorRayStep::CColorRayStep()
{
	InitColorRayRange(RGB(255,255,255), RGB(0,0,0), 256, CPoint(0,0), true);
}

CColorRayStep::CColorRayStep( COLORREF clrStart, COLORREF clrEnd, int iRadius, CPoint ptOrg, bool bSmooth /*= true*/ )
{
	InitColorRayRange(clrStart, clrEnd, iRadius, ptOrg, bSmooth);
}

CColorRayStep::CColorRayStep( COLORREF clrStart, CPoint ptOrg, float fRStep/*=1.0*/, float fGStep/*=1.0*/, float fBStep/*=1.0*/, bool bSmooth /*= true*/ )
{
	InitColorRayRange(clrStart, ptOrg, fRStep, fGStep, fBStep, bSmooth);
}

CColorRayStep::~CColorRayStep()
{
	
}

void CColorRayStep::InitColorRayRange( COLORREF clrStart, COLORREF clrEnd, int iRadius, CPoint ptOrg, bool bSmooth/* = true*/ )
{
	m_colorStep.InitColorRange(clrStart, clrEnd, iRadius, bSmooth);
	m_ptOrg = ptOrg;
}

void CColorRayStep::InitColorRayRange( COLORREF clrStart, CPoint ptOrg, float fRStep, float fGStep, float fBStep, bool bSmooth/* = true*/ )
{
	m_colorStep.InitColorRange(clrStart, fRStep, fGStep, fBStep, bSmooth);
	m_ptOrg = ptOrg;
}

COLORREF CColorRayStep::GetColor( CPoint pt )
{
	return GetColor(pt.x, pt.y);
}

COLORREF CColorRayStep::GetColor( int x, int y )
{
	// ����õ���Բ�ĵ�ֱ�߾���
	int cx = x - m_ptOrg.x;
	int cy = y - m_ptOrg.y;
	int iDistance = (int)(sqrtf(((float)cx)*cx + ((float)cy)*cy));
	return m_colorStep.GetColor(iDistance);
}
