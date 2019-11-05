##################################
# Init
##################################
# Disable dynamic ThirdParty libraries
set(SFML_STATIC_LIBS ON CACHE INTERNAL "")
set(CMAKE_VS_PLATFORM_NAME "x64" CACHE STRING "" FORCE)

macro(init)
	set(W_MSBUILD 0)
	set(W_CLANG 0)
	set(W_VCXX 0)
	set(W_GCC 0)
	if(CMAKE_GENERATOR MATCHES "^(Visual Studio)")
		set(W_MSBUILD 1)
	endif()
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(W_CLANG 1)
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		set(W_VCXX 1)
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(W_GCC 1)
	else()
		message("\tWARNING: Unsupported compiler [${CMAKE_CXX_COMPILER_ID}], expect build warnings/errors!")
	endif()
endmacro()
init()

string(REPLACE "ZI" "Z7" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
string(REPLACE "Zi" "Z7" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})

if(W_GCC)
	set(LIB_PREFIX "lib")
	set(LIB_EXT "a")
else()
	set(LIB_PREFIX "")
	set(LIB_EXT "lib")
endif()

set(PLATFORM_DLLS
	openal32.dll
)

set(EXE_SUFFIX "" CACHE INTERNAL "" FORCE)

##################################
# Interface
##################################
function(set_target_platform_libraries TARGET_NAME)
	init()
	if ("${TARGET_NAME}" STREQUAL "LittleEngine")
		target_link_libraries(${TARGET_NAME} PUBLIC
			## SFML Windows
			gdi32
			winmm

			## SFML Dependencies
			freetype
			opengl32
			openal32
			flac
			vorbisenc
			vorbisfile
			vorbis
			ogg
		)
	endif()
endfunction()

function(set_target_compile_options TARGET_NAME)
	init()
	set(W_CLANG_FLAGS_COMMON /W4 -Werror=return-type -Wextra -Wconversion -Wunreachable-code -Wdeprecated-declarations -Wtype-limits)
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		target_compile_options(${TARGET_NAME} PRIVATE
			$<$<CONFIG:Debug>:
				-O0
				-g
			>
			$<$<NOT:$<CONFIG:Debug>>:
				-O2
				-Werror
			>
			-Wextra
			-Werror=return-type
			-fexceptions
			-utf-8
		)
	else()
		target_compile_options(${TARGET_NAME} PRIVATE
			$<$<CONFIG:Debug>:
				/Od
				/MDd
			>
			$<$<NOT:$<CONFIG:Debug>>:
				/O2
				/Oi
				/Ot
				$<$<BOOL:${W_MSBUILD}>:/GL>
				/Gy
				/MD
				$<$<BOOL:${W_CLANG}>:-Werror>
			>
			/sdl
			/Z7
			/EHsc
			$<$<BOOL:${W_CLANG}>:${W_CLANG_FLAGS_COMMON}>
			$<$<BOOL:${W_MSBUILD}>:/MP>
		)
	endif()
endfunction()

function(set_target_link_options TARGET_NAME)
	init()
	if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		target_link_options(${TARGET_NAME} PRIVATE
			$<$<CONFIG:Debug>:
				/SUBSYSTEM:CONSOLE
				/OPT:NOREF
				/DEBUG:FULL
			>
			$<$<NOT:$<CONFIG:Debug>>:
				/ENTRY:mainCRTStartup
				/SUBSYSTEM:WINDOWS
				/OPT:REF
				/OPT:ICF
				/INCREMENTAL:NO
				$<$<BOOL:${W_VCXX}>:/LTCG>
			>
			$<$<CONFIG:RelWithDebinfo>:
				/DEBUG:FULL
			>
		)
	endif()
endfunction()
