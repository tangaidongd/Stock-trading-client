#pragma once

class CBridgeWnd
{
	HWND		m_hWnd;	
	CString		m_tClassName;
public:
	CBridgeWnd(void);
	~CBridgeWnd(void);
	HWND	Create();
	static LRESULT CALLBACK WindowProc(__in HWND hWnd, __in UINT Msg,  __in WPARAM wParam,  __in LPARAM lParam);
	LRESULT Loop();
};

//	ÀëÏßµÇÂ¼
extern bool OfflineLogin();
