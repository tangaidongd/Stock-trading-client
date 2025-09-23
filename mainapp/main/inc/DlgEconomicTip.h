#pragma once
#include <map>
#include <vector>
#include "DlgEconomicData.h"
#define CHILD_HEIGHT_V1 120
#define CHILD_HEIGHT_V2 125
#define CHILD_WIDTH 335


// CDlgEconomicTip �Ի���

typedef struct _EconoInfo
{
	int singleCount;// ͳ��vecEconomicDataֻ���������������һ�����Ŀ
	int bothCount;// ͳ��vecEconomicData��������Ҳ�����յ���Ŀ
	std::vector<T_EconomicData> vecEconomicData;
}T_EconoInfo;

class CDlgEconomicTip : public CDialog
{
	DECLARE_DYNAMIC(CDlgEconomicTip)

public:
	CDlgEconomicTip(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgEconomicTip();

	BOOL m_bTracking;
	BOOL m_bOver;

	// first: ����ʱ�䣨��ʱ����С�ڵ���3���ӵĲƾ��������ݣ�����Ϊһ������ʱ�䣩
	// second���ƾ���������
	std::map<CString, T_EconoInfo> m_mapEconomicData;

	// �ƾ�������Ϣ����
	int m_count;

	// ÿ���ƾ����ݣ��ڴ�ֱ�����ϵ�ƫ����
	int m_offsetV;

	// first: ͼ������  second: ͼ����ļ���
	std::map<CString, CString> m_mapCountryIcon;// �洢����ͼ���ļ�
	std::map<CString, CString> m_mapLevelIcon;// �洢��Ҫ�ȼ�ͼ���ļ�

	std::vector<CDlgEconomicData*> m_vecChildWindow;

	
	void InitChildWindow(int iCount, std::vector<T_EconomicData> &vecData);
	void ReleaseChildWindow();


// �Ի�������
	enum { IDD = IDD_DIALOG_ECONOMIC_TIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
};
