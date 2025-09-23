#ifndef _IOVIEW_CHOOSESTOCK_H_
#define _IOVIEW_CHOOSESTOCK_H_

#include "PlugInStruct.h"

class  CChooseStockStateNotify
{
public:
	virtual void OnChooseStockStateResp() = 0;
};

// 选股状态 - 发送插件选股状态请求更新选股状态
class CChooseStockState : public CWnd
{
public:
	DECLARE_DYNAMIC(CChooseStockState)
	friend class CViewData;

	~CChooseStockState();;

	static CChooseStockState &Instance();

	u32	    GetCurrentChooseStockState() const { return m_uChooseStockStatus; } ;
	void	SetCurrentChooseStockState(u32 uStatus);
	void	AddChooseStateNotify(CChooseStockStateNotify *pNotify);
	void	RemoveChooseStateNotify(CChooseStockStateNotify *pNotify);
	
	u32		m_uChooseStockStatus;
	CArray<CChooseStockStateNotify *, CChooseStockStateNotify*> m_pChooseStateNotifyPtrList;

protected:
	CChooseStockState();
	
	CChooseStockState(const CChooseStockState&);
	CChooseStockState &operator=(const CChooseStockState &);

	void	RequestViewData();

	//{{AFX_MSG(CChooseStockState)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
};

#endif