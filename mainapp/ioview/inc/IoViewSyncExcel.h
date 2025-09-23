#ifndef _IO_VIEW_SYNCEXCEL_H_
#define _IO_VIEW_SYNCEXCEL_H_

#include <map>
using	std::map;

#include "IoViewBase.h"
#include "MerchManager.h"
#include "BlockManager.h"
#include "excel.h"
using namespace excel;
#include "ReportScheme.h"
#include "PlugInStruct.h"

// ����ֻ�Ǵ���IoView���ݣ�����ʾ
class CIoViewSyncExcel : public CIoViewBase
{
// Construction 
public:
	
	virtual ~CIoViewSyncExcel();

	DECLARE_DYNCREATE(CIoViewSyncExcel)

public:
	typedef	CArray<T_Block, const T_Block &>	BlockArray;		// ����ֻ��ʵ�ʰ��Ĳ�����Ʒ
	typedef CArray<CMerch *, CMerch *>	MerchArray;
public:
	// from CIoViewBase
	virtual void	SetChildFrameTitle(){};
	virtual bool32	FromXml(TiXmlElement *pElement){return true;};		// ��ʵ��Xml�洢
	virtual CString	ToXml(){return CString();};
	virtual	CString GetDefaultXML(){return CString();};
	
	virtual void	OnIoViewActive(){};						// ��Ȼ����ʾ��������Ҳû��
	virtual void	OnIoViewDeactive(){};
	virtual bool32	GetStdMenuEnable(MSG* pMsg){return false;};

	virtual void	LockRedraw(){};
	virtual void	UnLockRedraw(){};

	virtual E_IoViewType GetIoViewType() { return EIVT_MultiMerch; }

	// from CIoViewBase			// �󲿷ֽӿڶ�û��
public:
	virtual void	OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch);
	virtual void	OnVDataRealtimePriceUpdate(IN CMerch *pMerch);
	virtual void	OnVDataForceUpdate();
	virtual void	OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs);
	virtual void	OnVDataGridHeaderChanged(E_ReportType eReportType);

	virtual void	OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType);
	virtual	bool32	IsPluginDataView() { return true; }
	virtual	void	OnVDataPluginResp(const CMmiCommBase *pResp);

	virtual void	OnIoViewColorChanged();			
	virtual void	OnIoViewFontChanged();
public:
	void	AddSyncBlock(const T_Block &block);
	void	RemoveSyncBlock(const CString &StrBlockName, bool32 bRemoveExcelSheet = false);		// ��blockname��ɾ���Ƿ���ʣ���
	bool32	ResetSyncBlockArray(const BlockArray &aBlocks);		// ���ù�ע��block�Լ�block��Ʒ��

	void	RemoveAllSyncBlock();			// ȡ�����е�block��ע
	//void	InitialBlockHeader();				// ��ʼ����block�ı�ͷ��ʾ - ÿ�����ݸ���ʱ�������±�ͷ
	bool32  InitialBlockHeader(const T_Block &block);		// ��ʼ���ض�block�ı�ͷ��Ϣ - ÿ�����ݸ���ʱ�������±�ͷ
	int32	GetBlockHeaderList(CStringArray *pStrCols, E_ReportType eHeadType);
	int32   GetBlockHeaderList(CArray<T_HeadInfo,T_HeadInfo> &aHeaderInfos, E_ReportType eHeadType);
	
	// ����û��sink�����ݸ���Ҳ�ǵ��������������������������ڻ�ȡ��sheetʧ��ʱ��ȡ��ʧ��block�Ĺ�ע - ��Ϊ��excel�߳�֪ͨ
	bool32	UpdateMerchData(CMerch *pMerch, bool32 bForce = false);		// ���¸���Ʒ�����ݣ������ޱ仯����ܲ����� - �п��ܴ��ڶ��block��
	bool32	UpdateBlockMerchData(const T_Block &block, CMerch *pMerch, bool32 bForce = false, int32 index=-1);

	bool32	AddBlockShowMerch(const T_Block &block, int32 iPos, const MerchArray &aNewMerchs); // �ڸ�ָ��pos������µ�merch��ʾ - excel��1��ʼ
	bool32	ClearBlockShowData(const T_Block &block); // ����block����Ʒ�Ĵ�С�ͱ�ͷ�У����excel����
	bool32	UpdateBlockData(const T_Block &block);	// ���¸�block��������Ʒ������ - ǿ��
	bool32	UpdateData();		// �������е����� - ǿ��

	void	AddAttendMerchs(const CArray<CMerch *, CMerch *> &aMerchs, bool bRequestViewData = true); // �������Ȥ����Ʒ, ���ܻᷢ������

	void	RequestViewData();
	void	RequestViewData(const CArray<CSmartAttendMerch, CSmartAttendMerch&> &aRequestSmarts);
	
	// �û��ֶ��ر�sheet��book����Ҫ���� - ����û��ʵ��sink����ʱ��������
	//excel
	//BOOL    OnBookBeforeClose(BOOL *pCancel);

	static	CIoViewSyncExcel &Instance(); // ����ֻ��һ��ʵ����������
	static void DelInstance();
	// Report����ص�

	// ������
	void	UserManageSyncBlock();		// �û�����ͬ����� - ɾ�����/��Ʒ(Ŀǰֻ֧��ɾ��)

	// exel��Դ
	static  void		InitialExcelResource();			    // ��ʼ��excel������Դ - ��Ҫ�ȵ�ExcelAppInit��Ϣ����֪����û�гɹ�
	static	void		UnInitialExcelResource();			// �ͷ����е�Excel������Դ
	
	void	SetExcelCellValue(const CString &StrSheet, USHORT nRow, USHORT nCol, const CString &StrValue, bool32 bUseColor, COLORREF clr, bool32 bIgnoreCache);

private:
	// ���ۻ���
	bool32	IsPriceCached(const CString &StrBlock, CMerch *pMerch);
	void	CachePrice(const CString &StrBlock, CMerch *pMerch);
	void	RemoveCache(const CString &StrBlock);			// ɾ����黺��
	void	RemoveCache(const CString &StrBlock, CMerch *pMerch);	// ɾ����Ʒ����

	// ͬblock��Ʒ�ϲ� - newMerchs�н����ԭ����block��û�е���Ʒ
	int		FindExistBlockIndex(const BlockArray &aBlocks, const T_Block &newBlock, OUT MerchArray &aNewMerchs);

	//
	bool32  IsAttendFinanceData();
	bool32	IsAttendFinanceData(const T_Block &block);		// �Ƿ��ע��������
	bool32	IsAttendMerchIndexData();
	bool32	IsAttendMerchIndexData(const T_Block &block);	// �Ƿ��עѡ������

private:
	// ��ȡ����Ҫ��Excel��Դ - �Ƶ�����excel�߳�
	
private:
	CIoViewSyncExcel();
	CIoViewSyncExcel(const CIoViewSyncExcel&);
	CIoViewSyncExcel &operator=(const CIoViewSyncExcel &);

	BlockArray		m_aAttendBlocks;
	static CIoViewSyncExcel *m_sthis;
	BlockArray		m_aWaitSyncBlocks;
	
	typedef	map<CMerch *, CRealtimePrice>	MerchRealtimePriceMap;
	typedef map<CString, MerchRealtimePriceMap > BlockRealTimePriceMap;
	//MerchRealtimePriceMap	m_mapRealtimePrices;		//	ʵʱ���ۻ��� - ��������û�仯ʱ�Ĳ���Ҫˢ��(����û��ֶ�ɾ����ʾ���� - ������)
	BlockRealTimePriceMap	m_mapRealtimePrices;		//  �����¹��ı��ۻ���

	typedef	 map<CMerch *, T_RespMerchIndex>	MerchIndexMap;	
	MerchIndexMap					m_mapMerchIndex;			// �յ���ѡ����������
	

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIoViewSyncExcel)
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIoViewSyncExcel)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnSheetError(WPARAM w, LPARAM l);
	afx_msg LRESULT OnAllSheetError(WPARAM w, LPARAM l);
	afx_msg LRESULT OnExcelAppInit(WPARAM w, LPARAM l);
	//}}AFX_MSG

	
 	DECLARE_MESSAGE_MAP()

	// ���ӵ�sink���Ժ�ʵ��
// 
// 	DECLARE_INTERFACE_MAP()
// 	BEGIN_INTERFACE_PART(MyWorkBookEventsSink, WorkbookEvents)
// 		STDMETHOD_(void, OnBeforeClose)(BOOL *pCancel);
// 	END_INTERFACE_PART(MyWorkBookEventsSink)

	//DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// �����̴߳���excel���ݲ��� CSyncExcelThread - �߳���Ϣ�����ܼ򵥣��Ƿ���Կ���ʹ�ø��̴߳���һ�����ڣ�Ȼ�󴰿ڴ�����Ϣ:)
class CSyncExcelThread : public CWinThread
{
	DECLARE_DYNCREATE(CSyncExcelThread)

public:
	enum{
		EUMT_AddSheet = WM_USER + 0x123,		// ֪ͨ�߳����sheet wparam(LPCTSTR)�����û�г�ʼ��excel����ʼ��excel,
		EUMT_CheckSheetExist,					// ���sheet�Ƿ��Ѿ��رջ���δ���� wparam(LPCTSTR)
		EUMT_CheckExcelStatus,					// ֪ͨ�̼߳��excel״̬
		EUMT_InitializeExcel,					// ֪ͨ�̴߳���excel��ʼ��
		EUMT_SetCellValue,						// ֪ͨ�߳����ñ��
		EUMT_ClearCellRange,					// ֪ͨ�߳������1��1Ϊ���ָ����Χ�ڵ�cell, wparam(LPCTSTR - sheet name) lparam(LO:X, HI:Y = LO:������, HI:������)
		EUMT_ShowExcelApp,						// ֪ͨ�߳���ʾexcel
		
		EUMT_ExcelSheetError,							// ��sheet�����������ܱ��û��رջ��������쳣��֪ͨ��msgOwner, WPARAM(LPCTSTR) - ����Ϣsend����
		EUMT_ExcelAllSheetError,						// ��Ӧ���е�sheet�޷��������� Send����
		EUMT_ExcelAppInit,								// ������msgOwner ��ʼ���Ƿ�ɹ� WPARAM(TRUE/false)
	};

	struct T_CellValue{
		CString		m_StrSheet;		// ����sheet
		USHORT		m_nRow;			// ������ - 1��ʼ
		USHORT		m_nCol;			// ������ - 1��ʼ
		CString		m_StrValue;		// ֵ
		bool32		m_bUseColor;	// �Ƿ�������ɫ
		COLORREF	m_clr;		// ��ɫ

		bool32		m_bIgnoreCache;	// �Ƿ������cacheֵ��ǿ������ - �ڲ�ʹ��:)���ⲿ��SetCellValueʱ�ڶ�������

		T_CellValue();
		bool32	operator==(const T_CellValue &cell) const;
		bool32  operator!=(const T_CellValue &cell) const {return !(*this == cell);}
		bool32	IsValid() const;
		CString GetDebugString();
	};
		
protected:
	CSyncExcelThread();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	CSyncExcelThread(const CSyncExcelThread &);
	CSyncExcelThread &operator=(const CSyncExcelThread &);
	virtual ~CSyncExcelThread();
	
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// �ⲿ����
	bool32	AddSheet(const CString &StrSheet);		// ���excel sheet��������ڸ����ֵ�sheet���򼤻��sheet - ͬ�����������ʧ��
	bool32	CheckSheetExist(const CString &StrSheet);
	void	SetCellValue(const T_CellValue &cell, bool32 bIgnoreCache = false); // ����ָ��cell��ֵ��bIgnoreCacheָ���Ƿ��ڻ�����ȵ������ǿ������
													// ���excel�и�cell�ǵ�һ����������clear��Ȼ��set - ��setΪ�첽����
	void	SetCellValue(const CString &StrSheet, USHORT nRow, USHORT nCol, const CString &StrValue, bool32 bUseColor, COLORREF clr, bool32 bIgnoreCache = false);
	void	ClearCellRange(const CString &StrShhet, USHORT nRowCount, USHORT nColCount);
	void	ShowExcelApp();		// ��ʾexcel

	void	CheckExcelStatus();		// ��鵱ǰexcel״̬������û��ʵ��sink��������Ҫÿ��һ��ʱ����excel״̬�������û��ر�excel״̬�¼�ʱ�ͷ���Դ

	static	CSyncExcelThread &Instance();	// ��ȡSynExcel����(�ú�����ͬ��) - ȷ��ֻ��һ���߳�ʵ�� һ��excelappʵ��

	// ָ������excel�쳣��Ϣ���ں�֪ͨmsg
	void	InitializeExcelResource(HWND hwndMsgOwner, DWORD dwMsgSheetError = EUMT_ExcelSheetError, DWORD dwMsgAllSheetError = EUMT_ExcelAllSheetError);		// ��ʼ��excel��Դ - �ⲿ���� - ��ʼ����Դ��ɺ����ݽ������msgOwner AppInit���
	void	UninitializeExcelResource();	// �ͷ�excel��Դ�������߳����� - �ⲿ����

	virtual void Delete();
	virtual BOOL PumpMessage();

private:
	typedef	map<DWORD, T_CellValue>		CellMap;	// HI:Y + LO:X ���DWORD������cell���� (�����飿)
	typedef map<CString, CellMap >		SheetMap;	// ���е�sheet��������

	DWORD	MakeCellIndex(USHORT nRow, USHORT nCol);
	bool32	IsCellNeedUpdate(const T_CellValue &cell, OUT bool32 *pCellIsNew); // ��cell�Ƿ���Ҫ����, pCellIsNew��ΪNULL����ᰵʾ�Ƿ�Ϊ����ӵ�cell
	void	CacheCell(const T_CellValue &cell);
	void	RemoveCacheCell(const T_CellValue &cell);
	void	RemoveCacheSheet(const CString &StrSheet);

	bool32	DoCheckExcelStatus();		// ���excel״̬
	bool32	DoInitializeExcelResource();
	void	DoUninitializeExcel();

	bool32	DoAddSheet(LPCTSTR lpszSheet);
	bool32	DoCheckSheetExist(LPCTSTR lpszSheet);

	// w������, lΪ������ָ�� - �ú���ֻΪ��ʱ�������Ҳ����׳��쳣���������ͬ��
	void	SendSyncThreadMessage(DWORD dwMsg, WPARAM w, OUT LPARAM lpResult);		// ��������̣߳�������ʱ����������Ի���Ȼᵼ�³�������
	
	bool32	AppendCellValue(const T_CellValue &cell, bool32 bIgnoreCache); // ���Ҫ���µ�cellֵ���п��ܲ�������sheet���������ʧ��
	void	PeekNeedUpdateCellValue(OUT SheetMap &sheetData);			   // ��ȡ��Ҫ���µ�ֵ����ȡ���ֵ�Ƴ����¶���
	void	RestoreToUpdate(SheetMap::iterator itSheet);					// setcellvalue���ã�������������

	void	SaveAsynStringParam(DWORD dwKey, LPCTSTR lpszStr);
	bool32	ReadAsynStringParam(DWORD dwKey, CString &StrParam);

	void	DoSheetError(const CString &StrSheet);		// ĳsheet�쳣����
	void	DoAllSheetError();							// ����sheet�쳣����
private:
	static	CSyncExcelThread *m_spExcelThread;

	bool32			m_bExcelSuspended;
	DWORD			m_nExcelSuspendedWaitTime;		// excel���ܳ��ֲ��ܽ�����Ϣ�����

	SheetMap		m_mapSheetCache;
	CRITICAL_SECTION	m_csLockSheetCache;			// ͬ������

	SheetMap		m_mapCellsToSet;				// ��Ҫ���õĵ�Ԫ�񼯺�
	CRITICAL_SECTION	m_csLockCellsToSet;			// ͬ�����õ�Ԫ�񼯺�

	map<DWORD, CString>	m_mapAsyncStringParam;		// �첽���ͱ�����ִ�
	CRITICAL_SECTION	m_csLockAsynStringParam;	

	DWORD			m_nSheetErrorMsg;				//  ��Ӧ�쳣��Ϣ����
	DWORD			m_nAllSheetErrorMsg;
	HWND			m_hwndMsgOwner;			
	
	MSG				m_msgSync;						// ��Ҫͬ������Ϣ
	HANDLE			m_hEvtWaitMsgEnter;				// �ȴ�����ȴ���Ϣ������ - ������������msgSync - �ȴ�pump message���������Ϣ - �����������
	HANDLE			m_hEvtPumpMsgEnd;				// pump message��������Ϣ

private:
	// ��ȡ����Ҫ��Excel��Դ
	_Application *GetExcelApplication();		// �п���ΪNULL - ���� - �����ڽ��ᴴ��
	Workbooks	 *GetExcelWorkBooks();			// ���� - �����ڽ��ᴴ��
	_Workbook	 *GetExcelWorkBook();			// ��ȡworkBook���п������½��� - ���� - �����ڽ��ᴴ��
	Worksheets   *GetExcelWorkSheets();			// ����block����һ��Sheets���� - �����ڽ��ᴴ��
	Worksheets   *GetExcelExistWorkSheets();
	bool		GetExcelWorkSheet(const CString &StrBlock, OUT _Worksheet &sheet, bool bCanCreate = false, bool32 *pbIsCreate = NULL, SCODE *pResult = NULL); // bCreate - �Ƿ�������sheet���������sheet�����ڵ�����»ᴴ��
	
	static	bool32	GetExcelCellPosString(UINT iRow, UINT iCol, CString &StrOut); // row/col��1Ϊ��ʼ
	
	// ��Щ��Ա��Ӧ��ֱ��ʹ�ã�ʹ������Ļ�ȡ��������ȡ��ʼ���˵���Դ
	_Application	m_sExcelApp;
	Workbooks	m_sExcelWorkBooks;
	_Workbook	m_sExcelWorkBook;
	Worksheets	m_sExcelWorkSheets;

	static	CString	m_sStrDlgFilePath;
	
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnIntializeExcelResource(WPARAM w, LPARAM l);
	afx_msg void OnAddSheet(WPARAM w, LPARAM l);
	afx_msg void OnSetCellValue(WPARAM w, LPARAM l);
	afx_msg void OnCheckSheetExist(WPARAM w, LPARAM l);
	afx_msg void OnCheckExcelStatus(WPARAM w, LPARAM l);
	afx_msg void OnClearCellRange(WPARAM w, LPARAM l);
	afx_msg void OnShowExcelApp(WPARAM w, LPARAM l);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_IO_VIEW_SYNCEXCEL_H_
