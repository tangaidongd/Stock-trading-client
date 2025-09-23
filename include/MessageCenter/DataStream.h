#ifndef _DATA_STREAM_H_
#define _DATA_STREAM_H_

#include "typedef.h"

typedef enum E_DataStreamState
{
	EDSSOk = 0,			// 正常
 	EDSSEof			// 流缓冲数据到达数据末尾
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
	bool32          Skip(int32 iSkipLen );				// 删除指定流长度的数据
	bool32          Copy(CDataStream &DataStreamAppend);	// 流的拷贝（追加）

	bool32			WriteInt(uint32 uiData);           
	bool32			WriteShort(uint16 usData);
	bool32			WriteByte(uint8 ucData);

	bool32			ReadInt(uint32 &uiData);
	bool32			ReadShort(uint16 &usData);
	bool32			ReadByte(uint8 &ucData);
	
	// 锁
	bool32			Lock();
	void			UnLock();
	
	//
	int32			GetWritableDataLength();
	int32	        GetValidDataLength() const { return m_iValidDataLen; }           // 当前流缓冲中数据长度
	int32	        GetStreamBufSize() const { return m_iStreamBufSize; }           // 当前流缓冲的长度
	const int8*		Ptr() const { return m_pcStreamBuf; }

	void			SetStreamSize(int32 iStreamSize);
	int32			GetStreamSize() { return m_iStreamSize; }

	int				GetStreamWriteLength() { return m_iStreamWriteLength; }			// 流经的数据量

	//
	int32		    GetFillPos(void);
	E_DataStreamState   GetStreamState(void) { return m_eStreamState; }

	bool32			IsReadAllData();
	
	
private:
	// 超出缓冲区大小,重新分配再写入,但总缓冲不能大于DATA_STREAM_BUFFER_SIZE
	int32			WriteOver(const int8 *pcBuf, int32 iWriteSize);

	// 初始化数据
	bool32			InitData();


private:
	int8			*m_pcStreamBuf;
	int32			m_iStreamBufSize;
	int32			m_iValidDataLen;
	
	int32			m_iStreamWriteLength;	// 当前流长度
	int32			m_iStreamSize;			// < 0表示长度未知， 整个流长度,当当前流等于整个流时.为结束! 如果整个流长度为未知时
											// 比较每一个字符,直到为-1时,则为结束!并返回长度
		
	E_DataStreamState	m_eStreamState;		// 缓冲状态
	
	HANDLE			m_hMutex;			// 同步互斥
};

#endif 
