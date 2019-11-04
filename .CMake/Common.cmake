# Toolchain support list: [=] OK [x] Not supported [?] Untested
# -	Windows (Win64.cmake)
#	[=] Ninja (via Visual Studio CMake) / MSBuild (via generated solution)
#		[=] clang-cl
#		[=] VC++
#	[=] MinGW Make
#		[=] g++
#	[?] Others
# - MacOS (OSX.cmake)
# 	[=] XCode
# 		[=] Apple Clang
#	[?] Others
# - Linux (Linux.cmake)
# 	[=] Ninja / Make
# 		[=] clang
# 		[=] g++
#	[?] Others

##################################
# Init
##################################
set(STATIC_LIB_NAMES
	physfs
)
set(SFML_LIB_NAMES
	sfml-audio
	sfml-graphics
	sfml-window
	sfml-system
)

find_package(PythonInterp 3.0)
if(NOT PythonInterp_FOUND)
	message(WARNING "Python 3 not found; tools/installers/etc scripts won't work!")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(PLATFORM "Win64")
	include(.CMake/Platforms/Win64.cmake)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	set(PLATFORM "OSX")
	include(.CMake/Platforms/OSX.cmake)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(PLATFORM "Linux")
	include(.CMake/Platforms/Linux.cmake)
else()
	message(FATAL_ERROR "Unsupported system [${CMAKE_SYSTEM_NAME}]!")
endif()

message(STATUS "Environment: [${PLATFORM}] [${CMAKE_GENERATOR}] [${CMAKE_CXX_COMPILER_ID}]")

##################################
# Interface
##################################
function(add_target_compile_definitions TARGET_NAME)
	target_compile_definitions(${TARGET_NAME} PRIVATE
		_UNICODE
		$<$<BOOL:${SFML_STATIC_LIBS}>:SFML_STATIC>
		$<$<NOT:$<CONFIG:Debug>>:
			NDEBUG
			SHIPPING
		>
		$<$<CONFIG:Debug>:
			DEBUGGING
		>
	)
endfunction()

function(add_le_static_library TARGET_NAME SOURCES)
	add_library(${TARGET_NAME} STATIC ${SOURCES})
	target_compile_features(${TARGET_NAME} PUBLIC cxx_std_17)
	output_name(${TARGET_NAME} ${TARGET_NAME}-$<CONFIG>)
	if(LIBRARIES_PATH)
		output_directory(${TARGET_NAME} "${LIBRARIES_PATH}")
	endif()
	recursive_include(${TARGET_NAME} "${CMAKE_CURRENT_SOURCE_DIR}")
	add_target_compile_definitions(${TARGET_NAME})
	set_target_compile_options(${TARGET_NAME})
endfunction()

function(add_le_executable TARGET_NAME EXE_NAME SOURCES)
	add_executable(${TARGET_NAME} ${SOURCES})
	output_name(${TARGET_NAME} ${EXE_NAME})
	if(EXECUTABLE_PATH)
		output_directory(${TARGET_NAME} "${EXECUTABLE_PATH}")
	endif()
	recursive_include(${TARGET_NAME} "${CMAKE_CURRENT_SOURCE_DIR}")
	add_target_compile_definitions(${TARGET_NAME})
	set_target_compile_options(${TARGET_NAME})
	set_target_link_options(${TARGET_NAME})
endfunction()
