#ifndef  _CONFIG_STRUCT_DEF_05_18_
#define  _CONFIG_STRUCT_DEF_05_18_
#include <vector>
#include <string>
#include <GdiPlus.h>

using namespace Gdiplus;

#define DEL(p) { if (p) delete(p); p = NULL; }

// 按钮类型
enum E_ButtonType
{
	BTYPE_NONE = -1,

    BTYPE_SINGLE = 0,
    BTYPE_PAIR,
    BTYPE_CHILD,

	BTYPE_MAX ,
};

//数据信息类型表示值
enum E_DataClassType
{
	E_DCT_BASE,				//	表示T_DataBase
	E_DCT_BUTTONDATA,		//	表示T_ButtonData
	E_DCT_CHILDBUTTONDATA,	//	表示T_ChildButtonData
	E_DCT_COLUMNDATA,		//	表示T_ColumnData
	E_DCT_TOOLBARDATA		//  表示T_ToolBarData
};

//数据信息类型表示值
enum E_FixedType
{
	EFT_NONE,				//  表示不固定
	EFT_LEFT,				//	表示左固定
	EFT_RIGHT,		        //	表示右固定
};

//基本信息基类
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

// 二级下拉菜单按钮结构体
struct T_ChildButtonData : public T_DataBase
{
    int m_ID;
	int m_iGroupId;							   // 按钮分组ID 
    CString m_Text;
    CString m_FuncText;
    CString m_TextPos;
    std::string m_strImage;                    // 背景图片base64编码
	std::string m_strStatusImage;              // 二级菜单选中状态base64编码
    Image *m_pBkImage;                         // 背景图片指针
	Image *m_pImgStatus;                       // 二级菜单选中状态
    CString  m_EventType;
    CString  m_EventParam;
    CString m_DialogWidth;
    CString m_DialogHeight;
	std::vector<T_ChildButtonData> m_ChildSubMenuData;      // 下拉菜单按钮集
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
	int m_iGroupId;							            // 按钮分组ID 
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
    std::vector<T_ChildButtonData> m_ChildBtnData;      // 下拉菜单按钮集
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
    int m_iColWidth;                            // 列宽
	E_FixedType m_eFixedType;					// 固定类型
    std::vector<T_ButtonData> m_BtnData;        // 包含的按钮集

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
    Image *m_pBkImage;                             // 背景图片指针
    std::string m_strImage;
    CString m_SimpleBtnFont;
    CString m_CategoryBtnFont;
    CString m_SimpleFontSize;
    CString m_CategoryFontSize;
    int     m_iHeight;                             // 工具栏高度
    int     m_iLeftSpace;                          // 左侧间距
    int     m_iTopSpace;                           // 上侧间距
    int     m_iBottomSpace;                        // 下侧间距

    void Clear()
    {
        m_pBkImage = NULL;
        m_strImage = "";
        m_SimpleBtnFont = L"宋体";
        m_CategoryBtnFont = L"宋体";
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