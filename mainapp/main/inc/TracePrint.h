#ifndef TRACEPRINT_H_
#define TRACEPRINT_H_

#define  TRACE_ELAPSE_BEGIN() iTickBegin = GetTickCount();
#define  TRACE_ELAPSE_END(msg)\
{\
	int iTickEnd = GetTickCount();\
	float f = (iTickEnd - iTickBegin) / 1000.0f;\
	TRACE("%s:%f\n", msg, f);\
}
#define  TRACE_ELAPSE_END_TWO(iTickBegin, msg)\
{\
	int iTickEnd = GetTickCount();\
	float f = (iTickEnd - iTickBegin) / 1000.0f;\
	TRACE("%s:%f\n", msg, f);\
}

#define 