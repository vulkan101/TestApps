﻿# CMakeList.txt : CMake project for testSignatures, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (testSignatures "testSignatures.cpp" "testSignatures.h" "codec/enum_codec.c" "codec/enum_codec.h" "printing/printing.c" "printing/printing.h" "utils/utils.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET testSignatures PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
