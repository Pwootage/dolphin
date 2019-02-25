#include <cstdint>
#include <Core/PowerPC/MMU.h>
#include "../PrimeMemoryDumping/common/GameMemory.h"

#define FIX_ADDR(addr) ((addr) | 0x80000000)
#define RETURN_0_IF_INVALID(addr) if ((addr) >= 0x81800000 || (addr) < 0x80000000){return 0;}

namespace GameMemory {
    uint64_t read_u64(uint32_t address) {
      address = FIX_ADDR(address);
      RETURN_0_IF_INVALID(address);
      return PowerPC::HostRead_U64(address);
    }

    uint32_t read_u32(uint32_t address) {
      address = FIX_ADDR(address);
      RETURN_0_IF_INVALID(address);
      return PowerPC::HostRead_U32(address);
    }

    float read_float(uint32_t address) {
      address = FIX_ADDR(address);
      RETURN_0_IF_INVALID(address);
      union {
          uint32_t i;
          float f;
      } fi;
      fi.i = read_u32(address);
      return fi.f;
    }

    double read_double(uint32_t address) {
      address = FIX_ADDR(address);
      RETURN_0_IF_INVALID(address);
      union {
          uint64_t i;
          double d;
      } di;
      di.i = read_u64(address);
      return di.d;
    }
}
