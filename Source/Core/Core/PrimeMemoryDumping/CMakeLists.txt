set(MEM_SRCS
        common/game_value.cpp
        prime1/CGameAllocator.cpp
        )

add_definitions(-std=c++11)
include_directories(common)
add_library(primememorydumping ${MEM_SRCS})