/*********************************************************************
* 版权所有 (C) 2004-2005, 点证科技有限公司
* 
* 文件名称： CheckBox.h
* 文件标识： 
* 内容摘要： 自绘checkbox控件，包括贴图和绘制显示文本
* 其它说明： 控件使用的状态图，是在一张图中，并且是垂直布局的，状态的顺序是：选中-不选中
* 当前版本： 
* 作    者： 
* 完成日期： 
* 
* 修改记录1：
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容： 
* 修改记录2：…
**********************************************************************/

#pragma once
#include <atlimage.h>
#include "dllexport.h"

// CCheckBox

class CONTROL_EXPORT CCheckBox : public CButton
{
	DECLARE_DYNAMIC(CCheckBox)

	CImage m_imgBK;
	int m_nStateCount;
	BOOL m_bChecked;
	CFont m_font;
	COLORREF m_clrText;
	COLORREF m_clrBkText;// 文本背景色

public:
	CCheckBox();
	virtual ~CCheckBox();

	void SetBkFromFile(const TCHAR *pFilePath, int nStateCount);
	void SetBkFromResource(int nResourceID, int nStateCount);
	void SetTitleColor(COLORREF clr);
	BOOL GetChecked();
	void SetChecked(BOOL bChecked);
	void SetBkText(COLORREF clrBk);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};


