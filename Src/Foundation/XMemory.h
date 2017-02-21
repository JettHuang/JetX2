//\brief
//		memory allocator.
// NOTE: not support multi-threads use it currently.
//

#ifndef __JETX_MEMORY_H__
#define __JETX_MEMORY_H__

#include <new>
#include "OutputDevice.h"

#pragma push_macro("new")
#undef new

// memory allocator
class FMemory
{
public:
	static void* Alloc(size_t InBytes);
	static void* Alloc(size_t InBytes, const char* InFile, int InLine);
	static void Free(void* InPtr);

	static size_t MemoryUsed();
	static void DumpLeak(FOutputDevice *InOutput);


	//statistic data structure
	class FMemAllocEntry
	{
	public:
		FMemAllocEntry(void* InPtr, size_t InBytes, const char* InFile, int InLine)
			: Ptr(InPtr)
			, Bytes(InBytes)
			, File(InFile)
			, Line(InLine)
			, Next(nullptr)
		{}

		~FMemAllocEntry()
		{}

		void*		Ptr;
		size_t		Bytes;
		const char* File;
		int			Line;

		FMemAllocEntry* Next;
	};

private:
	static FMemAllocEntry*	Head;
};

// overload new & delete
void* operator new(size_t Size);

void* operator new[](size_t Size);

void* operator new(size_t Size, const std::nothrow_t&) throw();

void* operator new[](size_t Size, const std::nothrow_t&) throw();

void* operator new(size_t Size, const char* InFile, int InLine);

void* operator new[](size_t Size, const char* InFile, int InLine);

void* operator new(size_t Size, const char* InFile, int InLine, const std::nothrow_t&) throw();

void* operator new[](size_t Size, const char* InFile, int InLine, const std::nothrow_t&) throw();

void operator delete(void* Ptr);

void operator delete[](void* Ptr);

void operator delete  (void* Ptr, const std::nothrow_t&)throw();

void operator delete[](void* Ptr, const std::nothrow_t&)throw();

void operator delete(void* Ptr, const char* InFile, int InLine);

void operator delete[](void* Ptr, const char* InFile, int InLine);

void operator delete  (void* Ptr, const char* InFile, int InLine, const std::nothrow_t&)throw();

void operator delete[](void* Ptr, const char* InFile, int InLine, const std::nothrow_t&)throw();

#pragma pop_macro("new")

//////////////////////////////////////////////////////////////////////////
// memory leak debug

#define XMEM_DETECT_LEAKS		1

#if XMEM_DETECT_LEAKS
	#undef new
	#define new new(__FILE__, __LINE__)
#endif


#endif // __JETX_MEMORY_H__
