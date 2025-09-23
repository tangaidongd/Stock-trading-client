#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

// 常用类型
typedef double long				int64;		/* signed 64 bit value */
//typedef unsigned double long	uint64;		/* usigned 64 bit value */
typedef signed long				int32;		/* signed 32 bit value */
typedef unsigned long			uint32;		/* unsigned 32 bit value */
typedef signed short			int16;		/* signed 16 bit value */
typedef unsigned short			uint16;		/* unsigned 16 bit value */
typedef signed char				int8;		/* signed 8  bit value */
typedef unsigned char			uint8;		/* unsigned 8  bit value */
typedef unsigned short			word;		/* unsigned 16 bit value type. */
typedef unsigned long			dword;		/* unsigned 32 bit value type. */

// 常用缩写类型
//typedef uint64					u64;
typedef uint32					u32;
typedef uint16					u16;
typedef uint8					u8;

// 
#ifndef byte
typedef unsigned char			byte;		/* Unsigned 8  bit value type. */
#endif

#ifndef boolean
typedef	unsigned char			boolean;	/* Boolean value type. */
#endif

#ifndef bool32
typedef	unsigned long			bool32;		/* Boolean value type. */
#endif

// 兼容unicode和ansi字符编码
typedef TCHAR					tchar;


///////////////////////////////////////
// 参数修饰符
///////////////////////////////////////
#define IN
#define OUT
#define INOUT


#endif



