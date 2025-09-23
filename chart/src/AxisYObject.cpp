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
	m_bAlignRectTop = true;	// 默认顶部
}

bool32 CPriceToAxisYObject::SetCalcParam( E_AxisYType eType, float fAxisYMin, float fAxisYMax, float fPriceBase )
{	
	
//	if ( eType >= EAYT_Normal && eType <= EAYT_Log
// 		&& fAxisYMax >= fAxisYMin		// 允许设立无效值，以反应错误的数据内容
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
				m_fPriceBase = fAxisYMin;// 没有有效值的，普通的以最低值为基本
			}
			break;
		case EAYT_Pecent:
			m_bValidMinMaxNormal = m_fAxisYMax >= m_fAxisYMin;
			if ( 0.0f == m_fPriceBase || !m_bValidMinMaxNormal || !_finite(fPriceBase) )
			{
				m_bValidMinMaxNormal = false;
				m_fPriceBase = GetInvalidPriceBaseYValue();	// 无效值不能计算
			}
			break;
		case EAYT_Log:
			m_bValidMinMaxNormal = m_fAxisYMax >= m_fAxisYMin;
			if ( 0.0f == m_fPriceBase || !m_bValidMinMaxNormal || !_finite(fPriceBase) )
			{
				m_bValidMinMaxNormal = false;
				m_fPriceBase = GetInvalidPriceBaseYValue();	// 无效值不能计算
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
	// 价格转为坐标 无检验
	return PriceYToAxisYByBase(m_eAxisYCalcType, m_fPriceBase, fPriceY, fAxisY);
}

bool32 CPriceToAxisYObject::AxisYToPriceY( float fAxisY, OUT float &fPriceY )
{
	// 坐标值转为价格 - 无检验
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
	
	// 限定范围
	if ( (bMustInRC && !_MY_INRANGEF(fAxisY, m_fAxisYMin, m_fAxisYMax)) )		// 坐标轴最大最小值
	{
		return false;
	}

	if ( m_fAxisYMax == m_fAxisYMin )
	{
		if ( m_bAlignRectTop )
		{
			iRectY = rc.top;// 同价格靠近顶部
		}
		else
		{
			iRectY = rc.bottom;// 同价格靠近底部
		}
		return true;			// 方向从下道上
	}
	
	// 三种坐标系，坐标值与regiony都是线性关系
	iRectY = rc.bottom - (int32)((fAxisY-m_fAxisYMin)*rc.Height()/(m_fAxisYMax-m_fAxisYMin));
	return true;
}

bool32 CPriceToAxisYObject::RectYToAxisY( const CRect &rc, int32 iRectY, OUT float &fAxisY, bool32 bMustInRC/*=true*/ )
{
	fAxisY = 0.0f;
	
	if ( !m_bValidMinMaxNormal )		// 坐标轴最大最小值
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
	
	// 三种坐标系，坐标值与regiony都是线性关系
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
	// 价格转为坐标 无检验
	// 保证计算的axis是最大值>=最小值的，与Price大小方向一致
	switch (eType)
	{
	case EAYT_Normal:
		fAxisY = fPriceY;		// 1-1对应
		return true;
	case EAYT_Pecent:
		{
			// 百分比坐标与价格: 百分比实际上为所有价格与基础价格的百分比
			if ( fPriceBase != GetInvalidPriceBaseYValue() && fPriceBase != 0.0f )
			{
				// 有没有基础价0的呢？
				// 带符号的绝对偏差比例
				float fAbsBase = fPriceBase < 0.0f ? -fPriceBase : fPriceBase;
				fAxisY = (fPriceY-fPriceBase)/fAbsBase;
				return true;
			}
		}
		break;
	case EAYT_Log:
		{
			// 对数坐标与价格: 对数为上一个%10价格的百分比
			// price = base * ((1.10)^(fAxisY*10))
			// axisY = log(1.1, price/base) = log10(price/base) / log10(1.10)
			if ( fPriceBase != GetInvalidPriceBaseYValue() && fPriceBase != 0.0f )
			{
				static float sfDivide1 = logf(1.10f);
				static float sfDivide2 = logf(0.9f);
				float fUp = fPriceY/fPriceBase;	// 出现了0&负数，怎么办
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
	// 坐标值转为价格 - 无检验
	switch (eType)
	{
	case EAYT_Normal:
		fPriceY = fAxisY;		// 1-1对应
		return true;
	case EAYT_Pecent:
		{
			// 百分比坐标与价格: 百分比实际上为所有价格与基础价格的百分比
			if ( fPriceBase != GetInvalidPriceBaseYValue() )
			{
				// 有没有基础价0的呢？
				// 带符号的绝对偏差比例
				float fAbsBase = fPriceBase < 0.0f ? -fPriceBase : fPriceBase;
				fPriceY = fAbsBase * (fAxisY) + fPriceBase;
				return true;
			}
		}
		break;
	case EAYT_Log:
		{
			// 对数坐标与价格: 对数为上一个%10价格的百分比
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

