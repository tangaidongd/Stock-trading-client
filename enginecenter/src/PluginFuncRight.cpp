#include "StdAfx.h"
#include "PluginFuncRight.h"
#include "proxy_auth_client_base.h"
using namespace auth;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
CPluginFuncRight::CPluginFuncRight()
{
	m_hWndShowTip = NULL;
    m_iUserGroupType = 1;
	Clear();
}

CPluginFuncRight::~CPluginFuncRight()
{
	Clear();
}

void	CPluginFuncRight::ClearRightTip()
{
	m_arrRightTips.clear();
}

void	CPluginFuncRight::ClearRight()
{
	m_arrUserRights.clear();
}

void CPluginFuncRight::Clear()
{
	ClearRightTip();
	ClearRight();
}

void CPluginFuncRight::SetShowTipWnd(HWND hWnd, int iUserGroupType)
{
	m_hWndShowTip = hWnd;
    m_iUserGroupType = iUserGroupType;
}

void CPluginFuncRight::AddUserRight(const T_UserRightInfo& stUserRight)
{
	m_arrUserRights.push_back(stUserRight);
}

void CPluginFuncRight::AddRightTips(const T_RightPromptCode& stRightTip)
{
	m_arrRightTips.push_back(stRightTip);
}

bool32 CPluginFuncRight::IsUserHasRight(int32 iRightCode, bool32 bShowTip)
{
	CString StrRightName = GetUserRightName(iRightCode);

	return IsUserHasRight(StrRightName, bShowTip);
}

bool32 CPluginFuncRight::IsUserHasRight(CString StrRightName, bool32 bShowTip, bool32 bAddUserDealRecord)
{
	int32 iRightCode = -1;	// 权限号
	bool32 bRight = true; // 是否有权限, 默认有此权限

	for (UserRightInfoArray::const_iterator itRight=m_arrUserRights.begin(); itRight!=m_arrUserRights.end(); ++itRight)
	{
		if (itRight->StrRightName == StrRightName)
		{
			bRight = itRight->bRight;
			iRightCode = itRight->iRightCode;

			if (bAddUserDealRecord && m_hWndShowTip != NULL)
			{
				// 点击了功能权限列表里的指标，添加到用户行为记录里
				CString *pFunName = new CString(StrRightName);
				PostMessage(m_hWndShowTip, UM_User_Deal_Record, (WPARAM)pFunName, 0);
			}
			break;
		}
	}

	// 工作人员拥有所有权限
	if(m_iUserGroupType == 2)
	{
		return true;
	}

	// 无权限
	if (!bRight)
	{
		if (bShowTip)
		{
			ShowPromptDialog(iRightCode);
		}
	}

	return bRight;
}

CString CPluginFuncRight::GetUserRightName(int32 iRightCode)
{	
	for(UserRightInfoArray::size_type i = 0; i < m_arrUserRights.size(); ++i)
	{
		if (m_arrUserRights[i].iRightCode == iRightCode)
		{
			return m_arrUserRights[i].StrRightName;
		}
	}
	return CString();
}

int32 CPluginFuncRight::GetUserRightCode(CString StrRightName)
{
	for(UserRightInfoArray::size_type i = 0; i < m_arrUserRights.size(); ++i)
	{
		if (m_arrUserRights[i].StrRightName == StrRightName)
		{
			return m_arrUserRights[i].iRightCode;
		}
	}
	return -1;
}

CPluginFuncRight & CPluginFuncRight::Instance()
{
	static CPluginFuncRight right;
	return right;
}

void CPluginFuncRight::ShowPromptDialog(int32 iRightCode)
{
	if (m_hWndShowTip != NULL)
	{
		bool32 bFind = false;
		T_RightPromptCode *pstRightTip = new T_RightPromptCode();
		for (RightPromptCodeArray::const_iterator itRightTip=m_arrRightTips.begin(); itRightTip!=m_arrRightTips.end(); ++itRightTip)
		{
			if (itRightTip->iRightCode == iRightCode)
			{
				bFind = true;
				*pstRightTip = *itRightTip;
				break;
			}
		}

		// 两个地址都为空，则messagebox提示
		if (pstRightTip->StrImgUrl.IsEmpty() && pstRightTip->StrLinkUrl.IsEmpty())
		{
			bFind = false;
		}

		// 没有在提示列表中找到对应权限号，默认提示框提示
		if (!bFind)
		{
			CString	StrPrompt = _T("");
			StrPrompt.Format(_T("当前用户不具备[%s]权限，请联系客服人员。"), GetUserRightName(iRightCode).GetBuffer());
			pstRightTip->StrMsgTip = StrPrompt;
		}
		
		// 在外面释放pstRightTip
		PostMessage(m_hWndShowTip, UM_Right_ShowTip, (WPARAM)pstRightTip, 0);
	}
}

bool32 CPluginFuncRight::IsUserHasIndexRight( CString StrIndexName, bool32 bShowTip/* = false*/ )
{
	// 比对指标StrIndexName是否超过了有效期
// 	vector<CString>::iterator iter;
// 	for (iter = m_vecIndexName.begin() ; iter != m_vecIndexName.end() ; ++iter)
// 	{
// 		if ((*iter).CompareNoCase(StrIndexName) == 0)
// 		{
			return IsUserHasRight(StrIndexName, bShowTip);
// 		}
// 	}
	
	// 如果没有配置要求权限，则默认是有权限打开该指标的
//	return true;
}

bool32 CPluginFuncRight::IsUserHasSaneIndexRight( bool32 bShowDlg )
{
	CString StrSaneIndexName = _T("金盾指标");
	return IsUserHasIndexRight(StrSaneIndexName, bShowDlg);
}

CString CPluginFuncRight::GetSaneIndexShowName()
{
	CString StrSaneIndexName = _T("金盾指标");
	return StrSaneIndexName;
}

void CPluginFuncRight::AddIndexRight(CString StrIndexName)
{
	m_vecIndexName.push_back(StrIndexName);
}
