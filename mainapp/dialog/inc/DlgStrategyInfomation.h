#pragma once
#include "afxcmn.h"
#include <map>

using namespace std;


// CDlgStrategyInfomation dialog



class CDlgStrategyInfomation : public CDialogEx
{
	//DECLARE_DYNAMIC(CDlgStrategyInfomation)

public:
	CDlgStrategyInfomation(CWnd* pParent = NULL);   // standard constructor
	CDlgStrategyInfomation(long long llStrategyTime, multimap<long long, T_StrategyInfo> *mulmapStrategyInfo, CWnd* pParent = NULL);
	virtual ~CDlgStrategyInfomation();

// Dialog Data
	enum { IDD = IDD_DIALOG_STATEGY_INFO };


public:


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

	//void   SetStategyInfo();

private:
	bool32 ConstructGrid();

	CGridCtrlSys    m_GridCtrl;
	CImageList		m_ImageList;
	CXScrollBar		m_XSBVert;
	CXScrollBar		m_XSBHorz;

	multimap<long long, T_StrategyInfo>  *m_pmulmapStrategyInfo;
	long long							 m_llStrategyTime;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
};
