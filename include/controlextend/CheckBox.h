/*********************************************************************
* ��Ȩ���� (C) 2004-2005, ��֤�Ƽ����޹�˾
* 
* �ļ����ƣ� CheckBox.h
* �ļ���ʶ�� 
* ����ժҪ�� �Ի�checkbox�ؼ���������ͼ�ͻ�����ʾ�ı�
* ����˵���� �ؼ�ʹ�õ�״̬ͼ������һ��ͼ�У������Ǵ�ֱ���ֵģ�״̬��˳���ǣ�ѡ��-��ѡ��
* ��ǰ�汾�� 
* ��    �ߣ� 
* ������ڣ� 
* 
* �޸ļ�¼1��
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ� 
* �޸ļ�¼2����
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
	COLORREF m_clrBkText;// �ı�����ɫ

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


