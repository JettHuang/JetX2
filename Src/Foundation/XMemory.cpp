//\brief
//		memory manager
//

#include <cstdlib>
#include <cassert>
#include "XMemory.h"

#undef new

//////////////////////////////////////////////////////////////////////////
// FMemory

FMemory::FMemAllocEntry	* FMemory::Head = nullptr;

void* FMemory::Alloc(size_t InBytes)
{
	return Alloc(InBytes, nullptr, -1);
}

void* FMemory::Alloc(size_t InBytes, const char* InFile, int InLine)
{
	void* Ptr = malloc(InBytes);
	
#if XMEM_DETECT_LEAKS
	if (Ptr)
	{
		void* pData = malloc(sizeof(FMemAllocEntry));
		if (pData)
		{
			FMemAllocEntry* pEntry = new(pData) FMemAllocEntry(Ptr, InBytes, InFile, InLine);
			
			pEntry->Next = Head;
			Head = pEntry;
		}
	}
#endif
	return Ptr;
}

void FMemory::Free(void* InPtr)
{
	if (InPtr)
	{
		free(InPtr);

#if XMEM_DETECT_LEAKS
		FMemAllocEntry**PrevLink = &Head;
		FMemAllocEntry *Itr = Head;
		while (Itr)
		{
			if (Itr->Ptr == InPtr)
			{
				*PrevLink = Itr->Next;
				Itr->~FMemAllocEntry();
				free(Itr);

				break;
			}

			PrevLink = &(Itr->Next);
			Itr = Itr->Next;
		} // end while

		assert(Itr); // must find a entry
#endif
	}
}

size_t FMemory::MemoryUsed()
{
	size_t TotalBytes = 0;

#if XMEM_DETECT_LEAKS
	FMemAllocEntry *Itr = Head;
	while (Itr)
	{
		TotalBytes += Itr->Bytes;
		Itr = Itr->Next;
	} // end while

#endif
	return TotalBytes;
}

void FMemory::DumpLeak(FOutputDevice *InOutput)
{
#if XMEM_DETECT_LEAKS
	FMemAllocEntry *Itr = Head;
	while (Itr)
	{
		InOutput->Log(Log_Info, "Ptr:%p, Bytes:%d, __FILE__:%s, __LINE__:%d\n", Itr->Ptr, Itr->Bytes, (Itr->File ? Itr->File : "N/A"), Itr->Line);
		Itr = Itr->Next;
	} // end while
#endif
}

// overload new & delete
#pragma push_macro("new")
#undef new

void* operator new(size_t Size)
{
	return FMemory::Alloc(Size);
}

void* operator new[](size_t Size)
{
	return FMemory::Alloc(Size);
}

void* operator new(size_t Size, const std::nothrow_t&) throw()
{
	return FMemory::Alloc(Size);
}

void* operator new[](size_t Size, const std::nothrow_t&) throw()
{
	return FMemory::Alloc(Size);
}

void* operator new(size_t Size, const char* InFile, int InLine)
{
	return FMemory::Alloc(Size, InFile, InLine);
}

void* operator new[](size_t Size, const char* InFile, int InLine)
{
	return FMemory::Alloc(Size, InFile, InLine);
}

void* operator new(size_t Size, const char* InFile, int InLine, const std::nothrow_t&) throw()
{
	return FMemory::Alloc(Size, InFile, InLine);
}

void* operator new[](size_t Size, const char* InFile, int InLine, const std::nothrow_t&) throw()
{
	return FMemory::Alloc(Size, InFile, InLine);
}

void operator delete(void* Ptr)
{
	FMemory::Free(Ptr);
}

void operator delete[](void* Ptr)
{
	FMemory::Free(Ptr);
}

void operator delete  (void* Ptr, const std::nothrow_t&)throw()
{
	FMemory::Free(Ptr);
}

void operator delete[](void* Ptr, const std::nothrow_t&)throw()
{
	FMemory::Free(Ptr);
}

void operator delete(void* Ptr, const char* InFile, int InLine)
{
	FMemory::Free(Ptr);
}

void operator delete[](void* Ptr, const char* InFile, int InLine)
{
	FMemory::Free(Ptr);
}

void operator delete  (void* Ptr, const char* InFile, int InLine, const std::nothrow_t&)throw()
{
	FMemory::Free(Ptr);
}

void operator delete[](void* Ptr, const char* InFile, int InLine, const std::nothrow_t&)throw()
{
	FMemory::Free(Ptr);
}

#pragma pop_macro("new")
