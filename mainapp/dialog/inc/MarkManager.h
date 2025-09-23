#ifndef _MARKMANAGER_H_
#define _MARKMANAGER_H_

#include "MerchManager.h"
#include "tinystr.h"
#include "tinyxml.h"

// ��ǽṹ�����

enum E_MarkType
{
	EMT_Text = 0,	// �������
	EMT_1,			// ���1,2...
	EMT_2,
	EMT_3,
	EMT_4,
	EMT_5,
	EMT_6,

	EMT_Count		// ����
};

struct T_MarkData
{
	E_MarkType m_eType;	// �������
	CString	   m_StrTip;	// Ӧ����ʾ������, ��Ӧ����1...��ʾΪ��..., ���ֱ��Ϊ�û����õı��(һ�㲻��Ϊ��)

	T_MarkData(){ m_eType = EMT_Count; }

	T_MarkData(E_MarkType eType)
	{
		m_eType = eType;
		switch ( m_eType )
		{
		case EMT_1:
			m_StrTip = _T("��");
			break;
		case EMT_2:
			m_StrTip = _T("��");
			break;
		case EMT_3:
			m_StrTip = _T("��");
			break;
		case EMT_4:
			m_StrTip = _T("��");
			break;
		case EMT_5:
			m_StrTip = _T("��");
			break;
		case EMT_6:
			m_StrTip = _T("��");
			break;
		case EMT_Text:
			break;	// ���ֱ�ǲ�����
		case EMT_Count:
			break;	// ��Ч
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

// ֪ͨ�仯�ӿ�
class CMerchMarkChangeListener
{
public:
	virtual ~CMerchMarkChangeListener();

	// ��������Ѿ�����������ǰ��markOldData��EMT_Count������ӣ������ǰ�У�����û�о���ɾ�����������Ǳ������
	virtual void OnMerchMarkChanged(int32 iMarketId, const CString &StrMerchCode, const T_MarkData &MarkOldData) = 0;
};

class CMerchKey;
// ������
class CMarkManager
{
public:
	typedef CMap<CMerch *, CMerch *, T_MarkData, const T_MarkData &> MarkMap;
	typedef CMap<CString, LPCTSTR, T_MarkData, const T_MarkData &> MarkIdStringMap;
public:
	static CMarkManager &Instance();	// ��ʵ��

	
	bool32	QueryMark(CMerch *pMerch, OUT T_MarkData &MarkData) const;	// ��ѯ�Ƿ����Ʒ�Ƿ���ڱ��
	bool32	QueryMark(const CMerchKey &MerchKey, OUT T_MarkData &MarkData) const;
	bool32	QueryMark(int32 iMarketId, const CString &StrMerchCode, OUT T_MarkData &MarkData) const;

	bool32	SetMark(CMerch *pMerch, const T_MarkData &MarkData);	// ���ø���Ʒ�ı��
	bool32	SetMark(CMerch *pMerch, E_MarkType eType);				// ���ñ��1...
	bool32	SetMark(int32 iMarketId, const CString &StrMerchCode, const T_MarkData &MarkData);
	bool32	SetMark(int32 iMarketId, const CString &StrMerchCode, E_MarkType eType);

	void	RemoveMark(CMerch *pMerch);		// ɾ����Ʒ�ı��
	void	RemoveMark(int32 iMarketId, const CString &StrMerchCode);

	bool32	RebuildMerchMark();			// ��ԭʼ�������ؽ���ǰ���ڵ���Ʒ�������
	const MarkMap &GetMarkMap();	// ��ȡ������Ʒ�ı�ǣ��������ر��ı�ǣ����˽ӿ�Ӧ�ÿ�������

	// �������
	void	AddMerchMarkChangeListener(CMerchMarkChangeListener *pListener);
	void	RemoveMerchMarkChangeListener(CMerchMarkChangeListener *pListener);

private:
	CMarkManager();
	CMarkManager(const CMarkManager &);
	const CMarkManager &operator=(const CMarkManager &);

	bool32	LoadFromXml();	// ����
	bool32	SaveToXml();	// ����

	bool32	UpdateMerchMap(int32 iMarketId, const CString &StrMerchCode, const T_MarkData &MarkData);	// ���´�����Ʒ�ı����Ϣ�����marktypeΪcount��ɾ��
	bool32  UpdateMerchMap(CMerch *pMerch, const T_MarkData &MarkData);

	bool32	UpdateXmlValue(const CString &StrId, const T_MarkData &MarkData, bool32 bSave = true);	// ����xml�е�ֵ
	void	DeleteXmlValue(const CString &StrId, bool32 bSave = true);	// ɾ��xml�е�ֵ

	void	FireChange(int32 iMarketId, const CString &StrMerchCode, const T_MarkData &MarkOldData);

	MarkIdStringMap	m_mapIdMarks;	// ԭʼ�ı������
	MarkMap	m_mapMarks;		// �����ڵ���Ʒӵ�б�־������������
	TiXmlDocument	m_TiDoc;	// �ĵ�
	bool32	m_bInitedMerchMap;	// �Ƿ��Ѿ���ʼ���˴�����Ʒ�ı��
	CArray<CMerchMarkChangeListener *, CMerchMarkChangeListener *> m_aListeners;
};


#endif //_MARKMANAGER_H_