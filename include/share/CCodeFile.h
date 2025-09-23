#ifndef _CODE_FILE_H_ 
#define _CODE_FILE_H_
#include "typedef.h"
class TiXmlElement;

// ����
class AFX_EXT_CLASS CCodeFile
{
public:
	enum E_CodeFileType				// ���ܵ�����
	{
		ECFTBase64 = 0,				// base64
		ECFTOwn,					// �Զ����. ���ļ�ĩ���ϼ��ܱ�־, ÿ���ֽ���� sKcCode �ٽ�����4λ�͵�4λ	
		ECFTCount
	};

public:
	// base64 �㷨
	static int32			base64_encode( unsigned char *dst, int32 *dlen, const unsigned char *src, int32 slen);				  
	static int32			base64_decode( unsigned char *dst, int32 *dlen, const unsigned char *src, int32 slen);
				  

	// ԭ���ļ��ܷ���,���������ļ�
	static bool32		EnCodeFile(const CString& StrFilePath, E_CodeFileType eCodeFileType = ECFTOwn);							// �����ļ�
	static bool32		DeCodeFile(const CString& StrFilePath, E_CodeFileType eCodeFileType = ECFTOwn);							// �����ļ�
	static void			SwitchHiLoByte(INOUT char& charToSwitch);																// ����һ��char �ĸߵ���λ	

	// �����ַ���
	static bool32		EnCodeString(char * pInput, int32 iLenInput, char * pOutput, int32 *pLenOutput, E_CodeFileType eCodeFileType);
	static bool32		DeCodeString(char * pInput, int32 iLenInput, char * pOutput, int32 *pLenOutput, E_CodeFileType eCodeFileType);

	// ����ĳ��xml �ڵ�
	static char*		EnCodeXmlNode(TiXmlElement* pNode, E_CodeFileType eCodeFileType = ECFTBase64);

	//CRC32У��
	static DWORD		crc_32(const char *pBuf, int iLen);
};

#endif  //_CODE_FILE_H_