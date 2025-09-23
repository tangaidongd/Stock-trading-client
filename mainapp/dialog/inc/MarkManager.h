#ifndef _MARKMANAGER_H_
#define _MARKMANAGER_H_

#include "MerchManager.h"
#include "tinystr.h"
#include "tinyxml.h"

// 标记结构与管理

enum E_MarkType
{
	EMT_Text = 0,	// 标记文字
	EMT_1,			// 标记1,2...
	EMT_2,
	EMT_3,
	EMT_4,
	EMT_5,
	EMT_6,

	EMT_Count		// 结束
};

struct T_MarkData
{
	E_MarkType m_eType;	// 标记类型
	CString	   m_StrTip;	// 应当显示的文字, 对应与标记1...显示为①..., 文字标记为用户设置的标记(一般不会为空)

	T_MarkData(){ m_eType = EMT_Count; }

	T_MarkData(E_MarkType eType)
	{
		m_eType = eType;
		switch ( m_eType )
		{
		case EMT_1:
			m_StrTip = _T("①");
			break;
		case EMT_2:
			m_StrTip = _T("②");
			break;
		case EMT_3:
			m_StrTip = _T("③");
			break;
		case EMT_4:
			m_StrTip = _T("④");
			break;
		case EMT_5:
			m_StrTip = _T("⑤");
			break;
		case EMT_6:
			m_StrTip = _T("⑥");
			break;
		case EMT_Text:
			break;	// 文字标记不设置
		case EMT_Count:
			break;	// 无效
		default:
			ASSERT( 0 );
			m_eType = EMT_Count;
		}
	}

	bool32 operator==(const T_MarkData &MarkData) const
	{
		return MarkData.m_eType == m_eType
			&& MarkData.m_StrTip == m_StrTip;
	}
	bool32 operator!=(const T_MarkData &MarkData) const
	{
		return !(*this==MarkData);
	}
};

// 通知变化接口
class CMerchMarkChangeListener
{
public:
	virtual ~CMerchMarkChangeListener();

	// 标记数据已经变更，如果以前的markOldData是EMT_Count则是添加，如果以前有，现在没有就是删除，其余则是变更类型
	virtual void OnMerchMarkChanged(int32 iMarketId, const CString &StrMerchCode, const T_MarkData &MarkOldData) = 0;
};

class CMerchKey;
// 管理类
class CMarkManager
{
public:
	typedef CMap<CMerch *, CMerch *, T_MarkData, const T_MarkData &> MarkMap;
	typedef CMap<CString, LPCTSTR, T_MarkData, const T_MarkData &> MarkIdStringMap;
public:
	static CMarkManager &Instance();	// 单实例

	
	bool32	QueryMark(CMerch *pMerch, OUT T_MarkData &MarkData) const;	// 查询是否该商品是否存在标记
	bool32	QueryMark(const CMerchKey &MerchKey, OUT T_MarkData &MarkData) const;
	bool32	QueryMark(int32 iMarketId, const CString &StrMerchCode, OUT T_MarkData &MarkData) const;

	bool32	SetMark(CMerch *pMerch, const T_MarkData &MarkData);	// 设置该商品的标记
	bool32	SetMark(CMerch *pMerch, E_MarkType eType);				// 设置标记1...
	bool32	SetMark(int32 iMarketId, const CString &StrMerchCode, const T_MarkData &MarkData);
	bool32	SetMark(int32 iMarketId, const CString &StrMerchCode, E_MarkType eType);

	void	RemoveMark(CMerch *pMerch);		// 删除商品的标记
	void	RemoveMark(int32 iMarketId, const CString &StrMerchCode);

	bool32	RebuildMerchMark();			// 从原始数据中重建当前存在的商品标记数据
	const MarkMap &GetMarkMap();	// 获取存在商品的标记，不会有特别多的标记，仅此接口应该可以满足

	// 变更侦听
	void	AddMerchMarkChangeListener(CMerchMarkChangeListener *pListener);
	void	RemoveMerchMarkChangeListener(CMerchMarkChangeListener *pListener);

private:
	CMarkManager();
	CMarkManager(const CMarkManager &);
	const CMarkManager &operator=(const CMarkManager &);

	bool32	LoadFromXml();	// 加载
	bool32	SaveToXml();	// 保存

	bool32	UpdateMerchMap(int32 iMarketId, const CString &StrMerchCode, const T_MarkData &MarkData);	// 更新存在商品的标记信息，如果marktype为count则删除
	bool32  UpdateMerchMap(CMerch *pMerch, const T_MarkData &MarkData);

	bool32	UpdateXmlValue(const CString &StrId, const T_MarkData &MarkData, bool32 bSave = true);	// 更新xml中的值
	void	DeleteXmlValue(const CString &StrId, bool32 bSave = true);	// 删除xml中的值

	void	FireChange(int32 iMarketId, const CString &StrMerchCode, const T_MarkData &MarkOldData);

	MarkIdStringMap	m_mapIdMarks;	// 原始的标记数据
	MarkMap	m_mapMarks;		// 仅存在的商品拥有标志，其它的抛弃
	TiXmlDocument	m_TiDoc;	// 文档
	bool32	m_bInitedMerchMap;	// 是否已经初始化了存在商品的标记
	CArray<CMerchMarkChangeListener *, CMerchMarkChangeListener *> m_aListeners;
};


#endif //_MARKMANAGER_H_