#ifndef _IXSCROLLBARFORGRID_H_
#define _IXSCROLLBARFORGRID_H_

// XScrollBar Grid��Ϊ�ӿ�

class iXScrollBarForGrid
{
public:
	// ������Ϊ
	virtual int		GetScrollPos() const = 0;
	virtual int		SetScrollPos(int nPos, BOOL bRedraw = TRUE) = 0;
	virtual void	SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw = TRUE) = 0;
	virtual	void	GetScrolRange(int& iMin, int& iMax) = 0;

	// ʹ����Ϊ
	virtual BOOL	EnableBarWindow(BOOL bEnable) = 0;

	// �Ƿ���Ч
	BOOL	IsSafeValid(){ return NULL!=this; };
};

#endif //!_IXSCROLLBARFORGRID_H_