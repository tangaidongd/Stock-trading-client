#ifndef __FACE_SCHEME_TYPE_LL__H_
#define __FACE_SCHEME_TYPE_LL__H_

// ϵͳ��ɫ
enum E_SysColor 
{
	ESCBackground = 0,		// ����ɫ

	ESCText,				// ��ͨ�ı���ɫ��Ĭ���ı���ɫ

	ESCChartAxisLine,		// chartͼ����������ɫ

	ESCKLineRise,			// K������ͼ����ɫ
	ESCKLineKeep,			// K�߳�ƽͼ����ɫ
	ESCKLineFall,			// K���µ�ͼ����ɫ

	ESCRise,				// ������ɫ
	ESCKeep,				// ƽ����ɫ
	ESCFall,				// �µ���ɫ

	ESCVolume,				// �ɽ�����ɫ
	ESCAmount,				// �ɽ������ɫ

	ESCGridLine,			// ���ָ���
	ESCGridSelected,		// ���ѡ������ɫ
	//...fangz0531		 xinjia
	ESCGridFixedBk,			// ���̶����ɫ
	//ESCGridFixedText,		// ���̶���������ɫ
	ESCTitleBkColor,        //���ⱳ����ɫ

	ESCSpliter,				// �ָ�����ɫ
	ESCVolume2,				// ���۱���ʾ�ĳɽ�����ɫ

	ESCGuideLine1,			// ָ����1
	ESCGuideLine2,			// ָ����2
	ESCGuideLine3,			// ָ����3
	ESCGuideLine4,			// ָ����4
	ESCGuideLine5,			// ָ����5
	ESCGuideLine6,			// ָ����6
	ESCGuideLine7,			// ָ����7
	ESCGuideLine8,			// ָ����8
	ESCGuideLine9,			// ָ����9
	ESCGuideLine10,			// ָ����10
	ESCGuideLine11,			// ָ����11
	ESCGuideLine12,			// ָ����12
	ESCGuideLine13,			// ָ����13
	ESCGuideLine14,			// ָ����14
	ESCGuideLine15,			// ָ����15
	ESCGuideLine16,			// ָ����16


	// 
	ESCCount
};
// ϵͳ����
enum E_SysFont
{
	ESFNormal = 0,			// ��������
	ESFSmall,				// С����
	ESFBig,					// ������
	ESFText,				// �ı�����
	ESFCount				
};

#endif