#ifndef _LEFT_BAR_MANAGER_H_
#define _LEFT_BAR_MANAGER_H_

#include <map>
#include <vector>

using std::map;
using std::vector;

// ���������
enum E_LeftBarType
{
	ELBT_NONE,
	ELBT_StockCN,								 // ����A��   
	ELBT_StockHK,                                // �۹�
	ELBT_Futures,                                // �ڻ�
	ELBT_GlobalIndex,                            // ȫ��ָ��
	ELBT_QuoteForeign,                           // ���
	ELBT_SmartSelStock,                          //����ѡ��
	ELBT_SmartSelStock1,						 // ����ѡ��
	ELBT_SmartSelStock2,						 // ����ѡ��
	ELBT_SmartSelStock3,						 // ����ѡ��
	ELBT_StockDecision,							 // ���ɾ���
};

// ��ťͼƬ����
enum E_ImageType
{

	EIT_HQBJ,				    // ���鱨��				
	EIT_JBCW,					// ��������
	EIT_ZHPM,				    // �ۺ�����
	EIT_DGTL,					// ���ͬ��
	EIT_ZJPM,					// �ʽ�����
	EIT_JGCC,					// �����ֲ�
	EIT_LHBD,					// ������
	EIT_JYXW,					// ������Ϊ
	EIT_YJTS,					// ҵ��͸��
	EIT_XGIPO,					// �¹�IPO
	EIT_RMBK,					// ���Ű��
	EIT_CPTX,					// ��������
	EIT_RDWJ,					// �ȵ��ھ�
	EIT_ZLJK,					// �������
	EIT_TREND,					// ��ʱͼ
	EIT_KLINE,					// K��ͼ
	EIT_CJMX,					// �ɽ���ϸ
	EIT_FJB,					// �ּ۱�
	EIT_ZJLX,					// �ʽ�����
	EIT_DZQT,					// ������ͼ
	EIT_F10,					// F10
	EIT_GGZX,					// ������Ѷ
	EIT_WXLH,					// ��������
	EIT_GGYJ,					// ����Ԥ��
	EIT_GGCD,					// ���ɳ���
	EIT_GGTD,					// �����Ӷ�
	EIT_YJSD,					// һ������
};

enum E_BtnID
{
	// ����A��
	EBID_StockCN_HQBJ = 10000,                      // ���鱨��
	EBID_StockCN_JBCW,							    // ��������
	EBID_StockCN_ZHPM,							    // �ۺ�����					
	EBID_StockCN_DGTL,								// ���ͬ��
	EBID_StockCN_ZJJM,								// �ʽ����

	// �۹�
	EBID_StockHK_HQBJ = 10100,	                    // ���鱨��                			
	EBID_StockHK_DGTL,								// ���ͬ��

	// �ڻ�
	EBID_Future_HQBJ = 10200,						// ���鱨��
	EBID_Future_DGTL,								// ���ͬ�� 

	// ȫ��ָ��
	EBID_GlobalIndex_HQBJ = 10400,					// ���鱨��
	EBID_GlobalIndex_DGTL,							// ���ͬ��

	// ���
	EBID_QuoteForeign_HQBJ = 10500,					// ���鱨��
	EBID_QuoteForeign_DGTL,							// ���ͬ��

	// ����ѡ��
	EBID_SmartSelStock_YJFX = 10600,				// 120-����ǿ��-һ������-�ȵ�׷��
	EBID_SmartSelStock_QYZS,						// 121-ǿ�ߺ�ǿ-����ֱ��-������ţ
	EBID_SmartSelStock_SLXS,						// 122 ���β���-������ˮ-��ͣԤ��
	EBID_SmartSelStock_SLNT,		                // ��������
	EBID_SmartSelStock_XZDF,						// 119 �𵴶���-��ׯ��-������
	EBID_SmartSelStock_HFFL,						// 118 ��ͷ����-�ط����-���׷���ʦ
	EBID_SmartSelStock_SHJD,						// 117 �����۶�-CCTV���
	EBID_SmartSelStock_SLQF,						// 116 �ط����-˫�����

	// ���ɾ���
	EBID_StockDecision_XLFD = 10700,				// С��ɵ� -���ƾѻ�
	EBID_StockDecision_CHGR,						// ������� -��ʤ���
	EBID_StockDecision_XRZL,						// ����ָ·
	EBID_StockDecision_YYLJ,						// ��������
	EBID_StockDecision_MSKK,						// ���ֿտ�
	EBID_StockDecision_SJHY,						// ������һ
	EBID_StockDecision_DXCP,						// ���߲���
	EBID_StockDecision_ZXCP,						// ���߲���
	EBID_StockDecision_QRFJ,						// ǿ���ֽ�
	EBID_StockDecision_JGCB,						// �����ɱ�
	EBID_StockDecision_JDZB,						// ���ָ��
};

struct T_ButtonItem
{
	CString m_StrName;
	int32   m_iBtnID;
	CString m_StrWspName;
	CString m_StrDesc;
	bool32  m_bHasResult;
	bool32  m_bChecked;

	T_ButtonItem(int32 iID, CString StrName, CString StrWspName, bool32 bChecked=true, CString StrDesc=L"", bool32 hasResult = false)
	{
		m_StrName = StrName;
		m_iBtnID  = iID;
		m_StrWspName = StrWspName;
		m_bHasResult = hasResult;
		m_StrDesc    = StrDesc;
		m_bChecked   = bChecked;
	}
};

class CLeftBarItemManager
{
public:
	static CLeftBarItemManager &Instance();
protected:
	CLeftBarItemManager();
	~CLeftBarItemManager();

public:
	bool32 InitItemMap();
	void   InitMarketMap();
	
public:
	void GetItemsByType(E_LeftBarType eLeftBarType,OUT vector<T_ButtonItem>& vecItemInfo);
	void ChangeCheckStatus(const E_LeftBarType &eType, const int32 &iID, const CString &StrWspName);
	// ����״̬
	void ResetCheckStatus(const CString &StrWspName);
    
private:
	map<E_LeftBarType, vector<T_ButtonItem>> m_mapItemList;   // �������ť����
	map<E_ImageType, Image*> m_mapImageList;                  // �������ťͼƬ����

public:
	map<int32, E_LeftBarType> m_mapMarketList;				  // �г�����
private:
	DWORD                     m_dwChooseStockStatus;          // ѡ��״̬
};

#endif