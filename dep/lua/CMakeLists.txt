cmake_minimum_required(VERSION 3.15)

project("lua")

add_library(${PROJECT_NAME} STATIC
    "lapi.c"
    "lauxlib.c"
    "lbaselib.c"
    "lcode.c"
    "lcorolib.c"
    "lctype.c"
    "ldblib.c"
    "ldebug.c"
    "ldo.c"
    "ldump.c"
    "lfunc.c"
    "lgc.c"
    "linit.c"
    "liolib.c"
    "llex.c"
    "lmathlib.c"
    "lmem.c"
    "loadlib.c"
    "lobject.c"
    "lopcodes.c"
    "loslib.c"
    "lparser.c"
    "lstate.c"
    "lstring.c"
    "lstrlib.c"
    "ltable.c"
    "ltablib.c"
    "ltm.c"
    "lundump.c"
    "lutf8lib.c"
    "lvm.c"
    "lzio.c"
)

if(UNIX)
  add_definitions(-DLUA_USE_POSIX)
endif()

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W0)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -w)
endif()



target_include_directories(${PROJECT_NAME} PUBLIC "./")