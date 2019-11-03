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

	set(CMAKE_INSTALL_RPATH_USE_ORIGIN ON PARENT_SCOPE)
	set(CMAKE_INSTALL_RPATH "$ORIGIN/Lib" PARENT_SCOPE)
endmacro()
init()

set(EXE_SUFFIX ".lx" PARENT_SCOPE)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
	set(EXE_SUFFIX ".lxa" PARENT_SCOPE)
endif()

##################################
# Interface
##################################
function(set_target_platform_libraries)
	init()
	if ("${PROJECT_NAME}" STREQUAL "Core")
		target_link_libraries(${PROJECT_NAME} PUBLIC
			pthread
			X11
			Xrandr
		)
	elseif("${PROJECT_NAME}" STREQUAL "Engine" AND NOT BUILD_SHARED_LIBS)
		target_link_libraries(${PROJECT_NAME} PRIVATE
			freetype
			FLAC
			GL
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

function(set_target_compile_options)
	init()
	target_compile_options(${PROJECT_NAME} PRIVATE
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

function(set_target_link_options)
	init()
	target_link_options(${PROJECT_NAME} PRIVATE
		-no-pie         # Build as application
		-Wl,-z,origin   # Allow $ORIGIN in RUNPATH
	)
endfunction()
