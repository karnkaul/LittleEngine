cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

# Default Configuration
set(CMAKE_BUILD_TYPE "Develop" CACHE STRING "")
set(CMAKE_DISABLE_SOURCE_CHANGES TRUE)
set(CMAKE_DISABLE_IN_SOURCE_BUILD TRUE)

project(LittleEngine)

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
	Debug
	Develop
	Release
	CACHE STRING "" FORCE
)

set(INSTALL_CONFIGS
	Develop
	Release
)

set(CMAKE_C_FLAGS_DEVELOP "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_DEVELOP "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_EXE_LINKER_FLAGS_DEVELOP "${CMAKE_EXE_LINKER_FLAGS}")

################################################################################
# Init
################################################################################
# Prevent in-source builds
string(FIND "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}" SRC_IDX)
if(NOT SRC_IDX EQUAL -1)
	message(FATAL_ERROR "Cannot build in './Source' (or a sub-directory)!\nNOTE: Remove 'CMakeCache.txt' and 'CMakeFiles/' from ${PROJECT_BINARY_DIR}")
endif()

# Init/Update Submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/../.git")
	option(GIT_SUBMODULE "Init/Update submodules during generation" ON)
	if(GIT_SUBMODULE)
		message(STATUS "Updating git submodules...")
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
			WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
			RESULT_VARIABLE GIT_SUBMODULE_RESULT
		)
		if(NOT GIT_SUBMODULE_RESULT EQUAL "0")
			message(FATAL_ERROR "git submodule update failed!")
		endif()
	endif()
endif()
set(DEEP_SUBMODULE_FILE "Core/Core/Game/LECoreUtils/LEAsserts/LESysDialog/SysDialog.cpp")
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/${DEEP_SUBMODULE_FILE}")
	message(FATAL_ERROR "Missing required submodules/files!\n[${DEEP_SUBMODULE_FILE}]")
endif()

get_filename_component(ROOT_PATH "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)
if(CI_BUILD)
	set(BUILD_PATH "${ROOT_PATH}/_Build/CI")
else()
	set(BUILD_PATH "${ROOT_PATH}/_Build")
endif()
set(BUILD_THIRD_PARTY_PATH "${BUILD_PATH}/ThirdParty")
set(RUNTIME_PATH "${ROOT_PATH}/Runtime")

# Dynamic load path
SET(CMAKE_SKIP_BUILD_RPATH FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 1)

# Suppress ZERO-CHECK
set(CMAKE_SUPPRESS_REGENERATION true)

# Version
execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
	OUTPUT_VARIABLE BUILD_COMMIT_HASH
	ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT BUILD_COMMIT_HASH)
	set(BUILD_COMMIT_HASH 0000000)
endif()
string(STRIP "${BUILD_COMMIT_HASH}" BUILD_COMMIT_HASH)
set(BUILD_VERSION_MAJOR 0 CACHE STRING "Build Version Major" FORCE)
set(BUILD_VERSION_MINOR 4 CACHE STRING "Build Version Minor" FORCE)
set(BUILD_VERSION_PATCH 9 CACHE STRING "Build Version Patch" FORCE)
set(BUILD_VERSION_PRE 0 CACHE STRING "Build Version Pre" FORCE)
set(BUILD_COMMIT_HASH ${BUILD_COMMIT_HASH} CACHE STRING "Build Version Commit" FORCE)

################################################################################
# Includes
################################################################################
include(.CMake/Utils.cmake)
include(.CMake/Common.cmake)

################################################################################
# ThirdParty outputs
################################################################################
# This flow uses low-level `if(EXISTS) instead of
# `find_library()` so as to track filesystem changes
ensure_dependencies_present()

################################################################################
# Use IDE folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################
add_subdirectory(Core)
add_subdirectory(Engine)
add_subdirectory(Game)
add_subdirectory(App)
