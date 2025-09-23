// UtilTool.h: interface for the CUtilTool class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_UTILTOOL_H__31262CC4_4DE1_4651_B6C7_D6B6CB9B3AF2__INCLUDED_)
#define AFX_UTILTOOL_H__31262CC4_4DE1_4651_B6C7_D6B6CB9B3AF2__INCLUDED_

#include <Afx.h>
#include <string>
#include <windows.h>   
#include <iostream>   
#include <fstream>   

class AFX_EXT_CLASS CUtilTool  
{
private:

	CUtilTool();

	void IntToChar(int nvalue,TCHAR *pbuf);

	int CharToInt(const TCHAR *pstr,int nbits);
public:	
	virtual ~CUtilTool();

	static CUtilTool &GetInistance();


	void TimeToChar(const CTime& time,CString &strTime);

	void CharToTime (CString strTime,CTime &time);
	

};
  
#endif // !defined(AFX_UTILTOOL_H__31262CC4_4DE1_4651_B6C7_D6B6CB9B3AF2__INCLUDED_)