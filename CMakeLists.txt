cmake_minimum_required(VERSION 3.0)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(psp2wpp)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -Wall -O2 -fno-inline")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")

# set(VITA_ELF_CREATE_FLAGS "${VITA_ELF_CREATE_FLAGS} -vvv")
# set(VITA_MAKE_FSELF_FLAGS "${VITA_MAKE_FSELF_FLAGS} -a 0x2808000000000000")

enable_language(C ASM)

include_directories(
)

link_directories(
  # ../kernel/build/stubs/
)

add_executable(${PROJECT_NAME}
  src/main.c
  src/conf_reader.c
  src/wave_config.c
)

target_link_libraries(${PROJECT_NAME}
  ScePaf_stub_weak
  SceIofilemgr_stub
  SceKernelThreadMgr_stub
  SceLibKernel_stub
  SceVshBridge_stub
  taihen_stub
)

set_target_properties(${PROJECT_NAME}
  PROPERTIES LINK_FLAGS "-nostdlib"
  COMPILE_FLAGS "-D__PSP2_USER__"
)

vita_create_self(${PROJECT_NAME}.suprx ${PROJECT_NAME}
  CONFIG exports.yml
  UNSAFE
  STRIPPED
  REL_OPTIMIZE
)
