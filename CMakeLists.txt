cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(SUN VERSION 1.00 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/out/debug)
    set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

add_compile_options(/utf-8 /Od)

set(_BUILD_TIME_CHECK_GIT FALSE)
add_subdirectory(3rd/ckfont)
add_subdirectory(3rd/cktext)
add_subdirectory(3rd/TSpp)

file(GLOB_RECURSE SOURCE
    src/*.h src/*.cpp
)

add_definitions(-DOUTPUT_MANGLED_NAMES -DNOMINMAX)
add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")
add_library(DLL SHARED ${SOURCE} 
    src/res/resource.h
    src/res/res.rc
)
target_include_directories(DLL PRIVATE src 3rd 3rd/TSpp/src 3rd/include)
target_link_libraries(DLL PRIVATE TSpp ckfont cktext)
target_precompile_headers(DLL PRIVATE src/pch.h)
set_target_properties(DLL PROPERTIES OUTPUT_NAME "SUN")
target_compile_definitions(DLL PRIVATE
    -DWIN32
    -D_WINDOWS
    -D_UNICODE
    -DUNICODE
    $<$<CONFIG:Debug>:-D_DEBUG>
)
