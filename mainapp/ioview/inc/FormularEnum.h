// FormularEnum.h: interface for the CFormularEnum class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FORMULAR_ENUM_H
#define _FORMULAR_ENUM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


extern CString StrUsesEnumMsg;
#define ENUM_MSG	\
{	\
	StrUsesEnumMsg.Format(L"�˴���ֲδ���(%s�ļ�%d��)",__FILE__,__LINE__);	\
	TRACE(StrUsesEnumMsg);	\
}


#endif //_FORMULAR_ENUM_H
