#ifndef _GMT_TIME_H_
#define _GMT_TIME_H_
#include "typedef.h"
class CGmtTime;

class AFX_EXT_CLASS CGmtTimeSpan
{
public:

// Constructors
	CGmtTimeSpan();
	CGmtTimeSpan(time_t time);
	CGmtTimeSpan(LONG lDays, int nHours, int nMins, int nSecs);
	CGmtTimeSpan(const CGmtTimeSpan& timeSpanSrc);
	const CGmtTimeSpan& operator=(const CGmtTimeSpan& timeSpanSrc);

// Attributes
	// extract parts
	LONG GetDays() const;
	LONG GetTotalHours() const;
	int GetHours() const;
	LONG GetTotalMinutes() const;
	int GetMinutes() const;
	LONG GetTotalSeconds() const;
	int GetSeconds() const;

// Operations
	// time math
	CGmtTimeSpan operator-(const CGmtTimeSpan &timeSpan) const;		
	CGmtTimeSpan operator+(const CGmtTimeSpan &timeSpan) const;		
	const CGmtTimeSpan& operator+=(const CGmtTimeSpan &timeSpan);	
	const CGmtTimeSpan& operator-=(const CGmtTimeSpan &timeSpan);	
	BOOL operator==(const CGmtTimeSpan &timeSpan) const;			
	BOOL operator!=(const CGmtTimeSpan &timeSpan) const;			
	BOOL operator<(const CGmtTimeSpan &timeSpan) const;				
	BOOL operator>(const CGmtTimeSpan &timeSpan) const;				
	BOOL operator<=(const CGmtTimeSpan &timeSpan) const;			
	BOOL operator>=(const CGmtTimeSpan &timeSpan) const	;			

private:
	time_t m_timeSpan;
	friend class CGmtTime;
};

class AFX_EXT_CLASS CGmtTime
{
public:
	static CGmtTime PASCAL GetCurrentTime();

	CGmtTime();
	CGmtTime(time_t time);
	CGmtTime(const CGmtTime& timeSrc);
	const CGmtTime& operator=(const CGmtTime& timeSrc);
	const CGmtTime& operator=(time_t t);
	CGmtTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nDST = -1);
	struct tm* GetGmtTm(struct tm* ptm = NULL) const;
	struct tm* GetLocalTm(struct tm* ptm = NULL) const;

public:
	time_t GetTime() const;		
	int GetYear() const;			
	int GetMonth() const;			
	int GetDay() const;			
	int GetHour() const;		
	int GetMinute() const;			
	int GetSecond() const;			
	int GetDayOfWeek() const;		

public:
	// time math
	CGmtTimeSpan operator-(const CGmtTime &time) const;			
	CGmtTime operator-(const CGmtTimeSpan &timeSpan) const;		
	CGmtTime operator+(const CGmtTimeSpan &timeSpan) const;		
	const CGmtTime& operator+=(const CGmtTimeSpan &timeSpan);	
	const CGmtTime& operator-=(const CGmtTimeSpan &timeSpan);	
	BOOL operator==(const CGmtTime &time) const;				
	BOOL operator!=(const CGmtTime &time) const;				
	BOOL operator<(const CGmtTime &time) const;					
	BOOL operator>(const CGmtTime &time) const;					
	BOOL operator<=(const CGmtTime &time) const;				
	BOOL operator>=(const CGmtTime &time) const;				
private:
	time_t m_time;
};

class AFX_EXT_CLASS CMsTime
{
public:
	CMsTime();
	CMsTime(const CGmtTime &Time);
	
public:
	bool			operator==(const CMsTime &MsTime) const;
	bool			operator!=(const CMsTime &MsTime) const;
	bool			operator<(const CMsTime &MsTime) const;
	bool			operator>(const CMsTime &MsTime) const;
	bool			operator<=(const CMsTime &MsTime) const;
	bool			operator>=(const CMsTime &MsTime) const;
	
public:
	CGmtTime		m_Time;			// 时间， 秒
	uint32			m_uiMs;			// 毫秒
	uint32			m_uiIndexMs;	// 同一毫秒可能有几笔数据, 这是同一毫秒内的索引值
};


#endif

