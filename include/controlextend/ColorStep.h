#ifndef _COLORSTEP_H_
#define _COLORSTEP_H_

#include "dllexport.h"
// ��һ����ɫ���䵽��һ����ɫ
class CONTROL_EXPORT CColorStep{
public:
	CColorStep();
	CColorStep(COLORREF clrStart, COLORREF clrEnd, int iRange, bool bSmooth = true);
	CColorStep(COLORREF clrStart, float fRStep=1.0, float fGStep=1.0, float fBStep=1.0, bool bSmooth = true);  //fRstep - Red������λ 1/1
	~CColorStep();
	
	void	InitColorRange(COLORREF clrStart, float fRStep=1.0, float fGStep=1.0, float fBStep=1.0, bool bSmooth = true);
	void	InitColorRange(COLORREF clrStart, COLORREF clrEnd, int iRange, bool bSmooth = true);
	void	InitColorRange(COLORREF clrBase, float fRatioClrEnd, int iRange, bool bSmooth = true);
	void	InitColorRange(float fRatioClrBase, COLORREF clrEndBase, int iRange, bool bSmooth = true);
	void    ResetStep();
	COLORREF NextColor();		   // ����m_iPos����ɫ��m_iPosָ����һ����ɫ
	COLORREF GetColor(int iPos) const; // ѭ���ڷ�Χ��ȡֵ iPos % m_iRange
	COLORREF GetColor()const {return GetColor(m_iPos);};

	void	SetColorSmooth(bool bSmooth = true){m_bColorSmooth = bSmooth;};  // ����Խ�����һ��ѭ����ɫ��ʲô

private:
	COLORREF	m_clrStart;
	COLORREF	m_clrEnd;
	int		m_iRange;
	float		m_fRStepUnit;
	float		m_fGStepUnit;
	float		m_fBStepUnit;
	int		m_iPos;

	bool		m_bColorSmooth;
};

// �����Խ���
class CONTROL_EXPORT CColorRayStep
{
public:
	CColorRayStep();
	CColorRayStep(COLORREF clrStart, COLORREF clrEnd, int iRadius, CPoint ptOrg, bool bSmooth = true);
	CColorRayStep(COLORREF clrStart, CPoint ptOrg, float fRStep=1.0, float fGStep=1.0, float fBStep=1.0, bool bSmooth = true);  //fRstep - Red������λ 1/1
	~CColorRayStep();

	void		InitColorRayRange(COLORREF clrStart, COLORREF clrEnd, int iRadius, CPoint ptOrg, bool bSmooth = true);
	void        InitColorRayRange(COLORREF clrStart, CPoint ptOrg, float fRStep, float fGStep, float fBStep, bool bSmooth = true);  //fRstep - Red������λ 1/1
	COLORREF	GetColor(int x, int y);
	COLORREF	GetColor(CPoint pt);

private:
	CPoint		m_ptOrg;
	CColorStep  m_colorStep;
};


#endif //_COLORSTEP_H_