//
// Created by pwootage on 1/4/17.
//

#ifndef DOLPHIN_EMU_GAME_PTR_H
#define DOLPHIN_EMU_GAME_PTR_H

#include <cstdint>
#include "GameMemory.h"

#define GPTR_METHODS(type) type(uint32_t base_ptr) : game_value(base_ptr, 0) {} \
  type(const game_value &base_ptr, uint32_t ptr_offset) : game_value(base_ptr, ptr_offset) {} \
  type(uint32_t base_ptr, uint32_t ptr_offset) : game_value(base_ptr, ptr_offset) {}

#define GPTR(type, name, offset) inline type name() const { return type((this->base), (offset)); }

class game_value {
public:
    game_value(uint32_t base_ptr, uint32_t ptr_offset) : base(base_ptr), offset(ptr_offset) {}
    game_value(const game_value &base_ptr, uint32_t ptr_offset) : base(base_ptr.ptr()), offset(ptr_offset) {}

    inline uint32_t ptr() const {
      return base + offset;
    }


protected:
    uint32_t base;
    uint32_t offset;
};

class game_u32 : public game_value {
public:
    GPTR_METHODS(game_u32)

    inline uint32_t read() const {
      return GameMemory::read_u32(ptr());
    }
};

template<class T>
class game_ptr : public game_value {
public:
    GPTR_METHODS(game_ptr)

  inline T read() const {
    uint32_t base = ptr();
    return T(base);
  }
};


#endif //DOLPHIN_EMU_GAME_PTR_H
