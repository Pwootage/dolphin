#include "Core/ConfigManager.h"
#include "Common/FileUtil.h"
#include "Common/Assert.h"
#include "ScriptingHandler.h"
#include "duktape.h"
#include "JSMethods.h"
#include <vector>
#include <string>
#include <iostream>

namespace Scripting {
    using namespace std;

    // Static values
    static duk_context *ctx = nullptr;
    static bool overrideGamepad = false;
    static GCPadStatus currentFrameDesiredStatus;

    //Internal Methods
    void InitScripting();

    void SetupScriptingEnvironment();

    void RunScriptingFrame() {
      if (ctx == nullptr) {
        InitScripting();
      }
      duk_push_global_object(ctx);
      duk_get_prop_string(ctx, -1, "onFrame");
      if (duk_is_callable(ctx, -1)) {
        if (duk_pcall(ctx, 0) != 0) {
          cout << "Error in script: " << duk_safe_to_string(ctx, -1) << endl;
        }
        duk_pop(ctx); // Ignore result
      } else {
        duk_pop(ctx); // undefined
      }
      duk_pop(ctx); // Global object
    }

    void InitScripting() {
      if (ctx != nullptr) {
        ResetScriptingEnvironment();
      }
      ctx = duk_create_heap_default();
      if (ctx == nullptr) {
        PanicAlert("Failed to init duktape context");
      }
      SetupScriptingEnvironment();

      string scriptPath;
      SConfig::GetInstance()
          .LoadLocalGameIni()
          .GetOrCreateSection("Scripting")
          ->Get("script", &scriptPath, "");

      if (scriptPath.length() > 0) {
        string path = File::GetUserPath(D_USER_IDX) + scriptPath;
        //TODO: fix dumb hack
        if (scriptPath[0] == '/' || (scriptPath.length() >= 2 && scriptPath[1] == ':')) {
          path = scriptPath;
        }
        cout << "Loading script from path " << path << endl;
        if (duk_peval_file(ctx, path.c_str()) != 0) {
          cout << "Error in script: " << duk_safe_to_string(ctx, -1) << endl;
        }
        duk_pop(ctx);
      } else {
        cout << "No script set." << endl;
      }
    }

    void SetupScriptingEnvironment() {
      duk_push_global_object(ctx);

      duk_push_object(ctx);

      duk_push_c_function(ctx, JSMethods::SaveState, 1);
      duk_put_prop_string(ctx, -2, "saveState");
      duk_push_c_function(ctx, JSMethods::LoadState, 1);
      duk_put_prop_string(ctx, -2, "loadState");

      duk_push_c_function(ctx, JSMethods::Read32, 1);
      duk_put_prop_string(ctx, -2, "read32");

      //Global Object (from above)
      duk_put_prop_string(ctx, -2, "Dolphin");
      duk_pop(ctx); //remove global object
    }

    void ResetScriptingEnvironment() {
      if (ctx != nullptr) {
        cout << "Shutting down scripting." << endl;
        duk_destroy_heap(ctx);
        ctx = nullptr;
      }
    }

    void ProcessGCPadStatus(GCPadStatus *status, int id) {
      if (overrideGamepad) {
        *status = currentFrameDesiredStatus;
      }
    }
}