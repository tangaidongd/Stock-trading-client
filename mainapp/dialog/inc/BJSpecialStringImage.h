#ifndef _BJSPECAILSTRINGIMAGE_H_
#define _BJSPECAILSTRINGIMAGE_H_
#include "GdiPlusTS.h"
#include <map>
using std::map;

// ������Ŀ���⻰��һЩ�ִ���Image�ļ������ﴦ��
class CBJSpecialStringImage
{
public:
	typedef map<DWORD, CString> Id2StringMap;
	enum E_Id
	{
		ToolBar_DPCD = 1,		// ���̳���
		ToolBar_DPTD,			// �����Ӷ�
		ToolBar_NXFJ,			// ţ�ֽܷ�
		ToolBar_DXCP,			// ���߲���
		ToolBar_ZXCP,			// ���߲���
		ToolBar_ZJLX,			// �ʽ�����
		ToolBar_YJSD,			// һ������
		ToolBar_DTXG,			// ��̬ѡ��
		ToolBar_RDJJ,			// �ȵ���
		ToolBar_DDPH,			// ������
		ToolBar_RMQS,			// ����ǿ�� ˽ļǿ��


		String_Start = 100,
		String_DPZLCB,			// ���̻����ɱ�
		String_DPCWTX,			// ���̲�λ����
		String_GGZLCB,			// ���ɻ����ɱ�
		String_GGCWTX,			// ���ɲ�λ����
		String_JGLT,			// ������̧
		String_XPJS,			// ϴ�̽���
		String_ZJZT,			// ׷����ͣ
		String_NSQL,			// ����ǿ��
		String_CDFT,			// ��������

		String_DPCL,			// ���̲���
		String_DPHSSSCL,		// ���̺�ɫ��������
		String_DPLSTZCL,		// ������ɫ��������
		String_DPLSXDCL,		// ������ɫ�µ�����
		String_DPHSZDCL,		// ���̻�ɫ�𵴲���
		String_DPBDHQCL,		// ���̱����������

		String_DDQSG,			// ��ǿ�ƹ�
		String_RMQSG,			// ����ǿ�ƹ�
		String_ZJQSG,			// �ʽ�ǿ�ƹ�

		String_DDPH_DDTJ,		// ������ - ��ͳ��
		String_DDPH_DDPH,		// ������ - ������

		String_ZLCP,			// ս�Բ���
		String_ZQGZ,			// ���ڹ���
		String_ZLKP,			// ս�Կ���

		String_DPCD,			// ���̳���
		String_DPTD,			// �����Ӷ�
		String_NXFJ,			// ţ�ֽܷ�
		String_DXCP,			// ���߲���
		String_ZXCP,			// ���߲���
		String_GGCP,			// ���ɲ���
		String_ZJLX,			// �ʽ�����
		String_DXXG,			// ����ѡ��
		String_ZXXG,			// ����ѡ��
		String_MRJX,			// ���뾫ѡ
		String_ZLXG,			// ս��ѡ��
		String_DDPH,			// ������
		String_RDWJ,			// �ȵ��ھ�
		String_ZLJK,			// �������
		String_CPTX,			// ��������
		String_CWFX,			// ��λ����
		String_ZDYC,			// �ǵ�Ԥ��
		String_ZLCB,			// �����ɱ�
		String_GGCD,			// ���ɳ���
		String_GGTD,			// �����Ӷ�
		String_NXQR,			// ţ��ǿ��
		String_XGXH,			// ѡ���ź�
		String_QSJX,			// ǿ�ƾ�ѡ

		String_DXXG_D,			// ����ѡ�ɶ�
		String_ZXXG_D,			// ����ѡ�ɶ�
		String_ZLCD,			// ս�Գ���
		String_BDCD,			// ���γ���
		//String_CDFT,			// ��������
		String_JDXG_D,			// ���ѡ�ɶ�
		String_DXXG_K,			// ����ѡ�ɿ�
		String_ZXXG_K,			// ����ѡ�ɿ�
		String_ZLZK_K,			// ս������
		String_BDZK_K,			// ��������
		String_CMHT_K,			// ����ص���
		String_JDXG_K,			// ���ѡ�ɿ�

		String_WebSiteBtn,			// ��վ��ַ��ť
		String_TradeBtn,			// ���װ�ť
		String_TradeMode,			// ����ģʽ
	};

	static CBJSpecialStringImage &Instance();
	~CBJSpecialStringImage(){}

	CString	GetSpecialString(E_Id eId);		// ��ȡ���Զ����ִ�
	Image *LoadSpecailImage(E_Id eImage);	// �����Զ����ͼƬ
	CString GetSpecialString(const LPCTSTR lpszDefault);	// ����ԭ�����ִ���ȡ�����Զ�����ִ�

	void	GetMenuId2StringMap(OUT Id2StringMap &mapId2String);	// ��ȡ��ɫ�˵��µ�id����ʾ�ִ�
	void	SetMenuId2StringMap(const Id2StringMap &mapId2String);	// ������ɫ�˵��µ�id����ʾ�ִ�

private:
	CBJSpecialStringImage(){ Init(); };
	CBJSpecialStringImage(const CBJSpecialStringImage &);
	CBJSpecialStringImage &operator=(const CBJSpecialStringImage &);

	void	Init();		// ��ʼ��Ĭ����ʾ����
	
	
	Id2StringMap	m_mapId2String;
	Id2StringMap	m_mapMenuId2String;
};

#endif //_BJSPECAILSTRINGIMAGE_H_