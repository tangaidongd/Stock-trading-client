#ifndef _IXSCROLLBARFORGRID_H_
#define _IXSCROLLBARFORGRID_H_

// XScrollBar Grid行为接口

class iXScrollBarForGrid
{
public:
	// 滚动行为
	virtual int		GetScrollPos() const = 0;
	virtual int		SetScrollPos(int nPos, BOOL bRedraw = TRUE) = 0;
	virtual void	SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw = TRUE) = 0;
	virtual	void	GetScrolRange(int& iMin, int& iMax) = 0;

	// 使能行为
	virtual BOOL	EnableBarWindow(BOOL bEnable) = 0;

	// 是否有效
	BOOL	IsSafeValid(){ return NULL!=this; };
};

#endif //!_IXSCROLLBARFORGRID_H_