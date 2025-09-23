#include "StdAfx.h"

#include "AxisYObject.h"
#include "float.h"
#include "math.h"

#define _MY_INRANGEF(fValue, fMin, fMax) (fMax>=fMin? (fValue>=fMin&&fValue<=fMax) : (fValue<=fMin&&fValue>=fMax))

CPriceToAxisYObject::CPriceToAxisYObject()
{
	m_eAxisYCalcType = EAYT_Normal;
	m_fAxisYMax = m_fAxisYMin = 0.0f;
	m_fPriceBase = GetInvalidPriceBaseYValue();
	m_bValidMinMaxNormal = false;
	m_bAlignRectTop = true;	// Ĭ�϶���
}

bool32 CPriceToAxisYObject::SetCalcParam( E_AxisYType eType, float fAxisYMin, float fAxisYMax, float fPriceBase )
{	
	
//	if ( eType >= EAYT_Normal && eType <= EAYT_Log
// 		&& fAxisYMax >= fAxisYMin		// ����������Чֵ���Է�Ӧ�������������
// 		&& !_isnan(fPriceBase)
// 		&& _finite(fPriceBase)
//		)
	if(eType >= EAYT_Normal && eType <= EAYT_Log)//lint !e568
	{
		m_eAxisYCalcType = eType;
		m_fAxisYMin = fAxisYMin;
		m_fAxisYMax = fAxisYMax;
		m_fPriceBase = fPriceBase;
		m_bValidMinMaxNormal = m_fAxisYMax >= m_fAxisYMin;		
		switch (m_eAxisYCalcType)
		{
		case EAYT_Normal:
			if ( fPriceBase == GetInvalidPriceBaseYValue() )
			{
				m_fPriceBase = fAxisYMin;// û����Чֵ�ģ���ͨ�������ֵΪ����
			}
			break;
		case EAYT_Pecent:
			m_bValidMinMaxNormal = m_fAxisYMax >= m_fAxisYMin;
			if ( 0.0f == m_fPriceBase || !m_bValidMinMaxNormal || !_finite(fPriceBase) )
			{
				m_bValidMinMaxNormal = false;
				m_fPriceBase = GetInvalidPriceBaseYValue();	// ��Чֵ���ܼ���
			}
			break;
		case EAYT_Log:
			m_bValidMinMaxNormal = m_fAxisYMax >= m_fAxisYMin;
			if ( 0.0f == m_fPriceBase || !m_bValidMinMaxNormal || !_finite(fPriceBase) )
			{
				m_bValidMinMaxNormal = false;
				m_fPriceBase = GetInvalidPriceBaseYValue();	// ��Чֵ���ܼ���
			}
			break;	
		}
		
		return true;
	}
	return false;
}


bool32 CPriceToAxisYObject::operator==( const CPriceToAxisYObject &obj ) const
{
	return m_eAxisYCalcType == obj.m_eAxisYCalcType
		&& m_fAxisYMin == obj.m_fAxisYMin
		&& m_fAxisYMax == obj.m_fAxisYMax
		&& m_fPriceBase == obj.m_fPriceBase
		&& m_bValidMinMaxNormal == obj.m_bValidMinMaxNormal ;
}

bool32 CPriceToAxisYObject::operator!=( const CPriceToAxisYObject &obj ) const
{
	return !(*this == obj);
}


bool32 CPriceToAxisYObject::PriceYToAxisY( float fPriceY, OUT float &fAxisY )
{
	// �۸�תΪ���� �޼���
	return PriceYToAxisYByBase(m_eAxisYCalcType, m_fPriceBase, fPriceY, fAxisY);
}

bool32 CPriceToAxisYObject::AxisYToPriceY( float fAxisY, OUT float &fPriceY )
{
	// ����ֵתΪ�۸� - �޼���
	return AxisYToPriceYByBase(m_eAxisYCalcType, m_fPriceBase, fAxisY, fPriceY);
}

bool32 CPriceToAxisYObject::PriceYToRectY( const CRect &rc, float fPriceY, OUT int32 &iRectY, bool32 bMustInRC/*=true*/ )
{
	float fAxisY;
	return PriceYToAxisY(fPriceY, fAxisY) && AxisYToRectY(rc, fAxisY, iRectY, bMustInRC);
}

bool32 CPriceToAxisYObject::RectYToPriceY( const CRect &rc, int32 iRectY, OUT float &fPriceY, bool32 bMustInRC/*=true*/ )
{
	float fAxisY;
	return ( RectYToAxisY(rc, iRectY, fAxisY, bMustInRC) && AxisYToPriceY(fAxisY, fPriceY) );
}

bool32 CPriceToAxisYObject::ExtendPriceYToRectY( const CRect &rc, float fPriceY, OUT int32 &iRectY )
{
	return PriceYToRectY(rc, fPriceY, iRectY, false);
}

bool32 CPriceToAxisYObject::ExtendRectYToPriceY( const CRect &rc, int32 iRectY, OUT float &fPriceY)
{
	return RectYToPriceY(rc, iRectY, fPriceY, false);
}

bool32 CPriceToAxisYObject::AxisYToRectY( const CRect &rc, float fAxisY, OUT int32 &iRectY, bool32 bMustInRC/*=true*/ )
{
	iRectY = -1;
	
	// �޶���Χ
	if ( (bMustInRC && !_MY_INRANGEF(fAxisY, m_fAxisYMin, m_fAxisYMax)) )		// �����������Сֵ
	{
		return false;
	}

	if ( m_fAxisYMax == m_fAxisYMin )
	{
		if ( m_bAlignRectTop )
		{
			iRectY = rc.top;// ͬ�۸񿿽�����
		}
		else
		{
			iRectY = rc.bottom;// ͬ�۸񿿽��ײ�
		}
		return true;			// ������µ���
	}
	
	// ��������ϵ������ֵ��regiony�������Թ�ϵ
	iRectY = rc.bottom - (int32)((fAxisY-m_fAxisYMin)*rc.Height()/(m_fAxisYMax-m_fAxisYMin));
	return true;
}

bool32 CPriceToAxisYObject::RectYToAxisY( const CRect &rc, int32 iRectY, OUT float &fAxisY, bool32 bMustInRC/*=true*/ )
{
	fAxisY = 0.0f;
	
	if ( !m_bValidMinMaxNormal )		// �����������Сֵ
	{
		return false;
	}

	if ( bMustInRC )
	{
		if ( iRectY < rc.top || iRectY > rc.bottom )
		{
			return false;
		}
	}
	
	if ( rc.Height() <= 0 )
	{
		fAxisY = m_fAxisYMin;
		return true;
	}
	
	// ��������ϵ������ֵ��regiony�������Թ�ϵ
	fAxisY = m_fAxisYMin + (rc.bottom - iRectY)*(m_fAxisYMax-m_fAxisYMin)/rc.Height();
	return true;
}

bool32 CPriceToAxisYObject::GetAxisBaseY( OUT float &fBaseAxisY )
{
	if ( !m_bValidMinMaxNormal )
	{
		return false;
	}
	
	return PriceYToAxisY(m_fPriceBase, fBaseAxisY);
}

bool32 CPriceToAxisYObject::PriceYToAxisYByBase( E_AxisYType eType, float fPriceBase, float fPriceY, OUT float &fAxisY )
{
	// �۸�תΪ���� �޼���
	// ��֤�����axis�����ֵ>=��Сֵ�ģ���Price��С����һ��
	switch (eType)
	{
	case EAYT_Normal:
		fAxisY = fPriceY;		// 1-1��Ӧ
		return true;
	case EAYT_Pecent:
		{
			// �ٷֱ�������۸�: �ٷֱ�ʵ����Ϊ���м۸�������۸�İٷֱ�
			if ( fPriceBase != GetInvalidPriceBaseYValue() && fPriceBase != 0.0f )
			{
				// ��û�л�����0���أ�
				// �����ŵľ���ƫ�����
				float fAbsBase = fPriceBase < 0.0f ? -fPriceBase : fPriceBase;
				fAxisY = (fPriceY-fPriceBase)/fAbsBase;
				return true;
			}
		}
		break;
	case EAYT_Log:
		{
			// ����������۸�: ����Ϊ��һ��%10�۸�İٷֱ�
			// price = base * ((1.10)^(fAxisY*10))
			// axisY = log(1.1, price/base) = log10(price/base) / log10(1.10)
			if ( fPriceBase != GetInvalidPriceBaseYValue() && fPriceBase != 0.0f )
			{
				static float sfDivide1 = logf(1.10f);
				static float sfDivide2 = logf(0.9f);
				float fUp = fPriceY/fPriceBase;	// ������0&��������ô��
				if ( fUp > 0.0 )
				{
					if ( fUp >= 1.0 )
					{
						fAxisY = logf(fUp)/sfDivide1;
					}
					else
					{
						fAxisY = -logf(fUp)/sfDivide2;
					}
					return true;
				}
			}
		}
		break;
	default:
		ASSERT( 0 );
	}
	
	return false;
}

bool32 CPriceToAxisYObject::AxisYToPriceYByBase( E_AxisYType eType, float fPriceBase, float fAxisY, OUT float &fPriceY )
{
	// ����ֵתΪ�۸� - �޼���
	switch (eType)
	{
	case EAYT_Normal:
		fPriceY = fAxisY;		// 1-1��Ӧ
		return true;
	case EAYT_Pecent:
		{
			// �ٷֱ�������۸�: �ٷֱ�ʵ����Ϊ���м۸�������۸�İٷֱ�
			if ( fPriceBase != GetInvalidPriceBaseYValue() )
			{
				// ��û�л�����0���أ�
				// �����ŵľ���ƫ�����
				float fAbsBase = fPriceBase < 0.0f ? -fPriceBase : fPriceBase;
				fPriceY = fAbsBase * (fAxisY) + fPriceBase;
				return true;
			}
		}
		break;
	case EAYT_Log:
		{
			// ����������۸�: ����Ϊ��һ��%10�۸�İٷֱ�
			// price = base * ((1.10)^(fAxisY*10))
			if ( fPriceBase != GetInvalidPriceBaseYValue() )
			{
				if ( fAxisY >= 0 )
				{
					fPriceY = fPriceBase*powf(1.10f, fAxisY);
				}
				else
				{
					fPriceY = fPriceBase*powf(0.9f, -fAxisY);
				}
				return true;
			}
		}
		break;
	default:
		ASSERT( 0 );
	}
	
	return false;
}

bool32 CPriceToAxisYObject::IsValid() const
{
	return m_bValidMinMaxNormal;
}

