#ifndef TIME_INTERVAL_
#define TIME_INTERVAL_

enum E_NodeTimeInterval
{
	ENTIMinute = 0,			// 分钟线	
	ENTIDay,				// 日线
	ENTIMinute5,
	ENTIMinute15,
	ENTIMinute30,
	ENTIMinute60,
	ENTIMinuteUser,			// 用户自定义数目分钟线	
	ENTIDayUser,			// 用户自定义数目日线
	ENTIWeek,				// 周线
	ENTIMonth,				// 月线
	ENTIQuarter,			// 季度
	ENTIYear,				// 一年
    ENTIMinute180,
    ENTIMinute240,
	ENTICount
};

#endif