#include <cstdint>
#include <Core/PowerPC/PowerPC.h>
#include "GameMemory.h"

namespace GameMemory {
    uint64_t read_u64(uint32_t address) {
      return PowerPC::HostRead_U64(address | 0x80000000);
    }

    uint32_t read_u32(uint32_t address) {
      return PowerPC::HostRead_U32(address | 0x80000000);
    }

    float read_float(uint32_t address) {
      union {
          uint32_t i;
          float f;
      } fi;
      fi.i = read_u32(address);
      return fi.f;
    }

    double read_double(uint32_t address) {
      union {
          uint64_t i;
          double d;
      } di;
      di.i = read_u64(address);
      return di.d;
    }
}