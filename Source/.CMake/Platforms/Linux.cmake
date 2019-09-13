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
SET(CMAKE_INSTALL_RPATH "$ORIGIN/Lib")

set(PLATFORM_STATIC_LIBS_RELEASE "")
set(PLATFORM_STATIC_LIBS_DEBUG "")
set(PLATFORM_SHARED_LIBS_RELEASE "")
set(PLATFORM_SHARED_LIBS_DEBUG "")
foreach(LIB_NAME ${STATIC_LIB_NAMES})
	list(APPEND PLATFORM_STATIC_LIBS_RELEASE "lib${LIB_NAME}.a")
	list(APPEND PLATFORM_STATIC_LIBS_DEBUG "lib${LIB_NAME}-d.a")
endforeach()
foreach(LIB_NAME ${SFML_LIB_NAMES})
	if(NOT SFML_STATIC_LIBS)
		list(APPEND PLATFORM_SHARED_LIBS_RELEASE "lib${LIB_NAME}.so")
		list(APPEND PLATFORM_SHARED_LIBS_RELEASE "lib${LIB_NAME}.so.2.5")
		list(APPEND PLATFORM_SHARED_LIBS_RELEASE "lib${LIB_NAME}.so.2.5.1")
		list(APPEND PLATFORM_SHARED_LIBS_DEBUG "lib${LIB_NAME}-d.so")
		list(APPEND PLATFORM_SHARED_LIBS_DEBUG "lib${LIB_NAME}-d.so.2.5")
		list(APPEND PLATFORM_SHARED_LIBS_DEBUG "lib${LIB_NAME}-d.so.2.5.1")
	else()
		list(APPEND PLATFORM_STATIC_LIBS_RELEASE "lib${LIB_NAME}-s.a")
		list(APPEND PLATFORM_STATIC_LIBS_DEBUG "lib${LIB_NAME}-s-d.a")
	endif()
endforeach()

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

function(ensure_dependencies_present)
	file(MAKE_DIRECTORY "${BUILD_THIRD_PARTY_PATH}/Lib")
	# SFML runtime libraries
	find_package(X11)
	find_library(X11_RANDR Xrandr)
	find_library(OPENAL openal)
	if(X11_RANDR STREQUAL "X11_RANDR-NOTFOUND")
		message(FATAL_ERROR "`Xrandr` not found!")
	endif()
	if(OPENAL STREQUAL "OPENAL-NOTFOUND")
		message(FATAL_ERROR "`openal` not found!")
	endif()
	set(LIB_FILENAMES 
		${PLATFORM_STATIC_LIBS_RELEASE}
		${PLATFORM_SHARED_LIBS_RELEASE}
	)
	if(NOT CI_BUILD)
		list(APPEND LIB_FILENAMES 
			${PLATFORM_STATIC_LIBS_DEBUG}
			${PLATFORM_SHARED_LIBS_DEBUG}
		)
	endif()
	ensure_files_present("${BUILD_THIRD_PARTY_PATH}/Lib" "${LIB_FILENAMES}")
endfunction()

function(install_runtime EXE_NAME)
	install_file_list("${PLATFORM_SHARED_LIBS_RELEASE}" "${BUILD_THIRD_PARTY_PATH}/Lib" "${RUNTIME_PATH}/Lib")
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
