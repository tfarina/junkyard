cmake_minimum_required(VERSION 2.8)
project(hello ASM)

include(CheckLanguage)

check_language(ASM)

if(CMAKE_ASM_COMPILER)
  enable_language(ASM)
endif()

if(MSVC)
  enable_language(ASM_MASM)
else()
  check_language(ASM-ATT)
  if(CMAKE_ASM-ATT_COMPILER)
    enable_language(ASM-ATT)
  endif()
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  add_definitions(-DARCH_x64)
  set(SRCS
    hello_x64.s
  )
else()
  set(SRCS
    hello_x86.s
  )
endif()

add_executable(hello ${SRCS})
