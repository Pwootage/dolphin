//
// Created by pwootage on 1/4/17.
//

#include "game_ptr.h"

#ifndef DOLPHIN_EMU_CGAMEALLOCATOR_H
#define DOLPHIN_EMU_CGAMEALLOCATOR_H

class CMemoryBlock : public game_ptr {
public:
    GPTR_METHODS(CMemoryBlock)

    GPTR(u32_ptr, sentinel, 0x0)
    GPTR(u32_ptr, size, 0x4)
    GPTR(game_ptr, fileAndLinePtr, 0x8)
    GPTR(game_ptr, typePtr, 0xC)
    GPTR(CMemoryBlock, prev, 0x10)
    GPTR(CMemoryBlock, next, 0x14)
    GPTR(u32_ptr, ctx, 0x18)
    GPTR(u32_ptr, canary, 0x1C)
};

class CGameAllocator : public game_ptr {
public:
    GPTR_METHODS(CGameAllocator)

    GPTR(game_ptr, heapSize, 0x8)
    GPTR(CMemoryBlock, first, 0xC)
    GPTR(CMemoryBlock, last, 0x10)
};

#endif //DOLPHIN_EMU_CGAMEALLOCATOR_H
