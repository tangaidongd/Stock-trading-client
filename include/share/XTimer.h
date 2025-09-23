#ifndef	_X_TIMER_H_
#define	_X_TIMER_H_

/********************************************************************
created:	2006/03/24
filename: 	CXTimer
author:		goman
purpose:	millisecond Timer.
the default Minimum timer resolution is 5.
*********************************************************************/
#include <mmsystem.h>
#include <afxtempl.h>

#include "typedef.h" 

class CXTimer;


#define  MAX_TIMER		50	//

////////////////////
class CXTimerListener
{
public:
	virtual void OnXTimer(int32 iTimerID) = 0;	
};


///////////////
struct T_XTimerRecorder 
{
	CXTimer			*pXTimer;
	int				nID;
	static int		nRes;
};

class AFX_EXT_CLASS CXTimer
{
public:
	CXTimer();
	virtual ~CXTimer();
	
	UINT			getTimerRes() { return timerRes; };
	
	bool			startTimer(UINT period,CXTimerListener *pListener = NULL,int32 iTimerID = 1,bool oneShot = FALSE);
	bool			stopTimer();
	bool			stopTimer(int32 iTimerID);
	void			timerProc();
	CXTimerListener *m_pListener;
	
protected:
	UINT			timerRes;
	UINT			timerId;
	UINT			timerId2;
	T_XTimerRecorder m_Recoder[MAX_TIMER] ;
		
	CMap<int,int,int,int> m_mapTimerID;
};


class AFX_EXT_CLASS CXTimerAgent : public CXTimerListener
{
public:
	CXTimerAgent(void);
	~CXTimerAgent(void);

public:
	void			StartTimer(int32 iTimerID, int32 iDelay);
	void			StopTimer(int32 iTimerID);
	
public:
	virtual void	OnXTimer(int32 iTimerID) = 0;
	
protected:
	CXTimer			m_Timer;
};




#endif