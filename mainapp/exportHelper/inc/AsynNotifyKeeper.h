#ifndef _ASYNNOTIFYKEEPER_H_
#define _ASYNNOTIFYKEEPER_H_

#include <assert.h>

// �����ü���һ�𣬽�����߳�ʱ����̬Add/RemoveNotify��NotifyCallBack֮��ľ�������
// ����:
// !!!����notify���첽�����Ժ���ܱ䶯�����Ե���ʱ���ܱ�������߳�remove���ˣ���Ҫ��removeʱ
// �ж��Ƿ��ʱ����ִ�лص����������ִ�лص�����ȴ��ص���ɲ�remove
// ԭ����ļ��ֽ������:
//	1. ���ж�����һ���߳��ڴ���. Ч��������
//	2. ʹ�����ü���������notify������ʱ�䣬ʹ�������е���ʱ��û����. ��notifyʵ����Ҫ�󣬲��������ڸ��ӽӿڵ�һ����notify��
//  3. �ϸ���ƻص�������ʱ����ͨ������ͽṹ��ƽ��. û�Ǹ�����
//	4. ��removeʱ������Ƿ����remove��������ȴ�����remove��ʱ�� :)


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
					// �����ϵ��
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
	volatile bool	_bInRemoving; // ����ɾ��
	volatile long	_lMyRef;
	ObjClassType	*_pObj;
};


#endif //!_ASYNNOTIFYKEEPER_H_