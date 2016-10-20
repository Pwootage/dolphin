#ifndef DOLPHIN_EMU_JSMETHODS_H
#define DOLPHIN_EMU_JSMETHODS_H

#include "duktape.h"

namespace JSMethods {
    duk_ret_t SaveState(duk_context *ctx);
    duk_ret_t LoadState(duk_context *ctx);
    duk_ret_t Read32(duk_context *ctx);
}

#endif //DOLPHIN_EMU_JSMETHODS_H
