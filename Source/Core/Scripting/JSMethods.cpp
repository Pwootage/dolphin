#include "JSMethods.h"
#include "Core/State.h"
#include "Common/FileUtil.h"
#include "Core/PowerPC/PowerPC.h"

#include <string>
#include <iostream>

namespace JSMethods {
    using namespace std;

    duk_ret_t SaveState(duk_context *ctx) {
      int args = duk_get_top(ctx);
      if (args != 1) {
        return DUK_RET_TYPE_ERROR;
      }
      duk_int_t typ = duk_get_type(ctx, 0);
      if (typ == DUK_TYPE_NUMBER) {
        int slot = duk_require_int(ctx, 0);
        if (slot < 0 || slot >= State::NUM_STATES) {
          return DUK_RET_RANGE_ERROR;
        }
        cout << "Saving state " << slot << endl;
        State::Save(slot, true);
        duk_push_true(ctx);
        return 1;
      }
      if (typ == DUK_TYPE_STRING) {
        //TODO: unify this to prevent bugs (with load)
        const char *str = duk_require_string(ctx, 0);
        string name(str);
        if (name.size() == 0) {
          return DUK_RET_RANGE_ERROR;
        }
        string path = File::GetUserPath(D_USER_IDX) + "/ScriptSaveStates/" + name + ".sav";
        //TODO: fix dumb hack
        if (name[0] == '/' || (name.length() >= 2 && name[1] == ':')) {
          path = name + ".sav";
        }
        cout << "Saving state " << path << endl;
        State::SaveAs(path, true);
        duk_push_true(ctx);
        return 1;
      }
      return DUK_RET_TYPE_ERROR;
    }

    duk_ret_t LoadState(duk_context *ctx) {
      int args = duk_get_top(ctx);
      if (args != 1) {
        return DUK_RET_TYPE_ERROR;
      }
      duk_int_t typ = duk_get_type(ctx, 0);
      if (typ == DUK_TYPE_NUMBER) {
        int slot = duk_require_int(ctx, 0);
        if (slot < 0 || slot >= State::NUM_STATES) {
          return DUK_RET_RANGE_ERROR;
        }
        cout << "Loading state " << slot << endl;
        State::Load(slot);
        duk_push_true(ctx);
        return 1;
      }
      if (typ == DUK_TYPE_STRING) {
        //TODO: unify this to prevent bugs (with save)
        const char *str = duk_require_string(ctx, 0);
        string name(str);
        if (name.size() == 0) {
          return DUK_RET_RANGE_ERROR;
        }
        string path = File::GetUserPath(D_USER_IDX) + "/ScriptSaveStates/" + name + ".sav";
        //TODO: fix dumb hack
        if (name[0] == '/' || (name.length() >= 2 && name[1] == ':')) {
          path = name + ".sav";
        }
        cout << "Loading state " << path << endl;
        State::LoadAs(path);
        duk_push_true(ctx);
        return 1;
      }
      return DUK_RET_TYPE_ERROR;
    }

    duk_ret_t Read32(duk_context *ctx) {
      int args = duk_get_top(ctx);
      if (args != 1) {
        return DUK_RET_TYPE_ERROR;
      }
      u32 addr = duk_require_uint(ctx, 0);
      u32 ret = PowerPC::HostRead_U32(addr);
//      cout << "Recieved read request for " << addr << " " << ret << endl;
      duk_push_int(ctx, ret);
      return 1;
    }
}