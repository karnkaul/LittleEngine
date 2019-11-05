##################################
# Init
##################################
# Force dynamic ThirdParty libraries
set(SFML_STATIC_LIBS OFF CACHE INTERNAL "")

macro(init)
	set(APL_CLANG 0)
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
		set(APL_CLANG 1)
	else()
		message("\tWARNING: Unsupported compiler [${CMAKE_CXX_COMPILER_ID}], expect build warnings/errors!")
	endif()
	set(CMAKE_INSTALL_NAME_DIR @executable_path)
	set(CMAKE_INSTALL_RPATH "@executable_path/Lib")
	set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
endmacro()
init()

set(EXE_SUFFIX "" CACHE INTERNAL "" FORCE)

##################################
# Interface
##################################
function(set_target_platform_libraries TARGET_NAME)
	init()
	if ("${TARGET_NAME}" STREQUAL "LittleEngine")
		target_link_libraries(${TARGET_NAME} PUBLIC
			"-framework IOKit"
			"-framework CoreFoundation"
			"-framework Foundation"
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
	)
endfunction()

function(set_target_link_options TARGET_NAME)
	init()
	# Nothing to do here
endfunction()
