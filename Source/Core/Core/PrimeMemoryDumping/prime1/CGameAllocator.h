//
// Created by pwootage on 1/4/17.
//

#include "game_value.h"

#ifndef DOLPHIN_EMU_CGAMEALLOCATOR_H
#define DOLPHIN_EMU_CGAMEALLOCATOR_H

class CMemoryBlock : public game_value {
public:
    GPTR_METHODS(CMemoryBlock)

    GPTR(game_u32, sentinel, 0x0)
    GPTR(game_u32, size, 0x4)
    GPTR(game_value, fileAndLinePtr, 0x8)
    GPTR(game_value, typePtr, 0xC)
    GPTR(game_ptr<CMemoryBlock>, prev, 0x10)
    GPTR(game_ptr<CMemoryBlock>, next, 0x14)
    GPTR(game_u32, ctx, 0x18)
    GPTR(game_u32, canary, 0x1C)
};

class CGameAllocator : public game_value {
public:
    GPTR_METHODS(CGameAllocator)

    GPTR(game_u32, heapSize, 0x8)
    GPTR(game_ptr<CMemoryBlock>, first, 0xC)
    GPTR(game_ptr<CMemoryBlock>, last, 0x10)
};

#endif //DOLPHIN_EMU_CGAMEALLOCATOR_H
