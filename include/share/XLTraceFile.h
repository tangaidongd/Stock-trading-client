#ifndef _XLTRACEFILE_H_
#define _XLTRACEFILE_H_

#include <stdarg.h>

class AFX_EXT_CLASS XLTraceFile{
public:
	XLTraceFile(LPCTSTR pszFileName, bool bResetFile = false);

	~XLTraceFile();

	void FlushXLTraceFile(){ m_file.Flush(); };
	void CloseXLTraceFile();;

	void ResetFileLength();

	void Trace0(LPCTSTR lpszMsg);
	
	void Trace(LPCTSTR lpszFormat, ...);

	void TraceWithTimestamp(LPCTSTR lpszMsg, BOOL bUseRecord = FALSE, SYSTEMTIME *ptimeStamp=NULL);		// 加个时间戳，如果ptimeStamp为NULL，则使用当前时间
	
	
	static XLTraceFile &GetXLTraceFile();

	static XLTraceFile &GetXLTraceFile(LPCTSTR pszFileName);		// 返回一个临时的对象，不要保存它
	
private:
	XLTraceFile(const XLTraceFile &);
	
	BOOL InitializeXLTraceFile();

	BOOL IsFileOK();
	
	const static CString c_StrFileName;

	CStdioFile m_file;
	CString	   m_StrFileName;
	ULARGE_INTEGER	m_timeLastRecord;
};

#endif