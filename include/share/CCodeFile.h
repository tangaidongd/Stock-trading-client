#ifndef _CODE_FILE_H_ 
#define _CODE_FILE_H_
#include "typedef.h"
class TiXmlElement;

// 加密
class AFX_EXT_CLASS CCodeFile
{
public:
	enum E_CodeFileType				// 加密的类型
	{
		ECFTBase64 = 0,				// base64
		ECFTOwn,					// 自定义的. 在文件末加上加密标志, 每个字节异或 sKcCode 再交换高4位和低4位	
		ECFTCount
	};

public:
	// base64 算法
	static int32			base64_encode( unsigned char *dst, int32 *dlen, const unsigned char *src, int32 slen);				  
	static int32			base64_decode( unsigned char *dst, int32 *dlen, const unsigned char *src, int32 slen);
				  

	// 原来的加密方法,加密整个文件
	static bool32		EnCodeFile(const CString& StrFilePath, E_CodeFileType eCodeFileType = ECFTOwn);							// 加密文件
	static bool32		DeCodeFile(const CString& StrFilePath, E_CodeFileType eCodeFileType = ECFTOwn);							// 解密文件
	static void			SwitchHiLoByte(INOUT char& charToSwitch);																// 交换一个char 的高低四位	

	// 加密字符串
	static bool32		EnCodeString(char * pInput, int32 iLenInput, char * pOutput, int32 *pLenOutput, E_CodeFileType eCodeFileType);
	static bool32		DeCodeString(char * pInput, int32 iLenInput, char * pOutput, int32 *pLenOutput, E_CodeFileType eCodeFileType);

	// 加密某个xml 节点
	static char*		EnCodeXmlNode(TiXmlElement* pNode, E_CodeFileType eCodeFileType = ECFTBase64);

	//CRC32校验
	static DWORD		crc_32(const char *pBuf, int iLen);
};

#endif  //_CODE_FILE_H_