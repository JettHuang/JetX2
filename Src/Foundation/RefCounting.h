// \brief
//		base class for reference counting object
//

#ifndef __JETX_REFCOUNTING_H__
#define __JETX_REFCOUNTING_H__

#include "JetX.h"


class FRefCountedObject
{
public:
	FRefCountedObject() :
		nRefCount(0)
	{
	}

	virtual ~FRefCountedObject()
	{
		assert(nRefCount == 0);
	}

	int32_t Retain()
	{
		return ++nRefCount;
	}

	int32_t Release()
	{
		assert(nRefCount > 0);
		int32_t NewRef = --nRefCount;
		if (NewRef == 0)
		{
			delete this;
		}

		return NewRef;
	}

	int32_t RetainCount()
	{
		return nRefCount;
	}

protected:
	int32_t	nRefCount;
};

// smart pointer base on ref-counting
template<typename ReferencedType>
class TRefCountPtr
{
public:
	TRefCountPtr() : Reference(nullptr)
	{
	}

	TRefCountPtr(ReferencedType *InReference, bool bRetain=true)
	{
		Reference = InReference;
		if (Reference && bRetain)
		{
			Reference->Retain();
		}
	}

	TRefCountPtr(const TRefCountPtr &InRefCountPtr)
	{
		Reference = InRefCountPtr.Reference;
		if (Reference)
		{
			Reference->Retain();
		}
	}

	~TRefCountPtr()
	{
		if (Reference)
		{
			Reference->Release();
		}
	}

	TRefCountPtr& operator =(ReferencedType* InRefObject)
	{
		ReferencedType* OldRefObject = Reference;
		
		Reference = InRefObject;
		if (Reference)
		{
			Reference->Retain();
		}
		if (OldRefObject)
		{
			OldRefObject->Release();
		}

		return *this;
	}

	TRefCountPtr& operator =(const TRefCountPtr &rhs)
	{
		return (*this = rhs.Reference);
	}

	bool operator==(const TRefCountPtr &rhs)
	{
		return (Reference == rhs.Reference);
	}

	ReferencedType* operator->() const
	{
		return Reference;
	}

	operator ReferencedType*() const
	{
		return Reference;
	}

	ReferencedType* DeRef() const
	{
		return Reference;
	}

	void SafeRelease()
	{
		*this = nullptr;
	}

	int32_t GetReferenceCount()
	{
		if (Reference)
		{
			Reference->Retain();
			return Reference->Release();
		}

		return 0;
	}

	bool IsValidRef() const
	{
		return Reference != nullptr;
	}

	friend bool IsValidRef(const TRefCountPtr &InRef)
	{
		return (InRef.Reference != nullptr);
	}

private:
	ReferencedType	*Reference;
};

typedef TRefCountPtr<FRefCountedObject>		FRefCountedObjectRef;

#endif // __JETX_REFCOUNTING_H__
