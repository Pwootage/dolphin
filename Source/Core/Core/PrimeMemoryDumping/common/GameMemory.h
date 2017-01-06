

#ifndef DOLPHIN_EMU_GAMEMEMORY_H
#define DOLPHIN_EMU_GAMEMEMORY_H


namespace GameMemory {
    uint32_t read_u32(uint32_t address);
    float read_float(uint32_t address);
}

#endif //DOLPHIN_EMU_GAMEMEMORY_H
