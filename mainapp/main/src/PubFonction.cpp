#include "stdafx.h"
#include "PubFonction.h"
#include "base64.h"
#include "coding.h"

Image * TransBase64CodeToImage( char* str )
{
    if(NULL == str)
    {
        return NULL;
    }

    Image *pImage = NULL;

    int len = strlen(str);
    char* strDecode = new char[len];
    memset(strDecode, 0, len);

    int lenDecode = Base64Decode(strDecode, str, len);
    if(lenDecode > 0 && strDecode != NULL)
    {
        ImageFromByte((BYTE*)strDecode, lenDecode, pImage);
    }

    DEL_ARRAY(strDecode);
    return pImage;
}

char*  TransImageToBase64Code(CString strImgPath)
{
    CFile data(strImgPath, CFile::modeRead | CFile::shareDenyNone);
    int len = data.GetLength();
    char *srcData = new char[len];
    memset(srcData, 0, len);
    data.Read(srcData, len);
    data.Close();

    int slen = (len / 3) * 4;
    slen += 10;

    char* str64Code = new char[slen];
    memset(str64Code, 0, slen);
    Base64Encode(str64Code, srcData, len);

    DEL_ARRAY(srcData);

    return str64Code;
}

bool IsAllNumbers(CString str)
{
    if(str.IsEmpty())
    {
        return false;
    }

    for(int i=0;i<str.GetLength();i++)
    {
        if(((int)str.GetAt(i)<48 || (int)str.GetAt(i)>57))
        {
            return false;
        }
    }

    return true;
}

bool IsCombineByNum(std::string strParam)
{
    if(strParam.empty())
    {
        return false;
    }

    for(int i=0;i<strParam.length();i++)
    {
        if(((int)strParam[i] < 48 || (int)strParam[i] > 57))
        {
            return false;
        }
    }

    return true;
}

char buf[512];
char text[]="这是检验值，用来判断密码是否正确";//放在文件头用于密码校验，防止错误的密码破坏源文件
const int TextLength=strlen(text);

CString GetTmpPath()
{
    TCHAR szModule[_MAX_PATH];  
    GetModuleFileName(NULL, szModule, _MAX_PATH);
    CString StrPath = szModule;
    CString StrAbsPath = StrPath.Left(StrPath.ReverseFind('\\'));
    StrAbsPath += L"\\tmp";

    return StrAbsPath;
}

//加密函数
int Encrypt(const char path[])
{
    char key[] = "12345678";

    FILE *fp=fopen(path,"rb");
    if(fp==NULL)
        return -1;

    CString StrAbsPath = GetTmpPath();

	string sPath = _Unicode2MultiChar(StrAbsPath);
	const char* pStrPath = sPath.c_str();

    char tmp[_MAX_PATH];
    memset(tmp, 0, _MAX_PATH);
    strcpy(tmp, pStrPath);

    FILE *fp2=fopen(tmp,"wb");
    int n,i,KeyLength=strlen(key);
    for(i=0;i<TextLength;i++)
        buf[i]=text[i]^key[i%KeyLength];
    fwrite(buf,sizeof(char),TextLength,fp2);//TextLength不能写成sizeof(text)
    while((n=fread(buf,sizeof(char),KeyLength,fp))>0)
    {
        for(i=0;i<n;i++)
            buf[i]^=key[i];
        fwrite(buf,sizeof(char),n,fp2);
    }
    fclose(fp);
    fclose(fp2);
    remove(path);
    rename(tmp,path);
    return 0;
}

//解密函数
int Decrypt(const char path[])
{
    CString StrAbsPath = GetTmpPath();

	string sPath = _Unicode2MultiChar(StrAbsPath);
	const char* pStrPath = sPath.c_str();

    char tmp[_MAX_PATH];
    memset(tmp, 0, _MAX_PATH);
    strcpy(tmp, pStrPath);

    char key[] = "12345678";
    FILE *fp=fopen(path,"rb");

    if(fp==NULL)
    {
        return -1;
    }
    int i,KeyLength=strlen(key);
    int n=fread(buf,sizeof(char),TextLength,fp);
    for(i=0;i<n;i++)
        buf[i]^=key[i%KeyLength];
    buf[n]='\0';
    if(strcmp(buf,text)!=0)
    {
        fclose(fp);
        return 1;
    }	

    FILE *fp2=fopen(tmp,"wb");

    while((n=fread(buf,sizeof(char),KeyLength,fp))>0)
    {
        for(i=0;i<n;i++)
            buf[i]^=key[i];
        fwrite(buf,sizeof(char),n,fp2);
    }
    fclose(fp);
    fclose(fp2);
    remove(path);
    rename(tmp,path);
    return 0;
}
