cmake_minimum_required(VERSION 3.16)
project(VitaOverdrive C CXX)

if(NOT DEFINED ENV{VITASDK})
  message(FATAL_ERROR "VITASDK environment variable is not set")
endif()

set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "Vita toolchain")
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

add_executable(VitaOverdrive
  src/main.cpp
)

target_link_libraries(VitaOverdrive
  vita2d
  SceDisplay_stub
  SceCtrl_stub
  SceSysmodule_stub
  SceGxm_stub
  SceCommonDialog_stub
)

vita_create_self(VitaOverdrive.self VitaOverdrive)
vita_create_vpk(VitaOverdrive.vpk VODR00001 VitaOverdrive.self
  VERSION 01.00
  NAME "Vita Overdrive"
)