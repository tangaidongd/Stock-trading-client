#pragma once
#include "afxwin.h"
#include "StaticEx.h"
#include "EditFlat.h"

// CDlgEconomicData 对话框

// 财经日历
typedef struct _EconomicData
{
	bool bBoth;					// true: 利多、利空都有   false：只有利多或利空中的一项
	int nNewID;					//
	std::string release_time;	// 发布时间
	std::string content;		// 指标内容
	std::string former_value;	// 前值
	std::string forecast_value;	// 预期
	std::string actual_value;	// 实际
	std::string strLK;			// 利空
	std::string strLD;			// 利多
	std::string strPlaceholder;	// 利多或利空返回的无效字符串（目前返回的是：--）
	std::string country_type;	// 国别类型
	std::string important_level;// 重要等级
	
	_EconomicData()
	{
		bBoth = false;
		nNewID = 0;
	}
}T_EconomicData;

class CDlgEconomicData : public CDialog
{
	DECLARE_DYNAMIC(CDlgEconomicData)

public:
	CDlgEconomicData(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgEconomicData();

	Image *m_imgCountry;
	Image *m_imgLevel;

	bool m_bAll;// 利空和利多是不是都有

	CString m_strLD, m_strLDName;// 利多
	CString m_strLK, m_strLKName;// 利空
	CString m_strPlaceholder;// 利空或利多显示位置的占位符
	CStaticEx m_label_time;
	CStaticEx m_lable_former;
	CStaticEx m_lable_former_value;
	CStaticEx m_lable_forecast;
	CStaticEx m_lable_forecast_value;
	CStaticEx m_lable_actual;
	CStaticEx m_lable_actual_valule;

	CString m_strContent;

	CFont m_font_content;
	
	void InitData(T_EconomicData &stData);
	void LoadCountryIcon(CString strFileName);
	void LoadLevelIcon(CString strFileName);
	void SaveLDorLK(CString &str);

	void DrawLD(Graphics &graphic, CPaintDC &dc);// 绘制利多
	void DrawLK(Graphics &graphic, CPaintDC &dc);// 绘制利空
	void DrawPlaceholder(Graphics &graphic, CPaintDC &dc);// 绘制占位符


// 对话框数据
	enum { IDD = IDD_DIALOG_ECONOMIC_DATA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

};
