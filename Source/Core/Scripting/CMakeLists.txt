set(SRCS ScriptingHandler.cpp
        JSMethods.cpp
        duktape.cpp)

set(LIBS common)

#add_definitions(-DDUK_OPT_CPP_EXCEPTIONS)

add_dolphin_library(scripting "${SRCS}" "${LIBS}")
