//
// Created by pwootage on 1/4/17.
//

#ifndef DOLPHIN_EMU_GAME_PTR_H
#define DOLPHIN_EMU_GAME_PTR_H

#include <cstdint>

#define GPTR_METHODS(type) type(uint32_t base_ptr) : game_ptr(base_ptr, 0) {} \
  type(const game_ptr &base_ptr, uint32_t ptr_offset) : game_ptr(base_ptr, ptr_offset) {} \
  type(uint32_t base_ptr, uint32_t ptr_offset) : game_ptr(base_ptr, ptr_offset) {}

#define GPTR(type, name, offset) inline type name() const { return type((this->base), (offset)); }

class game_ptr {
public:
    game_ptr(uint32_t base_ptr, uint32_t ptr_offset) : base(base_ptr), offset(ptr_offset) {}
    game_ptr(const game_ptr &base_ptr, uint32_t ptr_offset) : base(base_ptr.ptr()), offset(ptr_offset) {}

    inline uint32_t ptr() const {
      return base + offset;
    }


protected:
    uint32_t base;
    uint32_t offset;
};

class u32_ptr : public game_ptr {
public:
    GPTR_METHODS(u32_ptr)
};

#endif //DOLPHIN_EMU_GAME_PTR_H
