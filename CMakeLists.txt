cmake_minimum_required(VERSION 3.21)
project(wasmapp)

set(CMAKE_CXX_STANDARD 20)

if (APPLE)
    file(GLOB WASMER_LIB $ENV{WASMER_DIR}/lib/libwasmer.dylib)
else ()
    file(GLOB WASMER_LIB $ENV{WASMER_DIR}/lib/libwasmer.so)
endif ()

add_executable(wasmapp main.cpp)

target_link_libraries(wasmapp ${WASMER_LIB})

target_include_directories(wasmapp PUBLIC $ENV{WASMER_DIR}/include)