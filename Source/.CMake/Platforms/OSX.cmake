##################################
# Init
##################################
# Force dynamic ThirdParty libraries
set(SFML_STATIC_LIBS OFF CACHE INTERNAL "")

set(APL_CLANG 0)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
	set(APL_CLANG 1)
	foreach(CONFIG in ${CMAKE_CONFIGURATION_TYPES})
		file(MAKE_DIRECTORY "${BUILD_THIRD_PARTY_PATH}/Lib/${CONFIG}")
	endforeach()
else()
	message("\tWARNING: Unsupported compiler [${CMAKE_CXX_COMPILER_ID}], expect build warnings/errors!")
endif()

SET(CMAKE_INSTALL_RPATH "@executable_path/Lib")

set(PLATFORM_STATIC_LIBS_RELEASE "")
set(PLATFORM_STATIC_LIBS_DEBUG "")
set(PLATFORM_SHARED_LIBS_RELEASE "")
set(PLATFORM_SHARED_LIBS_DEBUG "")
foreach(LIB_NAME ${STATIC_LIB_NAMES})
	list(APPEND PLATFORM_STATIC_LIBS_RELEASE "lib${LIB_NAME}.a")
	list(APPEND PLATFORM_STATIC_LIBS_DEBUG "lib${LIB_NAME}-d.a")
endforeach()
foreach(LIB_NAME ${SFML_LIB_NAMES})
	list(APPEND PLATFORM_SHARED_LIBS_RELEASE "lib${LIB_NAME}.dylib")
	list(APPEND PLATFORM_SHARED_LIBS_RELEASE "lib${LIB_NAME}.2.5.dylib")
	list(APPEND PLATFORM_SHARED_LIBS_RELEASE "lib${LIB_NAME}.2.5.1.dylib")
	list(APPEND PLATFORM_SHARED_LIBS_DEBUG "lib${LIB_NAME}-d.dylib")
	list(APPEND PLATFORM_SHARED_LIBS_DEBUG "lib${LIB_NAME}-d.2.5.dylib")
	list(APPEND PLATFORM_SHARED_LIBS_DEBUG "lib${LIB_NAME}-d.2.5.1.dylib")
endforeach()

set(PLATFORM_FRAMEWORKS_COMMON
	FLAC.framework
	freetype.framework
	ogg.framework
	OpenAL.framework
	vorbis.framework
	vorbisenc.framework
	vorbisfile.framework
)

##################################
# Interface
##################################
function(set_target_platform_libraries)
	if ("${PROJECT_NAME}" STREQUAL "Engine")
		target_link_libraries(${PROJECT_NAME} PUBLIC
			"-framework IOKit"
			"-framework CoreFoundation"
			"-framework Foundation"
		)
	endif()
endfunction()

function(ensure_dependencies_present)
	file(MAKE_DIRECTORY "${BUILD_THIRD_PARTY_PATH}/Lib")
	set(LIB_FILENAMES
		${PLATFORM_STATIC_LIBS_RELEASE}
		${PLATFORM_STATIC_LIBS_DEBUG}
		${PLATFORM_SHARED_LIBS_RELEASE}
		${PLATFORM_SHARED_LIBS_DEBUG}
	)
	ensure_files_present("${BUILD_THIRD_PARTY_PATH}/Lib" "${LIB_FILENAMES}")
	file(MAKE_DIRECTORY "${BUILD_THIRD_PARTY_PATH}/Frameworks")
	ensure_files_present("${BUILD_THIRD_PARTY_PATH}/Frameworks" "${PLATFORM_FRAMEWORKS_COMMON}")
endfunction()

function(install_runtime EXE_NAME)
	cmake_policy(SET CMP0087 NEW)  # Enable generator expressions during `install`
	install_file_list("${PLATFORM_SHARED_LIBS_RELEASE}" "${BUILD_THIRD_PARTY_PATH}/Lib" "${RUNTIME_PATH}/Lib")
	install_directory_list("${PLATFORM_FRAMEWORKS_COMMON}" "${BUILD_THIRD_PARTY_PATH}/Frameworks" "${RUNTIME_PATH}/Frameworks")
	set(SCRIPT "${RUNTIME_PATH}/Utils/create_bundle.sh")
	if(EXISTS "${SCRIPT}")
		install(CODE "execute_process(COMMAND sh \"${SCRIPT}\" ${EXE_NAME})")
	endif()
endfunction()

function(set_target_compile_options)
	target_compile_options(${PROJECT_NAME} PRIVATE
		$<$<OR:$<CONFIG:Debug>,$<CONFIG:Develop>>:
			-O0
		>
		$<$<OR:$<CONFIG:Release>,$<CONFIG:Ship>>:
			-O2
			-Werror
		>
		-g
		-Wextra
		-Werror=return-type
		-fexceptions
	)
endfunction()

function(set_target_link_options)
	# Nothing to do here
endfunction()
