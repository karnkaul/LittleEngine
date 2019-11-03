##################################
# Init
##################################
# Force dynamic ThirdParty libraries
set(SFML_STATIC_LIBS OFF CACHE INTERNAL "")

set(APL_CLANG 0)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
	set(APL_CLANG 1)
	foreach(CONFIG in ${CMAKE_CONFIGURATION_TYPES})
		file(MAKE_DIRECTORY "${THIRD_PARTY_BUILD_PATH}/Lib/${CONFIG}")
	endforeach()
else()
	message("\tWARNING: Unsupported compiler [${CMAKE_CXX_COMPILER_ID}], expect build warnings/errors!")
endif()

# TODO: Move to LEDemo
set(CMAKE_INSTALL_RPATH "@executable_path/Lib")
set(BUILD_WITH_INSTALL_RPATH ON)

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

function(set_target_compile_options)
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
	)
endfunction()

function(set_target_link_options)
	# Nothing to do here
endfunction()
