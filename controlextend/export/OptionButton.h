#pragma once
#include <vector>
#include <string>
#include <map>
#include "dllexport.h"
using namespace std;

/*
* OptionButton 
* 使用方法
* COptionButton obj;
* 设置父窗口 obj.Create
* 设置 按钮图片 obj.SetBtnInfo
* //设置为选中状态 obj.SetSel
*/
typedef std::basic_string<TCHAR, char_traits<TCHAR>, allocator<TCHAR> > Tstring;

#define UM_TBNCLICK WM_USER + 100//Wparam BtnID LPARAM this

enum eImagePlace//图片排列
{
	IMAGE_VERTICAL,//竖直排列
	IMAGE_HORIZONTAL//水平排列
};
typedef struct BTN_Info
{
	UINT uID;
	UINT uGroupID;
	Tstring szText;
 	Tstring Img;//按钮四中状态 Nor Hot Sel Disable
	COLORREF clr[4];//字体颜色 Nor Hot Sel Disbale
	LOGFONT font;
	eImagePlace eimgPlace;
	UINT uCountStatus;
	BTN_Info()
	{
		uID = 0;
		uGroupID = 0;
		uCountStatus = 1;
		eimgPlace = IMAGE_VERTICAL;
		::ZeroMemory(&font,sizeof(font));
		font.lfHeight = 18;
		font.lfWeight = FW_NORMAL;
		lstrcpy(font.lfFaceName,_T("Arial"));
		for (int i = 0 ; i < 4 ; ++i)
		{
			clr[i] = RGB(255,255,255);
		}
	}
}BTN_Info;

class CONTROL_EXPORT COptionButton:public CRect
{
public:
	COptionButton();
	virtual ~COptionButton();
	
	BOOL Create(BTN_Info *pBtn,CWnd *pParent,CRect rtPos,BOOL bLoadFromRes = FALSE);

	BTN_Info * GetBtnInfo();
	void SetSel();
	CWnd *GetParent();
	
	static void SetResourcePath(LPCTSTR srcPth);
	static void SetSel(COptionButton *PCurBtn);
	static void HandleMessage(CWnd *pParent,UINT message,WPARAM wParam,LPARAM lParam);
private:
	void SetParent(CWnd *pParent);
	void SetBtnInfo(BTN_Info *pBtn,BOOL IsRedraw = FALSE);

	void WindowProc(UINT message,WPARAM wParam,LPARAM lParam);
	void OnPaint(WPARAM wParam,LPARAM lParam);
	void OnDraw(HDC hdc);
	
	void OnLButtonUp(WPARAM wParam,LPARAM lParam);
	void OnMouseMove(WPARAM wParam,LPARAM lParam);
	void OnMouseLeave(WPARAM wParam,LPARAM lParam);

public:
	BTN_Info m_Btn;
	BOOL m_bLoadFramRes;
private:
 
	BOOL m_isHot;
	BOOL m_isSel;
	CWnd *m_pParent;
	static Tstring m_srcPath;
	static map<COptionButton *,COptionButton *> m_BtnMessage; 

};