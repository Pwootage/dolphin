

#ifndef DOLPHIN_EMU_LUAHANDLER_H
#define DOLPHIN_EMU_LUAHANDLER_H

#include "InputCommon/GCPadStatus.h"
#include "duk_config.h"

namespace Scripting {
    struct ScriptOverridableButton {
        const u16 mask;
        bool override;
        bool value;

        ScriptOverridableButton(u16 buttonMask) : mask(buttonMask), override(false), value(false) {
        };
    };

    struct ScriptOverridableAxis {
        bool override;
        u8 value;

        ScriptOverridableAxis() : override(false), value(0) {
        };
    };

    struct ScriptGCPadStatus {
        ScriptOverridableButton
            left = ScriptOverridableButton(PAD_BUTTON_LEFT),
            right = ScriptOverridableButton(PAD_BUTTON_RIGHT),
            down = ScriptOverridableButton(PAD_BUTTON_DOWN),
            up = ScriptOverridableButton(PAD_BUTTON_UP),
            z = ScriptOverridableButton(PAD_TRIGGER_Z),
            r = ScriptOverridableButton(PAD_TRIGGER_R),
            l = ScriptOverridableButton(PAD_TRIGGER_L),
            a = ScriptOverridableButton(PAD_BUTTON_A),
            b = ScriptOverridableButton(PAD_BUTTON_B),
            x = ScriptOverridableButton(PAD_BUTTON_X),
            y = ScriptOverridableButton(PAD_BUTTON_Y),
            start = ScriptOverridableButton(PAD_BUTTON_START);
        ScriptOverridableAxis
            mainX,
            mainY,
            cX,
            cY,
            leftTrigger,
            rightTrigger,
            analogA,
            analogB;
    };

    void RunScriptingFrame();

    void ResetScriptingEnvironment();

    void ProcessGCPadStatus(GCPadStatus *pStatus, int id);
}


#endif //DOLPHIN_EMU_LUAHANDLER_H
