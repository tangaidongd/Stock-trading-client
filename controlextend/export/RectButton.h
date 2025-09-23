#ifndef _RECT_BUTTON_H_
#define _RECT_BUTTON_H_

#pragma warning(push)
#pragma warning(disable:4786)

#include <map>
using std::map;
#include <vector>
using std::vector;

#include "TBWndBase.h"
#include "ColorStep.h"

/***********************************
* ʾ��
class CRectButtonParentExample
{
	// ��ȡ��DC�����ͷ�
	CDC		*GetDrawNudeDC();
	void	ReleaseDrawNudeDC(CDC *pDC);

	// �ؼ�id
	int		GetDlgCtrlID();
	HWND	GetSafeHwnd();

    // Rectת��
	BOOL	ScreenToRectBtnPt(CPoint *pt);		// ��Ļ���굽��ť�����ת�� !!!����ʱ����������ת�������谴ť������dc����һ��	

	// focus
	CRectButton *GetCaptureBtn();						// ��ǰ�Ƿ��а�ť��ȡ��focus
	void	RequestMouseCapture(CRectButton &btn);		// ��ť����mouse capture
	void	ReleaseMouseCapture(CRectButton &btn);		// ��ť�����ͷ���ǰ�����mouse capture
};

  // ʵ����Щ����Ҫ��:
  //		1. onsizeʱ���䰴ť��С
  //		2. onpaintʱ����draw����
  //		3. PreTranslateMsg�н���Ϣ���ݸ���ť
  // ��ť�ڴ���ʱͨ�� UM_RECTBUTTON_NOTIFY WPARAM=id=����İ�ťid   LPARAM = RECT_BTN_NOTIFY_ITEM *
  // �ṹ RECT_BTN_NOTIFY_ITEM ֪ͨOwner����,notifyCode=NM_CLICK(һ�������)/NM_RCLICK
**************************************
*/

#define UM_RECTBUTTON_NOTIFY	(WM_USER+0x1222)

template<class ParentType> class CRectRadioBtnGroup;

template<class RectButtonParent>
class CONTROL_EXPORT CRectButton
{
public:
	typedef CRectRadioBtnGroup<RectButtonParent> RectRadioBtnGroup;
	typedef map<int, CString> MapStr;

	enum E_BtnDrawState{		// ��ť����״̬ - 
		Normal,				// ����״̬
		Focus,				// ����ƶ� - ��Select״̬�£��������focus
		Selected,			// selected / pressed - ����Ŀǰʹ��LButtonDown����click��������û��select����������Ҳû����ѹ��Ч��- -
		Disable				// ��ֹ - ��ֹ״̬�£����ᷢ���κ���Ϣ
	};

	// ��ť��Ϊ
	enum E_BtnBehavior{
		BTNB_NORMAL,		// ��ͨ��ť��ֻ��click����check
		BTNB_RADIO,			// radioButton��ͬ����ֻ��һ���ܱ�ѡ��
		BTNB_CHECKBOX,		// CheckBox������һ�£�ѡ��/��ѡ���л�
		BTNB_SEPARATOR,		// �ָ�����û��click�¼���ֻ��һ������
	};

	// ��ɫ���� - ��16λΪ�������ͣ���8λ�������ť�����״̬(NFSD)����8Ϊ�������米��1��2
	enum E_BtnColorType{
		BTN_COLOR_Background	=	0,			
		BTN_COLOR_Frame			=   1,
		BTN_COLOR_Text			=	2,
		BTN_COLOR_OTHER_START	=   3,
		BTN_COLOR_OTHER_END     =   0xffff
	};

	enum E_BackgroundColorIndex{
		BTN_Background1,
		BTN_Background2
	};

	enum E_DrawStyle{
		Draw_Style1,		// ���ֻ��Ƶķ���
		Draw_Style2
	};

	enum E_ClickLBtnDrawStyle{
		ClickAfterLButtonUpDraw,		// �Ƿ��ڷ���click��Ϣ����ư�ť����״̬
		ClickBeforLButtonUpDraw,
	};

	// Marlett char�������õ��ַ�
	enum E_MarlettChar{
		Marlett_Begin = '0'-1,				// '0' Ϊ��һ��(������ǰ�棬����0)
		Marlett_Min = 48,					//  48 Min
		Marlett_MAX,						//  49 max
		Marlett_RESTORE,					//  50 Restore
		Marlett_CHECKMARK=98,				//  98 Checkmark 
		Marlett_Bullet = 105,				// 105 Bullet
		Marlett_Close = 114,				// 114 Close
		Marlett_Help,						// ?
		Marlett_UpTriangle,					// �������ϵ�����
		Marlett_DownTroangle,				// ���µ�����
		Marlett_End = 'z'					// 'z'Ϊ���һ��(������)
	};

	// ֪ͨ
	typedef struct tagRECT_BTN_NOTIFY_ITEM
	{
		NMHDR	hdr;			// ��������������tbwnd parent����
		uint16	uBtnId;			// ���崥����btn
		CRectButton	*pBtn;		
		
		LPARAM	lParam;			// �������� - ����wsp��ť���ļ���...
	}RECT_BTN_NOTIFY_ITEM;


	// ����s
	CRectButton(RectButtonParent &wndTB)
		: m_wndTB(wndTB)
	{
		Initial(NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL);
		//m_wndTB.OnNewBtn(*this);
	}
	// hWndOwner - ���������, nID - ��button���е�ID, pszText��button����ʾ����
	// pszTip - ��ʾ����
	// �Լ������ֻ���
	CRectButton(RectButtonParent &wndTB, CWnd *pWndOwner, uint16 nID, LPCTSTR pszText, LPCTSTR pszTip = NULL) 
		:m_wndTB(wndTB)
	{
		Initial(pWndOwner, nID, pszText, NULL, NULL, NULL, NULL, pszTip);
		//m_wndTB.OnNewBtn(*this);
	}

	// �Լ������ֻ��� - ʹ��marllet��������ַ�ch
	CRectButton(RectButtonParent &wndTB, CWnd *pWndOwner, uint16 nID, E_MarlettChar chMarllet, LPCTSTR pszTip = NULL) 
		:m_wndTB(wndTB)
	{
		TCHAR chs[2];
		chs[0] = (TCHAR)chMarllet;
		chs[1] = '\0';
		Initial(pWndOwner, nID, chs, NULL, NULL, NULL, NULL, pszTip);
		SetUseMarlettChar(true);
	}

	// ��ͼ���� - ������ѡ��(ѹ��)������ƶ�����ֹ
	CRectButton(RectButtonParent &wndTB, CWnd *pWndOwner, uint16 nID, Image *pImageNomal, Image *pImageSelected, Image *pImageFocus, Image *pImageDisable, LPCTSTR pszTip = NULL)
		:m_wndTB(wndTB)
	{
		Initial(pWndOwner, nID, NULL, pImageNomal, pImageSelected, pImageFocus, pImageDisable, pszTip);
		//m_wndTB.OnNewBtn(*this);
	}

	// ��ͼ���� + ���֣����ָ����Ϸ� - ������ѡ��(ѹ��)������ƶ�����ֹ
	CRectButton(RectButtonParent &wndTB, CWnd *pWndOwner, uint16 nID, LPCTSTR pszText, Image *pImageNomal, Image *pImageSelected, Image *pImageFocus, Image *pImageDisable, LPCTSTR pszTip = NULL)
		:m_wndTB(wndTB)
	{
		Initial(pWndOwner, nID, pszText, pImageNomal, pImageSelected, pImageFocus, pImageDisable, pszTip);
		//m_wndTB.OnNewBtn(*this);
	}

	// ���ݳ�ʼ��
	void	Initial(CWnd *pWndOwner, uint16 nID, LPCTSTR pszText, Image *pImageNomal, Image *pImageSelected, Image *pImageFocus, Image *pImageDisable, LPCTSTR pszTip)
	{
		m_pWndOwner		= pWndOwner;
		m_nID			= nID;
		m_StrText		= pszText;
		m_StrTip		= pszTip;
		m_pImageNomal	= pImageNomal;
		m_pImageSelected = pImageSelected;
		m_pImageFocus	= pImageFocus;
		m_pImageDisable = pImageDisable;
		
		m_RectButton.SetRectEmpty();
		
		m_eBtnState     = Normal;
		
		m_eBtnBehavior	= BTNB_NORMAL;
		m_pRadioGroup	= NULL;
		m_bCheck		= false;
		
		LOGFONT lf = {0};
		lf.lfHeight = 12;
		_tcscpy(lf.lfFaceName, _T("������"));
		m_font.CreateFontIndirect(&lf);
		
		m_bUseMarllet = false;
		
		m_iGroupId = -1;
		
		m_eDrawStyle = Draw_Style1;
		
		m_eClickStyle = ClickAfterLButtonUpDraw;
		
		m_bInLButtonUp = false;
		
		// ��ʼ��������ɫ
		COLORREF clrStartN = RGB(250,250,250);
		COLORREF clrEndN   = RGB(190,190,190);
		COLORREF clrStartS = RGB(140,140,140);
		COLORREF clrEndS   = clrEndN;
		COLORREF clrFrameN = RGB(110,110,110);
		COLORREF clrFrameF = RGB(205,104,65);
		COLORREF clrTextN   = RGB(0,0,0);
		COLORREF clrTextS   = RGB(255,0,0);
		COLORREF clrTextD   = GetSysColor(COLOR_GRAYTEXT);
		SetColor(BTN_COLOR_Background, BTN_Background1, clrStartN, _T("��������"), false);		// ��������
		SetColor(BTN_COLOR_Background, BTN_Background2, clrEndN,   _T("�����м�"), false);
		SetColor(BTN_COLOR_Background, Selected, BTN_Background1, clrStartS); // ����selected
		SetColor(BTN_COLOR_Background, Selected, BTN_Background2, clrEndS);
		
		SetColor(BTN_COLOR_Frame, 0, clrFrameN, _T("�߿�"), false);
		SetColor(BTN_COLOR_Frame, Focus, 0, clrFrameF);
		
		SetColor(BTN_COLOR_Text, 0, clrTextN, _T("����"), false);
		SetColor(BTN_COLOR_Text, Focus, 0, clrFrameF);
		SetColor(BTN_COLOR_Text, Selected, 0, clrTextS);
		SetColor(BTN_COLOR_Text, Disable, 0, clrTextD);
	}

	// ����
	virtual ~CRectButton()
	{
		if ( m_pRadioGroup )
		{
			m_pRadioGroup->RemoveRadioBtn(this);
		}
	}

	// ��Ϊ
	// �ı�button����Ϊ�����������ڳ�ʼ����ʱ����� pGroup - Radio��Ҫһ������Э������radio��check״̬
	void		SetBtnBehavior(E_BtnBehavior eNewBehavior, CRectRadioBtnGroup *pGroup = NULL) 
	{
		E_BtnBehavior eOld = m_eBtnBehavior;
		if ( BTNB_RADIO == eOld )
		{
			if ( m_pRadioGroup )
			{
				m_pRadioGroup->RemoveRadioBtn(this);
			}
		}
		m_eBtnBehavior	=	eNewBehavior;
		if ( BTNB_RADIO == eNewBehavior )
		{
			ASSERT( pGroup && !m_pRadioGroup );		// ֻ�ܼ���һ����
			m_pRadioGroup = pGroup;
			if ( m_pRadioGroup )
			{
				SetCheck(false);		// �½����飬��ǿ�ƽ�checkΪfalse
				m_pRadioGroup->AddRadioBtn(this);
			}
		}
	}

	// ֻ�� checkbox / radio �ܹ�����
	void		SetCheck(bool32 bCheck)				
	{
		//ASSERT( IsEnable() );
		if ( BTNB_RADIO == m_eBtnBehavior || BTNB_CHECKBOX == m_eBtnBehavior )
		{
			bool32	bOld = m_bCheck;
			m_bCheck = bCheck;
			if ( m_bCheck != bOld )
			{
				DrawNude();
			}
			if ( m_bCheck && BTNB_RADIO == m_eBtnBehavior )
			{
				if ( m_pRadioGroup )
				{
					m_pRadioGroup->IamCheckBtn(this);
				}
			}
		}
		else
		{
			ASSERT( 0 );
		}
	}

	bool32		GetCheck() const
	{
		if ( BTNB_RADIO == m_eBtnBehavior || BTNB_CHECKBOX == m_eBtnBehavior )
		{
		}
		else
		{
			ASSERT( 0 );
		}
		return m_bCheck;
	}

	// �������Ե�
	E_BtnBehavior	GetBtnBehavior()const { return m_eBtnBehavior; };				// ��ȡ��ť��Ϊ
	void		SetUseMarlettChar(bool32 bUse = false){ m_bUseMarllet = bUse; }		// �����Ƿ�ʹ��MarlettChar���������б���ͼ��Ļ��� - lpszText�ᱻ����MarlettChar
	bool32		IsSeparator()const { return BTNB_SEPARATOR == m_eBtnBehavior;  };	// �Ƿ�Ϊ�ָ���

	// ����
	void		SetBtnText(LPCTSTR pcszText){ m_StrText = pcszText; };		// Ŀǰֻ�ṩ��Щ���Բ���
	void		SetBtnTextWithDraw(LPCTSTR lpszText)
	{
		if ( m_StrText != lpszText )
		{
			m_StrText = lpszText;
			DrawNude();
		}
	}
	void		SetBtnTip(LPCTSTR pcszTip){ m_StrTip = pcszTip; };
	void		SetBtnGroupId(int iGroupId) { m_iGroupId = iGroupId; };
	void		SetDrawStyle(E_DrawStyle eStyle) { m_eDrawStyle = eStyle; } 
	void		SetClickStyle(E_ClickLBtnDrawStyle eStyle) { m_eClickStyle = eStyle; }
	void		SetBtnImages(Image *pImageNomal, Image *pImageSelected = NULL, Image *pImageFocus = NULL, Image *pImageDisable = NULL)
	{
		m_pImageNomal = pImageNomal;
		m_pImageSelected = pImageSelected;
		m_pImageFocus = pImageFocus;
		m_pImageDisable = pImageDisable;
	}
	void	SetFont(CFont &font)
	{
		LOGFONT lf = {0};
		font.GetLogFont(&lf);
		m_font.DeleteObject();
		m_font.CreateFontIndirect(&lf);
	}
	void	SetBtnState(E_BtnDrawState eState, bool32 bForce = false)		// ���ð�ť�µ�״̬
	{
		ASSERT( m_eBtnBehavior != BTNB_SEPARATOR );
		E_BtnDrawState eOld = m_eBtnState;
		if ( Selected == m_eBtnState && Focus == eState && !bForce )
		{
			// focus״̬���ܸı�ѡ��״̬
		}
		else
		{
			m_eBtnState = eState;
		}
		
		if ( eOld != m_eBtnState )
		{
			// 		TRACE(_T("SetBtnState: %d->%d\r\n"), eOld, m_eBtnState);
			// 		if ( eOld == Selected )
			// 		{
			// 			int i = 0;
			// 		}
			DrawNude();
		}
	}
	

	// ��ȡ
	CString		GetBtnText() const { return m_StrText; };
	CString		GetBtnTip() const { return m_StrTip; };
	uint16		GetBtnId() const { return m_nID; };
	int			GetBtnGroupId() const  { return m_iGroupId; };
	E_DrawStyle	GetDrawStyle()const { return m_eDrawStyle; }
	E_ClickLBtnDrawStyle GetClickStyle() const { return m_eClickStyle; }
	void		GetBtnImages(Image **ppImageNomal, Image **ppImageSelected = NULL, Image **ppImageFocus = NULL, Image **ppImageDisable = NULL) const;
	const CFont &GetFont() const { return m_font; }
	E_BtnDrawState GetBtnState() const { return m_eBtnState; }
	RectButtonParent	&GetBtnParent() { return m_wndTB; }

	// ����
	virtual		CSize   GetBtnTextExtend(CDC *pDC)		// �������ֻ�ȡ������Ҫ�Ĵ�С
	{
		bool32	bNeedReleaseDC = false;
		if ( pDC == NULL )
		{
			pDC = m_wndTB.GetDrawNudeDC();
			if ( NULL == pDC )
			{
				return CSize(0,0);
			}
			bNeedReleaseDC = true;
		}
		CSize sizeText(0,0);
		CFont *pFontOld = pDC->SelectObject(&m_font);
		CString StrText = m_StrText;
		if ( StrText.IsEmpty() )
		{
			StrText = _T("�߶Ȳ���");
		}
		sizeText = pDC->GetTextExtent(StrText);
		pDC->SelectObject(m_font);
		if ( bNeedReleaseDC )
		{
			m_wndTB.ReleaseDrawNudeDC(pDC);
			pDC = NULL;
		}
		
		sizeText.cy += 2*2;
		if ( m_StrText.IsEmpty() )
		{
			sizeText.cx = 2*2;
		}
		if ( BTNB_SEPARATOR == m_eBtnBehavior )
		{
			sizeText.cx = 2;
		}
		
		return sizeText;
	}
	CSize	MeasureBtnRect(CDC *pDC, OUT bool32 &bFixed)			// ��ȡ��Rectϣ����ȡ�Ĵ�С, bFixed��ʾ�Ƿ�Ӧ���̶���С
	{
		bFixed  = false;
		bool32 bDeleteDC = false;
		if ( NULL == pDC )
		{
			pDC = m_wndTB.GetDrawNudeDC();
			if ( NULL == pDC )
			{
				ASSERT( 0 );
				return CSize(0,0);
			}
			bDeleteDC = true;
		}
		
		// ��������ͼƬ��������������
		CSize sizeRet(0,0);
		Image *pImageNormal = m_pImageNomal;
		if ( NULL != pImageNormal )
		{
			sizeRet.cx = pImageNormal->GetWidth() + 2;	// �߿�
			sizeRet.cy = pImageNormal->GetHeight() + 2;
			if ( sizeRet.cy < 20 )
			{
				sizeRet.cy = 20;	// ��С�߶�20
			}
			bFixed = true;
		}
		else
		{
			CFont *pFontOld = pDC->SelectObject(&m_font);
			sizeRet = pDC->GetTextExtent(m_StrText);
			pDC->SelectObject(&m_font);
			sizeRet.cx += 14;
			sizeRet.cy += 6;
		}
		if ( bDeleteDC )
		{
			m_wndTB.ReleaseDrawNudeDC(pDC);
			pDC = NULL;
		}
		return sizeRet;
	}
	void	GetBtnRect(OUT CRect &RectBtn){RectBtn = m_RectButton;}
	// ����recalcLayout���ⲿ���ǵ������������������ΰɣ�������ɫ��ʱ����ѭ��
	void	MoveBtn(const CRect &RectBtn, bool32 bDraw = true)			
	{
		m_RectButton = RectBtn;
		m_RectButton.NormalizeRect();
		if ( m_RectButton.Width() > 2000 || m_RectButton.Height() > 2000 )
		{
			ASSERT( 0 );
			m_RectButton.SetRectEmpty();
		}
		
		if ( bDraw )
		{
			DrawNude();
		}
	}

	// ��ɫ����
	// �洢��ť������ɫ��map
	TBWndColorItemMap &GetBtnColorsRef() { return m_mapColorItems ; };
	bool32	GetColor(E_BtnColorType eType, E_BtnDrawState eState, int32 iIndex, COLORREF &clr)		// ���ݵ�ǰ״̬��ȡ�ض����͵���ɫ
	{
		int iColorIndex = CombineBtnColorIndex(eType, eState, iIndex);
		if ( m_mapColorItems.count(iColorIndex) > 0 )
		{
			clr = m_mapColorItems[iColorIndex].m_clr;
			return true;
		}
		else
		{
			// ������û��Normal״̬�� / index 0 ��
			
			iColorIndex = CombineBtnColorIndex(eType, Normal, iIndex);
			if ( m_mapColorItems.count(iColorIndex) > 0 )
			{
				clr = m_mapColorItems[iColorIndex].m_clr;
				return true;
			}
			else
			{
				iColorIndex = CombineBtnColorIndex(eType, Normal, 0);
				clr = m_mapColorItems[iColorIndex].m_clr;
				return true;
			}
			
			ASSERT( 0 ); // ������ɫ�����ٳ�ʼ��Normal
			return false;
		}
	}
	void	SetColor(E_BtnColorType eType, E_BtnDrawState eState, int32 iIndex, COLORREF clr, LPCTSTR lpszDesc=NULL, bool32 bNeedDraw = false)	// ����ǵ�һ�εĻ�����Ҫ���������ִ�
	{
		int iColorIndex = CombineBtnColorIndex(eType, eState, iIndex);
		if ( m_mapColorItems.count(iColorIndex) > 0 )
		{
			if ( m_mapColorItems[iColorIndex].m_clr != clr )
			{
				m_mapColorItems[iColorIndex].m_clr = clr;
				if ( bNeedDraw && m_eBtnState == eState)
				{
					DrawNude();
				}
			}
		}
		else
		{
			// ��ʼ��
			ASSERT( NULL != lpszDesc && _tcslen(lpszDesc) > 0 );
			CString StrDesc = lpszDesc;
			if ( StrDesc.IsEmpty() )
			{
				StrDesc.Format(_T("��ɫ%x"), iColorIndex);
			}
			CTBWndColorValue	btnClr(this, StrDesc, clr);
			m_mapColorItems[iColorIndex] = btnClr;
		}
	}
	void	SetColor(E_BtnColorType eType, int32 iIndex, COLORREF clr, LPCTSTR lpszDesc=NULL, bool32 bNeedDraw=false)	// ��4��״̬�Ķ�����ͬ����ɫ
	{
		MapStr	Strs;
		Strs[Normal]   = _T("(Normal)");
		Strs[Focus]    = _T("(Focus)");
		Strs[Selected] = _T("(Press/Selected)");
		Strs[Disable]  = _T("(Disable)");
		
		
		int iColorIndex = CombineBtnColorIndex(eType, Normal, iIndex);
		
		CString StrDesc = lpszDesc == NULL ? _T("") : lpszDesc;
		if ( StrDesc.IsEmpty() )
		{
			StrDesc.Format(_T("��ɫ%x"), iColorIndex);
		}
		for ( MapStr::iterator it=Strs.begin(); it!=Strs.end(); it++ )
		{
			SetColor(eType, (E_BtnDrawState)it->first, iIndex, clr, StrDesc+it->second, bNeedDraw);
		}
	}
	int		CombineBtnColorIndex(E_BtnColorType eType, E_BtnDrawState eState, int32 iIndex)	// ���ذ��չ�����Ƶ�����
	{
		return (int)((eType<<16) | ((eState << 8)&0xff00) | ((iIndex)&0xff));
	}
	void	SetBtnColors(const TBWndColorItemMap &mapColors, bool32 bDrawIfChange = true)			// ���ð�ť��ɫ
	{
		bool32 bDraw = false;
		for ( TBWndColorItemMap::const_iterator it=m_mapColorItems.begin();
		it != m_mapColorItems.end();
		it++)
		{
			TBWndColorItemMap::const_iterator itSet = mapColors.find(it->first);
			if ( itSet != mapColors.end() )		// �оͽ��и�������
			{
				m_mapColorItems[it->first] = itSet->second;
				bDraw = true;
			}
		}
		
		if ( bDraw && bDrawIfChange )
		{
			DrawNude();
		}
	}
	void	GetBtnColors(OUT TBWndColorItemMap &mapColor) const {mapColor = m_mapColorItems;}

	// ��Ϊ����
	// Ŀǰ�������click������LButtonDown
	virtual     bool32	DoClicked(CPoint ptClick)		// �û�����˸ð�ť - ����֪ͨ��hWndOwner������hWndOwner�����أ�Ĭ�Ϸ���true����btnState����Ϊselect
	{
		// ֻ���ڸþ��η�Χ�ڵ�pt���ܴ�����
		if ( !m_RectButton.PtInRect(ptClick) )
		{
			ASSERT( 0 );
			return false;
		}
		
		bool32 bNotify = false;
		if ( BTNB_CHECKBOX == m_eBtnBehavior )
		{
			bool32 bCheck = !m_bCheck;
			bNotify = NotifyOwner(NM_CLICK, NULL);		// check & radioӦ������check��Ȼ����֪ͨ��Ӧ�����ƺܶ�֪ͨ����
			if ( bNotify )
			{
				SetCheck(bCheck);
			}
		}
		else if ( BTNB_RADIO == m_eBtnBehavior )
		{
			ASSERT( !m_bCheck );		// ֻ����uncheck - > check
			if ( !m_bCheck )
			{
				bNotify = NotifyOwner();
				if ( bNotify )
				{
					SetCheck(true);
				}
			}
		}
		else		// ��ͨ��ť
		{
			ASSERT( IsEnable() );
			bNotify =  NotifyOwner();
		}
		
		return bNotify;
	}
	
	bool32	IsEnable() const								// �Ƿ�ɽ�����Ϣ - �粻�ܽ�����TBWnd���ᷢ�������Ϣ�������ť
	{
		return Disable != m_eBtnState && BTNB_SEPARATOR != m_eBtnBehavior;		// �ָ����൱�ڽ�ֹ��
	}
	void	EnableBtn(bool32 bEnable = true)
	{
		if ( m_wndTB.GetCaptureBtn() == this )
		{
			m_wndTB.ReleaseMouseCapture(*this);		// �����ͷ����,wndTB�����ͷŴ���
		}
		
		if ( bEnable && !IsEnable() )
		{
			SetBtnState(Normal);
		}
		else if ( !bEnable && IsEnable() )
		{
			SetBtnState(Disable);
		}
		// else // �Ѿ���Ҫ���״̬�ˣ����ô���
	}

	// �����Ϣ��click�ɰ�ť�Լ����� - ���治���Լ�����windows button������
	// ���Parent�յ�mouseleave�����һ���۽���ťӦ���յ�point(-1,-1)��mousemove��������mousemove״̬
	virtual		void	OnMouseMove(UINT nFlag, CPoint point)		
	{
		// ��ͨmove�� lButton Press��move�� mouseLeave���͵�-1-1move
		ASSERT( IsEnable() );
		if ( !IsEnable() )
		{
			return;
		}
		if ( m_RectButton.PtInRect(point) )		// ״̬���Գ���״̬ջ����
		{
			if ( Selected != m_eBtnState )		
			{
				SetBtnState(Focus);		// ��ͨmove
			}
			// else // lButton press��move - ���ı�״̬
		}
		else
		{
			if ( Selected != m_eBtnState )
			{
				SetBtnState(Normal);		// ��ͨ�뿪������
			}
			else if ( m_wndTB.GetCaptureBtn() != this )
			{
				ASSERT( m_wndTB.GetCaptureBtn() == NULL );
				if ( !m_bInLButtonUp )
				{
					SetBtnState(Normal);		// ��Ȼ�а��±�־�����������ť��û��capture����ԭ
				}
			}
			// else // lButton����뿪 - �Ѿ���capture�ˣ�����Ҫ�ı�
		}
	}
	virtual		void	OnLButtonDown(UINT nFlag, CPoint point)	// ��ʱ��Ҫcapturemouse - �Ȳ�ʵ��capture��ֻ����һ����ʾЧ����up��ʱ���ֱ��click
	{
		ASSERT( IsEnable() );
		if ( !IsEnable() )
		{
			return;
		}
		if ( m_RectButton.PtInRect(point) )
		{
			SetBtnState(Selected);
			m_wndTB.RequestMouseCapture(*this);				// capture
			ASSERT( m_wndTB.GetCaptureBtn() == this );
		}
		else
		{
			//ASSERT( 0 );  // �����ڸð�ť���򣬲�Ӧ���յ�down��Ϣ - ȫת��
		}
	}
	virtual     void	OnLButtonUp(UINT nFlag, CPoint point)		// ��ʱ��Ҫ����TBWnd�����releasemouse
	{
		ASSERT( IsEnable() );
		if ( m_wndTB.GetCaptureBtn() == this )
		{
			m_wndTB.ReleaseMouseCapture(*this);		// �����ͷ����,wndTB�����ͷŴ���
		}
		if ( !IsEnable() )
		{
			return;
		}
		m_bInLButtonUp = true;
		
		if ( m_RectButton.PtInRect(point) )
		{
			if ( BTNB_RADIO == m_eBtnBehavior && m_bCheck )	// radio check����click
			{
				
			}
			else if ( Selected == m_eBtnState )	// �����Ȱ���
			{
				if ( m_eClickStyle == ClickBeforLButtonUpDraw || BTNB_CHECKBOX == m_eBtnBehavior )
				{		
					DoClicked(point);		// ����¼�����
					SetBtnState(Normal);		// ���ڰ�ť��
				}
				else
				{
					SetBtnState(Normal);		// ���ڰ�ť��
					DoClicked(point);		// ����¼�����
					
				}
			}
			else
			{
				SetBtnState(Normal);		// ���ڰ�ť��
			}
		}
		else
		{
			SetBtnState(Normal);	// �뿪��
		}
		m_bInLButtonUp = false;
	}
	virtual		void	OnRButtonUp(UINT nFlag, CPoint point)		// �򵥵㣬ֻ��������Ҽ�������ò��ѹ�²��Ǻܱ�Ҫcapture
	{
		// �������κ�״̬ - ���ܽ����жԹ���������ѽʲô��������Ҫ����¼�
		ASSERT( IsEnable() );
		
		// ��ɫ����
		if ( m_RectButton.PtInRect(point) )
		{
			NotifyOwner(NM_RCLICK, NULL);
		}
	}

	// Parent PreTranslateMsg����Ϣ����������ｫ�ַ���갴�£��������ƶ����뿪������
	void	RelayEventMsg(MSG *pMsg)
	{
		ASSERT( NULL!=pMsg );

		// ���ܴ�����߲��Ǹ����ڵ���Ϣ
		if ( !IsEnable() || pMsg->hwnd != m_wndTB.GetSafeHwnd()  )
		{
			return;
		}
		
		if ( WM_LBUTTONDOWN!=pMsg->message
			&& WM_LBUTTONUP!=pMsg->message
			&& WM_MOUSEMOVE!=pMsg->message
			&& WM_RBUTTONUP!=pMsg->message
			&& WM_MOUSELEAVE!=pMsg->message
			&& WM_NCLBUTTONDOWN!=pMsg->message
			&& WM_NCLBUTTONUP!=pMsg->message
			&& WM_NCMOUSEMOVE!=pMsg->message
			&& WM_NCRBUTTONUP!=pMsg->message
			&& 0x02A2!=pMsg->message	//WM_NCMOUSELEAVE
			)
		{
			return;
		}

		CPoint ptRect = pMsg->pt;
		if ( !m_wndTB.ScreenToRectBtnPt(&ptRect) )
		{
			ASSERT( 0 );
			return;
		}

		switch (pMsg->message)
		{
		case WM_LBUTTONDOWN:
		case WM_NCLBUTTONDOWN:
			OnLButtonDown((UINT)pMsg->wParam, ptRect);
			break;
		case WM_LBUTTONUP:
		case WM_NCLBUTTONUP:
			OnLButtonUp((UINT)pMsg->wParam, ptRect);
			break;
		case WM_MOUSEMOVE:
		case WM_NCMOUSEMOVE:
			OnMouseMove((UINT)pMsg->wParam, ptRect);
			break;
		case WM_MOUSELEAVE:
		case 0x2A2:	//WM_NCMOUSELEAVE
			OnMouseMove(0, CPoint(-1,-1));
			break;
		}
	}

	
	// ����
	virtual		void	Draw(CDC &dc)					// ����state����Button�������߿����
	{
		switch (m_eDrawStyle)
		{
		case Draw_Style2:
			DrawStyle2(dc);
			break;
		default:
			DrawStyle1(dc);
		}
	}
	void	DrawNude()
	{
		if ( !m_RectButton.IsRectEmpty() )
		{
			CDC *pDC = m_wndTB.GetDrawNudeDC();
			if ( NULL!=pDC )
			{
				Draw(*pDC);
			}
			m_wndTB.ReleaseDrawNudeDC(pDC);
		}
	}
	

protected:
	friend RectRadioBtnGroup;

	// button������Ϣ��ʽ SendMessage(hWndOwner, nMessageNotifyId, LPARAM)
	// ����wndTB��Ϣ֪ͨ��ʽ��֪ͨ hWndOwner click
	virtual		bool32	NotifyOwner(UINT nNotifyCode = NM_CLICK, LPARAM lParam = NULL)						
	{
		HWND hWndOwner = m_pWndOwner->GetSafeHwnd();
		if ( IsEnable() && IsWindow(hWndOwner) )
		{
			HWND hWndTB = m_wndTB.GetSafeHwnd();
			RECT_BTN_NOTIFY_ITEM item = {0};
			item.hdr.idFrom = m_wndTB.GetDlgCtrlID();
			item.hdr.hwndFrom = hWndTB;
			item.hdr.code = nNotifyCode;
			item.lParam = lParam;
			item.uBtnId = m_nID;
			item.pBtn = this;
			return SendMessage(m_pWndOwner->GetSafeHwnd(), UM_RECTBUTTON_NOTIFY, m_nID, (LPARAM)&item);
		}
		// �Ǵ��ڣ���Ӧ������Ϣ����
		ASSERT( 0 );
		return false;
	}
	
	// ���Ʒ�ʽ1
	void	DrawStyle1(CDC &dc)
	{
		if ( m_RectButton.IsRectEmpty() )
		{
			return;
		}
		
		if ( m_RectButton.Width() <0 || m_RectButton.Width() > 20000
			|| m_RectButton.Height() <0 || m_RectButton.Height() > 20000)
		{
			ASSERT( 0 );
			return;
		}
		int iSave = dc.SaveDC();
		
		CRect rcDraw(m_RectButton);
		//ASSERT( rcDraw.Width() < 200 && rcDraw.Width() >= 0 );
		//ASSERT( rcDraw.Height() < 200 && rcDraw.Height() >= 0 );
		dc.SelectObject(&m_font);
		
		bool32 bColorOK = true;
		COLORREF clrBK1;
		COLORREF clrBK2;
		bColorOK = GetColor(BTN_COLOR_Background, m_eBtnState, BTN_Background1, clrBK1) && bColorOK;
		bColorOK = GetColor(BTN_COLOR_Background, m_eBtnState, BTN_Background2, clrBK2) && bColorOK;
		
		COLORREF clrText;
		COLORREF clrFrame;
		bColorOK = GetColor(BTN_COLOR_Frame, m_eBtnState, 0, clrFrame) && bColorOK;
		bColorOK = GetColor(BTN_COLOR_Text,  m_eBtnState, 0, clrText) && bColorOK;
		ASSERT( bColorOK );
		
		if ( IsSeparator() )	// ������һ���ָ���
		{
			rcDraw.InflateRect(0, -rcDraw.Height()/6);
			rcDraw.NormalizeRect();
			CRect rcLeft(rcDraw), rcRight(rcDraw);
			rcLeft.right = rcLeft.left + rcDraw.Width()/2;
			rcRight.left = rcLeft.right;
			
			CColorStep step;
			step.InitColorRange(clrBK1, -50.0, -50.0, -50.0, true);
			clrBK1 = step.GetColor(1);
			step.InitColorRange(clrBK2, 50.0, 50.0, 50.0, true);
			clrBK2 = step.GetColor(1);
			dc.FillSolidRect(rcLeft, clrBK2);
			dc.FillSolidRect(rcRight, clrBK1);
			return;
		}
		
		bool32	bDrawFocus = true;
		bool32	bDrawSelected = false;
		
		DWORD dwTextDT = DT_SINGLELINE | DT_CENTER | DT_END_ELLIPSIS | DT_VCENTER;
		
		Image *pImage = m_pImageNomal;
		if ( Disable == m_eBtnState )
		{
			if ( m_pImageDisable )
			{
				pImage = m_pImageDisable;
			}
		}
		else if ( Selected == m_eBtnState )
		{
			bDrawSelected = true;
			if ( m_pImageSelected )
			{
				pImage = m_pImageSelected;
			}
		}
		else if ( Focus == m_eBtnState )
		{
			if ( m_pImageFocus )
			{
				pImage = m_pImageFocus;
			}
		}
		
		if ( Disable != m_eBtnState )
		{
			if ( (BTNB_RADIO == m_eBtnBehavior || BTNB_CHECKBOX == m_eBtnBehavior) && m_bCheck )
			{
				// check״̬�£�����Select״̬ͼƬ����ɫ��
				pImage = m_pImageSelected;
				
				bColorOK = GetColor(BTN_COLOR_Background, Selected, BTN_Background1, clrBK1) && bColorOK;
				bColorOK = GetColor(BTN_COLOR_Background, Selected, BTN_Background2, clrBK2) && bColorOK;
				bColorOK = GetColor(BTN_COLOR_Frame, Selected, 0, clrFrame) && bColorOK;
				bColorOK = GetColor(BTN_COLOR_Text,  Selected, 0, clrText) && bColorOK;
				ASSERT( bColorOK );
				
				bDrawFocus = false;
				bDrawSelected = true;
			}
		}
		
		dc.SetTextColor(clrText);
		
		CRgn rgn;
		rgn.CreateRoundRectRgn(rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom, 3, 3);
		dc.SelectClipRgn(&rgn, RGN_AND);
		
		CBrush brush;
		brush.CreateSolidBrush(clrFrame);
		dc.FrameRgn(&rgn, &brush, 1, 1);
		
		rcDraw.InflateRect(-1, -1);
		rgn.DeleteObject();
		rgn.CreateRoundRectRgn(rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom, 3, 3);
		dc.SelectClipRgn(&rgn, RGN_AND);
		
		if ( bDrawSelected )
		{
			// 		rcDraw.InflateRect(-1, -1);
			// 		dc.Draw3dRect(rcDraw, RGB(127,127,127), RGB(255,255,255));
			// 		rcDraw.InflateRect(-1, -1);
			clrBK1 = clrBK2;
		}
		
		int32 iMid = max(rcDraw.Height()/3, 1);
		CColorStep step(clrBK2, clrBK1, iMid);
		step.SetColorSmooth(true);
		//dc.FillSolidRect(m_RectButton, clrBK); 
		TRIVERTEX tvx[2];
		GRADIENT_RECT grr = {0, 1};
		tvx[0].x	= rcDraw.left;
		tvx[0].y	= rcDraw.top;
		tvx[0].Red	= (GetRValue(clrBK2)<<8);
		tvx[0].Green	= (GetGValue(clrBK2)<<8);
		tvx[0].Blue	= (GetBValue(clrBK2)<<8);

		tvx[1].x	= rcDraw.right;
		tvx[1].y	= rcDraw.top + iMid;
		tvx[1].Red	= (GetRValue(clrBK1)<<8);
		tvx[1].Green	= (GetGValue(clrBK1)<<8);
		tvx[1].Blue	= (GetBValue(clrBK1)<<8);
		
		GradientFill(dc.GetSafeHdc(), tvx, 2, &grr, 1, GRADIENT_FILL_RECT_V);
		grr.LowerRight	= 0;
		grr.UpperLeft	= 1;
		tvx[0].x		= rcDraw.right;
		tvx[0].y		= rcDraw.bottom;
		tvx[1].x		= rcDraw.left;
		GradientFill(dc.GetSafeHdc(), tvx, 2, &grr, 1, GRADIENT_FILL_RECT_V);

// 		for ( int32 iTop=rcDraw.top; iTop <= rcDraw.bottom; iTop++ )
// 		{
// 			dc.FillSolidRect(rcDraw.left, iTop, rcDraw.Width(), 1, step.NextColor());
// 		}
		
		
		if ( pImage )
		{
			Graphics	g(dc.m_hDC);
			g.DrawImage(pImage, rcDraw.left, rcDraw.right, rcDraw.Width(), rcDraw.Height());
		}
		
		rcDraw.InflateRect(-1,-1);
		
		
		// ��focus��־ - û��ͼƬ�������
		if ( !pImage && Focus == m_eBtnState && bDrawFocus )
		{
			COLORREF clrFocus = clrFrame;
			
			// ����С��� o
			CBrush brhFocus;
			brhFocus.CreateSolidBrush(clrFocus);
			//dc.FrameRgn(&rgn, &brhFocus, 1, 1);
		}
		
		if ( !m_StrText.IsEmpty() && BTNB_SEPARATOR != m_eBtnBehavior )
		{
			dc.SetBkMode(TRANSPARENT);
			// Ϊ�����ۺ�focus�����հף���rcDraw������С��ôһ��
			rcDraw.InflateRect(-2, 0);
			if ( m_bUseMarllet )
			{
				rcDraw.InflateRect(-2, -2);
				CFont MyFont;
				LOGFONT logfont;
				logfont.lfHeight = -rcDraw.Height();
				logfont.lfWidth = 0;
				logfont.lfEscapement = 0;
				logfont.lfOrientation = 0;
				logfont.lfWeight = FW_NORMAL;
				logfont.lfItalic = FALSE;
				logfont.lfUnderline = FALSE;
				logfont.lfStrikeOut = FALSE;
				logfont.lfCharSet = DEFAULT_CHARSET;
				logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
				logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
				logfont.lfQuality = DEFAULT_QUALITY;
				logfont.lfPitchAndFamily = DEFAULT_PITCH;
				
				_tcscpy(logfont.lfFaceName,_T("Marlett"));
				
				MyFont.CreateFontIndirect (&logfont);
				
				CFont* pOldFont = dc.SelectObject (&MyFont); 
				int OldMode = dc.SetBkMode(TRANSPARENT);  
				
				dc.DrawText (m_StrText,1,rcDraw,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
				dc.SetBkMode(OldMode);
				dc.SelectObject(pOldFont);
			}
			else
			{
				dc.DrawText(m_StrText, rcDraw, dwTextDT);
			}
		}
		
		dc.RestoreDC(iSave);
	}

	// ���Ʒ�ʽ2
	void	DrawStyle2(CDC &dc)
	{
		if ( m_RectButton.IsRectEmpty() )
		{
			return;
		}
		
		int iSave = dc.SaveDC();
		
		CRect rcDraw(m_RectButton);
		dc.SelectObject(&m_font);
		
		bool32 bColorOK = true;
		COLORREF clrBK1;
		COLORREF clrBK2;
		bColorOK = GetColor(BTN_COLOR_Background, m_eBtnState, BTN_Background1, clrBK1) && bColorOK;
		bColorOK = GetColor(BTN_COLOR_Background, m_eBtnState, BTN_Background2, clrBK2) && bColorOK;
		
		COLORREF clrText;
		COLORREF clrFrame;
		bColorOK = GetColor(BTN_COLOR_Frame, m_eBtnState, 0, clrFrame) && bColorOK;
		bColorOK = GetColor(BTN_COLOR_Text,  m_eBtnState, 0, clrText) && bColorOK;
		ASSERT( bColorOK );
		
		if ( IsSeparator() )	// ������һ���ָ���
		{
			rcDraw.InflateRect(0, -rcDraw.Height()/6);
			rcDraw.NormalizeRect();
			CRect rcLeft(rcDraw), rcRight(rcDraw);
			rcLeft.right = rcLeft.left + rcDraw.Width()/2;
			rcRight.left = rcLeft.right;
			
			CColorStep step;
			step.InitColorRange(clrBK1, -50.0, -50.0, -50.0, true);
			clrBK1 = step.GetColor(1);
			step.InitColorRange(clrBK2, 50.0, 50.0, 50.0, true);
			clrBK2 = step.GetColor(1);
			dc.FillSolidRect(rcLeft, clrBK2);
			dc.FillSolidRect(rcRight, clrBK1);
			return;
		}
		
		bool32	bDrawFocus = false;
		bool32	bDrawSelected = false;
		bool32  bGrayImage = false;
		bool32  bSelectedLikePush = false;
		
		DWORD dwTextDT = DT_SINGLELINE | DT_CENTER | DT_END_ELLIPSIS | DT_VCENTER;
		
		Image *pImage = m_pImageNomal;
		if ( Disable == m_eBtnState )
		{
			if ( m_pImageDisable )
			{
				pImage = m_pImageDisable;
			}
			else
			{
				bGrayImage = true;
			}
		}
		else if ( Selected == m_eBtnState )
		{
			bDrawSelected = true;
			if ( m_pImageSelected )
			{
				pImage = m_pImageSelected;
			}
			else
			{
				bSelectedLikePush = true;
			}
		}
		else if ( Focus == m_eBtnState )
		{
			if ( m_pImageFocus )
			{
				pImage = m_pImageFocus;
			}
			bDrawFocus = true;
		}
		
		if ( Disable != m_eBtnState )
		{
			if ( (BTNB_RADIO == m_eBtnBehavior || BTNB_CHECKBOX == m_eBtnBehavior) && m_bCheck )
			{
				// check״̬�£�����Select״̬ͼƬ����ɫ��
				if ( m_pImageSelected != NULL )
				{
					pImage = m_pImageSelected;
				}
				else
				{
					bSelectedLikePush = true;
				}
				
				bColorOK = GetColor(BTN_COLOR_Background, Selected, BTN_Background1, clrBK1) && bColorOK;
				bColorOK = GetColor(BTN_COLOR_Background, Selected, BTN_Background2, clrBK2) && bColorOK;
				bColorOK = GetColor(BTN_COLOR_Frame, Selected, 0, clrFrame) && bColorOK;
				bColorOK = GetColor(BTN_COLOR_Text,  Selected, 0, clrText) && bColorOK;
				ASSERT( bColorOK );
				
				bDrawFocus = false;
				bDrawSelected = true;
			}
		}
		
		dc.SetTextColor(clrText);
		
		// �û������α߿򣬱�������1/2�м���ƣ���������²����߿�focus�»���͹��߿�select���ư��ݱ߿�
		bool32 bInflateRectAfterFillBk = false;
		if ( bDrawFocus )
		{
			dc.DrawEdge(rcDraw, EDGE_RAISED, BF_RECT);
			rcDraw.InflateRect(-1, -1);
		}
		else if ( bDrawSelected )
		{
			dc.DrawEdge(rcDraw, EDGE_SUNKEN, BF_RECT);
			rcDraw.InflateRect(-1, -1);
		}
		else
		{
			bInflateRectAfterFillBk = true;
		}
		CRgn rgn;
		rgn.CreateRectRgnIndirect(rcDraw);
		dc.SelectClipRgn(&rgn, RGN_AND);
		
		int32 iMid = max(rcDraw.Height()/2, 1);
		CColorStep step(clrBK2, clrBK1, iMid);
		step.SetColorSmooth(true);
		//dc.FillSolidRect(m_RectButton, clrBK); 
		TRIVERTEX tvx[2];
		GRADIENT_RECT grr = {0, 1};
		tvx[0].x	= rcDraw.left;
		tvx[0].y	= rcDraw.top;
		tvx[0].Red	= GetRValue(clrBK2);
		tvx[0].Green	= GetGValue(clrBK2);
		tvx[0].Blue	= GetBValue(clrBK2);
		
		tvx[1].x	= rcDraw.right;
		tvx[1].y	= rcDraw.top + iMid;
		tvx[1].Red	= GetRValue(clrBK1);
		tvx[1].Green	= GetGValue(clrBK1);
		tvx[1].Blue	= GetBValue(clrBK1);
		
		GradientFill(dc.GetSafeHdc(), tvx, 2, &grr, 1, GRADIENT_FILL_RECT_V);
		grr.LowerRight	= 0;
		grr.UpperLeft	= 1;
		tvx[0].x		= rcDraw.right;
		tvx[0].y		= rcDraw.bottom;
		tvx[1].x		= rcDraw.left;
		GradientFill(dc.GetSafeHdc(), tvx, 2, &grr, 1, GRADIENT_FILL_RECT_V);

// 		for ( int32 iTop=rcDraw.top; iTop <= rcDraw.bottom; iTop++ )
// 		{
// 			dc.FillSolidRect(rcDraw.left, iTop, rcDraw.Width(), 1, step.NextColor());
// 		}
		if ( bInflateRectAfterFillBk )
		{
			rcDraw.InflateRect(-1, -1);
		}
		
		if ( pImage != NULL )
		{
			Graphics	g(dc.m_hDC);
			//g.DrawImage(pImage, rcDraw.left, rcDraw.right, rcDraw.Width(), rcDraw.Height());
			CSize sizeImage(0, 0);
			sizeImage.cx = pImage->GetWidth();
			sizeImage.cy = pImage->GetHeight();
			if ( rcDraw.Width() > sizeImage.cx )
			{
				rcDraw.left += (rcDraw.Width() - sizeImage.cx)/2;
				rcDraw.right = rcDraw.left + sizeImage.cx;
			}
			if ( rcDraw.Height() > sizeImage.cy )
			{
				rcDraw.top += (rcDraw.Height() - sizeImage.cy)/2;
				rcDraw.bottom = rcDraw.top + sizeImage.cy;
			}
			if ( bGrayImage )	// ͼƬ�ҶȻ�
			{
				ImageAttributes  imageAttributes;
				UINT             width = pImage->GetWidth();
				UINT             height = pImage->GetHeight();
				Status			 staRet;
				
				ColorMatrix colorMatrix = {
					0.299f,	0.299f, 0.299f, 0.0f, 0.0f,
						0.587f,	0.587f, 0.587f,	0.0f, 0.0f,
						0.144f,	0.144f,	0.144f, 0.0f, 0.0f,
						0.0f,	0.0f,	0.0f,	1.0f, 0.0f,
						0.37f,	0.37f,	0.37f,	0.0f, 1.0f};
					
					COLORREF clrDisable = GetSysColor(COLOR_GRAYTEXT);
					ColorMap	clrMapText[100];
					for ( int iMapItem =0; iMapItem < 100 ; iMapItem++ )
					{
						clrMapText[iMapItem].newColor = Color(255, GetRValue(clrDisable), GetGValue(clrDisable), GetBValue(clrDisable));
						clrMapText[iMapItem].oldColor = Color(255, (BYTE)iMapItem, (BYTE)iMapItem, (BYTE)iMapItem);
					}
					
					// 			staRet = imageAttributes.SetRemapTable(100, clrMapText, ColorAdjustTypeBitmap);
					// 			ASSERT( staRet == Ok );
					staRet = imageAttributes.SetColorMatrix(	&colorMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);
					ASSERT( staRet == Ok );
					
					g.DrawImage(
						pImage, 
						Rect(rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height()),  // destination rectangle 
						0, 0,        // upper-left corner of source rectangle 
						width,       // width of source rectangle
						height,      // height of source rectangle
						UnitPixel,
						&imageAttributes);
			}
			else
			{
				CRect rcImage(rcDraw);
				if ( bSelectedLikePush )
				{
					rcImage.OffsetRect(1, 1);
				}
				DrawImage(g, pImage, rcImage, 1, 0, true);
				// 			UINT             width = pImage->GetWidth();
				// 			UINT             height = pImage->GetHeight();
				// 			g.DrawImage(
				// 				pImage, 
				// 				Rect(rcImage.left, rcImage.top, width, height),  // destination rectangle 
				// 				0, 0,        // upper-left corner of source rectangle 
				// 				width,       // width of source rectangle
				// 				height,      // height of source rectangle
				// 				UnitPixel
				// 					);
			}
		}
		else
		{
			rcDraw.InflateRect(-1,-1);
			
			if ( !m_StrText.IsEmpty() && BTNB_SEPARATOR != m_eBtnBehavior )
			{
				dc.SetBkMode(TRANSPARENT);
				// Ϊ�����ۺ�focus�����հף���rcDraw������С��ôһ��
				rcDraw.InflateRect(-2, 0);
				if ( m_bUseMarllet )
				{
					rcDraw.InflateRect(-2, -2);
					CFont MyFont;
					LOGFONT logfont;
					logfont.lfHeight = -rcDraw.Height();
					logfont.lfWidth = 0;
					logfont.lfEscapement = 0;
					logfont.lfOrientation = 0;
					logfont.lfWeight = FW_NORMAL;
					logfont.lfItalic = FALSE;
					logfont.lfUnderline = FALSE;
					logfont.lfStrikeOut = FALSE;
					logfont.lfCharSet = DEFAULT_CHARSET;
					logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
					logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
					logfont.lfQuality = DEFAULT_QUALITY;
					logfont.lfPitchAndFamily = DEFAULT_PITCH;
					
					_tcscpy(logfont.lfFaceName,_T("Marlett"));
					
					MyFont.CreateFontIndirect (&logfont);
					
					CFont* pOldFont = dc.SelectObject (&MyFont); 
					int OldMode = dc.SetBkMode(TRANSPARENT);  
					
					dc.DrawText (m_StrText,1,rcDraw,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
					dc.SetBkMode(OldMode);
					dc.SelectObject(pOldFont);
				}
				else
				{
					dc.DrawText(m_StrText, rcDraw, dwTextDT);
				}
			}
		}
		
		dc.RestoreDC(iSave);
	}

	RectButtonParent					&m_wndTB;		// ������

	CWnd					*m_pWndOwner;
	uint16					m_nID;			// ushort 16 λ
	CString					m_StrText;
	CString					m_StrTip;
	Image					*m_pImageNomal;
	Image					*m_pImageSelected;
	Image					*m_pImageFocus;
	Image					*m_pImageDisable;

	CRect					m_RectButton;		// ��button��TBWnd����ľ���(TBWnd��Client����)
	E_BtnDrawState				m_eBtnState;

	E_BtnBehavior			m_eBtnBehavior;		// ����ͨ��ť����check/radio?
	RectRadioBtnGroup		*m_pRadioGroup;		// �����radio��ť���������� - ����ΪNULL
	bool32					m_bCheck;

	CFont					m_font;
	bool32					m_bUseMarllet;		// ʹ����������marllet����һЩϵͳͼ��

	int						m_iGroupId;			// ������group

	E_DrawStyle				m_eDrawStyle;

	E_ClickLBtnDrawStyle			m_eClickStyle;
	bool32					m_bInLButtonUp;

	TBWndColorItemMap		m_mapColorItems;
};


//////////////////////////////////////////////////////////////////////////
// radio���� - ��ʵ�֣�group����Ҫ�������btn�������ڳ�
template<class RectButtonParent>
class CRectRadioBtnGroup{
public:
	typedef CRectButton<RectButtonParent> RectButton;
	
	void	RemoveAllBtns(){ m_mapRadioBtns.RemoveAll(); }
	int32	GetBtnsCount()const { return m_mapRadioBtns.GetCount(); }
	void	ResetCheckState()		// �����а�ť��check״̬ȡ��
	{
		POSITION pos = m_mapRadioBtns.GetStartPosition();
		while ( pos != NULL )
		{
			void *pKey;
			WORD v;
			m_mapRadioBtns.GetNextAssoc(pos, pKey, v);
			if ( NULL != pKey )
			{
				((RectButton *)pKey)->SetCheck(false);
			}
		}
	}

protected:
	friend RectButton;
	typedef	CMapPtrToWord	TBBtnMap;

	void	AddRadioBtn(RectButton *pBtn)
	{
		WORD v;
		if ( !m_mapRadioBtns.Lookup(pBtn, v) )
		{
			m_mapRadioBtns[pBtn] = 0;		// 
		}
	}
	void	RemoveRadioBtn(RectButton *pBtn)
	{
		WORD v;
		if ( m_mapRadioBtns.Lookup(pBtn, v) )
		{
			m_mapRadioBtns.RemoveKey(pBtn);		// 
		}
	}
	void	IamCheckBtn(RectButton *pBtn)	// ���buttonһ��Ҫ�Ѿ���������飬����button������Ϊfalse
	{
		WORD v;
		if ( m_mapRadioBtns.Lookup(pBtn, v) )
		{
			POSITION pos = m_mapRadioBtns.GetStartPosition();
			while ( pos )
			{
				WORD v;
				void *pKey = NULL;
				m_mapRadioBtns.GetNextAssoc(pos, ((void *)pKey), v);
				if ( pKey && pKey!=pBtn )
				{
					((RectButton *)pKey)->SetCheck(false);
				}
			}
		}
		else
		{
			ASSERT( 0 );
		}
	}

	TBBtnMap	m_mapRadioBtns;		// ��֤��һ����ֻ��һ��radiobutton��ѡ�У����Դ��ڶ�ûѡ�е���� - TBWndҪЭ������Щָ�������
};

// RectButton��α������
template<class RectButtonParentWndType>
class CRectButtonWndParent
{
public:
	typedef CRectButton<RectButtonParentWndType> RectButton;

	// ����
	CRectButtonWndParent() { m_pRcBtnCaptured = NULL; }

	// ��ȡ��DC�����ͷ�
	CDC		*GetDrawNudeDC() { return GetDC(); };
	void	ReleaseDrawNudeDC(CDC *pDC) { ReleaseDC(pDC); };
	
	// �ؼ�id
	//int		GetDlgCtrlID();
	//HWND		GetSafeHwnd();
	
    // Rectת��
	BOOL	ScreenToRectBtnPt(CPoint *ppt){ ScreenToClient(ppt); return TRUE; };		// ��Ļ���굽��ť�����ת�� !!!����ʱ����������ת�������谴ť������dc����һ��	
	
	// focus
	// ��ǰ�Ƿ��а�ť��ȡ��focus
	RectButton *GetCaptureBtn()	{ return m_pRcBtnCaptured; }					
	// ��ť����mouse capture
	void	RequestMouseCapture(RectButton &btn)
	{
		m_pRcBtnCaptured = &btn;
		::SetCapture( btn.GetBtnParent().GetSafeHwnd() );
	}
	// ��ť�����ͷ���ǰ�����mouse capture
	void	ReleaseMouseCapture(RectButton &btn)
	{
		if ( &btn == m_pRcBtnCaptured )
		{
			if ( ::GetCapture()==btn.GetBtnParent().GetSafeHwnd() )
			{
				ReleaseCapture();
			}
			m_pRcBtnCaptured = NULL;
		}
	}

protected:
	RectButton		*m_pRcBtnCaptured;
};

#endif // _RECT_BUTTON_H_