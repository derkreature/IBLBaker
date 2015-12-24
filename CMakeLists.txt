cmake_minimum_required (VERSION 2.8.3)
project(IBLBaker)

set (UNICODE_DEFAULT ON)


if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/critter")
  message(FATAL_ERROR "The Instant Meshes dependency repositories (Critter, AssImp, etc.) are missing! "
    "You probably did not clone the project with --recursive. It is possible to recover "
    "by calling \"git submodule update --init --recursive\"")
endif()

if (CMAKE_GENERATOR MATCHES "Visual Studio") 
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()


add_definitions( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4005 /wd4996 /wd4477 /wd4267 /wd4244 /nologo" )

# Sanitize build environment for static build with C++11
if (MSVC)

  add_definitions (/D "_CRT_SECURE_NO_WARNINGS")
  add_definitions (/D "__TBB_NO_IMPLICIT_LINKAGE")
  add_definitions (/D "_UNICODE")
  # Parallel build on MSVC (all targets)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

  if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")

    # Disable Eigen vectorization for Windows 32 bit builds (issues with unaligned access segfaults)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DEIGEN_DONT_ALIGN")
  endif()

  # Static build
  set(CompilerFlags
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
  foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
  endforeach()
endif()

#Zlib
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/critter/dependencies/zlib ext_build/zlib)
set_target_properties(zlibstatic PROPERTIES FOLDER "Dependencies")

#AssImp
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/critter/dependencies/assimp ext_build/assimp)
set_target_properties(assimp PROPERTIES FOLDER "Dependencies")

#LibZip
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/critter/dependencies/libzip/ ext_build/libZip)
set_target_properties(zip PROPERTIES FOLDER "Dependencies")

#FreeImage
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/critter/dependencies/FreeImage/Source ext_build/FreeImage)
set_target_properties(FreeImage PROPERTIES FOLDER "Dependencies")
set_target_properties(OpenEXR PROPERTIES FOLDER "Dependencies")
set_target_properties(LibJPEG PROPERTIES FOLDER "Dependencies")
set_target_properties(LibJXR PROPERTIES FOLDER "Dependencies")
set_target_properties(LibOpenJPEG PROPERTIES FOLDER "Dependencies")
set_target_properties(LibWebP PROPERTIES FOLDER "Dependencies")
set_target_properties(LibPNG PROPERTIES FOLDER "Dependencies")
set_target_properties(LibRawLite PROPERTIES FOLDER "Dependencies")
set_target_properties(LibTIFF4 PROPERTIES FOLDER "Dependencies")

#Critter
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/critter ext_build/critter)

set_target_properties(add_from_buffer add_from_filep example fopen_unchanged fread minigzip modify name_locate set_comment_all set_comment_localonly set_comment_removeglobal set_comment_revert set_compression stat_index tryopen uninstall zipcmp zipmerge ziptorrent zlib PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
set_target_properties(add_from_buffer add_from_filep example fopen_unchanged fread minigzip modify name_locate set_comment_all set_comment_localonly set_comment_removeglobal set_comment_revert set_compression stat_index tryopen uninstall zipcmp zipmerge ziptorrent zlib  PROPERTIES FOLDER "Dependency Cruft")

include_directories(
  #IblBaker
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  #critter
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/application
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/codecs
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/input
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/math
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/newui
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/nodes
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/renderAPI
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/resources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/swizzling
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/ui
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/dependencies/assimp/include/assimp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/dependencies/assimp/code
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/dependencies/cmdLine
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/dependencies/FreeImage
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/dependencies/FreeImage/Source
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/dependencies/MurmerHash
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/dependencies/imgui
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/dependencies/libzip/lib
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/dependencies/libzip/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/dependencies/nanovg
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/dependencies/pugixml/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/dependencies/zlib
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/rendererD3D11 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/rendererD3D11/effectsD3D11
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/rendererD3D11/effectsD3D11/Binary
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/rendererD3D11/effectsD3D11/Inc
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/rendererD3D11/errorD3D11
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/resources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/critter/swizzling
  ${CMAKE_CURRENT_SOURCE_DIR}/ui)


# Compile with compiler warnings
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-3]")
    string(REGEX REPLACE "/W[0-3]" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
  endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-anonymous-struct -Wno-c99-extensions -Wno-nested-anon-types")
  endif()
endif()

if(WIN32)
  set(EXTRA_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/resources/IBLBaker.rc")
endif()

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

add_executable(IBLBaker WIN32
  src/IblApplication.cpp
  src/IblApplication.h
  src/IblApplicationHUD.cpp
  src/IblApplicationHUD.h
  src/main.cpp
  ${EXTRA_SOURCE})

set_target_properties(IBLBaker PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE /ENTRY:\"mainCRTStartup\"")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:\"mainCRTStartup\"")

set_target_properties(IBLBaker PROPERTIES FOLDER "Application")
set_target_properties(IBLBaker PROPERTIES COMPILE_DEFINITIONS "IBL_USE_ASS_IMP_AND_FREEIMAGE=1;DIRECTINPUT_VERSION=0x0800;_SCL_SECURE_NO_WARNINGS=1;_CRT_SECURE_NO_WARNINGS=1")

target_link_libraries(IBLBaker zlibstatic zip assimp FreeImage assimp Critter winmm.lib XInput9_1_0.lib D3DCompiler.lib d3D11.lib dxguid.lib dinput8.lib dxgi.lib)

set_target_properties( IBLBaker
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin64"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin64")

add_custom_command(TARGET IBLBaker POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                        $<TARGET_FILE_DIR:IBLBaker> ${CMAKE_CURRENT_SOURCE_DIR}/bin64)


if (WIN32)
  # Quench some warnings on MSVC
  if (MSVC)
    set_source_files_properties(ext/rply/rply.c PROPERTIES COMPILE_FLAGS "/wd4127")
  endif()
endif()
 
