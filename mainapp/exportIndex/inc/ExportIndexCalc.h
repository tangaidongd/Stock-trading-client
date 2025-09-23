#ifndef _EXPORTINDEXCALC_H_
#define _EXPORTINDEXCALC_H_

#include <vector>
using std::vector;
#include <map>
using std::map;

#include "iExportIndexCalc.h"

#include "ExportMainThreadWnd.h"
#include "ExportMT2DispatchThread.h"

#include "synch.h"
#include "AsynNotifyKeeper.h"

#include "CFormularContent.h"

using namespace cfjdExport;

class CFormularContent;

class CExportIndexCalc : public iExportIndexCalc, public iExportMT2DispatchThreadProcessor
{
public:
	enum E_FormulaChangedType
	{
		EFCTAdd = 0,		// 新增
		EFCTModify,
		EFCTDel,
	};

	class CFormularContentAdp : public CFormularContent
	{
	public:
		
		CFormularContentAdp(){}
		~CFormularContentAdp(){}

		CFormularContentAdp(const CFormularContent &stSrc)
		{
			this->Assign(stSrc);
		}
		CFormularContentAdp(const CFormularContentAdp &stSrc)
		{
			this->Assign(stSrc);
		}
		CFormularContentAdp &operator=(const CFormularContent &stSrc)
		{
			if ( this!=&stSrc )
			{
				this->Assign(stSrc);
			}
			return *this;
		}
		CFormularContentAdp &operator=(const CFormularContentAdp &stSrc)
		{
			if ( this!=&stSrc )
			{
				this->Assign(stSrc);
			}
			return *this;
		}
	};

public:
	~CExportIndexCalc();

	static CExportIndexCalc &Instance();

public:
	// 初始化 - 主线程调用
	bool	InitInstance();
	void	UnInit();

	// 公式指标变更 - ViewData调用，不独立抽出接口
	void	OnFormulaChanged(int eFormulaChangeType, LPCTSTR ptszName);

	// iExportIndexCalc
public:
	// 新增、删除事件关注
	virtual void	AddIndexNotify(iExportIndexCalcNotify *pNotify);
	virtual void	RemoveIndexNotify(iExportIndexCalcNotify *pNotify);
	// 显示指标管理对话框
	virtual void	ShowIndexManagerDlg();
	// 检查指标是否存在
	virtual	bool	IsIndexExist(const wchar_t *pwszIndexName);
	virtual bool	CalcIndex(const wchar_t *pwszIndexName, const T_ExportKLineUnit *pKLine, int iKLineUnitCount, OUT T_ExportIndexCalcResult **ppResult, OUT int &iResultDataCount);
	virtual	void	FreeCalcResultData(T_ExportIndexCalcResult *pResult, int iResultDataCount);

	// iExportMT2DispatchThreadProcessor
public:
	virtual void	DispatchThreadProcess(bool bSucDisp, LPARAM l);

private:
	CExportIndexCalc();

	bool	GetFormulaContent(const wchar_t *pwszIndexName, CFormularContent &formula);

	// DTP = DispatchThreadProcess
	void	DTPNotifyFormulaChanged(T_ExportMT2DispatchThreadProcessorLParam *pParam);

private:
	typedef AsynNotifyKeeper<iExportIndexCalcNotify>	NotifyKeeper;
	LockSingle		m_LockNotify;
	typedef vector<NotifyKeeper *> NotifyVector;
	NotifyVector	m_aNotify;

	struct T_ExportIndexNameCmpFunc 
	{
		bool operator()(LPCTSTR ptsz1, LPCTSTR ptsz2) const
		{
			if ( NULL==ptsz1 )
			{
				return NULL!=ptsz2;
			}
			else if ( NULL==ptsz2 )
			{
				return false;
			}

			return _tcsicmp(ptsz1, ptsz2) < 0;
		}
	};

	LockSingle		m_LockFormula;
	typedef map<CString, CFormularContentAdp, T_ExportIndexNameCmpFunc>	String2FormulaMap;
	String2FormulaMap	m_mapFormula;
};

#endif //!_EXPORTINDEXCALC_H_