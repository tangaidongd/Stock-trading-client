#ifndef _DATA_STREAM_H_
#define _DATA_STREAM_H_

#include "typedef.h"

typedef enum E_DataStreamState
{
	EDSSOk = 0,			// ����
 	EDSSEof			// ���������ݵ�������ĩβ
}E_DataStreamState;
 

class AFX_EXT_CLASS CDataStream  
{
public:
	CDataStream();
	virtual ~CDataStream();

public:
	bool32			Construct();
	bool32			Construct( int32 iSize );

public:
	void			Clear();

	int32			Read(int32 iNeedReadLen, OUT int8 *pcBuf, bool32 bOneOrMore = true);
	int32			Write(IN const int8 *pcBuf, int32 iNeedWriteSize, bool32 bOneOrMore = true);
	bool32          Skip(int32 iSkipLen );				// ɾ��ָ�������ȵ�����
	bool32          Copy(CDataStream &DataStreamAppend);	// ���Ŀ�����׷�ӣ�

	bool32			WriteInt(uint32 uiData);           
	bool32			WriteShort(uint16 usData);
	bool32			WriteByte(uint8 ucData);

	bool32			ReadInt(uint32 &uiData);
	bool32			ReadShort(uint16 &usData);
	bool32			ReadByte(uint8 &ucData);
	
	// ��
	bool32			Lock();
	void			UnLock();
	
	//
	int32			GetWritableDataLength();
	int32	        GetValidDataLength() const { return m_iValidDataLen; }           // ��ǰ�����������ݳ���
	int32	        GetStreamBufSize() const { return m_iStreamBufSize; }           // ��ǰ������ĳ���
	const int8*		Ptr() const { return m_pcStreamBuf; }

	void			SetStreamSize(int32 iStreamSize);
	int32			GetStreamSize() { return m_iStreamSize; }

	int				GetStreamWriteLength() { return m_iStreamWriteLength; }			// ������������

	//
	int32		    GetFillPos(void);
	E_DataStreamState   GetStreamState(void) { return m_eStreamState; }

	bool32			IsReadAllData();
	
	
private:
	// ������������С,���·�����д��,���ܻ��岻�ܴ���DATA_STREAM_BUFFER_SIZE
	int32			WriteOver(const int8 *pcBuf, int32 iWriteSize);

	// ��ʼ������
	bool32			InitData();


private:
	int8			*m_pcStreamBuf;
	int32			m_iStreamBufSize;
	int32			m_iValidDataLen;
	
	int32			m_iStreamWriteLength;	// ��ǰ������
	int32			m_iStreamSize;			// < 0��ʾ����δ֪�� ����������,����ǰ������������ʱ.Ϊ����! �������������Ϊδ֪ʱ
											// �Ƚ�ÿһ���ַ�,ֱ��Ϊ-1ʱ,��Ϊ����!�����س���
		
	E_DataStreamState	m_eStreamState;		// ����״̬
	
	HANDLE			m_hMutex;			// ͬ������
};

#endif 
