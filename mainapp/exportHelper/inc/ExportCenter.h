#ifndef _EXPORTCENTER_H_
#define _EXPORTCENTER_H_

//class CExportTest;

class CExportCenter
{
public:
	~CExportCenter();

	static CExportCenter &Instance();

public:
	bool	Init();
	void	UnInit();

	void	StartTest();
	void	StopTest();

	void	DoTest2();

private:
	CExportCenter();

private:
	bool		m_bInited;

//	CExportTest *m_pTest;
};

#endif //!_EXPORTCENTER_H_