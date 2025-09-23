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

	void TraceWithTimestamp(LPCTSTR lpszMsg, BOOL bUseRecord = FALSE, SYSTEMTIME *ptimeStamp=NULL);		// �Ӹ�ʱ��������ptimeStampΪNULL����ʹ�õ�ǰʱ��
	
	
	static XLTraceFile &GetXLTraceFile();

	static XLTraceFile &GetXLTraceFile(LPCTSTR pszFileName);		// ����һ����ʱ�Ķ��󣬲�Ҫ������
	
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