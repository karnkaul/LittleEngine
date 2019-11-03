cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project(ThirdParty C CXX)

################################################################################
# Default Configuration
################################################################################
# Pack debug info within lib
string(REPLACE "Zi" "Z7" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
string(REPLACE "Zi" "Z7" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
string(REPLACE "Zi" "Z7" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
string(REPLACE "Zi" "Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

# Ensure x64 for MSVC
if(NOT CMAKE_VS_PLATFORM_NAME)
	set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
if(NOT CMAKE_VS_PLATFORM_NAME STREQUAL "x64")
	message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

################################################################################
# Remote dependencies
################################################################################
# SFML
set(SFML_VERSION 2.5.1)
download_extract_archive(
	"SFML-${SFML_VERSION}-sources.zip"
	"https://www.sfml-dev.org/files/SFML-${SFML_VERSION}-sources.zip"
	"SFML-${SFML_VERSION}"
	"SFML"
)

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
set(PHYSFS_BUILD_TEST OFF CACHE BOOL "" FORCE)

# SFML
set(THIRD_PARTY_INCLUDE_PATHS "${THIRD_PARTY_INCLUDE_PATHS}" "${PROJECT_SOURCE_DIR}/Include" PARENT_SCOPE)

set(SFML_EXTLIBS_PATH "${PROJECT_SOURCE_DIR}/SFML/extlibs")
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
if(PLATFORM STREQUAL "Win64")
	# Force SFML static libraries
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		file(GLOB MSVC_EXT_LIBS "${SFML_EXTLIBS_PATH}/libs-mingw/x64/*.a")
	else()
		if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
			# extlibs path is not auto-detected for clang-cl
			list(APPEND CMAKE_LIBRARY_PATH "${SFML_EXTLIBS_PATH}/libs-msvc-universal/x64")
			# CRT flags are not set for clang-cl
			set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd /Z7")
			set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MD /Z7")
		endif()
		file(GLOB MSVC_EXT_LIBS "${SFML_EXTLIBS_PATH}/libs-msvc-universal/x64/*.lib")
	endif()
	if(EXECUTABLE_PATH)
		set(MSVC_EXT_DLLS "${SFML_EXTLIBS_PATH}/bin/x64/openal32.dll")
		file(COPY ${MSVC_EXT_DLLS} DESTINATION "${EXECUTABLE_PATH}")
	endif()
	if(LIBRARIES_PATH)
		file(COPY ${MSVC_EXT_LIBS} DESTINATION "${LIBRARIES_PATH}")
	endif()
elseif(PLATFORM STREQUAL "OSX")
	# Force SFML shared libraries
	set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
	if(LIBRARIES_PATH)
		file(GLOB OSX_FRAMEWORKS LIST_DIRECTORIES true "${SFML_EXTLIBS_PATH}/libs-osx/Frameworks/*.*")
		file(COPY ${OSX_FRAMEWORKS} DESTINATION "${LIBRARIES_PATH}/../Frameworks")
	endif()
	# Handle Mono presence breaking SFML build pipeline
	find_library(MONO Mono)
	if(NOT "${MONO}" STREQUAL "MONO-NOTFOUND")
		message("Mono is present! Force-including ${MONO}/Headers/freetype2 for sfml-graphics!")
		include_directories("${MONO}/Headers/freetype2")
	endif()
elseif(PLATFORM STREQUAL "Linux")
	# Set SFML static libraries as default
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
endif()

################################################################################
# Sub-projects
################################################################################
add_subdirectory(physfs)
add_subdirectory(SFML)

set_target_properties(physfs-static PROPERTIES OUTPUT_NAME "physfs$<$<CONFIG:DEBUG>:-d>")
if(PLATFORM STREQUAL "Win64")
	set(DEFS _CRT_SECURE_NO_WARNINGS)
	if(W_CLANG OR W_GCC)
		set(OPTIONS -Wno-missing-braces -Wno-deprecated-declarations)
	elseif(W_VCXX)
		set(OPTIONS /wd4996)
	endif()
	foreach(TGT sfml-graphics sfml-system sfml-window)
		target_compile_definitions(${TGT} PRIVATE ${DEFS})
		target_compile_options(${TGT} PRIVATE ${OPTIONS})
	endforeach()
endif()

if(LIBRARIES_PATH)
	foreach(LIB physfs-static sfml-audio sfml-graphics sfml-system sfml-window)
	if ("${CONFIG}" STREQUAL "DEBUG")
		output_directory(${LIB}-d "${LIBRARIES_PATH}")
	else()
		output_directory(${LIB} "${LIBRARIES_PATH}")
	endif()
	endforeach()
endif()
