#ifndef _DATA_ENGINE_MANAGER_H_
#define _DATA_ENGINE_MANAGER_H_
#include "DataEngineStruct.h"


////////////////////////////////////////////////////////////////////////////////////
// ���е�����֪ͨ
class  CDataManagerNotify
{
public:
	/**********************************************************************
	* ��������:	OnDataServiceConnectStatus
	* ��������:	��������״̬֪ͨ
	* �������:	iServiceId			����ID
	* �������:	eConnectStatus		����״̬EG_CNT_STATUS_CONNECT�����ӣ���EG_CNT_STATUS_DIS_CONNECT���Ͽ���
	***********************************************************************/
	virtual void	OnDataServiceConnectStatus(int32 iServiceId, E_Connect_Status eConnectStatus) = 0;

	/**********************************************************************
	* ��������:	OnDataRequestTimeOut
	* ��������:	��������ʱ֪ͨ
	* �������:	iMmiReqId		��������ID
	***********************************************************************/
	virtual void	OnDataRequestTimeOut(int32 iMmiReqId) = 0;			//������û����

	/**********************************************************************
	* ��������:	OnDataCommResponse
	* ��������:	��������ظ�֪ͨ
	***********************************************************************/
	virtual void	OnDataCommResponse() = 0;

	/**********************************************************************
	* ��������:	OnDataServerConnectStatus
	* ��������:	����������״̬֪ͨ
	* �������:	iCommunicationId	���ӷ�����ͨѶID
	* �������:	eConnectStatus		����״̬EG_CNT_STATUS_CONNECT�����ӣ���EG_CNT_STATUS_DIS_CONNECT���Ͽ���
	***********************************************************************/
	virtual void	OnDataServerConnectStatus(int32 iCommunicationId, E_Connect_Status eConnectStatus) = 0;

	/**********************************************************************
	* ��������:	OnDataSyncServerTime
	* ��������:	ͬ��������ʱ�䵽����
	* �������:	TimeServerNow		������ʱ��
	***********************************************************************/
	virtual void	OnDataSyncServerTime(CGmtTime &TimeServerNow) = 0;

	/**********************************************************************
	* ��������:	OnDataServerLongTimeNoRecvData
	* ��������:	��������ʱ��û�����ݻظ�
	* �������:	iCommunicationId		������ͨѶID
	* �������:	eType					�쳣��������
	***********************************************************************/
	virtual void	OnDataServerLongTimeNoRecvData(int32 iCommunicationId, E_ReConnectType eType) = 0;
};


// �������ݹ�����࣬ �����˶����ṩ�Ĺ��ܽӿ�
class  CAbsDataManager
{

public:

	/**********************************************************************
	* ��������:	Release
	* ��������:	�ͷ��ڴ�ռ�
	* �������:	
	***********************************************************************/
	virtual void Release() = 0;
	
	/**********************************************************************
	* ��������:	RequestData
	* ��������:	�������������
	* �������:	pMmiCommReq			����������
	* �������	aMmiReqNodes		����ڵ����	
	* �������	eDataSource			��������Դ����
	* �������	iCommunicationId	ͨѶID
	***********************************************************************/
	virtual int32	RequestData(CMmiCommBase *pMmiCommReq, OUT CArray<CMmiReqNode*, CMmiReqNode*> &aMmiReqNodes, 
									E_DataSource eDataSource = EDSAuto, int32 iCommunicationId = -1) = 0;

	/**********************************************************************
	* ��������:	GetRespCount
	* ��������:	������Ӧ�������Ŀ
	* �������:
	***********************************************************************/
	virtual int32	GetRespCount() = 0;

	/**********************************************************************
	* ��������:	PeekAndRemoveFirstResp
	* ��������:	ȡӦ���������
	* �������:	
	***********************************************************************/
	virtual CMmiRespNode*	PeekAndRemoveFirstResp() = 0;

	/**********************************************************************
	* ��������:	AddDataManagerNotify
	* ��������:	ע�����֪ͨ��
	* �������:	pDataManagerNotify		֪ͨ�ص���ָ��
	***********************************************************************/
	virtual void	AddDataManagerNotify(CDataManagerNotify *pDataManagerNotify) = 0;

	/**********************************************************************
	* ��������:	RemoveDataManagerNotify
	* ��������:	�Ƴ�ע�����֪ͨ��
	* �������:	pDataManagerNotify		�Ƴ�֪ͨ�ص���ָ��
	***********************************************************************/
	virtual void	RemoveDataManagerNotify(CDataManagerNotify *pDataManagerNotify) = 0;

	/**********************************************************************
	* ��������:	RemoveDataManagerNotify
	* ��������:	���������ʷ�����¼
	* �������:	iCommunicationId	������ͨѶ��
	***********************************************************************/
	virtual void	RemoveHistoryReqOfSpecifyServer(int32 iCommunicationId) = 0;

	/**********************************************************************
	* ��������:	ForceRequestType
	* ��������:	ǿ�Ƹı�����ʽ
	* �������:	eDataSource	������������
	***********************************************************************/
	virtual void	ForceRequestType(E_DataSource eDataSource) = 0;

	/**********************************************************************
	* ��������:	GetOfflineKLinesSnapshot
	* ��������:	��ȡ����K�߿���
	* �������:	iMarketId		�г�ID
	* �������	StrMerchCode	��Ʒ����
	* �������	eKLineTypeBase	K�߻�������
	* �������	eOffKLineType	������������
	* �������	TimeStart		��ʼʱ��
	* �������	TimeEnd			����ʱ��
	* �������	iKLineCount		��ȡ��K������
	***********************************************************************/
	virtual bool32	GetOfflineKLinesSnapshot(int32 iMarketId, const CString &StrMerchCode, E_KLineTypeBase eKLineTypeBase, 
		E_OffKLineType eOffKLineType, OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd, OUT int32 &iKLineCount) = 0;
	
	/**********************************************************************
	* ��������:	GetOfflineKLinesSnapshot
	* ��������:	д������K������
	* �������:	iMarketId		�г�ID
	* �������	StrMerchCode	��Ʒ����
	* �������	eKLineTypeBase	K�߻�������
	* �������	eOffKLineType	����K����������
	* �������	aKLines			K�����ݻ���
	***********************************************************************/
	virtual void	WriteOfflineKLines(int32 iMarketId, const CString &StrMerchCode, E_KLineTypeBase eKLineTypeBase, 
		E_OffKLineType eOffKLineType, IN const CArray<CKLine, CKLine> &aKLines) = 0;
	
	/**********************************************************************
	* ��������:	GetOfflineKLinesSnapshot
	* ��������:	��ȡ����K������
	* �������:	iMarketId		�г�ID
	* �������	StrMerchCode	��Ʒ����
	* �������	eKLineTypeBase	K�߻�������
	* �������	eOffKLineType	����K����������
	* �������	aKLines			K�����ݻ�����
	***********************************************************************/
	virtual void	ReadOfflineKLines(int32 iMarketId, const CString &StrMerchCode, E_KLineTypeBase eKLineTypeBase, 
		E_OffKLineType eOffKLineType, OUT CArray<CKLine, CKLine> &aKLines) = 0;

	/**********************************************************************
	* ��������:	GetOfflineKLinesSnapshot
	* ��������:	��ȡ����K������
	* �������:	iMarketId		�г�ID
	* �������	StrMerchCode	��Ʒ����
	* �������	eKLineTypeBase	K�߻�������
	* �������	eOffKLineType	����K����������
	* �������	TimeStart		��ʼʱ��
	* �������	TimeEnd			����ʱ��
	* �������	aKLines			K�����ݻ�����
	***********************************************************************/
	virtual void	ReadOfflineKLines(int32 iMarketId, const CString &StrMerchCode, E_KLineTypeBase eKLineTypeBase, 
		E_OffKLineType eOffKLineType, const CGmtTime &TimeStart, const CGmtTime &TimeEnd, OUT CArray<CKLine, CKLine> &aKLines) = 0;

	/**********************************************************************
	* ��������:	GetOfflineKLinesSnapshot
	* ��������:	��ȡ����K������
	* �������:	iMarketId		�г�ID
	* �������	StrMerchCode	��Ʒ����
	* �������	eKLineTypeBase	K�߻�������
	* �������	eOffKLineType	����K����������
	* �������	TimeSpecify		����ʱ���
	* �������	iCount			K����Ŀ
	* �������	aKLines			K�����ݻ�����
	***********************************************************************/
	virtual void	ReadOfflineKLines(int32 iMarketId, const CString &StrMerchCode, E_KLineTypeBase eKLineTypeBase,
		E_OffKLineType eOffKLineType, const CGmtTime &TimeSpecify, int32 iCount, OUT CArray<CKLine, CKLine> &aKLines) = 0;
	
	/**********************************************************************
	* ��������:	RemoveOfflineKLines
	* ��������:	�Ƴ��������K������
	* �������:	iMarketId		�г�ID
	* �������	StrMerchCode	��Ʒ����
	* �������	eKLineTypeBase	K�߻�������
	* �������	eOffKLineType	����K����������
	***********************************************************************/
	virtual void	RemoveOfflineKLines(int32 iMarketId, const CString &StrMerchCode, E_KLineTypeBase eKLineTypeBase, 
		E_OffKLineType eOffKLineType) = 0;

	/**********************************************************************
	* ��������:	WriteOfflineRealtimePrice
	* ��������:	д���������齻������
	* �������:	stRealtimePrice		���齻������
	***********************************************************************/
	virtual	void	WriteOfflineRealtimePrice(const CRealtimePrice& stRealtimePrice) = 0;

	/**********************************************************************
	* ��������:	ReadOfflineRealtimePrice
	* ��������:	��ȡ������ʵ�۸�
	* �������:	iMarketId		�г�ID
	* �������	StrMerchCode	��Ʒ����
	* �������	RealtimePrice	���齻������
	***********************************************************************/
	virtual void	ReadOfflineRealtimePrice(int32 iMarkerId, const CString& StrMerchCode, OUT CRealtimePrice& RealtimePrice) = 0;

	/**********************************************************************
	* ��������:	GetPublishFilePath
	* ��������:	��ȡ�����ļ�·��
	* �������:	ePublicFileType		�ļ�����
	* �������	iMarketId			��Ʒ����
	* �������	StrMerchCode		��Ʒ����
	* �������	StrFilePath			�ļ�·��
	***********************************************************************/
	virtual bool32	GetPublishFilePath(E_PublicFileType ePublicFileType, int32 iMarketId, const CString &StrMerchCode, 
		OUT CString &StrFilePath) = 0;	

	/**********************************************************************
	* ��������:	InitAllService
	* ��������:	��ʼ������
	* �������:	aServerInfo			�������б�
	* �������	ProxyInfo			�����������Ϣ
	* �������	StrUserName			�û�����
	* �������	StrPassword			�û�����
	***********************************************************************/
	virtual void	InitAllService( IN CArray<T_ServerInfo, T_ServerInfo&> &aServerInfo, IN const CProxyInfo &ProxyInfo, 
		IN const CString &StrUserName, IN const CString &StrPassword) = 0;

	/**********************************************************************
	* ��������:	InitAllService
	* ��������:	�������з���
	* �������:	bOnlyStartFirst		�Ƿ���������ĵ�һ̨��������Ĭ����������
	***********************************************************************/
	virtual void	StartAllService(bool32 bOnlyStartFirst = false) = 0;

	/**********************************************************************
	* ��������:	InitAllService
	* ��������:	ֹͣ���з���
	***********************************************************************/
	virtual void	StopAllService() = 0; 

	/**********************************************************************
	* ��������:	StartService
	* ��������:	����ָ��ҵ���ͨѶ������
	* �������:	iCommunication		ͨѶ������ID
	***********************************************************************/
	virtual	void	StartService(int32 iCommunication) = 0;

	/**********************************************************************
	* ��������:	StartService
	* ��������:	ָֹͣ��ҵ���ͨѶ������
	* �������:	iCommunication		ͨѶ������ID
	***********************************************************************/
	virtual void	StopService(int32 iCommunication) = 0;

	/**********************************************************************
	* ��������:	GetActiveCommunicationByService
	* ��������:	��ȡ��ǰ���ӵķ������б�
	* �������:	iServiceId				����ID
	* �������:	aCommunicationIds		������ͨѶID
	***********************************************************************/
	virtual void	GetActiveCommunicationByService(IN int32  iServiceId, OUT CArray<int32, int32> &aCommunicationIds) = 0;


	/**********************************************************************
	* ��������:	GetCommunicationID
	* ��������:	��ȡ��������ͨѶID
	* �������:	StrAddress	����ID
	* �������:	iPort		������ͨѶID
	* ����ֵ:				������ͨѶID
	***********************************************************************/
	virtual int32	GetCommunicationID(IN const CString& StrAddress, IN const uint32 iPort) = 0;

	// ������Ӻ�����֤�Ƿ���Ҫ
	///////////////////////////////////////////////////////////////
	/**********************************************************************
	* ��������:	ReSetCommunicationIndex
	* ��������:	ͨ������������ID��������
	* �������:	aServersAccording	��������Ϣ����
	***********************************************************************/
	virtual void	ReSetCommunicationIndex(IN const CArray<CServerState, CServerState&>& aServersAccording) = 0;

	/**********************************************************************
	* ��������:	BeFirstCommunication
	* ��������:	�Ƿ�Ϊ�׸�������
	* ����ֵ:	true���ǣ�false����
	***********************************************************************/
	virtual bool32	BeFirstCommunication(IN const CString& StrAddress, IN const int32 iPort) = 0;

	/**********************************************************************
	* ��������:	GetCommnunicaionList
	* ��������:	��ȡͨѶ��������Ϣ�б�
	* �������:	aCommunicationList	��������Ϣ�б���
	***********************************************************************/
	virtual void GetCommnunicaionList(OUT CArray<CAbsCommunicationShow*, CAbsCommunicationShow*> &aCommunicationList) = 0;

	/**********************************************************************
	* ��������:	GetServerServiceList
	* ��������:	��ȡָ���������ķ����б�
	* �������:	StrAddress	��������ַ
	* �������:	iPort		�������˿�
	* �������:	aServiceIDs	����������ID�б�
	***********************************************************************/
	virtual void GetServerServiceList(IN const CString& StrAddress, IN const uint32 iPort, OUT CArray<int32,int32>& aServiceIDs) = 0;

	/**********************************************************************
	* ��������:	GetStage
	* ��������:	��ȡ��ǰͨѶ������Ϣ
	* ����ֵ:	E_COMMSTAGE	0:���ڵ�¼��ʼ�������� 1:�����ṩ��
	***********************************************************************/
	virtual DWORD GetStage() = 0;		// 

	/**********************************************************************
	* ��������:	SetStage
	* ��������:	���õ�ǰͨѶ������Ϣ
	* �������:	eStage	ECSTAGE_LoginInit:���ڵ�¼��ʼ�������� ECSTAGE_QuoteSupplier:�����ṩ��
	***********************************************************************/
	virtual void  SetStage(E_COMMSTAGE eStage) = 0; // 

	/**********************************************************************
	* ��������:	IsServerConneted
	* ��������:	 ��ǰ�������Ƿ�������
	* �������:	true�������� false��δ����
	***********************************************************************/
	virtual bool32	IsServerConneted(IN const CString& StrAddress, IN const int32 iPort) = 0;

	/**********************************************************************
	* ��������:	StopServiceAfterSortServer
	* ��������:	ping ���Ժ�ֹͣ�����������ķ���
	***********************************************************************/
	virtual void StopServiceAfterSortServer() = 0;
};


AFX_API_IMPORT CAbsDataManager * GetDataEngineObj();


#endif