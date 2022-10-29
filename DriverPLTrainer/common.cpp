#include "pch.h"
#include "common.h"

bool memory_readable(void* ptr, size_t byteCount)
{
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(ptr, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == 0)
        return false;

    if (mbi.State != MEM_COMMIT)
        return false;

    if (mbi.Protect == PAGE_NOACCESS || mbi.Protect == PAGE_EXECUTE)
        return false;

    // This checks that the start of memory block is in the same "region" as the
    // end. If it isn't you "simplify" the problem into checking that the rest of 
    // the memory is readable.
    size_t blockOffset = (size_t)((char*)ptr - (char*)mbi.AllocationBase);
    size_t blockBytesPostPtr = mbi.RegionSize - blockOffset;

    if (blockBytesPostPtr < byteCount)
        return memory_readable((char*)ptr + blockBytesPostPtr,
            byteCount - blockBytesPostPtr);

    return true;
}