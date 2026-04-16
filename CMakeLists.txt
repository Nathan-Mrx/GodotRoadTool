cmake_minimum_required(VERSION 3.20)

project(GodotRoadTool LANGUAGES NONE)

find_program(SCONS_EXECUTABLE NAMES scons REQUIRED)

cmake_host_system_information(RESULT GRT_HOST_CORES QUERY NUMBER_OF_LOGICAL_CORES)
if(NOT GRT_HOST_CORES)
    set(GRT_HOST_CORES 8)
endif()

set(GRT_PLATFORM "linuxbsd" CACHE STRING "Godot/SCons platform value (linuxbsd, windows, macos, ...)")
set(GRT_ARCH "x86_64" CACHE STRING "Godot/SCons architecture")
set(GRT_JOBS "${GRT_HOST_CORES}" CACHE STRING "Parallel job count passed to SCons (-j)")
set(GRT_GAME_DIR "${CMAKE_SOURCE_DIR}/game" CACHE PATH "Path to the Godot game project")
option(GRT_DEV_BUILD "Build editor with dev_build=yes" ON)
option(GRT_RUN_TARGETS_BUILD_FIRST "Make editor/play depend on the build target" ON)

if(GRT_PLATFORM STREQUAL "linuxbsd")
    set(_grt_default_gdext_platform "linux")
else()
    set(_grt_default_gdext_platform "${GRT_PLATFORM}")
endif()
set(GRT_GDEXT_PLATFORM "${_grt_default_gdext_platform}" CACHE STRING "godot-cpp/extension SCons platform value")

set(_grt_editor_dev_suffix "")
if(GRT_DEV_BUILD)
    set(_grt_editor_dev_suffix ".dev")
endif()

if(CMAKE_HOST_WIN32)
    set(_grt_exe_suffix ".exe")
else()
    set(_grt_exe_suffix "")
endif()

set(
    GRT_ENGINE_BIN
    "${CMAKE_SOURCE_DIR}/engine/bin/godot.${GRT_PLATFORM}.editor${_grt_editor_dev_suffix}.${GRT_ARCH}${_grt_exe_suffix}"
    CACHE FILEPATH
    "Path to the Godot editor binary used by editor/play/dump-extension-api"
)

set(_grt_engine_build_cmd
    ${SCONS_EXECUTABLE}
    -C
    ${CMAKE_SOURCE_DIR}/engine
    platform=${GRT_PLATFORM}
    arch=${GRT_ARCH}
    target=editor
)
if(GRT_DEV_BUILD)
    list(APPEND _grt_engine_build_cmd dev_build=yes)
endif()
list(APPEND _grt_engine_build_cmd -j${GRT_JOBS})

set(_grt_bindings_build_cmd
    ${SCONS_EXECUTABLE}
    -C
    ${CMAKE_SOURCE_DIR}/src/godot-cpp
    platform=${GRT_GDEXT_PLATFORM}
    arch=${GRT_ARCH}
    target=template_debug
    -j${GRT_JOBS}
)

set(_grt_extension_build_cmd
    ${SCONS_EXECUTABLE}
    -C
    ${CMAKE_SOURCE_DIR}/src
    platform=${GRT_GDEXT_PLATFORM}
    arch=${GRT_ARCH}
    target=template_debug
    compiledb=yes
    -j${GRT_JOBS}
)

add_custom_target(build
    COMMAND ${_grt_engine_build_cmd}
    COMMAND ${GRT_ENGINE_BIN} --headless --dump-extension-api
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/extension_api.json
            ${CMAKE_SOURCE_DIR}/src/godot-cpp/gdextension/extension_api.json
    COMMAND ${_grt_bindings_build_cmd}
    COMMAND ${_grt_extension_build_cmd}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Build engine, refresh extension API, rebuild godot-cpp bindings, and build the RoadTool extension"
    USES_TERMINAL
)

if(GRT_RUN_TARGETS_BUILD_FIRST)
    set(_grt_run_depends build)
else()
    set(_grt_run_depends "")
endif()

add_custom_target(editor
    COMMAND ${GRT_ENGINE_BIN} --path ${GRT_GAME_DIR} -e
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS ${_grt_run_depends}
    COMMENT "Launch Godot editor for the game project"
    USES_TERMINAL
)

add_custom_target(play
    COMMAND ${GRT_ENGINE_BIN} --path ${GRT_GAME_DIR}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS ${_grt_run_depends}
    COMMENT "Run the game project"
    USES_TERMINAL
)

