#pragma once
#include <map>
#include <vector>
#include "DlgEconomicData.h"
#define CHILD_HEIGHT_V1 120
#define CHILD_HEIGHT_V2 125
#define CHILD_WIDTH 335


// CDlgEconomicTip 对话框

typedef struct _EconoInfo
{
	int singleCount;// 统计vecEconomicData只有利多或利空其中一项的数目
	int bothCount;// 统计vecEconomicData既有利多也有利空的数目
	std::vector<T_EconomicData> vecEconomicData;
}T_EconoInfo;

class CDlgEconomicTip : public CDialog
{
	DECLARE_DYNAMIC(CDlgEconomicTip)

public:
	CDlgEconomicTip(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgEconomicTip();

	BOOL m_bTracking;
	BOOL m_bOver;

	// first: 发布时间（将时间间隔小于等于3分钟的财经日历数据，将作为一个发布时间）
	// second：财经日历数据
	std::map<CString, T_EconoInfo> m_mapEconomicData;

	// 财经日历信息条数
	int m_count;

	// 每条财经数据，在垂直方向上的偏移量
	int m_offsetV;

	// first: 图标类型  second: 图标的文件名
	std::map<CString, CString> m_mapCountryIcon;// 存储国家图标文件
	std::map<CString, CString> m_mapLevelIcon;// 存储重要等级图标文件

	std::vector<CDlgEconomicData*> m_vecChildWindow;

	
	void InitChildWindow(int iCount, std::vector<T_EconomicData> &vecData);
	void ReleaseChildWindow();


// 对话框数据
	enum { IDD = IDD_DIALOG_ECONOMIC_TIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
};
