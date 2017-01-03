include_directories(src/bson)

add_definitions(-std=c++11 -fexceptions)

# Ignore some of the spammy warnings
add_definitions(-Wno-unknown-pragmas -Wno-missing-declarations -Wno-shadow -Wno-unused-variable)

set(BSON_SRCS
        src/bson/time_support.cpp
        src/bson/json.cpp
        src/bson/base64.cpp
        src/bson/parse_number.cpp
        src/bson/hex.cpp
        src/bson/valid.cpp
        src/bson/bson.cpp
        )

add_library(bsoncxx STATIC ${BSON_SRCS})