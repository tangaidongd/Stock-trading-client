#include "stdafx.h"

#include "StockSelectManager.h"
#include "PluginFuncRight.h"


//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
CStockSelectManager::CStockSelectManager()
{
	m_eCurrentSelect = IndexShortMidSelect;

	m_dwIndexSelectStockFlag = 0;//ShortLine;
// 	m_dwStrategySelectStockFlag = StrategyJGLT;
// 	m_dwIndexStrategySelectFlag = RedStrategy;
	m_dwHotSelectStockFlag  = 0;//HotTimeSale; 
}

///////////////////////////////////////////////////////////////////////////////
// 
CStockSelectManager::~CStockSelectManager()
{
	
}

CStockSelectManager	&CStockSelectManager::Instance()
{
	static CStockSelectManager sManager;
	static bool32 bInitHot = false;			// 给一个初始化，这样还是不怎么确定
	static bool32 bInitIndex = false;
	if ( sManager.GetHotSelectStockFlag() == 0 && sManager.IsReqUnderUserRight(HotSelect, HotTimeSale) && !bInitHot )
	{
		sManager.SetHotSelectStockFlag(HotTimeSale);
		bInitHot = true;
	}
	if ( sManager.GetIndexSelectStockFlag() == 0 && sManager.IsReqUnderUserRight(IndexShortMidSelect, ShortLine) && !bInitIndex )
	{
		sManager.SetIndexSelectStockFlag(StrategyDXQS);
		bInitIndex = true;
	}
	return sManager;
}
/*
	error 568: (Warning -- non-negative quantity is never less than zero)
*/
//lint --e{568}
void CStockSelectManager::SetCurrentReqType( E_CurrentSelect eSel, bool32 bAlarmIfNotRight/* = false*/, bool32 bForceFire /*= false*/ )
{
	int eRightPrompt;
	if ( eSel > HotSelect || eSel < IndexShortMidSelect )
	{
		eSel = IndexShortMidSelect;
	}

	if ( !IsCurrentReqUnderUserRight(eSel, &eRightPrompt) )
	{
		if ( bAlarmIfNotRight )
		{
			PromptLackRight(eRightPrompt);
		}
		return;
	}
	E_CurrentSelect eOld = m_eCurrentSelect;

	
	ASSERT( IsCurrentReqUnderUserRight() );		// 必须在进入这个之前判断出权限
	if ( m_eCurrentSelect > HotSelect || m_eCurrentSelect < IndexShortMidSelect )
	{
		ASSERT( 0 );
		m_eCurrentSelect = IndexShortMidSelect;
	}

	// 通知
	if ( bForceFire || eSel != eOld )
	{
		m_eCurrentSelect = eSel;
		FireChanged();
	}
}

bool32 CStockSelectManager::IsCurrentReqUnderUserRight(int *pRightDeny)
{
	return IsCurrentReqUnderUserRight(m_eCurrentSelect, pRightDeny);
}

bool32 CStockSelectManager::IsCurrentReqUnderUserRight( E_CurrentSelect eSelectType, int *pRightDeny )
{
	if ( IndexShortMidSelect == eSelectType )
	{
		return IsReqUnderUserRight(eSelectType, GetIndexSelectStockFlag(), pRightDeny);
	}
	else
	{
		return IsReqUnderUserRight(eSelectType, GetHotSelectStockFlag(), pRightDeny);
	}
}

bool32 CStockSelectManager::IsDefaultReqUnderUserRight(int *pRightDeny)
{
	return IsReqUnderUserRight(IndexShortMidSelect, ShortLine, pRightDeny);
}

bool32 CStockSelectManager::IsReqUnderUserRight(E_CurrentSelect eSelectType, DWORD dwFlag, int *pRightDeny)
{
	if ( dwFlag == 0 )
	{
		return true;
	}
	// 短线 中线 买入精选 请求   && 热门强势请求
	CPluginFuncRight::E_FuncRight eRight = CPluginFuncRight::FuncEnd;
	if ( NULL != pRightDeny )
	{
		*pRightDeny = eRight;
	}
	CArray<CPluginFuncRight::E_FuncRight, CPluginFuncRight::E_FuncRight> aRigths;

	if ( IndexShortMidSelect == eSelectType )
	{
		// 并集
		DWORD dwSel = dwFlag;

		if ( dwSel & ShortLine )
		{
			eRight = CPluginFuncRight::FuncShortSelectStock;
			aRigths.Add(eRight);
		}
		if ( dwSel & MidLine )
		{
			eRight = CPluginFuncRight::FuncMidSelectStock;
			aRigths.Add(eRight);
		}
		if ( dwSel & BuySelect )
		{
			eRight = CPluginFuncRight::FuncBuySelectStock;
			aRigths.Add(eRight);
		}
		if ( dwSel & StrategyJGLT )	// 机构拉抬
		{
			eRight = CPluginFuncRight::FuncJiGouLaTai;
			aRigths.Add(eRight);
		}
		if ( dwSel & StrategyXPJS )	// 机构拉抬
		{
			eRight = CPluginFuncRight::FuncXiPanJieShu;
			aRigths.Add(eRight);
		}
		if ( dwSel & StrategyYJZT )	// 追击涨停
		{
			eRight = CPluginFuncRight::FuncZhuiJiZhangTing;
			aRigths.Add(eRight);
		}
		if ( dwSel & StrategyNSQL )	// 逆势强龙
		{
			eRight = CPluginFuncRight::FuncNiShiQiangLong;
			aRigths.Add(eRight);
		}
		if ( dwSel & StrategyCDFT )	// 超跌反弹
		{
			eRight = CPluginFuncRight::FuncChaoDieFanTan;
			aRigths.Add(eRight);
		}
		if ( dwSel & StrategySLXS ) // 神龙吸水
		{
			eRight = CPluginFuncRight::FuncShenLongXiShui;
			aRigths.Add(eRight);
		}
		if ( dwSel & StrategyHFFL ) // 回风拂柳
		{
			eRight = CPluginFuncRight::FuncHuiFengFuLiu;
			aRigths.Add(eRight);
		}
		if ( dwSel & StrategySHJD ) // 三花聚顶
		{
			eRight = CPluginFuncRight::FuncSanHuaJuDing;
			aRigths.Add(eRight);
		}
		// 多余的金盾选股待权限 TODO
	}
	else if ( HotSelect == eSelectType )
	{
		if ( dwFlag & HotTimeSale )	// 大单强势
		{
			eRight = CPluginFuncRight::FuncDaDanQiangShi;
			aRigths.Add(eRight);
		}
		if ( dwFlag & HotForce )	// 热门强势
		{
			eRight = CPluginFuncRight::FuncReMenQiangShi;
			aRigths.Add(eRight);
		}
		if ( dwFlag & HotCapital )	// 资金强势
		{
			eRight = CPluginFuncRight::FuncZiJinQiangShi;
			aRigths.Add(eRight);
		}
	}

	for ( int32 i=0; i < aRigths.GetSize() ; i++ )
	{
		if ( !CPluginFuncRight::Instance().IsUserHasRight(aRigths[i]) )
		{
			if ( NULL != pRightDeny )
			{
				*pRightDeny = aRigths[i];
				//*pStrDeny = CPluginFuncRight::Instance().GetDenyPromptString(aRigths[i]);
			}
			return false;
		}
	}

	return true;
}

void CStockSelectManager::PromptLackRight( int iRightDeny )
{
	CPluginFuncRight::Instance().ShowPromptDialog(iRightDeny);
}

bool32 CStockSelectManager::SetIndexSelectStockFlag( DWORD dwIndexFlag, bool32 bAlarmIfNoRight/* = false*/, bool32 bForceFire /*= false*/ )
{
	// 处理大盘策略选股，因为大盘策略事实上只有一种类型的请求，统一编制为redstrategy
	if ( dwIndexFlag >= GreenStrategy )
	{
		dwIndexFlag = RedStrategy | (dwIndexFlag & 0xff);
	}

	int eRightDeny;
	if ( !IsReqUnderUserRight(IndexShortMidSelect, dwIndexFlag, &eRightDeny) )
	{
		if ( bAlarmIfNoRight )
		{
			PromptLackRight(eRightDeny);
		}
		return false;
	}
	
	DWORD dwOld = m_dwIndexSelectStockFlag;

	if ( bForceFire || dwIndexFlag != dwOld )
	{
		m_dwIndexSelectStockFlag = dwIndexFlag;
		FireChanged();
	}

	return true;
}


bool32 CStockSelectManager::SetHotSelectStockFlag( DWORD dwHotFlag, bool32 bAlarmIfNoRight /*= false*/, bool32 bForceFire /*= false*/ )
{
	int eRightDeny;
	if ( !IsReqUnderUserRight(HotSelect, dwHotFlag, &eRightDeny) )
	{
		if ( bAlarmIfNoRight )
		{
			PromptLackRight(eRightDeny);
		}
		return false;
	}

	DWORD dwOld = m_dwHotSelectStockFlag;

	if ( bForceFire || dwOld != dwHotFlag )
	{
		m_dwHotSelectStockFlag = dwHotFlag;
		FireChanged();
	}

	return true;
}


DWORD CStockSelectManager::GetIndexSelectStockFlag() const
{
	ASSERT( m_dwIndexSelectStockFlag < GreenStrategy );
	if ( m_dwIndexSelectStockFlag >= GreenStrategy )
	{
		ASSERT( 0 );
	}
	return m_dwIndexSelectStockFlag;
}

bool32 CStockSelectManager::GetIndexSelectStockEnumByCmdId( UINT nId, OUT E_IndexSelectStock &eJGXG )
{
	typedef map<UINT, E_IndexSelectStock> CmdEnumMap;
	static CmdEnumMap mapCmd;
	if ( mapCmd.empty() )
	{
		mapCmd[ID_SPECIAL_JGLT] = StrategyJGLT;	
		mapCmd[ID_SPECIAL_XPJS] = StrategyXPJS;
		mapCmd[ID_SPECIAL_ZJZT] = StrategyYJZT;
		mapCmd[ID_SPECIAL_NSQL] = StrategyNSQL;
		mapCmd[ID_SPECIAL_CDFT] = StrategyCDFT;
		mapCmd[ID_SPECIAL_QZHQ] = StrategyQZHQ;
		mapCmd[ID_SPECIAL_DXQS] = StrategyDXQS;
		mapCmd[ID_SPECIAL_SLXS] = StrategyBDCZ;
		mapCmd[ID_SPECIAL_HFFL] = StrategyKTXJ;
		mapCmd[ID_SPECIAL_SHJD] = StrategySHJD;
		mapCmd[ID_SPECIAL_ZLXG] = IndexStrategy;	// 战略选股
		mapCmd[ID_SPECIAL_XZDF] = StrategyZDSD;
		mapCmd[ID_SPECIAL_SLQF] = StrategyHFFL;		// 回风拂柳-双龙齐飞
	}
	
	CmdEnumMap::const_iterator it = mapCmd.find(nId);
	if ( it != mapCmd.end() )
	{
		eJGXG = it->second;
		return true;
	}
	return false;
}

void CStockSelectManager::AddListener( CStockSelectTypeChanged *pListener )
{
	if ( NULL != pListener )
	{
		for ( int i=0; i < m_aListeners.GetSize() ; i++ )
		{
			if ( m_aListeners[i] == pListener )
			{
				return;
			}
		}
		m_aListeners.Add(pListener);
	}
}

void CStockSelectManager::RemoveListener( CStockSelectTypeChanged *pListener )
{
	if ( NULL != pListener )
	{
		for ( int i=0; i < m_aListeners.GetSize() ; i++ )
		{
			if ( m_aListeners[i] == pListener )
			{
				m_aListeners.RemoveAt(i);
				return;
			}
		}
	}
}

void CStockSelectManager::FireChanged()
{
	for ( int i=0; i < m_aListeners.GetSize() ; i++ )
	{
		if ( m_aListeners[i] != NULL )
		{
			m_aListeners[i]->OnStockSelectTypeChanged();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
CStockSelectTypeChanged::CStockSelectTypeChanged()
{
	CStockSelectManager::Instance().AddListener(this);
}

CStockSelectTypeChanged::~CStockSelectTypeChanged()
{
	CStockSelectManager::Instance().RemoveListener(this);
}
