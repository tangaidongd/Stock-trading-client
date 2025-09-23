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

// 该类只是处理IoView数据，不显示
class CIoViewSyncExcel : public CIoViewBase
{
// Construction 
public:
	
	virtual ~CIoViewSyncExcel();

	DECLARE_DYNCREATE(CIoViewSyncExcel)

public:
	typedef	CArray<T_Block, const T_Block &>	BlockArray;		// 可能只有实际板块的部分商品
	typedef CArray<CMerch *, CMerch *>	MerchArray;
public:
	// from CIoViewBase
	virtual void	SetChildFrameTitle(){};
	virtual bool32	FromXml(TiXmlElement *pElement){return true;};		// 不实现Xml存储
	virtual CString	ToXml(){return CString();};
	virtual	CString GetDefaultXML(){return CString();};
	
	virtual void	OnIoViewActive(){};						// 既然不显示，这两个也没用
	virtual void	OnIoViewDeactive(){};
	virtual bool32	GetStdMenuEnable(MSG* pMsg){return false;};

	virtual void	LockRedraw(){};
	virtual void	UnLockRedraw(){};

	virtual E_IoViewType GetIoViewType() { return EIVT_MultiMerch; }

	// from CIoViewBase			// 大部分接口都没用
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
	void	RemoveSyncBlock(const CString &StrBlockName, bool32 bRemoveExcelSheet = false);		// 用blockname来删除是否合适？？
	bool32	ResetSyncBlockArray(const BlockArray &aBlocks);		// 重置关注的block以及block商品等

	void	RemoveAllSyncBlock();			// 取消所有的block关注
	//void	InitialBlockHeader();				// 初始化各block的表头显示 - 每次数据更新时，不更新表头
	bool32  InitialBlockHeader(const T_Block &block);		// 初始化特定block的表头信息 - 每次数据更新时，不更新表头
	int32	GetBlockHeaderList(CStringArray *pStrCols, E_ReportType eHeadType);
	int32   GetBlockHeaderList(CArray<T_HeadInfo,T_HeadInfo> &aHeaderInfos, E_ReportType eHeadType);
	
	// 由于没有sink，数据更新也是调用这个函数，所以这个函数会在获取的sheet失败时，取消失败block的关注 - 改为由excel线程通知
	bool32	UpdateMerchData(CMerch *pMerch, bool32 bForce = false);		// 更新该商品的数据，数据无变化则可能不更新 - 有可能存在多个block中
	bool32	UpdateBlockMerchData(const T_Block &block, CMerch *pMerch, bool32 bForce = false, int32 index=-1);

	bool32	AddBlockShowMerch(const T_Block &block, int32 iPos, const MerchArray &aNewMerchs); // 在该指定pos处添加新的merch显示 - excel以1开始
	bool32	ClearBlockShowData(const T_Block &block); // 根据block先商品的大小和表头列，清空excel数据
	bool32	UpdateBlockData(const T_Block &block);	// 更新该block下所有商品的数据 - 强制
	bool32	UpdateData();		// 更新所有的数据 - 强制

	void	AddAttendMerchs(const CArray<CMerch *, CMerch *> &aMerchs, bool bRequestViewData = true); // 加入感兴趣的商品, 可能会发出请求

	void	RequestViewData();
	void	RequestViewData(const CArray<CSmartAttendMerch, CSmartAttendMerch&> &aRequestSmarts);
	
	// 用户手动关闭sheet，book等需要处理 - 由于没有实现sink，暂时处理不理想
	//excel
	//BOOL    OnBookBeforeClose(BOOL *pCancel);

	static	CIoViewSyncExcel &Instance(); // 现在只有一个实例，返回它
	static void DelInstance();
	// Report中相关的

	// 板块管理
	void	UserManageSyncBlock();		// 用户管理同步板块 - 删除板块/商品(目前只支持删除)

	// exel资源
	static  void		InitialExcelResource();			    // 初始化excel共用资源 - 需要等到ExcelAppInit消息来才知道有没有成功
	static	void		UnInitialExcelResource();			// 释放所有的Excel共用资源
	
	void	SetExcelCellValue(const CString &StrSheet, USHORT nRow, USHORT nCol, const CString &StrValue, bool32 bUseColor, COLORREF clr, bool32 bIgnoreCache);

private:
	// 报价缓存
	bool32	IsPriceCached(const CString &StrBlock, CMerch *pMerch);
	void	CachePrice(const CString &StrBlock, CMerch *pMerch);
	void	RemoveCache(const CString &StrBlock);			// 删除板块缓存
	void	RemoveCache(const CString &StrBlock, CMerch *pMerch);	// 删除商品缓存

	// 同block商品合并 - newMerchs中将存放原来的block中没有的商品
	int		FindExistBlockIndex(const BlockArray &aBlocks, const T_Block &newBlock, OUT MerchArray &aNewMerchs);

	//
	bool32  IsAttendFinanceData();
	bool32	IsAttendFinanceData(const T_Block &block);		// 是否关注财务数据
	bool32	IsAttendMerchIndexData();
	bool32	IsAttendMerchIndexData(const T_Block &block);	// 是否关注选股数据

private:
	// 获取所需要的Excel资源 - 移到单独excel线程
	
private:
	CIoViewSyncExcel();
	CIoViewSyncExcel(const CIoViewSyncExcel&);
	CIoViewSyncExcel &operator=(const CIoViewSyncExcel &);

	BlockArray		m_aAttendBlocks;
	static CIoViewSyncExcel *m_sthis;
	BlockArray		m_aWaitSyncBlocks;
	
	typedef	map<CMerch *, CRealtimePrice>	MerchRealtimePriceMap;
	typedef map<CString, MerchRealtimePriceMap > BlockRealTimePriceMap;
	//MerchRealtimePriceMap	m_mapRealtimePrices;		//	实时报价缓存 - 避免数据没变化时的不必要刷新(如果用户手动删除显示数据 - 不处理)
	BlockRealTimePriceMap	m_mapRealtimePrices;		//  板块更新过的报价缓存

	typedef	 map<CMerch *, T_RespMerchIndex>	MerchIndexMap;	
	MerchIndexMap					m_mapMerchIndex;			// 收到的选股数据请求
	

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

	// 连接点sink等以后实现
// 
// 	DECLARE_INTERFACE_MAP()
// 	BEGIN_INTERFACE_PART(MyWorkBookEventsSink, WorkbookEvents)
// 		STDMETHOD_(void, OnBeforeClose)(BOOL *pCancel);
// 	END_INTERFACE_PART(MyWorkBookEventsSink)

	//DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// 单独线程处理excel数据插入 CSyncExcelThread - 线程消息处理功能简单，是否可以考虑使用该线程创建一个窗口，然后窗口处理消息:)
class CSyncExcelThread : public CWinThread
{
	DECLARE_DYNCREATE(CSyncExcelThread)

public:
	enum{
		EUMT_AddSheet = WM_USER + 0x123,		// 通知线程添加sheet wparam(LPCTSTR)，如果没有初始化excel，初始化excel,
		EUMT_CheckSheetExist,					// 检查sheet是否已经关闭或者未创建 wparam(LPCTSTR)
		EUMT_CheckExcelStatus,					// 通知线程检查excel状态
		EUMT_InitializeExcel,					// 通知线程处理excel初始化
		EUMT_SetCellValue,						// 通知线程设置表格
		EUMT_ClearCellRange,					// 通知线程清除以1，1为起点指定范围内的cell, wparam(LPCTSTR - sheet name) lparam(LO:X, HI:Y = LO:列数量, HI:行数量)
		EUMT_ShowExcelApp,						// 通知线程显示excel
		
		EUMT_ExcelSheetError,							// 该sheet操作出错，可能被用户关闭或者其它异常，通知其msgOwner, WPARAM(LPCTSTR) - 该消息send发出
		EUMT_ExcelAllSheetError,						// 对应所有的sheet无法正常操作 Send发出
		EUMT_ExcelAppInit,								// 回馈给msgOwner 初始化是否成功 WPARAM(TRUE/false)
	};

	struct T_CellValue{
		CString		m_StrSheet;		// 所属sheet
		USHORT		m_nRow;			// 所在行 - 1开始
		USHORT		m_nCol;			// 所在列 - 1开始
		CString		m_StrValue;		// 值
		bool32		m_bUseColor;	// 是否设置颜色
		COLORREF	m_clr;		// 颜色

		bool32		m_bIgnoreCache;	// 是否忽略其cache值而强制设置 - 内部使用:)，外部在SetCellValue时第二个参数

		T_CellValue();
		bool32	operator==(const T_CellValue &cell) const;
		bool32  operator!=(const T_CellValue &cell) const {return !(*this == cell);}
		bool32	IsValid() const;
		CString GetDebugString();
	};
		
protected:
	CSyncExcelThread();           // 动态创建所使用的受保护的构造函数
	CSyncExcelThread(const CSyncExcelThread &);
	CSyncExcelThread &operator=(const CSyncExcelThread &);
	virtual ~CSyncExcelThread();
	
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// 外部调用
	bool32	AddSheet(const CString &StrSheet);		// 添加excel sheet，如果存在该名字的sheet，则激活该sheet - 同步，可能添加失败
	bool32	CheckSheetExist(const CString &StrSheet);
	void	SetCellValue(const T_CellValue &cell, bool32 bIgnoreCache = false); // 设置指定cell的值，bIgnoreCache指定是否在缓存想等的情况下强行设置
													// 如果excel中该cell是第一次设置则先clear，然后set - 该set为异步设置
	void	SetCellValue(const CString &StrSheet, USHORT nRow, USHORT nCol, const CString &StrValue, bool32 bUseColor, COLORREF clr, bool32 bIgnoreCache = false);
	void	ClearCellRange(const CString &StrShhet, USHORT nRowCount, USHORT nColCount);
	void	ShowExcelApp();		// 显示excel

	void	CheckExcelStatus();		// 检查当前excel状态，由于没有实现sink，所有需要每隔一段时间检查excel状态，以在用户关闭excel状态下及时释放资源

	static	CSyncExcelThread &Instance();	// 获取SynExcel对象(该函数不同步) - 确保只有一个线程实例 一个excelapp实例

	// 指定接收excel异常消息窗口和通知msg
	void	InitializeExcelResource(HWND hwndMsgOwner, DWORD dwMsgSheetError = EUMT_ExcelSheetError, DWORD dwMsgAllSheetError = EUMT_ExcelAllSheetError);		// 初始化excel资源 - 外部调用 - 初始化资源完成后会根据结果发给msgOwner AppInit结果
	void	UninitializeExcelResource();	// 释放excel资源，结束线程生命 - 外部调用

	virtual void Delete();
	virtual BOOL PumpMessage();

private:
	typedef	map<DWORD, T_CellValue>		CellMap;	// HI:Y + LO:X 组成DWORD索引的cell缓存 (用数组？)
	typedef map<CString, CellMap >		SheetMap;	// 所有的sheet缓存数据

	DWORD	MakeCellIndex(USHORT nRow, USHORT nCol);
	bool32	IsCellNeedUpdate(const T_CellValue &cell, OUT bool32 *pCellIsNew); // 该cell是否需要更新, pCellIsNew不为NULL，则会暗示是否为新添加的cell
	void	CacheCell(const T_CellValue &cell);
	void	RemoveCacheCell(const T_CellValue &cell);
	void	RemoveCacheSheet(const CString &StrSheet);

	bool32	DoCheckExcelStatus();		// 检查excel状态
	bool32	DoInitializeExcelResource();
	void	DoUninitializeExcel();

	bool32	DoAddSheet(LPCTSTR lpszSheet);
	bool32	DoCheckSheetExist(LPCTSTR lpszSheet);

	// w传参数, l为输出结果指针 - 该函数只为短时间运行且不会抛出异常的情况做简单同步
	void	SendSyncThreadMessage(DWORD dwMsg, WPARAM w, OUT LPARAM lpResult);		// 如果是主线程，在锁定时，如果弹出对话框等会导致程序锁死
	
	bool32	AppendCellValue(const T_CellValue &cell, bool32 bIgnoreCache); // 添加要更新的cell值，有可能不存在在sheet而导致添加失败
	void	PeekNeedUpdateCellValue(OUT SheetMap &sheetData);			   // 获取需要更新的值，获取后该值移出更新队列
	void	RestoreToUpdate(SheetMap::iterator itSheet);					// setcellvalue调用，返还更新数据

	void	SaveAsynStringParam(DWORD dwKey, LPCTSTR lpszStr);
	bool32	ReadAsynStringParam(DWORD dwKey, CString &StrParam);

	void	DoSheetError(const CString &StrSheet);		// 某sheet异常处理
	void	DoAllSheetError();							// 所有sheet异常处理
private:
	static	CSyncExcelThread *m_spExcelThread;

	bool32			m_bExcelSuspended;
	DWORD			m_nExcelSuspendedWaitTime;		// excel可能出现不能接受消息的情况

	SheetMap		m_mapSheetCache;
	CRITICAL_SECTION	m_csLockSheetCache;			// 同步缓存

	SheetMap		m_mapCellsToSet;				// 需要设置的单元格集合
	CRITICAL_SECTION	m_csLockCellsToSet;			// 同步设置单元格集合

	map<DWORD, CString>	m_mapAsyncStringParam;		// 异步发送保存的字串
	CRITICAL_SECTION	m_csLockAsynStringParam;	

	DWORD			m_nSheetErrorMsg;				//  对应异常消息接收
	DWORD			m_nAllSheetErrorMsg;
	HWND			m_hwndMsgOwner;			
	
	MSG				m_msgSync;						// 需要同步的消息
	HANDLE			m_hEvtWaitMsgEnter;				// 等待进入等待消息队列锁 - 进入马上设置msgSync - 等待pump message处理完该消息 - 清除锁，返回
	HANDLE			m_hEvtPumpMsgEnd;				// pump message处理完消息

private:
	// 获取所需要的Excel资源
	_Application *GetExcelApplication();		// 有可能为NULL - 共用 - 不存在将会创建
	Workbooks	 *GetExcelWorkBooks();			// 共用 - 不存在将会创建
	_Workbook	 *GetExcelWorkBook();			// 获取workBook，有可能是新建的 - 共用 - 不存在将会创建
	Worksheets   *GetExcelWorkSheets();			// 所有block共用一个Sheets变量 - 不存在将会创建
	Worksheets   *GetExcelExistWorkSheets();
	bool		GetExcelWorkSheet(const CString &StrBlock, OUT _Worksheet &sheet, bool bCanCreate = false, bool32 *pbIsCreate = NULL, SCODE *pResult = NULL); // bCreate - 是否允许创建sheet，如果允许，sheet不存在的情况下会创建
	
	static	bool32	GetExcelCellPosString(UINT iRow, UINT iCol, CString &StrOut); // row/col以1为开始
	
	// 这些成员不应当直接使用，使用上面的获取函数来获取初始化了的资源
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
