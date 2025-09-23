#include "stdafx.h"
#include "GridCellSys.h"
#include "GridCtrl.h"
#include "facescheme.h"
#include "sharefun.h"

#include "GridCellLevel2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGridCellLevel2, CGridCellSys)


CGridCellLevel2::CGridCellLevel2()
{
	m_eShowType		= CIoViewLevel2::ESTBrokerCode;
	m_StrTipTitle	= L"Level2详情";
}

void CGridCellLevel2::SetContent(/*float fVolume,*/ const CString &StrBrokerCode, const CString &StrBrokerName, CIoViewLevel2::E_ShowType eShowType)
{
// 	m_fVolume			= fVolume;
	m_StrBrokerCode		= StrBrokerCode;
	m_StrBrokerName		= StrBrokerName;
	
	m_eShowType			= eShowType;
	
	// 生成显示用的字符串
	if (CIoViewLevel2::ESTBrokerCode == m_eShowType)
	{
		SetText(m_StrBrokerCode);
		SetDefaultTextColor(ESCText);
	}
	else if (CIoViewLevel2::ESTBrokerName == m_eShowType)
	{
		SetText(m_StrBrokerName);
		SetDefaultTextColor(ESCText);
	}
// 	else
// 	{	
// 		SetText(Float2String(m_fVolume, 0, true));
// 		SetDefaultTextColor(ESCVolume);
//	}

	// 生成tip字符串
	// CString StrVolume = Float2String(m_fVolume, 0, true);
	m_StrTipText.Format(L"号: %s\n 名: %s\n", m_StrBrokerCode.GetBuffer(), m_StrBrokerName.GetBuffer()/*, StrVolume*/);
}
void CGridCellLevel2::SetContent(CString StrContent)
{
	//m_StrTipText.Format(L"%s",StrContent);
	m_StrTipText = StrContent;
}
LPCTSTR CGridCellLevel2::GetTipText() const
{
	return m_StrTipText;
}
void CGridCellLevel2::SetTiPTitle(CString StrTipTitle)
{
	m_StrTipTitle = StrTipTitle;
}
LPCTSTR CGridCellLevel2::GetTipTitle() const
{
	return m_StrTipTitle;
}