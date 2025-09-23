#ifndef  _CONFIG_STRUCT_DEF_05_18_
#define  _CONFIG_STRUCT_DEF_05_18_
#include <vector>
#include <string>
#include <GdiPlus.h>

using namespace Gdiplus;

#define DEL(p) { if (p) delete(p); p = NULL; }

// ��ť����
enum E_ButtonType
{
	BTYPE_NONE = -1,

    BTYPE_SINGLE = 0,
    BTYPE_PAIR,
    BTYPE_CHILD,

	BTYPE_MAX ,
};

//������Ϣ���ͱ�ʾֵ
enum E_DataClassType
{
	E_DCT_BASE,				//	��ʾT_DataBase
	E_DCT_BUTTONDATA,		//	��ʾT_ButtonData
	E_DCT_CHILDBUTTONDATA,	//	��ʾT_ChildButtonData
	E_DCT_COLUMNDATA,		//	��ʾT_ColumnData
	E_DCT_TOOLBARDATA		//  ��ʾT_ToolBarData
};

//������Ϣ���ͱ�ʾֵ
enum E_FixedType
{
	EFT_NONE,				//  ��ʾ���̶�
	EFT_LEFT,				//	��ʾ��̶�
	EFT_RIGHT,		        //	��ʾ�ҹ̶�
};

//������Ϣ����
struct T_DataBase 
{
	DWORD	m_crlTextColor;
	float   m_fFontSize;
	E_DataClassType	m_classType;
	T_DataBase()
	{
		m_crlTextColor	= RGB(187, 193, 199);
		m_fFontSize     = 0.0;
		m_classType		= E_DCT_BASE;
	}	
};

// ���������˵���ť�ṹ��
struct T_ChildButtonData : public T_DataBase
{
    int m_ID;
	int m_iGroupId;							   // ��ť����ID 
    CString m_Text;
    CString m_FuncText;
    CString m_TextPos;
    std::string m_strImage;                    // ����ͼƬbase64����
	std::string m_strStatusImage;              // �����˵�ѡ��״̬base64����
    Image *m_pBkImage;                         // ����ͼƬָ��
	Image *m_pImgStatus;                       // �����˵�ѡ��״̬
    CString  m_EventType;
    CString  m_EventParam;
    CString m_DialogWidth;
    CString m_DialogHeight;
	std::vector<T_ChildButtonData> m_ChildSubMenuData;      // �����˵���ť��
	bool	m_bShowText;

    void Clear()
    {
        m_ID = -1;
		m_iGroupId = -1;
        m_Text = L"";
        m_FuncText = L"";
        m_TextPos = L"";
        m_strImage = "";
		m_strStatusImage = "";
        m_pBkImage = NULL;
		m_pImgStatus = NULL;
        m_EventType = L"";
        m_EventParam = L"";
        m_DialogWidth = L"";
        m_DialogHeight = L"";
		m_classType	= E_DCT_CHILDBUTTONDATA;
		m_crlTextColor	= RGB(187, 193, 199);	
		m_fFontSize     = 0.0;
		m_ChildSubMenuData.clear();
		m_bShowText = true;
    }

    T_ChildButtonData()
    {
        Clear();
    }
};

struct T_ButtonData : public T_DataBase
{
    int m_ID;
	int m_iGroupId;							            // ��ť����ID 
    CString m_Text;
    CString m_FuncText;
    CString m_TextPos;
	CString m_AreaPos;
    Image   *m_pBkImage;
    std::string m_strImage;
    CString  m_EventType;
    CString  m_EventParam;
    CString m_DialogWidth;
    CString m_DialogHeight;
    std::vector<T_ChildButtonData> m_ChildBtnData;      // �����˵���ť��
	bool	m_bShowText;

    void Clear()
    {
        m_ID = -1;
		m_iGroupId = -1;
        m_Text = L"";
        m_FuncText = L"";
        m_TextPos = L"";
		m_AreaPos = L"";
        m_pBkImage = NULL;
        m_EventType = L"";
        m_strImage = "";
        m_EventParam = L"";
        m_DialogWidth = L"";
        m_DialogHeight = L"";
        m_ChildBtnData.clear();
		m_classType = E_DCT_BUTTONDATA;
		m_crlTextColor	= RGB(187, 193, 199);	
		m_bShowText = true;
    }

    T_ButtonData()
    {
        Clear();
    }

	void ReleaseImageResource()
	{
		std::vector<T_ChildButtonData>::iterator it;
		for (it = m_ChildBtnData.begin() ; it != m_ChildBtnData.end() ; ++it)
		{
			if (NULL != it->m_pBkImage)
			{
				delete (it->m_pBkImage);
				it->m_pBkImage = NULL;
			}

			if (NULL != it->m_pImgStatus)
			{
				delete (it->m_pImgStatus);
				it->m_pImgStatus = NULL;
			}
		}
		std::vector<T_ChildButtonData>().swap(m_ChildBtnData);
	}
};

struct T_ColumnData :public T_DataBase
{
    int m_ID;
    int m_iColWidth;                            // �п�
	E_FixedType m_eFixedType;					// �̶�����
    std::vector<T_ButtonData> m_BtnData;        // �����İ�ť��

    T_ColumnData()
    {
        m_ID = -1;
        m_iColWidth = 0;
		m_eFixedType = EFT_NONE;
        m_BtnData.clear();
		m_classType	= E_DCT_COLUMNDATA;
    }

	void ReleaseImage()
	{
		std::vector<T_ButtonData>::iterator it;
		for (it = m_BtnData.begin() ; it != m_BtnData.end() ; ++it)
		{
			if (NULL != it->m_pBkImage)
			{
				it->ReleaseImageResource();
				delete (it->m_pBkImage);
				it->m_pBkImage = NULL;
			}
		}
		std::vector<T_ButtonData>().swap(m_BtnData);
	}
};

struct T_ToolBarData :public T_DataBase
{
    Image *m_pBkImage;                             // ����ͼƬָ��
    std::string m_strImage;
    CString m_SimpleBtnFont;
    CString m_CategoryBtnFont;
    CString m_SimpleFontSize;
    CString m_CategoryFontSize;
    int     m_iHeight;                             // �������߶�
    int     m_iLeftSpace;                          // �����
    int     m_iTopSpace;                           // �ϲ���
    int     m_iBottomSpace;                        // �²���

    void Clear()
    {
        m_pBkImage = NULL;
        m_strImage = "";
        m_SimpleBtnFont = L"����";
        m_CategoryBtnFont = L"����";
        m_SimpleFontSize = L"9";
        m_CategoryFontSize = L"9";
        m_iHeight = 0;
        m_iLeftSpace = 0;
        m_iTopSpace = 0;
        m_iBottomSpace = 0;
		m_classType	= E_DCT_TOOLBARDATA;
    }

    T_ToolBarData()
    {
        Clear();
    }
};


#endif