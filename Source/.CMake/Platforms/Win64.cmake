##################################
# Init
##################################
# Disable dynamic ThirdParty libraries
set(SFML_STATIC_LIBS ON CACHE INTERNAL "")
set(CMAKE_VS_PLATFORM_NAME "x64" CACHE STRING "" FORCE)

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

if(W_CLANG)
	# /ZI throws warnings, replace with /Zi
	string(REPLACE "ZI" "Zi" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
endif()

if(W_GCC)
	set(LIB_PREFIX "lib")
	set(LIB_EXT "a")
else()
	set(LIB_PREFIX "")
	set(LIB_EXT "lib")
endif()

set(PLATFORM_STATIC_LIBS_RELEASE "")
set(PLATFORM_STATIC_LIBS_DEBUG "")
foreach(LIB_NAME ${STATIC_LIB_NAMES})
	list(APPEND PLATFORM_STATIC_LIBS_RELEASE "${LIB_PREFIX}${LIB_NAME}.${LIB_EXT}")
	list(APPEND PLATFORM_STATIC_LIBS_DEBUG "${LIB_PREFIX}${LIB_NAME}-d.${LIB_EXT}")
endforeach()
foreach(LIB_NAME ${SFML_LIB_NAMES})
	list(APPEND PLATFORM_STATIC_LIBS_RELEASE "${LIB_PREFIX}${LIB_NAME}-s.${LIB_EXT}")
	list(APPEND PLATFORM_STATIC_LIBS_DEBUG "${LIB_PREFIX}${LIB_NAME}-s-d.${LIB_EXT}")
endforeach()

set(PLATFORM_STATIC_LIBS_COMMON
	${LIB_PREFIX}FLAC.${LIB_EXT}
	${LIB_PREFIX}freetype.${LIB_EXT}
	${LIB_PREFIX}ogg.${LIB_EXT}
	${LIB_PREFIX}openal32.${LIB_EXT}
	${LIB_PREFIX}vorbis.${LIB_EXT}
	${LIB_PREFIX}vorbisenc.${LIB_EXT}
	${LIB_PREFIX}vorbisfile.${LIB_EXT}
)

set(PLATFORM_DLLS
	openal32.dll
)

set(EXE_SUFFIX "")

##################################
# Interface
##################################
function(set_target_platform_libraries)
	if ("${PROJECT_NAME}" STREQUAL "Engine")
		target_link_libraries(${PROJECT_NAME} PUBLIC
			## SFML Windows
			gdi32
			winmm

			## SFML Dependencies
			freetype
			opengl32
			gdi32
			openal32
			flac
			vorbisenc
			vorbisfile
			vorbis
			ogg
		)
	endif()
endfunction()

function(ensure_dependencies_present)
	file(MAKE_DIRECTORY "${BUILD_THIRD_PARTY_PATH}/Lib")
	set(LIB_FILENAMES
		${PLATFORM_STATIC_LIBS_COMMON}
		${PLATFORM_STATIC_LIBS_RELEASE}
		${PLATFORM_STATIC_LIBS_DEBUG}
		${PLATFORM_DLLS}
	)
	ensure_files_present("${BUILD_THIRD_PARTY_PATH}/Lib" "${LIB_FILENAMES}")
	copy_file_list("${PLATFORM_DLLS}" "${BUILD_THIRD_PARTY_PATH}/Lib" "${RUNTIME_PATH}")
endfunction()

function(install_runtime EXE_NAME)
	install_file_list("${PLATFORM_DLLS}" "${BUILD_THIRD_PARTY_PATH}/Lib" "${RUNTIME_PATH}")
endfunction()

function(set_target_compile_options)
	set(W_CLANG_FLAGS_SHIP -Werror)
	set(W_CLANG_FLAGS_COMMON /W4 -Werror=return-type -Wextra -Wconversion -Wunreachable-code -Wdeprecated-declarations -Wtype-limits)
	if(W_GCC)
		target_compile_options(${PROJECT_NAME} PRIVATE
			$<$<OR:$<CONFIG:Debug>,$<CONFIG:Develop>>:
				-O0
			>
			$<$<OR:$<CONFIG:Release>,$<CONFIG:Ship>>:
				-O2
				-Werror
			>
			$<$<NOT:$<CONFIG:Ship>>:
				-g
			>
			-Wextra
			-Werror=return-type
			-fexceptions
			-utf-8
		)
	else()
		target_compile_options(${PROJECT_NAME} PRIVATE
			$<$<CONFIG:Debug>:
				/Od
				/MDd
			>
			$<$<CONFIG:Develop>:
				/Od
				/MD
			>
			$<$<OR:$<CONFIG:Release>,$<CONFIG:Ship>>:
				/O2
				/Oi
				/Ot
				$<$<BOOL:${W_MSBUILD}>:/GL>
				/Gy
				/MD
				$<$<BOOL:${W_CLANG}>:${W_CLANG_FLAGS_SHIP}>
			>
			$<$<CONFIG:Release>:
				/Zo
			>
			/sdl
			/Z7
			/EHsc
			/std:c++17
			$<$<BOOL:${W_CLANG}>:${W_CLANG_FLAGS_COMMON}>
			$<$<BOOL:${W_MSBUILD}>:/MP>
		)
	endif()
endfunction()

function(set_target_link_options)
	if(NOT W_GCC)
		target_link_options(${PROJECT_NAME} PRIVATE
			$<$<OR:$<CONFIG:Debug>,$<CONFIG:Develop>>:
				/SUBSYSTEM:CONSOLE
				/OPT:NOREF
			>
			$<$<OR:$<CONFIG:Release>,$<CONFIG:Ship>>:
				/ENTRY:mainCRTStartup
				/SUBSYSTEM:WINDOWS
				/OPT:REF
				/OPT:ICF
				/INCREMENTAL:NO
				$<$<BOOL:${W_VCXX}>:/LTCG>
			>
			/DEBUG:FULL
		)
	endif()
endfunction()


