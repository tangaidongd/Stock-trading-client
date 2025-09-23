#ifndef _GRID_CELL_LEVEL2_H_
#define _GRID_CELL_LEVEL2_H_


#include "GridCellSys.h"
#include "IoViewLevel2.h"

class CGridCellLevel2 : public CGridCellSys
{
    friend class CGridCtrl;
    DECLARE_DYNCREATE(CGridCellLevel2)
public:
	CGridCellLevel2();

	// from CGridCtrlBase
public:
	virtual LPCTSTR	GetTipText() const;
	virtual LPCTSTR	GetTipTitle() const;	
	void			SetTiPTitle(CString StrTipTitle);
public:
	void			SetContent(/*float fVolume,*/ const CString &StrBrokerCode, const CString &StrBrokerName, CIoViewLevel2::E_ShowType eShowType);
	void			SetContent(CString StrContent);
private:
	//float			m_fVolume;			// 成交量
	CString			m_StrBrokerCode;	// 经济席位代号	
	CString			m_StrBrokerName;	// 经济席位名称
	CIoViewLevel2::E_ShowType     m_eShowType;
private:
	CString			m_StrTipText;
	CString			m_StrTipTitle;
};

#endif //_GRID_CELL_LEVEL2_H_



