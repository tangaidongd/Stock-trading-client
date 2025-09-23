#ifndef _IOVIEW_COWBEAR_H_
#define _IOVIEW_COWBEAR_H_

#include "PlugInStruct.h"

// ţ�ֲܷ�״̬ - ����ţ�ֽܷ�������´���ţ��״̬
class CCowBearState : public CWnd
{
public:
	DECLARE_DYNAMIC(CCowBearState)
	friend class CViewData;

	~CCowBearState();

	static CCowBearState &Instance();

	E_DpnxState	GetCurrentCowBearState() const { return m_eCowBearState; }
	void		SetCurrentCowBearState(E_DpnxState eState) { m_eCowBearState = eState; }
	
	E_DpnxState	m_eCowBearState;

protected:
	CCowBearState();
	
	CCowBearState(const CCowBearState&);
	CCowBearState &operator=(const CCowBearState &);

	void	RequestViewData();

	//{{AFX_MSG(CCowBearState)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
};

#endif