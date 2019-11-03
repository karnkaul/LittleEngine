##################################
# Init
##################################
# Force dynamic ThirdParty libraries
set(SFML_STATIC_LIBS OFF CACHE INTERNAL "")

macro(init)
	set(APL_CLANG 0)
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
		set(APL_CLANG 1)
		foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
			if(NOT EXISTS "${LIBRARIES_PATH}/${CONFIG}")
				file(MAKE_DIRECTORY "${LIBRARIES_PATH}/${CONFIG}")
			endif()
		endforeach()
	else()
		message("\tWARNING: Unsupported compiler [${CMAKE_CXX_COMPILER_ID}], expect build warnings/errors!")
	endif()
	set(CMAKE_INSTALL_RPATH "@executable_path/Lib" PARENT_SCOPE)
	set(BUILD_WITH_INSTALL_RPATH ON PARENT_SCOPE)
endmacro()
init()

##################################
# Interface
##################################
function(set_target_platform_libraries)
	init()
	if ("${PROJECT_NAME}" STREQUAL "Engine")
		target_link_libraries(${PROJECT_NAME} PUBLIC
			"-framework IOKit"
			"-framework CoreFoundation"
			"-framework Foundation"
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
	)
endfunction()

function(set_target_link_options)
	# Nothing to do here
endfunction()
