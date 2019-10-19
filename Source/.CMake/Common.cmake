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
function(add_target_compile_definitions)
	target_compile_definitions(${PROJECT_NAME} PRIVATE
		_UNICODE
		$<$<BOOL:${SFML_STATIC_LIBS}>:SFML_STATIC>
		$<$<NOT:$<CONFIG:Debug>>:
			NDEBUG
		>
		$<$<NOT:$<CONFIG:Release>>:
			DEBUGGING
		>
		$<$<CONFIG:Release>:
			SHIPPING
		>
	)
endfunction()

function(add_le_static_library SOURCES)
	add_library(${PROJECT_NAME} STATIC ${SOURCES})
	target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
	output_name(${PROJECT_NAME}-$<CONFIG>)
	output_directory("${BUILD_PATH}/Lib")
	recursive_include("${CMAKE_CURRENT_SOURCE_DIR}")
	add_target_compile_definitions()
	set_target_compile_options()
endfunction()

function(add_le_executable EXE_NAME SOURCES)
	add_executable(${PROJECT_NAME} ${SOURCES})
	output_name(${EXE_NAME})
	output_directory("${BUILD_PATH}/Exe")
	recursive_include("${CMAKE_CURRENT_SOURCE_DIR}")
	add_target_compile_definitions()
	set_target_compile_options()
	set_target_link_options()
endfunction()
