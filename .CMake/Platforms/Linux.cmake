##################################
# Init
##################################
macro(init)
	set(LX_CLANG 0)
	set(LX_GCC 0)
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(LX_CLANG 1)
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(LX_GCC 1)
	else()
		message("\tWARNING: Unsupported compiler [${CMAKE_CXX_COMPILER_ID}], expect build warnings/errors!")
	endif()
	set(CMAKE_INSTALL_RPATH_USE_ORIGIN ON)
	set(CMAKE_INSTALL_RPATH "$ORIGIN/Lib")
endmacro()
init()

set(EXE_SUFFIX ".lx" CACHE INTERNAL "" FORCE)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
	set(EXE_SUFFIX ".lxa" CACHE INTERNAL "" FORCE)
endif()

##################################
# Interface
##################################
function(set_target_platform_libraries TARGET_NAME)
	init()
	if ("${TARGET_NAME}" STREQUAL "Core")
		target_link_libraries(${TARGET_NAME} PUBLIC
			m
			pthread
			X11
			Xrandr
		)
	elseif("${TARGET_NAME}" STREQUAL "LittleEngine" AND NOT BUILD_SHARED_LIBS)
		target_link_libraries(${TARGET_NAME} PRIVATE
			freetype
			FLAC
			GL
			m
			ogg
			openal
			udev
			vorbis
			vorbisenc
			vorbisfile
			X11
			Xrandr
		)
	endif()
endfunction()

function(set_target_compile_options TARGET_NAME)
	init()
	target_compile_options(${TARGET_NAME} PRIVATE
		$<$<OR:$<CONFIG:Debug>,$<CONFIG:Develop>>:
			-O0
		>
		$<$<CONFIG:Release>:
			-O2
			-Werror
		>
		$<$<NOT:$<CONFIG:Release>>:
			-g
		>
		-Wextra
		-Werror=return-type
		-fexceptions
		$<$<BOOL:${LX_GCC}>:-utf-8>
	)
endfunction()

function(set_target_link_options TARGET_NAME)
	init()
	target_link_options(${TARGET_NAME} PRIVATE
		-no-pie         # Build as application
		-Wl,-z,origin   # Allow $ORIGIN in RUNPATH
	)
endfunction()
