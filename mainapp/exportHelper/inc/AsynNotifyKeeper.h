#ifndef _ASYNNOTIFYKEEPER_H_
#define _ASYNNOTIFYKEEPER_H_

#include <assert.h>

// 与引用记数一起，解决多线程时，动态Add/RemoveNotify与NotifyCallBack之间的纠结问题
// 问题:
// !!!由于notify在异步启动以后可能变动，所以调用时可能被另外的线程remove掉了，需要在remove时
// 判断是否此时正在执行回调，如果正在执行回调，则等待回调完成才remove
// 原本想的几种解决方法:
//	1. 所有动作在一个线程内处理. 效率有问题
//	2. 使用引用记数，控制notify的生存时间，使其在所有调用时都没死亡. 对notify实现有要求，不方便用在复杂接口的一个子notify上
//  3. 严格控制回调发生的时机，通过编码和结构设计解决. 没那个能力
//	4. 在remove时，检测是否可以remove，不能则等待可以remove的时机 :)


template <class ObjClass>
class AsynNotifyKeeper
{
public:
	typedef ObjClass	ObjClassType;
	
public:
	AsynNotifyKeeper(ObjClassType *pObj)
		: _lMyRef(1), _pObj(pObj), _bInRemoving(false)
	{
	}
	
	// keeper
	long	AddRef()
	{
		assert(_lMyRef>0);
		if ( _lMyRef>0 )
		{
			return InterlockedIncrement((long *)&_lMyRef);
		}
		return 0;
	}
	
	// release
	long	DecRef()
	{
		assert(_lMyRef>0);
		if ( _lMyRef>0 )
		{
			long l = InterlockedDecrement((long *)&_lMyRef);
			if ( 0==l )
			{
				if ( _pObj )
				{
					// 解除关系了
					_pObj = NULL;
				}
			}
			return l;
		}
		return -1;
	}
	
	long	RefCount() const 
	{ 
		return _lMyRef;
	}

	ObjClassType *GetPtr()
	{
		return _pObj;
	}

	ObjClassType *operator->()
	{
		return GetPtr();
	}

	void	Removing()
	{
		_bInRemoving = true;
	}

	bool	IsRemoving() const
	{
		return _bInRemoving;
	}

	bool	CanRemove() const
	{
		return RefCount()<=1;
	}
	
private:
	// no copy
	AsynNotifyKeeper(const AsynNotifyKeeper &);
	AsynNotifyKeeper& operator=(const AsynNotifyKeeper &);
	
protected:
	volatile bool	_bInRemoving; // 即将删除
	volatile long	_lMyRef;
	ObjClassType	*_pObj;
};


#endif //!_ASYNNOTIFYKEEPER_H_