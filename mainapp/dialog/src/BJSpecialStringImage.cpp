#include "StdAfx.h"
#include "BJSpecialStringImage.h"
#include "XmlShare.h"
#include "PathFactory.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
static const TCHAR KStrBeiJingStringSection[] = _T("BeiJingString");
static const TCHAR KStrBeiJingImageSection[] = _T("BeiJingImage");

CBJSpecialStringImage & CBJSpecialStringImage::Instance()
{
	static CBJSpecialStringImage sObj;
	return sObj;
}

void CBJSpecialStringImage::Init()
{
#define PutMap_XL(m, k, v) {CString StrTmp;\
							CEtcXmlConfig::Instance().ReadEtcConfig(KStrBeiJingStringSection, v, v, StrTmp);\
							m[k] = StrTmp;}
#define PutMap_XL2(m, k, v, dv) {CString StrTmp;\
	CEtcXmlConfig::Instance().ReadEtcConfig(KStrBeiJingStringSection, v, dv, StrTmp);\
							m[k] = StrTmp;}
#define PutMap_XL_Image(m, k, v) {CString StrTmp;\
	CEtcXmlConfig::Instance().ReadEtcConfig(KStrBeiJingImageSection, v, v, StrTmp);\
							m[k] = StrTmp;}
	PutMap_XL_Image(m_mapId2String, ToolBar_DPCD, _T("���̳���"));
	PutMap_XL_Image(m_mapId2String, ToolBar_DPTD, _T("�����Ӷ�"));
	PutMap_XL_Image(m_mapId2String, ToolBar_NXFJ, _T("ţ�ֽܷ�"));
	PutMap_XL_Image(m_mapId2String, ToolBar_DXCP, _T("���߲���"));
	PutMap_XL_Image(m_mapId2String, ToolBar_ZXCP, _T("���߲���"));
	PutMap_XL_Image(m_mapId2String, ToolBar_ZJLX, _T("�ʽ�����"));
	PutMap_XL_Image(m_mapId2String, ToolBar_DTXG, _T("��̬ѡ��"));
	PutMap_XL_Image(m_mapId2String, ToolBar_DDPH, _T("������"));
	PutMap_XL_Image(m_mapId2String, ToolBar_RDJJ, _T("�ȵ���"));
	PutMap_XL_Image(m_mapId2String, ToolBar_RMQS, _T("����ǿ��"));
	PutMap_XL_Image(m_mapId2String, ToolBar_YJSD, _T("һ������"));

	
	PutMap_XL(m_mapId2String, String_DPZLCB, _T("���������ɱ�"));
	PutMap_XL(m_mapId2String, String_DPCWTX, _T("���̲�λ����"));
	PutMap_XL(m_mapId2String, String_GGZLCB, _T("���������ɱ�"));
	PutMap_XL(m_mapId2String, String_GGCWTX, _T("���ɲ�λ����"));
	PutMap_XL(m_mapId2String, String_JGLT, _T("������̧"));
	PutMap_XL(m_mapId2String, String_XPJS, _T("ϴ�̽���"));
	PutMap_XL(m_mapId2String, String_ZJZT, _T("׷����ͣ"));
	PutMap_XL(m_mapId2String, String_NSQL, _T("����ǿ��"));
	PutMap_XL(m_mapId2String, String_CDFT, _T("��������"));

	PutMap_XL(m_mapId2String, String_DPCL, _T("���̲���"));
	PutMap_XL(m_mapId2String, String_DPHSSSCL, _T("���̺�ɫ��������"));
	PutMap_XL(m_mapId2String, String_DPLSTZCL, _T("������ɫ��������"));
	PutMap_XL(m_mapId2String, String_DPLSXDCL, _T("������ɫ�µ�����"));
	PutMap_XL(m_mapId2String, String_DPHSZDCL, _T("���̻�ɫ�𵴲���"));
	PutMap_XL(m_mapId2String, String_DPBDHQCL, _T("���̱����������"));

	PutMap_XL(m_mapId2String, String_DDQSG, _T("��ǿ�ƹ�"));
	PutMap_XL(m_mapId2String, String_RMQSG, _T("����ǿ�ƹ�"));
	PutMap_XL(m_mapId2String, String_ZJQSG, _T("�ʽ�ǿ�ƹ�"));

	PutMap_XL2(m_mapId2String, String_DDPH_DDPH, _T("������2"), _T("������"));
	PutMap_XL2(m_mapId2String, String_DDPH_DDTJ, _T("��ͳ��2"), _T("��ͳ��"));

	PutMap_XL(m_mapId2String, String_DPCD, _T("���̳���"));
	PutMap_XL(m_mapId2String, String_DPTD, _T("�����Ӷ�"));
	PutMap_XL(m_mapId2String, String_NXFJ, _T("ţ�ֽܷ�"));
	PutMap_XL(m_mapId2String, String_DXCP, _T("���߲���"));
	PutMap_XL(m_mapId2String, String_ZXCP, _T("���߲���"));
	PutMap_XL(m_mapId2String, String_GGCP, _T("���ɲ���"));
	PutMap_XL(m_mapId2String, String_ZJLX, _T("�ʽ�����"));
	PutMap_XL(m_mapId2String, String_DXXG, _T("����ѡ��"));
	PutMap_XL(m_mapId2String, String_ZXXG, _T("����ѡ��"));
	PutMap_XL(m_mapId2String, String_MRJX, _T("���뾫ѡ"));
	PutMap_XL(m_mapId2String, String_ZLXG, _T("ս��ѡ��"));
	PutMap_XL(m_mapId2String, String_DDPH, _T("������"));
	PutMap_XL(m_mapId2String, String_RDWJ, _T("�ȵ��ھ�"));
	PutMap_XL(m_mapId2String, String_ZLJK, _T("�������"));
	PutMap_XL(m_mapId2String, String_CPTX, _T("��������"));
	PutMap_XL(m_mapId2String, String_CWFX, _T("��λ����"));
	PutMap_XL(m_mapId2String, String_ZDYC, _T("�ǵ�Ԥ��"));
	PutMap_XL(m_mapId2String, String_ZLCB, _T("�����ɱ�"));
	PutMap_XL(m_mapId2String, String_GGCD, _T("���ɳ���"));
	PutMap_XL(m_mapId2String, String_GGTD, _T("�����Ӷ�"));
	PutMap_XL(m_mapId2String, String_NXQR, _T("ţ��ǿ��"));
	PutMap_XL(m_mapId2String, String_XGXH, _T("ѡ���ź�"));
	PutMap_XL(m_mapId2String, String_QSJX, _T("ǿ�ƾ�ѡ"));

	PutMap_XL2(m_mapId2String, String_DXXG_D, _T("����ѡ�ɶ�"), _T("����ѡ��(��)"));
	PutMap_XL2(m_mapId2String, String_ZXXG_D, _T("����ѡ�ɶ�"), _T("����ѡ��(��)"));
	PutMap_XL(m_mapId2String, String_ZLCD, _T("ս�Գ���"));
	PutMap_XL(m_mapId2String, String_BDCD, _T("���γ���"));
	//PutMap_XL(m_mapId2String, String_CDFT, _T("��������")); // ��ǰ�У��ظ�
	PutMap_XL2(m_mapId2String, String_JDXG_D, _T("���ѡ�ɶ�"), _T("���ѡ��(��)"));

	PutMap_XL2(m_mapId2String, String_DXXG_K, _T("����ѡ�ɿ�"), _T("����ѡ��(��)"));
	PutMap_XL2(m_mapId2String, String_ZXXG_K, _T("����ѡ�ɿ�"), _T("����ѡ��(��)"));
	PutMap_XL(m_mapId2String, String_ZLZK_K, _T("ս������"));
	PutMap_XL(m_mapId2String, String_BDZK_K, _T("��������"));
	PutMap_XL2(m_mapId2String, String_CMHT_K, _T("����ص���"), _T("����ص�(��)"));
	PutMap_XL2(m_mapId2String, String_JDXG_K, _T("���ѡ�ɿ�"), _T("���ѡ��(��)"));

	PutMap_XL(m_mapId2String, String_WebSiteBtn, _T("��վ"));
	PutMap_XL(m_mapId2String, String_TradeBtn, _T("����"));
	PutMap_XL(m_mapId2String, String_TradeMode, _T("����ģʽ"));

#undef PutMap_XL
#undef PutMap_XL2
#undef PutMap_XL_Image
}

CString CBJSpecialStringImage::GetSpecialString( const LPCTSTR lpszDefault )
{
	CString StrTmp;
	if ( NULL != lpszDefault )
	{
		CEtcXmlConfig::Instance().ReadEtcConfig(KStrBeiJingStringSection, lpszDefault, lpszDefault, StrTmp);
	}
	return StrTmp;
}

CString CBJSpecialStringImage::GetSpecialString( E_Id eId )
{
	Id2StringMap::iterator it = m_mapId2String.find(eId);
	if ( it != m_mapId2String.end() )
	{
		return it->second;
	}
	return CString();
}

Image * CBJSpecialStringImage::LoadSpecailImage( E_Id eImage )
{
	Id2StringMap::iterator it = m_mapId2String.find(eImage);
	if ( it != m_mapId2String.end() )
	{
		CString StrImagePath = CPathFactory::GetImagePath();
		StrImagePath += it->second;
		StrImagePath += _T(".png");		// image/xx.png
		Image *pImage = Image::FromFile(StrImagePath);	// ���ļ��м���
		if ( NULL != pImage && pImage->GetLastStatus() != Ok )
		{
			delete pImage;	// ����del��
			pImage = NULL;
		}
		return pImage;
	}
	return NULL;
}

void CBJSpecialStringImage::GetMenuId2StringMap( OUT Id2StringMap &mapId2String )
{
	mapId2String = m_mapMenuId2String;
}

void CBJSpecialStringImage::SetMenuId2StringMap( const Id2StringMap &mapId2String )
{
	m_mapMenuId2String = mapId2String;
}
