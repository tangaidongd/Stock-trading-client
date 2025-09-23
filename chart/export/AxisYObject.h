#ifndef _AXISYOBJECT_H_
#define _AXISYOBJECT_H_
#include "typedef.h"
#include <float.h>
#include "chartexport.h"
//////////////////////////////////////////////////////////////////////////
// ����Y���һ��С����
// ���� ����ֵ��ʵ����ʾ��Rect��ֵ�����Ա���, ����ֵ��۸�ת��ͨ�������㷨

class CHART_EXPORT CPriceToAxisYObject
{
public:
	enum E_AxisYType
	{
		EAYT_Normal = 0,		// ��ͨ����
		EAYT_Pecent,			// �ٷֱ�����
		EAYT_Log,			// ��������
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
	static float  GetInvalidPriceBaseYValue() { return -FLT_MAX; }		// ��������Чֵ

	// ��չ���� ���㲻һ���Ƿ�Χ�ڵ������ܼ��������
	bool32	ExtendPriceYToRectY(const CRect &rc, float fPriceY, OUT int32 &iRectY);
	bool32	ExtendRectYToPriceY(const CRect &rc, int32 iRectY, OUT float &fPriceY);

	bool32			GetAxisBaseY(OUT float &fBaseAxisY);	// ��ȡ������Ļ���ֵ

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
	float m_fAxisYMin;		// ��С�������ֵ
	float m_fAxisYMax;

	float m_fPriceBase;     // �۸����ֵ, �������ֵ��ɼ۸�������ת��
	bool32	m_bValidMinMaxNormal;	// �Ƿ���Ч�������Сֵ, ������ʾ��ͨ����£�����������ܲ�����, Ŀǰ3�����������ʾ������Ч��~~

	bool32	m_bAlignRectTop;		// �����������ͬ�������������ǵײ�
};

#endif //_AXISYOBJECT_H_