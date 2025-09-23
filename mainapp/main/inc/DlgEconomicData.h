#pragma once
#include "afxwin.h"
#include "StaticEx.h"
#include "EditFlat.h"

// CDlgEconomicData �Ի���

// �ƾ�����
typedef struct _EconomicData
{
	bool bBoth;					// true: ���ࡢ���ն���   false��ֻ������������е�һ��
	int nNewID;					//
	std::string release_time;	// ����ʱ��
	std::string content;		// ָ������
	std::string former_value;	// ǰֵ
	std::string forecast_value;	// Ԥ��
	std::string actual_value;	// ʵ��
	std::string strLK;			// ����
	std::string strLD;			// ����
	std::string strPlaceholder;	// ��������շ��ص���Ч�ַ�����Ŀǰ���ص��ǣ�--��
	std::string country_type;	// ��������
	std::string important_level;// ��Ҫ�ȼ�
	
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
	CDlgEconomicData(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgEconomicData();

	Image *m_imgCountry;
	Image *m_imgLevel;

	bool m_bAll;// ���պ������ǲ��Ƕ���

	CString m_strLD, m_strLDName;// ����
	CString m_strLK, m_strLKName;// ����
	CString m_strPlaceholder;// ���ջ�������ʾλ�õ�ռλ��
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

	void DrawLD(Graphics &graphic, CPaintDC &dc);// ��������
	void DrawLK(Graphics &graphic, CPaintDC &dc);// ��������
	void DrawPlaceholder(Graphics &graphic, CPaintDC &dc);// ����ռλ��


// �Ի�������
	enum { IDD = IDD_DIALOG_ECONOMIC_DATA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

};
