#ifndef _AXISYOBJECT_H_
#define _AXISYOBJECT_H_
#include "typedef.h"
#include <float.h>
#include "chartexport.h"
//////////////////////////////////////////////////////////////////////////
// 计算Y轴的一个小东西
// 基于 坐标值与实际显示的Rect的值成线性比例, 坐标值与价格转换通过其它算法

class CHART_EXPORT CPriceToAxisYObject
{
public:
	enum E_AxisYType
	{
		EAYT_Normal = 0,		// 普通坐标
		EAYT_Pecent,			// 百分比坐标
		EAYT_Log,			// 对数坐标
	};
public:
	CPriceToAxisYObject();
	
	bool32 PriceYToAxisY(float fPriceY, OUT float &fAxisY);
	bool32 AxisYToPriceY(float fAxisY, OUT float &fPriceY);
	bool32	PriceYToRectY(const CRect &rc, float fPriceY, OUT int32 &iRectY, bool32 bMustInRC=true);
	bool32	RectYToPriceY(const CRect &rc, int32 iRectY, OUT float &fPriceY, bool32 bMustInRC=true);
	bool32	AxisYToRectY(const CRect &rc, float fAxisY, OUT int32 &iRectY, bool32 bMustInRC=true);
	bool32	RectYToAxisY(const CRect &rc, int32 iRectY, OUT float &fAxisY, bool32 bMustInRC=true);

	static bool32 PriceYToAxisYByBase(E_AxisYType eType, float fPriceBase, float fPriceY, OUT float &fAxisY);
	static bool32 AxisYToPriceYByBase(E_AxisYType eType, float fPriceBase, float fAxisY, OUT float &fPriceY);
	static float  GetInvalidPriceBaseYValue() { return -FLT_MAX; }		// 基础的无效值

	// 扩展运算 计算不一定是范围内的所有能计算的数据
	bool32	ExtendPriceYToRectY(const CRect &rc, float fPriceY, OUT int32 &iRectY);
	bool32	ExtendRectYToPriceY(const CRect &rc, int32 iRectY, OUT float &fPriceY);

	bool32			GetAxisBaseY(OUT float &fBaseAxisY);	// 获取坐标轴的基础值

	bool32 SetCalcParam(E_AxisYType eType, float fAxisYMin, float fAxisYMax, float fPriceBase);

	bool32 operator==(const CPriceToAxisYObject &obj) const;
	bool32 operator!=(const CPriceToAxisYObject &obj) const;

	bool32 IsValid() const;

	float  GetAxisYMax() const { return m_fAxisYMax; }
	float  GetAxisYMin() const { return m_fAxisYMin; }
	float  GetPriceBaseY() const { return m_fPriceBase; }
	E_AxisYType GetAxisYType() const { return m_eAxisYCalcType; }

	void	SetAlignRectTop(bool32 bAlignTop) { m_bAlignRectTop = bAlignTop; }
	bool32  GetAlignRectTop() const { return m_bAlignRectTop; }

private:
	E_AxisYType  m_eAxisYCalcType;
	float m_fAxisYMin;		// 最小最大坐标值
	float m_fAxisYMax;

	float m_fPriceBase;     // 价格基础值, 依赖这个值完成价格与坐标转换
	bool32	m_bValidMinMaxNormal;	// 是否有效的最大最小值, 仅仅暗示普通情况下，特殊情况可能不成立, 目前3中坐标这个暗示还是有效的~~

	bool32	m_bAlignRectTop;		// 如果最低最高相同，靠近顶部还是底部
};

#endif //_AXISYOBJECT_H_