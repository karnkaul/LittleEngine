cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

# Build Configurations
set(CMAKE_CONFIGURATION_TYPES
	"Debug"
	"RelWithDebInfo"
	CACHE STRING "" FORCE
)
set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "")

project(ThirdParty C CXX)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(PLATFORM "Win64")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	set(PLATFORM "OSX")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(PLATFORM "Linux")
else()
	message(FATAL_ERROR "Unsupported system [${CMAKE_SYSTEM_NAME}]!")
endif()

message(STATUS "Environment: [${PLATFORM}] [${CMAKE_GENERATOR}] [${CMAKE_CXX_COMPILER_ID}]")

# Prevent in-source builds
string(FIND "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}" SRC_IDX)
if(NOT SRC_IDX EQUAL -1)
	message(FATAL_ERROR "Cannot build in './Source/ThirdParty' (or a sub-directory)!\nNOTE: Remove 'CMakeCache.txt' and 'CMakeFiles/' from ${PROJECT_BINARY_DIR}")
endif()

################################################################################
# Default Configuration
################################################################################
# Pack debug info within lib
string(REPLACE "Zi" "Z7" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
string(REPLACE "Zi" "Z7" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
string(REPLACE "Zi" "Z7" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
string(REPLACE "Zi" "Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

# Suppress Zero Check
set(CMAKE_SUPPRESS_REGENERATION true)
# Use IDE folders feature
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# Disable in-source changes and builds
set(CMAKE_DISABLE_SOURCE_CHANGES TRUE)
set(CMAKE_DISABLE_IN_SOURCE_BUILD TRUE)

set(OUTPUT_PATH "${PROJECT_SOURCE_DIR}/../../_Build/ThirdParty/Lib")
option(BUILD_SHARED_LIBS OFF)

# Force set output paths
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_PATH} CACHE STRING "" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_PATH} CACHE STRING "" FORCE)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
	string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OUTPUT_PATH} CACHE STRING "" FORCE)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OUTPUT_PATH} CACHE STRING "" FORCE)
endforeach()

# Ensure x64 for MSVC
if(NOT CMAKE_VS_PLATFORM_NAME)
	set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"))
	message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

################################################################################
# Library overrides
################################################################################
# PhysicsFS
set(PHYSFS_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_7Z OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_GRP OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_HOG OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_ISO9660 OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_MVL OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_QPAK OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_SLB OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_VDF OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_WAD OFF CACHE BOOL "" FORCE)

# SFML
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
if(PLATFORM STREQUAL "Win64")
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${PROJECT_SOURCE_DIR}/SFML/extlibs/libs-msvc-universal/x64" FORCE)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
		set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MD")
	endif()
elseif(UNIX)
	set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
endif()

################################################################################
# Remote dependencies
################################################################################
# SFML
set(SFML_VERSION SFML-2.5.1)
if(EXISTS "${PROJECT_SOURCE_DIR}/${SFML_VERSION}")
	file(RENAME "${PROJECT_SOURCE_DIR}/${SFML_VERSION}" "${PROJECT_SOURCE_DIR}/SFML")
endif()
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/SFML")
	set(DELETE_ZIP OFF)
	if(NOT EXISTS "${PROJECT_SOURCE_DIR}/${SFML_VERSION}-sources.zip")
		message(STATUS "Downloading https://www.sfml-dev.org/files/${SFML_VERSION}-sources.zip...")
		file(DOWNLOAD
			"https://www.sfml-dev.org/files/${SFML_VERSION}-sources.zip" 
			"${PROJECT_SOURCE_DIR}/${SFML_VERSION}-sources.zip"
		)
		set(DELETE_ZIP ON)
	endif()
	message(STATUS "Extracting ${SFML_VERSION}-sources.zip to SFML...")
	execute_process(COMMAND 
		${CMAKE_COMMAND} -E tar -xf "${PROJECT_SOURCE_DIR}/${SFML_VERSION}-sources.zip"
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		OUTPUT_QUIET
		ERROR_QUIET
	)
	file(RENAME "${PROJECT_SOURCE_DIR}/${SFML_VERSION}" "${PROJECT_SOURCE_DIR}/SFML")
	if(DELETE_ZIP)
		file(REMOVE "${PROJECT_SOURCE_DIR}/${SFML_VERSION}-sources.zip")
	endif()
endif()

################################################################################
# Local dependencies
################################################################################
# SFML
set(SFML_EXTLIBS_PATH "${PROJECT_SOURCE_DIR}/SFML/extlibs")
if(PLATFORM STREQUAL "Win64")
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		file(GLOB MSVC_EXT_LIBS "${SFML_EXTLIBS_PATH}/libs-msvc-universal/x64/*.lib")
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		file(GLOB MSVC_EXT_LIBS "${SFML_EXTLIBS_PATH}/libs-mingw/x64/*.a")
	else()
		message(WARNING "Unknown compiler environment! SFML libraries not copied, LittleEngine build will fail!")
	endif()
	list(APPEND MSVC_EXT_LIBS "${SFML_EXTLIBS_PATH}/bin/x64/openal32.dll")
	file(COPY ${MSVC_EXT_LIBS} DESTINATION "${OUTPUT_PATH}")
elseif(PLATFORM STREQUAL "OSX")
	file(GLOB OSX_FRAMEWORKS LIST_DIRECTORIES true "${SFML_EXTLIBS_PATH}/libs-osx/Frameworks/*.*")
	file(COPY ${OSX_FRAMEWORKS} DESTINATION "${OUTPUT_PATH}/../Frameworks")
endif()

################################################################################
# Sub-projects
################################################################################
add_subdirectory(physfs)
add_subdirectory(SFML)

set_target_properties(physfs-static PROPERTIES OUTPUT_NAME "physfs$<$<CONFIG:DEBUG>:-d>")

if(PLATFORM STREQUAL "OSX")
	# Handle Mono presence breaking SFML build pipeline
	find_library(MONO Mono)
	if(NOT "${MONO}" STREQUAL "MONO-NOTFOUND")
		message("Mono is present! Force-including ${MONO}/Headers/freetype2 for sfml-graphics!")
		target_include_directories(sfml-graphics PRIVATE "${MONO}/Headers/freetype2")
	endif()
endif()
