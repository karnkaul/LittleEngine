##################################
# Init
##################################
option(SFML_STATIC_LIBS "Whether using static libs" ON)

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

set(EXE_SUFFIX ".lx")
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
	set(EXE_SUFFIX ".lxa")
endif()

##################################
# Interface
##################################
function(set_target_platform_libraries)
	if ("${PROJECT_NAME}" STREQUAL "Core")
		target_link_libraries(${PROJECT_NAME} PUBLIC
			pthread
			X11
			Xrandr
		)
	elseif("${PROJECT_NAME}" STREQUAL "Engine" AND SFML_STATIC_LIBS)
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
	# Call and set target_compile_options here
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
		$<${LX_GCC}:-utf-8>
	)
endfunction()

function(set_target_link_options)
	target_link_options(${PROJECT_NAME} PRIVATE
		-no-pie         # Build as application
		-Wl,-z,origin   # Allow $ORIGIN in RUNPATH
	)
endfunction()
