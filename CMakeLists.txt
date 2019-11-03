cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

project(LittleEngine C CXX)

set(CMAKE_DISABLE_SOURCE_CHANGES TRUE)
set(CMAKE_DISABLE_IN_SOURCE_BUILD TRUE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Init
################################################################################
# Prevent in-source builds
if("${PROJECT_BINARY_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}")
	message(FATAL_ERROR "Cannot build in './Source' (or a sub-directory)!\nNOTE: Remove 'CMakeCache.txt' and 'CMakeFiles/' from ${PROJECT_BINARY_DIR}")
endif()

# Init/Update Submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
	option(UPDATE_SUBMODULES "Init/Update submodules during generation" ON)
	if(UPDATE_SUBMODULES)
		message(STATUS "Updating git submodules...")
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
			WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
			RESULT_VARIABLE UPDATE_SUBMODULES_RESULT
		)
		if(NOT UPDATE_SUBMODULES_RESULT EQUAL "0")
			message(FATAL_ERROR "git submodule update failed!")
		endif()
	endif()
endif()

set(THIRD_PARTY_INCLUDE_PATHS "")

# Dynamic load path
SET(CMAKE_SKIP_BUILD_RPATH FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

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
set(BUILD_VERSION_MINOR 5 CACHE STRING "Build Version Minor" FORCE)
set(BUILD_VERSION_PATCH 0 CACHE STRING "Build Version Patch" FORCE)
set(BUILD_VERSION_PRE 0 CACHE STRING "Build Version Pre" FORCE)
set(BUILD_COMMIT_HASH ${BUILD_COMMIT_HASH} CACHE STRING "Build Version Commit" FORCE)

################################################################################
# Includes
################################################################################
include(.CMake/Utils.cmake)
include(.CMake/Common.cmake)

################################################################################
# Sub-projects
################################################################################
add_subdirectory(ThirdParty)
add_subdirectory(Core)
add_subdirectory(Engine)

